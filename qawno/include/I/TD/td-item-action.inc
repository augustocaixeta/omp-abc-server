#if defined _INC_TD_ITEM_ACTION
    #endinput
#endif
#define _INC_TD_ITEM_ACTION

#include <open.mp>

#include <I\item-attributes>
#include <I\TD\td-container>

// Required even though td-container pulls it in: pp-hooks includes y_unique
// above its own include guard, so each #include here is what advances the
// counter that keeps two files hooking the same callback from colliding.
#include <pp-hooks>

/**
 * # Header
 */

// The little menu that opens against a slot when a player clicks an item:
// Use, Drop, Equip, Close.
//
// Which rows appear is not decided here. The framework has no idea what a
// gamemode's attributes mean, so an action is registered with the attribute
// that switches it on, and a row is drawn for every registered action the
// clicked item's build carries:
//
//     AddItemAction("Use",   ITEM_ATTR_USABLE);
//     AddItemAction("Drop",  ITEM_ATTR_DROPPABLE);
//     AddItemAction("Equip", ITEM_ATTR_EQUIPPABLE);
//
// A gamemode that later invents ITEM_ATTR_SPLITTABLE registers "Split" beside
// them and nothing in this file changes -- the row appears on the items that
// carry the flag, and the choice arrives at OnPlayerSelectItemAction with the
// action id it was registered under. That is the whole extension point.
//
// The numbers in the corners of the slots it covers are put away while it is
// open, and come back when it closes. Both are player textdraws, and which of
// two ends up on top is the client's business rather than something this side
// can state -- so rather than argue about it, whatever would be read through is
// simply not drawn.
//
// Which ones those are is arithmetic rather than a search. The menu hangs
// straight down from the slot it was opened against and is narrower than a
// column, so it can only ever reach the slot itself and the ones directly
// beneath it -- which is that slot's index plus the width of the grid, once per
// row it reaches.
//
// Close is always the last row and is never registered: it is answered here.
//
// Opening is the gamemode's call, not this library's. A click on a slot is a
// click, and only the gamemode knows whether it should open a menu, use the
// item outright, or start a drag:
//
//     public OnPlayerClickContainerMenuSlot(playerid, ContainerMenu:menuid, slot, index, Item:itemid) {
//         ShowItemActionMenu(playerid, menuid, slot);
//         return 1;
//     }

#if !defined MAX_ITEM_ACTIONS
    #define MAX_ITEM_ACTIONS (ItemAction:8)
#endif

#if !defined MAX_ITEM_ACTION_LABEL
    #define MAX_ITEM_ACTION_LABEL (16)
#endif

// How tall one menu is allowed to get, Info and Close included. This is not the same
// limit as MAX_ITEM_ACTIONS and must not be tied to it: a gamemode can register
// a dozen actions across many kinds of item and still want no item to open a
// menu of a dozen rows. Five reads well against a slot -- four actions and
// Close; past that the box stops looking like a context menu.
//
// An item qualifying for more than this shows the first of them in registration
// order, so the order actions are registered in is also their priority.
// GetItemActions is not capped this way and will report all of them.
#if !defined MAX_ITEM_ACTION_ROWS
    #define MAX_ITEM_ACTION_ROWS (5)
#endif

// One per row, plus the label over them and the backdrop under them.
#if MAX_ITEM_ACTION_ROWS + 2 > _:MAX_PLAYER_TEXT_DRAWS
    #error MAX_ITEM_ACTION_ROWS leaves no player textdraws for the rows
#endif

#if !defined ITEM_ACTION_TEXT_DRAW_ARRAY_CELL
    #define ITEM_ACTION_TEXT_DRAW_ARRAY_CELL (1)
#endif

// Geometry, taken off a four-row menu built in a textdraw editor rather than
// reasoned about: a row is 42x12 with a pixel of padding around it, the backdrop
// is the rows plus that padding on every side, and the label sits two pixels
// into the first row. They are separate defines so tuning one does not disturb
// the rest, but the four that describe a row were measured together and only
// hold together together.
#if !defined ITEM_ACTION_ROW_SIZE_X
    #define ITEM_ACTION_ROW_SIZE_X (42.0)
#endif

#if !defined ITEM_ACTION_ROW_SIZE_Y
    #define ITEM_ACTION_ROW_SIZE_Y (12.0)
#endif

// The gap between rows, which is what makes them read as separate boxes, and
// also the border the backdrop shows around them.
#if !defined ITEM_ACTION_ROW_PADDING
    #define ITEM_ACTION_ROW_PADDING (1.0)
#endif

