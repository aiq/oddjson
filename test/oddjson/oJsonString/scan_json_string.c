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
         "\"lets check it out \\uff6f!\": 54",
         "lets check it out ｯ!",
         ": 54"
      ),
      t_(
         "\"𝄞 is the right tune for \\ud834\\udd1e!\",",
         "𝄞 is the right tune for 𝄞!",
         ","
      )
   );

   for_each_c_( i, test const*, t, tests )
   {
      cScanner* sca = &cstr_scanner_c_( t->json );
      oJsonString jstr;
      expect_c_( i, scan_json_string_o( sca, &jstr ) );

      cChars exp = c_c( t->exp );
      expect_c_( i, json_string_is_o( &jstr, exp ) );
      expect_c_( i, jstr.byteLength == exp.s );
      expect_c_( i, jstr.length == count_runes_c( exp ) );

      cVarChars buf = var_chars_c_( 128 );
      cChars decoded = decode_json_string_chars_o( &jstr, buf );
      expect_c_( i, eq_chars_c( exp, decoded ) );
   }

   return finish_tap_c_();
}