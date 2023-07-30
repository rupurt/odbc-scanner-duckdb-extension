#include "storage/odbc_table_entry.hpp"
#include "duckdb/storage/statistics/base_statistics.hpp"
#include "duckdb/storage/table_storage_info.hpp"
#include "odbc_scan.hpp"
#include "storage/odbc_catalog.hpp"
#include "storage/odbc_transaction.hpp"

namespace duckdb {

OdbcTableEntry::OdbcTableEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateTableInfo &info)
    : TableCatalogEntry(catalog, schema, info) {}

unique_ptr<BaseStatistics> OdbcTableEntry::GetStatistics(ClientContext &context, column_t column_id) {
  return nullptr;
}

void OdbcTableEntry::BindUpdateConstraints(LogicalGet &, LogicalProjection &, LogicalUpdate &,
                                           ClientContext &) {}

TableFunction OdbcTableEntry::GetScanFunction(ClientContext &context, unique_ptr<FunctionData> &bind_data) {
  // auto result = make_uniq<OdbcBindData>();
  // for (auto &col : columns.Logical()) {
  //   result->names.push_back(col.GetName());
  //   result->types.push_back(col.GetType());
  // }
  // auto &odbc_catalog = catalog.Cast<OdbcCatalog>();
  // result->file_name = odbc_catalog.path;
  // result->table_name = name;
  //
  // auto &transaction = Transaction::Get(context, catalog).Cast<OdbcTransaction>();
  // auto &db = transaction.GetDB();
  //
  // if (!db.GetMaxRowId(name, result->max_rowid)) {
  //   result->max_rowid = idx_t(-1);
  //   result->rows_per_group = idx_t(-1);
  // }
  // if (!transaction.IsReadOnly() || odbc_catalog.InMemory()) {
  //   // for in-memory databases or if we have transaction-local changes we can only do a single-threaded
  //   scan
  //       // set up the transaction's connection object as the global db
  //       result->global_db = &db;
  //   result->rows_per_group = idx_t(-1);
  // }
  //
  // bind_data = std::move(result);
  // return SqliteScanFunction();

  // ----------------------
  //

  // return OdbcScanFunction();

  // ----------------------
  //
  std::cout << "---------------------" << std::endl;
  std::cout << "in OdbcTableEntry::GetScanFunction" << std::endl;

  auto result = make_uniq<OdbcScanBindData>();
  for (auto &col : columns.Logical()) {
    result->names.push_back(col.GetName());
    result->types.push_back(col.GetType());
  }
  // auto &odbc_catalog = catalog.Cast<OdbcCatalog>();
  // result->file_name = odbc_catalog.path;
  // result->table_name = name;
  //
  // auto &transaction = Transaction::Get(context, catalog).Cast<OdbcTransaction>();
  // auto &db = transaction.GetDB();
  //
  // if (!db.GetMaxRowId(name, result->max_rowid)) {
  //   result->max_rowid = idx_t(-1);
  //   result->rows_per_group = idx_t(-1);
  // }
  // if (!transaction.IsReadOnly() || odbc_catalog.InMemory()) {
  //   // for in-memory databases or if we have transaction-local changes we can only do a single-threaded
  //   scan
  //   // set up the transaction's connection object as the global db
  //   result->global_db = &db;
  //   result->rows_per_group = idx_t(-1);
  // }

  bind_data = std::move(result);
  return OdbcScanFunction();
}

TableStorageInfo OdbcTableEntry::GetStorageInfo(ClientContext &context) {
  auto &transaction = Transaction::Get(context, catalog).Cast<OdbcTransaction>();
  auto &db = transaction.GetDB();
  TableStorageInfo result;
  if (!db.GetMaxRowId(name, result.cardinality)) {
    // probably
    result.cardinality = 10000;
  }
  result.index_info = db.GetIndexInfo(name);
  return result;
}

} // namespace duckdb
