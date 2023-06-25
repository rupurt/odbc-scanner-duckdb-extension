import duckdb

def test_analyze_record():
    conn = duckdb.connect('');
    conn.execute("SELECT analyze_record('Sam') as value;");
    res = conn.fetchall()
    assert(res[0][0] == "TODO: analyze_record Sam 🐥");
