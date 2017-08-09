#ifdef __VSFIX
#include "vsfix.h"
#endif

#include "global.h"

#include "menulogic.h"

#include "assets.gen.h"

#include "gamelogic.h"

#include "menu-fork/menu.h"



static const MenuAssets assets = {&MenuBG, &MenuFooter, &MenuHeader, {NULL}, NULL};


//0=newgame 1=resume 2=del
uint8_t GameMenuNewgame(bool canresume) {
	//TODO add help
	MenuItem itemnoresume[2] = {
		{&MenuNewGame, 0, &MenuNewGame_lbl},
		{NULL, 0, NULL}
	};
	MenuItem itemresume[4] = {
		{&MenuResumeSaved, 0, &MenuResumeSaved_lbl},
		{&MenuDeleteSaved, 0, &MenuDeleteSaved_lbl},
		{&MenuNewGame, 0, &MenuNewGame_lbl},
		{NULL, 0, NULL}
	};
	Menu m(player[g_mastercube].vid, &assets, canresume ? itemresume : itemnoresume);
	m.anchor(0, true);
	struct MenuEvent e;
	uint8_t item;

	while (m.pollEvent(&e)) {
		if (!(playerOn & (0x80000000 >> g_mastercube))) {
			//cube lost
			CubeSet cubes;
			cubes.setMask(playerOn);
			unsigned i;
			cubes.findFirst(i);
			g_mastercube = i;
			return GameMenuNewgame(canresume);
		}

		switch (e.type) {

		case MENU_ITEM_PRESS:

			/*
			// Game Buddy is not clickable, so don't do anything on press
			if (e.item >= 3) {
				// Prevent the default action
				continue;
			} else {
				m.anchor(e.item);
			}
			*/
			break;

		case MENU_EXIT:
			// this is not possible when pollEvent is used as the condition to the while loop.
			// NOTE: this event should never have its default handler skipped.
			ASSERT(false);
			break;

		case MENU_NEIGHBOR_ADD:
			LOG("found cube %d on side %d of menu (neighbor's %d side)\n",
				e.neighbor.neighbor, e.neighbor.masterSide, e.neighbor.neighborSide);
			break;

		case MENU_NEIGHBOR_REMOVE:
			LOG("lost cube %d on side %d of menu (neighbor's %d side)\n",
				e.neighbor.neighbor, e.neighbor.masterSide, e.neighbor.neighborSide);
			break;

		case MENU_ITEM_ARRIVE:
			LOG("arriving at menu item %d\n", e.item);
			item = e.item;
			break;

		case MENU_ITEM_DEPART:
			LOG("departing from menu item %d, scrolling %s\n", item, e.direction > 0 ? "forward" : "backward");
			break;

		case MENU_PREPAINT:
			// do your implementation-specific drawing here
			// NOTE: this event should never have its default handler skipped.
			break;

		case MENU_UNEVENTFUL:
			// this should never happen. if it does, it can/should be ignored.
			ASSERT(false);
			break;
		}

		m.performDefault();
	}

	ASSERT(e.type == MENU_EXIT);

	//LOG("Selected Game: %d\n", e.item);
	
	if (canresume) {
		return (e.item + 1) % 3;
	} else {
		return 0;
	}
}

uint8_t GameMenuSettings() {
	uint8_t playercount_disp = playerCount - 2;

	MenuItem items[5] = {
		{&MenuPlayerCount, playercount_disp, &MenuPlayerCount_lbl},
		{&MenuWinMode, winmode, &MenuWinMode_lbl},
		{&MenuWinScore, winscore, &MenuWinScore_lbl},
		{&MenuOK, 1, &MenuOK_lbl},
		{NULL, 0, NULL}
	};
	Menu m(player[g_mastercube].vid, &assets, items);
	m.anchor(0, false);
	struct MenuEvent e;
	uint8_t item;

	bool cubefit = true;//false if cubehave < need

	while (m.pollEvent(&e)) {
		if (!(playerOn & (0x80000000 >> g_mastercube))) {
			//cube lost
			CubeSet cubes;
			cubes.setMask(playerOn);
			unsigned i;
			cubes.findFirst(i);
			g_mastercube = i;
			return GameMenuSettings();
		}
		if ((playerCount >= playercount_disp + 2) ^ cubefit) {
			cubefit = !cubefit;
			m.replaceIcon(3, &MenuOK, &MenuOK_lbl, (unsigned)cubefit);
		}

		switch (e.type) {

		case MENU_ITEM_PRESS:

			if (e.item == 0) {
				playercount_disp = (playercount_disp + 1) % 11;
				m.replaceIcon(0, &MenuPlayerCount, &MenuPlayerCount_lbl, playercount_disp);
				continue;
			} else if (e.item == 1) {
				winmode = !winmode;
				m.replaceIcon(1, &MenuWinMode, &MenuWinMode_lbl, winmode);
				continue;
			} else if (e.item == 2) {
				winscore = (winscore + 1) % 19;
				m.replaceIcon(2, &MenuWinScore, &MenuWinScore_lbl, winscore);
				continue;
			} else {
				if (cubefit) {
					System::setCubeRange(playercount_disp + 2);
				} else {
					continue;
				}
			}
			
			break;

		case MENU_EXIT:
			// this is not possible when pollEvent is used as the condition to the while loop.
			// NOTE: this event should never have its default handler skipped.
			ASSERT(false);
			break;

		case MENU_NEIGHBOR_ADD:
			LOG("found cube %d on side %d of menu (neighbor's %d side)\n",
				e.neighbor.neighbor, e.neighbor.masterSide, e.neighbor.neighborSide);
			break;

		case MENU_NEIGHBOR_REMOVE:
			LOG("lost cube %d on side %d of menu (neighbor's %d side)\n",
				e.neighbor.neighbor, e.neighbor.masterSide, e.neighbor.neighborSide);
			break;

		case MENU_ITEM_ARRIVE:
			LOG("arriving at menu item %d\n", e.item);
			item = e.item;
			break;

		case MENU_ITEM_DEPART:
			LOG("departing from menu item %d, scrolling %s\n", item, e.direction > 0 ? "forward" : "backward");
			break;

		case MENU_PREPAINT:
			// do your implementation-specific drawing here
			// NOTE: this event should never have its default handler skipped.
			break;

		case MENU_UNEVENTFUL:
			// this should never happen. if it does, it can/should be ignored.
			ASSERT(false);
			break;
		}

		m.performDefault();
	}

	ASSERT(e.type == MENU_EXIT);

	//LOG("Selected Game: %d\n", e.item);
	//System::setCubeRange(playercount_disp + 2);

	//loop until all cubes are connected/disconnected
	while (playerCount != playercount_disp + 2) {
		System::paint();
	}
	g_gamestate |= 2;
	
	LOG_INT(playerCount); LOG_INT(CubeSet::connected().count()); LOG_INT(playercount_disp + 2);

	//TODO save settings
	return 0;
}

