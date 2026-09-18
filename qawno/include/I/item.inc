#if defined _INC_ITEM
    #endinput
#endif
#define _INC_ITEM

#include <open.mp>

#include <I\button>
#include <I\PP\button-plus>

#include <Randomix>
#include <mathutil>
#include <pp-hooks>

/**
 * # Header
 */

#if !defined MAX_ITEM_BUILDS
    #define MAX_ITEM_BUILDS (ItemBuild:128)
#endif

#if !defined MAX_ITEMS
    #define MAX_ITEMS (Item:8192)
#endif

#if !defined MAX_ITEM_BUILD_NAME
    #define MAX_ITEM_BUILD_NAME (32)
#endif

#if !defined MAX_ITEM_EXTRA_NAME
    #define MAX_ITEM_EXTRA_NAME (32)
#endif

// The size of a buffer that GetItemName writes into: the build's name, the
// item's own, and the " (" ")" between them. This is a caller's buffer only --
// what an item stores is its own name alone, so sizing the stored field with
// this would carry a copy of the build name per item that nothing ever writes.
#if !defined MAX_ITEM_NAME
    #define MAX_ITEM_NAME (MAX_ITEM_BUILD_NAME + MAX_ITEM_EXTRA_NAME + 3)
#endif

#if !defined ITEM_KEY_PICK_UP_ITEM
    #define ITEM_KEY_PICK_UP_ITEM (KEY_SECONDARY_ATTACK)
#endif

#if !defined ITEM_KEY_DROP_ITEM
    #define ITEM_KEY_DROP_ITEM (KEY_NO)
#endif

#if !defined ITEM_KEY_USE_ITEM
    #define ITEM_KEY_USE_ITEM (KEY_SECONDARY_ATTACK)
#endif

#if !defined ITEM_DEFAULT_DRAW_DISTANCE
    #define ITEM_DEFAULT_DRAW_DISTANCE (100.0)
#endif

#if !defined ITEM_DEFAULT_STREAM_DISTANCE
    #define ITEM_DEFAULT_STREAM_DISTANCE (100.0)
#endif

#if !defined ITEM_DEFAULT_BUTTON_PRIORITY
    #define ITEM_DEFAULT_BUTTON_PRIORITY (1)
#endif

// Thirty six characters of RFC 4122 and the terminator: the size of a buffer
// GetItemUUID writes into, not the size of what an item holds.
#if !defined MAX_ITEM_UUID_LENGTH
    #define MAX_ITEM_UUID_LENGTH (37)
#endif

// What an item holds is the 128 bits a UUID actually is. Keeping the printed
// form instead would spend 37 cells an item to carry 4 cells of information,
// which at 65,000 items is 9.6 MB against 1 MB. The text is produced when
// something asks for it, which is when the item is saved or shown, never in a
// loop.
#define ITEM_UUID_CELLS (4)

#define INVALID_ITEM_BUILD_ID (ItemBuild:-1)
#define INVALID_ITEM_ID (Item:-1)

#define ITEM_ATTACH_INDEX (0)
#define ITEM_FLOOR_OFFSET (0.96)

// How much room one of these takes up, for whatever wants to know whether it
// fits. The framework stores it and answers it and has no opinion about it: a
// container asking whether a rifle goes in a rucksack is a gamemode's question,
// and this is only where the number lives so that everything asks the same one.
//
// A number rather than a few named steps, because the spread is wide and
// uneven. A build that never says is ITEM_BUILD_DEFAULT_SIZE, so a catalogue
// writes a size only for what is awkward to carry and stays quiet about pills.
#if !defined ITEM_BUILD_DEFAULT_SIZE
    #define ITEM_BUILD_DEFAULT_SIZE (1)
#endif

// Under the reserved prefix, so a gamemode cannot take this name and break the
// link between a button and the item it picks up. Short because it is hashed on
// every lookup, and there are a lot of those.
static const
    BUTTON_KEY_ITEM[] = "@item"
;

static enum _:E_ITEM_BUILD_DATA {
    E_ITEM_BUILD_NAME[MAX_ITEM_BUILD_NAME],
    E_ITEM_BUILD_MODEL_ID,
    E_ITEM_BUILD_SIZE,

    // TODO:
    Float:E_ITEM_BUILD_OBJ_ROT_OFFSET_X,
    Float:E_ITEM_BUILD_OBJ_ROT_OFFSET_Y,
    Float:E_ITEM_BUILD_OBJ_ROT_OFFSET_Z,
    Float:E_ITEM_BUILD_OBJ_OFFSET_Z,

    // Attach
    Float:E_ITEM_BUILD_ATTACH_X,
    Float:E_ITEM_BUILD_ATTACH_Y,
    Float:E_ITEM_BUILD_ATTACH_Z,
    Float:E_ITEM_BUILD_ATTACH_ROT_X,
    Float:E_ITEM_BUILD_ATTACH_ROT_Y,
    Float:E_ITEM_BUILD_ATTACH_ROT_Z,
    Float:E_ITEM_BUILD_ATTACH_SCALE_X,
    Float:E_ITEM_BUILD_ATTACH_SCALE_Y,
    Float:E_ITEM_BUILD_ATTACH_SCALE_Z,
    E_ITEM_BUILD_MATERIAL_COLOUR_1,
    E_ITEM_BUILD_MATERIAL_COLOUR_2,
    bool:E_ITEM_BUILD_USE_CARRY_ANIM,
    E_ITEM_BUILD_ATTACH_BONE_ID,
    Float:E_ITEM_BUILD_BTN_OFFSET_Z
};

static
    gItemBuildData[MAX_ITEM_BUILDS][E_ITEM_BUILD_DATA],
    ItemBuild:gItemBuildCount
;

static enum _:E_ITEM_DATA {
    bool:E_ITEM_CREATED,
    bool:E_ITEM_IN_WORLD,

    ItemBuild:E_ITEM_BUILD_ID,
    E_ITEM_EXTRA_ID,

    // World
    Float:E_ITEM_X,
    Float:E_ITEM_Y,
    Float:E_ITEM_Z,

    Float:E_ITEM_ROT_X,
    Float:E_ITEM_ROT_Y,
    Float:E_ITEM_ROT_Z,

    E_ITEM_WORLD_ID,
    E_ITEM_INTERIOR_ID,

    Button:E_ITEM_BUTTON_ID,
    STREAMER_TAG_OBJECT:E_ITEM_OBJECT_ID,

    // Extra
    E_ITEM_UUID[ITEM_UUID_CELLS],
    E_ITEM_EXTRA_NAME[MAX_ITEM_EXTRA_NAME]
};

