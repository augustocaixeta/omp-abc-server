#if defined _INC_BUTTON
    #endinput
#endif
#define _INC_BUTTON

#include <open.mp>

// y_va is here for the same reason it is in extended-text-styles: Pawn cannot
// hand the arguments of one function to another one by itself. It brings
// nothing else in with it and it does not fight pp-hooks.
#include <YSI_Coding\y_va>
#include <mathutil>
#include <pp-hooks>
#include <streamer>

/**
 * # Header
 */

// A place in the world where a key means something. A door handle, a locker, an
// item lying on the ground: each is a sphere the streamer watches, and a press
// inside one arrives here as a button rather than as a key and a position.
//
// What the press then means is never this library's business. It answers which
// button, and the gamemode answers what for.

#if !defined MAX_BUTTONS
    #define MAX_BUTTONS (Button:8192)
#endif

#if !defined MAX_BUTTON_POPUP_LENGTH
    #define MAX_BUTTON_POPUP_LENGTH (128)
#endif

#if !defined MAX_OVERLAPPING_AREAS
    #define MAX_OVERLAPPING_AREAS (256)
#endif

#if !defined MAX_AREA_BUTTONS
    #define MAX_AREA_BUTTONS (8)
#endif

#if !defined BUTTON_DEFAULT_KEY
    #define BUTTON_DEFAULT_KEY (KEY_SECONDARY_ATTACK)
#endif

#if !defined BUTTON_DEFAULT_DRAW_DISTANCE
    #define BUTTON_DEFAULT_DRAW_DISTANCE (4.0)
#endif

#if !defined BUTTON_DEFAULT_STREAM_DISTANCE
    #define BUTTON_DEFAULT_STREAM_DISTANCE (4.0)
#endif

#if !defined BUTTON_STREAMER_IDENTIFIER
    #define BUTTON_STREAMER_IDENTIFIER (0x62746E)
#endif

#define INVALID_BUTTON_ID (Button:-1)

// The popup text is packed: a hundred and twenty eight characters live in
// thirty two cells instead of a hundred and twenty eight, and every button
// carries the room for it whether it is given any text or not.
static enum _:E_BUTTON_DATA {
    bool:E_BUTTON_CREATED,

    KEY:E_BUTTON_KEYS,
    E_BUTTON_PRIORITY,
    E_BUTTON_TEXT[MAX_BUTTON_POPUP_LENGTH char],

    Float:E_BUTTON_X,
    Float:E_BUTTON_Y,
    Float:E_BUTTON_Z,
    Float:E_BUTTON_SIZE,

    E_BUTTON_WORLD_ID,
    E_BUTTON_INTERIOR_ID,

    STREAMER_TAG_AREA:E_BUTTON_AREA_ID,
    STREAMER_TAG_3D_TEXT_LABEL:E_BUTTON_TEXT3D_ID
};

// How far off straight ahead a button may sit and still count as being in front
// of the player: forty five each way, so a ninety degree cone. Wide enough that
// nobody has to aim, narrow enough that two things a step apart are told apart
// by turning.
#if !defined BUTTON_FACING_ANGLE
    #define BUTTON_FACING_ANGLE (45.0)
#endif

// Only what a scan has to read off the area itself is kept here: the priority
// used to live on the area as well and had to be written in two places every
// time it changed, which is one place too many.
static enum _:E_BUTTON_AREA_DATA {
    E_BUTTON_AREA_TYPE,
    Button:E_BUTTON_AREA_REF_ID
};

// Everything a button is made of lives in a plain Pawn array, because reading a
// cell out of one is a single instruction while a plugin container answers the
// same question through a call out and back, and these are read on every key
// press and every area scan.
//
// The array is also what hands the ids out, through a free list: every slot
// that is given back points at the one given back before it, and slots never
// used yet come off a high water mark. Taking one is a read and a write, giving
// one back is the same, and a full array is answered without looking at
// anything. Nothing is set up at boot either.
//
// A rotating cursor was tried first and walks the array to find a hole, which
// at four thousand slots costs 293 us for an allocation on a full array against
// 0.349 us here, and a refusal costs the whole array. y_iterate is worse again:
// it searches from zero every time and then walks its chain to keep the list
// sorted, 237 us per allocation, and never finished filling 65536 slots.
static
    gButtonNextFree[_:MAX_BUTTONS],
    gButtonFreeHead = -1,
    gButtonHighWater,
    gButtonCount,
    gButtonData[MAX_BUTTONS][E_BUTTON_DATA],
    Button:gPlayerButtonCurrent[MAX_PLAYERS] = { INVALID_BUTTON_ID, ... },
    Button:gPlayerButtonHeld[MAX_PLAYERS] = { INVALID_BUTTON_ID, ... }
;

// Anything that can change who outranks whom steps this counter, and a player
// whose answer was worked out against an older one works theirs out again the
// next time it is asked for. Nothing has to go looking for the players it
// affected, and a player nobody asks about never pays for the change at all.
static
    gButtonGeneration = 1,
    gPlayerButtonGeneration[MAX_PLAYERS]
;

/**
 * # Functions
 */

