#include "storage/odbc_transaction_manager.hpp"
#include "duckdb/main/attached_database.hpp"

namespace duckdb {

OdbcTransactionManager::OdbcTransactionManager(AttachedDatabase &db_p, OdbcCatalog &sqlite_catalog)
    : TransactionManager(db_p), sqlite_catalog(sqlite_catalog) {
}

Transaction *OdbcTransactionManager::StartTransaction(ClientContext &context) {
	auto transaction = make_uniq<OdbcTransaction>(sqlite_catalog, *this, context);
	transaction->Start();
	auto result = transaction.get();
	lock_guard<mutex> l(transaction_lock);
	transactions[result] = std::move(transaction);
	return result;
}

string OdbcTransactionManager::CommitTransaction(ClientContext &context, Transaction *transaction) {
	auto sqlite_transaction = (OdbcTransaction *)transaction;
	sqlite_transaction->Commit();
	lock_guard<mutex> l(transaction_lock);
	transactions.erase(transaction);
	return string();
}

void OdbcTransactionManager::RollbackTransaction(Transaction *transaction) {
	auto sqlite_transaction = (OdbcTransaction *)transaction;
	sqlite_transaction->Rollback();
	lock_guard<mutex> l(transaction_lock);
	transactions.erase(transaction);
}

void OdbcTransactionManager::Checkpoint(ClientContext &context, bool force) {
	auto &transaction = OdbcTransaction::Get(context, db.GetCatalog());
	auto &db = transaction.GetDB();
	db.Execute("PRAGMA wal_checkpoint");
}

} // namespace duckdb
