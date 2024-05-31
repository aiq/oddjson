#ifndef ODDJSON_OJSONROUTE_H
#define ODDJSON_OJSONROUTE_H

#include "clingo/io/cRecorder.h"
#include "clingo/io/cScanner.h"
#include "oddjson/apidecl.h"

/*******************************************************************************
********************************************************************* Functions
********************************************************************************

*******************************************************************************/

ODDJSON_API bool record_json_route_key_o( cRecorder rec[static 1],
                                          cChars key );

ODDJSON_API bool record_json_route_index_o( cRecorder rec[static 1],
                                            int64_t index );

/*******************************************************************************

*******************************************************************************/

#endif