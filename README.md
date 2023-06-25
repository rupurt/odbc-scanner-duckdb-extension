# ODBC Scanner DuckDB Extension

A DuckDB extension to read data directly from databases supporting the ODBC interface

### odbc_scan

```duckdb
D select * from odbc_scan('DSN...', 'db2inst1', 'people');
┌──────────────┐─────────┐
│    name      │   age   │
│   varchar    │   int   │
├──────────────┤─────────┤
│ Lebron James │   36    │
│ Spiderman    │   25    │
│ Wonder Woman │   22    │
│ David Bowie  │   69    │
└──────────────┘─────────┘
```

## Development

Use the official DuckDB `cmake` builder

```sh
make
```

Or... use the experimental `nix` flake and `zig` builder within this repo

```sh
nix run .#build-fast
```

## Test

```sh
make test
```

### Installing the deployed binaries

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
