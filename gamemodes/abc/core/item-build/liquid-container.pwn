#if defined _CORE_ITEM_BUILD_LIQUID_CONTAINER
    #endinput
#endif
#define _CORE_ITEM_BUILD_LIQUID_CONTAINER

#include <pp-hooks>

/**
 * # Things with liquid in them
 *
 * A can, a bottle, a jerrycan. The build says how much fits and whether it is
 * worth keeping once emptied; the item says what is in it and how much is left.
 *
 * Not a container in the other sense. What is inside is a quantity of one
 * liquid, not a list of items, so none of core/item-build/container applies --
 * an item cannot be a container and this at once, and nothing stops the
 * catalogue from trying, which is the one thing worth remembering here.
 *
 * Drinking is a hold, like eating: one bar, and mouthfuls are marks it passes.
 * Letting go leaves the rest in the can.
 *
 * A mouthful is one whole play of VEND_DRINK2_P, which raises the tin, drinks,
 * and holds it up at rest before coming down. That pause is why the animation
 * is started again on every mark rather than left looping: looping it plays the
 * rest as part of the loop and the character never finishes a swallow.
 *
 * The bar covers at most ITEM_MAX_SIPS_PER_HOLD mouthfuls, however much is in
 * the vessel. Twenty litres at a tenth of a litre a swallow is two hundred of
 * them, and a bar that takes thirteen minutes is not a bar -- a jerrycan is
 * carried, not drunk, and anybody determined to empty one may hold the key
 * again.
 *
 * A container that is not reusable is thrown away when it empties. One that is
 * stays, at zero, waiting to be filled -- and what fills it is another vessel:
 * PourItemLiquid puts one into the other, which is a litre of petrol in one can
 * and three in another becoming four in one can and nothing to carry.
 *
 * Which is why a jerrycan poured into a chainsaw is still a jerrycan. Emptying
 * one is not losing it, and a character who wants rid of it says so on the
 * DISCARD row rather than having it decided for them.
 *
 * Emptying one is not SetItemLiquid with nothing. That call is for filling and
 * refuses to be told zero, so a can poured out to the last drop by it kept
 * everything it had -- DrawItemLiquid is the other direction and goes to the
 * bottom.
 *
 * How much is left is the item's amount, not another key in its extra data.
 * That is what puts "330ML" in the corner of the slot without this file drawing
 * anything: DefineItemBuildLiquidContainer says the amount is a volume, and the
 * container menu already knows how to show one. Nothing else is competing for
 * the cell, a bottle having no freshness of its own.
 */

#if !defined ITEM_LIQUID_KEY
    #define ITEM_LIQUID_KEY "liquid"
#endif

// One bit per liquid, so a build says what it turns up holding without a list
// to walk. MAX_LIQUIDS is thirty two, which is one cell.
static
    Float:gItemBuildLiquidCapacity[MAX_ITEM_BUILDS],
    bool:gItemBuildLiquidReusable[MAX_ITEM_BUILDS],
    gItemBuildLiquidHolds[MAX_ITEM_BUILDS],

    Item:gPlayerDrinking[MAX_PLAYERS] = { INVALID_ITEM_ID, ... },
    gPlayerSipsTaken[MAX_PLAYERS]
;

/**
 * # Functions
 */

/**
 * @brief      Say that a kind of item holds liquid, and how much.
 *
 * A vessel and a container are different insides and an item may have only
 * one, so a build already given the other is refused here rather than left
 * to be discovered: one carrying both would answer yes to two systems that
 * each think they own what is in it.
 *
 * @param      buildid   Build to make a vessel.
 * @param      capacity  How many millilitres it takes.
 * @param      reusable  Whether it survives being emptied.
 *
 * @date       19:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the build holds liquid
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + `capacity` is zero or below
 *                 + the build already holds items, which is a different inside
 */
forward bool:DefineItemBuildLiquidContainer(ItemBuild:buildid, capacity, bool:reusable = false);

/**
 * @brief      Whether an item is a vessel for liquid.
 *
 * @param      itemid  Item to check.
 *
 * @date       19:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` its build was given a capacity
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + its build holds no liquid
 */
forward bool:IsItemLiquidContainer(Item:itemid);

/**
 * @brief      Whether a vessel is worth keeping once it is empty.
 *
 * A jerrycan is: it is a thing that holds petrol whether or not there is
 * any in it today. A tin of cola is not, and goes in the bin with the last
 * mouthful.
 *
 * What the answer decides is whether emptying one destroys it. A kept one
 * is the character's to be rid of, through the DISCARD row.
 *
 * @param      itemid  Vessel to ask.
 *
 * @date       12:40 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it survives being emptied
 *             - `false` it is thrown away with the last of what was in it
 */
