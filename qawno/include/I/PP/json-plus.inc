#if defined _INC_JSON_PLUS
    #endinput
#endif
#define _INC_JSON_PLUS

#include <open.mp>

#include <json>
#include <PawnPlus>

/**
 * # Header
 */

// A two-way bridge between a PawnPlus Map and a JSON document, so a script can
// keep its data in a map at runtime and still hand the database a single text
// column instead of a table of its own.
//
// The shape it covers is a flat object: keys hold a number, a boolean, a string
// or an array of those, mixed types in one array included. That is the shape a
// map can hold without changing who owns what, and it round-trips byte for
// byte, integers staying integers and 3.0 staying a float.
//
// Deeper shapes -- a nested object, an array of arrays, an array of objects --
// have no map equivalent, since holding one would mean nesting a Map: inside a
// Map: and deciding who frees it. They are left out rather than guessed at, in
// both directions, and each declaration below says exactly what that looks like
// for the key involved.
//
// Who owns the node
//
// json.inc collects Node: variables when the frame holding them ends, and it
// counts a function's arguments among those variables. A conversion that took a
// node and did nothing about it would therefore delete the caller's document on
// the way out: the first call would work and every one after it would read
// back empty.
//
// Both converters switch that collection off for the node they are handed, so
// it outlives the call and one parsed document can feed as many maps as needed.
// The cost is that the node no longer frees itself, and the caller must end it
// with JSON_Cleanup:
//
//     new Node:node;
//
//     JSON_Parse(text, node);
//     ConvertJSONToMap(node, map);
//     JSON_Cleanup(node);
//
// StringifyMap owns the node it makes and already does this, so a save path
// that goes through it has nothing to release.
//
// Telling 3 from 3.0
//
// JSON_Get*Int answers 0 on success and refuses a real, which is the only way
// the plugin offers to make that distinction. Every number read below is tried
// as an integer first and only re-read as a float when that refusal comes back,
// so the common case costs one lookup.
//
// Inside an array there is one difference that matters: reading an element
// hands its storage back, so the refused integer read has already consumed it
// and the float has to come from a second fetch of the same index.
// JSON_GetNodeFloat accepts whole numbers too, so it cannot be the probe itself
// without turning every 1 into 1.0.

#if !defined MAX_JSON_KEY_LENGTH
    #define MAX_JSON_KEY_LENGTH (32)
#endif

#if !defined MAX_JSON_BUFFER_LENGTH
    #define MAX_JSON_BUFFER_LENGTH (256)
#endif

/**
 * # Functions
 */

/**
 * @brief      Read every key of a JSON object into an existing map.
 *
 * A number arrives as a cell when it has no fractional part and as a `Float:`
 * otherwise, a boolean as `bool:`, a string as a string element, and an array
 * as a nested `List:` whose elements keep their own types. Writing a key the
 * map already holds replaces it, and a `List:` stored under that key is deleted
 * first, so deserializing twice into the same map neither merges nor leaks.
 *
 * A key longer than MAX_JSON_KEY_LENGTH is skipped, because JSON_Keys can only
 * hand back a truncated copy that no longer names anything in the document. A
 * string longer than MAX_JSON_BUFFER_LENGTH is stored truncated.
 *
 * A key holding null or a nested object is left out of the map entirely. A key
 * holding an array is always created, but elements that are themselves objects
 * or arrays are left out of the list, so `{"a":[[1,2]]}` arrives as an empty
 * list under `"a"` rather than as no key at all. Its neighbours are unaffected
 * either way.
 *
 * The node comes back with automatic cleanup switched off -- see the header --
 * so the same document can feed one map after another and is the caller's to
 * release.
 *
 * @param      node  Document to read. Not released.
 * @param      map   Map to write into. Nothing happens if it is not live.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward void:ConvertJSONToMap(Node:node, Map:map);

/**
 * @brief      Write every entry of a map into an existing JSON object.
 *
 * The inverse of ConvertJSONToMap. Cells, floats, booleans, strings and lists
 * of those are written. An entry holding anything else -- a nested `Map:`, an
 * `Iter:`, a `Variant:` -- is skipped, since it has no JSON form that would
 * read back as the same value.
 *
 * Keys are cut to MAX_JSON_KEY_LENGTH and strings to MAX_JSON_BUFFER_LENGTH.
 *
 * The node comes back with automatic cleanup switched off, as above, so it
 * survives the return and is the caller's to release.
 *
 * @param      map   Map to read. Nothing is written if it is not live.
 * @param      node  Object to write into. Not released.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward void:ConvertMapToJSON(Map:map, Node:node);

/**
 * @brief      Encode a map straight to text, the one call a save path needs.
 *
 * Writes `"{}"` when the map is not live or is empty, so the output is always
 * valid JSON and never left holding whatever it had before. Text longer than
 * `size` is truncated by JSON_Stringify, so size the buffer for the map that
 * is expected.
 *
 * The node it makes is its own and released before it returns, so unlike the
 * two converters this leaves nothing for the caller to clean up.
 *
 * @param      map     Map to encode.
 * @param      output  Reads back the text.
 * @param      delete  Whether to consume the map: deep-deleted once the text
 *                     is produced, which also frees the lists inside it.
 * @param      size    How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward void:StringifyMap(Map:map, output[], bool:delete = false, size = sizeof (output));

/**
 * # External
 */

