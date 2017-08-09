#ifdef __VSFIX
#include "vsfix.h"
#endif

#include "global.h"

#include "animation.h"
#include "stack.h"

//don't call unless moving
static inline void printPan(UID uid) {
	//LOG("PrintPan for %d,%d\n", uid, buffered);
	changeWindow(uid, 2);
	
	int16_t panx = player[uid].scroller.position;
	player[uid].vid.bg0.setPanning(vec(panx % (18 * 8), 80));

	//x=0~16 时 buffered=1，此时缓冲的区域为8格桌面+2.5张牌

	int16_t loadrowfirst;//第一个加载的列
	int16_t loadrowcount;//要加载的列数

	int8_t loadcardfirst;//要加载的第一张牌的序号
	int8_t loadcardcount;//要加载的牌的数量，1~5张

	int16_t buffered = player[uid].viewbuffer;
	int16_t newbuffered;

	if (panx > buffered * 8 + 8) {//panx大，加载右侧图片
		newbuffered = panx >> 3;

		if (buffered < newbuffered - 18) buffered = newbuffered - 18;//一次最多加载18列

		loadrowfirst = buffered + 17;
		loadrowcount = newbuffered - buffered;

		loadcardfirst = (buffered + 9) >> 2;
		loadcardcount = (newbuffered >> 2) - loadcardfirst + 3;

		//LOG("R\t%d\t%d\t%d\t%d\n", loadrowfirst, loadrowcount, loadcardfirst, loadcardcount);

		goto loadanddraw;

	} else if (panx < buffered * 8 - 8) {//panx小，加载左侧图片
		newbuffered = (panx >> 3) + 1;

		if (buffered > newbuffered + 17) buffered = newbuffered + 17;//一次最多加载17列

		loadrowfirst = newbuffered - 1;
		loadrowcount = buffered - newbuffered;

		loadcardfirst = (newbuffered - 9) >> 2;
		loadcardcount = ((buffered - 2) >> 2) - loadcardfirst - 1;

		//LOG("L\t%d\t%d\t%d\t%d\n", loadrowfirst, loadrowcount, loadcardfirst, loadcardcount);
		//LOG("%d\t%d\t%d\t%d\n", (buffered - 2) >> 2, loadcardfirst, newbuffered, loadcardcount);

	loadanddraw:

		//load
		CARDID id[5] = {-1, -1, -1, -1, -1};
		getPlayerCards(uid, loadcardfirst, loadcardcount, id);

		//draw
		for (int16_t i = loadrowfirst; i < loadrowfirst + loadrowcount; i++) {
			//卡片的第 n=i%4 列
			//列表中的 k=i/4-loadcardfirst-2张卡

			//LOG("n=%d\tk=%d\n", i % 4, i / 4 - loadcardfirst - 1);
			//LOG("%d,%d,%d,%d,%d\n", id[0], id[1], id[2], id[3], id[4]);
			//LOG("%d\n", (i >> 2) - loadcardfirst - 2);
			CARDID idthis = id[(i >> 2) - loadcardfirst - 2];

			if (idthis < 0) {
				for (uint8_t j = 0; j < 5; j++) {
					player[uid].vid.bg0.image(vec(i % 18, 11 + j), BackGroundPic, g_random.randrange(8));
				}
			} else {
				player[uid].vid.bg0.image(vec(i % 18, 11), vec(1, 5), CardPic, vec(i & 3, 0), idthis);
			}
		}
		player[uid].viewbuffer = newbuffered;
	}
}

//this is called only when playing/receiving cards
static inline void printNoPan(UID uid, int8_t pancount) {
	CARDID id[5] = {-1, -1, -1, -1, -1};
	getPlayerCards(uid, pancount - 2, 5, id);
	id[2] = -1;
	for (uint8_t i = 0; i < 16; i++) {
		CARDID idthis = id[(i + 2) >> 2];
		if (idthis < 0) {
			for (uint8_t j = 0; j < 5; j++) {
				player[uid].vid.bg0.image(vec(i, (uint8_t)(11 + j)), BackGroundPic, g_random.randrange(8));
			}
		} else {
			player[uid].vid.bg0.image(vec(i, (uint8_t)11), vec(1, 5), CardPic, vec((i + 2) & 3, 0), idthis);
		}
		
	}
	player[uid].viewbuffer = -17;
	changeWindow(uid, 0);
}

