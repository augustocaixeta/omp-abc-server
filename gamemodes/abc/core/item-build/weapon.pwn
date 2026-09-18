#if defined _CORE_ITEM_BUILD_WEAPON
    #endinput
#endif
#define _CORE_ITEM_BUILD_WEAPON

#include <pp-hooks>

/**
 * # Weapons
 *
 * What a kind of item arms with, what it eats, and how much of it the thing
 * holds. A weapon item is never attached to a hand like everything else: it is
 * given, because a character holding a rifle is holding the game's rifle.
 *
 * # The arsenal is the game's
 *
 * A character carries as many weapons as San Andreas lets them, one to a weapon
 * slot, and scrolls through them the way they always have. Nothing here takes
 * that away: drawing a second weapon does not put the first one back, switching
 * is the game's own, and the sawnoff dance -- fire, switch away, switch back to
 * a full clip -- still works, because none of it is being reimplemented.
 *
 * What is added is only this: the rounds a weapon holds come out of an item, go
 * back into an item, and are carried in the pockets as boxes.
 *
 * # One number, because the game owns the other
 *
 * A weapon item holds how many rounds are in it, and a build says how many it
 * can hold at all. That is the whole of it.
 *
 * There is no magazine here, and there should not be: San Andreas already keeps
 * a clip, empties it as the character fires, and refills it from the rest when
 * they reload or switch weapons. Writing a second clip beside the game's would
 * mean two numbers counting the same rounds and disagreeing the moment anybody
 * pulled a trigger.
 *
 * So the game holds the live count while the weapon is drawn, and the item is
 * where it rests. They are read back into each other at the boundaries -- being
 * put away, dropped, opening the pockets, dying, disconnecting -- which is
 * every moment anybody can see either one.
 *
 * # One weapon to a slot
 *
 * San Andreas sorts weapons into twelve slots and keeps one weapon in each, so
 * drawing an AK-47 while carrying an M4 replaces it: both are assault rifles.
 * The game does that silently and without asking, which for an item system is
 * an item that stops existing.
 *
 * So the slot a weapon belongs to is known here, from its id and nothing else,
 * and whatever is in that slot is put back in the pockets before the game gets
 * the chance to throw it away.
 *
 * The replaced weapon keeps its rounds. It goes back into the pockets loaded
 * exactly as it came out, and nothing is turned into boxes by putting a weapon
 * away -- boxes are what UNLOAD makes, and only UNLOAD.
 *
 * # Running out
 *
 * San Andreas takes a weapon off a character when its last round is gone. The
 * item is not lost with it: it goes back into the pockets empty, which is a
 * weapon somebody still owns and cannot currently fire.
 *
 * Drawing an empty one is refused for the same reason. There is nothing to hold
 * and the game would take it straight back, so it is loaded in the pockets
 * first and drawn afterwards.
 *
 * Noticing the moment it happens takes two things, because San Andreas offers
 * neither on its own.
 *
 * Letting go of the trigger is the cheap one and covers somebody who stops
 * firing. It does not cover somebody who holds the trigger down until the
 * magazine is gone -- the key is never released, so nothing is ever asked, and
 * the weapon leaves their hands unnoticed.
 *
 * The last shot itself is the other. There is no OnPlayerWeaponAmmoEmpty, but a
 * shot fired while the game reports WEAPONSTATE_LAST_BULLET with one round left
 * is the last one by definition, and that is a callback's worth of information:
 * OnPlayerEmptyWeapon is declared below so anything else can hear it too.
 *
 * That one only reaches weapons the game reports shots for, which is firearms
 * and not the chainsaw. A tank running dry is caught by the next full sync
 * instead, which is the moment anybody could have noticed anyway.
 *
 * # Where the weapon is
 *
 * Drawing one takes the item out of the pockets, because a rifle on a
 * character's back is not also a rifle in their coat. The slot it came from is
 * free while it is out, and putting it away is the same journey back.
 *
 * The ammunition of a drawn weapon is not hidden by that: the game's own HUD
 * has been showing it all along, which is what the HUD is for. The pockets show
 * what is stored -- spare weapons, boxes of rounds -- and that is the division.
 *
 * # Calibres that are poured
 *
 * A chainsaw takes petrol the way a rifle takes 7.62mm, and core/item/calibre
 * says so. Such a weapon is filled from a vessel rather than from a box, and
 * counted in millilitres so that everything here stays whole numbers.
 */

static
    WEAPON:gItemBuildWeapon[MAX_ITEM_BUILDS],
    Calibre:gItemBuildWeaponCalibre[MAX_ITEM_BUILDS] = { INVALID_CALIBRE_ID, ... },
    gItemBuildWeaponCapacity[MAX_ITEM_BUILDS],

    // How many of the game's own shots one unit of the tank is worth. One, for
    // anything firing rounds: a round is a round. More, for something that
    // atomises what is in it -- a spray can holds half a litre and gets a few
    // thousand puffs out of it, because the game is counting puffs and the can
    // is measured in millilitres.
    //
    // Without it the two have to be the same number, and a can that lasts has
    // to claim to hold three litres.
    gItemBuildWeaponShots[MAX_ITEM_BUILDS] = { 1, ... },

    // Whether the thing is gone once it is empty. A spray can and an
    // extinguisher are the container as much as the contents -- an empty one is
    // rubbish, not a thing waiting to be filled. A chainsaw and a flamethrower
    // are refilled, so an empty one is still a weapon and stays where it is.
    bool:gItemBuildWeaponDisposable[MAX_ITEM_BUILDS]
;

// Which item is in which of the game's weapon slots, held as id + 1 so that a
// zeroed cell reads as an empty slot -- the same trick container.inc uses, and
// for the same reason: Item:0 is a real item, so nothing may be left to mean
// "none" by accident.
//
// The other way round as well, packed as (playerid + 1) << 4 | slot, because
// thirteen slots fit in four bits. It is what makes destroying an item cost one
// read instead of a walk over every slot of every player -- and it is the only
// thing that can answer there at all, since an item is already invalid by the
// time OnItemDestroy runs and can no longer be asked which weapon it was.
static
    gPlayerWeaponSlotItem[MAX_PLAYERS][MAX_WEAPON_SLOTS],
    gItemWeaponHolder[MAX_ITEMS]
;

// What a disconnect copies over a player's row: one block copy rather than
// thirteen writes.
static const
    gWeaponSlotNone[MAX_WEAPON_SLOTS]
