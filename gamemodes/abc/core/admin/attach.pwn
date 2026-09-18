#if defined _CORE_ADMIN_ATTACH
    #endinput
#endif
#define _CORE_ADMIN_ATTACH

/**
 * # Finding attach offsets
 *
 * The six numbers that put an object in a character's hands cannot be reasoned
 * about: an object's pivot is wherever the model was authored, never the grip
 * of a hand. EditItemAttach hands the client's own editor to the player, and
 * what comes back is printed in the shape it has to be pasted into the
 * catalogue -- the values live only as long as the process, and the point is to
 * get them into source.
 *
 * Most of the catalogue never needed this, because ScavengeSurvive had already
 * found the numbers. Anything invented here does.
 */

CMD:attach(playerid, params[]) {
    if (!EditItemAttach(playerid)) {
        SendClientMessage(playerid, -1, "Pick something up first.");

        return 1;
    }

    SendClientMessage(playerid, -1, "Drag it into place, then save.");

    return 1;
}

public OnPlayerEditItemAttach(playerid, ItemBuild:buildid, Float:offsetX, Float:offsetY, Float:offsetZ, Float:rotationX, Float:rotationY, Float:rotationZ, bone, Float:scaleX, Float:scaleY, Float:scaleZ) {
    new
        name[MAX_ITEM_BUILD_NAME]
    ;

    GetItemBuildName(buildid, name);

    printf("// %s", name);
    printf("SetItemBuildAttach(buildid, %f, %f, %f, %f, %f, %f, %i, %f, %f, %f);", offsetX, offsetY, offsetZ, rotationX, rotationY, rotationZ, bone, scaleX, scaleY, scaleZ);

    SendClientMessage(playerid, -1, "Saved. The line is in the server log.");

    return 1;
}
