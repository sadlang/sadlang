# بسم الله الرحمن الرحيم
# 🗄️ Phase 2.2 - Database & ORM System
# المرحلة 2.2 - قواعد البيانات ونظام ORM

**التاريخ / Date:** 7 يناير 2026  
**المرحلة / Phase:** Phase 2.2 - Database & ORM  
**الحالة / Status:** 🔵 **مخطط / PLANNED**  
**المدة المتوقعة / Duration:** 6-8 أسابيع  
**الأولوية / Priority:** 🔴 **حرجة / CRITICAL**

---

## 🎯 الهدف الرئيسي / Main Objective

بناء نظام شامل لقواعد البيانات يدعم SQL و NoSQL مع ORM متقدم، مما يمكن المطورين من بناء تطبيقات قواعد بيانات احترافية بسهولة.

### لماذا Database & ORM؟
```
✅ لا تطبيقات حديثة بدون قواعد بيانات
✅ ORM يبسط العمل مع البيانات
✅ دعم SQL & NoSQL يوسع الإمكانيات
✅ منافسة مع Django ORM, TypeORM, GORM
✅ أساسي لـ Full-stack development
```

---

## 📋 المكونات الرئيسية (3 مكونات)

```
Phase 2.2 - Database & ORM
├── 2.2.1: SQL Drivers (الأسبوع 1-3)
│   ├── PostgreSQL Driver
│   ├── MySQL/MariaDB Driver
│   ├── SQLite Driver (تحسين الموجود)
│   ├── Connection Pooling
│   └── Prepared Statements
│
├── 2.2.2: ORM System (الأسبوع 4-6)
│   ├── Model Definition
│   ├── Query Builder
│   ├── Relations (One-to-One, One-to-Many, Many-to-Many)
│   ├── Migrations System
│   ├── Schema Builder
│   └── Transaction Support
│
└── 2.2.3: NoSQL Drivers (الأسبوع 7-8)
    ├── MongoDB Driver
    ├── Redis Driver
    ├── Basic query support
    └── Connection management
```

---

## 🔧 Component 2.2.1 - SQL Drivers

### الأهداف
```
✅ دعم 3 قواعد بيانات SQL رئيسية
✅ واجهة برمجية موحدة
✅ Connection pooling فعال
✅ Prepared statements للأمان
✅ Transaction support
✅ معالجة أخطاء شاملة
```

### البنية المطلوبة

#### هيكل الملفات
```
include/sad/database/
├── connection.h               (Base connection interface)
├── result.h                   (Query result set)
├── statement.h                (Prepared statement)
├── transaction.h              (Transaction handling)
├── connection_pool.h          (Connection pooling)
├── database_error.h           (Error handling)
│
├── drivers/
│   ├── driver_interface.h     (Driver interface)
│   ├── postgres_driver.h      (PostgreSQL driver)
│   ├── mysql_driver.h         (MySQL driver)
│   └── sqlite_driver.h        (SQLite driver)
│
└── types/
    ├── value.h                (Generic value type)
    ├── row.h                  (Row representation)
    └── column.h               (Column metadata)

src/database/
├── connection.cpp             (~400 lines)
├── result.cpp                 (~350 lines)
├── statement.cpp              (~450 lines)
├── transaction.cpp            (~300 lines)
├── connection_pool.cpp        (~600 lines)
├── database_error.cpp         (~200 lines)
│
├── drivers/
│   ├── postgres_driver.cpp    (~900 lines)
│   ├── mysql_driver.cpp       (~850 lines)
│   └── sqlite_driver.cpp      (~500 lines)
│
└── types/
    ├── value.cpp              (~400 lines)
    ├── row.cpp                (~250 lines)
    └── column.cpp             (~200 lines)

stdlib/database/
├── postgres.s                 (PostgreSQL API)
├── mysql.s                    (MySQL API)
├── sqlite.s                   (SQLite API)
└── examples/
    ├── postgres_example.s
    ├── mysql_example.s
    └── sqlite_example.s

tests/database/
├── test_postgres.cpp
├── test_mysql.cpp
├── test_sqlite.cpp
├── test_connection_pool.cpp
└── test_transactions.cpp
```

### الميزات المطلوبة

