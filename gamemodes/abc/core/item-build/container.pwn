#if defined _CORE_ITEM_BUILD_CONTAINER
    #endinput
#endif
#define _CORE_ITEM_BUILD_CONTAINER

#include <pp-hooks>

/**
 * # Items with an inside
 *
 * A bag is two things: an item that can be carried and dropped, and a container
 * that holds other items. The framework has both halves and no opinion about
 * joining them -- an item with an inside is a gamemode's idea, not an item's --
 * so the join is here.
 *
 * The inside is built the first time it is asked for rather than when the item
 * is made. Most bags are never opened, and a container that exists is a
 * container that costs a slot in the framework's table.
 *
 * Both arrays are declared with tagged sizes, so an ItemBuild: and an Item:
 * index them and a Container: comes back out, none of it needing a cast.
 */

// How deep a box in a box in a box may go before the walk that checks for
// loops gives up. Nothing legitimate comes close: the shape this allows is a
// locker holding a rucksack holding a pizza box, which is three.
#if !defined MAX_CONTAINER_NESTING
    #define MAX_CONTAINER_NESTING (8)
#endif

// One bit per kind of item, so asking whether a pizza box takes a pizza is a
// shift and an and rather than a walk down a list. Four cells for each build at
// the default ceiling of 128, which is two kilobytes for the whole catalogue.
#define CONTAINER_ACCEPT_CELLS ((_:MAX_ITEM_BUILDS + 31) / 32)

// What an inside looks like on disk, under "@inside".
//
// An item nobody has ever looked inside has no container yet, so there is
// nothing to write and no key is written: an empty array on every crate in the
// world would make all of them bigger on disk for nothing.
//
// The array goes onto the document first and is filled through it.
// JSON_ArrayAppend takes the node that holds the array and the key it is under,
// not the array itself -- appending to a loose array node reports nothing and
// drops everything, which is an "@inside":[] on a crate that was full.
//
// Each entry is written through SerializeItem, so it recurses: a bag inside a
// crate writes its own contents the same way, and the depth is whatever the
// catalogue allowed to be nested. A build that was never given a name cannot be
// written down at all, and writing the rest of it would load back as an item of
// no kind -- so it is skipped loudly rather than saved wrongly.
//
// Coming back, the inside is asked for only once there is something to put in
// it, because asking is what builds it. An item loaded with nothing inside
// stays a single cell. JSON_GetArray answers 0 on success, like the rest of the
// plugin, so the read is negated.
//
// A saved entry that has an inside of its own is refused on the way back in: it
// was written before that rule existed, and restoring it would put an item
// somewhere it could never be taken out of again. Each entry is given its own
// keys and its own inside before it goes in, because an item that cannot be put
// away is destroyed, and destroying a half-filled one would take its contents
// with it either way.

static
    gItemBuildContainerSize[MAX_ITEM_BUILDS],
    // The biggest single thing this kind of container will swallow. The size of
    // the thing itself is I\item's, because it is a property of the item and not
    // of anything that holds it; this is the other half of the comparison, and
    // it only exists where there is an inside to compare against.
    gItemBuildContainerMaxSize[MAX_ITEM_BUILDS] = { ITEM_BUILD_DEFAULT_SIZE, ... },
    gItemBuildAccepts[MAX_ITEM_BUILDS][CONTAINER_ACCEPT_CELLS],
    bool:gItemBuildRestricted[MAX_ITEM_BUILDS],
    Container:gItemContainer[MAX_ITEMS] = { INVALID_CONTAINER_ID, ... },

    // The other way round. Knowing which item an inside belongs to is what
    // makes it possible to walk up out of a container, and walking up is the
    // only way to answer whether putting something in would close a loop.
    Item:gContainerItem[MAX_CONTAINERS] = { INVALID_ITEM_ID, ... }
;

/**
 * # Functions
 */

/**
 * @brief      Say that a build has an inside, and how much fits in it.
 *
 * Nothing is created here. The container itself is built the first time
 * somebody looks inside one of these items.
 *
 * @param      buildid   Build to give an inside to.
 * @param      capacity     How many slots the inside has.
 * @param      maxItemSize  The biggest single thing it will swallow. The size
 *                          of the thing itself is I\item's, because that is a
 *                          property of the item and not of anything holding
 *                          it; this is the other half of the comparison.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the build now has an inside
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + `capacity` is zero or below
 */
forward bool:DefineItemBuildContainer(ItemBuild:buildid, capacity, maxItemSize = ITEM_BUILD_DEFAULT_SIZE);

