#if defined _INC_PROPERTY
    #endinput
#endif
#define _INC_PROPERTY

// A property is a door with a name on it, an owner behind it and a lock in it.
// entrance.inc builds the door, and comes before pp-hooks because it pulls
// pp-hooks in itself: hooks written after the last include of it are the ones
// that get a number of their own.
//
// y_va is not included here. The framework already pulls it in for ___, and
// this file uses nothing of YSI's.
#include <foreach>

#include <humanize>
#include <I\entrance>

#include <pp-hooks>

/**
 * # Header
 */

#if !defined MAX_PROPERTIES
    #define MAX_PROPERTIES (Property:1024)
#endif

// One slot per kind of property a script writes a fragment for: houses,
// businesses, workshops. Registered once and never given back, so there is no
// free list here.
#if !defined MAX_PROPERTY_TYPES
    #define MAX_PROPERTY_TYPES (PropertyType:32)
#endif

#if !defined MAX_PROPERTY_NAME
    #define MAX_PROPERTY_NAME (32)
#endif

#if !defined MAX_PROPERTY_TYPE_NAME
    #define MAX_PROPERTY_TYPE_NAME (24)
#endif

// A house and the house next door are the same interior, so two players who
// think they are in two different rooms would be standing on top of each other.
// Every property is given a virtual world of its own, counted from here. A
// script that hands out worlds itself either raises this above whatever it uses
// or passes a world in and takes the job over.
#if !defined PROPERTY_WORLD_BASE
    #define PROPERTY_WORLD_BASE (10000)
#endif

// What the prompt outside reads. The name is on the first line because it is
// what tells one door from the next; the key is on the second because it is the
// same key on every one of them.
#if !defined PROPERTY_KEY_TEXT
    #define PROPERTY_KEY_TEXT "~k~~VEHICLE_ENTER_EXIT~"
#endif

#if !defined PROPERTY_EXTERIOR_POPUP
    #define PROPERTY_EXTERIOR_POPUP "%s~n~Press %s to enter"
#endif

#if !defined PROPERTY_EXTERIOR_POPUP_LOCKED
    #define PROPERTY_EXTERIOR_POPUP_LOCKED "%s~n~This %s is locked."
#endif

#if !defined PROPERTY_INTERIOR_POPUP
    #define PROPERTY_INTERIOR_POPUP "Press %s to leave"
#endif

// Buying takes two presses, because one key away from owning something is one
// key away from owning it by accident.
#if !defined PROPERTY_BUY_KEY
    #define PROPERTY_BUY_KEY (KEY_WALK)
#endif

#if !defined PROPERTY_CONFIRM_KEY
    #define PROPERTY_CONFIRM_KEY (KEY_YES)
#endif

#if !defined PROPERTY_BUY_KEY_TEXT
    #define PROPERTY_BUY_KEY_TEXT "~k~~SNEAK_ABOUT~"
#endif

#if !defined PROPERTY_CONFIRM_KEY_TEXT
    #define PROPERTY_CONFIRM_KEY_TEXT "~k~~CONVERSATION_YES~"
#endif

#if !defined PROPERTY_BUY_POPUP
    #define PROPERTY_BUY_POPUP "Press %s to purchase this %s for $%s."
#endif

#if !defined PROPERTY_CONFIRM_POPUP
    #define PROPERTY_CONFIRM_POPUP "Press %s to confirm purchase for $%s."
#endif

#if !defined PROPERTY_PICKUP_TYPE
    #define PROPERTY_PICKUP_TYPE (1)
#endif

// Two markers, because they answer two different questions and a player is
// entitled to want one without the other. The arrow says there is a way in and
// out here; the sale marker says the place can be had, and carries the sign,
// the words and the blip that go with being on the market.
//
// The arrow is the same on every property, so it is decided when the script is
// built: setting it to nothing leaves the doorways bare. The sale marker is per
// property, because only some kinds of place are ever bought, and it starts as
// nothing at all: a gym is not for sale and should not be wearing a price.
#if !defined PROPERTY_ENTRANCE_PICKUP_MODEL
    #define PROPERTY_ENTRANCE_PICKUP_MODEL (1318)
#endif

#if !defined PROPERTY_SALE_LABEL_COLOUR
    #define PROPERTY_SALE_LABEL_COLOUR (0xFFFF99FF)
#endif

#if !defined PROPERTY_SALE_LABEL_DISTANCE
    #define PROPERTY_SALE_LABEL_DISTANCE (12.0)
#endif

#if !defined PROPERTY_SALE_LABEL_OFFSET_Z
    #define PROPERTY_SALE_LABEL_OFFSET_Z (0.5)
#endif

// The kind as it was registered rather than as it reads mid sentence, because
// this one starts a line: a Hotel suite, not a hotel suite.
#if !defined PROPERTY_SALE_LABEL_FOR_SALE
    #define PROPERTY_SALE_LABEL_FOR_SALE "%s: %s\nPrice: $%s"
#endif

#if !defined PROPERTY_SALE_LABEL_OWNED
    #define PROPERTY_SALE_LABEL_OWNED "%s: %s"
#endif

#if !defined PROPERTY_MAP_ICON_COLOUR
    #define PROPERTY_MAP_ICON_COLOUR (0)
#endif

#if !defined PROPERTY_MAP_ICON_STYLE
    #define PROPERTY_MAP_ICON_STYLE (MAPICON_LOCAL)
#endif

// The sale marker stands where it stands, and a player has to be able to reach
// for it there. An area of its own goes up with it, so that buying happens on
// the sign the way the story mode does it, and not only at the door the sign is
// pointing at.
#if !defined PROPERTY_SALE_BUTTON_SIZE
    #define PROPERTY_SALE_BUTTON_SIZE (1.5)
#endif

#define INVALID_PROPERTY_ID (Property:-1)
#define INVALID_PROPERTY_TYPE (PropertyType:-1)

// Passed as the interior world to ask for a world of this property's own.
#define PROPERTY_AUTO_WORLD (-1)

// What GetPropertyExtra reads as until a script writes something else there.
#define PROPERTY_NO_EXTRA (0)

// The name is packed: thirty two characters live in eight cells instead of
// thirty two, and every property carries the room for it either way.
static enum _:E_PROPERTY_DATA {
    bool:E_PROPERTY_CREATED,

    PropertyType:E_PROPERTY_TYPE,
    E_PROPERTY_NAME[MAX_PROPERTY_NAME char],

    bool:E_PROPERTY_OWNED,
    E_PROPERTY_EXTRA,
    E_PROPERTY_PRICE,
    bool:E_PROPERTY_LOCKED,

    // What lets DestroyProperty skip walking the players when there is nobody
    // in the room, which is nearly always.
    E_PROPERTY_PLAYER_COUNT,

    // The deeds one player holds, threaded through the properties themselves
    // rather than gathered by walking all of them. Which player, so that a
    // record knows whose chain it is in and can take itself out without being
    // looked for; and both ways along, so that taking it out is a handful of
    // writes instead of a walk.
    //
    // This is a slot and not a person: it is INVALID_PLAYER_ID while the owner
    // is away, and it says nothing about whether the place is owned.
    E_PROPERTY_OWNER_PLAYER_ID,
    Property:E_PROPERTY_OWNER_NEXT,
    Property:E_PROPERTY_OWNER_PREV,

    // The arrows, one on each side of the door.
    STREAMER_TAG_PICKUP:E_PROPERTY_EXT_PICKUP_ID,
    STREAMER_TAG_PICKUP:E_PROPERTY_INT_PICKUP_ID,

    // The sale marker: what it shows in each state, where it stands, and the
    // three things it is made of. Its spot is kept rather than taken from the
    // door every time, so that a script which moves it off the doorstep still
    // has it there after the state changes and it is built again.
    E_PROPERTY_SALE_MODEL_FOR_SALE,
    E_PROPERTY_SALE_MODEL_OWNED,
    E_PROPERTY_SALE_ICON_FOR_SALE,
    E_PROPERTY_SALE_ICON_OWNED,

    Float:E_PROPERTY_SALE_X,
    Float:E_PROPERTY_SALE_Y,
    Float:E_PROPERTY_SALE_Z,

    Button:E_PROPERTY_SALE_BUTTON_ID,

