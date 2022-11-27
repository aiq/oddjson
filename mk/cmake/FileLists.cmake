################################################################################
# INC_FILES
################################################################################
set( INC_FILES
   ${INC_DIR}/oddjson/apidecl.h
   ${INC_DIR}/oddjson/error.h

   ${INC_DIR}/oddjson/o_JsonType.h
   ${INC_DIR}/oddjson/OJsonBuilder.h
   ${INC_DIR}/oddjson/oJsonMember.h
   ${INC_DIR}/oddjson/oJsonParser.h
   ${INC_DIR}/oddjson/oJsonString.h
)

################################################################################
# SRC_FILES
################################################################################
set( SRC_FILES
   ${SRC_DIR}/oddjson/error.c
   ${SRC_DIR}/oddjson/o_JsonType.c
   ${SRC_DIR}/oddjson/OJsonBuilder.c
   ${SRC_DIR}/oddjson/oJsonMember.c
   ${SRC_DIR}/oddjson/oJsonParser.c
   ${SRC_DIR}/oddjson/oJsonString.c
)

################################################################################
# TEST_FILES
################################################################################
set( TEST_FILES
   # OJsonBuilder
   ${TEST_DIR}/oddjson/OJsonBuilder/append_json_bool_value.c
   ${TEST_DIR}/oddjson/OJsonBuilder/append_json_bool.c
   ${TEST_DIR}/oddjson/OJsonBuilder/append_json_number_value.c
   ${TEST_DIR}/oddjson/OJsonBuilder/append_json_string_value.c
   ${TEST_DIR}/oddjson/OJsonBuilder/OJsonBuilder-overview.c
   # oJsonParser
   ${TEST_DIR}/oddjson/oJsonParser/oJsonParser-overview.c
   # oJsonString
   ${TEST_DIR}/oddjson/oJsonString/move_if_json_string_is.c
   ${TEST_DIR}/oddjson/oJsonString/scan_json_string.c
)

################################################################################
# APP_FILES
################################################################################
set( APP_FILES
#   ${APP_DIR}/ebml-navi/ebml-navi.c
)
