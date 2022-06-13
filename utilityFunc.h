#pragma once
#include "structList.h"
#include "globalVar.h"
#include "vector.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void print_player(int th);
int cardHandler( Player * player, int num );
bool discardCard( Player * player, int index );
bool printCard( Card card );
void clean_buffer(char *arr);
int scan(int min, int max, char *str);
void shuffle();
bool takeCard( Player *p1, Player *p2, int index );