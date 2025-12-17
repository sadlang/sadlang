// بسم الله الرحمن الرحيم
/**
 * @file database_module.h
 * @brief Database Module - SQLite Integration - وحدة قاعدة البيانات
 * 
 * This module provides comprehensive SQLite database functionality for Sad Language.
 * هذه الوحدة توفر وظائف شاملة لقاعدة بيانات SQLite للغة الحزينة
 * 
 * Features / المميزات:
 * - Database connection management / إدارة اتصال قاعدة البيانات
 * - SQL query execution / تنفيذ استعلامات SQL
 * - Prepared statements / الاستعلامات المحضرة
 * - Transaction support / دعم المعاملات
 * - Result set handling / معالجة مجموعات النتائج
 * - ORM basics / أساسيات ORM
 * - Connection pooling / تجميع الاتصالات
 * - Query builder / بناء الاستعلامات
 * - Database migrations / هجرة قاعدة البيانات
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <variant>
#include <chrono>

// Forward declaration for SQLite
struct sqlite3;
struct sqlite3_stmt;

namespace sad {
namespace stdlib {
namespace database {

// ============================================================================
// Type Definitions - تعريفات الأنواع
// ============================================================================

/**
 * @brief Database value types - أنواع قيم قاعدة البيانات
 */
using DbValue = std::variant<
    std::monostate,     // NULL
    int64_t,           // INTEGER
    double,            // REAL
    std::string,       // TEXT
    std::vector<uint8_t> // BLOB
>;

/**
 * @brief Database row (column name -> value)
 * صف قاعدة البيانات (اسم العمود -> القيمة)
 */
using DbRow = std::map<std::string, DbValue>;

/**
 * @brief Database result set (list of rows)
 * مجموعة نتائج قاعدة البيانات (قائمة الصفوف)
 */
using DbResultSet = std::vector<DbRow>;

/**
 * @brief Database column type - نوع عمود قاعدة البيانات
 */
enum class DbColumnType {
    INTEGER,    // صحيح
    REAL,       // حقيقي
    TEXT,       // نص
    BLOB,       // ثنائي
    NULL_TYPE   // فارغ
};

/**
 * @brief Transaction isolation level - مستوى عزل المعاملة
 */
enum class IsolationLevel {
    READ_UNCOMMITTED,  // قراءة غير مؤكدة
    READ_COMMITTED,    // قراءة مؤكدة
    REPEATABLE_READ,   // قراءة قابلة للتكرار
    SERIALIZABLE       // متسلسل
};

/**
 * @brief Database open mode - وضع فتح قاعدة البيانات
 */
enum class DbOpenMode {
    READ_ONLY,      // قراءة فقط
    READ_WRITE,     // قراءة وكتابة
    CREATE,         // إنشاء إذا لم تكن موجودة
    MEMORY          // في الذاكرة
};

// ============================================================================
// Column Info Structure - معلومات العمود
// ============================================================================

/**
 * @brief Column information - معلومات العمود
 */
struct ColumnInfo {
    std::string name;           // اسم العمود / Column name
    DbColumnType type;          // نوع العمود / Column type
    bool not_null;              // غير فارغ / Not null
    bool primary_key;           // مفتاح أساسي / Primary key
    bool auto_increment;        // زيادة تلقائية / Auto increment
    std::optional<DbValue> default_value; // القيمة الافتراضية / Default value
    
    ColumnInfo() : type(DbColumnType::TEXT), not_null(false), 
                   primary_key(false), auto_increment(false) {}
};

// ============================================================================
// Database Exception - استثناء قاعدة البيانات
// ============================================================================

/**
 * @brief Database exception class - فئة استثناء قاعدة البيانات
 */
class DatabaseException : public std::runtime_error {
public:
    explicit DatabaseException(const std::string& message) 
        : std::runtime_error(message) {}
};

// ============================================================================
// Prepared Statement Class - فئة الاستعلام المحضر
// ============================================================================

/**
 * @brief Prepared statement for safe SQL execution
 * استعلام محضر للتنفيذ الآمن لـ SQL
 */
class PreparedStatement {
public:
    PreparedStatement(sqlite3* db, const std::string& sql);
    ~PreparedStatement();
    
    // منع النسخ / Prevent copying
    PreparedStatement(const PreparedStatement&) = delete;
    PreparedStatement& operator=(const PreparedStatement&) = delete;
    
