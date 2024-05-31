#include "clingo/lang/expect.h"
#include "oddjson/oJsonRoute.h"

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
      t_( cs_c_( "a", "b", "c" ), c_c( "\"a\"b\"c\"" ) ),
      t_( cs_c_( "a 2", "b.1", "c\"d" ), c_c( "\"a 2\"b.1\"c\\\"d\"" ) )
   );

   for_each_c_( test const*, t, tests )
   {
      cRecorder* rec = &dyn_recorder_c_( 0 );

      for_each_c_( cChars const*, key, t->keys )
      {
         record_json_route_key_o( rec, *key );
      }

      cChars route = recorded_chars_c( rec );
      bool res = eq_chars_c( route, t->exp );
      tap_descf_c( res, "expected %s / got %s", t->exp.v, route.v );

      free_recorder_mem_c( rec );
   }
   cRecorder* rec = &dyn_recorder_c_( 0 );

   cCharsSlice keys = cs_c_( "a", "b", "c" );

   for_each_c_( cChars const*, key, keys )
   {
      record_json_route_key_o( rec, *key );
   }

   return finish_tap_c_();
}