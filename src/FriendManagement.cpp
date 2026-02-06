// Xonix Game
// Friend management - uses Hash Table for username search, Linked List for friends, Stack for requests

#include "../header/FriendManagement.h"
#include <iostream>

using namespace sf;
using namespace std;

FriendManagement::FriendManagement(RenderWindow &win, string &id)
    : window(win), playerId(id), friends(nullptr), requests(nullptr),
      friendTexts(nullptr), requestTexts(nullptr), friendOffset(0),
      requestOffset(0), visibleLimit(8), currentPlayer(nullptr),
      isSearching(false), searchText(""), isFriendMenuOpen(true), selectedRequest(0)
{
    if (!font.loadFromFile("assets/fonts/arial.ttf"))
        cout << "Failed to load arial font\n";

    if (!backgroundTexture.loadFromFile("assets/images/FriendsBackground.png"))
        cout << "Failed to load friends background image\n";
    backgroundSprite.setTexture(backgroundTexture);

    currentPlayer = system.returnPlayer(playerId);
    if (!currentPlayer)
    {
        cout << "Error: Could not load player data\n";
        isFriendMenuOpen = false;
        return;
    }

    initializeArrays();
    setupUI();
}

FriendManagement::~FriendManagement()
{
    cleanup();
    if (currentPlayer)
    {
        delete currentPlayer;
    }
}

void FriendManagement::cleanup()
{
    if (friendTexts)
        delete[] friendTexts;
    if (requestTexts)
        delete[] requestTexts;
    if (friends)
        delete[] friends;
    if (requests)
        delete[] requests;
    friendTexts = nullptr;
    requestTexts = nullptr;
    friends = nullptr;
    requests = nullptr;
}

void FriendManagement::initializeArrays()
{
    // Allocate arrays (minimum size 1 to avoid issues)
    int friendCount = currentPlayer->noOfFriends > 0 ? currentPlayer->noOfFriends : 1;
    int requestCount = currentPlayer->noOfRequests > 0 ? currentPlayer->noOfRequests : 1;

    friendTexts = new Text[friendCount];
    requestTexts = new Text[requestCount];
    friends = new string[friendCount];
    requests = new string[requestCount];

    // Copy friends from linked list to array
    NodeList *curr = currentPlayer->friendList.head;
    for (int i = 0; curr && i < currentPlayer->noOfFriends; i++)
    {
        friends[i] = curr->data;
        curr = curr->next;
    }

    // Copy requests from stack to array
    node *reqCurr = currentPlayer->friendRequest.head;
    for (int i = 0; reqCurr && i < currentPlayer->noOfRequests; i++)
    {
        requests[i] = reqCurr->data;
        reqCurr = reqCurr->next;
    }
}

void FriendManagement::setupUI()
{
    // Friends section title
    friendsTitle.setFont(font);
    friendsTitle.setString("Your Friends (" + to_string(currentPlayer->noOfFriends) + ")");
    friendsTitle.setCharacterSize(22);
    friendsTitle.setFillColor(Color::Green);
    friendsTitle.setPosition(30, 70);

    // Requests section title
    requestsTitle.setFont(font);
    requestsTitle.setString("Pending Requests (" + to_string(currentPlayer->noOfRequests) + ")");
    requestsTitle.setCharacterSize(22);
    requestsTitle.setFillColor(Color::Magenta);
    requestsTitle.setPosition(400, 70);

    // Search label
    searchLabel.setFont(font);
    searchLabel.setString("Search Username to Add Friend:");
    searchLabel.setCharacterSize(18);
    searchLabel.setFillColor(Color::White);
    searchLabel.setPosition(30, 420);

    // Search input
    searchInput.setFont(font);
    searchInput.setCharacterSize(20);
    searchInput.setFillColor(Color::Yellow);
    searchInput.setPosition(30, 450);
    searchInput.setString("_");

    // Status message
    statusMessage.setFont(font);
    statusMessage.setCharacterSize(18);
    statusMessage.setFillColor(Color::White);
    statusMessage.setPosition(30, 490);

    // Instructions
    instructionsText.setFont(font);
    instructionsText.setString(
        "Controls: [TAB] Search | [A] Accept Request | [R] Reject Request | [UP/DOWN] Scroll | [ESC] Back");
    instructionsText.setCharacterSize(14);
    instructionsText.setFillColor(Color(150, 150, 150));
    instructionsText.setPosition(30, 530);
}

