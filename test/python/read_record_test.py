import duckdb

def test_read_record():
    conn = duckdb.connect('');
    conn.execute("SELECT read_record('Sam') as value;");
    res = conn.fetchall()
    assert(res[0][0] == "TODO: read_record Sam 🐥");
