include_directories(include)

add_subdirectory(storage)

add_library(
  odbc_ext_library OBJECT
  odbc_db.cpp
  odbc_scanner_extension.cpp
  odbc_scan.cpp
  odbc_stmt.cpp
  odbc_storage.cpp
  odbc_utils.cpp
)
set(
  ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:odbc_ext_library>
  PARENT_SCOPE
)
