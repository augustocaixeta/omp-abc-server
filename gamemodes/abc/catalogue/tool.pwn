#if defined _CATALOGUE_TOOL
    #endinput
#endif
#define _CATALOGUE_TOOL

#include <pp-hooks>

/**
 * # Things used on something else
 *
 * Not worn, not eaten, and not a weapon id -- which is what makes them a file
 * rather than an addition to one. A crowbar is swung and a spanner is turned,
 * and neither is something the game has a slot for, so each is a model with a
 * name until whatever it is for is written.
 *
 * # What they do not do yet
 *
 * Nothing, the same as catalogue/gear. They are carried, dropped, given and
 * looted, and a repair system, a lock system or a restraint system would each
 * reach for one of these and decide then what using it means.
 *
 * The handcuffs are the odd one in that list: what they are used on is a
 * person, which is a different kind of answer from a car or a door, and it
 * wants a consent rule before it wants any code.
 */

new
    ItemBuild:gItemBuildWrench      = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildScrewdriver = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildCrowbar     = INVALID_ITEM_BUILD_ID,
    ItemBuild:gItemBuildHandcuffs   = INVALID_ITEM_BUILD_ID
;

hook OnGameModeInit() {
    gItemBuildWrench      = BuildItem("Wrench", 18633, 2, 0.0, 0.0, 0.0, -0.030, 0.076999, 0.035999, -0.004000, 92.500198, -89.999816, -1.200001);
    gItemBuildScrewdriver = BuildItem("Screwdriver", 18644, 1, 90.0, 0.0, 0.0, -0.025, 0.093000, 0.033999, -0.005999, 178.800292, 5.200007, 0.100000);
    gItemBuildCrowbar     = BuildItem("Crowbar", 18634, 4, 0.0, 90.0, 0.0, -0.030, 0.081999, 0.031999, -0.062999, 95.100791, -88.499671, 6.199991);
    gItemBuildHandcuffs   = BuildItem("Handcuffs", 11749, 1, 90.0);

    SetKeyItemBuild("wrench", gItemBuildWrench);
    SetKeyItemBuild("screwdriver", gItemBuildScrewdriver);
    SetKeyItemBuild("crowbar", gItemBuildCrowbar);
    SetKeyItemBuild("handcuffs", gItemBuildHandcuffs);

    new const
        toolAttributes = ITEM_ATTRIBUTE_HOLDABLE | ITEM_ATTRIBUTE_GIVEABLE | ITEM_ATTRIBUTE_DROPPABLE | ITEM_ATTRIBUTE_LOOTABLE
    ;

    SetItemBuildAttributes(gItemBuildWrench, toolAttributes);
    SetItemBuildAttributes(gItemBuildScrewdriver, toolAttributes);
    SetItemBuildAttributes(gItemBuildCrowbar, toolAttributes);
    SetItemBuildAttributes(gItemBuildHandcuffs, toolAttributes);

    SetItemBuildPreviewSettings(gItemBuildWrench, 35.0, 35.0, 90.0, 90.0, 0.0, 1.30, -2.0, 9.0);
    SetItemBuildPreviewSettings(gItemBuildScrewdriver, 66.0, 64.0, 180.0, 0.0, 0.0, 2.0, 0.5, 18.5);
    SetItemBuildPreviewSettings(gItemBuildCrowbar, 35.0, 35.0, 180.0, 315.0, 90.0, 1.0, -3.0, 10.0);
    SetItemBuildPreviewSettings(gItemBuildHandcuffs, 35.0, 35.0, 270.0, 180.0, 215.0, 1.25, 0.0, 2.0);

    return 0;
}
