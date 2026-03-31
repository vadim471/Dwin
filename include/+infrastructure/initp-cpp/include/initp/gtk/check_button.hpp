#ifndef INITPLUS_GTK_CHECK_BUTTON_HPP_INCLUDED
#define INITPLUS_GTK_CHECK_BUTTON_HPP_INCLUDED

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

class check_button:
    public Gtk::CheckButton {
private: // Private types
    typedef check_button self_type;
public: // Construction
    check_button(void):
        Gtk::CheckButton() {}
    check_button(tools::dispatcher::ptr d):
        Gtk::CheckButton(),
        dispatcher_(d) {}
    check_button(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder):
        Gtk::CheckButton(cobject) {}
    virtual ~check_button(void) {}
public: // Public methods
    void dispatch_active(bool value) {
        if (tools::dispatcher::ptr d = this->dispatcher()) {
            DISPATCH(d, set_active, value);
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

class bi_check_button:
    public gtk::check_button,
    public std::enable_shared_from_this<bi_check_button> {
private: // Private types
    typedef bi_check_button self_type;
public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::vector<ptr> vector;
public: // Constructors
    bi_check_button(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder):
        check_button(cobject, builder),
        bind_(NULL),
        shared_(false) {
        this->connect_signals();
    }
    bi_check_button(void):
        check_button(),
        bind_(NULL),
        shared_(true) {
        //INFO_("ui::bi_check_button();");
        this->connect_signals();
    }
    bi_check_button(tools::dispatcher::ptr dispatcher):
        check_button(dispatcher),
        bind_(NULL),
        shared_(true) {
        //INFO_("ui::bi_check_button(i::dispatcher::ptr);");
        this->connect_signals();
    }
    bi_check_button(const self_type&) = delete;
public: // Destructor
    virtual ~bi_check_button(void) {
        //INFO_("ui::~bi_check_button();");
        this->unbind();
    }
public: // Public methods
    self_type* bind(tools::bind<bool>* value) {
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
    self_type* bind(tools::bind<bool>* value, tools::dispatcher::ptr dispatcher) {
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
                double value = this->bind_->value();
                if (this->shared_) {
                    d->dispatch(BIND(set_active, value));
                } else DISPATCH(d, set_active, value);
            }

        }
    }
    void remove_binding(void) {
        this->bind_ = NULL;
    }
    void connect_signals(void) {
        this->signal_toggled().connect(SIGC(on_check_active_changed));
    }
    void on_check_active_changed(void) {
        if (this->bind_) {
            this->bind_->value(this->get_active());
        }
    }
protected: // Private fields
    tools::bind<bool>* bind_;
    bool shared_;
};

}}

#endif // INITPLUS_GTK_CHECK_BUTTON_HPP_INCLUDED
