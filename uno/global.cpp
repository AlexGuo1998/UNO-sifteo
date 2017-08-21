#ifdef __VSFIX
#include "vsfix.h"
#endif

#include "global.h"
#include "assets.gen.h"

Random g_random = Random();

TimeStep g_frameclock;

AssetSlot MainSlot = AssetSlot::allocate()
	.bootstrap(BootstrapGroup);

AssetLoader g_loader;
AssetConfiguration<4> g_loaderconfig;

UID g_mastercube;
PLAYERMASK playerOn = 0;
Player player[12];
UID playerCount = 0;
UID lostCount = 0;

bool winmode = 0;
uint8_t winscore = 0;

uint8_t g_gamestate = 0;
