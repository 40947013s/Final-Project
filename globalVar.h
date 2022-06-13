#pragma once

#include "structList.h"
extern int PLAYERS_NUM; //遊戲人數
extern int SHERIFF_NUM, DEPUTIES_NUM, OUTLAWS_NUM, RENEGADE_NUM; //身分人數

extern int *SEAT_POSITION; //座位紀錄 (從0開始記錄至遊戲人數-1)
extern int DISTANCE[7][7]; //相對距離表 distance[i][j]: i 看 j
extern GMode GAME_STATE; //遊戲狀態
extern Card CARD[80]; //消耗牌+裝備牌
extern Player *PLAYERS_LIST; //玩家狀態紀錄

extern Card_vector* deck;
extern Card_vector* discardPile;
extern Skill skills[16];
