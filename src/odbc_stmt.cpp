#include "odbc_stmt.hpp"
#include "odbc_db.hpp"

namespace duckdb {
// TODO:
// - this was not a real constructor. Just here to get things to compile
OdbcStatement::OdbcStatement() {}

// OdbcStatement::OdbcStatement() : db(nullptr), stmt(nullptr) {
// }

// OdbcStatement::OdbcStatement(sqlite3 *db, sqlite3_stmt *stmt) : db(db), stmt(stmt) {
// 	D_ASSERT(db);
// }

OdbcStatement::~OdbcStatement() { Close(); }

OdbcStatement::OdbcStatement(OdbcStatement &&other) noexcept {
  // std::swap(db, other.db);
  // std::swap(stmt, other.stmt);
}

OdbcStatement &OdbcStatement::operator=(OdbcStatement &&other) noexcept {
  // std::swap(db, other.db);
  // std::swap(stmt, other.stmt);
  return *this;
}

int OdbcStatement::Step() {
  // D_ASSERT(db);
  // D_ASSERT(stmt);
  // auto rc = sqlite3_step(stmt);
  // if (rc == SQLITE_ROW) {
  // 	return true;
  // }
  // if (rc == SQLITE_DONE) {
  // 	return false;
  // }
  // throw std::runtime_error(string(sqlite3_errmsg(db)));

  return false;
}
int OdbcStatement::GetType(idx_t col) {
  // D_ASSERT(stmt);
  // return sqlite3_column_type(stmt, col);

  return false;
}

bool OdbcStatement::IsOpen() {
  // return stmt;

  return false;
}

void OdbcStatement::Close() {
  // if (!IsOpen()) {
  // 	return;
  // }
  // sqlite3_finalize(stmt);
  // db = nullptr;
  // stmt = nullptr;
}

// void OdbcStatement::CheckTypeMatches(sqlite3_value *val, int sqlite_column_type, int expected_type, idx_t
// col_idx) { 	D_ASSERT(stmt); 	if (sqlite_column_type != expected_type) { 		auto column_name
// = string(sqlite3_column_name(stmt, int(col_idx))); 		auto value_as_text = string((char
// *)sqlite3_value_text(val)); 		auto message = "Invalid type in column \"" + column_name + "\": column
// was
// declared as " + 		               OdbcUtils::TypeToString(expected_type) + ", found \"" +
// value_as_text + "\" of type \"" + 		               OdbcUtils::TypeToString(sqlite_column_type) + "\" instead."; 		throw
// Exception(ExceptionType::MISMATCH_TYPE, message);
// 	}
// }
//
// void OdbcStatement::CheckTypeIsFloatOrInteger(sqlite3_value *val, int sqlite_column_type, idx_t col_idx) {
// 	if (sqlite_column_type != SQLITE_FLOAT && sqlite_column_type != SQLITE_INTEGER) {
// 		auto column_name = string(sqlite3_column_name(stmt, int(col_idx)));
// 		auto value_as_text = string((const char *)sqlite3_value_text(val));
// 		auto message = "Invalid type in column \"" + column_name + "\": expected float or integer,
// found
// \"" + 		               value_as_text + "\" of type \"" +
// OdbcUtils::TypeToString(sqlite_column_type) + "\" instead."; 		throw
// Exception(ExceptionType::MISMATCH_TYPE, message);
// 	}
// }

void OdbcStatement::Reset() {
  // OdbcUtils::Check(sqlite3_reset(stmt), db);
}

template <> string OdbcStatement::GetValue(idx_t col) {
  // D_ASSERT(stmt);
  // auto ptr = sqlite3_column_text(stmt, col);
  // if (!ptr) {
  // 	return string();
  // }
  // return string((char *)ptr);

  return "todo";
}

template <> int OdbcStatement::GetValue(idx_t col) {
  // D_ASSERT(stmt);
  // return sqlite3_column_int(stmt, col);

  return false;
}

template <> int64_t OdbcStatement::GetValue(idx_t col) {
  // D_ASSERT(stmt);
  // return sqlite3_column_int64(stmt, col);

  return false;
}

// template <>
// sqlite3_value *OdbcStatement::GetValue(idx_t col) {
// 	// D_ASSERT(stmt);
// 	// return sqlite3_column_value(stmt, col);
//
//   return false;
// }

template <> void OdbcStatement::Bind(idx_t col, int32_t value) {
  // OdbcUtils::Check(sqlite3_bind_int(stmt, col + 1, value), db);
}

template <> void OdbcStatement::Bind(idx_t col, int64_t value) {
  // OdbcUtils::Check(sqlite3_bind_int64(stmt, col + 1, value), db);
}

template <> void OdbcStatement::Bind(idx_t col, double value) {
  // OdbcUtils::Check(sqlite3_bind_double(stmt, col + 1, value), db);
}

void OdbcStatement::BindText(idx_t col, const string_t &value) {
  // OdbcUtils::Check(sqlite3_bind_text(stmt, col + 1, value.GetDataUnsafe(), value.GetSize(), nullptr), db);
}

template <> void OdbcStatement::Bind(idx_t col, std::nullptr_t value) {
  // OdbcUtils::Check(sqlite3_bind_null(stmt, col + 1), db);
}

void OdbcStatement::BindValue(Vector &col, idx_t c, idx_t r) {
  // auto &mask = FlatVector::Validity(col);
  // if (!mask.RowIsValid(r)) {
  // 	Bind<nullptr_t>(c, nullptr);
  // } else {
  // 	switch (col.GetType().id()) {
  // 	case LogicalTypeId::BIGINT:
  // 		Bind<int64_t>(c, FlatVector::GetData<int64_t>(col)[r]);
  // 		break;
  // 	case LogicalTypeId::DOUBLE:
  // 		Bind<double>(c, FlatVector::GetData<double>(col)[r]);
  // 		break;
  // 	case LogicalTypeId::BLOB:
  // 	case LogicalTypeId::VARCHAR:
  // 		BindText(c, FlatVector::GetData<string_t>(col)[r]);
  // 		break;
  // 	default:
  // 		throw InternalException("Unsupported type \"%s\" for Odbc::BindValue", col.GetType());
  // 	}
  // }
}

} // namespace duckdb
