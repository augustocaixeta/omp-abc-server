#define MAX_PLAYERS (2)
#define MAX_BUTTONS (Button:1024)

#include <open.mp>
#include <physique>
#include <extended-text-styles>

#include <I\button>
#include <I\entrance>

#define GYM_INTERIOR_ID                 (6)

// The doorway of the Binco in Las Venturas, which is what the gym is put behind
// here. Outside is the street, inside is the door of the gym itself.
#define GYM_DOOR_EXTERIOR_X              (2101.9431)
#define GYM_DOOR_EXTERIOR_Y              (2257.4814)
#define GYM_DOOR_EXTERIOR_Z              (11.0234)
#define GYM_DOOR_EXTERIOR_ANGLE          (270.0000)

#define GYM_DOOR_INTERIOR_X               (774.0961)
#define GYM_DOOR_INTERIOR_Y               (-50.4399)
#define GYM_DOOR_INTERIOR_Z               (1000.5859)
#define GYM_DOOR_INTERIOR_ANGLE           (0.0000)

// The arrow on the ground, the way the story mode marks a door worth walking
// into. The entrance itself is what actually answers the key.
#define GYM_DOOR_PICKUP_MODEL           (1318)
#define GYM_DOOR_PICKUP_TYPE            (1)

// How long the box stays on screen, and how long the one that ends the day
// stays: being turned away for a quarter of an hour is worth reading twice.
#define POPUP_TIME                      (6000)
#define POPUP_TIME_LONG                 (10000)

// The speed the lifting animations run at when the weight is comfortable, the
// slowest they are allowed to get while the player strains under it and the
// fastest they go on weights that are nothing to them.
#define REPETITION_SPEED_NORMAL         (4.100000)
#define REPETITION_SPEED_SLOWEST        (1.800000)
#define REPETITION_SPEED_FASTEST        (6.500000)

// The effort the library reports when the weight is exactly everything the
// player has: the last repetition they get up before it stops going up at all.
#define EFFORT_AT_LIMIT                 (1.000000)

// How long the movement itself takes at REPETITION_SPEED_NORMAL. A slower
// animation takes proportionally longer to play, and that part is the movement
// still running, not the character standing there holding the weight.
//
// The straining animation is a longer one than the other two, so it carries its
// own number: the weight comes down when the movement actually playing ends,
// not when the average of the three would.
#define REPETITION_TIME_BARBELL         (1200)
#define REPETITION_TIME_BARBELL_HARD    (1200)
#define REPETITION_TIME_DUMBELL         (1200)
#define REPETITION_TIME_DUMBELL_HARD    (1400)

// The beat the character holds the weight at the top, once the movement is
// over. It does not stretch with the weight: a heavy repetition already takes
// longer to play, and freezing the character on top of that is what looks
// stuck. Without it the curl arrives at the top and drops in the same frame.
#define REPETITION_PAUSE_BARBELL        (0)
#define REPETITION_PAUSE_DUMBELL        (125)

#define HUD_LABEL_X                     (470.000000)
#define HUD_LABEL_Y                     (178.000000)
#define HUD_VALUE_X                     (607.000000)

// A line of the label is the letter height of the font, so anything that sits
// on a row of its own is this far below the row before it.
#define HUD_LINE_HEIGHT                 (18.000000)

// The row the breath bar sits on, counting the one the power bar is on as zero.
#define HUD_BREATH_ROW                  (2.000000)

#define HUD_FRAME_X                     (545.000000)
#define HUD_FRAME_Y                     (183.000000)
#define HUD_FRAME_WIDTH                 (66.000000)
#define HUD_FRAME_HEIGHT                (8.000000)

#define HUD_FILL_X                      (546.000000)
#define HUD_FILL_Y                      (184.000000)
#define HUD_FILL_WIDTH                  (64.000000)
#define HUD_FILL_HEIGHT                 (6.000000)
#define HUD_FILL_COLOUR                 (0xADD8E6FF)
#define HUD_EMPTY_COLOUR                (0xADD8E644)

#define HUD_BREATH_FRAME_Y              (HUD_FRAME_Y + HUD_LINE_HEIGHT * HUD_BREATH_ROW)
#define HUD_BREATH_FILL_Y               (HUD_FILL_Y + HUD_LINE_HEIGHT * HUD_BREATH_ROW)

// What the test character is worth on the day it connects.
#define TEST_STRENGTH                   (100.000000)
#define TEST_STAMINA                    (40.000000)

static const
    Float:gBarbellPositions[][] = {
        {760.00000, -49.00000, 999.57500, 180.00000},
        {758.00000, -49.00000, 999.57500, 180.00000},
        {756.00000, -49.00000, 999.57500, 180.00000}
};

static const
    Float:gDumbellPositions[][] = {
        {776.75000, -41.75500, 999.82500, 90.00000},
        {776.75000, -40.50750, 999.82500, 90.00000},
        {776.75000, -39.26000, 999.82500, 90.00000}
};

static const
    Float:gTreadmillPositions[][] = {
        {777.00000, -48.35000, 999.57500, 270.00000},
        {777.00000, -46.35000, 999.57500, 270.00000}
};

static const
    Float:gBikePositions[][] = {
        {772.00000, -49.00000, 999.57500, 180.00000},
        {770.00000, -49.00000, 999.57500, 180.00000},
        {768.00000, -49.00000, 999.57500, 180.00000},
        {766.00000, -49.00000, 999.57500, 180.00000}
};

