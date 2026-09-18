#define MAX_PLAYERS (2)

// pp-menu hands its answer back through a PawnPlus task, which is read with
// await inside a function that yields. Both keywords have to be asked for
// before the include that pulls PawnPlus in, and pp-menu is that include: it
// brings pp-hooks, and pp-hooks brings PawnPlus.
#define PP_SYNTAX_AWAIT
#define PP_SYNTAX_YIELD

#include <open.mp>

#include <pp-menu>
#include <components>

// Everything the modshop says to the player goes out as GAME_TEXT_STYLE_STUNT.
// It is the style that sits clear of the list; POPUP is drawn over the middle
// of the screen, which is exactly where the menu is, so it would be talking to
// the player by covering up what they were reading.
#include <extended-text-styles>

// y_commands and sscanf2 are gone from this example. y_commands pulls in
// y_hooks and y_foreach, which redefine `hook` and `yield` on top of pp-hooks
// and PawnPlus -- pp-hooks refuses outright with `#error Not compatible with
// y_hooks` when YSI gets there first, and silently breaks `yield` when it does
// not. race-example and ammu-list-menu both take commands the plain way for
// the same reason, and one model id does not need a parser.

#define MODSHOP_SOUND_PURCHASED (1133)
#define MODSHOP_SOUND_DENIED (1055)

static
    gVehicleID[MAX_PLAYERS] = { INVALID_VEHICLE_ID, ... },
    gVehicleComponentID[MAX_VEHICLES][MAX_COMPONENT_TYPES] = { { INVALID_VEHICLE_COMPONENT_ID, ... }, ... }
;

// What each row of the menu on screen stands for. The old menu closed over this
// with `using inline`, which a task cannot do: OnPlayerListMenuSelectionChange
// arrives as a plain callback with a row number and nothing else, so the row
// has to be turned back into a component through something the whole file can
// see.
static
    gMenuItemValue[MAX_PLAYERS][MAX_MENU_ITEMS]
;

// The component hanging on the car only to be looked at. E_PREVIEW_ORIGINAL is
// what was bolted on before the player started walking the list, so backing out
// of the menu can put it back; E_PREVIEW_CURRENT is what is on there now, so
// the next row takes the last one off instead of stacking on top of it.
static enum _:E_PREVIEW_DATA {
    E_PREVIEW_VEHICLE_ID,
    E_PREVIEW_TYPE,
    E_PREVIEW_ORIGINAL,
    E_PREVIEW_CURRENT
};

static
    gPreview[MAX_PLAYERS][E_PREVIEW_DATA]
;

/**
 * # Events
 */

forward OnComponentAddToVehicle(vehicleid, componentid);

main(){}

/**
 * # Components
 */

AddComponentToVehicle(vehicleid, componentid) {
    if (!(0 <= (componentid - 1000) < MAX_VEHICLE_COMPONENTS)) {
        return;
    }

    new const
        linked = GetVehicleComponentPair(componentid)
    ;

    if (linked != INVALID_VEHICLE_COMPONENT_ID) {
        AddVehicleComponent(vehicleid, linked);
    }

    AddVehicleComponent(vehicleid, componentid);
}

RemoveComponentFromVehicle(vehicleid, componentid) {
    if (!(0 <= (componentid - 1000) < MAX_VEHICLE_COMPONENTS)) {
        return;
    }

    new const
        linked = GetVehicleComponentPair(componentid)
    ;

    if (linked != INVALID_VEHICLE_COMPONENT_ID) {
        RemoveVehicleComponent(vehicleid, linked);
    }

    RemoveVehicleComponent(vehicleid, componentid);
}

/**
 * # Preview
 */

ApplyPreview(playerid, componentid) {
    new const
        vehicleid = gPreview[playerid][E_PREVIEW_VEHICLE_ID]
    ;

    if (vehicleid == INVALID_VEHICLE_ID) {
        return;
    }

    new
        showing = gPreview[playerid][E_PREVIEW_CURRENT]
    ;

    if (showing == INVALID_VEHICLE_COMPONENT_ID) {
        showing = gPreview[playerid][E_PREVIEW_ORIGINAL];
    }

    RemoveComponentFromVehicle(vehicleid, showing);
    AddComponentToVehicle(vehicleid, componentid);

    gPreview[playerid][E_PREVIEW_CURRENT] = componentid;
}

StartPreview(playerid, vehicleid, type, componentid) {
    gPreview[playerid][E_PREVIEW_VEHICLE_ID] = vehicleid;
    gPreview[playerid][E_PREVIEW_TYPE]       = type;
    gPreview[playerid][E_PREVIEW_ORIGINAL]   = gVehicleComponentID[vehicleid][type];
    gPreview[playerid][E_PREVIEW_CURRENT]    = INVALID_VEHICLE_COMPONENT_ID;

    ApplyPreview(playerid, componentid);
}

