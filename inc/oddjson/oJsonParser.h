#ifndef ODDJSON_OJSON_PARSER_H
#define ODDJSON_OJSON_PARSER_H

#include "clingo/clingo.h"
#include "oddjson/apidecl.h"
#include "oddjson/error.h"
#include "oddjson/oJsonString.h"
#include "oddjson/OJson.h"

/*******************************************************************************
********************************************************* Types and Definitions
********************************************************************************
 
*******************************************************************************/

struct oJsonParser
{
   cScanner ownSca;
   cScanner* sca;
   char const* err;
};
typedef struct oJsonParser oJsonParser;

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

ODDJSON_API bool init_json_parser_o( oJsonParser p[static 1], cChars full );

ODDJSON_API bool init_json_parser_shell_o( oJsonParser p[static 1],
                                           cScanner sca[static 1] );

/*******************************************************************************

*******************************************************************************/

ODDJSON_API bool push_json_parser_error_o( cErrorStack es[static 1],
                                           oJsonParser const p[static 1] );

/*******************************************************************************

*******************************************************************************/

#define json_name_is_o_( P, Cstr )                                             \
   json_name_is_o( (P), c_c( Cstr ) )
ODDJSON_API bool json_name_is_o( oJsonParser p[static 1],
                                 cChars exp );

/*******************************************************************************

*******************************************************************************/

ODDJSON_API bool maybe_json_object_o( oJsonParser p[static 1] );

ODDJSON_API bool maybe_json_array_o( oJsonParser p[static 1] );

ODDJSON_API bool maybe_json_string_o( oJsonParser p[static 1] );

ODDJSON_API bool maybe_json_number_o( oJsonParser p[static 1] );

ODDJSON_API bool maybe_json_bool_o( oJsonParser p[static 1] );

ODDJSON_API bool maybe_json_null_o( oJsonParser p[static 1] );

/*******************************************************************************

*******************************************************************************/

ODDJSON_API bool skip_json_member_o( oJsonParser p[static 1] );

ODDJSON_API bool skip_json_value_o( oJsonParser p[static 1] );

/*******************************************************************************

*******************************************************************************/

#define begin_parse_json_object_o_( P )                                        \
   begin_parse_json_object_o( P );                                             \
   while ( parser_in_json_object_o( P ) )
ODDJSON_API bool begin_parse_json_object_o( oJsonParser p[static 1] );

ODDJSON_API bool parser_in_json_object_o( oJsonParser p[static 1] );

ODDJSON_API bool finish_parse_json_object_o( oJsonParser p[static 1] );

/*******************************************************************************

*******************************************************************************/

#define begin_parse_json_array_o_( P )                                         \
   begin_parse_json_array_o( P );                                              \
   while ( parser_in_json_array_o( P ) )
ODDJSON_API bool begin_parse_json_array_o( oJsonParser p[static 1] );

ODDJSON_API bool parser_in_json_array_o( oJsonParser p[static 1] );

ODDJSON_API bool finish_parse_json_array_o( oJsonParser p[static 1] );

/*******************************************************************************

*******************************************************************************/

ODDJSON_API bool view_json_string_o( oJsonParser p[static 1],
                                     oJsonString jstr[static 1] );

ODDJSON_API bool parse_json_number_o( oJsonParser p[static 1],
                                      double value[static 1] );

ODDJSON_API bool parse_json_int_number_o( oJsonParser p[static 1],
                                          int64_t value[static 1] );

ODDJSON_API bool parse_json_bool_o( oJsonParser p[static 1],
                                    bool value[static 1] );

ODDJSON_API bool parse_json_null_o( oJsonParser p[static 1] );

ODDJSON_API bool parse_json_object_o( oJsonParser p[static 1],
                                      OJsonObject* obj );

ODDJSON_API bool parse_json_array_o( oJsonParser p[static 1],
                                     OJsonArray* arr );

ODDJSON_API bool parse_json_o( oJsonParser p[static 1],
                               OJson json[static 1] );

#endif