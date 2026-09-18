#if defined _CORE_ITEM_BUILD_FUEL
    #endinput
#endif
#define _CORE_ITEM_BUILD_FUEL

#include <pp-hooks>

/**
 * # A weapon that burns what is in it
 *
 * A chainsaw does not fire rounds, it runs. Hold the trigger and the tank goes
 * down; let go and it stops where it is. That is true of every weapon here
 * whose calibre is a liquid -- the chainsaw and the flamethrower on gasoline,
 * the spray can on paint, the extinguisher on foam -- so it is written once
 * against the rule and not four times against the four of them.
 *
 * Which is the whole reason a liquid can be a calibre. The question "does this
 * run out while the trigger is down" has one answer for all of them, and it is
 * `IsCalibreLiquid`.
 *
 * # Two kinds of running out
 *
 * The four are not alike underneath. A flamethrower and an extinguisher spend
 * ammunition the game is already counting, a round at a time; a chainsaw spends
 * nothing at all, because to the game it is a melee weapon that happens to have
 * a number attached.
 *
 * So the tank is read from whichever is actually moving. Every tick asks the
 * game what it has left: if that fell, the game is the meter and the fall is
 * the burn. If it did not move, nothing is counting it and the time the weapon
 * was being swung is the burn instead.
 *
 * Deciding this per weapon out of a table would be the same answer written down
 * twice, and wrong the moment a weapon joins one list and not the other.
 * Watching the number that matters cannot go out of step with itself.
 *
 * # The gauge
 *
 * Up for as long as the weapon is out, and not only while the trigger is down.
 * It is a fuel gauge: a thing a character glances at to decide whether to start
 * at all, which a bar appearing only once they have committed cannot be.
 *
 * It moves when the weapon is used and at no other time. Pulling a trigger is
 * not using a thing -- a chainsaw revs, a flamethrower can be held at somebody
 * without a drop leaving it -- and neither is falling down a hill with a finger
 * still on the button.
 *
 * That is also why it is a gauge and not a clock. Half a tank is half a bar
 * because the bar is told where the tank is, rather than counting seconds and
 * hoping the two agree.
 *
 * It outranks nothing, so eating, drinking and changing clothes all take the
 * bar off it, and it is offered the bar back when they are done. Scrolling to
 * another weapon hides it without taking it down: nothing else would notice
 * them scrolling back, so the tick stays running and watches for it.
 *
 * # Filling one
 *
 * In the pockets, by dropping a can onto it, exactly as a box of rounds is
 * dropped onto a rifle -- core/ui/item-transfer, one gesture for both.
 *
 * # Empty
 *
 * Back in the pockets. A dry chainsaw left in the hands is a chainsaw that
 * still cuts: nothing about the game's idea of it has changed, so saying it is
 * dry and leaving it there says nothing at all. Drawing an empty one is refused
 * for the same reason, which is core/item-build/weapon's.
 */

static
    // The weapon the gauge is reading. Set only while the gauge is the action
    // holding the bar, so it doubles as "the bar is ours".
    Item:gPlayerFuelItem[MAX_PLAYERS] = { INVALID_ITEM_ID, ... },

    // The game's own counter as it was last tick, and `-1` for no reading yet.
    // Only the weapon actually in the character's hands has one: scrolling to a
    // pistol makes that counter somebody else's number, so the reading is
    // dropped and taken again on the way back.
    gPlayerFuelAmmo[MAX_PLAYERS] = { -1, ... },

    // Shots the game has spent that have not yet come to a whole unit of the
    // tank. A spray can gets several puffs out of a millilitre, so most ticks
    // take nothing off it and the remainder waits rather than rounding away.
    gPlayerFuelSpent[MAX_PLAYERS],

    // Whether the bar is on the screen, so that a tick which changes nothing
    // sends nothing. Ten redraws a second for as long as a chainsaw is out is
    // what this is here to not do.
    bool:gPlayerFuelShown[MAX_PLAYERS],

    // Whether the gauge is in the middle of being moved. Moving it means taking
    // the bar down, taking it down is heard as a stop, and a stop is one of the
    // things that asks for it to be moved -- so it would ask itself.
    bool:gPlayerFuelMoving[MAX_PLAYERS]
