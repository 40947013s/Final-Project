#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

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

typedef struct _card Card;
struct _card
{    
    bool exist;
    bool is_orange; //消耗 or 裝備
    int suit;
    int number;
    int distance;
    void (*func) (Card *this);
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

int DECK_NUM, DISCARD_NUM;
int PLAYERS_NUM;
int SHERIFF_NUM, DEPUTIES_NUM, OUTLAWS_NUM, RENEGADE_NUM;
int *SEAT_POSITION;
GMode GAME_STATE;
Card DECK, DISCARD;
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
}

int main()
{
    game_prepare();
    print_player(1);
  print_player(3);
    return 0;
}


