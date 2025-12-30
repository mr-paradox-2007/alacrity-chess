#include "./../../tools/raylib_native/src/raylib.h"

int main()
{
    InitWindow(1024,768,"Native Raylib Test App");
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground({10,10,10});
        DrawFPS(10,10);
        DrawText("Hello World From Ammar Anas!",50,50,20,RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
