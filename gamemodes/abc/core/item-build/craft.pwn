#if defined _CORE_ITEM_BUILD_CRAFT
    #endinput
#endif
#define _CORE_ITEM_BUILD_CRAFT

#include <pp-hooks>

/**
 * # Crafting
 *
 * A recipe is a result, how many of it come off one lot, and what one lot
 * costs. A run makes as many lots as the materials allow, spending and
 * delivering one at a time while the hold bar fills.
 *
 * Materials are read out of a container and results go back into it, so the
 * pockets, a bench and a machine are the same call with a different container.
 *
 * # Stations
 *
 * A recipe may want a kind of station -- a press, a stove -- and a container
 * may be one. The recipe names the kind and never the thing: it knows it needs
 * a press the way it knows it needs a screwdriver, and where that press stands
 * is nothing to do with it.
 *
 * A station is a workspace and not a shelf. What may be put into one is what
 * the recipes running there are made of, so it cannot quietly fill up with
 * somebody's spare clothes.
 */

#if !defined MAX_CRAFTS
    #define MAX_CRAFTS (Craft:64)
#endif

#if !defined MAX_CRAFT_INGREDIENTS
    #define MAX_CRAFT_INGREDIENTS (6)
#endif

// One lot on its own, and the ceiling every run is squeezed into.
#if !defined CRAFT_LOT_TIME
    #define CRAFT_LOT_TIME (3000)
#endif

#if !defined CRAFT_MAX_TIME
    #define CRAFT_MAX_TIME (30000)
#endif

#if !defined MAX_CRAFT_LOTS
    #define MAX_CRAFT_LOTS (500)
#endif

#if !defined MAX_CRAFT_STATIONS
    #define MAX_CRAFT_STATIONS (CraftStation:8)
#endif

#if !defined MAX_CRAFT_STATION_NAME
    #define MAX_CRAFT_STATION_NAME (24)
#endif

#define INVALID_CRAFT_ID (Craft:-1)
#define INVALID_CRAFT_STATION (CraftStation:-1)

static enum _:E_CRAFT_DATA {
    ItemBuild:E_CRAFT_RESULT,
    CraftStation:E_CRAFT_STATION,
    E_CRAFT_YIELD,
    E_CRAFT_TIME,
    E_CRAFT_SIZE
};

static
    gCraftData[MAX_CRAFTS][E_CRAFT_DATA],
    gCraftCount,

    ItemBuild:gCraftIngredientBuild[_:MAX_CRAFTS * MAX_CRAFT_INGREDIENTS],
    gCraftIngredientAmount[_:MAX_CRAFTS * MAX_CRAFT_INGREDIENTS],

    gCraftStationName[MAX_CRAFT_STATIONS][MAX_CRAFT_STATION_NAME],
    gCraftStationCount,

    CraftStation:gContainerStation[MAX_CONTAINERS] = { INVALID_CRAFT_STATION, ... }
;

static enum _:E_PLAYER_CRAFT_DATA {
    Craft:E_PLAYER_CRAFT_ID,
    Container:E_PLAYER_CRAFT_CONTAINER,
    E_PLAYER_CRAFT_LOTS,
    E_PLAYER_CRAFT_DONE,
    E_PLAYER_CRAFT_DURATION
};

static
    gPlayerCraftData[MAX_PLAYERS][E_PLAYER_CRAFT_DATA]
;

/**
 * # Functions
 */

/**
 * @brief      Write down a recipe.
 *
 * The ingredients follow as pairs of a build and how much of it one lot
 * costs. An amount of nothing is a tool: it has to be there for the recipe
 * to run and is never taken.
 *
 * Every lot takes CRAFT_LOT_TIME until SetCraftTime says otherwise.
 *
 * @param      resultid  What comes out.
 * @param      yield     How many of it one lot makes.
 * @param      ...       Pairs of build and amount, up to
 *                       MAX_CRAFT_INGREDIENTS of them.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the recipe, to be read or timed later
 *             - `INVALID_CRAFT_ID` when:
 *                 + `resultid` is not a registered build
 *                 + `yield` is zero or below
 *                 + the table is full
 *                 + the ingredients are not whole pairs, or there are none
 *                 + there are more than MAX_CRAFT_INGREDIENTS of them
 *                 + any ingredient is not a registered build
 *                 + any amount is below zero
 *                 + every amount is nothing, so a lot would cost nothing
 */
forward Craft:DefineItemCraft(ItemBuild:resultid, yield, {ItemBuild,_}:...);

