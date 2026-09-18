#if defined _CORE_ITEM_BUILD_STACK
    #endinput
#endif
#define _CORE_ITEM_BUILD_STACK

#include <pp-hooks>

/**
 * # Stacking
 *
 * How much of a thing one slot is allowed to hold. Cash is the reason it exists:
 * without a ceiling a character carries their entire fortune in one slot and the
 * inventory stops being a constraint on anything.
 *
 * Only kinds that were given a limit stack at all. That is deliberate -- two
 * identical rifles are two rifles, and merging them into one slot holding "2"
 * would lose whatever each of them separately knew.
 *
 * The limit lives on the build rather than the item because it is a property of
 * the kind of thing: every wallet holds the same maximum, whatever is in it.
 */

new
    gItemBuildStackLimit[MAX_ITEM_BUILDS],
    gItemBuildStackStart[MAX_ITEM_BUILDS]
;

/**
 * # Functions
 */

/**
 * @brief      Say that a build counts rather than being counted, and how high.
 *
 * The limit is what stops one slot holding a fortune: money stacks to
 * whatever is set here and the rest stays where it was.
 *
 * @param      buildid   Build to make stackable.
 * @param      limit     Most that may sit in one slot.
 * @param      starts    How much a newly made one holds. Nothing means nothing,
 *                       which is right for a build whose amount is decided by
 *                       whoever made it and wrong for one found lying about.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the build now stacks
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + `limit` is zero or below
 */
forward bool:DefineItemBuildStack(ItemBuild:buildid, limit, starts = 0);

/**
 * @brief      Most that fits in one slot of this kind of item.
 *
 * @param      buildid   Build to read.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the limit it was defined with
 *             - `0` when:
 *                 + `buildid` is not a registered build
 *                 + the build does not stack
 */
forward GetItemBuildStackLimit(ItemBuild:buildid);

/**
 * @brief      Whether two of these can be poured into one.
 *
 * @param      itemid    Item to check.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` its build was given a stack limit
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + its build does not stack
 */
forward bool:IsItemStackable(Item:itemid);

/**
 * @brief      How much more this particular item will take.
 *
 * The limit less what it already holds, which is what a merge pours and
 * what it leaves behind: pouring 4000 into a wallet with 2000 of room
 * fills it and leaves 2000 in the other slot.
 *
 * @param      itemid    Item to measure.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the room left, one or more
 *             - `0` when:
 *                 + its build does not stack
 *                 + it is already at the limit
 */
forward GetItemStackSpace(Item:itemid);

/**
 * # External
 */

stock bool:DefineItemBuildStack(ItemBuild:buildid, limit, starts = 0) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    if (limit <= 0) {
        return false;
    }

    gItemBuildStackLimit[buildid] = limit;
    gItemBuildStackStart[buildid] = (starts > limit) ? limit : starts;

    return true;
}

/**
 * # Calls
 */

// A stack made with nothing in it is a stack of nothing, and the corner of the
// slot shows it as such -- which is what a bundle of cash out of a present
// looked like. Written on creation so that every path agrees: the admin, a
// locker being filled, a present being opened.
hook OnItemCreate(Item:itemid) {
    new const
        starts = gItemBuildStackStart[GetItemBuild(itemid)]
    ;

    if (starts == 0) {
        return 0;
    }

    SetItemAmountInt(itemid, starts);

    return 0;
}

stock GetItemBuildStackLimit(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return 0;
    }

    return gItemBuildStackLimit[buildid];
}

stock bool:IsItemStackable(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    return (GetItemBuildStackLimit(GetItemBuild(itemid)) != 0);
}

stock GetItemStackSpace(Item:itemid) {
    new const
        limit = GetItemBuildStackLimit(GetItemBuild(itemid))
    ;

    if (limit == 0) {
        return 0;
    }

    new const
        held = GetItemAmountInt(itemid)
    ;

    if (held >= limit) {
        return 0;
    }

    return limit - held;
}
