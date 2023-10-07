#include "Cell.h"
#include <wx/wx.h>
#include <wx/gdicmn.h>

Cell::Cell() : alive(false), generationsAlive(0) {
    color = wxColour(255, 255, 255); // Default color (white)
}

bool Cell::isAlive() const {
    return alive;
}

void Cell::setAlive(bool value) {
    alive = value;
}

int Cell::getGenerationsAlive() const {
    return generationsAlive;
}

void Cell::incrementGenerationsAlive() {
    generationsAlive++;
}

// Get the cell's color
wxColour Cell::getColor() const {
    return color;
}

// Set the cell's color
void Cell::setColor(const wxColour& newColor) {
    color = newColor;
}
