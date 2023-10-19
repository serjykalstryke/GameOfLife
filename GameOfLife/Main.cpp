


#include "Universe.h"
#include <wx/wx.h>
#include <random>
#include <wx/colordlg.h>
#include <wx/dcbuffer.h>
#include <wx/file.h>
#include <iosfwd>
#include <sstream>
#include "../Binaries/include/wx/app.h"
#include <filesystem>






class GameOfLifeFrame : public wxFrame {
public:

    enum {
        ID_Menu_Exit = wxID_HIGHEST + 1,
        ID_Menu_About,
        ID_Menu_Save,
        ID_Menu_Load,
        ID_Menu_ChangeGridColor,
        ID_Menu_ChangeBackgroundColor,
        ID_Menu_SaveSettings,
        ID_Menu_LoadSettings,
        ID_Menu_ResetDefaults,
        ID_TOROIDAL
    };




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
    void UpdateStatusBar();
    void OnNext(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnMenuSave(wxCommandEvent& event);
    void OnMenuLoad(wxCommandEvent& event);
    void OnChangeGridColor(wxCommandEvent& event);
    void OnChangeBackgroundColor(wxCommandEvent& event);
    void OnSaveSettings(wxCommandEvent& event);
    void OnLoadSettings(wxCommandEvent& event);
    void OnResetDefaults(wxCommandEvent& event);
    void RefreshGrid();
    void InitializeGrid();
    void OnToggleToroidal(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    wxString GetToroidalMenuItemLabel() const;
    wxColour currentGridColor;
    GameOfLifeFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
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
    wxButton* nextButton;
    wxButton* pauseButton; 
    bool paused = false; 
    wxTimer* timer;
    wxMenu* settingsMenu;
    wxMenuItem* toggleToroidalMenuItem;

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
    canvas->Refresh();

    canvas->SetBackgroundStyle(wxBG_STYLE_PAINT);
    canvas->Bind(wxEVT_LEFT_DOWN, &GameOfLifeFrame::OnDrawCell, this);
    canvas->Bind(wxEVT_SIZE, &GameOfLifeFrame::OnResize, this);

    CreateStatusBar(2); // Creates a status bar with 2 fields
    wxColour currentGridColor = wxColour(0, 0, 0);

   



    // Create a menu bar
    wxMenuBar* menuBar = new wxMenuBar;

    // Create a "File" menu
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(ID_Menu_Save, "&Save", "Save the current game state");
    fileMenu->Append(ID_Menu_Load, "&Load", "Load a game state");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_Menu_Exit, "E&xit", "Exit the application");
    menuBar->Append(fileMenu, "&File");

    // Create a "Settings" menu
    wxMenu* settingsMenu = new wxMenu;
    settingsMenu->Append(ID_Menu_ChangeGridColor, "&Change Grid Color", "Change the color of the grid");
    settingsMenu->Append(ID_Menu_ChangeBackgroundColor, "&Change Background Color", "Change the background color");
    settingsMenu->Append(ID_Menu_SaveSettings, "&Save Settings", "Save the current settings to a file");
    settingsMenu->Append(ID_Menu_LoadSettings, _("Load Settings"), _("Load application settings from file"));
    settingsMenu->Append(ID_Menu_ResetDefaults, _("Reset to Default"), _("Restore default application settings"));
    wxMenuItem* toggleToroidalMenuItem = new wxMenuItem(settingsMenu, ID_TOROIDAL, "Toggle Universe Type");
    settingsMenu->Append(toggleToroidalMenuItem);


    menuBar->Append(settingsMenu, _("Settings"));




    // Create a "Help" menu
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(ID_Menu_About, "&About", "Information about the application");
    menuBar->Append(helpMenu, "&Help");

    // Assign the menu bar to the frame
    SetMenuBar(menuBar);


    Bind(wxEVT_MENU, &GameOfLifeFrame::OnMenuSave, this, ID_Menu_Save);
    Bind(wxEVT_MENU, &GameOfLifeFrame::OnMenuLoad, this, ID_Menu_Load);
    Bind(wxEVT_MENU, &GameOfLifeFrame::OnChangeGridColor, this, ID_Menu_ChangeGridColor);
    Bind(wxEVT_MENU, &GameOfLifeFrame::OnChangeBackgroundColor, this, ID_Menu_ChangeBackgroundColor);
    Bind(wxEVT_MENU, &GameOfLifeFrame::OnSaveSettings, this, ID_Menu_SaveSettings);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &GameOfLifeFrame::OnLoadSettings, this, ID_Menu_LoadSettings);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &GameOfLifeFrame::OnResetDefaults, this, ID_Menu_ResetDefaults);
    Bind(wxEVT_MENU, &GameOfLifeFrame::OnToggleToroidal, this, ID_TOROIDAL); // Bind the event handler
    // Inside the GameOfLifeFrame constructor
    Bind(wxEVT_CLOSE_WINDOW, &GameOfLifeFrame::OnClose, this);




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

    nextButton = new wxButton(this, wxID_ANY, "Next");
    nextButton->Bind(wxEVT_BUTTON, &GameOfLifeFrame::OnNext, this);
    nextButton->Disable(); // Disable the button initially
    pauseButton = new wxButton(this, wxID_ANY, "Pause");
    pauseButton->Bind(wxEVT_BUTTON, &GameOfLifeFrame::OnPause, this);


    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(startButton, 0, wxALL, 10);
    buttonSizer->Add(pauseButton, 0, wxALL, 10);
    buttonSizer->Add(nextButton, 0, wxALL, 10);
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

    GameOfLifeFrame::RefreshGrid();
    GameOfLifeFrame::InitializeGrid();

    // Define the autosave file path.
    std::string autosavePath = "autosave.gol";

    // Check if the autosave file exists and if so, load the game state from it.
    if (std::filesystem::exists(autosavePath)) {
        // Load the universe state
        universe.load(autosavePath, currentGridColor, backgroundColor);

        // Logging the loaded grid color for debug purposes
        wxColour loadedGridColor = currentGridColor; // This color should have been set by the load function

        // At this point, currentGridColor and backgroundColor have the new values from the loaded file.
        // The OnPaint method will use these colors in its next invocation.


        canvas->SetBackgroundColour(backgroundColor); // This sets the new background color

        // Trigger a repaint event. This will cause OnPaint to be called, where your actual drawing logic is.
        canvas->Refresh();
        canvas->Update();  // This forces an immediate redraw; use it if you're facing issues with delayed updates.
    }


    

}