/**
 * @brief      Put a button in the world.
 *
 * A sphere is made for it unless one is handed over, which is how a button
 * gets a shape that is not a sphere: make the area, then give it here. A
 * button made that way cannot be moved afterwards, because reinterpreting a
 * cube or a polygon is not this library's job.
 *
 * @param      x           World X.
 * @param      y           World Y.
 * @param      z           World Z.
 * @param      buttonKeys  Which keys work on it, ORed together.
 * @param      size        Radius, when a sphere is being made.
 * @param      worldid     Virtual world, `-1` for all of them.
 * @param      interiorid  Interior, `-1` for all of them.
 * @param      priority    Which button wins when two overlap. Higher wins.
 * @param      areaid      An area to use instead of making a sphere.
 * @param      call        Whether to fire OnButtonCreate.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the new button
 *             - `INVALID_BUTTON_ID` MAX_BUTTONS already exist
 */
forward Button:CreateButton(Float:x, Float:y, Float:z, KEY:buttonKeys = BUTTON_DEFAULT_KEY, Float:size = 1.0, worldid = -1, interiorid = -1, priority = 0, STREAMER_TAG_AREA:areaid = STREAMER_TAG_AREA:INVALID_STREAMER_ID, bool:call = true);

/**
 * @brief      Take a button back out of the world.
 *
 * Its area and its label go with it. Anybody standing in it is not told
 * they left, because leaving is never reported for an area that stopped
 * existing -- instead everyone's answer is marked stale, and whoever asks
 * next works theirs out again.
 *
 * @param      buttonid  Button to remove.
 * @param      call      Whether to fire OnButtonDestroy.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is gone
 *             - `false` `buttonid` is not a live button
 */
forward bool:DestroyButton(Button:buttonid, bool:call = true);

/**
 * @brief      Whether a button exists.
 *
 * @param      buttonid  Button to check.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is in the world
 *             - `false` when:
 *                 + `buttonid` is out of range
 *                 + it was never made, or has been destroyed
 */
forward bool:IsValidButton(Button:buttonid);

/**
 * @brief      How many buttons are in the world.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many exist right now
 */
forward GetButtonCount();

/**
 * @brief      Move a button.
 *
 * Its area and its label move with it, the label keeping whatever height
 * above the button it was given.
 *
 * @param      buttonid  Button to move.
 * @param      x         New world X.
 * @param      y         New world Y.
 * @param      z         New world Z.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it has moved
 *             - `false` when:
 *                 + `buttonid` is not a live button
 *                 + its area is not a sphere, so moving it would mean
 *                   reinterpreting a shape this library did not make
 */
forward bool:SetButtonPos(Button:buttonid, Float:x, Float:y, Float:z);

/**
 * @brief      Where a button is.
 *
 * @param      buttonid  Button to read.
 * @param      x         Reads back world X.
 * @param      y         Reads back world Y.
 * @param      z         Reads back world Z.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the outputs were written
 *             - `false` `buttonid` is not a live button
 */
forward bool:GetButtonPos(Button:buttonid, &Float:x, &Float:y, &Float:z);

/**
 * @brief      Change which keys work on a button.
 *
 * Several ORed together all work, and any of them presses it.
 *
 * @param      buttonid    Button to write.
 * @param      buttonKeys  The keys.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the keys were written
 *             - `false` `buttonid` is not a live button
 */
forward bool:SetButtonKeys(Button:buttonid, KEY:buttonKeys);

/**
 * @brief      Which keys work on a button.
 *
 * @param      buttonid  Button to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the keys it answers to
 *             - `KEY:0` `buttonid` is not a live button
 */
forward KEY:GetButtonKeys(Button:buttonid);

/**
 * @brief      Change which button wins when two overlap.
 *
 * Higher wins, and it is the second question asked after facing: a locker
 * set above the items lying against it still wins, however the character
 * turns. Everyone's current answer is marked stale.
 *
 * @param      buttonid  Button to write.
 * @param      priority  The new precedence.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the precedence was written
 *             - `false` `buttonid` is not a live button
 */
forward bool:SetButtonPriority(Button:buttonid, priority);

/**
 * @brief      A button's precedence.
 *
 * @param      buttonid  Button to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - its precedence
 *             - `0` when:
 *                 + `buttonid` is not a live button
 *                 + it was never given one
 */
forward GetButtonPriority(Button:buttonid);

/**
 * @brief      Resize a button's sphere.
 *
 * @param      buttonid  Button to resize.
 * @param      size      New radius.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is the new size
 *             - `false` when:
 *                 + `buttonid` is not a live button
 *                 + its area is not a sphere, so it has no radius to change
 */
forward bool:SetButtonSize(Button:buttonid, Float:size);

/**
 * @brief      A button's radius.
 *
 * @param      buttonid  Button to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the radius it was made with
 *             - `0.0` `buttonid` is not a live button
 */
forward Float:GetButtonSize(Button:buttonid);

/**
 * @brief      Move a button to another virtual world.
 *
 * Its area and label move with it, so it stops existing for everybody in
 * the world it left.
 *
 * @param      buttonid  Button to move.
 * @param      worldid   Virtual world, `-1` for all of them.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it has moved
 *             - `false` `buttonid` is not a live button
 */
forward bool:SetButtonVirtualWorld(Button:buttonid, worldid);

/**
 * @brief      Which virtual world a button is in.
 *
 * @param      buttonid  Button to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the world it is in
 *             - `-1` when:
 *                 + `buttonid` is not a live button
 *                 + it is in every world
 */
forward GetButtonVirtualWorld(Button:buttonid);

/**
 * @brief      Move a button to another interior.
 *
 * @param      buttonid    Button to move.
 * @param      interiorid  Interior, `-1` for all of them.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it has moved
 *             - `false` `buttonid` is not a live button
 */
