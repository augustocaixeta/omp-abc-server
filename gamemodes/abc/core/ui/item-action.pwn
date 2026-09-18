#if defined _CORE_UI_ITEM_ACTION
    #endinput
#endif
#define _CORE_UI_ITEM_ACTION

#include <pp-hooks>

/**
 * # The item menu
 *
 * Which rows exist, and in what order they are drawn. This is the only file
 * that registers any of them: an action file answers for its row and never asks
 * for it, so what the menu looks like is readable in one place.
 *
 * The library draws INFO and CLOSE itself and leaves the rest to these. Only
 * MAX_DRAWN_ITEM_ACTIONS of them fit on any one item, so the order below is
 * also the priority -- an item that qualifies for more rows than fit shows the
 * first of them.
 */

new
    ItemAction:gItemActionUse,
    ItemAction:gItemActionEquip,
    ItemAction:gItemActionHold,
    ItemAction:gItemActionOpen,
    ItemAction:gItemActionSplit,
    ItemAction:gItemActionLoad,
    ItemAction:gItemActionUnload,
    ItemAction:gItemActionGive,
    ItemAction:gItemActionDrop,
    ItemAction:gItemActionDiscard
;

hook OnGameModeInit() {
    gItemActionUse     = AddItemAction("USE", ITEM_ATTRIBUTE_USABLE);
    gItemActionEquip   = AddItemAction("EQUIP", ITEM_ATTRIBUTE_EQUIPPABLE);
    gItemActionHold    = AddItemAction("PICK UP", ITEM_ATTRIBUTE_HOLDABLE);
    gItemActionOpen    = AddItemAction("OPEN", ITEM_ATTRIBUTE_OPENABLE);
    gItemActionSplit   = AddItemAction("SPLIT", ITEM_ATTRIBUTE_SPLITTABLE);
    gItemActionLoad    = AddItemAction("LOAD", ITEM_ATTRIBUTE_LOADABLE);
    gItemActionUnload  = AddItemAction("UNLOAD", ITEM_ATTRIBUTE_UNLOADABLE);
    gItemActionGive    = AddItemAction("GIVE", ITEM_ATTRIBUTE_GIVEABLE);
    gItemActionDrop    = AddItemAction("DROP", ITEM_ATTRIBUTE_DROPPABLE);
    gItemActionDiscard = AddItemAction("DISCARD", ITEM_ATTRIBUTE_DISCARDABLE);

    return 0;
}
