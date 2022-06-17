#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structList.h"
#include "vector.h"
#include "globalVar.h"
#include "defineList.h"

void printHandCard( Card_vector *cards, int color[], bool visible );
int setColor( int **c, int index, int kind, int attribute, Card_vector* handcard, int color ) ;
void printUI( Player *nowPlayer );
void print_player(int th);
void printCard( Card card, char *color );