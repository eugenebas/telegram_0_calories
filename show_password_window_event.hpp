#ifndef __TZC_SHOW_PASSWORD_WINDOW_EVENT_HPP__
#define __TZC_SHOW___TZC_SHOW_PASSWORD_WINDOW_EVENT_HPP___WINDOW_EVENT_HPP__

#include <wx/event.h>
#include <future>
#include <memory>
#include <string>

class ShowPasswordWindowEvent: public wxEvent {
public:
    ShowPasswordWindowEvent(std::shared_ptr<std::promise<std::string>> passwordPromise);
    ShowPasswordWindowEvent(const ShowPasswordWindowEvent&);
    ShowPasswordWindowEvent(ShowPasswordWindowEvent&&) = default;
    wxEvent* Clone() const override;
    std::shared_ptr<std::promise<std::string>> getPasswordPromise() const;
private:
    std::shared_ptr<std::promise<std::string>> mPasswordPromise;
};
wxDECLARE_EVENT(TZC_SHOW_PASSWORD_WINDOW_EVENT, ShowPasswordWindowEvent);

#define ShowPasswordWindowEventHandler(func) (&func)

#endif // __TZC_SHOW___TZC_SHOW_PASSWORD_WINDOW_EVENT_HPP___WINDOW_EVENT_HPP__
