#if defined _CORE_PLAYER_REACH
    #endinput
#endif
#define _CORE_PLAYER_REACH

#include <pp-hooks>

/**
 * # What is within reach
 *
 * The item a character is standing next to, and the prompt that says so. The
 * framework raises an area for every item in the world and tells us when
 * somebody walks into one, so this only has to remember which.
 *
 * A character can stand in two areas at once. Rather than guess which one they
 * meant, the last one entered is the one offered -- walking towards something
 * is the answer to which of them is wanted -- and leaving it falls back to
 * whatever else is still underfoot, which the framework can be asked for.
 */

static
    Item:gPlayerReachItem[MAX_PLAYERS] = { INVALID_ITEM_ID, ... }
;

/**
 * # Functions
 */

/**
 * @brief      The item a character could pick up without moving.
 *
 * Kept up to date by the framework's item areas rather than measured on
 * demand, so asking is a read. When several are in reach it is the
 * nearest, and it is cleared when they walk away from it or it is
 * destroyed under them.
 *
 * @param      playerid  Character to read.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the item within arm's length
 *             - `INVALID_ITEM_ID` when:
 *                 + the character is not connected
 *                 + nothing is in reach
 */
forward Item:GetPlayerReachItem(playerid);

/**
 * # External
 */

stock Item:GetPlayerReachItem(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_ITEM_ID;
    }

    return gPlayerReachItem[playerid];
}

static stock Item:NearestReachItemInternal(playerid) {
    new
        Item:items[MAX_AREA_BUTTONS]
    ;

    if (!GetPlayerNearbyItems(playerid, items)) {
        return INVALID_ITEM_ID;
    }

    return items[0];
}

/**
 * # Calls
 */

hook OnPlayerEnterItemArea(playerid, Item:itemid) {
    gPlayerReachItem[playerid] = itemid;

    UpdatePlayerPrompt(playerid);

    return 0;
}

hook OnPlayerLeaveItemArea(playerid, Item:itemid) {
    if (gPlayerReachItem[playerid] == itemid) {
        gPlayerReachItem[playerid] = NearestReachItemInternal(playerid);
    }

    UpdatePlayerPrompt(playerid);

    return 0;
}

hook OnPlayerPickedUpItem(playerid, Item:itemid) {
    if (gPlayerReachItem[playerid] == itemid) {
        gPlayerReachItem[playerid] = NearestReachItemInternal(playerid);
    }

    UpdatePlayerPrompt(playerid);

    return 0;
}

hook OnItemDestroy(Item:itemid) {
    foreach (new i : Player) {
        if (gPlayerReachItem[i] != itemid) {
            continue;
        }

        gPlayerReachItem[i] = INVALID_ITEM_ID;
        UpdatePlayerPrompt(i);
    }

    return 0;
}

hook OnPlayerConnect(playerid) {
    gPlayerReachItem[playerid] = INVALID_ITEM_ID;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    gPlayerReachItem[playerid] = INVALID_ITEM_ID;

    return 0;
}

hook OnPlayerRequestPrompt(playerid) {
    new const
        Item:itemid = gPlayerReachItem[playerid]
    ;

    if (!IsValidItem(itemid)) {
        return 0;
    }

    if (!IsItemInWorld(itemid)) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (IsItemOpenable(itemid)) {
        OfferPlayerPrompt(playerid, PLAYER_PROMPT_REACH, "Press %s to pick up the %s, or %s to open it.", ITEM_KEY_PICK_UP_NAME, name, ITEM_KEY_OPEN_NAME);
    } else {
        OfferPlayerPrompt(playerid, PLAYER_PROMPT_REACH, "Press %s to pickup the %s.", ITEM_KEY_PICK_UP_NAME, name);
    }

    return 0;
}
