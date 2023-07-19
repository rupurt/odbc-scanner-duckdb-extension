#pragma once

#include "odbc.hpp"

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

struct OdbcColumnBinding {
  OdbcColumnBinding(OdbcColumnDescription col_desc, SQLINTEGER row_array_size) {
    column_buffer_length = col_desc.length;
    sql_data_type = col_desc.sql_data_type;
    c_data_type = col_desc.c_data_type;

    strlen_or_ind = new SQLLEN[row_array_size];
    memset(strlen_or_ind, 0, row_array_size);

    buffer = new unsigned char[row_array_size * column_buffer_length];
    memset(buffer, 0, row_array_size * column_buffer_length);
  }
  ~OdbcColumnBinding() {
    // TODO:
    // - why does freeing these cause a segfault?
    // - should I copy the values when setting the DuckDB output?
    // delete[] strlen_or_ind;
    // delete[] buffer;
  }

  SQLULEN column_buffer_length;
  SQLSMALLINT sql_data_type;
  SQLSMALLINT c_data_type;
  SQLLEN *strlen_or_ind;
  unsigned char *buffer;
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
} // namespace duckdb
