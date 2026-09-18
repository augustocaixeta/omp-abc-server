#if defined _INC_LOCKER
    #endinput
#endif
#define _INC_LOCKER

#include <open.mp>

#include <I\button>
#include <I\PP\button-plus>
#include <I\container>
#include <pp-hooks>

/**
 * # Header
 */

// A locker is three things at once and none of them is an item: an object
// somebody can see, a container that holds things, and a button that opens it.
// A wardrobe, a fridge, a bin. It is never picked up and never carried, which
// is exactly what separates it from a crate -- a crate is an item that happens
// to have an inside.
//
// Because it is three things, creating one is three allocations, and anything
// already taken is given back when one of the others cannot be had. A locker
// either exists whole or does not exist.
//
// The button carries the locker it belongs to as a key, so a press only has to
// read it back off the button that was pressed: no table on the side, and no
// walk over every locker to find which one that button was.

#if !defined MAX_LOCKERS
    #define MAX_LOCKERS (Locker:64)
#endif

#if !defined MAX_LOCKER_SIZE
    #define MAX_LOCKER_SIZE (50)
#endif

#if !defined LOCKER_BUTTON_KEY
    #define LOCKER_BUTTON_KEY (KEY_SECONDARY_ATTACK)
#endif

#if !defined LOCKER_BUTTON_PRIORITY
    #define LOCKER_BUTTON_PRIORITY (0)
#endif

#if !defined LOCKER_BUTTON_SIZE
    #define LOCKER_BUTTON_SIZE (0.85)
#endif

#define INVALID_LOCKER_ID (Locker:-1)

static const
    BUTTON_KEY_LOCKER[] = "@locker"
;

static enum _:E_LOCKER_DATA {
    bool:E_LOCKER_CREATED,

    Container:E_LOCKER_CONTAINER_ID,
    Button:E_LOCKER_BUTTON_ID,
    STREAMER_TAG_OBJECT:E_LOCKER_OBJECT_ID
};

static
    gLockerNextFree[_:MAX_LOCKERS],
    gLockerFreeHead = -1,
    gLockerHighWater,
    gLockerCount,
    gLockerData[MAX_LOCKERS][E_LOCKER_DATA]
;

/**
 * # Functions
 */

/**
 * @brief      Put a locker in the world.
 *
 * The object, the container and the button are made together. The button
 * offsets move it away from the object's own origin, which is what a bin
 * lid or a wardrobe door needs: `buttonOffsetX` is to the object's right and
 * `buttonOffsetY` in front of it, both turned by `rotationZ` so they stay
 * where they were put when the object is turned around.
 *
 * @param      name            What the container is called in a menu.
 * @param      modelid         Object model to place.
 * @param      size            How many slots it holds, up to MAX_LOCKER_SIZE.
 * @param      x               World X of the object.
 * @param      y               World Y of the object.
 * @param      z               World Z of the object.
 * @param      rotationX       Object pitch.
 * @param      rotationY       Object roll.
 * @param      rotationZ       Object yaw, which also turns the button offsets.
 * @param      worldid         Virtual world, `-1` for all of them.
 * @param      interiorid      Interior, `-1` for all of them.
 * @param      streamDistance  How far away the object starts existing.
 * @param      drawDistance    How far away the object starts being drawn.
 * @param      buttonOffsetX   Button offset to the object's right.
 * @param      buttonOffsetY   Button offset in front of the object.
 * @param      buttonOffsetZ   Button offset above the object.
 * @param      call            Whether to fire OnLockerCreate.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the new locker
 *             - `INVALID_LOCKER_ID` when:
 *                 + `size` is below 1 or above MAX_LOCKER_SIZE
 *                 + MAX_LOCKERS are already in the world
 *                 + the container or the button could not be made, in which
 *                   case nothing of the locker is left behind
 */
forward Locker:CreateLocker(const name[], modelid, size, Float:x, Float:y, Float:z, Float:rotationX, Float:rotationY, Float:rotationZ, worldid = -1, interiorid = -1, Float:streamDistance = 100.0, Float:drawDistance = 100.0, Float:buttonOffsetX = 0.0, Float:buttonOffsetY = 0.0, Float:buttonOffsetZ = 0.0, bool:call = true);

