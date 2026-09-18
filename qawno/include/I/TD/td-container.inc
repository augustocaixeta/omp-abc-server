#if defined _INC_TD_CONTAINER
    #endinput
#endif
#define _INC_TD_CONTAINER

#include <open.mp>

#define USE_CONTAINER_DURATION_HELPERS

#include <I\container>
#include <I\item-amount>
#include <I\TD\td-array-data>
#include <I\TD\td-item-preview>

#include <pp-hooks>

/**
 * # Header
 */

#if !defined MAX_CONTAINER_MENUS
    #define MAX_CONTAINER_MENUS (ContainerMenu:4)
#endif

// One player textdraw is spent per slot, the amount, out of the 256 a player
// has. Anything past this cannot be drawn, so it is refused where the menu is
// built rather than silently half drawn later.
//
// Every per player cache below is sized by this, so the default stays modest
// and a gamemode that wants a bigger grid raises it knowingly.
#if !defined MAX_CONTAINER_MENU_SLOTS
    #define MAX_CONTAINER_MENU_SLOTS (50)
#endif

#if MAX_CONTAINER_MENU_SLOTS > _:MAX_PLAYER_TEXT_DRAWS
    #error MAX_CONTAINER_MENU_SLOTS needs one player textdraw per slot
#endif

#if !defined CONTAINER_TEXT_DRAW_ARRAY_CELL
    #define CONTAINER_TEXT_DRAW_ARRAY_CELL (0)
#endif

#if !defined CONTAINER_MENU_SELECT_COLOUR
    #define CONTAINER_MENU_SELECT_COLOUR (0x7845C1FF)
#endif

// A menu can be given a frame: the panel it sits in, its name, how much of the
// container is on screen, and a scroll bar to turn the pages with. All of it is
// worked out from the grid, so the only thing a gamemode says is that it wants
// one.
//
// The numbers below are the gaps between the grid and everything drawn around
// it. They are the design and not a limit, but they are named rather than
// written into the layout twice.
#if !defined CONTAINER_FRAME_BORDER
    #define CONTAINER_FRAME_BORDER (1.0)
#endif

#if !defined CONTAINER_FRAME_BAR_WIDTH
    #define CONTAINER_FRAME_BAR_WIDTH (11.0)
#endif

// How far down the bar the thumb can start, which is what the buttons at each
// end of it take up.
#if !defined CONTAINER_FRAME_BUTTON_SPAN
    #define CONTAINER_FRAME_BUTTON_SPAN (13.0)
#endif

#if !defined CONTAINER_FRAME_TITLE_HEIGHT
    #define CONTAINER_FRAME_TITLE_HEIGHT (14.0)
#endif

#if !defined CONTAINER_FRAME_OUTER_MARGIN
    #define CONTAINER_FRAME_OUTER_MARGIN (2.0)
#endif

#if !defined MAX_CONTAINER_MENU_TITLE
    #define MAX_CONTAINER_MENU_TITLE (24)
#endif

// What the counter reads: how much of the container is on this page against
// everything in it.
#if !defined CONTAINER_FRAME_COUNT_FORMAT
    #define CONTAINER_FRAME_COUNT_FORMAT "%i-%i"
#endif

#if !defined CONTAINER_FRAME_PANEL_COLOUR
    #define CONTAINER_FRAME_PANEL_COLOUR (0x181818FF)
#endif

#if !defined CONTAINER_FRAME_INNER_COLOUR
    #define CONTAINER_FRAME_INNER_COLOUR (0x353535FF)
#endif

#if !defined CONTAINER_FRAME_LINE_COLOUR
    #define CONTAINER_FRAME_LINE_COLOUR (0x00FA9EFF)
#endif

#if !defined CONTAINER_FRAME_BAR_COLOUR
    #define CONTAINER_FRAME_BAR_COLOUR (0x202020FF)
#endif

#if !defined CONTAINER_FRAME_BUTTON_COLOUR
    #define CONTAINER_FRAME_BUTTON_COLOUR (0x404040FF)
#endif

#if !defined CONTAINER_FRAME_TEXT_COLOUR
    #define CONTAINER_FRAME_TEXT_COLOUR (0x696969FF)
#endif

// The two ends of the scroll bar, answered through the same array data the slot
// boxes carry. A click arrives knowing which menu it belongs to either way, so
// the page buttons cost no lookup of their own -- they are slots that are not
// on the grid.
#define CONTAINER_MENU_SLOT_PAGE_UP   (-1)
#define CONTAINER_MENU_SLOT_PAGE_DOWN (-2)


#define INVALID_CONTAINER_MENU_ID (ContainerMenu:-1)

enum E_CONTAINER_AMOUNT_POSITION {
    CONTAINER_AMOUNT_TOP_LEFT,
    CONTAINER_AMOUNT_TOP_RIGHT,
    CONTAINER_AMOUNT_BOTTOM_LEFT,
    CONTAINER_AMOUNT_BOTTOM_RIGHT,
    CONTAINER_AMOUNT_CUSTOM
};

static enum _:E_TEXT_DRAW_ARRAY_DATA {
    E_TEXT_DRAW_ARRAY_CELL,
    E_TEXT_DRAW_ARRAY_SLOT,
    ContainerMenu:E_TEXT_DRAW_ARRAY_MENU
};

static enum _:E_CONTAINER_MENU_DATA {
    Float:E_CONTAINER_MENU_START_X,
    Float:E_CONTAINER_MENU_START_Y,
    Float:E_CONTAINER_MENU_BOX_SIZE_X,
    Float:E_CONTAINER_MENU_BOX_SIZE_Y,
    Float:E_CONTAINER_MENU_PADDING,
    E_CONTAINER_MENU_ROWS,
    E_CONTAINER_MENU_COLUMNS,
    E_CONTAINER_MENU_TOTAL_SLOTS,

    // Kept so a slot painted some other colour can be painted back. Nothing else
    // reads it: the boxes are given this at creation and never asked again.
    E_CONTAINER_MENU_COLOUR,

    Float:E_CONTAINER_MENU_AMOUNT_OFFSET_X,
    Float:E_CONTAINER_MENU_AMOUNT_OFFSET_Y,
    Float:E_CONTAINER_MENU_AMOUNT_LETTER_SIZE_X,
    Float:E_CONTAINER_MENU_AMOUNT_LETTER_SIZE_Y,
    TEXT_DRAW_ALIGN:E_CONTAINER_MENU_AMOUNT_ALIGNMENT
};

static
    gContainerMenuData[MAX_CONTAINER_MENUS][E_CONTAINER_MENU_DATA],
    ContainerMenu:gContainerMenuCount
;

// The frame never changes and is the same for everybody, so it is one global
// textdraw per slot, built once and only ever shown or hidden.
//
// The model preview is global too. Size, rotation and model only change the
// definition held on the server -- a player already looking at it is sent
// nothing -- so setting them and showing immediately after hands that state to
// that one player and leaves everybody else on what they were last sent.
static
    Text:gContainerMenuBox[MAX_CONTAINER_MENUS][MAX_CONTAINER_MENU_SLOTS],
    Text:gContainerMenuModel[MAX_CONTAINER_MENUS][MAX_CONTAINER_MENU_SLOTS]
;

// One slot of one menu on one player's screen.
//
// The amount cannot be a global textdraw the way the box and the model are.
// TextDrawSetString is the one setter that does reach everybody currently
// showing the textdraw, so two players looking at different containers would
// read each other's numbers. It is created the first time a player opens the
// menu and then kept: making and destroying them on every open is what a page
// turn would otherwise pay for.
//
// The other three are what is on the screen right now, so a redraw can send
// only what actually differs. Without them a page turn resends every slot even
// when it is showing the same thing, which is where the lag comes from.
//
// The item is held as id + 1 so a zeroed cell reads as INVALID_ITEM_ID.
//
// Menu, slot and field are folded into one index rather than written as three
// more dimensions. Pawn stores an offset cell for every element of every
// dimension but the last, so writing this as [player][menu][slot][field] spends
// 400,000 cells reaching 1,600,000 cells of data. One row per player leaves
// 1,000. Measured against the four separate three-dimensional arrays this
// replaces, it is 19 KB smaller, the same speed for a single field, and 20%
// faster where a redraw computes the slot base once and reads the fields off
// it.
//
// The cost is the tag: an index is not a dimension, so E_PLAYER_SLOT_AMOUNT
// comes back untagged and the four sites that hand it to a PlayerTextDraw
// native say PlayerText: out loud. Nothing outside this file can reach it.
static enum _:E_PLAYER_SLOT_DATA {
    E_PLAYER_SLOT_AMOUNT,
    E_PLAYER_SLOT_ITEM,
    E_PLAYER_SLOT_MODEL,
    E_PLAYER_SLOT_AMOUNT_REVISION,

    // What the box behind this slot was last painted for this player. The box
    // is a global textdraw and a global textdraw has one colour, so two people
    // looking at different crates are only both right because each was shown it
    // after their own colour was set -- which is what this remembers, so a
    // redraw that changes nothing sends nothing.
    E_PLAYER_SLOT_COLOUR
};

#define CONTAINER_MENU_SLOT_COUNT (_:MAX_CONTAINER_MENUS * MAX_CONTAINER_MENU_SLOTS)
#define CONTAINER_MENU_SLOT_BASE(%0,%1) ((_:(%0) * MAX_CONTAINER_MENU_SLOTS + (%1)) * E_PLAYER_SLOT_DATA)

static
    gPlayerSlotData[MAX_PLAYERS][CONTAINER_MENU_SLOT_COUNT * E_PLAYER_SLOT_DATA]
;

// One bit per slot saying whether what is drawn there counts down. Asking the
// item instead costs 245 ns a slot and the answer only changes when the slot
// changes, so the second tick would spend that on every static slot on every
// screen forever. Thirty two slots are cleared with one comparison here.
//
// Every menu shares one flat row rather than a row each, so the tick walks the
// whole player in a single pass. A menu that is closed has its bits cleared, so
// nothing has to be asked about whether it is open either.
#define CONTAINER_MENU_MASK_CELLS ((MAX_CONTAINER_MENU_SLOTS + 31) / 32)
#define CONTAINER_MENU_MASK_SIZE  (_:MAX_CONTAINER_MENUS * CONTAINER_MENU_MASK_CELLS)

static
    gPlayerSlotTimedMask[MAX_PLAYERS][CONTAINER_MENU_MASK_SIZE]
;

// Laid out the same way, one bit per slot.
//
// Shown is whether the number is on the client's screen. It cannot be worked
// out from what the slot holds. Something with no number at all -- a bat, a
// weapon with nothing in it -- never sends one, so a slot can be full and its
// number still never have left the server. Reading "the slot was empty" as the
// answer leaves the next item's text set on a textdraw the client has not been
// shown, and the number simply does not appear.
//
// Covered is whether something drawn over the grid asked for it to be put
// away. It has to outlive a redraw: a slot that fills or counts down under an
// open menu would otherwise send its number again and come back on top of the
// menu, which is the whole thing putting it away was for.
static
    gPlayerSlotShownMask[MAX_PLAYERS][CONTAINER_MENU_MASK_SIZE],
    gPlayerSlotCoveredMask[MAX_PLAYERS][CONTAINER_MENU_MASK_SIZE]