// The one door in and out of this gym.
static
    Entrance:gGymEntrance
;

static enum _:E_GYM_HUD_DATA {
    PlayerText:E_GYM_HUD_VALUE,
    PlayerText:E_GYM_HUD_BAR,

    // Breath on the machines, muscle on the weights: what the player has left
    // of whatever the equipment is spending.
    PlayerText:E_GYM_HUD_SECOND
};

// The labels and the frames of the bars are the same for everybody, so they are
// made once and only shown to whoever is exercising. One label reads REPS, the
// other DISTANCE. The second row of frames is the breath bar, which only the
// machines have.
static
    Text:gGymLabel[2] = {
        Text:INVALID_TEXT_DRAW,
        Text:INVALID_TEXT_DRAW
    },
    Text:gGymBarFrame[2] = {
        Text:INVALID_TEXT_DRAW,
        Text:INVALID_TEXT_DRAW
    },
    Text:gGymBarEmpty[2] = {
        Text:INVALID_TEXT_DRAW,
        Text:INVALID_TEXT_DRAW
    }
;

static
    gGymHud[MAX_PLAYERS][E_GYM_HUD_DATA],
    bool:gGymHudVisible[MAX_PLAYERS]
;

// Tuned in game with /hold while watching the character, because how long the
// movement takes is not something that can be read off the animation.
static
    gRepetitionTime[2][2] = {
        {REPETITION_TIME_BARBELL, REPETITION_TIME_BARBELL_HARD},
        {REPETITION_TIME_DUMBELL, REPETITION_TIME_DUMBELL_HARD}
    },
    gRepetitionPause[2] = {REPETITION_PAUSE_BARBELL, REPETITION_PAUSE_DUMBELL}
;

// Which row of the two above a repetition belongs to.
static GetEquipmentSlot(E_GYM_EQUIPMENT_TYPE:type) {
    return (type == GYM_EQUIPMENT_BARBELL) ? 0 : 1;
}

static IsStrainingSlot(Float:effort) {
    return (effort < GYM_EFFORT_HARD) ? 1 : 0;
}

main(){}

/**
 * @hud
 */

static Text:CreateGymLabel(const text[]) {
    new const
        Text:t = TextDrawCreate(HUD_LABEL_X, HUD_LABEL_Y, text)
    ;

    TextDrawLetterSize(t, 0.400000, 1.800000);
    TextDrawAlignment(t, TEXT_DRAW_ALIGN_RIGHT);
    TextDrawColour(t, -1);
    TextDrawSetShadow(t, 1);
    TextDrawSetOutline(t, 2);
    TextDrawBackgroundColour(t, 255);
    TextDrawFont(t, TEXT_DRAW_FONT_2);
    TextDrawSetProportional(t, true);

    return t;
}

// The black frame and the empty bar behind the one that moves are the same
// sprite: only where they sit, how big they are and their colour change.
static Text:CreateGymBar(Float:x, Float:y, Float:width, Float:height, colour) {
    new const
        Text:t = TextDrawCreate(x, y, "LD_DUAL:white")
    ;

    TextDrawTextSize(t, width, height);
    TextDrawAlignment(t, TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(t, colour);
    TextDrawSetShadow(t, 0);
    TextDrawSetOutline(t, 0);
    TextDrawBackgroundColour(t, 255);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(t, true);

    return t;
}

static PlayerText:CreateGymFill(playerid, Float:y) {
    new const
        PlayerText:t = CreatePlayerTextDraw(playerid, HUD_FILL_X, y, "LD_DUAL:white")
    ;

    PlayerTextDrawTextSize(playerid, t, HUD_FILL_WIDTH, HUD_FILL_HEIGHT);
    PlayerTextDrawAlignment(playerid, t, TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, t, HUD_FILL_COLOUR);
    PlayerTextDrawSetShadow(playerid, t, 0);
    PlayerTextDrawSetOutline(playerid, t, 0);
    PlayerTextDrawBackgroundColour(playerid, t, 255);
    PlayerTextDrawFont(playerid, t, TEXT_DRAW_FONT_SPRITE_DRAW);
    PlayerTextDrawSetProportional(playerid, t, true);

    return t;
}

// A sprite textdraw is sized, not stretched to an edge, so the width alone
// carries the value of the bar.
static ShowGymFill(playerid, PlayerText:fill, Float:share) {
    if (share <= 0.0) {
        PlayerTextDrawHide(playerid, fill);

        return;
    }

    PlayerTextDrawTextSize(playerid, fill, HUD_FILL_WIDTH * ((share > 1.0) ? 1.0 : share), HUD_FILL_HEIGHT);
    PlayerTextDrawShow(playerid, fill);
}

static CreateGymHud(playerid, E_GYM_EQUIPMENT_TYPE:type) {
    new const
        bool:lifting = (type == GYM_EQUIPMENT_BARBELL || type == GYM_EQUIPMENT_DUMBELL)
    ;

    TextDrawShowForPlayer(playerid, gGymLabel[lifting ? 0 : 1]);

    new const
        PlayerText:pt = gGymHud[playerid][E_GYM_HUD_VALUE] = CreatePlayerTextDraw(playerid, HUD_VALUE_X, HUD_LABEL_Y, "~n~~b~~h~~h~~h~0")
    ;

    PlayerTextDrawLetterSize(playerid, pt, 0.400000, 1.800000);
    PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, pt, -1);
    PlayerTextDrawSetShadow(playerid, pt, 1);
    PlayerTextDrawSetOutline(playerid, pt, 2);
    PlayerTextDrawBackgroundColour(playerid, pt, 255);
    PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, pt, true);
    PlayerTextDrawShow(playerid, pt);

    TextDrawShowForPlayer(playerid, gGymBarFrame[0]);
    TextDrawShowForPlayer(playerid, gGymBarEmpty[0]);

    gGymHud[playerid][E_GYM_HUD_BAR] = CreateGymFill(playerid, HUD_FILL_Y);
    gGymHud[playerid][E_GYM_HUD_SECOND] = CreateGymFill(playerid, HUD_BREATH_FILL_Y);

    TextDrawShowForPlayer(playerid, gGymBarFrame[1]);
    TextDrawShowForPlayer(playerid, gGymBarEmpty[1]);

    gGymHudVisible[playerid] = true;
}

