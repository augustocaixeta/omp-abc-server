#if defined _INC_PP_MODEL_DIALOG
    #endinput
#endif
#define _INC_PP_MODEL_DIALOG

#include <pp-hooks>

#if !defined MAX_MODEL_DIALOG_DESC_LEN
    #define MAX_MODEL_DIALOG_DESC_LEN (32)
#endif

enum {
    MODEL_DIALOG_RESPONSE_SELECT,
    MODEL_DIALOG_RESPONSE_CANCEL
};

enum _:E_MODEL_DIALOG_DATA {
    E_MODEL_DIALOG_RESPONSE,
    E_MODEL_DIALOG_MENUITEM,
    E_MODEL_DIALOG_MODEL_ID
};

const
    MODEL_DIALOG_ROWS = 2,
    MODEL_DIALOG_COLUMNS = 6,
    MODEL_DIALOG_PAGE_ITEMS = (MODEL_DIALOG_ROWS * MODEL_DIALOG_COLUMNS)
;

static enum E_TEXT_DRAW_DATA {
    Text:E_TEXT_DRAW_BACKGROUND[9],
    Text:E_TEXT_DRAW_BOX_BUTTON[MODEL_DIALOG_PAGE_ITEMS],
    Text:E_TEXT_DRAW_BOX_TOP[MODEL_DIALOG_PAGE_ITEMS],
    Text:E_TEXT_DRAW_MODEL_PREVIEW[MODEL_DIALOG_PAGE_ITEMS],
    Text:E_TEXT_DRAW_UP_BUTTON,
    Text:E_TEXT_DRAW_DOWN_BUTTON,
    Text:E_TEXT_DRAW_SELECT_BUTTON,
    Text:E_TEXT_DRAW_CANCEL_BUTTON
};

static enum E_PLAYER_TEXT_DRAW_DATA {
    PlayerText:E_PLAYER_TEXT_DRAW_SCROLL_BAR,
    PlayerText:E_PLAYER_TEXT_DRAW_CAPTION,
    PlayerText:E_PLAYER_TEXT_DRAW_PAGE,
    PlayerText:E_PLAYER_TEXT_DRAW_FIRST_BUTTON,
    PlayerText:E_PLAYER_TEXT_DRAW_SECOND_BUTTON,
    PlayerText:E_PLAYER_TEXT_DRAW_DESC[MODEL_DIALOG_PAGE_ITEMS]
};

static enum _:E_DIALOG_PREVIEW_DATA {
    E_DIALOG_PREVIEW_DESC[MAX_MODEL_DIALOG_DESC_LEN],
    E_DIALOG_PREVIEW_MODEL_ID,
    Float:E_DIALOG_PREVIEW_SIZE_X,
    Float:E_DIALOG_PREVIEW_SIZE_Y,
    Float:E_DIALOG_PREVIEW_ROTATION_X,
    Float:E_DIALOG_PREVIEW_ROTATION_Y,
    Float:E_DIALOG_PREVIEW_ROTATION_Z,
    Float:E_DIALOG_PREVIEW_ZOOM,
    Float:E_DIALOG_PREVIEW_OFFSET_X,
    Float:E_DIALOG_PREVIEW_OFFSET_Y
};

static enum E_PLAYER_MENU_DATA {
    List:E_PLAYER_MENU_LIST,
    Task:E_PLAYER_MENU_TASK,
    E_PLAYER_MENU_PAGE,
    E_PLAYER_MENU_SIZE,
    E_PLAYER_MENU_ITEM_CHOSEN
};

static
    gTextDrawData[E_TEXT_DRAW_DATA],
    gPlayerTextDrawData[MAX_PLAYERS][E_PLAYER_TEXT_DRAW_DATA],
    gPlayerMenuData[MAX_PLAYERS][E_PLAYER_MENU_DATA],
    gTextDrawBoxMenuItem[MAX_TEXT_DRAWS] = { -1, ... }
