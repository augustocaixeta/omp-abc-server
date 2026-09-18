#if defined _INC_GYM
    #endinput
#endif
#define _INC_GYM

// The equipment is a button: an area with a prompt in it that says which key
// answers. button.inc is what builds that, and it comes before pp-hooks because
// it pulls pp-hooks in itself, and the hooks written after the last include of
// pp-hooks are the ones that get a number of their own.
#include <I\button>

#include <pp-hooks>
#include <streamer>

/**
 * # Header
 */

#if defined GYM_STREAMER_OBJECTS
    #define GYM_INVALID_OBJECT_ID (INVALID_STREAMER_ID)
#else
    #define GYM_INVALID_OBJECT_ID (INVALID_OBJECT_ID)
#endif

#if !defined GYM_AREA_SIZE
    #define GYM_AREA_SIZE (1.5)
#endif

// How many of each kind of equipment a gym can hold. The data of every piece
// lives in a plain array rather than inside the pool, and an array has to be
// given its size when it is declared.
#if !defined MAX_GYM_EQUIPMENT
    #define MAX_GYM_EQUIPMENT (128)
#endif

// Barbell, dumbell, treadmill and bike. Kept as a define rather than a member
// of E_GYM_EQUIPMENT_TYPE so that nothing which is not equipment can be passed
// where equipment is expected.
#define GYM_EQUIPMENT_TYPES (4)

// What the prompt says, and the key token that goes into it. Changing
// GYM_ENTER_KEY means changing this to match: the game draws whatever key the
// player has bound to the action named here.
#if !defined GYM_ENTER_KEY_TEXT
    #define GYM_ENTER_KEY_TEXT "~k~~VEHICLE_ENTER_EXIT~"
#endif

#if !defined GYM_BARBELL_POPUP
    #define GYM_BARBELL_POPUP "Press %s to use the bench press"
#endif

#if !defined GYM_DUMBELL_POPUP
    #define GYM_DUMBELL_POPUP "Press %s to pick up the dumbbells"
#endif

#if !defined GYM_TREADMILL_POPUP
    #define GYM_TREADMILL_POPUP "Press %s to use the treadmill"
#endif

#if !defined GYM_BIKE_POPUP
    #define GYM_BIKE_POPUP "Press %s to use the exercise bike"
#endif

#if !defined GYM_UPDATE_TIME
    #define GYM_UPDATE_TIME (500)
#endif

#if !defined GYM_POWER_GAIN
    #define GYM_POWER_GAIN (8.0)
#endif

#if !defined GYM_POWER_DECAY
    #define GYM_POWER_DECAY (4.0)
#endif

#if !defined GYM_POWER_MAX
    #define GYM_POWER_MAX (100.0)
#endif

// Weight of a lifting equipment, in kilograms, and the weight somebody at the
// top of their strength handles without breaking a sweat.
// The two animations each lifting equipment has: one where the character
// strains and one where the weight goes up smoothly. Swap them if the pair
// reads the other way around in game.
#if !defined GYM_BARBELL_ANIMATION_HARD
    #define GYM_BARBELL_ANIMATION_HARD "GYM_BP_UP_B"
#endif

#if !defined GYM_BARBELL_ANIMATION_EASY
    #define GYM_BARBELL_ANIMATION_EASY "GYM_BP_UP_A"
#endif

// The game keeps a third animation of each movement for a weight that is
// nothing to the character, and it is the one the single player game runs when
// a trained body picks up something light.
#if !defined GYM_BARBELL_ANIMATION_SMOOTH
    #define GYM_BARBELL_ANIMATION_SMOOTH "GYM_BP_UP_SMOOTH"
#endif

#if !defined GYM_DUMBELL_ANIMATION_HARD
    #define GYM_DUMBELL_ANIMATION_HARD "GYM_FREE_B"
#endif

#if !defined GYM_DUMBELL_ANIMATION_EASY
    #define GYM_DUMBELL_ANIMATION_EASY "GYM_FREE_A"
#endif

#if !defined GYM_DUMBELL_ANIMATION_SMOOTH
    #define GYM_DUMBELL_ANIMATION_SMOOTH "GYM_FREE_UP_SMOOTH"
#endif

#if !defined GYM_REPETITION_SPEED
    #define GYM_REPETITION_SPEED (4.1)
#endif

// The heaviest a player at GYM_POWER_MAX of strength still gets up, in
// kilograms, and what every weight is measured against. The bar is what both
// arms push together while lying down, a dumbell is what one hand carries on
// its own, so the two are nowhere near the same number: 140 kg is a very strong
// bench press, 140 kg in one hand is nothing anybody lifts.
#if !defined GYM_BARBELL_WEIGHT_AT_FULL_STRENGTH
    #define GYM_BARBELL_WEIGHT_AT_FULL_STRENGTH (140.0)
#endif

#if !defined GYM_DUMBELL_WEIGHT_AT_FULL_STRENGTH
    #define GYM_DUMBELL_WEIGHT_AT_FULL_STRENGTH (45.0)
#endif

// What equipment is loaded with until the gamemode says otherwise.
#if !defined GYM_BARBELL_DEFAULT_WEIGHT
    #define GYM_BARBELL_DEFAULT_WEIGHT (60.0)
#endif

#if !defined GYM_DUMBELL_DEFAULT_WEIGHT
    #define GYM_DUMBELL_DEFAULT_WEIGHT (12.0)
#endif

// How much slower or faster the repetitions get, at the ends of the range.
#if !defined GYM_EFFORT_MIN
    #define GYM_EFFORT_MIN (0.3)
#endif

#if !defined GYM_EFFORT_MAX
    #define GYM_EFFORT_MAX (2.5)
#endif

// Where the weight stops being comfortable. Below this much effort the player
// is working near their limit, which is what the straining animations are for.
#if !defined GYM_EFFORT_HARD
    #define GYM_EFFORT_HARD (1.35)
#endif

// Effort at which the weight stops being work at all: above this the movement
// runs clean instead of merely not straining.
#if !defined GYM_EFFORT_SMOOTH
    #define GYM_EFFORT_SMOOTH (2.0)
#endif

// A repetition too heavy for the player can be dropped. Nothing is ever dropped
// while the weight asks for less than GYM_FAIL_THRESHOLD of the strength of the
// player, nothing goes up at all once it asks for more than GYM_FAIL_LIMIT of
// it, and in between the chance climbs up to GYM_FAIL_CHANCE_MAX.
#if !defined GYM_FAIL_THRESHOLD
    #define GYM_FAIL_THRESHOLD (0.9)
#endif

#if !defined GYM_FAIL_LIMIT
    #define GYM_FAIL_LIMIT (1.1)
#endif

#if !defined GYM_FAIL_CHANCE_MAX
    #define GYM_FAIL_CHANCE_MAX (0.75)
#endif

// How much of the bar goes away with a dropped repetition and how long the
// player is left catching their breath before pushing again.
#if !defined GYM_FAIL_POWER_LOSS
    #define GYM_FAIL_POWER_LOSS (60.0)
#endif

#if !defined GYM_FAIL_REST_TIME
    #define GYM_FAIL_REST_TIME (2)
#endif

#if !defined GYM_REPETITION_POWER
    #define GYM_REPETITION_POWER (90.0)
#endif

// How many repetitions the muscle has in it before it gives out: under a weight
// that is everything the player has, and under one that is nothing to them.
// Anything between the two comes from the effort, so the same bar is a set of
// six for whoever can barely lift it and a set of fifty for whoever cannot feel
// it. This is what stops a player repeating forever.
#if !defined GYM_REPETITIONS_AT_LIMIT
    #define GYM_REPETITIONS_AT_LIMIT (6)
#endif

#if !defined GYM_REPETITIONS_AT_EASE
    #define GYM_REPETITIONS_AT_EASE (50)
#endif

// Time the weights stay on the floor once the muscle has given out. Nobody goes
// straight back under a bar they could not lift, and a set that ends in failure
// is the one that earns the longest rest of all. Set it to 0 for a gym where
// the player is the one who decides that.
//
// It is written in seconds because that is what somebody waits, but it is the
// same thing the bar on the screen is showing: these thirty seconds are thirty
// of the hundred the muscle takes to come back at the recovery below. Move one
// of the two and the other is worth moving with it, or the player is told to
// wait for something the bar says already happened.
#if !defined GYM_MUSCLE_REST_TIME
    #define GYM_MUSCLE_REST_TIME (30)
#endif

// The muscle comes back this much every second the player is off the equipment.
// A muscle taken all the way out is a hundred of these, so a minute and a half
// of standing around, and it is multiplied by what SetPlayerGymRecovery says
// about the player, which is where a gamemode puts whatever it sells or trains.
#if !defined GYM_FATIGUE_RECOVERY
    #define GYM_FATIGUE_RECOVERY (1.0)
#endif

// Standing with the weight in hand and moving nothing is not a rest, it is a
// plank: the muscle goes on spending this much a second, and a bar held at the
// chest costs more than dumbbells held at the sides. From a fresh muscle that
// is around twenty five seconds of holding a bar and forty of holding
// dumbbells, and rather less after a set that already spent most of it.
#if !defined GYM_BARBELL_HOLD_COST
    #define GYM_BARBELL_HOLD_COST (4.0)
#endif

#if !defined GYM_DUMBELL_HOLD_COST
    #define GYM_DUMBELL_HOLD_COST (2.5)
#endif

// What is left of that under a weight that is nothing to the player. Holding
// something light is easier, never free: an arm held out is an arm held out.
#if !defined GYM_HOLD_EASE
    #define GYM_HOLD_EASE (0.7)
#endif

// Time without a press before holding starts counting.
#if !defined GYM_HOLD_TIME
    #define GYM_HOLD_TIME (1000)
#endif

// How many times the muscle can be taken all the way to failure before the day
// is over, for a body with no training at all and for one with all of it.
// Nobody spends the whole day in a gym, and the strong last longer than the
// rest before they have to stop.
#if !defined GYM_FAILURES_UNTRAINED
    #define GYM_FAILURES_UNTRAINED (4)
#endif

#if !defined GYM_FAILURES_TRAINED
    #define GYM_FAILURES_TRAINED (10)
#endif

// What comes back of the day every second off the equipment. This is the long
// rest, nothing like the one between two sets.
#if !defined GYM_EXHAUSTION_RECOVERY
    #define GYM_EXHAUSTION_RECOVERY (0.15)
#endif

// Time between the weight coming down for the last time and the player leaving
// the equipment, once the muscle is spent.
#if !defined GYM_FAILURE_TIME
    #define GYM_FAILURE_TIME (1200)
#endif

#define GYM_FATIGUE_MAX (100.0)

// A machine nobody is pushing any more does not keep its speed: once this long
// goes by without a press it comes down at GYM_COAST_DECAY instead of the
// ordinary decay, so the player is not left waiting for the wheel to stop.
#if !defined GYM_COAST_TIME
    #define GYM_COAST_TIME (1000)
#endif

#if !defined GYM_COAST_DECAY
    #define GYM_COAST_DECAY (18.0)
#endif

// The fastest a machine can still be going for the player to step off it.
#if !defined GYM_DISMOUNT_SPEED
    #define GYM_DISMOUNT_SPEED (GYM_SPEED_SLOW)
#endif

// A treadmill is the one machine that does not refuse. The belt is under the
// player rather than around their feet, so leaving at speed is not a decision
// they get to make politely: it throws them off, which is what GYM_TREAD_FALLOFF
// and GYM_WALK_FALLOFF are for. Turn this off to have it hold them like the bike
// does until the belt has slowed down.
#if !defined GYM_TREADMILL_FALL_OFF
    #define GYM_TREADMILL_FALL_OFF (true)
#endif

#if !defined GYM_SPEED_SLOW_POWER
    #define GYM_SPEED_SLOW_POWER (5.0)
#endif

#if !defined GYM_SPEED_MEDIUM_POWER
    #define GYM_SPEED_MEDIUM_POWER (40.0)
#endif

#if !defined GYM_SPEED_FAST_POWER
    #define GYM_SPEED_FAST_POWER (90.0)
#endif

#if !defined GYM_ABSENT_TIMEOUT
    #define GYM_ABSENT_TIMEOUT (5)
#endif

#if !defined GYM_REPETITION_GRACE
    #define GYM_REPETITION_GRACE (3)
#endif

// How high above the equipment the player is placed. On the bench press this
// is what decides whether the bar sits over the chest or over the face.
#if !defined GYM_BARBELL_PLAYER_HEIGHT
    #define GYM_BARBELL_PLAYER_HEIGHT (1.0)
#endif

#if !defined GYM_DUMBELL_PLAYER_HEIGHT
    #define GYM_DUMBELL_PLAYER_HEIGHT (0.0)
#endif

#if !defined GYM_TREADMILL_PLAYER_HEIGHT
    #define GYM_TREADMILL_PLAYER_HEIGHT (1.0)
#endif

#if !defined GYM_BIKE_PLAYER_HEIGHT
    #define GYM_BIKE_PLAYER_HEIGHT (1.0)
#endif

#if !defined GYM_ATTACH_SLOT_PRIMARY
    #define GYM_ATTACH_SLOT_PRIMARY (0)
#endif

#if !defined GYM_ATTACH_SLOT_SECONDARY
    #define GYM_ATTACH_SLOT_SECONDARY (1)
#endif

/**
 * # Timings
 */

#define GYM_CAMERA_MODE_CUT (0)
#define GYM_CAMERA_MODE_INTERPOLATE (1)
#define GYM_CAMERA_MODE_ORBIT (2)

#if !defined GYM_CAMERA_MODE
    #define GYM_CAMERA_MODE GYM_CAMERA_MODE_CUT
#endif

// The key that gets a player on and off. It is also the key button.inc is told
// to watch, so changing it here changes both.
#if !defined GYM_ENTER_KEY
    #define GYM_ENTER_KEY (KEY_SECONDARY_ATTACK)
#endif

#if !defined GYM_CAMERA_KEY
    #define GYM_CAMERA_KEY (KEY_CROUCH)
#endif

#if !defined GYM_CAMERA_STEP_ANGLE
    #define GYM_CAMERA_STEP_ANGLE (90.0)
#endif

#if !defined GYM_CAMERA_STEPS
    #define GYM_CAMERA_STEPS (4)
#endif

#if !defined GYM_CAMERA_HOLD_SPEED
    #define GYM_CAMERA_HOLD_SPEED (120.0)
#endif

#if !defined GYM_CAMERA_HOLD_TICK
    #define GYM_CAMERA_HOLD_TICK (40)
#endif

#if !defined GYM_CAMERA_MOVE_TIME
    #define GYM_CAMERA_MOVE_TIME (400)
#endif

#if !defined GYM_CAMERA_MOVE_TICK
    #define GYM_CAMERA_MOVE_TICK (40)
#endif

#if !defined GYM_CAMERA_MOVE_SEGMENTS
    #if GYM_CAMERA_MOVE_TICK && GYM_CAMERA_MOVE_TIME > GYM_CAMERA_MOVE_TICK
        #define GYM_CAMERA_MOVE_SEGMENTS (GYM_CAMERA_MOVE_TIME / GYM_CAMERA_MOVE_TICK)
    #else
        #define GYM_CAMERA_MOVE_SEGMENTS (1)
    #endif
#endif

#if !defined GYM_CAMERA_LOOK_AT_HEIGHT
    #define GYM_CAMERA_LOOK_AT_HEIGHT (0.5)
#endif

#if !defined GYM_BARBELL_CAMERA_DISTANCE
    #define GYM_BARBELL_CAMERA_DISTANCE (3.0)
#endif

#if !defined GYM_BARBELL_CAMERA_HEIGHT
    #define GYM_BARBELL_CAMERA_HEIGHT (3.0)
#endif

#if !defined GYM_BARBELL_CAMERA_ANGLE
    #define GYM_BARBELL_CAMERA_ANGLE (0.0)
#endif

#if !defined GYM_DUMBELL_CAMERA_DISTANCE
    #define GYM_DUMBELL_CAMERA_DISTANCE (3.0)
#endif

#if !defined GYM_DUMBELL_CAMERA_HEIGHT
    #define GYM_DUMBELL_CAMERA_HEIGHT (2.0)
#endif

#if !defined GYM_DUMBELL_CAMERA_ANGLE
    #define GYM_DUMBELL_CAMERA_ANGLE (180.0)
#endif

#if !defined GYM_TREADMILL_CAMERA_DISTANCE
    #define GYM_TREADMILL_CAMERA_DISTANCE (3.0)
#endif

#if !defined GYM_TREADMILL_CAMERA_HEIGHT
    #define GYM_TREADMILL_CAMERA_HEIGHT (3.0)
#endif

#if !defined GYM_TREADMILL_CAMERA_ANGLE
    #define GYM_TREADMILL_CAMERA_ANGLE (0.0)
#endif

#if !defined GYM_BIKE_CAMERA_DISTANCE
    #define GYM_BIKE_CAMERA_DISTANCE (3.0)
#endif

#if !defined GYM_BIKE_CAMERA_HEIGHT
    #define GYM_BIKE_CAMERA_HEIGHT (3.0)
#endif

#if !defined GYM_BIKE_CAMERA_ANGLE
    #define GYM_BIKE_CAMERA_ANGLE (0.0)
#endif

#if !defined GYM_BARBELL_GET_ON_TIME
    #define GYM_BARBELL_GET_ON_TIME (4500)
#endif

#if !defined GYM_BARBELL_GET_OFF_TIME
    #define GYM_BARBELL_GET_OFF_TIME (2500)
#endif

#if !defined GYM_BARBELL_FINISH_TIME
    #define GYM_BARBELL_FINISH_TIME (5000)
#endif

#if !defined GYM_BARBELL_LOWER_TIME
    #define GYM_BARBELL_LOWER_TIME (2000)
#endif

#if !defined GYM_DUMBELL_GET_ON_TIME
    #define GYM_DUMBELL_GET_ON_TIME (2200)
#endif

#if !defined GYM_DUMBELL_GET_OFF_TIME
    #define GYM_DUMBELL_GET_OFF_TIME (1000)
#endif

#if !defined GYM_DUMBELL_FINISH_TIME
    #define GYM_DUMBELL_FINISH_TIME (2000)
#endif

#if !defined GYM_DUMBELL_LOWER_TIME
    #define GYM_DUMBELL_LOWER_TIME (1500)
#endif

#if !defined GYM_TREADMILL_GET_ON_TIME
    #define GYM_TREADMILL_GET_ON_TIME (2100)
#endif

#if !defined GYM_TREADMILL_GET_OFF_TIME
    #define GYM_TREADMILL_GET_OFF_TIME (3400)
#endif

#if !defined GYM_TREADMILL_FALL_OFF_TIME
    #define GYM_TREADMILL_FALL_OFF_TIME (2100)
#endif

#if !defined GYM_BIKE_GET_ON_TIME
    #define GYM_BIKE_GET_ON_TIME (1600)
#endif

#if !defined GYM_BIKE_GET_OFF_TIME
    #define GYM_BIKE_GET_OFF_TIME (1600)
#endif

#if !defined GYM_MODEL_BENCH
    #define GYM_MODEL_BENCH (2629)
#endif

/**
 * # Models
 */

#define GYM_MODEL_BARBELL               (2913)
#define GYM_MODEL_DUMBELL_LEFT          (3072)
#define GYM_MODEL_DUMBELL_RIGHT         (3071)
#define GYM_MODEL_TREADMILL             (2627)
#define GYM_MODEL_BIKE                  (2630)

/**
 * # Animation indexes
 */

#define GYM_ANIM_BARBELL_LOWER          (47)
#define GYM_ANIM_BARBELL_GET_OFF        (48)
#define GYM_ANIM_BARBELL_GET_ON         (49)

