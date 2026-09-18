#if defined _CORE_UI_CONTAINER_MENU
    #endinput
#endif
#define _CORE_UI_CONTAINER_MENU

#include <pp-hooks>

/**
 * # The inventory, and whatever is in front of it
 *
 * Two menus. The pockets on the right, which are the character's own and are
 * what everything else is moved to and from, and a second grid on the left for
 * anything else with an inside: a crate, a locker, a bin, a shop counter.
 *
 * Opening anything that is not the character's own pockets opens both, and in
 * that order -- the pockets first so the thing the character walked up to is
 * the one in front. Escape closes the front one and leaves the pockets, and
 * escape again closes those, which is why the order they were opened in
 * matters.
 *
 * Two menus rather than one shared menu showing one container at a time,
 * because taking something out of a crate is a gesture between two places. With
 * one grid it is two menus' worth of clicking to move a single item.
 *
 * What a click on a slot means is this file's decision, not the library's -- a
 * click could equally mean use it outright or start a drag. Here it opens the
 * action menu against the slot.
 *
 * Clicking what is already open closes it, and that is asked before the
 * transfer: a second click on the same slot is answered there as a selection
 * being called off -- true, but it leaves the menu standing over a slot the
 * character has just let go of.
 */

new
    ContainerMenu:gInventoryMenu = INVALID_CONTAINER_MENU_ID,
    ContainerMenu:gContainerMenu = INVALID_CONTAINER_MENU_ID
;

/**
 * # Functions
 */

/**
 * @brief      Put a character's own pockets on their screen.
 *
 * @param      playerid  Character to show it to.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the menu is up
 *             - `false` the character has no inventory container
 */
forward bool:ShowPlayerInventory(playerid);

/**
 * @brief      Put any container on a character's screen.
 *
 * Their own pockets go in the menu on the right and stand alone. Anything
 * else goes in the menu on the left, and opens the pockets beside it --
 * everything a character does with a crate is done between the two, and a
 * crate on its own would only be half of it.
 *
 * @param      playerid  Character to show it to.
 * @param      containerid Container to draw into the menu.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the menu is up
 *             - `false` when:
 *                 + `containerid` is not a live container
 *                 + the framework refused to draw it
 */
forward bool:ShowPlayerContainer(playerid, Container:containerid);

/**
 * @brief      Take the thing in front of the pockets off the screen.
 *
 * The pockets are left where they are, which is what escape does with the
 * front menu and what closing a crate means: the character is still
 * standing there holding what they took out of it.
 *
 * @param      playerid  Character to hide it from.
 *
 * @date       15:30 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` something was in front and is now gone
 *             - `false` nothing was
 */
forward bool:HidePlayerSecondaryContainer(playerid);

/**
 * @brief      What the character has open in front of their pockets.
 *
 * @param      playerid  Character to read.
 *
 * @date       15:30 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - the container drawn in the menu on the left
 *             - `INVALID_CONTAINER_ID` there is nothing in front
 */
forward Container:GetPlayerSecondaryContainer(playerid);

/**
 * @brief      Take the menu off a character's screen.
 *
 * Named for the inventory but it closes whatever is in the menu, because
 * there is only ever the one.
 *
 * @param      playerid  Character to hide it from.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it was up and is now gone
 *             - `false` it was not up
 */
forward bool:HidePlayerInventory(playerid);

/**
 * @brief      Which container a character is looking at, if any.
 *
 * The menu carries the answer already -- it is what it is drawing -- so this is
 * a read rather than any bookkeeping of its own. Anything acting on a slot the
 * character clicked wants this and not their pockets: the pockets are only the
 * answer when the pockets are what is open.
 *
 * @param      playerid  Character to read.
 *
 * @date       00:20 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - the container on their screen
 *             - `INVALID_CONTAINER_ID` the menu is not open for them
 */
forward Container:GetPlayerOpenContainer(playerid);

/**
 * # External
 */

stock bool:ShowPlayerContainer(playerid, Container:containerid) {
    if (!IsValidContainer(containerid)) {
        return false;
    }

    if (containerid == GetPlayerContainer(playerid)) {
        return ShowContainerMenu(playerid, containerid, gInventoryMenu);
    }

    // The pockets first, so escape takes the crate and leaves them.
    ShowContainerMenu(playerid, GetPlayerContainer(playerid), gInventoryMenu);

    return ShowContainerMenu(playerid, containerid, gContainerMenu);
}

