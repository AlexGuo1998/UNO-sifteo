#ifdef __VSFIX
#include "vsfix.h"
#endif

#include "global.h"

#include "typedef.h"
//#include <sifteo/macros.h>
#include "gamelogic.h"
#include "stack.h"
#include "animation.h"
//#include "states.h"
//#include "cubeman.h"




static CARDID lastcard;
static uint8_t lastcolor;
static class TiltShakeRecognizer shake;


void GameMenu(void) {
	//TODO

	//playerOn = 0x3;
}

void InputName(void) {
}

//TODO no inline?
inline UID getNextPlayer(bool reverse, UID player) {
	ASSERT(playerOn != 0);
	uint8_t pos = 0;//user position
	while (playermap[pos] != player) pos++;

	do {
		pos = (pos + (reverse ? 11 : 1)) % 12;
		player = playermap[pos];
	} while (!(playerOn & (PLAYERMASK)(0x80000000 >> player)));
	return player;
}

inline bool isCardValid(CARDID thiscard) {
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
/*inline */bool waitForCard(UID nowplayer, bool (*verify)(CARDID thiscard), CARDID *thiscard, CARDCOUNT *playcardpos, bool lock = false) {
	bool played = true;

	player[nowplayer].scroller.status = 1;//force update popup
	if (lock) {//lock movement
		player[nowplayer].scroller.locked = true;
	}

	do {
		//if (player[nowplayer].shake.update() & TiltShakeRecognizer::Shake_Begin) {
		if (shake.update() & TiltShakeRecognizer::Shake_Begin) {
			played = false;
			break;
		}

		animUpdateScroll(playerOn, nowplayer);
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

inline uint8_t selectColor(UID nowplayer) {
	uint8_t color = 0;
	
	//init scroller
	Scroller scroller;
	scroller.init(4);

	//init graphic
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
					animUpdateScroll(playerOn & ~(PLAYERMASK)(0x80000000 >> nowplayer));
					System::paint();
				}
				changeWindow(nowplayer, 2);
			} else {
				changeWindow(nowplayer, 1);
				player[nowplayer].vid.sprites[0].setImage(ArrowDownPic, 0);
			}
		}
		if (viewing) {
			animUpdateScroll(playerOn);
		} else {
			scroller.update(player[nowplayer].cid.accel().x);
			player[nowplayer].vid.bg1.setPanning(vec(-64 + ((int)scroller.position >> 1), -64));
			animUpdateScroll(playerOn & ~(PLAYERMASK)(0x80000000 >> nowplayer));
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

//TODO inline?
inline void playCard(UID user, CARDCOUNT pos) {
	player[user].scroller.init(player[user].scroller.maxcount - 1);
	animPlayCard(user, lastcard);
	setOneCard(-1, pos);
}

void PlaySingleGame(void) {
	//TODO
	const UID startTurn = 0;
	
	//new
	LOG_INT(playerOn);

	//init graphic
	for (uint8_t i = 0; i < 12; i++) {
		if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
			player[i].vid.bg1.setMask(BG1Mask::filled(vec(0, 0), vec(4, 6)));
			player[i].vid.bg1.image(vec(0, 0), BlankCard, 0);
			paintDefBg(i);
			player[i].vid.bg0.image(vec(3, 3), CardPic, 54);

		}
	}
	g_frameclock.next();
	System::paint();
	g_frameclock.next();

	resetStack();
	UID nowplayer = startTurn;
	bool reverse = false;

	//draw 7
	ASSERT(playerOn & (PLAYERMASK)(0x80000000 >> nowplayer));
	for (uint8_t i = 0; i < INIT_CARD_COUNT; i++) {
		do {
			animDrawCard(nowplayer, drawOne(nowplayer), true);
			nowplayer = getNextPlayer(reverse, nowplayer);
		} while (nowplayer != startTurn);
	}

	//discard 1
	do {
		lastcard = drawOne(-1);
		animDiscardCard(playerOn, lastcard);
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

					shake.attach(nowplayer);
					lastcolor = selectColor(nowplayer);
					player[nowplayer].cid.detachMotionBuffer();
					
					//popup
					for (uint8_t i = 0; i < 12; i++) {
						if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
							changeWindow(i, 1);
							player[i].vid.bg0.image(vec(13, 3), BubblePic, 0);
							player[i].vid.bg0.image(vec(14, 4), WildColorPic, lastcolor);
						}
					}
					System::paint(); System::finish();
				}

				if (CARD_ISWILDDRAW4(lastcard)) {
					UID nextplayer = getNextPlayer(reverse, nowplayer);
					bool queried = false;
					if (!finished) {
						//todo query
						queried = false;
					}
					if (queried) {
						if (validplay) {
							//next player draw6
							animDrawN(6);
							for (uint8_t i = 0; i < 6; i++) {
								animDrawCard(nextplayer, drawOne(nextplayer), true);
							}
						} else {
							//this player draw4
							animDrawN(4);
							for (uint8_t i = 0; i < 4; i++) {
								animDrawCard(nowplayer, drawOne(nowplayer), true);
							}
						}
					} else {
						//next player draw4
						animDrawN(4);
						for (uint8_t i = 0; i < 4; i++) {
							animDrawCard(nextplayer, drawOne(nextplayer), true);
						}
					}
					nowplayer = nextplayer;
				}
			} else { //not wild card
				switch (CARD_GETID(lastcard)) {
				case 0://skip
				{
					bool played;
					do {
						played = false;
						nowplayer = getNextPlayer(reverse, nowplayer);
						
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
							System::finish();
							shake.attach(nowplayer);
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
					animSkip();
					break;
				}
				case 1://+2
				{
					UID oldplayer = nowplayer;
					bool played = true;
					uint8_t addcount = 0;
					do {
						played = false;
						nowplayer = getNextPlayer(reverse, nowplayer);
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
							System::finish();
							shake.attach(nowplayer);
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
					for (uint8_t i = 0; i < addcount; i++) {
						animDrawCard(nowplayer, drawOne(nowplayer), true);
					}

					System::paint();

					if (nowplayer == oldplayer) {
						//if the playing user received cards, the game won't finish
						finished = false;
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
			if (finished) {
				//TODO finish
				break;
			}
		}
		System::finish();
		animUpdateScroll(playerOn); //set popupPlayer = -1, make sure arrows show next
		nowplayer = getNextPlayer(reverse, nowplayer);

		//wait for play, check valibility
		{
			CARDID thiscard;
			CARDCOUNT playcardpos;
			played = true; //false if user shaked for a card
			uint8_t blinkcount; //for stack animation
			uint8_t delay = 0;
			
			System::finish();
			shake.attach(nowplayer);
			for (;;) {
				played = waitForCard(nowplayer, isCardValid, &thiscard, &playcardpos);
				if (played) break;

				//else: getcard
				player[nowplayer].scroller.status = 1;
				animUpdateScroll(playerOn, nowplayer);
				player[nowplayer].vid.bg1.image(vec(0, 0), BlankCard, 0);

				//loop to prevent the calling of finish
				for (uint8_t i = 0; i < 10; i++) {
					animUpdateScroll(playerOn & ~(PLAYERMASK)(0x80000000 >> nowplayer));
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
					animUpdateScroll(playerOn & ~(PLAYERMASK)(0x80000000 >> nowplayer));
					System::paint();
				} while (!(
					//(player[nowplayer].shake.update() & TiltShakeRecognizer::Shake_Begin) || 
					(shake.update() & TiltShakeRecognizer::Shake_Begin) ||
					player[nowplayer].cid.isTouching()
				));
				if (player[nowplayer].cid.isTouching()) break;
				
				player[nowplayer].vid.bg1.image(vec(0, 0), BlankCard, 0);
				player[nowplayer].vid.bg1.setPanning(vec(-48, -8));
				for (uint8_t i = 0; i < 10; i++) {
					animUpdateScroll(playerOn & ~(PLAYERMASK)(0x80000000 >> nowplayer));
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
					for (uint8_t i = 0; i < 12; i++) {
						if (playerOn & (PLAYERMASK)(0x80000000 >> i)) {
							for (uint8_t x = 13; x < 16; x++) {
								for (uint8_t y = 3; y < 8; y++) {
									player[i].vid.bg0.image(vec(x, y), BackGroundPic, g_random.randrange(8));
								}
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
					;
					//TODO say UNO!
				}
			}
		}
		
	}
}

void EndSingle(void) {
}

void EndAll(void) {
}
