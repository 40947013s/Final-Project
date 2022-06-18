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

//Skill function
// Skill skills[16] = {
//     fBart_Cassidy, fBlack_Jack, fCalamity_Janet, nullFunc,
//     fJesse_Jones, fJourdonnais, fKit_Carlson, nullFunc,
//     fPaul_Regret, fPedro_Ramirez, fRose_Doolan, fSid_Ketchum,
//     fSlab_the_Killer, fSuzy_Lafayette, nullFunc, fWilly_the_Kid
// };


// new: fBart_Cassidy, fSid_Ketchum, fSuzy_Lafayette, fCalamity_Janet
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
    "Paul_Regret", "Pedro Ramirez", "Rose_Doolan", "Sid_Ketchum",
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

void init_player(Player *i)
{
    i->hp = 0;
    i->hp_limit = 0;
    i->handcard = create_vector(10);
    i->handcard->id = i->id;
    
    i->judgeCards = create_vector(5);
    i->judgeCards->id = i->id;
  
    i->attack_distance = 1;
    i->attack_power = 1;
    
    i->equipWeapon = NONE;
    i->weapon = create_vector(5);
    i->weapon->id = i->id;
    
    i->equipShield = NONE;    
    i->shield = create_vector(5);
    i->shield->id = i->id;
    
    i->equipScope = NONE;
    i->equipMustang = NONE;
    i->distance_item = create_vector(5);
    i->distance_item->id = i->id;
    
    i->role = El_Gringo;
    i->state = SET;
    i->identity = Sheriff;
    i->numOfBang = 1;
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


//   #ifdef DEBUG 
    

    // PLAYERS_LIST[0].role = Paul_Regret;
    // PLAYERS_LIST[1].role = Pedro_Ramirez;
    // PLAYERS_LIST[2].role = Rose_Doolan;
    // PLAYERS_LIST[3].role = Sid_Ketchum;
    // PLAYERS_LIST[4].role = Slab_the_Killer;
    // PLAYERS_LIST[5].role = Suzy_Lafayette;
    // PLAYERS_LIST[6].role = Willy_the_Kid;

    // Card c; 
    // c.kind = MUSTANG; 
    // c.sticker = MUSTANG; 
    // c.number = 2; 
    // c.suit = 2; 
    // c.attribute = 1; 
    // c.is_orange = false; 

    // push_back( PLAYERS_LIST[0].handcard, c ); 
    // push_back( PLAYERS_LIST[1].handcard, c ); 
    // push_back( PLAYERS_LIST[2].handcard, c ); 
    // push_back( PLAYERS_LIST[3].handcard, c ); 
    // push_back( PLAYERS_LIST[4].handcard, c ); 
    // push_back( PLAYERS_LIST[5].handcard, c ); 
    // push_back( PLAYERS_LIST[6].handcard, c ); 

    
//   #endif
}

void game_prepare()
{
    //設定遊戲人數    
    PLAYERS_NUM = scan(4, 7, "Input the numbers of players (4~7): ");
    ALIVE_NUM = PLAYERS_NUM;
    PLAYERS_LIST = (Player*)calloc( PLAYERS_NUM, sizeof(Player) );
    
    //設定遊戲玩家名稱
    char *players = calloc(100, sizeof(char));
    for(int i = 0; i < PLAYERS_NUM; i++)
    {
        PLAYERS_LIST[i].id = i;  
      init_player(&PLAYERS_LIST[i]);
        
        PLAYERS_LIST[i].handcard->id = i;
        printf( "Input the name of player %d : ", i+1 );
        fgets( players, 1000, stdin );
        if( strcmp( players, "\n") == 0 ) {
            sprintf( players, "Player %d", i );
        }
        clean_buffer(players);
        strcpy(PLAYERS_LIST[i].name, players);
        //printf("%s\n",PLAYERS_LIST[i].name);
    }
  
    memset( OFFSET_DISTANCE, 0, 100 * sizeof(int) );
    memset( DISTANCE, 0, 100 * sizeof(int) );

    calcDistance();
    
    //設定身分人數
    SHERIFF_NUM = 1, RENEGADE_NUM = 1;
    OUTLAWS_NUM = PLAYERS_NUM > 5 ? 3 : 2;
    DEPUTIES_NUM = PLAYERS_NUM - SHERIFF_NUM - RENEGADE_NUM - OUTLAWS_NUM;
    
    int counter = 0;
    for(int i = 0; i < SHERIFF_NUM; i++)
        PLAYERS_LIST[counter++].identity = Sheriff;
    for(int i = 0; i < DEPUTIES_NUM; i++)
        PLAYERS_LIST[counter++].identity = Deputies;
    for(int i = 0; i < OUTLAWS_NUM; i++)
        PLAYERS_LIST[counter++].identity = Outlaws;
    for(int i = 0; i < RENEGADE_NUM; i++)
        PLAYERS_LIST[counter++].identity = Renegade;
    SHERIFF_POSITION = identity_shuffle(); //警長位置
    
    /*for(int i = 0; i < PLAYERS_NUM; i++)
        printf("%d\n", PLAYERS_LIST[i].identity);*/

    //第一次洗牌    
    deck = create_vector(80);
    discardPile = create_vector(80);
    
    for (int i = 0; i < 80; i++) {
        push_back(discardPile, CARD[i]);
    }

    shuffle();   
  
    //設定角色    
    role_shuffle();
    for ( int i = 0; i < PLAYERS_NUM; i++ ) {
      Player *p = PLAYERS_LIST + i;
       //skills[p->role]( &p, NULL );
    }
    PLAYERS_LIST[SHERIFF_POSITION].hp++;
    PLAYERS_LIST[SHERIFF_POSITION].hp_limit++;    
 
    GAME_STATE = NOT_YET_START;
}

void set_card(int i, bool o, int s, int n, int a, int k) 
{
    CARD[i].is_orange = o;
    CARD[i].kind = k;
    CARD[i].sticker = k;
    CARD[i].suit = s;
    CARD[i].number = n;
    CARD[i].attribute = a;
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
        //printf("%2d  %2d%6d%9d%9d%9d\n", a, b, c, d, e, f);        
        set_card(i, d, b, c, e, f);
    } 
    fclose(fp);
    free(line);
}

