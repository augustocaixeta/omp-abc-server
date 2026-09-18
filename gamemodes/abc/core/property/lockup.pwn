#if defined _CORE_PROPERTY_LOCKUP
    #endinput
#endif
#define _CORE_PROPERTY_LOCKUP

#include <pp-hooks>

/**
 * # Lock-ups
 *
 * A property type like a house and much smaller: no safe, no limit, no lock of
 * its own. What it adds is a place to park, a van to park there, and a crate
 * inside to put what the van brings back.
 *
 * Owning one is what lets somebody rob at all. A job starts at a point inside
 * and ends when the van is put away: the boot is emptied into the locker on the
 * way past.
 */

#if !defined MAX_LOCKUPS
    #define MAX_LOCKUPS (Lockup:16)
#endif

#if !defined LOCKUP_DEFAULT_PRICE
    #define LOCKUP_DEFAULT_PRICE (250000)
#endif

#if !defined LOCKUP_VAN_MODEL
    #define LOCKUP_VAN_MODEL (498)
#endif

#if !defined LOCKUP_VAN_CAPACITY
    #define LOCKUP_VAN_CAPACITY (120)
#endif

// What stands on the marker. Every property gets one to walk up to; what is on
// it belongs to the type.
#if !defined LOCKUP_FOR_SALE_PICKUP_MODEL
    #define LOCKUP_FOR_SALE_PICKUP_MODEL (1273)
#endif

#if !defined LOCKUP_OWNED_PICKUP_MODEL
    #define LOCKUP_OWNED_PICKUP_MODEL (1272)
#endif

#if !defined LOCKUP_FOR_SALE_MAP_ICON
    #define LOCKUP_FOR_SALE_MAP_ICON (31)
#endif

#if !defined LOCKUP_OWNED_MAP_ICON
    #define LOCKUP_OWNED_MAP_ICON (32)
#endif

#if !defined LOCKUP_STORE_MODEL
    #define LOCKUP_STORE_MODEL (1750)
#endif

#if !defined LOCKUP_STORE_CAPACITY
    #define LOCKUP_STORE_CAPACITY (120)
#endif

// Where the locker and the job point stand until SetLockupStorePos and
// SetLockupJobPos are told better: a step into the room, and on the spot
// somebody arrives at.
#if !defined LOCKUP_STORE_OFFSET
    #define LOCKUP_STORE_OFFSET (1.5)
#endif

#if !defined LOCKUP_PRESS_MODEL
    #define LOCKUP_PRESS_MODEL (1271)
#endif

#if !defined LOCKUP_PRESS_CAPACITY
    #define LOCKUP_PRESS_CAPACITY (12)
#endif

// Which kind of station the press is, asked for by name because the kinds
// themselves are the catalogue's and it is included after this.
#if !defined LOCKUP_PRESS_STATION
    #define LOCKUP_PRESS_STATION "Press"
#endif

#if !defined LOCKUP_JOB_BUTTON_SIZE
    #define LOCKUP_JOB_BUTTON_SIZE (1.2)
#endif

// Stored one above the id, because a missing key reads as zero and zero is a
// lock-up.
#define LOCKUP_JOB_KEY "job"

#define INVALID_LOCKUP_ID (Lockup:-1)

static enum _:E_LOCKUP_DATA {
    bool:E_LOCKUP_CREATED,

    Property:E_LOCKUP_PROPERTY_ID,

    Locker:E_LOCKUP_STORE,
    Locker:E_LOCKUP_PRESS,

    Button:E_LOCKUP_JOB_BUTTON,
    Float:E_LOCKUP_JOB_X,
    Float:E_LOCKUP_JOB_Y,
    Float:E_LOCKUP_JOB_Z,

    E_LOCKUP_VAN_ID,
    Container:E_LOCKUP_VAN_CONTAINER,

    Float:E_LOCKUP_VAN_X,
    Float:E_LOCKUP_VAN_Y,
    Float:E_LOCKUP_VAN_Z,
    Float:E_LOCKUP_VAN_A
};

