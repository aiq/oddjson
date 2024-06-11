#include "clingo/io/print.h"
#include "clingo/lang/expect.h"
#include "oddjson/oJsonBuilder.h"

int main( void )
{
   init_tap_c_();

   OJsonArray* arr = new_json_array_o();

   add_to_json_array_o( arr, new_json_with_bool_o_( true ) );
   add_to_json_array_o( arr, new_json_with_number_o_( 1 ) );
   add_to_json_array_o( arr, new_json_o_() );
   add_to_json_array_o( arr, new_json_with_array_o_( new_json_array_o() ) );
   
   OJsonArray* beatles = new_json_array_o();
   {
      add_to_json_array_o( beatles, new_json_with_string_o_( lit_c( "john" ) ) );
      add_to_json_array_o( beatles, new_json_with_string_o_( lit_c( "paul" ) ) );
      add_to_json_array_o( beatles, new_json_with_string_o_( lit_c( "george" ) ) );
      add_to_json_array_o( beatles, new_json_with_string_o_( lit_c( "ringo" ) ) );
   }
   add_to_json_array_o( arr, new_json_with_array_o_( beatles ) );
   add_to_json_array_o( arr, new_json_with_object_o_( new_json_object_o() ) );

   OJsonObject* dict = new_json_object_o();
   {
      set_on_json_object_o_( dict, "test", new_json_with_string_o_( lit_c( "testen" ) ) );
      set_on_json_object_o_( dict, "check", let_json_with_string_o_( lit_c( "prüfen" ) ) );
      set_on_json_object_o_( dict, "control", new_json_with_string_o_( lit_c( "kontrollieren" ) ) );
   }
   add_to_json_array_o( arr, new_json_with_object_o_( dict ) );

   oJsonBuilder* b = &json_builder_o_( "", "  " );
   cRecorder* rec = &recorder_c_( 1024 );
   init_json_builder_shell_o( b, rec );

   add_json_array_value_o( b, arr );

   cChars json = built_json_o( b );

   println_c_( "{cs}", json );

   release_c( arr );

   return finish_tap_c_();
}