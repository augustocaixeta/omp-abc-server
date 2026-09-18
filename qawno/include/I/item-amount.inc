#if defined _item_amount_included
    #endinput
#endif
#define _item_amount_included

#include <open.mp>

#include <chrono>
#include <I\item>

// Required even though I\item pulls it in: pp-hooks includes y_unique above
// its own include guard, so each #include here is what advances the counter
// that keeps two files hooking the same callback from colliding.
#include <pp-hooks>

/**
 * # Header
 */

// The number in the corner of a slot, and what kind of number it is. A round
// count, a weight, a shelf life counting down, litres in a can, or words the
// gamemode writes itself.
//
// The kind belongs to the build, because every copy of a thing is measured the
// same way. The number belongs to the item, because that is the copy.
//
// The numeric amount is a cell rather than a key in the extra data map: it is
// read on every menu redraw and every duration tick, 40 ns against 990 ns
// measured. It still reaches the database as part of the same JSON document,
// through the OnItemSerialize answered in I\PP\item-amount-plus.
//
// ITEM_AMOUNT_CUSTOM is the rare one, so its text is a string handle rather
// than a fixed buffer per item: one cell each, and memory is only taken by the
// items that actually set one.
//
// Durations are not a timer each. They are bucketed by the second they run out
// on, and one timer drains the buckets that have come due. Anything expiring
// within ITEM_DURATION_FINE_HORIZON gets a bucket of its own second; everything
// further out shares a coarse bucket and is re-filed when it comes closer, so a
// thousand items a week away cost a handful of buckets rather than a thousand.
//
// A tick delivers at most MAX_ITEM_DURATION_BUDGET expirations and replays at
// most MAX_ITEM_DURATION_CATCHUP seconds of backlog after a stall. A bucket
// that survives its drain means the budget ran out inside it, so that second
// stays due and the clock does not move past it.

#if !defined MAX_ITEM_AMOUNT_LENGTH
    #define MAX_ITEM_AMOUNT_LENGTH (16)
#endif

#if !defined MAX_ITEM_UNIT_LENGTH
    #define MAX_ITEM_UNIT_LENGTH (8)
#endif

// Expirations delivered per tick. Leftovers wait for the next one.
#if !defined MAX_ITEM_DURATION_BUDGET
    #define MAX_ITEM_DURATION_BUDGET (20000)
#endif

// Seconds of backlog a single tick will replay after a stall.
#if !defined MAX_ITEM_DURATION_CATCHUP
    #define MAX_ITEM_DURATION_CATCHUP (600)
#endif

#if !defined ITEM_DURATION_FINE_HORIZON
    #define ITEM_DURATION_FINE_HORIZON (Seconds:300)
#endif

#if !defined ITEM_DURATION_COARSE_STEP
    #define ITEM_DURATION_COARSE_STEP (Seconds:60)
#endif

#if ITEM_DURATION_FINE_HORIZON <= ITEM_DURATION_COARSE_STEP
    #error ITEM_DURATION_FINE_HORIZON must exceed ITEM_DURATION_COARSE_STEP
#endif

// Answered by GetItemAmountRevision when it cannot know, so a view treats the
// text as changed every time and keeps working.
#define ITEM_AMOUNT_REVISION_UNKNOWN (cellmin)

#if defined USE_CONTAINER_DURATION_HELPERS
    #define SDUR(%0) Seconds:(%0)
    #define MDUR(%0) Seconds:(%0 * _:MinuteInSeconds)
    #define HDUR(%0) Seconds:(%0 * _:HourInSeconds)
    #define DDUR(%0) Seconds:(%0 * _:DayInSeconds)
#endif

enum E_ITEM_AMOUNT_TYPE {
    ITEM_AMOUNT_INT,
    ITEM_AMOUNT_FLOAT,
    ITEM_AMOUNT_DURATION,

    // Litres, written the way a duration is written: the largest unit that
    // applies and the next one down. A float on its own reads as 0.33 and says
    // nothing about what kind of number it is.
    ITEM_AMOUNT_VOLUME,

