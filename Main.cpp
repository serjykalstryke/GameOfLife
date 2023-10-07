#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "Universe.h"
#include <wx/app.h>

#include <wx/dcbuffer.h> // Ensure this is included at the top of your file.

class GameOfLifeFrame : public wxFrame {
public:
    GameOfLifeFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnStart(wxCommandEvent& event);
    void OnDrawCell(wxMouseEvent& event);

    void OnPaint(wxPaintEvent& event);

    void OnInsertGlider(wxCommandEvent& event);

    ~GameOfLifeFrame();



private:

    struct Cell {
        bool alive;
        wxColour color;

        Cell() : alive(false), color(wxColour(0, 0, 0)) {} // Default is black and dead
    };

    static const int GRID_WIDTH = 100;
    static const int GRID_HEIGHT = 100;

    Universe universe;

    wxPanel* canvas;

    wxButton* startButton;
    wxButton* insertGliderButton;


    wxTimer* timer;

    void OnTimer(wxTimerEvent& event);

    wxBitmap bufferBitmap;

    Cell grid[GRID_WIDTH][GRID_HEIGHT];
    wxColour currentColor = *wxBLACK;  // default color
};

GameOfLifeFrame::GameOfLifeFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size), grid{} {

    // Fill our local grid based on the Universe class.
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            Cell grid[GRID_WIDTH][GRID_HEIGHT];

        }
    }

    // Create a canvas for drawing
    canvas = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    canvas->SetBackgroundStyle(wxBG_STYLE_PAINT);
    
    canvas->Bind(wxEVT_LEFT_DOWN, &GameOfLifeFrame::OnDrawCell, this);

    // Create a Start button
    startButton = new wxButton(this, wxID_ANY, "Start");
    startButton->Bind(wxEVT_BUTTON, &GameOfLifeFrame::OnStart, this);

    // Layout the controls
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(canvas, 1, wxEXPAND);
    sizer->Add(startButton, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    SetSizer(sizer);



    // Manually set the size of the frame
    SetSize(size);

    // Bind the paint event to the OnPaint function
    canvas->Bind(wxEVT_PAINT, &GameOfLifeFrame::OnPaint, this);

    // Bind timer event

    timer = new wxTimer(this);

    Bind(wxEVT_TIMER, &GameOfLifeFrame::OnTimer, this, timer->GetId());

    // Add glider to grid
    insertGliderButton = new wxButton(this, wxID_ANY, "Insert Glider");
    sizer->Add(insertGliderButton, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    // Bind glider button to glider event
    insertGliderButton->Bind(wxEVT_BUTTON, &GameOfLifeFrame::OnInsertGlider, this);




}

GameOfLifeFrame::~GameOfLifeFrame() {
    if (timer) {
        timer->Stop();
        delete timer;
    }
}

void GameOfLifeFrame::OnStart(wxCommandEvent& event) {
    if (timer->IsRunning()) {
        timer->Stop();
        startButton->SetLabel("Start");
    }
    else {
        timer->Start(100); // Start the timer with an interval of 100ms
        startButton->SetLabel("Stop");
    }
}



void GameOfLifeFrame::OnDrawCell(wxMouseEvent& event) {
    int cellWidth = canvas->GetSize().GetWidth() / GRID_WIDTH;
    int cellHeight = canvas->GetSize().GetHeight() / GRID_HEIGHT;

    int x = event.GetX() / cellWidth;  // Compute the column based on mouse x-coordinate
    int y = event.GetY() / cellHeight; // Compute the row based on mouse y-coordinate

    // Toggle the cell's state in the local grid
    grid[x][y].alive = !grid[x][y].alive;


    // Synchronize the change with the Universe class's grid
    universe.setCellAlive(x, y, grid[x][y].alive);

    int topLeftX = x * cellWidth;
    int topLeftY = y * cellHeight;

    canvas->RefreshRect(wxRect(topLeftX, topLeftY, cellWidth, cellHeight));
}


void GameOfLifeFrame::OnPaint(wxPaintEvent& event) {
    wxBufferedPaintDC dc(canvas);

    // Check if the buffer bitmap has the correct size
    if (bufferBitmap.GetWidth() != canvas->GetSize().GetWidth() ||
        bufferBitmap.GetHeight() != canvas->GetSize().GetHeight()) {
        bufferBitmap = wxBitmap(canvas->GetSize().GetWidth(), canvas->GetSize().GetHeight());
    }

    wxMemoryDC memDC;
    memDC.SelectObject(bufferBitmap);

    // Grid dimensions
    int cellWidth = canvas->GetSize().GetWidth() / GRID_WIDTH;
    int cellHeight = canvas->GetSize().GetHeight() / GRID_HEIGHT;

    // Draw cells on memDC
    // Draw cells on memDC
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            if (grid[j][i].alive) {
                memDC.SetBrush(*wxBLACK_BRUSH);  // black for alive cells
            }
            else {
                memDC.SetBrush(*wxWHITE_BRUSH);  // white for dead cells
            }
            memDC.DrawRectangle(j * cellWidth, i * cellHeight, cellWidth, cellHeight);
        }
    }


    // Draw gridlines on memDC
    memDC.SetPen(*wxLIGHT_GREY_PEN);
    for (int i = 0; i <= GRID_HEIGHT; i++) {
        memDC.DrawLine(0, i * cellHeight, GRID_WIDTH * cellWidth, i * cellHeight);
    }
    for (int j = 0; j <= GRID_WIDTH; j++) {
        memDC.DrawLine(j * cellWidth, 0, j * cellWidth, GRID_HEIGHT * cellHeight);
    }

    // Now, copy from memDC to the actual device context
    dc.Blit(0, 0, canvas->GetSize().GetWidth(), canvas->GetSize().GetHeight(), &memDC, 0, 0, wxCOPY, false);
}


void GameOfLifeFrame::OnTimer(wxTimerEvent& event) {
    
    // Sync GUI grid state to Universe's grid before play
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            universe.setCellAlive(j, i, grid[j][i].alive);
        }
    }


    // Advance the game by one generation
    universe.play();

    // Update our local grid based on the Universe class after evolution
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            grid[j][i].alive = universe.getCellState(j, i);
        }
    }

    canvas->Refresh(); // Refresh the canvas to reflect the new state
}


void GameOfLifeFrame::OnInsertGlider(wxCommandEvent& event) {
    // For simplicity, let's place the glider at a fixed position, say (10,10).
    int startX = 10;
    int startY = 10;

    

    // Synchronize the change with the Universe class's grid
    universe.setCellAlive(startX + 1, startY, true);
    universe.setCellAlive(startX + 2, startY + 1, true);
    universe.setCellAlive(startX, startY + 2, true);
    universe.setCellAlive(startX + 1, startY + 2, true);
    universe.setCellAlive(startX + 2, startY + 2, true);

    canvas->Refresh();

    // Define the glider pattern.
    int gliderPattern[3][3] = {
        {0, 1, 0},
        {1, 0, 1},
        {0, 1, 1}
    };

    // Update the grid and the universe.
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            grid[startX + j][startY + i].alive = gliderPattern[i][j];
            universe.setCellAlive(startX + j, startY + i, gliderPattern[i][j]);
        }
    }


    // Refresh the canvas to show the new pattern.
    canvas->Refresh();
}



class GameOfLifeApp : public wxApp {
public:
    virtual bool OnInit() {
        GameOfLifeFrame* frame = new GameOfLifeFrame("Conway's Game of Life", wxPoint(50, 50), wxSize(1050, 1100));
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(GameOfLifeApp);