/**
 * @brief      Say how long one lot of a recipe takes.
 *
 * A run is this times the number of lots, and never longer than
 * CRAFT_MAX_TIME however many there are.
 *
 * @param      craftid       Recipe to time.
 * @param      milliseconds  How long one lot takes.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the recipe takes that long
 *             - `false` when:
 *                 + `craftid` is not a registered recipe
 *                 + `milliseconds` is zero or below
 */
forward bool:SetCraftTime(Craft:craftid, milliseconds);

/**
 * @brief      Name a kind of station.
 *
 * A press, a stove, a bench. Recipes ask for the kind and containers are
 * given it, so where one stands and what it looks like is nothing to do
 * with either.
 *
 * @param      name  What it is called, for a message that has to say.
 *
 * @date       11:20 18/09/2026
 * @author     augustocaixeta
 *
 * @return     - the station kind
 *             - `INVALID_CRAFT_STATION` there are already
 *               MAX_CRAFT_STATIONS of them
 */
forward CraftStation:DefineCraftStation(const name[]);

/**
 * @brief      Say a recipe only runs at a kind of station.
 *
 * Without this a recipe runs anywhere, pockets included.
 *
 * @param      craftid    Recipe to tie down.
 * @param      stationid  Kind of station it needs.
 *
 * @date       11:20 18/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the recipe wants one
 *             - `false` when:
 *                 + `craftid` is not a registered recipe
 *                 + `stationid` is not a registered station kind
 */
forward bool:SetCraftStation(Craft:craftid, CraftStation:stationid);

/**
 * @brief      Whether a kind of station has been named.
 *
 * @param      stationid  Station kind to check.
 *
 * @date       11:20 18/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is a registered kind
 *             - `false` it is not
 */
forward bool:IsValidCraftStation(CraftStation:stationid);

/**
 * @brief      What kind of station a recipe needs.
 *
 * @param      craftid  Recipe to read.
 *
 * @date       11:20 18/09/2026
 * @author     augustocaixeta
 *
 * @return     - the kind it needs
 *             - `INVALID_CRAFT_STATION` it runs anywhere
 */
forward CraftStation:GetCraftStation(Craft:craftid);

/**
 * @brief      What a kind of station is called.
 *
 * @param      stationid  Station kind to read.
 * @param      output     Buffer the name is written into.
 * @param      size       Size of that buffer.
 *
 * @date       11:20 18/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` a name was written
 *             - `false` `output` is emptied, `stationid` is not a registered
 *               station kind
 */
forward bool:GetCraftStationName(CraftStation:stationid, output[], size = sizeof (output));

/**
 * @brief      The kind of station with a name.
 *
 * For anything that has to name one before the catalogue has defined it --
 * a lock-up knows its press is a press, and asks for it by that.
 *
 * @param      name  What it is called, matched whatever the case.
 *
 * @date       11:20 18/09/2026
 * @author     augustocaixeta
 *
 * @return     - the station kind
 *             - `INVALID_CRAFT_STATION` nothing is called that
 */
forward CraftStation:FindCraftStation(const name[]);

/**
 * @brief      Make a container a station of a kind.
 *
 * What it then is, is a workspace: only what the recipes running there are
 * made of may be put into it, though anything already in it comes back out.
 *
 * @param      containerid  Container to turn into one.
 * @param      stationid    Kind it becomes. `INVALID_CRAFT_STATION` makes it
 *                          an ordinary container again.
 *
 * @date       11:20 18/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the container is that kind of station
 *             - `false` when:
 *                 + `containerid` is not a live container
 *                 + `stationid` is neither a registered kind nor
 *                   `INVALID_CRAFT_STATION`
 */
forward bool:SetContainerCraftStation(Container:containerid, CraftStation:stationid);

/**
 * @brief      What kind of station a container is.
 *
 * @param      containerid  Container to ask.
 *
 * @date       11:20 18/09/2026
 * @author     augustocaixeta
 *
 * @return     - the kind it is
 *             - `INVALID_CRAFT_STATION` it is an ordinary container
 */
forward CraftStation:GetContainerCraftStation(Container:containerid);

/**
 * @brief      Whether a kind of item may be put into a station.
 *
 * True when some recipe running there is made of it. An ordinary container
 * says yes to everything, which is what keeps this answerable for any
 * container at all.
 *
 * A result is not an ingredient and answers no: one arrives in a station
 * because it was made there, not because somebody carried it in.
 *
 * @param      containerid  Container being put into.
 * @param      buildid      Kind of item going in.
 *
 * @date       11:20 18/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it belongs there
 *             - `false` the container is a station and nothing made there
 *               is made of that
 */
forward bool:CanPutBuildInCraftStation(Container:containerid, ItemBuild:buildid);

