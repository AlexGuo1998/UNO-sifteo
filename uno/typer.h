#pragma once

#include "typedef.h"

struct Typer {
	int8_t state = 0; // 0 = over 1 = running 2/6 = ok?(L/R)
	uint8_t shift = 0, capslock = 0; //0 or 1
	int8_t nowx = 5, nowy = 0;
};

extern Typer *typer;

void typeName(void);

void typerRepaint(UID id);
void typerClear(UID id);
