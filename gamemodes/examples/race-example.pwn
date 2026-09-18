#define MAX_PLAYERS (8)

// pp-menu and pp-dialogs both hand their answer back through a PawnPlus task,
// which is read with await inside a function that yields. Both keywords have to
// be asked for before the include that pulls PawnPlus in, and pp-menu is that
// include: it brings pp-hooks, and pp-hooks brings PawnPlus.
#define PP_SYNTAX_AWAIT
#define PP_SYNTAX_YIELD

#include <open.mp>
#include <tick-difference>
#include <Pawn.CMD>
#include <sscanf2>

// Before extended-text-styles, and that order is the point: it is what lets the
// styles break a sentence to fit on their own, instead of the sentence needing
// a ~n~ measured and placed by hand. Without it everything still works and the
// long ones simply run off the edge.
#include <td-string-width>
#include <extended-text-styles>

#include <pp-menu>
#include <pp-dialogs>

#include <race>

/**
 * # Header
 */

#define SPAWN_X                         (2093.9287)
#define SPAWN_Y                         (2213.6853)
#define SPAWN_Z                         (10.8203)

// The two ends of the vehicle model range. Written down here because the number
// goes into a prompt the player reads and into the check the answer is held to,
// and those two have to be the same number.
#define MIN_RACE_VEHICLE_MODEL          (400)
#define MAX_RACE_VEHICLE_MODEL          (611)

// How long between two checkpoints being dropped while a course is being drawn.
// Without it one click puts down as many as the key repeats.
#define RACE_CP_COOLDOWN                (1000)

#define MENU_X                          (20.0)
#define MENU_Y                          (130.0)
#define MENU_WIDTH                      (250.0)

#define GAME_TEXT_TIME                  (1000)
#define GAME_TEXT_TIME_LONG             (3000)

// The game's own big centred style, for the four things that are events and not
// sentences: the lights, the lap and the finish. GameTextForPlayer draws these
// the way the story mode draws them, and nothing here improves on that.
#define GAME_TEXT_STYLE                 (3)

// And extended-text-styles for the two things the native style cannot do.
//
// STUNT carries what is being said to the player in words, which needs to sit
// still while an event flashes over it. POPUP is the boxed panel on the left,
// for the running count. Keeping those two off style 3 is what stops a
// checkpoint counter from wiping a sentence the player is still reading.
#define ANNOUNCE_STYLE                  (GAME_TEXT_STYLE_STUNT)
#define STATUS_STYLE                    (GAME_TEXT_STYLE_POPUP)

// Where the race setup menu goes when a row is picked. The order is the order
// the rows are added in, so the two lists are read side by side.
enum {
    RACE_MENU_NAME,
    RACE_MENU_MAP,
    RACE_MENU_VEHICLE,
    RACE_MENU_COLOUR_1,
    RACE_MENU_COLOUR_2,
    RACE_MENU_LAPS,
    RACE_MENU_MAX_RACERS,
    RACE_MENU_COUNTDOWN,
    RACE_MENU_PRIVATE,
    RACE_MENU_GHOST,
    RACE_MENU_NITRO,
    RACE_MENU_AIR,
    RACE_MENU_START
};

// The four settings that are a number typed into a box differ only in what they
// are called, what they ask and what they will accept, so they are one dialog
// reading one table rather than four near copies of each other.
enum {
    RACE_VALUE_VEHICLE,
    RACE_VALUE_COLOUR_1,
    RACE_VALUE_COLOUR_2,
    RACE_VALUE_LAPS,
    RACE_VALUE_MAX_RACERS,
    RACE_VALUE_COUNTDOWN
};

static enum _:E_RACE_VALUE_DATA {
    E_RACE_VALUE_TITLE[32],
    E_RACE_VALUE_PROMPT[96],
    E_RACE_VALUE_MIN,
    E_RACE_VALUE_MAX
};

static const gRaceValueData[][E_RACE_VALUE_DATA] = {
    { "Race Vehicle",    "Which vehicle model does everybody drive?", MIN_RACE_VEHICLE_MODEL, MAX_RACE_VEHICLE_MODEL },
    { "Race Colour 1",   "Primary colour. -1 leaves it to chance.",   RACE_COLOUR_RANDOM,     MAX_RACE_COLOUR        },
    { "Race Colour 2",   "Secondary colour. -1 leaves it to chance.", RACE_COLOUR_RANDOM,     MAX_RACE_COLOUR        },
    { "Race Laps",       "How many laps of the course?",              MIN_RACE_LAPS,          MAX_RACE_LAPS          },
    { "Race Max Racers", "How many cars on the grid?",                MIN_RACERS,             MAX_RACERS             },
    { "Race Countdown",  "How many seconds on the lights?",           MIN_RACE_COUNT_DOWN,    MAX_RACE_COUNT_DOWN    }
};

// A course that is already there when the server comes up, so /editrace and
// /joinrace have something to work with without anybody drawing a map first.
//
// The coordinates are a plain loop of Las Venturas street laid out by hand and
// not driven, so they are somewhere to start rather than a good race. Draw a
// real one with /recordrace and pick it off the Map row.
#define DEMO_COURSE_NAME                "Practice Loop"
#define DEMO_RACE_NAME                  "Practice"
#define DEMO_VEHICLE_MODEL              (411)

static const Float:gDemoStarts[][4] = {
    { 2085.0, 2200.0, 10.8, 0.0 },
    { 2090.0, 2200.0, 10.8, 0.0 },
    { 2095.0, 2200.0, 10.8, 0.0 },
    { 2100.0, 2200.0, 10.8, 0.0 }
};

static const Float:gDemoCheckpoints[][3] = {
    { 2092.0, 2300.0, 10.8 },
    { 2092.0, 2400.0, 10.8 },
    { 2000.0, 2450.0, 10.8 },
    { 1900.0, 2450.0, 10.8 },
    { 1850.0, 2350.0, 10.8 },
    { 1850.0, 2250.0, 10.8 },
    { 1900.0, 2180.0, 10.8 },
    { 2000.0, 2170.0, 10.8 },
    { 2092.0, 2190.0, 10.8 }
};

