#if defined _CATALOGUE_GEAR
    #endinput
#endif
#define _CATALOGUE_GEAR

#include <pp-hooks>

/**
 * # Things worn rather than wielded
 *
 * Equipment a character carries that GivePlayerWeapon has no answer for. Body
 * armour and a jetpack are both put on, both change what the character can do,
 * and neither is a weapon id -- so neither belongs in the weapon catalogue
 * beside the goggles and the camera, which look like the same sort of thing and
 * are weapons 44, 45 and 43.
 *
 * That is the whole of the distinction and it is worth being strict about: a
 * weapon here is whatever the game will accept as one, not whatever a player
 * would call one. The night vision goggles are a weapon and the vest is not,
 * however little sense that makes outside the engine.
 *
 * # What they do not do yet
 *
 * Nothing. They are items with a name, a size and a model framed for the
 * inventory, and they are carried, dropped, given and looted like anything
 * else. Wearing them is not written, because what wearing them means is a
 * decision nobody has made:
 *
 *     armour    SetPlayerArmour on equip, and then what? Armour that comes off
 *               with the vest, or a vest that is spent once worn and gone --
 *               and whether damage taken is written back to the item.
 *     jetpack   SPECIAL_ACTION_USEJETPACK is not a thing a character holds, it
 *               is a state they are in, so it is closer to a vehicle than to an
 *               item in a pocket.
 *
 * Both want an item action of their own rather than EQUIP, which is the weapon
 * module's and arms a weapon slot. Neither is marked equippable for that
 * reason: an EQUIP row on a vest would be a row that does nothing.
 *
 * # Glasses and hats
 *
 * Which are worn in the other sense: an attached object on the head, not a
 * skin -- catalogue/clothes is a list of skins and a pair of sunglasses is not
 * one of those. They sit here until something can put them on, and they are
 * named by colour because that is the only thing that separates them: eight
 * pairs called "Glasses" on a shop counter is a shop counter nobody can buy
 * from.
 */

new
    ItemBuild:gItemBuildArmour            = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildJetpack           = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGlassesPinkTinge  = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGlassesBlueTint   = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGlassesPurpleTint = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGlassesPinkTint   = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGlassesRedTint    = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGlassesYellow     = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGlassesClear      = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGlassesBlack      = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildHatChristmas      = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildHatCluckinBell    = INVALID_ITEM_BUILD_ID
;

hook OnGameModeInit() {
    gItemBuildArmour            = BuildItem("Body Armour", 1242, 6, 90.0);
    gItemBuildJetpack           = BuildItem("Jetpack", 370, 9, 90.0);
    gItemBuildGlassesPinkTinge  = BuildItem("Pink Sunglasses", 19010, 1, 90.0);
    gItemBuildGlassesBlueTint   = BuildItem("Blue Sunglasses", 19023, 1, 90.0);
    gItemBuildGlassesPurpleTint = BuildItem("Purple Sunglasses", 19024, 1, 90.0);
    gItemBuildGlassesPinkTint   = BuildItem("Pink Glasses", 19025, 1, 90.0);
    gItemBuildGlassesRedTint    = BuildItem("Red Glasses", 19026, 1, 90.0);
    gItemBuildGlassesYellow     = BuildItem("Yellow Glasses", 19028, 1, 90.0);
    gItemBuildGlassesClear      = BuildItem("Clear Glasses", 19031, 1, 90.0);
    gItemBuildGlassesBlack      = BuildItem("Black Sunglasses", 19138, 1, 90.0);
    gItemBuildHatChristmas      = BuildItem("Christmas Hat", 19065, 1, 90.0);
    gItemBuildHatCluckinBell    = BuildItem("Cluckin' Bell Hat", 19137, 1, 90.0);

    SetKeyItemBuild("armour", gItemBuildArmour);
    SetKeyItemBuild("jetpack", gItemBuildJetpack);
    SetKeyItemBuild("glasses-pink-tinge", gItemBuildGlassesPinkTinge);
    SetKeyItemBuild("glasses-blue", gItemBuildGlassesBlueTint);
    SetKeyItemBuild("glasses-purple", gItemBuildGlassesPurpleTint);
    SetKeyItemBuild("glasses-pink", gItemBuildGlassesPinkTint);
    SetKeyItemBuild("glasses-red", gItemBuildGlassesRedTint);
    SetKeyItemBuild("glasses-yellow", gItemBuildGlassesYellow);
    SetKeyItemBuild("glasses-clear", gItemBuildGlassesClear);
    SetKeyItemBuild("glasses-black", gItemBuildGlassesBlack);
    SetKeyItemBuild("hat-christmas", gItemBuildHatChristmas);
    SetKeyItemBuild("hat-cluckin-bell", gItemBuildHatCluckinBell);

    new const
        gearAttributes = ITEM_ATTRIBUTE_GIVEABLE | ITEM_ATTRIBUTE_DROPPABLE | ITEM_ATTRIBUTE_LOOTABLE
    ;

    SetItemBuildAttributes(gItemBuildArmour, gearAttributes);
    SetItemBuildAttributes(gItemBuildJetpack, gearAttributes);
    SetItemBuildAttributes(gItemBuildGlassesPinkTinge, gearAttributes);
    SetItemBuildAttributes(gItemBuildGlassesBlueTint, gearAttributes);
    SetItemBuildAttributes(gItemBuildGlassesPurpleTint, gearAttributes);
    SetItemBuildAttributes(gItemBuildGlassesPinkTint, gearAttributes);
    SetItemBuildAttributes(gItemBuildGlassesRedTint, gearAttributes);
    SetItemBuildAttributes(gItemBuildGlassesYellow, gearAttributes);
    SetItemBuildAttributes(gItemBuildGlassesClear, gearAttributes);
    SetItemBuildAttributes(gItemBuildGlassesBlack, gearAttributes);
    SetItemBuildAttributes(gItemBuildHatChristmas, gearAttributes);
    SetItemBuildAttributes(gItemBuildHatCluckinBell, gearAttributes);

    SetItemBuildPreviewSettings(gItemBuildArmour, 35.0, 35.0, 0.0, 0.0, 0.0, 1.5, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildJetpack, 35.0, 35.0, 0.0, 0.0, 45.0, 2.5, 0.0, 1.0);
    SetItemBuildPreviewSettings(gItemBuildGlassesPinkTinge, 35.0, 35.0, -15.0, 0.0, 90.0, 1.0, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildGlassesBlueTint, 35.0, 35.0, -15.0, 0.0, 90.0, 1.0, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildGlassesPurpleTint, 35.0, 35.0, -15.0, 0.0, 90.0, 1.0, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildGlassesPinkTint, 35.0, 35.0, -15.0, 0.0, 90.0, 1.0, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildGlassesRedTint, 35.0, 35.0, -15.0, 0.0, 90.0, 1.0, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildGlassesYellow, 35.0, 35.0, -15.0, 0.0, 90.0, 1.0, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildGlassesClear, 35.0, 35.0, -15.0, 0.0, 90.0, 1.0, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildGlassesBlack, 35.0, 35.0, -15.0, 0.0, 90.0, 1.0, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildHatChristmas, 36.0, 35.0, 0.0, -30.0, 200.0, 1.25, 2.5, 5.0);
    SetItemBuildPreviewSettings(gItemBuildHatCluckinBell, 35.0, 35.0, -10.0, 270.0, 0.0, 1.1, 0.0, 2.0);

    return 0;
}
