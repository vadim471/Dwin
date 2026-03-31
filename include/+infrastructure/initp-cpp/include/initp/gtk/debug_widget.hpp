#ifndef INITPLUS_GTK_DEBUG_WIDGET_HPP_INCLUDED
#define INITPLUS_GTK_DEBUG_WIDGET_HPP_INCLUDED

#include "../tools/enable_class_name.hpp"
#include "../tools/event_proxy_c11.hpp"

#include <functional>

#include <gtkmm-3.0/gtkmm.h>

namespace initp {
namespace gtk {

template<typename T, bool show_debug = true>
class debug_widget:
    public T,
    public virtual tools::enable_class_name<T> {
private:
    typedef debug_widget<T> self_type;
public:
    typedef tools::event_proxy<void(void)> events;
public:
    debug_widget(typename T::BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>&):
        T(cobject) {
        if (!this->debug_name_.empty()) {
            TRACE_("Widget created: ", this->class_name(), " with name ", this->debug_name_);
        } else {
            TRACE_("Widget created: ", this->class_name());
        }
    }
    virtual ~debug_widget(void) {
        this->on_delete_.rise();
        if (!this->debug_name_.empty()) {
            TRACE_("Widget deleted: ", this->class_name(), " with name ", this->debug_name_);
        } else {
            TRACE_("Widget deleted: ", this->class_name());
        }
    };
public:
    const std::string& debug_name(void) {
        return this->debug_name_;
    }
    void debug_name(const std::string& value) {
        this->debug_name_ = value;
    }
    events& on_delete(void) {
        return this->on_delete_;
    }
private:
    std::string debug_name_;
    events on_delete_;
};

}}}}

#endif // INITPLUS_GTK_DEBUG_WIDGET_HPP_INCLUDED
