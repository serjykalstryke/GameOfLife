#pragma once

#include "Cell.h"
#include <vector>
#include <set>
#include <utility> // For std::pair
#include <wx/colour.h>
#include <map>

// Utility structure to represent coordinates on the grid
struct GridCoord {
    int x;
    int y;
    GridCoord(int xCoord, int yCoord) : x(xCoord), y(yCoord) {}
};

struct wxColourComparator {
    bool operator()(const wxColour& a, const wxColour& b) const {
        if (a.Red() < b.Red()) return true;
        if (a.Red() > b.Red()) return false;

        if (a.Green() < b.Green()) return true;
        if (a.Green() > b.Green()) return false;

        return a.Blue() < b.Blue();
    }
};

class Universe {
public:
    Universe(int width, int height);

    Cell& getCell(int x, int y);
    bool getCellState(int x, int y) const;
    void setCellAlive(int x, int y, bool alive);  // Overloaded version without color
    void setCellAlive(int x, int y, bool alive, wxColour color);  // Existing version with color

    void initializeRandomUniverse();
    void clearAll(const wxColour& clearColor);
    void play();
    void save(const std::string& filename, const wxColour& gridColor, const wxColour& backgroundColor);   
    bool load(const std::string& filename, wxColour& currentGridColor, wxColour& backgroundColor);
    void clearAll();
    wxColour getCellColor(int x, int y) const;
    void setCellColor(const GridCoord& coord, const wxColour& color);
    bool isWithinBounds(int x, int y) const;

    wxColour determineBirthColor(int x, int y);
    int countNeighbors(int x, int y) const;

    static const int GRID_WIDTH;
    static const int GRID_HEIGHT;

    inline static int getGridWidth() { return GRID_WIDTH; }
    inline static int getGridHeight() { return GRID_HEIGHT; }

    void resize(int newWidth, int newHeight);

    void setToroidal(bool toroidal) {
        isToroidal = toroidal;
    }

    bool getToroidal() const {
        return isToroidal;
    }

  

private:
    int width;
    int height;
    std::vector<std::vector<wxColour>> colors;
    std::vector<std::vector<bool>> cells;
    std::vector<std::vector<Cell>> grid;
    std::vector<std::vector<Cell>> scratchPad;

    bool isToroidal;

};
