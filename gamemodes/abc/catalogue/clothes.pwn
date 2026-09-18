#if defined _CATALOGUE_CLOTHES
    #endinput
#endif
#define _CATALOGUE_CLOTHES

#include <pp-hooks>

/**
 * # Outfits
 *
 * The list a package of clothes can hold one of. Ported from ScavengeSurvive's,
 * minus its spawn chances: those decide what turns up in a package found lying
 * about, and here a package comes from a shop or a wardrobe, where somebody
 * chose what is in it.
 *
 * The order is the Clothes, so nothing above an entry may be removed without
 * moving every character wearing one below it. Add at the end.
 *
 * Each kind is written down under a name rather than a build id: ids are
 * handed out in the order these calls run, so putting a new one above another
 * here would turn every saved item of one kind into the other.
 */

new
    ItemBuild:gItemBuildClothes = INVALID_ITEM_BUILD_ID
;

hook OnGameModeInit() {
    gItemBuildClothes = BuildItem("Clothes", 2891, 1, 0.0, 0.0, 0.0, 0.0, 0.269091, 0.166367, 0.000000, 90.000000, 0.000000, 0.000000);

    SetKeyItemBuild("clothes", gItemBuildClothes);
    SetItemBuildAttributes(gItemBuildClothes, ITEM_ATTRIBUTE_HOLDABLE | ITEM_ATTRIBUTE_GIVEABLE | ITEM_ATTRIBUTE_DROPPABLE | ITEM_ATTRIBUTE_LOOTABLE);
    SetItemBuildPreviewSettings(gItemBuildClothes, 30.0, 30.0, 0.0, 0.0, 0.0, 1.0, 0.0, -0.5);
    DefineItemBuildClothes(gItemBuildClothes);

    DefineClothes(60 , "Civilian", GENDER_MALE);
    DefineClothes(170, "Civilian", GENDER_MALE);
    DefineClothes(250, "Civilian", GENDER_MALE);
    DefineClothes(188, "Civilian", GENDER_MALE);
    DefineClothes(206, "Civilian", GENDER_MALE);
    DefineClothes(44 , "Civilian", GENDER_MALE);
    DefineClothes(289, "Zero"    , GENDER_MALE, false, false);
    DefineClothes(50 , "Mechanic", GENDER_MALE);
    DefineClothes(254, "Biker"   , GENDER_MALE);
    DefineClothes(283, "Cop"     , GENDER_MALE, false, false);
    DefineClothes(287, "Military", GENDER_MALE, false, true);
    DefineClothes(285, "S.W.A.T.", GENDER_MALE, false, false);
    DefineClothes(141, "Business", GENDER_MALE);

    DefineClothes(192, "Civilian", GENDER_FEMALE);
    DefineClothes(93 , "Civilian", GENDER_FEMALE);
    DefineClothes(233, "Civilian", GENDER_FEMALE);
    DefineClothes(193, "Civilian", GENDER_FEMALE);
    DefineClothes(90 , "Civilian", GENDER_FEMALE, false, false);
    DefineClothes(195, "Civilian", GENDER_FEMALE);
    DefineClothes(131, "Indian"  , GENDER_FEMALE);
    DefineClothes(157, "Country" , GENDER_FEMALE, false, false);
    DefineClothes(191, "Military", GENDER_FEMALE);

    return 0;
}
