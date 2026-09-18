#if defined _CORE_ITEM_CALIBRE
    #endinput
#endif
#define _CORE_ITEM_CALIBRE

#include <pp-hooks>

/**
 * # What a weapon eats
 *
 * A list of calibres, and nothing about who is carrying one. 9mm, 7.62mm, 12
 * gauge -- a name, and the answer to "will this box go in that rifle?".
 *
 * It is the same shape as core/item/liquid and kept apart for the same reason:
 * a new calibre is a line in a catalogue, a new kind of magazine is a build,
 * and neither is a reason to touch the other. What matches what is settled
 * here; core/item-build/ammunition says which items are boxes of it, and
 * core/item-build/weapon says which items eat it.
 *
 * Nothing about ballistics. A calibre here does not carry muzzle velocity,
 * penetration or a bleed rate, because nothing in this gamemode reads them yet
 * -- a weapon is armed and the game decides what it does. When a damage layer
 * arrives it adds its own numbers beside these rather than inside them, the
 * same way ammunition and weapon already add theirs.
 *
 * # Calibres that are poured
 *
 * A chainsaw takes petrol the way a rifle takes 7.62mm: it is the thing that
 * has to be right before the trigger does anything. So a calibre may name a
 * `Liquid:` instead of being carried in boxes, and every weapon then asks the
 * one question -- what calibre? -- whichever kind of answer it gets.
 *
 * ScavengeSurvive gets there by passing a liquid id into the calibre argument
 * and setting a flag, which works because both are numbers. That is not done
 * here: a `Liquid:` and a `Calibre:` are different tags and neither converts
 * into the other, so the liquid is named as a liquid and kept as one.
 *
 * A poured calibre is measured in millilitres rather than rounds, so that
 * everything downstream counts in whole numbers. A two litre tank is 2000, and
 * core/item-build/weapon is what turns that back into `2L` on the screen.
 */

#define INVALID_CALIBRE_ID (Calibre:-1)

#if !defined MAX_CALIBRES
    #define MAX_CALIBRES (Calibre:32)
#endif

#if !defined MAX_CALIBRE_NAME
    #define MAX_CALIBRE_NAME (16)
#endif

static enum _:E_CALIBRE_DATA {
    E_CALIBRE_NAME[MAX_CALIBRE_NAME],

    // INVALID_LIQUID_ID for everything carried in boxes. Written by every
    // registration, because a zeroed cell would read as Liquid:0 -- which is a
    // real liquid, and would make the first drink ever declared the fuel for
    // every calibre nobody said anything about.
    Liquid:E_CALIBRE_LIQUID
};

static
    gCalibreData[MAX_CALIBRES][E_CALIBRE_DATA],
    Calibre:gCalibreCount
;

/**
 * # Functions
 */

/**
 * @brief      Register a calibre.
 *
 * The order these are called in is the `Calibre:`, and a weapon that has been
 * defined remembers the number rather than the name -- so nothing above an
 * entry may be removed without turning every rifle below it into something
 * that eats the wrong thing. Add at the end.
 *
 * Naming a liquid makes it a calibre that is poured rather than loaded: the
 * weapon is filled from a vessel holding that liquid, and what it holds is
 * counted in millilitres.
 *
 * @param      name    What it is called, shown on a box and on the weapon.
 * @param      liquid  The liquid it is poured from, or nothing for a calibre
 *                     carried in boxes.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - the `Calibre:` it was given
 *             - `INVALID_CALIBRE_ID` when:
 *                 + MAX_CALIBRES are already registered
 *                 + `name` is empty
 *                 + `liquid` was named but is not a registered liquid
 */
forward Calibre:DefineCalibre(const name[], Liquid:liquid = INVALID_LIQUID_ID);

/**
 * @brief      Whether a calibre was ever registered under this number.
 *
 * @param      calibreid  Calibre to check.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is one of the registered calibres
 *             - `false` it is below zero or past the last one registered
 */
forward bool:IsValidCalibre(Calibre:calibreid);

