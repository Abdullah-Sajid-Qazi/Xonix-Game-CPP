// Xonix Game
// Single player game implementation with save/load using custom TileList (Linked List)

#include "../header/Game.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>

using namespace std;
using namespace sf;

TileNode::TileNode(int r, int c, int s) : row(r), col(c), state(s), next(nullptr) {}

TileList::TileList() : head(nullptr) {}

TileList::~TileList()
{
    clear();
}

void TileList::insert(int row, int col, int state)
{
    TileNode *newNode = new TileNode(row, col, state);
    newNode->next = head;
    head = newNode;
}

void TileList::clear()
{
    while (head)
    {
        TileNode *temp = head;
        head = head->next;
        delete temp;
    }
}

int TileList::getSize() const
{
    int count = 0;
    TileNode *curr = head;
    while (curr)
    {
        count++;
        curr = curr->next;
    }
    return count;
}

bool TileList::isEmpty() const
{
    return head == nullptr;
}

GameState::GameState()
{
    saveID[0] = '\0';
    timestamp[0] = '\0';
    playerID[0] = '\0';
    score = 0;
    bonusCount = 0;
    powerUps = 0;
    nextPowerUpScore = 50;
    playerX = playerY = 0;
    playerDX = playerDY = 0;
    powerUpActive = false;
    powerUpRemainingTime = 0;
    levelNo = 1;
    bgR = bgG = bgB = 0;
    bgA = 255;
    enemyCount = 4;
    for (int i = 0; i < 10; i++)
    {
        enemyX[i] = enemyY[i] = 0;
        enemyDX[i] = enemyDY[i] = 0;
    }
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            grid[i][j] = 0;
        }
    }
}

// Generate unique save ID: playerID_YYYYMMDD_HHMMSS
void GameState::generateSaveID(const char *pID)
{
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);

    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y%m%d_%H%M%S", timeinfo);

    int i = 0;
    const char *p = pID;
    while (*p && i < 45)
        saveID[i++] = *p++;
    
    saveID[i++] = '_';
    p = timeStr;
    while (*p && i < 49)
        saveID[i++] = *p++;
    saveID[i] = '\0';
}

void GameState::setTimestamp()
{
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
}

// Serialize game state to file
bool GameState::serialize() const
{
    string filename = "data/saves/" + string(saveID) + ".sav";

    ofstream file(filename);
    if (!file.is_open())
    {
        filename = string(saveID) + ".sav";
        file.open(filename);
        if (!file.is_open())
        {
            cerr << "Error: Could not create save file: " << filename << endl;
            return false;
        }
    }

    // Write save data
    file << "XONIX_SAVE_V1" << endl;
    file << saveID << endl;
    file << timestamp << endl;
    file << playerID << endl;
    file << score << endl;
    file << bonusCount << endl;
    file << powerUps << endl;
    file << nextPowerUpScore << endl;
    file << playerX << " " << playerY << endl;
    file << playerDX << " " << playerDY << endl;
    file << (powerUpActive ? 1 : 0) << endl;
    file << powerUpRemainingTime << endl;
    file << levelNo << endl;
    file << bgR << " " << bgG << " " << bgB << " " << bgA << endl;

    file << enemyCount << endl;
    for (int i = 0; i < enemyCount; i++)
        file << enemyX[i] << " " << enemyY[i] << " " << enemyDX[i] << " " << enemyDY[i] << endl;

    // Write grid
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            file << grid[i][j];
            if (j < N - 1)
                file << " ";
        }
        file << endl;
    }

    // Write tile linked list
    TileNode *curr = tiles.head;
    int tileCount = tiles.getSize();
    file << tileCount << endl;
    while (curr)
    {
        file << curr->row << " " << curr->col << " " << curr->state << endl;
        curr = curr->next;
    }

    file.close();

    // Update saves index file
    string indexFile = "data/saves/index_" + string(playerID) + ".txt";
    string existingSaves[100];
    int existingCount = 0;
    
    ifstream idxIn(indexFile);
    if (idxIn.is_open())
    {
        string line;
        while (getline(idxIn, line) && existingCount < 100)
        {
            if (!line.empty())
                existingSaves[existingCount++] = line;
        }
        idxIn.close();
    }

    // Add new save if not present
    bool found = false;
    for (int i = 0; i < existingCount; i++)
    {
        if (existingSaves[i] == string(saveID))
        {
            found = true;
            break;
        }
    }

    if (!found && existingCount < 100)
        existingSaves[existingCount++] = string(saveID);

    ofstream idxOut(indexFile);
    if (idxOut.is_open())
    {
        for (int i = 0; i < existingCount; i++)
            idxOut << existingSaves[i] << endl;
        idxOut.close();
    }

    cout << "Game saved! Save ID: " << saveID << endl;
    return true;
}