// The menu is centred on the slot it belongs to and hangs from halfway down it,
// which is what puts it over the lower half of the item's box. These two are the
// nudge off that, not the position itself: the vertical one is what the editor
// put it at against a 45 pixel slot, and a menu built with taller or shorter
// boxes is the reason it is a define and not a literal.
#if !defined ITEM_ACTION_OFFSET_X
    #define ITEM_ACTION_OFFSET_X (0.0)
#endif

#if !defined ITEM_ACTION_OFFSET_Y
    #define ITEM_ACTION_OFFSET_Y (-4.5)
#endif

// Opaque black, so the menu reads as its own surface over the slots rather than
// as four boxes floating on them.
#if !defined ITEM_ACTION_BACK_COLOUR
    #define ITEM_ACTION_BACK_COLOUR (0x000000FF)
#endif

#if !defined ITEM_ACTION_ROW_COLOUR
    #define ITEM_ACTION_ROW_COLOUR (0x252525FF)
#endif

// Close is the one row that undoes rather than does, so it does not look like
// the others. Dark enough to stay a background: the label on top of it is the
// thing being read.
#if !defined ITEM_ACTION_CLOSE_COLOUR
    #define ITEM_ACTION_CLOSE_COLOUR (0x800000FF)
#endif

#if !defined ITEM_ACTION_TEXT_COLOUR
    #define ITEM_ACTION_TEXT_COLOUR (0xFFFFFFFF)
#endif

// The letter sizes belong to this font: changing one without the others is what
// makes the lines stop landing on the rows.
#if !defined ITEM_ACTION_TEXT_FONT
    #define ITEM_ACTION_TEXT_FONT (TEXT_DRAW_FONT_1)
#endif

#if !defined ITEM_ACTION_LETTER_SIZE_X
    #define ITEM_ACTION_LETTER_SIZE_X (0.135)
#endif

#if !defined ITEM_ACTION_LETTER_SIZE_Y
    #define ITEM_ACTION_LETTER_SIZE_Y (0.730)
#endif

// How far into a row its word sits.
#if !defined ITEM_ACTION_TEXT_OFFSET_Y
    #define ITEM_ACTION_TEXT_OFFSET_Y (2.0)
#endif

// What separates two labels in the one textdraw that carries all of them. Two
// breaks rather than one because a line at ITEM_ACTION_LETTER_SIZE_Y advances
// about half a row: this is the third number tied to the other two, and the one
// that has to change if either of them does.
#if !defined ITEM_ACTION_ROW_BREAK
    #define ITEM_ACTION_ROW_BREAK "~n~~n~"
#endif

// The two rows this file draws itself rather than taking from a registration,
// each 0 or 1. Escape already closes the menu, and a gamemode with nothing to
// say about an item has no use for Info, so both are preferences rather than
// requirements -- which is why they are here and not welded into the drawing.
//
// Info is the only row that is not an attribute. Reading what a thing is does
// not depend on what the thing is, and nothing else turned out to share that:
// a package meant to be handed to someone is not a package meant to be left on
// the floor, so even Drop belongs to an attribute like every other row.
#if !defined ITEM_ACTION_USE_CLOSE_ROW
    #define ITEM_ACTION_USE_CLOSE_ROW (1)
#endif

#if !defined ITEM_ACTION_USE_INFO_ROW
    #define ITEM_ACTION_USE_INFO_ROW (1)
#endif

// How many of a menu's rows are registered actions: the rest of them.
#define MAX_DRAWN_ITEM_ACTIONS (MAX_ITEM_ACTION_ROWS - ITEM_ACTION_USE_CLOSE_ROW - ITEM_ACTION_USE_INFO_ROW)

#if MAX_DRAWN_ITEM_ACTIONS < 1
    #error MAX_ITEM_ACTION_ROWS leaves no room for an action beside Info and Close
#endif

#if !defined ITEM_ACTION_CLOSE_LABEL
    #define ITEM_ACTION_CLOSE_LABEL "Close"
#endif

#if !defined ITEM_ACTION_INFO_LABEL
    #define ITEM_ACTION_INFO_LABEL "Info"
#endif

#define INVALID_ITEM_ACTION_ID (ItemAction:-1)

// Registered as the attribute, this makes a row that every item gets. Info is
// drawn without one, so this is for a gamemode that wants a universal row of its
// own beside it. It is spelled out rather than left as 0, so that a registration
// missing its attribute is still refused as the mistake it is.
#define ITEM_ACTION_ALWAYS (-1)

// The rows this file draws itself. They are not registered actions, so they have
// ids of their own rather than places in the table. Info arrives at
// OnPlayerSelectItemAction like any other row and is answered by the gamemode --
// only the drawing of it is settled here, because only the gamemode knows what
// there is to say about an item.
#define ITEM_ACTION_CLOSE (ItemAction:-2)
#define ITEM_ACTION_INFO (ItemAction:-3)