#define GYM_ANIM_DUMBELL_LOWER          (573)
#define GYM_ANIM_DUMBELL_GET_ON         (575)
#define GYM_ANIM_DUMBELL_GET_OFF        (576)

#define GYM_ANIM_TREADMILL_GET_OFF      (660)
#define GYM_ANIM_TREADMILL_JOG          (662)
#define GYM_ANIM_TREADMILL_SPRINT       (663)
#define GYM_ANIM_TREADMILL_TIRED        (664)
#define GYM_ANIM_TREADMILL_WALK         (665)

#define GYM_ANIM_BIKE_FAST              (649)
#define GYM_ANIM_BIKE_GET_OFF           (651)
#define GYM_ANIM_BIKE_PEDAL             (653)
#define GYM_ANIM_BIKE_SLOW              (654)
#define GYM_ANIM_BIKE_STILL             (655)

#define INVALID_BARBELL_ID              (Barbell:-1)
#define INVALID_DUMBELL_ID              (Dumbell:-1)
#define INVALID_TREADMILL_ID            (Treadmill:-1)
#define INVALID_BIKE_ID                 (Bike:-1)

enum E_GYM_EQUIPMENT_TYPE {
    GYM_EQUIPMENT_NONE = -1,
    GYM_EQUIPMENT_BARBELL,
    GYM_EQUIPMENT_DUMBELL,
    GYM_EQUIPMENT_TREADMILL,
    GYM_EQUIPMENT_BIKE
};

enum E_GYM_SPEED_LEVEL {
    GYM_SPEED_IDLE,
    GYM_SPEED_SLOW,
    GYM_SPEED_MEDIUM,
    GYM_SPEED_FAST
};

enum E_GYM_EXERCISE_STATE {
    GYM_EXERCISE_NONE,
    GYM_EXERCISE_GETTING_ON,
    GYM_EXERCISE_ACTIVE,
    GYM_EXERCISE_GETTING_OFF
};

static enum _:E_GYM_EQUIPMENT_DATA {
    bool:E_GYM_EQUIPMENT_CREATED,

    Float:E_GYM_EQUIPMENT_X,
    Float:E_GYM_EQUIPMENT_Y,
    Float:E_GYM_EQUIPMENT_Z,
    Float:E_GYM_EQUIPMENT_ANGLE,
    Float:E_GYM_EQUIPMENT_SIZE,

    Float:E_GYM_EQUIPMENT_WEIGHT,
    bool:E_GYM_EQUIPMENT_ENABLED,

    Float:E_GYM_EQUIPMENT_CAMERA_DISTANCE,
    Float:E_GYM_EQUIPMENT_CAMERA_HEIGHT,
    Float:E_GYM_EQUIPMENT_CAMERA_ANGLE,
    E_GYM_EQUIPMENT_CAMERA_STEPS,
    Float:E_GYM_EQUIPMENT_CAMERA_STEP_ANGLE,

    E_GYM_EQUIPMENT_WORLD_ID,
    E_GYM_EQUIPMENT_INTERIOR_ID,
    E_GYM_EQUIPMENT_PLAYER_ID,

    Button:E_GYM_EQUIPMENT_BUTTON_ID,
    E_GYM_EQUIPMENT_FRAME_ID,
    E_GYM_EQUIPMENT_PROP_ID[2]
};

static enum _:E_GYM_PLAYER_DATA {
    E_GYM_EQUIPMENT_TYPE:E_GYM_PLAYER_TYPE,
    E_GYM_PLAYER_INDEX,
    E_GYM_EXERCISE_STATE:E_GYM_PLAYER_STATE,

    Float:E_GYM_PLAYER_POWER,
    E_GYM_PLAYER_METRIC,

    // What the muscle has spent and when it was last put down, which is all the
    // recovery needs: nothing has to tick for a player who is not in a gym.
    Float:E_GYM_PLAYER_FATIGUE,
    E_GYM_PLAYER_FATIGUE_TIME,

    // The same over a day instead of over a set: every muscle taken to failure
    // is part of this one, and it is what says the player is done training.
    Float:E_GYM_PLAYER_EXHAUSTION,

    // Set the moment the muscle gives out and cleared by the next set, so the
    // weight going down and the player leaving happen once between them.
    bool:E_GYM_PLAYER_SPENT,

    // What the player is worth at resting, fed from the outside like strength.
    Float:E_GYM_PLAYER_RECOVERY,

    // Fed from the outside, by whoever keeps how strong the player is.
    Float:E_GYM_PLAYER_STRENGTH,
    E_GYM_PLAYER_REST_UNTIL,
    E_GYM_PLAYER_LOWER_TIME,

    E_GYM_PLAYER_LAST_ACTIVITY,
    E_GYM_PLAYER_LAST_INPUT,

    E_GYM_PLAYER_TIMER,
    E_GYM_PLAYER_SESSION,

    bool:E_GYM_PLAYER_CAMERA_MOVED,
    Float:E_GYM_PLAYER_CAMERA_ANGLE,

    E_GYM_PLAYER_CAMERA_TIMER,
    E_GYM_PLAYER_CAMERA_SEGMENT,
    E_GYM_PLAYER_CAMERA_SEGMENTS,
    Float:E_GYM_PLAYER_CAMERA_FROM,
    Float:E_GYM_PLAYER_CAMERA_DELTA,
    E_GYM_PLAYER_CAMERA_MOVE_END
};

// What a piece of equipment is made of lives in a plain Pawn array, because
// reading a cell out of one is a single instruction while a plugin container
// answers the same question through a call out and back, and these are read on
// every tick of every exercise.
//
// The array is also what hands the indexes out, one free list per kind of
// equipment: a slot given back points at the one given back before it, and
// slots never used yet come off a high water mark. See the note in button.inc
// for what this was measured against.
static
    gGymEquipmentData[GYM_EQUIPMENT_TYPES][MAX_GYM_EQUIPMENT][E_GYM_EQUIPMENT_DATA],
    gGymEquipmentNextFree[GYM_EQUIPMENT_TYPES][MAX_GYM_EQUIPMENT],
    gGymEquipmentFreeHead[GYM_EQUIPMENT_TYPES] = { -1, ... },
    gGymEquipmentHighWater[GYM_EQUIPMENT_TYPES],
    gGymEquipmentCount[GYM_EQUIPMENT_TYPES]
;

// Which piece of equipment a button belongs to, the other way round. A press
// arrives holding a button and this is what turns it back into equipment, in
// one array read: extra data would answer the same question through a map of
// strings, which is a call out to the plugin on a path a player hits every
// time they walk up to something.
static enum _:E_GYM_BUTTON_DATA {
    bool:E_GYM_BUTTON_OWNED,
    E_GYM_EQUIPMENT_TYPE:E_GYM_BUTTON_TYPE,
    E_GYM_BUTTON_INDEX
};

static
    gGymButtonData[MAX_BUTTONS][E_GYM_BUTTON_DATA]
;

static
    gGymPlayerData[MAX_PLAYERS][E_GYM_PLAYER_DATA],
    gGymSessionCounter
;

/**
 * # Functions
 */

/**
 * Creates an equipment: the objects it is made of, plus the dynamic area a
 * player has to stand in to use it.
 *
 * @param x, y, z     Spot the player is placed on while using the equipment.
 * @param angle       Facing of the equipment, in degrees.
 * @param size        Radius of the dynamic area created around it.
 * @param worldid     Virtual world of the area, -1 for every world. It only
 *                    reaches the objects with GYM_STREAMER_OBJECTS defined,
 *                    since native objects are visible everywhere.
 * @param interiorid  Interior of the area, -1 for every interior, with the
 *                    same reach as worldid.
 * @param priority    Beats overlapping gym areas created with a lower value.
 * @param cameraDistance  How far behind the equipment the camera sits, 0.0 or
 *                    less leaves the camera of the player untouched.
 * @param cameraHeight    How high above the equipment the camera sits.
 * @param cameraAngle     Rotates the camera around the equipment, in degrees:
 *                    0.0 stays behind it, 90.0 goes to its side, 180.0 puts
 *                    the camera in front of it.
 *
 * @return Id of the new equipment, or the invalid id of its own type.
 */
forward Barbell:CreateBarbell(Float:x, Float:y, Float:z, Float:angle, Float:size = GYM_AREA_SIZE, worldid = -1, interiorid = -1, priority = 0, Float:cameraDistance = GYM_BARBELL_CAMERA_DISTANCE, Float:cameraHeight = GYM_BARBELL_CAMERA_HEIGHT, Float:cameraAngle = GYM_BARBELL_CAMERA_ANGLE);
forward Dumbell:CreateDumbell(Float:x, Float:y, Float:z, Float:angle, Float:size = GYM_AREA_SIZE, worldid = -1, interiorid = -1, priority = 0, Float:cameraDistance = GYM_DUMBELL_CAMERA_DISTANCE, Float:cameraHeight = GYM_DUMBELL_CAMERA_HEIGHT, Float:cameraAngle = GYM_DUMBELL_CAMERA_ANGLE);
forward Treadmill:CreateTreadmill(Float:x, Float:y, Float:z, Float:angle, Float:size = GYM_AREA_SIZE, worldid = -1, interiorid = -1, priority = 0, Float:cameraDistance = GYM_TREADMILL_CAMERA_DISTANCE, Float:cameraHeight = GYM_TREADMILL_CAMERA_HEIGHT, Float:cameraAngle = GYM_TREADMILL_CAMERA_ANGLE);
forward Bike:CreateBike(Float:x, Float:y, Float:z, Float:angle, Float:size = GYM_AREA_SIZE, worldid = -1, interiorid = -1, priority = 0, Float:cameraDistance = GYM_BIKE_CAMERA_DISTANCE, Float:cameraHeight = GYM_BIKE_CAMERA_HEIGHT, Float:cameraAngle = GYM_BIKE_CAMERA_ANGLE);

/**
 * Destroys the equipment, its objects and its dynamic area. A player who is
 * exercising on it is released before it goes away.
 *
 * @param barbellid|dumbellid|treadmillid|bikeid  Equipment to destroy.
 *
 * @return True when the equipment existed.
 */
forward bool:DestroyBarbell(Barbell:barbellid);
forward bool:DestroyDumbell(Dumbell:dumbellid);
forward bool:DestroyTreadmill(Treadmill:treadmillid);
forward bool:DestroyBike(Bike:bikeid);

/**
 * Tells whether the id still points to an existing equipment of that type.
 *
 * @param barbellid|dumbellid|treadmillid|bikeid  Equipment to check.
 *
 * @return True when the equipment exists.
 */
forward bool:IsValidBarbell(Barbell:barbellid);
forward bool:IsValidDumbell(Dumbell:dumbellid);
forward bool:IsValidTreadmill(Treadmill:treadmillid);
forward bool:IsValidBike(Bike:bikeid);

/**
 * Reads back the position the equipment was created with.
 *
 * @param barbellid|dumbellid|treadmillid|bikeid  Equipment to read.
 * @param x, y, z     Filled with the position of the equipment.
 * @param angle       Filled with the facing of the equipment.
 *
 * @return True when the equipment exists.
 */
forward bool:GetBarbellPos(Barbell:barbellid, &Float:x, &Float:y, &Float:z, &Float:angle);
forward bool:GetDumbellPos(Dumbell:dumbellid, &Float:x, &Float:y, &Float:z, &Float:angle);
forward bool:GetTreadmillPos(Treadmill:treadmillid, &Float:x, &Float:y, &Float:z, &Float:angle);
forward bool:GetBikePos(Bike:bikeid, &Float:x, &Float:y, &Float:z, &Float:angle);

/**
 * Moves the camera used by the next player who gets on the equipment, which is
 * how a spot with a wall behind it is worked around. It does not move the
 * camera of somebody already exercising.
 *
 * @param barbellid|dumbellid|treadmillid|bikeid  Equipment to change.
 * @param distance    How far behind the equipment the camera sits, 0.0 or less
 *                    leaves the camera of the player untouched.
 * @param height      How high above the equipment the camera sits.
 * @param angle       Rotates the camera around the equipment, in degrees.
 * @param steps       How many views the player can cycle through with
 *                    GYM_CAMERA_KEY. 1 leaves the camera fixed.
 * @param stepAngle   Degrees between two of those views.
 *
 * @return True when the equipment exists.
 */
forward bool:SetBarbellCamera(Barbell:barbellid, Float:distance, Float:height, Float:angle, steps = GYM_CAMERA_STEPS, Float:stepAngle = GYM_CAMERA_STEP_ANGLE);
forward bool:SetDumbellCamera(Dumbell:dumbellid, Float:distance, Float:height, Float:angle, steps = GYM_CAMERA_STEPS, Float:stepAngle = GYM_CAMERA_STEP_ANGLE);
forward bool:SetTreadmillCamera(Treadmill:treadmillid, Float:distance, Float:height, Float:angle, steps = GYM_CAMERA_STEPS, Float:stepAngle = GYM_CAMERA_STEP_ANGLE);
forward bool:SetBikeCamera(Bike:bikeid, Float:distance, Float:height, Float:angle, steps = GYM_CAMERA_STEPS, Float:stepAngle = GYM_CAMERA_STEP_ANGLE);

/**
 * Reads back the camera the equipment is configured with.
 *
 * @param barbellid|dumbellid|treadmillid|bikeid  Equipment to read.
 * @param distance    Filled with the distance of the camera.
 * @param height      Filled with the height of the camera.
 * @param angle       Filled with the rotation of the camera.
 * @param steps       Filled with how many views the cycle has.
 * @param stepAngle   Filled with the degrees between two views.
 *
 * @return True when the equipment exists.
 */
forward bool:GetBarbellCamera(Barbell:barbellid, &Float:distance, &Float:height, &Float:angle, &steps = 0, &Float:stepAngle = 0.0);
forward bool:GetDumbellCamera(Dumbell:dumbellid, &Float:distance, &Float:height, &Float:angle, &steps = 0, &Float:stepAngle = 0.0);
forward bool:GetTreadmillCamera(Treadmill:treadmillid, &Float:distance, &Float:height, &Float:angle, &steps = 0, &Float:stepAngle = 0.0);
forward bool:GetBikeCamera(Bike:bikeid, &Float:distance, &Float:height, &Float:angle, &steps = 0, &Float:stepAngle = 0.0);

/**
 * Sets how heavy the equipment is, in kilograms, which is what makes one bench
 * press harder than the next one. What the weight means depends on the type: a
 * player at GYM_POWER_MAX of strength is at their very limit on a bar of
 * GYM_BARBELL_WEIGHT_AT_FULL_STRENGTH and on dumbells of
 * GYM_DUMBELL_WEIGHT_AT_FULL_STRENGTH each. Closing on that limit the
 * repetitions come slower and start being dropped; past it nothing goes up.
 * Running equipment has no weight.
 *
 * @param barbellid|dumbellid|treadmillid|bikeid  Equipment to change.
 * @param weight      Weight in kilograms.
 *
 * @return True when the equipment exists and the weight is not negative.
 */
forward bool:SetBarbellWeight(Barbell:barbellid, Float:weight);
forward bool:SetDumbellWeight(Dumbell:dumbellid, Float:weight);
forward bool:SetTreadmillWeight(Treadmill:treadmillid, Float:weight);
forward bool:SetBikeWeight(Bike:bikeid, Float:weight);

/**
 * Gives the weight of the equipment, in kilograms.
 *
 * @param barbellid|dumbellid|treadmillid|bikeid  Equipment to read.
 *
 * @return The weight, 0.0 when the equipment does not exist.
 */
forward Float:GetBarbellWeight(Barbell:barbellid);
forward Float:GetDumbellWeight(Dumbell:dumbellid);
forward Float:GetTreadmillWeight(Treadmill:treadmillid);
forward Float:GetBikeWeight(Bike:bikeid);

/**
 * Takes an equipment out of service, or puts it back. A disabled equipment
 * keeps its objects and its area, it simply refuses players; whoever is already
 * on it stays until they leave.
 *
 * @param barbellid|dumbellid|treadmillid|bikeid  Equipment to change.
 * @param enabled     False to take it out of service.
 *
 * @return True when the equipment exists.
 */
forward bool:SetBarbellEnabled(Barbell:barbellid, bool:enabled);
forward bool:SetDumbellEnabled(Dumbell:dumbellid, bool:enabled);
forward bool:SetTreadmillEnabled(Treadmill:treadmillid, bool:enabled);
forward bool:SetBikeEnabled(Bike:bikeid, bool:enabled);

/**
 * Tells whether the equipment is in service.
 *
 * @param barbellid|dumbellid|treadmillid|bikeid  Equipment to check.
 *
 * @return True while players are allowed on it.
 */
forward bool:IsBarbellEnabled(Barbell:barbellid);
forward bool:IsDumbellEnabled(Dumbell:dumbellid);
forward bool:IsTreadmillEnabled(Treadmill:treadmillid);
forward bool:IsBikeEnabled(Bike:bikeid);

/**
 * Reads the weight, or the strength that weight asks for, of any equipment.
 * These take the type and the index the callbacks hand out, so a library that
 * keeps how strong a player is can answer without knowing the tags.
 *
 * @param type        Type of the equipment.
 * @param index       Id of the equipment inside its type.
 *
 * @return The weight in kilograms, or the strength needed to lift it, 0.0 for
 *         equipment that is not lifted.
 */
forward Float:GetGymEquipmentWeight(E_GYM_EQUIPMENT_TYPE:type, index);

/**
 * The weight and the service state of any equipment, again by type and index,
 * for code that holds no tag: a command, a menu, a database load.
 *
 * @param type        Type of the equipment.
 * @param index       Id of the equipment inside its type.
 * @param weight      Weight in kilograms.
 * @param enabled     False to take the equipment out of service.
 *
 * @return True when the equipment exists.
 */
forward bool:SetGymEquipmentWeight(E_GYM_EQUIPMENT_TYPE:type, index, Float:weight);
forward bool:SetGymEquipmentEnabled(E_GYM_EQUIPMENT_TYPE:type, index, bool:enabled);
forward bool:IsGymEquipmentEnabled(E_GYM_EQUIPMENT_TYPE:type, index);
forward Float:GetGymEquipmentRequiredStrength(E_GYM_EQUIPMENT_TYPE:type, index);

/**
 * Tells whether the weight on the equipment is inside what the player can still
 * get up. Everything above it is dropped on every single repetition, so this is
 * what to answer OnPlayerRequestExercise with to keep players off weights they
 * have no business under. Equipment with no weight is always within reach.
 *
 * @param playerid    Player to measure.
 * @param type        Type of the equipment.
 * @param index       Id of the equipment inside its type.
 *
 * @return True when the player can get the weight up at all.
 */
forward bool:CanPlayerLiftGymEquipment(playerid, E_GYM_EQUIPMENT_TYPE:type, index);

/**
 * Gives how fast the machine the player is on is going, which is what the
 * running animations are picked by. Equipment with a weight has no speed.
 *
 * @param playerid    Player to read.
 *
 * @return GYM_SPEED_IDLE, GYM_SPEED_SLOW, GYM_SPEED_MEDIUM or GYM_SPEED_FAST.
 */
forward E_GYM_SPEED_LEVEL:GetPlayerExerciseSpeed(playerid);

/**
 * Tells whether the player can step off what they are on right now. A machine
 * running faster than GYM_DISMOUNT_SPEED has to be brought down first, which is
 * what the key to get off does nothing about until then.
 *
 * @param playerid    Player to read.
 *
 * @return True when the player is exercising and can get off.
 */
forward bool:CanPlayerStopExercise(playerid);

