#include "test.h"

void HPModify( Player* attacker, Player *defender, int n, Kind reason ){
  if ( n < 0 ) {
    printf( "Player %s minus %d hp.\n", defender->name, n );
    ENTER;
    RMode tmp = defender->state;
    defender->state = MINUS_HP;
    skills[defender->role]( defender, &reason );
    defender->state = tmp;
    defender->hp += n;
  }
  else {
    printf( "Player %s plus %d hp.\n", attacker->name, n );
    ENTER;
    defender->hp += n;
  }

  // call IsGameOver( attacker, defender );
}

// if killer is null, then there is no killer
// e.g. if the player is killed by dynamite, there is no killer
void IsGameOver( Player *killer, Player *player ){
  if ( player->hp <= 0 )
    // call dying function, which can use barrels or some else to add hp
    ;
  if ( player->hp <= 0 ) {
    player->state = IS_DEAD;
    PLAYERS_NUM--;
     // remove the player from the player list
    int k = 0;
    for ( int i = 0; i < PLAYERS_NUM; i++ ) {
        if ( player->id == PLAYERS_LIST[i].id ) {
            k = i;
            break;
        }
    }
    for ( int i = k; i < PLAYERS_NUM-1; i++ ) {
        PLAYERS_LIST[i] = PLAYERS_LIST[i+1];
    }
    printf( "Player %s is dead and his identity is %s\n", player->name, identityName[player->identity] );
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
    if ( SHERIFF_NUM == 0 ) {
      GAME_STATE = END;
      // set winner
      return;
    }
    if ( killer != NULL ) { 
      if ( player->identity == Outlaws ) {
        cardHandler( killer, 3 );
      }
      else if ( player->identity == Deputies && killer->identity == Sheriff ) {
        discardAllCard( killer );
      }
    }

    for ( int i = 0; i < PLAYERS_NUM; i++ ) {
      if ( PLAYERS_LIST[i].identity == Vulture_Sam && PLAYERS_LIST[i].hp > 0 ) {
        takeAllCards( player, &PLAYERS_LIST[i] );
      }
    }

    if ( !isEmpty( player->handcard ) ) {
      discardAllCard( player );
    }
  }
}

// 選擇攻擊對象
// if limitDistance == -1 then 沒有距離限制
Player *choosePlayer( Player *attacker, int limitDistance ) {
    if ( attacker == NULL ) return NULL;
    Player *p = NULL; 
    if( limitDistance == -1 ) limitDistance = 20; //開無限
    int *ID = malloc(PLAYERS_NUM), id_count = 0;
    printf("Which player to attack: \n");
    printf("Choice   Name    ID\n");
    for(int i = 0; i < PLAYERS_NUM; i++) {
        p = PLAYERS_LIST + i;           
        int distance = DISTANCE[attacker->id][p->id];
        if( p->id != attacker->id && distance <= limitDistance ) {
            printf("[%d] %10s %7d \n", id_count, p->name, p->id);
            ID[id_count++] = p->id;
        }            
    }

    if ( id_count <= 0 ) {
        printf("There isn't any players to be chosen.\n");
        return NULL;
    } // 沒有可以使用的對象
  
    int choice = scan(0, id_count-1, "--> ");
    for(int i = 0; i < PLAYERS_NUM; i++) {
        p = PLAYERS_LIST + i;           
        if( p->id == ID[choice] ) {
            break;
        }            
    }
    free(ID);
    return p;
} 

