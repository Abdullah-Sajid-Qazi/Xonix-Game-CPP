// Xonix Game
// System class - manages player database using Array, Hash Table, and per-player Linked Lists

#pragma once
#include <string>
#include "Player.h"
#include "../data_structures/HashTable.h"

using namespace std;

// Central player management system
// Uses: Dynamic Array for player IDs, Hash Table for username lookup
class System
{
public:
    string *ArrayOfPlayers;    // Dynamic array of all player IDs
    int noOfPlayers;
    HashTable playerHashTable; // Username -> Player ID mapping

public:
    System();
    ~System();

    void createArrayOfPlayers();  // Load player IDs from AllPlayers.txt
    void buildHashTable();        // Build hash table for O(1) username lookup

    Player *returnPlayer(string id);  // Load player from file (caller must delete)
    
    bool isValid(string Username, string Password);  // Validate login credentials
    string returnId(string Username);                // Get player ID by username
    string returnIdByUsername(const string &username);
    bool usernameExists(string Username);

    void addPlayer(string regtime, string Username, string Password);  // Register new player

    // Friend request system
    bool sendFriendRequest(const string &senderId, const string &receiverId);
    bool acceptFriendRequest(const string &acceptorId, const string &requesterId);
    bool rejectFriendRequest(const string &rejecterId, const string &requesterId);

    void savePlayers();  // Save master player list to file
    void displayArrayOfPlayers();
    void displayHashTable();
    
    bool isIdPresent(const string &id);
    bool idExist(const string &id);
    Player *getPlayerByUsername(const string &username);
};