;

/**
 * # Functions
 */

forward void:AddModelDialogMenuItem(List:list, modelid, const description[] = "", Float:sizeX = 53.000, Float:sizeY = 53.000, Float:rotationX = 0.000, Float:rotationY = 0.000, Float:rotationZ = 0.000, Float:zoom = 1.000, Float:offsetX = 0.000, Float:offsetY = 0.000);
forward bool:ShowModelDialogMenu(playerid, List:list, const caption[], const button1[], const button2[], start = 0);
forward Task:ShowAsyncModelDialogMenu(playerid, List:list, const caption[], const button1[], const button2[], start = 0);
forward bool:HideModelDialogMenu(playerid);

/**
 * # Events
 */

forward OnModelDialogResponse(playerid, response, menuItem, modelid);

/**
 * # Internal
 */

static void:ShowModelDialogMenuTextDraws(playerid, const caption[], const button1[], const button2[]) {
    for (new i; i != 9; ++i) {
        TextDrawShowForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_BACKGROUND][i]);
    }

    TextDrawShowForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_UP_BUTTON]);
    TextDrawShowForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_DOWN_BUTTON]);
    
    TextDrawShowForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_SELECT_BUTTON]);
    TextDrawShowForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_CANCEL_BUTTON]);

    new
        PlayerText:pt
    ;

    pt = gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_SCROLL_BAR] = CreatePlayerTextDraw(playerid, 0.0, 0.0, "LD_DUAL:WHITE");
    PlayerTextDrawColour(playerid, pt, 0xFF0000FF);
    PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_SPRITE);

    pt = gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_CAPTION] = CreatePlayerTextDraw(playerid, 142.000, 132.000, caption);
    PlayerTextDrawLetterSize(playerid, pt, 0.180, 1.000);
    PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawSetShadow(playerid, pt, 0);
    PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_BANK_GOTHIC);
    PlayerTextDrawSetProportional(playerid, pt, true);
    PlayerTextDrawShow(playerid, pt);

    pt = gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_PAGE] = CreatePlayerTextDraw(playerid, 498.500, 132.000, "-/-");
    PlayerTextDrawLetterSize(playerid, pt, 0.180, 1.000);
    PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawSetShadow(playerid, pt, 0);
    PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_BANK_GOTHIC);
    PlayerTextDrawSetProportional(playerid, pt, true);
    PlayerTextDrawShow(playerid, pt);

    pt = gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_FIRST_BUTTON] = CreatePlayerTextDraw(playerid, 291.500, 295.000, button1);
    PlayerTextDrawLetterSize(playerid, pt, 0.180, 1.000);
    PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawSetShadow(playerid, pt, 0);
    PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_BANK_GOTHIC);
    PlayerTextDrawSetProportional(playerid, pt, true);
    PlayerTextDrawShow(playerid, pt);

    pt = gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_SECOND_BUTTON] = CreatePlayerTextDraw(playerid, 347.500, 295.000, button2);
    PlayerTextDrawLetterSize(playerid, pt, 0.180, 1.000);
    PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawSetShadow(playerid, pt, 0);
    PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_BANK_GOTHIC);
    PlayerTextDrawSetProportional(playerid, pt, true);
    PlayerTextDrawShow(playerid, pt);
}

static void:UpdateModelDialogMenuScrollBar(playerid, page, totalItems, totalPages) {
    new const
        Float:h = 112.000 * (float(MODEL_DIALOG_PAGE_ITEMS) / float(totalItems)),
        Float:y = 161.500 + (112.000 - h) * (float(page) / float(totalPages - 1))
    ;

    PlayerTextDrawSetString(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_PAGE], "%i/%i", page + 1, totalPages);
    PlayerTextDrawSetPos(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_SCROLL_BAR], 485.000, y);
    PlayerTextDrawTextSize(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_SCROLL_BAR], 8.000, h);
    PlayerTextDrawShow(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_SCROLL_BAR]);
}

