#if defined _CORE_ITEM_PILLS
    #endinput
#endif
#define _CORE_ITEM_PILLS

#include <pp-hooks>

/**
 * # Taking something out of a bottle
 *
 * A bottle of pills is not eaten and not drunk: it is opened, one is swallowed,
 * and the bottle goes back in the pocket with fewer in it. So it is neither
 * food nor a vessel -- it is a count, and the count is the item.
 *
 * The count lives in the amount rather than in extra data, because unlike a
 * burger's mouthfuls there is nothing else competing for it: a bottle of pills
 * has no freshness. That also puts the number in the corner of the slot for
 * free, which is what a bottle of pills is mostly asked about.
 *
 * One pill is a short hold. Short enough that the bar is barely a bar, but it
 * is the same gesture as everything else a character does with their hands, and
 * a swallow that could be interrupted is more honest than one that could not.
 */

static
    gItemBuildPillCount[MAX_ITEM_BUILDS],
    Float:gItemBuildPillStrength[MAX_ITEM_BUILDS],

    Item:gPlayerTakingPill[MAX_PLAYERS] = { INVALID_ITEM_ID, ... }
;

/**
 * # Functions
 */

/**
 * @brief      Say that a kind of item is a bottle of pills.
 *
 * @param      buildid   Build to fill.
 * @param      count     How many are in a full bottle.
 * @param      strength  What one is worth, for whoever is counting.
 *
 * The count becomes the item's amount, so the corner of the slot shows how
 * many are left without anybody drawing anything.
 *
 * @date       19:20 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the build is pills
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + `count` is zero or below
 */
forward bool:DefineItemBuildPills(ItemBuild:buildid, count = 1, Float:strength = 0.0);

/**
 * @brief      Whether an item is a bottle of pills.
 *
 * @param      itemid  Item to check.
 *
 * @date       19:20 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` its build was made a bottle
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + its build is not pills
 */
forward bool:IsItemPills(Item:itemid);

/**
 * @brief      How many are left in this particular bottle.
 *
 * A bottle nobody has opened answers its build's full count rather than
 * nothing, so one just made and one never touched read the same.
 *
 * @param      itemid  Bottle to count.
 *
 * @date       19:20 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many are left
 *             - `0` when it is not pills, or is empty
 */
forward GetItemPillCount(Item:itemid);

/**
 * @brief      Swallow one, if the character is holding the bottle.
 *
 * @param      playerid  Character taking it.
 * @param      itemid    Bottle in their hands.
 *
 * @date       19:20 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they have started on it
 *             - `false` when:
 *                 + the item is not pills, or the bottle is empty
 *                 + it is not the item in their hands
 *                 + they are busy with something that outranks it
 */
forward bool:TakePill(playerid, Item:itemid);

/**
 * # Events
 */

/**
 * @brief      A character has swallowed one.
 *
 * Where whatever pills are for reads the strength. The bottle may already be
 * gone by the time this is called, an empty one being thrown away.
 *
 * @param      playerid   Character who took it.
 * @param      itemid     The bottle, valid only if any are left.
 * @param      strength   What that one was worth.
 * @param      remaining  How many are left after it.
 *
 * @date       19:20 08/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerTakePill(playerid, Item:itemid, Float:strength, remaining);

/**
 * # External
 */

stock bool:DefineItemBuildPills(ItemBuild:buildid, count = 1, Float:strength = 0.0) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    if (count <= 0) {
        return false;
    }

    gItemBuildPillCount[buildid] = count;
    gItemBuildPillStrength[buildid] = strength;

    SetItemBuildAmountType(buildid, ITEM_AMOUNT_INT, " left");

    return true;
}

stock bool:IsItemPills(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    return (gItemBuildPillCount[GetItemBuild(itemid)] != 0);
}

