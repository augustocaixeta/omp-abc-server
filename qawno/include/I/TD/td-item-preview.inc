#if defined _td_item_preview_included
    #endinput
#endif
#define _td_item_preview_included

#include <open.mp>

#include <I\item>
#include <pp-hooks>

/**
 * # Header
 */

// How a build's model is framed inside a slot, and which model a particular
// item shows. The framing is per build because every copy of a thing is drawn
// the same way; the model is per item because one copy can disagree -- a
// clothes package that shows the skin it holds rather than the package.
//
// The per-item cell is zero for every item that has never disagreed, and zero
// means "ask the build", so the override costs nothing to not use.

enum E_ITEM_BUILD_PREVIEW_DATA {
    Float:E_ITEM_BUILD_PREVIEW_SIZE_X,
    Float:E_ITEM_BUILD_PREVIEW_SIZE_Y,
    Float:E_ITEM_BUILD_PREVIEW_ROTATION_X,
    Float:E_ITEM_BUILD_PREVIEW_ROTATION_Y,
    Float:E_ITEM_BUILD_PREVIEW_ROTATION_Z,
    Float:E_ITEM_BUILD_PREVIEW_ZOOM,
    Float:E_ITEM_BUILD_PREVIEW_OFFSET_X,
    Float:E_ITEM_BUILD_PREVIEW_OFFSET_Y,

    // What the square behind it is painted. Untagged, unlike everything above
    // it, because it is a colour and not a measurement.
    E_ITEM_BUILD_PREVIEW_COLOUR
};

static
    gItemBuildViewData[MAX_ITEM_BUILDS][E_ITEM_BUILD_PREVIEW_DATA],
    gItemPreviewModel[MAX_ITEMS]
;

/**
 * # Functions
 */

/**
 * @brief      An item is being drawn as a different model.
 *
 * So that anything with the item on a screen can draw it again. A slot
 * redraws only what it sees change, and an item whose model was swapped
 * under it is the same item with the same amount -- nothing it compares
 * would differ, and the old model would stay on screen until the square
 * happened to be redrawn for some other reason.
 *
 * Fired once the item already answers the new model.
 *
 * @param      itemid    Item that changed.
 * @param      modelid   What it draws as now.
 * @param      previous  What it drew as before.
 *
 * @date       02:10 12/09/2026
 * @author     augustocaixeta
 */
forward OnItemPreviewModelChange(Item:itemid, modelid, previous);

/**
 * @brief      Say how a build's model is framed in a slot.
 *
 * A model is drawn centred in its slot, in a square the width of the slot.
 * A slot is taller than it is wide so that the amount has a corner of its
 * own to sit in, and a model stretched into that shape leans -- so the
 * square is what everything is framed against, and a build that asks for
 * nothing at all is centred in it at the slot's full width.
 *
 * `offsetX` and `offsetY` move it from there, in pixels, and are the whole
 * of what a build says about where it sits. Positive is right and down.
 * They are a distance and never a place on the screen: a screen position
 * would be a position in one menu, and the same build is drawn in the
 * pockets, in a crate and inside a bag, each somewhere else.
 *
 * Most builds want nothing here. A model that reads as off-centre usually
 * wants a different `zoom` or rotation instead, and an offset is for the
 * ones whose model is not centred on its own origin.
 *
 * @param      buildid    Build to frame.
 * @param      sizeX      Width of the preview box.
 * @param      sizeY      Height of the preview box.
 * @param      rotationX  Pitch of the model inside the box.
 * @param      rotationY  Roll of the model inside the box.
 * @param      rotationZ  Yaw of the model inside the box.
 * @param      zoom       How close the camera sits to it.
 * @param      offsetX    How far right of centred it sits.
 * @param      offsetY    How far below centred it sits.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the framing was written
 *             - `false` `buildid` is not a registered build
 */
forward bool:SetItemBuildPreviewSettings(ItemBuild:buildid, Float:sizeX, Float:sizeY, Float:rotationX = 0.0, Float:rotationY = 0.0, Float:rotationZ = 0.0, Float:zoom = 1.0, Float:offsetX = 0.0, Float:offsetY = 0.0);

/**
 * @brief      How a build's model is framed.
 *
 * Every output has a default, so a caller that only wants the size can ask
 * for the size and ignore the rest.
 *
 * @param      buildid    Build to read.
 * @param      sizeX      Width of the preview box.
 * @param      sizeY      Height of the preview box.
 * @param      rotationX  Pitch of the model inside the box.
 * @param      rotationY  Roll of the model inside the box.
 * @param      rotationZ  Yaw of the model inside the box.
 * @param      zoom       How close the camera sits to it.
 * @param      offsetX    How far right of centred it sits.
 * @param      offsetY    How far below centred it sits.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the outputs were written
 *             - `false` `buildid` is not a registered build, and nothing was
 *               written
 */
forward bool:GetItemBuildPreviewSettings(ItemBuild:buildid, &Float:sizeX = 0.0, &Float:sizeY = 0.0, &Float:rotationX = 0.0, &Float:rotationY = 0.0, &Float:rotationZ = 0.0, &Float:zoom = 0.0, &Float:offsetX = 0.0, &Float:offsetY = 0.0);