static
    RaceRecord:gDemoRecordID = INVALID_RACE_RECORD_ID
;

// The cars standing on the grid while a course is being drawn, so that whoever
// is drawing it can see where they put the start line. They are not the cars the
// race runs with: race.inc makes those itself when a player joins.
static
    gRaceRecordVehicleID[MAX_RACE_RECORDS][MAX_RACERS] = { { INVALID_VEHICLE_ID, ... }, ... }
;

static
    RaceRecord:gEditRaceRecordID[MAX_PLAYERS] = { INVALID_RACE_RECORD_ID, ... },
    bool:gIsCreationRaceCPEnabled[MAX_PLAYERS char],
    gCreationCPCooldown[MAX_PLAYERS]
;

static
    Race:gOwnerRaceID[MAX_PLAYERS] = { INVALID_RACE_ID, ... }
;

// Which race or map each row of a list stood for. The lists are built out of
// whatever exists at the moment they are shown, so the row number the player
// clicks means nothing on its own.
static
    Race:gListedRaceID[MAX_PLAYERS][_:MAX_RACES],
    RaceRecord:gListedRecordID[MAX_PLAYERS][_:MAX_RACE_RECORDS]
;

main() {}

/**
 * # Helpers
 */

static GetName(playerid, name[MAX_PLAYER_NAME + 1]) {
    name[0] = EOS;

    if (!IsPlayerConnected(playerid)) {
        strcopy(name, "Nobody");

        return;
    }

    GetPlayerName(playerid, name, MAX_PLAYER_NAME + 1);
}

// Takes the preview cars off the map. Called both when a course is saved and
// when it is thrown away, because either way they have done their job.
static ClearRecordVehicles(RaceRecord:recordid) {
    for (new i, size = GetRaceRecordRacerStartCount(recordid); i != size; ++i) {
        if (gRaceRecordVehicleID[recordid][i] == INVALID_VEHICLE_ID) {
            continue;
        }

        DestroyVehicle(gRaceRecordVehicleID[recordid][i]);
        gRaceRecordVehicleID[recordid][i] = INVALID_VEHICLE_ID;
    }
}

// What a colour row reads. RACE_COLOUR_RANDOM is not a colour, it is the
// absence of a choice, and it is written as such.
static ColourName(colour, name[MAX_MENU_ITEM_LENGTH]) {
    if (colour == RACE_COLOUR_RANDOM) {
        strcopy(name, "Random", MAX_MENU_ITEM_LENGTH);

        return;
    }

    Format(name, MAX_MENU_ITEM_LENGTH, "%i", colour);
}

// A row that is turned rather than pressed.
//
// A selector always opens on the option that went on first and there is no way
// to tell it otherwise, so the one the race is already set to is the one put on
// first. That is why the order depends on the flag rather than being fixed.
//
// Which of the two is showing is then never worked out from the option index:
// with exactly two options every move is a flip, so the callback below toggles
// the flag and the row and the race stay in step whichever way round they went
// on.
static AddSwitchRow(playerid, item, const label[], const off[], const on[], bool:enabled) {
    AddListMenuItem(playerid, 0, label);

    if (enabled) {
        AddListMenuItemSelector(playerid, item, on);
        AddListMenuItemSelector(playerid, item, off);

        return;
    }

    AddListMenuItemSelector(playerid, item, off);
    AddListMenuItemSelector(playerid, item, on);
}

static BuildDemoCourse() {
    gDemoRecordID = RecordRace(DEMO_COURSE_NAME);

    if (!IsValidRaceRecord(gDemoRecordID)) {
        return;
    }

    for (new i; i != sizeof (gDemoStarts); ++i) {
        CreateRaceRecordRacerStart(gDemoRecordID, gDemoStarts[i][0], gDemoStarts[i][1], gDemoStarts[i][2], gDemoStarts[i][3]);
    }

    for (new i; i != sizeof (gDemoCheckpoints); ++i) {
        CreateRaceRecordCP(gDemoRecordID, gDemoCheckpoints[i][0], gDemoCheckpoints[i][1], gDemoCheckpoints[i][2]);
    }

    printf("[race] built '%s': %d starts, %d checkpoints",
        DEMO_COURSE_NAME,
        GetRaceRecordRacerStartCount(gDemoRecordID),
        GetRaceRecordCPCount(gDemoRecordID));
}

// One race per player, standing and ready, so /editrace works the moment they
// spawn. Made on spawn rather than on connect because CreateRace wants somebody
// connected to own it, and it is theirs: OnPlayerDisconnect takes it away
// again.
static GiveStarterRace(playerid) {
    if (IsValidRace(gOwnerRaceID[playerid])) {
        return;
    }

    if (!IsValidRaceRecord(gDemoRecordID)) {
        return;
    }

    new const
        Race:raceid = CreateRace(playerid, DEMO_VEHICLE_MODEL, DEMO_RACE_NAME, gDemoRecordID)
    ;

    if (raceid == INVALID_RACE_ID) {
        return;
    }

    gOwnerRaceID[playerid] = raceid;

    SendClientMessage(playerid, -1, "{00FF00}INFO: {FFFFFF}A race on '%s' is ready and yours. {FFFF00}/editrace{FFFFFF} to set it up, {FFFF00}/joinrace{FFFFFF} to get on the grid.", DEMO_COURSE_NAME);
}

static StopRecording(playerid) {
    gEditRaceRecordID[playerid] = INVALID_RACE_RECORD_ID;
    gIsCreationRaceCPEnabled{playerid} = false;
}

