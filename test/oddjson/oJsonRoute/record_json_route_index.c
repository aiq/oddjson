#include "oddjson/oddjson.h"

#define expect_( Rec, Cstr )                                                   \
(                                                                              \
    expect_at_c_( recorded_is_c( ( Rec ), (Cstr) ) )                           \
)

int main( void )
{
   init_tap_c_();

   cRecorder* rec = &dyn_recorder_c_( 0 );

   require_c_( record_json_route_index_o( rec, 0 ) );
   expect_( rec, "\"0\"" );

   cCharsSlice keys = cs_c_( "a 2", "b.1", "c\"d" );
   each_c_( cChars const*, key, keys )
   {
      require_c_( record_json_route_key_o( rec, *key ) );
   }
   expect_( rec, "\"0'a 2'b.1'c\\\"d\"" );

   require_c_( record_json_route_index_o( rec, 23 ) );
   expect_( rec, "\"0'a 2'b.1'c\\\"d'23\"" );

   return finish_tap_c_();
}