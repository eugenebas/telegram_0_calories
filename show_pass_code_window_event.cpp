#include "show_pass_code_window_event.hpp"

wxDEFINE_EVENT(TZC_SHOW_PASS_CODE_WINDOW_EVENT, ShowPassCodeWindowEvent);

ShowPassCodeWindowEvent::ShowPassCodeWindowEvent(std::shared_ptr<std::promise<std::string> > passCodePromise) :
    wxEvent(0, TZC_SHOW_PASS_CODE_WINDOW_EVENT),
    mPassCodePromise(passCodePromise) {
}

ShowPassCodeWindowEvent::ShowPassCodeWindowEvent(const ShowPassCodeWindowEvent& other):
    wxEvent(other),
    mPassCodePromise(other.mPassCodePromise) {
}

wxEvent* ShowPassCodeWindowEvent::Clone() const {
    return new ShowPassCodeWindowEvent(*this);
}

std::shared_ptr<std::promise<std::string>> ShowPassCodeWindowEvent::getPassCodePromise() const {
    return mPassCodePromise;
}

