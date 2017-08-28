#ifdef __VSFIX
#include "vsfix.h"
#endif

#include "global.h"

#include "gamelogic.h"
#include "stack.h"
#include "animation.h"

static CARDID lastcard;
static uint8_t lastcolor;
static class TiltShakeRecognizer shake;

//r: unpair this.right
static void unpair(UID *rightplayer, UID i, bool r) {
	UID right;
	if (r) {
		right = rightplayer[i];
		rightplayer[i] = -1;
	} else {
		right = i;
		for (i = 0; i < PLAYER_MAX; i++) {
			if (rightplayer[i] == right) {
				rightplayer[i] = -1;
				break;
			}
		}
		if (i == PLAYER_MAX) i = -1;
	}
	
	if (i != -1) {
		player[i].vid.bg0rom.fill(vec(2, 9), vec(12, 1), BG0ROMDrawable::charTile(' ', BG0ROM_offcolor));
	}

	if (right != -1) {
		player[right].vid.bg0rom.fill(vec(2, 5), vec(12, 1), BG0ROMDrawable::charTile(' ', BG0ROM_offcolor));
	}
}

void clearPairLoopDisp(UID i) {
	ASSERT(player[i].vid.mode() == BG0_ROM);
	player[i].vid.bg0rom.erase(BG0ROMDrawable::charTile(' ', BG0ROM_offcolor));
	player[i].vid.bg0rom.text(vec(2, 1), "Neighbor to", BG0ROM_offcolor);
	player[i].vid.bg0rom.text(vec(2, 2), "pair a loop", BG0ROM_offcolor);
	player[i].vid.bg0rom.text(vec(2, 7), player[i].name, BG0ROM_offcolor);
	player[i].vid.bg0rom.plot(vec(0, 5), BG0ROMDrawable::charTile('<', BG0ROM_offcolor));
	player[i].vid.bg0rom.plot(vec(0, 5), BG0ROMDrawable::charTile('<', BG0ROM_offcolor));
	player[i].vid.bg0rom.plot(vec(0, 7), BG0ROMDrawable::charTile('-', BG0ROM_offcolor));
	player[i].vid.bg0rom.plot(vec(15, 7), BG0ROMDrawable::charTile('-', BG0ROM_offcolor));
	player[i].vid.bg0rom.plot(vec(15, 9), BG0ROMDrawable::charTile('>', BG0ROM_offcolor));
}