/**
 * Gives how hard the equipment the player is on is for them: the strength of
 * the player over the strength its weight asks for. Above GYM_EFFORT_HARD the
 * weight is comfortable, below it the player is straining, at 1.0 it is the
 * most they can carry, and it is bounded by GYM_EFFORT_MIN and GYM_EFFORT_MAX.
 *
 * @param playerid    Player to read.
 *
 * @return The effort of the player, 1.0 when not exercising.
 */
/**
 * Gives how much of the muscle of the player is spent, from 0.0 to
 * GYM_FATIGUE_MAX. It goes up with every repetition, by more the heavier the
 * weight is for them, and comes back at GYM_FATIGUE_RECOVERY a second once they
 * are off the equipment. At the top the muscle gives out and the set ends.
 *
 * @param playerid    Player to read.
 *
 * @return The fatigue, already counting the rest since the last set.
 */
forward Float:GetPlayerGymFatigue(playerid);

/**
 * Writes the fatigue of a player, for a gamemode that keeps it between
 * sessions or hands out a rest of its own.
 *
 * @param playerid    Player to write.
 * @param fatigue     0.0 for a muscle that is fresh, GYM_FATIGUE_MAX for one
 *                    that is spent. Anything outside is brought inside.
 *
 * @return False when the player id is not a player.
 */
forward bool:SetPlayerGymFatigue(playerid, Float:fatigue);

/**
 * Gives how many repetitions of an equipment the player still has in them,
 * which is what the muscle has left over what one of those repetitions costs.
 * Answers for equipment the player is not on, so a gamemode can say what a
 * weight is going to be worth before anyone lies under it.
 *
 * @param playerid    Player to read.
 * @param type        Type of the equipment.
 * @param index       Id of the equipment inside its type.
 *
 * @return Repetitions left, 0 when the muscle is spent.
 */
forward GetGymRepetitionsLeft(playerid, E_GYM_EQUIPMENT_TYPE:type, index);

/**
 * Gives how much of the day of the player is spent, from 0.0 to
 * GYM_FATIGUE_MAX. Every muscle taken all the way to failure is a share of it,
 * and how big that share is comes from their strength: a body with no training
 * has GYM_FAILURES_UNTRAINED of them in it, a trained one
 * GYM_FAILURES_TRAINED. At the top there is no training left in the player
 * whatever weight they stand in front of.
 *
 * @param playerid    Player to read.
 *
 * @return The exhaustion, already counting the rest since the last set.
 */
forward Float:GetPlayerGymExhaustion(playerid);

/**
 * Writes it, for a gamemode that keeps the day between sessions or hands out a
 * rest of its own.
 *
 * @param playerid      Player to write.
 * @param exhaustion    0.0 for a player who has not trained, GYM_FATIGUE_MAX
 *                      for one who is done. Anything outside is brought inside.
 *
 * @return False when the player id is not a player.
 */
forward bool:SetPlayerGymExhaustion(playerid, Float:exhaustion);

/**
 * Gives how many more sets the player has in them before the day is over.
 *
 * @param playerid    Player to read.
 *
 * @return Sets left, 0 when there is no training left in them today.
 */
forward GetPlayerGymSetsLeft(playerid);

/**
 * Gives how many seconds are left of the rest the player owes. A muscle that
 * gave out puts the weights down for GYM_MUSCLE_REST_TIME, and until that is
 * over GetGymRepetitionsLeft answers 0 for everything and nothing goes up.
 *
 * @param playerid    Player to read.
 *
 * @return Seconds left, 0 when the player owes nothing.
 */
forward GetPlayerGymRestTime(playerid);

/**
 * Gives what the player is worth at resting, which multiplies everything that
 * comes back: the muscle between two sets, and the day between two visits.
 *
 * @param playerid    Player to read.
 *
 * @return The multiplier, 1.0 for a player nobody has said anything about.
 */
forward Float:GetPlayerGymRecovery(playerid);

/**
 * Writes it. The library never touches this on its own: how much a player rests
 * is whatever the gamemode decides it is, out of how used to a gym they are,
 * what they are eating, what they were sold, or nothing at all.
 *
 * @param playerid    Player to write.
 * @param recovery    1.0 to rest like everybody else, 2.0 for twice that, 0.0
 *                    to stop resting altogether.
 *
 * @return False when the player id is not a player.
 */
forward bool:SetPlayerGymRecovery(playerid, Float:recovery);

forward Float:GetPlayerExerciseEffort(playerid);

/**
 * Gives the strength the equipment the player is on asks for.
 *
 * @param playerid    Player to read.
 *
 * @return The strength needed, 0.0 when not exercising or not lifting.
 */
forward Float:GetPlayerExerciseRequiredStrength(playerid);

/**
 * Sets how long the current repetition takes before the weight comes back down,
 * which is what a slower animation needs. Only worth calling from inside
 * OnPlayerRepetition, where it lasts for that repetition.
 *
 * @param playerid    Player exercising.
 * @param time        Milliseconds until the weight is lowered.
 *
 * @return True when the player is exercising and the time is positive.
 */
forward bool:SetPlayerRepetitionTime(playerid, time);

/**
 * Tells the library how strong the player is, on the same 0.0 to GYM_POWER_MAX
 * scale the weights are measured against. Players start at the top, so weights
 * mean nothing until something starts feeding this in.
 *
 * @param playerid    Player to change.
 * @param strength    Strength of the player.
 *
 * @return True when the player id is valid.
 */
forward bool:SetPlayerGymStrength(playerid, Float:strength);

/**
 * Gives the strength the library was told the player has.
 *
 * @param playerid    Player to read.
 *
 * @return The strength of the player.
 */
forward Float:GetPlayerGymStrength(playerid);

/**
 * Gives the player currently using the equipment.
 *
 * @param barbellid|dumbellid|treadmillid|bikeid  Equipment to check.
 *
 * @return The playerid using it, INVALID_PLAYER_ID when it is free.
 */
forward GetBarbellPlayer(Barbell:barbellid);
forward GetDumbellPlayer(Dumbell:dumbellid);
forward GetTreadmillPlayer(Treadmill:treadmillid);
forward GetBikePlayer(Bike:bikeid);

/**
 * Tells whether somebody is using the equipment right now.
 *
 * @param barbellid|dumbellid|treadmillid|bikeid  Equipment to check.
 *
 * @return True while a player is on it.
 */
forward bool:IsBarbellOccupied(Barbell:barbellid);
forward bool:IsDumbellOccupied(Dumbell:dumbellid);
forward bool:IsTreadmillOccupied(Treadmill:treadmillid);
forward bool:IsBikeOccupied(Bike:bikeid);

/**
 * Counts how many equipments of that type exist.
 *
 * @return The amount of equipments of the type.
 */
forward CountBarbell();
forward CountDumbell();
forward CountTreadmill();
forward CountBike();

/**
 * Puts the player on the equipment, playing the get on animation, as if the
 * interaction key had been pressed inside its area.
 *
 * @param playerid    Player to put on the equipment.
 * @param barbellid|dumbellid|treadmillid|bikeid  Equipment to be used.
 *
 * @return True when the exercise started, false when the player is already
 *         exercising, the equipment is taken or OnPlayerRequestExercise
 *         refused it.
 */
forward bool:PutPlayerOnBarbell(playerid, Barbell:barbellid);
forward bool:PutPlayerOnDumbell(playerid, Dumbell:dumbellid);
forward bool:PutPlayerOnTreadmill(playerid, Treadmill:treadmillid);
forward bool:PutPlayerOnBike(playerid, Bike:bikeid);

/**
 * Gives the equipment of that type the player is exercising on.
 *
 * @param playerid    Player to check.
 *
 * @return The equipment in use, or the invalid id of the type when the player
 *         is not exercising on one.
 */
forward Barbell:GetPlayerBarbell(playerid);
forward Dumbell:GetPlayerDumbell(playerid);
forward Treadmill:GetPlayerTreadmill(playerid);
forward Bike:GetPlayerBike(playerid);

/**
 * The button an equipment was built on, so a script can change what its prompt
 * says, move it, or hang its own data off it.
 */
forward Button:GetBarbellButton(Barbell:barbellid);
forward Button:GetDumbellButton(Dumbell:dumbellid);
forward Button:GetTreadmillButton(Treadmill:treadmillid);
forward Button:GetBikeButton(Bike:bikeid);

/**
 * Answers which equipment the player is standing at, which is what the key to
 * get on has to know before it can do anything. In a gym where the benches are
 * close together the player can be in range of more than one at once: the one
 * created with the highest priority wins, and with freeOnly the ones somebody
 * is already using are passed over in that order, so walking up to a row of
 * benches and pressing the key lands on one that is free.
 *
 * @param playerid    Player to look around.
 * @param type        Filled with the type of the equipment found.
 * @param index       Filled with the id of the equipment inside its type.
 * @param freeOnly    Skips equipments already taken by another player.
 *
 * @return True when an equipment was found.
 */
forward bool:GetPlayerGymEquipment(playerid, &E_GYM_EQUIPMENT_TYPE:type, &index, bool:freeOnly = true);

/**
 * Tells whether the player is getting on, using or getting off an equipment.
 *
 * @param playerid    Player to check.
 *
 * @return True while the player is not free.
 */
forward bool:IsPlayerExercising(playerid);

/**
 * Gives the type of equipment the player is exercising on.
 *
 * @param playerid    Player to check.
 *
 * @return The equipment type, GYM_EQUIPMENT_NONE when the player is free.
 */
forward E_GYM_EQUIPMENT_TYPE:GetPlayerExerciseType(playerid);

/**
 * Gives the id, inside its own type, of the equipment the player is on.
 *
 * @param playerid    Player to check.
 *
 * @return The equipment id, -1 when the player is free.
 */
forward GetPlayerExerciseIndex(playerid);

/**
 * Gives which step of the exercise the player is in.
 *
 * @param playerid    Player to check.
 *
 * @return GYM_EXERCISE_NONE, GYM_EXERCISE_GETTING_ON, GYM_EXERCISE_ACTIVE or
 *         GYM_EXERCISE_GETTING_OFF.
 */
forward E_GYM_EXERCISE_STATE:GetPlayerExerciseState(playerid);

/**
 * Gives how much strength the player has built up.
 *
 * @param playerid    Player to check.
 *
 * @return Strength between 0.0 and GYM_POWER_MAX, 0.0 when not exercising.
 */
forward Float:GetPlayerExercisePower(playerid);

/**
 * Overwrites the strength of the player, clamped to the allowed range.
 *
 * @param playerid    Player to change.
 * @param power       New strength.
 *
 * @return True when the player is exercising.
 */
forward bool:SetPlayerExercisePower(playerid, Float:power);

/**
 * Gives the repetitions, or the distance, collected in the current exercise.
 *
 * @param playerid    Player to check.
 *
 * @return The metric of the exercise, 0 when not exercising.
 */
forward GetPlayerExerciseMetric(playerid);

/**
 * Overwrites the metric of the current exercise.
 *
 * @param playerid    Player to change.
 * @param metric      New amount of repetitions or distance.
 *
 * @return True when the player is exercising.
 */
forward bool:SetPlayerExerciseMetric(playerid, metric);

/**
 * Tells whether the camera of the player is still travelling to a view. Turning
 * the camera again is refused while this is true, so a key held down does not
 * cut a movement in half.
 *
 * @param playerid    Player to check.
 *
 * @return True while the camera is moving.
 */
forward bool:IsPlayerExerciseCameraMoving(playerid);

/**
 * Moves the camera of the player to the next view of the equipment, the same
 * thing GYM_CAMERA_KEY does, in GYM_CAMERA_MOVE_TIME milliseconds. The camera
 * walks around the equipment, one piece every GYM_CAMERA_MOVE_TICK
 * milliseconds, instead of cutting straight through it. After the last view it
 * comes back to the first one.
 *
 * How it gets there follows GYM_CAMERA_MODE: it cuts, it walks the arc, or the
 * camera is orbiting on its own while the key is held and this does one step.
 *
 * The camera only turns while the exercise is running, never during the get on
 * or the get off animation.
 *
 * @param playerid    Player to move the camera of.
 *
 * @return True when the camera started moving, false when the exercise is not
 *         running, the library does not own the camera of this player or it is
 *         still moving.
 */
forward bool:RotatePlayerExerciseCamera(playerid);

/**
 * Gives how much the player turned the camera around the equipment with
 * GYM_CAMERA_KEY, on top of the angle the equipment was configured with.
 *
 * @param playerid    Player to check.
 *
 * @return The rotation of the player, between 0.0 and 360.0 degrees.
 */
forward Float:GetPlayerExerciseCameraAngle(playerid);

/**
 * Turns the camera of the player around the equipment and applies it right
 * away. The rotation is on top of the angle of the equipment and is wrapped
 * into 0.0 .. 360.0, so adding GYM_CAMERA_STEP_ANGLE repeatedly cycles.
 *
 * @param playerid    Player to move the camera of.
 * @param angle       New rotation, in degrees.
 *
 * Like the key, it only answers while the exercise is running.
 *
 * @return True when the camera started moving, false when the exercise is not
 *         running, the equipment was created with a camera distance of 0.0 or
 *         the camera is still moving.
 */
forward bool:SetPlayerExerciseCameraAngle(playerid, Float:angle);

/**
 * Takes the player out of the equipment.
 *
 * @param playerid    Player to take out.
 * @param animate     True plays the regular get off animation, false cancels
 *                    the exercise right away.
 *
 * @return True when the player was exercising.
 */
forward bool:StopPlayerExercise(playerid, bool:animate = true);

/**
 * # Callbacks
 */

/**
 * Called before a player gets on an equipment.
 *
 * @param playerid    Player asking to exercise.
 * @param type        Type of the equipment.
 * @param index       Id of the equipment inside its type.
 *
 * @return Return 0 to keep the player off the equipment.
 */
forward OnPlayerRequestExercise(playerid, E_GYM_EQUIPMENT_TYPE:type, index);

/**
 * Called once the get on animation is over and the exercise really starts,
 * which is where the hud of the exercise is usually created.
 *
 * @param playerid    Player who started exercising.
 * @param type        Type of the equipment.
 * @param index       Id of the equipment inside its type.
 */
forward OnPlayerStartExercise(playerid, E_GYM_EQUIPMENT_TYPE:type, index);

/**
 * Called every GYM_UPDATE_TIME milliseconds while the exercise is running.
 *
 * @param playerid    Player exercising.
 * @param type        Type of the equipment.
 * @param index       Id of the equipment inside its type.
 * @param power       Current strength, between 0.0 and GYM_POWER_MAX.
 * @param metric      Repetitions done, or distance covered, so far.
 */
forward OnPlayerUpdateExercise(playerid, E_GYM_EQUIPMENT_TYPE:type, index, Float:power, metric);

/**
 * Called when the player is free again and the equipment is released, which is
 * where the hud of the exercise is usually destroyed.
 *
 * @param playerid    Player who finished exercising.
 * @param type        Type of the equipment.
 * @param index       Id of the equipment inside its type.
 * @param metric      Repetitions done, or distance covered, in total.
 */
forward OnPlayerFinishExercise(playerid, E_GYM_EQUIPMENT_TYPE:type, index, metric);

/**
 * Called when a repetition was too heavy and the player dropped it. The bar
 * loses GYM_FAIL_POWER_LOSS, the player is left catching their breath for
 * GYM_FAIL_REST_TIME seconds and the repetition is not counted.
 *
 * @param playerid    Player who failed the repetition.
 * @param type        Type of the equipment.
 * @param index       Id of the equipment inside its type.
 */
forward OnPlayerFailRepetition(playerid, E_GYM_EQUIPMENT_TYPE:type, index);

/**
 * Called on every repetition, before the library plays anything, so the
 * gamemode can show the weight on the character its own way: which of the two
 * animations of the equipment to use, how fast to play it and, with
 * SetPlayerRepetitionTime, how long the weight stays up.
 *
 * Answer 0 to let the library play its own: the straining animation while the
 * weight is above what the player carries easily, the smooth one once it is
 * not, both at GYM_REPETITION_SPEED.
 *
 * @param playerid    Player doing the repetition.
 * @param type        Type of the equipment.
 * @param index       Id of the equipment inside its type.
 * @param effort      Strength of the player over the strength the weight asks
 *                    for: below 1.0 the player is straining.
 *
 * @return Return anything other than 0 when the animation was played here.
 */
forward OnPlayerRepetition(playerid, E_GYM_EQUIPMENT_TYPE:type, index, Float:effort);

/**
 * Called when the muscle gives out, on the repetition that spent the last of
 * it. The weight is on its way down and the player leaves the equipment
 * GYM_FAILURE_TIME later, so this is where a gamemode says why.
 *
 * How many repetitions that took is the weight against the strength of the
 * player: GYM_REPETITIONS_AT_LIMIT of them when the bar is everything they
 * have, GYM_REPETITIONS_AT_EASE when it is nothing.
 *
 * @param playerid    Player whose muscle gave out.
 * @param type        Type of the equipment.
 * @param index       Id of the equipment inside its type.
 * @param metric      Repetitions the set ended on.
 */
forward OnPlayerMuscleFailure(playerid, E_GYM_EQUIPMENT_TYPE:type, index, metric);

/**
 * Called on the muscle failure that ends the day, right after
 * OnPlayerMuscleFailure. From here GetGymRepetitionsLeft answers 0 for every
 * weight in the building until the player has rested, which is what turns them
 * away at the door.
 *
 * @param playerid    Player who is done training.
 * @param type        Type of the equipment it ended on.
 * @param index       Id of the equipment inside its type.
 */
forward OnPlayerGymExhausted(playerid, E_GYM_EQUIPMENT_TYPE:type, index);

/**
 * # Internal Functions
 */

static stock GetFreeGymEquipmentIndexInternal(E_GYM_EQUIPMENT_TYPE:type) {
    if (gGymEquipmentFreeHead[_:type] != -1) {
        new const
            index = gGymEquipmentFreeHead[_:type]
        ;

        gGymEquipmentFreeHead[_:type] = gGymEquipmentNextFree[_:type][index];

        return index;
    }

    if (gGymEquipmentHighWater[_:type] != MAX_GYM_EQUIPMENT) {
        return gGymEquipmentHighWater[_:type]++;
    }

    return -1;
}

static stock AllocGymEquipmentInternal(E_GYM_EQUIPMENT_TYPE:type) {
    new const
        index = GetFreeGymEquipmentIndexInternal(type)
    ;

    if (index == -1) {
        return -1;
    }

    ++gGymEquipmentCount[_:type];

    return index;
}

static stock FreeGymEquipmentInternal(E_GYM_EQUIPMENT_TYPE:type, index) {
    gGymEquipmentNextFree[_:type][index] = gGymEquipmentFreeHead[_:type];
    gGymEquipmentFreeHead[_:type] = index;

    --gGymEquipmentCount[_:type];
}

static stock bool:IsValidGymEquipmentInternal(E_GYM_EQUIPMENT_TYPE:type, index) {
    if (!(0 <= _:type < GYM_EQUIPMENT_TYPES)) {
        return false;
    }

    if (!(0 <= index < MAX_GYM_EQUIPMENT)) {
        return false;
    }

    return bool:gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_CREATED];
}

static stock GetGymEquipmentCellInternal(E_GYM_EQUIPMENT_TYPE:type, index, offset) {
    if (!IsValidGymEquipmentInternal(type, index)) {
        return 0;
    }

    return gGymEquipmentData[_:type][index][offset];
}

static stock bool:SetGymEquipmentCellInternal(E_GYM_EQUIPMENT_TYPE:type, index, offset, AnyTag:value) {
    if (!IsValidGymEquipmentInternal(type, index)) {
        return false;
    }

    gGymEquipmentData[_:type][index][offset] = _:value;

    return true;
}