static
    gLockupData[MAX_LOCKUPS][E_LOCKUP_DATA],
    gLockupCount,

    PropertyType:gLockupType = INVALID_PROPERTY_TYPE,
    Lockup:gPropertyLockup[MAX_PROPERTIES] = { INVALID_LOCKUP_ID, ... }
;

/**
 * # Functions
 */

/**
 * @brief      The property type lock-ups are registered under.
 *
 * What tells a property that happens to be a lock-up from one that is a
 * house, for anything holding a property without knowing which it has.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the type, once OnGameModeInit has defined it
 *             - `INVALID_PROPERTY_TYPE` before that
 */
forward PropertyType:GetLockupPropertyType();

/**
 * @brief      Put a lock-up in the world, for sale.
 *
 * A door to walk up to, a world behind it, and a parking space outside. It
 * is created owned by nobody and priced, so the first thing that happens to
 * it is somebody buying it.
 *
 * No van is made here. One is brought out with TakeLockupVan, and only
 * while a job wants it.
 *
 * @param      name         What is written on the door.
 * @param      extX         Where the door is.
 * @param      extY         Where the door is.
 * @param      extZ         Where the door is.
 * @param      extA         Which way somebody faces coming out of it.
 * @param      intX         Where they arrive inside.
 * @param      intY         Where they arrive inside.
 * @param      intZ         Where they arrive inside.
 * @param      intA         Which way they face inside.
 * @param      intInterior  Which game interior that is.
 * @param      vanX         Where the van is parked.
 * @param      vanY         Where the van is parked.
 * @param      vanZ         Where the van is parked.
 * @param      vanA         Which way it is parked.
 * @param      price        What it sells for.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the lock-up
 *             - `INVALID_LOCKUP_ID` when:
 *                 + there are already MAX_LOCKUPS of them
 *                 + the property behind it could not be created
 */
forward Lockup:CreateLockup(const name[], Float:extX, Float:extY, Float:extZ, Float:extA, Float:intX, Float:intY, Float:intZ, Float:intA, intInterior, Float:vanX, Float:vanY, Float:vanZ, Float:vanA, price = LOCKUP_DEFAULT_PRICE);

/**
 * @brief      Whether a lock-up exists.
 *
 * @param      lockupid  Lock-up to check.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it was created
 *             - `false` it is out of range, or was never created
 */
forward bool:IsValidLockup(Lockup:lockupid);

/**
 * @brief      The property a lock-up is.
 *
 * Its door, its price, its owner and its lock all belong to the property
 * library, and this is the handle to them.
 *
 * @param      lockupid  Lock-up to read.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the property behind it
 *             - `INVALID_PROPERTY_ID` `lockupid` is not a live lock-up
 */
forward Property:GetLockupProperty(Lockup:lockupid);

/**
 * @brief      The lock-up a property is, if it is one.
 *
 * The other way round from GetLockupProperty, for answering a callback that
 * hands over a property and nothing else.
 *
 * @param      propertyid  Property to ask about.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the lock-up it is
 *             - `INVALID_LOCKUP_ID` when:
 *                 + `propertyid` is out of range
 *                 + it is some other kind of property
 */
forward Lockup:GetPropertyLockup(Property:propertyid);

/**
 * @brief      Whether a property is a lock-up.
 *
 * @param      propertyid  Property to ask about.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is one
 *             - `false` it is out of range, or some other kind
 */
forward bool:IsPropertyLockup(Property:propertyid);

/**
 * @brief      The lock-up a character owns.
 *
 * The first one found with their name on it. What gates robbing: somebody
 * with none has nowhere to work out of and no van to do it in.
 *
 * @param      playerid  Character to look up.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the lock-up they own
 *             - `INVALID_LOCKUP_ID` they own none
 */
forward Lockup:GetPlayerLockup(playerid);

/**
 * @brief      The crate inside a lock-up.
 *
 * Where what a job brings back is kept. A container like any other, so a
 * menu opens it, items move in and out of it and nothing has to know it
 * belongs to a lock-up.
 *
 * @param      lockupid  Lock-up to read.
 *
 * @date       16:10 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the container inside it
 *             - `INVALID_CONTAINER_ID` `lockupid` is not a live lock-up
 */
forward Container:GetLockupStore(Lockup:lockupid);

