#if defined _INC_CONTAINER
    #endinput
#endif
#define _INC_CONTAINER

#include <open.mp>

#include <I\item>
#include <pp-hooks>

/**
 * # Header
 */

// Somewhere items can be, that is not the ground and not a hand. An inventory,
// a locker, the inside of a crate: all of them are this, and the thing that
// owns one decides what it means.
//
// A container knows what is in it and every item knows which container it is
// in, so both questions are a read rather than a walk. The two are only ever
// written together, which is what keeps a slot from holding an item that no
// longer thinks it is there.

#if !defined MAX_CONTAINERS
    #define MAX_CONTAINERS (Container:2048)
#endif

#if !defined MAX_CONTAINER_CAPACITY
    #define MAX_CONTAINER_CAPACITY (50)
#endif

#if !defined MAX_CONTAINER_NAME
    #define MAX_CONTAINER_NAME (24)
#endif

#define INVALID_CONTAINER_ID (Container:-1)

static enum _:E_CONTAINER_DATA {
    bool:E_CONTAINER_CREATED,

    E_CONTAINER_NAME[MAX_CONTAINER_NAME char],
    E_CONTAINER_CAPACITY,
    E_CONTAINER_SIZE
};

static
    gContainerNextFree[_:MAX_CONTAINERS],
    gContainerFreeHead = -1,
    gContainerHighWater,
    gContainerCount,
    gContainerData[MAX_CONTAINERS][E_CONTAINER_DATA]
;

// Slots are a plain grid rather than a sparse structure, so a slot index is an
// array index and nothing has to be searched to read one. A container declares
// a capacity of at most MAX_CONTAINER_CAPACITY and only that many slots answer.
//
// The empty slot is INVALID_ITEM_ID, which is -1, so the grid holds id + 1 and
// a zeroed cell reads as empty without anything having been written first.
static
    gContainerSlot[MAX_CONTAINERS][MAX_CONTAINER_CAPACITY]
;

static
    Container:gItemContainer[MAX_ITEMS] = { INVALID_CONTAINER_ID, ... },
    gItemContainerSlot[MAX_ITEMS]       = { -1, ... }
;

/**
 * # Functions
 */

/**
 * @brief      Make somewhere items can be.
 *
 * Slots are a plain grid, so `capacity` is fixed for the life of the
 * container and every slot answers from the moment it is made.
 *
 * @param      name      What a menu puts at the top of it.
 * @param      capacity  How many slots, up to MAX_CONTAINER_CAPACITY.
 * @param      call      Whether to fire OnContainerCreate.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the new container
 *             - `INVALID_CONTAINER_ID` when:
 *                 + `capacity` is below 1 or above MAX_CONTAINER_CAPACITY
 *                 + MAX_CONTAINERS already exist
 */
forward Container:CreateContainer(const name[], capacity, bool:call = true);

/**
 * @brief      Take a container away.
 *
 * Everything inside leaves it either way, because an item still pointing at
 * a container that stopped existing could never be placed anywhere again.
 * `clear` only decides whether those items are then destroyed or left loose
 * for the caller to put somewhere.
 *
 * The slots are emptied before OnContainerDestroy fires, so a handler sees
 * an empty container -- read what is inside from the owner's own destroy
 * event instead, which runs first.
 *
 * @param      containerid  Container to remove.
 * @param      clear        Whether to destroy the items that were inside.
 * @param      call         Whether to fire OnContainerDestroy.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is gone
 *             - `false` `containerid` is not a live container
 */
forward bool:DestroyContainer(Container:containerid, bool:clear = true, bool:call = true);

/**
 * @brief      Whether a container exists.
 *
 * @param      containerid  Container to check.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it exists
 *             - `false` when:
 *                 + `containerid` is out of range
 *                 + it was never made, or has been destroyed
 */
forward bool:IsValidContainer(Container:containerid);

/**
 * @brief      How many containers exist.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many exist right now
 */
forward GetContainerCount();

/**
 * @brief      Rename a container.
 *
 * Cut to MAX_CONTAINER_NAME. Nothing redraws on its own, so a menu already
 * on screen keeps the old name until it is drawn again.
 *
 * @param      containerid  Container to rename.
 * @param      name         What to call it.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the name was written
 *             - `false` `containerid` is not a live container
 */
forward bool:SetContainerName(Container:containerid, const name[]);

/**
 * @brief      What a container is called.
 *
 * @param      containerid  Container to read.
 * @param      output       Reads back the name.
 * @param      size         How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` `containerid` is not a live container
 */
forward bool:GetContainerName(Container:containerid, output[], size = sizeof (output));

