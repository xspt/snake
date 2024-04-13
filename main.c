#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <raylib.h>

#include "snake.h"

// Window size
static const int winWidth = 480;
static const int winHeight = 480;

// Grid size
static const int scaledWidth = 32;
static const int scaledHeight = 32;

static Vector2 pixelSize;

static Vector2 headPos;
static node_t * head;
static enum { up, right, down, left } direction = right;
static bool canPress = true;
static int tickCount = 0;

static Vector2 applePos;

static bool pause = false;
static enum { menu, game, gameover } screen = menu;

static void InitGame(void);
static void UpdateGame(void);
static void DrawGame(void);
static void DrawNode(node_t * head);
static void SpawnApple(void);

int main()
{
   // Create game window
   InitWindow(winWidth, winHeight, "Snake");

   pixelSize = (Vector2) { winWidth / scaledWidth, winHeight / scaledHeight };

   // Initialize snake position and spawn first apple
   InitGame();

   SetTargetFPS(60);

   while (!WindowShouldClose())
   {
      UpdateGame();
      DrawGame();
   }

   deleteSnake(head);

   return 0;
}

void InitGame()
{
   screen = game;
   direction = right;
   // Set the snake head position to be on the middle of the grid
   headPos.x = ((int) (winWidth / pixelSize.x) / 2) * pixelSize.x;
   headPos.y = ((int) (winHeight / pixelSize.y) / 2) * pixelSize.y;

   head = NULL;

   // Create a snake with 3 segments to start the game
   for (int i = 0; i < 3; i++) {
      head = createHead(head, headPos);
      headPos.x = headPos.x + pixelSize.x;
   }

   SpawnApple();
}

void UpdateGame()
{
   switch (screen)
   {
      case menu:
         break;

      case game:
         if (IsKeyPressed(KEY_P))
         {
            pause = !pause;
         }

         if (!pause)
         {
            tickCount++;

            // Input handling
            if (IsKeyPressed(KEY_RIGHT) && canPress && direction != left)
            {
               direction = right;
               canPress = false;
            }
            if (IsKeyPressed(KEY_LEFT) && canPress && direction != right)
            {
               direction = left;
               canPress = false;
            }
            if (IsKeyPressed(KEY_UP) && canPress && direction != down)
            {
               direction = up;
               canPress = false;
            }
            if (IsKeyPressed(KEY_DOWN) && canPress && direction != up)
            {
               direction = down;
               canPress = false;
            }

            // Update snake position
            if (tickCount >= 5)
            {
               tickCount = 0;

               switch (direction)
               {
                  case up:
                     headPos.y -= pixelSize.y;
                     break;
                  case down:
                     headPos.y += pixelSize.y;
                     break;
                  case left:
                     headPos.x -= pixelSize.x;
                     break;
                  case right:
                     headPos.x += pixelSize.x;
                     break;
               }

               // Check if snake has eaten apple
               if (head->position.x == applePos.x && head->position.y == applePos.y)
               {
                  do
                  {
                     SpawnApple();
                  }
                  while (isColliding(head, applePos));
               }
               // If the snake has not eaten a apple, delete the tail
               else
               {
                  deleteTail(head);
               }

               head = createHead(head, headPos);

               canPress = true;
            }

            // Collision check
            if (isColliding(head->next, head->position)
                  ||headPos.x < pixelSize.x
                  || headPos.y < pixelSize.y
                  || headPos.x >= winWidth - pixelSize.x
                  || headPos.y >= winHeight - pixelSize.y)
            {
               deleteSnake(head);
               InitGame();
            }
         }
         break;

      case gameover:
         break;
   }

}

void DrawGame()
{
   BeginDrawing();

      switch (screen)
      {
         case menu:
            break;

         case game:
            ClearBackground(BLACK);

            // Drawl walls
            for (int i = 0; i <= winWidth - pixelSize.x; i += pixelSize.x)
            {
               // Top row
               DrawRectangle(i, 0, pixelSize.x, pixelSize.y, RAYWHITE);
               DrawRectangleLines(i, 0, pixelSize.x, pixelSize.y, BLACK);

               // Bottom row
               DrawRectangle(i, winHeight - pixelSize.y, pixelSize.x, pixelSize.y, RAYWHITE);
               DrawRectangleLines(i, winHeight - pixelSize.y, pixelSize.x, pixelSize.y, BLACK);
            }
            for (int i = pixelSize.y; i <= winHeight - pixelSize.y * 2; i += pixelSize.y)
            {
               // Left row
               DrawRectangle(0, i, pixelSize.x, pixelSize.y, RAYWHITE);
               DrawRectangleLines(0, i, pixelSize.x, pixelSize.y, BLACK);

               // Right row
               DrawRectangle(winWidth - pixelSize.x, i, pixelSize.x, pixelSize.y, WHITE);
               DrawRectangleLines(winWidth - pixelSize.x, i, pixelSize.x, pixelSize.y, BLACK);
            }

            // Draw the apple
            DrawRectangleV(applePos, pixelSize, RED);
            DrawRectangleLines(applePos.x, applePos.y, pixelSize.x, pixelSize.y, BLACK);

            // Draw the snake
            forEach(head, &DrawNode);

            if (pause)
            {
               DrawText("PAUSED", winWidth / 2 - MeasureText("PAUSED", 30) / 2, winHeight / 2 - 30, 30, WHITE);
            }
            break;

         case gameover:
            break;
      }


   EndDrawing();
}

static void DrawNode(node_t * node)
{
   DrawRectangleV(node->position, pixelSize, GREEN);
   DrawRectangleLines(node->position.x, node->position.y, pixelSize.x, pixelSize.y, BLACK);
}

void SpawnApple()
{
   // Prevents apple from spawning outside screen or inside the bottom and right walls
   int maxX = winWidth / pixelSize.x - 2;
   int maxY = winHeight / pixelSize.y - 2;

   applePos.x = GetRandomValue(1, maxX) * pixelSize.x;
   applePos.y = GetRandomValue(1, maxY) * pixelSize.y;
}
