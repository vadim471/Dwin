#ifndef INITPLUS_GTK_ENTRY_HPP_INCLUDED
#define INITPLUS_GTK_ENTRY_HPP_INCLUDED

#include "macro.hpp"
#include "utils.hpp"
#include "../utils/macro_c11.hpp"
#include "../tools/bind.hpp"
#include "../tools/weak_bind.hpp"
#include "../tools/dispatch.hpp"
#include "../utils/string.hpp"

#include <iostream>
#include <memory>
#include <functional>

#include <gtkmm-3.0/gtkmm.h>

namespace initp {
namespace gtk {

class entry:
    public Gtk::Entry {
private: // Private types
    typedef entry self_type;
public: // Construction
    entry(void):
        Gtk::Entry(),
        focus_(NULL) {}
    entry(tools::dispatcher::ptr d):
        Gtk::Entry(),
        dispatcher_(d),
        focus_(NULL) {}
    entry(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder):
        Gtk::Entry(cobject),
        focus_(NULL) {}
    virtual ~entry(void) {}
public: // Public methods
    void dispatch_text(const Glib::ustring& s) {
        if (tools::dispatcher::ptr d = this->dispatcher()) {
            DISPATCH(d, set_text, s);
        }
    }
    void focus_on_enter(Gtk::Widget* widget) {
        this->focus_ = widget;
    }
protected: // Protected methods
    virtual bool on_key_release_event(GdkEventKey* key_event) {
        bool result = Gtk::Entry::on_key_release_event(key_event);
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

class bi_entry:
    public gtk::entry,
    public std::enable_shared_from_this<bi_entry> {
private: // Private types
    typedef bi_entry self_type;
public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::vector<ptr> vector;
public: // Constructors
    bi_entry(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder):
        entry(cobject, builder),
        bind_(NULL),
        shared_(false),
        precision_(-1),
        fixed_(false) {
        this->connect_signals();
    }
    bi_entry(void):
        entry(),
        bind_(NULL),
        shared_(true),
        precision_(-1),
        fixed_(false) {
        //MEMORY_("gtk::bi_entry();");
        this->connect_signals();
    }
    bi_entry(tools::dispatcher::ptr dispatcher):
        entry(dispatcher),
        bind_(NULL),
        shared_(true),
        precision_(-1),
        fixed_(false) {
        //MEMORY_("gtk::bi_entry(tools::dispatcher::ptr);");
        this->connect_signals();
    }
    bi_entry(const self_type&) = delete;
public: // Destructor
    virtual ~bi_entry(void) {
        //MEMORY_("gtk::~bi_entry();");
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
    void precision(int value, bool fixed) {
        this->precision_ = value;
        this->fixed_ = fixed;
    }
protected: // Private methods
    void update(void) {
        if (this->bind_) {
            if (tools::dispatcher::ptr d = this->dispatcher()) {
                std::stringstream s;
                if (this->precision_ >= 0) {
                    s << std::fixed << std::showpoint;
                    s << std::setprecision(this->precision_);
                }
                s << *this->bind_;
                std::string string = s.str();
                if (this->precision_ >= 0 && !this->fixed_)
                    initp::utils::trim_float(string);
                if (this->shared_) {
                    d->dispatch(BIND(set_text, utils::to_utf8(string)));
                } else DISPATCH(d, set_text, utils::to_utf8(string));
            }

        }
    }
    void remove_binding(void) {
        this->bind_ = NULL;
    }
    void connect_signals(void) {
        this->get_buffer()->signal_inserted_text().connect(SIGC(on_text_insert));
        this->get_buffer()->signal_deleted_text().connect(SIGC(on_text_delete));
    }
    void on_text_insert(guint position, const gchar* chars, guint n_chars) {
        this->on_text_change();
    }
    void on_text_delete(guint position, guint n_chars) {
        this->on_text_change();
    }
    void on_text_change(void) {
        if (this->bind_) {
            std::string text = utils::to_locale(this->get_buffer()->get_text());
            this->bind_->store(text, this);
        }
    }
protected: // Private fields
    tools::bindable* bind_;
    bool shared_;
    int precision_;
    bool fixed_;
};

}}

#endif // INITPLUS_GTK_ENTRY_HPP_INCLUDED