/**
 * @brief      Show a different model for one item than its build says.
 *
 * For a copy that is not what its kind looks like: a package that shows the
 * skin inside it, a tin whose label was changed. The framing stays the
 * build's, so the substituted model is drawn at the same size and angle.
 *
 * @param      itemid   Item to override.
 * @param      modelid  Model to show. `0` is the same as clearing it.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the override was written
 *             - `false` `itemid` is not a live item
 */
forward bool:SetItemPreviewModel(Item:itemid, modelid);

/**
 * @brief      Put an item back to showing its build's model.
 *
 * @param      itemid  Item to clear.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the override is gone
 *             - `false` `itemid` is not a live item
 */
forward bool:ResetItemPreviewModel(Item:itemid);

/**
 * @brief      Whether an item shows something other than its build's model.
 *
 * @param      itemid  Item to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it carries an override
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + it shows whatever its build shows
 */
forward bool:HasItemPreviewModel(Item:itemid);

/**
 * @brief      Which model an item actually draws.
 *
 * Its override if it has one, its build's model otherwise, so a caller that
 * only wants to draw the thing never has to ask which of the two it is.
 *
 * @param      itemid  Item to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the model to draw
 *             - `0` when:
 *                 + `itemid` is not a live item
 *                 + neither it nor its build names a model
 */
forward GetItemPreviewModel(Item:itemid);

/**
 * @brief      Paint the square this kind of item stands in.
 *
 * A property of the build and not of the player or the slot, which is the
 * whole reason it is stored here rather than asked for as the grid is
 * drawn: the menu reads one cell per square it is about to change, and a
 * page that changed nothing reads nothing.
 *
 * Zero is the menu's own colour, and is what every build starts at. Almost
 * everything should stay there -- a grid where every square is painted says
 * nothing about any of them.
 *
 * @param      buildid  Build to paint.
 * @param      colour   Colour for its square, or `0` for the menu's own.
 *
 * @date       17:20 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the build is painted
 *             - `false` `buildid` is not a registered build
 */
forward bool:SetItemBuildSlotColour(ItemBuild:buildid, colour);

/**
 * @brief      What the square behind this kind of item is painted.
 *
 * @param      buildid  Build to read.
 *
 * @date       17:20 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - the colour it asked for
 *             - `0` it asked for none, or is not a build
 */
forward GetItemBuildSlotColour(ItemBuild:buildid);

/**
 * # External
 */

stock bool:SetItemBuildPreviewSettings(ItemBuild:buildid, Float:sizeX, Float:sizeY, Float:rotationX = 0.0, Float:rotationY = 0.0, Float:rotationZ = 0.0, Float:zoom = 1.0, Float:offsetX = 0.0, Float:offsetY = 0.0) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_SIZE_X]     = sizeX;
    gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_SIZE_Y]     = sizeY;
    gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_ROTATION_X] = rotationX;
    gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_ROTATION_Y] = rotationY;
    gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_ROTATION_Z] = rotationZ;
    gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_ZOOM]       = zoom;
    gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_OFFSET_X]   = offsetX;
    gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_OFFSET_Y]   = offsetY;

    return true;
}

stock bool:GetItemBuildPreviewSettings(ItemBuild:buildid, &Float:sizeX = 0.0, &Float:sizeY = 0.0, &Float:rotationX = 0.0, &Float:rotationY = 0.0, &Float:rotationZ = 0.0, &Float:zoom = 0.0, &Float:offsetX = 0.0, &Float:offsetY = 0.0) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    sizeX     = gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_SIZE_X];
    sizeY     = gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_SIZE_Y];
    rotationX = gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_ROTATION_X];
    rotationY = gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_ROTATION_Y];
    rotationZ = gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_ROTATION_Z];
    zoom      = gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_ZOOM];
    offsetX   = gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_OFFSET_X];
    offsetY   = gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_OFFSET_Y];

    return true;
}

stock bool:SetItemBuildSlotColour(ItemBuild:buildid, colour) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_COLOUR] = colour;

    return true;
}

stock GetItemBuildSlotColour(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return 0;
    }

    return gItemBuildViewData[buildid][E_ITEM_BUILD_PREVIEW_COLOUR];
}

stock bool:SetItemPreviewModel(Item:itemid, modelid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    new const
        previous = gItemPreviewModel[itemid]
    ;

    if (previous == modelid) {
        return true;
    }

    gItemPreviewModel[itemid] = modelid;

    CallLocalFunction("OnItemPreviewModelChange", "iii", _:itemid, GetItemPreviewModel(itemid), previous);

    return true;
}

stock bool:ResetItemPreviewModel(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    new const
        previous = gItemPreviewModel[itemid]
    ;

    if (previous == 0) {
        return true;
    }

    gItemPreviewModel[itemid] = 0;

    CallLocalFunction("OnItemPreviewModelChange", "iii", _:itemid, GetItemPreviewModel(itemid), previous);

    return true;
}

stock bool:HasItemPreviewModel(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    return gItemPreviewModel[itemid] != 0;
}

stock GetItemPreviewModel(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return 0;
    }

    new const
        modelid = gItemPreviewModel[itemid]
    ;

    return modelid ? modelid : GetItemBuildModel(GetItemBuild(itemid));
}

/**
 * # Calls
 */

hook OnItemDestroy(Item:itemid) {
    gItemPreviewModel[itemid] = 0;

    return 0;
}
