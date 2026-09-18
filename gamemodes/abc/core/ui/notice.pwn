#if defined _CORE_UI_NOTICE
    #endinput
#endif
#define _CORE_UI_NOTICE

#include <pp-hooks>

/**
 * # Telling a character what just happened
 *
 * A sentence across the middle of the screen for a moment: the answer to
 * something they just tried. It belongs where they are already looking rather
 * than in a chat log, which is the wrong place to explain a key -- by the time
 * anybody reads it, three other lines have pushed it up.
 *
 * This is not a prompt. A prompt is what is true right now and is offered by
 * whichever part of the gamemode knows it; a notice is what just happened, is
 * sent by whoever made it happen, and goes on its own. They use two different
 * styles from extended-text-styles for exactly that reason -- one sits down the
 * side and one crosses the middle, so neither is mistaken for the other.
 */

/**
 * # Functions
 */

/**
 * @brief      Tell a character what just happened, across the middle of the screen.
 *
 * Not a prompt. A prompt is what is true right now and is offered by
 * whoever knows it; a notice is what just happened and is sent by whoever
 * made it happen. They use two styles from extended-text-styles so that
 * neither is mistaken for the other.
 *
 * Nothing following the message means nothing inside it is a placeholder
 * either, so a per cent sign in a sentence stays a per cent sign.
 *
 * @param      playerid  Character to tell.
 * @param      message   The sentence, format placeholders and all.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 */
forward SendPlayerNotice(playerid, const message[], OPEN_MP_TAGS:...);

/**
 * # External
 */

stock SendPlayerNotice(playerid, const message[], OPEN_MP_TAGS:...) {
    if (!IsPlayerConnected(playerid)) {
        return;
    }

    new
        text[MAX_PLAYER_PROMPT_LENGTH]
    ;

    Format(text, sizeof (text), message, ___(2));

    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_STUNT, PLAYER_NOTICE_TIME, text);
}
