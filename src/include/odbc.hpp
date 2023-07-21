#pragma once

#include "exception.hpp"

#include "duckdb.hpp"
#include "duckdb/function/table_function.hpp"

#include "sql.h"
#include "sqlext.h"

namespace duckdb {
struct OdbcEnvironment {
  OdbcEnvironment() { handle = SQL_NULL_HENV; }
  ~OdbcEnvironment() { FreeHandle(); }

  SQLHENV handle;

  void FreeHandle() {
    if (handle != SQL_NULL_HENV) {
      auto return_code = SQLFreeHandle(SQL_HANDLE_ENV, handle);
      if (!SQL_SUCCEEDED(return_code)) {
        ThrowExceptionWithDiagnostics("OdbcEnvironment->Init() SQLFreeHandle", SQL_HANDLE_ENV, handle,
                                      return_code);
      }
    }
  }

public:
  void Init() {
    SQLRETURN return_code;

    if (handle != SQL_NULL_HENV) {
      throw Exception("OdbcEnvironment->Init() handle is not null");
    }

    return_code = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HENV, &handle);
    if (return_code == SQL_SUCCESS || return_code == SQL_SUCCESS_WITH_INFO) {
      // ok
    } else if (return_code == SQL_INVALID_HANDLE) {
      throw Exception("OdbcEnvironment->Init() SQLAllocHandle SQL_RETURN=" + std::to_string(return_code) +
                      " SQL_INVALID_HANDLE");
    } else if (return_code == SQL_ERROR) {
      throw Exception("OdbcEnvironment->Init() SQLAllocHandle SQL_RETURN=" + std::to_string(return_code) +
                      " SQL_ERROR");
    } else {
      throw Exception("OdbcEnvironment->Init() SQLAllocHandle SQL_RETURN=" + std::to_string(return_code) +
                      " UNKNOWN_SQL_RETURN");
    }

    return_code = SQLSetEnvAttr(handle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER *)SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED(return_code)) {
      ThrowExceptionWithDiagnostics("OdbcEnvironment->Init() SQLSetEnvAttr", SQL_HANDLE_ENV, handle,
                                    return_code);
    }
  }
  SQLHENV Handle() const { return handle; }
};

#define MAX_CONN_STR_OUT 1024

struct OdbcConnection {
  OdbcConnection() : handle(SQL_NULL_HDBC), dialed(false) {}
  ~OdbcConnection() {
    Disconnect();
    FreeHandle();
  }

  SQLHDBC handle;
  bool dialed;