;

/**
 * # Functions
 */

/**
 * @brief      Pour a liquid container into a weapon that runs on it.
 *
 * The same act as loading a box of rounds into a rifle, and it answers the
 * same way: as much goes in as there is room for, and what will not fit
 * stays in the can. A can emptied by it is the caller's to throw away.
 *
 * The liquid has to be the one the weapon runs on. Filling a chainsaw with
 * water is not an error to be reported, it is two things that have nothing
 * to do with each other.
 *
 * `limit` pours at most that much, for filling a tank a little at a time
 * while somebody stands there holding a key. Nothing, or below it, pours
 * everything that will fit.
 *
 * @param      weaponid     Weapon to fill.
 * @param      containerid  Can to pour from.
 * @param      limit        Most to pour, in millilitres. `0` for all of it.
 *
 * @date       19:40 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many millilitres went in
 *             - `0` when:
 *                 + either is not a live item
 *                 + the weapon does not run on a liquid
 *                 + the can does not hold the one it runs on
 *                 + the weapon is already full, or the can is empty
 */
forward LoadItemWeaponFromLiquid(Item:weaponid, Item:containerid, limit = 0);

/**
 * # Internal
 */

static stock bool:IsFuelWeaponInternal(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    if (!IsCalibreLiquid(GetItemWeaponCalibre(itemid))) {
        return false;
    }

    return GetItemBuildWeaponCapacity(GetItemBuild(itemid)) > 0;
}

// Which one the character has out. The one in their hands first, so somebody
// carrying a chainsaw and a flamethrower is shown the one they are looking
// down -- and otherwise whichever of them is drawn, because a gauge that went
// out every time they glanced at their pistol would flicker rather than read.
static stock Item:PlayerFuelWeaponInternal(playerid) {
    new const
        Item:armed = GetPlayerArmedItem(playerid)
    ;

    if (IsFuelWeaponInternal(armed)) {
        return armed;
    }

    for (new WEAPON_SLOT:slot; slot != MAX_WEAPON_SLOTS; ++slot) {
        new const
            Item:itemid = GetPlayerWeaponSlotItem(playerid, slot)
        ;

        if (IsFuelWeaponInternal(itemid)) {
            return itemid;
        }
    }

    return INVALID_ITEM_ID;
}

static stock ClearFuelGaugeInternal(playerid) {
    gPlayerFuelItem[playerid] = INVALID_ITEM_ID;
    gPlayerFuelAmmo[playerid] = -1;
    gPlayerFuelSpent[playerid] = 0;
    gPlayerFuelShown[playerid] = false;
}

static stock ResetFuelGaugeInternal(playerid) {
    ClearFuelGaugeInternal(playerid);

    gPlayerFuelMoving[playerid] = false;
}

static stock ShowFuelGaugeInternal(playerid, bool:shown) {
    if (gPlayerFuelShown[playerid] == shown) {
        return;
    }

    gPlayerFuelShown[playerid] = shown;

    SetHoldActionShown(playerid, shown);
}

static stock MoveFuelGaugeInternal(playerid) {
    new const
        Item:itemid = PlayerFuelWeaponInternal(playerid)
    ;

    if (gPlayerFuelItem[playerid] == itemid) {
        return;
    }

    // Reading the wrong weapon, or one that is not out any more. Stopping it
    // clears the state, through the hook at the bottom of this file.
    if (gPlayerFuelItem[playerid] != INVALID_ITEM_ID) {
        StopHoldAction(playerid);
    }

    if (itemid == INVALID_ITEM_ID) {
        return;
    }

    new
        title[MAX_LIQUID_NAME]
    ;

    GetLiquidName(GetCalibreLiquid(GetItemWeaponCalibre(itemid)), title);

    new const
        capacity = GetItemBuildWeaponCapacity(GetItemBuild(itemid))
    ;

    // No key. It is not a hold, it is a reading, and it comes down when the
    // weapon is put away rather than when a finger comes off something.
    // Draining, because a tank going down should look like a tank going down.
    if (!StartHoldAction(playerid, capacity, title, KEY_NONE, HOLD_PRIORITY_FUEL, true, true)) {
        return;
    }

    gPlayerFuelItem[playerid] = itemid;
    gPlayerFuelAmmo[playerid] = -1;
    gPlayerFuelSpent[playerid] = 0;
    gPlayerFuelShown[playerid] = true;

    SetHoldActionProgress(playerid, capacity - GetItemWeaponAmmo(itemid));

    ShowFuelGaugeInternal(playerid, GetPlayerArmedItem(playerid) == itemid);
}