    ITEM_AMOUNT_CUSTOM
};

static enum _:E_ITEM_BUILD_AMOUNT_DATA {
    E_ITEM_AMOUNT_TYPE:E_ITEM_BUILD_AMOUNT_TYPE,
    E_ITEM_BUILD_AMOUNT_UNIT[MAX_ITEM_UNIT_LENGTH]
};

static
    gItemBuildAmountData[MAX_ITEM_BUILDS][E_ITEM_BUILD_AMOUNT_DATA]
;

static enum _:E_ITEM_AMOUNT_DATA {
    bool:E_ITEM_AMOUNT_SET,
    bool:E_ITEM_AMOUNT_ARMED,

    E_ITEM_AMOUNT_VALUE,
    String:E_ITEM_AMOUNT_TEXT
};

static
    gItemAmountData[MAX_ITEMS][E_ITEM_AMOUNT_DATA]
;

static
    Map:gItemExpiryBuckets = INVALID_MAP,
    gItemExpiryTimer = INVALID_TIMER,
    Timestamp:gItemExpiryLastTick
;

/**
 * # Functions
 */

/**
 * @brief      Say what kind of number a build is measured in.
 *
 * The unit is written straight after the number for the two plain types and
 * ignored by the rest, which write their own: a duration has `D` and `H` in
 * it already, and litres have `L`.
 *
 * @param      buildid  Build to measure.
 * @param      type     Which kind of number it is.
 * @param      unit     What to write after it, for the plain types.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the kind was written
 *             - `false` `buildid` is not a registered build
 */
forward bool:SetItemBuildAmountType(ItemBuild:buildid, E_ITEM_AMOUNT_TYPE:type, const unit[] = "");

/**
 * @brief      What kind of number a build is measured in.
 *
 * A build nobody has measured reads as `ITEM_AMOUNT_INT`, which is also what
 * a build that does not exist reads as.
 *
 * @param      buildid  Build to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the kind it was given
 *             - `ITEM_AMOUNT_INT` when:
 *                 + `buildid` is not a registered build
 *                 + nobody has said otherwise
 */
forward E_ITEM_AMOUNT_TYPE:GetItemBuildAmountType(ItemBuild:buildid);

/**
 * @brief      What is written after a build's number.
 *
 * @param      buildid  Build to read.
 * @param      output   Reads back the unit.
 * @param      size     How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written, empty if it has no unit
 *             - `false` `buildid` is not a registered build
 */
forward bool:GetItemBuildAmountUnit(ItemBuild:buildid, output[], size = sizeof (output));

/**
 * @brief      Write an item's amount as a whole number.
 *
 * Rounds left in a magazine, pills left in a bottle, notes in a roll.
 *
 * @param      itemid  Item to write.
 * @param      value   The number.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the amount was written
 *             - `false` `itemid` is not a live item
 */
forward bool:SetItemAmountInt(Item:itemid, value);

/**
 * @brief      An item's amount as a whole number.
 *
 * An item with no amount and an item whose amount is `0` read the same, so
 * ask HasItemAmountValue when the difference matters.
 *
 * @param      itemid  Item to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the number it carries
 *             - `0` when:
 *                 + `itemid` is not a live item
 *                 + it carries no amount
 */
forward GetItemAmountInt(Item:itemid);

/**
 * @brief      Write an item's amount as a real.
 *
 * The same cell as the whole number, read back differently -- so an item
 * carries one amount, not two, and writing either replaces the other.
 *
 * @param      itemid  Item to write.
 * @param      value   The number.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the amount was written
 *             - `false` `itemid` is not a live item
 */
forward bool:SetItemAmountFloat(Item:itemid, Float:value);

/**
 * @brief      An item's amount as a real.
 *
 * @param      itemid  Item to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the number it carries
 *             - `0.0` when:
 *                 + `itemid` is not a live item
 *                 + it carries no amount
 */
forward Float:GetItemAmountFloat(Item:itemid);