/**
 * @brief      Take a locker back out of the world.
 *
 * The object, the button and the container all go. `clear` decides what
 * happens to what was inside: destroyed with it, or left as loose items for
 * the caller to place somewhere.
 *
 * OnLockerDestroy fires before anything is taken apart, so a handler can
 * still read what is in it.
 *
 * @param      lockerid  Locker to remove.
 * @param      clear     Whether to destroy the items inside it too.
 * @param      call      Whether to fire OnLockerDestroy.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is gone
 *             - `false` `lockerid` is not a live locker
 */
forward bool:DestroyLocker(Locker:lockerid, bool:clear = true, bool:call = true);

/**
 * @brief      Whether a locker exists.
 *
 * @param      lockerid  Locker to check.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is in the world
 *             - `false` when:
 *                 + `lockerid` is out of range
 *                 + it was never made, or has been destroyed
 */
forward bool:IsValidLocker(Locker:lockerid);

/**
 * @brief      How many lockers are in the world.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many exist right now
 */
forward GetLockerCount();

/**
 * @brief      What a locker is called.
 *
 * The container's name, which is what a menu puts at the top of it.
 *
 * @param      lockerid  Locker to read.
 * @param      output    Reads back the name.
 * @param      size      How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` `lockerid` is not a live locker
 */
forward bool:GetLockerName(Locker:lockerid, output[], size = sizeof (output));

/**
 * @brief      Which interior a locker is in.
 *
 * @param      lockerid  Locker to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the interior it was made in
 *             - `-1` when:
 *                 + `lockerid` is not a live locker
 *                 + it is in every interior
 */
forward GetLockerInterior(Locker:lockerid);

/**
 * @brief      Which virtual world a locker is in.
 *
 * @param      lockerid  Locker to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the world it was made in
 *             - `-1` when:
 *                 + `lockerid` is not a live locker
 *                 + it is in every world
 */
forward GetLockerVirtualWorld(Locker:lockerid);

/**
 * @brief      Where a locker is.
 *
 * The object's position by default, which is where the thing is. Ask for
 * `buttonOffsets` to get the button's instead, which is where a character
 * has to stand to open it -- the two are the same only when no offsets were
 * given.
 *
 * @param      lockerid       Locker to read.
 * @param      x              Reads back world X.
 * @param      y              Reads back world Y.
 * @param      z              Reads back world Z.
 * @param      buttonOffsets  Whether to answer the button rather than the
 *                            object.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the outputs were written
 *             - `false` `lockerid` is not a live locker
 */
forward bool:GetLockerPos(Locker:lockerid, &Float:x, &Float:y, &Float:z, bool:buttonOffsets = false);

/**
 * @brief      Which way a locker is turned.
 *
 * @param      lockerid   Locker to read.
 * @param      rotationX  Reads back pitch.
 * @param      rotationY  Reads back roll.
 * @param      rotationZ  Reads back yaw.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the outputs were written
 *             - `false` `lockerid` is not a live locker
 */
forward bool:GetLockerRot(Locker:lockerid, &Float:rotationX, &Float:rotationY, &Float:rotationZ);

/**
 * @brief      Put an item into a locker.
 *
 * `playerid` is who is doing it, for the container's own events to report --
 * a locker filled by the script rather than by a character leaves it as
 * `INVALID_PLAYER_ID`.
 *
 * @param      lockerid    Locker to fill.
 * @param      itemid      Item to put in.
 * @param      index       Reads back the slot it went into.
 * @param      addToIndex  Slot to use, or `-1` for the first free one.
 * @param      playerid    Who is putting it in, if anybody.
 * @param      call        Whether to fire the container's events.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is inside
 *             - `false` when:
 *                 + `lockerid` is not a live locker
 *                 + the item is not live, or is somewhere else already
 *                 + `addToIndex` is outside the capacity, or in use
 *                 + the locker is full
 */
