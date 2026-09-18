#if defined _INC_PROPERTIES_HOUSE
    #endinput
#endif
#define _INC_PROPERTIES_HOUSE

// A fragment, and the shape every fragment takes. property.inc owns the door,
// the world behind it, the name, the owner and the lock; this owns only what
// makes a house a house rather than a shop: a safe to keep money in, a lock
// that starts shut, and a limit on how many of them one person may hold.
//
// It has ids of its own. Houses are numbered from zero and so are businesses,
// because a fragment is a library in its own right and a House: is not a
// Business: even when both are standing on a Property:. Pawn has no tag that
// widens into another one, so the two id spaces meet in two named functions
// rather than in a cast: GetHouseProperty going down, GetPropertyHouse coming
// back up. That is the same seam entrance.inc has with button.inc and gym.inc
// has with its equipment.
//
// property.inc comes before pp-hooks because it pulls pp-hooks in itself, and
// the hooks written after the last include of pp-hooks are the ones that get a
// number of their own.
#include <property>

#include <pp-hooks>

/**
 * # Header
 */

// A fragment sets its own ceiling, and it is not the framework's: a server with
// four thousand properties on it might have five hundred houses among them, and
// the array behind these only has to be as big as that.
#if !defined MAX_HOUSES
    #define MAX_HOUSES (House:512)
#endif

// How many houses one account may hold. This is the fragment deciding, not the
// framework: a shop or a workshop counts against nothing here.
#if !defined MAX_PLAYER_HOUSES
    #define MAX_PLAYER_HOUSES (3)
#endif

#if !defined HOUSE_DEFAULT_PRICE
    #define HOUSE_DEFAULT_PRICE (100000)
#endif

// The sign on the front lawn: the one the story mode puts on a house that can
// be had, and the one it puts on a house that has been.
#if !defined HOUSE_FOR_SALE_PICKUP_MODEL
    #define HOUSE_FOR_SALE_PICKUP_MODEL (1273)
#endif

#if !defined HOUSE_OWNED_PICKUP_MODEL
    #define HOUSE_OWNED_PICKUP_MODEL (1272)
#endif

// And the blip on the map that goes with each of them.
#if !defined HOUSE_FOR_SALE_MAP_ICON
    #define HOUSE_FOR_SALE_MAP_ICON (31)
#endif

#if !defined HOUSE_OWNED_MAP_ICON
    #define HOUSE_OWNED_MAP_ICON (32)
#endif

#define INVALID_HOUSE_ID (House:-1)

static enum _:E_HOUSE_DATA {
    bool:E_HOUSE_CREATED,

    Property:E_HOUSE_PROPERTY_ID,

    E_HOUSE_SAFE_MONEY
};

// The same free list every other library here hands its ids out of: a slot
// given back points at the one given back before it, and slots never used yet
// come off a high water mark.
static
    gHouseNextFree[_:MAX_HOUSES],
    gHouseFreeHead = -1,
    gHouseHighWater,
    gHouseCount,
    gHouseData[MAX_HOUSES][E_HOUSE_DATA]
;

// Which house a property is, the other way round. Every callback the framework
// raises arrives holding a property, and this is what turns it back into a
// house in one array read. One cell per property is what the whole of this
// costs; the house data itself only takes room for houses.
static
    PropertyType:gHouseType = INVALID_PROPERTY_TYPE,
    House:gPropertyHouse[MAX_PROPERTIES] = { INVALID_HOUSE_ID, ... }
;

/**
 * # Functions
 */

forward PropertyType:GetHousePropertyType();

forward House:CreateHouse(const name[], Float:extX, Float:extY, Float:extZ, Float:extA, Float:intX, Float:intY, Float:intZ, Float:intA, intInterior, price = HOUSE_DEFAULT_PRICE);
forward bool:DestroyHouse(House:houseid);
forward bool:IsValidHouse(House:houseid);
forward GetHouseCount();
forward GetHousePoolSize();

forward Property:GetHouseProperty(House:houseid);
forward House:GetPropertyHouse(Property:propertyid);
forward bool:IsPropertyHouse(Property:propertyid);

forward bool:SetHouseSafeMoney(House:houseid, amount);
forward GetHouseSafeMoney(House:houseid);

forward bool:GetHouseName(House:houseid, output[], size = sizeof (output));
forward bool:SetHouseLocked(House:houseid, bool:locked);
forward bool:IsHouseLocked(House:houseid);
forward bool:IsPlayerHouseOwner(playerid, House:houseid);

forward House:GetPlayerHouse(playerid);
forward CountPlayerHouses(playerid);
forward GetPlayerHouses(playerid, House:houses[], size = sizeof (houses));

forward OnHouseCreate(House:houseid);
forward OnHouseDestroy(House:houseid);

/**
 * # OnHouseCreate
 * # OnHouseDestroy
 *
 * # Returns
 *   - These callbacks do not handle returns.
 */

forward OnPlayerHouseLimit(playerid, House:houseid);

