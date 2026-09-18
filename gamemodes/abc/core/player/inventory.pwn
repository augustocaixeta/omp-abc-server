#if defined _CORE_PLAYER_INVENTORY
    #endinput
#endif
#define _CORE_PLAYER_INVENTORY

/**
 * # Moving items
 *
 * The four things that can happen to an item a character has in front of them,
 * written once here rather than in each of the action files that ask for them.
 * Every one of them is a change of custody, and every one can fail -- the
 * callers report, these only do.
 *
 * They take the container the item is in rather than assuming the character's
 * own pockets. A locker is a container and a bag is a container, and an item
 * taken out of one of those into a hand is the same move as one taken out of a
 * pocket -- written once, it works wherever a menu can be opened.
 */

/**
 * # Functions
 */

/**
 * @brief      Where in a character's pockets an item is.
 *
 * @param      playerid  Character whose pockets to look in.
 * @param      itemid    Item to find.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the slot index, counting from zero
 *             - `-1` when:
 *                 + the item is in nobody's pockets
 *                 + the item is in somebody else's
 */
forward GetPlayerInventoryItemIndex(playerid, Item:itemid);

/**
 * @brief      Take an item out of a container and into the hands.
 *
 * The last place the rules about what goes where can be broken, so they are
 * asked here as one question about both halves: a bag may go in a bin and
 * not in a pocket, and nothing at all may go inside itself.
 *
 * Asked before anything is moved. Taking the new one out first and finding
 * out afterwards that the old one has nowhere to go leaves the character
 * holding it with the slot already emptied, and the swap half done.
 *
 * What was in the hands goes back into the slot the new one just left,
 * which is free by definition -- so taking a rifle out of a locker leaves
 * the bag in the locker and not on the character.
 *
 * Whatever was already in the hands goes back into the slot the new one
 * came out of, so a swap does not shuffle the rest of the inventory.
 *
 * @param      playerid     Character to arm.
 * @param      containerid  Container it is in.
 * @param      itemid       Item to take out.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is now in their hands
 *             - `false` when:
 *                 + the item is not in that container
 *                 + the container refused to give it up
 */
forward bool:HoldContainerItem(playerid, Container:containerid, Item:itemid);

/**
 * @brief      Take an item out of a container and leave it on the ground.
 *
 * It lands where one dropped from the hand lands, using the framework's own
 * numbers rather than a second set: PlayerDropItem puts an item 0.85 in
 * front and ITEM_FLOOR_OFFSET below. Guessing that offset again is what
 * left everything hovering -- the build's own rise off the floor is added
 * to it, so being six centimetres out here is six centimetres of daylight
 * under the model.
 *
 * It lands a metre in front of the character and at their feet, in their
 * own world and interior, so it is reachable from where they are standing
 * rather than inside them.
 *
 * @param      playerid     Character dropping it.
 * @param      containerid  Container it is in.
 * @param      itemid       Item to drop.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is now lying in the world
 *             - `false` when:
 *                 + the item is not in that container
 *                 + the container refused to give it up
 */
forward bool:DropContainerItem(playerid, Container:containerid, Item:itemid);

/**
 * @brief      Hand an item from an open container into another character's pockets.
 *
 * An item taken out of one inventory and refused by the other is put back
 * in the slot it came from, so a failed hand-over never destroys it.
 *
 * @param      playerid     Character giving it.
 * @param      targetid     Character receiving it.
 * @param      containerid  Container it is in.
 * @param      itemid       Item to hand over.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is now in the target's pockets
 *             - `false` when:
 *                 + the item is not in that container
 *                 + the item is too big to be stored at all
 *                 + the target is not connected
 *                 + the target has no room
 *                 + either side of the move was refused
 */
forward bool:GiveContainerItemToPlayer(playerid, targetid, Container:containerid, Item:itemid);

/**
 * @brief      Destroy an item a character is carrying.
 *
 * Taken out of the container before it is destroyed, rather than left for
 * OnItemDestroy to tidy up, so the container's removal names the character
 * who ate it. A locker that loses a slot to nobody is a locker nothing can
 * be logged on.
 *
 * The check is what makes this different from DestroyItem: a character
 * cannot eat what is not in front of them.
 *
 * @param      playerid     Character using it.
 * @param      containerid  Container it is in.
 * @param      itemid       Item to use up.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is gone
 *             - `false` the item is not in that container
 */
forward bool:ConsumeContainerItem(playerid, Container:containerid, Item:itemid);

/**
 * @brief      Who is standing closest, within arm's length.
 *
 * The candidates come from personal-space, which the streamer keeps, so
 * nothing here walks the player list. With one candidate the distance is
 * not even measured.
 *
 * @param      playerid  Character to look around.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - the closest character's id
 *             - `INVALID_PLAYER_ID` when nobody is within `PERSONAL_SPACE_RADIUS`
 */
forward GetPlayerNearestPlayer(playerid);

