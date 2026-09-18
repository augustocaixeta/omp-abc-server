#if defined _CORE_PLAYER_COMMAND
    #endinput
#endif
#define _CORE_PLAYER_COMMAND

#include <pp-hooks>

/**
 * # Commands
 *
 * Everything here is gameplay. The tools for building the gamemode live under
 * dev and are listed separately, so the day they come out nothing in this file
 * changes.
 *
 * Pawn.CMD dispatches these, which is why none of them reads cmdtext: the name
 * is the function's, and the rest of the line arrives as params. A name cannot
 * carry a hyphen -- it is a Pawn identifier -- so anything that wants two words
 * runs them together.
 */

static stock bool:TurnInventoryPageInternal(playerid, page) {
    if (GetContainerMenuPage(playerid, gInventoryMenu) == -1) {
        SendClientMessage(playerid, -1, "Nothing is open.");

        return false;
    }

    if (!SetContainerMenuPage(playerid, gInventoryMenu, page)) {
        SendClientMessage(playerid, -1, "There is no page %i.", page + 1);

        return false;
    }

    SendClientMessage(playerid, -1, "Page %i of %i.",
        page + 1, GetContainerMenuPageCount(playerid, gInventoryMenu));

    return true;
}

CMD:help(playerid, params[]) {
    SendClientMessage(playerid, -1, "Inventory: %s with empty hands, or /inv. Escape closes one menu at a time.", INVENTORY_KEY_NAME);
    SendClientMessage(playerid, -1, "Inventory: /inv /page /next /prev /close /stow");
    SendClientMessage(playerid, -1, "Crafting: /craft to see what you can make, /craft <name> to make it.");
    SendClientMessage(playerid, -1, "Development: /dev, /devlocker [bin|shop|sealed]");
    SendClientMessage(playerid, -1, "Properties: /devint then /devprop, /devpropmarker /devproplist /devpropdel /devpropdump");

    return 1;
}

CMD:inv(playerid, params[]) {
    if (!ShowPlayerInventory(playerid)) {
        SendClientMessage(playerid, -1, "You cannot check your pockets right now.");

        return 1;
    }

    SendClientMessage(playerid, -1, "Page %i of %i.",
        GetContainerMenuPage(playerid, gInventoryMenu) + 1,
        GetContainerMenuPageCount(playerid, gInventoryMenu));

    return 1;
}

CMD:stow(playerid, params[]) {
    if (!StoreHeldItem(playerid)) {
        if (!IsValidItem(GetPlayerHoldItem(playerid))) {
            SendClientMessage(playerid, -1, "Your hands are empty.");
        }

        return 1;
    }

    return 1;
}

CMD:close(playerid, params[]) {
    HidePlayerInventory(playerid);

    return 1;
}

CMD:page(playerid, params[]) {
    new
        page
    ;

    if (sscanf(params, "d", page)) {
        SendClientMessage(playerid, -1, "Usage: /page <number>");

        return 1;
    }

    TurnInventoryPageInternal(playerid, page - 1);

    return 1;
}

CMD:next(playerid, params[]) {
    TurnInventoryPageInternal(playerid, GetContainerMenuPage(playerid, gInventoryMenu) + 1);

    return 1;
}

CMD:prev(playerid, params[]) {
    TurnInventoryPageInternal(playerid, GetContainerMenuPage(playerid, gInventoryMenu) - 1);

    return 1;
}

/**
 * # Crafting
 *
 * The pockets stand in for a bench until there is one. A bench is a container,
 * so it starts a run with the same call and its own container.
 */

static stock Craft:FindPlayerCraftInternal(const name[]) {
    new
        resultName[MAX_ITEM_BUILD_NAME]
    ;

    for (new Craft:craftid, crafts = GetCraftCount(); _:craftid != crafts; ++craftid) {
        GetItemBuildName(GetCraftResult(craftid), resultName);

        if (strfind(resultName, name, true) == 0) {
            return craftid;
        }
    }

    return INVALID_CRAFT_ID;
}

static stock ListCraftIngredientsInternal(Container:containerid, Craft:craftid, output[], size = sizeof (output)) {
    output[0] = EOS;

    new
        entry[MAX_ITEM_BUILD_NAME + 16],
        name[MAX_ITEM_BUILD_NAME],
        ItemBuild:buildid = INVALID_ITEM_BUILD_ID,
        amount
    ;

    for (new i, count = GetCraftIngredientCount(craftid); i != count; ++i) {
        GetCraftIngredient(craftid, i, buildid, amount);
        GetItemBuildName(buildid, name);

        if (amount == 0) {
            format(entry, sizeof (entry), "%s held", name);
        } else {
            format(entry, sizeof (entry), "%s %i/%i", name, CountContainerItemBuild(containerid, buildid), amount);
        }

        if (i) {
            strcat(output, ", ", size);
        }

        strcat(output, entry, size);
    }
}

CMD:craft(playerid, params[]) {
    new const
        Container:containerid = GetPlayerContainer(playerid)
    ;

    new
        name[MAX_ITEM_BUILD_NAME],
        ingredients[144]
    ;

    if (isnull(params)) {
        if (GetCraftCount() == 0) {
            SendClientMessage(playerid, -1, "Nothing is made out of anything yet.");

            return 1;
        }

        for (new Craft:craftid, crafts = GetCraftCount(); _:craftid != crafts; ++craftid) {
            GetItemBuildName(GetCraftResult(craftid), name);
            ListCraftIngredientsInternal(containerid, craftid, ingredients);

            new const
                lots = CountCraftLots(containerid, craftid)
            ;

            new
                station[MAX_CRAFT_STATION_NAME]
            ;

            if (GetCraftStationName(GetCraftStation(craftid), station)) {
                SendClientMessage(playerid, -1, "%s: on a %s. %s", name, station, ingredients);

                continue;
            }

            SendClientMessage(playerid, -1, "%s: %i lots, %i of them. %s",
                name, lots, lots * GetCraftYield(craftid), ingredients);
        }

        return 1;
    }

    new const
        Craft:craftid = FindPlayerCraftInternal(params)
    ;

    if (craftid == INVALID_CRAFT_ID) {
        SendClientMessage(playerid, -1, "Nobody knows how to make that.");

        return 1;
    }

    GetItemBuildName(GetCraftResult(craftid), name);

    new
        station[MAX_CRAFT_STATION_NAME]
    ;

    if (GetCraftStationName(GetCraftStation(craftid), station)) {
        SendClientMessage(playerid, -1, "%s is made on a %s, not in your hands.", name, station);

        return 1;
    }

    new const
        lots = CountCraftLots(containerid, craftid)
    ;

    if (lots <= 0) {
        ListCraftIngredientsInternal(containerid, craftid, ingredients);

        SendClientMessage(playerid, -1, "You cannot make %s: %s.", name, ingredients);

        return 1;
    }

    if (!StartPlayerCraft(playerid, craftid, containerid)) {
        SendClientMessage(playerid, -1, "You are already in the middle of something.");

        return 1;
    }

    SendClientMessage(playerid, -1, "You start %i lots of %s, %i in all.",
        lots, name, lots * GetCraftYield(craftid));

    return 1;
}

hook OnPlayerFinishCraft(playerid, Craft:craftid, lots) {
    new
        name[MAX_ITEM_BUILD_NAME]
    ;

    GetItemBuildName(GetCraftResult(craftid), name);

    if (lots == 0) {
        SendClientMessage(playerid, -1, "You get nowhere with the %s.", name);

        return 0;
    }

    SendClientMessage(playerid, -1, "You make %i %s.", lots * GetCraftYield(craftid), name);

    return 0;
}
