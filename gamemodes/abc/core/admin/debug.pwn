#if defined _CORE_ADMIN_DEBUG
    #endinput
#endif
#define _CORE_ADMIN_DEBUG

#include <pp-hooks>

/**
 * # Looking at what happened
 *
 * Not gameplay. Counters, raw key values and a live preview
 * mover: the things that turn a disagreement about what the server is doing
 * into one measurement.
 *
 * The dev prefix on the commands is what tells anybody using the gamemode that
 * these are not part of it. Everything under this folder is expected to come
 * out, and taking it out is deleting the folder and its lines from main.
 */

CMD:devstats(playerid, params[]) {
    SendClientMessage(playerid, -1, "kinds %i, items %i, containers %i, buttons %i, lockers %i", CountItemBuild(), GetItemCount(), GetContainerCount(), GetButtonCount(), GetLockerCount());
    SendClientMessage(playerid, -1, "inventory %i/%i, durations pending %i", GetPlayerInventorySize(playerid), GetPlayerInventoryCapacity(playerid), GetPendingItemDurationCount());

    return 1;
}

// What the item says against what the game says. The two are only read back
// into each other at the boundaries -- drawing, putting away, dropping, dying --
// so anywhere else they are allowed to differ, and this is how to see by how
// much before deciding whether that is a bug.
CMD:devweapon(playerid, params[]) {
    SyncPlayerWeaponAmmo(playerid);

    SendClientMessage(playerid, -1, "--- carrying ---");

    new
        carried
    ;

    for (new WEAPON_SLOT:slot; slot != MAX_WEAPON_SLOTS; ++slot) {
        new const
            Item:itemid = GetPlayerWeaponSlotItem(playerid, slot)
        ;

        if (itemid == INVALID_ITEM_ID) {
            continue;
        }

        ++carried;

        new
            name[MAX_ITEM_NAME],
            text[MAX_ITEM_AMOUNT_LENGTH]
        ;

        GetItemName(itemid, name);
        FormatItemAmount(itemid, text);

        SendClientMessage(playerid, -1, "slot %i: %s  item '%s'  game ammo %i%s",
            _:slot, name, text, GetPlayerAmmo(playerid),
            (GetPlayerArmedItem(playerid) == itemid) ? "  <- in hand" : "");
    }

    if (carried == 0) {
        SendClientMessage(playerid, -1, "nothing");
    }

    SendClientMessage(playerid, -1, "--- in pockets ---");

    for (new Calibre:calibreid; calibreid != GetCalibreCount(); ++calibreid) {
        new const
            rounds = GetPlayerAmmunitionCount(playerid, calibreid)
        ;

        if (rounds == 0) {
            continue;
        }

        new
            calibre[MAX_CALIBRE_NAME]
        ;

        GetCalibreName(calibreid, calibre);

        SendClientMessage(playerid, -1, "%s: %i rounds", calibre, rounds);
    }

    return 1;
}

CMD:devjson(playerid, params[]) {
    new const
        Item:itemid = GetPlayerInventorySlotItem(playerid, 0)
    ;

    if (itemid == INVALID_ITEM_ID) {
        SendClientMessage(playerid, -1, "Slot 0 is empty. /devadd first.");

        return 1;
    }

    SetItemExtraData(itemid, "durability", var_new(random(100)));
    SetItemExtraData(itemid, "owner", var_new_str("tester"));

    new const
        Node:node = JSON_Object()
    ;

    SerializeItem(itemid, node);

    new
        output[MAX_JSON_BUFFER_LENGTH]
    ;

    JSON_Stringify(node, output);

    SendClientMessage(playerid, -1, "Item %i:", _:itemid);
    SendClientMessage(playerid, -1, output);

    new const
        Item:copy = CreateItem(GetItemBuild(itemid))
    ;

    DeserializeItem(copy, node);

    new
        text[MAX_ITEM_AMOUNT_LENGTH]
    ;

    FormatItemAmount(copy, text);

    SendClientMessage(playerid, -1, "Copy %i restored, durability %i, keys %i.", _:copy, GetItemExtraDataInt(copy, "durability"), GetItemExtraDataCount(copy));
    SendClientMessage(playerid, -1, "Amount: %s", text);

    DestroyItem(copy);

    return 1;
}

