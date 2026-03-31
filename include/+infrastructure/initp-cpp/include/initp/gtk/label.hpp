#ifndef INITPLUS_GTK_LABEL_INCLUDED
#define INITPLUS_GTK_LABEL_INCLUDED

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

class label:
    public Gtk::Label {
private: // Private types
    typedef label self_type;
public: // Construction
    label(void): Gtk::Label() {}
    label(tools::dispatcher::ptr d):
        Gtk::Label(),
        dispatcher_(d) {}
    label(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder):
        Gtk::Label(cobject) {}
    virtual ~label(void) {}
public: // Public methods
    void dispatch_label(const Glib::ustring& s) {
        if (tools::dispatcher::ptr d = this->dispatcher()) {
            DISPATCH(d, set_label, s);
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

class label_chunk {
public: // Constructors
    label_chunk(const std::string& s):
        string_(s),
        value_(NULL),
        is_text_(true)
    {}
    label_chunk(tools::bindable* v):
        string_(),
        value_(v),
        is_text_(false)
    {}
    label_chunk(const label_chunk& c):
        string_(c.is_text_ ? c.string_ : ""),
        value_(c.is_text_ ? NULL : c.value_),
        is_text_(c.is_text_)
    {}
public: // Operators
    friend std::ostream& operator<<(std::ostream& stream, const label_chunk& instance) {
        if (instance.is_text_) {
            stream << instance.string_;
        } else stream << *instance.value_;
        return stream;
    }
public: // Public methods
    bool is_binded_to(tools::bindable* v) const {
        if (!this->is_text_) {
            return this->value_ == v;
        } else return false;
    }
    void unbind(void* t) {
        if (this->value_) {
            this->is_text_ = true;
            this->value_->on_change().remove(t);
            this->value_->on_delete().remove(t);
            this->value_ = NULL;
        }
    }
private: // Private fields
    std::string string_;
    tools::bindable* value_;
    bool is_text_;
};

class bi_label:
    public gtk::label,
    public std::enable_shared_from_this<bi_label> {
private: // Private types
    typedef bi_label self_type;
public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::vector<ptr> vector;
public: // Constructors
    bi_label(void):
        label(),
        shared_(false),
        precision_(-1) {
        //INFO_("ui::bi_label();");
    }
    bi_label(tools::dispatcher::ptr dispatcher):
        label(dispatcher),
        shared_(false),
        precision_(-1) {
        //INFO_("ui::bi_label(tools::dispatcher::ptr);");
    }
    bi_label(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder):
        label(cobject, builder),
        shared_(false),
        precision_(-1) {}
    bi_label(const self_type&) = delete;
public: // Destructor
    virtual ~bi_label(void) {
        //INFO_("ui::~bi_label();");
        this->unbind();
    }
public: // Public methods
    template<typename T>
    self_type* append(const T& value) {
        std::stringstream s;
        s << value;
        this->chunks_.push_back(label_chunk(s.str()));
        this->update();
        return this;
    }
    self_type* bind(tools::bindable* value) {
        if (value) {
            if (this->shared_) {
                ptr _this = this->shared_from_this();
                value->on_change().connect(tools::weak::bind<void(void)>(RBIND(update), _this), this);
                value->on_delete().connect(tools::weak::bind<void(void)>(RBIND(remove_binding, value), _this), this);
            } else {
                if (tools::dispatcher::ptr d = this->dispatcher()) {
                    value->on_change().connect(tools::weak::bind<void(void)>(RBIND(update), d), this);
                    value->on_delete().connect(tools::weak::bind<void(void)>(RBIND(remove_binding, value), d), this);
                } else {
                    value->on_change().connect(RBIND(update), this);
                    value->on_delete().connect(RBIND(remove_binding, value), this);
                }
            }
            this->chunks_.push_back(label_chunk(value));
        }
        this->update();
        return this;
    }
    self_type* bind(tools::bindable* value, tools::dispatcher::ptr dispatcher) {
        this->dispatcher(dispatcher);
        return this->bind(value);
    }
    void unbind(void) {
        for each_i(std::vector<label_chunk>, it, this->chunks_) {
            it->unbind(this);
        }
    }
    void precision(int value) {
        this->precision_ = value;
    }
protected: // Private methods
    void update(void) {
        if (tools::dispatcher::ptr d = this->dispatcher()) {
            std::stringstream s;
            if (this->precision_ >= 0) {
                s << std::fixed << std::showpoint;
                s << std::setprecision(this->precision_);
            }
            for each_c(std::vector<label_chunk>, it, this->chunks_)
                s << IT;
            if (this->shared_) {
                d->dispatch(BIND(set_label, utils::to_utf8(s.str())));
            } else DISPATCH(d, set_label, utils::to_utf8(s.str()));
        }
    }
    void remove_binding(tools::bindable* v) {
        for each_i(std::vector<label_chunk>, it, this->chunks_) {
            if (it->is_binded_to(v)) {
                this->chunks_.erase(it);
                break;
            }
        }
    }
protected: // Private fields
    bool shared_;
    std::vector<label_chunk> chunks_;
    int precision_;
};

}}

#endif // INITPLUS_GTK_LABEL_INCLUDED