    STREAMER_TAG_PICKUP:E_PROPERTY_SALE_PICKUP_ID,
    STREAMER_TAG_MAP_ICON:E_PROPERTY_SALE_MAP_ICON_ID,
    STREAMER_TAG_3D_TEXT_LABEL:E_PROPERTY_SALE_3DTEXT_ID,

    Entrance:E_PROPERTY_ENTRANCE_ID
};

static enum _:E_PROPERTY_TYPE_DATA {
    E_PROPERTY_TYPE_NAME[MAX_PROPERTY_TYPE_NAME char],
    E_PROPERTY_TYPE_COUNT,
    bool:E_PROPERTY_TYPE_REGISTERED
};

// The array hands out the ids as well as holding the properties, the same way
// the doors and the buttons under them are kept: a free list, with slots never
// used yet coming off a high water mark. See the note in button.inc for what
// that was measured against.
static
    gPropertyNextFree[_:MAX_PROPERTIES],
    gPropertyFreeHead = -1,
    gPropertyHighWater,
    gPropertyCount,
    gPropertyData[MAX_PROPERTIES][E_PROPERTY_DATA],
    gPropertyTypeData[MAX_PROPERTY_TYPES][E_PROPERTY_TYPE_DATA],
    gPropertyTypeCount
;

// Which property an entrance belongs to, the other way round. Every press on
// either side of every door in the server arrives holding an entrance, and this
// is what turns it back into a property in one array read.
static
    Property:gEntranceProperty[MAX_ENTRANCES] = { INVALID_PROPERTY_ID, ... }
;

// Which property a sale marker area belongs to. A door is already reachable the
// other way round through the entrance it belongs to; this one has no door
// under it, so it needs a way back of its own.
static
    Property:gSaleButtonProperty[MAX_BUTTONS] = { INVALID_PROPERTY_ID, ... }
;

// Which property a player is inside.
static
    Property:gPlayerProperty[MAX_PLAYERS] = { INVALID_PROPERTY_ID, ... }
;

// Which property this player has been asked to confirm buying. Per player and
// not per property, because two people can stand at the same door and only one
// of them has said anything yet.
static
    Property:gPlayerPendingProperty[MAX_PLAYERS] = { INVALID_PROPERTY_ID, ... }
;

// Where each player chain starts, and how long it is. Asking a player what they
// hold is then a read, and walking it is a walk over what they hold rather than
// over every property on the server: at four thousand of them that walk
// measured 440 microseconds, and a free list cannot be searched any faster,
// because what it hands out is in no order to binary search through.
//
// Nothing on the server is searched for an owner, because nothing here knows
// what an owner is. A script says which player holds which property as it loads
// them, and from then on every question about who holds what is this chain.
static
    Property:gPlayerPropertyHead[MAX_PLAYERS] = { INVALID_PROPERTY_ID, ... },
    gPlayerPropertyCount[MAX_PLAYERS]
;

/**
 * # Functions
 */

forward PropertyType:DefinePropertyType(const name[]);
forward bool:IsValidPropertyType(PropertyType:type);
forward GetPropertyTypeCount();
forward bool:GetPropertyTypeName(PropertyType:type, output[], size = sizeof (output));

forward Property:CreateProperty(PropertyType:type, const name[], Float:extX, Float:extY, Float:extZ, Float:extA, extWorld, extInterior, Float:intX, Float:intY, Float:intZ, Float:intA, intWorld = PROPERTY_AUTO_WORLD, intInterior = 0, KEY:buttonKey = ENTRANCE_DEFAULT_BUTTON_KEY, priority = ENTRANCE_DEFAULT_BUTTON_PRIORITY);
forward bool:DestroyProperty(Property:propertyid);
forward bool:IsValidProperty(Property:propertyid);
forward GetPropertyCount(PropertyType:type = INVALID_PROPERTY_TYPE);
forward GetPropertyPoolSize();

forward PropertyType:GetPropertyType(Property:propertyid);
forward bool:SetPropertyName(Property:propertyid, const name[], OPEN_MP_TAGS:...);
forward bool:GetPropertyName(Property:propertyid, output[], size = sizeof (output), bool:kind = false);
forward bool:SetPropertyOwned(Property:propertyid, bool:owned);
forward bool:IsPropertyOwned(Property:propertyid);
forward bool:SetPropertyExtra(Property:propertyid, extra);
forward GetPropertyExtra(Property:propertyid);
forward bool:SetPropertyPrice(Property:propertyid, price);
forward GetPropertyPrice(Property:propertyid);
forward bool:SetPropertyLocked(Property:propertyid, bool:locked);
forward bool:IsPropertyLocked(Property:propertyid);
forward GetPropertyPlayerCount(Property:propertyid);
forward bool:SetPropertySaleMarker(Property:propertyid, forSaleModel, ownedModel, forSaleIcon = 0, ownedIcon = 0);
forward bool:SetPropertySaleMarkerPos(Property:propertyid, Float:x, Float:y, Float:z);
forward bool:GetPropertySaleMarkerPos(Property:propertyid, &Float:x, &Float:y, &Float:z);
forward GetPropertySaleMarkerModel(Property:propertyid);
forward GetPropertySaleMarkerIcon(Property:propertyid);
forward bool:RefreshPropertySaleMarker(Property:propertyid);
forward STREAMER_TAG_PICKUP:GetPropertySaleMarkerPickup(Property:propertyid);
forward STREAMER_TAG_MAP_ICON:GetPropertySaleMarkerMapIcon(Property:propertyid);
forward STREAMER_TAG_3D_TEXT_LABEL:GetPropertySaleMarkerLabel(Property:propertyid);
forward STREAMER_TAG_PICKUP:GetPropertyEntrancePickup(Property:propertyid, bool:inside = false);
forward Button:GetPropertySaleMarkerButton(Property:propertyid);
forward Property:GetPlayerSaleProperty(playerid);
forward bool:GetPlayerPropertyPrompt(playerid, output[], size = sizeof (output));
forward Property:GetPlayerPendingProperty(playerid);
forward bool:CancelPlayerPropertyPurchase(playerid);

forward Entrance:GetPropertyEntrance(Property:propertyid);
forward Button:GetPropertyExteriorButton(Property:propertyid);
forward Button:GetPropertyInteriorButton(Property:propertyid);
forward GetPropertyVirtualWorld(Property:propertyid);

forward Property:GetEntranceProperty(Entrance:entranceid);
forward Property:GetButtonProperty(Button:buttonid);

forward bool:SetPropertyPlayer(Property:propertyid, playerid);
forward GetPropertyPlayer(Property:propertyid);
forward bool:IsPlayerPropertyOwner(playerid, Property:propertyid);
forward CountPlayerProperties(playerid, PropertyType:type = INVALID_PROPERTY_TYPE);
forward Property:GetPlayerFirstProperty(playerid);
forward Property:GetPropertyNextOwner(Property:propertyid);
forward GetPlayerProperties(playerid, Property:properties[], size = sizeof (properties), PropertyType:type = INVALID_PROPERTY_TYPE);

forward bool:BuyProperty(playerid, Property:propertyid);
forward bool:SellProperty(playerid, Property:propertyid);

forward Property:GetPlayerProperty(playerid);
forward bool:IsPlayerInsideProperty(playerid);
forward bool:SetPlayerPropertyInterior(playerid, Property:propertyid, Float:offset = ENTRANCE_DEFAULT_FORWARD_OFFSET);
forward bool:SetPlayerPropertyExterior(playerid, Property:propertyid, Float:offset = ENTRANCE_DEFAULT_FORWARD_OFFSET);

/**
 * # Events
 *
 * Where a return of 1 stops something, it stops it the way pp-hooks stops
 * anything: a hook that returns something other than zero ends the chain there,
 * so neither the hooks after it nor the callback itself are reached.
 */

forward OnPropertyCreate(Property:propertyid);

/**
 * # OnPropertyCreate
 *
 * A slot handed out again is a slot something else was built on before, so a
 * fragment keeping data of its own beside a property clears it here.
 *
 * # Returns
 *   - This callback does not handle returns.
 */

forward OnPropertyDestroy(Property:propertyid);

/**
 * # OnPropertyDestroy
 *
 * # Returns
 *   - This callback does not handle returns.
 */

forward OnPlayerEnterProperty(playerid, Property:propertyid);

/**
 * # OnPlayerEnterProperty
 *
 * # Returns
 *   - `0`: Allows the player to be set inside the property.
 *   - `1`: Prevents the player from being set inside the property.
 */

