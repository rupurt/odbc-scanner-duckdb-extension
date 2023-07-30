#pragma once

#include "duckdb/transaction/transaction.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "odbc_db.hpp"

namespace duckdb {
class OdbcCatalog;
class OdbcTableEntry;

class OdbcTransaction : public Transaction {
public:
	OdbcTransaction(OdbcCatalog &odbc_catalog, TransactionManager &manager, ClientContext &context);
	~OdbcTransaction() override;

	void Start();
	void Commit();
	void Rollback();

	OdbcDB &GetDB();
	optional_ptr<CatalogEntry> GetCatalogEntry(const string &table_name);
	void DropEntry(CatalogType type, const string &table_name, bool cascade);
	void ClearTableEntry(const string &table_name);

	static OdbcTransaction &Get(ClientContext &context, Catalog &catalog);

private:
	OdbcCatalog &odbc_catalog;
	OdbcDB *db;
	OdbcDB owned_db;
	case_insensitive_map_t<unique_ptr<CatalogEntry>> catalog_entries;
};

} // namespace duckdb