static UpdateGymHud(playerid, E_GYM_EQUIPMENT_TYPE:type, index, Float:power, metric) {
    if (!gGymHudVisible[playerid]) {
        return;
    }

    new const
        PlayerText:fill = gGymHud[playerid][E_GYM_HUD_BAR]
    ;

    ShowGymFill(playerid, fill, power / 100.0);

    new const
        Float:weight = GetGymEquipmentWeight(type, index)
    ;

    if (weight > 0.0) {
        // The third line is what the muscle has left, which is what says how
        // many more repetitions there are before it gives out, and the fourth
        // is the weight with how hard it is for this player.
        ShowGymFill(playerid, gGymHud[playerid][E_GYM_HUD_SECOND], (GYM_FATIGUE_MAX - GetPlayerGymFatigue(playerid)) / GYM_FATIGUE_MAX);

        PlayerTextDrawSetString(playerid, gGymHud[playerid][E_GYM_HUD_VALUE], "~n~~b~~h~~h~~h~%i~n~~n~~b~~h~~h~~h~%.0fKG %.2fx~n~", metric, weight, GetPlayerExerciseEffort(playerid));
        PlayerTextDrawShow(playerid, gGymHud[playerid][E_GYM_HUD_VALUE]);

        return;
    }

    // On the machines the third line is the breath of the player and the fourth
    // is the speed it is being spent at, in red while it is going too fast to
    // step off.
    ShowGymFill(playerid, gGymHud[playerid][E_GYM_HUD_SECOND], GetPlayerBreath(playerid) / GetPlayerMaxBreath(playerid));

    PlayerTextDrawSetString(playerid, gGymHud[playerid][E_GYM_HUD_VALUE], "~n~~b~~h~~h~~h~%i~n~~n~%s%i~n~",
        metric,
        CanPlayerStopExercise(playerid) ? ("~b~~h~~h~~h~") : ("~r~"),
        _:GetPlayerExerciseSpeed(playerid)
    );

    PlayerTextDrawShow(playerid, gGymHud[playerid][E_GYM_HUD_VALUE]);
}

static DestroyGymHud(playerid) {
    if (!gGymHudVisible[playerid]) {
        return;
    }

    TextDrawHideForPlayer(playerid, gGymLabel[0]);
    TextDrawHideForPlayer(playerid, gGymLabel[1]);

    for (new i = sizeof (gGymBarFrame); --i >= 0;) {
        TextDrawHideForPlayer(playerid, gGymBarFrame[i]);
        TextDrawHideForPlayer(playerid, gGymBarEmpty[i]);
    }

    PlayerTextDrawDestroy(playerid, gGymHud[playerid][E_GYM_HUD_VALUE]);
    PlayerTextDrawDestroy(playerid, gGymHud[playerid][E_GYM_HUD_BAR]);
    PlayerTextDrawDestroy(playerid, gGymHud[playerid][E_GYM_HUD_SECOND]);

    gGymHudVisible[playerid] = false;
}

// The library says how hard the weight is for the player; how that reads on the
// character is for the gamemode to decide. At the limit of the player the bar
// barely moves, it picks up until the weight stops being a strain, and from
// there it gets snappy.
static Float:GetRepetitionSpeed(Float:effort) {
    if (effort <= EFFORT_AT_LIMIT) {
        return REPETITION_SPEED_SLOWEST;
    }

    if (effort < GYM_EFFORT_HARD) {
        return REPETITION_SPEED_SLOWEST + (effort - EFFORT_AT_LIMIT) / (GYM_EFFORT_HARD - EFFORT_AT_LIMIT) * (REPETITION_SPEED_NORMAL - REPETITION_SPEED_SLOWEST);
    }

    if (effort >= GYM_EFFORT_MAX) {
        return REPETITION_SPEED_FASTEST;
    }

    return REPETITION_SPEED_NORMAL + (effort - GYM_EFFORT_HARD) / (GYM_EFFORT_MAX - GYM_EFFORT_HARD) * (REPETITION_SPEED_FASTEST - REPETITION_SPEED_NORMAL);
}

/**
 * @callback
 */

