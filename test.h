#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "structList.h"
#include "globalVar.h"
#include "vector.h"
#include "utilityFunc.h"
#include "UI.h"
#include "defineList.h"

void HPModify( Player* attacker, Player *defender, int n, Kind reason );
void IsGameOver( Player *killer, Player *player );

// 選擇攻擊對象
// if limitDistance == -1 then 沒有距離限制
Player *choosePlayer( Player *attacker, int limitDistance );
bool chooseCard( Player *player, Card_vector* cards, Kind kind );

// ----------------------------------------------------
// orange card

bool Bang( Player *attacker );
bool Miss( Player *defender, int n );