/**
 * @brief      Whether a recipe has been written down.
 *
 * @param      craftid  Recipe to check.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is a registered recipe
 *             - `false` it is not
 */
forward bool:IsValidCraft(Craft:craftid);

/**
 * @brief      How many recipes there are.
 *
 * They are numbered from nothing, so this is also one past the last of them.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many have been defined
 */
forward GetCraftCount();

/**
 * @brief      What a recipe makes.
 *
 * @param      craftid  Recipe to read.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the build it produces
 *             - `INVALID_ITEM_BUILD_ID` `craftid` is not a registered recipe
 */
forward ItemBuild:GetCraftResult(Craft:craftid);

/**
 * @brief      How many one lot of a recipe makes.
 *
 * @param      craftid  Recipe to read.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many come off one lot
 *             - `0` `craftid` is not a registered recipe
 */
forward GetCraftYield(Craft:craftid);

/**
 * @brief      How long one lot of a recipe takes, in milliseconds.
 *
 * @param      craftid  Recipe to read.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how long a lot takes
 *             - `0` `craftid` is not a registered recipe
 */
forward GetCraftTime(Craft:craftid);

/**
 * @brief      How many different things a recipe asks for.
 *
 * The number of ingredients rather than the amount of any of them, which is
 * what a loop over GetCraftIngredient counts to.
 *
 * @param      craftid  Recipe to read.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many ingredients it has
 *             - `0` `craftid` is not a registered recipe
 */
forward GetCraftIngredientCount(Craft:craftid);

/**
 * @brief      One ingredient of a recipe, and what a lot of it costs.
 *
 * An amount of nothing means the build is a tool: wanted, and not taken.
 *
 * @param      craftid  Recipe to read.
 * @param      index    Which ingredient, from nothing upwards.
 * @param      buildid  Written with the build it asks for.
 * @param      amount   Written with how much of it one lot costs.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` both were written
 *             - `false` `buildid` and `amount` are emptied, when:
 *                 + `craftid` is not a registered recipe
 *                 + it has no ingredient at `index`
 */
forward bool:GetCraftIngredient(Craft:craftid, index, &ItemBuild:buildid, &amount);

/**
 * @brief      A recipe that makes a kind of item.
 *
 * The first one written down for it, so a build made by two recipes answers
 * whichever was defined first.
 *
 * @param      resultid  Build to look for.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the recipe that makes it
 *             - `INVALID_CRAFT_ID` nothing makes it
 */
forward Craft:FindCraft(ItemBuild:resultid);

/**
 * @brief      How much of a kind of item a container holds.
 *
 * Counted in what the item is worth rather than in slots: a stack answers
 * its number and a box of ammunition its rounds, so five slots of ten metal
 * are fifty.
 *
 * @param      containerid  Container to count.
 * @param      buildid      Build to count.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how much of it is in there
 *             - `0` when:
 *                 + `containerid` is not a live container
 *                 + `buildid` is not a registered build
 *                 + there is none of it
 */
forward CountContainerItemBuild(Container:containerid, ItemBuild:buildid);

/**
 * @brief      How many lots of a recipe a container could make.
 *
 * The scarcest ingredient decides: each is divided by what a lot of it
 * costs, and the smallest answer is the number of lots. Tools are asked
 * about and left out of the arithmetic.
 *
 * @param      containerid  Where the materials are.
 * @param      craftid      Recipe to measure.
 * @param      limit        Most to answer, for asking whether a set number is
 *                          possible. Below zero is as many as there are.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many lots could be made, up to MAX_CRAFT_LOTS
 *             - `0` when:
 *                 + `craftid` is not a registered recipe
 *                 + `containerid` is not a live container
 *                 + a tool is missing, or a material has run out
 */
forward CountCraftLots(Container:containerid, Craft:craftid, limit = -1);

/**
 * @brief      Start making something.
 *
 * The hold bar runs for as long as the lots take, and each one is paid for
 * and delivered as the bar reaches it. Letting go or being interrupted keeps
 * the lots that finished and leaves the rest of the materials alone.
 *
 * Both the materials and the results are the container's: a run started
 * against the pockets empties and fills the pockets, and one started against
 * a bench empties and fills the bench.
 *
 * @param      playerid     Character doing it.
 * @param      craftid      Recipe to make.
 * @param      containerid  Where the materials are and the results go.
 * @param      lots         How many to make. Below zero is as many as the
 *                          materials allow.
 * @param      key          Key that must stay down for it to carry on.
 *                          `KEY_NONE` runs on its own.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the run has started
 *             - `false` when:
 *                 + `playerid` is not connected
 *                 + `craftid` is not a registered recipe
 *                 + `containerid` is not a live container
 *                 + there is not enough for a single lot
 *                 + OnPlayerStartCraft refused it
 *                 + they are doing something the bar will not be taken from
 */