// The library hands every repetition over before playing anything, so this is
// where the weight becomes visible on the character: the straining animation
// while the player is under the weight, and the whole movement slowed down in
// proportion to how far under it they are. Answering 1 means it was handled
// here.
// The movement of the weight, going up or coming down. Which one goes up is the
// same choice the library makes on its own, by effort: forcing any of the three
// on every weight only looks wrong, because the speed of the repetition is what
// the animations disagree about. What the gamemode adds here is that speed.
static PlayGymAnimation(playerid, E_GYM_EQUIPMENT_TYPE:type, Float:effort, Float:speed) {
    if (type == GYM_EQUIPMENT_BARBELL) {
        if (effort >= GYM_EFFORT_SMOOTH) {
            ApplyAnimation(playerid, "BENCHPRESS", GYM_BARBELL_ANIMATION_SMOOTH, speed, false, false, false, true, 0, SYNC_ALL);

            return;
        }

        ApplyAnimation(playerid, "BENCHPRESS", (effort < GYM_EFFORT_HARD)
            ? GYM_BARBELL_ANIMATION_HARD
            : GYM_BARBELL_ANIMATION_EASY, speed, false, false, false, true, 0, SYNC_ALL);

        return;
    }

    if (type != GYM_EQUIPMENT_DUMBELL) {
        return;
    }

    if (effort >= GYM_EFFORT_SMOOTH) {
        ApplyAnimation(playerid, "FREEWEIGHTS", GYM_DUMBELL_ANIMATION_SMOOTH, speed, false, false, false, true, 0, SYNC_ALL);

        return;
    }

    ApplyAnimation(playerid, "FREEWEIGHTS", (effort < GYM_EFFORT_HARD)
        ? GYM_DUMBELL_ANIMATION_HARD
        : GYM_DUMBELL_ANIMATION_EASY, speed, false, false, false, true, 0, SYNC_ALL);
}

public OnPlayerMuscleFailure(playerid, E_GYM_EQUIPMENT_TYPE:type, index, metric) {
    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME_LONG, "Your muscle gave out on repetition %i of %.0f kg.~n~~n~Weights down for %i seconds. %i more sets in you today.",
        metric,
        GetGymEquipmentWeight(type, index),
        GetPlayerGymRestTime(playerid),
        GetPlayerGymSetsLeft(playerid)
    );

    return 1;
}

public OnPlayerGymExhausted(playerid, E_GYM_EQUIPMENT_TYPE:type, index) {
    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME_LONG, "That is everything you had today. Come back in %i minutes.",
        floatround(GYM_FATIGUE_MAX / GYM_EXHAUSTION_RECOVERY / 60.0, floatround_ceil)
    );

    return 1;
}

public OnPlayerRepetition(playerid, E_GYM_EQUIPMENT_TYPE:type, index, Float:effort) {
    new const
        Float:speed = GetRepetitionSpeed(effort)
    ;

    if (type != GYM_EQUIPMENT_BARBELL && type != GYM_EQUIPMENT_DUMBELL) {
        return 0;
    }

    new const
        equipment = GetEquipmentSlot(type)
    ;

    // The weight comes down when the movement finishes, plus the beat at the
    // top: a slower animation takes longer to play and only that part stretches,
    // so a heavy repetition is slow instead of frozen.
    SetPlayerRepetitionTime(playerid, floatround(float(gRepetitionTime[equipment][IsStrainingSlot(effort)]) * REPETITION_SPEED_NORMAL / speed) + gRepetitionPause[equipment]);

    PlayGymAnimation(playerid, type, effort, speed);

    return 1;
}

// Weight past the limit of the player is dropped on every single repetition, so
// there is no point letting them lie down under it in the first place, and the
// same goes for a muscle with no repetition left in it for that weight.
public OnPlayerRequestExercise(playerid, E_GYM_EQUIPMENT_TYPE:type, index) {
    if (!CanPlayerLiftGymEquipment(playerid, type, index)) {
        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "%.0f kg is more than you can get up: it asks for %.1f of strength and you have %.1f.",
            GetGymEquipmentWeight(type, index),
            GetGymEquipmentRequiredStrength(type, index),
            GetPlayerGymStrength(playerid)
        );

        return 0;
    }

    if (!GetPlayerGymSetsLeft(playerid)) {
        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME_LONG, "You are done training for today: %i minutes before there is anything in you again.",
            floatround(GetPlayerGymExhaustion(playerid) / GYM_EXHAUSTION_RECOVERY / 60.0, floatround_ceil)
        );

        return 0;
    }

    // Everything below belongs to the arms and the chest. A treadmill is legs
    // and breath, so a set of dumbbells that went to failure has nothing to say
    // about whether the player can go for a run.
    if (type != GYM_EQUIPMENT_BARBELL && type != GYM_EQUIPMENT_DUMBELL) {
        return 1;
    }

    if (GetPlayerGymRestTime(playerid)) {
        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "The weights are staying down for another %i seconds.", GetPlayerGymRestTime(playerid));

        return 0;
    }

    if (!GetGymRepetitionsLeft(playerid, type, index)) {
        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "There is nothing left in the muscle for that weight: give it %i seconds, or put less on the bar.",
            floatround(GetPlayerGymFatigue(playerid) / GYM_FATIGUE_RECOVERY, floatround_ceil)
        );

        return 0;
    }

    return 1;
}

public OnPlayerFailRepetition(playerid, E_GYM_EQUIPMENT_TYPE:type, index) {
    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Too heavy: you dropped the repetition and need a moment.");

    return 1;
}

