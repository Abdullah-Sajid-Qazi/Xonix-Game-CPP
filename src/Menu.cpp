// Xonix Game
// Main menu system with theme selection, profile, leaderboard

#include <SFML/Graphics.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <iostream>
#include <fstream>
#include <cstring>
#include "../header/System.h"
#include "../header/FriendManagement.h"
#include "../header/Game.h"
#include "../header/Multiplayer.h"
#include "../header/Inventory.h"

#include "../header/LeaderBoard.h"
using namespace std;
using namespace sf;

// External declarations for music control (defined in Authentication.cpp)
extern bool musicEnabled;
extern void toggleBackgroundMusic();
extern bool isMusicEnabled();
extern void setMusicEnabled(bool enabled);

class MenuSystem
{
private:
    RenderWindow *window;
    Font font;
    Font msgfont;
    Text options[9];
    Text welcomeMsg;
    int selectedIndex = 0;
    bool shouldLogout = false; // Flag to indicate logout (return to login screen)
    bool shouldExit = false;   // Flag to indicate exit (close application)

    string playerID;
    System system;

    ThemeInventory themeInventory;
    Texture backgroundTexture;
    Sprite backgroundSprite;

    Texture inventorybackgroundTexture;
    Sprite inventorybackgroundSprite;

    Texture profilebackgroundTexture;
    Sprite profilebackgroundSprite;

    Texture leaderboardTexture;
    Sprite leaderboardSprite;

    Texture gameTypeTexture;
    Sprite gameTypeSprite;

    Texture friendsTexture;
    Sprite friendsSprite;

    Texture historyTexture;
    Sprite historySprite;

    Texture gameOverTexture;
    Sprite gameOverSprite;

    Texture settingsTexture;
    Sprite settingsSprite;

    const string labels[9] = {
        "Play Game",
        "Leaderboard",
        "Player Profile",
        "Inventory",
        "Friend List",
        "Match History",
        "Settings",
        "Logout",
        "Exit"};

public:
    MenuSystem(RenderWindow *win, const string &id) : window(win), playerID(id)
    {
        if (!font.loadFromFile("assets/fonts/STENCIL.ttf"))
        {
            cout << "Failed to load font\n";
        }
        if (!msgfont.loadFromFile("assets/fonts/MATURASC.ttf"))
        {
            cout << "Failed to load font\n";
        }
        if (!backgroundTexture.loadFromFile("assets/images/menuBackground.png"))
        {
            cout << "Failed to load image\n";
        }
        backgroundSprite.setTexture(backgroundTexture);
        if (!inventorybackgroundTexture.loadFromFile("assets/images/inventoryBackground.png"))
        {
            cout << "Failed to load image\n";
        }
        inventorybackgroundSprite.setTexture(inventorybackgroundTexture);

        if (!profilebackgroundTexture.loadFromFile("assets/images/background.jpg"))
        {
            cout << "Failed to load image\n";
        }
        profilebackgroundSprite.setTexture(profilebackgroundTexture);

        if (!leaderboardTexture.loadFromFile("assets/images/leaderboardBackground.png"))
        {
            cout << "Failed to load image\n";
        }
        leaderboardSprite.setTexture(leaderboardTexture);
        if (!gameTypeTexture.loadFromFile("assets/images/BgXonic.png"))
        {
            cout << "Failed to load image\n";
        }
        gameTypeSprite.setTexture(gameTypeTexture);

        if (!friendsTexture.loadFromFile("assets/images/FriendsBackground.png"))
        {
            cout << "Failed to load image\n";
        }
        friendsSprite.setTexture(friendsTexture);

        if (!historyTexture.loadFromFile("assets/images/historyBackground.png"))
        {
            cout << "Failed to load image\n";
        }
        historySprite.setTexture(historyTexture);

        if (!gameOverTexture.loadFromFile("assets/images/gameOverBackground.png"))
        {
            cout << "Failed to load image\n";
        }
        gameOverSprite.setTexture(gameOverTexture);

        if (!settingsTexture.loadFromFile("assets/images/setting-background.jpg"))
        {
            cout << "Failed to load settings background image\n";
        }
        settingsSprite.setTexture(settingsTexture);

        setUI();
    }
    void setUI()
    {
        float windowWidth = static_cast<float>(window->getSize().x);

        for (int i = 0; i < 9; ++i)
        {
            options[i].setFont(font);
            options[i].setString(labels[i]);
            options[i].setCharacterSize(28);

            // setting postion
            FloatRect textBounds = options[i].getLocalBounds();
            float xPosition = (windowWidth - textBounds.width) / 2;

            options[i].setPosition(xPosition, 140.f + i * 45.f); // Reduced spacing for 9 items

            options[i].setFillColor(i == selectedIndex ? Color::Yellow : Color::White);
        }
        welcomeMsg.setFont(msgfont);
        welcomeMsg.setCharacterSize(70);
        welcomeMsg.setFillColor(Color::Blue);
        Player *p = system.returnPlayer(playerID);

        welcomeMsg.setString("Welcome " + p->username + "!");

        delete p;
        welcomeMsg.setPosition(80, 50);
    }

    // Returns: 0 = window closed, 1 = logout (return to login), 2 = exit (close app)
    int run()
    {
        while (window->isOpen())
        {
            handleInput();
            render();

            // Check if logout or exit was requested
            if (shouldLogout)
            {
                shouldLogout = false;
                return 1; // Logout - return to login screen
            }
            if (shouldExit)
            {
                return 2; // Exit - close application
            }
        }
        return 0; // Window was closed
    }

private:
    void handleInput()
    {
        Event event;
        while (window->pollEvent(event))
        {
            if (event.type == Event::Closed)
                window->close();

            handleKeyboard(event);
            handleMouse(event);
        }
    }

    void handleKeyboard(const Event &event)
    {
        if (event.type == Event::KeyPressed)
        {
            if (event.key.code == Keyboard::Up)
            {
                selectedIndex = (selectedIndex - 1 + 9) % 9;
                updateHighlight();
            }
            else if (event.key.code == Keyboard::Down)
            {
                selectedIndex = (selectedIndex + 1) % 9;
                updateHighlight();
            }
            else if (event.key.code == Keyboard::Enter)
            {
                execute(labels[selectedIndex]);
            }
        }
    }

    void handleMouse(const Event &event)
    {
        if (event.type == Event::MouseButtonPressed)
        {
            Vector2f mousePos = window->mapPixelToCoords(Mouse::getPosition(*window));
            for (int i = 0; i < 9; ++i)
            {
                if (options[i].getGlobalBounds().contains(mousePos))
                {
                    execute(labels[i]);
                    break;
                }
            }
        }
    }

    void updateHighlight()
    {
        for (int i = 0; i < 9; ++i)
        {
            options[i].setFillColor(i == selectedIndex ? Color::Blue : Color::White);
        }
    }

    // Get theme background color using AVL tree O(log n) lookup
    Color gettheme(int themeID)
    {
        AVLNode *theme = themeInventory.searchThemeByID(themeID);
        if (theme)
            return themeInventory.getBackgroundColor(theme->CC);
        return Color(20, 20, 20);
    }

    string getThemeImagePath(int themeID)
    {
        AVLNode *theme = themeInventory.searchThemeByID(themeID);
        if (theme)
            return theme->backgroundImagePath;
        return "";
    }

    int getPlayerTheme()
    {
        return themeInventory.loadPlayerThemePreference(playerID);
    }

