#include "snake.h"

node_t * createHead(node_t * next, struct point p) {
   node_t * head = (node_t *) malloc(sizeof(node_t));

   // Make it point to the next node
   head->next = next;
   head->p = p;

   return head;
}

node_t * deleteTail(node_t * head) {
   node_t * current = head;

   while (current->next->next != NULL) {
      current = current->next;
   }

   node_t * deleted = current->next;

   free(current->next);
   current->next = NULL;

   return deleted;
}

void deleteSnake(node_t * head) {
   node_t * current = head;

   while (current != NULL) {
      node_t * next = current->next;
      free(current);
      current = next;
   }
}

void forEach(node_t * head, void (*callback)(node_t * node)) {
   node_t * current = head;

   while (current != NULL) {
      callback(current);
      current = current->next;
   }
}

int isColliding(node_t * head, struct point p) {
   node_t * current = head;

   while (current != NULL) {
      if (current->p.x == p.x && current->p.y == p.y) {
         return 1;
      }
      current = current->next;
   }

   return 0;
}
