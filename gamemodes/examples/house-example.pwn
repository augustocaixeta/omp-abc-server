#define MAX_PLAYERS (8)
#define MAX_BUTTONS (Button:64)
#define MAX_PROPERTIES (Property:16)
#define MAX_HOUSES (House:8)
#define MAX_ENTRANCES (Entrance:16)

#include <open.mp>
#include <properties\properties>
#include <extended-text-styles>

// What goes up the moment a house changes hands. Style 3 is the big centred
// one, which is where an event belongs; the conversation around it stays in
// chat. The figure is left unbroken here on purpose, the way the story mode
// writes its own.
#define PURCHASED_GAME_TEXT             "~y~~h~HOUSE PURCHASED"
#define PURCHASED_GAME_TEXT_TIME        (5000)
#define PURCHASED_GAME_TEXT_STYLE       (3)

// What comes back on giving a house up. A number the framework never decides,
// because paying for anything is this script's job and not the library's.
#define HOUSE_REFUND_SHARE              (2)

// How long a line stays up. Nothing here goes to chat: a message the player is
// meant to read at the moment it happens belongs on the screen they are looking
// at, and the list is the only one that needs longer than a glance.
#define MESSAGE_TIME                    (5000)

#define SPAWN_X                         (2093.9287)
#define SPAWN_Y                         (2213.6853)
#define SPAWN_Z                         (10.8203)

#define STARTING_MONEY                  (250000)

// Until when this player has a banner across the middle of their screen. The
// prompt box on the left waits for it rather than being drawn underneath it,
// which is two things being said at once and neither of them read.
static gPlayerAnnounceUntil[MAX_PLAYERS];

main() {}

/**
 * Turns whatever a player is standing at into a property: the one they are
 * inside if they are inside one, and the one whose door they are at otherwise.
 * Buying happens on the doorstep and selling from the sofa, so both count.
 */
static Property:GetPlayerNearestProperty(playerid) {
    new const
        Property:propertyid = GetPlayerProperty(playerid)
    ;

    if (propertyid != INVALID_PROPERTY_ID) {
        return propertyid;
    }

    return GetButtonProperty(GetPlayerButton(playerid));
}

public OnGameModeInit() {
    DisableInteriorEnterExits();

    AddPlayerClass(0, SPAWN_X, SPAWN_Y, SPAWN_Z, 0.0, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);

    // Two houses, both on the market, both locked once somebody owns them. The
    // second one is the same interior as the first and is still a different
    // room: every property is given a virtual world of its own.
    new const
        House:first = CreateHouse("Rockshore West 4",
            2097.6431, 2224.4697, 11.0234, 180.0000,
            774.0961, -50.4399, 1000.5859, 0.0000, 6,
            75000
        ),
        House:second = CreateHouse("Rockshore West 6",
            2090.4866, 2224.4895, 11.0234, 180.0000,
            774.0961, -50.4399, 1000.5859, 0.0000, 6,
            120000
        )
    ;

    // The sale sign stands out by the kerb rather than on the doorstep, the way
    // the story mode puts it: the arrow at the door is for going in, and this is
    // the one you walk onto to buy. No angle, because a pickup has no facing.
    SetPropertySaleMarkerPos(GetHouseProperty(first),  2097.6086, 2220.0173, 10.8203);
    SetPropertySaleMarkerPos(GetHouseProperty(second), 2090.4851, 2220.4902, 10.8203);

    // Houses are numbered from zero in their own space; the properties under
    // them are numbered in the framework's.
    printf("[houses] %d built: house %d on property %d, house %d on property %d",
        GetHouseCount(),
        _:first, _:GetHouseProperty(first),
        _:second, _:GetHouseProperty(second)
    );

    printf("[houses] and two worlds of their own, %d and %d",
        GetPropertyVirtualWorld(GetHouseProperty(first)),
        GetPropertyVirtualWorld(GetHouseProperty(second))
    );

    // The signs on the ground are the framework's now: 1273 while a house is to
    // be had and 1272 once it has been, which house.inc sets and property.inc
    // repaints whenever the deed moves.
    printf("[houses] signs %d and %d",
        GetPropertySaleMarkerModel(GetHouseProperty(first)),
        GetPropertySaleMarkerModel(GetHouseProperty(second))
    );

    // Temporary: where each house put its two areas, so that a build can be
    // told apart from the one before it at a glance.
    new
        Float:x, Float:y, Float:z
    ;

    for (new House:houseid; _:houseid != GetHousePoolSize(); ++houseid) {
        if (!IsValidHouse(houseid)) {
            continue;
        }

        new const
            Property:propertyid = GetHouseProperty(houseid)
        ;

        GetButtonPos(GetPropertySaleMarkerButton(propertyid), x, y, z);

        printf("[mark] house %d: door button %d, sale button %d at %.4f %.4f",
            _:houseid,
            _:GetPropertyExteriorButton(propertyid),
            _:GetPropertySaleMarkerButton(propertyid), x, y);
    }

    return 1;
}

