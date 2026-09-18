#if defined _INC_ITEM_PLUS
    #endinput
#endif
#define _INC_ITEM_PLUS

#include <open.mp>

#include <I\item>
#include <I\PP\json-plus>

#include <pp-hooks>

/**
 * # Header
 */

// Everything an item carries that the framework itself knows nothing about,
// and the one way that reaches a database. A PawnPlus map per item, made the
// first time somebody writes to that item, so a world full of ordinary items
// costs one cell each.
//
// The namespace has two halves. Keys starting with ITEM_RESERVED_KEY_PREFIX
// belong to the framework: a module keeps its own field in an array, because a
// duration read on every menu redraw would be felt as a map lookup, and only
// writes it into the document at save time. SetItemExtraData refuses that half,
// so the developer's keys and the framework's can never collide.
//
// A build is written down by the name it was registered under and never by its
// id. Ids are handed out in registration order, so adding one kind of item
// above another in a catalogue would silently turn every saved item of one
// into the other.

#if !defined MAX_ITEM_KEY_LENGTH
    #define MAX_ITEM_KEY_LENGTH (32)
#endif

#if !defined ITEM_RESERVED_KEY_PREFIX
    #define ITEM_RESERVED_KEY_PREFIX '@'
#endif

#if !defined MAX_ITEM_ATTRIBUTES
    #define MAX_ITEM_ATTRIBUTES (16)
#endif

static
    Map:gItemBuildMap,
    Map:gItemExtraDataMap[MAX_ITEMS]
;

// The same pairing the map holds, the other way round. A save path has a build
// in its hand and needs the name to write down; a load path has the name and
// needs the build. The map answers the second and would have to be walked for
// the first, so the answer is kept rather than searched for -- one string per
// build, written once at registration and read once per item saved.
static
    gItemBuildKey[MAX_ITEM_BUILDS][MAX_ITEM_KEY_LENGTH]
;

/**
 * # Functions
 */

/**
 * @brief      Register a build under a name a document can carry.
 *
 * The name is what a save writes down instead of the build id, because ids
 * are handed out in registration order: inserting a kind of item above
 * another in a catalogue would turn every saved item of one into the other.
 *
 * Calling it twice for the same build replaces the name, and the old one
 * stops naming anything.
 *
 * @param      key      Name to register it under.
 * @param      buildid  Build the name means.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the name is registered
 *             - `false` when:
 *                 + `key` is empty, or too long for MAX_ITEM_KEY_LENGTH
 *                 + `buildid` is not a registered build
 */
forward bool:SetKeyItemBuild(const key[], ItemBuild:buildid);

/**
 * @brief      Which build a name means.
 *
 * What a load path calls to turn a written-down name back into a build.
 *
 * @param      key  Name to look up.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the build registered under it
 *             - `INVALID_ITEM_BUILD_ID` when:
 *                 + nothing has ever been registered
 *                 + no build carries that name
 */
forward ItemBuild:GetKeyItemBuild(const key[]);

/**
 * @brief      The name a build was registered under.
 *
 * The inverse lookup, for a save path that has the build in its hand. Kept
 * rather than searched for, so it costs a copy rather than a walk.
 *
 * @param      buildid  Build to look up.
 * @param      output   Reads back the name.
 * @param      size     How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + it was never given a name, and the output is cleared
 */
forward bool:GetItemBuildKey(ItemBuild:buildid, output[], size = sizeof (output));

/**
 * @brief      Whether a name means any build at all.
 *
 * What a load path asks before trusting a row: a name that no longer
 * matches anything is a kind of item the catalogue has dropped.
 *
 * @param      key  Name to look for.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` some build carries it
 *             - `false` when:
 *                 + nothing has ever been registered
 *                 + no build carries that name
 */
forward bool:HasKeyAnyItemBuild(const key[]);

/**
 * @brief      Whether a name means one particular build.
 *
 * @param      key      Name to check.
 * @param      buildid  Build it should mean.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` that name means that build
 *             - `false` when:
 *                 + `buildid` is not a registered build
 *                 + the name means something else, or nothing
 */
forward bool:HasKeyItemBuild(const key[], ItemBuild:buildid);

/**
 * @brief      Whether an item carries any keys of its own.
 *
 * Only the developer's half of the namespace counts, so an item with a
 * duration and nothing else answers false.
 *
 * @param      itemid  Item to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it carries at least one key
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + nothing has ever been written to it
 */
