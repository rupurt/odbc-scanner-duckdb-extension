#pragma once

#include "duckdb.hpp"
// #include "sqlite3.h"

namespace duckdb {

class OdbcUtils {
public:
	// static void Check(int rc, sqlite3 *db);
	static string TypeToString(int sqlite_type);
	static LogicalType TypeToLogicalType(const string &sqlite_type);
	static string SanitizeString(const string &table_name);
	static string SanitizeIdentifier(const string &table_name);
	static LogicalType ToOdbcType(const LogicalType &input);
	string ToOdbcTypeAlias(const LogicalType &input);
};

} // namespace duckdb
