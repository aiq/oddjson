#include "oddjson/OJsonArray.h"

#include "clingo/io/read_type.h"

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

static inline int64_t get_json_array_pos( OJsonArray const* arr,
                                          cChars key,
                                          cErrorStack es[static 1] )
{
   cScanner* sca = &make_scanner_c_( key.s, key.v );
   int64_t pos = 0;
   if ( not read_int64_c_( sca, &pos ) or sca->space != 0 )
      return -1;

   cVecInfo const* info = info_of_json_array_o( arr );
   if ( not in_range_c_( 0, pos, info->cap-1 ) )
      return -1;

   return pos;
}

static OJson* get_via_chars_from_json_array_o( OJsonArray const* arr,
                                               cChars key,
                                               cErrorStack es[static 1] )
{
   int64_t pos = get_json_array_pos( arr, key, es );
   if ( pos < 0 )
      return NULL;

   return get_from_json_array_o( arr, pos );
}

static bool remove_via_chars_from_json_array_o( OJsonArray* arr,
                                                cChars key,
                                                cErrorStack es[static 1] )
{
   int64_t pos = get_json_array_pos( arr, key, es );
   if ( pos < 0 )
      return false;

   return remove_from_json_array_o( arr, pos );
}

static bool set_via_chars_on_json_array_o( OJsonArray* arr,
                                           cChars key,
                                           OJson* val,
                                           cErrorStack es[static 1] )
{
   int64_t pos = get_json_array_pos( arr, key, es );
   if ( pos < 0 )
      return false;

   set_on_json_array_o( arr, pos, val );
   return true;
}
