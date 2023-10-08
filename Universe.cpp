#include "Universe.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <utility>  // For std::swap
#include <wx/wx.h>

const int Universe::GRID_WIDTH = 100;
const int Universe::GRID_HEIGHT = 100;

Universe::Universe(int width, int height)
    : width(width), height(height) {
    

    grid.assign(width, std::vector<Cell>(height));
    scratchPad.assign(width, std::vector<Cell>(height));
}

// Utility function to check boundaries
inline bool Universe::isWithinBounds(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Universe::getCellState(int x, int y) const {
    if (isWithinBounds(x, y)) {
        return grid[x][y].isAlive();
    }
    return false;  // or throw an exception
}

wxColour Universe::getCellColor(int x, int y) const {
    if (isWithinBounds(x, y)) {
        return grid[x][y].getCellColor();
    }
    return *wxBLACK;  // default or throw an exception
}

void Universe::setCellColor(const GridCoord& coord, const wxColour& color) {
    if (isWithinBounds(coord.x, coord.y)) {
        grid[coord.x][coord.y].setCellColor(color);
    }
    // else throw an exception or handle the error
}

void Universe::setCellAlive(int x, int y, bool alive) {
    if (isWithinBounds(x, y)) {
        grid[x][y].setAlive(alive);
    }
}

void Universe::setCellAlive(int x, int y, bool alive, wxColour color) {
    if (isWithinBounds(x, y)) {
        grid[x][y].setAlive(alive);
        grid[x][y].setCellColor(color);
    }
}



int Universe::countNeighbors(int x, int y) const {
    int count = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;  // Skip the cell itself

            int newX = x + i;
            int newY = y + j;

            // Check boundaries
            if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                if (grid[newX][newY].isAlive()) {
                    count++;
                }
            }
        }
    }
    return count;
}

wxColour Universe::determineBirthColor(int x, int y) {
    std::map<wxColour, int, wxColourComparator> colorCount;

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;  // Skip the cell itself

            int newX = x + i;
            int newY = y + j;

            // Check boundaries
            if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                if (grid[newX][newY].isAlive()) {
                    colorCount[grid[newX][newY].getCellColor()]++;
                }

            }
        }
    }

    // Find the most common color
    wxColour mostCommonColor = *wxBLACK;
    int maxCount = 0;
    for (const auto& pair : colorCount) {
        if (pair.second > maxCount) {
            mostCommonColor = pair.first;
            maxCount = pair.second;
        };
    }

    return mostCommonColor;
}


void Universe::initializeRandomUniverse() {
    srand(static_cast<unsigned>(time(0)));  // Seed the random number generator.
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            bool isAlive = rand() % 2 == 1;  // Randomly choose between alive (true) or dead (false).
            wxColour randomColor(rand() % 256, rand() % 256, rand() % 256);  // Generate a random color.
            setCellAlive(i, j, isAlive, randomColor);  // Set the cell's state and color.
        }
    }
}

void Universe::clearAll() {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            setCellAlive(i, j, false, *wxBLACK);  // Set each cell to dead and assign a default black color.
        }
    }
}

