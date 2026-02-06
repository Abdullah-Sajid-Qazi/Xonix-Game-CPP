# Xonix Game
# M.Asad (24I-2122), Abdullah Sajid (24I-2101)

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./header -I./data_structures
SFML_LIBS = -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system

SRC_DIR = src
OBJ_DIR = obj
TARGET = Xonix.exe

SOURCES = $(filter-out $(SRC_DIR)/Menu.cpp, $(wildcard $(SRC_DIR)/*.cpp))
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(SFML_LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

rebuild: clean all

-include $(OBJECTS:.o=.d)

.PHONY: all clean run rebuild