forward bool:SetButtonInterior(Button:buttonid, interiorid);

/**
 * @brief      Which interior a button is in.
 *
 * @param      buttonid  Button to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the interior it is in
 *             - `-1` when:
 *                 + `buttonid` is not a live button
 *                 + it is in every interior
 */
forward GetButtonInterior(Button:buttonid);

/**
 * @brief      What a character standing at a button is told they can do.
 *
 * Only on screen while they are the one addressing it, so two buttons a
 * step apart show one prompt at a time -- whichever is being faced.
 *
 * Kept packed, so a hundred and twenty eight characters live in thirty two
 * cells. Every button carries the room whether it is given any text or not.
 *
 * @param      buttonid  Button to write.
 * @param      message   Text, with placeholders if anything follows it.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the popup was written
 *             - `false` `buttonid` is not a live button
 */
forward bool:SetButtonPopupText(Button:buttonid, const message[], OPEN_MP_TAGS:...);

/**
 * @brief      What a button's popup currently says.
 *
 * @param      buttonid  Button to read.
 * @param      output    Reads back the popup text.
 * @param      size      How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` `buttonid` is not a live button
 */
forward bool:GetButtonPopupText(Button:buttonid, output[], size = sizeof (output));

/**
 * @brief      Hang a 3D label over a button.
 *
 * Unlike the popup this is drawn in the world for anybody in range, whether
 * they are addressing the button or not. Calling it again replaces the
 * label rather than adding a second one, so use UpdateButtonLabelText for
 * text that changes often.
 *
 * @param      buttonid        Button to label.
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
 *             - `false` `buttonid` is not a live button
 */
forward bool:SetButtonLabelText(Button:buttonid, const message[], colour = 0xFFFFFFFF, Float:drawDistance = BUTTON_DEFAULT_DRAW_DISTANCE, Float:streamDistance = BUTTON_DEFAULT_STREAM_DISTANCE, bool:testLOS = true, Float:offsetZ = 0.0, OPEN_MP_TAGS:...);

/**
 * @brief      Rewrite a label that is already up.
 *
 * Changes the words in place rather than taking the label down and putting
 * a new one up, so it keeps its height, its distances and its identity.
 *
 * @param      buttonid  Button whose label to rewrite.
 * @param      message   Text, with placeholders if anything follows
 *                       `colour`.
 * @param      colour    Label colour.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the label says something else now
 *             - `false` when:
 *                 + `buttonid` is not a live button
 *                 + it has no label up
 */
forward bool:UpdateButtonLabelText(Button:buttonid, const message[], colour = 0xFFFFFFFF, OPEN_MP_TAGS:...);

/**
 * @brief      Take a button's label down.
 *
 * The button itself stays, and its popup with it.
 *
 * @param      buttonid  Button to unlabel.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the label is gone
 *             - `false` when:
 *                 + `buttonid` is not a live button
 *                 + it had no label up
 */
forward bool:DestroyButtonLabel(Button:buttonid);

/**
 * @brief      A button's 3D label.
 *
 * @param      buttonid  Button to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - its label
 *             - `INVALID_STREAMER_ID` when:
 *                 + `buttonid` is not a live button
 *                 + it has no label up
 */
forward STREAMER_TAG_3D_TEXT_LABEL:GetButtonLabel(Button:buttonid);

/**
 * @brief      The area a button is watched through.
 *
 * @param      buttonid  Button to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - its area
 *             - `INVALID_STREAMER_ID` `buttonid` is not a live button
 */
forward STREAMER_TAG_AREA:GetButtonArea(Button:buttonid);

/**
 * @brief      Give a button a different shape.
 *
 * How a button becomes a cube, a cylinder or a polygon: make the area, then
 * hand it over. The sphere it had is destroyed, and everyone's current
 * answer is marked stale because the shape they were standing in is gone.
 *
 * @param      buttonid  Button to reshape.
 * @param      areaid    Area to use from now on.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the button is that shape now
 *             - `false` when:
 *                 + `buttonid` is not a live button
 *                 + `areaid` is not a live area
 *                 + it is already the button's area
 */
forward bool:SetButtonArea(Button:buttonid, STREAMER_TAG_AREA:areaid);

/**
 * @brief      Whether a button's shape is a plain sphere.
 *
 * What SetButtonPos and SetButtonSize ask first, since neither can change a
 * shape this library did not make.
 *
 * @param      buttonid  Button to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is a sphere
 *             - `false` when:
 *                 + `buttonid` is not a live button
 *                 + it was given some other shape
 */
forward bool:IsButtonAreaSphere(Button:buttonid);

/**
 * @brief      Whether a character is standing at any button.
 *
 * @param      playerid  Player to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` there is a button they are addressing
 *             - `false` when:
 *                 + they are not connected
 *                 + they are standing in no button's area
 */
forward bool:IsPlayerInAnyButton(playerid);

/**
 * @brief      The button a character is addressing.
 *
 * Out of everything they are standing in, three questions decide it, each
 * one only settling what the one before it tied on:
 *
 *   - is it in front of them? Standing between two crates, the one being
 *     looked at is the one being talked about
 *   - is its priority higher? A locker set above the items lying against it
 *     still wins
 *   - is it nearer? Only reached when two are equally faced and equally
 *     important
 *
 * Nothing is ever ruled out for being behind them. Facing sorts, it does
 * not exclude: a character standing in exactly one button's area reaches it
 * whichever way they are turned, which is what stops a door becoming
 * unopenable because it is behind the shoulder.
 *
 * The answer is remembered and only worked out again when something that
 * could change it has happened, so asking is normally one array read.
 *
 * @param      playerid  Player to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the button they are addressing
 *             - `INVALID_BUTTON_ID` when:
 *                 + they are not connected
 *                 + they are standing in no button's area
 */
