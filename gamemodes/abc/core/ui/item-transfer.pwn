#if defined _CORE_UI_ITEM_TRANSFER
    #endinput
#endif
#define _CORE_UI_ITEM_TRANSFER

#include <pp-hooks>

/**
 * # Picking one up to put it somewhere
 *
 * A slot is selected, it goes a different colour, and the next slot clicked is
 * what happens to it:
 *
 *   the same kind, and it stacks   the two are merged, up to the ceiling
 *   anything else                  the two swap places
 *
 * Merging is the reason the ceiling exists. Two of a kind that does not stack
 * cannot be merged, because they are two things -- so those swap like everything
 * else, which is still worth doing: rearranging an inventory is most of what
 * anybody does with one.
 *
 * A merge that does not fit leaves the remainder behind rather than refusing.
 * Pouring 4000 into a wallet holding 3000 with a ceiling of 5000 fills it and
 * leaves 2000 where it was, which is what happens when you pour something.
 *
 * # Rounds onto a weapon
 *
 * One case is neither a merge nor a swap: a box of ammunition dropped onto a
 * weapon that eats that calibre loads it. Picking up the rounds and putting
 * them on the rifle is what somebody means by it, and there is nothing else
 * those two items could sensibly do to each other.
 *
 * It is the same pouring as a merge, so it behaves the same way: as much goes
 * in as the weapon has room for, the rest stays in the box, and a box emptied
 * by it is gone. A calibre that does not match is not an error -- it is two
 * items that have nothing to do with each other, so they swap like anything
 * else.
 *
 * A can of fuel onto a chainsaw is the same click and the same answer. A
 * calibre that is poured is still a calibre, so the only difference underneath
 * is which of the two loading calls has anything to say about it -- and a
 * character filling a chainsaw does it the way they fill a rifle, rather than
 * learning a second gesture for the same idea.
 *
 * # One can into another
 *
 * A litre of petrol in one jerrycan and three in another is two things being
 * carried where there could be one. Dropping either onto the other pours it
 * across and leaves a single can holding four, whichever way round it is done.
 *
 * What is left of the one poured out depends on what it is. A tin goes in the
 * bin with the last of what was in it; a jerrycan is still a jerrycan, and is
 * kept until the character says otherwise on the DISCARD row.
 *
 * It is the merge again, in all but the call -- as much goes over as there is
 * room for, and a can with more than the other can take keeps the rest. Two
 * holding different liquids have nothing to say to each other and swap, because
 * mixing them would need a rule about what the two make and there is none.
 *
 * # Between two menus
 *
 * The same gesture reaches across the screen. A slot picked up in the pockets
 * and put down in a crate is the same click twice and means the same thing, so
 * it does the same four things -- move, merge, load, swap -- between two
 * containers rather than inside one.
 *
 * What changes is that a container can refuse. Moving something out of a crate
 * is the character's to do; taking it off a shop counter is not, and a bin does
 * not hand anything back. core/container/access answers which, and a shop
 * answers for itself through OnPlayerBuyContainerItem -- so a refusal is a
 * sentence to the character rather than a click that did nothing.
 *
 * That reaches inside a single container too. Sliding something from one slot
 * of a shop to another is a take and a put on the same counter, and a character
 * who may not do either may not do both at once -- so only a container that is
 * open both ways can be rearranged at all.
 */

static
    gPlayerSelectedIndex[MAX_PLAYERS] = { -1, ... },
    ContainerMenu:gPlayerSelectedMenu[MAX_PLAYERS] = { INVALID_CONTAINER_MENU_ID, ... },
    Item:gPlayerSelectedItem[MAX_PLAYERS] = { INVALID_ITEM_ID, ... },
    gPlayerPaintedSlot[MAX_PLAYERS] = { -1, ... }
;

/**
 * # Functions
 */

/**
 * @brief      Pick a slot up, waiting for somewhere to put it.
 *
 * What is remembered is the index into the container, not the slot on
 * screen: a slot is a square on a page, and the page can be turned before
 * the second click. The item is remembered too, so a selection whose item
 * moved out from under it can be told apart from one that did not.
 *
 * Selecting again replaces the first selection rather than stacking, and
 * the square goes INVENTORY_SELECTED_COLOUR on whatever page it is on.
 *
 * @param      playerid  Character selecting.
 * @param      menuid    Menu the slot is in.
 * @param      slot      Square on the current page.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the slot is now carried
 *             - `false` when:
 *                 + that menu is not open for them
 *                 + the slot is empty
 */