static
    bool:gDevWatchingKeys[MAX_PLAYERS]
;

CMD:devkeys(playerid, params[]) {
    gDevWatchingKeys[playerid] = !gDevWatchingKeys[playerid];

    SendClientMessage(playerid, -1, gDevWatchingKeys[playerid] ? ("Watching keys. Press Y and N.") : ("Not watching keys."));

    return 1;
}

hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if (!gDevWatchingKeys[playerid]) {
        return 0;
    }

    SendClientMessage(playerid, -1, "keys %i  (Y is %i, N is %i)", _:newkeys, _:KEY_YES, _:KEY_NO);

    return 0;
}

hook OnPlayerConnect(playerid) {
    gDevWatchingKeys[playerid] = false;

    return 0;
}

CMD:devpreview(playerid, params[]) {
    new
        Float:x,
        Float:y,
        Float:size
    ;

    if (sscanf(params, "ffF(0.0)", x, y, size)) {
        SendClientMessage(playerid, -1, "Usage: /devpreview <offsetX> <offsetY> [size] -- from centred, not a screen position.");

        return 1;
    }

    new const
        Item:itemid = GetPlayerInventorySlotItem(playerid, 0)
    ;

    if (itemid == INVALID_ITEM_ID) {
        SendClientMessage(playerid, -1, "Slot 0 is empty. /devadd first.");

        return 1;
    }

    new const
        ItemBuild:buildid = GetItemBuild(itemid)
    ;

    new
        Float:sx,
        Float:sy,
        Float:rx,
        Float:ry,
        Float:rz,
        Float:zoom,
        Float:ox,
        Float:oy,
        name[MAX_ITEM_BUILD_NAME]
    ;

    GetItemBuildPreviewSettings(buildid, sx, sy, rx, ry, rz, zoom, ox, oy);
    GetItemBuildName(buildid, name);

    if (size > 0.0) {
        sx = size;
        sy = size;
    }

    SetItemBuildPreviewSettings(buildid, sx, sy, rx, ry, rz, zoom, x, y);

    HidePlayerInventory(playerid);
    ShowPlayerInventory(playerid);

    SendClientMessage(playerid, -1, "%s: size %.1f, offset %.1f,%.1f from centred (was %.1f,%.1f).", name, sx, x, y, ox, oy);

    return 1;
}

/**
 * The hold bar, resized where it stands, with a slow one started so it can be
 * looked at. For finding numbers by eye: what a textdraw box does at very small
 * letter sizes is not something this side can work out, and two goes at
 * reasoning about it both came out wrong.
 *
 * Nothing here is kept. Whatever looks right goes into hud.pwn by hand.
 */
/**
 * Every animation the character goes through for the next few seconds, by index
 * and by name.
 *
 * For anything that has to know what somebody is actually doing rather than
 * what key they are holding -- core/item-build/fuel asks this of a chainsaw,
 * because a finger on the trigger while falling off a roof is not sawing.
 *
 * It watches rather than answering once, because the thing worth reading is
 * what plays *while* something is happening, and a command cannot be typed
 * with a trigger held down. Start it, go and do the thing, and read the list
 * afterwards.
 *
 * Only changes are printed. Standing still is one line, not fifty.
 *
 * It is also how to find out whether a library somebody told you about exists
 * at all: an animation that never plays never prints.
 */

static
    gDevAnimTimer[MAX_PLAYERS] = { -1, ... },
    gDevAnimLast[MAX_PLAYERS] = { -1, ... },
    gDevAnimLeft[MAX_PLAYERS]
;

forward @DevAnim_Sample(playerid);

