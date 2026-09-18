#if defined _CORE_UI_ITEM_ACTION_SPLIT
    #endinput
#endif
#define _CORE_UI_ITEM_ACTION_SPLIT

#include <pp-hooks>

/**
 * # SPLIT
 *
 * Half of a stack becomes a second item of the same kind. Cash is what this is
 * for: handing someone 200 out of 500 is one of the few things a roleplay
 * gamemode does constantly, and doing it by destroying and recreating loses
 * everything else the item was carrying.
 *
 * The half that leaves is the smaller one, so an odd number stays whole in the
 * hand it started in.
 */

hook OnPlayerSelectItemAction(playerid, ItemAction:actionid, Item:itemid, ContainerMenu:menuid, slot) {
    if (actionid != gItemActionSplit) {
        return 0;
    }

    new const
        amount = GetItemAmountInt(itemid)
    ;

    if (amount < 2) {
        SendClientMessage(playerid, -1, "There is not enough of it to divide.");

        return 1;
    }

    if (IsPlayerInventoryFull(playerid)) {
        SendClientMessage(playerid, -1, "You have nowhere to put the other half.");

        return 1;
    }

    new const
        taken = amount / 2,
        Item:half = CreateItem(GetItemBuild(itemid))
    ;

    if (half == INVALID_ITEM_ID) {
        SendClientMessage(playerid, -1, "That cannot be divided right now.");

        return 1;
    }

    SetItemAmountInt(half, taken);

    if (!AddItemToInventory(playerid, half)) {
        DestroyItem(half);

        SendClientMessage(playerid, -1, "You have nowhere to put the other half.");

        return 1;
    }

    SetItemAmountInt(itemid, amount - taken);

    RefreshContainerMenu(playerid, menuid);

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    SendClientMessage(playerid, -1, "You divide %s into %i and %i.", name, amount - taken, taken);

    return 1;
}