// Everything /startrace has to be able to say no to, in one place, because the
// menu has a Start row that has to say the same things.
static bool:TryStartRace(playerid, Race:raceid) {
    if (!IsValidRace(raceid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}You didn't create a race.");

        return false;
    }

    if (IsRaceStarted(raceid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}That race is already running.");

        return false;
    }

    if (GetRaceRacerCount(raceid) < MIN_RACERS) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}Nobody has joined yet (they use /joinrace).");

        return false;
    }

    if (!StartRace(raceid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}That race could not be started.");

        return false;
    }

    return true;
}

/**
 * # Race Setup Menu
 */

static ShowRaceEditMenu(playerid) {
    yield 1;

    new const
        Race:raceid = gOwnerRaceID[playerid]
    ;

    if (!IsValidRace(raceid)) {
        return;
    }

    // A race that is already running has nothing left to set: every switch on
    // this menu moves a start line or a car that is already out on the course.
    if (IsRaceStarted(raceid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}That race is running, there is nothing left to set.");

        return;
    }

    new const
        RaceRecord:recordid = GetRaceRecord(raceid)
    ;

    new
        raceName[MAX_RACE_NAME + 1],
        recordName[MAX_RACE_RECORD_NAME + 1],
        buffer[MAX_MENU_ITEM_LENGTH]
    ;

    GetRaceName(raceid, raceName);
    GetRaceRecordName(recordid, recordName);

    AddListMenuItem(playerid, 0, "Name");
    AddListMenuItem(playerid, 1, raceName);

    AddListMenuItem(playerid, 0, "Map");
    AddListMenuItem(playerid, 1, recordName);

    Format(buffer, sizeof (buffer), "%i", GetRaceVehicleModel(raceid));
    AddListMenuItem(playerid, 0, "Vehicle");
    AddListMenuItem(playerid, 1, buffer);

    new
        colour1,
        colour2
    ;

    GetRaceVehicleColours(raceid, colour1, colour2);

    // A colour nobody chose is shown as the word rather than as minus one,
    // because minus one is not a colour and the row is read, not typed.
    ColourName(colour1, buffer);
    AddListMenuItem(playerid, 0, "Colour 1");
    AddListMenuItem(playerid, 1, buffer);

    ColourName(colour2, buffer);
    AddListMenuItem(playerid, 0, "Colour 2");
    AddListMenuItem(playerid, 1, buffer);

    Format(buffer, sizeof (buffer), "%i", GetRaceLaps(raceid));
    AddListMenuItem(playerid, 0, "Laps");
    AddListMenuItem(playerid, 1, buffer);

    // What the room was set to and what the map can actually seat, because the
    // smaller of the two is the one that decides who gets in.
    Format(buffer, sizeof (buffer), "%i (map: %i)", GetRaceMaxRacers(raceid), GetRaceRecordRacerStartCount(recordid));
    AddListMenuItem(playerid, 0, "Max Racers");
    AddListMenuItem(playerid, 1, buffer);

    Format(buffer, sizeof (buffer), "%i sec", GetRaceCountdown(raceid));
    AddListMenuItem(playerid, 0, "Countdown");
    AddListMenuItem(playerid, 1, buffer);

    // The four switches are selectors rather than rows to press: left and right
    // turn them, and OnPlayerListMenuOptionChange below writes the answer
    // straight into the race. A row that is pressed has to close the menu and
    // build it again to show the new value; this one just changes in place.
    //
    // The options go on in the order the flag reads as off then on, so the
    // option index is the flag: 0 is off, 1 is on.
    AddSwitchRow(playerid, RACE_MENU_PRIVATE, "Private", "No", "Yes", IsRacePrivated(raceid));
    AddSwitchRow(playerid, RACE_MENU_GHOST, "Ghost Cars", "No", "Yes", IsRaceGhostVehicleEnabled(raceid));
    AddSwitchRow(playerid, RACE_MENU_NITRO, "Nitro", "No", "Yes", IsRaceNitroEnabled(raceid));
    AddSwitchRow(playerid, RACE_MENU_AIR, "Course", "Ground", "Air", IsRaceArial(raceid));

    Format(buffer, sizeof (buffer), "%i/%i", GetRaceRacerCount(raceid), GetRaceMaxRacers(raceid));
    AddListMenuItem(playerid, 0, "Start Race");
    AddListMenuItem(playerid, 1, buffer);

    new const
        Task:t = ShowAsyncListMenu(playerid, "Race Setup", MENU_X, MENU_Y, MENU_WIDTH)
    ;

    if (!t) {
        return;
    }

    new
        responses[E_ASYNC_MENU_DATA]
    ;

    await_arr(responses) t;

    if (!responses[E_ASYNC_MENU_RESPONSE]) {
        return;
    }

    // The room can go while the menu is open: the owner disconnecting takes it
    // with them, and so does /destroyrace from another session.
    if (!IsValidRace(raceid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}That race is no longer there.");

        return;
    }

    switch (responses[E_ASYNC_MENU_LISTITEM]) {
        case RACE_MENU_NAME: {
            ShowRaceNameDialog(playerid);
        }

        case RACE_MENU_MAP: {
            ShowRaceMapMenu(playerid);
        }

        case RACE_MENU_VEHICLE: {
            ShowRaceValueDialog(playerid, RACE_VALUE_VEHICLE);
        }

        case RACE_MENU_COLOUR_1: {
            ShowRaceValueDialog(playerid, RACE_VALUE_COLOUR_1);
        }

        case RACE_MENU_COLOUR_2: {
            ShowRaceValueDialog(playerid, RACE_VALUE_COLOUR_2);
        }

        case RACE_MENU_LAPS: {
            ShowRaceValueDialog(playerid, RACE_VALUE_LAPS);
        }

        case RACE_MENU_MAX_RACERS: {
            ShowRaceValueDialog(playerid, RACE_VALUE_MAX_RACERS);
        }

        case RACE_MENU_COUNTDOWN: {
            ShowRaceValueDialog(playerid, RACE_VALUE_COUNTDOWN);
        }

        // The four switch rows are turned with left and right, not pressed, so
        // pressing one only puts the menu back up where it was.
        case RACE_MENU_PRIVATE, RACE_MENU_GHOST, RACE_MENU_NITRO, RACE_MENU_AIR: {
            ShowRaceEditMenu(playerid);
        }

        case RACE_MENU_START: {
            if (!TryStartRace(playerid, raceid)) {
                ShowRaceEditMenu(playerid);
            }
        }
    }
}