forward bool:AddItemToLocker(Locker:lockerid, Item:itemid, &index = -1, addToIndex = -1, playerid = INVALID_PLAYER_ID, bool:call = true);

/**
 * @brief      Take whatever is in a slot back out of a locker.
 *
 * The item is not destroyed and not put anywhere: it comes out held by
 * nothing, and the caller decides where it goes next.
 *
 * @param      lockerid  Locker to empty a slot of.
 * @param      index     Slot to empty.
 * @param      itemid    Reads back what came out.
 * @param      playerid  Who is taking it, if anybody.
 * @param      call      Whether to fire the container's events.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the slot is empty and `itemid` is what was in it
 *             - `false` when:
 *                 + `lockerid` is not a live locker
 *                 + `index` is outside the capacity
 *                 + the slot was already empty
 */
forward bool:RemoveItemFromLocker(Locker:lockerid, index, &Item:itemid = INVALID_ITEM_ID, playerid = INVALID_PLAYER_ID, bool:call = true);

/**
 * @brief      How many slots a locker has.
 *
 * @param      lockerid  Locker to measure.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the slot count it was made with
 *             - `0` `lockerid` is not a live locker
 */
forward GetLockerCapacity(Locker:lockerid);

/**
 * @brief      How many of those slots are in use.
 *
 * @param      lockerid  Locker to count.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many items are in it
 *             - `0` when:
 *                 + `lockerid` is not a live locker
 *                 + it is empty
 */
forward GetLockerSize(Locker:lockerid);

/**
 * @brief      Whether a locker holds nothing.
 *
 * @param      lockerid  Locker to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` there is nothing in it
 *             - `false` when:
 *                 + `lockerid` is not a live locker
 *                 + something is in it
 */
forward bool:IsLockerEmpty(Locker:lockerid);

/**
 * @brief      Whether there is no room left in a locker.
 *
 * @param      lockerid  Locker to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` every slot is used
 *             - `false` when:
 *                 + `lockerid` is not a live locker
 *                 + at least one slot is free
 */
forward bool:IsLockerFull(Locker:lockerid);

/**
 * @brief      Whether one particular slot holds something.
 *
 * @param      lockerid  Locker to ask.
 * @param      index     Slot to look at.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` an item is in it
 *             - `false` when:
 *                 + `lockerid` is not a live locker
 *                 + `index` is outside the capacity
 *                 + the slot is empty
 */
forward bool:IsLockerSlotUsed(Locker:lockerid, index);

/**
 * @brief      What is in one particular slot.
 *
 * @param      lockerid  Locker to read.
 * @param      index     Slot to look at.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the item in it
 *             - `INVALID_ITEM_ID` when:
 *                 + `lockerid` is not a live locker
 *                 + `index` is outside the capacity
 *                 + the slot is empty
 */
forward Item:GetLockerSlotItem(Locker:lockerid, index);

/**
 * @brief      The first slot with nothing in it.
 *
 * Slots are not compacted, so this is the first gap rather than the end.
 *
 * @param      lockerid  Locker to search.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the index of the first free slot
 *             - `-1` when:
 *                 + `lockerid` is not a live locker
 *                 + it is full
 */
forward GetLockerFreeSlot(Locker:lockerid);

/**
 * @brief      The container a locker holds things in.
 *
 * For code that already speaks container -- a menu, a transfer -- and does
 * not need to know that this one happens to be a locker.
 *
 * @param      lockerid  Locker to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - its container
 *             - `INVALID_CONTAINER_ID` `lockerid` is not a live locker
 */
forward Container:GetLockerContainer(Locker:lockerid);

/**
 * @brief      The object a locker is drawn as.
 *
 * For moving it, restyling it, or attaching something to it. Destroying it
 * leaves a locker that can still be opened by a button nobody can see.
 *
 * @param      lockerid  Locker to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - its dynamic object
 *             - `INVALID_STREAMER_ID` `lockerid` is not a live locker
 */
forward STREAMER_TAG_OBJECT:GetLockerObject(Locker:lockerid);

