#include "cardFunc.h"

void HPModify( Player* attacker, Player *defender, int n, Kind reason ) {
  if ( n < 0 ) {    
    RMode tmp = defender->state;
    defender->state = MINUS_HP;
    skills[defender->role]( defender );
    fEl_Gringo( defender, attacker, n, reason );
    printf( "%s minus %d hp.\n", defender->name, -n );
    ENTER;
    defender->state = tmp;
    defender->hp += n;
  }
  else {
    printf( "%s plus %d hp.\n", defender->name, n );
    ENTER;
    defender->hp += n;
  }

  IsGameOver( attacker, defender );
}

// if killer is null, then there is no killer
// e.g. if the player is killed by dynamite, there is no killer
void IsGameOver( Player *killer, Player *player ){
  if ( player->hp <= 0 && ALIVE_NUM > 2 ) {
// bool Beer( Player *player, killer ); 
    
    printf( "You are dying\n" );
    printf( "Are you going to use beer to add up hp?\n" );
    int choice = scan( 0, 1, "(1: yes, 0: no) " );
    if ( choice == 1 ) {
      Card beer = chooseCard( player, player->handcard, BEER, NULL, false,  true );
      if ( beer.number != -1 ) {
          Beer( player, killer );
          return;
      } else {
          puts( "You didn't have beer" );
          ENTER;
      }
    } 
  }


  if ( player->hp <= 0 ) {
      
    player->state = IS_DEAD;
    ALIVE_NUM--;

    // remove the player from the player list
    int k = find_position(player->id);  
    calcDistance();
    

    printUI( player, "" );
    printf( "Player %s is dead and his identity is %s\n", player->name, identityName[player->identity] );
    ENTER;
    switch ( player->identity ) {
      case Sheriff:
        SHERIFF_NUM--;
        break;
      case Deputies:
        DEPUTIES_NUM--;
        break;
      case Outlaws:
        OUTLAWS_NUM--;
        break;
      case Renegade:
        RENEGADE_NUM--;
        break;
    }
      
    
    // 判斷遊戲是否結束
    if( SHERIFF_NUM + DEPUTIES_NUM + OUTLAWS_NUM == 0 ) {
        GAME_STATE = END;
        // set winner(叛徒)
        puts( "Renegade win : " );    
        for(int i = 0; i < PLAYERS_NUM; i++) {
            if( PLAYERS_LIST[i].identity == Renegade ) {
                printf("Player %s\n", PLAYERS_LIST[i].name);  
            }
        }
    } else if ( SHERIFF_NUM == 0 ) {
        GAME_STATE = END;
        // set winner(歹徒)
        puts( "Outlaws win : " );    
        for(int i = 0; i < PLAYERS_NUM; i++) {
            if( PLAYERS_LIST[i].identity == Outlaws ) {
                printf("Player %s\n", PLAYERS_LIST[i].name);  
            }
        }
    } else if ( OUTLAWS_NUM + RENEGADE_NUM == 0 ) {
        GAME_STATE = END;
        // set winner(警長副警長)
        puts( "Sheriff and Deputies win : " );    
        printf("Player %s\n", PLAYERS_LIST[SHERIFF_POSITION].name);  //警長            
        for(int i = 0; i < PLAYERS_NUM; i++) {
            Player p = PLAYERS_LIST[i];      
            if( p.identity == Deputies ) {
                printf("Player %s\n", PLAYERS_LIST[i].name);              
            }
        } 
    }

    if ( GAME_STATE == END ) {
      // puts( "Game Over" );
      // ENTER;
      // #define DEBUG
      printUI( player, "Game Over" );
      exit(0);
      // return;
    }

    if ( killer != NULL && killer->state != IS_DEAD ) { 
      if ( player->identity == Outlaws ) {
        cardHandler( killer, 3 );
      }
      // discardAllCard( killer );
    }

    if ( player->identity == Deputies && killer->identity == Sheriff ) {
      printUI( killer, "" );
      puts( "You killed your deputy!" );
      puts( "You have to discard all you handcards and equipments" );
      ENTER;
      discardAllCard( killer );
    }

    for ( int i = 0; i < PLAYERS_NUM; i++ ) {
      if ( PLAYERS_LIST[i].role == Vulture_Sam && PLAYERS_LIST[i].state != IS_DEAD ) {
        printUI( PLAYERS_LIST + i, "" );
        printf( "Active Vulture_Sam's skill\n" );
        puts( "You can take all the cards from the deceased" );
        ENTER;
        takeAllCards( player, &PLAYERS_LIST[i] );
        printUI( PLAYERS_LIST + i, "" );
        puts( "Take!" );
        ENTER;
      }
    }
    
    discardAllCard( player );
  }
}

