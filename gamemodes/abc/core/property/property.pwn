#if defined _CORE_PROPERTY_PROPERTY
    #endinput
#endif
#define _CORE_PROPERTY_PROPERTY

#include <pp-hooks>

// Money and words for every kind of property. The library owns the door, the
// world behind it and the deed; paying for one is the same wherever it leads,
// so it is written once here and property/house adds only what a house needs.

hook OnPlayerRequestPrompt(playerid) {
    new
        prompt[MAX_BUTTON_POPUP_LENGTH]
    ;

    if (GetPlayerPropertyPrompt(playerid, prompt)) {
        // Through %s: the library's sentence carries a price and could carry a
        // percent sign, which OfferPlayerPrompt would read as a placeholder.
        OfferPlayerPrompt(playerid, PLAYER_PROMPT_PROPERTY, "%s", prompt);
    }

    return 0;
}

hook OnPlayerPropertyPromptChange(playerid, Property:propertyid) {
    UpdatePlayerPrompt(playerid);

    return 0;
}

hook OnPlayerBuyProperty(playerid, Property:propertyid, price) {
    if (GetPlayerMoney(playerid) < price) {
        new
            money[16]
        ;

        HumanizeThousand(price, money);

        SendPlayerNotice(playerid, "That costs $%s, and you do not have it.", money);

        return 1;
    }

    return 0;
}

hook OnPlayerBoughtProperty(playerid, Property:propertyid, price, previousPlayer, previousExtra) {
    new
        name[MAX_PROPERTY_NAME],
        money[16]
    ;

    GetPropertyName(propertyid, name);
    HumanizeThousand(price, money);

    GivePlayerMoney(playerid, -price);

    SendPlayerNotice(playerid, "%s is yours, for $%s.", name, money);

    // A sale between two people is a payment to somebody rather than to nobody,
    // which is the only reason the seller is handed over at all.
    if (previousPlayer == INVALID_PLAYER_ID) {
        return 0;
    }

    GivePlayerMoney(previousPlayer, price);

    SendPlayerNotice(previousPlayer, "%s sold for $%s.", name, money);

    return 0;
}

hook OnPlayerSoldProperty(playerid, Property:propertyid, price) {
    new const
        refund = price / PROPERTY_REFUND_SHARE
    ;

    new
        name[MAX_PROPERTY_NAME],
        money[16]
    ;

    GetPropertyName(propertyid, name);
    HumanizeThousand(refund, money);

    GivePlayerMoney(playerid, refund);

    SendPlayerNotice(playerid, "You give up %s for $%s.", name, money);

    return 0;
}

hook OnPlayerPropertyLocked(playerid, Property:propertyid) {
    new
        kind[MAX_PROPERTY_TYPE_NAME]
    ;

    // The kind and not the name: which one it is is already on the door.
    GetPropertyName(propertyid, kind, .kind = true);

    SendPlayerNotice(playerid, "That %s is locked.", kind);

    return 0;
}
