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
	.cubeRange(2, 12);





void main() {
	g_random = Random();
	g_loader.init();
	loadSettings();

	Events::cubeConnect.set(eventCubeConnect);
	Events::cubeDisconnect.set(eventCubeLost);
	Events::cubeAssetDone.set(eventLoadFinish);

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
	g_gamestate &= ~8;
	typeName();
	g_gamestate |= 4;
	PairLoop();

	for (uint8_t i = 0; i < 12; i++) {
		player[i].vid.initMode(BG0_SPR_BG1);
	}

	{
		bool loading = startLoad(CubeSet::connected().mask());
		if (loading && !isLoadFinish()) {
			paintDefBg(g_mastercube);
			player[g_mastercube].vid.bg0.image(vec(3, 5), GameLabelPic, 0);
			player[g_mastercube].vid.bg1.setMask(BG1Mask::filled(vec(0, 0), vec(8, 1)));
			player[g_mastercube].vid.bg1.setPanning(vec(-32, -96));
			player[g_mastercube].vid.bg1.image(vec(0, 0), AlexSoftLabel, 0);
			while (loadingCycle(true)) {
				System::paint();
			}
		}
	}

	{
		CubeSet cubes = CubeSet::connected();
		uint8_t count = cubes.count();
		unsigned i;
		while (cubes.clearFirst(i)) {
			player[i].cid.detachVideoBuffer();
		}
		//TODO
		for (uint8_t i = 0; i < count; i++) {
			player[i].cid = i;
			player[i].vid.attach(player[i].cid);
		}
	}

	PlaySingleGame();

	return;
}

