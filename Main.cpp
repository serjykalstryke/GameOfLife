#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "Universe.h"
#include <wx/app.h>
#include <wx/event.h>
#include <random>
#include <wx/colordlg.h>
#include <wx/dcbuffer.h>

#include <map>




class GameOfLifeFrame : public wxFrame {
public:
    GameOfLifeFrame(const wxString& title, const wxPoint& pos, const wxSize& size);


    static const int GRID_WIDTH;
    static const int GRID_HEIGHT;
    void OnStart(wxCommandEvent& event);
    void OnDrawCell(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnInsertGlider(wxCommandEvent& event);
    void OnInsertSpaceship(wxCommandEvent& event);
    void OnInsertPulsar(wxCommandEvent& event);
    void OnRandomize(wxCommandEvent& event);
    void OnClearAll(wxCommandEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnChangeColor(wxCommandEvent& event);
    ~GameOfLifeFrame();

   

private:
    Universe universe;
    wxPanel* canvas;
    wxButton* startButton;
    wxButton* randomizeButton;
    wxButton* insertGliderButton;
    wxButton* insertSpaceshipButton;
    wxButton* insertPulsarButton;
    wxButton* clearButton;
    wxTimer* timer;

    void OnTimer(wxTimerEvent& event);
    wxBitmap bufferBitmap;
    bool simulationRunning = false;
    wxColour currentCellColor = *wxBLACK;  // Default color for alive cells
    wxColour backgroundColor = *wxWHITE;   // Default background color

    void DrawPattern(const std::vector<std::vector<int>>& pattern);
};
const int GameOfLifeFrame::GRID_WIDTH = Universe::getGridWidth();
const int GameOfLifeFrame::GRID_HEIGHT = Universe::getGridHeight();

GameOfLifeFrame::GameOfLifeFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size), universe(GRID_WIDTH, GRID_HEIGHT) {
    canvas = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    canvas->SetBackgroundStyle(wxBG_STYLE_PAINT);
    canvas->Bind(wxEVT_LEFT_DOWN, &GameOfLifeFrame::OnDrawCell, this);
    canvas->Bind(wxEVT_SIZE, &GameOfLifeFrame::OnResize, this);

    


    // Create buttons
    startButton = new wxButton(this, wxID_ANY, "Start");
    startButton->Bind(wxEVT_BUTTON, &GameOfLifeFrame::OnStart, this);
    randomizeButton = new wxButton(this, wxID_ANY, "Randomize");
    randomizeButton->Bind(wxEVT_BUTTON, &GameOfLifeFrame::OnRandomize, this);
    insertGliderButton = new wxButton(this, wxID_ANY, "Insert Glider");
    insertGliderButton->Bind(wxEVT_BUTTON, &GameOfLifeFrame::OnInsertGlider, this);
    clearButton = new wxButton(this, wxID_ANY, "Clear All");
    clearButton->Bind(wxEVT_BUTTON, &GameOfLifeFrame::OnClearAll, this);
    insertSpaceshipButton = new wxButton(this, wxID_ANY, "Insert Spaceship");
    insertSpaceshipButton->Bind(wxEVT_BUTTON, &GameOfLifeFrame::OnInsertSpaceship, this);
    insertPulsarButton = new wxButton(this, wxID_ANY, "Insert Pulsar");
    insertPulsarButton->Bind(wxEVT_BUTTON, &GameOfLifeFrame::OnInsertPulsar, this);
    wxButton* changeColorButton = new wxButton(this, wxID_ANY, "Change Cell Color");
    changeColorButton->Bind(wxEVT_BUTTON, &GameOfLifeFrame::OnChangeColor, this);

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(startButton, 0, wxALL, 10);
    buttonSizer->Add(randomizeButton, 0, wxALL, 10);
    buttonSizer->Add(clearButton, 0, wxALL, 10);
    buttonSizer->Add(insertGliderButton, 0, wxALL, 10);
    buttonSizer->Add(insertSpaceshipButton, 0, wxALL, 10);
    buttonSizer->Add(insertPulsarButton, 0, wxALL, 10);
    buttonSizer->Add(changeColorButton, 0, wxALL, 10);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(canvas, 1, wxEXPAND);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    SetSizer(mainSizer);
    SetSize(size);
    canvas->Bind(wxEVT_PAINT, &GameOfLifeFrame::OnPaint, this);

    timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &GameOfLifeFrame::OnTimer, this, timer->GetId());
}

GameOfLifeFrame::~GameOfLifeFrame() {
    if (timer && timer->IsRunning()) {
        timer->Stop();
    }
    delete timer;
}

