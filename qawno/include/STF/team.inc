#if defined _INC_TEAM
    #endinput
#endif
#define _INC_TEAM

#include <pp-hooks>

#if !defined MAX_TEAMS
    #define MAX_TEAMS (Team:10000)
#endif

#if !defined MAX_TEAM_RANKS
    #define MAX_TEAM_RANKS (TeamRank:16)
#endif

#if !defined MAX_TEAM_MEMBERS
    #define MAX_TEAM_MEMBERS (TeamMember:10000)
#endif

#if !defined MAX_TEAM_VEHICLES
    #define MAX_TEAM_VEHICLES (TeamVehicle:64)
#endif

#if !defined MAX_TEAM_NAME
    #define MAX_TEAM_NAME (64)
#endif

#if !defined MAX_TEAM_RANK_NAME
    #define MAX_TEAM_RANK_NAME (32)
#endif

#if !defined MAX_TEAM_ABBREVIATION
    #define MAX_TEAM_ABBREVIATION (4)
#endif

#if !defined MAX_PLAYER_TEAMS
    #define MAX_PLAYER_TEAMS (10000)
#endif

#define INVALID_TEAM_ID (Team:-1)
#define INVALID_TEAM_RANK_ID (TeamRank:-1)
#define INVALID_TEAM_MEMBER_ID (TeamMember:-1)
#define INVALID_TEAM_VEHICLE_ID (TeamVehicle:-1)

static enum _:E_TEAM_DATA {
    E_TEAM_NAME[MAX_TEAM_NAME],
    E_TEAM_ABBREVIATION[MAX_TEAM_ABBREVIATION],
    E_TEAM_COLOR,
    E_TEAM_MAXIMUM_RANKS,
    E_TEAM_MAXIMUM_MEMBERS,
    E_TEAM_MAXIMUM_VEHICLES
};

static enum _:E_TEAM_RANK_DATA {
    E_TEAM_RANK_NAME[MAX_TEAM_RANK_NAME],
    E_TEAM_RANK_LEVEL,
    E_TEAM_RANK_PERMISSIONS,
    E_TEAM_RANK_COLOR
};

static enum _:E_TEAM_MEMBER_DATA {
    E_TEAM_MEMBER_NAME[MAX_PLAYER_NAME],
    TeamRank:E_TEAM_MEMBER_RANK_ID,
    E_TEAM_MEMBER_PLAYER_ID
};

static enum E_TEAM_COLLETION_DATA {
    Pool:E_TEAM_RANK_POOL,
    Pool:E_TEAM_MEMBER_POOL
};

static gTeamData[MAX_TEAMS][E_TEAM_COLLETION_DATA];

static Pool:gTeamsPool;

static Map:gTeamBindMap;
static Map:gPlayerTeamMap[MAX_PLAYERS];

/**
 * @Functions
 */
forward Team:CreateTeam(const name[], const abbreviation[], color, maximumRanks = _:MAX_TEAM_RANKS, maximumMembers = _:MAX_TEAM_MEMBERS, maximumVehicles = _:MAX_TEAM_VEHICLES, bool:call = true);
forward bool:IsValidTeam(Team:teamid);
forward TeamMember:AddMemberToTeam(Team:teamid, const name[], TeamRank:rankid = INVALID_TEAM_RANK_ID, bool:call = false);
forward bool:RemoveMemberFromTeam(Team:teamid, TeamMember:memberid, bool:call = true);
forward bool:IsValidTeamMember(Team:teamid, TeamMember:memberid);
forward GetTeamMemberCount(Team:teamid);
forward bool:IsTeamFull(Team:teamid, &bool:value);
forward TeamRank:GetTeamMemberRank(Team:teamid, TeamMember:memberid);
forward GetTeamMemberLinkedPlayer(Team:teamid, TeamMember:memberid);
forward Iter:GetTeamMemberIter(Team:teamid, index = 0);