public OnPlayerConnect(playerid) {
    gPlayerAnnounceUntil[playerid] = 0;

    // Where a real server hands the framework the deeds it has just read out
    // of its own storage, one SetPropertyPlayer per row. This one has no
    // storage, so nobody arrives holding anything.

    return 1;
}

public OnPlayerRequestClass(playerid, classid) {
    SetPlayerPos(playerid, SPAWN_X, SPAWN_Y, SPAWN_Z);
    SetPlayerCameraPos(playerid, SPAWN_X, SPAWN_Y + 4.0, SPAWN_Z + 1.0);
    SetPlayerCameraLookAt(playerid, SPAWN_X, SPAWN_Y, SPAWN_Z);

    return 1;
}

public OnPlayerSpawn(playerid) {
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, STARTING_MONEY);

    return 1;
}

/**
 * The prompt on the door. button.inc holds the text and says nothing on its
 * own, which is the point of it: how a script shows it is the script's choice.
 *
 * Both of these fire after GetPlayerButton has been brought up to date, so
 * asking it in each of them shows the right prompt whichever way round the two
 * arrive when the player walks straight from one door into another.
 */
public OnPlayerEnterButtonArea(playerid, Button:buttonid) {
    RefreshButtonPrompt(playerid);

    return 0;
}

public OnPlayerLeaveButtonArea(playerid, Button:buttonid) {
    RefreshButtonPrompt(playerid);

    return 0;
}

public OnPlayerPropertyPromptChange(playerid, Property:propertyid) {
    RefreshButtonPrompt(playerid);

    return 1;
}

static RefreshButtonPrompt(playerid) {
    new
        prompt[MAX_BUTTON_POPUP_LENGTH]
    ;

    // What the framework has to say comes first: an offer, or the question that
    // follows reaching for it. Only one player is ever being asked, so this
    // cannot live on the button, which everybody at the door reads.
    if (GetPlayerPropertyPrompt(playerid, prompt)) {
        PlayerGameTextShowDelayed(playerid, GAME_TEXT_STYLE_POPUP, GetPlayerAnnounceDelay(playerid), 0, prompt);

        return;
    }

    new const
        Button:buttonid = GetPlayerButton(playerid)
    ;

    if (buttonid == INVALID_BUTTON_ID) {
        PlayerGameTextHide(playerid, GAME_TEXT_STYLE_POPUP);

        return;
    }

    GetButtonPopupText(buttonid, prompt);

    // A button with nothing to say says nothing, rather than an empty box.
    if (prompt[0] == EOS) {
        PlayerGameTextHide(playerid, GAME_TEXT_STYLE_POPUP);

        return;
    }

    // Held rather than timed: the prompt belongs to standing there, so it stays
    // while the player does and goes when they walk off. A gametext given a
    // time would leave them at the door with nothing on screen, and follow them
    // away from it if they left before it ran out.
    PlayerGameTextShowDelayed(playerid, GAME_TEXT_STYLE_POPUP, GetPlayerAnnounceDelay(playerid), 0, prompt);
}

/**
 * How long the prompt box has to wait before it is worth putting up, which is
 * whatever is left of the banner across the middle of the screen.
 */
static GetPlayerAnnounceDelay(playerid) {
    new const
        remaining = gPlayerAnnounceUntil[playerid] - GetTickCount()
    ;

    return (remaining > 0) ? (remaining) : (0);
}

/**
 * # Money
 *
 * Every one of these is this script reaching for the money, never the library.
 */

public OnPlayerBuyProperty(playerid, Property:propertyid, price) {
    if (GetPlayerMoney(playerid) < price) {
        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, MESSAGE_TIME, "~r~You do not have enough money to buy this house.");

        return 1;
    }

    return 0;
}

public OnPlayerBoughtProperty(playerid, Property:propertyid, price, previousPlayer, previousExtra) {
    GivePlayerMoney(playerid, -price);

    // What just left the wallet, in the counter the game keeps it in.
    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_MONEY_NEGATIVE, MESSAGE_TIME, "-$%i", price);

    // The moment itself, in the big centred style, because it is an event and
    // not a line of conversation. Everything else is told to keep out of its
    // way until it has gone.
    GameTextForPlayer(playerid, PURCHASED_GAME_TEXT, PURCHASED_GAME_TEXT_TIME, PURCHASED_GAME_TEXT_STYLE);

    gPlayerAnnounceUntil[playerid] = GetTickCount() + PURCHASED_GAME_TEXT_TIME;

    PlayerGameTextHide(playerid, GAME_TEXT_STYLE_POPUP);

    // What a real server writes on the deed so that it can find this place
    // again after a restart: an account id, a row id, whatever it keeps. The
    // framework never reads it.
    SetPropertyExtra(propertyid, 1000 + playerid);

    // A sale between two players is a payment to somebody rather than to
    // nobody. The seller is paid where they stand if they are here; if they are
    // not, previousExtra is what a real server would take to its database.
    if (previousPlayer == INVALID_PLAYER_ID) {
        return 1;
    }

    GivePlayerMoney(previousPlayer, price);
    PlayerGameTextShow(previousPlayer, GAME_TEXT_STYLE_MONEY, MESSAGE_TIME, "$%i", price);

    return 1;
}

