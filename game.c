#include "game.h"

void game_prepare()
{
    //設定遊戲人數    
    PLAYERS_NUM = scan(4, 7, "Input the numbers of players (4~7): ");
    ALIVE_NUM = PLAYERS_NUM;
    PLAYERS_LIST = (Player*)calloc( PLAYERS_NUM, sizeof(Player) );
    
    //設定遊戲玩家名稱
    char *players = calloc(100, sizeof(char));
    for(int i = 0; i < PLAYERS_NUM; i++)
    {
        PLAYERS_LIST[i].id = i;  
      init_player(&PLAYERS_LIST[i]);
        
        PLAYERS_LIST[i].handcard->id = i;
        printf( "Input the name of player %d : ", i+1 );
        fgets( players, 1000, stdin );
        if( strcmp( players, "\n") == 0 ) {
            sprintf( players, "Player %d", i+1 );
        }
        clean_buffer(players);
        strcpy(PLAYERS_LIST[i].name, players);
        //printf("%s\n",PLAYERS_LIST[i].name);
    }
  
    memset( OFFSET_DISTANCE, 0, 100 * sizeof(int) );
    memset( DISTANCE, 0, 100 * sizeof(int) );

    calcDistance();
    
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
       //skills[p->role]( &p, NULL );
    }
    PLAYERS_LIST[SHERIFF_POSITION].hp++;
    PLAYERS_LIST[SHERIFF_POSITION].hp_limit++;    
 
    GAME_STATE = NOT_YET_START;
}

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

// 判斷階段：炸藥->監獄
void judgeTurn( Player *player, Player *nextPlayer ) {
  
  bool judge = false;
  // 判斷炸藥
  for ( int i = 0; i < player->judgeCards->size; i++ ) {
    Card card = get_element( player->judgeCards, i );
    if ( card.kind == DYNAMITE ) {
      printUI( player, "Judge Turn" );
      printf( "%sJudge dynamite%s\n\n", BLUE, RESET );
      judge = judgeFunc( player, DYNAMITE );
      if ( judge ) {
        printf( "%sJudgment successful%s\n", GREEN, RESET );
        ENTER;
        takeCard( player->judgeCards, nextPlayer->judgeCards, i );
      }
      else {
        discardCard( player->judgeCards, i );
        printf( "%sJudgment failed%s\n", RED, RESET );
        printf( "Player %s loses 3 hp", player->name );
        ENTER;
        HPModify( NULL, player, -3, DYNAMITE );
        if ( GAME_STATE == END ) return;
      }
      i--;
    }
  }

  // 判斷監獄
  while ( !isEmpty( player->judgeCards ) ) {
    Card card = get_element( player->judgeCards, 0 );
    // printf( "main 133: card kind = %s\n", cardKindName[card.kind] );
    ASSERT( card.kind == JAIL );

    printUI( player, "Judge Turn" );
    printf( "%sJudge jail%s\n\n", BLUE, RESET );
    judge = judgeFunc( player, JAIL );
    discardCard( player->judgeCards, 0  );
    
    if ( judge ) {
      printf( "%sThe jailbreak is successful, you can continue this round.%s\n", GREEN, RESET );
      ENTER;
    }
    else{
      printf( "%sIf the jailbreak fails, this round will be suspended once.%s\n", RED, RESET );
      ENTER;
      while ( !isEmpty( player->judgeCards ) ) {
        Card card = get_element( player->judgeCards, 0 );
        ASSERT( card.kind == JAIL );
        discardCard( player->judgeCards, 0  );
      }
      player->state = DISCARD_CARD;
      
    }
  }

  if ( player->state == JUDGE ) {
    player->state = GET_CARD;
    puts( "State: Judge -> Get Card" );
  }
  else {
    puts( "State: Judge -> Discard Card" );
  }
  ENTER;
}

