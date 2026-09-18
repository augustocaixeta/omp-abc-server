#if defined _CORE_ROBBERY_LOCKUP
    #endinput
#endif
#define _CORE_ROBBERY_LOCKUP

#include <pp-hooks>

/**
 * # The lock-up job
 *
 * The run of a robbery from the job point to the payoff, one step at a time:
 *
 *     leave    out of the lock-up, the van is waiting
 *     van      get in it
 *     travel   a checkpoint on somebody's front door, and the place is filled
 *     loot     take what will fit and get it in the back
 *     return   a checkpoint on the parking space
 *     park     get out and leave it there, which finishes the job
 *
 * The lock-up knows nothing about robbing and the robbery knows nothing about
 * vans. This is the line between them, and every step it takes is something one
 * of the two already announced.
 */

#if !defined LOCKUP_JOB_CP_SIZE
    #define LOCKUP_JOB_CP_SIZE (3.0)
#endif

#if !defined LOCKUP_JOB_PARK_RANGE
    #define LOCKUP_JOB_PARK_RANGE (8.0)
#endif

#if !defined LOCKUP_JOB_TEXT_TIME
    #define LOCKUP_JOB_TEXT_TIME (4000)
#endif

enum {
    LOCKUP_JOB_NONE,
    LOCKUP_JOB_LEAVE,
    LOCKUP_JOB_VAN,
    LOCKUP_JOB_TRAVEL,
    LOCKUP_JOB_LOOT,
    LOCKUP_JOB_RETURN,
    LOCKUP_JOB_PARK
};

static enum _:E_LOCKUP_JOB_DATA {
    E_LOCKUP_JOB_STAGE,

    Lockup:E_LOCKUP_JOB_LOCKUP,
    STREAMER_TAG_CP:E_LOCKUP_JOB_CP
};

static
    gLockupJobData[MAX_PLAYERS][E_LOCKUP_JOB_DATA]
;

static stock ClearLockupJobCheckpointInternal(playerid) {
    if (gLockupJobData[playerid][E_LOCKUP_JOB_CP] == STREAMER_TAG_CP:INVALID_STREAMER_ID) {
        return;
    }

    DestroyDynamicCP(gLockupJobData[playerid][E_LOCKUP_JOB_CP]);

    gLockupJobData[playerid][E_LOCKUP_JOB_CP] = STREAMER_TAG_CP:INVALID_STREAMER_ID;
}

static stock ClearLockupJobInternal(playerid) {
    ClearLockupJobCheckpointInternal(playerid);

    gLockupJobData[playerid][E_LOCKUP_JOB_STAGE] = LOCKUP_JOB_NONE;
    gLockupJobData[playerid][E_LOCKUP_JOB_LOCKUP] = INVALID_LOCKUP_ID;
}

static stock MarkLockupJobInternal(playerid, Float:x, Float:y, Float:z, worldid, interiorid) {
    ClearLockupJobCheckpointInternal(playerid);

    gLockupJobData[playerid][E_LOCKUP_JOB_CP] = CreateDynamicCP(x, y, z, LOCKUP_JOB_CP_SIZE, worldid, interiorid, playerid);
}

// Somewhere that is not theirs and is not the lock-up they came out of.
static stock Property:PickLockupJobTargetInternal(playerid) {
    new
        House:candidates[_:MAX_HOUSES],
        count
    ;

    for (new House:houseid, pool = GetHousePoolSize(); _:houseid != pool && count != sizeof (candidates); ++houseid) {
        if (!IsValidHouse(houseid)) {
            continue;
        }

        if (IsPlayerHouseOwner(playerid, houseid)) {
            continue;
        }

        candidates[count++] = houseid;
    }

    if (count == 0) {
        return INVALID_PROPERTY_ID;
    }

    return GetHouseProperty(candidates[random(count)]);
}

static stock bool:SendPlayerToLockupTargetInternal(playerid) {
    new const
        Property:propertyid = PickLockupJobTargetInternal(playerid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return false;
    }

    if (!StartPlayerRobbery(playerid, propertyid)) {
        return false;
    }

    new const
        Entrance:entranceid = GetPropertyEntrance(propertyid)
    ;

    new
        Float:x,
        Float:y,
        Float:z
    ;

    GetEntranceExteriorPos(entranceid, x, y, z);

    MarkLockupJobInternal(playerid, x, y, z, GetEntranceExteriorVirtualWorld(entranceid), GetEntranceExteriorInterior(entranceid));

    new
        name[MAX_PROPERTY_NAME]
    ;

    GetPropertyName(propertyid, name);

    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, LOCKUP_JOB_TEXT_TIME, "~w~Drive to the ~y~Mark");
    SendPlayerNotice(playerid, "%s. Take what will fit and get it in the back.", name);

    return true;
}

static stock SendPlayerHomeInternal(playerid) {
    new const
        Lockup:lockupid = gLockupJobData[playerid][E_LOCKUP_JOB_LOCKUP]
    ;

    new
        Float:x,
        Float:y,
        Float:z,
        Float:a
    ;

    if (!GetLockupVanPos(lockupid, x, y, z, a)) {
        return;
    }

    MarkLockupJobInternal(playerid, x, y, z, 0, 0);

    gLockupJobData[playerid][E_LOCKUP_JOB_STAGE] = LOCKUP_JOB_RETURN;

    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, LOCKUP_JOB_TEXT_TIME, "~w~Back to the ~y~Lock-up");
    SendPlayerNotice(playerid, "Take it back to the lock-up.");
}

static stock bool:IsPlayerInLockupVanInternal(playerid, Lockup:lockupid) {
    new const
        vehicleid = GetLockupVan(lockupid)
    ;

    if (vehicleid == INVALID_VEHICLE_ID) {
        return false;
    }

    return GetPlayerVehicleID(playerid) == vehicleid;
}

