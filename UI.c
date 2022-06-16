#include "UI.h"

void printHandCard( Card_vector *cards, int color[], bool visible ) {
    if ( isEmpty( cards ) ) {
        printf("There is no card\n");
        return;
    }
    puts("");
    printf( "%-15s", "Card" );
    char *tmp, tmp2[5];
    tmp = (char *)calloc( strlen("Card") + 6, sizeof(char) );    
    for ( int i = 1; i <= cards->size; i++ ) {
        strcpy( tmp, "Card" );    
        sprintf( tmp2, "%d", i );
        strcat( tmp, tmp2 );
        printf( "%s%-15s%s", Color[color[i-1]], tmp, Color[0] );
    }
    free( tmp );

    puts("");
    printf( "%-15s", "Suit" );
    for (int i = 0; i < cards->size; i++) {
        Card card = cards->data[i];
        printf("%s%-15s%s", Color[color[i]], (visible) ? suitName[card.suit] : "*****", Color[0]);
    }
    puts("");

    printf( "%-15s", "Number" );
    for (int i = 0; i < cards->size; i++) {
        Card card = cards->data[i];
        printf("%s%-15d%s", Color[color[i]], (visible) ? card.number : 0, Color[0] );
    }
    puts("");

    printf( "%-15s", "Kind" );
    for (int i = 0; i < cards->size; i++) {
        Card card = cards->data[i];
        printf("%s%-15s%s", Color[color[i]], (visible) ? cardKindName[card.sticker] : "*****", Color[0] );
    }
    puts("");
}
// 若不指定index,kind,attribute，則填入-1
// if index == -1 and kind == -1 and attribute == -1, then  all the card will be colored
int setColor( int **c, int index, int kind, int attribute, Card_vector* cards, int color ) {
    if ( cards == NULL || isEmpty( cards ) ) {
        return 0;
    }
    
    int num = 0;
    if ( *c == NULL ) {
        *c = (int*)calloc( cards->size, sizeof(int));
        memset( *c, 0, cards->size * sizeof(int) );
    }

    if ( index == -1 && kind == -1 && attribute == -1 ) {
        for ( int i = 0; i < cards->size; i++ ) {
          Card tmp = get_element( cards, i );
          if ( tmp.suit == -1 ) (*c)[i] = 8;
          else (*c)[i] = color;
        }
        return cards->size;
    }

    if ( index != -1 ) {
      Card tmp = get_element( cards, index );
      if ( tmp.suit == -1 ) (*c)[index] = 8;
      else (*c)[index] = color;
        num++;
    }
    if ( kind != -1 ) {
        for ( int i = 0; i < cards->size; i++ ) {
          Card tmp = get_element( cards, i );
          if ( tmp.sticker == kind ) {
            if ( tmp.suit == -1 ) (*c)[i] = 8;
            else (*c)[i] = color;
              num++;
          }
        }
    }
    if ( attribute != -1 ) {
        for ( int i = 0; i < cards->size; i++ ) {
          Card tmp = get_element( cards, i );  
          if ( cards->data[i].attribute == attribute ) {
            if ( tmp.suit == -1 ) (*c)[i] = 8;
            else (*c)[i] = color;
            num++;
          }
        }
    }

    return num;
}

