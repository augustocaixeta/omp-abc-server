#if defined _INC_EXTENDED_TEXT_STYLES
    #endinput
#endif
#define _INC_EXTENDED_TEXT_STYLES

// y_va is the one piece of YSI in here, and it is here because Pawn cannot hand
// the arguments of one function to another one: doing it by hand reads the first
// cell of a string instead of its address, so every %s comes out as a single
// letter. It brings nothing else in with it and it does not fight pp-hooks.
#include <YSI_Coding\y_va>
#include <pp-hooks>

/**
 * # Header
 */

// What the two styles that carry sentences are drawn at, and how wide they are
// allowed to run. Written here rather than only inside the textdraw, because
// wrapping has to measure the text in the same font and size it will be drawn
// in or it breaks in the wrong place.
#define GAME_TEXT_FONT                  (TEXT_DRAW_FONT_1)

#define GAME_TEXT_STUNT_LETTER_WIDTH    (0.580)
#define GAME_TEXT_STUNT_LETTER_HEIGHT   (2.420)
#define GAME_TEXT_POPUP_LETTER_WIDTH    (0.416)

// A textdraw never wraps on its own. TextSize gives it a box and an alignment
// reference and nothing more, so a sentence wider than the screen simply runs
// off it: the disqualification warning measures 566 wide, and STUNT is centred
// at 380, which leaves it 520 before it starts falling off the right hand edge.
//
// With td-string-width included before this, sentences are broken to fit on
// their own and nothing has to carry a ~n~ put in by hand. Without it they are
// left exactly as they were.
// The same 460 the style is given as its text size, so a line is broken at the
// edge the style itself says it has.
#if !defined GAME_TEXT_STUNT_WIDTH
    #define GAME_TEXT_STUNT_WIDTH (460.0)
#endif

#if !defined GAME_TEXT_POPUP_WIDTH
    #define GAME_TEXT_POPUP_WIDTH (212.0)
#endif

#if !defined MAX_GAME_TEXT_LENGTH
    #define MAX_GAME_TEXT_LENGTH (128)
#endif

#define IS_VALID_GAME_TEXT_STYLE(%0) \
    (GAME_TEXT_STYLE_INVAL < (%0) < GAME_TEXT_STYLE_COUNT)

enum GAME_TEXT_STYLE: (+= 1) {
    GAME_TEXT_STYLE_INVAL,

    // The money counter, which the game draws in two colours: what was gained
    // and what was lost. They are one indicator wearing two faces, so they
    // stand in the same place and only ever one of them is up.
    GAME_TEXT_STYLE_MONEY,
    GAME_TEXT_STYLE_MONEY_NEGATIVE,

    GAME_TEXT_STYLE_STUNT,
    GAME_TEXT_STYLE_POPUP,
    GAME_TEXT_STYLE_COUNT
};

// Which style is the other face of this one, or GAME_TEXT_STYLE_INVAL for the
// styles that stand alone. Two of them sharing a place on the screen would
// otherwise be drawn over each other and neither would be readable.
#define GAME_TEXT_STYLE_OPPOSITE(%0) (     (%0) == GAME_TEXT_STYLE_MONEY ? GAME_TEXT_STYLE_MONEY_NEGATIVE :     (%0) == GAME_TEXT_STYLE_MONEY_NEGATIVE ? GAME_TEXT_STYLE_MONEY :     GAME_TEXT_STYLE_INVAL)

// One of each style is made for a player when they connect and kept for as long
// as they are here: showing a message is then a string and a show, instead of
// building a textdraw and throwing it away every time somebody is told
// something.
static
    PlayerText:gGameTextText[MAX_PLAYERS][GAME_TEXT_STYLE_COUNT] = { { INVALID_PLAYER_TEXT_DRAW, ... }, ... },
    gGameTextTimerID[MAX_PLAYERS][GAME_TEXT_STYLE_COUNT] = { { 0, ... }, ... },
    bool:gGameTextShown[MAX_PLAYERS][GAME_TEXT_STYLE_COUNT] = { { false, ... }, ... }
;