  void FreeHandle() {
    if (handle != SQL_NULL_HDBC) {
      auto return_code = SQLFreeHandle(SQL_HANDLE_DBC, handle);
      if (!SQL_SUCCEEDED(return_code)) {
        ThrowExceptionWithDiagnostics("OdbcConnection->FreeHandle() SQLFreeHandle", SQL_HANDLE_DBC, handle,
                                      return_code);
      }
    }
  }

public:
  void Init(shared_ptr<OdbcEnvironment> &env) {
    if (handle != SQL_NULL_HDBC) {
      throw Exception("OdbcConnection->Init(): connection handle is not null");
    }

    auto sql_return = SQLAllocHandle(SQL_HANDLE_DBC, env->Handle(), &handle);
    if (sql_return == SQL_SUCCESS || sql_return == SQL_SUCCESS_WITH_INFO) {
      return;
    } else if (sql_return == SQL_INVALID_HANDLE) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_ENV, env->Handle());
      throw Exception("OdbcConnection->Init() SQLAllocHandle SQL_RETURN=" + std::to_string(sql_return) +
                      " SQL_INVALID_HANDLE msg='" + diagnostics->msg + "' state=" + diagnostics->state +
                      " native=" + std::to_string(diagnostics->native));
    } else if (sql_return == SQL_ERROR) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_ENV, env->Handle());
      throw Exception("OdbcConnection->Init() SQLAllocHandle SQL_RETURN=" + std::to_string(sql_return) +
                      " SQL_ERROR msg='" + diagnostics->msg + "' state=" + diagnostics->state +
                      " native=" + std::to_string(diagnostics->native));
    } else {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_ENV, env->Handle());
      throw Exception("OdbcConnection->Init() SQLAllocHandle SQL_RETURN=" + std::to_string(sql_return) +
                      " UNKNOWN_SQL_RETURN msg='" + diagnostics->msg + "' state=" + diagnostics->state +
                      " native=" + std::to_string(diagnostics->native));
    }
  }
  void Dial(string connection_string) {
    auto conn_str_in_len = (SQLSMALLINT)connection_string.length();
    SQLSMALLINT conn_str_out_len = 0;
    SQLCHAR conn_str_out[MAX_CONN_STR_OUT + 1] = {0};

    auto sql_return =
        SQLDriverConnect(handle, NULL, (SQLCHAR *)connection_string.c_str(), conn_str_in_len, conn_str_out,
                         (SQLSMALLINT)MAX_CONN_STR_OUT, &conn_str_out_len, SQL_DRIVER_NOPROMPT);
    if (sql_return == SQL_SUCCESS || sql_return == SQL_SUCCESS_WITH_INFO) {
      dialed = true;
    } else if (sql_return == SQL_NO_DATA_FOUND) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_DBC, handle);
      throw Exception("OdbcConnection->Dial() SQLDriverConnect SQL_RETURN=" + std::to_string(sql_return) +
                      " SQL_NO_DATA_FOUND msg='" + diagnostics->msg + "' state=" + diagnostics->state +
                      " native=" + std::to_string(diagnostics->native));
    } else if (sql_return == SQL_INVALID_HANDLE) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_DBC, handle);
      throw Exception("OdbcConnection->Dial() SQLDriverConnect SQL_RETURN=" + std::to_string(sql_return) +
                      " SQL_INVALID_HANDLE msg='" + diagnostics->msg + "' state=" + diagnostics->state +
                      " native=" + std::to_string(diagnostics->native));
    } else if (sql_return == SQL_ERROR) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_DBC, handle);
      throw Exception(std::to_string(sql_return) + " SQL_ERROR msg='" + diagnostics->msg +
                      "' state=" + diagnostics->state + " native=" + std::to_string(diagnostics->native));
    } else {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_DBC, handle);
      throw Exception("OdbcConnection->Dial() SQLDriverConnect SQL_RETURN=" + std::to_string(sql_return) +
                      " UNKNOWN_SQL_RETURN msg='" + diagnostics->msg + "' state=" + diagnostics->state +
                      " native=" + std::to_string(diagnostics->native));
    }
  }
  void Disconnect() {
    if (!dialed) {
      return;
    }

    auto sql_return = SQLDisconnect(handle);
    if (sql_return == SQL_SUCCESS || sql_return == SQL_SUCCESS_WITH_INFO) {
      return;
    } else if (sql_return == SQL_INVALID_HANDLE) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_DBC, handle);
      throw Exception("OdbcConnection->Disconnect() SQLDisconnect SQL_RETURN=" + std::to_string(sql_return) +
                      " SQL_INVALID_HANDLE msg='" + diagnostics->msg + "' state=" + diagnostics->state +
                      " native=" + std::to_string(diagnostics->native));
    } else if (sql_return == SQL_ERROR) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_DBC, handle);
      throw Exception("OdbcConnection->Disconnect() SQLDisconnect SQL_RETURN=" + std::to_string(sql_return) +
                      " SQL_ERROR msg='" + diagnostics->msg + "' state=" + diagnostics->state +
                      " native=" + std::to_string(diagnostics->native));
    } else {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_DBC, handle);
      throw Exception("OdbcConnection->Disconnect() SQLDisconnect SQL_RETURN=" + std::to_string(sql_return) +
                      " UNKNOWN_SQL_RETURN msg='" + diagnostics->msg + "' state=" + diagnostics->state +
                      " native=" + std::to_string(diagnostics->native));
    }
    dialed = false;
  }
  SQLHSTMT Handle() { return handle; }
};

struct OdbcColumnDescription {
  SQLCHAR name[32];
  SQLSMALLINT name_length;
  SQLSMALLINT sql_data_type;
  SQLSMALLINT c_data_type;
  SQLULEN size;
  SQLULEN length;
  SQLSMALLINT decimal_digits;
  SQLSMALLINT nullable;
};

