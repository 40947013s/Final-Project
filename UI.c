#include "UI.h"


void printHandCard( Card_vector *cards, int color[], bool visible ) {
    if ( isEmpty( cards ) ) {
        printf("There is no card\n");
        return;
    }
    puts("");
    printf( "%-12s", "Card" );
    char *tmp, tmp2[5];
    tmp = (char *)calloc( strlen("Card") + 6, sizeof(char) );    
    for ( int i = 1; i <= cards->size; i++ ) {
        strcpy( tmp, "Card" );    
        sprintf( tmp2, "%d", i );
        strcat( tmp, tmp2 );
        printf( "%s%-12s%s", Color[color[i-1]], tmp, Color[0] );
    }
    free( tmp );

    puts("");
    printf( "%-12s", "Suit" );
    for (int i = 0; i < cards->size; i++) {
        Card card = cards->data[i];
        printf("%s%-12s%s", Color[color[i]], (visible) ? suitName[card.suit] : "*****", Color[0]);
    }
    puts("");

    printf( "%-12s", "Number" );
    for (int i = 0; i < cards->size; i++) {
        Card card = cards->data[i];
        printf("%s%-12d%s", Color[color[i]], (visible) ? card.number : 0, Color[0] );
    }
    puts("");

    printf( "%-12s", "Kind" );
    for (int i = 0; i < cards->size; i++) {
      Card card = cards->data[i];
      printf("%s%-12s%s", Color[color[i]], (visible) ? cardKindName[card.kind] : "*****", Color[0] );
    }

    puts("");
  
    int now_player_pos = find_position( cards->id );
    if ( PLAYERS_LIST[now_player_pos].role == Calamity_Janet ) {
      printf( "%-12s", "Sticker" );
      for (int i = 0; i < cards->size; i++) {
        Card card = cards->data[i];
        printf("%s%-12s%s", Color[color[i]], (visible) ? cardKindName[card.sticker] : "*****", Color[0] );
      }
      puts("");
    }
    
}

// 若不指定index,kind,attribute，則填入-1
// if index == -1 and kind == -1 and attribute == -1, then  all the card will be colored
int setColor( int **c, int index, int kind, int attribute, Card_vector* cards, int color ) {
  if ( cards == NULL || isEmpty( cards ) ) {
    return 0;
  }
  
  int num = 0;
  if ( *c == NULL ) {
    *c = (int*)calloc( cards->size, sizeof(int));
    memset( *c, 0, cards->size * sizeof(int) );
  }

  if ( index == -1 && kind == -1 && attribute == -1 ) {
    for ( int i = 0; i < cards->size; i++ ) {
      Card tmp = get_element( cards, i );
      if ( tmp.suit == -1  ) (*c)[i] = 8;
      else (*c)[i] = color;
    }
    return cards->size;
  }

  if ( index != -1 ) {
    Card tmp = get_element( cards, index );
    if ( tmp.suit == -1 ) (*c)[index] = 8;
    else (*c)[index] = color;
      num++;
  }
  if ( kind != -1 ) {
    for ( int i = 0; i < cards->size; i++ ) {
      Card tmp = get_element( cards, i );
      if ( tmp.sticker == kind || tmp.kind == kind ) {
        if ( tmp.suit == -1 ) (*c)[i] = 8;
        else (*c)[i] = color;
        num++;
      }
    }
  }
  if ( attribute != -1 ) {
    for ( int i = 0; i < cards->size; i++ ) {
      Card tmp = get_element( cards, i );  
      if ( cards->data[i].attribute == attribute ) {
        if ( tmp.suit == -1 ) (*c)[i] = 8;
        else (*c)[i] = color;
        num++;
      }
    }
  }

  return num;
}


