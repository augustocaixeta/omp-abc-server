#if defined _INC_INVENTORY
    #endinput
#endif
#define _INC_INVENTORY

#include <open.mp>

#include <I\container>
#include <pp-hooks>

/**
 * # Header
 */

// An inventory is a container owned by a player, so the container does the
// holding and this only records which one belongs to whom, in both directions.
//
// Nearly everything here is that lookup followed by the container call of the
// same name, which is why so few of these check anything themselves: an id
// that is not a player reads as INVALID_CONTAINER_ID, and the container side
// refuses that already.

#if !defined MAX_INVENTORY_CAPACITY
    #define MAX_INVENTORY_CAPACITY (50)
#endif

#if !defined INVENTORY_NAME
    #define INVENTORY_NAME "INVENTORY"
#endif

static
    Container:gPlayerContainer[MAX_PLAYERS] = { INVALID_CONTAINER_ID, ... },
    gContainerPlayer[MAX_CONTAINERS]        = { INVALID_PLAYER_ID, ... }
;

/**
 * # Functions
 */

/**
 * @brief      The container that is a player's inventory.
 *
 * @param      playerid  Player to look up.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - their inventory container
 *             - `INVALID_CONTAINER_ID` when:
 *                 + `playerid` is out of range
 *                 + they have no inventory yet
 */
forward Container:GetPlayerContainer(playerid);

/**
 * @brief      Whose inventory a container is, if it is one.
 *
 * The other half of the lookup, for code holding a container and needing to
 * know whether it belongs to somebody.
 *
 * @param      containerid  Container to look up.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the player who owns it
 *             - `INVALID_PLAYER_ID` when:
 *                 + `containerid` is not a live container
 *                 + it is not anybody's inventory
 */
forward GetContainerPlayer(Container:containerid);

/**
 * @brief      Whether a player has an inventory at all.
 *
 * They get one on connect and lose it on disconnect, so this is false only
 * before the first and after the last.
 *
 * @param      playerid  Player to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they have one
 *             - `false` when:
 *                 + `playerid` is out of range
 *                 + it has not been made, or is already gone
 */
forward bool:HasPlayerInventory(playerid);

/**
 * @brief      How many slots a player's inventory has.
 *
 * @param      playerid  Player to measure.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the slot count
 *             - `0` when they have no inventory
 */
forward GetPlayerInventoryCapacity(playerid);

/**
 * @brief      How many of those slots are in use.
 *
 * @param      playerid  Player to count.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many items are in it
 *             - `0` when:
 *                 + they have no inventory
 *                 + it is empty
 */
forward GetPlayerInventorySize(playerid);

/**
 * @brief      Whether a player is carrying nothing.
 *
 * @param      playerid  Player to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` there is nothing in it
 *             - `false` when:
 *                 + they have no inventory
 *                 + something is in it
 */
forward bool:IsPlayerInventoryEmpty(playerid);

/**
 * @brief      Whether there is no room left.
 *
 * @param      playerid  Player to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` every slot is used
 *             - `false` when:
 *                 + they have no inventory
 *                 + at least one slot is free
 */
forward bool:IsPlayerInventoryFull(playerid);

/**
 * @brief      Whether one particular slot holds something.
 *
 * @param      playerid  Player to ask.
 * @param      index     Slot to look at.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` an item is in it
 *             - `false` when:
 *                 + they have no inventory
 *                 + `index` is outside the capacity
 *                 + the slot is empty
 */
forward bool:IsPlayerInventorySlotUsed(playerid, index);

/**
 * @brief      What is in one particular slot.
 *
 * @param      playerid  Player to read.
 * @param      index     Slot to look at.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the item in it
 *             - `INVALID_ITEM_ID` when:
 *                 + they have no inventory
 *                 + `index` is outside the capacity
 *                 + the slot is empty
 */
forward Item:GetPlayerInventorySlotItem(playerid, index);

/**
 * @brief      The first slot with nothing in it.
 *
 * Where an item goes when nobody says where. Slots are not compacted, so
 * this is the first gap rather than the end.
 *
 * @param      playerid  Player to search.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the index of the first free slot
 *             - `-1` when:
 *                 + they have no inventory
 *                 + it is full
 */
forward GetPlayerInventoryFreeSlot(playerid);

/**
 * @brief      Put an item into a player's inventory.
 *
 * Passing `call` as false moves it without telling anybody, for the middle
 * of a move that is not finished -- a transfer between two containers that
 * should read as one event, or a load that is rebuilding what was already
 * there. Everything ordinary wants it left alone.
 *
 * @param      playerid    Player to give it to.
 * @param      itemid      Item to put in.
 * @param      index       Reads back the slot it went into.
 * @param      addToIndex  Slot to use, or `-1` for the first free one.
 * @param      call        Whether to fire the events.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is in the inventory
 *             - `false` when:
 *                 + they have no inventory
 *                 + the item is not live, or is somewhere else already
 *                 + `addToIndex` is outside the capacity, or in use
 *                 + the inventory is full
 */
forward bool:AddItemToInventory(playerid, Item:itemid, &index = -1, addToIndex = -1, bool:call = true);

/**
 * @brief      Take whatever is in a slot back out.
 *
 * The item is not destroyed and not put anywhere: it comes out held by
 * nothing, and the caller decides where it goes next.
 *
 * @param      playerid  Player to take it from.
 * @param      index     Slot to empty.
 * @param      itemid    Reads back what came out.
 * @param      call      Whether to fire the events.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the slot is empty and `itemid` is what was in it
 *             - `false` when:
 *                 + they have no inventory
 *                 + `index` is outside the capacity
 *                 + the slot was already empty
 */