forward OnPlayerLeaveProperty(playerid, Property:propertyid);

/**
 * # OnPlayerLeaveProperty
 *
 * # Returns
 *   - `0`: Allows the player to be set outside the property.
 *   - `1`: Prevents the player from being set outside the property.
 */

forward OnPlayerEnteredProperty(playerid, Property:propertyid);

/**
 * # OnPlayerEnteredProperty
 *
 * # Returns
 *   - This callback does not handle returns.
 */

forward OnPlayerLeftProperty(playerid, Property:propertyid);

/**
 * # OnPlayerLeftProperty
 *
 * # Returns
 *   - This callback does not handle returns.
 */

forward OnPlayerPropertyLocked(playerid, Property:propertyid);

/**
 * # OnPlayerPropertyLocked
 *
 * The door was shut against this player. Called instead of
 * OnPlayerEnterProperty, not before it, so that a refusal is one event and not
 * two.
 *
 * # Returns
 *   - This callback does not handle returns.
 */

forward OnPlayerEnterPropertyArea(playerid, Property:propertyid, bool:inside);

/**
 * # OnPlayerEnterPropertyArea
 *
 * # Returns
 *   - `0`: (Continue) Continues subsequent calls to the entrance area callback.
 *   - `1`: (OK) Breaks subsequent calls to the entrance area callback.
 */

forward OnPlayerLeavePropertyArea(playerid, Property:propertyid, bool:inside);

/**
 * # OnPlayerLeavePropertyArea
 *
 * # Returns
 *   - `0`: (Continue) Continues subsequent calls to the entrance area callback.
 *   - `1`: (OK) Breaks subsequent calls to the entrance area callback.
 */

forward OnPlayerPropertyPromptChange(playerid, Property:propertyid);

/**
 * # OnPlayerPropertyPromptChange
 *
 * What this player should be reading at this door has changed: they armed a
 * purchase, gave one up, or went through with one. This library keeps the words
 * and never puts them on screen, the same way button.inc does, so this is the
 * script being told to show them again through GetPlayerPropertyPrompt.
 *
 * # Returns
 *   - This callback does not handle returns.
 */

forward OnPlayerBuyProperty(playerid, Property:propertyid, price);

/**
 * # OnPlayerBuyProperty
 *
 * Where the script says whether the player can afford it. The property still
 * belongs to whoever it belonged to, so GetPropertyPlayer and GetPropertyExtra
 * both read the seller here.
 *
 * # Returns
 *   - `0`: Allows the sale to go through.
 *   - `1`: Refuses the sale.
 */

forward OnPlayerBoughtProperty(playerid, Property:propertyid, price, previousPlayer, previousExtra);

/**
 * # OnPlayerBoughtProperty
 *
 * Where the script moves the money, and where it writes on the deed whatever it
 * needs in order to find this place again — SetPropertyExtra, with the buyer's
 * account id or row id or whatever the script keeps.
 *
 * What the seller had is handed over because it cannot be read back afterwards.
 * `previousPlayer` is INVALID_PLAYER_ID when the place was going spare or its
 * owner is away, and `previousExtra` is what was written on the deed, which is
 * how a seller who is not here gets paid.
 *
 * # Returns
 *   - This callback does not handle returns.
 */

forward OnPlayerSellProperty(playerid, Property:propertyid, price);

/**
 * # OnPlayerSellProperty
 *
 * # Returns
 *   - `0`: Allows the property to be given up.
 *   - `1`: Refuses it.
 */

forward OnPlayerSoldProperty(playerid, Property:propertyid, price);

/**
 * # OnPlayerSoldProperty
 *
 * Where the script pays the seller, whatever part of the price it pays back.
 *
 * # Returns
 *   - This callback does not handle returns.
 */

/**
 * # Internal
 */

static stock GetFreePropertyIndexInternal() {
    if (gPropertyFreeHead != -1) {
        new const
            index = gPropertyFreeHead
        ;

        gPropertyFreeHead = gPropertyNextFree[index];

        return index;
    }

    if (gPropertyHighWater != _:MAX_PROPERTIES) {
        return gPropertyHighWater++;
    }

    return -1;
}

static stock Property:AllocPropertyInternal() {
    new const
        index = GetFreePropertyIndexInternal()
    ;

    if (index == -1) {
        return INVALID_PROPERTY_ID;
    }

    ++gPropertyCount;

    return Property:index;
}

static stock FreePropertyInternal(Property:propertyid) {
    gPropertyNextFree[_:propertyid] = gPropertyFreeHead;
    gPropertyFreeHead = _:propertyid;

    --gPropertyCount;
}

static stock bool:IsValidPropertyInternal(Property:propertyid) {
    if (!(0 <= _:propertyid < _:MAX_PROPERTIES)) {
        return false;
    }

    return gPropertyData[propertyid][E_PROPERTY_CREATED];
}

static stock bool:IsValidPropertyTypeInternal(PropertyType:type) {
    if (!(0 <= _:type < _:MAX_PROPERTY_TYPES)) {
        return false;
    }

    return gPropertyTypeData[type][E_PROPERTY_TYPE_REGISTERED];
}

static stock Property:GetEntrancePropertyInternal(Entrance:entranceid) {
    if (!(0 <= _:entranceid < _:MAX_ENTRANCES)) {
        return INVALID_PROPERTY_ID;
    }

    new const
        Property:propertyid = gEntranceProperty[entranceid]
    ;

    return IsValidPropertyInternal(propertyid) ? propertyid : INVALID_PROPERTY_ID;
}

static stock RefreshPropertyPopupInternal(Property:propertyid) {
    new const
        Button:buttonid = GetEntranceExteriorButton(gPropertyData[propertyid][E_PROPERTY_ENTRANCE_ID])
    ;

    new
        name[MAX_PROPERTY_NAME]
    ;

    strunpack(name, gPropertyData[propertyid][E_PROPERTY_NAME]);

    if (gPropertyData[propertyid][E_PROPERTY_LOCKED]) {
        new
            kind[MAX_PROPERTY_TYPE_NAME]
        ;

        GetPropertyKindInternal(propertyid, kind);
        SetButtonPopupText(buttonid, PROPERTY_EXTERIOR_POPUP_LOCKED, name, kind);

        return;
    }

    SetButtonPopupText(buttonid, PROPERTY_EXTERIOR_POPUP, name, PROPERTY_KEY_TEXT);
}

static stock GetPropertyKindInternal(Property:propertyid, output[], size = sizeof (output)) {
    new const
        PropertyType:type = gPropertyData[propertyid][E_PROPERTY_TYPE]
    ;

    strunpack(output, gPropertyTypeData[type][E_PROPERTY_TYPE_NAME], size);

    for (new i; output[i] != EOS; ++i) {
        output[i] = tolower(output[i]);
    }
}

static stock GetPropertySaleModelInternal(Property:propertyid) {
    return gPropertyData[propertyid][E_PROPERTY_OWNED]
        ? gPropertyData[propertyid][E_PROPERTY_SALE_MODEL_OWNED]
        : gPropertyData[propertyid][E_PROPERTY_SALE_MODEL_FOR_SALE];
}

static stock GetPropertySaleIconInternal(Property:propertyid) {
    return gPropertyData[propertyid][E_PROPERTY_OWNED]
        ? gPropertyData[propertyid][E_PROPERTY_SALE_ICON_OWNED]
        : gPropertyData[propertyid][E_PROPERTY_SALE_ICON_FOR_SALE];
}

static stock DestroyPropertySaleButtonInternal(Property:propertyid) {
    new const
        Button:buttonid = gPropertyData[propertyid][E_PROPERTY_SALE_BUTTON_ID]
    ;

    gPropertyData[propertyid][E_PROPERTY_SALE_BUTTON_ID] = INVALID_BUTTON_ID;

    if (!IsValidButton(buttonid)) {
        return;
    }

    gSaleButtonProperty[buttonid] = INVALID_PROPERTY_ID;
    DestroyButton(buttonid);
}

