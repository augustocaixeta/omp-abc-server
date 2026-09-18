#if defined _INC_FITNESS
    #endinput
#endif
#define _INC_FITNESS

#include <pp-hooks>

/**
 * # Header
 */

#if !defined FITNESS_MAX
    #define FITNESS_MAX (100.0)
#endif

#if !defined FITNESS_STRENGTH_GAIN
    #define FITNESS_STRENGTH_GAIN (0.25)
#endif

#if !defined FITNESS_STAMINA_GAIN
    #define FITNESS_STAMINA_GAIN (0.02)
#endif

#if !defined FITNESS_STRENGTH_POWER
    #define FITNESS_STRENGTH_POWER (3.0)
#endif

#if !defined FITNESS_STAMINA_POWER
    #define FITNESS_STAMINA_POWER (2.5)
#endif

// Seconds of sprint a player has with no training at all, and with all of it.
#if !defined FITNESS_BREATH_BASE
    #define FITNESS_BREATH_BASE (6.0)
#endif

#if !defined FITNESS_BREATH_TRAINED
    #define FITNESS_BREATH_TRAINED (18.0)
#endif

// How fast breath comes back, as a share of what running spends.
#if !defined FITNESS_BREATH_RECOVERY
    #define FITNESS_BREATH_RECOVERY (0.6)
#endif

// Share of the breath needed before an exhausted player can sprint again.
#if !defined FITNESS_BREATH_RECOVER_TO
    #define FITNESS_BREATH_RECOVER_TO (0.35)
#endif

#if !defined FITNESS_BREATH_TICK
    #define FITNESS_BREATH_TICK (250)
#endif

// Training a second of street running is worth.
#if !defined FITNESS_SPRINT_GAIN
    #define FITNESS_SPRINT_GAIN (0.05)
#endif

// What a second on a treadmill or a bike costs in breath, against a second of
// running down the street, one for each speed the machine runs at.
#if !defined FITNESS_EXERCISE_BREATH_SLOW
    #define FITNESS_EXERCISE_BREATH_SLOW (0.15)
#endif

#if !defined FITNESS_EXERCISE_BREATH_MEDIUM
    #define FITNESS_EXERCISE_BREATH_MEDIUM (0.45)
#endif

#if !defined FITNESS_EXERCISE_BREATH_FAST
    #define FITNESS_EXERCISE_BREATH_FAST (1.0)
#endif

// How much of the bar a player out of breath loses every time the exercise
// ticks: pressing harder does not answer for not being able to breathe.
#if !defined FITNESS_EXHAUSTED_POWER_LOSS
    #define FITNESS_EXHAUSTED_POWER_LOSS (20.0)
#endif

#if defined _INC_GYM
    // Running out of breath at this pace or faster does not slow the player
    // down, it takes their legs out from under them and the belt does the rest.
    // Anywhere slower they simply lose the pace. GYM_SPEED_SLOW would have them
    // go down at a walk; a level past the fastest one means never.
    #if !defined FITNESS_EXHAUSTED_FALL_SPEED
        #define FITNESS_EXHAUSTED_FALL_SPEED (GYM_SPEED_FAST)
    #endif
#endif

#define FITNESS_ANIM_RUN_CYCLE (1180)
#define FITNESS_ANIM_RUN_A (1061)
#define FITNESS_ANIM_RUN_B (1062)
#define FITNESS_ANIM_RUN_C (1129)
#define FITNESS_ANIM_RUN_D (1133)
#define FITNESS_ANIM_RUN_E (1196)
#define FITNESS_ANIM_RUN_F (1231)
#define FITNESS_ANIM_RUN_G (1266)

#if !defined FITNESS_EXHAUSTED_LIBRARY
    #define FITNESS_EXHAUSTED_LIBRARY "FAT"
#endif

#if !defined FITNESS_EXHAUSTED_ANIMATION
    #define FITNESS_EXHAUSTED_ANIMATION "IDLE_TIRED"
