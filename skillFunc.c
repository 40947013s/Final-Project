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
void fBart_Cassidy( void* this ){
  if ( this == NULL ) return;
  Player* player = (Player*)this;
  if ( player->state == MINUS_HP ) {
    cardHandler( player, 1 );
  }
}


//    如果是抽牌階段 -> 抽兩張牌 -> if(第二張如果是紅心或方塊) 再抽一張
void fBlack_Jack( void* this ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    if ( player->state == GET_CARD ) {
        cardHandler( player, 2 );
        int size = player->handcard->size;
        Card card = get_element( player->handcard, size-1 );
        int suit = card.suit;
        if( suit == 2 || suit == 3) {
            puts("Get an extra card because of role skill.");
            puts("(the second card you draw is heart or diamond)");
            cardHandler( player, 1 );
        }            
    }        
    puts( "State change from GET_CARD to PLAY_CARD" );
    ENTER;
    player->state == PLAY_CARD;
};


//    其他玩家看他+1
void fPaul_Regret( void* this ){
  if ( this == NULL ) return;
  Player* player = (Player*)this;
  if(player->state == SET) {
    int id = player->id;
      for(int i=0;i<10;i++) {
        if(id!=i) {
          DISTANCE[i][id]++;
          OFFSET_DISTANCE[i][id]++;
        }
      }
  }
};

//    他看其他玩家-1
void fRose_Doolan( void* this ){
  if ( this == NULL ) return;
  Player* player = (Player*)this;

  if(player->state == SET){
    int id = player->id;
    for(int j=0;j<10;j++)
    {
      if(id!=j)
      {
        DISTANCE[id][j]--;
        OFFSET_DISTANCE[id][j]--;
      }
    }
  }
};

//    如果是決鬥or出牌 -> 失手 當 bang
//    如果是被攻擊 -> bang 當 失手
void fCalamity_Janet( void* this ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    int size = player->handcard->size;
    if ( player->state == FIGHT || player->state == PLAY_CARD ) {
        for(int i = 0; i < size; i++) {
            if( get_element( player->handcard, i ).sticker == MISSED) {
                player->handcard->data[i].sticker = BANG;                
            }
        }
    }
    if ( player->state == IS_ATTACKED) {
        for(int i = 0; i < size; i++) {
            if(get_element( player->handcard, i ).sticker == BANG) {
                player->handcard->data[i].sticker = MISSED;
            }
        }
    }
        
};

//    if(IS_ATTACK) 從傷害玩家抽取傷害數手牌，炸彈不算
void fEl_Gringo( Player *player, Player *attacker, int n, int kind ) {
    // if ( this == NULL ) return;
    // Player* player = (Player*)this;
    // Kind kind = (Kind) argv;
    if( player->state == MINUS_HP && kind != DYNAMITE ) {
        int num = n, card_size = attacker->handcard->size;
        if(num >= card_size) {
            printf("Player %s's sum of card is less than or equal to the number you can draw.\n",attacker->name);
            printf("All of this player's card belong to you.\n");
            while(!isEmpty(attacker->handcard)) {
                takeCard( attacker->handcard, player->handcard, 0 );
            }
            // 沒手牌 Suzy 抽牌
            // if( attacker->role == Suzy_Lafayette ) {
            //     fSuzy_Lafayette( attacker );
            // }
         } else {
             printf("You can choose %d cards from Player %s\n", num, attacker->name);
             for(int i = 0; i < num; i++) {
                 int size = attacker->handcard->size;
                 printf("(0~%d) :\n", size-1);
                 int choice = scan(0, size-1, "--> ");
                 takeCard( attacker->handcard, player->handcard, choice );
             }
        }        
    }
        
}

