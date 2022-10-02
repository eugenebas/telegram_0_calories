#include "show_main_window_event.hpp"

wxDEFINE_EVENT(TZC_SHOW_MAIN_WINDOW_EVENT, ShowMainWindowEvent);

ShowMainWindowEvent::ShowMainWindowEvent() : wxEvent(0, TZC_SHOW_MAIN_WINDOW_EVENT) {
}

wxEvent* ShowMainWindowEvent::Clone() const {
    return new ShowMainWindowEvent(*this);
}