// 選擇攻擊對象
// if limitDistance == -1 then 沒有距離限制
Player *choosePlayer( Player *attacker, int limitDistance ) {
  if ( attacker == NULL ) return NULL;
  
  int *color = NULL;
  int num = setPlayerColor( &color, limitDistance, attacker, 3, false );
  if ( num == 0 ) {
    puts( "You can't attack anyone" );
    ENTER;
    return NULL;
  }


  bool warn = false;
  while ( 1 ) {
    
    if ( warn ) 
      printUI2( attacker, color, "You should choose a player to attack\n" );
    else
      printUI2( attacker, color, "Which player to attack\n" );

    int choice = scan( 1, PLAYERS_NUM, "Choose a player: " );
  
    char input[100] = {0};
    
    if ( color[choice-1] == 2 ) {
      puts( "You can't attack yourself" );
      ENTER;
      continue;
    }
    else if ( color[choice-1] == 3 ) {
      color[choice-1] = 1;
      printUI2( attacker, color, "Which player to attack\n" );
      printf( "Are you sure you want to choose this player? (Y/n) " );
      fgets(input, 100, stdin);
      if ( input[0] == 'Y' || input[0] == 'y' || input[0] == '\n'  ) {
        free( color );
        return PLAYERS_LIST + (choice-1);
      }
      else {
        warn = false;
        color[choice] = 3;
        continue;
      }
        
    }
    else {
      warn = true;
      continue;
    }
    
  }
  
  free( color );
  return NULL;
  
} 

// if kind == -1, then all the card can be chosen
// if get_card != NULL, then card -> get_card
// otherwise discard the chosen card
// if except == true, then all the card can be chosen except the kind card
Card chooseCard( Player *player, Card_vector* cards, int kind, Card_vector* get_card, bool except,  bool visible ) {
  Card c;
  c.number = -1;
  if ( player == NULL ) return c;
  if ( cards == NULL ) return c;

  printUI( player, "" );

  if ( isEmpty( cards ) ) {
    printf("There is no card\n");
    ENTER;
    return c;
  }
  int *color = NULL;
  int num = 0;

  if ( !except ) {
    num = setColor( &color, -1, kind, -1, cards, 3 );
  }
  else { // assert except == true
    num = setColor( &color, -1, -1, -1, cards, 3 );
    int tmp = setColor( &color, -1, kind, -1, cards, 0 );
    num -= tmp;
  }
  
  if ( num == 0 ) {
    printHandCard( cards, color, visible );
    printf("No card to choose\n");
    ENTER;
    free( color );
    return c;
  }

  char str[100];
  sprintf( str, "Choose a card (1~%d, 0: quit): ", cards->size );
  int choice = 0;
  bool warn = false;
  char input[100];
  while ( 1 ) {
    printUI( player, "" );
    if ( warn )
      printf( "You can't choose this card\n" );
    printHandCard( cards, color, visible );
    choice = scan(0, cards->size, str );
    if ( choice == 0 ) {
      free( color );
      return c;
    }
    if ( color[choice-1] == 3 ) {
      warn = false;
      color[choice-1] = 1; // green to red
      printUI( player, "" );
      printHandCard( cards, color, visible );
      Card tmp = get_element( cards, choice-1 );
      if ( visible )
        printf( "Are you sure you want to choose the [%s] card? (Y/n) ", cardKindName[tmp.sticker] );
      fgets(input, 100, stdin);
      if ( input[0] == 'Y' || input[0] == 'y' || input[0] == '\n' || !visible ) {
        c = get_element( cards, choice-1 );
        if ( get_card != NULL ) {
          takeCard( cards, get_card, choice-1 );
        }
        else {
          discardCard( cards, choice-1 );
        }
          free( color );
          return c;
      }else {
          color[choice-1] = 3; // red to green
          continue;
      }
    }
    else {
      warn = true;
      continue;
    }
  }

  free( color );
  return c;
}

