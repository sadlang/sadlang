// بسم الله الرحمن الرحيم
/**
 * @file test_database_module.cpp
 * @brief Comprehensive Tests for Database Module - اختبارات شاملة لوحدة قاعدة البيانات
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#include <gtest/gtest.h>
#include "stdlib/database/database_module.h"
#include <filesystem>

using namespace sad::stdlib::database;
namespace fs = std::filesystem;

// ============================================================================
// Test Fixture - إعداد الاختبار
// ============================================================================

class DatabaseTest : public ::testing::Test {
protected:
    std::string test_db_path;
    
    void SetUp() override {
        test_db_path = "test_database.db";
        // إزالة قاعدة بيانات الاختبار السابقة إن وجدت
        if (fs::exists(test_db_path)) {
            fs::remove(test_db_path);
        }
    }
    
    void TearDown() override {
        // تنظيف قاعدة البيانات بعد كل اختبار
        if (fs::exists(test_db_path)) {
            fs::remove(test_db_path);
        }
    }
};

// ============================================================================
// Database Connection Tests - اختبارات الاتصال
// ============================================================================

TEST_F(DatabaseTest, OpenDatabase) {
    Database db;
    
    ASSERT_NO_THROW({
        db.open(test_db_path);
    });
    
    EXPECT_TRUE(db.is_open());
    EXPECT_EQ(test_db_path, db.get_path());
}

TEST_F(DatabaseTest, OpenMemoryDatabase) {
    Database db;
    
    ASSERT_NO_THROW({
        db.open(":memory:", DbOpenMode::MEMORY);
    });
    
    EXPECT_TRUE(db.is_open());
}

TEST_F(DatabaseTest, CloseDatabase) {
    Database db(test_db_path);
    
    EXPECT_TRUE(db.is_open());
    
    db.close();
    
    EXPECT_FALSE(db.is_open());
}

TEST_F(DatabaseTest, ConstructorWithPath) {
    ASSERT_NO_THROW({
        Database db(test_db_path);
        EXPECT_TRUE(db.is_open());
    });
}

TEST_F(DatabaseTest, ReadOnlyMode) {
    // إنشاء قاعدة بيانات أولاً
    {
        Database db(test_db_path);
        db.execute_update("CREATE TABLE test (id INTEGER)");
    }
    
    // فتح في وضع القراءة فقط
    Database db;
    ASSERT_NO_THROW({
        db.open(test_db_path, DbOpenMode::READ_ONLY);
    });
    
    EXPECT_TRUE(db.is_open());
    
    // يجب أن يفشل الكتابة
    EXPECT_THROW({
        db.execute_update("INSERT INTO test VALUES (1)");
    }, DatabaseException);
}

// ============================================================================
// Basic SQL Execution Tests - اختبارات التنفيذ الأساسية لـ SQL
// ============================================================================

TEST_F(DatabaseTest, CreateTable) {
    Database db(test_db_path);
    
    ASSERT_NO_THROW({
        db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)");
    });
    
    EXPECT_TRUE(db.table_exists("users"));
}

TEST_F(DatabaseTest, InsertData) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    
    int affected = db.execute_update("INSERT INTO users (id, name) VALUES (1, 'Ahmad')");
    
    EXPECT_EQ(1, affected);
}

TEST_F(DatabaseTest, SelectData) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    db.execute_update("INSERT INTO users (id, name) VALUES (1, 'Ahmad')");
    db.execute_update("INSERT INTO users (id, name) VALUES (2, 'Fatima')");
    
    DbResultSet results = db.execute("SELECT * FROM users ORDER BY id");
    
    ASSERT_EQ(2, results.size());
    
    EXPECT_EQ(int64_t(1), std::get<int64_t>(results[0].at("id")));
    EXPECT_EQ("Ahmad", std::get<std::string>(results[0].at("name")));
    
    EXPECT_EQ(int64_t(2), std::get<int64_t>(results[1].at("id")));
    EXPECT_EQ("Fatima", std::get<std::string>(results[1].at("name")));
}

TEST_F(DatabaseTest, UpdateData) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    db.execute_update("INSERT INTO users (id, name) VALUES (1, 'Ahmad')");
    
    int affected = db.execute_update("UPDATE users SET name = 'Ahmed' WHERE id = 1");
    
    EXPECT_EQ(1, affected);
    
    auto row = db.execute_single("SELECT name FROM users WHERE id = 1");
    ASSERT_TRUE(row.has_value());
    EXPECT_EQ("Ahmed", std::get<std::string>(row->at("name")));
}

TEST_F(DatabaseTest, DeleteData) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    db.execute_update("INSERT INTO users (id, name) VALUES (1, 'Ahmad')");
    db.execute_update("INSERT INTO users (id, name) VALUES (2, 'Fatima')");
    
    int affected = db.execute_update("DELETE FROM users WHERE id = 1");
    
    EXPECT_EQ(1, affected);
    
    DbResultSet results = db.execute("SELECT * FROM users");
    ASSERT_EQ(1, results.size());
    EXPECT_EQ(int64_t(2), std::get<int64_t>(results[0].at("id")));
}

// ============================================================================
// Prepared Statement Tests - اختبارات الاستعلامات المحضرة
// ============================================================================

TEST_F(DatabaseTest, PreparedStatementInsert) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)");
    
    auto stmt = db.prepare("INSERT INTO users (id, name, age) VALUES (?, ?, ?)");
    
    stmt.bind(1, DbValue(int64_t(1)));
    stmt.bind(2, DbValue(std::string("Ahmad")));
    stmt.bind(3, DbValue(int64_t(25)));
    
    int affected = stmt.execute_update();
    
    EXPECT_EQ(1, affected);
}

TEST_F(DatabaseTest, PreparedStatementSelect) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    db.execute_update("INSERT INTO users (id, name) VALUES (1, 'Ahmad')");
    db.execute_update("INSERT INTO users (id, name) VALUES (2, 'Fatima')");
    
    auto stmt = db.prepare("SELECT * FROM users WHERE id = ?");
    stmt.bind(1, DbValue(int64_t(1)));
    
    DbResultSet results = stmt.execute();
    
    ASSERT_EQ(1, results.size());
    EXPECT_EQ("Ahmad", std::get<std::string>(results[0].at("name")));
}

TEST_F(DatabaseTest, PreparedStatementNamedParameters) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    
    auto stmt = db.prepare("INSERT INTO users (id, name) VALUES (:id, :name)");
    
    stmt.bind(":id", DbValue(int64_t(1)));
    stmt.bind(":name", DbValue(std::string("Ahmad")));
    
    int affected = stmt.execute_update();
    
    EXPECT_EQ(1, affected);
}

TEST_F(DatabaseTest, PreparedStatementReuse) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    
    auto stmt = db.prepare("INSERT INTO users (id, name) VALUES (?, ?)");
    
    // إدراج أول
    stmt.bind(1, DbValue(int64_t(1)));
    stmt.bind(2, DbValue(std::string("Ahmad")));
    stmt.execute_update();
    
    // إعادة تعيين وإدراج ثاني
    stmt.reset();
    stmt.bind(1, DbValue(int64_t(2)));
    stmt.bind(2, DbValue(std::string("Fatima")));
    stmt.execute_update();
    
    // التحقق
    DbResultSet results = db.execute("SELECT COUNT(*) as cnt FROM users");
    EXPECT_EQ(int64_t(2), std::get<int64_t>(results[0].at("cnt")));
}

// ============================================================================
// Transaction Tests - اختبارات المعاملات
// ============================================================================

TEST_F(DatabaseTest, TransactionCommit) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    
    {
        auto txn = db.begin_transaction();
        
        db.execute_update("INSERT INTO users (id, name) VALUES (1, 'Ahmad')");
        db.execute_update("INSERT INTO users (id, name) VALUES (2, 'Fatima')");
        
        txn->commit();
    }
    
    DbResultSet results = db.execute("SELECT COUNT(*) as cnt FROM users");
    EXPECT_EQ(int64_t(2), std::get<int64_t>(results[0].at("cnt")));
}

TEST_F(DatabaseTest, TransactionRollback) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    
    {
        auto txn = db.begin_transaction();
        
        db.execute_update("INSERT INTO users (id, name) VALUES (1, 'Ahmad')");
        db.execute_update("INSERT INTO users (id, name) VALUES (2, 'Fatima')");
        
        txn->rollback();
    }
    
    DbResultSet results = db.execute("SELECT COUNT(*) as cnt FROM users");
    EXPECT_EQ(int64_t(0), std::get<int64_t>(results[0].at("cnt")));
}

TEST_F(DatabaseTest, TransactionAutoRollback) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    
    {
        auto txn = db.begin_transaction();
        
        db.execute_update("INSERT INTO users (id, name) VALUES (1, 'Ahmad')");
        
        // عدم استدعاء commit - يجب أن يتم التراجع تلقائيًا
    }
    
    DbResultSet results = db.execute("SELECT COUNT(*) as cnt FROM users");
    EXPECT_EQ(int64_t(0), std::get<int64_t>(results[0].at("cnt")));
}

TEST_F(DatabaseTest, TransactionLambda) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    
    ASSERT_NO_THROW({
        db.transaction([&]() {
            db.execute_update("INSERT INTO users (id, name) VALUES (1, 'Ahmad')");
            db.execute_update("INSERT INTO users (id, name) VALUES (2, 'Fatima')");
        });
    });
    
    DbResultSet results = db.execute("SELECT COUNT(*) as cnt FROM users");
    EXPECT_EQ(int64_t(2), std::get<int64_t>(results[0].at("cnt")));
}

// ============================================================================
// Schema Information Tests - اختبارات معلومات المخطط
// ============================================================================

TEST_F(DatabaseTest, GetTables) {
    Database db(test_db_path);
    
    db.execute_update("CREATE TABLE users (id INTEGER)");
    db.execute_update("CREATE TABLE posts (id INTEGER)");
    db.execute_update("CREATE TABLE comments (id INTEGER)");
    
    auto tables = db.get_tables();
    
    EXPECT_EQ(3, tables.size());
    EXPECT_TRUE(std::find(tables.begin(), tables.end(), "users") != tables.end());
    EXPECT_TRUE(std::find(tables.begin(), tables.end(), "posts") != tables.end());
    EXPECT_TRUE(std::find(tables.begin(), tables.end(), "comments") != tables.end());
}

TEST_F(DatabaseTest, TableExists) {
    Database db(test_db_path);
    
    db.execute_update("CREATE TABLE users (id INTEGER)");
    
    EXPECT_TRUE(db.table_exists("users"));
    EXPECT_FALSE(db.table_exists("posts"));
}

TEST_F(DatabaseTest, GetColumns) {
    Database db(test_db_path);
    
    db.execute_update(
        "CREATE TABLE users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "age INTEGER, "
        "email TEXT"
        ")"
    );
    
    auto columns = db.get_columns("users");
    
    ASSERT_EQ(4, columns.size());
    
    EXPECT_EQ("id", columns[0].name);
    EXPECT_EQ(DbColumnType::INTEGER, columns[0].type);
    EXPECT_TRUE(columns[0].primary_key);
    
    EXPECT_EQ("name", columns[1].name);
    EXPECT_EQ(DbColumnType::TEXT, columns[1].type);
    EXPECT_TRUE(columns[1].not_null);
    
    EXPECT_EQ("age", columns[2].name);
    EXPECT_EQ(DbColumnType::INTEGER, columns[2].type);
    
    EXPECT_EQ("email", columns[3].name);
    EXPECT_EQ(DbColumnType::TEXT, columns[3].type);
}

TEST_F(DatabaseTest, GetPrimaryKeys) {
    Database db(test_db_path);
    
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    
    auto pks = db.get_primary_keys("users");
    
    ASSERT_EQ(1, pks.size());
    EXPECT_EQ("id", pks[0]);
}

// ============================================================================
// Utility Functions Tests - اختبارات الدوال المساعدة
// ============================================================================

TEST_F(DatabaseTest, LastInsertRowId) {
    Database db(test_db_path);
    
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT)");
    db.execute_update("INSERT INTO users (name) VALUES ('Ahmad')");
    
    int64_t id = db.last_insert_rowid();
    
    EXPECT_EQ(1, id);
    
    db.execute_update("INSERT INTO users (name) VALUES ('Fatima')");
    
    id = db.last_insert_rowid();
    
    EXPECT_EQ(2, id);
}

TEST_F(DatabaseTest, Changes) {
    Database db(test_db_path);
    
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    db.execute_update("INSERT INTO users VALUES (1, 'Ahmad'), (2, 'Fatima'), (3, 'Ali')");
    
    db.execute_update("UPDATE users SET name = 'Updated' WHERE id <= 2");
    
    int changes = db.changes();
    
    EXPECT_EQ(2, changes);
}

TEST_F(DatabaseTest, TotalChanges) {
    Database db(test_db_path);
    
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    db.execute_update("INSERT INTO users VALUES (1, 'Ahmad')");
    db.execute_update("INSERT INTO users VALUES (2, 'Fatima')");
    db.execute_update("UPDATE users SET name = 'Updated'");
    
    int total = db.total_changes();
    
    EXPECT_GE(total, 4); // At least 2 inserts + 2 updates
}

TEST_F(DatabaseTest, Vacuum) {
    Database db(test_db_path);
    
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    db.execute_update("INSERT INTO users VALUES (1, 'Ahmad')");
    
    ASSERT_NO_THROW({
        db.vacuum();
    });
}

TEST_F(DatabaseTest, GetVersion) {
    Database db(test_db_path);
    
    std::string version = db.get_version();
    
    EXPECT_FALSE(version.empty());
    EXPECT_TRUE(version.find("3.") != std::string::npos); // SQLite 3.x
}

TEST_F(DatabaseTest, Backup) {
    Database db(test_db_path);
    
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    db.execute_update("INSERT INTO users VALUES (1, 'Ahmad')");
    
    std::string backup_path = "backup_test.db";
    
    ASSERT_NO_THROW({
        db.backup(backup_path);
    });
    
    EXPECT_TRUE(fs::exists(backup_path));
    
    // التحقق من محتوى النسخة الاحتياطية
    Database backup_db(backup_path);
    DbResultSet results = backup_db.execute("SELECT * FROM users");
    
    ASSERT_EQ(1, results.size());
    EXPECT_EQ("Ahmad", std::get<std::string>(results[0].at("name")));
    
    // تنظيف
    fs::remove(backup_path);
}

// ============================================================================
// QueryBuilder Tests - اختبارات QueryBuilder
// ============================================================================

TEST_F(DatabaseTest, QueryBuilderSelect) {
    QueryBuilder qb;
    
    std::string sql = qb.select({"id", "name", "email"})
                        .from("users")
                        .where("age > 18")
                        .order_by("name")
                        .limit(10)
                        .build();
    
    EXPECT_TRUE(sql.find("SELECT id, name, email") != std::string::npos);
    EXPECT_TRUE(sql.find("FROM users") != std::string::npos);
    EXPECT_TRUE(sql.find("WHERE age > 18") != std::string::npos);
    EXPECT_TRUE(sql.find("ORDER BY name ASC") != std::string::npos);
    EXPECT_TRUE(sql.find("LIMIT 10") != std::string::npos);
}

TEST_F(DatabaseTest, QueryBuilderSelectAll) {
    QueryBuilder qb;
    
    std::string sql = qb.select().from("users").build();
    
    EXPECT_TRUE(sql.find("SELECT *") != std::string::npos);
    EXPECT_TRUE(sql.find("FROM users") != std::string::npos);
}

TEST_F(DatabaseTest, QueryBuilderWhere) {
    QueryBuilder qb;
    
    std::string sql = qb.select()
                        .from("users")
                        .where("age > 18")
                        .and_where("status = 'active'")
                        .or_where("role = 'admin'")
                        .build();
    
    EXPECT_TRUE(sql.find("WHERE age > 18") != std::string::npos);
    EXPECT_TRUE(sql.find("AND status = 'active'") != std::string::npos);
    EXPECT_TRUE(sql.find("OR role = 'admin'") != std::string::npos);
}

TEST_F(DatabaseTest, QueryBuilderJoin) {
    QueryBuilder qb;
    
    std::string sql = qb.select()
                        .from("users")
                        .join("posts", "users.id = posts.user_id")
                        .build();
    
    EXPECT_TRUE(sql.find("JOIN posts ON users.id = posts.user_id") != std::string::npos);
}

TEST_F(DatabaseTest, QueryBuilderGroupBy) {
    QueryBuilder qb;
    
    std::string sql = qb.select({"category", "COUNT(*) as count"})
                        .from("products")
                        .group_by("category")
                        .having("COUNT(*) > 5")
                        .build();
    
    EXPECT_TRUE(sql.find("GROUP BY category") != std::string::npos);
    EXPECT_TRUE(sql.find("HAVING COUNT(*) > 5") != std::string::npos);
}

TEST_F(DatabaseTest, QueryBuilderInsert) {
    QueryBuilder qb;
    
    std::map<std::string, DbValue> values;
    values["id"] = DbValue(int64_t(1));
    values["name"] = DbValue(std::string("Ahmad"));
    
    std::string sql = qb.insert_into("users").values(values).build();
    
    EXPECT_TRUE(sql.find("INSERT INTO users") != std::string::npos);
    EXPECT_TRUE(sql.find("(id, name)") != std::string::npos || sql.find("(name, id)") != std::string::npos);
    EXPECT_TRUE(sql.find("VALUES (?, ?)") != std::string::npos);
}

TEST_F(DatabaseTest, QueryBuilderUpdate) {
    QueryBuilder qb;
    
    std::map<std::string, DbValue> values;
    values["name"] = DbValue(std::string("Ahmad"));
    values["age"] = DbValue(int64_t(25));
    
    std::string sql = qb.update("users")
                        .set(values)
                        .where("id = 1")
                        .build();
    
    EXPECT_TRUE(sql.find("UPDATE users") != std::string::npos);
    EXPECT_TRUE(sql.find("SET") != std::string::npos);
    EXPECT_TRUE(sql.find("WHERE id = 1") != std::string::npos);
}

TEST_F(DatabaseTest, QueryBuilderDelete) {
    QueryBuilder qb;
    
    std::string sql = qb.delete_from("users")
                        .where("id = 1")
                        .build();
    
    EXPECT_TRUE(sql.find("DELETE FROM users") != std::string::npos);
    EXPECT_TRUE(sql.find("WHERE id = 1") != std::string::npos);
}

// ============================================================================
// DbValue Tests - اختبارات DbValue
// ============================================================================

TEST_F(DatabaseTest, DbValueNull) {
    DbValue val = std::monostate{};
    
    EXPECT_TRUE(is_null(val));
    EXPECT_EQ("NULL", db_value_to_string(val));
    EXPECT_EQ("NULL", db_value_type_name(val));
}

TEST_F(DatabaseTest, DbValueInteger) {
    DbValue val = int64_t(42);
    
    EXPECT_FALSE(is_null(val));
    EXPECT_EQ("42", db_value_to_string(val));
    EXPECT_EQ("INTEGER", db_value_type_name(val));
}

TEST_F(DatabaseTest, DbValueReal) {
    DbValue val = 3.14159;
    
    EXPECT_FALSE(is_null(val));
    EXPECT_TRUE(db_value_to_string(val).find("3.14") != std::string::npos);
    EXPECT_EQ("REAL", db_value_type_name(val));
}

TEST_F(DatabaseTest, DbValueText) {
    DbValue val = std::string("مرحبا");
    
    EXPECT_FALSE(is_null(val));
    EXPECT_EQ("مرحبا", db_value_to_string(val));
    EXPECT_EQ("TEXT", db_value_type_name(val));
}

TEST_F(DatabaseTest, DbValueBlob) {
    std::vector<uint8_t> data = {0x01, 0x02, 0x03};
    DbValue val = data;
    
    EXPECT_FALSE(is_null(val));
    EXPECT_EQ("<BLOB>", db_value_to_string(val));
    EXPECT_EQ("BLOB", db_value_type_name(val));
}

// ============================================================================
// Edge Cases and Error Handling - الحالات الحدية ومعالجة الأخطاء
// ============================================================================

TEST_F(DatabaseTest, InvalidSQL) {
    Database db(test_db_path);
    
    EXPECT_THROW({
        db.execute("INVALID SQL QUERY");
    }, DatabaseException);
}

TEST_F(DatabaseTest, ExecuteOnClosedDatabase) {
    Database db;
    
    EXPECT_THROW({
        db.execute("SELECT 1");
    }, DatabaseException);
}

TEST_F(DatabaseTest, DoubleOpen) {
    Database db(test_db_path);
    
    EXPECT_THROW({
        db.open("another.db");
    }, DatabaseException);
}

TEST_F(DatabaseTest, EmptyResultSet) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    
    DbResultSet results = db.execute("SELECT * FROM users");
    
    EXPECT_TRUE(results.empty());
}

TEST_F(DatabaseTest, ExecuteScalarEmpty) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    
    auto value = db.execute_scalar("SELECT name FROM users");
    
    EXPECT_FALSE(value.has_value());
}

TEST_F(DatabaseTest, SqlEscape) {
    std::string input = "O'Brien";
    std::string escaped = sql_escape(input);
    
    EXPECT_EQ("O''Brien", escaped);
}

TEST_F(DatabaseTest, ColumnTypeToString) {
    EXPECT_EQ("INTEGER", column_type_to_string(DbColumnType::INTEGER));
    EXPECT_EQ("REAL", column_type_to_string(DbColumnType::REAL));
    EXPECT_EQ("TEXT", column_type_to_string(DbColumnType::TEXT));
    EXPECT_EQ("BLOB", column_type_to_string(DbColumnType::BLOB));
    EXPECT_EQ("NULL", column_type_to_string(DbColumnType::NULL_TYPE));
}

// ============================================================================
// Performance Tests - اختبارات الأداء
// ============================================================================

TEST_F(DatabaseTest, BulkInsert) {
    Database db(test_db_path);
    db.execute_update("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    
    auto stmt = db.prepare("INSERT INTO users (id, name) VALUES (?, ?)");
    
    auto txn = db.begin_transaction();
    
    for (int i = 1; i <= 1000; ++i) {
        stmt.reset();
        stmt.bind(1, DbValue(int64_t(i)));
        stmt.bind(2, DbValue(std::string("User_") + std::to_string(i)));
        stmt.execute_update();
    }
    
    txn->commit();
    
    auto count = db.execute_scalar("SELECT COUNT(*) FROM users");
    ASSERT_TRUE(count.has_value());
    EXPECT_EQ(int64_t(1000), std::get<int64_t>(*count));
}

// ============================================================================
// Main Test Runner - مشغل الاختبارات الرئيسي
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// الحمد لله رب العالمين