forward bool:HasItemAnyExtraData(Item:itemid);

/**
 * @brief      Whether an item carries one particular key.
 *
 * @param      itemid  Item to ask.
 * @param      key     Name to look for.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the key is there
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + it carries no such key
 */
forward bool:HasItemExtraData(Item:itemid, const key[]);

/**
 * @brief      Write one key onto an item.
 *
 * Takes a `Variant:`, so one call covers a cell, a float, a boolean, a
 * string and a `List:`. What comes out is read back with the matching
 * getter, and reading a key as the wrong type gives the cell reinterpreted
 * rather than an error.
 *
 * A `List:` written here belongs to the item's map from then on, and is
 * freed with it.
 *
 * @param      itemid  Item to write.
 * @param      key     Name to write it under.
 * @param      value   The value, of any of those types.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the key is written
 *             - `false` when:
 *                 + `key` is empty, or too long for MAX_ITEM_KEY_LENGTH
 *                 + `key` starts with ITEM_RESERVED_KEY_PREFIX, which is the
 *                   framework's half of the namespace
 *                 + `itemid` is not a live item
 *                 + the item is at MAX_ITEM_ATTRIBUTES and the key is new
 */
forward bool:SetItemExtraData(Item:itemid, const key[], Variant:value);

/**
 * @brief      Read a key back as a list.
 *
 * The list belongs to the item, so it is for reading through and adding to,
 * not for deleting: it is freed when the item is.
 *
 * @param      itemid  Item to read.
 * @param      key     Name to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the list stored under it
 *             - `INVALID_LIST` when:
 *                 + `itemid` is not a live item
 *                 + it carries no such key
 *                 + what is stored there is not a list
 */
forward List:GetItemExtraDataList(Item:itemid, const key[]);

/**
 * @brief      Read a key back as a number.
 *
 * A key that was never written and a key written as `0` read the same, so
 * ask HasItemExtraData when the difference matters.
 *
 * @param      itemid  Item to read.
 * @param      key     Name to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the cell stored under it
 *             - `0` when:
 *                 + `itemid` is not a live item
 *                 + it carries no such key
 */
forward GetItemExtraDataInt(Item:itemid, const key[]);

/**
 * @brief      Read a key back as a float.
 *
 * @param      itemid  Item to read.
 * @param      key     Name to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the float stored under it
 *             - `0.0` when:
 *                 + `itemid` is not a live item
 *                 + it carries no such key
 */
forward Float:GetItemExtraDataFloat(Item:itemid, const key[]);

/**
 * @brief      Read a key back as a string.
 *
 * @param      itemid  Item to read.
 * @param      key     Name to read.
 * @param      output  Reads back the text.
 * @param      size    How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + it carries no such key
 */
forward bool:GetItemExtraDataString(Item:itemid, const key[], output[], size = sizeof (output));

/**
 * @brief      How many keys of its own an item carries.
 *
 * @param      itemid  Item to count.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many keys are on it
 *             - `0` when:
 *                 + `itemid` is not a live item
 *                 + nothing has ever been written to it
 */
forward GetItemExtraDataCount(Item:itemid);

/**
 * @brief      The item's own map, for walking every key it carries.
 *
 * The developer's keys alone: the framework's own fields live in arrays and
 * only appear in the JSON. That is deliberate, and it is why a save has to
 * go through GetItemExtraDataJSON rather than being built by hand from this
 * -- doing the latter would save an item and lose its ammunition.
 *
 * @param      itemid  Item to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the map of its keys
 *             - `INVALID_MAP` when:
 *                 + `itemid` is not a live item
 *                 + it carries no keys of its own
 */
forward Map:GetItemExtraDataMap(Item:itemid);

/**
 * @brief      Everything an item carries, as one piece of JSON.
 *
 * Half of the pair a gamemode needs to persist items. The document holds
 * the developer's keys and, through OnItemSerialize, whatever the framework
 * modules keep in their own arrays -- an amount, a duration, what is inside
 * a container.
 *
 * An item with no keys of its own still produces a document, because it can
 * still have a duration.
 *
 *     new data[512];
 *
 *     GetItemExtraDataJSON(itemid, data);
 *     SetItemExtraDataJSON(itemid, data);
 *
 * @param      itemid  Item to write down.
 * @param      output  Reads back the JSON, or `"{}"` if the item is not
 *                     live.
 * @param      delete  Whether to drop the item's keys afterwards, for a
 *                     save that is on its way to destroying it.
 * @param      size    How much the output holds. Longer text is truncated,
 *                     so size it for the items that are expected.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the JSON was written
 *             - `false` `itemid` is not a live item
 */
