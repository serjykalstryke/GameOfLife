#pragma once
#include <wx/colour.h>

class Cell {
public:
    // Default constructor initializing the cell's state
    Cell() : alive(false), generationsAlive(0), color(wxColour(0, 0, 0)) {} // Default to a dead cell with black color

    // Returns true if the cell is alive, false otherwise
    inline bool isAlive() const { return alive; }

    // Sets the cell's alive state to the given value
    inline void setAlive(bool value) { alive = value; }

    // Gets the number of generations the cell has been alive
    inline int getGenerationsAlive() const { return generationsAlive; }

    // Increments the number of generations the cell has been alive
    inline void incrementGenerationsAlive() { ++generationsAlive; }

    // Gets the color of the cell
    inline wxColour getCellColor() const { return color; }

    // Sets the color of the cell to the given value
    inline void setCellColor(const wxColour& value) { color = value; }

private:
    bool alive;            // Indicates whether the cell is alive
    int generationsAlive; // Number of generations the cell has been alive
    wxColour color;       // Color of the cell
};
