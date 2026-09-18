#if defined _CORE_PLAYER_VOMIT
    #endinput
#endif
#define _CORE_PLAYER_VOMIT

#include <pp-hooks>

/**
 * # Being sick
 *
 * A character on their knees for a few seconds, frozen while it lasts. It is
 * the body answering for something that was swallowed, so it lives here rather
 * than with whatever was swallowed -- petrol out of a jerrycan and one burger
 * too many are the same thing happening for different reasons, and neither of
 * them owns it.
 *
 * Not a hold action. A hold is something a character is choosing to do and may
 * stop doing; a bar here would be offering a decision that has already been
 * made. It is a timer and a frozen player, and nothing about it is optional.
 *
 * The reasons are elsewhere. core/item-build/liquid-container calls this after
 * two mouthfuls of something worth less than nothing, and core/item/food will
 * call it when there is a stomach to fill -- which is the one thing missing for
 * the story mode version of this, where the character eats until they cannot.
 */

static
    gPlayerVomitTimer[MAX_PLAYERS] = { INVALID_TIMER, ... }
;

/**
 * # Functions
 */

/**
 * @brief      Bring a character to their knees.
 *
 * They are frozen for ITEM_VOMIT_TIME and handed back afterwards. Calling it
 * again while it is running restarts it rather than stacking, the first timer
 * being killed before the second is set.
 *
 * @param      playerid  Character to turn inside out.
 *
 * @date       21:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they are down
 *             - `false` they are not connected
 */
forward bool:VomitPlayer(playerid);

/**
 * @brief      Whether a character is in the middle of being sick.
 *
 * Worth asking before starting anything they would have to be standing for.
 *
 * @param      playerid  Character to check.
 *
 * @date       21:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` they are on their knees
 *             - `false` they are not
 */
forward bool:IsPlayerVomiting(playerid);

/**
 * # Internal
 */

forward @Player_VomitEnd(playerid);

static stock ClearVomitTimerInternal(playerid) {
    if (gPlayerVomitTimer[playerid] != INVALID_TIMER) {
        KillTimer(gPlayerVomitTimer[playerid]);
    }

    gPlayerVomitTimer[playerid] = INVALID_TIMER;
}

/**
 * # External
 */

stock bool:VomitPlayer(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    ClearVomitTimerInternal(playerid);

    ClearAnimations(playerid);
    SetCameraBehindPlayer(playerid);
    TogglePlayerControllable(playerid, false);

    ApplyAnimation(playerid, "FOOD", "EAT_VOMIT_P", ITEM_EAT_ANIMATION_SPEED, false, false, false, false, 0);

    gPlayerVomitTimer[playerid] = SetTimerEx("@Player_VomitEnd", ITEM_VOMIT_TIME, false, "i", playerid);

    return true;
}

stock bool:IsPlayerVomiting(playerid) {
    return (gPlayerVomitTimer[playerid] != INVALID_TIMER);
}

/**
 * # Calls
 */

public @Player_VomitEnd(playerid) {
    gPlayerVomitTimer[playerid] = INVALID_TIMER;

    if (!IsPlayerConnected(playerid)) {
        return;
    }

    ClearAnimations(playerid);
    TogglePlayerControllable(playerid, true);
}

hook OnPlayerConnect(playerid) {
    ClearVomitTimerInternal(playerid);

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    ClearVomitTimerInternal(playerid);

    return 0;
}
