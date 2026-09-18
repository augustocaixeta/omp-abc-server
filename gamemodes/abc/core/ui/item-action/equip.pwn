#if defined _CORE_UI_ITEM_ACTION_EQUIP
    #endinput
#endif
#define _CORE_UI_ITEM_ACTION_EQUIP

#include <pp-hooks>

/**
 * # EQUIP
 *
 * The row that attaches nothing. An armed weapon is a weapon slot, so this
 * draws the game's weapon and never SetPlayerItem -- attaching the model
 * instead would put a prop in the character's hands that they cannot fire.
 *
 * Drawing one takes it out of the pockets and leaves it there: a character
 * carries as many weapons as the game has slots for and scrolls between them
 * normally. Picking the same item again is what puts it back.
 *
 * Everything about what is in the weapon belongs to core/item-build/weapon.
 * This row only says which one.
 */

hook OnPlayerSelectItemAction(playerid, ItemAction:actionid, Item:itemid, ContainerMenu:menuid, slot) {
    if (actionid != gItemActionEquip) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (!IsItemWeapon(itemid)) {
        SendPlayerNotice(playerid, "That is not a weapon.");

        return 1;
    }

    if (IsPlayerWeaponEquipped(playerid, itemid)) {
        if (!UnequipPlayerWeapon(playerid, itemid)) {
            SendPlayerNotice(playerid, "No room in your pockets for the %s.", name);

            return 1;
        }

        RefreshContainerMenu(playerid, menuid);

        SendPlayerNotice(playerid, "You put the %s away.", name);

        return 1;
    }

    if (!EquipPlayerWeapon(playerid, itemid)) {
        // EquipPlayerWeapon refuses for half a dozen reasons and says only that
        // it refused, so the ones a character can do something about are asked
        // again here. "You cannot draw the Spray Can" while stood still holding
        // one is not a sentence anybody can act on.
        if (IsItemWeaponLoadable(itemid) && GetItemWeaponAmmo(itemid) <= 0) {
            SendPlayerNotice(playerid, "The %s is empty.", name);

            return 1;
        }

        // The game keeps one weapon to a slot, and a spray can, an extinguisher
        // and a camera all want the same one. Drawing the second means putting
        // the first away, and that needs a pocket free.
        new const
            Item:occupant = GetPlayerWeaponSlotItem(playerid, GetWeaponSlotOfItem(itemid))
        ;

        if (occupant != INVALID_ITEM_ID && occupant != itemid) {
            new
                occupantName[MAX_ITEM_NAME]
            ;

            GetItemName(occupant, occupantName);

            SendPlayerNotice(playerid, "No room to put the %s away first.", occupantName);

            return 1;
        }

        SendPlayerNotice(playerid, "You cannot draw the %s.", name);

        return 1;
    }

    RefreshContainerMenu(playerid, menuid);

    if (!IsItemWeaponLoadable(itemid)) {
        SendPlayerNotice(playerid, "You draw the %s.", name);

        return 1;
    }

    SendPlayerNotice(playerid, "You draw the %s. %i rounds.", name, GetItemWeaponAmmo(itemid));

    return 1;
}
