#include "odbc_scan.hpp"

#include "duckdb.hpp"

#include "duckdb/function/table_function.hpp"

namespace duckdb {
static string OdbcScanToString(const FunctionData *bind_data_p) {
  D_ASSERT(bind_data_p);

  auto bind_data = (const OdbcScanBindData *)bind_data_p;
  return bind_data->table_name;
}

OdbcScanFunction::OdbcScanFunction()
    : TableFunction(
          "odbc_scan",
          {LogicalType::VARCHAR, LogicalType::VARCHAR, LogicalType::VARCHAR},
          OdbcScan, OdbcScanBind, OdbcScanInitGlobalState,
          OdbcScanInitLocalState) {
  to_string = OdbcScanToString;
  // projection_pushdown = true;
}
} // namespace duckdb