void PairLoop(void) {	
	UID count = CubeSet::connected().count();
	UID pos[PLAYER_MAX]; //pos[newuid] = olduid

	if (count == 2) {
		pos[0] = 0; pos[1] = 1;
	} else {
		UID rightplayer[PLAYER_MAX];
		memset8((uint8_t *)rightplayer, -1, sizeof(rightplayer));

		bool running;
		PLAYERMASK lastmask = CubeSet::connected().mask();

		for (UID i = 0; i < PLAYER_MAX; i++) {
			clearPairLoopDisp(i);
		}

		do {
			System::paint();

			unsigned i;
			CubeSet cubes = CubeSet::connected();
			if (cubes.mask() != lastmask) {
				CubeSet lost;
				lost.setMask(lastmask & ~cubes.mask());
				while (lost.clearFirst(i)) {
					unpair(rightplayer, i, true);
					unpair(rightplayer, i, false);
				}
				lastmask = cubes.mask();
			}

			while (cubes.clearFirst(i)) {
				CubeID right = Neighborhood(i).cubeAt(RIGHT);
				if (right.isDefined() && Neighborhood(right).cubeAt(LEFT) == i && rightplayer[i] != right) {
					LOG("Pair:%d,%d\n", i, (int)right);

					unpair(rightplayer, i, true);
					unpair(rightplayer, right, false);

					rightplayer[i] = right;

					player[i].vid.bg0rom.fill(vec(2, 9), vec(12, 1), BG0ROMDrawable::charTile(' ', BG0ROM_offcolor));
					player[i].vid.bg0rom.text(vec(2, 9), player[right].name, BG0ROM_offcolor);

					player[right].vid.bg0rom.fill(vec(2, 5), vec(12, 1), BG0ROMDrawable::charTile(' ', BG0ROM_offcolor));
					player[right].vid.bg0rom.text(vec(2, 5), player[i].name, BG0ROM_offcolor);
				}
			}

			running = false;
			cubes = CubeSet::connected();

			while (cubes.clearFirst(i)) {
				if (rightplayer[i] == -1) {
					running = true;
					break;
				}
			}
			if (!running) {
				//check loop
				PLAYERMASK loopplayer = 0;
				CubeSet::connected().findFirst(i);
				UID nowplayer = i;

				do {
					loopplayer |= (PLAYERMASK)(0x80000000 >> nowplayer);
					nowplayer = rightplayer[nowplayer];
				} while ((nowplayer != i) && !(loopplayer & (PLAYERMASK)(0x80000000 >> nowplayer)));
				running = (loopplayer != CubeSet::connected().mask());
			}

		} while (running);

		//fill pos[]
		{
			unsigned startplayer;
			CubeSet::connected().findFirst(startplayer);
			UID nowplayer = startplayer;
			UID index = 0;
			do {
				pos[index] = nowplayer;
				nowplayer = rightplayer[nowplayer];
				index++;
			} while (nowplayer != startplayer);
		}
	}
	
	//ending
	{
		unsigned startplayer;
		uint8_t starty = (12 - count) >> 1;
		CubeSet::connected().findFirst(startplayer);
		for (UID i = 0; i < PLAYER_MAX; i++) {
			player[i].vid.bg0rom.erase(BG0ROMDrawable::charTile(' ', BG0ROM_offcolor));
			for (UID index = 0; index < count; index++) {
				UID nowplayer = pos[index];
				player[i].vid.bg0rom.text(vec<int>(2, starty + index), player[nowplayer].name, BG0ROM_offcolor);
				if (nowplayer == i) {
					player[i].vid.bg0rom.plot(vec<int>(0, starty + index), BG0ROMDrawable::charTile('-', BG0ROM_offcolor));
					player[i].vid.bg0rom.plot(vec<int>(15, starty + index), BG0ROMDrawable::charTile('-', BG0ROM_offcolor));
					player[i].vid.bg0rom.plot(vec<int>(0, starty + (index + count - 1) % count), BG0ROMDrawable::charTile('<', BG0ROM_offcolor));
					player[i].vid.bg0rom.plot(vec<int>(15, starty + (index + count - 1) % count), BG0ROMDrawable::charTile('<', BG0ROM_offcolor));
					player[i].vid.bg0rom.plot(vec<int>(0, starty + (index + 1) % count), BG0ROMDrawable::charTile('>', BG0ROM_offcolor));
					player[i].vid.bg0rom.plot(vec<int>(15, starty + (index + 1) % count), BG0ROMDrawable::charTile('>', BG0ROM_offcolor));
				}
			}

			player[i].vid.bg0rom.text(vec(3, 13), "Touch to", BG0ROM_offcolor);
			player[i].vid.bg0rom.text(vec(2, 14), "continue...", BG0ROM_offcolor);
		}
	}

	{
		bool running;
		do {
			running = false;
			unsigned i;
			CubeSet cubes = CubeSet::connected();
			while (cubes.clearFirst(i)) {
				if (CubeID(i).isTouching()) {
					running = true;
					break;
				}
			}
			System::paint();
		} while (running);
	}

	{
		CubeSet ok;
		do {
			System::paint();
			
			unsigned i;
			CubeSet cubes = CubeSet::connected();

			while (cubes.clearFirst(i)) {
				if (!ok.test(i) && CubeID(i).isTouching()) {
					ok.mark(i);
					player[i].vid.bg0rom.fill(vec(2, 13), vec(11, 2), BG0ROMDrawable::charTile(' ', BG0ROM_offcolor));
				}
			}
		} while (ok.count() != CubeSet::connected().count());
	}

	//detach vid
	{
		unsigned i;
		CubeSet cubes = CubeSet::connected();
		while (cubes.clearFirst(i)) {
			player[i].cid.detachVideoBuffer();
		}
	}

	//re-attach
	{
		char namebuffer[sizeof(player[0].name)];
		for (UID i = 0; i < count; i++) {
			player[i].cid = pos[i];
			player[i].vid.attach(player[i].cid);
			if (pos[i] != i) {
				memcpy8((uint8_t *)namebuffer, (uint8_t *)player[i].name, sizeof(namebuffer));
				memcpy8((uint8_t *)player[i].name, (uint8_t *)player[pos[i]].name, sizeof(namebuffer));
				memcpy8((uint8_t *)player[pos[i]].name, (uint8_t *)namebuffer, sizeof(namebuffer));
			}
		}
	}
}


//TODO no inline?
static inline UID getNextPlayer(bool reverse, UID player) {
	if (reverse) {
		return (player ? player : playerCount) - 1;
	} else {
		UID newplayer = player + 1;
		return (newplayer == playerCount ? 0 : newplayer);
	}
}