/**
 * # OnPlayerHouseLimit
 *
 * The player already holds as many houses as they may. Called instead of
 * OnPlayerBuyProperty, which never sees the sale at all, so this is the only
 * chance to say why nothing happened.
 *
 * # Returns
 *   - This callback does not handle returns.
 */

/**
 * # Internal
 */

static stock GetFreeHouseIndexInternal() {
    if (gHouseFreeHead != -1) {
        new const
            index = gHouseFreeHead
        ;

        gHouseFreeHead = gHouseNextFree[index];

        return index;
    }

    if (gHouseHighWater != _:MAX_HOUSES) {
        return gHouseHighWater++;
    }

    return -1;
}

static stock House:AllocHouseInternal() {
    new const
        index = GetFreeHouseIndexInternal()
    ;

    if (index == -1) {
        return INVALID_HOUSE_ID;
    }

    ++gHouseCount;

    return House:index;
}

static stock FreeHouseInternal(House:houseid) {
    new const
        Property:propertyid = gHouseData[houseid][E_HOUSE_PROPERTY_ID]
    ;

    if (0 <= _:propertyid < _:MAX_PROPERTIES) {
        gPropertyHouse[propertyid] = INVALID_HOUSE_ID;
    }

    static const
        HOUSE_DATA_NONE[E_HOUSE_DATA]
    ;

    gHouseData[houseid] = HOUSE_DATA_NONE;

    gHouseNextFree[_:houseid] = gHouseFreeHead;
    gHouseFreeHead = _:houseid;

    --gHouseCount;
}

static stock bool:IsValidHouseInternal(House:houseid) {
    if (!(0 <= _:houseid < _:MAX_HOUSES)) {
        return false;
    }

    return gHouseData[houseid][E_HOUSE_CREATED];
}

// The property is only bounds checked, not asked whether it is still standing:
// OnPropertyDestroy arrives after the property has already been given back, and
// that is the one call that most needs an answer out of this. The other end is
// kept straight instead, cleared when a house goes and again when a property
// slot is handed out.
static stock House:GetPropertyHouseInternal(Property:propertyid) {
    if (!(0 <= _:propertyid < _:MAX_PROPERTIES)) {
        return INVALID_HOUSE_ID;
    }

    new const
        House:houseid = gPropertyHouse[propertyid]
    ;

    return IsValidHouseInternal(houseid) ? houseid : INVALID_HOUSE_ID;
}

/**
 * # External
 */

stock PropertyType:GetHousePropertyType() {
    return gHouseType;
}

/**
 * A house is a property that starts locked and starts on the market. Everything
 * about the door itself is the framework doing its job underneath.
 */
stock House:CreateHouse(const name[], Float:extX, Float:extY, Float:extZ, Float:extA, Float:intX, Float:intY, Float:intZ, Float:intA, intInterior, price = HOUSE_DEFAULT_PRICE) {
    new const
        House:houseid = AllocHouseInternal()
    ;

    if (houseid == INVALID_HOUSE_ID) {
        return INVALID_HOUSE_ID;
    }

    new const
        Property:propertyid = CreateProperty(gHouseType, name,
            extX, extY, extZ, extA, 0, 0,
            intX, intY, intZ, intA, PROPERTY_AUTO_WORLD, intInterior
        )
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        FreeHouseInternal(houseid);

        return INVALID_HOUSE_ID;
    }

    gHouseData[houseid][E_HOUSE_CREATED]     = true;
    gHouseData[houseid][E_HOUSE_PROPERTY_ID] = propertyid;
    gHouseData[houseid][E_HOUSE_SAFE_MONEY]  = 0;

    gPropertyHouse[propertyid] = houseid;

    SetPropertyPrice(propertyid, price);
    SetPropertyForSale(propertyid, true);
    // The sale marker, which is a different thing from the arrow on the door:
    // this one says the place can be had, and carries the sign, the price and
    // the blip. A house wears both; a gym would wear only the arrow.
    SetPropertySaleMarker(propertyid,
        HOUSE_FOR_SALE_PICKUP_MODEL, HOUSE_OWNED_PICKUP_MODEL,
        HOUSE_FOR_SALE_MAP_ICON, HOUSE_OWNED_MAP_ICON
    );

    CallLocalFunction("OnHouseCreate", "i", _:houseid);

    return houseid;
}

stock bool:DestroyHouse(House:houseid) {
    if (!IsValidHouseInternal(houseid)) {
        return false;
    }

    new const
        Property:propertyid = gHouseData[houseid][E_HOUSE_PROPERTY_ID]
    ;

    // Given back before the property goes, so that the hook on
    // OnPropertyDestroy finds nothing to free and does not come back round here.
    FreeHouseInternal(houseid);

    DestroyProperty(propertyid);

    CallLocalFunction("OnHouseDestroy", "i", _:houseid);

    return true;
}

stock bool:IsValidHouse(House:houseid) {
    return IsValidHouseInternal(houseid);
}

stock GetHouseCount() {
    return gHouseCount;
}

/**
 * One past the highest house id ever handed out, for walking every house the
 * way GetPropertyPoolSize walks every property.
 */
stock GetHousePoolSize() {
    return gHouseHighWater;
}

