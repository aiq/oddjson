#include "oddjson/o_JsonType.h"

/*******************************************************************************

*******************************************************************************/

char const* stringify_ebml_type_c( o_JsonType type )
{
   switch ( type )
   {
      #define XMAP_C_( N, I ) case N: return #N;
         oJSON_TYPE_
      #undef XMAP_C_
   }
}
