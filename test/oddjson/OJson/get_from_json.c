#include "oddjson/oddjson.h"

TEMP_SLICE_C_(
   test,
   {
      char const* json;
      cCharsSlice route;
      char const* val;
   }
)
#define t_( ... ) ((test){__VA_ARGS__})

int main( void )
{
   init_tap_c_();

   testSlice tests = slice_c_( test,
      t_(
         "{\"x\":1,\"y\":2,\"z\":3}",
         cs_c_( "z" ),
         "3"
      )
   );

   times_c_( tests.s, i )
   {
      test const t = tests.v[i];
      cErrorStack* es = &error_stack_c_( 1024 );

      cScanner* sca = &cstr_scanner_c_( t.json );
      OJson* json = let_json_o_();
      require_c_( unmarshal_json_o( sca, json, es ) );

      OJson* val = get_from_json_o( json, t.route, es );

      oJsonBuilder* b = &json_builder_o_( "", "" );
      require_c_( init_json_builder_o( b, 1024 ) );

      add_json_value_o( b, val );
      cChars built = built_json_o( b );
      bool res = chars_is_c( built, t.val );
      expect_c_( i, res );

      cleanup_json_builder_o( b );
      release_all_c_( json );
   }

   return finish_tap_c_();
}