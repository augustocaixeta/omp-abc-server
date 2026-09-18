#if defined _CORE_ROBBERY_ROBBERY
    #endinput
#endif
#define _CORE_ROBBERY_ROBBERY

#include <pp-hooks>

/**
 * # Robbing a place
 *
 * A robbery is a set of items put into the world when somebody starts taking
 * them and cleared away when it is over. Nothing here knows what a house is: it
 * is given somewhere to put things and told what to put there, so a shop, a
 * warehouse or a lock-up is the same call with different numbers.
 *
 * What a place is worth is answered by whoever knows, on OnPlayerStartRobbery.
 */

#if !defined MAX_ROBBERY_LOOT
    #define MAX_ROBBERY_LOOT (8)
#endif

static
    Property:gRobberyProperty[MAX_PLAYERS] = { INVALID_PROPERTY_ID, ... },
    Item:gRobberyLoot[MAX_PLAYERS][MAX_ROBBERY_LOOT],
    gRobberyLootCount[MAX_PLAYERS]
;

/**
 * # Functions
 */

/**
 * @brief      Start turning a place over.
 *
 * OnPlayerStartRobbery is sent so that whatever knows the place can fill it
 * with AddPlayerRobberyLoot. Nothing being added means there was nothing
 * worth taking, and the robbery is rolled back as though it never started.
 *
 * @param      playerid    Character doing it.
 * @param      propertyid  Place being robbed.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the job is on and the loot is lying about
 *             - `false` when:
 *                 + they are already robbing somewhere
 *                 + `propertyid` is not a live property
 *                 + nobody added anything to take
 */
forward bool:StartPlayerRobbery(playerid, Property:propertyid);

/**
 * @brief      Put one thing where it can be taken from.
 *
 * Answered during OnPlayerStartRobbery, once per thing worth taking. The
 * item is made in the world behind the property's door, so two characters
 * robbing the same address never see each other's.
 *
 * @param      playerid  Character being robbed for.
 * @param      buildid   What to put there.
 * @param      x         Where it lies, inside the property.
 * @param      y         Where it lies, inside the property.
 * @param      z         Where it lies, inside the property.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is lying there
 *             - `false` when:
 *                 + they are not robbing anywhere
 *                 + there are already MAX_ROBBERY_LOOT things out
 *                 + the item could not be created
 */
forward bool:AddPlayerRobberyLoot(playerid, ItemBuild:buildid, Float:x, Float:y, Float:z);

/**
 * @brief      End a job.
 *
 * Anything still lying where it was put is destroyed. What the character
 * picked up and carried off is theirs and is not taken back.
 *
 * @param      playerid  Character to finish with.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the job is over and the loot is cleared away
 *             - `false` they were not robbing anywhere
 */
forward bool:StopPlayerRobbery(playerid);

/**
 * @brief      Where a character is robbing.
 *
 * @param      playerid  Character to ask about.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the property they are turning over
 *             - `INVALID_PROPERTY_ID` they are not robbing anywhere
 */
forward Property:GetPlayerRobbery(playerid);

/**
 * @brief      Whether a character is part way through a job.
 *
 * @param      playerid  Character to check.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` a job is on
 *             - `false` they are not robbing anywhere
 */
forward bool:IsPlayerRobbing(playerid);

/**
 * # Events
 */

/**
 * @brief      A job has started and the place is waiting to be filled.
 *
 * Answer it with AddPlayerRobberyLoot, once per thing worth taking. A place
 * nobody fills is a place with nothing in it, and the robbery is rolled back
 * as though it never started.
 *
 * Whether the place is the kind this listener knows about is the listener's
 * to ask: every one of them hears about every robbery.
 *
 * @param      playerid    Character robbing.
 * @param      propertyid  Place being robbed.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerStartRobbery(playerid, Property:propertyid);

/**
 * @brief      A job is over.
 *
 * Whatever was still lying where it was put has already gone; what the
 * character picked up and carried off is theirs.
 *
 * @param      playerid    Character who was robbing.
 * @param      propertyid  Place that was robbed.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerStopRobbery(playerid, Property:propertyid);

/**
 * # Internal
 */

// Only what is still lying where it was put; what was carried off is theirs.
static stock ClearRobberyLootInternal(playerid) {
    for (new i; i != gRobberyLootCount[playerid]; ++i) {
        new const
            Item:itemid = gRobberyLoot[playerid][i]
        ;

        gRobberyLoot[playerid][i] = INVALID_ITEM_ID;

        if (!IsValidItem(itemid)) {
            continue;
        }

        if (!IsItemInWorld(itemid)) {
            continue;
        }

        DestroyItem(itemid);
    }

    gRobberyLootCount[playerid] = 0;
}

