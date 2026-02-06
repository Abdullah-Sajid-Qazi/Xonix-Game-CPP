// Xonix Game
// Matchmaking system - Priority Queue for ranked pairing, Circular Queue for match history

#include "../header/MatchmakingQueue.h"
#include <iostream>

using namespace std;

QueuedPlayer::QueuedPlayer() : playerID(""), score(0), username("") {}
QueuedPlayer::QueuedPlayer(string id, int s, string name) : playerID(id), score(s), username(name) {}

Match::Match() : p1(""), p2(""), p1Username(""), p2Username(""), p1Score(0), p2Score(0) {}

// Priority Queue - max-heap by score for skill-based matchmaking
PlayerPriorityQueue::PlayerPriorityQueue(int cap) : capacity(cap), size(0)
{
    heap = new QueuedPlayer[capacity];
}

PlayerPriorityQueue::~PlayerPriorityQueue()
{
    delete[] heap;
}

void PlayerPriorityQueue::heapifyUp(int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;
        // Max-heap: higher scores bubble up
        if (heap[index].score > heap[parent].score)
        {
            QueuedPlayer temp = heap[index];
            heap[index] = heap[parent];
            heap[parent] = temp;
            index = parent;
        }
        else
            break;
    }
}

void PlayerPriorityQueue::heapifyDown(int index)
{
    while (2 * index + 1 < size)
    {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        if (left < size && heap[left].score > heap[largest].score)
            largest = left;
        if (right < size && heap[right].score > heap[largest].score)
            largest = right;

        if (largest != index)
        {
            QueuedPlayer temp = heap[index];
            heap[index] = heap[largest];
            heap[largest] = temp;
            index = largest;
        }
        else
            break;
    }
}

bool PlayerPriorityQueue::isEmpty() const { return size == 0; }
bool PlayerPriorityQueue::isFull() const { return size == capacity; }
int PlayerPriorityQueue::getSize() const { return size; }

bool PlayerPriorityQueue::insert(const QueuedPlayer &player)
{
    if (isFull())
    {
        cerr << "Priority Queue is full!\n";
        return false;
    }

    // Check if player already in queue
    for (int i = 0; i < size; i++)
    {
        if (heap[i].playerID == player.playerID)
        {
            cout << "Player already in queue!\n";
            return false;
        }
    }

    heap[size] = player;
    heapifyUp(size);
    size++;
    return true;
}

QueuedPlayer PlayerPriorityQueue::extractMax()
{
    if (isEmpty())
    {
        return QueuedPlayer();
    }

    QueuedPlayer max = heap[0];
    heap[0] = heap[size - 1];
    size--;
    if (size > 0)
    {
        heapifyDown(0);
    }
    return max;
}

QueuedPlayer PlayerPriorityQueue::peek() const
{
    if (isEmpty())
    {
        return QueuedPlayer();
    }
    return heap[0];
}

QueuedPlayer PlayerPriorityQueue::getPlayerAt(int index) const
{
    if (index >= 0 && index < size)
    {
        return heap[index];
    }
    return QueuedPlayer();
}

bool PlayerPriorityQueue::removePlayer(const string &playerID)
{
    int index = -1;
    for (int i = 0; i < size; i++)
    {
        if (heap[i].playerID == playerID)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
        return false;

    // Replace with last element and reheapify
    heap[index] = heap[size - 1];
    size--;

    if (index < size)
    {
        heapifyDown(index);
        heapifyUp(index);
    }
    return true;
}

void PlayerPriorityQueue::clear()
{
    size = 0;
}

// Circular Queue for match history
MatchQueue::MatchQueue(int size) : capacity(size), front(0), rear(-1), count(0)
{
    data = new Match[capacity];
}

MatchQueue::~MatchQueue()
{
    delete[] data;
}

bool MatchQueue::isEmpty() const { return count == 0; }
bool MatchQueue::isFull() const { return count == capacity; }
int MatchQueue::size() const { return count; }

bool MatchQueue::enqueue(const Match &m)
{
    if (isFull())
    {
        cerr << "Match Queue is full!\n";
        return false;
    }
    rear = (rear + 1) % capacity;
    data[rear] = m;
    ++count;
    return true;
}

Match MatchQueue::dequeue()
{
    if (isEmpty())
    {
        return Match();
    }
    Match m = data[front];
    front = (front + 1) % capacity;
    --count;
    return m;
}

Match MatchQueue::peek() const
{
    if (isEmpty())
    {
        return Match();
    }
    return data[front];
}

void MatchQueue::clear()
{
    front = 0;
    rear = -1;
    count = 0;
}

Match MatchQueue::getMatchAt(int index) const
{
    if (index >= 0 && index < count)
    {
        int actualIndex = (front + index) % capacity;
        return data[actualIndex];
    }
    return Match();
}

// Game Room - combines priority queue and circular queue for matchmaking
GameRoom::GameRoom(System *sys) : waitingQueue(20), matchQueue(10), system(sys) {}

bool GameRoom::addPlayerByID(const string &playerID)
{
    // Check if player ID exists
    if (!system->isIdPresent(playerID))
    {
        return false;
    }

    // Get player data
    Player *p = system->returnPlayer(playerID);
    if (!p)
        return false;

    // Create queued player with score from player data
    QueuedPlayer qp(playerID, p->highScore, p->username);
    delete p;

    return waitingQueue.insert(qp);
}

bool GameRoom::addPlayerToQueue(const string &username, const string &password)
{
    // Validate credentials
    if (!system->isValid(username, password))
    {
        return false;
    }

    // Get player data
    string playerID = system->returnId(username);
    Player *p = system->returnPlayer(playerID);
    if (!p)
        return false;

    // Create queued player with score from leaderboard
    QueuedPlayer qp(playerID, p->highScore, p->username);
    delete p;

    return waitingQueue.insert(qp);
}

bool GameRoom::removePlayerFromQueue(const string &playerID)
{
    return waitingQueue.removePlayer(playerID);
}

int GameRoom::getWaitingCount() const
{
    return waitingQueue.getSize();
}

QueuedPlayer GameRoom::getWaitingPlayer(int index) const
{
    return waitingQueue.getPlayerAt(index);
}

bool GameRoom::canStartMatchmaking() const
{
    return waitingQueue.getSize() >= 2;
}

int GameRoom::createMatches()
{
    matchQueue.clear();
    int matchesCreated = 0;

    // Pair players while at least 2 are available
    while (waitingQueue.getSize() >= 2)
    {
        QueuedPlayer p1 = waitingQueue.extractMax(); // Highest score
        QueuedPlayer p2 = waitingQueue.extractMax(); // Second highest

        Match m;
        m.p1 = p1.playerID;
        m.p2 = p2.playerID;
        m.p1Username = p1.username;
        m.p2Username = p2.username;
        m.p1Score = p1.score;
        m.p2Score = p2.score;

        matchQueue.enqueue(m);
        matchesCreated++;
    }

    return matchesCreated;
}

bool GameRoom::hasNextMatch() const
{
    return !matchQueue.isEmpty();
}

Match GameRoom::getNextMatch()
{
    return matchQueue.dequeue();
}

Match GameRoom::peekNextMatch() const
{
    return matchQueue.peek();
}

int GameRoom::getMatchCount() const
{
    return matchQueue.size();
}

Match GameRoom::getMatchAt(int index) const
{
    return matchQueue.getMatchAt(index);
}

void GameRoom::clearAll()
{
    waitingQueue.clear();
    matchQueue.clear();
}
