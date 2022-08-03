#include <iostream>
#include <wx/wx.h>
#include <wx/windowptr.h>
#include "login_window.hpp"
#include "pass_code_window.hpp"
#include "password_window.hpp"

class MyFrame final: public wxFrame {
public:
    MyFrame();
private:
    void OnExit(wxCommandEvent& event);
};

class MyApplication final: public wxApp {
public:
    LoginWindow* mLoginWindow;
    bool OnInit() override {
        std::cout << "Hello World!" << std::endl;
        try {
            std::cout << "user login: " << LoginWindow::GetUserLogin() << std::endl;
            std::cout << "user pass code: " << PassCodeWindow::GetPassCode() << std::endl;
            std::cout << "user password: " << PasswordWindow::GetPassword() << std::endl;
        } catch(const std::domain_error&) {
            std::cout << "login cancelled" << std::endl;
        }

        wxClose(false);
        return true;
    }
    int OnExit() override {
        std::cout << "Goodbye world!" << std::endl;
        return 0;
    }
};

wxIMPLEMENT_APP(MyApplication);
