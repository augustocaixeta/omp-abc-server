#if defined _CORE_UTIL_VEHICLE
    #endinput
#endif
#define _CORE_UTIL_VEHICLE

/**
 * # Where things are on a vehicle
 *
 * Answers about a model and an angle. Nothing here owns anything or hooks
 * anything: whatever wants a boot, a bonnet or a point beside a door asks.
 */

/**
 * # Functions
 */

/**
 * @brief      How far behind the middle of a vehicle its boot is.
 *
 * Half the length of its model and a little clear of the bumper, along the
 * vehicle's own Y. Negative, because that axis runs forwards.
 *
 * Goes straight into GetVehicleOffsetPos as an `offsetY`, which is what
 * GetVehicleBootPos does with it.
 *
 * @param      vehicleid  Vehicle to measure.
 * @param      margin     How far clear of the bumper to sit.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how far back the boot is
 *             - `-margin` when `vehicleid` is not a live vehicle, there being
 *               no model to measure
 */
forward Float:GetVehicleBootOffset(vehicleid, Float:margin = 0.1);

/**
 * @brief      How far in front of the middle of a vehicle its bonnet is.
 *
 * The same measurement as the boot and the other way round.
 *
 * @param      vehicleid  Vehicle to measure.
 * @param      margin     How far clear of the bumper to sit.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - how far forward the bonnet is
 *             - `margin` when `vehicleid` is not a live vehicle
 */
forward Float:GetVehicleBonnetOffset(vehicleid, Float:margin = 0.1);

/**
 * @brief      A point in a vehicle's own space, answered in the world's.
 *
 * X is to its right, Y is forwards and Z is up, all three turned by however
 * the vehicle happens to be facing. What every other position here is
 * written in terms of.
 *
 * @param      vehicleid  Vehicle to measure from.
 * @param      offsetX    How far to its right.
 * @param      offsetY    How far in front of it.
 * @param      offsetZ    How far above it.
 * @param      x          Written with the world position.
 * @param      y          Written with the world position.
 * @param      z          Written with the world position.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the three were written
 *             - `false` `vehicleid` is not a live vehicle
 */
forward bool:GetVehicleOffsetPos(vehicleid, Float:offsetX, Float:offsetY, Float:offsetZ, &Float:x, &Float:y, &Float:z);

/**
 * @brief      Where a vehicle's boot is, in the world.
 *
 * Level with the middle of it rather than with the floor: whatever stands
 * there decides its own height.
 *
 * @param      vehicleid  Vehicle to measure.
 * @param      x          Written with the world position.
 * @param      y          Written with the world position.
 * @param      z          Written with the world position.
 * @param      margin     How far clear of the bumper to sit.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the three were written
 *             - `false` `vehicleid` is not a live vehicle
 */
forward bool:GetVehicleBootPos(vehicleid, &Float:x, &Float:y, &Float:z, Float:margin = 0.1);

/**
 * @brief      Where a vehicle's bonnet is, in the world.
 *
 * @param      vehicleid  Vehicle to measure.
 * @param      x          Written with the world position.
 * @param      y          Written with the world position.
 * @param      z          Written with the world position.
 * @param      margin     How far clear of the bumper to sit.
 *
 * @date       14:30 17/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the three were written
 *             - `false` `vehicleid` is not a live vehicle
 */
forward bool:GetVehicleBonnetPos(vehicleid, &Float:x, &Float:y, &Float:z, Float:margin = 0.1);

/**
 * # External
 */

stock Float:GetVehicleBootOffset(vehicleid, Float:margin = 0.1) {
    return -GetVehicleBonnetOffset(vehicleid, margin);
}

stock Float:GetVehicleBonnetOffset(vehicleid, Float:margin = 0.1) {
    new
        Float:sizeX,
        Float:sizeY,
        Float:sizeZ
    ;

    GetVehicleModelInfo(GetVehicleModel(vehicleid), VEHICLE_MODEL_INFO_SIZE, sizeX, sizeY, sizeZ);

    return sizeY / 2.0 + margin;
}

stock bool:GetVehicleOffsetPos(vehicleid, Float:offsetX, Float:offsetY, Float:offsetZ, &Float:x, &Float:y, &Float:z) {
    if (!GetVehiclePos(vehicleid, x, y, z)) {
        return false;
    }

    new
        Float:angle
    ;

    GetVehicleZAngle(vehicleid, angle);

    new const
        Float:sin = floatsin(-angle, degrees),
        Float:cos = floatcos(-angle, degrees)
    ;

    x += offsetX * cos + offsetY * sin;
    y += offsetY * cos - offsetX * sin;
    z += offsetZ;

    return true;
}

stock bool:GetVehicleBootPos(vehicleid, &Float:x, &Float:y, &Float:z, Float:margin = 0.1) {
    return GetVehicleOffsetPos(vehicleid, 0.0, GetVehicleBootOffset(vehicleid, margin), 0.0, x, y, z);
}

stock bool:GetVehicleBonnetPos(vehicleid, &Float:x, &Float:y, &Float:z, Float:margin = 0.1) {
    return GetVehicleOffsetPos(vehicleid, 0.0, GetVehicleBonnetOffset(vehicleid, margin), 0.0, x, y, z);
}
