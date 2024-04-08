#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/color.h>
#include <allegro5/timer.h>
#include <allegro5/events.h>
#include <allegro5/allegro_primitives.h>
#include "snake.h"

// Window size
#define WIN_WIDTH 480
#define WIN_HEIGHT 480

// Grid size
#define SCALED_WIDTH 32
#define SCALED_HEIGHT 32

float scaleFactorX;
float scaleFactorY;

void mainLoop(ALLEGRO_EVENT_QUEUE * queue, ALLEGRO_TIMER * timer);
void drawNode(node_t * node);
node_t * reset(node_t * head);
int genRandom(int max);
void spawnApple(struct point * appleLocation);

struct point headPos;
node_t * head = NULL;
struct point applePos;

int main() {
   // Initializing allegro and its components
   if (!al_init()) {
      printf("Failed to init allegro.\n");
      return EXIT_FAILURE;
   }

   if (!al_init_primitives_addon()) {
      printf("Failed to init primitives addon.\n");
   }

   if (!al_install_keyboard()) {
      printf("Failed to init keyboard.\n");
      return EXIT_FAILURE;
   }

   // Around 15 fps
   ALLEGRO_TIMER * timer = al_create_timer(1.0 / 15.0);
   if (!timer) {
      printf("Failed to create timer.\n");
      return EXIT_FAILURE;
   }

   ALLEGRO_EVENT_QUEUE * queue = al_create_event_queue();
   if (!queue) {
      printf("Failed to create event queue.\n");
      return EXIT_FAILURE;
   }

   ALLEGRO_DISPLAY * display = al_create_display(WIN_WIDTH, WIN_HEIGHT);
   if (!display) {
      printf("Failed to create display.\n");
      return EXIT_FAILURE;
   }

   // Registering events in event queue
   al_register_event_source(queue, al_get_keyboard_event_source());
   al_register_event_source(queue, al_get_timer_event_source(timer));
   al_register_event_source(queue, al_get_display_event_source(display));

   scaleFactorX = al_get_display_width(display) / (float) SCALED_WIDTH;
   scaleFactorY = al_get_display_height(display) / (float) SCALED_HEIGHT;

   // Position of the snake tail
   headPos.x = ((al_get_display_width(display) / scaleFactorX) / 4) * scaleFactorX;
   headPos.y = ((al_get_display_width(display) / scaleFactorY) / 2) * scaleFactorY;

   // Create a snake with 3 segments to start the game
   for (int i = 0; i < 3; i++) {
      head = createHead(head, headPos);
      headPos.x = headPos.x + scaleFactorX;
   }

   // Spawn first apple
   spawnApple(&applePos);

   mainLoop(queue, timer);

   // Free resources before exiting
   deleteSnake(head);
   al_destroy_timer(timer);
   al_destroy_display(display);
   al_destroy_event_queue(queue);

   return 0;
}