// What the prompt on a piece of equipment reads. The name of the exercise and
// the key, because a player walking past has no other way of knowing either.
static stock SetGymEquipmentPopupInternal(E_GYM_EQUIPMENT_TYPE:type, Button:buttonid) {
    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            SetButtonPopupText(buttonid, GYM_BARBELL_POPUP, GYM_ENTER_KEY_TEXT);
        }
        case GYM_EQUIPMENT_DUMBELL: {
            SetButtonPopupText(buttonid, GYM_DUMBELL_POPUP, GYM_ENTER_KEY_TEXT);
        }
        case GYM_EQUIPMENT_TREADMILL: {
            SetButtonPopupText(buttonid, GYM_TREADMILL_POPUP, GYM_ENTER_KEY_TEXT);
        }
        case GYM_EQUIPMENT_BIKE: {
            SetButtonPopupText(buttonid, GYM_BIKE_POPUP, GYM_ENTER_KEY_TEXT);
        }
        default: {}
    }
}

static stock GetGymOffsetPositionInternal(Float:x, Float:y, Float:z, Float:angle, Float:forwardOffset, Float:rightOffset, Float:upOffset, &Float:outputX, &Float:outputY, &Float:outputZ) {
    new const
        Float:sinAngle = floatsin(-angle, degrees),
        Float:cosAngle = floatcos(-angle, degrees)
    ;

    outputX = x + forwardOffset * sinAngle + rightOffset * cosAngle;
    outputY = y + forwardOffset * cosAngle - rightOffset * sinAngle;
    outputZ = z + upOffset;
}

static stock CreateGymObjectInternal(modelid, Float:x, Float:y, Float:z, Float:rotationX, Float:rotationY, Float:rotationZ, worldid, interiorid) {
    #if defined GYM_STREAMER_OBJECTS
        return _:CreateDynamicObject(modelid, x, y, z, rotationX, rotationY, rotationZ, worldid, interiorid);
    #else
        #pragma unused worldid
        #pragma unused interiorid

        return CreateObject(modelid, x, y, z, rotationX, rotationY, rotationZ);
    #endif
}

static stock bool:IsValidGymObjectInternal(objectid) {
    #if defined GYM_STREAMER_OBJECTS
        return bool:IsValidDynamicObject(STREAMER_TAG_OBJECT:objectid);
    #else
        return IsValidObject(objectid);
    #endif
}

static stock DestroyGymObjectInternal(objectid) {
    if (!IsValidGymObjectInternal(objectid)) {
        return;
    }

    #if defined GYM_STREAMER_OBJECTS
        DestroyDynamicObject(STREAMER_TAG_OBJECT:objectid);
    #else
        DestroyObject(objectid);
    #endif
}

static stock GetGymGetOnTimeInternal(E_GYM_EQUIPMENT_TYPE:type) {
    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            return GYM_BARBELL_GET_ON_TIME;
        }
        case GYM_EQUIPMENT_DUMBELL: {
            return GYM_DUMBELL_GET_ON_TIME;
        }
        case GYM_EQUIPMENT_TREADMILL: {
            return GYM_TREADMILL_GET_ON_TIME;
        }
        case GYM_EQUIPMENT_BIKE: {
            return GYM_BIKE_GET_ON_TIME;
        }
        default: {}
    }

    return 0;
}

static stock CreateGymEquipmentPropsInternal(E_GYM_EQUIPMENT_TYPE:type, index) {
    if (!IsValidGymEquipmentInternal(type, index)) {
        return;
    }

    new
        Float:x, Float:y, Float:z
    ;

    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            if (IsValidGymObjectInternal(gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_PROP_ID])) {
                return;
            }

            GetGymOffsetPositionInternal(
                gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_X], gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_Y], gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_Z], gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_ANGLE],
                0.55, -0.445, 1.0,
                x, y, z
            );

            SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_PROP_ID, CreateGymObjectInternal(
                GYM_MODEL_BARBELL,
                x, y, z,
                0.0, 90.0, gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_ANGLE],
                gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_WORLD_ID], gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_INTERIOR_ID]
            ));
        }
        case GYM_EQUIPMENT_DUMBELL: {
            if (!IsValidGymObjectInternal(gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_PROP_ID])) {
                GetGymOffsetPositionInternal(
                    gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_X], gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_Y], gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_Z], gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_ANGLE],
                    0.0, -0.2, 0.0,
                    x, y, z
                );

                SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_PROP_ID, CreateGymObjectInternal(
                    GYM_MODEL_DUMBELL_LEFT,
                    x, y, z,
                    0.0, 90.0, gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_ANGLE],
                    gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_WORLD_ID], gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_INTERIOR_ID]
                ));
            }

            if (!IsValidGymObjectInternal(gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_PROP_ID + 1])) {
                GetGymOffsetPositionInternal(
                    gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_X], gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_Y], gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_Z], gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_ANGLE],
                    0.0, 0.2, 0.0,
                    x, y, z
                );

                SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_PROP_ID + 1, CreateGymObjectInternal(
                    GYM_MODEL_DUMBELL_RIGHT,
                    x, y, z,
                    0.0, 90.0, gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_ANGLE],
                    gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_WORLD_ID], gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_INTERIOR_ID]
                ));
            }
        }
        default: {}
    }
}

static stock DestroyGymEquipmentPropsInternal(E_GYM_EQUIPMENT_TYPE:type, index) {
    for (new i; i != 2; ++i) {
        DestroyGymObjectInternal(GetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_PROP_ID + i));

        SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_PROP_ID + i, GYM_INVALID_OBJECT_ID);
    }
}

static stock RefreshGymObjectsInternal(playerid) {
    #if defined GYM_STREAMER_OBJECTS
        if (!IsPlayerConnected(playerid)) {
            return;
        }

        Streamer_Update(playerid, STREAMER_TYPE_OBJECT);
    #else
        #pragma unused playerid
    #endif
}

static stock Float:GetGymWrappedAngleInternal(Float:angle) {
    new const
        Float:turns = angle / 360.0
    ;

    return (turns - float(floatround(turns, floatround_floor))) * 360.0;
}

static stock Float:GetGymNextCameraAngleInternal(Float:angle, steps, Float:stepAngle) {
    if (steps <= 1 || stepAngle == 0.0) {
        return 0.0;
    }

    new const
        Float:next = angle + stepAngle
    ;

    return (next >= steps * stepAngle) ? 0.0 : next;
}

static stock Float:GetGymShortestDeltaInternal(Float:fromAngle, Float:toAngle) {
    new const
        Float:delta = GetGymWrappedAngleInternal(toAngle - fromAngle)
    ;

    return (delta > 180.0) ? (delta - 360.0) : delta;
}

static stock GetGymCameraPositionInternal(const data[E_GYM_EQUIPMENT_DATA], Float:extraAngle, &Float:outputX, &Float:outputY, &Float:outputZ) {
    new const
        Float:cameraDistance = data[E_GYM_EQUIPMENT_CAMERA_DISTANCE],
        Float:cameraAngle = data[E_GYM_EQUIPMENT_ANGLE] + data[E_GYM_EQUIPMENT_CAMERA_ANGLE] + extraAngle
    ;

    outputX = data[E_GYM_EQUIPMENT_X] - cameraDistance * floatsin(-cameraAngle, degrees);
    outputY = data[E_GYM_EQUIPMENT_Y] - cameraDistance * floatcos(-cameraAngle, degrees);
    outputZ = data[E_GYM_EQUIPMENT_Z] + data[E_GYM_EQUIPMENT_CAMERA_HEIGHT];
}

static stock bool:ApplyGymCameraInternal(playerid, const data[E_GYM_EQUIPMENT_DATA], Float:extraAngle) {
    if (data[E_GYM_EQUIPMENT_CAMERA_DISTANCE] <= 0.0) {
        return false;
    }

    new
        Float:targetX, Float:targetY, Float:targetZ
    ;

    GetGymCameraPositionInternal(data, extraAngle, targetX, targetY, targetZ);

    SetPlayerCameraPos(playerid, targetX, targetY, targetZ);
    SetPlayerCameraLookAt(playerid, data[E_GYM_EQUIPMENT_X], data[E_GYM_EQUIPMENT_Y], data[E_GYM_EQUIPMENT_Z] + GYM_CAMERA_LOOK_AT_HEIGHT, CAMERA_CUT);

    return true;
}

static stock bool:MoveGymCameraInternal(playerid, const data[E_GYM_EQUIPMENT_DATA], Float:fromAngle, Float:toAngle, moveTime) {
    if (data[E_GYM_EQUIPMENT_CAMERA_DISTANCE] <= 0.0) {
        return false;
    }

    if (moveTime <= 0) {
        return ApplyGymCameraInternal(playerid, data, toAngle);
    }

    new
        Float:fromX, Float:fromY, Float:fromZ,
        Float:toX, Float:toY, Float:toZ
    ;

    GetGymCameraPositionInternal(data, fromAngle, fromX, fromY, fromZ);
    GetGymCameraPositionInternal(data, toAngle, toX, toY, toZ);

    new const
        Float:lookAtX = data[E_GYM_EQUIPMENT_X],
        Float:lookAtY = data[E_GYM_EQUIPMENT_Y],
        Float:lookAtZ = data[E_GYM_EQUIPMENT_Z] + GYM_CAMERA_LOOK_AT_HEIGHT
    ;

    InterpolateCameraPos(playerid, fromX, fromY, fromZ, toX, toY, toZ, moveTime, CAMERA_MOVE);
    InterpolateCameraLookAt(playerid, lookAtX, lookAtY, lookAtZ, lookAtX, lookAtY, lookAtZ, moveTime, CAMERA_MOVE);

    return true;
}

static stock PlacePlayerOnGymEquipmentInternal(playerid, E_GYM_EQUIPMENT_TYPE:type, const data[E_GYM_EQUIPMENT_DATA]) {
    new const
        Float:x = data[E_GYM_EQUIPMENT_X],
        Float:y = data[E_GYM_EQUIPMENT_Y],
        Float:z = data[E_GYM_EQUIPMENT_Z],
        Float:angle = data[E_GYM_EQUIPMENT_ANGLE],
        Float:sinAngle = floatsin(-angle, degrees),
        Float:cosAngle = floatcos(-angle, degrees)
    ;

    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            SetPlayerPos(playerid, x - 1.0 * sinAngle, y - 1.0 * cosAngle, z + GYM_BARBELL_PLAYER_HEIGHT);
            SetPlayerFacingAngle(playerid, angle);
        }
        case GYM_EQUIPMENT_DUMBELL: {
            SetPlayerPos(playerid, x - 1.0 * sinAngle, y - 1.0 * cosAngle, z + GYM_DUMBELL_PLAYER_HEIGHT);
            SetPlayerFacingAngle(playerid, angle);
        }
        case GYM_EQUIPMENT_TREADMILL: {
            SetPlayerPos(playerid, x - 1.5 * sinAngle, y - 1.5 * cosAngle, z + GYM_TREADMILL_PLAYER_HEIGHT);
            SetPlayerFacingAngle(playerid, angle);
        }
        case GYM_EQUIPMENT_BIKE: {
            SetPlayerPos(playerid,
                x + 0.5 * floatsin(-angle + 90.0, degrees) + 0.5 * sinAngle,
                y + 0.5 * floatcos(-angle + 90.0, degrees) + 0.5 * cosAngle,
                z + GYM_BIKE_PLAYER_HEIGHT
            );

            SetPlayerFacingAngle(playerid, angle - 180.0);
        }
        default: {}
    }

    TogglePlayerControllable(playerid, false);

    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_ANGLE]    = 0.0;
    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_SEGMENT]  = 0;
    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_SEGMENTS] = 0;
    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_FROM]     = 0.0;
    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_DELTA]    = 0.0;
    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_MOVE_END] = 0;
    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_MOVED]    = ApplyGymCameraInternal(playerid, data, 0.0);

    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            ApplyAnimation(playerid, "BENCHPRESS", "GYM_BP_GETON", 4.1, false, false, false, true, 0, SYNC_ALL);
        }
        case GYM_EQUIPMENT_DUMBELL: {
            ApplyAnimation(playerid, "FREEWEIGHTS", "GYM_FREE_PICKUP", 4.1, false, false, false, true, 0, SYNC_ALL);
        }
        case GYM_EQUIPMENT_TREADMILL: {
            ApplyAnimation(playerid, "GYMNASIUM", "GYM_TREAD_GETON", 4.1, false, false, false, true, 0, SYNC_ALL);
        }
        case GYM_EQUIPMENT_BIKE: {
            ApplyAnimation(playerid, "GYMNASIUM", "GYM_BIKE_GETON", 4.1, false, false, false, true, 0, SYNC_ALL);
        }
        default: {}
    }
}

static stock Float:GetGymDefaultWeightInternal(E_GYM_EQUIPMENT_TYPE:type) {
    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            return GYM_BARBELL_DEFAULT_WEIGHT;
        }
        case GYM_EQUIPMENT_DUMBELL: {
            return GYM_DUMBELL_DEFAULT_WEIGHT;
        }
        default: {}
    }

    return 0.0;
}

// Only what is lifted has a weight worth answering for; running and pedalling
// answer to breath instead.
static stock Float:GetGymRequiredStrengthInternal(E_GYM_EQUIPMENT_TYPE:type, index) {
    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            return Float:GetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_WEIGHT) * GYM_POWER_MAX / GYM_BARBELL_WEIGHT_AT_FULL_STRENGTH;
        }
        case GYM_EQUIPMENT_DUMBELL: {
            return Float:GetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_WEIGHT) * GYM_POWER_MAX / GYM_DUMBELL_WEIGHT_AT_FULL_STRENGTH;
        }
        case GYM_EQUIPMENT_TREADMILL, GYM_EQUIPMENT_BIKE: {
            // Nothing to lift.
        }
        default: {}
    }

    return 0.0;
}

// Under the strength the weight asks for the repetitions crawl, over it they
// come easy.
static stock Float:GetGymEffortFactorInternal(playerid, E_GYM_EQUIPMENT_TYPE:type, index) {
    new const
        Float:required = GetGymRequiredStrengthInternal(type, index)
    ;

    if (required <= 0.0) {
        return 1.0;
    }

    new const
        Float:factor = gGymPlayerData[playerid][E_GYM_PLAYER_STRENGTH] / required
    ;

    if (factor < GYM_EFFORT_MIN) {
        return GYM_EFFORT_MIN;
    }

    if (factor > GYM_EFFORT_MAX) {
        return GYM_EFFORT_MAX;
    }

    return factor;
}

// Nothing is dropped while the weight stays well under what the player carries,
// the chance climbs as it closes on their limit, and past that limit the weight
// does not go up at all.
static stock Float:GetGymFailChanceInternal(playerid, E_GYM_EQUIPMENT_TYPE:type, index) {
    new const
        Float:required = GetGymRequiredStrengthInternal(type, index),
        Float:strength = gGymPlayerData[playerid][E_GYM_PLAYER_STRENGTH]
    ;

    if (required <= 0.0) {
        return 0.0;
    }

    if (strength <= 0.0) {
        return 1.0;
    }

    new const
        Float:asked = required / strength
    ;

    if (asked <= GYM_FAIL_THRESHOLD) {
        return 0.0;
    }

    if (asked >= GYM_FAIL_LIMIT) {
        return 1.0;
    }

    return (asked - GYM_FAIL_THRESHOLD) / (GYM_FAIL_LIMIT - GYM_FAIL_THRESHOLD) * GYM_FAIL_CHANCE_MAX;
}

// Equipment with a weight on it, where filling the bar is a repetition.
static stock bool:IsGymLiftingEquipmentInternal(E_GYM_EQUIPMENT_TYPE:type) {
    switch (type) {
        case GYM_EQUIPMENT_BARBELL, GYM_EQUIPMENT_DUMBELL: {
            return true;
        }
        default: {}
    }

    return false;
}

// What a muscle spends belongs to the muscle that spent it: arms and chest for
// the weights. A treadmill is legs and breath, and breath is somebody else's to
// keep, so nothing the weights are owed holds one back.
static stock bool:IsGymRestingInternal(playerid, E_GYM_EQUIPMENT_TYPE:type) {
    if (!IsGymLiftingEquipmentInternal(type)) {
        return false;
    }

    return gettime() < gGymPlayerData[playerid][E_GYM_PLAYER_REST_UNTIL];
}

static stock Float:GetGymClampedPowerInternal(Float:power) {
    if (power < 0.0) {
        return 0.0;
    }

    if (power > GYM_POWER_MAX) {
        return GYM_POWER_MAX;
    }

    return power;
}

// The three animations the game keeps for a movement: straining under a weight
// that is near everything the player has, ordinary, and the clean one for a
// weight that is nothing to them. No speed of its own: that is a number the
// gamemode is in a better place to choose.
static stock PlayGymLiftAnimationInternal(playerid, E_GYM_EQUIPMENT_TYPE:type, Float:effort) {
    if (type == GYM_EQUIPMENT_BARBELL) {
        if (effort < GYM_EFFORT_HARD) {
            ApplyAnimation(playerid, "BENCHPRESS", GYM_BARBELL_ANIMATION_HARD, GYM_REPETITION_SPEED, false, false, false, true, 0, SYNC_ALL);

            return;
        }

        if (effort >= GYM_EFFORT_SMOOTH) {
            ApplyAnimation(playerid, "BENCHPRESS", GYM_BARBELL_ANIMATION_SMOOTH, GYM_REPETITION_SPEED, false, false, false, true, 0, SYNC_ALL);

            return;
        }

        ApplyAnimation(playerid, "BENCHPRESS", GYM_BARBELL_ANIMATION_EASY, GYM_REPETITION_SPEED, false, false, false, true, 0, SYNC_ALL);

        return;
    }

    if (type != GYM_EQUIPMENT_DUMBELL) {
        return;
    }

    if (effort < GYM_EFFORT_HARD) {
        ApplyAnimation(playerid, "FREEWEIGHTS", GYM_DUMBELL_ANIMATION_HARD, GYM_REPETITION_SPEED, false, false, false, true, 0, SYNC_ALL);

        return;
    }

    if (effort >= GYM_EFFORT_SMOOTH) {
        ApplyAnimation(playerid, "FREEWEIGHTS", GYM_DUMBELL_ANIMATION_SMOOTH, GYM_REPETITION_SPEED, false, false, false, true, 0, SYNC_ALL);

        return;
    }

    ApplyAnimation(playerid, "FREEWEIGHTS", GYM_DUMBELL_ANIMATION_EASY, GYM_REPETITION_SPEED, false, false, false, true, 0, SYNC_ALL);
}

// How good this player is at resting. Nothing in the library writes it: it is
// there for whatever a gamemode wants it to mean.
static stock Float:GetGymRecoveryInternal(playerid) {
    new const
        Float:recovery = gGymPlayerData[playerid][E_GYM_PLAYER_RECOVERY]
    ;

    return (recovery <= 0.0) ? 1.0 : recovery;
}

// Away from the equipment nothing ticks: what is stored is where the player
// was when they left and the second they left, and the rest since then is
// worked out here, whenever somebody asks.
static stock Float:GetGymRestedInternal(playerid, Float:spent, Float:rate) {
    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_NONE) {
        return spent;
    }

    new const
        Float:left = spent - float(gettime() - gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE_TIME]) * rate * GetGymRecoveryInternal(playerid)
    ;

    return (left < 0.0) ? 0.0 : left;
}

static stock Float:GetGymFatigueInternal(playerid) {
    return GetGymRestedInternal(playerid, gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE], GYM_FATIGUE_RECOVERY);
}

static stock Float:GetGymExhaustionInternal(playerid) {
    return GetGymRestedInternal(playerid, gGymPlayerData[playerid][E_GYM_PLAYER_EXHAUSTION], GYM_EXHAUSTION_RECOVERY);
}

