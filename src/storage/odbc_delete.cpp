#include "storage/odbc_delete.hpp"
#include "storage/odbc_table_entry.hpp"
#include "duckdb/planner/operator/logical_delete.hpp"
#include "storage/odbc_catalog.hpp"
#include "storage/odbc_transaction.hpp"
#include "odbc_db.hpp"
#include "odbc_stmt.hpp"

namespace duckdb {

OdbcDelete::OdbcDelete(LogicalOperator &op, TableCatalogEntry &table)
    : PhysicalOperator(PhysicalOperatorType::EXTENSION, op.types, 1), table(table) {
}

//===--------------------------------------------------------------------===//
// States
//===--------------------------------------------------------------------===//
class OdbcDeleteGlobalState : public GlobalSinkState {
public:
	explicit OdbcDeleteGlobalState(OdbcTableEntry &table) : table(table), delete_count(0) {
	}

	OdbcTableEntry &table;
	OdbcStatement statement;
	idx_t delete_count;
};

string GetDeleteSQL(const string &table_name) {
	string result;
	result = "DELETE FROM " + KeywordHelper::WriteOptionallyQuoted(table_name);
	result += " WHERE rowid = ?";
	return result;
}

unique_ptr<GlobalSinkState> OdbcDelete::GetGlobalSinkState(ClientContext &context) const {
	auto &sqlite_table = table.Cast<OdbcTableEntry>();

	auto &transaction = OdbcTransaction::Get(context, sqlite_table.catalog);
	auto result = make_uniq<OdbcDeleteGlobalState>(sqlite_table);
	result->statement = transaction.GetDB().Prepare(GetDeleteSQL(sqlite_table.name));
	return std::move(result);
}

//===--------------------------------------------------------------------===//
// Sink
//===--------------------------------------------------------------------===//
SinkResultType OdbcDelete::Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const {
	auto &gstate = input.global_state.Cast<OdbcDeleteGlobalState>();

	chunk.Flatten();
	auto &row_identifiers = chunk.data[0];
	auto row_data = FlatVector::GetData<row_t>(row_identifiers);
	for (idx_t i = 0; i < chunk.size(); i++) {
		gstate.statement.Bind<int64_t>(0, row_data[i]);
		gstate.statement.Step();
		gstate.statement.Reset();
	}
	gstate.delete_count += chunk.size();
	return SinkResultType::NEED_MORE_INPUT;
}

//===--------------------------------------------------------------------===//
// GetData
//===--------------------------------------------------------------------===//
SourceResultType OdbcDelete::GetData(ExecutionContext &context, DataChunk &chunk, OperatorSourceInput &input) const {
	auto &insert_gstate = sink_state->Cast<OdbcDeleteGlobalState>();
	chunk.SetCardinality(1);
	chunk.SetValue(0, 0, Value::BIGINT(insert_gstate.delete_count));

	return SourceResultType::FINISHED;
}

//===--------------------------------------------------------------------===//
// Helpers
//===--------------------------------------------------------------------===//
string OdbcDelete::GetName() const {
	return "DELETE";
}

string OdbcDelete::ParamsToString() const {
	return table.name;
}

//===--------------------------------------------------------------------===//
// Plan
//===--------------------------------------------------------------------===//
unique_ptr<PhysicalOperator> OdbcCatalog::PlanDelete(ClientContext &context, LogicalDelete &op,
                                                       unique_ptr<PhysicalOperator> plan) {
	if (op.return_chunk) {
		throw BinderException("RETURNING clause not yet supported for deletion of a Odbc table");
	}
	auto insert = make_uniq<OdbcDelete>(op, op.table);
	insert->children.push_back(std::move(plan));
	return std::move(insert);
}

} // namespace duckdb
