#include "oddjson/oJsonRoute.h"

#include "clingo/io/write_type.h"
#include "_/util.h"

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

static inline bool record_escaped_route_char( cRecorder rec[static 1], char c )
{
   switch ( c )
   {
      case '\'': return write_cstr_c_( rec, "^'" );
      case '^':  return write_cstr_c_( rec, "^^" );
      default: return record_escaped_char( rec, c );
   }
}

static inline bool record_json_route_characters( cRecorder rec[static 1],
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
         if ( not record_escaped_route_char( rec, r.c[0] ) )
            return false;
      }  
   }

   return true;
}

/*******************************************************************************

*******************************************************************************/

bool record_json_route_key_o( cRecorder rec[static 1], cChars key )
{
   cChars route = recorded_chars_c( rec );
   if ( route.s > 0 )
   {
      char last = last_c_( route );
      if ( last == '"' or last == '\'' )
      {
         move_recorder_c( rec, -1 );

         return record_char_c( rec, '\'' ) and
                record_json_route_characters( rec, key ) and
                record_char_c( rec, '"' );
      }
   }

   return record_quoted_json_string( rec, key );
}

bool record_json_route_index_o( cRecorder rec[static 1], int64_t index )
{
   cRecorder* buf = &recorder_c_( 256 );
   write_int64_c_( buf, index );

   return record_json_route_key_o( rec, recorded_chars_c( buf ) );
}

bool record_json_route_o( cRecorder rec[static 1], cCharsSlice route )
{
   each_c_( cChars const*, key, route )
   {
      if ( not record_json_route_key_o( rec, *key ) )
         return false;
   }

   return true;
}