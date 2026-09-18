#if defined _CORE_UI_PROMPT
    #endinput
#endif
#define _CORE_UI_PROMPT

#include <pp-hooks>

/**
 * # Telling a character what they can do
 *
 * A popup down the side of the screen, from extended-text-styles, saying what
 * the key under the character's finger would do right now. Chat is the wrong
 * place for it: a line about a crate scrolls away while the character is still
 * standing next to the crate.
 *
 * Nothing is stored per source. Every event that could change the answer calls
 * UpdatePlayerPrompt, which asks all of them again through OnPlayerRequestPrompt
 * and shows the highest offer -- the same shape ScavengeSurvive uses for its
 * key action UI, and for the same reason: a character can be standing in two
 * places at once, and remembering which one they entered first is a bookkeeping
 * problem that recomputing does not have.
 *
 * That is what settles the priority question. Walking into a second area does
 * not replace the first: both offer, and the higher number wins for as long as
 * both are true. Walking out of it puts the other one back without either of
 * them having to know the other exists.
 *
 * A prompt is shown for a few seconds and then goes, rather than sitting there
 * for as long as it is true. What is remembered is the sentence, not whether it
 * is still on screen -- so walking around inside one area says it once, and
 * anything that changes the answer says the new one.
 */

#define PLAYER_PROMPT_NONE      (0)

#define PLAYER_PROMPT_HELD      (10)

#define PLAYER_PROMPT_REACH     (20)

// A door being offered, or the question that follows reaching for it. Above an
// item on the floor: one is a thing to pick up, the other is being asked.
#define PLAYER_PROMPT_PROPERTY  (25)

#define PLAYER_PROMPT_BUSY      (30)

static
    gPromptPriority[MAX_PLAYERS],
    gPromptText[MAX_PLAYERS][MAX_PLAYER_PROMPT_LENGTH],
    gPromptShown[MAX_PLAYERS][MAX_PLAYER_PROMPT_LENGTH]
;

/**
 * # Functions
 */

/**
 * @brief      Ask everything again and show whatever wins.
 *
 * Called by every event that could change the answer. Nothing is stored
 * per source: the offers are collected fresh through OnPlayerRequestPrompt
 * and the highest one is shown, which is why walking out of an area puts
 * the previous offer back without either area knowing the other exists.
 *
 * A prompt that comes out the same as the one already up is not sent
 * again, so walking around inside one area says it once.
 *
 * @param      playerid  Character to recompute for.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 */
forward UpdatePlayerPrompt(playerid);

/**
 * @brief      Offer a sentence, to be shown if nothing better is offered.
 *
 * Only ever called from inside OnPlayerRequestPrompt. The priority is what
 * settles two things being true at once -- standing at a crate while
 * holding something -- without either offer having to check the other.
 *
 * @param      playerid  Character being asked about.
 * @param      priority  PLAYER_PROMPT_* band; higher wins.
 * @param      message   The sentence, format placeholders and all.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` this offer is the best so far and was kept
 *             - `false` when:
 *                 + the character is not connected
 *                 + something has already offered at this priority or above
 */
forward bool:OfferPlayerPrompt(playerid, priority, const message[], OPEN_MP_TAGS:...);

/**
 * # Events
 */

/**
 * @brief      Called when the prompt is being recomputed: offer yours now.
 *
 * Every module that has something to say about the key under a
 * character's finger answers this with OfferPlayerPrompt. Answering it
 * with anything else has no effect, and the return value is ignored.
 *
 * @param      playerid  Character being asked about.
 *
 * @date       20:15 07/09/2026
 * @author     augustocaixeta
 */
forward OnPlayerRequestPrompt(playerid);

/**
 * # External
 */

stock bool:OfferPlayerPrompt(playerid, priority, const message[], OPEN_MP_TAGS:...) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (priority <= gPromptPriority[playerid]) {
        return false;
    }

    Format(gPromptText[playerid], MAX_PLAYER_PROMPT_LENGTH, message, ___(3));

    gPromptPriority[playerid] = priority;

    return true;
}

stock UpdatePlayerPrompt(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return;
    }

    gPromptPriority[playerid] = PLAYER_PROMPT_NONE;
    gPromptText[playerid][0] = EOS;

    CallLocalFunction("OnPlayerRequestPrompt", "i", playerid);

    if (!strcmp(gPromptText[playerid], gPromptShown[playerid])) {
        return;
    }

    strcopy(gPromptShown[playerid], gPromptText[playerid]);

    if (gPromptShown[playerid][0] == EOS) {
        PlayerGameTextHide(playerid, GAME_TEXT_STYLE_POPUP);

        return;
    }

    PlayerGameTextShow(playerid, GAME_TEXT_STYLE_POPUP, PLAYER_PROMPT_TIME, gPromptShown[playerid]);
}

/**
 * # Calls
 */

hook OnPlayerConnect(playerid) {
    gPromptPriority[playerid] = PLAYER_PROMPT_NONE;
    gPromptText[playerid][0] = EOS;
    gPromptShown[playerid][0] = EOS;

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    gPromptPriority[playerid] = PLAYER_PROMPT_NONE;
    gPromptText[playerid][0] = EOS;
    gPromptShown[playerid][0] = EOS;

    return 0;
}

hook OnPlayerSpawn(playerid) {
    UpdatePlayerPrompt(playerid);

    return 0;
}
