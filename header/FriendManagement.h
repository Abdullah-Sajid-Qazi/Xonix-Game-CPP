// Xonix Game
// Friend management GUI - uses Hash Table for username search, Linked List for friends, Stack for requests

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "System.h"

using namespace sf;
using namespace std;

class FriendManagement
{
private:
    RenderWindow &window;
    Font font;

    Texture backgroundTexture;
    Sprite backgroundSprite;

    Text friendsTitle;
    Text requestsTitle;
    Text searchLabel;
    Text searchInput;
    Text statusMessage;
    Text instructionsText;

    string playerId;

    // Dynamic arrays for displaying friends and requests
    string *friends;
    string *requests;
    Text *friendTexts;
    Text *requestTexts;

    // Scrolling state
    int friendOffset;
    int requestOffset;
    const int visibleLimit;

    System system;
    Player *currentPlayer;

    bool isSearching;
    string searchText;
    bool isFriendMenuOpen;
    int selectedRequest;

    void cleanup();
    void initializeArrays();
    void setupUI();
    void refreshPlayerData();
    void handleInput(const Event &event);
    void handleSearchInput(const Event &event);
    void handleMenuInput(const Event &event);
    void sendFriendRequest();
    void acceptRequest();
    void rejectRequest();
    void renderFriendList();
    void renderRequestList();
    void render();
    int min(int a, int b);

public:
    FriendManagement(RenderWindow &win, string &id);
    ~FriendManagement();

    void run();
};
