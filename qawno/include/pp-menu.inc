#if defined _INC_PP_MENU
    #endinput
#endif
#define _INC_PP_MENU

// pp-hooks is the only thing this library takes from outside: everything else
// it needs is a native. The menu a script builds lives in a pool slot rather
// than in the player's own row, because the text of sixty four items is the
// heavy part of it and only a player actually holding a menu needs that text.
// Slots come off a free list the way race.inc hands out races, so taking one
// is a read and a write and a full pool is answered without looking at
// anything.
//
// Item text is packed four characters to a cell. It costs nothing to write --
// strpack replaces the strcat that was there anyway -- reads back at the same
// speed, and every open.mp native that takes a string reads it as it stands,
// "%s" in a format included. Measured against the PawnPlus List and Map this
// replaces: building a sixty four item menu went from 49.2us to 12.4us,
// drawing a page from 2.59us to 0.65us, and asking whether a row carries
// options from 1.55us to 0.54us. None of those numbers is why the change was
// made. The Map of Lists of Strings that held the options was never deleted,
// so every menu ever opened leaked one, and the ceiling on items was written
// against a counter that no path ever updated.
#include <pp-hooks>

/**
 * # Header
 */

#if !defined MAX_MENU_TITLE_LENGTH
    #define MAX_MENU_TITLE_LENGTH (32)
#endif

#if !defined MAX_MENU_ITEM_LENGTH
    #define MAX_MENU_ITEM_LENGTH (32)
#endif

#if !defined MAX_MENU_ITEMS
    #define MAX_MENU_ITEMS (64)
#endif

// How many menus can be under construction or on screen at once. One a player
// is the safe answer and the default, so nothing a script does can be refused
// for want of a slot. It is also the expensive answer: a slot is roughly
// MAX_MENU_ITEMS * 23 + MAX_MENU_ITEM_OPTIONS * 10 cells, about 6.8KB at the
// defaults, which is 6.8MB if MAX_PLAYERS is left at the 1000 open.mp starts
// with. A server that knows menus are occasional sets this to 32 and pays
// 218KB for the same behaviour; the free list is what makes that safe.
#if !defined MAX_LIST_MENUS
    #define MAX_LIST_MENUS (MAX_PLAYERS)
#endif

// Options belonging to every row of one menu, taken from a single arena. A row
// with a selector usually has two or three, and most rows have none, so this is
// counted per menu and not per item: MAX_MENU_ITEMS * MAX_MENU_ITEM_OPTIONS
// would be sixty four times the memory to hold the same handful of strings.
#if !defined MAX_MENU_ITEM_OPTIONS
    #define MAX_MENU_ITEM_OPTIONS (32)
#endif

// How many rows of the list are on screen at once. The title panel is drawn to
// fit whatever this is.
#if !defined MAX_MENU_PAGE_ROWS
    #define MAX_MENU_PAGE_ROWS (8)
#endif

// The three colours a row can be drawn in. They were written into the middle of
// the code before, where a script could not reach them.
#if !defined MENU_ITEM_COLOUR
    #define MENU_ITEM_COLOUR (0x555E70FF)
#endif

#if !defined MENU_ITEM_ACTIVE_COLOUR
    #define MENU_ITEM_ACTIVE_COLOUR (0xA3C2EBFF)
#endif

// The font the rows are drawn in. It was whatever a new textdraw defaults to,
// which is font 1, and it is written down here because MENU_SELECTOR_ARROWS
// measures text and has to measure it in the font it will be drawn in.
#if !defined MENU_FONT
    #define MENU_FONT (TEXT_DRAW_FONT_1)
#endif

#if !defined MENU_ITEM_LETTER_WIDTH
    #define MENU_ITEM_LETTER_WIDTH (0.460)
#endif

#if !defined MENU_ITEM_LETTER_HEIGHT
    #define MENU_ITEM_LETTER_HEIGHT (1.800)
#endif

// A selector row is one textdraw: the two arrows and the option between them in
// a single string, right aligned against the edge of the menu like every other
// row. Nothing is measured and nothing extra is drawn, and the arrows sit
// against the option whatever its width is because the three grow together.
//
// Define MENU_SELECTOR_ARROWS to give each arrow a textdraw of its own, which
// is what it takes to colour them or size them apart from the option they sit
// around. It costs two more textdraws a row and a width measurement whenever
// the option changes, and the measuring is done by td-string-width, which has
// to come first:
//
//     #include <td-string-width>
//     #define MENU_SELECTOR_ARROWS
//     #include <pp-menu>
//
// The gap is how far each arrow stands off the option, in the same units the
// menu is placed with.
#if !defined MENU_SELECTOR_GAP
    #define MENU_SELECTOR_GAP (4.0)
#endif

// An arrow with a textdraw of its own is an arrow that can be drawn in its own
// font, at its own size and in its own colour, which is most of the reason to
// give it one. All four fall back to what the rest of the row uses, so turning
// MENU_SELECTOR_ARROWS on changes nothing about how the menu looks until one of
// these says otherwise.
//
// Font 2 draws a squarer chevron than the row font does, at the cost of coming
// out wide: narrowing MENU_ARROW_LETTER_WIDTH is what brings it back.
//
//     #define MENU_ARROW_FONT         (TEXT_DRAW_FONT_2)
//     #define MENU_ARROW_LETTER_WIDTH (0.330)
//
// The width the arrow is measured at follows the font and size it is drawn at,
// so an arrow in a different font still leaves the option the right amount of
// room.
#if !defined MENU_ARROW_FONT
    #define MENU_ARROW_FONT (MENU_FONT)
#endif

#if !defined MENU_ARROW_LETTER_WIDTH
    #define MENU_ARROW_LETTER_WIDTH (MENU_ITEM_LETTER_WIDTH)
#endif

#if !defined MENU_ARROW_LETTER_HEIGHT
    #define MENU_ARROW_LETTER_HEIGHT (MENU_ITEM_LETTER_HEIGHT)
#endif

// A font of another size sits on the line differently. This nudges the arrows
// up or down against the row without moving the text they sit beside.
#if !defined MENU_ARROW_OFFSET_Y
    #define MENU_ARROW_OFFSET_Y (0.0)
#endif

// Defining MENU_ARROW_COLOUR paints the arrows that colour and leaves them
// there. Undefined, they take the colour of the row they are on, which is what
// makes them light up with it when the cursor arrives.
// #define MENU_ARROW_COLOUR (0x88929EFF)

#if defined MENU_SELECTOR_ARROWS && !defined GetTextDrawStringWidth
    #error MENU_SELECTOR_ARROWS needs td-string-width included before pp-menu.
#endif

