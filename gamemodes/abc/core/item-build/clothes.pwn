#if defined _CORE_ITEM_BUILD_CLOTHES
    #endinput
#endif
#define _CORE_ITEM_BUILD_CLOTHES

#include <pp-hooks>

/**
 * # A package of clothes
 *
 * Two registers, not one. There is a list of outfits -- each with a model, a
 * name and a gender -- and separately there are items that hold one of them.
 *
 * The list is the reason a tag exists. A skin id is a number the game
 * understands and nothing else does: it cannot be shown to anybody, it cannot be
 * refused for being the wrong gender, and two of them cannot be told apart when
 * they happen to share a model. A Clothes: is an entry in a list somebody
 * wrote, so it has a name to print and rules to check, and the model is one of
 * the things it knows rather than the whole of what it is.
 *
 * Wearing does not use the package up. ScavengeSurvive's clothes item swaps: the
 * character puts on what was in it and it is left holding what they took off, so
 * changing twice ends with the first outfit back rather than with nothing.
 *
 * Which outfit any one package holds is on the item, never on the build. Two
 * packages of the same kind are two different outfits, and after a swap they are
 * two more.
 */

#if !defined MAX_CLOTHES
    #define MAX_CLOTHES (Clothes:32)
#endif

#if !defined MAX_CLOTHES_NAME
    #define MAX_CLOTHES_NAME (32)
#endif

#define INVALID_CLOTHES_ID (Clothes:-1)

enum E_GENDER {
    GENDER_NEUTRAL,
    GENDER_MALE,
    GENDER_FEMALE
};

static enum _:E_CLOTHES_DATA {
    E_CLOTHES_NAME[MAX_CLOTHES_NAME],
    E_CLOTHES_MODEL,
    E_GENDER:E_CLOTHES_GENDER,

    bool:E_CLOTHES_WEARS_HATS,
    bool:E_CLOTHES_WEARS_MASKS
};

static const
    ITEM_KEY_CLOTHES[] = "clothes"
;

static
    gClothesData[MAX_CLOTHES][E_CLOTHES_DATA],
    Clothes:gClothesCount,

    bool:gItemBuildIsClothes[MAX_ITEM_BUILDS],
    
    Clothes:gPlayerClothes[MAX_PLAYERS] = { INVALID_CLOTHES_ID, ... },
    Item:gPlayerChangingInto[MAX_PLAYERS] = { INVALID_ITEM_ID, ... }
;

/**
 * # Functions
 */

/**
 * @brief      Register an outfit and answer the type it was given.
 *
 * The order they are registered in is the Clothes, and items and characters
 * both store one, so removing an entry moves every outfit below it. Add at the
 * end.
 *
 * @param      model       Skin model the outfit draws as, 1 to 311.
 * @param      name        What it is called, shown on an item holding it.
 * @param      gender      Who it is cut for, used to name the item.
 * @param      wearsHats   Whether a hat can go on over it.
 * @param      wearsMasks  Whether a mask can go on over it.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the `Clothes:` it was given
 *             - `INVALID_CLOTHES_ID` when:
 *                 + `MAX_CLOTHES` outfits already exist
 *                 + `model` is zero or below
 */
forward Clothes:DefineClothes(model, const name[], E_GENDER:gender, bool:wearsHats = true, bool:wearsMasks = true);

/**
 * @brief      Whether an outfit was ever registered under this type.
 *
 * @param      clothes  Type to check.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is one of the registered outfits
 *             - `false` it is below zero or past the last one registered
 */
forward bool:IsValidClothes(Clothes:clothes);

/**
 * @brief      How many outfits are registered.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     the count, which is also one past the highest valid type
 */
forward CountClothes();

/**
 * @brief      The skin model an outfit draws as.
 *
 * @param      clothes  Type to read.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the model, 1 to 311
 *             - `0` when `clothes` is not a registered type
 */
forward GetClothesModel(Clothes:clothes);

