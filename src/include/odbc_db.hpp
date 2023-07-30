#pragma once

#include "odbc_utils.hpp"

namespace duckdb {
class OdbcStatement;
struct IndexInfo;

class OdbcDB {
public:
	OdbcDB();
	// OdbcDB(sqlite3 *db);
	~OdbcDB();
	// disable copy constructors
	OdbcDB(const OdbcDB &other) = delete;
	OdbcDB &operator=(const OdbcDB &) = delete;
	//! enable move constructors
	OdbcDB(OdbcDB &&other) noexcept;
	OdbcDB &operator=(OdbcDB &&) noexcept;

	// sqlite3 *db;

public:
	static OdbcDB Open(const string &path, bool is_read_only = true, bool is_shared = false);
	bool TryPrepare(const string &query, OdbcStatement &result);
	OdbcStatement Prepare(const string &query);
	void Execute(const string &query);
	vector<string> GetTables();

	vector<string> GetEntries(string entry_type);
	CatalogType GetEntryType(const string &name);
	void GetTableInfo(const string &table_name, ColumnList &columns, vector<unique_ptr<Constraint>> &constraints,
	                  bool all_varchar);
	void GetViewInfo(const string &view_name, string &sql);
	void GetIndexInfo(const string &index_name, string &sql, string &table_name);
	idx_t RunPragma(string pragma_name);
	//! Gets the max row id of a table, returns false if the table does not have a rowid column
	bool GetMaxRowId(const string &table_name, idx_t &row_id);
	bool ColumnExists(const string &table_name, const string &column_name);
	vector<IndexInfo> GetIndexInfo(const string &table_name);

	bool IsOpen();
	void Close();
};

} // namespace duckdb
