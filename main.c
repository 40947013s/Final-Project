#include "init.h"

#define TESTCASE \
    Card c; \
    c.kind = CAT; \
    c.sticker = CAT; \
    c.number = 2; \
    c.suit = 2; \
    c.attribute = 1; \
    push_back( p->judgeCards, c ); \
    

void playerCard( Player *player, int *numOfBang ) {
    int *color = NULL;
    int num = setColor( &color, -1, -1, 1, player->handcard, 3 );
    if ( *numOfBang == 0 ) {
        setColor( &color, -1, BANG, 0, player->handcard, 0 );
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
        system("clear");
        printUI( player );
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
            printUI( player );
            printHandCard( player->handcard, color, true);
            Card tmp = get_element( player->handcard, choice-1 );
            printf( "Are you sure you want to use the [%s] card? (Y/n) ", cardKindName[tmp.sticker] );
            fgets(input, 100, stdin);
            if (( input[0] == 'Y' || input[0] == 'y' ) && tmp.sticker != NONE ) {
                warn = false;
                if ( tmp.is_orange ) {
                    if ( tmp.sticker == BEER && Beer( player, NULL ) ) {
                        discardCard( player->handcard, choice-1 );
                    }
                    else if ( orangeCards[tmp.sticker]( player ) ) {
                        discardCard( player->handcard, choice-1 );
                    }
                }
                else {
                    blueCards[tmp.sticker]( player, choice-1 );
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
}

// 判斷階段：炸藥->監獄
void judgeTurn( Player *player, Player *nextPlayer ) {
  
  bool judge = false;
  // 判斷炸藥
  for ( int i = 0; i < player->judgeCards->size; i++ ) {
    Card card = get_element( player->judgeCards, i );
    if ( card.kind == DYNAMITE ) {
      printUI( player );
      printf( "%sJudge dynamite%s\n", BLUE, RESET );
      judge = judgeFunc( player, DYNAMITE );
      if ( judge ) {
        printf( "%sJudgment successful%s\n", GREEN, RESET );
        takeCard( player->judgeCards, nextPlayer->judgeCards, i );
      }
      else {
        printf( "%sJudgment failed%s\n", RED, RESET );
        printf( "Player %s loses 3 hp", player->name );
        ENTER;
        HPModify( NULL, player, -3, DYNAMITE );
        if ( GAME_STATE == 0 ) return;
      }
    }
  }

  // 判斷監獄
  for ( int i = 0; i < player->judgeCards->size; i++ ) {
    Card card = get_element( player->judgeCards, i );
    if ( card.kind == JAIL ) {
      printUI( player );
      printf( "%sJudge jail%s\n", BLUE, RESET );
      judge = judgeFunc( player, JAIL );
      discardCard( player->judgeCards, i );
  
      if ( judge ) {
        printf( "%sThe jailbreak is successful, you can continue this round.%s\n", GREEN, RESET );
      }
      else{
        printf( "%sIf the jailbreak fails, this round will be suspended once.%s\n", RED, RESET );
        player->state = DISCARD_CARD;
      }
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


void discardTurn( Player *player ) {
    if( player->handcard->size <= player->hp ) {
        return; // 少於hp不用丟
    }
    
    if( player->state == IS_DEAD ) {
        return; // 死了
    }
    
    int num = player->handcard->size - player->hp;
    bool is_discard = false;
    while ( num ) {
      printUI( player );
      printf("You have to discard %d handcard(s)\n", num );
      ENTER;
      is_discard = chooseCard( player, player->handcard, -1, NULL, false, true ).number > 0 ? true : false;
      if ( is_discard ) num--;
    }
}

int main()
{
    init_card();
    game_prepare();

    int i = SHERIFF_POSITION; //從警長開始
    int numOfBang = PLAYERS_LIST[i].numOfBang; // -1表示可以連續無限次
    Player *p = PLAYERS_LIST + i;

    // 發牌給所有玩家
    if ( GAME_STATE == NOT_YET_START ) {
        for ( int i = 0; i <= PLAYERS_NUM; i++ ) {
          cardHandler( PLAYERS_LIST + i, PLAYERS_LIST[i].hp );
          // skills[(PLAYERS_LIST + j))->role]( &p );
          PLAYERS_LIST[i].state = FINISH_TIHS_TURN;
          
        }
        GAME_STATE = IN_ROUND;
    }

    
    TESTCASE
    p->state = JUDGE;
    
    while ( GAME_STATE == IN_ROUND && GAME_STATE != END ) 
    {
        printUI( p );

        // call skill function
        // for ( int j = 0; j < 15; j++ ) {
        //   skills[(PLAYERS_LIST + j))->role]( &p );
        // }

        if ( p->state == JUDGE )
        {
          // find next player
          int j = i;
          do {
            j++;
            if ( j == PLAYERS_NUM ) j = 0;  
          } while ( PLAYERS_LIST[j].state == IS_DEAD );
          judgeTurn( p, PLAYERS_LIST+j );
        }
        else if ( p->state == GET_CARD )
        {
            if ( cardHandler( p, 2 ) < 2 ) {
                puts( "場上卡片不足" );
                return 0;
            }
            puts( "Two cards from the deck" );
            p->state = PLAY_CARD;
            puts( "State: Get Card -> Player Card" );
            ENTER;
        }
        else if ( p->state == PLAY_CARD )
        {
            playerCard(p, &numOfBang);
        }
        else if ( p->state == DISCARD_CARD )
        {
          discardTurn( p );
          p->state = FINISH_TIHS_TURN;
          puts( "State: Judge -> Get Card" );
          ENTER;
        }
        else if( p->state == FINISH_TIHS_TURN )
        {
            printf( "Player %s ends the round\n", PLAYERS_LIST[i].name );
            // find next player
            do {
              i++;
              if ( i == PLAYERS_NUM ) i = 0;  
            } while ( PLAYERS_LIST[i].state == IS_DEAD );
            
            p = &(PLAYERS_LIST[i]);
            PLAYERS_LIST[i].state = JUDGE;
            numOfBang = PLAYERS_LIST[i].numOfBang;
            printf( "Player %s starts turn", p->name );
        }
        
    }
    
    return 0;
}