static stock DestroyPropertySaleSignInternal(Property:propertyid) {
    if (IsValidDynamicPickup(gPropertyData[propertyid][E_PROPERTY_SALE_PICKUP_ID])) {
        DestroyDynamicPickup(gPropertyData[propertyid][E_PROPERTY_SALE_PICKUP_ID]);
    }

    if (IsValidDynamicMapIcon(gPropertyData[propertyid][E_PROPERTY_SALE_MAP_ICON_ID])) {
        DestroyDynamicMapIcon(gPropertyData[propertyid][E_PROPERTY_SALE_MAP_ICON_ID]);
    }

    if (IsValidDynamic3DTextLabel(gPropertyData[propertyid][E_PROPERTY_SALE_3DTEXT_ID])) {
        DestroyDynamic3DTextLabel(gPropertyData[propertyid][E_PROPERTY_SALE_3DTEXT_ID]);
    }

    gPropertyData[propertyid][E_PROPERTY_SALE_PICKUP_ID]   = STREAMER_TAG_PICKUP:INVALID_STREAMER_ID;
    gPropertyData[propertyid][E_PROPERTY_SALE_MAP_ICON_ID] = STREAMER_TAG_MAP_ICON:INVALID_STREAMER_ID;
    gPropertyData[propertyid][E_PROPERTY_SALE_3DTEXT_ID]   = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;
}

static stock RefreshPropertySaleMarkerInternal(Property:propertyid) {
    DestroyPropertySaleSignInternal(propertyid);

    new const
        Entrance:entranceid = gPropertyData[propertyid][E_PROPERTY_ENTRANCE_ID],
        worldid = GetEntranceExteriorVirtualWorld(entranceid),
        interiorid = GetEntranceExteriorInterior(entranceid),
        Float:x = gPropertyData[propertyid][E_PROPERTY_SALE_X],
        Float:y = gPropertyData[propertyid][E_PROPERTY_SALE_Y],
        Float:z = gPropertyData[propertyid][E_PROPERTY_SALE_Z]
    ;

    if (!IsValidButton(gPropertyData[propertyid][E_PROPERTY_SALE_BUTTON_ID])) {
        new const
            Button:buttonid = CreateButton(x, y, z, PROPERTY_BUY_KEY, PROPERTY_SALE_BUTTON_SIZE, worldid, interiorid)
        ;

        gPropertyData[propertyid][E_PROPERTY_SALE_BUTTON_ID] = buttonid;

        if (buttonid != INVALID_BUTTON_ID) {
            gSaleButtonProperty[buttonid] = propertyid;
        }
    }

    new const
        model = GetPropertySaleModelInternal(propertyid),
        icon = GetPropertySaleIconInternal(propertyid)
    ;

    if (!model && !icon) {
        return;
    }

    if (model) {
        gPropertyData[propertyid][E_PROPERTY_SALE_PICKUP_ID] = CreateDynamicPickup(model, PROPERTY_PICKUP_TYPE, x, y, z, worldid, interiorid);
    }

    if (icon) {
        gPropertyData[propertyid][E_PROPERTY_SALE_MAP_ICON_ID] = CreateDynamicMapIcon(x, y, z, icon, PROPERTY_MAP_ICON_COLOUR, worldid, interiorid, .style = PROPERTY_MAP_ICON_STYLE);
    }

    new
        kind[MAX_PROPERTY_TYPE_NAME],
        name[MAX_PROPERTY_NAME],
        text[MAX_PROPERTY_NAME + MAX_PROPERTY_TYPE_NAME + 32]
    ;

    GetPropertyTypeName(gPropertyData[propertyid][E_PROPERTY_TYPE], kind, sizeof (kind));
    strunpack(name, gPropertyData[propertyid][E_PROPERTY_NAME]);

    if (!gPropertyData[propertyid][E_PROPERTY_OWNED]) {
        new
            price[16]
        ;

        HumanizeThousand(gPropertyData[propertyid][E_PROPERTY_PRICE], price);
        format(text, sizeof (text), PROPERTY_SALE_LABEL_FOR_SALE, kind, name, price);
    } else {
        format(text, sizeof (text), PROPERTY_SALE_LABEL_OWNED, kind, name);
    }

    gPropertyData[propertyid][E_PROPERTY_SALE_3DTEXT_ID] = CreateDynamic3DTextLabel(
        text, PROPERTY_SALE_LABEL_COLOUR,
        x, y, z + PROPERTY_SALE_LABEL_OFFSET_Z,
        PROPERTY_SALE_LABEL_DISTANCE,
        .worldid = worldid, .interiorid = interiorid
    );
}

static stock CreatePropertyEntrancePickupsInternal(Property:propertyid) {
    #if !PROPERTY_ENTRANCE_PICKUP_MODEL
        #pragma unused propertyid
    #else
        new
            Float:x,
            Float:y,
            Float:z
        ;

        new const
            Entrance:entranceid = gPropertyData[propertyid][E_PROPERTY_ENTRANCE_ID]
        ;

        GetEntranceExteriorPos(entranceid, x, y, z);

        gPropertyData[propertyid][E_PROPERTY_EXT_PICKUP_ID] = CreateDynamicPickup(
            PROPERTY_ENTRANCE_PICKUP_MODEL, PROPERTY_PICKUP_TYPE, x, y, z,
            GetEntranceExteriorVirtualWorld(entranceid),
            GetEntranceExteriorInterior(entranceid)
        );

        GetEntranceInteriorPos(entranceid, x, y, z);

        gPropertyData[propertyid][E_PROPERTY_INT_PICKUP_ID] = CreateDynamicPickup(
            PROPERTY_ENTRANCE_PICKUP_MODEL, PROPERTY_PICKUP_TYPE, x, y, z,
            GetEntranceInteriorVirtualWorld(entranceid),
            GetEntranceInteriorInterior(entranceid)
        );
    #endif
}

static stock Property:GetPlayerSalePropertyInternal(playerid) {
    new const
        Button:buttonid = GetPlayerButton(playerid)
    ;

    if (!(0 <= _:buttonid < _:MAX_BUTTONS)) {
        return INVALID_PROPERTY_ID;
    }

    new const
        Property:propertyid = gSaleButtonProperty[buttonid]
    ;

    return IsValidPropertyInternal(propertyid) ? propertyid : INVALID_PROPERTY_ID;
}

static stock UnlinkPropertyOwnerInternal(Property:propertyid) {
    new const
        playerid = gPropertyData[propertyid][E_PROPERTY_OWNER_PLAYER_ID]
    ;

    if (playerid == INVALID_PLAYER_ID) {
        return;
    }

    new const
        Property:prev = gPropertyData[propertyid][E_PROPERTY_OWNER_PREV],
        Property:next = gPropertyData[propertyid][E_PROPERTY_OWNER_NEXT]
    ;

    if (prev == INVALID_PROPERTY_ID) {
        gPlayerPropertyHead[playerid] = next;
    } else {
        gPropertyData[prev][E_PROPERTY_OWNER_NEXT] = next;
    }

    if (next != INVALID_PROPERTY_ID) {
        gPropertyData[next][E_PROPERTY_OWNER_PREV] = prev;
    }

    gPropertyData[propertyid][E_PROPERTY_OWNER_PLAYER_ID] = INVALID_PLAYER_ID;
    gPropertyData[propertyid][E_PROPERTY_OWNER_NEXT] = INVALID_PROPERTY_ID;
    gPropertyData[propertyid][E_PROPERTY_OWNER_PREV] = INVALID_PROPERTY_ID;

    --gPlayerPropertyCount[playerid];
}

static stock LinkPropertyOwnerInternal(playerid, Property:propertyid) {
    if (gPropertyData[propertyid][E_PROPERTY_OWNER_PLAYER_ID] == playerid) {
        return;
    }

    UnlinkPropertyOwnerInternal(propertyid);

    new const
        Property:head = gPlayerPropertyHead[playerid]
    ;

    gPropertyData[propertyid][E_PROPERTY_OWNER_PLAYER_ID] = playerid;
    gPropertyData[propertyid][E_PROPERTY_OWNER_NEXT] = head;
    gPropertyData[propertyid][E_PROPERTY_OWNER_PREV] = INVALID_PROPERTY_ID;

    if (head != INVALID_PROPERTY_ID) {
        gPropertyData[head][E_PROPERTY_OWNER_PREV] = propertyid;
    }

    gPlayerPropertyHead[playerid] = propertyid;

    ++gPlayerPropertyCount[playerid];
}

static stock SetPropertyPlayerInternal(Property:propertyid, playerid) {
    UnlinkPropertyOwnerInternal(propertyid);

    if (playerid != INVALID_PLAYER_ID) {
        LinkPropertyOwnerInternal(playerid, propertyid);
    }

    RefreshPropertySaleMarkerInternal(propertyid);
}