StopPreview(playerid, bool:keep) {
    new const
        vehicleid = gPreview[playerid][E_PREVIEW_VEHICLE_ID]
    ;

    gPreview[playerid][E_PREVIEW_VEHICLE_ID] = INVALID_VEHICLE_ID;

    if (vehicleid == INVALID_VEHICLE_ID) {
        return;
    }

    if (keep) {
        return;
    }

    RemoveComponentFromVehicle(vehicleid, gPreview[playerid][E_PREVIEW_CURRENT]);
    AddComponentToVehicle(vehicleid, gPreview[playerid][E_PREVIEW_ORIGINAL]);
}

/**
 * # Menus
 */

ShowUpgradeMenu(playerid, vehicleid) {
    yield 1;

    new const
        modelid = GetVehicleModel(vehicleid)
    ;

    if (!modelid) {
        return;
    }

    new
        count,
        bool:contains[MAX_COMPONENT_TYPES],
        name[MAX_COMPONENT_TYPE_NAME]
    ;

    AddListMenuItem(playerid, 0, "Colors");
    gMenuItemValue[playerid][count++] = COMPONENT_TYPE_COLORS;

    for (new componentid = 1000, size = MAX_VEHICLE_COMPONENTS + 1000, item; componentid != size; ++componentid) {
        item = GetVehicleComponentTypeID(componentid);

        if (item == COMPONENT_TYPE_NONE) {
            continue;
        }

        if (contains[item]) {
            continue;
        }

        if (!CanVehicleModelHaveComponent(modelid, componentid)) {
            continue;
        }

        contains[item] = true;
    }

    for (new i; i != MAX_COMPONENT_TYPES; ++i) {
        if (!contains[i]) {
            continue;
        }

        GetVehicleComponentTypeName(i, name);

        AddListMenuItem(playerid, 0, name);

        gMenuItemValue[playerid][count++] = i;
    }

    new const
        Task:t = ShowAsyncListMenu(playerid, "Upgrades", 20.0, 100.0, 200.0)
    ;

    if (!t) {
        return;
    }

    new responses[E_ASYNC_MENU_DATA];
    await_arr(responses) t;

    if (!responses[E_ASYNC_MENU_RESPONSE]) {
        return;
    }

    new const
        item = gMenuItemValue[playerid][responses[E_ASYNC_MENU_LISTITEM]]
    ;

    if (item == COMPONENT_TYPE_COLORS) {
        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, 2000, "Colors");

        return;
    }

    if (item == COMPONENT_TYPE_PAINT_JOBS) {
        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, 2000, "Paint Jobs");

        return;
    }

    ShowUpgradeComponentsMenu(playerid, vehicleid, item);
}

ShowUpgradeComponentsMenu(playerid, vehicleid, item) {
    yield 1;

    new const
        modelid = GetVehicleModel(vehicleid)
    ;

    new
        count,
        name[MAX_VEHICLE_COMPONENT_NAME],
        componentid
    ;

    for (new i; i != MAX_VEHICLE_COMPONENTS; ++i) {
        componentid = i + 1000;

        if (!CanVehicleModelHaveComponent(modelid, componentid)) {
            continue;
        }

        if (GetVehicleComponentTypeID(componentid) != item) {
            continue;
        }

        GetVehicleComponentName(componentid, name);

        AddListMenuItem(playerid, 0, name);

        gMenuItemValue[playerid][count++] = componentid;
    }

    if (!count) {
        ShowUpgradeMenu(playerid, vehicleid);

        return;
    }

    // The first row is previewed here rather than waiting on a callback: the
    // menu is not on screen yet, and this is the one row the script already
    // knows it put at the top.
    StartPreview(playerid, vehicleid, item, gMenuItemValue[playerid][0]);

    new const
        Task:t = ShowAsyncListMenu(playerid, "Upgrades", 20.0, 100.0, 200.0)
    ;

    if (!t) {
        StopPreview(playerid, false);

        return;
    }

    new responses[E_ASYNC_MENU_DATA];
    await_arr(responses) t;

    // Walking the list is what hung the part on the car, so leaving the list is
    // what has to decide whether it stays there.
    if (!responses[E_ASYNC_MENU_RESPONSE]) {
        StopPreview(playerid, false);
        ShowUpgradeMenu(playerid, vehicleid);

        return;
    }

    new const
        chosen = gMenuItemValue[playerid][responses[E_ASYNC_MENU_LISTITEM]]
    ;

    StopPreview(playerid, true);
    ShowUpgradeComponentContinue(playerid, chosen, vehicleid, item);
}

