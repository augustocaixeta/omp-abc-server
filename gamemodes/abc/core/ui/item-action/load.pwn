#if defined _CORE_UI_ITEM_ACTION_LOAD
    #endinput
#endif
#define _CORE_UI_ITEM_ACTION_LOAD

#include <pp-hooks>

/**
 * # LOAD
 *
 * The row on a box of ammunition that puts it into a weapon the character is
 * carrying. Whichever weapon is in their hands if it eats that calibre, and
 * otherwise the first one they have that does.
 *
 * It happens at once. There is no bar: a bar is for something a character is
 * seen doing and can be interrupted, and this is a click in a menu that is
 * already open -- there is no moment in it to hold a key through.
 *
 * The row lives on the ammunition and not on the weapon. A character thinking
 * "put this in my rifle" is looking at the box, and the rifle is already on
 * their back.
 *
 * Clicking the box and then clicking a weapon in the pockets does the same
 * thing to a weapon that is not being carried -- that is core/ui/item-transfer,
 * and the two are the same call underneath.
 *
 * The menu closes behind it. A box emptied by loading is destroyed and takes
 * the menu with it, but one only partly used stays in its slot -- and a menu
 * left standing over it would be describing a box that has since changed.
 *
 * Rounds only. A can of fuel goes into a weapon by being dropped onto it in the
 * pockets, which is the transfer above and needs no row -- and a row here would
 * have to guess which of ten chainsaws was meant, where dropping it on one says
 * so.
 */

hook OnPlayerSelectItemAction(playerid, ItemAction:actionid, Item:itemid, ContainerMenu:menuid, slot) {
    if (actionid != gItemActionLoad) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME],
        calibre[MAX_CALIBRE_NAME]
    ;

    GetItemName(itemid, name);

    new const
        Calibre:calibreid = GetItemAmmunitionCalibre(itemid)
    ;

    GetCalibreName(calibreid, calibre);

    new const
        Item:weaponid = FindPlayerWeaponOfCalibre(playerid, calibreid)
    ;

    if (weaponid == INVALID_ITEM_ID) {
        SendPlayerNotice(playerid, "You are carrying nothing that takes %s.", calibre);

        return 1;
    }

    new
        weaponName[MAX_ITEM_NAME]
    ;

    GetItemName(weaponid, weaponName);

    new const
        loaded = LoadPlayerWeaponFromAmmunition(playerid, itemid)
    ;

    if (loaded == 0) {
        SendPlayerNotice(playerid, "The %s is full.", weaponName);

        return 1;
    }

    HideItemActionMenu(playerid);
    RefreshContainerMenu(playerid, menuid);

    SendPlayerNotice(playerid, "You load %i into the %s.", loaded, weaponName);

    return 1;
}
