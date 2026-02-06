// Xonix Game
// Multiplayer Xonix - two players on same keyboard
// Player 1: Arrow keys, Player 2: WASD
// Power-ups freeze opponent temporarily

#include "../header/Multiplayer.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;

const int ts = 18;

Multiplayer::Enemy::Enemy(int startX, int startY)
{
    x = startX;
    y = startY;
    dx = 4 - rand() % 8;
    dy = 4 - rand() % 8;
    frozen = false;
}

void Multiplayer::Enemy::move(int grid[M][N])
{
    if (frozen)
        return;

    x += dx;
    if (grid[y / ts][x / ts] == 1)
    {
        dx = -dx;
        x += dx;
    }

    y += dy;
    if (grid[y / ts][x / ts] == 1)
    {
        dy = -dy;
        y += dy;
    }
}

// Constructor
Multiplayer::Multiplayer()
{
    score = 0;
    bonusCount = 0;
    powerUps = 0;
    nextPowerUpScore = 50;
    powerUpUsed = false;
    powerUpActive = false;
    powerUpRemainingTime = 0;
    player2Frozen = false;

    score2 = 0;
    bonusCount2 = 0;
    powerUps2 = 0;
    nextPowerUpScore2 = 50;
    powerUpUsed2 = false;
    powerUpActive2 = false;
    powerUpRemainingTime2 = 0;
    player1Frozen = false;

    useBackgroundImage = false;

    resetGame();
}

void Multiplayer::resetGame()
{
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = 0;

    // Set borders
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;

    score = 0;
    bonusCount = 0;
    powerUpActive = false;
    nextPowerUpScore = 50;

    score2 = 0;
    bonusCount2 = 0;
    powerUpActive2 = false;
    nextPowerUpScore2 = 50;
}

// Flood fill to mark enemy-reachable areas
void Multiplayer::drop(int y, int x)
{
    if (y < 0 || y >= M || x < 0 || x >= N)
        return;
    if (grid[y][x] != 0)
        return;

    grid[y][x] = -1;
    
    drop(y - 1, x);
    drop(y + 1, x);
    drop(y, x - 1);
    drop(y, x + 1);
}

/*
 * Method: countCapturedTiles
 * Description: Counts all tiles marked as -1 (enemy-reachable areas).
 * Returns: Number of captured tiles
 */
int Multiplayer::countCapturedTiles()
{
    int count = 0;
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (grid[i][j] == -1)
            {
                count++;
            }
        }
    }
    return count;
}