static stock Property:GetPlayerPendingPropertyInternal(playerid) {
    new const
        Property:pending = gPlayerPendingProperty[playerid]
    ;

    if (pending == INVALID_PROPERTY_ID) {
        return INVALID_PROPERTY_ID;
    }

    if (GetPlayerSalePropertyInternal(playerid) == pending) {
        return pending;
    }

    // Walking off the sign is answering no.
    gPlayerPendingProperty[playerid] = INVALID_PROPERTY_ID;

    return INVALID_PROPERTY_ID;
}

static stock ClearPlayerPendingInternal(playerid) {
    if (gPlayerPendingProperty[playerid] == INVALID_PROPERTY_ID) {
        return;
    }

    new const
        Property:propertyid = gPlayerPendingProperty[playerid]
    ;

    gPlayerPendingProperty[playerid] = INVALID_PROPERTY_ID;

    CallLocalFunction("OnPlayerPropertyPromptChange", "ii", playerid, _:propertyid);
}

static stock ClearPlayerPropertyInternal(playerid) {
    new const
        Property:propertyid = gPlayerProperty[playerid]
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return;
    }

    gPlayerProperty[playerid] = INVALID_PROPERTY_ID;

    if (!IsValidPropertyInternal(propertyid)) {
        return;
    }

    --gPropertyData[propertyid][E_PROPERTY_PLAYER_COUNT];
}

static stock SetPlayerPropertyInternal(playerid, Property:propertyid) {
    ClearPlayerPropertyInternal(playerid);

    gPlayerProperty[playerid] = propertyid;

    ++gPropertyData[propertyid][E_PROPERTY_PLAYER_COUNT];
}

/**
 * # Implementation
 */

stock PropertyType:DefinePropertyType(const name[]) {
    if (gPropertyTypeCount == _:MAX_PROPERTY_TYPES) {
        return INVALID_PROPERTY_TYPE;
    }

    new const
        PropertyType:type = PropertyType:gPropertyTypeCount++
    ;

    strpack(gPropertyTypeData[type][E_PROPERTY_TYPE_NAME], name, MAX_PROPERTY_TYPE_NAME char);
    gPropertyTypeData[type][E_PROPERTY_TYPE_COUNT] = 0;
    gPropertyTypeData[type][E_PROPERTY_TYPE_REGISTERED] = true;

    return type;
}

stock bool:IsValidPropertyType(PropertyType:type) {
    return IsValidPropertyTypeInternal(type);
}

stock GetPropertyTypeCount() {
    return gPropertyTypeCount;
}

stock bool:GetPropertyTypeName(PropertyType:type, output[], size = sizeof (output)) {
    if (!IsValidPropertyTypeInternal(type)) {
        return false;
    }

    strunpack(output, gPropertyTypeData[type][E_PROPERTY_TYPE_NAME], size);

    return true;
}

/**
 * What kind of place this one is, ready to sit in the middle of a sentence
 * rather than at the head of a list: GetPropertyTypeName hands back the name
 * the kind was registered under, and this hands back the same word in lower
 * case, so that a script writing "this house is locked" does not have to know
 * which fragment it is talking about.
 */
/**
 * Builds a property: the door in and out of it, and everything about it that
 * does not depend on what kind of property it is.
 *
 * @param type        A kind handed back by DefinePropertyType.
 * @param name        What the prompt outside the door reads.
 * @param extX..extA  Where the player stands and which way they face outside.
 * @param intX..intA  The same, inside.
 * @param intWorld    PROPERTY_AUTO_WORLD gives this property a world of its own.
 */
stock Property:CreateProperty(PropertyType:type, const name[], Float:extX, Float:extY, Float:extZ, Float:extA, extWorld, extInterior, Float:intX, Float:intY, Float:intZ, Float:intA, intWorld = PROPERTY_AUTO_WORLD, intInterior = 0, KEY:buttonKey = ENTRANCE_DEFAULT_BUTTON_KEY, priority = ENTRANCE_DEFAULT_BUTTON_PRIORITY) {
    // A property nothing claims is a property no fragment will ever be called
    // about, which is a quieter bug than being told the kind does not exist.
    if (!IsValidPropertyTypeInternal(type)) {
        return INVALID_PROPERTY_ID;
    }

    new const
        Property:propertyid = AllocPropertyInternal()
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return INVALID_PROPERTY_ID;
    }

    if (intWorld == PROPERTY_AUTO_WORLD) {
        intWorld = PROPERTY_WORLD_BASE + _:propertyid;
    }

    new const
        Entrance:entranceid = CreateEntrance(
            extX, extY, extZ, extA, extWorld, extInterior,
            intX, intY, intZ, intA, intWorld, intInterior,
            buttonKey, priority
        )
    ;

    if (entranceid == INVALID_ENTRANCE_ID) {
        FreePropertyInternal(propertyid);

        return INVALID_PROPERTY_ID;
    }

    strpack(gPropertyData[propertyid][E_PROPERTY_NAME], name, MAX_PROPERTY_NAME char);
    gPropertyData[propertyid][E_PROPERTY_CREATED]             = true;
    gPropertyData[propertyid][E_PROPERTY_TYPE]                = type;
    gPropertyData[propertyid][E_PROPERTY_OWNED]               = false;
    gPropertyData[propertyid][E_PROPERTY_EXTRA]               = PROPERTY_NO_EXTRA;
    gPropertyData[propertyid][E_PROPERTY_PRICE]               = 0;
    gPropertyData[propertyid][E_PROPERTY_LOCKED]              = false;
    gPropertyData[propertyid][E_PROPERTY_PLAYER_COUNT]        = 0;
    gPropertyData[propertyid][E_PROPERTY_ENTRANCE_ID]         = entranceid;

    // Nothing on the market until a fragment says so, and the marker stands on
    // the doorstep until a script moves it.
    gPropertyData[propertyid][E_PROPERTY_SALE_MODEL_FOR_SALE] = 0;
    gPropertyData[propertyid][E_PROPERTY_SALE_MODEL_OWNED]    = 0;
    gPropertyData[propertyid][E_PROPERTY_SALE_ICON_FOR_SALE]  = 0;
    gPropertyData[propertyid][E_PROPERTY_SALE_ICON_OWNED]     = 0;

    gPropertyData[propertyid][E_PROPERTY_SALE_X]              = extX;
    gPropertyData[propertyid][E_PROPERTY_SALE_Y]              = extY;
    gPropertyData[propertyid][E_PROPERTY_SALE_Z]              = extZ;

    gPropertyData[propertyid][E_PROPERTY_SALE_BUTTON_ID]      = INVALID_BUTTON_ID;

    gPropertyData[propertyid][E_PROPERTY_EXT_PICKUP_ID]       = STREAMER_TAG_PICKUP:INVALID_STREAMER_ID;
    gPropertyData[propertyid][E_PROPERTY_INT_PICKUP_ID]       = STREAMER_TAG_PICKUP:INVALID_STREAMER_ID;
    gPropertyData[propertyid][E_PROPERTY_SALE_PICKUP_ID]      = STREAMER_TAG_PICKUP:INVALID_STREAMER_ID;
    gPropertyData[propertyid][E_PROPERTY_SALE_MAP_ICON_ID]    = STREAMER_TAG_MAP_ICON:INVALID_STREAMER_ID;
    gPropertyData[propertyid][E_PROPERTY_SALE_3DTEXT_ID]      = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;

    // Written out rather than left to the wipe a freed slot was given, because
    // nought is a real player and a real property and this has to mean neither.
    gPropertyData[propertyid][E_PROPERTY_OWNER_PLAYER_ID]     = INVALID_PLAYER_ID;
    gPropertyData[propertyid][E_PROPERTY_OWNER_NEXT]          = INVALID_PROPERTY_ID;
    gPropertyData[propertyid][E_PROPERTY_OWNER_PREV]          = INVALID_PROPERTY_ID;

    gEntranceProperty[entranceid] = propertyid;
    
    ++gPropertyTypeData[type][E_PROPERTY_TYPE_COUNT];

    CreatePropertyEntrancePickupsInternal(propertyid);
    RefreshPropertyPopupInternal(propertyid);

    SetButtonPopupText(GetEntranceInteriorButton(entranceid), PROPERTY_INTERIOR_POPUP, PROPERTY_KEY_TEXT);

    CallLocalFunction("OnPropertyCreate", "i", _:propertyid);

    return propertyid;
}

