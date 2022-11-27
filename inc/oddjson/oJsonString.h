#ifndef ODDJSON_UTIL_H
#define ODDJSON_UTIL_H

#include "clingo/io/cRecorder.h"
#include "clingo/io/cScanner.h"
#include "clingo/string/CString.h"
#include "oddjson/apidecl.h"

/*******************************************************************************
********************************************************* Types and Definitions
********************************************************************************
 
*******************************************************************************/

struct oJsonString
{
   cChars quoted;
   cChars raw;
   int64_t byteLength;
   int64_t length;
};
typedef struct oJsonString oJsonString;

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

ODDJSON_API cChars decode_json_string_chars_o( oJsonString const jstr[static 1],
                                               cVarChars buf );

ODDJSON_API CString* decode_json_string_o( oJsonString const jstr[static 1] );

/*******************************************************************************

*******************************************************************************/

#define json_string_is_o_( Jstr, Cstr )                                        \
   json_string_is_o( (Jstr), c_c( Cstr ) )
ODDJSON_API bool json_string_is_o( oJsonString const jstr[static 1],
                                   cChars chars );

#define iterate_json_string_o_( Itr, Rune, Jstr )                              \
   for ( char const* Itr = next_json_string_rune_o( Jstr, NULL, &Rune );       \
         Itr != NULL;                                                          \
         Itr = next_json_string_rune_o( Jstr, Itr, &Rune ) )

ODDJSON_API char const* next_json_string_rune_o( oJsonString const jstr[static 1],
                                                 char const* itr,
                                                 cRune r[static 1] );

/*******************************************************************************

*******************************************************************************/

#define move_if_json_string_is_o_( Sca, Cstr )                                 \
   move_if_json_string_is_o( (Sca), c_c( Cstr ) )
ODDJSON_API bool move_if_json_string_is_o( cScanner sca[static 1],
                                           cChars chars );

ODDJSON_API bool scan_json_string_o( cScanner sca[static 1],
                                     oJsonString jstr[static 1] );

#endif