// Xonix Game
// Player class - stores profile, friends (LinkedList), match history (Stack), friend requests (Stack)

#pragma once
#include <string>
#include "../data_structures/List.h"
#include "../data_structures/Stack.h"
using namespace std;

class Player
{
public:
    // Player info
    string ID;
    string username;
    string password;
    string regTime;

    // Game stats
    int highScore;
    int highScoreLevel;  // Difficulty (1-5) when high score was achieved
    int powerUps;

    // Social features using custom data structures
    LinkedList friendList;   // Friends stored in Linked List
    int noOfFriends;
    Stack matchHistory;      // Match history stored in Stack (LIFO)
    int noOfMatches;
    Stack friendRequest;     // Pending requests in Stack
    int noOfRequests;

    int preferredTheme;  // 1-5

public:
    Player();
    ~Player();

    // Update match history and high score after a game
    void addMatchUpdate(string opponent, bool win, int score, int powerUpsNow, int difficultyLevel = 1);
    
    void saveToFile();
    void addRequest(string request_id);
    void display();
    void rejectRequest(string id);
    void acceptRequest(string id);
    bool changePassword(string newPassword);
};
