#ifdef __VSFIX
#include "vsfix.h"
#endif

#include "global.h"

#include <sifteo.h>
#include "assets.gen.h"
//#include "typer.h"
#include "typedef.h"
#include "gamelogic.h"

#include "menulogic.h"

#include "animation.h"

#include "cubeman.h"

static Metadata M = Metadata()
	.title("UNO!")
	.package("com.alexguo1998.uno", "0.1")
	.icon(Icon)
	.cubeRange(2, 12);





void main() {
	g_random = Random();
	g_loader.init();

	Events::cubeConnect.set(eventCubeConnect);
	Events::cubeDisconnect.set(eventCubeLost);
	Events::cubeAssetDone.set(eventLoadFinish);
	
	//set master cube
	{
		unsigned i;
		CubeSet::connected().findFirst(i);
		g_mastercube = i;
	}

	Bootstrap();

	g_loaderconfig.append(MainSlot, GameGroup);
	
	startLoad(CubeSet::connected().mask());

	GameMenuNewgame(false);//todo
	GameMenuSettings();
	g_gamestate &= ~8;
	{
		bool loading = startLoad(CubeSet::connected().mask());
		if (loading && !isLoadFinish()) {
			paintDefBg(g_mastercube);
			while (loadingCycle(true)) {
				System::paint();
			}
		}
	}

	PlaySingleGame();

	return;
}

