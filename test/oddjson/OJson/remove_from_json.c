#include "clingo/lang/expect.h"
#include "oddjson/OJson.h"
#include "oddjson/oJsonBuilder.h"
#include "clingo/io/print.h"

TEMP_SLICE_C_(
   test,
   {
      char const* json;
      cCharsSlice route;
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
         "{\"a\":{}}"
      ),
      t_(
         "[0,1,2,{\"4\":{\"x\":[0,{}]}}]",
         cs_c_( "3", "4", "x", "0" ),
         "[0,1,2,{\"4\":{\"x\":[{}]}}]",
      )
   );

   times_c_( tests.s, i )
   {
      test const t = tests.v[i];
      cErrorStack* es = &error_stack_c_( 1024 );
    
      cScanner* sca = &cstr_scanner_c_( t.json );
      OJson* json = let_json_o_();
      require_c_( unmarshal_json_o( sca, json, es ) );

      expect_c_( remove_from_json_o( json, t.route, es ) );

      oJsonBuilder* b = &json_builder_o_( "", "" );
      require_c_( init_json_builder_o( b, 1024 ) );

      add_json_value_o( b, json );
      cChars built = built_json_o( b );
      println_c_( "{cs}", built );
      bool res = chars_is_c( built, t.exp );
      expect_c_( res );

      cleanup_json_builder_o( b );
      release_all_c_( json );
   }

   return finish_tap_c_();
}