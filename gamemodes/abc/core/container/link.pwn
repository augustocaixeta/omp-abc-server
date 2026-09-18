#if defined _CORE_CONTAINER_LINK
    #endinput
#endif
#define _CORE_CONTAINER_LINK

#include <pp-hooks>

/**
 * # A container on something that moves
 *
 * A locker is a container on an object that stays put; this is the same idea
 * where the thing can drive off. A boot is the only one so far.
 *
 * The button's area is attached to the vehicle, so the streamer carries it and
 * nothing here follows anything.
 */

#if !defined CONTAINER_LINK_BUTTON_SIZE
    #define CONTAINER_LINK_BUTTON_SIZE (1.2)
#endif

// Stored one above the vehicle id: a missing key reads as zero, and zero is a
// vehicle.
#define CONTAINER_LINK_KEY "boot"

static
    Container:gVehicleContainer[MAX_VEHICLES] = { INVALID_CONTAINER_ID, ... },
    Button:gVehicleButton[MAX_VEHICLES] = { INVALID_BUTTON_ID, ... }
;

static stock ButtonVehicleInternal(Button:buttonid) {
    new const
        stored = GetButtonExtraData(buttonid, CONTAINER_LINK_KEY)
    ;

    return stored ? (stored - 1) : INVALID_VEHICLE_ID;
}

/**
 * # Functions
 */

/**
 * @brief      The container a vehicle carries.
 *
 * @param      vehicleid  Vehicle to ask.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - its boot
 *             - `INVALID_CONTAINER_ID` when:
 *                 + `vehicleid` is out of range
 *                 + nothing was linked to it
 */
forward Container:GetVehicleContainer(vehicleid);

/**
 * @brief      Hang a container off the back of a vehicle.
 *
 * A button appears at the boot and travels with it, opening the container
 * for anybody standing there. One container to a vehicle.
 *
 * @param      vehicleid    Vehicle to hang it on.
 * @param      containerid  Container it opens.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the boot is there and opens
 *             - `false` when:
 *                 + `vehicleid` is out of range or is not a live vehicle
 *                 + `containerid` is not a live container
 *                 + that vehicle already carries one
 *                 + the button could not be created
 */
forward bool:LinkContainerToVehicle(vehicleid, Container:containerid);

/**
 * @brief      Take the boot off a vehicle.
 *
 * The button goes. The container does not: whoever made it decides whether
 * it and its contents are destroyed.
 *
 * @param      vehicleid  Vehicle to unlink.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the button is gone
 *             - `false` when:
 *                 + `vehicleid` is out of range
 *                 + it carried nothing
 */
forward bool:UnlinkContainerFromVehicle(vehicleid);

/**
 * @brief      The boot a character is standing at.
 *
 * Whichever button is nearest them, if it happens to be one of these. What
 * the open key and the prompt both read.
 *
 * @param      playerid  Character to ask about.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - the container they could open
 *             - `INVALID_CONTAINER_ID` when:
 *                 + they are at no button
 *                 + the button they are at is not a boot
 */
forward Container:GetPlayerLinkedContainer(playerid);

stock Container:GetVehicleContainer(vehicleid) {
    if (!(0 <= vehicleid < MAX_VEHICLES)) {
        return INVALID_CONTAINER_ID;
    }

    return gVehicleContainer[vehicleid];
}

stock bool:LinkContainerToVehicle(vehicleid, Container:containerid) {
    if (!(0 <= vehicleid < MAX_VEHICLES)) {
        return false;
    }

    if (!IsValidContainer(containerid)) {
        return false;
    }

    if (gVehicleContainer[vehicleid] != INVALID_CONTAINER_ID) {
        return false;
    }

    new
        Float:x,
        Float:y,
        Float:z
    ;

    if (!GetVehicleBootPos(vehicleid, x, y, z)) {
        return false;
    }

    new const
        Button:buttonid = CreateButton(x, y, z, ITEM_KEY_OPEN_ITEM, CONTAINER_LINK_BUTTON_SIZE, GetVehicleVirtualWorld(vehicleid), -1)
    ;

    if (buttonid == INVALID_BUTTON_ID) {
        return false;
    }

    AttachDynamicAreaToVehicle(GetButtonArea(buttonid), vehicleid, 0.0, GetVehicleBootOffset(vehicleid), 0.0);
    SetButtonExtraData(buttonid, CONTAINER_LINK_KEY, vehicleid + 1);

    gVehicleContainer[vehicleid] = containerid;
    gVehicleButton[vehicleid] = buttonid;

    return true;
}

stock bool:UnlinkContainerFromVehicle(vehicleid) {
    if (!(0 <= vehicleid < MAX_VEHICLES)) {
        return false;
    }

    if (gVehicleContainer[vehicleid] == INVALID_CONTAINER_ID) {
        return false;
    }

    DestroyButton(gVehicleButton[vehicleid]);

    gVehicleContainer[vehicleid] = INVALID_CONTAINER_ID;
    gVehicleButton[vehicleid] = INVALID_BUTTON_ID;

    return true;
}

stock Container:GetPlayerLinkedContainer(playerid) {
    new const
        Button:buttonid = GetPlayerButton(playerid)
    ;

    if (buttonid == INVALID_BUTTON_ID) {
        return INVALID_CONTAINER_ID;
    }

    new const
        vehicleid = ButtonVehicleInternal(buttonid)
    ;

    if (vehicleid == INVALID_VEHICLE_ID) {
        return INVALID_CONTAINER_ID;
    }

    return gVehicleContainer[vehicleid];
}

/**
 * # Calls
 */

// The area went with the vehicle; the stored position did not, and it is about
// to be read to decide which of two buttons the character means.
hook OnPlayerEnterButtonArea(playerid, Button:buttonid) {
    new const
        vehicleid = ButtonVehicleInternal(buttonid)
    ;

    if (vehicleid == INVALID_VEHICLE_ID) {
        return 0;
    }

    new
        Float:x,
        Float:y,
        Float:z
    ;

    if (GetVehicleBootPos(vehicleid, x, y, z)) {
        SetButtonPos(buttonid, x, y, z);
    }

    return 0;
}

hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if (!(newkeys & ITEM_KEY_OPEN_ITEM)) {
        return 0;
    }

    if (IsPlayerInAnyVehicle(playerid)) {
        return 0;
    }

    new const
        Container:containerid = GetPlayerLinkedContainer(playerid)
    ;

    if (containerid == INVALID_CONTAINER_ID) {
        return 0;
    }

    ShowPlayerContainer(playerid, containerid);

    return 1;
}

hook OnPlayerRequestPrompt(playerid) {
    if (GetPlayerLinkedContainer(playerid) != INVALID_CONTAINER_ID) {
        OfferPlayerPrompt(playerid, PLAYER_PROMPT_REACH, "Press %s to open the boot.", ITEM_KEY_OPEN_NAME);
    }

    return 0;
}
