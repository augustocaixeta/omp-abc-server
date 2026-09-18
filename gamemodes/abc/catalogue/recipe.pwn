#if defined _CATALOGUE_RECIPE
    #endinput
#endif
#define _CATALOGUE_RECIPE

#include <pp-hooks>

/**
 * # What is made out of what
 *
 * The catalogue says what things are; this says which of them come from the
 * others. Every amount is per lot: five metal, two powder and one primer is
 * one pull of the press and ten rounds come off it.
 *
 * Rounds are made on a press and nowhere else, which is a kind of station and
 * not a particular one: a lock-up has a press, and so would anywhere else that
 * wanted to make ammunition.
 *
 * Last of the catalogue, because a recipe names builds and every one of them
 * has to exist before it can be named.
 */

hook OnGameModeInit() {
    new const
        CraftStation:press = DefineCraftStation("Press")
    ;

    SetCraftStation(DefineItemCraft(gItemBuildAmmo762, 10,
        gItemBuildMetal,  5,
        gItemBuildPowder, 2,
        gItemBuildPrimer, 1
    ), press);

    SetCraftStation(DefineItemCraft(gItemBuildAmmo9mm, 15,
        gItemBuildMetal,  4,
        gItemBuildPowder, 2,
        gItemBuildPrimer, 1
    ), press);

    return 0;
}
