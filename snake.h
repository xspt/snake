#ifndef SNAKE_H
#define SNAKE_H
#include <stdlib.h>

struct point {
   int x;
   int y;
};

// Our snake will be represented by a linked list
typedef struct node {
   struct point p;
   struct node * next;
} node_t;

// Creates a new node that points to next, returns the pointer to the head
node_t * createHead(node_t * next, struct point p);

// Given the first node of the list, deletes the last node and returns its pointer
node_t * deleteTail(node_t * head);

// Given the first node of the list, free all nodes
void deleteSnake(node_t * head);

void forEach(node_t * head, void (*callback)(node_t * node));

// Checks if point p is colliding with any of the nodes
int isColliding(node_t * head, struct point p);

#endif
