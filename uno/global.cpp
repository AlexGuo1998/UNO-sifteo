#ifdef __VSFIX
#include "vsfix.h"
#endif

#include "global.h"
#include "assets.gen.h"

Random g_random = Random();

TimeStep g_frameclock;

AssetSlot MainSlot = AssetSlot::allocate()
	.bootstrap(BootstrapGroup)
#ifdef _DEBUG
	.bootstrap(BootstrapGroup2)
	.bootstrap(MenuGroup)
	.bootstrap(GameGroup)
#endif
;

AssetLoader g_loader;
AssetConfiguration<4> g_loaderconfig;

UID g_mastercube;
Player player[PLAYER_MAX];
UID playerCount;
CubeSet lostMask;

bool winmode = 0;
uint8_t winscore = 0;

uint8_t g_gamestate = 0;