forward TeamMember:AddPlayerToTeam(playerid, Team:teamid, TeamRank:rankid = INVALID_TEAM_RANK_ID, bool:call = true);
forward bool:RemovePlayerFromTeam(playerid, Team:teamid, bool:call = true);
forward bool:IsPlayerInAnyTeam(playerid);
forward bool:IsPlayerInTeam(playerid, Team:teamid);
forward TeamMember:GetPlayerTeamMember(playerid, Team:teamid);
forward GetPlayerTeamCount(playerid);
forward Iter:GetPlayerTeamIter(playerid, index = 0);

forward OnMemberAddedToTeam(Team:teamid, TeamMember:memberid);
forward OnMemberRemovedFromTeam(Team:teamid, TeamMember:memberid);
forward OnPlayerAddedToTeam(playerid, Team:teamid, TeamMember:memberid);
forward OnPlayerRemovedFromTeam(playerid, Team:teamid, TeamMember:memberid);

/**
 * @Internal Functions
 */
static stock TeamMember:_internal_AddCore(Team:teamid, const name[], TeamRank:rankid, playerid) {
    new Pool:membersCollection =
        gTeamData[teamid][E_TEAM_MEMBER_POOL];
    
    if (!pool_valid(membersCollection)) {
        gTeamData[teamid][E_TEAM_MEMBER_POOL] = membersCollection = pool_new();
    }

    if (pool_size(membersCollection) >= pool_get(gTeamsPool, _:teamid, E_TEAM_MAXIMUM_MEMBERS)) {
        return INVALID_TEAM_MEMBER_ID;
    }

    if (playerid != INVALID_PLAYER_ID) {
        if (!map_valid(gPlayerTeamMap[playerid])) {
            gPlayerTeamMap[playerid] = map_new();
        }
        if (map_size(gPlayerTeamMap[playerid]) >= MAX_PLAYER_TEAMS) {
            return INVALID_TEAM_MEMBER_ID;
        }
        if (map_has_key(gPlayerTeamMap[playerid], _:teamid)) {
            return INVALID_TEAM_MEMBER_ID;
        }
    }

    new data[E_TEAM_MEMBER_DATA];
    strcat(data[E_TEAM_MEMBER_NAME], name);
    data[E_TEAM_MEMBER_RANK_ID] = rankid;
    data[E_TEAM_MEMBER_PLAYER_ID] = playerid;

    new const TeamMember:memberid =
        TeamMember:pool_add_arr(membersCollection, data);

    if (playerid != INVALID_PLAYER_ID) {
        map_set(gPlayerTeamMap[playerid], _:teamid, _:memberid);
    }

    return memberid;
}

static stock bool:_internal_RemoveCore(Team:teamid, TeamMember:memberid) {
    if (!pool_valid(gTeamData[teamid][E_TEAM_MEMBER_POOL])) {
        return false;
    }

    new const Pool:membersCollection =
        gTeamData[teamid][E_TEAM_MEMBER_POOL];

    if (!pool_has(membersCollection, _:memberid)) {
        return false;
    }

    new const playerid =
        pool_get(membersCollection, _:memberid, E_TEAM_MEMBER_PLAYER_ID);

    if (playerid != INVALID_PLAYER_ID) {
        new const Map:playerTeamsCollection =
            gPlayerTeamMap[playerid];

        if (map_valid(playerTeamsCollection) &&
            map_has_key(playerTeamsCollection, _:teamid) &&
            map_remove(playerTeamsCollection, _:teamid) &&
            map_size(playerTeamsCollection) == 0
        ) {
            map_delete(playerTeamsCollection);
            gPlayerTeamMap[playerid] = INVALID_MAP;
        }
    }

    if (pool_remove(membersCollection, _:memberid) && pool_size(membersCollection) == 0) {
        pool_delete(membersCollection);
        gTeamData[teamid][E_TEAM_MEMBER_POOL] = INVALID_POOL;
    }

    return true;
}