// How many times this player can take a muscle to failure before the day is
// over. Strength is the only thing the library knows about how used to a gym
// somebody is, so it is what the two ends are read between.
static stock Float:GetGymFailuresInternal(playerid) {
    new
        Float:share = gGymPlayerData[playerid][E_GYM_PLAYER_STRENGTH] / GYM_POWER_MAX
    ;

    if (share < 0.0) {
        share = 0.0;
    }

    if (share > 1.0) {
        share = 1.0;
    }

    new const
        Float:failures = float(GYM_FAILURES_UNTRAINED) + float(GYM_FAILURES_TRAINED - GYM_FAILURES_UNTRAINED) * share
    ;

    return (failures < 1.0) ? 1.0 : failures;
}

// What one repetition takes out of the muscle. A weight at the limit of the
// player is worth many of an easy one, which is the whole of why a set ends.
static stock Float:GetGymFatigueCostInternal(Float:effort) {
    new const
        Float:span = GYM_EFFORT_MAX - 1.0
    ;

    new
        Float:share = (span > 0.0) ? ((effort - 1.0) / span) : 1.0
    ;

    if (share < 0.0) {
        share = 0.0;
    }

    if (share > 1.0) {
        share = 1.0;
    }

    new const
        Float:repetitions = float(GYM_REPETITIONS_AT_LIMIT) + float(GYM_REPETITIONS_AT_EASE - GYM_REPETITIONS_AT_LIMIT) * share
    ;

    if (repetitions < 1.0) {
        return GYM_FATIGUE_MAX;
    }

    return GYM_FATIGUE_MAX / repetitions;
}

// Equipment the player runs or pedals on, which has a speed instead of a
// weight and answers to breath rather than strength.
static stock bool:IsGymRunningEquipmentInternal(E_GYM_EQUIPMENT_TYPE:type) {
    switch (type) {
        case GYM_EQUIPMENT_TREADMILL, GYM_EQUIPMENT_BIKE: {
            return true;
        }
        default: {}
    }

    return false;
}

// Pushing keeps the machine where it is, letting go brings it down quickly.
static stock Float:GetGymDecayInternal(playerid, E_GYM_EQUIPMENT_TYPE:type) {
    if (!IsGymRunningEquipmentInternal(type)) {
        return GYM_POWER_DECAY;
    }

    if ((GetTickCount() - gGymPlayerData[playerid][E_GYM_PLAYER_LAST_INPUT]) < GYM_COAST_TIME) {
        return GYM_POWER_DECAY;
    }

    return GYM_COAST_DECAY;
}

static stock E_GYM_SPEED_LEVEL:GetGymSpeedLevelInternal(Float:power) {
    if (power <= GYM_SPEED_SLOW_POWER) {
        return GYM_SPEED_IDLE;
    }

    if (power <= GYM_SPEED_MEDIUM_POWER) {
        return GYM_SPEED_SLOW;
    }

    if (power <= GYM_SPEED_FAST_POWER) {
        return GYM_SPEED_MEDIUM;
    }

    return GYM_SPEED_FAST;
}

// Nobody steps off a treadmill still running at full speed: the player has to
// bring it down first.
static stock bool:IsGymDismountAllowedInternal(playerid) {
    new const
        E_GYM_EQUIPMENT_TYPE:type = gGymPlayerData[playerid][E_GYM_PLAYER_TYPE]
    ;

    if (!IsGymRunningEquipmentInternal(type)) {
        return true;
    }

    #if GYM_TREADMILL_FALL_OFF
        // The pedals of a bike are turning under the player's feet and have to come
        // down first. A treadmill just throws them off, and the animation for that
        // has been sitting here unreachable for as long as this said otherwise.
        if (type == GYM_EQUIPMENT_TREADMILL) {
            return true;
        }
    #endif

    return GetGymSpeedLevelInternal(gGymPlayerData[playerid][E_GYM_PLAYER_POWER]) <= GYM_DISMOUNT_SPEED;
}

static stock bool:IsGymSessionValidInternal(playerid, session) {
    return IsPlayerConnected(playerid) && gGymPlayerData[playerid][E_GYM_PLAYER_SESSION] == session;
}

static stock KillGymUpdateTimerInternal(playerid) {
    if (gGymPlayerData[playerid][E_GYM_PLAYER_TIMER]) {
        KillTimer(gGymPlayerData[playerid][E_GYM_PLAYER_TIMER]);
        gGymPlayerData[playerid][E_GYM_PLAYER_TIMER] = 0;
    }
}

static stock KillGymCameraTimerInternal(playerid) {
    if (gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_TIMER]) {
        KillTimer(gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_TIMER]);
        gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_TIMER] = 0;
    }
}

static stock bool:StartGymCameraArcInternal(playerid, const data[E_GYM_EQUIPMENT_DATA], Float:fromAngle, Float:delta) {
    KillGymCameraTimerInternal(playerid);

    if (GYM_CAMERA_MOVE_TIME <= 0 || delta == 0.0) {
        return ApplyGymCameraInternal(playerid, data, fromAngle + delta);
    }

    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_MOVE_END] = GetTickCount() + GYM_CAMERA_MOVE_TIME;

    new const
        segments = GYM_CAMERA_MOVE_SEGMENTS,
        segmentTime = GYM_CAMERA_MOVE_TIME / segments
    ;

    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_FROM] = fromAngle;
    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_DELTA] = delta;
    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_SEGMENTS] = segments;
    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_SEGMENT] = 1;
    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_TIMER] = SetTimerEx("@Gym_CameraArc", segmentTime, true, "ii", playerid, gGymPlayerData[playerid][E_GYM_PLAYER_SESSION]);

    return MoveGymCameraInternal(playerid, data, fromAngle, fromAngle + delta / float(segments), segmentTime);
}

#if GYM_CAMERA_MODE == GYM_CAMERA_MODE_ORBIT
static stock bool:StartGymCameraHoldInternal(playerid) {
    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_ACTIVE) {
        return false;
    }

    if (!gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_MOVED]) {
        return false;
    }

    KillGymCameraTimerInternal(playerid);

    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_TIMER] = SetTimerEx("@Gym_CameraHold", GYM_CAMERA_HOLD_TICK, true, "ii", playerid, gGymPlayerData[playerid][E_GYM_PLAYER_SESSION]);

    return true;
}
#endif

static stock bool:ChangeGymCameraAngleInternal(playerid, const data[E_GYM_EQUIPMENT_DATA], Float:fromAngle, Float:delta) {
    #if GYM_CAMERA_MODE == GYM_CAMERA_MODE_INTERPOLATE
        return StartGymCameraArcInternal(playerid, data, fromAngle, delta);
    #else
        return ApplyGymCameraInternal(playerid, data, fromAngle + delta);
    #endif
}

static stock ResetGymPlayerDataInternal(playerid) {
    KillGymUpdateTimerInternal(playerid);
    KillGymCameraTimerInternal(playerid);

    gGymPlayerData[playerid][E_GYM_PLAYER_TYPE]          = GYM_EQUIPMENT_NONE;
    gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]         = -1;
    gGymPlayerData[playerid][E_GYM_PLAYER_STATE]         = GYM_EXERCISE_NONE;
    gGymPlayerData[playerid][E_GYM_PLAYER_POWER]         = 0.0;
    gGymPlayerData[playerid][E_GYM_PLAYER_METRIC]        = 0;
    gGymPlayerData[playerid][E_GYM_PLAYER_LAST_ACTIVITY] = 0;
    gGymPlayerData[playerid][E_GYM_PLAYER_LAST_INPUT]    = 0;
    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_MOVED]  = false;
    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_ANGLE]  = 0.0;
    gGymPlayerData[playerid][E_GYM_PLAYER_SESSION]       = ++gGymSessionCounter;
}

static stock bool:IsGymPowerInputAllowedInternal(playerid) {
    new const
        animationIndex = GetPlayerAnimationIndex(playerid)
    ;

    switch (gGymPlayerData[playerid][E_GYM_PLAYER_TYPE]) {
        case GYM_EQUIPMENT_BARBELL: {
            return animationIndex == GYM_ANIM_BARBELL_LOWER || animationIndex == GYM_ANIM_BARBELL_GET_ON;
        }
        case GYM_EQUIPMENT_DUMBELL: {
            return animationIndex == GYM_ANIM_DUMBELL_LOWER || animationIndex == GYM_ANIM_DUMBELL_GET_ON;
        }
        case GYM_EQUIPMENT_TREADMILL: {
            return animationIndex == GYM_ANIM_TREADMILL_JOG || animationIndex == GYM_ANIM_TREADMILL_SPRINT || animationIndex == GYM_ANIM_TREADMILL_TIRED || animationIndex == GYM_ANIM_TREADMILL_WALK;
        }
        case GYM_EQUIPMENT_BIKE: {
            return animationIndex == GYM_ANIM_BIKE_FAST || animationIndex == GYM_ANIM_BIKE_PEDAL || animationIndex == GYM_ANIM_BIKE_SLOW || animationIndex == GYM_ANIM_BIKE_STILL;
        }
        default: {}
    }

    return false;
}

static stock FailGymRepetitionInternal(playerid, E_GYM_EQUIPMENT_TYPE:type, index) {
    new const
        Float:left = gGymPlayerData[playerid][E_GYM_PLAYER_POWER] - GYM_FAIL_POWER_LOSS
    ;

    gGymPlayerData[playerid][E_GYM_PLAYER_POWER] = (left < 0.0) ? 0.0 : left;
    gGymPlayerData[playerid][E_GYM_PLAYER_REST_UNTIL] = gettime() + GYM_FAIL_REST_TIME;

    // The rest is not the player standing around doing nothing.
    gGymPlayerData[playerid][E_GYM_PLAYER_LAST_ACTIVITY] = gettime() + GYM_FAIL_REST_TIME;

    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            ApplyAnimation(playerid, "BENCHPRESS", "GYM_BP_DOWN", 4.1, false, false, false, true, 0, SYNC_ALL);
        }
        case GYM_EQUIPMENT_DUMBELL: {
            ApplyAnimation(playerid, "FREEWEIGHTS", "GYM_FREE_DOWN", 4.1, false, false, false, true, 0, SYNC_ALL);
        }
        default: {}
    }

    CallLocalFunction("OnPlayerFailRepetition", "iii", playerid, _:type, index);
}

static stock CompleteGymRepetitionInternal(playerid, E_GYM_EQUIPMENT_TYPE:type, index, session) {
    if (float(random(1000)) / 1000.0 < GetGymFailChanceInternal(playerid, type, index)) {
        FailGymRepetitionInternal(playerid, type, index);

        return;
    }

    gGymPlayerData[playerid][E_GYM_PLAYER_POWER] = 0.0;
    gGymPlayerData[playerid][E_GYM_PLAYER_METRIC]++;
    gGymPlayerData[playerid][E_GYM_PLAYER_LAST_ACTIVITY] = gettime() + GYM_REPETITION_GRACE;

    new const
        Float:effort = GetGymEffortFactorInternal(playerid, type, index)
    ;

    new const
        Float:spent = gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE] + GetGymFatigueCostInternal(effort)
    ;

    gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE] = (spent > GYM_FATIGUE_MAX) ? GYM_FATIGUE_MAX : spent;

    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            gGymPlayerData[playerid][E_GYM_PLAYER_LOWER_TIME] = GYM_BARBELL_LOWER_TIME;
        }
        case GYM_EQUIPMENT_DUMBELL: {
            gGymPlayerData[playerid][E_GYM_PLAYER_LOWER_TIME] = GYM_DUMBELL_LOWER_TIME;
        }
        default: {}
    }

    // The gamemode gets the repetition first: it knows what the weight should
    // look like on the character. Answering anything other than zero means it
    // played the animation itself.
    if (!CallLocalFunction("OnPlayerRepetition", "iiif", playerid, _:type, index, effort)) {
        PlayGymLiftAnimationInternal(playerid, type, effort);
    }

    SetTimerEx("@Gym_Lower", gGymPlayerData[playerid][E_GYM_PLAYER_LOWER_TIME], false, "ii", playerid, session);
}

static stock bool:IsGymRequestBlockedInternal(playerid, E_GYM_EQUIPMENT_TYPE:type, index) {
    if (funcidx("OnPlayerRequestExercise") == -1) {
        return false;
    }

    return !CallLocalFunction("OnPlayerRequestExercise", "iii", playerid, _:type, index);
}

static stock FinishGymExerciseInternal(playerid, bool:call = true) {
    new const
        E_GYM_EQUIPMENT_TYPE:type = gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
        index = gGymPlayerData[playerid][E_GYM_PLAYER_INDEX],
        metric = gGymPlayerData[playerid][E_GYM_PLAYER_METRIC]
    ;

    if (type == GYM_EQUIPMENT_NONE) {
        return;
    }

    SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_PLAYER_ID, INVALID_PLAYER_ID);

    // The muscle starts resting from here, and the fatigue itself is left alone
    // by the reset below: a set does not undo the one before it.
    gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE_TIME] = gettime();

    if (IsPlayerConnected(playerid)) {
        switch (type) {
            case GYM_EQUIPMENT_BARBELL, GYM_EQUIPMENT_DUMBELL: {
                RemovePlayerAttachedObject(playerid, GYM_ATTACH_SLOT_PRIMARY);
                RemovePlayerAttachedObject(playerid, GYM_ATTACH_SLOT_SECONDARY);
            }
            default: {}
        }

        ClearAnimations(playerid, SYNC_ALL);
        TogglePlayerControllable(playerid, true);

        if (gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_MOVED]) {
            SetCameraBehindPlayer(playerid);
        }
    }

    ResetGymPlayerDataInternal(playerid);

    if (call) {
        CallLocalFunction("OnPlayerFinishExercise", "iiii", playerid, _:type, index, metric);
    }
}

static stock bool:StartGymExerciseInternal(playerid, E_GYM_EQUIPMENT_TYPE:type, index) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_NONE) {
        return false;
    }

    if (!IsValidGymEquipmentInternal(type, index)) {
        return false;
    }

    if (gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_PLAYER_ID] != INVALID_PLAYER_ID) {
        return false;
    }

    if (!gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_ENABLED]) {
        return false;
    }

    if (IsGymRequestBlockedInternal(playerid, type, index)) {
        return false;
    }

    new const
        session = ++gGymSessionCounter
    ;

    SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_PLAYER_ID, playerid);

    gGymPlayerData[playerid][E_GYM_PLAYER_SPENT]         = false;

    gGymPlayerData[playerid][E_GYM_PLAYER_EXHAUSTION]    = GetGymExhaustionInternal(playerid);
    gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE]       = GetGymFatigueInternal(playerid);

    gGymPlayerData[playerid][E_GYM_PLAYER_TYPE]          = type;
    gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]         = index;
    gGymPlayerData[playerid][E_GYM_PLAYER_STATE]         = GYM_EXERCISE_GETTING_ON;
    gGymPlayerData[playerid][E_GYM_PLAYER_POWER]         = 0.0;
    gGymPlayerData[playerid][E_GYM_PLAYER_METRIC]        = 0;
    gGymPlayerData[playerid][E_GYM_PLAYER_LAST_ACTIVITY] = gettime();
    gGymPlayerData[playerid][E_GYM_PLAYER_SESSION]       = session;

    PlacePlayerOnGymEquipmentInternal(playerid, type, gGymEquipmentData[_:type][index]);

    SetTimerEx("@Gym_GetOn", GetGymGetOnTimeInternal(type), false, "ii", playerid, session);

    return true;
}

static stock PlayGymExitAnimationInternal(playerid, E_GYM_EQUIPMENT_TYPE:type) {
    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            ApplyAnimation(playerid, "BENCHPRESS", "GYM_BP_GETOFF", 4.1, false, false, false, false, 0, SYNC_ALL);

            return GYM_BARBELL_GET_OFF_TIME;
        }
        case GYM_EQUIPMENT_DUMBELL: {
            ApplyAnimation(playerid, "FREEWEIGHTS", "GYM_FREE_PUTDOWN", 4.1, false, false, false, true, 0, SYNC_ALL);

            return GYM_DUMBELL_GET_OFF_TIME;
        }
        case GYM_EQUIPMENT_TREADMILL: {
            switch (GetPlayerAnimationIndex(playerid)) {
                case GYM_ANIM_TREADMILL_SPRINT: {
                    ApplyAnimation(playerid, "GYMNASIUM", "GYM_TREAD_FALLOFF", 4.1, false, false, false, true, 0, SYNC_ALL);

                    return GYM_TREADMILL_FALL_OFF_TIME;
                }
                case GYM_ANIM_TREADMILL_TIRED, GYM_ANIM_TREADMILL_WALK: {
                    ApplyAnimation(playerid, "GYMNASIUM", "GYM_WALK_FALLOFF", 4.1, false, false, false, true, 0, SYNC_ALL);

                    return GYM_TREADMILL_GET_OFF_TIME;
                }
            }

            ApplyAnimation(playerid, "GYMNASIUM", "GYM_TREAD_GETOFF", 4.1, false, false, false, true, 0, SYNC_ALL);

            return GYM_TREADMILL_GET_OFF_TIME;
        }
        case GYM_EQUIPMENT_BIKE: {
            ApplyAnimation(playerid, "GYMNASIUM", "GYM_BIKE_GETOFF", 4.1, false, false, false, true, 0, SYNC_ALL);

            return GYM_BIKE_GET_OFF_TIME;
        }
        default: {}
    }

    return 0;
}

static stock bool:StopGymExerciseInternal(playerid) {
    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_ACTIVE) {
        return false;
    }

    KillGymUpdateTimerInternal(playerid);

    new const
        session = ++gGymSessionCounter
    ;

    gGymPlayerData[playerid][E_GYM_PLAYER_STATE] = GYM_EXERCISE_GETTING_OFF;
    gGymPlayerData[playerid][E_GYM_PLAYER_SESSION] = session;

    SetTimerEx("@Gym_GetOff", PlayGymExitAnimationInternal(playerid, gGymPlayerData[playerid][E_GYM_PLAYER_TYPE]), 
        false,
        "ii",
        playerid,
        session
    );

    return true;
}

/**
 * # Timers
 *
 * Every timer carries the session it was scheduled with, so a timer left over
 * from an exercise that already ended simply does nothing.
 */

#if GYM_CAMERA_MODE == GYM_CAMERA_MODE_ORBIT
forward @Gym_CameraHold(playerid, session);
#endif
forward @Gym_CameraArc(playerid, session);
forward @Gym_GetOn(playerid, session);
forward @Gym_Update(playerid, session);
forward @Gym_Lower(playerid, session);
forward @Gym_Spent(playerid, session);
forward @Gym_GetOff(playerid, session);
forward @Gym_Finish(playerid, session);

#if GYM_CAMERA_MODE == GYM_CAMERA_MODE_ORBIT
public @Gym_CameraHold(playerid, session) {
    if (!IsGymSessionValidInternal(playerid, session)) {
        return 0;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_ACTIVE) {
        KillGymCameraTimerInternal(playerid);

        return 0;
    }

    new const
        E_GYM_EQUIPMENT_TYPE:type = gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
        index = gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]
    ;

    if (!IsValidGymEquipmentInternal(type, index)) {
        KillGymCameraTimerInternal(playerid);

        return 0;
    }

    new const
        Float:angle = GetGymWrappedAngleInternal(gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_ANGLE] + GYM_CAMERA_HOLD_SPEED * float(GYM_CAMERA_HOLD_TICK) / 1000.0)
    ;

    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_ANGLE] = angle;

    ApplyGymCameraInternal(playerid, gGymEquipmentData[_:type][index], angle);

    return 0;
}
#endif