std::string SerializeSettings(const wxColour& gridColor, const wxColour& backgroundColor) {
    std::stringstream ss;
    ss << "GridColor=" << gridColor.GetAsString() << "\n";
    ss << "BackgroundColor=" << backgroundColor.GetAsString() << "\n";
    return ss.str();
}

void DeserializeSettings(const std::string& data, wxColour& gridColor, wxColour& bgColor) {
    std::istringstream ss(data);

    std::string line;
    while (std::getline(ss, line)) {
        std::size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            if (key == "GridColor") {
                gridColor = wxColour(value);
            }
            else if (key == "BackgroundColor") {
                bgColor = wxColour(value);
            }
        }
    }
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
    if (paused) {
        // Resume the simulation
        paused = false;
        startButton->SetLabel("Stop");
        timer->Start(100);
        pauseButton->Enable(); // Enable the "Pause" button when starting or resuming the simulation
        nextButton->Disable(); // Disable the "Next" button when starting or resuming the simulation
        UpdateStatusBar();
    }
    else if (simulationRunning) {
        // Stop the simulation
        simulationRunning = false;
        timer->Stop();
        startButton->SetLabel("Start");
        pauseButton->Disable(); // Disable the "Pause" button when stopping the simulation
        nextButton->Disable(); // Disable the "Next" button when stopping the simulation
        UpdateStatusBar();
    }
    else {
        // Start the simulation
        simulationRunning = true;
        timer->Start(100);
        startButton->SetLabel("Stop");
        pauseButton->Enable(); // Enable the "Pause" button when starting the simulation
        nextButton->Disable(); // Disable the "Next" button when starting the simulation
        UpdateStatusBar();
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
    UpdateStatusBar();
  
}




void GameOfLifeFrame::OnPaint(wxPaintEvent& event) {
    wxBufferedPaintDC dc(canvas);

    // Ensure buffer bitmap has the correct size
    if (bufferBitmap.GetWidth() != canvas->GetSize().GetWidth() ||
        bufferBitmap.GetHeight() != canvas->GetSize().GetHeight()) {
        bufferBitmap = wxBitmap(canvas->GetSize().GetWidth(), canvas->GetSize().GetHeight());
    }

    wxMemoryDC memDC;
    memDC.SelectObject(bufferBitmap);

    // Use dynamic grid dimensions
    int cellWidth = canvas->GetSize().GetWidth() / universe.getGridWidth();
    int cellHeight = canvas->GetSize().GetHeight() / universe.getGridHeight();

    // Draw cells on memDC
    for (int i = 0; i < universe.getGridHeight(); i++) {
        for (int j = 0; j < universe.getGridWidth(); j++) {
            if (universe.getCellState(j, i)) {
                memDC.SetBrush(wxBrush(universe.getCellColor(j, i)));
            }
            else {
                memDC.SetBrush(backgroundColor);
            }
            memDC.DrawRectangle(j * cellWidth, i * cellHeight, cellWidth, cellHeight);
        }
    }

    // Draw gridlines on memDC
    
    memDC.SetPen(currentGridColor);
    for (int i = 0; i <= universe.getGridHeight(); i++) {
        memDC.DrawLine(0, i * cellHeight, universe.getGridWidth() * cellWidth, i * cellHeight);
    }
    for (int j = 0; j <= universe.getGridWidth(); j++) {
        memDC.DrawLine(j * cellWidth, 0, j * cellWidth, universe.getGridHeight() * cellHeight);
    }

    // Copy from memDC to the actual device context
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
                        UpdateStatusBar();
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
    UpdateStatusBar();
}