int setPlayerColor( int **c, int limit_distance, Player *player, int index, int color, bool is_default ) {
  if ( player == NULL  ) return 0;
  
  int num = 0;
  if ( *c == NULL ) {
    *c = (int*)calloc( PLAYERS_NUM, sizeof(int));
    memset( *c, 0, PLAYERS_NUM * sizeof(int) );
  }

  if ( index != -1 ) {
    ASSERT( index >= 0 && index < PLAYERS_NUM );
    (*c)[index] = color;
    return 1;
  }

  (*c)[SHERIFF_POSITION] = 4;
  for ( int i = 0; i < PLAYERS_NUM; i++ ) {
    Player *p = PLAYERS_LIST + i;
    if ( p->state == IS_DEAD )
      (*c)[i] = 8;
    else if ( p->id == player->id ) 
      (*c)[i] = 2;
  }
  
  if ( is_default ) return 0;


  if ( limit_distance == -1 ) {
    for ( int i = 0; i < PLAYERS_NUM; i++ ) {
      Player *p = PLAYERS_LIST + i;
      if ( p->state == IS_DEAD || p->id == player->id )
        continue;
      else {
        (*c)[i] = color;
        num++;
      }
        
    }
    return num;
  }

  for ( int i = 0; i < PLAYERS_NUM; i++ ) {
    Player *p = PLAYERS_LIST + i;
    int distance = DISTANCE[player->id][p->id];
    if ( p->state == IS_DEAD || p->id == player->id )
      continue;
    else if ( distance <= limit_distance ) {
      (*c)[i] = color;  
      num++;
    }
      
  }


  return num;
}


