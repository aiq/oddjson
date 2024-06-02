#include "clingo/lang/expect.h"
#include "clingo/io/write_type.h"
#include "oddjson/oJsonParser.h"

bool check( char const* json, OJsonObject* exp, cRecorder rec[static 1] )
{
   oJsonParser* p = &(oJsonParser){};
   init_json_parser_o( p, c_c( json ) );

   OJsonObject* obj = new_json_object_o();

   if ( not parse_json_object_o( p, obj ) )
   {
      release_c( obj );
      cErrorStack* es = &error_stack_c_( 1024 );
      push_json_parser_error_o( es, p );
      write_error_c( rec, es->err, "" );
      return false;
   }

   return false;
}

int main( void )
{
   init_tap_c_();
/*
   testSlice tests = slice_c_( test,
      t_(
         "{\n"
         "  \"name\":\"Full HD\",\n"
         "  \"ignore\":\"me\",\n"
         "  \"resolutions\":[\n"
         "    {\n"
         "      \"width\":1280,\n"
         "      \"height\":720\n"
         "    },\n"
         "    {\n"
         "      \"width\":1920,\n"
         "      \"height\":1080\n"
         "    }\n"
         "  ],\n"
         "  \"overscan\":true,\n"
         "  \"missing\":null\n"
         "}"
      )
   );
*/
   return finish_tap_c_();
}