#endif

enum E_FITNESS_ATTRIBUTE {
    FITNESS_NONE = -1,
    FITNESS_STRENGTH,
    FITNESS_STAMINA
};

static enum _:E_FITNESS_BREATH_DATA {
    Float:E_FITNESS_BREATH,
    bool:E_FITNESS_EXHAUSTED,
    E_FITNESS_BREATH_TIMER
};

static
    Float:gFitnessData[MAX_PLAYERS][E_FITNESS_ATTRIBUTE],
    gFitnessBreath[MAX_PLAYERS][E_FITNESS_BREATH_DATA]
;

/**
 * # Functions
 */

/**
 * Gives how trained the player is at that attribute.
 *
 * @param playerid    Player to read.
 * @param attribute   FITNESS_STRENGTH or FITNESS_STAMINA.
 *
 * @return A value between 0.0 and FITNESS_MAX, 0.0 for an invalid player.
 */
forward Float:GetPlayerFitness(playerid, E_FITNESS_ATTRIBUTE:attribute);

/**
 * Overwrites an attribute, clamped to the allowed range. This is what a
 * gamemode calls when it loads a player from its database.
 *
 * @param playerid    Player to change.
 * @param attribute   FITNESS_STRENGTH or FITNESS_STAMINA.
 * @param value       New amount of training.
 *
 * @return True when the player and the attribute exist.
 */
forward bool:SetPlayerFitness(playerid, E_FITNESS_ATTRIBUTE:attribute, Float:value);

/**
 * Trains an attribute. What is really added shrinks as the attribute grows, so
 * the first sessions of a player are worth much more than the last ones, and
 * FITNESS_MAX is approached without ever being handed out.
 *
 * @param playerid    Player to train.
 * @param attribute   FITNESS_STRENGTH or FITNESS_STAMINA.
 * @param amount      How much training the effort is worth at zero.
 *
 * @return What was really added.
 */
forward Float:GivePlayerFitness(playerid, E_FITNESS_ATTRIBUTE:attribute, Float:amount);

/**
 * Takes training away, which is how a gamemode makes a player who stopped
 * going to the gym lose the shape they had.
 *
 * @param playerid    Player to detrain.
 * @param factor      Share of the current training to take, 0.0 to 1.0.
 *
 * @return True when the player exists.
 */
forward bool:DecayPlayerFitness(playerid, Float:factor);

/**
 * Clears every attribute of the player.
 *
 * @param playerid    Player to reset.
 *
 * @return True when the player exists.
 */
forward bool:ResetPlayerFitness(playerid);

/**
 * Shortcuts for the two attributes.
 */
forward Float:GetPlayerStrength(playerid);
forward Float:GetPlayerStamina(playerid);
forward bool:SetPlayerStrength(playerid, Float:value);
forward bool:SetPlayerStamina(playerid, Float:value);
forward Float:GivePlayerStrength(playerid, Float:amount);
forward Float:GivePlayerStamina(playerid, Float:amount);

/**
 * Gives how many seconds of sprint the player has left.
 *
 * @param playerid    Player to read.
 *
 * @return Seconds of sprint, 0.0 for an invalid player.
 */
forward Float:GetPlayerBreath(playerid);

/**
 * Gives how many seconds of sprint the player holds when rested. It grows with
 * FITNESS_STAMINA, from FITNESS_BREATH_BASE with no training at all to
 * FITNESS_BREATH_TRAINED with all of it.
 *
 * @param playerid    Player to read.
 *
 * @return Seconds of sprint the player can hold.
 */
forward Float:GetPlayerMaxBreath(playerid);

/**
 * Overwrites the breath of the player, clamped to what they can hold.
 *
 * @param playerid    Player to change.
 * @param value       Seconds of sprint left.
 *
 * @return True when the player exists.
 */
forward bool:SetPlayerBreath(playerid, Float:value);

