#ifndef ODDJSON_OJSONTYPE_H
#define ODDJSON_OJSONTYPE_H

#include "oddjson/apidecl.h"

/*******************************************************************************
********************************************************* Types and Definitions
********************************************************************************
 
*******************************************************************************/

#define oJSON_TYPE_                                                            \
   XMAP_C_( o_JsonObject, 0 )                                                  \
   XMAP_C_( o_JsonArray, 1 )                                                   \
   XMAP_C_( o_JsonString, 2 )                                                  \
   XMAP_C_( o_JsonNumber, 3 )                                                  \
   XMAP_C_( o_JsonBool, 4 )                                                    \
   XMAP_C_( o_JsonNull, 5 )

#define XMAP_C_( N, I ) N = I,
enum o_JsonType { oJSON_TYPE_ };
#undef XMAP_C_

typedef enum o_JsonType o_JsonType;

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

ODDJSON_API char const* stringify_json_type_c( o_JsonType type );

#endif