// Load game state from file
bool GameState::deserialize(const string &saveIDToLoad)
{
    string filename = "data/saves/" + saveIDToLoad + ".sav";
    ifstream file(filename);

    if (!file.is_open())
    {
        filename = saveIDToLoad + ".sav";
        file.open(filename);
        if (!file.is_open())
        {
            cerr << "Error: Could not open save file: " << saveIDToLoad << endl;
            return false;
        }
    }

    // Verify file format
    string header;
    getline(file, header);
    if (header != "XONIX_SAVE_V1")
    {
        cerr << "Error: Invalid save file format!" << endl;
        file.close();
        return false;
    }

    string line;
    getline(file, line);
    strncpy(saveID, line.c_str(), sizeof(saveID) - 1);
    saveID[sizeof(saveID) - 1] = '\0';

    getline(file, line);
    strncpy(timestamp, line.c_str(), sizeof(timestamp) - 1);
    timestamp[sizeof(timestamp) - 1] = '\0';

    getline(file, line);
    strncpy(playerID, line.c_str(), sizeof(playerID) - 1);
    playerID[sizeof(playerID) - 1] = '\0';

    file >> score >> bonusCount >> powerUps >> nextPowerUpScore;
    file >> playerX >> playerY >> playerDX >> playerDY;

    int activeFlag;
    file >> activeFlag >> powerUpRemainingTime;
    powerUpActive = (activeFlag != 0);

    file >> levelNo;
    file >> bgR >> bgG >> bgB >> bgA;

    file >> enemyCount;
    for (int i = 0; i < enemyCount; i++)
        file >> enemyX[i] >> enemyY[i] >> enemyDX[i] >> enemyDY[i];

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            file >> grid[i][j];

    // Read tile linked list (insert in reverse to maintain order)
    tiles.clear();
    int tileCount;
    file >> tileCount;

    if (tileCount > 0)
    {
        int *rows = new int[tileCount];
        int *cols = new int[tileCount];
        int *states = new int[tileCount];

        for (int i = 0; i < tileCount; i++)
            file >> rows[i] >> cols[i] >> states[i];

        for (int i = tileCount - 1; i >= 0; i--)
            tiles.insert(rows[i], cols[i], states[i]);

        delete[] rows;
        delete[] cols;
        delete[] states;
    }

    file.close();
    cout << "Game loaded from Save ID: " << saveID << endl;
    return true;
}

SinglePlayer::Enemy::Enemy()
{
    x = y = 300;
    dx = 4 - rand() % 8;
    dy = 4 - rand() % 8;
    frozen = false;
}

void SinglePlayer::Enemy::move(int grid[M][N], int ts, bool frozen, float speedMultiplier)
{
    if (frozen)
        return;

    float newX = x + dx * speedMultiplier;
    float newY = y + dy * speedMultiplier;

    if (newX < 0)
        newX = 0;
    if (newX > (N - 1) * ts)
        newX = (N - 1) * ts;
    if (newY < 0)
        newY = 0;
    if (newY > (M - 1) * ts)
        newY = (M - 1) * ts;

    int rowOld = int(y) / ts;
    int colNewX = int(newX) / ts;
    if (grid[rowOld][colNewX] == 1)
    {
        dx = -dx;
    }
    else
    {
        x = newX;
    }

    int colOld = int(x) / ts;
    int rowNewY = int(newY) / ts;
    if (grid[rowNewY][colOld] == 1)
    {
        dy = -dy;
    }
    else
    {
        y = newY;
    }
}

