#if defined _INC_RACE
    #endinput
#endif
#define _INC_RACE

// pp-hooks is the only thing this library takes from outside: everything else
// it needs is a native. Ids come from free lists in plain arrays, the racers of
// a race sit packed in a row of their own, and the countdown is a SetTimerEx
// carrying the number of the run it belongs to.
#include <pp-hooks>

/**
 * # Header
 */

#if !defined MAX_RACES
    #define MAX_RACES (Race:16)
#endif

#if !defined MAX_RACE_RECORDS
    #define MAX_RACE_RECORDS (RaceRecord:64)
#endif

#if !defined MAX_RACE_RECORD_NAME
    #define MAX_RACE_RECORD_NAME (24)
#endif

#if !defined MAX_RACE_NAME
    #define MAX_RACE_NAME (24)
#endif

#if !defined MIN_RACE_NAME
    #define MIN_RACE_NAME (3)
#endif

#if !defined MIN_RACERS
    #define MIN_RACERS (1)
#endif

#if !defined MAX_RACERS
    #define MAX_RACERS (32)
#endif

#if !defined MIN_RACE_LAPS
    #define MIN_RACE_LAPS (1)
#endif

#if !defined MAX_RACE_LAPS
    #define MAX_RACE_LAPS (5)
#endif

#if !defined MIN_RACE_COUNT_DOWN
    #define MIN_RACE_COUNT_DOWN (3)
#endif

#if !defined MAX_RACE_COUNT_DOWN
    #define MAX_RACE_COUNT_DOWN (30)
#endif

#if !defined MIN_RACE_CHECKPOINTS
    #define MIN_RACE_CHECKPOINTS (8)
#endif

#if !defined MAX_RACE_CHECKPOINTS
    #define MAX_RACE_CHECKPOINTS (64)
#endif

// How wide a checkpoint is, and which part the nitro is. Both were written into
// the middle of the code before, where a script could not reach them.
#if !defined RACE_CHECKPOINT_SIZE
    #define RACE_CHECKPOINT_SIZE (5.0)
#endif

#if !defined RACE_NITRO_COMPONENT
    #define RACE_NITRO_COMPONENT (1010)
#endif

// How long a racer has to get back into their car after stepping out of it,
// in milliseconds. Zero turns the whole thing off and a racer may walk for as
// long as they like.
//
// The clock only runs while the race is going and only for somebody who has
// not finished it, and it stops when they are back in the driving seat of the
// car the race gave them. Any other car is not their car.
#if !defined RACE_RETURN_TIME
    #define RACE_RETURN_TIME (10000)
#endif

// What a race paints its cars, when it has not been told a colour. It is what
// CreateVehicle already reads as "pick one", so a race left alone still turns
// out the mixed grid it always did.
#define RACE_COLOUR_RANDOM              (-1)

#define MIN_RACE_COLOUR                 (0)
#define MAX_RACE_COLOUR                 (255)

#define INVALID_RACE_RECORD_ID          (RaceRecord:-1)
#define INVALID_RACE_ID                 (Race:-1)

// What y_iterate's foreach was for, without y_iterate:
//
//     ForEachRacer (playerid : raceid) {
//         GameTextForPlayer(playerid, "~w~- %i -", 1000, 3, countdown);
//     }
//
// It walks only the players in that race, in the order they joined, and it
// declares playerid itself the way foreach did.
//
// It reads the race as it goes, so do not take a racer out of the race inside
// one: the walk is standing on the row that removing them rearranges. Reach for
// GetRaceRacers when the body can do that, which hands back a copy that nothing
// can move underneath it, or count downwards with GetRaceRacer.
#define ForEachRacer(%0:%1) \
    for (new %0 = GetRaceFirstRacer(%1); %0 != INVALID_PLAYER_ID; %0 = GetNextRacer(%0))

#define IsRacePrivated(%0)              HasRaceFlags(%0, RACE_FLAGS_PRIVATED)
#define IsRaceArial(%0)                 HasRaceFlags(%0, RACE_FLAGS_ARIAL)
#define IsRaceGhostVehicleEnabled(%0)   HasRaceFlags(%0, RACE_FLAGS_GHOST)
#define IsRaceNitroEnabled(%0)          HasRaceFlags(%0, RACE_FLAGS_NITRO)
#define IsRaceStarted(%0)               HasRaceFlags(%0, RACE_FLAGS_STARTED)

enum (<<= 1) {
    RACE_FLAGS_NONE,
    RACE_FLAGS_PRIVATED = 1,
    RACE_FLAGS_ARIAL,
    RACE_FLAGS_GHOST,
    RACE_FLAGS_NITRO,
    RACE_FLAGS_STARTED
};

// RACE_FLAGS_STARTED says whether the countdown has run out, which is the
// library's answer to give and not the script's. ToggleRaceFlags takes it out
// of whatever it is handed so that a script flipping every switch on a menu
// cannot tell a standing race that it never started.
#define RACE_FLAGS_INTERNAL             (RACE_FLAGS_STARTED)

// Why a player was not put into a race. PutPlayerInRace used to answer this
// with the numbers one to four and no names for them, so a script could only
// say that joining failed and never which of the four it was.
enum RACE_JOIN: (+= 1) {
    RACE_JOIN_OK,
    RACE_JOIN_INVALID_PLAYER,
    RACE_JOIN_INVALID_RACE,
    RACE_JOIN_ALREADY_IN_RACE,
    RACE_JOIN_STARTED,
    RACE_JOIN_FULL,
    RACE_JOIN_NO_RECORD
};

static enum _:E_RACE_RECORD_DATA {
    bool:E_RACE_RECORD_CREATED,

    E_RACE_RECORD_NAME[(MAX_RACE_RECORD_NAME + 1) char],
    E_RACE_RECORD_START_COUNT,
    E_RACE_RECORD_CP_COUNT
};

static enum _:E_RACE_DATA {
    bool:E_RACE_CREATED,

    RaceRecord:E_RACE_RECORD_ID,
    E_RACE_CREATOR_ID,
    E_RACE_MODEL_ID,
    E_RACE_COLOUR_1,
    E_RACE_COLOUR_2,
    E_RACE_NAME[(MAX_RACE_NAME + 1) char],
    E_RACE_LAPS,
    E_RACE_MAX_RACERS,
    E_RACE_COUNT_DOWN,
    E_RACE_WORLD_ID,
    E_RACE_INTERIOR_ID,
    E_RACE_FLAGS,

    // What the countdown has left to say, which is not the same number as the
    // countdown the race was set up with. They shared one cell before, so a
    // race that had started answered "zero seconds" when asked how long its
    // countdown is, and starting it a second time went straight to GO.
    E_RACE_COUNT_DOWN_LEFT,
    E_RACE_TIMER_ID,
    E_RACE_FINISH_COUNT
};

static enum _:E_RACER_DATA {
    E_RACER_CP_COUNT,
    E_RACER_LAP_COUNT,
    E_RACER_START_INDEX,
    E_RACER_POSITION,
    bool:E_RACER_FINISHED,

    // The tick the return clock runs out on, so how long is left is a
    // subtraction rather than something the library has to keep telling
    // anybody. Zero when they are in the car where they belong.
    E_RACER_RETURN_DEADLINE,

    Float:E_RACER_LAST_X,
    Float:E_RACER_LAST_Y,
    Float:E_RACER_LAST_Z,
    Float:E_RACER_LAST_A,
    E_RACER_LAST_WORLD_ID,
    E_RACER_LAST_INTERIOR_ID
};

static
    gRaceRecordData[MAX_RACE_RECORDS][E_RACE_RECORD_DATA],
    Float:gRaceRecordStarts[MAX_RACE_RECORDS][MAX_RACERS][4],
    Float:gRaceRecordCPs[MAX_RACE_RECORDS][MAX_RACE_CHECKPOINTS][3]
;

static
    gRaceData[MAX_RACES][E_RACE_DATA],
    gRacerData[MAX_PLAYERS][E_RACER_DATA]
;

// Ids come off a free list: a slot given back points at the one given back
// before it, and slots never used yet come off a high water mark. Taking one is
// a read and a write, giving one back is the same, and a full array is answered
// without looking at anything. Nothing is set up at boot.
static
    gRaceRecordNextFree[_:MAX_RACE_RECORDS],
    gRaceRecordFreeHead = -1,
    gRaceRecordHighWater,
    gRaceRecordCount
;

static
    gRaceNextFree[_:MAX_RACES],
    gRaceFreeHead = -1,
    gRaceHighWater,
    gRaceCount
;