/**
 * @brief      Write words in place of an item's number.
 *
 * For `ITEM_AMOUNT_CUSTOM`: a key that opens house 102, a phone with a
 * number on it. The words are kept on the item and saved with it, so
 * nothing has to work them out again on every redraw.
 *
 * Held as a string handle rather than a buffer per item, so only the items
 * that actually carry words pay for them.
 *
 * @param      itemid  Item to write.
 * @param      value   The words.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the words were written
 *             - `false` `itemid` is not a live item
 */
forward bool:SetItemAmountString(Item:itemid, const value[]);

/**
 * @brief      The words an item carries in place of a number.
 *
 * @param      itemid  Item to read.
 * @param      output  Reads back the words.
 * @param      size    How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + it carries no words
 */
forward bool:GetItemAmountString(Item:itemid, output[], size = sizeof (output));

/**
 * @brief      Take an item's amount away entirely.
 *
 * The number, the words and any duration armed on it. An item cleared this
 * way shows nothing in the corner of its slot.
 *
 * @param      itemid  Item to clear.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it carries no amount now
 *             - `false` `itemid` is not a live item
 */
forward bool:ClearItemAmount(Item:itemid);

/**
 * @brief      Whether an item carries a number.
 *
 * An item can carry a number, words, or both, and they are set
 * independently. This answers for the number alone, which is what anything
 * writing the amount down wants: asking the loose question there would put
 * a number in the document that the item never had.
 *
 * @param      itemid  Item to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` a number has been written to it
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + it carries no number
 */
forward bool:HasItemAmountValue(Item:itemid);

/**
 * @brief      Whether an item carries words.
 *
 * The other half of the pair, for the same reason.
 *
 * @param      itemid  Item to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` words have been written to it
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + it carries no words
 */
forward bool:HasItemAmountString(Item:itemid);

/**
 * @brief      Whether an item carries an amount of any kind.
 *
 * Either a number or words. What a menu asks, because a menu only wants to
 * know whether there is anything to draw in the corner.
 *
 * @param      itemid  Item to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it carries one or the other
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + it carries neither
 */
forward bool:HasItemAmount(Item:itemid);

/**
 * @brief      An item's amount as the text a slot shows.
 *
 * Written the way its build is measured: `12`, `2.50KG`, `01D 22H`,
 * `1L 100ML`, or whatever the gamemode says for a custom one.
 *
 * A custom amount answers the item's own words first, and only asks
 * OnItemAmountFormat when it has none -- so text that was written down
 * costs a copy, and text that has to be worked out costs a callback.
 *
 * The unit is read where it lives rather than through
 * GetItemBuildAmountUnit, which would copy it into a buffer first: that
 * copy was 42% of the cost of formatting a plain number.
 *
 * @param      itemid  Item to format.
 * @param      output  Reads back the text.
 * @param      size    How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` the output was written
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + it carries no amount at all
 */
forward bool:FormatItemAmount(Item:itemid, output[], size = sizeof (output));

/**
 * @brief      A length of time, written short.
 *
 * The largest unit that applies and the next one down: `02Y 14D`,
 * `02D 06H`, `01H 30M`, `05M 20S`, `45S`. Never more than two, because the
 * corner of a slot has no room for more and the smaller units stop being
 * information once the larger ones are big.
 *
 * @param      duration  How long.
 * @param      output    Reads back the text.
 * @param      size      How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` always
 */
forward DurationFormatShort(Seconds:duration, output[], size = sizeof (output));

/**
 * @brief      A volume, written short.
 *
 * Litres and millilitres, in the shape DurationFormatShort writes days and
 * hours: whole litres and the remainder when there are litres, millilitres
 * alone when there are not. Twenty litres is `20L` rather than `20L 000ML`,
 * since a zero remainder is not information -- the same reason a duration
 * of exactly one hour is not written `01H 00M` by hand.
 *
 * @param      litres  How much.
 * @param      output  Reads back the text.
 * @param      size    How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `0` always
 */
forward VolumeFormatShort(Float:litres, output[], size = sizeof (output));