void FriendManagement::refreshPlayerData()
{
    // Reload player data from file
    if (currentPlayer)
    {
        delete currentPlayer;
    }
    currentPlayer = system.returnPlayer(playerId);

    // Reinitialize arrays
    cleanup();
    initializeArrays();

    // Update UI titles
    friendsTitle.setString("Your Friends (" + to_string(currentPlayer->noOfFriends) + ")");
    requestsTitle.setString("Pending Requests (" + to_string(currentPlayer->noOfRequests) + ")");

    // Reset selection if out of bounds
    if (selectedRequest >= currentPlayer->noOfRequests)
    {
        selectedRequest = currentPlayer->noOfRequests > 0 ? currentPlayer->noOfRequests - 1 : 0;
    }
}

void FriendManagement::run()
{
    while (window.isOpen() && isFriendMenuOpen)
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            handleInput(event);
        }
        render();
    }
}

void FriendManagement::handleInput(const Event &event)
{
    if (isSearching)
    {
        handleSearchInput(event);
    }
    else
    {
        handleMenuInput(event);
    }
}

void FriendManagement::handleSearchInput(const Event &event)
{
    if (event.type == Event::TextEntered)
    {
        if (event.text.unicode == '\b')
        {
            // Backspace
            if (!searchText.empty())
            {
                searchText.pop_back();
            }
            searchInput.setString(searchText + "_");
        }
        else if (event.text.unicode == '\r' || event.text.unicode == '\n')
        {
            // Enter - send friend request
            if (!searchText.empty())
            {
                sendFriendRequest();
            }
            isSearching = false;
            searchInput.setString("_");
        }
        else if (event.text.unicode == 27)
        {
            // Escape - cancel search
            isSearching = false;
            searchText.clear();
            searchInput.setString("_");
            statusMessage.setString("");
        }
        else if (event.text.unicode >= 32 && event.text.unicode < 127)
        {
            // Regular character
            if (searchText.length() < 20)
            {
                searchText += static_cast<char>(event.text.unicode);
            }
            searchInput.setString(searchText + "_");
        }
    }
}

void FriendManagement::handleMenuInput(const Event &event)
{
    if (event.type == Event::KeyPressed)
    {
        switch (event.key.code)
        {
        case Keyboard::Tab:
            // Start search mode
            isSearching = true;
            searchText.clear();
            searchInput.setString("_");
            statusMessage.setString("Enter username and press Enter...");
            statusMessage.setFillColor(Color::Yellow);
            break;

        case Keyboard::Up:
            // Scroll friends list up
            if (friendOffset > 0)
            {
                friendOffset--;
            }
            break;

        case Keyboard::Down:
            // Scroll friends list down
            if (friendOffset + visibleLimit < currentPlayer->noOfFriends)
            {
                friendOffset++;
            }
            break;

        case Keyboard::Left:
            // Select previous request
            if (selectedRequest > 0)
            {
                selectedRequest--;
            }
            break;

        case Keyboard::Right:
            // Select next request
            if (selectedRequest < currentPlayer->noOfRequests - 1)
            {
                selectedRequest++;
            }
            break;

        case Keyboard::A:
            // Accept selected request
            acceptRequest();
            break;

        case Keyboard::R:
            // Reject selected request
            rejectRequest();
            break;

        case Keyboard::Escape:
            // Exit friend management
            isFriendMenuOpen = false;
            break;

        default:
            break;
        }
    }
}