/**
 * Spends or returns breath, which is how a gamemode makes swimming, climbing or
 * carrying something cost the player the same air running costs.
 *
 * @param playerid    Player to change.
 * @param amount      Seconds to add, negative to spend.
 *
 * @return What was really added.
 */
forward Float:GivePlayerBreath(playerid, Float:amount);

/**
 * Tells whether the player ran out of breath and is still catching it.
 *
 * @param playerid    Player to check.
 *
 * @return True while the player cannot sprint.
 */
forward bool:IsPlayerExhausted(playerid);

/**
 * Puts the player out of breath, or lets them sprint again. Being exhausted
 * plays the tired animation and blocks sprinting until enough breath is back.
 *
 * @param playerid    Player to change.
 * @param exhausted   True to knock the wind out of the player.
 *
 * @return True when the player exists.
 */
forward bool:SetPlayerExhausted(playerid, bool:exhausted);

/**
 * Tells whether the player is sprinting on foot right now.
 *
 * @param playerid    Player to check.
 *
 * @return True while the player is running with the sprint key.
 */
forward bool:IsPlayerSprinting(playerid);

/**
 * # Callbacks
 */

/**
 * Called whenever an attribute of a player moves, by training or by a call of
 * the gamemode, which is where the hud of the player is usually refreshed and
 * where the new value is written to a database.
 *
 * @param playerid    Player whose training changed.
 * @param attribute   FITNESS_STRENGTH or FITNESS_STAMINA.
 * @param amount      How much it moved, negative when training was lost.
 * @param total       How trained the player is now.
 */
forward OnPlayerFitnessChange(playerid, E_FITNESS_ATTRIBUTE:attribute, Float:amount, Float:total);

/**
 * Called when the player runs out of breath and stops being able to sprint.
 *
 * @param playerid    Player who ran out of breath.
 */
forward OnPlayerBreathOver(playerid);

/**
 * Called when the player has caught enough breath to sprint again.
 *
 * @param playerid    Player who recovered.
 */
forward OnPlayerBreathRecover(playerid);

/**
 * # Internal Functions
 */

static stock Float:GetFitnessInternal(playerid, E_FITNESS_ATTRIBUTE:attribute) {
    return gFitnessData[playerid][attribute];
}

static stock bool:IsValidFitnessAttributeInternal(playerid, E_FITNESS_ATTRIBUTE:attribute) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    return FITNESS_STRENGTH <= attribute <= FITNESS_STAMINA;
}

static stock Float:GetClampedFitnessInternal(Float:value) {
    if (value < 0.0) {
        return 0.0;
    }

    if (value > FITNESS_MAX) {
        return FITNESS_MAX;
    }

    return value;
}

static stock ApplyFitnessInternal(playerid, E_FITNESS_ATTRIBUTE:attribute, Float:value) {
    new const
        Float:current = gFitnessData[playerid][attribute],
        Float:clamped = GetClampedFitnessInternal(value)
    ;

    if (clamped == current) {
        return;
    }

    gFitnessData[playerid][attribute] = clamped;

    #if defined _INC_GYM
        if (attribute == FITNESS_STRENGTH) {
            SetPlayerGymStrength(playerid, clamped);
        }
    #endif

    CallLocalFunction("OnPlayerFitnessChange", "iiff", playerid, _:attribute, clamped - current, clamped);
}

static stock Float:GetMaxBreathInternal(playerid) {
    return FITNESS_BREATH_BASE + (FITNESS_BREATH_TRAINED - FITNESS_BREATH_BASE) * GetFitnessInternal(playerid, FITNESS_STAMINA) / FITNESS_MAX;
}

