#pragma once

#include "odbc_utils.hpp"

#include <cstddef>

namespace duckdb {

class OdbcStatement {
public:
	OdbcStatement();
	// OdbcStatement(sqlite3 *db, sqlite3_stmt *stmt);
	~OdbcStatement();
	// disable copy constructors
	OdbcStatement(const OdbcStatement &other) = delete;
	OdbcStatement &operator=(const OdbcStatement &) = delete;
	//! enable move constructors
	OdbcStatement(OdbcStatement &&other) noexcept;
	OdbcStatement &operator=(OdbcStatement &&) noexcept;

	// sqlite3 *db;
	// sqlite3_stmt *stmt;

public:
	int Step();
	template <class T>
	T GetValue(idx_t col) {
		throw InternalException("Unsupported type for OdbcStatement::GetValue");
	}
	template <class T>
	void Bind(idx_t col, T value) {
		throw InternalException("Unsupported type for OdbcStatement::Bind");
	}
	void BindText(idx_t col, const string_t &value);
	void BindValue(Vector &col, idx_t c, idx_t r);
	int GetType(idx_t col);
	bool IsOpen();
	void Close();
	// void CheckTypeMatches(sqlite3_value *val, int sqlite_column_type, int expected_type, idx_t col_idx);
	// void CheckTypeIsFloatOrInteger(sqlite3_value *val, int sqlite_column_type, idx_t col_idx);
	void Reset();
};

template <>
string OdbcStatement::GetValue(idx_t col);
template <>
int OdbcStatement::GetValue(idx_t col);
template <>
int64_t OdbcStatement::GetValue(idx_t col);
// template <>
// sqlite3_value *OdbcStatement::GetValue(idx_t col);

template <>
void OdbcStatement::Bind(idx_t col, int32_t value);
template <>
void OdbcStatement::Bind(idx_t col, int64_t value);
template <>
void OdbcStatement::Bind(idx_t col, double value);
template <>
void OdbcStatement::Bind(idx_t col, std::nullptr_t value);

} // namespace duckdb
