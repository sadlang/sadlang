// بسم الله الرحمن الرحيم
/**
 * @file database_module.cpp
 * @brief Database Module Implementation - تنفيذ وحدة قاعدة البيانات
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#include "database/database_module.h"

#ifdef HAS_SQLITE3
#include <sqlite3.h>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace sad {
namespace stdlib {
namespace database {

// ============================================================================
// Helper Functions - دوال مساعدة
// ============================================================================

namespace {

/**
 * @brief Convert SQLite type to DbColumnType
 * تحويل نوع SQLite إلى DbColumnType
 */
DbColumnType sqlite_type_to_db_type(int sqlite_type) {
    switch (sqlite_type) {
        case SQLITE_INTEGER: return DbColumnType::INTEGER;
        case SQLITE_FLOAT: return DbColumnType::REAL;
        case SQLITE_TEXT: return DbColumnType::TEXT;
        case SQLITE_BLOB: return DbColumnType::BLOB;
        case SQLITE_NULL: return DbColumnType::NULL_TYPE;
        default: return DbColumnType::TEXT;
    }
}

/**
 * @brief Convert column value from SQLite statement to DbValue
 * تحويل قيمة العمود من استعلام SQLite إلى DbValue
 */
DbValue get_column_value(sqlite3_stmt* stmt, int column) {
    int type = sqlite3_column_type(stmt, column);
    
    switch (type) {
        case SQLITE_INTEGER:
            return sqlite3_column_int64(stmt, column);
            
        case SQLITE_FLOAT:
            return sqlite3_column_double(stmt, column);
            
        case SQLITE_TEXT: {
            const char* text = reinterpret_cast<const char*>(
                sqlite3_column_text(stmt, column));
            return std::string(text ? text : "");
        }
            
        case SQLITE_BLOB: {
            const void* blob = sqlite3_column_blob(stmt, column);
            int size = sqlite3_column_bytes(stmt, column);
            std::vector<uint8_t> data(static_cast<const uint8_t*>(blob),
                                     static_cast<const uint8_t*>(blob) + size);
            return data;
        }
            
        case SQLITE_NULL:
        default:
            return std::monostate{};
    }
}

/**
 * @brief Bind DbValue to prepared statement
 * ربط DbValue باستعلام محضر
 */
void bind_value(sqlite3_stmt* stmt, int index, const DbValue& value) {
    if (std::holds_alternative<std::monostate>(value)) {
        sqlite3_bind_null(stmt, index);
    } else if (std::holds_alternative<int64_t>(value)) {
        sqlite3_bind_int64(stmt, index, std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        sqlite3_bind_double(stmt, index, std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        const auto& str = std::get<std::string>(value);
        sqlite3_bind_text(stmt, index, str.c_str(), str.length(), SQLITE_TRANSIENT);
    } else if (std::holds_alternative<std::vector<uint8_t>>(value)) {
        const auto& blob = std::get<std::vector<uint8_t>>(value);
        sqlite3_bind_blob(stmt, index, blob.data(), blob.size(), SQLITE_TRANSIENT);
    }
}

} // anonymous namespace

// ============================================================================
// PreparedStatement Implementation - تنفيذ PreparedStatement
// ============================================================================

struct PreparedStatement::Impl {
    sqlite3* db;
    sqlite3_stmt* stmt;
    std::string sql;
    
    Impl(sqlite3* db_, const std::string& sql_) 
        : db(db_), stmt(nullptr), sql(sql_) {
        if (!db) {
            throw DatabaseException("Database connection is null");
        }
        
        const char* tail = nullptr;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, &tail);
        
        if (rc != SQLITE_OK) {
            std::string error = sqlite3_errmsg(db);
            throw DatabaseException("Failed to prepare statement: " + error);
        }
    }
    
    ~Impl() {
        if (stmt) {
            sqlite3_finalize(stmt);
        }
    }
};

PreparedStatement::PreparedStatement(sqlite3* db, const std::string& sql)
    : pImpl(std::make_unique<Impl>(db, sql)) {
}

PreparedStatement::~PreparedStatement() = default;