forward Button:GetPlayerButton(playerid);

/**
 * @brief      The button a character is holding the key on.
 *
 * A release belongs to whatever was pressed, not to whatever they happen to
 * be standing at by the time they let go -- so holding the key while
 * walking into the next button does not end with that one being released
 * without ever having been pressed.
 *
 * @param      playerid  Player to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the button they pressed and have not let go of
 *             - `INVALID_BUTTON_ID` when:
 *                 + they are not connected
 *                 + they are holding nothing
 *                 + what they were holding has been destroyed
 */
forward Button:GetPlayerHeldButton(playerid);

/**
 * @brief      Which way a character would have to turn to face a button.
 *
 * The angle from them to it, in the game's own reckoning, so it can be
 * handed straight to SetPlayerFacingAngle.
 *
 * @param      playerid  Player to measure from.
 * @param      buttonid  Button to measure to.
 * @param      angle     Reads back the angle.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` when:
 *                 + `buttonid` is not a live button
 *                 + `playerid` is not connected
 */
forward bool:GetPlayerAngleToButton(playerid, Button:buttonid, &Float:angle);

/**
 * @brief      Which way a button would face to look at a character.
 *
 * The same measurement the other way round, for turning an object at a
 * button towards whoever is using it.
 *
 * @param      playerid  Player to measure to.
 * @param      buttonid  Button to measure from.
 * @param      angle     Reads back the angle.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` when:
 *                 + `buttonid` is not a live button
 *                 + `playerid` is not connected
 */
forward bool:GetButtonAngleToPlayer(playerid, Button:buttonid, &Float:angle);

/**
 * @brief      Every button a character is standing in, most important first.
 *
 * For a menu that offers all of them rather than only the one being faced.
 * Sorted by precedence alone, so it is not the same order
 * GetPlayerButton decides with -- that one asks about facing first.
 *
 * Only the first `count` entries are written; anything past that is left as
 * the caller had it.
 *
 * @param      playerid  Player to ask about.
 * @param      buttons   Reads back the buttons.
 * @param      size      How many the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many were written
 *             - `0` when:
 *                 + they are not connected
 *                 + they are standing in no button's area
 */
forward GetPlayerNearbyButtons(playerid, Button:buttons[MAX_AREA_BUTTONS] = { INVALID_BUTTON_ID, ... }, size = sizeof (buttons));

/**
 * # Events
 */

/**
 * @brief      A button has been put in the world.
 *
 * @param      buttonid  The new button.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnButtonCreate(Button:buttonid);

/**
 * @brief      A button is going away.
 *
 * Fired once it has already been taken apart, so it says that the button
 * has ended rather than what it was.
 *
 * @param      buttonid  The button that is gone.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnButtonDestroy(Button:buttonid);

/**
 * @brief      A character pressed a button.
 *
 * Only for the button they are addressing, and only on the press itself --
 * holding the key down does not fire it again.
 *
 * @param      playerid  Character who pressed it.
 * @param      buttonid  Button they pressed.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` other OnButtonPress hooks still run
 *             - `1` the chain ends here
 */
forward OnButtonPress(playerid, Button:buttonid);

/**
 * @brief      A character let go of a button.
 *
 * Fired for whatever they pressed, even if they have since walked into
 * another one -- and fired on walking out of the area as well, since
 * leaving is letting go.
 *
 * @param      playerid  Character who let go.
 * @param      buttonid  Button they were holding.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` other OnButtonRelease hooks still run
 *             - `1` the chain ends here
 */
forward OnButtonRelease(playerid, Button:buttonid);

/**
 * @brief      A character walked into a button's area.
 *
 * Fired for every button whose area they entered, not only the one they end
 * up addressing -- walking into two overlapping buttons fires it twice.
 *
 * @param      playerid  Character who arrived.
 * @param      buttonid  Button whose area they are in.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerEnterButtonArea(playerid, Button:buttonid);

/**
 * @brief      A character walked out of a button's area.
 *
 * If they were holding that button's key, OnButtonRelease fires first.
 *
 * @param      playerid  Character who left.
 * @param      buttonid  Button whose area they left.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerLeaveButtonArea(playerid, Button:buttonid);

/**
 * # Internal
 */

static stock GetFreeButtonIndexInternal() {
    if (gButtonFreeHead != -1) {
        new const
            index = gButtonFreeHead
        ;

        gButtonFreeHead = gButtonNextFree[index];

        return index;
    }

    if (gButtonHighWater != _:MAX_BUTTONS) {
        return gButtonHighWater++;
    }

    return -1;
}

static stock Button:AllocButtonInternal() {
    new const
        index = GetFreeButtonIndexInternal()
    ;

    if (index == -1) {
        return INVALID_BUTTON_ID;
    }

    ++gButtonCount;

    return Button:index;
}

static stock FreeButtonInternal(Button:buttonid) {
    gButtonNextFree[_:buttonid] = gButtonFreeHead;
    gButtonFreeHead = _:buttonid;

    --gButtonCount;
}

