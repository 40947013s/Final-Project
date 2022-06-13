#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include"structList.h"

Card_vector *create_vector(size_t n);
void delete_vector(Card_vector *v);
size_t resize_vector(Card_vector *v, size_t n);
static void _setValue(const Card_vector *v, size_t n, Card value);
Card get_element(Card_vector *v, size_t n);
void push_back(Card_vector *v, Card x);
void clear_vector(Card_vector *v);
bool isEmpty(Card_vector *v);
Card pop_back(Card_vector *v);
bool remove_element(Card_vector *v, int index);