void GameOfLifeFrame::OnInsertSpaceship(wxCommandEvent& event) {
    std::vector<std::vector<int>> spaceshipPattern = {
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {1, 0, 0, 1, 0}
    };
    DrawPattern(spaceshipPattern);  // Using the generalized function
    UpdateStatusBar();
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
    UpdateStatusBar();
}




void GameOfLifeFrame::OnRandomize(wxCommandEvent& event) {
    universe.initializeRandomUniverse();
    canvas->Refresh();
    UpdateStatusBar();
}
void GameOfLifeFrame::OnClearAll(wxCommandEvent& event) {
    universe.clearAll(backgroundColor);
    canvas->Refresh();
    UpdateStatusBar();
}

void GameOfLifeFrame::RefreshGrid() {
    // Get the current size of the canvas
    wxSize canvasSize = canvas->GetSize();

    // Update the size of the grid based on the canvas size
    int newWidth = canvasSize.GetWidth();
    int newHeight = canvasSize.GetHeight();

    // Resize the universe with the new size
    universe.resize(newWidth, newHeight);

    // Refresh the canvas to redraw the updated grid
    canvas->Refresh();
}


void GameOfLifeFrame::OnResize(wxSizeEvent& event) {
    // This ensures that the event is propagated (e.g., allowing min/max size logic, if any, to work).
    event.Skip();

    // Update the universe's dimensions based on the canvas size
    // TODO: Ensure the Universe class has methods to support dynamic resizing without reinitialization, if required.

    RefreshGrid();
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

void GameOfLifeFrame::UpdateStatusBar() {
    int aliveCount = 0;
    int deadCount = 0;

    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            if (universe.getCellState(j, i)) {
                aliveCount++;
            }
            else {
                deadCount++;
            }
        }
    }

    wxString aliveStr = wxString::Format("Alive cells: %d", aliveCount);
    wxString deadStr = wxString::Format("Dead cells: %d", deadCount);

    SetStatusText(aliveStr, 0);
    SetStatusText(deadStr, 1);
}

void GameOfLifeFrame::OnPause(wxCommandEvent& event) {
    if (paused) {
        // Resume the simulation
        paused = false;
        startButton->SetLabel("Stop");
        timer->Start(100);
        nextButton->Disable(); // Disable the "Next" button when resuming the simulation
        UpdateStatusBar();
    }
    else {
        // Pause the simulation
        paused = true;
        startButton->SetLabel("Start");
        timer->Stop();
        nextButton->Enable(); // Enable the "Next" button when pausing the simulation
        UpdateStatusBar();
    }
}


void GameOfLifeFrame::OnNext(wxCommandEvent& event) {
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
            }
            else {
                // Dead cell: If it becomes alive, set its color to the current picked color
                if (neighbors == 3) {
                    nextGeneration.setCellAlive(j, i, true, universe.determineBirthColor(j, i));
                }
            }
        }
    }

    // Update the universe with the calculated next generation
    universe = nextGeneration;

    // Refresh the canvas to display the next generation
    canvas->Refresh();
    UpdateStatusBar();
}

void GameOfLifeFrame::OnMenuSave(wxCommandEvent& event) {
    wxFileDialog saveFileDialog(this, "Save Game State", "", "",
        "Game State files (*.gol)|*.gol", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL) {
        return;  // User cancelled
    }

    // Log the colors for debugging purposes

    // Save the current state of the universe to the selected file
    universe.save(saveFileDialog.GetPath().ToStdString(), currentGridColor, backgroundColor);
}

