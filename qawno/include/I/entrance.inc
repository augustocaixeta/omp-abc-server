#if defined _INC_ENTRANCE
    #endinput
#endif
#define _INC_ENTRANCE

#include <open.mp>

#include <I\button>
#include <pp-hooks>

/**
 * # Header
 */

// A door: two places and a button at each, so pressing one puts the character
// down at the other. The library knows nothing about what is behind it -- a
// shop, a house, a cell -- only that the two sides are the two sides.
//
// A door with one side is worse than no door, because a character would walk in
// and have no way back out. So an entrance either gets both buttons or is not
// made at all.

#if !defined MAX_ENTRANCES
    #define MAX_ENTRANCES (Entrance:1024)
#endif

#if !defined ENTRANCE_DEFAULT_BUTTON_KEY
    #define ENTRANCE_DEFAULT_BUTTON_KEY (KEY_SECONDARY_ATTACK)
#endif

#if !defined ENTRANCE_DEFAULT_BUTTON_PRIORITY
    #define ENTRANCE_DEFAULT_BUTTON_PRIORITY (0)
#endif

#if !defined ENTRANCE_DEFAULT_BUTTON_SIZE
    #define ENTRANCE_DEFAULT_BUTTON_SIZE (1.0)
#endif

// What the prompt on each side reads, and the key token that goes into it.
// Generic on purpose: this library does not know what is behind the door, and a
// script that does can write over either one through GetEntranceExteriorButton
// and GetEntranceInteriorButton.
#if !defined ENTRANCE_KEY_TEXT
    #define ENTRANCE_KEY_TEXT "~k~~VEHICLE_ENTER_EXIT~"
#endif

#if !defined ENTRANCE_EXTERIOR_POPUP
    #define ENTRANCE_EXTERIOR_POPUP "Press %s to enter"
#endif

#if !defined ENTRANCE_INTERIOR_POPUP
    #define ENTRANCE_INTERIOR_POPUP "Press %s to leave"
#endif

// How far in front of the door the player is put down, so they do not land
// inside the doorway they just walked through.
#if !defined ENTRANCE_DEFAULT_FORWARD_OFFSET
    #define ENTRANCE_DEFAULT_FORWARD_OFFSET (1.0)
#endif

#define INVALID_ENTRANCE_ID (Entrance:-1)

static enum _:E_ENTRANCE_DATA {
    bool:E_ENTRANCE_CREATED,

    Float:E_ENTRANCE_EXT_X,
    Float:E_ENTRANCE_EXT_Y,
    Float:E_ENTRANCE_EXT_Z,
    Float:E_ENTRANCE_EXT_A,

    E_ENTRANCE_EXT_WORLD_ID,
    E_ENTRANCE_EXT_INTERIOR_ID,

    Float:E_ENTRANCE_INT_X,
    Float:E_ENTRANCE_INT_Y,
    Float:E_ENTRANCE_INT_Z,
    Float:E_ENTRANCE_INT_A,

    E_ENTRANCE_INT_WORLD_ID,
    E_ENTRANCE_INT_INTERIOR_ID,

    Button:E_ENTRANCE_EXT_BUTTON_ID,
    Button:E_ENTRANCE_INT_BUTTON_ID
};

// Which entrance a button belongs to, and which side of it. A press arrives
// holding a button and this is what turns it back into a door.
//
// It is an array rather than button extra data because of where it is read: not
// only on a press, but on every walk into and out of every button area in the
// server. A map keyed by a string answers the same question through a call out
// to the plugin, and that is the wrong price to pay on a path players cross by
// simply walking around.
static enum _:E_ENTRANCE_BUTTON_DATA {
    bool:E_ENTRANCE_BUTTON_OWNED,
    Entrance:E_ENTRANCE_BUTTON_ENTRANCE_ID,
    bool:E_ENTRANCE_BUTTON_INSIDE
};

// The array hands out the ids as well as holding the doors, the same way the
// buttons themselves are kept: a free list, with slots never used yet coming
// off a high water mark. See the note in button.inc for what that was measured
// against.
static
    gEntranceNextFree[_:MAX_ENTRANCES],
    gEntranceFreeHead = -1,
    gEntranceHighWater,
    gEntranceCount,
    gEntranceData[MAX_ENTRANCES][E_ENTRANCE_DATA],
    gEntranceButtonData[MAX_BUTTONS][E_ENTRANCE_BUTTON_DATA]
;

