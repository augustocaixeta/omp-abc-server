#if defined _CORE_PROPERTY_HOUSE
    #endinput
#endif
#define _CORE_PROPERTY_HOUSE

#include <pp-hooks>

/**
 * # Houses
 *
 * property.inc owns the door, the world behind it, the name, the owner and the
 * lock. This owns what makes a house a house: a safe, a lock that starts shut,
 * a limit on how many one character may hold, and the commands for dealing in
 * them.
 *
 * Houses are numbered from zero in their own space. House: and Property: meet
 * in two named functions rather than a cast -- GetHouseProperty going down and
 * GetPropertyHouse coming back up.
 */

/**
 * # Header
 */

#if !defined MAX_HOUSES
    #define MAX_HOUSES (House:512)
#endif

// How many houses one account may hold. Other kinds of property count against
// nothing here.
#if !defined MAX_PLAYER_HOUSES
    #define MAX_PLAYER_HOUSES (3)
#endif

#if !defined HOUSE_DEFAULT_PRICE
    #define HOUSE_DEFAULT_PRICE (100000)
#endif

// The sign on the front lawn, and the blip that goes with it.
#if !defined HOUSE_FOR_SALE_PICKUP_MODEL
    #define HOUSE_FOR_SALE_PICKUP_MODEL (1273)
#endif

#if !defined HOUSE_OWNED_PICKUP_MODEL
    #define HOUSE_OWNED_PICKUP_MODEL (1272)
#endif

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

// A free list: a slot given back points at the one given back before it, and
// slots never used yet come off a high water mark.
static
    gHouseNextFree[_:MAX_HOUSES],
    gHouseFreeHead = -1,
    gHouseHighWater,
    gHouseCount,
    gHouseData[MAX_HOUSES][E_HOUSE_DATA]
;

// Which house a property is, for the callbacks that arrive holding a property.
static
    PropertyType:gHouseType = INVALID_PROPERTY_TYPE,
    House:gPropertyHouse[MAX_PROPERTIES] = { INVALID_HOUSE_ID, ... }
;

/**
 * # Functions
 */

/**
 * @brief      The property type houses are registered under.
 *
 * What tells a property that happens to be a house from one that is a
 * lock-up, for anything holding a property without knowing which it has.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the type, once OnGameModeInit has defined it
 *             - `INVALID_PROPERTY_TYPE` before that
 */
forward PropertyType:GetHousePropertyType();

/**
 * @brief      Put a house in the world, for sale.
 *
 * A door to walk up to, a world behind it, a safe with nothing in it, and a
 * lock that starts shut. It is owned by nobody and priced, so the first
 * thing that happens to it is somebody buying it.
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
 * @param      price        What it sells for.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the house
 *             - `INVALID_HOUSE_ID` when:
 *                 + there are already MAX_HOUSES of them
 *                 + the property behind it could not be created
 */
forward House:CreateHouse(const name[], Float:extX, Float:extY, Float:extZ, Float:extA, Float:intX, Float:intY, Float:intZ, Float:intA, intInterior, price = HOUSE_DEFAULT_PRICE);

/**
 * @brief      Take a house out of the world.
 *
 * The property goes with it, and so does whatever was in the safe. The id
 * goes back on the free list to be handed out again.
 *
 * @param      houseid  House to destroy.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is gone
 *             - `false` `houseid` is not a live house
 */
forward bool:DestroyHouse(House:houseid);

/**
 * @brief      Whether a house exists.
 *
 * @param      houseid  House to check.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is standing
 *             - `false` it is out of range, or was destroyed
 */
forward bool:IsValidHouse(House:houseid);

/**
 * @brief      How many houses are standing.
 *
 * Not the same as the pool size: destroyed ones leave holes.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many there are
 */
forward GetHouseCount();

/**
 * @brief      One past the highest house id ever handed out.
 *
 * What a walk over every house counts to, the way GetPropertyPoolSize walks
 * every property. Ids inside it may be holes, so IsValidHouse still has to
 * be asked.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how far a walk has to go
 */
forward GetHousePoolSize();