    int promptPlayMode(bool &selectedToPlay)
    {
        selectedToPlay = 0;
        // Prepare the two options
        const string labels[2] = {"1) Single Player", "2) Multiplayer"};
        Text texts[2];
        for (int i = 0; i < 2; i++)
        {
            texts[i].setFont(font);
            texts[i].setString(labels[i]);
            texts[i].setCharacterSize(28);
            texts[i].setFillColor(Color::White);
            // vertically center, spaced by 50px
            texts[i].setPosition(
                (window->getSize().x - texts[i].getLocalBounds().width) / 2.f,
                250.f + i * 50.f);
        }

        int highlighted = 0;
        texts[highlighted].setFillColor(Color::Yellow);

        // Loop until selection
        while (window->isOpen())
        {
            Event evt;
            while (window->pollEvent(evt))
            {
                if (evt.type == Event::Closed)
                {
                    window->close();
                    return 1;
                }
                if (evt.type == Event::KeyPressed)
                {
                    if (evt.key.code == Keyboard::Escape)
                    {
                        selectedToPlay = 0;
                        return 1;
                    }
                    if (evt.key.code == Keyboard::Up || evt.key.code == Keyboard::W)
                    {
                        texts[highlighted].setFillColor(Color::White);
                        highlighted = (highlighted + 1) % 2;
                        texts[highlighted].setFillColor(Color::Yellow);
                    }
                    else if (evt.key.code == Keyboard::Down || evt.key.code == Keyboard::S)
                    {
                        texts[highlighted].setFillColor(Color::White);
                        highlighted = (highlighted + 1) % 2;
                        texts[highlighted].setFillColor(Color::Yellow);
                    }
                    else if (evt.key.code == Keyboard::Enter)
                    {
                        selectedToPlay = 1;
                        return highlighted + 1;
                    }
                    // direct number keys
                    else if (evt.key.code == Keyboard::Num1)
                    {
                        selectedToPlay = 1;
                        return 1;
                    }
                    else if (evt.key.code == Keyboard::Num2)
                    {
                        selectedToPlay = 1;
                        return 2;
                    }
                }
            }

            // Draw
            window->clear(Color(20, 20, 20));
            window->draw(gameTypeSprite);
            for (int i = 0; i < 2; ++i)
                window->draw(texts[i]);
            window->display();
        }

        return 1;
    }

    int promptPlayNeworSave(bool &selectedToPlay)
    {
        selectedToPlay = 0;
        // Prepare the two options
        const string labels[2] = {"1) New Game", "2) Saved Game"};
        Text texts[2];
        for (int i = 0; i < 2; i++)
        {
            texts[i].setFont(font);
            texts[i].setString(labels[i]);
            texts[i].setCharacterSize(28);
            texts[i].setFillColor(Color::White);
            // vertically center, spaced by 50px
            texts[i].setPosition(
                (window->getSize().x - texts[i].getLocalBounds().width) / 2.f,
                250.f + i * 50.f);
        }

        int highlighted = 0;
        texts[highlighted].setFillColor(Color::Yellow);

        // Loop until selection
        while (window->isOpen())
        {
            Event evt;
            while (window->pollEvent(evt))
            {
                if (evt.type == Event::Closed)
                {
                    window->close();
                    return 1;
                }
                if (evt.type == Event::KeyPressed)
                {
                    if (evt.key.code == Keyboard::Escape)
                    {
                        selectedToPlay = 0;
                        return 1;
                    }
                    if (evt.key.code == Keyboard::Up || evt.key.code == Keyboard::W)
                    {
                        texts[highlighted].setFillColor(Color::White);
                        highlighted = (highlighted + 1) % 2;
                        texts[highlighted].setFillColor(Color::Yellow);
                    }
                    else if (evt.key.code == Keyboard::Down || evt.key.code == Keyboard::S)
                    {
                        texts[highlighted].setFillColor(Color::White);
                        highlighted = (highlighted + 1) % 2;
                        texts[highlighted].setFillColor(Color::Yellow);
                    }
                    else if (evt.key.code == Keyboard::Enter)
                    {
                        selectedToPlay = 1;
                        return highlighted + 1;
                    }
                    // direct number keys
                    else if (evt.key.code == Keyboard::Num1)
                    {
                        selectedToPlay = 1;
                        return 1;
                    }
                    else if (evt.key.code == Keyboard::Num2)
                    {
                        selectedToPlay = 1;
                        return 2;
                    }
                }
            }

            // Draw
            window->clear(Color(20, 20, 20));
            window->draw(gameTypeSprite);
            for (int i = 0; i < 2; ++i)
                window->draw(texts[i]);
            window->display();
        }

        return 1;
    }

    /*
     * Function: promptSaveID
     * Description: Prompts the player to enter a Save ID to load a saved game.
     *              The Save ID is a unique identifier generated when the game was saved.
     *              Supports pasting from clipboard with Ctrl+V.
     * Parameters:
     *   selectedToPlay - Reference to flag indicating if valid selection was made
     * Returns: The entered Save ID string, or empty string if cancelled
     */
    string promptSaveID(bool &selectedToPlay)
    {
        selectedToPlay = 0;
        Text promptText, inputText, errorText, instructionText, helpText, pasteHintText;

        promptText.setFont(font);
        promptText.setString("Enter Save ID:");
        promptText.setCharacterSize(28);
        promptText.setFillColor(Color::White);
        promptText.setPosition(
            (window->getSize().x - promptText.getLocalBounds().width) / 2.f,
            80.f);

        instructionText.setFont(font);
        instructionText.setString("(Save ID format: playerID_YYYYMMDD_HHMMSS)");
        instructionText.setCharacterSize(16);
        instructionText.setFillColor(Color::Cyan);
        instructionText.setPosition(
            (window->getSize().x - instructionText.getLocalBounds().width) / 2.f,
            120.f);

        pasteHintText.setFont(font);
        pasteHintText.setString("Tip: Press Ctrl+V to paste from clipboard (copy from Settings)");
        pasteHintText.setCharacterSize(14);
        pasteHintText.setFillColor(Color::Green);
        pasteHintText.setPosition(
            (window->getSize().x - pasteHintText.getLocalBounds().width) / 2.f,
            150.f);

        inputText.setFont(font);
        inputText.setCharacterSize(24);
        inputText.setFillColor(Color::Yellow);
        inputText.setPosition(
            (window->getSize().x - 400.f) / 2.f,
            200.f);

        errorText.setFont(font);
        errorText.setCharacterSize(20);
        errorText.setFillColor(Color::Red);
        errorText.setPosition(
            (window->getSize().x - 400.f) / 2.f,
            260.f);

        helpText.setFont(font);
        helpText.setString("Enter: Load | Ctrl+V: Paste | Escape: Cancel");
        helpText.setCharacterSize(18);
        helpText.setFillColor(Color::White);
        helpText.setPosition(
            (window->getSize().x - helpText.getLocalBounds().width) / 2.f,
            500.f);

        string input;
        string errorMsg = "";

        while (window->isOpen())
        {
            Event evt;
            while (window->pollEvent(evt))
            {
                if (evt.type == Event::Closed)
                    window->close();

                if (evt.type == Event::KeyPressed)
                {
                    if (evt.key.code == Keyboard::Escape)
                    {
                        selectedToPlay = false;
                        return "";
                    }
                    // Handle Ctrl+V for paste
                    else if (evt.key.code == Keyboard::V && evt.key.control)
                    {
                        // Get text from clipboard
                        String clipboardText = Clipboard::getString();
                        string pastedText = clipboardText.toAnsiString();

                        // Remove any newlines or extra whitespace
                        string cleanText = "";
                        for (size_t i = 0; i < pastedText.length(); i++)
                        {
                            char c = pastedText[i];
                            if (c >= 32 && c < 127 && c != ' ')
                            {
                                cleanText += c;
                            }
                        }

                        if (!cleanText.empty())
                        {
                            input = cleanText;
                            errorMsg = "";
                        }
                    }
                    // Handle Enter key
                    else if (evt.key.code == Keyboard::Enter)
                    {
                        if (input.empty())
                        {
                            errorMsg = "Error: Invalid Save ID!";
                        }
                        else
                        {
                            // Check if save belongs to the logged-in player
                            // Save ID format: playerID_YYYYMMDD_HHMMSS
                            size_t underscorePos = input.find('_');
                            if (underscorePos == string::npos)
                            {
                                errorMsg = "Error: Invalid Save ID!";
                            }
                            else
                            {
                                string saveOwnerID = input.substr(0, underscorePos);
                                if (saveOwnerID != playerID)
                                {
                                    errorMsg = "Error: Invalid Save ID!";
                                }
                                else
                                {
                                    // Check if save file exists
                                    string filename = "data/saves/" + input + ".sav";
                                    ifstream testFile(filename);
                                    if (!testFile.is_open())
                                    {
                                        // Try current directory
                                        filename = input + ".sav";
                                        testFile.open(filename);
                                    }

                                    if (testFile.is_open())
                                    {
                                        testFile.close();
                                        selectedToPlay = true;
                                        return input;
                                    }
                                    else
                                    {
                                        errorMsg = "Error: Invalid Save ID!";
                                    }
                                }
                            }
                        }
                    }
                }

                if (evt.type == Event::TextEntered)
                {
                    uint32_t c = evt.text.unicode;
                    // Skip Enter key (handled above) and Ctrl characters
                    if (c == '\r' || c == '\n' || c < 32)
                    {
                        // Skip control characters
                    }
                    else if (c == '\b')
                    {
                        if (!input.empty())
                        {
                            input.pop_back();
                            errorMsg = "";
                        }
                    }
                    else if (c >= 32 && c < 127)
                    {
                        input.push_back(static_cast<char>(c));
                        errorMsg = "";
                    }
                }
            }

            inputText.setString(input + "_");
            errorText.setString(errorMsg);

            window->clear(Color(20, 20, 20));
            window->draw(gameTypeSprite);
            window->draw(promptText);
            window->draw(instructionText);
            window->draw(pasteHintText);
            window->draw(inputText);
            window->draw(helpText);
            if (!errorMsg.empty())
            {
                window->draw(errorText);
            }
            window->display();
        }

        return "";
    }

