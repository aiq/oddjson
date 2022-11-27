#include "clingo/lang/expect.h"
#include "oddjson/oJsonString.h"

TEMP_SLICE_C_(
   test,
   {
      char const* json;
      char const* exp;
      char const* tail;
   }
)
#define t_( ... ) ((test){__VA_ARGS__})

int main( void )
{
   init_tap_c_();

   testSlice tests = slice_c_( test,
      t_(
         "\"\\\"quoted\\\t\\u0022\na newline\", ",
         "\"quoted\t\"\na newline",
         ", "
      ),
      t_(
         "\"\": \"\"",
         "",
         ": \"\""
      )
   );

   for_each_c_( test const*, t, tests )
   {
      cScanner* sca = &cstr_scanner_c_( t->json );
      expect_c_( move_if_json_string_is_o_( sca, t->exp ) );
      expect_c_( unscanned_is_c( sca, t->tail ) );
   }

   return finish_tap_c_();
}