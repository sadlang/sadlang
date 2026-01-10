// بسم الله الرحمن الرحيم
/**
 * @file database_builtins.cpp
 * @brief Database Module Builtin Functions - دوال قاعدة البيانات المدمجة
 * 
 * This file provides interpreter integration for database functionality.
 * هذا الملف يوفر تكامل المفسر لوظائف قاعدة البيانات
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#include "database/database_module.h"
#include "data/types/value.h"
#include "interpreter/value.h"
#include <memory>
#include <map>

namespace sad {
namespace stdlib {
namespace database {

// ============================================================================
// Global Database Instances - نسخ قاعدة البيانات العامة
// ============================================================================

static std::map<std::string, std::unique_ptr<Database>> g_databases;
static std::string g_current_db_name = "default";

// ============================================================================
// Helper Functions - دوال مساعدة
// ============================================================================

namespace {

using Value = sad::interpreter::Value;
using ValueType = sad::interpreter::ValueType;

/**
 * @brief Get string argument from args
 * الحصول على وسيط نصي من args
 */
std::string get_string_arg(const std::vector<Value>& args, size_t index, const std::string& func_name) {
    if (index >= args.size()) {
        throw std::runtime_error(func_name + ": Missing argument at index " + std::to_string(index));
    }
    
    if (args[index].type != ValueType::STRING) {
        throw std::runtime_error(func_name + ": Argument at index " + std::to_string(index) + " must be a string");
    }
    
    return args[index].string_val;
}

/**
 * @brief Get integer argument from args
 * الحصول على وسيط صحيح من args
 */
int64_t get_int_arg(const std::vector<Value>& args, size_t index, const std::string& func_name) {
    if (index >= args.size()) {
        throw std::runtime_error(func_name + ": Missing argument at index " + std::to_string(index));
    }
    
    if (args[index].type != ValueType::NUMBER) {
        throw std::runtime_error(func_name + ": Argument at index " + std::to_string(index) + " must be a number");
    }
    
    return static_cast<int64_t>(args[index].number_val);
}

/**
 * @brief Get current database or throw error
 * الحصول على قاعدة البيانات الحالية أو رمي خطأ
 */
Database& get_current_db(const std::string& func_name) {
    auto it = g_databases.find(g_current_db_name);
    if (it == g_databases.end() || !it->second) {
        throw std::runtime_error(func_name + ": No database is currently open");
    }
    return *it->second;
}

/**
 * @brief Convert DbValue to interpreter Value
 * تحويل DbValue إلى قيمة المفسر
 */
Value db_value_to_value(const DbValue& db_val) {
    Value val;
    
    if (std::holds_alternative<std::monostate>(db_val)) {
        val.type = ValueType::NULL_TYPE;
    } else if (std::holds_alternative<int64_t>(db_val)) {
        val.type = ValueType::NUMBER;
        val.number_val = static_cast<double>(std::get<int64_t>(db_val));
    } else if (std::holds_alternative<double>(db_val)) {
        val.type = ValueType::NUMBER;
        val.number_val = std::get<double>(db_val);
    } else if (std::holds_alternative<std::string>(db_val)) {
        val.type = ValueType::STRING;
        val.string_val = std::get<std::string>(db_val);
    } else if (std::holds_alternative<std::vector<uint8_t>>(db_val)) {
        val.type = ValueType::STRING;
        val.string_val = "<BLOB>";
    }
    
    return val;
}

/**
 * @brief Convert DbRow to interpreter Value (map)
 * تحويل DbRow إلى قيمة المفسر (خريطة)
 */
Value db_row_to_value(const DbRow& row) {
    Value val;
    val.type = ValueType::MAP;
    
    for (const auto& [key, db_val] : row) {
        val.map_val[key] = db_value_to_value(db_val);
    }
    
    return val;
}

/**
 * @brief Convert DbResultSet to interpreter Value (array of maps)
 * تحويل DbResultSet إلى قيمة المفسر (مصفوفة من الخرائط)
 */
