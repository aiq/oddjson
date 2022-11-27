#include "clingo/lang/expect.h"
#include "clingo/io/print.h"
#include "oddjson/OJsonBuilder.h"

TEMP_SLICE_C_(
   test,
   {
      char const* prefix;
      char const* indent;
      char const* name;
      bool value;
      char const* exp;
   }
)
#define t_( ... ) ((test){__VA_ARGS__})

int main( void )
{
   init_tap_c_();

   testSlice tests = slice_c_( test,
      t_( "", "", "enabled", true, "{\"enabled\":true}" ),
      t_( "", "  ", "enabled", false, "{\n  \"enabled\":false\n}" )
   );

   for_each_c_( test const*, t, tests )
   {
      OJsonBuilder* b = make_json_builder_o_( t->prefix, t->indent );

      begin_json_object_value_o( b );
      append_json_bool_o_( b, t->name, t->value );
      finish_json_object_o( b );

      cChars res = built_json_o( b );
      println_c_( "res: {cs}", res );
      expect_c_( chars_is_c( res, t->exp ) );

      release_c( b );
   }

   return finish_tap_c_();
}