#ifndef INITPLUS_GTK_COMBO_BOX_HPP_INCLUDED
#define INITPLUS_GTK_COMBO_BOX_HPP_INCLUDED

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

template<typename T>
class combo_box:
    public Gtk::ComboBox {
private: // Private types
    typedef combo_box<T> self_type;
public: // Construction
    combo_box(void):
        Gtk::ComboBox(),
        column_index_(-1) {}
    combo_box(tools::dispatcher::ptr d):
        Gtk::ComboBox(),
        dispatcher_(d),
        column_index_(-1) {}
    combo_box(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder):
        Gtk::ComboBox(cobject),
        column_index_(-1) {}
    virtual ~combo_box(void) {}
public: // Public methods
    void dispatch_active_value(T value) {
        if (tools::dispatcher::ptr d = this->dispatcher()) {
            DISPATCH(d, set_active_value, value);
        }
    }
    void set_active_value(T value) {
        if (this->column_index_ < 0) return;
        T row;
        Gtk::TreeModel::const_iterator end = this->get_model()->children().end();
        for (Gtk::TreeModel::const_iterator it = this->get_model()->children().begin(); it != end; ++it) {
            it->get_value(this->column_index_, row);
            if (row == value) {
                this->set_active(it);
                return;
            }
        }
        this->set_active(-1);
    }
public: // Public properties
    tools::dispatcher::ptr dispatcher(void) {
        return this->dispatcher_.lock();
    }
    void dispatcher(tools::dispatcher::ptr dispatcher) {
        this->dispatcher_ = dispatcher;
    }
    int column_index(void) const {
        return this->column_index_;
    }
    void column_index(int value) {
        this->column_index_ = value;
    }
protected: // Private fields
    tools::dispatcher::wptr dispatcher_;
    int column_index_;
};

template<typename T>
class bi_combo_box:
    public gtk::combo_box<T>,
    public std::enable_shared_from_this<bi_combo_box<T>> {
private: // Private types
    typedef bi_combo_box<T> self_type;
public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::vector<ptr> vector;
public: // Constructors
    bi_combo_box(typename combo_box<T>::BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder):
        combo_box<T>(cobject, builder),
        bind_(NULL),
        shared_(false) {
        this->connect_signals();
    }
    bi_combo_box(void):
        combo_box<T>(),
        bind_(NULL),
        shared_(true) {
        //INFO_("gtk::bi_combo_box();");
        this->connect_signals();
    }
    bi_combo_box(tools::dispatcher::ptr dispatcher):
        combo_box<T>(dispatcher),
        bind_(NULL),
        shared_(true) {
        //INFO_("gtk::bi_combo_box(tools::dispatcher::ptr);");
        this->connect_signals();
    }
    bi_combo_box(const self_type&) = delete;
public: // Destructor
    virtual ~bi_combo_box(void) {
        //INFO_("gtk::~bi_combo_box();");
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
                T value = this->bind_->parse<T>();
                if (this->shared_) {
                    d->dispatch(BIND(set_active_value, value));
                } else DISPATCH(d, set_active_value, value);
            }
        }
    }
    void remove_binding(void) {
        this->bind_ = NULL;
    }
    void connect_signals(void) {
        this->signal_changed().connect(SIGC(on_combo_changed));
    }
    void on_combo_changed(void) {
        if (this->bind_) {
            Gtk::TreeModel::const_iterator selected = this->get_active();
            if (!selected) {
                this->bind_->store(T(0), this);
            } else {
                T value;
                selected->get_value(this->column_index_, value);
                this->bind_->store(value, this);
            }
        }
    }
protected: // Private fields
    tools::bindable* bind_;
    bool shared_;
};

}}

#endif // INITPLUS_GTK_COMBO_BOX_HPP_INCLUDED
