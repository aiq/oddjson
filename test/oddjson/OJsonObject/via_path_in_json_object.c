#include "clingo/lang/expect.h"
#include "oddjson/OJsonObject.h"

TEMP_SLICE_C_(
   test,
   {
      char const* obj;
      cCharsSlice path;
   }
)
#define t_( ... )

int main( void )
{
   init_tap_c_();

   testSlice tests = slice_c_( test,
      t_(
         "",
         cs_c_( )
      ),
      t_(
         "",
         cs_c_( )
      )
   );

   return finish_tap_c_();
}