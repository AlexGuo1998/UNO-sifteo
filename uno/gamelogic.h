#pragma once
#include "typedef.h"
#include <sifteo/math.h>
#include <sifteo/video.h>
#include <sifteo/cube.h>
#include "scroller.h"

#ifdef _DEBUG
#define INIT_CARD_COUNT 1
#else
#define INIT_CARD_COUNT 7
#endif

void clearPairLoopDisp(UID i);

//玩家形成一个环
void PairLoop(void);

//单局游戏中
//return winner UID
UID PlaySingleGame(void);

//单局游戏结束，显示计分，etc
void EndSingle(UID winner);

//全部游戏结束，显示总分，etc
void EndAll(void);

