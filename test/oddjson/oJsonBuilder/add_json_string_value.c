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

   for_each_c_( test const*, t, tests )
   {
      oJsonBuilder* b = &json_builder_o_( "", "" );
      init_json_builder_o( b, 1024 );

      expect_c_( add_json_string_value_o( b, c_c( t->inp ) ) );

      cChars json = built_json_o( b );
      bool res = chars_is_c( json, t->exp );
      tap_desc_c_( res, "expected {s:q}, got {s:q}", t->exp, json.v );

      cleanup_json_builder_o( b );
   }

   return finish_tap_c_();
}