static void:ShowModelDialogMenuPage(playerid, page) {
    new
        Iter:iterator = list_iter(gPlayerMenuData[playerid][E_PLAYER_MENU_LIST], page * MODEL_DIALOG_PAGE_ITEMS),
        menuItem,
        data[E_DIALOG_PREVIEW_DATA]
    ;

    for (; iter_inside(iterator) && menuItem != MODEL_DIALOG_PAGE_ITEMS; iter_move_next(iterator), ++menuItem) {
        iter_get_arr(iterator, data);

        TextDrawSetPos(gTextDrawData[E_TEXT_DRAW_MODEL_PREVIEW][menuItem], 
            148.000 + menuItem % MODEL_DIALOG_COLUMNS * 56.000 + data[E_DIALOG_PREVIEW_OFFSET_X],
            158.000 + menuItem / MODEL_DIALOG_COLUMNS * 66.000 + data[E_DIALOG_PREVIEW_OFFSET_Y]
        );
        TextDrawTextSize(gTextDrawData[E_TEXT_DRAW_MODEL_PREVIEW][menuItem],
            data[E_DIALOG_PREVIEW_SIZE_X],
            data[E_DIALOG_PREVIEW_SIZE_Y]
        );
        TextDrawSetPreviewRot(gTextDrawData[E_TEXT_DRAW_MODEL_PREVIEW][menuItem],
            data[E_DIALOG_PREVIEW_ROTATION_X],
            data[E_DIALOG_PREVIEW_ROTATION_Y],
            data[E_DIALOG_PREVIEW_ROTATION_Z],
            data[E_DIALOG_PREVIEW_ZOOM]
        );
        TextDrawSetPreviewModel(gTextDrawData[E_TEXT_DRAW_MODEL_PREVIEW][menuItem], data[E_DIALOG_PREVIEW_MODEL_ID]);
        TextDrawColour(gTextDrawData[E_TEXT_DRAW_BOX_BUTTON][menuItem], 0x424242FF);

        TextDrawShowForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_BOX_BUTTON][menuItem]);
        TextDrawShowForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_BOX_TOP][menuItem]);
        TextDrawShowForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_MODEL_PREVIEW][menuItem]);

        if (!PlayerTextDrawSetString(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_DESC][menuItem], data[E_DIALOG_PREVIEW_DESC])) {
            gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_DESC][menuItem] = CreatePlayerTextDraw(playerid,
                151.000 + menuItem % MODEL_DIALOG_COLUMNS * 56.000,
                155.000 + menuItem / MODEL_DIALOG_COLUMNS * 66.000, data[E_DIALOG_PREVIEW_DESC]
            );
            PlayerTextDrawLetterSize(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_DESC][menuItem], 0.120, 0.800);
            PlayerTextDrawSetShadow(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_DESC][menuItem], 0);
            PlayerTextDrawFont(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_DESC][menuItem], TEXT_DRAW_FONT_BANK_GOTHIC);
            PlayerTextDrawSetProportional(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_DESC][menuItem], true);
            PlayerTextDrawShow(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_DESC][menuItem]);
        }
    }

    for (; menuItem != MODEL_DIALOG_PAGE_ITEMS; ++menuItem) {
        TextDrawHideForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_BOX_BUTTON][menuItem]);
        TextDrawHideForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_BOX_TOP][menuItem]);
        TextDrawHideForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_MODEL_PREVIEW][menuItem]);
        PlayerTextDrawSetString(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_DESC][menuItem], "");
    }

    new const
        totalItems = gPlayerMenuData[playerid][E_PLAYER_MENU_SIZE],
        totalPages = (totalItems + MODEL_DIALOG_PAGE_ITEMS - 1) / MODEL_DIALOG_PAGE_ITEMS
    ;

    UpdateModelDialogMenuScrollBar(playerid, page, totalItems, totalPages);
    gPlayerMenuData[playerid][E_PLAYER_MENU_PAGE] = page;
    gPlayerMenuData[playerid][E_PLAYER_MENU_ITEM_CHOSEN] = -1;
}

