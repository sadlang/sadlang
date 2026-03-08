# وثائق API — وحدة database

> المسار: `stdlib/database/`
> مساحة الأسماء: `sad::stdlib::database`

## نظرة عامة
توفر الوحدة غلافاً لقاعدة البيانات (SQLite-style) مع تنفيذ مباشر وPrepared Statements.

## الملفات الأساسية
- `database/database_module.h`
- `database/database_module.cpp`

## API رئيسية
- `class Database`
  - `open(path, mode)`
  - `close()`
  - `is_open()`
  - `execute(sql)`
  - `execute_update(sql)`
  - `execute_single(sql)`
  - `prepare(sql)`
  - `table_exists(name)`
- `enum class DbOpenMode`
  - `READ_ONLY`, `MEMORY`, ...
- `class PreparedStatement`
  - `bind(index, DbValue)`
  - `bind(name, DbValue)`
  - `execute()`, `execute_update()`
- `class DatabaseException`

## مثال سريع
```cpp
using namespace sad::stdlib::database;
Database db("app.db");
db.execute_update("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT)");
```
