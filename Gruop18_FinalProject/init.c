#include "init.h"
#include <unistd.h>
#include <getopt.h>

int PLAYERS_NUM, DEAD_NUM; //遊戲人數
int SHERIFF_NUM, DEPUTIES_NUM, OUTLAWS_NUM, RENEGADE_NUM; //身分人數
int SHERIFF_POSITION;
int DISTANCE[10][10]; //相對距離表 distance[i][j]: i 看 j
int OFFSET_DISTANCE[10][10];
GMode GAME_STATE; //遊戲狀態
Card CARD[80]; //消耗牌+裝備牌
Player *PLAYERS_LIST; //玩家狀態紀錄
Player *DEAD_LIST; //死亡玩家狀態紀錄
int ALIVE_NUM;

Card_vector* deck;
Card_vector* discardPile;

//Skill
Skill skills[16] = {
    fBart_Cassidy, fBlack_Jack, fCalamity_Janet, nullFunc,
    fJesse_Jones, fJourdonnais, fKit_Carlson, nullFunc,
    fPaul_Regret, fPedro_Ramirez, fRose_Doolan, fSid_Ketchum,
    fSlab_the_Killer, fSuzy_Lafayette, nullFunc, fWilly_the_Kid
};


// Orange Card
OrangeCard orangeCards[12] = { Bang, nullFunc2, Gatling, Indians, panic,
                                cat, Stagecoach, Wells, Store, nullFunc2, Saloow, Duel};

// Blue Card
BlueCard blueCards[23] = { nullFunc3, nullFunc3, nullFunc3, nullFunc3, nullFunc3,
                            nullFunc3, nullFunc3, nullFunc3, nullFunc3, nullFunc3,
                            nullFunc3, nullFunc3, EquipBarrel,
                            EquipScope, EquipMustang, EquipJail, 
                            EquipDynamite, EquipVolcanic, EquipSchofield, EquipRemington,
                            EquipRev, EquipWinchester, nullFunc3 };


// 身分 
char *identityName[] = { "Sheriff", "Deputies", "Outlaws", "Renegade" };

// 角色
char *roleName[] = {
    "Bart_Cassidy", "Black_Jack", "Calamity_Janet", "El_Gringo", 
    "Jesse_Jones", "Jourdonnais", "Kit_Carlson", "Lucky_Duke",
    "Paul_Regret", "Pedro_Ramirez", "Rose_Doolan", "Sid_Ketchum",
    "Slab_the_Killer", "Suzy_Lafayette", "Vulture_Sam", "Willy_the_Kid"
};

// 消耗牌+裝備牌
char *cardKindName[] = {
    "BANG", "MISSED", "GATLING", "INDIANS", "PANIC", 
    "CAT", "STAGECOACH", "WELLS", "STORE", "BEER", 
    "SALLON", "DUEL", "BARREL", "SCOPE", "MUSTANG", 
    "JAIL", "DYNAMITE", "VOLCANIC", "SCHOFIELD", 
    "REMINGTON", "REV", "WINCHEDTER"
};

// 花色
char *suitName[] = { "none", "spade", "heart", "diamond", "club" };

// Color
char *Color[11] = { RESET, RED, YELLOW, GREEN, BLUE, MAG, CYN, WHT, GRAY, GRAY_BACK, RED_BACK };

// State
char *stateName[12] = { "SET", "JUDGE", "GET_CARD", "AFTER_GET", "PLAY_CARD", "DISCARD_CARD", "MINUS_HP", "FIGHT", "ATTACKED", "FINISH", "DEAD"};

void init_player(Player *player)
{
    player->hp = 0;
    player->hp_limit = 0;
    player->handcard = create_vector(10);
    player->handcard->id = player->id;
    
    player->judgeCards = create_vector(5);
    player->judgeCards->id = player->id;
  
    player->attack_distance = 1;
    player->attack_power = 1;
    
    player->equipWeapon = NONE;
    player->weapon = create_vector(5);
    player->weapon->id = player->id;
    
    player->equipShield = NONE;    
    player->shield = create_vector(5);
    player->shield->id = player->id;
    
    player->equipScope = NONE;
    player->equipMustang = NONE;
    player->distance_item = create_vector(5);
    player->distance_item->id = player->id;
    
    player->role = El_Gringo;
    player->state = SET;
    player->identity = Sheriff;
    player->numOfBang = 0;
    player->bangLimit = 1;
}

int identity_shuffle() {
    int res = 0;
    srand(time(NULL));
    for(int i = 0; i < PLAYERS_NUM; i++) {
        int j = rand() % PLAYERS_NUM;
        Identity tmp = PLAYERS_LIST[i].identity;
        PLAYERS_LIST[i].identity = PLAYERS_LIST[j].identity;
        PLAYERS_LIST[j].identity = tmp;

        if( PLAYERS_LIST[j].identity == Sheriff ) res = j;
        if( PLAYERS_LIST[i].identity == Sheriff ) res = i;
    }
    return res;
}