// orange card

bool Bang( Player *attacker ){
  if ( attacker == NULL ) return false;
  int limitDistance = attacker->attack_distance;
  Player *defender = choosePlayer( attacker, limitDistance );
  if ( defender == NULL ) {
    printf( "No player to attack\n" );
    return false; // 若沒有使用的對象，則不算有使用bang，所以不丟棄那張牌
  }
  printf( "%s is attacked by %s\n", defender->name, attacker->name );

  if ( attacker->role == Slab_the_Killer ) {
    printUI( defender, "" );
    printf( "Active Slab_the_Killer's skill\n" );
    printf( "You have to use two (MISS/BARREL) to avoid his BANG\n" );
    ENTER;
  }
  
  bool miss = Miss( defender, attacker->attack_power );

  printUI( attacker, "" );

  if ( !miss ) {
    printf( "successful attack!\n" );
    HPModify( attacker, defender, -1, BANG );
  }
  else {
    printf( "Missed attack!\n" );
    ENTER;
  }
    

  

  return true;

}


bool Miss( Player *defender, int n ) {
  if ( defender == NULL ) return false;
  
  printUI( defender, "" );
  for ( int i = 0; i < defender->shield->size; i++ ) {

    printf( "%sJudge barrel%s\n\n", BLUE, RESET );
    bool result = judgeFunc( defender, BARREL );
    if ( result )
      printf( "%sJudgment successful%s\n", GREEN, RESET );
    else
      printf( "%sJudgment failed%s\n", RED, RESET );
    ENTER;
    
    if ( result ) n--;
    if ( n == 0 ) return true;
  }

  int numOfMiss = 0;
  for ( int i = 0; i < defender->handcard->size; i++ ) {
    Card card = get_element( defender->handcard, i );
    if ( card.kind == MISSED || card.sticker == MISSED )
      numOfMiss++;
  }

  if ( numOfMiss < n ) {
    printf( "You don't have enough MISSED(s) to use\n" );
    ENTER;
    return false;
  }

  if( scan(0, 1, "Do you want to use card MISSED(s) to avoid attack ? (0 : NO, 1 : YES) \n") == 0 ) {
      return false;
  }


  while ( n ) {
    
    printUI( defender, "" );
    printf( "You still need to throw %d MISSED card(s)\n", n );
 
    bool is_use = chooseCard( defender, defender->handcard, MISSED, NULL, false, true ).number > 0 ? true : false ;
    if ( is_use ) n--;

  }

  printf( "Avoid attack!\n" );
  ENTER;
  return true;
}  

// 所有玩家棄一張bang，沒有棄的扣一滴血
bool Indians( Player *attacker ) {
  if ( attacker == NULL ) return false;
  Player *p;     
  printf("All the others in the game except you have to discard a BANG.\n");
  printf("Or minus 1 hp.\n");
  ENTER;
  int i = attacker->id;
  
  // find thec next player
  do{
    i++;
    if( i >= PLAYERS_NUM ) i = 0;
  } while( PLAYERS_LIST[i].state == IS_DEAD && PLAYERS_LIST[i].id != attacker->id );
  
  do {
    p = PLAYERS_LIST + i;

    bool is_discard_bang = chooseCard( p, p->handcard, BANG, false, false, true ).number > 0 ? true : false; 
    printUI( attacker, "" );
    if ( is_discard_bang ) {
        printf( "%s discard a BANG.\n", p->name );
        ENTER;
    }
    else {
      printf( "successful attack!\n" );
      HPModify( p, p, -1, INDIANS );
      printUI( attacker, "" );
    }

    do{
      i++;
      if( i >= PLAYERS_NUM ) i = 0;
    } while( PLAYERS_LIST[i].state == IS_DEAD );
  } while( i != attacker->id );
  
  return true;
}

