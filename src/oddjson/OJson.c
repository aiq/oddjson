#include "oddjson/OJson.h"

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

OBJ_VEC_IMPL_C_(
   ,//optional      // Static
   OJsonArray,      // VecType
   OJson,           // ObjType
   json_array_o,    // FuncName
   O_JsonArrayMeta  // Meta
)

static inline void cleanup( void* instance )
{
   OJson* json = instance;
   if ( json->type == o_JsonObject or
        json->type == o_JsonArray or
        json->type == o_JsonString ) {
      release_c( json->object );
   }
}

cMeta const O_JsonMeta = {
   .desc = stringify_c_( OJson ),
   .cleanup = &cleanup
};

/*******************************************************************************

*******************************************************************************/

extern inline OJson* set_json_string_o( OJson json[static 1], CString* str );

extern inline OJson* set_json_number_o( OJson json[static 1], double number );

extern inline OJson* set_json_object_o( OJson json[static 1], OJsonObject* obj );

extern inline OJson* set_json_array_o( OJson json[static 1], OJsonArray* arr );

extern inline OJson* set_json_bool_o( OJson json[static 1], bool val );

extern inline OJson* set_json_null_o( OJson json[static 1] );

/******************************************************************************/

static bool record_missing_note( cRecorder rec[static 1],
                                 cRecorder const route[static 1],
                                 OJson const* val )
{
   char const* type = stringify_json_type_o( val->type );
   return jotln_c_( rec, "-- ", route, " missing ", type );
}

static bool record_unexpected_note( cRecorder rec[static 1],
                                    cRecorder const route[static 1],
                                    OJson const* val )
{
   char const* type = stringify_json_type_o( val->type );
   return jotln_c_( rec, "++ ", route, " unexpected ", type );
}

static bool record_diff_type_note( cRecorder rec[static 1],
                                   cRecorder const route[static 1],
                                   o_JsonType exp,
                                   o_JsonType got )
{
   char const* space = route->pos == 0 ? "" : " ";
   return jotln_c_( rec, "~= ", route, space, "different types: ",
                         "expected ", stringify_json_type_o( exp ),
                         ", got ", stringify_json_type_o( got ) );
}

static bool record_diff_string_note( cRecorder rec[static 1],
                                     cRecorder const route[static 1],
                                     CString const* exp,
                                     CString const* got )
{
   char const* space = route->pos == 0 ? "" : " ";
   cChars expChars = sc_c( exp );
   cRune expRune;
   char const* expItr = next_rune_c( expChars, NULL, &expRune );
   cChars gotChars = sc_c( got );
   cRune gotRune;
   char const* gotItr = next_rune_c( gotChars, NULL, &gotRune );

   if ( is_empty_c_( expChars ) )
   {
      return jotln_c_( rec, "~= ", route, space, "different strings: ",
                            "expected an emtpy string" );
   }

   if ( is_empty_c_( gotChars ) )
   {
      return jotln_c_( rec, "~= ", route, space, "different strings: ",
                            "got an empty string" );
   }

   int64_t count = 0;
   while ( expItr != NULL and gotItr != NULL )
   {
      if ( not eq_rune_c( expRune, gotRune ) )
      {
         jotln_c_( rec, "~= ", route, space, "different strings: ",
                        "at rune ", count, " expected ", expRune,
                        ", got ", gotRune );
      }

      expItr = next_rune_c( expChars, expItr, &expRune );
      gotItr = next_rune_c( gotChars, gotItr, &gotRune );
      ++count;
   }

   return jotln_c_( rec, "~= ", route, space, "different strings: ",
                         "different length: ",
                         "expected ", string_length_c( exp ),
                         ", got ", string_length_c( got ) );
}

static bool record_diff_number_note( cRecorder rec[static 1],
                                     cRecorder const route[static 1],
                                     double exp,
                                     double got )
{
   char const* space = route->pos == 0 ? "" : " ";
   return jotln_c_( rec, "~= ", route, space, "different numbers: ",
                         "expected ", exp, ", got ", got );
}

static bool record_diff_boolean_note( cRecorder rec[static 1],
                                      cRecorder const route[static 1],
                                      bool exp,
                                      bool got )
{
   char const* space = route->pos == 0 ? "" : " ";
   return jotln_c_( rec, "~= ", route, space, "different booleans: ",
                         "expected ", exp, ", got ", got );
}

/******************************************************************************/

static bool intl_record_json_diff( cRecorder rec[static 1],
                                   cRecorder route[static 1],
                                   OJson const* json,
                                   OJson const* oth );

static bool intl_record_json_object_diff( cRecorder rec[static 1],
                                          cRecorder route[static 1],
                                          OJsonObject const* obj,
                                          OJsonObject const* oth );

static bool intl_record_json_array_diff( cRecorder rec[static 1],
                                         cRecorder route[static 1],
                                         OJsonArray const* arr,
                                         OJsonArray const* oth );

/******************************************************************************/