static stock StopDevAnimInternal(playerid) {
    if (gDevAnimTimer[playerid] == -1) {
        return;
    }

    KillTimer(gDevAnimTimer[playerid]);

    gDevAnimTimer[playerid] = -1;
    gDevAnimLast[playerid] = -1;
    gDevAnimLeft[playerid] = 0;
}

public @DevAnim_Sample(playerid) {
    if (!IsPlayerConnected(playerid)) {
        StopDevAnimInternal(playerid);

        return;
    }

    new const
        index = GetPlayerAnimationIndex(playerid)
    ;

    if (index != gDevAnimLast[playerid]) {
        gDevAnimLast[playerid] = index;

        new
            library[32],
            name[32]
        ;

        if (GetAnimationName(index, library, sizeof (library), name, sizeof (name))) {
            SendClientMessage(playerid, -1, "  %i  %s / %s", index, library, name);
        } else {
            SendClientMessage(playerid, -1, "  %i  (no name)", index);
        }
    }

    if (--gDevAnimLeft[playerid] > 0) {
        return;
    }

    StopDevAnimInternal(playerid);

    SendClientMessage(playerid, -1, "Done watching.");
}

CMD:devanim(playerid, params[]) {
    if (gDevAnimTimer[playerid] != -1) {
        StopDevAnimInternal(playerid);

        SendClientMessage(playerid, -1, "Stopped watching.");

        return 1;
    }

    new
        seconds
    ;

    if (sscanf(params, "D(10)", seconds) || seconds <= 0) {
        seconds = 10;
    }

    gDevAnimLeft[playerid] = seconds * 1000 / DEV_ANIM_STEP;
    gDevAnimLast[playerid] = -1;
    gDevAnimTimer[playerid] = SetTimerEx("@DevAnim_Sample", DEV_ANIM_STEP, true, "i", playerid);

    SendClientMessage(playerid, -1, "Watching animations for %i seconds. Go and do the thing.", seconds);

    return 1;
}

CMD:devbar(playerid, params[]) {
    new
        Float:height,
        Float:padY,
        Float:padX
    ;

    new
        Float:x,
        Float:y
    ;

    if (sscanf(params, "fF(1.5)F(0.0)F(-1.0)F(-1.0)", height, padY, padX, x, y)) {
        SendClientMessage(playerid, -1, "Usage: /devbar <height> [padY] [padX] [x] [y]");
        SendClientMessage(playerid, -1, "Now %.2f tall, padding %.2f x %.2f, at %.1f,%.1f.",
            HOLD_ACTION_BAR_HEIGHT, HOLD_ACTION_BAR_PADDING_X, HOLD_ACTION_BAR_PADDING_Y,
            HOLD_ACTION_BAR_X, HOLD_ACTION_BAR_Y);

        return 1;
    }

    new const
        PlayerBar:barid = GetPlayerHoldActionBar(playerid)
    ;

    if (barid == INVALID_PLAYER_BAR_ID) {
        SendClientMessage(playerid, -1, "No bar.");

        return 1;
    }

    StopHoldAction(playerid);

    SetPlayerProgressBarHeight(playerid, barid, height);
    SetPlayerProgressBarPadding(playerid, barid, padX, padY);

    // Left where it is unless a position was actually given, so the size can be
    // tried on its own without the bar jumping somewhere else.
    if (x >= 0.0 && y >= 0.0) {
        SetPlayerProgressBarPos(playerid, barid, x, y);
    }

    // Draining and on no key, so it sits there for half a minute being looked
    // at rather than needing something held down.
    StartHoldAction(playerid, 30000, "GASOLINE", KEY_NONE, 99, true);

    SendClientMessage(playerid, -1, "Bar %.2f tall, padding %.2f x %.2f, fill %.2f.",
        height, padX, padY, height - padY * 2.0);

    return 1;
}

