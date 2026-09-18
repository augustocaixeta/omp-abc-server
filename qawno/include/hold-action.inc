#if defined _INC_HOLD_ACTION
    #endinput
#endif
#define _INC_HOLD_ACTION

#include <open.mp>

#define PROGRESS2_NO_YSI
#include <progress2>

#include <pp-hooks>

/**
 * # Holding a key to do something
 *
 * A bar that fills while a key is held down and empties the moment it is not.
 * Changing clothes, prying a door, emptying a crate: things that should take a
 * moment, be visible while they take it, and be abandonable by letting go.
 *
 * The key is checked on every tick rather than watched for a release. A release
 * that never arrives -- a player who alt-tabs, a callback something else
 * swallowed -- would otherwise leave the bar filling on its own, which is a hold
 * that does not need holding.
 *
 * One bar, one action. Anything asking for it while it is in use is refused and
 * can say what the character is already doing, which is also how a gamemode
 * stops somebody changing clothes in the middle of picking a lock.
 *
 * From Southclaws' hold-action, rewritten: the original keeps one shared bar
 * variable assigned per connect, watches for the key release instead of checking
 * the key, and has no title, owner or priority.
 */

#if !defined HOLD_ACTION_STEP
    #define HOLD_ACTION_STEP (100)
#endif

#if !defined HOLD_ACTION_BAR_X
    #define HOLD_ACTION_BAR_X (291.0)
#endif

#if !defined HOLD_ACTION_BAR_Y
    #define HOLD_ACTION_BAR_Y (345.0)
#endif

#if !defined HOLD_ACTION_BAR_WIDTH
    #define HOLD_ACTION_BAR_WIDTH (57.5)
#endif

#if !defined HOLD_ACTION_BAR_HEIGHT
    #define HOLD_ACTION_BAR_HEIGHT (5.19)
#endif

#if !defined HOLD_ACTION_BAR_COLOUR
    #define HOLD_ACTION_BAR_COLOUR (0xC8C8C8FF)
#endif

// The gap between the frame and the fill inside it. progress2's own defaults,
// repeated here so a thin bar can have them: the game draws a textdraw box
// four pixels taller than its letter size asks for, so the fill cannot be
// thinner than four and a bar of eight with a gap of one either side is
// already close to as thin as one goes. Below that the fill stops shrinking,
// keeps its four, and hangs out of the frame.
#if !defined HOLD_ACTION_BAR_PADDING_X
    #define HOLD_ACTION_BAR_PADDING_X (1.2)
#endif

#if !defined HOLD_ACTION_BAR_PADDING_Y
    #define HOLD_ACTION_BAR_PADDING_Y (1.0)
#endif

#if !defined MAX_HOLD_ACTION_TITLE
    #define MAX_HOLD_ACTION_TITLE (32)
#endif

// Off the left of the bar by default, so that moving the bar alone still takes
// the title with it. A gamemode that has both out of the editor sets both.
#if !defined HOLD_ACTION_TITLE_X
    #define HOLD_ACTION_TITLE_X (HOLD_ACTION_BAR_X - 4.0)
#endif

#if !defined HOLD_ACTION_TITLE_Y
    #define HOLD_ACTION_TITLE_Y (HOLD_ACTION_BAR_Y - 5.0)
#endif

#if !defined HOLD_ACTION_TITLE_LETTER_X
    #define HOLD_ACTION_TITLE_LETTER_X (0.170)
#endif

#if !defined HOLD_ACTION_TITLE_LETTER_Y
    #define HOLD_ACTION_TITLE_LETTER_Y (0.900)
#endif

#if !defined HOLD_ACTION_TITLE_COLOUR
    #define HOLD_ACTION_TITLE_COLOUR (0xFFFFFFFF)
#endif

static enum _:E_HOLD_ACTION_DATA {
    bool:E_HOLD_ACTION_RUNNING,

