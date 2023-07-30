#include "duckdb.hpp"

// #include "sqlite3.h"
#include "duckdb/catalog/catalog_entry/schema_catalog_entry.hpp"
#include "duckdb/catalog/catalog_entry/table_catalog_entry.hpp"
#include "duckdb/parser/parsed_data/attach_info.hpp"
#include "duckdb/transaction/transaction_manager.hpp"
#include "odbc_storage.hpp"
#include "odbc_utils.hpp"
#include "storage/odbc_catalog.hpp"
#include "storage/odbc_transaction_manager.hpp"

#include <iostream>

namespace duckdb {

static unique_ptr<Catalog> OdbcAttach(StorageExtensionInfo *storage_info, AttachedDatabase &db,
                                      const string &name, AttachInfo &info, AccessMode access_mode) {
  std::cout << "---------------------" << std::endl;
  std::cout << "in OdbcStorage::OdbcAttach name=" << info.name << ", path=" << info.path << std::endl;
  return make_uniq<OdbcCatalog>(db, info.path, access_mode);
}

static unique_ptr<TransactionManager> OdbcCreateTransactionManager(StorageExtensionInfo *storage_info,
                                                                   AttachedDatabase &db, Catalog &catalog) {
  auto &odbc_catalog = catalog.Cast<OdbcCatalog>();
  return make_uniq<OdbcTransactionManager>(db, odbc_catalog);
}

OdbcStorageExtension::OdbcStorageExtension() {
  attach = OdbcAttach;
  create_transaction_manager = OdbcCreateTransactionManager;
}

} // namespace duckdb
