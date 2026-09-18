#if defined _CORE_UI_ITEM_ACTION_DROP
    #endinput
#endif
#define _CORE_UI_ITEM_ACTION_DROP

#include <pp-hooks>

/**
 * # DROP
 *
 * Onto the ground in front of the character, where it becomes a world object
 * with a button on it that anyone can pick up.
 *
 * This row is an attribute rather than something every item gets, which is the
 * whole reason ITEM_ATTRIBUTE_DROPPABLE exists: papers, keys and anything handed over
 * rather than abandoned simply never draw it.
 */

hook OnPlayerSelectItemAction(playerid, ItemAction:actionid, Item:itemid, ContainerMenu:menuid, slot) {
    if (actionid != gItemActionDrop) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (!DropContainerItem(playerid, GetContainerMenuContainer(playerid, menuid), itemid)) {
        SendClientMessage(playerid, -1, "You cannot put that down here.");

        return 1;
    }

    SendClientMessage(playerid, -1, "You drop %s.", name);

    return 1;
}