#### PostgreSQL Driver
```
- الاتصال بقاعدة البيانات
- تنفيذ استعلامات SQL
- Prepared statements
- Parameterized queries
- Binary protocol support
- COPY command support
- Notification system (LISTEN/NOTIFY)
- Transaction isolation levels
- Connection string parsing
- SSL/TLS support
```

#### MySQL Driver
```
- الاتصال بـ MySQL/MariaDB
- تنفيذ الاستعلامات
- Prepared statements
- Multiple result sets
- Stored procedures
- Binary protocol
- Character set handling
- Transaction support
- Connection options
- SSL support
```

#### SQLite Driver (تحسين الموجود)
```
- الاتصال بملف SQLite
- In-memory database
- WAL mode support
- Prepared statements
- Transaction support
- Backup & restore
- User-defined functions
- Virtual tables
- Full-text search (FTS5)
```

#### Connection Pooling
```
- Pool creation & management
- Connection acquisition & release
- Min/Max pool size
- Connection timeout
- Health checks
- Connection recycling
- Thread-safe operations
- Statistics & monitoring
```

### المكتبات الخارجية
```
Dependencies:
├── libpq (PostgreSQL)
├── libmysqlclient (MySQL)
├── sqlite3 (موجود بالفعل)
└── OpenSSL (للاتصالات الآمنة)
```

### التقدم الحالي
```
التقييم: 10% (SQLite driver موجود)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░ 10%

ملاحظة: SQLite موجود في src/stdlib/database/
يحتاج تحسين ودمج مع البنية الجديدة
```

---

## 🏗️ Component 2.2.2 - ORM System

### الأهداف
```
✅ ORM كامل بواجهة بسيطة
✅ Model definition بـ Sad
✅ Query Builder قوي
✅ Relations support (1-1, 1-N, N-N)
✅ Migrations system
✅ Schema Builder
✅ Transactions
✅ Eager/Lazy loading
```

### البنية المطلوبة

#### هيكل الملفات
```
include/sad/database/orm/
├── model.h                    (Base model class)
├── query_builder.h            (Query construction)
├── relation.h                 (Relations support)
├── migration.h                (Schema migrations)
├── schema_builder.h           (Table creation)
├── collection.h               (Model collection)
├── paginator.h                (Pagination)
└── validator.h                (Data validation)

src/database/orm/
├── model.cpp                  (~1000 lines)
├── query_builder.cpp          (~1500 lines)
├── relation.cpp               (~800 lines)
├── migration.cpp              (~600 lines)
├── schema_builder.cpp         (~900 lines)
├── collection.cpp             (~450 lines)
├── paginator.cpp              (~350 lines)
└── validator.cpp              (~500 lines)

stdlib/database/
├── orm.s                      (ORM API)
├── model.s                    (Model base class)
├── query.s                    (Query builder)
└── examples/
    ├── user_model.s
    ├── blog_models.s
    ├── ecommerce_models.s
    └── relations_example.s

tests/database/orm/
├── test_model.cpp
├── test_query_builder.cpp
├── test_relations.cpp
├── test_migrations.cpp
└── test_validation.cpp
```

### الميزات المطلوبة

#### Model Features
```
- تعريف Models بـ Sad classes
- Auto-generated IDs
- Timestamps (created_at, updated_at)
- Soft deletes
- Custom attributes
- Type casting
- Mutators & Accessors
- Serialization (to JSON)
- Validation rules
```

#### Query Builder
```
- SELECT queries (all columns or specific)
- WHERE conditions (=, !=, >, <, >=, <=, LIKE, IN, BETWEEN)
- AND/OR chaining
- ORDER BY (ASC/DESC)
- LIMIT & OFFSET
- GROUP BY & HAVING
- Joins (INNER, LEFT, RIGHT, FULL)
- Subqueries
- Aggregates (COUNT, SUM, AVG, MIN, MAX)
- Raw queries
```

#### Relations
```
- One-to-One (hasOne, belongsTo)
- One-to-Many (hasMany, belongsTo)
- Many-to-Many (belongsToMany)
- Polymorphic relations
- Eager loading (with, load)
- Lazy loading
- Relationship constraints
```

#### Migrations
```
- Create/Drop tables
- Add/Remove columns
- Modify columns
- Indexes (create, drop)
- Foreign keys
- Up/Down methods
- Migration runner
- Version control
- Rollback support
```

