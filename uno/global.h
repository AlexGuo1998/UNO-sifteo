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
extern PLAYERMASK playerOn; //在场玩家的UID bitmap
extern Player player[12]; //所有玩家，每一个UID对应特定cube
extern UID playerCount;
extern UID playerCount_set; // set by code
extern UID playermap[12]; //以出牌顺序排列的UID列表

//TODO load from so
extern bool winmode; //true=elmination
extern uint8_t winscore; //index only 0=single 1=100 2=150 etc.

//gamestate
//1 = is displaying booting screen
//2 = usercount is set
//4 = user is binded
//8 = should display boot_finished
extern uint8_t g_gamestate;
