#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "structList.h"
#include "globalVar.h"
#include "vector.h"

// void HPModify( Player* attacker, Player *defender, int n, int reason );
void IsGameOver( Player *killer, Player *player ); // set winner part 還沒加

// 選擇攻擊對象
// if limitDistance == -1 then 沒有距離限制
// Player *choosePlayer( Player *attacker, int limitDistance );
// bool chooseCard( Card_vector* cards, Kind kind );

// ----------------------------------------------------
// orange card

// bool Bang( Player *attacker );
void Gatling( Player *attacker );

// 需要n張Miss來抵銷Bang的攻擊
// bool Miss( Player *defender, int n );

// void Indians( Player *attacker ); 
// void Indians_respond( Player *defender );

void panic( Player *attacker );

void cat( Player *attacker );
void cat_respond( Player *defender );

// void Stagecoach( Player *player );
// void Wells( Player *player );

// void Store( Player *player );
// void Store_choose( Player *player, Card *open );

// void Beer( Player *player );

// void Saloow( Player *player );

// void Duel( Player *attacker );
// bool Duel_end( Player *player );

// ----------------------------------------------------
// blue card
/*
Equip weapon 裝備武器
unload weapon 卸下武器
*/

// bool find_position(int id,int *position);

// bool EquipScope( Player *player );
// bool UnloadScope( Player *player );

// bool EquipMustang( Player *player );
// bool UnloadMustang( Player *player );

// bool EquipVolcanic( Player *player );
// bool UnloadVolcanic( Player *player );

bool EquipBarrel( Player *player );
bool UnloadBarrel( Player *player );

bool EquipJail( Player *player );
bool UnloadJail( Player *player, Card *card );
/*
  卸下Jail時要進行判定
  card為指定判定的牌，若card == NULL，則在function中抽一張
*/

// bool EquipDynamite( Player *player );
// bool UnloadDynamite( Player *player );

// bool EquipSchofield( Player *player );
// bool UnloadSchofield( Player *player );

// bool EquipRemington( Player *player );
// bool UnloadRemington( Player *player );

// bool EquipRev( Player *player );
// bool UnloadRev( Player *player );

// bool EquipWinchester( Player *player );
// bool UnloadWinchester( Player *player );

// bool UnloadWeapon( Player* player, Card_vector *cards );

/*

Player *choosePlayer( Player *attacker, int limitDistance );
bool chooseCard( Player *player, Card_vector* cards, int kind, Card_vector* get_card );

// orange card
bool Bang( Player *attacker );
bool Miss( Player *defender, int n );
bool Indians( Player *attacker );
bool Indians_respond( Player *attacker, Player *defender );
bool Stagecoach( Player *player );
bool Wells( Player *player );
bool Store( Player *player );
bool Beer( Player *player, Player *attacker );
bool Saloow( Player *player );
bool Duel( Player *attacker );
bool find_position( int id,int *position );
bool EquipScope( Player *player, int card_position );
bool UnloadScope( Player *player, Card_vector *cards );
bool EquipMustang( Player *player, int card_position );
bool UnloadMustang( Player *player, Card_vector *cards );

bool UnloadWeapon( Player* player, Card_vector *cards );

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
*/