stock bool:DestroyProperty(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    new const
        Entrance:entranceid = gPropertyData[propertyid][E_PROPERTY_ENTRANCE_ID]
    ;

    // Anybody standing in a room that is about to stop existing goes back out
    // through the door before it goes with it. Walking the players is only
    // worth doing when there is somebody to find, which the count answers
    // without looking at any of them, and the walk is over who is connected
    // rather than over every slot the server has: on a full size server that is
    // twenty players read instead of a thousand.
    if (gPropertyData[propertyid][E_PROPERTY_PLAYER_COUNT]) {
        foreach (new playerid : Player) {
            if (gPlayerProperty[playerid] != propertyid) {
                continue;
            }

            // The door is already gone when this runs for a property that
            // lost its entrance rather than the other way round, and then
            // there is nowhere to put anybody back down. Straightening the
            // bookkeeping is all that is left to do, and it has to happen
            // either way or a slot handed out again reads as having them in it.
            SetPlayerEntranceExterior(playerid, entranceid);
            ClearPlayerPropertyInternal(playerid);
        }
    }

    // Out of whosever chain it is in, before the record it is threaded through
    // stops meaning anything.
    UnlinkPropertyOwnerInternal(propertyid);

    if (IsValidDynamicPickup(gPropertyData[propertyid][E_PROPERTY_EXT_PICKUP_ID])) {
        DestroyDynamicPickup(gPropertyData[propertyid][E_PROPERTY_EXT_PICKUP_ID]);
    }

    if (IsValidDynamicPickup(gPropertyData[propertyid][E_PROPERTY_INT_PICKUP_ID])) {
        DestroyDynamicPickup(gPropertyData[propertyid][E_PROPERTY_INT_PICKUP_ID]);
    }

    DestroyPropertySaleButtonInternal(propertyid);
    DestroyPropertySaleSignInternal(propertyid);

    gEntranceProperty[entranceid] = INVALID_PROPERTY_ID;

    DestroyEntrance(entranceid);

    --gPropertyTypeData[gPropertyData[propertyid][E_PROPERTY_TYPE]][E_PROPERTY_TYPE_COUNT];

    static const
        PROPERTY_DATA_NONE[E_PROPERTY_DATA]
    ;

    gPropertyData[propertyid] = PROPERTY_DATA_NONE;
    FreePropertyInternal(propertyid);

    CallLocalFunction("OnPropertyDestroy", "i", _:propertyid);

    return true;
}

stock bool:IsValidProperty(Property:propertyid) {
    return IsValidPropertyInternal(propertyid);
}

/**
 * How many properties are standing, of one kind or of all of them.
 */
stock GetPropertyCount(PropertyType:type = INVALID_PROPERTY_TYPE) {
    if (type == INVALID_PROPERTY_TYPE) {
        return gPropertyCount;
    }

    if (!IsValidPropertyTypeInternal(type)) {
        return 0;
    }

    return gPropertyTypeData[type][E_PROPERTY_TYPE_COUNT];
}

stock GetPropertyPoolSize() {
    return gPropertyHighWater;
}

/**
 * # Property
 */

stock PropertyType:GetPropertyType(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return INVALID_PROPERTY_TYPE;
    }

    return gPropertyData[propertyid][E_PROPERTY_TYPE];
}

stock bool:SetPropertyName(Property:propertyid, const name[], OPEN_MP_TAGS:...) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    new
        text[MAX_PROPERTY_NAME]
    ;

    if (numargs() == 2) {
        format(text, sizeof (text), "%s", name);
    } else {
        format(text, sizeof (text), name, ___(2));
    }

    strpack(gPropertyData[propertyid][E_PROPERTY_NAME], text, MAX_PROPERTY_NAME char);

    RefreshPropertyPopupInternal(propertyid);
    RefreshPropertySaleMarkerInternal(propertyid);

    return true;
}

stock bool:GetPropertyName(Property:propertyid, output[], size = sizeof (output), bool:kind = false) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    // What it is rather than which one it is: "house" where the name would say
    // "Rockshore West 4". A sentence usually wants one or the other.
    if (kind) {
        GetPropertyKindInternal(propertyid, output, size);

        return true;
    }

    strunpack(output, gPropertyData[propertyid][E_PROPERTY_NAME], size);

    return true;
}

stock bool:SetPropertyOwned(Property:propertyid, bool:owned) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    gPropertyData[propertyid][E_PROPERTY_OWNED] = owned;

    if (!owned) {
        UnlinkPropertyOwnerInternal(propertyid);
    }

    RefreshPropertySaleMarkerInternal(propertyid);

    return true;
}

stock bool:IsPropertyOwned(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    return gPropertyData[propertyid][E_PROPERTY_OWNED];
}

stock bool:SetPropertyExtra(Property:propertyid, extra) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    gPropertyData[propertyid][E_PROPERTY_EXTRA] = extra;

    return true;
}

stock GetPropertyExtra(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return PROPERTY_NO_EXTRA;
    }

    return gPropertyData[propertyid][E_PROPERTY_EXTRA];
}

stock bool:SetPropertyPrice(Property:propertyid, price) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    gPropertyData[propertyid][E_PROPERTY_PRICE] = price;

    // The sign carries the price, so it is one of the things that changes it.
    RefreshPropertySaleMarkerInternal(propertyid);

    return true;
}

stock GetPropertyPrice(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return 0;
    }

    return gPropertyData[propertyid][E_PROPERTY_PRICE];
}

stock bool:SetPropertyLocked(Property:propertyid, bool:locked) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    gPropertyData[propertyid][E_PROPERTY_LOCKED] = locked;

    RefreshPropertyPopupInternal(propertyid);

    return true;
}

stock bool:IsPropertyLocked(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    return gPropertyData[propertyid][E_PROPERTY_LOCKED];
}

stock GetPropertyPlayerCount(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return 0;
    }

    return gPropertyData[propertyid][E_PROPERTY_PLAYER_COUNT];
}

stock bool:SetPropertySaleMarker(Property:propertyid, forSaleModel, ownedModel, forSaleIcon = 0, ownedIcon = 0) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    gPropertyData[propertyid][E_PROPERTY_SALE_MODEL_FOR_SALE] = forSaleModel;
    gPropertyData[propertyid][E_PROPERTY_SALE_MODEL_OWNED] = ownedModel;
    gPropertyData[propertyid][E_PROPERTY_SALE_ICON_FOR_SALE] = forSaleIcon;
    gPropertyData[propertyid][E_PROPERTY_SALE_ICON_OWNED] = ownedIcon;

    RefreshPropertySaleMarkerInternal(propertyid);

    return true;
}

stock bool:SetPropertySaleMarkerPos(Property:propertyid, Float:x, Float:y, Float:z) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    gPropertyData[propertyid][E_PROPERTY_SALE_X] = x;
    gPropertyData[propertyid][E_PROPERTY_SALE_Y] = y;
    gPropertyData[propertyid][E_PROPERTY_SALE_Z] = z;

    DestroyPropertySaleButtonInternal(propertyid);
    RefreshPropertySaleMarkerInternal(propertyid);

    return true;
}

stock bool:GetPropertySaleMarkerPos(Property:propertyid, &Float:x, &Float:y, &Float:z) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    x = gPropertyData[propertyid][E_PROPERTY_SALE_X];
    y = gPropertyData[propertyid][E_PROPERTY_SALE_Y];
    z = gPropertyData[propertyid][E_PROPERTY_SALE_Z];

    return true;
}

stock GetPropertySaleMarkerModel(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return 0;
    }

    return GetPropertySaleModelInternal(propertyid);
}

stock GetPropertySaleMarkerIcon(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return 0;
    }

    return GetPropertySaleIconInternal(propertyid);
}

stock bool:RefreshPropertySaleMarker(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    RefreshPropertySaleMarkerInternal(propertyid);

    return true;
}

stock STREAMER_TAG_PICKUP:GetPropertySaleMarkerPickup(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return STREAMER_TAG_PICKUP:INVALID_STREAMER_ID;
    }

    return gPropertyData[propertyid][E_PROPERTY_SALE_PICKUP_ID];
}

stock STREAMER_TAG_3D_TEXT_LABEL:GetPropertySaleMarkerLabel(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;
    }

    return gPropertyData[propertyid][E_PROPERTY_SALE_3DTEXT_ID];
}

