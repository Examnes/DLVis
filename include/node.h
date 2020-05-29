#if !defined(NODE_H)
#define NODE_H

#include <stdint.h>

struct node
{
    node* previous;
    node* next;
    uint32_t data;
};

#define initalize_node(n,_data) {n = new node; n->data = _data; n->next = nullptr; n->previous = nullptr;}

#endif // NODE_H
