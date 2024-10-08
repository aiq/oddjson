cmake_minimum_required( VERSION 3.2 )

project( oddjson VERSION 0.1.0 LANGUAGES C )
set( CMAKE_C_STANDARD          11 )
set( CMAKE_C_STANDARD_REQUIRED ON )
set( CMAKE_C_EXTENSIONS        OFF )

if( WIN32 )
    set( CMAKE_C_VISIBILITY_PRESET hidden )
    set( CMAKE_VISIBILITY_INLINES_HIDDEN ON )
endif()

enable_testing()

set( LIB_NAME ${PROJECT_NAME} )

# where to look first for modules, before ${CMAKE_ROOT}/Modules/ is checked
set( ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../.. )
set( APP_DIR ${ROOT}/app )
set( DOC_DIR ${ROOT}/doc )
set( INC_DIR ${ROOT}/inc )
set( SRC_DIR ${ROOT}/src )
set( TEST_DIR ${ROOT}/test )

include( CTest )
include( FetchContent )
include( ${CMAKE_CURRENT_SOURCE_DIR}/Custom.cmake )
include( ${CMAKE_CURRENT_SOURCE_DIR}/FileLists.cmake )

################################################################################
# fetch
################################################################################

fetchcontent_declare(
   clingo
   GIT_REPOSITORY "https://github.com/aiq/clingo"
   GIT_TAG "main"
   SOURCE_SUBDIR "mk/cmake"
)

fetchcontent_makeavailable( clingo )

################################################################################
# build - make
################################################################################
add_library( ${LIB_NAME} SHARED ${SRC_FILES} )
set_default_flags( ${LIB_NAME} )
target_include_directories( ${LIB_NAME} PUBLIC ${INC_DIR} PRIVATE ${SRC_DIR} )
if( WIN32 )
    target_link_libraries( ${LIB_NAME} clingo )
else()
    target_link_libraries( ${LIB_NAME} PUBLIC m clingo )
endif()

set( TEST_INC_DIRS ${INC_DIR} ${SRC_DIR} )
if( WIN32 )
    set( TEST_LINK_LIBS ${LIB_NAME} clingo )
else()
    set( TEST_LINK_LIBS ${LIB_NAME} m clingo )
endif()
add_tests( "${PROJECT_NAME}" "${TEST_FILES}" "${TEST_INC_DIRS}" "${TEST_LINK_LIBS}" )

set( APP_INC_DIRS ${INC_DIR} ${SRC_DIR} ${APP_DIR} )
if( WIN32 )
   set( APP_LINK_LIBS ${LIB_NAME} clingo )
else()
   set( APP_LINK_LIBS ${LIB_NAME} m clingo )
endif()
add_apps( "${APP_FILES}" "${APP_INC_DIRS}" "${APP_LINK_LIBS}" )
