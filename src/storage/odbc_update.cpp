#include "storage/odbc_update.hpp"
#include "storage/odbc_table_entry.hpp"
#include "duckdb/planner/operator/logical_update.hpp"
#include "storage/odbc_catalog.hpp"
#include "storage/odbc_transaction.hpp"
#include "odbc_db.hpp"
#include "odbc_stmt.hpp"

namespace duckdb {

OdbcUpdate::OdbcUpdate(LogicalOperator &op, TableCatalogEntry &table, vector<PhysicalIndex> columns_p)
    : PhysicalOperator(PhysicalOperatorType::EXTENSION, op.types, 1), table(table), columns(std::move(columns_p)) {
}

//===--------------------------------------------------------------------===//
// States
//===--------------------------------------------------------------------===//
class OdbcUpdateGlobalState : public GlobalSinkState {
public:
	explicit OdbcUpdateGlobalState(OdbcTableEntry &table) : table(table), update_count(0) {
	}

	OdbcTableEntry &table;
	OdbcStatement statement;
	idx_t update_count;
};

string GetUpdateSQL(OdbcTableEntry &table, const vector<PhysicalIndex> &index) {
	string result;
	result = "UPDATE " + KeywordHelper::WriteOptionallyQuoted(table.name);
	result += " SET ";
	for (idx_t i = 0; i < index.size(); i++) {
		if (i > 0) {
			result += ", ";
		}
		auto &col = table.GetColumn(LogicalIndex(index[i].index));
		result += KeywordHelper::WriteOptionallyQuoted(col.GetName());
		result += " = ?";
	}
	result += " WHERE rowid = ?";
	return result;
}

unique_ptr<GlobalSinkState> OdbcUpdate::GetGlobalSinkState(ClientContext &context) const {
	auto &sqlite_table = table.Cast<OdbcTableEntry>();

	auto &transaction = OdbcTransaction::Get(context, sqlite_table.catalog);
	auto result = make_uniq<OdbcUpdateGlobalState>(sqlite_table);
	result->statement = transaction.GetDB().Prepare(GetUpdateSQL(sqlite_table, columns));
	return std::move(result);
}

//===--------------------------------------------------------------------===//
// Sink
//===--------------------------------------------------------------------===//
SinkResultType OdbcUpdate::Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const {
	auto &gstate = input.global_state.Cast<OdbcUpdateGlobalState>();

	chunk.Flatten();
	auto &row_identifiers = chunk.data[chunk.ColumnCount() - 1];
	auto row_data = FlatVector::GetData<row_t>(row_identifiers);
	auto &stmt = gstate.statement;
	auto update_columns = chunk.ColumnCount() - 1;
	for (idx_t r = 0; r < chunk.size(); r++) {
		// bind the SET values
		for (idx_t c = 0; c < update_columns; c++) {
			auto &col = chunk.data[c];
			stmt.BindValue(col, c, r);
		}
		// bind the row identifier
		stmt.Bind<int64_t>(update_columns, row_data[r]);
		stmt.Step();
		stmt.Reset();
	}
	gstate.update_count += chunk.size();
	return SinkResultType::NEED_MORE_INPUT;
}

//===--------------------------------------------------------------------===//
// GetData
//===--------------------------------------------------------------------===//
SourceResultType OdbcUpdate::GetData(ExecutionContext &context, DataChunk &chunk, OperatorSourceInput &input) const {
	auto &insert_gstate = sink_state->Cast<OdbcUpdateGlobalState>();
	chunk.SetCardinality(1);
	chunk.SetValue(0, 0, Value::BIGINT(insert_gstate.update_count));

	return SourceResultType::FINISHED;
}

//===--------------------------------------------------------------------===//
// Helpers
//===--------------------------------------------------------------------===//
string OdbcUpdate::GetName() const {
	return "UPDATE";
}

string OdbcUpdate::ParamsToString() const {
	return table.name;
}

//===--------------------------------------------------------------------===//
// Plan
//===--------------------------------------------------------------------===//
unique_ptr<PhysicalOperator> OdbcCatalog::PlanUpdate(ClientContext &context, LogicalUpdate &op,
                                                       unique_ptr<PhysicalOperator> plan) {
	if (op.return_chunk) {
		throw BinderException("RETURNING clause not yet supported for updates of a Odbc table");
	}
	for (auto &expr : op.expressions) {
		if (expr->type == ExpressionType::VALUE_DEFAULT) {
			throw BinderException("SET DEFAULT is not yet supported for updates of a Odbc table");
		}
	}
	auto insert = make_uniq<OdbcUpdate>(op, op.table, std::move(op.columns));
	insert->children.push_back(std::move(plan));
	return std::move(insert);
}

} // namespace duckdb
