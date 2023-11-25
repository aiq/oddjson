#include "oddjson/oJsonBuilder.h"

#include "clingo/container/pile.h"
#include "clingo/io/read_type.h"
#include "clingo/io/write_type.h"
#include "clingo/string/cStringBuilder.h"
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

struct oJsonBuilderInternal {
   cStringBuilder b;
   oBuilderStatePile pile;
   o_JsonBuilderError err;
   char const* lit;
};

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

static inline bool with_newline( oJsonBuilder b[static 1] )
{
   return not is_empty_c_( b->prefix ) or not is_empty_c_( b->indent );
}

static inline bool append_opt_newline( oJsonBuilder b[static 1] )
{
   return with_newline( b ) ? append_char_c( &b->_->b, '\n' )
                            : true;
}

static inline bool append_opt_comma( oJsonBuilder b[static 1] )
{
   if ( b->_->pile.s < 2 )
      return true;

   o_BuilderState state = last_c_( b->_->pile );
   if ( state == o_FirstObjectMember or state == o_FirstArrayValue )
      return append_opt_newline( b );

   return append_char_c( &b->_->b, ',' ) and append_opt_newline( b );
}

static bool append_indention( oJsonBuilder b[static 1] )
{
   if ( not append_chars_c( &b->_->b, b->prefix ) )
      return false;

   times_c_( b->_->pile.s - 1, n )
   {
      if ( not append_chars_c( &b->_->b, b->indent ) )
         return false;
   }

   return true;
}

static inline bool append_escaped_char( oJsonBuilder b[static 1], char c )
{
   switch ( c )
   {
      case '\"': return append_cstr_c( &b->_->b, "\\\"" ); // double quotation mark
      case '\\': return append_cstr_c( &b->_->b, "\\\\" ); // backslash
      case '\b': return append_cstr_c( &b->_->b, "\\b" );  // backspace
      case '\f': return append_cstr_c( &b->_->b, "\\f" );  // formfeef
      case '\n': return append_cstr_c( &b->_->b, "\\n" );  // newline
      case '\r': return append_cstr_c( &b->_->b, "\\r" );  // carriage return
      case '\t': return append_cstr_c( &b->_->b, "\\t" );  // horzontal tab
      default: return append_char_c( &b->_->b, c );
   }
}

static inline bool set_error( oJsonBuilder b[static 1],
                              o_JsonBuilderError err )
{
   if ( b->_->err != cNoError_ )
      b->_->err = err;

   return false;
}

static inline bool set_lit_error( oJsonBuilder b[static 1],
                                  char const lit[static 1] )
{
   if ( b->_->err != cNoError_ )
   {
      b->_->err = o_JsonBuilderLitError;
      b->_->lit = lit;
   }
   return false;
}

static inline bool set_to_nth_state( oJsonBuilder b[static 1] )
{
   o_BuilderState* state = rbegin_c_( b->_->pile );
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

static inline bool append_raw_bool_value( oJsonBuilder b[static 1],
                                          bool value )
{
   char const* cstr = value ? "true" : "false";
   return append_cstr_c( &b->_->b, cstr );
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

static inline bool append_raw_fmt_number_value( oJsonBuilder b[static 1],
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
   return append_chars_c( &b->_->b, recorded_chars_c( rec ) );
}

static inline bool append_raw_string_value( oJsonBuilder b[static 1],  
                                            cChars value )
{
   if ( not append_char_c( &b->_->b, '"' ) )
      return set_error( b, o_JsonBuilderMemError );

   cRune r;
   iterate_runes_c_( itr, &r, value )
   {
      if ( rune_size_c( r ) > 1 )
      {
         if ( not append_rune_c( &b->_->b, r ) )
            return set_error( b, o_JsonBuilderMemError );
      }
      else
      {
         if ( not append_escaped_char( b, r.c[0] ) )
            return set_error( b, o_JsonBuilderMemError );
      }  
   }

   if ( not append_char_c( &b->_->b, '"' ) )
      return set_error( b, o_JsonBuilderMemError );

   return true;
}

/******************************************************************************/

static bool append_member_head( oJsonBuilder b[static 1], cChars name )
{
   return append_opt_comma( b ) and
          append_indention( b ) and
          append_raw_string_value( b, name ) and
          append_char_c( &b->_->b, ':' );
}

static bool append_member_tail( oJsonBuilder b[static 1] )
{
   return set_to_nth_state( b );
}

static bool append_value_head( oJsonBuilder b[static 1] )
{
   return append_opt_comma( b ) and
          append_indention( b );
}

static bool append_value_tail( oJsonBuilder b[static 1] )
{
   return set_to_nth_state( b );
}

/*******************************************************************************

*******************************************************************************/

bool init_json_builder_o( oJsonBuilder b[static 1], int64_t cap)
{
   b->_ = alloc_c_( oJsonBuilderInternal );
   if ( b->_ == NULL )
   {
      cleanup_json_builder_o( b );
      return NULL;
   }

   if ( not init_string_builder_c( &(b->_->b), 1024 ) )
   {
      cleanup_json_builder_o( b );
      return NULL;
   }

   if ( not alloc_pile_of_builder_state_o( &(b->_->pile),8 ) )
   {
      cleanup_json_builder_o( b );
      return NULL;
   }
   put_builder_state_o( &(b->_->pile), o_FirstArrayValue );

   return b;
}

void cleanup_json_builder_o( oJsonBuilder b[static 1] )
{
   if ( b->_ )
   {
      cleanup_string_builder_c( &(b->_->b) );

      if ( b->_->pile.v )
      {
         free( b->_->pile.v );
      }
      free( b->_ );
   }
}

/*******************************************************************************

*******************************************************************************/

bool json_builder_has_error_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b );

   return b->_->err != cNoError_;
}