    // Whether the action wants the bar on the screen. Its own say, and not the
    // last word -- see E_HOLD_ACTION_COVERED's other half below.
    bool:E_HOLD_ACTION_SHOWN,

    // Whether the bar reads as what is left rather than as what is done. The
    // action runs exactly the same either way -- this is only which end of it
    // the player is being shown.
    bool:E_HOLD_ACTION_DRAINING,

    // Whether the bar is measuring something other than time. The tick still
    // watches the key and still redraws, but it stops counting: what the bar
    // reads is the caller's to say, through SetHoldActionProgress.
    bool:E_HOLD_ACTION_GAUGE,

    E_HOLD_ACTION_LIMIT,
    E_HOLD_ACTION_PROGRESS,
    E_HOLD_ACTION_TIMER,
    E_HOLD_ACTION_PRIORITY,
    KEY:E_HOLD_ACTION_KEY,
    E_HOLD_ACTION_TITLE[MAX_HOLD_ACTION_TITLE],
    PlayerBar:E_HOLD_ACTION_BAR,
    PlayerText:E_HOLD_ACTION_LABEL
};

static
    gHoldActionData[MAX_PLAYERS][E_HOLD_ACTION_DATA],

    // Whether the stop being reported right now is somebody else taking the
    // bar. Only true for the length of one callback, and only read from inside
    // it -- see OnHoldActionRelease for why a listener wants to know.
    bool:gHoldActionOutranked[MAX_PLAYERS],

    // Whether something else is drawn over where the bar goes. A property of
    // the screen rather than of the action, so it outlives any one of them and
    // is not cleared when one ends: a menu covering the bar goes on covering it
    // while the character eats, drinks and changes clothes underneath.
    bool:gHoldActionCovered[MAX_PLAYERS]
;

/**
 * # Functions
 */

/**
 * @brief      Starts the bar filling, and says what for.
 *
 * `draining` turns the bar around: it starts full and empties as the action
 * runs. For something the character is spending rather than building -- a
 * tank of fuel going down while the trigger is held -- where a bar filling up
 * would say the opposite of what is happening. Nothing else changes: the same
 * duration, the same callbacks, and finishing still means the far end.
 *
 * @param      playerid  Character doing it.
 * @param      duration  How long it takes, in milliseconds.
 * @param      title     Shown to the left of the bar. Empty draws no title.
 * @param      key       Key that must stay down. `KEY_NONE` runs on its own.
 * @param      priority  Beats a running action of a lower one.
 * @param      draining  Whether the bar starts full and empties instead.
 * @param      gauge     Whether the caller is measuring it rather than time.
 *
 * @date       02:10 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the bar is up and running
 *             - `false` nothing changed, when:
 *                 + `playerid` is not connected
 *                 + `duration` is zero or below
 *                 + another action is running and `priority` does not beat it
 */
forward bool:StartHoldAction(playerid, duration, const title[] = "", KEY:key = KEY_NONE, priority = 0, bool:draining = false, bool:gauge = false);

/**
 * @brief      Move a gauge to where the thing it is measuring actually is.
 *
 * For a bar that is not a clock. A tank being emptied by the game itself
 * does not come down at a rate this side chose, so the bar has to be told
 * where it is rather than work it out -- and `duration` becomes the full
 * measure rather than a length of time.
 *
 * Only for an action started as a gauge. A timed one is counting and being
 * told a different answer would only be overwritten on the next tick.
 *
 * Reaching the far end finishes the action exactly as running out of time
 * would, so OnHoldActionFinish means the same thing either way.
 *
 * @param      playerid  Character whose bar it is.
 * @param      progress  How far along, in whatever the caller is counting.
 *
 * @date       19:40 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the bar is showing that
 *             - `false` when:
 *                 + nothing is running for them
 *                 + what is running is counting time and not a gauge
 */
forward bool:SetHoldActionProgress(playerid, progress);

