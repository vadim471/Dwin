#ifndef INITPLUS_SYSTEM_COMPILER_HPP_INCLUDED
#define INITPLUS_SYSTEM_COMPILER_HPP_INCLUDED

#ifdef likely
#undef likely
#endif // likely

#ifdef unlikely
#undef unlikely
#endif // unlikely

#ifndef NO_INLINE
#undef NO_INLINE
#endif // NO_INLINE

#ifndef RETURNS_NON_NULL
#undef RETURNS_NON_NULL
#endif // RETURNS_NON_NULL

#if defined(__GNUC__)

#define likely() __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define NO_INLINE __attribute__((__noinline__))
#define RETURNS_NON_NULL __attribute__((returns_nonnull))

#else // __GNUC__

#define likely(x) (x)
#define unlikely(x) (x)
#define NO_INLINE
#define RETURNS_NON_NULL

#endif // !__GNUC__

#endif // INITPLUS_SYSTEM_COMPILER_HPP_INCLUDED