CMD:devslotcolour(playerid, params[]) {
    new
        slot,
        colour
    ;

    if (sscanf(params, "dx", slot, colour)) {
        SendClientMessage(playerid, -1, "Usage: /devslotcolour <slot> <RRGGBBAA>");

        return 1;
    }

    if (!SetContainerMenuSlotColour(playerid, gInventoryMenu, slot, colour)) {
        SendClientMessage(playerid, -1, "Not looking at that menu.");

        return 1;
    }

    SendClientMessage(playerid, -1, "Slot %i painted. Default is %x.", slot, GetContainerMenuColour(gInventoryMenu));

    return 1;
}

// Proves the round trip an item with an inside has to survive: fill a crate,
// turn it into text, build a second one back out of that text, and count what
// came across. Nothing here is gameplay -- it is the save path being read out
// loud before there is anywhere to save to.
CMD:devinside(playerid, params[]) {
    new const
        Item:crate = CreateItem(gItemBuildSmallCrate)
    ;

    if (crate == INVALID_ITEM_ID) {
        SendClientMessage(playerid, -1, "Could not make a crate.");

        return 1;
    }

    new const
        Container:containerid = GetItemInside(crate)
    ;

    for (new i; i != 3; ++i) {
        new const
            Item:contained = CreateItem(gItemBuildPills)
        ;

        SetItemAmountInt(contained, 10 + i);
        SetItemExtraData(contained, "batch", var_new(700 + i));

        if (!AddItemToContainer(containerid, contained)) {
            DestroyItem(contained);
        }
    }

    new const
        Node:node = JSON_Object()
    ;

    SerializeItem(crate, node);

    new
        output[1024]
    ;

    JSON_Stringify(node, output);

    SendClientMessage(playerid, -1, "Saved: %s", output);

    new const
        Item:copy = CreateItem(gItemBuildSmallCrate)
    ;

    DeserializeItem(copy, node);

    new const
        Container:restored = GetItemInside(copy)
    ;

    SendClientMessage(playerid, -1, "Restored %i of %i items.", GetContainerSize(restored), GetContainerSize(containerid));

    new const
        Item:first = GetContainerSlotItem(restored, 0)
    ;

    if (IsValidItem(first)) {
        new
            name[MAX_ITEM_NAME]
        ;

        GetItemName(first, name);

        SendClientMessage(playerid, -1, "Slot 0: %s, amount %i, batch %i.", name, GetItemAmountInt(first), GetItemExtraDataInt(first, "batch"));
    }

    DestroyItem(crate);
    DestroyItem(copy);

    return 1;
}