// The same free list button.inc uses, for the same reason: taking an id and
// giving it back are each a read and a write, and a full array is answered
// without looking at anything. Items are never walked as a set -- every lookup
// here is by id -- so nothing is lost by not keeping one.
static
    gItemNextFree[_:MAX_ITEMS],
    gItemFreeHead = -1,
    gItemHighWater,
    gItemCount,
    gItemData[MAX_ITEMS][E_ITEM_DATA]
;

static
    Item:gInteractItemID[MAX_PLAYERS] = { INVALID_ITEM_ID, ... },
    Item:gHoldItemID[MAX_PLAYERS]     = { INVALID_ITEM_ID, ... },
    gItemInteractorID[MAX_ITEMS]      = { INVALID_PLAYER_ID, ... },
    gItemHolderID[MAX_ITEMS]          = { INVALID_PLAYER_ID, ... },
    gInteractTimer[MAX_PLAYERS]       = { INVALID_TIMER, ... }
;

/**
 * # Functions
 */

forward ItemBuild:BuildItem(const name[], modelid, size = ITEM_BUILD_DEFAULT_SIZE, Float:objRotOffsetX = 0.0, Float:objRotOffsetY = 0.0, Float:objRotOffsetZ = 0.0, Float:objOffsetZ = 0.0, Float:attachX = 0.0, Float:attachY = 0.0, Float:attachZ = 0.0, Float:attachRotX = 0.0, Float:attachRotY = 0.0, Float:attachRotZ = 0.0, Float:attachScaleX = 1.0, Float:attachScaleY = 1.0, Float:attachScaleZ = 1.0, materialColour1 = 0, materialColour2 = 0, bool:useCarryAnim = false, bone = 6, Float:buttonOffsetZ = ITEM_FLOOR_OFFSET);
forward bool:IsValidItemBuild(ItemBuild:buildid);
forward bool:GetItemBuildName(ItemBuild:buildid, output[], size = sizeof (output));
forward GetItemBuildModel(ItemBuild:buildid);

/**
 * @brief      Say how much room a kind of item takes up.
 *
 * Not a weight and not a capacity: it is only a number to compare against
 * another number. What it means is entirely the caller's -- the framework
 * neither reads it nor lets it stop anything.
 *
 * It is a call of its own rather than another argument to BuildItem, which
 * already takes seventeen. A build says what it is in one line and is then told
 * what it does, one line per thing.
 *
 * @param      buildid  Build to size.
 * @param      size     How big it is, one and up.
 *
 * @date       12:40 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the size was written
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + `size` is below ITEM_BUILD_DEFAULT_SIZE
 */
forward bool:SetItemBuildSize(ItemBuild:buildid, size);

/**
 * @brief      How much room a kind of item takes up.
 *
 * @param      buildid  Build to read.
 *
 * @date       12:40 08/09/2026
 * @author     augustocaixeta
 *
 * @return     the size, `ITEM_BUILD_DEFAULT_SIZE` for a build that never said
 */
forward GetItemBuildSize(ItemBuild:buildid);
forward bool:SetItemBuildAttach(ItemBuild:buildid, Float:offsetX, Float:offsetY, Float:offsetZ, Float:rotationX, Float:rotationY, Float:rotationZ, bone = 6, Float:scaleX = 1.0, Float:scaleY = 1.0, Float:scaleZ = 1.0);
forward bool:GetItemBuildAttach(ItemBuild:buildid, &Float:offsetX, &Float:offsetY, &Float:offsetZ, &Float:rotationX, &Float:rotationY, &Float:rotationZ, &bone = 0, &Float:scaleX = 0.0, &Float:scaleY = 0.0, &Float:scaleZ = 0.0);
forward CountItemBuild();

forward Item:CreateItem(ItemBuild:buildid, const uuid[] = "", extraid = -1, const extraName[] = "");
forward bool:DestroyItem(Item:itemid);
forward bool:IsValidItem(Item:itemid);
forward GetItemCount();
forward ItemBuild:GetItemBuild(Item:itemid);
forward bool:SetItemExtraID(Item:itemid, extraid);
forward GetItemExtraID(Item:itemid);
forward bool:SetItemExtraName(Item:itemid, const name[], OPEN_MP_TAGS:...);
forward bool:GetItemExtraName(Item:itemid, output[], size = sizeof (output));
forward bool:GetItemUUID(Item:itemid, output[], size = sizeof (output));
forward bool:GetItemName(Item:itemid, output[], size = sizeof (output));

forward bool:CreateItemInWorld(Item:itemid, Float:x, Float:y, Float:z, Float:rotationX, Float:rotationY, Float:rotationZ, KEY:buttonKey = ITEM_KEY_PICK_UP_ITEM, worldid = -1, interiorid = -1, Float:drawDistance = ITEM_DEFAULT_DRAW_DISTANCE, Float:streamDistance = ITEM_DEFAULT_STREAM_DISTANCE, bool:useRotOffsets = true, priority = ITEM_DEFAULT_BUTTON_PRIORITY);
forward bool:IsItemInWorld(Item:itemid);
forward bool:RemoveItemFromWorld(Item:itemid);
forward bool:SetItemPos(Item:itemid, Float:x, Float:y, Float:z);
forward bool:GetItemPos(Item:itemid, &Float:x, &Float:y, &Float:z);
forward bool:SetItemRot(Item:itemid, Float:rotationX, Float:rotationY, Float:rotationZ, bool:useRotOffsets = true);
forward bool:GetItemRot(Item:itemid, &Float:rotationX, &Float:rotationY, &Float:rotationZ);
forward bool:SetItemVirtualWorld(Item:itemid, worldid);
forward GetItemVirtualWorld(Item:itemid);
forward bool:SetItemInterior(Item:itemid, interiorid);
forward GetItemInterior(Item:itemid);
forward bool:SetItemLabelText(Item:itemid, const format[], colour = 0xFFFFFFFF, Float:drawDistance = ITEM_DEFAULT_DRAW_DISTANCE, Float:streamDistance = ITEM_DEFAULT_STREAM_DISTANCE, bool:testLOS = true, Float:offsetZ = 0.5, OPEN_MP_TAGS:...);
forward Button:GetItemButton(Item:itemid);
forward bool:HasButtonItem(Button:buttonid);
forward Item:GetButtonItem(Button:buttonid);
forward Item:GetPlayerItem(playerid);
forward bool:GetPlayerNearbyItems(playerid, Item:items[MAX_AREA_BUTTONS] = { INVALID_ITEM_ID, ... }, &count = 0);

forward bool:GiveWorldItemToPlayer(playerid, Item:itemid);
forward bool:SetPlayerItem(playerid, Item:itemid);
forward bool:PlayerPickUpItem(playerid, Item:itemid);
forward bool:PlayerDropItem(playerid);
forward bool:PlayerUseItem(playerid);
forward bool:EditItemAttach(playerid);
forward bool:RemoveCurrentItem(playerid);
forward GetItemHolder(Item:itemid);
forward GetItemInteractor(Item:itemid);
forward Item:GetPlayerHoldItem(playerid);
forward Item:GetPlayerInteractItem(playerid);