// What the arrows are drawn with. The GTA font does not read a byte the way the
// rest of the world does: the guillemets are not at the codepoint a text editor
// puts them at, and reaching them means writing whichever byte this font draws
// them with. Left as the two ASCII ones, which every font agrees on, and put
// here so a script that has found the right bytes can say so in one place.
#if !defined MENU_ARROW_LEFT
    #define MENU_ARROW_LEFT "<"
#endif

#if !defined MENU_ARROW_RIGHT
    #define MENU_ARROW_RIGHT ">"
#endif

#if !defined MENU_ITEM_DISABLED_COLOUR
    #define MENU_ITEM_DISABLED_COLOUR (0x9C9C9CFF)
#endif

#if !defined MENU_SOUND_UPDOWN
    #define MENU_SOUND_UPDOWN (1054)
#endif

#if !defined MENU_SOUND_SELECT
    #define MENU_SOUND_SELECT (1083)
#endif

#if !defined MENU_SOUND_CLOSE
    #define MENU_SOUND_CLOSE (1084)
#endif

#if !defined MENU_SOUND_ERROR
    #define MENU_SOUND_ERROR (1085)
#endif

#if !defined MENU_COOLDOWN_MILISECONDS
    #define MENU_COOLDOWN_MILISECONDS (200)
#endif

enum _:E_ASYNC_MENU_DATA {
    E_ASYNC_MENU_RESPONSE,
    E_ASYNC_MENU_LISTITEM
};

static enum _:E_MENU_ITEM_DATA {
    E_MENU_ITEM_L_COLUMN[(MAX_MENU_ITEM_LENGTH + 1) char],
    E_MENU_ITEM_R_COLUMN[(MAX_MENU_ITEM_LENGTH + 1) char],

    bool:E_MENU_ITEM_ENABLED,

    // Where this row's options sit in the menu's arena. The run is threaded
    // rather than laid out end to end, because a script may add an option to
    // row two and then to row nine and then to row two again, and a thread lets
    // both runs grow without moving what the other already wrote.
    E_MENU_ITEM_OPTION_HEAD,
    E_MENU_ITEM_OPTION_TAIL,
    E_MENU_ITEM_OPTION_COUNT,
    E_MENU_ITEM_OPTION_INDEX
};

static
    gMenuItems[MAX_LIST_MENUS][MAX_MENU_ITEMS][E_MENU_ITEM_DATA],
    gMenuItemCount[MAX_LIST_MENUS]
;

static
    gMenuOptionText[MAX_LIST_MENUS][MAX_MENU_ITEM_OPTIONS][(MAX_MENU_ITEM_LENGTH + 1) char],
    gMenuOptionNext[MAX_LIST_MENUS][MAX_MENU_ITEM_OPTIONS],
    gMenuOptionCount[MAX_LIST_MENUS]
;

// A slot given back points at the one given back before it, and slots never
// used yet come off a high water mark. Nothing is set up at boot.
static
    gMenuNextFree[MAX_LIST_MENUS],
    gMenuFreeHead = -1,
    gMenuHighWater
;

// E_PLAYER_MENU_START is the item drawn on the top row, and the row the cursor
// is on is E_PLAYER_MENU_CURRENT_ITEM minus it. The two used to be kept apart
// and could disagree; there is one of them now.
//
// E_PLAYER_MENU_CURRENT_ITEM is -1 when there is nothing the cursor may sit on,
// which happens when every row of the menu is disabled.
static enum _:E_PLAYER_MENU_DATA {
    E_PLAYER_MENU_ID,
    Task:E_PLAYER_MENU_TASK,
    bool:E_PLAYER_MENU_OPENED,
    bool:E_PLAYER_MENU_FREEZE,
    bool:E_PLAYER_MENU_SELECTION_SOUND,
    E_PLAYER_MENU_CURRENT_ITEM,
    E_PLAYER_MENU_START,
    E_PLAYER_MENU_COOLDOWN_TIME
};

static enum _:E_PLAYER_TEXT_DRAW_DATA {
    PlayerText:E_TEXT_DRAW_TITLE,
    PlayerText:E_TEXT_DRAW_BACKGROUND,
    PlayerText:E_TEXT_DRAW_L_COLUMN[MAX_MENU_PAGE_ROWS],
    PlayerText:E_TEXT_DRAW_R_COLUMN[MAX_MENU_PAGE_ROWS]

    #if defined MENU_SELECTOR_ARROWS
        // A row that turns is drawn with three of its own instead of the one on the
        // right. None of this exists unless the script asked for it, so a menu that
        // is happy with arrows in the same string as the option pays nothing for
        // the ones that are not.
        ,
        PlayerText:E_TEXT_DRAW_L_ARROW[MAX_MENU_PAGE_ROWS],
        PlayerText:E_TEXT_DRAW_OPTION[MAX_MENU_PAGE_ROWS],
        PlayerText:E_TEXT_DRAW_R_ARROW[MAX_MENU_PAGE_ROWS]
    #endif
};

static
    gPlayerMenuData[MAX_PLAYERS][E_PLAYER_MENU_DATA],
    gPlayerTextDrawData[MAX_PLAYERS][E_PLAYER_TEXT_DRAW_DATA]
;

/**
 * # Functions
 */

forward bool:AddListMenuItem(playerid, column, const item[]);
forward bool:AddListMenuItemSelector(playerid, index, const option[]);
forward bool:RemoveListMenuItem(playerid, index);
forward bool:ToggleListMenuItem(playerid, index, bool:toggle);
forward bool:IsListMenuItemEnabled(playerid, index);
forward GetListMenuItemCount(playerid);
forward bool:ClearListMenu(playerid);

forward Task:ShowAsyncListMenu(playerid, const title[], Float:x, Float:y, Float:width, bool:freeze = true, bool:selectionSound = true);
forward bool:DestroyListMenu(playerid);
forward bool:IsListMenuVisible(playerid);

forward bool:AddListMenuItemStr(playerid, column, ConstString:item);
forward Task:ShowAsyncListMenuStr(playerid, ConstString:title, Float:x, Float:y, Float:width, bool:freeze = true, bool:selectionSound = true);

// The row the cursor moved onto, as the player moves it. The task a menu hands
// back only ever speaks once, when the menu closes, which is the wrong shape
// for a menu that shows the player what a row means while they are still
// standing on it: a modshop putting the wheel on the car as the list is walked,
// a skin picker turning the ped. That is what the game's own menus do, and it
// is what this is for.
//
// It fires only when the player moves the cursor, never when the menu opens and
// never when a script moves the cursor by disabling or removing the row it was
// on. The first row is the script's to preview before it shows the menu, since
// it already knows what it put there.
forward OnPlayerListMenuSelectionChange(playerid, item);