/**
 * @brief      Narrow a container to one kind of item, one call at a time.
 *
 * A container that is never told anything takes whatever the rules allow. The
 * first call turns that off: from then on it takes only what it has been told,
 * so a pizza box holds eight slices and not eight rucksacks.
 *
 * This is also what keeps nesting honest without a rule about nesting. A
 * courier's rucksack takes anything, including pizza boxes; a pizza box takes
 * pizza. The shape that ends up possible is the shape somebody declared.
 *
 * @param      buildid   Container build to narrow.
 * @param      accepted  Build it will take.
 *
 * @date       01:30 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it now takes that kind
 *             - `false` when:
 *                 + either build is not registered
 *                 + `buildid` has no inside to put anything in
 */
forward bool:DefineItemBuildContainerAccepts(ItemBuild:buildid, ItemBuild:accepted);

/**
 * @brief      Whether a kind of container takes a kind of item.
 *
 * @param      buildid   Container build to ask.
 * @param      accepted  Build to ask about.
 *
 * @date       01:30 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it takes it, or takes everything
 *             - `false` it was narrowed and this is not on the list
 */
forward bool:DoesItemBuildContainerAccept(ItemBuild:buildid, ItemBuild:accepted);

/**
 * @brief      How many slots the inside of this kind of item has.
 *
 * @param      buildid   Build to read.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the capacity it was defined with
 *             - `0` when:
 *                 + `buildid` is not a registered build
 *                 + the build has no inside
 */
forward GetItemBuildContainerSize(ItemBuild:buildid);

/**
 * @brief      The container an item holds, building it if this is the first look.
 *
 * Nothing is allocated until somebody looks, so a crate that was never
 * opened costs one cell rather than a container.
 *
 * Most bags are never opened, and a container that exists costs a slot in
 * the framework's table -- so one is only made when somebody asks. It is
 * named after the item, so a locker list reads as the items themselves.
 *
 * The container is destroyed with the item, by the hook at the foot of
 * this file.
 *
 * @param      itemid    Item to look inside.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the `Container:` it holds
 *             - `INVALID_CONTAINER_ID` when:
 *                 + `itemid` is not a live item
 *                 + its build has no inside
 */
forward Container:GetItemInside(Item:itemid);

/**
 * @brief      Whether an item has an inside at all.
 *
 * Answered from the build, so asking does not build the container the way
 * GetItemInside does.
 *
 * @param      itemid    Item to check.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` its build was given a capacity
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + its build has no inside
 */
forward bool:IsItemContainer(Item:itemid);

/**
 * @brief      Whether an item may be carried in a character's pockets.
 *
 * Anything without an inside may. Nothing with one may, whatever its size: a
 * pizza box in a pocket is a pocket with eight more slots in it, and a
 * character carrying ten of them has an inventory ten times the size the number
 * on the screen says. They are carried in the hands, or inside something that
 * is itself being carried.
 *
 * This is about pockets only. Where else a thing may go is
 * CanPutItemInContainer's, and the answer there is often yes when it is no
 * here -- a rucksack goes in a bin and a pizza box goes in a rucksack.
 *
 * @param      itemid  Item to check.
 *
 * @date       01:30 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it goes in a pocket
 *             - `false` it has an inside of its own
 */
forward bool:CanStoreItem(Item:itemid);

/**
 * @brief      Which item an inside belongs to.
 *
 * The reverse of GetItemInside, and the step that makes walking up out of a
 * container possible.
 *
 * @param      containerid  Container to look up.
 *
 * @date       01:10 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - the item whose inside this is
 *             - `INVALID_ITEM_ID` when it belongs to no item: a character's
 *               pockets, a locker, anything standing on its own
 */
forward Item:GetContainerItem(Container:containerid);

/**
 * @brief      Whether an item may be put into a particular container.
 *
 * Four questions, in this order:
 *
 *   - would it end up inside itself? Walking up from the destination costs
 *     one step per box it is nested in, which is two or three, and this is
 *     the only rule that has to live here rather than in a catalogue: the
 *     others are balance, this one is the invariant
 *   - is it too big? Size is asked before the whitelist because it is the
 *     question a list of kinds cannot answer -- a crate and a crate are the
 *     same kind, so no whitelist could ever say that one does not go in the
 *     other
 *   - is it a kind this container takes? An inside takes what that kind of
 *     item was told to take, and a rucksack was told nothing, so it takes
 *     anything. That is what lets a courier carry pizza boxes in a rucksack
 *     without nesting being a rule of its own: what may go where is a
 *     sentence in the catalogue
 *   - what kind of place is it? Pockets take no containers at all -- not
 *     because it would break anything, the walk above sees to that, but
 *     because a rucksack in a pocket is a pocket with pockets. A locker
 *     belongs to neither rule: it is fixed in the world and can never be
 *     inside anything, so a bag thrown in a bin is still a bag somebody can
 *     open
 *
 * The walk is bounded by MAX_CONTAINER_NESTING. A loop that already existed
 * would walk forever, and while this is the only door in it cannot happen
 * -- which is exactly why the bound is worth having: the day something
 * writes one, this says so instead of hanging.
 *
 * Four rules, in the order they are asked:
 *
 *   - nothing goes inside itself, at any depth. This is the invariant, and the
 *     only one of the four that is not a matter of taste
 *   - the inside of an item takes nothing bigger than it was told to swallow,
 *     and then only the kinds it was told to take -- a build told nothing about
 *     kinds takes any kind that fits
 *   - pockets take no containers, so that slots cannot be multiplied by buying
 *     bags
 *   - a locker takes anything, being fixed in the world and inside nothing
 *
 * @param      itemid       Item being put away.
 * @param      containerid  Where it is going.
 *
 * @date       01:10 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it may go in
 *             - `false` when:
 *                 + either id is not live
 *                 + the container is inside the item, at any depth
 *                 + the item is too big for that kind of container
 */
