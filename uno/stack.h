#pragma once
//对牌堆的操作，包括初始化，洗牌，抽牌，得到一个玩家拥有的牌，出牌

#include "typedef.h"

#define CARD_GETCOLOR(card)		((card) / 13)
#define CARD_GETID(card)		((card) % 13)

#define CARD_ISWILD(card)		((card) >= 52)
#define CARD_ISWILDDRAW4(card)	((card) >= 53)

#define CARD_ISSKIP(card)		(!CARD_ISWILD(card) && (CARD_GETID(card) == 0))
#define CARD_ISDRAW2(card)		(!CARD_ISWILD(card) && (CARD_GETID(card) == 1))

inline bool CARD_ISSKIP_FUN(CARDID cardid) {
	return CARD_ISSKIP(cardid);
}

inline bool CARD_ISDRAW2_FUN(CARDID cardid) {
	return CARD_ISDRAW2(cardid);
}

//garbage cleaning(?)
//push used cards to start of the stack
//return used card count
CARDCOUNT gcStack(void);

//suffle manually
void suffleStack(CARDCOUNT start);

//get n cards belong to one user
CARDCOUNT getPlayerCards(UID player, CARDCOUNT startcount, CARDCOUNT getcount, CARDID *retcardid);

//move one player to another
//TODO used when reload cubes
//INLINE?
void movePlayer(UID oldplayer, UID newplayer);

inline void resetStack(void) {
	extern Card stack[108];
	extern CARDCOUNT stackNext;

	//init stack
	for (CARDCOUNT i = 0; i < 100; i++) {
		stack[i].id = (i % 25) / 2 + (i / 25) * 13;
		stack[i].player = -1;
	}
	for (CARDCOUNT i = 100; i < 104; i++) {
		stack[i].id = 52;
		stack[i].player = -1;
	}
	for (CARDCOUNT i = 104; i < 108; i++) {
		stack[i].id = 53;
		stack[i].player = -1;
	}
	//suffile
	suffleStack(0);
	stackNext = 0;
#ifdef _DEBUG
	stack[1].id = 52; //for quick-finish
#endif // _DEBUG
}

//get card count belong to one user
inline CARDCOUNT getPlayerCardCount(UID player) {
	extern Card stack[108];
	extern CARDCOUNT stackNext;
	//go through the list
	CARDCOUNT i = 0, getcount = 0;
	for (CARDCOUNT i = 0; i < stackNext; i++) {
		if (stack[i].player == player) getcount++;
	}
	return getcount;
}

//get 1 card belong to one user
inline bool getPlayerCard(UID player, CARDCOUNT startcount, CARDID *retcardid, CARDCOUNT *retcardpos = NULL) {
	extern Card stack[108];
	
	extern CARDCOUNT stackNext;
	//go through the list
	CARDCOUNT i = 0;
	do {
		//found 1 card
		if (stack[i].player == player) {
			//LOG("HIT sc=%d\n", startcount);
			if (startcount > 0) {
				startcount--;
			} else {
				*retcardid = stack[i].id;
				if (retcardpos) *retcardpos = i;
				return true;
			}
		}
		i++;
	} while (i < stackNext);
	return false;
}

//suffle automatically
inline void gcAndSuffleStack(void) {
	extern CARDCOUNT stackNext;
	//todo show a animation
	stackNext = gcStack();
	suffleStack(stackNext);
}

//auto gc
inline CARDID drawOne(UID uid) {
	extern Card stack[108];
	extern CARDCOUNT stackNext;
	CARDID getid = stack[stackNext].id;
	stack[stackNext].player = uid;
	stackNext++;
	if (stackNext >= 108) gcAndSuffleStack();
	return getid;
}

inline void setOneCard(UID newuid, CARDCOUNT cardpos) {
	extern Card stack[108];
	stack[cardpos].player = newuid;//discard
}

inline SCORE getCardScore(CARDID cardid) {
	if (CARD_ISWILD(cardid)) {
		return 50;
	} else {
		uint8_t id = CARD_GETID(cardid);
		if (id < 3) {
			return 20;
		} else {
			return id - 3;
		}
	}
}