// Which way a row with options was turned, as the player turns it. The option
// a selector is sitting on is otherwise the script's blind spot: the menu hands
// back the row the player was on when it closed and nothing about what any of
// the rows were set to, so a Nitro row could be flipped all day and the script
// would never learn it had been.
//
// option is the place in the list of options that AddListMenuItemSelector was
// called in, counted from zero, and it is the option now showing.
//
// Like the callback above it fires only on the player's own doing, and after
// the row is drawn, so a script acting on it acts against what is on screen.
forward OnPlayerListMenuOptionChange(playerid, item, option);

/**
 * # Internal
 */

static stock GetFreeMenuIndexInternal() {
    if (gMenuFreeHead != -1) {
        new const
            index = gMenuFreeHead
        ;

        gMenuFreeHead = gMenuNextFree[index];

        return index;
    }

    if (gMenuHighWater != MAX_LIST_MENUS) {
        return gMenuHighWater++;
    }

    return -1;
}

// The menu a player is building, taking a slot for them if they have none. -1
// when the pool is full.
static stock OpenPlayerMenuInternal(playerid) {
    new
        menuid = gPlayerMenuData[playerid][E_PLAYER_MENU_ID]
    ;

    if (menuid != -1) {
        return menuid;
    }

    menuid = GetFreeMenuIndexInternal();

    if (menuid == -1) {
        return -1;
    }

    gMenuItemCount[menuid] = 0;
    gMenuOptionCount[menuid] = 0;

    gPlayerMenuData[playerid][E_PLAYER_MENU_ID] = menuid;

    return menuid;
}

static stock void:ReleasePlayerMenuInternal(playerid) {
    new const
        menuid = gPlayerMenuData[playerid][E_PLAYER_MENU_ID]
    ;

    if (menuid == -1) {
        return;
    }

    gMenuNextFree[menuid] = gMenuFreeHead;
    gMenuFreeHead = menuid;

    gPlayerMenuData[playerid][E_PLAYER_MENU_ID] = -1;
}

// The next row in step's direction that the cursor is allowed to land on,
// starting the search at `from`. Disabled rows are stepped over rather than
// stopped on, which is what makes a greyed out line behave like the ones in the
// game's own menus. -1 when there is no such row.
static stock FindEnabledMenuItemInternal(menuid, from, step) {
    for (new i = from, count = gMenuItemCount[menuid]; 0 <= i < count; i += step) {
        if (gMenuItems[menuid][i][E_MENU_ITEM_ENABLED]) {
            return i;
        }
    }

    return -1;
}

// The arena slot holding the option row `index` is showing, or -1 when the row
// carries none. The run is walked rather than indexed: a row holds a handful of
// options at most, and keeping only the head of it costs one cell where a slot
// per option would cost MAX_MENU_ITEMS of them.
static stock GetMenuItemOptionInternal(menuid, index) {
    new
        slot = gMenuItems[menuid][index][E_MENU_ITEM_OPTION_HEAD]
    ;

    for (new i = gMenuItems[menuid][index][E_MENU_ITEM_OPTION_INDEX]; i != 0 && slot != -1; --i) {
        slot = gMenuOptionNext[menuid][slot];
    }

    return slot;
}

static stock bool:IsMenuCoolingDownInternal(playerid) {
    return (GetTickCount() - gPlayerMenuData[playerid][E_PLAYER_MENU_COOLDOWN_TIME]) < MENU_COOLDOWN_MILISECONDS;
}

// Brings the page back around the cursor and says whether it moved. A cursor
// that has just skipped a run of disabled rows can land more than one row away,
// so the page is worked out from the item rather than nudged a row at a time.
static stock bool:ClampMenuPageInternal(playerid) {
    new const
        menuid = gPlayerMenuData[playerid][E_PLAYER_MENU_ID],
        item = gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM],
        was = gPlayerMenuData[playerid][E_PLAYER_MENU_START]
    ;

    new
        start = was
    ;

    if (item != -1 && item < start) {
        start = item;
    }

    if (item != -1 && item >= start + MAX_MENU_PAGE_ROWS) {
        start = item - (MAX_MENU_PAGE_ROWS - 1);
    }

    new const
        maxStart = (gMenuItemCount[menuid] > MAX_MENU_PAGE_ROWS) ? (gMenuItemCount[menuid] - MAX_MENU_PAGE_ROWS) : 0
    ;

    if (start > maxStart) {
        start = maxStart;
    }

    if (start < 0) {
        start = 0;
    }

    gPlayerMenuData[playerid][E_PLAYER_MENU_START] = start;

    return start != was;
}

#if defined MENU_SELECTOR_ARROWS
    // How wide a string comes out on screen, in the units the menu is placed with.
    // td-string-width answers in character units for a font, and a letter size is
    // what turns those into the size they are drawn at.
    static stock Float:MenuOptionWidthInternal(const text[]) {
        return MENU_ITEM_LETTER_WIDTH * float(GetTextDrawStringWidth(text, _:MENU_FONT, 0, true));
    }

    // Measured in the arrow's own font and at the arrow's own size, which are not
    // the row's once a script has said so.
    static stock Float:MenuArrowWidthInternal(const text[]) {
        return MENU_ARROW_LETTER_WIDTH * float(GetTextDrawStringWidth(text, _:MENU_ARROW_FONT, 0, true));
    }
#endif