// Main game loop - returns winner's username or "Tie"
string Multiplayer::run(Color bgcolor, string id1, string id2, RenderWindow &window, const string &bgImagePath)
{
    srand(time(0));
    Player *p1 = system.returnPlayer(id1);
    Player *p2 = system.returnPlayer(id2);

    string player1Name = p1 ? p1->username : "Player 1";
    string player2Name = p2 ? p2->username : "Player 2";

    powerUps = p1->powerUps;
    powerUps2 = p2->powerUps;
    backgroundImagePath = bgImagePath;

    // Create the game window
    window.setSize(Vector2u(N * ts, M * ts));
    View view(FloatRect(0, 0, N * ts, M * ts));
    window.setView(view);
    window.setFramerateLimit(60);

    // Load background image if path provided
    useBackgroundImage = false;
    if (!bgImagePath.empty())
    {
        if (backgroundTexture.loadFromFile(bgImagePath))
        {
            // Set texture with smooth scaling
            backgroundTexture.setSmooth(true);
            backgroundSprite.setTexture(backgroundTexture, true); // resetRect = true
            backgroundSprite.setPosition(0, 0);

            // Calculate scale to fill entire game window
            float windowWidth = (float)(N * ts);
            float windowHeight = (float)(M * ts);
            float textureWidth = (float)backgroundTexture.getSize().x;
            float textureHeight = (float)backgroundTexture.getSize().y;

            float scaleX = windowWidth / textureWidth;
            float scaleY = windowHeight / textureHeight;
            backgroundSprite.setScale(scaleX, scaleY);

            useBackgroundImage = true;
            cout << "Background image loaded: " << bgImagePath << " (Scale: " << scaleX << "x" << scaleY << ")" << endl;
        }
        else
        {
            cout << "Failed to load background image: " << bgImagePath << ", using solid color." << endl;
        }
    }

    // Load textures
    Texture t1, t2, t3;
    t1.loadFromFile("assets/images/tiles.png");
    t2.loadFromFile("assets/images/gameover.png");
    t3.loadFromFile("assets/images/enemy.png");

    Font font;
    if (!font.loadFromFile("assets/fonts/arial.ttf"))
    {
        cout << "Error loading font. Using default font.\n";
    }

    // Create text objects for score and power-ups
    // Player 1 score - LEFT side of screen
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(16);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10, 5);
    scoreText.setStyle(Text::Bold);

    // Player 2 score - RIGHT side of screen
    Text scoreText2;
    scoreText2.setFont(font);
    scoreText2.setCharacterSize(16);
    scoreText2.setFillColor(Color::White);
    scoreText2.setPosition(N * ts - 280, 5); // Right side
    scoreText2.setStyle(Text::Bold);

    // Shared game timer - CENTER of screen
    Text timerText;
    timerText.setFont(font);
    timerText.setCharacterSize(18);
    timerText.setFillColor(Color::Yellow);
    timerText.setPosition((N * ts) / 2 - 40, 5);
    timerText.setStyle(Text::Bold);

    Text instructionsText;
    instructionsText.setFont(font);
    instructionsText.setCharacterSize(12);
    instructionsText.setFillColor(Color::White);
    instructionsText.setPosition(10, M * ts - 25);
    instructionsText.setString("P1: Arrows, SPACE=PowerUp | P2: WASD, F=PowerUp | P/Esc: Pause");

    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sGameover.setPosition(100, 100);
    sEnemy.setOrigin(20, 20);

    // Game variables
    int enemyCount = 4;
    Enemy a[4] = {
        Enemy(2 * ts, 2 * ts),
        Enemy(2 * ts, (M - 3) * ts),
        Enemy((N - 3) * ts, 2 * ts),
        Enemy((N - 3) * ts, (M - 3) * ts)};

    bool Game = true;
    int x = 0, y = 0, dx = 0, dy = 0;
    int x2 = N - 1, y2 = M - 1, dx2 = 0, dy2 = 0;
    float timer2 = 0, delay2 = 0.07f;
    bool isPlayer1Alive = true;
    bool isPlayer2Alive = true;
    float timer = 0, delay = 0.07f;
    Clock clock;
    Clock gameTimer; // Shared game timer
    float gameTime = 0;

    // Track if players are currently constructing (on trail, not on wall)
    bool p1Constructing = false;
    bool p2Constructing = false;

    resetGame();

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;
        timer2 += time;

        if (powerUpActive)
        {
            powerUpRemainingTime = 3.0f - powerUpClock.getElapsedTime().asSeconds();
            player2Frozen = true; // Freeze Player 2
            if (powerUpRemainingTime <= 0)
            {
                powerUpActive = false;
                powerUpRemainingTime = 0;
                player2Frozen = false; // Unfreeze Player 2
            }
        }
        else
        {
            player2Frozen = false;
        }

        if (powerUpActive2)
        {
            powerUpRemainingTime2 = 3.0f - powerUpClock2.getElapsedTime().asSeconds();
            player1Frozen = true; // Freeze Player 1
            if (powerUpRemainingTime2 <= 0)
            {
                powerUpActive2 = false;
                powerUpRemainingTime2 = 0;
                player1Frozen = false; // Unfreeze Player 1
            }
        }
        else
        {
            player1Frozen = false;
        }

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed)
            {
                /*
                 * Power-Up Activation Rules:
                 * - Player must be ALIVE to use power-up
                 * - Player must have power-ups in inventory (powerUps > 0)
                 * - No freeze can be currently active (prevents abuse)
                 * - Player 1: Space key | Player 2: F key
                 */

                // Player 1 Power-Up: F key (left side player uses left side key)
                if (e.key.code == Keyboard::F &&
                    isPlayer1Alive && // Must be alive
                    powerUps > 0 &&   // Must have power-ups
                    !powerUpActive && // P1's freeze not active
                    !powerUpActive2)
                { // P2's freeze not active (prevent overlap)
                    powerUps--;
                    powerUpUsed = true;
                    powerUpClock.restart();
                    powerUpActive = true;
                    powerUpRemainingTime = 3.0f;
                }

                // Player 2 Power-Up: SPACE key (right side player uses right side key)
                if (e.key.code == Keyboard::Space &&
                    isPlayer2Alive &&  // Must be alive
                    powerUps2 > 0 &&   // Must have power-ups
                    !powerUpActive2 && // P2's freeze not active
                    !powerUpActive)
                { // P1's freeze not active (prevent overlap)
                    powerUps2--;
                    powerUpUsed2 = true;
                    powerUpClock2.restart();
                    powerUpActive2 = true;
                    powerUpRemainingTime2 = 3.0f;
                }

                if (e.key.code == Keyboard::P || e.key.code == Keyboard::Escape)
                {
                    bool shouldReset = false;
                    showPauseMenu(window, Game, shouldReset);

                    // Handle reset if selected from pause menu
                    if (shouldReset)
                    {
                        resetGame();
                        x = 0;
                        y = 0;
                        x2 = N - 1;
                        y2 = M - 1;
                        dx = 0;
                        dy = 0;
                        dx2 = 0;
                        dy2 = 0;
                        isPlayer1Alive = true;
                        isPlayer2Alive = true;
                        Game = true;
                    }
                    // Handle End Game from pause menu - exit immediately
                    else if (!Game)
                    {
                        // Update match history and exit
                        bool p1Wins = (score > score2);
                        bool p2Wins = (score2 > score);
                        p1->addMatchUpdate(p2->username, p1Wins, score, powerUps);
                        p2->addMatchUpdate(p1->username, p2Wins, score2, powerUps2);

                        string u1 = p1->username, u2 = p2->username;
                        delete p1;
                        delete p2;

                        // Show game over screen
                        displayGameOverScreen2(&window, score, score2, u1, u2);

                        if (score > score2)
                            return u1;
                        else if (score2 > score)
                            return u2;
                        else
                            return "Tie";
                    }
                }
            }
        }

        // Player 1 controls - WASD (left side player uses left side keys)
        if (Keyboard::isKeyPressed(Keyboard::A))
        {
            dx = -1;
            dy = 0;
        } // Left
        if (Keyboard::isKeyPressed(Keyboard::D))
        {
            dx = 1;
            dy = 0;
        } // Right
        if (Keyboard::isKeyPressed(Keyboard::W))
        {
            dx = 0;
            dy = -1;
        } // Up
        if (Keyboard::isKeyPressed(Keyboard::S))
        {
            dx = 0;
            dy = 1;
        } // Down

        // Player 2 controls - Arrow keys (right side player uses right side keys)
        if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            dx2 = -1;
            dy2 = 0;
        } // Left
        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            dx2 = 1;
            dy2 = 0;
        } // Right
        if (Keyboard::isKeyPressed(Keyboard::Up))
        {
            dx2 = 0;
            dy2 = -1;
        } // Up
        if (Keyboard::isKeyPressed(Keyboard::Down))
        {
            dx2 = 0;
            dy2 = 1;
        } // Down

        if (!Game)
            continue;

        // Update shared game timer
        gameTime += time;

        // Player 1 movement
        if (timer > delay && isPlayer1Alive && !player1Frozen)
        {
            x += dx;
            y += dy;

            if (x < 0)
                x = 0;
            if (x > N - 1)
                x = N - 1;
            if (y < 0)
                y = 0;
            if (y > M - 1)
                y = M - 1;

            // Player 1 hits their own trail
            if (grid[y][x] == 2)
                isPlayer1Alive = false;
            // Player 1 hits Player 2's constructing trail - Player 1 dies
            if (grid[y][x] == 3)
                isPlayer1Alive = false;

            // Check if Player 1 is on wall (safe) or constructing
            p1Constructing = (grid[y][x] != 1);

            // Mark new trail if on empty space
            if (grid[y][x] == 0)
                grid[y][x] = 2;
            timer = 0;
        }
        else if (timer > delay && isPlayer1Alive && player1Frozen)
        {
            timer = 0; // Consume timer even when frozen
        }

        // Player 2 movement
        if (timer2 > delay2 && isPlayer2Alive && !player2Frozen)
        {
            x2 += dx2;
            y2 += dy2;

            if (x2 < 0)
                x2 = 0;
            if (x2 > N - 1)
                x2 = N - 1;
            if (y2 < 0)
                y2 = 0;
            if (y2 > M - 1)
                y2 = M - 1;

            // Player 2 hits their own trail
            if (grid[y2][x2] == 3)
                isPlayer2Alive = false;
            // Player 2 hits Player 1's constructing trail - Player 2 dies
            if (grid[y2][x2] == 2)
                isPlayer2Alive = false;

            // Check if Player 2 is on wall (safe) or constructing
            p2Constructing = (grid[y2][x2] != 1);

            // Mark new trail if on empty space
            if (grid[y2][x2] == 0)
                grid[y2][x2] = 3;
            timer2 = 0;
        }
        else if (timer2 > delay2 && isPlayer2Alive && player2Frozen)
        {
            timer2 = 0; // Consume timer even when frozen
        }

        /*
         * Player-to-Player Collision Rules:
         * 1. If both players collide while BOTH are constructing (moving) -> BOTH die
         * 2. If P1 touches P2's constructing trail -> P1 dies (handled above)
         * 3. If P2 touches P1's constructing trail -> P2 dies (handled above)
         * 4. If P1 is constructing and collides with P2 (who is not constructing) -> P1 dies
         * 5. If P2 is constructing and collides with P1 (who is not constructing) -> P2 dies
         */
        if (isPlayer1Alive && isPlayer2Alive && x == x2 && y == y2)
        {
            // Players are on the same tile - collision!
            if (p1Constructing && p2Constructing)
            {
                // Both constructing - both die
                isPlayer1Alive = false;
                isPlayer2Alive = false;
            }
            else if (p1Constructing && !p2Constructing)
            {
                // P1 constructing, P2 safe - P1 dies
                isPlayer1Alive = false;
            }
            else if (!p1Constructing && p2Constructing)
            {
                // P2 constructing, P1 safe - P2 dies
                isPlayer2Alive = false;
            }
            // If neither is constructing (both on wall), no collision death
        }

        // Enemy movement (same as single-player)
        for (int i = 0; i < enemyCount; i++)
        {
            if (!powerUpActive && !powerUpActive2)
            {
                a[i].frozen = false;
                a[i].move(grid);
            }
            else
            {
                a[i].frozen = true;
            }
        }

        // Check if both players are dead (before area-filling)
        if (!isPlayer1Alive && !isPlayer2Alive)
        {
            Game = false;
        }

        // Player 1 reaches safe wall - fill area (same as single-player)
        if (grid[y][x] == 1 && isPlayer1Alive)
        {
            dx = dy = 0;

            // Flood fill algorithm to determine enclosed areas
            // Step 1: Mark all areas reachable by enemies with -1
            for (int i = 0; i < enemyCount; i++)
                drop(a[i].y / ts, a[i].x / ts);

            int captured = 0;

            // Step 2: Convert ONLY Player 1's trail to walls and fill enclosed areas
            //         DO NOT touch Player 2's trail (value 3)
            for (int i = 0; i < M; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    if (grid[i][j] == -1)
                    {
                        // Enemy-reachable area - reset to empty
                        grid[i][j] = 0;
                    }
                    else if (grid[i][j] == 2)
                    {
                        // Player 1 trail becomes wall - COUNT THIS
                        grid[i][j] = 1;
                        captured++;
                    }
                    else if (grid[i][j] == 0)
                    {
                        // Enclosed empty area - fill with walls but DON'T count
                        grid[i][j] = 1;
                    }
                    // Leave Player 2's trail (value 3) untouched!
                }
            }

            /*
             * Points System for Player 1:
             * - Each tile captured = 1 point
             * - Capturing >10 tiles in single move = x2 points
             * - After 3 bonus occurrences: threshold reduces to 5 tiles
             * - After 5 bonus occurrences: capturing >5 tiles = x4 points
             */
            int bonusThreshold = (bonusCount >= 3) ? 5 : 10;
            int multiplier = 1;

            if (captured > bonusThreshold)
            {
                if (bonusCount >= 5)
                {
                    multiplier = 4; // x4 after 5 bonus occurrences
                }
                else
                {
                    multiplier = 2; // x2 for bonus capture
                }
                bonusCount++; // Increment AFTER determining multiplier
            }

            score += captured * multiplier;

            /*
             * Power-up thresholds: 50, 70, 100, 130, 160, 190...
             */
            while (score >= nextPowerUpScore)
            {
                powerUps++;
                if (nextPowerUpScore == 50)
                {
                    nextPowerUpScore = 70;
                }
                else if (nextPowerUpScore == 70)
                {
                    nextPowerUpScore = 100;
                }
                else
                {
                    nextPowerUpScore += 30;
                }
            }
        }

        if (grid[y2][x2] == 1 && isPlayer2Alive)
        {
            dx2 = dy2 = 0;

            // Flood fill algorithm to determine enclosed areas
            // Step 1: Mark all areas reachable by enemies with -1
            for (int i = 0; i < enemyCount; i++)
                drop(a[i].y / ts, a[i].x / ts);

            int captured2 = 0;

            // Step 2: Convert ONLY Player 2's trail to walls and fill enclosed areas
            //         DO NOT touch Player 1's trail (value 2)
            for (int i = 0; i < M; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    if (grid[i][j] == -1)
                    {
                        // Enemy-reachable area - reset to empty
                        grid[i][j] = 0;
                    }
                    else if (grid[i][j] == 3)
                    {
                        // Player 2 trail becomes wall - COUNT THIS
                        grid[i][j] = 1;
                        captured2++;
                    }
                    else if (grid[i][j] == 0)
                    {
                        // Enclosed empty area - fill with walls but DON'T count
                        grid[i][j] = 1;
                    }
                    // Leave Player 1's trail (value 2) untouched!
                }
            }

            /*
             * Points System for Player 2:
             * - Each tile captured = 1 point
             * - Capturing >10 tiles in single move = x2 points
             * - After 3 bonus occurrences: threshold reduces to 5 tiles
             * - After 5 bonus occurrences: capturing >5 tiles = x4 points
             */
            int bonusThreshold2 = (bonusCount2 >= 3) ? 5 : 10;
            int multiplier = 1;

            if (captured2 > bonusThreshold2)
            {
                if (bonusCount2 >= 5)
                {
                    multiplier = 4; // x4 after 5 bonus occurrences
                }
                else
                {
                    multiplier = 2; // x2 for bonus capture
                }
                bonusCount2++; // Increment AFTER determining multiplier
            }

            score2 += captured2 * multiplier;

            /*
             * Power-up thresholds: 50, 70, 100, 130, 160, 190...
             */
            while (score2 >= nextPowerUpScore2)
            {
                powerUps2++;
                if (nextPowerUpScore2 == 50)
                {
                    nextPowerUpScore2 = 70;
                }
                else if (nextPowerUpScore2 == 70)
                {
                    nextPowerUpScore2 = 100;
                }
                else
                {
                    nextPowerUpScore2 += 30;
                }
            }
        }

        // Enemy collision check (identical to single-player: line 367-369)
        // Check AFTER area-filling to match single-player structure
        for (int i = 0; i < enemyCount; i++)
        {
            int enemyGridY = a[i].y / ts;
            int enemyGridX = a[i].x / ts;

            // Bounds check to prevent crashes
            if (enemyGridY >= 0 && enemyGridY < M && enemyGridX >= 0 && enemyGridX < N)
            {
                // Enemy on Player 1's trail (value 2) - Player 1 dies
                if (grid[enemyGridY][enemyGridX] == 2)
                {
                    isPlayer1Alive = false;
                }
                // Enemy on Player 2's trail (value 3) - Player 2 dies
                if (grid[enemyGridY][enemyGridX] == 3)
                {
                    isPlayer2Alive = false;
                }
            }
        }

        /////////draw//////////
        // Use background image if available, otherwise solid color
        if (useBackgroundImage)
        {
            window.clear();
            window.draw(backgroundSprite);
        }
        else
        {
            window.clear(bgcolor);
        }

        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (grid[i][j] == 0)
                    continue;

                if (grid[i][j] == 1)
                    sTile.setTextureRect(IntRect(0, 0, ts, ts));
                if (grid[i][j] == 2)
                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
                if (grid[i][j] == 3)
                    sTile.setTextureRect(IntRect(36, 0, ts, ts));

                // Blue tint for tiles when any freeze power-up is active
                if (powerUpActive || powerUpActive2)
                {
                    sTile.setColor(Color(150, 150, 255, 255)); // Blue tint for frozen effect
                }
                else
                {
                    sTile.setColor(Color::White); // Normal color
                }

                sTile.setPosition(j * ts, i * ts);
                window.draw(sTile);
            }
        }

        if (isPlayer1Alive)
        {
            sTile.setTextureRect(IntRect(36, 0, ts, ts));
            sTile.setPosition(x * ts, y * ts);
            // Tint player sprite if frozen
            if (player1Frozen)
            {
                sTile.setColor(Color(100, 100, 255, 200)); // Blue tint for frozen
            }
            else
            {
                sTile.setColor(Color::White); // Normal color
            }
            window.draw(sTile);
        }

        if (isPlayer2Alive)
        {
            sTile.setTextureRect(IntRect(72, 0, ts, ts));
            sTile.setPosition(x2 * ts, y2 * ts);
            // Tint player sprite if frozen
            if (player2Frozen)
            {
                sTile.setColor(Color(100, 100, 255, 200)); // Blue tint for frozen
            }
            else
            {
                sTile.setColor(Color::White); // Normal color
            }
            window.draw(sTile);
        }

        sEnemy.rotate(10);
        for (int i = 0; i < enemyCount; i++)
        {
            if (a[i].frozen)
            {
                sEnemy.setColor(Color(100, 100, 255, 200)); // Blue tint for frozen enemies
            }
            else
            {
                sEnemy.setColor(Color(255, 255, 255, 255)); // Normal color
                sEnemy.rotate(10);
            }
            sEnemy.setPosition(a[i].x, a[i].y);
            window.draw(sEnemy);
        }

        // Blue overlay effect when either player's power-up (freeze) is active
        if (powerUpActive || powerUpActive2)
        {
            RectangleShape blueEffect(Vector2f(N * ts, M * ts));
            blueEffect.setFillColor(Color(0, 100, 255, 50)); // Semi-transparent blue overlay
            window.draw(blueEffect);
        }

        string player1Status = !isPlayer1Alive ? " [DEAD]" : (player1Frozen ? " [FROZEN]" : "");
        string player2Status = !isPlayer2Alive ? " [DEAD]" : (player2Frozen ? " [FROZEN]" : "");

        // Player 1 UI - LEFT side
        scoreText.setString(player1Name + ": " + to_string(score) + " | PU:" + to_string(powerUps) +
                            (powerUpActive ? " [!]" : "") + player1Status);

        // Player 2 UI - RIGHT side
        scoreText2.setString(player2Name + ": " + to_string(score2) + " | PU:" + to_string(powerUps2) +
                             (powerUpActive2 ? " [!]" : "") + player2Status);

        // Shared game timer - CENTER
        int minutes = (int)gameTime / 60;
        int seconds = (int)gameTime % 60;
        string timerStr = "Time: " + to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds);
        timerText.setString(timerStr);

        window.draw(scoreText);
        window.draw(scoreText2);
        window.draw(timerText);
        window.draw(instructionsText);

        window.display();

        // Check if game has ended (either both players dead, or ended via pause menu)
        if (!Game || (!isPlayer1Alive && !isPlayer2Alive))
        {
            /*
             * Save match history for BOTH players:
             * - Each player gets their own record showing opponent name, win/lose, and score
             * - Player 1's history shows: "vs Player2Name - WIN/LOSE (Score: X)"
             * - Player 2's history shows: "vs Player1Name - WIN/LOSE (Score: X)"
             */

            // Determine winner based on score
            bool p1Wins = (score > score2);
            bool p2Wins = (score2 > score);
            // If scores are equal, it's a tie (both get "LOSE" or we could add TIE)

            // Update Player 1's match history
            p1->addMatchUpdate(p2->username, p1Wins, score, powerUps);

            // Update Player 2's match history
            p2->addMatchUpdate(p1->username, p2Wins, score2, powerUps2);

            string u1 = p1->username, u2 = p2->username;
            delete p1;
            delete p2;

            // Show game over screen with both scores
            displayGameOverScreen2(&window, score, score2, u1, u2);

            // Return winner's name
            if (score > score2)
            {
                return u1;
            }
            else if (score2 > score)
            {
                return u2;
            }
            else
            {
                return "Tie";
            }
        }
    }

    // Return empty string if loop exits normally (shouldn't happen)
    return "";
}

