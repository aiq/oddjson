#include "clingo/io/print.h"
#include "clingo/lang/expect.h"
#include "oddjson/OJsonBuilder.h"

TEMP_SLICE_C_(
   resolution,
   {
      double width;
      double height;
   }
)
#define r_( ... ) ((resolution){__VA_ARGS__})

int main( void )
{
   init_tap_c_();

   OJsonBuilder* b = make_json_builder_o_( "> ", "  " );

   resolutionSlice resolutions = slice_c_( resolution,
      r_( 1280, 720 ),
      r_( 1920, 1080)
   );

   begin_json_object_value_o( b );
   {
      append_json_string_o_( b, "name", c_c( "Full HD" ) );

      begin_json_array_o_( b, "resolutions" );
      for_each_c_( resolution const*, res, resolutions )
      {
         begin_json_object_value_o( b );
         {
            append_json_number_o_( b, "width", res->width );
            append_json_number_o_( b, "height", res->height );
         }
         finish_json_object_o( b );
      }
      finish_json_array_o( b );

      append_json_bool_o_( b, "overscan", true );
      append_json_null_o_( b, "missing" );
   }
   finish_json_object_o( b );


   cChars json = built_json_o( b );
   char const* exp = "> {\n"
                     ">   \"name\":\"Full HD\",\n"
                     ">   \"resolutions\":[\n"
                     ">     {\n"
                     ">       \"width\":1280,\n"
                     ">       \"height\":720\n"
                     ">     },\n"
                     ">     {\n"
                     ">       \"width\":1920,\n"
                     ">       \"height\":1080\n"
                     ">     }\n"
                     ">   ],\n"
                     ">   \"overscan\":true,\n"
                     ">   \"missing\":null\n"
                     "> }";
      bool res = chars_is_c( json, exp );
      tap_descf_c( res, "expected '%s', got '%s'", exp, json.v );

   release_c( b );

   return finish_tap_c_();
}
