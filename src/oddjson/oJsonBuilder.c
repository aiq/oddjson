#include "oddjson/oJsonBuilder.h"

#include "clingo/container/pile.h"
#include "clingo/io/read_type.h"
#include "clingo/io/write_type.h"
#include "_/error.h"
#include "_/util.h"

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
   cRecorder ownRec;
   cRecorder* rec;
   oBuilderStatePile pile;
   char const* err;
};

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

static inline bool with_format( oJsonBuilder b[static 1] )
{
   return not is_empty_c_( b->prefix ) or not is_empty_c_( b->indent );
}

static inline bool add_opt_newline( oJsonBuilder b[static 1] )
{
   return with_format( b ) ? write_char_c_( b->_->rec, '\n' )
                           : true;
}

static inline bool add_opt_comma( oJsonBuilder b[static 1] )
{
   if ( b->_->pile.s < 2 )
      return true;

   o_BuilderState state = last_c_( b->_->pile );
   if ( state == o_FirstObjectMember or state == o_FirstArrayValue )
      return add_opt_newline( b );

   return write_char_c_( b->_->rec, ',' ) and add_opt_newline( b );
}

static bool add_indention( oJsonBuilder b[static 1] )
{
   if ( not write_chars_c_( b->_->rec, b->prefix ) )
      return false;

   times_c_( b->_->pile.s - 1, n )
   {
      if ( not write_chars_c_( b->_->rec, b->indent ) )
         return false;
   }

   return true;
}

static inline bool set_lit_error( oJsonBuilder b[static 1],
                                  char const lit[static 1] )
{
   if ( b->_->err != cNoError_ )
   {
      b->_->err = lit;
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

   return true;
}

/******************************************************************************/

static inline bool add_opt_space( oJsonBuilder b[static 1] )
{
   return with_format( b ) ? write_char_c_( b->_->rec, ' ' )
                           : true;
}

static bool add_member_head( oJsonBuilder b[static 1], cChars name )
{
   return add_opt_comma( b ) and
          add_indention( b ) and
          record_quoted_json_string( b->_->rec, name ) and
          write_char_c_( b->_->rec, ':' ) and
          add_opt_space( b );
}

static bool add_member_tail( oJsonBuilder b[static 1] )
{
   return set_to_nth_state( b );
}

static bool add_value_head( oJsonBuilder b[static 1] )
{
   return add_opt_comma( b ) and
          add_indention( b );
}

static bool add_value_tail( oJsonBuilder b[static 1] )
{
   return set_to_nth_state( b );
}

/*******************************************************************************

*******************************************************************************/

static inline bool init_json_builder_internal( oJsonBuilder b[static 1] )
{
   b->_ = alloc_c_( oJsonBuilderInternal );
   if ( b->_ == NULL )
   {
      cleanup_json_builder_o( b );
      return false;
   }

   if ( not alloc_pile_of_builder_state_o( &b->_->pile, 8 ) )
   {
      cleanup_json_builder_o( b );
      return false;
   }
   put_builder_state_o( &b->_->pile, o_FirstArrayValue );

   return true;
}

bool init_json_builder_o( oJsonBuilder b[static 1], int64_t cap)
{
   if ( not init_json_builder_internal( b ) )
   {
      cleanup_json_builder_o( b );
      return false;
   }

   if ( not realloc_recorder_mem_c( &b->_->ownRec, 1024 ) )
   {
      cleanup_json_builder_o( b );
      return NULL;
   }
   b->_->rec = &b->_->ownRec;

   return true;
}

bool init_json_builder_shell_o( oJsonBuilder b[static 1],
                                cRecorder rec[static 1] )
{
   if ( not init_json_builder_internal( b ) )
   {
      cleanup_json_builder_o( b );
      return false;
   }
   b->_->rec = rec;

   return true;
}

void cleanup_json_builder_o( oJsonBuilder b[static 1] )
{
   if ( b->_ )
   {
      if ( &b->_->ownRec == b->_->rec )
      {
         free_recorder_mem_c( b->_->rec );
      }

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

   return b->_->err != cNoError_ or
          b->_->rec->err != cNoError_;
}

bool push_json_builder_error_o( cErrorStack es[static 1], oJsonBuilder b[static 1] )
{
   must_exist_c_( b->_ );

   if ( b->_->rec->err != cNoError_ )
   {
      return push_recorder_error_c( es, b->_->rec ) ||
             push_json_lit_error_o( es, "oJsonBuilder" );
   }
   if ( b->_->err != cNoError_ )
   {
      return push_lit_error_c( es, b->_->err ) ||
             push_json_lit_error_o( es, "oJsonBuilder" );
   }

   return false;
}

/*******************************************************************************

*******************************************************************************/

cChars built_json_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b->_ );
   return recorded_chars_c( b->_->rec );
}