bool Gatling( Player *attacker ) {
  if ( attacker == NULL ) return false;
  Player *defender;     
  printf("Attack all players\n");
  ENTER;

  int i = attacker->id;
  // find thec next player
  do{
    i++;
    if( i >= PLAYERS_NUM ) i = 0;
  } while( PLAYERS_LIST[i].state == IS_DEAD && PLAYERS_LIST[i].id != defender->id );
  
  do {
    defender = PLAYERS_LIST + i;

    bool miss = Miss( defender, 1 );  
    printUI( attacker, "" );
    if ( miss ) {
        printf( "%s avoid attack.\n", defender->name );
        printf( "Missed attacked!\n" );
        ENTER;
    }
    else {
        printf( "successful attack!\n" );
        HPModify( attacker, defender, -1, INDIANS );
        printUI( attacker, "" );
    }

    do{
      i++;
      if( i >= PLAYERS_NUM ) i = 0;
    } while( PLAYERS_LIST[i].state == IS_DEAD );
  } while( i != attacker->id );
  
  return true;
}


//玩家從牌庫頂抽兩張牌
bool Stagecoach( Player *player ){
  if ( player == NULL ) return false;
  printf( "Player %s get two cards\n", player->name );
  ENTER;
  cardHandler( player, 2 );
  return true;
}

//玩家從牌庫頂抽三張牌
bool Wells( Player *player ) {
  if ( player == NULL ) return false;
  printf( "Player %s get three cards\n", player->name );
  ENTER;
  cardHandler( player, 3 );
  return true;
}

// 計算目前仍在遊戲中的人數，從牌庫翻開相同數量的牌，每位玩家挑選一張加入自己的手牌中
// 由當回合的玩家開始挑選，以順時鐘方向挑選
bool Store( Player *player ) {
    if ( player == NULL ) return false;
    Player tmpPlayer, *p;
  
    tmpPlayer.handcard = create_vector(10);
    tmpPlayer.state = IS_DEAD;
  
    cardHandler( &tmpPlayer, ALIVE_NUM );
    int i = player->id;
    do {
        p = PLAYERS_LIST + i;
        bool get_card = false;
        bool warn = false;
        while ( !get_card ) {
          if ( warn ) 
            puts( "Please choose a card" );
          get_card = chooseCard( p, tmpPlayer.handcard, -1, p->handcard, false, true ).number > 0 ? true : false ;
          warn = true;
        }
        do {
          i++;
          if ( i == PLAYERS_NUM ) i = 0;
        } while ( PLAYERS_LIST[i].state == IS_DEAD );
        
    } while ( i != player->id );
    return true;
}

// (自己回合 or 瀕死) and 血量不是上限 
// 因為有可能在瀕死時使用，所以須紀錄attacker是誰
bool Beer( Player *player, Player *attacker ){
  if( player == NULL ) return false;
  if( ALIVE_NUM == 2 )
  {
    printf("You can not use Beer when only two players left.\n");
    return false;
  }
  if( player->hp == player->hp_limit ) //已經是上限
  {
    printf("Your hp limit reached.\n");
    return false;
  }

  HPModify( attacker, player, 1, BEER );
  return true;
}

// 對其他所有玩家補一點血量並自己補一點血量，即所有玩家恢復一點血量
// 只能在自己回合使用
bool Saloow( Player *player ){
  if( player == NULL ) return false;

  for(int i=0;i<PLAYERS_NUM;i++)
  {
    printUI( player, "" );
    Player *p = PLAYERS_LIST + i;
    if ( p->state == IS_DEAD )
      continue;
    if( p->hp == p->hp_limit ) //已經是上限
    {
      printf("Player %s's hp limit reached.\n", p->name);
      ENTER;    
    }
    else 
      HPModify( NULL, p, 1, SALOOW );
  }

  return true;
}

// 出不了BANG的一方扣血
// dueler first
bool Duel( Player *attacker ) {
    Player *dueler = choosePlayer( attacker, -1 );
    Player *player, *winner;
    int turn = 2; // 1: attacker, 2: dueler
    bool is_discard_bang = false;
    while(1) {
        if ( turn == 1 ) {
          is_discard_bang = chooseCard( attacker, attacker->handcard, BANG, NULL, false, true ).number > 0 ? true : false ;
          if ( is_discard_bang ) {
            printf( "Player %s discard a BANG.\n", attacker->name );
            printf( "Now is %s's turn\n", dueler->name );
            turn = 2;
            ENTER;
          }
          else {
            printf("Duel Ended.\n");
            printf( "%s won the game.\n", dueler->name );
            HPModify( dueler, attacker, -1, DUEL );
            break;
          }
        }
        else {
          is_discard_bang = chooseCard( dueler, dueler->handcard, BANG, NULL, false, true ).number > 0 ? true : false ;
          if ( is_discard_bang ) {
            printf( "Player %s discard a BANG.\n", dueler->name );
            printf( "Now is %s's turn\n", attacker->name );
            turn = 1;
            ENTER;
          }
          else {
            printf("Duel Ended.\n");
            printf( "%s won the game.\n", attacker->name );
            HPModify( attacker, dueler, -1, DUEL );
            break;
          }
        }
    } 
    return true;
}

