#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/color.h>
#include <allegro5/timer.h>
#include <allegro5/events.h>

#define WIN_WIDTH 480
#define WIN_HEIGHT 480

void mainLoop(ALLEGRO_EVENT_QUEUE * queue, ALLEGRO_TIMER * timer);

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

   mainLoop(queue, timer);

   // Free resources before exiting
   al_destroy_timer(timer);
   al_destroy_display(display);
   al_destroy_event_queue(queue);

   return 0;
}

void mainLoop(ALLEGRO_EVENT_QUEUE * queue, ALLEGRO_TIMER * timer) {
   ALLEGRO_EVENT event;

   bool redraw = false;

   al_start_timer(timer);
   while(1) {
      al_wait_for_event(queue, &event);
      switch (event.type) {
         // Game logic
         case ALLEGRO_EVENT_TIMER:
            redraw = true;
            break;
         // User input
         case ALLEGRO_EVENT_KEY_DOWN:
            switch (event.keyboard.keycode) {
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

         al_flip_display();
         redraw = false;
      }
   }
}