/**
 * # Events
 */

forward OnItemCreate(Item:itemid);
forward OnItemDestroy(Item:itemid);
forward OnItemCreateInWorld(Item:itemid);
forward OnItemRemoveFromWorld(Item:itemid);
forward OnPlayerEnterItemArea(playerid, Item:itemid);
forward OnPlayerLeaveItemArea(playerid, Item:itemid);
forward OnPlayerPickUpItem(playerid, Item:itemid);
forward OnPlayerPickUpItemWithItem(playerid, Item:itemid1, Item:itemid2);
forward OnPlayerPickedUpItem(playerid, Item:itemid);
forward OnPlayerDropItem(playerid, Item:itemid);
forward OnPlayerDroppedItem(playerid, Item:itemid);
forward OnPlayerGetItem(playerid, Item:itemid);
forward OnPlayerGotItem(playerid, Item:itemid);
forward OnPlayerUseItem(playerid, Item:itemid);
forward OnPlayerEditItemAttach(playerid, ItemBuild:buildid, Float:offsetX, Float:offsetY, Float:offsetZ, Float:rotationX, Float:rotationY, Float:rotationZ, bone, Float:scaleX, Float:scaleY, Float:scaleZ);

/**
 * # Internal
 */

forward @Item_PickUpItemDelay(playerid, bool:animation);
forward @Item_DropItemDelay(playerid);

// UUID generation via Randomix (ChaCha20 CSPRNG). The include is pulled in
// after button.inc, which brings in YSI first -- that order keeps YSI's
// RandomFloat macro in place before Randomix's stock of the same name is seen.
// The UUID arrives as a 36-character string and is parsed into the compact
// 4-cell binary form the item carries, keeping the memory layout unchanged.
// Written a nibble at a time rather than through format, whose %x is upper
// case: RFC 4122 reads case insensitively but prints in lower case, and a
// column holding both spellings of the same id is a comparison waiting to go
// wrong somewhere the collation is not what was assumed.
static stock FormatItemUUIDInternal(const uuid[], output[], size) {
    static const
        HEX[] = "0123456789abcdef"
    ;

    if (size < MAX_ITEM_UUID_LENGTH) {
        output[0] = EOS;

        return;
    }

    new
        pos = 0
    ;

    for (new i; i != ITEM_UUID_CELLS * 8; ++i) {
        // 8-4-4-4-12, so a dash goes in front of these four.
        if (i == 8 || i == 12 || i == 16 || i == 20) {
            output[pos++] = '-';
        }

        output[pos++] = HEX[(uuid[i >>> 3] >>> ((7 - (i & 7)) * 4)) & 0xF];
    }

    output[pos] = EOS;
}

// Reads the printed form back, so a UUID that came from the database lands on
// the same 128 bits it left as. Anything that is not a hex digit is skipped,
// which lets the dashes through without a position table.
static stock bool:ParseItemUUIDInternal(const text[], uuid[]) {
    new
        digits = 0
    ;

    uuid[0] = 0;
    uuid[1] = 0;
    uuid[2] = 0;
    uuid[3] = 0;

    for (new i; text[i] != EOS; ++i) {
        new const
            ch = text[i]
        ;

        new
            value
        ;

        if ('0' <= ch <= '9') {
            value = ch - '0';
        } else if ('a' <= ch <= 'f') {
            value = ch - ('a' - 10);
        } else if ('A' <= ch <= 'F') {
            value = ch - ('A' - 10);
        } else {
            continue;
        }

        if (digits == ITEM_UUID_CELLS * 8) {
            return false;
        }

        uuid[digits >>> 3] = (uuid[digits >>> 3] << 4) | value;

        ++digits;
    }

    return digits == ITEM_UUID_CELLS * 8;
}

static stock GenerateItemUUIDInternal(uuid[]) {
    new text[MAX_ITEM_UUID_LENGTH];
    RandUUID(text);
    ParseItemUUIDInternal(text, uuid);
}

static stock GetFreeItemIndexInternal() {
    if (gItemFreeHead != -1) {
        new const
            index = gItemFreeHead
        ;

        gItemFreeHead = gItemNextFree[index];

        return index;
    }

    if (gItemHighWater != _:MAX_ITEMS) {
        return gItemHighWater++;
    }

    return -1;
}

static stock Item:AllocItemInternal() {
    new const
        index = GetFreeItemIndexInternal()
    ;

    if (index == -1) {
        return INVALID_ITEM_ID;
    }

    ++gItemCount;

    return Item:index;
}

static stock FreeItemInternal(Item:itemid) {
    gItemNextFree[_:itemid] = gItemFreeHead;
    gItemFreeHead = _:itemid;

    --gItemCount;
}

static stock bool:IsValidItemInternal(Item:itemid) {
    if (!(0 <= _:itemid < _:MAX_ITEMS)) {
        return false;
    }

    return gItemData[itemid][E_ITEM_CREATED];
}

public @Item_PickUpItemDelay(playerid, bool:animation) {
    if (animation) {
        ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_2IDLE", 4.0, false, false, false, false, 0);
    }

    new const
        Item:itemid = gInteractItemID[playerid]
    ;

    gInteractItemID[playerid] = INVALID_ITEM_ID;
    gItemInteractorID[itemid] = INVALID_PLAYER_ID;

    if (CallLocalFunction("OnPlayerPickedUpItem", "ii", playerid, _:itemid)) {
        return;
    }

    GiveWorldItemToPlayer(playerid, itemid);
}

public @Item_DropItemDelay(playerid) {
    if (!IsValidItem(gHoldItemID[playerid])) {
        return;
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:a
    ;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);

    new const
        Item:itemid = gHoldItemID[playerid]
    ;

    CreateItemInWorld(
        itemid,
        (x + (0.85 * floatsin(-a, degrees))),
        (y + (0.85 * floatcos(-a, degrees))),
        (z - ITEM_FLOOR_OFFSET),
        0.0,
        0.0,
        a,
        _,
        GetPlayerVirtualWorld(playerid),
        GetPlayerInterior(playerid)
    );

    RemoveCurrentItem(playerid);
    ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_2IDLE", 4.0, false, false, false, false, 0);

    Streamer_Update(playerid);

    CallLocalFunction("OnPlayerDroppedItem", "ii", playerid, _:itemid);
}

