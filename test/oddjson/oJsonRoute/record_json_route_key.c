#include "oddjson/oddjson.h"

TEMP_SLICE_C_(
   test,
   {
      cCharsSlice keys;
      cChars exp;
   }
)
#define t_( ... ) ((test){__VA_ARGS__})

int main( void )
{
   init_tap_c_();

   testSlice tests = slice_c_( test,
      t_( cs_c_( "a", "b", "c" ), c_c( "\"a'b'c\"" ) ),
      t_( cs_c_( "a 2", "b.1", "c\"d", "e'f" ), c_c( "\"a 2'b.1'c\\\"d'e^'f\"" ) ),
      t_( cs_c_( "store", "bookRelease", "0", "author"), c_c( "\"store'bookRelease'0'author\"" ) )
   );

   for_each_c_( i, test const*, t, tests )
   {
      cRecorder* rec = &dyn_recorder_c_( 0 );

      each_c_( cChars const*, key, t->keys )
      {
         record_json_route_key_o( rec, *key );
      }

      cChars route = recorded_chars_c( rec );
      bool res = eq_chars_c( route, t->exp );
      expect_block_c_( i, res )
      {
         tap_exp_line_c_( "{s}", t->exp.v );
         tap_got_line_c_( "{cs}", route );
      }

      free_recorder_mem_c( rec );
   }

   return finish_tap_c_();
}