/*******************************************************************************

*******************************************************************************/

bool begin_json_object_o( oJsonBuilder b[static 1], cChars name )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_member_head( b, name ) and
          write_char_c_( b->_->rec, '{' ) and
          put_builder_state_o( &b->_->pile, o_FirstObjectMember );
}

bool begin_json_object_value_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_value_head( b ) and
          write_char_c_( b->_->rec, '{' ) and
          put_builder_state_o( &b->_->pile, o_FirstObjectMember );
}

bool finish_json_object_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   if ( b->_->pile.s == 0 )
      return set_lit_error( b, "unexpected finish of object" );

   o_BuilderState state = last_c_( b->_->pile );
   if ( state == o_FirstObjectMember )
   {
      b->_->pile.s -= 1;
      return write_char_c_( b->_->rec, '}' ) and set_to_nth_state( b );
   }

   b->_->pile.s -= 1;
   return add_opt_newline( b ) and
          add_indention( b ) and
          write_char_c_( b->_->rec, '}' ) and
          set_to_nth_state( b );
}

/**************************************/

bool begin_json_array_o( oJsonBuilder b[static 1], cChars name )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_member_head( b, name ) and
          write_char_c_( b->_->rec, '[' ) and
          put_builder_state_o( &b->_->pile, o_FirstArrayValue );
}

bool begin_json_array_value_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_value_head( b ) and
          write_char_c_( b->_->rec, '[') and
          put_builder_state_o( &b->_->pile, o_FirstArrayValue );
}

bool finish_json_array_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   if ( b->_->pile.s == 0 )
      return set_lit_error( b, "unexpected finish of array" );

   o_BuilderState state = last_c_( b->_->pile );
   if ( state == o_FirstArrayValue )
   {
      b->_->pile.s -= 1;
      return write_char_c_( b->_->rec, ']' ) and set_to_nth_state( b );
   }

   b->_->pile.s -= 1;
   return add_opt_newline( b ) and
          add_indention( b ) and
          write_char_c_( b->_->rec, ']' ) and
          set_to_nth_state( b );
}

/*******************************************************************************

*******************************************************************************/

bool add_json_bool_o( oJsonBuilder b[static 1], cChars name, bool value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_member_head( b, name ) and
          write_bool_c_( b->_->rec, value ) and
          add_member_tail( b );
}
bool add_json_bool_value_o( oJsonBuilder b[static 1], bool value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_value_head( b ) and
          write_bool_c_( b->_->rec, value ) and
          add_value_tail( b );
}

/**************************************/

bool add_json_null_o( oJsonBuilder b[static 1], cChars name )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_member_head( b, name ) and
          write_cstr_c_( b->_->rec, "null" ) and
          add_member_tail( b );
}

bool add_json_null_value_o( oJsonBuilder b[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_value_head( b ) and
          write_cstr_c_( b->_->rec, "null" ) and
          add_value_tail( b );
}

/**************************************/

static inline char const* conv_number_fmt( char const src[static 1],
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

static inline bool add_raw_fmt_number_value( oJsonBuilder b[static 1],
                                             double value,
                                             char const fmt[static 1] )
{
   if ( not isfinite( value ) )
      return set_lit_error( b, "number value is not finite" );

   fmt = conv_number_fmt( fmt, &recorder_c_( 8 ) );
   if ( fmt == NULL )
      return set_lit_error( b, "invalid number fmt" );

   return write_double_c( b->_->rec, value, fmt );
}

bool add_json_fmt_number_o( oJsonBuilder b[static 1],
                            cChars name,
                            double value,
                            char const fmt[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_member_head( b, name ) and
          add_raw_fmt_number_value( b, value, fmt ) and
          add_member_tail( b );
}

bool add_json_fmt_number_value_o( oJsonBuilder b[static 1],
                                  double value,
                                  char const fmt[static 1] )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_value_head( b ) and
            add_raw_fmt_number_value( b, value, fmt ) and
            add_value_tail( b );
}

/**************************************/

bool add_json_number_o( oJsonBuilder b[static 1], cChars name, double value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_member_head( b, name ) and
          add_raw_fmt_number_value( b, value, "" ) and
          add_member_tail( b );
}

bool add_json_number_value_o( oJsonBuilder b[static 1], double value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_value_head( b ) and
          add_raw_fmt_number_value( b, value, "" ) and
          add_value_tail( b );
}

/**************************************/

bool add_json_string_o( oJsonBuilder b[static 1], cChars name, cChars value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_member_head( b, name ) and
          record_quoted_json_string( b->_->rec, value ) and
          add_value_tail( b );
}

bool add_json_string_value_o( oJsonBuilder b[static 1], cChars value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return add_value_head( b ) and
          record_quoted_json_string( b->_->rec, value ) and
          add_value_tail( b );
}

/**************************************/

static inline bool add_json_array_entries( oJsonBuilder b[static 1],
                                           OJsonArray const* arr )
{
   cVecInfo const* info = info_of_json_array_o( arr );
   times_c_( info->count, i )
   {
      OJson const* entry = get_from_json_array_o( arr, i );
      if ( not add_json_value_o( b, entry ) )
         return false;
   }
   return true;        
}

bool add_json_array_o( oJsonBuilder b[static 1],
                       cChars name,
                       OJsonArray const* arr )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   return begin_json_array_o( b, name ) and
          add_json_array_entries( b, arr ) and
          finish_json_array_o( b );
}

bool add_json_array_value_o( oJsonBuilder b[static 1],
                             OJsonArray const* arr )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;
   
   return begin_json_array_value_o( b ) and
          add_json_array_entries( b, arr ) and
          finish_json_array_o( b );
}

