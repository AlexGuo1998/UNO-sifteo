#ifdef __VSFIX
#include "vsfix.h"
#endif // __VSFIX

#include "global.h"

#include "stack.h"

Card stack[108];
CARDCOUNT stackNext;

CARDCOUNT gcStack(void) {
	CARDCOUNT count = 0;
	CARDCOUNT firstused, firstfree = 0;//used moveto-> free
	//find firstfree. any card < firstfree is unmoved.
	while (firstfree < 108 && stack[firstfree].player ^ -1) {
		firstfree++;
	}
	if (firstfree == 108) return 108;//no card free

	firstused = firstfree + 1;
	for (;;) {
		while (firstused < 108 && stack[firstused].player == -1) firstused++;
		if (firstused >= 108) break;
		//else: found
		//swap
		{
			Card buffer = stack[firstused];
			stack[firstused] = stack[firstfree];
			stack[firstfree] = buffer;
		}
		firstfree++; firstused++;
	}
	return firstfree;
}

void suffleStack(CARDCOUNT start) {
	//when start = 107, there are only 1 card.
	while (start < 107) {
		//pick a card from start to 108
		CARDCOUNT n = g_random.randrange<CARDCOUNT>(start, 108);
		//swap
		{
			CARDID buffer = stack[start].id;
			stack[start].id = stack[n].id;
			stack[n].id = buffer;
		}
		start++;
	}
}

CARDCOUNT getPlayerCards(UID player, CARDCOUNT startcount, CARDCOUNT getcount, CARDID *retcardid) {
	//go through the list
	CARDCOUNT i = 0;
	for (;;) {
		//no more cards found
		if (startcount <= -getcount || i >= stackNext) return (startcount > 0 ? 0 : -startcount);
		//found 1 card
		if (stack[i].player == player) {
			//LOG("HIT sc=%d\n", startcount);
			if (startcount <= 0) {
				retcardid[-startcount] = stack[i].id;
			}
			startcount--;
		}
		i++;
	}
}

void movePlayer(UID oldplayer, UID newplayer) {
	for (CARDCOUNT i = 0; i < 108; i++) {
		if (stack[i].player == oldplayer) {
			stack[i].player == newplayer;
		}
	}
}