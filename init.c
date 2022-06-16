#include "init.h"

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
Skill skills[16] = {
    fBart_Cassidy, fBlack_Jack, fCalamity_Janet, nullFunc,
    fJesse_Jones, fJourdonnais, fKit_Carlson, nullFunc,
    fPaul_Regret, fPedro_Ramirez, fRose_Doolan, fSid_Ketchum,
    fSlab_the_Killer, fSuzy_Lafayette, nullFunc, fWilly_the_Kid
};


// typedef enum _card_kind
// {
//     BANG, MISSED, GATLING, INDIANS, PANIC, 
//     CAT, STAGECOACH, WELLS, STORE, BEER, 
//     SALOOW, DUEL, 
    
//     BARREL, SCOPE, MUSTANG, 
//     JAIL, DYNAMITE, VOLCANIC, SCHOFIELD, 
//     REMINGTON, REV, WINCHEDTER, NONE
// } Kind;

// Orange Card
OrangeCard orangeCards[12] = { Bang, nullFunc2, Indians, nullFunc2, panic,
                                nullFunc2, Stagecoach, Wells, Store, nullFunc2, Saloow, Duel};

// Blue Card
BlueCard blueCards[23] = { nullFunc3, nullFunc3, nullFunc3, nullFunc3, nullFunc3,
                            nullFunc3, nullFunc3, nullFunc3, nullFunc3, nullFunc3,
                            nullFunc3, nullFunc3, nullFunc3,
                            EquipScope, EquipMustang, nullFunc3, 
                            EquipDynamite, EquipVolcanic, EquipSchofield, EquipRemington,
                            EquipRev, EquipWinchester};


// 身分 
char *identityName[] = { "Sheriff", "Deputies", "Outlaws", "Renegade" };

// 角色
char *roleName[] = {
    "Bart Cassidy", "Black Jack", "Calamity Janet", "El Gringo", 
    "Jesse Jones", "Jourdonnais", "Kit Carlson", "Lucky Duke",
    "Paul Regret", "Pedro Ramirez", "Rose Doolan", "Sid Ketchum",
    "Slab the Killer", "Suzy Lafayette", "Vulture Sam", "Willy the Kid"
};

// 消耗牌+裝備牌
char *cardKindName[] = {
    "BANG", "MISSED", "GATLING", "INDIANS", "PANIC", 
    "CAT", "STAGECOACH", "WELLS", "STORE", "BEER", 
    "SALOOW", "DUEL", "BARREL", "SCOPE", "MUSTANG", 
    "JAIL", "DYNAMITE", "VOLCANIC", "SCHOFIELD", 
    "REMINGTON", "REV", "WINCHEDTER"
};

// 花色
char *suitName[] = { "none", "spade", "heart", "diamond", "club" };

// Color
char *Color[11] = { RESET, RED, YELLOW, GREEN, BLUE, MAG, CYN, WHT, GRAY, GRAY_BACK, RED_BACK };

// State
char *stateName[10] = { "SET", "JUDGE", "GET_CARD", "PLAY_CARD", "DISCARD_CARD", "MINUS_HP", "FIGHT", "ATTACKED", "FINISH", "DEAD"};

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
        // print role k
        puts( "Which role card to choose (0 or 1): " );
        printf( "[0] %s, [1] %s\n", roleName[j], roleName[k]);
        int choice = scan(0, 1, "--> ");
        if(choice == 0) {
            PLAYERS_LIST[i].role = j, res = j;
        } else {
            PLAYERS_LIST[i].role = k, res = k;
        }
        printf( "Your role: %s\n\n", roleName[res] );
        PLAYERS_LIST[i].hp = 4, PLAYERS_LIST[i].hp_limit = 4;
        if( res == El_Gringo || res == Paul_Regret ) {
            PLAYERS_LIST[i].hp --, PLAYERS_LIST[i].hp_limit --;
        }
    }
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
        printf("Input the name of player %d : ", i+1);
        fgets(players, 1000, stdin);
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
      // skills[p->role]( &p, NULL );
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