// Who is in each race, packed end to end with no gaps. The row is MAX_RACERS
// wide and not MAX_PLAYERS wide, because a race cannot hold more than it has
// start lines for and the grid slot allocator below already refuses past that.
//
// Joining appends. Leaving moves whoever is last into the hole, which is why
// gRacerIndex is kept alongside: it says where a player sits in their race's
// row, so taking one out is a read and three writes wherever they are.
//
// A linked list threaded through two MAX_PLAYERS arrays does the same job in
// the same time, and was what this held first. It loses on both counts that
// matter here: 2*MAX_PLAYERS cells against MAX_RACES*MAX_RACERS + MAX_PLAYERS,
// which at the defaults is 2032 against 1528, and walking it chases a pointer
// into a scattered array per step where this one reads straight down a row.
// The row is also what lets GetRaceRacers hand the script a snapshot cheaply.
static
    Race:gRacerRaceID[MAX_PLAYERS] = { INVALID_RACE_ID, ... },
    gRacerVehicleID[MAX_PLAYERS] = { INVALID_VEHICLE_ID, ... },
    gRacerIndex[MAX_PLAYERS] = { -1, ... },
    gRaceParticipants[MAX_RACES][MAX_RACERS],
    gRaceRacerCount[MAX_RACES]
;

// Which grid slot each racer is on. A slot given up is pushed back onto the
// race's own stack and handed to the next joiner, which the counter this
// replaces never did: it only ever counted up, so the thirty third player to
// pass through a two man race was placed at start position thirty two, off the
// end of an array that has room for the starts the map was drawn with.
static
    gRaceStartFree[MAX_RACES][MAX_RACERS],
    gRaceStartFreeCount[MAX_RACES],
    gRaceStartHighWater[MAX_RACES]
;

// Every countdown timer carries the run it was made for. A timer belonging to a
// race that has since been destroyed, restarted or refilled finds a number that
// no longer matches and returns without touching anything, so a stale tick can
// never start a race that is not there.
static
    gRaceSession[MAX_RACES],
    gRaceSessionCounter
;

// The same trick one level down. A racer's return clock carries the run it was
// started for, and the number is stepped whenever they join, leave, finish or
// get back in, so a timer left over from the last time they were on foot finds
// a number it does not recognise and does nothing.
static
    gRacerSession[MAX_PLAYERS],
    gRacerSessionCounter,
    gRacerReturnTimer[MAX_PLAYERS]
;

/**
 * # Functions
 */

forward RaceRecord:RecordRace(const name[] = "");
forward bool:DestroyRaceRecord(RaceRecord:recordid);
forward bool:IsValidRaceRecord(RaceRecord:recordid);
forward GetRaceRecordCount();
forward RaceRecord:GetRaceRecordPoolSize();
forward bool:SetRaceRecordName(RaceRecord:recordid, const name[]);
forward bool:GetRaceRecordName(RaceRecord:recordid, name[], size = sizeof (name));
forward CreateRaceRecordRacerStart(RaceRecord:recordid, Float:x, Float:y, Float:z, Float:a);
forward CreateRaceRecordCP(RaceRecord:recordid, Float:x, Float:y, Float:z);
forward GetRaceRecordRacerStartCount(RaceRecord:recordid);
forward GetRaceRecordCPCount(RaceRecord:recordid);
forward bool:GetRaceRecordRacerStart(RaceRecord:recordid, index, &Float:x, &Float:y, &Float:z, &Float:a);
forward bool:GetRaceRecordCP(RaceRecord:recordid, index, &Float:x, &Float:y, &Float:z);

forward Race:CreateRace(creatorid, modelid, const name[] = "", RaceRecord:recordid = INVALID_RACE_RECORD_ID, laps = MIN_RACE_LAPS, maxRacers = MAX_RACERS, countDown = MIN_RACE_COUNT_DOWN, worldid = 0, interiorid = 0, colour1 = RACE_COLOUR_RANDOM, colour2 = RACE_COLOUR_RANDOM);
forward bool:DestroyRace(Race:raceid);
forward bool:IsValidRace(Race:raceid);
forward GetRaceCount();
forward Race:GetRacePoolSize();
forward bool:StartRace(Race:raceid);
forward bool:StopRace(Race:raceid);
forward bool:SetRaceCreator(Race:raceid, playerid);
forward GetRaceCreator(Race:raceid);
forward bool:SetRaceVehicleModel(Race:raceid, modelid);
forward GetRaceVehicleModel(Race:raceid);
forward bool:SetRaceVehicleColours(Race:raceid, colour1, colour2);
forward bool:GetRaceVehicleColours(Race:raceid, &colour1, &colour2);
forward bool:SetRaceName(Race:raceid, const name[]);
forward bool:GetRaceName(Race:raceid, name[], size = sizeof (name));
forward bool:SetRaceRecord(Race:raceid, RaceRecord:recordid);
forward RaceRecord:GetRaceRecord(Race:raceid);
forward bool:SetRaceLaps(Race:raceid, laps);
forward GetRaceLaps(Race:raceid);
forward bool:SetRaceMaxRacers(Race:raceid, maxRacers);
forward GetRaceMaxRacers(Race:raceid);
forward bool:SetRaceCountdown(Race:raceid, countDown);
forward GetRaceCountdown(Race:raceid);
forward GetRaceCountdownLeft(Race:raceid);
forward bool:SetRaceVirtualWorld(Race:raceid, worldid);
forward GetRaceVirtualWorld(Race:raceid);
forward bool:SetRaceInterior(Race:raceid, interiorid);
forward GetRaceInterior(Race:raceid);
forward bool:ToggleRaceFlags(Race:raceid, flags);
forward bool:HasRaceFlags(Race:raceid, flags);
forward GetRaceRacerCount(Race:raceid);
forward GetRaceFreeSlots(Race:raceid);
forward GetRaceRacer(Race:raceid, index);
forward GetRaceRacers(Race:raceid, racers[], size = sizeof (racers));
forward GetRaceFirstRacer(Race:raceid);
forward GetNextRacer(playerid);

forward RACE_JOIN:PutPlayerInRace(playerid, Race:raceid);
forward bool:RemovePlayerFromRace(playerid, Float:x = 0.0, Float:y = 0.0, Float:z = 0.0, Float:a = 0.0, worldid = 0, interiorid = 0, bool:spawnLastPos = true);
forward bool:IsPlayerInAnyRace(playerid);
forward Race:GetPlayerRace(playerid);
forward GetPlayerRaceVehicle(playerid);
forward GetPlayerRaceCP(playerid);
forward GetPlayerRaceLap(playerid);
forward GetPlayerRacePosition(playerid);
forward bool:HasPlayerFinishedRace(playerid);

// Milliseconds left to get back into the car, or zero for a racer who is in it.
//
// This is the whole of the support for putting a countdown on the screen, and
// it is a read and a subtraction: the library never calls out once a second to
// tell anybody the number, because a script that wants it already knows when to
// ask. OnPlayerRaceVehicleExit says start showing, OnPlayerRaceVehicleReturn
// says stop, and between the two this answers what to show.
forward GetPlayerRaceReturnTimeLeft(playerid);

forward OnRaceCreate(Race:raceid);
// The creator comes with it because by the time this fires there is no way
// left to ask: the race has already been taken apart, so GetRaceCreator answers
// INVALID_PLAYER_ID like it would for any race that is not there. A script
// holding an id per player has to be able to find whose it was without going
// through every player to do it.
forward OnRaceDestroy(Race:raceid, creatorid);

// The three that speak to the whole grid are handed it, rather than being
// called once a head and leaving the script to ask who is there.
//
// participants is a copy made by the call, not the race's own row, and that is
// most of the point: the script can take racers out of the race while it walks
// what it was given and nothing moves underneath it. OnRaceFinish is exactly
// where a script does that, putting everybody back on their feet.
//
// size is how many of them there are, so the array is read to size and never to
// sizeof: only the first size cells mean anything.
forward OnRaceStart(Race:raceid, const participants[], size);
forward OnRaceFinish(Race:raceid, const participants[], size);

forward OnPlayerJoinRace(playerid, Race:raceid);
forward OnPlayerLeaveRace(playerid, Race:raceid);

// Once a second, for the race and not once for each racer in it. Calling out a
// head made the fan out the library's decision and charged a public lookup per
// racer per second for it; this hands the field over and lets the script decide
// who is shown what.
//
// There is no flag here saying the countdown has run out, because the lights
// going is OnRaceStart. This one only ever counts.
//
// Away from these three callbacks the field is still asked for rather than
// given: ForEachRacer to walk it, GetRaceRacers for a copy of it.
forward OnRaceCountdownUpdate(Race:raceid, countdown, const participants[], size);