;

/**
 * # Functions
 */

/**
 * @brief      Say which weapon a build arms with, and what it eats.
 *
 * Naming no calibre makes it a weapon that is simply armed -- a knife, a
 * bat -- with nothing to load and nothing drawn in the corner of its slot.
 *
 * `capacity` is every round the weapon can hold at once, not a magazine.
 * San Andreas keeps the clip and refills it from this as the character
 * reloads, so a rifle of 150 is one a character can be carrying 150 rounds
 * inside, thirty of which happen to be chambered at any moment.
 *
 * A poured calibre reads `capacity` as the size of the tank in millilitres:
 * a two litre chainsaw is `2000`.
 *
 * @param      buildid    Build to arm.
 * @param      weapon     Weapon it gives.
 * @param      calibreid  What it eats, or nothing for a weapon with no
 *                        ammunition at all.
 * @param      capacity   Every round it can hold, or millilitres for a
 *                        poured calibre.
 * @param      shots      How many of the game's own shots one of those is
 *                        worth. One for rounds; more for something that
 *                        atomises, where the game counts far faster than the
 *                        thing itself empties.
 * @param      disposable Whether an empty one is thrown away rather than
 *                        kept to be refilled.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the build now arms
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + a calibre was named but is not registered
 *                 + a calibre was named and `capacity` is zero or below
 */
forward bool:DefineItemBuildWeapon(ItemBuild:buildid, WEAPON:weapon, Calibre:calibreid = INVALID_CALIBRE_ID, capacity = 0, shots = 1, bool:disposable = false);

/**
 * @brief      Which weapon this kind of item arms with.
 *
 * @param      buildid  Build to read.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the weapon it was defined with
 *             - `WEAPON_FIST` when:
 *                 + `buildid` is not a registered build
 *                 + the build is not a weapon at all
 */
forward WEAPON:GetItemBuildWeapon(ItemBuild:buildid);

/**
 * @brief      What calibre this kind of weapon eats.
 *
 * @param      buildid  Build to read.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - the calibre it was defined with
 *             - `INVALID_CALIBRE_ID` when:
 *                 + `buildid` is not a registered build
 *                 + it takes no ammunition
 */
forward Calibre:GetItemBuildWeaponCalibre(ItemBuild:buildid);

/**
 * @brief      How many rounds this kind of weapon can hold at once.
 *
 * Millilitres, for a weapon whose calibre is poured.
 *
 * @param      buildid  Build to read.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - the capacity it was defined with
 *             - `0` when the build takes no ammunition
 */
forward GetItemBuildWeaponCapacity(ItemBuild:buildid);

/**
 * @brief      How many of the game's shots one unit of a weapon's tank is worth.
 *
 * One for anything firing rounds. More for something that atomises what is
 * in it, where the game's counter runs down far faster than the thing
 * actually empties -- what the game spends is divided by this to find what
 * the tank lost.
 *
 * @param      buildid  Build to read.
 *
 * @date       10:20 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many shots one unit is worth, never below one
 */
forward GetItemBuildWeaponShots(ItemBuild:buildid);

/**
 * @brief      Whether an empty one is thrown away rather than kept.
 *
 * A spray can is the container as much as the contents: emptied, it is
 * rubbish. A chainsaw is refilled, so an empty one is still a chainsaw.
 *
 * @param      buildid  Build to read.
 *
 * @date       12:30 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` an empty one is gone
 *             - `false` an empty one is kept
 */
forward bool:IsItemBuildWeaponDisposable(ItemBuild:buildid);

/**
 * @brief      Whether picking this item up would arm the character.
 *
 * Fists are the answer for everything that is not a weapon, so they are
 * also what says no here: nothing can be defined as arming with fists.
 *
 * @param      itemid  Item to check.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` its build arms with something
 *             - `false` its build arms with nothing
 */
forward bool:IsItemWeapon(Item:itemid);

/**
 * @brief      Whether a weapon is one that takes ammunition.
 *
 * False for a knife, and false for anything that is not a weapon at all.
 *
 * @param      itemid  Item to ask.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it has a calibre and room for it
 *             - `false` when:
 *                 + it is not a weapon
 *                 + it takes no ammunition
 */
forward bool:IsItemWeaponLoadable(Item:itemid);

/**
 * @brief      What calibre a particular weapon eats.
 *
 * @param      itemid  Weapon to read.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - its calibre
 *             - `INVALID_CALIBRE_ID` when it takes no ammunition
 */
forward Calibre:GetItemWeaponCalibre(Item:itemid);

/**
 * @brief      How many rounds are in a weapon.
 *
 * While it is drawn this is the last figure read back from the game rather
 * than the live one, because the game is what is counting. Call
 * SyncPlayerWeaponAmmo first when the difference matters -- opening the
 * pockets already does.
 *
 * @param      itemid  Weapon to read.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - the rounds it holds
 *             - `0` when:
 *                 + it is not a weapon that takes ammunition
 *                 + it is empty
 */
forward GetItemWeaponAmmo(Item:itemid);

/**
 * @brief      Say how many rounds are in a weapon.
 *
 * Clamped to what that kind of weapon can hold, so nothing puts two hundred
 * rounds in a hundred and fifty round rifle by asking.
 *
 * @param      itemid  Weapon to write.
 * @param      rounds  How many are in it.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the count was written
 *             - `false` `itemid` is not a weapon that takes ammunition
 */
forward bool:SetItemWeaponAmmo(Item:itemid, rounds);

/**
 * @brief      Put rounds into a weapon.
 *
 * Takes as much as fits and answers what did not, so nothing is quietly
 * lost. For a shop selling a loaded rifle, or a shipment filling a crate.
 *
 * @param      itemid  Weapon to fill.
 * @param      rounds  How many are being put in.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many rounds would not fit
 *             - `rounds` unchanged when `itemid` takes no ammunition
 */
forward LoadItemWeapon(Item:itemid, rounds);

/**
 * @brief      Pour a box of ammunition into a weapon.
 *
 * Two items and nothing else: no character, no drawing, no game. The
 * calibres have to match and the weapon has to have room, and as much moves
 * as both allow.
 *
 * This is what the click-one-then-the-other gesture runs on, so it works on
 * a weapon sitting in a pocket as readily as on one being carried.
 *
 * A box emptied by this is left empty rather than destroyed, because
 * whether an empty box is thrown away is a gameplay question and this is
 * arithmetic. The callers that are gestures throw it away.
 *
 * @param      weaponid      Weapon to fill.
 * @param      ammunitionid  Box to draw from.
 *
 * @date       22:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many rounds moved
 *             - `0` when:
 *                 + either item is not what it should be
 *                 + the calibres differ
 *                 + the box is empty, or the weapon is full
 */