forward bool:SelectContainerMenuSlot(playerid, ContainerMenu:menuid, slot);

/**
 * @brief      Put a carried slot back down, changing nothing.
 *
 * The green square is repainted to the menu's own colour first, and only
 * on the page the slot is actually on -- slot boxes are global textdraws
 * whose definition lives on per player, so painting blindly leaves the
 * colour on whatever slot is in that square on another page.
 *
 * @param      playerid  Character to clear.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` something was carried and is not any more
 *             - `false` nothing was carried
 */
forward bool:ClearContainerMenuSelection(playerid);

/**
 * @brief      Answer the second click: move, merge, load or swap.
 *
 * What happens depends on what is in the destination:
 *
 *     empty                       the two swap, which moves it
 *     a weapon of that calibre    the rounds are loaded into it
 *     the same build, stackable   poured in, up to the stack limit
 *     anything else               the two swap places
 *
 * A pour that does not fit leaves the remainder in the slot it came from
 * rather than refusing, which is what happens when you pour something.
 *
 * The selection is dropped on every path, including the ones that do
 * nothing: a click that resolves nothing still ends the gesture.
 *
 * The destination does not have to be the menu the slot was picked up in.
 * Across two menus it is the same four answers between two containers, and
 * a container that refuses to be taken from or put into stops it there.
 *
 * The return says the click was spent on a transfer, not that anything
 * moved -- it is what tells the caller not to also open a menu on it.
 *
 * @param      playerid  Character clicking.
 * @param      menuid    Menu clicked in.
 * @param      slot      Square clicked.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the click was the second half of a transfer
 *             - `false` nothing was being carried
 */
forward bool:TransferToContainerMenuSlot(playerid, ContainerMenu:menuid, slot);

/**
 * @brief      Which container index a character is carrying, if any.
 *
 * @param      playerid  Character to read.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the index into the container, counting from zero
 *             - `-1` nothing is being carried
 */
forward GetPlayerSelectedIndex(playerid);
/**
 * @brief      Whether a character has a slot picked up and not yet put down.
 *
 * @param      playerid  Character to read.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` a slot is waiting for somewhere to go
 *             - `false` nothing is being carried
 */
forward bool:IsPlayerCarryingSlot(playerid);

/**
 * # Internal
 */

static stock SlotOfIndexInternal(playerid, ContainerMenu:menuid, index) {
    new const
        slots = GetContainerMenuSlots(menuid),
        page = GetContainerMenuPage(playerid, menuid)
    ;

    if (page == -1) {
        return -1;
    }

    new const
        slot = index - page * slots
    ;

    if (!(0 <= slot < slots)) {
        return -1;
    }

    return slot;
}

static stock UnpaintSelectionInternal(playerid) {
    new const
        slot = gPlayerPaintedSlot[playerid],
        ContainerMenu:menuid = gPlayerSelectedMenu[playerid]
    ;

    gPlayerPaintedSlot[playerid] = -1;

    if (slot == -1 || menuid == INVALID_CONTAINER_MENU_ID) {
        return;
    }

    // Back to whatever the slot belongs at rather than to the menu's own,
    // which is not the same thing once a rare item tints the square it is
    // standing in.
    SetContainerMenuSlotColour(playerid, menuid, slot, GetContainerMenuSlotColour(playerid, menuid, slot));
}

static stock PaintSelectionInternal(playerid) {
    new const
        ContainerMenu:menuid = gPlayerSelectedMenu[playerid]
    ;

    if (menuid == INVALID_CONTAINER_MENU_ID) {
        return;
    }

    new const
        slot = SlotOfIndexInternal(playerid, menuid, gPlayerSelectedIndex[playerid])
    ;

    if (slot == -1) {
        return;
    }

    gPlayerPaintedSlot[playerid] = slot;

    SetContainerMenuSlotColour(playerid, menuid, slot, INVENTORY_SELECTED_COLOUR);
}

/**
 * # External
 */