static const
    ITEM_ACTION_BREAK[] = ITEM_ACTION_ROW_BREAK
;

static enum _:E_ITEM_ACTION_DATA {
    E_ITEM_ACTION_LABEL[MAX_ITEM_ACTION_LABEL],
    E_ITEM_ACTION_ATTRIBUTE
};

static
    gItemActionData[MAX_ITEM_ACTIONS][E_ITEM_ACTION_DATA],
    ItemAction:gItemActionCount
;

static enum _:E_TEXT_DRAW_ARRAY_DATA {
    E_TEXT_DRAW_ARRAY_CELL,
    E_TEXT_DRAW_ARRAY_ROW
};

// The rows a player is looking at right now. Which actions they are depends on
// the item that was clicked, so the answer is per player and is worked out once
// when the menu opens rather than again when a row is picked.
static enum _:E_PLAYER_ACTION_DATA {
    bool:E_PLAYER_ACTION_BUILT,
    bool:E_PLAYER_ACTION_OPEN,

    ContainerMenu:E_PLAYER_ACTION_MENU,
    E_PLAYER_ACTION_SLOT,
    Item:E_PLAYER_ACTION_ITEM,
    E_PLAYER_ACTION_ROWS,

    // Where this player sits in the open list below. Kept here rather than in an
    // array of its own because the record already exists: one more field costs a
    // cell per player, while a second array would cost the same cells and a
    // second set of row offsets on top of them.
    E_PLAYER_ACTION_LIST_SLOT
};

// Who has a menu up, packed at the front. Every item destroyed or taken out of a
// container has to ask whether anybody was looking at it, and asking that by
// walking the player table makes the cost of destroying an item the size of the
// server rather than the number of people looking at anything.
//
// A menu is open for a moment, by one player, while items are destroyed and
// moved constantly -- so this is nearly always empty and never more than a
// handful long. Adding is an append and removing is a swap with the last, two
// writes each, so it costs nothing at the ends either.
static
    gOpenActionMenuList[MAX_PLAYERS],
    gOpenActionMenus,
    gPlayerActionData[MAX_PLAYERS][E_PLAYER_ACTION_DATA],
    PlayerText:gPlayerActionBack[MAX_PLAYERS],
    PlayerText:gPlayerActionRow[MAX_PLAYERS][MAX_ITEM_ACTION_ROWS],
    PlayerText:gPlayerActionLabel[MAX_PLAYERS],
    ItemAction:gPlayerActionOf[MAX_PLAYERS][MAX_ITEM_ACTION_ROWS]
;

/**
 * # Functions
 */

/**
 * @brief      Register a row the menu can show.
 *
 * The row appears on every item whose build carries `attribute`, so what a
 * row means is the gamemode's and which items get it is the attribute's.
 *
 * Registration order is the order rows appear, above Info and Close, and it
 * is also their priority: an item qualifying for more rows than
 * MAX_ITEM_ACTION_ROWS shows the first of them.
 *
 * Pass `ITEM_ACTION_ALWAYS` for a row every item gets. It is spelled out
 * rather than left as `0`, so that a registration missing its attribute is
 * still refused as the mistake it is.
 *
 * @param      label      What the player reads.
 * @param      attribute  Flag an item's build must carry, the same value
 *                        passed to AddItemBuildAttribute.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the id the choice will arrive under
 *             - `INVALID_ITEM_ACTION_ID` when:
 *                 + MAX_ITEM_ACTIONS are already registered
 *                 + `attribute` is `0`, which no item can carry
 *                 + `label` is empty
 */
forward ItemAction:AddItemAction(const label[], attribute);

/**
 * @brief      Which rows an item would get, without drawing anything.
 *
 * The whole of the dynamic decision, kept apart from the textdraws so the
 * same question can be asked for a dialog, a command, or a test.
 *
 * Info and Close are not included: they are rows, not actions. Nor is this
 * capped at MAX_ITEM_ACTION_ROWS the way the menu is -- it reports all of
 * them.
 *
 * @param      itemid   Item to ask about.
 * @param      actions  Reads back the action ids.
 * @param      size     How many the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many were written
 *             - `0` when:
 *                 + `itemid` is not a live item
 *                 + its build carries none of the registered attributes
 */
forward GetItemActions(Item:itemid, ItemAction:actions[], size = sizeof (actions));

/**
 * @brief      Whether an id names a registered action.
 *
 * False for `ITEM_ACTION_INFO` and `ITEM_ACTION_CLOSE`, which are rows this
 * file draws itself rather than registrations.
 *
 * @param      actionid  Id to check.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is a registered action
 *             - `false` when:
 *                 + `actionid` is out of range
 *                 + it is one of the rows drawn here
 */
