#if defined _CATALOGUE_CONSUMABLE
    #endinput
#endif
#define _CATALOGUE_CONSUMABLE

#include <pp-hooks>

/**
 * # Food, drink and medicine
 *
 * Things that stop existing when they are used. Attach numbers from
 * ScavengeSurvive; see catalogue/weapon for how its parameter list maps onto
 * BuildItem's.
 *
 * What using one does to a character -- hunger, thirst, bleeding, health -- is
 * not decided here and not decided yet. item-action/use consumes the item and
 * says so, and this is where a condition subsystem would be told which of these
 * feeds and which of these heals.
 *
 * # The pizza box
 *
 * Size seven, the same as a rucksack, which is what keeps one out of the other:
 * a rucksack swallows six. Two levels is as deep as this catalogue goes -- a
 * crate takes either of them, and neither takes the other. It is narrowed to
 * slices, so nothing else goes in it whatever its size.
 *
 * # Vessels
 *
 * A can is one swallow short of a third of a litre and is thrown away; a
 * jerrycan is twenty litres and is kept, which is what reusable means.
 *
 * What each turns up holding is separate from what it can hold: a can is one of
 * three drinks, and a jerrycan is petrol or nothing -- the nothing being the
 * empty one somebody has to fill. The liquids themselves are catalogue/liquid's,
 * because catalogue/weapon needs them too.
 *
 * # Food
 *
 * Bites, what a bite is worth, how long before it turns, and which of the three
 * FOOD animations it is eaten with. A joint of meat is gnawed off the bone,
 * which is EAT_CHICKEN; a slice is folded and bitten, which is EAT_PIZZA;
 * everything else takes the default.
 *
 * Raw meat goes off in a day and cooked keeps for three. The countdown is the
 * amount, so saying a life is also saying what the corner of the slot shows --
 * which is why mouthfuls are not the amount, and ride in the item's own data
 * instead.
 *
 * Pills are the exception that is not eaten: they stop being a thing that
 * vanishes when used and become a bottle with a number in it.
 *
 * # No USE row on any of it
 *
 * Food and drink are taken into the hands and used there, by holding the key.
 * A USE row is PICK UP with a step after it, which is two ways to do one thing
 * -- and worse than redundant: the row consumes whatever it does not recognise,
 * so USE on a full can destroyed the can without anybody drinking from it, and
 * a bottle of pills went in the bin with eleven left in it.
 *
 * What keeps the row is what does nothing in the hands. A bandage and a medkit
 * have no in-hand behaviour written, so USE is the whole of what they do and
 * consuming them is exactly right.
 *
 * # Two of some things
 *
 * Two pizza boxes, two juice cartons and five bottles of spirits, and in each
 * case the pair or the five are one item with more than one model. A character
 * sees "Pizza Box" whichever they are holding, which is right -- it is a pizza
 * box -- and the catalogue tells them apart by the variable rather than by the
 * name. The same choice as the glasses in catalogue/gear.
 *
 * The bottles differ in what is in them rather than in what they are. Each is
 * a Bottle holding one spirit, so the slot reads "Bottle (Whiskey)" the way a
 * tin reads "Can of Drink (Cola)", and the five models are five labels on a
 * shelf rather than five kinds of thing to carry.
 */

new
    ItemBuild:gItemBuildBurger     = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildMeat       = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildMeatCooked = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildPizza      = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildPizzaSlice = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildCanDrink   = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildBandage    = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildPills      = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildMedkit     = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildJerrycan   = INVALID_ITEM_BUILD_ID,

    ItemBuild:gItemBuildPizzaRed   = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildTacos      = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildJuice      = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildJuiceApple = INVALID_ITEM_BUILD_ID,

    ItemBuild:gItemBuildWhiskey    = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildVodka      = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildRum        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildGin        = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildTequila    = INVALID_ITEM_BUILD_ID
;