void role_shuffle() {
    srand(time(NULL));
    bool is_take[16] = {0};
    for(int i = 0; i < PLAYERS_NUM; i++) {
        system( "clear" );
        Role j = rand() % 16, res;
        while(is_take[j]) {
            j = (j+1) % 16;
        }
        is_take[j] = true;
        int k = rand() % 16;
        while(is_take[k]) {
            k = (k+1) % 16;
        }
        is_take[k] = true;
      
        // print role j
        char str[1000] = {0};
        strcpy( str, "grep " );
        strcat( str, roleName[j] );
        strcat( str, " role_skill.txt | awk '{print $2}' > j.txt" );
        
        system( str );
        FILE *pFile = pFile = fopen( "j.txt", "r" );
        assert( pFile != NULL );
        fgets( str, 1000, pFile );
        printf( "%sRole1: %s%s\n\n", BLUE, roleName[j], RESET );
        printf( "%sSkill detail: %s\n%s\n", MAG, RESET, str );

        // print role k
        strcpy( str, "grep " );
        strcat( str, roleName[k] );
        strcat( str, " role_skill.txt | awk '{print $2}' > k.txt" );
        
        system( str );
        pFile = fopen( "k.txt", "r" );
        assert( pFile != NULL );
        fgets( str, 1000, pFile );
        printf( "%sRole2: %s%s\n\n", BLUE, roleName[k], RESET );
        printf( "%sSkill detail: %s\n%s\n", MAG, RESET, str );

        remove( "j.txt" );
        remove( "k.txt" );

      
        puts( "Which role card to choose (0 or 1): " );
        printf( "[0] %s, [1] %s\n", roleName[j], roleName[k]);
        int choice = scan(0, 1, "--> ");
        if(choice == 0) {
            PLAYERS_LIST[i].role = j, res = j;
        } else {
            PLAYERS_LIST[i].role = k, res = k;
        }
        printf( "%sYour role: %s%s\n\n", YELLOW, roleName[res], RESET );
        ENTER;
        PLAYERS_LIST[i].hp = 4, PLAYERS_LIST[i].hp_limit = 4;
        if( res == El_Gringo || res == Paul_Regret ) {
            PLAYERS_LIST[i].hp --, PLAYERS_LIST[i].hp_limit --;
        }
    }
    

// typedef enum _role_kind
// {
//     Bart_Cassidy, Black_Jack, Calamity_Janet, El_Gringo,
//     Jesse_Jones, Jourdonnais, Kit_Carlson, Lucky_Duke,
//     Paul_Regret, Pedro_Ramirez, Rose_Doolan, Sid_Ketchum,
//     Slab_the_Killer, Suzy_Lafayette, Vulture_Sam, Willy_the_Kid
// } Role;


  #ifdef DEBUG 
    

    // PLAYERS_LIST[0].role = Lucky_Duke;
    // PLAYERS_LIST[2].role = Pedro_Ramirez;
    // PLAYERS_LIST[2].role = Rose_Doolan;
    // PLAYERS_LIST[3].role = Sid_Ketchum;
    // PLAYERS_LIST[4].role = Slab_the_Killer;
    // PLAYERS_LIST[5].role = Suzy_Lafayette;
    // PLAYERS_LIST[6].role = Willy_the_Kid;

    // Card c; 
    // c.kind = DYNAMITE; 
    // c.sticker = DYNAMITE; 
    // c.number = 2; 
    // c.suit = 2; 
    // c.attribute = 1; 
    // c.is_orange = false; 

    // push_back( PLAYERS_LIST[0].judgeCards, c ); 

    // c.kind = JAIL; 
    // c.sticker = JAIL; 
    // c.number = 2; 
    // c.suit = 2; 
    // c.attribute = 1; 
    // c.is_orange = false; 
    // push_back( PLAYERS_LIST[0].judgeCards, c ); 

    // push_back( PLAYERS_LIST[1].handcard, c ); 
    // push_back( PLAYERS_LIST[2].handcard, c ); 
    // push_back( PLAYERS_LIST[3].handcard, c ); 
    // push_back( PLAYERS_LIST[4].handcard, c ); 
    // push_back( PLAYERS_LIST[5].handcard, c ); 
    // push_back( PLAYERS_LIST[6].handcard, c ); 

    
  #endif
}


void init_card()
{    
    FILE *fp;
    fp = fopen("table.txt", "r");
    char *line = malloc(1000);
    fgets(line,1000,fp);
    for(int i = 0; i < 80; i++)
    {
        int a, b, c, d, e, f;
        fscanf(fp, "%d%d%d%d%d%d", &a, &b, &c, &d, &e, &f);
        CARD[i].is_orange = d;
        CARD[i].kind = f;
        CARD[i].sticker = f;
        CARD[i].suit = b;
        CARD[i].number = c;
        CARD[i].attribute = e;
    } 
    fclose(fp);
    free(line);
}

