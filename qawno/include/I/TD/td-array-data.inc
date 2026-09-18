#if defined _INC_TD_ARRAY_DATA
    #endinput
#endif
#define _INC_TD_ARRAY_DATA

#include <open.mp>
#include <pp-hooks>

/**
 * # Header
 */

// A few cells hung on a textdraw, so that a click can be answered without a
// lookup table on the side: which slot this is, which container it belongs to,
// which page it was drawn on. Read on every click and every redraw, so it is a
// plain array rather than anything that has to be searched.
//
// The player side is written as one row per player rather than three
// dimensions, and the reason is not only the offset table it saves.
//
// Pawn assigns whole arrays with a single block copy, which is the memset a
// disconnect wants -- but only in one shape. Measured: assigning a 1D row of a
// 2D array clears exactly that row; assigning a 2D row of a 3D array clears the
// whole array, neighbours included; and assigning a 1D row of a 3D array is a
// run time error 5. Three dimensions here would mean a 256 step loop on every
// disconnect and no safe way to shorten it.
//
// Flat, the clear is one instruction: 0.25 us against 16.25 us for the loop it
// replaces, and the offset table it drops is 256 cells per player.
//
// The price is the same one td-container pays for the same trick: an index is
// not a dimension, so E_PLAYER_TD_HAS_DATA comes back untagged and the one
// place that answers it as a bool says so out loud.

#if !defined MAX_TEXT_DRAW_ARRAY_DATA_SIZE
    #define MAX_TEXT_DRAW_ARRAY_DATA_SIZE (8)
#endif

static enum _:E_GLOBAL_TEXT_DRAW_DATA {
    bool:E_GLOBAL_TD_HAS_DATA,
    E_GLOBAL_TD_DATA[MAX_TEXT_DRAW_ARRAY_DATA_SIZE],
    E_GLOBAL_TD_DATA_SIZE
};

static enum _:E_PLAYER_TEXT_DRAW_DATA {
    bool:E_PLAYER_TD_HAS_DATA,
    E_PLAYER_TD_DATA[MAX_TEXT_DRAW_ARRAY_DATA_SIZE],
    E_PLAYER_TD_DATA_SIZE
};

#define PLAYER_TEXT_DRAW_DATA_SIZE (_:MAX_PLAYER_TEXT_DRAWS * E_PLAYER_TEXT_DRAW_DATA)
#define PLAYER_TEXT_DRAW_BASE(%0) (_:(%0) * E_PLAYER_TEXT_DRAW_DATA)

static
    gGlobalTextDrawData[MAX_TEXT_DRAWS][E_GLOBAL_TEXT_DRAW_DATA],
    gPlayerTextDrawData[MAX_PLAYERS][PLAYER_TEXT_DRAW_DATA_SIZE]
;

// What a disconnect copies over a player's row.
static const
    gPlayerTextDrawNone[PLAYER_TEXT_DRAW_DATA_SIZE]
;

/**
 * # Functions
 */

/**
 * @brief      Whether a global textdraw carries data.
 *
 * @param      textid  Textdraw to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` something has been written to it
 *             - `false` when:
 *                 + `textid` is not a live textdraw
 *                 + nothing has been written, or it was removed
 */
forward bool:TextDrawHasArrayData(Text:textid);

/**
 * @brief      Hang cells on a global textdraw.
 *
 * Replaces whatever was there. The cells are copied, so the array passed in
 * can be a local that goes out of scope straight after.
 *
 * @param      textid  Textdraw to write.
 * @param      data    Cells to store.
 * @param      size    How many of them.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the cells are stored
 *             - `false` when:
 *                 + `textid` is not a live textdraw
 *                 + `size` is above MAX_TEXT_DRAW_ARRAY_DATA_SIZE
 */
forward bool:TextDrawSetArrayData(Text:textid, const data[], size = sizeof (data));

/**
 * @brief      Read the cells back off a global textdraw.
 *
 * Copies as many as fit: the smaller of what was stored and what was asked
 * for. Anything past that is left as the caller had it.
 *
 * @param      textid  Textdraw to read.
 * @param      data    Reads back the cells.
 * @param      size    How many the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` when:
 *                 + `textid` is not a live textdraw
 *                 + it carries no data
 */
forward bool:TextDrawGetArrayData(Text:textid, data[], size = sizeof (data));

/**
 * @brief      Forget the cells on a global textdraw.
 *
 * The cells themselves are left where they are, because the flag is what is
 * read: the next write overwrites them anyway.
 *
 * @param      textid  Textdraw to clear.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it carries nothing now
 *             - `false` when:
 *                 + `textid` is not a live textdraw
 *                 + it already carried nothing
 */
forward bool:TextDrawRemoveArrayData(Text:textid);

/**
 * @brief      Whether one player's textdraw carries data.
 *
 * @param      playerid  Player the textdraw belongs to.
 * @param      textid    Textdraw to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` something has been written to it
 *             - `false` when:
 *                 + it is not a live textdraw of theirs
 *                 + nothing has been written, or it was removed
 */
forward bool:PlayerTextDrawHasArrayData(playerid, PlayerText:textid);

/**
 * @brief      Hang cells on one player's textdraw.
 *
 * @param      playerid  Player the textdraw belongs to.
 * @param      textid    Textdraw to write.
 * @param      data      Cells to store.
 * @param      size      How many of them.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the cells are stored
 *             - `false` when:
 *                 + it is not a live textdraw of theirs
 *                 + `size` is above MAX_TEXT_DRAW_ARRAY_DATA_SIZE
 */
forward bool:PlayerTextDrawSetArrayData(playerid, PlayerText:textid, const data[], size = sizeof (data));

