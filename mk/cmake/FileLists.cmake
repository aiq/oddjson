
################################################################################
# SRC_FILES
################################################################################
set( SRC_FILES )
aux_source_directory( ${SRC_DIR}/oddjson SRC_FILES )

################################################################################
# TEST_FILES
################################################################################
set( TEST_FILES )
aux_source_directory( ${TEST_DIR}/oddjson/OJson TEST_FILES )
aux_source_directory( ${TEST_DIR}/oddjson/oJsonBuilder TEST_FILES )
aux_source_directory( ${TEST_DIR}/oddjson/oJsonParser TEST_FILES )
aux_source_directory( ${TEST_DIR}/oddjson/oJsonRoute TEST_FILES )
aux_source_directory( ${TEST_DIR}/oddjson/oJsonString TEST_FILES )

################################################################################
# APP_FILES
################################################################################
set( APP_FILES
#   ${APP_DIR}/ebml-navi/ebml-navi.c
)
