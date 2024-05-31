#include "oddjson/oJsonRoute.h"

#include "clingo/io/write_type.h"
#include "_/util.h"

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

bool record_json_route_key_o( cRecorder rec[static 1], cChars key )
{
   cChars route = recorded_chars_c( rec );
   if ( route.s > 0 )
   {
      char last = last_c_( route );
      if ( last == '"' )
      {
         return record_json_characters( rec, key ) and
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