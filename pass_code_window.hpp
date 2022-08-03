#ifndef __TZC_PASS_CODE_WINDOW_HPP__
#define __TZC_PASS_CODE_WINDOW_HPP__

#include <wx/wx.h>
#include <string>
#include <stdexcept>

struct AutentificationAborted: std::domain_error{
    AutentificationAborted();
};

class PassCodeWindow final: public wxPasswordEntryDialog {
public:
    PassCodeWindow();
    PassCodeWindow(const PassCodeWindow&) = delete;
    PassCodeWindow(PassCodeWindow&&) = delete;
    PassCodeWindow* operator=(const PassCodeWindow&) = delete;
    PassCodeWindow* operator=(PassCodeWindow&&) = delete;
    static std::string GetPassCode();
};

#endif // __TZC_PASS_CODE_WINDOW_HPP__