/**
 * @Team Functions
 */
stock Team:CreateTeam(const name[], const abbreviation[], color, maximumRanks = _:MAX_TEAM_RANKS, maximumMembers = _:MAX_TEAM_MEMBERS, maximumVehicles = _:MAX_TEAM_VEHICLES, bool:call = true) {
    if (pool_size(gTeamsPool) >= _:MAX_TEAMS) {
        return INVALID_TEAM_ID;
    }

    new data[E_TEAM_DATA];
    strcat(data[E_TEAM_NAME], name);
    strcat(data[E_TEAM_ABBREVIATION], abbreviation);
    data[E_TEAM_COLOR]            = color;
    data[E_TEAM_MAXIMUM_RANKS]    = maximumRanks;
    data[E_TEAM_MAXIMUM_MEMBERS]  = maximumMembers;
    data[E_TEAM_MAXIMUM_VEHICLES] = maximumVehicles;

    new const Team:teamid =
        Team:pool_add_arr(gTeamsPool, data);

    if (call) {
        CallLocalFunction("OnTeamCreate", "i", _:teamid);
    }

    return teamid;
}

stock bool:IsValidTeam(Team:teamid) {
    return _:teamid >= 0 && pool_has(gTeamsPool, _:teamid);
}

stock TeamMember:AddMemberToTeam(Team:teamid, const name[], TeamRank:rankid = INVALID_TEAM_RANK_ID, bool:call = false) {
    if (!IsValidTeam(teamid)) {
        return INVALID_TEAM_MEMBER_ID;
    }

    new const TeamMember:memberid =
        _internal_AddCore(teamid, name, rankid, INVALID_PLAYER_ID);

    if (memberid != INVALID_TEAM_MEMBER_ID && call) {
        CallLocalFunction("OnMemberAddedToTeam", "ii", _:teamid, _:memberid);
    }

    return memberid;
}

stock bool:RemoveMemberFromTeam(Team:teamid, TeamMember:memberid, bool:call = true) {
    if (!IsValidTeam(teamid)) {
        return false;
    }

    if (!_internal_RemoveCore(teamid, memberid)) {
        return false;
    }

    if (call) {
        CallLocalFunction("OnMemberRemovedFromTeam", "ii", _:teamid, _:memberid);
    }

    return true;
}

stock bool:IsValidTeamMember(Team:teamid, TeamMember:memberid) {
    if (!IsValidTeam(teamid)) {
        return false;
    }
    if (!pool_valid(gTeamData[teamid][E_TEAM_MEMBER_POOL])) {
        return false;
    }

    return pool_has(gTeamData[teamid][E_TEAM_MEMBER_POOL], _:memberid);
}

stock GetTeamMemberCount(Team:teamid) {
    if (!IsValidTeam(teamid)) {
        return 0;
    }
    if (!pool_valid(gTeamData[teamid][E_TEAM_MEMBER_POOL])) {
        return 0;
    }

    return pool_size(gTeamData[teamid][E_TEAM_MEMBER_POOL]);
}

stock bool:IsTeamFull(Team:teamid, &bool:value) {
    if (!IsValidTeam(teamid)) {
        return false;
    }
    if (!pool_valid(gTeamData[teamid][E_TEAM_MEMBER_POOL])) {
        return false;
    }
    
    new const current = pool_size(gTeamData[teamid][E_TEAM_MEMBER_POOL]);
    new const maximumMembers = pool_get(gTeamsPool, _:teamid, E_TEAM_MAXIMUM_MEMBERS);

    value = (current >= maximumMembers);

    return true;
}

stock TeamRank:GetTeamMemberRank(Team:teamid, TeamMember:memberid) {
    if (!IsValidTeamMember(teamid, memberid)) {
        return INVALID_TEAM_RANK_ID;
    }

    return TeamRank:pool_get(gTeamData[teamid][E_TEAM_MEMBER_POOL], _:memberid, E_TEAM_MEMBER_RANK_ID);
}

