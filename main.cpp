#include <iostream>
#include <wx/wx.h>
#include <wx/windowptr.h>
#include "login_window.hpp"
#include "show_login_window_event.hpp"
#include "pass_code_window.hpp"
#include "password_window.hpp"
#include <thread>
#include "tg_io.hpp"

class MyFrame final: public wxFrame {
public:
    MyFrame();
private:
    void OnExit(wxCommandEvent& event);
};

class MyApplication final: public wxApp {
public:
    LoginWindow* mLoginWindow;
    MyApplication(): wxApp() {
        Bind(TZC_SHOW_LOGIN_WINDOW_EVENT, &MyApplication::OnShowLoginWindow, this);
    }
    void OnShowLoginWindow(ShowLoginWindowEvent& event) {
        std::cout << "OnShowLoginWindow" << std::endl;
        auto loginPromise = event.getLoginPromise();
        try {
            loginPromise->set_value(LoginWindow::GetUserLogin());
        } catch(const std::domain_error&) {
            loginPromise->set_exception(std::current_exception());
        }
    }
    bool OnInit() override {

        mIOThread = std::thread(io_loop, this);
//        ioThread.detach();
        std::cout << "Hello World!" << std::endl;
        try {
//            std::cout << "user login: " << LoginWindow::GetUserLogin() << std::endl;
//            std::cout << "user pass code: " << PassCodeWindow::GetPassCode() << std::endl;
//            std::cout << "user password: " << PasswordWindow::GetPassword() << std::endl;
        } catch(const std::domain_error&) {
            std::cout << "login cancelled" << std::endl;
        }

//        wxClose(false);
        return true;
    }
    int OnExit() override {
        std::cout << "Goodbye world!" << std::endl;
        return 0;
    }

    std::thread mIOThread;
};

wxIMPLEMENT_APP(MyApplication);