/**
 * @brief      How many calibres are registered.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     the count, which is also one past the highest valid calibre
 */
forward Calibre:GetCalibreCount();

/**
 * @brief      What a calibre is called.
 *
 * @param      calibreid  Calibre to read.
 * @param      output     Reads back the name.
 * @param      size       How much the output holds.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the name was written
 *             - `false` `calibreid` is not registered, and the output is
 *               emptied
 */
forward bool:GetCalibreName(Calibre:calibreid, output[], size = sizeof (output));

/**
 * @brief      Which liquid a calibre is poured from.
 *
 * @param      calibreid  Calibre to read.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - the liquid it was registered with
 *             - `INVALID_LIQUID_ID` when:
 *                 + `calibreid` is not registered
 *                 + it is carried in boxes rather than poured
 */
forward Liquid:GetCalibreLiquid(Calibre:calibreid);

/**
 * @brief      Whether a calibre is poured rather than loaded.
 *
 * What anything filling a weapon asks first, because the two come from
 * different places: a vessel for one, a box for the other.
 *
 * @param      calibreid  Calibre to ask.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it comes out of a vessel, measured in millilitres
 *             - `false` when:
 *                 + `calibreid` is not registered
 *                 + it is carried in boxes and counted in rounds
 */
forward bool:IsCalibreLiquid(Calibre:calibreid);

/**
 * @brief      Which calibre is poured from a liquid, if any is.
 *
 * The reverse lookup, for a vessel that has been tipped into something and
 * needs to know what it just filled.
 *
 * @param      liquid  Liquid to look up.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - the calibre poured from it
 *             - `INVALID_CALIBRE_ID` when:
 *                 + `liquid` is not registered
 *                 + no calibre is poured from it
 */
forward Calibre:GetLiquidCalibre(Liquid:liquid);

/**
 * # External
 */

stock Calibre:DefineCalibre(const name[], Liquid:liquid = INVALID_LIQUID_ID) {
    if (gCalibreCount == MAX_CALIBRES) {
        return INVALID_CALIBRE_ID;
    }

    if (isnull(name)) {
        return INVALID_CALIBRE_ID;
    }

    if (liquid != INVALID_LIQUID_ID && !IsValidLiquid(liquid)) {
        return INVALID_CALIBRE_ID;
    }

    new const
        Calibre:calibreid = gCalibreCount++
    ;

    strcopy(gCalibreData[calibreid][E_CALIBRE_NAME], name);
    gCalibreData[calibreid][E_CALIBRE_LIQUID] = liquid;

    return calibreid;
}

stock bool:IsValidCalibre(Calibre:calibreid) {
    return (0 <= _:calibreid < _:gCalibreCount);
}

stock Calibre:GetCalibreCount() {
    return gCalibreCount;
}

stock bool:GetCalibreName(Calibre:calibreid, output[], size = sizeof (output)) {
    output[0] = EOS;

    if (!IsValidCalibre(calibreid)) {
        return false;
    }

    strcopy(output, gCalibreData[calibreid][E_CALIBRE_NAME], size);

    return true;
}

stock Liquid:GetCalibreLiquid(Calibre:calibreid) {
    if (!IsValidCalibre(calibreid)) {
        return INVALID_LIQUID_ID;
    }

    return gCalibreData[calibreid][E_CALIBRE_LIQUID];
}

stock bool:IsCalibreLiquid(Calibre:calibreid) {
    return GetCalibreLiquid(calibreid) != INVALID_LIQUID_ID;
}

stock Calibre:GetLiquidCalibre(Liquid:liquid) {
    if (!IsValidLiquid(liquid)) {
        return INVALID_CALIBRE_ID;
    }

    for (new Calibre:calibreid; calibreid != gCalibreCount; ++calibreid) {
        if (gCalibreData[calibreid][E_CALIBRE_LIQUID] == liquid) {
            return calibreid;
        }
    }

    return INVALID_CALIBRE_ID;
}
