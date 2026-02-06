// Xonix Game
// Theme Inventory - AVL Tree for balanced O(log n) theme lookups

#include "../header/Inventory.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace sf;

AVLNode::AVLNode(int id, const string &n, const string &desc, const string &color, const string &imagePath)
    : ID(id), name(n), description(desc), CC(color), backgroundImagePath(imagePath),
      left(nullptr), right(nullptr), height(1) {}

ThemeInventory::ThemeInventory() : root(nullptr), size(0)
{
    initializeThemes();
}

ThemeInventory::~ThemeInventory()
{
    while (root)
    {
        AVLNode *temp = root;
        root = root->left ? root->left : root->right;
        delete temp;
    }
}

int ThemeInventory::height(AVLNode *node)
{
    return node ? node->height : 0;
}

int ThemeInventory::max(int a, int b)
{
    return (a > b) ? a : b;
}

AVLNode *ThemeInventory::rotateRight(AVLNode *node)
{
    AVLNode *newRoot = node->left;
    node->left = newRoot->right;
    newRoot->right = node;
    node->height = max(height(node->left), height(node->right)) + 1;
    newRoot->height = max(height(newRoot->left), height(newRoot->right)) + 1;
    return newRoot;
}

AVLNode *ThemeInventory::rotateLeft(AVLNode *node)
{
    AVLNode *newRoot = node->right;
    node->right = newRoot->left;
    newRoot->left = node;
    node->height = max(height(node->left), height(node->right)) + 1;
    newRoot->height = max(height(newRoot->left), height(newRoot->right)) + 1;
    return newRoot;
}

AVLNode *ThemeInventory::balance(AVLNode *node)
{
    if (!node) return node;

    node->height = max(height(node->left), height(node->right)) + 1;
    int bf = height(node->left) - height(node->right);

    // Left-heavy
    if (bf > 1)
    {
        if (height(node->left->left) >= height(node->left->right))
            return rotateRight(node);  // LL case
        node->left = rotateLeft(node->left);
        return rotateRight(node);  // LR case
    }

    // Right-heavy
    if (bf < -1)
    {
        if (height(node->right->right) >= height(node->right->left))
            return rotateLeft(node);  // RR case
        node->right = rotateRight(node->right);
        return rotateLeft(node);  // RL case
    }

    return node;
}

AVLNode *ThemeInventory::insert(AVLNode *node, int id, const string &name, const string &desc,
                                const string &color, const string &imagePath)
{
    if (!node)
        return new AVLNode(id, name, desc, color, imagePath);

    if (id < node->ID)
        node->left = insert(node->left, id, name, desc, color, imagePath);
    else if (id > node->ID)
        node->right = insert(node->right, id, name, desc, color, imagePath);
    else
        return node;  // Duplicate

    return balance(node);
}

AVLNode *ThemeInventory::search(AVLNode *node, int id)
{
    if (node == nullptr || node->ID == id)
        return node;
    if (id < node->ID)
        return search(node->left, id);
    return search(node->right, id);
}

AVLNode *ThemeInventory::searchByName(AVLNode *node, const string &name)
{
    if (node == nullptr)
        return nullptr;
    if (node->name == name)
        return node;

    // Search left subtree
    AVLNode *result = searchByName(node->left, name);
    if (result)
        return result;

    // Search right subtree
    return searchByName(node->right, name);
}

void ThemeInventory::display(AVLNode *node)
{
    if (node == nullptr)
        return;
    display(node->left);
    cout << node->ID << "\t" << node->name << "\t" << node->description << "\t" << node->CC << endl;
    display(node->right);
}

void ThemeInventory::collectThemes(AVLNode *node, AVLNode **themes, int &index)
{
    if (node == nullptr)
        return;
    collectThemes(node->left, themes, index);
    themes[index++] = node;
    collectThemes(node->right, themes, index);
}

void ThemeInventory::initializeThemes()
{
    // Theme ID, Name, Description, Color Code, Background Image Path
    addTheme(1, "Forest", "A lush green forest theme.", "Green", "assets/backgrounds/Forest-background.jpg");
    addTheme(2, "Desert", "A sandy desert theme.", "Brown", "assets/backgrounds/desert-background.jpg");
    addTheme(3, "Ocean", "A deep blue ocean theme.", "Blue", "assets/backgrounds/Ocean-background.jpg");
    addTheme(4, "Mountain", "A rocky mountain theme.", "Gray", "assets/backgrounds/mountain-background.jpg");
    addTheme(5, "City", "A bustling city theme.", "Red", "assets/backgrounds/city-background.jpg");
}

void ThemeInventory::addTheme(int id, const string &name, const string &desc,
                              const string &color, const string &imagePath)
{
    root = insert(root, id, name, desc, color, imagePath);
    size++;
}

void ThemeInventory::displayThemes()
{
    cout << "ID\tName\t Description\t\t\tColor" << endl;
    cout << "=====================================================" << endl;
    display(root);
}

AVLNode *ThemeInventory::searchThemeByID(int id)
{
    return search(root, id);
}

AVLNode *ThemeInventory::searchThemeByName(const string &name)
{
    return searchByName(root, name);
}