// Reads the nesting rules out loud, so the numbers in the catalogue can be
// checked against what the code actually answers instead of against themselves.
CMD:devnest(playerid, params[]) {
    new const
        Item:backpack = CreateItem(gItemBuildBackpack),
        Item:crateA   = CreateItem(gItemBuildSmallCrate),
        Item:crateB   = CreateItem(gItemBuildSmallCrate),
        Item:large    = CreateItem(gItemBuildLargeCrate),
        Item:box      = CreateItem(gItemBuildPizza),
        Item:slice    = CreateItem(gItemBuildPizzaSlice),
        Item:rifle    = CreateItem(gItemBuildAK47),
        Item:pistol   = CreateItem(gItemBuildPistol),
        Item:burger   = CreateItem(gItemBuildBurger)
    ;

    new const
        Container:inBackpack = GetItemInside(backpack),
        Container:inCrate    = GetItemInside(crateA),
        Container:inLarge    = GetItemInside(large),
        Container:inBox      = GetItemInside(box),
        Container:pockets    = GetPlayerContainer(playerid),
        Container:bin        = CreateContainer("Bin", 10)
    ;

    SendClientMessage(playerid, -1, "size  crate %i  large %i  backpack %i  box %i  rifle %i",
        GetItemBuildSize(gItemBuildSmallCrate), GetItemBuildSize(gItemBuildLargeCrate),
        GetItemBuildSize(gItemBuildBackpack), GetItemBuildSize(gItemBuildPizza),
        GetItemBuildSize(gItemBuildAK47));

    SendClientMessage(playerid, -1, "crate  -> crate     %i want 0", CanPutItemInContainer(crateB, inCrate));
    SendClientMessage(playerid, -1, "large  -> crate     %i want 0", CanPutItemInContainer(large, inCrate));
    SendClientMessage(playerid, -1, "box    -> backpack  %i want 1", CanPutItemInContainer(box, inBackpack));
    SendClientMessage(playerid, -1, "backpk -> large     %i want 1", CanPutItemInContainer(backpack, inLarge));
    SendClientMessage(playerid, -1, "rifle  -> backpack  %i want 0", CanPutItemInContainer(rifle, inBackpack));
    SendClientMessage(playerid, -1, "rifle  -> large     %i want 1", CanPutItemInContainer(rifle, inLarge));
    SendClientMessage(playerid, -1, "pistol -> backpack  %i want 1", CanPutItemInContainer(pistol, inBackpack));
    SendClientMessage(playerid, -1, "slice  -> box       %i want 1", CanPutItemInContainer(slice, inBox));
    SendClientMessage(playerid, -1, "pistol -> box       %i want 0", CanPutItemInContainer(pistol, inBox));
    SendClientMessage(playerid, -1, "burger -> box       %i want 0", CanPutItemInContainer(burger, inBox));
    SendClientMessage(playerid, -1, "box    -> pockets   %i want 0", CanPutItemInContainer(box, pockets));
    SendClientMessage(playerid, -1, "backpk -> bin       %i want 1", CanPutItemInContainer(backpack, bin));
    SendClientMessage(playerid, -1, "box    -> own       %i want 0", CanPutItemInContainer(box, inBox));

    AddItemToContainer(inBackpack, box);

    SendClientMessage(playerid, -1, "backpk -> box in it %i want 0", CanPutItemInContainer(backpack, inBox));

    DestroyItem(backpack); DestroyItem(crateA); DestroyItem(crateB); DestroyItem(large);
    DestroyItem(slice); DestroyItem(rifle); DestroyItem(pistol); DestroyItem(burger);
    DestroyContainer(bin);

    return 1;
}

// Proves the draw does what the flag says: nothing without ITEM_ATTRIBUTE_LOOTABLE
// ever comes out, and over enough draws no build is favoured. Reservoir picking
// is the kind of thing that looks right and is quietly biased, so it is checked
// rather than trusted.
CMD:devloot(playerid, params[]) {
    new
        counts[MAX_ITEM_BUILDS],
        lootable,
        name[MAX_ITEM_BUILD_NAME]
    ;

    for (new ItemBuild:b; b != ItemBuild:CountItemBuild(); ++b) {
        if (HasItemBuildAttribute(b, ITEM_ATTRIBUTE_LOOTABLE)) {
            ++lootable;
        }
    }

    if (lootable == 0) {
        SendClientMessage(playerid, -1, "Nothing in the catalogue is lootable.");

        return 1;
    }

    for (new i; i != 4000; ++i) {
        new const
            ItemBuild:drawn = RandomItemBuild()
        ;

        if (!HasItemBuildAttribute(drawn, ITEM_ATTRIBUTE_LOOTABLE)) {
            GetItemBuildName(drawn, name);

            SendClientMessage(playerid, -1, "Drew a build that is not lootable: %s", name);

            return 1;
        }

        ++counts[drawn];
    }

    new
        lowest = cellmax,
        highest
    ;

    for (new ItemBuild:b; b != ItemBuild:CountItemBuild(); ++b) {
        if (!HasItemBuildAttribute(b, ITEM_ATTRIBUTE_LOOTABLE)) {
            continue;
        }

        if (counts[b] < lowest) {
            lowest = counts[b];
        }

        if (counts[b] > highest) {
            highest = counts[b];
        }
    }

    SendClientMessage(playerid, -1, "%i of %i builds lootable.", lootable, _:CountItemBuild());
    SendClientMessage(playerid, -1, "4000 draws: low %i, high %i, even would be %i.", lowest, highest, 4000 / lootable);

    return 1;
}