//    如果是抽牌階段 -> 選第一抽牌庫or別人的手牌
void fJesse_Jones( void* this ){
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    char *str = malloc(1000);
    int *store_position = (int *)calloc(PLAYERS_NUM,sizeof(int));
    if( player->state == GET_CARD){ 
      int choice = scan(0, 1, "Do you want to get the card from other players?  ( 0 : No, 1 : Yes ) :\n");
      if(choice == 0)
      {
        cardHandler( player, 2 );
        return;
      }
      else
      {
        int att_arr = 0;
        for(int i=0;i<PLAYERS_NUM;i++)
        {
          Player p = PLAYERS_LIST[i];
          if(p.state != IS_DEAD) 
          {
              if(strcmp(p.name,player->name)==0) //找attacker在array的位置
              {
                att_arr = i;
                  break;
              }
          }  
          
        }
        printf("Enter the player you like to choose: \n");
        int counter=0;
        
        for(int i=0;i<PLAYERS_NUM;i++)
        {
          if(att_arr!=i && PLAYERS_LIST[i].state != IS_DEAD)
          {
            char *tmp = malloc(100);
            sprintf(tmp,"[%d] name: %s ID: %d\n",counter,PLAYERS_LIST[i].name,PLAYERS_LIST[i].id);
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
        takeCard( PLAYERS_LIST[array_position].handcard, player->handcard, which_card );
      }
      cardHandler( player, 1 );
    }

    puts( "State change from GET_CARD to PLAY_CARD" );
    ENTER;
    player->state == PLAY_CARD;
  free(str);
  free(store_position);
};

//    內建酒桶 若有兩個可以執行兩次
void fJourdonnais( void* this ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    Card barrel;
    barrel.suit = -1, barrel.kind = BARREL;
    if( player->state == SET ) {
        push_back(player->judgeCards, barrel);
    }    
}

//    如果是抽牌階段 -> 抽三張牌 -> 選一張丟棄
void fKit_Carlson( void* this ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    int size = 0;
    if ( !isEmpty(deck) ) size = deck->size;
    if( player->state == GET_CARD) {
        cardHandler( &tmpPlayer, 3 );
        printf("Enter your choices to delete :\n");
        for(int i = 0; i < 3; i++) {
            Card card = get_element( tmpPlayer.handcard, i );
            printf("[%d] %d ", i, card.kind);            
        }
        printf("\n");
        int choice = scan(0, 2, "Which role card to choose (0 or 1 or 2): ");
        discardCard( tmpPlayer.handcard, choice );
        
        while( !isEmpty( tmpPlayer.handcard) )     
            takeCard( tmpPlayer.handcard, player->handcard, 0 );         
    }
    puts( "State change from GET_CARD to PLAY_CARD" );
    ENTER;
    player->state == PLAY_CARD;
}

// if(抽牌判定) 可以抽兩張挑一張
Card getJudgementCard( Player *player ) {
  cardHandler( &tmpPlayer, 1 );
  Card c = get_element( tmpPlayer.handcard, 0 );
  if ( player == NULL || player->identity != Lucky_Duke  ) return c;

  // assert player == Lucky_Duke here
  while ( !isEmpty( player->judgeCards ) ) {
    Card card = pop_back( player->judgeCards );
    cardHandler( &tmpPlayer, 1 );
    Card card1 = get_element( tmpPlayer.handcard, 0 );
    Card card2 = get_element( tmpPlayer.handcard, 1 );
    discardCard( tmpPlayer.handcard, 0 );
    discardCard( tmpPlayer.handcard, 1 );
    
    printf( "card1: " );
    printCard( card1 );
    printf( "card2: " );
    printCard( card2 );
  
    char *str = "Choose the card you want to use as a decision card( 1 or 2): ";
    int choice = scan(1, 2, str);
    printf( "You choose the card%d\n", choice );
    ENTER;
    return ( choice == 1 ) ? card1 : card2;
  } 

}

//    如果是抽牌階段 -> 選第一抽(牌庫or棄牌)
void fPedro_Ramirez( void* this ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    int size = 0;
    //if ( !isEmpty(deck) ) size = deck->size;
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
            printf("DISCARD_PILE is empty.\n");
            cardHandler( player, 1 );
        }
      cardHandler( player, 1 );
    }
    puts( "State change from GET_CARD to PLAY_CARD" );
    ENTER;
    player->state == PLAY_CARD;
}

//    任何時刻丟兩張守牌換一滴血 (觸發條件???)
void fSid_Ketchum( void* this ){
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    if(player->handcard->size >= 2) {
        for(int i = 0; i < 2; i++) {
            printf("%d -> ", i);
            for(int j = 0; j < player->handcard->size; j++) {
                printf("[%d] %d ", j, get_element( player->handcard, j).kind );
            } 
          
            printf("\n");
            int size = player->handcard->size;            
            int choice = scan(0, size-1, "Choose which card to delete (exchange hp) :");
            discardCard( player->handcard, choice);
        }
    }    
}

//    對方必須以兩張 (閃躲或是酒桶) 躲 bang
void fSlab_the_Killer( void* this ){
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    if( player->state == SET ) {
        // call BANG function
        player->attack_power = 2;
    }
}

//    if(沒手牌) 抽牌
void fSuzy_Lafayette( void* this ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    if ( player->handcard->size <= 0 && player->state!= FIGHT /*&& player->state != PLAY_CARD*/ ) {
      cardHandler( player, 1 );
    }  
};

//    在自身回合 bang無上限 (PLAY_CARD 和 DISCARD_CARD 狀態前要進入)
void fWilly_the_Kid( void* this ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    if( player->state == PLAY_CARD ) {
        player->numOfBang = -1; //無上限
    } else if( player->state == DISCARD_CARD ) {
        player->numOfBang = 1;
    }
}