// A message can be asked to wait its turn: one style crossing another on the
// screen is two things being said at once and neither being read. What waits is
// only the showing of it, because the words are written into the textdraw when
// they are handed over and the textdraw is where a message lives anyway, so
// none of it has to be kept anywhere to be remembered until then.
static
    gGameTextDelayID[MAX_PLAYERS][GAME_TEXT_STYLE_COUNT] = { { 0, ... }, ... },
    gGameTextDelayTime[MAX_PLAYERS][GAME_TEXT_STYLE_COUNT] = { { 0, ... }, ... }
;

/**
 * # Functions
 */

forward bool:PlayerGameTextShow(playerid, GAME_TEXT_STYLE:style, time, const message[], OPEN_MP_TAGS:...);
forward bool:PlayerGameTextShowDelayed(playerid, GAME_TEXT_STYLE:style, delay, time, const message[], OPEN_MP_TAGS:...);
forward bool:PlayerGameTextHide(playerid, GAME_TEXT_STYLE:style);
forward bool:IsPlayerGameTextShown(playerid, GAME_TEXT_STYLE:style);
forward bool:IsPlayerGameTextPending(playerid, GAME_TEXT_STYLE:style);

/**
 * # Internal
 */

// One row a style, so a new one is a row added here and nothing else touched.
// The alternative was a case per style in the function below, which is where
// this started and where every new style meant another eight lines of the same
// eight calls in a different order.
//
// A zero means "leave it at whatever a new textdraw is": the natives are only
// called for the settings a row actually asks for, so a style that wants no box
// simply does not fill the box colour in.
static enum _:E_GAME_TEXT_STYLE_DATA {
    Float:E_GAME_TEXT_X,
    Float:E_GAME_TEXT_Y,

    Float:E_GAME_TEXT_LETTER_WIDTH,
    Float:E_GAME_TEXT_LETTER_HEIGHT,

    Float:E_GAME_TEXT_SIZE_X,
    Float:E_GAME_TEXT_SIZE_Y,

    TEXT_DRAW_ALIGN:E_GAME_TEXT_ALIGN,
    TEXT_DRAW_FONT:E_GAME_TEXT_FONT,

    E_GAME_TEXT_COLOUR,
    E_GAME_TEXT_BOX_COLOUR,
    E_GAME_TEXT_BACKGROUND_COLOUR,

    bool:E_GAME_TEXT_USE_BOX,
    bool:E_GAME_TEXT_PROPORTIONAL,

    E_GAME_TEXT_SHADOW,
    E_GAME_TEXT_OUTLINE,

    // How wide a line may run before it is broken, or zero for a style that
    // carries a number rather than a sentence and has nowhere to break.
    Float:E_GAME_TEXT_WRAP_WIDTH
};

static const gGameTextStyleData[GAME_TEXT_STYLE_COUNT][E_GAME_TEXT_STYLE_DATA] = {
    // GAME_TEXT_STYLE_INVAL, which is never drawn and is here to keep the row
    // numbers and the style numbers the same.
    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, TEXT_DRAW_ALIGN:0, TEXT_DRAW_FONT:0, 0, 0, 0, false, false, 0, 0, 0.0 },

    // The money counter, in the two colours it is drawn in.
    { 608.000, 77.000, 0.550, 2.200, 10.000, 200.000,
      TEXT_DRAW_ALIGN_RIGHT, TEXT_DRAW_FONT_PRICEDOWN,
      0x36682CFF, 0x000000FF, 0, false, false, 0, 2, 0.0 },

    { 608.000, 77.000, 0.550, 2.200, 10.000, 200.000,
      TEXT_DRAW_ALIGN_RIGHT, TEXT_DRAW_FONT_PRICEDOWN,
      0xB4191DFF, 0x000000FF, 0, false, false, 0, 2, 0.0 },

    // The centred line the game announces on, at the size the game draws it.
    { 380.000, 341.150, GAME_TEXT_STUNT_LETTER_WIDTH, GAME_TEXT_STUNT_LETTER_HEIGHT, 40.000, GAME_TEXT_STUNT_WIDTH,
      TEXT_DRAW_ALIGN_CENTER, GAME_TEXT_FONT,
      0xDDDDDBFF, 0, 0x000000FF, false, true, 2, 0, GAME_TEXT_STUNT_WIDTH },

    // The boxed panel on the left.
    { 28.000, 150.000, GAME_TEXT_POPUP_LETTER_WIDTH, 1.760, GAME_TEXT_POPUP_WIDTH, 200.000,
      TEXT_DRAW_ALIGN:0, GAME_TEXT_FONT,
      0xFFFFFF96, 0x000000DD, 0, true, true, 0, 0, GAME_TEXT_POPUP_WIDTH }
};