#### Schema Builder
```
- Table creation
- Column types (INT, VARCHAR, TEXT, DATE, etc.)
- Primary keys
- Foreign keys
- Indexes (unique, composite)
- Default values
- Nullable columns
- Auto-increment
```

### التقدم الحالي
```
التقييم: 0%
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░ 0%

هذا النظام يحتاج بناء من الصفر
```

---

## 📊 Component 2.2.3 - NoSQL Drivers

### الأهداف
```
✅ MongoDB driver
✅ Redis driver
✅ واجهة برمجية بسيطة
✅ Connection management
✅ Basic CRUD operations
```

### البنية المطلوبة

#### هيكل الملفات
```
include/sad/database/nosql/
├── mongodb/
│   ├── client.h               (MongoDB client)
│   ├── database.h             (Database operations)
│   ├── collection.h           (Collection operations)
│   └── document.h             (Document handling)
│
└── redis/
    ├── client.h               (Redis client)
    ├── commands.h             (Redis commands)
    └── pubsub.h               (Pub/Sub support)

src/database/nosql/
├── mongodb/
│   ├── client.cpp             (~600 lines)
│   ├── database.cpp           (~400 lines)
│   ├── collection.cpp         (~800 lines)
│   └── document.cpp           (~500 lines)
│
└── redis/
    ├── client.cpp             (~700 lines)
    ├── commands.cpp           (~900 lines)
    └── pubsub.cpp             (~400 lines)

stdlib/database/
├── mongodb.s                  (MongoDB API)
├── redis.s                    (Redis API)
└── examples/
    ├── mongodb_example.s
    └── redis_example.s

tests/database/nosql/
├── test_mongodb.cpp
└── test_redis.cpp
```

### الميزات المطلوبة

#### MongoDB Features
```
- الاتصال بـ MongoDB
- Database selection
- Collection operations
- CRUD operations (Insert, Find, Update, Delete)
- Query filters
- Aggregation pipeline
- Indexes
- Transactions (MongoDB 4.0+)
- GridFS (for large files)
```

#### Redis Features
```
- الاتصال بـ Redis
- String operations (GET, SET, DEL, etc.)
- Hash operations (HSET, HGET, etc.)
- List operations (LPUSH, RPOP, etc.)
- Set operations (SADD, SMEMBERS, etc.)
- Sorted set operations (ZADD, ZRANGE, etc.)
- Pub/Sub
- Transactions (MULTI/EXEC)
- Pipelining
- Lua scripting
```

### المكتبات الخارجية
```
Dependencies:
├── mongoc (MongoDB C driver)
│   └── libbson
└── hiredis (Redis C client)
```

### التقدم الحالي
```
التقييم: 0%
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░ 0%
```

---

## 📅 الجدول الزمني التفصيلي / Detailed Timeline

### الأسبوع 1-3: SQL Drivers
```
الأسبوع 1: PostgreSQL
├─ الأيام 1-2: تصميم الـ API
├─ الأيام 3-5: كتابة postgres_driver
├─ الأيام 6-7: اختبارات PostgreSQL

الأسبوع 2: MySQL & SQLite
├─ الأيام 1-3: كتابة mysql_driver
├─ الأيام 4-5: تحسين sqlite_driver
├─ الأيام 6-7: اختبارات MySQL & SQLite

الأسبوع 3: Connection Pooling & Integration
├─ الأيام 1-3: Connection pooling
├─ الأيام 4-5: دمج جميع الـ drivers
├─ الأيام 6-7: اختبارات شاملة + أمثلة
```

### الأسبوع 4-6: ORM System
```
الأسبوع 4: Model & Query Builder
├─ الأيام 1-3: Model base class
├─ الأيام 4-7: Query Builder

الأسبوع 5: Relations & Schema
├─ الأيام 1-3: Relations system
├─ الأيام 4-5: Schema Builder
├─ الأيام 6-7: Migrations system

الأسبوع 6: Advanced Features & Testing
├─ الأيام 1-2: Validation & Collection
├─ الأيام 3-4: Pagination
├─ الأيام 5-7: اختبارات ORM + أمثلة
```