PreparedStatement::PreparedStatement(PreparedStatement&& other) noexcept = default;
PreparedStatement& PreparedStatement::operator=(PreparedStatement&& other) noexcept = default;

void PreparedStatement::bind(int index, const DbValue& value) {
    bind_value(pImpl->stmt, index, value);
}

void PreparedStatement::bind(const std::string& name, const DbValue& value) {
    int index = sqlite3_bind_parameter_index(pImpl->stmt, name.c_str());
    if (index == 0) {
        throw DatabaseException("Parameter not found: " + name);
    }
    bind(index, value);
}

DbResultSet PreparedStatement::execute() {
    DbResultSet results;
    int column_count = sqlite3_column_count(pImpl->stmt);
    
    while (sqlite3_step(pImpl->stmt) == SQLITE_ROW) {
        DbRow row;
        
        for (int i = 0; i < column_count; ++i) {
            const char* col_name = sqlite3_column_name(pImpl->stmt, i);
            row[col_name] = get_column_value(pImpl->stmt, i);
        }
        
        results.push_back(std::move(row));
    }
    
    return results;
}

int PreparedStatement::execute_update() {
    int rc = sqlite3_step(pImpl->stmt);
    
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        std::string error = sqlite3_errmsg(pImpl->db);
        throw DatabaseException("Failed to execute update: " + error);
    }
    
    return sqlite3_changes(pImpl->db);
}

void PreparedStatement::reset() {
    sqlite3_reset(pImpl->stmt);
}

void PreparedStatement::clear_bindings() {
    sqlite3_clear_bindings(pImpl->stmt);
}

// ============================================================================
// Transaction Implementation - تنفيذ Transaction
// ============================================================================

struct Transaction::Impl {
    sqlite3* db;
    bool active;
    bool committed;
    
    Impl(sqlite3* db_) : db(db_), active(true), committed(false) {
        if (!db) {
            throw DatabaseException("Database connection is null");
        }
        
        char* error = nullptr;
        int rc = sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, &error);
        
        if (rc != SQLITE_OK) {
            std::string err_msg = error ? error : "Unknown error";
            sqlite3_free(error);
            throw DatabaseException("Failed to begin transaction: " + err_msg);
        }
    }
    
    ~Impl() {
        if (active && !committed) {
            // Auto rollback if not committed
            sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        }
    }
};

Transaction::Transaction(sqlite3* db)
    : pImpl(std::make_unique<Impl>(db)) {
}

Transaction::~Transaction() = default;

void Transaction::commit() {
    if (!pImpl->active) {
        throw DatabaseException("Transaction is not active");
    }
    
    char* error = nullptr;
    int rc = sqlite3_exec(pImpl->db, "COMMIT", nullptr, nullptr, &error);
    
    if (rc != SQLITE_OK) {
        std::string err_msg = error ? error : "Unknown error";
        sqlite3_free(error);
        throw DatabaseException("Failed to commit transaction: " + err_msg);
    }
    
    pImpl->committed = true;
    pImpl->active = false;
}

void Transaction::rollback() {
    if (!pImpl->active) {
        throw DatabaseException("Transaction is not active");
    }
    
    char* error = nullptr;
    sqlite3_exec(pImpl->db, "ROLLBACK", nullptr, nullptr, &error);
    sqlite3_free(error);
    
    pImpl->active = false;
}

bool Transaction::is_active() const {
    return pImpl->active;
}

// ============================================================================
// Database Implementation - تنفيذ Database
// ============================================================================

struct Database::Impl {
    sqlite3* db;
    std::string path;
    
    Impl() : db(nullptr) {}
    
    ~Impl() {
        if (db) {
            sqlite3_close(db);
        }
    }
};

Database::Database() : pImpl(std::make_unique<Impl>()) {
}

Database::Database(const std::string& path, DbOpenMode mode)
    : pImpl(std::make_unique<Impl>()) {
    open(path, mode);
}

Database::~Database() = default;

Database::Database(Database&& other) noexcept = default;
Database& Database::operator=(Database&& other) noexcept = default;

