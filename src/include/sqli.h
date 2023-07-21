// Copied from OTL library for extended SQL types
// https://github.com/GerHobbelt/otl

#ifndef __FAKED_DB2_H__
#define __FAKED_DB2_H__


// [i_a] how to shut up ex674_db2
// somehow the whole DB2 stuff is foobarred for 64-bit builds (AMD+Itanium) RRRAAAGGHH!




#include <windows.h>
#include <sqlext.h>
#include <sql.h>










#if 0



/******************************************************************************
 *
 * Source File Name = sqlcli1.h
 *
 * (C) COPYRIGHT International Business Machines Corp. 1993, 2004
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Function = Include File defining:
 *              DB2 CLI Interface - Constants
 *              DB2 CLI Interface - Function Prototypes
 *
 * Operating System = Common C Include File
 *
 *****************************************************************************/

#ifndef SQL_H_SQLCLI1
   #define SQL_H_SQLCLI1           /* Permit duplicate Includes */

/* Prevent inclusion of winsock.h in windows.h */
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#define DB2_WINSOCKAPI_
#endif

/* ODBC64 should be used instead of CLI_WIN64 for linking with libdb2o.dll */
#ifndef ODBC64
#ifdef CLI_WIN64
#define ODBC64
#endif
#endif

//#include "sqlsystm.h"              /* System dependent defines  */


#if defined(DB2NT)
#include <windows.h>
#endif

//#include    "sqlca.h"
/******************************************************************************
 *
 * Source File Name = SQLCA.H
 *
 * (C) COPYRIGHT International Business Machines Corp. 1987, 1997
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Function = Include File defining:
 *              SQL Communications Area
 *
 * Operating System = Common C Include File
 *
 *****************************************************************************/


#if !(defined(SQLCODE) || defined(SQLCADE)) /* Permit Duplicate Includes  */

//#include "sqlsystm.h"                      /* System dependent defines   */
/******************************************************************************
 *
 * Source File Name = SQLSYSTM.H
 *
 *
 * (C) COPYRIGHT International Business Machines Corp. 1993
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Function = Include File defining:
 *              Operating System Specific Information
 *
 * Operating System = NT 3.5
 *
 *****************************************************************************/

#ifndef  SQL_H_SQLSYSTM
   #define SQL_H_SQLSYSTM          /* Permit duplicate Includes */


#if !defined DB2NT
  #define DB2NT     1
#endif


#if !defined SQL_API_RC
 #define SQL_API_RC      int
 #define SQL_STRUCTURE   struct
 #define SQL_POINTER
 #define SQL_API_FN      __stdcall
 #define SQL_API_INTR    __stdcall
 #define PSQL_API_FN     __stdcall *
#endif

/******************************************************************************
**
** The SQLOLDCHAR macro may be used to maintain compatibility between
** version 1 applications and version 2 header files.  In version 1, many
** strings were declared as 'unsigned char'.  In keeping with the spirit
** of ANSI C, all character data, structure members and function
** parameters with string semantics have been changed to 'char' in version 2.
** This change may produce type conflicts with some compilers.  Adding
** -DSQLOLDCHAR to the compile command will cause the changed items to
** revert to their version 1 types.  Note that this should be used for
** compatibility purposes only.  New code should be written using plain
** 'char' where indicated in the documentation.
******************************************************************************/

#undef _SQLOLDCHAR
#if defined SQLOLDCHAR
 #define _SQLOLDCHAR     unsigned char
#else
 #define _SQLOLDCHAR     char
#endif

/******************************************************************************
**
** Define fixed size integer types.
**
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


typedef char            sqlint8;
typedef unsigned char   sqluint8;

typedef short           sqlint16;
typedef unsigned short  sqluint16;

#if defined DB2_FORCE_INT32_TYPES_TO_INT
   typedef int             sqlint32;
   typedef unsigned int    sqluint32;
#else
   typedef long            sqlint32;
   typedef unsigned long   sqluint32;
#endif

#if !defined SQL_BIGINT_TYPE
   #if defined _MSC_VER
      #define SQL_BIGINT_TYPE __int64
      #define DB2_CONSTRUCT_BIGINT_CONSTANT(db2BigIntConstantValue) db2BigIntConstantValue##i64
   #else
      #define DB2_CONSTRUCT_BIGINT_CONSTANT(db2BigIntConstantValue) db2BigIntConstantValue##LL
      #define SQL_BIGINT_TYPE long long
   #endif
#endif
#if !defined SQL_BIGUINT_TYPE
   #if defined _MSC_VER
      #define SQL_BIGUINT_TYPE unsigned __int64
   #else
      #define SQL_BIGUINT_TYPE unsigned long long
   #endif
#endif
typedef SQL_BIGINT_TYPE  sqlint64;
typedef SQL_BIGUINT_TYPE sqluint64;

/******************************************************************************
**
** The sqlintptr and sqluintptr are defined as integer types large enough
** to contain pointer values on this platform.
**
******************************************************************************/

#if defined _WIN64
   #define db2Is64bit

   typedef sqlint64        sqlintptr;
   typedef sqluint64       sqluintptr;
#else
   typedef sqlint32        sqlintptr;
   typedef sqluint32       sqluintptr;
#endif

#ifdef __cplusplus
}
#endif


#endif /* SQL_H_SQLSYSTM */













#ifdef __cplusplus
   extern "C" {
#endif


#if defined(DB2NT)
#if defined _WIN64
#pragma pack(8)
#else
#pragma pack(4)
#endif
#elif (defined(DB2AIX) && defined(__64BIT__))
#pragma options align=natural
#elif (defined(DB2AIX))
#pragma options align=power
#endif

/* SQL Communication Area - SQLCA */
/* _SQLOLDCHAR defaults to 'char'.  See sqlsystm.h for details. */

SQL_STRUCTURE  sqlca
{
   _SQLOLDCHAR    sqlcaid[8];           /* Eyecatcher = 'SQLCA   '        */
   sqlint32       sqlcabc;              /* SQLCA size in bytes = 136      */
#ifdef DB2_SQL92E
   sqlint32       sqlcade;              /* SQL return code                */
#else
   sqlint32       sqlcode;              /* SQL return code                */
#endif
   short          sqlerrml;             /* Length for SQLERRMC            */
   _SQLOLDCHAR    sqlerrmc[70];         /* Error message tokens           */

   _SQLOLDCHAR    sqlerrp[8];           /* Diagnostic information         */

   sqlint32       sqlerrd[6];           /* Diagnostic information         */
   _SQLOLDCHAR    sqlwarn[11];          /* Warning flags                  */

#ifdef DB2_SQL92E
   _SQLOLDCHAR    sqlstat[5];           /* State corresponding to SQLCODE */
#else
   _SQLOLDCHAR    sqlstate[5];          /* State corresponding to SQLCODE */
#endif

};

#ifdef DB2_SQL92E
#define   SQLCADE        sqlca.sqlcade
#else
#define   SQLCODE        sqlca.sqlcode
#endif
#define   SQLWARN0       sqlca.sqlwarn[0]
#define   SQLWARN1       sqlca.sqlwarn[1]
#define   SQLWARN2       sqlca.sqlwarn[2]
#define   SQLWARN3       sqlca.sqlwarn[3]
#define   SQLWARN4       sqlca.sqlwarn[4]
#define   SQLWARN5       sqlca.sqlwarn[5]
#define   SQLWARN6       sqlca.sqlwarn[6]
#define   SQLWARN7       sqlca.sqlwarn[7]
#define   SQLWARN8       sqlca.sqlwarn[8]
#define   SQLWARN9       sqlca.sqlwarn[9]
#define   SQLWARNA       sqlca.sqlwarn[10]


/* sqlerrd tokens updated when compound SQL statements processed */

#define   SQL_CMP_NA_ERRORS       1
#define   SQL_CMP_ROWS_AFFECTED   2
#define   SQL_CMP_STMTS_COMPLETED 3
#define   SQL_CMP_REF_INT_ROWS    4


/* sqlerrd tokens updated when CONNECT statements processed */

#define   SQL_CONNECT_DB_APP2DB_CONVFACTOR     0
#define   SQL_CONNECT_DB_DB2APP_CONVFACTOR     1
#define   SQL_CONNECT_DB_UPDATEABILITY_IN_UOW  2
#define   SQL_CONNECT_DB_COMMIT_TYPE           3


/* Values returned for sqlerrd[SQL_CONNECT_DB_UPDATEABILITY_IN_UOW]     */

#define   SQL_DB_UPDATEABLE                    1
#define   SQL_DB_READ_ONLY                     2


/* Values returned for sqlerrd[SQL_CONNECT_DB_COMMIT_TYPE]       */

#define   SQL_DB_ONE_PHASE_COMMIT              1
#define   SQL_DB_ONE_PHASE_READ_ONLY           2
#define   SQL_DB_TWO_PHASE_COMMIT              3

/* sqlerrd tokens updated when MPP statements processed       */
#define SQL_ERRD_NODE_NUM 1

#if defined(DB2NT)
#pragma pack()
#elif defined(DB2AIX)
#pragma options align=reset
#endif

#endif /* SQLCODE */

#ifdef __cplusplus
}
#endif








//#include    "sqlcli.h"
/******************************************************************************
 *
 * Source File Name = sqlcli.h
 *
 * (C) COPYRIGHT International Business Machines Corp. 1993, 1999
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Function = Include File defining:
 *              DB2 CLI Interface - Constants
 *              DB2 CLI Interface - Data Structures
 *              DB2 CLI Interface - Function Prototypes
 *
 * Operating System = Common C Include File
 *
 *****************************************************************************/

//#ifndef SQL_H_SQLCLI
//#define SQL_H_SQLCLI             /* Permit duplicate Includes */

/* Prevent inclusion of winsock.h in windows.h */
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#define DB2_WINSOCKAPI_
#endif

/* DB2CLI_VER            DB2 Call Level Interface Version Number (0x0210).
 *                       To revert to Version 1.0 definitions,
 *                       issue #define DB2CLI_VER 0x0110 before including
 *                       sqlcli.h and sqlcli1.h
 */

/* If DB2CLI_VER is not defined, assume version 2.10 */
#ifndef DB2CLI_VER
#define DB2CLI_VER 0x0310
#endif

/* ODBC64 should be used instead of CLI_WIN64 for linking with libdb2o.dll */
#ifndef ODBC64
#ifdef CLI_WIN64
#define ODBC64
#endif
#endif

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//#include "sqlsystm.h"                  /* System dependent defines  */




/* generally useful constants */
#define  SQL_MAX_ID_LENGTH        128  /* maximum identifier name size,
                                          e.g. cursor names               */

/* date/time length constants */
#define SQL_DATE_LEN           10
#define SQL_TIME_LEN            8  /* add P+1 if precision is nonzero */
#define SQL_TIMESTAMP_LEN      19  /* add P+1 if precision is nonzero */

/* handle type identifiers */
#define SQL_HANDLE_ENV             1
#define SQL_HANDLE_DBC             2
#define SQL_HANDLE_STMT            3
#define SQL_HANDLE_DESC            4

/* RETCODE values             */
#define  SQL_SUCCESS             0
#define  SQL_SUCCESS_WITH_INFO   1
#define  SQL_NEED_DATA           99
#define  SQL_NO_DATA             100
#define  SQL_STILL_EXECUTING     2

/* test for SQL_SUCCESS or SQL_SUCCESS_WITH_INFO */
#define SQL_SUCCEEDED(rc) (((rc)&(~1))==0)

/* SQLFreeStmt option values  */
#define  SQL_CLOSE               0
#define  SQL_DROP                1
#define  SQL_UNBIND              2
#define  SQL_RESET_PARAMS        3

/* SQLTransact option values  */
#define  SQL_COMMIT              0
#define  SQL_ROLLBACK            1

/* Standard SQL data types */
#define  SQL_UNKNOWN_TYPE        0
#define  SQL_CHAR                1
#define  SQL_NUMERIC             2
#define  SQL_DECIMAL             3
#define  SQL_INTEGER             4
#define  SQL_SMALLINT            5
#define  SQL_FLOAT               6
#define  SQL_REAL                7
#define  SQL_DOUBLE              8
#define  SQL_DATETIME            9
#define  SQL_VARCHAR            12
#define  SQL_WCHAR              (-8)
#define  SQL_WVARCHAR           (-9)
#define  SQL_WLONGVARCHAR       (-10)
#define  SQL_DECFLOAT           (-360)
/* One-parameter shortcuts for date/time data types */
#define SQL_TYPE_DATE      91
#define SQL_TYPE_TIME      92
#define SQL_TYPE_TIMESTAMP 93

/* Statement attribute values for cursor sensitivity */
#define SQL_UNSPECIFIED     0
#define SQL_INSENSITIVE     1
#define SQL_SENSITIVE       2

/* Default conversion code for SQLBindCol(), SQLBindParam() and SQLGetData() */
#define SQL_DEFAULT        99

/* SQLGetData() code indicating that the application row descriptor
 * specifies the data type
 */
#define SQL_ARD_TYPE      (-99)

/* SQL date/time type subcodes */
#define SQL_CODE_DATE       1
#define SQL_CODE_TIME       2
#define SQL_CODE_TIMESTAMP  3

/* SQL extended data types */
#define  SQL_GRAPHIC            -95
#define  SQL_VARGRAPHIC         -96
#define  SQL_LONGVARGRAPHIC     -97
#define  SQL_BLOB               -98
#define  SQL_CLOB               -99
#define  SQL_DBCLOB             -350
#define  SQL_XML                -370
#define  SQL_DATALINK           -400
#define  SQL_USER_DEFINED_TYPE  -450

