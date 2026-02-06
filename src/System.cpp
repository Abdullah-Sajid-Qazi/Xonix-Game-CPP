// Xonix Game
// System - manages players using dynamic array and hash table for O(1) lookup

#include "../header/System.h"
#include <iostream>
#include <fstream>

using namespace std;

System::System()
{
    ArrayOfPlayers = nullptr;
    noOfPlayers = 0;
    createArrayOfPlayers();
    buildHashTable();
}

System::~System()
{
    if (ArrayOfPlayers)
        delete[] ArrayOfPlayers;
}

void System::createArrayOfPlayers()
{
    ifstream allPlayers("data/AllPlayers.txt");
    if (!allPlayers.is_open())
    {
        cerr << "Error opening AllPlayers.txt" << endl;
        noOfPlayers = 0;
        ArrayOfPlayers = nullptr;
        return;
    }

    allPlayers >> noOfPlayers;
    allPlayers.ignore();

    if (noOfPlayers > 0)
    {
        ArrayOfPlayers = new string[noOfPlayers];
        for (int i = 0; i < noOfPlayers; i++)
            getline(allPlayers, ArrayOfPlayers[i]);
    }
    allPlayers.close();
}

// Build hash table mapping username -> player ID
void System::buildHashTable()
{
    playerHashTable.clear();

    for (int i = 0; i < noOfPlayers; i++)
    {
        Player *p = returnPlayer(ArrayOfPlayers[i]);
        if (p)
        {
            playerHashTable.insert(p->username, p->ID, i);
            delete p;
        }
    }
}

// Load player from file - caller must delete returned pointer
Player *System::returnPlayer(string id)
{
    Player *p = new Player;
    string s = "data/" + id + ".txt";
    ifstream p1(s);
    p->ID = id;

    if (!p1.is_open())
    {
        cerr << "Error: File " << s << " not found!" << endl;
        delete p;
        return nullptr;
    }

    getline(p1, p->username);
    getline(p1, p->password);
    getline(p1, p->regTime);
    p1 >> p->highScore;
    p1 >> p->highScoreLevel;
    p1 >> p->powerUps;

    // Load friends into linked list
    p1 >> p->noOfFriends;
    p1.ignore();
    for (int i = 0; i < p->noOfFriends; i++)
    {
        getline(p1, s);
        p->friendList.insert(s);
    }

    // Load match history into stack (reverse order to maintain order)
    p1 >> p->noOfMatches;
    p1.ignore();
    if (p->noOfMatches > 0)
    {
        string *tempMatches = new string[p->noOfMatches];
        for (int i = 0; i < p->noOfMatches; i++)
            getline(p1, tempMatches[i]);
        for (int i = p->noOfMatches - 1; i >= 0; i--)
            p->matchHistory.push(tempMatches[i]);
        delete[] tempMatches;
    }

    // Load friend requests into stack
    p1 >> p->noOfRequests;
    p1.ignore();
    if (p->noOfRequests > 0)
    {
        string *tempRequests = new string[p->noOfRequests];
        for (int i = 0; i < p->noOfRequests; i++)
            getline(p1, tempRequests[i]);
        for (int i = p->noOfRequests - 1; i >= 0; i--)
            p->friendRequest.push(tempRequests[i]);
        delete[] tempRequests;
    }

    if (p1 >> p->preferredTheme)
    {
        if (p->preferredTheme < 1 || p->preferredTheme > 5)
            p->preferredTheme = 1;
    }
    else
    {
        p->preferredTheme = 1; // Default to Forest theme
    }

    p1.close();
    return p;
}

// Validate login using hash table for O(1) lookup
bool System::isValid(string Username, string Password)
{
    string playerId = playerHashTable.getPlayerId(Username);

    if (!playerId.empty())
    {
        Player *p = returnPlayer(playerId);
        if (p)
        {
            bool valid = (p->password == Password);
            delete p;
            return valid;
        }
    }

    // Fallback to linear search
    for (int i = 0; i < noOfPlayers; i++)
    {
        Player *p = returnPlayer(ArrayOfPlayers[i]);
        if (p && p->username == Username)
        {
            bool valid = (p->password == Password);
            delete p;
            return valid;
        }
        if (p)
            delete p;
    }

    return false;
}

string System::returnId(string Username)
{
    string id = playerHashTable.getPlayerId(Username);
    if (!id.empty())
        return id;

    // Fallback to linear search
    for (int i = 0; i < noOfPlayers; i++)
    {
        Player *p = returnPlayer(ArrayOfPlayers[i]);
        if (p && p->username == Username)
        {
            id = p->ID;
            delete p;
            return id;
        }
        if (p)
            delete p;
    }

    return "";
}

string System::returnIdByUsername(const string &username)
{
    return returnId(username);
}

bool System::usernameExists(string Username)
{
    if (playerHashTable.exists(Username))
        return true;

    for (int i = 0; i < noOfPlayers; i++)
    {
        Player *p = returnPlayer(ArrayOfPlayers[i]);
        if (p && p->username == Username)
        {
            delete p;
            return true;
        }
        if (p)
            delete p;
    }

    return false;
}