void Database::open(const std::string& path, DbOpenMode mode) {
    if (pImpl->db) {
        throw DatabaseException("Database is already open");
    }
    
    int flags = 0;
    
    switch (mode) {
        case DbOpenMode::READ_ONLY:
            flags = SQLITE_OPEN_READONLY;
            break;
        case DbOpenMode::READ_WRITE:
            flags = SQLITE_OPEN_READWRITE;
            break;
        case DbOpenMode::CREATE:
            flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
            break;
        case DbOpenMode::MEMORY:
            pImpl->path = ":memory:";
            flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY;
            break;
    }
    
    if (mode != DbOpenMode::MEMORY) {
        pImpl->path = path;
    }
    
    int rc = sqlite3_open_v2(pImpl->path.c_str(), &pImpl->db, flags, nullptr);
    
    if (rc != SQLITE_OK) {
        std::string error = pImpl->db ? sqlite3_errmsg(pImpl->db) : "Unknown error";
        if (pImpl->db) {
            sqlite3_close(pImpl->db);
            pImpl->db = nullptr;
        }
        throw DatabaseException("Failed to open database: " + error);
    }
}

void Database::close() {
    if (pImpl->db) {
        sqlite3_close(pImpl->db);
        pImpl->db = nullptr;
        pImpl->path.clear();
    }
}

bool Database::is_open() const {
    return pImpl->db != nullptr;
}

std::string Database::get_path() const {
    return pImpl->path;
}

DbResultSet Database::execute(const std::string& sql) {
    if (!pImpl->db) {
        throw DatabaseException("Database is not open");
    }
    
    PreparedStatement stmt(pImpl->db, sql);
    return stmt.execute();
}

int Database::execute_update(const std::string& sql) {
    if (!pImpl->db) {
        throw DatabaseException("Database is not open");
    }
    
    char* error = nullptr;
    int rc = sqlite3_exec(pImpl->db, sql.c_str(), nullptr, nullptr, &error);
    
    if (rc != SQLITE_OK) {
        std::string err_msg = error ? error : "Unknown error";
        sqlite3_free(error);
        throw DatabaseException("Failed to execute update: " + err_msg);
    }
    
    return sqlite3_changes(pImpl->db);
}

std::optional<DbRow> Database::execute_single(const std::string& sql) {
    DbResultSet results = execute(sql);
    
    if (results.empty()) {
        return std::nullopt;
    }
    
    return results[0];
}

std::optional<DbValue> Database::execute_scalar(const std::string& sql) {
    auto row = execute_single(sql);
    
    if (!row || row->empty()) {
        return std::nullopt;
    }
    
    return row->begin()->second;
}

PreparedStatement Database::prepare(const std::string& sql) {
    if (!pImpl->db) {
        throw DatabaseException("Database is not open");
    }
    
    return PreparedStatement(pImpl->db, sql);
}

std::unique_ptr<Transaction> Database::begin_transaction() {
    if (!pImpl->db) {
        throw DatabaseException("Database is not open");
    }
    
    return std::make_unique<Transaction>(pImpl->db);
}

void Database::transaction(std::function<void()> func) {
    auto txn = begin_transaction();
    
    try {
        func();
        txn->commit();
    } catch (...) {
        txn->rollback();
        throw;
    }
}

std::vector<std::string> Database::get_tables() {
    std::vector<std::string> tables;
    
    DbResultSet results = execute(
        "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name");
    
    for (const auto& row : results) {
        auto it = row.find("name");
        if (it != row.end() && std::holds_alternative<std::string>(it->second)) {
            tables.push_back(std::get<std::string>(it->second));
        }
    }
    
    return tables;
}

bool Database::table_exists(const std::string& table_name) {
    auto stmt = prepare(
        "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name=?");
    stmt.bind(1, table_name);
    
    auto results = stmt.execute();
    if (results.empty() || results[0].empty()) {
        return false;
    }
    
    auto& value = results[0].begin()->second;
    return std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) > 0;
}