/**
 * @brief      How many slots a container has.
 *
 * @param      containerid  Container to measure.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the capacity it was made with
 *             - `0` `containerid` is not a live container
 */
forward GetContainerCapacity(Container:containerid);

/**
 * @brief      How many of those slots are in use.
 *
 * Kept as a count rather than found by walking the grid, because it is read
 * far more often than it changes.
 *
 * @param      containerid  Container to count.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many items are in it
 *             - `0` when:
 *                 + `containerid` is not a live container
 *                 + it is empty
 */
forward GetContainerSize(Container:containerid);

/**
 * @brief      Whether a container holds nothing.
 *
 * @param      containerid  Container to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` there is nothing in it
 *             - `false` when:
 *                 + `containerid` is not a live container
 *                 + something is in it
 */
forward bool:IsContainerEmpty(Container:containerid);

/**
 * @brief      Whether there is no room left.
 *
 * @param      containerid  Container to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` every slot is used
 *             - `false` when:
 *                 + `containerid` is not a live container
 *                 + at least one slot is free
 */
forward bool:IsContainerFull(Container:containerid);

/**
 * @brief      Whether one particular slot holds something.
 *
 * @param      containerid  Container to ask.
 * @param      index        Slot to look at.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` an item is in it
 *             - `false` when:
 *                 + `containerid` is not a live container
 *                 + `index` is outside the capacity
 *                 + the slot is empty
 */
forward bool:IsContainerSlotUsed(Container:containerid, index);

/**
 * @brief      What is in one particular slot.
 *
 * @param      containerid  Container to read.
 * @param      index        Slot to look at.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the item in it
 *             - `INVALID_ITEM_ID` when:
 *                 + `containerid` is not a live container
 *                 + `index` is outside the capacity
 *                 + the slot is empty
 */
forward Item:GetContainerSlotItem(Container:containerid, index);

/**
 * @brief      The first slot with nothing in it.
 *
 * Where an item goes when nobody says where. Slots are not compacted, so
 * this is the first gap rather than the end: taking something out of slot 0
 * of a full container puts the next thing added back into slot 0.
 *
 * @param      containerid  Container to search.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the index of the first free slot
 *             - `-1` when:
 *                 + `containerid` is not a live container
 *                 + it is full
 */
forward GetContainerFreeSlot(Container:containerid);

/**
 * @brief      Put an item into a container.
 *
 * An item can only be in one container at a time, so one that is already
 * somewhere is refused rather than moved: take it out of where it is first.
 *
 * @param      containerid  Container to fill.
 * @param      itemid       Item to put in.
 * @param      index        Reads back the slot it went into.
 * @param      addToIndex   Slot to use, or `-1` for the first free one.
 * @param      playerid     Who is doing it, for the event to report.
 * @param      call         Whether to fire OnItemAddToContainer.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is inside, and `index` says where
 *             - `false` when:
 *                 + `containerid` is not a live container
 *                 + `itemid` is not a live item
 *                 + the item is in a container already
 *                 + `addToIndex` is outside the capacity, or in use
 *                 + the container is full
 */
forward bool:AddItemToContainer(Container:containerid, Item:itemid, &index = -1, addToIndex = -1, playerid = INVALID_PLAYER_ID, bool:call = true);

/**
 * @brief      Take whatever is in a slot back out.
 *
 * The item is not destroyed and not put anywhere: it comes out held by
 * nothing, and the caller decides where it goes next. An item left like
 * that is invisible until something places it.
 *
 * @param      containerid  Container to empty a slot of.
 * @param      index        Slot to empty.
 * @param      itemid       Reads back what came out.
 * @param      playerid     Who is doing it, for the event to report.
 * @param      call         Whether to fire OnItemRemoveFromContainer.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the slot is empty and `itemid` is what was in it
 *             - `false` when:
 *                 + `containerid` is not a live container
 *                 + `index` is outside the capacity
 *                 + the slot was already empty
 */
forward bool:RemoveItemFromContainer(Container:containerid, index, &Item:itemid = INVALID_ITEM_ID, playerid = INVALID_PLAYER_ID, bool:call = true);

/**
 * @brief      Two slots of one container trade what is in them.
 *
 * Either may be empty, so a move to an empty slot is the same call with
 * nothing coming back the other way.
 *
 * This exists rather than being left to a remove and an add each way,
 * because those are four calls with four callbacks, and in the middle of
 * them both items belong to no container at all -- a state nothing should
 * ever see and anything hooking the removals does. Here the two slots
 * change together and one callback says so.
 *
 * @param      containerid  Container both slots are in.
 * @param      first        One slot.
 * @param      second       The other.
 * @param      playerid     Who is doing it, for the event to report.
 * @param      call         Whether to fire OnContainerSlotsSwap.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they have traded
 *             - `false` when:
 *                 + `containerid` is not a live container
 *                 + either slot is outside the capacity
 *                 + they are the same slot
 *                 + both slots are empty, so nothing would move
 */
