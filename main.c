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
#include "UI.h"
#include "test.h"

int PLAYERS_NUM, DEAD_NUM; //遊戲人數
int SHERIFF_NUM, DEPUTIES_NUM, OUTLAWS_NUM, RENEGADE_NUM; //身分人數
int SHERIFF_POSITION;
int DISTANCE[7][7]; //相對距離表 distance[i][j]: i 看 j
GMode GAME_STATE; //遊戲狀態
Card CARD[80]; //消耗牌+裝備牌
Player *PLAYERS_LIST; //玩家狀態紀錄
Player *DEAD_LIST; //死亡玩家狀態紀錄

Card_vector* deck;
Card_vector* discardPile;

// typedef void (*Skills) ( void *this, void* argv );
Skill skills[16] = {
    fBart_Cassidy, fBlack_Jack, fCalamity_Janet, fEl_Gringo, 
    fJesse_Jones, fJourdonnais, fKit_Carlson, fLucky_Duke,
    fPaul_Regret, fPedro_Ramirez, fRose_Doolan, fSid_Ketchum,
    fSlab_the_Killer, fSuzy_Lafayette, fVulture_Sam, fWilly_the_Kid
};

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
char *Color[9] = { RESET, RED, YELLOW, GREEN, BLUE, MAG, CYN, WHT, RED_BACK };

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
    i->hp = 0;
    i->hp_limit = 0;
    i->handcard = create_vector(10);
    i->judgeCards = create_vector(5);
    i->attack_distance = 1;
    i->equipWeapon = NONE;
    i->weapon = create_vector(5);
    i->equipShield = NONE;
    i->shield = create_vector(5);
    i->equipScope = NONE;
    i->equipMustang = NONE;
    i->distance_item = create_vector(5);
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
        printf( "Your role: %s\n", roleName[res] );
        PLAYERS_LIST[i].hp = 4, PLAYERS_LIST[i].hp_limit = 4;
        if( res == El_Gringo || res == Paul_Regret ) {
            PLAYERS_LIST[i].hp --, PLAYERS_LIST[i].hp_limit --;
        }
    }
  
    // srand(time(NULL));
    // bool is_take[16] = {0};
    // int num = PLAYERS_NUM*2, choice = -1, last = -1;
    // for (int i = 0; i < num; i++) {
    //     int j = rand() % 16;
    //     if (is_take[j]) {
    //         i--;
    //         continue;
    //     }
    //     is_take[j] = true;
    //     if(i%2 == 1) {  
    //         choice = scan(0, 1, "Which role card to choose (0 or 1): ");
    //         int c = choice == 0 ? (last-1) : (j-1);
    //         PLAYERS_LIST[i/2].role = c;            
    //     } last = j;
    // }
}