/**
 * @brief      The bar itself, to be measured or restyled.
 *
 * Everything about how it looks is progress2's, and this is the handle to
 * it: a gamemode that wants it somewhere else, or a different shape, says
 * so through that rather than through a wrapper here for every setter.
 *
 * It exists for the whole time the player is connected, whether an action
 * is running or not, so it can be set up once and left alone.
 *
 * @param      playerid  Character whose bar it is.
 *
 * @date       21:10 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - their bar
 *             - `INVALID_PLAYER_BAR_ID` they are not connected
 */
forward PlayerBar:GetPlayerHoldActionBar(playerid);

/**
 * @brief      Take the bar off the screen without stopping what it measures.
 *
 * For an action that is running but has nothing to show yet. A trigger held
 * on a weapon that is not actually dispensing anything is that: the action
 * has to be watching, because watching is how it finds out, but a bar sat
 * at full says something is happening when nothing is.
 *
 * The action carries on either way -- the key is still watched, the ticks
 * still arrive, and finishing still finishes. This is only whether anybody
 * can see it.
 *
 * @param      playerid  Character whose bar it is.
 * @param      shown     Whether it should be on screen.
 *
 * @date       11:05 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the bar is in the state asked for
 *             - `false` nothing is running for them
 */
forward bool:SetHoldActionShown(playerid, bool:shown);

/**
 * @brief      Say that something else is drawn over where the bar goes.
 *
 * The other half of `SetHoldActionShown`, and deliberately not the same
 * switch. One is the action saying whether it has anything to show; this is
 * the screen saying whether there is anywhere to show it. The bar is drawn
 * when both agree, so neither has to know what the other decided.
 *
 * Unlike everything else here it is not part of an action. It is set while
 * nothing is running and stays set across actions starting and ending,
 * because a menu covering the bar does not stop covering it because the
 * character started eating.
 *
 * @param      playerid  Character whose screen it is.
 * @param      covered   `true` there is something over it, `false` it is free.
 *
 * @date       15:10 13/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` always; there is no state this can be wrong in
 */
forward bool:SetHoldActionCovered(playerid, bool:covered);

/**
 * @brief      Stops it wherever it got to.
 *
 * Nothing is called back: an abandoned action is not a finished one.
 *
 * @param      playerid  Character to stop.
 *
 * @date       02:10 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` one was running and is not any more
 *             - `false` there was nothing to stop
 */
forward bool:StopHoldAction(playerid);

/**
 * @brief      Whether the bar is in use.
 *
 * @param      playerid  Character to check.
 *
 * @date       02:10 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` something is filling it
 *             - `false` it is free
 */
forward bool:IsPlayerHoldingAction(playerid);

/**
 * @brief      What the character is already doing, for refusing a second thing.
 *
 * @param      playerid  Character to ask.
 * @param      output    Buffer the title is written into.
 * @param      size      Size of that buffer.
 *
 * @date       02:10 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` a title was written into `output`
 *             - `false` `output` is emptied, when nothing is running or the
 *               running action was given no title
 */
forward bool:GetPlayerHoldActionTitle(playerid, output[], size = sizeof (output));

/**
 * @brief      The priority of what is running, for deciding whether to interrupt.
 *
 * @param      playerid  Character to ask.
 *
 * @date       02:10 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the priority it was started with
 *             - `0` when nothing is running
 */
forward GetPlayerHoldActionPriority(playerid);

/**
 * @brief      How far along it is, in milliseconds.
 *
 * @param      playerid  Character to ask.
 *
 * @date       02:10 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - milliseconds elapsed
 *             - `0` when nothing is running
 */
forward GetPlayerHoldActionProgress(playerid);

/**
 * # Events
 */

/**
 * @brief      The bar moved.
 *
 * Every HOLD_ACTION_STEP while an action runs, and not on the tick that
 * finishes it -- that one sends OnHoldActionFinish instead.
 *
 * @param      playerid  Character holding it.
 * @param      progress  How far along, in milliseconds. For a gauge, in
 *                       whatever the caller is counting.
 *
 * @date       02:10 07/09/2026
 * @author     augustocaixeta
 */
