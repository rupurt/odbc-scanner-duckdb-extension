#pragma once

#include "duckdb/storage/storage_extension.hpp"

namespace duckdb {

class OdbcStorageExtension : public StorageExtension {
public:
	OdbcStorageExtension();
};

} // namespace duckdb
