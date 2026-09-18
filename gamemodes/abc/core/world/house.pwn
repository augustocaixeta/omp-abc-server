#if defined _CORE_WORLD_HOUSE
    #endinput
#endif
#define _CORE_WORLD_HOUSE

#include <pp-hooks>

// A tick after boot rather than during it. YSI's script-init runs inside
// OnGameModeInit and does not come back, so anything built behind it in that
// callback is never built at all.

forward @House_Build();

// They all lead to the same room until there are more interiors to lead to.
static stock BuildHouseInternal(const name[], Float:extX, Float:extY, Float:extZ, Float:extA, Float:markerX, Float:markerY, Float:markerZ, price) {
    new const
        House:houseid = CreateHouse(name,
            extX, extY, extZ, extA,
            22.8460, 1403.3585, 1084.4370, 0.0000, 5,
            price
        )
    ;

    if (houseid == INVALID_HOUSE_ID) {
        printf("[house] %s was refused", name);

        return;
    }

    SetPropertySaleMarkerPos(GetHouseProperty(houseid), markerX, markerY, markerZ);
}

public @House_Build() {
    BuildHouseInternal("House 1", 2465.1709, -1995.8982, 14.0193, 180.0000, 2463.6931, -2001.4835, 13.5469, 75000);
    BuildHouseInternal("House 2", 2465.3237, -2020.7490, 14.1242, 0.0000, 2465.1914, -2016.3645, 13.5469, 90000);
    BuildHouseInternal("House 3", 2486.4102, -2021.4005, 13.9988, 0.0000, 2485.4634, -2016.2733, 13.5469, 120000);
}

hook OnGameModeInit() {
    SetTimerEx("@House_Build", 1, false, "");

    return 0;
}
