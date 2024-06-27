#include "clingo/lang/expect.h"
#include "oddjson/oJsonBuilder.h"

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

   for_each_c_( i, test const*, t, tests )
   {
      oJsonBuilder* b = &json_builder_o_( "", "" );
      init_json_builder_o( b, 1024 );

      expect_c_( i, add_json_string_value_o( b, c_c( t->inp ) ) );

      cChars json = built_json_o( b );
      bool res = chars_is_c( json, t->exp );
      expect_block_c_( i, res )
      {
         tap_exp_line_c_( "{s}", t->exp );
         tap_got_line_c_( "{cs}", json );
      }

      cleanup_json_builder_o( b );
   }

   return finish_tap_c_();
}