void playerCard( Player *player ) {
  if ( player == NULL ) return;
  if ( player->state == IS_DEAD ) return;

  int *color = NULL;
  int num = setColor( &color, -1, -1, 1, player->handcard, 3 );

  if ( player->role == Willy_the_Kid ) {
    printf( "Active Willy_the_Kid's skill\n" );
    puts( "You have unlimited times to use BANG" );
    ENTER;
  }

  if ( player->role == Calamity_Janet ) {
    setColor( &color, -1, MISSED, 0, player->handcard, 3 );
  }
  
  if ( player->numOfBang >= player->bangLimit && player->bangLimit != -1 ) {
    setColor( &color, -1, BANG, 0, player->handcard, 0 );
    setColor( &color, -1, MISSED, 0, player->handcard, 0 );
  }

  if ( ALIVE_NUM <= 2 ) {
    setColor( &color, -1, BEER, 0, player->handcard, 0 );
  }

 
  
  if ( player->hp >= player->hp_limit ) {
    setColor( &color, -1, SALOOW, 0, player->handcard, 0 );
    setColor( &color, -1, BEER, 0, player->handcard, 0 );
  }
  
  if ( num == 0 ) {
    puts("No cards available");
    player->state = DISCARD_CARD;
    puts( "State: Player Card -> Discard Card" );
    ENTER;
    return;
  }

  int choice, size = player->handcard->size;
  char *str = (char *)calloc( strlen("Choice the card you want to play(1~, 0: end the round): ") + 5, sizeof(char) );
  sprintf( str, "Choice the card you want to play(1~%d, 0: end the round): ", size );
  char input[100] = {0};
  bool warn = false;
  while ( 1 ) {
      
      printUI( player, "" );
      if ( warn )
        puts("You can't play this card");
      printHandCard( player->handcard, color, true);
      choice = scan( 0, size, str );
      if ( choice == 0 ) {
        player->state = DISCARD_CARD;
        puts( "State: Player Card -> Discard Card" );
        ENTER;
        return;
      }
      else if ( color[choice-1] == 3 ) {
        color[choice-1] = 1;
        printUI( player, "Play Card" );
        printHandCard( player->handcard, color, true);
        Card tmp = get_element( player->handcard, choice-1 );
        printf( "Are you sure you want to use the card? (Y/n) " );
        fgets(input, 100, stdin);
        if (( input[0] == 'Y' || input[0] == 'y' || input[0] == '\n' ) && tmp.sticker != NONE ) {
            warn = false;
            if ( tmp.is_orange ) {
              if ( tmp.sticker == BEER && Beer( player, NULL ) ) {
                Card c = get_element( player->handcard, choice-1 );
                discardCard( player->handcard, choice-1 );
              }
              else if ( tmp.attribute == 1 && orangeCards[tmp.kind]( player ) ) {
                if ( tmp.kind == BANG ) {
                  player->numOfBang++;
                }
                discardCard( player->handcard, choice-1 );
              }
              else if ( tmp.attribute == 2 && orangeCards[tmp.sticker]( player ) ) {
                if ( tmp.sticker == BANG ) {
                  player->numOfBang++;
                }
                discardCard( player->handcard, choice-1 );
              }
            }
            else {
              blueCards[tmp.kind]( player, choice-1 );
            }
            break;
          }
          else {
            warn = false;
            color[choice-1] = 3;
            continue;
          } 
        }
        else {
          warn = true;
          continue;
      }
  }
  free( str );
  free( color );
}

void discardTurn( Player *player ) {
  
    if( player->state == IS_DEAD ) {
        return; // 死了
    }
    
    int num = player->handcard->size - player->hp;
    
    bool is_discard = false;
    int choice = 1;
    while ( true ) {
      printUI( player, "Discard Turn" );
      if ( num > 0 ) {
        printf("You have to discard %d handcard(s)\n", num );
        ENTER;
      }
      else if ( player->handcard->size > 1 ) {
        choice = scan( 0, 1, "Do you want to discard cards? (1: yes, 0: no) " );
        if ( choice == 0 ) return;
      }
      else return;
        
      is_discard = chooseCard( player, player->handcard, -1, NULL, false, true, "Discard Turn" ).number > 0 ? true : false;
      if ( is_discard && num ) num--;
    }
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
      Card beer = chooseCard( player, player->handcard, BEER, NULL, false, true, "Choose Beer to add up your HP" );
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
