#ifndef ODDJSON_OJSONBUILDER_H
#define ODDJSON_OJSONBUILDER_H

#include "clingo/clingo.h"
#include "oddjson/apidecl.h"
#include "oddjson/error.h"
#include "oddjson/OJson.h"

/*******************************************************************************
********************************************************* Types and Definitions
********************************************************************************
 
*******************************************************************************/

struct oJsonBuilderInternal;
typedef struct oJsonBuilderInternal oJsonBuilderInternal;

struct oJsonBuilder {
   cChars prefix;
   cChars indent;
   oJsonBuilderInternal* _;
};
typedef struct oJsonBuilder oJsonBuilder;

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

#define json_builder_o_( Prefix, Indent )                                      \
(                                                                              \
   (oJsonBuilder){ .prefix=c_c(Prefix), .indent=c_c(Indent), ._=NULL }         \
)

ODDJSON_API bool init_json_builder_o( oJsonBuilder b[static 1], int64_t cap );

ODDJSON_API bool init_json_builder_shell_o( oJsonBuilder b[static 1],
                                            cRecorder rec[static 1] );

ODDJSON_API void cleanup_json_builder_o( oJsonBuilder b[static 1] );

/*******************************************************************************

*******************************************************************************/

ODDJSON_API bool json_builder_has_error_o( oJsonBuilder b[static 1] );

ODDJSON_API bool push_json_builder_error_o( cErrorStack es[static 1],
                                            oJsonBuilder b[static 1] );

/*******************************************************************************

*******************************************************************************/

ODDJSON_API cChars built_json_o( oJsonBuilder b[static 1] );

ODDJSON_API char const* built_json_cstr_o( oJsonBuilder b[static 1] );

/*******************************************************************************

*******************************************************************************/

#define begin_json_object_o_( Builder, Name )                                  \
   begin_json_object_o( (Builder), c_c( Name ) )
ODDJSON_API bool begin_json_object_o( oJsonBuilder b[static 1], cChars name );

ODDJSON_API bool begin_json_object_value_o( oJsonBuilder b[static 1] );

ODDJSON_API bool finish_json_object_o( oJsonBuilder b[static 1] );

/**************************************/

#define begin_json_array_o_( Builder, Name )                                   \
   begin_json_array_o( (Builder), c_c( Name ) )
ODDJSON_API bool begin_json_array_o( oJsonBuilder b[static 1], cChars name );

ODDJSON_API bool begin_json_array_value_o( oJsonBuilder b[static 1] );

ODDJSON_API bool finish_json_array_o( oJsonBuilder b[static 1] );

/*******************************************************************************

*******************************************************************************/

#define add_json_bool_o_( Builder, Name, Value )                               \
   add_json_bool_o( (Builder), c_c( Name ), (Value) )
ODDJSON_API bool add_json_bool_o( oJsonBuilder b[static 1],
                                  cChars name,
                                  bool value );

ODDJSON_API bool add_json_bool_value_o( oJsonBuilder b[static 1], bool value );

/**************************************/

#define add_json_null_o_( Builder, Name )                                      \
   add_json_null_o( (Builder), c_c( Name ) )
ODDJSON_API bool add_json_null_o( oJsonBuilder b[static 1], cChars name );

ODDJSON_API bool add_json_null_value_o( oJsonBuilder b[static 1] );

/**************************************/

#define add_json_fmt_number_o_( Builder, Name, Value, Fmt )                    \
   add_json_fmt_number_o( (Builder), c_c( Name ), (Value), (Fmt) )
ODDJSON_API bool add_json_fmt_number_o( oJsonBuilder b[static 1],
                                        cChars name,
                                        double value,
                                        char const fmt[static 1] );

ODDJSON_API bool add_json_fmt_number_value_o( oJsonBuilder b[static 1],
                                              double value,
                                              char const fmt[static 1] );

/**************************************/

#define add_json_number_o_( Builder, Name, Value )                             \
   add_json_number_o( (Builder), c_c( Name ), (Value) )
ODDJSON_API
bool add_json_number_o( oJsonBuilder b[static 1], cChars name, double value );

ODDJSON_API bool add_json_number_value_o( oJsonBuilder b[static 1],
                                          double value );

/**************************************/

#define add_json_string_o_( Builder, Name, Value )                             \
   add_json_string_o( (Builder), c_c( Name ), (Value) )
ODDJSON_API
bool add_json_string_o( oJsonBuilder b[static 1], cChars name, cChars value );

ODDJSON_API bool add_json_string_value_o( oJsonBuilder b[static 1],
                                          cChars value );

/**************************************/

#define add_json_array_o_( Builder, Name, Arr )                                \
   add_json_array_o( (Builder), c_c( Name ), (Arr) )
ODDJSON_API bool add_json_array_o( oJsonBuilder b[static 1],
                                   cChars name,
                                   OJsonArray const* arr );

ODDJSON_API bool add_json_array_value_o( oJsonBuilder b[static 1],
                                         OJsonArray const* arr );

/**************************************/

#define add_json_object_o_( Builder, Name, Obj )                               \
   add_json_object_o( (Builder), c_c( Name ), (Obj) )
ODDJSON_API bool add_json_object_o( oJsonBuilder b[static 1],
                                    cChars name,
                                    OJsonObject const* obj );

ODDJSON_API bool add_json_object_value_o( oJsonBuilder b[static 1],
                                          OJsonObject const* obj );

/**************************************/

#define add_json_o_( Builder, Name, Value )                                    \
   add_json_o( (Builder), c_c( Name ), (Value) )
ODDJSON_API bool add_json_o( oJsonBuilder b[static 1],
                             cChars name,
                             OJson const* value );

ODDJSON_API bool add_json_value_o( oJsonBuilder b[static 1],
                                   OJson const* value );

#endif