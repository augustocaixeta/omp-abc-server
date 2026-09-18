#if defined _CORE_ITEM_POUR
    #endinput
#endif
#define _CORE_ITEM_POUR

#include <pp-hooks>

/**
 * # Standing there emptying a can into something
 *
 * A can in the hands, a key held down, and a bar that fills as the thing on the
 * other end of it does. Letting go stops it where it is: what has gone in has
 * gone in, and a tank half filled is a tank half filled rather than a job that
 * has to be started again.
 *
 * What it is being poured into is not this file's business. It offers a tick's
 * worth to whoever is listening and writes off however much they say they took,
 * so a fuel tank, a generator or a barrel are all the same act -- one gesture,
 * one bar, and one place that knows how fast a can empties.
 *
 * # Nothing listens yet
 *
 * This is here for the vehicle work and has no caller until then. Weapons are
 * filled in the pockets instead, by dropping a can onto one the way a box of
 * rounds is dropped onto a rifle -- somebody carrying ten chainsaws cannot say
 * which one they mean by standing still, and a slot says it exactly.
 *
 * What survived from that is everything below: the rate, the bar that reads the
 * tank rather than the clock, and the carry that stops a can losing a drop it
 * never poured.
 */

// How much goes in on one tick. Pouring is continuous: what has gone in has
// gone in, and letting go of the key stops it rather than undoing it.
#define ITEM_POUR_STEP (ITEM_REFUEL_RATE * HOLD_ACTION_STEP / 1000)

static
    Item:gPlayerPouring[MAX_PLAYERS] = { INVALID_ITEM_ID, ... },

    // How full the thing being filled is, in the same units the can is measured
    // in. Kept here rather than asked for every tick, because the bar reads it
    // and whoever is being poured into may not have a number to give back.
    gPlayerPourFilled[MAX_PLAYERS],
    gPlayerPourCapacity[MAX_PLAYERS],
    gPlayerPourTotal[MAX_PLAYERS]
;

/**
 * # Functions
 */

/**
 * @brief      Start pouring a can into something, a tick at a time.
 *
 * The bar fills rather than drains, because this is work being done and not
 * a tank going down, and it is a gauge rather than a clock: something four
 * fifths full opens four fifths along and climbs the last fifth.
 *
 * Every tick offers `OnPlayerPourLiquid` a measure and takes out of the can
 * exactly what it says went in, so a tank that fills up, a can that runs
 * out and a character who lets go all end it the same way.
 *
 * @param      playerid     Character pouring.
 * @param      containerid  Can in their hands.
 * @param      capacity     How much the thing being filled holds.
 * @param      filled       How much is in it already.
 * @param      title        Shown beside the bar. Empty draws no title.
 * @param      priority     Which action wins the bar. See settings.
 *
 * @date       09:10 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they are pouring
 *             - `false` when:
 *                 + it is not a live liquid container, or is empty
 *                 + the thing being filled holds nothing, or is full
 *                 + something they are already doing outranks it
 */
forward bool:StartPlayerPour(playerid, Item:containerid, capacity, filled = 0, const title[] = "", priority = HOLD_PRIORITY_REFUEL);

/**
 * @brief      Stop pouring, keeping what has gone in.
 *
 * @param      playerid  Character to stop.
 *
 * @date       09:10 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they were pouring and are not any more
 *             - `false` they were not pouring
 */
forward bool:StopPlayerPour(playerid);

/**
 * @brief      The can a character is pouring, if any.
 *
 * @param      playerid  Character to read.
 *
 * @date       09:10 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - the can in their hands
 *             - `INVALID_ITEM_ID` they are not pouring
 */
forward Item:GetPlayerPourContainer(playerid);

/**
 * @brief      How full the thing being poured into is.
 *
 * @param      playerid  Character to read.
 *
 * @date       09:10 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - what is in it, in the units the can is measured in
 *             - `0` they are not pouring
 */
forward GetPlayerPourFilled(playerid);

/**
 * # Callbacks
 */

/**
 * @brief      A tick's worth is being offered. Say how much of it went in.
 *
 * Answering with less than was offered fills the rest of the tick with
 * nothing; answering with nothing ends the pour, which is how a tank says
 * it is full. Exactly what is answered comes out of the can.
 *
 * @param      playerid     Character pouring.
 * @param      containerid  Can it is coming out of.
 * @param      amount       Most that can go in this tick.
 *
 * @return     how much actually went in
 */
forward OnPlayerPourLiquid(playerid, Item:containerid, amount);

/**
 * @brief      The pour is over, however it ended.
 *
 * The can may already have been destroyed by then, which is what happens
 * to one that has been emptied -- `containerid` says which it was rather
 * than promising it still exists.
 *
 * @param      playerid     Character who was pouring.
 * @param      containerid  Can it came out of.
 * @param      poured       How much went in, all told.
 */
