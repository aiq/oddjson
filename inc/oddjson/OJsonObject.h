#ifndef ODDJSON_OJSONOBJECT_H
#define ODDJSON_OJSONOBJECT_H

#include "clingo/clingo.h"
#include "oddjson/apidecl.h"

/*******************************************************************************
********************************************************* Types and Definitions
********************************************************************************
 
*******************************************************************************/

struct OJson;
typedef struct OJson OJson;

OBJ_OBJ_MAP_DEF_C_(
   ODDJSON_API,      // LibApi
   OJsonObject,      // MapType
   CString,          // KeyType
   OJson,            // ValType
   json_object_o,    // FuncSuffix
   O_JsonObjectMeta  // Meta
)

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

#define in_json_object_o_( Obj, Key )                                          \
   via_chars_in_json_object_o( (Obj), c_c( Key) )

#define get_from_json_object_o_( Obj, Key )                                    \
   get_via_chars_from_json_object_o( (Obj), c_c( Key ) )

#define remove_from_json_object_o_( Obj, Key )                                 \
   remove_via_chars_from_json_object_o( (Obj), c_c( Key ) )

#define set_on_json_object_o_( Obj, Key, Val )                                 \
   set_via_chars_on_json_object_o( (Obj), c_c( Key ), Val )

/*******************************************************************************

*******************************************************************************/

CLINGO_API inline bool via_chars_in_json_object_o( OJsonObject* obj, cChars key )
{
   must_exist_c_( obj );

   CString* keyStr = retain_c( make_string_c( key ) );
   if ( !keyStr ) return false;

   bool ok = in_json_object_o( obj, keyStr );
   release_c( keyStr );
   return ok;
}

CLINGO_API inline OJson* get_via_chars_from_json_object_o( OJsonObject* obj,
                                                           cChars key )
{
   must_exist_c_( obj );

   CString* keyStr = retain_c( make_string_c( key ) );
   if ( !keyStr ) return false;

   OJson* result = get_from_json_object_o( obj, keyStr );
   release_c( keyStr );
   return result;
}

CLINGO_API inline bool remove_via_chars_from_json_object_o( OJsonObject* obj,
                                                            cChars key )
{
   must_exist_c_( obj );

   CString* keyStr = retain_c( make_string_c( key ) );
   if ( !keyStr ) return false;

   bool ok = remove_from_json_object_o( obj, keyStr );
   release_c( keyStr );
   return ok;
}

CLINGO_API inline bool set_via_chars_on_json_object_o( OJsonObject* obj,
                                                       cChars key,
                                                       OJson* val )
{
   must_exist_c_( obj );

   CString* keyStr = retain_c( make_string_c( key ) );
   if ( !keyStr ) return false;

   bool ok = set_on_json_object_o( obj, keyStr, val );
   release_c( keyStr );
   return ok;
}

#endif