/**
 * @brief      What a character standing at a locker is told they can do.
 *
 * The button's popup, so it is only on screen while they are close enough
 * to press it.
 *
 * @param      lockerid  Locker to write.
 * @param      message   Text, with placeholders if anything follows it.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the popup was written
 *             - `false` `lockerid` is not a live locker
 */
forward bool:SetLockerPopupText(Locker:lockerid, const message[], OPEN_MP_TAGS:...);

/**
 * @brief      What a locker's popup currently says.
 *
 * @param      lockerid  Locker to read.
 * @param      output    Reads back the popup text.
 * @param      size      How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` `lockerid` is not a live locker
 */
forward bool:GetLockerPopupText(Locker:lockerid, output[], size = sizeof (output));

/**
 * @brief      Hang a 3D label over a locker.
 *
 * Unlike the popup this is drawn in the world for anybody in range, whether
 * they are close enough to open it or not. Calling it again replaces the
 * label rather than adding a second one.
 *
 * @param      lockerid        Locker to label.
 * @param      message         Text, with placeholders if anything follows
 *                             `offsetZ`.
 * @param      colour          Label colour.
 * @param      drawDistance    How far away it starts being drawn.
 * @param      streamDistance  How far away it starts existing.
 * @param      testLOS         Whether a wall between hides it.
 * @param      offsetZ         How far above the button it floats.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the label is up
 *             - `false` `lockerid` is not a live locker
 */
forward bool:SetLockerLabelText(Locker:lockerid, const message[], colour = 0xFFFFFFFF, Float:drawDistance = BUTTON_DEFAULT_DRAW_DISTANCE, Float:streamDistance = BUTTON_DEFAULT_STREAM_DISTANCE, bool:testLOS = true, Float:offsetZ = 0.0, OPEN_MP_TAGS:...);

/**
 * @brief      The button that opens a locker.
 *
 * @param      lockerid  Locker to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - its button
 *             - `INVALID_BUTTON_ID` `lockerid` is not a live locker
 */
forward Button:GetLockerButton(Locker:lockerid);

/**
 * @brief      Whether a button belongs to a locker.
 *
 * What a handler watching every button press asks first, to find out
 * whether this press is any of its business.
 *
 * @param      buttonid  Button to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it opens a locker
 *             - `false` when:
 *                 + `buttonid` is not a live button
 *                 + it is some other kind of button
 */
forward bool:HasButtonLocker(Button:buttonid);

/**
 * @brief      Which locker a button opens.
 *
 * @param      buttonid  Button to look up.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the locker it belongs to
 *             - `INVALID_LOCKER_ID` when:
 *                 + `buttonid` is not a live button
 *                 + it is some other kind of button
 */
forward Locker:GetButtonLocker(Button:buttonid);

/**
 * @brief      The locker a character is standing at.
 *
 * Whichever button they are in range of, if that button is a locker's. A
 * character standing at two things at once gets the one the button module
 * picked, which is the one they are facing.
 *
 * @param      playerid  Player to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the locker in front of them
 *             - `INVALID_LOCKER_ID` when:
 *                 + they are at no button
 *                 + the button they are at is not a locker's
 */
forward Locker:GetPlayerLocker(playerid);

/**
 * # Events
 */

/**
 * @brief      A locker has been put in the world.
 *
 * Fired once all three parts exist, so the container can be filled and the
 * button labelled from here.
 *
 * @param      lockerid  The new locker.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnLockerCreate(Locker:lockerid);

/**
 * @brief      A locker is about to be taken apart.
 *
 * Fired while all three parts are still whole, so what is inside can be
 * read and moved somewhere else before `clear` decides its fate.
 *
 * @param      lockerid  The locker, still live for this call.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnLockerDestroy(Locker:lockerid);

/**
 * @brief      A character has pressed a locker's button.
 *
 * Where a menu is opened. Nothing is opened by the framework itself: a
 * locker is a container and a button, and what that looks like on screen is
 * the gamemode's to decide.
 *
 * The return value is not used, and the button press goes on to whatever
 * else is listening for it.
 *
 * @param      playerid  Character who pressed it.
 * @param      lockerid  Locker they pressed.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerOpenLocker(playerid, Locker:lockerid);

/**
 * # Internal
 */

