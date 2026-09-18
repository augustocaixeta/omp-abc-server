/**
 * # Roleplay
 *
 * A gamemode built on the Interaction framework (qawno/include/I). The layout
 * follows ScavengeSurvive's: one entry file that does nothing but set the order,
 * and a module per subject underneath it, each hooking the callbacks it cares
 * about and owning its own state.
 *
 * The order below is the only thing this file decides, and it matters twice:
 *
 *   - settings comes before <open.mp>, because the framework sizes its static
 *     arrays from those defines at compile time.
 *   - core/action comes before the action files, because the rows of the item
 *     menu are registered there in the order they are drawn, and each action
 *     file only answers for the row it is given.
 */

#include "./abc/settings.pwn"

#include <open.mp>

// Where everything is drawn, before anything that draws. The libraries below
// read these for their own textdraws, and a library's own numbers are only what
// it looks like when nobody has said otherwise.
#include "./abc/core/ui/hud.pwn"

#include <Pawn.CMD>
#include <sscanf2>
#include <foreach>

#include <extended-text-styles>
#include <personal-space>
#include <hold-action>

#include <I\item>
#include <I\item-attributes>
#include <I\container>
#include <I\inventory>
#include <I\locker>

#include <I\PP\button-plus>
#include <I\PP\item-plus>
#include <I\PP\item-amount-plus>

#include <I\TD\td-container>
#include <I\TD\td-item-action>

#include <property>

/**
 * # Layout
 *
 * A folder is a subject, and the order below is what needs what. A tagged
 * function called before the compiler has met its forward is assumed untagged
 * and then corrected, which costs a reparse and a warning -- so anything that
 * answers questions is included before whatever asks them.
 *
 *   core/ui/         what is on the screen (hud.pwn is up top, see above)
 *   core/item-build/ what a build is, beyond the model the framework keeps
 *   core/item/       one file per kind of item, and what that kind does
 *   core/player/     the character: their pockets, their hands, their reach
 *   core/admin/      tools for building the gamemode, expected to come out
 *   catalogue/       the builds themselves, one call each
 */

// Answers about the world that belong to nobody in particular.
#include "./abc/core/util/vehicle.pwn"

#include "./abc/core/ui/notice.pwn"
#include "./abc/core/ui/prompt.pwn"
#include "./abc/core/ui/screen.pwn"

/**
 * # The body
 *
 * What can happen to a character whatever caused it. Before everything that
 * causes it, which is most of the gamemode.
 */

#include "./abc/core/player/vomit.pwn"

/**
 * # What things are made of
 *
 * Registries of their own, answering questions the builds below them ask. A
 * liquid is not an item and not a build: it is a name and a nutrition, and what
 * has some in it is a build's business.
 */

#include "./abc/core/item/liquid.pwn"
#include "./abc/core/item/calibre.pwn"

/**
 * # Item builds
 *
 * What a build is, on top of a model and a name. Each owns its storage keyed by
 * build and its own DefineItemBuild* call, so it can grow into a library of its
 * own without any definition having to move.
 */

#include "./abc/core/item-build/attribute.pwn"
#include "./abc/core/item-build/rarity.pwn"
#include "./abc/core/item-build/ammunition.pwn"
#include "./abc/core/item-build/weapon.pwn"
#include "./abc/core/item-build/container.pwn"
#include "./abc/core/item-build/clothes.pwn"
#include "./abc/core/item-build/stack.pwn"
#include "./abc/core/item-build/liquid-container.pwn"
#include "./abc/core/item-build/fuel.pwn"
#include "./abc/core/item-build/craft.pwn"

/**
 * # Items
 *
 * One file per kind of item and what that kind does when it is used, the way
 * ScavengeSurvive keeps a file per item. A build says what a thing is; this
 * says what happens with it.
 */

#include "./abc/core/item/gift.pwn"
#include "./abc/core/item/food.pwn"
#include "./abc/core/item/pills.pwn"
#include "./abc/core/item/pour.pwn"

/**
 * # The pockets
 *
 * After the builds, because everything here asks what an item is: the rule that
 * a container never goes inside another one is item-build/container's to answer,
 * and this is one of the places that asks. Before the screen, because a window
 * onto a container is drawn by asking what is in it.
 */

#include "./abc/core/player/inventory.pwn"