stock STREAMER_TAG_MAP_ICON:GetPropertySaleMarkerMapIcon(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return STREAMER_TAG_MAP_ICON:INVALID_STREAMER_ID;
    }

    return gPropertyData[propertyid][E_PROPERTY_SALE_MAP_ICON_ID];
}

stock Property:GetPlayerSaleProperty(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_PROPERTY_ID;
    }

    return GetPlayerSalePropertyInternal(playerid);
}

stock Button:GetPropertySaleMarkerButton(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return INVALID_BUTTON_ID;
    }

    return gPropertyData[propertyid][E_PROPERTY_SALE_BUTTON_ID];
}

stock STREAMER_TAG_PICKUP:GetPropertyEntrancePickup(Property:propertyid, bool:inside = false) {
    if (!IsValidPropertyInternal(propertyid)) {
        return STREAMER_TAG_PICKUP:INVALID_STREAMER_ID;
    }

    if (inside) {
        return gPropertyData[propertyid][E_PROPERTY_INT_PICKUP_ID];
    }

    return gPropertyData[propertyid][E_PROPERTY_EXT_PICKUP_ID];
}

stock Entrance:GetPropertyEntrance(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return INVALID_ENTRANCE_ID;
    }

    return gPropertyData[propertyid][E_PROPERTY_ENTRANCE_ID];
}

stock Button:GetPropertyExteriorButton(Property:propertyid) {
    return GetEntranceExteriorButton(GetPropertyEntrance(propertyid));
}

stock Button:GetPropertyInteriorButton(Property:propertyid) {
    return GetEntranceInteriorButton(GetPropertyEntrance(propertyid));
}

stock GetPropertyVirtualWorld(Property:propertyid) {
    return GetEntranceInteriorVirtualWorld(GetPropertyEntrance(propertyid));
}

stock Property:GetEntranceProperty(Entrance:entranceid) {
    return GetEntrancePropertyInternal(entranceid);
}

stock Property:GetButtonProperty(Button:buttonid) {
    if (0 <= _:buttonid < _:MAX_BUTTONS) {
        new const
            Property:propertyid = gSaleButtonProperty[buttonid]
        ;

        if (IsValidPropertyInternal(propertyid)) {
            return propertyid;
        }
    }

    new
        Entrance:entranceid = GetButtonEntranceExterior(buttonid)
    ;

    if (entranceid == INVALID_ENTRANCE_ID) {
        entranceid = GetButtonEntranceInterior(buttonid);
    }

    return GetEntrancePropertyInternal(entranceid);
}

/**
 * # Player
 */

stock bool:SetPropertyPlayer(Property:propertyid, playerid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    if (playerid != INVALID_PLAYER_ID && !IsPlayerConnected(playerid)) {
        return false;
    }

    // Handing it to somebody is saying it is theirs, so the deed says so too. A
    // script loading a world in any order then cannot end up with a place held
    // by a player and reading as going spare.
    if (playerid != INVALID_PLAYER_ID) {
        gPropertyData[propertyid][E_PROPERTY_OWNED] = true;
    }

    SetPropertyPlayerInternal(propertyid, playerid);

    return true;
}

stock GetPropertyPlayer(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return INVALID_PLAYER_ID;
    }

    return gPropertyData[propertyid][E_PROPERTY_OWNER_PLAYER_ID];
}

stock bool:IsPlayerPropertyOwner(playerid, Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    return gPropertyData[propertyid][E_PROPERTY_OWNER_PLAYER_ID] == playerid;
}

stock CountPlayerProperties(playerid, PropertyType:type = INVALID_PROPERTY_TYPE) {
    if (!IsPlayerConnected(playerid)) {
        return 0;
    }

    if (type == INVALID_PROPERTY_TYPE) {
        return gPlayerPropertyCount[playerid];
    }

    new
        count
    ;

    for (new Property:propertyid = gPlayerPropertyHead[playerid];
        propertyid != INVALID_PROPERTY_ID;
        propertyid = gPropertyData[propertyid][E_PROPERTY_OWNER_NEXT]
    ) {
        if (gPropertyData[propertyid][E_PROPERTY_TYPE] != type) {
            continue;
        }

        ++count;
    }

    return count;
}

stock GetPlayerProperties(playerid, Property:properties[], size = sizeof (properties), PropertyType:type = INVALID_PROPERTY_TYPE) {
    if (!IsPlayerConnected(playerid)) {
        return 0;
    }

    new
        count
    ;

    for (new Property:propertyid = gPlayerPropertyHead[playerid];
        propertyid != INVALID_PROPERTY_ID && count != size;
        propertyid = gPropertyData[propertyid][E_PROPERTY_OWNER_NEXT]
    ) {
        if (type != INVALID_PROPERTY_TYPE && gPropertyData[propertyid][E_PROPERTY_TYPE] != type) {
            continue;
        }

        properties[count++] = propertyid;
    }

    return count;
}

stock Property:GetPlayerFirstProperty(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_PROPERTY_ID;
    }

    return gPlayerPropertyHead[playerid];
}

stock Property:GetPropertyNextOwner(Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return INVALID_PROPERTY_ID;
    }

    return gPropertyData[propertyid][E_PROPERTY_OWNER_NEXT];
}

stock Property:GetPlayerProperty(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_PROPERTY_ID;
    }

    return gPlayerProperty[playerid];
}

stock bool:IsPlayerInsideProperty(playerid) {
    return GetPlayerProperty(playerid) != INVALID_PROPERTY_ID;
}

stock bool:SetPlayerPropertyInterior(playerid, Property:propertyid, Float:offset = ENTRANCE_DEFAULT_FORWARD_OFFSET) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    return SetPlayerEntranceInterior(playerid, gPropertyData[propertyid][E_PROPERTY_ENTRANCE_ID], offset);
}

stock bool:SetPlayerPropertyExterior(playerid, Property:propertyid, Float:offset = ENTRANCE_DEFAULT_FORWARD_OFFSET) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    return SetPlayerEntranceExterior(playerid, gPropertyData[propertyid][E_PROPERTY_ENTRANCE_ID], offset);
}

stock bool:GetPlayerPropertyPrompt(playerid, output[], size = sizeof (output)) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    new
        price[16]
    ;

    new const
        Property:pending = GetPlayerPendingPropertyInternal(playerid)
    ;

    if (IsValidPropertyInternal(pending)) {
        HumanizeThousand(gPropertyData[pending][E_PROPERTY_PRICE], price);

        format(output, size, PROPERTY_CONFIRM_POPUP,
            PROPERTY_CONFIRM_KEY_TEXT,
            price
        );

        return true;
    }

    new const
        Property:propertyid = GetPlayerSalePropertyInternal(playerid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return false;
    }

    if (gPropertyData[propertyid][E_PROPERTY_OWNED]) {
        return false;
    }

    new
        kind[MAX_PROPERTY_TYPE_NAME]
    ;

    GetPropertyKindInternal(propertyid, kind);
    HumanizeThousand(gPropertyData[propertyid][E_PROPERTY_PRICE], price);

    format(output, size, PROPERTY_BUY_POPUP,
        PROPERTY_BUY_KEY_TEXT,
        kind,
        price
    );

    return true;
}

stock Property:GetPlayerPendingProperty(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_PROPERTY_ID;
    }

    return GetPlayerPendingPropertyInternal(playerid);
}

stock bool:CancelPlayerPropertyPurchase(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (gPlayerPendingProperty[playerid] == INVALID_PROPERTY_ID) {
        return false;
    }

    ClearPlayerPendingInternal(playerid);

    return true;
}

stock bool:BuyProperty(playerid, Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (gPropertyData[propertyid][E_PROPERTY_OWNED]) {
        return false;
    }

    if (gPropertyData[propertyid][E_PROPERTY_OWNER_PLAYER_ID] == playerid) {
        return false;
    }

    // What the deed said before it changed hands. It is read out now because
    // afterwards it is gone, and it is the only thing the script has to go on
    // when the seller is not on the server to be paid.
    new const
        previousExtra = gPropertyData[propertyid][E_PROPERTY_EXTRA],
        previousPlayer = gPropertyData[propertyid][E_PROPERTY_OWNER_PLAYER_ID],
        price = gPropertyData[propertyid][E_PROPERTY_PRICE]
    ;

    if (CallLocalFunction("OnPlayerBuyProperty", "iii", playerid, _:propertyid, price)) {
        return false;
    }

    gPropertyData[propertyid][E_PROPERTY_OWNED] = true;
    gPropertyData[propertyid][E_PROPERTY_EXTRA] = PROPERTY_NO_EXTRA;

    SetPropertyPlayerInternal(propertyid, playerid);

    CallLocalFunction("OnPlayerBoughtProperty", "iiiii", playerid, _:propertyid, price, previousPlayer, previousExtra);

    return true;
}