;

// One menu on one player's screen. The container is what INVALID_CONTAINER_ID
// in it means the menu is closed, so it doubles as the open flag, and built
// says whether this player's textdraws for the menu exist yet -- it stays true
// across a close so reopening costs nothing.
static enum _:E_PLAYER_MENU_DATA {
    bool:E_PLAYER_MENU_BUILT,

    Container:E_PLAYER_MENU_CONTAINER,
    E_PLAYER_MENU_PAGE,

    // When it was opened, so two menus on one screen can be told apart by
    // which is in front. Only the order matters and never the number itself,
    // so one counter for the whole server answers it for everybody.
    E_PLAYER_MENU_ORDER
};

static
    gPlayerMenuData[MAX_PLAYERS][MAX_CONTAINER_MENUS][E_PLAYER_MENU_DATA]
;

static
    gMenuOrder = 0
;

// Who has a menu open, kept as a compact list so a refresh walks the people
// actually looking at something rather than everybody online. A container is
// shared -- one player dropping into a locker has to redraw it for whoever else
// is standing at it -- so the playerid that came with the event cannot answer
// this, and the answer is almost always a handful of people.
static
    gMenuViewer[MAX_PLAYERS],
    gMenuViewerSlot[MAX_PLAYERS] = { -1, ... },
    gMenuViewerCount
;

// Everything the frame draws that is the same for everybody looking at the
// menu. Built with the menu when one is asked for, and only ever shown or
// hidden after that.
static enum _:E_CONTAINER_FRAME_DATA {
    bool:E_CONTAINER_FRAME_BUILT,

    Text:E_CONTAINER_FRAME_PANEL,
    Text:E_CONTAINER_FRAME_LINE,
    Text:E_CONTAINER_FRAME_INNER,
    Text:E_CONTAINER_FRAME_BAR,
    Text:E_CONTAINER_FRAME_UP_BOX,
    Text:E_CONTAINER_FRAME_UP_ARROW,
    Text:E_CONTAINER_FRAME_DOWN_BOX,
    Text:E_CONTAINER_FRAME_DOWN_ARROW,

    // Where the thumb lives, worked out once when the frame is laid out. The
    // page it is showing changes; the rail it slides on does not.
    Float:E_CONTAINER_FRAME_THUMB_X,
    Float:E_CONTAINER_FRAME_THUMB_Y,
    Float:E_CONTAINER_FRAME_THUMB_SPAN,

    E_CONTAINER_FRAME_TITLE[MAX_CONTAINER_MENU_TITLE char]
};

static
    gContainerFrameData[MAX_CONTAINER_MENUS][E_CONTAINER_FRAME_DATA]
;

// The three pieces that read differently on each screen. The name is the
// container's, the counter is of what that player is looking at, and the thumb
// sits on their page -- so none of the three can be shared.
//
// Folded into one row per player for the same reason the slots are.
static enum _:E_PLAYER_FRAME_DATA {
    E_PLAYER_FRAME_TITLE,
    E_PLAYER_FRAME_COUNT,
    E_PLAYER_FRAME_THUMB,

    // What the three are reading right now, so a redraw sends only the ones
    // that actually changed. The frame is updated every time an item lands in
    // the container, and the name in particular is the same name every time.
    Container:E_PLAYER_FRAME_DRAWN_CONTAINER,
    E_PLAYER_FRAME_DRAWN_ON_PAGE,
    E_PLAYER_FRAME_DRAWN_TOTAL,
    E_PLAYER_FRAME_DRAWN_PAGE,
    E_PLAYER_FRAME_DRAWN_PAGES
};

#define CONTAINER_FRAME_BASE(%0) (_:(%0) * E_PLAYER_FRAME_DATA)

static
    gPlayerFrameData[MAX_PLAYERS][_:MAX_CONTAINER_MENUS * E_PLAYER_FRAME_DATA],
    bool:gPlayerFrameBuilt[MAX_PLAYERS][MAX_CONTAINER_MENUS]
;

/**
 * # Functions
 */

/**
 * @brief      Lay out a grid of slots on the screen.
 *
 * A menu is a shape, not a container: it is built once at start-up and then
 * pointed at whatever needs showing. One window can serve an inventory, a
 * locker and the inside of a crate, one after another.
 *
 * The amount is the small figure in the corner of each slot -- a count, a
 * countdown, litres. Its four corners are named; `CONTAINER_AMOUNT_CUSTOM`
 * takes the margins as a position of their own instead.
 *
 * @param      startX           Screen X of the first slot.
 * @param      startY           Screen Y of the first slot.
 * @param      boxSizeX         Width of one slot.
 * @param      boxSizeY         Height of one slot.
 * @param      rows             How many rows.
 * @param      columns          How many columns.
 * @param      colour           Slot colour.
 * @param      padding          Gap between slots, and the border around them.
 * @param      amountPos        Which corner the amount sits in.
 * @param      amountMarginX    How far in from that corner, across.
 * @param      amountMarginY    How far in from that corner, down.
 * @param      amountLetterX    Amount letter width.
 * @param      amountLetterY    Amount letter height.
 * @param      amountAlignment  Which way the amount text runs.
 * @param      frame            Whether to draw a panel, a name, a counter
 *                              and a scroll bar around it. It is built here
 *                              rather than added afterwards because a
 *                              textdraw made earlier is drawn behind one
 *                              made later, and the panel has to be behind
 *                              the slots.
 * @param      title            Name to draw for a container that has none
 *                              of its own. Ignored without `frame`. The
 *                              title bar is drawn in capitals either way,
 *                              so a row of menus is set the same however
 *                              the things in them are named.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the new menu
 *             - `INVALID_CONTAINER_MENU_ID` when:
 *                 + MAX_CONTAINER_MENUS are already built
 *                 + `rows` or `columns` is below one
 *                 + the two come to more than MAX_CONTAINER_MENU_SLOTS,
 *                   which is one player textdraw per slot and cannot be
 *                   drawn
 */
forward ContainerMenu:BuildContainerMenu(Float:startX, Float:startY, Float:boxSizeX, Float:boxSizeY, rows, columns, colour, Float:padding = 1.0, E_CONTAINER_AMOUNT_POSITION:amountPos = CONTAINER_AMOUNT_BOTTOM_RIGHT, Float:amountMarginX = 2.0, Float:amountMarginY = 8.0, Float:amountLetterX = 0.110, Float:amountLetterY = 0.700, TEXT_DRAW_ALIGN:amountAlignment = TEXT_DRAW_ALIGN_RIGHT, bool:frame = false, const title[] = "");

/**
 * @brief      Point a menu at a container and put it on a screen.
 *
 * The same call opens a menu and moves it to something else, so a character
 * opening a bag that was lying in a crate does not need the window closed
 * first. OnPlayerChangeContainerMenu says which of the two happened.
 *
 * The textdraws are made the first time a character sees a menu and kept
 * for as long as they are connected, so reopening costs content and not
 * creation.
 *
 * @param      playerid     Character to show it to.
 * @param      containerid  Container to show.
 * @param      menuid       Menu to show it in.
 * @param      page         Which page to open on.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is on their screen
 *             - `false` when:
 *                 + `playerid` is not connected
 *                 + `containerid` is not a live container
 *                 + `menuid` is not a built menu
 *                 + `page` is past the end of the container
 */
forward bool:ShowContainerMenu(playerid, Container:containerid, ContainerMenu:menuid, page = 0);

/**
 * @brief      Draw a menu again.
 *
 * Only the slots that actually differ send anything, so a slot already
 * showing the right item with the right text costs one comparison. That is
 * what makes it safe to call this whenever anything might have changed.
 *
 * "Differ" is not only a different item. An amount can change without the
 * item changing -- money poured from one stack into another, pills taken
 * out of a bottle -- and leaving the figure that was true a moment ago is
 * worse than redrawing something that did not move, because the number a
 * character is reading stops being the number they have.
 *
 * So the test is the amount's revision, which moves exactly when the text
 * would and costs a fraction of producing it. A countdown of two days reads
 * the same `02D 00H` for an hour, which is 3599 formats a slot that never
 * happen.
 *
 * @param      playerid  Character whose screen to redraw.
 * @param      menuid    Menu to redraw.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the screen is up to date
 *             - `false` when:
 *                 + `playerid` is not connected
 *                 + they do not have that menu open
 */
forward bool:RefreshContainerMenu(playerid, ContainerMenu:menuid);

/**
 * @brief      Redraw a container for everybody looking at it.
 *
 * For a change that moved nothing: a stack spent, a box topped up, litres
 * drawn out of a can. Items arriving and leaving are already redrawn on
 * their own, and a number written over an item that stays where it is is
 * not.
 *
 * Every menu showing that container is redrawn, for whoever has it open.
 *
 * @param      containerid  Container whose contents changed.
 *
 * @date       18:20 15/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` whoever had it open has been sent it again
 *             - `false` `containerid` is not a live container
 */
forward bool:RefreshContainerViewers(Container:containerid);

/**
 * @brief      Take a menu off a character's screen.
 *
 * The textdraws are kept and only what they were showing is forgotten, so
 * the next open pays for content and not for creation.
 *
 * @param      playerid  Character to hide it from.
 * @param      menuid    Menu to hide.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is gone
 *             - `false` when:
 *                 + `playerid` is not connected
 *                 + they did not have it open
 */
forward bool:HideContainerMenu(playerid, ContainerMenu:menuid);

/**
 * @brief      Whether a menu has been built.
 *
 * @param      menuid  Menu to check.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it was built
 *             - `false` `menuid` is outside what has been built
 */
forward bool:IsValidContainerMenu(ContainerMenu:menuid);

/**
 * @brief      Whether a character has a menu open.
 *
 * @param      playerid  Player to ask about.
 * @param      menuid    Menu to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is on their screen
 *             - `false` when:
 *                 + they are not connected
 *                 + `menuid` is not a built menu
 *                 + they do not have it open
 */
forward bool:IsPlayerUsingContainerMenu(playerid, ContainerMenu:menuid);

/**
 * @brief      How many slots one page of a menu has.
 *
 * Rows times columns: the shape of the grid, not how much the container
 * holds.
 *
 * @param      menuid  Menu to measure.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many slots are on a page
 *             - `0` `menuid` is not a built menu
 */
forward GetContainerMenuSlots(ContainerMenu:menuid);

/**
 * @brief      Paint one slot's box a different colour, for one character.
 *
 * What a selection is made of: an item picked up to be moved somewhere has
 * to look picked up. Painting it back is the same call with the menu's own
 * colour, which GetContainerMenuColour answers.
 *
 * The boxes are global textdraws shared by every viewer, but a definition
 * only reaches a client when the textdraw is shown to them -- so whoever is
 * drawn next overwrites the definition without disturbing anybody else's
 * screen, and this character keeps what the show carried to them.
 *
 * @param      playerid  Character to paint it for.
 * @param      menuid    Menu the slot belongs to.
 * @param      slot      Slot to paint.
 * @param      colour    Colour to paint it.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the slot is that colour on their screen
 *             - `false` when:
 *                 + they do not have that menu open
 *                 + `slot` is outside the grid
 */
