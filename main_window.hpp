#ifndef __TZC_MAIN_WINDOW_HPP__
#define __TZC_MAIN_WINDOW_HPP__

#include <wx/wx.h>
#include <wx/splitter.h>

class MainWindow final: public wxFrame {
public:
    MainWindow();
    void init();
private:
    wxBoxSizer* mSizer;
    wxSplitterWindow* mSplitterWindow;
};

#endif // __TZC_MAIN_WINDOWS_HPP__