public @Gym_CameraArc(playerid, session) {
    if (!IsGymSessionValidInternal(playerid, session)) {
        return 0;
    }

    new const
        E_GYM_EQUIPMENT_TYPE:type = gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
        index = gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]
    ;

    if (!IsValidGymEquipmentInternal(type, index)) {
        KillGymCameraTimerInternal(playerid);

        return 0;
    }

    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_SEGMENT]++;

    new const
        segment = gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_SEGMENT],
        segments = gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_SEGMENTS]
    ;

    new const
        Float:baseAngle = gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_FROM],
        Float:delta = gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_DELTA]
    ;

    // The last piece is over: the camera is pinned where it landed, otherwise
    // the client takes it back once the interpolation ends.
    if (segment > segments) {
        KillGymCameraTimerInternal(playerid);
        ApplyGymCameraInternal(playerid, gGymEquipmentData[_:type][index], baseAngle + delta);

        return 0;
    }

    new const
        Float:fromAngle = baseAngle + delta * float(segment - 1) / float(segments),
        Float:toAngle = baseAngle + delta * float(segment) / float(segments),
        segmentTime = GYM_CAMERA_MOVE_TIME / segments
    ;

    MoveGymCameraInternal(playerid, gGymEquipmentData[_:type][index], fromAngle, toAngle, segmentTime);

    return 0;
}

public @Gym_GetOn(playerid, session) {
    if (!IsGymSessionValidInternal(playerid, session)) {
        return 0;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_GETTING_ON) {
        return 0;
    }

    new const
        E_GYM_EQUIPMENT_TYPE:type = gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
        index = gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]
    ;

    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            DestroyGymEquipmentPropsInternal(type, index);
            RefreshGymObjectsInternal(playerid);

            SetPlayerAttachedObject(playerid, GYM_ATTACH_SLOT_PRIMARY, GYM_MODEL_BARBELL, 6,
                0.040999, 0.034000, -0.128999,
                3.199999, -2.700021, 8.299994,
                1.000000, 1.000000, 1.000000
            );
        }
        case GYM_EQUIPMENT_DUMBELL: {
            DestroyGymEquipmentPropsInternal(type, index);
            RefreshGymObjectsInternal(playerid);

            SetPlayerAttachedObject(playerid, GYM_ATTACH_SLOT_PRIMARY, GYM_MODEL_DUMBELL_LEFT, 5);
            SetPlayerAttachedObject(playerid, GYM_ATTACH_SLOT_SECONDARY, GYM_MODEL_DUMBELL_RIGHT, 6);
        }
        case GYM_EQUIPMENT_TREADMILL: {
            ApplyAnimation(playerid, "GYMNASIUM", "GYM_TREAD_TIRED", 4.1, true, false, false, true, 0, SYNC_ALL);
        }
        case GYM_EQUIPMENT_BIKE: {
            ApplyAnimation(playerid, "GYMNASIUM", "GYM_BIKE_STILL", 4.1, true, false, false, true, 0, SYNC_ALL);
        }
        default: {}
    }

    gGymPlayerData[playerid][E_GYM_PLAYER_STATE]         = GYM_EXERCISE_ACTIVE;
    gGymPlayerData[playerid][E_GYM_PLAYER_POWER]         = 0.0;
    gGymPlayerData[playerid][E_GYM_PLAYER_METRIC]        = 0;
    gGymPlayerData[playerid][E_GYM_PLAYER_LAST_ACTIVITY] = gettime();
    gGymPlayerData[playerid][E_GYM_PLAYER_LAST_INPUT]    = GetTickCount();
    gGymPlayerData[playerid][E_GYM_PLAYER_TIMER]         = SetTimerEx("@Gym_Update", GYM_UPDATE_TIME, true, "ii", playerid, session);

    CallLocalFunction("OnPlayerStartExercise", "iii", playerid, _:type, index);

    return 0;
}

// A muscle gives out on the repetition that spent the last of it, or under a
// weight held still for long enough, and both end the same way: the weight goes
// down, the day is a set shorter and the weights stay on the floor for a while.
// The flag is what keeps that from happening twice on the way out.
static stock FailGymMuscleInternal(playerid, session) {
    if (gGymPlayerData[playerid][E_GYM_PLAYER_SPENT]) {
        return;
    }

    gGymPlayerData[playerid][E_GYM_PLAYER_SPENT] = true;

    new const
        E_GYM_EQUIPMENT_TYPE:type = gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
        index = gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]
    ;

    new const
        Float:day = gGymPlayerData[playerid][E_GYM_PLAYER_EXHAUSTION] + GYM_FATIGUE_MAX / GetGymFailuresInternal(playerid)
    ;

    gGymPlayerData[playerid][E_GYM_PLAYER_EXHAUSTION] = (day > GYM_FATIGUE_MAX) ? GYM_FATIGUE_MAX : day;

    // The weights go on the floor and stay there for a while: this is the rest
    // a set taken to failure asks for.
    gGymPlayerData[playerid][E_GYM_PLAYER_REST_UNTIL] = gettime() + GYM_MUSCLE_REST_TIME;

    CallLocalFunction("OnPlayerMuscleFailure", "iiii", playerid, _:type, index, gGymPlayerData[playerid][E_GYM_PLAYER_METRIC]);

    if (gGymPlayerData[playerid][E_GYM_PLAYER_EXHAUSTION] >= GYM_FATIGUE_MAX) {
        CallLocalFunction("OnPlayerGymExhausted", "iii", playerid, _:type, index);
    }

    SetTimerEx("@Gym_Spent", GYM_FAILURE_TIME, false, "ii", playerid, session);
}

public @Gym_Lower(playerid, session) {
    if (!IsGymSessionValidInternal(playerid, session)) {
        return 0;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_ACTIVE) {
        return 0;
    }

    switch (gGymPlayerData[playerid][E_GYM_PLAYER_TYPE]) {
        case GYM_EQUIPMENT_BARBELL: {
            ApplyAnimation(playerid, "BENCHPRESS", "GYM_BP_DOWN", 4.1, false, false, false, true, 0, SYNC_ALL);
        }
        case GYM_EQUIPMENT_DUMBELL: {
            ApplyAnimation(playerid, "FREEWEIGHTS", "GYM_FREE_DOWN", 4.1, false, false, false, true, 0, SYNC_ALL);
        }
        default: {}
    }

    // The weight is put down first and the player leaves after it, instead of
    // the set ending on top of an animation still playing.
    if (gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE] >= GYM_FATIGUE_MAX) {
        FailGymMuscleInternal(playerid, session);
    }

    return 0;
}

public @Gym_Spent(playerid, session) {
    if (!IsGymSessionValidInternal(playerid, session)) {
        return 0;
    }

    StopGymExerciseInternal(playerid);

    return 0;
}

public @Gym_GetOff(playerid, session) {
    if (!IsGymSessionValidInternal(playerid, session)) {
        return 0;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_GETTING_OFF) {
        return 0;
    }

    new const
        E_GYM_EQUIPMENT_TYPE:type = gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
        index = gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]
    ;

    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            CreateGymEquipmentPropsInternal(type, index);
            RefreshGymObjectsInternal(playerid);
            RemovePlayerAttachedObject(playerid, GYM_ATTACH_SLOT_PRIMARY);

            SetTimerEx("@Gym_Finish", GYM_BARBELL_FINISH_TIME, false, "ii", playerid, session);

            return 0;
        }
        case GYM_EQUIPMENT_DUMBELL: {
            CreateGymEquipmentPropsInternal(type, index);
            RefreshGymObjectsInternal(playerid);
            RemovePlayerAttachedObject(playerid, GYM_ATTACH_SLOT_PRIMARY);
            RemovePlayerAttachedObject(playerid, GYM_ATTACH_SLOT_SECONDARY);

            SetTimerEx("@Gym_Finish", GYM_DUMBELL_FINISH_TIME, false, "ii", playerid, session);

            return 0;
        }
        default: {}
    }

    FinishGymExerciseInternal(playerid);

    return 0;
}

public @Gym_Finish(playerid, session) {
    if (!IsGymSessionValidInternal(playerid, session)) {
        return 0;
    }

    FinishGymExerciseInternal(playerid);

    return 0;
}

// What a second of holding the weight still costs. A bar at the chest is worse
// than dumbbells at the sides, and a weight that is nothing to the player is
// easier to hold than one at their limit, but never free.
static stock Float:GetGymHoldCostInternal(playerid, E_GYM_EQUIPMENT_TYPE:type, index) {
    new
        Float:cost = 0.0
    ;

    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            cost = GYM_BARBELL_HOLD_COST;
        }
        case GYM_EQUIPMENT_DUMBELL: {
            cost = GYM_DUMBELL_HOLD_COST;
        }
        default: {}
    }

    new const
        Float:span = GYM_EFFORT_MAX - 1.0
    ;

    new
        Float:share = (span > 0.0) ? ((GetGymEffortFactorInternal(playerid, type, index) - 1.0) / span) : 1.0
    ;

    if (share < 0.0) {
        share = 0.0;
    }

    if (share > 1.0) {
        share = 1.0;
    }

    return cost * (1.0 - (1.0 - GYM_HOLD_EASE) * share);
}

// Holding a weight still is not a rest: it is a plank, and the muscle goes on
// spending until it gives out and the player is put off the equipment. Waiting
// under it does not count against the absent timeout, because somebody holding
// a bar over their chest is not somebody who walked away from the keyboard.
static stock HoldGymMuscleInternal(playerid, E_GYM_EQUIPMENT_TYPE:type, index, session) {
    if (!IsGymLiftingEquipmentInternal(type)) {
        return;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_SPENT]) {
        return;
    }

    // Halfway through a movement the weight is on its way somewhere and the
    // repetition is what pays for it. This is the same question the key asks
    // before it adds anything: the weight is down and nothing is moving.
    if (!IsGymPowerInputAllowedInternal(playerid)) {
        return;
    }

    if ((GetTickCount() - gGymPlayerData[playerid][E_GYM_PLAYER_LAST_INPUT]) < GYM_HOLD_TIME) {
        return;
    }

    new const
        Float:spent = gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE] + GetGymHoldCostInternal(playerid, type, index) * float(GYM_UPDATE_TIME) / 1000.0
    ;

    gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE] = (spent > GYM_FATIGUE_MAX) ? GYM_FATIGUE_MAX : spent;
    gGymPlayerData[playerid][E_GYM_PLAYER_LAST_ACTIVITY] = gettime();

    if (spent >= GYM_FATIGUE_MAX) {
        FailGymMuscleInternal(playerid, session);
    }
}

public @Gym_Update(playerid, session) {
    if (!IsGymSessionValidInternal(playerid, session)) {
        return 0;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_ACTIVE) {
        return 0;
    }

    new const
        E_GYM_EQUIPMENT_TYPE:type = gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
        index = gGymPlayerData[playerid][E_GYM_PLAYER_INDEX],
        animationIndex = GetPlayerAnimationIndex(playerid)
    ;

    new const
        Float:power = GetGymClampedPowerInternal(gGymPlayerData[playerid][E_GYM_PLAYER_POWER] - GetGymDecayInternal(playerid, type))
    ;

    gGymPlayerData[playerid][E_GYM_PLAYER_POWER] = power;

    HoldGymMuscleInternal(playerid, type, index, session);

    if ((gettime() - gGymPlayerData[playerid][E_GYM_PLAYER_LAST_ACTIVITY]) >= GYM_ABSENT_TIMEOUT) {
        StopGymExerciseInternal(playerid);

        return 0;
    }

    switch (type) {
        case GYM_EQUIPMENT_BARBELL, GYM_EQUIPMENT_DUMBELL: {
            if (power >= GYM_REPETITION_POWER) {
                CompleteGymRepetitionInternal(playerid, type, index, session);
            }
        }
        case GYM_EQUIPMENT_TREADMILL: {
            switch (GetGymSpeedLevelInternal(power)) {
                case GYM_SPEED_SLOW: {
                    if (animationIndex != GYM_ANIM_TREADMILL_WALK) {
                        ApplyAnimation(playerid, "GYMNASIUM", "GYM_TREAD_WALK", 4.1, true, false, false, true, 0, SYNC_ALL);
                    }

                    gGymPlayerData[playerid][E_GYM_PLAYER_METRIC] += 2;
                }
                case GYM_SPEED_MEDIUM: {
                    if (animationIndex != GYM_ANIM_TREADMILL_JOG) {
                        ApplyAnimation(playerid, "GYMNASIUM", "GYM_TREAD_JOG", 4.1, true, false, false, true, 0, SYNC_ALL);
                    }

                    gGymPlayerData[playerid][E_GYM_PLAYER_METRIC] += 5;
                }
                case GYM_SPEED_FAST: {
                    if (animationIndex != GYM_ANIM_TREADMILL_SPRINT) {
                        ApplyAnimation(playerid, "GYMNASIUM", "GYM_TREAD_SPRINT", 4.1, true, false, false, true, 0, SYNC_ALL);
                    }

                    gGymPlayerData[playerid][E_GYM_PLAYER_METRIC] += 10;
                }
                case GYM_SPEED_IDLE: {
                    if (animationIndex != GYM_ANIM_TREADMILL_TIRED) {
                        ApplyAnimation(playerid, "GYMNASIUM", "GYM_TREAD_TIRED", 4.1, true, false, false, true, 0, SYNC_ALL);
                    }
                }
            }

            if (animationIndex != GYM_ANIM_TREADMILL_TIRED) {
                gGymPlayerData[playerid][E_GYM_PLAYER_LAST_ACTIVITY] = gettime();
            }
        }
        case GYM_EQUIPMENT_BIKE: {
            switch (GetGymSpeedLevelInternal(power)) {
                case GYM_SPEED_SLOW: {
                    if (animationIndex != GYM_ANIM_BIKE_SLOW) {
                        ApplyAnimation(playerid, "GYMNASIUM", "GYM_BIKE_SLOW", 4.1, true, false, false, true, 0, SYNC_ALL);
                    }

                    gGymPlayerData[playerid][E_GYM_PLAYER_METRIC] += 2;
                }
                case GYM_SPEED_MEDIUM: {
                    if (animationIndex != GYM_ANIM_BIKE_PEDAL) {
                        ApplyAnimation(playerid, "GYMNASIUM", "GYM_BIKE_PEDAL", 4.1, true, false, false, true, 0, SYNC_ALL);
                    }

                    gGymPlayerData[playerid][E_GYM_PLAYER_METRIC] += 5;
                }
                case GYM_SPEED_FAST: {
                    if (animationIndex != GYM_ANIM_BIKE_FAST) {
                        ApplyAnimation(playerid, "GYMNASIUM", "GYM_BIKE_FAST", 4.1, true, false, false, true, 0, SYNC_ALL);
                    }

                    gGymPlayerData[playerid][E_GYM_PLAYER_METRIC] += 10;
                }
                case GYM_SPEED_IDLE: {
                    if (animationIndex != GYM_ANIM_BIKE_STILL) {
                        ApplyAnimation(playerid, "GYMNASIUM", "GYM_BIKE_STILL", 4.1, true, false, false, true, 0, SYNC_ALL);
                    }
                }
            }

            if (animationIndex != GYM_ANIM_BIKE_STILL) {
                gGymPlayerData[playerid][E_GYM_PLAYER_LAST_ACTIVITY] = gettime();
            }
        }
        default: {}
    }

    CallLocalFunction("OnPlayerUpdateExercise", "iiifi",
        playerid,
        _:type,
        index,
        gGymPlayerData[playerid][E_GYM_PLAYER_POWER],
        gGymPlayerData[playerid][E_GYM_PLAYER_METRIC]
    );

    return 0;
}

/**
 * # Equipment Internal Functions
 */

static stock CreateGymEquipmentInternal(E_GYM_EQUIPMENT_TYPE:type, Float:x, Float:y, Float:z, Float:angle, Float:size, worldid, interiorid, priority, Float:cameraDistance, Float:cameraHeight, Float:cameraAngle) {
    if (!(0 <= _:type < GYM_EQUIPMENT_TYPES)) {
        return -1;
    }

    new
        data[E_GYM_EQUIPMENT_DATA]
    ;

    data[E_GYM_EQUIPMENT_X]                 = x;
    data[E_GYM_EQUIPMENT_Y]                 = y;
    data[E_GYM_EQUIPMENT_Z]                 = z;
    data[E_GYM_EQUIPMENT_ANGLE]             = angle;
    data[E_GYM_EQUIPMENT_SIZE]              = size;

    data[E_GYM_EQUIPMENT_WEIGHT]            = GetGymDefaultWeightInternal(type);
    data[E_GYM_EQUIPMENT_ENABLED]           = true;

    data[E_GYM_EQUIPMENT_CAMERA_DISTANCE]   = cameraDistance;
    data[E_GYM_EQUIPMENT_CAMERA_HEIGHT]     = cameraHeight;
    data[E_GYM_EQUIPMENT_CAMERA_ANGLE]      = cameraAngle;
    data[E_GYM_EQUIPMENT_CAMERA_STEPS]      = GYM_CAMERA_STEPS;
    data[E_GYM_EQUIPMENT_CAMERA_STEP_ANGLE] = GYM_CAMERA_STEP_ANGLE;

    data[E_GYM_EQUIPMENT_WORLD_ID]          = worldid;
    data[E_GYM_EQUIPMENT_INTERIOR_ID]       = interiorid;
    data[E_GYM_EQUIPMENT_PLAYER_ID]         = INVALID_PLAYER_ID;

    data[E_GYM_EQUIPMENT_FRAME_ID]          = GYM_INVALID_OBJECT_ID;
    data[E_GYM_EQUIPMENT_PROP_ID][0]        = GYM_INVALID_OBJECT_ID;
    data[E_GYM_EQUIPMENT_PROP_ID][1]        = GYM_INVALID_OBJECT_ID;

    switch (type) {
        case GYM_EQUIPMENT_BARBELL: {
            data[E_GYM_EQUIPMENT_FRAME_ID] = CreateGymObjectInternal(GYM_MODEL_BENCH, x, y, z, 0.0, 0.0, angle, worldid, interiorid);
        }
        case GYM_EQUIPMENT_TREADMILL: {
            data[E_GYM_EQUIPMENT_FRAME_ID] = CreateGymObjectInternal(GYM_MODEL_TREADMILL, x, y, z, 0.0, 0.0, angle, worldid, interiorid);
        }
        case GYM_EQUIPMENT_BIKE: {
            data[E_GYM_EQUIPMENT_FRAME_ID] = CreateGymObjectInternal(GYM_MODEL_BIKE, x, y, z, 0.0, 0.0, angle, worldid, interiorid);
        }
        default: {}
    }

    // button.inc builds the area, keeps the priority and watches the key, so
    // the equipment does none of that for itself.
    new const
        Button:buttonid = CreateButton(x, y, z, GYM_ENTER_KEY, size, worldid, interiorid, priority)
    ;

    if (buttonid == INVALID_BUTTON_ID) {
        DestroyGymObjectInternal(data[E_GYM_EQUIPMENT_FRAME_ID]);

        return -1;
    }

    data[E_GYM_EQUIPMENT_BUTTON_ID] = buttonid;

    new const
        index = AllocGymEquipmentInternal(type)
    ;

    if (index == -1) {
        DestroyGymObjectInternal(data[E_GYM_EQUIPMENT_FRAME_ID]);
        DestroyButton(buttonid);

        return -1;
    }

    data[E_GYM_EQUIPMENT_CREATED] = true;

    gGymEquipmentData[_:type][index] = data;

    gGymButtonData[buttonid][E_GYM_BUTTON_OWNED] = true;
    gGymButtonData[buttonid][E_GYM_BUTTON_TYPE] = type;
    gGymButtonData[buttonid][E_GYM_BUTTON_INDEX] = index;

    SetGymEquipmentPopupInternal(type, buttonid);

    CreateGymEquipmentPropsInternal(type, index);

    return index;
}