bool push_json_builder_error_o( cErrorStack es[static 1], oJsonBuilder b[static 1] )
{
   must_exist_c_( b->_ );

   if ( b->_->err == o_JsonBuilderMemError ) {
      return push_errno_error_c( es, ENOMEM ) ||
             push_json_lit_error_o( es, "oJsonBuilder" );
   }
   else if ( b->_->err == o_JsonBuilderLitError ) {
      return push_lit_error_c( es, b->_->lit ) ||
             push_json_lit_error_o( es, "oJsonBuilder" );
   }

   return false;
}

/*******************************************************************************

*******************************************************************************/

cChars built_json_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b->_ );
   return built_chars_c( &b->_->b );
}

char const* built_json_cstr_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b->_ );
   return built_cstr_c( &b->_->b );
}

/*******************************************************************************

*******************************************************************************/

bool begin_json_object_o( oJsonBuilder b[static 1], cChars name )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_member_head( b, name ) and
              append_char_c( &b->_->b, '{' ) and
              put_builder_state_o( &(b->_->pile), o_FirstObjectMember );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool begin_json_object_value_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_value_head( b ) and
              append_char_c( &b->_->b, '{' ) and
              put_builder_state_o( &(b->_->pile), o_FirstObjectMember );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool finish_json_object_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   if ( b->_->pile.s == 0 )
      return set_lit_error( b, "unexpected finish of object" );

   b->_->pile.s -= 1;
   bool res = append_opt_newline( b ) and
              append_indention( b ) and
              append_char_c( &b->_->b, '}' ) and
              set_to_nth_state( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

/**************************************/

bool begin_json_array_o( oJsonBuilder b[static 1], cChars name )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_member_head( b, name ) and
              append_char_c( &b->_->b, '[' ) and
              put_builder_state_o( &(b->_->pile), o_FirstArrayValue );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool begin_json_array_value_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_value_head( b ) and
              append_char_c( &b->_->b, '[') and
              put_builder_state_o( &(b->_->pile), o_FirstArrayValue );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool finish_json_array_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   if ( b->_->pile.s == 0 )
      return set_lit_error( b, "unexpected finish of object" );

   b->_->pile.s -= 1;
   bool res = append_opt_newline( b ) and
              append_indention( b ) and
              append_char_c( &b->_->b, ']' ) and
              set_to_nth_state( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

/*******************************************************************************

*******************************************************************************/

bool append_json_bool_o( oJsonBuilder b[static 1], cChars name, bool value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_member_head( b, name ) and
              append_raw_bool_value( b, value ) and
              append_member_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}
bool append_json_bool_value_o( oJsonBuilder b[static 1], bool value )
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

bool append_json_null_o( oJsonBuilder b[static 1], cChars name )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_member_head( b, name ) and
              append_cstr_c( &b->_->b, "null" ) and
              append_member_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool append_json_null_value_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_value_head( b ) and
              append_cstr_c( &b->_->b, "null" ) and
              append_value_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

/**************************************/

bool append_json_fmt_number_o( oJsonBuilder b[static 1],
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

bool append_json_fmt_number_value_o( oJsonBuilder b[static 1],
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

bool append_json_number_o( oJsonBuilder b[static 1], cChars name, double value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_member_head( b, name ) and
              append_raw_fmt_number_value( b, value, "" ) and
              append_member_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool append_json_number_value_o( oJsonBuilder b[static 1], double value )
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

bool append_json_string_o( oJsonBuilder b[static 1], cChars name, cChars value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_member_head( b, name ) and
              append_raw_string_value( b, value ) and
              append_value_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}

bool append_json_string_value_o( oJsonBuilder b[static 1], cChars value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   bool res = append_value_head( b ) and
              append_raw_string_value( b, value ) and
              append_value_tail( b );
   return res ? true
              : set_error( b, o_JsonBuilderMemError );
}