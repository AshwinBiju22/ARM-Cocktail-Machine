#ifndef MAKER_H
#define MAKER_H

#include <stdio.h>

#define LCD_ADDR 0x27 // use i2cdetect to find right address
// define gpio connections for up, down and selection buttons
#define UP_BUTTON 17
#define DOWN_BUTTON 27
#define SELECT_BUTTON 22
#define STATE_TRANSITION_TIME 3
// time in seconds
#define DEBOUNCE_TIME 200000 // 200ms
// time between presses, to stop double-input

// volumes all in ml
#define VOLUME_PER_PART 15
#define GLASS_VOLUME 300

#define TIME_PER_ML 0.25
// HOPE THIS IS THE SAME FOR ALL OF THEM
// NEED TO CALCULATE EXPERIMENTALLY

//    Enum Name
//                      Name as a string
//                                                                v Vodka
//                                                                   v Rum
//                                                                      v Triple sec
//                                                                         v Lime juice
//                                                                            v Cranberry juice

#define FSM_STATES                                                               \
	X(Cosmopolitan,     "Cosmopolitan",                           4, 0, 2, 2, 4) \
	X(Cosmorada,        "Cosmorada",                              0, 4, 2, 2, 4) \
	X(LostDaiquiri,     "Lost Daiquiri",                          0, 4, 2, 2, 0) \
	X(CrimsonDaiquiri,  "Crimson Daiquiri",                       0, 4, 0, 2, 4) \
	X(CapeCodder,       "Cape Codder",                            4, 0, 0, 0, 8) \
	X(RumPunch,         "Rum Punch",                              0, 4, 0, 2, 4) \
	X(Kamikaze,         "Kamikaze",                               4, 0, 2, 2, 0) \
	X(RedKamikaze,      "Red Kamikaze",                           3, 0, 2, 2, 3) \
	X(VodkaSour,        "Vodka Sour",                             4, 0, 2, 2, 0) \
	X(Vodkarita,        "Vodkarita",                              4, 0, 2, 2, 0) \
	X(SeaBreeze,        "Sea Breeze",                             4, 0, 0, 4, 4) \
	X(HalfTai,          "Half-Tai",                               1, 3, 1, 1, 1) \
	X(RumSidecar,       "Rum Sidecar",                            0, 4, 2, 2, 0) \
	X(RedWave,          "Red Wave",                               2, 2, 0, 0, 6) \
	X(SunsetSlap,       "Sunset Slap",                            0, 4, 2, 0, 4) \
	X(SiberianSlam,     "Siberian Slam",                          5, 0, 0, 0, 0) \
	X(KrakensKiss,      "Kraken's Kiss",                          0, 5, 0, 0, 0) \
	X(VirginSacrifice,  "Virgin Sacrifice",                       0, 0, 0, 0, 9) \
	X(ZestInPeace,      "Zest In Peace",                          2, 2, 2, 2, 2) \
	X(ThatsIt,          "That's it!\n(for now!)",                 0, 0, 0, 0, 0) \
	X(Dispensing,       "Pouring...\nPlease wait...",             0, 0, 0, 0, 0) \
	X(FinishDispensing, "Finished pouring\nEnjoy your drink",     0, 0, 0, 0, 0) \
	X(Start,            "Welcome!\nPlease wait...",               0, 0, 0, 0, 0) \
	X(Error,            "A critical error\nhas occurred...",      0, 0, 0, 0, 0)

typedef enum {
#define X(elem, str, v, r, t, l, c) elem,
	FSM_STATES
#undef X
		TOTAL_STATE_COUNT
} FSMState;

enum Ingredients { VODKA, RUM, TRIPLE_SEC, LIME_JUICE, CRANBERRY_JUICE, NUM_INGREDIENTS };

void print_with_timestamp(FILE *fp, const char *message);
#endif