SinglePlayer::SinglePlayer()
    : playerId(""),
      lastSaveID(""),
      backgroundColor(Color::Black),
      backgroundImagePath(""),
      useBackgroundImage(false),
      score(0),
      p(nullptr),
      bonusCount(0),
      powerUps(0),
      nextPowerUpScore(50),
      powerUpUsed(false),
      powerUpActive(false),
      powerUpRemainingTime(0),
      ts(18),
      x(0),
      y(0),
      dx(0),
      dy(0),
      Game(true),
      timer(0),
      enemyCount(4),
      currentLevel(1),
      delay(0.07f),
      enemySpeed(1.0f),
      levelNo(1)
{
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = 0;
}

// Flood fill to mark enemy-reachable areas (these won't be captured)
void SinglePlayer::drop(int y, int x)
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

void SinglePlayer::resetGame()
{
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = 0;

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;

    score = 0;
    bonusCount = 0;

    powerUps = 0;
    powerUpActive = false;
    nextPowerUpScore = 50;
}

int SinglePlayer::countCapturedTiles()
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

// Adjust difficulty based on level
void SinglePlayer::setLevel(int levelNo)
{
    currentLevel = levelNo;
    switch (levelNo)
    {
    case 1:
        delay = 0.07f;
        enemySpeed = 1.0f;
        break;
    case 2:
        delay = 0.06f;
        enemySpeed = 1.2f;
        enemyCount = 5;
        break;
    case 3:
        delay = 0.05f;
        enemySpeed = 1.5f;
        enemyCount = 6;
        break;
    case 4:
        delay = 0.04f;
        enemySpeed = 1.8f;
        enemyCount = 7;
        break;
    case 5:
        delay = 0.03f;
        enemySpeed = 2.0f;
        enemyCount = 10;
        break;
    default:
        break;
    }
}