// Keys say what is pressed, the animation says what the ped is doing: a player
// can hold the sprint key against a wall, mid jump or with a weapon out. When
// movement.inc is around its reading is used, otherwise the same animation
// indexes are read here, which need UsePlayerPedAnims() to mean anything.
static stock bool:IsSprintingInternal(playerid) {
    #if defined _INC_GYM
        // Whatever the keys say, somebody on a machine is not running down the
        // street: the gym answers for that effort on its own.
        if (IsPlayerExercising(playerid)) {
            return false;
        }
    #endif

    #if defined _INC_MOVEMENT
        return GetPlayerMovement(playerid) == PLAYER_MOVEMENT_RUN;
    #else
        if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) {
            return false;
        }

        if (GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_USEJETPACK) {
            return false;
        }

        new
            KEY:keys, updown, leftright
        ;

        GetPlayerKeys(playerid, keys, updown, leftright);

        if (!(keys & KEY_SPRINT)) {
            return false;
        }

        // Holding the key while standing still is not running.
        if (!updown && !leftright) {
            return false;
        }

        switch (GetPlayerAnimationIndex(playerid)) {
            case FITNESS_ANIM_RUN_CYCLE, FITNESS_ANIM_RUN_A, FITNESS_ANIM_RUN_B, FITNESS_ANIM_RUN_C,
                 FITNESS_ANIM_RUN_D, FITNESS_ANIM_RUN_E, FITNESS_ANIM_RUN_F, FITNESS_ANIM_RUN_G: {
                return true;
            }
        }

        return false;
    #endif
}

static stock KillBreathTimerInternal(playerid) {
    if (gFitnessBreath[playerid][E_FITNESS_BREATH_TIMER]) {
        KillTimer(gFitnessBreath[playerid][E_FITNESS_BREATH_TIMER]);

        gFitnessBreath[playerid][E_FITNESS_BREATH_TIMER] = 0;
    }
}

static stock StartBreathTimerInternal(playerid) {
    if (gFitnessBreath[playerid][E_FITNESS_BREATH_TIMER]) {
        return;
    }

    gFitnessBreath[playerid][E_FITNESS_BREATH_TIMER] = SetTimerEx("@Fitness_Breath", FITNESS_BREATH_TICK, true, "i", playerid);
}

// While the player is on a machine the gym is the one playing animations, and
// dropping the tired one on top of a movement halfway through only fights it:
// the tick puts the exercise animation straight back and the character twitches.
// Being out of breath shows there as the pace falling away instead.
static stock bool:IsFitnessAnimationOwnedInternal(playerid) {
    #if defined _INC_GYM
        return IsPlayerExercising(playerid);
    #else
        #pragma unused playerid

        return false;
    #endif
}

static stock ExhaustPlayerInternal(playerid, bool:exhausted) {
    if (gFitnessBreath[playerid][E_FITNESS_EXHAUSTED] == exhausted) {
        return;
    }

    gFitnessBreath[playerid][E_FITNESS_EXHAUSTED] = exhausted;

    if (exhausted) {
        if (!IsFitnessAnimationOwnedInternal(playerid)) {
            ApplyAnimation(playerid, FITNESS_EXHAUSTED_LIBRARY, FITNESS_EXHAUSTED_ANIMATION, 4.1, false, false, false, false, 0, SYNC_ALL);
        }

        CallLocalFunction("OnPlayerBreathOver", "i", playerid);

        #if defined _INC_GYM
            // Only the treadmill. A bike leaves the player sitting down with
            // nothing to fall off, and the weights are the arms rather than the
            // lungs. The animation that plays is picked by the gym from whatever
            // the player was doing, which at this moment is still running.
            if (GetPlayerExerciseType(playerid) == GYM_EQUIPMENT_TREADMILL && GetPlayerExerciseSpeed(playerid) >= FITNESS_EXHAUSTED_FALL_SPEED) {
                StopPlayerExercise(playerid);
            }
        #endif

        return;
    }

    CallLocalFunction("OnPlayerBreathRecover", "i", playerid);
}

/**
 * # Timers
 */