static stock GetFreeLockerIndexInternal() {
    if (gLockerFreeHead != -1) {
        new const
            index = gLockerFreeHead
        ;

        gLockerFreeHead = gLockerNextFree[index];

        return index;
    }

    if (gLockerHighWater != _:MAX_LOCKERS) {
        return gLockerHighWater++;
    }

    return -1;
}

static stock Locker:AllocLockerInternal() {
    new const
        index = GetFreeLockerIndexInternal()
    ;

    if (index == -1) {
        return INVALID_LOCKER_ID;
    }

    ++gLockerCount;

    return Locker:index;
}

static stock FreeLockerInternal(Locker:lockerid) {
    gLockerNextFree[_:lockerid] = gLockerFreeHead;
    gLockerFreeHead = _:lockerid;

    --gLockerCount;
}

static stock bool:IsValidLockerInternal(Locker:lockerid) {
    if (!(0 <= _:lockerid < _:MAX_LOCKERS)) {
        return false;
    }

    return gLockerData[lockerid][E_LOCKER_CREATED];
}

/**
 * # External
 */

stock Locker:CreateLocker(const name[], modelid, size, Float:x, Float:y, Float:z, Float:rotationX, Float:rotationY, Float:rotationZ, worldid = -1, interiorid = -1, Float:streamDistance = 100.0, Float:drawDistance = 100.0, Float:buttonOffsetX = 0.0, Float:buttonOffsetY = 0.0, Float:buttonOffsetZ = 0.0, bool:call = true) {
    if (!(1 <= size <= MAX_LOCKER_SIZE)) {
        return INVALID_LOCKER_ID;
    }

    new const
        Locker:lockerid = AllocLockerInternal()
    ;

    if (lockerid == INVALID_LOCKER_ID) {
        return INVALID_LOCKER_ID;
    }

    new const
        Container:containerid = CreateContainer(name, size)
    ;

    if (containerid == INVALID_CONTAINER_ID) {
        FreeLockerInternal(lockerid);

        return INVALID_LOCKER_ID;
    }

    new const
        Float:sine = floatsin(rotationZ, degrees),
        Float:cosine = floatcos(rotationZ, degrees)
    ;

    new const
        Button:buttonid = CreateButton(
            x + (buttonOffsetX * cosine) - (buttonOffsetY * sine),
            y + (buttonOffsetX * sine) + (buttonOffsetY * cosine),
            z + buttonOffsetZ,
            LOCKER_BUTTON_KEY,
            LOCKER_BUTTON_SIZE,
            worldid,
            interiorid,
            LOCKER_BUTTON_PRIORITY
        )
    ;

    if (buttonid == INVALID_BUTTON_ID) {
        DestroyContainer(containerid);
        FreeLockerInternal(lockerid);

        return INVALID_LOCKER_ID;
    }

    gLockerData[lockerid][E_LOCKER_CREATED]      = true;
    gLockerData[lockerid][E_LOCKER_CONTAINER_ID] = containerid;
    gLockerData[lockerid][E_LOCKER_BUTTON_ID]    = buttonid;
    gLockerData[lockerid][E_LOCKER_OBJECT_ID]    = CreateDynamicObject(modelid, x, y, z, rotationX, rotationY, rotationZ, worldid, interiorid, .streamdistance = streamDistance, .drawdistance = drawDistance);

    SetButtonExtraData(buttonid, BUTTON_KEY_LOCKER, _:lockerid);

    if (call) {
        CallLocalFunction("OnLockerCreate", "i", _:lockerid);
    }

    return lockerid;
}