/**
 * # Functions
 */

/**
 * @brief      Put a door in the world.
 *
 * Both sides are made together: a button outside and a button inside, each
 * in its own world and interior. Pressing either one takes the character to
 * the other side.
 *
 * The two facing angles are which way the character is left looking once
 * they arrive, and they are also what the forward offset is measured
 * along -- so a character comes out facing away from the door rather than
 * standing in the doorway.
 *
 * @param      extX         World X outside.
 * @param      extY         World Y outside.
 * @param      extZ         World Z outside.
 * @param      extA         Which way they face when put outside.
 * @param      extWorld     Virtual world outside.
 * @param      extInterior  Interior outside.
 * @param      intX         World X inside.
 * @param      intY         World Y inside.
 * @param      intZ         World Z inside.
 * @param      intA         Which way they face when put inside.
 * @param      intWorld     Virtual world inside.
 * @param      intInterior  Interior inside.
 * @param      buttonKey    Which key opens it, on both sides.
 * @param      priority     Which button wins when two overlap.
 * @param      call         Whether to fire OnEntranceCreate.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the new entrance
 *             - `INVALID_ENTRANCE_ID` when:
 *                 + MAX_ENTRANCES already exist
 *                 + either button could not be made, in which case neither
 *                   is left behind
 */
forward Entrance:CreateEntrance(Float:extX, Float:extY, Float:extZ, Float:extA, extWorld, extInterior, Float:intX, Float:intY, Float:intZ, Float:intA, intWorld, intInterior, KEY:buttonKey = ENTRANCE_DEFAULT_BUTTON_KEY, priority = ENTRANCE_DEFAULT_BUTTON_PRIORITY, bool:call = true);

/**
 * @brief      Take a door back out of the world.
 *
 * Both buttons go with it. Anybody already on the far side stays there:
 * this removes the way through, not the places.
 *
 * OnEntranceDestroy fires while the entrance can still be read, so a
 * handler can write down where it was.
 *
 * @param      entranceid  Entrance to remove.
 * @param      call        Whether to fire OnEntranceDestroy.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is gone
 *             - `false` `entranceid` is not a live entrance
 */
forward bool:DestroyEntrance(Entrance:entranceid, bool:call = true);

/**
 * @brief      Whether a door exists.
 *
 * @param      entranceid  Entrance to check.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is in the world
 *             - `false` when:
 *                 + `entranceid` is out of range
 *                 + it was never made, or has been destroyed
 */
forward bool:IsValidEntrance(Entrance:entranceid);

/**
 * @brief      How many doors are in the world.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many exist right now
 */
forward GetEntranceCount();

/**
 * @brief      Where the outside of a door is.
 *
 * @param      entranceid  Entrance to read.
 * @param      x           Reads back world X.
 * @param      y           Reads back world Y.
 * @param      z           Reads back world Z.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the outputs were written
 *             - `false` `entranceid` is not a live entrance
 */
forward bool:GetEntranceExteriorPos(Entrance:entranceid, &Float:x, &Float:y, &Float:z);

/**
 * @brief      Which way a character faces when put outside.
 *
 * @param      entranceid  Entrance to read.
 * @param      a           Reads back the angle.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` `entranceid` is not a live entrance
 */
forward bool:GetEntranceExteriorFacingAngle(Entrance:entranceid, &Float:a);

/**
 * @brief      Which virtual world the outside of a door is in.
 *
 * @param      entranceid  Entrance to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the world it was made in
 *             - `-1` when:
 *                 + `entranceid` is not a live entrance
 *                 + it is in every world
 */
forward GetEntranceExteriorVirtualWorld(Entrance:entranceid);

/**
 * @brief      Which interior the outside of a door is in.
 *
 * @param      entranceid  Entrance to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the interior it was made in
 *             - `-1` when:
 *                 + `entranceid` is not a live entrance
 *                 + it is in every interior
 */
forward GetEntranceExteriorInterior(Entrance:entranceid);

/**
 * @brief      The button on the outside of a door.
 *
 * For writing over the popup, which is generic here because this library
 * does not know what is behind the door.
 *
 * @param      entranceid  Entrance to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - its outside button
 *             - `INVALID_BUTTON_ID` `entranceid` is not a live entrance
 */
forward Button:GetEntranceExteriorButton(Entrance:entranceid);

