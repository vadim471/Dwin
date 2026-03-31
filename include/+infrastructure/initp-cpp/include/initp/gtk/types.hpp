#ifndef INITPLUS_GTK_TYPES_HPP_INCLUDED
#define INITPLUS_GTK_TYPES_HPP_INCLUDED

#include <memory>

namespace initp {
namespace gtk {

class label;
class bi_label;

class entry;
class bi_entry;

class spin_button;
class bi_spin_button;

class check_button;
class bi_check_button;

class link_button;

template<typename T>
class combo_box;
template<typename T>
class bi_combo_box;

class builder;
typedef std::shared_ptr<builder> builder_ptr;

}}

#endif // INITPLUS_GTK_TYPES_HPP_INCLUDED