static stock bool:IsValidButtonInternal(Button:buttonid) {
    if (!(0 <= _:buttonid < _:MAX_BUTTONS)) {
        return false;
    }

    return gButtonData[buttonid][E_BUTTON_CREATED];
}

static stock MarkButtonAreaInternal(Button:buttonid, STREAMER_TAG_AREA:areaid) {
    new
        areaData[E_BUTTON_AREA_DATA]
    ;

    areaData[E_BUTTON_AREA_TYPE] = BUTTON_STREAMER_IDENTIFIER;
    areaData[E_BUTTON_AREA_REF_ID] = buttonid;

    Streamer_SetArrayData(
        STREAMER_TYPE_AREA,
        areaid,
        E_STREAMER_EXTRA_ID,
        areaData
    );
}

static stock bool:GetButtonAreaDataInternal(STREAMER_TAG_AREA:areaid, &Button:buttonid) {
    new
        areaData[E_BUTTON_AREA_DATA]
    ;

    Streamer_GetArrayData(
        STREAMER_TYPE_AREA,
        areaid,
        E_STREAMER_EXTRA_ID,
        areaData
    );

    if (areaData[E_BUTTON_AREA_TYPE] != BUTTON_STREAMER_IDENTIFIER) {
        return false;
    }

    buttonid = areaData[E_BUTTON_AREA_REF_ID];

    return IsValidButtonInternal(buttonid);
}

static stock bool:IsButtonAreaSphereInternal(Button:buttonid) {
    return Streamer_GetIntData(
        STREAMER_TYPE_AREA,
        gButtonData[buttonid][E_BUTTON_AREA_ID],
        E_STREAMER_TYPE
    ) == STREAMER_AREA_TYPE_SPHERE;
}

// Whether a button is in front of the player rather than merely near them.
//
// AngleInRangeOfAngle is not used, on purpose: it subtracts the two angles and
// compares, which is right everywhere except across north. Facing 350 degrees
// at a button lying at 10 gives a difference of 340 rather than the 20 degrees
// they actually are apart, and every button standing near north would be
// unreachable from one side of it.
static stock bool:IsButtonInFrontInternal(Button:buttonid, Float:x, Float:y, Float:facing) {
    new
        Float:difference = GetAbsoluteAngle(GetAngleToPoint(x, y, gButtonData[buttonid][E_BUTTON_X], gButtonData[buttonid][E_BUTTON_Y]) - facing)
    ;

    if (difference > 180.0) {
        difference -= 360.0;
    }

    return (-BUTTON_FACING_ANGLE < difference < BUTTON_FACING_ANGLE);
}

// Which button the player is addressing, worked out here and remembered: see
// GetPlayerButton for the three questions and why facing sorts without
// excluding.
static stock RefreshPlayerButtonInternal(playerid) {
    new
        STREAMER_TAG_AREA:areas[MAX_OVERLAPPING_AREAS],
        Button:buttonid,
        Button:lastButtonId = INVALID_BUTTON_ID,
        lastPriority = -1,
        Float:lastDistance = 0.0,
        bool:lastFacing = false,
        Float:x,
        Float:y,
        Float:z,
        Float:facing
    ;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, facing);

    for (new i, size = GetPlayerDynamicAreas(playerid, areas); i != size; ++i) {
        if (!GetButtonAreaDataInternal(areas[i], buttonid)) {
            continue;
        }

        new const
            bool:isFacing = IsButtonInFrontInternal(buttonid, x, y, facing),
            priority = gButtonData[buttonid][E_BUTTON_PRIORITY],
            Float:distance = GetDistance3D(x, y, z, gButtonData[buttonid][E_BUTTON_X], gButtonData[buttonid][E_BUTTON_Y], gButtonData[buttonid][E_BUTTON_Z])
        ;

        if (lastButtonId != INVALID_BUTTON_ID) {
            if (lastFacing != isFacing) {
                if (!isFacing) {
                    continue;
                }
            } else if (priority != lastPriority) {
                if (priority < lastPriority) {
                    continue;
                }
            } else if (distance >= lastDistance) {
                continue;
            }
        }

        lastButtonId = buttonid;
        lastPriority = priority;
        lastDistance = distance;
        lastFacing = isFacing;
    }

    gPlayerButtonCurrent[playerid] = lastButtonId;
    gPlayerButtonGeneration[playerid] = gButtonGeneration;
}

/**
 * # External
 */

stock Button:CreateButton(Float:x, Float:y, Float:z, KEY:buttonKeys = BUTTON_DEFAULT_KEY, Float:size = 1.0, worldid = -1, interiorid = -1, priority = 0, STREAMER_TAG_AREA:areaid = STREAMER_TAG_AREA:INVALID_STREAMER_ID, bool:call = true) {
    new const
        Button:buttonid = AllocButtonInternal()
    ;

    if (buttonid == INVALID_BUTTON_ID) {
        return INVALID_BUTTON_ID;
    }

    gButtonData[buttonid][E_BUTTON_CREATED]     = true;

    gButtonData[buttonid][E_BUTTON_KEYS]        = buttonKeys;
    gButtonData[buttonid][E_BUTTON_PRIORITY]    = priority;

    gButtonData[buttonid][E_BUTTON_X]           = x;
    gButtonData[buttonid][E_BUTTON_Y]           = y;
    gButtonData[buttonid][E_BUTTON_Z]           = z;
    gButtonData[buttonid][E_BUTTON_SIZE]        = size;

    gButtonData[buttonid][E_BUTTON_WORLD_ID]    = worldid;
    gButtonData[buttonid][E_BUTTON_INTERIOR_ID] = interiorid;

    gButtonData[buttonid][E_BUTTON_AREA_ID]     = IsValidDynamicArea(areaid)
        ? areaid
        : CreateDynamicSphere(x, y, z, size, worldid, interiorid);

    gButtonData[buttonid][E_BUTTON_TEXT3D_ID]   = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;

    MarkButtonAreaInternal(buttonid, gButtonData[buttonid][E_BUTTON_AREA_ID]);

    if (call) {
        CallLocalFunction("OnButtonCreate", "i", _:buttonid);
    }

    return buttonid;
}