stock bool:ClearContainerMenuSelection(playerid) {
    if (gPlayerSelectedIndex[playerid] == -1) {
        return false;
    }

    UnpaintSelectionInternal(playerid);

    gPlayerSelectedIndex[playerid] = -1;
    gPlayerSelectedMenu[playerid] = INVALID_CONTAINER_MENU_ID;
    gPlayerSelectedItem[playerid] = INVALID_ITEM_ID;

    return true;
}

stock bool:SelectContainerMenuSlot(playerid, ContainerMenu:menuid, slot) {
    if (!IsPlayerUsingContainerMenu(playerid, menuid)) {
        return false;
    }

    new const
        Item:itemid = GetContainerMenuSlotItem(playerid, menuid, slot),
        index = GetContainerMenuSlotIndex(playerid, menuid, slot)
    ;

    if (itemid == INVALID_ITEM_ID || index == -1) {
        return false;
    }

    ClearContainerMenuSelection(playerid);

    gPlayerSelectedIndex[playerid] = index;
    gPlayerSelectedMenu[playerid] = menuid;
    gPlayerSelectedItem[playerid] = itemid;

    PaintSelectionInternal(playerid);

    return true;
}

stock GetPlayerSelectedIndex(playerid) {
    return gPlayerSelectedIndex[playerid];
}

stock bool:IsPlayerCarryingSlot(playerid) {
    return (gPlayerSelectedIndex[playerid] != -1);
}

static stock bool:ResolveTransferInternal(playerid, Container:containerid, from, to) {
    new const
        Item:source = GetContainerSlotItem(containerid, from),
        Item:target = GetContainerSlotItem(containerid, to)
    ;

    if (source == INVALID_ITEM_ID) {
        return false;
    }

    if (target == INVALID_ITEM_ID) {
        return SwapContainerSlots(containerid, from, to);
    }

    if (LoadItemWeaponFromAmmunition(target, source) != 0) {
        if (GetItemAmmunitionRounds(source) <= 0) {
            RemoveItemFromContainer(containerid, from, .playerid = playerid);
            DestroyItem(source);
        }

        return true;
    }

    if (LoadItemWeaponFromLiquid(target, source) != 0) {
        // An emptied vessel is kept or not according to what it is, which the
        // build already answers: a jerrycan is a jerrycan whether or not there
        // is petrol in it today, and a tin of cola is rubbish. One that is kept
        // is the character's to be rid of, on the DISCARD row.
        if (GetItemLiquidAmount(source) <= 0 && !IsItemLiquidContainerReusable(source)) {
            RemoveItemFromContainer(containerid, from, .playerid = playerid);
            DestroyItem(source);
        }

        return true;
    }

    if (PourItemLiquid(target, source) != 0) {
        if (GetItemLiquidAmount(source) <= 0 && !IsItemLiquidContainerReusable(source)) {
            RemoveItemFromContainer(containerid, from, .playerid = playerid);
            DestroyItem(source);
        }

        return true;
    }

    new
        space = 0
    ;

    if (GetItemBuild(source) == GetItemBuild(target)) {
        space = GetItemStackSpace(target);
    }

    if (space > 0) {
        new const
            held = GetItemAmountInt(source)
        ;

        new
            moved = held
        ;

        if (moved > space) {
            moved = space;
        }

        SetItemAmountInt(target, GetItemAmountInt(target) + moved);

        if (moved == held) {
            DestroyItem(source);

            return true;
        }

        SetItemAmountInt(source, held - moved);

        return true;
    }

    return SwapContainerSlots(containerid, from, to);
}


