#include "oddjson/oddjson.h"

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

   for_each_c_( i, test const*, t, tests )
   {
      oJsonBuilder* b = &json_builder_o_( "", "" );
      init_json_builder_o( b, 1024 );

      expect_c_( i, add_json_bool_value_o( b, t->inp ) );

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