#ifdef __VSFIX
#include "vsfix.h"
#endif

#include "global.h"

#include "savedata.h"

#include <sifteo/filesystem.h>

StoredObject so_settings = StoredObject(0);
StoredObject so_gamestate = StoredObject(1);

#pragma pack(push)
#pragma pack(1)

typedef struct {
	char name[13];
	uint16_t score;
	
} Player_save;

typedef struct {
	bool winmode;
	uint8_t winscore;//index only
	uint8_t playercount;
	Player_save user[12];
} Game_save;

typedef struct {
	bool winmode;
	uint8_t winscore;
} Settings_save;

#pragma pack(pop)

bool saveGamestate(void) {
	Game_save save;

	save.winmode = winmode;
	save.winscore = winscore;
	save.playercount = playerCount;

	for (UID i = 0; i < playerCount; i++) {
		memcpy8((uint8_t *)save.user[i].name, (uint8_t *)player[i].name, sizeof(player[i].name));
		save.user[i].score = player[i].score;
	}

	int ret = so_gamestate.write(&save, (sizeof(Game_save) - sizeof(Player_save) * 12) + playerCount * sizeof(Player_save));

	ASSERT(ret < 0 || ret == (sizeof(Game_save) - sizeof(Player_save) * 12) + playerCount * sizeof(Player_save));
	return (ret >= 0);
}

bool saveSettings(void) {
	Settings_save settings;
	settings.winmode = winmode;
	settings.winscore = winscore;

	int ret = so_settings.writeObject(settings);
	ASSERT(ret < 0 || ret == sizeof(Settings_save));
	return (ret >= 0);
}

bool loadSettings(void) {
	Settings_save settings;
	int ret = so_settings.readObject(settings);
	LOG_INT(ret);
	ASSERT(ret <= 0 || ret == sizeof(Settings_save));

	if (ret == sizeof(Settings_save)) {
		winmode = settings.winmode;
		winscore = settings.winscore;
	}
	return (ret == sizeof(Settings_save));
}