struct OdbcStatementOptions {
  OdbcStatementOptions(SQLULEN _row_array_size) : row_array_size(_row_array_size) {}

  SQLULEN row_array_size;
};

struct OdbcStatement {
  OdbcStatement(shared_ptr<OdbcConnection> _conn)
      : conn(_conn), handle(SQL_NULL_HSTMT), prepared(false), executing(false) {}
  ~OdbcStatement() {
    prepared = false;
    executing = false;
    FreeHandle();
  }

  shared_ptr<OdbcConnection> conn;
  SQLHSTMT handle;
  bool prepared;
  bool executing;

  void FreeHandle() {
    if (handle != SQL_NULL_HSTMT) {
      SQLRETURN return_code = SQLFreeHandle(SQL_HANDLE_STMT, handle);
      if (!SQL_SUCCEEDED(return_code)) {
        ThrowExceptionWithDiagnostics("OdbcStatement->FreeHandle() SQLFreeHandle", SQL_HANDLE_STMT, handle,
                                      return_code);
      }
    }
  }

public:
  void Init() {
    if (handle != SQL_NULL_HSTMT) {
      throw Exception("OdbcStatement->Init() handle has already been initialized. To "
                      "execute a different statement instantiate a new statement");
    }

    auto sql_return = SQLAllocHandle(SQL_HANDLE_STMT, conn->Handle(), &handle);
    if (sql_return == SQL_SUCCESS || sql_return == SQL_SUCCESS_WITH_INFO) {
      return;
    } else if (sql_return == SQL_INVALID_HANDLE) {
      throw Exception("OdbcStatement->Init() SQLAllocHandle SQL_RETURN=" + std::to_string(sql_return) +
                      " SQL_INVALID_HANDLE");
    } else if (sql_return == SQL_ERROR) {
      throw Exception("OdbcStatement->Init() SQLAllocHandle SQL_RETURN=" + std::to_string(sql_return) +
                      " SQL_ERROR");
    } else {
      throw Exception("OdbcStatement->Init() SQLAllocHandle SQL_RETURN=" + std::to_string(sql_return) +
                      " UNKNOWN_SQL_RETURN");
    }
  }
  void Prepare(std::string sql_statement) {
    if (handle == SQL_NULL_HSTMT) {
      throw Exception("OdbcStatement->Prepare() handle has not been allocated. Call "
                      "OdbcStatement#Init() before OdbcStatement#Prepare()");
    }

    auto sql_len = (SQLSMALLINT)sql_statement.length();
    auto sql_return = SQLPrepare(handle, (SQLCHAR *)sql_statement.c_str(), sql_len);
    if (sql_return == SQL_SUCCESS || sql_return == SQL_SUCCESS_WITH_INFO) {
      prepared = true;
      return;
    } else if (sql_return == SQL_ERROR) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->Prepare() SQLPrepare SQL_RETURN=" + std::to_string(sql_return) +
                      " SQL_ERROR msg='" + diagnostics->msg + "' state=" + diagnostics->state +
                      " native=" + std::to_string(diagnostics->native));
    } else if (sql_return == SQL_INVALID_HANDLE) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->Prepare() SQLPrepare SQL_RETURN=" + std::to_string(sql_return) +
                      " SQL_INVALID_HANDLE msg='" + diagnostics->msg + "' state=" + diagnostics->state +
                      " native=" + std::to_string(diagnostics->native));
    } else {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->Prepare() SQLPrepare SQL_RETURN=" + std::to_string(sql_return) +
                      " UNKNOWN_SQL_RETURN msg='" + diagnostics->msg + "' state=" + diagnostics->state +
                      " native=" + std::to_string(diagnostics->native));
    }
  }
  void SetAttribute(SQLINTEGER attribute, SQLPOINTER value) {
    if (handle == SQL_NULL_HSTMT) {
      throw Exception("OdbcStatement->SetAttribute() handle has not been allocated. Call "
                      "OdbcStatement#Init() before OdbcStatement#SetAttribute()");
    }

    auto row_status_sql_return = SQLSetStmtAttr(handle, attribute, value, 0);
    if (row_status_sql_return == SQL_SUCCESS || row_status_sql_return == SQL_SUCCESS_WITH_INFO) {
      return;
    } else if (row_status_sql_return == SQL_ERROR) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->SetAttribute() SQLSetStmtAttr SQL_RETURN=" +
                      std::to_string(row_status_sql_return) + " SQL_ERROR msg='" + diagnostics->msg +
                      "' state=" + diagnostics->state + " native=" + std::to_string(diagnostics->native));
    } else if (row_status_sql_return == SQL_INVALID_HANDLE) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->SetAttribute() SQLSetStmtAttr SQL_RETURN=" +
                      std::to_string(row_status_sql_return) + " SQL_INVALID_HANDLE msg='" + diagnostics->msg +
                      "' state=" + diagnostics->state + " native=" + std::to_string(diagnostics->native));
    } else {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->SetAttribute() SQLSetStmtAttr SQL_RETURN=" +
                      std::to_string(row_status_sql_return) + " UNKNOWN_SQL_RETURN msg='" + diagnostics->msg +
                      "' state=" + diagnostics->state + " native=" + std::to_string(diagnostics->native));
    }
  }
  void BindColumn(SQLUSMALLINT column_number, SQLSMALLINT c_data_type, unsigned char *buffer,
                  SQLULEN column_buffer_length, SQLLEN *strlen_or_ind) {
    if (handle == SQL_NULL_HSTMT) {
      throw Exception("OdbcStatement->BindColumn() handle has not been allocated. Call "
                      "OdbcStatement#Init() before OdbcStatement#BindColumn()");
    }

    auto return_code =
        SQLBindCol(handle, column_number, c_data_type, buffer, column_buffer_length, strlen_or_ind);
    if (!SQL_SUCCEEDED(return_code)) {
      ThrowExceptionWithDiagnostics("OdbcStatement->BindCol() SQLBindCol", SQL_HANDLE_STMT, handle,
                                    return_code);
    }
  }
  SQLSMALLINT NumResultCols() {
    if (handle == SQL_NULL_HSTMT) {
      throw Exception("OdbcStatement->NumResultCols() handle has not been allocated. Call "
                      "OdbcStatement#Init() before OdbcStatement#Prepare()");
    }
    if (!prepared) {
      throw Exception("OdbcStatement->NumResultCols() statement has "
                      "not been prepared. Call OdbcStatement#Prepare() before "
                      "OdbcStatement#NumResultCols()");
    }

    SQLSMALLINT num_result_cols = 0;
    auto return_code = SQLNumResultCols(handle, &num_result_cols);
    if (!SQL_SUCCEEDED(return_code)) {
      ThrowExceptionWithDiagnostics("OdbcStatement->NumResultCols() SQLNumResultCols", SQL_HANDLE_STMT,
                                    handle, return_code);
    }

    return num_result_cols;
  }
  vector<OdbcColumnDescription> DescribeColumns() {
    auto num_result_cols = NumResultCols();
    auto column_descriptions = vector<OdbcColumnDescription>(num_result_cols);

    for (SQLUSMALLINT i = 0; i < num_result_cols; i++) {
      auto col_desc = &column_descriptions.at(i);

      auto return_code = SQLDescribeCol(handle, i + 1, col_desc->name, sizeof(col_desc->name),
                                        &col_desc->name_length, &col_desc->sql_data_type, &col_desc->size,
                                        &col_desc->decimal_digits, &col_desc->nullable);
      if (!SQL_SUCCEEDED(return_code)) {
        ThrowExceptionWithDiagnostics("OdbcStatement->DescribeColumns() SQLDescribeCol", SQL_HANDLE_STMT,
                                      handle, return_code);
      }

      SqlDataTypeToCDataType(col_desc);
    }

    return column_descriptions;
  }
  void Execute(unique_ptr<OdbcStatementOptions> &opts) {
    if (handle == SQL_NULL_HSTMT) {
      throw Exception("OdbcStatement->Execute() handle is null");
    }
    if (!prepared) {
      throw Exception("OdbcStatement->Execute() statement is not prepared");
    }
    if (executing) {
      throw Exception("OdbcStatement->Execute() previous statement is executing");
    }

    SetAttribute(SQL_ATTR_ROW_BIND_TYPE, SQL_BIND_BY_COLUMN);
    SetAttribute(SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)opts->row_array_size);

    auto execute_sql_return = SQLExecute(handle);
    if (execute_sql_return == SQL_SUCCESS || execute_sql_return == SQL_SUCCESS_WITH_INFO) {
      executing = true;
      return;
    } else if (execute_sql_return == SQL_ERROR) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->DescribeColumns() SQLExecute SQL_RETURN=" +
                      std::to_string(execute_sql_return) + " SQL_ERROR msg='" + diagnostics->msg +
                      "' state=" + diagnostics->state + " native=" + std::to_string(diagnostics->native));
    } else if (execute_sql_return == SQL_INVALID_HANDLE) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->DescribeColumns() SQLExecute SQL_RETURN=" +
                      std::to_string(execute_sql_return) + " SQL_INVALID_HANDLE msg='" + diagnostics->msg +
                      "' state=" + diagnostics->state + " native=" + std::to_string(diagnostics->native));
    } else if (execute_sql_return == SQL_NEED_DATA) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->DescribeColumns() SQLExecute SQL_RETURN=" +
                      std::to_string(execute_sql_return) + " SQL_NEED_DATA msg='" + diagnostics->msg +
                      "' state=" + diagnostics->state + " native=" + std::to_string(diagnostics->native));
    } else if (execute_sql_return == SQL_NO_DATA_FOUND) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->DescribeColumns() SQLExecute SQL_RETURN=" +
                      std::to_string(execute_sql_return) + " SQL_NO_DATA_FOUND msg='" + diagnostics->msg +
                      "' state=" + diagnostics->state + " native=" + std::to_string(diagnostics->native));
    } else {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->DescribeColumns() SQLExecute SQL_RETURN=" +
                      std::to_string(execute_sql_return) + " UNKNOWN_SQL_RETURN msg='" + diagnostics->msg +
                      "' state=" + diagnostics->state + " native=" + std::to_string(diagnostics->native));
    }
  }
  // TODO:
  // - support multiple fetch orientations
  SQLLEN Fetch() {
    if (handle == SQL_NULL_HSTMT) {
      throw Exception("OdbcStatement->Fetch() handle is null");
    }
    if (!prepared) {
      throw Exception("OdbcStatement->Fetch() statement is not prepared");
    }
    if (!executing) {
      throw Exception("OdbcStatement->Fetch() statement is not executing");
    }

    SQLLEN rows_fetched = 0;
    SetAttribute(SQL_ATTR_ROWS_FETCHED_PTR, (SQLPOINTER)&rows_fetched);

    // TODO:
    // - should this return a tuple with the rows fetched + sql returns + error?
    auto execute_sql_return = SQLFetchScroll(handle, SQL_FETCH_NEXT, 0);
    if (execute_sql_return == SQL_SUCCESS || execute_sql_return == SQL_SUCCESS_WITH_INFO ||
        execute_sql_return == SQL_NO_DATA_FOUND) {
      return rows_fetched;
      // } else if (execute_sql_return == SQL_NO_DATA_FOUND) {
      //   auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT,
      //   handle); throw Exception(
      //       "OdbcStatement->Fetch() SQLFetchScroll SQL_RETURN=" +
      //       std::to_string(execute_sql_return) + " SQL_NO_DATA_FOUND msg='" +
      //       diagnostics->msg + "' state=" + diagnostics->state +
      //       " native=" + std::to_string(diagnostics->native));
    } else if (execute_sql_return == SQL_ERROR) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->Fetch() SQLFetchScroll SQL_RETURN=" +
                      std::to_string(execute_sql_return) + " SQL_ERROR msg='" + diagnostics->msg +
                      "' state=" + diagnostics->state + " native=" + std::to_string(diagnostics->native));
    } else if (execute_sql_return == SQL_INVALID_HANDLE) {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->Fetch() SQLFetchScroll SQL_RETURN=" +
                      std::to_string(execute_sql_return) + " SQL_INVALID_HANDLE msg='" + diagnostics->msg +
                      "' state=" + diagnostics->state + " native=" + std::to_string(diagnostics->native));
    } else {
      auto diagnostics = ExtractDiagnostics(SQL_HANDLE_STMT, handle);
      throw Exception("OdbcStatement->Fetch() SQLFetchScroll SQL_RETURN=" +
                      std::to_string(execute_sql_return) + " UNKNOWN_SQL_RETURN msg='" + diagnostics->msg +
                      "' state=" + diagnostics->state + " native=" + std::to_string(diagnostics->native));
    }
  }

