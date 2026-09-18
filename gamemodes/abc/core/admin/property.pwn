#if defined _CORE_ADMIN_PROPERTY
    #endinput
#endif
#define _CORE_ADMIN_PROPERTY

/**
 * # Placing properties without recompiling
 *
 * Not gameplay. A property needs a door and an inside, and the two are never in
 * the same place, so making one is two steps: stand inside and remember it with
 * /devint, then stand at the door and make it with /devprop.
 *
 * Nothing here is saved. /devpropdump writes every property on the server back
 * out as the CreateHouse and CreateLockup lines that would build them again, to
 * be pasted into core/world.
 *
 * A new kind of property is one more branch in /devprop and one more in the
 * dump.
 */

static enum _:E_DEV_PROPERTY_MARK {
    bool:E_DEV_PROPERTY_MARKED,

    Float:E_DEV_PROPERTY_X,
    Float:E_DEV_PROPERTY_Y,
    Float:E_DEV_PROPERTY_Z,
    Float:E_DEV_PROPERTY_A,

    E_DEV_PROPERTY_INTERIOR
};

static
    gDevInside[MAX_PLAYERS][E_DEV_PROPERTY_MARK],
    gDevVan[MAX_PLAYERS][E_DEV_PROPERTY_MARK]
;

static stock MarkPlayerSpotInternal(playerid, mark[E_DEV_PROPERTY_MARK]) {
    new
        Float:x,
        Float:y,
        Float:z,
        Float:a
    ;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);

    mark[E_DEV_PROPERTY_MARKED]   = true;
    mark[E_DEV_PROPERTY_X]        = x;
    mark[E_DEV_PROPERTY_Y]        = y;
    mark[E_DEV_PROPERTY_Z]        = z;
    mark[E_DEV_PROPERTY_A]        = a;
    mark[E_DEV_PROPERTY_INTERIOR] = GetPlayerInterior(playerid);
}

// The one the character means: whichever they are inside, or the nearest door
// they are standing at. GetPlayerProperty alone only answers the first, and
// these commands are run out on the street.
static stock Property:NearestPropertyInternal(playerid, Float:radius = 5.0) {
    new const
        Property:inside = GetPlayerProperty(playerid)
    ;

    if (inside != INVALID_PROPERTY_ID) {
        return inside;
    }

    new
        Float:px,
        Float:py,
        Float:pz
    ;

    GetPlayerPos(playerid, px, py, pz);

    new const
        world = GetPlayerVirtualWorld(playerid),
        interior = GetPlayerInterior(playerid)
    ;

    new
        Property:nearest = INVALID_PROPERTY_ID,
        Float:closest = radius
    ;

    for (new Property:propertyid, pool = GetPropertyPoolSize(); _:propertyid != pool; ++propertyid) {
        if (!IsValidProperty(propertyid)) {
            continue;
        }

        new const
            Entrance:entranceid = GetPropertyEntrance(propertyid)
        ;

        if (GetEntranceExteriorVirtualWorld(entranceid) != world) {
            continue;
        }

        if (GetEntranceExteriorInterior(entranceid) != interior) {
            continue;
        }

        new
            Float:x,
            Float:y,
            Float:z
        ;

        if (!GetEntranceExteriorPos(entranceid, x, y, z)) {
            continue;
        }

        new const
            Float:distance = VectorSize(x - px, y - py, z - pz)
        ;

        if (distance > closest) {
            continue;
        }

        closest = distance;
        nearest = propertyid;
    }

    return nearest;
}

static stock DumpPropertyInternal(Property:propertyid) {
    new const
        Entrance:entranceid = GetPropertyEntrance(propertyid)
    ;

    if (!IsValidEntrance(entranceid)) {
        return;
    }

    new
        name[MAX_PROPERTY_NAME],
        Float:extX,
        Float:extY,
        Float:extZ,
        Float:extA,
        Float:intX,
        Float:intY,
        Float:intZ,
        Float:intA
    ;

    GetPropertyName(propertyid, name);

    GetEntranceExteriorPos(entranceid, extX, extY, extZ);
    GetEntranceExteriorFacingAngle(entranceid, extA);
    GetEntranceInteriorPos(entranceid, intX, intY, intZ);
    GetEntranceInteriorFacingAngle(entranceid, intA);

    new const
        PropertyType:type = GetPropertyType(propertyid),
        interior = GetEntranceInteriorInterior(entranceid),
        price = GetPropertyPrice(propertyid)
    ;

    if (type == GetLockupPropertyType()) {
        new
            Float:vanX,
            Float:vanY,
            Float:vanZ,
            Float:vanA
        ;

        GetLockupVanPos(GetPropertyLockup(propertyid), vanX, vanY, vanZ, vanA);

        printf("CreateLockup(\"%s\", %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %i, %.4f, %.4f, %.4f, %.4f, %i);",
            name, extX, extY, extZ, extA, intX, intY, intZ, intA, interior, vanX, vanY, vanZ, vanA, price);
    } else {
        printf("CreateHouse(\"%s\", %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %i, %i);",
            name, extX, extY, extZ, extA, intX, intY, intZ, intA, interior, price);
    }

    new
        Float:markerX,
        Float:markerY,
        Float:markerZ
    ;

    if (!GetPropertySaleMarkerPos(propertyid, markerX, markerY, markerZ)) {
        return;
    }

    // Only when it was moved off the door, which is where it starts.
    if (VectorSize(markerX - extX, markerY - extY, markerZ - extZ) < 0.05) {
        return;
    }

    printf("SetPropertySaleMarkerPos(propertyid, %.4f, %.4f, %.4f);", markerX, markerY, markerZ);
}

