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
bool Bang( Player *attacker ); // yes
bool Miss( Player *defender, int n );
bool Indians( Player *attacker ); // yes
bool Stagecoach( Player *player ); // yes
bool Wells( Player *player ); // yes
bool Store( Player *player ); // yes
bool Beer( Player *player, Player *attacker ); 
bool Saloow( Player *player ); // yes
bool Duel( Player *attacker ); // yes
bool panic( Player *attacker ); // yes
bool cat( Player *attacker ); // yes
bool Gatling( Player *attacker ) ; // yes

// blue card
bool EquipScope( Player *player, int card_position ); // yes
bool UnloadScope( Player *player, Card_vector *cards ); // yes
bool EquipMustang( Player *player, int card_position ); // yes
bool UnloadMustang( Player *player, Card_vector *cards ); //yes

bool UnloadWeapon( Player* player, Card_vector *cards ); 
bool UnloadEquip( Player* player, int kind ); 

bool EquipVolcanic( Player *player, int card_position ); // yes
bool UnloadVolcanic( Player *player, Card_vector *cards ); // yes

bool EquipSchofield( Player *player, int card_position ); // yes
bool UnloadSchofield( Player *player, Card_vector *cards ); // yes
bool EquipRemington( Player *player, int card_position );
bool UnloadRemington( Player *player, Card_vector *cards );
bool EquipRev( Player *player, int card_position ); // yes
bool UnloadRev( Player *player, Card_vector *cards ); // yes
bool EquipWinchester( Player *player, int card_position ); // yes
bool UnloadWinchester( Player *player, Card_vector *cards ); // yes
bool EquipDynamite( Player *player, int index );
bool EquipJail( Player *player , int index ); // yes
bool EquipBarrel( Player *player , int index ); // yes
// bool JudgeBarrel( Player *player ); // error
bool UnloadBarrel( Player *player );

bool nullFunc2( Player *player );
bool nullFunc3( Player *player, int pos );