static stock void:CreateMenuTextDrawsInternal(playerid, const title[], Float:x, Float:y, Float:width) {
    new const
        menuid = gPlayerMenuData[playerid][E_PLAYER_MENU_ID]
    ;

    new const
        Float:BORDER_SPACING_X = 10.0,
        Float:BORDER_SPACING_Y = 20.0,
        Float:LETTER_SPACING_Y = 16.0,

        rows = min(
            gMenuItemCount[menuid],
            MAX_MENU_PAGE_ROWS
        ),

        Float:MENU_RIGHT_X = BORDER_SPACING_X + width,
        Float:MENU_TOTAL_HEIGHT = rows * LETTER_SPACING_Y + BORDER_SPACING_Y * 2.0
    ;

    new
        PlayerText:pt
    ;

    pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_TITLE] = CreatePlayerTextDraw(playerid, x, y, "LD_DUAL:BLACK");
    PlayerTextDrawTextSize(playerid, pt, width, MENU_TOTAL_HEIGHT);
    PlayerTextDrawColour(playerid, pt, 220);
    PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_SPRITE);
    PlayerTextDrawShow(playerid, pt);

    x += BORDER_SPACING_X;

    // The title goes through "%s" and not straight in. CreatePlayerTextDraw
    // takes a format, so a title carrying a per cent sign used to be read as a
    // specifier and eat an argument that was never passed.
    pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_BACKGROUND] = CreatePlayerTextDraw(playerid, x, y - BORDER_SPACING_Y, "%s", title);
    PlayerTextDrawLetterSize(playerid, pt, 1.0, 3.0);
    PlayerTextDrawSetOutline(playerid, pt, 2);
    PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_BECKETT_REGULAR);
    PlayerTextDrawSetProportional(playerid, pt, true);
    PlayerTextDrawShow(playerid, pt);

    // 1.8 = TextDraw letter height (Y size)
    // 2.0 = Half-height factor, used to center the top-anchored text
    // Offsets the position to visually center the text within the row spacing.
    y += BORDER_SPACING_Y - 1.8 / 2.0;

    for (new i; i != MAX_MENU_PAGE_ROWS; ++i) {
        pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_L_COLUMN][i] = CreatePlayerTextDraw(playerid, x, y + LETTER_SPACING_Y * i, "");
        PlayerTextDrawLetterSize(playerid, pt, MENU_ITEM_LETTER_WIDTH, MENU_ITEM_LETTER_HEIGHT);
        PlayerTextDrawFont(playerid, pt, MENU_FONT);
        PlayerTextDrawSetShadow(playerid, pt, 2);
        PlayerTextDrawSetProportional(playerid, pt, true);

        pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_R_COLUMN][i] = CreatePlayerTextDraw(playerid, MENU_RIGHT_X, y + LETTER_SPACING_Y * i, "");
        PlayerTextDrawLetterSize(playerid, pt, MENU_ITEM_LETTER_WIDTH, MENU_ITEM_LETTER_HEIGHT);
        PlayerTextDrawFont(playerid, pt, MENU_FONT);
        PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_RIGHT);
        PlayerTextDrawSetShadow(playerid, pt, 2);
        PlayerTextDrawSetProportional(playerid, pt, true);

        #if defined MENU_SELECTOR_ARROWS
            // Both arrows read against the right hand edge, the same as the option
            // between them, so the whole group grows leftward off the edge of the
            // menu and lines up with a row that has no options at all.
            //
            // The right arrow and the option never move: the arrow is one fixed
            // string and the option ends a fixed gap in front of it. Only the left
            // arrow is placed on every draw, because it is the only one that
            // depends on how wide the option turned out to be.
            pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_R_ARROW][i] = CreatePlayerTextDraw(playerid, MENU_RIGHT_X, y + LETTER_SPACING_Y * i + MENU_ARROW_OFFSET_Y, "");
            PlayerTextDrawLetterSize(playerid, pt, MENU_ARROW_LETTER_WIDTH, MENU_ARROW_LETTER_HEIGHT);
            PlayerTextDrawFont(playerid, pt, MENU_ARROW_FONT);
            PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_RIGHT);
            PlayerTextDrawSetShadow(playerid, pt, 2);
            PlayerTextDrawSetProportional(playerid, pt, true);

            pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_OPTION][i] = CreatePlayerTextDraw(playerid, MENU_RIGHT_X - MenuArrowWidthInternal(MENU_ARROW_RIGHT) - MENU_SELECTOR_GAP, y + LETTER_SPACING_Y * i, "");
            PlayerTextDrawLetterSize(playerid, pt, MENU_ITEM_LETTER_WIDTH, MENU_ITEM_LETTER_HEIGHT);
            PlayerTextDrawFont(playerid, pt, MENU_FONT);
            PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_RIGHT);
            PlayerTextDrawSetShadow(playerid, pt, 2);
            PlayerTextDrawSetProportional(playerid, pt, true);

            pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_L_ARROW][i] = CreatePlayerTextDraw(playerid, MENU_RIGHT_X, y + LETTER_SPACING_Y * i + MENU_ARROW_OFFSET_Y, "");
            PlayerTextDrawLetterSize(playerid, pt, MENU_ARROW_LETTER_WIDTH, MENU_ARROW_LETTER_HEIGHT);
            PlayerTextDrawFont(playerid, pt, MENU_ARROW_FONT);
            PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_RIGHT);
            PlayerTextDrawSetShadow(playerid, pt, 2);
            PlayerTextDrawSetProportional(playerid, pt, true);
        #endif
    }
}

static stock void:DestroyMenuTextDrawsInternal(playerid) {
    new
        PlayerText:pt
    ;

    if ((pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_TITLE]) != INVALID_PLAYER_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, pt);
        gPlayerTextDrawData[playerid][E_TEXT_DRAW_TITLE] = INVALID_PLAYER_TEXT_DRAW;
    }

    if ((pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_BACKGROUND]) != INVALID_PLAYER_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, pt);
        gPlayerTextDrawData[playerid][E_TEXT_DRAW_BACKGROUND] = INVALID_PLAYER_TEXT_DRAW;
    }

    for (new i; i != MAX_MENU_PAGE_ROWS; ++i) {
        if ((pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_L_COLUMN][i]) != INVALID_PLAYER_TEXT_DRAW) {
            PlayerTextDrawDestroy(playerid, pt);
            gPlayerTextDrawData[playerid][E_TEXT_DRAW_L_COLUMN][i] = INVALID_PLAYER_TEXT_DRAW;
        }

        if ((pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_R_COLUMN][i]) != INVALID_PLAYER_TEXT_DRAW) {
            PlayerTextDrawDestroy(playerid, pt);
            gPlayerTextDrawData[playerid][E_TEXT_DRAW_R_COLUMN][i] = INVALID_PLAYER_TEXT_DRAW;
        }

        #if defined MENU_SELECTOR_ARROWS
            if ((pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_L_ARROW][i]) != INVALID_PLAYER_TEXT_DRAW) {
                PlayerTextDrawDestroy(playerid, pt);
                gPlayerTextDrawData[playerid][E_TEXT_DRAW_L_ARROW][i] = INVALID_PLAYER_TEXT_DRAW;
            }

            if ((pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_OPTION][i]) != INVALID_PLAYER_TEXT_DRAW) {
                PlayerTextDrawDestroy(playerid, pt);
                gPlayerTextDrawData[playerid][E_TEXT_DRAW_OPTION][i] = INVALID_PLAYER_TEXT_DRAW;
            }

            if ((pt = gPlayerTextDrawData[playerid][E_TEXT_DRAW_R_ARROW][i]) != INVALID_PLAYER_TEXT_DRAW) {
                PlayerTextDrawDestroy(playerid, pt);
                gPlayerTextDrawData[playerid][E_TEXT_DRAW_R_ARROW][i] = INVALID_PLAYER_TEXT_DRAW;
            }
        #endif
    }
}