// if kind == -1, then all the card can be chosen
// if get_card != NULL, then card -> get_card
// otherwise discard the chosen card
bool chooseCard( Player *player, Card_vector* cards, int kind, Card_vector* get_card ) {
    if ( player == NULL ) return false;
    if ( cards == NULL ) return false;

    printUI( player );

    if ( isEmpty( cards ) ) {
      printf("There is no card\n");
      ENTER;
      return false;
    }
    int *color = NULL;
    int num = setColor( &color, -1, kind, -1, cards, 3 );
    if ( num == 0 ) {
      printHandCard( cards, color );
      printf("No card to choose\n");
      ENTER;
      return false;
    }

    char str[100];
    sprintf( str, "Choose a card (1~%d, 0: quit): ", cards->size );
    int choice = 0;
    bool warn = false;
    char input[100];
    while ( 1 ) {
      system("clear");
      printUI( player );
      if ( warn )
        printf( "You can't choose this card\n" );
      printHandCard( cards, color );
      choice = scan(0, cards->size, str );
      if ( choice == 0 ) return false;
      if ( color[choice-1] == 3 ) {
        warn = false;
        color[choice-1] = 1; // green to red
        system("clear");
        printUI( player );
        printHandCard( cards, color);
        Card tmp = get_element( cards, choice-1 );
        printf( "Are you sure you want to choose the [%s] card? (Y/n) ", cardKindName[tmp.sticker] );
        fgets(input, 100, stdin);
        if ( input[0] == 'Y' || input[0] == 'y' ) {
          if ( get_card != NULL ) {
            takeCard( cards, get_card, choice-1 );
          }
          else {
            discardCard( cards, choice-1 );
          }
            return true;
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
    return true;
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
  printf( "Player %s is attacked by %s\n", defender->name, attacker->name );
  bool miss = Miss( defender, 1 );

  system("clear");
  printUI( attacker );

  if ( !miss ) {
    HPModify( attacker, defender, -1, BANG );
    printf( "successful attack!\n" );
  }
  else
    printf( "Missed attack!\n" );

  ENTER;

  return true;

}

bool Miss( Player *defender, int n ) {
    if ( defender == NULL ) return false;
    system("clear");
    printUI( defender );

    if( scan(0, 1, "Do you want to use card MISSED to avoid attack ? (0 : NO, 1 : YES) \n") == 0 ) {
        return false;
    }
    bool is_use = chooseCard( defender, defender->handcard, MISSED, NULL );
    if ( is_use ) {
      printf( "Avoid attack!\n" );
      ENTER;
    }
    return is_use;
}  

// 所有玩家棄一張bang，沒有棄的扣一滴血
bool Indians( Player *attacker ) {
    if ( attacker == NULL ) return false;
    Player *p;     
    printf("All the others in the game except you have to discard a BANG.\n");
    printf("Or minus 1 hp.\n");
    for(int i = 0; i < PLAYERS_NUM; i++) {
        p = PLAYERS_LIST + i;  
        // 如果不是自己        
        if( p->id != attacker->id ) {
            // 其他人的對應            
            Indians_respond( attacker, p );
        }            
    }
  return true;
}

// Indians 被攻擊者回應
bool Indians_respond( Player *attacker, Player *defender ) {
    bool is_discard_bang = chooseCard( defender, defender->handcard, BANG, false );
    system("clear");
    printUI( defender );
    if ( is_discard_bang ) {
        printf( "Player %s discard a BANG.\n", defender->name );
        ENTER;
        return true;
    }
    else {
        HPModify( defender, defender, -1, INDIANS );
        return false;
    }
    
}

//玩家從牌庫頂抽兩張牌
bool Stagecoach( Player *player ){
  if ( player == NULL ) return false;
  printf( "Player %s get two cards\n", player->name );
  cardHandler( player, 2 );
  ENTER;
  return true;
}

//玩家從牌庫頂抽三張牌
bool Wells( Player *player ) {
  if ( player == NULL ) return false;
  printf( "Player %s get three cards\n", player->name );
  cardHandler( player, 3 );
  ENTER;
  return true;
}

// 計算目前仍在遊戲中的人數，從牌庫翻開相同數量的牌，每位玩家挑選一張加入自己的手牌中
// 由當回合的玩家開始挑選，以順時鐘方向挑選
bool Store( Player *player ) {
    if ( player == NULL ) return false;
    Player tmpPlayer, *p;
    int start = -1;
    tmpPlayer.handcard = create_vector(10);
    for(int i = 0; i < PLAYERS_NUM; i++) {
        p = PLAYERS_LIST + i;
        if( p->id == player->id ) {
            start = i;
            break;
        }
    }

    cardHandler( &tmpPlayer, PLAYERS_NUM );
    int i = start;
    do {
        p = PLAYERS_LIST + (start + i) % PLAYERS_NUM;
        bool get_card = false;
        bool warn = false;
        while ( !get_card ) {
          if ( warn ) 
            puts( "Please choose a card" );
          get_card = chooseCard( p, tmpPlayer.handcard, -1, p->handcard );
          warn = true;
        }
        i++;
    } while ( (start + i) % PLAYERS_NUM != start );
    return true;
}

// (自己回合 or 瀕死) and 血量不是上限 
// 因為有可能在瀕死時使用，所以須紀錄attacker是誰
bool Beer( Player *player, Player *attacker ){
  if( player == NULL ) return false;
  if( PLAYERS_NUM==2 )
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
    Player p = PLAYERS_LIST[i];
    if( player->hp == player->hp_limit ) //已經是上限
    {
      printf("Player %s's hp limit reached.\n", player->name);
      ENTER;    
    }
    else 
      HPModify( &p, &p, 1, SALOOW );
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
          is_discard_bang = chooseCard( attacker, attacker->handcard, BANG, NULL );
          if ( is_discard_bang ) {
            printf( "Player %s discard a BANG.\n", attacker->name );
            printf( "Now is %s's turn\n", dueler->name );
            turn = 2;
            ENTER;
          }
          else {
            printf("Duel Ended.\n");
            printf( "%s won the game.\n", dueler->name );
            HPModify( attacker, attacker, -1, DUEL );
            break;
          }
        }
        else {
          is_discard_bang = chooseCard( dueler, dueler->handcard, BANG, NULL );
          if ( is_discard_bang ) {
            printf( "Player %s discard a BANG.\n", dueler->name );
            printf( "Now is %s's turn\n", attacker->name );
            turn = 1;
            ENTER;
          }
          else {
            printf("Duel Ended.\n");
            printf( "%s won the game.\n", attacker->name );
            HPModify( dueler, dueler, -1, DUEL );
            break;
          }
        }
    } 
    return true;
}

bool find_position( int id,int *position )
{
  for(int i=0;i<PLAYERS_NUM;i++)
  {
     Player p = PLAYERS_LIST[i];
      if(p.id == id )
      {
        *position = i;
      }
  }
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
  int arr_position = 0;
  find_position( player->id, &arr_position );
  
  for(int i=0;i<PLAYERS_NUM;i++)
  {
    if(i!=arr_position)
    {
      DISTANCE[arr_position][i]--;
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

  system("clear");
  printUI( player );
  puts( "You are going to unload the scope" );
  ENTER;
  
  bool is_discard = false;
  if ( cards == NULL ) {
    is_discard = chooseCard( player, player->distance_item, SCOPE, NULL );
  }
  else {
    is_discard = chooseCard( player, player->distance_item, SCOPE, cards );
  }
    
  if ( !is_discard ) {
    puts( "Not unload the scope" );
    ENTER;
    return false;
  }

  int player_id = player->id;
  int arr_position=0;
  find_position(player_id, &arr_position);
  
  for(int i=0;i<PLAYERS_NUM;i++)
  {
    if(i!=arr_position)
    {
      DISTANCE[arr_position][i]++;
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

  int player_id = player->id;
  int arr_position=0;
 
  player->distance_item = create_vector(1);
  // handcard 給 distance_item 
  takeCard( player->handcard, player->distance_item, card_position );
  
  find_position(player_id,&arr_position);

  for(int i=0;i<PLAYERS_NUM;i++)
  {
    if(i!=arr_position)
    {
      DISTANCE[i][arr_position]++;
    }
  }

  player->equipMustang = MUSTANG;
  puts( "Mustang equipped" );
  ENTER;
  return true;
}
    
bool UnloadMustang( Player *player, Card_vector *cards ){
  if ( player == NULL ) return false;

  system("clear");
  printUI( player );
  puts( "You are going to unload the mustang" );
  ENTER;
  
  bool is_discard = false;
  if ( cards == NULL ) {
    is_discard = chooseCard( player, player->distance_item, MUSTANG, NULL );
  }
  else {
    is_discard = chooseCard( player, player->distance_item, MUSTANG, cards );
  }
    
  if ( !is_discard ) {
    puts( "Not unload the mustang" );
    ENTER;
    return false;
  }
  
  int player_id = player->id;
  int arr_position=0;
  find_position(player_id,&arr_position);

  for(int i=0;i<PLAYERS_NUM;i++)
  {
    if(i!=arr_position)
    {
      DISTANCE[i][arr_position]--;
    }
  }

  puts( "Mustang unloaded" );
  ENTER;
  return true;
}

bool UnloadWeapon( Player* player, Card_vector *cards ) {
  if ( player == NULL ) return false;
  
  if ( player->equipWeapon != NONE ) {
    system("clear");
    printUI( player );
    puts( "You are going to unload the weapon" );
    ENTER;
    bool is_discard = false;
 
    if ( player->equipWeapon == VOLCANIC ) {
      if ( cards == NULL )
        is_discard = chooseCard( player, player->weapon, VOLCANIC, NULL );
    
      else
        is_discard = chooseCard( player, player->weapon, VOLCANIC, cards );
      if ( is_discard )
        UnloadVolcanic( player, NULL );
    }
    else if ( player->equipWeapon == SCHOFIELD ) {
      if ( cards == NULL )
        is_discard = chooseCard( player, player->weapon, SCHOFIELD, NULL );
    
      else
        is_discard = chooseCard( player, player->weapon, SCHOFIELD, cards );
      if ( is_discard )
        UnloadSchofield( player, NULL );
    }
    else if ( player->equipWeapon == REMINGTON ) {
      if ( cards == NULL )
        is_discard = chooseCard( player, player->weapon, REMINGTON, NULL );
    
      else
        is_discard = chooseCard( player, player->weapon, REMINGTON, cards );
      if ( is_discard )
        UnloadRemington( player, NULL );
    }
    else if ( player->equipWeapon == REV ) {
      if ( cards == NULL )
        is_discard = chooseCard( player, player->weapon, REV, NULL );
    
      else
        is_discard = chooseCard( player, player->weapon, REV, cards );
      if ( is_discard )
        UnloadRev( player, NULL );
    }
    else if ( player->equipWeapon == WINCHEDTER ) {
      if ( cards == NULL )
        is_discard = chooseCard( player, player->weapon, WINCHEDTER, NULL );
    
      else
        is_discard = chooseCard( player, player->weapon, WINCHEDTER, cards );
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

  player->numOfBang = -1;
  puts( "Equip the Volcanic" );
  ENTER;
  return true;
}

bool UnloadVolcanic( Player *player, Card_vector *cards ) {
  if ( player == NULL ) return false;
  // Willy the Kid 的回合可以用任意張 BANG
  player->numOfBang = (player->role != Willy_the_Kid) ? 1 : -1;  

  system("clear");
  printUI( player );
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
  system("clear");
  printUI( player );
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
  system("clear");
  printUI( player );
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
  system("clear");
  printUI( player );
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
  system("clear");
  printUI( player );
  puts( "Winchester unloaded" );
  ENTER;
  return true;
}