#if defined _CORE_PLAYER_INTERACT
    #endinput
#endif
#define _CORE_PLAYER_INTERACT

#include <pp-hooks>

/**
 * # Opening what is in reach
 *
 * Two keys, two answers, and only ever about items -- a locker is not an item
 * and is answered in core/world/locker.
 *
 * ITEM_KEY_PICK_UP_ITEM lifts whatever is in front of the character, which the
 * framework already does on its own. ITEM_KEY_OPEN_ITEM opens it where it lies,
 * or opens what is being carried when there is nothing in front of them.
 * Nothing is timed and nothing is held, because there is no longer a question
 * to answer: the two things a character might want are two things they press.
 *
 * That is also what gives the hold back to what a hold is for. Eating, drinking
 * and bandaging are actions that take a moment and can be given up halfway
 * through; opening a crate is neither.
 *
 * The open key is read off the key state rather than through the item's button,
 * because a button belongs to one key and that key is the pick-up one. What is
 * in reach is core/player/reach's answer, and it is the same answer the prompt
 * is written from -- so what the screen offers and what the key does cannot
 * drift apart.
 *
 * What the character is standing at wins over what they are carrying, which is
 * the same order the prompt uses -- PLAYER_PROMPT_REACH outranks
 * PLAYER_PROMPT_HELD, so the screen was already saying this while the key did
 * the opposite.
 *
 * It is also the only order that lets one container be put inside another.
 * Carrying a bag and standing at a crate, the crate is what wants opening: the
 * bag can be opened by walking away from everything, and the crate cannot be
 * opened by anything else at all.
 */

/**
 * # Functions
 */

/**
 * @brief      Open an item where it lies, without picking it up.
 *
 * @param      playerid  Character opening it.
 * @param      itemid    Item to open.
 *
 * @date       00:20 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` its inside is on their screen
 *             - `false` when:
 *                 + the item cannot be opened
 *                 + its container could not be built
 *                 + the menu refused to show it
 */
forward bool:OpenItem(playerid, Item:itemid);

/**
 * @brief      Kneel for something low, stay standing for something high.
 *
 * Only for something lying in the world. An item in the character's own
 * hands is already at chest height and there is nothing to stoop for.
 *
 * The animation is the bomb planting crouch the framework already uses for
 * putting an item down, because a character opening a crate at their feet is
 * doing the same thing with their body.
 *
 * Two things stop the crouch. Height, measured off the floor the character is
 * standing on rather than off the world, so the same crate is low whether it is
 * at the top of a hill or the bottom. And size, because something big has a lid
 * at waist height wherever it is sitting.
 *
 * @param      playerid  Character reaching.
 * @param      z         Height of what they are reaching for.
 * @param      size      How big it is, from the item's build.
 *
 * @date       13:20 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they knelt for it
 *             - `false` they reached it standing, and nothing was played
 */
forward bool:ReachForItem(playerid, Float:z, size = ITEM_BUILD_DEFAULT_SIZE);

/**
 * # External
 */

stock bool:ReachForItem(playerid, Float:z, size = ITEM_BUILD_DEFAULT_SIZE) {
    if (size > ITEM_CROUCH_REACH_SIZE) {
        return false;
    }

    new
        Float:playerX,
        Float:playerY,
        Float:playerZ
    ;

    GetPlayerPos(playerid, playerX, playerY, playerZ);

    if ((z - (playerZ - ITEM_FLOOR_OFFSET)) > ITEM_CROUCH_REACH_HEIGHT) {
        return false;
    }

    ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_2IDLE", 4.0, false, false, false, false, 0);

    return true;
}

stock bool:OpenItem(playerid, Item:itemid) {
    if (!IsItemOpenable(itemid)) {
        return false;
    }

    new const
        Container:containerid = GetItemInside(itemid)
    ;

    if (containerid == INVALID_CONTAINER_ID) {
        return false;
    }

    if (!ShowPlayerContainer(playerid, containerid)) {
        return false;
    }

    if (IsItemInWorld(itemid)) {
        new
            Float:itemX,
            Float:itemY,
            Float:itemZ
        ;

        GetItemPos(itemid, itemX, itemY, itemZ);
        ReachForItem(playerid, itemZ, GetItemBuildSize(GetItemBuild(itemid)));
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);
    SendPlayerNotice(playerid, "You open the %s. %i of %i used.", name, GetContainerSize(containerid), GetContainerCapacity(containerid));

    return true;
}

/**
 * # Calls
 */

hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if (!(newkeys & ITEM_KEY_OPEN_ITEM)) {
        return 0;
    }

    if (IsPlayerInAnyVehicle(playerid)) {
        return 0;
    }

    new const
        Item:reach = GetPlayerReachItem(playerid)
    ;

    if (IsItemOpenable(reach)) {
        OpenItem(playerid, reach);

        return 0;
    }

    OpenItem(playerid, GetPlayerHoldItem(playerid));

    return 0;
}
