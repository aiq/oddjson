#include "clingo/lang/expect.h"
#include "oddjson/OJsonBuilder.h"

TEMP_SLICE_C_(
   test,
   {
      char const* inp;
      char const* exp;
   }
)
#define t_( ... ) ((test){__VA_ARGS__})

int main( void )
{
   init_tap_c_();

   testSlice tests = slice_c_( test,
      t_( "apple", "\"apple\"" )
   );

   for_each_c_( test const*, t, tests )
   {
      OJsonBuilder* b = new_json_builder_o();

      append_json_string_value_o( b, c_c( t->inp ) );
      cChars res = built_json_o( b );

      expect_c_( chars_is_c( res, t->exp ) );

      release_c( b );
   }

   return finish_tap_c_();
}