forward bool:IsItemLiquidContainerReusable(Item:itemid);

/**
 * @brief      Say that a kind of vessel turns up holding a liquid.
 *
 * One call per liquid. A vessel told nothing turns up empty, which is what a
 * jerrycan found in a shed usually is; one told several is filled with one of
 * them, chosen when it is made.
 *
 * @param      buildid  Vessel build.
 * @param      liquid   A liquid it may be found holding.
 *
 * @date       19:40 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it may now turn up holding that
 *             - `false` when:
 *                 + the build holds no liquid
 *                 + `liquid` is not registered
 */
forward bool:DefineItemBuildLiquidHolds(ItemBuild:buildid, Liquid:liquid);

/**
 * @brief      How many litres this kind of vessel takes.
 *
 * @param      buildid  Build to read.
 *
 * @date       19:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - the capacity in litres
 *             - `0.0` when the build holds no liquid
 */
forward GetItemBuildLiquidCapacity(ItemBuild:buildid);

/**
 * @brief      Put liquid into a vessel, replacing whatever was in it.
 *
 * Saying nothing means to the brim, which is what filling something usually
 * means and saves every caller repeating the capacity back.
 *
 * Filling a can of cola with water makes it a can of water. Mixing is not a
 * thing that happens: there is one liquid in a vessel and pouring in another
 * would need a rule about what the two make, which nothing here has.
 *
 * The item renames itself after what is in it, the way a package of clothes
 * does, so a slot shows "Can of Drink (Water)" without the menu knowing that
 * liquids exist.
 *
 * @param      itemid  Vessel to fill.
 * @param      liquid  What to put in it.
 * @param      millilitres  How much, capped at the build's capacity.
 *
 * @date       19:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is filled
 *             - `false` when:
 *                 + the item is not a vessel
 *                 + `liquid` is not a registered liquid
 *                 + `millilitres` is zero or below
 */
forward bool:SetItemLiquid(Item:itemid, Liquid:liquid, millilitres = -1);

/**
 * @brief      What is in a vessel.
 *
 * @param      itemid  Vessel to read.
 *
 * @date       19:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - the `Liquid:` in it
 *             - `INVALID_LIQUID_ID` when it is empty or not a vessel
 */
forward Liquid:GetItemLiquid(Item:itemid);

/**
 * @brief      How much is left in a vessel.
 *
 * @param      itemid  Vessel to read.
 *
 * @date       19:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - the millilitres left
 *             - `0.0` when it is empty or not a vessel
 */
forward GetItemLiquidAmount(Item:itemid);

/**
 * @brief      Take liquid out of a vessel, down to the last drop.
 *
 * The other direction from `SetItemLiquid`, which is for filling and
 * refuses to be told nothing -- draining a can to exactly zero through it
 * left the can full, which is a can that can be poured out forever.
 *
 * What was in it is remembered: an emptied jerrycan is an empty jerrycan of
 * gasoline rather than an empty jerrycan of nothing.
 *
 * @param      itemid       Vessel to draw from.
 * @param      millilitres  How much to take.
 *
 * @date       11:20 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - how much actually came out, which is all of it or less
 *             - `0` when the item is not a vessel, or is already empty
 */
forward DrawItemLiquid(Item:itemid, millilitres);

/**
 * @brief      Pour one vessel into another.
 *
 * As much goes across as there is room for, and what will not fit stays
 * where it is -- so a litre poured into a can with room for half a litre
 * leaves half a litre behind rather than refusing.
 *
 * Mixing is not a thing that happens: the two have to hold the same liquid,
 * or the one being filled has to be empty, in which case it becomes a
 * vessel of whatever went in. That is the same rule `SetItemLiquid` keeps.
 *
 * An emptied vessel is the caller's to throw away. This knows how much
 * moved, not whether anybody wants the can back.
 *
 * @param      intoid  Vessel being filled.
 * @param      fromid  Vessel being poured out.
 * @param      limit   Most to pour, in millilitres. `0` for all of it.
 *
 * @date       11:20 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many millilitres went across
 *             - `0` when:
 *                 + either is not a vessel, or they are the same one
 *                 + the one being poured is empty
 *                 + the one being filled is full, or holds something else
 */
forward PourItemLiquid(Item:intoid, Item:fromid, limit = 0);

