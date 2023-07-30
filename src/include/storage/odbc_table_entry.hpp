#pragma once

#include "duckdb/catalog/catalog_entry/table_catalog_entry.hpp"

namespace duckdb {

class OdbcTableEntry : public TableCatalogEntry {
public:
  OdbcTableEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateTableInfo &info);

public:
  unique_ptr<BaseStatistics> GetStatistics(ClientContext &context, column_t column_id) override;

  TableFunction GetScanFunction(ClientContext &context, unique_ptr<FunctionData> &bind_data) override;

  TableStorageInfo GetStorageInfo(ClientContext &context) override;

  void BindUpdateConstraints(LogicalGet &get, LogicalProjection &proj, LogicalUpdate &update,
                             ClientContext &context) override;
};

} // namespace duckdb
