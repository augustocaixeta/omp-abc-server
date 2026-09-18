#define MAX_PLAYERS (4)

#include <open.mp>

// movement.inc has to come first: fitness uses its reading of what the ped
// is really doing instead of guessing from the sprint key. Without it the
// library falls back to reading the animation index on its own.
#tryinclude <movement>
#include <fitness>

#define HUD_BAR_X                       (500.000000)
#define HUD_BAR_Y                       (330.000000)
#define HUD_BAR_WIDTH                   (110.000000)
#define HUD_BAR_HEIGHT                  (0.400000)

#define HUD_UPDATE_TIME                 (200)

static enum _:E_HUD_DATA {
    PlayerText:E_HUD_BAR_BACKGROUND,
    PlayerText:E_HUD_BAR_FILL,
    PlayerText:E_HUD_INFO
};

static
    Hud[MAX_PLAYERS][E_HUD_DATA],
    HudTimer[MAX_PLAYERS]
;

#if defined _INC_MOVEMENT
static const
    MovementName[][] = {
        "unknown",
        "idle",
        "normal",
        "walk",
        "RUN",
        "jump",
        "fall"
    };
#endif

main(){}

/**
 * @hud
 */

static CreateHud(playerid) {
    new
        PlayerText:playerText
    ;

    playerText = Hud[playerid][E_HUD_BAR_BACKGROUND] = CreatePlayerTextDraw(playerid, HUD_BAR_X, HUD_BAR_Y, "_");
    PlayerTextDrawUseBox(playerid, playerText, true);
    PlayerTextDrawBoxColour(playerid, playerText, 0x000000AA);
    PlayerTextDrawLetterSize(playerid, playerText, 0.000000, HUD_BAR_HEIGHT);
    PlayerTextDrawTextSize(playerid, playerText, HUD_BAR_X + HUD_BAR_WIDTH, 0.000000);
    PlayerTextDrawShow(playerid, playerText);

    playerText = Hud[playerid][E_HUD_BAR_FILL] = CreatePlayerTextDraw(playerid, HUD_BAR_X, HUD_BAR_Y, "_");
    PlayerTextDrawUseBox(playerid, playerText, true);
    PlayerTextDrawBoxColour(playerid, playerText, 0x33AA33FF);
    PlayerTextDrawLetterSize(playerid, playerText, 0.000000, HUD_BAR_HEIGHT);
    PlayerTextDrawTextSize(playerid, playerText, HUD_BAR_X + HUD_BAR_WIDTH, 0.000000);
    PlayerTextDrawShow(playerid, playerText);

    playerText = Hud[playerid][E_HUD_INFO] = CreatePlayerTextDraw(playerid, HUD_BAR_X, HUD_BAR_Y + 12.0, "_");
    PlayerTextDrawLetterSize(playerid, playerText, 0.200000, 1.100000);
    PlayerTextDrawFont(playerid, playerText, TEXT_DRAW_FONT_2);
    PlayerTextDrawSetOutline(playerid, playerText, 1);
    PlayerTextDrawSetShadow(playerid, playerText, 0);
    PlayerTextDrawSetProportional(playerid, playerText, true);
    PlayerTextDrawShow(playerid, playerText);
}

static DestroyHud(playerid) {
    for (new i = sizeof (Hud[]); --i >= 0;) {
        PlayerTextDrawDestroy(playerid, PlayerText:Hud[playerid][i]);
    }
}

static GetMovementLabel(playerid, label[], size = sizeof (label)) {
#if defined _INC_MOVEMENT
    strcat(label, MovementName[_:GetPlayerMovement(playerid)], size);
#else
    strcat(label, IsPlayerSprinting(playerid) ? ("RUN") : ("-"), size);
#endif
}

forward @Hud_Update(playerid);

public @Hud_Update(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return 0;
    }

    new const
        Float:breath = GetPlayerBreath(playerid),
        Float:maximum = GetPlayerMaxBreath(playerid)
    ;

    new const
        PlayerText:fill = Hud[playerid][E_HUD_BAR_FILL]
    ;

    if (breath <= 0.0) {
        PlayerTextDrawHide(playerid, fill);
    }
    else {
        PlayerTextDrawBoxColour(playerid, fill, IsPlayerExhausted(playerid) ? 0xAA3333FF : 0x33AA33FF);
        PlayerTextDrawTextSize(playerid, fill, HUD_BAR_X + HUD_BAR_WIDTH * (breath / maximum), 0.000000);
        PlayerTextDrawShow(playerid, fill);
    }

    new
        movement[16]
    ;

    GetMovementLabel(playerid, movement);

    PlayerTextDrawSetString(playerid, Hud[playerid][E_HUD_INFO],
        "BREATH %.1f/%.1f  STR %.0f  STA %.0f  ~n~MOVE %s%s",
        breath, maximum,
        GetPlayerStrength(playerid), GetPlayerStamina(playerid),
        movement,
        IsPlayerExhausted(playerid) ? (" ~r~EXHAUSTED") : (""));

    PlayerTextDrawShow(playerid, Hud[playerid][E_HUD_INFO]);

    return 0;
}

