#if defined _INC_PERSONAL_SPACE
    #endinput
#endif
#define _INC_PERSONAL_SPACE

#include <open.mp>
#include <streamer>
#include <mathutil>
#include <foreach>
#include <pp-hooks>

/**
 * # Personal space
 *
 * Who is standing next to whom, without anybody ever asking. A sphere is
 * attached to each player and the streamer says when one crosses another, so
 * the answer is already known by the time it is wanted.
 *
 * The point is what it replaces. "Who is near me" written by hand is a walk
 * over every player slot on the server, done again on every give, every shout,
 * every interaction -- and it is the same answer the streamer already worked
 * out to decide what to send anyone. Asking it instead is a read.
 *
 * From Southclaws' personal-space, adapted: this project has no logger and
 * calls the callback OnPlayerEnterDynamicArea rather than OnPlayerEnterDynArea,
 * and hooks with pp-hooks rather than y_hooks.
 */

/**
 * # Header
 */

// How close counts as next to. Two metres is arm's length plus a step, which is
// the distance a thing can be handed over.
#if !defined PERSONAL_SPACE_RADIUS
    #define PERSONAL_SPACE_RADIUS (2.0)
#endif

// Stamped into the area's extra data so this file can tell its own areas from
// everybody else's. Spelled the way button.inc spells its own: the letters, so
// a clash is visible rather than a number nobody recognises.
#if !defined PERSONAL_SPACE_IDENTIFIER
    #define PERSONAL_SPACE_IDENTIFIER (0x707370)
#endif

enum E_APPROACH_DIRECTION {
    APPROACH_FROM_FRONT,
    APPROACH_FROM_LEFT,
    APPROACH_FROM_BACK,
    APPROACH_FROM_RIGHT
};

static enum _:E_PERSONAL_SPACE_AREA_DATA {
    E_PERSONAL_SPACE_AREA_TYPE,
    E_PERSONAL_SPACE_AREA_PLAYER_ID
};

static
    STREAMER_TAG_AREA:gPersonalSpaceArea[MAX_PLAYERS],
    Iterator:NearbyPlayers[MAX_PLAYERS]<MAX_PLAYERS>
;

/**
 * # Functions
 */

forward bool:IsPlayerNextToPlayer(playerid, targetid);
forward GetPlayersNextToPlayer(playerid, players[], size = sizeof (players));
forward E_APPROACH_DIRECTION:GetApproachDirection(Float:angle);

/**
 * # Events
 */

forward OnPlayerApproachPlayer(playerid, targetid, E_APPROACH_DIRECTION:fromDirection, Float:angle);
forward OnPlayerLeavePlayer(playerid, targetid);

/**
 * # Internal
 */

static stock bool:GetPersonalSpaceAreaDataInternal(STREAMER_TAG_AREA:areaid, &targetid) {
    new
        data[E_PERSONAL_SPACE_AREA_DATA]
    ;

    Streamer_GetArrayData(
        STREAMER_TYPE_AREA,
        areaid,
        E_STREAMER_EXTRA_ID,
        data
    );

    if (data[E_PERSONAL_SPACE_AREA_TYPE] != PERSONAL_SPACE_IDENTIFIER) {
        return false;
    }

    targetid = data[E_PERSONAL_SPACE_AREA_PLAYER_ID];

    return true;
}

static stock bool:IsPersonalSpacePairInternal(playerid, targetid) {
    if (playerid == targetid) {
        return false;
    }

    if (!IsPlayerConnected(targetid)) {
        return false;
    }

    if (GetPlayerState(playerid) == PLAYER_STATE_SPECTATING) {
        return false;
    }

    if (GetPlayerState(targetid) == PLAYER_STATE_SPECTATING) {
        return false;
    }

    return true;
}

/**
 * # Implementation
 */

stock bool:IsPlayerNextToPlayer(playerid, targetid) {
    if (!IsPersonalSpacePairInternal(playerid, targetid)) {
        return false;
    }

    return IsPlayerInDynamicArea(playerid, gPersonalSpaceArea[targetid]);
}

stock GetPlayersNextToPlayer(playerid, players[], size = sizeof (players)) {
    if (!IsPlayerConnected(playerid)) {
        return 0;
    }

    new
        count
    ;

    foreach (new i : NearbyPlayers[playerid]) {
        if (count == size) {
            break;
        }

        players[count++] = i;
    }

    return count;
}

stock E_APPROACH_DIRECTION:GetApproachDirection(Float:angle) {
    if (155.0 < angle < 205.0) {
        return APPROACH_FROM_BACK;
    }

    if (225.0 < angle < 315.0) {
        return APPROACH_FROM_RIGHT;
    }

    if (45.0 < angle < 155.0) {
        return APPROACH_FROM_LEFT;
    }

    return APPROACH_FROM_FRONT;
}

/**
 * # Calls
 */

hook OnGameModeInit() {
    Iter_Init(NearbyPlayers);

    return 0;
}

hook OnPlayerConnect(playerid) {
    new
        data[E_PERSONAL_SPACE_AREA_DATA]
    ;

    data[E_PERSONAL_SPACE_AREA_TYPE] = PERSONAL_SPACE_IDENTIFIER;
    data[E_PERSONAL_SPACE_AREA_PLAYER_ID] = playerid;

    gPersonalSpaceArea[playerid] = CreateDynamicSphere(0.0, 0.0, 0.0, PERSONAL_SPACE_RADIUS);

    Streamer_SetArrayData(STREAMER_TYPE_AREA, gPersonalSpaceArea[playerid], E_STREAMER_EXTRA_ID, data);

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    DestroyDynamicArea(gPersonalSpaceArea[playerid]);
    gPersonalSpaceArea[playerid] = STREAMER_TAG_AREA:INVALID_STREAMER_ID;

    return 0;
}

hook OnPlayerSpawn(playerid) {
    AttachDynamicAreaToPlayer(gPersonalSpaceArea[playerid], playerid);

    return 0;
}

hook OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA:areaid) {
    new
        targetid
    ;

    if (!GetPersonalSpaceAreaDataInternal(areaid, targetid)) {
        return 0;
    }

    if (!IsPersonalSpacePairInternal(playerid, targetid)) {
        return 0;
    }

    Iter_Add(NearbyPlayers[playerid], targetid);
    Iter_Add(NearbyPlayers[targetid], playerid);

    new
        Float:x,
        Float:y,
        Float:z,
        Float:targetX,
        Float:targetY,
        Float:targetZ
    ;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerPos(targetid, targetX, targetY, targetZ);

    new const
        Float:angle = GetAbsoluteAngle(GetAngleToPoint(targetX, targetY, x, y))
    ;

    CallLocalFunction("OnPlayerApproachPlayer", "iiif", playerid, targetid, _:GetApproachDirection(angle), angle);

    return 0;
}

hook OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA:areaid) {
    new
        targetid
    ;

    if (!GetPersonalSpaceAreaDataInternal(areaid, targetid)) {
        return 0;
    }

    if (playerid == targetid) {
        return 0;
    }

    Iter_Remove(NearbyPlayers[playerid], targetid);
    Iter_Remove(NearbyPlayers[targetid], playerid);

    CallLocalFunction("OnPlayerLeavePlayer", "ii", playerid, targetid);

    return 0;
}
