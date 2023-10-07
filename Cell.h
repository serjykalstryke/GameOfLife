#pragma once
#include <wx/colour.h>
#ifndef CELL_H
#define CELL_H

class Cell {
public:
    Cell();

    bool isAlive() const;
    void setAlive(bool value);
    int getGenerationsAlive() const;
    void incrementGenerationsAlive();

    wxColor getColor() const;

    void setColor(const wxColor& newColor);


private:
    bool alive;
    int generationsAlive;
    wxColor color;
};

#endif // CELL_H
