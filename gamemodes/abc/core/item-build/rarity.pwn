#if defined _CORE_ITEM_BUILD_RARITY
    #endinput
#endif
#define _CORE_ITEM_BUILD_RARITY

#include <pp-hooks>

/**
 * # How special a thing is
 *
 * A tint on the square an item stands in, and nothing else yet. A minigun is
 * gold, a rocket launcher is purple, and everything a character actually
 * carries around is the colour the grid already was.
 *
 * # Most things have no rarity at all
 *
 * Which is not the same as being common, and the difference is the whole point.
 * Common is a rarity -- the grey present is one -- and a burger is not a common
 * burger, it is a burger. If every square were tinted the grid would be a
 * carnival and the gold one would stop meaning anything, so the default is
 * RARITY_NONE and a build says otherwise on purpose.
 *
 * That makes the catalogue the list of what is worth noticing: read down it and
 * the handful of SetItemBuildRarity calls are the answer.
 *
 * # What it is for
 *
 * Being seen, today. It is also the obvious handle for anything that has to ask
 * "how good is this" later -- what a present turns into, what a shop will pay,
 * what turns up in a crate -- so it is one number on the build rather than a
 * flag in each of those.
 */

enum Rarity {
    RARITY_NONE,
    RARITY_COMMON,
    RARITY_UNCOMMON,
    RARITY_RARE,
    RARITY_EPIC,
    RARITY_LEGENDARY
};

static
    Rarity:gItemBuildRarity[MAX_ITEM_BUILDS],
    gRarityColour[Rarity] = {
        0,
        RARITY_COMMON_COLOUR,
        RARITY_UNCOMMON_COLOUR,
        RARITY_RARE_COLOUR,
        RARITY_EPIC_COLOUR,
        RARITY_LEGENDARY_COLOUR
    }
;

/**
 * # Functions
 */

/**
 * @brief      Say how special a kind of item is.
 *
 * A build told nothing has no rarity, which is what almost everything
 * should be: the tint is worth having because it is rare on the screen as
 * well as in the world.
 *
 * @param      buildid  Build to mark.
 * @param      rarity   How special it is.
 *
 * @date       16:40 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the build is marked
 *             - `false` `buildid` is not a registered build
 */
forward bool:SetItemBuildRarity(ItemBuild:buildid, Rarity:rarity);

/**
 * @brief      How special a kind of item is.
 *
 * @param      buildid  Build to read.
 *
 * @date       16:40 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - its `Rarity:`
 *             - `RARITY_NONE` it was never told, or is not a build
 */
forward Rarity:GetItemBuildRarity(ItemBuild:buildid);

/**
 * @brief      The colour a rarity tints a square.
 *
 * @param      rarity  Rarity to read.
 *
 * @date       16:40 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - the colour
 *             - `0` for `RARITY_NONE`, which is "leave it alone"
 */
forward GetRarityColour(Rarity:rarity);

/**
 * # Implementation
 */

stock bool:SetItemBuildRarity(ItemBuild:buildid, Rarity:rarity) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    if (!(RARITY_NONE <= rarity < Rarity)) {
        return false;
    }

    gItemBuildRarity[buildid] = rarity;
    
    SetItemBuildSlotColour(buildid, gRarityColour[rarity]);

    return true;
}

stock Rarity:GetItemBuildRarity(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return RARITY_NONE;
    }

    return gItemBuildRarity[buildid];
}

stock GetRarityColour(Rarity:rarity) {
    if (!(RARITY_NONE <= rarity < Rarity)) {
        return 0;
    }

    return gRarityColour[rarity];
}

