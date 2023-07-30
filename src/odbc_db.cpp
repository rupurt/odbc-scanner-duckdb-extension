#include "odbc_db.hpp"
#include "duckdb/parser/column_list.hpp"
#include "duckdb/parser/constraints/not_null_constraint.hpp"
#include "duckdb/parser/constraints/unique_constraint.hpp"
#include "duckdb/parser/expression/constant_expression.hpp"
#include "duckdb/parser/parser.hpp"
#include "duckdb/storage/table_storage_info.hpp"
#include "odbc_stmt.hpp"

#include <iostream>

namespace duckdb {
// TODO:
// - need to implement the constructor below. This is a hack to get it compiling...
OdbcDB::OdbcDB() {}

// OdbcDB::OdbcDB() : db(nullptr) {
// }

// OdbcDB::OdbcDB(sqlite3 *db) : db(db) {
// }

OdbcDB::~OdbcDB() { Close(); }

OdbcDB::OdbcDB(OdbcDB &&other) noexcept {
  // std::swap(db, other.db);
}

OdbcDB &OdbcDB::operator=(OdbcDB &&other) noexcept {
  // std::swap(db, other.db);
  return *this;
}

OdbcDB OdbcDB::Open(const string &path, bool is_read_only, bool is_shared) {
  std::cout << "---------------------" << std::endl;
  std::cout << "in OdbcDB::Open" << std::endl;

  OdbcDB result;
  // int flags = SQLITE_OPEN_PRIVATECACHE;
  // if (is_read_only) {
  // 	flags |= SQLITE_OPEN_READONLY;
  // } else {
  // 	flags |= SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  // }
  // if (!is_shared) {
  // 	// FIXME: we should just make sure we are not re-using the same `sqlite3` object across threads
  // 	flags |= SQLITE_OPEN_NOMUTEX;
  // }
  // flags |= SQLITE_OPEN_EXRESCODE;
  // auto rc = sqlite3_open_v2(path.c_str(), &result.db, flags, nullptr);
  // if (rc != SQLITE_OK) {
  // 	throw std::runtime_error("Unable to open database \"" + path + "\": " + string(sqlite3_errstr(rc)));
  // }
  return result;
}

bool OdbcDB::TryPrepare(const string &query, OdbcStatement &stmt) {
  std::cout << "---------------------" << std::endl;
  std::cout << "in OdbcDB::TryPrepare" << query << std::endl;

  // stmt.db = db;
  // auto rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt.stmt, nullptr);
  // if (rc != SQLITE_OK) {
  // 	return false;
  // }
  return true;
}

OdbcStatement OdbcDB::Prepare(const string &query) {
  std::cout << "---------------------" << std::endl;
  std::cout << "in OdbcDB::Prepare" << query << std::endl;

  OdbcStatement stmt;
  // if (!TryPrepare(query, stmt)) {
  // 	string error = "Failed to prepare query \"" + query + "\": " + string(sqlite3_errmsg(db));
  // 	throw std::runtime_error(error);
  // }
  return stmt;
}

void OdbcDB::Execute(const string &query) {
  std::cout << "---------------------" << std::endl;
  std::cout << "in OdbcDB::Execute" << query << std::endl;

  // auto rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);
  // if (rc != SQLITE_OK) {
  // 	string error = "Failed to execute query \"" + query + "\": " + string(sqlite3_errmsg(db));
  // 	throw std::runtime_error(error);
  // }
}

bool OdbcDB::IsOpen() {
  std::cout << "---------------------" << std::endl;
  std::cout << "in OdbcDB::IsOpen" << std::endl;

  // return db;
  return false;
}

void OdbcDB::Close() {
  // if (!IsOpen()) {
  // 	return;
  // }
  // auto rc = sqlite3_close_v2(db);
  // if (rc == SQLITE_BUSY) {
  // 	throw InternalException("Failed to close database - SQLITE_BUSY");
  // }
  // db = nullptr;
}

vector<string> OdbcDB::GetEntries(string entry_type) {
  vector<string> result;
  OdbcStatement stmt = Prepare("SELECT name FROM sqlite_master WHERE type='" + entry_type + "'");
  while (stmt.Step()) {
    auto table_name = stmt.GetValue<string>(0);
    result.push_back(std::move(table_name));
  }
  return result;
}

vector<string> OdbcDB::GetTables() { return GetEntries("table"); }

CatalogType OdbcDB::GetEntryType(const string &name) {
  OdbcStatement stmt;
  stmt = Prepare(StringUtil::Format("SELECT type FROM sqlite_master WHERE lower(name)=lower('%s');",
                                    OdbcUtils::SanitizeString(name)));
  while (stmt.Step()) {
    auto type = stmt.GetValue<string>(0);
    if (type == "table") {
      return CatalogType::TABLE_ENTRY;
    } else if (type == "view") {
      return CatalogType::VIEW_ENTRY;
    } else if (type == "index") {
      return CatalogType::INDEX_ENTRY;
    } else {
      throw InternalException("Unrecognized Odbc type \"%s\"", name);
    }
  }
  return CatalogType::INVALID;
}

void OdbcDB::GetIndexInfo(const string &index_name, string &sql, string &table_name) {
  OdbcStatement stmt;
  stmt = Prepare(StringUtil::Format("SELECT tbl_name, sql FROM sqlite_master WHERE lower(name)=lower('%s');",
                                    OdbcUtils::SanitizeString(index_name)));
  while (stmt.Step()) {
    table_name = stmt.GetValue<string>(0);
    sql = stmt.GetValue<string>(1);
    return;
  }
  throw InternalException("GetViewInfo - index \"%s\" not found", index_name);
}

void OdbcDB::GetViewInfo(const string &view_name, string &sql) {
  OdbcStatement stmt;
  stmt = Prepare(StringUtil::Format("SELECT sql FROM sqlite_master WHERE lower(name)=lower('%s');",
                                    OdbcUtils::SanitizeString(view_name)));
  while (stmt.Step()) {
    sql = stmt.GetValue<string>(0);
    return;
  }
  throw InternalException("GetViewInfo - view \"%s\" not found", view_name);
}

void OdbcDB::GetTableInfo(const string &table_name, ColumnList &columns,
                          vector<unique_ptr<Constraint>> &constraints, bool all_varchar) {
  OdbcStatement stmt;

  idx_t primary_key_index = idx_t(-1);
  vector<string> primary_keys;

  bool found = false;

  stmt = Prepare(StringUtil::Format("PRAGMA table_info('%s')", OdbcUtils::SanitizeString(table_name)));
  while (stmt.Step()) {
    auto cid = stmt.GetValue<int>(0);
    auto sqlite_colname = stmt.GetValue<string>(1);
    auto sqlite_type = StringUtil::Lower(stmt.GetValue<string>(2));
    auto not_null = stmt.GetValue<int>(3);
    auto default_value = stmt.GetValue<string>(4);
    auto pk = stmt.GetValue<int>(5);
    StringUtil::Trim(sqlite_type);
    auto column_type = all_varchar ? LogicalType::VARCHAR : OdbcUtils::TypeToLogicalType(sqlite_type);

    ColumnDefinition column(std::move(sqlite_colname), std::move(column_type));
    if (!default_value.empty()) {
      auto expressions = Parser::ParseExpressionList(default_value);
      if (expressions.empty()) {
        throw InternalException("Expression list is empty");
      }
      column.SetDefaultValue(std::move(expressions[0]));
    }
    columns.AddColumn(std::move(column));
    if (not_null) {
      constraints.push_back(make_uniq<NotNullConstraint>(LogicalIndex(cid)));
    }
    if (pk) {
      primary_key_index = cid;
      primary_keys.push_back(sqlite_colname);
    }
    found = true;
  }
  if (!found) {
    throw InternalException("GetTableInfo - table \"%s\" not found", table_name);
  }
  if (!primary_keys.empty()) {
    if (primary_keys.size() == 1) {
      constraints.push_back(make_uniq<UniqueConstraint>(LogicalIndex(primary_key_index), true));
    } else {
      constraints.push_back(make_uniq<UniqueConstraint>(std::move(primary_keys), true));
    }
  }
}

bool OdbcDB::ColumnExists(const string &table_name, const string &column_name) {
  OdbcStatement stmt;

  stmt = Prepare(StringUtil::Format("PRAGMA table_info(\"%s\")", OdbcUtils::SanitizeIdentifier(table_name)));
  while (stmt.Step()) {
    auto sqlite_colname = stmt.GetValue<string>(1);
    if (sqlite_colname == column_name) {
      return true;
    }
  }
  return false;
}

bool OdbcDB::GetMaxRowId(const string &table_name, idx_t &max_row_id) {
  OdbcStatement stmt;
  if (!TryPrepare(
          StringUtil::Format("SELECT MAX(ROWID) FROM \"%s\"", OdbcUtils::SanitizeIdentifier(table_name)),
          stmt)) {
    return false;
  }
  if (!stmt.Step()) {
    return false;
  }
  int64_t val = stmt.GetValue<int64_t>(0);
  ;
  if (val <= 0) {
    return false;
  }
  max_row_id = idx_t(val);
  return true;
}

vector<IndexInfo> OdbcDB::GetIndexInfo(const string &table_name) {
  vector<IndexInfo> info;
  // fetch the primary key
  OdbcStatement stmt;
  stmt = Prepare(StringUtil::Format("SELECT cid FROM pragma_table_info('%s') WHERE pk",
                                    OdbcUtils::SanitizeString(table_name)));
  IndexInfo pk_index;
  while (stmt.Step()) {
    auto cid = stmt.GetValue<int64_t>(0);
    pk_index.column_set.insert(cid);
  }
  if (!pk_index.column_set.empty()) {
    // we have a pk - add it
    pk_index.is_primary = true;
    pk_index.is_unique = true;
    pk_index.is_foreign = false;
    info.push_back(std::move(pk_index));
  }

  // now query the set of unique constraints for the table
  stmt =
      Prepare(StringUtil::Format("SELECT name FROM pragma_index_list('%s') WHERE \"unique\" AND origin='u'",
                                 OdbcUtils::SanitizeString(table_name)));
  vector<string> unique_indexes;
  while (stmt.Step()) {
    auto index_name = stmt.GetValue<string>(0);
    unique_indexes.push_back(index_name);
  }
  for (auto &index_name : unique_indexes) {
    stmt = Prepare(
        StringUtil::Format("SELECT cid FROM pragma_index_info('%s')", OdbcUtils::SanitizeString(index_name)));
    IndexInfo unique_index;
    while (stmt.Step()) {
      auto cid = stmt.GetValue<int64_t>(0);
      unique_index.column_set.insert(cid);
    }
    if (!unique_index.column_set.empty()) {
      // we have a pk - add it
      unique_index.is_primary = false;
      unique_index.is_unique = true;
      unique_index.is_foreign = false;
      info.push_back(std::move(unique_index));
    }
  }
  return info;
}

idx_t OdbcDB::RunPragma(string pragma_name) {
  OdbcStatement stmt;
  stmt = Prepare("PRAGMA " + pragma_name);
  while (stmt.Step()) {
    return idx_t(stmt.GetValue<int64_t>(0));
  }
  throw InternalException("No result returned from pragma " + pragma_name);
}

} // namespace duckdb