forward bool:GetItemExtraDataJSON(Item:itemid, output[], bool:delete = false, size = sizeof (output));

/**
 * @brief      Put everything an item carries back, from one piece of JSON.
 *
 * The other half of the pair. Keys already on the item are replaced rather
 * than merged with, and the framework's own half is handed to the modules
 * through OnItemDeserialize and then taken back out of the map.
 *
 * @param      itemid  Item to fill in.
 * @param      json    A document GetItemExtraDataJSON produced.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the item carries what the document said
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + `json` is not valid JSON
 */
forward bool:SetItemExtraDataJSON(Item:itemid, const json[]);

/**
 * @brief      Write an item into a JSON node somebody else owns.
 *
 * What GetItemExtraDataJSON is built on, for a caller that is assembling a
 * larger document -- a container writing down what is inside it, where each
 * item is one node of an array.
 *
 * The node is the caller's to release.
 *
 * @param      itemid  Item to write down.
 * @param      node    Object to write into.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the node carries the item
 *             - `false` `itemid` is not a live item
 */
forward bool:SerializeItem(Item:itemid, Node:node);

/**
 * @brief      Fill an item in from a JSON node somebody else owns.
 *
 * The inverse of SerializeItem, and what SetItemExtraDataJSON is built on.
 * The node is the caller's to release.
 *
 * @param      itemid  Item to fill in.
 * @param      node    Object to read from.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the item carries what the node said
 *             - `false` `itemid` is not a live item
 */
forward bool:DeserializeItem(Item:itemid, Node:node);

/**
 * @brief      Drop the framework's half of an item's keys.
 *
 * Called at the end of a load, once the modules have taken what is theirs
 * out of the document, so the map is left holding only the developer's
 * keys -- which is what GetItemExtraDataMap promises.
 *
 * @param      itemid  Item to tidy.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many keys were dropped
 *             - `0` when:
 *                 + `itemid` is not a live item
 *                 + it carried none of them
 */
forward RemoveReservedItemExtraData(Item:itemid);

/**
 * # Macros
 *
 * Other names for the four build-key calls above, reading as verbs rather than
 * as get and set. They are the same functions, so whichever name a catalogue
 * uses, the behaviour and the documentation are the ones written above.
 *
 * The forwards are here so that a name only ever reached through the macro is
 * still declared, and so an editor can find it.
 */

#define BindKeyToItemBuild SetKeyItemBuild
#define FetchItemBuildByKey GetKeyItemBuild
#define DoesKeyHaveItemBuild HasKeyAnyItemBuild
#define DoesKeyMatchItemBuild HasKeyItemBuild

/**
 * @brief      Another name for SetKeyItemBuild.
 *
 * @param      key      Name to register it under.
 * @param      buildid  Build the name means.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     what SetKeyItemBuild returns
 */
forward bool:BindKeyToItemBuild(const key[], ItemBuild:buildid);

/**
 * @brief      Another name for GetKeyItemBuild.
 *
 * @param      key  Name to look up.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     what GetKeyItemBuild returns
 */
forward ItemBuild:FetchItemBuildByKey(const key[]);

/**
 * @brief      Another name for HasKeyAnyItemBuild.
 *
 * @param      key  Name to look for.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     what HasKeyAnyItemBuild returns
 */
forward bool:DoesKeyHaveItemBuild(const key[]);

/**
 * @brief      Another name for HasKeyItemBuild.
 *
 * @param      key      Name to check.
 * @param      buildid  Build it should mean.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     what HasKeyItemBuild returns
 */
forward bool:DoesKeyMatchItemBuild(const key[], ItemBuild:buildid);

/**
 * # Item Build
 */

static stock bool:IsValidItemKeyInternal(const key[]) {
    new const
        length = strlen(key)
    ;

    if (length == 0) {
        return false;
    }

    return length < MAX_ITEM_KEY_LENGTH;
}

stock bool:SetKeyItemBuild(const key[], ItemBuild:buildid) {
    if (!IsValidItemKeyInternal(key)) {
        return false;
    }

    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    if (!map_valid(gItemBuildMap)) {
        gItemBuildMap = map_new();
    }

    map_str_set(gItemBuildMap, key, _:buildid);

    strcopy(gItemBuildKey[buildid], key);

    return true;
}

