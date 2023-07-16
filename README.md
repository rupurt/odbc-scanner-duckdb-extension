# ODBC Scanner DuckDB Extension

A DuckDB extension to read data directly from databases supporting the ODBC interface

### odbc_scan

```shell
ODBCSYSINI=${PWD} ODBCINSTINI=.odbcinst.ini ODBCINI=.odbc.ini ./build/release/duckdb
```
```duckdb
D select * from odbc_scan(
    'Driver={db2 odbctest};Hostname=localhost;Database=odbctest;Uid=db2inst1;Pwd=password;Port=50000',
    'DB2INST1',
    'PEOPLE'
);
┌──────────────┬───────┬───────────────┐
│     NAME     │  AGE  │    SALARY     │
│   varchar    │ int32 │ decimal(20,2) │
├──────────────┼───────┼───────────────┤
│ Lebron James │    37 │        100.10 │
│ Spiderman    │    25 │        200.20 │
│ Wonder Woman │    22 │        300.30 │
│ David Bowie  │    69 │        400.40 │
└──────────────┴───────┴───────────────┘
```

## Known Supported Databases

This extension is tested and known to work with the ODBC drivers of the following databases.

| Database   | Tests                                                    | x86_64 | aarch64 |
| ---------- | :------------------------------------------------------: | :----: | :-----: |
| IBM Db2    | [odbc_scan_db2](./test/sql/odbc_scan_db2.test)           | `[x]`  | `[ ]`   |
| MSSQL      | [odbc_scan_msql](./test/sql/odbc_scan_mssql.test)        | `[ ]`  | `[ ]`   |
| Oracle     | [odbc_scan_oracle](./test/sql/odbc_scan_oracle.test)     | `[ ]`  | `[ ]`   |
| Postgres   | [odbc_scan_postgres](./test/sql/odbc_scan_postgres.test) | `[x]`  | `[x]`   |
| MariaDB    | [odbc_scan_mariadb](./test/sql/odbc_scan_mariadb.test)   | `[ ]`  | `[ ]`   |

## Planned Supported Databases

| Database   | Tests                                                       | x86_64 | aarch64 |
| ---------- | :---------------------------------------------------------: | :----: | :-----: |
| Snowflake  | [odbc_scan_snowflake](./test/sql/odbc_scan_snowflake.test)  | `[ ]`  | `[ ]`   |

If you have tested the extension against other databases let us know by opening an [issue](https://github.com/rupurt/odbc-scanner-duckdb-extension/issues/new)
or creating a pull request with a set of tests.

## Connection String and DSN Formats

For a full list of supported values read the [connection string](./docs/ODBC_CONNECTION_STRING_AND_DSN_FORMATS.md) documentation.

## Development

This repository manages development dependencies such drivers and shared libraries with [nix](https://nixos.org). It assumes you
have it [installed](https://github.com/DeterminateSystems/nix-installer).

All `development` and `test` tasks should be run within a nix shell

```shell
nix develop -c $SHELL
```

The `odbc-scanner-duckdb-extension` is built with a `clang` toolchain. To enable `clangd` LSP support run the `.clangd`
generator nix application.

```shell
nix run .#generate-dot-clangd
```

To build the extension with the official DuckDB `cmake` toolchain and `clangd` run the build nix application which will link
to the correct version of `unixodbc`.

```shell
nix run .#build
./build/release/duckdb
```

To use ODBC DSN's with driver paths managed by the `odbc-drivers-nix` flake run the generate nix apps.

```shell
nix run .#generate-odbc-ini
nix run .#generate-odbcinst-ini
```

## Test

Run the official DuckDB `cmake` builder with `nix` to ensure `unixodbc` is linked correctly

```shell
docker compose up
nix run .#test
```

## Installing the deployed binaries

To install your extension binaries from S3, you will need to do two things. Firstly, DuckDB should be launched with the
`allow_unsigned_extensions` option set to true. How to set this will depend on the client you're using. Some examples:

CLI:
```shell
duckdb -unsigned
```

Python:
```python
con = duckdb.connect(':memory:', config={'allow_unsigned_extensions' : 'true'})
```

NodeJS:
```js
db = new duckdb.Database(':memory:', {"allow_unsigned_extensions": "true"});
```

Secondly, you will need to set the repository endpoint in DuckDB to the HTTP url of your bucket + version of the extension
you want to install. To do this run the following SQL query in DuckDB:
```sql
SET custom_extension_repository='bucket.s3.eu-west-1.amazonaws.com/<your_extension_name>/latest';
```
Note that the `/latest` path will allow you to install the latest extension version available for your current version of
DuckDB. To specify a specific version, you can pass the version instead.

After running these steps, you can install and load your extension using the regular INSTALL/LOAD commands in DuckDB:
```sql
INSTALL 'build/release/extension/odbc_scanner/odbc_scanner.duckdb_extension';
LOAD 'build/release/extension/odbc_scanner/odbc_scanner.duckdb_extension';
```

## License

`odbc-scanner-duckdb-extension` is released under the [MIT license](./LICENSE)
