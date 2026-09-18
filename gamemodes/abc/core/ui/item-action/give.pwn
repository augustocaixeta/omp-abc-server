#if defined _CORE_UI_ITEM_ACTION_GIVE
    #endinput
#endif
#define _CORE_UI_ITEM_ACTION_GIVE

#include <pp-hooks>

/**
 * # GIVE
 *
 * Handing something to whoever is standing closest. A gamemode with more than a
 * handful of players asks which of them -- a dialog, or a name in the command --
 * but the choice is the only part that changes: the move itself is one call, and
 * it puts the item back if the other inventory filled in between.
 */

hook OnPlayerSelectItemAction(playerid, ItemAction:actionid, Item:itemid, ContainerMenu:menuid, slot) {
    if (actionid != gItemActionGive) {
        return 0;
    }

    new const
        targetid = GetPlayerNearestPlayer(playerid)
    ;

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (targetid == INVALID_PLAYER_ID) {
        SendClientMessage(playerid, -1, "There is nobody close enough.");

        return 1;
    }

    if (!GiveContainerItemToPlayer(playerid, targetid, GetContainerMenuContainer(playerid, menuid), itemid)) {
        SendClientMessage(playerid, -1, "They have no room for %s.", name);

        return 1;
    }

    new
        giver[MAX_PLAYER_NAME],
        taker[MAX_PLAYER_NAME]
    ;

    GetPlayerName(playerid, giver);
    GetPlayerName(targetid, taker);

    SendClientMessage(playerid, -1, "You hand %s to %s.", name, taker);
    SendClientMessage(targetid, -1, "%s hands you %s.", giver, name);

    return 1;
}