static void:SelectModelDialogMenuItem(playerid, menuItem) {
    if (gPlayerMenuData[playerid][E_PLAYER_MENU_ITEM_CHOSEN] >= 0) {
        new const
            lastItem = gPlayerMenuData[playerid][E_PLAYER_MENU_ITEM_CHOSEN]
        ;

        TextDrawColour(gTextDrawData[E_TEXT_DRAW_BOX_BUTTON][lastItem], 0x424242FF);
        TextDrawShowForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_BOX_BUTTON][lastItem]);
    }

    TextDrawColour(gTextDrawData[E_TEXT_DRAW_BOX_BUTTON][menuItem], 0xFF0000FF);
    TextDrawShowForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_BOX_BUTTON][menuItem]);
    gPlayerMenuData[playerid][E_PLAYER_MENU_ITEM_CHOSEN] = menuItem;
}

static void:HandleModelDialogMenuSelection(playerid, response, menuItem) {
    new const
        modelid = (menuItem >= 0) ? list_get(gPlayerMenuData[playerid][E_PLAYER_MENU_LIST], menuItem, E_DIALOG_PREVIEW_MODEL_ID) : -1
    ;

    if (gPlayerMenuData[playerid][E_PLAYER_MENU_TASK] != INVALID_TASK) {
        new
            data[E_MODEL_DIALOG_DATA]
        ;

        data[E_MODEL_DIALOG_RESPONSE] = response;
        data[E_MODEL_DIALOG_MENUITEM] = menuItem;
        data[E_MODEL_DIALOG_MODEL_ID] = modelid;

        HideModelDialogMenu(playerid);

        new const
            Task:t = gPlayerMenuData[playerid][E_PLAYER_MENU_TASK]
        ;

        gPlayerMenuData[playerid][E_PLAYER_MENU_TASK] = INVALID_TASK;
        task_set_result_arr(t, data);
    } else {
        CallLocalFunction("OnModelDialogResponse", "iiii", playerid, response, menuItem, modelid);
        HideModelDialogMenu(playerid);
    }
}

static void:ResetModelDialogMenuData(playerid) {
    gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_SCROLL_BAR] = INVALID_PLAYER_TEXT_DRAW;
    gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_CAPTION] = INVALID_PLAYER_TEXT_DRAW;
    gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_PAGE] = INVALID_PLAYER_TEXT_DRAW;
    gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_FIRST_BUTTON] = INVALID_PLAYER_TEXT_DRAW;
    gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_SECOND_BUTTON] = INVALID_PLAYER_TEXT_DRAW;

    if (gPlayerMenuData[playerid][E_PLAYER_MENU_LIST]) {
        list_delete(gPlayerMenuData[playerid][E_PLAYER_MENU_LIST]);
    }

    gPlayerMenuData[playerid][E_PLAYER_MENU_LIST] = INVALID_LIST;
    gPlayerMenuData[playerid][E_PLAYER_MENU_PAGE] = 0;
    gPlayerMenuData[playerid][E_PLAYER_MENU_SIZE] = 0;
    gPlayerMenuData[playerid][E_PLAYER_MENU_ITEM_CHOSEN] = -1;
}

static void:ResetModelDialogMenuDataAndTask(playerid) {
    if (gPlayerMenuData[playerid][E_PLAYER_MENU_TASK]) {
        task_delete(gPlayerMenuData[playerid][E_PLAYER_MENU_TASK]);
    }

    for (new i; i != MODEL_DIALOG_PAGE_ITEMS; ++i) {
        gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_DESC][i] = INVALID_PLAYER_TEXT_DRAW;
    }

    gPlayerMenuData[playerid][E_PLAYER_MENU_TASK] = INVALID_TASK;
    ResetModelDialogMenuData(playerid);
}

