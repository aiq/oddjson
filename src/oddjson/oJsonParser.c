#include "oddjson/oJsonParser.h"

#include "clingo/io/read_type.h"
#include "_/error.h"

/*******************************************************************************

*******************************************************************************/

static inline void skip_ws( cScanner sca[static 1] )
{
   move_while_any_char_c_( sca, " \r\n\t" );
}

static inline void skip_comma( cScanner sca[static 1] )
{
   skip_ws( sca );
   if ( move_if_char_c( sca, ',' ) )
   {
      skip_ws( sca );
   }
}

static inline void skip_colon( cScanner sca[static 1] )
{
   skip_ws( sca );
   if ( move_if_char_c( sca, ':' ) )
   {
      skip_ws( sca );
   }
}

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

bool init_json_parser_o( oJsonParser p[static 1], cChars full )
{
   p->sca = make_scanner_c_( full.s, full.v );
   p->err = cNoError_;
   skip_ws( &(p->sca) );
   return true;
}

/*******************************************************************************

*******************************************************************************/

bool push_json_parser_error_o( cErrorStack es[static 1],
                               oJsonParser const p[static 1] )
{
   if ( p->err != NULL )
   {
      return push_lit_error_c( es, p->err ) ||
             push_json_lit_error_o( es, "oJsonParser" );
   }

   return false;
}

/*******************************************************************************

*******************************************************************************/

bool json_name_is_o( oJsonParser p[static 1], cChars exp )
{
   if ( p->err != cNoError_ ) return false;

   bool res = move_if_json_string_is_o( &(p->sca), exp );
   skip_ws( &(p->sca) );
   skip_colon( &(p->sca) );

   return res;
}

/*******************************************************************************

*******************************************************************************/

bool maybe_json_object_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   return on_char_c( &(p->sca), '{' );
}

bool maybe_json_array_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   return on_char_c( &(p->sca), '[' );
}

bool maybe_json_string_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   return on_char_c( &(p->sca), '"' );
}

bool maybe_json_number_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   return on_any_char_c_( &(p->sca), "-0123456789" );
}

bool maybe_json_bool_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   return on_any_char_c_( &(p->sca), "tf" );
}

bool maybe_json_null_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   return on_char_c( &(p->sca), 'n' );
}

/*******************************************************************************

*******************************************************************************/

bool skip_json_member_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   oJsonString jstr;
   if ( scan_json_string_o( &(p->sca), &jstr ) )
   {
      skip_colon( &(p->sca) );
      return skip_json_value_o( p );
   }

   p->err = "not able to skip member";
   return false;
}

bool skip_json_value_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   if ( maybe_json_object_o( p ) )
   {
      begin_parse_json_object_o_( p )
      {
         skip_json_member_o( p );
      }
      return finish_parse_json_object_o( p );
   }
   else if ( maybe_json_array_o( p ) )
   {
      begin_parse_json_array_o_( p )
      {
         skip_json_value_o( p );
      }
      return finish_parse_json_array_o( p );
   }
   else if ( maybe_json_string_o( p ) )
   {
      oJsonString jstr;
      return view_json_string_o( p, &jstr );
   }
   else if ( maybe_json_number_o( p ) )
   {
      double num;
      return parse_json_number_o( p, &num );
   }
   else if ( maybe_json_bool_o( p ) )
   {
      bool val;
      return parse_json_bool_o( p, &val );
   }
   else if ( maybe_json_null_o( p ) )
   {
      return parse_json_null_o( p );
   }

   p->err = "not able to skip value";
   return false;
}

/*******************************************************************************

*******************************************************************************/

bool begin_parse_json_object_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   if ( move_if_char_c( &(p->sca), '{' ) )
   {
      skip_ws( &(p->sca) );
      return true;
   }

   p->err = "not able to begin object";
   return false;
}

bool parser_in_json_object_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   return not on_char_c( &(p->sca), '}' );
}

bool finish_parse_json_object_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   if ( move_if_char_c( &(p->sca), '}' ) )
   {
      skip_comma( &(p->sca) );
      return true;
   }

   p->err = "not able to finish object";
   return false;
}

/*******************************************************************************

*******************************************************************************/

bool begin_parse_json_array_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   if ( move_if_char_c( &(p->sca), '[' ) )
   {
      skip_ws( &(p->sca) );
      return true;
   }

   p->err = "not able to begin array";
   return false;
}

bool parser_in_json_array_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   return not on_char_c( &(p->sca), ']' );
}