/**
 * @brief      The press inside a lock-up.
 *
 * Where ammunition is made. A station rather than a shelf: only what a
 * recipe running there is made of may be put into it, so it never becomes a
 * second place to keep things.
 *
 * @param      lockupid  Lock-up to read.
 *
 * @date       11:20 18/09/2026
 * @author     augustocaixeta
 *
 * @return     - the container on the press
 *             - `INVALID_CONTAINER_ID` `lockupid` is not a live lock-up
 */
forward Container:GetLockupPress(Lockup:lockupid);

/**
 * @brief      Stand the press somewhere else in the room.
 *
 * The same as SetLockupStorePos, for the other of the two.
 *
 * @param      lockupid  Lock-up to rearrange.
 * @param      x         Where the press stands.
 * @param      y         Where the press stands.
 * @param      z         Where the press stands.
 * @param      a         Which way it faces.
 *
 * @date       11:20 18/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the press is there
 *             - `false` when:
 *                 + `lockupid` is not a live lock-up
 *                 + a new one could not be made, and the old is kept
 */
forward bool:SetLockupPressPos(Lockup:lockupid, Float:x, Float:y, Float:z, Float:a = 0.0);

/**
 * @brief      Stand the locker somewhere else in the room.
 *
 * The position is the object's own, not a spot on the floor: whatever a
 * model editor says, straight in. Anything already in the old one is moved
 * across.
 *
 * Meant to be called once, next to CreateLockup, the same way the sale
 * marker is placed.
 *
 * @param      lockupid  Lock-up to rearrange.
 * @param      x         Where the locker stands.
 * @param      y         Where the locker stands.
 * @param      z         Where the locker stands.
 * @param      a         Which way it faces.
 *
 * @date       17:40 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the locker is there
 *             - `false` when:
 *                 + `lockupid` is not a live lock-up
 *                 + a new locker could not be made, and the old one is kept
 */
forward bool:SetLockupStorePos(Lockup:lockupid, Float:x, Float:y, Float:z, Float:a = 0.0);

/**
 * @brief      Move the spot inside where a job is started.
 *
 * Where somebody stands to take the van out and begin. Until this is called
 * it is wherever they arrive walking in.
 *
 * @param      lockupid  Lock-up to rearrange.
 * @param      x         Where the job point is.
 * @param      y         Where the job point is.
 * @param      z         Where the job point is.
 *
 * @date       17:40 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the point is there
 *             - `false` when:
 *                 + `lockupid` is not a live lock-up
 *                 + the button could not be made
 */
forward bool:SetLockupJobPos(Lockup:lockupid, Float:x, Float:y, Float:z);

/**
 * @brief      Where a job is started inside a lock-up.
 *
 * @param      lockupid  Lock-up to read.
 * @param      x         Written with the job point.
 * @param      y         Written with the job point.
 * @param      z         Written with the job point.
 *
 * @date       17:40 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the three were written
 *             - `false` `lockupid` is not a live lock-up
 */
forward bool:GetLockupJobPos(Lockup:lockupid, &Float:x, &Float:y, &Float:z);

/**
 * @brief      Where a lock-up parks its van.
 *
 * The spot rather than the vehicle, which is there whether one is out or
 * not. What a dev tool reads to write the lock-up back out as source.
 *
 * @param      lockupid  Lock-up to read.
 * @param      x         Written with the parking space.
 * @param      y         Written with the parking space.
 * @param      z         Written with the parking space.
 * @param      a         Written with which way it is parked.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the four were written
 *             - `false` `lockupid` is not a live lock-up
 */
forward bool:GetLockupVanPos(Lockup:lockupid, &Float:x, &Float:y, &Float:z, &Float:a);

/**
 * @brief      The van that is out of a lock-up.
 *
 * @param      lockupid  Lock-up to ask.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the vehicle standing outside it
 *             - `INVALID_VEHICLE_ID` when:
 *                 + `lockupid` is not a live lock-up
 *                 + its van is put away
 */
forward GetLockupVan(Lockup:lockupid);