forward bool:IsValidItemAction(ItemAction:actionid);

/**
 * @brief      How many actions are registered.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many have been registered
 */
forward GetItemActionCount();

/**
 * @brief      What an action's row reads.
 *
 * @param      actionid  Action to read.
 * @param      output    Reads back the label.
 * @param      size      How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` `actionid` is not a registered action
 */
forward bool:GetItemActionLabel(ItemAction:actionid, output[], size = sizeof (output));

/**
 * @brief      Which flag switches an action on.
 *
 * @param      actionid  Action to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the attribute it was registered with
 *             - `0` `actionid` is not a registered action
 */
forward GetItemActionAttribute(ItemAction:actionid);

/**
 * @brief      Open the menu against a slot.
 *
 * Laid out from halfway down that slot's box, and centred on it: a menu is
 * wider than a slot, so sharing the slot's left edge would hang the whole
 * thing off to the right of the item it belongs to.
 *
 * Opening is the gamemode's call and not this library's. A click on a slot
 * is a click, and only the gamemode knows whether it should open a menu,
 * use the item outright, or start a drag:
 *
 *     public OnPlayerClickContainerMenuSlot(playerid, ContainerMenu:menuid, slot, index, Item:itemid) {
 *         ShowItemActionMenu(playerid, menuid, slot);
 *
 *         return 1;
 *     }
 *
 * @param      playerid  Character to show it to.
 * @param      menuid    Menu the slot belongs to.
 * @param      slot      Slot to open against.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the menu is on their screen
 *             - `false` when:
 *                 + they are not using that container menu
 *                 + the slot is empty
 *                 + the item's build carries none of the registered
 *                   attributes, so there would be nothing to show but Info
 *                   and Close
 */
forward bool:ShowItemActionMenu(playerid, ContainerMenu:menuid, slot);

/**
 * @brief      Take the menu off a character's screen.
 *
 * The container menu underneath is left alone.
 *
 * @param      playerid  Character to hide it from.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is gone
 *             - `false` they had no menu up
 */
forward bool:HideItemActionMenu(playerid);

/**
 * @brief      Whether a character has the menu up.
 *
 * @param      playerid  Player to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is on their screen
 *             - `false` when:
 *                 + they are not connected
 *                 + they have no menu up
 */
forward bool:IsPlayerUsingItemActionMenu(playerid);

/**
 * @brief      Which item a character's open menu is about.
 *
 * @param      playerid  Player to ask about.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the item the menu was opened on
 *             - `INVALID_ITEM_ID` when:
 *                 + they have no menu up
 *                 + the item has since been destroyed
 */
forward Item:GetPlayerItemActionItem(playerid);

/**
 * # Events
 */

/**
 * @brief      A row was picked.
 *
 * `actionid` is what AddItemAction answered, or one of the two rows this
 * file draws itself:
 *
 *   - `ITEM_ACTION_INFO` is left entirely to the handler, because only the
 *     gamemode knows what there is to say about an item
 *   - `ITEM_ACTION_CLOSE` has already hidden the menu by the time this is
 *     called, so a handler that only cares about the real actions can
 *     ignore it
 *
 * The menu is left open for every other row, because whether a Use closes
 * the inventory is the gamemode's decision. Call HideItemActionMenu to
 * close it.
 *
 * @param      playerid  Character who picked it.
 * @param      actionid  Row they picked.
 * @param      itemid    Item the menu was about.
 * @param      menuid    Container menu it was opened over.
 * @param      slot      Slot it was opened against.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` other OnPlayerSelectItemAction hooks still run
 *             - `1` the chain ends here, which is what a handler that owns
 *               this row answers
 */
forward OnPlayerSelectItemAction(playerid, ItemAction:actionid, Item:itemid, ContainerMenu:menuid, slot);

/**
 * # Internal
 */

static stock bool:IsValidItemActionInternal(ItemAction:actionid) {
    return (0 <= _:actionid < _:gItemActionCount);
}

