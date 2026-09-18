#if defined _SETTINGS
    #endinput
#endif
#define _SETTINGS

/**
 * # Settings
 *
 * Everything the framework sizes itself from, in one place and before anything
 * else. These are read by the Interaction includes at compile time to size their
 * static arrays, so this file has to come before <open.mp> and cannot include
 * anything itself.
 *
 * Nothing here describes the screen. What a thing looks like and where it sits
 * is core/ui/hud.pwn's, which is the file that gets opened with the textdraw
 * editor next to it.
 */

#define MAX_PLAYERS                     (3)

#define MAX_ITEMS                       (Item:2048)
#define MAX_BUTTONS                     (Button:2048)
#define MAX_CONTAINERS                  (Container:2048)

#define MAX_CONTAINER_CAPACITY          (120)
#define MAX_CONTAINER_MENU_SLOTS        (120)
#define MAX_INVENTORY_CAPACITY          (120)

#define USE_CONTAINER_DURATION_HELPERS

/**
 * # Gamemode
 */

#define PERSONAL_SPACE_RADIUS           (2.0)

/**
 * Which key does what, and how that key is spelled on screen. The two stay
 * together: a key nobody can name is a key nobody presses.
 */
#define ITEM_KEY_STORE_ITEM             (KEY_YES)

// Opening the pockets is its own key, and the same one that drops what is in
// the hands: the two never overlap, because one wants empty hands and the
// other wants full ones.
#define INVENTORY_KEY                   (KEY_YES)
#define INVENTORY_KEY_NAME              "~k~~CONVERSATION_NO~"

// Opening is its own key rather than a long press of the pick-up one. Two
// things one key could mean is a rule the player has to be taught; two keys is
// a rule they read once off the screen. It also gives the hold back to the
// things a hold is actually for -- eating, drinking, bandaging.
#define ITEM_KEY_OPEN_ITEM              (KEY_WALK)

#define ITEM_KEY_PICK_UP_NAME           "~k~~VEHICLE_ENTER_EXIT~"
#define ITEM_KEY_OPEN_NAME              "~k~~SNEAK_ABOUT~"
#define ITEM_KEY_STORE_NAME             "~k~~CONVERSATION_YES~"
#define ITEM_KEY_DROP_NAME              "~k~~CONVERSATION_NO~"

/**
 * When reaching for something means crouching for it. A crate on the floor is
 * knelt over; the same crate on a table is opened standing, and so is anything
 * big enough to have a lid at waist height wherever it is sitting.
 *
 * The height is measured from the floor the character is standing on, not from
 * the world -- a crate at the top of a hill and one at the bottom are both on
 * the ground.
 */
#define ITEM_CROUCH_REACH_HEIGHT        (0.55)
#define ITEM_CROUCH_REACH_SIZE          (8)

#define ITEM_CHANGE_CLOTHES_TIME        (3000)
// How long one mouthful takes. The bar is this many times however many bites
// the food has in it, so a burger is a longer hold than a slice of pizza.
#define ITEM_EAT_TIME                   (3200)

// A mouthful of liquid, and how much of the bottle goes down with it. A can of
// 0.33 is three swallows; a jerrycan of twenty is a long stand.
// One mouthful is one whole play of VEND_DRINK2_P: raise, swallow, hold it up
// at rest. Four seconds is that cycle measured on screen, so the bar and the
// arm finish together instead of one waiting on the other.
#define ITEM_DRINK_TIME                 (3400)
#define ITEM_SIP_MILLILITRES            (100)

// However full the vessel, one hold is at most this many mouthfuls. A jerrycan
// is two hundred of them and a thirteen minute bar is not a bar.
#define ITEM_MAX_SIPS_PER_HOLD          (8)

// How many mouthfuls of something that harms go down before the body decides,
// and how long it takes about it. Two is enough to be a mistake and not enough
// to be a choice.
#define ITEM_VOMIT_SIPS                 (2)
#define ITEM_VOMIT_TIME                 (8000)

// 4.1 is the value the documentation gives for playing an animation at its own
// speed, and that is what both of these want -- the timing is the hold's job,
// not the animation's.
#define ITEM_DRINK_ANIMATION_SPEED      (4.1)
#define ITEM_EAT_ANIMATION_SPEED        (4.1)

// One pill. Barely a bar, but the same gesture as everything else.
#define ITEM_PILL_TIME                  (2500)

// The FOOD animation for anything that did not ask for another. It has to be
// one that never ends: EAT_CHICKEN and EAT_PIZZA simply eat, while EAT_BURGER
// takes two bites and wipes its mouth, so looping it reads as a character who
// keeps finishing and starting again.
//
// Bare, with no brackets around it, and it is the one define here that cannot
// have them. It is the default of a `const animation[]` argument, and Pawn will
// only take a plain string literal there -- ("EAT_BURGER") is an expression and
// an expression is not a constant it can copy into an array.
#define ITEM_FOOD_DEFAULT_ANIMATION     "EAT_BURGER"

// Lobbing something away underarm. The grenade library because a grenade is the
// only thing the game has anybody throwing, and the underarm one of the two
// because getting rid of a tin is not an attack. Taken from the animation list
// rather than seen: /devanim says what actually runs if it looks wrong.
#define ITEM_DISCARD_ANIMATION_LIBRARY  ("GRENADE")
#define ITEM_DISCARD_ANIMATION_NAME     ("WEAPON_THROWU")
#define ITEM_DISCARD_ANIMATION_SPEED    (4.1)
#define ITEM_HOLD_ACTION_TIME           (600)

/**
 * Which action wins the bar when two want it. Nothing yet outranks changing
 * clothes, which is the only thing holding a key does so far -- eating and
 * bandaging land here next.
 */
// What a character gets back for giving a property up, as a share of what they
// paid. Never the library's to decide: money is this gamemode's.
#define PROPERTY_REFUND_SHARE           (2)

#define HOLD_PRIORITY_PILL              (12)
#define HOLD_PRIORITY_EAT               (15)
#define HOLD_PRIORITY_DRINK             (15)
#define HOLD_PRIORITY_CLOTHES           (20)

// Under everything a character does with their hands, and above the fuel gauge.
#define HOLD_PRIORITY_CRAFT             (10)

// Under everything. The fuel gauge is a reading rather than a thing being
// done, so anything a character actually does takes the bar off it -- and it
// is offered the bar back the moment they have finished.
#define HOLD_PRIORITY_FUEL              (5)

// Above drinking: somebody stood over a tank with a can in their hands is
// filling it and not taking a swig.
#define HOLD_PRIORITY_REFUEL            (18)

// The animation library a character is in while actually cutting with a
// chainsaw. Checked by name because a name is the game saying what it is doing,
// where an index is a table somebody has to keep. /devanim prints both.
#define ITEM_CHAINSAW_ANIMATION_LIBRARY ("CHAINSAW")


// How often /devanim looks. Fast enough to catch an animation that only plays
// for a moment, slow enough not to fill the chat with the same line.
#define DEV_ANIM_STEP                   (200)

// How fast a can pours into a tank, in millilitres a second. A rate and not a
// length of time: half a tank takes half as long as a whole one, and stopping
// half way through leaves half of it in.
#define ITEM_REFUEL_RATE                (700)

// The corner of a slot now carries a calibre beside its number, and the colour
// codes that separate the two cost six characters nobody can see. Sixteen cells
// was not enough for the text alone; thirty two holds the longest of them --
// "1L 500ML: ~b~~h~Petrol" -- with room to spare.
#define MAX_ITEM_AMOUNT_LENGTH          (32)