stock bool:SellProperty(playerid, Property:propertyid) {
    if (!IsValidPropertyInternal(propertyid)) {
        return false;
    }

    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (gPropertyData[propertyid][E_PROPERTY_OWNER_PLAYER_ID] != playerid) {
        return false;
    }

    new const
        price = gPropertyData[propertyid][E_PROPERTY_PRICE]
    ;

    if (CallLocalFunction("OnPlayerSellProperty", "iii", playerid, _:propertyid, price)) {
        return false;
    }

    gPropertyData[propertyid][E_PROPERTY_OWNED] = false;
    gPropertyData[propertyid][E_PROPERTY_EXTRA] = PROPERTY_NO_EXTRA;

    // Back on the market rather than left as a room nobody can reach, and
    // opened on the way: a lock with no owner behind it admits nobody at all,
    // so leaving it shut would shut it for good. A script that wants either of
    // those the other way round says so in OnPlayerSoldProperty.
    //
    // Said before the deed is let go rather than after, so that the marker is
    // built once, against the state it is going to keep. Refreshing it twice
    // would take the sale area down and put it back under whoever is standing
    // on it, and an area built under a player is not one they are inside.
    gPropertyData[propertyid][E_PROPERTY_LOCKED] = false;

    UnlinkPropertyOwnerInternal(propertyid);

    RefreshPropertySaleMarkerInternal(propertyid);
    RefreshPropertyPopupInternal(propertyid);

    CallLocalFunction("OnPlayerSoldProperty", "iii", playerid, _:propertyid, price);

    return true;
}

/**
 * # Calls
 */

hook OnPlayerConnect(playerid) {
    gPlayerPropertyHead[playerid] = INVALID_PROPERTY_ID;
    gPlayerPropertyCount[playerid] = 0;

    gPlayerProperty[playerid] = INVALID_PROPERTY_ID;
    gPlayerPendingProperty[playerid] = INVALID_PROPERTY_ID;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    // The deeds stay where they are, and stay owned; what goes is this player
    // holding them, so that the slot they leave behind does not read as owning
    // anything and the places do not read as going spare.
    while (gPlayerPropertyHead[playerid] != INVALID_PROPERTY_ID) {
        UnlinkPropertyOwnerInternal(gPlayerPropertyHead[playerid]);
    }

    ClearPlayerPropertyInternal(playerid);

    gPlayerPendingProperty[playerid] = INVALID_PROPERTY_ID;

    return 0;
}

hook OnPlayerEnterEntrance(playerid, Entrance:entranceid) {
    new const
        Property:propertyid = GetEntrancePropertyInternal(entranceid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return 0;
    }

    if (gPropertyData[propertyid][E_PROPERTY_LOCKED] && gPropertyData[propertyid][E_PROPERTY_OWNER_PLAYER_ID] != playerid) {
        CallLocalFunction("OnPlayerPropertyLocked", "ii", playerid, _:propertyid);

        return 1;
    }

    return CallLocalFunction("OnPlayerEnterProperty", "ii", playerid, _:propertyid) ? 1 : 0;
}

hook OnPlayerLeaveEntrance(playerid, Entrance:entranceid) {
    new const
        Property:propertyid = GetEntrancePropertyInternal(entranceid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return 0;
    }

    return CallLocalFunction("OnPlayerLeaveProperty", "ii", playerid, _:propertyid) ? 1 : 0;
}

hook OnPlayerEnteredEntrance(playerid, Entrance:entranceid) {
    new const
        Property:propertyid = GetEntrancePropertyInternal(entranceid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return 0;
    }

    SetPlayerPropertyInternal(playerid, propertyid);

    CallLocalFunction("OnPlayerEnteredProperty", "ii", playerid, _:propertyid);

    return 0;
}

hook OnPlayerLeftEntrance(playerid, Entrance:entranceid) {
    new const
        Property:propertyid = GetEntrancePropertyInternal(entranceid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return 0;
    }

    ClearPlayerPropertyInternal(playerid);

    CallLocalFunction("OnPlayerLeftProperty", "ii", playerid, _:propertyid);

    return 0;
}

hook OnPlayerEnterExteriorArea(playerid, Entrance:entranceid) {
    new const
        Property:propertyid = GetEntrancePropertyInternal(entranceid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return 0;
    }

    return CallLocalFunction("OnPlayerEnterPropertyArea", "iii", playerid, _:propertyid, false) ? 1 : 0;
}

hook OnPlayerEnterInteriorArea(playerid, Entrance:entranceid) {
    new const
        Property:propertyid = GetEntrancePropertyInternal(entranceid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return 0;
    }

    return CallLocalFunction("OnPlayerEnterPropertyArea", "iii", playerid, _:propertyid, true) ? 1 : 0;
}

hook OnPlayerLeaveExteriorArea(playerid, Entrance:entranceid) {
    new const
        Property:propertyid = GetEntrancePropertyInternal(entranceid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return 0;
    }

    return CallLocalFunction("OnPlayerLeavePropertyArea", "iii", playerid, _:propertyid, false) ? 1 : 0;
}

hook OnPlayerLeaveInteriorArea(playerid, Entrance:entranceid) {
    new const
        Property:propertyid = GetEntrancePropertyInternal(entranceid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return 0;
    }

    return CallLocalFunction("OnPlayerLeavePropertyArea", "iii", playerid, _:propertyid, true) ? 1 : 0;
}

// Buying is watched here rather than through the button, because a button
// answers one set of keys and the door already spends that set on going inside.
// Everything this reads is an array away, so a key press that means nothing here
// costs two comparisons.
hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    new const
        Property:pending = GetPlayerPendingPropertyInternal(playerid)
    ;

    // Consumed rather than passed on: the same key stows what is in the hands,
    // and answering a question should not also do that.
    if (pending != INVALID_PROPERTY_ID) {
        if ((newkeys & PROPERTY_CONFIRM_KEY) && !(oldkeys & PROPERTY_CONFIRM_KEY)) {
            gPlayerPendingProperty[playerid] = INVALID_PROPERTY_ID;

            // Whether it goes through or is turned down, what they were reading
            // is no longer what they should be reading.
            BuyProperty(playerid, pending);

            CallLocalFunction("OnPlayerPropertyPromptChange", "ii", playerid, _:pending);

            return 1;
        }
    }

    if (!(newkeys & PROPERTY_BUY_KEY) || (oldkeys & PROPERTY_BUY_KEY)) {
        return 0;
    }

    new const
        Property:propertyid = GetPlayerSalePropertyInternal(playerid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return 0;
    }

    if (gPropertyData[propertyid][E_PROPERTY_OWNED]) {
        return 0;
    }

    if (pending == propertyid) {
        return 0;
    }

    gPlayerPendingProperty[playerid] = propertyid;

    CallLocalFunction("OnPlayerPropertyPromptChange", "ii", playerid, _:propertyid);

    return 0;
}

// A button slot handed out again is one a sale marker may have stood on before.
hook OnButtonCreate(Button:buttonid) {
    gSaleButtonProperty[buttonid] = INVALID_PROPERTY_ID;

    return 0;
}

hook OnButtonDestroy(Button:buttonid) {
    gSaleButtonProperty[buttonid] = INVALID_PROPERTY_ID;

    return 0;
}

// A slot handed out again starts clean, whatever was built on it before.
hook OnEntranceCreate(Entrance:entranceid) {
    gEntranceProperty[entranceid] = INVALID_PROPERTY_ID;

    return 0;
}

// A door taken down by hand rather than through DestroyProperty would leave a
// property standing with no way into or out of it, which is worse than not
// having it at all.
hook OnEntranceDestroy(Entrance:entranceid) {
    new const
        Property:propertyid = GetEntrancePropertyInternal(entranceid)
    ;

    gEntranceProperty[entranceid] = INVALID_PROPERTY_ID;

    if (propertyid != INVALID_PROPERTY_ID) {
        DestroyProperty(propertyid);
    }

    return 0;
}
