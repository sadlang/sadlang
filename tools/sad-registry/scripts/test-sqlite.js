const { DatabaseSync } = require('node:sqlite');
const db = new DatabaseSync(':memory:');
db.exec('CREATE TABLE t(x INT)');
db.prepare('INSERT INTO t VALUES(?)').run(42);
console.log(db.prepare('SELECT * FROM t').get());
db.close();
console.log('node:sqlite OK');
