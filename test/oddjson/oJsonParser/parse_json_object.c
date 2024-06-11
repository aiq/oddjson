#include "clingo/lang/expect.h"
#include "clingo/io/print.h"
#include "clingo/io/write_type.h"
#include "oddjson/oJsonBuilder.h"
#include "oddjson/oJsonParser.h"

TEMP_SLICE_C_(
   test,
   {
      char const* inp;
      char const* exp;
   }
)
#define t_( ... ) ((test){__VA_ARGS__})

int main( void )
{
   init_tap_c_();

   testSlice tests = slice_c_( test,
      t_(
         "{\n"
         "  \"name\": \"Full HD\",\n"
         "  \"ignore\": \"me\",\n"
         "  \"resolutions\": [\n"
         "    {\n"
         "      \"width\": 1280,\n"
         "      \"height\": 720\n"
         "    },\n"
         "    {\n"
         "      \"width\": 1920,\n"
         "      \"height\": 1080\n"
         "    }\n"
         "  ],\n"
         "  \"emptyArray\": [],\n"
         "  \"emptyObject\": {},\n"
         "  \"overscan\": true,\n"
         "  \"missing\": null\n"
         "}",
         "> {\n"
         ">   \"emptyArray\": [],\n"
         ">   \"emptyObject\": {},\n"
         ">   \"ignore\": \"me\",\n"
         ">   \"missing\": null,\n"
         ">   \"name\": \"Full HD\",\n"
         ">   \"overscan\": true,\n"
         ">   \"resolutions\": [\n"
         ">     {\n"
         ">       \"height\": 720,\n"
         ">       \"width\": 1280\n"
         ">     },\n"
         ">     {\n"
         ">       \"height\": 1080,\n"
         ">       \"width\": 1920\n"
         ">     }\n"
         ">   ]\n"
         "> }"
      )
   );

   for_each_c_( test const*, t, tests )
   {
      oJsonParser* p = &(oJsonParser){};
      init_json_parser_o( p, c_c( t->inp ) );

      OJsonObject* obj = new_json_object_o();
      parse_json_object_o( p, obj );

      cRecorder* rec = &dyn_recorder_c_( 64 );
      oJsonBuilder* b = &json_builder_o_( "> ", "  " );
      init_json_builder_shell_o( b, rec );
      add_json_object_value_o( b, obj );

      expect_c_( recorded_is_c( rec, t->exp ) );

      release_all_c_( obj );
      cleanup_json_builder_o( b );
      free_recorder_mem_c( rec );
   }

   return finish_tap_c_();
}