/**
 * @brief      Start drinking, if the character is holding it.
 *
 * The bar is what is left in mouthfuls, capped so that a jerrycan is a
 * series of holds rather than one enormous one. Each swallow starts its own
 * animation, but only while the hold is still running: the mark that ends
 * it is counted from the finish, where the character is putting the tin
 * away rather than raising it again.
 *
 * Two mouthfuls of something that harms and the body stops asking. The
 * vessel goes away first -- a character on their knees is not holding a
 * jerrycan, and what is left in it is still in it.
 *
 * The same rule food has, for the same reason: what is in the hands is what
 * goes in the mouth, and hands full of a crate are hands that cannot.
 *
 * @param      playerid  Character drinking.
 * @param      itemid    Vessel in their hands.
 *
 * @date       19:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they have started on it
 *             - `false` when:
 *                 + the item is not a vessel, or is empty
 *                 + it is not the item in their hands
 *                 + they are busy with something that outranks drinking
 */
forward bool:DrinkItem(playerid, Item:itemid);

/**
 * # Events
 */

/**
 * @brief      A character has taken a mouthful of something.
 *
 * The tick that would have reported the last mark is the tick that ended
 * the action instead, so the final swallow is reported when the bar
 * finishes. The vessel is emptied only if it actually ran dry -- a capped
 * hold on something large leaves it part full, and the character may hold
 * the key again.
 *
 * @param      playerid   Character who drank.
 * @param      itemid     What they drank from, gone if it was the last of it
 *                        and the vessel was not worth keeping.
 * @param      liquid     What they drank.
 * @param      nutrition  What that mouthful was worth, negative for what harms.
 * @param      remaining  Litres left after it.
 *
 * @date       19:00 08/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerDrink(playerid, Item:itemid, Liquid:liquid, Float:nutrition, Float:remaining);

/**
 * # External
 */

// Litres are what the amount system stores and draws -- a volume reads as "1.5L"
// and nothing shows it in thousandths. Millilitres are what everything asking a
// question about it uses, because a weapon's tank is measured in them and a can
// pouring into a tank has to be measured in the same thing.
//
// So the storage stays litres and the whole of the conversion is here, rather
// than every caller multiplying by a thousand and rounding it its own way.
static stock MillilitresInternal(Float:litres) {
    return floatround(litres * 1000.0);
}

stock bool:DefineItemBuildLiquidContainer(ItemBuild:buildid, capacity, bool:reusable = false) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    if (capacity <= 0) {
        return false;
    }

    if (GetItemBuildContainerSize(buildid) != 0) {
        printf("[liquid] %i already holds items, so it cannot hold liquid", _:buildid);

        return false;
    }

    gItemBuildLiquidCapacity[buildid] = float(capacity) / 1000.0;
    gItemBuildLiquidReusable[buildid] = reusable;

    SetItemBuildAmountType(buildid, ITEM_AMOUNT_VOLUME);

    return true;
}

stock bool:DefineItemBuildLiquidHolds(ItemBuild:buildid, Liquid:liquid) {
    if (gItemBuildLiquidCapacity[buildid] <= 0.0) {
        return false;
    }

    if (!IsValidLiquid(liquid)) {
        return false;
    }

    gItemBuildLiquidHolds[buildid] |= (1 << _:liquid);

    return true;
}

stock bool:IsItemLiquidContainer(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    return (gItemBuildLiquidCapacity[GetItemBuild(itemid)] > 0.0);
}

stock bool:IsItemLiquidContainerReusable(Item:itemid) {
    if (!IsItemLiquidContainer(itemid)) {
        return false;
    }

    return gItemBuildLiquidReusable[GetItemBuild(itemid)];
}

stock GetItemBuildLiquidCapacity(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return 0;
    }

    return MillilitresInternal(gItemBuildLiquidCapacity[buildid]);
}

stock Liquid:GetItemLiquid(Item:itemid) {
    if (!IsItemLiquidContainer(itemid)) {
        return INVALID_LIQUID_ID;
    }

    if (!HasItemExtraData(itemid, ITEM_LIQUID_KEY)) {
        return INVALID_LIQUID_ID;
    }

    return Liquid:GetItemExtraDataInt(itemid, ITEM_LIQUID_KEY);
}

stock GetItemLiquidAmount(Item:itemid) {
    if (!IsItemLiquidContainer(itemid)) {
        return 0;
    }

    return MillilitresInternal(GetItemAmountFloat(itemid));
}