/**
 * @brief      Bring the van out.
 *
 * It appears in the lock-up's parking space with an empty boot, which is a
 * container of its own for whatever a job fills it with.
 *
 * @param      lockupid  Lock-up to take it from.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the van is outside
 *             - `false` when:
 *                 + `lockupid` is not a live lock-up
 *                 + its van is already out
 *                 + the vehicle or its boot could not be created
 */
forward bool:TakeLockupVan(Lockup:lockupid, playerid = INVALID_PLAYER_ID);

/**
 * @brief      Put the van away, and what is in the back with it.
 *
 * The boot is emptied into the crate inside on the way past. Anything that
 * will not fit is gone: a boot is not somewhere to keep things between jobs
 * and neither is a van that is not there any more.
 *
 * OnLockupVanStored follows, carrying how much was actually unloaded.
 *
 * @param      lockupid  Lock-up to put it back in.
 * @param      playerid  Who put it away, for the callback. Nobody in
 *                       particular is `INVALID_PLAYER_ID`.
 *
 * @date       16:10 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the van is away and the boot is in the crate
 *             - `false` when:
 *                 + `lockupid` is not a live lock-up
 *                 + its van was already away
 */
forward bool:StoreLockupVan(Lockup:lockupid, playerid = INVALID_PLAYER_ID);

/**
 * # Events
 */

/**
 * @brief      A van has been brought out of a lock-up.
 *
 * It is already standing in the parking space by the time this is called.
 * What a job listens for to begin.
 *
 * @param      lockupid  Lock-up it came out of.
 * @param      playerid  Who took it out, or `INVALID_PLAYER_ID`.
 *
 * @date       19:20 17/09/2026
 * @author     augustocaixeta
 */
forward OnLockupVanTaken(Lockup:lockupid, playerid);

/**
 * @brief      A van has been put away and its boot emptied into the crate.
 *
 * The van is already gone by the time this is called. What a job watches
 * for: coming home with a full boot is what finishes one.
 *
 * @param      lockupid  Lock-up it was put back into.
 * @param      playerid  Who put it away, or `INVALID_PLAYER_ID`.
 * @param      unloaded  How many items reached the crate, which is nought
 *                       for an empty boot or a full crate.
 *
 * @date       16:10 17/09/2026
 * @author     augustocaixeta
 */
forward OnLockupVanStored(Lockup:lockupid, playerid, unloaded);

/**
 * @brief      Somebody is starting a job at a lock-up's job point.
 *
 * Sent before the van is brought out, so refusing it means nothing happened
 * at all. Where picking a target, charging for the job or asking a faction
 * whether this member may work belongs.
 *
 * @param      playerid  Character starting.
 * @param      lockupid  Lock-up they are standing in.
 *
 * @date       17:40 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` let it start
 *             - non-zero refuse it, and no van comes out
 */
forward OnPlayerStartLockupJob(playerid, Lockup:lockupid);

/**
 * # Internal
 */

static stock Locker:MakeLockupStoreInternal(Property:propertyid, Float:x, Float:y, Float:z, Float:a, interior) {
    return CreateLocker("Lock-up", LOCKUP_STORE_MODEL, LOCKUP_STORE_CAPACITY,
        x, y, z, 0.0, 0.0, a,
        GetPropertyVirtualWorld(propertyid), interior,
        .buttonOffsetZ = 1.0
    );
}

static stock Locker:MakeLockupPressInternal(Property:propertyid, Float:x, Float:y, Float:z, Float:a, interior) {
    new const
        Locker:lockerid = CreateLocker("Press", LOCKUP_PRESS_MODEL, LOCKUP_PRESS_CAPACITY,
            x, y, z, 0.0, 0.0, a,
            GetPropertyVirtualWorld(propertyid), interior,
            .buttonOffsetZ = 1.0
        )
    ;

    if (lockerid == INVALID_LOCKER_ID) {
        return INVALID_LOCKER_ID;
    }

    SetContainerCraftStation(GetLockerContainer(lockerid), FindCraftStation(LOCKUP_PRESS_STATION));

    return lockerid;
}

