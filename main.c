#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "defineList.h"
#include "structList.h"
#include "vector.h"
#include "skillFunc.h"
#include "utilityFunc.h"
#include "globalVar.h"

int PLAYERS_NUM; //遊戲人數
int SHERIFF_NUM, DEPUTIES_NUM, OUTLAWS_NUM, RENEGADE_NUM; //身分人數

int *SEAT_POSITION; //座位紀錄 (從0開始記錄至遊戲人數-1)
int DISTANCE[7][7]; //相對距離表 distance[i][j]: i 看 j
GMode GAME_STATE; //遊戲狀態
Card CARD[80]; //消耗牌+裝備牌
Player *PLAYERS_LIST; //玩家狀態紀錄

Card_vector* deck;
Card_vector* discardPile;

typedef void (*Skill) ( void *this, void* argv );
Skill skills[16] = {
    fBart_Cassidy, fBlack_Jack, fCalamity_Janet, fEl_Gringo, 
    fJesse_Jones, fJourdonnais, fKit_Carlson, fLucky_Duke,
    fPaul_Regret, fPedro_Ramirez, fRose_Doolan, fSid_Ketchum,
    fSlab_the_Killer, fSuzy_Lafayette, fVulture_Sam, fWilly_the_Kid
};

int min(int a, int b)
{
  if(a<=b)
  {
    return a;
  }
  else
  {
    return b;
  }
}
void init_player(Player *i)
{
    // i->dynamite = false;
    // i->jail = false;
    i->hp = 0;
    i->handcard = create_vector(10);
    i->judgeCards = create_vector(5);
    i->equipment = create_vector(5);
    i->role = Bart_Cassidy;
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
        if(j == res) res = i;
        if(i == res) res = j;
    }
    return res;
}

void role_shuffle() {
    srand(time(NULL));
    bool is_take[16] = {0};
    int num = PLAYERS_NUM*2, choice = -1, last = -1;
    for (int i = 0; i < num; i++) {
        int j = rand() % 16;
        if (is_take[j]) {
            i--;
            continue;
        }
        is_take[j] = true;
        if(i%2 == 1) {  
            choice = scan(0, 1, "Which role card to choose (0 or 1): ");
            choice == 0 ? last : j;
            PLAYERS_LIST[i/2].role = j+choice;            
        } last = j;
    }
}

void game_prepare()
{
    //設定遊戲人數    
    PLAYERS_NUM = scan(4, 7, "Input the numbers of players (4~7): ");
    
    //設定座位    
    SEAT_POSITION = calloc(PLAYERS_NUM, sizeof(int));
    PLAYERS_LIST = calloc(PLAYERS_NUM, sizeof(Player));
      
    //設定遊戲玩家名稱
    char *players = calloc(100, sizeof(char));
    for(int i = 0; i < PLAYERS_NUM; i++)
    {
        SEAT_POSITION[i] = i;
        init_player(&PLAYERS_LIST[i]);
        PLAYERS_LIST[i].id = i;
        printf("Input the name of player %d : ", i+1);
        fgets(players, 1000, stdin);
        clean_buffer(players);
        strcpy(PLAYERS_LIST[i].name, players);
        //printf("%s\n",PLAYERS_LIST[i].name);
    }

    //設定座位    
    for(int i=0;i<7;i++)
    {
      for(int j=0;j<7;j++)
      {
        int tmp = 0;
        if(i>=j)
        {
          tmp = (7-i)+(j-0);
        }
        else
        {
          tmp = (i-0)+(7-j);
        }
        DISTANCE[i][j] = min(abs(i-j),tmp);
      }
    }
    
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
    counter = identity_shuffle(); //警長位置
    PLAYERS_LIST[counter].hp++;
    
    /*for(int i = 0; i < PLAYERS_NUM; i++)
        printf("%d\n", PLAYERS_LIST[i].identity);*/

    //設定角色    
    role_shuffle();
    for ( int i = 0; i < PLAYERS_NUM; i++ ) {
      Player p = PLAYERS_LIST[i];
      skills[p.role]( &p, NULL );
    }
    
    //第一次洗牌    
    deck = create_vector(80);
    discardPile = create_vector(80);
    
    for (int i = 0; i < 80; i++) {
        push_back(discardPile, CARD[i]);
    }

    shuffle();    
    printf("\n");
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

void print_card(Card i) 
{
    printf("th  suit number  is_orange attribute kind\n");
    printf("%2d%6d%9d%9d%9d\n", i.suit,  i.number, i.is_orange, i.attribute, i.kind);   
    return;    
}

int main()
{
    init_card();
    game_prepare();

    int i = 0; //從警長開始
    while ( GAME_STATE != END ) 
    {
        
        Player p = PLAYERS_LIST[i];

        // if ( p.state == IS_DEAD )
        // {
        //     i++;
        //     if ( i == PLAYERS_NUM ) i = 0;
        //     continue;
        // }

      
        skills[p.role]( &p, NULL );

        if ( p.state == SET )
        {

        }
        else if ( p.state == JUDGE )
        {
        
        }
        else if ( p.state == GET_CARD )
        {

        }
        else if ( p.state == PLAY_CARD )
        {
          
        }
        else if ( p.state == DISCARD_CARD )
        {

        }
        else if( p.state == FINISH_TIHS_TURN )
        {
            i++;
            if ( i == PLAYERS_NUM ) i = 0;
            PLAYERS_LIST[i].state = JUDGE;
        }
        
    }
    
    return 0;
}


