#include "clingo/lang/expect.h"
#include "oddjson/OJsonBuilder.h"

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

   for_each_c_( test const*, t, tests )
   {
      OJsonBuilder* b = new_json_builder_o();

      expect_c_( append_json_fmt_number_value_o( b, t->inp, t->fmt ) );
      cChars json = built_json_o( b );
      bool res = chars_is_c( json, t->exp );
      tap_descf_c( res, "expected '%s', got '%s'", t->exp, json.v );

      release_c( b );
   }

   return finish_tap_c_();
}