Value result_set_to_value(const DbResultSet& results) {
    Value val;
    val.type = ValueType::ARRAY;
    
    for (const auto& row : results) {
        val.array_val.push_back(db_row_to_value(row));
    }
    
    return val;
}

/**
 * @brief Convert interpreter Value to DbValue
 * تحويل قيمة المفسر إلى DbValue
 */
DbValue value_to_db_value(const Value& val) {
    switch (val.type) {
        case ValueType::NULL_TYPE:
            return std::monostate{};
        case ValueType::NUMBER:
            if (val.number_val == static_cast<int64_t>(val.number_val)) {
                return static_cast<int64_t>(val.number_val);
            }
            return val.number_val;
        case ValueType::STRING:
            return val.string_val;
        case ValueType::BOOLEAN:
            return val.bool_val ? int64_t(1) : int64_t(0);
        default:
            return std::monostate{};
    }
}

} // anonymous namespace

// ============================================================================
// Database Connection Functions - دوال اتصال قاعدة البيانات
// ============================================================================

/**
 * @brief Open database connection
 * فتح اتصال قاعدة البيانات
 * 
 * Usage: db_open("database.db")
 * Usage: افتح_قاعدة_بيانات("database.db")
 */
Value builtin_db_open(const std::vector<Value>& args) {
    std::string path = get_string_arg(args, 0, "db_open");
    
    auto db = std::make_unique<Database>(path);
    g_databases[g_current_db_name] = std::move(db);
    
    Value result;
    result.type = ValueType::BOOLEAN;
    result.bool_val = true;
    return result;
}

/**
 * @brief Close current database connection
 * إغلاق اتصال قاعدة البيانات الحالي
 * 
 * Usage: db_close()
 * Usage: أغلق_قاعدة_بيانات()
 */
Value builtin_db_close(const std::vector<Value>& args) {
    auto it = g_databases.find(g_current_db_name);
    if (it != g_databases.end()) {
        g_databases.erase(it);
    }
    
    Value result;
    result.type = ValueType::NULL_TYPE;
    return result;
}

/**
 * @brief Check if database is open
 * التحقق مما إذا كانت قاعدة البيانات مفتوحة
 * 
 * Usage: db_is_open()
 * Usage: قاعدة_بيانات_مفتوحة()
 */
Value builtin_db_is_open(const std::vector<Value>& args) {
    auto it = g_databases.find(g_current_db_name);
    bool is_open = (it != g_databases.end() && it->second && it->second->is_open());
    
    Value result;
    result.type = ValueType::BOOLEAN;
    result.bool_val = is_open;
    return result;
}

// ============================================================================
// Query Execution Functions - دوال تنفيذ الاستعلامات
// ============================================================================

/**
 * @brief Execute SQL query and return results
 * تنفيذ استعلام SQL وإرجاع النتائج
 * 
 * Usage: db_query("SELECT * FROM users")
 * Usage: استعلم("SELECT * FROM users")
 */
Value builtin_db_query(const std::vector<Value>& args) {
    Database& db = get_current_db("db_query");
    std::string sql = get_string_arg(args, 0, "db_query");
    
    try {
        DbResultSet results = db.execute(sql);
        return result_set_to_value(results);
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_query: " + std::string(e.what()));
    }
}

/**
 * @brief Execute SQL update/insert/delete
 * تنفيذ تحديث/إدراج/حذف SQL
 * 
 * Usage: db_execute("INSERT INTO users VALUES (1, 'Ahmad')")
 * Usage: نفذ_استعلام("INSERT INTO users VALUES (1, 'Ahmad')")
 */
Value builtin_db_execute(const std::vector<Value>& args) {
    Database& db = get_current_db("db_execute");
    std::string sql = get_string_arg(args, 0, "db_execute");
    
    try {
        int affected = db.execute_update(sql);
        
        Value result;
        result.type = ValueType::NUMBER;
        result.number_val = static_cast<double>(affected);
        return result;
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_execute: " + std::string(e.what()));
    }
}

