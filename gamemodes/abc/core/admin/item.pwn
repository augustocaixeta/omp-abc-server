#if defined _CORE_ADMIN_ITEM
    #endinput
#endif
#define _CORE_ADMIN_ITEM

#include <pp-hooks>

/**
 * # Spawning items
 *
 * Not gameplay. These put items into a character's pockets so
 * the rest of the gamemode has something to be tested against, and they obey
 * the same rules everything else does -- a container is put on the ground
 * rather than in a pocket, because that is where a container goes.
 *
 * The dev prefix on the commands is what tells anybody using the gamemode that
 * these are not part of it. Everything under this folder is expected to come
 * out, and taking it out is deleting the folder and its lines from main.
 *
 * Nothing invents an amount here. Every kind of item that carries a number
 * fills it in for itself when it is made -- rounds, pills, litres, a shelf
 * life, a bundle of cash -- so a figure written at this point would be a figure
 * written over the true one, and this command would be the only way to get an
 * item that lies about itself.
 *
 * Where a made item may go is asked of the item rather than of the build,
 * because that depends on where it is going: a bin takes a rucksack and a
 * pocket does not.
 */

static stock ItemBuild:FindItemBuildInternal(const name[]) {
    new
        buildName[MAX_ITEM_BUILD_NAME]
    ;

    for (new ItemBuild:buildid; buildid != ItemBuild:CountItemBuild(); ++buildid) {
        GetItemBuildName(buildid, buildName);

        if (strfind(buildName, name, true) == 0) {
            return buildid;
        }
    }

    return INVALID_ITEM_BUILD_ID;
}

static stock Item:GiveItemInternal(playerid, ItemBuild:buildid) {
    if (buildid == INVALID_ITEM_BUILD_ID) {
        return INVALID_ITEM_ID;
    }

    new const
        Item:itemid = CreateItem(buildid)
    ;

    if (itemid == INVALID_ITEM_ID) {
        return INVALID_ITEM_ID;
    }

    if (!CanStoreItem(itemid)) {
        DestroyItem(itemid);

        return INVALID_ITEM_ID;
    }

    new
        index = -1
    ;

    if (!AddItemToInventory(playerid, itemid, index)) {
        DestroyItem(itemid);

        return INVALID_ITEM_ID;
    }

    return itemid;
}

static stock Item:SpawnItemInternal(playerid, ItemBuild:buildid) {
    if (buildid == INVALID_ITEM_BUILD_ID) {
        return INVALID_ITEM_ID;
    }

    new const
        Item:itemid = CreateItem(buildid)
    ;

    if (itemid == INVALID_ITEM_ID) {
        return INVALID_ITEM_ID;
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:angle
    ;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angle);

    CreateItemInWorld(itemid,
        x + (floatsin(-angle, degrees) * 0.85),
        y + (floatcos(-angle, degrees) * 0.85),
        z - ITEM_FLOOR_OFFSET,
        0.0,
        0.0,
        angle,
        .worldid = GetPlayerVirtualWorld(playerid),
        .interiorid = GetPlayerInterior(playerid)
    );

    return itemid;
}

CMD:devadd(playerid, params[]) {
    new
        count
    ;

    sscanf(params, "D(12)", count);

    if (!(1 <= count <= 100)) {
        SendClientMessage(playerid, -1, "Between 1 and 100.");

        return 1;
    }

    new
        given
    ;

    for (new i; i != count; ++i) {
        if (GiveItemInternal(playerid, RandomItemBuild()) != INVALID_ITEM_ID) {
            ++given;
        }
    }

    SendClientMessage(playerid, -1, "Added %i. Inventory %i/%i.", given, GetPlayerInventorySize(playerid), GetPlayerInventoryCapacity(playerid));

    return 1;
}

// /devitem with a number written over the amount, for putting fifty of
// something in a pocket without fifty commands.
CMD:devstack(playerid, params[]) {
    new
        name[MAX_ITEM_BUILD_NAME],
        amount
    ;

    if (sscanf(params, "s[32]i", name, amount)) {
        SendClientMessage(playerid, -1, "Usage: /devstack <name> <amount>");

        return 1;
    }

    new const
        ItemBuild:buildid = FindItemBuildInternal(name)
    ;

    if (buildid == INVALID_ITEM_BUILD_ID) {
        SendClientMessage(playerid, -1, "No kind of item starts with that.");

        return 1;
    }

    new const
        Item:itemid = GiveItemInternal(playerid, buildid)
    ;

    if (itemid == INVALID_ITEM_ID) {
        SendClientMessage(playerid, -1, "You have no room for it.");

        return 1;
    }

    SetItemAmountInt(itemid, amount);

    new
        buildName[MAX_ITEM_BUILD_NAME]
    ;

    GetItemBuildName(buildid, buildName);

    SendClientMessage(playerid, -1, "Gave you %s reading %i.", buildName, amount);

    return 1;
}