/**************************************/

SLICES_C_(
   CString const*,   // Type
   tmpStrings,       // SliceType
   tmpVarStrings     // VarSliceType
)

static QSORT_C_(
   sort_keys,        // FuncName
   tmpVarStrings,    // FuncName
   CString const*,   // ValueType
   cmp_string_c,     // CmpFunc
   do_deref_c_       // DoDeref
)

static inline bool sorted_keys( OJsonObject const* obj, tmpVarStrings keys[static 1] )
{
   *keys = (tmpVarStrings)empty_c_();

   cMapInfo const* info = info_of_json_object_o( obj );
   if ( info->count == 0 ) return true;

   *keys = (tmpVarStrings)heap_slice_c_( info->count, CString const* );
   if ( keys->v == NULL ) return false;

   int64_t i = 0;
   CString const* key;
   OJson const* val;
   iterate_map_c_( itr, &key, &val, obj, next_in_json_object_o )
   {
      keys->v[i] = key;
      ++i;
   }
   sort_keys( *keys );
   return true;
}

static inline bool add_json_object_entries( oJsonBuilder b[static 1],
                                            OJsonObject const* obj )
{
   tmpVarStrings keys;
   if ( not sorted_keys( obj, &keys ) ) return false;

   for_each_c_( CString const**, key, keys )
   {
      OJson const* val = get_from_json_object_o( obj, *key );
      if ( not add_json_o( b, sc_c( *key ), val ) )
      {
         free( keys.v );
         return false;
      }
   }

   free( keys.v );
   return true;
}

bool add_json_object_o( oJsonBuilder b[static 1],
                        cChars name,
                        OJsonObject const* obj )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;
   
   return begin_json_object_o( b, name ) and
          add_json_object_entries( b, obj ) and
          finish_json_object_o( b );
}

bool add_json_object_value_o( oJsonBuilder b[static 1],
                              OJsonObject const* obj )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;
   
   return begin_json_object_value_o( b ) and
          add_json_object_entries( b, obj ) and
          finish_json_object_o( b );
}

/**************************************/

bool add_json_o( oJsonBuilder b[static 1], cChars name, OJson const* value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   switch ( value->type )
   {
      case o_JsonObject:
         return add_json_object_o( b, name, value->object );
      case o_JsonArray:
         return add_json_array_o( b, name, value->array );
      case o_JsonString:
         return add_json_string_o( b, name, sc_c( value->string ) );
      case o_JsonNumber:
         return add_json_number_o( b, name, value->number );
      case o_JsonBool:
         return add_json_bool_o( b, name, value->boolean );
      case o_JsonNull:
         return add_json_null_o( b, name );
      default:
         return set_lit_error( b, "unknown json type" );
   }
}

bool add_json_value_o( oJsonBuilder b[static 1], OJson const* value )
{
   must_exist_c_( b );
   if ( json_builder_has_error_o( b ) ) return false;

   switch ( value->type )
   {
      case o_JsonObject:
         return add_json_object_value_o( b, value->object );
      case o_JsonArray:
         return add_json_array_value_o( b, value->array );
      case o_JsonString:
         return add_json_string_value_o( b, sc_c( value->string ) );
      case o_JsonNumber:
         return add_json_number_value_o( b, value->number );
      case o_JsonBool:
         return add_json_bool_value_o( b, value->boolean );
      case o_JsonNull:
         return add_json_null_value_o( b );
      default:
         return set_lit_error( b, "unknown json type" );
   }
}