/**
 * @brief Execute SQL and return single row
 * تنفيذ SQL وإرجاع صف واحد
 * 
 * Usage: db_query_single("SELECT * FROM users WHERE id=1")
 * Usage: استعلم_واحد("SELECT * FROM users WHERE id=1")
 */
Value builtin_db_query_single(const std::vector<Value>& args) {
    Database& db = get_current_db("db_query_single");
    std::string sql = get_string_arg(args, 0, "db_query_single");
    
    try {
        auto row = db.execute_single(sql);
        
        if (row) {
            return db_row_to_value(*row);
        } else {
            Value result;
            result.type = ValueType::NULL_TYPE;
            return result;
        }
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_query_single: " + std::string(e.what()));
    }
}

/**
 * @brief Execute SQL and return single value
 * تنفيذ SQL وإرجاع قيمة واحدة
 * 
 * Usage: db_scalar("SELECT COUNT(*) FROM users")
 * Usage: قيمة_واحدة("SELECT COUNT(*) FROM users")
 */
Value builtin_db_scalar(const std::vector<Value>& args) {
    Database& db = get_current_db("db_scalar");
    std::string sql = get_string_arg(args, 0, "db_scalar");
    
    try {
        auto value = db.execute_scalar(sql);
        
        if (value) {
            return db_value_to_value(*value);
        } else {
            Value result;
            result.type = ValueType::NULL_TYPE;
            return result;
        }
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_scalar: " + std::string(e.what()));
    }
}

// ============================================================================
// Transaction Functions - دوال المعاملات
// ============================================================================

/**
 * @brief Begin transaction
 * بدء معاملة
 * 
 * Usage: db_begin()
 * Usage: ابدأ_معاملة()
 */
Value builtin_db_begin(const std::vector<Value>& args) {
    Database& db = get_current_db("db_begin");
    
    try {
        db.execute_update("BEGIN TRANSACTION");
        
        Value result;
        result.type = ValueType::BOOLEAN;
        result.bool_val = true;
        return result;
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_begin: " + std::string(e.what()));
    }
}

/**
 * @brief Commit transaction
 * تأكيد المعاملة
 * 
 * Usage: db_commit()
 * Usage: أكد_معاملة()
 */
Value builtin_db_commit(const std::vector<Value>& args) {
    Database& db = get_current_db("db_commit");
    
    try {
        db.execute_update("COMMIT");
        
        Value result;
        result.type = ValueType::BOOLEAN;
        result.bool_val = true;
        return result;
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_commit: " + std::string(e.what()));
    }
}

/**
 * @brief Rollback transaction
 * التراجع عن المعاملة
 * 
 * Usage: db_rollback()
 * Usage: تراجع_عن_معاملة()
 */
Value builtin_db_rollback(const std::vector<Value>& args) {
    Database& db = get_current_db("db_rollback");
    
    try {
        db.execute_update("ROLLBACK");
        
        Value result;
        result.type = ValueType::BOOLEAN;
        result.bool_val = true;
        return result;
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_rollback: " + std::string(e.what()));
    }
}

// ============================================================================
// Schema Functions - دوال المخطط
// ============================================================================

/**
 * @brief Get list of tables
 * الحصول على قائمة الجداول
 * 
 * Usage: db_tables()
 * Usage: جداول()
 */
Value builtin_db_tables(const std::vector<Value>& args) {
    Database& db = get_current_db("db_tables");
    
    try {
        auto tables = db.get_tables();
        
        Value result;
        result.type = ValueType::ARRAY;
        
        for (const auto& table : tables) {
            Value val;
            val.type = ValueType::STRING;
            val.string_val = table;
            result.array_val.push_back(val);
        }
        
        return result;
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_tables: " + std::string(e.what()));
    }
}

/**
 * @brief Check if table exists
 * التحقق من وجود جدول
 * 
 * Usage: db_table_exists("users")
 * Usage: جدول_موجود("users")
 */
