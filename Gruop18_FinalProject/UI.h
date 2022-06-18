#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structList.h"
#include "vector.h"
#include "globalVar.h"
#include "utilityFunc.h"

void printHandCard( Card_vector *cards, int color[], bool visible );
int setColor( int **c, int index, int kind, int attribute, Card_vector* handcard, int color ) ;
void printUI( Player *nowPlayer, char *msg );
void print_player(int th);
void printCard( Card card, char *color );
int setPlayerColor( int **c, int limit_distance, Player *player, int index, int color, bool is_default );
void printUI2( Player *nowPlayer, int *color, char *msg );
void gameStartUI();
void gameEndUI();