ShowUpgradeComponentContinue(playerid, componentid, vehicleid, item) {
    yield 1;

    new
        name[MAX_VEHICLE_COMPONENT_NAME],
        cost[MAX_MENU_ITEM_LENGTH]
    ;

    GetVehicleComponentName(componentid, name);

    new const
        price = GetVehicleComponentCost(componentid)
    ;

    format(cost, sizeof (cost), "$%i", price);

    AddListMenuItem(playerid, 0, name);
    AddListMenuItem(playerid, 1, cost);

    new const
        Task:t = ShowAsyncListMenu(playerid, "Upgrades", 20.0, 100.0, 250.0)
    ;

    if (!t) {
        return;
    }

    new responses[E_ASYNC_MENU_DATA];
    await_arr(responses) t;

    if (!responses[E_ASYNC_MENU_RESPONSE]) {
        ShowUpgradeComponentsMenu(playerid, vehicleid, item);

        return;
    }

    if (gVehicleComponentID[vehicleid][item] == componentid) {
        PlayerPlaySound(playerid, MODSHOP_SOUND_DENIED, 0.0, 0.0, 0.0);
        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, 2000, "~r~Already fitted");
        ShowUpgradeComponentContinue(playerid, componentid, vehicleid, item);

        return;
    }

    if (GetPlayerMoney(playerid) < price) {
        PlayerPlaySound(playerid, MODSHOP_SOUND_DENIED, 0.0, 0.0, 0.0);
        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, 2000, "~r~Not enough money");
        ShowUpgradeComponentContinue(playerid, componentid, vehicleid, item);

        return;
    }

    PlayerPlaySound(playerid, MODSHOP_SOUND_PURCHASED, 0.0, 0.0, 0.0);
    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, 2000, "~g~%s~n~~w~$%i", name, price);
    GivePlayerMoney(playerid, -price);

    RemoveComponentFromVehicle(vehicleid, gVehicleComponentID[vehicleid][item]);
    AddComponentToVehicle(vehicleid, componentid);

    gVehicleComponentID[vehicleid][item] = componentid;

    CallLocalFunction("OnComponentAddToVehicle", "ii", vehicleid, componentid);

    ShowUpgradeMenu(playerid, vehicleid);
}

/**
 * # Calls
 */

// What MENU_RESPONSE_UP and MENU_RESPONSE_DOWN used to be. The old menu called
// back on every move with the row it had landed on; this is the same thing
// spelled as a callback, which is what a task based menu has to use for
// anything it wants to say before it closes.
//
// `public` and not `hook`: pp-hooks' `hook` is for a library that wants to sit
// in front of a callback the gamemode may also want. The gamemode is the end
// of that chain, so it writes the plain public, the way race-example does.
public OnPlayerListMenuSelectionChange(playerid, item) {
    if (gPreview[playerid][E_PREVIEW_VEHICLE_ID] == INVALID_VEHICLE_ID) {
        return 0;
    }

    ApplyPreview(playerid, gMenuItemValue[playerid][item]);

    return 0;
}

public OnPlayerConnect(playerid) {
    gVehicleID[playerid] = INVALID_VEHICLE_ID;
    gPreview[playerid][E_PREVIEW_VEHICLE_ID] = INVALID_VEHICLE_ID;

    return 0;
}

public OnPlayerDisconnect(playerid, reason) {
    gPreview[playerid][E_PREVIEW_VEHICLE_ID] = INVALID_VEHICLE_ID;

    return 0;
}

public OnPlayerSpawn(playerid) {
    GivePlayerMoney(playerid, 10000);

    return 1;
}

/**
 * # Commands
 */

SpawnVehicleForPlayer(playerid, modelid) {
    if (!(400 <= modelid <= 611)) {
        SendClientMessage(playerid, -1, "* /v <model-id>, 400 to 611.");

        return;
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:a
    ;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);

    x += floatmul(5.0, floatsin(-a, degrees));
    y += floatmul(5.0, floatcos(-a, degrees));

    gVehicleID[playerid] = CreateVehicle(modelid, x, y, z, a - 45.0, -1, -1, -1);
}

public OnPlayerCommandText(playerid, cmdtext[]) {
    if (!strcmp(cmdtext, "/v", true, 2)) {
        SpawnVehicleForPlayer(playerid, strval(cmdtext[2]));

        return 1;
    }

    if (strequal(cmdtext, "/menu")) {
        if (gVehicleID[playerid] == INVALID_VEHICLE_ID) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, 2000, "~r~Spawn a vehicle with /v first");

            return 1;
        }

        // No TogglePlayerControllable here any more: ShowAsyncListMenu freezes
        // on the way in and lets go on the way out, and doing it by hand as
        // well left the player frozen whenever a menu closed without opening
        // another.
        ShowUpgradeMenu(playerid, gVehicleID[playerid]);

        return 1;
    }

    if (strequal(cmdtext, "/unfreeze")) {
        TogglePlayerControllable(playerid, true);

        return 1;
    }

    return 0;
}