static ShowRaceNameDialog(playerid) {
    yield 1;

    new const
        Race:raceid = gOwnerRaceID[playerid]
    ;

    if (!IsValidRace(raceid)) {
        return;
    }

    new const
        Task:t = ShowPlayerAsyncDialog(playerid, DIALOG_STYLE_INPUT, "Race Name",
            "{FFFFFF}What is this race called?\n\nBetween "#MIN_RACE_NAME" and "#MAX_RACE_NAME" characters.",
            "Save", "Back"
        )
    ;

    if (!t) {
        return;
    }

    new
        responses[E_ASYNC_DIALOG]
    ;

    await_arr(responses) t;

    if (responses[E_ASYNC_DIALOG_RESPONSE]) {
        if (!(MIN_RACE_NAME <= strlen(responses[E_ASYNC_DIALOG_INPUTTEXT]) <= MAX_RACE_NAME)) {
            SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}A name is between "#MIN_RACE_NAME" and "#MAX_RACE_NAME" characters.");
        } else {
            SetRaceName(raceid, responses[E_ASYNC_DIALOG_INPUTTEXT]);
        }
    }

    ShowRaceEditMenu(playerid);
}

static ShowRaceValueDialog(playerid, field) {
    yield 1;

    new const
        Race:raceid = gOwnerRaceID[playerid]
    ;

    if (!IsValidRace(raceid)) {
        return;
    }

    new const
        low = gRaceValueData[field][E_RACE_VALUE_MIN],
        high = gRaceValueData[field][E_RACE_VALUE_MAX]
    ;

    new
        body[192]
    ;

    Format(body, sizeof (body), "{FFFFFF}%s\n\nA number between %i and %i.",
        gRaceValueData[field][E_RACE_VALUE_PROMPT], low, high);

    new const
        Task:t = ShowPlayerAsyncDialog(playerid, DIALOG_STYLE_INPUT,
            gRaceValueData[field][E_RACE_VALUE_TITLE], body, "Save", "Back")
    ;

    if (!t) {
        return;
    }

    new
        responses[E_ASYNC_DIALOG]
    ;

    await_arr(responses) t;

    if (responses[E_ASYNC_DIALOG_RESPONSE]) {
        new const
            value = strval(responses[E_ASYNC_DIALOG_INPUTTEXT])
        ;

        if (!(low <= value <= high)) {
            SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}Enter a number between %i and %i.", low, high);
        } else {
            switch (field) {
                case RACE_VALUE_VEHICLE: {
                    SetRaceVehicleModel(raceid, value);
                }

                // One at a time, so the other one is read back and handed
                // straight in again: the library takes the pair or neither.
                case RACE_VALUE_COLOUR_1: {
                    new
                        colour1,
                        colour2
                    ;

                    GetRaceVehicleColours(raceid, colour1, colour2);
                    SetRaceVehicleColours(raceid, value, colour2);
                }

                case RACE_VALUE_COLOUR_2: {
                    new
                        colour1,
                        colour2
                    ;

                    GetRaceVehicleColours(raceid, colour1, colour2);
                    SetRaceVehicleColours(raceid, colour1, value);
                }

                case RACE_VALUE_LAPS: {
                    SetRaceLaps(raceid, value);
                }

                // The only one of the four that the library can still turn
                // down, because a room cannot be made smaller than the number
                // of people already sitting in it.
                case RACE_VALUE_MAX_RACERS: {
                    if (!SetRaceMaxRacers(raceid, value)) {
                        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}There are already %i racers on the grid.", GetRaceRacerCount(raceid));
                    }
                }

                case RACE_VALUE_COUNTDOWN: {
                    SetRaceCountdown(raceid, value);
                }
            }
        }
    }

    ShowRaceEditMenu(playerid);
}

static ShowRaceMapMenu(playerid) {
    yield 1;

    new const
        Race:raceid = gOwnerRaceID[playerid]
    ;

    if (!IsValidRace(raceid)) {
        return;
    }

    // Changing the map moves every start line, so the library only allows it
    // while the room is empty. Saying so here beats a menu that does nothing.
    if (GetRaceRacerCount(raceid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}The map cannot change while there are racers on the grid.");

        ShowRaceEditMenu(playerid);

        return;
    }

    new
        recordName[MAX_RACE_RECORD_NAME + 1],
        buffer[MAX_MENU_ITEM_LENGTH],
        listitem
    ;

    // Counting to the pool size and skipping the empty slots is how every list
    // in this library is walked: the ids are handed out of a free list, so the
    // ones in use are not a run from zero.
    for (new RaceRecord:recordid; recordid != GetRaceRecordPoolSize(); ++recordid) {
        if (!IsValidRaceRecord(recordid)) {
            continue;
        }

        GetRaceRecordName(recordid, recordName);
        Format(buffer, sizeof (buffer), "%i CPs, %i cars", GetRaceRecordCPCount(recordid), GetRaceRecordRacerStartCount(recordid));

        AddListMenuItem(playerid, 0, recordName);
        AddListMenuItem(playerid, 1, buffer);

        gListedRecordID[playerid][listitem++] = recordid;
    }

    if (!listitem) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}No maps have been drawn yet (/recordrace).");

        ShowRaceEditMenu(playerid);

        return;
    }

    new const
        Task:t = ShowAsyncListMenu(playerid, "Race Map", MENU_X, MENU_Y, MENU_WIDTH)
    ;

    if (!t) {
        return;
    }

    new
        responses[E_ASYNC_MENU_DATA]
    ;

    await_arr(responses) t;

    if (responses[E_ASYNC_MENU_RESPONSE]) {
        new const
            RaceRecord:recordid = gListedRecordID[playerid][responses[E_ASYNC_MENU_LISTITEM]]
        ;

        if (!IsValidRaceRecord(recordid)) {
            SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}That map is no longer there.");
        } else if (!SetRaceRecord(raceid, recordid)) {
            SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}That map could not be set.");
        }
    }

    ShowRaceEditMenu(playerid);
}

