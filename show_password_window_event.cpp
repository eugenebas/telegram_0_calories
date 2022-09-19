#include "show_password_window_event.hpp"

wxDEFINE_EVENT(TZC_SHOW_PASSWORD_WINDOW_EVENT, ShowPasswordWindowEvent);

ShowPasswordWindowEvent::ShowPasswordWindowEvent(std::shared_ptr<std::promise<std::string> > passwordPromise) :
    wxEvent(0, TZC_SHOW_PASSWORD_WINDOW_EVENT),
    mPasswordPromise(passwordPromise) {
}

ShowPasswordWindowEvent::ShowPasswordWindowEvent(const ShowPasswordWindowEvent& other):
    wxEvent(other),
    mPasswordPromise(other.mPasswordPromise) {
}

wxEvent* ShowPasswordWindowEvent::Clone() const {
    return new ShowPasswordWindowEvent(*this);
}

std::shared_ptr<std::promise<std::string>> ShowPasswordWindowEvent::getPasswordPromise() const {
    return mPasswordPromise;
}