void printUI2( Player *nowPlayer, int *color, char *msg ) 
{
  
  system("clear");
  for ( int i = 0; i < PLAYERS_NUM + 2; i++ ) printf( "---------------" );
  puts("");
  
  printf( "%s%-15s%s", GRAY_BACK, "ID", RESET );
  for ( int i = 0; i < PLAYERS_NUM; i++ ) {
      printf( "%s%-18d%s", GRAY_BACK, PLAYERS_LIST[i].id+1, RESET );
  }
  puts("");

  printf( "%-15s", "Name" );
  for (int i = 0; i < PLAYERS_NUM; i++) {
    printf("%s%-18s%s", Color[color[i]], PLAYERS_LIST[i].name, RESET );
  }
      
  puts("");

  printf( "%-15s", "HP" );
  for (int i = 0; i < PLAYERS_NUM; i++){
    printf("%s%-18d%s", Color[color[i]], PLAYERS_LIST[i].hp, RESET);
  }
  puts("");

  printf( "%-15s", "HandCard" );
  for (int i = 0; i < PLAYERS_NUM; i++) {
    printf("%s%-18d%s", Color[color[i]], PLAYERS_LIST[i].handcard->size, RESET);
  }
  puts("");

  printf( "%-15s", "Role" );
  for (int i = 0; i < PLAYERS_NUM; i++) {
    printf("%s%-18s%s", Color[color[i]], roleName[PLAYERS_LIST[i].role], RESET);
  }
      
  puts("");

  printf( "%-15s", "Distance" );
  
  for (int i = 0; i < PLAYERS_NUM; i++) {
    int id = PLAYERS_LIST[i].id;
    printf("%s%-18d%s", Color[color[i]], DISTANCE[nowPlayer->id][id], RESET);
  }
  puts("");

  printf("%-15s", "Identity");
  for (int i = 0; i < PLAYERS_NUM; i++) {
  #ifdef DEBUG
    
    printf("%s%-18s%s", Color[color[i]], identityName[PLAYERS_LIST[i].identity], RESET);
    
  #endif

  #ifndef DEBUG
    if ( PLAYERS_LIST[i].state == IS_DEAD || GAME_STATE == END )
      printf("%s%-18s%s", Color[color[i]], identityName[PLAYERS_LIST[i].identity], RESET);
    else if ( PLAYERS_LIST[i].identity == Sheriff || ( i == nowPlayer->id && !nowPlayer->isAI ) )
      printf("%s%-18s%s", Color[color[i]], identityName[PLAYERS_LIST[i].identity], RESET );
    else
      printf("%s%-18s%s", Color[color[i]], "*****", RESET );
  #endif

  }
  puts("");

  printf("%-15s", "State");
  for (int i = 0; i < PLAYERS_NUM; i++) {
    printf("%s%-18s%s", Color[color[i]], stateName[PLAYERS_LIST[i].state], RESET);
  }
  puts("");

  printf("%-15s", "Attack");
  for (int i = 0; i < PLAYERS_NUM; i++) {
    printf("%s%-18d%s", Color[color[i]], PLAYERS_LIST[i].attack_distance, RESET);
  }
  puts("");

  
  for ( int i = 0; i < PLAYERS_NUM + 2; i++ ) printf( "---------------" );
  puts("");
  

  printf( "%-15s", "Weapon" );
  for (int i = 0; i < PLAYERS_NUM; i++) {
    if ( isEmpty(PLAYERS_LIST[i].weapon) )
      printf("%s%-18s%s", RESET, "None", RESET );
    else
      printf("%s%-18s%s", YELLOW, cardKindName[get_element(PLAYERS_LIST[i].weapon, 0).kind], RESET );

  }
  puts("");

  printf( "%-15s", "Shield" );
  for (int i = 0; i < PLAYERS_NUM; i++) {
    if ( isEmpty(PLAYERS_LIST[i].shield) )
      printf("%s%-18s%s", RESET, "None", RESET );
    else if ( get_element(PLAYERS_LIST[i].shield, 0).suit == -1 && PLAYERS_LIST[i].shield->size == 1 ) 
      printf("%s%-18s%s", GRAY, cardKindName[get_element(PLAYERS_LIST[i].shield, 0).kind], RESET );
    else
      printf("%s%-18s%s", YELLOW, cardKindName[get_element(PLAYERS_LIST[i].shield, 0).kind], RESET );
  }
  puts("");

  printf( "%-15s", "Scope" );
  for (int i = 0; i < PLAYERS_NUM; i++) {
    if ( PLAYERS_LIST[i].equipScope == NONE )
      printf("%s%-18s%s", RESET, "None", RESET );
    else
      printf("%s%-18s%s", YELLOW, cardKindName[PLAYERS_LIST[i].equipScope], RESET );
  }
  puts("");

  printf( "%-15s", "Mustang" );
  for (int i = 0; i < PLAYERS_NUM; i++) {
    if ( PLAYERS_LIST[i].equipMustang == NONE )
      printf("%s%-18s%s", RESET, "None", RESET );
    else
      printf("%s%-18s%s", YELLOW, cardKindName[PLAYERS_LIST[i].equipMustang], RESET );
  }
  puts("");

  
  bool still_have_card = true;
  for ( int j = 0; still_have_card; j++ ) {
    printf( "%-15s", "Judge Card" );
    still_have_card = false;
    for (int i = 0; i < PLAYERS_NUM; i++) {
      if ( j >= PLAYERS_LIST[i].judgeCards->size )
        printf("%s%-18s%s", RESET, "None", RESET );
      else {
        printf("%s%-18s%s", YELLOW, cardKindName[get_element(PLAYERS_LIST[i].judgeCards, j).kind], RESET );
        if ( j+1 < PLAYERS_LIST[i].judgeCards->size ) still_have_card = true;
        
      }
    
    }
    puts( "" );
  }

  for ( int i = 0; i < PLAYERS_NUM + 2; i++ ) printf( "---------------" );
  puts("");
  

  printf( "Now turn: %s\t\tDeck number: %d\t\tDiscard Pile number: %d\n\n", nowPlayer->name, deck->size, discardPile->size );
  printf( "%s\n", msg );

}

void printUI( Player *player, char *msg ) {
  int *color = NULL;
  setPlayerColor( &color, -1, player, -1, 0, true );
  printUI2( player, color, msg );
  free( color );
}