// Register new player - resize array, create file, update hash table
void System::addPlayer(string regtime, string Username, string Password)
{
    // Find next available ID
    int newId = 0;
    for (int i = 0; i < noOfPlayers; i++)
    {
        int existingId = 0;
        for (int j = 0; j < (int)ArrayOfPlayers[i].length(); j++)
        {
            if (ArrayOfPlayers[i][j] >= '0' && ArrayOfPlayers[i][j] <= '9')
                existingId = existingId * 10 + (ArrayOfPlayers[i][j] - '0');
        }
        if (existingId >= newId)
            newId = existingId + 1;
    }

    // Resize dynamic array
    string *temp = new string[noOfPlayers + 1];
    if (ArrayOfPlayers)
    {
        for (int i = 0; i < noOfPlayers; i++)
            temp[i] = ArrayOfPlayers[i];
        delete[] ArrayOfPlayers;
    }

    temp[noOfPlayers] = to_string(newId);
    ArrayOfPlayers = temp;
    noOfPlayers++;

    Player *newPlayer = new Player;
    newPlayer->username = Username;
    newPlayer->password = Password;
    newPlayer->ID = to_string(newId);
    newPlayer->regTime = regtime;
    newPlayer->saveToFile();

    playerHashTable.insert(Username, newPlayer->ID, noOfPlayers - 1);

    savePlayers();
    delete newPlayer;

    cout << "Player " << Username << " registered with ID " << newId << endl;
}

bool System::sendFriendRequest(const string &senderId, const string &receiverId)
{
    Player *sender = returnPlayer(senderId);
    Player *receiver = returnPlayer(receiverId);

    if (!sender || !receiver)
    {
        if (sender)
            delete sender;
        if (receiver)
            delete receiver;
        cout << "Error: Player not found.\n";
        return false;
    }

    // Check if already friends
    if (sender->friendList.search(receiverId))
    {
        cout << "You are already friends with this player.\n";
        delete sender;
        delete receiver;
        return false;
    }

    // Check if request already pending
    if (receiver->friendRequest.search(senderId))
    {
        cout << "Friend request already pending.\n";
        delete sender;
        delete receiver;
        return false;
    }

    // Add request to receiver's pending requests
    receiver->addRequest(senderId);

    delete sender;
    delete receiver;

    cout << "Friend request sent successfully!\n";
    return true;
}

bool System::acceptFriendRequest(const string &acceptorId, const string &requesterId)
{
    // Load acceptor (the one accepting the request)
    Player *acceptor = returnPlayer(acceptorId);
    if (!acceptor)
    {
        cout << "Error: Acceptor player not found.\n";
        return false;
    }

    // Step 1: Remove request from acceptor's pending requests
    if (acceptor->friendRequest.search(requesterId))
    {
        acceptor->friendRequest.remove(requesterId);
        acceptor->noOfRequests--;
    }

    // Step 2: Add requester to acceptor's friends list
    if (!acceptor->friendList.search(requesterId))
    {
        acceptor->friendList.insert(requesterId);
        acceptor->noOfFriends++;
    }

    // Step 3: Save acceptor's data
    acceptor->saveToFile();
    delete acceptor;

    // Load requester (the one who sent the request)
    Player *requester = returnPlayer(requesterId);
    if (!requester)
    {
        cout << "Error: Requester player not found.\n";
        return false;
    }

    // Step 4: Add acceptor to requester's friends list
    if (!requester->friendList.search(acceptorId))
    {
        requester->friendList.insert(acceptorId);
        requester->noOfFriends++;
    }

    // Step 5: Save requester's data
    requester->saveToFile();
    delete requester;

    cout << "Friend request accepted! Both players are now friends.\n";
    return true;
}

bool System::rejectFriendRequest(const string &rejecterId, const string &requesterId)
{
    Player *rejecter = returnPlayer(rejecterId);

    if (!rejecter)
    {
        cout << "Error: Player not found.\n";
        return false;
    }

    rejecter->rejectRequest(requesterId);
    delete rejecter;

    cout << "Friend request rejected.\n";
    return true;
}

void System::savePlayers()
{
    ofstream allPlayers("data/AllPlayers.txt");

    if (!allPlayers.is_open())
    {
        cerr << "Error: couldn't open AllPlayers.txt for writing\n";
        return;
    }

    allPlayers << noOfPlayers << endl;
    for (int i = 0; i < noOfPlayers; i++)
    {
        allPlayers << ArrayOfPlayers[i] << endl;
    }
    allPlayers.close();
}

void System::displayArrayOfPlayers()
{
    cout << "Array of Players (" << noOfPlayers << " total):\n";
    for (int i = 0; i < noOfPlayers; i++)
    {
        cout << "  [" << i << "] ID: " << ArrayOfPlayers[i] << endl;
    }
}

void System::displayHashTable()
{
    playerHashTable.display();
}

bool System::isIdPresent(const string &id)
{
    for (int i = 0; i < noOfPlayers; i++)
    {
        if (ArrayOfPlayers[i] == id)
        {
            return true;
        }
    }
    return false;
}

bool System::idExist(const string &id)
{
    return isIdPresent(id);
}

Player *System::getPlayerByUsername(const string &username)
{
    string playerId = playerHashTable.getPlayerId(username);
    if (playerId.empty())
    {
        return nullptr;
    }
    return returnPlayer(playerId);
}
