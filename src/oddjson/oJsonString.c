#include "oddjson/oJsonString.h"

#include "clingo/io/print.h"
#include "clingo/io/read_type.h"

/*******************************************************************************

*******************************************************************************/

static inline char const* get_uxxxx_value( cChars chars,
                                           char const* itr,
                                           cRune r[static 1] )
{
   char const* beg = itr+1;
   char const* end = beg+4;
   if ( not points_into_c_( chars, end ) ) return NULL;

   cScanner* sca = &make_scanner_c_( 4, beg );
   uint16_t val = 0;
   if ( not read_uint16_c( sca, &val, "x" ) ) return NULL;

   if ( not is_high_utf16_surrogate_c( val ) )
   {
      *r = utf16_rune_c( val );
      return end-1;
   }

   beg = end;
   end = beg+6;
   if ( not points_into_c_( chars, end ) ) return NULL;

   sca = &make_scanner_c_( 6, beg );
   if ( not move_if_chars_c_( sca, "\\u" ) ) return NULL;

   uint16_t low = 0;
   if ( not read_uint16_c( sca, &low, "x" ) ) return NULL;

   *r = surrogate_rune_c( val, low );
   return end-1;
}

static inline char const* next_raw_rune( cChars raw,
                                         char const* itr,
                                         cRune r[static 1],
                                         bool isEnd[static 1] )
{
   *isEnd = false;
   itr = next_rune_c( raw, itr, r );
   if ( itr == NULL ) return NULL;

   if ( r->c[0] == '\"' )
   {
      *isEnd = true;
      return itr;
   }

   if ( r->c[0] == '\\' )
   {
      itr = next_rune_c( raw, itr, r );
      switch ( r->c[0] )
      {
         case '\"':
         case '\\':
            return itr;
         case '/':
            *r = rune_c( "/" );
            return itr;
         case 'b':
            *r = rune_c( "\b" );
            return itr;
         case 'f':
            *r = rune_c( "\f" );
            return itr;
         case 'n':
            *r = rune_c( "\n" );
            return itr;
         case 'r':
            *r = rune_c( "\r" );
            return itr;
         case 't':
            *r = rune_c( "\t" );
            return itr;
         case 'u':
            return get_uxxxx_value( raw, itr, r );
      }
   }

   return itr;
}

static inline bool move_sca_back( cScanner sca[static 1], int64_t oldPos )
{
   move_scanner_to_c( sca, oldPos );
   return false;
}

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

cChars decode_json_string_chars_o( oJsonString const jstr[static 1],
                                   cVarChars buf )
{
   cRecorder* rec = &make_recorder_c_( buf.s, buf.v );

   cRune r;
   iterate_json_string_o_( itr, r, jstr )
   {
      if ( not record_rune_c( rec, r ) )
         return (cChars)invalid_c_();
   }

   return recorded_chars_c( rec );
}

static inline char* record_from_json_string( cRecorder rec[static 1],
                                             oJsonString const jstr[static 1] )
{
   cRune r;
   iterate_json_string_o_( itr, r, jstr )
   {
      if ( not record_rune_c( rec, r ) )
         return NULL;
   }

   return turn_into_cstr_c( rec );
}

CString* decode_json_string_o( oJsonString const jstr[static 1] )
{
   int64_t size;
   if ( not iadd64_c( jstr->byteLength, 1, &size ) )
      return NULL;

   cRecorder* rec = &heap_recorder_c_( size );
   if ( rec->mem == NULL )
      return NULL;


   char* cstr = record_from_json_string( rec, jstr );
   if ( cstr == NULL )
   {
      free( rec->mem );
      return NULL;
   }

   CString* str = adopt_cstr_c( cstr );
   if ( str == NULL )
   {
      free( rec->mem );
      return NULL;
   }

   return str;
}

/*******************************************************************************

*******************************************************************************/

bool json_string_is_o( oJsonString const jstr[static 1], cChars chars )
{
   cScanner* sca = &make_scanner_c_( jstr->quoted.s, jstr->quoted.v );
   if ( not move_if_json_string_is_o( sca, chars ) )
      return false;

   return ( sca->space == 0 ) ? true
                              : false;
}

char const* next_json_string_rune_o( oJsonString const jstr[static 1],
                                     char const* itr,
                                     cRune r[static 1] )
{
   bool isEnd = false;
   itr = next_raw_rune( jstr->raw, itr, r, &isEnd );
   if ( isEnd ) return NULL;

   return itr;
}

/*******************************************************************************

*******************************************************************************/

bool move_if_json_string_is_o( cScanner sca[static 1], cChars chars )
{
   int64_t oldPos = sca->pos;
   if ( not move_if_char_c( sca, '\"' ) )
      return false;

   cChars raw = unscanned_chars_c_( sca );

   bool isEnd = false;
   cRune rawRune;
   char const* rawItr = next_raw_rune( raw, NULL, &rawRune, &isEnd );
   cRune expRune;
   char const* expItr = NULL;
   while ( not isEnd )
   {
      expItr = next_rune_c( chars, expItr, &expRune );
      if ( expItr == NULL )
         return move_sca_back( sca, oldPos );

      if ( not eq_rune_c( rawRune, expRune ) )
         return move_sca_back( sca, oldPos );

      rawItr = next_raw_rune( raw, rawItr, &rawRune, &isEnd );
   }

   expItr = next_rune_c( chars, expItr, &expRune );
   if ( expItr != NULL )
      return move_sca_back( sca, oldPos );

   int64_t diff = rawItr - raw.v;
   move_scanner_c( sca, diff );
   if ( not move_if_char_c( sca, '"' ) )
      return move_sca_back( sca, oldPos );
 
   return true;
}

bool scan_json_string_o( cScanner sca[static 1], oJsonString jstr[static 1] )
{
   int64_t oldPos = sca->pos;
   jstr->quoted.v = sca->mem;
   if ( not move_if_char_c( sca, '\"' ) )
      return false;

   jstr->length = 0;
   jstr->byteLength = 0;
   jstr->raw = unscanned_chars_c_( sca );

   bool isEnd = false;
   cRune rawRune;
   char const* rawItr = next_raw_rune( jstr->raw, NULL, &rawRune, &isEnd );
   while ( not isEnd )
   {
      jstr->length += 1;
      jstr->byteLength += rune_size_c( rawRune );

      rawItr = next_raw_rune( jstr->raw, rawItr, &rawRune, &isEnd );
   }

   jstr->raw.s = rawItr - jstr->raw.v;
   move_scanner_c( sca, jstr->raw.s );
   if ( not move_if_char_c( sca, '"' ) )
      return move_sca_back( sca, oldPos );

   jstr->quoted.s = jstr->raw.s + 2;
   return true;
}