forward bool:CanPutItemInContainer(Item:itemid, Container:containerid);

/**
 * @brief      Whether an item can be opened where it lies.
 *
 * Both halves have to be true: the build has to have an inside, and the item
 * has to be openable. Everything else ignores the open key.
 *
 * @param      itemid  Item to check.
 *
 * @date       00:20 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the open key works on it
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + it is not openable
 *                 + its build has no inside
 */
forward bool:IsItemOpenable(Item:itemid);

/**
 * # External
 */

stock bool:DefineItemBuildContainer(ItemBuild:buildid, capacity, maxItemSize = ITEM_BUILD_DEFAULT_SIZE) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    if (capacity <= 0) {
        return false;
    }

    if (maxItemSize < ITEM_BUILD_DEFAULT_SIZE) {
        return false;
    }

    gItemBuildContainerSize[buildid] = capacity;
    gItemBuildContainerMaxSize[buildid] = maxItemSize;

    return true;
}

stock bool:DefineItemBuildContainerAccepts(ItemBuild:buildid, ItemBuild:accepted) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    if (!IsValidItemBuild(accepted)) {
        return false;
    }

    if (gItemBuildContainerSize[buildid] == 0) {
        return false;
    }

    gItemBuildAccepts[buildid][_:accepted >>> 5] |= (1 << (_:accepted & 31));
    gItemBuildRestricted[buildid] = true;

    return true;
}

stock bool:DoesItemBuildContainerAccept(ItemBuild:buildid, ItemBuild:accepted) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    if (!IsValidItemBuild(accepted)) {
        return false;
    }

    if (!gItemBuildRestricted[buildid]) {
        return true;
    }

    return (gItemBuildAccepts[buildid][_:accepted >>> 5] & (1 << (_:accepted & 31))) != 0;
}

stock GetItemBuildContainerSize(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return 0;
    }

    return gItemBuildContainerSize[buildid];
}

stock Container:GetItemInside(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return INVALID_CONTAINER_ID;
    }

    if (IsValidContainer(gItemContainer[itemid])) {
        return gItemContainer[itemid];
    }

    new const
        capacity = GetItemBuildContainerSize(GetItemBuild(itemid))
    ;

    if (capacity == 0) {
        return INVALID_CONTAINER_ID;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    gItemContainer[itemid] = CreateContainer(name, capacity);

    if (IsValidContainer(gItemContainer[itemid])) {
        gContainerItem[gItemContainer[itemid]] = itemid;
    }

    return gItemContainer[itemid];
}

stock bool:IsItemContainer(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    return (GetItemBuildContainerSize(GetItemBuild(itemid)) != 0);
}

stock Item:GetContainerItem(Container:containerid) {
    if (!IsValidContainer(containerid)) {
        return INVALID_ITEM_ID;
    }

    return gContainerItem[containerid];
}

stock bool:CanStoreItem(Item:itemid) {
    return !IsItemContainer(itemid);
}

stock bool:CanPutItemInContainer(Item:itemid, Container:containerid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    if (!IsValidContainer(containerid)) {
        return false;
    }

    for (new Container:above = containerid, depth; IsValidContainer(above); ++depth) {
        new const
            Item:owner = GetContainerItem(above)
        ;

        if (owner == INVALID_ITEM_ID) {
            break;
        }

        if (owner == itemid) {
            return false;
        }

        if (depth > MAX_CONTAINER_NESTING) {
            printf("[container] %i is nested past the limit, so nothing may go in", _:containerid);

            return false;
        }

        new
            Container:parent = INVALID_CONTAINER_ID,
            index = -1
        ;

        if (!GetItemContainer(owner, parent, index)) {
            break;
        }

        above = parent;
    }

    new const
        Item:owner = GetContainerItem(containerid)
    ;

    if (owner != INVALID_ITEM_ID) {
        new const
            ItemBuild:ownerBuild = GetItemBuild(owner)
        ;

        if (GetItemBuildSize(GetItemBuild(itemid)) > gItemBuildContainerMaxSize[ownerBuild]) {
            return false;
        }

        return DoesItemBuildContainerAccept(ownerBuild, GetItemBuild(itemid));
    }

    if (GetContainerPlayer(containerid) != INVALID_PLAYER_ID) {
        return CanStoreItem(itemid);
    }

    return true;
}

