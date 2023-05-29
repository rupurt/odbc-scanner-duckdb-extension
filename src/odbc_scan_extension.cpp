#define DUCKDB_EXTENSION_MAIN

#include "odbc_scan_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"


#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

namespace duckdb {

inline void Odbc_scanScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
	    name_vector, result, args.size(),
	    [&](string_t name) { 
			return StringVector::AddString(result, "Odbc_scan "+name.GetString()+" 🐥");;
        });
}

static void LoadInternal(DatabaseInstance &instance) {
	Connection con(instance);
    con.BeginTransaction();

    auto &catalog = Catalog::GetSystemCatalog(*con.context);

    CreateScalarFunctionInfo odbc_scan_fun_info(
            ScalarFunction("odbc_scan", {LogicalType::VARCHAR}, LogicalType::VARCHAR, Odbc_scanScalarFun));
    odbc_scan_fun_info.on_conflict = OnCreateConflict::ALTER_ON_CONFLICT;
    catalog.CreateFunction(*con.context, &odbc_scan_fun_info);
    con.Commit();
}

void Odbc_scanExtension::Load(DuckDB &db) {
	LoadInternal(*db.instance);
}
std::string Odbc_scanExtension::Name() {
	return "odbc_scan";
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void odbc_scan_init(duckdb::DatabaseInstance &db) {
	LoadInternal(db);
}

DUCKDB_EXTENSION_API const char *odbc_scan_version() {
	return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