// Reads VolumeFormatShort out loud across the awkward values: the ones either
// side of a litre, and the ones that round up into one.
CMD:devvolume(playerid, params[]) {
    new
        out[MAX_ITEM_AMOUNT_LENGTH]
    ;

    new const Float:cases[] = {
        0.0, 0.001, 0.033, 0.100, 0.330, 0.500, 0.999,
        1.0, 1.1, 2.05, 12.4, 20.0, 0.9999
    };

    for (new i; i != sizeof (cases); ++i) {
        VolumeFormatShort(cases[i], out);

        SendClientMessage(playerid, -1, "%.4f -> %s", cases[i], out);
    }

    return 1;
}

// What every kind of item shows in the corner of its slot the moment it is
// made. A dash is a build that carries no amount at all, which is right for a
// crate and wrong for anything that has a number somebody would want to read.
CMD:devamounts(playerid, params[]) {
    new
        name[MAX_ITEM_BUILD_NAME],
        text[MAX_ITEM_AMOUNT_LENGTH]
    ;

    for (new ItemBuild:b; b != ItemBuild:CountItemBuild(); ++b) {
        new const
            Item:itemid = CreateItem(b)
        ;

        if (itemid == INVALID_ITEM_ID) {
            continue;
        }

        GetItemBuildName(b, name);

        if (!FormatItemAmount(itemid, text)) {
            text = "-";
        }

        SendClientMessage(playerid, -1, "%s: %s", name, text);

        DestroyItem(itemid);
    }

    return 1;
}

// Proves the custom amount round trip, which is what a key that says what it
// opens will rest on: write words on an item, ask the menu what it would draw,
// serialize it, rebuild it, and ask again.
CMD:devkeytext(playerid, params[]) {
    new
        text[64]
    ;

    if (sscanf(params, "s[64]", text)) {
        text = "HOUSE #102";
    }

    new const
        Item:key = CreateItem(gItemBuildHouseKey)
    ;

    if (key == INVALID_ITEM_ID) {
        SendClientMessage(playerid, -1, "Could not make a key.");

        return 1;
    }

    new
        shown[MAX_ITEM_AMOUNT_LENGTH]
    ;

    if (!FormatItemAmount(key, shown)) {
        shown = "-";
    }

    SendClientMessage(playerid, -1, "Fresh key shows: %s", shown);

    SetItemAmountString(key, text);

    if (!FormatItemAmount(key, shown)) {
        shown = "-";
    }

    SendClientMessage(playerid, -1, "After writing '%s' it shows: %s", text, shown);

    new const
        Node:node = JSON_Object()
    ;

    SerializeItem(key, node);

    new const
        Item:copy = CreateItem(gItemBuildHouseKey)
    ;

    DeserializeItem(copy, node);

    if (!FormatItemAmount(copy, shown)) {
        shown = "-";
    }

    SendClientMessage(playerid, -1, "After a save and load it shows: %s", shown);

    DestroyItem(key);
    DestroyItem(copy);

    return 1;
}

// What the action menu believes about itself. Run it the moment the menu is on
// screen when it should not be: if it says it is open, something re-opened it
// and the fault is in the gamemode; if it says it is closed, the textdraws are
// a ghost the client is still drawing and the fault is in how they are hidden.
CMD:devaction(playerid, params[]) {
    new
        name[MAX_ITEM_NAME] = "-"
    ;

    new const
        Item:itemid = GetPlayerItemActionItem(playerid)
    ;

    if (IsValidItem(itemid)) {
        GetItemName(itemid, name);
    }

    SendClientMessage(playerid, -1, "Action menu open: %i, on item: %s", IsPlayerUsingItemActionMenu(playerid), name);

    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    StopDevAnimInternal(playerid);

    return 0;
}
