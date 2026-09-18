#if defined _CORE_PLAYER_HANDS
    #endinput
#endif
#define _CORE_PLAYER_HANDS

#include <pp-hooks>

/**
 * # What the hands can do
 *
 * ScavengeSurvive gives a held item two ways out: onto the ground, or into the
 * pockets. The framework already binds the first one -- ITEM_KEY_DROP_ITEM goes
 * through PlayerDropItem, which drops the object where the character is
 * standing -- and this is the second, which it has no opinion about because
 * where an item goes when it leaves the hands is a gamemode's decision.
 *
 * Both are advertised through core/prompt while something is being carried, so
 * the keys do not have to be learned from a manual.
 *
 * The prompt offers only what the key actually does. A crate has an inside, so
 * it goes in no pocket, and offering the stow key on it is telling the
 * character to press something that answers with a refusal.
 *
 * The open key is offered alongside it, because with nothing in front of the
 * character it falls back to what they are carrying -- and this offer only ever
 * reaches the screen when nothing in reach outbid it, which is exactly when
 * that is true.
 */

/**
 * # Functions
 */

/**
 * @brief      Put whatever is in a character's hands away.
 *
 * Whatever is open takes it, and the pockets only when nothing is.
 * Standing at a locker with a rifle in hand and having the key insist on a
 * pocket that will not take it is the key answering a question nobody
 * asked: the character is looking into the locker, so that is where it
 * goes.
 *
 * The framework binds the other way out -- the drop key goes through
 * PlayerDropItem -- and has no opinion about this one, because where an
 * item goes when it leaves the hands is a gamemode's decision.
 *
 * @param      playerid  Character to empty the hands of.
 * @param      notify    Whether a refusal is explained on their screen.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is now in their pockets
 *             - `false` when:
 *                 + their hands are empty
 *                 + the item has an inside, so it cannot be stored
 *                 + their pockets are full
 *                 + the inventory refused it
 */
forward bool:StoreHeldItem(playerid, bool:notify = true);

/**
 * # External
 */

stock bool:StoreHeldItem(playerid, bool:notify = true) {
    new const
        Item:itemid = GetPlayerHoldItem(playerid)
    ;

    if (!IsValidItem(itemid)) {
        return false;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    new
        Container:containerid = GetPlayerOpenContainer(playerid)
    ;

    if (containerid == INVALID_CONTAINER_ID) {
        containerid = GetPlayerContainer(playerid);
    }

    if (!IsValidContainer(containerid)) {
        return false;
    }

    if (!CanPutItemInContainer(itemid, containerid)) {
        if (notify) {
            new
                where[MAX_CONTAINER_NAME]
            ;

            GetContainerName(containerid, where);

            SendPlayerNotice(playerid, "The %s does not go in the %s. Put it down with %s.", name, where, ITEM_KEY_DROP_NAME);
        }

        return false;
    }

    if (IsContainerFull(containerid)) {
        if (notify) {
            SendPlayerNotice(playerid, "There is no room for the %s.", name);
        }

        return false;
    }

    new
        index = -1
    ;

    if (containerid == GetPlayerContainer(playerid)) {
        if (!AddItemToInventory(playerid, itemid, index)) {
            if (notify) {
                SendPlayerNotice(playerid, "You cannot put the %s away.", name);
            }

            return false;
        }
    } else if (!AddItemToContainer(containerid, itemid, index, .playerid = playerid)) {
        if (notify) {
            SendPlayerNotice(playerid, "You cannot put the %s away.", name);
        }

        return false;
    }

    RemoveCurrentItem(playerid);

    UpdatePlayerPrompt(playerid);

    if (notify) {
        SendPlayerNotice(playerid, "You put the %s away.", name);
    }

    return true;
}

/**
 * # Calls
 */

hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if (!(newkeys & ITEM_KEY_STORE_ITEM)) {
        return 0;
    }

    if (IsPlayerInAnyVehicle(playerid)) {
        return 0;
    }

    StoreHeldItem(playerid);

    return 0;
}

hook OnPlayerDropItem(playerid, Item:itemid) {
    if (HasItemAttribute(itemid, ITEM_ATTRIBUTE_DROPPABLE)) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    SendPlayerNotice(playerid, "You are not leaving the %s on the ground.", name);

    return 1;
}

hook OnPlayerUpdate(playerid) {
    if (GetPlayerSpecialAction(playerid) != SPECIAL_ACTION_CARRY) {
        return 1;
    }

    if (GetPlayerWeapon(playerid) == WEAPON_FIST) {
        return 1;
    }

    SetPlayerArmedWeapon(playerid, WEAPON_FIST);

    return 1;
}

hook OnPlayerGotItem(playerid, Item:itemid) {
    UpdatePlayerPrompt(playerid);

    return 0;
}

hook OnPlayerDroppedItem(playerid, Item:itemid) {
    UpdatePlayerPrompt(playerid);

    return 0;
}

hook OnPlayerRequestPrompt(playerid) {
    new const
        Item:itemid = GetPlayerHoldItem(playerid)
    ;

    if (!IsValidItem(itemid)) {
        return 0;
    }

    new
        name[MAX_ITEM_NAME]
    ;

    GetItemName(itemid, name);

    new
        Container:containerid = GetPlayerOpenContainer(playerid)
    ;

    if (containerid == INVALID_CONTAINER_ID) {
        containerid = GetPlayerContainer(playerid);
    }

    new const
        bool:stowable = CanPutItemInContainer(itemid, containerid),
        bool:droppable = HasItemAttribute(itemid, ITEM_ATTRIBUTE_DROPPABLE)
    ;

    if (IsItemOpenable(itemid)) {
        if (stowable) {
            OfferPlayerPrompt(playerid, PLAYER_PROMPT_HELD, "Press %s to open the %s, %s to stow it or %s to put it down.", ITEM_KEY_OPEN_NAME, name, ITEM_KEY_STORE_NAME, ITEM_KEY_DROP_NAME);
        } else {
            OfferPlayerPrompt(playerid, PLAYER_PROMPT_HELD, "Press %s to open the %s, or %s to put it down.", ITEM_KEY_OPEN_NAME, name, ITEM_KEY_DROP_NAME);
        }
    } else if (stowable && droppable) {
        OfferPlayerPrompt(playerid, PLAYER_PROMPT_HELD, "Press %s to stow or %s to drop the %s.", ITEM_KEY_STORE_NAME, ITEM_KEY_DROP_NAME, name);
    } else if (stowable) {
        OfferPlayerPrompt(playerid, PLAYER_PROMPT_HELD, "Press %s to stow the %s.", ITEM_KEY_STORE_NAME, name);
    } else if (droppable) {
        OfferPlayerPrompt(playerid, PLAYER_PROMPT_HELD, "Press %s to put the %s down.", ITEM_KEY_DROP_NAME, name);
    }

    return 0;
}
