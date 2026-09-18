#if defined _CORE_UI_ITEM_ACTION_UNLOAD
    #endinput
#endif
#define _CORE_UI_ITEM_ACTION_UNLOAD

#include <pp-hooks>

/**
 * # UNLOAD
 *
 * The row on a weapon that empties it back into the pockets. Magazine and
 * reserve both, turned back into boxes of the calibre it eats.
 *
 * Nothing is lost by it. A rifle at nothing is a rifle that cannot be fired,
 * not a rifle that has been thrown away, and the rounds are in the pockets to
 * be put back in whenever the character wants them there. That is the whole
 * point of the row: a weapon carries what it carries, and this is how a
 * character decides it should carry nothing.
 *
 * It works on a weapon in the pockets and on the one in their hands alike. The
 * one in their hands is emptied in the game as well, so what they are holding
 * agrees with what the item says.
 *
 * Pockets with no room left are the only thing that stops it part way. As much
 * comes out as fits, and what is left stays in the weapon rather than falling
 * on the floor.
 *
 * The menu closes behind it. Most rows are closed for them by the item leaving
 * the slot they were opened against, and this one does not move the weapon --
 * so it says so itself, rather than leaving a menu standing over a rifle whose
 * rows no longer describe it.
 */

hook OnPlayerSelectItemAction(playerid, ItemAction:actionid, Item:itemid, ContainerMenu:menuid, slot) {
    if (actionid != gItemActionUnload) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (!IsItemWeaponLoadable(itemid)) {
        SendPlayerNotice(playerid, "The %s takes no ammunition.", name);

        return 1;
    }

    new const
        held = GetItemWeaponAmmo(itemid)
    ;

    if (held <= 0) {
        SendPlayerNotice(playerid, "The %s is already empty.", name);

        return 1;
    }

    new const
        moved = UnloadPlayerWeapon(playerid, itemid)
    ;

    if (moved == 0) {
        SendPlayerNotice(playerid, "No room in your pockets for it.");

        return 1;
    }

    HideItemActionMenu(playerid);
    RefreshContainerMenu(playerid, menuid);

    if (moved == held) {
        SendPlayerNotice(playerid, "You empty the %s. %i back in your pockets.", name, moved);

        return 1;
    }

    SendPlayerNotice(playerid, "You take %i out of the %s. %i stays in it.", moved, name, held - moved);

    return 1;
}
