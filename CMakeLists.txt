cmake_minimum_required(VERSION 2.8.12)
set(TARGET_NAME odbc_scanner)
set(EXTENSION_NAME ${TARGET_NAME}_extension)
project(${TARGET_NAME})

# include_directories(src/include)
#
# set(
#   EXTENSION_SOURCES
#   src/odbc_scan.cpp
#   src/odbc_scanner_extension.cpp
# )
# add_library(${EXTENSION_NAME} STATIC ${EXTENSION_SOURCES})
add_subdirectory(src)
set(EXTENSION_OBJECT_FILES ${ALL_OBJECT_FILES})
add_library(${EXTENSION_NAME} STATIC ${EXTENSION_OBJECT_FILES})

set(PARAMETERS "-warnings")
# build_loadable_extension(${TARGET_NAME} ${PARAMETERS} ${EXTENSION_SOURCES})
build_loadable_extension(${TARGET_NAME} ${PARAMETERS} ${EXTENSION_OBJECT_FILES})

# nix store location of ODBC_CONFIG is passed in as CLIENT_FLAGS to root level Makefile
# set(ODBC_CONFIG /nix/store/xs4bg5404nsjarivdzxszq0z0pn2ckzv-unixODBC-2.3.11/bin/odbc_config)
find_package(ODBC REQUIRED ODBC_INCLUDE_DIR ODBC_LIBRARY)
if(NOT ODBC_FOUND)
  message(FATAL_ERROR "No ODBC found")
endif()
# Link ODBC to static lib
target_include_directories(${EXTENSION_NAME} PUBLIC $<BUILD_INTERFACE:${ODBC_INCLUDE_DIR}>)
target_link_libraries(${EXTENSION_NAME} ${ODBC_LIBRARIES})
# Link ODBC to loadable binary
target_include_directories(${TARGET_NAME}_loadable_extension PRIVATE $<BUILD_INTERFACE:${ODBC_INCLUDE_DIR}>)
target_link_libraries(${TARGET_NAME}_loadable_extension ${ODBC_LIBRARIES})

install(
  TARGETS ${EXTENSION_NAME}
  EXPORT "${DUCKDB_EXPORT_SET}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")
