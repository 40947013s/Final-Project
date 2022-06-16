#include "utilityFunc.h"

void print_player(int th)
{   
    printf("Player %d\n", th+1);
    printf("Name : %s\n", PLAYERS_LIST[th].name);
    printf("Role : %d\n", PLAYERS_LIST[th].role);
    printf("State : %d\n", PLAYERS_LIST[th].state);
    // printf("Equipment : ");
    // for(int j = 0; j < PLAYERS_LIST[th].equipment->size; j++) {
    //   printf("%d ", PLAYERS_LIST[th].equipment->data[j].kind); 
    // }
    printf( "Weapon : " );
    for ( int i = 0; i < PLAYERS_LIST[th].weapon->size; i++ ) {
        printf( "%d ", PLAYERS_LIST[th].weapon->data[i].kind );
    }
    printf( "\nShield : " );
    for ( int i = 0; i < PLAYERS_LIST[th].shield->size; i++ ) {
        printf( "%d ", PLAYERS_LIST[th].shield->data[i].kind );
    }
    printf( "\nDistance_item : " );
    for ( int i = 0; i < PLAYERS_LIST[th].distance_item->size; i++ ) {
        printf( "%d ", PLAYERS_LIST[th].distance_item->data[i].kind );
    }
    printf("\nHandcard : ");
    for(int j = 0; j < PLAYERS_LIST[th].handcard->size; j++) {
      printf("%d ", PLAYERS_LIST[th].handcard->data[j].kind); 
    }
        
    printf("\n");
    printf("Identity : %d\n\n\n", PLAYERS_LIST[th].identity);    
}

/*棄牌區的牌丟牌庫*/
void shuffle() {
    srand(time(NULL));
    bool is_take[80] = {0};
    int num = discardPile->size;
    for (int i = 0; i < num; i++) {
        int j = rand() % num;
        if (is_take[j]) {
            i--;
            continue;
        }
        is_take[j] = true;
        Card tmp = get_element(discardPile, j);
        push_back(deck, tmp);
    }
    clear_vector(discardPile);
}

int cardHandler( Player * player, int num ) {
    if ( num > deck->size ) {
        shuffle();
    }

    for ( int i = 0; i < num; i++ ) {
        if ( deck->size == 0 ) return i;
        Card tmp = pop_back(deck);
        tmp.sticker = tmp.kind;
        push_back(player->handcard, tmp);
    }

    return num;
}

bool discardCard( Card_vector * cards, int index ) {
    if ( cards == NULL ) return false;
    if ( isEmpty( cards ) ) return false;
    Card tmp = get_element( cards, index);
    remove_element((cards), index);
    // 如果是內建則直接丟棄
    if( tmp.suit == -1 ) {
        return true;
    }
    tmp.sticker = tmp.kind;
    push_back(discardPile, tmp);

    // assert cards->id == -1 or cards->id == some players.id
    if ( cards->id >= 0 && cards->id < PLAYERS_NUM  ) {
      Player *player = &(PLAYERS_LIST[cards->id]);
      if ( player->state == IS_DEAD ) return true;
      skills[player->role]( player ); 
    }
    
    return true;
}

// 棄掉所有的牌，包含手牌、装備
void discardAllCard( Player *player ) {
  if ( player == NULL ) return;
  while ( !isEmpty( player->handcard ) ) {
    Card tmp = pop_back(player->handcard);
    if ( tmp.suit == -1 ) continue;
    tmp.sticker = tmp.kind;
    push_back(discardPile, tmp);
  }

  // bool UnloadWeapon( Player* player, Card_vector *cards );
  while ( !isEmpty( player->weapon ) ) {
    Card tmp = pop_back(player->weapon);
    if ( tmp.suit == -1 ) continue;
    tmp.sticker = tmp.kind;
    push_back(discardPile, tmp);
  }

  player->equipWeapon = NONE;
  while ( !isEmpty(player->shield) ) {
    Card tmp = pop_back(player->shield);
    if ( tmp.suit == -1 ) continue;
    tmp.sticker = tmp.kind;
    push_back(discardPile, tmp);
  }
  player->equipShield = NONE;
  while ( !isEmpty( player->distance_item ) ) {
    Card tmp = pop_back(player->distance_item);
    if ( tmp.suit == -1 ) continue;
    tmp.sticker = tmp.kind;
    push_back(discardPile, tmp);
  }
  player->equipScope = NONE;
  player->equipMustang = NONE;

  while ( !isEmpty( player->judgeCards ) ) {
    Card tmp = pop_back(player->judgeCards);
    if ( tmp.suit == -1 ) continue;
    tmp.sticker = tmp.kind;
    push_back(discardPile, tmp);
  }

  // asssert the player is already dead
  if ( player->state == IS_DEAD ) return;
  skills[player->role]( player ); 
}

void printCard( Card card, char *color ) 
{
    // printf( "is_orange: %d\n", card.is_orange );
    printf( "%ssuit: %s%s\n", color, suitName[card.suit], RESET );
    printf( "%snumber: %d%s\n", color, card.number, RESET );
    // printf( "attribute: %d\n", card.attribute );
    printf( "%skind: %s%s\n", color, roleName[card.kind], RESET );
    printf( "%sSticker: %s%s\n", color, roleName[card.sticker], RESET );
}

void clean_buffer(char *arr)
{
    if(arr[strlen(arr)-1] == '\n')
        arr[strlen(arr)-1] = 0;
    else {
        int32_t c = 0;
        while((c = fgetc(stdin)) != '\n' && c != EOF){}
    }
}