static stock Button:MakeLockupJobButtonInternal(Lockup:lockupid, Property:propertyid, Float:x, Float:y, Float:z, interior) {
    new const
        Button:buttonid = CreateButton(x, y, z, ITEM_KEY_OPEN_ITEM, LOCKUP_JOB_BUTTON_SIZE, GetPropertyVirtualWorld(propertyid), interior)
    ;

    if (buttonid == INVALID_BUTTON_ID) {
        return INVALID_BUTTON_ID;
    }

    SetButtonExtraData(buttonid, LOCKUP_JOB_KEY, _:lockupid + 1);

    return buttonid;
}

static stock Lockup:ButtonLockupInternal(Button:buttonid) {
    new const
        stored = GetButtonExtraData(buttonid, LOCKUP_JOB_KEY)
    ;

    if (!stored) {
        return INVALID_LOCKUP_ID;
    }

    return Lockup:(stored - 1);
}

// Which interior of the game a lock-up's inside is, for putting anything in it.
static stock LockupInteriorInternal(Lockup:lockupid) {
    return GetEntranceInteriorInterior(GetPropertyEntrance(gLockupData[lockupid][E_LOCKUP_PROPERTY_ID]));
}

static stock MoveContainerItemsInternal(Container:from, Container:to, playerid) {
    if (!IsValidContainer(from) || !IsValidContainer(to)) {
        return 0;
    }

    new
        moved
    ;

    for (new i, capacity = GetContainerCapacity(from); i != capacity; ++i) {
        new
            Item:itemid = INVALID_ITEM_ID
        ;

        if (!RemoveItemFromContainer(from, i, itemid, .playerid = playerid)) {
            continue;
        }

        if (!AddItemToContainer(to, itemid, .playerid = playerid)) {
            DestroyItem(itemid);

            continue;
        }

        ++moved;
    }

    return moved;
}

/**
 * # External
 */

stock PropertyType:GetLockupPropertyType() {
    return gLockupType;
}

stock bool:IsValidLockup(Lockup:lockupid) {
    if (!(0 <= _:lockupid < _:MAX_LOCKUPS)) {
        return false;
    }

    return gLockupData[lockupid][E_LOCKUP_CREATED];
}

stock Property:GetLockupProperty(Lockup:lockupid) {
    if (!IsValidLockup(lockupid)) {
        return INVALID_PROPERTY_ID;
    }

    return gLockupData[lockupid][E_LOCKUP_PROPERTY_ID];
}

stock Lockup:GetPropertyLockup(Property:propertyid) {
    if (!(0 <= _:propertyid < _:MAX_PROPERTIES)) {
        return INVALID_LOCKUP_ID;
    }

    new const
        Lockup:lockupid = gPropertyLockup[propertyid]
    ;

    return IsValidLockup(lockupid) ? lockupid : INVALID_LOCKUP_ID;
}

stock bool:IsPropertyLockup(Property:propertyid) {
    return GetPropertyLockup(propertyid) != INVALID_LOCKUP_ID;
}

