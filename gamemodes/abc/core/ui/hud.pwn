#if defined _CORE_UI_HUD
    #endinput
#endif
#define _CORE_UI_HUD

/**
 * # Where things sit on the screen
 *
 * The positions, sizes and colours of everything the gamemode draws, including
 * the numbers the libraries read for the textdraws they own. A library ships
 * defaults so that it works on its own; these are this gamemode saying
 * otherwise, which is why the file holds nothing but defines and is included
 * before the libraries that read them.
 *
 * Every number here came out of the textdraw editor, and nothing else in the
 * gamemode carries one -- so this is the only file to open when something is a
 * few pixels off.
 */

/**
 * # The inventory
 */

// The pockets sit on the right of the screen, because the left is where
// whatever they are being emptied into or filled from is drawn.
#define INVENTORY_MENU_X            (389.0)
#define INVENTORY_MENU_Y            (147.0)
#define INVENTORY_SLOT_SIZE_X       (35.0)
#define INVENTORY_SLOT_SIZE_Y       (40.0)
#define INVENTORY_MENU_ROWS         (5)
#define INVENTORY_MENU_COLUMNS      (6)
#define INVENTORY_MENU_COLOUR       (0x202020FF)
#define INVENTORY_MENU_TITLE        ("INVENTORY")

/**
 * # Anything else with an inside
 *
 * A second grid, drawn to the left of the pockets and showing whatever the
 * character has opened: a crate, a locker, a bin, a shop counter. Narrower than
 * the pockets by a column, so the two read as the character's own space and
 * somewhere else rather than as one wide grid.
 *
 * The slots are the same size as the pockets' on purpose. An item is the same
 * item on either side and a square that changed size between them would say it
 * was not.
 */

#define CONTAINER_MENU_X            (191.0)
#define CONTAINER_MENU_Y            (147.0)
#define CONTAINER_MENU_SLOT_SIZE_X  (35.0)
#define CONTAINER_MENU_SLOT_SIZE_Y  (40.0)
#define CONTAINER_MENU_ROWS         (5)
#define CONTAINER_MENU_COLUMNS      (5)
#define CONTAINER_MENU_COLOUR       (0x202020FF)
#define CONTAINER_MENU_TITLE        ("CONTAINER")

// The slot a character has picked up something from and not yet put down.
#define INVENTORY_SELECTED_COLOUR   (0x5A7A32FF)

/**
 * # Rarity
 *
 * The square an item of note stands in. Dark, because they sit in the grid
 * rather than on top of it: the square has to read as one of thirty and still
 * say gold from across the screen.
 *
 * Nothing else is tinted. Most items have no rarity at all, so most of the grid
 * stays INVENTORY_MENU_COLOUR and these are worth looking at when they appear.
 */

#define RARITY_COMMON_COLOUR        (0x6E6E6EFF)
#define RARITY_UNCOMMON_COLOUR      (0x2E6B2EFF)
#define RARITY_RARE_COLOUR          (0x2A4A85FF)
#define RARITY_EPIC_COLOUR          (0x63308AFF)
#define RARITY_LEGENDARY_COLOUR     (0x8A6A16FF)

#define INVENTORY_MOVE_SOUND        (45400)


// How fast a weapon that runs on a liquid gets through it, in millilitres a
// second. A chainsaw holds two litres, so it runs for twenty.
#define ITEM_FUEL_BURN_RATE         (100)

/**
 * # Names hanging over things in the world
 *
 * These are measured from the thing's button and not from the thing, because
 * that is where the framework hangs a label. A button already stands clear of
 * what it belongs to -- ITEM_FLOOR_OFFSET, near enough a metre, for anything
 * lying on the ground, and whatever a locker was built with for furniture --
 * so the name is at reading height before either of these is added.
 *
 * Which is to say they are a nudge and not a height. A metre here put the name
 * of a rifle two metres over the rifle.
 */

#define WORLD_LABEL_COLOUR          (0xC8C8C8FF)
#define WORLD_LABEL_ITEM_OFFSET_Z   (0.0)
#define WORLD_LABEL_LOCKER_OFFSET_Z (0.1)

/**
 * # The action menu
 *
 * Six rows rather than the library's five, which is four actions beside INFO
 * and CLOSE. A weapon that takes ammunition wants EQUIP, UNLOAD, GIVE and DROP,
 * and at five one of them -- DROP, being registered last -- simply stopped
 * being drawn.
 */