// 從距離為1的玩家拿取一張牌
// 野馬和望遠鏡可以改變距離，可以拿取玩家的一張手牌或是玩家面前的裝備或武器，也可以拿場面的炸藥到自己手牌
// 除了監獄 ---> done
bool panic( Player *attacker ){
  if(attacker == NULL) return false;
  Player *choose_player = choosePlayer( attacker, 1 );
  if(choose_player == NULL) return false;
  
  int choice = 0;
  Card card;
  card.number = -1;
  while ( card.number == -1 ) {
    printUI( attacker, "" );
    puts( "Choose the kind of item you want to take" );
    puts( "[0] Quit [1] handcard [2] weapon [3] shield [4] distance item [5] judgement card" );
    choice = scan( 0, 5, "" );
    switch ( choice ) {
      case 0:
        puts( "Give up to use the card" );
        ENTER;
        return false;
      case 1:
        card = chooseCard( attacker, choose_player->handcard, -1, attacker->handcard, false, false );
        break;
      case 2:
        card = chooseCard( attacker, choose_player->weapon, -1, attacker->handcard, false, true );
        break;
      case 3:
        card = chooseCard( attacker, choose_player->shield, -1, attacker->handcard, false, true );
        break;
      case 4:
        card = chooseCard( attacker, choose_player->distance_item, -1, attacker->handcard, false, true );
        break;
      case 5:
        card = chooseCard( attacker, choose_player->judgeCards, JAIL, attacker->handcard, true, true );
        break;
    }
    
  }

  if ( choice != 1 )
     UnloadEquip( choose_player, card.kind );

  printUI( attacker, "" );
  printf( "Get the card[%s] from %s\n", cardKindName[card.kind], choose_player->name );
  ENTER;
  return true;

}

// 要求某玩家棄置手牌或裝備牌
bool cat( Player *attacker ) {
    
  if(attacker == NULL) return false;
  Player *choose_player = choosePlayer( attacker, -1 );
  bool is_choose = false;

  int all_size =  choose_player->handcard->size + choose_player->weapon->size;
  all_size += choose_player->shield->size + choose_player->distance_item->size;
  if( all_size == 0 ) {
      printf( "Player %s didn't have card to discard.\n", choose_player->name );
      ENTER;
      return false;
  }

  Card card;
  card.number = -1;
  int choice = 0;
  while ( card.number == -1 ) {
    printUI( choose_player, "" );
    puts( "Choose the kind of item you want to discard" );
    puts( "[1] handcard [2] weapon [3] shield [4] distance item [5] judgement card" );
    choice = scan( 0, 5, "" );
    switch ( choice ) {
      case 0:
        puts( "Choose again" );
        ENTER;
        return false;
      case 1:
        card = chooseCard( choose_player, choose_player->handcard, -1, NULL, false, true );
        break;
      case 2:
        card = chooseCard( choose_player, choose_player->weapon, -1, NULL, false, true );
        break;
      case 3:
        card = chooseCard( choose_player, choose_player->shield, -1, NULL, false, true );
        if ( card.number != -1 )
          // call function
        break;
      case 4:
        card = chooseCard( choose_player, choose_player->distance_item, -1, NULL, false, true );
        if ( card.number != -1 )
          // call function
        break;
      case 5:
        card = chooseCard( choose_player, choose_player->judgeCards, JAIL, NULL, true, true );
        break;
    }
  }


  if ( choice != 1 )
     UnloadEquip( choose_player, card.kind );

  printUI( attacker, "" );
  printf( "You discard one card[%s] from player %s\n", cardKindName[card.kind], choose_player->name );
  ENTER;
  return true;
}