forward bool:SwapContainerSlots(Container:containerid, first, second, playerid = INVALID_PLAYER_ID, bool:call = true);

/**
 * @brief      Whether an item is inside something.
 *
 * False for an item on the ground, in a hand, or held by nothing at all.
 *
 * @param      itemid  Item to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` a container holds it
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + nothing holds it
 */
forward bool:IsItemInContainer(Item:itemid);

/**
 * @brief      Which container holds an item, and which slot of it.
 *
 * Read from the item rather than found by walking every container, so this
 * costs the same whether there is one container or two thousand.
 *
 * @param      itemid       Item to look up.
 * @param      containerid  Reads back what holds it.
 * @param      index        Reads back which slot.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the outputs were written
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + nothing holds it, and both outputs are cleared
 */
forward bool:GetItemContainer(Item:itemid, &Container:containerid = INVALID_CONTAINER_ID, &index = -1);

/**
 * # Events
 */

/**
 * @brief      A container has been made.
 *
 * @param      containerid  The new container.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnContainerCreate(Container:containerid);

/**
 * @brief      A container is going away.
 *
 * Fired once its slots are already empty, so it says that the container is
 * ending rather than what was in it.
 *
 * @param      containerid  The container, still live for this call.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnContainerDestroy(Container:containerid);

/**
 * @brief      An item went into a container.
 *
 * @param      containerid  Container it went into.
 * @param      itemid       Item that went in.
 * @param      index        Slot it went into.
 * @param      playerid     Who did it, or `INVALID_PLAYER_ID` if nobody.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnItemAddToContainer(Container:containerid, Item:itemid, index, playerid);

/**
 * @brief      An item came out of a container.
 *
 * Fired after the slot is already empty and the item belongs to nothing, so
 * it says where the item was rather than where it is.
 *
 * @param      containerid  Container it came out of.
 * @param      itemid       Item that came out.
 * @param      index        Slot it came out of.
 * @param      playerid     Who did it, or `INVALID_PLAYER_ID` if nobody.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnItemRemoveFromContainer(Container:containerid, Item:itemid, index, playerid);

/**
 * @brief      Two slots traded places.
 *
 * Either item may be `INVALID_ITEM_ID`, which is the empty half of a move.
 * Both never are: a swap of two empty slots is refused rather than
 * reported.
 *
 * @param      containerid  Container both slots are in.
 * @param      firstItem    What was in `first`, now in `second`.
 * @param      secondItem   What was in `second`, now in `first`.
 * @param      first        One slot.
 * @param      second       The other.
 * @param      playerid     Who did it, or `INVALID_PLAYER_ID` if nobody.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnContainerSlotsSwap(Container:containerid, Item:firstItem, Item:secondItem, first, second, playerid);

/**
 * # Internal
 */

static stock GetFreeContainerIndexInternal() {
    if (gContainerFreeHead != -1) {
        new const
            index = gContainerFreeHead
        ;

        gContainerFreeHead = gContainerNextFree[index];

        return index;
    }

    if (gContainerHighWater != _:MAX_CONTAINERS) {
        return gContainerHighWater++;
    }

    return -1;
}

static stock Container:AllocContainerInternal() {
    new const
        index = GetFreeContainerIndexInternal()
    ;

    if (index == -1) {
        return INVALID_CONTAINER_ID;
    }

    ++gContainerCount;

    return Container:index;
}

static stock FreeContainerInternal(Container:containerid) {
    gContainerNextFree[_:containerid] = gContainerFreeHead;
    gContainerFreeHead = _:containerid;

    --gContainerCount;
}

static stock bool:IsValidContainerInternal(Container:containerid) {
    if (!(0 <= _:containerid < _:MAX_CONTAINERS)) {
        return false;
    }

    return gContainerData[containerid][E_CONTAINER_CREATED];
}

static stock bool:IsSlotInRangeInternal(Container:containerid, index) {
    return (0 <= index < gContainerData[containerid][E_CONTAINER_CAPACITY]);
}

static stock Item:GetSlotItemInternal(Container:containerid, index) {
    new const
        value = gContainerSlot[containerid][index]
    ;

    return value ? (Item:(value - 1)) : INVALID_ITEM_ID;
}