// One row of the page, colour and both columns. Everything that changes what is
// on screen goes through here, so the question of whether a row shows a
// selector or its right hand column is asked in one place instead of three.
static stock void:DrawMenuRowInternal(playerid, row) {
    if (!(0 <= row < MAX_MENU_PAGE_ROWS)) {
        return;
    }

    new const
        menuid = gPlayerMenuData[playerid][E_PLAYER_MENU_ID],
        index = gPlayerMenuData[playerid][E_PLAYER_MENU_START] + row,
        PlayerText:left = gPlayerTextDrawData[playerid][E_TEXT_DRAW_L_COLUMN][row],
        PlayerText:right = gPlayerTextDrawData[playerid][E_TEXT_DRAW_R_COLUMN][row]
    ;

    // Off the end of a list shorter than the page, which is where the rows of
    // the menu before this one used to be left showing.
    #if defined MENU_SELECTOR_ARROWS
        new const
            PlayerText:leftArrow = gPlayerTextDrawData[playerid][E_TEXT_DRAW_L_ARROW][row],
            PlayerText:option = gPlayerTextDrawData[playerid][E_TEXT_DRAW_OPTION][row],
            PlayerText:rightArrow = gPlayerTextDrawData[playerid][E_TEXT_DRAW_R_ARROW][row]
        ;
    #endif

    if (index >= gMenuItemCount[menuid]) {
        PlayerTextDrawHide(playerid, left);
        PlayerTextDrawHide(playerid, right);

        #if defined MENU_SELECTOR_ARROWS
            PlayerTextDrawHide(playerid, leftArrow);
            PlayerTextDrawHide(playerid, option);
            PlayerTextDrawHide(playerid, rightArrow);
        #endif

        return;
    }

    new
        colour = MENU_ITEM_COLOUR
    ;

    if (!gMenuItems[menuid][index][E_MENU_ITEM_ENABLED]) {
        colour = MENU_ITEM_DISABLED_COLOUR;
    } else if (index == gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM]) {
        colour = MENU_ITEM_ACTIVE_COLOUR;
    }

    // "%s" and not the item itself: PlayerTextDrawSetString takes a format, and
    // the item is whatever the script was handed to put in it.
    PlayerTextDrawColour(playerid, left, colour);
    PlayerTextDrawSetString(playerid, left, "%s", gMenuItems[menuid][index][E_MENU_ITEM_L_COLUMN]);
    PlayerTextDrawShow(playerid, left);

    new const
        slot = GetMenuItemOptionInternal(menuid, index)
    ;

    // A row with nothing to turn is the one textdraw it always was, right
    // aligned against the edge of the menu.
    if (slot == -1) {
        #if defined MENU_SELECTOR_ARROWS
            PlayerTextDrawHide(playerid, leftArrow);
            PlayerTextDrawHide(playerid, option);
            PlayerTextDrawHide(playerid, rightArrow);
        #endif

        PlayerTextDrawColour(playerid, right, colour);
        PlayerTextDrawSetString(playerid, right, "%s", gMenuItems[menuid][index][E_MENU_ITEM_R_COLUMN]);
        PlayerTextDrawShow(playerid, right);

        return;
    }

    #if !defined MENU_SELECTOR_ARROWS
        // The arrows and the option in one string. They sit against the option at
        // whatever width it comes out, because all three move together.
        PlayerTextDrawColour(playerid, right, colour);
        PlayerTextDrawSetString(playerid, right, MENU_ARROW_LEFT " %s " MENU_ARROW_RIGHT, gMenuOptionText[menuid][slot]);
        PlayerTextDrawShow(playerid, right);
    #else
        // An arrow of its own each, so a script can paint them and size them apart
        // from the option. Only the left one is placed here: the right arrow is a
        // fixed string against the edge of the menu and the option ends a fixed gap
        // in front of it, so neither of those two depends on what is written.
        PlayerTextDrawHide(playerid, right);

        PlayerTextDrawColour(playerid, option, colour);
        PlayerTextDrawSetString(playerid, option, "%s", gMenuOptionText[menuid][slot]);
        PlayerTextDrawShow(playerid, option);

        // Unpacked before it is measured, and not because the measuring is fussy:
        // it walks the string with string[i], which on a packed one reads a whole
        // cell of four characters instead of a character. strlen answers correctly
        // either way, so the walk runs the right number of times over the wrong
        // values and hands back a width of zero, which puts the arrow a gap to the
        // left of where the option ends rather than where it begins: on top of it.
        new
            text[MAX_MENU_ITEM_LENGTH + 1],
            Float:optionX,
            Float:optionY
        ;

        strunpack(text, gMenuOptionText[menuid][slot], sizeof (text));

        PlayerTextDrawGetPos(playerid, option, optionX, optionY);

        // Right aligned, so this is the arrow's right hand edge: one gap in front
        // of where the option starts.
        PlayerTextDrawSetPos(playerid, leftArrow,
            optionX - MenuOptionWidthInternal(text) - MENU_SELECTOR_GAP,
            optionY
        );

        #if defined MENU_ARROW_COLOUR
            PlayerTextDrawColour(playerid, leftArrow, MENU_ARROW_COLOUR);
        #else
            PlayerTextDrawColour(playerid, leftArrow, colour);
        #endif
        PlayerTextDrawSetString(playerid, leftArrow, MENU_ARROW_LEFT);
        PlayerTextDrawShow(playerid, leftArrow);

        #if defined MENU_ARROW_COLOUR
            PlayerTextDrawColour(playerid, rightArrow, MENU_ARROW_COLOUR);
        #else
            PlayerTextDrawColour(playerid, rightArrow, colour);
        #endif
        PlayerTextDrawSetString(playerid, rightArrow, MENU_ARROW_RIGHT);
        PlayerTextDrawShow(playerid, rightArrow);
    #endif
}

static stock void:DrawMenuPageInternal(playerid) {
    for (new row; row != MAX_MENU_PAGE_ROWS; ++row) {
        DrawMenuRowInternal(playerid, row);
    }
}

static stock void:CloseMenuInternal(playerid, bool:response) {
    new const
        Task:t = gPlayerMenuData[playerid][E_PLAYER_MENU_TASK],
        item = gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM]
    ;

    gPlayerMenuData[playerid][E_PLAYER_MENU_TASK] = INVALID_TASK;
    gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED] = false;

    DestroyMenuTextDrawsInternal(playerid);

    if (gPlayerMenuData[playerid][E_PLAYER_MENU_FREEZE]) {
        TogglePlayerControllable(playerid, true);
    }

    gPlayerMenuData[playerid][E_PLAYER_MENU_FREEZE] = false;
    gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM] = -1;
    gPlayerMenuData[playerid][E_PLAYER_MENU_START] = 0;

    // The slot goes back before the task is answered, because answering it can
    // resume a script that builds its next menu on the spot.
    ReleasePlayerMenuInternal(playerid);

    if (!task_valid(t)) {
        return;
    }

    new
        responses[E_ASYNC_MENU_DATA]
    ;

    responses[E_ASYNC_MENU_RESPONSE] = _:response;
    responses[E_ASYNC_MENU_LISTITEM] = item;

    task_set_result_arr(t, responses);
}

