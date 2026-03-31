#ifndef INITPLUS_GTK_LINK_BUTTON_HPP_INCLUDED
#define INITPLUS_GTK_LINK_BUTTON_HPP_INCLUDED

#include "macro.hpp"
#include "utils.hpp"
#include "../tools/dispatch.hpp"
#include "../tools/weak_bind.hpp"

#include <gtkmm-3.0/gtkmm.h>

namespace initp {
namespace gtk {

class link_button:
    public Gtk::Button {
private: // Private types
    typedef link_button self_type;
public: // Construction
    link_button(void):
        Gtk::Button() {
        this->connect_signals();
    }
    link_button(tools::dispatcher::ptr d):
        Gtk::Button(),
        dispatcher_(d) {
        this->connect_signals();
    }
    link_button(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder):
        Gtk::Button(cobject) {
        this->connect_signals();
    }
    virtual ~link_button(void) {}
public: // Public methods
    void dispatch_label(const Glib::ustring& value) {
        if (tools::dispatcher::ptr d = this->dispatcher()) {
            DISPATCH(d, set_label, value);
        }
    }
    void dispatch_label(const std::string& value) {
        if (tools::dispatcher::ptr d = this->dispatcher()) {
            DISPATCH(d, set_label, utils::to_utf8(value));
        }
    }
private: // Private methods
    void connect_signals(void) {
        this->signal_realize().connect(sigc::bind<bool>(SIGC(set_hand_cursor), true));
        this->signal_unrealize().connect(sigc::bind<bool>(SIGC(set_hand_cursor), false));
    }
    void set_hand_cursor(bool value) {
        Glib::RefPtr<Gdk::Window> window = this->get_event_window();
        if (window) {
            if (value) {
                window->set_cursor(Gdk::Cursor::create(Gdk::CursorType::HAND1));
            } else window->set_cursor(Glib::RefPtr<Gdk::Cursor>());
            this->get_display()->flush();
        }
    }
public: // Public properties
    tools::dispatcher::ptr dispatcher(void) {
        return this->dispatcher_.lock();
    }
    void dispatcher(tools::dispatcher::ptr dispatcher) {
        this->dispatcher_ = dispatcher;
    }
protected: // Private fields
    tools::dispatcher::wptr dispatcher_;
};

}}

#endif // INITPLUS_GTK_LINK_BUTTON_HPP_INCLUDED