// Main game loop - returns final score or -1 if saved
int SinglePlayer::run(Color bgColor, string id, bool &madeSave, bool playingSave, int level,
                      RenderWindow &window, const string &saveIDToLoad, string &savedGameID,
                      const string &bgImagePath)
{
    levelNo = level;
    backgroundColor = bgColor;
    backgroundImagePath = bgImagePath;
    srand(time(0));
    p = system.returnPlayer(id);
    powerUps = p->powerUps;
    playerId = id;

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
            // Set texture with repeated mode disabled
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

    Texture t1, t2, t3;
    t1.loadFromFile("assets/images/tiles.png");
    t2.loadFromFile("assets/images/gameover.png");
    t3.loadFromFile("assets/images/enemy.png");

    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sGameover.setPosition(100, 100);
    sEnemy.setOrigin(20, 20);

    Clock clock;
    Font font;
    if (!font.loadFromFile("assets/fonts/COOPBL.ttf"))
    {
        cout << "Failed to load font\n";
    }

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10, 5);
    scoreText.setStyle(Text::Bold);

    Text bonusText;
    bonusText.setFont(font);
    bonusText.setCharacterSize(16);
    bonusText.setFillColor(Color::Yellow);
    bonusText.setPosition(10, 25);

    Text powerUpText;
    powerUpText.setFont(font);
    powerUpText.setCharacterSize(16);
    powerUpText.setFillColor(Color::Cyan);
    powerUpText.setPosition(N - 2, N - 2);

    // Initialize grid borders
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;

    // Load saved game if requested
    if (playingSave)
    {
        if (!saveIDToLoad.empty())
        {
            // Load using new Save ID system
            if (!loadGame(saveIDToLoad))
            {
                cout << "Failed to load save, starting new game." << endl;
            }
        }
        else
        {
            // Try legacy load
            loadGame("");
        }
    }
    setLevel(levelNo);
    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        if (powerUpActive)
        {
            powerUpRemainingTime = 3.0f - powerUpClock.getElapsedTime().asSeconds();
            if (powerUpRemainingTime <= 0)
            {
                powerUpActive = false;
                powerUpRemainingTime = 0;
            }
        }

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Escape || e.key.code == Keyboard::P)
                {
                    // ESC or P pressed - show pause menu (same as multiplayer)
                    bool shouldReset = false;
                    showPauseMenu(window, Game, madeSave, savedGameID, shouldReset);

                    if (shouldReset)
                    {
                        // Reset game - restore initial state
                        resetGame();
                        x = 0;
                        y = 0;
                        dx = 0;
                        dy = 0;
                        // Re-initialize enemies
                        for (int i = 0; i < enemyCount; i++)
                        {
                            a[i].x = a[i].y = 300;
                            a[i].dx = 4 - rand() % 8;
                            a[i].dy = 4 - rand() % 8;
                            a[i].frozen = false;
                        }
                        Game = true;
                    }
                    else if (!Game && madeSave)
                    {
                        // Game was saved, return -1 to indicate save
                        return -1;
                    }
                    else if (!Game)
                    {
                        // Game ended without saving
                        p->addMatchUpdate("PC", 1, score, powerUps, levelNo);
                        delete p;
                        return score;
                    }
                }
                /*
                 * Power-Up Activation (Single Player):
                 * - Player must be alive (Game == true)
                 * - Must have power-ups in inventory
                 * - No freeze currently active
                 * - Key: SPACE
                 * Effect: Freezes all enemies for 3 seconds
                 */
                else if (e.key.code == Keyboard::Space && Game && powerUps > 0 && !powerUpActive)
                {
                    powerUps--;
                    powerUpUsed = true;
                    powerUpClock.restart();
                    powerUpActive = true;
                    powerUpRemainingTime = 3.0f;
                }
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::A))
        {
            dx = -1;
            dy = 0;
        };
        if (Keyboard::isKeyPressed(Keyboard::D))
        {
            dx = 1;
            dy = 0;
        };
        if (Keyboard::isKeyPressed(Keyboard::W))
        {
            dx = 0;
            dy = -1;
        };
        if (Keyboard::isKeyPressed(Keyboard::S))
        {
            dx = 0;
            dy = 1;
        };

        if (!Game)
            continue;

        if (timer > delay)
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

            if (grid[y][x] == 2)
                Game = false;
            if (grid[y][x] == 0)
                grid[y][x] = 2;
            timer = 0;
        }

        for (int i = 0; i < enemyCount; i++)
        {
            if (!powerUpActive)
            {
                a[i].frozen = false;
                a[i].move(grid, ts, a[i].frozen, enemySpeed);
            }
            else
            {
                a[i].frozen = true;
            }
        }
        if (grid[y][x] == 1)
        {
            dx = dy = 0;

            // Flood fill algorithm to determine enclosed areas
            // Step 1: Mark all areas reachable by enemies with -1 (don't fill these)
            for (int i = 0; i < enemyCount; i++)
                drop(a[i].y / ts, a[i].x / ts);

            int captured = 0;

            // Step 2: Convert player trail (2) to walls (1), count captured tiles
            //         Fill enclosed areas with walls but don't count them for score
            //         Leave enemy-reachable areas (-1) as empty (0)
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
                        // Player trail becomes wall - COUNT THIS
                        grid[i][j] = 1;
                        captured++;
                    }
                    else if (grid[i][j] == 0)
                    {
                        // Enclosed empty area - fill with walls but DON'T count
                        grid[i][j] = 1;
                    }
                }
            }

            /*
             * Points System Implementation:
             * - Each tile captured = 1 point
             * - Capturing >10 tiles in single move = x2 points (double)
             * - After 3 bonus occurrences: threshold reduces to 5 tiles for double
             * - After 5 bonus occurrences: capturing >5 tiles = x4 points
             */
            int bonusThreshold = (bonusCount >= 3) ? 5 : 10;
            int multiplier = 1;

            if (captured > bonusThreshold)
            {
                // Award bonus multiplier for THIS capture
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
             * Power-up System:
             * - First power-up at score 50
             * - Additional at 70, 100, 130, then every +30 points
             * - Power-ups stack and are stored in inventory
             */
            while (score >= nextPowerUpScore)
            {
                powerUps++;
                // Power-up thresholds: 50, 70, 100, 130, 160, 190...
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

        for (int i = 0; i < enemyCount; i++)
            if (grid[a[i].y / ts][a[i].x / ts] == 2)
                Game = false;

        // Drawing - use background image if available, otherwise solid color
        if (useBackgroundImage)
        {
            window.clear();
            window.draw(backgroundSprite);
        }
        else
        {
            window.clear(backgroundColor);
        }

        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
            {
                if (grid[i][j] == 0)
                    continue;

                if (grid[i][j] == 1)
                    sTile.setTextureRect(IntRect(0, 0, ts, ts));
                if (grid[i][j] == 2)
                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
                if (grid[i][j] == 3)
                    sTile.setTextureRect(IntRect(72, 0, ts, ts));

                // Blue tint for tiles when freeze power-up is active
                if (powerUpActive)
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

        sTile.setTextureRect(IntRect(36, 0, ts, ts));
        sTile.setPosition(x * ts, y * ts);
        window.draw(sTile);

        sEnemy.rotate(10);
        for (int i = 0; i < enemyCount; i++)
        {
            // Blue tint for frozen enemies (visual consistency with multiplayer)
            if (a[i].frozen)
            {
                sEnemy.setColor(Color(100, 100, 255, 200)); // Blue tint for frozen enemies
            }
            else
            {
                sEnemy.setColor(Color(255, 255, 255, 255)); // Normal color
            }
            sEnemy.setPosition(a[i].x, a[i].y);
            window.draw(sEnemy);
        }

        // Blue overlay effect when power-up (freeze) is active
        if (powerUpActive)
        {
            RectangleShape blueEffect(Vector2f(N * ts, M * ts));
            blueEffect.setFillColor(Color(0, 100, 255, 50)); // Semi-transparent blue overlay
            window.draw(blueEffect);
        }

        window.draw(scoreText);
        window.draw(bonusText);
        window.draw(powerUpText);
        scoreText.setString("Player: " + p->username + " Score: " + to_string(score) + "  Power-Ups: " + to_string(powerUps) + (powerUpActive ? "  [ENEMIES FROZEN]" : ""));
        window.draw(scoreText);
        if (!Game)
        {
            p->addMatchUpdate("PC", 1, score, powerUps, levelNo);
            delete p;
            return score;
        }

        window.display();
    }

    // Return final score if loop exits normally
    return score;
}

/*
 * Method: saveGame
 * Description: Saves the current game state using the GameState structure.
 *              Creates a unique Save ID and serializes all game data to file.
 *              Includes a linked list of tiles the player has interacted with.
 *
 * Implementation Details:
 * 1. Creates a GameState object
 * 2. Populates it with current game data
 * 3. Builds linked list of tiles (player trail and modified tiles)
 * 4. Generates unique Save ID based on player ID and timestamp
 * 5. Serializes to file
 *
 * Returns: The generated Save ID (for display to player)
 */
string SinglePlayer::saveGame()
{
    GameState state;

    // Set identification info
    strncpy(state.playerID, playerId.c_str(), sizeof(state.playerID) - 1);
    state.playerID[sizeof(state.playerID) - 1] = '\0';
    state.generateSaveID(playerId.c_str());
    state.setTimestamp();

    // Set game progress
    state.score = score;
    state.bonusCount = bonusCount;
    state.powerUps = powerUps;
    state.nextPowerUpScore = nextPowerUpScore;

    // Set player state
    state.playerX = x;
    state.playerY = y;
    state.playerDX = dx;
    state.playerDY = dy;

    // Set power-up state
    state.powerUpActive = powerUpActive;
    state.powerUpRemainingTime = powerUpRemainingTime;

    // Set level info
    state.levelNo = levelNo;
    state.bgR = backgroundColor.r;
    state.bgG = backgroundColor.g;
    state.bgB = backgroundColor.b;
    state.bgA = backgroundColor.a;

    // Set enemy data
    state.enemyCount = enemyCount;
    for (int i = 0; i < enemyCount; i++)
    {
        state.enemyX[i] = a[i].x;
        state.enemyY[i] = a[i].y;
        state.enemyDX[i] = a[i].dx;
        state.enemyDY[i] = a[i].dy;
    }

    // Copy grid state
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            state.grid[i][j] = grid[i][j];
        }
    }

    // Build linked list of tiles player has interacted with
    // This includes player trail (2) and any modified tiles
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            // Store tiles that are part of player's trail or modified
            if (grid[i][j] == 2)
            { // Player trail
                state.tiles.insert(i, j, grid[i][j]);
            }
        }
    }

    // Also store current player position tile
    state.tiles.insert(y, x, grid[y][x]);

    // Serialize to file
    if (state.serialize())
    {
        lastSaveID = string(state.saveID);
        return lastSaveID;
    }

    return "";
}