protected:
  static void SqlDataTypeToCDataType(OdbcColumnDescription *col_desc) {
    // TODO:
    // - unixodbc doesn't seem to define all possible sql types
    switch (col_desc->sql_data_type) {
    // case SQL_BIT:
    //   col_desc->c_data_type = SQL_C_BIT;
    //   col_desc->length = sizeof(SQLCHAR);
    //   break;
    case SQL_SMALLINT:
      col_desc->c_data_type = SQL_C_SHORT;
      col_desc->length = sizeof(SQLSMALLINT);
      break;
    case SQL_INTEGER:
      col_desc->c_data_type = SQL_C_LONG;
      col_desc->length = sizeof(SQLINTEGER);
      break;
    case SQL_BIGINT:
      col_desc->c_data_type = SQL_C_SBIGINT;
      col_desc->length = sizeof(SQLBIGINT);
      break;
    // case SQL_DECFLOAT:
    //   col_desc->c_data_type = SQL_C_CHAR;
    //   break;
    case SQL_DECIMAL:
    case SQL_NUMERIC:
      col_desc->c_data_type = SQL_C_CHAR;
      // TODO:
      // - this calculation is incorrect
      // - it needs to take into account the scale
      // - + (precision if decimal digits > 0)
      // - + (decimal point if decimal digit > 0)
      // - + (newline???)
      col_desc->length = col_desc->size + sizeof(SQLCHAR);
      break;
    case SQL_DOUBLE:
    case SQL_FLOAT:
      col_desc->c_data_type = SQL_C_DOUBLE;
      col_desc->length = sizeof(double);
      break;
    case SQL_REAL:
      col_desc->c_data_type = SQL_C_FLOAT;
      col_desc->length = sizeof(float);
      break;
    case SQL_CHAR:
    // case SQL_CLOB:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
      col_desc->c_data_type = SQL_C_CHAR;
      col_desc->length = col_desc->size + sizeof(SQLCHAR);
      break;
    case SQL_BINARY:
    // case SQL_BLOB:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
      col_desc->c_data_type = SQL_C_BINARY;
      col_desc->length = col_desc->size + sizeof(SQLCHAR);
      break;
    // case SQL_BLOB_LOCATOR:
    //   col_desc->c_data_type = SQL_C_BLOB_LOCATOR;
    //   break;
    // case SQL_DBCLOB:
    // case SQL_GRAPHIC:
    // case SQL_LONGVARGRAPHIC:
    // case SQL_VARGRAPHIC:
    //   col_desc->c_data_type = SQL_C_DBCHAR;
    //   break;
    // case SQL_DBCLOB_LOCATOR:
    //   col_desc->c_data_type = SQL_C_DBCLOB_LOCATOR;
    //   break;
    // case SQL_CLOB_LOCATOR:
    //   col_desc->c_data_type = SQL_C_CLOB_LOCATOR;
    //   break;
    // case SQL_ROWID:
    //   col_desc->c_data_type = SQL_C_CHAR;
    //   break;
    case SQL_TYPE_DATE:
      col_desc->c_data_type = SQL_C_TYPE_DATE;
      col_desc->length = col_desc->size + sizeof(SQLCHAR);
      break;
    case SQL_TYPE_TIME:
      col_desc->c_data_type = SQL_C_TYPE_TIME;
      col_desc->length = col_desc->size + sizeof(SQLCHAR);
      break;
    case SQL_TYPE_TIMESTAMP:
      col_desc->c_data_type = SQL_C_TYPE_TIMESTAMP;
      col_desc->length = col_desc->size + sizeof(SQLCHAR);
      break;
    // case SQL_XML:
    //   col_desc->c_data_type = SQL_C_BINARY;
    //   break;
    default:
      throw Exception("OdbcSqlDataTypeToCDataType() unknown sql_data_type=" +
                      std::to_string(col_desc->sql_data_type));
      break;
    }
  }
};
} // namespace duckdb
