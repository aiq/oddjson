#include "oddjson/oJsonParser.h"

#include "clingo/io/read_type.h"

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

bool can_be_json_object_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   return on_char_c( &(p->sca), '{' );
}

bool can_be_json_array_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   return on_char_c( &(p->sca), '[' );
}

bool can_be_json_string_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   return on_char_c( &(p->sca), '"' );
}

bool can_be_json_number_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   return on_any_char_c_( &(p->sca), "-0123456789" );
}

bool can_be_json_bool_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   return on_any_char_c_( &(p->sca), "tf" );
}

bool can_be_json_null_o( oJsonParser p[static 1] )
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

   return false;
}

bool skip_json_value_o( oJsonParser p[static 1] )
{
   if ( p->err != cNoError_ ) return false;

   if ( can_be_json_object_o( p ) )
   {
      begin_parse_json_object_o( p );
      while ( in_json_object_o( p ) )
      {
         skip_json_member_o( p );
      }
      return finish_parse_json_object_o( p );
   }
   else if ( can_be_json_array_o( p ) )
   {
      begin_parse_json_array_o( p );
      while ( in_json_array_o( p ) )
      {
         skip_json_value_o( p );
      }
      return finish_parse_json_array_o( p );
   }
   else if ( can_be_json_string_o( p ) )
   {
      oJsonString jstr;
      return view_json_string_o( p, &jstr );
   }
   else if ( can_be_json_number_o( p ) )
   {
      double num;
      return parse_json_number_o( p, &num );
   }
   else if ( can_be_json_bool_o( p ) )
   {
      bool val;
      return parse_json_bool_o( p, &val );
   }
   else if ( can_be_json_null_o( p ) )
   {
      return parse_json_null_o( p );
   }

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

   return false;
}

bool in_json_object_o( oJsonParser p[static 1] )
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

   return false;
}

bool in_json_array_o( oJsonParser p[static 1] )
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

   return false;
}