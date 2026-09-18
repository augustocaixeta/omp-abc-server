#if defined _INC_ITEM_ATTRIBUTES
    #endinput
#endif
#define _INC_ITEM_ATTRIBUTES

#include <open.mp>

#include <I\item>

/**
 * # Header
 */

// Flags carried by a build and read through its items: usable, openable,
// stackable, whatever the gamemode decides those words mean. The framework only
// stores and answers them, so what an attribute does is never its business.
//
// They live on the build rather than on the item because that is where the
// answer is the same for every copy, and it makes the whole system one cell per
// build -- 512 bytes at the default 128 builds -- instead of one per item. An
// item that needs to differ from its build wants its own key through
// I\PP\item-plus, not an attribute.
//
// Thirty two of them fit, one per bit of a cell. That is the ceiling, and it is
// a real one: a thirty third attribute needs a second cell per build here
// rather than anything on the calling side.
//
// Declare them shifting, never counting:
//
//     enum (<<= 1) {
//         ITEM_ATTR_USABLE = 1,
//         ITEM_ATTR_OPENABLE,
//         ITEM_ATTR_EQUIPPABLE
//     };
//
// A plain enum hands out 1, 2, 3 -- and 3 is the first two bits together, so
// the third name silently means "either of the other two" everywhere it is
// asked about. Nothing here can detect that, because 3 is a legitimate pair.

static
    gItemBuildAttributes[MAX_ITEM_BUILDS]
;

/**
 * # Functions
 */

/**
 * @brief      Replace every attribute of a build at once.
 *
 * Pass a set of flags ORed together, or 0 to clear them all.
 *
 * @param      buildid     Build to write.
 * @param      attributes  The whole new set.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the set was written
 *             - `false` `buildid` is not a registered build
 */
forward bool:SetItemBuildAttributes(ItemBuild:buildid, attributes);

/**
 * @brief      A build's whole set of attributes.
 *
 * A build that does not exist reads the same as a build with no attributes,
 * so ask IsValidItemBuild when the difference matters.
 *
 * @param      buildid     Build to read.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the set, as flags ORed together
 *             - `0` when:
 *                 + `buildid` is not a registered build
 *                 + the build carries no attributes
 */
forward GetItemBuildAttributes(ItemBuild:buildid);

/**
 * @brief      Turn flags on, leaving the rest of the set alone.
 *
 * @param      buildid     Build to write.
 * @param      attribute   Flag, or several ORed together.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they are on
 *             - `false` `buildid` is not a registered build
 */
forward bool:AddItemBuildAttribute(ItemBuild:buildid, attribute);

/**
 * @brief      Turn flags off, leaving the rest of the set alone.
 *
 * @param      buildid     Build to write.
 * @param      attribute   Flag, or several ORed together.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they are off
 *             - `false` `buildid` is not a registered build
 */
forward bool:RemoveItemBuildAttribute(ItemBuild:buildid, attribute);

/**
 * @brief      Whether a build carries a flag.
 *
 * Asking about one flag is the ordinary case. Asking about several is the
 * same question as HasItemBuildAllAttributes and answers identically.
 *
 * @param      buildid     Build to ask.
 * @param      attribute   Flag to look for.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the build carries every flag passed
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + any of the flags is missing
 */
forward bool:HasItemBuildAttribute(ItemBuild:buildid, attribute);

/**
 * @brief      Whether a build carries every one of these flags.
 *
 * @param      buildid     Build to ask.
 * @param      attributes  Flags ORed together.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` all of them are on
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + any one of them is missing
 */
forward bool:HasItemBuildAllAttributes(ItemBuild:buildid, attributes);

/**
 * @brief      Whether a build carries at least one of these flags.
 *
 * @param      buildid     Build to ask.
 * @param      attributes  Flags ORed together.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` one or more of them is on
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + none of them is on
 */
forward bool:HasItemBuildAnyAttribute(ItemBuild:buildid, attributes);

/**
 * @brief      An item's whole set of attributes, read from its build.
 *
 * An item that does not exist has no build, and the build side refuses
 * that, so one check covers both -- these four need no validity check of
 * their own first.
 *
 * @param      itemid      Item to read.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the set, as flags ORed together
 *             - `0` when:
 *                 + `itemid` is not a live item
 *                 + its build carries no attributes
 */
forward GetItemAttributes(Item:itemid);

/**
 * @brief      Whether an item carries a flag.
 *
 * @param      itemid      Item to ask.
 * @param      attribute   Flag to look for.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` its build carries every flag passed
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + any of the flags is missing
 */
forward bool:HasItemAttribute(Item:itemid, attribute);

/**
 * @brief      Whether an item carries every one of these flags.
 *
 * @param      itemid      Item to ask.
 * @param      attributes  Flags ORed together.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` all of them are on
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + any one of them is missing
 */
forward bool:HasItemAllAttributes(Item:itemid, attributes);

/**
 * @brief      Whether an item carries at least one of these flags.
 *
 * @param      itemid      Item to ask.
 * @param      attributes  Flags ORed together.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` one or more of them is on
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + none of them is on
 */
forward bool:HasItemAnyAttribute(Item:itemid, attributes);

/**
 * # External
 */

stock bool:SetItemBuildAttributes(ItemBuild:buildid, attributes) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    gItemBuildAttributes[buildid] = attributes;

    return true;
}

stock GetItemBuildAttributes(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return 0;
    }

    return gItemBuildAttributes[buildid];
}

stock bool:AddItemBuildAttribute(ItemBuild:buildid, attribute) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    gItemBuildAttributes[buildid] |= attribute;

    return true;
}

stock bool:RemoveItemBuildAttribute(ItemBuild:buildid, attribute) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    gItemBuildAttributes[buildid] &= ~attribute;

    return true;
}

stock bool:HasItemBuildAttribute(ItemBuild:buildid, attribute) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    return (gItemBuildAttributes[buildid] & attribute) == attribute;
}

stock bool:HasItemBuildAllAttributes(ItemBuild:buildid, attributes) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    return (gItemBuildAttributes[buildid] & attributes) == attributes;
}

stock bool:HasItemBuildAnyAttribute(ItemBuild:buildid, attributes) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    return (gItemBuildAttributes[buildid] & attributes) != 0;
}

stock GetItemAttributes(Item:itemid) {
    return GetItemBuildAttributes(GetItemBuild(itemid));
}

stock bool:HasItemAttribute(Item:itemid, attribute) {
    return HasItemBuildAttribute(GetItemBuild(itemid), attribute);
}

stock bool:HasItemAllAttributes(Item:itemid, attributes) {
    return HasItemBuildAllAttributes(GetItemBuild(itemid), attributes);
}

stock bool:HasItemAnyAttribute(Item:itemid, attributes) {
    return HasItemBuildAnyAttribute(GetItemBuild(itemid), attributes);
}
