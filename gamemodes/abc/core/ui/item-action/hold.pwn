#if defined _CORE_UI_ITEM_ACTION_HOLD
    #endinput
#endif
#define _CORE_UI_ITEM_ACTION_HOLD

#include <pp-hooks>

/**
 * # PICK UP
 *
 * The row that does attach something. A crate, a bag, a body: things carried in
 * the hands rather than armed or eaten.
 *
 * Picking a second thing up puts the first back in the slot the second is
 * leaving, which is HoldContainerItem's doing -- a character has one pair of hands
 * and an item let go of has to land somewhere.
 */

hook OnPlayerSelectItemAction(playerid, ItemAction:actionid, Item:itemid, ContainerMenu:menuid, slot) {
    if (actionid != gItemActionHold) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    new const
        Item:held = GetPlayerHoldItem(playerid),
        Container:containerid = GetContainerMenuContainer(playerid, menuid)
    ;

    if (IsValidItem(held) && !CanPutItemInContainer(held, containerid)) {
        new
            heldName[MAX_ITEM_NAME]
        ;

        GetItemName(held, heldName);

        SendPlayerNotice(playerid, "Put the %s down first. It does not go in there.", heldName);

        return 1;
    }

    if (!HoldContainerItem(playerid, containerid, itemid)) {
        SendPlayerNotice(playerid, "You cannot pick that up.");

        return 1;
    }

    SendClientMessage(playerid, -1, "You are carrying %s.", name);

    return 1;
}
