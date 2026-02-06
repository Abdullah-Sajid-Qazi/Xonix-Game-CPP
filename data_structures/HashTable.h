// Xonix Game - DSA Project
// Hash Table for fast player lookup by username
// Uses chaining (linked list) for collision resolution

#pragma once
#include <string>
using namespace std;

struct HashNode
{
    string username;  // Key
    string playerId;  // Value - maps to player file
    int arrayIndex;   // Index in player array
    HashNode *next;   // For chaining

    HashNode(const string &user, const string &id, int idx);
};

// Hash Table with chaining - maps username to player ID
class HashTable
{
private:
    static const int TABLE_SIZE = 53;  // Prime number for better distribution
    HashNode *table[TABLE_SIZE];
    int count;

    int hashFunction(const string &username) const;

public:
    HashTable();
    ~HashTable();

    void insert(const string &username, const string &playerId, int arrayIndex);
    HashNode *search(const string &username) const;
    string getPlayerId(const string &username) const;
    int getArrayIndex(const string &username) const;
    bool exists(const string &username) const;
    bool remove(const string &username);
    void clear();
    int getCount() const;
    void display() const;
};