forward LoadItemWeaponFromAmmunition(Item:weaponid, Item:ammunitionid);

/**
 * @brief      Load a weapon a character is carrying from a box in their
 *             pockets.
 *
 * The gesture on top of the arithmetic. Whichever weapon they have in their
 * hands is filled if it eats that calibre; otherwise the first one they are
 * carrying that does. A box emptied by this is destroyed, because an empty
 * box is nothing.
 *
 * The game is told the new figure straight away, so the HUD moves while the
 * character is still looking at it.
 *
 * @param      playerid      Character loading it.
 * @param      ammunitionid  Box to load from.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many rounds went in
 *             - `0` when:
 *                 + `ammunitionid` is not ammunition, or is empty
 *                 + they are carrying nothing that eats it
 *                 + everything that does is already full
 */
forward LoadPlayerWeaponFromAmmunition(playerid, Item:ammunitionid);

/**
 * @brief      Take every round out of a weapon and back into the pockets.
 *
 * The rounds become boxes again -- topping up partly empty ones of that
 * calibre before making new ones -- so nothing is lost by emptying a rifle,
 * and a weapon at nothing is a weapon that cannot be fired rather than one
 * that has been thrown away.
 *
 * Only as much comes out as the pockets will hold. The rest stays in the
 * weapon, which is the honest answer when there is nowhere to put it.
 *
 * @param      playerid  Character whose pockets take them.
 * @param      weaponid  Weapon to empty.
 *
 * @date       22:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many rounds came out
 *             - `0` when:
 *                 + `weaponid` is not a weapon that takes ammunition
 *                 + its calibre is poured, so there are no boxes to make
 *                 + it is already empty
 *                 + there is no room in the pockets for any of it
 */
forward UnloadPlayerWeapon(playerid, Item:weaponid);

/**
 * @brief      Draw a weapon item.
 *
 * The item leaves the pockets and the character is armed with it. Nothing
 * else they are carrying is put away: San Andreas holds one weapon per
 * slot, so a pistol and a rifle are carried together and switched between
 * like always.
 *
 * A weapon that would take a slot another item is already in puts that one
 * back in the pockets first, because the game would have replaced it
 * anyway -- and an item the game has quietly thrown away is an item lost.
 * The one put away keeps its rounds: nothing here turns ammunition into
 * boxes, which is UNLOAD's job and only UNLOAD's.
 *
 * An empty weapon is refused. The game takes one off a character as soon as
 * its last round is gone, so there is nothing to draw until it has been
 * loaded in the pockets.
 *
 * @param      playerid  Character to arm.
 * @param      itemid    Weapon to arm them with.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they are carrying it
 *             - `false` when:
 *                 + `playerid` is not connected
 *                 + `itemid` is not a weapon
 *                 + it takes ammunition and has none
 *                 + it is not in their pockets to be drawn from
 *                 + something already in that slot has nowhere to go back to
 */
forward bool:EquipPlayerWeapon(playerid, Item:itemid);

/**
 * @brief      Put one weapon away.
 *
 * The live count is read back into the item first, so a rifle put away half
 * empty is half empty when it comes out again, and then the item goes back
 * into the pockets it was drawn from.
 *
 * Pockets with no room are the one thing that stops it. The weapon stays
 * where it is rather than falling on the floor, because dropping somebody's
 * rifle for them because their coat is full is not something they asked
 * for.
 *
 * @param      playerid  Character to disarm.
 * @param      itemid    Weapon to put away.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is back in their pockets
 *             - `false` when:
 *                 + they are not carrying it
 *                 + there is no room in their pockets for it
 */
forward bool:UnequipPlayerWeapon(playerid, Item:itemid);

/**
 * @brief      Put every weapon away.
 *
 * As many as the pockets will take, and the rest stay where they are.
 *
 * @param      playerid  Character to disarm.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many went back into the pockets
 *             - `0` when they were carrying none
 */
forward UnequipPlayerWeapons(playerid);

/**
 * @brief      Whether a character is carrying a particular weapon item.
 *
 * @param      playerid  Player to ask about.
 * @param      itemid    Weapon to look for.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is in one of their weapon slots
 *             - `false` when:
 *                 + they are not connected
 *                 + it is in their pockets, or somewhere else entirely
 */
forward bool:IsPlayerWeaponEquipped(playerid, Item:itemid);

/**
 * @brief      Which of the game's weapon slots an item belongs to.
 *
 * A property of the weapon and of nothing else, so it answers the same
 * whether anybody is carrying one or not. Two items that answer the same
 * slot cannot be carried together: drawing one puts the other away.
 *
 * @param      itemid  Weapon to place.
 *
 * @date       23:30 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - the slot San Andreas files it under
 *             - `WEAPON_SLOT_UNKNOWN` when it is not a weapon
 */
forward WEAPON_SLOT:GetWeaponSlotOfItem(Item:itemid);

/**
 * @brief      Which item is in one of a character's weapon slots.
 *
 * @param      playerid  Player to read.
 * @param      slot      Weapon slot to look in.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - the weapon item in it
 *             - `INVALID_ITEM_ID` when:
 *                 + they are not connected
 *                 + `slot` is out of range
 *                 + the slot is empty
 */
forward Item:GetPlayerWeaponSlotItem(playerid, WEAPON_SLOT:slot);

/**
 * @brief      Which item is the weapon currently in a character's hands.
 *
 * What the game's HUD is showing the ammunition for.
 *
 * @param      playerid  Player to ask about.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - the item they have out
 *             - `INVALID_ITEM_ID` when:
 *                 + they are not connected
 *                 + they are empty handed
 *                 + what they are holding is not one of ours
 */
forward Item:GetPlayerArmedItem(playerid);

/**
 * @brief      The first weapon a character carries that eats a calibre.
 *
 * Whatever is in their hands wins, if it takes that calibre, so loading
 * fills the thing they are actually looking at.
 *
 * Only what they are carrying, and never the pockets. Ten chainsaws in a
 * bag are ten things a character cannot tell apart, so filling one of them
 * by name is filling one at random -- and a can that pours into a pocket is
 * a can that can no longer be poured into a car, or drunk.
 *
 * @param      playerid   Player to search.
 * @param      calibreid  Calibre to match.
 *
 * @date       23:20 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - a weapon of theirs that eats it
 *             - `INVALID_ITEM_ID` when they carry none that does
 */
