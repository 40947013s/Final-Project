#include"skillFunc.h"
#include"structList.h"
#include"stdbool.h"
#include"utilityFunc.h"
#include"vector.h"

/*
- Paul Regret        被動       初始化階段 // set
- Rose Doolan        被動       初始化階段 // set
Lucky Duke         主動       判定階段 // judge
- Black Jack         被動       抽牌階段 // get card
Jesse Jones        主動       抽牌階段 
- Kit Carlson        被動       抽牌階段
- Pedro Ramirez      被動       抽牌階段 
- Bart Cassidy       被動       扣血時 // minus_hp
Calamity Janet     主動       出牌階段/被攻擊時 // play_card or is_attacked
- Suzy Lafayette     被動       (沒手牌) // if ( player.handcard->size <= 0 )
                                     // then do somthing
*/

/*
    SET,
    JUDGE,
    GET_CARD,
    PLAY_CARD,
    DISCARD_CARD,
    MINUS_HP,
    FIGHT,
    IS_ATTACKED,
    FINISH_TIHS_TURN,
    IS_DEAD
*/

Player tmpPlayer;

//    if(被扣一滴血) 抽一張牌
void fBart_Cassidy( void* this, void* argv ){
  if ( this == NULL ) return;
  Player* player = (Player*)this;
  if ( player->state == MINUS_HP ) {
    cardHandler( player, 1 );
  }
};


//    如果是抽牌階段 -> 抽兩張牌 -> if(第二張如果是紅心或方塊) 再抽一張
void fBlack_Jack( void* this, void* argv ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    if ( player->state == GET_CARD ) {
        cardHandler( player, 2 );
        int size = player->handcard.size;
        int suit = player->handcard[size-1]->data.suit;
        if( suit == 2 || suit == 3) {
            //printf("Get an extra card.\n");
            cardHandler( player, 1 );
        }            
    }        
};


//    其他玩家看他+1
void fPaul_Regret( void* this, void* argv ){
  if ( this == NULL ) return;
  Player* player = (Player*)this;
  int id = player->id;
  for(int i=0;i<7;i++) {
    if(id!=i) {
      DISTANCE[i][id]++;
    }
  }
};


//    他看其他玩家-1
void fRose_Doolan( void* this, void* argv ){
  if ( this == NULL ) return;
  Player* player = (Player*)this;
  int id = player->id;
  for(int j=0;j<7;j++)
  {
    if(id!=j)
    {
      DISTANCE[id][j]--;
    }
  }
};

//    如果是決鬥or出牌 -> 失手 當 bang
//    如果是被攻擊 -> bang 當 失手
void fCalamity_Janet( void* this, void* argv ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    int size = player->handcard.size;
    if ( player->state == FIGHT || player->state == PLAY_CARD ) {
        for(int i = 0; i < size; i++) {
            if(player->handcard[i].sticker == MISSED) {
                player->handcard[i].sticker == BANG;                
            }
        }
    }
    if ( player->state == IS_ATTACKED) {
        for(int i = 0; i < size; i++) {
            if(player->handcard[i].sticker == BANG) {
                player->handcard[i].sticker == MISSED;                
            }
        }
    }
        
};

//    if(IS_ATTACK) 從傷害玩家抽取傷害數卡牌，炸彈不算
void fEl_Gringo( void* this, void* argv ){};

//    如果是抽牌階段 -> 選第一抽牌庫or別人的牌
void fJesse_Jones( void* this, void* argv ){
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    char *line = malloc(10);
    if( player->state == GET_CARD){ 
      int choice = scan(0, 1, "Do you want to get the card from other players?  ( 0 : No, 1 : Yes ) :\n");
      if(choice == 0)
      {
        cardHandler( player, 2 );
        return;
      }
      else
      {
        int id = scan(0,PLAYERS_NUM, "Enter the player's ID you like to choose: \n");
        while(isEmpty(PLAYERS_LIST[id].handcard))
        {
          printf("Player %d's handcard is empty."\n);
          id = scan(0,PLAYERS_NUM-1, "Please enter again:\n");
        }
        int max = PLAYERS_LIST[id].handcard->size;
        int which_card = scan(1,max,"Choose the card you want from 1 to number of cards: \n");
      }
    }

  cardHandler( player, 1 );
};

void fJourdonnais( void* this, void* argv ){};

// 如果是抽牌階段 -> 抽三張牌 -> 選一張丟棄
void fKit_Carlson( void* this, void* argv ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    int size = deck.size;
    if( player->state == GET_CARD) {
        cardHandler( &tmpPlayer, 3 );
        printf("Enter your choices to delete :\n")
        for(int i = 0; i < 3; i++) {
            Card card = get_element( &tmpPlayer, i );
            printf("[%d] %d ", i, card.kind);            
        }
        printf("\n");
        int choice = scan(0, 2, "Which role card to choose (0 or 1 or 2): ");
        discardCard( &tmpPlayer, choice );
        
        while( !IS_EMPTY(tmpPlayer.handcard) )     
            takeCard( &tmpPlayer, player, 0 );         
    }
};

//    if(抽牌判定) 可以抽兩張挑一張
void fLucky_Duke( void* this, void* argv ){
  if ( this == NULL ) return;
  Player* player = (Player*)this;
  if ( player->state == JUDGE ) {
    while ( !isEmpty( player->judgeCards ) ) {
      Card card = pop_back( player->judgeCards );
      cardHandler( &tmpPlayer, 2 );
      Card card1 = get_element( &tmpPlayer, 0 );
      Card card2 = get_element( &tmpPlayer, 1 );
      discardCard( &tmpPlayer, 0 );
      discardCard( &tmpPlayer, 1 );
      
      printf( "card1: " );
      printCard( card1 );
      printf( "card2: " );
      printCard( card2 );
    
      char *str = "Choose the card you want to use as a decision card( 1 or 2): ";
      int choice = scan(1, 2, str);

      if ( card.kind == JAIL ) {
        // call JAIL function
      }
      else if ( card.kind == DYNAMITE ) {
        // call DYNAMITE function
      }   
      
    }    
  }
};

//    如果是抽牌階段 -> 選第一抽(牌庫or棄牌)
void fPedro_Ramirez( void* this, void* argv ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    int size = deck.size;
    if( player->state == GET_CARD ) {        
        if( !isEmpty(discardPile) ) {
            int choice = scan(0, 1, "Enter your where to get card ( 0 : discard, 1 : deck ) :\n");
            if(choice == 1) cardHandler( player, 1 );
            else {
                Card card = pop_back(discardPile);
                push_back(player->handcard, card);
            }
        }
        else {
            printf("Deck is empty.\n");
            cardHandler( player, 1 );
        }
    }
    cardHandler( player, 1 );
};


void fSid_Ketchum( void* this, void* argv ){};
void fSlab_the_Killer( void* this, void* argv ){};

//    if(沒手牌) 抽牌
void fSuzy_Lafayette( void* this, void* argv ){
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    if ( player.handcard->size <= 0 && player->state!= FIGHT )     {
      cardHandler( player, 1 );
    }  
};
    
void fVulture_Sam( void* this, void* argv ){};
void fWilly_the_Kid( void* this, void* argv ){};