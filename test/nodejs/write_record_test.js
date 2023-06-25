var duckdb = require('../../duckdb/tools/nodejs');
var assert = require('assert');

describe(`write_record`, () => {
    let db;
    let conn;
    before((done) => {
        db = new duckdb.Database(':memory:');
        conn = new duckdb.Connection(db);
        done();
    });

    it('function should return expected constant', function (done) {
        db.all("SELECT write_record('Sam') as value;", function (err, res) {
            if (err) throw err;
            assert.deepEqual(res, [{value: "TODO: write_record Sam 🐥"}]);
            done();
        });
    });
});