/**
 * # Recording A Course
 */

static ShowRecordNameDialog(playerid) {
    yield 1;

    new const
        RaceRecord:recordid = gEditRaceRecordID[playerid]
    ;

    if (!IsValidRaceRecord(recordid)) {
        return;
    }

    new const
        Task:t = ShowPlayerAsyncDialog(playerid, DIALOG_STYLE_INPUT, "Race Map Name",
            "{FFFFFF}What is this course called?\n\nBetween "#MIN_RACE_NAME" and "#MAX_RACE_RECORD_NAME" characters.",
            "Save", "Cancel"
        )
    ;

    if (!t) {
        return;
    }

    new
        responses[E_ASYNC_DIALOG]
    ;

    await_arr(responses) t;

    if (!responses[E_ASYNC_DIALOG_RESPONSE]) {
        return;
    }

    if (!(MIN_RACE_NAME <= strlen(responses[E_ASYNC_DIALOG_INPUTTEXT]) <= MAX_RACE_RECORD_NAME)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}A name is between "#MIN_RACE_NAME" and "#MAX_RACE_RECORD_NAME" characters.");

        ShowRecordNameDialog(playerid);

        return;
    }

    SetRaceRecordName(recordid, responses[E_ASYNC_DIALOG_INPUTTEXT]);

    ClearRecordVehicles(recordid);
    StopRecording(playerid);

    SendClientMessage(playerid, -1, "{00FF00}INFO: {FFFFFF}Saved map '%s' (%i): %i starts and %i checkpoints.",
        responses[E_ASYNC_DIALOG_INPUTTEXT], _:recordid,
        GetRaceRecordRacerStartCount(recordid), GetRaceRecordCPCount(recordid));
}

/**
 * # Joining
 */

static ShowRaceJoinDialog(playerid) {
    yield 1;

    new
        creatorName[MAX_PLAYER_NAME + 1],
        raceName[MAX_RACE_NAME + 1],
        recordName[MAX_RACE_RECORD_NAME + 1],
        body[(MAX_PLAYER_NAME + MAX_RACE_NAME + MAX_RACE_RECORD_NAME + 24) * (_:MAX_RACES + 1)] = "Creator\tRace\tMap\tRacers\n",
        listitem
    ;

    // Four columns, which is one more than a textdraw menu has room for, so
    // this one is a dialog while the other two lists are menus.
    for (new Race:raceid; raceid != GetRacePoolSize(); ++raceid) {
        if (!IsValidRace(raceid)) {
            continue;
        }

        // A private race is one you have to be asked to; it is not on the list.
        if (IsRacePrivated(raceid)) {
            continue;
        }

        // Nor is one that has already gone: the library turns those away too,
        // and a row nobody can click is a row that should not be drawn.
        if (IsRaceStarted(raceid)) {
            continue;
        }

        GetName(GetRaceCreator(raceid), creatorName);
        GetRaceName(raceid, raceName);
        GetRaceRecordName(GetRaceRecord(raceid), recordName);

        Format(body, sizeof (body), "%s%s\t%s\t%s\t%i/%i\n", body,
            creatorName, raceName, recordName,
            GetRaceRacerCount(raceid), GetRaceMaxRacers(raceid));

        gListedRaceID[playerid][listitem++] = raceid;
    }

    if (!listitem) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}There are no races to join right now (/createrace).");

        return;
    }

    new const
        Task:t = ShowPlayerAsyncDialog(playerid, DIALOG_STYLE_TABLIST_HEADERS, "Races", body, "Join", "Close")
    ;

    if (!t) {
        return;
    }

    new
        responses[E_ASYNC_DIALOG]
    ;

    await_arr(responses) t;

    if (!responses[E_ASYNC_DIALOG_RESPONSE]) {
        return;
    }

    new const
        Race:raceid = gListedRaceID[playerid][responses[E_ASYNC_DIALOG_LISTITEM]]
    ;

    new
        creatorPickedName[MAX_PLAYER_NAME + 1],
        pickedName[MAX_RACE_NAME + 1]
    ;

    GetName(GetRaceCreator(raceid), creatorPickedName);
    GetRaceName(raceid, pickedName);

    // Every way the join can be turned down, said in the words of the thing
    // that turned it down. This used to be a number between one and four with
    // no name on it, and the script could only ever say that the race was full.
    switch (PutPlayerInRace(playerid, raceid)) {
        case RACE_JOIN_OK: {
            SendClientMessage(playerid, -1, "{00FF00}INFO: {FFFFFF}You are on the grid for '%s' by %s. Wait for the lights.", pickedName, creatorPickedName);

            return;
        }

        case RACE_JOIN_INVALID_RACE: {
            SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}That race is no longer there.");
        }

        case RACE_JOIN_ALREADY_IN_RACE: {
            SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}You are already in a race (/leaverace).");
        }

        case RACE_JOIN_STARTED: {
            SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}That race has already started.");
        }

        case RACE_JOIN_FULL: {
            SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}'%s' is full.", pickedName);
        }

        case RACE_JOIN_NO_RECORD: {
            SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}'%s' has no map on it.", pickedName);
        }

        case RACE_JOIN_INVALID_PLAYER: {
            SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}You cannot join a race right now.");
        }
    }

    ShowRaceJoinDialog(playerid);
}