std::vector<ColumnInfo> Database::get_columns(const std::string& table_name) {
    std::vector<ColumnInfo> columns;
    
    std::string sql = "PRAGMA table_info(" + sql_escape(table_name) + ")";
    DbResultSet results = execute(sql);
    
    for (const auto& row : results) {
        ColumnInfo info;
        
        // name
        auto name_it = row.find("name");
        if (name_it != row.end() && std::holds_alternative<std::string>(name_it->second)) {
            info.name = std::get<std::string>(name_it->second);
        }
        
        // type
        auto type_it = row.find("type");
        if (type_it != row.end() && std::holds_alternative<std::string>(type_it->second)) {
            std::string type_str = std::get<std::string>(type_it->second);
            std::transform(type_str.begin(), type_str.end(), type_str.begin(), ::toupper);
            
            if (type_str.find("INT") != std::string::npos) {
                info.type = DbColumnType::INTEGER;
            } else if (type_str.find("REAL") != std::string::npos || 
                       type_str.find("FLOAT") != std::string::npos ||
                       type_str.find("DOUBLE") != std::string::npos) {
                info.type = DbColumnType::REAL;
            } else if (type_str.find("BLOB") != std::string::npos) {
                info.type = DbColumnType::BLOB;
            } else {
                info.type = DbColumnType::TEXT;
            }
        }
        
        // not null
        auto notnull_it = row.find("notnull");
        if (notnull_it != row.end() && std::holds_alternative<int64_t>(notnull_it->second)) {
            info.not_null = std::get<int64_t>(notnull_it->second) != 0;
        }
        
        // primary key
        auto pk_it = row.find("pk");
        if (pk_it != row.end() && std::holds_alternative<int64_t>(pk_it->second)) {
            info.primary_key = std::get<int64_t>(pk_it->second) != 0;
        }
        
        columns.push_back(info);
    }
    
    return columns;
}

std::vector<std::string> Database::get_primary_keys(const std::string& table_name) {
    std::vector<std::string> pks;
    auto columns = get_columns(table_name);
    
    for (const auto& col : columns) {
        if (col.primary_key) {
            pks.push_back(col.name);
        }
    }
    
    return pks;
}

std::vector<std::string> Database::get_indexes(const std::string& table_name) {
    std::vector<std::string> indexes;
    
    auto stmt = prepare("SELECT name FROM sqlite_master WHERE type='index' AND tbl_name=?");
    stmt.bind(1, table_name);
    
    DbResultSet results = stmt.execute();
    
    for (const auto& row : results) {
        auto it = row.find("name");
        if (it != row.end() && std::holds_alternative<std::string>(it->second)) {
            indexes.push_back(std::get<std::string>(it->second));
        }
    }
    
    return indexes;
}

int64_t Database::last_insert_rowid() {
    if (!pImpl->db) {
        throw DatabaseException("Database is not open");
    }
    
    return sqlite3_last_insert_rowid(pImpl->db);
}

int Database::changes() {
    if (!pImpl->db) {
        throw DatabaseException("Database is not open");
    }
    
    return sqlite3_changes(pImpl->db);
}

int Database::total_changes() {
    if (!pImpl->db) {
        throw DatabaseException("Database is not open");
    }
    
    return sqlite3_total_changes(pImpl->db);
}

void Database::set_foreign_keys(bool enabled) {
    std::string sql = enabled ? "PRAGMA foreign_keys = ON" : "PRAGMA foreign_keys = OFF";
    execute_update(sql);
}

void Database::backup(const std::string& dest_path) {
    if (!pImpl->db) {
        throw DatabaseException("Database is not open");
    }
    
    sqlite3* dest_db = nullptr;
    int rc = sqlite3_open(dest_path.c_str(), &dest_db);
    
    if (rc != SQLITE_OK) {
        std::string error = dest_db ? sqlite3_errmsg(dest_db) : "Unknown error";
        if (dest_db) sqlite3_close(dest_db);
        throw DatabaseException("Failed to open destination database: " + error);
    }
    
    sqlite3_backup* backup = sqlite3_backup_init(dest_db, "main", pImpl->db, "main");
    
    if (backup) {
        sqlite3_backup_step(backup, -1);
        sqlite3_backup_finish(backup);
    }
    
    rc = sqlite3_errcode(dest_db);
    sqlite3_close(dest_db);
    
    if (rc != SQLITE_OK) {
        throw DatabaseException("Backup failed");
    }
}

