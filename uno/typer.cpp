#ifdef __VSFIX
#include "vsfix.h"
#endif

#include "global.h"

#include "typer.h"

Typer *typer = NULL;

#define dx 5
#define dy 10
#define overlapx 6
#define overlapy 3
#define offsetx (int)(-5.5 * dx)
#define offsety (int)(0.5 * dy)
#define startx 2
#define starty 6
#define texty 4

static const char keys[2][4][13] = {
	{"1234567890-=", "QWERTYUIOP\\\x9D", "\222ASDFGHJKL\x9D\x99", "\x9EZXCVBNM ,./"},
	{"!@#$%^&*()_+", "QWERTYUIOP|\x9D", "\222ASDFGHJKL\x9D\x99", "\x9EZXCVBNM :'?"}
};

// \x01 shift \x02 capslck \x03 bs \x04 ok
static const char keymask[2][4][13] = {
	{"1234567890-=", "qwertyuiop\\\x03", "\002asdfghjkl\x04\x04", "\x01zxcvbnm ,./"},
	{"!@#$%^&*()_+", "QWERTYUIOP|\x03", "\002ASDFGHJKL\x04\x04", "\x01ZXCVBNM :'?"}
};

static const char OKCancelLbl[2][7] = {"  OK  ", "Cancel"};
static const char blank[] = "             ";

static const BG0ROMDrawable::Palette oncolor = BG0ROMDrawable::LTBLUE_ON_DKBLUE, offcolor = BG0ROMDrawable::WHITE_ON_GREEN;

static inline void changeShift(UID id) {
	typer[id].shift ^= 1;
	typerRepaint(id);
}

static inline void changeCaps(UID id) {
	typer[id].capslock ^= 1;
	typerRepaint(id);
}

static inline void clearBoard(UID id) {
	for (uint8_t i = 0; i < 4; i++) { 
		player[id].vid.bg0rom.text(vec(startx - 1, starty + i), &blank[0], offcolor);
	}
}

static inline void printOKCancel(UID id) {
	player[id].vid.bg0rom.text(vec(startx, starty + 2), OKCancelLbl[0], typer[id].state & 0x04 ? offcolor : oncolor);
	player[id].vid.bg0rom.text(vec(startx + 6, starty + 2), OKCancelLbl[1], typer[id].state & 0x04 ? oncolor : offcolor);
}

static inline void onMove(UID id) {
	Byte3 acc = player[id].cid.accel();
	if (typer[id].state == 1) {
		int8_t movex, movey;
		int8_t newx, newy;
		movex = acc.x - (typer[id].nowx * dx + offsetx);
		movey = acc.y - (typer[id].nowy * dy + offsety);

		if (movex >= dx + overlapx) {
			newx = typer[id].nowx + (movex - overlapx) / dx;
			if (newx > 11) newx = 11;
		} else if (-movex >= dx + overlapx) {
			newx = typer[id].nowx - (-movex - overlapx) / dx;
			if (newx < 0) newx = 0;
		} else {
			newx = typer[id].nowx;
		}

		if (movey >= dy + overlapy) {
			newy = typer[id].nowy + (movey - overlapy) / dy;
			if (newy > 3) newy = 3;
		} else if (-movey >= dy + overlapy) {
			newy = typer[id].nowy - (-movey - overlapy) / dy;
			if (newy < 0) newy = 0;
		} else {
			newy = typer[id].nowy;
		}

		if (typer[id].nowx != newx || typer[id].nowy != newy) {
			player[id].vid.bg0rom.plot(
				vec(typer[id].nowx + startx, typer[id].nowy + starty),
				BG0ROMDrawable::charTile(keys[typer[id].shift ^ typer[id].capslock][typer[id].nowy][typer[id].nowx], offcolor)
			);
			player[id].vid.bg0rom.plot(
				vec(newx + startx, newy + starty),
				BG0ROMDrawable::charTile(keys[typer[id].shift ^ typer[id].capslock][newy][newx], oncolor)
			);
			typer[id].nowx = newx; typer[id].nowy = newy;
		}
	} else if (typer[id].state & 0x04) {//r
		if (acc.x < -(int)(dx / 2 + overlapx)) {
			typer[id].state &= 0x03;
			printOKCancel(id);
		}
	} else {//l
		if (acc.x >(int)(dx / 2 + overlapx)) {
			typer[id].state |= 0x04;
			printOKCancel(id);
		}
	}
}