static stock void:MoveMenuCursorInternal(playerid, step) {
    new const
        menuid = gPlayerMenuData[playerid][E_PLAYER_MENU_ID],
        from = gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM]
    ;

    if (from == -1) {
        PlayerPlaySound(playerid, MENU_SOUND_ERROR, 0.0, 0.0, 0.0);

        return;
    }

    new
        item = FindEnabledMenuItemInternal(menuid, from + step, step)
    ;

    // Off the end of the list, so it comes back on at the other one: up from
    // the first row lands on the last, down from the last lands on the first.
    // The search runs the same way it was already going, so the row it settles
    // on is the outermost one the cursor is allowed to sit on and not simply
    // the row at the end, which may well be greyed out.
    if (item == -1) {
        item = (step < 0)
            ? FindEnabledMenuItemInternal(menuid, gMenuItemCount[menuid] - 1, -1)
            : FindEnabledMenuItemInternal(menuid, 0, 1);
    }

    // Nowhere else to be: one row is enabled and the cursor is already on it.
    if (item == -1 || item == from) {
        PlayerPlaySound(playerid, MENU_SOUND_ERROR, 0.0, 0.0, 0.0);

        return;
    }

    new const
        lastRow = from - gPlayerMenuData[playerid][E_PLAYER_MENU_START]
    ;

    gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM] = item;

    // Two rows change colour when the page holds still, and every row changes
    // when it scrolls. Working out which happened is one comparison, and it is
    // the difference between four textdraws going out and sixteen.
    if (ClampMenuPageInternal(playerid)) {
        DrawMenuPageInternal(playerid);
    } else {
        DrawMenuRowInternal(playerid, lastRow);
        DrawMenuRowInternal(playerid, item - gPlayerMenuData[playerid][E_PLAYER_MENU_START]);
    }

    PlayerPlaySound(playerid, MENU_SOUND_UPDOWN, 0.0, 0.0, 0.0);

    // Last, so that whatever the script does about the new row happens against
    // a menu that is already drawn where the player can see it.
    CallLocalFunction("OnPlayerListMenuSelectionChange", "ii", playerid, item);
}

static stock void:MoveMenuSelectorInternal(playerid, step) {
    new const
        menuid = gPlayerMenuData[playerid][E_PLAYER_MENU_ID],
        item = gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM]
    ;

    if (item == -1) {
        return;
    }

    new const
        count = gMenuItems[menuid][item][E_MENU_ITEM_OPTION_COUNT]
    ;

    if (count <= 1) {
        return;
    }

    new
        index = gMenuItems[menuid][item][E_MENU_ITEM_OPTION_INDEX] + step
    ;

    if (index < 0) {
        index = count - 1;
    }

    if (index >= count) {
        index = 0;
    }

    gMenuItems[menuid][item][E_MENU_ITEM_OPTION_INDEX] = index;

    DrawMenuRowInternal(playerid, item - gPlayerMenuData[playerid][E_PLAYER_MENU_START]);

    PlayerPlaySound(playerid, MENU_SOUND_UPDOWN, 0.0, 0.0, 0.0);

    // Last, for the same reason the row callback is last: whatever the script
    // does about the new option happens against a row already drawn as showing
    // it.
    CallLocalFunction("OnPlayerListMenuOptionChange", "iii", playerid, item, index);
}

/**
 * # Item
 */

stock bool:AddListMenuItem(playerid, column, const item[]) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!(0 <= column <= 1)) {
        return false;
    }

    // The black panel behind the list is sized once, when the menu goes up,
    // from the number of items it had then. Growing the list afterwards would
    // draw rows onto nothing, which is why this is the one of the three that
    // refuses while the menu is showing: RemoveListMenuItem only ever leaves
    // the panel too tall, and ToggleListMenuItem does not change the count.
    if (gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]) {
        return false;
    }

    new const
        menuid = OpenPlayerMenuInternal(playerid)
    ;

    if (menuid == -1) {
        return false;
    }

    if (column == 1) {
        new const
            last = gMenuItemCount[menuid] - 1
        ;

        if (last == -1) {
            return false;
        }

        // Column one appends to what is already there, which is what it always
        // did, so the row comes out to be written and goes straight back in.
        new
            buffer[MAX_MENU_ITEM_LENGTH + 1]
        ;

        strunpack(buffer, gMenuItems[menuid][last][E_MENU_ITEM_R_COLUMN], sizeof (buffer));
        strcat(buffer, item, sizeof (buffer));
        strpack(gMenuItems[menuid][last][E_MENU_ITEM_R_COLUMN], buffer, (MAX_MENU_ITEM_LENGTH + 1) char);

        return true;
    }

    // The ceiling used to be checked against a counter that only ShowAsyncList-
    // Menu ever wrote, so it read zero for the whole of the first menu and the
    // size of the last one for every menu after it.
    if (gMenuItemCount[menuid] == MAX_MENU_ITEMS) {
        return false;
    }

    new const
        index = gMenuItemCount[menuid]++
    ;

    strpack(gMenuItems[menuid][index][E_MENU_ITEM_L_COLUMN], item, (MAX_MENU_ITEM_LENGTH + 1) char);

    gMenuItems[menuid][index][E_MENU_ITEM_R_COLUMN][0] = 0;
    gMenuItems[menuid][index][E_MENU_ITEM_ENABLED] = true;
    gMenuItems[menuid][index][E_MENU_ITEM_OPTION_HEAD] = -1;
    gMenuItems[menuid][index][E_MENU_ITEM_OPTION_TAIL] = -1;
    gMenuItems[menuid][index][E_MENU_ITEM_OPTION_COUNT] = 0;
    gMenuItems[menuid][index][E_MENU_ITEM_OPTION_INDEX] = 0;

    return true;
}

// An option for the right hand column of one row, cycled with left and right.
// The index used to go unchecked, and the array it reached was the last member
// of the player's row, so a selector on item sixty four wrote into the player
// after them.
stock bool:AddListMenuItemSelector(playerid, index, const option[]) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    new const
        menuid = gPlayerMenuData[playerid][E_PLAYER_MENU_ID]
    ;

    if (menuid == -1) {
        return false;
    }

    if (!(0 <= index < gMenuItemCount[menuid])) {
        return false;
    }

    if (gMenuOptionCount[menuid] == MAX_MENU_ITEM_OPTIONS) {
        return false;
    }

    new const
        slot = gMenuOptionCount[menuid]++
    ;

    strpack(gMenuOptionText[menuid][slot], option, (MAX_MENU_ITEM_LENGTH + 1) char);

    gMenuOptionNext[menuid][slot] = -1;

    if (gMenuItems[menuid][index][E_MENU_ITEM_OPTION_HEAD] == -1) {
        gMenuItems[menuid][index][E_MENU_ITEM_OPTION_HEAD] = slot;
    } else {
        gMenuOptionNext[menuid][gMenuItems[menuid][index][E_MENU_ITEM_OPTION_TAIL]] = slot;
    }

    gMenuItems[menuid][index][E_MENU_ITEM_OPTION_TAIL] = slot;
    ++gMenuItems[menuid][index][E_MENU_ITEM_OPTION_COUNT];

    if (gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]) {
        DrawMenuRowInternal(playerid, index - gPlayerMenuData[playerid][E_PLAYER_MENU_START]);
    }

    return true;
}