static stock void:SetPlayerItemInternal(playerid, Item:itemid) {
    RemoveItemFromWorld(itemid);
    RemoveCurrentItem(GetItemHolder(itemid));

    if (CallLocalFunction("OnPlayerGetItem", "ii", playerid, _:itemid)) {
        return;
    }

    new const
        ItemBuild:buildid = gItemData[itemid][E_ITEM_BUILD_ID]
    ;

    gItemData[itemid][E_ITEM_X] = 0.0;
    gItemData[itemid][E_ITEM_Y] = 0.0;
    gItemData[itemid][E_ITEM_Z] = 0.0;

    // Empties the hand this is about to fill, which is not what the call at the
    // top of the function did: that one takes the item away from whoever was
    // holding it, this one lets go of whatever this player was holding. Without
    // it gHoldItemID moves on while the item left behind still names this player
    // as its holder, and nothing can find it again. It happens after
    // OnPlayerGetItem so that a refused get leaves the hand as it was.
    RemoveCurrentItem(playerid);

    gHoldItemID[playerid] = itemid;
    gItemHolderID[itemid] = playerid;
    gInteractItemID[playerid] = INVALID_ITEM_ID;
    gItemInteractorID[itemid] = INVALID_PLAYER_ID;

    ApplyItemAttachInternal(playerid, buildid);

    if (gItemBuildData[buildid][E_ITEM_BUILD_USE_CARRY_ANIM]) {
        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_CARRY);
    }

    CallLocalFunction("OnPlayerGotItem", "ii", playerid, _:itemid);
}

// The build is the single source of truth for where a held object sits, so the
// only way it reaches the client is through here. The in-game editor writes
// straight into the client's copy, which is why an edit that ends -- saved or
// cancelled -- comes back through this to put the two back in step.
static stock void:ApplyItemAttachInternal(playerid, ItemBuild:buildid) {
    SetPlayerAttachedObject(playerid,
        ITEM_ATTACH_INDEX,
        gItemBuildData[buildid][E_ITEM_BUILD_MODEL_ID],
        gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_BONE_ID],
        gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_X],
        gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_Y],
        gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_Z],
        gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_ROT_X],
        gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_ROT_Y],
        gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_ROT_Z],
        gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_SCALE_X],
        gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_SCALE_Y],
        gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_SCALE_Z],
        gItemBuildData[buildid][E_ITEM_BUILD_MATERIAL_COLOUR_1],
        gItemBuildData[buildid][E_ITEM_BUILD_MATERIAL_COLOUR_2]
    );
}

/**
 * Whether the character is standing there doing nothing, which is the only
 * moment it is fair to start something that takes one. Indexes rather than
 * names because that is what GetPlayerAnimationIndex answers with.
 *
 * There is a stance per build of ped, not one for everybody: a female skin
 * stands in WOMAN_IDLESTANCE and never once plays IDLE_STANCE, an old ped has
 * its own, and CJ has two more for how heavy he is. Listing only the ordinary
 * male ones leaves every other skin permanently busy.
 *
 *      0  nothing playing              320  CHAINSAW / IDLE_CSAW
 *    454  FAT / FATIDLE                904  MUSCULAR / MUSCLEIDLE
 *    455  FAT / FATIDLE_ARMED          905  MUSCULAR / MUSCLEIDLE_ARMED
 *    456  FAT / FATIDLE_CSAW           906  MUSCULAR / MUSCLEIDLE_CSAW
 *    457  FAT / FATIDLE_ROCKET         907  MUSCULAR / MUSCLEIDLE_ROCKET
 *    471  FAT / IDLE_TIRED            1164  PED / GUN_2_IDLE
 *   1181  PED / IDLESTANCE_FAT        1183  PED / IDLE_ARMED
 *   1182  PED / IDLESTANCE_OLD        1185  PED / IDLE_CSAW
 *   1189  PED / IDLE_STANCE           1188  PED / IDLE_ROCKET
 *   1275  PED / WOMAN_IDLESTANCE      1191  PED / IDLE_TIRED
 */
static stock bool:IsIdleAnimationInternal(index) {
    switch (index) {
        case 0, 320, 454, 455, 456, 457, 471, 904, 905, 906, 907, 1164, 1181, 1182, 1183, 1185, 1188, 1189, 1191, 1275: {
            return true;
        }
    }

    return false;
}

/**
 * # Item Build
 */

stock ItemBuild:BuildItem(const name[], modelid, size = ITEM_BUILD_DEFAULT_SIZE, Float:objRotOffsetX = 0.0, Float:objRotOffsetY = 0.0, Float:objRotOffsetZ = 0.0, Float:objOffsetZ = 0.0, Float:attachX = 0.0, Float:attachY = 0.0, Float:attachZ = 0.0, Float:attachRotX = 0.0, Float:attachRotY = 0.0, Float:attachRotZ = 0.0, Float:attachScaleX = 1.0, Float:attachScaleY = 1.0, Float:attachScaleZ = 1.0, materialColour1 = 0, materialColour2 = 0, bool:useCarryAnim = false, bone = 6, Float:buttonOffsetZ = ITEM_FLOOR_OFFSET) {
    if (gItemBuildCount >= MAX_ITEM_BUILDS) {
        return INVALID_ITEM_BUILD_ID;
    }
   
    new const
        ItemBuild:buildid = gItemBuildCount++
    ;

    strcopy(gItemBuildData[buildid][E_ITEM_BUILD_NAME], name);
    gItemBuildData[buildid][E_ITEM_BUILD_MODEL_ID] = modelid;
    gItemBuildData[buildid][E_ITEM_BUILD_SIZE] = size;

    // World
    gItemBuildData[buildid][E_ITEM_BUILD_OBJ_ROT_OFFSET_X]  = objRotOffsetX;
    gItemBuildData[buildid][E_ITEM_BUILD_OBJ_ROT_OFFSET_Y]  = objRotOffsetY;
    gItemBuildData[buildid][E_ITEM_BUILD_OBJ_ROT_OFFSET_Z]  = objRotOffsetZ;
    gItemBuildData[buildid][E_ITEM_BUILD_OBJ_OFFSET_Z]      = objOffsetZ;

    // Attach
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_X]          = attachX;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_Y]          = attachY;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_Z]          = attachZ;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_ROT_X]      = attachRotX;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_ROT_Y]      = attachRotY;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_ROT_Z]      = attachRotZ;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_SCALE_X]    = attachScaleX;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_SCALE_Y]    = attachScaleY;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_SCALE_Z]    = attachScaleZ;
    gItemBuildData[buildid][E_ITEM_BUILD_MATERIAL_COLOUR_1] = materialColour1;
    gItemBuildData[buildid][E_ITEM_BUILD_MATERIAL_COLOUR_2] = materialColour2;
    gItemBuildData[buildid][E_ITEM_BUILD_USE_CARRY_ANIM]    = useCarryAnim;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_BONE_ID]    = bone;
    gItemBuildData[buildid][E_ITEM_BUILD_BTN_OFFSET_Z]      = buttonOffsetZ;

    return buildid;
}