/**
 * @brief      What an outfit is called.
 *
 * @param      clothes  Type to read.
 * @param      output   Buffer the name is written into.
 * @param      size     Size of that buffer.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` a name was written into `output`
 *             - `false` `output` is left alone, when `clothes` is not a
 *               registered type
 */
forward bool:GetClothesName(Clothes:clothes, output[], size = sizeof (output));

/**
 * @brief      Who an outfit is cut for.
 *
 * @param      clothes  Type to read.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `GENDER_MALE`, `GENDER_FEMALE` or `GENDER_NEUTRAL`
 *             - `GENDER_NEUTRAL` when `clothes` is not a registered type
 */
forward E_GENDER:GetClothesGender(Clothes:clothes);

/**
 * @brief      Whether a hat can go on over an outfit.
 *
 * @param      clothes  Type to read.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` a hat is allowed
 *             - `false` it is not, or `clothes` is not a registered type
 */
forward bool:ClothesWearsHats(Clothes:clothes);

/**
 * @brief      Whether a mask can go on over an outfit.
 *
 * @param      clothes  Type to read.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` a mask is allowed
 *             - `false` it is not, or `clothes` is not a registered type
 */
forward bool:ClothesWearsMasks(Clothes:clothes);

/**
 * @brief      What a character is wearing.
 *
 * The skin cannot answer this: two outfits are allowed to share a model, and
 * the game only knows the model.
 *
 * @param      playerid  Character to read.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the `Clothes:` they are in
 *             - `INVALID_CLOTHES_ID` when they are not connected, or have not
 *               been dressed through this file
 */
forward Clothes:GetPlayerClothes(playerid);

/**
 * @brief      Put an outfit on a character.
 *
 * @param      playerid  Character to dress.
 * @param      clothes   Outfit to put on them.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the skin was set and remembered
 *             - `false` nothing changed, when `clothes` is not a registered type
 */
forward bool:SetPlayerClothes(playerid, Clothes:clothes);

/**
 * @brief      Say that this kind of item is a package of clothes.
 *
 * A package has no quantity to count, so the corner of its slot carries the
 * skin instead. That is set here rather than in the catalogue for the same
 * reason the rest of this is: a build that has been told it is clothes has
 * been told everything that follows from it.
 *
 * Which outfit any one package holds is the item's business, not the kind's.
 *
 * @param      buildid  Kind of item to mark.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the kind is now clothing
 *             - `false` `buildid` is not a valid build
 */
forward bool:DefineItemBuildClothes(ItemBuild:buildid);

/**
 * @brief      Whether an item is a package of clothes.
 *
 * @param      itemid  Item to check.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` its kind was marked with `DefineItemBuildClothes`
 *             - `false` it was not, or `itemid` is not a valid item
 */
forward bool:IsItemClothes(Item:itemid);

/**
 * @brief      What a package is holding.
 *
 * @param      itemid  Package to read.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the `Clothes:` folded into it
 *             - `INVALID_CLOTHES_ID` when:
 *                 + `itemid` is not a package of clothes
 *                 + nothing has been folded into it yet
 */
forward Clothes:GetItemClothes(Item:itemid);

/**
 * @brief      Fold an outfit into a package, named after what is inside.
 *
 * What is in the package is what is worth looking at, so the slot shows the
 * outfit rather than the wrapping: the preview becomes the skin, and the
 * number beside it is which one.
 *
 * Only the preview changes. The object left on the ground and the one in a
 * character's hand are still the package, because that is what a person
 * carrying folded clothes is holding.
 *
 * A shop or a wardrobe calls this when it makes one. A package that was never
 * told holds nothing and cannot be worn.
 *
 * @param      itemid   Package to fill.
 * @param      clothes  Outfit to fold into it.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the package holds it and reads as "Male Cop" or similar
 *             - `false` nothing changed, when:
 *                 + `itemid` is not a package of clothes
 *                 + `clothes` is not a registered type
 */
forward bool:SetItemClothes(Item:itemid, Clothes:clothes);

/**
 * @brief      Put a package on, leaving it holding what was taken off.
 *
 * The two outfits swap places, so a character who changes twice ends up with the
 * first one back rather than with nothing.
 *
 * @param      playerid  Character to dress.
 * @param      itemid    Package to wear.
 * @param      worn      Set to the outfit that was taken off.
 *
 * @date       01:20 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the character changed
 *             - `false` nothing changed, when the package holds nothing
 */
forward bool:WearItemClothes(playerid, Item:itemid, &Clothes:worn = INVALID_CLOTHES_ID);

/**
 * # Internal
 */

static stock StopChangingInternal(playerid) {
    if (!IsValidItem(gPlayerChangingInto[playerid])) {
        return;
    }

    gPlayerChangingInto[playerid] = INVALID_ITEM_ID;

    StopHoldAction(playerid);
}

/**
 * # External
 */

stock Clothes:DefineClothes(model, const name[], E_GENDER:gender, bool:wearsHats = true, bool:wearsMasks = true) {
    if (gClothesCount == MAX_CLOTHES) {
        return INVALID_CLOTHES_ID;
    }

    if (model <= 0) {
        return INVALID_CLOTHES_ID;
    }

    new const
        Clothes:clothes = gClothesCount++
    ;

    strcat(gClothesData[clothes][E_CLOTHES_NAME], name);
    gClothesData[clothes][E_CLOTHES_MODEL] = model;
    gClothesData[clothes][E_CLOTHES_GENDER] = gender;
    gClothesData[clothes][E_CLOTHES_WEARS_HATS] = wearsHats;
    gClothesData[clothes][E_CLOTHES_WEARS_MASKS] = wearsMasks;

    return clothes;
}

stock bool:IsValidClothes(Clothes:clothes) {
    return (0 <= _:clothes < _:gClothesCount);
}

stock CountClothes() {
    return _:gClothesCount;
}

stock GetClothesModel(Clothes:clothes) {
    if (!IsValidClothes(clothes)) {
        return 0;
    }

    return gClothesData[clothes][E_CLOTHES_MODEL];
}

stock bool:GetClothesName(Clothes:clothes, output[], size = sizeof (output)) {
    if (!IsValidClothes(clothes)) {
        return false;
    }

    strcopy(output, gClothesData[clothes][E_CLOTHES_NAME], size);

    return true;
}

stock E_GENDER:GetClothesGender(Clothes:clothes) {
    if (!IsValidClothes(clothes)) {
        return GENDER_NEUTRAL;
    }

    return gClothesData[clothes][E_CLOTHES_GENDER];
}

stock bool:ClothesWearsHats(Clothes:clothes) {
    if (!IsValidClothes(clothes)) {
        return false;
    }

    return gClothesData[clothes][E_CLOTHES_WEARS_HATS];
}

stock bool:ClothesWearsMasks(Clothes:clothes) {
    if (!IsValidClothes(clothes)) {
        return false;
    }

    return gClothesData[clothes][E_CLOTHES_WEARS_MASKS];
}

stock Clothes:GetPlayerClothes(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_CLOTHES_ID;
    }

    return gPlayerClothes[playerid];
}

stock bool:SetPlayerClothes(playerid, Clothes:clothes) {
    if (!IsValidClothes(clothes)) {
        return false;
    }

    SetPlayerSkin(playerid, gClothesData[clothes][E_CLOTHES_MODEL]);

    gPlayerClothes[playerid] = clothes;

    return true;
}

stock bool:DefineItemBuildClothes(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    gItemBuildIsClothes[buildid] = true;

    SetItemBuildAmountType(buildid, ITEM_AMOUNT_INT);

    return true;
}

stock bool:IsItemClothes(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    return gItemBuildIsClothes[GetItemBuild(itemid)];
}

stock Clothes:GetItemClothes(Item:itemid) {
    if (!IsItemClothes(itemid)) {
        return INVALID_CLOTHES_ID;
    }

    if (!HasItemExtraData(itemid, ITEM_KEY_CLOTHES)) {
        return INVALID_CLOTHES_ID;
    }

    return Clothes:GetItemExtraDataInt(itemid, ITEM_KEY_CLOTHES);
}