public OnPlayerSoldProperty(playerid, Property:propertyid, price) {
    new const
        refund = price / HOUSE_REFUND_SHARE
    ;

    GivePlayerMoney(playerid, refund);
    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_MONEY, MESSAGE_TIME, "$%i", refund);

    return 1;
}

/**
 * # Refusals
 *
 * Both of these are reached instead of the sale or the doorway, not before it.
 */

public OnPlayerHouseLimit(playerid, House:houseid) {
    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, MESSAGE_TIME,
        "~r~You already own the maximum number of houses allowed.");

    return 1;
}

public OnPlayerPropertyLocked(playerid, Property:propertyid) {
    new
        kind[MAX_PROPERTY_TYPE_NAME]
    ;

    // The kind, not the name: the sentence wants the word house in the middle
    // of it, and which house it is is already on the door.
    GetPropertyKind(propertyid, kind);

    // In the message style and not the box style. The box belongs to the door
    // and stays while the player does; a reaction put there would take the door
    // prompt down and leave nothing behind when it ran out.
    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, MESSAGE_TIME,
        "~r~This %s is locked. Only the owner can enter.", kind);

    return 1;
}

public OnPlayerCommandText(playerid, cmdtext[]) {
    // How many, and not which ones. A list of names and prices is more than one
    // line and more than a glance, and the game says nothing that way: a script
    // that wants to show the list itself puts it in a dialog.
    if (!strcmp(cmdtext, "/houses", true)) {
        new const
            count = CountPlayerHouses(playerid)
        ;

        if (!count) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, MESSAGE_TIME, "~w~You own no houses.");

            return 1;
        }

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, MESSAGE_TIME,
            "~w~You own %d of %d houses.", count, MAX_PLAYER_HOUSES);

        return 1;
    }

    // Temporary: what the framework thinks the player is standing on. Here to
    // tell a sale area that is not being entered from one that is not mapped.
    if (!strcmp(cmdtext, "/b", true)) {
        new const
            Button:buttonid = GetPlayerButton(playerid)
        ;

        if (buttonid == INVALID_BUTTON_ID) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, MESSAGE_TIME, "~r~In no area at all.");

            return 1;
        }

        // Which property is here at all, and which one could be bought here.
        // Those are two different questions and only the second one answers
        // the buy key.
        new const
            Property:sale = GetPlayerSaleProperty(playerid)
        ;

        // Every guard BuyProperty runs, in the order it runs them, so that a
        // refusal says which one turned it down instead of only that it did.
        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, MESSAGE_TIME,
            "~w~btn %d  here %d  sale %d~n~forsale %d  owned %d  holder %d~n~houses %d/%d  money %d/%d",
            _:buttonid,
            _:GetButtonProperty(buttonid),
            _:sale,
            _:IsPropertyForSale(sale),
            _:IsPropertyOwned(sale),
            GetPropertyPlayer(sale),
            CountPlayerHouses(playerid), MAX_PLAYER_HOUSES,
            GetPlayerMoney(playerid), GetPropertyPrice(sale)
        );

        return 1;
    }

    // The same question the buy key asks, and not "what property is here": the
    // door is a way in, not a place to buy a house.
    if (!strcmp(cmdtext, "/buy", true)) {
        new const
            Property:propertyid = GetPlayerSaleProperty(playerid)
        ;

        if (propertyid == INVALID_PROPERTY_ID) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, MESSAGE_TIME, "~r~There is nothing to buy here.");

            return 1;
        }

        if (!IsPropertyForSale(propertyid)) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, MESSAGE_TIME, "~r~That one is not on the market.");

            return 1;
        }

        // Everything the sale is refused for says so itself, on its way past.
        BuyProperty(playerid, propertyid);

        return 1;
    }

    if (!strcmp(cmdtext, "/sell", true)) {
        new const
            Property:propertyid = GetPlayerNearestProperty(playerid)
        ;

        if (!IsPlayerPropertyOwner(playerid, propertyid)) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, MESSAGE_TIME, "~r~You can only give up what is yours.");

            return 1;
        }

        SellProperty(playerid, propertyid);

        return 1;
    }

    if (!strcmp(cmdtext, "/lock", true)) {
        new const
            Property:propertyid = GetPlayerNearestProperty(playerid)
        ;

        if (!IsPlayerPropertyOwner(playerid, propertyid)) {
            PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, MESSAGE_TIME, "~r~You can only lock what is yours.");

            return 1;
        }

        new const
            bool:locked = !IsPropertyLocked(propertyid)
        ;

        SetPropertyLocked(propertyid, locked);

        PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, MESSAGE_TIME,
            locked ? ("~w~Locked.") : ("~w~Open."));

        return 1;
    }

    return 0;
}
