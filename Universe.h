#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "Cell.h"
#include <wx/colour.h>
#include <vector> // Add this for using std::vector

class Universe {
public:
    Universe(int width, int height);
    Universe();

    Cell& getCell(int x, int y);
    void setCellAlive(int x, int y, bool isAlive);
    void setCellColor(int x, int y, const wxColor& color);
    void initializeRandomUniverse();
    void play();
    bool getCellState(int x, int y) const;

private:
    std::vector<std::vector<Cell>> grid;
    std::vector<std::vector<Cell>> scratchPad; // Declare scratchPad

    static const int WIDTH = 100;
    static const int HEIGHT = 100;
    static const int MAX_GENERATIONS = 100;

    int countNeighbors(int x, int y) const;
    void printUniverse() const;

    int width;
    int height;
};

#endif // UNIVERSE_H
