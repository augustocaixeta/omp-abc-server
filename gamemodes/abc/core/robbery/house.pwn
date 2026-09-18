#if defined _CORE_ROBBERY_HOUSE
    #endinput
#endif
#define _CORE_ROBBERY_HOUSE

#include <pp-hooks>

// What a house is worth turning over. One line per thing, and the only place a
// house says what is in it -- a shop or a warehouse is another file hooking the
// same callback.

hook OnPlayerStartRobbery(playerid, Property:propertyid) {
    if (!IsPropertyHouse(propertyid)) {
        return 0;
    }

    AddPlayerRobberyLoot(playerid, gItemBuildGasCanister, 33.41170, 1409.90027, 1083.54761);

    return 0;
}
