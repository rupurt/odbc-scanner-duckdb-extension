#pragma once

#include "odbc.hpp"
#include "odbc_mapping.hpp"

#include "duckdb.hpp"
#include "duckdb/common/exception_format_value.hpp"
#include "duckdb/function/table_function.hpp"

#include "sql.h"
#include "sqlext.h"

#include <cstdint>
#include <iostream>
#include <vector>

namespace duckdb {
struct OdbcScanBindData : public FunctionData {
  string connection_string;
  string schema_name;
  string table_name;
  shared_ptr<OdbcEnvironment> environment;
  shared_ptr<OdbcConnection> connection;
  unique_ptr<OdbcStatement> statement;
  unique_ptr<OdbcStatementOptions> statement_opts;

  vector<string> names;
  vector<LogicalType> types;
  vector<OdbcColumnDescription> column_descriptions;

public:
  unique_ptr<FunctionData> Copy() const override { throw NotImplementedException(""); }
  bool Equals(const FunctionData &other) const override { throw NotImplementedException(""); }
};

struct OdbcScanLocalState : public LocalTableFunctionState {
  OdbcScanLocalState(SQLINTEGER _row_array_size)
      : offset(0), row_status(vector<SQLUSMALLINT>(_row_array_size)) {}

  idx_t offset;
  vector<SQLUSMALLINT> row_status;
  vector<OdbcColumnBinding> column_bindings;
};

struct OdbcScanGlobalState : public GlobalTableFunctionState {
  OdbcScanGlobalState() {}
};

class OdbcScanFunction : public TableFunction {
public:
  OdbcScanFunction();
};

class OdbcScanFunctionFilterPushdown : public TableFunction {
public:
  OdbcScanFunctionFilterPushdown();
};
} // namespace duckdb
