#if defined _CORE_ITEM_LIQUID
    #endinput
#endif
#define _CORE_ITEM_LIQUID

#include <pp-hooks>

/**
 * # What can be in a bottle
 *
 * A list of liquids, and nothing about who is holding one. Water, cola, petrol
 * and bleach are the same shape of thing here -- a name and what a mouthful of
 * it is worth -- and it is core/item-build/liquid-container that decides which
 * of them a particular can has in it.
 *
 * Kept apart because the two change for different reasons. A new drink is a
 * line in the catalogue; a new kind of bottle is a build. Neither is a reason
 * to touch the other.
 *
 * Nutrition can be negative. Bleach is a drink in exactly the way petrol is a
 * drink, and the number is what says so rather than a flag saying "bad".
 */

#define INVALID_LIQUID_ID (Liquid:-1)

#if !defined MAX_LIQUIDS
    #define MAX_LIQUIDS (Liquid:32)
#endif

#if !defined MAX_LIQUID_NAME
    #define MAX_LIQUID_NAME (24)
#endif

static enum _:E_LIQUID_DATA {
    E_LIQUID_NAME[MAX_LIQUID_NAME],
    Float:E_LIQUID_NUTRITION
};

static
    gLiquidData[MAX_LIQUIDS][E_LIQUID_DATA],
    Liquid:gLiquidCount
;

/**
 * # Functions
 */

/**
 * @brief      Register a liquid.
 *
 * The order these are called in is the `Liquid:`, and an item that has been
 * filled remembers the number rather than the name -- so nothing above an entry
 * may be removed without turning every bottle below it into something else. Add
 * at the end.
 *
 * @param      name       What it is called, shown on the bottle holding it.
 * @param      nutrition  What a mouthful is worth. Negative for what harms.
 *
 * @date       19:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - the `Liquid:` it was given
 *             - `INVALID_LIQUID_ID` when `MAX_LIQUIDS` already exist
 */
forward Liquid:DefineLiquid(const name[], Float:nutrition = 0.0);

/**
 * @brief      Whether a liquid was ever registered under this number.
 *
 * @param      liquid  Liquid to check.
 *
 * @date       19:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is one of the registered liquids
 *             - `false` it is below zero or past the last one registered
 */
forward bool:IsValidLiquid(Liquid:liquid);

/**
 * @brief      How many liquids are registered.
 *
 * @date       19:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     the count, which is also one past the highest valid liquid
 */
forward Liquid:GetLiquidCount();

/**
 * @brief      What a liquid is called.
 *
 * @param      liquid  Liquid to read.
 * @param      output  Buffer for the name.
 * @param      size    Size of that buffer.
 *
 * @date       19:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the name was written
 *             - `false` `liquid` is not registered, and output is emptied
 */
forward bool:GetLiquidName(Liquid:liquid, output[], size = sizeof (output));

/**
 * @brief      What one mouthful of a liquid is worth.
 *
 * @param      liquid  Liquid to read.
 *
 * @date       19:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - the nutrition, which may be negative
 *             - `0.0` when `liquid` is not registered
 */
forward Float:GetLiquidNutrition(Liquid:liquid);

/**
 * # External
 */

stock Liquid:DefineLiquid(const name[], Float:nutrition = 0.0) {
    if (gLiquidCount >= MAX_LIQUIDS) {
        return INVALID_LIQUID_ID;
    }

    new const
        Liquid:liquid = gLiquidCount++
    ;

    strcopy(gLiquidData[liquid][E_LIQUID_NAME], name);
    gLiquidData[liquid][E_LIQUID_NUTRITION] = nutrition;

    return liquid;
}

stock bool:IsValidLiquid(Liquid:liquid) {
    return (0 <= _:liquid < _:gLiquidCount);
}

stock Liquid:GetLiquidCount() {
    return gLiquidCount;
}

stock bool:GetLiquidName(Liquid:liquid, output[], size = sizeof (output)) {
    output[0] = EOS;

    if (!IsValidLiquid(liquid)) {
        return false;
    }

    strcopy(output, gLiquidData[liquid][E_LIQUID_NAME], size);

    return true;
}

stock Float:GetLiquidNutrition(Liquid:liquid) {
    if (!IsValidLiquid(liquid)) {
        return 0.0;
    }

    return gLiquidData[liquid][E_LIQUID_NUTRITION];
}