// Spelled out, and not OnPlayerRaceCP as it was. pp-hooks rewrites CP to
// Checkpoint in the name of anything hooked through it, so a script writing
// `hook OnPlayerRaceCP` would end up listening on OnPlayerRaceCheckpoint and
// never hear a word: the short name compiles, registers and does nothing. The
// table in pp-hooks carries a DynamicCP entry that does nothing but shield
// OnPlayerEnterDynamicCP from that same rewrite, which is what the rule looks
// like from the other side.
forward OnPlayerRaceCheckpoint(playerid, Race:raceid, check);
forward OnPlayerRaceLap(playerid, Race:raceid, lap);

// Stepping out of the car and getting back into it. time is how long they have,
// so a script can put the same number on the screen the library is counting.
forward OnPlayerRaceVehicleExit(playerid, Race:raceid, time);
forward OnPlayerRaceVehicleReturn(playerid, Race:raceid);

// Once a second while they are on foot, counting down. The library is the one
// holding the clock, so it is the one that knows who is walking without having
// to go looking: a script driving this itself would have to search every race
// and every racer once a second to find the one or two who are out of the car.
forward OnPlayerRaceReturnUpdate(playerid, Race:raceid, secondsLeft);

// The clock ran out. Answer zero to leave them in the race and let them keep
// walking; anything else, including not writing the callback at all, takes them
// out the same way death and disconnection do.
forward OnPlayerRaceReturnTimeout(playerid, Race:raceid);
forward OnPlayerFinishRace(playerid, Race:raceid, position);

/**
 * # Internal
 */

static stock GetFreeRaceRecordIndexInternal() {
    if (gRaceRecordFreeHead != -1) {
        new const
            index = gRaceRecordFreeHead
        ;

        gRaceRecordFreeHead = gRaceRecordNextFree[index];

        return index;
    }

    if (gRaceRecordHighWater != _:MAX_RACE_RECORDS) {
        return gRaceRecordHighWater++;
    }

    return -1;
}

static stock GetFreeRaceIndexInternal() {
    if (gRaceFreeHead != -1) {
        new const
            index = gRaceFreeHead
        ;

        gRaceFreeHead = gRaceNextFree[index];

        return index;
    }

    if (gRaceHighWater != _:MAX_RACES) {
        return gRaceHighWater++;
    }

    return -1;
}

static stock bool:IsValidRaceRecordInternal(RaceRecord:recordid) {
    if (!(0 <= _:recordid < _:MAX_RACE_RECORDS)) {
        return false;
    }

    return gRaceRecordData[recordid][E_RACE_RECORD_CREATED];
}

static stock bool:IsValidRaceInternal(Race:raceid) {
    if (!(0 <= _:raceid < _:MAX_RACES)) {
        return false;
    }

    return gRaceData[raceid][E_RACE_CREATED];
}

// A real colour, or the word for "pick one". Anything else is neither, and is
// turned into random rather than handed to CreateVehicle, which reads a number
// off the end of the table and paints the car with whatever is there.
static stock bool:IsRaceColourInternal(colour) {
    if (colour == RACE_COLOUR_RANDOM) {
        return true;
    }

    return (MIN_RACE_COLOUR <= colour <= MAX_RACE_COLOUR);
}

static stock bool:IsRacerInternal(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }

    return IsValidRaceInternal(gRacerRaceID[playerid]);
}

// How many players this race can hold at once: whichever is smaller of the
// number it was set up for and the number of places the map was drawn with. A
// map with two starts cannot seat thirty two whatever the room says.
static stock GetRaceCapacityInternal(Race:raceid) {
    new const
        RaceRecord:recordid = gRaceData[raceid][E_RACE_RECORD_ID]
    ;

    if (!IsValidRaceRecordInternal(recordid)) {
        return 0;
    }

    new const
        starts = gRaceRecordData[recordid][E_RACE_RECORD_START_COUNT],
        seats = gRaceData[raceid][E_RACE_MAX_RACERS]
    ;

    if (starts < seats) {
        return starts;
    }

    return seats;
}

// Takes the grid slot given back most recently, or the next one never used.
static stock ClaimRaceStartSlotInternal(Race:raceid) {
    if (gRaceStartFreeCount[raceid]) {
        return gRaceStartFree[raceid][--gRaceStartFreeCount[raceid]];
    }

    if (gRaceStartHighWater[raceid] < GetRaceCapacityInternal(raceid)) {
        return gRaceStartHighWater[raceid]++;
    }

    return -1;
}

static stock ReleaseRaceStartSlotInternal(Race:raceid, index) {
    if (!(0 <= index < MAX_RACERS)) {
        return;
    }

    gRaceStartFree[raceid][gRaceStartFreeCount[raceid]++] = index;
}

static stock ResetRaceStartSlotsInternal(Race:raceid) {
    gRaceStartFreeCount[raceid] = 0;
    gRaceStartHighWater[raceid] = 0;
}

static stock AddRacerInternal(Race:raceid, playerid) {
    new const
        index = gRaceRacerCount[raceid]++
    ;

    gRaceParticipants[raceid][index] = playerid;
    gRacerIndex[playerid] = index;
}

// Whoever is last is moved into the hole rather than everybody behind it being
// shuffled forward, so this costs the same whether the racer leaving is the
// first on the grid or the last.
static stock RemoveRacerInternal(Race:raceid, playerid) {
    new const
        index = gRacerIndex[playerid],
        last = --gRaceRacerCount[raceid]
    ;

    if (!(0 <= index <= last)) {
        gRacerIndex[playerid] = -1;

        return;
    }

    new const
        moved = gRaceParticipants[raceid][last]
    ;

    gRaceParticipants[raceid][index] = moved;
    gRacerIndex[moved] = index;

    gRacerIndex[playerid] = -1;
}

static stock StopRaceTimerInternal(Race:raceid) {
    if (!gRaceData[raceid][E_RACE_TIMER_ID]) {
        return;
    }

    KillTimer(gRaceData[raceid][E_RACE_TIMER_ID]);
    gRaceData[raceid][E_RACE_TIMER_ID] = 0;
}

static stock StopRacerReturnTimerInternal(playerid) {
    gRacerData[playerid][E_RACER_RETURN_DEADLINE] = 0;

    if (!gRacerReturnTimer[playerid]) {
        return;
    }

    KillTimer(gRacerReturnTimer[playerid]);
    gRacerReturnTimer[playerid] = 0;
}

// Stepped so that a clock already on its way finds a run it does not know, and
// called from everywhere a racer's standing changes rather than only where the
// clock is stopped: leaving and rejoining between two ticks would otherwise be
// answered by the timer the first spell on foot started.
static stock StepRacerSessionInternal(playerid) {
    StopRacerReturnTimerInternal(playerid);

    gRacerSession[playerid] = ++gRacerSessionCounter;
}

static stock bool:IsRacerSessionValidInternal(playerid, session) {
    if (!IsRacerInternal(playerid)) {
        return false;
    }

    return gRacerSession[playerid] == session;
}

static stock bool:IsRaceSessionValidInternal(Race:raceid, session) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    return gRaceSession[raceid] == session;
}

// Puts the next checkpoint in front of a racer. The last one of the last lap is
// a finish and has nothing after it; the last one of any earlier lap points
// back at the first, because a lap ends where it began.
static stock ArmRacerCheckpointInternal(playerid) {
    new const
        Race:raceid = gRacerRaceID[playerid],
        RaceRecord:recordid = gRaceData[raceid][E_RACE_RECORD_ID],
        count = gRaceRecordData[recordid][E_RACE_RECORD_CP_COUNT],
        check = gRacerData[playerid][E_RACER_CP_COUNT]
    ;

    if (!(0 <= check < count)) {
        return;
    }

    new const
        bool:lastCP = (check == count - 1),
        bool:lastLap = (gRacerData[playerid][E_RACER_LAP_COUNT] == gRaceData[raceid][E_RACE_LAPS] - 1)
    ;

    // Written out rather than chosen inside the call, because a conditional
    // hands back a value with no tag on it and these two are CP_TYPE.
    new
        CP_TYPE:type
    ;

    if (lastCP && lastLap) {
        if (IsRaceArial(raceid)) {
            type = CP_TYPE_AIR_FINISH;
        } else {
            type = CP_TYPE_GROUND_FINISH;
        }

        SetPlayerRaceCheckpoint(playerid, type,
            gRaceRecordCPs[recordid][check][0], gRaceRecordCPs[recordid][check][1], gRaceRecordCPs[recordid][check][2],
            0.0, 0.0, 0.0,
            RACE_CHECKPOINT_SIZE
        );

        return;
    }

    if (IsRaceArial(raceid)) {
        type = CP_TYPE_AIR_NORMAL;
    } else {
        type = CP_TYPE_GROUND_NORMAL;
    }

    new const
        next = (check + 1) % count
    ;

    SetPlayerRaceCheckpoint(playerid, type,
        gRaceRecordCPs[recordid][check][0], gRaceRecordCPs[recordid][check][1], gRaceRecordCPs[recordid][check][2],
        gRaceRecordCPs[recordid][next][0], gRaceRecordCPs[recordid][next][1], gRaceRecordCPs[recordid][next][2],
        RACE_CHECKPOINT_SIZE
    );
}

