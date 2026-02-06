// Xonix Game
// Hash Table implementation for O(1) username lookups

#include "../data_structures/HashTable.h"
#include <iostream>

HashNode::HashNode(const std::string &user, const std::string &id, int idx)
    : username(user), playerId(id), arrayIndex(idx), next(nullptr) {}

HashTable::HashTable() : count(0)
{
    for (int i = 0; i < TABLE_SIZE; i++)
        table[i] = nullptr;
}

HashTable::~HashTable()
{
    clear();
}

// Polynomial rolling hash for good distribution
int HashTable::hashFunction(const std::string &username) const
{
    unsigned long hash = 0;
    const int prime = 31;

    for (int i = 0; i < (int)username.length(); i++)
        hash = hash * prime + username[i];

    return hash % TABLE_SIZE;
}

void HashTable::insert(const std::string &username, const std::string &playerId, int arrayIndex)
{
    int index = hashFunction(username);

    // Update if username exists
    HashNode *current = table[index];
    while (current)
    {
        if (current->username == username)
        {
            current->playerId = playerId;
            current->arrayIndex = arrayIndex;
            return;
        }
        current = current->next;
    }

    // Insert at front of chain
    HashNode *newNode = new HashNode(username, playerId, arrayIndex);
    newNode->next = table[index];
    table[index] = newNode;
    count++;
}

HashNode *HashTable::search(const std::string &username) const
{
    int index = hashFunction(username);

    HashNode *current = table[index];
    while (current)
    {
        if (current->username == username)
        {
            return current;
        }
        current = current->next;
    }

    return nullptr;
}

std::string HashTable::getPlayerId(const std::string &username) const
{
    HashNode *node = search(username);
    return node ? node->playerId : "";
}

int HashTable::getArrayIndex(const std::string &username) const
{
    HashNode *node = search(username);
    return node ? node->arrayIndex : -1;
}

bool HashTable::exists(const std::string &username) const
{
    return search(username) != nullptr;
}

bool HashTable::remove(const std::string &username)
{
    int index = hashFunction(username);
    HashNode *current = table[index];
    HashNode *prev = nullptr;

    while (current)
    {
        if (current->username == username)
        {
            if (prev)
                prev->next = current->next;
            else
                table[index] = current->next;
            delete current;
            count--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

void HashTable::clear()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        HashNode *current = table[i];
        while (current)
        {
            HashNode *next = current->next;
            delete current;
            current = next;
        }
        table[i] = nullptr;
    }
    count = 0;
}

int HashTable::getCount() const
{
    return count;
}

void HashTable::display() const
{
    std::cout << "\n===== Hash Table Contents =====\n";
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (table[i])
        {
            std::cout << "Bucket " << i << ": ";
            HashNode *current = table[i];
            while (current)
            {
                std::cout << "[" << current->username << " -> ID:" << current->playerId
                          << ", Idx:" << current->arrayIndex << "] ";
                current = current->next;
            }
            std::cout << std::endl;
        }
    }
    std::cout << "Total entries: " << count << std::endl;
    std::cout << "================================\n";
}