/**
 * @brief      The property a house is.
 *
 * The seam between the two id spaces, and how anything in property.inc is
 * reached:
 *
 *     SetPropertyPlayer(GetHouseProperty(houseid), playerid);
 *
 * @param      houseid  House to read.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the property behind it
 *             - `INVALID_PROPERTY_ID` `houseid` is not a live house
 */
forward Property:GetHouseProperty(House:houseid);

/**
 * @brief      The house a property is, if it is one.
 *
 * The seam the other way, for answering a callback that hands over a
 * property and nothing else.
 *
 * @param      propertyid  Property to ask about.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the house it is
 *             - `INVALID_HOUSE_ID` when:
 *                 + `propertyid` is out of range
 *                 + it is some other kind of property
 */
forward House:GetPropertyHouse(Property:propertyid);

/**
 * @brief      Whether a property is a house.
 *
 * @param      propertyid  Property to ask about.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is one
 *             - `false` it is out of range, or some other kind
 */
forward bool:IsPropertyHouse(Property:propertyid);

/**
 * @brief      Say how much is in a house's safe.
 *
 * Written rather than added to, so a caller taking money out reads it,
 * subtracts and writes it back.
 *
 * @param      houseid  House to write.
 * @param      amount   How much is in there now.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the safe holds that
 *             - `false` `houseid` is not a live house
 */
forward bool:SetHouseSafeMoney(House:houseid, amount);

/**
 * @brief      How much is in a house's safe.
 *
 * @param      houseid  House to read.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - what is in there
 *             - `0` `houseid` is not a live house, or the safe is empty
 */
forward GetHouseSafeMoney(House:houseid);

/**
 * @brief      What is written on a house's door.
 *
 * @param      houseid  House to read.
 * @param      output   Buffer the name is written into.
 * @param      size     Size of that buffer.
 * @param      kind     `true` writes what kind of property it is instead of
 *                      which one it is.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` something was written
 *             - `false` `houseid` is not a live house
 */
forward bool:GetHouseName(House:houseid, output[], size = sizeof (output), bool:kind = false);

/**
 * @brief      Lock or unlock a house.
 *
 * A locked door turns away anybody who is not its owner. Houses are created
 * shut and are shut again behind whoever buys one.
 *
 * @param      houseid  House to lock.
 * @param      locked   `true` shut, `false` open.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the door is as asked
 *             - `false` `houseid` is not a live house
 */
forward bool:SetHouseLocked(House:houseid, bool:locked);

/**
 * @brief      Whether a house is locked.
 *
 * @param      houseid  House to check.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the door is shut
 *             - `false` it is open, or `houseid` is not a live house
 */
forward bool:IsHouseLocked(House:houseid);

/**
 * @brief      Whether a house is a character's.
 *
 * @param      playerid  Character to ask about.
 * @param      houseid   House to ask about.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is theirs
 *             - `false` it is somebody else's, unsold, or not a live house
 */
forward bool:IsPlayerHouseOwner(playerid, House:houseid);

/**
 * @brief      The house a character is standing at or inside.
 *
 * Whichever property they are at, turned back into a house. What a command
 * run on the spot reads instead of asking for an id.
 *
 * @param      playerid  Character to ask about.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the house they are at
 *             - `INVALID_HOUSE_ID` they are at no property, or at one that is
 *               not a house
 */
forward House:GetPlayerHouse(playerid);

/**
 * @brief      How many houses a character holds.
 *
 * What MAX_PLAYER_HOUSES is measured against. Walks what they own rather
 * than every house on the server.
 *
 * @param      playerid  Character to count for.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many are theirs
 */
forward CountPlayerHouses(playerid);

/**
 * @brief      The houses a character holds.
 *
 * Declare the array with the ceiling untagged, since MAX_HOUSES carries the
 * House: tag and a list wants a plain index:
 *
 *     new House:houses[_:MAX_HOUSES];
 *
 * @param      playerid  Character to list for.
 * @param      houses    Array the houses are written into.
 * @param      size      How many will fit in it.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many were written, up to `size`
 */
forward GetPlayerHouses(playerid, House:houses[], size = sizeof (houses));

/**
 * # Events
 */

/**
 * @brief      A house has been built.
 *
 * @param      houseid  The new house.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 */
forward OnHouseCreate(House:houseid);