// A race is over once everybody still in it has crossed the line. The flag goes
// down here so the room can be started again without being built again.
static stock CheckRaceOverInternal(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return;
    }

    // A race that has already been called cannot be called again, which is also
    // what keeps this from going round twice when the script empties the grid
    // from inside OnRaceFinish.
    if (!IsRaceStarted(raceid)) {
        return;
    }

    // A grid that has emptied ends the race as surely as everybody crossing the
    // line does. Without this the room stays flagged as running with nobody in
    // it: nothing can be started, nothing can be set, and the only way out is
    // to throw the room away and build it again.
    if (gRaceRacerCount[raceid] && gRaceData[raceid][E_RACE_FINISH_COUNT] < gRaceRacerCount[raceid]) {
        return;
    }

    gRaceData[raceid][E_RACE_FLAGS] &= ~RACE_FLAGS_STARTED;
    gRaceData[raceid][E_RACE_COUNT_DOWN_LEFT] = 0;

    CallLocalFunction("OnRaceFinish", "iai", _:raceid, gRaceParticipants[raceid], gRaceRacerCount[raceid]);
}

static stock FinishRacerInternal(playerid) {
    new const
        Race:raceid = gRacerRaceID[playerid],
        position = ++gRaceData[raceid][E_RACE_FINISH_COUNT]
    ;

    gRacerData[playerid][E_RACER_FINISHED] = true;
    gRacerData[playerid][E_RACER_POSITION] = position;

    // Whoever has crossed the line may get out and walk about.
    StepRacerSessionInternal(playerid);

    DisablePlayerRaceCheckpoint(playerid);

    CallLocalFunction("OnPlayerFinishRace", "iii", playerid, _:raceid, position);

    CheckRaceOverInternal(raceid);
}

static stock DoEnterRaceCPInternal(playerid) {
    if (!IsRacerInternal(playerid)) {
        return;
    }

    new const
        Race:raceid = gRacerRaceID[playerid]
    ;

    if (!IsRaceStarted(raceid)) {
        return;
    }

    if (gRacerData[playerid][E_RACER_FINISHED]) {
        return;
    }

    new const
        RaceRecord:recordid = gRaceData[raceid][E_RACE_RECORD_ID],
        count = gRaceRecordData[recordid][E_RACE_RECORD_CP_COUNT],
        check = gRacerData[playerid][E_RACER_CP_COUNT]
    ;

    DisablePlayerRaceCheckpoint(playerid);

    CallLocalFunction("OnPlayerRaceCheckpoint", "iii", playerid, _:raceid, check);

    // The script is allowed to pull the player out of the race from in there,
    // and everything below this line is about a player who is still in one.
    if (gRacerRaceID[playerid] != raceid) {
        return;
    }

    if (check + 1 < count) {
        gRacerData[playerid][E_RACER_CP_COUNT] = check + 1;

        ArmRacerCheckpointInternal(playerid);

        return;
    }

    new const
        lap = ++gRacerData[playerid][E_RACER_LAP_COUNT]
    ;

    gRacerData[playerid][E_RACER_CP_COUNT] = 0;

    if (lap < gRaceData[raceid][E_RACE_LAPS]) {
        CallLocalFunction("OnPlayerRaceLap", "iii", playerid, _:raceid, lap);

        if (gRacerRaceID[playerid] != raceid) {
            return;
        }

        ArmRacerCheckpointInternal(playerid);

        return;
    }

    FinishRacerInternal(playerid);
}

static stock BeginRaceInternal(Race:raceid) {
    StopRaceTimerInternal(raceid);

    gRaceData[raceid][E_RACE_COUNT_DOWN_LEFT] = 0;
    gRaceData[raceid][E_RACE_FLAGS] |= RACE_FLAGS_STARTED;

    // Everybody is let go and given their first checkpoint before anybody is
    // told the lights have gone, so a script that pulls a racer out from inside
    // that callback cannot leave the racers behind them sitting frozen on the
    // grid with nothing in front of them. Nothing in this walk calls out, so
    // nothing can be unlinked underneath it.
    for (new i, count = gRaceRacerCount[raceid]; i != count; ++i) {
        new const
            playerid = gRaceParticipants[raceid][i]
        ;

        TogglePlayerControllable(playerid, true);
        ArmRacerCheckpointInternal(playerid);
    }

    // The lights going is OnRaceStart, and it always was: a second callback
    // saying the same thing with a flag set was one too many.
    CallLocalFunction("OnRaceStart", "iai", _:raceid, gRaceParticipants[raceid], gRaceRacerCount[raceid]);
}

// Takes a player out of a race without putting them anywhere: the two callers
// want opposite things done with the body afterwards, and neither of them wants
// this deciding it.
static stock ClearRacerInternal(playerid, Race:raceid) {
    StepRacerSessionInternal(playerid);

    RemoveRacerInternal(raceid, playerid);
    ReleaseRaceStartSlotInternal(raceid, gRacerData[playerid][E_RACER_START_INDEX]);

    if (gRacerData[playerid][E_RACER_FINISHED]) {
        --gRaceData[raceid][E_RACE_FINISH_COUNT];
    }

    if (gRacerVehicleID[playerid] != INVALID_VEHICLE_ID) {
        DestroyVehicle(gRacerVehicleID[playerid]);
    }

    DisablePlayerRaceCheckpoint(playerid);

    gRacerRaceID[playerid]                    = INVALID_RACE_ID;
    gRacerVehicleID[playerid]                 = INVALID_VEHICLE_ID;

    gRacerData[playerid][E_RACER_CP_COUNT]    = 0;
    gRacerData[playerid][E_RACER_LAP_COUNT]   = 0;
    gRacerData[playerid][E_RACER_START_INDEX] = -1;
    gRacerData[playerid][E_RACER_POSITION]    = 0;
    gRacerData[playerid][E_RACER_FINISHED]    = false;
}

/**
 * # Race Record
 */

stock RaceRecord:RecordRace(const name[] = "") {
    new const
        index = GetFreeRaceRecordIndexInternal()
    ;

    if (index == -1) {
        return INVALID_RACE_RECORD_ID;
    }

    new const
        RaceRecord:recordid = RaceRecord:index
    ;

    static const
        RACE_RECORD_DATA_NONE[E_RACE_RECORD_DATA]
    ;

    gRaceRecordData[recordid] = RACE_RECORD_DATA_NONE;
    gRaceRecordData[recordid][E_RACE_RECORD_CREATED] = true;

    ++gRaceRecordCount;

    if (name[0] == EOS) {
        new
            buffer[MAX_RACE_RECORD_NAME + 1]
        ;

        format(buffer, sizeof (buffer), "Race Map %i", index);

        strpack(gRaceRecordData[recordid][E_RACE_RECORD_NAME], buffer, (MAX_RACE_RECORD_NAME + 1) char);
    } else {
        strpack(gRaceRecordData[recordid][E_RACE_RECORD_NAME], name, (MAX_RACE_RECORD_NAME + 1) char);
    }

    return recordid;
}

// A map that a room is still pointing at is not thrown away: the room would be
// left holding a number that answers nothing, and every racer in it would be
// sent to a start line that is no longer written down anywhere.
stock bool:DestroyRaceRecord(RaceRecord:recordid) {
    if (!IsValidRaceRecordInternal(recordid)) {
        return false;
    }

    for (new Race:raceid; _:raceid != gRaceHighWater; ++raceid) {
        if (!gRaceData[raceid][E_RACE_CREATED]) {
            continue;
        }

        if (gRaceData[raceid][E_RACE_RECORD_ID] == recordid) {
            return false;
        }
    }

    static const
        RACE_RECORD_DATA_NONE[E_RACE_RECORD_DATA]
    ;

    gRaceRecordData[recordid] = RACE_RECORD_DATA_NONE;

    gRaceRecordNextFree[_:recordid] = gRaceRecordFreeHead;
    gRaceRecordFreeHead = _:recordid;

    --gRaceRecordCount;

    return true;
}