forward Item:FindPlayerWeaponOfCalibre(playerid, Calibre:calibreid);

/**
 * @brief      Read the live count of the weapon in hand back into its item.
 *
 * The one a character can have just fired, which is why this is what the
 * trigger being released asks for: it costs two calls out to the game
 * rather than one per slot, and nothing they are not holding can have
 * changed.
 *
 * @param      playerid  Character to read.
 *
 * @date       01:10 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the item now says what the gun says
 *             - `false` when:
 *                 + they are not connected
 *                 + they are empty handed
 *                 + what they hold is not one of ours, or takes no ammunition
 *                 + the game had already taken it, in which case it has gone
 *                   back to their pockets empty
 */
forward bool:SyncPlayerArmedWeaponAmmo(playerid);

/**
 * @brief      Read the game's live counts back into the items.
 *
 * The game is what fires the guns, so the game is what knows how much is
 * left. Every slot is read, not only the weapon in hand, because switching
 * away from one does not stop it having been fired.
 *
 * Called at every boundary already, opening the pockets included. Call it
 * directly when something wants the figures current outside one.
 *
 * @param      playerid  Character to read.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many items were brought up to date
 *             - `0` when they are carrying no weapons of ours
 */
forward SyncPlayerWeaponAmmo(playerid);

/**
 * # Events
 */

/**
 * @brief      A character has drawn a weapon item.
 *
 * @param      playerid  Character now carrying it.
 * @param      itemid    Weapon they drew.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerEquipWeapon(playerid, Item:itemid);

/**
 * @brief      A character has put a weapon away.
 *
 * Fired once the item knows what the gun had left in it, so what it reports
 * is what the weapon actually has.
 *
 * @param      playerid  Character who put it away.
 * @param      itemid    Weapon now back in their pockets.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerUnequipWeapon(playerid, Item:itemid);

/**
 * @brief      A character has fired the last round in a weapon.
 *
 * The callback San Andreas does not have. It is worked out from the shot
 * itself: one fired while the game reports WEAPONSTATE_LAST_BULLET with a
 * single round left is the last one, and the weapon is about to leave their
 * hands.
 *
 * Fired before this module puts the item away, so the weapon is still in
 * the character's slot and can be read.
 *
 * Only reaches weapons the game reports shots for. Melee weapons and the
 * chainsaw never fire it.
 *
 * @param      playerid  Character who fired it.
 * @param      weapon    Weapon that is now empty.
 *
 * @date       01:40 12/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerEmptyWeapon(playerid, WEAPON:weapon);

/**
 * @brief      A character has loaded rounds into a weapon.
 *
 * The box may already be gone by the time this is called, an emptied one
 * being thrown away.
 *
 * @param      playerid      Character who loaded it.
 * @param      weaponid      Weapon that was loaded.
 * @param      ammunitionid  Box it came out of, valid only if any is left.
 * @param      rounds        How many went in.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerLoadWeapon(playerid, Item:weaponid, Item:ammunitionid, rounds);

/**
 * # Internal
 */

static stock Item:SlotItemInternal(playerid, WEAPON_SLOT:slot) {
    new const
        value = gPlayerWeaponSlotItem[playerid][slot]
    ;

    return value ? (Item:(value - 1)) : INVALID_ITEM_ID;
}

static stock SetSlotItemInternal(playerid, WEAPON_SLOT:slot, Item:itemid) {
    new const
        Item:previous = SlotItemInternal(playerid, slot)
    ;

    if (previous != INVALID_ITEM_ID) {
        gItemWeaponHolder[previous] = 0;
    }

    gPlayerWeaponSlotItem[playerid][slot] = (itemid == INVALID_ITEM_ID) ? 0 : (_:itemid + 1);

    if (itemid != INVALID_ITEM_ID) {
        gItemWeaponHolder[itemid] = ((playerid + 1) << 4) | _:slot;
    }
}

// The slot an item is in without asking the item anything, so it still answers
// once the item has stopped being valid.
static stock bool:HolderOfItemInternal(Item:itemid, &playerid, &WEAPON_SLOT:slot) {
    new const
        packed = gItemWeaponHolder[itemid]
    ;

    if (packed == 0) {
        return false;
    }

    playerid = (packed >>> 4) - 1;
    slot = WEAPON_SLOT:(packed & 0xF);

    return true;
}

// Which of the game's slots a weapon belongs to. There is no native for it, and
// it cannot be found by asking what the character is carrying: the question is
// asked before the weapon is given, precisely to find out what is about to be
// thrown out of that slot.
//
// So it is the table San Andreas itself uses, written down. It depends on the
// weapon and on nothing else.
static stock WEAPON_SLOT:WeaponSlotInternal(WEAPON:weapon) {
    switch (weapon) {
        case WEAPON_BRASSKNUCKLE: {
            return WEAPON_SLOT_UNARMED;
        }
        case WEAPON_GOLFCLUB .. WEAPON_CHAINSAW: {
            return WEAPON_SLOT_MELEE;
        }
        case WEAPON_COLT45, WEAPON_SILENCED, WEAPON_DEAGLE: {   
            return WEAPON_SLOT_PISTOL;
        }
        case WEAPON_SHOTGUN, WEAPON_SAWEDOFF, WEAPON_SHOTGSPA: {
            return WEAPON_SLOT_SHOTGUN;
        }
        case WEAPON_UZI, WEAPON_MP5, WEAPON_TEC9: {
            return WEAPON_SLOT_MACHINE_GUN;
        }
        case WEAPON_AK47, WEAPON_M4: {
            return WEAPON_SLOT_ASSAULT_RIFLE;
        }
        case WEAPON_RIFLE, WEAPON_SNIPER: {
            return WEAPON_SLOT_LONG_RIFLE;
        }
        case WEAPON_ROCKETLAUNCHER, WEAPON_HEATSEEKER, WEAPON_FLAMETHROWER, WEAPON_MINIGUN: {
            return WEAPON_SLOT_ARTILLERY;
        }
        case WEAPON_GRENADE, WEAPON_TEARGAS, WEAPON_MOLTOV, WEAPON_SATCHEL: {
            return WEAPON_SLOT_EXPLOSIVES;
        }
        case WEAPON_SPRAYCAN, WEAPON_FIREEXTINGUISHER, WEAPON_CAMERA: {
            return WEAPON_SLOT_EQUIPMENT;
        }
        case WEAPON_DILDO .. WEAPON_CANE: {
            return WEAPON_SLOT_GIFT;
        }
        case WEAPON_NIGHT_VISION_GOGGLES, WEAPON_THERMAL_GOGGLES, WEAPON_PARACHUTE: {
            return WEAPON_SLOT_GADGET;
        }
        case WEAPON_BOMB: {
            return WEAPON_SLOT_DETONATOR;
        }
    }

    return WEAPON_SLOT_UNKNOWN;
}