int scan(int min, int max, char *str)
{
    int warn = 0, choice = -1;
    char *input = malloc(1000);
    while(1) {
        if(warn) printf("Wrong input.\n\n");
        printf("%s", str);
        fgets(input, 1000, stdin);
        clean_buffer(input);
        choice = strtol(input, NULL , 10);
        if( strcmp( input, "0") != 0 && choice == 0 ) {
            warn++;            
            continue;
        }
        warn++;
        if(choice >= min && choice <= max) break;
    }  

    free( input );
    return choice;
}

// 把牌從 player1第[index]張牌給player2, index = 0, 1, 2....
bool takeCard( Card_vector *p1, Card_vector *p2, int index ) {
  if ( p1 == NULL || p2 == NULL ) return false;
  if ( isEmpty( p1 ) ) return false;

  Card card = get_element( p1, index );
  if ( card.suit == -1 ) false;
  remove_element( p1, index );
  card.sticker = card.kind;
  push_back( p2, card );

  // assert p1->id == -1 or p1->id == some players.id
  if ( p1->id >= 0 && p1->id < PLAYERS_NUM  ) {
    Player *player = &(PLAYERS_LIST[p1->id]);
    if ( player->state == IS_DEAD ) return true;
    skills[player->role]( player );  
  }
  return true;
}

// 把player 1 所有的牌給 player 2，包含手牌、装備
void takeAllCards( Player *p1, Player *p2 ) {
  if ( p1 == NULL || p2 == NULL ) return;
  if ( p2->state == IS_DEAD ) return;
  for ( int i = 0; i < p1->handcard->size; i++ ) {
      Card card = pop_back( p1->handcard );
      if ( card.suit == -1 ) continue;
      card.sticker = card.kind;
      push_back( p2->handcard, card );
  }
  for ( int i = 0; i < p1->weapon->size; i++ ) {
    Card card = pop_back( p1->weapon );
    if ( card.suit == -1 ) continue;
    card.sticker = card.kind;
    push_back( p2->handcard, card );
  }
  for ( int i = 0; i < p1->shield->size; i++ ) {
    Card card = pop_back( p1->shield );
    if ( card.suit == -1 ) continue;
    card.sticker = card.kind;
    push_back( p2->handcard, card );
  }
  for ( int i = 0; i < p1->distance_item->size; i++ ) {
    Card card = pop_back( p1->distance_item );
    if ( card.suit == -1 ) continue;
    card.sticker = card.kind;
    push_back( p2->handcard, card );
  }

  // assert p1->id == -1 or p1->id == some players.id
  if ( p1->id >= 0 && p1->id < PLAYERS_NUM  ) {
    Player *player = &(PLAYERS_LIST[p1->id]);
    if ( player->state == IS_DEAD ) return;
    skills[player->role]( player );  
  }
}

// 回傳此種sticker 種類卡牌位置，沒有則回傳-1
int find_sticker( Card_vector *card, Kind kind ) {
    for(int i = 0; i < card->size; i++) {
        if( card->data[i].sticker == kind ) {
            if( card->data[i].suit >= 1 && card->data[i].suit <= 13 )
                return i;
        }
    }
    return -1;
}

int find_position( int id ) {
  
  for(int i=0; i<PLAYERS_NUM; i++ ) {
     Player *p = PLAYERS_LIST + i;
      if(p->id == id ) {
        return i;
      }
  }

  return -1;
}

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

void calcDistance() {
  int tmpArr[10][10] = {0};
  for(int i=0;i<ALIVE_NUM;i++)
  {
    for(int j=0; j<ALIVE_NUM; j++)
    {
      int tmp = 0;
      if(i>=j)
      {
        tmp = (ALIVE_NUM-i)+(j-0);
      }
      else
      {
        tmp = (i-0)+(ALIVE_NUM-j);
      }
      tmpArr[i][j] = min(abs(i-j),tmp);
    }
  }

  int k = 0, l = 0;
  for ( int i = 0, k = 0; i < PLAYERS_NUM; i++ ) {
    Player *p = PLAYERS_LIST + i;
    if ( p->state == IS_DEAD ) {
      memset( DISTANCE[i], 0, 10 );
      memset( OFFSET_DISTANCE[i], 0, 10 );
      continue;
    }
    for ( int j = 0, l = 0; j < PLAYERS_NUM; j++ ) {
      p = PLAYERS_LIST + j; 
      if ( p->state == IS_DEAD ) {
        DISTANCE[i][j] = 0;
        OFFSET_DISTANCE[i][j] = 0;
      } 
      else {
        DISTANCE[i][j] = OFFSET_DISTANCE[i][j] + tmpArr[k][l];
        l++;
      }
    }
    k++;
  }

}

bool judgeFunc( Player *player, int kind ) {
    Card card = getJudgementCard( player );
    printf( "%sJudge card:%s\n", RESET, RESET );

    if ( kind == JAIL ) {
        if ( card.suit == 2 ){
          printCard( card, GREEN );
          return true; // 玩家越獄成功，棄掉監獄繼續進行回合
        } 
        else {
          printCard( card, RED );
          return false; // 棄掉監獄並暫停一回合
        }
    }
    if ( kind == DYNAMITE ) {
      if ( card.suit == 1 && ( card.number >= 2 && card.number <= 9 ) ) {
        printCard( card, RED );
        return false; // 玩家立刻損失3點血量並棄掉此牌繼續進行回合
      }
        
      else {
        printCard( card, GREEN );
        return true; // 將炸藥傳給左手邊的玩家
      }
    }

    if ( kind == BARREL ) {
      if ( card.suit == 2 ) {
        printCard( card, GREEN );
        return true;
      }
      else {
        printCard( card, RED );
        return false;
      }
    }

    return false;
}