forward OnPlayerStopPour(playerid, Item:containerid, poured);

/**
 * # Internal
 */

static stock ClearPourInternal(playerid) {
    gPlayerPouring[playerid] = INVALID_ITEM_ID;
    gPlayerPourFilled[playerid] = 0;
    gPlayerPourCapacity[playerid] = 0;
    gPlayerPourTotal[playerid] = 0;
}

// The end of it, however it ended: filled up, run out, or let go of. What went
// in is already in -- this is only the tidying up after it.
static stock EndPourInternal(playerid) {
    new const
        Item:containerid = gPlayerPouring[playerid],
        poured = gPlayerPourTotal[playerid]
    ;

    if (containerid == INVALID_ITEM_ID) {
        return;
    }

    ClearPourInternal(playerid);
    StopHoldAction(playerid);

    if (IsValidItem(containerid) && GetItemLiquidAmount(containerid) <= 0) {
        new
            name[MAX_ITEM_NAME]
        ;

        GetItemName(containerid, name);

        // A vessel worth keeping is kept, and the build is what says so. What
        // is left of a jerrycan poured out is a jerrycan, and being rid of it
        // is the character's to decide on the DISCARD row.
        if (IsItemLiquidContainerReusable(containerid)) {
            SendPlayerNotice(playerid, "You empty the %s.", name);
        } else {
            DestroyItem(containerid);
            
            SendPlayerNotice(playerid, "You empty the %s and throw it away.", name);
        }
    }

    CallLocalFunction("OnPlayerStopPour", "iii", playerid, _:containerid, poured);
}

/**
 * # External
 */

stock Item:GetPlayerPourContainer(playerid) {
    return gPlayerPouring[playerid];
}

stock GetPlayerPourFilled(playerid) {
    return gPlayerPourFilled[playerid];
}

stock bool:StopPlayerPour(playerid) {
    if (gPlayerPouring[playerid] == INVALID_ITEM_ID) {
        return false;
    }

    EndPourInternal(playerid);

    return true;
}

stock bool:StartPlayerPour(playerid, Item:containerid, capacity, filled = 0, const title[] = "", priority = HOLD_PRIORITY_REFUEL) {
    if (!IsItemLiquidContainer(containerid)) {
        return false;
    }

    if (GetItemLiquidAmount(containerid) <= 0) {
        return false;
    }

    if (capacity <= 0) {
        return false;
    }

    if (filled < 0) {
        filled = 0;
    }

    if (filled >= capacity) {
        return false;
    }

    if (!StartHoldAction(playerid, capacity, title, ITEM_KEY_USE_ITEM, priority, false, true)) {
        return false;
    }

    gPlayerPouring[playerid] = containerid;
    gPlayerPourFilled[playerid] = filled;
    gPlayerPourCapacity[playerid] = capacity;
    gPlayerPourTotal[playerid] = 0;

    SetHoldActionProgress(playerid, filled);

    return true;
}

/**
 * # Calls
 */

hook OnHoldActionUpdate(playerid, progress) {
    new const
        Item:containerid = gPlayerPouring[playerid]
    ;

    if (containerid == INVALID_ITEM_ID) {
        return 0;
    }

    // Drunk, dropped or destroyed with the key still down.
    if (!IsValidItem(containerid)) {
        ClearPourInternal(playerid);
        StopHoldAction(playerid);

        return 0;
    }

    new const
        available = GetItemLiquidAmount(containerid),
        room = gPlayerPourCapacity[playerid] - gPlayerPourFilled[playerid]
    ;

    new
        offered = ITEM_POUR_STEP
    ;

    if (offered > available) {
        offered = available;
    }

    if (offered > room) {
        offered = room;
    }

    // Nothing more will go in: it is full or the can is dry, and either way
    // there is no reason to stand there.
    if (offered <= 0) {
        EndPourInternal(playerid);

        return 0;
    }

    new const
        taken = CallLocalFunction("OnPlayerPourLiquid", "iii", playerid, _:containerid, offered)
    ;

    if (taken <= 0) {
        EndPourInternal(playerid);

        return 0;
    }

    gPlayerPourFilled[playerid] += taken;
    gPlayerPourTotal[playerid] += taken;

    DrawItemLiquid(containerid, taken);
    SetHoldActionProgress(playerid, gPlayerPourFilled[playerid]);

    return 0;
}

hook OnHoldActionFinish(playerid) {
    EndPourInternal(playerid);

    return 0;
}

hook OnHoldActionStop(playerid, progress) {
    EndPourInternal(playerid);

    return 0;
}

hook OnPlayerConnect(playerid) {
    ClearPourInternal(playerid);

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    ClearPourInternal(playerid);

    return 0;
}
