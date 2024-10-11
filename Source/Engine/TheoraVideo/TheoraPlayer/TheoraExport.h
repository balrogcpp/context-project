
#ifndef TheoraPlayerExport_H
#define TheoraPlayerExport_H
#define THEORAPLAYER_STATIC_DEFINE
#ifdef THEORAPLAYER_STATIC_DEFINE
#  define TheoraPlayerExport
#  define THEORAPLAYER_NO_EXPORT
#else
#  ifndef TheoraPlayerExport
#    ifdef theoraplayer_EXPORTS
        /* We are building this library */
#      define TheoraPlayerExport __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define TheoraPlayerExport __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef THEORAPLAYER_NO_EXPORT
#    define THEORAPLAYER_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef THEORAPLAYER_DEPRECATED
#  define THEORAPLAYER_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef THEORAPLAYER_DEPRECATED_EXPORT
#  define THEORAPLAYER_DEPRECATED_EXPORT TheoraPlayerExport THEORAPLAYER_DEPRECATED
#endif

#ifndef THEORAPLAYER_DEPRECATED_NO_EXPORT
#  define THEORAPLAYER_DEPRECATED_NO_EXPORT THEORAPLAYER_NO_EXPORT THEORAPLAYER_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef THEORAPLAYER_NO_DEPRECATED
#    define THEORAPLAYER_NO_DEPRECATED
#  endif
#endif

#endif /* TheoraPlayerExport_H */
