add_library(
  odbc_ext_storage OBJECT
  odbc_catalog.cpp
  odbc_delete.cpp
  odbc_index.cpp
  odbc_index_entry.cpp
  odbc_insert.cpp
  odbc_table_entry.cpp
  odbc_schema_entry.cpp
  odbc_transaction.cpp
  odbc_transaction_manager.cpp
  odbc_update.cpp
)
set(
  ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:odbc_ext_storage>
  PARENT_SCOPE
)