/* C data type to SQL data type mapping */
#define  SQL_C_DBCHAR         SQL_DBCLOB
#define  SQL_C_DECIMAL_IBM    SQL_DECIMAL
#define  SQL_C_DATALINK       SQL_C_CHAR
#define  SQL_C_PTR            2463
#define  SQL_C_DECIMAL_OLEDB  2514
#define  SQL_C_DECIMAL64      SQL_DECFLOAT
#define  SQL_C_DECIMAL128     -361

/*
 *  locator type identifier
 */

#define SQL_BLOB_LOCATOR       31
#define SQL_CLOB_LOCATOR       41
#define SQL_DBCLOB_LOCATOR    -351

/*
 * C Data Type for the LOB locator types
 */
#define SQL_C_BLOB_LOCATOR     SQL_BLOB_LOCATOR
#define SQL_C_CLOB_LOCATOR     SQL_CLOB_LOCATOR
#define SQL_C_DBCLOB_LOCATOR   SQL_DBCLOB_LOCATOR

/*
 * NULL status defines; these are used in SQLColAttributes, SQLDescribeCol,
 * to describe the nullability of a column in a table.
 */

#define  SQL_NO_NULLS         0
#define  SQL_NULLABLE         1
#define  SQL_NULLABLE_UNKNOWN 2

/* values of UNNAMED field in descriptor used in SQLColAttribute */
#define SQL_NAMED             0
#define SQL_UNNAMED           1

/* values of ALLOC_TYPE field in descriptor */
#define SQL_DESC_ALLOC_AUTO 1
#define SQL_DESC_ALLOC_USER 2

/* values of USER_DEFINED_TYPE_CODE */
#define SQL_TYPE_BASE            0
#define SQL_TYPE_DISTINCT        1
#define SQL_TYPE_STRUCTURED      2
#define SQL_TYPE_REFERENCE       3

/* Special length values  */

/* SQLColAttributes defines */
#define  SQL_COLUMN_SCHEMA_NAME      16
#define  SQL_COLUMN_CATALOG_NAME     17
#define  SQL_COLUMN_DISTINCT_TYPE    1250
#define  SQL_DESC_DISTINCT_TYPE      SQL_COLUMN_DISTINCT_TYPE
#define  SQL_COLUMN_REFERENCE_TYPE   1251
#define  SQL_DESC_REFERENCE_TYPE     SQL_COLUMN_REFERENCE_TYPE
#define  SQL_DESC_STRUCTURED_TYPE    1252
#define  SQL_DESC_USER_TYPE          1253
#define  SQL_DESC_BASE_TYPE          1254
#define  SQL_DESC_KEY_TYPE           1255
#define  SQL_DESC_KEY_MEMBER         1266

/* identifiers of fields in the SQL descriptor */
#define SQL_DESC_COUNT                  1001
#define SQL_DESC_TYPE                   1002
#define SQL_DESC_LENGTH                 1003
#define SQL_DESC_OCTET_LENGTH_PTR       1004
#define SQL_DESC_PRECISION              1005
#define SQL_DESC_SCALE                  1006
#define SQL_DESC_DATETIME_INTERVAL_CODE 1007
#define SQL_DESC_NULLABLE               1008
#define SQL_DESC_INDICATOR_PTR          1009
#define SQL_DESC_DATA_PTR               1010
#define SQL_DESC_NAME                   1011
#define SQL_DESC_UNNAMED                1012
#define SQL_DESC_OCTET_LENGTH           1013
#define SQL_DESC_ALLOC_TYPE             1099
#define SQL_DESC_USER_DEFINED_TYPE_CODE 1098


/* Defines for SQL_DESC_KEY_TYPE */
#define  SQL_KEYTYPE_NONE             0
#define  SQL_KEYTYPE_PRIMARYKEY       1
#define  SQL_KEYTYPE_UNIQUEINDEX      2


/* SQLColAttribute defines for SQL_COLUMN_UPDATABLE condition */
#define  SQL_UPDT_READONLY            0
#define  SQL_UPDT_WRITE               1
#define  SQL_UPDT_READWRITE_UNKNOWN   2

/*
 * SQLColAttribute defines for SQL_COLUMN_SEARCHABLE condition.
 */
#define  SQL_PRED_NONE         0
#define  SQL_PRED_CHAR         1
#define  SQL_PRED_BASIC        2

/* NULL handle defines    */

/* identifiers of fields in the diagnostics area */
#define SQL_DIAG_RETURNCODE        1
#define SQL_DIAG_NUMBER            2
#define SQL_DIAG_ROW_COUNT         3
#define SQL_DIAG_SQLSTATE          4
#define SQL_DIAG_NATIVE            5
#define SQL_DIAG_MESSAGE_TEXT      6
#define SQL_DIAG_DYNAMIC_FUNCTION  7
#define SQL_DIAG_CLASS_ORIGIN      8
#define SQL_DIAG_SUBCLASS_ORIGIN   9
#define SQL_DIAG_CONNECTION_NAME  10
#define SQL_DIAG_SERVER_NAME      11
#define SQL_DIAG_DYNAMIC_FUNCTION_CODE 12

/* dynamic function codes */
#define SQL_DIAG_ALTER_TABLE            4
#define SQL_DIAG_CALL                                   7
#define SQL_DIAG_CREATE_INDEX          (-1)
#define SQL_DIAG_CREATE_TABLE          77
#define SQL_DIAG_CREATE_VIEW           84
#define SQL_DIAG_DELETE_WHERE          19
#define SQL_DIAG_DROP_INDEX            (-2)
#define SQL_DIAG_DROP_TABLE            32
#define SQL_DIAG_DROP_VIEW             36
#define SQL_DIAG_DYNAMIC_DELETE_CURSOR 38
#define SQL_DIAG_DYNAMIC_UPDATE_CURSOR 81
#define SQL_DIAG_GRANT                 48
#define SQL_DIAG_INSERT                50
#define SQL_DIAG_MERGE                128
#define SQL_DIAG_REVOKE                59
#define SQL_DIAG_SELECT_CURSOR         85
#define SQL_DIAG_UNKNOWN_STATEMENT      0
#define SQL_DIAG_UPDATE_WHERE          82

/*
 *  IBM specific SQLGetDiagField values.
 */

#define SQL_DIAG_DEFERRED_PREPARE_ERROR   1279

/* SQL_DIAG_ROW_NUMBER values */
#define SQL_ROW_NO_ROW_NUMBER                   (-1)
#define SQL_ROW_NUMBER_UNKNOWN                  (-2)

/* SQL_DIAG_COLUMN_NUMBER values */
#define SQL_COLUMN_NO_COLUMN_NUMBER             (-1)
#define SQL_COLUMN_NUMBER_UNKNOWN               (-2)

/*
 * The following are provided to enhance portability and compatibility
 * with ODBC
 */

typedef           signed   char         SCHAR;
typedef           unsigned char         UCHAR;

typedef           short    int          SWORD;
typedef  unsigned short                 USHORT;

typedef  signed   short                 SSHORT;
typedef  unsigned short    int          UWORD;

#if defined(DB2NT)
   typedef                 long         SDWORD;
   typedef        unsigned long         ULONG;
   typedef        unsigned long         UDWORD;
   typedef                 long         SLONG;
#else
   typedef                 sqlint32     SDWORD;
   typedef                 sqluint32    ULONG;
   typedef                 sqluint32    UDWORD;
   typedef                 sqlint32     SLONG;
#endif
typedef                    double       SDOUBLE;
typedef                    float        SFLOAT;
typedef  unsigned          char         SQLDATE;
typedef  unsigned          char         SQLTIME;
typedef  unsigned          char         SQLTIMESTAMP;
typedef  unsigned          char         SQLDECIMAL;
typedef  unsigned          char         SQLNUMERIC;



#if defined(WINDOWS)
typedef  long     double                LDOUBLE;
#else
typedef           double                LDOUBLE;
#endif

typedef void FAR *         PTR;
typedef void FAR *         HENV;
typedef void FAR *         HDBC;
typedef void FAR *         HSTMT;

typedef signed short       RETCODE;



/* SQL portable types for C  */
typedef  UCHAR             SQLCHAR;
typedef  UCHAR             SQLVARCHAR;
typedef  SCHAR             SQLSCHAR;
typedef  SDWORD            SQLINTEGER;
typedef  SWORD             SQLSMALLINT;
typedef  SDOUBLE           SQLDOUBLE;
typedef  SDOUBLE           SQLFLOAT;
typedef  SFLOAT            SQLREAL;

typedef  SQLSMALLINT       SQLRETURN;

#if (DB2CLI_VER >= 0x0200)
typedef  UDWORD            SQLUINTEGER;
typedef  UWORD             SQLUSMALLINT;
#else
typedef  SQLINTEGER        SQLUINTEGER;
typedef  SQLSMALLINT       SQLUSMALLINT;
#endif

/* 64-bit Length Defines */
#ifdef ODBC64
typedef sqlint64        SQLLEN;
typedef sqluint64       SQLULEN;
typedef sqluint64       SQLSETPOSIROW;
#else
#define SQLLEN          SQLINTEGER
#define SQLULEN         SQLUINTEGER
#define SQLSETPOSIROW   SQLUSMALLINT
#endif

typedef  PTR               SQLPOINTER;

/*
 * Double Byte Character Set support
 */

/*
 * Do not support SQL_WCHART_CONVERT in UNICODE
 */
#ifdef UNICODE
#undef SQL_WCHART_CONVERT
#endif

#ifdef SQL_WCHART_CONVERT
typedef  wchar_t           SQLDBCHAR;
#else
typedef  unsigned short    SQLDBCHAR;
#endif


#ifdef DB2WIN

typedef  SQLINTEGER        SQLHANDLE;
typedef  HENV              SQLHENV;
typedef  HDBC              SQLHDBC;
typedef  HSTMT             SQLHSTMT;
typedef  HWND              SQLHWND;

#else
#if ((defined DB2NT && defined _WIN64) || defined ODBC64)
typedef  void *            SQLHANDLE;
typedef  SQLHANDLE         SQLHENV;
typedef  SQLHANDLE         SQLHDBC;
typedef  SQLHANDLE         SQLHSTMT;
#else

#ifndef __SQLTYPES
typedef  SQLINTEGER        SQLHANDLE;
typedef  SQLINTEGER        SQLHENV;
typedef  SQLINTEGER        SQLHDBC;
typedef  SQLINTEGER        SQLHSTMT;
#endif
#endif

#if defined (DB2NT)
typedef  HWND              SQLHWND;
#else
typedef  SQLPOINTER        SQLHWND;
#endif

#endif
typedef  SQLHANDLE         SQLHDESC;

#ifndef __SQLTYPES

/*
 * SQL_NO_NATIVE_BIGINT_SUPPORT and SQL_BIGINT_TYPE are defined in sqlsystm.h
 *
 */

#if defined(SQL_NO_NATIVE_BIGINT_SUPPORT)
typedef struct  SQLBIGINT
{
        SQLUINTEGER dwLowWord;
        SQLINTEGER  dwHighWord;
} SQLBIGINT;
typedef struct  SQLUBIGINT
{
        SQLUINTEGER dwLowWord;
        SQLUINTEGER dwHighWord;
} SQLUBIGINT;
#elif defined(SQL_BIGINT_TYPE)
typedef SQL_BIGINT_TYPE   SQLBIGINT;
typedef SQL_BIGUINT_TYPE  SQLUBIGINT;
#endif

typedef  struct DATE_STRUCT
  {
    SQLSMALLINT    year;
    SQLUSMALLINT   month;
    SQLUSMALLINT   day;
  } DATE_STRUCT;

typedef DATE_STRUCT SQL_DATE_STRUCT;

typedef  struct TIME_STRUCT
  {
    SQLUSMALLINT   hour;
    SQLUSMALLINT   minute;
    SQLUSMALLINT   second;
  } TIME_STRUCT;

typedef TIME_STRUCT SQL_TIME_STRUCT;

typedef  struct TIMESTAMP_STRUCT
  {
    SQLSMALLINT    year;
    SQLUSMALLINT   month;
    SQLUSMALLINT   day;
    SQLUSMALLINT   hour;
    SQLUSMALLINT   minute;
    SQLUSMALLINT   second;
    SQLUINTEGER    fraction;     /* fraction of a second */
  } TIMESTAMP_STRUCT;


typedef TIMESTAMP_STRUCT SQL_TIMESTAMP_STRUCT;

typedef enum
{
        SQL_IS_YEAR                                             = 1,
        SQL_IS_MONTH                                    = 2,
        SQL_IS_DAY                                              = 3,
        SQL_IS_HOUR                                             = 4,
        SQL_IS_MINUTE                                   = 5,
        SQL_IS_SECOND                                   = 6,
        SQL_IS_YEAR_TO_MONTH                    = 7,
        SQL_IS_DAY_TO_HOUR                              = 8,
        SQL_IS_DAY_TO_MINUTE                    = 9,
        SQL_IS_DAY_TO_SECOND                    = 10,
        SQL_IS_HOUR_TO_MINUTE                   = 11,
        SQL_IS_HOUR_TO_SECOND                   = 12,
        SQL_IS_MINUTE_TO_SECOND                 = 13
} SQLINTERVAL;

typedef struct tagSQL_YEAR_MONTH
{
                SQLUINTEGER             year;
                SQLUINTEGER             month;
} SQL_YEAR_MONTH_STRUCT;

typedef struct tagSQL_DAY_SECOND
{
                SQLUINTEGER             day;
                SQLUINTEGER             hour;
                SQLUINTEGER             minute;
                SQLUINTEGER             second;
                SQLUINTEGER             fraction;
} SQL_DAY_SECOND_STRUCT;