static inline bool isCardValid(CARDID thiscard) {
	uint8_t lastcolor_get = CARD_GETCOLOR(lastcard);//计算的color
	if (lastcolor_get < 4) {//normal
		ASSERT(lastcolor_get == lastcolor);
		if (CARD_ISWILD(thiscard) ||						/*this is wild*/
			CARD_GETCOLOR(thiscard) == lastcolor_get ||		/*same color*/
			CARD_GETID(lastcard) == CARD_GETID(thiscard)	/*same num*/
			) return true;
	} else {//wild->normal
		if (!CARD_ISWILD(thiscard) && CARD_GETCOLOR(thiscard) == lastcolor) return true;//same color
	}
	return false;
}

//wait for a user to play a card
//return true if the card is played and valid, false if she user shaked.
//TODO inline?
static /*inline */bool waitForCard(UID nowplayer, bool (*verify)(CARDID thiscard), CARDID *thiscard, CARDCOUNT *playcardpos, bool lock = false) {
	bool played = true;

	player[nowplayer].scroller.status = 1;//force update popup
	if (lock) {//lock movement
		player[nowplayer].scroller.locked = true;
	}

	do {
		if (shake.update() & TiltShakeRecognizer::Shake_Begin) {
			played = false;
			break;
		}

		animUpdateScroll(nowplayer, 0);
		System::paint();
	} while (!(
		player[nowplayer].cid.isTouching() &&		/*touching*/
		player[nowplayer].scroller.status == 0 &&	/*scroll stopped*/
		getPlayerCard(nowplayer, player[nowplayer].scroller.stop_count, thiscard, playcardpos) &&	/*havecard*/
		verify(*thiscard)							/*valid*/
	));

	player[nowplayer].scroller.locked = false;

	//TODO ?
	if (!lock) {
		player[nowplayer].vid.sprites[0].hide();
		player[nowplayer].vid.sprites[1].hide();
	}
	return played;
}

static inline uint8_t selectColor(UID nowplayer) {
	//init scroller
	Scroller scroller;
	scroller.init(4);

	//init graphic
	ASSERT(player[nowplayer].displaypart < 2);
	player[nowplayer].vid.bg1.setPanning(vec(-64, -64));
	player[nowplayer].vid.bg1.setMask(BG1Mask::filled(vec(0, 0), vec(8, 2)));
	player[nowplayer].vid.sprites[0].setImage(ArrowDownPic, 0);
	player[nowplayer].vid.sprites[0].move(56, 48);
	for (uint8_t i = 0; i < 8; i++) {
		player[nowplayer].vid.bg1.image(vec(i, (uint8_t)0), WildColorPic, (i >> 1));
		player[nowplayer].vid.bg1.image(vec(i, (uint8_t)1), WildColorPic, (i >> 1));
	}
	bool viewing = false;
	//loop
	do {
		if (shake.update() & TiltShakeRecognizer::Shake_Begin) {
			viewing = !viewing;
			if (viewing) {
				player[nowplayer].vid.sprites[0].hide();
				for (uint8_t i = 0; i < 5; i++) {
					animUpdateScroll(nowplayer, 1);
					System::paint();
				}
				changeWindow(nowplayer, 2);
			} else {
				changeWindow(nowplayer, 1);
				player[nowplayer].vid.sprites[0].setImage(ArrowDownPic, 0);
			}
		}
		if (viewing) {
			animUpdateScroll();
		} else {
			scroller.update(player[nowplayer].cid.accel().x);
			player[nowplayer].vid.bg1.setPanning(vec(-64 + ((int)scroller.position >> 1), -64));
			animUpdateScroll(nowplayer, 1);
		}
		System::paint();
	} while (viewing || !(
		player[nowplayer].cid.isTouching() &&
		scroller.status == 0
	));
	
	player[nowplayer].vid.bg1.setMask(BG1Mask::filled(vec(0, 0), vec(4, 6)));
	player[nowplayer].vid.bg1.image(vec(0, 0), BlankCard, 0);
	player[nowplayer].vid.sprites[0].hide();
	return scroller.stop_count;
}