forward bool:SetContainerMenuSlotColour(playerid, ContainerMenu:menuid, slot, colour);

/**
 * @brief      A menu's own slot colour.
 *
 * What SetContainerMenuSlotColour paints a slot back to.
 *
 * @param      menuid  Menu to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the colour it was built with
 *             - `0` `menuid` is not a built menu
 */
forward GetContainerMenuColour(ContainerMenu:menuid);

/**
 * @brief      What the box behind a slot is currently painted.
 *
 * Which is not always what is on the screen: a menu may paint a slot some
 * other colour for a moment, and this is what it goes back to. Anything
 * doing that wants this rather than GetContainerMenuColour, or the slot
 * comes back the wrong colour when the moment passes.
 *
 * @param      playerid  Player whose screen it is.
 * @param      menuid    Menu the slot is in.
 * @param      slot      Square on the current page.
 *
 * @date       16:40 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - the colour the slot belongs at
 *             - the menu's own colour when nothing has said otherwise
 */
forward GetContainerMenuSlotColour(playerid, ContainerMenu:menuid, slot);

/**
 * @brief      How many slots across a menu is.
 *
 * The width of the grid, which is also the step from a slot to the one
 * directly beneath it. Anything drawn over the grid needs it to say which
 * slots it is covering without measuring them.
 *
 * @param      menuid  Menu to measure.
 *
 * @date       12:40 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many columns it was built with
 *             - `0` `menuid` is not a built menu
 */
forward GetContainerMenuColumns(ContainerMenu:menuid);

/**
 * @brief      Take one slot's number off the screen, or put it back.
 *
 * For something drawn over the grid that has to be read through. A slot's
 * number is a player textdraw and so is whatever is covering it, and which
 * of two player textdraws ends up on top is the client's business rather
 * than anything this side can state -- so the one underneath is put away
 * instead of being argued with.
 *
 * What is asked for here outlives a redraw. A slot that fills or counts
 * down while it is put away stays put away, and comes back when this is
 * called again -- otherwise anything happening under the cover would send
 * the number again and put it straight back on top.
 *
 * Putting it back is refused for a slot with nothing in it, because an
 * empty slot has no number to show. It stops being covered either way.
 *
 * @param      playerid  Character whose screen it is.
 * @param      menuid    Menu the slot belongs to.
 * @param      slot      Slot to hide or show the number of.
 * @param      shown     Whether it should be on screen.
 *
 * @date       11:20 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the number is in the state asked for
 *             - `false` when:
 *                 + they do not have that menu open
 *                 + `slot` is outside the grid
 *                 + it was asked to be shown and the slot is empty
 */
forward bool:SetContainerMenuSlotAmountShown(playerid, ContainerMenu:menuid, slot, bool:shown);

/**
 * @brief      Where a slot sits on the screen.
 *
 * So something drawn against a slot -- a context menu, a tooltip, a
 * highlight -- can be placed without repeating the grid arithmetic or being
 * told the layout a second time.
 *
 * @param      menuid  Menu the slot belongs to.
 * @param      slot    Slot to locate.
 * @param      x       Reads back screen X of its top left.
 * @param      y       Reads back screen Y of its top left.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the outputs were written
 *             - `false` when:
 *                 + `menuid` is not a built menu
 *                 + `slot` is outside the grid
 */
forward bool:GetContainerMenuSlotPos(ContainerMenu:menuid, slot, &Float:x, &Float:y);

/**
 * @brief      How big a menu's slots are.
 *
 * The other half of what something drawn against a slot needs.
 *
 * @param      menuid    Menu to measure.
 * @param      boxSizeX  Reads back the slot width.
 * @param      boxSizeY  Reads back the slot height.
 * @param      padding   Reads back the gap between slots.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the outputs were written
 *             - `false` `menuid` is not a built menu
 */
forward bool:GetContainerMenuSize(ContainerMenu:menuid, &Float:boxSizeX, &Float:boxSizeY, &Float:padding);

/**
 * @brief      Which page a character is looking at.
 *
 * @param      playerid  Player to ask about.
 * @param      menuid    Menu to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the page they are on, counting from zero
 *             - `0` they do not have that menu open
 */
forward GetContainerMenuPage(playerid, ContainerMenu:menuid);

/**
 * @brief      How many pages the open container comes to.
 *
 * The container's capacity over the menu's slots, rounded up, so it depends
 * on both and is never below one.
 *
 * @param      playerid  Player to ask about.
 * @param      menuid    Menu to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many pages there are
 *             - `0` they do not have that menu open
 */
forward GetContainerMenuPageCount(playerid, ContainerMenu:menuid);

/**
 * @brief      Turn to another page.
 *
 * Every slot on the screen now shows something else, and
 * OnPlayerChangeContainerMenuPage says so -- which is what anything the
 * gamemode had drawn on top of a slot listens for.
 *
 * @param      playerid  Character to turn the page for.
 * @param      menuid    Menu to turn.
 * @param      page      Page to turn to, counting from zero.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they are looking at that page
 *             - `false` when:
 *                 + they do not have that menu open
 *                 + `page` is past the end of the container
 */
forward bool:SetContainerMenuPage(playerid, ContainerMenu:menuid, page);

/**
 * @brief      Which container a menu is showing.
 *
 * @param      playerid  Player to ask about.
 * @param      menuid    Menu to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the container on their screen
 *             - `INVALID_CONTAINER_ID` they do not have that menu open
 */
forward Container:GetContainerMenuContainer(playerid, ContainerMenu:menuid);

/**
 * @brief      What is drawn in one slot of a menu.
 *
 * The slot is a square on the screen; which item is in it depends on the
 * page as well, which is why this takes the character and not just the
 * menu.
 *
 * @param      playerid  Player whose screen to read.
 * @param      menuid    Menu the slot belongs to.
 * @param      slot      Slot to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the item drawn there
 *             - `INVALID_ITEM_ID` when:
 *                 + they do not have that menu open
 *                 + `slot` is outside the grid
 *                 + nothing is drawn there
 */
forward Item:GetContainerMenuSlotItem(playerid, ContainerMenu:menuid, slot);

/**
 * @brief      Which slot of the container a slot of the screen is showing.
 *
 * The page turned back into a place in the container, for anything that has
 * to talk to the container rather than to the screen.
 *
 * @param      playerid  Player whose screen to read.
 * @param      menuid    Menu the slot belongs to.
 * @param      slot      Slot on the screen.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the index into the container
 *             - `-1` when:
 *                 + they do not have that menu open
 *                 + `slot` is outside the grid
 */
forward GetContainerMenuSlotIndex(playerid, ContainerMenu:menuid, slot);

/**
 * # Events
 */

/**
 * @brief      A character clicked a slot.
 *
 * Fired for an empty slot too, because dropping something into one is a
 * click on nothing. What it means is the gamemode's: open an action menu,
 * use the item outright, start a drag.
 *
 * @param      playerid  Character who clicked.
 * @param      menuid    Menu they clicked in.
 * @param      slot      Slot on the screen they clicked.
 * @param      index     Which slot of the container that is.
 * @param      itemid    What was drawn there, or `INVALID_ITEM_ID`.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerClickContainerMenuSlot(playerid, ContainerMenu:menuid, slot, index, Item:itemid);

/**
 * @brief      A character pressed escape with this menu open.
 *
 * @param      playerid  Character who pressed it.
 * @param      menuid    Menu that is open.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` the menu closes, which is what escape normally means
 *             - `1` the menu stays open and the press is consumed, which is
 *               what something drawn over the menu wants: the first escape
 *               dismisses that, and the second closes the menu underneath
 */
forward OnPlayerCancelContainerMenu(playerid, ContainerMenu:menuid);

/**
 * @brief      A player's first menu went up, or their last one came down.
 *
 * Not one menu: the answer to "is this player looking at a menu at all",
 * which is the question anything else drawn on the screen has to ask before
 * drawing. Opening a crate beside the pockets is not a change and does not
 * fire; closing it while the pockets stay open is not either.
 *
 * It is sent from the one place that knows, so every way out is covered --
 * hidden, escaped, or disconnected in the middle of it.
 *
 * @param      playerid  Player whose screen it is.
 * @param      using     `true` a menu is up, `false` the screen is free.
 */
forward OnPlayerUseContainerMenu(playerid, bool:using);

/**
 * @brief      The page turned.
 *
 * Every slot on the screen now shows something else. What this is for is
 * anything the gamemode had drawn on top of a slot: a highlight, a marker,
 * a colour -- none of which the redraw knows about, all of which are now on
 * the wrong square.
 *
 * @param      playerid  Character whose page turned.
 * @param      menuid    Menu that turned.
 * @param      page      Page they are on now.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerChangeContainerMenuPage(playerid, ContainerMenu:menuid, page);

/**
 * @brief      The menu is now showing something else.
 *
 * Opened for the first time, or the same window pointed at a different
 * container. Turning a page is not this -- the container is the same one,
 * and OnPlayerChangeContainerMenuPage says so.
 *
 * It exists because one window is shared by everything with an inside, so
 * "which container is on screen" changes without anything else about the
 * menu changing, and whatever was drawn against the old contents has to
 * hear about it.
 *
 * Fired after the page is drawn, so anything answering it is looking at
 * what is actually on the screen.
 *
 * @param      playerid     Character whose menu changed.
 * @param      menuid       Menu that changed.
 * @param      containerid  What it shows now.
 * @param      previous     What it showed before, or `INVALID_CONTAINER_ID`
 *                          when the window was shut.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerChangeContainerMenu(playerid, ContainerMenu:menuid, Container:containerid, Container:previous);

/**
 * @brief      Whether a menu was given a frame.
 *
 * @param      menuid  Menu to read.
 *
 * @date       14:10 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it has one
 *             - `false` it does not, or is not a built menu
 */
forward bool:HasContainerMenuFrame(ContainerMenu:menuid);

/**
 * @brief      Turn one page, the way the scroll bar does.
 *
 * A page either side of the container is not an error, it is the end of the
 * list -- so this says whether the page actually moved and nothing has to
 * work out first whether it could.
 *
 * @param      playerid  Character turning it.
 * @param      menuid    Menu to turn.
 * @param      step      How many pages, negative to go back.
 *
 * @date       14:10 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the page changed
 *             - `false` when:
 *                 + that menu is not open for them
 *                 + they are already at that end
 */
forward bool:ScrollContainerMenu(playerid, ContainerMenu:menuid, step);

/**
 * # Internal
 */

static stock bool:HasAnyMenuOpenInternal(playerid) {
    for (new ContainerMenu:menuid; menuid != MAX_CONTAINER_MENUS; ++menuid) {
        if (gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_CONTAINER] != INVALID_CONTAINER_ID) {
            return true;
        }
    }

    return false;
}

static stock AddMenuViewerInternal(playerid) {
    if (gMenuViewerSlot[playerid] != -1) {
        return;
    }

    gMenuViewerSlot[playerid] = gMenuViewerCount;
    gMenuViewer[gMenuViewerCount++] = playerid;

    // The edge and not the opening. Whether this player is looking at a menu
    // is exactly what being a viewer means, so the two cannot drift apart.
    CallLocalFunction("OnPlayerUseContainerMenu", "ii", playerid, true);
}