// One row of boxes and one label over them. Created the first time a player
// opens the menu and then kept: a menu that opens on every click would
// otherwise pay for creating and destroying ten textdraws each time.
static stock BuildPlayerActionMenuInternal(playerid) {
    if (gPlayerActionData[playerid][E_PLAYER_ACTION_BUILT]) {
        return;
    }

    new
        data[E_TEXT_DRAW_ARRAY_DATA]
    ;

    data[E_TEXT_DRAW_ARRAY_CELL] = ITEM_ACTION_TEXT_DRAW_ARRAY_CELL;

    new const
        PlayerText:back = CreatePlayerTextDraw(playerid, 0.0, 0.0, "LD_DUAL:WHITE")
    ;

    PlayerTextDrawColour(playerid, back, ITEM_ACTION_BACK_COLOUR);
    PlayerTextDrawFont(playerid, back, TEXT_DRAW_FONT_SPRITE);

    gPlayerActionBack[playerid] = back;

    for (new row; row != MAX_ITEM_ACTION_ROWS; ++row) {
        new const
            PlayerText:pt = CreatePlayerTextDraw(playerid, 0.0, 0.0, "LD_DUAL:WHITE")
        ;

        PlayerTextDrawTextSize(playerid, pt, ITEM_ACTION_ROW_SIZE_X, ITEM_ACTION_ROW_SIZE_Y);
        PlayerTextDrawColour(playerid, pt, ITEM_ACTION_ROW_COLOUR);
        PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_SPRITE);
        PlayerTextDrawSetSelectable(playerid, pt, true);

        data[E_TEXT_DRAW_ARRAY_ROW] = row;
        PlayerTextDrawSetArrayData(playerid, pt, data);

        gPlayerActionRow[playerid][row] = pt;
    }

    new const
        PlayerText:label = CreatePlayerTextDraw(playerid, 0.0, 0.0, "_")
    ;

    PlayerTextDrawFont(playerid, label, ITEM_ACTION_TEXT_FONT);
    PlayerTextDrawLetterSize(playerid, label, ITEM_ACTION_LETTER_SIZE_X, ITEM_ACTION_LETTER_SIZE_Y);
    PlayerTextDrawColour(playerid, label, ITEM_ACTION_TEXT_COLOUR);
    PlayerTextDrawAlignment(playerid, label, TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawSetProportional(playerid, label, true);
    PlayerTextDrawSetShadow(playerid, label, 0);

    gPlayerActionLabel[playerid] = label;

    gPlayerActionData[playerid][E_PLAYER_ACTION_BUILT] = true;
}

// The half of closing a menu that is only bookkeeping: out of the open list, and
// nothing remembered about what it was showing.
//
// Kept apart from the drawing because a connection is where a player textdraw
// lives. A slot that changes hands carries the variables in it and nothing else,
// so at connect and disconnect there is no menu on any screen to take down --
// telling the client to hide textdraws it does not have is a message for
// somebody who is not there.
static stock ForgetPlayerActionMenuInternal(playerid) {
    if (!gPlayerActionData[playerid][E_PLAYER_ACTION_OPEN]) {
        return;
    }

    gPlayerActionData[playerid][E_PLAYER_ACTION_OPEN] = false;
    gPlayerActionData[playerid][E_PLAYER_ACTION_ROWS] = 0;
    gPlayerActionData[playerid][E_PLAYER_ACTION_ITEM] = INVALID_ITEM_ID;

    new const
        slot = gPlayerActionData[playerid][E_PLAYER_ACTION_LIST_SLOT],
        moved = gOpenActionMenuList[--gOpenActionMenus]
    ;

    gOpenActionMenuList[slot] = moved;
    gPlayerActionData[moved][E_PLAYER_ACTION_LIST_SLOT] = slot;
}

// The whole of it: off the screen as well. Anything closing a menu on somebody
// who is actually looking at it wants this one.
// The slot the menu was opened against, and the ones under it that it reaches.
// A menu of n rows hangs (n spacings + a padding) down from halfway through its
// slot, so the row below is covered and the one after that is not -- but it is
// measured rather than assumed, because the row count is the item's and the slot
// height is the gamemode's.
static stock CoverActionMenuAmountsInternal(playerid, ContainerMenu:menuid, slot, rows, bool:shown) {
    new
        Float:boxSizeX,
        Float:boxSizeY,
        Float:padding
    ;

    if (!GetContainerMenuSize(menuid, boxSizeX, boxSizeY, padding)) {
        return;
    }

    new const
        columns = GetContainerMenuColumns(menuid),
        slots = GetContainerMenuSlots(menuid)
    ;

    if (columns == 0) {
        return;
    }

    new const
        Float:spacedY = ITEM_ACTION_ROW_SIZE_Y + ITEM_ACTION_ROW_PADDING,

        // How far below the top of its own slot the menu ends.
        Float:reach = boxSizeY / 2.0 + ITEM_ACTION_OFFSET_Y
                    + float(rows) * spacedY + ITEM_ACTION_ROW_PADDING
    ;

    for (new row; ; ++row) {
        // Where that row's number sits, which is the bottom of its box.
        new const
            Float:bottom = float(row) * (boxSizeY + padding) + boxSizeY
        ;

        if (row != 0 && bottom > reach) {
            return;
        }

        new const
            covered = slot + row * columns
        ;

        if (covered >= slots) {
            return;
        }

        SetContainerMenuSlotAmountShown(playerid, menuid, covered, shown);
    }
}

