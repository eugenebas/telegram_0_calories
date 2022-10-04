#include "dialogs_panel.hpp"
#include <wx/listctrl.h>

DialogsPanel::DialogsPanel(wxWindow *parent): wxPanel(parent) {
    SetBackgroundColour(wxColour(* wxRED));
    auto sizer = new wxBoxSizer(wxVERTICAL);
    auto list = new wxListView(this);
    sizer->Add(list, 1, wxEXPAND | wxALL, 0, nullptr);
    SetSizer(sizer);
    auto columntId = list->AppendColumn("Dialogs");
    list->InsertItem(columntId, "zero");
    list->InsertItem(columntId, "one");
    list->InsertItem(columntId, "two");
    list->InsertItem(columntId, "three");
    list->InsertItem(columntId, "four");
    list->InsertItem(columntId, "five");
    list->InsertItem(columntId, "six");
    list->InsertItem(columntId, "seven");
    list->InsertItem(columntId, "eight");
    list->InsertItem(columntId, "nine");
    Layout();
}