void FriendManagement::sendFriendRequest()
{
    if (searchText.empty())
    {
        statusMessage.setString("Please enter a username.");
        statusMessage.setFillColor(Color::Red);
        return;
    }

    // Check if trying to add self
    if (searchText == currentPlayer->username)
    {
        statusMessage.setString("You cannot add yourself as a friend!");
        statusMessage.setFillColor(Color::Red);
        searchText.clear();
        return;
    }

    // Use hash table to find player by username (O(1) lookup)
    string targetId = system.returnIdByUsername(searchText);

    if (targetId.empty())
    {
        statusMessage.setString("User '" + searchText + "' not found!");
        statusMessage.setFillColor(Color::Red);
        searchText.clear();
        return;
    }

    // Check if already friends (using linked list)
    if (currentPlayer->friendList.search(targetId))
    {
        statusMessage.setString("You are already friends with " + searchText + "!");
        statusMessage.setFillColor(Color::Yellow);
        searchText.clear();
        return;
    }

    // Check if request already sent (check target's pending requests)
    Player *targetPlayer = system.returnPlayer(targetId);
    if (targetPlayer)
    {
        if (targetPlayer->friendRequest.search(playerId))
        {
            statusMessage.setString("Friend request already pending!");
            statusMessage.setFillColor(Color::Yellow);
            delete targetPlayer;
            searchText.clear();
            return;
        }
        delete targetPlayer;
    }

    // Send the friend request
    if (system.sendFriendRequest(playerId, targetId))
    {
        statusMessage.setString("Friend request sent to " + searchText + "!");
        statusMessage.setFillColor(Color::Green);
    }
    else
    {
        statusMessage.setString("Failed to send friend request.");
        statusMessage.setFillColor(Color::Red);
    }

    searchText.clear();
}

void FriendManagement::acceptRequest()
{
    if (currentPlayer->noOfRequests == 0)
    {
        statusMessage.setString("No pending requests to accept.");
        statusMessage.setFillColor(Color::Yellow);
        return;
    }

    if (selectedRequest >= currentPlayer->noOfRequests)
    {
        selectedRequest = 0;
    }

    string requesterId = requests[selectedRequest];

    // Accept the request (updates both players' friend lists)
    if (system.acceptFriendRequest(playerId, requesterId))
    {
        // Get requester's username for display
        Player *requester = system.returnPlayer(requesterId);
        string requesterName = requester ? requester->username : requesterId;
        if (requester)
            delete requester;

        statusMessage.setString("You are now friends with " + requesterName + "!");
        statusMessage.setFillColor(Color::Green);

        // Refresh player data
        refreshPlayerData();
    }
    else
    {
        statusMessage.setString("Failed to accept request.");
        statusMessage.setFillColor(Color::Red);
    }
}

void FriendManagement::rejectRequest()
{
    if (currentPlayer->noOfRequests == 0)
    {
        statusMessage.setString("No pending requests to reject.");
        statusMessage.setFillColor(Color::Yellow);
        return;
    }

    if (selectedRequest >= currentPlayer->noOfRequests)
    {
        selectedRequest = 0;
    }

    string requesterId = requests[selectedRequest];

    // Reject the request
    if (system.rejectFriendRequest(playerId, requesterId))
    {
        statusMessage.setString("Friend request rejected.");
        statusMessage.setFillColor(Color::White);

        // Refresh player data
        refreshPlayerData();
    }
    else
    {
        statusMessage.setString("Failed to reject request.");
        statusMessage.setFillColor(Color::Red);
    }
}