/**
 * @brief      A number that moves exactly when the text would change.
 *
 * A view redrawing every second asks this first and formats only when it
 * moved. An item with two days left shows the same `02D 00H` for an hour,
 * so that is 3599 formats out of 3600 that never had to happen -- and it
 * costs a fraction of producing the text to find out.
 *
 * @param      itemid  Item to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - a number that changes with the text
 *             - `ITEM_AMOUNT_REVISION_UNKNOWN` for a custom amount, because
 *               only the gamemode knows when its own words change, so every
 *               attempt redraws
 *             - `0` when:
 *                 + `itemid` is not a live item
 *                 + it carries no amount
 */
forward GetItemAmountRevision(Item:itemid);

/**
 * @brief      Start an item running out.
 *
 * The amount becomes the second it expires on rather than a length, so a
 * save and a load do not hand it back its whole shelf life. When that
 * second arrives OnItemDurationExpired fires; nothing is destroyed by this
 * module, and what expiry means is the gamemode's to decide.
 *
 * @param      itemid   Item to arm.
 * @param      seconds  How long it has from now.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is counting down
 *             - `false` `itemid` is not a live item
 */
forward bool:SetItemDuration(Item:itemid, Seconds:seconds);

/**
 * @brief      How long an item has left.
 *
 * @param      itemid  Item to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many seconds are left
 *             - `0` when:
 *                 + `itemid` is not a live item
 *                 + it carries no amount
 *                 + the second it expires on has already gone by
 */
forward Seconds:GetItemDuration(Item:itemid);

/**
 * @brief      The second an item expires on.
 *
 * What is actually stored. For a save, or for comparing two items without
 * turning both into lengths first.
 *
 * @param      itemid  Item to read.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - the timestamp it runs out on
 *             - `0` when:
 *                 + `itemid` is not a live item
 *                 + it carries no amount
 */
forward Timestamp:GetItemExpirationTimestamp(Item:itemid);

/**
 * @brief      Whether an item is counting down right now.
 *
 * False for one that has already expired, and false for one loaded from a
 * document until RestoreItemDuration arms it again.
 *
 * @param      itemid  Item to ask.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is armed and waiting
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + it was never armed, or has already expired
 */
forward bool:HasItemDuration(Item:itemid);

/**
 * @brief      Arm a loaded item's countdown again.
 *
 * A load writes the second an item expires on but does not put it back in a
 * bucket, because an item whose second has already gone by should not fire:
 * it expired while nobody was looking. This is what decides between the two
 * and is called from the deserialize path.
 *
 * @param      itemid  Item to arm.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - `true` it is counting down again
 *             - `false` when:
 *                 + `itemid` is not a live item
 *                 + it carries no amount
 *                 + the second it expires on has already gone by
 */
forward bool:RestoreItemDuration(Item:itemid);

/**
 * @brief      How many countdowns are waiting.
 *
 * Everything in every bucket, the coarse ones included, so it counts items
 * that are not due for a week as well as ones due this second.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 *
 * @return     - how many items are armed
 *             - `0` when nothing has ever been armed
 */
forward GetPendingItemDurationCount();

/**
 * # Events
 */

/**
 * @brief      A custom amount needs its words worked out.
 *
 * Only asked for `ITEM_AMOUNT_CUSTOM`, and only when the item carries no
 * words of its own -- so this is for text that is derived rather than kept:
 * a corner that reads from something outside the item.
 *
 * It is asked on every redraw where the revision is unknown, which for a
 * custom amount is every one, so it should be cheap.
 *
 * @param      itemid   Item being drawn.
 * @param      buildid  Its build, already in hand.
 * @param      output   Write the words here.
 * @param      size     How much the output holds.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnItemAmountFormat(Item:itemid, ItemBuild:buildid, output[], size);

/**
 * @brief      An item's countdown has reached zero.
 *
 * Nothing is destroyed by this module: food turning and a parking ticket
 * running out are the same event and different consequences, so the
 * consequence is the gamemode's.
 *
 * The item is still live, and still carries the second it expired on.
 *
 * @param      itemid  Item that ran out.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnItemDurationExpired(Item:itemid);

/**
 * @brief      A second has passed and the due buckets have been drained.
 *
 * Fired once per tick whether anything expired or not, which is what a view
 * showing countdowns redraws on.
 *
 * @date       23:03 10/09/2026
 * @author     augustocaixeta
 */