void print_player(int th)
{   
    printf("Player %d\n", th+1);
    printf("Name : %s\n", PLAYERS_LIST[th].name);
    printf("Role : %d\n", PLAYERS_LIST[th].role);
    printf("State : %d\n", PLAYERS_LIST[th].state);

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

void printCard( Card card, char *color ) 
{
  // printf( "is_orange: %d\n", card.is_orange );
  // printf( "attribute: %d\n", card.attribute );
  printf( "%ssuit: %s%s\n", color, suitName[card.suit], RESET );
  printf( "%snumber: %d%s\n", color, card.number, RESET );
  printf( "%skind: %s%s\n", color, cardKindName[card.kind], RESET );
  printf( "%sSticker: %s%s\n", color, cardKindName[card.sticker], RESET );
}

void gameStartUI() {
    printf("\n");
    printf("====================================================================================\n");
    printf("||                                                                                ||\n");
    printf("||                                                                                ||\n");
    printf("||                           Welcome to the game Bang!                            ||\n");
    printf("||                                                                                ||\n");
    printf("||                                     _____                                      ||\n");
    printf("||                                   __|   |__                                    ||\n");
    printf("||                                  |____⍟____|                                   ||\n");
    printf("||                                   *  | |  *                                    ||\n");
    printf("||                                    *******                                     ||\n");
    printf("||                                                                                ||\n");
    printf("||                                                                                ||\n");
    printf("||                                                                                ||\n");
    printf("||    __________          _______      ___               __________      ___      ||\n");
    printf("||    |         |        |       |     |  |         |   |               |   |     ||\n");
    printf("||    |         |       |         |    |   |        |   |               |   |     ||\n");
    printf("||    |==========|     |           |   |    |       |   |               |   |     ||\n");
    printf("||    |           |    |===========|   |     |      |   |    ========|  |   |     ||\n");
    printf("||    |           |   |             |  |      |     |   |            |  |   |     ||\n");
    printf("||    |           |   |             |  |       |    |   |            |   | |      ||\n");
    printf("||    |___________|   |             |  |        |___|   |____________|  |___|     ||\n");
    printf("||                                                                                ||\n");
    printf("||                                                                                ||\n");
    printf("||                                                                                ||\n");
    printf("||                        ---------------------------------                       ||\n");
    printf("||                          Press Enter To Start The Game                         ||\n");
    printf("||                        ---------------------------------                       ||\n");
    printf("||                                                                                ||\n");
    printf("||                                                                                ||\n");
    printf("====================================================================================\n");
   
    while( getchar() != '\n' );
    system("clear");
}

void gameEndUI() {
    printf("\n");
    printf("====================================================================================\n");
    printf("||                                                                                ||\n");
    printf("||                                                                                ||\n");
    printf("||                               Thanks for playing!                              ||\n");
    printf("||                                                                                ||\n");
    printf("||                                     _____                                      ||\n");
    printf("||                                   __|   |__                                    ||\n");
    printf("||                                  |____⍟____|                                   ||\n");
    printf("||                                   *  | |  *                                    ||\n");
    printf("||                                    *******                                     ||\n");
    printf("||                                                                                ||\n");
    printf("||                                                                                ||\n");
    printf("||                                                                                ||\n");
    printf("||    __________          _______      ___               __________      ___      ||\n");
    printf("||    |         |        |       |     |  |         |   |               |   |     ||\n");
    printf("||    |         |       |         |    |   |        |   |               |   |     ||\n");
    printf("||    |==========|     |           |   |    |       |   |               |   |     ||\n");
    printf("||    |           |    |===========|   |     |      |   |    ========|  |   |     ||\n");
    printf("||    |           |   |             |  |      |     |   |            |  |   |     ||\n");
    printf("||    |           |   |             |  |       |    |   |            |   | |      ||\n");
    printf("||    |___________|   |             |  |        |___|   |____________|  |___|     ||\n");
    printf("||                                                                                ||\n");
    printf("||                                                                                ||\n");
    printf("||                                                                                ||\n");
    printf("||                        ---------------------------------                       ||\n");
    printf("||                                   GAME OVER                                    ||\n");
    printf("||                        ---------------------------------                       ||\n");
    printf("||                                                                                ||\n");
    printf("||                                                                                ||\n");
    printf("====================================================================================\n");
   
    puts( "" );
    ENTER;
    system("clear");
}