/**
 * @brief      Whether a button is the outside of a door.
 *
 * @param      buttonid  Button to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is an entrance's outside button
 *             - `false` when:
 *                 + `buttonid` is not a live button
 *                 + it is an inside button, or some other kind entirely
 */
forward bool:HasButtonEntranceExterior(Button:buttonid);

/**
 * @brief      Which door a button is the outside of.
 *
 * @param      buttonid  Button to look up.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the entrance it belongs to
 *             - `INVALID_ENTRANCE_ID` when:
 *                 + `buttonid` is not a live button
 *                 + it is an inside button, or some other kind entirely
 */
forward Entrance:GetButtonEntranceExterior(Button:buttonid);

/**
 * @brief      Put a character outside a door.
 *
 * Moves them, turns them to face away from it, and puts the camera behind
 * them. The offset keeps them out of the doorway itself.
 *
 * This is the move without the press, so nothing asks permission: it fires
 * OnPlayerLeftEntrance to say it has happened, not OnPlayerLeaveEntrance to
 * ask whether it may.
 *
 * @param      playerid    Character to move.
 * @param      entranceid  Entrance to put them outside.
 * @param      offset      How far in front of the door to leave them.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they are outside
 *             - `false` when:
 *                 + `entranceid` is not a live entrance
 *                 + `playerid` is not connected
 */
forward bool:SetPlayerEntranceExterior(playerid, Entrance:entranceid, Float:offset = ENTRANCE_DEFAULT_FORWARD_OFFSET);

/**
 * @brief      The door a character is standing outside.
 *
 * Whichever button they are in range of, if it is an entrance's outside.
 *
 * @param      playerid  Player to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the entrance in front of them
 *             - `INVALID_ENTRANCE_ID` when:
 *                 + they are at no button
 *                 + the button they are at is not an entrance's outside
 */
forward Entrance:GetPlayerEntranceExterior(playerid);

/**
 * @brief      The outside button a character is standing at.
 *
 * @param      playerid  Player to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the button in front of them
 *             - `INVALID_BUTTON_ID` when:
 *                 + they are at no button
 *                 + the button they are at is not an entrance's outside
 */
forward Button:GetPlayerEntranceExteriorButton(playerid);

/**
 * @brief      Where the inside of a door is.
 *
 * @param      entranceid  Entrance to read.
 * @param      x           Reads back world X.
 * @param      y           Reads back world Y.
 * @param      z           Reads back world Z.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the outputs were written
 *             - `false` `entranceid` is not a live entrance
 */
forward bool:GetEntranceInteriorPos(Entrance:entranceid, &Float:x, &Float:y, &Float:z);

/**
 * @brief      Which way a character faces when put inside.
 *
 * @param      entranceid  Entrance to read.
 * @param      a           Reads back the angle.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` `entranceid` is not a live entrance
 */
forward bool:GetEntranceInteriorFacingAngle(Entrance:entranceid, &Float:a);

/**
 * @brief      Which virtual world the inside of a door is in.
 *
 * @param      entranceid  Entrance to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the world it was made in
 *             - `-1` when:
 *                 + `entranceid` is not a live entrance
 *                 + it is in every world
 */
forward GetEntranceInteriorVirtualWorld(Entrance:entranceid);

/**
 * @brief      Which interior the inside of a door is in.
 *
 * @param      entranceid  Entrance to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the interior it was made in
 *             - `-1` when:
 *                 + `entranceid` is not a live entrance
 *                 + it is in every interior
 */
forward GetEntranceInteriorInterior(Entrance:entranceid);

/**
 * @brief      The button on the inside of a door.
 *
 * @param      entranceid  Entrance to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - its inside button
 *             - `INVALID_BUTTON_ID` `entranceid` is not a live entrance
 */
forward Button:GetEntranceInteriorButton(Entrance:entranceid);

/**
 * @brief      Whether a button is the inside of a door.
 *
 * @param      buttonid  Button to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is an entrance's inside button
 *             - `false` when:
 *                 + `buttonid` is not a live button
 *                 + it is an outside button, or some other kind entirely
 */
forward bool:HasButtonEntranceInterior(Button:buttonid);

/**
 * @brief      Which door a button is the inside of.
 *
 * @param      buttonid  Button to look up.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the entrance it belongs to
 *             - `INVALID_ENTRANCE_ID` when:
 *                 + `buttonid` is not a live button
 *                 + it is an outside button, or some other kind entirely
 */
forward Entrance:GetButtonEntranceInterior(Button:buttonid);

