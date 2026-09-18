#if defined _CATALOGUE_PERSONAL
    #endinput
#endif
#define _CATALOGUE_PERSONAL

#include <pp-hooks>

/**
 * # What a character carries
 *
 * The roleplay half of the catalogue, and the reason attributes are a set
 * rather than a type. None of these is usable, holdable or equippable in the
 * same sense as the rest:
 *
 *   Papers are read and shown, never dropped. A character who can leave their
 *   identity on a pavement is a character an administrator spends the evening
 *   restoring, so ITEM_ATTRIBUTE_DROPPABLE is simply absent and the row never draws.
 *
 *   A key is the same, and is handed over rather than dropped: giving someone a
 *   key to somewhere is a roleplay act, losing one is an accident.
 *
 *   Cash is the only thing here that stacks, which is what SPLIT is for and the
 *   reason that action exists at all.
 *
 * Papers and a house key belong to one character each: they are handed over,
 * never found. Cash and the box itself turn up.
 *
 * # The gift boxes
 *
 * A week of paper, with nothing named inside -- so what comes out is drawn from
 * everything lootable at the moment it is opened, and a present found in a
 * crate is not the same present as the one on a shelf.
 *
 * Five of them, one per rarity, which is the one place in the catalogue where
 * rarity is the item rather than a note about it: a gold box is not a box that
 * happens to be rare, it is the thing somebody is hoping to find. They are the
 * same present underneath for now -- what a legendary one should be worth
 * opening is a decision nobody has made, and GetItemBuildRarity is what will
 * answer it when somebody does.
 *
 * # The house key
 *
 * Its amount is whatever it opens, written on it: `HOUSE #102`, `INFERNUS #1`.
 * It stays blank until something says so, because a key that belongs to nothing
 * has nothing to say, and the housing that would name it does not exist yet.
 * SetItemAmountString is all it will take.
 *
 * # Cash
 *
 * Five thousand to a slot, and a bundle found somewhere is two hundred and
 * fifty of it. Whatever puts cash in the world later sets its own figure after
 * creating it; the second number is what an unattended one is worth.
 */

new
    ItemBuild:gItemBuildPapers        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildHouseKey      = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildCash          = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGiftCommon    = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGiftUncommon  = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGiftRare      = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGiftEpic      = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGiftLegendary = INVALID_ITEM_BUILD_ID
;