static inline bool challengeQuery(UID nowplayer) {
	//init scroller
	Scroller scroller;
	scroller.init(2);

	//init graphic
	changeWindow(nowplayer, 1);
	player[nowplayer].vid.bg1.setPanning(vec(-64, -62));
	player[nowplayer].vid.bg1.setMask(BG1Mask::filled(vec(0, 0), vec(7, 1)));
	player[nowplayer].vid.sprites[0].setImage(ArrowDownPic, 0);
	player[nowplayer].vid.sprites[0].move(56, 60);
	player[nowplayer].vid.bg1.image(vec(0, 0), ChallengeSelectPic, 0);

	//loop
	do {
		scroller.update(player[nowplayer].cid.accel().x);
		player[nowplayer].vid.bg1.setPanning(vec((int)(scroller.position * ((float)60 / 64) + (.5 - 64)), -76));
		animUpdateScroll(nowplayer, 1);
		System::paint();
	} while (!(
		player[nowplayer].cid.isTouching() &&
		scroller.status == 0
		));

	player[nowplayer].vid.bg1.setMask(BG1Mask::filled(vec(0, 0), vec(4, 6)));
	player[nowplayer].vid.bg1.image(vec(0, 0), BlankCard, 0);
	player[nowplayer].vid.sprites[0].hide();
	return !scroller.stop_count;
}

//TODO inline?
static inline void playCard(UID user, CARDCOUNT pos) {
	player[user].scroller.init(player[user].scroller.maxcount - 1);
	animPlayCard(user, lastcard);
	setOneCard(-1, pos);
}

static void eventRepaintScreen(void *p, unsigned id) {
	player[id].vid.touch();
	if (player[id].displaypart == 1) {
		Int2 bg1pan = player[id].vid.bg1.getPanning();
		Int2 spr0pan = player[id].vid.sprites[0].position();
		Int2 spr1pan = player[id].vid.sprites[1].position();
		Int2 spr2pan = player[id].vid.sprites[2].position();
		System::finish();
		changeWindow(id, 2);
		player[id].vid.bg0.setPanning(vec((int)player[id].scroller.position % (18 * 8), 80));
		player[id].vid.bg1.setPanning(vec(0, -48));
		player[id].vid.sprites[0].move(0, 48);
		player[id].vid.sprites[1].move(0, 48);
		player[id].vid.sprites[2].move(0, 48);
		System::paint(); System::finish();
		changeWindow(id, 1);
		player[id].vid.bg1.setPanning(bg1pan);
		player[id].vid.sprites[0].move(spr0pan);
		player[id].vid.sprites[1].move(spr1pan);
		player[id].vid.sprites[2].move(spr2pan);
	} else if (player[id].displaypart == 2) {
		Int2 bg0pan = player[id].vid.bg0.getPanning();
		Int2 bg1pan = player[id].vid.bg1.getPanning();
		Int2 spr0pan = player[id].vid.sprites[0].position();
		Int2 spr1pan = player[id].vid.sprites[1].position();
		System::finish();
		changeWindow(id, 1);
		//if y=-64, it's in selectColor() - viewing. don't move bg1.
		if (bg1pan.y != -64) {
			player[id].vid.bg1.setPanning(vec(0, -88));
		}
		player[id].vid.sprites[0].move(0, 88);
		player[id].vid.sprites[1].move(0, 88);
		System::paint(); System::finish();
		changeWindow(id, 2);
		player[id].vid.bg0.setPanning(bg0pan);
		player[id].vid.bg1.setPanning(bg1pan);
		player[id].vid.sprites[0].move(spr0pan);
		player[id].vid.sprites[1].move(spr1pan);
	}
}

