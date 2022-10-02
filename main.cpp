#include <iostream>
#include <wx/wx.h>
#include <wx/windowptr.h>
#include "login_window.hpp"
#include "show_login_window_event.hpp"
#include "show_pass_code_window_event.hpp"
#include "show_password_window_event.hpp"
#include "show_main_window_event.hpp"
#include "pass_code_window.hpp"
#include "password_window.hpp"
#include "password_window.hpp"
#include "main_window.hpp"
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
    MainWindow* mMainWindow{nullptr};

    MyApplication(): wxApp() {
        Bind(TZC_SHOW_LOGIN_WINDOW_EVENT, &MyApplication::OnShowLoginWindow, this);
        Bind(TZC_SHOW_PASS_CODE_WINDOW_EVENT, &MyApplication::OnShowPassCodeWindow, this);
        Bind(TZC_SHOW_PASSWORD_WINDOW_EVENT, &MyApplication::OnShowPasswordWindow, this);
        Bind(TZC_SHOW_MAIN_WINDOW_EVENT, &MyApplication::OnShowMainWindow, this);
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

    void OnShowPassCodeWindow(ShowPassCodeWindowEvent& event) {
        std::cout << "OnShowPassCodeWindow" << std::endl;
        auto passCodePromise = event.getPassCodePromise();
        try {
            passCodePromise->set_value(PassCodeWindow::GetPassCode());
        } catch(const std::domain_error&) {
            passCodePromise->set_exception(std::current_exception());
        }
    }

    void OnShowPasswordWindow(ShowPasswordWindowEvent& event) {
        std::cout << "OnShowPasswordWindow" << std::endl;
        auto passwordPromise = event.getPasswordPromise();
        try {
            passwordPromise->set_value(PasswordWindow::GetPassword());
        } catch(const std::domain_error&) {
            passwordPromise->set_exception(std::current_exception());
        }
    }

    void OnShowMainWindow(ShowMainWindowEvent&) {
        std::cout << "OnShowMainWindow" << std::endl;
        mMainWindow = new MainWindow();
        mMainWindow->Show();
    }

    bool OnInit() override {
        mIOThread = std::thread(io_loop, this);
        mIOThread.detach();
        SetExitOnFrameDelete(false);
        return true;
    }

    int OnExit() override {
        return 0;
    }

    std::thread mIOThread;
};

wxIMPLEMENT_APP(MyApplication);