stock bool:SetItemLiquid(Item:itemid, Liquid:liquid, millilitres = -1) {
    if (!IsItemLiquidContainer(itemid)) {
        return false;
    }

    if (!IsValidLiquid(liquid)) {
        return false;
    }

    new const
        capacity = MillilitresInternal(gItemBuildLiquidCapacity[GetItemBuild(itemid)])
    ;

    if (millilitres < 0) {
        millilitres = capacity;
    }

    if (millilitres <= 0) {
        return false;
    }

    if (millilitres > capacity) {
        millilitres = capacity;
    }

    SetItemExtraData(itemid, ITEM_LIQUID_KEY, var_new(_:liquid));
    SetItemAmountFloat(itemid, float(millilitres) / 1000.0);

    new
        name[MAX_LIQUID_NAME]
    ;

    GetLiquidName(liquid, name);

    return SetItemExtraName(itemid, name);
}

stock DrawItemLiquid(Item:itemid, millilitres) {
    if (!IsItemLiquidContainer(itemid)) {
        return 0;
    }

    if (millilitres <= 0) {
        return 0;
    }

    new const
        held = GetItemLiquidAmount(itemid)
    ;

    new
        drawn = millilitres
    ;

    if (drawn > held) {
        drawn = held;
    }

    if (drawn <= 0) {
        return 0;
    }

    // The amount and not SetItemLiquid, which is the filling call and will not
    // be told nothing. What is in it is left alone on the way down: a can with
    // the last drop gone is still a can that had gasoline in it.
    SetItemAmountFloat(itemid, float(held - drawn) / 1000.0);

    return drawn;
}

stock PourItemLiquid(Item:intoid, Item:fromid, limit = 0) {
    if (intoid == fromid) {
        return 0;
    }

    if (!IsItemLiquidContainer(intoid)) {
        return 0;
    }

    if (!IsItemLiquidContainer(fromid)) {
        return 0;
    }

    new const
        Liquid:liquid = GetItemLiquid(fromid)
    ;

    if (!IsValidLiquid(liquid)) {
        return 0;
    }

    new const
        held = GetItemLiquidAmount(intoid)
    ;

    // Mixing is not a thing that happens, so the two have to agree on what they
    // are holding. One with nothing in it agrees with anything, and becomes a
    // vessel of whatever was poured into it.
    if (held > 0 && GetItemLiquid(intoid) != liquid) {
        return 0;
    }

    new
        room = GetItemBuildLiquidCapacity(GetItemBuild(intoid)) - held
    ;

    if (limit > 0 && room > limit) {
        room = limit;
    }

    if (room <= 0) {
        return 0;
    }

    new const
        poured = DrawItemLiquid(fromid, room)
    ;

    if (poured == 0) {
        return 0;
    }

    SetItemLiquid(intoid, liquid, held + poured);

    return poured;
}

stock bool:DrinkItem(playerid, Item:itemid) {
    if (!IsItemLiquidContainer(itemid)) {
        return false;
    }

    if (GetPlayerHoldItem(playerid) != itemid) {
        return false;
    }

    new const
        millilitres = GetItemLiquidAmount(itemid)
    ;

    if (millilitres <= 0) {
        return false;
    }

    if (!IsValidLiquid(GetItemLiquid(itemid))) {
        return false;
    }

    new
        sips = (millilitres + ITEM_SIP_MILLILITRES - 1) / ITEM_SIP_MILLILITRES
    ;

    if (sips > ITEM_MAX_SIPS_PER_HOLD) {
        sips = ITEM_MAX_SIPS_PER_HOLD;
    }

    if (sips <= 0) {
        return false;
    }

    if (!StartHoldAction(playerid, sips * ITEM_DRINK_TIME, "Drinking", ITEM_KEY_USE_ITEM, HOLD_PRIORITY_DRINK)) {
        return false;
    }

    gPlayerDrinking[playerid] = itemid;
    gPlayerSipsTaken[playerid] = 0;

    ApplyAnimation(playerid, "VENDING", "VEND_DRINK2_P", ITEM_DRINK_ANIMATION_SPEED, false, false, false, false, 0);

    return true;
}

/**
 * # Internal
 */

// Every mouthful the bar has gone past since the last look, so a tick that
// arrives late covers what it skipped rather than losing it.
static stock TakeSipsInternal(playerid, Item:itemid, upTo) {
    new const
        Liquid:liquid = GetItemLiquid(itemid)
    ;

    while (gPlayerSipsTaken[playerid] < upTo) {
        ++gPlayerSipsTaken[playerid];

        new
            remaining = GetItemLiquidAmount(itemid) - ITEM_SIP_MILLILITRES
        ;

        if (remaining < 0) {
            remaining = 0;
        }

        SetItemAmountFloat(itemid, float(remaining) / 1000.0);

        CallLocalFunction("OnPlayerDrink", "iiiff", playerid, _:itemid, _:liquid, _:GetLiquidNutrition(liquid), _:remaining);

        if (GetLiquidNutrition(liquid) < 0.0 && gPlayerSipsTaken[playerid] >= ITEM_VOMIT_SIPS) {
            new
                name[MAX_ITEM_NAME]
            ;

            GetItemName(itemid, name);

            StopHoldAction(playerid);
            PutAwayHeldItemInternal(playerid, itemid);
            VomitPlayer(playerid);

            SendPlayerNotice(playerid, "The %s comes straight back up.", name);

            return;
        }

        if (remaining > 0.0 && IsPlayerHoldingAction(playerid)) {
            ApplyAnimation(playerid, "VENDING", "VEND_DRINK2_P", ITEM_DRINK_ANIMATION_SPEED, false, false, false, false, 0);
        }
    }
}