Value builtin_db_table_exists(const std::vector<Value>& args) {
    Database& db = get_current_db("db_table_exists");
    std::string table_name = get_string_arg(args, 0, "db_table_exists");
    
    try {
        bool exists = db.table_exists(table_name);
        
        Value result;
        result.type = ValueType::BOOLEAN;
        result.bool_val = exists;
        return result;
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_table_exists: " + std::string(e.what()));
    }
}

/**
 * @brief Get columns of a table
 * الحصول على أعمدة جدول
 * 
 * Usage: db_columns("users")
 * Usage: أعمدة_جدول("users")
 */
Value builtin_db_columns(const std::vector<Value>& args) {
    Database& db = get_current_db("db_columns");
    std::string table_name = get_string_arg(args, 0, "db_columns");
    
    try {
        auto columns = db.get_columns(table_name);
        
        Value result;
        result.type = ValueType::ARRAY;
        
        for (const auto& col : columns) {
            Value col_val;
            col_val.type = ValueType::MAP;
            
            Value name_val;
            name_val.type = ValueType::STRING;
            name_val.string_val = col.name;
            col_val.map_val["name"] = name_val;
            
            Value type_val;
            type_val.type = ValueType::STRING;
            type_val.string_val = column_type_to_string(col.type);
            col_val.map_val["type"] = type_val;
            
            Value not_null_val;
            not_null_val.type = ValueType::BOOLEAN;
            not_null_val.bool_val = col.not_null;
            col_val.map_val["not_null"] = not_null_val;
            
            Value pk_val;
            pk_val.type = ValueType::BOOLEAN;
            pk_val.bool_val = col.primary_key;
            col_val.map_val["primary_key"] = pk_val;
            
            result.array_val.push_back(col_val);
        }
        
        return result;
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_columns: " + std::string(e.what()));
    }
}

// ============================================================================
// Utility Functions - دوال مساعدة
// ============================================================================

/**
 * @brief Get last insert row ID
 * الحصول على معرف آخر صف مُدرج
 * 
 * Usage: db_last_insert_id()
 * Usage: آخر_معرف_إدراج()
 */
Value builtin_db_last_insert_id(const std::vector<Value>& args) {
    Database& db = get_current_db("db_last_insert_id");
    
    try {
        int64_t id = db.last_insert_rowid();
        
        Value result;
        result.type = ValueType::NUMBER;
        result.number_val = static_cast<double>(id);
        return result;
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_last_insert_id: " + std::string(e.what()));
    }
}

/**
 * @brief Get number of affected rows
 * الحصول على عدد الصفوف المتأثرة
 * 
 * Usage: db_affected_rows()
 * Usage: صفوف_متأثرة()
 */
Value builtin_db_affected_rows(const std::vector<Value>& args) {
    Database& db = get_current_db("db_affected_rows");
    
    try {
        int affected = db.changes();
        
        Value result;
        result.type = ValueType::NUMBER;
        result.number_val = static_cast<double>(affected);
        return result;
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_affected_rows: " + std::string(e.what()));
    }
}

/**
 * @brief Backup database
 * نسخ احتياطي لقاعدة البيانات
 * 
 * Usage: db_backup("backup.db")
 * Usage: نسخ_احتياطي("backup.db")
 */
Value builtin_db_backup(const std::vector<Value>& args) {
    Database& db = get_current_db("db_backup");
    std::string dest_path = get_string_arg(args, 0, "db_backup");
    
    try {
        db.backup(dest_path);
        
        Value result;
        result.type = ValueType::BOOLEAN;
        result.bool_val = true;
        return result;
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_backup: " + std::string(e.what()));
    }
}

/**
 * @brief Optimize database (VACUUM)
 * تحسين قاعدة البيانات
 * 
 * Usage: db_vacuum()
 * Usage: حسن_قاعدة_بيانات()
 */