    // السماح بالنقل / Allow moving
    PreparedStatement(PreparedStatement&& other) noexcept;
    PreparedStatement& operator=(PreparedStatement&& other) noexcept;
    
    /**
     * @brief Bind parameter by index (1-based)
     * ربط المعامل بالفهرس (يبدأ من 1)
     */
    void bind(int index, const DbValue& value);
    
    /**
     * @brief Bind parameter by name (e.g., ":name")
     * ربط المعامل بالاسم (مثال: ":name")
     */
    void bind(const std::string& name, const DbValue& value);
    
    /**
     * @brief Execute statement and return result set
     * تنفيذ الاستعلام وإرجاع مجموعة النتائج
     */
    DbResultSet execute();
    
    /**
     * @brief Execute statement and return number of affected rows
     * تنفيذ الاستعلام وإرجاع عدد الصفوف المتأثرة
     */
    int execute_update();
    
    /**
     * @brief Reset statement for reuse
     * إعادة تعيين الاستعلام لإعادة الاستخدام
     */
    void reset();
    
    /**
     * @brief Clear all bindings
     * مسح جميع الربطات
     */
    void clear_bindings();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ============================================================================
// Transaction Class - فئة المعاملة
// ============================================================================

/**
 * @brief Database transaction with RAII semantics
 * معاملة قاعدة البيانات مع دلالات RAII
 */
class Transaction {
public:
    explicit Transaction(sqlite3* db);
    ~Transaction();
    
    // منع النسخ والنقل / Prevent copying and moving
    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;
    Transaction(Transaction&&) = delete;
    Transaction& operator=(Transaction&&) = delete;
    
    /**
     * @brief Commit transaction
     * تأكيد المعاملة
     */
    void commit();
    
    /**
     * @brief Rollback transaction
     * التراجع عن المعاملة
     */
    void rollback();
    
    /**
     * @brief Check if transaction is active
     * التحقق مما إذا كانت المعاملة نشطة
     */
    bool is_active() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ============================================================================
// Database Connection Class - فئة اتصال قاعدة البيانات
// ============================================================================

/**
 * @brief Database connection class
 * فئة اتصال قاعدة البيانات
 */
class Database {
public:
    /**
     * @brief Default constructor
     * المنشئ الافتراضي
     */
    Database();
    
    /**
     * @brief Constructor with database path
     * منشئ مع مسار قاعدة البيانات
     */
    explicit Database(const std::string& path, DbOpenMode mode = DbOpenMode::CREATE);
    
    /**
     * @brief Destructor
     * المدمر
     */
    ~Database();
    
    // منع النسخ / Prevent copying
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
    // السماح بالنقل / Allow moving
    Database(Database&& other) noexcept;
    Database& operator=(Database&& other) noexcept;
    
    // ========================================================================
    // Connection Management - إدارة الاتصال
    // ========================================================================
    
    /**
     * @brief Open database connection
     * فتح اتصال قاعدة البيانات
     */
    void open(const std::string& path, DbOpenMode mode = DbOpenMode::CREATE);
    
    /**
     * @brief Close database connection
     * إغلاق اتصال قاعدة البيانات
     */
    void close();
    
    /**
     * @brief Check if database is open
     * التحقق مما إذا كانت قاعدة البيانات مفتوحة
     */
    bool is_open() const;
    
    /**
     * @brief Get database file path
     * الحصول على مسار ملف قاعدة البيانات
     */
    std::string get_path() const;
    
    // ========================================================================
    // Query Execution - تنفيذ الاستعلامات
    // ========================================================================
    
    /**
     * @brief Execute SQL query and return result set
     * تنفيذ استعلام SQL وإرجاع مجموعة النتائج
     */
    DbResultSet execute(const std::string& sql);
    
    /**
     * @brief Execute SQL update/insert/delete and return affected rows
     * تنفيذ تحديث/إدراج/حذف SQL وإرجاع الصفوف المتأثرة
     */
    int execute_update(const std::string& sql);
    
    /**
     * @brief Execute SQL query and return single row
     * تنفيذ استعلام SQL وإرجاع صف واحد
     */
    std::optional<DbRow> execute_single(const std::string& sql);
    
