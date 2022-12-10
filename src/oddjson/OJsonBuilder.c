#include "oddjson/OJsonBuilder.h"

#include "clingo/container/pile.h"
#include "clingo/io/read_type.h"
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

enum o_JsonBuilderError
{
   o_JsonBuilderMemError = 1,
   o_JsonBuilderLitError = 2
};
typedef enum o_JsonBuilderError o_JsonBuilderError;

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
   o_JsonBuilderError err;
   char const* lit;
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
   if ( b->pile.s < 2 )
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

   times_c_( b->pile.s - 1, n )
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
                              o_JsonBuilderError err )
{
   if ( b->err != cNoError_ )
      b->err = err;

   return false;
}

static inline bool set_lit_error( OJsonBuilder b[static 1],
                                  char const lit[static 1] )
{
   if ( b->err != cNoError_ )
   {
      b->err = o_JsonBuilderLitError;
      b->lit = lit;
   }
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

static inline char const* conv_fmt( char const src[static 1],
                                    cRecorder rec[static 1] )
{
   cScanner* sca = &cstr_scanner_c_( src );
   if ( move_if_char_c( sca, 'g' ) )
   {
      record_char_c( rec, 'g' );
   }
   else if ( move_if_char_c( sca, 'f' ) )
   {
      record_char_c( rec, 'f' );
   }
   else if ( move_if_char_c( sca, 'e' ) )
   {
      record_char_c( rec, 'e' );
   }

   uint8_t precision;
   if ( read_uint8_c( sca, &precision, "d" ) )
   {
      record_char_c( rec, '.' );
      write_uint8_c( rec, precision, "d" );
   }

   return sca->space == 0 ? turn_into_cstr_c( rec )
                          : NULL;
}

static inline bool append_raw_fmt_number_value( OJsonBuilder b[static 1],
                                                double value,
                                                char const fmt[static 1] )
{
   if ( not isfinite( value ) )
      return set_lit_error( b, "number value is not finite" );

   fmt = conv_fmt( fmt, &recorder_c_( 8 ) );
   if ( fmt == NULL )
      return set_lit_error( b, "invalid number fmt" );

   cRecorder* rec = &recorder_c_( 256 );
   write_double_c( rec, value, fmt );
   return append_chars_c( b->b, recorded_chars_c( rec ) );
}

static inline bool append_raw_string_value( OJsonBuilder b[static 1],  
                                            cChars value )
{
   if ( not append_char_c( b->b, '"' ) )
      return set_error( b, o_JsonBuilderMemError );

   cRune r;
   iterate_runes_c_( itr, r, value )
   {
      if ( rune_size_c( r ) > 1 )
      {
         if ( not append_rune_c( b->b, r ) )
            return set_error( b, o_JsonBuilderMemError );
      }
      else
      {
         if ( not append_escaped_char( b, r.c[0] ) )
            return set_error( b, o_JsonBuilderMemError );
      }  
   }

   if ( not append_char_c( b->b, '"' ) )
      return set_error( b, o_JsonBuilderMemError );

   return true;
}

/******************************************************************************/

static bool append_member_head( OJsonBuilder* b, cChars name )
{
   return append_opt_comma( b ) and
          append_indention( b ) and
          append_raw_string_value( b, name ) and
          append_char_c( b->b, ':' );
}

static bool append_member_tail( OJsonBuilder* b )
{
   return set_to_nth_state( b );
}

static bool append_value_head( OJsonBuilder* b )
{
   return append_opt_comma( b ) and
          append_indention( b );
}

static bool append_value_tail( OJsonBuilder* b )
{
   return set_to_nth_state( b );
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
   put_builder_state_o( &(b->pile), o_FirstArrayValue );

   b->prefix = prefix;
   b->indent = indent;

   return b;
}

/*******************************************************************************

*******************************************************************************/

bool json_builder_has_error_o( OJsonBuilder* b )
{
   must_exist_c_( b );

   return b->err != cNoError_;
}

bool push_json_builder_error_o( cErrorStack es[static 1], OJsonBuilder* b )
{
   must_exist_c_( b );

   if ( b->err == o_JsonBuilderMemError ) {
      return push_errno_error_c( es, ENOMEM ) ||
             push_json_lit_error_o( es, "OJsonBuilder" );
   }
   else if ( b->err == o_JsonBuilderLitError ) {
      return push_lit_error_c( es, b->lit ) ||
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
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_member_head( b, name ) and
              append_char_c( b->b, '{' ) and
              put_builder_state_o( &(b->pile), o_FirstObjectMember );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool begin_json_object_value_o( OJsonBuilder* b )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_value_head( b ) and
              append_char_c( b->b, '{' ) and
              put_builder_state_o( &(b->pile), o_FirstObjectMember );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool finish_json_object_o( OJsonBuilder* b )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   if ( b->pile.s == 0 )
      return set_lit_error( b, "unexpected finish of object" );

   b->pile.s -= 1;
   bool res = append_opt_newline( b ) and
              append_indention( b ) and
              append_char_c( b->b, '}' ) and
              set_to_nth_state( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

/**************************************/

bool begin_json_array_o( OJsonBuilder* b, cChars name )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_member_head( b, name ) and
              append_char_c( b->b, '[' ) and
              put_builder_state_o( &(b->pile), o_FirstArrayValue );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool begin_json_array_value_o( OJsonBuilder* b )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_value_head( b ) and
              append_char_c( b->b, '[') and
              put_builder_state_o( &(b->pile), o_FirstArrayValue );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool finish_json_array_o( OJsonBuilder* b )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   if ( b->pile.s == 0 )
      return set_lit_error( b, "unexpected finish of object" );

   b->pile.s -= 1;
   bool res = append_opt_newline( b ) and
              append_indention( b ) and
              append_char_c( b->b, ']' ) and
              set_to_nth_state( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

/*******************************************************************************

*******************************************************************************/

bool append_json_bool_o( OJsonBuilder* b, cChars name, bool value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_member_head( b, name ) and
              append_raw_bool_value( b, value ) and
              append_member_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}
bool append_json_bool_value_o( OJsonBuilder* b, bool value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_value_head( b ) and
              append_raw_bool_value( b, value ) and
              append_value_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

/**************************************/

bool append_json_null_o( OJsonBuilder* b, cChars name )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_member_head( b, name ) and
              append_cstr_c( b->b, "null" ) and
              append_member_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool append_json_null_value_o( OJsonBuilder* b )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_value_head( b ) and
              append_cstr_c( b->b, "null" ) and
              append_value_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

/**************************************/

bool append_json_fmt_number_o( OJsonBuilder* b,
                               cChars name,
                               double value,
                               char const fmt[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_member_head( b, name ) and
              append_raw_fmt_number_value( b, value, fmt ) and
              append_member_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool append_json_fmt_number_value_o( OJsonBuilder* b,
                                     double value,
                                     char const fmt[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_value_head( b ) and
              append_raw_fmt_number_value( b, value, fmt ) and
              append_value_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

/**************************************/

bool append_json_number_o( OJsonBuilder* b, cChars name, double value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_member_head( b, name ) and
              append_raw_fmt_number_value( b, value, "" ) and
              append_member_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool append_json_number_value_o( OJsonBuilder* b, double value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_value_head( b ) and
              append_raw_fmt_number_value( b, value, "" ) and
              append_value_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

/**************************************/

bool append_json_string_o( OJsonBuilder* b, cChars name, cChars value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_member_head( b, name ) and
              append_raw_string_value( b, value ) and
              append_value_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool append_json_string_value_o( OJsonBuilder* b, cChars value )
{
   must_exist_c_( b->b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_value_head( b ) and
              append_raw_string_value( b, value ) and
              append_value_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}