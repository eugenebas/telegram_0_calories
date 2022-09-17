#include <iostream>
#include <wx/wx.h>
#include <wx/windowptr.h>
#include "login_window.hpp"
#include "show_login_window_event.hpp"
#include "show_pass_code_window_event.hpp"
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
        Bind(TZC_SHOW_PASS_CODE_WINDOW_EVENT, &MyApplication::OnShowPassCodeWindow, this);
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

    bool OnInit() override {

        mIOThread = std::thread(io_loop, this);
        mIOThread.detach();
        std::cout << "Hello World!" << std::endl;
//        wxClose(false);
        SetExitOnFrameDelete(false);
        return true;
    }
    int OnExit() override {
        std::cout << "Goodbye world!" << std::endl;
        return 0;
    }

    std::thread mIOThread;
};

wxIMPLEMENT_APP(MyApplication);
