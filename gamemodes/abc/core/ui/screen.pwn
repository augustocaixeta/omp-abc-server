#if defined _CORE_UI_SCREEN
    #endinput
#endif
#define _CORE_UI_SCREEN

#include <pp-hooks>

/**
 * # What is covering the screen
 *
 * The pockets are drawn over most of it, and anything else the gamemode draws
 * -- a bar, a counter, a compass -- is drawn somewhere the pockets might land
 * on top of. Something has to decide who gets out of the way, and this is it.
 *
 * Layering cannot do it. Global textdraws are drawn in the order they were
 * created and player textdraws are drawn over all of them, so where two things
 * land depends on which file was included first -- an ordering nobody sets on
 * purpose and that changes the moment a include moves. Hiding is a decision;
 * z-order here is an accident that happens to work until it does not.
 *
 * # Why a module and not a check
 *
 * Because a check is written once per element and then forgotten by the next
 * one. There is one bar today; a gamemode this size ends up with half a dozen
 * things on the edges of the screen, and every one of them would have to know
 * about the pockets, the crate beside them, and whatever covers the screen next.
 *
 * So covering is a set of reasons rather than a flag. Each source owns a bit and
 * sets it without caring who else has one -- setting a bit that is already set
 * or clearing one that is not changes nothing, which is what makes it safe to
 * call from both ends of a menu that can also be closed with escape.
 * OnPlayerCoverScreen fires only when the answer as a whole changes.
 */

// One bit per reason the screen is not free. The menus are the only one today.
#define SCREEN_COVER_MENU (1)

static
    gPlayerScreenCover[MAX_PLAYERS]
;

/**
 * # Functions
 */

/**
 * @brief      Say that something is or is not covering the screen.
 *
 * Idempotent on purpose: a reason that is already set can be set again and
 * one that was never set can be cleared, and neither is a mistake. That is
 * what lets a menu claim the screen when it opens and release it from both
 * the close it was asked for and the escape it was not.
 *
 * @param      playerid  Character whose screen it is.
 * @param      cover     Which reason, one of the `SCREEN_COVER_*` bits.
 * @param      covered   `true` it is covering, `false` it has gone.
 *
 * @date       15:10 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the screen as a whole changed, and everything drawing
 *               on it has been told
 *             - `false` nothing changed: no bits given, or some other reason
 *               is still covering it
 */
forward bool:SetPlayerScreenCover(playerid, cover, bool:covered);

/**
 * @brief      Whether anything is covering the screen.
 *
 * @param      playerid  Character to ask.
 *
 * @date       15:10 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` something is drawn over it
 *             - `false` it is free
 */
forward bool:IsPlayerScreenCovered(playerid);

/**
 * # Callbacks
 */

/**
 * @brief      The screen has been covered, or is free again.
 *
 * Fired on the change and not on every claim, so a listener may take this
 * as "hide" and "show" without counting anything itself.
 *
 * The hold bar is already handled below and does not need a listener. This
 * is for whatever is drawn next.
 *
 * @param      playerid  Character whose screen it is.
 * @param      covered   `true` get out of the way, `false` come back.
 */
forward OnPlayerCoverScreen(playerid, bool:covered);

/**
 * # Internal
 */

static stock ApplyScreenCoverInternal(playerid) {
    new const
        bool:covered = gPlayerScreenCover[playerid] != 0
    ;

    // Done here rather than through the callback because the bar is not the
    // gamemode's to hook -- it belongs to hold-action, which knows nothing
    // about menus and should not have to.
    SetHoldActionCovered(playerid, covered);

    CallLocalFunction("OnPlayerCoverScreen", "ii", playerid, covered);
}

/**
 * # External
 */

stock bool:IsPlayerScreenCovered(playerid) {
    return gPlayerScreenCover[playerid] != 0;
}

stock bool:SetPlayerScreenCover(playerid, cover, bool:covered) {
    if (cover == 0) {
        return false;
    }

    new const
        before = gPlayerScreenCover[playerid]
    ;

    if (covered) {
        gPlayerScreenCover[playerid] |= cover;
    } else {
        gPlayerScreenCover[playerid] &= ~cover;
    }

    // Still covered, or still free. One reason going while another stays is
    // not a change to anybody drawing on it.
    if ((before != 0) == (gPlayerScreenCover[playerid] != 0)) {
        return false;
    }

    ApplyScreenCoverInternal(playerid);

    return true;
}

/**
 * # Calls
 */

hook OnPlayerUseContainerMenu(playerid, bool:using) {
    SetPlayerScreenCover(playerid, SCREEN_COVER_MENU, using);

    return 0;
}

hook OnPlayerConnect(playerid) {
    gPlayerScreenCover[playerid] = 0;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    gPlayerScreenCover[playerid] = 0;

    return 0;
}