typedef struct tagSQL_INTERVAL_STRUCT
{
        SQLINTERVAL             interval_type;
        SQLSMALLINT             interval_sign;
        union {
                SQL_YEAR_MONTH_STRUCT           year_month;
                SQL_DAY_SECOND_STRUCT           day_second;
        } intval;

} SQL_INTERVAL_STRUCT;

/* Maximum precision (in base 10) of an SQL_C_NUMERIC value */
#define SQL_MAX_C_NUMERIC_PRECISION     38

/* internal representation of numeric data type */
#define SQL_MAX_NUMERIC_LEN             16
typedef struct tagSQL_NUMERIC_STRUCT
{
        SQLCHAR         precision;
        SQLSCHAR        scale;
        SQLCHAR         sign;   /* 1 if positive, 0 if negative */
        SQLCHAR         val[SQL_MAX_NUMERIC_LEN];
} SQL_NUMERIC_STRUCT;

#endif


#define SQL_DECIMAL64_LEN               8
#define SQL_DECIMAL128_LEN              16

typedef struct tagSQLDECIMAL64 {
    union {
              SQLDOUBLE dummy;   /* Dummy member for alignment purposes */
              SQLCHAR   dec64[SQL_DECIMAL64_LEN];
          } udec64;
} SQLDECIMAL64;

typedef struct tagSQLDECIMAL128 {
    union {
              SQLDOUBLE dummy;   /* Dummy member for alignment purposes */
              SQLCHAR   dec128[SQL_DECIMAL128_LEN];
          } udec128;
} SQLDECIMAL128;

/* Core Function Prototypes  */




SQLRETURN SQL_API_FN  SQLAllocConnect  (SQLHENV           henv,
                                        SQLHDBC     FAR   *phdbc);


SQLRETURN SQL_API_FN  SQLAllocEnv      (SQLHENV     FAR   *phenv);

SQLRETURN SQL_API_FN  SQLAllocStmt     (SQLHDBC           hdbc,
                                        SQLHSTMT    FAR   *phstmt);

SQLRETURN SQL_API_FN SQLAllocHandle(    SQLSMALLINT fHandleType,
                                        SQLHANDLE hInput,
                                        SQLHANDLE * phOutput );


SQLRETURN SQL_API_FN  SQLCancel        (SQLHSTMT          hstmt);





SQLRETURN SQL_API_FN  SQLConnect       (SQLHDBC           hdbc,
                                        SQLCHAR     FAR   *szDSN,
                                        SQLSMALLINT       cbDSN,
                                        SQLCHAR     FAR   *szUID,
                                        SQLSMALLINT       cbUID,
                                        SQLCHAR     FAR   *szAuthStr,
                                        SQLSMALLINT       cbAuthStr);


SQLRETURN SQL_API_FN  SQLDisconnect    (SQLHDBC           hdbc);

SQLRETURN SQL_API_FN  SQLError         (SQLHENV           henv,
                                        SQLHDBC           hdbc,
                                        SQLHSTMT          hstmt,
                                        SQLCHAR     FAR   *szSqlState,
                                        SQLINTEGER  FAR   *pfNativeError,
                                        SQLCHAR     FAR   *szErrorMsg,
                                        SQLSMALLINT       cbErrorMsgMax,
                                        SQLSMALLINT FAR   *pcbErrorMsg);

SQLRETURN SQL_API_FN  SQLExecDirect    (SQLHSTMT          hstmt,
                                        SQLCHAR     FAR   *szSqlStr,
                                        SQLINTEGER        cbSqlStr);

SQLRETURN SQL_API_FN  SQLExecute       (SQLHSTMT          hstmt);

SQLRETURN SQL_API_FN  SQLFetch         (SQLHSTMT          hstmt);

SQLRETURN SQL_API_FN  SQLFreeConnect   (SQLHDBC           hdbc);

SQLRETURN SQL_API_FN  SQLFreeEnv       (SQLHENV           henv);

SQLRETURN SQL_API_FN  SQLFreeStmt      (SQLHSTMT          hstmt,
                                        SQLUSMALLINT      fOption);

SQLRETURN SQL_API_FN SQLCloseCursor(    SQLHSTMT hStmt );

SQLRETURN SQL_API_FN  SQLGetCursorName (SQLHSTMT          hstmt,
                                        SQLCHAR     FAR   *szCursor,
                                        SQLSMALLINT       cbCursorMax,
                                        SQLSMALLINT FAR   *pcbCursor);


SQLRETURN SQL_API_FN  SQLNumResultCols (SQLHSTMT          hstmt,
                                        SQLSMALLINT FAR   *pccol);

SQLRETURN SQL_API_FN  SQLPrepare       (SQLHSTMT          hstmt,
                                        SQLCHAR     FAR   *szSqlStr,
                                        SQLINTEGER        cbSqlStr);


SQLRETURN SQL_API_FN  SQLSetCursorName (SQLHSTMT          hstmt,
                                        SQLCHAR     FAR   *szCursor,
                                        SQLSMALLINT       cbCursor);


SQLRETURN SQL_API_FN  SQLTransact      (SQLHENV           henv,
                                        SQLHDBC           hdbc,
                                        SQLUSMALLINT      fType);

SQLRETURN SQL_API_FN SQLEndTran(        SQLSMALLINT fHandleType,
                                        SQLHANDLE hHandle,
                                        SQLSMALLINT fType );

SQLRETURN SQL_API_FN SQLFreeHandle(     SQLSMALLINT fHandleType,
                                        SQLHANDLE hHandle );

SQLRETURN SQL_API_FN SQLGetDiagRec(     SQLSMALLINT fHandleType,
                                        SQLHANDLE hHandle,
                                        SQLSMALLINT iRecNumber,
                                        SQLCHAR * pszSqlState,
                                        SQLINTEGER * pfNativeError,
                                        SQLCHAR * pszErrorMsg,
                                        SQLSMALLINT cbErrorMsgMax,
                                        SQLSMALLINT * pcbErrorMsg );

SQLRETURN SQL_API_FN SQLGetDiagField(   SQLSMALLINT fHandleType,
                                        SQLHANDLE hHandle,
                                        SQLSMALLINT iRecNumber,
                                        SQLSMALLINT fDiagIdentifier,
                                        SQLPOINTER pDiagInfo,
                                        SQLSMALLINT cbDiagInfoMax,
                                        SQLSMALLINT * pcbDiagInfo );

SQLRETURN  SQL_API_FN SQLCopyDesc(      SQLHDESC hDescSource,
                                        SQLHDESC hDescTarget );

SQLRETURN  SQL_API_FN SQLGetDescField(  SQLHDESC DescriptorHandle,
                                        SQLSMALLINT RecNumber,
                                        SQLSMALLINT FieldIdentifier,
                                        SQLPOINTER Value,
                                        SQLINTEGER BufferLength,
                                        SQLINTEGER *StringLength);


SQLRETURN  SQL_API_FN SQLSetDescField(  SQLHDESC DescriptorHandle,
                                        SQLSMALLINT RecNumber,
                                        SQLSMALLINT FieldIdentifier,
                                        SQLPOINTER Value,
                                        SQLINTEGER BufferLength);




#ifdef __cplusplus
}
#endif

/*
 * Include ODBC header files for
 * functions that are not specified in the X/Open Call Level Interface.
 * This is included with permission from Microsoft.
 * Do not modify (i.e. must not add, remove, rearrange) any part of the
 * contents of sqlext.h
 * Note: SQLDrivers is not supported by DB2 CLI.
 */
#ifndef __SQL
#define __SQL
#define ODBCVER 0x0351
#endif
#if !defined(WINDOWS) && !defined(WIN32) &&  !defined(SQLWINT)
typedef SQLWCHAR * LPWSTR;
typedef sqluint32  DWORD;
#endif

#include <sqlext.h>


#ifdef DB2_WINSOCKAPI_
#undef _WINSOCKAPI_
#undef DB2_WINSOCKAPI_
#endif

//#endif /* SQL_H_SQLCLI */