forward OnHoldActionUpdate(playerid, progress);

/**
 * @brief      The bar reached the far end.
 *
 * The action is already over and the bar is down. Whatever the hold was for
 * happens here.
 *
 * @param      playerid  Character who held it.
 *
 * @date       02:10 07/09/2026
 * @author     augustocaixeta
 */
forward OnHoldActionFinish(playerid);

/**
 * @brief      The action was given up on.
 *
 * Let go of, interrupted by something that outranked it, or lost to a
 * disconnect. Everything raised when the bar went up and has to come down
 * again belongs here, an animation most of all -- the game goes on playing
 * one long after whatever asked for it has stopped.
 *
 * Never sent for an action that finished.
 *
 * @param      playerid  Character who was holding it.
 * @param      progress  How far it had got, so an action worth something in
 *                       parts can keep the parts it finished.
 *
 * @date       02:10 07/09/2026
 * @author     augustocaixeta
 */
forward OnHoldActionStop(playerid, progress);

/**
 * @brief      The bar is free, and anything that wants it may have it.
 *
 * Sent after a stop or a finish rather than as part of either, so that
 * something waiting for the bar hears about it whatever the action that had
 * it answered.
 *
 * Not sent when an action was pushed off the bar by one that outranks it:
 * the bar went straight to somebody else and was never free.
 *
 * For a bar that belongs to a state rather than to an act -- a fuel gauge is
 * up because a chainsaw is in somebody's hands, and it comes back when
 * whatever borrowed the bar has finished with it.
 *
 * @param      playerid  Character whose bar it is.
 *
 * @date       11:05 13/09/2026
 * @author     augustocaixeta
 */
forward OnHoldActionRelease(playerid);

/**
 * # Internal
 */

forward @HoldAction_Update(playerid);

/**
 * # Implementation
 */

static stock bool:IsHoldActionKeyDownInternal(playerid) {
    new const
        KEY:key = gHoldActionData[playerid][E_HOLD_ACTION_KEY]
    ;

    if (key == KEY_NONE) {
        return true;
    }

    new
        KEY:keys,
        updown,
        leftright
    ;

    GetPlayerKeys(playerid, keys, updown, leftright);

    return ((keys & key) != KEY_NONE);
}

// Everything that has to come down, without saying why it came down. Finishing
// and giving up clear exactly the same state and are told apart by which
// callback follows, which is the caller's to decide -- OnHoldActionStop firing
// on a bar that filled would mean every action had to work out for itself
// whether it had just succeeded.
// Shown when the action has something to show and the screen has somewhere to
// show it. Split in two because the two are decided by different people at
// different times, and either of them saying no is enough.
static stock ApplyHoldActionVisibilityInternal(playerid) {
    if (!gHoldActionData[playerid][E_HOLD_ACTION_RUNNING]) {
        return;
    }

    if (!gHoldActionData[playerid][E_HOLD_ACTION_SHOWN] || gHoldActionCovered[playerid]) {
        HidePlayerProgressBar(playerid, gHoldActionData[playerid][E_HOLD_ACTION_BAR]);
        PlayerTextDrawHide(playerid, gHoldActionData[playerid][E_HOLD_ACTION_LABEL]);

        return;
    }

    ShowPlayerProgressBar(playerid, gHoldActionData[playerid][E_HOLD_ACTION_BAR]);

    if (gHoldActionData[playerid][E_HOLD_ACTION_TITLE][0] != EOS) {
        PlayerTextDrawShow(playerid, gHoldActionData[playerid][E_HOLD_ACTION_LABEL]);
    }
}