void animUpdateScroll(PLAYERMASK mask, UID popupPlayer) {	
	static UID lastuid = -1;
	static uint16_t animationBuffer = 0;
	static uint8_t blinkcount = 0;
	
	if (lastuid != popupPlayer) {
		lastuid = popupPlayer;
		animationBuffer = 0;
		blinkcount = 0;
	}

	for (UID i = 0; i < 12; i++) {
		if (mask & (PLAYERMASK)(0x80000000 >> i)) {
			player[i].scroller.update(player[i].cid.accel().x);
			bool doUpdatePan = player[i].scroller.status || player[i].scroller.flag;//true if we have to printpan
	
			if (i == popupPlayer) {
				//blink
				if (!player[i].scroller.locked) {
					//2 arrows blinking
					blinkcount = (blinkcount + 1) & 0xDF; //32 loop
					uint8_t state = blinkcount & 0x1F;
					if (state > 16) state = 32 - state;
					state >>= 2;

					bool nowl = player[i].scroller.position >= 64;
					bool nowr = player[i].scroller.position <= player[i].scroller.max - 64;

					if (nowl) {
						if (!(blinkcount & 0x80)) {
							blinkcount = (blinkcount & 0x7F) | 0x80;
							player[i].vid.sprites[0].setImage(ArrowLRPic, 0);
						}
						//move
						player[i].vid.sprites[0].move(state, 0);
					} else {
						if (blinkcount & 0x80) {
							blinkcount = blinkcount & 0x7F;
							player[i].vid.sprites[0].hide();
						}
					}

					if (nowr) {
						if (!(blinkcount & 0x40)) {
							blinkcount = (blinkcount & 0xBF) | 0x40;
							player[i].vid.sprites[1].setImage(ArrowLRPic, 1);
						}
						//move
						player[i].vid.sprites[1].move(128 - 8 - state, 0);
					} else {
						if (blinkcount & 0x40) {
							blinkcount = blinkcount & 0xBF;
							player[i].vid.sprites[1].hide();
						}
					}
				}
				
				
				if (player[i].scroller.flag) { //state changed
					//reset animation buffer
					
					animationBuffer = 0;

					if (player[i].scroller.flag & 1) {	//just stopping: fill popup layer
						CARDID popupcardid;
						bool ret = getPlayerCard(i, player[i].scroller.stop_count, &popupcardid);
						ASSERT(ret);
						player[i].vid.bg1.image(vec(0, 0), CardPic, popupcardid);
						player[i].vid.bg1.setPanning(vec(-48, -8));
					} else {							//just starting: erase popup layer
						player[i].vid.bg1.image(vec(0, 0), BlankCard, 0);
						player[i].vid.bg1.setPanning(vec(-48, -8));//avoid popup card flashing
					}
				} else {	//same state, same user, play animation
					if (!player[i].scroller.status) {//stopped
						if (animationBuffer < 500) {
							//LOG_INT(animationBuffer);
							animationBuffer += g_frameclock.delta().milliseconds();
							if (animationBuffer < 500) {
								player[i].vid.bg1.setPanning(vec(-48, -(8 + ((0 - 8) * animationBuffer / 500))));
							} else {
								player[i].vid.bg1.setPanning(vec(-48, 0));
							}
						}
					}
				}
			}

			if (doUpdatePan) {
				printPan(i);
			}
		}
	}
}

