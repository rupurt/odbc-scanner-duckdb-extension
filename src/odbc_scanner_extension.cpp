#define DUCKDB_EXTENSION_MAIN

#include "odbc_storage.hpp"
// #include "odbc_scanner_extension.hpp"
// #include "odbc_attach.hpp"
#include "odbc_scan.hpp"
#include "odbc_scanner_extension.hpp"

#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/function/table_function.hpp"
#include "duckdb/main/extension_util.hpp"

#include "duckdb/parser/parsed_data/create_scalar_function_info.hpp"
#include "duckdb/parser/parsed_data/create_table_function_info.hpp"

namespace duckdb {
static void LoadInternal(DatabaseInstance &db) {
  // table functions
  Connection con(db);
  con.BeginTransaction();
  auto &context = *con.context;
  auto &catalog = Catalog::GetSystemCatalog(context);

  OdbcScanFunction scan_fun;
  CreateTableFunctionInfo scan_info(scan_fun);
  catalog.CreateTableFunction(context, scan_info);

  // OdbcScanFunctionFilterPushdown scan_fun_filter_pushdown;
  // CreateTableFunctionInfo scan_filter_pushdown_info(scan_fun_filter_pushdown);
  // catalog.CreateTableFunction(context, scan_filter_pushdown_info);

  // TableFunction attach_fun("odbc_attach", {LogicalType::VARCHAR}, AttachFunction, AttachBind);
  // attach_fun.named_parameters["overwrite"] = LogicalType::BOOLEAN;
  // attach_fun.named_parameters["filter_pushdown"] = LogicalType::BOOLEAN;
  // attach_fun.named_parameters["source_schema"] = LogicalType::VARCHAR;
  // // attach_fun.named_parameters["sink_schema_prefix"] = LogicalType::VARCHAR;
  // attach_fun.named_parameters["sink_schema"] = LogicalType::VARCHAR;
  // attach_fun.named_parameters["suffix"] = LogicalType::VARCHAR;
  // CreateTableFunctionInfo attach_info(attach_fun);
  // catalog.CreateTableFunction(context, attach_info);

  auto &config = DBConfig::GetConfig(db);
	// config.AddExtensionOption("sqlite_all_varchar", "Load all SQLite columns as VARCHAR columns", LogicalType::BOOLEAN);

	config.storage_extensions["odbc_scanner"] = make_uniq<OdbcStorageExtension>();

  con.Commit();
}

void Odbc_scannerExtension::Load(DuckDB &db) { LoadInternal(*db.instance); }
std::string Odbc_scannerExtension::Name() { return "odbc_scanner"; }
} // namespace duckdb

extern "C" {
DUCKDB_EXTENSION_API void odbc_scanner_init(duckdb::DatabaseInstance &db) { LoadInternal(db); }

DUCKDB_EXTENSION_API const char *odbc_scanner_version() { return duckdb::DuckDB::LibraryVersion(); }
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
