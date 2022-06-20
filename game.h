#pragma once

#include <unistd.h>
#include "init.h"

void game_prepare();
void HPModify( Player* attacker, Player *defender, int n, Kind reason );
void playerCard( Player *player );
void judgeTurn( Player *player, Player *nextPlayer );
void discardTurn( Player *player );
void IsGameOver( Player *killer, Player *player );