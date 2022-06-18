#include"skillFunc.h"
#include"structList.h"
#include"stdbool.h"
#include"utilityFunc.h"
#include"vector.h"

// 內建酒桶 若有兩個可以執行兩次
void fJourdonnais( void* this ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    Card barrel;
    barrel.suit = -1, barrel.kind = BARREL, barrel.sticker = BARREL;
    if( player->state == SET ) {
      printf( "Active Jourdonnais's skill\n" );
      printf( "You have a build-in barrel\n" );
      ENTER;
      push_back(player->shield, barrel);
    }    
};

// 其他玩家看他+1
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
    printf( "Active Paul Regret's skill\n" );
    printf( "Everyone's distance to you +1\n" );
    ENTER;
  }
};

// 他看其他玩家-1
void fRose_Doolan( void* this ){
  if ( this == NULL ) return;
  Player* player = (Player*)this;
  
  if(player->state == SET){
    int id = player->id;
    for(int j=0;j<10;j++) {
      if(id!=j)
      {
        DISTANCE[id][j]--;
        OFFSET_DISTANCE[id][j]--;
      }
    }

    printf( "Active Rose_Doolan's skill\n" );
    printf( "Your distance to everyone's -1\n" );
    ENTER;
    
  }

};

// 兩個(酒桶/失手)
void fSlab_the_Killer( void* this ){
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    if( player->state == SET ) {
      player->attack_power = 2;
    }
};

// 在自身回合 bang無上限 (PLAY_CARD 和 DISCARD_CARD 狀態前要進入)
void fWilly_the_Kid( void* this ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    if( player->state == SET ) {
      
        player->numOfBang = -1; //無上限
  
    }
};



// 如果是抽牌階段 -> 選第一抽牌庫or別人的手牌
void fJesse_Jones( void* this ){
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    
    if( player->state == GET_CARD ){ 
      printf( "Active Jesse Jones's skill\n" );
      int choice = 0;
      while ( 1 ) {
        choice = scan(0, 1, "Do you want to get a card from other players?  ( 0 : No, 1 : Yes ) :\n");
        if(choice == 0)
        {
          cardHandler( player, 2 );
          return;
        }
        else
        {
  
          Player *target = choosePlayer( player, -1 );
          if ( chooseCard( player, target->handcard, -1, player->handcard, false, false ).number == -1 )
            continue;
          cardHandler( player, 1 );
          player->state = PLAY_CARD;
          puts( "State change from GET_CARD to PLAY_CARD" );
          ENTER;
          return;
        }
      }
      
    }

}

// 如果是抽牌階段 -> 抽兩張牌 -> if(第二張如果是紅心或方塊) 再抽一張
void fBlack_Jack( void* this ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    if ( player->state == GET_CARD ) {
      cardHandler( player, 2 );
      printf( "Judge Black Jack's skill...\n" );
      ENTER;
      
      int size = player->handcard->size;
      Card card = get_element( player->handcard, size-1 );
      int suit = card.suit;
      printf( "Active Black Jack's skill\n" );
      puts( "Get an extra card if the suit is heart or diamond\n" );
      printf( "%sJudge card%s\n\n", BLUE, RESET );
      if( suit == 2 || suit == 3) {
        printCard( card, GREEN );  
        puts( "Get an extra card." );
        ENTER;
        cardHandler( player, 1 );
      } else {          
        printCard( card, RED );    
        puts( "Fail." );
        ENTER;
      }
      player->state = PLAY_CARD;
      puts( "State change from GET_CARD to PLAY_CARD" );
      ENTER;
        
    }            
}

// 如果是抽牌階段 -> 抽三張牌 -> 選一張牌堆頂端
void fKit_Carlson( void* this ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
  
    if( player->state == GET_CARD) {
      Player *tmpPlayer = malloc( sizeof(Player) * 1 );
      tmpPlayer->handcard = create_vector( 5 );
      tmpPlayer->state = IS_DEAD;
      
      cardHandler( tmpPlayer, 3 );
      printf( "Active Kit Carlson's skill\n" );
      puts( "You can get 3 of 2 cards from deck" );
      ENTER;

      int n = 2;
      Card card;
    
      while ( n ) {
        
        card = chooseCard( player, tmpPlayer->handcard, -1, player->handcard, false, true );
        if ( card.number != -1 ) n--;
        else {
          printf( "Active Kit Carlson's skill\n" );
          puts( "You can get 3 of 2 cards from deck" );
          puts( "You need to take card!" );
          ENTER;
        }
      }

      ASSERT( tmpPlayer->handcard->size == 1 );
      takeCard( tmpPlayer->handcard, deck, 0 );
      
      player->state = PLAY_CARD;
      puts( "State change from GET_CARD to PLAY_CARD" );
      ENTER;

      delete_vector( tmpPlayer->handcard );
      free( tmpPlayer );
    }   

}

