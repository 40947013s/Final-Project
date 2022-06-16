#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "structList.h"
#include "globalVar.h"
#include "vector.h"
#include "utilityFunc.h"
#include "UI.h"
#include "defineList.h"
#include "skillFunc.h"

void HPModify( Player* attacker, Player *defender, int n, Kind reason );
void IsGameOver( Player *killer, Player *player );

Player *choosePlayer( Player *attacker, int limitDistance );
Card chooseCard( Player *player, Card_vector* cards, int kind, Card_vector* get_card, bool except, bool visible );

// orange card
bool Bang( Player *attacker );
bool Miss( Player *defender, int n );
bool Indians( Player *attacker );
bool Stagecoach( Player *player );
bool Wells( Player *player );
bool Store( Player *player );
bool Beer( Player *player, Player *attacker );
bool Saloow( Player *player );
bool Duel( Player *attacker );
bool panic( Player *attacker );
bool cat( Player *attacker ); // new
bool Gatling( Player *attacker ) ; // new, still have to test

// blue card
bool EquipScope( Player *player, int card_position );
bool UnloadScope( Player *player, Card_vector *cards );
bool EquipMustang( Player *player, int card_position );
bool UnloadMustang( Player *player, Card_vector *cards );

bool UnloadWeapon( Player* player, Card_vector *cards );
bool UnloadEquip( Player* player, int kind ); // new

bool EquipVolcanic( Player *player, int card_position );
bool UnloadVolcanic( Player *player, Card_vector *cards );

bool EquipSchofield( Player *player, int card_position );
bool UnloadSchofield( Player *player, Card_vector *cards );
bool EquipRemington( Player *player, int card_position );
bool UnloadRemington( Player *player, Card_vector *cards );
bool EquipRev( Player *player, int card_position );
bool UnloadRev( Player *player, Card_vector *cards );
bool EquipWinchester( Player *player, int card_position );
bool UnloadWinchester( Player *player, Card_vector *cards );
bool EquipDynamite( Player *player, int index );
bool EquipJail( Player *player , int index ); // new
bool EquipBarrel( Player *player , int index ); // new
bool JudgeBarrel( Player *player ); // new
bool UnloadBarrel( Player *player ); // new

bool nullFunc2( Player *player );
bool nullFunc3( Player *player, int pos );