stock bool:GetItemBuildKey(ItemBuild:buildid, output[], size = sizeof (output)) {
    output[0] = EOS;

    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    if (gItemBuildKey[buildid][0] == EOS) {
        return false;
    }

    strcopy(output, gItemBuildKey[buildid], size);

    return true;
}

stock ItemBuild:GetKeyItemBuild(const key[]) {
    if (!HasKeyAnyItemBuild(key)) {
        return INVALID_ITEM_BUILD_ID;
    }

    return ItemBuild:map_str_get(gItemBuildMap, key);
}

stock bool:HasKeyAnyItemBuild(const key[]) {
    if (!map_valid(gItemBuildMap)) {
        return false;
    }

    return map_has_str_key(gItemBuildMap, key);
}

stock bool:HasKeyItemBuild(const key[], ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    return GetKeyItemBuild(key) == buildid;
}

/**
 * # Events
 */

/**
 * @brief      An item is being written down.
 *
 * What a module answers to put its own field into the document. A module
 * keeps that field in an array because a duration read on every menu redraw
 * would be felt as a map lookup, and only writes it here -- so persistence
 * stays one JSON blob while the runtime read stays an array read.
 *
 * Write under a key starting with ITEM_RESERVED_KEY_PREFIX, which the
 * developer's own keys can never collide with.
 *
 * @param      itemid  Item being written down.
 * @param      node    Object to add keys to.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnItemSerialize(Item:itemid, Node:node);

/**
 * @brief      An item is being filled back in.
 *
 * The inverse of OnItemSerialize: a module takes its own field back out of
 * the document and puts it where it keeps it. Everything not taken by the
 * time this returns is dropped from the reserved half.
 *
 * @param      itemid  Item being filled in.
 * @param      node    Object to read keys from.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnItemDeserialize(Item:itemid, Node:node);

/**
 * # Item
 */

static stock ClearItemExtraDataInternal(Item:itemid) {
    if (map_valid(gItemExtraDataMap[itemid])) {
        map_delete_deep(gItemExtraDataMap[itemid]);
    }

    gItemExtraDataMap[itemid] = INVALID_MAP;
}

static stock Map:ResolveItemMapInternal(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return INVALID_MAP;
    }

    return map_valid(gItemExtraDataMap[itemid]) ? gItemExtraDataMap[itemid] : INVALID_MAP;
}

stock bool:HasItemAnyExtraData(Item:itemid) {
    new const
        Map:map = ResolveItemMapInternal(itemid)
    ;

    return map != INVALID_MAP && map_size(map) != 0;
}

stock bool:HasItemExtraData(Item:itemid, const key[]) {
    new const
        Map:map = ResolveItemMapInternal(itemid)
    ;

    return map != INVALID_MAP && map_has_str_key(map, key);
}

stock bool:SetItemExtraData(Item:itemid, const key[], Variant:value) {
    if (!IsValidItemKeyInternal(key)) {
        return false;
    }

    if (key[0] == ITEM_RESERVED_KEY_PREFIX) {
        return false;
    }

    if (!IsValidItem(itemid)) {
        return false;
    }

    if (!map_valid(gItemExtraDataMap[itemid])) {
        gItemExtraDataMap[itemid] = map_new();
    }

    if (map_size(gItemExtraDataMap[itemid]) >= MAX_ITEM_ATTRIBUTES && !map_has_str_key(gItemExtraDataMap[itemid], key)) {
        return false;
    }

    switch (var_tag_uid(value)) {
        case tag_uid_cell, tag_uid_bool, tag_uid_list: {
            map_str_set(gItemExtraDataMap[itemid], key, var_get(value));
        }

        case tag_uid_float: {
            map_str_set(gItemExtraDataMap[itemid], key, Float:var_get(value));
        }

        case tag_uid_char: {
            new
                buffer[MAX_JSON_BUFFER_LENGTH]
            ;

            var_get_str(value, buffer);
            map_str_set_str(gItemExtraDataMap[itemid], key, buffer);
        }
    }

    return true;
}