static stock RemoveMenuViewerInternal(playerid) {
    new const
        slot = gMenuViewerSlot[playerid]
    ;

    if (slot == -1) {
        return;
    }

    new const
        last = gMenuViewer[--gMenuViewerCount]
    ;

    gMenuViewer[slot] = last;
    gMenuViewerSlot[last] = slot;
    gMenuViewerSlot[playerid] = -1;

    CallLocalFunction("OnPlayerUseContainerMenu", "ii", playerid, false);
}

// Every viewer of this container, redrawn. Only the slots that actually differ
// send anything, so a container nobody is looking at costs one comparison.
static stock RefreshContainerViewersInternal(Container:containerid) {
    for (new i; i != gMenuViewerCount; ++i) {
        new const
            viewer = gMenuViewer[i]
        ;

        for (new ContainerMenu:menuid; menuid != gContainerMenuCount; ++menuid) {
            if (gPlayerMenuData[viewer][menuid][E_PLAYER_MENU_CONTAINER] == containerid) {
                DrawPageInternal(viewer, menuid);
                UpdateFrameInternal(viewer, menuid);
            }
        }
    }
}

static stock bool:IsValidContainerMenuInternal(ContainerMenu:menuid) {
    return (0 <= _:menuid < _:gContainerMenuCount);
}

// Not a guard clause, so it is a question with a name rather than an or.
static stock bool:AmountNeedsRedrawInternal(revision, cached) {
    if (revision == ITEM_AMOUNT_REVISION_UNKNOWN) {
        return true;
    }

    return revision != cached;
}

// These take the slot base rather than the menu and slot so a caller touching
// the same slot more than once works out where it is only the first time. That
// is what makes the folded index pay: the arithmetic is cheap, but a redraw
// would otherwise repeat it for every field it reads.
static stock Item:GetDrawnItemInternal(playerid, base) {
    new const
        value = gPlayerSlotData[playerid][base + E_PLAYER_SLOT_ITEM]
    ;

    return value ? (Item:(value - 1)) : INVALID_ITEM_ID;
}

static stock SetDrawnItemInternal(playerid, base, Item:itemid) {
    gPlayerSlotData[playerid][base + E_PLAYER_SLOT_ITEM] = _:itemid + 1;
}

// A row of one of the masks above, one player's. Passing the row rather than
// the whole array is what lets the three share this: the cell arithmetic is the
// same for all of them and only the array differs.
static stock bool:GetSlotBitInternal(const mask[], ContainerMenu:menuid, slot) {
    new const
        cell = _:menuid * CONTAINER_MENU_MASK_CELLS + (slot >>> 5)
    ;

    return (mask[cell] & (1 << (slot & 31))) != 0;
}

static stock SetSlotBitInternal(mask[], ContainerMenu:menuid, slot, bool:set) {
    new const
        cell = _:menuid * CONTAINER_MENU_MASK_CELLS + (slot >>> 5)
    ;

    if (set) {
        mask[cell] |= (1 << (slot & 31));

        return;
    }

    mask[cell] &= ~(1 << (slot & 31));
}

static stock ResetPlayerMenuCacheInternal(playerid, ContainerMenu:menuid) {
    for (new i; i != MAX_CONTAINER_MENU_SLOTS; ++i) {
        new const
            base = CONTAINER_MENU_SLOT_BASE(menuid, i)
        ;

        gPlayerSlotData[playerid][base + E_PLAYER_SLOT_ITEM] = 0;
        gPlayerSlotData[playerid][base + E_PLAYER_SLOT_MODEL] = 0;
        gPlayerSlotData[playerid][base + E_PLAYER_SLOT_AMOUNT_REVISION] = 0;
        gPlayerSlotData[playerid][base + E_PLAYER_SLOT_COLOUR] = 0;
    }

    for (new i; i != CONTAINER_MENU_MASK_CELLS; ++i) {
        new const
            cell = _:menuid * CONTAINER_MENU_MASK_CELLS + i
        ;

        gPlayerSlotTimedMask[playerid][cell] = 0;
        gPlayerSlotShownMask[playerid][cell] = 0;
        gPlayerSlotCoveredMask[playerid][cell] = 0;
    }
}

// Made once per player per menu and then kept for as long as they are
// connected. Everything after this is a set or a show on a textdraw that
// already exists.
static stock BuildPlayerMenuInternal(playerid, ContainerMenu:menuid) {
    if (gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_BUILT]) {
        return;
    }

    new const
        Float:startX        = gContainerMenuData[menuid][E_CONTAINER_MENU_START_X],
        Float:startY        = gContainerMenuData[menuid][E_CONTAINER_MENU_START_Y],
        Float:spacedXBase   = gContainerMenuData[menuid][E_CONTAINER_MENU_BOX_SIZE_X],
        Float:spacedYBase   = gContainerMenuData[menuid][E_CONTAINER_MENU_BOX_SIZE_Y],
        Float:padding       = gContainerMenuData[menuid][E_CONTAINER_MENU_PADDING],
        columns             = gContainerMenuData[menuid][E_CONTAINER_MENU_COLUMNS],
        slots               = gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS],
        Float:amountOffsetX = gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_OFFSET_X],
        Float:amountOffsetY = gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_OFFSET_Y],
        Float:amountLetterX = gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_LETTER_SIZE_X],
        Float:amountLetterY = gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_LETTER_SIZE_Y],
        TEXT_DRAW_ALIGN:amountAlignment = gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_ALIGNMENT]
    ;

    new const
        Float:spacedX = spacedXBase + padding,
        Float:spacedY = spacedYBase + padding
    ;

    for (new i, PlayerText:pt; i != slots; ++i) {
        new const
            Float:baseX = startX + (i % columns) * spacedX,
            Float:baseY = startY + (i / columns) * spacedY
        ;

        pt = CreatePlayerTextDraw(playerid,
            baseX + amountOffsetX,
            baseY + amountOffsetY, "_"
        );
        PlayerTextDrawLetterSize(playerid, pt, amountLetterX, amountLetterY);
        PlayerTextDrawAlignment(playerid, pt, amountAlignment);
        PlayerTextDrawSetShadow(playerid, pt, 0);
        PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_BANK_GOTHIC);
        PlayerTextDrawSetProportional(playerid, pt, true);
        gPlayerSlotData[playerid][CONTAINER_MENU_SLOT_BASE(menuid, i) + E_PLAYER_SLOT_AMOUNT] = _:pt;
    }

    gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_BUILT] = true;
}

// The whole point of the file. A slot that is already showing the right item
// with the right text sends nothing at all, so turning a page costs only the
// slots that actually differ.
// The colour follows what is standing in the square, so it is read only when
// that changes -- which also means an empty menu, a page of the same items and
// a refresh that moved nothing all cost nothing at all.
//
// Read and not asked for. A colour belongs to the build, so there is nothing a
// callback per square could answer that an array cell does not, and thirty
// calls out into the script to be told "no colour" thirty times is thirty calls
// to learn nothing.
static stock ColourSlotInternal(playerid, ContainerMenu:menuid, slot, Item:itemid, Item:drawn) {
    if (itemid == drawn) {
        return 0;
    }

    new const
        base = CONTAINER_MENU_SLOT_BASE(menuid, slot)
    ;

    new
        colour = 0
    ;

    if (itemid != INVALID_ITEM_ID) {
        colour = GetItemBuildSlotColour(GetItemBuild(itemid));
    }

    if (colour == 0) {
        colour = gContainerMenuData[menuid][E_CONTAINER_MENU_COLOUR];
    }

    if (colour == gPlayerSlotData[playerid][base + E_PLAYER_SLOT_COLOUR]) {
        return 0;
    }

    gPlayerSlotData[playerid][base + E_PLAYER_SLOT_COLOUR] = colour;
    
    TextDrawColour(gContainerMenuBox[menuid][slot], colour);
    TextDrawShowForPlayer(playerid, gContainerMenuBox[menuid][slot]);

    return 1;
}

static stock DrawSlotInternal(playerid, ContainerMenu:menuid, slot, Item:itemid) {
    new const
        base = CONTAINER_MENU_SLOT_BASE(menuid, slot),
        Item:drawn = GetDrawnItemInternal(playerid, base)
    ;

    if (itemid == INVALID_ITEM_ID) {
        if (drawn == INVALID_ITEM_ID) {
            return 0;
        }

        TextDrawHideForPlayer(playerid, gContainerMenuModel[menuid][slot]);
        PlayerTextDrawHide(playerid, PlayerText:gPlayerSlotData[playerid][base + E_PLAYER_SLOT_AMOUNT]);

        SetDrawnItemInternal(playerid, base, INVALID_ITEM_ID);
        SetSlotBitInternal(gPlayerSlotTimedMask[playerid], menuid, slot, false);
        SetSlotBitInternal(gPlayerSlotShownMask[playerid], menuid, slot, false);

        gPlayerSlotData[playerid][base + E_PLAYER_SLOT_MODEL] = 0;
        gPlayerSlotData[playerid][base + E_PLAYER_SLOT_AMOUNT_REVISION] = 0;

        return 1 + ColourSlotInternal(playerid, menuid, slot, itemid, drawn);
    }

    new
        sent = ColourSlotInternal(playerid, menuid, slot, itemid, drawn)
    ;

    new const
        model = GetItemPreviewModel(itemid)
    ;

    if (model != gPlayerSlotData[playerid][base + E_PLAYER_SLOT_MODEL]) {
        new const
            ItemBuild:buildid = GetItemBuild(itemid),
            Text:textid = gContainerMenuModel[menuid][slot]
        ;

        new
            Float:sizeX,
            Float:sizeY,
            Float:rotationX,
            Float:rotationY,
            Float:rotationZ,
            Float:zoom,
            Float:offsetX,
            Float:offsetY
        ;

        GetItemBuildPreviewSettings(buildid, sizeX, sizeY, rotationX, rotationY, rotationZ, zoom, offsetX, offsetY);

        new const
            Float:fBase = gContainerMenuData[menuid][E_CONTAINER_MENU_BOX_SIZE_X]
        ;

        if (sizeX <= 0.0 || sizeY <= 0.0) {
            sizeX = fBase;
            sizeY = fBase;
        }

        if (zoom <= 0.0) {
            zoom = 1.0;
        }

        new
            Float:slotX,
            Float:slotY
        ;

        GetContainerMenuSlotPos(menuid, slot, slotX, slotY);

        TextDrawSetPos(
            textid,
            slotX + (fBase - sizeX) / 2.0 + offsetX,
            slotY + (fBase - sizeY) / 2.0 + offsetY
        );
        TextDrawTextSize(textid, sizeX, sizeY);
        TextDrawSetPreviewRot(textid, rotationX, rotationY, rotationZ, zoom);
        TextDrawSetPreviewModel(textid, model);
        TextDrawShowForPlayer(playerid, textid);

        gPlayerSlotData[playerid][base + E_PLAYER_SLOT_MODEL] = model;

        ++sent;
    }

    new const
        bool:timed = GetItemBuildAmountType(GetItemBuild(itemid)) == ITEM_AMOUNT_DURATION
    ;

    SetSlotBitInternal(gPlayerSlotTimedMask[playerid], menuid, slot, timed);

    new const
        revision = GetItemAmountRevision(itemid)
    ;

    // A different item in the square answers for itself whatever the revisions
    // say. Two items can hold the same one -- something with no amount at all
    // answers nothing, and so does an empty one -- and the square would be left
    // showing whatever the item before it wrote there. A bat carrying a rifle's
    // rounds is that, and it follows the bat around because the text is the
    // square's and nothing had told it to change.
    if (drawn != itemid || AmountNeedsRedrawInternal(revision, gPlayerSlotData[playerid][base + E_PLAYER_SLOT_AMOUNT_REVISION])) {
        new
            text[MAX_ITEM_AMOUNT_LENGTH]
        ;

        if (!FormatItemAmount(itemid, text)) {
            text[0] = EOS;
        }

        PlayerTextDrawSetString(playerid, PlayerText:gPlayerSlotData[playerid][base + E_PLAYER_SLOT_AMOUNT], text[0] ? text : (" "));

        gPlayerSlotData[playerid][base + E_PLAYER_SLOT_AMOUNT_REVISION] = revision;

        ++sent;
    }

    // Sent once, when the slot fills, and then left alone. Setting the string
    // of a textdraw that is already up changes it where it stands, while
    // showing one again sends it to the client afresh and it comes back on top
    // of everything drawn since -- which is how a number in the corner of a
    // slot ended up over a menu opened against it.
    //
    // It stands outside the block above rather than inside it, because a slot
    // can fill with something that has no number to draw and the block would
    // then never run. The number still has to reach the client, blank, so that
    // whatever lands in the slot next only has to set its text.
    if (!GetSlotBitInternal(gPlayerSlotShownMask[playerid], menuid, slot) && !GetSlotBitInternal(gPlayerSlotCoveredMask[playerid], menuid, slot)) {
        PlayerTextDrawShow(playerid, PlayerText:gPlayerSlotData[playerid][base + E_PLAYER_SLOT_AMOUNT]);
        SetSlotBitInternal(gPlayerSlotShownMask[playerid], menuid, slot, true);

        ++sent;
    }

    SetDrawnItemInternal(playerid, base, itemid);

    return sent;
}

