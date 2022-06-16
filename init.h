#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "defineList.h"
#include "structList.h"
#include "vector.h"
#include "skillFunc.h"
#include "utilityFunc.h"
#include "globalVar.h"
#include "UI.h"
#include "test.h"

void init_card();
void game_prepare();


// test case
/* test INDIANS Card
    Card c;
    c.kind = INDIANS;
    c.sticker = INDIANS;
    c.number = 13;
    c.suit = 3;
    c.attribute = 1;
    push_back( p->handcard, c );
    */
    /* test STAGECOACH Card
    Card c;
    c.kind = STAGECOACH;
    c.sticker = STAGECOACH;
    c.number = 9;
    c.suit = 1;
    c.attribute = 1;
    push_back( p->handcard, c );
    */
    /* test STORE Card
    Card c;
    c.kind = STORE;
    c.sticker = STORE;
    c.number = 9;
    c.suit = 1;
    c.attribute = 1;
    push_back( p->handcard, c );
    */
    /* test DUEL card
    Card c;
    c.kind = DUEL;
    c.sticker = DUEL;
    c.number = 9;
    c.suit = 1;
    c.attribute = 1;
    push_back( p->handcard, c );
    */
    /* test Scope card
    Card c;
    c.kind = SCOPE;
    c.sticker = SCOPE;
    c.number = 9;
    c.suit = 1;
    c.attribute = 1;
    push_back( p->handcard, c );
    push_back( p->handcard, c );
    */
    /* test Mustang card
    Card c;
    c.kind = MUSTANG;
    c.sticker = MUSTANG;
    c.number = 9;
    c.suit = 1;
    c.attribute = 1;
    push_back( p->handcard, c );
    push_back( p->handcard, c );
    */
    /* test distance item card
    Card c;
    c.kind = VOLCANIC;
    c.sticker = VOLCANIC;
    c.number = 9;
    c.suit = 1;
    c.attribute = 1;
    push_back( p->handcard, c );

    c.kind = SCHOFIELD;
    c.sticker = SCHOFIELD;
    push_back( p->handcard, c );

    c.kind = REMINGTON;
    c.sticker = REMINGTON;
    push_back( p->handcard, c );

    c.kind = REV;
    c.sticker = REV;
    push_back( p->handcard, c );

    c.kind = WINCHEDTER;
    c.sticker = WINCHEDTER;
    push_back( p->handcard, c );
    */