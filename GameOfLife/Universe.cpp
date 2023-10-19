#include "Universe.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <utility>  // For std::swap
#include <wx/wx.h>
#include <fstream>

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

    // Define the relative positions of all 8 neighbors
    int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (getToroidal()) {
            // Adjust nx and ny to wrap around the grid toroidally
            nx = (nx + GRID_WIDTH) % GRID_WIDTH;
            ny = (ny + GRID_HEIGHT) % GRID_HEIGHT;
        }

        if (isWithinBounds(nx, ny) && getCellState(nx, ny)) {
            count++;
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

            // Adjust for toroidal boundary wrapping
            newX = (newX + GRID_WIDTH) % GRID_WIDTH;
            newY = (newY + GRID_HEIGHT) % GRID_HEIGHT;

            // Check boundaries
            if (grid[newX][newY].isAlive()) {
                colorCount[grid[newX][newY].getCellColor()]++;
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

void Universe::clearAll(const wxColour& clearColor) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            setCellAlive(i, j, false, clearColor);  // Set each cell to dead and assign the clear color.
        }
    }
}

void Universe::resize(int newWidth, int newHeight) {
    // Resize the main grid and the scratchPad
    grid.resize(newWidth, std::vector<Cell>(newHeight, Cell())); // Assuming Cell has a default constructor
    scratchPad.resize(newWidth, std::vector<Cell>(newHeight, Cell()));

    // Update the width and height
    width = newWidth;
    height = newHeight;
}





void Universe::save(const std::string& filename, const wxColour& currentGridColor, const wxColour& backgroundColor) {
    std::ofstream outFile(filename, std::ios::binary);
    if (outFile.is_open()) {
        // Write the width and height first
        outFile.write(reinterpret_cast<char*>(&width), sizeof(int));
        outFile.write(reinterpret_cast<char*>(&height), sizeof(int));

        // Write each cell's state and color
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                bool alive = grid[i][j].isAlive();
                int generations = grid[i][j].getGenerationsAlive();
                wxColour color = grid[i][j].getCellColor();

                outFile.write(reinterpret_cast<char*>(&alive), sizeof(bool));
                outFile.write(reinterpret_cast<char*>(&generations), sizeof(int));

                // Serializing wxColour
                unsigned char red = color.Red();
                unsigned char green = color.Green();
                unsigned char blue = color.Blue();
                outFile.write(reinterpret_cast<char*>(&red), sizeof(unsigned char));
                outFile.write(reinterpret_cast<char*>(&green), sizeof(unsigned char));
                outFile.write(reinterpret_cast<char*>(&blue), sizeof(unsigned char));
            }
        }

        // After saving all cells, save the grid color and background color
        unsigned char r, g, b;

        r = currentGridColor.Red(); g = currentGridColor.Green(); b = currentGridColor.Blue();
        outFile.write(reinterpret_cast<char*>(&r), sizeof(unsigned char));
        outFile.write(reinterpret_cast<char*>(&g), sizeof(unsigned char));
        outFile.write(reinterpret_cast<char*>(&b), sizeof(unsigned char));

        r = backgroundColor.Red(); g = backgroundColor.Green(); b = backgroundColor.Blue();
        outFile.write(reinterpret_cast<char*>(&r), sizeof(unsigned char));
        outFile.write(reinterpret_cast<char*>(&g), sizeof(unsigned char));
        outFile.write(reinterpret_cast<char*>(&b), sizeof(unsigned char));

        outFile.close();
    }
    else {
        // Handle the error (e.g., could not open file for writing)
        // Perhaps throw an exception or return a special value indicating the save failed.
    }
}

bool Universe::load(const std::string& filename, wxColour& gridColor, wxColour& backgroundColor) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile.is_open()) {
        return false; // File could not be opened
    }

    // Read width and height
    inFile.read(reinterpret_cast<char*>(&width), sizeof(int));
    inFile.read(reinterpret_cast<char*>(&height), sizeof(int));

    // Resize the grid to match the loaded dimensions
    grid.resize(width, std::vector<Cell>(height));

    // Read each cell's state and color and update the grid
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            bool alive;
            int generations;
            unsigned char red, green, blue;

            inFile.read(reinterpret_cast<char*>(&alive), sizeof(bool));
            inFile.read(reinterpret_cast<char*>(&generations), sizeof(int));

            // Deserializing wxColour
            inFile.read(reinterpret_cast<char*>(&red), sizeof(unsigned char));
            inFile.read(reinterpret_cast<char*>(&green), sizeof(unsigned char));
            inFile.read(reinterpret_cast<char*>(&blue), sizeof(unsigned char));
            wxColour color(red, green, blue);

            grid[i][j].setAlive(alive);
            grid[i][j].setCellColor(color);
            // Set generations as needed, for example using a private setter or a friend function
            // grid[i][j].setGenerationsAlive(generations); // This method might need to be implemented if it doesn't exist.
        }
    }

    // After loading all cells, load the grid color and background color
    unsigned char r, g, b;

    // Deserializing gridColor
    inFile.read(reinterpret_cast<char*>(&r), sizeof(unsigned char));
    inFile.read(reinterpret_cast<char*>(&g), sizeof(unsigned char));
    inFile.read(reinterpret_cast<char*>(&b), sizeof(unsigned char));
    gridColor = wxColour(r, g, b);

    // Deserializing backgroundColor
    inFile.read(reinterpret_cast<char*>(&r), sizeof(unsigned char));
    inFile.read(reinterpret_cast<char*>(&g), sizeof(unsigned char));
    inFile.read(reinterpret_cast<char*>(&b), sizeof(unsigned char));
    backgroundColor = wxColour(r, g, b);

    inFile.close();
    return true;
}