static stock HidePlayerActionMenuInternal(playerid) {
    if (!gPlayerActionData[playerid][E_PLAYER_ACTION_OPEN]) {
        return;
    }

    CoverActionMenuAmountsInternal(
        playerid,
        gPlayerActionData[playerid][E_PLAYER_ACTION_MENU],
        gPlayerActionData[playerid][E_PLAYER_ACTION_SLOT],
        gPlayerActionData[playerid][E_PLAYER_ACTION_ROWS],
        true
    );

    for (new row; row != gPlayerActionData[playerid][E_PLAYER_ACTION_ROWS]; ++row) {
        PlayerTextDrawHide(playerid, gPlayerActionRow[playerid][row]);
    }

    PlayerTextDrawHide(playerid, gPlayerActionLabel[playerid]);
    PlayerTextDrawHide(playerid, gPlayerActionBack[playerid]);

    ForgetPlayerActionMenuInternal(playerid);
}

// What OnItemRemoveFromContainer and OnItemDestroy both mean: this menu is about
// one item in one slot, and the item is not in that slot any anymore -- whether
// it was used, moved, dropped or destroyed.
static stock CloseActionMenusForItemInternal(Item:itemid) {
    for (new i, viewer; i != gOpenActionMenus; ) {
        viewer = gOpenActionMenuList[i];

        if (gPlayerActionData[viewer][E_PLAYER_ACTION_ITEM] != itemid) {
            ++i;

            continue;
        }

        HidePlayerActionMenuInternal(viewer);

        if (gOpenActionMenus > i && gOpenActionMenuList[i] == viewer) {
            ++i;
        }
    }
}

/**
 * # External
 */

stock ItemAction:AddItemAction(const label[], attribute) {
    if (gItemActionCount == MAX_ITEM_ACTIONS) {
        return INVALID_ITEM_ACTION_ID;
    }

    if (!(1 <= strlen(label) < MAX_ITEM_ACTION_LABEL)) {
        return INVALID_ITEM_ACTION_ID;
    }

    if (attribute == 0) {
        return INVALID_ITEM_ACTION_ID;
    }

    new const
        ItemAction:actionid = gItemActionCount++
    ;

    strcopy(gItemActionData[actionid][E_ITEM_ACTION_LABEL], label);
    gItemActionData[actionid][E_ITEM_ACTION_ATTRIBUTE] = attribute;

    return actionid;
}

stock GetItemActions(Item:itemid, ItemAction:actions[], size = sizeof (actions)) {
    new
        count = 0
    ;

    if (!IsValidItem(itemid)) {
        return 0;
    }

    for (new ItemAction:actionid; actionid != gItemActionCount && count != size; ++actionid) {
        new const
            attribute = gItemActionData[actionid][E_ITEM_ACTION_ATTRIBUTE]
        ;

        if (attribute != ITEM_ACTION_ALWAYS && !HasItemAttribute(itemid, attribute)) {
            continue;
        }

        actions[count++] = actionid;
    }

    return count;
}

stock bool:IsValidItemAction(ItemAction:actionid) {
    return IsValidItemActionInternal(actionid);
}

stock GetItemActionCount() {
    return _:gItemActionCount;
}

stock bool:GetItemActionLabel(ItemAction:actionid, output[], size = sizeof (output)) {
    if (!IsValidItemActionInternal(actionid)) {
        return false;
    }

    strcopy(output, gItemActionData[actionid][E_ITEM_ACTION_LABEL], size);

    return true;
}

stock GetItemActionAttribute(ItemAction:actionid) {
    if (!IsValidItemActionInternal(actionid)) {
        return 0;
    }

    return gItemActionData[actionid][E_ITEM_ACTION_ATTRIBUTE];
}

