#include "clingo/lang/expect.h"
#include "oddjson/OJsonBuilder.h"

TEMP_SLICE_C_(
   test,
   {
      bool inp;
      char const* exp;
   }
)
#define t_( ... ) ((test){__VA_ARGS__})

int main( void )
{
   init_tap_c_();

   testSlice tests = slice_c_( test,
      t_( true, "true" ),
      t_( false, "false" )
   );

   for_each_c_( test const*, t, tests )
   {
      OJsonBuilder* b = new_json_builder_o();

      expect_c_( append_json_bool_value_o( b, t->inp ) );

      cChars res = built_json_o( b );

      release_c( b );
   }

   return finish_tap_c_();
}