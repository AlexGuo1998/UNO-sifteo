#pragma once

#include <sifteo/time.h>
#include <sifteo/asset.h>

using namespace Sifteo;

#include "typedef.h"

extern Random g_random;

extern TimeStep g_frameclock;

extern AssetSlot MainSlot;

extern AssetLoader g_loader;
extern AssetConfiguration<4> g_loaderconfig;

extern UID g_mastercube; //显示菜单的方块
extern Player player[12]; //所有玩家，bind之后只使用0~playercount-1
extern UID playerCount;
extern CubeSet lostMask;

//load from so
extern bool winmode; //true=elmination
extern uint8_t winscore; //index only 0=single 1=100 2=150 etc.

//gamestate
//1 = is displaying booting screen
//2 = usercount is set
//4 = user is binded (in game / loading)
//8 = should display boot_finished
//16= pairing
extern uint8_t g_gamestate;

const BG0ROMDrawable::Palette BG0ROM_offcolor = BG0ROMDrawable::WHITE_ON_GREEN;
const BG0ROMDrawable::Palette BG0ROM_oncolor = BG0ROMDrawable::LTBLUE_ON_DKBLUE;