static stock PutAwayHeldItemInternal(playerid, Item:itemid) {
    if (GetPlayerHoldItem(playerid) != itemid) {
        return;
    }

    new
        index = -1
    ;

    if (!AddItemToInventory(playerid, itemid, index)) {
        return;
    }

    RemoveCurrentItem(playerid);
}

static stock EmptyItemInternal(playerid, Item:itemid) {
    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (gItemBuildLiquidReusable[GetItemBuild(itemid)]) {
        SetItemExtraName(itemid, "Empty");

        SendPlayerNotice(playerid, "You empty the %s.", name);

        return;
    }

    RemoveCurrentItem(playerid);
    DestroyItem(itemid);

    SendPlayerNotice(playerid, "You finish the %s and throw it away.", name);
}

/**
 * # Calls
 */

// Filled when it is made rather than when it is found, so two cans off the same
// shelf are not the same can and nothing has to remember to fill one.
hook OnItemCreate(Item:itemid) {
    new const
        ItemBuild:buildid = GetItemBuild(itemid),
        holds = gItemBuildLiquidHolds[buildid]
    ;

    if (holds == 0) {
        return 0;
    }

    new
        Liquid:chosen = INVALID_LIQUID_ID,
        seen
    ;

    for (new Liquid:liquid; liquid != GetLiquidCount(); ++liquid) {
        if (!(holds & (1 << _:liquid))) {
            continue;
        }

        if (random(++seen) == 0) {
            chosen = liquid;
        }
    }

    if (IsValidLiquid(chosen)) {
        SetItemLiquid(itemid, chosen);
    }

    return 0;
}

hook OnPlayerUseItem(playerid, Item:itemid) {
    if (!IsItemLiquidContainer(itemid)) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (GetItemLiquidAmount(itemid) <= 0) {
        SendPlayerNotice(playerid, "The %s is empty.", name);

        return 1;
    }

    if (!DrinkItem(playerid, itemid)) {
        new
            busy[MAX_HOLD_ACTION_TITLE]
        ;

        if (GetPlayerHoldActionTitle(playerid, busy)) {
            SendPlayerNotice(playerid, "You are busy: %s.", busy);
        }
    }

    return 1;
}

hook OnHoldActionUpdate(playerid, progress) {
    new const
        Item:itemid = gPlayerDrinking[playerid]
    ;

    if (!IsValidItem(itemid)) {
        return 0;
    }

    TakeSipsInternal(playerid, itemid, progress / ITEM_DRINK_TIME);

    return 0;
}

hook OnHoldActionFinish(playerid) {
    new const
        Item:itemid = gPlayerDrinking[playerid]
    ;

    if (!IsValidItem(itemid)) {
        return 0;
    }

    gPlayerDrinking[playerid] = INVALID_ITEM_ID;

    TakeSipsInternal(playerid, itemid, gPlayerSipsTaken[playerid] + 1);

    ClearAnimations(playerid);

    if (GetItemLiquidAmount(itemid) > 0) {
        return 1;
    }

    EmptyItemInternal(playerid, itemid);

    return 1;
}

hook OnHoldActionStop(playerid, progress) {
    if (!IsValidItem(gPlayerDrinking[playerid])) {
        return 0;
    }

    gPlayerDrinking[playerid] = INVALID_ITEM_ID;

    ClearAnimations(playerid);

    return 0;
}

hook OnPlayerDropItem(playerid, Item:itemid) {
    if (gPlayerDrinking[playerid] == itemid) {
        StopHoldAction(playerid);
    }

    return 0;
}

hook OnItemDestroy(Item:itemid) {
    foreach (new i : Player) {
        if (gPlayerDrinking[i] == itemid) {
            gPlayerDrinking[i] = INVALID_ITEM_ID;
        }
    }

    return 0;
}

hook OnPlayerConnect(playerid) {
    gPlayerDrinking[playerid] = INVALID_ITEM_ID;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    gPlayerDrinking[playerid] = INVALID_ITEM_ID;

    return 0;
}
