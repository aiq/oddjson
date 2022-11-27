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
         "\"lets check it out \uff6f\": 54",
         "lets check it out ｯ",
         ": 54"
      )
   );

   for_each_c_( test const*, t, tests )
   {
      cScanner* sca = &cstr_scanner_c_( t->json );
      oJsonString jstr;
      expect_c_( scan_json_string_o( sca, &jstr ) );

      cChars exp = c_c( t->exp );
      expect_c_( json_string_is_o( &jstr, exp ) );
      expect_c_( jstr.byteLength == exp.s );
      expect_c_( jstr.length == count_runes_c( exp ) );
   }

   return finish_tap_c_();
}