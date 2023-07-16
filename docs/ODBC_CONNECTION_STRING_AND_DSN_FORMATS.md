# ODBC Connection String and DSN Formats

`odbc-scanner-duckdb` supports standard ODBC connection string and DSN formats. A detailed list can be obtained
from [connectionstrings.com](https://www.connectionstrings.com).

## Find the Path to your Nix managed ODBC Driver

```shell
nix run .#odbc-driver-paths
db2 /nix/store/6flbacf9h5bk09iw37b7sncgjn9mdkwj-db2-odbc-driver-11.5.8/lib/libdb2.so
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

## Connection Strings

### Db2

```
Driver=/nix/store/py6m0q4ij50pwjk6a5f18qhhahrvf2sk-db2-driver-11.5.8/lib/libdb2.so;Hostname=localhost;Database=odbctest;Uid=db2inst1;Pwd=password;Port=50000
```