CMD:devtp(playerid, params[]) {
    new
        Float:x,
        Float:y,
        Float:z,
        interior,
        world
    ;

    if (sscanf(params, "fffD(0)D(0)", x, y, z, interior, world)) {
        SendClientMessage(playerid, -1, "Usage: /devtp <x> <y> <z> [interior] [world]");

        return 1;
    }

    SetPlayerPos(playerid, x, y, z);
    SetPlayerInterior(playerid, interior);
    SetPlayerVirtualWorld(playerid, world);

    SendClientMessage(playerid, -1, "Moved to %.4f, %.4f, %.4f in interior %i, world %i.", x, y, z, interior, world);

    return 1;
}

CMD:devint(playerid, params[]) {
    MarkPlayerSpotInternal(playerid, gDevInside[playerid]);

    SendClientMessage(playerid, -1, "Inside remembered: %.4f, %.4f, %.4f, interior %i. Now stand at the door and /devprop.",
        gDevInside[playerid][E_DEV_PROPERTY_X],
        gDevInside[playerid][E_DEV_PROPERTY_Y],
        gDevInside[playerid][E_DEV_PROPERTY_Z],
        gDevInside[playerid][E_DEV_PROPERTY_INTERIOR]);

    return 1;
}

CMD:devvan(playerid, params[]) {
    MarkPlayerSpotInternal(playerid, gDevVan[playerid]);

    SendClientMessage(playerid, -1, "Parking space remembered: %.4f, %.4f, %.4f.",
        gDevVan[playerid][E_DEV_PROPERTY_X],
        gDevVan[playerid][E_DEV_PROPERTY_Y],
        gDevVan[playerid][E_DEV_PROPERTY_Z]);

    return 1;
}

/**
 * Stand where the door is, facing the way somebody walks out of it. A price of
 * nought takes the kind's own.
 */
CMD:devprop(playerid, params[]) {
    new
        kind[16],
        price,
        name[MAX_PROPERTY_NAME]
    ;

    // 32 is MAX_PROPERTY_NAME, which sscanf wants written out.
    if (sscanf(params, "s[16]is[32]", kind, price, name)) {
        SendClientMessage(playerid, -1, "Usage: /devprop <house|lockup> <price> <name>");
        SendClientMessage(playerid, -1, "A price of 0 takes the default. /devint first, and /devvan for a lock-up.");

        return 1;
    }

    if (!gDevInside[playerid][E_DEV_PROPERTY_MARKED]) {
        SendClientMessage(playerid, -1, "Go and stand inside it first, then /devint.");

        return 1;
    }

    new
        Float:extX,
        Float:extY,
        Float:extZ,
        Float:extA
    ;

    GetPlayerPos(playerid, extX, extY, extZ);
    GetPlayerFacingAngle(playerid, extA);

    new const
        Float:intX = gDevInside[playerid][E_DEV_PROPERTY_X],
        Float:intY = gDevInside[playerid][E_DEV_PROPERTY_Y],
        Float:intZ = gDevInside[playerid][E_DEV_PROPERTY_Z],
        Float:intA = gDevInside[playerid][E_DEV_PROPERTY_A],
        interior = gDevInside[playerid][E_DEV_PROPERTY_INTERIOR]
    ;

    new
        Property:propertyid = INVALID_PROPERTY_ID
    ;

    if (!strcmp(kind, "house", true)) {
        new const
            House:houseid = (price > 0)
                ? CreateHouse(name, extX, extY, extZ, extA, intX, intY, intZ, intA, interior, price)
                : CreateHouse(name, extX, extY, extZ, extA, intX, intY, intZ, intA, interior)
        ;

        if (houseid == INVALID_HOUSE_ID) {
            SendClientMessage(playerid, -1, "Could not make a house.");

            return 1;
        }

        propertyid = GetHouseProperty(houseid);
    } else if (!strcmp(kind, "lockup", true)) {
        if (!gDevVan[playerid][E_DEV_PROPERTY_MARKED]) {
            SendClientMessage(playerid, -1, "A lock-up needs somewhere to park. Stand on it and /devvan.");

            return 1;
        }

        new const
            Float:vanX = gDevVan[playerid][E_DEV_PROPERTY_X],
            Float:vanY = gDevVan[playerid][E_DEV_PROPERTY_Y],
            Float:vanZ = gDevVan[playerid][E_DEV_PROPERTY_Z],
            Float:vanA = gDevVan[playerid][E_DEV_PROPERTY_A]
        ;

        new const
            Lockup:lockupid = (price > 0)
                ? CreateLockup(name, extX, extY, extZ, extA, intX, intY, intZ, intA, interior, vanX, vanY, vanZ, vanA, price)
                : CreateLockup(name, extX, extY, extZ, extA, intX, intY, intZ, intA, interior, vanX, vanY, vanZ, vanA)
        ;

        if (lockupid == INVALID_LOCKUP_ID) {
            SendClientMessage(playerid, -1, "Could not make a lock-up.");

            return 1;
        }

        propertyid = GetLockupProperty(lockupid);
    } else {
        SendClientMessage(playerid, -1, "There is no kind of property called that. house, lockup.");

        return 1;
    }

    SendClientMessage(playerid, -1, "%s %i is up for $%i. /devpropmarker to move the sign, /devpropdump when you are done.",
        name, _:propertyid, GetPropertyPrice(propertyid));

    return 1;
}

