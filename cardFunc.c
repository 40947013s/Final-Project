#include "cardFunc.h"

void HPModify( Player* attacker, Player *defender, int n, Kind reason ){
  if ( n < 0 ) {
    RMode tmp = defender->state;
    defender->state = MINUS_HP;
    skills[defender->role]( defender, &reason );
    defender->state = tmp;
    defender->hp += n;
  }
  else
    defender->hp += n;

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

// if return true: player choose the card, and discard that card
bool chooseCard( Player *player, Card_vector* cards, Kind kind ) {
    if ( cards == NULL ) {
      printf("No card to choose\n");
      return false;
    }
    if ( isEmpty( cards ) ) return false;
    int *color = NULL;
    int num = setColor( &color, -1, kind, -1, cards, 3 );
    if ( num == 0 ) {
      printHandCard( cards, color );
      printf("No card to choose\n");
      ENTER;
      return false;
    }

    char str[100];
    sprintf( str, "Which card to use or discard (1~%zu, 0: quit): ", cards->size );
    int choice = 0;
    bool warn = false;
    char input[100];
    while ( 1 ) {
      system("clear");
      printUI( player );
      if ( warn )
        printf( "You can't use this card\n" );
      printHandCard( cards, color );
      choice = scan(1, cards->size, str );
      if ( choice == 0 ) return false;
      if ( color[choice-1] == 3 ) {
        warn = false;
        color[choice-1] = 1; // green to red
        system("clear");
        printUI( player );
        printHandCard( cards, color);
        Card tmp = get_element( cards, choice-1 );
        printf( "Are you sure you want to use the [%s] card? (Y/n) ", cardKindName[tmp.sticker] );
        fgets(input, 100, stdin);
        if ( input[0] == 'Y' || input[0] == 'y' ) {
            discardCard( cards, choice-1 );
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

bool Gatling( Player *attacker ) {
    if ( attacker == NULL ) return false;
    Player *p;     
    printf("You can attack all the others in the game.\n");
    for(int i = 0; i < PLAYERS_NUM; i++) {
        p = PLAYERS_LIST + i;  
        // 如果不是自己        
        if( p->id != attacker->id ) {
            if(/*啤酒桶*/) {
                
            } if( Miss(p, 1) ) {                
                printf("%10s %7d avoid attack.\n", p->name, p->id);
            }else {
                HPModify( attacker, p, -1, GATLING ); // 扣一滴血
                printf("%10s %7d had been attacked.\n", p->name, p->id);
            }
        }            
    }
    return true;
}

// Bang 無效
bool Miss( Player *defender, int n ) {
    if ( defender == NULL ) return false;
    if( scan(0, 1, "Do you want to use card MISSED to avoid attack ? (0 : NO, 1 : YES) \n") == 0 ) {
        return false;
    }
  
    bool is_use = chooseCard( defender, defender->handcard, MISSED );
    if ( is_use ) {
      printf( "Avoid attack!\n" );
      ENTER;
    }
    return is_use;
    int size = defender->handcard->size;
    int miss_count = 0;
    for(int i = 0; i < size; i++) {
        // 如果是失手        
        if( defender->handcard->data[i].sticker == MISSED ) {
            miss_count++;
        }
    }
    if( miss_count < n ) {
        printf("Sorry, you don't have enough MISSEDs to avoid attack\n");
        return false;
    }
    while( n ) {
        int i = 0;
        for(i = 0; i < defender->handcard->size; i++) {  
            if( defender->handcard->data[i].sticker == MISSED ) {
                break;
            } 
        } if(discardCard( defender, i )) n--;
    }
    return true;
}  

// Indians 被攻擊者回應
void Indians_respond( Player *attacker, Player *defender ) {
    printf("Give BANG?\n");
    int choice = scan(0, 1, "--> "), index = -1;  
    // 選擇棄BANG            
    if(choice) {
        for(int j = 0; j < defender->handcard->size; i++) {
            if( defender->handcard->data[j].sticker == BANG ) {
                index = j; break;
            }
        }
        // 沒有BANG                
        if(index == -1 ) {
            printf("Sorry, you don't have BANG to avoid this attack\n");                    
            HPModify( attacker, defender, -1, GATLING );
            printf("%10s %7d hp minus 1.\n", defender->name, defender->id);
        } else {
            discardCard( defender.hancard, index );
        }
    } else {
        HPModify( attacker, defender, -1, GATLING );
        printf("%10s %7d hp minus 1.\n", defender->name, defender->id);
    }

}

// 所有玩家棄一張bang，沒有棄的扣一滴血
bool Indians( Player *attacker ) {
    if ( attacker == NULL ) return;
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

//從距離為1的玩家拿取一張牌
bool panic( Player *attacker ){
  if(attacker == NULL) return false;
  int att_arr = 0;
  char *str = malloc(1000);
  int *store_position = (int *)calloc(PLAYERS_NUM,sizeof(int));
  for(int i=0;i<PLAYERS_NUM;i++)
  {
    Player p = PLAYERS_LIST[i];
    if(strcmp(p.name,attacker->name)==0) //找attacker在array的位置
    {
      att_arr = i;
      break;
    }
  }
  bool ch[10] = {0};
  for(int i=0;i<PLAYERS_NUM;i++)
  {
    if(DISTANCE[att_arr][i] == 1) //標記距離為1的人
    {
      ch[i] == true;
    }
  }

  int counter=0;
  printf("Enter the player you like to choose:\n");
  for(int i=0;i<PLAYERS_NUM;i++)
  {
    if(ch[i]==true)
    {
      Player p = PLAYERS_LIST[i];
      char *tmp = malloc(100);
      sprintf(tmp,"[%d] name: %s ID: %d\n",counter,p.name,p.id);
      strcat(str,tmp);
      free(tmp);
      store_position[counter] = i;
      counter++;
    }
  }
  int choose_player = scan(0,counter-1, str);
  int array_position= store_position[choose_player];

  while(isEmpty(PLAYERS_LIST[array_position].handcard))
  {
    printf("%s's handcard is empty.\n",PLAYERS_LIST[array_position].name );
    choose_player = scan(0,counter-1, "Please enter again:\n");
    array_position = store_position[choose_player];
  }
  int max = PLAYERS_LIST[array_position].handcard->size;
  printf("Player %s has %d card(s) (choose from 0 to %d)\n",PLAYERS_LIST[array_position].name,max,max-1);
  int which_card = scan(0,max-1,"Choose the card you want: \n");
  takeCard( PLAYERS_LIST[array_position].handcard, attacker->handcard, which_card );

  free(str);
  free(store_position);
  return true;
}

void cat_respond( Player *defender ) {
    printf("You need to discard a card.\n");
    printf("Which card to discard: \n");
    printf("Choice   kind\n");
    
    int h_size = defender->handcard->size, w_size = defender->weapon->size;
    int s_size = defender->shield->size, d_size = defender->distance_item->size;
    int counter = 0;
    for(int i = 0; i < h_size; i++, counter++) {
        printf("[%d] %d\n", counter, defender->handcard->data[i].kind);
    }
    for(int i = 0; i < e_size; i++, counter++) {
        printf("[%d] %d\n", counter, defender->weapon->data[i].kind);
    }
    for(int i = 0; i < e_size; i++, counter++) {
        printf("[%d] %d\n", counter, defender->shield->data[i].kind);
    }
    for(int i = 0; i < e_size; i++, counter++) {
        printf("[%d] %d\n", counter, defender->distance_item->data[i].kind);
    }
    int choice = scan(0, counter-1, "--> ");
    discardCard( defender, choice );
}

// 要求某玩家棄置手牌或裝備牌
bool cat( Player *attacker ) {
    Player *p;  
    int *INDEX = malloc(PLAYERS_NUM), id_count = 0; 
    printf("Which player to discard a card: \n");
    printf("Choice   Name    ID\n");
    for(int i = 0; i < PLAYERS_NUM; i++) {
        p = PLAYERS_LIST + i;  
        int card_size = p->handcard->size + p->weapon->size;
        card_size += defender->shield->size + defender->distance_item->size;
        // 如果不是自己且至少有一張手牌或裝備牌   
        if( p->id != attacker->id && card_size > 0) {
            INDEX[id_count++] = i;
            printf("[%d] %10s %7d \n", id_count, p->name, p->id);
        }            
    }
    if ( id_count <= 0 ) {
        printf("No one have any card to discard.\n");
        return;
    }
    int choice = scan(0, id_count-1, "--> ");
    // call PLAYERS_LIST[choice] to discard a card.
    cat_respond( PLAYERS_LIST[INDEX[choice]] );
    return true;
}

//玩家從牌庫頂抽兩張牌
bool Stagecoach( Player *player ){
  if ( player == NULL ) return false;
    cardHandler( player, 2 );
  return true;
}

//玩家從牌庫頂抽三張牌
bool Wells( Player *player ){
  if ( player == NULL ) return;
  cardHandler( player, 3 );
  return true;
}

bool Store_choose( Player *player, Card *open ) {
    printf("Please choose a card to your handcard.\n");
    printf("Which card to discard: \n");
    printf("Choice   kind\n");
    int size = open->size, counter = 0;
    for(int i = 0; i < size; i++, counter++) {
        printf("[%d] %d\n", counter, open->data[i].kind);
    }
    int choice = scan(0, counter-1, "--> ");
    takeCard( open, player->handcard, choice );
    return true;
}

// 計算目前仍在遊戲中的人數，從牌庫翻開相同數量的牌，每位玩家挑選一張加入自己的手牌中
bool Store( Player *player ) {
    if ( player == NULL ) return false;
    Player tmpPlayer, *p;
    int start = -1;
    
    cardHandler( &tmpPlayer, PLAYERS_NUM );
    for (int i = 0; i < PLAYERS_NUM && i != start; i++) {
        p = PLAYERS_LIST[i];
        if ( player->ip == p->ip ) {
            start == i;
        } else if ( i == PLAYERS_NUM-1 ) {
            i = 0;
        } // 選卡
        if(start) {
            Store_choose( &p, tmpPlayer->handcard );
        }
    }
    return true;
}

// (自己回合 or 瀕死) and 血量不是上限 
bool Beer( Player *player ){
  if(player == NULL) return;
  if(PLAYERS_NUM==2)
  {
    printf("You can not use Beer when only two players left.\n");
    return false;
  }
  if(player.hp == player.hp_limit) //已經是上限
  {
    printf("Your hp limit reached.\n");
    return false;
  }
  player.hp++;
  return true;
}

bool Saloow( Player *player ){
  if(player == NULL) return;
  for(int i=0;i<PLAYERS_NUM;i++)
  {
    Player p = PLAYERS_LIST[i];
    p.hp++;
  }
  return true;
}

bool Duel_end( Player *player ) {
    printf("Give BANG?\n");
    int choice = scan(0, 1, "--> "), index = -1;  
    // 選擇棄BANG            
    if(choice) {
        for(int j = 0; j < defender->handcard->size; i++) {
            if( defender->handcard->data[j].sticker == BANG ) {
                index = j; break;
            }
        }
        // 沒有BANG                
        if(index == -1 ) {
            printf("Sorry, you don't have BANG to avoid this attack\n");                    
            return true;
        } else {
            discardCard( defender, index );
        }
    } else return true;
}
// 出不了BANG的一方扣血
bool Duel( Player *attacker ) {
    Player *dueler = choosePlayer( attacker, -1 );
    Player *player, *winner;
    while(1) {
        player = dueler, winner = attacker;
        if(Duel_end( player )) break;
        player = attacker, winner = dueler;
        if(Duel_end( player )) break;
    } 
    printf("Duel Ended.\n");
    HPModify( winner, player, -1, DUEL );
    printf("%10s %7d hp minus 1.\n", player->name, player->id);
    return true;
}

// ----------------------------------------------------
// blue card
/*
Equip weapon 裝備武器
unload weapon 卸下武器
*/

bool find_position(int id,int *position)
{
  for(int i=0;i<PLAYERS_NUM;i++)
  {
     Player p = PLAYERS_LIST[i];
      if(p.id == player_id)
      {
        *position = i;
      }
  }
  return true;
}
//看別人的距離減1
/*v->data[n].attribute = value.attribute;
        v->data[n].kind = value.kind;
        v->data[n].number = value.number;
        v->data[n].suit = value.suit;
        v->data[n].sticker = value.sticker;
        v->data[n].is_orange = value.is_orange;*/

// 
bool EquipScope( Player *player ){
  int player_id = player.id;
  int arr_position=0, card_position=0; 
  int size = player->handcard->size;
  for(int i=0;i<size;i++)
  {
    Card tmp = get_element(player.handcard,i);
    if(tmp.sticker == 13)
    {
      card_position = i;
      break;
    }
  } 
    
    int d_size = player->distance_item->size;
    for(int i = 0; i < d_size; i++) {
        Card tmp = get_element(player.handcard,i);
        if(tmp.sticker == 13) {
            printf("You ha")
        }
    }
    // handcard 給 distance_item 
  takeCard( player->handcard, player->distance_item, card_position );
  find_position(player_id,&arr_position);
  
  for(int i=0;i<PLAYERS_NUM;i++)
  {
    if(i!=arr_position)
    {
      DISTANCE[arr_position][i]--;
    }
  }
  return true;
}
  
bool UnloadScope( Player *player ){
  int player_id = player.id;
  int arr_position=0;
  find_position(player_id,&arr_position);
  
  for(int i=0;i<PLAYERS_NUM;i++)
  {
    if(i!=arr_position)
    {
      DISTANCE[arr_position][i]++;
    }
  }
  // 丟進棄牌區
  while(!isEmpty()) {
      discardCard( player->distance_item, 0);
  }
  return true;
}

//所有其他玩家看你的距離加1，自己看其他玩家距離則不變
bool EquipMustang( Player *player ){
  int player_id = player.id;
  int arr_position=0;
  int card_position=0;
  int size = player->handcard->size;
  for(int i=0;i<size;i++)
  {
    Card tmp = get_element(player.handcard,i);
    if(tmp.sticker == 14)
    {
      card_position = i;
      break;
    }
  } 
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
  return true;
}
    
bool UnloadMustang( Player *player ){
  int player_id = player.id;
  int arr_position=0;
  find_position(player_id,&arr_position);

  for(int i=0;i<PLAYERS_NUM;i++)
  {
    if(i!=arr_position)
    {
      DISTANCE[i][arr_position]--;
    }
  }
    // 丟進棄牌區
  while(!isEmpty()) {
      discardCard( player->distance_item, 0);
  }
  return true;
}

//可以使用任意張數「砰」
bool EquipVolcanic( Player *player ) {
  numOfBang = -1;
  return true;
}
bool UnloadVolcanic( Player *player ) {
    // Willy the Kid 的回合可以用任意張 BANG
    numOfBang = (player.role != Willy_the_Kid) ? 1 : -1;  
    return true;
}

bool EquipBarrel( Player *player ) {
    
  return true;
}
bool UnloadBarrel( Player *player ){
  return true;
}

bool EquipJail( Player *player ){
  return true;
}
bool UnloadJail( Player *player, Card *card ){
  return true;
}
/*
  卸下Jail時要進行判定
  card為指定判定的牌，若card == NULL，則在function中抽一張
*/

bool EquipDynamite( Player *player ){
  return true;
}
bool UnloadDynamite( Player *player ){
  return true;
}


// change player.attack_distance

//射擊距離變成2
bool EquipSchofield( Player *player ){
  player.attack_distance = 2;
  return true;
}
bool UnloadSchofield( Player *player ){
  player.attack_distance = 1;
  return true;
}

//射擊距離變成3
bool EquipRemington( Player *player ){
  player.attack_distance = 3;
  return true;
}
bool UnloadRemington( Player *player ){
  player.attack_distance = 1;
  return true;
}

//射擊距離變成4
bool EquipRev( Player *player ){
  player.attack_distance = 4;
  return true;
}
bool UnloadRev( Player *player ){
  player.attack_distance = 1;
  return true;
}
//射擊距離變成5
bool EquipWinchester( Player *player ){
  player.attack_distance = 5;
  return true;
}
bool UnloadWinchester( Player *player ){
  player.attack_distance = 1;
  return true;
}