stock bool:DestroyButton(Button:buttonid, bool:call = true) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    DestroyDynamicArea(gButtonData[buttonid][E_BUTTON_AREA_ID]);

    if (IsValidDynamic3DTextLabel(gButtonData[buttonid][E_BUTTON_TEXT3D_ID])) {
        DestroyDynamic3DTextLabel(gButtonData[buttonid][E_BUTTON_TEXT3D_ID]);
    }

    static const
        BUTTON_DATA_NONE[E_BUTTON_DATA]
    ;

    gButtonData[buttonid] = BUTTON_DATA_NONE;

    FreeButtonInternal(buttonid);

    ++gButtonGeneration;

    if (call) {
        CallLocalFunction("OnButtonDestroy", "i", _:buttonid);
    }

    return true;
}

stock bool:IsValidButton(Button:buttonid) {
    return IsValidButtonInternal(buttonid);
}

stock GetButtonCount() {
    return gButtonCount;
}

stock bool:SetButtonPos(Button:buttonid, Float:x, Float:y, Float:z) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    if (!IsButtonAreaSphereInternal(buttonid)) {
        return false;
    }

    new const
        STREAMER_TAG_AREA:areaid = gButtonData[buttonid][E_BUTTON_AREA_ID],
        STREAMER_TAG_3D_TEXT_LABEL:labelid = gButtonData[buttonid][E_BUTTON_TEXT3D_ID]
    ;

    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_X, x);
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_Y, y);
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_Z, z);

    if (IsValidDynamic3DTextLabel(labelid)) {
        new
            Float:labelZ
        ;

        Streamer_GetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, labelid, E_STREAMER_Z, labelZ);

        labelZ -= gButtonData[buttonid][E_BUTTON_Z];

        Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, labelid, E_STREAMER_X, x);
        Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, labelid, E_STREAMER_Y, y);
        Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, labelid, E_STREAMER_Z, z + labelZ);
    }

    gButtonData[buttonid][E_BUTTON_X] = x;
    gButtonData[buttonid][E_BUTTON_Y] = y;
    gButtonData[buttonid][E_BUTTON_Z] = z;

    return true;
}

stock bool:GetButtonPos(Button:buttonid, &Float:x, &Float:y, &Float:z) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    x = gButtonData[buttonid][E_BUTTON_X];
    y = gButtonData[buttonid][E_BUTTON_Y];
    z = gButtonData[buttonid][E_BUTTON_Z];

    return true;
}

stock bool:SetButtonKeys(Button:buttonid, KEY:buttonKeys) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    gButtonData[buttonid][E_BUTTON_KEYS] = buttonKeys;

    return true;
}

stock KEY:GetButtonKeys(Button:buttonid) {
    if (!IsValidButtonInternal(buttonid)) {
        return KEY_NONE;
    }

    return gButtonData[buttonid][E_BUTTON_KEYS];
}

stock bool:SetButtonPriority(Button:buttonid, priority) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    gButtonData[buttonid][E_BUTTON_PRIORITY] = priority;

    ++gButtonGeneration;

    return true;
}

stock GetButtonPriority(Button:buttonid) {
    if (!IsValidButtonInternal(buttonid)) {
        return 0;
    }

    return gButtonData[buttonid][E_BUTTON_PRIORITY];
}

stock bool:SetButtonSize(Button:buttonid, Float:size) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    if (!IsButtonAreaSphereInternal(buttonid)) {
        return false;
    }

    Streamer_SetFloatData(
        STREAMER_TYPE_AREA,
        gButtonData[buttonid][E_BUTTON_AREA_ID],
        E_STREAMER_SIZE,
        size
    );

    gButtonData[buttonid][E_BUTTON_SIZE] = size;

    return true;
}

stock Float:GetButtonSize(Button:buttonid) {
    if (!IsValidButtonInternal(buttonid)) {
        return 0.0;
    }

    return gButtonData[buttonid][E_BUTTON_SIZE];
}

stock bool:SetButtonVirtualWorld(Button:buttonid, worldid) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    if (gButtonData[buttonid][E_BUTTON_WORLD_ID] == worldid) {
        return true;
    }

    new const
        STREAMER_TAG_3D_TEXT_LABEL:labelid = gButtonData[buttonid][E_BUTTON_TEXT3D_ID]
    ;

    Streamer_SetIntData(
        STREAMER_TYPE_AREA,
        gButtonData[buttonid][E_BUTTON_AREA_ID],
        E_STREAMER_WORLD_ID,
        worldid
    );

    if (IsValidDynamic3DTextLabel(labelid)) {
        Streamer_SetIntData(STREAMER_TYPE_3D_TEXT_LABEL, labelid, E_STREAMER_WORLD_ID, worldid);
    }

    gButtonData[buttonid][E_BUTTON_WORLD_ID] = worldid;

    return true;
}