forward bool:StartPlayerCraft(playerid, Craft:craftid, Container:containerid, lots = -1, KEY:key = KEY_NONE);

/**
 * @brief      Give up on a run part way through.
 *
 * What was already made is kept, the same as letting go of the key.
 *
 * @param      playerid  Character to stop.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` a run was stopped
 *             - `false` they were not making anything
 */
forward bool:StopPlayerCraft(playerid);

/**
 * @brief      Whether a character is part way through a run.
 *
 * @param      playerid  Character to check.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` a run is going
 *             - `false` they are not making anything
 */
forward bool:IsPlayerCrafting(playerid);

/**
 * @brief      What a character is making.
 *
 * @param      playerid  Character to ask.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the recipe they are running
 *             - `INVALID_CRAFT_ID` they are not making anything
 */
forward Craft:GetPlayerCraft(playerid);

/**
 * @brief      Where a character is making it.
 *
 * @param      playerid  Character to ask.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the container the run is taking from and filling
 *             - `INVALID_CONTAINER_ID` they are not making anything
 */
forward Container:GetPlayerCraftContainer(playerid);

/**
 * @brief      How many lots a character's run is for.
 *
 * Settled when the run started, so materials turning up half way through do
 * not make it longer.
 *
 * @param      playerid  Character to ask.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many lots it will make
 *             - `0` they are not making anything
 */
forward GetPlayerCraftLots(playerid);

/**
 * @brief      How many lots have come off so far.
 *
 * @param      playerid  Character to ask.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many are finished and paid for
 *             - `0` they are not making anything, or none has finished yet
 */
forward GetPlayerCraftDone(playerid);

/**
 * # Events
 */

/**
 * @brief      A character is about to start making something.
 *
 * Nothing has been taken and no bar is up yet. How many lots the run is for
 * is already settled, so this is the last chance to stop it.
 *
 * @param      playerid  Character starting.
 * @param      craftid   Recipe they are about to make.
 * @param      lots      How many lots the materials allow.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` let it start
 *             - non-zero refuse it, and nothing happens at all
 */
forward OnPlayerStartCraft(playerid, Craft:craftid, lots);

/**
 * @brief      One lot is finished.
 *
 * Its materials are gone and what it made is already in the container.
 *
 * @param      playerid  Character making it.
 * @param      craftid   Recipe being made.
 * @param      lot       Which lot it was, counting from one.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerCraftLot(playerid, Craft:craftid, lot);

/**
 * @brief      A run is over, however it ended.
 *
 * Finished, let go of, or pushed off the bar by something that outranked
 * it. The character is no longer crafting by the time this is called.
 *
 * @param      playerid  Character who was making it.
 * @param      craftid   Recipe that was being made.
 * @param      lots      How many were made, which is nought for a run given
 *                       up before the first one came off.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerFinishCraft(playerid, Craft:craftid, lots);

/**
 * # Internal
 */

static stock CraftItemUnitsInternal(Item:itemid) {
    if (IsItemAmmunition(itemid)) {
        return GetItemAmmunitionRounds(itemid);
    }

    if (IsItemStackable(itemid)) {
        return GetItemAmountInt(itemid);
    }

    return 1;
}

static stock CraftBuildLimitInternal(ItemBuild:buildid) {
    if (IsItemBuildAmmunition(buildid)) {
        return GetItemBuildAmmunitionRounds(buildid);
    }

    new const
        limit = GetItemBuildStackLimit(buildid)
    ;

    return limit ? limit : 1;
}

static stock CraftSetItemUnitsInternal(Item:itemid, units) {
    if (IsItemAmmunition(itemid)) {
        SetItemAmmunitionRounds(itemid, units);

        return;
    }

    if (IsItemStackable(itemid)) {
        SetItemAmountInt(itemid, units);
    }
}

// `emptied` says the item has nothing left in it, which for something that
// carries no number of its own is being drawn from at all.
static stock CraftDrawItemInternal(Item:itemid, units, &bool:emptied) {
    new const
        held = CraftItemUnitsInternal(itemid),
        drawn = (units < held) ? units : held
    ;

    CraftSetItemUnitsInternal(itemid, held - drawn);

    emptied = ((held - drawn) <= 0);

    return drawn;
}

static stock CraftFillItemInternal(Item:itemid, units) {
    new const
        held = CraftItemUnitsInternal(itemid),
        room = CraftBuildLimitInternal(GetItemBuild(itemid)) - held
    ;

    if (room <= 0) {
        return 0;
    }

    new const
        moved = (units < room) ? units : room
    ;

    CraftSetItemUnitsInternal(itemid, held + moved);

    return moved;
}

