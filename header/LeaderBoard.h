// Xonix Game
// Leaderboard using Min-Heap - maintains top 10 players by score
// Root holds minimum score so we can easily replace when higher scores come in

#pragma once
#include <string>
#include "System.h"

using namespace std;

// Min-Heap based leaderboard for top 10 players
class Leaderboard
{
private:
    string *heap;       // Array-based heap storing player IDs
    int size;
    int capacity;       // Fixed at 10 for top 10
    System *system;

    string *sortedHeap; // Cached sorted array for display
    bool isSorted;

    void heapifyUp(int index);
    void heapifyDown(int index);
    int findPlayerIndex(const string &playerID);
    void rebuildHeap();

public:
    Leaderboard(System *sys, int cap = 10);
    ~Leaderboard();

    int getHighScore(const string &id);
    int getHighScoreLevel(const string &id);
    string getLevelName(int level);

    void insert(const string &playerID);
    void replaceRoot(const string &playerID);
    void makeLeaderboard();  // Build from all players

    const string *getSortedHeap();  // Returns sorted (descending) for display
    int getSize() const;
    int getCapacity() const;
    void displayLeaderboard();
};