/**
 * # The screen
 *
 * container/access before item-transfer, because what a click is allowed to do
 * is asked before it is worked out how to do it.
 *
 * item-transfer before container-menu, because a click on a slot is answered by
 * whichever of the two is in the middle of something: a selection waiting for
 * somewhere to go comes before opening anything.
 *
 * item-action.pwn registers the rows of that menu and item-action/ answers them,
 * one file per row -- so a new row is a new file and a line. They cannot be one
 * file because pp-hooks allows a callback only one hook per file.
 */

#include "./abc/core/container/access.pwn"

#include "./abc/core/ui/item-transfer.pwn"
#include "./abc/core/ui/container-menu.pwn"
#include "./abc/core/ui/item-action.pwn"

#include "./abc/core/ui/item-action/use.pwn"
#include "./abc/core/ui/item-action/equip.pwn"
#include "./abc/core/ui/item-action/hold.pwn"
#include "./abc/core/ui/item-action/open.pwn"
#include "./abc/core/ui/item-action/split.pwn"
#include "./abc/core/ui/item-action/load.pwn"
#include "./abc/core/ui/item-action/unload.pwn"
#include "./abc/core/ui/item-action/give.pwn"
#include "./abc/core/ui/item-action/drop.pwn"
#include "./abc/core/ui/item-action/discard.pwn"
#include "./abc/core/ui/item-action/info.pwn"

/**
 * # The character
 *
 * What the character does out in the world: what is within their reach, what is
 * in their hands, and what happens when they hold a key on something. After the
 * screen, because all three of them open one.
 */

#include "./abc/core/player/reach.pwn"
#include "./abc/core/player/interact.pwn"
#include "./abc/core/player/hands.pwn"

/**
 * # The world
 *
 * What is standing in it that is not an item. A locker is an object, a
 * container and a button: it has an inside and is never picked up, which is the
 * whole of what separates it from a crate.
 */

#include "./abc/core/container/link.pwn"

#include "./abc/core/world/locker.pwn"
#include "./abc/core/world/label.pwn"

/**
 * # Property
 *
 * A door with a name on it, an owner behind it and a lock in it. property comes
 * first because it pays for all of them; house adds only what a house is.
 */

#include "./abc/core/property/property.pwn"
#include "./abc/core/property/house.pwn"
#include "./abc/core/property/lockup.pwn"
#include "./abc/core/world/house.pwn"
#include "./abc/core/world/lockup.pwn"

/**
 * # Catalogue
 *
 * Every kind of item the gamemode knows. A kind gets a name, then is told what
 * it is: its attributes, then whichever subsystems apply to it. Nothing is
 * configured by an index into anything.
 */

#include "./abc/catalogue/liquid.pwn"
#include "./abc/catalogue/weapon.pwn"
#include "./abc/catalogue/container.pwn"
#include "./abc/catalogue/consumable.pwn"
#include "./abc/catalogue/clothes.pwn"
#include "./abc/catalogue/personal.pwn"
#include "./abc/catalogue/gear.pwn"
#include "./abc/catalogue/tool.pwn"
#include "./abc/catalogue/material.pwn"
#include "./abc/catalogue/loot.pwn"

// Last: a recipe names builds, and every one of them is an id handed out by
// the files above rather than a number written here.
#include "./abc/catalogue/recipe.pwn"

/**
 * # Robbery
 *
 * A system of its own, not a kind of property: what it takes is items, what it
 * fills is containers, and what pays for it is money and factions later. After
 * the catalogue, because the loot lists name builds.
 */

#include "./abc/core/robbery/robbery.pwn"
#include "./abc/core/robbery/house.pwn"
#include "./abc/core/robbery/lockup.pwn"

#include "./abc/core/player/spawn.pwn"
#include "./abc/core/player/command.pwn"

/**
 * # Development
 *
 * Not gameplay. Everything here is a tool for building the gamemode and is
 * expected to come out, which is why it is the last thing included and the only
 * thing that would leave no hole behind.
 */

#include "./abc/core/admin/help.pwn"
#include "./abc/core/admin/property.pwn"
#include "./abc/core/admin/stats.pwn"
#include "./abc/core/admin/item.pwn"
#include "./abc/core/admin/world.pwn"
#include "./abc/core/admin/debug.pwn"
#include "./abc/core/admin/attach.pwn"

main(){}
