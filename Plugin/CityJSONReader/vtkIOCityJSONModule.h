
#ifndef VTKIOCITYJSON_EXPORT_H
#define VTKIOCITYJSON_EXPORT_H

#ifdef VTKIOCITYJSON_STATIC_DEFINE
#  define VTKIOCITYJSON_EXPORT
#  define VTKIOCITYJSON_NO_EXPORT
#else
#  ifndef VTKIOCITYJSON_EXPORT
#    ifdef IOCityJSON_EXPORTS
        /* We are building this library */
#      define VTKIOCITYJSON_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define VTKIOCITYJSON_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef VTKIOCITYJSON_NO_EXPORT
#    define VTKIOCITYJSON_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef VTKIOCITYJSON_DEPRECATED
#  define VTKIOCITYJSON_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef VTKIOCITYJSON_DEPRECATED_EXPORT
#  define VTKIOCITYJSON_DEPRECATED_EXPORT VTKIOCITYJSON_EXPORT VTKIOCITYJSON_DEPRECATED
#endif

#ifndef VTKIOCITYJSON_DEPRECATED_NO_EXPORT
#  define VTKIOCITYJSON_DEPRECATED_NO_EXPORT VTKIOCITYJSON_NO_EXPORT VTKIOCITYJSON_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef VTKIOCITYJSON_NO_DEPRECATED
#    define VTKIOCITYJSON_NO_DEPRECATED
#  endif
#endif

#endif /* VTKIOCITYJSON_EXPORT_H */