forward OnItemDurationTick();

/**
 * # Internal
 */

static stock EnsureItemDurationTimer() {
    if (IsValidTimer(gItemExpiryTimer)) {
        return;
    }

    gItemExpiryBuckets = map_new();
    gItemExpiryLastTick = Now();
    gItemExpiryTimer = SetTimer("@Item_DurationTick", 1000, true);
}

// The subtraction is clean because chrono answers Timestamp minus Timestamp in
// Seconds. The rounding is not: there is no operator for an instant modulo a
// duration, so that one line steps outside the tags and comes straight back in.
// It is kept here, alone, rather than spread over the callers.
static stock Timestamp:ItemExpiryKey(Timestamp:expireAt) {
    if (expireAt - gItemExpiryLastTick <= ITEM_DURATION_FINE_HORIZON) {
        return expireAt;
    }

    return Timestamp:(_:expireAt - _:expireAt % _:ITEM_DURATION_COARSE_STEP);
}

static stock ScheduleItemExpiry(Item:itemid, Timestamp:expireAt) {
    EnsureItemDurationTimer();

    new
        Timestamp:key = ItemExpiryKey(expireAt)
    ;

    if (key <= gItemExpiryLastTick) {
        key = gItemExpiryLastTick + Seconds:1;
    }

    if (!map_has_key(gItemExpiryBuckets, _:key)) {
        map_set(gItemExpiryBuckets, _:key, _:list_new());
    }

    list_add(List:map_get(gItemExpiryBuckets, _:key), _:itemid);
}

// Coarse entries land before their real expiry and are re-filed here until they
// fall inside the fine horizon. The same branch absorbs re-armed items, so a
// stale entry can never fire early.
static stock ReleaseItemExpiry(Item:itemid, Timestamp:second) {
    if (!IsValidItem(itemid)) {
        return;
    }

    if (!gItemAmountData[itemid][E_ITEM_AMOUNT_ARMED]) {
        return;
    }

    new const
        Timestamp:expireAt = Timestamp:gItemAmountData[itemid][E_ITEM_AMOUNT_VALUE]
    ;

    if (expireAt > second) {
        ScheduleItemExpiry(itemid, expireAt);

        return;
    }

    gItemAmountData[itemid][E_ITEM_AMOUNT_ARMED] = false;

    CallLocalFunction("OnItemDurationExpired", "i", _:itemid);
}

// The bucket's size is read again after the walk, because a handler is free to
// append to it while we are inside it.
static stock DrainItemExpiryBucket(Timestamp:second, budget) {
    if (!map_has_key(gItemExpiryBuckets, _:second)) {
        return 0;
    }

    new const
        List:bucket = List:map_get(gItemExpiryBuckets, _:second),
        count = min(list_size(bucket), budget)
    ;

    for (new i; i != count; ++i) {
        ReleaseItemExpiry(Item:list_get(bucket, i), second);
    }

    if (count < list_size(bucket)) {
        list_remove_range(bucket, 0, count);

        return count;
    }

    list_delete(bucket);
    map_remove(gItemExpiryBuckets, _:second);

    return count;
}

// How often the text is allowed to change, which is the unit
// DurationFormatShort puts on the right: seconds under an hour, then minutes,
// then hours, then days.
static stock Seconds:DurationGranularityInternal(Seconds:remaining) {
    if (remaining < HourInSeconds) {
        return Seconds:1;
    }

    if (remaining < DayInSeconds) {
        return MinuteInSeconds;
    }

    if (remaining < YearInSeconds) {
        return HourInSeconds;
    }

    return DayInSeconds;
}

forward @Item_DurationTick();