CMD:devpropmarker(playerid, params[]) {
    new const
        Property:propertyid = NearestPropertyInternal(playerid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        SendClientMessage(playerid, -1, "Stand at the one you mean, or inside it.");

        return 1;
    }

    new
        Float:x,
        Float:y,
        Float:z
    ;

    GetPlayerPos(playerid, x, y, z);

    if (!SetPropertySaleMarkerPos(propertyid, x, y, z)) {
        SendClientMessage(playerid, -1, "That one has no sign to move.");

        return 1;
    }

    SendClientMessage(playerid, -1, "The sign is where you are standing.");

    return 1;
}

CMD:devpropdel(playerid, params[]) {
    new const
        Property:propertyid = NearestPropertyInternal(playerid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        SendClientMessage(playerid, -1, "Stand at the one you mean, or inside it.");

        return 1;
    }

    new
        name[MAX_PROPERTY_NAME]
    ;

    GetPropertyName(propertyid, name);

    new const
        House:houseid = GetPropertyHouse(propertyid)
    ;

    // Through whichever fragment owns it, so its own record goes as well.
    if (houseid != INVALID_HOUSE_ID) {
        DestroyHouse(houseid);
    } else {
        DestroyProperty(propertyid);
    }

    SendClientMessage(playerid, -1, "%s is gone.", name);

    return 1;
}

CMD:devproplist(playerid, params[]) {
    new
        name[MAX_PROPERTY_NAME],
        kind[MAX_PROPERTY_TYPE_NAME],
        shown
    ;

    for (new Property:propertyid, pool = GetPropertyPoolSize(); _:propertyid != pool; ++propertyid) {
        if (!IsValidProperty(propertyid)) {
            continue;
        }

        GetPropertyName(propertyid, name);
        GetPropertyName(propertyid, kind, .kind = true);

        SendClientMessage(playerid, -1, "%i: %s (%s) $%i%s",
            _:propertyid, name, kind, GetPropertyPrice(propertyid),
            IsPropertyOwned(propertyid) ? (" sold") : (""));

        ++shown;
    }

    SendClientMessage(playerid, -1, "%i properties.", shown);

    return 1;
}

CMD:devpropdump(playerid, params[]) {
    new
        shown
    ;

    print("");
    print("// --- properties ---");

    for (new Property:propertyid, pool = GetPropertyPoolSize(); _:propertyid != pool; ++propertyid) {
        if (!IsValidProperty(propertyid)) {
            continue;
        }

        DumpPropertyInternal(propertyid);

        ++shown;
    }

    print("// --- end ---");

    SendClientMessage(playerid, -1, "%i properties written to the console and log.txt.", shown);

    return 1;
}

hook OnPlayerConnect(playerid) {
    gDevInside[playerid][E_DEV_PROPERTY_MARKED] = false;
    gDevVan[playerid][E_DEV_PROPERTY_MARKED] = false;

    return 0;
}