// The gauge follows what the character has out, and this is the only thing that
// puts it up or takes it down. Called on every equip and unequip, and again
// after anything else has finished with the bar: eating outranks a gauge and
// takes it off the screen, and the chainsaw is still in their hands afterwards.
static stock RefreshFuelGaugeInternal(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return;
    }

    // Not from inside itself, which is where the stop it causes comes back
    // from. One guard here rather than a check at each of the four callers.
    if (gPlayerFuelMoving[playerid]) {
        return;
    }

    gPlayerFuelMoving[playerid] = true;

    MoveFuelGaugeInternal(playerid);

    gPlayerFuelMoving[playerid] = false;
}

static stock bool:IsUsingWeaponInternal(playerid, WEAPON:weapon) {
    new
        library[32],
        name[32]
    ;

    return (weapon == WEAPON_CHAINSAW)
        && GetAnimationName(GetPlayerAnimationIndex(playerid), library, sizeof (library), name, sizeof (name))
        && strequal(library, ITEM_CHAINSAW_ANIMATION_LIBRARY, true);
}

static stock RunDryInternal(playerid, Item:itemid) {
    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);
    SetItemWeaponAmmo(itemid, 0);

    // The container as much as the contents: an empty one is rubbish rather
    // than a thing waiting to be filled, so it goes rather than being carried
    // around for the rest of the character's life.
    if (IsItemBuildWeaponDisposable(GetItemBuild(itemid))) {
        // Zero takes it out of the character's hands, and destroying it is
        // what clears the slot behind it -- core/item-build/weapon listens for
        // that, so there is nothing to unhook here.
        if (IsPlayerWeaponEquipped(playerid, itemid)) {
            SetPlayerAmmo(playerid, GetItemBuildWeapon(GetItemBuild(itemid)), 0);
        }

        DestroyItem(itemid);

        SendPlayerNotice(playerid, "The %s is empty, and no use to anybody.", name);

        return;
    }

    if (!IsPlayerWeaponEquipped(playerid, itemid)) {
        SendPlayerNotice(playerid, "The %s is dry.", name);

        return;
    }

    // Away, rather than left in the hands. To the game an empty chainsaw is a
    // chainsaw and it would go on cutting, so being told it is dry while
    // holding one that still works is being told nothing.
    if (!UnequipPlayerWeapon(playerid, itemid)) {
        SendPlayerNotice(playerid, "The %s is dry, and there is nowhere to put it.", name);

        return;
    }

    SendPlayerNotice(playerid, "The %s is dry.", name);
}

/**
 * # External
 */

stock LoadItemWeaponFromLiquid(Item:weaponid, Item:containerid, limit = 0) {
    if (!IsValidItem(weaponid)) {
        return 0;
    }

    if (!IsItemLiquidContainer(containerid)) {
        return 0;
    }

    new const
        Calibre:calibreid = GetItemWeaponCalibre(weaponid)
    ;

    if (!IsCalibreLiquid(calibreid)) {
        return 0;
    }

    if (GetCalibreLiquid(calibreid) != GetItemLiquid(containerid)) {
        return 0;
    }

    new const
        capacity = GetItemBuildWeaponCapacity(GetItemBuild(weaponid)),
        held = GetItemWeaponAmmo(weaponid),

        available = GetItemLiquidAmount(containerid)
    ;

    new
        poured = capacity - held
    ;

    if (poured > available) {
        poured = available;
    }

    if (limit > 0 && poured > limit) {
        poured = limit;
    }

    if (poured <= 0) {
        return 0;
    }

    SetItemWeaponAmmo(weaponid, held + poured);
    DrawItemLiquid(containerid, poured);

    return poured;
}

/**
 * # Calls
 */

hook OnPlayerEquipWeapon(playerid, Item:itemid) {
    RefreshFuelGaugeInternal(playerid);

    return 0;
}

