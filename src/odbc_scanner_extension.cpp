#define DUCKDB_EXTENSION_MAIN

#include "odbc_scanner_extension.hpp"
#include "odbc_scan.hpp"
#include "odbc_write.hpp"

#include "duckdb.hpp"

#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/function/table_function.hpp"
#include "duckdb/main/extension_util.hpp"

#include "duckdb/parser/parsed_data/create_table_function_info.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

namespace duckdb {
static void LoadInternal(DatabaseInstance &instance) {
  // scalar functions
  auto odbc_write_scalar_function =
      ScalarFunction("odbc_write", {LogicalType::VARCHAR}, LogicalType::VARCHAR,
                     OdbcWriteScalarFun);
  ExtensionUtil::RegisterFunction(instance, odbc_write_scalar_function);

  // table functions
  Connection con(instance);
  con.BeginTransaction();
  auto &context = *con.context;
  auto &catalog = Catalog::GetSystemCatalog(context);

  OdbcScanFunction odbc_scan_fun;
  CreateTableFunctionInfo odbc_scan_info(odbc_scan_fun);
  catalog.CreateTableFunction(context, odbc_scan_info);

  con.Commit();
}

void Odbc_scannerExtension::Load(DuckDB &db) { LoadInternal(*db.instance); }
std::string Odbc_scannerExtension::Name() { return "odbc_scanner"; }
} // namespace duckdb

extern "C" {
DUCKDB_EXTENSION_API void odbc_scanner_init(duckdb::DatabaseInstance &db) {
  LoadInternal(db);
}

DUCKDB_EXTENSION_API const char *odbc_scanner_version() {
  return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
