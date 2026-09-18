#if defined _CATALOGUE_WEAPON
    #endinput
#endif
#define _CATALOGUE_WEAPON

#include <pp-hooks>

/**
 * # Weapons
 *
 * Ported from ScavengeSurvive's item table. SIF's DefineItemType is where this
 * framework's BuildItem comes from, so the numbers carry across, but the two
 * parameter lists are not the same shape:
 *
 *   DefineItemType       BuildItem
 *   name                 name
 *   uname                --  SIF's script-facing unique name
 *   model                modelid
 *   size                 --  how many inventory slots the item costs
 *   rotx roty rotz       objRotOffsetX/Y/Z, the rotation of the object lying in the world
 *   modelz               objOffsetZ, lifting it out of the ground
 *   attx atty attz       attachX/Y/Z
 *   attrx attry attrz    attachRotX/Y/Z
 *   usecarryanim         useCarryAnim, positional there and named here
 *   colour               materialColour1, of which BuildItem takes two
 *   boneid               bone
 *   longpickup           --  a slower pick up animation for large things
 *   buttonz              buttonOffsetZ
 *   maxhitpoints         --  durability
 *
 * Every weapon here is 90 degrees on one axis and nothing else. That is not a
 * shortcut: rotx is how the object lies on the ground, and gun models are built
 * standing upright. The six attach numbers are absent for the same reason SIF
 * leaves them out -- a weapon is never attached to a bone, it is armed.
 *
 * Each kind is written down under a name rather than a build id: ids are
 * handed out in the order these calls run, so putting a new one above another
 * here would turn every saved item of one kind into the other -- were it not
 * for the name, which is what a save actually carries.
 *
 * That is what lets these stand in weapon id order rather than in the order
 * they happened to be written, and a new one belongs at its own number rather
 * than at the end. The order is a table of contents: the ids are the game's,
 * nothing here is free to choose them, and a gap in the run is a weapon this
 * gamemode has no item for yet.
 *
 * # Calibres
 *
 * The order these are declared in is the `Calibre:`, and a weapon remembers the
 * number rather than the name -- so nothing above an entry may be removed. Add
 * at the end.
 *
 * A calibre is short on purpose. It is drawn in the corner of a slot beside the
 * number, so the two together have to fit a square: "150X 7.62MM" fits and
 * "150X SEVEN POINT SIX TWO MILLIMETRE" is the number pushed off the edge.
 * GAS. is gasoline for that reason and not for any other.
 *
 * Gasoline is a calibre like any other. A chainsaw has to have the right thing
 * in it before the trigger does anything, which is the whole of what a calibre
 * means here, and core/item/calibre is what lets a liquid be one without a
 * `Liquid:` having to pretend to be a `Calibre:`.
 *
 * # What goes in what
 *
 * A weapon is declared with its calibre and how many rounds it can hold at
 * once. That is not a magazine: San Andreas keeps the clip itself and refills
 * it as the character reloads, so an AK-47 at 150 is one somebody can be
 * carrying 150 rounds inside, thirty of which are chambered at any moment.
 *
 * A box of ammunition is declared with its calibre and how many rounds a full
 * one holds. Nothing says which box goes in which gun: the calibre already did.
 */

new
    ItemBuild:gItemBuildKnuckles       = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGolfClub       = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildNightstick     = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildKnife          = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildBat            = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildShovel         = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildPoolCue        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildKatana         = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildChainsaw       = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildDildo          = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildDildoWhite     = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildVibrator       = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildVibratorSilver = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildFlowers        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildCane           = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGrenade        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildTearGas        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildMolotov        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildPistol         = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildSilenced       = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildDesertEagle    = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildShotgun        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildSawnOff        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildCombatShotgun  = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildMicroSMG       = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildMP5            = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildAK47           = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildM4             = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildTec9           = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildRifle          = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildSniper         = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildRocket         = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildHeatSeeker     = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildFlamethrower   = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildMinigun        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildSprayCan       = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildExtinguisher   = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildCamera         = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildNightVision    = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildThermal        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildParachute      = INVALID_ITEM_BUILD_ID,

    ItemBuild:gItemBuildAmmo9mm        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildAmmo357        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildAmmo12g        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildAmmo556        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildAmmo762        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildAmmoRocket     = INVALID_ITEM_BUILD_ID