stock List:GetItemExtraDataList(Item:itemid, const key[]) {
    new const
        Map:map = ResolveItemMapInternal(itemid)
    ;

    if (map == INVALID_MAP) {
        return INVALID_LIST;
    }

    if (!map_has_str_key(map, key)) {
        return INVALID_LIST;
    }

    new const
        List:l = List:map_str_get(map, key)
    ;

    if (!list_valid(l)) {
        return INVALID_LIST;
    }

    return l;
}

stock GetItemExtraDataInt(Item:itemid, const key[]) {
    new const
        Map:map = ResolveItemMapInternal(itemid)
    ;

    if (map == INVALID_MAP) {
        return 0;
    }

    if (!map_has_str_key(map, key)) {
        return 0;
    }

    return map_str_get(map, key);
}

stock Float:GetItemExtraDataFloat(Item:itemid, const key[]) {
    new const
        Map:map = ResolveItemMapInternal(itemid)
    ;

    if (map == INVALID_MAP) {
        return 0.0;
    }

    if (!map_has_str_key(map, key)) {
        return 0.0;
    }

    return Float:map_str_get(map, key);
}

stock bool:GetItemExtraDataString(Item:itemid, const key[], output[], size = sizeof (output)) {
    new const
        Map:map = ResolveItemMapInternal(itemid)
    ;

    if (map == INVALID_MAP) {
        return false;
    }

    if (!map_has_str_key(map, key)) {
        return false;
    }

    map_str_get_str(map, key, output, size);

    return true;
}

stock GetItemExtraDataCount(Item:itemid) {
    new const
        Map:map = ResolveItemMapInternal(itemid)
    ;

    return map != INVALID_MAP ? map_size(map) : 0;
}

stock Map:GetItemExtraDataMap(Item:itemid) {
    if (!HasItemAnyExtraData(itemid)) {
        return INVALID_MAP;
    }

    return gItemExtraDataMap[itemid];
}

stock bool:DeserializeItem(Item:itemid, Node:node) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    if (!map_valid(gItemExtraDataMap[itemid])) {
        gItemExtraDataMap[itemid] = map_new();
    }

    ConvertJSONToMap(node, gItemExtraDataMap[itemid]);

    CallLocalFunction("OnItemDeserialize", "ii", _:itemid, _:node);

    RemoveReservedItemExtraData(itemid);

    return true;
}

stock RemoveReservedItemExtraData(Item:itemid) {
    new const
        Map:map = ResolveItemMapInternal(itemid)
    ;

    if (map == INVALID_MAP) {
        return 0;
    }

    new const
        List:reserved = list_new()
    ;

    for (new Iter:iter = map_iter(map); iter_inside(iter); iter_move_next(iter)) {
        new
            key[MAX_ITEM_KEY_LENGTH]
        ;

        iter_get_key_str(iter, key);

        if (key[0] == ITEM_RESERVED_KEY_PREFIX) {
            list_add_str(reserved, key);
        }
    }

    new const
        count = list_size(reserved)
    ;

    for (new i; i != count; ++i) {
        new
            key[MAX_ITEM_KEY_LENGTH]
        ;

        list_get_str(reserved, i, key);

        map_str_remove(map, key);
    }

    list_delete(reserved);

    return count;
}

stock bool:SerializeItem(Item:itemid, Node:node) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    if (!map_valid(gItemExtraDataMap[itemid])) {
        gItemExtraDataMap[itemid] = map_new();
    }

    ConvertMapToJSON(gItemExtraDataMap[itemid], node);

    CallLocalFunction("OnItemSerialize", "ii", _:itemid, _:node);

    return true;
}

stock bool:GetItemExtraDataJSON(Item:itemid, output[], bool:delete = false, size = sizeof (output)) {
    if (!IsValidItem(itemid)) {
        format(output, size, "{}");

        return false;
    }

    new const
        Node:node = JSON_Object()
    ;

    SerializeItem(itemid, node);

    JSON_Stringify(node, output, size);
    JSON_Cleanup(node);

    if (delete) {
        ClearItemExtraDataInternal(itemid);
    }

    return true;
}

stock bool:SetItemExtraDataJSON(Item:itemid, const json[]) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    new
        Node:node
    ;

    if (JSON_Parse(json, node)) {
        return false;
    }

    new const
        bool:read = DeserializeItem(itemid, node)
    ;

    JSON_Cleanup(node);

    return read;
}

/**
 * # Calls
 */

hook OnItemDestroy(Item:itemid) {
    ClearItemExtraDataInternal(itemid);

    return 0;
}