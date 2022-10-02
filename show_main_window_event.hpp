#ifndef __TZC_SHOW_MAIN_WINDOW_EVENT_HPP__
#define __TZC_SHOW_MAIN_WINDOW_EVENT_HPP__

#include <wx/event.h>

class ShowMainWindowEvent: public wxEvent {
public:
    ShowMainWindowEvent();
    ShowMainWindowEvent(const ShowMainWindowEvent&) = default;
    ShowMainWindowEvent(ShowMainWindowEvent&&) = default;
    wxEvent* Clone() const override;
};
wxDECLARE_EVENT(TZC_SHOW_MAIN_WINDOW_EVENT, ShowMainWindowEvent);

#define ShowMainWindowEventHandler(func) (&func)

#endif // __TZC_SHOW_MAIN_WINDOW_EVENT_HPP__