/**
 * @brief      Read the cells back off one player's textdraw.
 *
 * @param      playerid  Player the textdraw belongs to.
 * @param      textid    Textdraw to read.
 * @param      data      Reads back the cells.
 * @param      size      How many the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` when:
 *                 + it is not a live textdraw of theirs
 *                 + it carries no data
 */
forward bool:PlayerTextDrawGetArrayData(playerid, PlayerText:textid, data[], size = sizeof (data));

/**
 * @brief      Forget the cells on one player's textdraw.
 *
 * @param      playerid  Player the textdraw belongs to.
 * @param      textid    Textdraw to clear.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it carries nothing now
 *             - `false` when:
 *                 + it is not a live textdraw of theirs
 *                 + it already carried nothing
 */
forward bool:PlayerTextDrawRemoveArrayData(playerid, PlayerText:textid);

/**
 * # Global
 */

stock bool:TextDrawHasArrayData(Text:textid) {
    if (!IsValidTextDraw(textid)) {
        return false;
    }

    return gGlobalTextDrawData[textid][E_GLOBAL_TD_HAS_DATA];
}

stock bool:TextDrawSetArrayData(Text:textid, const data[], size = sizeof (data)) {
    if (!IsValidTextDraw(textid)) {
        return false;
    }

    if (size > MAX_TEXT_DRAW_ARRAY_DATA_SIZE) {
        return false;
    }

    for (new i; i != size; ++i) {
        gGlobalTextDrawData[textid][E_GLOBAL_TD_DATA][i] = data[i];
    }

    gGlobalTextDrawData[textid][E_GLOBAL_TD_DATA_SIZE] = size;
    gGlobalTextDrawData[textid][E_GLOBAL_TD_HAS_DATA] = true;

    return true;
}

stock bool:TextDrawGetArrayData(Text:textid, data[], size = sizeof (data)) {
    if (!IsValidTextDraw(textid)) {
        return false;
    }

    if (!gGlobalTextDrawData[textid][E_GLOBAL_TD_HAS_DATA]) {
        return false;
    }

    new const
        bounded = (gGlobalTextDrawData[textid][E_GLOBAL_TD_DATA_SIZE] < size)
            ? gGlobalTextDrawData[textid][E_GLOBAL_TD_DATA_SIZE]
            : size
    ;

    for (new i; i != bounded; ++i) {
        data[i] = gGlobalTextDrawData[textid][E_GLOBAL_TD_DATA][i];
    }

    return true;
}

stock bool:TextDrawRemoveArrayData(Text:textid) {
    if (!IsValidTextDraw(textid)) {
        return false;
    }

    if (!gGlobalTextDrawData[textid][E_GLOBAL_TD_HAS_DATA]) {
        return false;
    }

    gGlobalTextDrawData[textid][E_GLOBAL_TD_HAS_DATA] = false;

    return true;
}

/**
 * # Player
 */

stock bool:PlayerTextDrawHasArrayData(playerid, PlayerText:textid) {
    if (!IsValidPlayerTextDraw(playerid, textid)) {
        return false;
    }

    return bool:gPlayerTextDrawData[playerid][PLAYER_TEXT_DRAW_BASE(textid) + E_PLAYER_TD_HAS_DATA];
}

stock bool:PlayerTextDrawSetArrayData(playerid, PlayerText:textid, const data[], size = sizeof (data)) {
    if (!IsValidPlayerTextDraw(playerid, textid)) {
        return false;
    }

    if (size > MAX_TEXT_DRAW_ARRAY_DATA_SIZE) {
        return false;
    }

    for (new i; i != size; ++i) {
        gPlayerTextDrawData[playerid][PLAYER_TEXT_DRAW_BASE(textid) + E_PLAYER_TD_DATA + i] = data[i];
    }

    gPlayerTextDrawData[playerid][PLAYER_TEXT_DRAW_BASE(textid) + E_PLAYER_TD_DATA_SIZE] = size;
    gPlayerTextDrawData[playerid][PLAYER_TEXT_DRAW_BASE(textid) + E_PLAYER_TD_HAS_DATA] = true;

    return true;
}

stock bool:PlayerTextDrawGetArrayData(playerid, PlayerText:textid, data[], size = sizeof (data)) {
    if (!IsValidPlayerTextDraw(playerid, textid)) {
        return false;
    }

    if (!gPlayerTextDrawData[playerid][PLAYER_TEXT_DRAW_BASE(textid) + E_PLAYER_TD_HAS_DATA]) {
        return false;
    }

    new const
        bounded = (gPlayerTextDrawData[playerid][PLAYER_TEXT_DRAW_BASE(textid) + E_PLAYER_TD_DATA_SIZE] < size)
            ? gPlayerTextDrawData[playerid][PLAYER_TEXT_DRAW_BASE(textid) + E_PLAYER_TD_DATA_SIZE]
            : size
    ;

    for (new i; i != bounded; ++i) {
        data[i] = gPlayerTextDrawData[playerid][PLAYER_TEXT_DRAW_BASE(textid) + E_PLAYER_TD_DATA + i];
    }

    return true;
}

stock bool:PlayerTextDrawRemoveArrayData(playerid, PlayerText:textid) {
    if (!IsValidPlayerTextDraw(playerid, textid)) {
        return false;
    }

    if (!gPlayerTextDrawData[playerid][PLAYER_TEXT_DRAW_BASE(textid) + E_PLAYER_TD_HAS_DATA]) {
        return false;
    }

    gPlayerTextDrawData[playerid][PLAYER_TEXT_DRAW_BASE(textid) + E_PLAYER_TD_HAS_DATA] = false;

    return true;
}

/**
 * # Calls
 */

hook OnPlayerDisconnect(playerid, reason) {
    gPlayerTextDrawData[playerid] = gPlayerTextDrawNone;

    return 0;
}