// The same four answers as above, between two different containers. It is a
// separate function rather than a branch in that one because every move here is
// a remove and an add where the other is a swap of two indexes, and the two
// have nothing in common but the order they ask their questions in.
//
// What is allowed at all was settled before this was called. This is only how.
static stock bool:ResolveCrossTransferInternal(playerid, Container:from, fromIndex, Container:to, toIndex) {
    new const
        Item:source = GetContainerSlotItem(from, fromIndex),
        Item:target = GetContainerSlotItem(to, toIndex)
    ;

    if (source == INVALID_ITEM_ID) {
        return false;
    }

    if (target == INVALID_ITEM_ID) {
        if (!RemoveItemFromContainer(from, fromIndex, .playerid = playerid)) {
            return false;
        }

        // Putting it back where it came from is the only thing left if the far
        // side refuses it, and it cannot refuse: the slot it came out of is
        // still empty. Checked anyway, because the alternative to checking is
        // an item that exists and is nowhere.
        if (!AddItemToContainer(to, source, .addToIndex = toIndex, .playerid = playerid)) {
            AddItemToContainer(from, source, .addToIndex = fromIndex, .playerid = playerid);

            return false;
        }

        return true;
    }

    if (LoadItemWeaponFromAmmunition(target, source) != 0) {
        if (GetItemAmmunitionRounds(source) <= 0) {
            RemoveItemFromContainer(from, fromIndex, .playerid = playerid);
            DestroyItem(source);
        }

        return true;
    }

    if (LoadItemWeaponFromLiquid(target, source) != 0) {
        if (GetItemLiquidAmount(source) <= 0 && !IsItemLiquidContainerReusable(source)) {
            RemoveItemFromContainer(from, fromIndex, .playerid = playerid);
            DestroyItem(source);
        }

        return true;
    }

    if (PourItemLiquid(target, source) != 0) {
        if (GetItemLiquidAmount(source) <= 0 && !IsItemLiquidContainerReusable(source)) {
            RemoveItemFromContainer(from, fromIndex, .playerid = playerid);
            DestroyItem(source);
        }

        return true;
    }

    new
        space = 0
    ;

    if (GetItemBuild(source) == GetItemBuild(target)) {
        space = GetItemStackSpace(target);
    }

    if (space > 0) {
        new const
            held = GetItemAmountInt(source)
        ;

        new
            moved = held
        ;

        if (moved > space) {
            moved = space;
        }

        SetItemAmountInt(target, GetItemAmountInt(target) + moved);

        if (moved == held) {
            RemoveItemFromContainer(from, fromIndex, .playerid = playerid);
            DestroyItem(source);

            return true;
        }

        SetItemAmountInt(source, held - moved);

        return true;
    }

    // Two things that have nothing to do with each other change places, which
    // across two containers means both slots are emptied before either is
    // filled. Done the other way round the second add has nowhere to go.
    if (!RemoveItemFromContainer(from, fromIndex, .playerid = playerid)) {
        return false;
    }

    if (!RemoveItemFromContainer(to, toIndex, .playerid = playerid)) {
        AddItemToContainer(from, source, .addToIndex = fromIndex, .playerid = playerid);

        return false;
    }

    AddItemToContainer(to, source, .addToIndex = toIndex, .playerid = playerid);
    AddItemToContainer(from, target, .addToIndex = fromIndex, .playerid = playerid);

    return true;
}

// Whether the character is allowed to move this at all, which is the container
// it is leaving and the one it is arriving in, and never the item.
//
// A shop is asked last and separately. The other three are a read of one cell;
// this one is a callback with a price and a wallet behind it, and it is only
// worth spending once everything cheaper has agreed.
static stock bool:AllowTransferInternal(playerid, Container:from, fromIndex, Container:to) {
    // Moving something about inside one container is taking it out and putting
    // it back, so it wants both answers and a shop only gives one of them.
    // Without this a counter cannot be bought from twice but can be tidied,
    // which is the same reach into it wearing a different hat.
    if (from == to) {
        if (CanTakeFromContainer(from) && CanPutInContainer(from)) {
            return true;
        }

        SendPlayerNotice(playerid, "That is not yours to rearrange.");

        return false;
    }

    if (!CanTakeFromContainer(from)) {
        SendPlayerNotice(playerid, "Nothing comes back out of that.");

        return false;
    }

    if (!CanPutInContainer(to)) {
        SendPlayerNotice(playerid, "That does not go in there.");

        return false;
    }

    new const
        Item:itemid = GetContainerSlotItem(from, fromIndex)
    ;

    if (itemid == INVALID_ITEM_ID) {
        return false;
    }

    // A station is a workspace: what goes on it is what is made there.
    if (!CanPutBuildInCraftStation(to, GetItemBuild(itemid))) {
        new
            station[MAX_CRAFT_STATION_NAME]
        ;

        GetCraftStationName(GetContainerCraftStation(to), station);

        SendPlayerNotice(playerid, "Nothing is made out of that on the %s.", station);

        return false;
    }

    if (!IsContainerShop(from)) {
        return true;
    }

    // Nothing has moved yet, so a shop that says no leaves the counter exactly
    // as it was and does not have to put anything back.
    return !!CallLocalFunction("OnPlayerBuyContainerItem", "iiii", playerid, _:from, _:itemid, fromIndex);
}