// Both of these fire after GetPlayerButton has been brought up to date, so
// asking it in each of them shows the right prompt whichever way round the two
// arrive when the player walks straight from one button into another.
public OnPlayerEnterButtonArea(playerid, Button:buttonid) {
    RefreshButtonPrompt(playerid);

    return 1;
}

public OnPlayerLeaveButtonArea(playerid, Button:buttonid) {
    RefreshButtonPrompt(playerid);

    return 1;
}

static RefreshButtonPrompt(playerid) {
    // Lying on the bench already, a prompt saying which key gets you onto the
    // bench is only in the way.
    if (IsPlayerExercising(playerid)) {
        PlayerGameTextHide(playerid, GAME_TEXT_STYLE_POPUP);

        return;
    }

    new const
        Button:buttonid = GetPlayerButton(playerid)
    ;

    if (buttonid == INVALID_BUTTON_ID) {
        PlayerGameTextHide(playerid, GAME_TEXT_STYLE_POPUP);

        return;
    }

    new
        prompt[MAX_BUTTON_POPUP_LENGTH]
    ;

    GetButtonPopupText(buttonid, prompt);

    // A button with nothing to say says nothing, rather than an empty box.
    if (prompt[0] == EOS) {
        PlayerGameTextHide(playerid, GAME_TEXT_STYLE_POPUP);

        return;
    }

    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, 0, prompt);
}

// Everything about where the player is and what is holding them, in one line,
// printed to the server console. Temporary: it is here to find out what the
// state actually is when the screen locks up, because a screenshot cannot say.
static ReportPlayer(playerid, const when[]) {
    new
        Float:x, Float:y, Float:z, Float:angle
    ;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angle);

    printf("[where] %-22s pos %.4f %.4f %.4f a %.2f | int %d vw %d | state %d anim %d | exercising %d type %d | button %d | held %d",
        when,
        x, y, z, angle,
        GetPlayerInterior(playerid),
        GetPlayerVirtualWorld(playerid),
        GetPlayerState(playerid),
        GetPlayerAnimationIndex(playerid),
        IsPlayerExercising(playerid),
        _:GetPlayerExerciseType(playerid),
        _:GetPlayerButton(playerid),
        _:GetPlayerHeldButton(playerid)
    );
}

public OnPlayerEnteredEntrance(playerid, Entrance:entranceid) {
    ReportPlayer(playerid, "after going in");

    return 1;
}

public OnPlayerLeftEntrance(playerid, Entrance:entranceid) {
    ReportPlayer(playerid, "after going out");

    return 1;
}

// The gym holds the player still, points a camera at them and puts weights in
// their hands. Moving them to another part of the map in the middle of that
// leaves all three behind, aimed at a room they are no longer standing in, and
// the player is left frozen holding a dumbell somewhere else. The door waits.
public OnPlayerLeaveEntrance(playerid, Entrance:entranceid) {
    ReportPlayer(playerid, "asked to go out");

    if (!IsPlayerExercising(playerid)) {
        return 0;
    }

    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Get off the equipment before leaving");

    return 1;
}

public OnPlayerEnterEntrance(playerid, Entrance:entranceid) {
    ReportPlayer(playerid, "asked to go in");

    if (!IsPlayerExercising(playerid)) {
        return 0;
    }

    return 1;
}

public OnPlayerStartExercise(playerid, E_GYM_EQUIPMENT_TYPE:type, index) {
    CreateGymHud(playerid, type);

    RefreshButtonPrompt(playerid);

    return 1;
}

public OnPlayerUpdateExercise(playerid, E_GYM_EQUIPMENT_TYPE:type, index, Float:power, metric) {
    UpdateGymHud(playerid, type, index, power, metric);

    return 1;
}

public OnPlayerFinishExercise(playerid, E_GYM_EQUIPMENT_TYPE:type, index, metric) {
    DestroyGymHud(playerid);

    // Back on their feet next to the equipment, so the prompt comes back.
    RefreshButtonPrompt(playerid);

    return 1;
}