stock bool:IsValidItemBuild(ItemBuild:buildid) {
    return (0 <= _:buildid < _:gItemBuildCount);
}

stock bool:GetItemBuildName(ItemBuild:buildid, output[], size = sizeof (output)) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    strcopy(output, gItemBuildData[buildid][E_ITEM_BUILD_NAME], size);

    return true;
}

stock GetItemBuildModel(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return 0;
    }

    return gItemBuildData[buildid][E_ITEM_BUILD_MODEL_ID];
}

stock bool:SetItemBuildSize(ItemBuild:buildid, size) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    if (size < ITEM_BUILD_DEFAULT_SIZE) {
        return false;
    }

    gItemBuildData[buildid][E_ITEM_BUILD_SIZE] = size;

    return true;
}

stock GetItemBuildSize(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return ITEM_BUILD_DEFAULT_SIZE;
    }

    return gItemBuildData[buildid][E_ITEM_BUILD_SIZE];
}

// Separate from BuildItem because these are the numbers nobody gets right on
// the first try: an object's pivot is wherever the model was authored, never the
// grip of a hand, so every build needs its own six values found by looking at
// it. EditItemAttach is the way to find them; this is where they land.
stock bool:SetItemBuildAttach(ItemBuild:buildid, Float:offsetX, Float:offsetY, Float:offsetZ, Float:rotationX, Float:rotationY, Float:rotationZ, bone = 6, Float:scaleX = 1.0, Float:scaleY = 1.0, Float:scaleZ = 1.0) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_X]       = offsetX;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_Y]       = offsetY;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_Z]       = offsetZ;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_ROT_X]   = rotationX;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_ROT_Y]   = rotationY;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_ROT_Z]   = rotationZ;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_BONE_ID] = bone;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_SCALE_X] = scaleX;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_SCALE_Y] = scaleY;
    gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_SCALE_Z] = scaleZ;

    return true;
}

stock bool:GetItemBuildAttach(ItemBuild:buildid, &Float:offsetX, &Float:offsetY, &Float:offsetZ, &Float:rotationX, &Float:rotationY, &Float:rotationZ, &bone = 0, &Float:scaleX = 0.0, &Float:scaleY = 0.0, &Float:scaleZ = 0.0) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    offsetX   = gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_X];
    offsetY   = gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_Y];
    offsetZ   = gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_Z];
    rotationX = gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_ROT_X];
    rotationY = gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_ROT_Y];
    rotationZ = gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_ROT_Z];
    bone      = gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_BONE_ID];
    scaleX    = gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_SCALE_X];
    scaleY    = gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_SCALE_Y];
    scaleZ    = gItemBuildData[buildid][E_ITEM_BUILD_ATTACH_SCALE_Z];

    return true;
}

stock CountItemBuild() {
    return _:gItemBuildCount;
}

/**
 * # Item
 */

stock Item:CreateItem(ItemBuild:buildid, const uuid[] = "", extraid = -1, const extraName[] = "") {
    if (!IsValidItemBuild(buildid)) {
        return INVALID_ITEM_ID;
    }

    new const
        Item:itemid = AllocItemInternal()
    ;

    if (itemid == INVALID_ITEM_ID) {
        return INVALID_ITEM_ID;
    }

    gItemData[itemid][E_ITEM_CREATED] = true;

    if (isnull(uuid)) {
        GenerateItemUUIDInternal(gItemData[itemid][E_ITEM_UUID]);
    } else if (!ParseItemUUIDInternal(uuid, gItemData[itemid][E_ITEM_UUID])) {
        // A caller that hands over something that is not a UUID gets a real one
        // rather than an item that cannot be told apart from another.
        GenerateItemUUIDInternal(gItemData[itemid][E_ITEM_UUID]);
    }

    if (!isnull(extraName)) {
        strcopy(gItemData[itemid][E_ITEM_EXTRA_NAME], extraName);
    }

    gItemData[itemid][E_ITEM_BUILD_ID] = buildid;
    gItemData[itemid][E_ITEM_EXTRA_ID] = extraid;

    CallLocalFunction("OnItemCreate", "i", _:itemid);

    return itemid;
}

stock bool:DestroyItem(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    RemoveItemFromWorld(itemid);
    RemoveCurrentItem(GetItemHolder(itemid));

    // Invalid from here on, so a handler that destroys it again is refused,
    // but the id is not handed out until the handlers have run.
    gItemData[itemid][E_ITEM_CREATED] = false;

    CallLocalFunction("OnItemDestroy", "i", _:itemid);

    static const
        ITEM_DATA_NONE[E_ITEM_DATA]
    ;

    gItemData[itemid] = ITEM_DATA_NONE;

    FreeItemInternal(itemid);

    return true;
}

stock bool:IsValidItem(Item:itemid) {
    return IsValidItemInternal(itemid);
}

stock GetItemCount() {
    return gItemCount;
}

stock ItemBuild:GetItemBuild(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return INVALID_ITEM_BUILD_ID;
    }

    return gItemData[itemid][E_ITEM_BUILD_ID];
}

stock bool:SetItemExtraID(Item:itemid, extraid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    gItemData[itemid][E_ITEM_EXTRA_ID] = extraid;

    return true;
}

stock GetItemExtraID(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return -1;
    }

    return gItemData[itemid][E_ITEM_EXTRA_ID];
}

stock bool:SetItemExtraName(Item:itemid, const name[], OPEN_MP_TAGS:...) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    new
        buffer[MAX_ITEM_EXTRA_NAME]
    ;

    if (numargs() == 2) {
        format(buffer, sizeof (buffer), "%s", name);
    } else {
        format(buffer, sizeof (buffer), name, ___(2));
    }

    strcopy(gItemData[itemid][E_ITEM_EXTRA_NAME], buffer);

    return true;
}

stock bool:GetItemExtraName(Item:itemid, output[], size = sizeof (output)) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    strcopy(output, gItemData[itemid][E_ITEM_EXTRA_NAME], size);

    return true;
}

stock bool:GetItemUUID(Item:itemid, output[], size = sizeof (output)) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    FormatItemUUIDInternal(gItemData[itemid][E_ITEM_UUID], output, size);

    return true;
}

stock bool:GetItemName(Item:itemid, output[], size = sizeof (output)) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    new const
        ItemBuild:buildid = gItemData[itemid][E_ITEM_BUILD_ID]
    ;

    if (isnull(gItemData[itemid][E_ITEM_EXTRA_NAME])) {
        strcopy(output, gItemBuildData[buildid][E_ITEM_BUILD_NAME], size);
        
        return true;
    }

    format(output, size, "%s (%s)",
        gItemBuildData[buildid][E_ITEM_BUILD_NAME],
        gItemData[itemid][E_ITEM_EXTRA_NAME]
    );

    return true;
}