void Database::vacuum() {
    execute_update("VACUUM");
}

std::string Database::get_version() {
    return sqlite3_libversion();
}

sqlite3* Database::get_handle() const {
    return pImpl->db;
}

// ============================================================================
// QueryBuilder Implementation - تنفيذ QueryBuilder
// ============================================================================

struct QueryBuilder::Impl {
    enum class QueryType { SELECT, INSERT, UPDATE, DELETE };
    
    QueryType type;
    std::vector<std::string> select_columns;
    std::string from_table;
    std::vector<std::string> where_conditions;
    std::vector<std::string> order_by_clauses;
    std::vector<std::string> group_by_clauses;
    std::string having_clause;
    int limit_count;
    int offset_count;
    std::map<std::string, DbValue> insert_values;
    std::map<std::string, DbValue> update_values;
    std::vector<std::pair<std::string, std::string>> joins;
    
    Impl() : type(QueryType::SELECT), limit_count(-1), offset_count(-1) {}
};

QueryBuilder::QueryBuilder() : pImpl(std::make_unique<Impl>()) {
}

QueryBuilder::~QueryBuilder() = default;

QueryBuilder& QueryBuilder::select(const std::vector<std::string>& columns) {
    pImpl->type = Impl::QueryType::SELECT;
    pImpl->select_columns = columns;
    return *this;
}

QueryBuilder& QueryBuilder::from(const std::string& table) {
    pImpl->from_table = table;
    return *this;
}

QueryBuilder& QueryBuilder::where(const std::string& condition) {
    pImpl->where_conditions.clear();
    pImpl->where_conditions.push_back(condition);
    return *this;
}

QueryBuilder& QueryBuilder::and_where(const std::string& condition) {
    if (!pImpl->where_conditions.empty()) {
        pImpl->where_conditions.push_back("AND " + condition);
    } else {
        pImpl->where_conditions.push_back(condition);
    }
    return *this;
}

QueryBuilder& QueryBuilder::or_where(const std::string& condition) {
    if (!pImpl->where_conditions.empty()) {
        pImpl->where_conditions.push_back("OR " + condition);
    } else {
        pImpl->where_conditions.push_back(condition);
    }
    return *this;
}

QueryBuilder& QueryBuilder::order_by(const std::string& column, bool ascending) {
    std::string clause = column + (ascending ? " ASC" : " DESC");
    pImpl->order_by_clauses.push_back(clause);
    return *this;
}

QueryBuilder& QueryBuilder::limit(int count) {
    pImpl->limit_count = count;
    return *this;
}

QueryBuilder& QueryBuilder::offset(int count) {
    pImpl->offset_count = count;
    return *this;
}

QueryBuilder& QueryBuilder::join(const std::string& table, const std::string& condition) {
    pImpl->joins.push_back({"JOIN " + table, condition});
    return *this;
}

QueryBuilder& QueryBuilder::left_join(const std::string& table, const std::string& condition) {
    pImpl->joins.push_back({"LEFT JOIN " + table, condition});
    return *this;
}

QueryBuilder& QueryBuilder::group_by(const std::string& column) {
    pImpl->group_by_clauses.push_back(column);
    return *this;
}

QueryBuilder& QueryBuilder::having(const std::string& condition) {
    pImpl->having_clause = condition;
    return *this;
}

QueryBuilder& QueryBuilder::insert_into(const std::string& table) {
    pImpl->type = Impl::QueryType::INSERT;
    pImpl->from_table = table;
    return *this;
}

QueryBuilder& QueryBuilder::values(const std::map<std::string, DbValue>& values) {
    pImpl->insert_values = values;
    return *this;
}

QueryBuilder& QueryBuilder::update(const std::string& table) {
    pImpl->type = Impl::QueryType::UPDATE;
    pImpl->from_table = table;
    return *this;
}