static stock PlayerGameTextCreateInternal(playerid, GAME_TEXT_STYLE:style) {
    new const
        Float:x = gGameTextStyleData[style][E_GAME_TEXT_X],
        Float:y = gGameTextStyleData[style][E_GAME_TEXT_Y]
    ;

    new const
        PlayerText:t = CreatePlayerTextDraw(playerid, x, y, " ")
    ;

    PlayerTextDrawLetterSize(playerid, t,
        gGameTextStyleData[style][E_GAME_TEXT_LETTER_WIDTH],
        gGameTextStyleData[style][E_GAME_TEXT_LETTER_HEIGHT]);

    PlayerTextDrawTextSize(playerid, t,
        gGameTextStyleData[style][E_GAME_TEXT_SIZE_X],
        gGameTextStyleData[style][E_GAME_TEXT_SIZE_Y]);

    PlayerTextDrawFont(playerid, t, gGameTextStyleData[style][E_GAME_TEXT_FONT]);
    PlayerTextDrawColour(playerid, t, gGameTextStyleData[style][E_GAME_TEXT_COLOUR]);
    PlayerTextDrawSetShadow(playerid, t, gGameTextStyleData[style][E_GAME_TEXT_SHADOW]);
    PlayerTextDrawSetOutline(playerid, t, gGameTextStyleData[style][E_GAME_TEXT_OUTLINE]);
    PlayerTextDrawSetProportional(playerid, t, gGameTextStyleData[style][E_GAME_TEXT_PROPORTIONAL]);

    // The rest are only worth a call when the row asked for them. Alignment
    // left alone is the left alignment a textdraw already has, and a colour of
    // zero is not a colour anybody wants drawn.
    if (gGameTextStyleData[style][E_GAME_TEXT_ALIGN] != TEXT_DRAW_ALIGN:0) {
        PlayerTextDrawAlignment(playerid, t, gGameTextStyleData[style][E_GAME_TEXT_ALIGN]);
    }

    if (gGameTextStyleData[style][E_GAME_TEXT_BOX_COLOUR]) {
        PlayerTextDrawBoxColour(playerid, t, gGameTextStyleData[style][E_GAME_TEXT_BOX_COLOUR]);
    }

    if (gGameTextStyleData[style][E_GAME_TEXT_BACKGROUND_COLOUR]) {
        PlayerTextDrawBackgroundColour(playerid, t, gGameTextStyleData[style][E_GAME_TEXT_BACKGROUND_COLOUR]);
    }

    if (gGameTextStyleData[style][E_GAME_TEXT_USE_BOX]) {
        PlayerTextDrawUseBox(playerid, t, true);
    }

    gGameTextText[playerid][style] = t;
    gGameTextShown[playerid][style] = false;
}

static stock PlayerGameTextTimerInternal(playerid, GAME_TEXT_STYLE:style) {
    if (!gGameTextTimerID[playerid][style]) {
        return;
    }

    KillTimer(gGameTextTimerID[playerid][style]);
    gGameTextTimerID[playerid][style] = 0;
}

static stock PlayerGameTextDelayInternal(playerid, GAME_TEXT_STYLE:style) {
    if (!gGameTextDelayID[playerid][style]) {
        return;
    }

    KillTimer(gGameTextDelayID[playerid][style]);
    gGameTextDelayID[playerid][style] = 0;
}

