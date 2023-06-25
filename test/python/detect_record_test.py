import duckdb

def test_detect_record():
    conn = duckdb.connect('');
    conn.execute("SELECT detect_record('Sam') as value;");
    res = conn.fetchall()
    assert(res[0][0] == "TODO: detect_record Sam 🐥");