/**
 * # Calls
 */

hook OnLockupVanTaken(Lockup:lockupid, playerid) {
    if (playerid == INVALID_PLAYER_ID) {
        return 0;
    }

    ClearLockupJobInternal(playerid);

    gLockupJobData[playerid][E_LOCKUP_JOB_LOCKUP] = lockupid;

    // Somebody who took it from the street is already where the next step
    // wants them, and there is no door for them to come out of.
    if (GetPlayerProperty(playerid) != GetLockupProperty(lockupid)) {
        gLockupJobData[playerid][E_LOCKUP_JOB_STAGE] = LOCKUP_JOB_VAN;

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, LOCKUP_JOB_TEXT_TIME, "~w~Get in the ~y~Van");

        return 0;
    }

    gLockupJobData[playerid][E_LOCKUP_JOB_STAGE] = LOCKUP_JOB_LEAVE;

    SendPlayerNotice(playerid, "The van is outside. Get out of here and take it.");

    return 0;
}

hook OnPlayerLeftProperty(playerid, Property:propertyid) {
    if (gLockupJobData[playerid][E_LOCKUP_JOB_STAGE] != LOCKUP_JOB_LEAVE) {
        return 0;
    }

    if (GetPropertyLockup(propertyid) != gLockupJobData[playerid][E_LOCKUP_JOB_LOCKUP]) {
        return 0;
    }

    gLockupJobData[playerid][E_LOCKUP_JOB_STAGE] = LOCKUP_JOB_VAN;

    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, LOCKUP_JOB_TEXT_TIME, "~w~Get in the ~y~Van");

    return 0;
}

hook OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate) {
    new const
        stage = gLockupJobData[playerid][E_LOCKUP_JOB_STAGE]
    ;

    if (stage == LOCKUP_JOB_NONE) {
        return 0;
    }

    new const
        Lockup:lockupid = gLockupJobData[playerid][E_LOCKUP_JOB_LOCKUP]
    ;

    if (newstate == PLAYER_STATE_DRIVER) {
        if (!IsPlayerInLockupVanInternal(playerid, lockupid)) {
            return 0;
        }

        if (stage == LOCKUP_JOB_VAN) {
            if (!SendPlayerToLockupTargetInternal(playerid)) {
                SendPlayerNotice(playerid, "There is nowhere worth turning over tonight.");

                return 0;
            }

            gLockupJobData[playerid][E_LOCKUP_JOB_STAGE] = LOCKUP_JOB_TRAVEL;

            return 0;
        }

        if (stage == LOCKUP_JOB_LOOT) {
            SendPlayerHomeInternal(playerid);
        }

        return 0;
    }

    // Out of the van at the parking space, which is the whole of delivering.
    if (newstate != PLAYER_STATE_ONFOOT || oldstate != PLAYER_STATE_DRIVER) {
        return 0;
    }

    if (stage != LOCKUP_JOB_PARK) {
        return 0;
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:a
    ;

    if (!GetLockupVanPos(lockupid, x, y, z, a)) {
        return 0;
    }

    if (!IsPlayerInRangeOfPoint(playerid, LOCKUP_JOB_PARK_RANGE, x, y, z)) {
        SendPlayerNotice(playerid, "Not here. Leave it on the space outside the lock-up.");

        return 0;
    }

    StoreLockupVan(lockupid, playerid);

    return 0;
}

hook OnPlayerEnterDynamicCP(playerid, STREAMER_TAG_CP:checkpointid) {
    if (checkpointid != gLockupJobData[playerid][E_LOCKUP_JOB_CP]) {
        return 0;
    }

    new const
        stage = gLockupJobData[playerid][E_LOCKUP_JOB_STAGE]
    ;

    ClearLockupJobCheckpointInternal(playerid);

    if (stage == LOCKUP_JOB_TRAVEL) {
        gLockupJobData[playerid][E_LOCKUP_JOB_STAGE] = LOCKUP_JOB_LOOT;

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, LOCKUP_JOB_TEXT_TIME, "~w~Rob the ~y~House");

        return 0;
    }

    if (stage == LOCKUP_JOB_RETURN) {
        gLockupJobData[playerid][E_LOCKUP_JOB_STAGE] = LOCKUP_JOB_PARK;

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, LOCKUP_JOB_TEXT_TIME, "~w~Leave the ~y~Van");
        SendPlayerNotice(playerid, "Leave it on the space and get out.");
    }

    return 0;
}

hook OnLockupVanStored(Lockup:lockupid, playerid, unloaded) {
    if (playerid == INVALID_PLAYER_ID) {
        return 0;
    }

    if (!IsPlayerRobbing(playerid)) {
        return 0;
    }

    if (unloaded == 0) {
        SendPlayerNotice(playerid, "You come back with an empty van.");
    } else {
        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, LOCKUP_JOB_TEXT_TIME, "~w~Job ~y~Done");
        SendPlayerNotice(playerid, "You unload %i into the locker. Whatever you took is in there.", unloaded);
    }

    StopPlayerRobbery(playerid);

    return 0;
}

// However a job ended, the steps it was in the middle of end with it.
hook OnPlayerStopRobbery(playerid, Property:propertyid) {
    ClearLockupJobInternal(playerid);

    return 0;
}

hook OnPlayerConnect(playerid) {
    gLockupJobData[playerid][E_LOCKUP_JOB_CP] = STREAMER_TAG_CP:INVALID_STREAMER_ID;

    ClearLockupJobInternal(playerid);

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    ClearLockupJobInternal(playerid);

    return 0;
}