stock bool:TransferToContainerMenuSlot(playerid, ContainerMenu:menuid, slot) {
    if (!IsPlayerCarryingSlot(playerid)) {
        return false;
    }

    // Everything the selection knows is read out before it is let go of,
    // including what was picked up -- clearing it sets that back to nothing,
    // and a check made afterwards would be comparing against nothing.
    new const
        ContainerMenu:fromMenu = gPlayerSelectedMenu[playerid],
        Container:from = GetContainerMenuContainer(playerid, fromMenu),
        Container:to = GetContainerMenuContainer(playerid, menuid),
        Item:carried = gPlayerSelectedItem[playerid],
        fromIndex = gPlayerSelectedIndex[playerid],
        toIndex = GetContainerMenuSlotIndex(playerid, menuid, slot)
    ;

    // The selection is spent on every path below, including the ones that do
    // nothing, so it is dropped once here rather than before each return.
    ClearContainerMenuSelection(playerid);

    if (from == INVALID_CONTAINER_ID || to == INVALID_CONTAINER_ID) {
        return false;
    }

    // The item that was picked up has to still be where it was picked up from.
    // A page turned, a crate emptied by somebody else standing at it, an item
    // eaten out from under the selection -- the index would still be a real
    // slot and would be the wrong one.
    if (GetContainerSlotItem(from, fromIndex) != carried) {
        return false;
    }

    if (toIndex == -1) {
        return true;
    }

    // Putting it back where it came from is not a move and is not refused
    // either, whatever the container is -- nothing happens and the character
    // has simply changed their mind.
    if (from == to && fromIndex == toIndex) {
        return true;
    }

    if (!AllowTransferInternal(playerid, from, fromIndex, to)) {
        return true;
    }

    if (from == to) {
        if (!ResolveTransferInternal(playerid, from, fromIndex, toIndex)) {
            return true;
        }

        RefreshContainerMenu(playerid, menuid);
        PlayerPlaySound(playerid, INVENTORY_MOVE_SOUND);

        return true;
    }

    if (!ResolveCrossTransferInternal(playerid, from, fromIndex, to, toIndex)) {
        return true;
    }

    // Both menus are redrawn by the framework as the items land, because each
    // add and remove reaches everybody looking at that container -- which is
    // the other half of what makes two menus work at all: somebody else
    // standing at the same crate sees it change too.
    PlayerPlaySound(playerid, INVENTORY_MOVE_SOUND);

    return true;
}

/**
 * # Calls
 */

hook OnPlayerCancelContainerMenu(playerid, ContainerMenu:menuid) {
    ClearContainerMenuSelection(playerid);

    return 0;
}

hook OnPlayerSelectItemAction(playerid, ItemAction:actionid, Item:itemid, ContainerMenu:menuid, slot) {
    ClearContainerMenuSelection(playerid);

    return 0;
}

hook OnPlayerChangeContainerMenuPage(playerid, ContainerMenu:menuid, page) {
    if (gPlayerSelectedMenu[playerid] != menuid) {
        return 0;
    }

    UnpaintSelectionInternal(playerid);
    PaintSelectionInternal(playerid);

    new const
        slot = SlotOfIndexInternal(playerid, menuid, gPlayerSelectedIndex[playerid])
    ;

    if (slot == -1) {
        return 0;
    }

    if (GetContainerMenuSlotItem(playerid, menuid, slot) != gPlayerSelectedItem[playerid]) {
        return 0;
    }

    ShowItemActionMenu(playerid, menuid, slot);

    return 0;
}

hook OnPlayerConnect(playerid) {
    gPlayerPaintedSlot[playerid] = -1;
    gPlayerSelectedIndex[playerid] = -1;
    gPlayerSelectedMenu[playerid] = INVALID_CONTAINER_MENU_ID;
    gPlayerSelectedItem[playerid] = INVALID_ITEM_ID;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    gPlayerPaintedSlot[playerid] = -1;
    gPlayerSelectedIndex[playerid] = -1;
    gPlayerSelectedMenu[playerid] = INVALID_CONTAINER_MENU_ID;
    gPlayerSelectedItem[playerid] = INVALID_ITEM_ID;

    return 0;
}
