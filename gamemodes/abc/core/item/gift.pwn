#if defined _CORE_ITEM_GIFT
    #endinput
#endif
#define _CORE_ITEM_GIFT

#include <pp-hooks>

/**
 * # Wrapped presents
 *
 * A gift is not a container. It looks like one and it is tempting to make it
 * one, but a container is a place things are kept and a gift is a thing that
 * turns into something else: it is opened once, it is gone, and what was inside
 * is now the character's.
 *
 * That difference is the whole reason this file exists rather than a capacity.
 * A container has to be carried in the hands and can never sit in a pocket,
 * because pockets full of containers is pockets without a limit. A gift has no
 * inside to nest, so it lives in a pocket like anything else, is handed to
 * somebody, and is never left on a pavement.
 *
 * What a gift turns into is declared in the catalogue rather than picked at
 * random. Somebody wrapped it, and they knew what they were wrapping.
 *
 * Declared with the tagged size, so an ItemBuild: indexes it and an ItemBuild:
 * comes back out, neither needing a cast.
 */

static
    ItemBuild:gItemBuildGift[MAX_ITEM_BUILDS] = { INVALID_ITEM_BUILD_ID, ... },

    // Kept apart from the build above, because a present with nothing named in
    // it is still a present -- the two cannot be told apart by the contents.
    bool:gItemBuildIsGift[MAX_ITEM_BUILDS],

    Seconds:gItemBuildGiftLife[MAX_ITEM_BUILDS]
;

/**
 * # Functions
 */

/**
 * @brief      Say that a build is a wrapped present, and what is inside it.
 *
 * A gift is not a container. What is inside is not stored anywhere and
 * cannot be reached: the build is a promise, and the item that keeps it
 * is made at the moment the paper comes off.
 *
 * Naming nothing is not a mistake: it is a present whose contents are drawn
 * from everything lootable at the moment it is opened, so two boxes off the
 * same shelf are not the same box, and adding a lootable kind to the
 * catalogue puts it in every present already lying about the world.
 *
 * What comes out goes straight into a pocket, and nothing with an inside
 * fits in one -- so naming a container is refused here, at boot, rather
 * than leaving an unwrappable present for somebody to find in play.
 *
 * @param      buildid   Build that is the wrapping.
 * @param      contains  Build that comes out of it, or nothing for a
 *                       present drawn when it is opened.
 * @param      life      How long before it is nothing but paper. Saying one
 *                       also says how it is displayed, for the same reason
 *                       food does.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the build now gives something
 *             - `false` either build is not registered
 */
forward bool:DefineItemBuildGift(ItemBuild:buildid, ItemBuild:contains = INVALID_ITEM_BUILD_ID, Seconds:life = Seconds:0);

/**
 * @brief      What this kind of present turns into.
 *
 * @param      buildid   Build to read.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the build that comes out of it
 *             - `INVALID_ITEM_BUILD_ID` when:
 *                 + `buildid` is not a registered build
 *                 + the build is not a present
 */
forward ItemBuild:GetItemBuildGift(ItemBuild:buildid);

/**
 * @brief      Whether an item is a present waiting to be opened.
 *
 * @param      itemid    Item to check.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` its build promises something
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + its build is not a present
 */
forward bool:IsItemGift(Item:itemid);

/**
 * @brief      Open a present: the wrapping is destroyed and its contents appear.
 *
 * The order matters. The contents are made and put away before the
 * wrapping is destroyed, and a present that cannot be handed over is
 * destroyed again rather than left loose -- so a full inventory costs the
 * character nothing, and no failure loses both halves.
 *
 * @param      playerid  Character opening it.
 * @param      itemid    The wrapped present.
 * @param      given     Written with the build that came out, on success or not.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the wrapping is gone and the contents are in their pockets
 *             - `false` when:
 *                 + the item is not a present
 *                 + the character has no room for what is inside
 *                 + the contents could not be made
 *                 + the contents were refused by the inventory
 */
forward bool:UnwrapItem(playerid, Item:itemid, &ItemBuild:given = INVALID_ITEM_BUILD_ID);

/**
 * # External
 */

stock bool:DefineItemBuildGift(ItemBuild:buildid, ItemBuild:contains = INVALID_ITEM_BUILD_ID, Seconds:life = Seconds:0) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    gItemBuildGiftLife[buildid] = life;

    if (life > Seconds:0) {
        SetItemBuildAmountType(buildid, ITEM_AMOUNT_DURATION);
    }

    if (contains == INVALID_ITEM_BUILD_ID) {
        gItemBuildGift[buildid] = INVALID_ITEM_BUILD_ID;
        gItemBuildIsGift[buildid] = true;

        return true;
    }

    if (!IsValidItemBuild(contains)) {
        return false;
    }

    if (GetItemBuildContainerSize(contains) != 0) {
        return false;
    }

    gItemBuildGift[buildid] = contains;
    gItemBuildIsGift[buildid] = true;

    return true;
}

stock ItemBuild:GetItemBuildGift(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return INVALID_ITEM_BUILD_ID;
    }

    return gItemBuildGift[buildid];
}

stock bool:IsItemGift(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    return gItemBuildIsGift[GetItemBuild(itemid)];
}

stock bool:UnwrapItem(playerid, Item:itemid, &ItemBuild:given = INVALID_ITEM_BUILD_ID) {
    if (!IsItemGift(itemid)) {
        return false;
    }

    given = GetItemBuildGift(GetItemBuild(itemid));

    if (given == INVALID_ITEM_BUILD_ID) {
        given = RandomItemBuild();
    }

    if (given == INVALID_ITEM_BUILD_ID) {
        return false;
    }

    if (IsPlayerInventoryFull(playerid)) {
        return false;
    }

    new const
        Item:contents = CreateItem(given)
    ;

    if (contents == INVALID_ITEM_ID) {
        return false;
    }

    if (!AddItemToInventory(playerid, contents)) {
        DestroyItem(contents);

        return false;
    }

    DestroyItem(itemid);

    return true;
}

/**
 * # Calls
 */

// A present is wrapped today. Written on creation so that every path that makes
// one agrees on how long the paper lasts.
hook OnItemCreate(Item:itemid) {
    new const
        Seconds:life = gItemBuildGiftLife[GetItemBuild(itemid)]
    ;

    if (life <= Seconds:0) {
        return 0;
    }

    SetItemDuration(itemid, life);

    return 0;
}