//a 0.30s animation shows one draw a card. Call after card allocated
//printbg0: print the card to bg0 and erase bg1
//auto update scroller
void animDrawCard(UID drawuid, CARDID cardid, bool printbg0) {
	//const int allframes = 750;
	const int allframes = 300;

	ASSERT(playerOn & (PLAYERMASK)(0x80000000 >> drawuid));
	
	//init mode
	//System::finish();
	for (UID i = 0; i < 12; i++) {
		if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
			if (i == drawuid) {
				//reset pan for receiveing user, because we have to access the under layer
				printNoPan(i, player[i].scroller.maxcount);
			} else {
				//upper window
				changeWindow(i, 1);
			}
			player[i].vid.bg1.image(vec(0, 0), CardPic, 54);
		}
	}

	System::paint();

	g_frameclock.next();
	int frames = 0;
	
	//TODO better animation
	while (frames < allframes) {
		Int2 panothers = vec<int>(-24, -24 - ((-48 - 24) * frames) / allframes);

		for (UID i = 0; i < 12; i++) {
			if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
				if (i == drawuid) {
					//pan for receving user
					player[i].vid.bg1.setPanning(
						vec(-24 - ((48 - 24) * frames) / allframes,
							-24 - ((88 - 24) * frames) / allframes)
					);
				} else {
					player[i].vid.bg1.setPanning(panothers);
				}
			}
		}
		System::paint();

		g_frameclock.next();
		frames += g_frameclock.delta().milliseconds();
	}

	//clean all
	for (UID i = 0; i < 12; i++) {
		if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
			if (i != drawuid) {
				//others: erase bg1
				player[i].vid.bg1.setPanning(vec(-24, 48));
				player[i].vid.bg1.image(vec(0, 0), BlankCard, 0);
			} else {
				//receiving user
				player[i].scroller.init(player[i].scroller.maxcount + 1, player[i].scroller.max + 16);
				//player[i].viewbuffer = -17;
				if (printbg0) {
					//print to bg0 then erase bg1
					player[i].vid.bg0.image(vec(6, 11), vec(4, 5), CardPic, vec(0, 0), cardid);
					player[i].vid.bg1.image(vec(0, 0), BlankCard, 0);
				} else {
					//set bg1 pos
					player[i].vid.bg1.setPanning(vec(-48, -88));
					player[i].vid.bg1.image(vec(0, 0), CardPic, cardid);
				}
			}
		}
	}
	System::paint();
}

//a 0.40s animation shows one play a card.
void animPlayCard(UID playuid, CARDID cardid) {
	const int allframes = 400;
	
	ASSERT(playerOn & (PLAYERMASK)(0x80000000 >> playuid));
	int pan0y; //original pany for playing user

	System::finish();

	//init mode
	
	for (UID i = 0; i < 12; i++) {
		if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
			if (i == playuid) {
				//full window
				printNoPan(i, player[playuid].scroller.stop_count);

				pan0y = player[i].vid.bg1.getPanning().y - 80;//add windows pan
				ASSERT(player[i].vid.bg1.getPanning().x == -48);
				//no need to paint this card again
			} else {
				//upper window
				changeWindow(i, 1);

				//paint card
				player[i].vid.bg1.image(vec(0, 0), CardPic, cardid);
			}
		}
	}
	
	g_frameclock.next();
	int frames = 0;

	//TODO better animation
	//user: 48, pan0y -> 72, 24
	//other:72,-48 -> 72, 24
	while (frames < allframes) {
		Int2 panothers = vec<int>(-72, 48 - ((24 - -48) * frames) / allframes);//pan for other users
		for (UID i = 0; i < 12; i++) {
			if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
				if (i == playuid) {
					//pan for receving user
					player[i].vid.bg1.setPanning(
						vec(-48 - ((72 - 48) * frames) / allframes,
							pan0y + ((-24 - pan0y) * frames) / allframes)
					);
				} else {
					player[i].vid.bg1.setPanning(panothers);
				}
			}
		}
		System::paint();

		g_frameclock.next();
		frames += g_frameclock.delta().milliseconds();
	}

	//clean all
	for (UID i = 0; i < 12; i++) {
		if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
			player[i].vid.bg1.image(vec(0, 0), BlankCard, 0);
			player[i].vid.bg0.image(vec(9, 3), CardPic, cardid);
		}
	}
	System::paint();
}

