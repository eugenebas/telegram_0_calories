#ifndef __TZC_SHOW_LOGIN_WINDOW_EVENT_HPP__
#define __TZC_SHOW_LOGIN_WINDOW_EVENT_HPP__

#include <wx/event.h>
#include <future>
#include <memory>
#include <string>

class ShowLoginWindowEvent: public wxEvent {
public:
    ShowLoginWindowEvent(std::shared_ptr<std::promise<std::string>> loginPromise);
    ShowLoginWindowEvent(const ShowLoginWindowEvent&);
    ShowLoginWindowEvent(ShowLoginWindowEvent&&) = default;
    wxEvent* Clone() const override;
    std::shared_ptr<std::promise<std::string>> getLoginPromise() const;
private:
    std::shared_ptr<std::promise<std::string>> mLoginPromise;
};
wxDECLARE_EVENT(TZC_SHOW_LOGIN_WINDOW_EVENT, ShowLoginWindowEvent);

#define ShowLoginWindowEventHandler(func) (&func)

#endif // __TZC_SHOW_LOGIN_WINDOW_EVENT_HPP__