QueryBuilder& QueryBuilder::set(const std::map<std::string, DbValue>& values) {
    pImpl->update_values = values;
    return *this;
}

QueryBuilder& QueryBuilder::delete_from(const std::string& table) {
    pImpl->type = Impl::QueryType::DELETE;
    pImpl->from_table = table;
    return *this;
}

std::string QueryBuilder::build() const {
    std::ostringstream sql;
    
    switch (pImpl->type) {
        case Impl::QueryType::SELECT: {
            sql << "SELECT ";
            
            for (size_t i = 0; i < pImpl->select_columns.size(); ++i) {
                if (i > 0) sql << ", ";
                sql << pImpl->select_columns[i];
            }
            
            sql << " FROM " << pImpl->from_table;
            
            for (const auto& [join_clause, condition] : pImpl->joins) {
                sql << " " << join_clause << " ON " << condition;
            }
            
            if (!pImpl->where_conditions.empty()) {
                sql << " WHERE ";
                for (size_t i = 0; i < pImpl->where_conditions.size(); ++i) {
                    if (i > 0) sql << " ";
                    sql << pImpl->where_conditions[i];
                }
            }
            
            if (!pImpl->group_by_clauses.empty()) {
                sql << " GROUP BY ";
                for (size_t i = 0; i < pImpl->group_by_clauses.size(); ++i) {
                    if (i > 0) sql << ", ";
                    sql << pImpl->group_by_clauses[i];
                }
            }
            
            if (!pImpl->having_clause.empty()) {
                sql << " HAVING " << pImpl->having_clause;
            }
            
            if (!pImpl->order_by_clauses.empty()) {
                sql << " ORDER BY ";
                for (size_t i = 0; i < pImpl->order_by_clauses.size(); ++i) {
                    if (i > 0) sql << ", ";
                    sql << pImpl->order_by_clauses[i];
                }
            }
            
            if (pImpl->limit_count > 0) {
                sql << " LIMIT " << pImpl->limit_count;
            }
            
            if (pImpl->offset_count > 0) {
                sql << " OFFSET " << pImpl->offset_count;
            }
            
            break;
        }
        
        case Impl::QueryType::INSERT: {
            sql << "INSERT INTO " << pImpl->from_table << " (";
            
            size_t i = 0;
            for (const auto& [key, _] : pImpl->insert_values) {
                if (i++ > 0) sql << ", ";
                sql << key;
            }
            
            sql << ") VALUES (";
            
            i = 0;
            for (const auto& [_, value] : pImpl->insert_values) {
                if (i++ > 0) sql << ", ";
                sql << "?";
            }
            
            sql << ")";
            break;
        }
        
        case Impl::QueryType::UPDATE: {
            sql << "UPDATE " << pImpl->from_table << " SET ";
            
            size_t i = 0;
            for (const auto& [key, _] : pImpl->update_values) {
                if (i++ > 0) sql << ", ";
                sql << key << " = ?";
            }
            
            if (!pImpl->where_conditions.empty()) {
                sql << " WHERE ";
                for (size_t i = 0; i < pImpl->where_conditions.size(); ++i) {
                    if (i > 0) sql << " ";
                    sql << pImpl->where_conditions[i];
                }
            }
            
            break;
        }
        
        case Impl::QueryType::DELETE: {
            sql << "DELETE FROM " << pImpl->from_table;
            
            if (!pImpl->where_conditions.empty()) {
                sql << " WHERE ";
                for (size_t i = 0; i < pImpl->where_conditions.size(); ++i) {
                    if (i > 0) sql << " ";
                    sql << pImpl->where_conditions[i];
                }
            }
            
            break;
        }
    }
    
    return sql.str();
}

void QueryBuilder::reset() {
    pImpl = std::make_unique<Impl>();
}

// ============================================================================
// Model Implementation - تنفيذ Model
// ============================================================================

