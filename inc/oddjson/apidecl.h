#ifndef ODDJSON_APIDECL_H
#define ODDJSON_APIDECL_H

#ifdef ODDJSON_STATIC_DEFINE
#  define ODDJSON_API
#  define ODDJSON_NO_EXPORT
#else
#  ifdef _WIN32
#    ifndef ODDJSON_API
#      ifdef ODDJSON_EXPORTS
#        define ODDJSON_API __declspec(dllexport)
#      else
#        define ODDJSON_API __declspec(dllimport)
#      endif
#    endif
#  else
#    define ODDJSON_API
#    define ODDJSON_NO_EXPORT
#  endif
#
#  ifndef ODDJSON_NO_EXPORT
#    define ODDJSON_NO_EXPORT 
#  endif
#endif

#endif