/**
 * # Internal
 */

static stock ContainerItemIndexInternal(Container:containerid, Item:itemid) {
    new
        Container:holder = INVALID_CONTAINER_ID,
        index = -1
    ;

    if (!GetItemContainer(itemid, holder, index)) {
        return -1;
    }

    if (holder != containerid) {
        return -1;
    }

    return index;
}

// A character's pockets are a container like any other, except that taking
// something out of them raises OnItemRemoveFromInventory and taking it out of a
// locker does not. Going through the container call for both would lose that.
static stock bool:TakeContainerItemInternal(playerid, Container:containerid, index) {
    if (containerid == GetPlayerContainer(playerid)) {
        return RemoveItemFromInventory(playerid, index);
    }

    return RemoveItemFromContainer(containerid, index, .playerid = playerid);
}

static stock bool:PutContainerItemInternal(playerid, Container:containerid, Item:itemid, index = -1) {
    if (!CanPutItemInContainer(itemid, containerid)) {
        return false;
    }

    if (containerid == GetPlayerContainer(playerid)) {
        return AddItemToInventory(playerid, itemid, .addToIndex = index);
    }

    return AddItemToContainer(containerid, itemid, .addToIndex = index, .playerid = playerid);
}

/**
 * # External
 */

stock GetPlayerInventoryItemIndex(playerid, Item:itemid) {
    new
        holder = INVALID_PLAYER_ID,
        index = -1
    ;

    if (!IsItemInInventory(itemid, holder, index)) {
        return -1;
    }

    if (holder != playerid) {
        return -1;
    }

    return index;
}

stock bool:HoldContainerItem(playerid, Container:containerid, Item:itemid) {
    new const
        index = ContainerItemIndexInternal(containerid, itemid)
    ;

    if (index == -1) {
        return false;
    }

    new const
        Item:held = GetPlayerHoldItem(playerid)
    ;

    if (IsValidItem(held) && !CanPutItemInContainer(held, containerid)) {
        return false;
    }

    if (!TakeContainerItemInternal(playerid, containerid, index)) {
        return false;
    }

    SetPlayerItem(playerid, itemid);

    if (held != INVALID_ITEM_ID) {
        PutContainerItemInternal(playerid, containerid, held, index);
    }

    return true;
}

stock bool:DropContainerItem(playerid, Container:containerid, Item:itemid) {
    new const
        index = ContainerItemIndexInternal(containerid, itemid)
    ;

    if (index == -1) {
        return false;
    }

    if (!TakeContainerItemInternal(playerid, containerid, index)) {
        return false;
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:angle
    ;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angle);

    CreateItemInWorld(itemid,
        x + (floatsin(-angle, degrees) * 0.85),
        y + (floatcos(-angle, degrees) * 0.85),
        z - ITEM_FLOOR_OFFSET,
        0.0,
        0.0,
        angle,
        .worldid = GetPlayerVirtualWorld(playerid),
        .interiorid = GetPlayerInterior(playerid)
    );

    return true;
}

stock bool:GiveContainerItemToPlayer(playerid, targetid, Container:containerid, Item:itemid) {
    new const
        index = ContainerItemIndexInternal(containerid, itemid)
    ;

    if (index == -1) {
        return false;
    }

    if (!CanStoreItem(itemid)) {
        return false;
    }

    if (!IsPlayerConnected(targetid)) {
        return false;
    }

    if (IsPlayerInventoryFull(targetid)) {
        return false;
    }

    if (!TakeContainerItemInternal(playerid, containerid, index)) {
        return false;
    }

    if (!AddItemToInventory(targetid, itemid)) {
        PutContainerItemInternal(playerid, containerid, itemid, index);

        return false;
    }

    return true;
}

stock bool:ConsumeContainerItem(playerid, Container:containerid, Item:itemid) {
    new const
        index = ContainerItemIndexInternal(containerid, itemid)
    ;

    if (index == -1) {
        return false;
    }

    if (!TakeContainerItemInternal(playerid, containerid, index)) {
        return false;
    }

    DestroyItem(itemid);

    return true;
}

stock GetPlayerNearestPlayer(playerid) {
    new
        players[MAX_PLAYERS],
        Float:x,
        Float:y,
        Float:z,
        nearest = INVALID_PLAYER_ID,
        Float:nearestDistance = PERSONAL_SPACE_RADIUS
    ;

    new const
        count = GetPlayersNextToPlayer(playerid, players)
    ;

    if (count == 0) {
        return INVALID_PLAYER_ID;
    }

    if (count == 1) {
        return players[0];
    }

    GetPlayerPos(playerid, x, y, z);

    for (new i; i != count; ++i) {
        new const
            Float:distance = GetPlayerDistanceFromPoint(players[i], x, y, z)
        ;

        if (distance > nearestDistance) {
            continue;
        }

        nearest = players[i];
        nearestDistance = distance;
    }

    return nearest;
}
