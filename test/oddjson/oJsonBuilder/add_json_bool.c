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

   for_each_c_( test const*, t, tests )
   {
      oJsonBuilder* b = &json_builder_o_( t->prefix, t->indent );
      init_json_builder_o( b, 1024 );

      expect_c_( begin_json_object_value_o( b ) );
      expect_c_( add_json_bool_o_( b, t->name, t->value ) );
      expect_c_( finish_json_object_o( b ) );

      cChars json = built_json_o( b );
      bool res = chars_is_c( json, t->exp );
      tap_descf_c( res, "expected '%s', got '%s'", t->exp, json.v );

      cleanup_json_builder_o( b );

   }

   return finish_tap_c_();
}