// Xonix Game
// Matchmaking system using Priority Queue (Max-Heap) and Circular Queue
// Higher score players get matched first

#pragma once
#include <string>
#include "System.h"

using namespace std;

struct QueuedPlayer
{
    string playerID;
    int score;
    string username;

    QueuedPlayer();
    QueuedPlayer(string id, int s, string name);
};

struct Match
{
    string p1;
    string p2;
    string p1Username;
    string p2Username;
    int p1Score;
    int p2Score;

    Match();
};

// Max-Heap for matchmaking - higher score = higher priority
class PlayerPriorityQueue
{
private:
    QueuedPlayer *heap;
    int capacity;
    int size;

    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    PlayerPriorityQueue(int cap = 20);
    ~PlayerPriorityQueue();

    bool isEmpty() const;
    bool isFull() const;
    int getSize() const;

    bool insert(const QueuedPlayer &player);
    QueuedPlayer extractMax();
    QueuedPlayer peek() const;
    QueuedPlayer getPlayerAt(int index) const;
    bool removePlayer(const string &playerID);
    void clear();
};

// Circular Queue for storing paired matches
class MatchQueue
{
private:
    Match *data;
    int capacity;
    int front;
    int rear;
    int count;

public:
    MatchQueue(int size = 20);
    ~MatchQueue();

    bool isEmpty() const;
    bool isFull() const;
    int size() const;

    bool enqueue(const Match &m);
    Match dequeue();
    Match peek() const;
    void clear();
    Match getMatchAt(int index) const;
};

// Game room - manages waiting queue and match creation
class GameRoom
{
private:
    PlayerPriorityQueue waitingQueue;
    MatchQueue matchQueue;
    System *system;

public:
    GameRoom(System *sys);

    bool addPlayerByID(const string &playerID);
    bool addPlayerToQueue(const string &username, const string &password);
    bool removePlayerFromQueue(const string &playerID);

    int getWaitingCount() const;
    QueuedPlayer getWaitingPlayer(int index) const;

    bool canStartMatchmaking() const;
    int createMatches();

    bool hasNextMatch() const;
    Match getNextMatch();
    Match peekNextMatch() const;
    int getMatchCount() const;
    Match getMatchAt(int index) const;

    void clearAll();
};
