#include "init.h"
#include "game.h"

#define TESTCASE \
    // Card c; \
    // c.kind = JAIL; \
    // c.sticker = JAIL; \
    // c.number = 2; \
    // c.suit = 2; \
    // c.attribute = 1; \
    // c.is_orange = false; \
    // push_back( p->handcard, c ); \

    

int main()
{
  init_card();
  gameStartUI();
  game_prepare();

  int i = SHERIFF_POSITION; //從警長開始
  int numOfBang = PLAYERS_LIST[i].numOfBang; // -1表示可以連續無限次
  Player *p = PLAYERS_LIST + i;

  // 發牌給所有玩家
  if ( GAME_STATE == NOT_YET_START ) {
    for ( int i = 0; i < PLAYERS_NUM; i++ ) {
      Player *tmpPlayer = PLAYERS_LIST + i;
      if ( tmpPlayer->state == IS_DEAD ) continue;
      
      cardHandler( tmpPlayer, tmpPlayer->hp );
      ASSERT( SKILL_RANGE(tmpPlayer->role) );
      if ( SKILL_RANGE(tmpPlayer->role) )
        skills[tmpPlayer->role]( tmpPlayer );
      tmpPlayer->state = FINISH_TIHS_TURN;
      
    }
    GAME_STATE = IN_ROUND;
  }

  
  TESTCASE
  p->state = JUDGE;
  
  while ( GAME_STATE == IN_ROUND && GAME_STATE != END ) 
  {
    printUI( p, "" );

    // call skill function
    for ( int j = 0; j < PLAYERS_NUM; j++ ) {
      Player *tmpPlayer = PLAYERS_LIST + j;
      if ( tmpPlayer->state == IS_DEAD ) continue;
      ASSERT( SKILL_RANGE( tmpPlayer->role)  );
      if ( SKILL_RANGE( tmpPlayer->role) )
        skills[tmpPlayer->role]( tmpPlayer );
    }

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
        playerCard(p);
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
        PLAYERS_LIST[i].numOfBang = 0;
        // printf( "Player %s starts turn", p->name );
    }
    
  }
    
    return 0;
}