stock Lockup:CreateLockup(const name[], Float:extX, Float:extY, Float:extZ, Float:extA, Float:intX, Float:intY, Float:intZ, Float:intA, intInterior, Float:vanX, Float:vanY, Float:vanZ, Float:vanA, price = LOCKUP_DEFAULT_PRICE) {
    if (gLockupCount == _:MAX_LOCKUPS) {
        return INVALID_LOCKUP_ID;
    }

    new const
        Lockup:lockupid = Lockup:gLockupCount
    ;

    new const
        Property:propertyid = CreateProperty(gLockupType, name,
            extX, extY, extZ, extA, 0, 0,
            intX, intY, intZ, intA, PROPERTY_AUTO_WORLD, intInterior
        )
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return INVALID_LOCKUP_ID;
    }

    ++gLockupCount;

    gLockupData[lockupid][E_LOCKUP_CREATED]       = true;
    gLockupData[lockupid][E_LOCKUP_PROPERTY_ID]   = propertyid;
    gLockupData[lockupid][E_LOCKUP_STORE]         = MakeLockupStoreInternal(propertyid,
        intX + (floatsin(-intA, degrees) * LOCKUP_STORE_OFFSET),
        intY + (floatcos(-intA, degrees) * LOCKUP_STORE_OFFSET),
        intZ, intA, intInterior);
    gLockupData[lockupid][E_LOCKUP_PRESS]         = MakeLockupPressInternal(propertyid,
        intX - (floatsin(-intA, degrees) * LOCKUP_STORE_OFFSET),
        intY - (floatcos(-intA, degrees) * LOCKUP_STORE_OFFSET),
        intZ, intA, intInterior);
    gLockupData[lockupid][E_LOCKUP_JOB_BUTTON]    = MakeLockupJobButtonInternal(lockupid, propertyid, intX, intY, intZ, intInterior);
    gLockupData[lockupid][E_LOCKUP_JOB_X]         = intX;
    gLockupData[lockupid][E_LOCKUP_JOB_Y]         = intY;
    gLockupData[lockupid][E_LOCKUP_JOB_Z]         = intZ;
    gLockupData[lockupid][E_LOCKUP_VAN_ID]        = INVALID_VEHICLE_ID;
    gLockupData[lockupid][E_LOCKUP_VAN_CONTAINER] = INVALID_CONTAINER_ID;
    gLockupData[lockupid][E_LOCKUP_VAN_X]         = vanX;
    gLockupData[lockupid][E_LOCKUP_VAN_Y]         = vanY;
    gLockupData[lockupid][E_LOCKUP_VAN_Z]         = vanZ;
    gLockupData[lockupid][E_LOCKUP_VAN_A]         = vanA;

    gPropertyLockup[propertyid] = lockupid;

    SetPropertyPrice(propertyid, price);
    SetPropertySaleMarker(propertyid,
        LOCKUP_FOR_SALE_PICKUP_MODEL, LOCKUP_OWNED_PICKUP_MODEL,
        LOCKUP_FOR_SALE_MAP_ICON, LOCKUP_OWNED_MAP_ICON
    );

    return lockupid;
}

stock Lockup:GetPlayerLockup(playerid) {
    for (new Lockup:lockupid; _:lockupid != gLockupCount; ++lockupid) {
        if (!gLockupData[lockupid][E_LOCKUP_CREATED]) {
            continue;
        }

        if (IsPlayerPropertyOwner(playerid, gLockupData[lockupid][E_LOCKUP_PROPERTY_ID])) {
            return lockupid;
        }
    }

    return INVALID_LOCKUP_ID;
}

stock bool:SetLockupStorePos(Lockup:lockupid, Float:x, Float:y, Float:z, Float:a = 0.0) {
    if (!IsValidLockup(lockupid)) {
        return false;
    }

    new const
        Locker:previous = gLockupData[lockupid][E_LOCKUP_STORE],
        Locker:lockerid = MakeLockupStoreInternal(gLockupData[lockupid][E_LOCKUP_PROPERTY_ID], x, y, z, a, LockupInteriorInternal(lockupid))
    ;

    if (lockerid == INVALID_LOCKER_ID) {
        return false;
    }

    gLockupData[lockupid][E_LOCKUP_STORE] = lockerid;

    MoveContainerItemsInternal(GetLockerContainer(previous), GetLockerContainer(lockerid), INVALID_PLAYER_ID);

    DestroyLocker(previous);

    return true;
}

stock bool:SetLockupJobPos(Lockup:lockupid, Float:x, Float:y, Float:z) {
    if (!IsValidLockup(lockupid)) {
        return false;
    }

    new const
        Button:buttonid = MakeLockupJobButtonInternal(lockupid, gLockupData[lockupid][E_LOCKUP_PROPERTY_ID], x, y, z, LockupInteriorInternal(lockupid))
    ;

    if (buttonid == INVALID_BUTTON_ID) {
        return false;
    }

    DestroyButton(gLockupData[lockupid][E_LOCKUP_JOB_BUTTON]);

    gLockupData[lockupid][E_LOCKUP_JOB_BUTTON] = buttonid;
    gLockupData[lockupid][E_LOCKUP_JOB_X] = x;
    gLockupData[lockupid][E_LOCKUP_JOB_Y] = y;
    gLockupData[lockupid][E_LOCKUP_JOB_Z] = z;

    return true;
}