void printUI( Player *nowPlayer ) {
    int indexOfSheriff = SHERIFF_POSITION;
    int indexOfNowPlayer = 0;
    // find the position of the now player
    for ( int i = 0; i < PLAYERS_NUM; i++ ) {
        if ( PLAYERS_LIST[i].id == nowPlayer->id ) {
            indexOfNowPlayer = i;
            break;
        }
    }

    system("clear");
    puts("---------------------------------------------------------------------------------------");
    printf( "%s%-15s%s", GRAY_BACK, "ID", RESET );
    for ( int i = 0; i < PLAYERS_NUM; i++ ) {
        printf( "%s%-18d%s", GRAY_BACK, PLAYERS_LIST[i].id, RESET );
    }
    puts("");

    printf( "%-15s", "Name" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( PLAYERS_LIST[i].state == IS_DEAD )
            printf("%s%-18s%s", GRAY, PLAYERS_LIST[i].name, RESET );
        else if ( i == indexOfNowPlayer )
            printf("%s%-18s%s", YELLOW, PLAYERS_LIST[i].name, RESET );
        else
            printf("%s%-18s%s", ( i == indexOfSheriff ) ? RED : RESET, PLAYERS_LIST[i].name, RESET );
    }
        
    puts("");

    printf( "%-15s", "HP" );
    for (int i = 0; i < PLAYERS_NUM; i++){
        if ( PLAYERS_LIST[i].state == IS_DEAD )
            printf("%s%-18d%s", GRAY, PLAYERS_LIST[i].hp, RESET);
        else if ( i == indexOfNowPlayer )
            printf("%s%-18d%s", YELLOW, PLAYERS_LIST[i].hp, RESET);
        else
            printf("%s%-18d%s", ( i == indexOfSheriff ) ? RED : RESET, PLAYERS_LIST[i].hp, RESET);
    }
    puts("");

    printf( "%-15s", "HandCard" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( PLAYERS_LIST[i].state == IS_DEAD )
            printf("%s%-18d%s", GRAY, PLAYERS_LIST[i].handcard->size, RESET);
        else if ( i == indexOfNowPlayer )
            printf("%s%-18d%s", YELLOW, PLAYERS_LIST[i].handcard->size, RESET);
        else
            printf("%s%-18d%s", ( i == indexOfSheriff ) ? RED : RESET, PLAYERS_LIST[i].handcard->size, RESET );
    }
    puts("");

    printf( "%-15s", "Role" );
    for (int i = 0; i < PLAYERS_NUM; i++) {

        if ( PLAYERS_LIST[i].state == IS_DEAD )
            printf("%s%-18s%s", GRAY, roleName[PLAYERS_LIST[i].role], RESET);
        else if ( i == indexOfNowPlayer )
            printf("%s%-18s%s", YELLOW, roleName[PLAYERS_LIST[i].role], RESET);
        else
            printf("%s%-18s%s", ( i == indexOfSheriff ) ? RED : RESET, roleName[PLAYERS_LIST[i].role], RESET );
      
      
    }
        
    puts("");

    printf( "%-15s", "Distance" );
    int nowPlayerID = PLAYERS_LIST[indexOfNowPlayer].id;
    for (int i = 0; i < PLAYERS_NUM; i++) {
        int id = PLAYERS_LIST[i].id;
        if ( PLAYERS_LIST[i].state == IS_DEAD )
            printf("%s%-18d%s", GRAY, DISTANCE[nowPlayerID][id], RESET);
        else if ( i == indexOfNowPlayer )
            printf("%s%-18d%s", YELLOW, DISTANCE[nowPlayerID][id], RESET);
        else
            printf("%s%-18d%s", ( i == indexOfSheriff ) ? RED : RESET, DISTANCE[nowPlayerID][id], RESET );
    }
    puts("");

    printf("%-15s", "Identity");
    for (int i = 0; i < PLAYERS_NUM; i++) {
    #ifdef DEBUG
        if ( PLAYERS_LIST[i].state == IS_DEAD )
            printf("%s%-18s%s", GRAY, identityName[PLAYERS_LIST[i].identity], RESET);
        else if ( i == indexOfNowPlayer )
            printf("%s%-18s%s", YELLOW, identityName[PLAYERS_LIST[i].identity], RESET);
        else
            printf("%s%-18s%s", ( i == indexOfSheriff ) ? RED : RESET, identityName[PLAYERS_LIST[i].identity], RESET );
    #endif

    #ifndef DEBUG
        if ( PLAYERS_LIST[i].state == IS_DEAD )
            printf("%s%-18s%s", GRAY, identityName[PLAYERS_LIST[i].identity], RESET);
        else if ( i == indexOfNowPlayer )
            printf("%s%-18s%s", YELLOW, "*****", RESET);
        else
            printf("%s%-18s%s", ( i == indexOfSheriff ) ? RED : RESET, "*****", RESET );
    #endif
    }
    puts("");

    printf("%-15s", "State");
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( i == indexOfNowPlayer )
            printf("%s%-18s%s", YELLOW, stateName[PLAYERS_LIST[i].state], RESET);
        else
            printf("%s%-18s%s", ( i == indexOfSheriff ) ? RED : RESET, stateName[PLAYERS_LIST[i].state], RESET );
    }
    puts("");

    printf("%-15s", "Attack");
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( i == indexOfNowPlayer )
            printf("%s%-18d%s", YELLOW, PLAYERS_LIST[i].attack_distance, RESET);
        else
            printf("%s%-18d%s", ( i == indexOfSheriff ) ? RED : RESET, PLAYERS_LIST[i].attack_distance, RESET );
    }
    puts("");

    puts("---------------------------------------------------------------------------------------");
    printf( "%-15s", "Weapon" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( isEmpty(PLAYERS_LIST[i].weapon) )
            printf("%s%-18s%s", RESET, "None", RESET );
        else
            printf("%s%-18s%s", YELLOW, cardKindName[get_element(PLAYERS_LIST[i].weapon, 0).kind], RESET );

    }
    puts("");

    printf( "%-15s", "Shield" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( isEmpty(PLAYERS_LIST[i].shield) )
            printf("%s%-18s%s", RESET, "None", RESET );
        else
            printf("%s%-18s%s", YELLOW, cardKindName[get_element(PLAYERS_LIST[i].shield, 0).kind], RESET );
    }
    puts("");

    printf( "%-15s", "Scope" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( PLAYERS_LIST[i].equipScope == NONE )
            printf("%s%-18s%s", RESET, "None", RESET );
        else
            printf("%s%-18s%s", YELLOW, cardKindName[PLAYERS_LIST[i].equipScope], RESET );
    }
    puts("");

    printf( "%-15s", "Mustang" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( PLAYERS_LIST[i].equipMustang == NONE )
            printf("%s%-18s%s", RESET, "None", RESET );
        else
            printf("%s%-18s%s", YELLOW, cardKindName[PLAYERS_LIST[i].equipMustang], RESET );
    }
    puts("");

    printf( "%-15s", "Judge Card" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( isEmpty(PLAYERS_LIST[i].judgeCards) )
            printf("%s%-18s%s", RESET, "None", RESET );
        else
            printf("%s%-18s%s", YELLOW, cardKindName[get_element(PLAYERS_LIST[i].judgeCards, 0).kind], RESET );
    }
    puts("");

    puts("---------------------------------------------------------------------------------------");

    printf( "%s's turn\n", nowPlayer->name );

}