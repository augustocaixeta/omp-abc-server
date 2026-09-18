#if defined _CORE_ADMIN_WORLD
    #endinput
#endif
#define _CORE_ADMIN_WORLD

#include <pp-hooks>

/**
 * # Putting things in the world
 *
 * Not gameplay. A locker to walk up to, with items in it, so
 * the container menu has something to open that is not a character's own
 * pockets.
 *
 * The dev prefix on the commands is what tells anybody using the gamemode that
 * these are not part of it. Everything under this folder is expected to come
 * out, and taking it out is deleting the folder and its lines from main.
 *
 * A locker has no item build behind it, so nothing raises its model off the
 * floor the way BuildItem's objOffsetZ does for an item. The model used is
 * 1271, the same one the Large Crate uses, so it needs the same 0.3112 that the
 * crate's build carries -- written out because there is no build to ask.
 *
 * It gets no popup of its own. What the key does is offered through
 * OnPlayerRequestPrompt like everything else, so that walking up to a locker
 * while carrying something says one thing and not two.
 *
 * What goes into it is asked through CanStoreItem. Reaching around that is what
 * once filled a locker with crates drawn in the locker's own model, which is
 * the same 1271.
 *
 * The command takes what kind of thing it is, because the three that are not a
 * crate only differ in what they refuse and that is the part worth being able
 * to stand in front of:
 *
 *     /devlocker         a crate -- anything in, anything out
 *     /devlocker bin     a bin -- anything in, nothing back out
 *     /devlocker shop    a counter -- nothing in, and out costs money
 *     /devlocker sealed  a window -- neither
 */

static
    Locker:gDevLocker[MAX_PLAYERS] = { INVALID_LOCKER_ID, ... }
;

// What the shop charges, which is the same for everything on the counter. A
// real one would read a price off the build; this one exists to prove that
// refusing and allowing both reach the menu.
#define DEV_SHOP_PRICE (250)

// Prints one of GTA's own particle textures onto a flat plane, for trying out
// the halo that marks something worth taking. 19475..19483 are the planes,
// 19836 is a model carrying particle.txd, and the colour tints the texture.
static
    STREAMER_TAG_OBJECT:gDevGlow[MAX_PLAYERS] = { STREAMER_TAG_OBJECT:INVALID_STREAMER_ID, ... }
;

CMD:devglow(playerid, params[]) {
    if (IsValidDynamicObject(gDevGlow[playerid])) {
        DestroyDynamicObject(gDevGlow[playerid]);

        gDevGlow[playerid] = STREAMER_TAG_OBJECT:INVALID_STREAMER_ID;

        SendClientMessage(playerid, -1, "Glow removed.");

        return 1;
    }

    new
        modelid,
        texture[24],
        Float:rotX,
        colour
    ;

    sscanf(params, "P< >D(19477)S(coronaringb)[24]F(0.0)H(FFFFFFFF)", modelid, texture, rotX, colour);

    new
        Float:x,
        Float:y,
        Float:z,
        Float:angle
    ;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angle);

    new const
        STREAMER_TAG_OBJECT:objectid = CreateDynamicObject(modelid,
            x + (floatsin(-angle, degrees) * 1.5),
            y + (floatcos(-angle, degrees) * 1.5),
            z - 0.9,
            rotX, 0.0, 0.0,
            .worldid = GetPlayerVirtualWorld(playerid),
            .interiorid = GetPlayerInterior(playerid)
        )
    ;

    if (!IsValidDynamicObject(objectid)) {
        SendClientMessage(playerid, -1, "Could not create it.");

        return 1;
    }

    SetDynamicObjectMaterial(objectid, 0, 19836, "particle", texture, colour);

    gDevGlow[playerid] = objectid;

    SendClientMessage(playerid, -1, "%s on model %i, rotated %.1f. /devglow again to remove.", texture, modelid, rotX);
    SendClientMessage(playerid, -1, "Try: coronaringb, coronastar, coronareflect, shad_exp, target256, lockon.");

    return 1;
}

CMD:devlocker(playerid, params[]) {
    if (IsValidLocker(gDevLocker[playerid])) {
        DestroyLocker(gDevLocker[playerid]);

        gDevLocker[playerid] = INVALID_LOCKER_ID;

        SendClientMessage(playerid, -1, "Locker removed.");

        return 1;
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:angle
    ;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angle);

    new const
        Float:modelOffsetZ = 0.3112
    ;

    new
        E_CONTAINER_ACCESS:access = CONTAINER_ACCESS_FREE,
        name[16] = "Storage"
    ;

    if (!strcmp(params, "bin", true)) {
        access = CONTAINER_ACCESS_DUMP;
        name = "Bin";
    }

    if (!strcmp(params, "shop", true)) {
        access = CONTAINER_ACCESS_SHOP;
        name = "Counter";
    }

    if (!strcmp(params, "sealed", true)) {
        access = CONTAINER_ACCESS_SEALED;
        name = "Display";
    }

    new const
        Locker:lockerid = CreateLocker(name, 1271, 20, x + (floatsin(-angle, degrees) * 1.5), y + (floatcos(-angle, degrees) * 1.5), z - ITEM_FLOOR_OFFSET + modelOffsetZ, 0.0, 0.0, angle, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), .buttonOffsetZ = 1.0)
    ;

    if (lockerid == INVALID_LOCKER_ID) {
        SendClientMessage(playerid, -1, "Could not create it.");

        return 1;
    }

    SetContainerAccess(GetLockerContainer(lockerid), access);

    gDevLocker[playerid] = lockerid;

    for (new i; i != 8; ++i) {
        new const
            ItemBuild:buildid = RandomItemBuild()
        ;

        if (GetItemBuildContainerSize(buildid) != 0) {
            continue;
        }

        new const
            Item:itemid = CreateItem(buildid)
        ;

        new
            index = -1
        ;

        if (!AddItemToLocker(lockerid, itemid, index)) {
            DestroyItem(itemid);
        }
    }

    SendClientMessage(playerid, -1, "%s %i with %i items. Walk up to it.", name, _:lockerid, GetLockerSize(lockerid));

    return 1;
}

/**
 * The counter answering for itself. Nothing has left it when this is called, so
 * saying no here is the whole of saying no -- there is nothing to put back.
 */
public OnPlayerBuyContainerItem(playerid, Container:containerid, Item:itemid, index) {
    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (GetPlayerMoney(playerid) < DEV_SHOP_PRICE) {
        SendPlayerNotice(playerid, "The %s costs $%i and you have $%i.", name, DEV_SHOP_PRICE, GetPlayerMoney(playerid));

        return 0;
    }

    GivePlayerMoney(playerid, -DEV_SHOP_PRICE);

    SendPlayerNotice(playerid, "You buy the %s for $%i.", name, DEV_SHOP_PRICE);

    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    if (IsValidLocker(gDevLocker[playerid])) {
        DestroyLocker(gDevLocker[playerid]);
    }

    gDevLocker[playerid] = INVALID_LOCKER_ID;

    return 0;
}