    /**
     * @brief Execute SQL query and return single value
     * تنفيذ استعلام SQL وإرجاع قيمة واحدة
     */
    std::optional<DbValue> execute_scalar(const std::string& sql);
    
    // ========================================================================
    // Prepared Statements - الاستعلامات المحضرة
    // ========================================================================
    
    /**
     * @brief Create prepared statement
     * إنشاء استعلام محضر
     */
    PreparedStatement prepare(const std::string& sql);
    
    // ========================================================================
    // Transaction Management - إدارة المعاملات
    // ========================================================================
    
    /**
     * @brief Begin transaction
     * بدء معاملة
     */
    std::unique_ptr<Transaction> begin_transaction();
    
    /**
     * @brief Execute function within transaction
     * تنفيذ دالة ضمن معاملة
     */
    void transaction(std::function<void()> func);
    
    // ========================================================================
    // Schema Information - معلومات المخطط
    // ========================================================================
    
    /**
     * @brief Get list of tables in database
     * الحصول على قائمة الجداول في قاعدة البيانات
     */
    std::vector<std::string> get_tables();
    
    /**
     * @brief Check if table exists
     * التحقق مما إذا كان الجدول موجودًا
     */
    bool table_exists(const std::string& table_name);
    
    /**
     * @brief Get columns of a table
     * الحصول على أعمدة الجدول
     */
    std::vector<ColumnInfo> get_columns(const std::string& table_name);
    
    /**
     * @brief Get primary key columns of a table
     * الحصول على أعمدة المفتاح الأساسي للجدول
     */
    std::vector<std::string> get_primary_keys(const std::string& table_name);
    
    /**
     * @brief Get indexes of a table
     * الحصول على فهارس الجدول
     */
    std::vector<std::string> get_indexes(const std::string& table_name);
    
    // ========================================================================
    // Utility Functions - دوال مساعدة
    // ========================================================================
    
    /**
     * @brief Get last insert row ID
     * الحصول على معرف آخر صف مُدرج
     */
    int64_t last_insert_rowid();
    
    /**
     * @brief Get number of rows changed by last statement
     * الحصول على عدد الصفوف التي تغيرت بآخر استعلام
     */
    int changes();
    
    /**
     * @brief Get total number of changes since connection opened
     * الحصول على إجمالي عدد التغييرات منذ فتح الاتصال
     */
    int total_changes();
    
    /**
     * @brief Enable/disable foreign keys
     * تمكين/تعطيل المفاتيح الخارجية
     */
    void set_foreign_keys(bool enabled);
    
    /**
     * @brief Backup database to another file
     * نسخ قاعدة البيانات إلى ملف آخر
     */
    void backup(const std::string& dest_path);
    
    /**
     * @brief Optimize database (VACUUM)
     * تحسين قاعدة البيانات (VACUUM)
     */
    void vacuum();
    
    /**
     * @brief Get database version
     * الحصول على إصدار قاعدة البيانات
     */
    std::string get_version();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    friend class PreparedStatement;
    friend class Transaction;
    
    sqlite3* get_handle() const;
};

// ============================================================================
// Query Builder Class - فئة بناء الاستعلامات
// ============================================================================

/**
 * @brief Query builder for constructing SQL queries programmatically
 * بناء الاستعلامات لإنشاء استعلامات SQL برمجيًا
 */
class QueryBuilder {
public:
    QueryBuilder();
    ~QueryBuilder();
    
    /**
     * @brief Start SELECT query
     * بدء استعلام SELECT
     */
    QueryBuilder& select(const std::vector<std::string>& columns = {"*"});
    
    /**
     * @brief Set FROM table
     * تعيين جدول FROM
     */
    QueryBuilder& from(const std::string& table);
    
    /**
     * @brief Add WHERE condition
     * إضافة شرط WHERE
     */
    QueryBuilder& where(const std::string& condition);
    
    /**
     * @brief Add AND condition
     * إضافة شرط AND
     */
    QueryBuilder& and_where(const std::string& condition);
    
    /**
     * @brief Add OR condition
     * إضافة شرط OR
     */
    QueryBuilder& or_where(const std::string& condition);
    
    /**
     * @brief Add ORDER BY clause
     * إضافة جملة ORDER BY
     */
    QueryBuilder& order_by(const std::string& column, bool ascending = true);
    
    /**
     * @brief Add LIMIT clause
     * إضافة جملة LIMIT
     */
    QueryBuilder& limit(int count);
    
