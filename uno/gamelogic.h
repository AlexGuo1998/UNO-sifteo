#pragma once
#include "typedef.h"
#include <sifteo/math.h>
#include <sifteo/video.h>
#include <sifteo/cube.h>
#include "scroller.h"

#define INIT_CARD_COUNT 7

//每个玩家输入名字
void InputName(void);

//玩家形成一个环
void PairLoop(void);

//单局游戏中
void PlaySingleGame(void);

//单局游戏结束，显示计分，etc
void EndSingle(void);

//全部游戏结束，显示总分，etc
void EndAll(void);

