#if defined _INC_DIALOG_PAGE_PLUS
    #endinput
#endif
#define _INC_DIALOG_PAGE_PLUS

#include <PawnPlus>

#include <YSI_Visual\y_dialog>
#include <YSI_Coding\y_hooks>

/**
 * # Header
 */

#if !defined MAX_DIALOG_PAGE_BODY_LENGTH
    #define MAX_DIALOG_PAGE_BODY_LENGTH \
        (4096)
#endif

#if !defined MAX_DIALOG_PAGE_TITLE_LENGTH
    #define MAX_DIALOG_PAGE_TITLE_LENGTH \
        (64)
#endif

#if !defined MAX_DIALOG_PAGE_ITEM_LENGTH
    #define MAX_DIALOG_PAGE_ITEM_LENGTH \
        (128)
#endif

#if !defined MAX_DIALOG_PAGE_BUTTON_LENGTH
    #define MAX_DIALOG_PAGE_BUTTON_LENGTH \
        (32)
#endif

#if !defined MAX_DIALOG_PAGE_LISTITEMS
    #define MAX_DIALOG_PAGE_LISTITEMS \
        (128)
#endif

static enum _:E_DIALOG_PAGE_DATA {
    Func:E_DIALOG_PAGE_CALLBACK<iiis>,

    DIALOG_STYLE:E_DIALOG_PAGE_STYLE,
    E_DIALOG_PAGE_MAX,
    List:E_DIALOG_PAGE_BODY_LIST,

    E_DIALOG_PAGE_TITLE[MAX_DIALOG_PAGE_TITLE_LENGTH],
    E_DIALOG_PAGE_HEADER[MAX_DIALOG_PAGE_ITEM_LENGTH],

    E_DIALOG_PAGE_BUTTON_1[MAX_DIALOG_PAGE_BUTTON_LENGTH],
    E_DIALOG_PAGE_BUTTON_2[MAX_DIALOG_PAGE_BUTTON_LENGTH],

    E_DIALOG_PAGE_BUTTON_NEXT[MAX_DIALOG_PAGE_BUTTON_LENGTH],
    E_DIALOG_PAGE_BUTTON_BACK[MAX_DIALOG_PAGE_BUTTON_LENGTH]
};

static enum _:E_DIALOG_PAGE_ITEM_DATA {
    E_DIALOG_PAGE_ITEM_LISTITEM_ID,
    E_DIALOG_PAGE_ITEM_NAME[MAX_DIALOG_PAGE_ITEM_LENGTH]
};

static
    Pool:gDialogPageData
;

/**
 * # Functions
 */

forward bool:DialogPage_AddItem(List:body, listItem, const format[], OPEN_MP_TAGS:...);
forward bool:DialogPage_ShowCallback(playerid, Func:cb<iiis>, DIALOG_STYLE:style, const title[], List:body, const button1[], const button2[] = "", max = 10, start = 0, const buttonNext[] = ">>>", const buttonBack[] = "<<<");
forward bool:DialogPage_Close(playerid);

/**
 * # Internal
 */

static bool:DialogPage_IsValidStyle(DIALOG_STYLE:style) {
    return (
        style == DIALOG_STYLE_LIST ||
        style == DIALOG_STYLE_TABLIST ||
        style == DIALOG_STYLE_TABLIST_HEADERS
    );
}