#ifdef __cplusplus
extern "C" {
#endif

/* SQLGetFunction defines  - supported functions */
#define  SQL_API_SQLALLOCCONNECT        1
#define  SQL_API_SQLALLOCENV            2
#define  SQL_API_SQLALLOCSTMT           3
#define  SQL_API_SQLBINDCOL             4
#define  SQL_API_SQLBINDPARAM           1002
#define  SQL_API_SQLCANCEL              5
#define  SQL_API_SQLCONNECT             7
#define  SQL_API_SQLCOPYDESC            1004
#define  SQL_API_SQLDESCRIBECOL         8
#define  SQL_API_SQLDISCONNECT          9
#define  SQL_API_SQLERROR              10
#define  SQL_API_SQLEXECDIRECT         11
#define  SQL_API_SQLEXECUTE            12
#define  SQL_API_SQLFETCH              13
#define  SQL_API_SQLFREECONNECT        14
#define  SQL_API_SQLFREEENV            15
#define  SQL_API_SQLFREESTMT           16
#define  SQL_API_SQLGETCURSORNAME      17
#define  SQL_API_SQLNUMRESULTCOLS      18
#define  SQL_API_SQLPREPARE            19
#define  SQL_API_SQLROWCOUNT           20
#define  SQL_API_SQLSETCURSORNAME      21
#define  SQL_API_SQLSETDESCFIELD       1017
#define  SQL_API_SQLSETDESCREC         1018
#define  SQL_API_SQLSETENVATTR         1019
#define  SQL_API_SQLSETPARAM           22
#define  SQL_API_SQLTRANSACT           23

#define  SQL_API_SQLCOLUMNS            40
#define  SQL_API_SQLGETCONNECTOPTION   42
#define  SQL_API_SQLGETDATA            43
#define  SQL_API_SQLGETDATAINTERNAL    174
#define  SQL_API_SQLGETDESCFIELD       1008
#define  SQL_API_SQLGETDESCREC         1009
#define  SQL_API_SQLGETDIAGFIELD       1010
#define  SQL_API_SQLGETDIAGREC         1011
#define  SQL_API_SQLGETENVATTR         1012
#define  SQL_API_SQLGETFUNCTIONS       44
#define  SQL_API_SQLGETINFO            45
#define  SQL_API_SQLGETSTMTOPTION      46
#define  SQL_API_SQLGETTYPEINFO        47
#define  SQL_API_SQLPARAMDATA          48
#define  SQL_API_SQLPUTDATA            49
#define  SQL_API_SQLSETCONNECTOPTION   50
#define  SQL_API_SQLSETSTMTOPTION      51
#define  SQL_API_SQLSPECIALCOLUMNS     52
#define  SQL_API_SQLSTATISTICS         53
#define  SQL_API_SQLTABLES             54
#define  SQL_API_SQLDATASOURCES        57
#define  SQL_API_SQLSETCONNECTATTR     1016
#define  SQL_API_SQLSETSTMTATTR        1020

#define  SQL_API_SQLBINDFILETOCOL      1250
#define  SQL_API_SQLBINDFILETOPARAM    1251
#define  SQL_API_SQLSETCOLATTRIBUTES   1252
#define  SQL_API_SQLGETSQLCA           1253
#define  SQL_API_SQLSETCONNECTION      1254
#define  SQL_API_SQLGETDATALINKATTR    1255
#define  SQL_API_SQLBUILDDATALINK      1256
#define  SQL_API_SQLNEXTRESULT         1257
#define  SQL_API_SQLEXTENDEDPREPARE    1296
#define  SQL_API_SQLEXTENDEDBIND       1297

#define  SQL_API_SQLFETCHSCROLL        1021
#define  SQL_API_SQLGETLENGTH          1022
#define  SQL_API_SQLGETPOSITION        1023
#define  SQL_API_SQLGETSUBSTRING       1024


#define  SQL_API_SQLALLOCHANDLE        1001
#define  SQL_API_SQLFREEHANDLE         1006
#define  SQL_API_SQLCLOSECURSOR        1003
#define  SQL_API_SQLENDTRAN            1005
#define  SQL_API_SQLCOLATTRIBUTE       6
#define  SQL_API_SQLGETSTMTATTR        1014
#define  SQL_API_SQLGETCONNECTATTR     1007

/* SQLGetFunction defines  - unsupported functions */
#define  SQL_EXT_API_LAST              72

/* Information requested by SQLGetInfo() */
#define SQL_MAX_DRIVER_CONNECTIONS           0
#define SQL_MAXIMUM_DRIVER_CONNECTIONS          SQL_MAX_DRIVER_CONNECTIONS
#define SQL_MAX_CONCURRENT_ACTIVITIES        1
#define SQL_MAXIMUM_CONCURRENT_ACTIVITIES       SQL_MAX_CONCURRENT_ACTIVITIES
#define SQL_ATTR_ANSI_APP                   115


/*
 *  Defines for SQLGetDataLinkAttr.
 */
#define SQL_DATALINK_URL        "URL"

/*
 *  Datalink attribute values for SQLGetDataLinkAttr.
 */

#define SQL_ATTR_DATALINK_COMMENT            1
#define SQL_ATTR_DATALINK_LINKTYPE           2
#define SQL_ATTR_DATALINK_URLCOMPLETE        3
#define SQL_ATTR_DATALINK_URLPATH            4
#define SQL_ATTR_DATALINK_URLPATHONLY        5
#define SQL_ATTR_DATALINK_URLSCHEME          6
#define SQL_ATTR_DATALINK_URLSERVER          7


/* SQLGetInfo defines  - Info Type */
#define  SQL_DATA_SOURCE_NAME                  2
#define  SQL_FETCH_DIRECTION                   8
#define  SQL_SERVER_NAME                      13
#define  SQL_SEARCH_PATTERN_ESCAPE            14
#define  SQL_DBMS_NAME                        17
#define  SQL_DBMS_VER                         18
#define  SQL_ACCESSIBLE_TABLES                19
#define  SQL_ACCESSIBLE_PROCEDURES            20
#define  SQL_CURSOR_COMMIT_BEHAVIOR           23
#define  SQL_DATA_SOURCE_READ_ONLY            25
#define  SQL_DEFAULT_TXN_ISOLATION            26
#define  SQL_IDENTIFIER_CASE                  28
#define  SQL_IDENTIFIER_QUOTE_CHAR            29
#define  SQL_MAX_COLUMN_NAME_LEN              30
#define  SQL_MAXIMUM_COLUMN_NAME_LENGTH       SQL_MAX_COLUMN_NAME_LEN
#define  SQL_MAX_CURSOR_NAME_LEN              31
#define  SQL_MAXIMUM_CURSOR_NAME_LENGTH       SQL_MAX_CURSOR_NAME_LEN
#define  SQL_MAX_TABLE_NAME_LEN               35
#define  SQL_SCROLL_CONCURRENCY               43
#define  SQL_TXN_CAPABLE                      46
#define  SQL_TRANSACTION_CAPABLE              SQL_TXN_CAPABLE
#define  SQL_USER_NAME                        47
#define  SQL_TXN_ISOLATION_OPTION             72
#define  SQL_TRANSACTION_ISOLATION_OPTION     SQL_TXN_ISOLATION_OPTION
#define  SQL_GETDATA_EXTENSIONS               81
#define  SQL_NULL_COLLATION                   85
#define  SQL_ALTER_TABLE                      86
#define  SQL_ORDER_BY_COLUMNS_IN_SELECT       90
#define  SQL_SPECIAL_CHARACTERS               94
#define  SQL_MAX_COLUMNS_IN_GROUP_BY          97
#define  SQL_MAXIMUM_COLUMNS_IN_GROUP_BY      SQL_MAX_COLUMNS_IN_GROUP_BY
#define  SQL_MAX_COLUMNS_IN_INDEX             98
#define  SQL_MAXIMUM_COLUMNS_IN_INDEX         SQL_MAX_COLUMNS_IN_INDEX
#define  SQL_MAX_COLUMNS_IN_ORDER_BY          99
#define  SQL_MAXIMUM_COLUMNS_IN_ORDER_BY      SQL_MAX_COLUMNS_IN_ORDER_BY
#define  SQL_MAX_COLUMNS_IN_SELECT           100
#define  SQL_MAXIMUM_COLUMNS_IN_SELECT       SQL_MAX_COLUMNS_IN_SELECT
#define  SQL_MAX_COLUMNS_IN_TABLE            101
#define  SQL_MAX_INDEX_SIZE                  102
#define  SQL_MAXIMUM_INDEX_SIZE              SQL_MAX_INDEX_SIZE
#define  SQL_MAX_ROW_SIZE                    104
#define  SQL_MAXIMUM_ROW_SIZE                SQL_MAX_ROW_SIZE
#define  SQL_MAX_STATEMENT_LEN               105
#define  SQL_MAXIMUM_STATEMENT_LENGTH        SQL_MAX_STATEMENT_LEN
#define  SQL_MAX_TABLES_IN_SELECT            106
#define  SQL_MAXIMUM_TABLES_IN_SELECT        SQL_MAX_TABLES_IN_SELECT
#define  SQL_MAX_USER_NAME_LEN               107
#define  SQL_MAXIMUM_USER_NAME_LENGTH        SQL_MAX_USER_NAME_LEN
#define  SQL_MAXIMUM_SCHEMA_NAME_LENGTH      SQL_MAX_SCHEMA_NAME_LEN
#define  SQL_MAXIMUM_CATALOG_NAME_LENGTH     SQL_MAX_CATALOG_NAME_LEN
#define  SQL_OJ_CAPABILITIES                115
#define  SQL_OUTER_JOIN_CAPABILITIES                SQL_OJ_CAPABILITIES
#define  SQL_XOPEN_CLI_YEAR               10000
#define  SQL_CURSOR_SENSITIVITY           10001
#define  SQL_DESCRIBE_PARAMETER           10002
#define  SQL_CATALOG_NAME                 10003
#define  SQL_COLLATION_SEQ                10004
#define  SQL_MAX_IDENTIFIER_LEN           10005
#define  SQL_MAXIMUM_IDENTIFIER_LENGTH    SQL_MAX_IDENTIFIER_LEN
#define  SQL_INTEGRITY                       73
#define  SQL_DATABASE_CODEPAGE               2519
#define  SQL_APPLICATION_CODEPAGE            2520
#define  SQL_CONNECT_CODEPAGE                2521
#define  SQL_ATTR_DB2_APPLICATION_ID         2532
#define  SQL_ATTR_DB2_APPLICATION_HANDLE     2533
#define  SQL_ATTR_HANDLE_XA_ASSOCIATED       2535
#define  SQL_DB2_DRIVER_VER                  2550
#define  SQL_ATTR_XML_DECLARATION            2552
#define  SQL_ATTR_CURRENT_IMPLICIT_XMLPARSE_OPTION    2553
#define  SQL_ATTR_XQUERY_STATEMENT           2557
#define  SQL_DB2_DRIVER_TYPE                 2567


#define SQL_INFO_LAST                         114
#define SQL_INFO_DRIVER_START                 1000

/*
 *  SQLExtendedPrepare statement types.
 */

#define SQL_CLI_STMT_UNDEFINED               0
#define SQL_CLI_STMT_ALTER_TABLE             1
#define SQL_CLI_STMT_CREATE_INDEX            5
#define SQL_CLI_STMT_CREATE_TABLE            6
#define SQL_CLI_STMT_CREATE_VIEW             7
#define SQL_CLI_STMT_DELETE_SEARCHED         8
#define SQL_CLI_STMT_DELETE_POSITIONED       9
#define SQL_CLI_STMT_DROP_PACKAGE            10
#define SQL_CLI_STMT_DROP_INDEX              11
#define SQL_CLI_STMT_DROP_TABLE              12
#define SQL_CLI_STMT_DROP_VIEW               13
#define SQL_CLI_STMT_GRANT                   14
#define SQL_CLI_STMT_INSERT                  15
#define SQL_CLI_STMT_REVOKE                  16
#define SQL_CLI_STMT_SELECT                  18
#define SQL_CLI_STMT_UPDATE_SEARCHED         19
#define SQL_CLI_STMT_UPDATE_POSITIONED       20
#define SQL_CLI_STMT_CALL                    24
#define SQL_CLI_STMT_SELECT_FOR_UPDATE        29
#define SQL_CLI_STMT_WITH                     30
#define SQL_CLI_STMT_SELECT_FOR_FETCH         31
#define SQL_CLI_STMT_VALUES                   32
#define SQL_CLI_STMT_CREATE_TRIGGER           34
#define SQL_CLI_STMT_SELECT_OPTIMIZE_FOR_NROWS 39
#define SQL_CLI_STMT_SELECT_INTO               40
#define SQL_CLI_STMT_CREATE_PROCEDURE          41
#define SQL_CLI_STMT_CREATE_FUNCTION           42
#define SQL_CLI_STMT_INSERT_VALUES             45
#define SQL_CLI_STMT_SET_CURRENT_QUERY_OPT     46
#define SQL_CLI_STMT_MERGE                     56
#define SQL_CLI_STMT_XQUERY                    59

/*
 *  IBM specific SQLGetInfo values.
 */

#define SQL_IBM_ALTERTABLEVARCHAR             1000

/* SQL_ALTER_TABLE bitmasks */
#define SQL_AT_ADD_COLUMN                       0x00000001L
#define SQL_AT_DROP_COLUMN                      0x00000002L
#define SQL_AT_ADD_CONSTRAINT                   0x00000008L

/* SQL_CURSOR_COMMIT_BEHAVIOR and SQL_CURSOR_ROLLBACK_BEHAVIOR values */


/* SQL_IDENTIFIER_CASE values */


/* SQL_TXN_CAPABLE values */


/* SQL_SCROLL_CONCURRENCY masks */

#define SQL_SCCO_READ_ONLY            0x00000001L
#define SQL_SCCO_LOCK                 0x00000002L
#define SQL_SCCO_OPT_ROWVER           0x00000004L
#define SQL_SCCO_OPT_VALUES           0x00000008L

/* SQL_FETCH_DIRECTION masks */

#define  SQL_FD_FETCH_NEXT            0x00000001L
#define  SQL_FD_FETCH_FIRST           0x00000002L
#define  SQL_FD_FETCH_LAST            0x00000004L
#define  SQL_FD_FETCH_PRIOR           0x00000008L
#define  SQL_FD_FETCH_ABSOLUTE        0x00000010L
#define  SQL_FD_FETCH_RELATIVE        0x00000020L
#define  SQL_FD_FETCH_RESUME          0x00000040L

/* SQL_TXN_ISOLATION_OPTION masks */
#define SQL_TXN_READ_UNCOMMITTED            0x00000001L
#define SQL_TRANSACTION_READ_UNCOMMITTED        SQL_TXN_READ_UNCOMMITTED
#define SQL_TXN_READ_COMMITTED              0x00000002L
#define SQL_TRANSACTION_READ_COMMITTED          SQL_TXN_READ_COMMITTED
#define SQL_TXN_REPEATABLE_READ             0x00000004L
#define SQL_TRANSACTION_REPEATABLE_READ         SQL_TXN_REPEATABLE_READ
#define SQL_TXN_SERIALIZABLE                0x00000008L
#define SQL_TRANSACTION_SERIALIZABLE            SQL_TXN_SERIALIZABLE
#define SQL_TXN_NOCOMMIT                   0x00000020L
#define SQL_TRANSACTION_NOCOMMIT                SQL_TXN_NOCOMMIT

/* SQL_GETDATA_EXTENSIONS values */

#define SQL_GD_ANY_COLUMN             0x00000001L
#define SQL_GD_ANY_ORDER              0x00000002L

/* SQL_OJ_CAPABILITIES values */

#define SQL_OJ_LEFT                   0x00000001L
#define SQL_OJ_RIGHT                  0x00000002L
#define SQL_OJ_FULL                   0x00000004L
#define SQL_OJ_NESTED                 0x00000008L
#define SQL_OJ_NOT_ORDERED            0x00000010L
#define SQL_OJ_INNER                  0x00000020L
#define SQL_OJ_ALL_COMPARISON_OPS     0x00000040L

/* SQL_DB2_DRIVER_TYPE values */
#define SQL_CLI_DRIVER_TYPE_UNDEFINED 0
#define SQL_CLI_DRIVER_RUNTIME_CLIENT 1
#define SQL_CLI_DRIVER_CLI_DRIVER     2


/* SQLGetTypeInfo define */
#define  SQL_ALL_TYPES                0

/* Options for SQLGetStmtOption/SQLSetStmtOption */

#define SQL_ATTR_AUTO_IPD               10001
#define SQL_ATTR_APP_ROW_DESC           10010
#define SQL_ATTR_APP_PARAM_DESC         10011
#define SQL_ATTR_IMP_ROW_DESC           10012
#define SQL_ATTR_IMP_PARAM_DESC         10013
#define SQL_ATTR_METADATA_ID            10014
#define SQL_ATTR_CURSOR_SCROLLABLE      (-1)
#define SQL_ATTR_CURSOR_SENSITIVITY     (-2)

/* SQL_ATTR_CURSOR_SCROLLABLE values */
#define SQL_NONSCROLLABLE                       0
#define SQL_SCROLLABLE                          1


/* Options for SQLGetStmtOption/SQLSetStmtOption extensions */
#define  SQL_CURSOR_HOLD              1250
#define  SQL_ATTR_CURSOR_HOLD         1250
#define  SQL_NODESCRIBE_OUTPUT        1251
#define  SQL_ATTR_NODESCRIBE_OUTPUT   1251

#define  SQL_NODESCRIBE_INPUT         1264
#define  SQL_ATTR_NODESCRIBE_INPUT    1264
#define  SQL_NODESCRIBE               SQL_NODESCRIBE_OUTPUT
#define  SQL_ATTR_NODESCRIBE          SQL_NODESCRIBE_OUTPUT
#define  SQL_CLOSE_BEHAVIOR           1257
#define  SQL_ATTR_CLOSE_BEHAVIOR      1257
#define  SQL_ATTR_CLOSE_BEHAVIOR      1257
#define  SQL_ATTR_CLOSEOPEN           1265
#define  SQL_ATTR_CURRENT_PACKAGE_SET 1276
#define  SQL_ATTR_DEFERRED_PREPARE    1277
#define  SQL_ATTR_EARLYCLOSE          1268
#define  SQL_ATTR_PROCESSCTL          1278
#define  SQL_ATTR_PREFETCH            1285
#define  SQL_ATTR_ENABLE_IPD_SETTING  1286
/*
 *  Descriptor value for setting the descriptor type.
 */

#define  SQL_DESC_DESCRIPTOR_TYPE     1287

#define  SQL_ATTR_OPTIMIZE_SQLCOLUMNS 1288
#define  SQL_ATTR_MEM_DEBUG_DUMP      1289
#define  SQL_ATTR_CONNECT_NODE        1290
#define  SQL_ATTR_CONNECT_WITH_XA     1291
#define  SQL_ATTR_GET_XA_RESOURCE     1292
#define  SQL_ATTR_DB2_SQLERRP         2451
#define  SQL_ATTR_SERVER_MSGTXT_SP    2452
#define  SQL_ATTR_OPTIMIZE_FOR_NROWS         2450
#define  SQL_ATTR_QUERY_OPTIMIZATION_LEVEL   1293
#define  SQL_ATTR_USE_LIGHT_OUTPUT_SQLDA     1298
#define  SQL_ATTR_CURSOR_BLOCK_NUM_ROWS      2453
#define  SQL_ATTR_CURSOR_BLOCK_EARLY_CLOSE   2454
#define  SQL_ATTR_SERVER_MSGTXT_MASK         2455
#define  SQL_ATTR_USE_LIGHT_INPUT_SQLDA      2458
#define  SQL_ATTR_BLOCK_FOR_NROWS            2459
#define  SQL_ATTR_OPTIMIZE_ROWS_FOR_BLOCKING 2460
#define  SQL_ATTR_STATICMODE                 2467
#define  SQL_ATTR_DB2_MESSAGE_PREFIX         2468
#define  SQL_ATTR_CALL_RETVAL_AS_PARM        2469
#define  SQL_ATTR_CALL_RETURN                2470
#define  SQL_ATTR_RETURN_USER_DEFINED_TYPES  2471
#define  SQL_ATTR_ENABLE_EXTENDED_PARAMDATA  2472
#define  SQL_ATTR_APP_TYPE                   2473
#define  SQL_ATTR_TRANSFORM_GROUP            2474
#define  SQL_ATTR_DESCRIBE_CALL              2476
#define  SQL_ATTR_AUTOCOMMCLEANUP            2477
#define  SQL_ATTR_USEMALLOC                  2478
#define  SQL_ATTR_PRESERVE_LOCALE            2479
#define  SQL_ATTR_MAPGRAPHIC                 2480
#define  SQL_ATTR_INSERT_BUFFERING           2481

#define  SQL_ATTR_USE_LOAD_API               2482
#define  SQL_ATTR_LOAD_RECOVERABLE           2483
#define  SQL_ATTR_LOAD_COPY_LOCATION         2484
#define  SQL_ATTR_LOAD_MESSAGE_FILE          2485
#define  SQL_ATTR_LOAD_SAVECOUNT             2486
#define  SQL_ATTR_LOAD_CPU_PARALLELISM       2487
#define  SQL_ATTR_LOAD_DISK_PARALLELISM      2488
#define  SQL_ATTR_LOAD_INDEXING_MODE         2489
#define  SQL_ATTR_LOAD_STATS_MODE            2490
#define  SQL_ATTR_LOAD_TEMP_FILES_PATH       2491
#define  SQL_ATTR_LOAD_DATA_BUFFER_SIZE      2492
#define  SQL_ATTR_LOAD_MODIFIED_BY           2493
#define  SQL_ATTR_DB2_RESERVED_2494          2494
#define  SQL_ATTR_DESCRIBE_BEHAVIOR          2495
#define  SQL_ATTR_FETCH_SENSITIVITY          2496
#define  SQL_ATTR_DB2_RESERVED_2497          2497
#define  SQL_ATTR_CLIENT_LOB_BUFFERING       2498
#define  SQL_ATTR_SKIP_TRACE                 2499
#define  SQL_ATTR_LOAD_INFO                  2501
#define  SQL_ATTR_DESCRIBE_INPUT_ON_PREPARE  2505
#define  SQL_ATTR_DESCRIBE_OUTPUT_LEVEL      2506
#define  SQL_ATTR_CURRENT_PACKAGE_PATH       2509
#define  SQL_ATTR_INFO_PROGRAMID             2511
#define  SQL_ATTR_INFO_PROGRAMNAME           2516
#define  SQL_ATTR_FREE_LOCATORS_ON_FETCH     2518
#define  SQL_ATTR_KEEP_DYNAMIC               2522
#define  SQL_ATTR_LOAD_ROWS_READ_PTR         2524
#define  SQL_ATTR_LOAD_ROWS_SKIPPED_PTR      2525
#define  SQL_ATTR_LOAD_ROWS_COMMITTED_PTR    2526
#define  SQL_ATTR_LOAD_ROWS_LOADED_PTR       2527
#define  SQL_ATTR_LOAD_ROWS_REJECTED_PTR     2528
#define  SQL_ATTR_LOAD_ROWS_DELETED_PTR      2529
#define  SQL_ATTR_LOAD_INFO_VER              2530
#define  SQL_ATTR_SET_SSA                    2531
#define  SQL_ATTR_BLOCK_LOBS                 2534
#define  SQL_ATTR_LOAD_ACCESS_LEVEL          2536
#define  SQL_ATTR_MAPCHAR                    2546
#define  SQL_ATTR_ARM_CORRELATOR             2554
#define  SQL_ATTR_CLIENT_DEBUGINFO           2556

/*
 *  SQL_ATTR_DESCRIBE_INPUT / SQL_ATTR_DESCRIBE_OUTPUT values
 */
#define SQL_DESCRIBE_NONE                 0
#define SQL_DESCRIBE_LIGHT                1
#define SQL_DESCRIBE_REGULAR              2
#define SQL_DESCRIBE_EXTENDED             3

/*
 *  Use load values.
 */

#define SQL_USE_LOAD_OFF                  0
#define SQL_USE_LOAD_INSERT               1
#define SQL_USE_LOAD_REPLACE              2
#define SQL_USE_LOAD_RESTART              3
#define SQL_USE_LOAD_TERMINATE            4

/*
 *  SQL_ATTR_PREFETCH_ENABLE values.
 */

#define SQL_PREFETCH_ON                  1
#define SQL_PREFETCH_OFF                 0
#define SQL_PREFETCH_DEFAULT             SQL_PREFETCH_OFF

/* SQL_CLOSE_BEHAVIOR values.                  */

#define SQL_CC_NO_RELEASE             0
#define SQL_CC_RELEASE                1
#define SQL_CC_DEFAULT                SQL_CC_NO_RELEASE

/* SQL_ATTR_DEFERRED_PREPARE values  */

#define SQL_DEFERRED_PREPARE_ON       1
#define SQL_DEFERRED_PREPARE_OFF      0
#define SQL_DEFERRED_PREPARE_DEFAULT  SQL_DEFERRED_PREPARE_ON

/* SQL_ATTR_EARLYCLOSE values  */

#define SQL_EARLYCLOSE_ON             1
#define SQL_EARLYCLOSE_OFF            0
#define SQL_EARLYCLOSE_SERVER         2
#define SQL_EARLYCLOSE_DEFAULT        SQL_EARLYCLOSE_ON

/* SQL_ATTR_APP_TYPE values  */

#define SQL_APP_TYPE_ODBC             1
#define SQL_APP_TYPE_OLEDB            2
#define SQL_APP_TYPE_JDBC             3
#define SQL_APP_TYPE_ADONET           4
#define SQL_APP_TYPE_DEFAULT          SQL_APP_TYPE_ODBC

/* SQL_ATTR_PROCESSCTL masks  */

#define SQL_PROCESSCTL_NOTHREAD       0x00000001L
#define SQL_PROCESSCTL_NOFORK         0x00000002L
#define SQL_PROCESSCTL_SHARESTMTDESC  0x00000004L

/* CLI attribute/option values */
#define SQL_FALSE               0
#define SQL_TRUE                1

/* Options for SQL_CURSOR_HOLD */
#define SQL_CURSOR_HOLD_ON        1
#define SQL_CURSOR_HOLD_OFF       0
#define SQL_CURSOR_HOLD_DEFAULT   SQL_CURSOR_HOLD_ON


/* Options for SQL_NODESCRIBE_INPUT/SQL_NODESCRIBE_OUTPUT */
#define SQL_NODESCRIBE_ON          1
#define SQL_NODESCRIBE_OFF         0
#define SQL_NODESCRIBE_DEFAULT     SQL_NODESCRIBE_OFF

/* Options for SQL_ATTR_DESCRIBE_CALL */
#define SQL_DESCRIBE_CALL_NEVER           0
#define SQL_DESCRIBE_CALL_BEFORE          1
#define SQL_DESCRIBE_CALL_ON_ERROR        2
#define SQL_DESCRIBE_CALL_DEFAULT         (-1)

/* Options for SQL_ATTR_CLIENT_LOB_BUFFERING */
#define SQL_CLIENTLOB_USE_LOCATORS          0
#define SQL_CLIENTLOB_BUFFER_UNBOUND_LOBS   1
#define SQL_CLIENTLOB_DEFAULT               SQL_CLIENTLOB_USE_LOCATORS

/* Options for SQL_ATTR_PREPDESC_BEHAVIOR */
/* To be determined */

/* Options for SQLGetConnectOption/SQLSetConnectOption extensions */
#define SQL_WCHARTYPE                              1252
#define SQL_LONGDATA_COMPAT                        1253
#define SQL_CURRENT_SCHEMA                         1254
#define SQL_DB2EXPLAIN                             1258
#define SQL_DB2ESTIMATE                            1259
#define SQL_PARAMOPT_ATOMIC                        1260
#define SQL_STMTTXN_ISOLATION                      1261
#define SQL_MAXCONN                                1262
#define SQL_ATTR_CLISCHEMA                         1280
#define SQL_ATTR_INFO_USERID                       1281
#define SQL_ATTR_INFO_WRKSTNNAME                   1282
#define SQL_ATTR_INFO_APPLNAME                     1283
#define SQL_ATTR_INFO_ACCTSTR                      1284
#define SQL_ATTR_AUTOCOMMIT_NOCOMMIT               2462
#define SQL_ATTR_QUERY_PATROLLER                   2466
#define SQL_ATTR_CHAINING_BEGIN                    2464
#define SQL_ATTR_CHAINING_END                      2465
#define SQL_ATTR_EXTENDEDBIND                      2475
#define SQL_ATTR_GRAPHIC_UNICODESERVER             2503
#define SQL_ATTR_RETURN_CHAR_AS_WCHAR_OLEDB        2517
#define SQL_ATTR_GATEWAY_CONNECTED                 2537
#define SQL_ATTR_SQLCOLUMNS_SORT_BY_ORDINAL_OLEDB  2542
#define SQL_ATTR_REPORT_ISLONG_FOR_LONGTYPES_OLEDB 2543
#define SQL_ATTR_PING_DB                           2545
#define SQL_ATTR_RECEIVE_TIMEOUT                   2547
#define SQL_ATTR_REOPT                             2548
#define SQL_ATTR_LOB_CACHE_SIZE                    2555
#define SQL_ATTR_STREAM_GETDATA                    2558
#define SQL_ATTR_APP_USES_LOB_LOCATOR              2559
#define SQL_ATTR_MAX_LOB_BLOCK_SIZE                2560
#define SQL_ATTR_USE_TRUSTED_CONTEXT               2561
#define SQL_ATTR_TRUSTED_CONTEXT_USERID            2562
#define SQL_ATTR_TRUSTED_CONTEXT_PASSWORD          2563
#define SQL_ATTR_USER_REGISTRY_NAME                2564
#define SQL_ATTR_DECFLOAT_ROUNDING_MODE            2565
#define SQL_ATTR_APPEND_FOR_FETCH_ONLY             2573
#define SQL_ATTR_ONLY_USE_BIG_PACKAGES             2577

#define SQL_ATTR_WCHARTYPE          SQL_WCHARTYPE
#define SQL_ATTR_LONGDATA_COMPAT    SQL_LONGDATA_COMPAT
#define SQL_ATTR_CURRENT_SCHEMA     SQL_CURRENT_SCHEMA
#define SQL_ATTR_DB2EXPLAIN         SQL_DB2EXPLAIN
#define SQL_ATTR_DB2ESTIMATE        SQL_DB2ESTIMATE
#define SQL_ATTR_PARAMOPT_ATOMIC    SQL_PARAMOPT_ATOMIC
#define SQL_ATTR_STMTTXN_ISOLATION  SQL_STMTTXN_ISOLATION
#define SQL_ATTR_MAXCONN            SQL_MAXCONN

/* Options for SQLSetConnectOption, SQLSetEnvAttr */
#define SQL_CONNECTTYPE              1255
#define SQL_SYNC_POINT               1256
#define SQL_MINMEMORY_USAGE          1263
#define SQL_CONN_CONTEXT             1269
#define SQL_ATTR_INHERIT_NULL_CONNECT    1270
#define SQL_ATTR_FORCE_CONVERSION_ON_CLIENT 1275
#define SQL_ATTR_INFO_KEYWORDLIST    2500

#define SQL_ATTR_CONNECTTYPE         SQL_CONNECTTYPE
#define SQL_ATTR_SYNC_POINT          SQL_SYNC_POINT
#define SQL_ATTR_MINMEMORY_USAGE     SQL_MINMEMORY_USAGE
#define SQL_ATTR_CONN_CONTEXT        SQL_CONN_CONTEXT

/* Options for SQL_LONGDATA_COMPAT */
#define SQL_LD_COMPAT_YES            1
#define SQL_LD_COMPAT_NO             0
#define SQL_LD_COMPAT_DEFAULT        SQL_LD_COMPAT_NO

/* Options for SQL_ATTR_EXTENDEDBIND */
#define SQL_ATTR_EXTENDEDBIND_COPY    1
#define SQL_ATTR_EXTENDEDBIND_NOCOPY  0
#define SQL_ATTR_EXTENDEDBIND_DEFAULT SQL_ATTR_EXTENDEDBIND_NOCOPY

/* SQL_NULL_COLLATION values */
#define SQL_NC_HIGH                         0
#define SQL_NC_LOW                          1

/* Options for SQLGetInfo extentions */
#define CLI_MAX_LONGVARCHAR           1250
#define CLI_MAX_VARCHAR               1251
#define CLI_MAX_CHAR                  1252
#define CLI_MAX_LONGVARGRAPHIC        1253
#define CLI_MAX_VARGRAPHIC            1254
#define CLI_MAX_GRAPHIC               1255

/*
 *  Private SQLGetDiagField extensions.
 */

#define SQL_DIAG_MESSAGE_TEXT_PTR       2456
#define SQL_DIAG_LINE_NUMBER            2461
#define SQL_DIAG_ERRMC                  2467
#define SQL_DIAG_BYTES_PROCESSED        2477
#define SQL_DIAG_RELATIVE_COST_ESTIMATE 2504
#define SQL_DIAG_ROW_COUNT_ESTIMATE     2507
#define SQL_DIAG_ELAPSED_SERVER_TIME       2538
#define SQL_DIAG_ELAPSED_NETWORK_TIME      2539
#define SQL_DIAG_ACCUMULATED_SERVER_TIME   2540
#define SQL_DIAG_ACCUMULATED_NETWORK_TIME  2541
#define SQL_DIAG_QUIESCE                   2549
#define SQL_DIAG_TOLERATED_ERROR           2559

/*
 *  Values for SQL_DIAG_QUIESCE
 */

#define SQL_DIAG_QUIESCE_NO                  0
#define SQL_DIAG_QUIESCE_DATABASE            1
#define SQL_DIAG_QUIESCE_INSTANCE            2

/*
 *  Private SQLSetEnvAttr extensions.
 */

#define SQL_ATTR_LITTLE_ENDIAN_UNICODE 2457
#define SQL_ATTR_DIAGLEVEL             2574
#define SQL_ATTR_NOTIFYLEVEL           2575
#define SQL_ATTR_DIAGPATH              2576

/*
 *  Options for SQL_PARAMOPT_ATOMIC
 */

#define SQL_ATOMIC_YES               1
#define SQL_ATOMIC_NO                0
#define SQL_ATOMIC_DEFAULT           SQL_ATOMIC_YES

/* Options for SQL_CONNECT_TYPE */
#define SQL_CONCURRENT_TRANS         1
#define SQL_COORDINATED_TRANS        2
#define SQL_CONNECTTYPE_DEFAULT      SQL_CONCURRENT_TRANS

/* Options for SQL_SYNCPOINT */
#define SQL_ONEPHASE                 1
#define SQL_TWOPHASE                 2
#define SQL_SYNCPOINT_DEFAULT        SQL_ONEPHASE

/* Options for SQL_DB2ESTIMATE */
#define SQL_DB2ESTIMATE_ON           1
#define SQL_DB2ESTIMATE_OFF          0
#define SQL_DB2ESTIMATE_DEFAULT      SQL_DB2ESTIMATE_OFF

/* Options for SQL_DB2EXPLAIN */
#define SQL_DB2EXPLAIN_OFF              0x00000000L
#define SQL_DB2EXPLAIN_SNAPSHOT_ON      0x00000001L
#define SQL_DB2EXPLAIN_MODE_ON          0x00000002L
#define SQL_DB2EXPLAIN_SNAPSHOT_MODE_ON SQL_DB2EXPLAIN_SNAPSHOT_ON+SQL_DB2EXPLAIN_MODE_ON
#define SQL_DB2EXPLAIN_ON               SQL_DB2EXPLAIN_SNAPSHOT_ON
#define SQL_DB2EXPLAIN_DEFAULT          SQL_DB2EXPLAIN_OFF

/* Options for SQL_WCHARTYPE
 * Note that you can only specify SQL_WCHARTYPE_CONVERT if you have an
 * external compile flag SQL_WCHART_CONVERT defined
 */
#ifdef SQL_WCHART_CONVERT
#define SQL_WCHARTYPE_CONVERT        1
#endif
#define SQL_WCHARTYPE_NOCONVERT      0
#define SQL_WCHARTYPE_DEFAULT        SQL_WCHARTYPE_NOCONVERT

/* Options for SQL_ATTR_OPTIMIZE_SQLCOLUMNS
 *
 */
#define SQL_OPTIMIZE_SQLCOLUMNS_OFF  0
#define SQL_OPTIMIZE_SQLCOLUMNS_ON   1
#define SQL_OPTIMIZE_SQLCOLUMNS_DEFAULT SQL_OPTIMIZE_SQLCOLUMNS_OFF

/* Options for SQL_ATTR_CONNECT_WITH_XA
 *
 */
#define SQL_CONNECT_WITH_XA_OFF  0
#define SQL_CONNECT_WITH_XA_ON   1
#define SQL_CONNECT_WITH_XA_DEFAULT SQL_CONNECT_WITH_XA_OFF

/*
 *  Options for SQL_ATTR_SERVER_MSGTXT_MASK
 */
#define  SQL_ATTR_SERVER_MSGTXT_MASK_LOCAL_FIRST 0x00000000
#define  SQL_ATTR_SERVER_MSGTXT_MASK_WARNINGS    0x00000001
#define  SQL_ATTR_SERVER_MSGTXT_MASK_ERRORS      0xFFFFFFFE
#define  SQL_ATTR_SERVER_MSGTXT_MASK_ALL         0xFFFFFFFF
#define  SQL_ATTR_SERVER_MSGTXT_MASK_DEFAULT SQL_ATTR_SERVER_MSGTXT_MASK_LOCAL_FIRST

/*
 * Options for SQL_ATTR_QUERY_PATROLLER
 */

#define SQL_ATTR_QUERY_PATROLLER_DISABLE   1
#define SQL_ATTR_QUERY_PATROLLER_ENABLE    2
#define SQL_ATTR_QUERY_PATROLLER_BYPASS    3

/*
 * Options for SQL_ATTR_STATICMODE
 */
#define SQL_STATICMODE_DISABLED 0
#define SQL_STATICMODE_CAPTURE  1
#define SQL_STATICMODE_MATCH    2

/*
 * Options for SQL_ATTR_DB2_MESSAGE_PREFIX
 */
#define SQL_ATTR_DB2_MESSAGE_PREFIX_OFF 0
#define SQL_ATTR_DB2_MESSAGE_PREFIX_ON  1
#define SQL_ATTR_DB2_MESSAGE_PREFIX_DEFAULT SQL_ATTR_DB2_MESSAGE_PREFIX_ON

/*
 * Options for SQL_ATTR_INSERT_BUFFERING
 */
#define SQL_ATTR_INSERT_BUFFERING_OFF     0
#define SQL_ATTR_INSERT_BUFFERING_ON      1
#define SQL_ATTR_INSERT_BUFFERING_IGD     2

/* Column types and scopes in SQLSpecialColumns */

#define SQL_SCOPE_CURROW             0
#define SQL_SCOPE_TRANSACTION        1
#define SQL_SCOPE_SESSION            2

/* Defines for SQLStatistics */
#define SQL_INDEX_UNIQUE             0
#define SQL_INDEX_ALL                1

/* Defines for SQLStatistics (returned in the result set) */
#define SQL_INDEX_CLUSTERED          1
#define SQL_INDEX_HASHED             2
#define SQL_INDEX_OTHER              3

/* Defines for SQLSpecialColumns (returned in the result set) */
#define SQL_PC_UNKNOWN               0
#define SQL_PC_NON_PSEUDO            1
#define SQL_PC_PSEUDO                2

/* Reserved value for the IdentifierType argument of SQLSpecialColumns() */
#define SQL_ROW_IDENTIFIER  1

/* Options for SQL_ATTR_MAPGRAPHIC */
#define SQL_MAPGRAPHIC_DEFAULT      -1
#define SQL_MAPGRAPHIC_GRAPHIC       0
#define SQL_MAPGRAPHIC_WCHAR         1

/* Options for SQL_ATTR_MAPCHAR */
#define SQL_MAPCHAR_DEFAULT          0
#define SQL_MAPCHAR_WCHAR            1

/* SQLDataSources "fDirection" values, also used on SQLExtendedFetch() */
/* See sqlext.h for additional SQLExtendedFetch fetch direction defines */
#define  SQL_FETCH_NEXT              1
#define  SQL_FETCH_FIRST             2

/* OTHER CODES USED FOR FETCHORIENTATION IN SQLFETCHSCROLL() */
#define SQL_FETCH_LAST      3
#define SQL_FETCH_PRIOR     4
#define SQL_FETCH_ABSOLUTE  5
#define SQL_FETCH_RELATIVE  6

/* SQL_ATTR_XML_DECLARATION bitmask values */
#define SQL_XML_DECLARATION_NONE     0x00000000
#define SQL_XML_DECLARATION_BOM      0x00000001
#define SQL_XML_DECLARATION_BASE     0x00000002
#define SQL_XML_DECLARATION_ENCATTR  0x00000004

/*
 * Environment attributes; note SQL_CONNECTTYPE, SQL_SYNC_POINT are also
 * environment attributes that are settable at the connection level
 */

#define SQL_ATTR_OUTPUT_NTS          10001


/*  LOB file reference options */
#ifndef SQL_H_SQL                     /* if sql.h is not included, then...  */
#define SQL_FILE_READ              2  /* Input file to read from            */
#define SQL_FILE_CREATE            8  /* Output file - new file to be       */
                                      /* created                            */
#define SQL_FILE_OVERWRITE        16  /* Output file - overwrite existing   */
                                      /* file or create a new file if it    */
                                      /* doesn't exist                      */
#define SQL_FILE_APPEND           32  /* Output file - append to an         */
                                      /* existing file or create a new file */
                                      /* if it doesn't exist                */
#endif

/*
 *  Source of string for SQLGetLength(), SQLGetPosition(),
 *  and SQLGetSubstring().
 */
#define SQL_FROM_LOCATOR           2
#define SQL_FROM_LITERAL           3

/*
 *  Options for Rounding Modes. These numeric values can
 *  be set with SQLSetConnectAttr() API for the attribute
 *  SQL_ATTR_DECFLOAT_ROUNDING_MODE. The SQLGetConnectAttr()
 *  API will return these values for the
 *  SQL_ATTR_DECFLOAT_ROUNDING_MODE attribute.
 */
#define SQL_ROUND_HALF_EVEN        0
#define SQL_ROUND_HALF_UP          1
#define SQL_ROUND_DOWN             2
#define SQL_ROUND_CEILING          3
#define SQL_ROUND_FLOOR            4
#define SQL_ROUND_HALF_DOWN        5
#define SQL_ROUND_UP               6

/*
 *  Function definitions of APIs in both X/Open CLI and ODBC
 */

SQLRETURN SQL_API_FN  SQLColumns       (SQLHSTMT          hstmt,
                                        SQLCHAR     FAR   *szCatalogName,
                                        SQLSMALLINT       cbCatalogName,
                                        SQLCHAR     FAR   *szSchemaName,
                                        SQLSMALLINT       cbSchemaName,
                                        SQLCHAR     FAR   *szTableName,
                                        SQLSMALLINT       cbTableName,
                                        SQLCHAR     FAR   *szColumnName,
                                        SQLSMALLINT       cbColumnName);

SQLRETURN SQL_API_FN  SQLDataSources   (SQLHENV           henv,
                                        SQLUSMALLINT      fDirection,
                                        SQLCHAR     FAR   *szDSN,
                                        SQLSMALLINT       cbDSNMax,
                                        SQLSMALLINT FAR   *pcbDSN,
                                        SQLCHAR     FAR   *szDescription,
                                        SQLSMALLINT       cbDescriptionMax,
                                        SQLSMALLINT FAR   *pcbDescription);
SQLRETURN  SQL_API SQLGetConnectAttr(   SQLHDBC ConnectionHandle,
                                        SQLINTEGER Attribute,
                                        SQLPOINTER Value,
                                        SQLINTEGER BufferLength,
                                        SQLINTEGER *StringLength);

SQLRETURN SQL_API_FN  SQLGetConnectOption (
                                        SQLHDBC           hdbc,
                                        SQLUSMALLINT      fOption,
                                        SQLPOINTER        pvParam);

SQLRETURN SQL_API_FN  SQLGetFunctions  (SQLHDBC           hdbc,
                                        SQLUSMALLINT      fFunction,
                                        SQLUSMALLINT FAR  *pfExists);

SQLRETURN SQL_API_FN  SQLGetInfo       (SQLHDBC           hdbc,
                                        SQLUSMALLINT      fInfoType,
                                        SQLPOINTER        rgbInfoValue,
                                        SQLSMALLINT       cbInfoValueMax,
                                        SQLSMALLINT FAR   *pcbInfoValue);

SQLRETURN  SQL_API SQLGetStmtAttr(      SQLHSTMT StatementHandle,
                                        SQLINTEGER Attribute,
                                        SQLPOINTER Value,
                                        SQLINTEGER BufferLength,
                                        SQLINTEGER *StringLength);

SQLRETURN SQL_API_FN  SQLGetStmtOption (SQLHSTMT          hstmt,
                                        SQLUSMALLINT      fOption,
                                        SQLPOINTER        pvParam);

SQLRETURN SQL_API_FN  SQLGetTypeInfo   (SQLHSTMT          hstmt,
                                        SQLSMALLINT       fSqlType);


SQLRETURN SQL_API_FN  SQLParamData     (SQLHSTMT          hstmt,
                                        SQLPOINTER  FAR   *prgbValue);


SQLRETURN SQL_API_FN  SQLSetConnectAttr(
                                        SQLHDBC           hdbc,
                                        SQLINTEGER        fOption,
                                        SQLPOINTER        pvParam,
                                        SQLINTEGER        fStrLen);


SQLRETURN SQL_API_FN  SQLSetStmtAttr   (SQLHSTMT          hstmt,
                                        SQLINTEGER        fOption,
                                        SQLPOINTER        pvParam,
                                        SQLINTEGER       fStrLen);


SQLRETURN SQL_API_FN  SQLSpecialColumns(SQLHSTMT          hstmt,
                                        SQLUSMALLINT      fColType,
                                        SQLCHAR     FAR   *szCatalogName,
                                        SQLSMALLINT       cbCatalogName,
                                        SQLCHAR     FAR   *szSchemaName,
                                        SQLSMALLINT       cbSchemaName,
                                        SQLCHAR     FAR   *szTableName,
                                        SQLSMALLINT       cbTableName,
                                        SQLUSMALLINT      fScope,
                                        SQLUSMALLINT      fNullable);

SQLRETURN SQL_API_FN  SQLStatistics    (SQLHSTMT          hstmt,
                                        SQLCHAR     FAR   *szCatalogName,
                                        SQLSMALLINT       cbCatalogName,
                                        SQLCHAR     FAR   *szSchemaName,
                                        SQLSMALLINT       cbSchemaName,
                                        SQLCHAR     FAR   *szTableName,
                                        SQLSMALLINT       cbTableName,
                                        SQLUSMALLINT      fUnique,

                                        SQLUSMALLINT      fAccuracy);

SQLRETURN SQL_API_FN  SQLTables        (SQLHSTMT          hstmt,
                                        SQLCHAR     FAR   *szCatalogName,
                                        SQLSMALLINT       cbCatalogName,
                                        SQLCHAR     FAR   *szSchemaName,
                                        SQLSMALLINT       cbSchemaName,
                                        SQLCHAR     FAR   *szTableName,
                                        SQLSMALLINT       cbTableName,
                                        SQLCHAR     FAR   *szTableType,
                                        SQLSMALLINT       cbTableType);

SQLRETURN SQL_API SQLNextResult        (SQLHSTMT          hstmtSource,
                                        SQLHSTMT          hstmtTarget);
/* UNICODE versions */

#if 0 /* [i_a] don't know why this throws a tantrum */
#ifdef ODBC64
SQLRETURN SQL_API SQLColAttributeW(
    SQLHSTMT        hstmt,
    SQLUSMALLINT    iCol,
    SQLUSMALLINT    iField,
    SQLPOINTER      pCharAttr,
    SQLSMALLINT     cbCharAttrMax,
    SQLSMALLINT     *pcbCharAttr,
    SQLLEN          *pNumAttr);
#else
    SQLRETURN SQL_API SQLColAttributeW(
    SQLHSTMT        hstmt,
    SQLUSMALLINT    iCol,
    SQLUSMALLINT    iField,
    SQLPOINTER      pCharAttr,
    SQLSMALLINT     cbCharAttrMax,
    SQLSMALLINT  *pcbCharAttr,
    SQLPOINTER      pNumAttr);
#endif
#endif


SQLRETURN SQL_API SQLConnectW(
    SQLHDBC            hdbc,
    SQLWCHAR        *szDSN,
    SQLSMALLINT        cbDSN,
    SQLWCHAR        *szUID,
    SQLSMALLINT        cbUID,
    SQLWCHAR        *szAuthStr,
    SQLSMALLINT        cbAuthStr);

SQLRETURN SQL_API SQLConnectWInt(
    SQLHDBC            hdbc,
    SQLWCHAR        *szDSN,
    SQLSMALLINT        cbDSN,
    SQLWCHAR        *szUID,
    SQLSMALLINT        cbUID,
    SQLWCHAR        *szAuthStr,
    SQLSMALLINT        cbAuthStr);




SQLRETURN SQL_API SQLErrorW(
    SQLHENV            henv,
    SQLHDBC            hdbc,
    SQLHSTMT           hstmt,
    SQLWCHAR        *szSqlState,
    SQLINTEGER     *pfNativeError,
    SQLWCHAR        *szErrorMsg,
    SQLSMALLINT        cbErrorMsgMax,
    SQLSMALLINT    *pcbErrorMsg);

SQLRETURN SQL_API SQLExecDirectW(
    SQLHSTMT           hstmt,
    SQLWCHAR        *szSqlStr,
    SQLINTEGER         cbSqlStr);

SQLRETURN SQL_API SQLGetConnectAttrW(
    SQLHDBC            hdbc,
    SQLINTEGER         fAttribute,
    SQLPOINTER         rgbValue,
    SQLINTEGER         cbValueMax,
    SQLINTEGER     *pcbValue);

SQLRETURN SQL_API SQLGetCursorNameW(
    SQLHSTMT           hstmt,
    SQLWCHAR        *szCursor,
    SQLSMALLINT        cbCursorMax,
    SQLSMALLINT    *pcbCursor);

SQLRETURN  SQL_API SQLSetDescFieldW(SQLHDESC DescriptorHandle,
                                   SQLSMALLINT RecNumber,
                                   SQLSMALLINT FieldIdentifier,
                                   SQLPOINTER Value,
                                   SQLINTEGER BufferLength);



SQLRETURN SQL_API SQLGetDescFieldW(
    SQLHDESC           hdesc,
    SQLSMALLINT        iRecord,
    SQLSMALLINT        iField,
    SQLPOINTER         rgbValue,
    SQLINTEGER         cbValueMax,
    SQLINTEGER     *pcbValue);


SQLRETURN SQL_API SQLGetDiagFieldW(
    SQLSMALLINT        fHandleType,
    SQLHANDLE          handle,
    SQLSMALLINT        iRecord,
    SQLSMALLINT        fDiagField,
    SQLPOINTER         rgbDiagInfo,
    SQLSMALLINT        cbDiagInfoMax,
    SQLSMALLINT    *pcbDiagInfo);

SQLRETURN SQL_API SQLGetDiagRecW(
    SQLSMALLINT        fHandleType,
    SQLHANDLE          handle,
    SQLSMALLINT        iRecord,
    SQLWCHAR        *szSqlState,
    SQLINTEGER     *pfNativeError,
    SQLWCHAR        *szErrorMsg,
    SQLSMALLINT        cbErrorMsgMax,
    SQLSMALLINT    *pcbErrorMsg);

SQLRETURN SQL_API_FN SQLGetEnvAttrW(
    SQLHENV    hEnv,
    SQLINTEGER fAttribute,
    SQLPOINTER pParam,
    SQLINTEGER cbParamMax,
    SQLINTEGER * pcbParam );

SQLRETURN SQL_API SQLPrepareW(
    SQLHSTMT           hstmt,
    SQLWCHAR        *szSqlStr,
    SQLINTEGER         cbSqlStr);

SQLRETURN SQL_API_FN SQLExtendedPrepareW( SQLHSTMT      hStmt,
                                          SQLWCHAR *    pszSqlStrIn,
                                          SQLINTEGER    cbSqlStr,
                                          SQLINTEGER    cPars,
                                          SQLSMALLINT   sStmtType,
                                          SQLINTEGER    cStmtAttrs,
                                          SQLINTEGER *  piStmtAttr,
                                          SQLINTEGER *  pvParams );

SQLRETURN SQL_API SQLSetConnectAttrW(
    SQLHDBC            hdbc,
    SQLINTEGER         fAttribute,
    SQLPOINTER         rgbValue,
    SQLINTEGER         cbValue);

SQLRETURN SQL_API SQLSetCursorNameW(
    SQLHSTMT           hstmt,
    SQLWCHAR        *szCursor,
    SQLSMALLINT        cbCursor);

SQLRETURN SQL_API_FN  SQLSetEnvAttrW(
    SQLHENV            hEnv,
    SQLINTEGER         fAttribute,
    SQLPOINTER         pParam,
    SQLINTEGER         cbParam );

SQLRETURN SQL_API SQLColumnsW(
    SQLHSTMT           hstmt,
    SQLWCHAR        *szCatalogName,
    SQLSMALLINT        cbCatalogName,
    SQLWCHAR        *szSchemaName,
    SQLSMALLINT        cbSchemaName,
    SQLWCHAR        *szTableName,
    SQLSMALLINT        cbTableName,
    SQLWCHAR        *szColumnName,
    SQLSMALLINT        cbColumnName);

SQLRETURN SQL_API SQLGetInfoW(
    SQLHDBC            hdbc,
    SQLUSMALLINT       fInfoType,
    SQLPOINTER         rgbInfoValue,
    SQLSMALLINT        cbInfoValueMax,
    SQLSMALLINT    *pcbInfoValue);

SQLRETURN SQL_API_FN SQLGetConnectOptionW(
    SQLHDBC hDbc,
    SQLUSMALLINT fOptionIn,
    SQLPOINTER pvParam );


SQLRETURN SQL_API_FN  SQLGetTypeInfoW(
    SQLHSTMT           hstmt,
    SQLSMALLINT        fSqlType);

SQLRETURN SQL_API SQLSpecialColumnsW(
    SQLHSTMT           hstmt,
    SQLUSMALLINT       fColType,
    SQLWCHAR        *szCatalogName,
    SQLSMALLINT        cbCatalogName,
    SQLWCHAR        *szSchemaName,
    SQLSMALLINT        cbSchemaName,
    SQLWCHAR        *szTableName,
    SQLSMALLINT        cbTableName,
    SQLUSMALLINT       fScope,
    SQLUSMALLINT       fNullable);

SQLRETURN SQL_API SQLStatisticsW(
    SQLHSTMT           hstmt,
    SQLWCHAR        *szCatalogName,
    SQLSMALLINT        cbCatalogName,
    SQLWCHAR        *szSchemaName,
    SQLSMALLINT        cbSchemaName,
    SQLWCHAR        *szTableName,
    SQLSMALLINT        cbTableName,
    SQLUSMALLINT       fUnique,
    SQLUSMALLINT       fAccuracy);

SQLRETURN SQL_API SQLTablesW(
    SQLHSTMT           hstmt,
    SQLWCHAR        *szCatalogName,
    SQLSMALLINT        cbCatalogName,
    SQLWCHAR        *szSchemaName,
    SQLSMALLINT        cbSchemaName,
    SQLWCHAR        *szTableName,
    SQLSMALLINT        cbTableName,
    SQLWCHAR        *szTableType,
    SQLSMALLINT        cbTableType);



SQLRETURN SQL_API SQLDataSourcesW(
    SQLHENV            henv,
    SQLUSMALLINT       fDirection,
    SQLWCHAR        *szDSN,
    SQLSMALLINT        cbDSNMax,
    SQLSMALLINT    *pcbDSN,
    SQLWCHAR        *szDescription,
    SQLSMALLINT        cbDescriptionMax,
    SQLSMALLINT    *pcbDescription);




SQLRETURN SQL_API SQLDriverConnectW(
    SQLHDBC            hdbc,
    SQLHWND            hwnd,
    SQLWCHAR        *szConnStrIn,
    SQLSMALLINT        cbConnStrIn,
    SQLWCHAR        *szConnStrOut,
    SQLSMALLINT        cbConnStrOutMax,
    SQLSMALLINT    *pcbConnStrOut,
    SQLUSMALLINT       fDriverCompletion);


SQLRETURN SQL_API SQLBrowseConnectW(
    SQLHDBC            hdbc,
    SQLWCHAR        *szConnStrIn,
    SQLSMALLINT        cbConnStrIn,
    SQLWCHAR        *szConnStrOut,
    SQLSMALLINT        cbConnStrOutMax,
    SQLSMALLINT    *pcbConnStrOut);

SQLRETURN SQL_API SQLColumnPrivilegesW(
    SQLHSTMT           hstmt,
    SQLWCHAR        *szCatalogName,
    SQLSMALLINT        cbCatalogName,
    SQLWCHAR        *szSchemaName,
    SQLSMALLINT        cbSchemaName,
    SQLWCHAR        *szTableName,
    SQLSMALLINT        cbTableName,
    SQLWCHAR        *szColumnName,
    SQLSMALLINT        cbColumnName);

SQLRETURN SQL_API SQLGetStmtAttrW(
    SQLHSTMT           hstmt,
    SQLINTEGER         fAttribute,
    SQLPOINTER         rgbValue,
    SQLINTEGER         cbValueMax,
    SQLINTEGER     *pcbValue);

SQLRETURN SQL_API SQLSetStmtAttrW(
    SQLHSTMT           hstmt,
    SQLINTEGER         fAttribute,
    SQLPOINTER         rgbValue,
    SQLINTEGER         cbValueMax);

SQLRETURN SQL_API SQLForeignKeysW(
    SQLHSTMT           hstmt,
    SQLWCHAR        *szPkCatalogName,
    SQLSMALLINT        cbPkCatalogName,
    SQLWCHAR        *szPkSchemaName,
    SQLSMALLINT        cbPkSchemaName,
    SQLWCHAR        *szPkTableName,
    SQLSMALLINT        cbPkTableName,
    SQLWCHAR        *szFkCatalogName,
    SQLSMALLINT        cbFkCatalogName,
    SQLWCHAR        *szFkSchemaName,
    SQLSMALLINT        cbFkSchemaName,
    SQLWCHAR        *szFkTableName,
    SQLSMALLINT        cbFkTableName);


SQLRETURN SQL_API SQLNativeSqlW(
    SQLHDBC            hdbc,
    SQLWCHAR        *szSqlStrIn,
    SQLINTEGER         cbSqlStrIn,
    SQLWCHAR        *szSqlStr,
    SQLINTEGER         cbSqlStrMax,
    SQLINTEGER     *pcbSqlStr);


SQLRETURN SQL_API SQLPrimaryKeysW(
    SQLHSTMT           hstmt,
    SQLWCHAR        *szCatalogName,
    SQLSMALLINT        cbCatalogName,
    SQLWCHAR        *szSchemaName,
    SQLSMALLINT        cbSchemaName,
    SQLWCHAR        *szTableName,
    SQLSMALLINT        cbTableName);

SQLRETURN SQL_API SQLProcedureColumnsW(
    SQLHSTMT           hstmt,
    SQLWCHAR        *szCatalogName,
    SQLSMALLINT        cbCatalogName,
    SQLWCHAR        *szSchemaName,
    SQLSMALLINT        cbSchemaName,
    SQLWCHAR        *szProcName,
    SQLSMALLINT        cbProcName,
    SQLWCHAR        *szColumnName,
    SQLSMALLINT        cbColumnName);

SQLRETURN SQL_API SQLProceduresW(
    SQLHSTMT           hstmt,
    SQLWCHAR        *szCatalogName,
    SQLSMALLINT        cbCatalogName,
    SQLWCHAR        *szSchemaName,
    SQLSMALLINT        cbSchemaName,
    SQLWCHAR        *szProcName,
    SQLSMALLINT        cbProcName);


SQLRETURN SQL_API SQLTablePrivilegesW(
    SQLHSTMT           hstmt,
    SQLWCHAR        *szCatalogName,
    SQLSMALLINT        cbCatalogName,
    SQLWCHAR        *szSchemaName,
    SQLSMALLINT        cbSchemaName,
    SQLWCHAR        *szTableName,
    SQLSMALLINT        cbTableName);


/*
 * DB2 specific CLI APIs
 */

SQLRETURN SQL_API_FN SQLBindFileToCol  (SQLHSTMT          hstmt,
                                        SQLUSMALLINT      icol,
                                        SQLCHAR     FAR   *FileName,
                                        SQLSMALLINT FAR   *FileNameLength,
                                        SQLUINTEGER FAR   *FileOptions,
                                        SQLSMALLINT       MaxFileNameLength,
                                        SQLINTEGER  FAR   *StringLength,
                                        SQLINTEGER  FAR   *IndicatorValue);

SQLRETURN SQL_API_FN SQLBindFileToParam(SQLHSTMT          hstmt,
                                        SQLUSMALLINT      ipar,
                                        SQLSMALLINT       fSqlType,
                                        SQLCHAR     FAR   *FileName,
                                        SQLSMALLINT FAR   *FileNameLength,
                                        SQLUINTEGER FAR   *FileOptions,
                                        SQLSMALLINT       MaxFileNameLength,
                                        SQLINTEGER  FAR   *IndicatorValue);

SQLRETURN SQL_API_FN SQLGetLength      (SQLHSTMT          hstmt,
                                        SQLSMALLINT       LocatorCType,
                                        SQLINTEGER        Locator,
                                        SQLINTEGER  FAR   *StringLength,
                                        SQLINTEGER  FAR   *IndicatorValue);

SQLRETURN SQL_API_FN SQLGetPosition    (SQLHSTMT          hstmt,
                                        SQLSMALLINT       LocatorCType,
                                        SQLINTEGER        SourceLocator,
                                        SQLINTEGER        SearchLocator,
                                        SQLCHAR     FAR   *SearchLiteral,
                                        SQLINTEGER        SearchLiteralLength,
                                        SQLUINTEGER       FromPosition,
                                        SQLUINTEGER FAR   *LocatedAt,
                                        SQLINTEGER  FAR   *IndicatorValue);


SQLRETURN SQL_API_FN SQLGetSQLCA       (SQLHENV           henv,
                                        SQLHDBC           hdbc,
                                        SQLHSTMT          hstmt,
                                        struct sqlca FAR  *pSqlca );

SQLRETURN SQL_API_FN SQLGetSubString   (SQLHSTMT          hstmt,
                                        SQLSMALLINT       LocatorCType,
                                        SQLINTEGER        SourceLocator,
                                        SQLUINTEGER       FromPosition,
                                        SQLUINTEGER       ForLength,
                                        SQLSMALLINT       TargetCType,
                                        SQLPOINTER        rgbValue,
                                        SQLINTEGER        cbValueMax,
                                        SQLINTEGER  FAR   *StringLength,
                                        SQLINTEGER  FAR   *IndicatorValue);

SQLRETURN SQL_API_FN SQLSetColAttributes (SQLHSTMT        hstmt,
                                        SQLUSMALLINT      icol,
                                        SQLCHAR     FAR   *pszColName,
                                        SQLSMALLINT       cbColName,
                                        SQLSMALLINT       fSQLType,
                                        SQLUINTEGER       cbColDef,
                                        SQLSMALLINT       ibScale,
                                        SQLSMALLINT       fNullable);

/*
 *  Set active connection API, for use in conjunction with embedded
 *  SQL programming only.
 */
SQLRETURN SQL_API_FN SQLSetConnection  (SQLHDBC           hdbc);

/*
 * APIs defined only by X/Open CLI
 */

SQLRETURN SQL_API_FN SQLGetEnvAttr     (SQLHENV           henv,
                                        SQLINTEGER        Attribute,
                                        SQLPOINTER        Value,
                                        SQLINTEGER        BufferLength,
                                        SQLINTEGER  FAR   *StringLength);

SQLRETURN SQL_API_FN SQLSetEnvAttr     (SQLHENV           henv,
                                        SQLINTEGER        Attribute,
                                        SQLPOINTER        Value,
                                        SQLINTEGER        StringLength);


/*
 *  Data link functions.
 */

SQLRETURN SQL_API_FN SQLBuildDataLink(  SQLHSTMT hStmt,
                                        SQLCHAR FAR * pszLinkType,
                                        SQLINTEGER cbLinkType,
                                        SQLCHAR FAR * pszDataLocation,
                                        SQLINTEGER cbDataLocation,
                                        SQLCHAR FAR * pszComment,
                                        SQLINTEGER cbComment,
                                        SQLCHAR FAR * pDataLink,
                                        SQLINTEGER cbDataLinkMax,
                                        SQLINTEGER FAR * pcbDataLink );

SQLRETURN SQL_API_FN SQLGetDataLinkAttr( SQLHSTMT hStmt,
                                         SQLSMALLINT fAttrType,
                                         SQLCHAR FAR * pDataLink,
                                         SQLINTEGER cbDataLink,
                                         SQLPOINTER pAttribute,
                                         SQLINTEGER cbAttributeMax,
                                         SQLINTEGER * pcbAttribute );

/*
 *  DB2 CLI APIs
 */

SQLRETURN SQL_API_FN SQLExtendedPrepare( SQLHSTMT      hstmt,
                                         SQLCHAR *     pszSqlStmt,
                                         SQLINTEGER    cbSqlStmt,
                                         SQLINTEGER    cPars,
                                         SQLSMALLINT   sStmtType,
                                         SQLINTEGER    cStmtAttrs,
                                         SQLINTEGER *  piStmtAttr,
                                         SQLINTEGER *  pvParams );

SQLRETURN SQL_API_FN  SQLExtendedBind    (SQLHSTMT          hstmt,
                                          SQLSMALLINT       fBindCol,
                                          SQLSMALLINT       cRecords,
                                          SQLSMALLINT *     pfCType,
                                          SQLPOINTER  *     rgbValue,
                                          SQLINTEGER  *     cbValueMax,
                                          SQLUINTEGER *     puiPrecisionCType,
                                          SQLSMALLINT *     psScaleCType,
                                          SQLINTEGER  **    pcbValue,
                                          SQLINTEGER  **    piIndicatorPtr,
                                          SQLSMALLINT *     pfParamType,
                                          SQLSMALLINT *     pfSQLType,
                                          SQLUINTEGER *     pcbColDef,
                                          SQLSMALLINT *     pibScale );



#define  SQL_C_WCHAR         SQL_WCHAR
#ifdef UNICODE
#define SQL_C_TCHAR     SQL_C_WCHAR
typedef SQLWCHAR        SQLTCHAR;
#else
#define SQL_C_TCHAR     SQL_C_CHAR
typedef SQLCHAR         SQLTCHAR;
#endif

#define SQLConnectWInt      SQLConnectW

#ifdef  UNICODE
#define SQLColAttribute     SQLColAttributeW
#define SQLColAttributes    SQLColAttributesW
#define SQLConnect          SQLConnectW
#define SQLDescribeCol      SQLDescribeColW
#define SQLError            SQLErrorW
#define SQLExecDirect       SQLExecDirectW
#define SQLGetConnectAttr   SQLGetConnectAttrW
#define SQLGetCursorName    SQLGetCursorNameW
#define SQLGetDescField     SQLGetDescFieldW
#define SQLGetDescRec       SQLGetDescRecW
#define SQLGetDiagField     SQLGetDiagFieldW
#define SQLGetDiagRec       SQLGetDiagRecW
#define SQLGetEnvAttr       SQLGetEnvAttrW
#define SQLPrepare          SQLPrepareW
#define SQLSetConnectAttr   SQLSetConnectAttrW
#define SQLSetCursorName    SQLSetCursorNameW
#define SQLSetDescField     SQLSetDescFieldW
#define SQLSetEnvAttr       SQLSetEnvAttrW
#define SQLSetStmtAttr      SQLSetStmtAttrW
#define SQLGetStmtAttr      SQLGetStmtAttrW
#define SQLColumns          SQLColumnsW
#define SQLGetInfo          SQLGetInfoW
#define SQLSpecialColumns   SQLSpecialColumnsW
#define SQLStatistics       SQLStatisticsW
#define SQLTables           SQLTablesW
#define SQLDataSources      SQLDataSourcesW
#define SQLDriverConnect    SQLDriverConnectW
#define SQLBrowseConnect    SQLBrowseConnectW
#define SQLColumnPrivileges SQLColumnPrivilegesW
#define SQLForeignKeys      SQLForeignKeysW
#define SQLNativeSql        SQLNativeSqlW
#define SQLPrimaryKeys      SQLPrimaryKeysW
#define SQLProcedureColumns SQLProcedureColumnsW
#define SQLProcedures       SQLProceduresW
#define SQLTablePrivileges  SQLTablePrivilegesW
#endif /* UNICODE */

#ifdef DB2_WINSOCKAPI_
#undef _WINSOCKAPI_
#undef DB2_WINSOCKAPI_
#endif

#ifdef __cplusplus
}
#endif

