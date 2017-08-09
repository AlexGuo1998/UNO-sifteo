#pragma once
#include <sifteo/math.h>
#include <sifteo/cube.h>
#include <sifteo/asset/image.h> //used by video.h
#include <sifteo/video.h>
#include "scroller.h"

typedef int8_t		UID;
typedef int8_t		CARDID; //-1 = nocard
typedef int8_t		CARDCOUNT;

typedef _SYSCubeIDVector	PLAYERMASK; //player mask, 0x80000000 >> n

//make sure card is 2bytes
#pragma pack(push)
#pragma pack(1)
typedef struct {
	CARDID id;
	UID player;
} Card;
#pragma pack(pop)

typedef struct {
	//UID uid;
	struct CubeID cid;
	struct VideoBuffer vid;
	struct Scroller scroller;//牌列表滚动区
	int16_t viewbuffer = -17;
	uint8_t displaypart = 0;//0=full screen 1=top screen 2=bottom screen
	char name[13];
	uint16_t score;
} Player;