/**
 * @brief      Put a character inside a door.
 *
 * The mirror of SetPlayerEntranceExterior, and equally the move without the
 * press: it fires OnPlayerEnteredEntrance to say it has happened, not
 * OnPlayerEnterEntrance to ask whether it may.
 *
 * @param      playerid    Character to move.
 * @param      entranceid  Entrance to put them inside.
 * @param      offset      How far in front of the door to leave them.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they are inside
 *             - `false` when:
 *                 + `entranceid` is not a live entrance
 *                 + `playerid` is not connected
 */
forward bool:SetPlayerEntranceInterior(playerid, Entrance:entranceid, Float:offset = ENTRANCE_DEFAULT_FORWARD_OFFSET);

/**
 * @brief      The door a character is standing inside.
 *
 * @param      playerid  Player to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the entrance in front of them
 *             - `INVALID_ENTRANCE_ID` when:
 *                 + they are at no button
 *                 + the button they are at is not an entrance's inside
 */
forward Entrance:GetPlayerEntranceInterior(playerid);

/**
 * @brief      The inside button a character is standing at.
 *
 * @param      playerid  Player to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the button in front of them
 *             - `INVALID_BUTTON_ID` when:
 *                 + they are at no button
 *                 + the button they are at is not an entrance's inside
 */
forward Button:GetPlayerEntranceInteriorButton(playerid);

/**
 * # Events
 *
 * Where a return of 1 stops something, it stops it the way pp-hooks stops
 * anything: a hook that returns something other than zero ends the chain there,
 * so neither the hooks after it nor the callback itself are reached.
 */

/**
 * @brief      A door has been put in the world.
 *
 * Fired once both sides exist, so this is where a popup saying what is
 * actually behind it goes.
 *
 * @param      entranceid  The new entrance.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnEntranceCreate(Entrance:entranceid);

/**
 * @brief      A door is about to be taken out of the world.
 *
 * Fired while both sides can still be read.
 *
 * @param      entranceid  The entrance, still live for this call.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnEntranceDestroy(Entrance:entranceid);

/**
 * @brief      A character walked up to the outside of a door.
 *
 * @param      playerid    Character who arrived.
 * @param      entranceid  Entrance they are outside.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` other OnPlayerEnterButtonArea hooks still run
 *             - `1` the chain ends here
 */
forward OnPlayerEnterExteriorArea(playerid, Entrance:entranceid);

/**
 * @brief      A character walked up to the inside of a door.
 *
 * @param      playerid    Character who arrived.
 * @param      entranceid  Entrance they are inside.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` other OnPlayerEnterButtonArea hooks still run
 *             - `1` the chain ends here
 */
forward OnPlayerEnterInteriorArea(playerid, Entrance:entranceid);

/**
 * @brief      A character walked away from the outside of a door.
 *
 * @param      playerid    Character who left.
 * @param      entranceid  Entrance they were outside.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` other OnPlayerLeaveButtonArea hooks still run
 *             - `1` the chain ends here
 */
forward OnPlayerLeaveExteriorArea(playerid, Entrance:entranceid);

/**
 * @brief      A character walked away from the inside of a door.
 *
 * @param      playerid    Character who left.
 * @param      entranceid  Entrance they were inside.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` other OnPlayerLeaveButtonArea hooks still run
 *             - `1` the chain ends here
 */
forward OnPlayerLeaveInteriorArea(playerid, Entrance:entranceid);

/**
 * @brief      A character is trying to go in.
 *
 * Asked before they are moved, so this is where a locked door refuses. It
 * is the question; OnPlayerEnteredEntrance is the answer.
 *
 * @param      playerid    Character at the door.
 * @param      entranceid  Entrance they pressed.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` they go in
 *             - `1` they stay where they are
 */
forward OnPlayerEnterEntrance(playerid, Entrance:entranceid);

/**
 * @brief      A character is trying to come out.
 *
 * The mirror of OnPlayerEnterEntrance, for a door that can be walked into
 * but not out of.
 *
 * @param      playerid    Character at the door.
 * @param      entranceid  Entrance they pressed.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` they come out
 *             - `1` they stay where they are
 */
forward OnPlayerLeaveEntrance(playerid, Entrance:entranceid);

