#include "storage/odbc_transaction.hpp"
#include "duckdb/catalog/catalog_entry/index_catalog_entry.hpp"
#include "duckdb/catalog/catalog_entry/view_catalog_entry.hpp"
#include "duckdb/parser/parsed_data/create_table_info.hpp"
#include "duckdb/parser/parsed_data/create_view_info.hpp"
#include "storage/odbc_catalog.hpp"
#include "storage/odbc_index_entry.hpp"
#include "storage/odbc_schema_entry.hpp"
#include "storage/odbc_table_entry.hpp"

namespace duckdb {

OdbcTransaction::OdbcTransaction(OdbcCatalog &odbc_catalog, TransactionManager &manager,
                                 ClientContext &context)
    : Transaction(manager, context), odbc_catalog(odbc_catalog) {
  if (odbc_catalog.InMemory()) {
    // in-memory database - get a reference to the in-memory connection
    db = odbc_catalog.GetInMemoryDatabase();
  } else {
    // on-disk database - open a new database connection
    owned_db = OdbcDB::Open(odbc_catalog.path,
                            odbc_catalog.access_mode == AccessMode::READ_ONLY ? true : false, true);
    db = &owned_db;
  }
}

OdbcTransaction::~OdbcTransaction() { odbc_catalog.ReleaseInMemoryDatabase(); }

void OdbcTransaction::Start() { db->Execute("BEGIN TRANSACTION"); }
void OdbcTransaction::Commit() { db->Execute("COMMIT"); }
void OdbcTransaction::Rollback() { db->Execute("ROLLBACK"); }

OdbcDB &OdbcTransaction::GetDB() { return *db; }

OdbcTransaction &OdbcTransaction::Get(ClientContext &context, Catalog &catalog) {
  return Transaction::Get(context, catalog).Cast<OdbcTransaction>();
}

optional_ptr<CatalogEntry> OdbcTransaction::GetCatalogEntry(const string &entry_name) {
  auto entry = catalog_entries.find(entry_name);
  if (entry != catalog_entries.end()) {
    return entry->second.get();
  }
  // catalog entry not found - look up table in main Odbc database
  auto type = db->GetEntryType(entry_name);
  if (type == CatalogType::INVALID) {
    // no table or view found
    return nullptr;
  }
  unique_ptr<CatalogEntry> result;
  switch (type) {
  case CatalogType::TABLE_ENTRY: {
    CreateTableInfo info(odbc_catalog.GetMainSchema(), entry_name);
    // FIXME: all_varchar from config
    db->GetTableInfo(entry_name, info.columns, info.constraints, false);
    D_ASSERT(!info.columns.empty());

    result = make_uniq<OdbcTableEntry>(odbc_catalog, odbc_catalog.GetMainSchema(), info);
    break;
  }
  case CatalogType::VIEW_ENTRY: {
    string sql;
    db->GetViewInfo(entry_name, sql);

    auto view_info = CreateViewInfo::FromCreateView(*context.lock(), sql);
    view_info->internal = false;
    result = make_uniq<ViewCatalogEntry>(odbc_catalog, odbc_catalog.GetMainSchema(), *view_info);
    break;
  }
  case CatalogType::INDEX_ENTRY: {
    CreateIndexInfo info;
    info.index_name = entry_name;

    string table_name;
    string sql;
    db->GetIndexInfo(entry_name, sql, table_name);

    auto index_entry =
        make_uniq<OdbcIndexEntry>(odbc_catalog, odbc_catalog.GetMainSchema(), info, std::move(table_name));
    index_entry->sql = std::move(sql);
    result = std::move(index_entry);
    break;
  }
  default:
    throw InternalException("Unrecognized catalog entry type");
  }
  auto result_ptr = result.get();
  catalog_entries[entry_name] = std::move(result);
  return result_ptr;
}

void OdbcTransaction::ClearTableEntry(const string &table_name) { catalog_entries.erase(table_name); }

string GetDropSQL(CatalogType type, const string &table_name, bool cascade) {
  string result;
  result = "DROP ";
  switch (type) {
  case CatalogType::TABLE_ENTRY:
    result += "TABLE ";
    break;
  case CatalogType::VIEW_ENTRY:
    result += "VIEW ";
    break;
  case CatalogType::INDEX_ENTRY:
    result += "INDEX ";
    break;
  default:
    throw InternalException("Unsupported type for drop");
  }
  result += KeywordHelper::WriteOptionallyQuoted(table_name);
  return result;
}

void OdbcTransaction::DropEntry(CatalogType type, const string &table_name, bool cascade) {
  catalog_entries.erase(table_name);
  db->Execute(GetDropSQL(type, table_name, cascade));
}

} // namespace duckdb
