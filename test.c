#include "test.h"

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
    sprintf( str, "Which card to use (1~%zu, 0: quit): ", cards->size );
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
    bool is_use = chooseCard( defender, defender->handcard, MISSED );
    if ( is_use ) {
      printf( "Avoid attack!\n" );
      ENTER;
    }
    return is_use;
}  