stock void:ConvertJSONToMap(Node:node, Map:map) {
    JSON_ToggleGC(node, false);

    if (!map_valid(map)) {
        return;
    }

    new
        key[MAX_JSON_KEY_LENGTH]
    ;

    for (new index; !JSON_Keys(node, index, key); ++index) {
        if (key[0] == '\0') {
            continue;
        }

        switch (JSON_GetType(node, key)) {
            case JSON_NODE_NUMBER: {
                new
                    value
                ;

                if (JSON_GetInt(node, key, value)) {
                    new
                        Float:number
                    ;

                    JSON_GetFloat(node, key, number);
                    map_str_set(map, key, number);
                } else {
                    map_str_set(map, key, value);
                }
            }

            case JSON_NODE_BOOLEAN: {
                new
                    bool:value
                ;

                JSON_GetBool(node, key, value);
                map_str_set(map, key, value);
            }

            case JSON_NODE_STRING: {
                new
                    value[MAX_JSON_BUFFER_LENGTH]
                ;

                JSON_GetString(node, key, value);
                map_str_set_str(map, key, value);
            }

            case JSON_NODE_ARRAY: {
                new
                    Node:array
                ;

                if (JSON_GetArray(node, key, array)) {
                    continue;
                }

                new const
                    List:list = list_new()
                ;

                new
                    Node:element,
                    elementIndex = -1
                ;

                while (!JSON_ArrayIterate(array, elementIndex, element)) {
                    switch (JSON_NodeType(element)) {
                        case JSON_NODE_NUMBER: {
                            new
                                value
                            ;

                            if (!JSON_GetNodeInt(element, value)) {
                                list_add(list, value);

                                continue;
                            }

                            new
                                Node:number
                            ;

                            if (JSON_ArrayObject(array, elementIndex, number)) {
                                continue;
                            }

                            new
                                Float:decimal
                            ;

                            JSON_GetNodeFloat(number, decimal);
                            list_add(list, decimal);
                        }

                        case JSON_NODE_BOOLEAN: {
                            new
                                bool:value
                            ;

                            JSON_GetNodeBool(element, value);
                            list_add(list, value);
                        }

                        case JSON_NODE_STRING: {
                            new
                                value[MAX_JSON_BUFFER_LENGTH]
                            ;

                            JSON_GetNodeString(element, value);
                            list_add_str(list, value);
                        }
                    }
                }

                if (map_has_str_key(map, key)) {
                    map_str_remove_deep(map, key);
                }

                map_str_set(map, key, list);
            }

            default: {}
        }
    }
}

stock void:ConvertMapToJSON(Map:map, Node:node) {
    JSON_ToggleGC(node, false);

    if (!map_valid(map)) {
        return;
    }

    new
        key[MAX_JSON_KEY_LENGTH]
    ;

    for (new const Iter:iterator = map_iter(map); iter_inside(iterator); iter_move_next(iterator)) {
        iter_get_key_str(iterator, key);

        switch (iter_tag_uid(iterator)) {
            case tag_uid_bool: {
                JSON_SetBool(node, key, bool:iter_get(iterator));
            }

            case tag_uid_cell: {
                JSON_SetInt(node, key, iter_get(iterator));
            }

            case tag_uid_float: {
                JSON_SetFloat(node, key, Float:iter_get(iterator));
            }

            case tag_uid_char, tag_uid_string: {
                new
                    buffer[MAX_JSON_BUFFER_LENGTH]
                ;

                if (iter_tag_uid(iterator) == tag_uid_char) {
                    iter_get_arr(iterator, buffer);
                } else {
                    str_get(iter_get_str_s(iterator), buffer);
                }

                JSON_SetString(node, key, buffer);
            }

            case tag_uid_list: {
                new const
                    List:list = List:iter_get(iterator)
                ;

                if (!list_valid(list)) {
                    continue;
                }

                JSON_SetArray(node, key, JSON_Array());

                for (new const Iter:element = list_iter(list); iter_inside(element); iter_move_next(element)) {
                    switch (iter_tag_uid(element)) {
                        case tag_uid_bool: {
                            JSON_ArrayAppend(node, key, JSON_Bool(bool:iter_get(element)));
                        }

                        case tag_uid_cell: {
                            JSON_ArrayAppend(node, key, JSON_Int(iter_get(element)));
                        }

                        case tag_uid_float: {
                            JSON_ArrayAppend(node, key, JSON_Float(Float:iter_get(element)));
                        }

                        case tag_uid_char, tag_uid_string: {
                            new
                                buffer[MAX_JSON_BUFFER_LENGTH]
                            ;

                            if (iter_tag_uid(element) == tag_uid_char) {
                                iter_get_arr(element, buffer);
                            } else {
                                str_get(iter_get_str_s(element), buffer);
                            }

                            JSON_ArrayAppend(node, key, JSON_String(buffer));
                        }
                    }
                }
            }
        }
    }
}

stock void:StringifyMap(Map:map, output[], bool:delete = false, size = sizeof (output)) {
    if (!map_valid(map)) {
        format(output, size, "{}");

        return;
    }

    new const
        Node:node = JSON_Object()
    ;

    ConvertMapToJSON(map, node);

    JSON_Stringify(node, output, size);
    JSON_Cleanup(node);

    if (delete) {
        map_delete_deep(map);
    }
}