// How the corner of a slot writes rounds: the number first, because that is
// what anybody glances at, and the calibre after it in a lighter blue so the
// two do not read as one word.
//
// The weapon's capacity is deliberately not in it. A weapon given back through
// GivePlayerWeapon always comes loaded, so a clip figure here would be a second
// number counting what the game is already counting -- the total is the only
// one this side actually owns.
#define ITEM_AMMUNITION_FORMAT      "%ix ~g~~h~~h~%s"

// The same shape for a calibre that is poured, whose amount is already text by
// the time it gets here -- "1L 500ML" rather than a number of millilitres.
#define ITEM_AMMUNITION_VOLUME_FORMAT "%s ~g~~h~~h~%s"

// How many rows may be registered at all, which is not how many an item shows.
// The library's default is eight and there are nine: LOAD and UNLOAD pushed
// DROP past the end, and AddItemAction answers INVALID_ITEM_ACTION_ID rather
// than making room -- so DROP quietly stopped existing on every item in the
// game. Sixteen leaves somewhere to grow.
#define MAX_ITEM_ACTIONS            (ItemAction:16)

#define MAX_ITEM_ACTION_ROWS        (6)

#define ITEM_ACTION_CLOSE_LABEL     ("CLOSE")
#define ITEM_ACTION_INFO_LABEL      ("INFO")

/**
 * # The hold bar
 *
 * The frame of the gym HUD's power bar, with its label in the gym's own place
 * to the left of it: the two are the same shape of thing and the eye should
 * find them in the same corner.
 */

// Found by eye with /devbar rather than worked out, and worth saying why.
//
// progress2 turns a height into a textdraw letter size as (height - 4) / 10,
// and hands back nothing once the height reaches four. What the game draws for
// a letter size of nothing is not something this side can predict -- it is not
// zero, and it is not four either. Both attempts at deriving a minimum from
// that formula came out wrong, one of them making the bar taller than it
// started.
//
// So these are measured. The fill here works out to a letter size of nothing
// and looks right; the same fill inside a 4.5 frame did not, which says the
// frame has to be big enough to hold whatever the game decides that is, and
// that the number to move when it looks wrong is the frame.
//
// A textdraw grows downward from its corner, so the Y above takes back the two
// pixels the taller frame added to the bottom.
//
// The border is wider at the ends than at the top and bottom because progress2
// adds 1.25 of its own to the horizontal padding and nothing to the vertical.
// Taking the horizontal padding to nothing is what evens them up.
#define HOLD_ACTION_BAR_X           (558.0)
#define HOLD_ACTION_BAR_Y           (186.0)
#define HOLD_ACTION_BAR_WIDTH       (50.0)
#define HOLD_ACTION_BAR_HEIGHT      (7.0)
#define HOLD_ACTION_BAR_PADDING_X   (0.0)
#define HOLD_ACTION_BAR_PADDING_Y   (0.8)

// The title is right aligned, so this is where it ends rather than where it
// begins, and the gap to the bar is deliberate: the two are one line with
// something between them, not a word crammed against a box.
#define HOLD_ACTION_TITLE_X         (470.0)
#define HOLD_ACTION_TITLE_Y         (178.0)
#define HOLD_ACTION_TITLE_LETTER_X  (0.400)
#define HOLD_ACTION_TITLE_LETTER_Y  (1.800)

/**
 * # Sentences
 *
 * How long one stays up. A notice answers something the character just did and
 * a prompt says what is true where they are standing, so the prompt outlasts it
 * -- walking away is what ends a prompt, not the clock.
 */

#define PLAYER_NOTICE_TIME          (3500)

#define PLAYER_PROMPT_TIME          (4000)

/**
 * Long enough for the sentence and for what it names. A key written as ~k~ is
 * around twenty characters that the client draws as one glyph, an item's full
 * name is up to 67 on its own, and the longest line here spends both twice --
 * so the buffer is nothing like the length of what is read on screen.
 *
 * The second one is extended-text-styles' own, raised for the same reason and
 * again for what it does after: a sentence is broken to the width of the style
 * by writing ~n~ into this same buffer, so the line breaks need room too.
 */
#define MAX_PLAYER_PROMPT_LENGTH    (192)

#define MAX_GAME_TEXT_LENGTH        (256)
