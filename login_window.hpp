#ifndef __TZC_LOGIN_WINDOW_HPP__
#define __TZC_LOGIN_WINDOW_HPP__

#include <wx/wx.h>
#include <string>
#include <stdexcept>

struct LoginCancelled: std::domain_error {
    LoginCancelled();
};

class LoginWindow final: public wxTextEntryDialog {
public:
    LoginWindow();
    LoginWindow(const LoginWindow&) = delete;
    LoginWindow(LoginWindow&&) = delete;
    LoginWindow* operator=(const LoginWindow&) = delete;
    LoginWindow* operator=(LoginWindow&&) = delete;
    static std::string GetUserLogin();
};

#endif // __TZC_LOGIN_WINDOW_HPP__
