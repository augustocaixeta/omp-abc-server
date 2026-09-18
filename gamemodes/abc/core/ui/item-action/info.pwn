#if defined _CORE_UI_ITEM_ACTION_INFO
    #endinput
#endif
#define _CORE_UI_ITEM_ACTION_INFO

#include <pp-hooks>

/**
 * # INFO
 *
 * The one row the library draws without being asked, because reading what a
 * thing is does not depend on what the thing is. What there is to read does
 * depend on it, which is why the library only draws the row and this answers it.
 *
 * A finished gamemode shows a panel. This is the same information as text, and
 * the uuid is here because it is the thing that survives a save and a load --
 * seeing it is how a persistence bug gets found.
 */

hook OnPlayerSelectItemAction(playerid, ItemAction:actionid, Item:itemid, ContainerMenu:menuid, slot) {
    if (actionid != ITEM_ACTION_INFO) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME],
        amount[MAX_ITEM_AMOUNT_LENGTH],
        attributes[128]
    ;

    GetItemName(itemid, name);

    if (HasItemAmount(itemid)) {
        FormatItemAmount(itemid, amount);

        SendClientMessage(playerid, -1, "%s, %s.", name, amount);
    } else {
        SendClientMessage(playerid, -1, "%s.", name);
    }

    if (FormatItemAttributes(itemid, attributes)) {
        SendClientMessage(playerid, -1, "It is %s.", attributes);
    }

    new const
        capacity = GetItemBuildContainerSize(GetItemBuild(itemid))
    ;

    if (capacity != 0) {
        SendClientMessage(playerid, -1, "It holds %i things.", capacity);
    }

    new
        uuid[MAX_ITEM_UUID_LENGTH]
    ;

    GetItemUUID(itemid, uuid);

    SendClientMessage(playerid, -1, "%s", uuid);

    return 1;
}