/**
 * @brief      A character has been put inside.
 *
 * Fired once they are already there, by the press and by
 * SetPlayerEntranceInterior alike. The return value is not used.
 *
 * @param      playerid    Character who went in.
 * @param      entranceid  Entrance they went through.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerEnteredEntrance(playerid, Entrance:entranceid);

/**
 * @brief      A character has been put outside.
 *
 * Fired once they are already there, by the press and by
 * SetPlayerEntranceExterior alike. The return value is not used.
 *
 * @param      playerid    Character who came out.
 * @param      entranceid  Entrance they came through.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerLeftEntrance(playerid, Entrance:entranceid);

/**
 * # Internal
 */

static stock GetFreeEntranceIndexInternal() {
    if (gEntranceFreeHead != -1) {
        new const
            index = gEntranceFreeHead
        ;

        gEntranceFreeHead = gEntranceNextFree[index];

        return index;
    }

    if (gEntranceHighWater != _:MAX_ENTRANCES) {
        return gEntranceHighWater++;
    }

    return -1;
}

static stock Entrance:AllocEntranceInternal() {
    new const
        index = GetFreeEntranceIndexInternal()
    ;

    if (index == -1) {
        return INVALID_ENTRANCE_ID;
    }

    ++gEntranceCount;

    return Entrance:index;
}

static stock FreeEntranceInternal(Entrance:entranceid) {
    gEntranceNextFree[_:entranceid] = gEntranceFreeHead;
    gEntranceFreeHead = _:entranceid;

    --gEntranceCount;
}

static stock bool:IsValidEntranceInternal(Entrance:entranceid) {
    if (!(0 <= _:entranceid < _:MAX_ENTRANCES)) {
        return false;
    }

    return gEntranceData[entranceid][E_ENTRANCE_CREATED];
}

static stock MarkEntranceButtonInternal(Button:buttonid, Entrance:entranceid, bool:inside) {
    gEntranceButtonData[buttonid][E_ENTRANCE_BUTTON_OWNED] = true;
    gEntranceButtonData[buttonid][E_ENTRANCE_BUTTON_ENTRANCE_ID] = entranceid;
    gEntranceButtonData[buttonid][E_ENTRANCE_BUTTON_INSIDE] = inside;
}

static stock ClearEntranceButtonInternal(Button:buttonid) {
    gEntranceButtonData[buttonid][E_ENTRANCE_BUTTON_OWNED] = false;
    gEntranceButtonData[buttonid][E_ENTRANCE_BUTTON_ENTRANCE_ID] = INVALID_ENTRANCE_ID;
    gEntranceButtonData[buttonid][E_ENTRANCE_BUTTON_INSIDE] = false;
}

// The entrance a button belongs to, on the side asked for. Both sides go
// through here so that neither can start disagreeing with the other about what
// counts as a door.
static stock Entrance:GetButtonEntranceInternal(Button:buttonid, bool:inside) {
    if (!IsValidButton(buttonid)) {
        return INVALID_ENTRANCE_ID;
    }

    if (!gEntranceButtonData[buttonid][E_ENTRANCE_BUTTON_OWNED]) {
        return INVALID_ENTRANCE_ID;
    }

    if (gEntranceButtonData[buttonid][E_ENTRANCE_BUTTON_INSIDE] != inside) {
        return INVALID_ENTRANCE_ID;
    }

    new const
        Entrance:entranceid = gEntranceButtonData[buttonid][E_ENTRANCE_BUTTON_ENTRANCE_ID]
    ;

    return IsValidEntranceInternal(entranceid) ? entranceid : INVALID_ENTRANCE_ID;
}

static stock SetPlayerEntranceSideInternal(playerid, Entrance:entranceid, bool:inside, Float:offset) {
    new
        Float:x, Float:y, Float:z, Float:angle,
        world, interior
    ;

    if (inside) {
        x        = gEntranceData[entranceid][E_ENTRANCE_INT_X];
        y        = gEntranceData[entranceid][E_ENTRANCE_INT_Y];
        z        = gEntranceData[entranceid][E_ENTRANCE_INT_Z];
        angle    = gEntranceData[entranceid][E_ENTRANCE_INT_A];
        world    = gEntranceData[entranceid][E_ENTRANCE_INT_WORLD_ID];
        interior = gEntranceData[entranceid][E_ENTRANCE_INT_INTERIOR_ID];
    } else {
        x        = gEntranceData[entranceid][E_ENTRANCE_EXT_X];
        y        = gEntranceData[entranceid][E_ENTRANCE_EXT_Y];
        z        = gEntranceData[entranceid][E_ENTRANCE_EXT_Z];
        angle    = gEntranceData[entranceid][E_ENTRANCE_EXT_A];
        world    = gEntranceData[entranceid][E_ENTRANCE_EXT_WORLD_ID];
        interior = gEntranceData[entranceid][E_ENTRANCE_EXT_INTERIOR_ID];
    }

    SetPlayerVirtualWorld(playerid, world);
    SetPlayerInterior(playerid, interior);

    SetPlayerPos(playerid,
        x + offset * floatsin(-angle, degrees),
        y + offset * floatcos(-angle, degrees),
        z
    );

    SetPlayerFacingAngle(playerid, angle);
    SetCameraBehindPlayer(playerid);
}

