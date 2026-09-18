#if defined _CORE_UI_ITEM_ACTION_DISCARD
    #endinput
#endif
#define _CORE_UI_ITEM_ACTION_DISCARD

#include <pp-hooks>

/**
 * # DISCARD
 *
 * Thrown away rather than put down. The character lobs it underarm and it is
 * gone: no object on the ground, no button on it, nothing for anybody to pick
 * up -- which is the whole difference from DROP, and the reason both rows exist
 * on the same item. One is putting something somewhere; this is being rid of it.
 *
 * An empty jerrycan is what it is for. A vessel is kept when it is poured out,
 * because a can that can be filled again is worth carrying, and a character who
 * disagrees says so here.
 *
 * A vessel with anything still in it is refused. Being rid of an empty can is a
 * tidy up; being rid of five litres of petrol by clicking one row below the one
 * that gives it away is an accident waiting on a menu, and pouring it out first
 * is one click that cannot go wrong.
 */

hook OnPlayerSelectItemAction(playerid, ItemAction:actionid, Item:itemid, ContainerMenu:menuid, slot) {
    if (actionid != gItemActionDiscard) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (IsItemLiquidContainer(itemid) && GetItemLiquidAmount(itemid) > 0) {
        SendPlayerNotice(playerid, "Pour the %s out first.", name);

        return 1;
    }

    new
        Container:containerid = INVALID_CONTAINER_ID,
        index = -1
    ;

    if (GetItemContainer(itemid, containerid, index)) {
        RemoveItemFromContainer(containerid, index, .playerid = playerid);
    }

    DestroyItem(itemid);

    HideItemActionMenu(playerid);

    // Underarm, because that is how somebody gets rid of a tin rather than how
    // they attack with one. The grenade library is where the game keeps its
    // only throw.
    ApplyAnimation(playerid, ITEM_DISCARD_ANIMATION_LIBRARY, ITEM_DISCARD_ANIMATION_NAME, ITEM_DISCARD_ANIMATION_SPEED, false, false, false, false, 0);

    SendPlayerNotice(playerid, "You throw the %s away.", name);

    return 1;
}
