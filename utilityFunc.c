#include "utilityFunc.h"

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
  if ( player == NULL ) return 0;
  if ( num >= deck->size ) {
      shuffle();
  }    

  int i = 0;
  while ( i < num ) {
      if ( deck->size == 0 ) return i;
      Card tmp = pop_back(deck);
      if ( tmp.suit == -1 || tmp.number == -1 ) continue;
      tmp.sticker = tmp.kind;
      push_back(player->handcard, tmp);
      i++;
  }

  
  if ( player->state == IS_DEAD ) return true;
  
  int tmpstate = player->state;
  player->state = AFTER_GET;

  if ( SKILL_RANGE(player->role) )
      skills[player->role]( player ); 
  
  player->state = tmpstate;
  
  return num;
}

bool discardCard( Card_vector * cards, int index ) {
  if ( cards == NULL ) return false;
  if ( isEmpty( cards ) ) return false;
  Card tmp = get_element( cards, index);
  remove_element((cards), index);
  // 如果是內建則直接丟棄
  if( tmp.suit == -1 || tmp.number == -1 ) {
      return true;
  }
  tmp.sticker = tmp.kind;
  push_back(discardPile, tmp);

  // assert cards->id == -1 or cards->id == some players.id
  if ( cards->id >= 0 && cards->id < PLAYERS_NUM  ) {
    Player *player = &(PLAYERS_LIST[cards->id]);
    if ( player->state == IS_DEAD ) return true;
    
    int tmpstate = player->state;
    player->state = DISCARD_CARD;
    ASSERT( SKILL_RANGE(player->role) );
    if ( SKILL_RANGE(player->role) )
      skills[player->role]( player ); 
    player->state = tmpstate;
  }
    
  return true;
}

