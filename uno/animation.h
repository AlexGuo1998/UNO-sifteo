#pragma once

#include "typedef.h"
#include "assets.gen.h"

//#define FREQ 60 //TODO freq?

//void printPan(VideoBuffer &vid, UID uid, int16_t panx, int16_t &buffered, bool zeropan = false);

void animUpdateScroll(PLAYERMASK mask, UID zeroPanPlayer = -1);

//TODO merge the 3 functions following

//a 0.75s animation shows one draw a card.
void animDrawCard(UID drawuid, CARDID cardid, bool printbg0);

//a 0.75s animation shows one play a card.
void animPlayCard(UID playuid, CARDID cardid);

//a 0.50s animation shows a card directly go to discard pile.
inline void animDiscardCard(PLAYERMASK mask, CARDID cardid) {
	const int allframes = 500;
	//TimeDelta delta = TimeDelta::hz(FREQ);

	//init mode
	//System::finish();
	
	for (UID i = 0; i < 12; i++) {
		if (mask & (PLAYERMASK)(0x80000000 >> i)) {
			//part window
			ASSERT(player[i].vid.windowFirstLine() == 0);
			ASSERT(player[i].vid.windowNumLines() >= 71);
			ASSERT(player[i].vid.bg0.getPanning() == vec(0, 0));

			//player[i].vid.bg1.fillMask(vec(0, 0), vec(4, 6));
			//player[i].vid.bg1.setPanning()
			
			player[i].vid.bg1.setPanning(vec(-24, -24));
			player[i].vid.bg1.image(vec(0, 0), CardPic, cardid);
			
		}
	}
	System::paint();

	g_frameclock.next();
	int frames = 0;

	//TODO better animation
	//24, 24 -> 72, 24
	while (frames < allframes) {
		//LOG_INT(frames);
		Int2 panothers = vec<int>(-(24 + ((72 - 24) * frames) / allframes), -24);//pan for other users
		for (UID i = 0; i < 12; i++) {
			if (mask & (PLAYERMASK)(0x80000000 >> i)) {
				player[i].vid.bg1.setPanning(panothers);
			}
		}
		System::paint();

		g_frameclock.next();
		frames += g_frameclock.delta().milliseconds();
	}

	//clean all
	for (UID i = 0; i < 12; i++) {
		if (mask & (PLAYERMASK)(0x80000000 >> i)) {
			//player[i].vid.bg1.image(vec(0, 0), BlankCard, 0);
			player[i].vid.bg0.image(vec(9, 3), CardPic, cardid);
			player[i].vid.bg1.image(vec(0, 0), BlankCard, 0);
			player[i].vid.bg1.setPanning(vec(32, 48));
		}
	}
	System::paint();
}

inline void paintDefBg(UID id) {
	for (uint8_t y = 0; y < 16; y++) {
		for (uint8_t x = 0; x < 18; x++) {
			player[id].vid.bg0.image(vec(x, y), BackGroundPic, g_random.randrange(8));
		}
	}
}

void animDrawN(uint8_t n);
