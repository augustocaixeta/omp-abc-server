#if defined _CORE_ADMIN_HELP
    #endinput
#endif
#define _CORE_ADMIN_HELP

#include <pp-hooks>

/**
 * # Development commands
 *
 * Not gameplay. This file is only the list of them: each
 * of the others owns a subject, so a new tool is a new file and a line here.
 *
 * The dev prefix on the commands is what tells anybody using the gamemode that
 * these are not part of it. Everything under this folder is expected to come
 * out, and taking it out is deleting the folder and its lines from main.
 */

CMD:dev(playerid, params[]) {
    SendClientMessage(playerid, -1, "/devadd [count] /devitem <name> /devfill /devlocker /devjson /devstats /devkeys /devpreview /attach");

    return 1;
}
