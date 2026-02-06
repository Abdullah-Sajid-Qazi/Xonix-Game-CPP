// Xonix Game
// Leaderboard using Min-Heap - keeps top N players sorted by high score

#include "../header/LeaderBoard.h"
#include <iostream>

using namespace std;

Leaderboard::Leaderboard(System *sys, int cap)
{
    capacity = cap;
    size = 0;
    system = sys;
    heap = new string[capacity];
    sortedHeap = new string[capacity];
    isSorted = false;
    for (int i = 0; i < capacity; i++)
    {
        heap[i] = "";
        sortedHeap[i] = "";
    }
}

Leaderboard::~Leaderboard()
{
    delete[] heap;
    delete[] sortedHeap;
}

void Leaderboard::heapifyUp(int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;
        // Min-heap: smaller scores bubble up
        if (getHighScore(heap[index]) < getHighScore(heap[parent]))
        {
            string temp = heap[index];
            heap[index] = heap[parent];
            heap[parent] = temp;
            index = parent;
        }
        else
            break;
    }
}

void Leaderboard::heapifyDown(int index)
{
    while (2 * index + 1 < size)
    {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < size && getHighScore(heap[left]) < getHighScore(heap[smallest]))
            smallest = left;
        if (right < size && getHighScore(heap[right]) < getHighScore(heap[smallest]))
            smallest = right;

        if (smallest != index)
        {
            string temp = heap[index];
            heap[index] = heap[smallest];
            heap[smallest] = temp;
            index = smallest;
        }
        else
            break;
    }
}

int Leaderboard::findPlayerIndex(const string &playerID)
{
    for (int i = 0; i < size; i++)
    {
        if (heap[i] == playerID)
        {
            return i;
        }
    }
    return -1;
}

void Leaderboard::rebuildHeap()
{
    // Build heap from last non-leaf node
    for (int i = (size / 2) - 1; i >= 0; i--)
        heapifyDown(i);
}

int Leaderboard::getHighScore(const string &id)
{
    Player *p = system->returnPlayer(id);
    int score = p ? p->highScore : 0;
    delete p;
    return score;
}

int Leaderboard::getHighScoreLevel(const string &id)
{
    Player *p = system->returnPlayer(id);
    int level = p ? p->highScoreLevel : 1;
    delete p;
    return level;
}

string Leaderboard::getLevelName(int level)
{
    switch (level)
    {
    case 1:
        return "Easy";
    case 2:
        return "Medium";
    case 3:
        return "Hard";
    case 4:
        return "Expert";
    case 5:
        return "Insane";
    default:
        return "Unknown";
    }
}

void Leaderboard::insert(const string &playerID)
{
    if (size < capacity)
    {
        heap[size++] = playerID;
        heapifyUp(size - 1); // Restore heap property
        isSorted = false;
    }
}

void Leaderboard::replaceRoot(const string &playerID)
{
    heap[0] = playerID;
    heapifyDown(0); // Restore heap property
    isSorted = false;
}

void Leaderboard::makeLeaderboard()
{
    size = 0;
    isSorted = false;

    for (int i = 0; i < system->noOfPlayers; ++i)
    {
        Player *p = system->returnPlayer(system->ArrayOfPlayers[i]);
        if (!p || p->highScore == 0)
        {
            if (p) delete p;
            continue;
        }

        string id = p->ID;
        int playerScore = p->highScore;
        delete p;

        // Skip duplicates
        if (findPlayerIndex(id) != -1)
            continue;

        if (size < capacity)
            insert(id);
        else if (playerScore > getHighScore(heap[0]))
            replaceRoot(id);  // Replace min if new score is higher
    }
}

const string *Leaderboard::getSortedHeap()
{
    if (!isSorted)
    {
        for (int i = 0; i < size; ++i)
            sortedHeap[i] = heap[i];

        // Selection sort descending (highest score first)
        for (int i = 0; i < size - 1; ++i)
        {
            int maxIdx = i;
            for (int j = i + 1; j < size; ++j)
            {
                if (getHighScore(sortedHeap[j]) > getHighScore(sortedHeap[maxIdx]))
                    maxIdx = j;
            }
            if (maxIdx != i)
            {
                string temp = sortedHeap[i];
                sortedHeap[i] = sortedHeap[maxIdx];
                sortedHeap[maxIdx] = temp;
            }
        }
        isSorted = true;
    }
    return sortedHeap;
}

int Leaderboard::getSize() const
{
    return size;
}

int Leaderboard::getCapacity() const
{
    return capacity;
}

void Leaderboard::displayLeaderboard()
{
    cout << "\n========== TOP " << capacity << " LEADERBOARD ==========\n";
    cout << "Rank | Player ID | Username | Score | Level\n";
    cout << "---------------------------------------------\n";

    const string *sorted = getSortedHeap();

    for (int i = 0; i < size; ++i)
    {
        Player *p = system->returnPlayer(sorted[i]);
        if (p)
        {
            cout << (i + 1) << ".   | " << p->ID << "         | "
                 << p->username << " | " << p->highScore
                 << " | " << getLevelName(p->highScoreLevel) << endl;
            delete p;
        }
    }

    if (size == 0)
    {
        cout << "No players on leaderboard yet!\n";
    }
    cout << "=============================================\n";
}