void FriendManagement::renderFriendList()
{
    float startY = 100.f;
    float spacing = 30.f;

    if (currentPlayer->noOfFriends == 0)
    {
        Text noFriends;
        noFriends.setFont(font);
        noFriends.setString("No friends yet. Add some!");
        noFriends.setCharacterSize(16);
        noFriends.setFillColor(Color(150, 150, 150));
        noFriends.setPosition(30, startY);
        window.draw(noFriends);
        return;
    }

    for (int i = 0; i < visibleLimit && friendOffset + i < currentPlayer->noOfFriends; i++)
    {
        string friendId = friends[friendOffset + i];

        // Get friend's username
        Player *friendPlayer = system.returnPlayer(friendId);
        string displayName = friendPlayer ? friendPlayer->username : friendId;
        if (friendPlayer)
            delete friendPlayer;

        friendTexts[i].setFont(font);
        friendTexts[i].setString(to_string(friendOffset + i + 1) + ". " + displayName);
        friendTexts[i].setCharacterSize(18);
        friendTexts[i].setFillColor(Color::Green);
        friendTexts[i].setPosition(30, startY + i * spacing);
        window.draw(friendTexts[i]);
    }

    // Show scroll indicator if needed
    if (currentPlayer->noOfFriends > visibleLimit)
    {
        Text scrollIndicator;
        scrollIndicator.setFont(font);
        scrollIndicator.setString("[" + to_string(friendOffset + 1) + "-" +
                                  to_string(min(friendOffset + visibleLimit, currentPlayer->noOfFriends)) +
                                  " of " + to_string(currentPlayer->noOfFriends) + "]");
        scrollIndicator.setCharacterSize(14);
        scrollIndicator.setFillColor(Color(100, 100, 100));
        scrollIndicator.setPosition(30, startY + visibleLimit * spacing);
        window.draw(scrollIndicator);
    }
}

void FriendManagement::renderRequestList()
{
    float startY = 100.f;
    float spacing = 30.f;

    if (currentPlayer->noOfRequests == 0)
    {
        Text noRequests;
        noRequests.setFont(font);
        noRequests.setString("No pending requests.");
        noRequests.setCharacterSize(16);
        noRequests.setFillColor(Color(150, 150, 150));
        noRequests.setPosition(400, startY);
        window.draw(noRequests);
        return;
    }

    for (int i = 0; i < visibleLimit && requestOffset + i < currentPlayer->noOfRequests; i++)
    {
        string requesterId = requests[requestOffset + i];

        // Get requester's username
        Player *requesterPlayer = system.returnPlayer(requesterId);
        string displayName = requesterPlayer ? requesterPlayer->username : requesterId;
        if (requesterPlayer)
            delete requesterPlayer;

        requestTexts[i].setFont(font);
        requestTexts[i].setString(to_string(requestOffset + i + 1) + ". " + displayName);
        requestTexts[i].setCharacterSize(18);

        // Highlight selected request
        if (requestOffset + i == selectedRequest)
        {
            requestTexts[i].setFillColor(Color::Yellow);
            requestTexts[i].setString("> " + displayName + " <");
        }
        else
        {
            requestTexts[i].setFillColor(Color::Magenta);
        }

        requestTexts[i].setPosition(400, startY + i * spacing);
        window.draw(requestTexts[i]);
    }
}

void FriendManagement::render()
{
    window.clear(Color(30, 30, 40));

    // Draw background
    window.draw(backgroundSprite);

    // Draw titles
    window.draw(friendsTitle);
    window.draw(requestsTitle);

    // Draw lists
    renderFriendList();
    renderRequestList();

    // Draw search section
    window.draw(searchLabel);
    window.draw(searchInput);
    window.draw(statusMessage);
    window.draw(instructionsText);

    // Draw search mode indicator
    if (isSearching)
    {
        RectangleShape searchBox(Vector2f(300, 30));
        searchBox.setPosition(28, 448);
        searchBox.setFillColor(Color::Transparent);
        searchBox.setOutlineColor(Color::Yellow);
        searchBox.setOutlineThickness(2);
        window.draw(searchBox);
    }

    window.display();
}

int FriendManagement::min(int a, int b)
{
    return a < b ? a : b;
}