/*
 * Method: loadGame
 * Description: Loads a saved game state from file using Save ID.
 *              Reconstructs the game state from the serialized data.
 *              Rebuilds the linked list of tiles.
 *
 * Parameters:
 *   saveIDToLoad - The unique Save ID to load (if empty, uses legacy format)
 *
 * Returns: true if load successful, false otherwise
 */
bool SinglePlayer::loadGame(const string &saveIDToLoad)
{
    GameState state;

    // If no Save ID provided, try legacy format for backwards compatibility
    if (saveIDToLoad.empty())
    {
        return loadGameLegacy();
    }

    // Deserialize from file
    if (!state.deserialize(saveIDToLoad))
    {
        return false;
    }

    // Restore game progress
    score = state.score;
    bonusCount = state.bonusCount;
    powerUps = state.powerUps;
    nextPowerUpScore = state.nextPowerUpScore;

    // Restore player state
    x = state.playerX;
    y = state.playerY;
    dx = state.playerDX;
    dy = state.playerDY;

    // Restore power-up state
    powerUpActive = state.powerUpActive;
    powerUpRemainingTime = state.powerUpRemainingTime;
    if (powerUpActive)
    {
        powerUpClock.restart();
    }

    // Restore level info
    levelNo = state.levelNo;
    backgroundColor = Color(
        (Uint8)state.bgR,
        (Uint8)state.bgG,
        (Uint8)state.bgB,
        (Uint8)state.bgA);

    // Restore enemy data
    enemyCount = state.enemyCount;
    for (int i = 0; i < enemyCount; i++)
    {
        a[i].x = state.enemyX[i];
        a[i].y = state.enemyY[i];
        a[i].dx = state.enemyDX[i];
        a[i].dy = state.enemyDY[i];
        a[i].frozen = false;
    }

    // Restore grid state
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            grid[i][j] = state.grid[i][j];
        }
    }

    // The linked list of tiles is already loaded in state.tiles
    // We can use this for verification or additional processing if needed
    cout << "Loaded " << state.tiles.getSize() << " tiles from save." << endl;

    lastSaveID = string(state.saveID);
    return true;
}

