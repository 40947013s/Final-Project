#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef enum _role_mode
{
    SET,
    IS_MY_TURN, 
    IS_OTHERS_TURN, 
    IS_DEAD
} RMode; 

typedef enum _game_mode
{
    NOT_YET_START,
    IN_ROUND,
    END
} GMode; 

typedef enum _identity_kind
{
    Sheriff, 
    Deputies,
    Outlaws,
    Renegade
} Identity; 

typedef enum _role_kind
{
    Bart_Cassidy, Black_Jack, Calamity_Janet, El_Gringo, 
    Jesse_Jones, Jourdonnais, Kit_Carlson, Lucky_Duke,
    Paul_Regret, Pedro_Ramirez, Rose_Doolan, Sid_Ketchum,
    Slab_the_Killer, Suzy_Lafayette, Vulture_Sam, Willy_the_Kid
} Role;

typedef enum _card_kind
{
    BANG, MISSED, GATLING, INDIANS, PANIC, 
    CAT, STAGECOACH, WELLS, STORE, BEER, 
    SALOOW, DUEL, BARREL, SCOPE, MUSTANG, 
    JAIL, DYNAMITE, VOLCANIC, SCHOFIELD, 
    REMINGTON, REV, WINCHEDTER
} Kind;

typedef struct _card Card;
struct _card
{    
    bool is_orange; //消耗 or 裝備
    int suit;
    int number;
    int kind;
    int attribute; // 主動 or 被動
    // 這張牌是否可以主動被出出來
};

typedef struct _player_list
{
    bool    dynamite;
    bool    jail;
    int     hp;
    //int     skill; --
    int     distance1;
    int     distance2;
    int     handcard_num;
    int     equipment;
    int     *handcard; 
    char    name[1000];    
    Role    role;
    RMode   state;    
    Identity identity;
} Player;


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

      
int DECK_NUM, DISCARD_NUM;
int PLAYERS_NUM;
int DECK[80], DISCARD[80];
int SHERIFF_NUM, DEPUTIES_NUM, OUTLAWS_NUM, RENEGADE_NUM;
int *SEAT_POSITION;
GMode GAME_STATE;
Card CARD[80];
Player *PLAYERS_LIST;

#define number_of_players(x) x >= 4 && x <= 7

void clean_buffer(char *arr)
{
    if(arr[strlen(arr)-1] == '\n')
        arr[strlen(arr)-1] = 0;
    else
    {
        int32_t c = 0;
        while((c = fgetc(stdin)) != '\n' && c != EOF){}
    }
}

void init_player(Player *i)
{
    i->dynamite = false;
    i->jail = false;
    i->hp = 0;
    i->distance1 = 0;
    i->distance2 = 0;
    i->handcard_num = 0;
    i->role = Bart_Cassidy;
    i->state = SET;
    i->equipment = -1;
    i->identity = Sheriff;
}

void print_player(int th)
{   
    printf("Player %d\n", th+1);
    printf("Dynamite : %s\n", PLAYERS_LIST[th].dynamite == true ? "true" : "false");
    printf("Jail : %s\n", PLAYERS_LIST[th].jail == true ? "true" : "false");
    printf("Distance1 : %d\n", PLAYERS_LIST[th].distance1);
    printf("Distance2 : %d\n", PLAYERS_LIST[th].distance2);
    printf("Name : %s\n", PLAYERS_LIST[th].name);
    printf("Role : %d\n", PLAYERS_LIST[th].role);
    printf("State : %d\n", PLAYERS_LIST[th].state);
    printf("Equipment : %d\n", PLAYERS_LIST[th].equipment); 
    printf("Handcard : ");
    for(int j = 0; j < PLAYERS_LIST[th].handcard_num; j++)
        printf("%d ", PLAYERS_LIST[th].handcard[j]); 
    printf("\n");
    printf("Identity : %d\n\n\n", PLAYERS_LIST[th].identity);
    
}

void shuffle()
{
    srand(time(NULL));
    int idx[80];
    for(int i = 1 ; i <= DISCARD_NUM; i++)
    {
        do {
            idx[i-1] = rand() % DISCARD_NUM + 1;  
            for(int j = 1 ; j < i ; j++)
                if(idx[i-1] == idx[j-1]) break;   
        } while(j != i);
        DECK[i-1] = DISCARD[idx[i-1]-1];
        //printf("%2d ", DECK[i-1]);
        //if(i%10 == 0) printf("\n");
    }  
    DECK_NUM += DISCARD_NUM;
    memset(DISCARD, 0, 80);
}

void game_prepare()
{
    char *players = calloc(1000, sizeof(char));
    int warn = 0;
    while(1)
    {
        if(warn) printf("Wrong input.\n");
        printf("Input the numbers of players (4~7): ");
        fgets(players, 1000, stdin);
        clean_buffer(players);
        PLAYERS_NUM = strtol(players, NULL , 10);
        warn++;
        if(number_of_players(PLAYERS_NUM) == true) break;
    } 
    SHERIFF_NUM = 1, RENEGADE_NUM = 1;
    OUTLAWS_NUM = PLAYERS_NUM > 5 ? 2 : 3;
    DEPUTIES_NUM = PLAYERS_NUM - SHERIFF_NUM - RENEGADE_NUM - OUTLAWS_NUM;
    
    SEAT_POSITION = calloc(PLAYERS_NUM, sizeof(int));
    PLAYERS_LIST = calloc(PLAYERS_NUM, sizeof(int));
    
    for(int i = 0; i < PLAYERS_NUM; i++)
    {
        SEAT_POSITION[i] = i;
        init_player(&PLAYERS_LIST[i]);
        printf("Input the name of player %d : ", i+1);
        fgets(players, 1000, stdin);
        clean_buffer(players);
        strcpy(PLAYERS_LIST[i].name, players);
        //printf("%s\n",PLAYERS_LIST[i].name);
    }
    DISCARD_NUM = 80, DECK_NUM = 0;
    for(int i = 0; i < 80; i++)
        DISCARD[i] = i;
    shuffle();
    printf("\n");
}

void set_card(int i, bool o, int s, int n, int a, int k) 
{
    CARD[i].is_orange = o;
    CARD[i].kind = k;
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
}

void print_card(int i)
{
    printf("th  suit number  is_orange attribute kind\n");
    printf("%2d  %2d%6d%9d%9d%9d\n", i, CARD[i].suit,  CARD[i].number, CARD[i].is_orange, CARD[i].attribute, CARD[i].kind);   
}

int main()
{
    init_card();
    game_prepare();
    print_player(1);
    print_player(3);

    //print_card(30);
    
    return 0;

    /*
    他總共最多80張牌
    那如果建一個txt 直接列80張牌
    然後一行一張牌
    比如說
       kind suit number  is_orange attribute
    1. BANG 黑桃    A      true       主動
    2. BANG 愛心    Ｑ     true       主動
    3. BANG 愛心    K      true       主動
    .....
    迴圈讀80次
    就建好80張牌
    這樣會比較好嗎
    沒有，我只是提議
    我覺得
    因為我們不知道我們會不會寫到一半突然想要新增card裡面的東西
    如果讀檔 我們頂多就是新增一列 
    在更改比較容易？
    嗯嗯恩
    */
}