    string promptOpponentID(bool &selectedToPlay)
    {
        selectedToPlay = 0;

        // Load current player to check friend list
        Player *currentPlayer = system.returnPlayer(playerID);
        if (!currentPlayer)
        {
            return "";
        }

        Text promptText, inputText, errorText, instructionText;
        promptText.setFont(font);
        promptText.setString("Enter Friend's Player ID:");
        promptText.setCharacterSize(24);
        promptText.setFillColor(Color::White);
        // center-top
        promptText.setPosition(
            (window->getSize().x - promptText.getLocalBounds().width) / 2.f,
            100.f);

        instructionText.setFont(font);
        instructionText.setString("(You can only play multiplayer with your friends)");
        instructionText.setCharacterSize(16);
        instructionText.setFillColor(Color::Cyan);
        instructionText.setPosition(
            (window->getSize().x - instructionText.getLocalBounds().width) / 2.f,
            140.f);

        inputText.setFont(font);
        inputText.setCharacterSize(24);
        inputText.setFillColor(Color::Yellow);
        // below prompt
        inputText.setPosition(
            (window->getSize().x - 400.f) / 2.f, // 400px wide input box
            200.f);

        errorText.setFont(font);
        errorText.setCharacterSize(20);
        errorText.setFillColor(Color::Red);
        errorText.setPosition(
            (window->getSize().x - 400.f) / 2.f,
            250.f);

        Text helpText;
        helpText.setFont(font);
        helpText.setString("Press Enter to confirm | Escape to cancel");
        helpText.setCharacterSize(18);
        helpText.setFillColor(Color::White);
        helpText.setPosition(
            (window->getSize().x - helpText.getLocalBounds().width) / 2.f,
            500.f);

        string input; // the accumulating user string
        string errorMsg = "";

        while (window->isOpen())
        {
            Event evt;
            while (window->pollEvent(evt))
            {
                if (evt.type == Event::Closed)
                    window->close();

                if (evt.type == Event::KeyPressed)
                {
                    if (evt.key.code == Keyboard::Escape)
                    {
                        // Cancel and return to menu - do NOT start match
                        selectedToPlay = false;
                        delete currentPlayer;
                        return ""; // Empty string means no opponent selected
                    }
                }

                if (evt.type == Event::TextEntered)
                {
                    uint32_t c = evt.text.unicode;
                    if (c == '\r' || c == '\n')
                    {
                        if (input.empty())
                        {
                            errorMsg = "Error: Please enter a Player ID!";
                        }
                        else if (input == playerID)
                        {
                            errorMsg = "Error: Cannot play against yourself!";
                        }
                        else if (!system.isIdPresent(input))
                        {
                            errorMsg = "Error: Player ID '" + input + "' not found!";
                        }
                        else if (!currentPlayer->friendList.search(input))
                        {
                            errorMsg = "Error: You can only play with your friends!";
                        }
                        else
                        {
                            // Valid friend opponent found
                            selectedToPlay = 1;
                            delete currentPlayer;
                            return input;
                        }
                    }
                    else if (c == '\b')
                    {
                        if (!input.empty())
                        {
                            input.pop_back();
                            errorMsg = ""; // Clear error when typing
                        }
                    }
                    else if (c >= 32 && c < 127)
                    {
                        input.push_back(static_cast<char>(c));
                        errorMsg = ""; // Clear error when typing
                    }
                }
            }

            // Update displayed text
            inputText.setString(input + "_"); // trailing underscore cursor
            errorText.setString(errorMsg);

            // Render
            window->clear(Color(20, 20, 20));
            window->draw(gameTypeSprite);
            window->draw(promptText);
            window->draw(instructionText);
            window->draw(inputText);
            window->draw(helpText);
            if (!errorMsg.empty())
            {
                window->draw(errorText);
            }
            window->display();
        }

        delete currentPlayer;
        return ""; // window closed or fallback
    }

    /*
     * Function: displayPlayerProfile
     * Description: Displays the player's profile information including:
     *              - Username
     *              - User ID
     *              - Friends list count
     *              - Total points (high score)
     */
    void displayPlayerProfile()
    {
        Player *player = system.returnPlayer(playerID);
        if (!player)
            return;

        Font profileFont;
        if (!profileFont.loadFromFile("assets/fonts/HARLOWSI.ttf"))
        {
            cout << "Failed to load profile font\n";
        }

        // Header
        Text header;
        header.setFont(font);
        header.setCharacterSize(50);
        Color lightCyan(200, 255, 255);
        header.setFillColor(lightCyan);
        header.setPosition(150.f, 30.f);
        header.setString("Player Profile");

        // Profile information lines
        Text lines[5];
        const float startY = 120.f, lineSpacing = 55.f;
        for (int i = 0; i < 5; ++i)
        {
            lines[i].setFont(profileFont);
            lines[i].setCharacterSize(40);
            lines[i].setFillColor(Color::White);
            lines[i].setPosition(100.f, startY + i * lineSpacing);
        }

        auto updateLines = [&]()
        {
            lines[0].setString("Username: " + player->username);
            lines[1].setString("User ID: " + player->ID);
            lines[2].setString("Friends: " + to_string(player->noOfFriends));
            lines[3].setString("Highest Points: " + to_string(player->highScore));
            lines[4].setString("Power-Ups: " + to_string(player->powerUps));
        };

        // Highlight User ID with bright yellow
        lines[1].setFillColor(Color::Yellow);
        updateLines();

        // Prompt text
        Color lightGold(255, 215, 100);
        Text prompt;
        prompt.setFont(profileFont);
        prompt.setCharacterSize(32);
        prompt.setFillColor(lightGold);
        prompt.setPosition(40.f, startY + 6 * lineSpacing);
        prompt.setString("Press Esc to return");

        while (window->isOpen())
        {
            Event evt;
            while (window->pollEvent(evt))
            {
                if (evt.type == Event::Closed)
                {
                    window->close();
                    delete player;
                    return;
                }

                // Exit screen
                if (evt.type == Event::KeyPressed && evt.key.code == Keyboard::Escape)
                {
                    delete player;
                    return;
                }
            }

            window->clear(Color(30, 30, 30));
            window->draw(profilebackgroundSprite);
            window->draw(header);
            for (int i = 0; i < 5; ++i)
                window->draw(lines[i]);

            window->draw(prompt);
            window->display();
        }

        delete player;
    }

