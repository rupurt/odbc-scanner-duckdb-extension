#include "odbc_scan.hpp"

#include "duckdb.hpp"

#include "duckdb/function/table_function.hpp"

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

static void OdbcScan(ClientContext &context, TableFunctionInput &data, DataChunk &output) {
  auto &bind_data = data.bind_data->Cast<OdbcScanBindData>();
  auto local_state = data.local_state->Cast<OdbcScanLocalState>();

  auto rows_fetched = bind_data.statement->Fetch();
  if (rows_fetched == 0) {
    // finished returning values
    return;
  }

  for (auto r = 0; r < rows_fetched; r++) {
    auto row_status = local_state.row_status[r];
    if ((row_status == SQL_ROW_SUCCESS) || (row_status == SQL_ROW_SUCCESS_WITH_INFO)) {
      for (auto c = 0; c < local_state.column_bindings.size(); c++) {
        auto column_binding = &local_state.column_bindings.at(c);
        auto buffer = &column_binding->buffer[r * column_binding->column_buffer_length];

        switch (column_binding->sql_data_type) {
        case SQL_SMALLINT:
          output.SetValue(c, local_state.offset, Value(*(std::int16_t *)buffer));
          break;
        case SQL_INTEGER:
          output.SetValue(c, local_state.offset, Value(*(std::int32_t *)buffer));
          break;
        case SQL_BIGINT:
          output.SetValue(c, local_state.offset, Value(*(std::int64_t *)buffer));
          break;
        case SQL_DOUBLE:
        case SQL_FLOAT:
          output.SetValue(c, local_state.offset, Value(*(double *)buffer));
          break;
        case SQL_DECIMAL:
        case SQL_NUMERIC:
          output.SetValue(c, local_state.offset, Value((char *)buffer));
          break;
        case SQL_CHAR:
        // case SQL_CLOB:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR: {
          output.SetValue(c, local_state.offset, Value((char *)buffer));
          break;
        }
        case SQL_BINARY:
        // case SQL_BLOB:
        case SQL_VARBINARY:
        case SQL_LONGVARBINARY:
          output.SetValue(c, local_state.offset, Value((char *)buffer));
          break;
        default:
          throw Exception("OdbcScanFunction#OdbcScan() unhandled output "
                          "mapping from ODBC to DuckDB sql_data_type=" +
                          std::to_string(column_binding->sql_data_type) +
                          ", c_data_type=" + std::to_string(column_binding->c_data_type));
        }
      }
    } else if (row_status == SQL_ROW_NOROW) {
      throw Exception("OdbcScanFunction#OdbcScan() row status=" + std::to_string(row_status) +
                      " SQL_ROW_NOROW");
    } else if (row_status == SQL_ROW_ERROR) {
      throw Exception("OdbcScanFunction#OdbcScan() row status=" + std::to_string(row_status) +
                      " SQL_ROW_ERROR");
    } else if (row_status == SQL_ROW_PROCEED) {
      throw Exception("OdbcScanFunction#OdbcScan() row status=" + std::to_string(row_status) +
                      " SQL_ROW_PROCEED");
    } else if (row_status == SQL_ROW_IGNORE) {
      throw Exception("OdbcScanFunction#OdbcScan() row status=" + std::to_string(row_status) +
                      " SQL_ROW_IGNORE");
    } else {
      throw Exception("OdbcScanFunction#OdbcScan() row status=" + std::to_string(row_status) +
                      " SQL_ROW_UNKNOWN");
    }

    // TODO:
    // - handle STANDARD_VECTOR_SIZE
    local_state.offset++;
    output.SetCardinality(local_state.offset);
  }
}

static unique_ptr<FunctionData> OdbcScanBind(ClientContext &context, TableFunctionBindInput &input,
                                             vector<LogicalType> &return_types,
                                             vector<string> &names) {
  auto bind_data = make_uniq<OdbcScanBindData>();
  bind_data->connection_string = input.inputs[0].GetValue<string>();
  bind_data->schema_name = input.inputs[1].GetValue<string>();
  bind_data->table_name = input.inputs[2].GetValue<string>();

  bind_data->environment = make_shared<OdbcEnvironment>();
  bind_data->environment->Init();

  bind_data->connection = make_shared<OdbcConnection>();
  bind_data->connection->Init(bind_data->environment);
  bind_data->connection->Dial(bind_data->connection_string);

  bind_data->statement = make_uniq<OdbcStatement>(bind_data->connection);
  bind_data->statement->Init();

  string sql_statement = "SELECT * FROM ";
  if (bind_data->schema_name.compare(string(""))) {
    sql_statement += bind_data->schema_name + ".";
  }
  sql_statement += bind_data->table_name;
  bind_data->statement->Prepare(sql_statement);

  auto columns = bind_data->statement->DescribeColumns();
  for (int i = 0; i < columns.size(); i++) {
    auto duckdb_type = OdbcColumnToDuckDBLogicalType(columns[i]);
    bind_data->column_descriptions.push_back(columns[i]);
    bind_data->names.push_back(string((char *)columns[i].name));
    bind_data->types.push_back(duckdb_type);
  }

  // bind_data->statement_opts = make_uniq<OdbcStatementOptions>(1);
  // bind_data->statement_opts = make_uniq<OdbcStatementOptions>(2);
  // bind_data->statement_opts =
  // make_uniq<OdbcStatementOptions>(STANDARD_VECTOR_SIZE * 2);
  bind_data->statement_opts = make_uniq<OdbcStatementOptions>(STANDARD_VECTOR_SIZE);
  bind_data->statement->Execute(bind_data->statement_opts);

  names = bind_data->names;
  return_types = bind_data->types;

  return std::move(bind_data);
}

static unique_ptr<GlobalTableFunctionState> OdbcScanInitGlobalState(ClientContext &context,
                                                                    TableFunctionInitInput &input) {
  return make_uniq<OdbcScanGlobalState>();
}

static unique_ptr<LocalTableFunctionState>
OdbcScanInitLocalState(ExecutionContext &context, TableFunctionInitInput &input,
                       GlobalTableFunctionState *global_state) {
  auto &bind_data = input.bind_data->Cast<OdbcScanBindData>();
  auto row_array_size = bind_data.statement_opts->RowArraySize();
  auto local_state = make_uniq<OdbcScanLocalState>(row_array_size);

  bind_data.statement->SetAttribute(SQL_ATTR_ROW_STATUS_PTR,
                                    (SQLPOINTER)&local_state->row_status[0]);

  for (SQLSMALLINT c = 0; c < bind_data.column_descriptions.size(); c++) {
    auto col_desc = bind_data.column_descriptions.at(c);

    local_state->column_bindings.emplace_back(col_desc, row_array_size);
    auto column_binding = &local_state->column_bindings.at(c);
    bind_data.statement->BindColumn(c + 1, column_binding->c_data_type, column_binding->buffer,
                                    column_binding->column_buffer_length,
                                    column_binding->strlen_or_ind);
  }

  return std::move(local_state);
}

static string OdbcScanToString(const FunctionData *bind_data_p) {
  D_ASSERT(bind_data_p);

  auto bind_data = (const OdbcScanBindData *)bind_data_p;
  return bind_data->table_name;
}

OdbcScanFunction::OdbcScanFunction()
    : TableFunction("odbc_scan", {LogicalType::VARCHAR, LogicalType::VARCHAR, LogicalType::VARCHAR},
                    OdbcScan, OdbcScanBind, OdbcScanInitGlobalState, OdbcScanInitLocalState) {
  to_string = OdbcScanToString;
  // projection_pushdown = true;
}
} // namespace duckdb
