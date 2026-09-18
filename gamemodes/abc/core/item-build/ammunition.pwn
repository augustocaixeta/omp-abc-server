#if defined _CORE_ITEM_BUILD_AMMUNITION
    #endinput
#endif
#define _CORE_ITEM_BUILD_AMMUNITION

#include <pp-hooks>

/**
 * # Boxes of ammunition
 *
 * A kind of item that is rounds of one calibre. A box of 7.62mm, a packet of
 * buckshot, a strip of .357 -- the same shape of thing, differing only in what
 * calibre it is and how many are in a full one.
 *
 * What is in a particular box is its amount, so the corner of its slot shows
 * how many are left without anybody drawing anything -- the same arrangement a
 * bottle of pills has, for the same reason.
 *
 * A box is never fired from. It is poured into a weapon's reserve by
 * core/item-build/weapon and shrinks as it goes; an empty one is gone. That is
 * the whole of what ammunition does here: there is no ballistics layer, and
 * nothing reads a round except to count it.
 *
 * # Why this is not simply a stack
 *
 * core/item-build/stack already knows how to split a number across two items,
 * and a box of rounds is a number. The difference is the calibre: two stacks of
 * cash are interchangeable and two boxes of ammunition are not, so merging and
 * splitting have to ask a question stack does not know how to ask. They are
 * kept apart until something actually wants a box split, at which point this is
 * where the calibre check would go.
 */

static
    Calibre:gItemBuildAmmunitionCalibre[MAX_ITEM_BUILDS] = { INVALID_CALIBRE_ID, ... },
    gItemBuildAmmunitionRounds[MAX_ITEM_BUILDS],

    // Which kind of box a calibre comes in, which is the question unloading
    // asks: rounds coming out of a rifle have to become something, and this is
    // what they become. The first build registered for a calibre wins, so a
    // catalogue declaring two kinds of 9mm box pours loose rounds into the one
    // it named first.
    ItemBuild:gCalibreAmmunitionBuild[MAX_CALIBRES] = { INVALID_ITEM_BUILD_ID, ... }
;

/**
 * # Functions
 */

/**
 * @brief      Say that a kind of item is a box of ammunition.
 *
 * The rounds become the item's amount, so a box made by anything -- the
 * admin, a shipment, a present -- turns up full without any of those knowing
 * what a full box is.
 *
 * The slot shows the calibre beside the count, worked out when it is drawn
 * rather than kept, so nothing has to be rewritten when a box is poured into
 * a rifle.
 *
 * @param      buildid    Build to make ammunition.
 * @param      calibreid  What calibre it is.
 * @param      rounds     How many are in a full one.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the build is ammunition
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + `calibreid` is not a registered calibre
 *                 + `calibreid` is poured rather than carried, which belongs
 *                   in a vessel and not in a box
 *                 + `rounds` is zero or below
 */
forward bool:DefineItemBuildAmmunition(ItemBuild:buildid, Calibre:calibreid, rounds = 30);

/**
 * @brief      Whether a kind of item is a box of ammunition.
 *
 * @param      buildid  Build to ask.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it was made ammunition
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + it is some other kind of thing
 */
forward bool:IsItemBuildAmmunition(ItemBuild:buildid);

/**
 * @brief      Whether an item is a box of ammunition.
 *
 * @param      itemid  Item to ask.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` its build is ammunition
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + its build is some other kind of thing
 */
forward bool:IsItemAmmunition(Item:itemid);

/**
 * @brief      What calibre a kind of box holds.
 *
 * @param      buildid  Build to read.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - the calibre it was defined with
 *             - `INVALID_CALIBRE_ID` when the build is not ammunition
 */
forward Calibre:GetItemBuildAmmunitionCalibre(ItemBuild:buildid);

/**
 * @brief      What calibre a particular box holds.
 *
 * @param      itemid  Item to read.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - its calibre
 *             - `INVALID_CALIBRE_ID` when the item is not ammunition
 */
forward Calibre:GetItemAmmunitionCalibre(Item:itemid);

/**
 * @brief      How many rounds a full box of this kind holds.
 *
 * @param      buildid  Build to read.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - the rounds it was defined with
 *             - `0` when the build is not ammunition
 */
forward GetItemBuildAmmunitionRounds(ItemBuild:buildid);

/**
 * @brief      How many rounds are left in a particular box.
 *
 * A box nobody has drawn from answers its build's full count rather than
 * nothing, so one just made and one never touched read the same.
 *
 * @param      itemid  Box to count.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many are left
 *             - `0` when it is not ammunition, or is empty
 */
forward GetItemAmmunitionRounds(Item:itemid);

/**
 * @brief      Say how many rounds are left in a box.
 *
 * Emptying one does not destroy it: whatever drew the last round decides
 * whether the box is thrown away, because that is a gameplay question and
 * this is a record.
 *
 * @param      itemid  Box to write.
 * @param      rounds  How many are left, clamped to nothing below zero.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the count was written
 *             - `false` `itemid` is not ammunition
 */