public OnGameModeInit() {
    DisableInteriorEnterExits();

    // Straight out of the textdraw editor: the labels of the story mode gym.
    gGymLabel[0] = CreateGymLabel("~b~~h~~h~~h~POWER~n~~b~~h~~h~~h~REPS~n~~b~~h~~h~~h~MUSCLE~n~~b~~h~~h~~h~WEIGHT~n~");
    gGymLabel[1] = CreateGymLabel("~b~~h~~h~~h~POWER~n~~b~~h~~h~~h~DISTANCE~n~~b~~h~~h~~h~BREATH~n~~b~~h~~h~~h~SPEED~n~");

    // The empty bar sits between the black frame and the one that moves, the
    // way the story mode shows what is still missing.
    gGymBarFrame[0] = CreateGymBar(HUD_FRAME_X, HUD_FRAME_Y, HUD_FRAME_WIDTH, HUD_FRAME_HEIGHT, 255);
    gGymBarEmpty[0] = CreateGymBar(HUD_FILL_X, HUD_FILL_Y, HUD_FILL_WIDTH, HUD_FILL_HEIGHT, HUD_EMPTY_COLOUR);

    gGymBarFrame[1] = CreateGymBar(HUD_FRAME_X, HUD_BREATH_FRAME_Y, HUD_FRAME_WIDTH, HUD_FRAME_HEIGHT, 255);
    gGymBarEmpty[1] = CreateGymBar(HUD_FILL_X, HUD_BREATH_FILL_Y, HUD_FILL_WIDTH, HUD_FILL_HEIGHT, HUD_EMPTY_COLOUR);

    CreateObject(2631, 777.26904, -40.51423, 999.57500, 0.00000, 0.00000, 90.00000);

    // A warm up bar, a working set and one that only somebody at the very top
    // of the strength scale gets up, and then not every time.
    new const
        Float:barbellWeights[] = {40.0, 100.0, 140.0}
    ;

    for (new i = sizeof (gBarbellPositions); --i >= 0;) {
        new const Barbell:barbellid = CreateBarbell(
            gBarbellPositions[i][0], gBarbellPositions[i][1], gBarbellPositions[i][2], gBarbellPositions[i][3],
            .interiorid = GYM_INTERIOR_ID
        );

        SetBarbellWeight(barbellid, barbellWeights[i]);
    }

    // A dumbell is what one hand carries on its own, which is why the numbers
    // are nothing like the ones on the bar.
    new const
        Float:dumbellWeights[] = {10.0, 22.0, 40.0}
    ;

    for (new i = sizeof (gDumbellPositions); --i >= 0;) {
        new const Dumbell:dumbellid = CreateDumbell(
            gDumbellPositions[i][0], gDumbellPositions[i][1], gDumbellPositions[i][2], gDumbellPositions[i][3],
            .interiorid = GYM_INTERIOR_ID
        );

        SetDumbellWeight(dumbellid, dumbellWeights[i]);
    }

    for (new i = sizeof (gTreadmillPositions); --i >= 0;) {
        CreateTreadmill(
            gTreadmillPositions[i][0], gTreadmillPositions[i][1], gTreadmillPositions[i][2], gTreadmillPositions[i][3],
            .interiorid = GYM_INTERIOR_ID
        );
    }

    for (new i = sizeof (gBikePositions); --i >= 0;) {
        new const Bike:bikeid = CreateBike(
            gBikePositions[i][0], gBikePositions[i][1], gBikePositions[i][2], gBikePositions[i][3],
            .interiorid = GYM_INTERIOR_ID
        );

        // The last one is out of service: it stays visible and keeps its area,
        // it just refuses whoever presses the key.
        if (i == 0) {
            SetBikeEnabled(bikeid, false);
        }
    }

    // The way in and the way out, one entrance holding both. The prompt on each
    // side comes with it, and the two pickups are only there to be seen from a
    // distance: walking onto one does nothing on its own.
    gGymEntrance = CreateEntrance(
        GYM_DOOR_EXTERIOR_X, GYM_DOOR_EXTERIOR_Y, GYM_DOOR_EXTERIOR_Z, GYM_DOOR_EXTERIOR_ANGLE, 0, 0,
        GYM_DOOR_INTERIOR_X, GYM_DOOR_INTERIOR_Y, GYM_DOOR_INTERIOR_Z, GYM_DOOR_INTERIOR_ANGLE, 0, GYM_INTERIOR_ID
    );

    CreateDynamicPickup(GYM_DOOR_PICKUP_MODEL, GYM_DOOR_PICKUP_TYPE, GYM_DOOR_EXTERIOR_X, GYM_DOOR_EXTERIOR_Y, GYM_DOOR_EXTERIOR_Z, 0, 0);
    CreateDynamicPickup(GYM_DOOR_PICKUP_MODEL, GYM_DOOR_PICKUP_TYPE, GYM_DOOR_INTERIOR_X, GYM_DOOR_INTERIOR_Y, GYM_DOOR_INTERIOR_Z, 0, GYM_INTERIOR_ID);

    ReportButtons();

    return 1;
}

// Every button in the world, and how close each one is to the door of the gym.
// Two buttons close enough to share a sphere means one key press can reach the
// wrong one, which is worth knowing before blaming anything else.
static ReportButtons() {
    printf("[buttons] %d in the world, gym door inside is button %d, outside is %d",
        GetButtonCount(), _:GetEntranceInteriorButton(gGymEntrance), _:GetEntranceExteriorButton(gGymEntrance));

    new
        Float:doorX, Float:doorY, Float:doorZ
    ;

    GetButtonPos(GetEntranceInteriorButton(gGymEntrance), doorX, doorY, doorZ);

    for (new Button:buttonid = Button:0; _:buttonid != GetButtonCount(); ++buttonid) {
        if (!IsValidButton(buttonid)) {
            continue;
        }

        new
            Float:x, Float:y, Float:z, text[MAX_BUTTON_POPUP_LENGTH]
        ;

        GetButtonPos(buttonid, x, y, z);
        GetButtonPopupText(buttonid, text);

        new const
            Float:size = GetButtonSize(buttonid),
            Float:apart = VectorSize(x - doorX, y - doorY, z - doorZ)
        ;

        printf("  button %2d  pos %8.2f %8.2f %8.2f  size %.2f  priority %d  %6.2f from the inside door%s  | %s",
            _:buttonid, x, y, z, size, GetButtonPriority(buttonid), apart,
            (apart < size + GetButtonSize(GetEntranceInteriorButton(gGymEntrance))) ? (" <- SPHERES TOUCH") : (""),
            text
        );
    }
}

public OnPlayerRequestClass(playerid, classid) {
    SetSpawnInfo(playerid, NO_TEAM, 60, 774.07500, -49.50000, 1000.62250, 0.00000);
    SetPlayerFacingAngle(playerid, 0.0000);
    SetPlayerInterior(playerid, GYM_INTERIOR_ID);
    SetCameraBehindPlayer(playerid);
    SpawnPlayer(playerid);
    
    return 1;
}