/**
 * # External
 */

stock void:AddModelDialogMenuItem(List:list, modelid, const description[] = "", Float:sizeX = 53.000, Float:sizeY = 53.000, Float:rotationX = 0.000, Float:rotationY = 0.000, Float:rotationZ = 0.000, Float:zoom = 1.000, Float:offsetX = 0.000, Float:offsetY = 0.000) {
    new
        data[E_DIALOG_PREVIEW_DATA]
    ;

    strcat(data[E_DIALOG_PREVIEW_DESC], description);
    data[E_DIALOG_PREVIEW_MODEL_ID] = modelid;
    data[E_DIALOG_PREVIEW_SIZE_X] = sizeX;
    data[E_DIALOG_PREVIEW_SIZE_Y] = sizeY;
    data[E_DIALOG_PREVIEW_ROTATION_X] = rotationX;
    data[E_DIALOG_PREVIEW_ROTATION_Y] = rotationY;
    data[E_DIALOG_PREVIEW_ROTATION_Z] = rotationZ;
    data[E_DIALOG_PREVIEW_ZOOM] = zoom;
    data[E_DIALOG_PREVIEW_OFFSET_X] = offsetX;
    data[E_DIALOG_PREVIEW_OFFSET_Y] = offsetY;

    list_add_arr(list, data);
}

stock bool:ShowModelDialogMenu(playerid, List:list, const caption[], const button1[], const button2[], start = 0) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (gPlayerMenuData[playerid][E_PLAYER_MENU_LIST]) {
        PlayerTextDrawSetString(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_CAPTION], caption);
        PlayerTextDrawSetString(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_FIRST_BUTTON], button1);
        PlayerTextDrawSetString(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_SECOND_BUTTON], button2);
    } else {
        ShowModelDialogMenuTextDraws(playerid, caption, button1, button2);
        SelectTextDraw(playerid, 0xFF0000FF);
    }

    gPlayerMenuData[playerid][E_PLAYER_MENU_LIST] = list;
    gPlayerMenuData[playerid][E_PLAYER_MENU_SIZE] = list_size(list);
    ShowModelDialogMenuPage(playerid, start);
    
    return true;
}

stock Task:ShowAsyncModelDialogMenu(playerid, List:list, const caption[], const button1[], const button2[], start = 0) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_TASK;
    }

    if (gPlayerMenuData[playerid][E_PLAYER_MENU_TASK]) {
        task_delete(gPlayerMenuData[playerid][E_PLAYER_MENU_TASK]);
    }

    new const
        Task:t = task_new()
    ;

    gPlayerMenuData[playerid][E_PLAYER_MENU_TASK] = t;
    ShowModelDialogMenu(playerid, list, caption, button1, button2, start);

    return t;
}

stock bool:HideModelDialogMenu(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!gPlayerMenuData[playerid][E_PLAYER_MENU_LIST]) {
        return false;
    }

    for (new i; i != 9; ++i) {
        TextDrawHideForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_BACKGROUND][i]);
    }

    for (new i; i != MODEL_DIALOG_PAGE_ITEMS; ++i) {
        TextDrawHideForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_BOX_BUTTON][i]);
        TextDrawHideForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_BOX_TOP][i]);
        TextDrawHideForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_MODEL_PREVIEW][i]);

        PlayerTextDrawDestroy(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_DESC][i]);
        gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_DESC][i] = INVALID_PLAYER_TEXT_DRAW;
    }

    TextDrawHideForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_UP_BUTTON]);
    TextDrawHideForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_DOWN_BUTTON]);

    TextDrawHideForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_SELECT_BUTTON]);
    TextDrawHideForPlayer(playerid, gTextDrawData[E_TEXT_DRAW_CANCEL_BUTTON]);

    PlayerTextDrawDestroy(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_SCROLL_BAR]);
    PlayerTextDrawDestroy(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_CAPTION]);
    PlayerTextDrawDestroy(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_PAGE]);
    PlayerTextDrawDestroy(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_FIRST_BUTTON]);
    PlayerTextDrawDestroy(playerid, gPlayerTextDrawData[playerid][E_PLAYER_TEXT_DRAW_SECOND_BUTTON]);

    ResetModelDialogMenuData(playerid);
    CancelSelectTextDraw(playerid);

    return true;
}