stock GetButtonVirtualWorld(Button:buttonid) {
    if (!IsValidButtonInternal(buttonid)) {
        return -1;
    }

    return gButtonData[buttonid][E_BUTTON_WORLD_ID];
}

stock bool:SetButtonInterior(Button:buttonid, interiorid) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    if (gButtonData[buttonid][E_BUTTON_INTERIOR_ID] == interiorid) {
        return true;
    }

    new const
        STREAMER_TAG_3D_TEXT_LABEL:labelid = gButtonData[buttonid][E_BUTTON_TEXT3D_ID]
    ;

    Streamer_SetIntData(
        STREAMER_TYPE_AREA,
        gButtonData[buttonid][E_BUTTON_AREA_ID],
        E_STREAMER_INTERIOR_ID,
        interiorid
    );

    if (IsValidDynamic3DTextLabel(labelid)) {
        Streamer_SetIntData(STREAMER_TYPE_3D_TEXT_LABEL, labelid, E_STREAMER_INTERIOR_ID, interiorid);
    }

    gButtonData[buttonid][E_BUTTON_INTERIOR_ID] = interiorid;

    return true;
}

stock GetButtonInterior(Button:buttonid) {
    if (!IsValidButtonInternal(buttonid)) {
        return -1;
    }

    return gButtonData[buttonid][E_BUTTON_INTERIOR_ID];
}

stock bool:SetButtonPopupText(Button:buttonid, const message[], OPEN_MP_TAGS:...) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    new
        text[MAX_BUTTON_POPUP_LENGTH]
    ;

    Format(text, sizeof (text), message, ___(2));

    strpack(gButtonData[buttonid][E_BUTTON_TEXT], text, MAX_BUTTON_POPUP_LENGTH char);

    return true;
}

stock bool:GetButtonPopupText(Button:buttonid, output[], size = sizeof (output)) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    strunpack(output, gButtonData[buttonid][E_BUTTON_TEXT], size);

    return true;
}

stock bool:SetButtonLabelText(Button:buttonid, const message[], colour = 0xFFFFFFFF, Float:drawDistance = BUTTON_DEFAULT_DRAW_DISTANCE, Float:streamDistance = BUTTON_DEFAULT_STREAM_DISTANCE, bool:testLOS = true, Float:offsetZ = 0.0, OPEN_MP_TAGS:...) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    new
        text[MAX_BUTTON_POPUP_LENGTH]
    ;

    Format(text, sizeof (text), message, ___(7));

    DestroyButtonLabel(buttonid);

    gButtonData[buttonid][E_BUTTON_TEXT3D_ID] = CreateDynamic3DTextLabel(
        text,
        colour,
        gButtonData[buttonid][E_BUTTON_X],
        gButtonData[buttonid][E_BUTTON_Y],
        gButtonData[buttonid][E_BUTTON_Z] + offsetZ,
        drawDistance,
        _,
        _,
        testLOS,
        gButtonData[buttonid][E_BUTTON_WORLD_ID],
        gButtonData[buttonid][E_BUTTON_INTERIOR_ID],
        _,
        streamDistance
    );

    return true;
}

stock bool:UpdateButtonLabelText(Button:buttonid, const message[], colour = 0xFFFFFFFF, OPEN_MP_TAGS:...) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    new const
        STREAMER_TAG_3D_TEXT_LABEL:labelid = gButtonData[buttonid][E_BUTTON_TEXT3D_ID]
    ;

    if (!IsValidDynamic3DTextLabel(labelid)) {
        return false;
    }

    new
        text[MAX_BUTTON_POPUP_LENGTH]
    ;

    Format(text, sizeof (text), message, ___(3));

    UpdateDynamic3DTextLabelText(labelid, colour, text);

    return true;
}

stock bool:DestroyButtonLabel(Button:buttonid) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    new const
        STREAMER_TAG_3D_TEXT_LABEL:labelid = gButtonData[buttonid][E_BUTTON_TEXT3D_ID]
    ;

    if (!IsValidDynamic3DTextLabel(labelid)) {
        return false;
    }

    DestroyDynamic3DTextLabel(labelid);

    gButtonData[buttonid][E_BUTTON_TEXT3D_ID] = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;

    return true;
}

stock STREAMER_TAG_3D_TEXT_LABEL:GetButtonLabel(Button:buttonid) {
    if (!IsValidButtonInternal(buttonid)) {
        return STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;
    }

    return gButtonData[buttonid][E_BUTTON_TEXT3D_ID];
}

stock STREAMER_TAG_AREA:GetButtonArea(Button:buttonid) {
    if (!IsValidButtonInternal(buttonid)) {
        return STREAMER_TAG_AREA:INVALID_STREAMER_ID;
    }

    return gButtonData[buttonid][E_BUTTON_AREA_ID];
}

stock bool:SetButtonArea(Button:buttonid, STREAMER_TAG_AREA:areaid) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    if (!IsValidDynamicArea(areaid)) {
        return false;
    }

    if (gButtonData[buttonid][E_BUTTON_AREA_ID] == areaid) {
        return false;
    }

    DestroyDynamicArea(gButtonData[buttonid][E_BUTTON_AREA_ID]);

    gButtonData[buttonid][E_BUTTON_AREA_ID] = areaid;

    MarkButtonAreaInternal(buttonid, areaid);

    ++gButtonGeneration;

    return true;
}