static stock TakeContainerItemBuildInternal(playerid, Container:containerid, ItemBuild:buildid, units) {
    new
        taken
    ;

    new const
        capacity = GetContainerCapacity(containerid)
    ;

    for (new i; i != capacity && taken < units; ++i) {
        new const
            Item:itemid = GetContainerSlotItem(containerid, i)
        ;

        if (itemid == INVALID_ITEM_ID) {
            continue;
        }

        if (GetItemBuild(itemid) != buildid) {
            continue;
        }

        new
            bool:emptied = false
        ;

        taken += CraftDrawItemInternal(itemid, units - taken, emptied);

        if (!emptied) {
            continue;
        }

        RemoveItemFromContainer(containerid, i, .playerid = playerid);
        DestroyItem(itemid);
    }

    return taken;
}

// Room in the ones already there, and a full one for every empty slot.
static stock CraftResultRoomInternal(Container:containerid, ItemBuild:buildid) {
    new
        room,
        free
    ;

    new const
        capacity = GetContainerCapacity(containerid),
        limit = CraftBuildLimitInternal(buildid)
    ;

    for (new i; i != capacity; ++i) {
        new const
            Item:itemid = GetContainerSlotItem(containerid, i)
        ;

        if (itemid == INVALID_ITEM_ID) {
            ++free;

            continue;
        }

        if (GetItemBuild(itemid) != buildid) {
            continue;
        }

        room += limit - CraftItemUnitsInternal(itemid);
    }

    return room + free * limit;
}

static stock GiveCraftResultInternal(playerid, Container:containerid, ItemBuild:buildid, units) {
    new const
        capacity = GetContainerCapacity(containerid)
    ;

    for (new i; i != capacity && units > 0; ++i) {
        new const
            Item:itemid = GetContainerSlotItem(containerid, i)
        ;

        if (itemid == INVALID_ITEM_ID) {
            continue;
        }

        if (GetItemBuild(itemid) != buildid) {
            continue;
        }

        units -= CraftFillItemInternal(itemid, units);
    }

    while (units > 0) {
        new const
            Item:itemid = CreateItem(buildid)
        ;

        if (itemid == INVALID_ITEM_ID) {
            return units;
        }

        new const
            limit = CraftBuildLimitInternal(buildid),
            moved = (units < limit) ? units : limit
        ;

        CraftSetItemUnitsInternal(itemid, moved);

        if (!AddItemToContainer(containerid, itemid, .playerid = playerid)) {
            DestroyItem(itemid);

            return units;
        }

        units -= moved;
    }

    return units;
}

static stock bool:MakeCraftLotInternal(playerid, Craft:craftid, Container:containerid) {
    if (CountCraftLots(containerid, craftid, 1) < 1) {
        return false;
    }

    new const
        ItemBuild:resultid = gCraftData[craftid][E_CRAFT_RESULT],
        yield = gCraftData[craftid][E_CRAFT_YIELD]
    ;

    if (CraftResultRoomInternal(containerid, resultid) < yield) {
        return false;
    }

    new const
        size = gCraftData[craftid][E_CRAFT_SIZE]
    ;

    for (new i; i != size; ++i) {
        new const
            base = _:craftid * MAX_CRAFT_INGREDIENTS + i,
            amount = gCraftIngredientAmount[base]
        ;

        if (amount == 0) {
            continue;
        }

        TakeContainerItemBuildInternal(playerid, containerid, gCraftIngredientBuild[base], amount);
    }

    new const
        left = GiveCraftResultInternal(playerid, containerid, resultid, yield)
    ;

    // Numbers changed on items that never moved, which nothing else redraws.
    RefreshContainerViewers(containerid);

    return (left == 0);
}

static stock ClearPlayerCraftInternal(playerid) {
    gPlayerCraftData[playerid][E_PLAYER_CRAFT_ID]        = INVALID_CRAFT_ID;
    gPlayerCraftData[playerid][E_PLAYER_CRAFT_CONTAINER] = INVALID_CONTAINER_ID;
    gPlayerCraftData[playerid][E_PLAYER_CRAFT_LOTS]      = 0;
    gPlayerCraftData[playerid][E_PLAYER_CRAFT_DONE]      = 0;
    gPlayerCraftData[playerid][E_PLAYER_CRAFT_DURATION]  = 0;
}

static stock EndCraftInternal(playerid) {
    new const
        Craft:craftid = gPlayerCraftData[playerid][E_PLAYER_CRAFT_ID],
        done = gPlayerCraftData[playerid][E_PLAYER_CRAFT_DONE]
    ;

    if (craftid == INVALID_CRAFT_ID) {
        return;
    }

    ClearPlayerCraftInternal(playerid);

    CallLocalFunction("OnPlayerFinishCraft", "iii", playerid, _:craftid, done);
}