static stock DrawPageInternal(playerid, ContainerMenu:menuid) {
    new const
        Container:containerid = gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_CONTAINER],
        slots = gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS],
        first = gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_PAGE] * slots
    ;

    new
        sent = 0
    ;

    for (new i; i != slots; ++i) {
        sent += DrawSlotInternal(playerid, menuid, i, GetContainerSlotItem(containerid, first + i));
    }

    return sent;
}


// Where the frame sits, from the grid it is wrapped around. Every piece of it
// is measured off these four, so a menu built anywhere on the screen with any
// number of columns is framed correctly without being told anything.
static stock bool:FrameBoundsInternal(ContainerMenu:menuid, &Float:gridX, &Float:gridY, &Float:gridSizeX, &Float:gridSizeY) {
    new const
        columns = gContainerMenuData[menuid][E_CONTAINER_MENU_COLUMNS],
        rows = gContainerMenuData[menuid][E_CONTAINER_MENU_ROWS],
        Float:padding = gContainerMenuData[menuid][E_CONTAINER_MENU_PADDING]
    ;

    if (columns == 0 || rows == 0) {
        return false;
    }

    gridX = gContainerMenuData[menuid][E_CONTAINER_MENU_START_X];
    gridY = gContainerMenuData[menuid][E_CONTAINER_MENU_START_Y];

    // The last column and row carry no padding after them, which is what makes
    // the panel sit tight against the grid rather than a pixel off it.
    gridSizeX = float(columns) * (gContainerMenuData[menuid][E_CONTAINER_MENU_BOX_SIZE_X] + padding) - padding;
    gridSizeY = float(rows) * (gContainerMenuData[menuid][E_CONTAINER_MENU_BOX_SIZE_Y] + padding) - padding;

    return true;
}

static stock Text:FramePanelInternal(Float:x, Float:y, Float:sizeX, Float:sizeY, colour) {
    new const
        Text:textid = TextDrawCreate(x, y, "LD_DUAL:white")
    ;

    TextDrawTextSize(textid, sizeX, sizeY);
    TextDrawColour(textid, colour);
    TextDrawFont(textid, TEXT_DRAW_FONT_SPRITE);
    TextDrawSetShadow(textid, 0);
    TextDrawSetOutline(textid, 0);
    TextDrawBackgroundColour(textid, 255);

    return textid;
}

static stock Text:FrameSpriteInternal(Float:x, Float:y, const sprite[], Float:sizeX, Float:sizeY, colour, bool:selectable) {
    new const
        Text:textid = TextDrawCreate(x, y, sprite)
    ;

    TextDrawTextSize(textid, sizeX, sizeY);
    TextDrawColour(textid, colour);
    TextDrawFont(textid, TEXT_DRAW_FONT_SPRITE);
    TextDrawSetShadow(textid, 0);
    TextDrawSetOutline(textid, 0);
    TextDrawBackgroundColour(textid, 255);
    TextDrawSetSelectable(textid, selectable);

    return textid;
}

// One player's three pieces, made the first time they open the menu and kept
// after that, the same way their slot numbers are.
static stock BuildPlayerFrameInternal(playerid, ContainerMenu:menuid) {
    if (gPlayerFrameBuilt[playerid][menuid]) {
        return;
    }

    new
        Float:gridX,
        Float:gridY,
        Float:gridSizeX,
        Float:gridSizeY
    ;

    if (!FrameBoundsInternal(menuid, gridX, gridY, gridSizeX, gridSizeY)) {
        return;
    }

    new const
        base = CONTAINER_FRAME_BASE(menuid),
        Float:outerX = gridX - CONTAINER_FRAME_BORDER - CONTAINER_FRAME_OUTER_MARGIN,
        Float:outerY = gridY - CONTAINER_FRAME_BORDER - CONTAINER_FRAME_TITLE_HEIGHT,
        Float:outerRight = gridX + gridSizeX + CONTAINER_FRAME_BORDER + CONTAINER_FRAME_BAR_WIDTH
                         + CONTAINER_FRAME_BORDER + CONTAINER_FRAME_OUTER_MARGIN
    ;

    new
        PlayerText:pt
    ;

    pt = CreatePlayerTextDraw(playerid, outerX + 1.5, outerY + 2.0, "_");
    PlayerTextDrawLetterSize(playerid, pt, 0.128, 0.699);
    PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, pt, CONTAINER_FRAME_TEXT_COLOUR);
    PlayerTextDrawSetShadow(playerid, pt, 0);
    PlayerTextDrawSetOutline(playerid, pt, 0);
    PlayerTextDrawBackgroundColour(playerid, pt, 150);
    PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, pt, true);
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_TITLE] = _:pt;

    pt = CreatePlayerTextDraw(playerid, outerRight - CONTAINER_FRAME_OUTER_MARGIN, outerY + 2.0, "_");
    PlayerTextDrawLetterSize(playerid, pt, 0.128, 0.699);
    PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, pt, CONTAINER_FRAME_TEXT_COLOUR);
    PlayerTextDrawSetShadow(playerid, pt, 0);
    PlayerTextDrawSetOutline(playerid, pt, 0);
    PlayerTextDrawBackgroundColour(playerid, pt, 150);
    PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, pt, true);
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_COUNT] = _:pt;

    pt = CreatePlayerTextDraw(
        playerid,
        gContainerFrameData[menuid][E_CONTAINER_FRAME_THUMB_X],
        gContainerFrameData[menuid][E_CONTAINER_FRAME_THUMB_Y],
        "LD_DUAL:white"
    );
    PlayerTextDrawTextSize(playerid, pt, CONTAINER_FRAME_BAR_WIDTH - 2.0, gContainerFrameData[menuid][E_CONTAINER_FRAME_THUMB_SPAN]);
    PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, pt, CONTAINER_FRAME_INNER_COLOUR);
    PlayerTextDrawSetShadow(playerid, pt, 0);
    PlayerTextDrawSetOutline(playerid, pt, 0);
    PlayerTextDrawBackgroundColour(playerid, pt, 255);
    PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_SPRITE);
    PlayerTextDrawSetProportional(playerid, pt, true);
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_THUMB] = _:pt;

    // Nothing has been drawn into them yet, and a zeroed cell would read as
    // container zero already being on the screen -- which is a real container
    // and would cost that player the name of it.
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_CONTAINER] = _:INVALID_CONTAINER_ID;
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_ON_PAGE] = -1;
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_TOTAL] = -1;
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_PAGE] = -1;
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_PAGES] = -1;

    gPlayerFrameBuilt[playerid][menuid] = true;
}

// The name, the counter and where the thumb sits, which is everything about the
// frame that depends on what is actually on the screen. Called wherever the
// page or the contents change, and it is three sets and a position -- the
// panels around it are never touched again after they are shown.
static stock UpdateFrameInternal(playerid, ContainerMenu:menuid) {
    if (!gContainerFrameData[menuid][E_CONTAINER_FRAME_BUILT]) {
        return;
    }

    if (!gPlayerFrameBuilt[playerid][menuid]) {
        return;
    }

    new const
        base = CONTAINER_FRAME_BASE(menuid),
        Container:containerid = gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_CONTAINER],
        slots = gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS],
        page = gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_PAGE]
    ;

    // The name only when it is a different container. A menu is redrawn every
    // time anything lands in it and the name is the same name every time.
    if (gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_CONTAINER] != _:containerid) {
        new
            title[MAX_CONTAINER_MENU_TITLE]
        ;

        title[0] = EOS;

        GetContainerName(containerid, title);

        if (title[0] == EOS) {
            strunpack(title, gContainerFrameData[menuid][E_CONTAINER_FRAME_TITLE], MAX_CONTAINER_MENU_TITLE);
        }

        // In capitals whatever the container calls itself. A title bar is a
        // label and not a sentence, and two of them side by side only read as
        // one thing when they are set the same way -- "Storage" next to
        // "INVENTORY" reads as two programs.
        for (new i; title[i] != EOS; ++i) {
            title[i] = toupper(title[i]);
        }

        PlayerTextDrawSetString(playerid, PlayerText:gPlayerFrameData[playerid][base + E_PLAYER_FRAME_TITLE], title);

        gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_CONTAINER] = _:containerid;
    }

    // What is under the player's eyes against what the container holds. The
    // last page is rarely full, so it is counted rather than assumed.
    new
        onPage = 0
    ;

    for (new i; i != slots; ++i) {
        if (GetDrawnItemInternal(playerid, CONTAINER_MENU_SLOT_BASE(menuid, i)) != INVALID_ITEM_ID) {
            ++onPage;
        }
    }

    new const
        total = GetContainerSize(containerid)
    ;

    if (gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_ON_PAGE] != onPage
        || gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_TOTAL] != total) {
        new
            text[MAX_CONTAINER_MENU_TITLE]
        ;

        format(text, sizeof (text), CONTAINER_FRAME_COUNT_FORMAT, onPage, total);

        PlayerTextDrawSetString(playerid, PlayerText:gPlayerFrameData[playerid][base + E_PLAYER_FRAME_COUNT], text);

        gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_ON_PAGE] = onPage;
        gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_TOTAL] = total;
    }

    new
        pages = GetContainerMenuPageCount(playerid, menuid)
    ;

    if (pages < 1) {
        pages = 1;
    }

    // The thumb has to be sent again to move or resize it, so it is only
    // touched when it is actually somewhere else. Filling a crate one item at a
    // time does not move it at all.
    if (gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_PAGE] == page
        && gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_PAGES] == pages) {
        return;
    }

    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_PAGE] = page;
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_PAGES] = pages;

    new const
        Float:span = gContainerFrameData[menuid][E_CONTAINER_FRAME_THUMB_SPAN],
        Float:height = span / float(pages)
    ;

    new const
        PlayerText:thumb = PlayerText:gPlayerFrameData[playerid][base + E_PLAYER_FRAME_THUMB]
    ;

    PlayerTextDrawSetPos(
        playerid, thumb,
        gContainerFrameData[menuid][E_CONTAINER_FRAME_THUMB_X],
        gContainerFrameData[menuid][E_CONTAINER_FRAME_THUMB_Y] + float(page) * height
    );
    PlayerTextDrawTextSize(playerid, thumb, CONTAINER_FRAME_BAR_WIDTH - 2.0, height);
    PlayerTextDrawShow(playerid, thumb);
}