stock bool:ShowItemActionMenu(playerid, ContainerMenu:menuid, slot) {
    if (!IsPlayerUsingContainerMenu(playerid, menuid)) {
        return false;
    }

    new const
        Item:itemid = GetContainerMenuSlotItem(playerid, menuid, slot)
    ;

    if (itemid == INVALID_ITEM_ID) {
        return false;
    }

    new
        Float:slotX,
        Float:slotY,
        Float:boxSizeX,
        Float:boxSizeY,
        Float:padding
    ;

    if (!GetContainerMenuSlotPos(menuid, slot, slotX, slotY)) {
        return false;
    }

    GetContainerMenuSize(menuid, boxSizeX, boxSizeY, padding);

    new
        ItemAction:actions[MAX_DRAWN_ITEM_ACTIONS]
    ;

    new const
        count = GetItemActions(itemid, actions)
    ;

    if (count == 0) {
        return false;
    }

    new
        text[MAX_ITEM_ACTION_ROWS * (MAX_ITEM_ACTION_LABEL + sizeof (ITEM_ACTION_BREAK))]
    ;

    for (new i; i != count; ++i) {
        if (i != 0) {
            strcat(text, ITEM_ACTION_BREAK);
        }

        strcat(text, gItemActionData[actions[i]][E_ITEM_ACTION_LABEL]);
        gPlayerActionOf[playerid][i] = actions[i];
    }

    new
        rows = count
    ;

    #if ITEM_ACTION_USE_INFO_ROW
        strcat(text, ITEM_ACTION_BREAK);
        strcat(text, ITEM_ACTION_INFO_LABEL);

        gPlayerActionOf[playerid][rows++] = ITEM_ACTION_INFO;
    #endif

    #if ITEM_ACTION_USE_CLOSE_ROW
        strcat(text, ITEM_ACTION_BREAK);
        strcat(text, ITEM_ACTION_CLOSE_LABEL);

        gPlayerActionOf[playerid][rows++] = ITEM_ACTION_CLOSE;
    #endif

    BuildPlayerActionMenuInternal(playerid);
    HidePlayerActionMenuInternal(playerid);

    new const
        Float:spacedY   = ITEM_ACTION_ROW_SIZE_Y + ITEM_ACTION_ROW_PADDING,
        Float:backSizeX = ITEM_ACTION_ROW_SIZE_X + ITEM_ACTION_ROW_PADDING * 2.0,
        Float:backSizeY = float(rows) * spacedY + ITEM_ACTION_ROW_PADDING,
        Float:backX     = slotX + (boxSizeX - backSizeX) / 2.0 + ITEM_ACTION_OFFSET_X,
        Float:backY     = slotY + boxSizeY / 2.0 + ITEM_ACTION_OFFSET_Y,
        Float:startX    = backX + ITEM_ACTION_ROW_PADDING,
        Float:startY    = backY + ITEM_ACTION_ROW_PADDING
    ;

    CoverActionMenuAmountsInternal(playerid, menuid, slot, rows, false);

    PlayerTextDrawSetPos(playerid, gPlayerActionBack[playerid], backX, backY);
    PlayerTextDrawTextSize(playerid, gPlayerActionBack[playerid], backSizeX, backSizeY);
    PlayerTextDrawShow(playerid, gPlayerActionBack[playerid]);

    for (new row; row != rows; ++row) {
        new const
            PlayerText:pt = gPlayerActionRow[playerid][row]
        ;

        if (gPlayerActionOf[playerid][row] == ITEM_ACTION_CLOSE) {
            PlayerTextDrawColour(playerid, pt, ITEM_ACTION_CLOSE_COLOUR);
        } else {
            PlayerTextDrawColour(playerid, pt, ITEM_ACTION_ROW_COLOUR);
        }

        PlayerTextDrawSetPos(playerid, pt,
            startX,
            startY + float(row) * spacedY
        );
        PlayerTextDrawShow(playerid, pt);
    }

    PlayerTextDrawSetPos(playerid, gPlayerActionLabel[playerid],
        startX + ITEM_ACTION_ROW_SIZE_X / 2.0,
        startY + ITEM_ACTION_TEXT_OFFSET_Y
    );
    PlayerTextDrawSetString(playerid, gPlayerActionLabel[playerid], text);
    PlayerTextDrawShow(playerid, gPlayerActionLabel[playerid]);

    gPlayerActionData[playerid][E_PLAYER_ACTION_OPEN] = true;
    gPlayerActionData[playerid][E_PLAYER_ACTION_ROWS] = rows;
    gPlayerActionData[playerid][E_PLAYER_ACTION_MENU] = menuid;
    gPlayerActionData[playerid][E_PLAYER_ACTION_SLOT] = slot;
    gPlayerActionData[playerid][E_PLAYER_ACTION_ITEM] = itemid;
    gPlayerActionData[playerid][E_PLAYER_ACTION_LIST_SLOT] = gOpenActionMenus;
    
    gOpenActionMenuList[gOpenActionMenus++] = playerid;

    return true;
}

stock bool:HideItemActionMenu(playerid) {
    if (!gPlayerActionData[playerid][E_PLAYER_ACTION_OPEN]) {
        return false;
    }

    HidePlayerActionMenuInternal(playerid);

    return true;
}