stock bool:GetLockupJobPos(Lockup:lockupid, &Float:x, &Float:y, &Float:z) {
    if (!IsValidLockup(lockupid)) {
        return false;
    }

    x = gLockupData[lockupid][E_LOCKUP_JOB_X];
    y = gLockupData[lockupid][E_LOCKUP_JOB_Y];
    z = gLockupData[lockupid][E_LOCKUP_JOB_Z];

    return true;
}

stock Container:GetLockupStore(Lockup:lockupid) {
    if (!IsValidLockup(lockupid)) {
        return INVALID_CONTAINER_ID;
    }

    return GetLockerContainer(gLockupData[lockupid][E_LOCKUP_STORE]);
}

stock Container:GetLockupPress(Lockup:lockupid) {
    if (!IsValidLockup(lockupid)) {
        return INVALID_CONTAINER_ID;
    }

    return GetLockerContainer(gLockupData[lockupid][E_LOCKUP_PRESS]);
}

stock bool:SetLockupPressPos(Lockup:lockupid, Float:x, Float:y, Float:z, Float:a = 0.0) {
    if (!IsValidLockup(lockupid)) {
        return false;
    }

    new const
        Locker:previous = gLockupData[lockupid][E_LOCKUP_PRESS],
        Locker:lockerid = MakeLockupPressInternal(gLockupData[lockupid][E_LOCKUP_PROPERTY_ID], x, y, z, a, LockupInteriorInternal(lockupid))
    ;

    if (lockerid == INVALID_LOCKER_ID) {
        return false;
    }

    gLockupData[lockupid][E_LOCKUP_PRESS] = lockerid;

    MoveContainerItemsInternal(GetLockerContainer(previous), GetLockerContainer(lockerid), INVALID_PLAYER_ID);

    DestroyLocker(previous);

    return true;
}

stock bool:GetLockupVanPos(Lockup:lockupid, &Float:x, &Float:y, &Float:z, &Float:a) {
    if (!IsValidLockup(lockupid)) {
        return false;
    }

    x = gLockupData[lockupid][E_LOCKUP_VAN_X];
    y = gLockupData[lockupid][E_LOCKUP_VAN_Y];
    z = gLockupData[lockupid][E_LOCKUP_VAN_Z];
    a = gLockupData[lockupid][E_LOCKUP_VAN_A];

    return true;
}

stock GetLockupVan(Lockup:lockupid) {
    if (!IsValidLockup(lockupid)) {
        return INVALID_VEHICLE_ID;
    }

    return gLockupData[lockupid][E_LOCKUP_VAN_ID];
}

stock bool:TakeLockupVan(Lockup:lockupid, playerid = INVALID_PLAYER_ID) {
    if (!IsValidLockup(lockupid)) {
        return false;
    }

    if (gLockupData[lockupid][E_LOCKUP_VAN_ID] != INVALID_VEHICLE_ID) {
        return false;
    }

    new const
        vehicleid = CreateVehicle(LOCKUP_VAN_MODEL,
            gLockupData[lockupid][E_LOCKUP_VAN_X],
            gLockupData[lockupid][E_LOCKUP_VAN_Y],
            gLockupData[lockupid][E_LOCKUP_VAN_Z],
            gLockupData[lockupid][E_LOCKUP_VAN_A],
            1, 1, -1)
    ;

    if (vehicleid == INVALID_VEHICLE_ID) {
        return false;
    }

    new const
        Container:containerid = CreateContainer("BOOT", LOCKUP_VAN_CAPACITY)
    ;

    if (containerid == INVALID_CONTAINER_ID) {
        DestroyVehicle(vehicleid);

        return false;
    }

    LinkContainerToVehicle(vehicleid, containerid);

    gLockupData[lockupid][E_LOCKUP_VAN_ID] = vehicleid;
    gLockupData[lockupid][E_LOCKUP_VAN_CONTAINER] = containerid;

    CallLocalFunction("OnLockupVanTaken", "ii", _:lockupid, playerid);

    return true;
}