hook OnGameModeInit() {
    gItemBuildPapers        = BuildItem("Identity Papers", 1279, 1, 90.0);
    gItemBuildHouseKey      = BuildItem("House Key", 11746, 1, 90.0, 0.0, 0.0, 0.08);
    gItemBuildCash          = BuildItem("Cash", 1212, 1);
    gItemBuildGiftCommon    = BuildItem("Common Gift", 19054, 1, 0.0, 0.0, 0.0, 0.0, 0.114177, 0.089762, -0.173014, 247.160079, 354.746368, 79.219100);
    gItemBuildGiftUncommon  = BuildItem("Uncommon Gift", 19057, 1, 0.0, 0.0, 0.0, 0.0, 0.114177, 0.089762, -0.173014, 247.160079, 354.746368, 79.219100);
    gItemBuildGiftRare      = BuildItem("Rare Gift", 19055, 1, 0.0, 0.0, 0.0, 0.0, 0.114177, 0.089762, -0.173014, 247.160079, 354.746368, 79.219100);
    gItemBuildGiftEpic      = BuildItem("Epic Gift", 19056, 1, 0.0, 0.0, 0.0, 0.0, 0.114177, 0.089762, -0.173014, 247.160079, 354.746368, 79.219100);
    gItemBuildGiftLegendary = BuildItem("Legendary Gift", 19058, 1, 0.0, 0.0, 0.0, 0.0, 0.114177, 0.089762, -0.173014, 247.160079, 354.746368, 79.219100);

    SetKeyItemBuild("papers", gItemBuildPapers);
    SetKeyItemBuild("house-key", gItemBuildHouseKey);
    SetKeyItemBuild("cash", gItemBuildCash);
    SetKeyItemBuild("gift-common", gItemBuildGiftCommon);
    SetKeyItemBuild("gift-uncommon", gItemBuildGiftUncommon);
    SetKeyItemBuild("gift-rare", gItemBuildGiftRare);
    SetKeyItemBuild("gift-epic", gItemBuildGiftEpic);
    SetKeyItemBuild("gift-legendary", gItemBuildGiftLegendary);

    SetItemBuildAttributes(gItemBuildPapers, ITEM_ATTRIBUTE_GIVEABLE);
    SetItemBuildAttributes(gItemBuildHouseKey, ITEM_ATTRIBUTE_USABLE | ITEM_ATTRIBUTE_GIVEABLE);
    SetItemBuildAttributes(gItemBuildCash, ITEM_ATTRIBUTE_SPLITTABLE | ITEM_ATTRIBUTE_GIVEABLE | ITEM_ATTRIBUTE_DROPPABLE | ITEM_ATTRIBUTE_LOOTABLE);
    new const
        giftAttributes = ITEM_ATTRIBUTE_USABLE | ITEM_ATTRIBUTE_GIVEABLE | ITEM_ATTRIBUTE_LOOTABLE
    ;

    SetItemBuildAttributes(gItemBuildGiftCommon, giftAttributes);
    SetItemBuildAttributes(gItemBuildGiftUncommon, giftAttributes);
    SetItemBuildAttributes(gItemBuildGiftRare, giftAttributes);
    SetItemBuildAttributes(gItemBuildGiftEpic, giftAttributes);
    SetItemBuildAttributes(gItemBuildGiftLegendary, giftAttributes);

    DefineItemBuildGift(gItemBuildGiftCommon, .life = DDUR(7));
    DefineItemBuildGift(gItemBuildGiftUncommon, .life = DDUR(7));
    DefineItemBuildGift(gItemBuildGiftRare, .life = DDUR(7));
    DefineItemBuildGift(gItemBuildGiftEpic, .life = DDUR(7));
    DefineItemBuildGift(gItemBuildGiftLegendary, .life = DDUR(7));

    SetItemBuildRarity(gItemBuildGiftCommon, RARITY_COMMON);
    SetItemBuildRarity(gItemBuildGiftUncommon, RARITY_UNCOMMON);
    SetItemBuildRarity(gItemBuildGiftRare, RARITY_RARE);
    SetItemBuildRarity(gItemBuildGiftEpic, RARITY_EPIC);
    SetItemBuildRarity(gItemBuildGiftLegendary, RARITY_LEGENDARY);

    SetItemBuildAmountType(gItemBuildHouseKey, ITEM_AMOUNT_CUSTOM);
    SetItemBuildAmountType(gItemBuildCash, ITEM_AMOUNT_INT, "$");

    DefineItemBuildStack(gItemBuildCash, 5000, 250);

    SetItemBuildPreviewSettings(gItemBuildPapers, 35.0, 35.0, -15.0, 0.0, 45.0, 1.0, 2.5, -2.0);
    SetItemBuildPreviewSettings(gItemBuildHouseKey, 48.0, 48.0, 180.0, 135.0, 0.0, 1.0, 1.0, -0.5);
    SetItemBuildPreviewSettings(gItemBuildCash, 30.0, 30.0, -15.0, 0.0, 35.0, 1.0, 0.0, -0.5);
    SetItemBuildPreviewSettings(gItemBuildGiftCommon, 35.0, 35.0, -15.0, 0.0, 45.0, 1.5, 0.5, -2.0);
    SetItemBuildPreviewSettings(gItemBuildGiftUncommon, 35.0, 35.0, -15.0, 0.0, 45.0, 1.5, 0.5, -2.0);
    SetItemBuildPreviewSettings(gItemBuildGiftRare, 35.0, 35.0, -15.0, 0.0, 45.0, 1.5, 0.5, -2.0);
    SetItemBuildPreviewSettings(gItemBuildGiftEpic, 35.0, 35.0, -15.0, 0.0, 45.0, 1.5, 0.5, -2.0);
    SetItemBuildPreviewSettings(gItemBuildGiftLegendary, 35.0, 35.0, -15.0, 0.0, 45.0, 1.5, 0.5, -2.0);

    return 0;
}
