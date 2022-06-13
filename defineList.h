#pragma once

#define IS_BANG(x) x >= 0 && x <= 24
#define IS_MISSED(x) x >= 25 && x <= 36
#define IS_GATLING(x) x == 37
#define IS_INDIANS(x) x >= 38 && x <= 39
#define IS_PANIC(x) x >= 40 && x <= 43
#define IS_CAT(x) x >= 44 && x <= 47
#define IS_STAGECOACH(x) x >= 48 && x <= 49
#define IS_WELLS(x) x == 50 
#define IS_STORE(x) x >= 51 && x <= 52
#define IS_BEER(x) x >= 53 && x <= 58
#define IS_SALOOW(x) x == 59
#define IS_DUEL(x) x >= 60 && x <= 62
#define IS_BARREL(x) x>=63 && x<=64
#define IS_SCOPE(x) x==65
#define IS_MUSTANG(x) x>=66 && x<=67
#define IS_JAIL(x) x>=68 && x<=70
#define IS_DYNAMITE(x) x == 71
#define IS_VOLCANIC(x) x>=72 && x<=73
#define IS_SCHOFIELD(x) x>=74 && x<=76
#define IS_REMINGTON(x) x == 77
#define IS_REV(x) x == 78
#define IS_WINCHEDTER(x) x == 79

#define number_of_players(x) x >= 4 && x <= 7