static stock AdvanceCraftInternal(playerid, target) {
    new const
        Craft:craftid = gPlayerCraftData[playerid][E_PLAYER_CRAFT_ID],
        Container:containerid = gPlayerCraftData[playerid][E_PLAYER_CRAFT_CONTAINER],
        lots = gPlayerCraftData[playerid][E_PLAYER_CRAFT_LOTS]
    ;

    if (target > lots) {
        target = lots;
    }

    while (gPlayerCraftData[playerid][E_PLAYER_CRAFT_DONE] < target) {
        if (!MakeCraftLotInternal(playerid, craftid, containerid)) {
            StopHoldAction(playerid);

            return;
        }

        ++gPlayerCraftData[playerid][E_PLAYER_CRAFT_DONE];

        CallLocalFunction("OnPlayerCraftLot", "iii", playerid, _:craftid, gPlayerCraftData[playerid][E_PLAYER_CRAFT_DONE]);
    }
}

/**
 * # External
 */

stock Craft:DefineItemCraft(ItemBuild:resultid, yield, {ItemBuild,_}:...) {
    if (!IsValidItemBuild(resultid)) {
        return INVALID_CRAFT_ID;
    }

    if (yield <= 0) {
        return INVALID_CRAFT_ID;
    }

    if (gCraftCount == _:MAX_CRAFTS) {
        return INVALID_CRAFT_ID;
    }

    new const
        given = numargs() - 2
    ;

    if (given < 2 || (given % 2) != 0) {
        return INVALID_CRAFT_ID;
    }

    new const
        size = given / 2,
        Craft:craftid = Craft:gCraftCount
    ;

    if (size > MAX_CRAFT_INGREDIENTS) {
        return INVALID_CRAFT_ID;
    }

    new
        cost
    ;

    for (new i; i != size; ++i) {
        new const
            base = _:craftid * MAX_CRAFT_INGREDIENTS + i,
            ItemBuild:buildid = ItemBuild:getarg(2 + i * 2),
            amount = getarg(3 + i * 2)
        ;

        if (!IsValidItemBuild(buildid)) {
            return INVALID_CRAFT_ID;
        }

        if (amount < 0) {
            return INVALID_CRAFT_ID;
        }

        cost += amount;

        gCraftIngredientBuild[base] = buildid;
        gCraftIngredientAmount[base] = amount;
    }

    if (cost == 0) {
        return INVALID_CRAFT_ID;
    }

    ++gCraftCount;

    gCraftData[craftid][E_CRAFT_RESULT]  = resultid;
    gCraftData[craftid][E_CRAFT_STATION] = INVALID_CRAFT_STATION;
    gCraftData[craftid][E_CRAFT_YIELD]   = yield;
    gCraftData[craftid][E_CRAFT_TIME]    = CRAFT_LOT_TIME;
    gCraftData[craftid][E_CRAFT_SIZE]    = size;

    return craftid;
}

stock bool:SetCraftTime(Craft:craftid, milliseconds) {
    if (!IsValidCraft(craftid)) {
        return false;
    }

    if (milliseconds <= 0) {
        return false;
    }

    gCraftData[craftid][E_CRAFT_TIME] = milliseconds;

    return true;
}

stock CraftStation:DefineCraftStation(const name[]) {
    if (gCraftStationCount == _:MAX_CRAFT_STATIONS) {
        return INVALID_CRAFT_STATION;
    }

    new const
        CraftStation:stationid = CraftStation:gCraftStationCount++
    ;

    strcopy(gCraftStationName[stationid], name);

    return stationid;
}

stock bool:IsValidCraftStation(CraftStation:stationid) {
    return (0 <= _:stationid < gCraftStationCount);
}

stock bool:SetCraftStation(Craft:craftid, CraftStation:stationid) {
    if (!IsValidCraft(craftid)) {
        return false;
    }

    if (!IsValidCraftStation(stationid)) {
        return false;
    }

    gCraftData[craftid][E_CRAFT_STATION] = stationid;

    return true;
}

stock CraftStation:GetCraftStation(Craft:craftid) {
    if (!IsValidCraft(craftid)) {
        return INVALID_CRAFT_STATION;
    }

    return gCraftData[craftid][E_CRAFT_STATION];
}

stock bool:GetCraftStationName(CraftStation:stationid, output[], size = sizeof (output)) {
    output[0] = EOS;

    if (!IsValidCraftStation(stationid)) {
        return false;
    }

    strcopy(output, gCraftStationName[stationid], size);

    return true;
}

