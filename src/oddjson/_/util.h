#ifndef ODDJSON_INTERN_UTIL_H
#define ODDJSON_INTERN_UTIL_H

#include "clingo/io/write_type.h"

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

static inline bool record_escaped_char( cRecorder rec[static 1], char c )
{
   switch ( c )
   {
      case '\"': return write_cstr_c_( rec, "\\\"" ); // double quotation mark
      case '\\': return write_cstr_c_( rec, "\\\\" ); // backslash
      case '\b': return write_cstr_c_( rec, "\\b" );  // backspace
      case '\f': return write_cstr_c_( rec, "\\f" );  // formfeef
      case '\n': return write_cstr_c_( rec, "\\n" );  // newline
      case '\r': return write_cstr_c_( rec, "\\r" );  // carriage return
      case '\t': return write_cstr_c_( rec, "\\t" );  // horzontal tab
      default: return write_char_c_( rec, c );
   }
}

static inline bool record_json_characters( cRecorder rec[static 1],
                                           cChars value )
{
   cRune r;
   iterate_runes_c_( itr, &r, value )
   {
      if ( rune_size_c( r ) > 1 )
      {
         if ( not write_rune_c_( rec, r ) )
            return false;
      }
      else
      {
         if ( not record_escaped_char( rec, r.c[0] ) )
            return false;
      }  
   }

   return true;
}

static inline bool record_quoted_json_string( cRecorder rec[static 1],  
                                              cChars value )
{
   return write_char_c_( rec, '"' ) and
          record_json_characters( rec, value ) and
          write_char_c_( rec, '"' );
}

#endif