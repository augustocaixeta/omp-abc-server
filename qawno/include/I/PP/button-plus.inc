#if defined _INC_BUTTON_PLUS
    #endinput
#endif
#define _INC_BUTTON_PLUS

#include <open.mp>

#include <I\button>

// Required even though I\button pulls it in: pp-hooks includes y_unique above
// its own include guard, so each #include here is what advances the counter
// that keeps two files hooking the same callback from colliding.
#include <pp-hooks>

/**
 * # Header
 */

// Keys a button carries that the framework knows nothing about: which door it
// belongs to, which shop, which locker. One PawnPlus map per button, made the
// first time somebody writes to that button and deleted when it is destroyed,
// so a world full of buttons nobody has annotated costs one cell each.
//
// The ceiling is per button rather than global, and it only refuses keys that
// are new: overwriting one that is already there is always allowed, so a full
// button cannot be made unwritable by its own contents.

#if !defined MAX_BUTTON_KEY_LENGTH
    #define MAX_BUTTON_KEY_LENGTH (32)
#endif

#if !defined MAX_BUTTON_EXTRA_DATA
    #define MAX_BUTTON_EXTRA_DATA (64)
#endif

static
    Map:gButtonExtraData[MAX_BUTTONS]
;

/**
 * # Functions
 */

/**
 * @brief      Whether a button carries a key.
 *
 * @param      buttonid  Button to ask.
 * @param      key       Name to look for.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the key is there
 *             - `false` when:
 *                 + `buttonid` is not a live button
 *                 + nothing has ever been written to it
 *                 + it carries no such key
 */
forward bool:HasButtonExtraData(Button:buttonid, const key[]);

/**
 * @brief      Write one key onto a button.
 *
 * One cell. Anything bigger goes in by its own id -- a container, an item, a
 * string handle -- and is read back with the tag it was written with.
 *
 * @param      buttonid  Button to write.
 * @param      key       Name to write it under.
 * @param      value     The cell.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the key is written
 *             - `false` when:
 *                 + `key` is empty, or too long for MAX_BUTTON_KEY_LENGTH
 *                 + `buttonid` is not a live button
 *                 + the button is at MAX_BUTTON_EXTRA_DATA and the key is new
 */
forward bool:SetButtonExtraData(Button:buttonid, const key[], value);

/**
 * @brief      Take one key back off a button.
 *
 * @param      buttonid  Button to write.
 * @param      key       Name to remove.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the key is gone
 *             - `false` when:
 *                 + `buttonid` is not a live button
 *                 + nothing has ever been written to it
 *                 + it carried no such key
 */
forward bool:RemoveButtonExtraData(Button:buttonid, const key[]);

/**
 * @brief      Read one key off a button.
 *
 * A key that was never written and a key written as `0` read the same, so
 * ask HasButtonExtraData when the difference matters.
 *
 * @param      buttonid  Button to read.
 * @param      key       Name to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the cell it was written with
 *             - `0` when:
 *                 + `buttonid` is not a live button
 *                 + it carries no such key
 */
forward GetButtonExtraData(Button:buttonid, const key[]);

/**
 * @brief      How many keys a button carries.
 *
 * @param      buttonid  Button to count.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many keys are on it
 *             - `0` when:
 *                 + `buttonid` is not a live button
 *                 + nothing has ever been written to it
 */
forward GetButtonExtraDataCount(Button:buttonid);

/**
 * @brief      The button's own map, for walking every key it carries.
 *
 * This is the map itself and not a copy, so it is for reading through --
 * map_iter and the like. Deleting it leaves this module holding a handle to
 * nothing until the button is destroyed.
 *
 * @param      buttonid  Button to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the map of its keys
 *             - `INVALID_MAP` when:
 *                 + `buttonid` is not a live button
 *                 + nothing has ever been written to it
 */
forward Map:GetButtonExtraDataMap(Button:buttonid);

/**
 * # Internal
 */

static stock Map:ResolveButtonMapInternal(Button:buttonid) {
    if (!IsValidButton(buttonid)) {
        return INVALID_MAP;
    }

    return map_valid(gButtonExtraData[buttonid]) ? gButtonExtraData[buttonid] : INVALID_MAP;
}

static stock bool:IsValidButtonKeyInternal(const key[]) {
    return (1 <= strlen(key) < MAX_BUTTON_KEY_LENGTH);
}

static stock ClearButtonExtraDataInternal(Button:buttonid) {
    if (map_valid(gButtonExtraData[buttonid])) {
        map_delete_deep(gButtonExtraData[buttonid]);
    }

    gButtonExtraData[buttonid] = INVALID_MAP;
}

/**
 * # External
 */

stock bool:HasButtonExtraData(Button:buttonid, const key[]) {
    new const
        Map:map = ResolveButtonMapInternal(buttonid)
    ;

    return map != INVALID_MAP && map_has_str_key(map, key);
}

stock bool:SetButtonExtraData(Button:buttonid, const key[], value) {
    if (!IsValidButtonKeyInternal(key)) {
        return false;
    }

    if (!IsValidButton(buttonid)) {
        return false;
    }

    if (!map_valid(gButtonExtraData[buttonid])) {
        gButtonExtraData[buttonid] = map_new();
    }

    if (map_size(gButtonExtraData[buttonid]) >= MAX_BUTTON_EXTRA_DATA && !map_has_str_key(gButtonExtraData[buttonid], key)) {
        return false;
    }

    map_str_set(gButtonExtraData[buttonid], key, value);

    return true;
}

stock bool:RemoveButtonExtraData(Button:buttonid, const key[]) {
    new const
        Map:map = ResolveButtonMapInternal(buttonid)
    ;

    if (map == INVALID_MAP) {
        return false;
    }

    return map_str_remove_deep(map, key);
}

stock GetButtonExtraData(Button:buttonid, const key[]) {
    new const
        Map:map = ResolveButtonMapInternal(buttonid)
    ;

    if (map == INVALID_MAP) {
        return 0;
    }

    if (!map_has_str_key(map, key)) {
        return 0;
    }

    return map_str_get(map, key);
}

stock GetButtonExtraDataCount(Button:buttonid) {
    new const
        Map:map = ResolveButtonMapInternal(buttonid)
    ;

    return map != INVALID_MAP ? map_size(map) : 0;
}

stock Map:GetButtonExtraDataMap(Button:buttonid) {
    return ResolveButtonMapInternal(buttonid);
}

/**
 * # Calls
 */

hook OnButtonCreate(Button:buttonid) {
    ClearButtonExtraDataInternal(buttonid);

    return 0;
}

hook OnButtonDestroy(Button:buttonid) {
    ClearButtonExtraDataInternal(buttonid);

    return 0;
}
