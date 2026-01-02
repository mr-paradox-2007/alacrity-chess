#include "client_main.hpp"
#include <string>

// Constants
const int INTERNAL_WIDTH = 1920;
const int INTERNAL_HEIGHT = 1080;

// Button class for UI elements
class Button
{
private:
    Rectangle rect;

public:
    Button(Rectangle _rect) : rect(_rect) {}
    
    void Draw(Color color)
    {
        DrawRectangleRec(rect, color);
    }
    
    bool IsHovered(Vector2 mousePos)
    {
        return CheckCollisionPointRec(mousePos, rect);
    }
};

// Game state enum
enum GameState
{
    STATE_MAIN_MENU,
    STATE_SETTINGS,
    STATE_PLAYING,
    STATE_EXIT
};

// Global variables
GameState currentState = STATE_MAIN_MENU;
RenderTexture2D displayScreen;
Vector2 virtualMouse = {0, 0};
Vector2 mouse = {0, 0};
float scale = 1.0f;
bool musicOn = true;
bool gameLoaded = false;
Music backgroundMusic;

// Function prototypes
void UpdateMouse();
void HandleButtonHover(Button& btn, Vector2 mousePos, Color hoverColor = {0, 0, 0, 40});
void DrawMainMenu();
void DrawSettings();
void DrawGame();

// Update mouse position with scaling
void UpdateMouse()
{
    scale = fminf((float)GetScreenWidth() / INTERNAL_WIDTH, 
                  (float)GetScreenHeight() / INTERNAL_HEIGHT);
    
    mouse = GetMousePosition();
    virtualMouse = {
        (mouse.x - (GetScreenWidth() - INTERNAL_WIDTH * scale) * 0.5f) / scale,
        (mouse.y - (GetScreenHeight() - INTERNAL_HEIGHT * scale) * 0.5f) / scale
    };
    
    if (virtualMouse.x < 0) virtualMouse.x = 0;
    if (virtualMouse.x > INTERNAL_WIDTH) virtualMouse.x = INTERNAL_WIDTH;
    if (virtualMouse.y < 0) virtualMouse.y = 0;
    if (virtualMouse.y > INTERNAL_HEIGHT) virtualMouse.y = INTERNAL_HEIGHT;
}

// Handle button hover effect
void HandleButtonHover(Button& btn, Vector2 mousePos, Color hoverColor)
{
    if (btn.IsHovered(mousePos))
        btn.Draw(hoverColor);
}

// Draw main menu
void DrawMainMenu()
{
    static Texture2D menuBackground = {0};
    static Font menuFont = {0};
    static bool assetsLoaded = false;
    
    if (!assetsLoaded)
    {
        if (FileExists("assets/main_menu_bg.png"))
            menuBackground = LoadTexture("assets/main_menu_bg.png");
        if (FileExists("assets/Montserrat-ExtraBold.ttf"))
            menuFont = LoadFont("assets/Montserrat-ExtraBold.ttf");
        assetsLoaded = true;
    }
    
    static int fontSize = 50;
    float leftX = 20;
    float rightX = 1550;
    float topY = INTERNAL_HEIGHT / 3.0f;
    float gap = 160;
    
    static Button btnStart({leftX, topY, 350, 80});
    static Button btnNewGame({rightX, topY, 350, 80});
    static Button btnLoadGame({leftX, topY + gap, 350, 80});
    static Button btnSettings({rightX, topY + gap, 350, 80});
    static Button btnExit({INTERNAL_WIDTH / 2.0f - 150, topY + 2 * gap + 235, 300, 60});
    
    ClearBackground(BLACK);
    
    // Draw background if loaded
    if (menuBackground.id > 0)
    {
        DrawTexturePro(menuBackground,
            {0, 0, (float)menuBackground.width, (float)menuBackground.height},
            {0, 0, (float)INTERNAL_WIDTH, (float)INTERNAL_HEIGHT},
            {0, 0}, 0, WHITE);
    }
    
    // Draw button hover effects
    HandleButtonHover(btnStart, virtualMouse);
    HandleButtonHover(btnNewGame, virtualMouse);
    HandleButtonHover(btnLoadGame, virtualMouse);
    HandleButtonHover(btnSettings, virtualMouse);
    HandleButtonHover(btnExit, virtualMouse);
    
    // Draw button text
    if (menuFont.baseSize > 0)
    {
        DrawTextEx(menuFont, "Start", {leftX, topY}, fontSize, 2, WHITE);
        DrawTextEx(menuFont, "New Game", {rightX, topY}, fontSize, 2, WHITE);
        DrawTextEx(menuFont, "Load Game", {leftX, topY + gap}, fontSize, 2, WHITE);
        DrawTextEx(menuFont, "Settings", {rightX, topY + gap}, fontSize, 2, WHITE);
        DrawTextEx(menuFont, "Exit", 
                  {INTERNAL_WIDTH / 2.0f - MeasureTextEx(menuFont, "Exit", fontSize, 2).x / 2, 
                   topY + 2 * gap + 240}, fontSize, 2, WHITE);
    }
    else
    {
        DrawText("Start", leftX, topY, 40, WHITE);
        DrawText("New Game", rightX, topY, 40, WHITE);
        DrawText("Load Game", leftX, topY + gap, 40, WHITE);
        DrawText("Settings", rightX, topY + gap, 40, WHITE);
        DrawText("Exit", INTERNAL_WIDTH / 2 - 50, topY + 2 * gap + 240, 40, WHITE);
    }
    
    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (btnExit.IsHovered(virtualMouse))
        {
            currentState = STATE_EXIT;
        }
        else if (btnSettings.IsHovered(virtualMouse))
        {
            currentState = STATE_SETTINGS;
        }
        else if (btnStart.IsHovered(virtualMouse))
        {
            if (gameLoaded)
            {
                currentState = STATE_PLAYING;
            }
        }
        else if (btnNewGame.IsHovered(virtualMouse))
        {
            gameLoaded = true;
            currentState = STATE_PLAYING;
        }
        else if (btnLoadGame.IsHovered(virtualMouse))
        {
            gameLoaded = true;
        }
    }
}