// blue card
// player 看其他player的距離減一
bool EquipScope( Player *player, int card_position ) {
  if ( player == NULL ) return false;
  // check if player already equip other items
  if ( player->equipScope != NONE ) {
    puts( "Please unload the other scope first" );
    if ( !UnloadScope( player, NULL ) ) {
      puts( "Not equip the scope" );
      return false;
    }
  }

  // handcard 給 distance_item
  takeCard( player->handcard, player->distance_item, card_position );

  int arr_position = find_position( player->id );
  
  for(int i=0;i<PLAYERS_NUM;i++)
  {
    if(i!=arr_position && PLAYERS_LIST[i].state != IS_DEAD)
    {
      DISTANCE[arr_position][i]--;
      OFFSET_DISTANCE[arr_position][i]--;
    }
  }

  player->equipScope = SCOPE;
  puts( "Equip the scope" );
  ENTER;
  return true;
}

// 有時候需要把item拔下來給某人
// if ( player2 != NULL ) then player.item -> cards
// else discard the card
bool UnloadScope( Player *player, Card_vector *cards ){
  if ( player == NULL ) return false;
  
  printUI( player, "" );
  puts( "You are going to unload the scope" );
  ENTER;
  
  bool is_discard = false;
  is_discard = chooseCard( player, player->distance_item, SCOPE, cards, false, true ).number > 0 ? true : false ;
    
  if ( !is_discard ) {
    puts( "Not unload the scope" );
    ENTER;
    return false;
  }
  
  int arr_position = find_position(player->id);
  
  for(int i=0;i<PLAYERS_NUM;i++)
  {
    if(i!=arr_position && PLAYERS_LIST[i].state != IS_DEAD)
    {
      DISTANCE[arr_position][i]++;
      OFFSET_DISTANCE[arr_position][i]++;
    }
  }

  puts( "Scope unloaded" );
  ENTER;
  return true;
}

//所有其他玩家看你的距離加1，自己看其他玩家距離則不變
bool EquipMustang( Player *player, int card_position ){

  if ( player == NULL ) return false;
  // check if player already equip other items
  if ( player->equipMustang != NONE ) {
    puts( "Please unload the other mustang first" );
    if ( !UnloadMustang( player, NULL ) ) {
      puts( "Not equip the mustang" );
      return false;
    }
  }
 
  // player->distance_item = create_vector(1);
  // handcard 給 distance_item 
  takeCard( player->handcard, player->distance_item, card_position );
  
  int arr_position = find_position(player->id);

  for(int i=0;i<PLAYERS_NUM;i++)
  {
    if(i!=arr_position && PLAYERS_LIST[i].state != IS_DEAD)
    {
      DISTANCE[i][arr_position]++;
      OFFSET_DISTANCE[i][arr_position]++;
    }
  }

  player->equipMustang = MUSTANG;
  puts( "Mustang equipped" );
  ENTER;
  return true;
}
    
bool UnloadMustang( Player *player, Card_vector *cards ){
  if ( player == NULL ) return false;

  printUI( player, "" );
  puts( "You are going to unload the mustang" );
  ENTER;
  
  bool is_discard = false;
  if ( cards == NULL ) {
    is_discard = chooseCard( player, player->distance_item, MUSTANG, NULL, false, true ).number > 0 ? true : false ;
  }
  else {
    is_discard = chooseCard( player, player->distance_item, MUSTANG, cards, false, true ).number > 0 ? true : false ;
  }
    
  if ( !is_discard ) {
    puts( "Not unload the mustang" );
    ENTER;
    return false;
  }
  
  int arr_position = find_position(player->id);

  for(int i=0;i<PLAYERS_NUM;i++)
  {
    if(i!=arr_position && PLAYERS_LIST[i].state != IS_DEAD)
    { 
      OFFSET_DISTANCE[i][arr_position]--;
      DISTANCE[i][arr_position]--;
    }
  }

  puts( "Mustang unloaded" );
  ENTER;
  return true;
}


//    BARREL, SCOPE, MUSTANG, 
//    DYNAMITE, VOLCANIC, SCHOFIELD, 
//    REMINGTON, REV, WINCHEDTER, NONE
    