static stock bool:ClearHoldActionInternal(playerid) {
    if (!gHoldActionData[playerid][E_HOLD_ACTION_RUNNING]) {
        return false;
    }

    KillTimer(gHoldActionData[playerid][E_HOLD_ACTION_TIMER]);

    gHoldActionData[playerid][E_HOLD_ACTION_RUNNING]  = false;
    gHoldActionData[playerid][E_HOLD_ACTION_SHOWN]    = false;
    gHoldActionData[playerid][E_HOLD_ACTION_LIMIT]    = 0;
    gHoldActionData[playerid][E_HOLD_ACTION_PROGRESS] = 0;
    gHoldActionData[playerid][E_HOLD_ACTION_TIMER]    = 0;
    gHoldActionData[playerid][E_HOLD_ACTION_PRIORITY] = 0;
    gHoldActionData[playerid][E_HOLD_ACTION_KEY]      = KEY_NONE;
    gHoldActionData[playerid][E_HOLD_ACTION_TITLE][0] = EOS;

    HidePlayerProgressBar(playerid, gHoldActionData[playerid][E_HOLD_ACTION_BAR]);
    PlayerTextDrawHide(playerid, gHoldActionData[playerid][E_HOLD_ACTION_LABEL]);

    return true;
}

stock bool:StartHoldAction(playerid, duration, const title[] = "", KEY:key = KEY_NONE, priority = 0, bool:draining = false, bool:gauge = false) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (duration <= 0) {
        return false;
    }

    if (gHoldActionData[playerid][E_HOLD_ACTION_RUNNING]) {
        if (priority <= gHoldActionData[playerid][E_HOLD_ACTION_PRIORITY]) {
            return false;
        }

        // Said out loud while it comes down, because a listener that answers a
        // stop by raising a bar of its own has to tell being interrupted from
        // being finished with -- the first would be raising it into a bar this
        // call is halfway through claiming.
        gHoldActionOutranked[playerid] = true;

        StopHoldAction(playerid);

        gHoldActionOutranked[playerid] = false;
    }

    // A listener that started something of its own anyway. Its timer is about
    // to be overwritten by the one below and would go on ticking with nothing
    // behind it, so it comes down here -- silently, because the callback for it
    // has already been sent.
    if (gHoldActionData[playerid][E_HOLD_ACTION_RUNNING]) {
        ClearHoldActionInternal(playerid);
    }

    gHoldActionData[playerid][E_HOLD_ACTION_RUNNING]  = true;
    gHoldActionData[playerid][E_HOLD_ACTION_SHOWN]    = true;
    gHoldActionData[playerid][E_HOLD_ACTION_DRAINING] = draining;
    gHoldActionData[playerid][E_HOLD_ACTION_GAUGE]    = gauge;
    gHoldActionData[playerid][E_HOLD_ACTION_LIMIT]    = duration;
    gHoldActionData[playerid][E_HOLD_ACTION_PROGRESS] = 0;
    gHoldActionData[playerid][E_HOLD_ACTION_PRIORITY] = priority;
    gHoldActionData[playerid][E_HOLD_ACTION_KEY]      = key;
    gHoldActionData[playerid][E_HOLD_ACTION_TIMER]    = SetTimerEx("@HoldAction_Update", HOLD_ACTION_STEP, true, "i", playerid);

    strcopy(gHoldActionData[playerid][E_HOLD_ACTION_TITLE], title);

    SetPlayerProgressBarMaxValue(playerid, gHoldActionData[playerid][E_HOLD_ACTION_BAR], float(duration));
    SetPlayerProgressBarValue(playerid, gHoldActionData[playerid][E_HOLD_ACTION_BAR], 0.0);

    if (draining) {
        SetPlayerProgressBarValue(playerid, gHoldActionData[playerid][E_HOLD_ACTION_BAR], float(duration));
    }

    if (title[0] != EOS) {
        PlayerTextDrawSetString(playerid, gHoldActionData[playerid][E_HOLD_ACTION_LABEL], title);
    }

    // Which may draw nothing at all, if the screen is covered. The action runs
    // either way -- being unable to see a bar is not being unable to do a thing.
    ApplyHoldActionVisibilityInternal(playerid);

    return true;
}

