// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadRoomBridge.kt
// الوصف: جسر Kotlin لقاعدة بيانات Room
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import androidx.room.*
import androidx.sqlite.db.SupportSQLiteDatabase
import kotlinx.coroutines.*
import java.util.concurrent.Executors

/**
 * جسر قاعدة بيانات Room — SadRoomBridge
 * 
 * ملاحظة: هذا الجسر يوفر واجهة عامة للتعامل مع Room.
 * في الاستخدام الفعلي، يجب تعريف الكيانات والـ DAOs الخاصة بتطبيقك.
 */
object SadRoomBridge {
    
    private var context: Context? = null
    private val databases = mutableMapOf<String, RoomDatabase>()
    private val executor = Executors.newFixedThreadPool(4)
    private val scope = CoroutineScope(Dispatchers.IO + SupervisorJob())
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Database Management
    // ═══════════════════════════════════════════════════════════════════════════════
    
    /**
     * إنشاء أو فتح قاعدة بيانات
     * @param name اسم قاعدة البيانات
     * @param dbClass كلاس قاعدة البيانات (يجب أن يكون موروث من RoomDatabase)
     */
    @JvmStatic
    fun <T : RoomDatabase> createDatabase(
        name: String,
        dbClass: Class<T>,
        allowMainThread: Boolean = false,
        destructiveMigration: Boolean = false
    ): T? {
        val ctx = context ?: return null
        
        return try {
            val builder = Room.databaseBuilder(ctx, dbClass, name)
            
            if (allowMainThread) {
                builder.allowMainThreadQueries()
            }
            
            if (destructiveMigration) {
                builder.fallbackToDestructiveMigration()
            }
            
            val db = builder.build()
            databases[name] = db
            db
        } catch (e: Exception) {
            null
        }
    }
    