static bool intl_record_json_diff( cRecorder rec[static 1],
                                   cRecorder route[static 1],
                                   OJson const* json,
                                   OJson const* oth )
{
   if ( json->type != oth->type )
   {
      return record_diff_type_note( rec, route, json->type, oth->type );
   }
   else if ( json->type == o_JsonObject )
   {
      return intl_record_json_object_diff( rec, route, json->object, oth->object );
   }
   else if ( json->type == o_JsonArray )
   {
      return intl_record_json_array_diff( rec, route, json->array, oth->array );
   }
   else if ( json->type == o_JsonString )
   {
      if ( not eq_string_c( json->string, oth->string ) )
      {
         return record_diff_string_note( rec, route, json->string, oth->string );
      }
   }
   else if ( json->type == o_JsonNumber )
   {
      if ( json->number != oth->number )
      {
         return record_diff_number_note( rec, route, json->number, oth->number );
      }
   }
   else if ( json->type == o_JsonBool )
   {
      if ( json->boolean != oth->boolean )
      {
         return record_diff_boolean_note( rec, route, json->boolean, oth->boolean);
      }
   }

   // nothing to do for o_JsonNull
   return true;
}

bool record_json_diff_o( cRecorder rec[static 1],
                         OJson const* json,
                         OJson const* oth )
{
   must_be_c_( json );
   must_be_c_( oth );

   cRecorder* route = &dyn_recorder_c_( 0 );
   bool res = intl_record_json_diff( rec, route, json, oth );
   free_recorder_mem_c( route );
   return res;
}

/******************************************************************************/

static bool intl_record_json_object_diff( cRecorder rec[static 1],
                                          cRecorder route[static 1],
                                          OJsonObject const* obj,
                                          OJsonObject const* oth )
{
   CString const* key;
   OJson const* val;
   int64_t oldPos = route->pos;
   iterate_map_c_( itr, &key, &val, obj, next_in_json_object_o )
   {
      if ( not record_json_route_key_o( route, sc_c( key ) ) )
         return false;

      OJson const* othVal = get_from_json_object_o( oth, key );
      if ( othVal == NULL )
      {
         if ( not record_missing_note( rec, route, val ) )
            return false;
      }
      else {
         if ( not intl_record_json_diff( rec, route, val, othVal ) )
            return false;
      }

      move_recorder_to_c( route, oldPos );
   }

   iterate_map_c_( itr, &key, &val, oth, next_in_json_object_o )
   {
      if ( not record_json_route_key_o( route, sc_c( key ) ) )
         return false;

      if ( not in_json_object_o( obj, key ) )
      {
         if ( not record_unexpected_note( rec, route, val ) )
            return false;
      }

      move_recorder_to_c( route, oldPos );
   }

   return true;
}

bool record_json_object_diff_o( cRecorder rec[static 1],
                                OJsonObject const* obj,
                                OJsonObject const* oth )
{
   must_be_c_( obj );
   must_be_c_( oth );

   cRecorder* route = &dyn_recorder_c_( 0 );
   bool res = intl_record_json_object_diff( rec, route, obj, oth );
   free_recorder_mem_c( route );
   return res;
}

/******************************************************************************/

static bool record_diff_size_note( cRecorder rec[static 1],
                                   cRecorder const route[static 1],
                                   int64_t exp,
                                   int64_t got )
{
   cRecorder* expRec = &recorder_c_( 256 );
   write_int64_c_( expRec, exp );

   cRecorder* gotRec = &recorder_c_( 256 );
   write_int64_c_( gotRec, got );

   return jotln_c_( rec, "~= ", route, " different number of elements: ",
                         "expected ", exp, ", got ", got );
}

static bool intl_record_json_array_diff( cRecorder rec[static 1],
                                         cRecorder route[static 1],
                                         OJsonArray const* arr,
                                         OJsonArray const* oth )
{
   int64_t const arrCount = info_of_json_array_o( arr )->count;
   int64_t const othCount = info_of_json_array_o( oth )->count;
   if ( arrCount != othCount )
   {
      return record_diff_size_note(rec, route, arrCount, othCount );
   }

   int64_t const oldPos = route->pos;
   cVecInfo const* info = info_of_json_array_o( arr );
   for ( int64_t i = 0; i < info->count; ++i )
   {
      if ( not record_json_route_index_o( route, i ) )
         return false;

      OJson const* arrEntry = get_from_json_array_o( arr, i );
      OJson const* othEntry = get_from_json_array_o( oth, i );
      if ( not intl_record_json_diff( rec, route, arrEntry, othEntry ) )
         return false;

      move_recorder_to_c( route, oldPos );
   }

   return true;
}

bool record_json_array_diff_o( cRecorder rec[static 1],
                               OJsonArray const* arr,
                               OJsonArray const* oth )
{
   must_exist_c_( arr );
   must_exist_c_( oth );

   cRecorder* route = &dyn_recorder_c_( 0 );
   bool res = intl_record_json_array_diff( rec, route, arr, oth );
   free_recorder_mem_c( route );
   return res;
}

/*******************************************************************************

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

bool unmarshal_json_o( cScanner sca[static 1],
                       OJson json[static 1],
                       cErrorStack es[static 1] )
{
   oJsonParser p;
   if ( not init_json_parser_shell_o( &p, sca ) )
      return push_json_parser_error_o( es, &p );

   if ( not parse_json_o( &p, json ) )
      return push_json_parser_error_o( es, &p );

   return true;
}

/*******************************************************************************

*******************************************************************************/

char const* stringify_json_type_o( o_JsonType type )
{
   switch ( type )
   {
      #define XMAP_C_( N, I ) case N: return #N;
         oJSON_TYPE_
      #undef XMAP_C_
   }
}