static stock bool:DestroyGymEquipmentInternal(E_GYM_EQUIPMENT_TYPE:type, index) {
    if (!IsValidGymEquipmentInternal(type, index)) {
        return false;
    }

    new const
        playerid = gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_PLAYER_ID]
    ;

    if (0 <= playerid < MAX_PLAYERS) {
        FinishGymExerciseInternal(playerid);
    }

    DestroyGymObjectInternal(gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_FRAME_ID]);
    DestroyGymEquipmentPropsInternal(type, index);

    DestroyButton(gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_BUTTON_ID]);

    // The slot goes back to the pool empty, so whatever is built on it next
    // starts from nothing rather than from the equipment that was here.
    static const
        GYM_EQUIPMENT_DATA_NONE[E_GYM_EQUIPMENT_DATA]
    ;

    gGymEquipmentData[_:type][index] = GYM_EQUIPMENT_DATA_NONE;

    FreeGymEquipmentInternal(type, index);

    return true;
}

static stock bool:GetGymEquipmentPosInternal(E_GYM_EQUIPMENT_TYPE:type, index, &Float:x, &Float:y, &Float:z, &Float:angle) {
    if (!IsValidGymEquipmentInternal(type, index)) {
        return false;
    }

    x = gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_X];
    y = gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_Y];
    z = gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_Z];
    angle = gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_ANGLE];

    return true;
}

static stock GetGymEquipmentPlayerInternal(E_GYM_EQUIPMENT_TYPE:type, index) {
    if (!IsValidGymEquipmentInternal(type, index)) {
        return INVALID_PLAYER_ID;
    }

    return GetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_PLAYER_ID);
}

static stock bool:SetGymEquipmentCameraInternal(E_GYM_EQUIPMENT_TYPE:type, index, Float:distance, Float:height, Float:cameraAngle, steps, Float:stepAngle) {
    if (!IsValidGymEquipmentInternal(type, index)) {
        return false;
    }

    SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_CAMERA_DISTANCE, distance);
    SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_CAMERA_HEIGHT, height);
    SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_CAMERA_ANGLE, cameraAngle);
    SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_CAMERA_STEPS, steps);
    SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_CAMERA_STEP_ANGLE, stepAngle);

    return true;
}

static stock bool:GetGymEquipmentCameraInternal(E_GYM_EQUIPMENT_TYPE:type, index, &Float:distance, &Float:height, &Float:cameraAngle, &steps, &Float:stepAngle) {
    if (!IsValidGymEquipmentInternal(type, index)) {
        return false;
    }

    distance = gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_CAMERA_DISTANCE];
    height = gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_CAMERA_HEIGHT];
    cameraAngle = gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_CAMERA_ANGLE];
    steps = gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_CAMERA_STEPS];
    stepAngle = gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_CAMERA_STEP_ANGLE];

    return true;
}

static stock bool:SetGymEquipmentWeightInternal(E_GYM_EQUIPMENT_TYPE:type, index, Float:weight) {
    if (weight < 0.0) {
        return false;
    }

    return SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_WEIGHT, weight);
}

static stock bool:SetGymEquipmentEnabledInternal(E_GYM_EQUIPMENT_TYPE:type, index, bool:enabled) {
    return SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_ENABLED, enabled);
}

static stock CountGymEquipmentInternal(E_GYM_EQUIPMENT_TYPE:type) {
    if (!(0 <= _:type < GYM_EQUIPMENT_TYPES)) {
        return 0;
    }

    return gGymEquipmentCount[_:type];
}

/**
 * # External Functions
 */

/**
 * ## Barbell
 */

stock Barbell:CreateBarbell(Float:x, Float:y, Float:z, Float:angle, Float:size = GYM_AREA_SIZE, worldid = -1, interiorid = -1, priority = 0, Float:cameraDistance = GYM_BARBELL_CAMERA_DISTANCE, Float:cameraHeight = GYM_BARBELL_CAMERA_HEIGHT, Float:cameraAngle = GYM_BARBELL_CAMERA_ANGLE) {
    return Barbell:CreateGymEquipmentInternal(GYM_EQUIPMENT_BARBELL, x, y, z, angle, size, worldid, interiorid, priority, cameraDistance, cameraHeight, cameraAngle);
}

stock bool:DestroyBarbell(Barbell:barbellid) {
    return DestroyGymEquipmentInternal(GYM_EQUIPMENT_BARBELL, _:barbellid);
}

stock bool:IsValidBarbell(Barbell:barbellid) {
    return IsValidGymEquipmentInternal(GYM_EQUIPMENT_BARBELL, _:barbellid);
}

stock bool:GetBarbellPos(Barbell:barbellid, &Float:x, &Float:y, &Float:z, &Float:angle) {
    return GetGymEquipmentPosInternal(GYM_EQUIPMENT_BARBELL, _:barbellid, x, y, z, angle);
}

stock bool:SetBarbellCamera(Barbell:barbellid, Float:distance, Float:height, Float:angle, steps = GYM_CAMERA_STEPS, Float:stepAngle = GYM_CAMERA_STEP_ANGLE) {
    return SetGymEquipmentCameraInternal(GYM_EQUIPMENT_BARBELL, _:barbellid, distance, height, angle, steps, stepAngle);
}

stock bool:GetBarbellCamera(Barbell:barbellid, &Float:distance, &Float:height, &Float:angle, &steps = 0, &Float:stepAngle = 0.0) {
    return GetGymEquipmentCameraInternal(GYM_EQUIPMENT_BARBELL, _:barbellid, distance, height, angle, steps, stepAngle);
}

stock bool:SetBarbellWeight(Barbell:barbellid, Float:weight) {
    return SetGymEquipmentWeightInternal(GYM_EQUIPMENT_BARBELL, _:barbellid, weight);
}

stock Float:GetBarbellWeight(Barbell:barbellid) {
    return GetGymEquipmentWeight(GYM_EQUIPMENT_BARBELL, _:barbellid);
}

stock bool:SetBarbellEnabled(Barbell:barbellid, bool:enabled) {
    return SetGymEquipmentEnabledInternal(GYM_EQUIPMENT_BARBELL, _:barbellid, enabled);
}

stock bool:IsBarbellEnabled(Barbell:barbellid) {
    return bool:GetGymEquipmentCellInternal(GYM_EQUIPMENT_BARBELL, _:barbellid, _:E_GYM_EQUIPMENT_ENABLED);
}

stock GetBarbellPlayer(Barbell:barbellid) {
    return GetGymEquipmentPlayerInternal(GYM_EQUIPMENT_BARBELL, _:barbellid);
}

stock bool:IsBarbellOccupied(Barbell:barbellid) {
    return GetGymEquipmentPlayerInternal(GYM_EQUIPMENT_BARBELL, _:barbellid) != INVALID_PLAYER_ID;
}

stock CountBarbell() {
    return CountGymEquipmentInternal(GYM_EQUIPMENT_BARBELL);
}

stock bool:PutPlayerOnBarbell(playerid, Barbell:barbellid) {
    return StartGymExerciseInternal(playerid, GYM_EQUIPMENT_BARBELL, _:barbellid);
}

stock Barbell:GetPlayerBarbell(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return INVALID_BARBELL_ID;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] == GYM_EXERCISE_NONE) {
        return INVALID_BARBELL_ID;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_TYPE] != GYM_EQUIPMENT_BARBELL) {
        return INVALID_BARBELL_ID;
    }

    return Barbell:gGymPlayerData[playerid][E_GYM_PLAYER_INDEX];
}

/**
 * ## Dumbell
 */

stock Dumbell:CreateDumbell(Float:x, Float:y, Float:z, Float:angle, Float:size = GYM_AREA_SIZE, worldid = -1, interiorid = -1, priority = 0, Float:cameraDistance = GYM_DUMBELL_CAMERA_DISTANCE, Float:cameraHeight = GYM_DUMBELL_CAMERA_HEIGHT, Float:cameraAngle = GYM_DUMBELL_CAMERA_ANGLE) {
    return Dumbell:CreateGymEquipmentInternal(GYM_EQUIPMENT_DUMBELL, x, y, z, angle, size, worldid, interiorid, priority, cameraDistance, cameraHeight, cameraAngle);
}

stock bool:DestroyDumbell(Dumbell:dumbellid) {
    return DestroyGymEquipmentInternal(GYM_EQUIPMENT_DUMBELL, _:dumbellid);
}

stock bool:IsValidDumbell(Dumbell:dumbellid) {
    return IsValidGymEquipmentInternal(GYM_EQUIPMENT_DUMBELL, _:dumbellid);
}

stock bool:GetDumbellPos(Dumbell:dumbellid, &Float:x, &Float:y, &Float:z, &Float:angle) {
    return GetGymEquipmentPosInternal(GYM_EQUIPMENT_DUMBELL, _:dumbellid, x, y, z, angle);
}

stock bool:SetDumbellCamera(Dumbell:dumbellid, Float:distance, Float:height, Float:angle, steps = GYM_CAMERA_STEPS, Float:stepAngle = GYM_CAMERA_STEP_ANGLE) {
    return SetGymEquipmentCameraInternal(GYM_EQUIPMENT_DUMBELL, _:dumbellid, distance, height, angle, steps, stepAngle);
}

stock bool:GetDumbellCamera(Dumbell:dumbellid, &Float:distance, &Float:height, &Float:angle, &steps = 0, &Float:stepAngle = 0.0) {
    return GetGymEquipmentCameraInternal(GYM_EQUIPMENT_DUMBELL, _:dumbellid, distance, height, angle, steps, stepAngle);
}

stock bool:SetDumbellWeight(Dumbell:dumbellid, Float:weight) {
    return SetGymEquipmentWeightInternal(GYM_EQUIPMENT_DUMBELL, _:dumbellid, weight);
}

stock Float:GetDumbellWeight(Dumbell:dumbellid) {
    return GetGymEquipmentWeight(GYM_EQUIPMENT_DUMBELL, _:dumbellid);
}

stock bool:SetDumbellEnabled(Dumbell:dumbellid, bool:enabled) {
    return SetGymEquipmentEnabledInternal(GYM_EQUIPMENT_DUMBELL, _:dumbellid, enabled);
}

stock bool:IsDumbellEnabled(Dumbell:dumbellid) {
    return bool:GetGymEquipmentCellInternal(GYM_EQUIPMENT_DUMBELL, _:dumbellid, _:E_GYM_EQUIPMENT_ENABLED);
}

stock GetDumbellPlayer(Dumbell:dumbellid) {
    return GetGymEquipmentPlayerInternal(GYM_EQUIPMENT_DUMBELL, _:dumbellid);
}

stock bool:IsDumbellOccupied(Dumbell:dumbellid) {
    return GetGymEquipmentPlayerInternal(GYM_EQUIPMENT_DUMBELL, _:dumbellid) != INVALID_PLAYER_ID;
}

stock CountDumbell() {
    return CountGymEquipmentInternal(GYM_EQUIPMENT_DUMBELL);
}

stock bool:PutPlayerOnDumbell(playerid, Dumbell:dumbellid) {
    return StartGymExerciseInternal(playerid, GYM_EQUIPMENT_DUMBELL, _:dumbellid);
}

stock Dumbell:GetPlayerDumbell(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return INVALID_DUMBELL_ID;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] == GYM_EXERCISE_NONE) {
        return INVALID_DUMBELL_ID;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_TYPE] != GYM_EQUIPMENT_DUMBELL) {
        return INVALID_DUMBELL_ID;
    }

    return Dumbell:gGymPlayerData[playerid][E_GYM_PLAYER_INDEX];
}

/**
 * ## Treadmill
 */

stock Treadmill:CreateTreadmill(Float:x, Float:y, Float:z, Float:angle, Float:size = GYM_AREA_SIZE, worldid = -1, interiorid = -1, priority = 0, Float:cameraDistance = GYM_TREADMILL_CAMERA_DISTANCE, Float:cameraHeight = GYM_TREADMILL_CAMERA_HEIGHT, Float:cameraAngle = GYM_TREADMILL_CAMERA_ANGLE) {
    return Treadmill:CreateGymEquipmentInternal(GYM_EQUIPMENT_TREADMILL, x, y, z, angle, size, worldid, interiorid, priority, cameraDistance, cameraHeight, cameraAngle);
}

stock bool:DestroyTreadmill(Treadmill:treadmillid) {
    return DestroyGymEquipmentInternal(GYM_EQUIPMENT_TREADMILL, _:treadmillid);
}

stock bool:IsValidTreadmill(Treadmill:treadmillid) {
    return IsValidGymEquipmentInternal(GYM_EQUIPMENT_TREADMILL, _:treadmillid);
}

stock bool:GetTreadmillPos(Treadmill:treadmillid, &Float:x, &Float:y, &Float:z, &Float:angle) {
    return GetGymEquipmentPosInternal(GYM_EQUIPMENT_TREADMILL, _:treadmillid, x, y, z, angle);
}

stock bool:SetTreadmillCamera(Treadmill:treadmillid, Float:distance, Float:height, Float:angle, steps = GYM_CAMERA_STEPS, Float:stepAngle = GYM_CAMERA_STEP_ANGLE) {
    return SetGymEquipmentCameraInternal(GYM_EQUIPMENT_TREADMILL, _:treadmillid, distance, height, angle, steps, stepAngle);
}

stock bool:GetTreadmillCamera(Treadmill:treadmillid, &Float:distance, &Float:height, &Float:angle, &steps = 0, &Float:stepAngle = 0.0) {
    return GetGymEquipmentCameraInternal(GYM_EQUIPMENT_TREADMILL, _:treadmillid, distance, height, angle, steps, stepAngle);
}

stock bool:SetTreadmillWeight(Treadmill:treadmillid, Float:weight) {
    return SetGymEquipmentWeightInternal(GYM_EQUIPMENT_TREADMILL, _:treadmillid, weight);
}

stock Float:GetTreadmillWeight(Treadmill:treadmillid) {
    return GetGymEquipmentWeight(GYM_EQUIPMENT_TREADMILL, _:treadmillid);
}

stock bool:SetTreadmillEnabled(Treadmill:treadmillid, bool:enabled) {
    return SetGymEquipmentEnabledInternal(GYM_EQUIPMENT_TREADMILL, _:treadmillid, enabled);
}

stock bool:IsTreadmillEnabled(Treadmill:treadmillid) {
    return bool:GetGymEquipmentCellInternal(GYM_EQUIPMENT_TREADMILL, _:treadmillid, _:E_GYM_EQUIPMENT_ENABLED);
}

stock GetTreadmillPlayer(Treadmill:treadmillid) {
    return GetGymEquipmentPlayerInternal(GYM_EQUIPMENT_TREADMILL, _:treadmillid);
}

stock bool:IsTreadmillOccupied(Treadmill:treadmillid) {
    return GetGymEquipmentPlayerInternal(GYM_EQUIPMENT_TREADMILL, _:treadmillid) != INVALID_PLAYER_ID;
}

stock CountTreadmill() {
    return CountGymEquipmentInternal(GYM_EQUIPMENT_TREADMILL);
}

stock bool:PutPlayerOnTreadmill(playerid, Treadmill:treadmillid) {
    return StartGymExerciseInternal(playerid, GYM_EQUIPMENT_TREADMILL, _:treadmillid);
}

stock Treadmill:GetPlayerTreadmill(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return INVALID_TREADMILL_ID;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] == GYM_EXERCISE_NONE) {
        return INVALID_TREADMILL_ID;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_TYPE] != GYM_EQUIPMENT_TREADMILL) {
        return INVALID_TREADMILL_ID;
    }

    return Treadmill:gGymPlayerData[playerid][E_GYM_PLAYER_INDEX];
}

/**
 * ## Bike
 */

stock Bike:CreateBike(Float:x, Float:y, Float:z, Float:angle, Float:size = GYM_AREA_SIZE, worldid = -1, interiorid = -1, priority = 0, Float:cameraDistance = GYM_BIKE_CAMERA_DISTANCE, Float:cameraHeight = GYM_BIKE_CAMERA_HEIGHT, Float:cameraAngle = GYM_BIKE_CAMERA_ANGLE) {
    return Bike:CreateGymEquipmentInternal(GYM_EQUIPMENT_BIKE, x, y, z, angle, size, worldid, interiorid, priority, cameraDistance, cameraHeight, cameraAngle);
}

stock bool:DestroyBike(Bike:bikeid) {
    return DestroyGymEquipmentInternal(GYM_EQUIPMENT_BIKE, _:bikeid);
}

stock bool:IsValidBike(Bike:bikeid) {
    return IsValidGymEquipmentInternal(GYM_EQUIPMENT_BIKE, _:bikeid);
}

stock bool:GetBikePos(Bike:bikeid, &Float:x, &Float:y, &Float:z, &Float:angle) {
    return GetGymEquipmentPosInternal(GYM_EQUIPMENT_BIKE, _:bikeid, x, y, z, angle);
}

stock bool:SetBikeCamera(Bike:bikeid, Float:distance, Float:height, Float:angle, steps = GYM_CAMERA_STEPS, Float:stepAngle = GYM_CAMERA_STEP_ANGLE) {
    return SetGymEquipmentCameraInternal(GYM_EQUIPMENT_BIKE, _:bikeid, distance, height, angle, steps, stepAngle);
}

stock bool:GetBikeCamera(Bike:bikeid, &Float:distance, &Float:height, &Float:angle, &steps = 0, &Float:stepAngle = 0.0) {
    return GetGymEquipmentCameraInternal(GYM_EQUIPMENT_BIKE, _:bikeid, distance, height, angle, steps, stepAngle);
}

stock bool:SetBikeWeight(Bike:bikeid, Float:weight) {
    return SetGymEquipmentWeightInternal(GYM_EQUIPMENT_BIKE, _:bikeid, weight);
}

stock Float:GetBikeWeight(Bike:bikeid) {
    return GetGymEquipmentWeight(GYM_EQUIPMENT_BIKE, _:bikeid);
}

stock bool:SetBikeEnabled(Bike:bikeid, bool:enabled) {
    return SetGymEquipmentEnabledInternal(GYM_EQUIPMENT_BIKE, _:bikeid, enabled);
}

stock bool:IsBikeEnabled(Bike:bikeid) {
    return bool:GetGymEquipmentCellInternal(GYM_EQUIPMENT_BIKE, _:bikeid, _:E_GYM_EQUIPMENT_ENABLED);
}

stock GetBikePlayer(Bike:bikeid) {
    return GetGymEquipmentPlayerInternal(GYM_EQUIPMENT_BIKE, _:bikeid);
}

stock bool:IsBikeOccupied(Bike:bikeid) {
    return GetGymEquipmentPlayerInternal(GYM_EQUIPMENT_BIKE, _:bikeid) != INVALID_PLAYER_ID;
}

stock CountBike() {
    return CountGymEquipmentInternal(GYM_EQUIPMENT_BIKE);
}

stock bool:PutPlayerOnBike(playerid, Bike:bikeid) {
    return StartGymExerciseInternal(playerid, GYM_EQUIPMENT_BIKE, _:bikeid);
}

stock Bike:GetPlayerBike(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return INVALID_BIKE_ID;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] == GYM_EXERCISE_NONE) {
        return INVALID_BIKE_ID;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_TYPE] != GYM_EQUIPMENT_BIKE) {
        return INVALID_BIKE_ID;
    }

    return Bike:gGymPlayerData[playerid][E_GYM_PLAYER_INDEX];
}

stock Float:GetGymEquipmentWeight(E_GYM_EQUIPMENT_TYPE:type, index) {
    if (!IsValidGymEquipmentInternal(type, index)) {
        return 0.0;
    }

    return Float:GetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_WEIGHT);
}