/*
 * Method: showPauseMenu
 * Description: Displays the pause menu with options to resume, reset, or end the game.
 * Parameters:
 *   window - Reference to the game window
 *   Game - Reference to game state flag
 *   shouldReset - Output flag indicating if game should be reset
 */
void Multiplayer::showPauseMenu(RenderWindow &window, bool &Game, bool &shouldReset)
{
    const int optionCount = 3;
    const string labels[optionCount] = {
        " Resume Game",
        " Reset Game",
        " End Game"};
    Font font;
    font.loadFromFile("assets/fonts/arial.ttf");

    // Title text
    Text titleText;
    titleText.setFont(font);
    titleText.setString("PAUSED");
    titleText.setCharacterSize(40);
    titleText.setFillColor(Color::Cyan);
    titleText.setPosition(100.f, 80.f);

    Text texts[optionCount];
    for (int i = 0; i < optionCount; ++i)
    {
        texts[i].setFont(font);
        texts[i].setString(labels[i]);
        texts[i].setCharacterSize(30);
        texts[i].setFillColor(i == 0 ? Color::Yellow : Color::White);
        texts[i].setPosition(100.f, 150.f + i * 50.f);
    }

    int selected = 0;
    shouldReset = false;

    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
            {
                window.close();
                return;
            }

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Up)
                {
                    texts[selected].setFillColor(Color::White);
                    selected = (selected - 1 + optionCount) % optionCount;
                    texts[selected].setFillColor(Color::Yellow);
                }
                else if (e.key.code == Keyboard::Down)
                {
                    texts[selected].setFillColor(Color::White);
                    selected = (selected + 1) % optionCount;
                    texts[selected].setFillColor(Color::Yellow);
                }
                else if (e.key.code == Keyboard::Enter)
                {
                    if (selected == 0)
                    {
                        // Resume Game
                        return;
                    }
                    else if (selected == 1)
                    {
                        // Reset Game
                        shouldReset = true;
                        return;
                    }
                    else if (selected == 2)
                    {
                        // End Game
                        Game = false;
                        return;
                    }
                }
                else if (e.key.code == Keyboard::Escape || e.key.code == Keyboard::P)
                {
                    // ESC or P in pause menu = resume
                    return;
                }
            }
        }

        // draw menu
        window.clear(Color(30, 30, 30));
        window.draw(titleText);
        for (int i = 0; i < optionCount; ++i)
            window.draw(texts[i]);
        window.display();
    }

    return;
}