/**
 * # External
 */

stock Entrance:CreateEntrance(Float:extX, Float:extY, Float:extZ, Float:extA, extWorld, extInterior, Float:intX, Float:intY, Float:intZ, Float:intA, intWorld, intInterior, KEY:buttonKey = ENTRANCE_DEFAULT_BUTTON_KEY, priority = ENTRANCE_DEFAULT_BUTTON_PRIORITY, bool:call = true) {
    new const
        Entrance:entranceid = AllocEntranceInternal()
    ;

    if (entranceid == INVALID_ENTRANCE_ID) {
        return INVALID_ENTRANCE_ID;
    }

    new const
        Button:extButton = CreateButton(extX, extY, extZ, buttonKey, ENTRANCE_DEFAULT_BUTTON_SIZE, extWorld, extInterior, priority),
        Button:intButton = CreateButton(intX, intY, intZ, buttonKey, ENTRANCE_DEFAULT_BUTTON_SIZE, intWorld, intInterior, priority)
    ;

    if (extButton == INVALID_BUTTON_ID) {
        DestroyButton(intButton);

        FreeEntranceInternal(entranceid);

        return INVALID_ENTRANCE_ID;
    }

    if (intButton == INVALID_BUTTON_ID) {
        DestroyButton(extButton);

        FreeEntranceInternal(entranceid);

        return INVALID_ENTRANCE_ID;
    }

    gEntranceData[entranceid][E_ENTRANCE_CREATED]         = true;

    gEntranceData[entranceid][E_ENTRANCE_EXT_X]           = extX;
    gEntranceData[entranceid][E_ENTRANCE_EXT_Y]           = extY;
    gEntranceData[entranceid][E_ENTRANCE_EXT_Z]           = extZ;
    gEntranceData[entranceid][E_ENTRANCE_EXT_A]           = extA;
    gEntranceData[entranceid][E_ENTRANCE_EXT_WORLD_ID]    = extWorld;
    gEntranceData[entranceid][E_ENTRANCE_EXT_INTERIOR_ID] = extInterior;

    gEntranceData[entranceid][E_ENTRANCE_INT_X]           = intX;
    gEntranceData[entranceid][E_ENTRANCE_INT_Y]           = intY;
    gEntranceData[entranceid][E_ENTRANCE_INT_Z]           = intZ;
    gEntranceData[entranceid][E_ENTRANCE_INT_A]           = intA;
    gEntranceData[entranceid][E_ENTRANCE_INT_WORLD_ID]    = intWorld;
    gEntranceData[entranceid][E_ENTRANCE_INT_INTERIOR_ID] = intInterior;

    gEntranceData[entranceid][E_ENTRANCE_EXT_BUTTON_ID]   = extButton;
    gEntranceData[entranceid][E_ENTRANCE_INT_BUTTON_ID]   = intButton;

    MarkEntranceButtonInternal(extButton, entranceid, false);
    MarkEntranceButtonInternal(intButton, entranceid, true);

    SetButtonPopupText(extButton, ENTRANCE_EXTERIOR_POPUP, ENTRANCE_KEY_TEXT);
    SetButtonPopupText(intButton, ENTRANCE_INTERIOR_POPUP, ENTRANCE_KEY_TEXT);

    if (call) {
        CallLocalFunction("OnEntranceCreate", "i", _:entranceid);
    }

    return entranceid;
}

