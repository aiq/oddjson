#include "oddjson/oddjson.h"

TEMP_SLICE_C_(
   test,
   {
      char const* json;
      char const* oth;
      char const* exp;
   }
)
#define t_( ... ) ((test){__VA_ARGS__})

int main( void )
{
   init_tap_c_();

   testSlice tests = slice_c_( test,
      t_(
        "true",
        "true",
         ""
      ),
      t_(
         "true",
         "false",
         "~= different booleans: expected true, got false\n"
      ),
      t_(
         "true",
         "100",
         "~= different types: expected o_JsonBool, got o_JsonNumber\n"
      ),
      t_(
         "1",
         "1",
         ""
      ),
      t_(
         "1.2",
         "12",
         "~= different numbers: expected 1.2, got 12\n"
      ),
      t_(
         "{\"a\":{\"b\":[1,2,3]}}",
         "{\"a\":{\"b\":[]}}",
         "~= \"a'b\" different number of elements: expected 3, got 0\n"
      ),
      t_(
         "{\"a\":{\"b\":{\"c\":[1,2,3]}}}",
         "{\"a\":{\"b\":{\"c\":[1,3]}}}",
         "~= \"a'b'c\" different number of elements: expected 3, got 2\n"
      ),
      t_(
         "{\"a\":{\"b\":{\"c\":[1,2,3]}}}",
         "{\"a\":{\"b\":{\"c\":[1,6,2]}}}",
         "~= \"a'b'c'1\" different numbers: expected 2, got 6\n"
         "~= \"a'b'c'2\" different numbers: expected 3, got 2\n"
      ),
      t_(
         "{\"a\":{\"b\":{\"x\":[]}}}",
         "{\"a\":{\"b\":{\"z\":{}}}}",
         "-- \"a'b'x\" missing o_JsonArray\n"
         "++ \"a'b'z\" unexpected o_JsonObject\n"
      )
   );

   times_c_( tests.s, i )
   {
      test t = tests.v[i];

      cScanner* sca;
      cErrorStack* es = &error_stack_c_( 1024 );

      OJson* json = let_json_o_();
      sca = &cstr_scanner_c_( t.json );
      unmarshal_json_o( sca, json, es );

      OJson* oth = let_json_o_();
      sca = &cstr_scanner_c_( t.oth );
      unmarshal_json_o( sca, oth, es );

      cRecorder* rec = &recorder_c_( 2048 );
      require_c_( record_json_diff_o( rec, json, oth ) );
      tap_desc_c_( recorded_is_c( rec, t.exp ), "@ {i64}", i );

      release_all_c_( json, oth );
   }

   return finish_tap_c_();
}