public OnPlayerSpawn(playerid) {
    SetPlayerVirtualWorld(playerid, 0);
    SetPlayerInterior(playerid, GYM_INTERIOR_ID);
    SetPlayerPos(playerid, 774.07500, -49.50000, 1000.62250);
    SetPlayerFacingAngle(playerid, 0.0000);
    SetCameraBehindPlayer(playerid);

    ReportPlayer(playerid, "after spawning");

    return 1;
}

public OnPlayerConnect(playerid) {
    gGymHudVisible[playerid] = false;

    // Fitness starts at nothing, and a character with no strength cannot get
    // under any of the bars: this is the trained body the example is about.
    SetPlayerStrength(playerid, TEST_STRENGTH);
    SetPlayerStamina(playerid, TEST_STAMINA);

    RemoveBuildingForPlayer(playerid, 2627, 759.6328, -48.1250, 999.6719, 0.25);
    RemoveBuildingForPlayer(playerid, 2629, 766.3047, -48.3047, 999.6719, 0.25);
    RemoveBuildingForPlayer(playerid, 2631, 756.4063, -47.9219, 999.7266, 0.25);
    RemoveBuildingForPlayer(playerid, 2630, 769.2422, -47.8984, 999.6797, 0.25);

    return 1;
}

public OnPlayerDisconnect(playerid, reason) {
    gGymHudVisible[playerid] = false;

    return 1;
}

public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ) {
    SetPlayerPosFindZ(playerid, fX, fY, fZ);

    return 1;
}
/**
 * @test
 */

// The equipment being used, or the one the player is standing in front of.
static bool:GetTestEquipment(playerid, &E_GYM_EQUIPMENT_TYPE:type, &index) {
    if (IsPlayerExercising(playerid)) {
        type = GetPlayerExerciseType(playerid);
        index = GetPlayerExerciseIndex(playerid);

        return true;
    }

    return GetPlayerGymEquipment(playerid, type, index, false);
}