static stock ShowFrameInternal(playerid, ContainerMenu:menuid) {
    if (!gContainerFrameData[menuid][E_CONTAINER_FRAME_BUILT]) {
        return;
    }

    BuildPlayerFrameInternal(playerid, menuid);

    TextDrawShowForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_PANEL]);
    TextDrawShowForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_LINE]);
    TextDrawShowForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_INNER]);
    TextDrawShowForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_BAR]);
    TextDrawShowForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_UP_BOX]);
    TextDrawShowForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_UP_ARROW]);
    TextDrawShowForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_DOWN_BOX]);
    TextDrawShowForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_DOWN_ARROW]);

    new const
        base = CONTAINER_FRAME_BASE(menuid)
    ;

    PlayerTextDrawShow(playerid, PlayerText:gPlayerFrameData[playerid][base + E_PLAYER_FRAME_TITLE]);
    PlayerTextDrawShow(playerid, PlayerText:gPlayerFrameData[playerid][base + E_PLAYER_FRAME_COUNT]);
}

static stock HideFrameInternal(playerid, ContainerMenu:menuid) {
    if (!gContainerFrameData[menuid][E_CONTAINER_FRAME_BUILT]) {
        return;
    }

    TextDrawHideForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_PANEL]);
    TextDrawHideForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_LINE]);
    TextDrawHideForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_INNER]);
    TextDrawHideForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_BAR]);
    TextDrawHideForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_UP_BOX]);
    TextDrawHideForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_UP_ARROW]);
    TextDrawHideForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_DOWN_BOX]);
    TextDrawHideForPlayer(playerid, gContainerFrameData[menuid][E_CONTAINER_FRAME_DOWN_ARROW]);

    if (!gPlayerFrameBuilt[playerid][menuid]) {
        return;
    }

    new const
        base = CONTAINER_FRAME_BASE(menuid)
    ;

    PlayerTextDrawHide(playerid, PlayerText:gPlayerFrameData[playerid][base + E_PLAYER_FRAME_TITLE]);
    PlayerTextDrawHide(playerid, PlayerText:gPlayerFrameData[playerid][base + E_PLAYER_FRAME_COUNT]);
    PlayerTextDrawHide(playerid, PlayerText:gPlayerFrameData[playerid][base + E_PLAYER_FRAME_THUMB]);

    // Nothing of the three is on the screen any more, so what they were reading
    // is no longer what the player can see. Left standing, the next open would
    // decide it had nothing to send.
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_CONTAINER] = _:INVALID_CONTAINER_ID;
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_ON_PAGE] = -1;
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_TOTAL] = -1;
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_PAGE] = -1;
    gPlayerFrameData[playerid][base + E_PLAYER_FRAME_DRAWN_PAGES] = -1;
}

// The frame's own pieces, in the order the client stacks them. A global
// textdraw made earlier is drawn behind one made later, so the panel has to
// exist before the slots it sits behind and the buttons after them -- which is
// why this is two halves of BuildContainerMenu rather than a call of its own.
static stock BuildFramePanelsInternal(ContainerMenu:menuid, const title[]) {
    new
        Float:gridX,
        Float:gridY,
        Float:gridSizeX,
        Float:gridSizeY
    ;

    if (!FrameBoundsInternal(menuid, gridX, gridY, gridSizeX, gridSizeY)) {
        return;
    }

    // The bar stands in its own column to the right of the slots, and the panel
    // behind them covers both. Everything else is measured off those two.
    new const
        Float:barX = gridX + gridSizeX + CONTAINER_FRAME_BORDER,
        Float:innerX = gridX - CONTAINER_FRAME_BORDER,
        Float:innerY = gridY - CONTAINER_FRAME_BORDER,
        Float:innerSizeX = barX + CONTAINER_FRAME_BAR_WIDTH + CONTAINER_FRAME_BORDER - innerX,
        Float:innerSizeY = gridSizeY + CONTAINER_FRAME_BORDER * 2.0,
        Float:outerX = innerX - CONTAINER_FRAME_OUTER_MARGIN,
        Float:outerY = gridY - CONTAINER_FRAME_BORDER - CONTAINER_FRAME_TITLE_HEIGHT
    ;

    gContainerFrameData[menuid][E_CONTAINER_FRAME_PANEL] = FramePanelInternal(
        outerX, outerY,
        innerSizeX + CONTAINER_FRAME_OUTER_MARGIN * 2.0,
        innerY + innerSizeY + CONTAINER_FRAME_OUTER_MARGIN + 1.0 - outerY,
        CONTAINER_FRAME_PANEL_COLOUR
    );

    gContainerFrameData[menuid][E_CONTAINER_FRAME_LINE] = FramePanelInternal(
        innerX, innerY - 3.0,
        innerSizeX, 1.5,
        CONTAINER_FRAME_LINE_COLOUR
    );

    gContainerFrameData[menuid][E_CONTAINER_FRAME_INNER] = FramePanelInternal(
        innerX, innerY,
        innerSizeX, innerSizeY,
        CONTAINER_FRAME_INNER_COLOUR
    );

    gContainerFrameData[menuid][E_CONTAINER_FRAME_BAR] = FramePanelInternal(
        barX, gridY,
        CONTAINER_FRAME_BAR_WIDTH, gridSizeY,
        CONTAINER_FRAME_BAR_COLOUR
    );

    gContainerFrameData[menuid][E_CONTAINER_FRAME_THUMB_X] = barX + 1.0;
    gContainerFrameData[menuid][E_CONTAINER_FRAME_THUMB_Y] = gridY + CONTAINER_FRAME_BUTTON_SPAN;
    gContainerFrameData[menuid][E_CONTAINER_FRAME_THUMB_SPAN] = gridSizeY - CONTAINER_FRAME_BUTTON_SPAN * 2.0;

    strpack(gContainerFrameData[menuid][E_CONTAINER_FRAME_TITLE], title, MAX_CONTAINER_MENU_TITLE char);
}

static stock BuildFrameButtonsInternal(ContainerMenu:menuid) {
    new
        Float:gridX,
        Float:gridY,
        Float:gridSizeX,
        Float:gridSizeY
    ;

    if (!FrameBoundsInternal(menuid, gridX, gridY, gridSizeX, gridSizeY)) {
        return;
    }

    new const
        Float:barX = gridX + gridSizeX + CONTAINER_FRAME_BORDER
    ;

    new
        data[E_TEXT_DRAW_ARRAY_DATA]
    ;

    data[E_TEXT_DRAW_ARRAY_CELL] = CONTAINER_TEXT_DRAW_ARRAY_CELL;
    data[E_TEXT_DRAW_ARRAY_MENU] = menuid;

    gContainerFrameData[menuid][E_CONTAINER_FRAME_UP_BOX] = FrameSpriteInternal(
        barX - 2.0, gridY - 1.5,
        "LD_BEAT:CHIT", CONTAINER_FRAME_BAR_WIDTH + 3.75, 15.0,
        CONTAINER_FRAME_BUTTON_COLOUR, true
    );

    data[E_TEXT_DRAW_ARRAY_SLOT] = CONTAINER_MENU_SLOT_PAGE_UP;
    TextDrawSetArrayData(gContainerFrameData[menuid][E_CONTAINER_FRAME_UP_BOX], data);

    // The arrows sit on top of the boxes and are not selectable themselves, so
    // a click anywhere on the button is the button and never a miss between the
    // arrow and the edge of what it is drawn on.
    gContainerFrameData[menuid][E_CONTAINER_FRAME_UP_ARROW] = FrameSpriteInternal(
        barX + 1.75, gridY + 2.5,
        "LD_BEAT:UP", 7.5, 7.0,
        CONTAINER_FRAME_TEXT_COLOUR, false
    );

    gContainerFrameData[menuid][E_CONTAINER_FRAME_DOWN_BOX] = FrameSpriteInternal(
        barX - 2.0, gridY + gridSizeY - CONTAINER_FRAME_BUTTON_SPAN - 0.5,
        "LD_BEAT:CHIT", CONTAINER_FRAME_BAR_WIDTH + 3.75, 15.0,
        CONTAINER_FRAME_BUTTON_COLOUR, true
    );

    data[E_TEXT_DRAW_ARRAY_SLOT] = CONTAINER_MENU_SLOT_PAGE_DOWN;
    TextDrawSetArrayData(gContainerFrameData[menuid][E_CONTAINER_FRAME_DOWN_BOX], data);

    gContainerFrameData[menuid][E_CONTAINER_FRAME_DOWN_ARROW] = FrameSpriteInternal(
        barX + 1.75, gridY + gridSizeY - 9.0,
        "LD_BEAT:DOWN", 7.5, 7.0,
        CONTAINER_FRAME_TEXT_COLOUR, false
    );

    gContainerFrameData[menuid][E_CONTAINER_FRAME_BUILT] = true;
}

/**
 * # External
 */

stock bool:HasContainerMenuFrame(ContainerMenu:menuid) {
    if (!IsValidContainerMenuInternal(menuid)) {
        return false;
    }

    return gContainerFrameData[menuid][E_CONTAINER_FRAME_BUILT];
}

stock bool:ScrollContainerMenu(playerid, ContainerMenu:menuid, step) {
    if (!IsPlayerUsingContainerMenu(playerid, menuid)) {
        return false;
    }

    new const
        page = gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_PAGE] + step
    ;

    if (page < 0) {
        return false;
    }

    if (page >= GetContainerMenuPageCount(playerid, menuid)) {
        return false;
    }

    return SetContainerMenuPage(playerid, menuid, page);
}

