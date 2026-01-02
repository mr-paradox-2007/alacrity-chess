#!/bin/bash

# Ensure bin directory exists
mkdir -p alacrity-chess/bin

# Define base project directory
PROJECT_DIR="alacrity-chess"

# Define source and include directories
CLIENT_SRC_DIR="$PROJECT_DIR/src/client"
CLIENT_NETWORK_DIR="$CLIENT_SRC_DIR/network"
CLIENT_SCREENS_DIR="$CLIENT_SRC_DIR/screens"
CLIENT_GAME_DIR="$CLIENT_SRC_DIR/game"
INCLUDES_DIR="$PROJECT_DIR/includes" # Assuming json.hpp is here

# Compiler flags
COMMON_FLAGS="-std=c++17 -pthread -Wall -Wextra"
INCLUDE_PATHS="-I$INCLUDES_DIR -I$CLIENT_GAME_DIR -I$CLIENT_SCREENS_DIR -I$CLIENT_NETWORK_DIR -I$CLIENT_SRC_DIR -I$PROJECT_DIR/raylib_native/src"

# Source files
CLIENT_NETWORK_SRCS="$CLIENT_NETWORK_DIR/network_client.cpp"
CLIENT_SCREENS_SRCS="$CLIENT_SCREENS_DIR/screen_splash.cpp $CLIENT_SCREENS_DIR/screen_login.cpp $CLIENT_SCREENS_DIR/camera_manager.cpp $CLIENT_SCREENS_DIR/input_manager.cpp $CLIENT_SCREENS_DIR/audio_manager.cpp $CLIENT_SCREENS_DIR/button.cpp $CLIENT_SCREENS_DIR/game_player.cpp"
CLIENT_GAME_SRCS="$CLIENT_GAME_DIR/utility.cpp $CLIENT_GAME_DIR/piece.cpp $CLIENT_GAME_DIR/pawn.cpp $CLIENT_GAME_DIR/rook.cpp $CLIENT_GAME_DIR/knight.cpp $CLIENT_GAME_DIR/bishop.cpp $CLIENT_GAME_DIR/queen.cpp $CLIENT_GAME_DIR/king.cpp $CLIENT_GAME_DIR/board.cpp $CLIENT_GAME_DIR/move_history.cpp"
MAIN_CLIENT_SRC="$CLIENT_SRC_DIR/main.cpp"

ALL_CLIENT_SRCS="$MAIN_CLIENT_SRC $CLIENT_NETWORK_SRCS $CLIENT_SCREENS_SRCS $CLIENT_GAME_SRCS"

# Raylib specific flags and libraries
RAYLIB_LIBS="-L$PROJECT_DIR/raylib_native/src -lraylib -lm -ldl -lrt -lX11"

# Output executable name
OUTPUT_BIN="$PROJECT_DIR/bin/alacrity_chess_client"

echo "Building Alacrity Chess Client..."

g++ $COMMON_FLAGS $INCLUDE_PATHS $ALL_CLIENT_SRCS -o $OUTPUT_BIN $RAYLIB_LIBS

if [ $? -eq 0 ]; then
    echo "Client compiled successfully: $OUTPUT_BIN"
    echo "Remember to download nlohmann/json and place json.hpp in the '$INCLUDES_DIR' folder."
    echo "Usage: ./$OUTPUT_BIN"
else
    echo "Client compilation failed!"
    exit 1
fi
