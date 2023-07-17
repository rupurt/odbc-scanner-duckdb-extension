#pragma once

#include "odbc.hpp"

#include "duckdb.hpp"
// #include "duckdb/common/exception_format_value.hpp"
// #include "duckdb/function/table_function.hpp"

#include "sql.h"
#include "sqlext.h"

// #include <cstdint>
// #include <iostream>
// #include <vector>

namespace duckdb {
static LogicalType OdbcColumnToDuckDBLogicalType(OdbcColumnDescription col_desc) {
  if (col_desc.sql_data_type == SQL_CHAR) {
    return LogicalType::VARCHAR;
  }
  if (col_desc.sql_data_type == SQL_VARCHAR) {
    return LogicalType::VARCHAR;
  }
  if (col_desc.sql_data_type == SQL_LONGVARCHAR) {
    return LogicalType::VARCHAR;
  }
  // // TODO:
  // // - how should unicode variable length character strings be handled?
  // // - VARCHAR_COLLATION(...)?
  // if (col_desc.data_type == SQL_WCHAR) {
  //   return LogicalType::VARCHAR;
  // }
  // if (col_desc.data_type == SQL_WVARCHAR) {
  //   return LogicalType::VARCHAR;
  // }
  // if (col_desc.data_type == SQL_WLONGVARCHAR) {
  //   return LogicalType::VARCHAR;
  // }
  if (col_desc.sql_data_type == SQL_DECIMAL) {
    return LogicalType::DECIMAL(col_desc.size, col_desc.decimal_digits);
  }
  if (col_desc.sql_data_type == SQL_NUMERIC) {
    return LogicalType::DECIMAL(col_desc.size, col_desc.decimal_digits);
  }
  if (col_desc.sql_data_type == SQL_SMALLINT) {
    return LogicalType::SMALLINT;
  }
  if (col_desc.sql_data_type == SQL_INTEGER) {
    return LogicalType::INTEGER;
  }
  if (col_desc.sql_data_type == SQL_REAL) {
    return LogicalType::FLOAT;
  }
  if (col_desc.sql_data_type == SQL_FLOAT) {
    return LogicalType::FLOAT;
  }
  if (col_desc.sql_data_type == SQL_DOUBLE) {
    return LogicalType::DOUBLE;
  }
  if (col_desc.sql_data_type == SQL_BIT) {
    return LogicalType::BIT;
  }
  if (col_desc.sql_data_type == SQL_TINYINT) {
    return LogicalType::TINYINT;
  }
  if (col_desc.sql_data_type == SQL_BIGINT) {
    return LogicalType::BIGINT;
  }
  if (col_desc.sql_data_type == SQL_BINARY) {
    // DuckDB doesn't support FIXED_SIZE_BINARY yet
    // https://github.com/duckdb/duckdb/blob/master/src/include/duckdb/common/types.hpp#L125
    return LogicalType::BLOB;
  }
  if (col_desc.sql_data_type == SQL_VARBINARY) {
    // DuckDB doesn't support variable length BINARY yet
    // https://github.com/duckdb/duckdb/blob/master/src/include/duckdb/common/types.hpp#L122
    return LogicalType::BLOB;
  }
  if (col_desc.sql_data_type == SQL_LONGVARBINARY) {
    // DuckDB doesn't support variable length BINARY yet
    // https://github.com/duckdb/duckdb/blob/master/src/include/duckdb/common/types.hpp#L122
    return LogicalType::BLOB;
  }
  if (col_desc.sql_data_type == SQL_TYPE_DATE) {
    return LogicalType::DATE;
  }
  if (col_desc.sql_data_type == SQL_TYPE_TIME) {
    return LogicalType::TIME;
  }
  if (col_desc.sql_data_type == SQL_TYPE_TIMESTAMP) {
    return LogicalType::TIMESTAMP;
  }
  // TODO:
  // - handle the following remaining date/time/interval types
  // SQL_TYPE_UTCDATETIME	UTCDATETIME	Year, month, day, hour, minute,
  // second, utchour, and utcminute fields. The utchour and utcminute fields
  // have 1/10 microsecond precision. SQL_TYPE_UTCTIME	UTCTIME	Hour, minute,
  // second, utchour, and utcminute fields. The utchour and utcminute fields
  // have 1/10 microsecond precision..
  // SQL_INTERVAL_MONTH[7]	INTERVAL MONTH(p)	Number of months between
  // two dates; p is the interval leading precision. SQL_INTERVAL_YEAR[7]
  // INTERVAL YEAR(p)	Number of years between two dates; p is the interval
  // leading precision. SQL_INTERVAL_YEAR_TO_MONTH[7]	INTERVAL YEAR(p) TO
  // MONTH Number of years and months between two dates; p is the interval
  // leading precision. SQL_INTERVAL_DAY[7]	INTERVAL DAY(p)	Number of days
  // between two dates; p is the interval leading precision.
  // SQL_INTERVAL_HOUR[7] INTERVAL HOUR(p)	Number of hours between two
  // date/times; p is the interval leading precision. SQL_INTERVAL_MINUTE[7]
  // INTERVAL MINUTE(p) Number of minutes between two date/times; p is the
  // interval leading precision. SQL_INTERVAL_SECOND[7]	INTERVAL SECOND(p,q)
  // Number of seconds between two date/times; p is the interval leading
  // precision and q is the interval seconds precision.
  // SQL_INTERVAL_DAY_TO_HOUR[7] INTERVAL DAY(p) TO HOUR	Number of
  // days/hours between two date/times; p is the interval leading precision.
  // SQL_INTERVAL_DAY_TO_MINUTE[7]	INTERVAL DAY(p) TO MINUTE	Number
  // of days/hours/minutes between two date/times; p is the interval leading
  // precision. SQL_INTERVAL_DAY_TO_SECOND[7]	INTERVAL DAY(p) TO SECOND(q)
  // Number of days/hours/minutes/seconds between two date/times; p is the
  // interval leading precision and q is the interval seconds precision.
  // SQL_INTERVAL_HOUR_TO_MINUTE[7]	INTERVAL HOUR(p) TO MINUTE Number of
  // hours/minutes between two date/times; p is the interval leading precision.
  // SQL_INTERVAL_HOUR_TO_SECOND[7]	INTERVAL HOUR(p) TO SECOND(q) Number of
  // hours/minutes/seconds between two date/times; p is the interval leading
  // precision and q is the interval seconds precision.
  // SQL_INTERVAL_MINUTE_TO_SECOND[7]	INTERVAL MINUTE(p) TO SECOND(q)	Number
  // of minutes/seconds between two date/times; p is the interval leading
  // precision and q is the interval seconds precision.
  if (col_desc.sql_data_type == SQL_GUID) {
    return LogicalType::UUID;
  }

  return LogicalType::INVALID;
}

struct OdbcColumnBinding {
  OdbcColumnBinding(OdbcColumnDescription odbc_column_description, SQLINTEGER row_array_size) {
    column_buffer_length = odbc_column_description.length;
    sql_data_type = odbc_column_description.sql_data_type;
    c_data_type = odbc_column_description.c_data_type;

    buffer = new unsigned char[row_array_size * column_buffer_length];
    memset(buffer, 0, row_array_size * column_buffer_length);

    strlen_or_ind = new SQLLEN[row_array_size];
    memset(strlen_or_ind, 0, row_array_size);
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
  unsigned char *buffer;
  SQLLEN *strlen_or_ind;
};
} // namespace duckdb
