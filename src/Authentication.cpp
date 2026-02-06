// Xonix Game
// Authentication system - login, registration, password validation

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>
#include "../header/Authentication.h"
#include "Menu.cpp"
#include "../header/System.h"
#include "../header/FriendManagement.h"
#include "../header/MatchmakingQueue.h"
#include "../header/Multiplayer.h"

using namespace std;
using namespace sf;

AuthenticationSystem::AuthenticationSystem(RenderWindow *win)
    : usernameInput(""), passwordInput(""), isRegisterMode(false), typingUsername(true)
{
    window = win;
    if (!font.loadFromFile("assets/fonts/COOPBL.ttf"))
        cout << "Failed to load font.\n";
    if (!sfont.loadFromFile("assets/fonts/MATURASC.ttf"))
        cout << "Failed to load font.\n";
    if (!backgroundTexture.loadFromFile("assets/images/authentication.png"))
        cout << "Failed to load image\n";
    backgroundSprite.setTexture(backgroundTexture);

    setupUI();
}

void AuthenticationSystem::setupUI()
{
    usernameBox.setSize(Vector2f(300, 40));
    usernameBox.setPosition(240, 200);
    usernameBox.setFillColor(Color::White);

    passwordBox.setSize(Vector2f(300, 40));
    passwordBox.setPosition(240, 260);
    passwordBox.setFillColor(Color::White);

    usernameLabel.setFont(font);
    usernameLabel.setCharacterSize(22);
    usernameLabel.setPosition(240, 175);
    usernameLabel.setFillColor(Color::White);
    usernameLabel.setString("Username:");

    passwordLabel.setFont(font);
    passwordLabel.setCharacterSize(22);
    passwordLabel.setPosition(240, 235);
    passwordLabel.setFillColor(Color::White);
    passwordLabel.setString("Password:");

    usernameText.setFont(font);
    usernameText.setCharacterSize(20);
    usernameText.setPosition(245, 205);
    usernameText.setFillColor(Color::Black);

    passwordText.setFont(font);
    passwordText.setCharacterSize(20);
    passwordText.setPosition(245, 265);
    passwordText.setFillColor(Color::Black);

    loginButton.setSize(Vector2f(140, 40));
    loginButton.setPosition(240, 330);
    loginButton.setFillColor(Color(100, 200, 100));

    loginButtonText.setFont(font);
    loginButtonText.setCharacterSize(27);
    loginButtonText.setPosition(255, 332);
    loginButtonText.setFillColor(Color::Black);
    loginButtonText.setString("Login");

    switchModeButton.setSize(Vector2f(140, 40));
    switchModeButton.setPosition(400, 330);
    switchModeButton.setFillColor(Color(200, 100, 100));

    switchModeButtonText.setFont(font);
    switchModeButtonText.setCharacterSize(27);
    switchModeButtonText.setPosition(410, 332);
    switchModeButtonText.setFillColor(Color::Black);
    switchModeButtonText.setString("Register");

    messageText.setFont(font);
    messageText.setCharacterSize(18);
    messageText.setFillColor(Color::Red);
    messageText.setPosition(40, 450);

    screenText.setFont(sfont);
    screenText.setCharacterSize(100);
    screenText.setPosition(55, 50);
    screenText.setFillColor(Color::Red);
    screenText.setString("Authentication!!");

    scheduledButton.setSize(Vector2f(320, 40));
    scheduledButton.setPosition(230, 390);
    scheduledButton.setFillColor(Color(100, 100, 200));

    scheduledButtonText.setFont(font);
    scheduledButtonText.setCharacterSize(24);
    scheduledButtonText.setPosition(290, 392);
    scheduledButtonText.setFillColor(Color::White);
    scheduledButtonText.setString("Game Room");
}

void AuthenticationSystem::authenticationLoop()
{
    while (window->isOpen())
    {
        Event evnt;
        while (window->pollEvent(evnt))
        {
            if (evnt.type == Event::Closed)
                window->close();
            else if (evnt.type == Event::TextEntered)
                handleTextInput(evnt);
            else if (evnt.type == Event::MouseButtonPressed)
                handleMouseClick(Mouse::getPosition(*window));
        }

        render();
    }
}

void AuthenticationSystem::handleTextInput(Event &event)
{
    if (event.text.unicode < 128 && isprint(event.text.unicode))
    {
        char c = static_cast<char>(event.text.unicode);
        if (typingUsername)
            usernameInput += c;
        else
            passwordInput += c;
    }
    else if (event.text.unicode == 8)
    {
        if (typingUsername && !usernameInput.empty())
            usernameInput.pop_back();
        else if (!typingUsername && !passwordInput.empty())
            passwordInput.pop_back();
    }
}