stock bool:StoreLockupVan(Lockup:lockupid, playerid = INVALID_PLAYER_ID) {
    if (!IsValidLockup(lockupid)) {
        return false;
    }

    new const
        vehicleid = gLockupData[lockupid][E_LOCKUP_VAN_ID]
    ;

    if (vehicleid == INVALID_VEHICLE_ID) {
        return false;
    }

    new const
        unloaded = MoveContainerItemsInternal(gLockupData[lockupid][E_LOCKUP_VAN_CONTAINER], GetLockupStore(lockupid), playerid)
    ;

    UnlinkContainerFromVehicle(vehicleid);
    DestroyContainer(gLockupData[lockupid][E_LOCKUP_VAN_CONTAINER]);
    DestroyVehicle(vehicleid);

    gLockupData[lockupid][E_LOCKUP_VAN_ID] = INVALID_VEHICLE_ID;
    gLockupData[lockupid][E_LOCKUP_VAN_CONTAINER] = INVALID_CONTAINER_ID;

    CallLocalFunction("OnLockupVanStored", "iii", _:lockupid, playerid, unloaded);

    return true;
}

/**
 * # Calls
 */

hook OnGameModeInit() {
    gLockupType = DefinePropertyType("Lock-up");

    return 0;
}

hook OnPropertyCreate(Property:propertyid) {
    gPropertyLockup[propertyid] = INVALID_LOCKUP_ID;

    return 0;
}

// A property taken down out from under a lock-up leaves the van standing in the
// street and the record pointing at nothing.
hook OnPropertyDestroy(Property:propertyid) {
    new const
        Lockup:lockupid = GetPropertyLockup(propertyid)
    ;

    if (lockupid == INVALID_LOCKUP_ID) {
        return 0;
    }

    StoreLockupVan(lockupid);
    DestroyLocker(gLockupData[lockupid][E_LOCKUP_STORE]);
    DestroyLocker(gLockupData[lockupid][E_LOCKUP_PRESS]);
    DestroyButton(gLockupData[lockupid][E_LOCKUP_JOB_BUTTON]);

    gLockupData[lockupid][E_LOCKUP_CREATED] = false;
    gPropertyLockup[propertyid] = INVALID_LOCKUP_ID;

    return 0;
}

hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if (!(newkeys & ITEM_KEY_OPEN_ITEM)) {
        return 0;
    }

    new const
        Lockup:lockupid = ButtonLockupInternal(GetPlayerButton(playerid))
    ;

    if (!IsValidLockup(lockupid)) {
        return 0;
    }

    if (!IsPlayerPropertyOwner(playerid, gLockupData[lockupid][E_LOCKUP_PROPERTY_ID])) {
        SendPlayerNotice(playerid, "This is not your lock-up to work out of.");

        return 1;
    }

    if (GetLockupVan(lockupid) != INVALID_VEHICLE_ID) {
        SendPlayerNotice(playerid, "The van is already outside.");

        return 1;
    }

    if (CallLocalFunction("OnPlayerStartLockupJob", "ii", playerid, _:lockupid)) {
        return 1;
    }

    if (!TakeLockupVan(lockupid, playerid)) {
        SendPlayerNotice(playerid, "The van will not start.");

        return 1;
    }

    return 1;
}

hook OnPlayerRequestPrompt(playerid) {
    new const
        Lockup:lockupid = ButtonLockupInternal(GetPlayerButton(playerid))
    ;

    if (!IsValidLockup(lockupid)) {
        return 0;
    }

    OfferPlayerPrompt(playerid, PLAYER_PROMPT_REACH, "Press %s to start a job.", ITEM_KEY_OPEN_NAME);

    return 0;
}

CMD:van(playerid, params[]) {
    new const
        Lockup:lockupid = GetPlayerLockup(playerid)
    ;

    if (lockupid == INVALID_LOCKUP_ID) {
        SendPlayerNotice(playerid, "You have no lock-up to keep a van in.");

        return 1;
    }

    if (GetLockupVan(lockupid) != INVALID_VEHICLE_ID) {
        if (!StoreLockupVan(lockupid, playerid)) {
            return 1;
        }

        SendPlayerNotice(playerid, "The van is put away, and the back of it is in the crate.");

        return 1;
    }

    if (!TakeLockupVan(lockupid, playerid)) {
        SendPlayerNotice(playerid, "The van will not start.");

        return 1;
    }

    SendPlayerNotice(playerid, "The van is outside your lock-up.");

    return 1;
}