/**
 * # Calls
 */

hook OnGameModeInit() {
    new
        Text:t
    ;

    t = gTextDrawData[E_TEXT_DRAW_BACKGROUND][0] = TextDrawCreate(139.000, 131.000, "LD_DUAL:BLACK");
    TextDrawTextSize(t, 361.500, 12.000);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);

    t = gTextDrawData[E_TEXT_DRAW_BACKGROUND][1] = TextDrawCreate(139.000, 143.000, "LD_DUAL:BLACK");
    TextDrawTextSize(t, 361.500, 172.500);
    TextDrawColour(t, 200);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);

    t = gTextDrawData[E_TEXT_DRAW_BACKGROUND][2] = TextDrawCreate(145.000, 150.000, "LD_DUAL:WHITE");
    TextDrawTextSize(t, 349.500, 135.000);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);

    t = gTextDrawData[E_TEXT_DRAW_BACKGROUND][3] = TextDrawCreate(145.500, 150.500, "LD_DUAL:BLACK");
    TextDrawTextSize(t, 338.000, 134.000);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);

    t = gTextDrawData[E_TEXT_DRAW_BACKGROUND][4] = TextDrawCreate(484.000, 150.500, "LD_DUAL:BLACK");
    TextDrawTextSize(t, 10.000, 134.000);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);

    // SCROLL BAR
    t = gTextDrawData[E_TEXT_DRAW_BACKGROUND][5] = TextDrawCreate(482.000, 148.000, "LD_BEAT:CHIT");
    TextDrawTextSize(t, 14.000, 14.000);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);

    t = gTextDrawData[E_TEXT_DRAW_BACKGROUND][6] = TextDrawCreate(482.000, 273.000, "LD_BEAT:CHIT");
    TextDrawTextSize(t, 14.000, 14.000);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);

    t = gTextDrawData[E_TEXT_DRAW_UP_BUTTON] = TextDrawCreate(485.000, 151.000, "LD_BEAT:UP");
    TextDrawTextSize(t, 8.000, 8.000);
    TextDrawColour(t, 255);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);
    TextDrawSetSelectable(t, true);

    t = gTextDrawData[E_TEXT_DRAW_DOWN_BUTTON] = TextDrawCreate(485.000, 276.500, "LD_BEAT:DOWN");
    TextDrawTextSize(t, 8.000, 8.000);
    TextDrawColour(t, 255);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);
    TextDrawSetSelectable(t, true);

    // BUTTON
    t = gTextDrawData[E_TEXT_DRAW_SELECT_BUTTON] = TextDrawCreate(264.500, 292.500, "LD_DUAL:WHITE");
    TextDrawTextSize(t, 54.000, 15.000);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);
    TextDrawSetSelectable(t, true);

    t = gTextDrawData[E_TEXT_DRAW_CANCEL_BUTTON] = TextDrawCreate(320.500, 292.500, "LD_DUAL:WHITE");
    TextDrawTextSize(t, 54.000, 15.000);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);
    TextDrawSetSelectable(t, true);

    t = gTextDrawData[E_TEXT_DRAW_BACKGROUND][7] = TextDrawCreate(265.000, 293.000, "LD_DUAL:BLACK");
    TextDrawTextSize(t, 53.000, 14.000);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);

    t = gTextDrawData[E_TEXT_DRAW_BACKGROUND][8] = TextDrawCreate(321.000, 293.000, "LD_DUAL:BLACK");
    TextDrawTextSize(t, 53.000, 14.000);
    TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);

    // BOX SELECTABLE
    for (new i; i != MODEL_DIALOG_PAGE_ITEMS; i++) {
        t = gTextDrawData[E_TEXT_DRAW_BOX_BUTTON][i] = TextDrawCreate(
            147.500 + i % MODEL_DIALOG_COLUMNS * 56.000,
            152.500 + i / MODEL_DIALOG_COLUMNS * 66.000, "LD_DUAL:WHITE"
        );
        TextDrawTextSize(t, 54.000, 64.000);
        TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);
        TextDrawSetSelectable(t, true);
        gTextDrawBoxMenuItem[t] = i;

        t = gTextDrawData[E_TEXT_DRAW_BOX_TOP][i] = TextDrawCreate(
            148.000 + i % MODEL_DIALOG_COLUMNS * 56.000,
            153.000 + i / MODEL_DIALOG_COLUMNS * 66.000, "LD_DUAL:WHITE"
        );
        TextDrawTextSize(t, 53.000, 63.000);
        TextDrawColour(t, 0x323232FF);
        TextDrawFont(t, TEXT_DRAW_FONT_SPRITE);

        t = gTextDrawData[E_TEXT_DRAW_MODEL_PREVIEW][i] = TextDrawCreate(0.000, 0.000, "_");
        TextDrawBackgroundColour(t, 0);
        TextDrawFont(t, TEXT_DRAW_FONT_PREVIEW);
    }

    return 0;
}