bool UnloadEquip( Player* player, int kind ) {
    if ( player == NULL ) return false;
    switch ( kind ) {
        case BARREL :             
            player->equipShield = NONE;
            return true;
        case SCOPE : 
            player->equipScope = NONE;
            int arr_position = find_position( player->id );  
            for(int i=0;i<PLAYERS_NUM;i++)
            {
                if(i!=arr_position && PLAYERS_LIST[i].state != IS_DEAD)
                {
                  DISTANCE[arr_position][i]++;
                  OFFSET_DISTANCE[arr_position][i]++;
                }
            }
            return true;
        case MUSTANG : 
            player->equipMustang = NONE;
            arr_position = find_position(player->id);
            for(int i=0;i<PLAYERS_NUM;i++)
            {
                if(i!=arr_position && PLAYERS_LIST[i].state != IS_DEAD)
                {
                  DISTANCE[i][arr_position]--;
                  OFFSET_DISTANCE[i][arr_position]--;
                }
            }
            return true;
        case DYNAMITE : 
            return true;
        case VOLCANIC:
            player->equipWeapon = NONE;
            player->numOfBang = (player->role != Willy_the_Kid) ? 1 : -1;  
            return true;
        case SCHOFIELD:
            player->equipWeapon = NONE;
            player->attack_distance = 1; return true;            
        case REMINGTON:
            player->equipWeapon = NONE;
            player->attack_distance = 1; return true;            
        case REV:
            player->equipWeapon = NONE;
            player->attack_distance = 1; return true;            
        case WINCHEDTER:
            player->equipWeapon = NONE;
            player->attack_distance = 1; return true;   
        default:
          return true;
    }
    
}

bool UnloadWeapon( Player* player, Card_vector *cards ) {
  if ( player == NULL ) return false;
  
  if ( player->equipWeapon != NONE ) {
    printUI( player, "" );
    puts( "You are going to unload the weapon" );
    ENTER;
    bool is_discard = false;
 
    if ( player->equipWeapon == VOLCANIC ) {
      is_discard = chooseCard( player, player->weapon, VOLCANIC, cards, false, true ).number > 0 ? true : false ;
      if ( is_discard )
        UnloadVolcanic( player, NULL );
    }
    else if ( player->equipWeapon == SCHOFIELD ) {
      is_discard = chooseCard( player, player->weapon, SCHOFIELD, cards, false, true ).number > 0 ? true : false ;
      if ( is_discard )
        UnloadSchofield( player, NULL );
    }
    else if ( player->equipWeapon == REMINGTON ) {
      is_discard = chooseCard( player, player->weapon, REMINGTON, cards, false, true ).number > 0 ? true : false ;
      if ( is_discard )
        UnloadRemington( player, NULL );
    }
    else if ( player->equipWeapon == REV ) {
      is_discard = chooseCard( player, player->weapon, REV, cards, false, true ).number > 0 ? true : false ;
      if ( is_discard )
        UnloadRev( player, NULL );
    }
    else if ( player->equipWeapon == WINCHEDTER ) {
      is_discard = chooseCard( player, player->weapon, WINCHEDTER, cards, false, true ).number > 0 ? true : false ;
      if ( is_discard )
        UnloadWinchester( player, NULL );
    }

    if ( is_discard ) {
      player->equipWeapon = NONE;
    }
    return is_discard;
  }
  return true;
}

bool EquipWeapon( Player *player, int card_position ) {
  if ( player == NULL ) return false;
  // check if player already equip other items
  if ( player->equipWeapon != NONE ) {
    puts( "Please unload the other weapon first" );
    if ( !UnloadWeapon( player, NULL ) ) {
      return false;
    }
  }
  // handcard 給 distance_item
  takeCard( player->handcard, player->weapon, card_position );
  return true;
}

//可以使用任意張數「砰」
bool EquipVolcanic( Player *player, int card_position ) {

  bool is_equip = EquipWeapon( player, card_position );
  if ( !is_equip ) {
    puts( "Not equip the Volcanic" );
    return false;
  }
  player->equipWeapon = VOLCANIC;

  player->bangLimit = -1;
  puts( "Equip the Volcanic" );
  ENTER;
  return true;
}

bool UnloadVolcanic( Player *player, Card_vector *cards ) {
  if ( player == NULL ) return false;
  // Willy the Kid 的回合可以用任意張 BANG
  
  player->bangLimit = (player->role != Willy_the_Kid) ? 1 : -1; 

  printUI( player, "" );
  puts( "Volcanic unloaded" );
  ENTER;
  return true;
}