/**
 * # Between the two id spaces
 *
 * Pawn has no tag that widens into another one, so these two are the seam. Any
 * function in property.inc is reached through the first of them:
 *
 *     SetPropertyPlayer(GetHouseProperty(houseid), playerid);
 *
 * The handful a house script reaches for on every other line are written out
 * below instead; the rest go through here and stay tag checked at both ends.
 */

stock Property:GetHouseProperty(House:houseid) {
    if (!IsValidHouseInternal(houseid)) {
        return INVALID_PROPERTY_ID;
    }

    return gHouseData[houseid][E_HOUSE_PROPERTY_ID];
}

stock House:GetPropertyHouse(Property:propertyid) {
    return GetPropertyHouseInternal(propertyid);
}

stock bool:IsPropertyHouse(Property:propertyid) {
    return GetPropertyHouseInternal(propertyid) != INVALID_HOUSE_ID;
}

/**
 * # House
 */

stock bool:SetHouseSafeMoney(House:houseid, amount) {
    if (!IsValidHouseInternal(houseid)) {
        return false;
    }

    gHouseData[houseid][E_HOUSE_SAFE_MONEY] = amount;

    return true;
}

stock GetHouseSafeMoney(House:houseid) {
    if (!IsValidHouseInternal(houseid)) {
        return 0;
    }

    return gHouseData[houseid][E_HOUSE_SAFE_MONEY];
}

stock bool:GetHouseName(House:houseid, output[], size = sizeof (output)) {
    return GetPropertyName(GetHouseProperty(houseid), output, size);
}

stock bool:SetHouseLocked(House:houseid, bool:locked) {
    return SetPropertyLocked(GetHouseProperty(houseid), locked);
}

stock bool:IsHouseLocked(House:houseid) {
    return IsPropertyLocked(GetHouseProperty(houseid));
}

stock bool:IsPlayerHouseOwner(playerid, House:houseid) {
    return IsPlayerPropertyOwner(playerid, GetHouseProperty(houseid));
}

/**
 * # Player
 */

stock House:GetPlayerHouse(playerid) {
    return GetPropertyHouseInternal(GetPlayerProperty(playerid));
}

/**
 * Off the chain the framework keeps for a player who is here, so this is a walk
 * over what they hold rather than over every house on the server.
 */
stock CountPlayerHouses(playerid) {
    return CountPlayerProperties(playerid, gHouseType);
}

/**
 * The houses this player holds, off the chain the framework keeps, so this is a
 * walk over what they hold rather than over every house on the server.
 *
 * The array to write into is declared with the ceiling untagged:
 *
 *     new House:houses[_:MAX_HOUSES];
 *
 * MAX_HOUSES carries the tag so that the arrays inside this file can only be
 * reached with a House:, and an array declared at that size the same way ends
 * up wanting a House: for an index as well, which is not what a list wants.
 */
stock GetPlayerHouses(playerid, House:houses[], size = sizeof (houses)) {
    new
        count
    ;

    for (new Property:propertyid = GetPlayerFirstProperty(playerid);
        propertyid != INVALID_PROPERTY_ID && count != size;
        propertyid = GetPropertyNextOwner(propertyid)
    ) {
        new const
            House:houseid = GetPropertyHouseInternal(propertyid)
        ;

        if (houseid == INVALID_HOUSE_ID) {
            continue;
        }

        houses[count++] = houseid;
    }

    return count;
}

/**
 * # Calls
 */

hook OnGameModeInit() {
    gHouseType = RegisterPropertyType("House");

    return 0;
}

// A property slot handed out again is one this fragment may have built on
// before, and it would still be pointing at whatever house was there.
hook OnPropertyCreate(Property:propertyid) {
    gPropertyHouse[propertyid] = INVALID_HOUSE_ID;

    return 0;
}

// A property taken down through the framework rather than through DestroyHouse
// would leave a house standing with nothing under it.
hook OnPropertyDestroy(Property:propertyid) {
    new const
        House:houseid = GetPropertyHouseInternal(propertyid)
    ;

    if (houseid == INVALID_HOUSE_ID) {
        return 0;
    }

    FreeHouseInternal(houseid);

    CallLocalFunction("OnHouseDestroy", "i", _:houseid);

    return 0;
}

// A bought house is shut behind whoever bought it. The lock reads who is
// holding it, so this only has to be said once and the right person keeps
// getting in.
hook OnPlayerBoughtProperty(playerid, Property:propertyid, price, previousPlayer, previousExtra) {
    if (!IsPropertyHouse(propertyid)) {
        return 0;
    }

    SetPropertyLocked(propertyid, true);

    return 0;
}

hook OnPlayerBuyProperty(playerid, Property:propertyid, price) {
    new const
        House:houseid = GetPropertyHouseInternal(propertyid)
    ;

    if (houseid == INVALID_HOUSE_ID) {
        return 0;
    }

    if (CountPlayerHouses(playerid) < MAX_PLAYER_HOUSES) {
        return 0;
    }

    CallLocalFunction("OnPlayerHouseLimit", "ii", playerid, _:houseid);

    return 1;
}
