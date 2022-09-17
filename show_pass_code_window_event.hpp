#ifndef __TZC_SHOW_PASS_CODE_WINDOW_EVENT_HPP__
#define __TZC_SHOW_PASS_CODE_WINDOW_EVENT_HPP__

#include <wx/event.h>
#include <future>
#include <memory>
#include <string>

class ShowPassCodeWindowEvent: public wxEvent {
public:
    ShowPassCodeWindowEvent(std::shared_ptr<std::promise<std::string>> loginPassCodePromise);
    ShowPassCodeWindowEvent(const ShowPassCodeWindowEvent&);
    ShowPassCodeWindowEvent(ShowPassCodeWindowEvent&&) = default;
    wxEvent* Clone() const override;
    std::shared_ptr<std::promise<std::string>> getPassCodePromise() const;
private:
    std::shared_ptr<std::promise<std::string>> mPassCodePromise;
};
wxDECLARE_EVENT(TZC_SHOW_PASS_CODE_WINDOW_EVENT, ShowPassCodeWindowEvent);

#define ShowPassCodeWindowEventHandler(func) (&func)

#endif // __TZC_SHOW_PASS_CODE_WINDOW_EVENT_HPP__