stock CraftStation:FindCraftStation(const name[]) {
    for (new CraftStation:stationid; _:stationid != gCraftStationCount; ++stationid) {
        if (!strcmp(gCraftStationName[stationid], name, true)) {
            return stationid;
        }
    }

    return INVALID_CRAFT_STATION;
}

stock bool:SetContainerCraftStation(Container:containerid, CraftStation:stationid) {
    if (!IsValidContainer(containerid)) {
        return false;
    }

    if (stationid != INVALID_CRAFT_STATION && !IsValidCraftStation(stationid)) {
        return false;
    }

    gContainerStation[containerid] = stationid;

    return true;
}

stock CraftStation:GetContainerCraftStation(Container:containerid) {
    if (!IsValidContainer(containerid)) {
        return INVALID_CRAFT_STATION;
    }

    return gContainerStation[containerid];
}

stock bool:CanPutBuildInCraftStation(Container:containerid, ItemBuild:buildid) {
    new const
        CraftStation:stationid = GetContainerCraftStation(containerid)
    ;

    if (stationid == INVALID_CRAFT_STATION) {
        return true;
    }

    for (new Craft:craftid; _:craftid != gCraftCount; ++craftid) {
        if (gCraftData[craftid][E_CRAFT_STATION] != stationid) {
            continue;
        }

        for (new i, size = gCraftData[craftid][E_CRAFT_SIZE]; i != size; ++i) {
            if (gCraftIngredientBuild[_:craftid * MAX_CRAFT_INGREDIENTS + i] == buildid) {
                return true;
            }
        }
    }

    return false;
}

stock bool:IsValidCraft(Craft:craftid) {
    return (0 <= _:craftid < gCraftCount);
}

stock GetCraftCount() {
    return gCraftCount;
}

stock ItemBuild:GetCraftResult(Craft:craftid) {
    if (!IsValidCraft(craftid)) {
        return INVALID_ITEM_BUILD_ID;
    }

    return gCraftData[craftid][E_CRAFT_RESULT];
}

stock GetCraftYield(Craft:craftid) {
    if (!IsValidCraft(craftid)) {
        return 0;
    }

    return gCraftData[craftid][E_CRAFT_YIELD];
}

stock GetCraftTime(Craft:craftid) {
    if (!IsValidCraft(craftid)) {
        return 0;
    }

    return gCraftData[craftid][E_CRAFT_TIME];
}

stock GetCraftIngredientCount(Craft:craftid) {
    if (!IsValidCraft(craftid)) {
        return 0;
    }

    return gCraftData[craftid][E_CRAFT_SIZE];
}

stock bool:GetCraftIngredient(Craft:craftid, index, &ItemBuild:buildid, &amount) {
    buildid = INVALID_ITEM_BUILD_ID;
    amount = 0;

    if (!IsValidCraft(craftid)) {
        return false;
    }

    if (!(0 <= index < gCraftData[craftid][E_CRAFT_SIZE])) {
        return false;
    }

    new const
        base = _:craftid * MAX_CRAFT_INGREDIENTS + index
    ;

    buildid = gCraftIngredientBuild[base];
    amount = gCraftIngredientAmount[base];

    return true;
}

stock Craft:FindCraft(ItemBuild:resultid) {
    for (new Craft:craftid; _:craftid != gCraftCount; ++craftid) {
        if (gCraftData[craftid][E_CRAFT_RESULT] == resultid) {
            return craftid;
        }
    }

    return INVALID_CRAFT_ID;
}

stock CountContainerItemBuild(Container:containerid, ItemBuild:buildid) {
    if (!IsValidContainer(containerid)) {
        return 0;
    }

    if (!IsValidItemBuild(buildid)) {
        return 0;
    }

    new
        units
    ;

    new const
        capacity = GetContainerCapacity(containerid)
    ;

    for (new i; i != capacity; ++i) {
        new const
            Item:itemid = GetContainerSlotItem(containerid, i)
        ;

        if (itemid == INVALID_ITEM_ID) {
            continue;
        }

        if (GetItemBuild(itemid) != buildid) {
            continue;
        }

        units += CraftItemUnitsInternal(itemid);
    }

    return units;
}