#endif /* SQL_H_SQLCLI1 */











#ifdef __cplusplus
extern "C" {
#endif


/*
http://publib.boulder.ibm.com/infocenter/db2luw/v8/index.jsp?topic=/com.ibm.db2.udb.doc/ad/r0000604.htm
http://publib.boulder.ibm.com/infocenter/dzichelp/v2r2/index.jsp?topic=/com.ibm.db29.doc.odbc/db2z_fngetdata.htm
*/
SQLRETURN   SQLGetData       (
               SQLHSTMT          StatementHandle,     /* hstmt */
               SQLUSMALLINT      ColumnNumber,        /* icol */
               SQLSMALLINT       TargetType,          /* fCType */
               SQLPOINTER        TargetValuePtr,      /* rgbValue */
               SQLINTEGER        BufferLength,        /* cbValueMax */
               SQLINTEGER        *StrLen_or_IndPtr);  /* pcbValue */ 




SQLRETURN  SQLBindParameter(
              SQLHSTMT          StatementHandle,     /* hstmt */
              SQLUSMALLINT      ParameterNumber,     /* ipar */
              SQLSMALLINT       InputOutputType,     /* fParamType */
              SQLSMALLINT       ValueType,           /* fCType */
              SQLSMALLINT       ParameterType,       /* fSqlType */
              SQLUINTEGER       ColumnSize,          /* cbColDef */
              SQLSMALLINT       DecimalDigits,       /* ibScale */
              SQLPOINTER        ParameterValuePtr,   /* rgbValue */
              SQLINTEGER        BufferLength,        /* cbValueMax */
              SQLINTEGER        *StrLen_or_IndPtr);  /* pcbValue */ 

SQLRETURN   SQLRowCount (
               SQLHSTMT     StatementHandle,   /* hstmt */
               SQLINTEGER   *RowCountPtr);     /* pcrow */ 

SQLRETURN   SQLBindCol (
               SQLHSTMT          StatementHandle,            /* hstmt */
               SQLUSMALLINT      ColumnNumber,               /* icol */
               SQLSMALLINT       TargetType,                 /* fCType */
               SQLPOINTER        TargetValuePtr,             /* rgbValue */
               SQLINTEGER        BufferLength,               /* dbValueMax */
               SQLINTEGER        *StrLen_or_IndPtr);         /* *pcbValue */ 

SQLRETURN   SQLDescribeCol (
               SQLHSTMT          StatementHandle,    /* hstmt */
               SQLUSMALLINT      ColumnNumber,       /* icol */
               SQLCHAR           *ColumnName,        /* szColName */
               SQLSMALLINT       BufferLength,       /* cbColNameMax */
               SQLSMALLINT       *NameLengthPtr,     /* pcbColName */
               SQLSMALLINT       *DataTypePtr,       /* pfSqlType */
               SQLUINTEGER       *ColumnSizePtr,     /* pcbColDef */
               SQLSMALLINT       *DecimalDigitsPtr,  /* pibScale */
               SQLSMALLINT       *NullablePtr);      /* pfNullable */ 


#ifdef __cplusplus
}
#endif



#else // #if 0

#if !defined DB2NT
#define DB2NT     1
#endif
/* If DB2CLI_VER is not defined, assume version 2.10 */
#ifndef DB2CLI_VER
#define DB2CLI_VER 0x0310
#endif



#define SQL_ATTR_LONGDATA_COMPAT    SQL_LONGDATA_COMPAT
#define SQL_LONGDATA_COMPAT                        1253
#define SQL_LD_COMPAT_YES            1
#define SQL_ATTR_CLOSE_BEHAVIOR      1257
#define SQL_CC_RELEASE                1



#endif // #if 0



#endif