/*
 * Method: loadGameLegacy
 * Description: Legacy load function for backwards compatibility with old save format.
 *              Loads from playerId_save.txt format.
 * Returns: true if load successful, false otherwise
 */
bool SinglePlayer::loadGameLegacy()
{
    string filename = playerId + "_save.txt";
    ifstream saveFile(filename);
    if (!saveFile.is_open())
        return false;

    saveFile >> score;
    saveFile >> powerUps;
    saveFile >> bonusCount;

    int activeFlag;
    saveFile >> activeFlag;
    powerUpActive = (activeFlag != 0);
    saveFile >> powerUpRemainingTime;
    saveFile >> nextPowerUpScore;

    if (powerUpActive)
    {
        powerUpClock.restart();
    }

    saveFile >> x >> y;
    saveFile >> dx >> dy;
    saveFile >> enemyCount;
    for (int i = 0; i < enemyCount; i++)
    {
        saveFile >> a[i].x >> a[i].y >> a[i].dx >> a[i].dy;
    }

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            saveFile >> grid[i][j];
        }
    }
    saveFile >> levelNo;
    int r, g, b, aVal;
    saveFile >> r >> g >> b >> aVal;
    backgroundColor = Color((Uint8)r, (Uint8)g, (Uint8)b, (Uint8)aVal);

    saveFile.close();
    return true;
}

// Getter for last save ID (for display purposes)
string SinglePlayer::getLastSaveID() const
{
    return lastSaveID;
}

/*
 * Method: showSaveConfirmationScreen
 * Description: Displays a dedicated screen showing the Save ID after saving.
 *              This screen is shown immediately after saving.
 */