hook OnPlayerUnequipWeapon(playerid, Item:itemid) {
    RefreshFuelGaugeInternal(playerid);

    return 0;
}

hook OnHoldActionUpdate(playerid, progress) {
    new const
        Item:itemid = gPlayerFuelItem[playerid]
    ;

    if (itemid == INVALID_ITEM_ID) {
        return 0;
    }

    // Dropped, given away or destroyed while it was out.
    if (!IsValidItem(itemid)) {
        ClearFuelGaugeInternal(playerid);
        StopHoldAction(playerid);

        return 0;
    }

    new const
        Item:armed = GetPlayerArmedItem(playerid)
    ;

    // Scrolled away from. The gauge keeps running because nothing else would
    // notice them scrolling back to it, but there is nothing to read: the
    // game's counter belongs to whatever is in their hands now.
    if (armed != itemid) {
        gPlayerFuelAmmo[playerid] = -1;

        ShowFuelGaugeInternal(playerid, false);

        // Scrolled to another one of these rather than to a pistol, so the
        // gauge is reading the wrong tank. Moved from inside the bar's own
        // tick, which is the same thing running dry does a few lines down.
        if (IsFuelWeaponInternal(armed)) {
            RefreshFuelGaugeInternal(playerid);
        }

        return 0;
    }

    ShowFuelGaugeInternal(playerid, true);

    new const
        ammo = GetPlayerAmmo(playerid)
    ;

    new const
        spent = (gPlayerFuelAmmo[playerid] == -1) ? 0 : gPlayerFuelAmmo[playerid] - ammo
    ;

    gPlayerFuelAmmo[playerid] = ammo;

    new
        fuel = GetItemWeaponAmmo(itemid)
    ;

    new const
        before = fuel
    ;

    // Whichever is actually moving. The game spends a flamethrower's tank for
    // us and leaves a chainsaw's alone, and this asks rather than assumes.
    //
    // Time is only the meter while the character is actually using the thing.
    // The trigger being down is not that: falling, drowning and being shot all
    // leave a finger on it, and none of them are sawing.
    if (spent > 0) {
        // The game counts its own shots, and one unit of the tank may be worth
        // several of them. What is left over stays left over until it comes to
        // a whole one -- a can getting six puffs to the millilitre would
        // otherwise lose a millilitre on every puff.
        gPlayerFuelSpent[playerid] += spent;

        new const
            shots = GetItemBuildWeaponShots(GetItemBuild(itemid)),
            used = gPlayerFuelSpent[playerid] / shots
        ;

        gPlayerFuelSpent[playerid] -= used * shots;

        fuel -= used;
    } else if (IsUsingWeaponInternal(playerid, GetItemBuildWeapon(GetItemBuild(itemid)))) {
        fuel -= ITEM_FUEL_BURN_RATE * HOLD_ACTION_STEP / 1000;
    }

    if (fuel < 0) {
        fuel = 0;
    }

    // Nothing came out of it this tick, which is most ticks: the gauge is
    // already showing this number and the tank is already holding it.
    if (fuel == before) {
        return 0;
    }

    SetItemWeaponAmmo(itemid, fuel);
    SetHoldActionProgress(playerid, GetItemBuildWeaponCapacity(GetItemBuild(itemid)) - fuel);

    if (fuel > 0) {
        return 0;
    }

    // Put away first and the bar taken down after. The other way round, the
    // bar being freed is heard while the weapon is still in their hands, and
    // the gauge goes straight back up on an empty tank to be taken down again
    // half a line later.
    ClearFuelGaugeInternal(playerid);
    RunDryInternal(playerid, itemid);
    StopHoldAction(playerid);

    return 0;
}

hook OnHoldActionStop(playerid, progress) {
    ClearFuelGaugeInternal(playerid);

    return 0;
}

hook OnHoldActionFinish(playerid) {
    ClearFuelGaugeInternal(playerid);

    return 0;
}

hook OnHoldActionRelease(playerid) {
    RefreshFuelGaugeInternal(playerid);

    return 0;
}

hook OnPlayerConnect(playerid) {
    ResetFuelGaugeInternal(playerid);

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    ResetFuelGaugeInternal(playerid);

    return 0;
}