Value builtin_db_vacuum(const std::vector<Value>& args) {
    Database& db = get_current_db("db_vacuum");
    
    try {
        db.vacuum();
        
        Value result;
        result.type = ValueType::BOOLEAN;
        result.bool_val = true;
        return result;
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_vacuum: " + std::string(e.what()));
    }
}

/**
 * @brief Get SQLite version
 * الحصول على إصدار SQLite
 * 
 * Usage: db_version()
 * Usage: إصدار_قاعدة_بيانات()
 */
Value builtin_db_version(const std::vector<Value>& args) {
    Database& db = get_current_db("db_version");
    
    try {
        std::string version = db.get_version();
        
        Value result;
        result.type = ValueType::STRING;
        result.string_val = version;
        return result;
    } catch (const DatabaseException& e) {
        throw std::runtime_error("db_version: " + std::string(e.what()));
    }
}

// ============================================================================
// Registration Function - دالة التسجيل
// ============================================================================

/**
 * @brief Register all database functions with interpreter
 * تسجيل جميع دوال قاعدة البيانات مع المفسر
 */
void register_database_functions(sad::interpreter::Interpreter& interp) {
    // Connection functions / دوال الاتصال
    interp.register_builtin("db_open", builtin_db_open);
    interp.register_builtin("افتح_قاعدة_بيانات", builtin_db_open);
    
    interp.register_builtin("db_close", builtin_db_close);
    interp.register_builtin("أغلق_قاعدة_بيانات", builtin_db_close);
    
    interp.register_builtin("db_is_open", builtin_db_is_open);
    interp.register_builtin("قاعدة_بيانات_مفتوحة", builtin_db_is_open);
    
    // Query functions / دوال الاستعلام
    interp.register_builtin("db_query", builtin_db_query);
    interp.register_builtin("استعلم", builtin_db_query);
    
    interp.register_builtin("db_execute", builtin_db_execute);
    interp.register_builtin("نفذ_استعلام", builtin_db_execute);
    
    interp.register_builtin("db_query_single", builtin_db_query_single);
    interp.register_builtin("استعلم_واحد", builtin_db_query_single);
    
    interp.register_builtin("db_scalar", builtin_db_scalar);
    interp.register_builtin("قيمة_واحدة", builtin_db_scalar);
    
    // Transaction functions / دوال المعاملات
    interp.register_builtin("db_begin", builtin_db_begin);
    interp.register_builtin("ابدأ_معاملة", builtin_db_begin);
    
    interp.register_builtin("db_commit", builtin_db_commit);
    interp.register_builtin("أكد_معاملة", builtin_db_commit);
    
    interp.register_builtin("db_rollback", builtin_db_rollback);
    interp.register_builtin("تراجع_عن_معاملة", builtin_db_rollback);
    
    // Schema functions / دوال المخطط
    interp.register_builtin("db_tables", builtin_db_tables);
    interp.register_builtin("جداول", builtin_db_tables);
    
    interp.register_builtin("db_table_exists", builtin_db_table_exists);
    interp.register_builtin("جدول_موجود", builtin_db_table_exists);
    
    interp.register_builtin("db_columns", builtin_db_columns);
    interp.register_builtin("أعمدة_جدول", builtin_db_columns);
    
    // Utility functions / دوال مساعدة
    interp.register_builtin("db_last_insert_id", builtin_db_last_insert_id);
    interp.register_builtin("آخر_معرف_إدراج", builtin_db_last_insert_id);
    
    interp.register_builtin("db_affected_rows", builtin_db_affected_rows);
    interp.register_builtin("صفوف_متأثرة", builtin_db_affected_rows);
    
    interp.register_builtin("db_backup", builtin_db_backup);
    interp.register_builtin("نسخ_احتياطي", builtin_db_backup);
    
    interp.register_builtin("db_vacuum", builtin_db_vacuum);
    interp.register_builtin("حسن_قاعدة_بيانات", builtin_db_vacuum);
    
    interp.register_builtin("db_version", builtin_db_version);
    interp.register_builtin("إصدار_قاعدة_بيانات", builtin_db_version);
}

} // namespace database
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين
