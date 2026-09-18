#if defined _CORE_ITEM_FOOD
    #endinput
#endif
#define _CORE_ITEM_FOOD

#include <pp-hooks>

/**
 * # Eating
 *
 * One bar for the whole of it. The key is held down, the bar fills across every
 * mouthful the thing has in it, and letting go leaves what has not been eaten.
 * That is what the hold action was freed up for when opening moved to its own
 * key.
 *
 * A bar per bite was the obvious first shape and it was the wrong one. The bar
 * counted one thing and the animation another: FOOD/EAT_BURGER takes two bites
 * and wipes its mouth, which is a beginning and an end, and restarting it every
 * few seconds reads as a character who keeps finishing and starting again.
 * EAT_CHICKEN and EAT_PIZZA have no end -- they simply eat -- so they play once
 * across the whole bar and stop when it does.
 *
 * Bites are still counted. They are marks the bar passes rather than bars of
 * their own, so a burger eaten halfway is a burger with half its bites left and
 * the nutrition of the ones that went down.
 *
 * Only what is in the hands can be eaten. Not a rule invented here -- it falls
 * out of taking the food into the hands first, which is refused when the hands
 * are full of something that will not go back in a pocket. A character holding a
 * crate has to put it down, and that is the same sentence they already get for
 * everything else they try to do with full hands.
 *
 * What a bite is worth is carried but not spent. OnPlayerEat is where a
 * condition system reads it, and until one exists eating is a thing a character
 * does that leaves them with less food.
 *
 * Bites live in the item's extra data rather than its amount, because the amount
 * of a burger is already how long it has left before it spoils. They ride into
 * the database with the rest of the item's document for free.
 */

#if !defined ITEM_FOOD_BITES_KEY
    #define ITEM_FOOD_BITES_KEY "bites"
#endif

#if !defined MAX_ITEM_FOOD_ANIMATION
    #define MAX_ITEM_FOOD_ANIMATION (24)
#endif

static
    gItemBuildFoodBites[MAX_ITEM_BUILDS],
    Seconds:gItemBuildFoodLife[MAX_ITEM_BUILDS],
    Float:gItemBuildFoodNutrition[MAX_ITEM_BUILDS],
    gItemBuildFoodAnimation[MAX_ITEM_BUILDS][MAX_ITEM_FOOD_ANIMATION],

    Item:gPlayerEating[MAX_PLAYERS] = { INVALID_ITEM_ID, ... },

    // How many marks the bar has already gone past this time round. Kept per
    // player rather than read back off the item, because the item's own count
    // is only written when a mark is passed and this is what decides that.
    gPlayerBitesTaken[MAX_PLAYERS]
;

/**
 * # Functions
 */

/**
 * @brief      Say that a kind of item is eaten, and how much of it there is.
 *
 * @param      buildid    Build to make edible.
 * @param      bites      How many mouthfuls it takes to finish.
 * @param      nutrition  What one bite is worth, for whoever is counting.
 * @param      life       How long before it turns. Nothing means it never does,
 *                        which is what tinned and dried things are. Saying one
 *                        also says how it is displayed: a shelf life is a
 *                        countdown, and a countdown is what the corner of the
 *                        slot shows, so a catalogue does not say both.
 * @param      animation  Which FOOD animation to play, one that does not end:
 *                        EAT_CHICKEN and EAT_PIZZA eat until told to stop, and
 *                        EAT_BURGER takes two bites and wipes its mouth, which
 *                        is a beginning and an end and looks like one.
 *
 * @date       15:10 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the build is food
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + `bites` is zero or below
 */
forward bool:DefineItemBuildFood(ItemBuild:buildid, bites = 1, Float:nutrition = 0.0, Seconds:life = Seconds:0, const animation[] = ITEM_FOOD_DEFAULT_ANIMATION);

/**
 * @brief      Whether an item can be eaten.
 *
 * @param      itemid  Item to check.
 *
 * @date       15:10 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` its build was made edible
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + its build is not food
 */
forward bool:IsItemFood(Item:itemid);

/**
 * @brief      How many mouthfuls are left of this particular one.
 *
 * A burger nobody has touched answers its build's full count rather than
 * nothing, so an item that has never been bitten and one that was just made read
 * the same -- which they are.
 *
 * @param      itemid  Item to measure.
 *
 * @date       15:10 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - the bites left, one or more
 *             - `0` when it is not food, or nothing is left of it
 */
forward GetItemFoodBites(Item:itemid);

/**
 * @brief      Start a bite, if the character is holding it.
 *
 * Being in the hands is the whole of the rule. Everything else that wanted
 * to be a rule -- not while carrying a crate, not out of a locker across
 * the room -- is that one sentence seen from another angle.
 *
 * One bar covers the whole food rather than one per mouthful: it runs for
 * as long as there is left to eat, and the bites are marks along it. The
 * animation loops for that whole time and is cleared when the bar comes
 * down, which is why a hold that ends early has to say so.
 *
 * The hold is the bite. Letting the key go stops it and nothing is eaten, which
 * is why it is worth a bar: a character interrupted halfway through a sandwich
 * still has the sandwich.
 *
 * @param      playerid  Character eating.
 * @param      itemid    Food in their hands.
 *
 * @date       15:10 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they have started on it
 *             - `false` when:
 *                 + the item is not food
 *                 + it is not the item in their hands
 *                 + there is nothing left of it
 *                 + they are busy with something that outranks eating
 */
forward bool:EatItem(playerid, Item:itemid);

/**
 * # Events
 */

/**
 * @brief      A character has taken a bite of something.
 *
 * The last mark is never crossed by an update -- the tick that would have
 * reported it is the one that ends the action instead -- so the final bite
 * is reported when the bar finishes.
 *
 * A couple of mouthfuls of something worth less than nothing and the body
 * decides: the same rule drinking has, off the same number.
 *
 * Where a condition system spends the nutrition. The bite has already been
 * taken by the time this is called, and the item may already be gone -- the
 * last bite of something destroys it, so check before reaching for its name.
 *
 * @param      playerid   Character who ate.
 * @param      itemid     What they ate, valid only if something is left.
 * @param      nutrition  What that bite was worth.
 * @param      remaining  Bites left after this one.
 *
 * @date       15:10 08/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerEat(playerid, Item:itemid, Float:nutrition, remaining);

/**
 * # External
 */

stock bool:DefineItemBuildFood(ItemBuild:buildid, bites = 1, Float:nutrition = 0.0, Seconds:life = Seconds:0, const animation[] = ITEM_FOOD_DEFAULT_ANIMATION) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    if (bites <= 0) {
        return false;
    }

    gItemBuildFoodBites[buildid] = bites;
    gItemBuildFoodNutrition[buildid] = nutrition;
    gItemBuildFoodLife[buildid] = life;

    if (life > Seconds:0) {
        SetItemBuildAmountType(buildid, ITEM_AMOUNT_DURATION);
    }

    strcopy(gItemBuildFoodAnimation[buildid], animation);

    return true;
}

stock bool:IsItemFood(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    return (gItemBuildFoodBites[GetItemBuild(itemid)] != 0);
}

stock GetItemFoodBites(Item:itemid) {
    if (!IsItemFood(itemid)) {
        return 0;
    }

    if (!HasItemExtraData(itemid, ITEM_FOOD_BITES_KEY)) {
        return gItemBuildFoodBites[GetItemBuild(itemid)];
    }

    return GetItemExtraDataInt(itemid, ITEM_FOOD_BITES_KEY);
}

stock bool:EatItem(playerid, Item:itemid) {
    if (!IsItemFood(itemid)) {
        return false;
    }

    if (GetPlayerHoldItem(playerid) != itemid) {
        return false;
    }

    new const
        bites = GetItemFoodBites(itemid)
    ;

    if (bites <= 0) {
        return false;
    }

    if (!StartHoldAction(playerid, bites * ITEM_EAT_TIME, "Eating", ITEM_KEY_USE_ITEM, HOLD_PRIORITY_EAT)) {
        return false;
    }

    gPlayerEating[playerid] = itemid;
    gPlayerBitesTaken[playerid] = 0;

    ApplyAnimation(playerid, "FOOD", gItemBuildFoodAnimation[GetItemBuild(itemid)], ITEM_EAT_ANIMATION_SPEED, true, false, false, false, 0);

    return true;
}

/**
 * # Calls
 */

// Freshly made means fresh. Without this a burger is created with no duration
// at all, which reads as expired everywhere that asks and shows nothing in the
// slot -- the corner was empty because there was no countdown, not because the
// countdown was not being drawn.
hook OnItemCreate(Item:itemid) {
    new const
        Seconds:life = gItemBuildFoodLife[GetItemBuild(itemid)]
    ;

    if (life <= Seconds:0) {
        return 0;
    }

    SetItemDuration(itemid, life);

    return 0;
}

hook OnPlayerUseItem(playerid, Item:itemid) {
    if (!IsItemFood(itemid)) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (!EatItem(playerid, itemid)) {
        new
            busy[MAX_HOLD_ACTION_TITLE]
        ;

        if (GetPlayerHoldActionTitle(playerid, busy)) {
            SendPlayerNotice(playerid, "You are busy: %s.", busy);
        }

        return 1;
    }

    return 1;
}

// Every mark the bar has gone past since the last look. Normally one at a time,
// but a tick that arrives late covers whatever it skipped rather than losing it.
static stock TakeBitesInternal(playerid, Item:itemid, upTo) {
    new const
        ItemBuild:buildid = GetItemBuild(itemid)
    ;

    while (gPlayerBitesTaken[playerid] < upTo) {
        ++gPlayerBitesTaken[playerid];

        new const
            remaining = GetItemFoodBites(itemid) - 1
        ;

        SetItemExtraData(itemid, ITEM_FOOD_BITES_KEY, var_new(remaining));

        CallLocalFunction("OnPlayerEat", "iifi", playerid, _:itemid, _:gItemBuildFoodNutrition[buildid], remaining);

        if (gItemBuildFoodNutrition[buildid] < 0.0 && gPlayerBitesTaken[playerid] >= ITEM_VOMIT_SIPS) {
            new
                name[MAX_ITEM_NAME]
            ;

            GetItemName(itemid, name);

            StopHoldAction(playerid);
            VomitPlayer(playerid);

            SendPlayerNotice(playerid, "The %s comes straight back up.", name);

            return;
        }
    }
}

hook OnHoldActionUpdate(playerid, progress) {
    new const
        Item:itemid = gPlayerEating[playerid]
    ;

    if (!IsValidItem(itemid)) {
        return 0;
    }

    TakeBitesInternal(playerid, itemid, progress / ITEM_EAT_TIME);

    return 0;
}

hook OnHoldActionFinish(playerid) {
    new const
        Item:itemid = gPlayerEating[playerid]
    ;

    if (!IsValidItem(itemid)) {
        return 0;
    }

    gPlayerEating[playerid] = INVALID_ITEM_ID;

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    TakeBitesInternal(playerid, itemid, GetItemFoodBites(itemid) + gPlayerBitesTaken[playerid]);

    ClearAnimations(playerid, SYNC_NONE);

    RemoveCurrentItem(playerid);
    DestroyItem(itemid);

    SendPlayerNotice(playerid, "You finish the %s.", name);

    return 1;
}

// Let go of, or interrupted. What was eaten stays eaten and the rest is still
// in their hand -- the animation is the only thing that has to be told, since
// the game would keep chewing on its own.
hook OnHoldActionStop(playerid, progress) {
    if (!IsValidItem(gPlayerEating[playerid])) {
        return 0;
    }

    gPlayerEating[playerid] = INVALID_ITEM_ID;

    ClearAnimations(playerid, SYNC_NONE);

    return 0;
}

hook OnPlayerDropItem(playerid, Item:itemid) {
    if (gPlayerEating[playerid] == itemid) {
        StopHoldAction(playerid);
    }

    return 0;
}

hook OnItemDestroy(Item:itemid) {
    foreach (new i : Player) {
        if (gPlayerEating[i] == itemid) {
            gPlayerEating[i] = INVALID_ITEM_ID;
        }
    }

    return 0;
}

hook OnPlayerConnect(playerid) {
    gPlayerEating[playerid] = INVALID_ITEM_ID;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    gPlayerEating[playerid] = INVALID_ITEM_ID;

    return 0;
}