    /**
     * @brief Add OFFSET clause
     * إضافة جملة OFFSET
     */
    QueryBuilder& offset(int count);
    
    /**
     * @brief Add JOIN clause
     * إضافة جملة JOIN
     */
    QueryBuilder& join(const std::string& table, const std::string& condition);
    
    /**
     * @brief Add LEFT JOIN clause
     * إضافة جملة LEFT JOIN
     */
    QueryBuilder& left_join(const std::string& table, const std::string& condition);
    
    /**
     * @brief Add GROUP BY clause
     * إضافة جملة GROUP BY
     */
    QueryBuilder& group_by(const std::string& column);
    
    /**
     * @brief Add HAVING clause
     * إضافة جملة HAVING
     */
    QueryBuilder& having(const std::string& condition);
    
    /**
     * @brief Start INSERT query
     * بدء استعلام INSERT
     */
    QueryBuilder& insert_into(const std::string& table);
    
    /**
     * @brief Set values for INSERT
     * تعيين قيم INSERT
     */
    QueryBuilder& values(const std::map<std::string, DbValue>& values);
    
    /**
     * @brief Start UPDATE query
     * بدء استعلام UPDATE
     */
    QueryBuilder& update(const std::string& table);
    
    /**
     * @brief Set values for UPDATE
     * تعيين قيم UPDATE
     */
    QueryBuilder& set(const std::map<std::string, DbValue>& values);
    
    /**
     * @brief Start DELETE query
     * بدء استعلام DELETE
     */
    QueryBuilder& delete_from(const std::string& table);
    
    /**
     * @brief Build and return SQL query
     * بناء وإرجاع استعلام SQL
     */
    std::string build() const;
    
    /**
     * @brief Reset builder to initial state
     * إعادة تعيين البناء إلى الحالة الأولية
     */
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ============================================================================
// ORM (Object-Relational Mapping) - ORM (ربط الكائنات بالعلاقات)
// ============================================================================

/**
 * @brief Simple ORM base class
 * فئة ORM الأساسية البسيطة
 */
class Model {
public:
    virtual ~Model() = default;
    
    /**
     * @brief Get table name
     * الحصول على اسم الجدول
     */
    virtual std::string get_table_name() const = 0;
    
    /**
     * @brief Convert model to database row
     * تحويل النموذج إلى صف قاعدة بيانات
     */
    virtual DbRow to_row() const = 0;
    
    /**
     * @brief Load model from database row
     * تحميل النموذج من صف قاعدة البيانات
     */
    virtual void from_row(const DbRow& row) = 0;
    
    /**
     * @brief Save model to database
     * حفظ النموذج في قاعدة البيانات
     */
    bool save(Database& db);
    
    /**
     * @brief Delete model from database
     * حذف النموذج من قاعدة البيانات
     */
    bool remove(Database& db);
    
    /**
     * @brief Get primary key column name
     * الحصول على اسم عمود المفتاح الأساسي
     */
    virtual std::string get_primary_key() const { return "id"; }
    
    /**
     * @brief Check if model exists in database
     * التحقق مما إذا كان النموذج موجودًا في قاعدة البيانات
     */
    bool exists(Database& db) const;
};

// ============================================================================
// Utility Functions - الدوال المساعدة
// ============================================================================

/**
 * @brief Convert DbValue to string for display
 * تحويل DbValue إلى سلسلة نصية للعرض
 */
std::string db_value_to_string(const DbValue& value);

/**
 * @brief Get type name of DbValue
 * الحصول على اسم نوع DbValue
 */
std::string db_value_type_name(const DbValue& value);

/**
 * @brief Check if DbValue is null
 * التحقق مما إذا كان DbValue فارغًا
 */
bool is_null(const DbValue& value);

/**
 * @brief Convert DbColumnType to string
 * تحويل DbColumnType إلى سلسلة نصية
 */
std::string column_type_to_string(DbColumnType type);

/**
 * @brief Escape string for SQL (basic escaping)
 * تجنب السلسلة النصية لـ SQL (تجنب أساسي)
 */
std::string sql_escape(const std::string& str);

/**
 * @brief Format SQL query for readability
 * تنسيق استعلام SQL للقراءة
 */
std::string format_sql(const std::string& sql);

} // namespace database
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين
