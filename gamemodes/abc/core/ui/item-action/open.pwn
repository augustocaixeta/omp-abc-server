#if defined _CORE_UI_ITEM_ACTION_OPEN
    #endinput
#endif
#define _CORE_UI_ITEM_ACTION_OPEN

#include <pp-hooks>

/**
 * # OPEN
 *
 * Shows what is inside an item in the same menu the pockets use. Which
 * container that is, and when it comes into existence, is itemtype/container's
 * business -- this only asks.
 */

hook OnPlayerSelectItemAction(playerid, ItemAction:actionid, Item:itemid, ContainerMenu:menuid, slot) {
    if (actionid != gItemActionOpen) {
        return 0;
    }

    new const
        Container:containerid = GetItemInside(itemid)
    ;

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (containerid == INVALID_CONTAINER_ID) {
        SendClientMessage(playerid, -1, "%s does not open.", name);

        return 1;
    }

    if (!ShowPlayerContainer(playerid, containerid)) {
        SendClientMessage(playerid, -1, "You cannot open %s right now.", name);

        return 1;
    }

    SendClientMessage(playerid, -1, "You open %s. %i of %i used.",
        name, GetContainerSize(containerid), GetContainerCapacity(containerid));

    return 1;
}
