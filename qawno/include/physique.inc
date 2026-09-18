#if defined _INC_PHYSIQUE
    #endinput
#endif
#define _INC_PHYSIQUE

// fitness.inc builds its gym half behind `#if defined _INC_GYM`: the strength
// it hands to the weights, the breath the machines spend and the tired
// animation it keeps to itself while the gym owns the player. Reaching it with
// fitness already included leaves all of that out of the build, quietly, so it
// is worth saying out loud instead.
#if defined _INC_FITNESS
    #error <physique> has to come before <fitness>, or the two halves do not meet
#endif

// The order below is the dependency: the gym knows nothing about fitness, and
// fitness fills itself in around whatever of the gym is already there.
#include <gym>
#include <fitness>
