// Xonix Game
// Single player Xonix game with save/load functionality
// Uses custom TileList (Linked List) to track player path

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Inventory.h"
#include "System.h"
#include "Constants.h"

using namespace std;
using namespace sf;

// Node for tracking tiles in player's path
struct TileNode
{
    int row;
    int col;
    int state;
    TileNode *next;

    TileNode(int r = 0, int c = 0, int s = 0);
};

// Custom Linked List for storing tile positions
class TileList
{
public:
    TileNode *head;

    TileList();
    ~TileList();

    void insert(int row, int col, int state);
    void clear();
    int getSize() const;
    bool isEmpty() const;
};

// Stores complete game state for save/load feature
struct GameState
{
    char saveID[50];
    char timestamp[30];
    char playerID[20];
    TileList tiles;  // Custom linked list for tile tracking

    // Game progress
    int score;
    int bonusCount;
    int powerUps;
    int nextPowerUpScore;

    // Player position and movement
    int playerX, playerY;
    int playerDX, playerDY;

    // Power-up state
    bool powerUpActive;
    float powerUpRemainingTime;

    // Level data
    int levelNo;
    int bgR, bgG, bgB, bgA;

    // Enemy data (max 10)
    int enemyCount;
    int enemyX[10], enemyY[10], enemyDX[10], enemyDY[10];

    int grid[M][N];

    GameState();

    void generateSaveID(const char *pID);
    void setTimestamp();
    bool serialize() const;     // Save to file
    bool deserialize(const string &saveIDToLoad);  // Load from file
};

/*
 * Class: SinglePlayer
 * Description: Main single-player game class with all game logic.
 */
class SinglePlayer
{
private:
    string playerId;
    string lastSaveID;
    Color backgroundColor;
    string backgroundImagePath;
    Texture backgroundTexture;
    Sprite backgroundSprite;
    bool useBackgroundImage;
    int score;
    System system;
    Player *p;
    int bonusCount;
    int powerUps;
    int nextPowerUpScore;
    bool powerUpUsed;
    Clock powerUpClock;
    bool powerUpActive;
    float powerUpRemainingTime;
    ThemeInventory TI;

    struct Enemy
    {
        int x, y, dx, dy;
        bool frozen;
        Enemy();
        void move(int grid[M][N], int ts, bool frozen, float speedMultiplier);
    };

    int grid[M][N];
    int ts;
    int x, y, dx, dy;
    bool Game;
    float timer;
    int enemyCount;
    Enemy a[10];
    int currentLevel;
    float delay;
    float enemySpeed;
    int levelNo;

    void drop(int y, int x);  // Flood fill algorithm to mark enemy territory
    void resetGame();
    int countCapturedTiles();
    void setLevel(int levelNo);

public:
    SinglePlayer();

    int run(Color bgColor, string id, bool &madeSave, bool playingSave, int level,
            RenderWindow &window, const string &saveIDToLoad = "", string &savedGameID = *(new string()),
            const string &bgImagePath = "");

    string saveGame();
    bool loadGame(const string &saveIDToLoad = "");
    bool loadGameLegacy();
    string getLastSaveID() const;

    void showSaveConfirmationScreen(RenderWindow &window, const string &savedID);
    void showPauseMenu(RenderWindow &window, bool &Game, bool &madeSave, string &savedID, bool &shouldReset);
};
