#ifndef ODDJSON_OJSON_H
#define ODDJSON_OJSON_H

#include "clingo/container/vec.h"
#include "clingo/io/cScanner.h"
#include "clingo/string/CStringList.h"
#include "oddjson/apidecl.h"
#include "oddjson/OJsonArray.h"
#include "oddjson/OJsonObject.h"

/*******************************************************************************
********************************************************* Types and Definitions
********************************************************************************
 
*******************************************************************************/

#define oJSON_TYPE_                                                            \
   XMAP_C_( o_JsonNull,    0 )                                                 \
   XMAP_C_( o_JsonObject,  1 )                                                 \
   XMAP_C_( o_JsonArray,   2 )                                                 \
   XMAP_C_( o_JsonString,  3 )                                                 \
   XMAP_C_( o_JsonNumber,  4 )                                                 \
   XMAP_C_( o_JsonBool,    5 )

#define XMAP_C_( N, I ) N = I,
enum o_JsonType { oJSON_TYPE_ };
#undef XMAP_C_

typedef enum o_JsonType o_JsonType;

ODDJSON_API extern cMeta const O_JsonMeta;

struct OJson
{
   o_JsonType type;
   union
   {
      CString* string;
      double number;
      OJsonObject* object;
      OJsonArray* array;
      bool boolean;
      void* null;
   };
};
typedef struct OJson OJson;

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

#define new_json_o_()                                                          \
   new_object_c_( OJson, &O_JsonMeta )

#define let_json_o_()                                                          \
   let_object_c_( OJson, &O_JsonMeta )

/*******************************************************************************

*******************************************************************************/

#define new_json_with_string_o_( Str )                                         \
   set_json_string_o( new_json_o_(), (Str) )

#define new_json_with_number_o_( Number )                                      \
   set_json_number_o( new_json_o_(), (Number) )

#define new_json_with_object_o_( Obj )                                         \
   set_json_object_o( new_json_o_(), (Obj) )

#define new_json_with_array_o_( Arr )                                          \
   set_json_array_o( new_json_o_(), (Arr) )

#define new_json_with_bool_o_( Value )                                         \
   set_json_bool_o( new_json_o_(), (Value) )

/*******************************************************************************

*******************************************************************************/

#define let_json_with_string_o_( Str )                                         \
   set_json_string_o( let_json_o_(), (Str) )

#define let_json_with_number_o_( Number )                                      \
   set_json_number_o( let_json_o_(), (Number) )

#define let_json_with_object_o_( Obj )                                         \
   set_json_object_o( let_json_o_(), (Obj) )

#define let_json_with_array_o_( Arr )                                          \
   set_json_array_o( let_json_o_(), (Arr) )

#define let_json_with_bool_o_( Value )                                         \
   set_json_bool_o( let_json_o_(), (Value) )

/*******************************************************************************

*******************************************************************************/

CLINGO_API inline OJson* set_json_string_o( OJson json[static 1], CString* str )
{
   json->type = o_JsonString;
   json->string = retain_c( str );
   return json;
}

CLINGO_API inline OJson* set_json_number_o( OJson json[static 1], double number )
{
   json->type = o_JsonNumber;
   json->number = number;
   return json;
}

CLINGO_API inline OJson* set_json_object_o( OJson json[static 1], OJsonObject* obj )
{
   json->type = o_JsonObject;
   json->object = obj;
   return json;
}

CLINGO_API inline OJson* set_json_array_o( OJson json[static 1], OJsonArray* arr )
{
   json->type = o_JsonArray;
   json->array = arr;
   return json;
}

CLINGO_API inline OJson* set_json_bool_o( OJson json[static 1], bool val )
{
   json->type = o_JsonBool;
   json->boolean = val;
   return json;
}

CLINGO_API inline OJson* set_json_null_o( OJson json[static 1] )
{
   json->type = o_JsonNull;
   json->null = NULL;
   return json;
}

/*******************************************************************************

*******************************************************************************/

ODDJSON_API bool record_json_diff_o( cRecorder rec[static 1],
                                     OJson const* json,
                                     OJson const* oth );

ODDJSON_API bool record_json_object_diff_o( cRecorder rec[static 1],
                                            OJsonObject const* obj,
                                            OJsonObject const* oth );

ODDJSON_API bool record_json_array_diff_o( cRecorder rec[static 1],
                                           OJsonArray const* arr,
                                           OJsonArray const* oth );

/*******************************************************************************

*******************************************************************************/

CLINGO_API bool unmarshal_json_o( cScanner sca[static 1],
                                  OJson json[static 1],
                                  cErrorStack es[static 1] );

/*******************************************************************************

*******************************************************************************/

ODDJSON_API char const* stringify_json_type_o( o_JsonType type );

#endif