#if defined _CORE_WORLD_LOCKER
    #endinput
#endif
#define _CORE_WORLD_LOCKER

#include <pp-hooks>

/**
 * # Fixtures with an inside
 *
 * A locker is not an item. It is an object, a container and a button standing
 * somewhere -- a wardrobe, a filing cabinet, a bin on a street corner -- and
 * none of it can be lifted. That is the whole of the difference: an item is
 * picked up and may happen to have an inside, a locker has an inside and is
 * never picked up.
 *
 * So the key has only one answer here, and it is opening -- the same
 * ITEM_KEY_OPEN_ITEM that opens a crate on the ground, so what the character
 * learns once works on both.
 *
 * It is read off the key state rather than through the locker's button, because
 * a button belongs to one key and the framework gives it the pick-up one. What
 * that key would lift here is nothing, so it is left alone and the locker never
 * answers it.
 *
 * A locker has no build to ask for a size, and most of them are furniture
 * standing at waist height anyway -- so only how low it sits decides whether a
 * character stoops. A bin on a pavement is stooped over, a wardrobe is not.
 */

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
        Locker:lockerid = GetPlayerLocker(playerid)
    ;

    if (!IsValidLocker(lockerid)) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetLockerName(lockerid, name);

    new const
        Container:containerid = GetLockerContainer(lockerid)
    ;

    if (!ShowPlayerContainer(playerid, containerid)) {
        return 0;
    }

    new
        Float:lockerX,
        Float:lockerY,
        Float:lockerZ
    ;

    GetLockerPos(lockerid, lockerX, lockerY, lockerZ);

    ReachForItem(playerid, lockerZ);

    SendPlayerNotice(playerid, "You open the %s. %i of %i used.", name, GetContainerSize(containerid), GetContainerCapacity(containerid));

    return 0;
}

hook OnPlayerRequestPrompt(playerid) {
    new const
        Locker:lockerid = GetPlayerLocker(playerid)
    ;

    if (!IsValidLocker(lockerid)) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetLockerName(lockerid, name);

    OfferPlayerPrompt(playerid, PLAYER_PROMPT_REACH, "Press %s to open the %s.", ITEM_KEY_OPEN_NAME, name);

    return 0;
}

hook OnPlayerEnterButtonArea(playerid, Button:buttonid) {
    UpdatePlayerPrompt(playerid);

    return 0;
}

hook OnPlayerLeaveButtonArea(playerid, Button:buttonid) {
    UpdatePlayerPrompt(playerid);

    return 0;
}