stock bool:SetItemClothes(Item:itemid, Clothes:clothes) {
    if (!IsItemClothes(itemid)) {
        return false;
    }

    if (!IsValidClothes(clothes)) {
        return false;
    }

    if (!SetItemExtraData(itemid, ITEM_KEY_CLOTHES, var_new(_:clothes))) {
        return false;
    }

    new
        name[MAX_CLOTHES_NAME],
        label[MAX_ITEM_EXTRA_NAME]
    ;

    GetClothesName(clothes, name);

    switch (GetClothesGender(clothes)) {
        case GENDER_MALE: {
            format(label, sizeof (label), "Male %s", name);
        }
        case GENDER_FEMALE: {
            format(label, sizeof (label), "Female %s", name);
        }
        case GENDER_NEUTRAL: {
            strcopy(label, name);
        }
    }

    SetItemPreviewModel(itemid, GetClothesModel(clothes));
    SetItemAmountInt(itemid, GetClothesModel(clothes));

    return SetItemExtraName(itemid, label);
}

stock bool:WearItemClothes(playerid, Item:itemid, &Clothes:worn = INVALID_CLOTHES_ID) {
    new const
        Clothes:clothes = GetItemClothes(itemid)
    ;

    if (!IsValidClothes(clothes)) {
        return false;
    }

    worn = gPlayerClothes[playerid];

    if (!SetPlayerClothes(playerid, clothes)) {
        return false;
    }

    if (!IsValidClothes(worn)) {
        return true;
    }

    return SetItemClothes(itemid, worn);
}

/**
 * # Calls
 */

// A package comes with something folded in it. Without this only the admin
// command filled one, so a package out of a present or a locker held nothing --
// and a package holding nothing has no skin to draw, so the slot fell back to
// the wrapping's own model under preview settings meant for a person. It looked
// like an item with no preview. It was an item with no clothes.
hook OnItemCreate(Item:itemid) {
    if (!IsItemClothes(itemid)) {
        return 0;
    }

    new const
        count = CountClothes()
    ;

    if (count == 0) {
        return 0;
    }

    SetItemClothes(itemid, Clothes:random(count));

    return 0;
}

hook OnPlayerConnect(playerid) {
    gPlayerClothes[playerid] = INVALID_CLOTHES_ID;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    gPlayerClothes[playerid] = INVALID_CLOTHES_ID;

    return 0;
}

hook OnPlayerUseItem(playerid, Item:itemid) {
    if (!IsItemClothes(itemid)) {
        return 0;
    }

    if (!IsValidClothes(GetItemClothes(itemid))) {
        SendPlayerNotice(playerid, "There is nothing folded in it.");

        return 1;
    }

    StopChangingInternal(playerid);

    gPlayerChangingInto[playerid] = itemid;

    if (!StartHoldAction(playerid, ITEM_CHANGE_CLOTHES_TIME, "Changing", ITEM_KEY_USE_ITEM, HOLD_PRIORITY_CLOTHES)) {
        new
            busy[MAX_HOLD_ACTION_TITLE]
        ;

        gPlayerChangingInto[playerid] = INVALID_ITEM_ID;

        if (GetPlayerHoldActionTitle(playerid, busy)) {
            SendPlayerNotice(playerid, "You are busy: %s.", busy);
        }

        return 1;
    }

    ClearAnimations(playerid, SYNC_NONE);

    return 1;
}

hook OnHoldActionFinish(playerid) {
    new const
        Item:itemid = gPlayerChangingInto[playerid]
    ;

    if (!IsValidItem(itemid)) {
        return 0;
    }

    gPlayerChangingInto[playerid] = INVALID_ITEM_ID;

    new
        Clothes:worn = INVALID_CLOTHES_ID,
        name[MAX_ITEM_NAME]
    ;

    if (WearItemClothes(playerid, itemid, worn)) {
        GetItemName(itemid, name);
        SendPlayerNotice(playerid, "You change clothes. The %s holds what you had on.", name);
    }

    return 1;
}

hook OnPlayerDropItem(playerid, Item:itemid) {
    StopChangingInternal(playerid);

    return 0;
}