public @Item_DurationTick() {
    new const
        Timestamp:now = Now()
    ;

    new
        budget = MAX_ITEM_DURATION_BUDGET,
        catchup = MAX_ITEM_DURATION_CATCHUP
    ;

    while (gItemExpiryLastTick < now && budget > 0 && catchup--) {
        new const
            Timestamp:second = gItemExpiryLastTick + Seconds:1
        ;

        budget -= DrainItemExpiryBucket(second, budget);

        if (map_has_key(gItemExpiryBuckets, _:second)) {
            break;
        }

        gItemExpiryLastTick = second;
    }

    CallLocalFunction("OnItemDurationTick", "");

    return 1;
}

/**
 * # Item Build
 */

stock bool:SetItemBuildAmountType(ItemBuild:buildid, E_ITEM_AMOUNT_TYPE:type, const unit[] = "") {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    gItemBuildAmountData[buildid][E_ITEM_BUILD_AMOUNT_TYPE] = type;
    gItemBuildAmountData[buildid][E_ITEM_BUILD_AMOUNT_UNIT][0] = EOS;

    strcat(gItemBuildAmountData[buildid][E_ITEM_BUILD_AMOUNT_UNIT], unit);

    return true;
}

stock E_ITEM_AMOUNT_TYPE:GetItemBuildAmountType(ItemBuild:buildid) {
    if (!IsValidItemBuild(buildid)) {
        return ITEM_AMOUNT_INT;
    }

    return gItemBuildAmountData[buildid][E_ITEM_BUILD_AMOUNT_TYPE];
}

stock bool:GetItemBuildAmountUnit(ItemBuild:buildid, output[], size = sizeof (output)) {
    if (!IsValidItemBuild(buildid)) {
        return false;
    }

    strcopy(output, gItemBuildAmountData[buildid][E_ITEM_BUILD_AMOUNT_UNIT], size);

    return true;
}

/**
 * # Amount
 */

stock bool:SetItemAmountInt(Item:itemid, value) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    gItemAmountData[itemid][E_ITEM_AMOUNT_VALUE] = value;
    gItemAmountData[itemid][E_ITEM_AMOUNT_SET] = true;

    return true;
}

stock GetItemAmountInt(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return 0;
    }

    return gItemAmountData[itemid][E_ITEM_AMOUNT_SET] ? gItemAmountData[itemid][E_ITEM_AMOUNT_VALUE] : 0;
}

stock bool:SetItemAmountFloat(Item:itemid, Float:value) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    gItemAmountData[itemid][E_ITEM_AMOUNT_VALUE] = _:value;
    gItemAmountData[itemid][E_ITEM_AMOUNT_SET] = true;

    return true;
}

stock Float:GetItemAmountFloat(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return 0.0;
    }

    return gItemAmountData[itemid][E_ITEM_AMOUNT_SET] ? (Float:gItemAmountData[itemid][E_ITEM_AMOUNT_VALUE]) : 0.0;
}

stock bool:SetItemAmountString(Item:itemid, const value[]) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    if (str_valid(gItemAmountData[itemid][E_ITEM_AMOUNT_TEXT])) {
        str_delete(gItemAmountData[itemid][E_ITEM_AMOUNT_TEXT]);
    }

    gItemAmountData[itemid][E_ITEM_AMOUNT_TEXT] = str_acquire(str_new(value));

    return true;
}

stock bool:GetItemAmountString(Item:itemid, output[], size = sizeof (output)) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    if (!str_valid(gItemAmountData[itemid][E_ITEM_AMOUNT_TEXT])) {
        return false;
    }

    str_get(gItemAmountData[itemid][E_ITEM_AMOUNT_TEXT], output, size);

    return true;
}

stock bool:HasItemAmountString(Item:itemid) {
    return IsValidItem(itemid) && str_valid(gItemAmountData[itemid][E_ITEM_AMOUNT_TEXT]);
}

stock bool:ClearItemAmount(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    if (str_valid(gItemAmountData[itemid][E_ITEM_AMOUNT_TEXT])) {
        str_delete(gItemAmountData[itemid][E_ITEM_AMOUNT_TEXT]);
    }

    gItemAmountData[itemid][E_ITEM_AMOUNT_TEXT] = STRING_NULL;
    gItemAmountData[itemid][E_ITEM_AMOUNT_SET] = false;
    gItemAmountData[itemid][E_ITEM_AMOUNT_ARMED] = false;
    gItemAmountData[itemid][E_ITEM_AMOUNT_VALUE] = 0;

    return true;
}

