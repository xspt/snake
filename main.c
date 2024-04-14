#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <raylib.h>

#include "snake.h"

// Window size
static int winWidth = 640;
static int winHeight = 480;
static int resolutions[5][2] = { { 640, 480 }, { 800, 600 }, { 1024, 768 }, { 1280, 720 }, { 1366, 768 } };
static int selectedResolution = 0;

// Grid size
static const int gridWidth = 22;
static const int gridHeight = 22;

static Vector2 pixelSize;
static Vector2 offset;

static Vector2 headPos;
static node_t * head;
static enum { up, right, down, left } direction = right;
static bool canPress = true;
static int tickCount = 0;

static Vector2 applePos;

static bool shouldQuit = false;
static bool pause = false;
static enum { menu, game, gameover } screen = menu;
static const char * options[] = { "start", "resolution", "quit" };
static const char * gameoverOptions[] = { "restart", "main menu" };
static int menuSelected = 0;
static int gameoverSelected = 0;

static void updateResolution(void);
static void InitGame(void);
static void UpdateGame(void);
static void DrawGame(void);
static void DrawNode(node_t * head);
static void SpawnApple(void);

int main()
{
   // Create game window
   InitWindow(winWidth, winHeight, "Snake");

   updateResolution();

   SetExitKey(KEY_NULL);

   SetTargetFPS(60);

   while (!shouldQuit)
   {
      UpdateGame();
      DrawGame();
   }

   CloseWindow();

   return 0;
}


static void updateResolution(void)
{
   if (winHeight < winWidth)
   {
      offset = (Vector2) { (winWidth - (winHeight - winHeight % gridHeight)) / 2.0, (winHeight % gridHeight) / 2.0};
      pixelSize = (Vector2) { (winHeight - offset.y * 2) / gridHeight, (winHeight - offset.y * 2) / gridHeight };
   }
   else
   {
      offset = (Vector2) { (winWidth % gridWidth) / 2.0, (winHeight - (winWidth - winWidth % gridWidth)) / 2.0 };
      pixelSize = (Vector2) { (winWidth - offset.x * 2) / gridWidth, (winWidth - offset.x * 2) / gridWidth };
   }
}

void InitGame()
{
   direction = right;
   // Set the snake head position to be on the middle of the grid
   headPos.x = gridWidth / 2.0 * pixelSize.x + offset.x;
   headPos.y = gridHeight / 2.0 * pixelSize.y + offset.y;

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
   shouldQuit = WindowShouldClose();
   switch (screen)
   {
      case menu:
         if (IsKeyPressed(KEY_DOWN) && menuSelected < 2)
         {
            menuSelected++;
         }
         if (IsKeyPressed(KEY_UP) && menuSelected > 0)
         {
            menuSelected--;
         }
         if (IsKeyPressed(KEY_ENTER))
         {
            switch (menuSelected)
            {
               case 0:
                  screen = game;
                  InitGame();
                  break;
               case 1:
                  selectedResolution = selectedResolution < 4 ? selectedResolution + 1 : 0;
                  winWidth = resolutions[selectedResolution][0];
                  winHeight = resolutions[selectedResolution][1];
                  SetWindowSize(winWidth, winHeight);
                  updateResolution();
                  break;
               case 2:
                  shouldQuit = true;
                  break;
            }
         }
         break;

      case game:
         if (IsKeyPressed(KEY_ESCAPE))
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
                  || headPos.x < pixelSize.x + offset.x
                  || headPos.y < pixelSize.y + offset.y
                  || headPos.x >= winWidth - (pixelSize.x + offset.x)
                  || headPos.y >= winHeight - (pixelSize.y + offset.y))
            {
               deleteSnake(head);
               screen = gameover;
               gameoverSelected = 0;
            }
         }
         break;

      case gameover:
         if (IsKeyPressed(KEY_DOWN) && gameoverSelected < 1)
         {
            gameoverSelected++;
         }
         if (IsKeyPressed(KEY_UP) && gameoverSelected > 0)
         {
            gameoverSelected--;
         }
         if (IsKeyPressed(KEY_ENTER))
         {
            switch (gameoverSelected)
            {
               case 0:
                  screen = game;
                  InitGame();
                  break;

               case 1:
                  screen = menu;
                  menuSelected = 0;
                  break;
            }
         }
         break;
   }

}

void DrawGame()
{
   BeginDrawing();

      switch (screen)
      {
         case menu:
            ClearBackground(BLACK);

            Color textColor;
            int posY;

            for (int i = 0; i < 3; i++)
            {
               textColor = menuSelected == i ? WHITE : GRAY;
               switch (i)
               {
                  case 0:
                     posY = GetScreenHeight() / 2 - 80;
                     break;

                  case 1:
                     posY = GetScreenHeight() / 2 - 40;
                     break;
                  case 2:
                     posY = GetScreenHeight() / 2;
                     break;
               }
               DrawText(options[i], winWidth / 2 - MeasureText(options[i], 40) / 2, posY, 40, textColor);
            }
            break;

         case game:
            ClearBackground(BLACK);

            // Drawl walls
            for (int i = offset.x; i <= winWidth - (pixelSize.x + offset.x); i += pixelSize.x)
            {
               // Top row
               DrawRectangle(i, offset.y, pixelSize.x, pixelSize.y, RAYWHITE);
               DrawRectangleLines(i, offset.y, pixelSize.x, pixelSize.y, BLACK);

               // Bottom row
               DrawRectangle(i, winHeight - (pixelSize.y + offset.y), pixelSize.x, pixelSize.y, RAYWHITE);
               DrawRectangleLines(i, winHeight - (pixelSize.y + offset.y), pixelSize.x, pixelSize.y, BLACK);
            }

            for (int i = pixelSize.y + offset.y; i <= winHeight - (pixelSize.y * 2 + offset.y); i += pixelSize.y)
            {
               // Left row
               DrawRectangle(offset.x, i, pixelSize.x, pixelSize.y, RAYWHITE);
               DrawRectangleLines(offset.x, i, pixelSize.x, pixelSize.y, BLACK);

               // Right row
               DrawRectangle(winWidth - (pixelSize.x + offset.x), i, pixelSize.x, pixelSize.y, WHITE);
               DrawRectangleLines(winWidth - (pixelSize.x + offset.x), i, pixelSize.x, pixelSize.y, BLACK);
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
            ClearBackground(BLACK);

            DrawText("GAME OVER", GetScreenWidth() / 2 - MeasureText("GAME OVER", 30) / 2, GetScreenHeight() / 2 - 30, 30, WHITE);

            textColor = GRAY;
            for (int i = 0; i < 2; i++)
            {
               textColor = gameoverSelected == i ? WHITE : GRAY;
               DrawText(gameoverOptions[i], GetScreenWidth() / 2 - MeasureText(gameoverOptions[i], 30) / 2, GetScreenHeight() / 2 + i * 30, 30, textColor);
            }
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
   int maxX = gridWidth - 2;
   int maxY = gridHeight - 2;

   applePos.x = GetRandomValue(1, maxX) * pixelSize.x + offset.x;
   applePos.y = GetRandomValue(1, maxY) * pixelSize.y + offset.y;
}
