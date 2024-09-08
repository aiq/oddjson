#include "oddjson/oddjson.h"

TEMP_SLICE_C_(
   test,
   {
      char const* json;
      cCharsSlice route;
      char const* val;
      char const* exp;
   }
)
#define t_( ... ) ((test){__VA_ARGS__})

int main( void )
{
   init_tap_c_();

   testSlice tests = slice_c_( test,
      t_(
         "{\"a\":{\"b\":1}}",
         cs_c_( "a", "b" ),
         "\"foo\"",
         "{\"a\":{\"b\":\"foo\"}}"
      ),
      t_(
         "[0,1,2,{\"4\":{\"x\":[0,1]}}]",
         cs_c_( "3", "4", "x", "1" ),
         "null",
         "[0,1,2,{\"4\":{\"x\":[0,null]}}]"
      )
   );

   times_c_( tests.s, i )
   {
      test const t = tests.v[i];
      cErrorStack* es = &error_stack_c_( 1024 );

      cScanner* sca = &cstr_scanner_c_( t.json );
      OJson* json = let_json_o_();
      require_c_( unmarshal_json_o( sca, json, es ) );

      sca = &cstr_scanner_c_( t.val );
      OJson* val = let_json_o_();
      require_c_( unmarshal_json_o( sca, val, es ) );

      require_c_( set_on_json_o( json , t.route, val, es ) );

      oJsonBuilder* b = &json_builder_o_( "", "" );
      require_c_( init_json_builder_o( b, 2024 ) );

      add_json_value_o( b , json );
      cChars built = built_json_o( b );
      bool res = chars_is_c( built, t.exp );
      expect_c_( i, res );

      cleanup_json_builder_o( b );
      release_all_c_( json, val );
   }

   return finish_tap_c_();
}