stock bool:HidePlayerSecondaryContainer(playerid) {
    return HideContainerMenu(playerid, gContainerMenu);
}

stock Container:GetPlayerSecondaryContainer(playerid) {
    if (!IsPlayerUsingContainerMenu(playerid, gContainerMenu)) {
        return INVALID_CONTAINER_ID;
    }

    return GetContainerMenuContainer(playerid, gContainerMenu);
}

stock bool:ShowPlayerInventory(playerid) {
    return ShowPlayerContainer(playerid, GetPlayerContainer(playerid));
}

stock bool:HidePlayerInventory(playerid) {
    // Whatever was in front of the pockets goes with them. They were opened
    // together and closing only the half behind would leave a crate hanging on
    // the screen with nothing to move anything into.
    HideContainerMenu(playerid, gContainerMenu);

    return HideContainerMenu(playerid, gInventoryMenu);
}

stock Container:GetPlayerOpenContainer(playerid) {
    if (!IsPlayerUsingContainerMenu(playerid, gInventoryMenu)) {
        return INVALID_CONTAINER_ID;
    }

    return GetContainerMenuContainer(playerid, gInventoryMenu);
}

/**
 * # Calls
 */

hook OnGameModeInit() {
    gInventoryMenu = BuildContainerMenu(
        INVENTORY_MENU_X,
        INVENTORY_MENU_Y,
        INVENTORY_SLOT_SIZE_X,
        INVENTORY_SLOT_SIZE_Y,
        INVENTORY_MENU_ROWS,
        INVENTORY_MENU_COLUMNS,
        INVENTORY_MENU_COLOUR,
        .frame = true,
        .title = INVENTORY_MENU_TITLE
    );

    if (gInventoryMenu == INVALID_CONTAINER_MENU_ID) {
        print("[menu] the inventory menu could not be built");
    }

    gContainerMenu = BuildContainerMenu(
        CONTAINER_MENU_X,
        CONTAINER_MENU_Y,
        CONTAINER_MENU_SLOT_SIZE_X,
        CONTAINER_MENU_SLOT_SIZE_Y,
        CONTAINER_MENU_ROWS,
        CONTAINER_MENU_COLUMNS,
        CONTAINER_MENU_COLOUR,
        .frame = true,
        .title = CONTAINER_MENU_TITLE
    );

    if (gContainerMenu == INVALID_CONTAINER_MENU_ID) {
        print("[menu] the container menu could not be built");
    }

    return 0;
}

/**
 * The pockets on a key rather than only a command. Y already stows whatever is
 * in the character's hands, so it is read as one gesture with two halves: with
 * something in their hands it goes away, and with their hands empty they look
 * at where it went.
 *
 * This runs before hands.pwn, which is what answers the other half, so it has
 * to leave a character holding something alone rather than open anything.
 */
hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if (!(newkeys & INVENTORY_KEY)) {
        return 0;
    }

    if (IsValidItem(GetPlayerHoldItem(playerid))) {
        return 0;
    }

    if (IsPlayerUsingContainerMenu(playerid, gInventoryMenu)) {
        HidePlayerInventory(playerid);

        return 1;
    }

    ShowPlayerInventory(playerid);

    return 1;
}

public OnPlayerClickContainerMenuSlot(playerid, ContainerMenu:menuid, slot, index, Item:itemid) {
    if (IsPlayerUsingItemActionMenu(playerid) && GetPlayerItemActionItem(playerid) == itemid && itemid != INVALID_ITEM_ID) {
        HideItemActionMenu(playerid);
        ClearContainerMenuSelection(playerid);

        return 1;
    }

    if (TransferToContainerMenuSlot(playerid, menuid, slot)) {
        return 1;
    }

    if (itemid == INVALID_ITEM_ID) {
        return 1;
    }

    SelectContainerMenuSlot(playerid, menuid, slot);

    if (!ShowItemActionMenu(playerid, menuid, slot)) {
        new
            name[MAX_ITEM_NAME]
        ;

        GetItemName(itemid, name);
        SendClientMessage(playerid, -1, "There is nothing to do with %s.", name);
    }

    return 1;
}
