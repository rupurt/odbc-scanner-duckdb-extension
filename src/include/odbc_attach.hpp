#pragma once

#include "odbc.hpp"
#include "odbc_mapping.hpp"

#include "duckdb.hpp"
// #include "duckdb/common/exception_format_value.hpp"
// #include "duckdb/function/table_function.hpp"

// #include "sql.h"
// #include "sqlext.h"
//
// #include <cstdint>
// #include <iostream>
#include <vector>

namespace duckdb {
struct AttachFunctionData : public TableFunctionData {
  AttachFunctionData() {}

  bool finished = false;
  string source_schema = "public";
  string sink_schema = "main";
  string suffix = "";
  bool overwrite = false;
  bool filter_pushdown = false;

  string connection_string = "";
};

static unique_ptr<FunctionData> AttachBind(ClientContext &context, TableFunctionBindInput &input,
                                           vector<LogicalType> &return_types,
                                           vector<string> &names) {

  auto result = make_uniq<AttachFunctionData>();
  result->connection_string = input.inputs[0].GetValue<string>();

  for (auto &kv : input.named_parameters) {
    if (kv.first == "source_schema") {
      result->source_schema = StringValue::Get(kv.second);
    } else if (kv.first == "sink_schema") {
      result->sink_schema = StringValue::Get(kv.second);
    } else if (kv.first == "overwrite") {
      result->overwrite = BooleanValue::Get(kv.second);
    } else if (kv.first == "filter_pushdown") {
      result->filter_pushdown = BooleanValue::Get(kv.second);
    }
  }

  return_types.push_back(LogicalType::BOOLEAN);
  names.emplace_back("Success");
  return std::move(result);
}

static void AttachFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
  auto &data = (AttachFunctionData &)*data_p.bind_data;
  if (data.finished) {
    return;
  }

  auto dconn = Connection(context.db->GetDatabase(context));

  auto environment = make_shared<OdbcEnvironment>();
  environment->Init();

  auto connection = make_shared<OdbcConnection>();
  connection->Init(environment);
  connection->Dial(data.connection_string);

  auto statement = make_uniq<OdbcStatement>(connection);
  statement->Init();

  auto opts = OdbcTableOptions{
      .catalog_name = SQL_ALL_CATALOGS, .schema_name = SQL_ALL_SCHEMAS, .table_name = "%"};
  statement->Tables(opts);

  statement->GetDescriptorField();
  auto columns = statement->DescribeColumns();
  // SQLTables doesn't support SQL_ATTR_ROW_ARRAY_SIZE > 1
  auto row_array_size = 1;

  vector<OdbcColumnBinding> column_bindings;
  for (auto c = 0; c < columns.size(); c++) {
    std::cout << columns[c].name << std::endl;

    auto col_desc = columns.at(c);

    column_bindings.emplace_back(col_desc, row_array_size);
    auto column_binding = &column_bindings.at(c);
    statement->BindColumn(c + 1, column_binding->c_data_type, column_binding->buffer,
                          column_binding->column_buffer_length, column_binding->strlen_or_ind);
  }

  vector<SQLUSMALLINT> row_status_buffer(row_array_size);
  statement->SetAttribute(SQL_ATTR_ROW_STATUS_PTR, (SQLPOINTER)&row_status_buffer[0]);

  idx_t offset = 0;
  auto fetch_next_rows = true;
  do {
    auto rows_fetched = statement->Fetch();
    fetch_next_rows = rows_fetched > 0;
    std::cout << "rows fetched=" << rows_fetched << std::endl;
    for (auto r = 0; r < rows_fetched; r++) {
      std::string schema = "";
      std::string table_name = "";
      auto row_status = row_status_buffer[r];

      if ((row_status == SQL_ROW_SUCCESS) || (row_status == SQL_ROW_SUCCESS_WITH_INFO)) {
        std::cout << "===================== row=" << r << std::endl;

        for (auto c = 0; c < columns.size(); c++) {
          auto column_binding = &column_bindings.at(c);
          auto buffer = &column_binding->buffer[r * column_binding->column_buffer_length];

          std::cout << "column idx=" << std::to_string(c) << std::endl;
          std::cout << "column_buffer_length=" << column_binding->column_buffer_length << std::endl;
          std::cout << "column_buffer value=" << buffer << std::endl;
          // std::cout << "column_buffer SQL_NULL_DATA="
          //           << std::to_string((int)&buffer == SQL_NULL_DATA) <<
          //           std::endl;
          // std::cout << "column_binding sql_data_type="
          //           << column_binding->sql_data_type << std::endl;
          // std::cout << "column_binding strlen_or_index="
          //           << std::to_string(column_binding->strlen_or_ind[c])
          //           << std::endl;
          std::cout << "..." << std::endl;

          if (c == 1) {
            schema = std::string((char *)buffer);
          } else if (c == 2) {
            table_name = std::string((char *)buffer);
          }
        }
      } else if (row_status == SQL_ROW_NOROW) {
        throw Exception("OdbcAttach() row status=" + std::to_string(row_status) + " SQL_ROW_NOROW");
      } else if (row_status == SQL_ROW_ERROR) {
        throw Exception("OdbcAttach() row status=" + std::to_string(row_status) + " SQL_ROW_ERROR");
      } else if (row_status == SQL_ROW_PROCEED) {
        throw Exception("OdbcAttach() row status=" + std::to_string(row_status) +
                        " SQL_ROW_PROCEED");
      } else if (row_status == SQL_ROW_IGNORE) {
        throw Exception("OdbcAttach() row status=" + std::to_string(row_status) +
                        " SQL_ROW_IGNORE");
      } else {
        throw Exception("OdbcAttach() row status=" + std::to_string(row_status) +
                        " SQL_ROW_UNKNOWN");
      }

      std::cout << "schema=" << schema << std::endl;
      std::cout << "table_name=" << table_name << std::endl;
      std::cout << "=====================" << std::endl;

      dconn
          .TableFunction(data.filter_pushdown ? "odbc_scan_pushdown" : "odbc_scan",
                         {Value(data.connection_string),
                          // Value(data.source_schema), Value(table_name)})
                          Value(schema), Value(table_name)})
          ->CreateView(data.sink_schema, table_name, data.overwrite, false);
    }
  } while (fetch_next_rows);

  data.finished = true;
}
} // namespace duckdb