// What the player is spending breath on right now, as a multiple of a second of
// street running. Standing around spends nothing.
static stock Float:GetBreathCostInternal(playerid) {
    #if defined _INC_GYM
        switch (GetPlayerExerciseType(playerid)) {
            case GYM_EQUIPMENT_TREADMILL, GYM_EQUIPMENT_BIKE: {
                switch (GetPlayerExerciseSpeed(playerid)) {
                    case GYM_SPEED_SLOW: {
                        return FITNESS_EXERCISE_BREATH_SLOW;
                    }
                    case GYM_SPEED_MEDIUM: {
                        return FITNESS_EXERCISE_BREATH_MEDIUM;
                    }
                    case GYM_SPEED_FAST: {
                        return FITNESS_EXERCISE_BREATH_FAST;
                    }
                    default: {}
                }

                return 0.0;
            }
            default: {}
        }
    #endif

    if (IsSprintingInternal(playerid)) {
        return 1.0;
    }

    return 0.0;
}

forward @Fitness_Breath(playerid);

public @Fitness_Breath(playerid) {
    if (!IsPlayerConnected(playerid)) {
        KillBreathTimerInternal(playerid);

        return 0;
    }

    new const
        Float:seconds = float(FITNESS_BREATH_TICK) / 1000.0,
        Float:maximum = GetMaxBreathInternal(playerid),
        Float:cost = GetBreathCostInternal(playerid)
    ;

    if (cost > 0.0 && !gFitnessBreath[playerid][E_FITNESS_EXHAUSTED]) {
        new const
            Float:left = gFitnessBreath[playerid][E_FITNESS_BREATH] - seconds * cost
        ;

        gFitnessBreath[playerid][E_FITNESS_BREATH] = (left < 0.0) ? 0.0 : left;

        // The street trains the same attribute the treadmill does, but what is
        // done on the equipment is counted by the gym when it ends.
        if (IsSprintingInternal(playerid)) {
            GivePlayerFitness(playerid, FITNESS_STAMINA, FITNESS_SPRINT_GAIN * seconds);
        }

        if (left <= 0.0) {
            ExhaustPlayerInternal(playerid, true);
        }

        return 0;
    }

    if (gFitnessBreath[playerid][E_FITNESS_BREATH] >= maximum) {
        if (!gFitnessBreath[playerid][E_FITNESS_EXHAUSTED]) {
            KillBreathTimerInternal(playerid);
        }

        return 0;
    }

    new const
        Float:recovered = gFitnessBreath[playerid][E_FITNESS_BREATH] + seconds * FITNESS_BREATH_RECOVERY * (1.0 + GetFitnessInternal(playerid, FITNESS_STAMINA) / FITNESS_MAX)
    ;

    gFitnessBreath[playerid][E_FITNESS_BREATH] = (recovered > maximum) ? maximum : recovered;

    if (gFitnessBreath[playerid][E_FITNESS_EXHAUSTED] && gFitnessBreath[playerid][E_FITNESS_BREATH] >= maximum * FITNESS_BREATH_RECOVER_TO) {
        ExhaustPlayerInternal(playerid, false);
    }

    return 0;
}

/**
 * # External Functions
 */

stock Float:GetPlayerFitness(playerid, E_FITNESS_ATTRIBUTE:attribute) {
    if (!IsValidFitnessAttributeInternal(playerid, attribute)) {
        return 0.0;
    }

    return gFitnessData[playerid][attribute];
}

stock bool:SetPlayerFitness(playerid, E_FITNESS_ATTRIBUTE:attribute, Float:value) {
    if (!IsValidFitnessAttributeInternal(playerid, attribute)) {
        return false;
    }

    ApplyFitnessInternal(playerid, attribute, value);

    return true;
}

stock Float:GivePlayerFitness(playerid, E_FITNESS_ATTRIBUTE:attribute, Float:amount) {
    if (!IsValidFitnessAttributeInternal(playerid, attribute)) {
        return 0.0;
    }

    new const
        Float:current = gFitnessData[playerid][attribute],
        // The closer to the top, the less an effort is worth.
        Float:gained = amount * (1.0 - current / FITNESS_MAX)
    ;

    ApplyFitnessInternal(playerid, attribute, current + gained);

    return gFitnessData[playerid][attribute] - current;
}