hook OnPlayerConnect(playerid) {
    ResetModelDialogMenuDataAndTask(playerid);

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    ResetModelDialogMenuDataAndTask(playerid);

    return 0;
}

hook OnPlayerClickTextDraw(playerid, Text:clickedid) {
    if (gPlayerMenuData[playerid][E_PLAYER_MENU_LIST]) {
        if (clickedid != INVALID_TEXT_DRAW) {
            new const
                menuItemClicked = gTextDrawBoxMenuItem[clickedid]
            ;

            if (menuItemClicked >= 0) {
                SelectModelDialogMenuItem(playerid, menuItemClicked);

                return 1;
            }
        }

        new const
            p = gPlayerMenuData[playerid][E_PLAYER_MENU_PAGE]
        ;

        if (clickedid == gTextDrawData[E_TEXT_DRAW_UP_BUTTON] && p > 0) {
            ShowModelDialogMenuPage(playerid, p - 1);

            return 1;
        }

        if (clickedid == gTextDrawData[E_TEXT_DRAW_DOWN_BUTTON] && p < (gPlayerMenuData[playerid][E_PLAYER_MENU_SIZE] + MODEL_DIALOG_PAGE_ITEMS - 1) / MODEL_DIALOG_PAGE_ITEMS - 1) {
            ShowModelDialogMenuPage(playerid, p + 1);

            return 1;
        }

        new const
            menuItemChosen = gPlayerMenuData[playerid][E_PLAYER_MENU_ITEM_CHOSEN],
            menuItem = menuItemChosen + p * MODEL_DIALOG_PAGE_ITEMS
        ;

        if (clickedid == gTextDrawData[E_TEXT_DRAW_SELECT_BUTTON] && menuItemChosen >= 0) {
            HandleModelDialogMenuSelection(playerid, MODEL_DIALOG_RESPONSE_SELECT, menuItem);

            return 1;
        }

        if (clickedid == INVALID_TEXT_DRAW || clickedid == gTextDrawData[E_TEXT_DRAW_CANCEL_BUTTON]) {
            HandleModelDialogMenuSelection(playerid, MODEL_DIALOG_RESPONSE_CANCEL, (menuItemChosen >= 0) ? menuItem : -1);

            return 1;
        }
    }

    return 0;
}