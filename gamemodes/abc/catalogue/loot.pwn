#if defined _CATALOGUE_LOOT
    #endinput
#endif
#define _CATALOGUE_LOOT

#include <pp-hooks>

// Things worth stealing. Not lootable: these are put in the world by whatever
// is being robbed, at the moment it is robbed, and taken back out again when it
// is over -- so they never turn up in a crate and never sit around costing a
// slot for the life of the server.

new
    ItemBuild:gItemBuildGasCanister = INVALID_ITEM_BUILD_ID
;

hook OnGameModeInit() {
    // Carried, never pocketed: the size is most of a character's pockets, so
    // taking one is a decision about what else they are carrying.
    gItemBuildGasCanister = BuildItem("Gas Canister", 918, 20, 0.0, 0.0, 0.0, 0.25, 0.016000, 0.164999, -0.165000, -18.399974, -7.999992, 2.299998, .useCarryAnim = true);

    SetKeyItemBuild("gas-canister", gItemBuildGasCanister);

    SetItemBuildAttributes(gItemBuildGasCanister, ITEM_ATTRIBUTE_HOLDABLE | ITEM_ATTRIBUTE_DROPPABLE);

    SetItemBuildPreviewSettings(gItemBuildGasCanister, 35.0, 35.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);

    return 0;
}