/**
 * # Item World
 */

stock bool:CreateItemInWorld(Item:itemid, Float:x, Float:y, Float:z, Float:rotationX, Float:rotationY, Float:rotationZ, KEY:buttonKey = ITEM_KEY_PICK_UP_ITEM, worldid = -1, interiorid = -1, Float:drawDistance = ITEM_DEFAULT_DRAW_DISTANCE, Float:streamDistance = ITEM_DEFAULT_STREAM_DISTANCE, bool:useRotOffsets = true, priority = ITEM_DEFAULT_BUTTON_PRIORITY) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    if (gItemData[itemid][E_ITEM_IN_WORLD]) {
        return false;
    }

    new const
        ItemBuild:buildid = gItemData[itemid][E_ITEM_BUILD_ID]
    ;

    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    new const
        Button:buttonid = CreateButton(x, y, z + gItemBuildData[buildid][E_ITEM_BUILD_BTN_OFFSET_Z], buttonKey, 1.0, worldid, interiorid, priority)
    ;

    if (buttonid == INVALID_BUTTON_ID) {
        return false;
    }

    gItemData[itemid][E_ITEM_X] = x;
    gItemData[itemid][E_ITEM_Y] = y;
    gItemData[itemid][E_ITEM_Z] = z;

    gItemData[itemid][E_ITEM_ROT_X] = rotationX;
    gItemData[itemid][E_ITEM_ROT_Y] = rotationY;
    gItemData[itemid][E_ITEM_ROT_Z] = rotationZ;

    gItemData[itemid][E_ITEM_WORLD_ID] = worldid;
    gItemData[itemid][E_ITEM_INTERIOR_ID] = interiorid;

    if (useRotOffsets) {
        rotationX += gItemBuildData[buildid][E_ITEM_BUILD_OBJ_ROT_OFFSET_X];
        rotationY += gItemBuildData[buildid][E_ITEM_BUILD_OBJ_ROT_OFFSET_Y];
        rotationZ += gItemBuildData[buildid][E_ITEM_BUILD_OBJ_ROT_OFFSET_Z];
    }

    gItemData[itemid][E_ITEM_BUTTON_ID] = buttonid;
    gItemData[itemid][E_ITEM_OBJECT_ID] = CreateDynamicObject(
        gItemBuildData[buildid][E_ITEM_BUILD_MODEL_ID],
        x,
        y,
        z + gItemBuildData[buildid][E_ITEM_BUILD_OBJ_OFFSET_Z],
        rotationX,
        rotationY,
        rotationZ,
        worldid,
        interiorid,
        .streamdistance = streamDistance,
        .drawdistance = drawDistance
    );

    SetButtonExtraData(buttonid, BUTTON_KEY_ITEM, _:itemid);

    gItemData[itemid][E_ITEM_IN_WORLD] = true;

    CallLocalFunction("OnItemCreateInWorld", "i", _:itemid);

    return true;
}

stock bool:IsItemInWorld(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    return gItemData[itemid][E_ITEM_IN_WORLD];
}

stock bool:RemoveItemFromWorld(Item:itemid) {
    if (!IsItemInWorld(itemid)) {
        return false;
    }

    DestroyButton(gItemData[itemid][E_ITEM_BUTTON_ID]);
    DestroyDynamicObject(gItemData[itemid][E_ITEM_OBJECT_ID]);

    gItemData[itemid][E_ITEM_BUTTON_ID] = INVALID_BUTTON_ID;
    gItemData[itemid][E_ITEM_OBJECT_ID] = STREAMER_TAG_OBJECT:INVALID_STREAMER_ID;

    gItemData[itemid][E_ITEM_IN_WORLD] = false;

    CallLocalFunction("OnItemRemoveFromWorld", "i", _:itemid);

    return true;
}

stock bool:SetItemPos(Item:itemid, Float:x, Float:y, Float:z) {
    if (!IsItemInWorld(itemid)) {
        return false;
    }

    new const
        ItemBuild:buildid = gItemData[itemid][E_ITEM_BUILD_ID]
    ;

    gItemData[itemid][E_ITEM_X] = x;
    gItemData[itemid][E_ITEM_Y] = y;
    gItemData[itemid][E_ITEM_Z] = z;

    SetButtonPos(gItemData[itemid][E_ITEM_BUTTON_ID], x, y, z + gItemBuildData[buildid][E_ITEM_BUILD_BTN_OFFSET_Z]);
    SetDynamicObjectPos(gItemData[itemid][E_ITEM_OBJECT_ID], x, y, z + gItemBuildData[buildid][E_ITEM_BUILD_OBJ_OFFSET_Z]);

    return true;
}

stock bool:GetItemPos(Item:itemid, &Float:x, &Float:y, &Float:z) {
    if (!IsItemInWorld(itemid)) {
        return false;
    }

    x = gItemData[itemid][E_ITEM_X];
    y = gItemData[itemid][E_ITEM_Y];
    z = gItemData[itemid][E_ITEM_Z];

    return true;
}

stock bool:SetItemRot(Item:itemid, Float:rotationX, Float:rotationY, Float:rotationZ, bool:useRotOffsets = true) {
    if (!IsItemInWorld(itemid)) {
        return false;
    }

    if (useRotOffsets) {
        new const
            ItemBuild:buildid = gItemData[itemid][E_ITEM_BUILD_ID]
        ;

        SetDynamicObjectRot(gItemData[itemid][E_ITEM_OBJECT_ID],
            rotationX + gItemBuildData[buildid][E_ITEM_BUILD_OBJ_ROT_OFFSET_X],
            rotationY + gItemBuildData[buildid][E_ITEM_BUILD_OBJ_ROT_OFFSET_Y],
            rotationZ + gItemBuildData[buildid][E_ITEM_BUILD_OBJ_ROT_OFFSET_Z]
        );
    } else {
        SetDynamicObjectRot(gItemData[itemid][E_ITEM_OBJECT_ID], rotationX, rotationY, rotationZ);
    }

    gItemData[itemid][E_ITEM_ROT_X] = rotationX;
    gItemData[itemid][E_ITEM_ROT_Y] = rotationY;
    gItemData[itemid][E_ITEM_ROT_Z] = rotationZ;

    return true;
}

stock bool:GetItemRot(Item:itemid, &Float:rotationX, &Float:rotationY, &Float:rotationZ) {
    if (!IsItemInWorld(itemid)) {
        return false;
    }

    rotationX = gItemData[itemid][E_ITEM_ROT_X];
    rotationY = gItemData[itemid][E_ITEM_ROT_Y];
    rotationZ = gItemData[itemid][E_ITEM_ROT_Z];

    return true;
}

