#ifdef __VSFIX
#include "vsfix.h"
#endif

#include "global.h"

#include "cubeman.h"

#include "animation.h"


bool bs2installed = false;
PLAYERMASK assetsLoadUID = 0;


void eventCubeConnect(void *pv, unsigned id) {
	player[id].vid.initMode(BG0_SPR_BG1); 
	player[id].vid.attach(id);

	LOG("Cube connect: #%d\n", id);
	
	if (g_gamestate & (1 | 8)) {//displaying loading screen
		paintDefBg(id);
	}
	startLoad((UID)id);

	if (g_gamestate & 2) {
		if (g_gamestate & 4) {
			//user is binded
			//TODO change uid
		} else {
			//user is not binded
		}
	} else {
		playerCount++;
		playerOn |= (PLAYERMASK)(0x80000000 >> id);
	}
}

void eventCubeLost(void *pv, unsigned id) {
	LOG("Cube lost: #%d\n", id);
	if (g_gamestate & 2) {
		if (g_gamestate & 4) {
			//user is binded
			//TODO change uid
		} else {
			//user is not binded
		}
	} else {
		playerCount--;
		playerOn &= ~(PLAYERMASK)(0x80000000 >> id);
	}
}

void eventLoadFinish(void *pv, unsigned id) {
	LOG("Load finish: #%d\n", id);
	if (g_gamestate & 8 && id != g_mastercube) {
		LOG("Paint\n");
		player[id].vid.bg0.image(vec(3, 5), GameLabelPic, 0);
		player[id].vid.bg1.setMask(BG1Mask::filled(vec(0, 0), vec(8, 1)));
		player[id].vid.bg1.setPanning(vec(-32, -96));
		player[id].vid.bg1.image(vec(0, 0), AlexSoftLabel, 0);
	}
}


inline void drawbar(PLAYERMASK ids, uint8_t p) {
	uint8_t a, b;
	a = p >> 3;
	b = p & 0x07;
	CubeSet cubes;
	cubes.setMask(assetsLoadUID);
	unsigned i;
	while (cubes.clearFirst(i)) {
		for (uint8_t j = 0; j < a; j++) {
			player[i].vid.bg0.image(vec(j, (uint8_t)14), LoadingBarPic, 8);
		}
		player[i].vid.bg0.image(vec(a, (uint8_t)14), LoadingBarPic, b);
		for (uint8_t j = a + 1; j < 16; j++) {
			player[i].vid.bg0.image(vec(j, (uint8_t)14), LoadingBarPic, 0);
		}
	}
}

bool loadingCycle(bool displayall) {
	static uint8_t counter;
	
	bool loading = !isLoadFinish();

	if (!bs2installed && ((++counter & 0xF) == 0 || !loading) && BootstrapGroup2.isInstalled(assetsLoadUID)) {
		CubeSet cubes;
		cubes.setMask(assetsLoadUID);
		unsigned i;
		while (cubes.clearFirst(i)) {
			player[i].vid.touch();
			player[i].vid.bg0.image(vec(3, 5), GameLabelPic, 0);
			player[i].vid.bg1.setMask(BG1Mask::filled(vec(0, 0), vec(8, 1)));
			player[i].vid.bg1.setPanning(vec(-32, -96));
			player[i].vid.bg1.image(vec(0, 0), AlexSoftLabel, 0);
		}
		bs2installed = true;
	}

	if (loading) {
		uint8_t state = g_loader.averageProgress(128);
		drawbar(displayall ? CubeSet::connected().mask() : assetsLoadUID, state);
	}
	return loading;
}


void Bootstrap(void) {
	//init
	for (uint8_t i = 0; i < 12; i++) {
		player[i].cid = i;
		player[i].vid.initMode(BG0_SPR_BG1);
	}

	//load bootstrap for connected cubes
	//when loading, new added cubes are ignored (are loaded later)

	CubeSet cubes = CubeSet::connected();
	playerOn = cubes.mask();
	unsigned i;
	while (cubes.clearFirst(i)) {
		playerCount++;
		player[i].vid.attach(i);
		paintDefBg(i);
	}


	g_loaderconfig.append(MainSlot, BootstrapGroup);
	g_loaderconfig.append(MainSlot, BootstrapGroup2);
	g_loaderconfig.append(MainSlot, MenuGroup);
	if (startLoad(playerOn)) {
		g_gamestate |= 1;
		while (loadingCycle(true)) {
			System::paint();
		}
		g_gamestate &= ~1;
		g_gamestate |= 8;
		{
			CubeSet cubes = CubeSet::connected();
			unsigned i;
			cubes.clearFirst(i);
			while (cubes.clearFirst(i)) {
				for (uint8_t x = 0; x < 16; x++) {
					player[i].vid.bg0.image(vec(x, (uint8_t)14), BackGroundPic, g_random.randrange(8));
				}
			}
		}
	} else {
		//loaded
		g_gamestate |= 8;
		CubeSet cubes = CubeSet::connected();
		unsigned i;
		cubes.clearFirst(i);
		while (cubes.clearFirst(i)) {
			eventLoadFinish(NULL, i);
		}
	}

	//g_gamestate = 2;
}