stock bool:RemoveListMenuItem(playerid, index) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    new const
        menuid = gPlayerMenuData[playerid][E_PLAYER_MENU_ID]
    ;

    if (menuid == -1) {
        return false;
    }

    if (!(0 <= index < gMenuItemCount[menuid])) {
        return false;
    }

    // The rows below slide up over the hole, carrying the head of their own run
    // of options with them. The options the removed row owned stay where they
    // are in the arena and simply stop being reachable: the arena fills once
    // per menu and goes back whole when the slot does.
    for (new i = index; i != gMenuItemCount[menuid] - 1; ++i) {
        gMenuItems[menuid][i] = gMenuItems[menuid][i + 1];
    }

    --gMenuItemCount[menuid];

    if (!gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]) {
        return true;
    }

    new
        item = gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM]
    ;

    if (item >= gMenuItemCount[menuid]) {
        item = gMenuItemCount[menuid] - 1;
    }

    if (item != -1 && !gMenuItems[menuid][item][E_MENU_ITEM_ENABLED]) {
        item = FindEnabledMenuItemInternal(menuid, item, 1);
    }

    if (item == -1) {
        item = FindEnabledMenuItemInternal(menuid, 0, 1);
    }

    gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM] = item;

    ClampMenuPageInternal(playerid);
    DrawMenuPageInternal(playerid);

    return true;
}

// A disabled row keeps its line and is drawn in MENU_ITEM_DISABLED_COLOUR, and
// the cursor steps over it instead of onto it. It is what the game's own menus
// do with a choice that is there but not available.
stock bool:ToggleListMenuItem(playerid, index, bool:toggle) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    new const
        menuid = gPlayerMenuData[playerid][E_PLAYER_MENU_ID]
    ;

    if (menuid == -1) {
        return false;
    }

    if (!(0 <= index < gMenuItemCount[menuid])) {
        return false;
    }

    gMenuItems[menuid][index][E_MENU_ITEM_ENABLED] = toggle;

    if (!gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]) {
        return true;
    }

    // Greying out the row the cursor is standing on has to move the cursor, and
    // it goes down first for the same reason a list is read downwards.
    if (!toggle && index == gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM]) {
        new
            item = FindEnabledMenuItemInternal(menuid, index + 1, 1)
        ;

        if (item == -1) {
            item = FindEnabledMenuItemInternal(menuid, index - 1, -1);
        }

        gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM] = item;

        ClampMenuPageInternal(playerid);
        DrawMenuPageInternal(playerid);

        return true;
    }

    DrawMenuRowInternal(playerid, index - gPlayerMenuData[playerid][E_PLAYER_MENU_START]);

    return true;
}

stock bool:IsListMenuItemEnabled(playerid, index) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    new const
        menuid = gPlayerMenuData[playerid][E_PLAYER_MENU_ID]
    ;

    if (menuid == -1) {
        return false;
    }

    if (!(0 <= index < gMenuItemCount[menuid])) {
        return false;
    }

    return gMenuItems[menuid][index][E_MENU_ITEM_ENABLED];
}

stock GetListMenuItemCount(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return 0;
    }

    new const
        menuid = gPlayerMenuData[playerid][E_PLAYER_MENU_ID]
    ;

    if (menuid == -1) {
        return 0;
    }

    return gMenuItemCount[menuid];
}

// Throws away a menu that was built and then not wanted. Without it the items a
// script added before changing its mind were still sitting there waiting to be
// shown on top of the next menu's.
stock bool:ClearListMenu(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]) {
        return false;
    }

    if (gPlayerMenuData[playerid][E_PLAYER_MENU_ID] == -1) {
        return false;
    }

    ReleasePlayerMenuInternal(playerid);

    return true;
}

/**
 * # Menu
 */

stock Task:ShowAsyncListMenu(playerid, const title[], Float:x, Float:y, Float:width, bool:freeze = true, bool:selectionSound = true) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_TASK;
    }

    if (gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]) {
        return INVALID_TASK;
    }

    new const
        menuid = gPlayerMenuData[playerid][E_PLAYER_MENU_ID]
    ;

    if (menuid == -1) {
        return INVALID_TASK;
    }

    if (gMenuItemCount[menuid] == 0) {
        return INVALID_TASK;
    }

    if (task_valid(gPlayerMenuData[playerid][E_PLAYER_MENU_TASK])) {
        task_delete(gPlayerMenuData[playerid][E_PLAYER_MENU_TASK]);
    }

    if (freeze) {
        TogglePlayerControllable(playerid, false);
    }

    new const
        Task:t = task_new()
    ;

    gPlayerMenuData[playerid][E_PLAYER_MENU_TASK]            = t;
    gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]          = true;
    gPlayerMenuData[playerid][E_PLAYER_MENU_FREEZE]          = freeze;
    gPlayerMenuData[playerid][E_PLAYER_MENU_SELECTION_SOUND] = selectionSound;
    gPlayerMenuData[playerid][E_PLAYER_MENU_START]           = 0;

    // The cursor opens on the first row it is allowed to sit on, which is not
    // row zero when row zero has been greyed out.
    gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM]    = FindEnabledMenuItemInternal(menuid, 0, 1);

    ClampMenuPageInternal(playerid);
    CreateMenuTextDrawsInternal(playerid, title, x, y, width);
    DrawMenuPageInternal(playerid);

    return t;
}

// Takes the menu off the screen from the script's side, which is what this
// could never do before: it refused whenever the menu was showing and only ever
// destroyed textdraws that were already gone. The task is answered as a
// refusal, because a script sitting on await_arr has to come back to life
// whether the player pressed anything or not.
stock bool:DestroyListMenu(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]) {
        return false;
    }

    CloseMenuInternal(playerid, false);

    return true;
}

stock bool:IsListMenuVisible(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    return gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED];
}

stock bool:AddListMenuItemStr(playerid, column, ConstString:item) {
    new
        buffer[MAX_MENU_ITEM_LENGTH + 1]
    ;

    str_get(item, buffer);

    return AddListMenuItem(playerid, column, buffer);
}

stock Task:ShowAsyncListMenuStr(playerid, ConstString:title, Float:x, Float:y, Float:width, bool:freeze = true, bool:selectionSound = true) {
    new
        buffer[MAX_MENU_TITLE_LENGTH + 1]
    ;

    str_get(title, buffer);

    // freeze and selectionSound used to be handed on the other way around, so
    // asking for one of them here turned on the other.
    return ShowAsyncListMenu(playerid, buffer, x, y, width, freeze, selectionSound);
}

