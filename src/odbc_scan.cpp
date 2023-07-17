#include "odbc_scan.hpp"

namespace duckdb {
static void OdbcScan(ClientContext &context, TableFunctionInput &data,
                     DataChunk &output) {
  auto &bind_data = data.bind_data->Cast<OdbcScanBindData>();
  auto local_state = data.local_state->Cast<OdbcScanLocalState>();

  auto rows_fetched = bind_data.statement->Fetch();
  if (rows_fetched == 0) {
    // finished returning values
    return;
  }

  for (auto r = 0; r < rows_fetched; r++) {
    auto row_status = local_state.row_status[r];
    if ((row_status == SQL_ROW_SUCCESS) ||
        (row_status == SQL_ROW_SUCCESS_WITH_INFO)) {
      for (auto c = 0; c < local_state.column_bindings.size(); c++) {
        auto column_binding = &local_state.column_bindings.at(c);
        auto buffer =
            &column_binding->buffer[r * column_binding->column_buffer_length];

        switch (column_binding->sql_data_type) {
        case SQL_SMALLINT:
          output.SetValue(c, local_state.offset,
                          Value(*(std::int16_t *)buffer));
          break;
        case SQL_INTEGER:
          output.SetValue(c, local_state.offset,
                          Value(*(std::int32_t *)buffer));
          break;
        case SQL_BIGINT:
          output.SetValue(c, local_state.offset,
                          Value(*(std::int64_t *)buffer));
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
          throw Exception(
              "OdbcScanFunction#OdbcScan() unhandled output "
              "mapping from ODBC to DuckDB sql_data_type=" +
              std::to_string(column_binding->sql_data_type) +
              ", c_data_type=" + std::to_string(column_binding->c_data_type));
        }
      }
    } else if (row_status == SQL_ROW_NOROW) {
      throw Exception("OdbcScanFunction#OdbcScan() row status=" +
                      std::to_string(row_status) + " SQL_ROW_NOROW");
    } else if (row_status == SQL_ROW_ERROR) {
      throw Exception("OdbcScanFunction#OdbcScan() row status=" +
                      std::to_string(row_status) + " SQL_ROW_ERROR");
    } else if (row_status == SQL_ROW_PROCEED) {
      throw Exception("OdbcScanFunction#OdbcScan() row status=" +
                      std::to_string(row_status) + " SQL_ROW_PROCEED");
    } else if (row_status == SQL_ROW_IGNORE) {
      throw Exception("OdbcScanFunction#OdbcScan() row status=" +
                      std::to_string(row_status) + " SQL_ROW_IGNORE");
    } else {
      throw Exception("OdbcScanFunction#OdbcScan() row status=" +
                      std::to_string(row_status) + " SQL_ROW_UNKNOWN");
    }

    // TODO:
    // - handle STANDARD_VECTOR_SIZE
    local_state.offset++;
    output.SetCardinality(local_state.offset);
  }
}

static unique_ptr<FunctionData> OdbcScanBind(ClientContext &context,
                                             TableFunctionBindInput &input,
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
  bind_data->statement_opts =
      make_uniq<OdbcStatementOptions>(STANDARD_VECTOR_SIZE);
  bind_data->statement->Execute(bind_data->statement_opts);

  names = bind_data->names;
  return_types = bind_data->types;

  return std::move(bind_data);
}

static unique_ptr<GlobalTableFunctionState>
OdbcScanInitGlobalState(ClientContext &context, TableFunctionInitInput &input) {
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
    bind_data.statement->BindColumn(
        c + 1, column_binding->c_data_type, column_binding->buffer,
        column_binding->column_buffer_length, column_binding->strlen_or_ind);
  }

  return std::move(local_state);
}

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
  projection_pushdown = true;
}

OdbcScanFunctionFilterPushdown::OdbcScanFunctionFilterPushdown()
    : TableFunction(
          "odbc_scan_pushdown",
          {LogicalType::VARCHAR, LogicalType::VARCHAR, LogicalType::VARCHAR},
          OdbcScan, OdbcScanBind, OdbcScanInitGlobalState,
          OdbcScanInitLocalState) {
  to_string = OdbcScanToString;
  projection_pushdown = true;
  filter_pushdown = true;
}
} // namespace duckdb