// Putting a message up and starting the clock on it. Both ways in reach this:
// one straight away, one when the message it was waiting for has gone.
static stock PlayerGameTextRaiseInternal(playerid, GAME_TEXT_STYLE:style, time) {
    // A style that shares its place with another takes that place: money gained
    // and money lost are the same counter, and the one being said now is the
    // one that is true.
    new const
        GAME_TEXT_STYLE:opposite = GAME_TEXT_STYLE_OPPOSITE(style)
    ;

    if (opposite != GAME_TEXT_STYLE_INVAL) {
        PlayerGameTextHide(playerid, opposite);
    }

    PlayerTextDrawShow(playerid, gGameTextText[playerid][style]);

    gGameTextShown[playerid][style] = true;

    if (time) {
        gGameTextTimerID[playerid][style] = SetTimerEx("@GameText_Elapse", time, false, "ii", playerid, _:style);
    }
}

forward @GameText_Appear(playerid, GAME_TEXT_STYLE:style);
forward @GameText_Elapse(playerid, GAME_TEXT_STYLE:style);

public @GameText_Appear(playerid, GAME_TEXT_STYLE:style) {
    // The timer is dead the moment it fires, and saying so here is what keeps
    // anything cancelling it from trying to kill it a second time.
    gGameTextDelayID[playerid][style] = 0;

    if (!IsPlayerConnected(playerid)) {
        return 0;
    }

    if (gGameTextText[playerid][style] == INVALID_PLAYER_TEXT_DRAW) {
        return 0;
    }

    // How long it stays is counted from when it appeared, not from when it was
    // asked for: waiting its turn is not part of its time on the screen.
    PlayerGameTextRaiseInternal(playerid, style, gGameTextDelayTime[playerid][style]);

    return 0;
}

public @GameText_Elapse(playerid, GAME_TEXT_STYLE:style) {
    // The timer is dead the moment it fires, and saying so here is what keeps
    // the hide below from trying to kill it a second time.
    gGameTextTimerID[playerid][style] = 0;

    PlayerGameTextHide(playerid, style);

    return 0;
}

// Everything the two ways of showing a message have in common, which is all of
// it bar the formatting: by the time this is reached the words are words, and
// the only question left is whether they go up now or in a moment.
static stock bool:PlayerGameTextApplyInternal(playerid, GAME_TEXT_STYLE:style, delay, time, const text[]) {
    new const
        PlayerText:t = gGameTextText[playerid][style]
    ;

    if (t == INVALID_PLAYER_TEXT_DRAW) {
        return false;
    }

    // A message replacing one already on the screen takes its time with it,
    // instead of being taken down when the one before it was due. One still
    // waiting its turn goes the same way: it was going to say something this
    // one is saying instead.
    PlayerGameTextTimerInternal(playerid, style);
    PlayerGameTextDelayInternal(playerid, style);

    // Taken down before the new words are written in, or the message on screen
    // would turn into the new one while that one is still meant to be waiting.
    if (delay && gGameTextShown[playerid][style]) {
        PlayerTextDrawHide(playerid, t);

        gGameTextShown[playerid][style] = false;
    }

    // The text goes through a placeholder of its own: it has been formatted
    // already, and this native would format it a second time otherwise.
    PlayerTextDrawSetString(playerid, t, "%s", text);

    if (delay) {
        gGameTextDelayTime[playerid][style] = time;
        gGameTextDelayID[playerid][style] = SetTimerEx("@GameText_Appear", delay, false, "ii", playerid, _:style);

        return true;
    }

    PlayerGameTextRaiseInternal(playerid, style, time);

    return true;
}

/**
 * # External
 */

// Breaks a sentence to fit the style it is going into, when there is something
// here able to measure it. The money styles are left alone: a running total is
// never a sentence and has nowhere to break.
static stock PlayerGameTextWrapInternal(GAME_TEXT_STYLE:style, text[], size) {
    #if defined SplitTextDrawString
        new const
            Float:width = gGameTextStyleData[style][E_GAME_TEXT_WRAP_WIDTH]
        ;

        if (width <= 0.0) {
            return;
        }

        // Measured in the font and at the size the row will be drawn at, which is
        // the same row: measuring one and drawing another breaks in the wrong place.
        SplitTextDrawString(text, width,
            gGameTextStyleData[style][E_GAME_TEXT_LETTER_WIDTH],
            _:gGameTextStyleData[style][E_GAME_TEXT_FONT],
            gGameTextStyleData[style][E_GAME_TEXT_OUTLINE],
            gGameTextStyleData[style][E_GAME_TEXT_PROPORTIONAL],
            size);
    #else
        #pragma unused style, text, size
    #endif
}

