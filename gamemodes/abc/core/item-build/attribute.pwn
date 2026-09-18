#if defined _CORE_ITEM_BUILD_ATTRIBUTE
    #endinput
#endif
#define _CORE_ITEM_BUILD_ATTRIBUTE

/**
 * # Item attributes
 *
 * What a kind of item is, as bits. The framework knows nothing about these --
 * it stores the set per build and answers HasItemAttribute -- so this file is
 * where their meaning lives, and the action files are where it is acted on.
 *
 * Shifting rather than counting: a plain enum would make the third name 3,
 * which is the first two bits together.
 *
 * The first three are the point of the whole set. They are not three flavours
 * of "the character does something with it", they are three different pieces
 * of the engine:
 *
 *   USABLE      consumed and gone. Food, drink, a bandage, a phone call. The
 *               item stops existing, or its amount goes down.
 *   EQUIPPABLE  a weapon, which means GivePlayerWeapon. An armed weapon is a
 *               weapon slot, not an object hanging off a bone -- attaching the
 *               model would hand the character a prop they cannot fire.
 *   HOLDABLE    carried in the hands as an object, which is
 *               SetPlayerAttachedObject. A crate, a bag, a body. This is the
 *               only one of the three that attaches anything.
 *
 * An item can carry more than one. A backpack is HOLDABLE and OPENABLE; a
 * bottle of pills is USABLE and GIVEABLE.
 */

enum (<<= 1) {
    ITEM_ATTRIBUTE_USABLE = 1,
    ITEM_ATTRIBUTE_EQUIPPABLE,
    ITEM_ATTRIBUTE_HOLDABLE,
    ITEM_ATTRIBUTE_OPENABLE,
    ITEM_ATTRIBUTE_DROPPABLE,

    // Thrown away rather than put down, which is a different act and not a
    // tidier way of doing the same one: what is dropped is still there and what
    // is discarded is gone. Rubbish carries it -- an emptied can, a spent
    // wrapper -- and a rifle never does.
    ITEM_ATTRIBUTE_DISCARDABLE,

    ITEM_ATTRIBUTE_GIVEABLE,
    ITEM_ATTRIBUTE_SPLITTABLE,

    // Rounds that go into a weapon. On the box rather than on the gun, because
    // loading is something done to ammunition: the row appears where the
    // character is looking when they think "put this in my rifle".
    ITEM_ATTRIBUTE_LOADABLE,

    // A weapon that can be emptied back into the pockets. The other half of
    // LOADABLE and on the other item, for the same reason: emptying is
    // something done to the gun.
    ITEM_ATTRIBUTE_UNLOADABLE,

    // May turn up without anybody choosing it: out of a present, in a crate
    // somebody found, from whatever fills the world later. It is the one
    // attribute about a kind of item rather than about what a character may do
    // with it, and it is opt in on purpose -- a build nobody thought about does
    // not appear in a shipment.
    ITEM_ATTRIBUTE_LOOTABLE
};

static const
    ITEM_ATTRIBUTE_NAME[][] = {
        "usable",
        "equippable",
        "holdable",
        "openable",
        "droppable",
        "discardable",
        "giveable",
        "splittable",
        "loadable",
        "unloadable",
        "lootable"
    }
;

/**
 * @brief      A random kind of item carrying every one of these attributes.
 *
 * One pass and no list. Each match replaces the one held so far with odds of
 * one in however many have been seen, which leaves every match equally likely
 * by the end -- so a catalogue of any size costs one walk and one cell.
 *
 * @param      attributes  Flags a build must all carry to be in the draw.
 *
 * @date       14:00 08/09/2026
 * @author     augustocaixeta
 *
 * @return     - a build carrying them
 *             - `INVALID_ITEM_BUILD_ID` when nothing does
 */
forward ItemBuild:RandomItemBuild(attributes = ITEM_ATTRIBUTE_LOOTABLE);

/**
 * # External
 */

stock ItemBuild:RandomItemBuild(attributes = ITEM_ATTRIBUTE_LOOTABLE) {
    new
        ItemBuild:chosen = INVALID_ITEM_BUILD_ID,
        seen
    ;

    for (new ItemBuild:buildid; buildid != ItemBuild:CountItemBuild(); ++buildid) {
        if (!HasItemBuildAllAttributes(buildid, attributes)) {
            continue;
        }

        if (random(++seen) == 0) {
            chosen = buildid;
        }
    }

    return chosen;
}

stock FormatItemAttributes(Item:itemid, output[], size = sizeof (output)) {
    new const
        attributes = GetItemAttributes(itemid)
    ;

    output[0] = EOS;

    for (new i, bit = 1; i != sizeof (ITEM_ATTRIBUTE_NAME); ++i, bit <<= 1) {
        if (!(attributes & bit)) {
            continue;
        }

        if (output[0] != EOS) {
            strcat(output, ", ", size);
        }

        strcat(output, ITEM_ATTRIBUTE_NAME[i], size);
    }

    return strlen(output);
}