stock GetTeamMemberLinkedPlayer(Team:teamid, TeamMember:memberid) {
    if (!IsValidTeamMember(teamid, memberid)) {
        return INVALID_PLAYER_ID;
    }

    return pool_get(gTeamData[teamid][E_TEAM_MEMBER_POOL], _:memberid, E_TEAM_MEMBER_PLAYER_ID);
}

stock Iter:GetTeamMemberIter(Team:teamid, index = 0) {
    if (!IsValidTeam(teamid)) {
        return INVALID_ITERATOR;
    }
    if (!pool_valid(gTeamData[teamid][E_TEAM_MEMBER_POOL])) {
        return INVALID_ITERATOR;
    }

    return iter_acquire(pool_iter(gTeamData[teamid][E_TEAM_MEMBER_POOL], index));
}

/**
 * @Player Functions
 */
stock TeamMember:AddPlayerToTeam(playerid, Team:teamid, TeamRank:rankid = INVALID_TEAM_RANK_ID, bool:call = true) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return INVALID_TEAM_MEMBER_ID;
    }
    if (!IsValidTeam(teamid)) {
        return INVALID_TEAM_MEMBER_ID;
    }

    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name);

    new TeamMember:memberid =
        _internal_AddCore(teamid, name, rankid, playerid);

    if (memberid != INVALID_TEAM_MEMBER_ID && call) {
        CallLocalFunction("OnPlayerAddedToTeam", "iii", playerid, _:teamid, _:memberid);
    }

    return memberid;
}

stock bool:RemovePlayerFromTeam(playerid, Team:teamid, bool:call = true) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }
    if (!IsValidTeam(teamid)) {
        return false;
    }
    if (!map_valid(gPlayerTeamMap[playerid])) {
        return false;
    }
    if (!map_has_key(gPlayerTeamMap[playerid], _:teamid)) {
        return false;
    }

    new const TeamMember:memberid =
        TeamMember:map_get(gPlayerTeamMap[playerid], _:teamid);
    
    if (!_internal_RemoveCore(teamid, memberid)) {
        return false;
    }

    if (call) {
        CallLocalFunction("OnPlayerRemovedFromTeam", "iii", playerid, _:teamid, _:memberid);
    }

    return true;
}

stock bool:IsPlayerInAnyTeam(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }
    if (!map_valid(gPlayerTeamMap[playerid])) {
        return false;
    }

    return map_size(gPlayerTeamMap[playerid]) != 0;
}

stock bool:IsPlayerInTeam(playerid, Team:teamid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return false;
    }
    if (!map_valid(gPlayerTeamMap[playerid])) {
        return false;
    }

    return map_has_key(gPlayerTeamMap[playerid], _:teamid);
}

stock TeamMember:GetPlayerTeamMember(playerid, Team:teamid) {
    if (!IsPlayerInTeam(playerid, teamid)) {
        return INVALID_TEAM_MEMBER_ID;
    }

    return TeamMember:map_get(gPlayerTeamMap[playerid], _:teamid);
}

stock GetPlayerTeamCount(playerid) {
    if (!(0 <= playerid < MAX_PLAYERS)) {
        return 0;
    }
    if (!map_valid(gPlayerTeamMap[playerid])) {
        return 0;
    }

    return map_size(gPlayerTeamMap[playerid]);
}

stock Iter:GetPlayerTeamIter(playerid, index = 0) {
    if (!IsPlayerInAnyTeam(playerid)) {
        return INVALID_ITERATOR;
    }

    return iter_acquire(map_iter(gPlayerTeamMap[playerid], index));
}

/**
 * @Hooks
 */
hook OnGameModeInit() {
    gTeamsPool = pool_new();

    return 0;
}