#if defined _INC_ITEM_AMOUNT_PLUS
    #endinput
#endif
#define _INC_ITEM_AMOUNT_PLUS

#include <open.mp>

#include <I\item-amount>
#include <I\PP\item-plus>
#include <pp-hooks>

/**
 * # Header
 */

// The bridge between a field and a document. I\item-amount keeps the amount in
// a cell because it is read on every menu redraw and every duration tick, and
// this puts that cell into the same JSON the developer's own keys go into, so
// the database still sees one column and no schema.
//
// It lives here rather than in the core module because it is what depends on
// the JSON plugin: a script that never persists anything can leave it out and
// I\item-amount keeps working on its own.
//
// The number and the text are written under separate keys and asked about
// separately, because they are set separately. Asking HasItemAmount instead
// would answer true for an item carrying only text, and write a "@amount":0
// beside it -- a number the item never had, which the load would then believe.
//
// A duration is stored as the second it runs out on rather than as a length,
// so a save and a load do not hand an item back its whole shelf life. That
// makes restoring it conditional: the timer is only armed again when that
// second has not already gone by.
//
// JSON_GetInt and JSON_GetString answer 0 on success, like the rest of the
// plugin, which is why the reads below are negated.

#if !defined ITEM_AMOUNT_KEY
    #define ITEM_AMOUNT_KEY "@amount"
#endif

#if !defined ITEM_AMOUNT_TEXT_KEY
    #define ITEM_AMOUNT_TEXT_KEY "@amount-text"
#endif

/**
 * # Calls
 */

hook OnItemSerialize(Item:itemid, Node:node) {
    if (HasItemAmountValue(itemid)) {
        JSON_SetInt(node, ITEM_AMOUNT_KEY, GetItemAmountInt(itemid));
    }

    new
        text[MAX_ITEM_AMOUNT_LENGTH]
    ;

    if (GetItemAmountString(itemid, text)) {
        JSON_SetString(node, ITEM_AMOUNT_TEXT_KEY, text);
    }

    return 0;
}

hook OnItemDeserialize(Item:itemid, Node:node) {
    new
        value
    ;

    if (!JSON_GetInt(node, ITEM_AMOUNT_KEY, value)) {
        SetItemAmountInt(itemid, value);

        if (GetItemBuildAmountType(GetItemBuild(itemid)) == ITEM_AMOUNT_DURATION) {
            RestoreItemDuration(itemid);
        }
    }

    new
        text[MAX_ITEM_AMOUNT_LENGTH]
    ;

    if (!JSON_GetString(node, ITEM_AMOUNT_TEXT_KEY, text)) {
        SetItemAmountString(itemid, text);
    }

    return 0;
}