/**
 * # Commands
 */

static bool:CommandRecordRace(playerid) {
    if (IsValidRaceRecord(gEditRaceRecordID[playerid])) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}You are already drawing a map (/saverace, /cancelrace).");

        return true;
    }

    new const
        RaceRecord:recordid = RecordRace()
    ;

    if (recordid == INVALID_RACE_RECORD_ID) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}There is no room for another map.");

        return true;
    }

    gEditRaceRecordID[playerid] = recordid;
    gIsCreationRaceCPEnabled{playerid} = false;

    SendClientMessage(playerid, -1, "{00FF00}INFO: {FFFFFF}Drawing map %i. On foot, use /addracer <model> to put down each start.", _:recordid);
    SendClientMessage(playerid, -1, "{00FF00}INFO: {FFFFFF}Then /enableracecp, and drive the course clicking to drop checkpoints.");

    return true;
}

static bool:CommandAddRacer(playerid, const params[]) {
    new const
        RaceRecord:recordid = gEditRaceRecordID[playerid]
    ;

    if (!IsValidRaceRecord(recordid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}You are not drawing a map (/recordrace).");

        return true;
    }

    // Getting out first is what makes the next start line land somewhere else:
    // the command puts the player into the car it just made.
    if (IsPlayerInAnyVehicle(playerid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}Get out first, then walk to where the next car goes.");

        return true;
    }

    new
        modelid
    ;

    if (sscanf(params, "i", modelid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}/addracer <model-id>");

        return true;
    }

    if (!(MIN_RACE_VEHICLE_MODEL <= modelid <= MAX_RACE_VEHICLE_MODEL)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}A model is between "#MIN_RACE_VEHICLE_MODEL" and "#MAX_RACE_VEHICLE_MODEL".");

        return true;
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:a
    ;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);

    new const
        index = CreateRaceRecordRacerStart(recordid, x, y, z, a)
    ;

    // A full map answers -1, which used to be written into the array of preview
    // cars as an index and put a vehicle id one cell in front of it.
    if (index == -1) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}This map already has all "#MAX_RACERS" starts on it.");

        return true;
    }

    gRaceRecordVehicleID[recordid][index] = CreateVehicle(modelid, x, y, z, a, -1, -1, -1);

    PutPlayerInVehicle(playerid, gRaceRecordVehicleID[recordid][index], 0);
    PlayerGameTextShow(playerid, STATUS_STYLE, GAME_TEXT_TIME, "~w~Start ~p~%i ~w~placed.", index + 1);

    return true;
}

static bool:CommandEnableRaceCP(playerid) {
    if (!IsValidRaceRecord(gEditRaceRecordID[playerid])) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}You are not drawing a map (/recordrace).");

        return true;
    }

    if (gIsCreationRaceCPEnabled{playerid}) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}Checkpoints are already on.");

        return true;
    }

    // A course with no start line is one nobody can be put on, so it is caught
    // here rather than at the far end when the first player tries to join.
    if (!GetRaceRecordRacerStartCount(gEditRaceRecordID[playerid])) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}Put down at least one start first (/addracer).");

        return true;
    }

    gIsCreationRaceCPEnabled{playerid} = true;

    SendClientMessage(playerid, -1, "{00FF00}INFO: {FFFFFF}Checkpoints are on. Drive the course and click (~k~~PED_FIREWEAPON~) to drop each one.");
    SendClientMessage(playerid, -1, "{00FF00}INFO: {FFFFFF}At least "#MIN_RACE_CHECKPOINTS" of them, then /saverace.");

    return true;
}

static bool:CommandSaveRace(playerid) {
    new const
        RaceRecord:recordid = gEditRaceRecordID[playerid]
    ;

    if (!IsValidRaceRecord(recordid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}You are not drawing a map (/recordrace).");

        return true;
    }

    if (GetRaceRecordCPCount(recordid) < MIN_RACE_CHECKPOINTS) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}A course needs at least "#MIN_RACE_CHECKPOINTS" checkpoints, this one has %i.", GetRaceRecordCPCount(recordid));

        return true;
    }

    ShowRecordNameDialog(playerid);

    return true;
}

static bool:CommandCancelRace(playerid) {
    new const
        RaceRecord:recordid = gEditRaceRecordID[playerid]
    ;

    if (!IsValidRaceRecord(recordid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}You are not drawing a map.");

        return true;
    }

    ClearRecordVehicles(recordid);
    StopRecording(playerid);

    // The slot goes back on the free list, which is the whole reason a half
    // drawn course does not cost a map for the rest of the session.
    DestroyRaceRecord(recordid);

    SendClientMessage(playerid, -1, "{00FF00}INFO: {FFFFFF}Map %i thrown away.", _:recordid);

    return true;
}

static bool:CommandCreateRace(playerid, const params[]) {
    if (IsValidRace(gOwnerRaceID[playerid])) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}You already have a race (/editrace, /destroyrace).");

        return true;
    }

    if (!GetRaceRecordCount()) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}No maps have been drawn yet (/recordrace).");

        return true;
    }

    new
        modelid
    ;

    if (sscanf(params, "i", modelid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}/createrace <model-id>");

        return true;
    }

    if (!(MIN_RACE_VEHICLE_MODEL <= modelid <= MAX_RACE_VEHICLE_MODEL)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}A model is between "#MIN_RACE_VEHICLE_MODEL" and "#MAX_RACE_VEHICLE_MODEL".");

        return true;
    }

    new const
        Race:raceid = CreateRace(playerid, modelid)
    ;

    if (raceid == INVALID_RACE_ID) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}There is no room for another race.");

        return true;
    }

    gOwnerRaceID[playerid] = raceid;

    ShowRaceEditMenu(playerid);

    return true;
}