stock bool:IsItemOpenable(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    if (!HasItemAttribute(itemid, ITEM_ATTRIBUTE_OPENABLE)) {
        return false;
    }

    return (GetItemBuildContainerSize(GetItemBuild(itemid)) != 0);
}

/**
 * # Saving
 *
 * What is inside an item is the one thing about it that is not a number, so it
 * is the one thing the flat map behind item extra data cannot hold -- json-plus
 * says so itself: a nested object or an array of objects has no map equivalent
 * and is left out rather than guessed at.
 *
 * It goes on the node instead. OnItemSerialize hands over the document itself,
 * not the map, so the contents are written straight into it as an array and
 * never pass through a map at all.
 *
 * None of this runs while anybody is playing. The inside is a cell in
 * gItemContainer for every read the game does; this is the one path that turns
 * that cell into text, and it is walked once per item saved.
 *
 * Each entry carries the build's registered name rather than its id, because
 * ids are handed out in registration order -- adding a kind of item above
 * another in a catalogue would otherwise turn every saved crate into a bag.
 */

#if !defined ITEM_CONTAINER_KEY
    #define ITEM_CONTAINER_KEY "@inside"
#endif

#if !defined ITEM_CONTAINER_BUILD_KEY
    #define ITEM_CONTAINER_BUILD_KEY "@build"
#endif

/**
 * # Calls
 */

hook OnItemSerialize(Item:itemid, Node:node) {
    new const
        Container:containerid = gItemContainer[itemid]
    ;

    if (!IsValidContainer(containerid)) {
        return 0;
    }

    if (IsContainerEmpty(containerid)) {
        return 0;
    }

    JSON_SetArray(node, ITEM_CONTAINER_KEY, JSON_Array());

    for (new i, size = GetContainerCapacity(containerid); i != size; ++i) {
        new const
            Item:contained = GetContainerSlotItem(containerid, i)
        ;

        if (contained == INVALID_ITEM_ID) {
            continue;
        }

        new
            key[MAX_ITEM_KEY_LENGTH]
        ;

        if (!GetItemBuildKey(GetItemBuild(contained), key)) {
            printf("[container] %i has no key, so it cannot be saved", _:GetItemBuild(contained));

            continue;
        }

        new const
            Node:entry = JSON_Object()
        ;

        JSON_SetString(entry, ITEM_CONTAINER_BUILD_KEY, key);

        SerializeItem(contained, entry);

        JSON_ArrayAppend(node, ITEM_CONTAINER_KEY, entry);
    }

    return 0;
}

hook OnItemDeserialize(Item:itemid, Node:node) {
    new
        Node:array
    ;

    if (JSON_GetArray(node, ITEM_CONTAINER_KEY, array)) {
        return 0;
    }

    new
        length
    ;

    if (JSON_ArrayLength(array, length)) {
        return 0;
    }

    new const
        Container:containerid = GetItemInside(itemid)
    ;

    if (containerid == INVALID_CONTAINER_ID) {
        return 0;
    }

    for (new i; i != length; ++i) {
        new
            Node:entry
        ;

        if (JSON_ArrayObject(array, i, entry)) {
            continue;
        }

        new
            key[MAX_ITEM_KEY_LENGTH]
        ;

        if (JSON_GetString(entry, ITEM_CONTAINER_BUILD_KEY, key)) {
            continue;
        }

        new const
            ItemBuild:buildid = GetKeyItemBuild(key)
        ;

        if (buildid == INVALID_ITEM_BUILD_ID) {
            printf("[container] no kind of item is registered as %s", key);

            continue;
        }

        if (GetItemBuildContainerSize(buildid) != 0) {
            printf("[container] %s has an inside and cannot be restored into one", key);

            continue;
        }

        new const
            Item:contained = CreateItem(buildid)
        ;

        if (contained == INVALID_ITEM_ID) {
            continue;
        }

        DeserializeItem(contained, entry);

        if (!AddItemToContainer(containerid, contained)) {
            DestroyItem(contained);
        }
    }

    return 0;
}

hook OnItemDestroy(Item:itemid) {
    if (IsValidContainer(gItemContainer[itemid])) {
        gContainerItem[gItemContainer[itemid]] = INVALID_ITEM_ID;

        DestroyContainer(gItemContainer[itemid]);
    }

    gItemContainer[itemid] = INVALID_CONTAINER_ID;

    return 0;
}