static stock GetSlotAmmoInternal(playerid, WEAPON_SLOT:slot) {
    new
        WEAPON:held,
        ammo
    ;

    if (!GetPlayerWeaponData(playerid, slot, held, ammo)) {
        return -1;
    }

    if (held == WEAPON_FIST) {
        return -1;
    }

    return ammo;
}

// A weapon the game is no longer holding, put back where it came from. The
// slot is cleared before the item moves, so this file's own container hook does
// not read it arriving in the pockets as somebody being disarmed.
//
// Pockets with no room leave it in the slot record rather than destroying it:
// it stays the character's, and the next sync tries again.
// Whether an empty one is worth the slot it would sit in.
//
// It is, when something in the catalogue can fill it again: a rifle with no
// rounds is a rifle somebody will load, and a chainsaw with a dry tank is a
// chainsaw somebody will pour petrol into. Keeping those is the whole reason an
// emptied weapon goes back to the pockets rather than vanishing.
//
// It is not, when nothing can. A molotov that has been thrown is gone, and what
// is left behind is a square reading "0x" that can never be anything else --
// space spent on an item that exists only to be dropped.
//
// Asked of the catalogue rather than declared per weapon, because the answer is
// already written there: GetCalibreAmmunitionBuild knows whether anybody makes
// rounds of that calibre. A liquid one is the exception and answers yes, since
// what fills it is a vessel rather than a box, and core/item-build/fuel decides
// those with the build's own disposable flag.
static stock bool:IsEmptyWeaponWorthKeepingInternal(Item:itemid) {
    new const
        Calibre:calibreid = gItemBuildWeaponCalibre[GetItemBuild(itemid)]
    ;

    if (calibreid == INVALID_CALIBRE_ID) {
        return true;
    }

    if (IsCalibreLiquid(calibreid)) {
        return true;
    }

    return GetCalibreAmmunitionBuild(calibreid) != INVALID_ITEM_BUILD_ID;
}

static stock bool:ReturnPlayerWeaponInternal(playerid, WEAPON_SLOT:slot, Item:itemid) {
    if (IsPlayerInventoryFull(playerid)) {
        return false;
    }

    SetSlotItemInternal(playerid, slot, INVALID_ITEM_ID);

    AddItemToInventory(playerid, itemid);

    CallLocalFunction("OnPlayerUnequipWeapon", "ii", playerid, _:itemid);

    return true;
}

// Give and then set, because GivePlayerWeapon adds rather than assigns: giving
// thirty to a character who already has ten leaves them with forty, and the next
// read back would write that forty into the item. The give is what puts the
// weapon in their hands; the set is what makes the count the one this module
// actually means.
static stock ArmPlayerWeaponInternal(playerid, WEAPON:weapon, ammo) {
    if (ammo < 0) {
        ammo = 0;
    }

    GivePlayerWeapon(playerid, weapon, ammo);
    SetPlayerAmmo(playerid, weapon, ammo);
}

// One slot, read back from the game. The loop above it is what costs, not this:
// GetPlayerWeaponSlotItem asks IsPlayerConnected every time it is called, and a
// native call thirteen times over is the whole of what a sync used to spend.
// Whether the game's ammunition counter is a reading of what this weapon holds.
//
// It is, for anything that fires rounds: the game takes one away per shot and
// that is the truth of it. It is not, for anything running on a liquid -- that
// is spent by how long the trigger was held, or by the game's own consumption,
// and core/item-build/fuel keeps the count. Its number here is whatever the
// weapon was armed with and has not moved since.
//
// Reading it back over the tank is how a half empty chainsaw came back full the
// moment the character scrolled to another weapon.
static stock bool:GameCountsAmmoInternal(Item:itemid) {
    return !IsCalibreLiquid(GetItemBuildWeaponCalibre(GetItemBuild(itemid)));
}

static stock bool:SyncSlotInternal(playerid, WEAPON_SLOT:slot) {
    new const
        Item:itemid = SlotItemInternal(playerid, slot)
    ;

    if (itemid == INVALID_ITEM_ID) {
        return false;
    }

    if (!IsItemWeaponLoadable(itemid)) {
        return false;
    }

    new
        WEAPON:held,
        ammo
    ;

    GetPlayerWeaponData(playerid, slot, held, ammo);

    // The game has stopped holding it, which for a weapon that takes rounds
    // means the last one is gone: it takes them off a character rather than
    // leaving an empty gun in their hands. The item is not lost with it -- it
    // goes back to the pockets empty, and can be loaded and drawn again.
    if (held != gItemBuildWeapon[GetItemBuild(itemid)]) {
        SetItemAmountInt(itemid, 0);

        // Nothing will ever fill it, so there is nothing to put away. The slot
        // is cleared first: this file's own container hook reads an item going
        // into the pockets as somebody being disarmed, and this one is not
        // going anywhere.
        if (!IsEmptyWeaponWorthKeepingInternal(itemid)) {
            new
                name[MAX_ITEM_NAME]
            ;

            GetItemName(itemid, name);

            SetSlotItemInternal(playerid, slot, INVALID_ITEM_ID);

            DestroyItem(itemid);

            CallLocalFunction("OnPlayerUnequipWeapon", "ii", playerid, _:itemid);

            SendPlayerNotice(playerid, "That was your last %s.", name);

            return false;
        }

        ReturnPlayerWeaponInternal(playerid, slot, itemid);

        return false;
    }

    if (!GameCountsAmmoInternal(itemid)) {
        return true;
    }

    SetItemWeaponAmmo(itemid, ammo);

    return true;
}

/**
 * # External
 */

