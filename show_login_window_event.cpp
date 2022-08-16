#include "show_login_window_event.hpp"

wxDEFINE_EVENT(TZC_SHOW_LOGIN_WINDOW_EVENT, ShowLoginWindowEvent);

ShowLoginWindowEvent::ShowLoginWindowEvent(std::shared_ptr<std::promise<std::string> > loginPromise) :
    wxEvent(0, TZC_SHOW_LOGIN_WINDOW_EVENT),
    mLoginPromise(loginPromise) {
}

ShowLoginWindowEvent::ShowLoginWindowEvent(const ShowLoginWindowEvent& other):
    wxEvent(other),
    mLoginPromise(other.mLoginPromise) {
}

wxEvent* ShowLoginWindowEvent::Clone() const {
    return new ShowLoginWindowEvent(*this);
}

std::shared_ptr<std::promise<std::string>> ShowLoginWindowEvent::getLoginPromise() const {
    return mLoginPromise;
}