CMD:devitem(playerid, params[]) {
    new
        name[MAX_ITEM_BUILD_NAME]
    ;

    if (sscanf(params, "s[32]", name)) {
        SendClientMessage(playerid, -1, "Usage: /devitem <name>");

        return 1;
    }

    new const
        ItemBuild:buildid = FindItemBuildInternal(name)
    ;

    if (buildid == INVALID_ITEM_BUILD_ID) {
        SendClientMessage(playerid, -1, "No kind of item starts with that.");

        return 1;
    }

    new
        buildName[MAX_ITEM_BUILD_NAME]
    ;

    GetItemBuildName(buildid, buildName);

    if (GetItemBuildContainerSize(buildid) != 0) {
        if (SpawnItemInternal(playerid, buildid) == INVALID_ITEM_ID) {
            SendClientMessage(playerid, -1, "Could not make a %s.", buildName);

            return 1;
        }

        SendClientMessage(playerid, -1, "Put a %s on the ground: it does not go in a pocket.", buildName);

        return 1;
    }

    if (GiveItemInternal(playerid, buildid) == INVALID_ITEM_ID) {
        SendClientMessage(playerid, -1, "You have no room for it.");

        return 1;
    }

    SendClientMessage(playerid, -1, "Added %s.", buildName);

    return 1;
}

// Fills whatever is open, which is the only way to get anything into a crate
// or a locker before there is loot in the world putting it there. With nothing
// open it fills the character's own pockets, which is what it always did.
CMD:devfill(playerid, params[]) {
    new
        Container:containerid = GetPlayerOpenContainer(playerid)
    ;

    if (containerid == INVALID_CONTAINER_ID) {
        containerid = GetPlayerContainer(playerid);
    }

    if (!IsValidContainer(containerid)) {
        SendClientMessage(playerid, -1, "Nothing to fill.");

        return 1;
    }

    new
        given
    ;

    while (!IsContainerFull(containerid)) {
        new const
            ItemBuild:buildid = RandomItemBuild()
        ;

        new const
            Item:itemid = CreateItem(buildid)
        ;

        if (itemid == INVALID_ITEM_ID) {
            break;
        }

        if (!CanPutItemInContainer(itemid, containerid)) {
            DestroyItem(itemid);

            continue;
        }

        if (!AddItemToContainer(containerid, itemid, .playerid = playerid)) {
            DestroyItem(itemid);

            break;
        }

        ++given;
    }

    new
        name[MAX_CONTAINER_NAME]
    ;

    GetContainerName(containerid, name);

    SendClientMessage(playerid, -1, "Filled %s with %i. Now %i/%i.", name, given, GetContainerSize(containerid), GetContainerCapacity(containerid));

    return 1;
}

/**
 * Every weapon in the catalogue, one of each, so the way they sit in their
 * slots can be looked at side by side. Written as a walk of the builds rather
 * than a list of them: a weapon added to the catalogue tomorrow turns up here
 * on its own, which is the whole point of having it while the framing of them
 * is still being worked out.
 *
 * Loadable ones arrive full. An empty gun draws the same as a full one and the
 * corner of the slot is half of what is being looked at.
 */
CMD:devfillweapons(playerid, params[]) {
    new
        Container:containerid = GetPlayerContainer(playerid)
    ;

    if (!IsValidContainer(containerid)) {
        SendClientMessage(playerid, -1, "You have no pockets.");

        return 1;
    }

    new
        given,
        skipped
    ;

    for (new ItemBuild:buildid, builds = CountItemBuild(); _:buildid != builds; ++buildid) {
        if (GetItemBuildWeapon(buildid) == WEAPON_FIST) {
            continue;
        }

        new const
            Item:itemid = CreateItem(buildid)
        ;

        if (itemid == INVALID_ITEM_ID) {
            break;
        }

        if (!AddItemToContainer(containerid, itemid, .playerid = playerid)) {
            DestroyItem(itemid);

            ++skipped;

            continue;
        }

        new const
            capacity = GetItemBuildWeaponCapacity(buildid)
        ;

        if (capacity > 0) {
            LoadItemWeapon(itemid, capacity);
        }

        ++given;
    }

    SendClientMessage(playerid, -1, "%i weapons in, %i would not fit. Now %i/%i.",
        given, skipped, GetContainerSize(containerid), GetContainerCapacity(containerid));

    return 1;
}