string ThemeInventory::getThemeColorCode(int id)
{
    AVLNode *result = search(root, id);
    if (result)
    {
        return result->CC;
    }
    return "";
}

string ThemeInventory::getThemeImagePath(int id)
{
    AVLNode *result = search(root, id);
    if (result)
    {
        return result->backgroundImagePath;
    }
    return "";
}

void ThemeInventory::browseThemes()
{
    cout << "Available Themes: " << endl;
    displayThemes();
}

int ThemeInventory::getSize() const
{
    return size;
}

int ThemeInventory::getAllThemes(AVLNode **themes)
{
    int index = 0;
    collectThemes(root, themes, index);
    return index;
}

Color ThemeInventory::getBackgroundColor(const string &color)
{
    Color backgroundColor;

    if (color == "Green")
    { // Forest theme
        backgroundColor = Color(0, 100, 0);
    }
    else if (color == "Brown")
    { // Desert theme
        backgroundColor = Color(139, 119, 101);
    }
    else if (color == "Blue")
    { // Ocean theme
        backgroundColor = Color(0, 0, 128);
    }
    else if (color == "Gray")
    { // Mountain theme
        backgroundColor = Color(70, 70, 70);
    }
    else if (color == "Red")
    { // City theme
        backgroundColor = Color(139, 0, 0);
    }
    else
    { // Default
        backgroundColor = Color(20, 20, 20);
    }

    return backgroundColor;
}

bool ThemeInventory::savePlayerThemePreference(const string &playerID, int themeID)
{
    string filename = "data/" + playerID + ".txt";

    // First, read all existing data from the file
    ifstream inFile(filename);
    if (!inFile.is_open())
    {
        return false;
    }

    // Read basic player information
    string username, password, regTime;
    int highScore, highScoreLevel, powerUps;

    getline(inFile, username);
    getline(inFile, password);
    getline(inFile, regTime);
    inFile >> highScore;
    inFile >> highScoreLevel;
    inFile >> powerUps;

    // Read friends list
    int noOfFriends;
    inFile >> noOfFriends;
    inFile.ignore();
    string *friends = new string[noOfFriends > 0 ? noOfFriends : 1];
    for (int i = 0; i < noOfFriends; i++)
    {
        getline(inFile, friends[i]);
    }

    // Read match history
    int noOfMatches;
    inFile >> noOfMatches;
    inFile.ignore();
    string *matches = new string[noOfMatches > 0 ? noOfMatches : 1];
    for (int i = 0; i < noOfMatches; i++)
    {
        getline(inFile, matches[i]);
    }

    // Read friend requests
    int noOfRequests;
    inFile >> noOfRequests;
    inFile.ignore();
    string *requests = new string[noOfRequests > 0 ? noOfRequests : 1];
    for (int i = 0; i < noOfRequests; i++)
    {
        getline(inFile, requests[i]);
    }

    inFile.close();

    // Now rewrite the file with the updated theme
    ofstream outFile(filename);
    if (!outFile.is_open())
    {
        delete[] friends;
        delete[] matches;
        delete[] requests;
        return false;
    }

    outFile << username << endl;
    outFile << password << endl;
    outFile << regTime << endl;
    outFile << highScore << endl;
    outFile << highScoreLevel << endl;
    outFile << powerUps << endl;

    outFile << noOfFriends << endl;
    for (int i = 0; i < noOfFriends; i++)
    {
        outFile << friends[i] << endl;
    }

    outFile << noOfMatches << endl;
    for (int i = 0; i < noOfMatches; i++)
    {
        outFile << matches[i] << endl;
    }

    outFile << noOfRequests << endl;
    for (int i = 0; i < noOfRequests; i++)
    {
        outFile << requests[i] << endl;
    }

    // Write the new theme preference
    outFile << themeID << endl;

    outFile.close();

    delete[] friends;
    delete[] matches;
    delete[] requests;

    return true;
}

int ThemeInventory::loadPlayerThemePreference(const string &playerID)
{
    string filename = "data/" + playerID + ".txt";
    ifstream file(filename);
    if (!file.is_open())
    {
        return 1; // Default to Forest theme
    }

    // Read basic player information to skip to the theme
    string temp;
    int intTemp;

    getline(file, temp); // username
    getline(file, temp); // password
    getline(file, temp); // regTime
    file >> intTemp;     // highScore
    file >> intTemp;     // highScoreLevel
    file >> intTemp;     // powerUps

    // Read friends list count and skip friends
    int noOfFriends;
    file >> noOfFriends;
    file.ignore();
    for (int i = 0; i < noOfFriends; i++)
    {
        getline(file, temp);
    }

    // Read match history count and skip matches
    int noOfMatches;
    file >> noOfMatches;
    file.ignore();
    for (int i = 0; i < noOfMatches; i++)
    {
        getline(file, temp);
    }

    // Read friend requests count and skip requests
    int noOfRequests;
    file >> noOfRequests;
    file.ignore();
    for (int i = 0; i < noOfRequests; i++)
    {
        getline(file, temp);
    }

    // Read the theme preference
    int themeID = 1; // Default value
    if (file >> themeID)
    {
        if (themeID >= 1 && themeID <= size)
        {
            file.close();
            return themeID;
        }
    }

    file.close();
    return 1; // Default to Forest theme
}
