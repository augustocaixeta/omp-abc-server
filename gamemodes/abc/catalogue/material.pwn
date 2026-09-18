#if defined _CATALOGUE_MATERIAL
    #endinput
#endif
#define _CATALOGUE_MATERIAL

#include <pp-hooks>

/**
 * # What things are made of
 *
 * Stock rather than equipment: none of it is used on its own, and each is a
 * count in a slot that a recipe spends. They stack, and one found or spawned
 * is one of it.
 */

new
    ItemBuild:gItemBuildMetal  = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildPowder = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildPrimer = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildRock   = INVALID_ITEM_BUILD_ID
;

hook OnGameModeInit() {
    gItemBuildMetal  = BuildItem("Scrap Metal", 19843, 2);
    gItemBuildPowder = BuildItem("Gunpowder", 2057, 1);
    gItemBuildPrimer = BuildItem("Primers", 2039, 1);
    gItemBuildRock   = BuildItem("Rock", 2936, 2);

    SetKeyItemBuild("metal", gItemBuildMetal);
    SetKeyItemBuild("powder", gItemBuildPowder);
    SetKeyItemBuild("primer", gItemBuildPrimer);
    SetKeyItemBuild("rock", gItemBuildRock);

    new const
        materialAttributes = ITEM_ATTRIBUTE_GIVEABLE | ITEM_ATTRIBUTE_LOOTABLE | ITEM_ATTRIBUTE_DISCARDABLE
    ;

    SetItemBuildAttributes(gItemBuildMetal, materialAttributes);
    SetItemBuildAttributes(gItemBuildPowder, materialAttributes);
    SetItemBuildAttributes(gItemBuildPrimer, materialAttributes);
    SetItemBuildAttributes(gItemBuildRock, materialAttributes);

    DefineItemBuildStack(gItemBuildMetal, 50, 1);
    DefineItemBuildStack(gItemBuildPowder, 50, 1);
    DefineItemBuildStack(gItemBuildPrimer, 50, 1);
    DefineItemBuildStack(gItemBuildRock, 10, 1);

    SetItemBuildPreviewSettings(gItemBuildMetal, 35.0, 35.0, -15.0, 0.0, 45.0, 1.25, 0.0, 1.0);
    SetItemBuildPreviewSettings(gItemBuildPowder, 35.0, 35.0, -15.0, 0.0, 235.0, 1.20, 1.0, 0.0);
    SetItemBuildPreviewSettings(gItemBuildPrimer, 35.0, 35.0, -15.0, 0.0, 235.0, 1.00, 1.0, 0.0);
    SetItemBuildPreviewSettings(gItemBuildRock, 35.0, 35.0, 0.0, 0.0, 0.0, 1.0, 0.0, 3.0);

    return 0;
}
