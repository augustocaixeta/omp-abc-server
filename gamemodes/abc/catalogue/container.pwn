#if defined _CATALOGUE_CONTAINER
    #endinput
#endif
#define _CATALOGUE_CONTAINER

#include <pp-hooks>

/**
 * # Bags and crates
 *
 * The things actually carried in the hands, and the only kind here that
 * attaches an object to a bone. Their six attach numbers came from
 * ScavengeSurvive, which means they were tuned against a real client rather
 * than guessed -- see catalogue/weapon for how the two parameter lists line up.
 */

new
    ItemBuild:gItemBuildSmallBag   = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildBackpack   = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildSmallCrate = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildLargeCrate = INVALID_ITEM_BUILD_ID
;

hook OnGameModeInit() {
    gItemBuildSmallBag   = BuildItem("Small Bag", 363, 4, 270.0, 0.0, 0.0, 0.0, 0.052853, 0.034967, -0.177413, 0.000000, 261.397491, 349.759826);
    gItemBuildBackpack   = BuildItem("Backpack", 3026, 7, 270.0, 0.0, 90.0, 0.0, 0.470918, 0.150153, 0.055384, 181.319580, 7.513789, 163.436065);
    gItemBuildSmallCrate = BuildItem("Small Crate", 2969, 6, 0.0, 0.0, 0.0, 0.0, 0.114177, 0.089762, -0.173014, 247.160079, 354.746368, 79.219100, .useCarryAnim = true);
    gItemBuildLargeCrate = BuildItem("Large Crate", 1271, 11, 0.0, 0.0, 0.0, 0.3112, 0.050000, 0.334999, -0.327000, -23.900018, -10.200002, 11.799987, .useCarryAnim = true);

    SetKeyItemBuild("small-bag", gItemBuildSmallBag);
    SetKeyItemBuild("backpack", gItemBuildBackpack);
    SetKeyItemBuild("small-crate", gItemBuildSmallCrate);
    SetKeyItemBuild("large-crate", gItemBuildLargeCrate);

    new const
        bagAttributes = ITEM_ATTRIBUTE_HOLDABLE | ITEM_ATTRIBUTE_OPENABLE | ITEM_ATTRIBUTE_DROPPABLE | ITEM_ATTRIBUTE_GIVEABLE | ITEM_ATTRIBUTE_LOOTABLE
    ;

    SetItemBuildAttributes(gItemBuildSmallBag, bagAttributes);
    SetItemBuildAttributes(gItemBuildBackpack, bagAttributes);
    SetItemBuildAttributes(gItemBuildSmallCrate, ITEM_ATTRIBUTE_HOLDABLE | ITEM_ATTRIBUTE_OPENABLE | ITEM_ATTRIBUTE_DROPPABLE);
    SetItemBuildAttributes(gItemBuildLargeCrate, ITEM_ATTRIBUTE_HOLDABLE | ITEM_ATTRIBUTE_OPENABLE | ITEM_ATTRIBUTE_DROPPABLE);

    DefineItemBuildContainer(gItemBuildSmallBag, 6, 3);
    DefineItemBuildContainer(gItemBuildBackpack, 18, 6);
    DefineItemBuildContainer(gItemBuildSmallCrate, 12, 5);
    DefineItemBuildContainer(gItemBuildLargeCrate, 30, 10);

    SetItemBuildPreviewSettings(gItemBuildSmallBag, 35.0, 35.0, 0.0, 0.0, 180.0, 1.5, -6.0, -3.0);
    SetItemBuildPreviewSettings(gItemBuildBackpack, 75.0, 75.0, 180.0, 90.0, 0.0, 2.0, -11.5, 11.0);
    SetItemBuildPreviewSettings(gItemBuildSmallCrate, 35.0, 35.0, -15.0, 0.0, 45.0, 1.0, 0.5, -2.0);
    SetItemBuildPreviewSettings(gItemBuildLargeCrate, 35.0, 35.0, -15.0, 0.0, 45.0, 1.0, 0.5, -2.0);

    return 0;
}
