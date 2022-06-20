#pragma once

// #define DEBUG

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define number_of_players(x) x >= 4 && x <= 7
#define SKILL_RANGE(x) ( x >= 0 && x < 16 )

#define GRAY "\033[30m"
#define RED   "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW   "\033[33m"
#define BLUE   "\033[34m"
#define MAG   "\033[35m"
#define CYN   "\033[36m"
#define WHT   "\033[37m"
#define GRAY_BACK "\033[1;100m"
#define RED_BACK   "\033[41m"
#define RESET "\033[0m"
#define ENTER printf("\n%s%sPress Enter to Continue%s", "\033[100m", GRAY, RESET); while( getchar() != '\n' )

#ifdef DEBUG
    #define ASSERT(x) assert(x)
#else
    #define ASSERT(x) assert(true)
#endif




/* 角色狀態
* -設定, -自身回合, -他人回合, -死亡*/
typedef enum _role_mode
{
    SET,
    JUDGE,
    GET_CARD,
    AFTER_GET,
    PLAY_CARD,
    DISCARD_CARD,
    MINUS_HP,
    FIGHT,
    IS_ATTACKED,
    FINISH_TIHS_TURN,
    IS_DEAD
} RMode; 

/* 遊戲狀態
* -尚未開始, -正在遊戲, -結束*/
typedef enum _game_mode
{
    NOT_YET_START,
    IN_ROUND,
    END
} GMode; 

/* 身分 
* -警長, -副警長, -歹徒, -叛徒*/
typedef enum _identity_kind
{
    Sheriff, 
    Deputies,
    Outlaws,
    Renegade
} Identity;

/* 角色*/
typedef enum _role_kind
{
    Bart_Cassidy, Black_Jack, Calamity_Janet, El_Gringo,
    Jesse_Jones, Jourdonnais, Kit_Carlson, Lucky_Duke,
    Paul_Regret, Pedro_Ramirez, Rose_Doolan, Sid_Ketchum,
    Slab_the_Killer, Suzy_Lafayette, Vulture_Sam, Willy_the_Kid
} Role;


/* 消耗牌+裝備牌*/
typedef enum _card_kind        
{
    BANG, MISSED, GATLING, INDIANS, PANIC, 
    CAT, STAGECOACH, WELLS, STORE, BEER, 
    SALOOW, DUEL, BARREL, SCOPE, MUSTANG, 
    JAIL, DYNAMITE, VOLCANIC, SCHOFIELD, 
    REMINGTON, REV, WINCHEDTER, NONE
} Kind;


typedef struct _card Card;
struct _card
{    
    int kind;
    int sticker;
    bool is_orange; //消耗 or 裝備
    int suit; //(0~3)
    int number; //(1~13)
    int attribute; // 主動(1) or 被動(2)
    // 這張牌是否可以主動被出出來
};

typedef struct {
    int size;
    int allocate_size;
    Card *data;
    int id;
} Card_vector;

typedef struct _player_list
{
    int id;
    int hp;
    int hp_limit;    
    // Card_vector* equipment;
    int attack_distance;
    int attack_power;
    // 武器
    Kind equipWeapon;
    Card_vector* weapon;
    // 防具
    Kind equipShield;
    Card_vector* shield;
    // 距離道具
    Kind equipScope;
    Kind equipMustang;
    Card_vector* distance_item;

    Card_vector* judgeCards;
    Card_vector* handcard;
    
    char    name[1000];    
    Role    role;
    RMode   state;    
    Identity identity;
    
    int numOfBang;
    int bangLimit;

    bool isAI;
} Player;



typedef void (*Skill) ( void *this );
typedef bool (*OrangeCard) ( Player *this );
typedef bool (*BlueCard) ( Player *player, int pos );