bool Model::save(Database& db) {
    DbRow row = to_row();
    std::string pk = get_primary_key();
    
    // Check if model exists
    if (row.find(pk) != row.end() && !is_null(row[pk])) {
        // Update
        QueryBuilder qb;
        qb.update(get_table_name())
          .set(row)
          .where(pk + " = ?");
        
        auto stmt = db.prepare(qb.build());
        stmt.bind(1, row[pk]);
        stmt.execute_update();
        
        return true;
    } else {
        // Insert
        QueryBuilder qb;
        qb.insert_into(get_table_name()).values(row);
        
        auto stmt = db.prepare(qb.build());
        
        int i = 1;
        for (const auto& [_, value] : row) {
            stmt.bind(i++, value);
        }
        
        stmt.execute_update();
        
        return true;
    }
}

bool Model::remove(Database& db) {
    DbRow row = to_row();
    std::string pk = get_primary_key();
    
    if (row.find(pk) == row.end() || is_null(row[pk])) {
        return false;
    }
    
    QueryBuilder qb;
    qb.delete_from(get_table_name()).where(pk + " = ?");
    
    auto stmt = db.prepare(qb.build());
    stmt.bind(1, row[pk]);
    stmt.execute_update();
    
    return true;
}

bool Model::exists(Database& db) const {
    DbRow row = to_row();
    std::string pk = get_primary_key();
    
    if (row.find(pk) == row.end() || is_null(row.at(pk))) {
        return false;
    }
    
    QueryBuilder qb;
    qb.select({"COUNT(*)"}).from(get_table_name()).where(pk + " = ?");
    
    auto stmt = db.prepare(qb.build());
    stmt.bind(1, row.at(pk));
    
    auto results = stmt.execute();
    if (results.empty() || results[0].empty()) {
        return false;
    }
    
    auto& value = results[0].begin()->second;
    return std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) > 0;
}

// ============================================================================
// Utility Functions - الدوال المساعدة
// ============================================================================

std::string db_value_to_string(const DbValue& value) {
    if (std::holds_alternative<std::monostate>(value)) {
        return "NULL";
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<uint8_t>>(value)) {
        return "<BLOB>";
    }
    return "";
}

std::string db_value_type_name(const DbValue& value) {
    if (std::holds_alternative<std::monostate>(value)) {
        return "NULL";
    } else if (std::holds_alternative<int64_t>(value)) {
        return "INTEGER";
    } else if (std::holds_alternative<double>(value)) {
        return "REAL";
    } else if (std::holds_alternative<std::string>(value)) {
        return "TEXT";
    } else if (std::holds_alternative<std::vector<uint8_t>>(value)) {
        return "BLOB";
    }
    return "UNKNOWN";
}

bool is_null(const DbValue& value) {
    return std::holds_alternative<std::monostate>(value);
}

std::string column_type_to_string(DbColumnType type) {
    switch (type) {
        case DbColumnType::INTEGER: return "INTEGER";
        case DbColumnType::REAL: return "REAL";
        case DbColumnType::TEXT: return "TEXT";
        case DbColumnType::BLOB: return "BLOB";
        case DbColumnType::NULL_TYPE: return "NULL";
        default: return "UNKNOWN";
    }
}

std::string sql_escape(const std::string& str) {
    std::string escaped;
    escaped.reserve(str.length());
    
    for (char c : str) {
        if (c == '\'') {
            escaped += "''";
        } else {
            escaped += c;
        }
    }
    
    return escaped;
}

std::string format_sql(const std::string& sql) {
    std::string formatted;
    bool in_string = false;
    int indent = 0;
    
    for (size_t i = 0; i < sql.length(); ++i) {
        char c = sql[i];
        
        if (c == '\'' && (i == 0 || sql[i-1] != '\\')) {
            in_string = !in_string;
        }
        
        if (!in_string) {
            if (c == '(') {
                indent++;
                formatted += c;
                formatted += '\n';
                formatted += std::string(indent * 2, ' ');
                continue;
            } else if (c == ')') {
                indent--;
                formatted += '\n';
                formatted += std::string(indent * 2, ' ');
                formatted += c;
                continue;
            }
        }
        
        formatted += c;
    }
    
    return formatted;
}

} // namespace database
} // namespace stdlib
} // namespace sad

#endif // HAS_SQLITE3

// الحمد لله رب العالمين