/**
 * # External
 */

stock Property:GetPlayerRobbery(playerid) {
    return gRobberyProperty[playerid];
}

stock bool:IsPlayerRobbing(playerid) {
    return gRobberyProperty[playerid] != INVALID_PROPERTY_ID;
}

stock bool:StartPlayerRobbery(playerid, Property:propertyid) {
    if (IsPlayerRobbing(playerid)) {
        return false;
    }

    if (!IsValidProperty(propertyid)) {
        return false;
    }

    gRobberyProperty[playerid] = propertyid;
    gRobberyLootCount[playerid] = 0;

    CallLocalFunction("OnPlayerStartRobbery", "ii", playerid, _:propertyid);

    if (gRobberyLootCount[playerid] == 0) {
        gRobberyProperty[playerid] = INVALID_PROPERTY_ID;

        return false;
    }

    return true;
}

stock bool:AddPlayerRobberyLoot(playerid, ItemBuild:buildid, Float:x, Float:y, Float:z) {
    if (!IsPlayerRobbing(playerid)) {
        return false;
    }

    if (gRobberyLootCount[playerid] == MAX_ROBBERY_LOOT) {
        return false;
    }

    new const
        Item:itemid = CreateItem(buildid)
    ;

    if (itemid == INVALID_ITEM_ID) {
        return false;
    }

    new const
        Entrance:entranceid = GetPropertyEntrance(gRobberyProperty[playerid])
    ;

    CreateItemInWorld(
        itemid,
        x,
        y,
        z,
        0.0,
        0.0,
        0.0,
        ITEM_KEY_PICK_UP_ITEM,
        GetEntranceInteriorVirtualWorld(entranceid),
        GetEntranceInteriorInterior(entranceid)
    );

    gRobberyLoot[playerid][gRobberyLootCount[playerid]++] = itemid;

    return true;
}

stock bool:StopPlayerRobbery(playerid) {
    new const
        Property:propertyid = gRobberyProperty[playerid]
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        return false;
    }

    gRobberyProperty[playerid] = INVALID_PROPERTY_ID;

    ClearRobberyLootInternal(playerid);

    CallLocalFunction("OnPlayerStopRobbery", "ii", playerid, _:propertyid);

    return true;
}

/**
 * # Calls
 */

hook OnPlayerStartRobbery(playerid, Property:propertyid) {
    new
        name[MAX_PROPERTY_NAME]
    ;

    GetPropertyName(propertyid, name);

    SendPlayerNotice(playerid, "You start turning %s over. Take what you find to a vehicle.", name);

    return 0;
}

hook OnPlayerStopRobbery(playerid, Property:propertyid) {
    SendPlayerNotice(playerid, "The job is over. Anything still lying about is gone.");

    return 0;
}

hook OnPlayerConnect(playerid) {
    gRobberyProperty[playerid] = INVALID_PROPERTY_ID;
    gRobberyLootCount[playerid] = 0;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    StopPlayerRobbery(playerid);

    return 0;
}

/**
 * # Commands
 */

CMD:rob(playerid, params[]) {
    if (IsPlayerRobbing(playerid)) {
        SendPlayerNotice(playerid, "You are already turning somewhere over. /robend when you are done.");

        return 1;
    }

    if (GetPlayerLockup(playerid) == INVALID_LOCKUP_ID) {
        SendPlayerNotice(playerid, "You need a lock-up before you go taking things.");

        return 1;
    }

    new const
        Property:propertyid = GetPlayerProperty(playerid)
    ;

    if (propertyid == INVALID_PROPERTY_ID) {
        SendPlayerNotice(playerid, "You have to be inside somewhere to rob it.");

        return 1;
    }

    if (IsPlayerPropertyOwner(playerid, propertyid)) {
        SendPlayerNotice(playerid, "You cannot rob your own place.");

        return 1;
    }

    if (!StartPlayerRobbery(playerid, propertyid)) {
        SendPlayerNotice(playerid, "There is nothing worth taking here.");

        return 1;
    }

    return 1;
}

CMD:robend(playerid, params[]) {
    if (!StopPlayerRobbery(playerid)) {
        SendPlayerNotice(playerid, "You are not robbing anywhere.");

        return 1;
    }

    return 1;
}