stock ContainerMenu:BuildContainerMenu(
    Float:startX,
    Float:startY,
    Float:boxSizeX,
    Float:boxSizeY,
    rows,
    columns,
    colour,
    Float:padding = 1.0,
    E_CONTAINER_AMOUNT_POSITION:amountPos = CONTAINER_AMOUNT_BOTTOM_RIGHT,
    Float:amountMarginX = 2.0,
    Float:amountMarginY = 8.0,
    Float:amountLetterX = 0.110,
    Float:amountLetterY = 0.700,
    TEXT_DRAW_ALIGN:amountAlignment = TEXT_DRAW_ALIGN_RIGHT,
    bool:frame = false,
    const title[] = ""
) {
    if (gContainerMenuCount == MAX_CONTAINER_MENUS) {
        return INVALID_CONTAINER_MENU_ID;
    }

    if (rows <= 0) {
        return INVALID_CONTAINER_MENU_ID;
    }

    if (columns <= 0) {
        return INVALID_CONTAINER_MENU_ID;
    }

    if (boxSizeX <= 0.0) {
        return INVALID_CONTAINER_MENU_ID;
    }

    if (boxSizeY <= 0.0) {
        return INVALID_CONTAINER_MENU_ID;
    }

    new const
        slots = rows * columns
    ;

    if (slots > MAX_CONTAINER_MENU_SLOTS) {
        return INVALID_CONTAINER_MENU_ID;
    }

    new const
        ContainerMenu:menuid = gContainerMenuCount++
    ;

    gContainerMenuData[menuid][E_CONTAINER_MENU_START_X]     = startX;
    gContainerMenuData[menuid][E_CONTAINER_MENU_START_Y]     = startY;
    gContainerMenuData[menuid][E_CONTAINER_MENU_BOX_SIZE_X]  = boxSizeX;
    gContainerMenuData[menuid][E_CONTAINER_MENU_BOX_SIZE_Y]  = boxSizeY;
    gContainerMenuData[menuid][E_CONTAINER_MENU_PADDING]     = padding;
    gContainerMenuData[menuid][E_CONTAINER_MENU_ROWS]        = rows;
    gContainerMenuData[menuid][E_CONTAINER_MENU_COLUMNS]     = columns;
    gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS] = slots;
    gContainerMenuData[menuid][E_CONTAINER_MENU_COLOUR]      = colour;

    switch (amountPos) {
        case CONTAINER_AMOUNT_TOP_LEFT: {
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_OFFSET_X] = amountMarginX;
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_OFFSET_Y] = amountMarginY;
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_ALIGNMENT] = TEXT_DRAW_ALIGN_LEFT;
        }
        case CONTAINER_AMOUNT_TOP_RIGHT: {
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_OFFSET_X] = boxSizeX - amountMarginX;
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_OFFSET_Y] = amountMarginY;
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_ALIGNMENT] = TEXT_DRAW_ALIGN_RIGHT;
        }
        case CONTAINER_AMOUNT_BOTTOM_LEFT: {
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_OFFSET_X] = amountMarginX;
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_OFFSET_Y] = boxSizeY - amountMarginY;
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_ALIGNMENT] = TEXT_DRAW_ALIGN_LEFT;
        }
        case CONTAINER_AMOUNT_BOTTOM_RIGHT: {
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_OFFSET_X] = boxSizeX - amountMarginX;
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_OFFSET_Y] = boxSizeY - amountMarginY;
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_ALIGNMENT] = TEXT_DRAW_ALIGN_RIGHT;
        }
        case CONTAINER_AMOUNT_CUSTOM: {
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_OFFSET_X] = amountMarginX;
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_OFFSET_Y] = amountMarginY;
            gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_ALIGNMENT] = amountAlignment;
        }
    }

    gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_LETTER_SIZE_X] = amountLetterX;
    gContainerMenuData[menuid][E_CONTAINER_MENU_AMOUNT_LETTER_SIZE_Y] = amountLetterY;

    // Before the slots, so the client stacks them the right way round.
    if (frame) {
        BuildFramePanelsInternal(menuid, title);
    }

    new const
        Float:spacedX = boxSizeX + padding,
        Float:spacedY = boxSizeY + padding
    ;

    for (new i, Text:t, data[E_TEXT_DRAW_ARRAY_DATA]; i != slots; ++i) {
        t = TextDrawCreate(
            startX + (i % columns) * spacedX,
            startY + (i / columns) * spacedY, "LD_DUAL:WHITE"
        );
        TextDrawTextSize(t, boxSizeX, boxSizeY);
        TextDrawColour(t, colour);
        TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);
        TextDrawSetSelectable(t, true);
        gContainerMenuBox[menuid][i] = t;

        data[E_TEXT_DRAW_ARRAY_CELL] = CONTAINER_TEXT_DRAW_ARRAY_CELL;
        data[E_TEXT_DRAW_ARRAY_SLOT] = i;
        data[E_TEXT_DRAW_ARRAY_MENU] = menuid;
        TextDrawSetArrayData(t, data);

        t = TextDrawCreate(
            startX + (i % columns) * spacedX,
            startY + (i / columns) * spacedY, "_"
        );
        TextDrawBackgroundColour(t, 0);
        TextDrawFont(t, TEXT_DRAW_FONT_PREVIEW);
        TextDrawTextSize(t, boxSizeX, boxSizeY);
        gContainerMenuModel[menuid][i] = t;
    }

    if (frame) {
        BuildFrameButtonsInternal(menuid);
    }

    return menuid;
}

stock bool:ShowContainerMenu(playerid, Container:containerid, ContainerMenu:menuid, page = 0) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IsValidContainer(containerid)) {
        return false;
    }

    if (!IsValidContainerMenuInternal(menuid)) {
        return false;
    }

    BuildPlayerMenuInternal(playerid, menuid);

    new const
        Container:previous = gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_CONTAINER],
        bool:wasOpen = previous != INVALID_CONTAINER_ID
    ;

    if (!wasOpen) {
        new const
            colour = gContainerMenuData[menuid][E_CONTAINER_MENU_COLOUR]
        ;

        for (new i, slots = gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS]; i != slots; ++i) {
            // Said rather than assumed. Somebody else's rare item may have left
            // the box a different colour, and what a player is shown is
            // whatever the definition held at the moment it was sent to them.
            TextDrawColour(gContainerMenuBox[menuid][i], colour);
            TextDrawShowForPlayer(playerid, gContainerMenuBox[menuid][i]);

            gPlayerSlotData[playerid][CONTAINER_MENU_SLOT_BASE(menuid, i) + E_PLAYER_SLOT_COLOUR] = colour;
        }
    }

    gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_CONTAINER] = containerid;
    gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_PAGE] = page;

    if (!wasOpen) {
        gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_ORDER] = ++gMenuOrder;
    }

    AddMenuViewerInternal(playerid);

    if (!wasOpen) {
        ShowFrameInternal(playerid, menuid);
    }

    DrawPageInternal(playerid, menuid);
    UpdateFrameInternal(playerid, menuid);

    if (!wasOpen) {
        SelectTextDraw(playerid, CONTAINER_MENU_SELECT_COLOUR);
    }

    if (previous != containerid) {
        CallLocalFunction("OnPlayerChangeContainerMenu", "iiii", playerid, _:menuid, _:containerid, _:previous);
    }

    return true;
}

stock bool:RefreshContainerMenu(playerid, ContainerMenu:menuid) {
    if (!IsPlayerUsingContainerMenu(playerid, menuid)) {
        return false;
    }

    DrawPageInternal(playerid, menuid);
    UpdateFrameInternal(playerid, menuid);

    return true;
}

stock bool:RefreshContainerViewers(Container:containerid) {
    if (!IsValidContainer(containerid)) {
        return false;
    }

    RefreshContainerViewersInternal(containerid);

    return true;
}

stock bool:HideContainerMenu(playerid, ContainerMenu:menuid) {
    if (!IsValidContainerMenuInternal(menuid)) {
        return false;
    }

    if (gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_CONTAINER] == INVALID_CONTAINER_ID) {
        return false;
    }

    for (new i, slots = gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS]; i != slots; ++i) {
        TextDrawHideForPlayer(playerid, gContainerMenuBox[menuid][i]);
        TextDrawHideForPlayer(playerid, gContainerMenuModel[menuid][i]);

        PlayerTextDrawHide(playerid, PlayerText:gPlayerSlotData[playerid][CONTAINER_MENU_SLOT_BASE(menuid, i) + E_PLAYER_SLOT_AMOUNT]);
    }

    HideFrameInternal(playerid, menuid);
    ResetPlayerMenuCacheInternal(playerid, menuid);

    gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_CONTAINER] = INVALID_CONTAINER_ID;
    gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_PAGE] = 0;

    gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_ORDER] = 0;

    // Only once nothing of theirs is left. Selection mode is the player's and
    // not the menu's, so cancelling it while another menu is still up leaves
    // that one drawn and unclickable.
    if (!HasAnyMenuOpenInternal(playerid)) {
        RemoveMenuViewerInternal(playerid);
        CancelSelectTextDraw(playerid);
    }

    return true;
}

stock bool:IsValidContainerMenu(ContainerMenu:menuid) {
    return IsValidContainerMenuInternal(menuid);
}

stock bool:IsPlayerUsingContainerMenu(playerid, ContainerMenu:menuid) {
    if (!IsValidContainerMenuInternal(menuid)) {
        return false;
    }

    return gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_CONTAINER] != INVALID_CONTAINER_ID;
}

stock GetContainerMenuSlots(ContainerMenu:menuid) {
    if (!IsValidContainerMenuInternal(menuid)) {
        return 0;
    }

    return gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS];
}

// The same arithmetic BuildContainerMenu lays the boxes out with, answered
// rather than repeated, so the grid is described in one place.
stock bool:SetContainerMenuSlotColour(playerid, ContainerMenu:menuid, slot, colour) {
    if (!IsPlayerUsingContainerMenu(playerid, menuid)) {
        return false;
    }

    if (!(0 <= slot < gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS])) {
        return false;
    }

    new const
        Text:textid = gContainerMenuBox[menuid][slot]
    ;

    TextDrawColour(textid, colour);
    TextDrawShowForPlayer(playerid, textid);

    return true;
}

stock GetContainerMenuColour(ContainerMenu:menuid) {
    if (!IsValidContainerMenuInternal(menuid)) {
        return 0;
    }

    return gContainerMenuData[menuid][E_CONTAINER_MENU_COLOUR];
}

stock GetContainerMenuSlotColour(playerid, ContainerMenu:menuid, slot) {
    if (!IsValidContainerMenuInternal(menuid)) {
        return 0;
    }

    if (!(0 <= slot < gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS])) {
        return 0;
    }

    new const
        colour = gPlayerSlotData[playerid][CONTAINER_MENU_SLOT_BASE(menuid, slot) + E_PLAYER_SLOT_COLOUR]
    ;

    if (colour == 0) {
        return gContainerMenuData[menuid][E_CONTAINER_MENU_COLOUR];
    }

    return colour;
}

