#ifndef ODDJSON_OJSONBUILDER_H
#define ODDJSON_OJSONBUILDER_H

#include "clingo/lang/CObject.h"
#include "oddjson/apidecl.h"
#include "oddjson/error.h"

/*******************************************************************************
********************************************************* Types and Definitions
********************************************************************************
 
*******************************************************************************/

ODDJSON_API extern cMeta const O_JsonBuilderMeta;

struct OJsonBuilder;
typedef struct OJsonBuilder OJsonBuilder;

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

ODDJSON_API OJsonBuilder* new_json_builder_o( void );

#define make_json_builder_o_( Prefix, Indent )                                 \
   make_json_builder_o( c_c(Prefix), c_c(Indent) )
ODDJSON_API OJsonBuilder* make_json_builder_o( cChars prefix, cChars indent );

/*******************************************************************************

*******************************************************************************/

ODDJSON_API bool json_builder_has_error_o( OJsonBuilder* b );

ODDJSON_API bool push_json_builder_error_o( cErrorStack es[static 1],
                                            OJsonBuilder* b );

/*******************************************************************************

*******************************************************************************/

ODDJSON_API cChars built_json_o( OJsonBuilder* b );

ODDJSON_API char const* built_json_cstr_o( OJsonBuilder* b );

/*******************************************************************************

*******************************************************************************/

#define begin_json_object_o_( Builder, Name )                                  \
   begin_json_object_o( (Builder), c_c( Name ) )
ODDJSON_API bool begin_json_object_o( OJsonBuilder* b, cChars name );

ODDJSON_API bool begin_json_object_value_o( OJsonBuilder* b );

ODDJSON_API bool finish_json_object_o( OJsonBuilder* b );

/**************************************/

#define begin_json_array_o_( Builder, Name )                                   \
   begin_json_array_o( (Builder), c_c( Name ) )
ODDJSON_API bool begin_json_array_o( OJsonBuilder* b, cChars name );

ODDJSON_API bool begin_json_array_value_o( OJsonBuilder* b );

ODDJSON_API bool finish_json_array_o( OJsonBuilder* b );

/*******************************************************************************

*******************************************************************************/

#define append_json_bool_o_( Builder, Name, Value )                            \
   append_json_bool_o( (Builder), c_c( Name ), (Value) )
ODDJSON_API bool append_json_bool_o( OJsonBuilder* b, cChars name, bool value );

ODDJSON_API bool append_json_bool_value_o( OJsonBuilder* b, bool value );

/**************************************/

#define append_json_null_o_( Builder, Name )                                   \
   append_json_null_o( (Builder), c_c( Name ) )
ODDJSON_API bool append_json_null_o( OJsonBuilder* b, cChars name );

ODDJSON_API bool append_json_null_value_o( OJsonBuilder* b );

/**************************************/

#define append_json_fmt_number_o_( Builder, Name, Value, Fmt )                 \
   append_json_fmt_number_o( (Builder), c_c( Name ), (Value), (Fmt) )
ODDJSON_API bool append_json_fmt_number_o( OJsonBuilder* b,
                                           cChars name,
                                           double value,
                                           char const fmt[static 1] );

ODDJSON_API bool append_json_fmt_number_value_o( OJsonBuilder* b,
                                                 double value,
                                                 char const fmt[static 1] );

/**************************************/

#define append_json_number_o_( Builder, Name, Value )                          \
   append_json_number_o( (Builder), c_c( Name ), (Value) )
ODDJSON_API
bool append_json_number_o( OJsonBuilder* b, cChars name, double value );

ODDJSON_API bool append_json_number_value_o( OJsonBuilder* b, double value );

/**************************************/

#define append_json_string_o_( Builder, Name, Value )                          \
   append_json_string_o( (Builder), c_c( Name ), (Value) )
ODDJSON_API
bool append_json_string_o( OJsonBuilder* b, cChars name, cChars value );

ODDJSON_API bool append_json_string_value_o( OJsonBuilder* b, cChars value );

#endif