UID PlaySingleGame(void) {
	//TODO
	const UID startTurn = 0;
	
	//new
	Events::cubeRefresh.set(eventRepaintScreen);

	//init graphic
	for (UID i = 0; i < playerCount; i++) {
		player[i].vid.bg1.setMask(BG1Mask::filled(vec(0, 0), vec(4, 6)));
		player[i].vid.bg1.image(vec(0, 0), BlankCard, 0);
		paintDefBg(i);
		player[i].vid.bg0.image(vec(3, 3), CardPic, 54);
		player[i].scroller.init(0);
	}
	g_frameclock.next();
	System::paint();
	g_frameclock.next();

	resetStack();
	UID nowplayer = startTurn;
	bool reverse = false;

	animShowNowPlayer(-1, reverse); //reset lastuid

	//draw 7
	ASSERT(nowplayer < playerCount);
	for (CARDCOUNT i = 0; i < INIT_CARD_COUNT; i++) {
		do {
			animShowNowPlayer(nowplayer, reverse);
			animDrawCard(nowplayer, drawOne(nowplayer), true);
			nowplayer = getNextPlayer(false, nowplayer);
		} while (nowplayer != startTurn);
	}
	animShowNowPlayer(nowplayer, reverse);

	//discard 1
	do {
		lastcard = drawOne(-1);
		animDiscardCard(lastcard);
	} while (CARD_ISWILDDRAW4(lastcard));
	System::paint();
	bool played = true;
	bool finished = false;

	for (;;) {
		System::finish();
		if (played) {
			//after-play action
			if (CARD_ISWILD(lastcard)) {
				bool validplay = true;
				if (!finished) {
					//go through the card to check the if play is valid
					{
						CARDCOUNT i = 0;
						CARDID retid = 54;
						bool ret;
						do {
							ret = getPlayerCard(nowplayer, i, &retid);
							validplay = (CARD_GETCOLOR(retid) != lastcolor);
							i++;
						} while (ret && validplay);
					}

					shake.attach(player[nowplayer].cid);
					lastcolor = selectColor(nowplayer);
					player[nowplayer].cid.detachMotionBuffer();
					
					//popup
					for (UID i = 0; i < playerCount; i++) {
						changeWindow(i, 1);
						player[i].vid.bg0.image(vec(13, 3), BubblePic, 0);
						player[i].vid.bg0.image(vec(14, 4), WildColorPic, lastcolor);
					}
					System::paint(); System::finish();
				}

				if (CARD_ISWILDDRAW4(lastcard)) {
					UID nextplayer = getNextPlayer(reverse, nowplayer);
					animShowNowPlayer(nextplayer, reverse);
					bool queried = false;
					if (!finished) {
						queried = challengeQuery(nextplayer);
					}
					if (queried) {
						if (validplay) {
							//next player draw6
							animDrawN(6);
							for (CARDCOUNT i = 0; i < 6; i++) {
								animDrawCard(nextplayer, drawOne(nextplayer), true);
							}
						} else {
							//this player draw4
							animShowNowPlayer(nowplayer, reverse);
							animDrawN(4);
							for (CARDCOUNT i = 0; i < 4; i++) {
								animDrawCard(nowplayer, drawOne(nowplayer), true);
							}
							animShowNowPlayer(nextplayer, reverse);
						}
					} else {
						//next player draw4
						animDrawN(4);
						for (CARDCOUNT i = 0; i < 4; i++) {
							animDrawCard(nextplayer, drawOne(nextplayer), true);
						}
					}
					if (!finished) nowplayer = nextplayer;
				}
			} else { //not wild card
				switch (CARD_GETID(lastcard)) {
				case 0://skip
				{
					UID oldplayer = nowplayer;
					bool played;
					do {
						played = false;
						nowplayer = getNextPlayer(reverse, nowplayer);
						animShowNowPlayer(nowplayer, reverse);
						//true if user have skip
						bool canplay;
						//go through the card to find
						{
							CARDCOUNT i = 0;
							CARDID retid = 54;
							bool ret;
							do {
								ret = getPlayerCard(nowplayer, i, &retid);
								canplay = CARD_ISSKIP(retid);
								i++;
							} while (ret && !canplay);
						}
						
						if (canplay) {
							//query nowplayer
							CARDCOUNT playcardpos;
							System::paint(); System::finish();
							shake.attach(player[nowplayer].cid);
							bool ret = waitForCard(nowplayer, CARD_ISSKIP_FUN, &lastcard, &playcardpos);
							player[nowplayer].cid.detachMotionBuffer();
							//player[nowplayer].vid.sprites[0].hide();
							//player[nowplayer].vid.sprites[1].hide();
							if (ret) {
								playCard(nowplayer, playcardpos);
								LOG("Played\n");
								played = true;
							}
						}
					} while (played);
					if (finished) {
						nowplayer = oldplayer;
					} else {
						animSkip();
					}
					break;
				}
				case 1://+2
				{
					UID oldplayer = nowplayer;
					bool played = true;
					CARDCOUNT addcount = 0;
					do {
						played = false;
						nowplayer = getNextPlayer(reverse, nowplayer);
						animShowNowPlayer(nowplayer, reverse);
						addcount += 2;

						//true if user have skip
						bool canplay;
						//go through the card to find
						{
							CARDCOUNT i = 0;
							CARDID retid = 54;
							bool ret;
							do {
								ret = getPlayerCard(nowplayer, i, &retid);
								canplay = CARD_ISDRAW2(retid);
								i++;
							} while (ret && !canplay);
						}

						if (canplay) {
							//query nowplayer
							CARDCOUNT playcardpos;
							System::paint(); System::finish();
							shake.attach(player[nowplayer].cid);
							bool ret = waitForCard(nowplayer, CARD_ISDRAW2_FUN, &lastcard, &playcardpos);
							player[nowplayer].cid.detachMotionBuffer();
							if (ret) {
								playCard(nowplayer, playcardpos);
								LOG("Played\n");
								played = true;
							}
						}
					} while (played);
					
					//draw n
					animDrawN(addcount);
					for (CARDCOUNT i = 0; i < addcount; i++) {
						animDrawCard(nowplayer, drawOne(nowplayer), true);
					}

					System::paint();

					if (nowplayer == oldplayer) {
						//if the playing user received cards, the game won't finish
						finished = false;
					} else if (finished) {
						nowplayer = oldplayer;
					}
					
					break;
				}
				case 2://reverse
					if (!finished && playerCount > 2) {
						reverse = !reverse;
						animReverse();
					}
					break;
				}
				lastcolor = CARD_GETCOLOR(lastcard);
			}
			if (finished) break;
		}
		nowplayer = getNextPlayer(reverse, nowplayer);
		animShowNowPlayer(nowplayer, reverse);
		System::paint(); System::finish();
		animUpdateScroll(); //set popupPlayer = -1, make sure arrows show next

		//wait for play, check valibility
		{
			CARDID thiscard;
			CARDCOUNT playcardpos;
			played = true; //false if user shaked for a card
			uint8_t blinkcount; //for stack animation
			uint8_t delay = 0;
			
			System::finish();
			shake.attach(player[nowplayer].cid);
			for (;;) {
				played = waitForCard(nowplayer, isCardValid, &thiscard, &playcardpos);
				if (played) break;

				//else: getcard
				player[nowplayer].scroller.status = 1;
				animUpdateScroll(nowplayer, 0);//TODO no popup?
				player[nowplayer].vid.bg1.image(vec(0, 0), BlankCard, 0);

				//loop to prevent the calling of finish
				for (uint8_t i = 0; i < 10; i++) {
					animUpdateScroll(nowplayer, 1);
					System::paint();
					if (i == 7) {
						changeWindow(nowplayer, 1);
					}
				}

				player[nowplayer].vid.bg1.image(vec(0, 0), CardPic, 54);
				blinkcount = 0;
				do {
					blinkcount = (blinkcount + 1) & 0x1F;
					uint8_t state = (blinkcount & 0x1F) >> 2;
					if (state > 4) state = 8 - state;
					player[nowplayer].vid.bg1.setPanning(vec(-24 - state, -24 - state));
					animUpdateScroll(nowplayer, 1);
					System::paint();
				} while (!(
					(shake.update() & TiltShakeRecognizer::Shake_Begin) ||
					player[nowplayer].cid.isTouching()
				));
				if (player[nowplayer].cid.isTouching()) break;
				
				player[nowplayer].vid.bg1.image(vec(0, 0), BlankCard, 0);
				player[nowplayer].vid.bg1.setPanning(vec(-48, -8));
				for (uint8_t i = 0; i < 10; i++) {
					animUpdateScroll(nowplayer, 1);
					System::paint();
				}
			}

			if (!played) {
				//get and ask play
				thiscard = drawOne(nowplayer);
				animDrawCard(nowplayer, thiscard, false);
				System::finish();
				played = waitForCard(nowplayer, isCardValid, &thiscard, &playcardpos, true);

				if (!played) player[nowplayer].vid.bg1.image(vec(0, 0), BlankCard, 0);
			}

			player[nowplayer].cid.detachMotionBuffer();

			if (played) {//playcard
				bool erasewild = false;
				if (CARD_ISWILD(lastcard)) {
					erasewild = true;
				}

				lastcard = thiscard;
				playCard(nowplayer, playcardpos);

				if (erasewild) {
					//erase wild popup
					for (UID i = 0; i < playerCount; i++) {
						for (uint8_t x = 13; x < 16; x++) {
							for (uint8_t y = 3; y < 8; y++) {
								player[i].vid.bg0.image(vec(x, y), BackGroundPic, g_random.randrange(8));
							}
						}
					}
					System::paint();
				}

				switch (getPlayerCardCount(nowplayer)) {
				case 0:
					finished = true;
					break;
				case 1:
					//say UNO!
					animUNO();
				}
			}
		}
	}
	Events::cubeRefresh.unset();
	return nowplayer;
}