stock bool:IsValidRaceRecord(RaceRecord:recordid) {
    return IsValidRaceRecordInternal(recordid);
}

stock GetRaceRecordCount() {
    return gRaceRecordCount;
}

// How far a script has to count to have seen every map. Slots below this that
// hold nothing answer false to IsValidRaceRecord.
stock RaceRecord:GetRaceRecordPoolSize() {
    return RaceRecord:gRaceRecordHighWater;
}

stock bool:SetRaceRecordName(RaceRecord:recordid, const name[]) {
    if (!IsValidRaceRecordInternal(recordid)) {
        return false;
    }

    if (name[0] == EOS) {
        return false;
    }

    strpack(gRaceRecordData[recordid][E_RACE_RECORD_NAME], name, (MAX_RACE_RECORD_NAME + 1) char);

    return true;
}

stock bool:GetRaceRecordName(RaceRecord:recordid, name[], size = sizeof (name)) {
    if (!IsValidRaceRecordInternal(recordid)) {
        return false;
    }

    strunpack(name, gRaceRecordData[recordid][E_RACE_RECORD_NAME], size);

    return true;
}

stock CreateRaceRecordRacerStart(RaceRecord:recordid, Float:x, Float:y, Float:z, Float:a) {
    if (!IsValidRaceRecordInternal(recordid)) {
        return -1;
    }

    if (gRaceRecordData[recordid][E_RACE_RECORD_START_COUNT] == MAX_RACERS) {
        return -1;
    }

    new const
        index = gRaceRecordData[recordid][E_RACE_RECORD_START_COUNT]++
    ;

    gRaceRecordStarts[recordid][index][0] = x;
    gRaceRecordStarts[recordid][index][1] = y;
    gRaceRecordStarts[recordid][index][2] = z;
    gRaceRecordStarts[recordid][index][3] = a;

    return index;
}

stock CreateRaceRecordCP(RaceRecord:recordid, Float:x, Float:y, Float:z) {
    if (!IsValidRaceRecordInternal(recordid)) {
        return -1;
    }

    if (gRaceRecordData[recordid][E_RACE_RECORD_CP_COUNT] == MAX_RACE_CHECKPOINTS) {
        return -1;
    }

    new const
        index = gRaceRecordData[recordid][E_RACE_RECORD_CP_COUNT]++
    ;

    gRaceRecordCPs[recordid][index][0] = x;
    gRaceRecordCPs[recordid][index][1] = y;
    gRaceRecordCPs[recordid][index][2] = z;

    return index;
}

stock GetRaceRecordRacerStartCount(RaceRecord:recordid) {
    if (!IsValidRaceRecordInternal(recordid)) {
        return 0;
    }

    return gRaceRecordData[recordid][E_RACE_RECORD_START_COUNT];
}

stock GetRaceRecordCPCount(RaceRecord:recordid) {
    if (!IsValidRaceRecordInternal(recordid)) {
        return 0;
    }

    return gRaceRecordData[recordid][E_RACE_RECORD_CP_COUNT];
}

stock bool:GetRaceRecordRacerStart(RaceRecord:recordid, index, &Float:x, &Float:y, &Float:z, &Float:a) {
    if (!IsValidRaceRecordInternal(recordid)) {
        return false;
    }

    if (!(0 <= index < gRaceRecordData[recordid][E_RACE_RECORD_START_COUNT])) {
        return false;
    }

    x = gRaceRecordStarts[recordid][index][0];
    y = gRaceRecordStarts[recordid][index][1];
    z = gRaceRecordStarts[recordid][index][2];
    a = gRaceRecordStarts[recordid][index][3];

    return true;
}

stock bool:GetRaceRecordCP(RaceRecord:recordid, index, &Float:x, &Float:y, &Float:z) {
    if (!IsValidRaceRecordInternal(recordid)) {
        return false;
    }

    if (!(0 <= index < gRaceRecordData[recordid][E_RACE_RECORD_CP_COUNT])) {
        return false;
    }

    x = gRaceRecordCPs[recordid][index][0];
    y = gRaceRecordCPs[recordid][index][1];
    z = gRaceRecordCPs[recordid][index][2];

    return true;
}

/**
 * # Race
 */

stock Race:CreateRace(creatorid, modelid, const name[] = "", RaceRecord:recordid = INVALID_RACE_RECORD_ID, laps = MIN_RACE_LAPS, maxRacers = MAX_RACERS, countDown = MIN_RACE_COUNT_DOWN, worldid = 0, interiorid = 0, colour1 = RACE_COLOUR_RANDOM, colour2 = RACE_COLOUR_RANDOM) {
    if (!IsPlayerConnected(creatorid)) {
        return INVALID_RACE_ID;
    }

    // A room without a map has nowhere to put anybody. The one handed in is
    // used when it is real, and the first one on the books otherwise, which is
    // a choice a script can always overrule with SetRaceRecord. The map handed
    // in used to be dropped on the floor unless it was invalid, so every race
    // ran on whichever map the random landed on.
    if (!IsValidRaceRecordInternal(recordid)) {
        recordid = INVALID_RACE_RECORD_ID;

        for (new RaceRecord:i; _:i != gRaceRecordHighWater; ++i) {
            if (!gRaceRecordData[i][E_RACE_RECORD_CREATED]) {
                continue;
            }

            recordid = i;
            break;
        }
    }

    if (recordid == INVALID_RACE_RECORD_ID) {
        return INVALID_RACE_ID;
    }

    new const
        Race:raceid = Race:GetFreeRaceIndexInternal()
    ;

    if (_:raceid == -1) {
        return INVALID_RACE_ID;
    }

    static const
        RACE_DATA_NONE[E_RACE_DATA]
    ;

    // Everything a run leaves behind is wiped here rather than at the end of
    // the run before, because a slot handed back to the free list is the one
    // place a race is certain to have finished with.
    gRaceData[raceid] = RACE_DATA_NONE;

    gRaceRacerCount[raceid] = 0;

    ResetRaceStartSlotsInternal(raceid);

    gRaceSession[raceid] = ++gRaceSessionCounter;

    gRaceData[raceid][E_RACE_CREATED]     = true;
    gRaceData[raceid][E_RACE_RECORD_ID]   = recordid;
    gRaceData[raceid][E_RACE_CREATOR_ID]  = creatorid;
    gRaceData[raceid][E_RACE_MODEL_ID]    = modelid;
    gRaceData[raceid][E_RACE_COLOUR_1]    = IsRaceColourInternal(colour1) ? colour1 : RACE_COLOUR_RANDOM;
    gRaceData[raceid][E_RACE_COLOUR_2]    = IsRaceColourInternal(colour2) ? colour2 : RACE_COLOUR_RANDOM;
    gRaceData[raceid][E_RACE_LAPS]        = clamp(laps, MIN_RACE_LAPS, MAX_RACE_LAPS);
    gRaceData[raceid][E_RACE_MAX_RACERS]  = clamp(maxRacers, MIN_RACERS, MAX_RACERS);
    gRaceData[raceid][E_RACE_COUNT_DOWN]  = clamp(countDown, MIN_RACE_COUNT_DOWN, MAX_RACE_COUNT_DOWN);
    gRaceData[raceid][E_RACE_WORLD_ID]    = worldid;
    gRaceData[raceid][E_RACE_INTERIOR_ID] = interiorid;

    if (name[0] == EOS) {
        new
            creatorName[MAX_PLAYER_NAME + 1],
            buffer[MAX_RACE_NAME + 1]
        ;

        GetPlayerName(creatorid, creatorName);
        format(buffer, sizeof (buffer), "%s's Race", creatorName);

        strpack(gRaceData[raceid][E_RACE_NAME], buffer, (MAX_RACE_NAME + 1) char);
    } else {
        strpack(gRaceData[raceid][E_RACE_NAME], name, (MAX_RACE_NAME + 1) char);
    }

    ++gRaceCount;

    CallLocalFunction("OnRaceCreate", "i", _:raceid);

    return raceid;
}

