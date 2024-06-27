#include "clingo/lang/expect.h"
#include "oddjson/oJsonBuilder.h"

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
      t_( "", "  ", "enabled", false, "{\n  \"enabled\": false\n}" )
   );

   for_each_c_( i, test const*, t, tests )
   {
      oJsonBuilder* b = &json_builder_o_( t->prefix, t->indent );
      init_json_builder_o( b, 1024 );

      expect_c_( i, begin_json_object_value_o( b ) );
      expect_c_( i, add_json_bool_o_( b, t->name, t->value ) );
      expect_c_( i, finish_json_object_o( b ) );

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