static bool:CommandDestroyRace(playerid) {
    if (!IsValidRace(gOwnerRaceID[playerid])) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}You didn't create a race.");

        return true;
    }

    // Everybody inside is put back where they joined from on the way out. The
    // old example reached into the iterator instead and left them sitting in
    // their cars, frozen, in a race that no longer existed.
    DestroyRace(gOwnerRaceID[playerid]);

    SendClientMessage(playerid, -1, "{00FF00}INFO: {FFFFFF}Your race is closed.");

    return true;
}

/**
 * # Calls
 */

public OnGameModeInit() {
    SetGameModeText("Race Example");

    AddPlayerClass(0, SPAWN_X, SPAWN_Y, SPAWN_Z, 0.0, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);

    BuildDemoCourse();

    return 1;
}

public OnPlayerSpawn(playerid) {
    GiveStarterRace(playerid);

    return 1;
}

public OnPlayerRequestClass(playerid, classid) {
    SetPlayerPos(playerid, SPAWN_X, SPAWN_Y, SPAWN_Z);
    SetPlayerCameraPos(playerid, SPAWN_X, SPAWN_Y + 5.0, SPAWN_Z + 2.0);
    SetPlayerCameraLookAt(playerid, SPAWN_X, SPAWN_Y, SPAWN_Z);

    return 1;
}

public OnPlayerConnect(playerid) {
    gEditRaceRecordID[playerid]        = INVALID_RACE_RECORD_ID;
    gOwnerRaceID[playerid]             = INVALID_RACE_ID;
    gIsCreationRaceCPEnabled{playerid} = false;
    gCreationCPCooldown[playerid]      = 0;

    SendClientMessage(playerid, -1, "{FFFFFF}Draw a course with {00FF00}/recordrace{FFFFFF}, open a race with {00FF00}/createrace{FFFFFF}, join one with {00FF00}/joinrace{FFFFFF}.");

    return 1;
}

public OnPlayerDisconnect(playerid, reason) {
    // The course they were halfway through drawing goes with them, cars and
    // all. race.inc takes them out of any race they were in by itself.
    if (IsValidRaceRecord(gEditRaceRecordID[playerid])) {
        new const
            RaceRecord:recordid = gEditRaceRecordID[playerid]
        ;

        ClearRecordVehicles(recordid);
        StopRecording(playerid);
        DestroyRaceRecord(recordid);
    }

    if (IsValidRace(gOwnerRaceID[playerid])) {
        DestroyRace(gOwnerRaceID[playerid]);
    }

    return 1;
}

public OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if (!(newkeys & KEY_FIRE)) {
        return 1;
    }

    if (!gIsCreationRaceCPEnabled{playerid}) {
        return 1;
    }

    new const
        RaceRecord:recordid = gEditRaceRecordID[playerid]
    ;

    if (!IsValidRaceRecord(recordid)) {
        return 1;
    }

    if (GetTickCountDifference(GetTickCount(), gCreationCPCooldown[playerid]) < RACE_CP_COOLDOWN) {
        return 1;
    }

    gCreationCPCooldown[playerid] = GetTickCount();

    new
        Float:x,
        Float:y,
        Float:z
    ;

    GetPlayerPos(playerid, x, y, z);

    new const
        index = CreateRaceRecordCP(recordid, x, y, z)
    ;

    if (index == -1) {
        PlayerGameTextShow(playerid, ANNOUNCE_STYLE, GAME_TEXT_TIME, "~w~This ~r~course ~w~has no room for another checkpoint.");

        return 1;
    }

    PlayerGameTextShow(playerid, STATUS_STYLE, GAME_TEXT_TIME, "~w~Checkpoint ~p~%i ~w~placed.", index + 1);

    return 1;
}

// Pawn.CMD matches the word for us and hands over only what came after it, so
// there is no chain of strcmp here and no counting characters to find where the
// argument starts. Everything a command has to say no to still lives in the
// Command* helpers above: these are the door, not the room.
CMD:recordrace(playerid, params[]) {
    #pragma unused params
    return _:CommandRecordRace(playerid);
}

CMD:enableracecp(playerid, params[]) {
    #pragma unused params
    return _:CommandEnableRaceCP(playerid);
}

CMD:saverace(playerid, params[]) {
    #pragma unused params
    return _:CommandSaveRace(playerid);
}

CMD:cancelrace(playerid, params[]) {
    #pragma unused params
    return _:CommandCancelRace(playerid);
}

CMD:destroyrace(playerid, params[]) {
    #pragma unused params
    return _:CommandDestroyRace(playerid);
}

CMD:addracer(playerid, params[]) {
    return _:CommandAddRacer(playerid, params);
}

CMD:createrace(playerid, params[]) {
    return _:CommandCreateRace(playerid, params);
}

CMD:editrace(playerid, params[]) {
    #pragma unused params

    if (!IsValidRace(gOwnerRaceID[playerid])) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}You didn't create a race.");

        return 1;
    }

    ShowRaceEditMenu(playerid);

    return 1;
}

CMD:startrace(playerid, params[]) {
    #pragma unused params

    TryStartRace(playerid, gOwnerRaceID[playerid]);

    return 1;
}

CMD:joinrace(playerid, params[]) {
    #pragma unused params

    if (IsPlayerInAnyRace(playerid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}You are already in a race (/leaverace).");

        return 1;
    }

    ShowRaceJoinDialog(playerid);

    return 1;
}

CMD:leaverace(playerid, params[]) {
    #pragma unused params

    if (!IsPlayerInAnyRace(playerid)) {
        SendClientMessage(playerid, -1, "{FF0000}ERROR: {FFFFFF}You are not in a race.");

        return 1;
    }

    RemovePlayerFromRace(playerid);

    return 1;
}

/**
 * # Race Calls
 */

