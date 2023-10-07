#define _CRT_SECURE_NO_WARNINGS

#include "Universe.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <utility>  // For std::swap

Universe::Universe() {
    this->width = Universe::WIDTH;
    this->height = Universe::HEIGHT;
    grid.resize(Universe::HEIGHT, std::vector<Cell>(Universe::WIDTH));
    scratchPad.resize(Universe::HEIGHT, std::vector<Cell>(Universe::WIDTH));
}


void Universe::initializeRandomUniverse() {
    // Initialize the universe with random cells
    std::srand(static_cast<unsigned>(std::time(nullptr))); // Seed the random number generator
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            grid[i][j].setAlive(std::rand() % 3 == 0);
        }
    }
}

void Universe::play() {
    // Calculate the next generation
    for (int i = 0; i < Universe::HEIGHT; ++i) {
        for (int j = 0; j < Universe::WIDTH; ++j) {
            int neighbors = countNeighbors(i, j);
            if (grid[i][j].isAlive()) {
                // Apply rules for living cells
                if (neighbors < 2 || neighbors > 3) {
                    scratchPad[i][j].setAlive(false);
                }
                else {
                    scratchPad[i][j].setAlive(true);
                    scratchPad[i][j].incrementGenerationsAlive();
                }
            }
            else {
                // Apply rules for dead cells
                if (neighbors == 3) {
                    scratchPad[i][j].setAlive(true);
                }
                else {
                    scratchPad[i][j].setAlive(false);
                }
            }
        }
    }

    // Swap grid and scratch pad
    std::swap(grid, scratchPad);
}

int Universe::countNeighbors(int x, int y) const {
    int count = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;  // Skip the current cell
            int nx = (x + i + Universe::HEIGHT) % Universe::HEIGHT;  // Wrap around for toroidal behavior
            int ny = (y + j + Universe::WIDTH) % Universe::WIDTH;
            if (grid[nx][ny].isAlive()) {
                count++;
            }
        }
    }
    return count;
}

Cell& Universe::getCell(int x, int y) {
    return grid[y][x];
}

void Universe::setCellAlive(int x, int y, bool isAlive) {
    grid[y][x].setAlive(isAlive);
}

void Universe::setCellColor(int x, int y, const wxColor& color) {
    grid[y][x].setColor(color);
}

bool Universe::getCellState(int x, int y) const {
    return grid[y][x].isAlive();
}

void Universe::printUniverse() const {
    for (int i = 0; i < Universe::HEIGHT; ++i) {
        for (int j = 0; j < Universe::WIDTH; ++j) {
            if (grid[i][j].isAlive()) {
                std::cout << 'X'; // Print 'X' for living cells
            }
            else {
                std::cout << ' '; // Print a space for dead cells
            }
        }
        std::cout << std::endl; // Newline for the next row
    }
}