    /**
     * إنشاء قاعدة بيانات في الذاكرة (للاختبار)
     */
    @JvmStatic
    fun <T : RoomDatabase> createInMemoryDatabase(
        dbClass: Class<T>,
        allowMainThread: Boolean = true
    ): T? {
        val ctx = context ?: return null
        
        return try {
            val builder = Room.inMemoryDatabaseBuilder(ctx, dbClass)
            
            if (allowMainThread) {
                builder.allowMainThreadQueries()
            }
            
            builder.build()
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun closeDatabase(name: String): Boolean {
        val db = databases[name]
        return if (db != null) {
            db.close()
            databases.remove(name)
            true
        } else {
            false
        }
    }
    
    @JvmStatic
    fun closeAllDatabases() {
        databases.values.forEach { it.close() }
        databases.clear()
    }
    
    @JvmStatic
    fun isDatabaseOpen(name: String): Boolean {
        return databases[name]?.isOpen == true
    }
    
    @JvmStatic
    fun getOpenDatabases(): List<String> {
        return databases.filter { it.value.isOpen }.keys.toList()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Raw Query Execution (for simple databases)
    // ═══════════════════════════════════════════════════════════════════════════════
    
    /**
     * تنفيذ استعلام SQL على قاعدة البيانات باستخدام SupportSQLiteDatabase
     */
    @JvmStatic
    fun executeRawQuery(
        db: RoomDatabase,
        query: String,
        args: Array<Any>? = null
    ): List<Map<String, Any?>> {
        val results = mutableListOf<Map<String, Any?>>()
        
        try {
            val sqliteDb = db.openHelper.readableDatabase
            val cursor = sqliteDb.query(query, args)
            
            cursor.use {
                val columnNames = it.columnNames
                while (it.moveToNext()) {
                    val row = mutableMapOf<String, Any?>()
                    for (i in columnNames.indices) {
                        val columnName = columnNames[i]
                        row[columnName] = when (it.getType(i)) {
                            android.database.Cursor.FIELD_TYPE_NULL -> null
                            android.database.Cursor.FIELD_TYPE_INTEGER -> it.getLong(i)
                            android.database.Cursor.FIELD_TYPE_FLOAT -> it.getDouble(i)
                            android.database.Cursor.FIELD_TYPE_STRING -> it.getString(i)
                            android.database.Cursor.FIELD_TYPE_BLOB -> it.getBlob(i)
                            else -> it.getString(i)
                        }
                    }
                    results.add(row)
                }
            }
        } catch (e: Exception) {
            // Return empty list on error
        }
        
        return results
    }
    
    /**
     * تنفيذ أمر SQL (INSERT, UPDATE, DELETE)
     */
    @JvmStatic
    fun executeRawStatement(
        db: RoomDatabase,
        sql: String,
        args: Array<Any?>? = null
    ): Boolean {
        return try {
            val sqliteDb = db.openHelper.writableDatabase
            if (args != null) {
                sqliteDb.execSQL(sql, args)
            } else {
                sqliteDb.execSQL(sql)
            }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Transaction Support
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun beginTransaction(db: RoomDatabase) {
        db.openHelper.writableDatabase.beginTransaction()
    }
    
    @JvmStatic
    fun setTransactionSuccessful(db: RoomDatabase) {
        db.openHelper.writableDatabase.setTransactionSuccessful()
    }
    
    @JvmStatic
    fun endTransaction(db: RoomDatabase) {
        db.openHelper.writableDatabase.endTransaction()
    }
    
    @JvmStatic
    fun inTransaction(db: RoomDatabase): Boolean {
        return db.openHelper.writableDatabase.inTransaction()
    }
    
    /**
     * تنفيذ كتلة داخل transaction
     */
    @JvmStatic
    fun <T> runInTransaction(db: RoomDatabase, block: () -> T): T? {
        return try {
            db.runInTransaction<T> { block() }
        } catch (e: Exception) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Async Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun executeAsync(
        operation: () -> Unit,
        onComplete: (() -> Unit)? = null,
        onError: ((Exception) -> Unit)? = null
    ) {
        scope.launch {
            try {
                operation()
                onComplete?.let {
                    withContext(Dispatchers.Main) { it() }
                }
            } catch (e: Exception) {
                onError?.let {
                    withContext(Dispatchers.Main) { it(e) }
                }
            }
        }
    }
    
    @JvmStatic
    fun <T> queryAsync(
        query: () -> T,
        onResult: (T) -> Unit,
        onError: ((Exception) -> Unit)? = null
    ) {
        scope.launch {
            try {
                val result = query()
                withContext(Dispatchers.Main) {
                    onResult(result)
                }
            } catch (e: Exception) {
                onError?.let {
                    withContext(Dispatchers.Main) { it(e) }
                }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Database Info
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getDatabasePath(dbName: String): String? {
        return context?.getDatabasePath(dbName)?.absolutePath
    }
    
    @JvmStatic
    fun databaseExists(dbName: String): Boolean {
        return context?.getDatabasePath(dbName)?.exists() == true
    }
    
    @JvmStatic
    fun deleteDatabase(dbName: String): Boolean {
        val ctx = context ?: return false
        
        // Close if open
        closeDatabase(dbName)
        
        return ctx.deleteDatabase(dbName)
    }
    
    @JvmStatic
    fun getDatabaseVersion(db: RoomDatabase): Int {
        return try {
            db.openHelper.readableDatabase.version
        } catch (e: Exception) {
            -1
        }
    }
    
    @JvmStatic
    fun getTables(db: RoomDatabase): List<String> {
        val tables = mutableListOf<String>()
        
        try {
            val cursor = db.openHelper.readableDatabase.query(
                "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' AND name NOT LIKE 'android_%'"
            )
            
            cursor.use {
                while (it.moveToNext()) {
                    tables.add(it.getString(0))
                }
            }
        } catch (e: Exception) {
            // Return empty list
        }
        
        return tables
    }
    
    @JvmStatic
    fun getTableInfo(db: RoomDatabase, tableName: String): List<Map<String, Any?>> {
        return executeRawQuery(db, "PRAGMA table_info($tableName)")
    }
    
    @JvmStatic
    fun getRowCount(db: RoomDatabase, tableName: String): Long {
        return try {
            val cursor = db.openHelper.readableDatabase.query(
                "SELECT COUNT(*) FROM $tableName"
            )
            cursor.use {
                if (it.moveToFirst()) it.getLong(0) else 0L
            }
        } catch (e: Exception) {
            0L
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Vacuum & Checkpoint
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun vacuum(db: RoomDatabase): Boolean {
        return executeRawStatement(db, "VACUUM")
    }
    
    @JvmStatic
    fun checkpoint(db: RoomDatabase): Boolean {
        return executeRawStatement(db, "PRAGMA wal_checkpoint(FULL)")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Migration Helper
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createMigration(
        startVersion: Int,
        endVersion: Int,
        migrate: (SupportSQLiteDatabase) -> Unit
    ): Migration {
        return object : Migration(startVersion, endVersion) {
            override fun migrate(database: SupportSQLiteDatabase) {
                migrate(database)
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Cleanup
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun cleanup() {
        closeAllDatabases()
        scope.cancel()
        executor.shutdown()
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Example Entity & DAO (for reference)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * مثال على كيان (Entity)
 */
/*
@Entity(tableName = "users")
data class User(
    @PrimaryKey(autoGenerate = true) val id: Long = 0,
    @ColumnInfo(name = "name") val name: String,
    @ColumnInfo(name = "email") val email: String?,
    @ColumnInfo(name = "created_at") val createdAt: Long = System.currentTimeMillis()
)
*/

/**
 * مثال على DAO
 */
/*
@Dao
interface UserDao {
    @Query("SELECT * FROM users")
    fun getAll(): List<User>
    
    @Query("SELECT * FROM users WHERE id = :id")
    fun getById(id: Long): User?
    
    @Insert
    fun insert(user: User): Long
    
    @Update
    fun update(user: User)
    
    @Delete
    fun delete(user: User)
    
    @Query("DELETE FROM users")
    fun deleteAll()
}
*/

/**
 * مثال على قاعدة البيانات
 */
/*
@Database(entities = [User::class], version = 1)
abstract class AppDatabase : RoomDatabase() {
    abstract fun userDao(): UserDao
}
*/
