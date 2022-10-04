#include "main_window.hpp"
#include "dialogs_panel.hpp"
#include "dialog_panel.hpp"

MainWindow::MainWindow() : wxFrame(nullptr, wxID_ANY, "Telegram Zero Calories", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, "Telegram Zero Calories"),
    mSizer(new wxBoxSizer(wxVERTICAL)),
    mSplitterWindow(new wxSplitterWindow(this)) {
    SetSize(640, 480);
    mSplitterWindow->SplitVertically(new DialogsPanel(mSplitterWindow), new DialogPanel(mSplitterWindow), GetSize().GetWidth() / 3);
    mSizer->Add(mSplitterWindow, 1, wxEXPAND | wxALL, 0, nullptr);
    SetSizer(mSizer);
    Layout();
}