bool finish_parse_json_array_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   if ( move_if_char_c( &(p->sca), ']' ) )
   {
      skip_comma( &(p->sca) );
      return true;
   }

   p->err = "not able to finish array";
   return false;
}

/*******************************************************************************

*******************************************************************************/

bool view_json_string_o( oJsonParser p[static 1], oJsonString jstr[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   if ( scan_json_string_o( &(p->sca), jstr ) )
   {
      skip_comma( &(p->sca) );
      return true;
   }

   p->err = "not able to view string";
   return false;
}

bool parse_json_number_o( oJsonParser p[static 1], double value[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   if ( read_double_c( &(p->sca), value, "" ) )
   {
      skip_comma( &(p->sca) );
      return true;
   }

   p->err = "not able to parse number";
   return false;
}

bool parse_json_int_number_o( oJsonParser p[static 1], int64_t value[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   if ( read_int64_c( &(p->sca), value, "" ) )
   {
      skip_comma( &(p->sca) );
      return true;
   }

   p->err = "not able to parse int number";
   return false;
}

bool parse_json_bool_o( oJsonParser p[static 1], bool value[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   if ( read_bool_c( &(p->sca), value, "l" ) )
   {
      skip_comma( &(p->sca) );
      return true;
   }

   p->err = "not able to parse bool";
   return false;
}

bool parse_json_null_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   if ( move_if_chars_c_( &(p->sca), "null" ) )
   {
      skip_comma( &(p->sca) );
      return true;
   }

   p->err = "not able to parse null";
   return false;
}

bool parse_json_object_o( oJsonParser p[static 1], OJsonObject* obj )
{
   must_exist_c_( obj );
   if ( p->err != cNoError_ ) return false;

   begin_parse_json_object_o_( p )
   {
      oJsonString name;
      if ( not scan_json_string_o( &(p->sca), &name ) )
      {
         return false;
      }
      CString* key = decode_json_string_o( &name );
      if ( key == NULL )
      {
         return false;
      }
      skip_colon( &(p->sca) );
      OJson* val = new_json_o_();
      if ( val == NULL )
      {
         release_c( key );
         return false;
      }
      if ( not parse_json_o( p, val ) )
      {
         release_all_c_( key, val );
         return false;
      }
      if ( not set_on_json_object_o( obj, key, val ) )
      {
         release_all_c_( key, val );
         return false;
      }
   }
   return finish_parse_json_object_o( p );
}

bool parse_json_array_o( oJsonParser p[static 1], OJsonArray* arr )
{
   must_exist_c_( arr );
   if ( p->err != cNoError_ ) return false;

   begin_parse_json_array_o_( p )
   {
      OJson* entry = new_json_o_();
      if ( entry == NULL )
      {
         // TODO MEM
         return false;
      }
      if ( not add_to_json_array_o( arr, entry ) )
      {
         release_c( entry );
         // TODO MEM
         return false;
      }
   }
   return finish_parse_json_array_o( p );
}

bool parse_json_o( oJsonParser p[static 1], OJson json[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   if ( maybe_json_string_o( p ) ) {
      oJsonString jstr;
      if ( not view_json_string_o( p, &jstr ) )
      {
         return false;
      }
      CString* str = decode_json_string_o( &jstr );
      if ( str == NULL )
      {
         // TODO mem error
         return false;
      }
      *json = json_with_string_o_( str );
      return true;
   }
   else if ( maybe_json_number_o( p ) ) {
      double num;
      if ( not parse_json_number_o( p, &num ) )
      {
         return false;
      }
      *json = json_with_number_o_( num );
   }
   else if ( maybe_json_object_o( p ) ) {
      OJsonObject* obj = new_json_object_o();
      if ( obj == NULL )
      {
         return false;
      }
      if ( not parse_json_object_o( p, obj ) )
      {
         release_c( obj );
         return false;
      }
      *json = json_with_object_o_( obj );
   }
   else if ( maybe_json_array_o( p ) ) {
      OJsonArray* arr = new_json_array_o();
      if ( arr == NULL )
      {
         // TODO mem error
         return false;
      }
      if ( not parse_json_array_o( p, arr ) )
      {
         release_c( arr );
         return false;
      }
      *json = json_with_array_o_( arr );
   }
   else if ( maybe_json_bool_o( p ) ) {
      bool b;
      if ( not parse_json_bool_o( p, &b ) )
      {
         return false;
      }
      *json = json_with_bool_o_( b );
   }
   else if ( maybe_json_null_o( p ) ) {
      if ( not parse_json_null_o( p ) )
      {
         return false;
      }
      *json = json_with_null_o_();
   }

   p->err = "unknown type";
   return false;
}