stock bool:DefineItemBuildWeapon(ItemBuild:buildid, WEAPON:weapon, Calibre:calibreid = INVALID_CALIBRE_ID, capacity = 0, shots = 1, bool:disposable = false) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    gItemBuildWeapon[buildid] = weapon;

    if (calibreid == INVALID_CALIBRE_ID) {
        return true;
    }

    if (!IsValidCalibre(calibreid)) {
        return false;
    }

    if (capacity <= 0) {
        return false;
    }

    if (shots < 1) {
        return false;
    }

    gItemBuildWeaponCalibre[buildid] = calibreid;
    gItemBuildWeaponCapacity[buildid] = capacity;
    gItemBuildWeaponShots[buildid] = shots;
    gItemBuildWeaponDisposable[buildid] = disposable;

    SetItemBuildAmountType(buildid, ITEM_AMOUNT_CUSTOM);

    return true;
}

stock WEAPON:GetItemBuildWeapon(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return WEAPON_FIST;
    }

    return gItemBuildWeapon[buildid];
}

stock Calibre:GetItemBuildWeaponCalibre(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return INVALID_CALIBRE_ID;
    }

    return gItemBuildWeaponCalibre[buildid];
}

stock bool:IsItemBuildWeaponDisposable(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    return gItemBuildWeaponDisposable[buildid];
}

stock GetItemBuildWeaponShots(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return 1;
    }

    if (gItemBuildWeaponShots[buildid] < 1) {
        return 1;
    }

    return gItemBuildWeaponShots[buildid];
}

// What the game has to be given so that a tank of this much lasts as long as it
// should. The one place the two counters meet.
static stock GameAmmoInternal(ItemBuild:buildid, amount) {
    return amount * GetItemBuildWeaponShots(buildid);
}

stock GetItemBuildWeaponCapacity(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return 0;
    }

    return gItemBuildWeaponCapacity[buildid];
}

stock bool:IsItemWeapon(Item:itemid) {
    return (GetItemBuildWeapon(GetItemBuild(itemid)) != WEAPON_FIST);
}

stock bool:IsItemWeaponLoadable(Item:itemid) {
    return GetItemBuildWeaponCalibre(GetItemBuild(itemid)) != INVALID_CALIBRE_ID;
}

stock Calibre:GetItemWeaponCalibre(Item:itemid) {
    return GetItemBuildWeaponCalibre(GetItemBuild(itemid));
}

stock GetItemWeaponAmmo(Item:itemid) {
    if (!IsItemWeaponLoadable(itemid)) {
        return 0;
    }

    return GetItemAmountInt(itemid);
}

stock bool:SetItemWeaponAmmo(Item:itemid, rounds) {
    if (!IsItemWeaponLoadable(itemid)) {
        return false;
    }

    new const
        capacity = gItemBuildWeaponCapacity[GetItemBuild(itemid)]
    ;

    if (rounds < 0) {
        rounds = 0;
    }

    if (rounds > capacity) {
        rounds = capacity;
    }

    return SetItemAmountInt(itemid, rounds);
}

stock LoadItemWeapon(Item:itemid, rounds) {
    if (!IsItemWeaponLoadable(itemid)) {
        return rounds;
    }

    if (rounds <= 0) {
        return 0;
    }

    new const
        held = GetItemAmountInt(itemid),
        room = gItemBuildWeaponCapacity[GetItemBuild(itemid)] - held
    ;

    if (room <= 0) {
        return rounds;
    }

    new const
        taken = (rounds < room) ? rounds : room
    ;

    SetItemAmountInt(itemid, held + taken);

    return rounds - taken;
}

stock LoadItemWeaponFromAmmunition(Item:weaponid, Item:ammunitionid) {
    if (!IsItemAmmunition(ammunitionid)) {
        return 0;
    }

    if (!IsItemWeaponLoadable(weaponid)) {
        return 0;
    }

    if (GetItemWeaponCalibre(weaponid) != GetItemAmmunitionCalibre(ammunitionid)) {
        return 0;
    }

    new const
        room = gItemBuildWeaponCapacity[GetItemBuild(weaponid)] - GetItemAmountInt(weaponid)
    ;

    if (room <= 0) {
        return 0;
    }

    new const
        drawn = DrawItemAmmunition(ammunitionid, room)
    ;

    if (drawn == 0) {
        return 0;
    }

    LoadItemWeapon(weaponid, drawn);

    return drawn;
}

stock WEAPON_SLOT:GetWeaponSlotOfItem(Item:itemid) {
    return WeaponSlotInternal(GetItemBuildWeapon(GetItemBuild(itemid)));
}

stock Item:GetPlayerWeaponSlotItem(playerid, WEAPON_SLOT:slot) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_ITEM_ID;
    }

    if (!(0 <= _:slot < _:MAX_WEAPON_SLOTS)) {
        return INVALID_ITEM_ID;
    }

    new const
        Item:itemid = SlotItemInternal(playerid, slot)
    ;

    if (!IsValidItem(itemid)) {
        return INVALID_ITEM_ID;
    }

    return itemid;
}

stock bool:IsPlayerWeaponEquipped(playerid, Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    new
        holder,
        WEAPON_SLOT:slot
    ;

    if (!HolderOfItemInternal(itemid, holder, slot)) {
        return false;
    }

    return holder == playerid;
}

stock Item:GetPlayerArmedItem(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_ITEM_ID;
    }

    new const
        WEAPON:weapon = GetPlayerWeapon(playerid)
    ;

    if (weapon == WEAPON_FIST) {
        return INVALID_ITEM_ID;
    }

    new const
        Item:itemid = SlotItemInternal(playerid, WeaponSlotInternal(weapon))
    ;

    return IsValidItem(itemid) ? itemid : INVALID_ITEM_ID;
}

stock Item:FindPlayerWeaponOfCalibre(playerid, Calibre:calibreid) {
    if (!IsValidCalibre(calibreid)) {
        return INVALID_ITEM_ID;
    }

    new const
        Item:armed = GetPlayerArmedItem(playerid)
    ;

    if (armed != INVALID_ITEM_ID && GetItemWeaponCalibre(armed) == calibreid) {
        return armed;
    }

    for (new WEAPON_SLOT:slot; slot != MAX_WEAPON_SLOTS; ++slot) {
        new const
            Item:itemid = SlotItemInternal(playerid, slot)
        ;

        if (itemid == INVALID_ITEM_ID) {
            continue;
        }

        if (GetItemWeaponCalibre(itemid) != calibreid) {
            continue;
        }

        return itemid;
    }

    return INVALID_ITEM_ID;
}

