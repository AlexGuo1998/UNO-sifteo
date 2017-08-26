#ifdef __VSFIX
#include "vsfix.h"
#endif

#include "global.h"

#include <sifteo.h>
#include "assets.gen.h"
//#include "typer.h"
#include "gamelogic.h"
#include "menulogic.h"
#include "animation.h"
#include "cubeman.h"
#include "savedata.h"
#include "typer.h"

static Metadata M = Metadata()
	.title("UNO!")
	.package("com.alexguo1998.uno", "0.1")
	.icon(Icon)
	.cubeRange(2, PLAYER_MAX);





void main() {
	g_random = Random();
	g_loader.init();
	loadSettings();

	Events::cubeConnect.set(eventCubeConnect);
	Events::cubeDisconnect.set(eventCubeLost);
	Events::cubeAssetDone.set(eventLoadFinish);

	for (;;) {
		Bootstrap();

		//set master cube
		{
			unsigned i;
			CubeSet::connected().findFirst(i);
			g_mastercube = i;
		}

		g_loaderconfig.append(MainSlot, GameGroup);

		startLoad(CubeSet::connected().mask());

		GameMenuNewgame(false);//todo
		GameMenuSettings();
		saveSettings();
		g_gamestate = 2;

		//DEBUG: input name automatically in sim_debug
#ifdef _DEBUG
		if (System::isDebug() && System::isSimDebug()) {
			char name[13] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
			for (UID i = 0; i < PLAYER_MAX; i++) {
				player[i].vid.initMode(BG0_ROM);
				name[0] = 'A' + i;
				memcpy8((uint8_t *)player[i].name, (uint8_t *)name, sizeof(player[i].name));
			}
		} else {
			typeName();
		}
#else
		typeName();
#endif

		g_gamestate = 2 | 16;
		PairLoop();
		g_gamestate = 2 | 4;

		for (UID i = 0; i < PLAYER_MAX; i++) {
			player[i].vid.initMode(BG0_SPR_BG1);
		}

		{
			bool loading = startLoad(CubeSet::connected().mask());
			if (loading && !isLoadFinish()) {
				g_gamestate = 1 | 2 | 4;
				paintDefBg(g_mastercube);
				player[g_mastercube].vid.bg0.image(vec(3, 5), GameLabelPic, 0);
				player[g_mastercube].vid.bg1.setMask(BG1Mask::filled(vec(0, 0), vec(8, 1)));
				player[g_mastercube].vid.bg1.setPanning(vec(-32, -96));
				player[g_mastercube].vid.bg1.image(vec(0, 0), AlexSoftLabel, 0);
				while (loadingCycle(true)) {
					System::paint();
				}
				g_gamestate = 2 | 4;
			}
		}

		for (;;) {
			PlaySingleGame();
			EndSingle();
			//determine if the game is over
			{
				bool finished;
				int16_t winscore_true = winscore ? winscore * 50 + 50 : 0;
				if (winmode) {
					//TODO calc for elimination
				} else {
					//classic
					finished = false;
					for (UID i = 0; i < playerCount; i++) {
						if (player[i].score >= winscore_true) {
							finished = true;
							break;
						}
					}
				}
				finished = true; //TODO THIS IS FOR DEBUG ONLY
				if (finished) break;
			}
		}

		EndAll();
	}
}