static stock SetSlotItemInternal(Container:containerid, index, Item:itemid) {
    gContainerSlot[containerid][index] = _:itemid + 1;
}

/**
 * # External
 */

stock Container:CreateContainer(const name[], capacity, bool:call = true) {
    if (!(1 <= capacity <= MAX_CONTAINER_CAPACITY)) {
        return INVALID_CONTAINER_ID;
    }

    new const
        Container:containerid = AllocContainerInternal()
    ;

    if (containerid == INVALID_CONTAINER_ID) {
        return INVALID_CONTAINER_ID;
    }

    gContainerData[containerid][E_CONTAINER_CREATED]  = true;
    gContainerData[containerid][E_CONTAINER_CAPACITY] = capacity;
    gContainerData[containerid][E_CONTAINER_SIZE]     = 0;

    strpack(gContainerData[containerid][E_CONTAINER_NAME], name, MAX_CONTAINER_NAME char);

    for (new i; i != capacity; ++i) {
        gContainerSlot[containerid][i] = 0;
    }

    if (call) {
        CallLocalFunction("OnContainerCreate", "i", _:containerid);
    }

    return containerid;
}

stock bool:DestroyContainer(Container:containerid, bool:clear = true, bool:call = true) {
    if (!IsValidContainerInternal(containerid)) {
        return false;
    }

    for (new i, capacity = gContainerData[containerid][E_CONTAINER_CAPACITY]; i != capacity; ++i) {
        new const
            Item:itemid = GetSlotItemInternal(containerid, i)
        ;

        if (itemid == INVALID_ITEM_ID) {
            continue;
        }

        RemoveItemFromContainer(containerid, i, .call = false);

        if (clear) {
            DestroyItem(itemid);
        }
    }

    gContainerData[containerid][E_CONTAINER_CREATED] = false;

    if (call) {
        CallLocalFunction("OnContainerDestroy", "i", _:containerid);
    }

    static const
        CONTAINER_DATA_NONE[E_CONTAINER_DATA]
    ;

    gContainerData[containerid] = CONTAINER_DATA_NONE;

    FreeContainerInternal(containerid);

    return true;
}

stock bool:IsValidContainer(Container:containerid) {
    return IsValidContainerInternal(containerid);
}

stock GetContainerCount() {
    return gContainerCount;
}

stock bool:SetContainerName(Container:containerid, const name[]) {
    if (!IsValidContainerInternal(containerid)) {
        return false;
    }

    strpack(gContainerData[containerid][E_CONTAINER_NAME], name, MAX_CONTAINER_NAME char);

    return true;
}

stock bool:GetContainerName(Container:containerid, output[], size = sizeof (output)) {
    if (!IsValidContainerInternal(containerid)) {
        return false;
    }

    strunpack(output, gContainerData[containerid][E_CONTAINER_NAME], size);

    return true;
}

stock GetContainerCapacity(Container:containerid) {
    if (!IsValidContainerInternal(containerid)) {
        return 0;
    }

    return gContainerData[containerid][E_CONTAINER_CAPACITY];
}

stock GetContainerSize(Container:containerid) {
    if (!IsValidContainerInternal(containerid)) {
        return 0;
    }

    return gContainerData[containerid][E_CONTAINER_SIZE];
}

stock bool:IsContainerEmpty(Container:containerid) {
    if (!IsValidContainerInternal(containerid)) {
        return false;
    }

    return gContainerData[containerid][E_CONTAINER_SIZE] == 0;
}

stock bool:IsContainerFull(Container:containerid) {
    if (!IsValidContainerInternal(containerid)) {
        return false;
    }

    return gContainerData[containerid][E_CONTAINER_SIZE] >= gContainerData[containerid][E_CONTAINER_CAPACITY];
}

stock bool:IsContainerSlotUsed(Container:containerid, index) {
    if (!IsValidContainerInternal(containerid)) {
        return false;
    }

    if (!IsSlotInRangeInternal(containerid, index)) {
        return false;
    }

    return GetSlotItemInternal(containerid, index) != INVALID_ITEM_ID;
}

stock Item:GetContainerSlotItem(Container:containerid, index) {
    if (!IsValidContainerInternal(containerid)) {
        return INVALID_ITEM_ID;
    }

    if (!IsSlotInRangeInternal(containerid, index)) {
        return INVALID_ITEM_ID;
    }

    return GetSlotItemInternal(containerid, index);
}

stock GetContainerFreeSlot(Container:containerid) {
    if (!IsValidContainerInternal(containerid)) {
        return -1;
    }

    for (new i, capacity = gContainerData[containerid][E_CONTAINER_CAPACITY]; i != capacity; ++i) {
        if (GetSlotItemInternal(containerid, i) == INVALID_ITEM_ID) {
            return i;
        }
    }

    return -1;
}

