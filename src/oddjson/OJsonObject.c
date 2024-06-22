#include "oddjson/OJsonObject.h"

#include "_/util.h"
#include "oddjson/oJsonParser.h"
#include "oddjson/oJsonRoute.h"

/*******************************************************************************
********************************************************* Types and Definitions
********************************************************************************
 
*******************************************************************************/

OBJ_OBJ_MAP_IMPL_C_(
   ,                 // Static
   OJsonObject,      // MapType
   OJsonObjectRow,   // RowType
   CString,          // KeyType
   OJson,            // ValType
   json_object_o,    // FuncSuffix
   O_JsonObjectMeta, // Meta
   hash_string_c,    // HashFunc
   cmp_string_c      // CmpFunc
)

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

extern inline bool via_chars_in_json_object_o( OJsonObject* obj, cChars key );

extern inline OJson* get_via_chars_from_json_object_o( OJsonObject* obj,
                                                       cChars key );

extern inline bool remove_via_chars_from_json_object_o( OJsonObject* obj,
                                                        cChars key );

extern inline bool set_via_chars_on_json_object_o( OJsonObject* obj,
                                                   cChars key,
                                                   OJson* val );

/*******************************************************************************

*******************************************************************************/
/*
static inline OJson* json_object_goto( OJsonObject* obj,
                                       cCharsSlice path )
{
   must_exist_c_( obj );

   OJson* val = let_json_with_object_o_( obj );
   times_c_( path.s, i )
   {
      cChars key = path.v[i];
      if ( val->type == o_JsonObject )
      {
         val = get_via_chars_from_json_object_o( val->object, key );
      }
      else if ( val->type == o_JsonArray )
      {
         cScanner* sca = &make_scanner_c_( key.s, key.v );
         int64_t pos = 0;
         if ( not read_int64_c_( sca, &pos ) or sca->space != 0 )
         {
            val = NULL;
            break;
         }
         val = get_from_json_array_o( val->array, pos );
      }
      else
      {
         val = NULL;
      }

      if ( val == NULL )
        break;
   }

   release_c( val );
   return val;
}
*/