hook OnGameModeInit() {
    gItemBuildBurger     = BuildItem("Burger", 2703, 1, -76.0, 257.0, -11.0, 0.0, 0.066739, 0.041782, 0.026828, 3.703052, 3.163064, 6.946474);
    gItemBuildMeat       = BuildItem("Raw Meat", 2804, 2, -76.0, 257.0, -11.0, 0.0, 0.142738, 0.016781, 0.049827, -45.797088, -3.537261, -106.353683, 0.253007, 0.299999, 0.494002);
    gItemBuildMeatCooked = BuildItem("Cooked Meat", 19847, 2, -76.0, 257.0, -11.0, 0.0, 0.066739, 0.041782, 0.026828, 3.703052, 3.163064, 6.946474);
    gItemBuildPizza      = BuildItem("Pizza Box", 1582, 7, 0.0, 0.0, 0.0, 0.0, 0.161344, 0.070041, -0.171704, 70.941932, 6.492503, 10.215372, .useCarryAnim = true);
    gItemBuildPizzaSlice = BuildItem("Pizza Slice", 2702, 2, 0.0, 0.0, 0.0, -0.05, 0.064343, 0.117041, 0.012295, 5.641870, -7.607425, 9.715314);
    gItemBuildCanDrink   = BuildItem("Can of Drink", 2601, 1, 0.0, 0.0, 0.0, 0.054, 0.109847, 0.074402, -0.029420, -178.199447, -13.663820, 80.178733, .bone = 5);
    gItemBuildBandage    = BuildItem("Bandage", 11747, 1, 90.0, 0.0, 0.0, 0.0, 0.076999, 0.059000, 0.000000, 2.799999, -5.600000, 0.000000);
    gItemBuildPills      = BuildItem("Pills", 2709, 1, 0.0, 0.0, 0.0, 0.09, 0.044038, 0.082106, 0.019000, 168.799621, 1.400001, -0.499999, .bone = 5);
    gItemBuildJerrycan   = BuildItem("Jerrycan", 1650, 5, 0.0, 0.0, 0.0, 0.260, 0.164739, 0.018782, -0.060171, 179.602432, -87.136924, 17.746496, .bone = 5);
    gItemBuildMedkit     = BuildItem("Medkit", 11736, 1, 0.0, 0.0, 0.0, 0.004, 0.197999, 0.038000, 0.021000, 79.700012, 0.000000, 90.899978);
    gItemBuildPizzaRed   = BuildItem("Pizza Box", 2814, 7, 0.0, 0.0, 0.0, 0.0, 0.161344, 0.070041, -0.171704, 70.941932, 6.492503, 10.215372, .useCarryAnim = true);
    gItemBuildTacos      = BuildItem("Tacos", 2769, 2, 90.0);
    gItemBuildJuice      = BuildItem("Juice Box", 19563, 1, 90.0, 0.0, 0.0, 0.0, 0.119000, 0.042999, -0.099000, 2.600049, 11.799937, 4.799999, .bone = 5);
    gItemBuildJuiceApple = BuildItem("Juice Box", 19564, 1, 90.0, 0.0, 0.0, 0.0, 0.119000, 0.042999, -0.099000, 2.600049, 11.799937, 4.799999, .bone = 5);
    gItemBuildWhiskey    = BuildItem("Bottle", 19820, 2, 90.0, 0.0, 0.0, 0.0, 0.137000, 0.023999, 0.502999, -0.800001, -174.499542, -30.900079, .bone = 5);
    gItemBuildVodka      = BuildItem("Bottle", 19821, 2, 90.0, 0.0, 0.0, 0.0, 0.187000, 0.063999, 0.495999, -4.400001, -170.696350, -30.900079, .bone = 5);
    gItemBuildRum        = BuildItem("Bottle", 19822, 2, 90.0, 0.0, 0.0, 0.0, 0.144000, 0.041999, 0.403998, -1.600001, -173.598159, -30.900079, .bone = 5);
    gItemBuildGin        = BuildItem("Bottle", 19823, 2, 90.0, 0.0, 0.0, 0.0, 0.131000, 0.031000, 0.267000, -0.800001, -174.499542, -30.900079, .bone = 5);
    gItemBuildTequila    = BuildItem("Bottle", 19824, 2, 90.0, 0.0, 0.0, 0.0, 0.130999, 0.030999, 0.434998, -0.800001, -174.499542, -30.900079, .bone = 5);

    SetKeyItemBuild("burger", gItemBuildBurger);
    SetKeyItemBuild("meat", gItemBuildMeat);
    SetKeyItemBuild("meat-cooked", gItemBuildMeatCooked);
    SetKeyItemBuild("pizza", gItemBuildPizza);
    SetKeyItemBuild("pizza-slice", gItemBuildPizzaSlice);
    SetKeyItemBuild("can-drink", gItemBuildCanDrink);
    SetKeyItemBuild("bandage", gItemBuildBandage);
    SetKeyItemBuild("pills", gItemBuildPills);
    SetKeyItemBuild("medkit", gItemBuildMedkit);
    SetKeyItemBuild("jerrycan", gItemBuildJerrycan);
    SetKeyItemBuild("pizza-red", gItemBuildPizzaRed);
    SetKeyItemBuild("tacos", gItemBuildTacos);
    SetKeyItemBuild("juice", gItemBuildJuice);
    SetKeyItemBuild("juice-apple", gItemBuildJuiceApple);
    SetKeyItemBuild("whiskey", gItemBuildWhiskey);
    SetKeyItemBuild("vodka", gItemBuildVodka);
    SetKeyItemBuild("rum", gItemBuildRum);
    SetKeyItemBuild("gin", gItemBuildGin);
    SetKeyItemBuild("tequila", gItemBuildTequila);

    new const
        heldAttributes = ITEM_ATTRIBUTE_HOLDABLE | ITEM_ATTRIBUTE_GIVEABLE | ITEM_ATTRIBUTE_DROPPABLE | ITEM_ATTRIBUTE_LOOTABLE,
        consumableAttributes = ITEM_ATTRIBUTE_USABLE | heldAttributes
    ;

    SetItemBuildAttributes(gItemBuildBurger, heldAttributes);
    SetItemBuildAttributes(gItemBuildMeat, heldAttributes);
    SetItemBuildAttributes(gItemBuildMeatCooked, heldAttributes);
    SetItemBuildAttributes(gItemBuildPizza, ITEM_ATTRIBUTE_OPENABLE | ITEM_ATTRIBUTE_HOLDABLE | ITEM_ATTRIBUTE_GIVEABLE | ITEM_ATTRIBUTE_DROPPABLE);

    DefineItemBuildContainer(gItemBuildPizza, 8, 2);
    DefineItemBuildContainerAccepts(gItemBuildPizza, gItemBuildPizzaSlice);

    SetItemBuildAttributes(gItemBuildPizzaRed, ITEM_ATTRIBUTE_OPENABLE | ITEM_ATTRIBUTE_HOLDABLE | ITEM_ATTRIBUTE_GIVEABLE | ITEM_ATTRIBUTE_DROPPABLE);

    DefineItemBuildContainer(gItemBuildPizzaRed, 8, 2);
    DefineItemBuildContainerAccepts(gItemBuildPizzaRed, gItemBuildPizzaSlice);

    SetItemBuildAttributes(gItemBuildPizzaSlice, heldAttributes);
    SetItemBuildAttributes(gItemBuildCanDrink, heldAttributes);
    SetItemBuildAttributes(gItemBuildPills, heldAttributes);
    SetItemBuildAttributes(gItemBuildTacos, heldAttributes);
    SetItemBuildAttributes(gItemBuildJuice, heldAttributes);
    SetItemBuildAttributes(gItemBuildJuiceApple, heldAttributes);
    SetItemBuildAttributes(gItemBuildWhiskey, heldAttributes);
    SetItemBuildAttributes(gItemBuildVodka, heldAttributes);
    SetItemBuildAttributes(gItemBuildRum, heldAttributes);
    SetItemBuildAttributes(gItemBuildGin, heldAttributes);
    SetItemBuildAttributes(gItemBuildTequila, heldAttributes);
    SetItemBuildAttributes(gItemBuildBandage, consumableAttributes);
    SetItemBuildAttributes(gItemBuildMedkit, consumableAttributes);
    SetItemBuildAttributes(gItemBuildJerrycan, ITEM_ATTRIBUTE_HOLDABLE | ITEM_ATTRIBUTE_GIVEABLE | ITEM_ATTRIBUTE_DROPPABLE | ITEM_ATTRIBUTE_DISCARDABLE | ITEM_ATTRIBUTE_LOOTABLE);

    DefineItemBuildLiquidContainer(gItemBuildCanDrink, 330);
    DefineItemBuildLiquidContainer(gItemBuildJerrycan, 5000, .reusable = true);
    DefineItemBuildLiquidContainer(gItemBuildJuice, 330);
    DefineItemBuildLiquidContainer(gItemBuildJuiceApple, 330);
    DefineItemBuildLiquidContainer(gItemBuildWhiskey, 700);
    DefineItemBuildLiquidContainer(gItemBuildVodka, 700);
    DefineItemBuildLiquidContainer(gItemBuildRum, 700);
    DefineItemBuildLiquidContainer(gItemBuildGin, 700);
    DefineItemBuildLiquidContainer(gItemBuildTequila, 700);

    DefineItemBuildLiquidHolds(gItemBuildCanDrink, gLiquidWater);
    DefineItemBuildLiquidHolds(gItemBuildCanDrink, gLiquidCola);
    DefineItemBuildLiquidHolds(gItemBuildCanDrink, gLiquidBeer);
    DefineItemBuildLiquidHolds(gItemBuildJerrycan, gLiquidGasoline);
    DefineItemBuildLiquidHolds(gItemBuildJuice, gLiquidJuice);
    DefineItemBuildLiquidHolds(gItemBuildJuiceApple, gLiquidJuice);
    DefineItemBuildLiquidHolds(gItemBuildWhiskey, gLiquidWhiskey);
    DefineItemBuildLiquidHolds(gItemBuildVodka, gLiquidVodka);
    DefineItemBuildLiquidHolds(gItemBuildRum, gLiquidRum);
    DefineItemBuildLiquidHolds(gItemBuildGin, gLiquidGin);
    DefineItemBuildLiquidHolds(gItemBuildTequila, gLiquidTequila);

    DefineItemBuildPills(gItemBuildPills, 12, 25.0);

    DefineItemBuildFood(gItemBuildBurger, 3, 20.0, HDUR(12));
    DefineItemBuildFood(gItemBuildMeat, 4, 12.0, DDUR(1), "EAT_CHICKEN");
    DefineItemBuildFood(gItemBuildMeatCooked, 4, 30.0, DDUR(3), "EAT_CHICKEN");
    DefineItemBuildFood(gItemBuildPizzaSlice, 2, 15.0, HDUR(8), "EAT_PIZZA");
    DefineItemBuildFood(gItemBuildTacos, 2, 18.0, HDUR(8), "EAT_PIZZA");

    DefineItemBuildStack(gItemBuildPills, 30);

    SetItemBuildPreviewSettings(gItemBuildBurger, 30.0, 30.0, 260.0, 0.0, -11.0, 1.0, 1.0, -1.5);
    SetItemBuildPreviewSettings(gItemBuildMeat, 30.0, 30.0, 260.0, 0.0, -11.0, 1.0, 1.0, -1.5);
    SetItemBuildPreviewSettings(gItemBuildMeatCooked, 30.0, 30.0, 260.0, 0.0, -11.0, 1.0, 1.0, -1.5);
    SetItemBuildPreviewSettings(gItemBuildPizza, 30.0, 30.0, -15.0, 0.0, 30.0, 1.0, -1.0, -0.5);
    SetItemBuildPreviewSettings(gItemBuildPizzaSlice, 30.0, 30.0, -15.0, 0.0, 30.0, 1.0, -1.0, -0.5);
    SetItemBuildPreviewSettings(gItemBuildCanDrink, 30.0, 30.0, -15.0, 0.0, -60.0, 1.0, 1.0, -0.5);
    SetItemBuildPreviewSettings(gItemBuildBandage, 30.0, 30.0, -30.0, 0.0, 0.0, 1.0, 1.0, -0.5);
    SetItemBuildPreviewSettings(gItemBuildPills, 30.0, 30.0, -10.0, 0.0, 0.0, 1.0, 1.0, -0.5);
    SetItemBuildPreviewSettings(gItemBuildMedkit, 30.0, 30.0, -90.0, 0.0, 0.0, 1.0, 0.0, -0.5);
    SetItemBuildPreviewSettings(gItemBuildJerrycan, 30.0, 30.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    SetItemBuildPreviewSettings(gItemBuildPizzaRed, 35.0, 35.0, -15.0, 0.0, 135.0, 1.25, 0.0, 3.0);
    SetItemBuildPreviewSettings(gItemBuildTacos, 35.0, 35.0, -10.0, 0.0, -82.0, 1.0, -1.0, 1.0);
    SetItemBuildPreviewSettings(gItemBuildJuice, 35.0, 35.0, 0.0, 0.0, 40.0, 1.30, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildJuiceApple, 35.0, 35.0, 0.0, 0.0, 40.0, 1.30, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildWhiskey, 35.0, 35.0, 0.0, 0.0, 0.0, 1.5, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildVodka, 35.0, 35.0, 0.0, 0.0, 0.0, 1.5, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildRum, 35.0, 35.0, 0.0, 0.0, 0.0, 1.5, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildGin, 35.0, 35.0, 0.0, 0.0, 0.0, 1.5, 0.0, 2.0);
    SetItemBuildPreviewSettings(gItemBuildTequila, 35.0, 35.0, 0.0, 0.0, 0.0, 1.5, 0.0, 2.0);

    return 0;
}