stock bool:DestroyLocker(Locker:lockerid, bool:clear = true, bool:call = true) {
    if (!IsValidLockerInternal(lockerid)) {
        return false;
    }

    gLockerData[lockerid][E_LOCKER_CREATED] = false;

    if (call) {
        CallLocalFunction("OnLockerDestroy", "i", _:lockerid);
    }

    DestroyContainer(gLockerData[lockerid][E_LOCKER_CONTAINER_ID], clear);
    DestroyButton(gLockerData[lockerid][E_LOCKER_BUTTON_ID]);
    DestroyDynamicObject(gLockerData[lockerid][E_LOCKER_OBJECT_ID]);

    static const
        LOCKER_DATA_NONE[E_LOCKER_DATA]
    ;

    gLockerData[lockerid] = LOCKER_DATA_NONE;

    FreeLockerInternal(lockerid);

    return true;
}

stock bool:IsValidLocker(Locker:lockerid) {
    return IsValidLockerInternal(lockerid);
}

stock GetLockerCount() {
    return gLockerCount;
}

stock bool:GetLockerName(Locker:lockerid, output[], size = sizeof (output)) {
    if (!IsValidLockerInternal(lockerid)) {
        return false;
    }

    return GetContainerName(gLockerData[lockerid][E_LOCKER_CONTAINER_ID], output, size);
}

stock GetLockerInterior(Locker:lockerid) {
    if (!IsValidLockerInternal(lockerid)) {
        return -1;
    }

    return GetButtonInterior(gLockerData[lockerid][E_LOCKER_BUTTON_ID]);
}

stock GetLockerVirtualWorld(Locker:lockerid) {
    if (!IsValidLockerInternal(lockerid)) {
        return -1;
    }

    return GetButtonVirtualWorld(gLockerData[lockerid][E_LOCKER_BUTTON_ID]);
}

stock bool:GetLockerPos(Locker:lockerid, &Float:x, &Float:y, &Float:z, bool:buttonOffsets = false) {
    if (!IsValidLockerInternal(lockerid)) {
        return false;
    }

    if (buttonOffsets) {
        return GetButtonPos(gLockerData[lockerid][E_LOCKER_BUTTON_ID], x, y, z);
    }

    GetDynamicObjectPos(gLockerData[lockerid][E_LOCKER_OBJECT_ID], x, y, z);

    return true;
}

stock bool:GetLockerRot(Locker:lockerid, &Float:rotationX, &Float:rotationY, &Float:rotationZ) {
    if (!IsValidLockerInternal(lockerid)) {
        return false;
    }

    GetDynamicObjectRot(gLockerData[lockerid][E_LOCKER_OBJECT_ID], rotationX, rotationY, rotationZ);

    return true;
}

stock bool:AddItemToLocker(Locker:lockerid, Item:itemid, &index = -1, addToIndex = -1, playerid = INVALID_PLAYER_ID, bool:call = true) {
    if (!IsValidLockerInternal(lockerid)) {
        return false;
    }

    return AddItemToContainer(gLockerData[lockerid][E_LOCKER_CONTAINER_ID], itemid, index, addToIndex, playerid, call);
}

stock bool:RemoveItemFromLocker(Locker:lockerid, index, &Item:itemid = INVALID_ITEM_ID, playerid = INVALID_PLAYER_ID, bool:call = true) {
    if (!IsValidLockerInternal(lockerid)) {
        return false;
    }

    return RemoveItemFromContainer(gLockerData[lockerid][E_LOCKER_CONTAINER_ID], index, itemid, playerid, call);
}

stock GetLockerCapacity(Locker:lockerid) {
    if (!IsValidLockerInternal(lockerid)) {
        return 0;
    }

    return GetContainerCapacity(gLockerData[lockerid][E_LOCKER_CONTAINER_ID]);
}

stock GetLockerSize(Locker:lockerid) {
    if (!IsValidLockerInternal(lockerid)) {
        return 0;
    }

    return GetContainerSize(gLockerData[lockerid][E_LOCKER_CONTAINER_ID]);
}

stock bool:IsLockerEmpty(Locker:lockerid) {
    if (!IsValidLockerInternal(lockerid)) {
        return false;
    }

    return IsContainerEmpty(gLockerData[lockerid][E_LOCKER_CONTAINER_ID]);
}

