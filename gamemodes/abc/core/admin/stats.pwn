#if defined _CORE_ADMIN_STATS
    #endinput
#endif
#define _CORE_ADMIN_STATS

CMD:devmoney(playerid, params[]) {
    new amount;

    if (sscanf(params, "i", amount)) {
        return SendClientMessage(playerid, -1, "Usage: /devmoney <amount>");
    }

    GivePlayerMoney(playerid, amount);

    return 1;
}

public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ) {
    SetPlayerPosFindZ(playerid, fX, fY, fZ);

    return 1;
}