stock bool:PlayerGameTextShow(playerid, GAME_TEXT_STYLE:style, time, const message[], OPEN_MP_TAGS:...) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IS_VALID_GAME_TEXT_STYLE(style)) {
        return false;
    }

    new
        text[MAX_GAME_TEXT_LENGTH]
    ;

    // Nothing following the text means nothing inside it is a placeholder
    // either: a per cent sign in a message stays a per cent sign.
    if (numargs() == 4) {
        Format(text, sizeof (text), "%s", message);
    } else {
        Format(text, sizeof (text), message, ___(4));
    }

    PlayerGameTextWrapInternal(style, text, sizeof (text));

    return PlayerGameTextApplyInternal(playerid, style, 0, time, text);
}

/**
 * The same as showing one, except that it waits `delay` milliseconds before it
 * appears, and `time` is then counted from when it did. What it is for is one
 * style not landing on top of another: a box down the left and a banner across
 * the middle are two things being said at once, and the reader gets neither.
 *
 * A delay of nothing behaves exactly like PlayerGameTextShow.
 */
stock bool:PlayerGameTextShowDelayed(playerid, GAME_TEXT_STYLE:style, delay, time, const message[], OPEN_MP_TAGS:...) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IS_VALID_GAME_TEXT_STYLE(style)) {
        return false;
    }

    new
        text[MAX_GAME_TEXT_LENGTH]
    ;

    if (numargs() == 5) {
        Format(text, sizeof (text), "%s", message);
    } else {
        Format(text, sizeof (text), message, ___(5));
    }

    PlayerGameTextWrapInternal(style, text, sizeof (text));

    return PlayerGameTextApplyInternal(playerid, style, delay, time, text);
}

stock bool:PlayerGameTextHide(playerid, GAME_TEXT_STYLE:style) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IS_VALID_GAME_TEXT_STYLE(style)) {
        return false;
    }

    // A message still on its way up is called off by a hide as much as one
    // already there: what is being asked for is nothing on the screen.
    new const
        bool:pending = gGameTextDelayID[playerid][style] != 0
    ;

    PlayerGameTextDelayInternal(playerid, style);

    if (!gGameTextShown[playerid][style]) {
        return pending;
    }

    PlayerGameTextTimerInternal(playerid, style);

    PlayerTextDrawHide(playerid, gGameTextText[playerid][style]);

    gGameTextShown[playerid][style] = false;

    return true;
}

stock bool:IsPlayerGameTextShown(playerid, GAME_TEXT_STYLE:style) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IS_VALID_GAME_TEXT_STYLE(style)) {
        return false;
    }

    return gGameTextShown[playerid][style];
}

/**
 * Whether something of this style is written and waiting rather than on screen.
 */
stock bool:IsPlayerGameTextPending(playerid, GAME_TEXT_STYLE:style) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IS_VALID_GAME_TEXT_STYLE(style)) {
        return false;
    }

    return gGameTextDelayID[playerid][style] != 0;
}

/**
 * # Calls
 */

hook OnPlayerConnect(playerid) {
    for (new GAME_TEXT_STYLE:style = GAME_TEXT_STYLE_INVAL; ++style != GAME_TEXT_STYLE_COUNT;) {
        gGameTextTimerID[playerid][style] = 0;
        gGameTextDelayID[playerid][style] = 0;
        gGameTextDelayTime[playerid][style] = 0;

        PlayerGameTextCreateInternal(playerid, style);
    }

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    // The textdraws of a player leave with them, but a timer of theirs would
    // still be counting down against whoever comes in on that id next.
    for (new GAME_TEXT_STYLE:style = GAME_TEXT_STYLE_INVAL; ++style != GAME_TEXT_STYLE_COUNT;) {
        PlayerGameTextTimerInternal(playerid, style);
        PlayerGameTextDelayInternal(playerid, style);

        gGameTextText[playerid][style] = INVALID_PLAYER_TEXT_DRAW;
        gGameTextShown[playerid][style] = false;
    }

    return 0;
}
