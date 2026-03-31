#ifndef INITPLUS_GTK_BUILDER_HPP_INCLUDED
#define INITPLUS_GTK_BUILDER_HPP_INCLUDED

#include "debug_widget.hpp"
#include "utils.hpp"

#include <memory>

#include <gtkmm-3.0/gtkmm.h>

namespace initp {
namespace gtk {

class builder:
    public boost::enable_shared_from_this<builder> {
private: // Private types
    typedef builder self_type;
public: // Public types
    typedef boost::shared_ptr<self_type> ptr;
public: // Constructors
    builder(void):
        builder_(Gtk::Builder::create()),
        root_(NULL) {}
    builder(const self_type&) = delete;
public: // Destructor
    virtual ~builder(void) {
        if (this->root_) {
            this->root_->remove();
            delete this->root_;
        }
    }
public: // Public methods
    bool add_from_resource(const char* path, const char* root = "root") {
        try {
            this->builder_->add_from_resource(path);
        } catch (const Gio::ResourceError& ex) {
            //ERROR_("ResourceError: ", ex.what());
            return false;
        } catch (const Glib::MarkupError& ex) {
            //ERROR_("MarkupError: ", ex.what());
            return false;
        } catch (const Gtk::BuilderError& ex) {
            //ERROR_("BuilderError: ", ex.what());
            return false;
        }
        this->builder_->get_widget(root, this->root_);
        return this->root_ != NULL;
    }
    template<typename T>
    T* get_widget(const char* id) {
        T* widget = NULL;
        this->builder_->get_widget(utils::to_utf8(id), widget);
        return widget;
    }
    template<typename T>
    T* get_widget_derived(const char* id) {
        T* widget = NULL;
        this->builder_->get_widget_derived(utils::to_utf8(id), widget);
        return widget;
    }
    template<typename T>
    T* get_object(const char* id) {
        return dynamic_cast<T*>(this->builder_->get_object(utils::to_utf8(id)).release());
    }
    template<typename T>
    ui::debug_widget<T>* get_debug_widget(const char* id) {
        ui::debug_widget<T>* widget = NULL;
        this->builder_->get_widget_derived(id, widget);
        return widget;
    }
    Glib::RefPtr<Gtk::Builder> get(void) {
         return this->builder_;
    }
private: // Private methods
    Glib::RefPtr<Gtk::Builder> builder_;
    Gtk::Window* root_;
};

}}

#endif // INITPLUS_GTK_BUILDER_HPP_INCLUDED
