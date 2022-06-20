#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "structList.h"
#include "vector.h"
#include "skillFunc.h"
#include "utilityFunc.h"
#include "globalVar.h"
#include "UI.h"
#include "cardFunc.h"

void init_card();
void init_player(Player *player);
int identity_shuffle();
void role_shuffle();
void setRelationship();

