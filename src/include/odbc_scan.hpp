#pragma once

#include "duckdb.hpp"

#include "duckdb/function/table_function.hpp"

namespace duckdb {
struct OdbcTypeInfo {
  string typname;
  int64_t typlen;
  string typtype;
  string nspname;
};

struct OdbcColumnInfo {
  string attname;
  OdbcTypeInfo type_info;
};

struct OdbcScanBindData : public FunctionData {
  vector<OdbcColumnInfo> columns;
  vector<string> names;
  vector<LogicalType> types;

public:
  unique_ptr<FunctionData> Copy() const override {
    throw NotImplementedException("");
  }
  bool Equals(const FunctionData &other) const override {
    throw NotImplementedException("");
  }
};

struct OdbcScanLocalState : public LocalTableFunctionState {
  OdbcScanLocalState() : offset(0) {}

  idx_t offset;
};

struct OdbcScanGlobalState : public GlobalTableFunctionState {
  OdbcScanGlobalState(idx_t max_threads) : max_threads(max_threads) {}

  idx_t max_threads;

  idx_t MaxThreads() const override { return max_threads; }
};

static idx_t PostgresMaxThreads(ClientContext &context,
                                const FunctionData *bind_data_p) {
  D_ASSERT(bind_data_p);

  auto bind_data = (const OdbcScanBindData *)bind_data_p;
  // return bind_data->pages_approx / bind_data->pages_per_task;
  return 1;
}

class OdbcScanFunction : public TableFunction {
public:
  OdbcScanFunction();

  static void OdbcScan(ClientContext &context, TableFunctionInput &data,
                       DataChunk &output) {
    auto &local_state = data.local_state->Cast<OdbcScanLocalState>();

    if (local_state.offset >= 1) {
      // finished returning values
      return;
    }

    // TODO:
    // - handle STANDARD_VECTOR_SIZE
    local_state.offset++;
    idx_t col_idx = 0;
    idx_t vector_idx = 0;
    output.SetValue(col_idx, vector_idx, Value("Lebron James"));
    vector_idx++;
    output.SetValue(col_idx, vector_idx, Value("Spiderman"));
    vector_idx++;
    output.SetValue(col_idx, vector_idx, Value("Wonder Woman"));
    vector_idx++;
    output.SetValue(col_idx, vector_idx, Value("David Bowie"));
    vector_idx++;

    output.SetCardinality(vector_idx);
  }

  static unique_ptr<FunctionData>
  OdbcScanBind(ClientContext &context, TableFunctionBindInput &input,
               vector<LogicalType> &return_types, vector<string> &names) {
    auto bind_data = make_uniq<OdbcScanBindData>();

    OdbcColumnInfo info;
    info.attname = "name";

    bind_data->names.push_back(info.attname);
    bind_data->types.push_back(LogicalType::VARCHAR);
    bind_data->columns.push_back(info);

    return_types = bind_data->types;
    names = bind_data->names;

    return std::move(bind_data);
  }

  static unique_ptr<GlobalTableFunctionState>
  OdbcScanInitGlobalState(ClientContext &context,
                          TableFunctionInitInput &input) {
    return make_uniq<OdbcScanGlobalState>(
        PostgresMaxThreads(context, input.bind_data.get()));
  }

  static unique_ptr<LocalTableFunctionState>
  OdbcScanInitLocalState(ExecutionContext &context,
                         TableFunctionInitInput &input,
                         GlobalTableFunctionState *global_state) {
    auto local_state = make_uniq<OdbcScanLocalState>();
    return std::move(local_state);
  }
};
} // namespace duckdb