stock bool:SetHoldActionShown(playerid, bool:shown) {
    if (!gHoldActionData[playerid][E_HOLD_ACTION_RUNNING]) {
        return false;
    }

    gHoldActionData[playerid][E_HOLD_ACTION_SHOWN] = shown;

    ApplyHoldActionVisibilityInternal(playerid);

    return true;
}

stock bool:SetHoldActionCovered(playerid, bool:covered) {
    gHoldActionCovered[playerid] = covered;

    ApplyHoldActionVisibilityInternal(playerid);

    return true;
}

stock PlayerBar:GetPlayerHoldActionBar(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_PLAYER_BAR_ID;
    }

    return gHoldActionData[playerid][E_HOLD_ACTION_BAR];
}

stock bool:SetHoldActionProgress(playerid, progress) {
    if (!gHoldActionData[playerid][E_HOLD_ACTION_RUNNING]) {
        return false;
    }

    if (!gHoldActionData[playerid][E_HOLD_ACTION_GAUGE]) {
        return false;
    }

    if (progress < 0) {
        progress = 0;
    }

    gHoldActionData[playerid][E_HOLD_ACTION_PROGRESS] = progress;

    new
        shown = progress
    ;

    if (gHoldActionData[playerid][E_HOLD_ACTION_DRAINING]) {
        shown = gHoldActionData[playerid][E_HOLD_ACTION_LIMIT] - progress;
    }

    if (shown < 0) {
        shown = 0;
    }

    SetPlayerProgressBarValue(playerid, gHoldActionData[playerid][E_HOLD_ACTION_BAR], float(shown));

    return true;
}

stock bool:StopHoldAction(playerid) {
    new const
        progress = gHoldActionData[playerid][E_HOLD_ACTION_PROGRESS]
    ;

    if (!ClearHoldActionInternal(playerid)) {
        return false;
    }

    // After the state is cleared, so that an answer which starts another action
    // is answering about one that is over rather than racing one still running.
    CallLocalFunction("OnHoldActionStop", "ii", playerid, progress);

    // Stopped to be taken rather than stopped to be free. Whoever is claiming
    // it is halfway through StartHoldAction and the bar is theirs already.
    if (gHoldActionOutranked[playerid]) {
        return true;
    }

    CallLocalFunction("OnHoldActionRelease", "i", playerid);

    return true;
}

stock bool:IsPlayerHoldingAction(playerid) {
    return gHoldActionData[playerid][E_HOLD_ACTION_RUNNING];
}

stock bool:GetPlayerHoldActionTitle(playerid, output[], size = sizeof (output)) {
    output[0] = EOS;

    if (!gHoldActionData[playerid][E_HOLD_ACTION_RUNNING]) {
        return false;
    }

    if (gHoldActionData[playerid][E_HOLD_ACTION_TITLE][0] == EOS) {
        return false;
    }

    strcopy(output, gHoldActionData[playerid][E_HOLD_ACTION_TITLE], size);

    return true;
}

stock GetPlayerHoldActionPriority(playerid) {
    if (!gHoldActionData[playerid][E_HOLD_ACTION_RUNNING]) {
        return 0;
    }

    return gHoldActionData[playerid][E_HOLD_ACTION_PRIORITY];
}

stock GetPlayerHoldActionProgress(playerid) {
    if (!gHoldActionData[playerid][E_HOLD_ACTION_RUNNING]) {
        return 0;
    }

    return gHoldActionData[playerid][E_HOLD_ACTION_PROGRESS];
}

/**
 * # Calls
 */

