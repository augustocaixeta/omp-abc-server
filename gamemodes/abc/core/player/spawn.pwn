#if defined _CORE_PLAYER_SPAWN
    #endinput
#endif
#define _CORE_PLAYER_SPAWN

#include <pp-hooks>

/**
 * # The character
 *
 * What a connection owns and what happens to it when the connection goes. The
 * inventory itself is the framework's -- inventory.inc creates a container per
 * player and destroys it on disconnect -- so this is only what the gamemode
 * adds on top.
 */

public OnPlayerConnect(playerid) {
    SendClientMessage(playerid, -1, "Roleplay. /help for what there is so far.");

    return 1;
}

public OnPlayerRequestClass(playerid, classid) {
    SetPlayerPos(playerid, 2453.2114, -1977.1317, 13.5469);
    SetPlayerCameraPos(playerid, 2453.2114, -1977.1317, 17.5469);
    SetPlayerCameraLookAt(playerid, 2453.2114, -1977.1317, 13.5469);

    return 1;
}

public OnPlayerSpawn(playerid) {
    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);

    SetPlayerClothes(playerid, Clothes:0);

    return 1;
}

hook OnGameModeInit() {
    SetGameModeText("Roleplay");

    AddPlayerClass(26, 2453.2114, -1977.1317, 13.5469, 0.0000,
        WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);

    return 0;
}

public OnPlayerPickedUpItem(playerid, Item:itemid) {
    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (HasItemAttribute(itemid, ITEM_ATTRIBUTE_HOLDABLE)) {
        SendPlayerNotice(playerid, "You pick up the %s.", name);

        return 0;
    }

    if (!CanStoreItem(itemid)) {
        SendPlayerNotice(playerid, "The %s is too big to pocket.", name);

        return 1;
    }

    if (IsPlayerInventoryFull(playerid)) {
        SendPlayerNotice(playerid, "You have no room for the %s.", name);

        return 1;
    }

    RemoveItemFromWorld(itemid);

    new
        index = -1
    ;

    if (!AddItemToInventory(playerid, itemid, index)) {
        SendPlayerNotice(playerid, "You have no room for the %s.", name);

        return 1;
    }

    SendPlayerNotice(playerid, "You pick up the %s.", name);

    return 1;
}

public OnItemDurationExpired(Item:itemid) {
    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    printf("[rp] %s (item %i) expired", name, _:itemid);

    DestroyItem(itemid);

    return 1;
}