stock SyncPlayerWeaponAmmo(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return 0;
    }

    new
        count
    ;

    for (new WEAPON_SLOT:slot; slot != MAX_WEAPON_SLOTS; ++slot) {
        if (SyncSlotInternal(playerid, slot)) {
            ++count;
        }
    }

    return count;
}

stock bool:SyncPlayerArmedWeaponAmmo(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    new const
        WEAPON:weapon = GetPlayerWeapon(playerid)
    ;

    if (weapon == WEAPON_FIST) {
        return false;
    }

    return SyncSlotInternal(playerid, WeaponSlotInternal(weapon));
}

stock bool:UnequipPlayerWeapon(playerid, Item:itemid) {
    if (!IsPlayerWeaponEquipped(playerid, itemid)) {
        return false;
    }

    if (IsPlayerInventoryFull(playerid)) {
        return false;
    }

    new const
        WEAPON:weapon = gItemBuildWeapon[GetItemBuild(itemid)],
        WEAPON_SLOT:slot = WeaponSlotInternal(weapon)
    ;

    if (slot != WEAPON_SLOT_UNKNOWN && IsItemWeaponLoadable(itemid) && GameCountsAmmoInternal(itemid)) {
        new const
            ammo = GetSlotAmmoInternal(playerid, slot)
        ;

        if (ammo != -1) {
            SetItemWeaponAmmo(itemid, ammo);
        }
    }

    // Cleared before the item moves, so this file's own container hook does not
    // read the item going back into the pockets as somebody being disarmed.
    SetSlotItemInternal(playerid, slot, INVALID_ITEM_ID);

    RemovePlayerWeapon(playerid, weapon);

    AddItemToInventory(playerid, itemid);

    CallLocalFunction("OnPlayerUnequipWeapon", "ii", playerid, _:itemid);

    return true;
}

stock UnequipPlayerWeapons(playerid) {
    new
        count
    ;

    for (new WEAPON_SLOT:slot; slot != MAX_WEAPON_SLOTS; ++slot) {
        new const
            Item:itemid = SlotItemInternal(playerid, slot)
        ;

        if (itemid == INVALID_ITEM_ID) {
            continue;
        }

        if (UnequipPlayerWeapon(playerid, itemid)) {
            ++count;
        }
    }

    return count;
}

stock bool:EquipPlayerWeapon(playerid, Item:itemid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IsItemWeapon(itemid)) {
        return false;
    }

    new
        index = -1
    ;

    if (!IsItemInInventory(itemid, .index = index)) {
        return false;
    }

    new const
        ItemBuild:buildid = GetItemBuild(itemid),
        WEAPON:weapon = gItemBuildWeapon[buildid]
    ;

    new const
        WEAPON_SLOT:slot = WeaponSlotInternal(weapon)
    ;

    if (slot == WEAPON_SLOT_UNKNOWN) {
        return false;
    }

    new const
        bool:loadable = gItemBuildWeaponCalibre[buildid] != INVALID_CALIBRE_ID
    ;

    // An empty one is refused rather than drawn. The game takes a weapon off a
    // character the moment its last round is gone, so drawing one with nothing
    // in it is handing over something that is given straight back -- and the
    // count it was given is what the HUD then counts down from, which is where
    // a negative figure on the screen comes from.
    //
    // A dry chainsaw is refused for the same reason and one of its own: to the
    // game an empty one is a chainsaw, and it would cut. It is filled where it
    // is, in the pockets, and drawn afterwards.
    if (loadable && GetItemAmountInt(itemid) <= 0) {
        return false;
    }

    // The game keeps one weapon to a slot, so anything already in this one is
    // about to be thrown away by it. That item is put back in the pockets
    // first, or it stops existing anywhere. It keeps whatever is in it.
    new const
        Item:occupant = GetPlayerWeaponSlotItem(playerid, slot)
    ;

    if (occupant != INVALID_ITEM_ID && occupant != itemid && !UnequipPlayerWeapon(playerid, occupant)) {
        return false;
    }

    if (!IsItemInInventory(itemid, .index = index)) {
        return false;
    }

    if (!RemoveItemFromInventory(playerid, index)) {
        return false;
    }

    ArmPlayerWeaponInternal(playerid, weapon, loadable ? GameAmmoInternal(buildid, GetItemAmountInt(itemid)) : 1);
    SetSlotItemInternal(playerid, slot, itemid);

    CallLocalFunction("OnPlayerEquipWeapon", "ii", playerid, _:itemid);

    return true;
}

stock LoadPlayerWeaponFromAmmunition(playerid, Item:ammunitionid) {
    if (!IsItemAmmunition(ammunitionid)) {
        return 0;
    }

    SyncPlayerWeaponAmmo(playerid);

    new const
        Item:weaponid = FindPlayerWeaponOfCalibre(playerid, GetItemAmmunitionCalibre(ammunitionid))
    ;

    if (weaponid == INVALID_ITEM_ID) {
        return 0;
    }

    new const
        drawn = LoadItemWeaponFromAmmunition(weaponid, ammunitionid)
    ;

    if (drawn == 0) {
        return 0;
    }

    if (IsPlayerWeaponEquipped(playerid, weaponid)) {
        SetPlayerAmmo(playerid, gItemBuildWeapon[GetItemBuild(weaponid)], GameAmmoInternal(GetItemBuild(weaponid), GetItemAmountInt(weaponid)));
    }

    new
        Item:remaining = ammunitionid
    ;

    if (GetItemAmmunitionRounds(ammunitionid) <= 0) {
        remaining = INVALID_ITEM_ID;

        new
            Container:containerid = INVALID_CONTAINER_ID,
            index = -1
        ;

        if (GetItemContainer(ammunitionid, containerid, index)) {
            RemoveItemFromContainer(containerid, index, .playerid = playerid);
        }

        DestroyItem(ammunitionid);
    }

    CallLocalFunction("OnPlayerLoadWeapon", "iiii", playerid, _:weaponid, _:remaining, drawn);

    return drawn;
}

