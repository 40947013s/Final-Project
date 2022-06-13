#include "utilityFunc.h"

void print_player(int th)
{   
    printf("Player %d\n", th+1);
    printf("Name : %s\n", PLAYERS_LIST[th].name);
    printf("Role : %d\n", PLAYERS_LIST[th].role);
    printf("State : %d\n", PLAYERS_LIST[th].state);
    printf("Equipment : ");
    for(int j = 0; j < PLAYERS_LIST[th].equipment->size; j++) {
      printf("%d ", PLAYERS_LIST[th].equipment->data[j].kind); 
    }
    printf("\nHandcard : ");
    for(int j = 0; j < PLAYERS_LIST[th].handcard->size; j++) {
      printf("%d ", PLAYERS_LIST[th].handcard->data[j].kind); 
    }
        
    printf("\n");
    printf("Identity : %d\n\n\n", PLAYERS_LIST[th].identity);    
}

/*棄牌區的牌丟牌庫*/
void shuffle() {
    srand(time(NULL));
    bool is_take[80] = {0};
    int num = discardPile->size;
    for (int i = 0; i < num; i++) {
        int j = rand() % num;
        if (is_take[j]) {
            i--;
            continue;
        }
        is_take[j] = true;
        Card tmp = get_element(discardPile, j);
        push_back(deck, tmp);
    }
    clear_vector(discardPile);
}

int cardHandler( Player * player, int num ) {
    if ( num > deck->size ) {
        shuffle();
    }

    for ( int i = 0; i < num; i++ ) {
        if ( deck->size == 0 ) return i;
        Card tmp = pop_back(deck);
        tmp.sticker = tmp.kind;
        push_back(player->handcard, tmp);
    }

    return num;
}

bool discardCard( Player * player, int index ) {
    if ( player->handcard->size <= index ) return false;
    Card tmp = get_element(player->handcard, index);
    remove_element((player->handcard), index);
    tmp.sticker = tmp.kind;
    push_back(discardPile, tmp);
  
    // for ( int i = 0; i < player->handcard->size; i++ ) {
    //     Card tmp = get_element(player->handcard, i);
    //     if ( tmp.kind == card_kind ) {
    //         remove_element((player->handcard), i);
    //         push_back(discardPile, tmp);
    //         return true;
    //     }
    // }

    return false;
}



bool printCard( Card card ) 
{
    char *roleKindName[] = {
        "Bart Cassidy", "Black Jack", "Calamity Janet", "El Gringo", 
        "Jesse Jones", "Jourdonnais", "Kit Carlson", "Lucky Duke",
         "Paul Regret", "Pedro Ramirez", "Rose Doolan", "Sid Ketchum",
        "Slab the Killer", "Suzy Lafayette", "Vulture Sam", "Willy the Kid"
    };
    char *suitName[] = { "黑桃", "愛心", "方塊", "梅花" };
    // printf( "is_orange: %d\n", card.is_orange );
    printf( "suit: %s\n", suitName[card.suit] );
    printf( "number: %d\n", card.number );
    // printf( "attribute: %d\n", card.attribute );
    printf( "kind: %s\n", roleKindName[card.kind] );
    printf( "Sticker: %s\n", roleKindName[card.sticker] );
}

void clean_buffer(char *arr)
{
    if(arr[strlen(arr)-1] == '\n')
        arr[strlen(arr)-1] = 0;
    else
    {
        int32_t c = 0;
        while((c = fgetc(stdin)) != '\n' && c != EOF){}
    }
}

int scan(int min, int max, char *str)
{
    int warn = 0, choice = -1;
    char *input = malloc(1000);
    while(1) {
        if(warn) printf("Wrong input.\n");
        printf("%s", str);
        fgets(input, 1000, stdin);
        clean_buffer(input);
        choice = strtol(input, NULL , 10);
        warn++;
        if(choice >= min || choice <= max) break;
    }  

    free( input );
    return choice;
}

// 把牌從 player1第[index]張牌給player2, index = 0, 1, 2....
bool takeCard( Player *p1, Player *p2, int index ) {
  if ( p1 == NULL || p2 == NULL ) return false;
  if ( p1->handcard->size <= index ) {
    return false;
  }

  Card card = get_element( p1->handcard, index );
  remove_element( p1->handcard, index );
  card.sticker = card.kind;
  push_back( p2->handcard, card );

  return true;
}