;

new
    Calibre:gCalibre9mm     = INVALID_CALIBRE_ID,
    Calibre:gCalibre357     = INVALID_CALIBRE_ID,
    Calibre:gCalibre12g     = INVALID_CALIBRE_ID,
    Calibre:gCalibre556     = INVALID_CALIBRE_ID,
    Calibre:gCalibre762     = INVALID_CALIBRE_ID,
    Calibre:gCalibreGas     = INVALID_CALIBRE_ID,

    Calibre:gCalibreRocket  = INVALID_CALIBRE_ID,
    Calibre:gCalibreGrenade = INVALID_CALIBRE_ID,
    Calibre:gCalibreTear    = INVALID_CALIBRE_ID,
    Calibre:gCalibreMolotov = INVALID_CALIBRE_ID,

    Calibre:gCalibrePaint   = INVALID_CALIBRE_ID,
    Calibre:gCalibreFoam    = INVALID_CALIBRE_ID
;

hook OnGameModeInit() {
    gItemBuildKnuckles       = BuildItem("Brass Knuckles", 331, 1, 90.0);
    gItemBuildGolfClub       = BuildItem("Golf Club", 333, 6, 90.0);
    gItemBuildNightstick     = BuildItem("Nightstick", 334, 4, 90.0);
    gItemBuildKnife          = BuildItem("Combat Knife", 335, 3, 90.0);
    gItemBuildBat            = BuildItem("Baseball Bat", 336, 7, 90.0);
    gItemBuildShovel         = BuildItem("Shovel", 337, 6, 90.0);
    gItemBuildPoolCue        = BuildItem("Pool Cue", 338, 6, 90.0);
    gItemBuildKatana         = BuildItem("Katana", 339, 6, 90.0);
    gItemBuildChainsaw       = BuildItem("Chainsaw", 341, 7, 90.0);
    gItemBuildDildo          = BuildItem("Double-ended Dildo", 321, 2, 90.0);
    gItemBuildDildoWhite     = BuildItem("Dildo", 322, 2, 90.0);
    gItemBuildVibrator       = BuildItem("Vibrator", 323, 2, 90.0);
    gItemBuildVibratorSilver = BuildItem("Silver Vibrator", 324, 2, 90.0);
    gItemBuildFlowers        = BuildItem("Flowers", 325, 2, 90.0);
    gItemBuildCane           = BuildItem("Cane", 326, 4, 90.0);
    gItemBuildGrenade        = BuildItem("Grenade", 342, 2, 90.0);
    gItemBuildTearGas        = BuildItem("Tear Gas", 343, 2, 90.0);
    gItemBuildMolotov        = BuildItem("Molotov Cocktail", 344, 2, 90.0);
    gItemBuildPistol         = BuildItem("9mm Pistol", 346, 3, 90.0);
    gItemBuildSilenced       = BuildItem("Silenced Pistol", 347, 3, 90.0);
    gItemBuildDesertEagle    = BuildItem("Desert Eagle", 348, 4, 90.0);
    gItemBuildShotgun        = BuildItem("Pump Shotgun", 349, 9, 90.0);
    gItemBuildSawnOff        = BuildItem("Sawn-off Shotgun", 350, 6, 90.0);
    gItemBuildCombatShotgun  = BuildItem("Combat Shotgun", 351, 9, 90.0);
    gItemBuildMicroSMG       = BuildItem("Micro SMG", 352, 4, 90.0);
    gItemBuildMP5            = BuildItem("MP5", 353, 6, 90.0);
    gItemBuildAK47           = BuildItem("AK-47", 355, 9, 90.0);
    gItemBuildM4             = BuildItem("M4", 356, 9, 90.0);
    gItemBuildTec9           = BuildItem("TEC-9", 372, 4, 90.0);
    gItemBuildRifle          = BuildItem("Country Rifle", 357, 9, 90.0);
    gItemBuildSniper         = BuildItem("Sniper Rifle", 358, 9, 90.0);
    gItemBuildRocket         = BuildItem("RPG", 359, 9, 90.0);
    gItemBuildHeatSeeker     = BuildItem("Heat Seeker", 360, 9, 90.0);
    gItemBuildFlamethrower   = BuildItem("Flamethrower", 361, 9, 90.0);
    gItemBuildMinigun        = BuildItem("Minigun", 362, 9, 90.0);
    gItemBuildSprayCan       = BuildItem("Spray Can", 365, 2, 90.0);
    gItemBuildExtinguisher   = BuildItem("Fire Extinguisher", 366, 4, 90.0);
    gItemBuildCamera         = BuildItem("Camera", 367, 2, 90.0);
    gItemBuildNightVision    = BuildItem("Night Vision", 368, 2, 90.0);
    gItemBuildThermal        = BuildItem("Thermal Goggles", 369, 2, 90.0);
    gItemBuildParachute      = BuildItem("Parachute", 371, 6, 90.0);

    // A model each, so a glance at the slot says which calibre it is.
    gItemBuildAmmo9mm        = BuildItem("9mm Rounds", 19995, 1, 0.0, 0.0, 0.0, 0.02);
    gItemBuildAmmo357        = BuildItem(".357 Rounds", 2043, 1, 0.0, 0.0, 0.0, 0.02);
    gItemBuildAmmo12g        = BuildItem("12 Gauge Shells", 2358, 2, 0.0, 0.0, 0.0, 0.02);
    gItemBuildAmmo556        = BuildItem("5.56mm Rounds", 19832, 2, 0.0, 0.0, 0.0, 0.02);
    gItemBuildAmmo762        = BuildItem("7.62mm Rounds", 2037, 2, 0.0, 0.0, 0.0, 0.02);
    gItemBuildAmmoRocket     = BuildItem("Rockets", 3013, 6, 0.0, 0.0, 0.0, 0.02);

    SetKeyItemBuild("knuckles", gItemBuildKnuckles);
    SetKeyItemBuild("golfclub", gItemBuildGolfClub);
    SetKeyItemBuild("nightstick", gItemBuildNightstick);
    SetKeyItemBuild("knife", gItemBuildKnife);
    SetKeyItemBuild("bat", gItemBuildBat);
    SetKeyItemBuild("shovel", gItemBuildShovel);
    SetKeyItemBuild("poolcue", gItemBuildPoolCue);
    SetKeyItemBuild("katana", gItemBuildKatana);
    SetKeyItemBuild("chainsaw", gItemBuildChainsaw);
    SetKeyItemBuild("dildo", gItemBuildDildo);
    SetKeyItemBuild("dildo2", gItemBuildDildoWhite);
    SetKeyItemBuild("vibrator", gItemBuildVibrator);
    SetKeyItemBuild("vibrator2", gItemBuildVibratorSilver);
    SetKeyItemBuild("flowers", gItemBuildFlowers);
    SetKeyItemBuild("cane", gItemBuildCane);
    SetKeyItemBuild("grenade", gItemBuildGrenade);
    SetKeyItemBuild("teargas", gItemBuildTearGas);
    SetKeyItemBuild("molotov", gItemBuildMolotov);
    SetKeyItemBuild("pistol", gItemBuildPistol);
    SetKeyItemBuild("silenced", gItemBuildSilenced);
    SetKeyItemBuild("deagle", gItemBuildDesertEagle);
    SetKeyItemBuild("shotgun", gItemBuildShotgun);
    SetKeyItemBuild("sawnoff", gItemBuildSawnOff);
    SetKeyItemBuild("combatshotgun", gItemBuildCombatShotgun);
    SetKeyItemBuild("microsmg", gItemBuildMicroSMG);
    SetKeyItemBuild("mp5", gItemBuildMP5);
    SetKeyItemBuild("ak47", gItemBuildAK47);
    SetKeyItemBuild("m4", gItemBuildM4);
    SetKeyItemBuild("tec9", gItemBuildTec9);
    SetKeyItemBuild("rifle", gItemBuildRifle);
    SetKeyItemBuild("sniper", gItemBuildSniper);
    SetKeyItemBuild("rpg", gItemBuildRocket);
    SetKeyItemBuild("heatseeker", gItemBuildHeatSeeker);
    SetKeyItemBuild("flamethrower", gItemBuildFlamethrower);
    SetKeyItemBuild("minigun", gItemBuildMinigun);
    SetKeyItemBuild("spraycan", gItemBuildSprayCan);
    SetKeyItemBuild("extinguisher", gItemBuildExtinguisher);
    SetKeyItemBuild("camera", gItemBuildCamera);
    SetKeyItemBuild("nightvision", gItemBuildNightVision);
    SetKeyItemBuild("thermal", gItemBuildThermal);
    SetKeyItemBuild("parachute", gItemBuildParachute);

    SetKeyItemBuild("ammo-9mm", gItemBuildAmmo9mm);
    SetKeyItemBuild("ammo-357", gItemBuildAmmo357);
    SetKeyItemBuild("ammo-12g", gItemBuildAmmo12g);
    SetKeyItemBuild("ammo-556", gItemBuildAmmo556);
    SetKeyItemBuild("ammo-762", gItemBuildAmmo762);
    SetKeyItemBuild("ammo-rocket", gItemBuildAmmoRocket);

    new const
        weaponAttributes = ITEM_ATTRIBUTE_EQUIPPABLE | ITEM_ATTRIBUTE_GIVEABLE | ITEM_ATTRIBUTE_DROPPABLE | ITEM_ATTRIBUTE_LOOTABLE,
        ammunitionAttributes = ITEM_ATTRIBUTE_LOADABLE | ITEM_ATTRIBUTE_GIVEABLE | ITEM_ATTRIBUTE_DROPPABLE | ITEM_ATTRIBUTE_LOOTABLE,
        firearmAttributes = weaponAttributes | ITEM_ATTRIBUTE_UNLOADABLE
    ;

    SetItemBuildAttributes(gItemBuildKnuckles, weaponAttributes);
    SetItemBuildAttributes(gItemBuildGolfClub, weaponAttributes);
    SetItemBuildAttributes(gItemBuildNightstick, weaponAttributes);
    SetItemBuildAttributes(gItemBuildKnife, weaponAttributes);
    SetItemBuildAttributes(gItemBuildBat, weaponAttributes);
    SetItemBuildAttributes(gItemBuildShovel, weaponAttributes);
    SetItemBuildAttributes(gItemBuildPoolCue, weaponAttributes);
    SetItemBuildAttributes(gItemBuildKatana, weaponAttributes);
    SetItemBuildAttributes(gItemBuildChainsaw, weaponAttributes);
    SetItemBuildAttributes(gItemBuildDildo, weaponAttributes);
    SetItemBuildAttributes(gItemBuildDildoWhite, weaponAttributes);
    SetItemBuildAttributes(gItemBuildVibrator, weaponAttributes);
    SetItemBuildAttributes(gItemBuildVibratorSilver, weaponAttributes);
    SetItemBuildAttributes(gItemBuildFlowers, weaponAttributes);
    SetItemBuildAttributes(gItemBuildCane, weaponAttributes);
    SetItemBuildAttributes(gItemBuildGrenade, weaponAttributes);
    SetItemBuildAttributes(gItemBuildTearGas, weaponAttributes);
    SetItemBuildAttributes(gItemBuildMolotov, weaponAttributes);
    SetItemBuildAttributes(gItemBuildPistol, firearmAttributes);
    SetItemBuildAttributes(gItemBuildSilenced, firearmAttributes);
    SetItemBuildAttributes(gItemBuildDesertEagle, firearmAttributes);
    SetItemBuildAttributes(gItemBuildShotgun, firearmAttributes);
    SetItemBuildAttributes(gItemBuildSawnOff, firearmAttributes);
    SetItemBuildAttributes(gItemBuildCombatShotgun, firearmAttributes);
    SetItemBuildAttributes(gItemBuildMicroSMG, firearmAttributes);
    SetItemBuildAttributes(gItemBuildMP5, firearmAttributes);
    SetItemBuildAttributes(gItemBuildAK47, firearmAttributes);
    SetItemBuildAttributes(gItemBuildM4, firearmAttributes);
    SetItemBuildAttributes(gItemBuildTec9, firearmAttributes);
    SetItemBuildAttributes(gItemBuildRifle, firearmAttributes);
    SetItemBuildAttributes(gItemBuildSniper, firearmAttributes);
    SetItemBuildAttributes(gItemBuildRocket, firearmAttributes);
    SetItemBuildAttributes(gItemBuildHeatSeeker, firearmAttributes);
    SetItemBuildAttributes(gItemBuildFlamethrower, weaponAttributes);
    SetItemBuildAttributes(gItemBuildMinigun, firearmAttributes);
    SetItemBuildAttributes(gItemBuildSprayCan, weaponAttributes);
    SetItemBuildAttributes(gItemBuildExtinguisher, weaponAttributes);
    SetItemBuildAttributes(gItemBuildCamera, weaponAttributes);
    SetItemBuildAttributes(gItemBuildNightVision, weaponAttributes);
    SetItemBuildAttributes(gItemBuildThermal, weaponAttributes);
    SetItemBuildAttributes(gItemBuildParachute, weaponAttributes);

    SetItemBuildAttributes(gItemBuildAmmo9mm, ammunitionAttributes);
    SetItemBuildAttributes(gItemBuildAmmo357, ammunitionAttributes);
    SetItemBuildAttributes(gItemBuildAmmo12g, ammunitionAttributes);
    SetItemBuildAttributes(gItemBuildAmmo556, ammunitionAttributes);
    SetItemBuildAttributes(gItemBuildAmmo762, ammunitionAttributes);
    SetItemBuildAttributes(gItemBuildAmmoRocket, ammunitionAttributes);

    gCalibre9mm     = DefineCalibre("9mm");
    gCalibre357     = DefineCalibre(".357");
    gCalibre12g     = DefineCalibre("12G");
    gCalibre556     = DefineCalibre("5.56mm");
    gCalibre762     = DefineCalibre("7.62mm");
    gCalibreGas     = DefineCalibre("GAS.", gLiquidGasoline);
    gCalibreRocket  = DefineCalibre("ROCKET");
    gCalibreGrenade = DefineCalibre("GRENADE");
    gCalibreTear    = DefineCalibre("TEAR");
    gCalibreMolotov = DefineCalibre("MOLOTOV");
    gCalibrePaint   = DefineCalibre("PAINT", gLiquidPaint);
    gCalibreFoam    = DefineCalibre("FOAM", gLiquidFoam);

    DefineItemBuildAmmunition(gItemBuildAmmo9mm, gCalibre9mm, 68);
    DefineItemBuildAmmunition(gItemBuildAmmo357, gCalibre357, 12);
    DefineItemBuildAmmunition(gItemBuildAmmo12g, gCalibre12g, 16);
    DefineItemBuildAmmunition(gItemBuildAmmo556, gCalibre556, 30);
    DefineItemBuildAmmunition(gItemBuildAmmo762, gCalibre762, 30);
    DefineItemBuildAmmunition(gItemBuildAmmoRocket, gCalibreRocket, 5);

    DefineItemBuildWeapon(gItemBuildKnuckles, WEAPON_BRASSKNUCKLE);
    DefineItemBuildWeapon(gItemBuildGolfClub, WEAPON_GOLFCLUB);
    DefineItemBuildWeapon(gItemBuildNightstick, WEAPON_NITESTICK);
    DefineItemBuildWeapon(gItemBuildKnife, WEAPON_KNIFE);
    DefineItemBuildWeapon(gItemBuildBat, WEAPON_BAT);
    DefineItemBuildWeapon(gItemBuildShovel, WEAPON_SHOVEL);
    DefineItemBuildWeapon(gItemBuildPoolCue, WEAPON_POOLSTICK);
    DefineItemBuildWeapon(gItemBuildKatana, WEAPON_KATANA);
    DefineItemBuildWeapon(gItemBuildChainsaw, WEAPON_CHAINSAW, gCalibreGas, 2000);
    DefineItemBuildWeapon(gItemBuildDildo, WEAPON_DILDO);
    DefineItemBuildWeapon(gItemBuildDildoWhite, WEAPON_DILDO2);
    DefineItemBuildWeapon(gItemBuildVibrator, WEAPON_VIBRATOR);
    DefineItemBuildWeapon(gItemBuildVibratorSilver, WEAPON_VIBRATOR2);
    DefineItemBuildWeapon(gItemBuildFlowers, WEAPON_FLOWER);
    DefineItemBuildWeapon(gItemBuildCane, WEAPON_CANE);
    DefineItemBuildWeapon(gItemBuildGrenade, WEAPON_GRENADE, gCalibreGrenade, 10);
    DefineItemBuildWeapon(gItemBuildTearGas, WEAPON_TEARGAS, gCalibreTear, 10);
    DefineItemBuildWeapon(gItemBuildMolotov, WEAPON_MOLTOV, gCalibreMolotov, 10);
    DefineItemBuildWeapon(gItemBuildPistol, WEAPON_COLT45, gCalibre9mm, 136);
    DefineItemBuildWeapon(gItemBuildSilenced, WEAPON_SILENCED, gCalibre9mm, 136);
    DefineItemBuildWeapon(gItemBuildDesertEagle, WEAPON_DEAGLE, gCalibre357, 48);
    DefineItemBuildWeapon(gItemBuildShotgun, WEAPON_SHOTGUN, gCalibre12g, 48);
    DefineItemBuildWeapon(gItemBuildSawnOff, WEAPON_SAWEDOFF, gCalibre12g, 48);
    DefineItemBuildWeapon(gItemBuildCombatShotgun, WEAPON_SHOTGSPA, gCalibre12g, 48);
    DefineItemBuildWeapon(gItemBuildMicroSMG, WEAPON_UZI, gCalibre9mm, 150);
    DefineItemBuildWeapon(gItemBuildMP5, WEAPON_MP5, gCalibre9mm, 150);
    DefineItemBuildWeapon(gItemBuildAK47, WEAPON_AK47, gCalibre762, 150);
    DefineItemBuildWeapon(gItemBuildM4, WEAPON_M4, gCalibre556, 150);
    DefineItemBuildWeapon(gItemBuildTec9, WEAPON_TEC9, gCalibre9mm, 150);
    DefineItemBuildWeapon(gItemBuildRifle, WEAPON_RIFLE, gCalibre762, 50);
    DefineItemBuildWeapon(gItemBuildSniper, WEAPON_SNIPER, gCalibre762, 50);
    DefineItemBuildWeapon(gItemBuildRocket, WEAPON_ROCKETLAUNCHER, gCalibreRocket, 10);
    DefineItemBuildWeapon(gItemBuildHeatSeeker, WEAPON_HEATSEEKER, gCalibreRocket, 10);
    DefineItemBuildWeapon(gItemBuildFlamethrower, WEAPON_FLAMETHROWER, gCalibreGas, 3000);
    DefineItemBuildWeapon(gItemBuildMinigun, WEAPON_MINIGUN, gCalibre762, 500);
    DefineItemBuildWeapon(gItemBuildSprayCan, WEAPON_SPRAYCAN, gCalibrePaint, 500, 6, true);
    DefineItemBuildWeapon(gItemBuildExtinguisher, WEAPON_FIREEXTINGUISHER, gCalibreFoam, 3000, 1, true);
    DefineItemBuildWeapon(gItemBuildCamera, WEAPON_CAMERA);
    DefineItemBuildWeapon(gItemBuildNightVision, WEAPON_NIGHT_VISION_GOGGLES);
    DefineItemBuildWeapon(gItemBuildThermal, WEAPON_THERMAL_GOGGLES);
    DefineItemBuildWeapon(gItemBuildParachute, WEAPON_PARACHUTE);

    SetItemBuildRarity(gItemBuildMinigun, RARITY_LEGENDARY);
    SetItemBuildRarity(gItemBuildHeatSeeker, RARITY_LEGENDARY);
    SetItemBuildRarity(gItemBuildRocket, RARITY_EPIC);

    SetItemBuildPreviewSettings(gItemBuildKnuckles, 35.0, 35.0, 0.0, 275.0, 0.0, 1.0, 0.0, 8.0);
    SetItemBuildPreviewSettings(gItemBuildGolfClub, 70.0, 70.0, 0.0, 136.0, 0.0, 3.0, 8.5, -9.5);
    SetItemBuildPreviewSettings(gItemBuildNightstick, 35.0, 35.0, 0.0, 315.0, 180.0, 1.5, -4.0, 3.0);
    SetItemBuildPreviewSettings(gItemBuildKnife, 35.0, 35.0, 0.0, 315.0, 180.0, 1.25, -5.0, 3.0);
    SetItemBuildPreviewSettings(gItemBuildBat, 35.0, 35.0, 0.0, 315.0, 180.0, 2.0, -6.0, 6.0);
    SetItemBuildPreviewSettings(gItemBuildShovel, 35.0, 35.0, 0.0, 315.0, 280.0, 2.0, -3.0, 7.0);
    SetItemBuildPreviewSettings(gItemBuildPoolCue, 35.0, 35.0, 0.0, 315.0, 180.0, 3.0, -7.0, 8.0);
    SetItemBuildPreviewSettings(gItemBuildKatana, 35.0, 35.0, 0.0, 315.0, 180.0, 2.0, -6.0, 8.0);
    SetItemBuildPreviewSettings(gItemBuildChainsaw, 45.0, 45.0, 0.0, 15.0, 180.0, 2.60, -10.0, 6.0);
    SetItemBuildPreviewSettings(gItemBuildDildo, 35.0, 35.0, 0.0, 300.0, 180.0, 1.0, -7.0, 7.0);
    SetItemBuildPreviewSettings(gItemBuildDildoWhite, 35.0, 35.0, 0.0, 315.0, 180.0, 1.0, -4.0, 0.0);
    SetItemBuildPreviewSettings(gItemBuildVibrator, 35.0, 35.0, 0.0, -45.0, 180.0, 1.0, -7.0, 5.0);
    SetItemBuildPreviewSettings(gItemBuildVibratorSilver, 35.0, 35.0, 0.0, 315.0, 180.0, 1.0, -4.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildFlowers, 35.0, 35.0, 0.0, 315.0, 180.0, 1.75, -6.0, 6.0);
    SetItemBuildPreviewSettings(gItemBuildCane, 35.0, 35.0, 0.0, 315.0, 180.0, 1.75, -7.0, 6.0);
    SetItemBuildPreviewSettings(gItemBuildGrenade, 46.0, 45.0, 0.0, 0.0, 90.0, 1.0, -2.5, 3.0);
    SetItemBuildPreviewSettings(gItemBuildTearGas, 45.0, 45.0, 0.0, 0.0, 180.0, 1.0, -3.0, 4.0);
    SetItemBuildPreviewSettings(gItemBuildMolotov, 35.0, 35.0, 0.0, 0.0, 180.0, 1.0, -3.0, 1.0);
    SetItemBuildPreviewSettings(gItemBuildPistol, 35.0, 35.0, 0.0, 36.0, 180.0, 1.0, -2.0, 6.0);
    SetItemBuildPreviewSettings(gItemBuildSilenced, 35.0, 35.0, 0.0, 36.0, 180.0, 1.0, -4.0, 6.0);
    SetItemBuildPreviewSettings(gItemBuildDesertEagle, 35.0, 35.0, 0.0, 45.0, 180.0, 1.0, -4.0, 7.0);
    SetItemBuildPreviewSettings(gItemBuildShotgun, 35.0, 35.0, 0.0, 35.0, 180.0, 2.25, -4.0, 5.0);
    SetItemBuildPreviewSettings(gItemBuildSawnOff, 35.0, 35.0, 0.0, 35.0, 180.0, 1.5, -6.0, 8.0);
    SetItemBuildPreviewSettings(gItemBuildCombatShotgun, 35.0, 35.0, 0.0, 35.0, 180.0, 2.0, -6.0, 8.0);
    SetItemBuildPreviewSettings(gItemBuildMicroSMG, 35.0, 35.0, 0.0, -41.0, 0.0, 1.5, 3.0, 1.0);
    SetItemBuildPreviewSettings(gItemBuildMP5, 35.0, 35.0, 0.0, 37.0, 180.0, 1.75, -2.0, 6.0);
    SetItemBuildPreviewSettings(gItemBuildAK47, 35.0, 35.0, 0.0, 40.0, 180.0, 2.2, -3.0, 5.0);
    SetItemBuildPreviewSettings(gItemBuildM4, 35.0, 35.0, 0.0, 40.0, 180.0, 2.2, -2.0, 5.0);
    SetItemBuildPreviewSettings(gItemBuildTec9, 35.0, 35.0, 0.0, 45.0, 180.0, 1.5, -3.0, 5.0);
    SetItemBuildPreviewSettings(gItemBuildRifle, 35.0, 35.0, 0.0, 32.0, 180.0, 2.2, -3.0, 5.0);
    SetItemBuildPreviewSettings(gItemBuildSniper, 35.0, 35.0, 0.0, 35.0, 180.0, 2.2, -2.0, 4.0);
    SetItemBuildPreviewSettings(gItemBuildRocket, 35.0, 35.0, 0.0, 45.0, 180.0, 2.5, 3.0, 1.0);
    SetItemBuildPreviewSettings(gItemBuildHeatSeeker, 35.0, 35.0, 0.0, 45.0, 180.0, 2.5, 3.0, 1.0);
    SetItemBuildPreviewSettings(gItemBuildFlamethrower, 45.0, 45.0, 0.0, 17.0, 180.0, 3.5, -9.0, 6.0);
    SetItemBuildPreviewSettings(gItemBuildMinigun, 48.0, 48.0, 0.0, 17.0, 180.0, 3.5, -11.5, 6.5);
    SetItemBuildPreviewSettings(gItemBuildSprayCan, 42.0, 42.0, 0.0, 5.0, 180.0, 1.0, -3.5, -1.5);
    SetItemBuildPreviewSettings(gItemBuildExtinguisher, 35.0, 35.0, 0.0, 0.0, 0.0, 2.0, 9.0, -3.0);
    SetItemBuildPreviewSettings(gItemBuildCamera, 46.0, 50.0, -10.0, 5.0, 113.0, 1.5, -7.5, 4.5);
    SetItemBuildPreviewSettings(gItemBuildNightVision, 44.0, 44.0, 0.0, 90.0, -10.0, 1.0, 0.5, -3.5);
    SetItemBuildPreviewSettings(gItemBuildThermal, 44.0, 44.0, 0.0, 90.0, -10.0, 1.0, 0.5, -3.5);
    SetItemBuildPreviewSettings(gItemBuildParachute, 42.0, 42.0, 0.0, 0.0, 180.0, 2.5, 0.5, 3.5);

    SetItemBuildPreviewSettings(gItemBuildAmmo9mm, 34.0, 35.0, 0.0, 0.0, 90.0, 1.00, 0.5, 1.0);
    SetItemBuildPreviewSettings(gItemBuildAmmo357, 35.0, 35.0, 90.0, 45.0, 0.0, 1.25, -1.0, 0.0);
    SetItemBuildPreviewSettings(gItemBuildAmmo12g, 35.0, 35.0, -13.0, 0.0, 45.0, 1.25, 1.0, 1.0);
    SetItemBuildPreviewSettings(gItemBuildAmmo556, 35.0, 35.0, -15.0, 0.0, 45.0, 1.25, 1.0, 1.0);
    SetItemBuildPreviewSettings(gItemBuildAmmo762, 36.0, 35.0, -15.0, 0.0, 45.0, 1.25, -0.5, 1.0);
    SetItemBuildPreviewSettings(gItemBuildAmmoRocket, 35.0, 35.0, -15.0, 0.0, 45.0, 1.25, 0.0, 1.0);

    return 0;
}
