#include "clingo/container/vec.h"
#include "clingo/lang/expect.h"
#include "oddjson/oJsonParser.h"
#include "clingo/io/print.h"

/*******************************************************************************
 types
*******************************************************************************/

struct resolution
{
   double width;
   double height;
};
typedef struct resolution resolution;

VAL_VEC_IMPL_C_(
   static,              // Static
   ResolutionVec,       // VecType
   resolution,          // ValType
   resolution_vec,      // FuncSuffix
   C_ResolutionVecMeta, // Meta
   do_nothing_c_        // InDepthCleanup
)

struct InputType
{
   CString* name;
   ResolutionVec* resolutions;
   bool overscan;
   void* missing;
};
typedef struct InputType InputType;

static inline void cleanup( void* instance )
{
   InputType* it = instance;
   release_c( it->name );
   release_c( it->resolutions );
}
static cMeta const C_InputTypeMeta = {
   .desc = stringify_c_( InputType ),
   .cleanup = &cleanup
};

/*******************************************************************************
 json
*******************************************************************************/

char const* inp = "{\n"
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
                  "}";

/*******************************************************************************

*******************************************************************************/
int main( void )
{
   init_tap_c_();

   InputType* it = new_object_c_( InputType, &C_InputTypeMeta );
   it->resolutions = new_resolution_vec();

   oJsonParser* p = &(oJsonParser){};
   init_json_parser_o( p, c_c( inp ) );

   begin_parse_json_object_o( p );
   while ( in_json_object_o( p ) )
   {
      if ( json_name_is_o_( p, "name" ) )
      {
         oJsonString jstr;
         if ( view_json_string_o( p, &jstr ) )
         {
            it->name = decode_json_string_o( &jstr );
         }
      }
      else if ( json_name_is_o_( p, "resolutions" ) )
      {
         begin_parse_json_array_o( p );
         while ( in_json_array_o( p ) )
         {
            resolution res;
            begin_parse_json_object_o( p );
            while ( in_json_object_o( p ) )
            {
               if ( json_name_is_o_( p, "height" ) )
               {
                  parse_json_number_o( p, &(res.height) );
               }
               else if ( json_name_is_o_( p, "width" ) )
               {
                  parse_json_number_o( p, &(res.width) );
               }
               else
               {
                  skip_json_member_o( p );
               }
            }
            finish_parse_json_object_o( p );

            add_to_resolution_vec( it->resolutions, res );
         }
         finish_parse_json_array_o( p );
      }
      else if ( json_name_is_o_( p, "overscan" ) )
      {
         parse_json_bool_o( p, &(it->overscan) );
      }
      else if ( json_name_is_o_( p, "missing" ) )
      {
         parse_json_null_o( p );
      }
      else
      {
         skip_json_member_o( p );
      }
   }
   finish_parse_json_object_o( p );

   expect_c_( string_is_c( it->name, "Full HD" ) );

   release_c( it );
   return finish_tap_c_();
}