// 如果是抽牌階段 -> 選第一抽(牌庫or棄牌)
void fPedro_Ramirez( void* this ) {
    if ( this == NULL ) return;
    Player* player = (Player*)this;
    int size = 0;
    
    if( player->state == GET_CARD ) {        
      if( !isEmpty(discardPile) ) {
        printf( "Active Pedro Ramirez's skill\n" );
        puts( "You can get first from deck or discard pile" );
        puts( "Enter where you want to get card ( 0 : discard, 1 : deck ) :" );
        
        int choice = scan(0, 1, "");
        if(choice == 1) cardHandler( player, 1 );
        else {
            Card card = pop_back(discardPile);
            push_back(player->handcard, card);
        }
      }
      else {
        // printf("DISCARD_PILE is empty.\n");
        cardHandler( player, 1 );
      }
      
      cardHandler( player, 1 );
      player->state = PLAY_CARD;
      puts( "State change from GET_CARD to PLAY_CARD" );
      ENTER;
        
    }
 }



// if(被扣一滴血) 抽一張牌
void fBart_Cassidy( void* this ) {
  if ( this == NULL ) return;
  Player* player = (Player*)this;
  if ( player->state == MINUS_HP ) {
    printf( "Active Bart Cassidy's skill\n" );
    printf( "You can get a card when hp-1\n" );
    ENTER;
    cardHandler( player, 1 );
  }
}
// 任何時刻丟兩張手牌換一滴血
void fSid_Ketchum( void* this ) {
  if ( this == NULL ) return;
  Player* player = (Player*)this;    
  if( GAME_STATE != IN_ROUND || player->state == DISCARD_CARD ) return;
  if( player->handcard->size < 2 || player->hp >= player->hp_limit ) return;
  
  while( player->handcard->size >= 2 ) {
    printf( "Active Sid Ketchum's skill\n" );
    char str[] = "Do you want to discard 2 cards to add 1 hp ( 0 : NO, 1 : YES ) ";
    int choice = scan(0, 1, str );
    if( choice == 0 ) return;        
    
    for(int i = 0; i < 2; i++) {
      Card c;
      c.number = -1;
      bool warn = false;
      while ( c.number == -1 ) {
        if ( warn ) puts( "You have to discard a handcard" );
        c = chooseCard( player, player->handcard, -1, NULL, false, true );
        warn = true;
      }
    } 
    puts( "Suceed." );
    HPModify( player, player, 1, NONE );
  }  

}


// if(沒手牌) 抽牌
void fSuzy_Lafayette( void* this ) {
    if ( this == NULL ) return;
    if ( GAME_STATE != IN_ROUND ) return;
    Player* player = (Player*)this;
    if ( player->handcard->size <= 0 ) {
      printf( "Active Suzy Lafayette's skill\n" );
      printf( "You can draw a card when you don't have any handcard.\n" );
      ENTER;
      cardHandler( player, 1 );
      puts( "Get 1 hand card" );
      ENTER;        
    }  
}


// 如果是決鬥or出牌 -> 失手 當 bang
// 如果是被攻擊 -> bang 當 失手
void fCalamity_Janet( void* this ) {
  if ( this == NULL ) return;
  Player* player = (Player*)this;
  int size = player->handcard->size;
  if ( player->state == AFTER_GET ) {
    printf( "Active Calamity Janet's skill\n" );
    puts( "You can see MISS as BANG, and see BANG as MISS" );
    ENTER;
    
    for(int i = 0; i < size; i++) {             
      if( get_element( player->handcard, i ).kind == MISSED) {
        player->handcard->data[i].sticker = BANG;                
      }
      if( get_element( player->handcard, i ).kind == BANG) {
        player->handcard->data[i].sticker = MISSED;                
      }
    }
  }
}


// -------------------
// if(IS_ATTACK) 從傷害玩家抽取傷害數手牌，炸彈不算
void fEl_Gringo( Player *player, Player *attacker, int num, int kind ) {
    if ( player == NULL || attacker == NULL ) return;
    if( player->state == MINUS_HP && kind != DYNAMITE && player->role == El_Gringo ) {
      int card_size = attacker->handcard->size;
      num = -num;
      printf( "Active El Gringo's skill\n" );
      printf("Since you are attacked by attacker and lose 1 health,\n" );
      printf( "you can draw 1 card from the %s's handcard\n", attacker->name );
      ENTER;

      while ( num && !isEmpty( attacker->handcard) ) {
        printf("You can choose %d cards from Player %s\n", num, attacker->name);
        chooseCard( player, attacker->handcard, -1, player->handcard, false, false );
        num--;
      }

      if ( num ) {
        printf( "The skill is invalid, because the attacker's hand is insufficient\n" );
        ENTER;
      }
      
    }
        
}

void fLucky_Duke( void *this ){}
void nullFunc( void *this){}