stock bool:SetGymEquipmentWeight(E_GYM_EQUIPMENT_TYPE:type, index, Float:weight) {
    return SetGymEquipmentWeightInternal(type, index, weight);
}

stock bool:SetGymEquipmentEnabled(E_GYM_EQUIPMENT_TYPE:type, index, bool:enabled) {
    return SetGymEquipmentEnabledInternal(type, index, enabled);
}

stock bool:IsGymEquipmentEnabled(E_GYM_EQUIPMENT_TYPE:type, index) {
    if (!IsValidGymEquipmentInternal(type, index)) {
        return false;
    }

    return bool:GetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_ENABLED);
}

stock Float:GetGymEquipmentRequiredStrength(E_GYM_EQUIPMENT_TYPE:type, index) {
    if (!IsValidGymEquipmentInternal(type, index)) {
        return 0.0;
    }

    return GetGymRequiredStrengthInternal(type, index);
}

stock bool:CanPlayerLiftGymEquipment(playerid, E_GYM_EQUIPMENT_TYPE:type, index) {
    if (!IsValidGymEquipmentInternal(type, index)) {
        return false;
    }

    new const
        Float:required = GetGymRequiredStrengthInternal(type, index)
    ;

    if (required <= 0.0) {
        return true;
    }

    return required < gGymPlayerData[playerid][E_GYM_PLAYER_STRENGTH] * GYM_FAIL_LIMIT;
}

stock E_GYM_SPEED_LEVEL:GetPlayerExerciseSpeed(playerid) {
    if (!IsPlayerExercising(playerid)) {
        return GYM_SPEED_IDLE;
    }

    return GetGymSpeedLevelInternal(gGymPlayerData[playerid][E_GYM_PLAYER_POWER]);
}

stock bool:CanPlayerStopExercise(playerid) {
    if (!IsPlayerExercising(playerid)) {
        return false;
    }

    return IsGymDismountAllowedInternal(playerid);
}

stock Float:GetPlayerGymFatigue(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return 0.0;
    }

    return GetGymFatigueInternal(playerid);
}

stock bool:SetPlayerGymFatigue(playerid, Float:fatigue) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE] = (fatigue < 0.0) ? 0.0 : ((fatigue > GYM_FATIGUE_MAX) ? GYM_FATIGUE_MAX : fatigue);
    gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE_TIME] = gettime();

    return true;
}

stock Float:GetPlayerGymExhaustion(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return 0.0;
    }

    return GetGymExhaustionInternal(playerid);
}

stock bool:SetPlayerGymExhaustion(playerid, Float:exhaustion) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    gGymPlayerData[playerid][E_GYM_PLAYER_EXHAUSTION] = (exhaustion < 0.0) ? 0.0 : ((exhaustion > GYM_FATIGUE_MAX) ? GYM_FATIGUE_MAX : exhaustion);
    gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE_TIME] = gettime();

    return true;
}

stock GetPlayerGymRestTime(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return 0;
    }

    new const
        left = gGymPlayerData[playerid][E_GYM_PLAYER_REST_UNTIL] - gettime()
    ;

    return (left < 0) ? 0 : left;
}

stock Float:GetPlayerGymRecovery(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return 0.0;
    }

    return GetGymRecoveryInternal(playerid);
}

stock bool:SetPlayerGymRecovery(playerid, Float:recovery) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    gGymPlayerData[playerid][E_GYM_PLAYER_EXHAUSTION]   = GetGymExhaustionInternal(playerid);
    gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE]      = GetGymFatigueInternal(playerid);
    gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE_TIME] = gettime();
    gGymPlayerData[playerid][E_GYM_PLAYER_RECOVERY]     = (recovery < 0.0) ? 0.0 : recovery;

    return true;
}

stock GetPlayerGymSetsLeft(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return 0;
    }

    new const
        Float:left = GYM_FATIGUE_MAX - GetGymExhaustionInternal(playerid)
    ;

    if (left <= 0.0) {
        return 0;
    }

    return floatround(left / (GYM_FATIGUE_MAX / GetGymFailuresInternal(playerid)), floatround_ceil);
}

stock GetGymRepetitionsLeft(playerid, E_GYM_EQUIPMENT_TYPE:type, index) {
    if (!IsValidGymEquipmentInternal(type, index)) {
        return 0;
    }

    // Nothing is lifted on a treadmill, so there is nothing to count.
    if (!IsGymLiftingEquipmentInternal(type)) {
        return 0;
    }

    if (GetGymExhaustionInternal(playerid) >= GYM_FATIGUE_MAX) {
        return 0;
    }

    if (IsGymRestingInternal(playerid, type)) {
        return 0;
    }

    new const
        Float:cost = GetGymFatigueCostInternal(GetGymEffortFactorInternal(playerid, type, index)),
        Float:left = GYM_FATIGUE_MAX - GetGymFatigueInternal(playerid)
    ;

    if (left <= 0.0) {
        return 0;
    }

    return floatround(left / cost, floatround_ceil);
}

stock Float:GetPlayerExerciseEffort(playerid) {
    if (!IsPlayerExercising(playerid)) {
        return 1.0;
    }

    return GetGymEffortFactorInternal(
        playerid,
        gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
        gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]
    );
}

stock Float:GetPlayerExerciseRequiredStrength(playerid) {
    if (!IsPlayerExercising(playerid)) {
        return 0.0;
    }

    return GetGymRequiredStrengthInternal(
        gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
        gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]
    );
}

stock bool:SetPlayerRepetitionTime(playerid, time) {
    if (!IsPlayerExercising(playerid)) {
        return false;
    }

    if (time <= 0) {
        return false;
    }

    gGymPlayerData[playerid][E_GYM_PLAYER_LOWER_TIME] = time;

    return true;
}

stock bool:SetPlayerGymStrength(playerid, Float:strength) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    gGymPlayerData[playerid][E_GYM_PLAYER_STRENGTH] = GetGymClampedPowerInternal(strength);

    return true;
}

stock Float:GetPlayerGymStrength(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return 0.0;
    }

    return gGymPlayerData[playerid][E_GYM_PLAYER_STRENGTH];
}

/**
 * ## Player
 */

// What one button is worth to the gym: the equipment behind it, if it is
// equipment at all, if that equipment still exists, and if it can be used.
static stock bool:GetGymButtonEquipmentInternal(Button:buttonid, &E_GYM_EQUIPMENT_TYPE:type, &index, bool:freeOnly) {
    if (buttonid == INVALID_BUTTON_ID) {
        return false;
    }

    if (!gGymButtonData[buttonid][E_GYM_BUTTON_OWNED]) {
        return false;
    }

    new const
        E_GYM_EQUIPMENT_TYPE:currentType = gGymButtonData[buttonid][E_GYM_BUTTON_TYPE],
        currentIndex = gGymButtonData[buttonid][E_GYM_BUTTON_INDEX]
    ;

    if (!IsValidGymEquipmentInternal(currentType, currentIndex)) {
        return false;
    }

    if (freeOnly && GetGymEquipmentPlayerInternal(currentType, currentIndex) != INVALID_PLAYER_ID) {
        return false;
    }

    type = currentType;
    index = currentIndex;

    return true;
}

// The button a piece of equipment was built on, so a script can change what its
// prompt says, move it, or hang its own data off it.
stock Button:GetBarbellButton(Barbell:barbellid) {
    return Button:GetGymEquipmentCellInternal(GYM_EQUIPMENT_BARBELL, _:barbellid, _:E_GYM_EQUIPMENT_BUTTON_ID);
}

stock Button:GetDumbellButton(Dumbell:dumbellid) {
    return Button:GetGymEquipmentCellInternal(GYM_EQUIPMENT_DUMBELL, _:dumbellid, _:E_GYM_EQUIPMENT_BUTTON_ID);
}

stock Button:GetTreadmillButton(Treadmill:treadmillid) {
    return Button:GetGymEquipmentCellInternal(GYM_EQUIPMENT_TREADMILL, _:treadmillid, _:E_GYM_EQUIPMENT_BUTTON_ID);
}

stock Button:GetBikeButton(Bike:bikeid) {
    return Button:GetGymEquipmentCellInternal(GYM_EQUIPMENT_BIKE, _:bikeid, _:E_GYM_EQUIPMENT_BUTTON_ID);
}

stock bool:GetPlayerGymEquipment(playerid, &E_GYM_EQUIPMENT_TYPE:type, &index, bool:freeOnly = true) {
    type = GYM_EQUIPMENT_NONE;
    index = -1;

    // Which button the player is answering to is worked out already and sitting
    // in an array, so the equipment behind it costs a couple of reads to reach.
    // Standing at one bench, which is nearly always what is happening, that is
    // the whole of this function.
    new const
        Button:buttonid = GetPlayerButton(playerid)
    ;

    if (GetGymButtonEquipmentInternal(buttonid, type, index, freeOnly)) {
        return true;
    }

    // Only when that one cannot be used: somebody is on it, or the button on
    // top is not equipment at all. Then what is underneath is worth asking for,
    // and asking costs the streamer a pass over every area around the player.
    new
        Button:buttons[MAX_AREA_BUTTONS]
    ;

    for (new i, count = GetPlayerNearbyButtons(playerid, buttons); i != count; ++i) {
        if (buttons[i] == buttonid) {
            continue;
        }

        if (GetGymButtonEquipmentInternal(buttons[i], type, index, freeOnly)) {
            return true;
        }
    }

    return false;
}

stock bool:IsPlayerExercising(playerid) {
    return (0 <= playerid < MAX_PLAYERS) && gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_NONE;
}

stock E_GYM_EQUIPMENT_TYPE:GetPlayerExerciseType(playerid) {
    return IsPlayerExercising(playerid) ? gGymPlayerData[playerid][E_GYM_PLAYER_TYPE] : GYM_EQUIPMENT_NONE;
}

stock GetPlayerExerciseIndex(playerid) {
    return IsPlayerExercising(playerid) ? gGymPlayerData[playerid][E_GYM_PLAYER_INDEX] : -1;
}

stock E_GYM_EXERCISE_STATE:GetPlayerExerciseState(playerid) {
    return (0 <= playerid < MAX_PLAYERS) ? gGymPlayerData[playerid][E_GYM_PLAYER_STATE] : GYM_EXERCISE_NONE;
}

stock Float:GetPlayerExercisePower(playerid) {
    return IsPlayerExercising(playerid) ? gGymPlayerData[playerid][E_GYM_PLAYER_POWER] : 0.0;
}

stock bool:SetPlayerExercisePower(playerid, Float:power) {
    if (!IsPlayerExercising(playerid)) {
        return false;
    }

    gGymPlayerData[playerid][E_GYM_PLAYER_POWER] = GetGymClampedPowerInternal(power);

    return true;
}

stock GetPlayerExerciseMetric(playerid) {
    return IsPlayerExercising(playerid) ? gGymPlayerData[playerid][E_GYM_PLAYER_METRIC] : 0;
}

stock bool:SetPlayerExerciseMetric(playerid, metric) {
    if (!IsPlayerExercising(playerid)) {
        return false;
    }

    gGymPlayerData[playerid][E_GYM_PLAYER_METRIC] = metric;

    return true;
}

stock Float:GetPlayerExerciseCameraAngle(playerid) {
    return IsPlayerExercising(playerid) ? gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_ANGLE] : 0.0;
}

stock bool:IsPlayerExerciseCameraMoving(playerid) {
    if (!IsPlayerExercising(playerid)) {
        return false;
    }

    return (GetTickCount() - gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_MOVE_END]) < 0;
}

stock bool:RotatePlayerExerciseCamera(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_ACTIVE) {
        return false;
    }

    if (IsPlayerExerciseCameraMoving(playerid)) {
        return false;
    }

    if (!gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_MOVED]) {
        return false;
    }

    new const
        E_GYM_EQUIPMENT_TYPE:type = gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
        index = gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]
    ;

    if (!IsValidGymEquipmentInternal(type, index)) {
        return false;
    }

    new const
        Float:stepAngle = gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_CAMERA_STEP_ANGLE],
        Float:current = gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_ANGLE],
        Float:next = GetGymNextCameraAngleInternal(current, gGymEquipmentData[_:type][index][E_GYM_EQUIPMENT_CAMERA_STEPS], stepAngle),
        Float:delta = (next > current) ? (next - current) : ((next < current) ? stepAngle : 0.0)
    ;

    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_ANGLE] = next;

    return ChangeGymCameraAngleInternal(playerid, gGymEquipmentData[_:type][index], current, delta);
}

stock bool:SetPlayerExerciseCameraAngle(playerid, Float:angle) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_ACTIVE) {
        return false;
    }

    if (IsPlayerExerciseCameraMoving(playerid)) {
        return false;
    }

    if (!gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_MOVED]) {
        return false;
    }

    new const
        E_GYM_EQUIPMENT_TYPE:type = gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
        index = gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]
    ;

    if (!IsValidGymEquipmentInternal(type, index)) {
        return false;
    }

    new const
        Float:wrapped = GetGymWrappedAngleInternal(angle),
        Float:current = gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_ANGLE]
    ;

    gGymPlayerData[playerid][E_GYM_PLAYER_CAMERA_ANGLE] = wrapped;

    return ChangeGymCameraAngleInternal(playerid, gGymEquipmentData[_:type][index], current, GetGymShortestDeltaInternal(current, wrapped));
}

stock bool:StopPlayerExercise(playerid, bool:animate = true) {
    if (!IsPlayerExercising(playerid)) {
        return false;
    }

    if (animate && gGymPlayerData[playerid][E_GYM_PLAYER_STATE] == GYM_EXERCISE_ACTIVE) {
        return StopGymExerciseInternal(playerid);
    }

    CreateGymEquipmentPropsInternal(
        gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
        gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]
    );

    RefreshGymObjectsInternal(playerid);
    FinishGymExerciseInternal(playerid);

    return true;
}

// What the key does depends on where the player is: on a piece of equipment it
// gets them off it, and standing next to one it gets them on. Both paths into
// the library end up here so that neither can drift from the other.
static stock bool:UseGymEquipmentInternal(playerid) {
    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] == GYM_EXERCISE_ACTIVE) {
        if (!IsGymDismountAllowedInternal(playerid)) {
            return false;
        }

        StopGymExerciseInternal(playerid);

        return true;
    }

    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_NONE) {
        return false;
    }

    new
        E_GYM_EQUIPMENT_TYPE:type,
        index
    ;

    if (!GetPlayerGymEquipment(playerid, type, index)) {
        return false;
    }

    return StartGymExerciseInternal(playerid, type, index);
}

/**
 * # Calls
 */

hook OnGameModeInit() {
    return 0;
}

hook OnPlayerConnect(playerid) {
    ResetGymPlayerDataInternal(playerid);

    // Whoever keeps the strength of the player feeds it in; until then every
    // weight is within reach, which is how the library behaved before weights.
    gGymPlayerData[playerid][E_GYM_PLAYER_STRENGTH]     = GYM_POWER_MAX;

    gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE]      = 0.0;
    gGymPlayerData[playerid][E_GYM_PLAYER_FATIGUE_TIME] = gettime();

    // Left out of the reset between sets on purpose: a rest earned by failing
    // is not undone by the player standing up.
    gGymPlayerData[playerid][E_GYM_PLAYER_REST_UNTIL]   = 0;
    gGymPlayerData[playerid][E_GYM_PLAYER_EXHAUSTION]   = 0.0;
    gGymPlayerData[playerid][E_GYM_PLAYER_RECOVERY]     = 1.0;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    new const
        E_GYM_EQUIPMENT_TYPE:type = gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
        index = gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]
    ;

    if (type != GYM_EQUIPMENT_NONE) {
        CreateGymEquipmentPropsInternal(type, index);
        SetGymEquipmentCellInternal(type, index, _:E_GYM_EQUIPMENT_PLAYER_ID, INVALID_PLAYER_ID);
    }

    ResetGymPlayerDataInternal(playerid);

    return 0;
}

hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    // Walking around changes keys all the time and none of it concerns the gym,
    // so the state of the player is only asked for once a key it watches moves.
    if (!((newkeys ^ oldkeys) & (KEY_SPRINT | KEY_SECONDARY_ATTACK | GYM_CAMERA_KEY))) {
        return 0;
    }

    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) {
        return 0;
    }

    if (((newkeys & KEY_SPRINT) == KEY_SPRINT) && ((oldkeys & KEY_SPRINT) != KEY_SPRINT)) {
        if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] == GYM_EXERCISE_ACTIVE && IsGymPowerInputAllowedInternal(playerid) && !IsGymRestingInternal(playerid, gGymPlayerData[playerid][E_GYM_PLAYER_TYPE])) {
            gGymPlayerData[playerid][E_GYM_PLAYER_LAST_INPUT] = GetTickCount();

            gGymPlayerData[playerid][E_GYM_PLAYER_POWER] = GetGymClampedPowerInternal(
                gGymPlayerData[playerid][E_GYM_PLAYER_POWER] + GYM_POWER_GAIN * GetGymEffortFactorInternal(
                    playerid,
                    gGymPlayerData[playerid][E_GYM_PLAYER_TYPE],
                    gGymPlayerData[playerid][E_GYM_PLAYER_INDEX]
                )
            );
        }
    }

    if (((newkeys & GYM_CAMERA_KEY) == GYM_CAMERA_KEY) && ((oldkeys & GYM_CAMERA_KEY) != GYM_CAMERA_KEY)) {
        #if GYM_CAMERA_MODE == GYM_CAMERA_MODE_ORBIT
            StartGymCameraHoldInternal(playerid);
        #else
            RotatePlayerExerciseCamera(playerid);
        #endif
    }

    #if GYM_CAMERA_MODE == GYM_CAMERA_MODE_ORBIT
        if (((oldkeys & GYM_CAMERA_KEY) == GYM_CAMERA_KEY) && ((newkeys & GYM_CAMERA_KEY) != GYM_CAMERA_KEY)) {
            KillGymCameraTimerInternal(playerid);
        }
    #endif

    // Getting off is answered here rather than through the button, because
    // being on a piece of equipment is not the same as standing in the area
    // that got the player onto it. A treadmill places them a metre up and a
    // metre and a half back, which is outside its own sphere, and asking the
    // button first meant the press never arrived.
    if (((newkeys & GYM_ENTER_KEY) == GYM_ENTER_KEY) && ((oldkeys & GYM_ENTER_KEY) != GYM_ENTER_KEY)) {
        if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_NONE) {
            UseGymEquipmentInternal(playerid);
        }
    }

    return 0;
}

// Getting on arrives here, already knowing which button outranked the others
// around it. Getting off does not: see the key hook above.
hook OnButtonPress(playerid, Button:buttonid) {
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) {
        return 0;
    }

    // Somebody already on a piece of equipment is the key hook's business, and
    // answering here as well would take them off it twice.
    if (gGymPlayerData[playerid][E_GYM_PLAYER_STATE] != GYM_EXERCISE_NONE) {
        return 0;
    }

    if (!gGymButtonData[buttonid][E_GYM_BUTTON_OWNED]) {
        return 0;
    }

    return UseGymEquipmentInternal(playerid) ? 1 : 0;
}

// A button taken down by hand rather than by DestroyGymEquipment would otherwise
// leave its slot pointing at equipment that is still there.
hook OnButtonDestroy(Button:buttonid) {
    gGymButtonData[buttonid][E_GYM_BUTTON_OWNED] = false;
    gGymButtonData[buttonid][E_GYM_BUTTON_TYPE] = GYM_EQUIPMENT_NONE;
    gGymButtonData[buttonid][E_GYM_BUTTON_INDEX] = -1;

    return 0;
}