// Everybody still inside goes back where they came from before the room stops
// existing, because nothing else is ever going to come and get them: there was
// no way to take a race down at all before, and the example reached into the
// iterator to do it, which left the racers frozen in their cars for good.
stock bool:DestroyRace(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    StopRaceTimerInternal(raceid);

    // Stepped so that a countdown tick already on its way finds a race it does
    // not recognise, whether or not the slot is taken again before it lands.
    gRaceSession[raceid] = ++gRaceSessionCounter;

    // Drained from the head rather than walked with the next one held in hand,
    // because taking one racer out can take others with them: the script hears
    // about every departure and is allowed to act on it. Whoever is at the
    // front is always still in the race, and taking them out always moves the
    // front along, so this ends however many go at a time.
    while (gRaceRacerCount[raceid]) {
        RemovePlayerFromRace(gRaceParticipants[raceid][0]);
    }

    // The script heard about every one of those departures and is allowed to
    // have taken the room down itself from inside one of them. Going on from
    // here would put this slot on the free list a second time, and a free list
    // holding the same id twice hands it to two races at once.
    if (!IsValidRaceInternal(raceid)) {
        return true;
    }

    new const
        creatorid = gRaceData[raceid][E_RACE_CREATOR_ID]
    ;

    static const
        RACE_DATA_NONE[E_RACE_DATA]
    ;

    gRaceData[raceid] = RACE_DATA_NONE;

    gRaceRacerCount[raceid] = 0;

    ResetRaceStartSlotsInternal(raceid);

    gRaceNextFree[_:raceid] = gRaceFreeHead;
    gRaceFreeHead = _:raceid;

    --gRaceCount;

    CallLocalFunction("OnRaceDestroy", "ii", _:raceid, creatorid);

    return true;
}

stock bool:IsValidRace(Race:raceid) {
    return IsValidRaceInternal(raceid);
}

stock GetRaceCount() {
    return gRaceCount;
}

stock Race:GetRacePoolSize() {
    return Race:gRaceHighWater;
}

stock bool:StartRace(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    if (IsRaceStarted(raceid)) {
        return false;
    }

    if (gRaceRacerCount[raceid] < MIN_RACERS) {
        return false;
    }

    StopRaceTimerInternal(raceid);

    // A run of its own, so that the timer left over from a countdown that was
    // stopped halfway cannot come back and start this one.
    gRaceSession[raceid] = ++gRaceSessionCounter;

    gRaceData[raceid][E_RACE_FINISH_COUNT] = 0;

    new const
        countDown = gRaceData[raceid][E_RACE_COUNT_DOWN]
    ;

    gRaceData[raceid][E_RACE_COUNT_DOWN_LEFT] = countDown;

    if (countDown <= 0) {
        BeginRaceInternal(raceid);

        return true;
    }

    // The first number goes up now rather than a second from now, so a three
    // second countdown shows three, two, one and then GO across three seconds.
    CallLocalFunction("OnRaceCountdownUpdate", "iiai", _:raceid, countDown, gRaceParticipants[raceid], gRaceRacerCount[raceid]);

    gRaceData[raceid][E_RACE_TIMER_ID] = SetTimerEx("@Race_Countdown", 1000, true, "ii", _:raceid, gRaceSession[raceid]);

    return true;
}

// Puts a race back to the line without emptying the room.
stock bool:StopRace(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    StopRaceTimerInternal(raceid);

    gRaceSession[raceid] = ++gRaceSessionCounter;

    gRaceData[raceid][E_RACE_COUNT_DOWN_LEFT] = 0;
    gRaceData[raceid][E_RACE_FINISH_COUNT] = 0;
    gRaceData[raceid][E_RACE_FLAGS] &= ~RACE_FLAGS_STARTED;

    for (new i = gRaceRacerCount[raceid]; i-- > 0; ) {
        new const
            playerid = gRaceParticipants[raceid][i]
        ;

        DisablePlayerRaceCheckpoint(playerid);

        gRacerData[playerid][E_RACER_CP_COUNT] = 0;
        gRacerData[playerid][E_RACER_LAP_COUNT] = 0;
        gRacerData[playerid][E_RACER_POSITION] = 0;
        gRacerData[playerid][E_RACER_FINISHED] = false;
    }

    return true;
}

stock bool:SetRaceCreator(Race:raceid, playerid) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    gRaceData[raceid][E_RACE_CREATOR_ID] = playerid;

    return true;
}

stock GetRaceCreator(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return INVALID_PLAYER_ID;
    }

    return gRaceData[raceid][E_RACE_CREATOR_ID];
}

stock bool:SetRaceVehicleModel(Race:raceid, modelid) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    gRaceData[raceid][E_RACE_MODEL_ID] = modelid;

    return true;
}

stock GetRaceVehicleModel(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return 0;
    }

    return gRaceData[raceid][E_RACE_MODEL_ID];
}

// The cars already on the grid are repainted, because this is what a setup menu
// calls and the point of picking a colour there is seeing it. A car that has not
// been made yet gets the colour when it is, in PutPlayerInRace.
//
// RACE_COLOUR_RANDOM only means anything at the moment a car is made, so asking
// for it does not repaint what is already standing there: there is no such
// colour to put on them.
stock bool:SetRaceVehicleColours(Race:raceid, colour1, colour2) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    if (!IsRaceColourInternal(colour1)) {
        return false;
    }

    if (!IsRaceColourInternal(colour2)) {
        return false;
    }

    gRaceData[raceid][E_RACE_COLOUR_1] = colour1;
    gRaceData[raceid][E_RACE_COLOUR_2] = colour2;

    if (colour1 == RACE_COLOUR_RANDOM || colour2 == RACE_COLOUR_RANDOM) {
        return true;
    }

    for (new i, count = gRaceRacerCount[raceid]; i != count; ++i) {
        new const
            vehicleid = gRacerVehicleID[gRaceParticipants[raceid][i]]
        ;

        if (vehicleid == INVALID_VEHICLE_ID) {
            continue;
        }

        ChangeVehicleColours(vehicleid, colour1, colour2);
    }

    return true;
}

// Two things to say, so this one still fills in references. RACE_COLOUR_RANDOM
// comes back for a race that paints its cars at random, which is what it was
// set up with unless somebody said otherwise.
stock bool:GetRaceVehicleColours(Race:raceid, &colour1, &colour2) {
    if (!IsValidRaceInternal(raceid)) {
        colour1 = RACE_COLOUR_RANDOM;
        colour2 = RACE_COLOUR_RANDOM;

        return false;
    }

    colour1 = gRaceData[raceid][E_RACE_COLOUR_1];
    colour2 = gRaceData[raceid][E_RACE_COLOUR_2];

    return true;
}

stock bool:SetRaceName(Race:raceid, const name[]) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    if (name[0] == EOS) {
        return false;
    }

    strpack(gRaceData[raceid][E_RACE_NAME], name, (MAX_RACE_NAME + 1) char);

    return true;
}

stock bool:GetRaceName(Race:raceid, name[], size = sizeof (name)) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    strunpack(name, gRaceData[raceid][E_RACE_NAME], size);

    return true;
}

// Changing the map moves every start line, so it is only allowed while the room
// is empty: a racer already sitting on grid slot four of the old map would be
// racing checkpoints that belong to a course they were never placed on.
stock bool:SetRaceRecord(Race:raceid, RaceRecord:recordid) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    if (!IsValidRaceRecordInternal(recordid)) {
        return false;
    }

    if (gRaceRacerCount[raceid]) {
        return false;
    }

    gRaceData[raceid][E_RACE_RECORD_ID] = recordid;

    ResetRaceStartSlotsInternal(raceid);

    return true;
}

// Answers the map, not whether there is one. A race with no map is not a thing
// this library lets exist, and a caller that wants to check anyway compares
// against INVALID_RACE_RECORD_ID, which is what it would have had to do with
// the reference this used to fill in.
stock RaceRecord:GetRaceRecord(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return INVALID_RACE_RECORD_ID;
    }

    return gRaceData[raceid][E_RACE_RECORD_ID];
}

stock bool:SetRaceLaps(Race:raceid, laps) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    if (!(MIN_RACE_LAPS <= laps <= MAX_RACE_LAPS)) {
        return false;
    }

    gRaceData[raceid][E_RACE_LAPS] = laps;

    return true;
}

stock GetRaceLaps(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return 0;
    }

    return gRaceData[raceid][E_RACE_LAPS];
}

stock bool:SetRaceMaxRacers(Race:raceid, maxRacers) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    if (!(MIN_RACERS <= maxRacers <= MAX_RACERS)) {
        return false;
    }

    // Lowering it below the number already on the grid would leave racers in a
    // room that says it cannot hold them.
    if (maxRacers < gRaceRacerCount[raceid]) {
        return false;
    }

    gRaceData[raceid][E_RACE_MAX_RACERS] = maxRacers;

    return true;
}

stock GetRaceMaxRacers(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return 0;
    }

    return gRaceData[raceid][E_RACE_MAX_RACERS];
}

stock bool:SetRaceCountdown(Race:raceid, countDown) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    if (!(MIN_RACE_COUNT_DOWN <= countDown <= MAX_RACE_COUNT_DOWN)) {
        return false;
    }

    gRaceData[raceid][E_RACE_COUNT_DOWN] = countDown;

    return true;
}