void mainLoop(ALLEGRO_EVENT_QUEUE * queue, ALLEGRO_TIMER * timer) {
   ALLEGRO_EVENT event;

   enum {up, right, down, left} direction = right;

   bool redraw = false;
   bool canPress = false;

   al_start_timer(timer);
   while(1) {
      al_wait_for_event(queue, &event);
      switch (event.type) {
         // Game logic
         case ALLEGRO_EVENT_TIMER:
            redraw = true;
            canPress = true;

            switch (direction) {
               case up:
                  headPos.y -= scaleFactorY;
                  break;
               case down:
                  headPos.y += scaleFactorY;
                  break;
               case left:
                  headPos.x -= scaleFactorX;
                  break;
               case right:
                  headPos.x += scaleFactorX;
                  break;
            }

            if (isColliding(head->next, head->p) // Check if the head has hit any part of the body
                  || headPos.x < scaleFactorX // Check if the head has hit the left wall
                  || headPos.x >= WIN_WIDTH - scaleFactorX // Check if the head has hit the right wall
                  || headPos.y < scaleFactorY // Check if the head has hit the top wall
                  || headPos.y >= WIN_HEIGHT - scaleFactorY) { // Check if the head has hit the bottom wall
               printf("Colidiu\n");
               direction = right;
               head = reset(head);

               // Spawn apple until it is not inside the snake
               do {
                  spawnApple(&applePos);
               } while (isColliding(head, applePos));
            }

            // If the snake has eaten the apple, spawn a new one and skip deleting the tail
            if (head->p.x == applePos.x && head->p.y == applePos.y) {
               do {
                  spawnApple(&applePos);
               } while (isColliding(head, applePos));
            // If the snake has not eaten a apple, delete the tail
            } else {
               deleteTail(head);
            }

            head = createHead(head, headPos);

            break;
         // User input
         case ALLEGRO_EVENT_KEY_DOWN:
            switch (event.keyboard.keycode) {
               case ALLEGRO_KEY_UP:
                  direction = direction != down && canPress ? up : down;
                  canPress = false;
                  break;
               case ALLEGRO_KEY_DOWN:
                  direction = direction != up && canPress ? down : up;
                  canPress = false;
                  break;
               case ALLEGRO_KEY_LEFT:
                  direction = direction != right && canPress ? left : right;
                  canPress = false;
                  break;
               case ALLEGRO_KEY_RIGHT:
                  direction = direction != left && canPress ? right : left;
                  canPress = false;
                  break;
               // Exit on esc pressed
               case ALLEGRO_KEY_ESCAPE:
                  return;
                  break;
            }
            break;
         // Window close signal
         case ALLEGRO_EVENT_DISPLAY_CLOSE:
            return;
            break;
         // Case unknown event, just ignore
         default:
            break;
      }

      // Drawing to the screen
      if (redraw && al_is_event_queue_empty(queue)) {
         al_clear_to_color(al_map_rgb(0, 0, 0));

         // Draw the walls
         //    Horizontally
         for (int i = 0; i <= WIN_WIDTH - scaleFactorX; i += scaleFactorX) {
            // Top row
            al_draw_filled_rectangle(i, 0, i + scaleFactorX, scaleFactorY, al_map_rgb(255, 255, 255));
            al_draw_rectangle(i, 0, i + scaleFactorX, scaleFactorY, al_map_rgb(0, 0, 0), 1);

            // Bottom row
            al_draw_filled_rectangle(i, WIN_HEIGHT - scaleFactorY, i + scaleFactorX, WIN_HEIGHT, al_map_rgb(255, 255, 255));
            al_draw_rectangle(i, WIN_HEIGHT - scaleFactorY, i + scaleFactorX, WIN_HEIGHT, al_map_rgb(0, 0, 0), 1);
         }
         //    Vertically
         for (int i = scaleFactorY; i <= WIN_HEIGHT - scaleFactorY * 2; i += scaleFactorY) {
            // Left row
            al_draw_filled_rectangle(0, i, scaleFactorX, i + scaleFactorY, al_map_rgb(255, 255, 255));
            al_draw_rectangle(0, i, scaleFactorX, i + scaleFactorY, al_map_rgb(0, 0, 0), 1);

            // Right row
            al_draw_filled_rectangle(WIN_WIDTH - scaleFactorX, i, WIN_WIDTH, i + scaleFactorY, al_map_rgb(255, 255, 255));
            al_draw_rectangle(WIN_WIDTH - scaleFactorX, i, WIN_WIDTH, i + scaleFactorY, al_map_rgb(0, 0, 0), 1);
         }

         // Draw the apple
         al_draw_filled_rectangle(applePos.x, applePos.y, applePos.x + scaleFactorX, applePos.y + scaleFactorY, al_map_rgb(255, 0, 0));
         al_draw_rectangle(applePos.x, applePos.y, applePos.x + scaleFactorX, applePos.y + scaleFactorY, al_map_rgb(0, 0, 0), 1);

         // Draw the snake
         forEach(head, *drawNode);

         al_flip_display();
         redraw = false;
      }
   }
}

void drawNode(node_t * node) {
   al_draw_filled_rectangle(node->p.x, node->p.y, node->p.x + scaleFactorX, node->p.y + scaleFactorY, al_map_rgb(0, 255, 0));
   al_draw_rectangle(node->p.x, node->p.y, node->p.x + scaleFactorX, node->p.y + scaleFactorY, al_map_rgb(0, 0, 0), 1);
}

node_t * reset(node_t * head) {
   deleteSnake(head);

   node_t * newhead = NULL;
   headPos.x = ((WIN_WIDTH / scaleFactorX) / 4) * scaleFactorX;
   headPos.y = ((WIN_HEIGHT / scaleFactorY) / 2) * scaleFactorY;

   for (int i = 0; i < 3; i++) {
      newhead = createHead(newhead, headPos);
      headPos.x = headPos.x + scaleFactorX;
   }

   return newhead;
}

int genRandom(int max) {
   int divisor = RAND_MAX/(max+1);
   int val;

   do {
      val = rand() / divisor;
   } while (val > max);

   return val;
}

void spawnApple(struct point * appleLocation) {
   int maxX = WIN_WIDTH / scaleFactorX - 2;
   int maxY = WIN_HEIGHT / scaleFactorY - 2;

   appleLocation->x = genRandom(maxX) * scaleFactorX;
   appleLocation->y = genRandom(maxY) * scaleFactorY;

   if (appleLocation->x < scaleFactorX) {
      appleLocation->x = scaleFactorX;
   }
   if (appleLocation->y < scaleFactorY) {
      appleLocation->y = scaleFactorY;
   }
}