stock UnloadPlayerWeapon(playerid, Item:weaponid) {
    if (!IsItemWeaponLoadable(weaponid)) {
        return 0;
    }

    new const
        Calibre:calibreid = GetItemWeaponCalibre(weaponid)
    ;

    if (IsCalibreLiquid(calibreid)) {
        return 0;
    }

    if (IsPlayerWeaponEquipped(playerid, weaponid)) {
        SyncPlayerWeaponAmmo(playerid);
    }

    new const
        held = GetItemAmountInt(weaponid)
    ;

    if (held <= 0) {
        return 0;
    }

    new const
        refused = GivePlayerAmmunition(playerid, calibreid, held),
        moved = held - refused
    ;

    if (moved <= 0) {
        return 0;
    }

    SetItemAmountInt(weaponid, refused);

    if (IsPlayerWeaponEquipped(playerid, weaponid)) {
        SetPlayerAmmo(playerid, gItemBuildWeapon[GetItemBuild(weaponid)], GameAmmoInternal(GetItemBuild(weaponid), refused));
    }

    return moved;
}

/**
 * # Calls
 */

// A weapon is found loaded. Written when the item is made so that every path
// producing one -- the admin, a shipment, a locker being filled -- agrees
// without any of them knowing what a weapon holds.
hook OnItemCreate(Item:itemid) {
    new const
        ItemBuild:buildid = GetItemBuild(itemid)
    ;

    if (gItemBuildWeaponCalibre[buildid] == INVALID_CALIBRE_ID) {
        return 0;
    }

    SetItemAmountInt(itemid, gItemBuildWeaponCapacity[buildid]);

    return 0;
}

hook OnItemDestroy(Item:itemid) {
    new
        holder,
        WEAPON_SLOT:slot
    ;

    if (HolderOfItemInternal(itemid, holder, slot)) {
        SetSlotItemInternal(holder, slot, INVALID_ITEM_ID);
    }

    return 0;
}

// The pockets are about to be looked at, so the figures in them are about to
// matter. Everything being carried is read back from the game first.
hook OnPlayerChangeContainerMenu(playerid, ContainerMenu:menuid, Container:containerid, Container:previous) {
    SyncPlayerWeaponAmmo(playerid);

    return 0;
}

// Dying takes the game's weapons away, so what they had in them is read back
// first. The items stay in the character's slots and are given again on spawn:
// what somebody was carrying is not lost by being killed, which is what the
// gamemode decides rather than what the game does.
hook OnPlayerDeath(playerid, killerid, WEAPON:reason) {
    SyncPlayerWeaponAmmo(playerid);

    return 0;
}

hook OnPlayerSpawn(playerid) {
    for (new WEAPON_SLOT:slot; slot != MAX_WEAPON_SLOTS; ++slot) {
        new const
            Item:itemid = SlotItemInternal(playerid, slot)
        ;

        if (itemid == INVALID_ITEM_ID) {
            continue;
        }

        new const
            ItemBuild:buildid = GetItemBuild(itemid)
        ;

        ArmPlayerWeaponInternal(
            playerid,
            gItemBuildWeapon[buildid],
            gItemBuildWeaponCalibre[buildid] == INVALID_CALIBRE_ID ? 1 : GetItemAmountInt(itemid)
        );
    }

    return 0;
}

// The shot that empties the weapon. WEAPONSTATE_LAST_BULLET with one round left
// is the last one being fired, which is as close to an OnPlayerWeaponAmmoEmpty
// as San Andreas gets -- and it is the only thing that catches somebody holding
// the trigger down, because they never release the key for the hook below to
// hear.
//
// This returns 1 rather than 0, and has to: the callback's own contract is that
// a zero throws the shot away, and under pp-hooks a chain where every hook
// returns zero ends with the callback returning zero. Nothing else in the
// gamemode hooks it, so ending the chain here costs nothing today -- but
// anything added later has to go above this one.
hook OnPlayerWeaponShot(playerid, WEAPON:weaponid, BULLET_HIT_TYPE:hittype, hitid, Float:fX, Float:fY, Float:fZ) {
    if (GetPlayerWeaponState(playerid) != WEAPONSTATE_LAST_BULLET) {
        return 1;
    }

    if (GetPlayerAmmo(playerid) != 1) {
        return 1;
    }

    CallLocalFunction("OnPlayerEmptyWeapon", "ii", playerid, _:weaponid);

    new const
        WEAPON_SLOT:slot = WeaponSlotInternal(weaponid)
    ;

    if (slot == WEAPON_SLOT_UNKNOWN) {
        return 1;
    }

    new const
        Item:itemid = SlotItemInternal(playerid, slot)
    ;

    if (itemid == INVALID_ITEM_ID) {
        return 1;
    }

    SetItemAmountInt(itemid, 0);
    ReturnPlayerWeaponInternal(playerid, slot, itemid);

    return 1;
}

// Letting go of the trigger is the other half, and the cheap one: it covers
// somebody who stops firing with rounds still in the weapon, where the count
// has moved but nothing has run out.
hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if ((oldkeys & KEY_FIRE) && !(newkeys & KEY_FIRE)) {
        SyncPlayerArmedWeaponAmmo(playerid);
    }

    return 0;
}

hook OnPlayerConnect(playerid) {
    gPlayerWeaponSlotItem[playerid] = gWeaponSlotNone;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    SyncPlayerWeaponAmmo(playerid);

    for (new WEAPON_SLOT:slot; slot != MAX_WEAPON_SLOTS; ++slot) {
        new const
            Item:itemid = SlotItemInternal(playerid, slot)
        ;

        if (itemid != INVALID_ITEM_ID) {
            gItemWeaponHolder[itemid] = 0;
        }
    }

    gPlayerWeaponSlotItem[playerid] = gWeaponSlotNone;

    return 0;
}

hook OnItemAmountFormat(Item:itemid, ItemBuild:buildid, output[], size) {
    new const
        Calibre:calibreid = gItemBuildWeaponCalibre[buildid]
    ;

    if (calibreid == INVALID_CALIBRE_ID) {
        return 0;
    }

    new
        calibre[MAX_CALIBRE_NAME]
    ;

    GetCalibreName(calibreid, calibre);

    if (IsCalibreLiquid(calibreid)) {
        new
            volume[MAX_ITEM_AMOUNT_LENGTH]
        ;

        VolumeFormatShort(float(GetItemAmountInt(itemid)) / 1000.0, volume);
        format(output, size, ITEM_AMMUNITION_VOLUME_FORMAT, volume, calibre);

        return 1;
    }

    format(output, size, ITEM_AMMUNITION_FORMAT, GetItemAmountInt(itemid), calibre);

    return 1;
}