void animDrawN(uint8_t n) {
	const unsigned ontime = 400;//numbers dropping
	const unsigned delaytime = 100;//delay between 2 numbers
	const unsigned sleeptime = 700;//sleep
	//const unsigned offtime = 400;//numbers raising

	const int endy = 40;
	const int y_0 = endy + 16;
	const int y_d = 8;

	const float a = 0.0009; //= (y_0 + 3 * y_d + 2 * sqrt(2 * y_d * (y_0 + y_d))) / pow(ontime, 2)
	const float t_1 = 800 / 3; //= sqrt((y_0 + y_d)/a)
	const float t_d = (ontime - t_1) / 2;
	const unsigned t_split = t_1 + t_d / 2;

	uint8_t numcount, x[3];
	if (n < 10) {
		numcount = 2;
		x[0] = 50;
		x[1] = 62;
	} else {
		numcount = 3;
		x[0] = 44;
		x[1] = 56;
		x[2] = 68;
	}
	
	//System::finish();
	
	//init mode
	for (UID i = 0; i < 12; i++) {
		if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
			//part window
			changeWindow(i, 1);

			//paint card(?) TODO
			player[i].vid.bg1.image(vec(0, 0), BlankCard, 0);
			
			player[i].vid.sprites[0].move(x[0], -16);
			player[i].vid.sprites[1].move(x[1], -16);
			player[i].vid.sprites[0].setImage(NumbersPic, 10);// '+'
			if (numcount == 2) {
				player[i].vid.sprites[1].setImage(NumbersPic, n);
			} else {
				player[i].vid.sprites[2].move(x[2], -16);
				player[i].vid.sprites[1].setImage(NumbersPic, n / 10);
				player[i].vid.sprites[2].setImage(NumbersPic, n % 10);
			}
		}
	}

	g_frameclock.next();

	//on
	int frames = 0;
	while (frames < (ontime - delaytime) + numcount * delaytime) {
		int8_t y[3];
		//calc y
		for (uint8_t j = 0; j < 3; j++) {
			int frames_j = (signed)frames - j * delaytime;
			if (frames_j <= 0) {
				y[j] = -16;
			} else if (frames_j >= ontime) {
				y[j] = endy;
			} else if (frames_j < t_split) {
				y[j] = endy + y_d - a * ((frames_j - t_1) * (frames_j - t_1));
			} else {
				y[j] = a * ((ontime - frames_j) * (ontime - frames_j)) + endy;
			}
		}
		//LOG("f=%d\t%d\t%d\t%d\n", frames, y[0], y[1], y[2]);

		for (UID i = 0; i < 12; i++) {
			if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
				for (uint8_t j = 0; j < numcount; j++) {
					player[i].vid.sprites[j].move(x[j], y[j]);
				}
			}
		}
		System::paint();

		g_frameclock.next();
		frames += g_frameclock.delta().milliseconds();
	}

	//on - fin
	for (UID i = 0; i < 12; i++) {
		if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
			for (uint8_t j = 0; j < numcount; j++) {
				player[i].vid.sprites[j].move(x[j], endy);
			}
		}
	}


	//sleep
	frames -= (ontime - delaytime) + numcount * delaytime;
	while (frames < sleeptime) {
		System::paint();
		g_frameclock.next();
		frames += g_frameclock.delta().milliseconds();
	}

#if 0

	//off
	frames -= sleeptime;
	while (frames < offtime) {
		//todo
		uint8_t y = -16;
		for (UID i = 0; i < 12; i++) {
			if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
				for (uint8_t j = 0; j < numcount; j++) {
					player[i].vid.sprites[j].move(x[j], y);
				}
			}
		}
		System::paint();
		g_frameclock.next();
		frames += g_frameclock.delta().milliseconds();
	}

#endif // 0

	//clean all
	for (UID i = 0; i < 12; i++) {
		if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
			for (uint8_t j = 0; j < 3; j++) {
				player[i].vid.sprites[j].hide();
			}
			//player[i].viewbuffer = -17;//TODO is it needed?
		}
	}
	System::paint();
}