stock CountCraftLots(Container:containerid, Craft:craftid, limit = -1) {
    if (!IsValidCraft(craftid)) {
        return 0;
    }

    if (!IsValidContainer(containerid)) {
        return 0;
    }

    new
        lots = (limit < 0) ? MAX_CRAFT_LOTS : limit
    ;

    if (lots > MAX_CRAFT_LOTS) {
        lots = MAX_CRAFT_LOTS;
    }

    if (lots <= 0) {
        return 0;
    }

    new const
        size = gCraftData[craftid][E_CRAFT_SIZE]
    ;

    for (new i; i != size; ++i) {
        new const
            base = _:craftid * MAX_CRAFT_INGREDIENTS + i,
            amount = gCraftIngredientAmount[base],
            held = CountContainerItemBuild(containerid, gCraftIngredientBuild[base])
        ;

        if (amount == 0) {
            if (held == 0) {
                return 0;
            }

            continue;
        }

        new const
            possible = held / amount
        ;

        if (possible < lots) {
            lots = possible;
        }

        if (lots == 0) {
            return 0;
        }
    }

    return lots;
}

stock bool:StartPlayerCraft(playerid, Craft:craftid, Container:containerid, lots = -1, KEY:key = KEY_NONE) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!IsValidCraft(craftid)) {
        return false;
    }

    if (!IsValidContainer(containerid)) {
        return false;
    }

    if (gCraftData[craftid][E_CRAFT_STATION] != INVALID_CRAFT_STATION) {
        if (GetContainerCraftStation(containerid) != gCraftData[craftid][E_CRAFT_STATION]) {
            return false;
        }
    }

    lots = CountCraftLots(containerid, craftid, lots);

    if (lots <= 0) {
        return false;
    }

    if (CallLocalFunction("OnPlayerStartCraft", "iii", playerid, _:craftid, lots)) {
        return false;
    }

    new
        duration = lots * gCraftData[craftid][E_CRAFT_TIME]
    ;

    if (duration > CRAFT_MAX_TIME) {
        duration = CRAFT_MAX_TIME;
    }

    new
        title[MAX_ITEM_BUILD_NAME]
    ;

    GetItemBuildName(gCraftData[craftid][E_CRAFT_RESULT], title);

    // Written after the bar is up: claiming it stops whatever had it, and that
    // stop is answered below by clearing exactly this.
    if (!StartHoldAction(playerid, duration, title, key, HOLD_PRIORITY_CRAFT)) {
        return false;
    }

    gPlayerCraftData[playerid][E_PLAYER_CRAFT_ID]        = craftid;
    gPlayerCraftData[playerid][E_PLAYER_CRAFT_CONTAINER] = containerid;
    gPlayerCraftData[playerid][E_PLAYER_CRAFT_LOTS]      = lots;
    gPlayerCraftData[playerid][E_PLAYER_CRAFT_DONE]      = 0;
    gPlayerCraftData[playerid][E_PLAYER_CRAFT_DURATION]  = duration;

    return true;
}

stock bool:StopPlayerCraft(playerid) {
    if (!IsPlayerCrafting(playerid)) {
        return false;
    }

    return StopHoldAction(playerid);
}

stock bool:IsPlayerCrafting(playerid) {
    return (gPlayerCraftData[playerid][E_PLAYER_CRAFT_ID] != INVALID_CRAFT_ID);
}

stock Craft:GetPlayerCraft(playerid) {
    return gPlayerCraftData[playerid][E_PLAYER_CRAFT_ID];
}

stock Container:GetPlayerCraftContainer(playerid) {
    return gPlayerCraftData[playerid][E_PLAYER_CRAFT_CONTAINER];
}

stock GetPlayerCraftLots(playerid) {
    return gPlayerCraftData[playerid][E_PLAYER_CRAFT_LOTS];
}

stock GetPlayerCraftDone(playerid) {
    return gPlayerCraftData[playerid][E_PLAYER_CRAFT_DONE];
}

/**
 * # Calls
 */

hook OnContainerCreate(Container:containerid) {
    gContainerStation[containerid] = INVALID_CRAFT_STATION;

    return 0;
}

hook OnPlayerConnect(playerid) {
    ClearPlayerCraftInternal(playerid);

    return 0;
}

hook OnHoldActionUpdate(playerid, progress) {
    if (!IsPlayerCrafting(playerid)) {
        return 0;
    }

    AdvanceCraftInternal(playerid, (gPlayerCraftData[playerid][E_PLAYER_CRAFT_LOTS] * progress) / gPlayerCraftData[playerid][E_PLAYER_CRAFT_DURATION]);

    return 0;
}

hook OnHoldActionFinish(playerid) {
    if (!IsPlayerCrafting(playerid)) {
        return 0;
    }

    AdvanceCraftInternal(playerid, gPlayerCraftData[playerid][E_PLAYER_CRAFT_LOTS]);
    EndCraftInternal(playerid);

    return 0;
}

hook OnHoldActionStop(playerid, progress) {
    EndCraftInternal(playerid);

    return 0;
}
