// Xonix Game
// Theme inventory using AVL Tree - self-balancing BST for efficient theme search
// Stores game themes with O(log n) operations

#pragma once
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

// AVL Tree node for storing theme data
class AVLNode
{
public:
    int ID;
    string name;
    string description;
    string CC;  // Color code
    string backgroundImagePath;
    AVLNode *left;
    AVLNode *right;
    int height;  // For AVL balancing

    AVLNode(int id, const string &n, const string &desc, const string &color, const string &imagePath = "");
};

// AVL Tree for theme management - self-balancing for O(log n) search
class ThemeInventory
{
private:
    AVLNode *root;
    int size;

    AVLNode *insert(AVLNode *node, int id, const string &name, const string &desc,
                    const string &color, const string &imagePath);
    AVLNode *search(AVLNode *node, int id);
    AVLNode *searchByName(AVLNode *node, const string &name);
    void display(AVLNode *node);
    void collectThemes(AVLNode *node, AVLNode **themes, int &index);

    int height(AVLNode *node);
    int max(int a, int b);
    
    // AVL rotations for balancing
    AVLNode *rotateRight(AVLNode *node);
    AVLNode *rotateLeft(AVLNode *node);
    AVLNode *balance(AVLNode *node);

public:
    ThemeInventory();
    ~ThemeInventory();

    void initializeThemes();
    void addTheme(int id, const string &name, const string &desc,
                  const string &color, const string &imagePath = "");
    void displayThemes();

    AVLNode *searchThemeByID(int id);
    AVLNode *searchThemeByName(const string &name);
    
    string getThemeColorCode(int id);
    string getThemeImagePath(int id);
    void browseThemes();
    int getSize() const;
    int getAllThemes(AVLNode **themes);
    Color getBackgroundColor(const string &color);
    
    bool savePlayerThemePreference(const string &playerID, int themeID);
    int loadPlayerThemePreference(const string &playerID);
};
