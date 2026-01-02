#include "screen_login.hpp"
#include <raylib.h>
#include <iostream> // For std::cerr
#include <json.hpp> // For JSON parsing

namespace ac {
namespace client {

screen_login::screen_login() : active_input_field_(0), login_button_pressed_(false), register_button_pressed_(false) {
    username_input_.reserve(32); // Pre-allocate some memory
    password_input_.reserve(32); // Pre-allocate some memory
    // input_ is default constructed
}

void screen_login::update(network_client& net) {
    input_.update_mouse(); // Update mouse state

    // Handle keyboard input for username and password fields
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125)) { // Only allow printable ASCII characters
            if (active_input_field_ == 0 && username_input_.length() < 31) { // Max 31 chars for username
                username_input_ += (char)key;
            } else if (active_input_field_ == 1 && password_input_.length() < 31) { // Max 31 chars for password
                password_input_ += (char)key;
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (active_input_field_ == 0 && !username_input_.empty()) {
            username_input_.pop_back();
        } else if (active_input_field_ == 1 && !password_input_.empty()) {
            password_input_.pop_back();
        }
    }

    // Handle input field selection (using virtual mouse position)
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse_v_pos = input_.get_v_m();
        if (CheckCollisionPointRec(mouse_v_pos, (Rectangle){100, 180, 300, 40})) {
            active_input_field_ = 0;
        } else if (CheckCollisionPointRec(mouse_v_pos, (Rectangle){100, 280, 300, 40})) {
            active_input_field_ = 1;
        } else {
            active_input_field_ = -1; // No input field active
        }

        // Handle button clicks
        Rectangle login_btn_rect = {100, 350, 140, 50};
        Rectangle register_btn_rect = {260, 350, 140, 50};

        if (CheckCollisionPointRec(mouse_v_pos, login_btn_rect)) {
            login_button_pressed_ = true;
            register_button_pressed_ = false; // Ensure only one button is pressed
        } else if (CheckCollisionPointRec(mouse_v_pos, register_btn_rect)) {
            register_button_pressed_ = true;
            login_button_pressed_ = false; // Ensure only one button is pressed
        }
    }

    // Process login/register requests
    if (login_button_pressed_ && !username_input_.empty() && !password_input_.empty()) {
        std::string request_json = "{\"action\":\"login\",\"username\":\"" + username_input_ + "\",\"password\":\"" + password_input_ + "\"}";
        status_message_ = "Logging in...";
        std::string response = net.send_request(request_json);
        
        try {
            nlohmann::json json_response = nlohmann::json::parse(response);
            if (json_response["status"] == "success") {
                status_message_ = "Login successful! Token: " + json_response["token"].get<std::string>().substr(0, 8) + "...";
                // Store token in net or client state if needed for future requests
                // net.set_session_token(json_response["token"].get<std::string>());
                next_state_ = STATE_MAIN_MENU; // Transition to main menu
            } else {
                status_message_ = "Login failed: " + json_response["message"].get<std::string>();
            }
        } catch (const nlohmann::json::parse_error& e) {
            status_message_ = "Server response parse error: " + std::string(e.what());
            std::cerr << "JSON parse error: " << e.what() << " Response: " << response << std::endl;
        } catch (const nlohmann::json::exception& e) {
            status_message_ = "Server response content error: " + std::string(e.what());
            std::cerr << "JSON content error: " << e.what() << " Response: " << response << std::endl;
        }
        login_button_pressed_ = false; // Reset button state
    } else if (register_button_pressed_ && !username_input_.empty() && !password_input_.empty()) {
        std::string request_json = "{\"action\":\"register\",\"username\":\"" + username_input_ + "\",\"password\":\"" + password_input_ + "\"}";
        status_message_ = "Registering...";
        std::string response = net.send_request(request_json);

        try {
            nlohmann::json json_response = nlohmann::json::parse(response);
            if (json_response["status"] == "success") {
                status_message_ = "Registration successful: " + json_response["message"].get<std::string>();
            } else {
                status_message_ = "Registration failed: " + json_response["message"].get<std::string>();
            }
        } catch (const nlohmann::json::parse_error& e) {
            status_message_ = "Server response parse error: " + std::string(e.what());
            std::cerr << "JSON parse error: " << e.what() << " Response: " << response << std::endl;
        } catch (const nlohmann::json::exception& e) {
            status_message_ = "Server response content error: " + std::string(e.what());
            std::cerr << "JSON content error: " << e.what() << " Response: " << response << std::endl;
        }
        register_button_pressed_ = false; // Reset button state
    } else if ((login_button_pressed_ || register_button_pressed_) && (username_input_.empty() || password_input_.empty())) {
        status_message_ = "Username and password cannot be empty.";
        login_button_pressed_ = false;
        register_button_pressed_ = false;
    }

    // Process server connection status
    if (!net.is_connected() && status_message_ != "Connecting to server...") {
        status_message_ = "Connecting to server...";
        // Attempt to reconnect if not connected and not already trying
        net.connect_to_server("127.0.0.1", 8080);
    } else if (net.is_connected() && status_message_ == "Connecting to server...") {
        status_message_ = "Connected to server!";
    }
}

void screen_login::draw() {
    ClearBackground(RAYWHITE);
    
    // Title
    int title_width = MeasureText("ALACRITY CHESS - LOGIN", 40);
    DrawText("ALACRITY CHESS - LOGIN", GetScreenWidth() / 2 - title_width / 2, 50, 40, DARKBLUE);
    
    // Username input
    DrawText("Username:", 100, 150, 20, BLACK);
    Rectangle username_rect = {100, 180, 300, 40};
    DrawRectangleRec(username_rect, (active_input_field_ == 0) ? LIGHTGRAY : Fade(LIGHTGRAY, 0.5f));
    DrawText(username_input_.c_str(), 110, 190, 20, BLACK);
    
    // Password input
    DrawText("Password:", 100, 250, 20, BLACK);
    Rectangle password_rect = {100, 280, 300, 40};
    DrawRectangleRec(password_rect, (active_input_field_ == 1) ? LIGHTGRAY : Fade(LIGHTGRAY, 0.5f));
    std::string password_display(password_input_.length(), '*');
    DrawText(password_display.c_str(), 110, 290, 20, BLACK);
    
    // Login Button
    Rectangle login_btn_rect = {100, 350, 140, 50};
    Color login_btn_color = (CheckCollisionPointRec(input_.get_v_m(), login_btn_rect)) ? LIME : DARKGREEN;
    DrawRectangleRec(login_btn_rect, login_btn_color);
    int login_text_width = MeasureText("LOGIN", 20);
    DrawText("LOGIN", login_btn_rect.x + login_btn_rect.width / 2 - login_text_width / 2, login_btn_rect.y + 15, 20, WHITE);

    // Register Button
    Rectangle register_btn_rect = {260, 350, 140, 50};
    Color register_btn_color = (CheckCollisionPointRec(input_.get_v_m(), register_btn_rect)) ? SKYBLUE : BLUE;
    DrawRectangleRec(register_btn_rect, register_btn_color);
    int register_text_width = MeasureText("REGISTER", 20);
    DrawText("REGISTER", register_btn_rect.x + register_btn_rect.width / 2 - register_text_width / 2, register_btn_rect.y + 15, 20, WHITE);
    
    // Status message
    if (!status_message_.empty()) {
        DrawText(status_message_.c_str(), 100, 450, 20, RED);
    }
}

game_state screen_login::get_next_state() {
    return next_state_;
}

}  // namespace client
}  // namespace ac