stock bool:DestroyEntrance(Entrance:entranceid, bool:call = true) {
    if (!IsValidEntranceInternal(entranceid)) {
        return false;
    }

    if (call) {
        CallLocalFunction("OnEntranceDestroy", "i", _:entranceid);
    }

    ClearEntranceButtonInternal(gEntranceData[entranceid][E_ENTRANCE_EXT_BUTTON_ID]);
    ClearEntranceButtonInternal(gEntranceData[entranceid][E_ENTRANCE_INT_BUTTON_ID]);

    DestroyButton(gEntranceData[entranceid][E_ENTRANCE_EXT_BUTTON_ID]);
    DestroyButton(gEntranceData[entranceid][E_ENTRANCE_INT_BUTTON_ID]);

    static const
        ENTRANCE_DATA_NONE[E_ENTRANCE_DATA]
    ;

    gEntranceData[entranceid] = ENTRANCE_DATA_NONE;

    FreeEntranceInternal(entranceid);

    return true;
}

stock bool:IsValidEntrance(Entrance:entranceid) {
    return IsValidEntranceInternal(entranceid);
}

stock GetEntranceCount() {
    return gEntranceCount;
}

/**
 * # Exterior
 */

stock bool:GetEntranceExteriorPos(Entrance:entranceid, &Float:x, &Float:y, &Float:z) {
    if (!IsValidEntranceInternal(entranceid)) {
        return false;
    }

    x = gEntranceData[entranceid][E_ENTRANCE_EXT_X];
    y = gEntranceData[entranceid][E_ENTRANCE_EXT_Y];
    z = gEntranceData[entranceid][E_ENTRANCE_EXT_Z];

    return true;
}

stock bool:GetEntranceExteriorFacingAngle(Entrance:entranceid, &Float:a) {
    if (!IsValidEntranceInternal(entranceid)) {
        return false;
    }

    a = gEntranceData[entranceid][E_ENTRANCE_EXT_A];

    return true;
}

stock GetEntranceExteriorVirtualWorld(Entrance:entranceid) {
    if (!IsValidEntranceInternal(entranceid)) {
        return -1;
    }

    return gEntranceData[entranceid][E_ENTRANCE_EXT_WORLD_ID];
}

stock GetEntranceExteriorInterior(Entrance:entranceid) {
    if (!IsValidEntranceInternal(entranceid)) {
        return -1;
    }

    return gEntranceData[entranceid][E_ENTRANCE_EXT_INTERIOR_ID];
}

stock Button:GetEntranceExteriorButton(Entrance:entranceid) {
    if (!IsValidEntranceInternal(entranceid)) {
        return INVALID_BUTTON_ID;
    }

    return gEntranceData[entranceid][E_ENTRANCE_EXT_BUTTON_ID];
}

stock bool:HasButtonEntranceExterior(Button:buttonid) {
    return GetButtonEntranceInternal(buttonid, false) != INVALID_ENTRANCE_ID;
}

stock Entrance:GetButtonEntranceExterior(Button:buttonid) {
    return GetButtonEntranceInternal(buttonid, false);
}

stock bool:SetPlayerEntranceExterior(playerid, Entrance:entranceid, Float:offset = ENTRANCE_DEFAULT_FORWARD_OFFSET) {
    if (!IsValidEntranceInternal(entranceid)) {
        return false;
    }

    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    SetPlayerEntranceSideInternal(
        playerid,
        entranceid,
        false,
        offset
    );

    CallLocalFunction("OnPlayerLeftEntrance", "ii", playerid, _:entranceid);

    return true;
}

stock Entrance:GetPlayerEntranceExterior(playerid) {
    return GetButtonEntranceInternal(GetPlayerButton(playerid), false);
}

stock Button:GetPlayerEntranceExteriorButton(playerid) {
    new const
        Button:buttonid = GetPlayerButton(playerid)
    ;

    return HasButtonEntranceExterior(buttonid) ? buttonid : INVALID_BUTTON_ID;
}

/**
 * # Interior
 */

stock bool:GetEntranceInteriorPos(Entrance:entranceid, &Float:x, &Float:y, &Float:z) {
    if (!IsValidEntranceInternal(entranceid)) {
        return false;
    }

    x = gEntranceData[entranceid][E_ENTRANCE_INT_X];
    y = gEntranceData[entranceid][E_ENTRANCE_INT_Y];
    z = gEntranceData[entranceid][E_ENTRANCE_INT_Z];

    return true;
}

stock bool:GetEntranceInteriorFacingAngle(Entrance:entranceid, &Float:a) {
    if (!IsValidEntranceInternal(entranceid)) {
        return false;
    }

    a = gEntranceData[entranceid][E_ENTRANCE_INT_A];

    return true;
}