stock bool:IsLockerFull(Locker:lockerid) {
    if (!IsValidLockerInternal(lockerid)) {
        return false;
    }

    return IsContainerFull(gLockerData[lockerid][E_LOCKER_CONTAINER_ID]);
}

stock bool:IsLockerSlotUsed(Locker:lockerid, index) {
    if (!IsValidLockerInternal(lockerid)) {
        return false;
    }

    return IsContainerSlotUsed(gLockerData[lockerid][E_LOCKER_CONTAINER_ID], index);
}

stock Item:GetLockerSlotItem(Locker:lockerid, index) {
    if (!IsValidLockerInternal(lockerid)) {
        return INVALID_ITEM_ID;
    }

    return GetContainerSlotItem(gLockerData[lockerid][E_LOCKER_CONTAINER_ID], index);
}

stock GetLockerFreeSlot(Locker:lockerid) {
    if (!IsValidLockerInternal(lockerid)) {
        return -1;
    }

    return GetContainerFreeSlot(gLockerData[lockerid][E_LOCKER_CONTAINER_ID]);
}

stock Container:GetLockerContainer(Locker:lockerid) {
    if (!IsValidLockerInternal(lockerid)) {
        return INVALID_CONTAINER_ID;
    }

    return gLockerData[lockerid][E_LOCKER_CONTAINER_ID];
}

stock STREAMER_TAG_OBJECT:GetLockerObject(Locker:lockerid) {
    if (!IsValidLockerInternal(lockerid)) {
        return STREAMER_TAG_OBJECT:INVALID_STREAMER_ID;
    }

    return gLockerData[lockerid][E_LOCKER_OBJECT_ID];
}

stock bool:SetLockerPopupText(Locker:lockerid, const message[], OPEN_MP_TAGS:...) {
    if (!IsValidLockerInternal(lockerid)) {
        return false;
    }

    return SetButtonPopupText(gLockerData[lockerid][E_LOCKER_BUTTON_ID], message, ___(2));
}

stock bool:GetLockerPopupText(Locker:lockerid, output[], size = sizeof (output)) {
    if (!IsValidLockerInternal(lockerid)) {
        return false;
    }

    return GetButtonPopupText(gLockerData[lockerid][E_LOCKER_BUTTON_ID], output, size);
}

stock bool:SetLockerLabelText(Locker:lockerid, const message[], colour = 0xFFFFFFFF, Float:drawDistance = BUTTON_DEFAULT_DRAW_DISTANCE, Float:streamDistance = BUTTON_DEFAULT_STREAM_DISTANCE, bool:testLOS = true, Float:offsetZ = 0.0, OPEN_MP_TAGS:...) {
    if (!IsValidLockerInternal(lockerid)) {
        return false;
    }

    return SetButtonLabelText(gLockerData[lockerid][E_LOCKER_BUTTON_ID], message, colour, drawDistance, streamDistance, testLOS, offsetZ, ___(7));
}

stock Button:GetLockerButton(Locker:lockerid) {
    if (!IsValidLockerInternal(lockerid)) {
        return INVALID_BUTTON_ID;
    }

    return gLockerData[lockerid][E_LOCKER_BUTTON_ID];
}

stock bool:HasButtonLocker(Button:buttonid) {
    return HasButtonExtraData(buttonid, BUTTON_KEY_LOCKER);
}

stock Locker:GetButtonLocker(Button:buttonid) {
    if (!HasButtonExtraData(buttonid, BUTTON_KEY_LOCKER)) {
        return INVALID_LOCKER_ID;
    }

    return Locker:GetButtonExtraData(buttonid, BUTTON_KEY_LOCKER);
}

stock Locker:GetPlayerLocker(playerid) {
    return GetButtonLocker(GetPlayerButton(playerid));
}

/**
 * # Calls
 */

hook OnButtonPress(playerid, Button:buttonid) {
    new const
        Locker:lockerid = GetButtonLocker(buttonid)
    ;

    if (lockerid == INVALID_LOCKER_ID) {
        return 0;
    }

    CallLocalFunction("OnPlayerOpenLocker", "ii", playerid, _:lockerid);

    return 0;
}