void SinglePlayer::showSaveConfirmationScreen(RenderWindow &window, const string &savedID)
{
    Font font;
    font.loadFromFile("assets/fonts/arial.ttf");

    Text titleText;
    titleText.setFont(font);
    titleText.setString("Game Saved Successfully!");
    titleText.setCharacterSize(36);
    titleText.setFillColor(Color::Green);
    titleText.setPosition(150.f, 100.f);

    Text saveIDLabel;
    saveIDLabel.setFont(font);
    saveIDLabel.setString("Your Save ID:");
    saveIDLabel.setCharacterSize(24);
    saveIDLabel.setFillColor(Color::White);
    saveIDLabel.setPosition(150.f, 200.f);

    Text saveIDText;
    saveIDText.setFont(font);
    saveIDText.setString(savedID);
    saveIDText.setCharacterSize(22);
    saveIDText.setFillColor(Color::Yellow);
    saveIDText.setPosition(150.f, 240.f);

    Text instructionText;
    instructionText.setFont(font);
    instructionText.setString("View and Copy Save IDs in Settings to resume your game later!");
    instructionText.setCharacterSize(18);
    instructionText.setFillColor(Color::Cyan);
    instructionText.setPosition(100.f, 320.f);

    Text tipText;
    tipText.setFont(font);
    tipText.setString("Tip: You can also find your saved games in Settings menu.");
    tipText.setCharacterSize(16);
    tipText.setFillColor(Color(180, 180, 180));
    tipText.setPosition(100.f, 360.f);

    Text promptText;
    promptText.setFont(font);
    promptText.setString("Press ESC or ENTER to return to Main Menu");
    promptText.setCharacterSize(20);
    promptText.setFillColor(Color::White);
    promptText.setPosition(180.f, 450.f);

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
                if (e.key.code == Keyboard::Escape || e.key.code == Keyboard::Enter)
                {
                    return;
                }
            }
        }

        window.clear(Color(30, 30, 30));
        window.draw(titleText);
        window.draw(saveIDLabel);
        window.draw(saveIDText);
        window.draw(instructionText);
        window.draw(tipText);
        window.draw(promptText);
        window.display();
    }
}

/*
 * Method: showPauseMenu
 * Description: Displays pause menu with options to Resume, Save, Reset, or End game.
 *              When saving, immediately shows the save confirmation screen.
 * Parameters:
 *   window - Reference to the game window
 *   Game - Reference to game running flag
 *   madeSave - Reference to flag indicating if save was made
 *   savedID - Reference to string to store the Save ID if saved
 *   shouldReset - Reference to flag indicating if game should be reset
 */
void SinglePlayer::showPauseMenu(RenderWindow &window, bool &Game, bool &madeSave, string &savedID, bool &shouldReset)
{
    const int optionCount = 4;
    const string labels[optionCount] = {
        "1) Resume Game",
        "2) Save Game",
        "3) Reset Game",
        "4) End Game"};
    Font font;
    font.loadFromFile("assets/fonts/arial.ttf");

    // Title
    Text titleText;
    titleText.setFont(font);
    titleText.setString("PAUSED");
    titleText.setCharacterSize(40);
    titleText.setFillColor(Color::Cyan);
    titleText.setPosition(100.f, 50.f);

    Text texts[optionCount];
    for (int i = 0; i < optionCount; ++i)
    {
        texts[i].setFont(font);
        texts[i].setString(labels[i]);
        texts[i].setCharacterSize(30);
        texts[i].setFillColor(i == 0 ? Color::Yellow : Color::White);
        texts[i].setPosition(100.f, 130.f + i * 50.f);
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
                        // Save Game - save and immediately show confirmation screen
                        savedID = saveGame();
                        if (!savedID.empty())
                        {
                            Game = false;
                            madeSave = true;
                            // Show save confirmation screen immediately
                            showSaveConfirmationScreen(window, savedID);
                            return;
                        }
                        else
                        {
                            // Show error briefly (could add error screen here)
                            cout << "Error: Could not save game!" << endl;
                        }
                    }
                    else if (selected == 2)
                    {
                        // Reset Game
                        shouldReset = true;
                        return;
                    }
                    else if (selected == 3)
                    {
                        // End Game
                        Game = false;
                        return;
                    }
                }
                else if (e.key.code == Keyboard::Escape)
                {
                    // ESC in pause menu = resume (like multiplayer)
                    return;
                }
            }
        }

        // Draw menu
        window.clear(Color(30, 30, 30));
        window.draw(titleText);
        for (int i = 0; i < optionCount; ++i)
            window.draw(texts[i]);

        window.display();
    }

    return;
}
