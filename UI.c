
#include "UI.h"

void printHandCard( Card_vector *cards, int color[] ) {
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
        printf("%s%-15s%s", Color[color[i]], suitName[card.suit], Color[0]);
    }
    puts("");

    printf( "%-15s", "Number" );
    for (int i = 0; i < cards->size; i++) {
        Card card = cards->data[i];
        printf("%s%-15d%s", Color[color[i]], card.number, Color[0] );
    }
    puts("");

    printf( "%-15s", "Kind" );
    for (int i = 0; i < cards->size; i++) {
        Card card = cards->data[i];
        printf("%s%-15s%s", Color[color[i]], cardKindName[card.sticker], Color[0] );
    }
    puts("");
}
// 若不指定index,kind,attribute，則填入-1
// if index == -1 and kind == -1 and attribute == -1, then  all the card will be colored
int setColor( int **c, int index, int kind, int attribute, Card_vector* handcard, int color ) {
    if ( handcard == NULL || isEmpty( handcard ) ) {
        return 0;
    }
    
    int num = 0;
    if ( *c == NULL ) {
        *c = (int*)calloc( handcard->size, sizeof(int));
        memset( *c, 0, handcard->size * sizeof(int) );
    }

    if ( index == -1 && kind == -1 && attribute == -1 ) {
        for ( int i = 0; i < handcard->size; i++ ) {
            (*c)[i] = color;
        }
        return handcard->size;
    }

    if ( index != -1 ) {
        (*c)[index] = color;
        num++;
    }
    if ( kind != -1 ) {
        for ( int i = 0; i < handcard->size; i++ ) {
            if ( handcard->data[i].sticker == kind ) {
                (*c)[i] = color;
                num++;
            }
        }
    }
    if ( attribute != -1 ) {
        for ( int i = 0; i < handcard->size; i++ ) {
            if ( handcard->data[i].attribute == attribute ) {
                (*c)[i] = color;
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
    puts("---------------------------------------------------------------------------------");
    printf( "%-15s", "ID" );
    for ( int i = 0; i < PLAYERS_NUM; i++ ) {
        printf( "%-15d", PLAYERS_LIST[i].id );
    }
    puts("");

    printf( "%-15s", "Name" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( i == indexOfNowPlayer )
            printf("%s%-15s%s", YELLOW, PLAYERS_LIST[i].name, RESET );
        else
            printf("%s%-15s%s", ( i == indexOfSheriff ) ? RED : RESET, PLAYERS_LIST[i].name, RESET );
    }
        
    puts("");

    printf( "%-15s", "HP" );
    for (int i = 0; i < PLAYERS_NUM; i++){
        if ( i == indexOfNowPlayer )
            printf("%s%-15d%s", YELLOW, PLAYERS_LIST[i].hp, RESET);
        else
            printf("%s%-15d%s", ( i == indexOfSheriff ) ? RED : RESET, PLAYERS_LIST[i].hp, RESET);
    }
    puts("");

    printf( "%-15s", "HandCard" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( i == indexOfNowPlayer )
            printf("%s%-15d%s", YELLOW, PLAYERS_LIST[i].handcard->size, RESET);
        else
            printf("%s%-15d%s", ( i == indexOfSheriff ) ? RED : RESET, PLAYERS_LIST[i].handcard->size, RESET );
    }
    puts("");

    printf( "%-15s", "Role" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( i == indexOfNowPlayer )
            printf("%s%-15s%s", YELLOW, roleName[PLAYERS_LIST[i].role], RESET);
        else
            printf("%s%-15s%s", ( i == indexOfSheriff ) ? RED : RESET, roleName[PLAYERS_LIST[i].role], RESET );
    }
        
    puts("");

    printf( "%-15s", "Distance" );
    int nowPlayerID = PLAYERS_LIST[indexOfNowPlayer].id;
    for (int i = 0; i < PLAYERS_NUM; i++) {
        int id = PLAYERS_LIST[i].id;
        if ( i == indexOfNowPlayer )
            printf("%s%-15d%s", YELLOW, DISTANCE[nowPlayerID][id], RESET);
        else
            printf("%s%-15d%s", ( i == indexOfSheriff ) ? RED : RESET, DISTANCE[nowPlayerID][id], RESET );
    }
    puts("");

    puts("---------------------------------------------------------------------------------");
    printf( "%-15s", "Weapon" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( isEmpty(PLAYERS_LIST[i].weapon) )
            printf("%s%-15s%s", RESET, "None", RESET );
        else
            printf("%s%-15s%s", YELLOW, cardKindName[get_element(PLAYERS_LIST[i].weapon, 0).kind], RESET );

    }
    puts("");

    printf( "%-15s", "Shield" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( isEmpty(PLAYERS_LIST[i].shield) )
            printf("%s%-15s%s", RESET, "None", RESET );
        else
            printf("%s%-15s%s", YELLOW, cardKindName[get_element(PLAYERS_LIST[i].shield, 0).kind], RESET );
    }
    puts("");

    printf( "%-15s", "Scope" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( PLAYERS_LIST[i].equipScope == NONE )
            printf("%s%-15s%s", RESET, "None", RESET );
        else
            printf("%s%-15s%s", YELLOW, cardKindName[PLAYERS_LIST[i].equipScope], RESET );
    }
    puts("");

    printf( "%-15s", "Mustang" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( PLAYERS_LIST[i].equipMustang == NONE )
            printf("%s%-15s%s", RESET, "None", RESET );
        else
            printf("%s%-15s%s", YELLOW, cardKindName[PLAYERS_LIST[i].equipMustang], RESET );
    }
    puts("");

    printf( "%-15s", "Judge Card" );
    for (int i = 0; i < PLAYERS_NUM; i++) {
        if ( isEmpty(PLAYERS_LIST[i].judgeCards) )
            printf("%s%-15s", RESET, "None" );
        else
            printf("%s%-15s", YELLOW, cardKindName[get_element(PLAYERS_LIST[i].judgeCards, 0).kind] );
    }
    puts("");

    puts("---------------------------------------------------------------------------------");

    printf( "%s's turn\n", nowPlayer->name );

}