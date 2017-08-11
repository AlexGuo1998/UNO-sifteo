#pragma once

#include "typedef.h"
#include "assets.gen.h"

//#define FREQ 60 //TODO freq?

//void printPan(VideoBuffer &vid, UID uid, int16_t panx, int16_t &buffered, bool zeropan = false);

void animUpdateScroll(PLAYERMASK mask, UID popupPlayer = -1);

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
			//upper window
			ASSERT(player[i].displaypart <= 1);

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

//change a player's window
//mode:0=force fullscreen 1=upper/full 2=force bottom
inline void changeWindow(UID uid, uint8_t mode) {
	switch (mode) {
	case 0:
		//this only happens when the user is receiving a card, or user is playing a card.
		//must be called after printNoPan
		if (player[uid].displaypart) {
			ASSERT(player[uid].viewbuffer = -17);
			player[uid].vid.setDefaultWindow();
			player[uid].displaypart = 0;
			player[uid].vid.bg0.setPanning(vec(0, 0));
		}
		break;
	case 1:
		if (player[uid].displaypart == 2) {
			player[uid].vid.setWindow(0, 88);
			player[uid].displaypart = 1;
			player[uid].vid.bg0.setPanning(vec(0, 0));
		}
		//else: no need to change
		break;
	case 2:
		if (player[uid].displaypart != 2) {
			player[uid].vid.setWindow(80, 48);
			player[uid].displaypart = 2;
		}
		break;
	default:
		ASSERT(false);
	}
}

void animDropDown(uint8_t count, uint8_t *x);

void animDrawN(uint8_t n);

inline void animSkip(void) {
	uint8_t x = 48;

	//init mode
	for (UID i = 0; i < 12; i++) {
		if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
			//part window
			changeWindow(i, 1);

			//paint card(?) TODO
			player[i].vid.bg1.image(vec(0, 0), BlankCard, 0);

			player[i].vid.sprites[0].move(x, -16);
			player[i].vid.sprites[0].setImage(SkipPopupPic);
		}
	}

	animDropDown(1, &x);
}

inline void animReverse(void) {
	uint8_t x = 48;

	//init mode
	for (UID i = 0; i < 12; i++) {
		if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
			//part window
			changeWindow(i, 1);

			//paint card(?) TODO
			player[i].vid.bg1.image(vec(0, 0), BlankCard, 0);

			player[i].vid.sprites[0].move(x, -16);
			player[i].vid.sprites[0].setImage(ReversePopupPic);
		}
	}

	animDropDown(1, &x);
}

void animShowCardCount(UID uid);

void animShowNowPlayer(UID uid, bool reverse);