stock bool:IsPlayerUsingItemActionMenu(playerid) {
    return gPlayerActionData[playerid][E_PLAYER_ACTION_OPEN];
}

stock Item:GetPlayerItemActionItem(playerid) {
    if (!gPlayerActionData[playerid][E_PLAYER_ACTION_OPEN]) {
        return INVALID_ITEM_ID;
    }

    return gPlayerActionData[playerid][E_PLAYER_ACTION_ITEM];
}

/**
 * # Calls
 */

// The first escape dismisses this menu and leaves the inventory underneath it
// alone; the second one reaches td-container and closes that. Answering the
// framework's own callback rather than the raw textdraw one is what makes the
// order certain: whichever way pp-hooks chains the click handlers, only one
// file answers this question.
// The menu is drawn against a slot, and a slot is a square on a page of one
// container. Both halves of that can move out from under it: the page can turn,
// and the window can be pointed at a different container -- which is what
// happens the moment somebody opens a bag that was lying in a crate.
//
// Either way the square now holds something else, and the alternative is a menu
// offering to eat what is no longer under it.
hook OnPlayerChangeContainerMenuPage(playerid, ContainerMenu:menuid, page) {
    HidePlayerActionMenuInternal(playerid);

    return 0;
}

hook OnPlayerChangeContainerMenu(playerid, ContainerMenu:menuid, Container:containerid, Container:previous) {
    HidePlayerActionMenuInternal(playerid);

    return 0;
}

hook OnPlayerCancelContainerMenu(playerid, ContainerMenu:menuid) {
    if (!gPlayerActionData[playerid][E_PLAYER_ACTION_OPEN]) {
        return 0;
    }

    HidePlayerActionMenuInternal(playerid);

    return 1;
}

hook OnPlayerClickPlayerTextDraw(playerid, PlayerText:clickedid) {
    if (!gPlayerActionData[playerid][E_PLAYER_ACTION_OPEN]) {
        return 0;
    }

    if (!PlayerTextDrawHasArrayData(playerid, clickedid)) {
        return 0;
    }

    new
        data[E_TEXT_DRAW_ARRAY_DATA]
    ;

    PlayerTextDrawGetArrayData(playerid, clickedid, data);

    if (data[E_TEXT_DRAW_ARRAY_CELL] != ITEM_ACTION_TEXT_DRAW_ARRAY_CELL) {
        return 0;
    }

    new const
        row = data[E_TEXT_DRAW_ARRAY_ROW]
    ;

    if (!(0 <= row < gPlayerActionData[playerid][E_PLAYER_ACTION_ROWS])) {
        return 0;
    }

    new const
        ItemAction:actionid = gPlayerActionOf[playerid][row],
        Item:itemid = gPlayerActionData[playerid][E_PLAYER_ACTION_ITEM],
        ContainerMenu:menuid = gPlayerActionData[playerid][E_PLAYER_ACTION_MENU],
        slot = gPlayerActionData[playerid][E_PLAYER_ACTION_SLOT]
    ;

    if (actionid == ITEM_ACTION_CLOSE) {
        HidePlayerActionMenuInternal(playerid);
    }

    CallLocalFunction("OnPlayerSelectItemAction", "iiiii", playerid, _:actionid, _:itemid, _:menuid, slot);

    return 1;
}

hook OnItemRemoveFromContainer(Container:containerid, Item:itemid, index, playerid) {
    CloseActionMenusForItemInternal(itemid);

    return 0;
}

hook OnItemDestroy(Item:itemid) {
    CloseActionMenusForItemInternal(itemid);

    return 0;
}

// A swap does not take an item out of its container, so the removal callback
// above never fires -- but the menu is about an item in a slot, and both items
// are in a different slot afterwards.
hook OnContainerSlotsSwap(Container:containerid, Item:firstItem, Item:secondItem, first, second, playerid) {
    CloseActionMenusForItemInternal(firstItem);
    CloseActionMenusForItemInternal(secondItem);

    return 0;
}

// Through the forget rather than by clearing the flag, which is what the open
// list cannot survive: setting E_PLAYER_ACTION_OPEN to false without taking the
// entry out leaves a name in the list and the count above it, and the next sweep
// then reads a player who is not open.
//
// The textdraws go with the connection, so BUILT has to go too -- the next
// occupant of the slot has none and needs them made again.
hook OnPlayerConnect(playerid) {
    ForgetPlayerActionMenuInternal(playerid);

    gPlayerActionData[playerid][E_PLAYER_ACTION_BUILT] = false;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    ForgetPlayerActionMenuInternal(playerid);

    gPlayerActionData[playerid][E_PLAYER_ACTION_BUILT] = false;

    return 0;
}
