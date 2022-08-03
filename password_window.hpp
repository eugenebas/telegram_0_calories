#ifndef __TZC_PASSWORD_WINDOW_HPP__
#define __TZC_PASSWORD_WINDOW_HPP__

#include <wx/wx.h>
#include <string>
#include <stdexcept>

struct AutentificationPasswordAborted: std::domain_error{
    AutentificationPasswordAborted();
};

class PasswordWindow final: public wxPasswordEntryDialog {
public:
    PasswordWindow();
    PasswordWindow(const PasswordWindow&) = delete;
    PasswordWindow(PasswordWindow&&) = delete;
    PasswordWindow* operator=(const PasswordWindow&) = delete;
    PasswordWindow* operator=(PasswordWindow&&) = delete;
    static std::string GetPassword();
};

#endif // __TZC_PASSWORD_WINDOW_HPP__