void AuthenticationSystem::handleMouseClick(Vector2i mousePos)
{
    if (usernameBox.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
        typingUsername = true;
    else if (passwordBox.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
        typingUsername = false;
    else if (loginButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
    {
        if (isRegisterMode)
            attemptRegister();
        else
            attemptLogin();
    }
    else if (switchModeButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
    {
        isRegisterMode = !isRegisterMode;
        switchModeButtonText.setString(isRegisterMode ? "Login" : "Register");
        loginButtonText.setString(isRegisterMode ? "Register" : "Login");
        usernameInput.clear();
        passwordInput.clear();
        messageText.setString("");
    }
    else if (scheduledButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
    {
        openGameRoom();
        cout << "Opening Game Room\n";
    }
}

void AuthenticationSystem::attemptLogin()
{
    if (usernameInput.empty() || passwordInput.empty())
    {
        messageText.setFillColor(Color::Red);
        messageText.setString("Please enter username and password.");
        return;
    }

    if (system.isValid(usernameInput, passwordInput))
    {
        messageText.setFillColor(Color::Green);
        messageText.setString("Login Successful!");
        string id = system.returnId(usernameInput);
        MenuSystem menu(window, id);
        int result = menu.run();

        if (result == 2)
            window->close();

        usernameInput.clear();
        passwordInput.clear();
        usernameText.setString("Username: ");
        passwordText.setString("Password: ");
        messageText.setString("");
    }
    else
    {
        messageText.setFillColor(Color::Red);
        messageText.setString("Invalid username or password.");
    }
}

void AuthenticationSystem::attemptRegister()
{
    string msg;
    if (usernameInput.empty() || passwordInput.empty())
    {
        messageText.setFillColor(Color::Red);
        messageText.setString("Fields cannot be empty.");
        return;
    }
    if (system.usernameExists(usernameInput))
    {
        messageText.setFillColor(Color::Red);
        messageText.setString("Username already taken.");
        return;
    }
    if (!isValidUsername(usernameInput, msg))
    {
        messageText.setFillColor(Color::Red);
        messageText.setString(msg);
        return;
    }
    if (!isStrongPassword(passwordInput, msg))
    {
        messageText.setFillColor(Color::Red);
        messageText.setString(msg);
        return;
    }
    system.addPlayer(getCurrentTimeString(), usernameInput, passwordInput);
    messageText.setFillColor(Color::Green);
    messageText.setString("Registration successful!");
}

// Username validation - no spaces allowed
bool AuthenticationSystem::isValidUsername(string &u, string &msg)
{
    Player *temp;
    while (!u.empty() && u.back() == ' ')
        u.pop_back();
    int len = static_cast<int>(u.length());

    // Check if username already exists in the system
    for (int i = 0; i < system.noOfPlayers; i++)
    {
        temp = system.returnPlayer(system.ArrayOfPlayers[i]);
        if (temp->username == u)
        {
            msg = "Username already exists.";
            return false;
        }
        delete temp;
    }

    // Check for spaces within the username
    for (int i = 0; i < len; ++i)
    {
        if (u[i] == ' ')
        {
            msg = "Invalid username! Spaces are not allowed.";
            return false;
        }
    }

    return true;
}

/*
 * Method: isStrongPassword
 * Description: Validates password strength according to security requirements:
 *              - Length must be between 8 and 15 characters
 *              - Must contain at least one digit
 *              - Must contain at least one letter
 *              - Must contain at least one special character
 *              - No spaces allowed
 * Parameters:
 *   p - Reference to password string (may be modified by trimming)
 *   msg - Reference to error/success message string
 * Returns: true if password meets all requirements, false otherwise
 */
bool AuthenticationSystem::isStrongPassword(string &p, string &msg)
{
    // Set default error message
    msg = "Password should be beteween(8-15), alphanumeric and a special character.";

    // Trim trailing spaces
    while (!p.empty() && p.back() == ' ')
        p.pop_back();
    int len = static_cast<int>(p.length());

    // Check length requirement (8-15 characters)
    if (len < 8 || len > 15)
        return false;

    if (p[len - 1] == ' ')
    {
        len--;
    }

    // Check for spaces within password (not allowed)
    for (int i = 0; i < len; ++i)
    {
        if (p[i] == ' ')
            return false;
    }

    // Check for required character types:
    // at least one digit, one letter, one special character
    bool hasDigit = false;
    bool hasLetter = false;
    bool hasSpecial = false;

    for (int i = 0; i < len; ++i)
    {
        char c = p[i];
        if (isdigit(static_cast<unsigned char>(c)))
        {
            hasDigit = true;
        }
        else if (isalpha(static_cast<unsigned char>(c)))
        {
            hasLetter = true;
        }
        else
        {
            hasSpecial = true;
        }

        // Early exit if all requirements met
        if (hasDigit && hasLetter && hasSpecial)
            break;
    }

    bool check = hasDigit && hasLetter && hasSpecial;
    if (check)
    {
        msg = ""; // Clear error message on success
    }
    return check;
}

void AuthenticationSystem::render()
{
    window->clear(sf::Color(240, 240, 240));
    window->draw(backgroundSprite);
    usernameText.setString(usernameInput);
    passwordText.setString(string(passwordInput.length(), '*'));

    window->draw(screenText);

    window->draw(usernameLabel);
    window->draw(passwordLabel);
    window->draw(usernameBox);
    window->draw(passwordBox);
    window->draw(usernameText);
    window->draw(passwordText);

    window->draw(loginButton);
    window->draw(loginButtonText);
    window->draw(switchModeButton);
    window->draw(switchModeButtonText);

    window->draw(messageText);
    window->draw(scheduledButton);
    window->draw(scheduledButtonText);
    window->display();
}

string AuthenticationSystem::getCurrentTimeString()
{
    time_t now = time(nullptr);
    tm localTime;

#ifdef _WIN32
    if (localtime_s(&localTime, &now) != 0)
        return "Error getting time";
#else
    if (localtime_r(&now, &localTime) == nullptr)
        return "Error getting time";
#endif

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", &localTime);
    return string(buffer);
}

// Game Room - matchmaking with Priority Queue for skill-based pairing
void AuthenticationSystem::openGameRoom()
{
    GameRoom gameRoom(&system);

    Font roomFont;
    if (!roomFont.loadFromFile("assets/fonts/arial.ttf"))
    {
        cerr << "Failed to load font\n";
        return;
    }

    // Fixed window size for Game Room
    const float W = 800, H = 600;
    (void)W;
    (void)H; // Suppress unused variable warnings

    // Title
    Text titleText("Game Room", roomFont, 36);
    titleText.setFillColor(Color::Cyan);
    titleText.setPosition(320, 15);

    // Join Queue Section
    Text joinLabel("Enter Player ID to Join Queue:", roomFont, 18);
    joinLabel.setFillColor(Color::White);
    joinLabel.setPosition(50, 70);

    RectangleShape idBox(Vector2f(150, 35));
    idBox.setPosition(280, 65);
    idBox.setFillColor(Color::White);
    idBox.setOutlineThickness(2);
    idBox.setOutlineColor(Color::Yellow);

    Text idInput("", roomFont, 20);
    idInput.setFillColor(Color::Black);
    idInput.setPosition(290, 70);

    RectangleShape joinButton(Vector2f(120, 35));
    joinButton.setPosition(450, 65);
    joinButton.setFillColor(Color(100, 200, 100));

    Text joinButtonText("Join Queue", roomFont, 16);
    joinButtonText.setFillColor(Color::Black);
    joinButtonText.setPosition(465, 72);

    // Waiting Queue Display
    Text queueLabel("Waiting Queue (Highest Score First):", roomFont, 16);
    queueLabel.setFillColor(Color::Yellow);
    queueLabel.setPosition(50, 120);

    RectangleShape queuePanel(Vector2f(340, 250));
    queuePanel.setPosition(50, 145);
    queuePanel.setFillColor(Color(40, 40, 40));
    queuePanel.setOutlineColor(Color::White);
    queuePanel.setOutlineThickness(2);

    // Match Queue Display
    Text matchLabel("Scheduled Matches:", roomFont, 16);
    matchLabel.setFillColor(Color::Yellow);
    matchLabel.setPosition(420, 120);

    RectangleShape matchPanel(Vector2f(340, 250));
    matchPanel.setPosition(420, 145);
    matchPanel.setFillColor(Color(40, 40, 40));
    matchPanel.setOutlineColor(Color::White);
    matchPanel.setOutlineThickness(2);

    // Buttons
    RectangleShape startMatchmakingBtn(Vector2f(180, 40));
    startMatchmakingBtn.setPosition(120, 420);
    startMatchmakingBtn.setFillColor(Color(200, 150, 50));

    Text startMatchmakingText("Start Matchmaking", roomFont, 16);
    startMatchmakingText.setFillColor(Color::Black);
    startMatchmakingText.setPosition(135, 430);

    RectangleShape playMatchesBtn(Vector2f(180, 40));
    playMatchesBtn.setPosition(500, 420);
    playMatchesBtn.setFillColor(Color(50, 150, 200));

    Text playMatchesText("Play Matches", roomFont, 16);
    playMatchesText.setFillColor(Color::Black);
    playMatchesText.setPosition(535, 430);

    RectangleShape clearQueueBtn(Vector2f(120, 35));
    clearQueueBtn.setPosition(330, 422);
    clearQueueBtn.setFillColor(Color(150, 100, 100));

    Text clearQueueText("Clear All", roomFont, 14);
    clearQueueText.setFillColor(Color::White);
    clearQueueText.setPosition(355, 430);

    RectangleShape backBtn(Vector2f(100, 35));
    backBtn.setPosition(350, 550);
    backBtn.setFillColor(Color(200, 100, 100));

    Text backBtnText("Back", roomFont, 16);
    backBtnText.setFillColor(Color::White);
    backBtnText.setPosition(380, 557);

    // Message text
    Text msgText("", roomFont, 16);
    msgText.setPosition(50, 480);

    // Instructions
    Text instrText("Enter Player ID (e.g., 0, 1, 2) and click 'Join Queue' | ESC: Back", roomFont, 13);
    instrText.setFillColor(Color(180, 180, 180));
    instrText.setPosition(180, 520);

    // State variables
    string playerIdInput = "";

    while (window->isOpen())
    {
        Event e;
        while (window->pollEvent(e))
        {
            if (e.type == Event::Closed)
            {
                window->close();
                return;
            }

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Escape)
                {
                    return;
                }
                else if (e.key.code == Keyboard::Enter)
                {
                    // Try to join queue with player ID
                    if (!playerIdInput.empty())
                    {
                        if (gameRoom.addPlayerByID(playerIdInput))
                        {
                            Player *p = system.returnPlayer(playerIdInput);
                            string name = p ? p->username : playerIdInput;
                            if (p)
                                delete p;
                            msgText.setFillColor(Color::Green);
                            msgText.setString(name + " (ID: " + playerIdInput + ") joined the queue!");
                            playerIdInput.clear();
                        }
                        else
                        {
                            msgText.setFillColor(Color::Red);
                            msgText.setString("Invalid Player ID or already in queue!");
                        }
                    }
                }
            }

            if (e.type == Event::TextEntered)
            {
                char c = (char)e.text.unicode;
                if (c == '\b')
                {
                    if (!playerIdInput.empty())
                        playerIdInput.pop_back();
                }
                else if (c >= '0' && c <= '9' && playerIdInput.size() < 10)
                {
                    playerIdInput.push_back(c);
                }
            }

            if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left)
            {
                Vector2f mousePos(static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y));

                // Join button
                if (joinButton.getGlobalBounds().contains(mousePos))
                {
                    if (!playerIdInput.empty())
                    {
                        if (gameRoom.addPlayerByID(playerIdInput))
                        {
                            Player *p = system.returnPlayer(playerIdInput);
                            string name = p ? p->username : playerIdInput;
                            if (p)
                                delete p;
                            msgText.setFillColor(Color::Green);
                            msgText.setString(name + " (ID: " + playerIdInput + ") joined the queue!");
                            playerIdInput.clear();
                        }
                        else
                        {
                            msgText.setFillColor(Color::Red);
                            msgText.setString("Invalid Player ID or already in queue!");
                        }
                    }
                    else
                    {
                        msgText.setFillColor(Color::Red);
                        msgText.setString("Please enter a Player ID!");
                    }
                }
                // Start Matchmaking button
                else if (startMatchmakingBtn.getGlobalBounds().contains(mousePos))
                {
                    if (gameRoom.canStartMatchmaking())
                    {
                        int matches = gameRoom.createMatches();
                        msgText.setFillColor(Color::Green);
                        msgText.setString("Created " + to_string(matches) + " match(es)! Click 'Play Matches' to begin.");
                    }
                    else
                    {
                        msgText.setFillColor(Color::Red);
                        msgText.setString("Need at least 2 players to start matchmaking!");
                    }
                }
                // Play Matches button
                else if (playMatchesBtn.getGlobalBounds().contains(mousePos))
                {
                    if (gameRoom.hasNextMatch())
                    {
                        playScheduledMatches(gameRoom);
                        msgText.setFillColor(Color::Cyan);
                        msgText.setString("All matches completed!");
                    }
                    else
                    {
                        msgText.setFillColor(Color::Red);
                        msgText.setString("No matches scheduled! Click 'Start Matchmaking' first.");
                    }
                }
                // Clear Queue button
                else if (clearQueueBtn.getGlobalBounds().contains(mousePos))
                {
                    gameRoom.clearAll();
                    msgText.setFillColor(Color::Yellow);
                    msgText.setString("Queue cleared!");
                }
                // Back button
                else if (backBtn.getGlobalBounds().contains(mousePos))
                {
                    return;
                }
            }
        }

        // Update input display
        idInput.setString(playerIdInput + "_");

        // Draw everything
        window->clear(Color(30, 30, 50));

        // Title
        window->draw(titleText);

        // Join section
        window->draw(joinLabel);
        window->draw(idBox);
        window->draw(idInput);
        window->draw(joinButton);
        window->draw(joinButtonText);

        // Queue panel
        window->draw(queueLabel);
        window->draw(queuePanel);

        // Draw waiting players
        int queueCount = gameRoom.getWaitingCount();
        for (int i = 0; i < queueCount && i < 7; i++)
        {
            QueuedPlayer qp = gameRoom.getWaitingPlayer(i);
            Text playerText(to_string(i + 1) + ". " + qp.username + " (ID:" + qp.playerID + ", Score:" + to_string(qp.score) + ")", roomFont, 14);
            playerText.setFillColor(i == 0 ? Color::Yellow : Color::White);
            playerText.setPosition(60, 155 + i * 32);
            window->draw(playerText);
        }

        if (queueCount == 0)
        {
            Text emptyText("No players in queue", roomFont, 14);
            emptyText.setFillColor(Color(150, 150, 150));
            emptyText.setPosition(130, 250);
            window->draw(emptyText);
        }

        // Queue count
        Text countText("Total: " + to_string(queueCount) + " player(s)", roomFont, 12);
        countText.setFillColor(Color::Cyan);
        countText.setPosition(60, 375);
        window->draw(countText);

        // Match panel
        window->draw(matchLabel);
        window->draw(matchPanel);

        // Draw scheduled matches
        int matchCount = gameRoom.getMatchCount();
        for (int i = 0; i < matchCount && i < 6; i++)
        {
            Match m = gameRoom.getMatchAt(i);
            Text matchText("Match " + to_string(i + 1) + ": " + m.p1Username + " vs " + m.p2Username, roomFont, 13);
            matchText.setFillColor(Color::White);
            matchText.setPosition(430, 155 + i * 38);
            window->draw(matchText);

            Text scoreText("(Score: " + to_string(m.p1Score) + " vs " + to_string(m.p2Score) + ")", roomFont, 11);
            scoreText.setFillColor(Color(180, 180, 180));
            scoreText.setPosition(440, 172 + i * 38);
            window->draw(scoreText);
        }

        if (matchCount == 0)
        {
            Text emptyMatchText("No matches scheduled", roomFont, 14);
            emptyMatchText.setFillColor(Color(150, 150, 150));
            emptyMatchText.setPosition(500, 250);
            window->draw(emptyMatchText);
        }

        // Match count
        Text matchCountText("Total: " + to_string(matchCount) + " match(es)", roomFont, 12);
        matchCountText.setFillColor(Color::Cyan);
        matchCountText.setPosition(430, 375);
        window->draw(matchCountText);

        // Buttons
        window->draw(startMatchmakingBtn);
        window->draw(startMatchmakingText);
        window->draw(playMatchesBtn);
        window->draw(playMatchesText);
        window->draw(clearQueueBtn);
        window->draw(clearQueueText);
        window->draw(backBtn);
        window->draw(backBtnText);

        window->draw(msgText);
        window->draw(instrText);

        window->display();
    }
}

// Play all scheduled matches sequentially
void AuthenticationSystem::playScheduledMatches(GameRoom &gameRoom)
{
    while (gameRoom.hasNextMatch() && window->isOpen())
    {
        Match m = gameRoom.getNextMatch();

        Multiplayer mp;
        mp.run(Color::Green, m.p1, m.p2, *window);

        // Reset window size after match
        window->setSize(Vector2u(800, 600));
        View view(FloatRect(0, 0, 800, 600));
        window->setView(view);

        Event evt;
        while (window->pollEvent(evt))
        {
            if (evt.type == Event::Closed)
            {
                window->close();
                return;
            }
        }
    }
}
