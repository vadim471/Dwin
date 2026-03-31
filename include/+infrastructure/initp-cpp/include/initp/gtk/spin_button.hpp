#ifndef INITPLUS_GTK_SPIN_BUTTON_HPP_INCLUDED
#define INITPLUS_GTK_SPIN_BUTTON_HPP_INCLUDED

#include "macro.hpp"
#include "utils.hpp"
#include "../utils/macro_c11.hpp"
#include "../tools/bind.hpp"
#include "../tools/weak_bind.hpp"
#include "../tools/dispatch.hpp"

#include <iostream>
#include <memory>
#include <functional>

#include <gtkmm-3.0/gtkmm.h>

namespace initp {
namespace gtk {

class spin_button:
    public Gtk::SpinButton {
private: // Private types
    typedef spin_button self_type;
public: // Construction
    spin_button(void):
        Gtk::SpinButton(),
        focus_(NULL) {}
    spin_button(tools::dispatcher::ptr d):
        Gtk::SpinButton(),
        dispatcher_(d),
        focus_(NULL) {}
    spin_button(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder):
        Gtk::SpinButton(cobject),
        focus_(NULL) {}
    virtual ~spin_button(void) {}
public: // Public methods
    void dispatch_value(double value) {
        if (tools::dispatcher::ptr d = this->dispatcher()) {
            DISPATCH(d, set_value, value);
        }
    }
    void focus_on_enter(Gtk::Widget* widget) {
        this->focus_ = widget;
    }
protected: // Protected methods
    virtual bool on_key_release_event(GdkEventKey* key_event) {
        bool result = Gtk::SpinButton::on_key_release_event(key_event);
        if (this->focus_ && key_event->keyval == GDK_KEY_Return)
            this->focus_->grab_focus();
        return result;
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
    Gtk::Widget* focus_;
};

class bi_spin_button:
    public gtk::spin_button,
    public std::enable_shared_from_this<bi_spin_button> {
private: // Private types
    typedef bi_spin_button self_type;
public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::vector<ptr> vector;
public: // Constructors
    bi_spin_button(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder):
        spin_button(cobject, builder),
        bind_(NULL),
        shared_(false) {
        this->connect_signal();
    }
    bi_spin_button(void):
        spin_button(),
        bind_(NULL),
        shared_(true) {
        //INFO_("gtk::bi_spin_button();");
        this->connect_signal();
    }
    bi_spin_button(tools::dispatcher::ptr dispatcher):
        spin_button(dispatcher),
        bind_(NULL),
        shared_(true) {
        //INFO_("gtk::bi_spin_button(tools::dispatcher::ptr);");
        this->connect_signal();
    }
    bi_spin_button(const self_type&) = delete;
public: // Destructor
    virtual ~bi_spin_button(void) {
        //INFO_("gtk::~bi_spin_button();");
        this->unbind();
    }
public: // Public methods
    self_type* bind(tools::bindable* value) {
        if (this->bind_) {
            if (this->bind_ != value) {
                this->unbind();
            } else return this;
        }
        if (value) {
            if (this->shared_) {
                ptr _this = this->shared_from_this();
                value->on_change().connect(tools::weak::bind<void(void)>(RBIND(update), _this), this);
                value->on_delete().connect(tools::weak::bind<void(void)>(RBIND(remove_binding), _this), this);
            } else {
                if (tools::dispatcher::ptr d = this->dispatcher()) {
                    value->on_change().connect(tools::weak::bind<void(void)>(RBIND(update), d), this);
                    value->on_delete().connect(tools::weak::bind<void(void)>(RBIND(remove_binding), d), this);
                } else {
                    value->on_change().connect(RBIND(update), this);
                    value->on_delete().connect(RBIND(remove_binding), this);
                }
            }
            this->bind_ = value;
        }
        this->update();
        return this;
    }
    self_type* bind(tools::bindable* value, tools::dispatcher::ptr dispatcher) {
        this->dispatcher(dispatcher);
        return this->bind(value);
    }
    void unbind(void) {
        if (this->bind_) {
            this->bind_->on_change().remove(this);
            this->bind_->on_delete().remove(this);
            this->bind_ = NULL;
        }
    }
protected: // Private methods
    void update(void) {
        if (this->bind_) {
            if (tools::dispatcher::ptr d = this->dispatcher()) {
                double value = this->bind_->parse<double>();
                if (this->shared_) {
                    d->dispatch(BIND(set_value, value));
                } else DISPATCH(d, set_value, value);
            }
        }
    }
    void remove_binding(void) {
        this->bind_ = NULL;
    }
    void connect_signal(void) {
        this->signal_value_changed().connect(SIGC(on_spin_value_changed));
    }
    void on_spin_value_changed(void) {
        if (this->bind_) this->bind_->store(this->get_value(), this);
    }
protected: // Private fields
    tools::bindable* bind_;
    bool shared_;
};

}}

#endif // INITPLUS_GTK_SPIN_BUTTON_HPP_INCLUDED