void game_prepare()
{
    //設定遊戲人數    
    PLAYERS_NUM = scan(4, 7, "Input the numbers of players (4~7): ");
    PLAYERS_LIST = (Player*)calloc( PLAYERS_NUM, sizeof(Player) );
    DEAD_LIST = (Player*)calloc( PLAYERS_NUM, sizeof(Player) );
    //設定遊戲玩家名稱
    char *players = calloc(100, sizeof(char));
    for(int i = 0; i < PLAYERS_NUM; i++)
    {
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

void print_card(Card i) 
{
    printf("th  suit number  is_orange attribute kind\n");
    printf("%2d%6d%9d%9d%9d\n", i.suit,  i.number, i.is_orange, i.attribute, i.kind);   
    return;    
}


void playerCard( Player *player, int *numOfBang ) {
    int *color = NULL;
    int num = setColor( &color, -1, -1, 1, player->handcard, 3 );
    if ( *numOfBang == 0 ) {
        setColor( &color, -1, BANG, 0, player->handcard, 0 );
    }
    if ( num == 0 ) {
        puts("No hand card");
        player->state = DISCARD_CARD;
        return;
    }

    int choice, size = player->handcard->size;
    char *str = (char *)calloc( strlen("Choice the card you want to play(1~, 0: end the round): ") + 5, sizeof(char) );
    sprintf( str, "Choice the card you want to play(1~%d, 0: end the round): ", size );
    char input[100] = {0};
    bool warn = false;
    while ( 1 ) {
        system("clear");
        printUI( player );
        if ( warn )
            puts("You can't play this card");
        printHandCard( player->handcard, color);
        choice = scan( 0, size, str );
        if ( choice == 0 ) {
            player->state = DISCARD_CARD;
            return;
        }
        else if ( color[choice-1] == 3 ) {
            color[choice-1] = 1;
            system("clear");
            printUI( player );
            printHandCard( player->handcard, color);
            Card tmp = get_element( player->handcard, choice-1 );
            printf( "Are you sure you want to use the [%s] card? (Y/n) ", cardKindName[tmp.sticker] );
            fgets(input, 100, stdin);
            if ( input[0] == 'Y' || input[0] == 'y' ) {
                warn = false;
                if ( tmp.sticker == BANG ) {
                    if ( Bang( player ) ) {
                      discardCard( player->handcard, choice-1 );
                    }
                }
                else if ( tmp.sticker == INDIANS ) {
                    if ( Indians( player ) ) {
                        discardCard( player->handcard, choice-1 );
                    }
                }
                else if ( tmp.sticker == STAGECOACH ) {
                    if ( Stagecoach( player ) ) {
                        discardCard( player->handcard, choice-1 );
                    }
                }
                else if ( tmp.sticker == STORE ){
                    if ( Store( player ) ) {
                        discardCard( player->handcard, choice-1 );
                    }
                }
                else if ( tmp.sticker == BEER ) {
                    if ( Beer( player, player ) ) {
                        discardCard( player->handcard, choice-1 );
                    }
                }
                else if ( tmp.sticker == SALOOW ) {
                    if ( Saloow( player ) ) {
                        discardCard( player->handcard, choice-1 );
                    }
                }
                else if ( tmp.sticker == DUEL ) {
                    if ( Duel( player ) ) {
                        discardCard( player->handcard, choice-1 );
                    }
                }
                else if ( tmp.sticker == SCOPE ) {
                    EquipScope( player, choice-1 );
                }
                else if ( tmp.sticker == MUSTANG ) {
                    EquipMustang( player, choice-1 );
                }
                else if ( tmp.sticker == VOLCANIC ) {
                    EquipVolcanic( player, choice-1 );
                }
                else if ( tmp.sticker == SCHOFIELD ) {
                    EquipSchofield( player, choice-1 );
                }
                else if ( tmp.sticker == REMINGTON ) {
                    EquipRemington( player, choice-1 );
                }
                else if ( tmp.sticker == REV ) {
                    EquipRev( player, choice-1 );
                }
                else if ( tmp.sticker == WINCHEDTER ) {
                    EquipWinchester( player, choice-1 );
                }
                break;
            }
            else {
                warn = false;
                color[choice-1] = 3;
                continue;
            }
          }
    }
    free( str );
}

int main()
{
    init_card();
    game_prepare();

    int i = SHERIFF_POSITION; //從警長開始
    int numOfBang = PLAYERS_LIST[i].numOfBang; // -1表示可以連續無限次
    Player *p = &(PLAYERS_LIST[i]);

    // 發牌給所有玩家
    if ( GAME_STATE == NOT_YET_START ) {
        for ( int i = 0; i <= PLAYERS_NUM; i++ ) {
          cardHandler( PLAYERS_LIST + i, PLAYERS_LIST[i].hp );
          PLAYERS_LIST[i].state = JUDGE;
          
        }
        GAME_STATE = IN_ROUND;
    }
    
    while ( GAME_STATE != END ) 
    {
        printUI( p );
        
        // skills[p->role]( &p, NULL );

        if ( p->state == SET )
        {
          p->state = JUDGE;
        }
        else if ( p->state == JUDGE )
        {
          p->state = GET_CARD;
        }
        else if ( p->state == GET_CARD )
        {
            if ( cardHandler( p, 2 ) < 2 ) {
                puts( "場上卡片不足" );
                return 0;
            }
            p->state = PLAY_CARD;
        }
        else if ( p->state == PLAY_CARD )
        {
            playerCard(p, &numOfBang);
        }
        else if ( p->state == DISCARD_CARD )
        {
            p->state = FINISH_TIHS_TURN;
        }
        else if( p->state == FINISH_TIHS_TURN )
        {
            i++;
            if ( i == PLAYERS_NUM ) i = 0;
            p = &(PLAYERS_LIST[i]);
            PLAYERS_LIST[i].state = JUDGE;
            numOfBang = PLAYERS_LIST[i].numOfBang;
        }
        
    }
    

    return 0;
}

    /* test INDIANS Card
    Card c;
    c.kind = INDIANS;
    c.sticker = INDIANS;
    c.number = 13;
    c.suit = 3;
    c.attribute = 1;
    push_back( p->handcard, c );
    */
    /* test STAGECOACH Card
    Card c;
    c.kind = STAGECOACH;
    c.sticker = STAGECOACH;
    c.number = 9;
    c.suit = 1;
    c.attribute = 1;
    push_back( p->handcard, c );
    */
    /* test STORE Card
    Card c;
    c.kind = STORE;
    c.sticker = STORE;
    c.number = 9;
    c.suit = 1;
    c.attribute = 1;
    push_back( p->handcard, c );
    */
    /* test DUEL card
    Card c;
    c.kind = DUEL;
    c.sticker = DUEL;
    c.number = 9;
    c.suit = 1;
    c.attribute = 1;
    push_back( p->handcard, c );
    */
    /* test Scope card
    Card c;
    c.kind = SCOPE;
    c.sticker = SCOPE;
    c.number = 9;
    c.suit = 1;
    c.attribute = 1;
    push_back( p->handcard, c );
    push_back( p->handcard, c );
    */
    /* test Mustang card
    Card c;
    c.kind = MUSTANG;
    c.sticker = MUSTANG;
    c.number = 9;
    c.suit = 1;
    c.attribute = 1;
    push_back( p->handcard, c );
    push_back( p->handcard, c );
    */
    /* test distance item card
    Card c;
    c.kind = VOLCANIC;
    c.sticker = VOLCANIC;
    c.number = 9;
    c.suit = 1;
    c.attribute = 1;
    push_back( p->handcard, c );

    c.kind = SCHOFIELD;
    c.sticker = SCHOFIELD;
    push_back( p->handcard, c );

    c.kind = REMINGTON;
    c.sticker = REMINGTON;
    push_back( p->handcard, c );

    c.kind = REV;
    c.sticker = REV;
    push_back( p->handcard, c );

    c.kind = WINCHEDTER;
    c.sticker = WINCHEDTER;
    push_back( p->handcard, c );
    */