// Left and right on one of the switch rows. Each of those rows carries two
// options, so every move is a flip and the option it landed on does not have to
// be read to know what it means.
public OnPlayerListMenuOptionChange(playerid, item, option) {
    new const
        Race:raceid = gOwnerRaceID[playerid]
    ;

    if (!IsValidRace(raceid)) {
        return 1;
    }

    new
        flags
    ;

    switch (item) {
        case RACE_MENU_PRIVATE: {
            flags = RACE_FLAGS_PRIVATED;
        }

        case RACE_MENU_GHOST: {
            flags = RACE_FLAGS_GHOST;
        }

        case RACE_MENU_NITRO: {
            flags = RACE_FLAGS_NITRO;
        }

        case RACE_MENU_AIR: {
            flags = RACE_FLAGS_ARIAL;
        }
    }

    if (!flags) {
        return 1;
    }

    ToggleRaceFlags(raceid, flags);

    return 1;
}

public OnRaceCountdownUpdate(Race:raceid, countdown, const participants[], size) {
    for (new i; i != size; ++i) {
        GameTextForPlayer(participants[i], "~w~- %i -", GAME_TEXT_TIME, GAME_TEXT_STYLE, countdown);
    }

    return 1;
}

public OnRaceStart(Race:raceid, const participants[], size) {
    for (new i; i != size; ++i) {
        GameTextForPlayer(participants[i], "~w~- ~g~GO ~w~-", GAME_TEXT_TIME_LONG, GAME_TEXT_STYLE);
    }

    return 1;
}

public OnPlayerRaceCheckpoint(playerid, Race:raceid, check) {
    GameTextForPlayer(playerid, "~w~CP ~p~%i~w~/%i", GAME_TEXT_TIME, GAME_TEXT_STYLE, check + 1, GetRaceRecordCPCount(GetRaceRecord(raceid)));

    return 1;
}

// Once a second while this racer is on foot. No timer here and nothing to
// search: the library knows who is walking because it is the one holding their
// clock, so it says so rather than making this script go and find out.
//
// Held on screen a shade longer than the gap between two ticks, so the line
// does not blink out in between.
public OnPlayerRaceReturnUpdate(playerid, Race:raceid, secondsLeft) {
    PlayerGameTextShow(playerid, ANNOUNCE_STYLE, 1100, "~w~You have %i seconds to return to your ~r~vehicle ~w~or you will be disqualified.", secondsLeft);

    return 1;
}

// The clock ran out. Answering zero would leave them walking; this example
// takes them out, which is what the library does on its own for death and
// disconnection anyway.
public OnPlayerRaceReturnTimeout(playerid, Race:raceid) {
    PlayerGameTextShow(playerid, ANNOUNCE_STYLE, GAME_TEXT_TIME_LONG, "~w~You were ~r~disqualified ~w~for leaving your vehicle.");

    return 1;
}

public OnPlayerRaceLap(playerid, Race:raceid, lap) {
    GameTextForPlayer(playerid, "~w~LAP ~p~%i~w~/%i", GAME_TEXT_TIME_LONG, GAME_TEXT_STYLE, GetPlayerRaceLap(playerid), GetRaceLaps(raceid));

    return 1;
}

public OnPlayerFinishRace(playerid, Race:raceid, position) {
    new
        raceName[MAX_RACE_NAME + 1],
        playerName[MAX_PLAYER_NAME + 1]
    ;

    GetRaceName(raceid, raceName);
    GetName(playerid, playerName);

    GameTextForPlayer(playerid, "~w~FINISHED ~p~%i~w~%s", GAME_TEXT_TIME_LONG, GAME_TEXT_STYLE, position, position == 1 ? "st" : position == 2 ? "nd" : position == 3 ? "rd" : "th");

    // Told to the rest of the field rather than to the whole server, because a
    // race is between the people in it.
    ForEachRacer (racerid : raceid) {
        if (racerid == playerid) {
            continue;
        }

        SendClientMessage(racerid, -1, "{00FF00}INFO: {FFFFFF}%s finished '%s' in position %i.", playerName, raceName, position);
    }

    return 1;
}

public OnRaceFinish(Race:raceid, const participants[], size) {
    // Everybody is back where they joined from. This empties the race as it
    // goes, and it is safe to do so straight down what was handed in: the array
    // is a copy the call made, so taking racers out cannot move it. Walking the
    // race itself with ForEachRacer here would be standing on the row that
    // removing them rearranges.
    for (new i; i != size; ++i) {
        SendClientMessage(participants[i], -1, "{00FF00}INFO: {FFFFFF}The race is over.");
        RemovePlayerFromRace(participants[i]);
    }

    return 1;
}

public OnPlayerJoinRace(playerid, Race:raceid) {
    new
        playerName[MAX_PLAYER_NAME + 1]
    ;

    GetName(playerid, playerName);

    ForEachRacer (racerid : raceid) {
        if (racerid == playerid) {
            continue;
        }

        SendClientMessage(racerid, -1, "{00FF00}INFO: {FFFFFF}%s is on the grid (%i/%i).", playerName, GetRaceRacerCount(raceid), GetRaceMaxRacers(raceid));
    }

    return 1;
}

public OnPlayerLeaveRace(playerid, Race:raceid) {
    if (!IsPlayerConnected(playerid)) {
        return 1;
    }

    SendClientMessage(playerid, -1, "{00FF00}INFO: {FFFFFF}You are out of the race.");

    return 1;
}

public OnRaceDestroy(Race:raceid, creatorid) {
    // The owner is holding this id and has no other way of hearing that it has
    // gone: the race can be taken down by them disconnecting, and the id would
    // otherwise be handed straight back out to somebody else's /createrace.
    //
    // Whose it was arrives with the callback, so nothing is searched for. The
    // id is checked against theirs anyway, because a creator who was moved off
    // this race with SetRaceCreator is not the one holding it.
    if (!(0 <= creatorid < MAX_PLAYERS)) {
        return 1;
    }

    if (gOwnerRaceID[creatorid] != raceid) {
        return 1;
    }

    gOwnerRaceID[creatorid] = INVALID_RACE_ID;

    return 1;
}
