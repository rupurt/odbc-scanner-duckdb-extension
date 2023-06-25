import duckdb

def test_write_record():
    conn = duckdb.connect('');
    conn.execute("SELECT write_record('Sam') as value;");
    res = conn.fetchall()
    assert(res[0][0] == "TODO: write_record Sam 🐥");