/*
 * Method: displayGameOverScreen2
 * Description: Displays the game over screen showing winner and scores.
 * Parameters:
 *   window - Pointer to the game window
 *   score - Player 1's final score
 *   score2 - Player 2's final score
 *   u1 - Player 1's username
 *   u2 - Player 2's username
 */
void Multiplayer::displayGameOverScreen2(RenderWindow *window, int score, int score2, string u1, string u2)
{
    // Prepare texts
    Font font;
    font.loadFromFile("assets/fonts/arial.ttf");
    Text title, scoreText, prompt;
    title.setFont(font);
    if (score == score2)
    {
        title.setString("Game Tie");
    }
    else if (score > score2)
    {
        title.setString("Winner " + u1);
    }
    else
    {
        title.setString("Winner " + u2);
    }
    title.setCharacterSize(48);
    title.setFillColor(Color::Red);
    // center at y = 150
    {
        FloatRect bounds = title.getLocalBounds();
        title.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        title.setPosition(window->getSize().x / 2.f, 150.f);
    }

    scoreText.setFont(font);
    scoreText.setString(u1 + ": " + to_string(score) + "   |   " + u2 + ": " + to_string(score2));
    scoreText.setCharacterSize(32);
    scoreText.setFillColor(Color::White);
    {
        FloatRect bounds = scoreText.getLocalBounds();
        scoreText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        scoreText.setPosition(window->getSize().x / 2.f, 250.f);
    }

    prompt.setFont(font);
    prompt.setString("Press Esc to return to Main Menu");
    prompt.setCharacterSize(24);
    prompt.setFillColor(Color::Yellow);
    {
        FloatRect bounds = prompt.getLocalBounds();
        prompt.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        prompt.setPosition(window->getSize().x / 2.f, 350.f);
    }

    // Loop until Esc or window close
    while (window->isOpen())
    {
        Event evt;
        while (window->pollEvent(evt))
        {
            if (evt.type == sf::Event::Closed)
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
        window->draw(title);
        window->draw(scoreText);
        window->draw(prompt);
        window->display();
    }
}
