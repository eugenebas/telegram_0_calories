#include "password_window.hpp"
#include <wx/windowptr.h>

AutentificationPasswordAborted::AutentificationPasswordAborted(): std::domain_error("User aborted authentification") {
}

PasswordWindow::PasswordWindow(): wxPasswordEntryDialog(nullptr,
                                                        "Please enter password:",
                                                        "Telegram Zero Calories Authentication") {
}

std::string PasswordWindow::GetPassword() {
    wxWindowPtr passwordWindow(new PasswordWindow());
    if(wxID_OK == passwordWindow->ShowModal()) {
        return passwordWindow->GetValue().mb_str().data();
    }
    throw AutentificationPasswordAborted();
}



