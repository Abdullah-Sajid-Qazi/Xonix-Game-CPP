// Xonix Game
// Player class - uses LinkedList for friends, Stack for match history and requests

#include "../header/Player.h"
#include <iostream>
#include <fstream>

using namespace std;

Player::Player()
    : ID(""), username(""), password(""), regTime(""),
      highScore(0), highScoreLevel(1), powerUps(0),
      noOfFriends(0), noOfMatches(0), noOfRequests(0),
      preferredTheme(1)
{
}

Player::~Player()
{
}

void Player::addMatchUpdate(string opponent, bool win, int score, int powerUpsNow, int difficultyLevel)
{
    string match = "";
    noOfMatches++;

    if (opponent == "PC")
    {
        match = "Single Player - Score: " + to_string(score) + " (Level " + to_string(difficultyLevel) + ")";
    }
    else
    {
        if (win)
            match = "vs " + opponent + " - WIN (Score: " + to_string(score) + ")";
        else
            match = "vs " + opponent + " - LOSE (Score: " + to_string(score) + ")";
    }

    matchHistory.push(match);

    if (score > highScore)
    {
        highScore = score;
        highScoreLevel = difficultyLevel;
    }

    powerUps = powerUpsNow;
    saveToFile();
}

void Player::saveToFile()
{
    string filename = "data/" + ID + ".txt";
    ofstream outFile(filename);

    outFile << username << endl;
    outFile << password << endl;
    outFile << regTime << endl;
    outFile << highScore << endl;
    outFile << highScoreLevel << endl;
    outFile << powerUps << endl;

    // Write friends (linked list traversal)
    outFile << noOfFriends << endl;
    NodeList *curr = friendList.head;
    while (curr)
    {
        outFile << curr->data << endl;
        curr = curr->next;
    }

    // Write match history (stack traversal)
    outFile << noOfMatches << endl;
    node *matchCurr = matchHistory.head;
    while (matchCurr)
    {
        outFile << matchCurr->data << endl;
        matchCurr = matchCurr->next;
    }

    // Write friend requests (stack traversal)
    outFile << noOfRequests << endl;
    node *reqCurr = friendRequest.head;
    while (reqCurr)
    {
        outFile << reqCurr->data << endl;
        reqCurr = reqCurr->next;
    }

    outFile << preferredTheme << endl;
    outFile.close();
}

void Player::addRequest(string request_id)
{
    if (friendRequest.search(request_id) || friendList.search(request_id))
        return;
    
    friendRequest.push(request_id);
    noOfRequests++;
    saveToFile();
    cout << "Request Added successfully\n";
}

void Player::display()
{
    cout << "Player ID: " << ID << endl;
    cout << "Username: " << username << endl;
    cout << "Password: " << password << endl;
    cout << "Registration Time: " << regTime << endl;
    cout << "High Score: " << highScore << endl;
    cout << "Power-Ups: " << powerUps << endl;

    cout << "\nFriends (" << noOfFriends << "): ";
    friendList.display();

    cout << "\nMatch History (" << noOfMatches << "): ";
    if (matchHistory.isEmpty())
    {
        cout << "No matches played.\n";
    }
    else
    {
        Stack temp = matchHistory;
        while (!temp.isEmpty())
        {
            cout << temp.pop();
            if (!temp.isEmpty())
                cout << " <- ";
        }
        cout << endl;
    }

    cout << "\nFriend Requests (" << noOfRequests << "): ";
    if (friendRequest.isEmpty())
    {
        cout << "No pending requests.\n";
    }
    else
    {
        Stack temp = friendRequest;
        while (!temp.isEmpty())
        {
            cout << temp.pop();
            if (!temp.isEmpty())
                cout << " <- ";
        }
        cout << endl;
    }
}

void Player::rejectRequest(string id)
{
    if (friendRequest.search(id))
    {
        friendRequest.remove(id);
        noOfRequests--;
    }
    saveToFile();
}

void Player::acceptRequest(string id)
{
    // Remove from requests
    if (friendRequest.search(id))
    {
        friendRequest.remove(id);
        cout << "Removed by " << ID << endl;
        noOfRequests--;
    }
    // Add to friends list if not already present
    if (!friendList.search(id))
    {
        friendList.insert(id);
        noOfFriends++;
        cout << "Added by " << ID << endl;
    }
    saveToFile();
}

bool Player::changePassword(string newPassword)
{
    password = newPassword;
    saveToFile();
    return true;
}
