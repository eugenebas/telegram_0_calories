#include "pass_code_window.hpp"
#include <wx/windowptr.h>

AutentificationAborted::AutentificationAborted(): std::domain_error("User aborted authentification") {
}

PassCodeWindow::PassCodeWindow(): wxPasswordEntryDialog(nullptr,
                                                        "Please enter pass code:",
                                                        "Telegram Zero Calories Authentication") {
}

std::string PassCodeWindow::GetPassCode() {
    wxWindowPtr passCodeWindow(new PassCodeWindow());
    if(wxID_OK == passCodeWindow->ShowModal()) {
        return passCodeWindow->GetValue().mb_str().data();
    }
    throw AutentificationAborted();
}



