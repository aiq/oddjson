#include "oddjson/OJsonObject.h"

#include "_/util.h"
#include "oddjson/oJsonParser.h"
#include "oddjson/oJsonRoute.h"
#include "clingo/io/jot.h"

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