stock GetRaceCountdown(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return 0;
    }

    return gRaceData[raceid][E_RACE_COUNT_DOWN];
}

stock GetRaceCountdownLeft(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return 0;
    }

    return gRaceData[raceid][E_RACE_COUNT_DOWN_LEFT];
}

stock bool:SetRaceVirtualWorld(Race:raceid, worldid) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    gRaceData[raceid][E_RACE_WORLD_ID] = worldid;

    return true;
}

stock GetRaceVirtualWorld(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return 0;
    }

    return gRaceData[raceid][E_RACE_WORLD_ID];
}

stock bool:SetRaceInterior(Race:raceid, interiorid) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    gRaceData[raceid][E_RACE_INTERIOR_ID] = interiorid;

    return true;
}

stock GetRaceInterior(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return 0;
    }

    return gRaceData[raceid][E_RACE_INTERIOR_ID];
}

stock bool:ToggleRaceFlags(Race:raceid, flags) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    gRaceData[raceid][E_RACE_FLAGS] ^= (flags & ~RACE_FLAGS_INTERNAL);

    return true;
}

stock bool:HasRaceFlags(Race:raceid, flags) {
    if (!IsValidRaceInternal(raceid)) {
        return false;
    }

    return ((gRaceData[raceid][E_RACE_FLAGS] & flags) == flags);
}

stock GetRaceRacerCount(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return 0;
    }

    return gRaceRacerCount[raceid];
}

stock GetRaceFreeSlots(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return 0;
    }

    return GetRaceCapacityInternal(raceid) - gRaceRacerCount[raceid];
}

// Walking a race is GetRaceFirstRacer followed by GetNextRacer until it answers
// INVALID_PLAYER_ID, and it touches only the players who are in it.
// One racer by their place in the race, counted from zero. This is the one to
// walk with when the body of the loop can take somebody out of the race: count
// downwards, and a racer leaving moves the last one into a slot that has
// already been passed.
stock GetRaceRacer(Race:raceid, index) {
    if (!IsValidRaceInternal(raceid)) {
        return INVALID_PLAYER_ID;
    }

    if (!(0 <= index < gRaceRacerCount[raceid])) {
        return INVALID_PLAYER_ID;
    }

    return gRaceParticipants[raceid][index];
}

// Everybody in the race, copied out in one go. The copy is the point: the race
// can be emptied while the script walks what it was handed, which is what a
// script does when it puts everybody back on their feet at the end of a run.
stock GetRaceRacers(Race:raceid, racers[], size = sizeof (racers)) {
    if (!IsValidRaceInternal(raceid)) {
        return 0;
    }

    new
        count = gRaceRacerCount[raceid]
    ;

    if (count > size) {
        count = size;
    }

    for (new i; i != count; ++i) {
        racers[i] = gRaceParticipants[raceid][i];
    }

    return count;
}

stock GetRaceFirstRacer(Race:raceid) {
    if (!IsValidRaceInternal(raceid)) {
        return INVALID_PLAYER_ID;
    }

    if (!gRaceRacerCount[raceid]) {
        return INVALID_PLAYER_ID;
    }

    return gRaceParticipants[raceid][0];
}

stock GetNextRacer(playerid) {
    if (!IsRacerInternal(playerid)) {
        return INVALID_PLAYER_ID;
    }

    new const
        Race:raceid = gRacerRaceID[playerid],
        index = gRacerIndex[playerid] + 1
    ;

    if (index >= gRaceRacerCount[raceid]) {
        return INVALID_PLAYER_ID;
    }

    return gRaceParticipants[raceid][index];
}

/**
 * # Player
 */

stock RACE_JOIN:PutPlayerInRace(playerid, Race:raceid) {
    if (!IsPlayerConnected(playerid)) {
        return RACE_JOIN_INVALID_PLAYER;
    }

    if (!IsValidRaceInternal(raceid)) {
        return RACE_JOIN_INVALID_RACE;
    }

    if (IsRacerInternal(playerid)) {
        return RACE_JOIN_ALREADY_IN_RACE;
    }

    if (IsRaceStarted(raceid)) {
        return RACE_JOIN_STARTED;
    }

    new const
        RaceRecord:recordid = gRaceData[raceid][E_RACE_RECORD_ID]
    ;

    if (!IsValidRaceRecordInternal(recordid)) {
        return RACE_JOIN_NO_RECORD;
    }

    new const
        index = ClaimRaceStartSlotInternal(raceid)
    ;

    if (index == -1) {
        return RACE_JOIN_FULL;
    }

    new const
        vehicleid = CreateVehicle(gRaceData[raceid][E_RACE_MODEL_ID],
            gRaceRecordStarts[recordid][index][0],
            gRaceRecordStarts[recordid][index][1],
            gRaceRecordStarts[recordid][index][2],
            gRaceRecordStarts[recordid][index][3],
            gRaceData[raceid][E_RACE_COLOUR_1],
            gRaceData[raceid][E_RACE_COLOUR_2],
            -1
        )
    ;

    if (vehicleid == INVALID_VEHICLE_ID) {
        ReleaseRaceStartSlotInternal(raceid, index);

        return RACE_JOIN_FULL;
    }

    if (IsRaceNitroEnabled(raceid)) {
        AddVehicleComponent(vehicleid, RACE_NITRO_COMPONENT);
    }

    if (IsRaceGhostVehicleEnabled(raceid)) {
        DisableRemoteVehicleCollisions(playerid, true);
    }

    GetPlayerPos(playerid, gRacerData[playerid][E_RACER_LAST_X], gRacerData[playerid][E_RACER_LAST_Y], gRacerData[playerid][E_RACER_LAST_Z]);
    GetPlayerFacingAngle(playerid, gRacerData[playerid][E_RACER_LAST_A]);

    gRacerData[playerid][E_RACER_LAST_WORLD_ID]    = GetPlayerVirtualWorld(playerid);
    gRacerData[playerid][E_RACER_LAST_INTERIOR_ID] = GetPlayerInterior(playerid);

    gRacerData[playerid][E_RACER_CP_COUNT]         = 0;
    gRacerData[playerid][E_RACER_LAP_COUNT]        = 0;
    gRacerData[playerid][E_RACER_START_INDEX]      = index;
    gRacerData[playerid][E_RACER_POSITION]         = 0;
    gRacerData[playerid][E_RACER_FINISHED]         = false;

    StepRacerSessionInternal(playerid);

    gRacerRaceID[playerid]                         = raceid;
    gRacerVehicleID[playerid]                      = vehicleid;

    SetVehicleVirtualWorld(vehicleid, gRaceData[raceid][E_RACE_WORLD_ID]);
    LinkVehicleToInterior(vehicleid, gRaceData[raceid][E_RACE_INTERIOR_ID]);

    SetPlayerVirtualWorld(playerid, gRaceData[raceid][E_RACE_WORLD_ID]);
    SetPlayerInterior(playerid, gRaceData[raceid][E_RACE_INTERIOR_ID]);

    PutPlayerInVehicle(playerid, vehicleid, 0);
    TogglePlayerControllable(playerid, false);

    AddRacerInternal(raceid, playerid);

    CallLocalFunction("OnPlayerJoinRace", "ii", playerid, _:raceid);

    return RACE_JOIN_OK;
}

stock bool:RemovePlayerFromRace(playerid, Float:x = 0.0, Float:y = 0.0, Float:z = 0.0, Float:a = 0.0, worldid = 0, interiorid = 0, bool:spawnLastPos = true) {
    if (!IsRacerInternal(playerid)) {
        return false;
    }

    new const
        Race:raceid = gRacerRaceID[playerid]
    ;

    if (spawnLastPos) {
        x = gRacerData[playerid][E_RACER_LAST_X];
        y = gRacerData[playerid][E_RACER_LAST_Y];
        z = gRacerData[playerid][E_RACER_LAST_Z];
        a = gRacerData[playerid][E_RACER_LAST_A];
        worldid = gRacerData[playerid][E_RACER_LAST_WORLD_ID];
        interiorid = gRacerData[playerid][E_RACER_LAST_INTERIOR_ID];
    }

    ClearRacerInternal(playerid, raceid);

    // Somebody who has already gone is taken off the race's books and not put
    // anywhere on the ground: OnPlayerDisconnect comes through here too.
    if (IsPlayerConnected(playerid)) {
        ClearAnimations(playerid);
        SetPlayerPos(playerid, x, y, z);
        SetPlayerFacingAngle(playerid, a);
        SetPlayerVirtualWorld(playerid, worldid);
        SetPlayerInterior(playerid, interiorid);
        TogglePlayerControllable(playerid, true);
        DisableRemoteVehicleCollisions(playerid, false);
    }

    CallLocalFunction("OnPlayerLeaveRace", "ii", playerid, _:raceid);

    // A countdown with nobody left waiting on it is stopped rather than left to
    // go off in an empty room and flag a race as running that nobody is in.
    if (!gRaceRacerCount[raceid] && IsValidRaceInternal(raceid)) {
        StopRaceTimerInternal(raceid);

        gRaceSession[raceid] = ++gRaceSessionCounter;
        gRaceData[raceid][E_RACE_COUNT_DOWN_LEFT] = 0;
    }

    // The field is one smaller than it was, and the racer who just left may
    // have been the one everybody else was waiting on.
    CheckRaceOverInternal(raceid);

    return true;
}