public OnPlayerCommandText(playerid, cmdtext[]) {
    new
        E_GYM_EQUIPMENT_TYPE:type,
        index
    ;

    // Strength belongs to the fitness library, which hands it to the gym on its
    // own: setting it here would last until the next exercise started.
    if (!strcmp(cmdtext, "/strength", true, 9)) {
        SetPlayerStrength(playerid, float(strval(cmdtext[9])));

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Strength set to %.1f.", GetPlayerStrength(playerid));

        return 1;
    }

    if (!strcmp(cmdtext, "/stamina", true, 8)) {
        SetPlayerStamina(playerid, float(strval(cmdtext[8])));

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Stamina set to %.1f, which is %.1f seconds of breath.",
            GetPlayerStamina(playerid),
            GetPlayerMaxBreath(playerid)
        );

        return 1;
    }

    if (!strcmp(cmdtext, "/weight", true, 7)) {
        if (!GetTestEquipment(playerid, type, index)) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Stand in front of an equipment first.");

            return 1;
        }

        SetGymEquipmentWeight(type, index, float(strval(cmdtext[7])));

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Weight set to %.0f kg, which asks for %.1f of strength (you have %.1f).",
            GetGymEquipmentWeight(type, index),
            GetGymEquipmentRequiredStrength(type, index),
            GetPlayerGymStrength(playerid));

        return 1;
    }

    // How long the movement takes at the normal speed, which is what decides
    // when the weight starts coming back down.
    if (!strcmp(cmdtext, "/hold", true, 5)) {
        if (!GetTestEquipment(playerid, type, index)) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Stand in front of an equipment first.");

            return 1;
        }

        if (type != GYM_EQUIPMENT_BARBELL && type != GYM_EQUIPMENT_DUMBELL) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Only the equipment with a weight on it lifts anything.");

            return 1;
        }

        // The weight on the equipment is what decides which of the animations
        // is going to play, and each of them is its own length.
        new const
            Float:required = GetGymEquipmentRequiredStrength(type, index),
            Float:effort = (required > 0.000000) ? (GetPlayerGymStrength(playerid) / required) : GYM_EFFORT_MAX,
            straining = IsStrainingSlot(effort),
            equipment = GetEquipmentSlot(type)
        ;

        gRepetitionTime[equipment][straining] = strval(cmdtext[5]);

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "The %s movement now takes %i ms at %.2f of speed, %i ms at the slowest.",
            straining ? ("straining") : ("ordinary"),
            gRepetitionTime[equipment][straining],
            REPETITION_SPEED_NORMAL,
            floatround(float(gRepetitionTime[equipment][straining]) * REPETITION_SPEED_NORMAL / REPETITION_SPEED_SLOWEST)
        );

        return 1;
    }

    // Whatever a gamemode wants resting to depend on goes through here: how
    // used to a gym the character is, what they ate, what they were sold.
    if (!strcmp(cmdtext, "/recovery", true, 9)) {
        SetPlayerGymRecovery(playerid, floatstr(cmdtext[9]));

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Resting at %.2fx: the muscle comes back at %.1f a second, and only away from the weights.",
            GetPlayerGymRecovery(playerid),
            GYM_FATIGUE_RECOVERY * GetPlayerGymRecovery(playerid)
        );

        return 1;
    }

    // The beat at the top, which is the same however heavy the weight is.
    if (!strcmp(cmdtext, "/pause", true, 6)) {
        if (!GetTestEquipment(playerid, type, index)) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Stand in front of an equipment first.");

            return 1;
        }

        if (type != GYM_EQUIPMENT_BARBELL && type != GYM_EQUIPMENT_DUMBELL) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Only the equipment with a weight on it lifts anything.");

            return 1;
        }

        new const
            equipment = GetEquipmentSlot(type)
        ;

        gRepetitionPause[equipment] = strval(cmdtext[6]);

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "The weight is now held at the top for %i ms before it comes down.", gRepetitionPause[equipment]);

        return 1;
    }

    // Every animation of the game has a number, and this plays the one asked
    // for wherever the player is standing: /anim 52 is the smooth bench press.
    if (!strcmp(cmdtext, "/anim", true, 5)) {
        new
            library[32],
            animation[32]
        ;

        new const
            animationIndex = strval(cmdtext[5])
        ;

        GetAnimationName(animationIndex, library, sizeof (library), animation, sizeof (animation));

        if (!library[0]) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "There is no animation %i.", animationIndex);

            return 1;
        }

        // The library has to reach the client before one of its animations can
        // play, and applying it once with no name is what sends it.
        ApplyAnimation(playerid, library, "null", 0.000000, false, false, false, false, 0);
        ApplyAnimation(playerid, library, animation, REPETITION_SPEED_NORMAL, false, false, false, true, 0, SYNC_ALL);

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "%i is %s / %s.", animationIndex, library, animation);

        return 1;
    }

    if (!strcmp(cmdtext, "/broken", true)) {
        if (!GetTestEquipment(playerid, type, index)) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Stand in front of an equipment first.");

            return 1;
        }

        SetGymEquipmentEnabled(type, index, !IsGymEquipmentEnabled(type, index));

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Equipment is now %s.", IsGymEquipmentEnabled(type, index) ? ("in service") : ("out of service"));

        return 1;
    }

    // Puts the player on the street outside, without going through the door, so
    // the way in can be tried on its own. If going in works and going out does
    // not, the two directions are not the same problem.
    if (!strcmp(cmdtext, "/outside", true)) {
        StopPlayerExercise(playerid, false);

        TogglePlayerControllable(playerid, true);
        RemovePlayerAttachedObject(playerid, 5);
        RemovePlayerAttachedObject(playerid, 6);
        ClearAnimations(playerid);

        SetPlayerVirtualWorld(playerid, 0);
        SetPlayerInterior(playerid, 0);
        SetPlayerPos(playerid, GYM_DOOR_EXTERIOR_X + 2.0, GYM_DOOR_EXTERIOR_Y, GYM_DOOR_EXTERIOR_Z);
        SetPlayerFacingAngle(playerid, GYM_DOOR_EXTERIOR_ANGLE);
        SetCameraBehindPlayer(playerid);

        ReportPlayer(playerid, "put on the street by hand");

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "On the street. Walk into the door and press the key.");

        return 1;
    }

    if (!strcmp(cmdtext, "/where", true)) {
        ReportPlayer(playerid, "asked by hand");

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Written to the server console.");

        return 1;
    }

    // Undoes everything the gym does to a player, in case one of them is left
    // hanging: the freeze, the camera and whatever is in their hands.
    if (!strcmp(cmdtext, "/unstick", true)) {
        ReportPlayer(playerid, "before unsticking");

        StopPlayerExercise(playerid, false);

        TogglePlayerControllable(playerid, true);
        SetCameraBehindPlayer(playerid);
        RemovePlayerAttachedObject(playerid, 5);
        RemovePlayerAttachedObject(playerid, 6);
        ClearAnimations(playerid);

        SetPlayerInterior(playerid, GYM_INTERIOR_ID);
        SetPlayerVirtualWorld(playerid, 0);
        SetPlayerPos(playerid, 774.07500, -49.50000, 1000.62250);

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Put back in the gym.");

        return 1;
    }

    if (!strcmp(cmdtext, "/gym", true)) {
        if (!GetTestEquipment(playerid, type, index)) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, POPUP_TIME, "Stand in front of an equipment first.");

            return 1;
        }

        // The only thing left in the chat: three lines of numbers to compare are
        // read in the scrollback, not in a box that takes itself down.
        SendClientMessage(playerid, 0xFFFFFFFF, "* Weight %.0f kg | asks for %.1f | you have %.1f | effort %.2fx | in service %i",
            GetGymEquipmentWeight(type, index),
            GetGymEquipmentRequiredStrength(type, index),
            GetPlayerGymStrength(playerid),
            GetPlayerExerciseEffort(playerid),
            IsGymEquipmentEnabled(type, index));

        SendClientMessage(playerid, 0xFFFFFFFF, "* Muscle %.0f%% spent | %i repetitions left on this one",
            GetPlayerGymFatigue(playerid),
            GetGymRepetitionsLeft(playerid, type, index));

        SendClientMessage(playerid, 0xFFFFFFFF, "* Day %.0f%% spent | %i sets left | resting at %.2fx | weights down for %i more seconds",
            GetPlayerGymExhaustion(playerid),
            GetPlayerGymSetsLeft(playerid),
            GetPlayerGymRecovery(playerid),
            GetPlayerGymRestTime(playerid));

        return 1;
    }

    return 0;
}
