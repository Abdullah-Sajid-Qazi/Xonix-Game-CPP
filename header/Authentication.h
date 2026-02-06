// Xonix Game
// Authentication system - handles login, registration, and scheduled matchmaking

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "System.h"
#include "MatchmakingQueue.h"
using namespace std;
using namespace sf;

extern void toggleMusic();
extern bool isMusicEnabled();
extern void setMusicEnabled(bool enabled);

class AuthenticationSystem
{
private:
    RenderWindow *window;
    Font font, sfont;

    Text usernameText, passwordText, messageText;
    Text usernameLabel, passwordLabel;
    RectangleShape usernameBox, passwordBox, loginButton, switchModeButton;
    Text loginButtonText, switchModeButtonText, screenText;

    string usernameInput;
    string passwordInput;
    bool isRegisterMode;
    bool typingUsername;

    System system;

    Texture backgroundTexture;
    Sprite backgroundSprite;

    RectangleShape scheduledButton;
    Text scheduledButtonText;

    void setupUI();
    void handleTextInput(Event &event);
    void handleMouseClick(Vector2i mousePos);
    void attemptLogin();
    void attemptRegister();
    bool isValidUsername(string &u, string &msg);
    bool isStrongPassword(string &p, string &msg);
    void render();
    string getCurrentTimeString();
    void openGameRoom();
    void playScheduledMatches(GameRoom &gameRoom);

public:
    AuthenticationSystem(RenderWindow *win);

    void authenticationLoop();
};
