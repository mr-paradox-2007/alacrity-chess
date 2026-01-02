#include <raylib.h>
#include "screens/screen_splash.hpp"
#include "screens/screen_login.hpp"
#include "network/network_client.hpp"
#include <memory>
#include <iostream>

// Include raygui for GUI elements later, if desired
// #define RAYGUI_IMPLEMENTATION
// #include "raygui.h"

using namespace ac::client;

int main() {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screen_width = 1280;
    const int screen_height = 720;

    InitWindow(screen_width, screen_height, "Alacrity Chess Client");
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Game variables
    network_client network;
    network.connect_to_server("127.0.0.1", 8080); // Attempt connection on startup

    game_state current_state = STATE_SPLASH;
    std::unique_ptr<screen> current_screen;

    // Load initial screen
    current_screen = std::make_unique<screen_splash>();

    std::cout << "[CLIENT] Starting game client..." << std::endl;

    // Main game loop
    while (!WindowShouldClose() && current_state != STATE_EXIT)
    {
        // Update
        //----------------------------------------------------------------------------------
        // Check for state change and transition to new screen if needed
        if (current_screen->get_next_state() != current_state) {
            current_state = current_screen->get_next_state();
            
            if (current_state == STATE_LOGIN) {
                current_screen = std::make_unique<screen_login>();
            } else if (current_state == STATE_MAIN_MENU) {
                // current_screen = std::make_unique<screen_main_menu>();
            }
            // Add other screen transitions here
        }

        current_screen->update(network);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            current_screen->draw();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    network.disconnect_from_server();
    CloseWindow();        // Close window and unload OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