stock GetEntranceInteriorVirtualWorld(Entrance:entranceid) {
    if (!IsValidEntranceInternal(entranceid)) {
        return -1;
    }

    return gEntranceData[entranceid][E_ENTRANCE_INT_WORLD_ID];
}

stock GetEntranceInteriorInterior(Entrance:entranceid) {
    if (!IsValidEntranceInternal(entranceid)) {
        return -1;
    }

    return gEntranceData[entranceid][E_ENTRANCE_INT_INTERIOR_ID];
}

stock Button:GetEntranceInteriorButton(Entrance:entranceid) {
    if (!IsValidEntranceInternal(entranceid)) {
        return INVALID_BUTTON_ID;
    }

    return gEntranceData[entranceid][E_ENTRANCE_INT_BUTTON_ID];
}

stock bool:HasButtonEntranceInterior(Button:buttonid) {
    return GetButtonEntranceInternal(buttonid, true) != INVALID_ENTRANCE_ID;
}

stock Entrance:GetButtonEntranceInterior(Button:buttonid) {
    return GetButtonEntranceInternal(buttonid, true);
}

stock bool:SetPlayerEntranceInterior(playerid, Entrance:entranceid, Float:offset = ENTRANCE_DEFAULT_FORWARD_OFFSET) {
    if (!IsValidEntranceInternal(entranceid)) {
        return false;
    }

    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    SetPlayerEntranceSideInternal(
        playerid,
        entranceid,
        true,
        offset
    );

    CallLocalFunction("OnPlayerEnteredEntrance", "ii", playerid, _:entranceid);

    return true;
}

stock Entrance:GetPlayerEntranceInterior(playerid) {
    return GetButtonEntranceInternal(GetPlayerButton(playerid), true);
}

stock Button:GetPlayerEntranceInteriorButton(playerid) {
    new const
        Button:buttonid = GetPlayerButton(playerid)
    ;

    return HasButtonEntranceInterior(buttonid) ? buttonid : INVALID_BUTTON_ID;
}

/**
 * # Calls
 */

hook OnButtonPress(playerid, Button:buttonid) {
    new
        Entrance:entranceid = GetButtonEntranceInternal(buttonid, false)
    ;

    if (entranceid != INVALID_ENTRANCE_ID) {
        if (CallLocalFunction("OnPlayerEnterEntrance", "ii", playerid, _:entranceid)) {
            return 1;
        }

        SetPlayerEntranceInterior(playerid, entranceid);

        return 1;
    }

    entranceid = GetButtonEntranceInternal(buttonid, true);

    if (entranceid != INVALID_ENTRANCE_ID) {
        if (CallLocalFunction("OnPlayerLeaveEntrance", "ii", playerid, _:entranceid)) {
            return 1;
        }

        SetPlayerEntranceExterior(playerid, entranceid);

        return 1;
    }

    return 0;
}

hook OnPlayerEnterButtonArea(playerid, Button:buttonid) {
    new
        Entrance:entranceid = GetButtonEntranceInternal(buttonid, false)
    ;

    if (entranceid != INVALID_ENTRANCE_ID) {
        return CallLocalFunction("OnPlayerEnterExteriorArea", "ii", playerid, _:entranceid) ? 1 : 0;
    }

    entranceid = GetButtonEntranceInternal(buttonid, true);

    if (entranceid != INVALID_ENTRANCE_ID) {
        return CallLocalFunction("OnPlayerEnterInteriorArea", "ii", playerid, _:entranceid) ? 1 : 0;
    }

    return 0;
}

hook OnPlayerLeaveButtonArea(playerid, Button:buttonid) {
    new
        Entrance:entranceid = GetButtonEntranceInternal(buttonid, false)
    ;

    if (entranceid != INVALID_ENTRANCE_ID) {
        return CallLocalFunction("OnPlayerLeaveExteriorArea", "ii", playerid, _:entranceid) ? 1 : 0;
    }

    entranceid = GetButtonEntranceInternal(buttonid, true);

    if (entranceid != INVALID_ENTRANCE_ID) {
        return CallLocalFunction("OnPlayerLeaveInteriorArea", "ii", playerid, _:entranceid) ? 1 : 0;
    }

    return 0;
}

hook OnButtonDestroy(Button:buttonid) {
    ClearEntranceButtonInternal(buttonid);

    return 0;
}

hook OnButtonCreate(Button:buttonid) {
    ClearEntranceButtonInternal(buttonid);

    return 0;
}