// Draw settings menu
void DrawSettings()
{
    static Texture2D settingsBackground = {0};
    static Font settingsFont = {0};
    static bool assetsLoaded = false;
    
    if (!assetsLoaded)
    {
        if (FileExists("assets/settings_bg.png"))
            settingsBackground = LoadTexture("assets/settings_bg.png");
        if (FileExists("assets/Montserrat-ExtraBold.ttf"))
            settingsFont = LoadFont("assets/Montserrat-ExtraBold.ttf");
        assetsLoaded = true;
    }
    
    static int fontSize = 50;
    float buttonWidth = 450;
    float buttonHeight = 80;
    float centerX = INTERNAL_WIDTH / 2.0f - buttonWidth / 2;
    float startY = INTERNAL_HEIGHT / 3.0f;
    float gap = 110;
    
    static Button btnMusic({centerX, startY + 0 * gap, buttonWidth, buttonHeight});
    static Button btnSaveData({centerX, startY + 1 * gap, buttonWidth, buttonHeight});
    static Button btnEraseData({centerX, startY + 2 * gap, buttonWidth, buttonHeight});
    static Button btnBack({centerX, startY + 3 * gap, buttonWidth, buttonHeight});
    
    ClearBackground(BLACK);
    
    // Draw background if loaded
    if (settingsBackground.id > 0)
    {
        DrawTexturePro(settingsBackground,
            {0, 0, (float)settingsBackground.width, (float)settingsBackground.height},
            {0, 0, (float)INTERNAL_WIDTH, (float)INTERNAL_HEIGHT},
            {0, 0}, 0, WHITE);
    }
    
    // Draw button hover effects
    HandleButtonHover(btnMusic, virtualMouse, {255, 255, 255, 40});
    HandleButtonHover(btnSaveData, virtualMouse, {255, 255, 255, 40});
    HandleButtonHover(btnEraseData, virtualMouse, {255, 255, 255, 40});
    HandleButtonHover(btnBack, virtualMouse, {255, 255, 255, 40});
    
    // Draw button text
    const char* musicText = musicOn ? "Music: ON" : "Music: OFF";
    
    if (settingsFont.baseSize > 0)
    {
        DrawTextEx(settingsFont, musicText,
            {centerX + buttonWidth / 2 - MeasureTextEx(settingsFont, musicText, fontSize, 2).x / 2,
             startY + 0 * gap + buttonHeight / 2 - fontSize / 2},
            fontSize, 2, WHITE);
        
        DrawTextEx(settingsFont, "Save Data",
            {centerX + buttonWidth / 2 - MeasureTextEx(settingsFont, "Save Data", fontSize, 2).x / 2,
             startY + 1 * gap + buttonHeight / 2 - fontSize / 2},
            fontSize, 2, WHITE);
        
        DrawTextEx(settingsFont, "Erase Data",
            {centerX + buttonWidth / 2 - MeasureTextEx(settingsFont, "Erase Data", fontSize, 2).x / 2,
             startY + 2 * gap + buttonHeight / 2 - fontSize / 2},
            fontSize, 2, WHITE);
        
        DrawTextEx(settingsFont, "Back",
            {centerX + buttonWidth / 2 - MeasureTextEx(settingsFont, "Back", fontSize, 2).x / 2,
             startY + 3 * gap + buttonHeight / 2 - fontSize / 2},
            fontSize, 2, WHITE);
    }
    else
    {
        DrawText(musicText, centerX + 100, startY + 20, 40, WHITE);
        DrawText("Save Data", centerX + 100, startY + gap + 20, 40, WHITE);
        DrawText("Erase Data", centerX + 100, startY + 2 * gap + 20, 40, WHITE);
        DrawText("Back", centerX + 150, startY + 3 * gap + 20, 40, WHITE);
    }
    
    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (btnMusic.IsHovered(virtualMouse))
        {
            musicOn = !musicOn;
            if (musicOn && IsMusicStreamPlaying(backgroundMusic))
                ResumeMusicStream(backgroundMusic);
            else if (!musicOn && IsMusicStreamPlaying(backgroundMusic))
                PauseMusicStream(backgroundMusic);
        }
        else if (btnSaveData.IsHovered(virtualMouse))
        {
            // Save functionality placeholder
        }
        else if (btnEraseData.IsHovered(virtualMouse))
        {
            // Erase functionality placeholder
        }
        else if (btnBack.IsHovered(virtualMouse))
        {
            currentState = STATE_MAIN_MENU;
        }
    }
    
    // ESC key to go back
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE))
    {
        currentState = STATE_MAIN_MENU;
    }
}