forward bool:SetItemAmmunitionRounds(Item:itemid, rounds);

/**
 * @brief      A box of a calibre in a character's pockets.
 *
 * The first one found with anything left in it. What a reload reaches for,
 * and what tells a prompt whether there is anything to reach for at all.
 *
 * @param      playerid   Character to search.
 * @param      calibreid  Calibre to look for.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - the first box of that calibre with rounds in it
 *             - `INVALID_ITEM_ID` when:
 *                 + they have no inventory
 *                 + `calibreid` is not a registered calibre
 *                 + they are carrying none of it
 */
forward Item:FindPlayerAmmunition(playerid, Calibre:calibreid);

/**
 * @brief      How much of a calibre a character is carrying.
 *
 * Every box in their pockets added up, which is the number an arsenal is
 * really asking for: not how many boxes, but how many rounds.
 *
 * @param      playerid   Character to count.
 * @param      calibreid  Calibre to count.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many rounds of it they have
 *             - `0` when:
 *                 + they have no inventory
 *                 + `calibreid` is not a registered calibre
 *                 + they are carrying none of it
 */
forward GetPlayerAmmunitionCount(playerid, Calibre:calibreid);

/**
 * @brief      Which kind of box a calibre comes in.
 *
 * The first build registered for it. What unloading a weapon asks, because
 * rounds taken out of one have to become something again.
 *
 * @param      calibreid  Calibre to look up.
 *
 * @date       22:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - the build its boxes are made of
 *             - `INVALID_ITEM_BUILD_ID` when:
 *                 + `calibreid` is not a registered calibre
 *                 + no box was ever declared for it
 */
forward ItemBuild:GetCalibreAmmunitionBuild(Calibre:calibreid);

/**
 * @brief      Put loose rounds into a character's pockets as boxes.
 *
 * Partly empty boxes of that calibre are topped up first, so unloading a
 * rifle into pockets already holding half a box fills that one rather than
 * leaving two halves. Whatever is left over becomes new boxes.
 *
 * Takes as much as the pockets will hold and answers what would not fit, so
 * a caller emptying a weapon can leave the remainder in it rather than
 * destroying rounds nobody has room for.
 *
 * @param      playerid   Character to give them to.
 * @param      calibreid  What calibre they are.
 * @param      rounds     How many are being handed over.
 *
 * @date       22:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many rounds would not fit
 *             - `rounds` unchanged when:
 *                 + `calibreid` has no box declared for it
 *                 + they have no inventory
 */
forward GivePlayerAmmunition(playerid, Calibre:calibreid, rounds);

/**
 * @brief      Take rounds out of a box.
 *
 * Draws as many as are actually there, so a caller asking for thirty from a
 * box of twelve gets twelve and the box is empty rather than owed eighteen.
 *
 * @param      itemid  Box to draw from.
 * @param      rounds  How many are wanted.
 *
 * @date       00:40 11/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many were actually drawn
 *             - `0` when:
 *                 + `itemid` is not ammunition
 *                 + it is empty
 *                 + `rounds` is zero or below
 */
forward DrawItemAmmunition(Item:itemid, rounds);

/**
 * # External
 */

stock bool:DefineItemBuildAmmunition(ItemBuild:buildid, Calibre:calibreid, rounds = 30) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    if (!IsValidCalibre(calibreid)) {
        return false;
    }

    if (IsCalibreLiquid(calibreid)) {
        return false;
    }

    if (rounds <= 0) {
        return false;
    }

    gItemBuildAmmunitionCalibre[buildid] = calibreid;
    gItemBuildAmmunitionRounds[buildid] = rounds;

    if (gCalibreAmmunitionBuild[calibreid] == INVALID_ITEM_BUILD_ID) {
        gCalibreAmmunitionBuild[calibreid] = buildid;
    }

    SetItemBuildAmountType(buildid, ITEM_AMOUNT_CUSTOM);

    return true;
}

stock ItemBuild:GetCalibreAmmunitionBuild(Calibre:calibreid) {
    if (!IsValidCalibre(calibreid)) {
        return INVALID_ITEM_BUILD_ID;
    }

    return gCalibreAmmunitionBuild[calibreid];
}

