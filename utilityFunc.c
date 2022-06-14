#include "utilityFunc.h"

void print_player(int th)
{   
    printf("Player %d\n", th+1);
    printf("Name : %s\n", PLAYERS_LIST[th].name);
    printf("Role : %d\n", PLAYERS_LIST[th].role);
    printf("State : %d\n", PLAYERS_LIST[th].state);
    // printf("Equipment : ");
    // for(int j = 0; j < PLAYERS_LIST[th].equipment->size; j++) {
    //   printf("%d ", PLAYERS_LIST[th].equipment->data[j].kind); 
    // }
    printf( "Weapon : " );
    for ( int i = 0; i < PLAYERS_LIST[th].weapon->size; i++ ) {
        printf( "%d ", PLAYERS_LIST[th].weapon->data[i].kind );
    }
    printf( "\nShield : " );
    for ( int i = 0; i < PLAYERS_LIST[th].shield->size; i++ ) {
        printf( "%d ", PLAYERS_LIST[th].shield->data[i].kind );
    }
    printf( "\nDistance_item : " );
    for ( int i = 0; i < PLAYERS_LIST[th].distance_item->size; i++ ) {
        printf( "%d ", PLAYERS_LIST[th].distance_item->data[i].kind );
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

bool discardCard( Card_vector * cards, int index ) {
    if ( cards->size <= index ) return false;
    Card tmp = get_element( cards, index);
    remove_element((cards), index);
    tmp.sticker = tmp.kind;
    push_back(discardPile, tmp);
    return false;
}

// 棄掉所有的牌，包含手牌、装備
void discardAllCard( Player *player ) {
    if ( player == NULL ) return;
    for ( int i = 0; i < player->handcard->size; i++ ) {
        Card tmp = pop_back(player->handcard);
        tmp.sticker = tmp.kind;
        push_back(discardPile, tmp);
    }
    for ( int i = 0; i < player->weapon->size; i++ ) {
        Card tmp = pop_back(player->weapon);
        tmp.sticker = tmp.kind;
        push_back(discardPile, tmp);
    }
    for ( int i = 0; i < player->shield->size; i++ ) {
        Card tmp = pop_back(player->shield);
        tmp.sticker = tmp.kind;
        push_back(discardPile, tmp);
    }
    for ( int i = 0; i < player->distance_item->size; i++ ) {
        Card tmp = pop_back(player->distance_item);
        tmp.sticker = tmp.kind;
        push_back(discardPile, tmp);
    }
}



void printCard( Card card ) 
{
    // printf( "is_orange: %d\n", card.is_orange );
    printf( "suit: %s\n", suitName[card.suit] );
    printf( "number: %d\n", card.number );
    // printf( "attribute: %d\n", card.attribute );
    printf( "kind: %s\n", roleName[card.kind] );
    printf( "Sticker: %s\n", roleName[card.sticker] );
}

void clean_buffer(char *arr)
{
    if(arr[strlen(arr)-1] == '\n')
        arr[strlen(arr)-1] = 0;
    else {
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
        if(choice >= min && choice <= max) break;
    }  

    free( input );
    return choice;
}

// 把牌從 player1第[index]張牌給player2, index = 0, 1, 2....
bool takeCard( Card_vector *p1, Card_vector *p2, int index ) {
  if ( p1 == NULL || p2 == NULL ) return false;
  if ( p1->size <= index ) {
    return false;
  }

  Card card = get_element( p1, index );
  remove_element( p1, index );
  card.sticker = card.kind;
  push_back( p2, card );

  return true;
}

// 把player 1 所有的牌給 player 2，包含手牌、装備
void takeAllCards( Player *p1, Player *p2 ) {
    if ( p1 == NULL || p2 == NULL ) return;
    for ( int i = 0; i < p1->handcard->size; i++ ) {
        Card card = pop_back( p1->handcard );
        card.sticker = card.kind;
        push_back( p2->handcard, card );
    }
    for ( int i = 0; i < p1->weapon->size; i++ ) {
        Card card = pop_back( p1->weapon );
        card.sticker = card.kind;
        push_back( p2->handcard, card );
    }
    for ( int i = 0; i < p1->shield->size; i++ ) {
        Card card = pop_back( p1->shield );
        card.sticker = card.kind;
        push_back( p2->handcard, card );
    }
    for ( int i = 0; i < p1->distance_item->size; i++ ) {
        Card card = pop_back( p1->distance_item );
        card.sticker = card.kind;
        push_back( p2->handcard, card );
    }
}