void GameOfLifeFrame::DrawPattern(const std::vector<std::vector<int>>& pattern) {
    int patternWidth = pattern[0].size();
    int patternHeight = pattern.size();

    // Generate a random starting position for the pattern.
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> distX(0, this->GRID_WIDTH - patternWidth);
    std::uniform_int_distribution<int> distY(0, this->GRID_HEIGHT - patternHeight);

    int startX = distX(mt);
    int startY = distY(mt);

    // Update the universe with the pattern.
    for (int i = 0; i < patternHeight; i++) {
        for (int j = 0; j < patternWidth; j++) {
            bool alive = pattern[i][j] == 1;
            int actualX = startX + j; // startX is the starting x position for your pattern on the universe
            int actualY = startY + i; // startY is the starting y position for your pattern on the universe
            universe.setCellAlive(actualX, actualY, alive, currentCellColor);
        }
    }
    canvas->Refresh();
}

void GameOfLifeFrame::OnStart(wxCommandEvent& event) {
    if (simulationRunning) {
        // Stop the simulation.
        simulationRunning = false;
        timer->Stop();
        startButton->SetLabel("Start");
    } else {
        // Start the simulation.
        simulationRunning = true;
        timer->Start(100);
        startButton->SetLabel("Stop");
    }
}
void GameOfLifeFrame::OnDrawCell(wxMouseEvent& event) {
    int cellWidth = canvas->GetSize().GetWidth() / GRID_WIDTH;
    int cellHeight = canvas->GetSize().GetHeight() / GRID_HEIGHT;

    int x = event.GetX() / cellWidth;
    int y = event.GetY() / cellHeight;

    bool currentState = universe.getCellState(x, y);
    universe.setCellAlive(x, y, !currentState, currentCellColor);  // Pass the current color when setting a cell alive

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
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            if (universe.getCellState(j, i)) {
                memDC.SetBrush(wxBrush(universe.getCellColor(j, i)));
            }
            else {
                memDC.SetBrush(*wxWHITE_BRUSH);
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

bool simulationRunning = false;

void GameOfLifeFrame::OnTimer(wxTimerEvent& event) {
    if (simulationRunning) {
        // Create a copy of the current grid to store the next generation temporarily
        Universe nextGeneration = universe;

        // Calculate the next generation of the grid
        for (int i = 0; i < GRID_HEIGHT; i++) {
            for (int j = 0; j < GRID_WIDTH; j++) {
                int neighbors = universe.countNeighbors(j, i);

                if (universe.getCellState(j, i)) {
                    // Existing cell: Just update alive status without changing color
                    if (neighbors < 2 || neighbors > 3) {
                        nextGeneration.setCellAlive(j, i, false);
                    }
                    // For alive cells that survive, we don't need to change the color or set them alive again.
                }
                else {
                    // Dead cell: If it becomes alive, set its color to the current picked color
                    if (neighbors == 3) {
                        nextGeneration.setCellAlive(j, i, true, universe.determineBirthColor(j, i));
                    }
                    // For dead cells that remain dead, no action is required.
                }
               
            }
        }

        // Update the universe with the calculated next generation
        universe = nextGeneration;

        // Refresh the canvas to display the next generation
        canvas->Refresh();
    }
}





void GameOfLifeFrame::OnInsertGlider(wxCommandEvent& event) {
    std::vector<std::vector<int>> gliderPattern = {
        {0, 1, 0},
        {0, 0, 1},
        {1, 1, 1}
    };
    DrawPattern(gliderPattern);
}

void GameOfLifeFrame::OnInsertSpaceship(wxCommandEvent& event) {
    std::vector<std::vector<int>> spaceshipPattern = {
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {1, 0, 0, 1, 0}
    };
    DrawPattern(spaceshipPattern);  // Using the generalized function
}

void GameOfLifeFrame::OnInsertPulsar(wxCommandEvent& event) {
    std::vector<std::vector<int>>  pulsarPattern = {
         {0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0},
        {0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0},
        {0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0},
    };
    DrawPattern(pulsarPattern);  // Using the generalized function
}




void GameOfLifeFrame::OnRandomize(wxCommandEvent& event) {
    universe.initializeRandomUniverse();
    canvas->Refresh();
}

void GameOfLifeFrame::OnClearAll(wxCommandEvent& event) {
    // TODO: Implement the clearAll method in Universe class.
    universe.clearAll();

    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            universe.setCellColor(GridCoord(j, i), backgroundColor);
        }
    }
    canvas->Refresh();
}

void GameOfLifeFrame::OnResize(wxSizeEvent& event) {
    // Recalculate the cell size to fit the window.
    // TODO: Implement this logic if needed.
    canvas->Refresh();
    event.Skip(); // It's important to skip the event for default handling
}







void GameOfLifeFrame::OnChangeColor(wxCommandEvent& event) {
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(currentCellColor); // setting the current color for the dialog

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK) {
        currentCellColor = dialog.GetColourData().GetColour();
        // No canvas refresh here. The new color will be used when new cells are born.
    }
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

