#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/color.h>
#include <allegro5/timer.h>
#include <allegro5/events.h>
#include <allegro5/allegro_primitives.h>
#include "snake.h"

#define WIN_WIDTH 480
#define WIN_HEIGHT 480
#define SCALED_WIDTH 32
#define SCALED_HEIGHT 32

float scaleFactorX;
float scaleFactorY;

void mainLoop(ALLEGRO_EVENT_QUEUE * queue, ALLEGRO_TIMER * timer);
void drawNode(node_t * node);

struct point headPos;
node_t * head = NULL;

int main() {
   // Initializing allegro and its components
   if (!al_init()) {
      printf("Failed to init allegro.\n");
      return EXIT_FAILURE;
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

   // Calculate the center of the screen
   headPos.x = ((al_get_display_width(display) / scaleFactorX) / 2) * scaleFactorX;
   headPos.y = ((al_get_display_width(display) / scaleFactorY) / 2) * scaleFactorY;

   // Create a snake with 3 segments to start the game
   for (int i = 0; i < 3; i++) {
      head = createHead(head, headPos);
      headPos.x = headPos.x + scaleFactorX;
   }

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

   enum {up, right, down, left} direction = left;

   bool redraw = false;

   al_start_timer(timer);
   while(1) {
      al_wait_for_event(queue, &event);
      switch (event.type) {
         // Game logic
         case ALLEGRO_EVENT_TIMER:
            redraw = true;

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

            head = createHead(head, headPos);
            deleteTail(head);

            break;
         // User input
         case ALLEGRO_EVENT_KEY_DOWN:
            switch (event.keyboard.keycode) {
               case ALLEGRO_KEY_UP:
                  direction = direction == down ? down : up;
                  break;
               case ALLEGRO_KEY_DOWN:
                  direction = direction == up ? up : down;
                  break;
               case ALLEGRO_KEY_LEFT:
                  direction = direction == right ? right : left;
                  break;
               case ALLEGRO_KEY_RIGHT:
                  direction = direction == left ? left : right;
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
