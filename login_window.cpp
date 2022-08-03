#include "login_window.hpp"
#include <wx/wx.h>
#include <wx/windowptr.h>

LoginWindow::LoginWindow(): wxTextEntryDialog(nullptr, "Please, enter your phone number:", "Telegram Zero Calories Authentication") {
}

std::string LoginWindow::GetUserLogin() {
    wxWindowPtr loginWindow(new LoginWindow());
    if(wxID_OK == loginWindow->ShowModal()) {
        return loginWindow->GetValue().mb_str().data();
    }
    throw LoginCancelled();
}

LoginCancelled::LoginCancelled(): std::domain_error("User cancelled login operation") {
}