stock GivePlayerAmmunition(playerid, Calibre:calibreid, rounds) {
    if (rounds <= 0) {
        return 0;
    }

    new const
        ItemBuild:buildid = GetCalibreAmmunitionBuild(calibreid)
    ;

    if (buildid == INVALID_ITEM_BUILD_ID) {
        return rounds;
    }

    if (!HasPlayerInventory(playerid)) {
        return rounds;
    }

    new const
        full = gItemBuildAmmunitionRounds[buildid],
        capacity = GetPlayerInventoryCapacity(playerid)
    ;

    for (new i; i != capacity && rounds > 0; ++i) {
        new const
            Item:itemid = GetPlayerInventorySlotItem(playerid, i)
        ;

        if (itemid == INVALID_ITEM_ID) {
            continue;
        }

        if (GetItemAmmunitionCalibre(itemid) != calibreid) {
            continue;
        }

        new const
            held = GetItemAmmunitionRounds(itemid),
            room = full - held
        ;

        if (room <= 0) {
            continue;
        }

        new const
            moved = (rounds < room) ? rounds : room
        ;

        SetItemAmmunitionRounds(itemid, held + moved);

        rounds -= moved;
    }

    while (rounds > 0) {
        if (IsPlayerInventoryFull(playerid)) {
            return rounds;
        }

        new const
            Item:itemid = CreateItem(buildid)
        ;

        if (itemid == INVALID_ITEM_ID) {
            return rounds;
        }

        new const
            moved = (rounds < full) ? rounds : full
        ;

        SetItemAmmunitionRounds(itemid, moved);

        if (!AddItemToInventory(playerid, itemid)) {
            DestroyItem(itemid);

            return rounds;
        }

        rounds -= moved;
    }

    return 0;
}

stock bool:IsItemBuildAmmunition(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    return gItemBuildAmmunitionCalibre[buildid] != INVALID_CALIBRE_ID;
}

stock bool:IsItemAmmunition(Item:itemid) {
    return IsItemBuildAmmunition(GetItemBuild(itemid));
}

stock Calibre:GetItemBuildAmmunitionCalibre(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return INVALID_CALIBRE_ID;
    }

    return gItemBuildAmmunitionCalibre[buildid];
}

stock Calibre:GetItemAmmunitionCalibre(Item:itemid) {
    return GetItemBuildAmmunitionCalibre(GetItemBuild(itemid));
}

stock GetItemBuildAmmunitionRounds(ItemBuild:buildid) {
    if (!IsItemBuildAmmunition(buildid)) {
        return 0;
    }

    return gItemBuildAmmunitionRounds[buildid];
}

stock GetItemAmmunitionRounds(Item:itemid) {
    if (!IsItemAmmunition(itemid)) {
        return 0;
    }

    if (!HasItemAmountValue(itemid)) {
        return gItemBuildAmmunitionRounds[GetItemBuild(itemid)];
    }

    return GetItemAmountInt(itemid);
}

stock bool:SetItemAmmunitionRounds(Item:itemid, rounds) {
    if (!IsItemAmmunition(itemid)) {
        return false;
    }

    return SetItemAmountInt(itemid, rounds < 0 ? 0 : rounds);
}

stock Item:FindPlayerAmmunition(playerid, Calibre:calibreid) {
    if (!IsValidCalibre(calibreid)) {
        return INVALID_ITEM_ID;
    }

    new const
        capacity = GetPlayerInventoryCapacity(playerid)
    ;

    for (new i; i != capacity; ++i) {
        new const
            Item:itemid = GetPlayerInventorySlotItem(playerid, i)
        ;

        if (itemid == INVALID_ITEM_ID) {
            continue;
        }

        if (GetItemAmmunitionCalibre(itemid) != calibreid) {
            continue;
        }

        if (GetItemAmmunitionRounds(itemid) <= 0) {
            continue;
        }

        return itemid;
    }

    return INVALID_ITEM_ID;
}

stock GetPlayerAmmunitionCount(playerid, Calibre:calibreid) {
    if (!IsValidCalibre(calibreid)) {
        return 0;
    }

    new
        rounds
    ;

    new const
        capacity = GetPlayerInventoryCapacity(playerid)
    ;

    for (new i; i != capacity; ++i) {
        new const
            Item:itemid = GetPlayerInventorySlotItem(playerid, i)
        ;

        if (itemid == INVALID_ITEM_ID) {
            continue;
        }

        if (GetItemAmmunitionCalibre(itemid) != calibreid) {
            continue;
        }

        rounds += GetItemAmmunitionRounds(itemid);
    }

    return rounds;
}

stock DrawItemAmmunition(Item:itemid, rounds) {
    if (!IsItemAmmunition(itemid)) {
        return 0;
    }

    if (rounds <= 0) {
        return 0;
    }

    new const
        left = GetItemAmmunitionRounds(itemid)
    ;

    if (left <= 0) {
        return 0;
    }

    new const
        drawn = (rounds < left) ? rounds : left
    ;

    SetItemAmmunitionRounds(itemid, left - drawn);

    return drawn;
}

/**
 * # Calls
 */

hook OnItemCreate(Item:itemid) {
    new const
        rounds = gItemBuildAmmunitionRounds[GetItemBuild(itemid)]
    ;

    if (rounds == 0) {
        return 0;
    }

    SetItemAmountInt(itemid, rounds);

    return 0;
}

hook OnItemAmountFormat(Item:itemid, ItemBuild:buildid, output[], size) {
    if (!IsItemBuildAmmunition(buildid)) {
        return 0;
    }

    new
        calibre[MAX_CALIBRE_NAME]
    ;

    GetCalibreName(gItemBuildAmmunitionCalibre[buildid], calibre);
    format(output, size, ITEM_AMMUNITION_FORMAT, GetItemAmmunitionRounds(itemid), calibre);

    return 1;
}
