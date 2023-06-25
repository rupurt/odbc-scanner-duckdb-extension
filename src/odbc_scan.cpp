#include "odbc_scan.hpp"

#include "duckdb.hpp"

#include "duckdb/function/table_function.hpp"

namespace duckdb {
OdbcScanFunction::OdbcScanFunction()
    : TableFunction(
          "odbc_scan",
          {LogicalType::VARCHAR, LogicalType::VARCHAR, LogicalType::VARCHAR},
          OdbcScan, OdbcScanBind, OdbcScanInitGlobalState,
          OdbcScanInitLocalState) {
  // to_string = PostgresScanToString;
  // projection_pushdown = true;
}
} // namespace duckdb