stock bool:HasItemAmountValue(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    return gItemAmountData[itemid][E_ITEM_AMOUNT_SET];
}

stock bool:HasItemAmount(Item:itemid) {
    if (HasItemAmountValue(itemid)) {
        return true;
    }

    return HasItemAmountString(itemid);
}

/**
 * # Duration
 */

stock bool:SetItemDuration(Item:itemid, Seconds:seconds) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    new const
        Timestamp:expireAt = Now() + seconds
    ;

    gItemAmountData[itemid][E_ITEM_AMOUNT_VALUE] = _:expireAt;
    gItemAmountData[itemid][E_ITEM_AMOUNT_SET] = true;
    gItemAmountData[itemid][E_ITEM_AMOUNT_ARMED] = true;

    ScheduleItemExpiry(itemid, expireAt);

    return true;
}

stock Seconds:GetItemDuration(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return Seconds:0;
    }

    if (!gItemAmountData[itemid][E_ITEM_AMOUNT_SET]) {
        return Seconds:0;
    }

    new const
        Timestamp:expireAt = Timestamp:gItemAmountData[itemid][E_ITEM_AMOUNT_VALUE],
        Timestamp:now = Now()
    ;

    return (expireAt > now) ? (expireAt - now) : Seconds:0;
}

stock Timestamp:GetItemExpirationTimestamp(Item:itemid) {
    return Timestamp:GetItemAmountInt(itemid);
}

stock bool:HasItemDuration(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    return gItemAmountData[itemid][E_ITEM_AMOUNT_ARMED];
}

stock bool:RestoreItemDuration(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    if (!gItemAmountData[itemid][E_ITEM_AMOUNT_SET]) {
        return false;
    }

    new const
        Timestamp:expireAt = Timestamp:gItemAmountData[itemid][E_ITEM_AMOUNT_VALUE]
    ;

    if (expireAt <= Now()) {
        return false;
    }

    gItemAmountData[itemid][E_ITEM_AMOUNT_ARMED] = true;

    ScheduleItemExpiry(itemid, expireAt);

    return true;
}

stock GetPendingItemDurationCount() {
    if (!map_valid(gItemExpiryBuckets)) {
        return 0;
    }

    new
        count
    ;

    for (new Iter:iter = map_iter(gItemExpiryBuckets); iter_inside(iter); iter_move_next(iter)) {
        count += list_size(List:iter_get(iter));
    }

    return count;
}

/**
 * # Formatting
 */

stock bool:FormatItemAmount(Item:itemid, output[], size = sizeof (output)) {
    if (!HasItemAmount(itemid)) {
        return false;
    }

    new const
        ItemBuild:buildid = GetItemBuild(itemid)
    ;

    switch (gItemBuildAmountData[buildid][E_ITEM_BUILD_AMOUNT_TYPE]) {
        case ITEM_AMOUNT_INT: {
            if (gItemBuildAmountData[buildid][E_ITEM_BUILD_AMOUNT_UNIT][0]) {
                format(output, size, "%i%s", GetItemAmountInt(itemid), gItemBuildAmountData[buildid][E_ITEM_BUILD_AMOUNT_UNIT]);
            } else {
                valstr(output, GetItemAmountInt(itemid));
            }
        }

        case ITEM_AMOUNT_FLOAT: {
            if (gItemBuildAmountData[buildid][E_ITEM_BUILD_AMOUNT_UNIT][0]) {
                format(output, size, "%.2f%s", GetItemAmountFloat(itemid), gItemBuildAmountData[buildid][E_ITEM_BUILD_AMOUNT_UNIT]);
            } else {
                format(output, size, "%.2f", GetItemAmountFloat(itemid));
            }
        }

        case ITEM_AMOUNT_DURATION: {
            DurationFormatShort(GetItemDuration(itemid), output, size);
        }

        case ITEM_AMOUNT_VOLUME: {
            VolumeFormatShort(GetItemAmountFloat(itemid), output, size);
        }

        case ITEM_AMOUNT_CUSTOM: {
            if (!GetItemAmountString(itemid, output, size)) {
                CallLocalFunction("OnItemAmountFormat", "iisi", _:itemid, _:buildid, output, size);
            }
        }
    }

    return true;
}