stock bool:IsPlayerInAnyRace(playerid) {
    return IsRacerInternal(playerid);
}

// INVALID_RACE_ID for somebody who is not racing, which IsPlayerInAnyRace also
// answers and this one says in the same breath as which race it is.
stock Race:GetPlayerRace(playerid) {
    if (!IsRacerInternal(playerid)) {
        return INVALID_RACE_ID;
    }

    return gRacerRaceID[playerid];
}

stock GetPlayerRaceVehicle(playerid) {
    if (!IsRacerInternal(playerid)) {
        return INVALID_VEHICLE_ID;
    }

    return gRacerVehicleID[playerid];
}

stock GetPlayerRaceCP(playerid) {
    if (!IsRacerInternal(playerid)) {
        return -1;
    }

    return gRacerData[playerid][E_RACER_CP_COUNT];
}

// Counted from one, because a racer on their first lap is on lap one.
stock GetPlayerRaceLap(playerid) {
    if (!IsRacerInternal(playerid)) {
        return 0;
    }

    return gRacerData[playerid][E_RACER_LAP_COUNT] + 1;
}

stock GetPlayerRacePosition(playerid) {
    if (!IsRacerInternal(playerid)) {
        return 0;
    }

    return gRacerData[playerid][E_RACER_POSITION];
}

// Subtracted rather than counted down, so a tick count that has wrapped round
// still answers correctly: the difference of two ticks is right even when the
// later one is the smaller number.
stock GetPlayerRaceReturnTimeLeft(playerid) {
    if (!IsRacerInternal(playerid)) {
        return 0;
    }

    if (!gRacerData[playerid][E_RACER_RETURN_DEADLINE]) {
        return 0;
    }

    new const
        left = gRacerData[playerid][E_RACER_RETURN_DEADLINE] - GetTickCount()
    ;

    if (left < 0) {
        return 0;
    }

    return left;
}

stock bool:HasPlayerFinishedRace(playerid) {
    if (!IsRacerInternal(playerid)) {
        return false;
    }

    return gRacerData[playerid][E_RACER_FINISHED];
}

/**
 * # Timers
 */

// One timer doing both jobs: it counts the second down for the script to draw
// and it is the alarm that goes off at the end. A separate one shot for the
// deadline would be a second timer saying the same thing.
forward @Race_ReturnTick(playerid, session);

public @Race_ReturnTick(playerid, session) {
    if (!IsRacerSessionValidInternal(playerid, session)) {
        return 0;
    }

    new const
        Race:raceid = gRacerRaceID[playerid],
        left = GetPlayerRaceReturnTimeLeft(playerid)
    ;

    if (left > 0) {
        // Rounded up, so the last second on the clock reads one and not zero.
        CallLocalFunction("OnPlayerRaceReturnUpdate", "iii", playerid, _:raceid, (left + 999) / 1000);

        return 1;
    }

    StopRacerReturnTimerInternal(playerid);

    // The script gets the last word. Saying zero leaves them in the race, and
    // the clock is not wound again: they walk with the library's blessing until
    // they get back in.
    if (!CallLocalFunction("OnPlayerRaceReturnTimeout", "ii", playerid, _:raceid)) {
        return 1;
    }

    RemovePlayerFromRace(playerid);

    return 1;
}

forward @Race_Countdown(Race:raceid, session);

// raceid arrives with no tag on it and cannot arrive any other way: SetTimerEx
// carries it through an "i". Every use of it below puts the tag back on at the
// point of use, which is the only place the compiler will take it.
public @Race_Countdown(Race:raceid, session) {
    if (!IsRaceSessionValidInternal(raceid, session)) {
        return 0;
    }

    // The countdown belongs to the race and is stepped once a tick. It used to
    // be stepped inside the walk over the racers, so it ran as many times a
    // second as there were people waiting on it, and only one of them was ever
    // told that it had reached zero.
    new const
        countdown = --gRaceData[raceid][E_RACE_COUNT_DOWN_LEFT]
    ;

    if (countdown > 0) {
        CallLocalFunction("OnRaceCountdownUpdate", "iiai", _:raceid, countdown, gRaceParticipants[raceid], gRaceRacerCount[raceid]);

        return 1;
    }

    BeginRaceInternal(raceid);

    return 1;
}

/**
 * # Calls
 */

hook OnPlayerConnect(playerid) {
    static const
        RACER_DATA_NONE[E_RACER_DATA]
    ;

    gRacerData[playerid] = RACER_DATA_NONE;
    gRacerData[playerid][E_RACER_START_INDEX] = -1;

    gRacerRaceID[playerid] = INVALID_RACE_ID;
    gRacerVehicleID[playerid] = INVALID_VEHICLE_ID;
    gRacerIndex[playerid] = -1;

    return 0;
}

// Without this the race kept a player who was no longer there: their grid slot
// never came back, their car stayed on the map, and a race waiting on the last
// finisher waited for somebody who had closed the game.
hook OnPlayerDisconnect(playerid, reason) {
    if (IsRacerInternal(playerid)) {
        RemovePlayerFromRace(playerid);
    }

    return 0;
}

// Dying puts a racer on the pavement outside their car with a checkpoint still
// in front of them, which is not racing. They come out of the race here, where
// they stand rather than back where they joined from, because the spawn is
// about to move them anyway.
hook OnPlayerDeath(playerid, killerid, WEAPON:reason) {
    if (IsRacerInternal(playerid)) {
        RemovePlayerFromRace(playerid, .spawnLastPos = false);
    }

    return 0;
}

// Getting out of the car and back into it. The clock is only for somebody who
// is actually racing: before the lights they are frozen and cannot get out, and
// once they have finished it no longer matters where they are.
hook OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate) {
    #if RACE_RETURN_TIME > 0
        if (!IsRacerInternal(playerid)) {
            return 0;
        }

        new const
            Race:raceid = gRacerRaceID[playerid]
        ;

        if (!IsRaceStarted(raceid)) {
            return 0;
        }

        if (gRacerData[playerid][E_RACER_FINISHED]) {
            return 0;
        }

        // Back in the driving seat of the car this race gave them. Another car is
        // somebody else's, and riding as a passenger is not driving.
        if (newstate == PLAYER_STATE_DRIVER) {
            if (GetPlayerVehicleID(playerid) != gRacerVehicleID[playerid]) {
                return 0;
            }

            if (!gRacerReturnTimer[playerid]) {
                return 0;
            }

            StepRacerSessionInternal(playerid);

            CallLocalFunction("OnPlayerRaceVehicleReturn", "ii", playerid, _:raceid);

            return 0;
        }

        // Out of the seat they were racing from, however they came to leave it.
        if (oldstate != PLAYER_STATE_DRIVER) {
            return 0;
        }

        StepRacerSessionInternal(playerid);

        gRacerData[playerid][E_RACER_RETURN_DEADLINE] = GetTickCount() + RACE_RETURN_TIME;
        gRacerReturnTimer[playerid] = SetTimerEx("@Race_ReturnTick", 1000, true, "ii", playerid, gRacerSession[playerid]);

        CallLocalFunction("OnPlayerRaceVehicleExit", "iii", playerid, _:raceid, RACE_RETURN_TIME);

        // The first number goes up now rather than a second from now, the same way
        // the starting countdown shows its first one at once: a repeating timer's
        // first tick is one interval away, and that interval would be a second of
        // nothing on screen.
        CallLocalFunction("OnPlayerRaceReturnUpdate", "iii", playerid, _:raceid, (RACE_RETURN_TIME + 999) / 1000);
    #else
        #pragma unused newstate, oldstate
    #endif

    return 0;
}

hook OnPlayerEnterRaceCheckpoint(playerid) {
    DoEnterRaceCPInternal(playerid);

    return 0;
}
