#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structList.h"
#include "vector.h"
#include "globalVar.h"
#include "defineList.h"

void printHandCard( Card_vector *cards, int color[] );
int setColor( int **c, int index, Kind kind, int attribute, Card_vector* handcard, int color ) ;
void printUI( Player *nowPlayer );