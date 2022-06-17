#pragma once

#include "structList.h"
#include "defineList.h"


extern int PLAYERS_NUM; //遊戲人數
extern int ALIVE_NUM; //存活遊戲人數
extern int SHERIFF_NUM, DEPUTIES_NUM, OUTLAWS_NUM, RENEGADE_NUM; //身分人數

extern int SHERIFF_POSITION; //警長位置
extern int OFFSET_DISTANCE[10][10];
extern int DISTANCE[10][10]; //相對距離表 distance[i][j]: i 看 j
extern GMode GAME_STATE; //遊戲狀態
extern Card CARD[80]; //消耗牌+裝備牌
extern Player *PLAYERS_LIST; //玩家狀態紀錄

extern Card_vector* deck;
extern Card_vector* discardPile;

extern Skill skills[16];
extern OrangeCard orangeCards[12];
extern BlueCard blueCards[23];

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

// state
extern char *stateName[];

