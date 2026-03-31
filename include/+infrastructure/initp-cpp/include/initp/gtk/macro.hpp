#ifndef INITPLUS_GTK_MACRO_HPP_INCLUDED
#define INITPLUS_GTK_MACRO_HPP_INCLUDED

#define DISPATCH(d, f, ...) d->dispatch(initp::tools::weak::bind<void(void)>(std::bind(&self_type::f, this, ##__VA_ARGS__), d))

#define SIGC(f) sigc::mem_fun(this, &self_type::f)

#endif // INITPLUS_GTK_MACRO_HPP_INCLUDED