stock GetItemPillCount(Item:itemid) {
    if (!IsItemPills(itemid)) {
        return 0;
    }

    if (!HasItemAmountValue(itemid)) {
        return gItemBuildPillCount[GetItemBuild(itemid)];
    }

    return GetItemAmountInt(itemid);
}

stock bool:TakePill(playerid, Item:itemid) {
    if (!IsItemPills(itemid)) {
        return false;
    }

    if (GetPlayerHoldItem(playerid) != itemid) {
        return false;
    }

    if (GetItemPillCount(itemid) <= 0) {
        return false;
    }

    if (!StartHoldAction(playerid, ITEM_PILL_TIME, "Taking", ITEM_KEY_USE_ITEM, HOLD_PRIORITY_PILL)) {
        return false;
    }

    gPlayerTakingPill[playerid] = itemid;

    ApplyAnimation(playerid, "VENDING", "VEND_DRINK_P", ITEM_DRINK_ANIMATION_SPEED, false, false, false, false, 0);

    return true;
}

/**
 * # Calls
 */

// A bottle comes full. Written when the item is made rather than left to
// GetItemPillCount's fallback, because the fallback answers a question and does
// not fill the cell -- and it is the cell the slot draws from, so a bottle
// nobody had opened showed nothing in the corner.
hook OnItemCreate(Item:itemid) {
    new const
        count = gItemBuildPillCount[GetItemBuild(itemid)]
    ;

    if (count == 0) {
        return 0;
    }

    SetItemAmountInt(itemid, count);

    return 0;
}

hook OnPlayerUseItem(playerid, Item:itemid) {
    if (!IsItemPills(itemid)) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (GetItemPillCount(itemid) <= 0) {
        SendPlayerNotice(playerid, "The %s is empty.", name);

        return 1;
    }

    if (!TakePill(playerid, itemid)) {
        new
            busy[MAX_HOLD_ACTION_TITLE]
        ;

        if (GetPlayerHoldActionTitle(playerid, busy)) {
            SendPlayerNotice(playerid, "You are busy: %s.", busy);
        }
    }

    return 1;
}

hook OnHoldActionFinish(playerid) {
    new const
        Item:itemid = gPlayerTakingPill[playerid]
    ;

    if (!IsValidItem(itemid)) {
        return 0;
    }

    gPlayerTakingPill[playerid] = INVALID_ITEM_ID;

    ClearAnimations(playerid, SYNC_NONE);

    new const
        remaining = GetItemPillCount(itemid) - 1
    ;

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);
    SetItemAmountInt(itemid, remaining);
    
    CallLocalFunction("OnPlayerTakePill", "iifi", playerid, _:itemid, _:gItemBuildPillStrength[GetItemBuild(itemid)], remaining);

    if (remaining > 0) {
        SendPlayerNotice(playerid, "You take one. %i left in the %s.", remaining, name);

        return 1;
    }

    RemoveCurrentItem(playerid);
    DestroyItem(itemid);

    SendPlayerNotice(playerid, "You take the last one and drop the %s.", name);

    return 1;
}

hook OnHoldActionStop(playerid, progress) {
    if (!IsValidItem(gPlayerTakingPill[playerid])) {
        return 0;
    }

    gPlayerTakingPill[playerid] = INVALID_ITEM_ID;
    ClearAnimations(playerid, SYNC_NONE);

    return 0;
}

hook OnPlayerDropItem(playerid, Item:itemid) {
    if (gPlayerTakingPill[playerid] == itemid) {
        StopHoldAction(playerid);
    }

    return 0;
}

hook OnItemDestroy(Item:itemid) {
    foreach (new i : Player) {
        if (gPlayerTakingPill[i] == itemid) {
            gPlayerTakingPill[i] = INVALID_ITEM_ID;
        }
    }

    return 0;
}

hook OnPlayerConnect(playerid) {
    gPlayerTakingPill[playerid] = INVALID_ITEM_ID;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    gPlayerTakingPill[playerid] = INVALID_ITEM_ID;

    return 0;
}
