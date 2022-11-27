#include "oddjson/OJsonBuilder.h"

#include "clingo/container/pile.h"
#include "clingo/io/write_type.h"
#include "clingo/string/CStringBuilder.h"
#include "_/error.h"

/*******************************************************************************

*******************************************************************************/

enum o_BuilderState
{
   o_FirstObjectMember = 1,
   o_NthObjectMember = 2,
   o_FirstArrayValue = 3,
   o_NthArrayValue = 4,
};
typedef enum o_BuilderState o_BuilderState;

enum o_BuilderError
{
   o_NoBuilderError = 0,
   o_BuilderMemError = 1,
   o_BuilderUnexpectedClose = 2,
};
typedef enum o_BuilderError o_BuilderError;

STATIC_VAL_PILE_IMPL_C_(
   __attribute__((unused)),   // Attr
   o_BuilderState,            // Type
   oBuilderStatePile,         // PileType
   builder_state_o            // FuncSuffix
)

/*******************************************************************************
********************************************************* Types and Definitions
********************************************************************************
 
*******************************************************************************/

struct OJsonBuilder {
   cChars prefix;
   cChars indent;
   CStringBuilder* b;
   oBuilderStatePile pile;
   o_BuilderError err;
};

static inline void cleanup( void* instance )
{
   OJsonBuilder* b = instance;
   release_c( b->b );

   if ( b->pile.v )
   {
      free( b->pile.v );
   }
}

cMeta const O_JsonBuilderMeta = {
   .desc = stringify_c_( OJsonBuilder ),
   .cleanup = &cleanup
};

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

static inline bool with_newline( OJsonBuilder b[static 1] )
{
   return not is_empty_c_( b->prefix ) or not is_empty_c_( b->indent );
}

static inline bool append_opt_newline( OJsonBuilder b[static 1] )
{
   return with_newline( b ) ? append_char_c( b->b, '\n' )
                            : true;
}

static inline bool append_opt_comma( OJsonBuilder b[static 1] )
{
   if ( is_empty_c_( b->pile ) )
      return true;

   o_BuilderState state = last_c_( b->pile );
   if ( state == o_FirstObjectMember or state == o_FirstArrayValue )
      return append_opt_newline( b );

   return append_char_c( b->b, ',' ) and append_opt_newline( b );
}

static bool append_indention( OJsonBuilder b[static 1] )
{
   if ( not append_chars_c( b->b, b->prefix ) )
      return false;

   times_c_( b->pile.s, n )
   {
      if ( not append_chars_c( b->b, b->indent ) )
         return false;
   }

   return true;
}

static inline bool append_escaped_char( OJsonBuilder b[static 1], char c )
{
   switch ( c )
   {
      case '\"': return append_cstr_c( b->b, "\\\"" ); // double quotation mark
      case '\\': return append_cstr_c( b->b, "\\\\" ); // backslash
      case '\b': return append_cstr_c( b->b, "\\b" );  // backspace
      case '\f': return append_cstr_c( b->b, "\\f" );  // formfeef
      case '\n': return append_cstr_c( b->b, "\\n" );  // newline
      case '\r': return append_cstr_c( b->b, "\\r" );  // carriage return
      case '\t': return append_cstr_c( b->b, "\\t" );  // horzontal tab
      default: return append_char_c( b->b, c );
   }
}

static inline bool set_error( OJsonBuilder b[static 1],
                              o_BuilderError err )
{
   b->err = err;
   return false;
}

static inline bool set_to_nth_state( OJsonBuilder b[static 1] )
{
   o_BuilderState* state = rbegin_c_( b->pile );
   if ( state == NULL )
      return false;
   
   if ( *state == o_FirstObjectMember )
   {
      *state = o_NthObjectMember;
      return true;
   }

   if ( *state == o_FirstArrayValue )
   {
      *state = o_NthArrayValue;
      return true;
   }

   return false;
}

/******************************************************************************/

static inline bool append_raw_bool_value( OJsonBuilder b[static 1],
                                          bool value )
{
   char const* cstr = value ? "true" : "false";
   return append_cstr_c( b->b, cstr );
}

static inline bool appen_raw_null_value( OJsonBuilder b[static 1] )
{
   return append_cstr_c( b->b, "null" );
}

static inline bool append_raw_number_value( OJsonBuilder b[static 1],
                                            double value )
{
   cRecorder* rec = &recorder_c_( 256 );
   write_double_c( rec, value, "" );
   return append_chars_c( b->b, recorded_chars_c( rec ) );
}

static inline bool append_raw_string_value( OJsonBuilder b[static 1],  
                                            cChars value )
{
   if ( not append_char_c( b->b, '"' ) )
      return false;

   cRune r;
   iterate_runes_c_( itr, r, value )
   {
      if ( rune_size_c( r ) > 1 )
      {
         if ( not append_rune_c( b->b, r ) )
            return false;
      }
      else
      {
         if ( not append_escaped_char( b, r.c[0] ) )
            return false;
      }  
   }

   if ( not append_char_c( b->b, '"' ) )
      return false;

   return true;
}

/*******************************************************************************

*******************************************************************************/

OJsonBuilder* new_json_builder_o( void )
{
   return make_json_builder_o_( "", "" );
}