// 棄掉所有的牌，包含手牌、装備
void discardAllCard( Player *player ) {
  if ( player == NULL ) return;
  while ( !isEmpty( player->handcard ) ) {
    Card tmp = pop_back(player->handcard);
    if ( tmp.suit == -1 || tmp.number == -1 ) continue;
    tmp.sticker = tmp.kind;
    push_back(discardPile, tmp);
  }

  // bool UnloadEquip( Player* player, int kind )
  while ( !isEmpty( player->weapon ) ) {
    Card tmp = pop_back(player->weapon);
    if ( tmp.suit == -1 || tmp.number == -1 ) continue;
    UnloadEquip( player, tmp.kind );
    tmp.sticker = tmp.kind;
    push_back(discardPile, tmp);
  }

  player->equipWeapon = NONE;
  while ( !isEmpty(player->shield) ) {
    Card tmp = pop_back(player->shield);
    if ( tmp.suit == -1 || tmp.number == -1) continue;
    UnloadEquip( player, tmp.kind );
    tmp.sticker = tmp.kind;
    push_back(discardPile, tmp);
  }

  while ( !isEmpty( player->distance_item ) ) {
    Card tmp = pop_back(player->distance_item);
    if ( tmp.suit == -1 || tmp.number == -1) continue;
    UnloadEquip( player, tmp.kind );
    tmp.sticker = tmp.kind;
    push_back(discardPile, tmp);
  }


  while ( player->state == IS_DEAD && !isEmpty( player->judgeCards ) ) {
    Card tmp = pop_back(player->judgeCards);
    if ( tmp.suit == -1 || tmp.number == -1 ) continue;
    tmp.sticker = tmp.kind;
    push_back(discardPile, tmp);
  }

  // ASSERT( player->state == IS_DEAD );
  if ( player->state == IS_DEAD ) return;
  
  ASSERT( SKILL_RANGE(player->role) );
  if ( SKILL_RANGE(player->role) )
    skills[player->role]( player );
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

int scan( int min, int max, char *str, bool isAI )
{

  if ( isAI ) {
    printf("%s", str);
    printf( "Choosing...\n" );
    sleep( waitTime );
    int ans = rand() % (max - min + 1) + min;
    #ifdef DEBUG
      printf( "Player choice: %d\n", ans );
      ENTER;
    #endif
    return ans;
  }

  int warn = 0, choice = -1;
  char *input = malloc(1000);
  while(1) {
    if(warn) printf("Wrong input.\n\n");
    printf("%s", str);
    fgets(input, 1000, stdin);
    clean_buffer(input);
    choice = strtol(input, NULL , 10);
    // if( strcmp( input, "0") != 0 && choice == 0 ) {
      warn++;            
    //   continue;
    // }
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

  /// assert p1->id == -1 or p1->id == some players.id
  Player *player = NULL;
  if ( p1->id >= 0 && p1->id < PLAYERS_NUM )
    player = &(PLAYERS_LIST[p1->id]);
  
  if ( player != NULL && player->state != IS_DEAD ) {
    int tmpstate = player->state;
    player->state = DISCARD_CARD;
    ASSERT( SKILL_RANGE(player->role) );
    if ( SKILL_RANGE(player->role) )
      skills[player->role]( player ); 
    player->state = tmpstate;
  }

  if ( p2->id >= 0 && p2->id < PLAYERS_NUM )
    player = &(PLAYERS_LIST[p1->id]);
  
  if ( player != NULL && player->state != IS_DEAD ) {
    Player *player = &(PLAYERS_LIST[p2->id]);
    int tmpstate = player->state;
    player->state = AFTER_GET;
    ASSERT( SKILL_RANGE(player->role) );
    if ( SKILL_RANGE(player->role) )
      skills[player->role]( player ); 
    player->state = tmpstate;
  }

  return true;
}

// 把player 1 所有的牌給 player 2，包含手牌、装備
void takeAllCards( Player *p1, Player *p2 ) {
  if ( p1 == NULL || p2 == NULL ) return;
  if ( p2->state == IS_DEAD ) return;
  while ( !isEmpty( p1->handcard ) ) {
      Card card = pop_back( p1->handcard );
      if ( card.suit == -1 ) continue;
      card.sticker = card.kind;
      push_back( p2->handcard, card );
  }
  
  while ( !isEmpty( p1->weapon ) ) {
    Card card = pop_back( p1->weapon );
    if ( card.suit == -1 ) continue;
    UnloadEquip( p1, card.kind );
    card.sticker = card.kind;
    push_back( p2->handcard, card );
  }

  while ( !isEmpty( p1->shield ) ) {
    Card card = pop_back( p1->shield );
    if ( card.suit == -1 ) continue;
    UnloadEquip( p1, card.kind );
    card.sticker = card.kind;
    push_back( p2->handcard, card );
  }

  while ( !isEmpty( p1->distance_item ) ) {
    Card card = pop_back( p1->distance_item );
    if ( card.suit == -1 ) continue;
    UnloadEquip( p1, card.kind );
    card.sticker = card.kind;
    push_back( p2->handcard, card );
  }


  
  /// assert p1->id == -1 or p1->id == some players.id
  if ( p1->id >= 0 && p1->id < PLAYERS_NUM && p1->state != IS_DEAD ) {
    Player *player = &(PLAYERS_LIST[p1->id]);
    int tmpstate = player->state;
    player->state = DISCARD_CARD;
    ASSERT( SKILL_RANGE(player->role) );
    if ( SKILL_RANGE(player->role) )
      skills[player->role]( player ); 
    player->state = tmpstate;
  }

  // assert p1->id == -1 or p1->id == some players.id
  if ( p2->id >= 0 && p2->id < PLAYERS_NUM && p2->state != IS_DEAD ) {
    Player *player = &(PLAYERS_LIST[p2->id]);
    int tmpstate = player->state;
    player->state = AFTER_GET;
    ASSERT( SKILL_RANGE(player->role) );
    if ( SKILL_RANGE(player->role) )
      skills[player->role]( player ); 
    player->state = tmpstate;
  }

  
}

// 回傳此種sticker 種類卡牌位置，沒有則回傳-1
int find_sticker( Card_vector *card, Kind kind ) {
    for(int i = 0; i < card->size; i++) {
        if( card->data[i].sticker == kind || card->data[i].kind == kind ) {
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

// if(抽牌判定) 可以抽兩張挑一張
Card getJudgementCard( Player *player, int kind ) {
  
  Player *tmpPlayer = malloc( sizeof(Player) * 1 );
  tmpPlayer->handcard = create_vector( 5 );
  cardHandler( tmpPlayer, 1 );
  
  Card c = get_element( tmpPlayer->handcard, 0 );
  if ( player == NULL || player->role != Lucky_Duke ) {
    discardCard( tmpPlayer->handcard, 0 );
    
    delete_vector( tmpPlayer->handcard );
    free( tmpPlayer );
    return c;
  }

  ASSERT( player->role == Lucky_Duke );
  while ( !isEmpty( player->judgeCards ) ) {
    printf( "Active Lucky Duke's skill\n" );
    printf( "You can draw two cards and choose one as your decision\n" );
    ENTER;
    // Card card = pop_back( player->judgeCards );
    cardHandler( tmpPlayer, 1 );
    Card card1 = get_element( tmpPlayer->handcard, 0 );
    Card card2 = get_element( tmpPlayer->handcard, 1 );
    discardCard( tmpPlayer->handcard, 0 );
    discardCard( tmpPlayer->handcard, 1 );


    bool judge1 = false, judge2 = false;
    switch ( kind ){
      case BARREL:
        if ( card1.suit == 2 ) judge1 = true;
        if ( card2.suit == 2 ) judge2 = true;
        break;
      case DYNAMITE:
        if ( !( card1.suit == 1 && ( card1.number >= 2 && card1.number <= 9 ) ) ) judge1 = true;
        if ( !( card2.suit == 1 && ( card2.number >= 2 && card2.number <= 9 ) ) ) judge2 = true;
        break;
      case JAIL:
        if ( card1.suit == 2 ) judge1 = true;
        if ( card2.suit == 2 ) judge2 = true;
        break;
    }
    
    puts( "card1: " );
    printCard( card1, judge1 ? GREEN : RED );
    puts( "card2: " );
    printCard( card2, judge2 ? GREEN : RED );
  
    char *str = "Choose the card you want to use as a decision card ( 1 or 2 ): ";
    int choice = scan(1, 2, str, player->isAI );
    printf( "You choose the card%d\n", choice );

    delete_vector( tmpPlayer->handcard );
    free( tmpPlayer );
    return ( choice == 1 ) ? card1 : card2;
  } 
  
  delete_vector( tmpPlayer->handcard );
  free( tmpPlayer );
  return c;
}

bool judgeFunc( Player *player, int kind ) {
    Card card = getJudgementCard( player, kind );
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

void renewRelationdhip( Player *attacker, Player *defender ) {
  if ( attacker == NULL || defender == NULL ) return;
  int attacker_id = attacker->id, defender_id = defender->id;

  if ( attacker_id < 0 || attacker_id >= PLAYERS_NUM || defender_id < 0 || defender_id >= PLAYERS_NUM ) return;

  RELATIONSHIP[defender_id][attacker_id]--;
  
  for ( int i = 0; i < PLAYERS_NUM; i++ ) {
    if ( i == attacker_id || i == defender_id ) continue;
    Player *p = PLAYERS_LIST + i;
    if ( p->identity == Deputies && defender->identity == Sheriff ) {
      RELATIONSHIP[p->id][attacker_id] -= 3;
    }
    else if ( p->identity == Outlaws && defender->identity == Sheriff ) {
      RELATIONSHIP[p->id][attacker_id] += 3;
    }
    else {
      if ( RELATIONSHIP[p->id][defender_id] > 0 ) {
        RELATIONSHIP[p->id][attacker_id]--;
      }
      else if ( RELATIONSHIP[p->id][defender_id] < 0  ) {
        RELATIONSHIP[p->id][attacker_id]++;
      }
    }
  }
  
}

