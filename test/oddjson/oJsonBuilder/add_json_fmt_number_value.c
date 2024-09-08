#include "oddjson/oddjson.h"

TEMP_SLICE_C_(
   test,
   {
      double inp;
      char const* fmt;
      char const* exp;
   }
)
#define t_( ... ) ((test){__VA_ARGS__})

int main( void )
{
   init_tap_c_();

   testSlice tests = slice_c_( test,
      t_( 10, "", "10" ),
      t_( 2.34, "2", "2.34" )
   );

   for_each_c_( i, test const*, t, tests )
   {
      oJsonBuilder* b = &json_builder_o_( "", "" );
      init_json_builder_o( b, 1024 );

      expect_c_( i, add_json_fmt_number_value_o( b, t->inp, t->fmt ) );
      cChars json = built_json_o( b );
      bool res = chars_is_c( json, t->exp );
      tap_desc_c_( res, "expected {s:q}, got {s:q}", t->exp, json.v );
      expect_block_c_( i, res )
      {
         tap_exp_line_c_( "{s}", t->exp );
         tap_got_line_c_( "{cs}", json );
      }

      cleanup_json_builder_o( b );
   }

   return finish_tap_c_();
}