    /*
     * Function: displayInventoryScreen
     * Description: Theme Inventory System using AVL Tree.
     *              Allows players to browse and select visual themes with preview.
     *              Uses in-order traversal for sorted display.
     *              Saves player's theme preference to file.
     * Data Structure: AVL Tree (from ThemeInventory class)
     * Features:
     *   - Browse themes in sorted order by ID
     *   - Visual preview of background images
     *   - Search theme by ID
     *   - Save theme preference to player profile
     */
    void displayInventoryScreen()
    {
        // Header
        Text header;
        header.setFont(msgfont);
        header.setCharacterSize(50);
        header.setFillColor(Color::Yellow);
        header.setPosition(180.f, 10.f);
        header.setString("Inventory");

        // Subheader
        Text subHeader;
        subHeader.setFont(font);
        subHeader.setCharacterSize(18);
        subHeader.setFillColor(Color::Cyan);
        subHeader.setPosition(50.f, 70.f);
        subHeader.setString("Browse and select your game background theme");

        // Get all themes from AVL tree (in-order traversal)
        const int MAX_THEMES = 10;
        AVLNode *themes[MAX_THEMES];
        int themeCount = themeInventory.getAllThemes(themes);

        // Load player's current theme preference
        int currentThemeID = themeInventory.loadPlayerThemePreference(playerID);
        int selectedIndex = 0;

        // Find the index of current theme
        for (int i = 0; i < themeCount; i++)
        {
            if (themes[i]->ID == currentThemeID)
            {
                selectedIndex = i;
                break;
            }
        }

        // Create text objects for each theme
        Text themeTexts[MAX_THEMES];
        for (int i = 0; i < themeCount; i++)
        {
            themeTexts[i].setFont(font);
            themeTexts[i].setCharacterSize(20);
            themeTexts[i].setPosition(50.f, 110.f + i * 45.f);

            // Format: ID) Name - Description
            string themeStr = to_string(themes[i]->ID) + ") " + themes[i]->name + " - " + themes[i]->description;
            themeTexts[i].setString(themeStr);

            // Highlight current selection
            if (i == selectedIndex)
            {
                themeTexts[i].setFillColor(Color::Yellow);
            }
            else
            {
                themeTexts[i].setFillColor(Color::White);
            }
        }

        // Current theme indicator
        Text currentThemeText;
        currentThemeText.setFont(font);
        currentThemeText.setCharacterSize(18);
        currentThemeText.setFillColor(Color::Green);
        currentThemeText.setPosition(50.f, 360.f);

        // Preview label
        Text previewLabel;
        previewLabel.setFont(font);
        previewLabel.setCharacterSize(18);
        previewLabel.setFillColor(Color::White);
        previewLabel.setPosition(450.f, 110.f);
        previewLabel.setString("Preview:");

        // Preview sprite for background images
        Texture previewTexture;
        Sprite previewSprite;
        RectangleShape previewBorder(Vector2f(310.f, 180.f));
        previewBorder.setPosition(445.f, 135.f);
        previewBorder.setFillColor(Color::Transparent);
        previewBorder.setOutlineColor(Color::White);
        previewBorder.setOutlineThickness(2.f);

        // Load initial preview
        if (themeCount > 0 && !themes[selectedIndex]->backgroundImagePath.empty())
        {
            if (previewTexture.loadFromFile(themes[selectedIndex]->backgroundImagePath))
            {
                previewTexture.setSmooth(true);
                previewSprite.setTexture(previewTexture, true);
                previewSprite.setPosition(450.f, 140.f);
                float scaleX = 300.f / (float)previewTexture.getSize().x;
                float scaleY = 170.f / (float)previewTexture.getSize().y;
                previewSprite.setScale(scaleX, scaleY);
            }
        }

        // Instructions
        Text instructions;
        instructions.setFont(font);
        instructions.setCharacterSize(16);
        instructions.setFillColor(Color::Cyan);
        instructions.setPosition(50.f, 400.f);
        instructions.setString("UP/DOWN: Navigate | ENTER: Select Theme | 1-5: Quick Select | ESC: Return");

        // Search prompt
        Text searchPrompt;
        searchPrompt.setFont(font);
        searchPrompt.setCharacterSize(16);
        searchPrompt.setFillColor(Color::White);
        searchPrompt.setPosition(450.f, 330.f);
        searchPrompt.setString("Press S to search by ID");

        // Status message
        Text statusMsg;
        statusMsg.setFont(font);
        statusMsg.setCharacterSize(20);
        statusMsg.setFillColor(Color::Green);
        statusMsg.setPosition(50.f, 450.f);

        bool showStatus = false;
        Clock statusClock;

        while (window->isOpen())
        {
            // Update current theme text
            AVLNode *currentTheme = themeInventory.searchThemeByID(currentThemeID);
            if (currentTheme)
            {
                currentThemeText.setString("Current Theme: " + currentTheme->name + " (ID: " + to_string(currentThemeID) + ")");
            }

            // Hide status message after 2 seconds
            if (showStatus && statusClock.getElapsedTime().asSeconds() > 2.0f)
            {
                showStatus = false;
            }

            Event evt;
            while (window->pollEvent(evt))
            {
                if (evt.type == Event::Closed)
                {
                    window->close();
                    return;
                }

                if (evt.type == Event::KeyPressed)
                {
                    if (evt.key.code == Keyboard::Escape)
                    {
                        return;
                    }
                    else if (evt.key.code == Keyboard::Up && themeCount > 0)
                    {
                        // Navigate up
                        themeTexts[selectedIndex].setFillColor(Color::White);
                        selectedIndex = (selectedIndex - 1 + themeCount) % themeCount;
                        themeTexts[selectedIndex].setFillColor(Color::Yellow);

                        // Update preview
                        if (!themes[selectedIndex]->backgroundImagePath.empty())
                        {
                            if (previewTexture.loadFromFile(themes[selectedIndex]->backgroundImagePath))
                            {
                                previewTexture.setSmooth(true);
                                previewSprite.setTexture(previewTexture, true);
                                previewSprite.setPosition(450.f, 140.f);
                                float scaleX = 300.f / (float)previewTexture.getSize().x;
                                float scaleY = 170.f / (float)previewTexture.getSize().y;
                                previewSprite.setScale(scaleX, scaleY);
                            }
                        }
                    }
                    else if (evt.key.code == Keyboard::Down && themeCount > 0)
                    {
                        // Navigate down
                        themeTexts[selectedIndex].setFillColor(Color::White);
                        selectedIndex = (selectedIndex + 1) % themeCount;
                        themeTexts[selectedIndex].setFillColor(Color::Yellow);

                        // Update preview
                        if (!themes[selectedIndex]->backgroundImagePath.empty())
                        {
                            if (previewTexture.loadFromFile(themes[selectedIndex]->backgroundImagePath))
                            {
                                previewTexture.setSmooth(true);
                                previewSprite.setTexture(previewTexture, true);
                                previewSprite.setPosition(450.f, 140.f);
                                float scaleX = 300.f / (float)previewTexture.getSize().x;
                                float scaleY = 170.f / (float)previewTexture.getSize().y;
                                previewSprite.setScale(scaleX, scaleY);
                            }
                        }
                    }
                    else if (evt.key.code == Keyboard::Enter && themeCount > 0)
                    {
                        // Select theme and save preference
                        currentThemeID = themes[selectedIndex]->ID;
                        themeInventory.savePlayerThemePreference(playerID, currentThemeID);
                        statusMsg.setString("Theme '" + themes[selectedIndex]->name + "' selected and saved!");
                        showStatus = true;
                        statusClock.restart();
                    }
                    // Quick select with number keys 1-5
                    else if (evt.key.code >= Keyboard::Num1 && evt.key.code <= Keyboard::Num5)
                    {
                        int quickID = (int)(evt.key.code - Keyboard::Num1) + 1;
                        AVLNode *quickTheme = themeInventory.searchThemeByID(quickID);
                        if (quickTheme)
                        {
                            // Find index
                            for (int i = 0; i < themeCount; i++)
                            {
                                if (themes[i]->ID == quickID)
                                {
                                    themeTexts[selectedIndex].setFillColor(Color::White);
                                    selectedIndex = i;
                                    themeTexts[selectedIndex].setFillColor(Color::Yellow);

                                    // Update preview
                                    if (!themes[selectedIndex]->backgroundImagePath.empty())
                                    {
                                        if (previewTexture.loadFromFile(themes[selectedIndex]->backgroundImagePath))
                                        {
                                            previewTexture.setSmooth(true);
                                            previewSprite.setTexture(previewTexture, true);
                                            previewSprite.setPosition(450.f, 140.f);
                                            float scaleX = 300.f / (float)previewTexture.getSize().x;
                                            float scaleY = 170.f / (float)previewTexture.getSize().y;
                                            previewSprite.setScale(scaleX, scaleY);
                                        }
                                    }

                                    // Save preference
                                    currentThemeID = quickID;
                                    themeInventory.savePlayerThemePreference(playerID, currentThemeID);
                                    statusMsg.setString("Theme '" + quickTheme->name + "' selected and saved!");
                                    showStatus = true;
                                    statusClock.restart();
                                    break;
                                }
                            }
                        }
                    }
                    else if (evt.key.code == Keyboard::S)
                    {
                        // Search by ID - prompt for ID input
                        int searchID = promptThemeSearchID();
                        if (searchID > 0)
                        {
                            AVLNode *foundTheme = themeInventory.searchThemeByID(searchID);
                            if (foundTheme)
                            {
                                // Find index and highlight
                                for (int i = 0; i < themeCount; i++)
                                {
                                    if (themes[i]->ID == searchID)
                                    {
                                        themeTexts[selectedIndex].setFillColor(Color::White);
                                        selectedIndex = i;
                                        themeTexts[selectedIndex].setFillColor(Color::Yellow);

                                        // Update preview
                                        if (!themes[selectedIndex]->backgroundImagePath.empty())
                                        {
                                            if (previewTexture.loadFromFile(themes[selectedIndex]->backgroundImagePath))
                                            {
                                                previewTexture.setSmooth(true);
                                                previewSprite.setTexture(previewTexture, true);
                                                previewSprite.setPosition(450.f, 140.f);
                                                float scaleX = 300.f / (float)previewTexture.getSize().x;
                                                float scaleY = 170.f / (float)previewTexture.getSize().y;
                                                previewSprite.setScale(scaleX, scaleY);
                                            }
                                        }

                                        statusMsg.setString("Found: " + foundTheme->name);
                                        statusMsg.setFillColor(Color::Green);
                                        showStatus = true;
                                        statusClock.restart();
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                statusMsg.setString("Theme ID " + to_string(searchID) + " not found!");
                                statusMsg.setFillColor(Color::Red);
                                showStatus = true;
                                statusClock.restart();
                            }
                        }
                    }
                }
            }

            // Draw
            window->clear(Color(30, 30, 30));
            window->draw(inventorybackgroundSprite);
            window->draw(header);
            window->draw(subHeader);

            // Draw theme list
            for (int i = 0; i < themeCount; i++)
            {
                window->draw(themeTexts[i]);
            }

            window->draw(currentThemeText);
            window->draw(previewLabel);
            window->draw(previewBorder);
            window->draw(previewSprite);
            window->draw(instructions);
            window->draw(searchPrompt);

            if (showStatus)
            {
                window->draw(statusMsg);
            }

            window->display();
        }
    }

    /*
     * Function: promptThemeSearchID
     * Description: Prompts user to enter a Theme ID to search for.
     *              Uses AVL tree search for O(log n) lookup.
     * Returns: Theme ID entered, or 0 if cancelled
     */
    int promptThemeSearchID()
    {
        Text promptText, inputText;

        promptText.setFont(font);
        promptText.setString("Enter Theme ID (1-5):");
        promptText.setCharacterSize(24);
        promptText.setFillColor(Color::White);
        promptText.setPosition(250.f, 200.f);

        inputText.setFont(font);
        inputText.setCharacterSize(28);
        inputText.setFillColor(Color::Yellow);
        inputText.setPosition(300.f, 260.f);

        Text helpText;
        helpText.setFont(font);
        helpText.setString("Enter: Search | Esc: Cancel");
        helpText.setCharacterSize(16);
        helpText.setFillColor(Color::Cyan);
        helpText.setPosition(270.f, 320.f);

        string input = "";

        while (window->isOpen())
        {
            Event evt;
            while (window->pollEvent(evt))
            {
                if (evt.type == Event::Closed)
                {
                    window->close();
                    return 0;
                }

                if (evt.type == Event::KeyPressed)
                {
                    if (evt.key.code == Keyboard::Escape)
                    {
                        return 0;
                    }
                    else if (evt.key.code == Keyboard::Enter && !input.empty())
                    {
                        // Convert to int
                        int id = 0;
                        for (size_t i = 0; i < input.length(); i++)
                        {
                            id = id * 10 + (input[i] - '0');
                        }
                        return id;
                    }
                }

                if (evt.type == Event::TextEntered)
                {
                    char c = static_cast<char>(evt.text.unicode);
                    if (c >= '0' && c <= '9' && input.length() < 2)
                    {
                        input += c;
                    }
                    else if (c == '\b' && !input.empty())
                    {
                        input.pop_back();
                    }
                }
            }

            inputText.setString(input + "_");

            window->clear(Color(40, 40, 40));
            window->draw(promptText);
            window->draw(inputText);
            window->draw(helpText);
            window->display();
        }

        return 0;
    }

    void displayGameOverScreen(int score)
    {
        // Prepare texts
        Text title, scoreText, prompt;
        title.setFont(font);
        title.setString("Game Over");
        title.setCharacterSize(48);
        title.setFillColor(Color::Red);
        // center at y = 150
        {
            auto bounds = title.getLocalBounds();
            title.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            title.setPosition(window->getSize().x / 2.f, 150.f);
        }

        scoreText.setFont(font);
        scoreText.setString("Your Score: " + to_string(score));
        scoreText.setCharacterSize(32);
        scoreText.setFillColor(Color::White);
        {
            auto bounds = scoreText.getLocalBounds();
            scoreText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            scoreText.setPosition(window->getSize().x / 2.f, 250.f);
        }

        prompt.setFont(font);
        prompt.setString("Press Esc to return to Main Menu");
        prompt.setCharacterSize(24);
        prompt.setFillColor(Color::Yellow);
        {
            auto bounds = prompt.getLocalBounds();
            prompt.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            prompt.setPosition(window->getSize().x / 2.f, 350.f);
        }

        // Loop until Esc or window close
        while (window->isOpen())
        {
            Event evt;
            while (window->pollEvent(evt))
            {
                if (evt.type == Event::Closed)
                {
                    window->close();
                    return;
                }
                if (evt.type == Event::KeyPressed && evt.key.code == Keyboard::Escape)
                {
                    return; // back to main menu
                }
            }

            window->clear(Color(20, 20, 20));
            window->draw(gameOverSprite);
            window->draw(title);
            window->draw(scoreText);
            window->draw(prompt);
            window->display();
        }
    }

    /*
     * Function: displaySaveConfirmation
     * Description: Displays a confirmation screen after game is saved.
     *              Shows the unique Save ID that player needs to resume later.
     * Parameters:
     *   saveID - The unique Save ID that was generated
     */
    void displaySaveConfirmation(const string &saveID)
    {
        Text title, saveIDText, instructionText, prompt;

        title.setFont(font);
        title.setString("Game Saved!");
        title.setCharacterSize(48);
        title.setFillColor(Color::Green);
        {
            auto bounds = title.getLocalBounds();
            title.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            title.setPosition(window->getSize().x / 2.f, 120.f);
        }

        saveIDText.setFont(font);
        saveIDText.setString("Save ID: " + saveID);
        saveIDText.setCharacterSize(24);
        saveIDText.setFillColor(Color::Yellow);
        {
            auto bounds = saveIDText.getLocalBounds();
            saveIDText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            saveIDText.setPosition(window->getSize().x / 2.f, 220.f);
        }

        instructionText.setFont(font);
        instructionText.setString("Write down this Save ID to resume your game later!");
        instructionText.setCharacterSize(20);
        instructionText.setFillColor(Color::White);
        {
            auto bounds = instructionText.getLocalBounds();
            instructionText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            instructionText.setPosition(window->getSize().x / 2.f, 300.f);
        }

        prompt.setFont(font);
        prompt.setString("Press Esc to return to Main Menu");
        prompt.setCharacterSize(20);
        prompt.setFillColor(Color::Cyan);
        {
            auto bounds = prompt.getLocalBounds();
            prompt.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            prompt.setPosition(window->getSize().x / 2.f, 400.f);
        }

        while (window->isOpen())
        {
            Event evt;
            while (window->pollEvent(evt))
            {
                if (evt.type == Event::Closed)
                {
                    window->close();
                    return;
                }
                if (evt.type == Event::KeyPressed && evt.key.code == Keyboard::Escape)
                {
                    return;
                }
            }

            window->clear(Color(20, 20, 20));
            window->draw(gameOverSprite);
            window->draw(title);
            window->draw(saveIDText);
            window->draw(instructionText);
            window->draw(prompt);
            window->display();
        }
    }

    void displayMatchHistory()
    {
        Player *player = system.returnPlayer(playerID);
        if (!player)
            return;

        int total = player->noOfMatches;

        // Handle empty history
        if (total == 0)
        {
            Text noMatchText;
            noMatchText.setFont(font);
            noMatchText.setCharacterSize(24);
            noMatchText.setFillColor(Color::White);
            noMatchText.setString("No matches played yet!");
            noMatchText.setPosition(200.f, 300.f);

            Text title;
            title.setFont(msgfont);
            title.setCharacterSize(70);
            title.setFillColor(Color::Green);
            title.setString("Match History");
            title.setPosition(20.f, 20.f);

            Text prompt;
            prompt.setFont(font);
            prompt.setCharacterSize(18);
            prompt.setFillColor(Color::Cyan);
            prompt.setString("Press Esc to return");
            prompt.setPosition(300.f, 500.f);

            while (window->isOpen())
            {
                Event evt;
                while (window->pollEvent(evt))
                {
                    if (evt.type == Event::Closed)
                    {
                        window->close();
                        delete player;
                        return;
                    }
                    if (evt.type == Event::KeyPressed && evt.key.code == Keyboard::Escape)
                    {
                        delete player;
                        return;
                    }
                }
                window->clear(Color(30, 30, 30));
                window->draw(historySprite);
                window->draw(title);
                window->draw(noMatchText);
                window->draw(prompt);
                window->display();
            }
            delete player;
            return;
        }

        Text *entries = new Text[total];
        string *raw = new string[total];

        // Get match history from stack (most recent first)
        node *curr = player->matchHistory.head;
        int i = 0;
        while (curr && i < total)
        {
            raw[i++] = curr->data;
            curr = curr->next;
        }

        // Format entries with match number (most recent = #1)
        for (int i = 0; i < total; ++i)
        {
            entries[i].setFont(font);
            entries[i].setCharacterSize(18);

            // Color code: Green for wins, Red for losses, White for single player
            if (raw[i].find("WIN") != string::npos)
            {
                entries[i].setFillColor(Color::Green);
            }
            else if (raw[i].find("LOSE") != string::npos)
            {
                entries[i].setFillColor(Color::Red);
            }
            else
            {
                entries[i].setFillColor(Color::White);
            }

            // Add match number
            entries[i].setString("#" + to_string(i + 1) + "  " + raw[i]);
        }

        delete[] raw;

        int offset = 0; // scroll state
        const int visible = 9;
        bool running = true;

        while (running && window->isOpen())
        {
            Event evt;
            while (window->pollEvent(evt))
            {
                if (evt.type == Event::Closed)
                {
                    window->close();
                    running = false;
                }
                else if (evt.type == Event::KeyPressed)
                {
                    if (evt.key.code == Keyboard::Escape)
                    {
                        running = false;
                        delete[] entries;
                        delete player;
                        return;
                    }
                    else if (evt.key.code == Keyboard::Down)
                    {
                        if (offset + visible < total)
                            offset++;
                    }
                    else if (evt.key.code == Keyboard::Up)
                    {
                        if (offset > 0)
                            offset--;
                    }
                }
            }

            window->clear(Color(30, 30, 30));
            window->draw(historySprite);

            Text title;
            title.setFont(msgfont);
            title.setCharacterSize(60);
            title.setFillColor(Color::Green);
            title.setString("Match History");
            title.setPosition(20.f, 20.f);
            window->draw(title);

            // Show total matches count
            Text countText;
            countText.setFont(font);
            countText.setCharacterSize(18);
            countText.setFillColor(Color::Yellow);
            countText.setString("Total Matches: " + to_string(total));
            countText.setPosition(20.f, 100.f);
            window->draw(countText);

            // Instructions
            Text instrText;
            instrText.setFont(font);
            instrText.setCharacterSize(14);
            instrText.setFillColor(Color::Cyan);
            instrText.setString("Use UP/DOWN to scroll | Esc to return");
            instrText.setPosition(20.f, 560.f);
            window->draw(instrText);

            float y = 140.f;
            for (int i = offset; i < total && i < offset + visible; ++i)
            {
                entries[i].setPosition(40.f, y);
                window->draw(entries[i]);
                y += 45.f;
            }

            window->display();
        }

        delete[] entries;
        delete player;
    }

    /*
     * Function: getSavedGamesForPlayer
     * Description: Scans the saves directory and current directory for save files
     *              belonging to the specified player.
     * Parameters:
     *   playerID - The player ID to search for
     *   saveIDs - Array to store found save IDs (output)
     *   maxSaves - Maximum number of saves to return
     * Returns: Number of saves found
     */
    int getSavedGamesForPlayer(const string &playerID, string *saveIDs, int maxSaves)
    {
        int count = 0;

        // Try to find save files that start with playerID
        // Check both saves/ directory and current directory
        string directories[2] = {"data/saves/", ""};

        for (int d = 0; d < 2 && count < maxSaves; d++)
        {
            // We'll check for files by trying to open them with common patterns
            // Since we can't list directories easily without dirent.h, we'll try common patterns

            // Try pattern: playerID_YYYYMMDD_HHMMSS.sav
            // We'll check recent dates (last 30 days worth of possible saves)
            for (int year = 2025; year >= 2024 && count < maxSaves; year--)
            {
                for (int month = 12; month >= 1 && count < maxSaves; month--)
                {
                    for (int day = 31; day >= 1 && count < maxSaves; day--)
                    {
                        for (int hour = 23; hour >= 0 && count < maxSaves; hour--)
                        {
                            for (int min = 59; min >= 0 && count < maxSaves; min -= 10)
                            {
                                for (int sec = 59; sec >= 0 && count < maxSaves; sec -= 10)
                                {
                                    char dateStr[20];
                                    snprintf(dateStr, sizeof(dateStr), "%04d%02d%02d_%02d%02d%02d", year, month, day, hour, min, sec);
                                    string saveID = playerID + "_" + string(dateStr);
                                    string filename = directories[d] + saveID + ".sav";

                                    ifstream file(filename);
                                    if (file.is_open())
                                    {
                                        // Verify it's a valid save file
                                        string header;
                                        getline(file, header);
                                        if (header == "XONIX_SAVE_V1")
                                        {
                                            // Check if not already in list
                                            bool duplicate = false;
                                            for (int i = 0; i < count; i++)
                                            {
                                                if (saveIDs[i] == saveID)
                                                {
                                                    duplicate = true;
                                                    break;
                                                }
                                            }
                                            if (!duplicate)
                                            {
                                                saveIDs[count++] = saveID;
                                            }
                                        }
                                        file.close();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        return count;
    }

    /*
     * Function: findSavedGamesSimple
     * Description: Simpler approach - reads a saves index file if it exists,
     *              or scans for files matching the player ID pattern.
     */
    int findSavedGamesSimple(const string &playerID, string *saveIDs, int maxSaves)
    {
        int count = 0;

        // First, try to read from a saves index file
        string indexFile = "data/saves/index_" + playerID + ".txt";
        ifstream idx(indexFile);
        if (idx.is_open())
        {
            string line;
            while (getline(idx, line) && count < maxSaves)
            {
                if (!line.empty())
                {
                    saveIDs[count++] = line;
                }
            }
            idx.close();
            return count;
        }

        // If no index file, try to find files by testing common patterns
        // Check both directories
        string dirs[2] = {"data/saves/", ""};

        for (int d = 0; d < 2; d++)
        {
            // Try recent timestamps (brute force approach for a few recent saves)
            // This is limited but works without directory listing
            time_t now = time(nullptr);
            for (int daysBack = 0; daysBack < 7 && count < maxSaves; daysBack++)
            {
                for (int h = 0; h < 24 && count < maxSaves; h++)
                {
                    for (int m = 0; m < 60 && count < maxSaves; m += 5)
                    {
                        for (int s = 0; s < 60 && count < maxSaves; s += 5)
                        {
                            time_t checkTime = now - (daysBack * 86400) - (23 - h) * 3600 - (59 - m) * 60 - (59 - s);
                            struct tm *timeinfo = localtime(&checkTime);

                            char dateStr[20];
                            strftime(dateStr, sizeof(dateStr), "%Y%m%d_%H%M%S", timeinfo);
                            string saveID = playerID + "_" + string(dateStr);
                            string filename = dirs[d] + saveID + ".sav";

                            ifstream file(filename);
                            if (file.is_open())
                            {
                                string header;
                                getline(file, header);
                                if (header == "XONIX_SAVE_V1")
                                {
                                    bool duplicate = false;
                                    for (int i = 0; i < count; i++)
                                    {
                                        if (saveIDs[i] == saveID)
                                        {
                                            duplicate = true;
                                            break;
                                        }
                                    }
                                    if (!duplicate)
                                    {
                                        saveIDs[count++] = saveID;
                                    }
                                }
                                file.close();
                            }
                        }
                    }
                }
            }
        }

        return count;
    }

    /*
     * Function: displaySettingsScreen
     * Description: Displays the settings menu with:
     *              1. List of saved games for the logged-in user (with copy functionality)
     *              2. Background music on/off toggle
     */
    void displaySettingsScreen()
    {
        Font settingsFont;
        if (!settingsFont.loadFromFile("assets/fonts/arial.ttf"))
        {
            cout << "Failed to load settings font\n";
            return;
        }

        // Title
        Text title;
        title.setFont(font);
        title.setCharacterSize(50);
        title.setFillColor(Color::Yellow);
        title.setString("Settings");
        title.setPosition(300.f, 20.f);

        // Music toggle section
        Text musicLabel;
        musicLabel.setFont(settingsFont);
        musicLabel.setCharacterSize(24);
        musicLabel.setFillColor(Color::White);
        musicLabel.setString("Background Music:");
        musicLabel.setPosition(50.f, 100.f);

        Text musicStatus;
        musicStatus.setFont(settingsFont);
        musicStatus.setCharacterSize(24);
        musicStatus.setPosition(280.f, 100.f);

        // Saved games section
        Text savedGamesLabel;
        savedGamesLabel.setFont(settingsFont);
        savedGamesLabel.setCharacterSize(22);
        savedGamesLabel.setFillColor(Color::Cyan);
        savedGamesLabel.setString("Your Saved Games (Press Enter to copy selected):");
        savedGamesLabel.setPosition(50.f, 160.f);

        // Find saved games for this player
        const int MAX_SAVES = 20;
        string saveIDs[MAX_SAVES];
        int saveCount = findSavedGamesSimple(playerID, saveIDs, MAX_SAVES);

        // Create text objects for each save
        Text *saveTexts = new Text[MAX_SAVES];
        for (int i = 0; i < saveCount; i++)
        {
            saveTexts[i].setFont(settingsFont);
            saveTexts[i].setCharacterSize(18);
            saveTexts[i].setFillColor(Color::White);
            saveTexts[i].setString(to_string(i + 1) + ") " + saveIDs[i]);
            saveTexts[i].setPosition(70.f, 200.f + i * 30.f);
        }

        // No saves message
        Text noSavesText;
        noSavesText.setFont(settingsFont);
        noSavesText.setCharacterSize(20);
        noSavesText.setFillColor(Color(150, 150, 150));
        noSavesText.setString("No saved games found.");
        noSavesText.setPosition(70.f, 200.f);

        // Copy confirmation text
        Text copyConfirm;
        copyConfirm.setFont(settingsFont);
        copyConfirm.setCharacterSize(18);
        copyConfirm.setFillColor(Color::Green);
        copyConfirm.setPosition(50.f, 520.f);

        // Instructions
        Text instructions;
        instructions.setFont(settingsFont);
        instructions.setCharacterSize(16);
        instructions.setFillColor(Color::White);
        instructions.setString("M: Toggle Music | UP/DOWN: Select Save | ENTER: Copy Save ID | ESC: Return");
        instructions.setPosition(50.f, 560.f);

        int selectedSave = 0;
        bool showCopyConfirm = false;
        Clock copyConfirmClock;

        // Highlight first save if any
        if (saveCount > 0)
        {
            saveTexts[0].setFillColor(Color::Yellow);
        }

        while (window->isOpen())
        {
            // Update music status text
            if (isMusicEnabled())
            {
                musicStatus.setString("[ON]  - Press M to turn OFF");
                musicStatus.setFillColor(Color::Green);
            }
            else
            {
                musicStatus.setString("[OFF] - Press M to turn ON");
                musicStatus.setFillColor(Color::Red);
            }

            // Hide copy confirmation after 2 seconds
            if (showCopyConfirm && copyConfirmClock.getElapsedTime().asSeconds() > 2.0f)
            {
                showCopyConfirm = false;
            }

            Event evt;
            while (window->pollEvent(evt))
            {
                if (evt.type == Event::Closed)
                {
                    window->close();
                    delete[] saveTexts;
                    return;
                }

                if (evt.type == Event::KeyPressed)
                {
                    if (evt.key.code == Keyboard::Escape)
                    {
                        delete[] saveTexts;
                        return;
                    }
                    else if (evt.key.code == Keyboard::M)
                    {
                        // Toggle music
                        toggleBackgroundMusic();
                    }
                    else if (evt.key.code == Keyboard::Up && saveCount > 0)
                    {
                        saveTexts[selectedSave].setFillColor(Color::White);
                        selectedSave = (selectedSave - 1 + saveCount) % saveCount;
                        saveTexts[selectedSave].setFillColor(Color::Yellow);
                    }
                    else if (evt.key.code == Keyboard::Down && saveCount > 0)
                    {
                        saveTexts[selectedSave].setFillColor(Color::White);
                        selectedSave = (selectedSave + 1) % saveCount;
                        saveTexts[selectedSave].setFillColor(Color::Yellow);
                    }
                    else if (evt.key.code == Keyboard::Enter && saveCount > 0)
                    {
                        // Copy selected save ID to clipboard
                        Clipboard::setString(saveIDs[selectedSave]);
                        copyConfirm.setString("Copied to clipboard: " + saveIDs[selectedSave]);
                        showCopyConfirm = true;
                        copyConfirmClock.restart();
                    }
                }
            }

            // Draw
            window->clear(Color(30, 30, 30));
            window->draw(settingsSprite);
            window->draw(title);
            window->draw(musicLabel);
            window->draw(musicStatus);
            window->draw(savedGamesLabel);

            if (saveCount == 0)
            {
                window->draw(noSavesText);
            }
            else
            {
                for (int i = 0; i < saveCount; i++)
                {
                    window->draw(saveTexts[i]);
                }
            }

            if (showCopyConfirm)
            {
                window->draw(copyConfirm);
            }

            window->draw(instructions);
            window->display();
        }

        delete[] saveTexts;
    }

    int promptLevel(bool &selectedToPlay)
    {
        selectedToPlay = 0;
        // Prepare the two options
        const string labels[5] = {" Easy", " Medium", " Hard", " Expert", " Mastero"};
        Text texts[5];
        for (int i = 0; i < 5; i++)
        {
            texts[i].setFont(font);
            texts[i].setString(labels[i]);
            texts[i].setCharacterSize(28);
            texts[i].setFillColor(Color::White);
            // vertically center, spaced by 50px
            texts[i].setPosition(
                (window->getSize().x - texts[i].getLocalBounds().width) / 2.f,
                150.f + i * 50.f);
        }

        int highlighted = 0;
        texts[highlighted].setFillColor(Color::Yellow);

        // Loop until selection
        while (window->isOpen())
        {
            Event evt;
            while (window->pollEvent(evt))
            {
                if (evt.type == Event::Closed)
                {
                    window->close();
                    return 1;
                }
                if (evt.type == Event::KeyPressed)
                {
                    if (evt.key.code == Keyboard::Escape)
                    {
                        selectedToPlay = 0;
                        return 1;
                    }
                    if (evt.key.code == Keyboard::Up || evt.key.code == Keyboard::W)
                    {
                        texts[highlighted].setFillColor(Color::White);
                        highlighted = (highlighted + 1) % 5;
                        texts[highlighted].setFillColor(Color::Yellow);
                    }
                    else if (evt.key.code == Keyboard::Down || evt.key.code == Keyboard::S)
                    {
                        texts[highlighted].setFillColor(Color::White);
                        highlighted = (highlighted + 1) % 5;
                        texts[highlighted].setFillColor(Color::Yellow);
                    }
                    else if (evt.key.code == Keyboard::Enter)
                    {
                        selectedToPlay = 1;
                        return highlighted + 1;
                    }
                }
            }

            // Draw
            window->clear(Color(20, 20, 20));
            window->draw(gameTypeSprite);
            for (int i = 0; i < 5; ++i)
                window->draw(texts[i]);
            window->display();
        }

        return 1;
    }

    void displayLeaderboardScreen()
    {
        Leaderboard lb(&system);
        lb.makeLeaderboard();
        const string *ids = lb.getSortedHeap(); // Returns internal array, no need to delete
        int count = lb.getSize();

        Font lfont;
        if (!lfont.loadFromFile("assets/fonts/arial.ttf"))
        {
            cout << "Error loading font\n";
            return;
        }

        // Create entries for display (max 10)
        const int maxDisplay = 10;
        Text entries[maxDisplay];

        for (int i = 0; i < count && i < maxDisplay; ++i)
        {
            Player *p = system.returnPlayer(ids[i]);
            if (!p)
                continue;

            entries[i].setFont(lfont);
            entries[i].setCharacterSize(22);

            // Color coding: Gold for #1, Silver for #2, Bronze for #3
            if (i == 0)
            {
                entries[i].setFillColor(Color(255, 215, 0)); // Gold
            }
            else if (i == 1)
            {
                entries[i].setFillColor(Color(192, 192, 192)); // Silver
            }
            else if (i == 2)
            {
                entries[i].setFillColor(Color(205, 127, 50)); // Bronze
            }
            else
            {
                entries[i].setFillColor(Color::White);
            }

            // Get difficulty level name
            string levelName = lb.getLevelName(p->highScoreLevel);

            // Display format: Rank. Username (ID: X) - Score: Y [Level]
            entries[i].setString(
                to_string(i + 1) + ". " +
                p->username + " (ID: " + p->ID + ") - Score: " +
                to_string(p->highScore) + " [" + levelName + "]");
            delete p;
        }

        const float startY = 160.f;
        const float spacing = 40.f;
        for (int i = 0; i < count && i < maxDisplay; ++i)
        {
            entries[i].setPosition(50.f, startY + i * spacing);
        }

        // Title
        Text title;
        title.setFont(font);
        title.setCharacterSize(60);
        title.setFillColor(Color::Yellow);
        title.setString("Leaderboard");
        title.setPosition(50.f, 30.f);

        // Subtitle showing top 10
        Text subtitle;
        subtitle.setFont(font);
        subtitle.setCharacterSize(22);
        subtitle.setFillColor(Color::Cyan);
        subtitle.setString("Top 10 Players (by High Score)");
        subtitle.setPosition(50.f, 110.f);

        // No players message
        Text noPlayersText;
        noPlayersText.setFont(lfont);
        noPlayersText.setCharacterSize(24);
        noPlayersText.setFillColor(Color::White);
        noPlayersText.setString("No players on leaderboard yet!");
        noPlayersText.setPosition(200.f, 300.f);

        // Instructions
        Text instrText;
        instrText.setFont(lfont);
        instrText.setCharacterSize(16);
        instrText.setFillColor(Color::White);
        instrText.setString("Press Esc to return");
        instrText.setPosition(300.f, 560.f);

        while (window->isOpen())
        {
            Event evt;
            while (window->pollEvent(evt))
            {
                if (evt.type == Event::Closed)
                {
                    window->close();
                    return;
                }
                if (evt.type == Event::KeyPressed &&
                    evt.key.code == Keyboard::Escape)
                {
                    return;
                }
            }

            window->clear(Color(20, 20, 20));
            window->draw(leaderboardSprite);
            window->draw(title);
            window->draw(subtitle);

            if (count == 0)
            {
                window->draw(noPlayersText);
            }
            else
            {
                for (int i = 0; i < count && i < maxDisplay; ++i)
                {
                    window->draw(entries[i]);
                }
            }

            window->draw(instrText);
            window->display();
        }
    }

    void execute(const string &label)
    {
        int themeID;
        int playMode;
        int newSaveChoice;
        string winner;
        string playerID = this->playerID;
        string opponent;
        bool selectedToPlay = 0;
        bool madeSave = 0;
        int score;
        int levelSelected;
        string saveIDToLoad = ""; // Save ID for loading saved games
        string savedGameID = "";  // Save ID generated when saving
        string bgImagePath = "";  // Background image path from theme

        if (label == "Play Game")
        {

            cout << "Starting game...\n";

            playMode = promptPlayMode(selectedToPlay);
            if (selectedToPlay)
            {
                cout << "Selected mode: " << playMode << endl;
                if (playMode == 1)
                {
                    // call for game with one player
                    // call for gameover screen showing score
                    SinglePlayer p;
                    newSaveChoice = promptPlayNeworSave(selectedToPlay);
                    if (!selectedToPlay)
                    {
                        return;
                    }
                    if (newSaveChoice == 1)
                    {
                        // New Game - use player's saved theme from Inventory
                        themeID = getPlayerTheme(); // Get saved theme preference
                        Color theme = gettheme(themeID);
                        bgImagePath = getThemeImagePath(themeID); // Get background image path

                        // Show theme confirmation
                        AVLNode *selectedTheme = themeInventory.searchThemeByID(themeID);
                        if (selectedTheme)
                        {
                            cout << "Using theme: " << selectedTheme->name << endl;
                        }

                        levelSelected = promptLevel(selectedToPlay);
                        if (!selectedToPlay)
                        {
                            return;
                        }

                        // Pass background image path to game
                        score = p.run(theme, playerID, madeSave, 0, levelSelected, *window, "", savedGameID, bgImagePath);
                        window->setSize(Vector2u(800, 600));
                        View view(FloatRect(0, 0, 800, 600));
                        window->setView(view);
                    }
                    else
                    {
                        // Load Saved Game - prompt for Save ID
                        saveIDToLoad = promptSaveID(selectedToPlay);
                        if (!selectedToPlay || saveIDToLoad.empty())
                        {
                            return;
                        }

                        // Use player's saved theme for loaded games too
                        themeID = getPlayerTheme();
                        bgImagePath = getThemeImagePath(themeID);

                        score = p.run(Color::Green, playerID, madeSave, 1, 1, *window, saveIDToLoad, savedGameID, bgImagePath);
                        window->setSize(Vector2u(800, 600));
                        View view(FloatRect(0, 0, 800, 600));
                        window->setView(view);
                    }

                    if (!madeSave)
                    {
                        displayGameOverScreen(score);
                    }
                    // Save confirmation is now shown directly from pause menu
                }
                else if (playMode == 2)
                {
                    Multiplayer mp;
                    // Use player's saved theme from Inventory
                    themeID = getPlayerTheme();
                    Color theme = gettheme(themeID);
                    bgImagePath = getThemeImagePath(themeID);

                    opponent = promptOpponentID(selectedToPlay);
                    // Check if valid opponent was selected
                    if (!selectedToPlay || opponent.empty())
                    {
                        return; // User cancelled or no valid opponent
                    }
                    // Pass background image path to multiplayer
                    winner = mp.run(theme, playerID, opponent, *window, bgImagePath);
                    window->setSize(Vector2u(800, 600));
                    View view(FloatRect(0, 0, 800, 600));
                    window->setView(view);
                }
            }
        }
        else if (label == "Leaderboard")
        {
            cout << "Displaying leaderboard...\n";
            displayLeaderboardScreen();
        }
        else if (label == "Player Profile")
        {
            cout << "Opening player profile...\n";
            displayPlayerProfile();
        }
        else if (label == "Inventory")
        {
            cout << "Opening inventory...\n";
            displayInventoryScreen();
        }
        else if (label == "Friend List")
        {
            FriendManagement f(*window, playerID);
            f.run();
            cout << "Accessing friend list...\n";
        }
        else if (label == "Match History")
        {
            cout << "Showing match history...\n";
            displayMatchHistory();
        }
        else if (label == "Settings")
        {
            cout << "Opening settings...\n";
            displaySettingsScreen();
        }
        else if (label == "Logout")
        {
            cout << "Logging out...\n";
            shouldLogout = true; // Return to login screen
        }
        else if (label == "Exit")
        {
            cout << "Exiting application...\n";
            shouldExit = true;
            window->close(); // Close the application
        }
    }

    bool isStrongPassword(string &p, string &msg)
    {
        //  length between 8 and 15
        msg = "Password should be beteween(8-15), alphanumeric and a special character.";
        while (!p.empty() && p.back() == ' ')
            p.pop_back();
        int len = static_cast<int>(p.length());
        if (len < 8 || len > 15)
            return false;

        if (p[len - 1] == ' ')
        {
            len--;
        }
        //  no spaces allowed
        for (int i = 0; i < len; ++i)
        {
            if (p[i] == ' ')
                return false;
        }

        //  at least one digit, one letter, one special character
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

            if (hasDigit && hasLetter && hasSpecial)
                break;
        }
        bool check = hasDigit && hasLetter && hasSpecial;
        if (check)
        {
            msg = "";
        }
        return check;
    }

    void render()
    {
        window->clear(Color(20, 20, 20));
        window->draw(backgroundSprite);
        for (int i = 0; i < 9; ++i)
        {
            window->draw(options[i]);
        }
        window->draw(welcomeMsg);

        window->display();
    }
};
