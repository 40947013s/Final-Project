#pragma once

void print_player(int th);
int cardHandler( Player * player, int num );
bool discardCard( Player * player, int index );
bool printCard( Card card );
void clean_buffer(char *arr);
int scan(int min, int max, char *str
bool takeCard( Player *p1, Player *p2, int index );