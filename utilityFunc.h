#pragma once
#include "structList.h"
#include "globalVar.h"
#include "skillFunc.h"
#include "vector.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


int cardHandler( Player * player, int num );
bool discardCard( Card_vector * cards, int index );
void clean_buffer(char *arr);
int scan(int min, int max, char *str);
void shuffle();
bool takeCard( Card_vector *p1, Card_vector *p2, int index );
void discardAllCard( Player *player );
void takeAllCards( Player *p1, Player *p2 );
int find_sticker( Card_vector *card, Kind kind );
void setPlayer( Player *p1, Player *p2 );
int find_position( int id );
int min(int a, int b);
void calcDistance() ;
Card getJudgementCard( Player *player, int kind );
bool judgeFunc( Player *player, int kind );