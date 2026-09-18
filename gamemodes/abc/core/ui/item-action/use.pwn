#if defined _CORE_UI_ITEM_ACTION_USE
    #endinput
#endif
#define _CORE_UI_ITEM_ACTION_USE

#include <pp-hooks>

/**
 * # USE
 *
 * The item stops existing. What it did before it went -- fed someone, stopped a
 * bleed, made a call -- is not decided here: this is the row, and a condition
 * or phone library is what would hook the same callback beside it and act on
 * the kind of item first.
 *
 * That is the shape every action file has. It answers for one row, does the one
 * thing the framework already offers, and returns 1 so nothing else does.
 *
 * Nothing eaten or drunk draws this row any more. Those are picked up and used
 * in the hands, and a row here would be PICK UP with a step after it -- worse
 * than redundant, because what this does with anything it does not recognise is
 * consume it, and a full can is not rubbish. catalogue/consumable says which
 * items keep the row and why.
 *
 * The food branch below is what happens if something eaten is marked usable
 * again: it puts it in the hands and lets OnPlayerUseItem start the eating,
 * rather than letting it fall past here and be thrown away.
 */

hook OnPlayerSelectItemAction(playerid, ItemAction:actionid, Item:itemid, ContainerMenu:menuid, slot) {
    if (actionid != gItemActionUse) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    if (IsItemGift(itemid)) {
        new
            ItemBuild:given = INVALID_ITEM_BUILD_ID
        ;

        if (!UnwrapItem(playerid, itemid, given)) {
            SendPlayerNotice(playerid, "You have no room for what is in the %s.", name);

            return 1;
        }

        new
            givenName[MAX_ITEM_BUILD_NAME]
        ;

        GetItemBuildName(given, givenName);

        SendPlayerNotice(playerid, "You unwrap the %s: %s.", name, givenName);

        return 1;
    }

    if (IsItemFood(itemid)) {
        if (GetPlayerHoldItem(playerid) == itemid) {
            EatItem(playerid, itemid);

            return 1;
        }

        if (!HoldContainerItem(playerid, GetContainerMenuContainer(playerid, menuid), itemid)) {
            new const
                Item:held = GetPlayerHoldItem(playerid)
            ;

            if (IsValidItem(held)) {
                new
                    heldName[MAX_ITEM_NAME]
                ;

                GetItemName(held, heldName);

                SendPlayerNotice(playerid, "Put the %s down first, then eat.", heldName);
            }

            return 1;
        }

        EatItem(playerid, itemid);

        return 1;
    }

    if (!ConsumeContainerItem(playerid, GetContainerMenuContainer(playerid, menuid), itemid)) {
        SendPlayerNotice(playerid, "You are not carrying that.");

        return 1;
    }

    SendPlayerNotice(playerid, "You use the %s.", name);

    return 1;
}
