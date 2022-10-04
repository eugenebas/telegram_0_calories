#include "main_window.hpp"

MainWindow::MainWindow() : wxFrame(nullptr, wxID_ANY, "Telegram Zero Calories", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, "Telegram Zero Calories"),
    mSizer(new wxBoxSizer(wxVERTICAL)),
    mSplitterWindow(new wxSplitterWindow(this)) {
    SetBackgroundColour(wxColour(* wxWHITE));
    auto panel1 = new wxPanel(mSplitterWindow);
    panel1->SetBackgroundColour(wxColour(* wxRED));
    auto panel2 = new wxPanel(mSplitterWindow);
    panel2->SetBackgroundColour(wxColour(* wxBLUE));
    mSplitterWindow->SplitVertically(panel1, panel2, GetSize().GetWidth() / 3);
    mSizer->Add(mSplitterWindow, 1, wxEXPAND | wxALL, 0, nullptr);
    SetSizer(mSizer);
    Layout();
}

void MainWindow::init() {
    Update();
}
