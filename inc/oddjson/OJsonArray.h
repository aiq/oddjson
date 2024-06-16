#ifndef ODDJSON_OJSONARRAY_H
#define ODDJSON_OJSONARRAY_H

#include "clingo/container/vec.h"
#include "oddjson/apidecl.h"
#include "oddjson/OJsonObject.h"

/*******************************************************************************
********************************************************* Types and Definitions
********************************************************************************
 
*******************************************************************************/

struct OJson;
typedef struct OJson OJson;

OBJ_VEC_DEF_C_(
   ODDJSON_API,     // LibApi
   OJsonArray,      // VecType
   OJson,           // ObjType
   json_array_o,    // FuncSuffix
   O_JsonArrayMeta  // Meta
)

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/



#endif