stock GetContainerMenuColumns(ContainerMenu:menuid) {
    if (!IsValidContainerMenu(menuid)) {
        return 0;
    }

    return gContainerMenuData[menuid][E_CONTAINER_MENU_COLUMNS];
}

stock bool:SetContainerMenuSlotAmountShown(playerid, ContainerMenu:menuid, slot, bool:shown) {
    if (!IsPlayerUsingContainerMenu(playerid, menuid)) {
        return false;
    }

    if (!(0 <= slot < gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS])) {
        return false;
    }

    new const
        base = CONTAINER_MENU_SLOT_BASE(menuid, slot)
    ;

    if (!shown) {
        SetSlotBitInternal(gPlayerSlotCoveredMask[playerid], menuid, slot, true);
        PlayerTextDrawHide(playerid, PlayerText:gPlayerSlotData[playerid][base + E_PLAYER_SLOT_AMOUNT]);
        SetSlotBitInternal(gPlayerSlotShownMask[playerid], menuid, slot, false);

        return true;
    }

    SetSlotBitInternal(gPlayerSlotCoveredMask[playerid], menuid, slot, false);

    if (GetDrawnItemInternal(playerid, base) == INVALID_ITEM_ID) {
        return false;
    }

    PlayerTextDrawShow(playerid, PlayerText:gPlayerSlotData[playerid][base + E_PLAYER_SLOT_AMOUNT]);
    SetSlotBitInternal(gPlayerSlotShownMask[playerid], menuid, slot, true);

    return true;
}

stock bool:GetContainerMenuSlotPos(ContainerMenu:menuid, slot, &Float:x, &Float:y) {
    if (!IsValidContainerMenuInternal(menuid)) {
        return false;
    }

    if (!(0 <= slot < gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS])) {
        return false;
    }

    new const
        columns = gContainerMenuData[menuid][E_CONTAINER_MENU_COLUMNS],
        Float:padding = gContainerMenuData[menuid][E_CONTAINER_MENU_PADDING]
    ;

    x = gContainerMenuData[menuid][E_CONTAINER_MENU_START_X]
        + float(slot % columns)
        * (gContainerMenuData[menuid][E_CONTAINER_MENU_BOX_SIZE_X] + padding);

    y = gContainerMenuData[menuid][E_CONTAINER_MENU_START_Y]
        + float(slot / columns)
        * (gContainerMenuData[menuid][E_CONTAINER_MENU_BOX_SIZE_Y] + padding);

    return true;
}

stock bool:GetContainerMenuSize(ContainerMenu:menuid, &Float:boxSizeX, &Float:boxSizeY, &Float:padding) {
    if (!IsValidContainerMenuInternal(menuid)) {
        return false;
    }

    boxSizeX = gContainerMenuData[menuid][E_CONTAINER_MENU_BOX_SIZE_X];
    boxSizeY = gContainerMenuData[menuid][E_CONTAINER_MENU_BOX_SIZE_Y];
    padding  = gContainerMenuData[menuid][E_CONTAINER_MENU_PADDING];

    return true;
}

stock GetContainerMenuPage(playerid, ContainerMenu:menuid) {
    if (!IsPlayerUsingContainerMenu(playerid, menuid)) {
        return -1;
    }

    return gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_PAGE];
}

stock GetContainerMenuPageCount(playerid, ContainerMenu:menuid) {
    if (!IsPlayerUsingContainerMenu(playerid, menuid)) {
        return 0;
    }

    new const
        slots = gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS],
        capacity = GetContainerCapacity(gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_CONTAINER])
    ;

    return (capacity + slots - 1) / slots;
}

stock bool:SetContainerMenuPage(playerid, ContainerMenu:menuid, page) {
    if (!IsPlayerUsingContainerMenu(playerid, menuid)) {
        return false;
    }

    if (!(0 <= page < GetContainerMenuPageCount(playerid, menuid))) {
        return false;
    }

    gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_PAGE] = page;

    DrawPageInternal(playerid, menuid);
    UpdateFrameInternal(playerid, menuid);

    CallLocalFunction("OnPlayerChangeContainerMenuPage", "iii", playerid, _:menuid, page);

    return true;
}

stock Container:GetContainerMenuContainer(playerid, ContainerMenu:menuid) {
    if (!IsPlayerUsingContainerMenu(playerid, menuid)) {
        return INVALID_CONTAINER_ID;
    }

    return gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_CONTAINER];
}

stock Item:GetContainerMenuSlotItem(playerid, ContainerMenu:menuid, slot) {
    if (!IsValidContainerMenuInternal(menuid)) {
        return INVALID_ITEM_ID;
    }

    if (!(0 <= slot < gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS])) {
        return INVALID_ITEM_ID;
    }

    return GetDrawnItemInternal(playerid, CONTAINER_MENU_SLOT_BASE(menuid, slot));
}

// The container slot behind a cell on screen, which is the cell plus whatever
// the current page skipped.
stock GetContainerMenuSlotIndex(playerid, ContainerMenu:menuid, slot) {
    if (!IsPlayerUsingContainerMenu(playerid, menuid)) {
        return -1;
    }

    if (!(0 <= slot < gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS])) {
        return -1;
    }

    return gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_PAGE] * gContainerMenuData[menuid][E_CONTAINER_MENU_TOTAL_SLOTS] + slot;
}

/**
 * # Calls
 */

hook OnPlayerClickTextDraw(playerid, Text:clickedid) {
    if (clickedid == INVALID_TEXT_DRAW) {
        // One menu a press, the one opened last. Two menus on a screen are a
        // locker standing in front of the pockets it is being emptied into,
        // and closing both at once takes away the half the character is still
        // using -- so the locker goes, the pockets stay, and escape again
        // takes those.
        new
            ContainerMenu:topmost = INVALID_CONTAINER_MENU_ID,
            order = 0
        ;

        for (new ContainerMenu:menuid; menuid != MAX_CONTAINER_MENUS; ++menuid) {
            if (gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_CONTAINER] == INVALID_CONTAINER_ID) {
                continue;
            }

            if (gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_ORDER] < order) {
                continue;
            }

            order = gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_ORDER];
            topmost = menuid;
        }

        if (topmost == INVALID_CONTAINER_MENU_ID) {
            return 0;
        }

        if (CallLocalFunction("OnPlayerCancelContainerMenu", "ii", playerid, _:topmost)) {
            SelectTextDraw(playerid, CONTAINER_MENU_SELECT_COLOUR);

            return 1;
        }

        HideContainerMenu(playerid, topmost);

        if (HasAnyMenuOpenInternal(playerid)) {
            SelectTextDraw(playerid, CONTAINER_MENU_SELECT_COLOUR);
        }

        return 0;
    }

    if (!TextDrawHasArrayData(clickedid)) {
        return 0;
    }

    new
        data[E_TEXT_DRAW_ARRAY_DATA]
    ;

    TextDrawGetArrayData(clickedid, data);

    if (data[E_TEXT_DRAW_ARRAY_CELL] != CONTAINER_TEXT_DRAW_ARRAY_CELL) {
        return 0;
    }

    new const
        ContainerMenu:menuid = data[E_TEXT_DRAW_ARRAY_MENU],
        slot = data[E_TEXT_DRAW_ARRAY_SLOT]
    ;

    if (!IsPlayerUsingContainerMenu(playerid, menuid)) {
        return 0;
    }

    // The ends of the scroll bar come through here carrying a slot that is not
    // on the grid, and are answered before anything looks for an item in it.
    if (slot == CONTAINER_MENU_SLOT_PAGE_UP) {
        ScrollContainerMenu(playerid, menuid, -1);

        return 1;
    }

    if (slot == CONTAINER_MENU_SLOT_PAGE_DOWN) {
        ScrollContainerMenu(playerid, menuid, 1);

        return 1;
    }

    CallLocalFunction(
        "OnPlayerClickContainerMenuSlot", "iiiii",
        playerid,
        _:menuid,
        slot,
        GetContainerMenuSlotIndex(playerid, menuid, slot),
        _:GetDrawnItemInternal(playerid, CONTAINER_MENU_SLOT_BASE(menuid, slot))
    );

    return 1;
}

// Only the countdowns that are on a screen right now, and only the ones whose
// text actually changed. DurationFormatShort drops to HH MM past an hour and
// DD HH past a day, so most seconds have nothing to send.
hook OnItemDurationTick() {
    for (new v; v != gMenuViewerCount; ++v) {
        new const
            playerid = gMenuViewer[v]
        ;

        for (new cell; cell != CONTAINER_MENU_MASK_SIZE; ++cell) {
            new
                bits = gPlayerSlotTimedMask[playerid][cell]
            ;

            if (!bits) {
                continue;
            }

            new const
                ContainerMenu:menuid = ContainerMenu:(cell / CONTAINER_MENU_MASK_CELLS),
                base = (cell % CONTAINER_MENU_MASK_CELLS) << 5
            ;

            for (new bit; bits; ++bit, bits >>>= 1) {
                if (!(bits & 1)) {
                    continue;
                }

                new const
                    slot = base + bit
                ;

                DrawSlotInternal(playerid, menuid, slot, GetDrawnItemInternal(playerid, CONTAINER_MENU_SLOT_BASE(menuid, slot)));
            }
        }
    }

    return 0;
}

// An item that draws as something else now. Everybody looking at whatever holds
// it is shown it again, which is the same answer a slot changing hands gets --
// and the redraw below only sends the model, because that is all that differs.
hook OnItemPreviewModelChange(Item:itemid, modelid, previous) {
    new
        Container:containerid = INVALID_CONTAINER_ID,
        index = -1
    ;

    if (GetItemContainer(itemid, containerid, index)) {
        RefreshContainerViewersInternal(containerid);
    }

    return 0;
}

// playerid here is whoever caused the change, which is not the same question as
// who can see it: a locker two people are standing at has to redraw for both.
hook OnItemRemoveFromContainer(Container:containerid, Item:itemid, index, playerid) {
    RefreshContainerViewersInternal(containerid);

    return 0;
}

hook OnItemAddToContainer(Container:containerid, Item:itemid, index, playerid) {
    RefreshContainerViewersInternal(containerid);

    return 0;
}

hook OnPlayerConnect(playerid) {
    RemoveMenuViewerInternal(playerid);

    for (new ContainerMenu:menuid; menuid != MAX_CONTAINER_MENUS; ++menuid) {
        gPlayerFrameBuilt[playerid][menuid] = false;
        gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_BUILT] = false;
        gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_CONTAINER] = INVALID_CONTAINER_ID;
        gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_PAGE] = 0;
        gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_ORDER] = 0;

        ResetPlayerMenuCacheInternal(playerid, menuid);
    }

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    RemoveMenuViewerInternal(playerid);

    for (new ContainerMenu:menuid; menuid != MAX_CONTAINER_MENUS; ++menuid) {
        gPlayerFrameBuilt[playerid][menuid] = false;
        gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_BUILT] = false;
        gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_CONTAINER] = INVALID_CONTAINER_ID;
        gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_PAGE] = 0;
        gPlayerMenuData[playerid][menuid][E_PLAYER_MENU_ORDER] = 0;

        ResetPlayerMenuCacheInternal(playerid, menuid);
    }

    return 0;
}