// Draw game screen
void DrawGame()
{
    ClearBackground(DARKGRAY);
    
    DrawText("GAME SCREEN", INTERNAL_WIDTH / 2 - 200, INTERNAL_HEIGHT / 2 - 50, 60, WHITE);
    DrawText("Press ESC to return to menu", INTERNAL_WIDTH / 2 - 250, INTERNAL_HEIGHT / 2 + 50, 30, LIGHTGRAY);
    
    // Return to menu on ESC
    if (IsKeyPressed(KEY_ESCAPE))
    {
        currentState = STATE_MAIN_MENU;
    }
}

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_FULLSCREEN_MODE | FLAG_VSYNC_HINT);
    InitWindow(INTERNAL_WIDTH, INTERNAL_HEIGHT, "Chess Client");
    
    SetWindowSize(INTERNAL_WIDTH, INTERNAL_HEIGHT);
    displayScreen = LoadRenderTexture(INTERNAL_WIDTH, INTERNAL_HEIGHT);
    SetTextureFilter(displayScreen.texture, TEXTURE_FILTER_BILINEAR);
    SetTargetFPS(60);
    
    // Initialize audio
    InitAudioDevice();
    if (FileExists("assets/background_music.mp3"))
    {
        backgroundMusic = LoadMusicStream("assets/background_music.mp3");
        SetMusicVolume(backgroundMusic, 0.5f);
        PlayMusicStream(backgroundMusic);
    }
    
    // Main game loop
    while (!WindowShouldClose() && currentState != STATE_EXIT)
    {
        // Update
        UpdateMouse();
        
        if (IsMusicStreamPlaying(backgroundMusic))
            UpdateMusicStream(backgroundMusic);
        
        // Handle fullscreen toggle
        if (IsKeyPressed(KEY_F11))
            ToggleFullscreen();
        
        // Render to texture
        BeginTextureMode(displayScreen);
        
        switch (currentState)
        {
            case STATE_MAIN_MENU:
                DrawMainMenu();
                break;
                
            case STATE_SETTINGS:
                DrawSettings();
                break;
                
            case STATE_PLAYING:
                DrawGame();
                break;
                
            default:
                break;
        }
        
        EndTextureMode();
        
        // Render to screen with scaling
        BeginDrawing();
        ClearBackground(BLACK);
        
        DrawTexturePro(
            displayScreen.texture,
            {0, 0, (float)displayScreen.texture.width, (float)-displayScreen.texture.height},
            {(GetScreenWidth() - INTERNAL_WIDTH * scale) * 0.5f,
             (GetScreenHeight() - INTERNAL_HEIGHT * scale) * 0.5f,
             INTERNAL_WIDTH * scale,
             INTERNAL_HEIGHT * scale},
            {0, 0}, 0.0f, WHITE
        );
        
        EndDrawing();
    }
    
    // Cleanup
    if (IsMusicStreamPlaying(backgroundMusic))
        UnloadMusicStream(backgroundMusic);
    CloseAudioDevice();
    UnloadRenderTexture(displayScreen);
    CloseWindow();
    
    return 0;
}