#if defined _INC_PROPERTIES
    #endinput
#endif
#define _INC_PROPERTIES

// The framework itself. Everything beside this file is a fragment: one kind of
// place, built on what property.inc already does with the door, the world
// behind it, the name, the owner and the lock, and adding only what makes that
// kind what it is.
//
// A fragment declares its kind once and keeps its own data in a plain array
// indexed by the property id. That id is already unique and already bounded, so
// a fragment never needs ids, a free list or a ceiling of its own:
//
//     static PropertyType:gHouseType = INVALID_PROPERTY_TYPE;
//     static gHouseData[MAX_PROPERTIES][E_HOUSE_DATA];
//
//     hook OnGameModeInit() {
//         gHouseType = RegisterPropertyType("House");
//
//         return 0;
//     }
//
//     hook OnPropertyCreate(Property:propertyid) {
//         static const HOUSE_DATA_NONE[E_HOUSE_DATA];
//
//         // Slots come back round, so a new property starts on whatever the
//         // last one left behind unless it is cleared here.
//         gHouseData[propertyid] = HOUSE_DATA_NONE;
//
//         return 0;
//     }
//
// Fragments are listed below as they are written.
#include <property>

#include "house"
