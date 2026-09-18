#if defined _CATALOGUE_LIQUID
    #endinput
#endif
#define _CATALOGUE_LIQUID

#include <pp-hooks>

/**
 * # What can be in a vessel
 *
 * The list of liquids, and only that. Which cans hold which of them is
 * catalogue/consumable's; which weapons run on one is catalogue/weapon's.
 *
 * It comes first among the catalogues because both of those ask it questions. A
 * jerrycan cannot be told it turns up full of petrol before petrol exists, and
 * a chainsaw cannot be told petrol is its calibre either.
 *
 * The order is the `Liquid:` an item remembers, so nothing above an entry may
 * be removed without turning every bottle below it into something else. Add at
 * the end.
 *
 * Nutrition can be negative. Bleach is a drink in exactly the way petrol is a
 * drink, and the number is what says so rather than a flag saying "bad".
 *
 * Not everything here is drunk or poured. Paint and foam are liquids because a
 * spray can and an extinguisher run out the way a chainsaw runs out, and a
 * calibre that is a liquid is how a thing that runs out is already written.
 * Nobody drinks either, which is what the nutrition says.
 */

new
    Liquid:gLiquidWater    = INVALID_LIQUID_ID,
    Liquid:gLiquidCola     = INVALID_LIQUID_ID,
    Liquid:gLiquidBeer     = INVALID_LIQUID_ID,
    Liquid:gLiquidGasoline = INVALID_LIQUID_ID,
    Liquid:gLiquidPaint    = INVALID_LIQUID_ID,
    Liquid:gLiquidFoam     = INVALID_LIQUID_ID,
    Liquid:gLiquidJuice    = INVALID_LIQUID_ID,
    Liquid:gLiquidWhiskey  = INVALID_LIQUID_ID,
    Liquid:gLiquidVodka    = INVALID_LIQUID_ID,
    Liquid:gLiquidRum      = INVALID_LIQUID_ID,
    Liquid:gLiquidGin      = INVALID_LIQUID_ID,
    Liquid:gLiquidTequila  = INVALID_LIQUID_ID
;

hook OnGameModeInit() {
    gLiquidWater    = DefineLiquid("Water", 8.0);
    gLiquidCola     = DefineLiquid("Cola", 12.0);
    gLiquidBeer     = DefineLiquid("Beer", 4.0);
    gLiquidGasoline = DefineLiquid("Gasoline", -40.0);
    gLiquidPaint    = DefineLiquid("Paint", -30.0);
    gLiquidFoam     = DefineLiquid("Foam", -10.0);
    gLiquidJuice    = DefineLiquid("Juice", 14.0);
    gLiquidWhiskey  = DefineLiquid("Whiskey", 2.0);
    gLiquidVodka    = DefineLiquid("Vodka", 2.0);
    gLiquidRum      = DefineLiquid("Rum", 2.0);
    gLiquidGin      = DefineLiquid("Gin", 2.0);
    gLiquidTequila  = DefineLiquid("Tequila", 2.0);

    return 0;
}