//射擊距離變成2
bool EquipSchofield( Player *player, int card_position ){
  bool is_equip = EquipWeapon( player, card_position );
  if ( !is_equip ) {
    puts( "Not equip the Schofield" );
    return false;
  }
  player->equipWeapon = SCHOFIELD;
  player->attack_distance = 2;
  puts( "Equip the Schofield" );
  ENTER;
  return true;
}

bool UnloadSchofield( Player *player, Card_vector *cards ){
  if ( player == NULL ) return false;
  
  player->attack_distance = 1;
  printUI( player, "" );
  puts( "Schofield unloaded" );
  return true;
}

//射擊距離變成3
bool EquipRemington( Player *player, int card_position ){
  bool is_equip = EquipWeapon( player, card_position );
  if ( !is_equip ) {
    puts( "Not equip the Remington" );
    return false;
  }
  player->equipWeapon = REMINGTON;

  player->attack_distance = 3;
  puts( "Equip the Remington" );
  ENTER;
  return true;
}

bool UnloadRemington( Player *player, Card_vector *cards ){
  if ( player == NULL ) return false;
  player->attack_distance = 1;
  printUI( player, "" );
  puts( "Remington unloaded" );
  
  return true;
}

//射擊距離變成4
bool EquipRev( Player *player, int card_position ){
  bool is_equip = EquipWeapon( player, card_position );
  if ( !is_equip ) {
    puts( "Not equip the Rev" );
    return false;
  }
  player->equipWeapon = REV;
  player->attack_distance = 4;
  puts( "Equip the Rev" );
  ENTER;
  return true;
}
bool UnloadRev( Player *player, Card_vector *cards ){
  if ( player == NULL ) return false;
  player->attack_distance = 1;
  printUI( player, "" );
  puts( "Rev unloaded" );
  ENTER;
  return true;
}

//射擊距離變成5
bool EquipWinchester( Player *player, int card_position ){
  bool is_equip = EquipWeapon( player, card_position );
  if ( !is_equip ) {
    puts( "Not equip the Winchester" );
    return false;
  }
  player->equipWeapon = WINCHEDTER;
  player->attack_distance = 5;
  puts( "Equip the Winchester" );
  ENTER;
  return true;
}
bool UnloadWinchester( Player *player, Card_vector *cards ){
  if ( player == NULL ) return false;
  player->attack_distance = 1;
  printUI( player, "" );
  puts( "Winchester unloaded" );
  ENTER;
  return true;
}

bool EquipDynamite( Player *player, int index ) {
  takeCard( player->handcard, player->judgeCards, index );   
  printUI( player, "" );
  puts( "Equip dynamite" );
  ENTER;
  return true;
}

bool EquipJail( Player *player , int index ) {
  Player *choose_player;
  
  choose_player = choosePlayer( player, -1 );
  
  if ( choose_player->identity == Sheriff ) {
    printf( "You can't use jail on the Sheriff\n" );
    ENTER;
    return false;
  }
  
  
  takeCard( player->handcard, choose_player->judgeCards, index );   
  printUI( player, "" );
  puts( "Equip jail" );
  ENTER;
  return true;
}

bool EquipBarrel( Player *player, int index ) {
    if ( player == NULL ) return false;
    bool had_barrel = false;
    for ( int i = 0; i < player->shield->size; i++ ) {
        if( get_element( player->shield, i).suit != -1 ) {
            had_barrel = true;
            break;
        }
    }
    
    if ( had_barrel ) {
        puts( "Please unload the other BARREL first" );
        if ( !UnloadBarrel( player ) ) {
          return false;
        }
    }
    
    takeCard( player->handcard, player->shield, index );   
    printUI( player, "" );
    puts( "Equip barrel" );
    ENTER;
    return true;
}


bool UnloadBarrel( Player *player ) {
    int index = -1;
    for(int i = 0; i < player->shield->size; i++) {
        if( player->shield->data[i].suit != -1 ) {
            index = i;
        }
    }
    
    if( index == -1 ) {
        puts( "You didn't have Barrel to unload" );
        return false;
    }    
    
    discardCard( player->shield, index ); 
    printUI( player, "" );
    puts( "Unload barrel" );     
    ENTER; 
    return true; 
}

bool nullFunc2( Player *player ){ return false; }
bool nullFunc3( Player *player, int pos ){ return false; }