#pragma once

#include "duckdb.hpp"
#include "duckdb/function/table_function.hpp"

#include "sql.h"
#include "sqlext.h"

namespace duckdb {
struct OdbcDiagnostics {
  std::string msg;
  std::string state;
  SQLINTEGER native;
};

static unique_ptr<OdbcDiagnostics> ExtractDiagnostics(SQLSMALLINT handle_type, SQLHANDLE handle) {
  SQLINTEGER i = 1;
  SQLINTEGER native;
  SQLCHAR state[7];
  SQLCHAR text[256];
  SQLSMALLINT len;
  SQLRETURN return_code;
  auto diagnostics = make_uniq<OdbcDiagnostics>();

  while (SQL_SUCCEEDED(
      SQLGetDiagRec(handle_type, handle, i, state, &native, text, sizeof(text), &len))) {
    diagnostics->msg += string((char *)text);
    diagnostics->state = string((char *)state);
    diagnostics->native = native;
    i++;
  }

  return std::move(diagnostics);
}

static std::string SqlReturnCodeToString(SQLRETURN return_code) {
  switch (return_code) {
  case SQL_SUCCESS:
    return "SQL_SUCCESS";
  case SQL_SUCCESS_WITH_INFO:
    return "SQL_SUCCESS_WITH_INFO";
  case SQL_NO_DATA:
    return "SQL_NO_DATA";
  case SQL_ERROR:
    return "SQL_ERROR";
  case SQL_INVALID_HANDLE:
    return "SQL_INVALID_HANDLE";
  case SQL_STILL_EXECUTING:
    return "SQL_STILL_EXECUTING";
  case SQL_NEED_DATA:
    return "SQL_NEED_DATA";
  default:
    return "UNKNOWN";
  }
}

static void ThrowExceptionWithDiagnostics(std::string msg_prefix, SQLHANDLE handle,
                                          SQLRETURN return_code) {
  auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
  throw Exception(msg_prefix + " return_code=" + std::to_string(return_code) + ":" +
                  SqlReturnCodeToString(return_code) + " msg='" + diagnostics->msg + "' state=" +
                  diagnostics->state + " native=" + std::to_string(diagnostics->native));
}
} // namespace duckdb
