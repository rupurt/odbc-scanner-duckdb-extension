#pragma once

#include "duckdb/transaction/transaction_manager.hpp"
#include "storage/odbc_catalog.hpp"
#include "storage/odbc_transaction.hpp"

namespace duckdb {

class OdbcTransactionManager : public TransactionManager {
public:
	OdbcTransactionManager(AttachedDatabase &db_p, OdbcCatalog &sqlite_catalog);

	Transaction *StartTransaction(ClientContext &context) override;
	string CommitTransaction(ClientContext &context, Transaction *transaction) override;
	void RollbackTransaction(Transaction *transaction) override;

	void Checkpoint(ClientContext &context, bool force = false) override;

private:
	OdbcCatalog &sqlite_catalog;
	mutex transaction_lock;
	unordered_map<Transaction *, unique_ptr<OdbcTransaction>> transactions;
};

} // namespace duckdb
