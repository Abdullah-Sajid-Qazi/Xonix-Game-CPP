# Xonix Game

A modern implementation of the classic Xonix arcade game built with C++ and SFML, featuring custom data structures, multiplayer support, and comprehensive game management systems.

## � Authors

- **M.Asad** (24I-2122)
- **Abdullah Sajid** (24I-2101)

## �📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Data Structures](#data-structures)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Building the Project](#building-the-project)
- [How to Play](#how-to-play)
- [Project Structure](#project-structure)
- [Game Modes](#game-modes)
- [Technical Details](#technical-details)
- [Contributing](#contributing)

## 🎮 Overview

Xonix is a classic arcade game where players control a marker on a rectangular grid, attempting to claim territory by drawing lines. This implementation features custom data structures and demonstrates efficient game development practices.

The game includes:
- Single-player mode with save/load functionality
- Multiplayer mode with matchmaking
- User authentication system
- Friend management
- Leaderboard system
- Inventory system
- Custom implementations of fundamental data structures

## ✨ Features

### Core Features
- **Single Player Mode**: Play solo with progressive difficulty
- **Multiplayer Mode**: Real-time multiplayer gameplay with matchmaking queue
- **User Authentication**: Secure login and registration system
- **Friend Management**: Add, remove, and manage friends
- **Leaderboard**: Track high scores and compete with other players
- **Inventory System**: Collect and manage in-game items
- **Save/Load System**: Save your progress and resume later
- **Audio System**: Background music with toggle controls

### Technical Features
- Custom data structure implementations (no STL)
- Hash table for efficient player data management
- Linked list for tile path tracking
- Stack-based operations
- File-based persistence
- SFML graphics and audio integration

## 🔧 Data Structures

The project implements custom data structures from scratch:

1. **Hash Table** (`HashTable.h`)
   - Used for player authentication and data management
   - Custom hash function implementation
   - Collision handling

2. **Linked List** (`List.h`)
   - TileList for tracking player movement path
   - Dynamic memory management
   - Efficient insertion and deletion

3. **Stack** (`Stack.h`)
   - Used for various game state management
   - LIFO operations

## 📦 Prerequisites

Before building the project, ensure you have the following installed:

- **C++ Compiler**: g++ with C++17 support
- **SFML Library**: Version 2.5 or higher
  - SFML Graphics
  - SFML Audio
  - SFML Window
  - SFML System
- **Make**: For using the Makefile

### Windows (MSYS2)
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-sfml
pacman -S make
```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install g++ make
sudo apt-get install libsfml-dev
```

### macOS
```bash
brew install sfml
brew install gcc make
```

## 🚀 Installation

1. **Clone the repository**
   ```bash
   git clone git@github.com:Abdullah-Sajid-Qazi/Xonix-Game-CPP.git
   cd Xonix-Game-CPP
   ```

2. **Ensure assets are present**
   - The `assets/` directory should contain:
     - `backgrounds/` - Game background images
     - `fonts/` - Font files for text rendering
     - `images/` - Game sprites and UI elements
     - `Sounds/` - Audio files including Background_Audio.mp3

3. **Set up data directories**
   - `data/` - Player data files
   - `data/saves/` - Game save files

## 🔨 Building the Project

### Using Make (Recommended)

```bash
# Build the project
make

# Build and run
make run

# Clean build files
make clean

# Rebuild from scratch
make rebuild
```

### Manual Compilation

If you need to modify the Makefile or compile manually:

```bash
# Create object directory
mkdir -p obj

# Compile source files
g++ -std=c++17 -Wall -Wextra -I./header -I./data_structures -c src/*.cpp

# Link and create executable
g++ obj/*.o -o Xonix.exe -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
```

**Note**: Update the SFML include and library paths in the Makefile to match your system configuration.

## 🎯 How to Play

### Starting the Game
1. Run the executable: `./Xonix.exe` or `make run`
2. Register a new account or login with existing credentials
3. Choose your game mode:
   - **Single Player**: Play alone and beat your high score
   - **Multiplayer**: Join the matchmaking queue and compete with others

### Gameplay
- **Objective**: Claim territory by moving across the grid
- **Controls**: Use arrow keys or WASD to move
- **Strategy**: 
  - Navigate from safe zones to unclaimed territory
  - Complete your path to claim the enclosed area
  - Avoid enemies while drawing your path
  - Return to safe territory to secure your claim

### Game Mechanics
- Start from the safe border
- Move into unclaimed territory to draw your path
- Complete the loop to capture area
- Score points based on territory claimed
- Avoid collision with enemies
- Use power-ups strategically

## 📁 Project Structure

```
.
├── Makefile              # Build configuration
├── README.md             # Project documentation
├── assets/               # Game resources
│   ├── backgrounds/      # Background images
│   ├── fonts/            # Font files
│   ├── images/           # Sprites and UI elements
│   └── Sounds/           # Audio files
├── data/                 # Player and game data
│   ├── AllPlayers.txt    # Player database
│   ├── [0-5].txt         # Individual player files
│   └── saves/            # Save game files
├── data_structures/      # Custom data structure implementations
│   ├── HashTable.h       # Hash table for player management
│   ├── List.h            # Linked list implementation
│   └── Stack.h           # Stack implementation
├── header/               # Header files
│   ├── Authentication.h  # User authentication
│   ├── Constants.h       # Game constants
│   ├── FriendManagement.h# Friend system
│   ├── Game.h            # Core game logic
│   ├── Inventory.h       # Item management
│   ├── LeaderBoard.h     # Score tracking
│   ├── MatchmakingQueue.h# Multiplayer matchmaking
│   ├── Multiplayer.h     # Multiplayer gameplay
│   ├── Player.h          # Player data structures
│   └── System.h          # System utilities
├── src/                  # Source files
│   ├── main.cpp          # Entry point
│   ├── Authentication.cpp
│   ├── FriendManagement.cpp
│   ├── Game.cpp
│   ├── HashTable.cpp
│   ├── Inventory.cpp
│   ├── LeaderBoard.cpp
│   ├── MatchmakingQueue.cpp
│   ├── Multiplayer.cpp
│   ├── Player.cpp
│   └── System.cpp
└── obj/                  # Compiled object files (generated)
```

## 🎮 Game Modes

### Single Player
- Progressive difficulty levels
- Save and load game functionality
- Personal high score tracking
- Multiple save slots
- Pause and resume gameplay

### Multiplayer
- Real-time competitive gameplay
- Matchmaking queue system
- Player vs Player territory claiming
- Live score comparison
- Friend challenges

##  Technical Details

### Grid System
- **Dimensions**: 25 rows × 40 columns (configurable in `Constants.h`)
- **Tile States**: Safe, Claimed, Path, Enemy
- **Rendering**: SFML graphics with custom sprites

### Save System
- Binary file format for efficient storage
- Metadata: SaveID, Timestamp, PlayerID
- State preservation: Grid state, player position, score, lives
- Multiple save slots per player
- Automatic save indexing

### Authentication System
- Hash-based password storage
- Secure login/registration
- Player profile management
- Session management

### Performance
- Efficient collision detection
- Optimized rendering pipeline
- Minimal memory footprint
- Custom data structures for better control

## 🤝 Contributing

Contributions and suggestions are welcome!

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Coding Standards
- Follow existing code style and formatting
- Comment complex algorithms
- Update documentation for new features
- Test thoroughly before submitting

## 🙏 Acknowledgments

- Classic Xonix game for inspiration
- SFML library for graphics and audio

## 📧 Contact

For questions, suggestions, or issues:
- Open an issue on GitHub

## 🔮 Future Enhancements

- [ ] Online multiplayer with server
- [ ] Power-ups and special abilities
- [ ] Multiple difficulty levels
- [ ] Tournament mode
- [ ] Mobile platform support
- [ ] Replay system
- [ ] Achievement system
- [ ] Custom themes and skins