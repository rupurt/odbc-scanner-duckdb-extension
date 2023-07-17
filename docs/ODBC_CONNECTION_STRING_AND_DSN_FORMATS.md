# ODBC Connection String and DSN Formats

`odbc-scanner-duckdb` supports standard ODBC connection string and DSN formats. A detailed list can be obtained
from [connectionstrings.com](https://www.connectionstrings.com).

## Find the Path to your Nix managed ODBC Driver

```shell
nix run .#odbc-driver-paths
db2 /nix/store/6flbacf9h5bk09iw37b7sncgjn9mdkwj-db2-odbc-driver-11.5.8/lib/libdb2.so
postgres /nix/store/j648cwmz16prd2n35h0xdhji9b02pip6-postgres-odbc-driver-15.00.0000/lib/psqlodbca.so
mysql /nix/store/j648cwmz16prd2n35h0xdhji9b02pip6-postgres-odbc-driver-15.00.0000/lib/psqlodbca.so
```

## DSN's

### Db2

```odbc.ini
[db2 odbctest]
Driver = db2
```

```odbcinst.ini
[db2]
Driver = ${DB2_DRIVER_PATH}
```

### Postgres

```odbc.ini
[postgres odbc_test]
Driver = postgres
```

```odbcinst.ini
[postgres]
Driver = ${POSTGRES_DRIVER_PATH}
```

### MySQL

```odbc.ini
[mysql odbc_test]
Driver = mysql
```

```odbcinst.ini
[mysql]
Driver = ${MYSQL_DRIVER_PATH}
```

## Connection Strings

### Db2

```
Driver=/nix/store/py6m0q4ij50pwjk6a5f18qhhahrvf2sk-db2-driver-11.5.8/lib/libdb2.so;Hostname=localhost;Database=odbctest;Uid=db2inst1;Pwd=password;Port=50000
```

### Postgres

```
Driver=/nix/store/j648cwmz16prd2n35h0xdhji9b02pip6-postgres-odbc-driver-15.00.0000/lib/psqlodbca.so;Server=localhost;Database=odbc_test;Uid=postgres;Pwd=password;Port=5432
```

### MySQL

```
Driver=/nix/store/j648cwmz16prd2n35h0xdhji9b02pip6-postgres-odbc-driver-15.00.0000/lib/psqlodbca.so;Server=localhost;Database=odbc_test;Uid=mysql;Pwd=password;Port=3306
```