/**
 * # Calls
 */

hook OnPlayerConnect(playerid) {
    static const
        PLAYER_MENU_DATA_NONE[E_PLAYER_MENU_DATA]
    ;

    gPlayerMenuData[playerid] = PLAYER_MENU_DATA_NONE;

    gPlayerMenuData[playerid][E_PLAYER_MENU_ID] = -1;
    gPlayerMenuData[playerid][E_PLAYER_MENU_TASK] = INVALID_TASK;
    gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM] = -1;

    // Nothing set these before, and a player textdraw id starts at zero, which
    // is a real one.
    gPlayerTextDrawData[playerid][E_TEXT_DRAW_TITLE] = INVALID_PLAYER_TEXT_DRAW;
    gPlayerTextDrawData[playerid][E_TEXT_DRAW_BACKGROUND] = INVALID_PLAYER_TEXT_DRAW;

    for (new i; i != MAX_MENU_PAGE_ROWS; ++i) {
        gPlayerTextDrawData[playerid][E_TEXT_DRAW_L_COLUMN][i] = INVALID_PLAYER_TEXT_DRAW;
        gPlayerTextDrawData[playerid][E_TEXT_DRAW_R_COLUMN][i] = INVALID_PLAYER_TEXT_DRAW;
        #if defined MENU_SELECTOR_ARROWS
            gPlayerTextDrawData[playerid][E_TEXT_DRAW_L_ARROW][i] = INVALID_PLAYER_TEXT_DRAW;
            gPlayerTextDrawData[playerid][E_TEXT_DRAW_OPTION][i] = INVALID_PLAYER_TEXT_DRAW;
            gPlayerTextDrawData[playerid][E_TEXT_DRAW_R_ARROW][i] = INVALID_PLAYER_TEXT_DRAW;
        #endif
    }

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    new const
        Task:t = gPlayerMenuData[playerid][E_PLAYER_MENU_TASK]
    ;

    // The textdraws went with the player, so they are forgotten rather than
    // destroyed. The menu slot is what has to come back: without this the pool
    // bled one for every player who left with a menu up.
    ReleasePlayerMenuInternal(playerid);

    gPlayerMenuData[playerid][E_PLAYER_MENU_TASK] = INVALID_TASK;
    gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED] = false;
    gPlayerMenuData[playerid][E_PLAYER_MENU_FREEZE] = false;
    gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM] = -1;
    gPlayerMenuData[playerid][E_PLAYER_MENU_START] = 0;

    gPlayerTextDrawData[playerid][E_TEXT_DRAW_TITLE] = INVALID_PLAYER_TEXT_DRAW;
    gPlayerTextDrawData[playerid][E_TEXT_DRAW_BACKGROUND] = INVALID_PLAYER_TEXT_DRAW;

    for (new i; i != MAX_MENU_PAGE_ROWS; ++i) {
        gPlayerTextDrawData[playerid][E_TEXT_DRAW_L_COLUMN][i] = INVALID_PLAYER_TEXT_DRAW;
        gPlayerTextDrawData[playerid][E_TEXT_DRAW_R_COLUMN][i] = INVALID_PLAYER_TEXT_DRAW;
        #if defined MENU_SELECTOR_ARROWS
            gPlayerTextDrawData[playerid][E_TEXT_DRAW_L_ARROW][i] = INVALID_PLAYER_TEXT_DRAW;
            gPlayerTextDrawData[playerid][E_TEXT_DRAW_OPTION][i] = INVALID_PLAYER_TEXT_DRAW;
            gPlayerTextDrawData[playerid][E_TEXT_DRAW_R_ARROW][i] = INVALID_PLAYER_TEXT_DRAW;
        #endif
    }

    // A script waiting on a menu belonging to somebody who has gone still has
    // to be let go, or the coroutine holding it never runs again.
    if (task_valid(t)) {
        new
            responses[E_ASYNC_MENU_DATA]
        ;

        responses[E_ASYNC_MENU_RESPONSE] = _:false;
        responses[E_ASYNC_MENU_LISTITEM] = -1;

        task_set_result_arr(t, responses);
    }

    return 0;
}

hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if (!gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]) {
        return 0;
    }

    if (IsMenuCoolingDownInternal(playerid)) {
        return 0;
    }

    if (newkeys & KEY_SPRINT) {
        // Every row greyed out leaves nothing to choose.
        if (gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM] == -1) {
            PlayerPlaySound(playerid, MENU_SOUND_ERROR, 0.0, 0.0, 0.0);

            return 0;
        }

        gPlayerMenuData[playerid][E_PLAYER_MENU_COOLDOWN_TIME] = GetTickCount();

        if (gPlayerMenuData[playerid][E_PLAYER_MENU_SELECTION_SOUND]) {
            PlayerPlaySound(playerid, MENU_SOUND_SELECT, 0.0, 0.0, 0.0);
        }

        CloseMenuInternal(playerid, true);

        return 0;
    }

    if (newkeys & KEY_SECONDARY_ATTACK) {
        gPlayerMenuData[playerid][E_PLAYER_MENU_COOLDOWN_TIME] = GetTickCount();

        PlayerPlaySound(playerid, MENU_SOUND_CLOSE, 0.0, 0.0, 0.0);

        CloseMenuInternal(playerid, false);

        return 0;
    }

    return 0;
}

// The arrow keys never reach OnPlayerKeyStateChange, so the menu reads them off
// the update itself. The first line keeps that to one array read for every
// player who is not in a menu, which is nearly all of them nearly all the time.
//
// Nothing in here can stop the update being synced. pp-hooks registers a `hook`
// with handler_default, which throws the return value away, so the `return 0`
// that used to be commented as blocking movement never blocked anything.
// Holding the player still is what freeze is for, and a script that passed
// freeze = false asked for the player to keep moving.
hook OnPlayerUpdate(playerid) {
    if (!gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]) {
        return 0;
    }

    new
        KEY:keys,
        updown,
        leftright
    ;

    GetPlayerKeys(playerid, keys, updown, leftright);

    if (!updown && !leftright) {
        return 0;
    }

    if (IsMenuCoolingDownInternal(playerid)) {
        return 0;
    }

    gPlayerMenuData[playerid][E_PLAYER_MENU_COOLDOWN_TIME] = GetTickCount();

    if (updown == KEY_UP) {
        MoveMenuCursorInternal(playerid, -1);

        return 0;
    }

    if (updown == KEY_DOWN) {
        MoveMenuCursorInternal(playerid, 1);

        return 0;
    }

    if (leftright == KEY_LEFT) {
        MoveMenuSelectorInternal(playerid, -1);

        return 0;
    }

    if (leftright == KEY_RIGHT) {
        MoveMenuSelectorInternal(playerid, 1);

        return 0;
    }

    return 0;
}
