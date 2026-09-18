#if defined _CORE_WORLD_LABEL
    #endinput
#endif
#define _CORE_WORLD_LABEL

#include <pp-hooks>

/**
 * # Names hanging over things
 *
 * A popup tells the character standing on top of something what it is. A label
 * tells everybody who can see it, from wherever they are standing, and the two
 * answer different questions: the popup is what you may do, the label is what
 * it is. Something dropped in a road is a grey shape at ten metres without one.
 *
 * The framework hangs them and streams them; what they say is the gamemode's,
 * which is the whole of this file.
 *
 * Items and lockers are every button in the gamemode -- each owns one and
 * nothing else creates any -- so labelling the two of them labels everything a
 * character can walk up to.
 *
 * An item carries its amount under its name when it has one, because the number
 * is most of what the character wants to know before deciding to bend down. It
 * is written when the item reaches the world and not kept in step afterwards:
 * an amount that changes while the thing is lying in a road is a thing somebody
 * is standing over, and they have the popup.
 *
 * The amount is written for a textdraw, where `~g~` is an instruction to change
 * colour. A label is not a textdraw and reads those three characters as three
 * characters, so they are taken out before it is hung. The alternative is a
 * second format for every amount in the gamemode, which is the same sentence
 * written down twice and only one of them ever maintained.
 */

/**
 * # Internal
 */

static stock StripTextDrawCodesInternal(text[]) {
    new
        write
    ;

    for (new read; text[read] != EOS; ++read) {
        if (text[read] == '~') {
            if (text[read + 1] == '~') {
                text[write++] = '~';
                ++read;

                continue;
            }

            if (text[read + 1] != EOS && text[read + 2] == '~') {
                read += 2;

                continue;
            }
        }

        text[write++] = text[read];
    }

    text[write] = EOS;
}

/**
 * # Calls
 */

hook OnItemCreateInWorld(Item:itemid) {
    new
        name[MAX_ITEM_NAME],
        amount[MAX_ITEM_AMOUNT_LENGTH]
    ;

    GetItemName(itemid, name);

    if (!FormatItemAmount(itemid, amount)) {
        SetItemLabelText(
            itemid, name, WORLD_LABEL_COLOUR,
            ITEM_DEFAULT_DRAW_DISTANCE, ITEM_DEFAULT_STREAM_DISTANCE,
            true, WORLD_LABEL_ITEM_OFFSET_Z
        );

        return 0;
    }

    StripTextDrawCodesInternal(amount);

    SetItemLabelText(
        itemid, "%s\n%s", WORLD_LABEL_COLOUR,
        ITEM_DEFAULT_DRAW_DISTANCE, ITEM_DEFAULT_STREAM_DISTANCE,
        true, WORLD_LABEL_ITEM_OFFSET_Z,
        name, amount
    );

    return 0;
}

hook OnLockerCreate(Locker:lockerid) {
    new
        name[MAX_ITEM_NAME]
    ;

    GetLockerName(lockerid, name);

    SetLockerLabelText(
        lockerid, name, WORLD_LABEL_COLOUR,
        BUTTON_DEFAULT_DRAW_DISTANCE, BUTTON_DEFAULT_STREAM_DISTANCE,
        true, WORLD_LABEL_LOCKER_OFFSET_Z
    );

    return 0;
}
