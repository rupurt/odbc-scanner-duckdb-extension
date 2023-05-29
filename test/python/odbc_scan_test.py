import duckdb

def test_odbc_scan():
    conn = duckdb.connect('');
    conn.execute("SELECT odbc_scan('Sam') as value;");
    res = conn.fetchall()
    assert(res[0][0] == "Odbc_scan Sam 🐥");