void GameOfLifeFrame::OnMenuLoad(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, "Load Game State", "", "",
        "Game State files (*.gol)|*.gol", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) {
        return;  // User cancelled
    }

    // Here you'd load the 'universe' object from the chosen file
    // For example:
    universe.load(openFileDialog.GetPath().ToStdString(), currentGridColor, backgroundColor);

    if (!universe.load(openFileDialog.GetPath().ToStdString(), currentGridColor, backgroundColor)) {
        // Handle the error (e.g., show a message to the user)
        wxMessageBox(_("Failed to load the game state."), _("Error"), wxICON_ERROR);
        return;
    }

    // Now, 'currentGridColor' and 'backgroundColor' should contain the new colors.
    // Update your GUI components with these colors. For example:
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(currentGridColor);
    canvas->SetBackgroundColour(backgroundColor);

    

    wxColour loadedGridColor;
    loadedGridColor = currentGridColor;


    // After loading, you might want to redraw the canvas to reflect the loaded state
    canvas->Refresh();
}

void GameOfLifeFrame::OnChangeGridColor(wxCommandEvent& event) {
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(currentGridColor);  

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK) {
        currentGridColor = dialog.GetColourData().GetColour();
        canvas->Refresh();  // Redraw to show the new color
    }
}

void GameOfLifeFrame::OnChangeBackgroundColor(wxCommandEvent& event) {
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(backgroundColor);

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK) {
        backgroundColor = dialog.GetColourData().GetColour();
        canvas->SetBackgroundColour(backgroundColor);
        canvas->Refresh();  // Redraw to show the new color
        canvas->Update();
    }
}
void GameOfLifeFrame::OnSaveSettings(wxCommandEvent& event) {
    wxFileDialog saveFileDialog(this, _("Save Settings"), "", "",
        "Settings files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;     // User cancelled

    wxFile file(saveFileDialog.GetPath(), wxFile::write);
    if (!file.IsOpened()) {
        wxMessageBox("Failed to save settings", "Error", wxICON_ERROR);
        return;
    }

    std::string settingsData = SerializeSettings(currentGridColor, backgroundColor);
    file.Write(settingsData);
}

void GameOfLifeFrame::OnLoadSettings(wxCommandEvent& event) {
    wxFileDialog loadFileDialog(this, _("Load Settings"), "", "",
        "Settings files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (loadFileDialog.ShowModal() == wxID_CANCEL)
        return;     // User cancelled

    wxFile file(loadFileDialog.GetPath(), wxFile::read);
    if (!file.IsOpened()) {
        wxMessageBox("Failed to load settings", "Error", wxICON_ERROR);
        return;
    }

    wxString settingsData;
    file.ReadAll(&settingsData);

    DeserializeSettings(settingsData.ToStdString(), currentGridColor, backgroundColor);
    canvas->Refresh();
    canvas->Update();

}

void GameOfLifeFrame::OnResetDefaults(wxCommandEvent& event) {
    currentGridColor = *wxBLACK;  // Set your default grid color here
    backgroundColor = *wxWHITE;   // Set your default background color here



    canvas->Refresh();  // Redraw to reflect the updated settings
}


void GameOfLifeFrame::InitializeGrid() {
  
    // Get the current size of the canvas
    wxSize canvasSize = canvas->GetSize();

    // Calculate the initial size of the grid based on the canvas size
    int initialWidth = canvasSize.GetWidth();
    int initialHeight = canvasSize.GetHeight();

    // Initialize the grid with the initial size
    universe = Universe(initialWidth, initialHeight);
}

void GameOfLifeFrame::OnToggleToroidal(wxCommandEvent& event) {
    // Toggle the toroidal state based on the current state
    bool isCurrentlyToroidal = universe.getToroidal();
    universe.setToroidal(!isCurrentlyToroidal);

    // Update the menu item label to reflect the new state
    /*wxMenuItem* toggleToroidalMenuItem = settingsMenu->FindItem(ID_TOROIDAL);
    if (toggleToroidalMenuItem) {
        toggleToroidalMenuItem->SetItemLabel(GetToroidalMenuItemLabel());
    }*/

    canvas->Refresh();
}

wxString GameOfLifeFrame::GetToroidalMenuItemLabel() const {
    return universe.getToroidal() ? "Change to Non-Toroidal" : "Change to Toroidal";
}

void GameOfLifeFrame::OnClose(wxCloseEvent& event)
{
    // Define a path for the autosave file
    std::string autosavePath = "autosave.gol";

    // Save the current state of the universe to the autosave file
    universe.save(autosavePath, currentGridColor, backgroundColor);
    // Proceed with the close event
    event.Skip(); // important: it allows the event to be processed by other handlers
}



class GameOfLifeApp : public wxApp {
public:
    virtual bool OnInit() {
        // Create the frame with the specified title and size (e.g., 1600x900)
        GameOfLifeFrame* frame = new GameOfLifeFrame("Conway's Game of Life", wxDefaultPosition, wxSize(1600, 900));

        // Show the frame
        frame->Show(true);

        return true;
    }

};

wxIMPLEMENT_APP(GameOfLifeApp);

