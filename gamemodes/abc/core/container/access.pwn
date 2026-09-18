#if defined _CORE_CONTAINER_ACCESS
    #endinput
#endif
#define _CORE_CONTAINER_ACCESS

#include <pp-hooks>

/**
 * # What a container lets a character do to it
 *
 * A crate, a bin and a shop counter are all a container with things in it. What
 * tells them apart is not what they hold but what a character is allowed to do
 * with what they hold, and that is one answer per container rather than a
 * different kind of container for each.
 *
 *     free      a crate, a locker, a bag -- anything in, anything out
 *     dump      a bin -- anything in, and nothing comes back out
 *     shop      a counter -- nothing in, and taking is buying
 *     sealed    a window -- read it, touch nothing
 *
 * A shop is the reason this exists. Moving an item out of a crate and buying
 * one off a counter look identical on the screen and are not the same act: one
 * is the character's to make and the other has to be agreed to first. Refusing
 * the move and asking instead is the whole difference, and it is a rule about
 * the container rather than a rule about the item in it.
 *
 * What buying costs is not here. This module knows that taking from a shop is a
 * purchase and nothing else about it -- OnPlayerBuyContainerItem is where a
 * price, a wallet and a refusal belong, and a gamemode with no money at all
 * simply never allows one.
 *
 * Every container is free until something says otherwise, so pockets, bags and
 * everything already written carry on as they were.
 */

enum E_CONTAINER_ACCESS {
    CONTAINER_ACCESS_FREE,
    CONTAINER_ACCESS_DUMP,
    CONTAINER_ACCESS_SHOP,
    CONTAINER_ACCESS_SEALED
};

static
    E_CONTAINER_ACCESS:gContainerAccess[MAX_CONTAINERS]
;

/**
 * # Functions
 */

/**
 * @brief      Say what a container lets a character do to it.
 *
 * @param      containerid  Container to rule on.
 * @param      access       Which of the four it is.
 *
 * @date       15:40 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is now that kind
 *             - `false` `containerid` is not a live container
 */
forward bool:SetContainerAccess(Container:containerid, E_CONTAINER_ACCESS:access);

/**
 * @brief      What a container lets a character do to it.
 *
 * @param      containerid  Container to read.
 *
 * @date       15:40 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - which of the four it is
 *             - `CONTAINER_ACCESS_FREE` when it has never been told, which
 *               is what a container is until it is
 */
forward E_CONTAINER_ACCESS:GetContainerAccess(Container:containerid);

/**
 * @brief      Whether things may be moved out of a container at all.
 *
 * A shop answers `true`: things do come out of a shop. Whether this
 * particular one comes out is OnPlayerBuyContainerItem's to say, and asking
 * that costs a callback -- so this is the cheap question that comes first.
 *
 * @param      containerid  Container to read.
 *
 * @date       15:40 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` something can leave it
 *             - `false` it is a bin or sealed
 */
forward bool:CanTakeFromContainer(Container:containerid);

/**
 * @brief      Whether things may be put into a container.
 *
 * @param      containerid  Container to read.
 *
 * @date       15:40 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` something can go in
 *             - `false` it is a shop or sealed
 */
forward bool:CanPutInContainer(Container:containerid);

/**
 * @brief      Whether taking from a container has to be paid for.
 *
 * @param      containerid  Container to read.
 *
 * @date       15:40 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is a counter and taking is buying
 *             - `false` anything else
 */
forward bool:IsContainerShop(Container:containerid);

/**
 * @brief      A character is trying to take something off a counter.
 *
 * Fired instead of the move, not before it: nothing has been taken when
 * this is called and nothing will be unless it is allowed. Charge here, and
 * say whether it was paid for.
 *
 * A shop that is out of the character's price range is not a failure of the
 * inventory, so tell them why here -- refusing silently reads as a bug in
 * the menu.
 *
 * @param      playerid     Character buying.
 * @param      containerid  Counter it is coming off.
 * @param      itemid       What they clicked.
 * @param      index        Where it sits on the counter.
 *
 * @date       15:40 12/09/2026
 * @author     augustocaixeta
 *
 * @return     - `1` it is paid for, hand it over
 *             - `0` it is not, leave it where it is
 */
forward OnPlayerBuyContainerItem(playerid, Container:containerid, Item:itemid, index);

/**
 * # External
 */

stock bool:SetContainerAccess(Container:containerid, E_CONTAINER_ACCESS:access) {
    if (!IsValidContainer(containerid)) {
        return false;
    }

    gContainerAccess[containerid] = access;

    return true;
}

stock E_CONTAINER_ACCESS:GetContainerAccess(Container:containerid) {
    if (!IsValidContainer(containerid)) {
        return CONTAINER_ACCESS_FREE;
    }

    return gContainerAccess[containerid];
}

stock bool:CanTakeFromContainer(Container:containerid) {
    new const
        E_CONTAINER_ACCESS:access = GetContainerAccess(containerid)
    ;

    if (access == CONTAINER_ACCESS_DUMP) {
        return false;
    }

    if (access == CONTAINER_ACCESS_SEALED) {
        return false;
    }

    return true;
}

stock bool:CanPutInContainer(Container:containerid) {
    new const
        E_CONTAINER_ACCESS:access = GetContainerAccess(containerid)
    ;

    if (access == CONTAINER_ACCESS_SHOP) {
        return false;
    }

    if (access == CONTAINER_ACCESS_SEALED) {
        return false;
    }

    return true;
}

stock bool:IsContainerShop(Container:containerid) {
    return GetContainerAccess(containerid) == CONTAINER_ACCESS_SHOP;
}

/**
 * # Calls
 */

// A container id is handed out again once the one before it is gone, so the
// rule the last one carried is cleared rather than inherited. A bin becoming a
// crate because it reused an id is the kind of thing that is only ever found
// by somebody losing something in it.
hook OnContainerCreate(Container:containerid) {
    gContainerAccess[containerid] = CONTAINER_ACCESS_FREE;

    return 0;
}