stock GetItemAmountRevision(Item:itemid) {
    if (!HasItemAmount(itemid)) {
        return 0;
    }

    new const
        ItemBuild:buildid = GetItemBuild(itemid)
    ;

    switch (gItemBuildAmountData[buildid][E_ITEM_BUILD_AMOUNT_TYPE]) {
        case ITEM_AMOUNT_DURATION: {
            new const
                Seconds:remaining = GetItemDuration(itemid)
            ;

            return _:(remaining / DurationGranularityInternal(remaining));
        }

        case ITEM_AMOUNT_CUSTOM: {
            return ITEM_AMOUNT_REVISION_UNKNOWN;
        }

        default: {}
    }

    return gItemAmountData[itemid][E_ITEM_AMOUNT_VALUE];
}

stock VolumeFormatShort(Float:litres, output[], size = sizeof (output)) {
    new const
        total = floatround(litres * 1000.0),
        whole = total / 1000,
        millilitres = total % 1000
    ;

    // Under a litre is written in millilitres, because "350ML" is shorter than
    // "0.35L" and reads as a number somebody would say out loud.
    if (whole == 0) {
        format(output, size, "%iML", millilitres);

        return 0;
    }

    if (millilitres == 0) {
        format(output, size, "%iL", whole);

        return 0;
    }

    // One number and not two. "3L 120ML" is the same measurement written twice
    // and it is the longest thing that ever goes in the corner of a slot --
    // long enough to push what it is a measurement of off the edge.
    //
    // Hundredths, and the trailing zero goes: a tenth of a litre is "3.1L" and
    // never "3.10L".
    new const
        hundredths = (millilitres + 5) / 10
    ;

    // Rounded up past the last hundredth, which is the next whole litre and not
    // "1.100L".
    if (hundredths >= 100) {
        format(output, size, "%iL", whole + 1);

        return 0;
    }

    if (hundredths % 10 == 0) {
        format(output, size, "%i.%iL", whole, hundredths / 10);

        return 0;
    }

    format(output, size, "%i.%02iL", whole, hundredths);

    return 0;
}

stock DurationFormatShort(Seconds:duration, output[], size = sizeof (output)) {
    new const years = _:(duration / YearInSeconds);
    duration %= YearInSeconds;

    new const days = _:(duration / DayInSeconds);
    duration %= DayInSeconds;

    new const hours = _:(duration / HourInSeconds);
    duration %= HourInSeconds;

    new const minutes = _:(duration / MinuteInSeconds);
    duration %= MinuteInSeconds;

    if (years > 0) {
        format(output, size, "%02iY %02iD", years, days);
    } else if (days > 0) {
        format(output, size, "%02iD %02iH", days, hours);
    } else if (hours > 0) {
        format(output, size, "%02iH %02iM", hours, minutes);
    } else if (minutes > 0) {
        format(output, size, "%02iM %02iS", minutes, _:duration);
    } else {
        format(output, size, "%02iS", _:duration);
    }

    return 0;
}

/**
 * # Calls
 */

hook OnItemDestroy(Item:itemid) {
    if (str_valid(gItemAmountData[itemid][E_ITEM_AMOUNT_TEXT])) {
        str_delete(gItemAmountData[itemid][E_ITEM_AMOUNT_TEXT]);
    }

    gItemAmountData[itemid][E_ITEM_AMOUNT_TEXT] = STRING_NULL;
    gItemAmountData[itemid][E_ITEM_AMOUNT_SET] = false;
    gItemAmountData[itemid][E_ITEM_AMOUNT_ARMED] = false;
    gItemAmountData[itemid][E_ITEM_AMOUNT_VALUE] = 0;

    return 0;
}