OJsonBuilder* make_json_builder_o( cChars prefix, cChars indent )
{
   OJsonBuilder* b = new_object_c_( OJsonBuilder, &O_JsonBuilderMeta );
   if ( b == NULL )
   {
      return NULL;
   }

   b->b = new_string_builder_c();
   if ( b->b == NULL )
   {
      release_c( b );
      return NULL;
   }

   if ( not alloc_pile_of_builder_state_o( &(b->pile),8 ) )
   {
      release_c( b );
      return NULL;
   }

   b->prefix = prefix;
   b->indent = indent;

   return b;
}

/*******************************************************************************

*******************************************************************************/

bool push_json_builder_error_o( cErrorStack es[static 1], OJsonBuilder* b )
{
   must_exist_c_( b );

   if ( b->err == o_BuilderMemError ) {
      return push_errno_error_c( es, ENOMEM ) ||
             push_json_lit_error_o( es, "OJsonBuilder" );
   }

   return false;
}

/*******************************************************************************

*******************************************************************************/

cChars built_json_o( OJsonBuilder* b )
{
   must_exist_c_( b );
   return built_chars_c( b->b );
}

char const* built_json_cstr_o( OJsonBuilder* b )
{
   must_exist_c_( b );
   return built_cstr_c( b->b );
}

/*******************************************************************************

*******************************************************************************/

bool begin_json_object_o( OJsonBuilder* b, cChars name )
{
   must_exist_c_( b );

   return append_opt_comma( b ) and
          append_indention( b ) and
          append_raw_string_value( b, name ) and
          append_char_c( b->b, ':' ) and
          append_char_c( b->b, '{' ) and
          put_builder_state_o( &(b->pile), o_FirstObjectMember );
}

bool begin_json_object_value_o( OJsonBuilder* b )
{
   must_exist_c_( b );

   return append_opt_comma( b ) and
          append_indention( b ) and
          append_char_c( b->b, '{' ) and
          put_builder_state_o( &(b->pile), o_FirstObjectMember );
}

bool finish_json_object_o( OJsonBuilder* b )
{
   must_exist_c_( b );

   if ( b->pile.s == 0 )
      return set_error( b, o_BuilderUnexpectedClose );

   b->pile.s -= 1;
   return append_opt_newline( b ) and
          append_indention( b ) and
          append_char_c( b->b, '}' ) and
          set_to_nth_state( b );
}

/*******************************************************************************

*******************************************************************************/

bool begin_json_array_o( OJsonBuilder* b, cChars name )
{
   must_exist_c_( b );

   return append_opt_comma( b ) and
          append_indention( b ) and
          append_raw_string_value( b, name ) and
          append_char_c( b->b, ':' ) and
          append_char_c( b->b, '[' ) and
          put_builder_state_o( &(b->pile), o_FirstArrayValue );
}

bool begin_json_array_value_o( OJsonBuilder* b )
{
   must_exist_c_( b );

   return append_opt_comma( b ) and
          append_char_c( b->b, '[') and
          put_builder_state_o( &(b->pile), o_FirstArrayValue );
}

bool finish_json_array_o( OJsonBuilder* b )
{
   must_exist_c_( b );

   if ( b->pile.s == 0 )
      return set_error( b, o_BuilderUnexpectedClose );

   b->pile.s -= 1;
   return append_opt_newline( b ) and
          append_indention( b ) and
          append_char_c( b->b, ']' ) and
          set_to_nth_state( b );
}

/*******************************************************************************

*******************************************************************************/

#define APPEND_( FuncName, ValueType, ValueFunc )                              \
bool FuncName( OJsonBuilder* b, cChars name, ValueType value )                 \
{                                                                              \
   must_exist_c_( b );                                                         \
                                                                               \
   return append_opt_comma( b ) and                                            \
          append_indention( b ) and                                            \
          append_raw_string_value( b, name ) and                               \
          append_char_c( b->b, ':' ) and                                       \
          ValueFunc( b, value ) and                                            \
          set_to_nth_state( b );                                               \
}

APPEND_( append_json_bool_o, bool, append_raw_bool_value )

bool append_json_bool_value_o( OJsonBuilder* b, bool value )
{
   must_exist_c_( b );
   return append_indention( b ) and append_raw_bool_value( b, value );
}

bool append_json_null_o( OJsonBuilder* b, cChars name )
{
   must_exist_c_( b );
   return append_opt_comma( b ) and
          append_indention( b ) and
          append_raw_string_value( b, name ) and
          append_char_c( b->b, ':' ) and
          append_cstr_c( b->b, "null" ) and
          set_to_nth_state( b );
}

bool append_json_null_value_o( OJsonBuilder* b )
{
   must_exist_c_( b );
   return appen_raw_null_value( b );
}

APPEND_( append_json_number_o, double, append_raw_number_value )

bool append_json_number_value_o( OJsonBuilder* b, double value )
{
   must_exist_c_( b );
   cRecorder* rec = &recorder_c_( 256 );
   write_double_c( rec, value, "" );
   return append_chars_c( b->b, recorded_chars_c( rec ) );
}

APPEND_( append_json_string_o, cChars, append_raw_string_value )

bool append_json_string_value_o( OJsonBuilder* b, cChars value )
{
   must_exist_c_( b->b );
   return append_raw_string_value( b, value );
}