//i:cube
//p:the player to draw
void showUserScore(UID i, uint8_t pos, UID p, char *scoretext, int8_t y = -2) {
	if (y == -2) {
		y = pos + 3;
	} else {
		y += 3;
	}

	LOG("pos=%d,y=%d,p=%d\n", pos, y, p);
	BG0ROMDrawable::Palette color1 = i == p ? BG0ROM_offcolor : BG0ROM_oncolor;
	BG0ROMDrawable::Palette color2 = i == p ? BG0ROM_oncolor : BG0ROM_offcolor;
	player[i].vid.bg0rom.plot(vec<int>(0, y), BG0ROMDrawable::charTile((pos < 9 ? pos + '1' : '*'), color2));
	player[i].vid.bg0rom.text(vec<int>(1, y), player[p].name, color1);
	player[i].vid.bg0rom.fill(vec<int>(13, y), vec(3, 1), BG0ROMDrawable::charTile(' ', color2));
	unsigned len = strnlen(scoretext, 16);
	player[i].vid.bg0rom.text(vec<int>(16 - len, y), scoretext, color2);
}

void showScoreList(UID *usermap, int8_t start, int8_t end) {
	LOG("ShowList: %d - %d\n", start, end);
	ASSERT(end > start);
	typedef int8_t playerpos;
	for (UID i = 0; i < playerCount; i++) {
		for (playerpos j = start; j < end; j++) {
			char scoretext[5];
			scoretext[0] = '\0';
			_SYS_strlcat_int(scoretext, player[usermap[j]].score, sizeof(scoretext));
			showUserScore(i, (winmode ? playerCount - 1 - j : j), usermap[j], scoretext);
		}
	}
}

