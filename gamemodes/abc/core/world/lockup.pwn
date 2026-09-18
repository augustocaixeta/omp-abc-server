#if defined _CORE_WORLD_LOCKUP
    #endinput
#endif
#define _CORE_WORLD_LOCKUP

#include <pp-hooks>

// A tick after boot rather than during it. YSI's script-init runs inside
// OnGameModeInit and does not come back, so anything built behind it in that
// callback is never built at all.

forward @Lockup_Build();

public @Lockup_Build() {
    new const
        Lockup:creek = CreateLockup("Lock-up 1",
            2447.8550, -1962.9861, 13.5469, 180.0000,
            318.5649, 1118.2099, 1083.8828, 0.0000, 5,
            2453.4553, -1985.5105, 13.5540, 180.0000)
    ;

    if (creek == INVALID_LOCKUP_ID) {
        print("[lockup] Lock-up 1 was refused");

        return;
    }

    SetPropertySaleMarkerPos(GetLockupProperty(creek), 2451.8931, -1963.7034, 13.5539);

    // Standing where the room's own locker was, which comes out below.
    SetLockupStorePos(creek, 316.18771, 1116.93213, 1082.86316, 270.0000);
    SetLockupJobPos(creek, 331.1346, 1128.4156, 1083.8828);
}

// The map's locker is scenery and cannot be opened, so it is taken out and one
// that can be is put back in its place.
hook OnPlayerConnect(playerid) {
    RemoveBuildingForPlayer(playerid, 1750, 315.6797, 1116.6563, 1082.8750, 0.25);

    return 0;
}

hook OnGameModeInit() {
    SetTimerEx("@Lockup_Build", 1, false, "");

    return 0;
}