stock bool:SetItemVirtualWorld(Item:itemid, worldid) {
    if (!IsItemInWorld(itemid)) {
        return false;
    }

    SetButtonVirtualWorld(gItemData[itemid][E_ITEM_BUTTON_ID], worldid);
    gItemData[itemid][E_ITEM_WORLD_ID] = worldid;

    return true;
}

stock GetItemVirtualWorld(Item:itemid) {
    if (!IsItemInWorld(itemid)) {
        return -1;
    }

    return gItemData[itemid][E_ITEM_WORLD_ID];
}

stock bool:SetItemInterior(Item:itemid, interiorid) {
    if (!IsItemInWorld(itemid)) {
        return false;
    }

    SetButtonInterior(gItemData[itemid][E_ITEM_BUTTON_ID], interiorid);
    gItemData[itemid][E_ITEM_INTERIOR_ID] = interiorid;

    return true;
}

stock GetItemInterior(Item:itemid) {
    if (!IsItemInWorld(itemid)) {
        return -1;
    }

    return gItemData[itemid][E_ITEM_INTERIOR_ID];
}

stock bool:SetItemLabelText(Item:itemid, const format[], colour = 0xFFFFFFFF, Float:drawDistance = ITEM_DEFAULT_DRAW_DISTANCE, Float:streamDistance = ITEM_DEFAULT_STREAM_DISTANCE, bool:testLOS = true, Float:offsetZ = 0.5, OPEN_MP_TAGS:...) {
    if (!IsItemInWorld(itemid)) {
        return false;
    }

    SetButtonLabelText(gItemData[itemid][E_ITEM_BUTTON_ID], format, colour, drawDistance, streamDistance, testLOS, offsetZ, ___(6));

    return true;
}

stock Button:GetItemButton(Item:itemid) {
    if (!IsItemInWorld(itemid)) {
        return INVALID_BUTTON_ID;
    }

    return gItemData[itemid][E_ITEM_BUTTON_ID];
}

stock bool:HasButtonItem(Button:buttonid) {
    return HasButtonExtraData(buttonid, BUTTON_KEY_ITEM);
}

stock Item:GetButtonItem(Button:buttonid) {
    if (!HasButtonExtraData(buttonid, BUTTON_KEY_ITEM)) {
        return INVALID_ITEM_ID;
    }

    return Item:GetButtonExtraData(buttonid, BUTTON_KEY_ITEM);
}

stock Item:GetPlayerItem(playerid) {
    new const
        Button:buttonid = GetPlayerButton(playerid)
    ;

    if (buttonid == INVALID_BUTTON_ID) {
        return INVALID_ITEM_ID;
    }

    return GetButtonItem(buttonid);
}

stock bool:GetPlayerNearbyItems(playerid, Item:items[MAX_AREA_BUTTONS] = { INVALID_ITEM_ID, ... }, &count = 0) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IsPlayerInAnyButton(playerid)) {
        return false;
    }

    new
        Button:buttons[MAX_AREA_BUTTONS],
        buttonCount,
        Item:buttonItem
    ;

    GetPlayerNearbyButtons(playerid, buttons, buttonCount);

    count = 0;

    for (new i; i != buttonCount; ++i) {
        buttonItem = GetButtonItem(buttons[i]);

        if (buttonItem != INVALID_ITEM_ID) {
            items[count++] = buttonItem;
        }
    }

    return (count != 0);
}

/**
 * # Interaction
 */

stock bool:GiveWorldItemToPlayer(playerid, Item:itemid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IsItemInWorld(itemid)) {
        return false;
    }

    SetPlayerItemInternal(playerid, itemid);

    return true;
}

stock bool:SetPlayerItem(playerid, Item:itemid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IsValidItem(itemid)) {
        return false;
    }

    SetPlayerItemInternal(playerid, itemid);

    return true;
}

stock bool:PlayerPickUpItem(playerid, Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    if (IsValidItem(gHoldItemID[playerid])) {
        return false;
    }

    new
        Float:x,
        Float:y,
        Float:z
    ;

    GetPlayerPos(playerid, x, y, z);
    ClearAnimations(playerid, SYNC_NONE);

    SetPlayerPos(playerid, x, y, z);
    SetPlayerFacingAngle(playerid, GetAngleToPoint(x, y, gItemData[itemid][E_ITEM_X], gItemData[itemid][E_ITEM_Y]));

    new const
        ItemBuild:buildid = gItemData[itemid][E_ITEM_BUILD_ID]
    ;

    gInteractItemID[playerid] = itemid;
    gItemInteractorID[itemid] = playerid;

    if ((z - gItemData[itemid][E_ITEM_Z]) < 0.3) {
        if (gItemBuildData[buildid][E_ITEM_BUILD_USE_CARRY_ANIM]) {
            ApplyAnimation(playerid, "CARRY", "LIFTUP105", 5.0, false, false, false, false, 400);
        } else {
            ApplyAnimation(playerid, "CASINO", "SLOT_PLYR", 4.0, false, false, false, false, 0);
        }

        gInteractTimer[playerid] = SetTimerEx("@Item_PickUpItemDelay", 400, false, "ii", playerid, false);
    } else {
        if (gItemBuildData[buildid][E_ITEM_BUILD_USE_CARRY_ANIM]) {
            ApplyAnimation(playerid, "CARRY", "LIFTUP", 5.0, false, false, false, false, 400);
        } else {
            ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_IN", 5.0, false, false, false, false, 450);
        }

        gInteractTimer[playerid] = SetTimerEx("@Item_PickUpItemDelay", 400, false, "ii", playerid, true);
    }

    return true;
}

stock bool:PlayerDropItem(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IsValidItem(gHoldItemID[playerid])) {
        return false;
    }

    if (CallLocalFunction("OnPlayerDropItem", "ii", playerid, _:gHoldItemID[playerid])) {
        return true;
    }

    new const
        Item:itemid = gHoldItemID[playerid],
        ItemBuild:buildid = gItemData[itemid][E_ITEM_BUILD_ID]
    ;

    gInteractItemID[playerid] = itemid;
    gItemInteractorID[itemid] = playerid;

    if (gItemBuildData[buildid][E_ITEM_BUILD_USE_CARRY_ANIM]) {
        ApplyAnimation(playerid, "CARRY", "PUTDWN", 5.0, false, false, false, false, 0);
    } else {
        ApplyAnimation(playerid, "BOMBER", "BOM_PLANT_IN", 5.0, true, false, false, false, 450);
    }

    gInteractTimer[playerid] = SetTimerEx("@Item_DropItemDelay", 400, false, "i", playerid);

    return true;
}