stock bool:DecayPlayerFitness(playerid, Float:factor) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    if (!(0.0 < factor <= 1.0)) {
        return false;
    }

    for (new E_FITNESS_ATTRIBUTE:attribute; attribute != E_FITNESS_ATTRIBUTE; ++attribute) {
        ApplyFitnessInternal(playerid, attribute, gFitnessData[playerid][attribute] * (1.0 - factor));
    }

    return true;
}

stock bool:ResetPlayerFitness(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    for (new E_FITNESS_ATTRIBUTE:attribute; attribute != E_FITNESS_ATTRIBUTE; ++attribute) {
        gFitnessData[playerid][attribute] = 0.0;
    }

    return true;
}

stock Float:GetPlayerStrength(playerid) {
    return GetPlayerFitness(playerid, FITNESS_STRENGTH);
}

stock Float:GetPlayerStamina(playerid) {
    return GetPlayerFitness(playerid, FITNESS_STAMINA);
}

stock bool:SetPlayerStrength(playerid, Float:value) {
    return SetPlayerFitness(playerid, FITNESS_STRENGTH, value);
}

stock bool:SetPlayerStamina(playerid, Float:value) {
    return SetPlayerFitness(playerid, FITNESS_STAMINA, value);
}

stock Float:GivePlayerStrength(playerid, Float:amount) {
    return GivePlayerFitness(playerid, FITNESS_STRENGTH, amount);
}

stock Float:GivePlayerStamina(playerid, Float:amount) {
    return GivePlayerFitness(playerid, FITNESS_STAMINA, amount);
}

stock Float:GetPlayerBreath(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return 0.0;
    }

    return gFitnessBreath[playerid][E_FITNESS_BREATH];
}

stock Float:GetPlayerMaxBreath(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return 0.0;
    }

    return GetMaxBreathInternal(playerid);
}

stock bool:SetPlayerBreath(playerid, Float:value) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    new const
        Float:maximum = GetMaxBreathInternal(playerid),
        Float:clamped = (value < 0.0) ? 0.0 : ((value > maximum) ? maximum : value)
    ;

    gFitnessBreath[playerid][E_FITNESS_BREATH] = clamped;

    if (clamped < maximum) {
        StartBreathTimerInternal(playerid);
    }

    return true;
}

stock Float:GivePlayerBreath(playerid, Float:amount) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return 0.0;
    }

    new const
        Float:before = gFitnessBreath[playerid][E_FITNESS_BREATH]
    ;

    SetPlayerBreath(playerid, before + amount);

    if (gFitnessBreath[playerid][E_FITNESS_BREATH] <= 0.0) {
        ExhaustPlayerInternal(playerid, true);
    }

    return gFitnessBreath[playerid][E_FITNESS_BREATH] - before;
}

stock bool:IsPlayerExhausted(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    return gFitnessBreath[playerid][E_FITNESS_EXHAUSTED];
}

stock bool:SetPlayerExhausted(playerid, bool:exhausted) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    if (exhausted) {
        gFitnessBreath[playerid][E_FITNESS_BREATH] = 0.0;
    }

    ExhaustPlayerInternal(playerid, exhausted);
    StartBreathTimerInternal(playerid);

    return true;
}

stock bool:IsPlayerSprinting(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    return IsSprintingInternal(playerid);
}

/**
 * # Calls
 */

hook OnPlayerConnect(playerid) {
    ResetPlayerFitness(playerid);

    KillBreathTimerInternal(playerid);

    gFitnessBreath[playerid][E_FITNESS_BREATH] = GetMaxBreathInternal(playerid);
    gFitnessBreath[playerid][E_FITNESS_EXHAUSTED] = false;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    KillBreathTimerInternal(playerid);

    return 0;
}

