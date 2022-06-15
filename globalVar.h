#pragma once

#include "structList.h"
extern int PLAYERS_NUM; //遊戲人數
extern int DEAD_NUM; //遊戲人數
extern int SHERIFF_NUM, DEPUTIES_NUM, OUTLAWS_NUM, RENEGADE_NUM; //身分人數

extern int SHERIFF_POSITION; //警長位置
extern int DISTANCE[7][7]; //相對距離表 distance[i][j]: i 看 j
extern GMode GAME_STATE; //遊戲狀態
extern Card CARD[80]; //消耗牌+裝備牌
extern Player *PLAYERS_LIST; //玩家狀態紀錄
extern Player *DEAD_LIST; //死亡狀態紀錄

extern Card_vector* deck;
extern Card_vector* discardPile;

extern Skill skills[16];

/* 身分 
* -警長, -副警長, -歹徒, -叛徒*/
extern char *identityName[];

/* 角色*/
extern char *roleName[];

/* 消耗牌+裝備牌*/
extern char *cardKindName[];

// 花色
extern char *suitName[];

// Color
extern char *Color[];