stock bool:PlayerUseItem(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IsIdleAnimationInternal(GetPlayerAnimationIndex(playerid))) {
        return false;
    }

    if (gInteractItemID[playerid] != INVALID_ITEM_ID) {
        return false;
    }

    if (gHoldItemID[playerid] == INVALID_ITEM_ID) {
        return false;
    }

    CallLocalFunction("OnPlayerUseItem", "ii", playerid, _:gHoldItemID[playerid]);

    return true;
}

// A development tool, not something a gamemode should leave reachable in play:
// it hands the player the client's own attachment editor so the six numbers a
// build needs can be found by dragging instead of guessed. What comes back is
// written into the build, which makes it live for every item of that build at
// once -- and dies with the process. OnPlayerEditItemAttach is where the values
// are handed over to be printed and pasted into SetItemBuildAttach.
stock bool:EditItemAttach(playerid) {
    if (!IsValidItem(gHoldItemID[playerid])) {
        return false;
    }

    if (!IsPlayerAttachedObjectSlotUsed(playerid, ITEM_ATTACH_INDEX)) {
        return false;
    }

    return EditAttachedObject(playerid, ITEM_ATTACH_INDEX);
}

stock bool:RemoveCurrentItem(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IsValidItem(gHoldItemID[playerid])) {
        return false;
    }

    new const
        Item:itemid = gHoldItemID[playerid],
        ItemBuild:buildid = gItemData[itemid][E_ITEM_BUILD_ID]
    ;

    if (IsPlayerAttachedObjectSlotUsed(playerid, ITEM_ATTACH_INDEX)) {
        RemovePlayerAttachedObject(playerid, ITEM_ATTACH_INDEX);
    }

    if (gItemBuildData[buildid][E_ITEM_BUILD_USE_CARRY_ANIM]) {
        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
    }

    gHoldItemID[playerid] = INVALID_ITEM_ID;
    gItemHolderID[itemid] = INVALID_PLAYER_ID;
    gInteractItemID[playerid] = INVALID_ITEM_ID;
    gItemInteractorID[itemid] = INVALID_PLAYER_ID;

    return true;
}

stock GetItemHolder(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return INVALID_PLAYER_ID;
    }

    return gItemHolderID[itemid];
}

stock GetItemInteractor(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return INVALID_PLAYER_ID;
    }

    return gItemInteractorID[itemid];
}

stock Item:GetPlayerHoldItem(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_ITEM_ID;
    }

    return gHoldItemID[playerid];
}

stock Item:GetPlayerInteractItem(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_ITEM_ID;
    }

    return gInteractItemID[playerid];
}

/**
 * # Calls
 */

hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if (!IsPlayerInAnyVehicle(playerid) && GetPlayerState(playerid) != PLAYER_STATE_SPECTATING) {
        if (newkeys & ITEM_KEY_DROP_ITEM) {
            PlayerDropItem(playerid);
        }

        if (newkeys & ITEM_KEY_USE_ITEM) {
            PlayerUseItem(playerid);
        }
    }

    // Zero, and not the 1 a callback is normally ended with: under pp-hooks a
    // hook that returns anything else is saying it has dealt with the key and
    // nothing further needs to see it. This one has not -- it acts on two keys
    // out of the set and has no opinion about the rest.
    return 0;
}

// The returns below are ones rather than the ~1 they used to be. That form is
// YSI's, where the complement of a value reads as "stop the chain and return
// this instead". pp-hooks has no such reading -- it stops on anything non-zero
// and hands that value back -- so ~1 only ever meant "stop, and return -2".
hook OnButtonPress(playerid, Button:buttonid) {
    new const
        Item:itemid = GetButtonItem(buttonid)
    ;

    if (itemid != INVALID_ITEM_ID) {
        if (gItemHolderID[itemid] == INVALID_PLAYER_ID && gItemInteractorID[itemid] == INVALID_PLAYER_ID) {
            if (IsValidItem(gHoldItemID[playerid])) {
                CallLocalFunction("OnPlayerPickUpItemWithItem", "iii", playerid, _:itemid, _:gHoldItemID[playerid]);

                return 1;
            }

            if (CallLocalFunction("OnPlayerPickUpItem", "ii", playerid, _:itemid)) {
                return 1;
            }

            PlayerPickUpItem(playerid, itemid);
        }
    }

    return 0;
}

hook OnPlayerEnterButtonArea(playerid, Button:buttonid) {
    new const
        Item:itemid = GetButtonItem(buttonid)
    ;

    if (itemid != INVALID_ITEM_ID) {
        if (CallLocalFunction("OnPlayerEnterItemArea", "ii", playerid, _:itemid)) {
            return 1;
        }
    }

    return 0;
}

hook OnPlayerLeaveButtonArea(playerid, Button:buttonid) {
    new const
        Item:itemid = GetButtonItem(buttonid)
    ;

    if (itemid != INVALID_ITEM_ID) {
        if (CallLocalFunction("OnPlayerLeaveItemArea", "ii", playerid, _:itemid)) {
            return 1;
        }
    }

    return 0;
}

hook OnPlayerEditAttachedObject(playerid, EDIT_RESPONSE:response, index, modelid, boneid, Float:offsetX, Float:offsetY, Float:offsetZ, Float:rotationX, Float:rotationY, Float:rotationZ, Float:scaleX, Float:scaleY, Float:scaleZ) {
    if (index != ITEM_ATTACH_INDEX) {
        return 0;
    }

    if (!IsValidItem(gHoldItemID[playerid])) {
        return 0;
    }

    new const
        ItemBuild:buildid = gItemData[gHoldItemID[playerid]][E_ITEM_BUILD_ID]
    ;

    // Cancelling closes the editor but leaves the client showing wherever the
    // object was dragged to, so both answers end with the build reapplied. The
    // difference is only whether the build learned anything first.
    if (response != EDIT_RESPONSE_CANCEL) {
        SetItemBuildAttach(buildid, offsetX, offsetY, offsetZ, rotationX, rotationY, rotationZ, boneid, scaleX, scaleY, scaleZ);
    }

    ApplyItemAttachInternal(playerid, buildid);

    if (response == EDIT_RESPONSE_CANCEL) {
        return 0;
    }

    CallLocalFunction("OnPlayerEditItemAttach", "iiffffffifff",
        playerid,
        _:buildid,
        offsetX,
        offsetY,
        offsetZ,
        rotationX,
        rotationY,
        rotationZ,
        boneid,
        scaleX,
        scaleY,
        scaleZ
    );

    return 0;
}

hook OnPlayerConnect(playerid) {
    gHoldItemID[playerid] = INVALID_ITEM_ID;
    gInteractItemID[playerid] = INVALID_ITEM_ID;
    
    KillTimer(gInteractTimer[playerid]);

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    KillTimer(gInteractTimer[playerid]);

    return 0;
}