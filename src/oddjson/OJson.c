#include "oddjson/OJson.h"

#include "_/array_util.h"
#include "_/error.h"
#include "_/util.h"
#include "oddjson/oJsonParser.h"
#include "oddjson/oJsonRoute.h"
#include "clingo/io/cTape.h"
#include "clingo/io/jot.h"

/*******************************************************************************
********************************************************* Types and Definitions
********************************************************************************
 
*******************************************************************************/



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

static char const* route_tail( cRecorder const route[static 1] )
{
   return route->pos == 0 ? "" : " ";
}

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
   char const* tail = route_tail( route );
   return jotln_c_( rec, "~= ", route, tail, "different types: ",
                         "expected ", stringify_json_type_o( exp ),
                         ", got ", stringify_json_type_o( got ) );
}

static bool record_diff_string_note( cRecorder rec[static 1],
                                     cRecorder const route[static 1],
                                     CString const* exp,
                                     CString const* got )
{
   char const* tail = route_tail( route );
   cChars expChars = sc_c( exp );
   cRune expRune;
   char const* expItr = next_rune_c( expChars, NULL, &expRune );
   cChars gotChars = sc_c( got );
   cRune gotRune;
   char const* gotItr = next_rune_c( gotChars, NULL, &gotRune );

   if ( is_empty_c_( expChars ) )
   {
      return jotln_c_( rec, "~= ", route, tail, "different strings: ",
                            "expected an emtpy string" );
   }

   if ( is_empty_c_( gotChars ) )
   {
      return jotln_c_( rec, "~= ", route, tail, "different strings: ",
                            "got an empty string" );
   }

   int64_t count = 0;
   while ( expItr != NULL and gotItr != NULL )
   {
      if ( not eq_rune_c( expRune, gotRune ) )
      {
         jotln_c_( rec, "~= ", route, tail, "different strings: ",
                        "at rune ", count, " expected ", expRune,
                        ", got ", gotRune );
      }

      expItr = next_rune_c( expChars, expItr, &expRune );
      gotItr = next_rune_c( gotChars, gotItr, &gotRune );
      ++count;
   }

   return jotln_c_( rec, "~= ", route, tail, "different strings: ",
                         "different length: ",
                         "expected ", string_length_c( exp ),
                         ", got ", string_length_c( got ) );
}

static bool record_diff_number_note( cRecorder rec[static 1],
                                     cRecorder const route[static 1],
                                     double exp,
                                     double got )
{
   char const* tail = route_tail( route );
   return jotln_c_( rec, "~= ", route, tail, "different numbers: ",
                         "expected ", exp, ", got ", got );
}

static bool record_diff_boolean_note( cRecorder rec[static 1],
                                      cRecorder const route[static 1],
                                      bool exp,
                                      bool got )
{
   char const* tail = route_tail( route );
   return jotln_c_( rec, "~= ", route, tail, "different booleans: ",
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

static inline OJson* get_value( OJson const* json,
                                cChars key,
                                cErrorStack es[static 1] )
{
   OJson* res = NULL;
   if ( json->type == o_JsonObject )
      res = get_via_chars_from_json_object_o( json->object, key );

   if ( json->type == o_JsonArray )
      res = get_via_chars_from_json_array_o( json->array, key, es );
   
   if ( res == NULL )
   {
      char const* typeStr = stringify_json_type_o( json->type );
      push_text_error_c_( es, "not able to get {cs:q} from {s}", key, typeStr );
   }
   return res;
}

static inline OJson* goto_json( OJson* json,
                                cCharsSlice route,
                                int64_t* steps,
                                cErrorStack es[static 1] )
{
   OJson* val = json;
   times_c_( route.s, i )
   {
      *steps = i+1;
      cChars key = route.v[i];
      val = get_value( val, key, es );

      if ( val == NULL )
         break;
   }

   return val;
}

static bool route_tape_func( cRecorder rec[static 1],
                             void const* i,
                             char const fmt[static 1] )
{
   must_exist_c_( i );
   cCharsSlice const* val = i;
   return record_json_route_o( rec, *val );
}
static void* push_route_error( cErrorStack* es,
                               cCharsSlice route,
                               int64_t len,
                               char const txt[static 1] )
{
   route = left_c_( cCharsSlice, route, len );
   cTape routeTape = (cTape){ .i=&route, .f=route_tape_func };
   push_text_error_c_( es, "{s} {t}", txt, routeTape );
   push_json_error( es );
   return NULL;
}

OJson* get_from_json_o( OJson const* json,
                        cCharsSlice route,
                        cErrorStack es[static 1] )
{
   must_exist_c_( json );

   if ( is_empty_c_( route ) )
      return false;

   cCharsSlice childRoute = mid_c_( cCharsSlice, route, 1 );
   cChars key = first_c_( route );
   OJson* val = get_value( json, key, es );
   if ( val == NULL )
      return push_route_error( es, route, 1, "not able to get" );

   int64_t steps = 0;
   OJson* res = goto_json( val, childRoute, &steps, es );
   if ( res == NULL )
      return push_route_error( es, route, steps+1, "not able to get" );

   return res;
}

bool remove_from_json_o( OJson* json,
                         cCharsSlice route,
                         cErrorStack es[static 1] )
{
   must_exist_c_( json );

   if ( is_empty_c_( route ) )
      return false;

   cCharsSlice parentRoute = left_c_( cCharsSlice, route, route.s-1 );
   cChars key = last_c_( route );
   int64_t steps = 0;
   OJson* parent = goto_json( json, parentRoute, &steps, es );
   if ( parent == NULL )
      return push_route_error( es, route, steps, "not able to get" );

   bool res = false;
   if ( parent->type == o_JsonObject )
   {
      res = remove_via_chars_from_json_object_o( parent->object, key );
   }
   else if ( parent->type == o_JsonArray )
   {
      res = remove_via_chars_from_json_array_o( parent->array,
                                                key,
                                                es );
   }

   if ( !res )
   {
      char const* typeStr = stringify_json_type_o( parent->type );
      push_text_error_c_( es, "not able to remove {cs:q} from {s}", key, typeStr );
      push_json_error( es );
   }
   return res;
}

bool set_on_json_o( OJson* json,
                    cCharsSlice route,
                    OJson* val,
                    cErrorStack es[static 1] )
{
   must_exist_c_( json );

   if ( is_empty_c_( route ) )
      return false;

   cCharsSlice parentRoute = left_c_( cCharsSlice, route, route.s-1 );
   cChars key = last_c_( route );
   int64_t steps = 0;
   OJson* parent = goto_json( json, parentRoute, &steps, es );
   if ( parent == NULL )
      return push_route_error( es, route, steps, "not able to get" );

   bool res = false;
   if ( parent->type == o_JsonObject )
   {
      res = set_via_chars_on_json_object_o( parent->object, key, val );
   }
   else if ( parent->type == o_JsonArray )
   {
      res = set_via_chars_on_json_array_o( parent->array, key, val, es );
   }

   if ( !res )
   {
      char const* typeStr = stringify_json_type_o( parent->type );
      push_text_error_c_( es, "not able to set {cs:q} on {s}", key, typeStr );
      push_json_error( es );
   }
   return res;
}

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