stock bool:AddItemToContainer(Container:containerid, Item:itemid, &index = -1, addToIndex = -1, playerid = INVALID_PLAYER_ID, bool:call = true) {
    index = -1;

    if (!IsValidContainerInternal(containerid)) {
        return false;
    }

    if (!IsValidItem(itemid)) {
        return false;
    }

    if (gItemContainer[itemid] != INVALID_CONTAINER_ID) {
        return false;
    }

    if (addToIndex >= 0) {
        if (!IsSlotInRangeInternal(containerid, addToIndex)) {
            return false;
        }

        if (GetSlotItemInternal(containerid, addToIndex) != INVALID_ITEM_ID) {
            return false;
        }

        index = addToIndex;
    } else {
        index = GetContainerFreeSlot(containerid);

        if (index == -1) {
            return false;
        }
    }

    SetSlotItemInternal(containerid, index, itemid);

    ++gContainerData[containerid][E_CONTAINER_SIZE];

    gItemContainer[itemid] = containerid;
    gItemContainerSlot[itemid] = index;

    if (call) {
        CallLocalFunction("OnItemAddToContainer", "iiii", _:containerid, _:itemid, index, playerid);
    }

    return true;
}

stock bool:RemoveItemFromContainer(Container:containerid, index, &Item:itemid = INVALID_ITEM_ID, playerid = INVALID_PLAYER_ID, bool:call = true) {
    itemid = INVALID_ITEM_ID;

    if (!IsValidContainerInternal(containerid)) {
        return false;
    }

    if (!IsSlotInRangeInternal(containerid, index)) {
        return false;
    }

    itemid = GetSlotItemInternal(containerid, index);

    if (itemid == INVALID_ITEM_ID) {
        return false;
    }

    SetSlotItemInternal(containerid, index, INVALID_ITEM_ID);

    --gContainerData[containerid][E_CONTAINER_SIZE];

    gItemContainer[itemid] = INVALID_CONTAINER_ID;
    gItemContainerSlot[itemid] = -1;

    if (call) {
        CallLocalFunction("OnItemRemoveFromContainer", "iiii", _:containerid, _:itemid, index, playerid);
    }

    return true;
}

stock bool:SwapContainerSlots(Container:containerid, first, second, playerid = INVALID_PLAYER_ID, bool:call = true) {
    if (!IsValidContainerInternal(containerid)) {
        return false;
    }

    if (!IsSlotInRangeInternal(containerid, first)) {
        return false;
    }

    if (!IsSlotInRangeInternal(containerid, second)) {
        return false;
    }

    if (first == second) {
        return false;
    }

    new const
        Item:firstItem = GetSlotItemInternal(containerid, first),
        Item:secondItem = GetSlotItemInternal(containerid, second)
    ;

    if (firstItem == INVALID_ITEM_ID && secondItem == INVALID_ITEM_ID) {
        return false;
    }

    SetSlotItemInternal(containerid, first, secondItem);
    SetSlotItemInternal(containerid, second, firstItem);

    if (firstItem != INVALID_ITEM_ID) {
        gItemContainerSlot[firstItem] = second;
    }

    if (secondItem != INVALID_ITEM_ID) {
        gItemContainerSlot[secondItem] = first;
    }

    if (call) {
        CallLocalFunction("OnContainerSlotsSwap", "iiiiii", _:containerid, _:firstItem, _:secondItem, first, second, playerid);
    }

    return true;
}

stock bool:IsItemInContainer(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    return gItemContainer[itemid] != INVALID_CONTAINER_ID;
}

stock bool:GetItemContainer(Item:itemid, &Container:containerid = INVALID_CONTAINER_ID, &index = -1) {
    containerid = INVALID_CONTAINER_ID;
    index = -1;

    if (!IsValidItem(itemid)) {
        return false;
    }

    if (gItemContainer[itemid] == INVALID_CONTAINER_ID) {
        return false;
    }

    containerid = gItemContainer[itemid];
    index = gItemContainerSlot[itemid];

    return true;
}

/**
 * # Calls
 */

hook OnItemDestroy(Item:itemid) {
    new const
        Container:containerid = gItemContainer[itemid]
    ;

    if (IsValidContainerInternal(containerid)) {
        RemoveItemFromContainer(containerid, gItemContainerSlot[itemid]);
    }

    gItemContainer[itemid] = INVALID_CONTAINER_ID;
    gItemContainerSlot[itemid] = -1;

    return 0;
}