/**
 * @brief      A house is going away.
 *
 * Sent by DestroyHouse, and by a property being destroyed underneath one.
 * The id has already been given back by the time this is called, so it says
 * which house ended rather than pointing at one still standing.
 *
 * @param      houseid  The house that ended.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 */
forward OnHouseDestroy(House:houseid);

/**
 * @brief      A character already holds as many houses as they may.
 *
 * Sent instead of OnPlayerBuyProperty, which never sees the sale at all, so
 * this is the only chance to say why nothing happened. Nothing has changed
 * hands and no money has moved.
 *
 * @param      playerid  Character who tried to buy.
 * @param      houseid   House they tried to buy.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerHouseLimit(playerid, House:houseid);

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

// Bounds checked and not asked whether the property is still standing:
// OnPropertyDestroy arrives after it has been given back.
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

    // Before the property goes, so the OnPropertyDestroy hook finds nothing to
    // free and does not come back round here.
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

stock GetHousePoolSize() {
    return gHouseHighWater;
}

/**
 * # Between the two id spaces
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

stock bool:GetHouseName(House:houseid, output[], size = sizeof (output), bool:kind = false) {
    return GetPropertyName(GetHouseProperty(houseid), output, size, kind);
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

stock CountPlayerHouses(playerid) {
    return CountPlayerProperties(playerid, gHouseType);
}

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
    gHouseType = DefinePropertyType("House");

    return 0;
}

hook OnPropertyCreate(Property:propertyid) {
    gPropertyHouse[propertyid] = INVALID_HOUSE_ID;

    return 0;
}

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

// A bought house is shut behind whoever bought it.
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

// Buying happens on the doorstep and selling from the sofa, so both count.
static stock Property:PlayerPropertyInternal(playerid) {
    new const
        Property:propertyid = GetPlayerProperty(playerid)
    ;

    if (propertyid != INVALID_PROPERTY_ID) {
        return propertyid;
    }

    return GetButtonProperty(GetPlayerButton(playerid));
}

hook OnPlayerHouseLimit(playerid, House:houseid) {
    SendPlayerNotice(playerid, "You already hold %i houses, which is all anyone may.", MAX_PLAYER_HOUSES);

    return 0;
}

CMD:buy(playerid, params[]) {
    new const
        Property:propertyid = GetPlayerSaleProperty(playerid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        SendPlayerNotice(playerid, "There is nothing for sale here.");

        return 1;
    }

    if (IsPropertyOwned(propertyid)) {
        SendPlayerNotice(playerid, "That one is not on the market.");

        return 1;
    }

    // Everything a sale is refused for says so on its way past.
    BuyProperty(playerid, propertyid);

    return 1;
}

CMD:sell(playerid, params[]) {
    new const
        Property:propertyid = PlayerPropertyInternal(playerid)
    ;

    if (!IsPlayerPropertyOwner(playerid, propertyid)) {
        SendPlayerNotice(playerid, "You can only give up what is yours.");

        return 1;
    }

    SellProperty(playerid, propertyid);

    return 1;
}

CMD:lock(playerid, params[]) {
    new const
        Property:propertyid = PlayerPropertyInternal(playerid)
    ;

    if (!IsPlayerPropertyOwner(playerid, propertyid)) {
        SendPlayerNotice(playerid, "You can only lock what is yours.");

        return 1;
    }

    new const
        bool:locked = !IsPropertyLocked(propertyid)
    ;

    SetPropertyLocked(propertyid, locked);

    SendPlayerNotice(playerid, locked ? ("Locked.") : ("Unlocked."));

    return 1;
}

CMD:houses(playerid, params[]) {
    new
        House:houses[MAX_PLAYER_HOUSES]
    ;

    new const
        count = GetPlayerHouses(playerid, houses)
    ;

    if (count == 0) {
        SendPlayerNotice(playerid, "You own no houses.");

        return 1;
    }

    SendClientMessage(playerid, -1, "Your houses (%i of %i):", count, MAX_PLAYER_HOUSES);

    for (new i; i != count; ++i) {
        new
            name[MAX_PROPERTY_NAME]
        ;

        GetHouseName(houses[i], name);

        SendClientMessage(playerid, -1, "  %s%s", name, IsHouseLocked(houses[i]) ? (" (locked)") : (""));
    }

    return 1;
}