public @HoldAction_Update(playerid) {
    if (!gHoldActionData[playerid][E_HOLD_ACTION_RUNNING]) {
        return;
    }

    if (!IsHoldActionKeyDownInternal(playerid)) {
        StopHoldAction(playerid);

        return;
    }

    // A gauge is not a clock: what it reads was set by whoever is measuring
    // the thing, and the tick is here to watch the key and redraw.
    if (!gHoldActionData[playerid][E_HOLD_ACTION_GAUGE]) {
        gHoldActionData[playerid][E_HOLD_ACTION_PROGRESS] += HOLD_ACTION_STEP;
    }

    new const
        progress = gHoldActionData[playerid][E_HOLD_ACTION_PROGRESS]
    ;

    if (progress >= gHoldActionData[playerid][E_HOLD_ACTION_LIMIT]) {
        // Cleared rather than stopped: the bar filled, so this is the one exit
        // that is not a giving up and OnHoldActionStop must not hear about it.
        ClearHoldActionInternal(playerid);

        CallLocalFunction("OnHoldActionFinish", "i", playerid);
        CallLocalFunction("OnHoldActionRelease", "i", playerid);

        return;
    }

    // A gauge is drawn by whoever is measuring the thing, through
    // SetHoldActionProgress, and drawing it here as well would be sending the
    // same number ten times a second. For a bar that stays up the whole time a
    // weapon is out, that is the whole of what it would cost.
    if (!gHoldActionData[playerid][E_HOLD_ACTION_GAUGE]) {
        new
            shown = progress
        ;

        if (gHoldActionData[playerid][E_HOLD_ACTION_DRAINING]) {
            shown = gHoldActionData[playerid][E_HOLD_ACTION_LIMIT] - progress;
        }

        SetPlayerProgressBarValue(playerid, gHoldActionData[playerid][E_HOLD_ACTION_BAR], float(shown));
    }

    CallLocalFunction("OnHoldActionUpdate", "ii", playerid, progress);
}

hook OnPlayerConnect(playerid) {
    gHoldActionOutranked[playerid] = false;
    gHoldActionCovered[playerid] = false;

    gHoldActionData[playerid][E_HOLD_ACTION_RUNNING]  = false;
    gHoldActionData[playerid][E_HOLD_ACTION_SHOWN]    = false;
    gHoldActionData[playerid][E_HOLD_ACTION_LIMIT]    = 0;
    gHoldActionData[playerid][E_HOLD_ACTION_PROGRESS] = 0;
    gHoldActionData[playerid][E_HOLD_ACTION_TIMER]    = 0;
    gHoldActionData[playerid][E_HOLD_ACTION_PRIORITY] = 0;
    gHoldActionData[playerid][E_HOLD_ACTION_KEY]      = KEY_NONE;
    gHoldActionData[playerid][E_HOLD_ACTION_TITLE][0] = EOS;

    PlayerBarUI_ResetPlayer(playerid);

    gHoldActionData[playerid][E_HOLD_ACTION_BAR] = CreatePlayerProgressBar(playerid, HOLD_ACTION_BAR_X, HOLD_ACTION_BAR_Y, HOLD_ACTION_BAR_WIDTH, HOLD_ACTION_BAR_HEIGHT, HOLD_ACTION_BAR_COLOUR, 100.0);

    SetPlayerProgressBarPadding(playerid, gHoldActionData[playerid][E_HOLD_ACTION_BAR], HOLD_ACTION_BAR_PADDING_X, HOLD_ACTION_BAR_PADDING_Y);

    new const
        PlayerText:label = CreatePlayerTextDraw(playerid, HOLD_ACTION_TITLE_X, HOLD_ACTION_TITLE_Y, "_")
    ;

    PlayerTextDrawLetterSize(playerid, label, HOLD_ACTION_TITLE_LETTER_X, HOLD_ACTION_TITLE_LETTER_Y);
    PlayerTextDrawAlignment(playerid, label, TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, label, HOLD_ACTION_TITLE_COLOUR);
    PlayerTextDrawSetShadow(playerid, label, 1);
    PlayerTextDrawSetOutline(playerid, label, 2);
    PlayerTextDrawBackgroundColour(playerid, label, 255);
    PlayerTextDrawFont(playerid, label, TEXT_DRAW_FONT_BANK_GOTHIC);
    PlayerTextDrawSetProportional(playerid, label, true);

    gHoldActionData[playerid][E_HOLD_ACTION_LABEL] = label;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    StopHoldAction(playerid);

    return 0;
}
