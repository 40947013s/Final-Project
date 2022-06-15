#include "vector.h"

Card_vector *create_vector(size_t n) {
    Card_vector *p = (Card_vector *) malloc(sizeof(Card_vector));
    if(p) {
        p->data = (Card*)malloc(n * sizeof(Card));
        p->size = 0;
        p->allocate_size = n;
    }
    return p;
}

void delete_vector(Card_vector *v) {
    if(v) {
        free(v->data);
        free(v);
    }
}

size_t resize_vector(Card_vector *v, size_t n) {
    
    if(v) {
        Card *p = (Card*)realloc(v->data, ( v -> allocate_size + n ) * sizeof(Card));
        if(p) {
            v->data = p;
            v->allocate_size += n;
        }
        return v->allocate_size;
    }
    return 0;
}


static void _setValue(const Card_vector *v, size_t n, Card value) {
    if(v && n < v->allocate_size) {
        v->data[n].attribute = value.attribute;
        v->data[n].kind = value.kind;
        v->data[n].number = value.number;
        v->data[n].suit = value.suit;
        v->data[n].sticker = value.sticker;
        v->data[n].is_orange = value.is_orange;
    }
}

Card get_element(Card_vector *v, size_t n) {
    if(v && n < v->size) {
        return v->data[n];
    }

    Card c;
    return c;
}

void push_back(Card_vector *v, Card x) {
  if ( v == NULL )
      v = create_vector(5);
  if(v) {
        if( v->size >= v->allocate_size ) {
            resize_vector(v, 5);
        }
        
        _setValue(v, v->size, x);
        v->size++;
    }
}

void clear_vector(Card_vector *v) {
    if(v) {
        v->size = 0;
    }
}

bool isEmpty(Card_vector *v) {
    if(v) {
        return v->size <= 0;
    }
    return true;
}

Card pop_back(Card_vector *v) {
    if(v) {
        if(v->size > 0) {
            Card x = v->data[v->size - 1];
            v->size--;
            return x;
        }
    }
    Card c;
    return c;
}

bool remove_element(Card_vector *v, int index) {
    
    if(v) {
        if(index < v->size) {
            for(int i = index; i < v->size - 1; i++) {
                v->data[i] = v->data[i + 1];
            }
            v->size--;
            return false;
        }
    }
    return false;
}