stock bool:IsButtonAreaSphere(Button:buttonid) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    return IsButtonAreaSphereInternal(buttonid);
}

stock bool:GetPlayerAngleToButton(playerid, Button:buttonid, &Float:angle) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    new
        Float:x,
        Float:y,
        Float:z
    ;

    GetPlayerPos(playerid, x, y, z);

    angle = GetAngleToPoint(x, y, gButtonData[buttonid][E_BUTTON_X], gButtonData[buttonid][E_BUTTON_Y]);

    return true;
}

stock bool:GetButtonAngleToPlayer(playerid, Button:buttonid, &Float:angle) {
    if (!IsValidButtonInternal(buttonid)) {
        return false;
    }

    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    new
        Float:x,
        Float:y,
        Float:z
    ;

    GetPlayerPos(playerid, x, y, z);

    angle = GetAngleToPoint(gButtonData[buttonid][E_BUTTON_X], gButtonData[buttonid][E_BUTTON_Y], x, y);

    return true;
}

stock bool:IsPlayerInAnyButton(playerid) {
    return GetPlayerButton(playerid) != INVALID_BUTTON_ID;
}

stock Button:GetPlayerHeldButton(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_BUTTON_ID;
    }

    if (!IsValidButtonInternal(gPlayerButtonHeld[playerid])) {
        gPlayerButtonHeld[playerid] = INVALID_BUTTON_ID;
    }

    return gPlayerButtonHeld[playerid];
}

stock Button:GetPlayerButton(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_BUTTON_ID;
    }

    if (gPlayerButtonGeneration[playerid] != gButtonGeneration) {
        RefreshPlayerButtonInternal(playerid);
    }

    return gPlayerButtonCurrent[playerid];
}

stock GetPlayerNearbyButtons(playerid, Button:buttons[MAX_AREA_BUTTONS] = { INVALID_BUTTON_ID, ... }, size = sizeof (buttons)) {
    if (!IsPlayerConnected(playerid)) {
        return 0;
    }

    if (GetPlayerButton(playerid) == INVALID_BUTTON_ID) {
        return 0;
    }

    new
        STREAMER_TAG_AREA:areas[MAX_OVERLAPPING_AREAS],
        Button:buttonid,
        count
    ;

    for (new i, areaCount = GetPlayerDynamicAreas(playerid, areas); i != areaCount && count != size; ++i) {
        if (!GetButtonAreaDataInternal(areas[i], buttonid)) {
            continue;
        }

        new const
            priority = gButtonData[buttonid][E_BUTTON_PRIORITY]
        ;

        buttons[count] = buttonid;

        for (new slot = count++; slot && gButtonData[buttons[slot - 1]][E_BUTTON_PRIORITY] < priority; --slot) {
            buttons[slot] = buttons[slot - 1];
            buttons[slot - 1] = buttonid;
        }
    }

    return count;
}

/**
 * # Calls
 */

hook OnPlayerConnect(playerid) {
    gPlayerButtonCurrent[playerid] = INVALID_BUTTON_ID;
    gPlayerButtonHeld[playerid] = INVALID_BUTTON_ID;
    gPlayerButtonGeneration[playerid] = gButtonGeneration;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    gPlayerButtonCurrent[playerid] = INVALID_BUTTON_ID;
    gPlayerButtonHeld[playerid] = INVALID_BUTTON_ID;
    gPlayerButtonGeneration[playerid] = gButtonGeneration;

    return 0;
}

hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    new const
        Button:heldButton = GetPlayerHeldButton(playerid)
    ;

    if (heldButton != INVALID_BUTTON_ID) {
        if (newkeys & gButtonData[heldButton][E_BUTTON_KEYS]) {
            return 0;
        }

        gPlayerButtonHeld[playerid] = INVALID_BUTTON_ID;

        return CallLocalFunction("OnButtonRelease", "ii", playerid, _:heldButton);
    }

    new const
        Button:buttonid = GetPlayerButton(playerid)
    ;

    if (buttonid != INVALID_BUTTON_ID) {
        new const
            KEY:buttonKeys = gButtonData[buttonid][E_BUTTON_KEYS]
        ;

        if ((newkeys & buttonKeys) && !(oldkeys & buttonKeys)) {
            gPlayerButtonHeld[playerid] = buttonid;

            return CallLocalFunction("OnButtonPress", "ii", playerid, _:buttonid);
        }
    }

    return 0;
}

hook OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA:areaid) {
    new
        Button:buttonid
    ;

    if (!GetButtonAreaDataInternal(areaid, buttonid)) {
        return 0;
    }

    RefreshPlayerButtonInternal(playerid);

    CallLocalFunction("OnPlayerEnterButtonArea", "ii", playerid, _:buttonid);

    return 0;
}

hook OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA:areaid) {
    new
        Button:buttonid
    ;

    if (!GetButtonAreaDataInternal(areaid, buttonid)) {
        return 0;
    }

    RefreshPlayerButtonInternal(playerid);

    if (gPlayerButtonHeld[playerid] == buttonid) {
        gPlayerButtonHeld[playerid] = INVALID_BUTTON_ID;

        CallLocalFunction("OnButtonRelease", "ii", playerid, _:buttonid);
    }

    CallLocalFunction("OnPlayerLeaveButtonArea", "ii", playerid, _:buttonid);

    return 0;
}