static inline void onTouch(UID id) {
	if (typer[id].state == 1) {
		char c = keymask[typer[id].shift ^ typer[id].capslock][typer[id].nowy][typer[id].nowx];
		if (c > 4) {//normal
			uint8_t i = 0;
			while (player[id].name[i]) i++;
			if (i >= 12) return;
			player[id].name[i] = c; player[id].name[i + 1] = 0;
			player[id].vid.bg0rom.plot(vec(i + 2, texty), BG0ROMDrawable::charTile(c, offcolor));
			if (typer[id].shift) {
				changeShift(id);
			}
		} else {//ctrl
			if (c == '\x01') {
				changeShift(id);
			} else if (c == '\x02') {
				changeCaps(id);
			} else if (c == '\x03') {
				uint8_t i = 0;
				while (player[id].name[i]) i++;
				if (i == 0) return;
				player[id].name[i - 1] = 0;
				player[id].vid.bg0rom.plot(vec((i + 1), texty), BG0ROMDrawable::charTile(' ', offcolor));
			} else {// 4
				typer[id].state = 6; //confirm
				clearBoard(id);
				player[id].vid.bg0rom.text(vec(startx + 2, starty), "Is it OK?", offcolor);
				printOKCancel(id);
			}
		}
	} else if (typer[id].state & 0x04) {//r
		//back to typer
		typer[id].state = 1;
		typerRepaint(id);
	} else {//l
		typer[id].state = 0;
		clearBoard(id);
		player[id].vid.bg0rom.text(vec(0, starty), "Wait while", offcolor);
		player[id].vid.bg0rom.text(vec(0, starty + 1), "others typing...", offcolor);
	}
}

static void onMoveMaster(void *p, unsigned id) {
	if (typer[id].state) onMove(id);
}

static void onTouchMaster(void *p, unsigned id) {
	if (player[id].cid.isTouching() && typer[id].state) onTouch(id);
}

void typeName(void) {
	Typer typerlist[12];
	typer = typerlist;

	//init
	for (uint8_t i = 0; i < 12; i++) {
		player[i].vid.initMode(BG0_ROM);
		player[i].vid.bg0rom.erase(BG0ROMDrawable::charTile(' ', offcolor));
		player[i].vid.bg0rom.text(vec(2, 1), "Please input", offcolor);
		player[i].vid.bg0rom.text(vec(3, 2), "your name:", offcolor);
		typerRepaint(i);
		typerlist[i].state = 1;
	}

	Events::cubeTouch.set(onTouchMaster);
	Events::cubeAccelChange.set(onMoveMaster);

	//wait
	bool running;
	do {
		System::paint();
		if (playerCount != playerCount_set) {
			running = true;
		} else {
			running = false;
			CubeSet cubes;
			cubes.setMask(playerOn);
			unsigned i;
			while (cubes.clearFirst(i)) {
				if (typer[i].state) {
					running = true;
					break;
				}
			}
		}
	} while (running);
	
	//end
	Events::cubeTouch.unset();
	Events::cubeAccelChange.unset();
	for (uint8_t i = 0; i < 12; i++) {
		player[i].vid.initMode(BG0_SPR_BG1);
	}
	typer = NULL;
}

void typerRepaint(UID id) {
	for (uint8_t i = 0; i < 4; i++) {
		player[id].vid.bg0rom.text(vec(startx, starty + i), keys[typer[id].shift ^ typer[id].capslock][i], offcolor);
	}
	player[id].vid.bg0rom.plot(
		vec(typer[id].nowx + startx, typer[id].nowy + starty),
		BG0ROMDrawable::charTile(keys[typer[id].shift ^ typer[id].capslock][typer[id].nowy][typer[id].nowx], oncolor)
	);
	player[id].vid.bg0rom.text(vec(2, texty), player[id].name, offcolor);
	player[id].vid.bg0rom.plot(vec(startx - 1, starty + 2), BG0ROMDrawable::charTile(typer[id].capslock ? '\x8E' : ' ', offcolor));
	player[id].vid.bg0rom.plot(vec(startx - 1, starty + 3), BG0ROMDrawable::charTile(typer[id].shift ? '\x8E' : ' ', offcolor));
}

void typerClear(UID id) {
	memset((uint8_t *)player[id].name, 0, sizeof(player[id].name));
	player[id].vid.bg0rom.text(vec(2, texty), &blank[1], offcolor);
	typer[id].state = 1;
	typer[id].shift = 0;
	typer[id].capslock = 0;
	typer[id].nowx = 5;
	typer[id].nowy = 0;
	typerRepaint(id);
}