### الأسبوع 7-8: NoSQL & Finalization
```
الأسبوع 7: NoSQL Drivers
├─ الأيام 1-3: MongoDB driver
├─ الأيام 4-6: Redis driver
├─ الأيام 7: اختبارات NoSQL

الأسبوع 8: Testing & Documentation
├─ الأيام 1-2: Integration testing
├─ الأيام 3-4: Performance testing
├─ الأيام 5-7: التوثيق الكامل + تقرير
```

---

## 📊 التقدم الإجمالي / Overall Progress

```
Phase 2.2 Progress: ~3% (SQLite موجود فقط)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░ 3%

Component 2.2.1 - SQL Drivers:    10% ███░░░░░░░
Component 2.2.2 - ORM System:      0% ░░░░░░░░░░
Component 2.2.3 - NoSQL Drivers:   0% ░░░░░░░░░░
```

### إحصائيات الملفات
```
┌──────────────────────┬────────────┬──────────┐
│ Component            │ Files      │ Lines    │
├──────────────────────┼────────────┼──────────┤
│ SQL Drivers          │ ~2/30      │ ~500/7,500│
│ ORM System           │ 0/20       │ 0/6,600  │
│ NoSQL Drivers        │ 0/12       │ 0/4,300  │
├──────────────────────┼────────────┼──────────┤
│ TOTAL                │ ~2/62      │ ~500/18,400│
└──────────────────────┴────────────┴──────────┘
```

---

## 🎯 معايير النجاح / Success Criteria

### Functionality
```
✓ 3 SQL drivers working (Postgres, MySQL, SQLite)
✓ Connection pooling (10-100 connections)
✓ ORM with full CRUD
✓ Relations (1-1, 1-N, N-N) working
✓ Migrations system functional
✓ 2 NoSQL drivers (MongoDB, Redis)
```

### Performance
```
✓ Connection pool: <1ms acquisition time
✓ Simple query: <5ms execution
✓ ORM overhead: <20% vs raw SQL
✓ 1000+ queries/second
```

### Testing
```
✓ 150+ unit tests
✓ 50+ integration tests
✓ Code coverage >80%
✓ All examples working
```

---

## 🔧 CMakeLists.txt Integration

### المطلوب إضافته
```cmake
# ═══ Database Library ═══
add_library(sad_database STATIC
    # Core
    src/database/connection.cpp
    src/database/result.cpp
    src/database/statement.cpp
    src/database/connection_pool.cpp
    
    # SQL Drivers
    src/database/drivers/postgres_driver.cpp
    src/database/drivers/mysql_driver.cpp
    src/database/drivers/sqlite_driver.cpp
    
    # ORM
    src/database/orm/model.cpp
    src/database/orm/query_builder.cpp
    src/database/orm/relation.cpp
    src/database/orm/migration.cpp
    
    # NoSQL
    src/database/nosql/mongodb/client.cpp
    src/database/nosql/redis/client.cpp
)

target_link_libraries(sad_database
    PostgreSQL::PostgreSQL
    MySQL::MySQL
    SQLite::SQLite3
    mongo::mongoc_shared
    hiredis::hiredis
)
```

---

## 📝 ملاحظات التنفيذ / Implementation Notes

### الأولويات
```
1. SQL Drivers أولاً (الأساس)
2. ORM System (القيمة الأكبر)
3. NoSQL Drivers (توسيع القدرات)
```

### النقاط المهمة
```
✅ ابدأ بـ SQLite لأنه موجود
✅ اكتب الواجهة الموحدة أولاً
✅ اختبر كل driver بشكل منفصل
✅ ORM يحتاج تصميم دقيق
✅ Migrations نظام حساس (versioning)
```

### التحديات المتوقعة
```
⚠️ Type mapping بين C++ و SQL
⚠️ ORM relation complexity
⚠️ Migration rollback safety
⚠️ Connection pool thread-safety
⚠️ NoSQL document mapping
```

---

**الحمد لله**  
*Phase 2.2 Database & ORM Plan - Ready to Start!*

---

*خطة المرحلة 2.2 - قواعد البيانات ونظام ORM*  
*التاريخ / Date: January 7, 2026*  
*النسخة / Version: 1.0*  
*الحالة / Status: 🔵 PLANNED*