static DialogPage_ShowCallbackInternal(playerid, start) {
    if (!pool_valid(gDialogPageData)) {
        Debug_PrintE("(DialogPage_ShowCallbackInternal) -> Invalid pool.");
        
        return;
    }

    if (!pool_has(gDialogPageData, playerid)) {
        Debug_PrintE("(DialogPage_ShowCallbackInternal) -> Player (%i) is not in the pool.", playerid);
        
        return;
    }

    new
        data[E_DIALOG_PAGE_DATA],
        item[E_DIALOG_PAGE_ITEM_DATA]
    ;

    static
        body[MAX_DIALOG_PAGE_BODY_LENGTH],
        listItems[MAX_DIALOG_PAGE_LISTITEMS]
    ;

    body[0] = EOS;
    listItems[0] = EOS;

    pool_get_arr(
        gDialogPageData,
        playerid,
        data
    );

    if (data[E_DIALOG_PAGE_STYLE] == DIALOG_STYLE_TABLIST_HEADERS) {
        strcat(body, data[E_DIALOG_PAGE_HEADER]);
    }

    new const
        List:buf = data[E_DIALOG_PAGE_BODY_LIST],
        maxItems = data[E_DIALOG_PAGE_MAX],
        listSize = list_size(buf),
        endIndex = floatround((listSize - 1) / maxItems, floatround_ceil),
        listIter = list_iter(buf, nextIndex) // Wrong place
    ;

    new
        nextIndex = start * maxItems,
        lastIndex = start < endIndex ? nextIndex + maxItems : listSize
    ;

    while (iter_inside(listIter) && nextIndex < lastIndex) {
        iter_get_arr(listIter, item);

        strcat(body, item[E_DIALOG_PAGE_ITEM_NAME]);
        strcat(body, "\n");

        listItems[nextIndex++] = item[E_DIALOG_PAGE_ITEM_LISTITEM_ID];
        iter_move_next(listIter);
    }

    if (start < endIndex) {
        strcat(body, data[E_DIALOG_PAGE_BUTTON_NEXT]);
        strcat(body, "\n");
    }

    if (start > 0) {
        strcat(body, data[E_DIALOG_PAGE_BUTTON_BACK]);
    }

    inline const OnResponse(response, listitem, string:inputtext[]) {
        if (response) {
            if (start < endIndex && listitem == maxItems) {
                DialogPage_ShowCallbackInternal(playerid, start + 1);

                @return 1;
            }

            if (listitem == (start == endIndex ? listSize - start * maxItems : maxItems + 1)) {
                DialogPage_ShowCallbackInternal(playerid, start - 1);

                @return 1;
            }

            new const
                index = listitem + start * maxItems
            ;

            DialogPage_CallInline(playerid, data[E_DIALOG_PAGE_CALLBACK], response, listItems[index], inputtext);
        } else {
            DialogPage_CallInline(playerid, data[E_DIALOG_PAGE_CALLBACK], response, -1, #);
        }
    }

    Dialog_ShowCallback(playerid, using inline OnResponse,
        data[E_DIALOG_PAGE_STYLE],
        data[E_DIALOG_PAGE_TITLE],
        body,
        data[E_DIALOG_PAGE_BUTTON_1],
        data[E_DIALOG_PAGE_BUTTON_2]
    );
}

static DialogPage_CallInline(playerid, Func:cb<iiis>, response, listItem, const inputtext[]) {
    DialogPage_Close(playerid);
    @.cb(playerid, response, listItem, inputtext);
    Indirect_Release(cb);
}

/**
 * # External
 */

stock bool:DialogPage_AddItem(List:body, listItem, const format[], OPEN_MP_TAGS:...) {
    if (!list_valid(body)) {
        Debug_PrintE("(DialogPage_AddItem) -> List (%i) invalid.", _:body);

        return false;
    }

    if (list_size(body) >= (MAX_DIALOG_PAGE_LISTITEMS - 1)) {
        Debug_PrintW("(DialogPage_AddItem) -> List (%i) is full.", _:body);

        return false;
    }

    new
        buffer[MAX_DIALOG_PAGE_ITEM_LENGTH],
        data[E_DIALOG_PAGE_ITEM_DATA]
    ;

    Format(
        buffer,
        _,
        format,
        ___(3)
    );

    data[E_DIALOG_PAGE_ITEM_LISTITEM_ID] = listItem;
    strcat(data[E_DIALOG_PAGE_ITEM_NAME], buffer);

    list_add_arr(body, data);

    return true;
}

stock bool:DialogPage_ShowCallback(playerid, Func:cb<iiis>, DIALOG_STYLE:style, const title[], List:body, const button1[], const button2[] = "", max = 10, start = 0, const buttonNext[] = ">>>", const buttonBack[] = "<<<") {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!DialogPage_IsValidStyle(style)) {
        Debug_PrintE("(DialogPage_ShowCallback) -> Player's dialog style (%i) is invalid.", playerid);

        return false;
    }

    if (!list_valid(body)) {
        Debug_PrintE("(DialogPage_ShowCallback) -> Player's item list (%i) is invalid.", playerid);

        return false;
    }

    if (gDialogPageData == INVALID_POOL) {
        gDialogPageData = pool_new();
    } else {
        if (pool_has(gDialogPageData, playerid)) {
            new
                data[E_DIALOG_PAGE_DATA]
            ;

            pool_get_arr(
                gDialogPageData,
                playerid,
                data
            );

            if (list_valid(data[E_DIALOG_PAGE_BODY_LIST])) {
                list_delete(data[E_DIALOG_PAGE_BODY_LIST]);
            }
        }
    }

    new
        data[E_DIALOG_PAGE_DATA]
    ;

    if (style == DIALOG_STYLE_TABLIST_HEADERS) {
        new
            item[E_DIALOG_PAGE_ITEM_DATA]
        ;

        list_get_arr(
            body,
            0,
            item
        );

        strcat(data[E_DIALOG_PAGE_HEADER], item[E_DIALOG_PAGE_ITEM_NAME]);
        strcat(data[E_DIALOG_PAGE_HEADER], "\n");
        
        list_remove(body, 0);
    }

    Indirect_Claim(cb);
    data[E_DIALOG_PAGE_CALLBACK] = cb;

    strcat(data[E_DIALOG_PAGE_TITLE], title);
    data[E_DIALOG_PAGE_STYLE] = style;
    data[E_DIALOG_PAGE_MAX] = max;
    data[E_DIALOG_PAGE_BODY_LIST] = body;

    strcat(data[E_DIALOG_PAGE_BUTTON_1], button1);
    strcat(data[E_DIALOG_PAGE_BUTTON_2], button2);

    strcat(data[E_DIALOG_PAGE_BUTTON_NEXT], buttonNext);
    strcat(data[E_DIALOG_PAGE_BUTTON_BACK], buttonBack);

    pool_set_arr(gDialogPageData, playerid, data);

    // Show
    DialogPage_ShowCallbackInternal(playerid, start);

    return true;
}

stock bool:DialogPage_Close(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }
    
    if (!pool_valid(gDialogPageData)) {
        return false;
    }

    if (!pool_has(gDialogPageData, playerid)) {
        return false;
    }

    new
        data[E_DIALOG_PAGE_DATA]
    ;

    pool_get_arr(
        gDialogPageData,
        playerid,
        data
    );
    
    if (list_valid(data[E_DIALOG_PAGE_BODY_LIST])) {
        list_delete(data[E_DIALOG_PAGE_BODY_LIST]);
    }

    pool_remove(gDialogPageData, playerid);

    if (pool_size(gDialogPageData) == 0) {
        pool_delete(gDialogPageData);
        gDialogPageData = INVALID_POOL;
    }

    return true;
}

/**
 * # Hooks
 */

hook OnPlayerDisconnect(playerid, reason) {
    DialogPage_Close(playerid);
    
    return 1;
}