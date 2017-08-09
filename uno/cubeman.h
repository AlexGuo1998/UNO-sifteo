#pragma once

#include "typedef.h"
//#include "assets.gen.h"
//extern UID mastercube;//used to show menu

//extern AssetSlot MainSlot;
//extern AssetLoader g_loader;

void Bootstrap(void);

//return false if no need to load
inline bool startLoad(UID id) {
	
	extern PLAYERMASK assetsLoadUID;
	extern bool bs2installed;
	PLAYERMASK thismask = (PLAYERMASK)(0x80000000 >> id);
	//check if we need to load
	{
		bool needtoload = false;
		for (uint8_t i = 0; i < g_loaderconfig.count(); i++) {
			if (!g_loaderconfig[i].group()->isInstalled(thismask)) needtoload = true;
		}
		if (!needtoload) return false;
	}

	assetsLoadUID |= thismask;
	bs2installed = false;

	if (g_loader.isComplete()) {
		//start loading
		g_loader.start(g_loaderconfig);
	}
	return true;
}

//return false if no need to load
inline bool startLoad(PLAYERMASK mask) {
	CubeSet cubes;
	cubes.setMask(mask);
	unsigned i;
	bool ret = false;
	while (cubes.clearFirst(i)) {
		ret = startLoad((UID)i) || ret;
	}
	return ret;
}

//return false if load finished
bool loadingCycle(bool displayall);

inline bool isLoadFinish(void) {
	extern PLAYERMASK assetsLoadUID;
	extern bool bs2installed;
	if (g_loader.isComplete()) {
		bool assetsLoading = false;
		//check all
		ASSERT(assetsLoadUID);
		for (uint8_t i = 0; i < g_loaderconfig.count(); i++) {
			if (!g_loaderconfig[i].group()->isInstalled(assetsLoadUID)) assetsLoading = true;
		}
		if (assetsLoading) {
			bs2installed = false;
			g_loader.start(g_loaderconfig);
		} else {
			assetsLoadUID = 0;
			return true;
		}
	}
	return false;
}

void eventCubeConnect(void *pv, unsigned id);
void eventCubeLost(void *pv, unsigned id);
void eventLoadFinish(void *pv, unsigned id);