// An exhausted player who insists on the sprint key is put back into the tired
// animation, which is what actually keeps them from running.
hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if (!((newkeys ^ oldkeys) & KEY_SPRINT)) {
        return 0;
    }

    if (!(newkeys & KEY_SPRINT)) {
        return 0;
    }

    if (gFitnessBreath[playerid][E_FITNESS_EXHAUSTED]) {
        if (!IsFitnessAnimationOwnedInternal(playerid)) {
            ApplyAnimation(playerid, FITNESS_EXHAUSTED_LIBRARY, FITNESS_EXHAUSTED_ANIMATION, 4.1, false, false, false, false, 0, SYNC_ALL);
        }

        return 0;
    }

    StartBreathTimerInternal(playerid);

    return 0;
}

#if defined _INC_GYM

// Training shows on the bar of the exercise: strength pushes the weights and
// stamina keeps the pace, so a trained player does more with the same effort.
// Hook order on connect is not something to rely on, so the strength is handed
// over again when an exercise actually starts.
hook OnPlayerStartExercise(playerid, E_GYM_EQUIPMENT_TYPE:type, index) {
    SetPlayerGymStrength(playerid, GetFitnessInternal(playerid, FITNESS_STRENGTH));

    // Running and pedalling take breath from the first second, so the timer
    // that spends and gives it back runs for as long as the exercise does.
    StartBreathTimerInternal(playerid);

    return 0;
}

hook OnPlayerUpdateExercise(playerid, E_GYM_EQUIPMENT_TYPE:type, index, Float:power, metric) {
    switch (type) {
        case GYM_EQUIPMENT_BARBELL, GYM_EQUIPMENT_DUMBELL: {
            SetPlayerExercisePower(playerid, power + FITNESS_STRENGTH_POWER * GetFitnessInternal(playerid, FITNESS_STRENGTH) / FITNESS_MAX);
        }
        case GYM_EQUIPMENT_TREADMILL, GYM_EQUIPMENT_BIKE: {
            // Out of breath there is no keeping the pace: the machine comes
            // down however hard the player presses.
            if (gFitnessBreath[playerid][E_FITNESS_EXHAUSTED]) {
                SetPlayerExercisePower(playerid, power - FITNESS_EXHAUSTED_POWER_LOSS);

                return 0;
            }

            SetPlayerExercisePower(playerid, power + FITNESS_STAMINA_POWER * GetFitnessInternal(playerid, FITNESS_STAMINA) / FITNESS_MAX);
        }
        default: {}
    }

    return 0;
}

// What was done on the equipment becomes training, weights into strength and
// distance into stamina.
hook OnPlayerFinishExercise(playerid, E_GYM_EQUIPMENT_TYPE:type, index, metric) {
    // Off the equipment the animation belongs to nobody again, so whoever got
    // off with nothing left is finally free to show it.
    if (gFitnessBreath[playerid][E_FITNESS_EXHAUSTED]) {
        ApplyAnimation(playerid, FITNESS_EXHAUSTED_LIBRARY, FITNESS_EXHAUSTED_ANIMATION, 4.1, false, false, false, false, 0, SYNC_ALL);
    }

    switch (type) {
        case GYM_EQUIPMENT_BARBELL, GYM_EQUIPMENT_DUMBELL: {
            // Lifting more is worth more, and how much more depends on the
            // equipment: a bar and a dumbell of the same kilograms are not the
            // same work at all, so what counts is the strength the weight asks
            // for, which is already on one scale for both.
            new const
                Float:required = GetGymEquipmentRequiredStrength(type, index),
                Float:worth = (required > 0.0) ? (required / GYM_POWER_MAX) : 1.0
            ;

            GivePlayerStrength(playerid, FITNESS_STRENGTH_GAIN * float(metric) * worth);
        }
        case GYM_EQUIPMENT_TREADMILL, GYM_EQUIPMENT_BIKE: {
            GivePlayerStamina(playerid, FITNESS_STAMINA_GAIN * float(metric));
        }
        default: {}
    }

    return 0;
}

#endif