forward bool:RemoveItemFromInventory(playerid, index, &Item:itemid = INVALID_ITEM_ID, bool:call = true);

/**
 * @brief      Whether an item is in somebody's inventory, and whose.
 *
 * An item in a container that is not an inventory -- a crate, a locker --
 * answers false, and leaves both outputs cleared.
 *
 * @param      itemid    Item to find.
 * @param      playerid  Reads back who is carrying it.
 * @param      index     Reads back which slot it is in.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` somebody is carrying it
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + it is in no container at all
 *                 + the container it is in is not an inventory
 */
forward bool:IsItemInInventory(Item:itemid, &playerid = INVALID_PLAYER_ID, &index = -1);

/**
 * # Events
 */

/**
 * @brief      A player has been given an inventory.
 *
 * Fired on connect, once the container exists. Where anything that wants to
 * put a character's belongings back arms itself.
 *
 * @param      playerid     Player it belongs to.
 * @param      containerid  The container that is now their inventory.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerInventoryCreate(playerid, Container:containerid);

/**
 * @brief      A player's inventory is about to go.
 *
 * Fired on disconnect while the container is still whole, so what is inside
 * it can still be read and written down. Afterwards the container is
 * destroyed and everything in it with it.
 *
 * @param      playerid     Player it belonged to.
 * @param      containerid  The container, still live for this call.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerInventoryDestroy(playerid, Container:containerid);

/**
 * @brief      An item went into a player's inventory.
 *
 * @param      playerid  Player carrying it.
 * @param      itemid    Item that went in.
 * @param      index     Slot it went into.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnItemAddToInventory(playerid, Item:itemid, index);

/**
 * @brief      An item came out of a player's inventory.
 *
 * Fired after the slot is already empty, so it says where the item was
 * rather than where it is.
 *
 * @param      playerid  Player who was carrying it.
 * @param      itemid    Item that came out.
 * @param      index     Slot it came out of.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnItemRemoveFromInventory(playerid, Item:itemid, index);

/**
 * # External
 */

stock Container:GetPlayerContainer(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return INVALID_CONTAINER_ID;
    }

    return gPlayerContainer[playerid];
}

stock GetContainerPlayer(Container:containerid) {
    if (!IsValidContainer(containerid)) {
        return INVALID_PLAYER_ID;
    }

    return gContainerPlayer[containerid];
}

stock bool:HasPlayerInventory(playerid) {
    return IsValidContainer(GetPlayerContainer(playerid));
}

stock GetPlayerInventoryCapacity(playerid) {
    return GetContainerCapacity(GetPlayerContainer(playerid));
}

stock GetPlayerInventorySize(playerid) {
    return GetContainerSize(GetPlayerContainer(playerid));
}

stock bool:IsPlayerInventoryEmpty(playerid) {
    return IsContainerEmpty(GetPlayerContainer(playerid));
}

stock bool:IsPlayerInventoryFull(playerid) {
    return IsContainerFull(GetPlayerContainer(playerid));
}

stock bool:IsPlayerInventorySlotUsed(playerid, index) {
    return IsContainerSlotUsed(GetPlayerContainer(playerid), index);
}

stock Item:GetPlayerInventorySlotItem(playerid, index) {
    return GetContainerSlotItem(GetPlayerContainer(playerid), index);
}

stock GetPlayerInventoryFreeSlot(playerid) {
    return GetContainerFreeSlot(GetPlayerContainer(playerid));
}

stock bool:AddItemToInventory(playerid, Item:itemid, &index = -1, addToIndex = -1, bool:call = true) {
    if (!AddItemToContainer(GetPlayerContainer(playerid), itemid, index, addToIndex, playerid, call)) {
        return false;
    }

    if (call) {
        CallLocalFunction("OnItemAddToInventory", "iii", playerid, _:itemid, index);
    }

    return true;
}

stock bool:RemoveItemFromInventory(playerid, index, &Item:itemid = INVALID_ITEM_ID, bool:call = true) {
    if (!RemoveItemFromContainer(GetPlayerContainer(playerid), index, itemid, playerid, call)) {
        return false;
    }

    if (call) {
        CallLocalFunction("OnItemRemoveFromInventory", "iii", playerid, _:itemid, index);
    }

    return true;
}

stock bool:IsItemInInventory(Item:itemid, &playerid = INVALID_PLAYER_ID, &index = -1) {
    playerid = INVALID_PLAYER_ID;
    index = -1;

    new
        Container:containerid = INVALID_CONTAINER_ID
    ;

    if (!GetItemContainer(itemid, containerid, index)) {
        return false;
    }

    playerid = gContainerPlayer[containerid];

    if (playerid == INVALID_PLAYER_ID) {
        index = -1;

        return false;
    }

    return true;
}

/**
 * # Calls
 */

hook OnPlayerConnect(playerid) {
    new const
        Container:containerid = CreateContainer(INVENTORY_NAME, MAX_INVENTORY_CAPACITY)
    ;

    if (containerid == INVALID_CONTAINER_ID) {
        return 0;
    }

    gPlayerContainer[playerid] = containerid;
    gContainerPlayer[containerid] = playerid;

    CallLocalFunction("OnPlayerInventoryCreate", "ii", playerid, _:containerid);

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    new const
        Container:containerid = gPlayerContainer[playerid]
    ;

    if (!IsValidContainer(containerid)) {
        return 0;
    }

    CallLocalFunction("OnPlayerInventoryDestroy", "ii", playerid, _:containerid);

    gPlayerContainer[playerid] = INVALID_CONTAINER_ID;
    gContainerPlayer[containerid] = INVALID_PLAYER_ID;

    DestroyContainer(containerid);

    return 0;
}