/**
 * @callback
 */

public OnPlayerBreathOver(playerid) {
    SendClientMessage(playerid, 0xAA3333FF, "* Out of breath: you cannot sprint until you catch it.");
    PlayerPlaySound(playerid, 1085, 0.0, 0.0, 0.0);

    return 1;
}

public OnPlayerBreathRecover(playerid) {
    SendClientMessage(playerid, 0x33AA33FF, "* You caught your breath and can sprint again.");
    PlayerPlaySound(playerid, 1084, 0.0, 0.0, 0.0);

    return 1;
}

public OnPlayerFitnessChange(playerid, E_FITNESS_ATTRIBUTE:attribute, Float:amount, Float:total) {
    // Only worth announcing when a whole point was crossed, the training of a
    // single tick of running is much smaller than that.
    if (floatround(total - amount, floatround_floor) != floatround(total, floatround_floor)) {
        SendClientMessage(playerid, 0xCCCCCCFF, "* %s is now %.0f.", (attribute == FITNESS_STRENGTH) ? ("Strength") : ("Stamina"), total);
    }

    return 1;
}

public OnGameModeInit() {
    // Without this the animation indexes read to tell running apart from
    // walking mean nothing.
    UsePlayerPedAnims();

    SetGameModeText("fitness test");

    return 1;
}

public OnPlayerConnect(playerid) {
    SetSpawnInfo(playerid, NO_TEAM, 60, 1958.33, 1343.12, 15.36, 90.0);

    return 1;
}

public OnPlayerSpawn(playerid) {
    CreateHud(playerid);

    HudTimer[playerid] = SetTimerEx("@Hud_Update", HUD_UPDATE_TIME, true, "i", playerid);

    SendClientMessage(playerid, 0xFFFFFFFF, "* Run around holding sprint and watch the bar. Commands:");
    SendClientMessage(playerid, 0xCCCCCCFF, "* /training <0-100>, /strength <0-100>, /stamina <0-100>, /tired, /breath");

    return 1;
}

public OnPlayerDisconnect(playerid, reason) {
    if (HudTimer[playerid]) {
        KillTimer(HudTimer[playerid]);

        HudTimer[playerid] = 0;
    }

    DestroyHud(playerid);

    return 1;
}

public OnPlayerCommandText(playerid, cmdtext[]) {
    if (!strcmp(cmdtext, "/tired", true)) {
        SetPlayerExhausted(playerid, true);

        return 1;
    }

    if (!strcmp(cmdtext, "/breath", true)) {
        SendClientMessage(playerid, 0xFFFFFFFF, "* Breath %.2f of %.2f | strength %.1f | stamina %.1f | sprinting %i | exhausted %i",
            GetPlayerBreath(playerid), GetPlayerMaxBreath(playerid),
            GetPlayerStrength(playerid), GetPlayerStamina(playerid),
            IsPlayerSprinting(playerid), IsPlayerExhausted(playerid));

        return 1;
    }

    if (!strcmp(cmdtext, "/training", true, 9)) {
        new const
            Float:value = float(strval(cmdtext[9]))
        ;

        SetPlayerStrength(playerid, value);
        SetPlayerStamina(playerid, value);
        SetPlayerBreath(playerid, GetPlayerMaxBreath(playerid));

        SendClientMessage(playerid, 0xFFFFFFFF, "* Training set to %.0f, breath is now %.1f seconds of sprint.", value, GetPlayerMaxBreath(playerid));

        return 1;
    }

    if (!strcmp(cmdtext, "/strength", true, 9)) {
        SetPlayerStrength(playerid, float(strval(cmdtext[9])));

        return 1;
    }

    if (!strcmp(cmdtext, "/stamina", true, 8)) {
        SetPlayerStamina(playerid, float(strval(cmdtext[8])));
        SetPlayerBreath(playerid, GetPlayerMaxBreath(playerid));

        return 1;
    }

    return 0;
}
