// Xonix Game
// Multiplayer Xonix - two players on same keyboard (Arrows + WASD)
// Power-ups freeze opponent, separate scoring

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "System.h"
#include "Constants.h"
#include "Inventory.h"

using namespace std;
using namespace sf;

extern const int ts;

class Multiplayer
{
private:
    System system;
    int grid[M][N];

    // Player 1 state
    int score;
    int bonusCount;
    int powerUps;
    int nextPowerUpScore;
    bool powerUpUsed;
    Clock powerUpClock;
    bool powerUpActive;
    float powerUpRemainingTime;
    bool player2Frozen;

    // Player 2 state
    int score2;
    int bonusCount2;
    int powerUps2;
    int nextPowerUpScore2;
    bool powerUpUsed2;
    Clock powerUpClock2;
    bool powerUpActive2;
    float powerUpRemainingTime2;
    bool player1Frozen;

    Texture backgroundTexture;
    Sprite backgroundSprite;
    bool useBackgroundImage;
    string backgroundImagePath;

    struct Enemy
    {
        int x, y, dx, dy;
        bool frozen;
        Enemy(int startX, int startY);
        void move(int grid[M][N]);
    };

    void resetGame();
    void drop(int y, int x);  // Flood fill to mark enemy territory
    int countCapturedTiles();

public:
    Multiplayer();

    // Returns winner's username or "Tie"
    string run(Color bgcolor, string id1, string id2, RenderWindow &window, const string &bgImagePath = "");

    void showPauseMenu(RenderWindow &window, bool &Game, bool &shouldReset);
    void displayGameOverScreen2(RenderWindow *window, int score, int score2, string u1, string u2);
};