void EndSingle(UID winner) {
	const CARDCOUNT CARD_GET_COUNT = 5;
	typedef int8_t playerpos;

	//TODO: add score, show change

	//title
	for (UID i = 0; i < playerCount; i++) {
		player[i].vid.initMode(BG0_ROM);
		player[i].vid.bg0rom.erase(BG0ROMDrawable::charTile(' ', BG0ROM_offcolor));
		player[i].vid.bg0rom.text(vec(5, 1), "Scores", BG0ROM_offcolor);
	}

	UID playermap[PLAYER_MAX];//user sorted by score, high->low
	for (playerpos i = 0; i < playerCount; i++) playermap[i] = winmode ? playerCount - 1 - i : i;

	//sort
	for (playerpos i = 0; i < playerCount; i++) {
		SCORE maxscore = -1;
		playerpos maxplayerpos; //position in playermap[]
		for (playerpos j = i; j < playerCount; j++) {
			if (maxscore < player[playermap[j]].score) {
				maxscore = player[playermap[j]].score;
				maxplayerpos = j;
			}
		}
		UID temp = playermap[i];
		playermap[i] = playermap[maxplayerpos];
		playermap[maxplayerpos] = temp;
	}

	//show original score
	showScoreList(playermap, 0, playerCount);

	for (uint16_t i = 0; i < 200; i++) {
		System::paint();
	}

	//calc scoreadd
	SCORE scoreadd[PLAYER_MAX];
	memset16((uint16_t *)scoreadd, 0, sizeof(scoreadd) / 2);

	for (UID i = 0; i < playerCount; i++) {
		CARDCOUNT count = getPlayerCardCount(i);
		CARDID retid[CARD_GET_COUNT];

		for (CARDCOUNT start = 0; start < count; start += CARD_GET_COUNT) {
			getPlayerCards(i, start, CARD_GET_COUNT, retid);
			for (CARDCOUNT j = 0; j < MIN(count - start, CARD_GET_COUNT); j++) {
				scoreadd[i] += getCardScore(retid[j]);
				//TODO animation?
			}
		}
	}

	//show scoreadd
	for (UID i = 0; i < playerCount; i++) {
		for (playerpos j = 0; j < playerCount; j++) {
			char scoretext[6];
			scoretext[0] = '+';
			scoretext[1] = '\0';
			_SYS_strlcat_int(scoretext, scoreadd[playermap[j]], sizeof(scoretext));
			showUserScore(i, (winmode ? playerCount - 1 - j : j), playermap[j], scoretext);
		}
	}

	for (uint16_t i = 0; i < 200; i++) {
		System::paint();
	}

	//sort added score
	if (winmode) {
		//elimination: one gets scores for himself

		for (UID i = 0; i < playerCount; i++) {
			player[i].score += scoreadd[i];
		}

		showScoreList(playermap, 0, playerCount);
		System::paint(); System::finish();

		for (playerpos i = 0; i < playerCount; i++) {
			SCORE maxscore = -1;
			playerpos maxplayerpos; //position in playermap[]
			for (playerpos j = i; j < playerCount; j++) {
				if (maxscore < player[playermap[j]].score) {
					maxscore = player[playermap[j]].score;
					maxplayerpos = j;
				}
			}

			if (maxplayerpos > i) {
				//the maxplayer pops up
				
				//sort
				UID maxplayer = playermap[maxplayerpos];
				for (playerpos j = maxplayerpos; j > i; j--) {
					playermap[j] = playermap[j - 1];
				}
				playermap[i] = maxplayer;

				uint8_t pany = (playerCount - maxplayerpos) * 8 + 16;
				LOG_INT(pany);
				LOG_INT(maxplayer); LOG_INT(maxplayerpos); LOG_INT(i);
				
				//init
				{
					char scoretext[5];
					scoretext[0] = '\0';
					_SYS_strlcat_int(scoretext, player[maxplayer].score, sizeof(scoretext));
					for (UID n = 0; n < playerCount; n++) {

						showUserScore(n, playerCount - 1 - i, maxplayer, scoretext, playerCount - i);

						player[n].vid.setWindow(pany, (maxplayerpos - i + 1) * 8);
						player[n].vid.bg0rom.setPanning(vec<int>(0, pany));
					}
				}

				//animation
				for (uint8_t t = 0; t < 64; t++) {
					int pany_1 = pany + (t >> 3);
					for (UID n = 0; n < playerCount; n++) {
						player[n].vid.bg0rom.setPanning(vec<int>(0, pany_1));
					}
					System::paint();
				}

				//end
				for (UID n = 0; n < playerCount; n++) {
					player[n].vid.setDefaultWindow();
					player[n].vid.bg0rom.setPanning(vec(0, 0));
					if (i == 0) player[n].vid.bg0rom.fill(vec(0, playerCount + 3), vec(16, 1), BG0ROMDrawable::charTile(' ', BG0ROM_offcolor));
				}
				showScoreList(playermap, i ? i - 1 : 0, maxplayerpos + 1);
				System::paint(); System::finish();
			}
		}
	} else {
		//classic: one gets scores for the winner

		playerpos newwinnerpos, oldwinnerpos;
		
		for (oldwinnerpos = 0; oldwinnerpos < playerCount; oldwinnerpos++) if (playermap[oldwinnerpos] == winner) break;
		ASSERT(oldwinnerpos < playerCount);

		for (UID i = 0; i < playerCount; i++) {
			player[winner].score += scoreadd[i];
		}

		showScoreList(playermap, 0, playerCount);

		//find player whose score < winner, then that position is winner's new position.
		for (newwinnerpos = 0; newwinnerpos < playerCount; newwinnerpos++) {
			if (player[winner].score > player[playermap[newwinnerpos]].score) break;
		}

		if (newwinnerpos < oldwinnerpos) {
			for (playerpos i = oldwinnerpos; i > newwinnerpos; i--) {
				playermap[i] = playermap[i - 1];
			}
			playermap[newwinnerpos] = winner;
			
			System::paint(); System::finish();

			uint8_t pany = newwinnerpos * 8 + 24;

			//init
			char scoretext[5];
			scoretext[0] = '\0';
			_SYS_strlcat_int(scoretext, player[winner].score, sizeof(scoretext));
			for (UID i = 0; i < playerCount; i++) {
				showUserScore(i, newwinnerpos, winner, scoretext, newwinnerpos - 1);
				player[i].vid.setWindow(pany, (oldwinnerpos - newwinnerpos + 1) * 8);
				player[i].vid.bg0rom.setPanning(vec<int>(0, pany));
			}
			
			//animation
			for (uint8_t t = 0; t < 64; t++) {
				int pany_1 = pany - (t >> 3);
				for (UID n = 0; n < playerCount; n++) {
					player[n].vid.bg0rom.setPanning(vec<int>(0, pany_1));
				}
				System::paint();
			}

			//end
			for (UID n = 0; n < playerCount; n++) {
				player[n].vid.setDefaultWindow();
				player[n].vid.bg0rom.setPanning(vec(0, 0));
				if (newwinnerpos == 0) player[n].vid.bg0rom.fill(vec(0, 2), vec(16, 1), BG0ROMDrawable::charTile(' ', BG0ROM_offcolor));
			}
			showScoreList(playermap, newwinnerpos ? newwinnerpos - 1 : 0, oldwinnerpos + 1);
		}
	}

	for (uint16_t i = 0; i < 200; i++) {
		System::paint();
	}

	for (UID i = 0; i < playerCount; i++) {
		player[i].vid.initMode(BG0_SPR_BG1);
	}
}

void EndAll(void) {
}
