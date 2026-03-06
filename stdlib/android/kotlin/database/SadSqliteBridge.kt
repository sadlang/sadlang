// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadSqliteBridge.kt
// الوصف: جسر Kotlin لقاعدة بيانات SQLite
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.ContentValues
import android.content.Context
import android.database.Cursor
import android.database.sqlite.SQLiteDatabase
import android.database.sqlite.SQLiteOpenHelper

/**
 * جسر SQLite — SadSqliteBridge
 */
object SadSqliteBridge {
    
    private var context: Context? = null
    private val databases = mutableMapOf<String, DatabaseHelper>()
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    @JvmStatic
    fun openDatabase(name: String, version: Int = 1): Boolean {
        val ctx = context ?: return false
        
        if (databases.containsKey(name)) return true
        
        try {
            val helper = DatabaseHelper(ctx, name, version)
            databases[name] = helper
            return true
        } catch (e: Exception) {
            return false
        }
    }
    
    @JvmStatic
    fun closeDatabase(name: String) {
        databases[name]?.close()
        databases.remove(name)
    }
    
    @JvmStatic
    fun closeAll() {
        databases.values.forEach { it.close() }
        databases.clear()
    }
    
    @JvmStatic
    fun execute(dbName: String, sql: String, args: Array<Any>? = null): Boolean {
        val db = databases[dbName]?.writableDatabase ?: return false
        
        return try {
            if (args != null) {
                db.execSQL(sql, args)
            } else {
                db.execSQL(sql)
            }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun query(
        dbName: String,
        sql: String,
        args: Array<String>? = null
    ): List<Map<String, Any?>> {
        val db = databases[dbName]?.readableDatabase ?: return emptyList()
        
        val cursor = db.rawQuery(sql, args)
        return cursorToList(cursor)
    }
    
    @JvmStatic
    fun insert(
        dbName: String,
        table: String,
        values: Map<String, Any?>
    ): Long {
        val db = databases[dbName]?.writableDatabase ?: return -1
        
        val contentValues = ContentValues()
        for ((key, value) in values) {
            when (value) {
                null -> contentValues.putNull(key)
                is String -> contentValues.put(key, value)
                is Int -> contentValues.put(key, value)
                is Long -> contentValues.put(key, value)
                is Float -> contentValues.put(key, value)
                is Double -> contentValues.put(key, value)
                is Boolean -> contentValues.put(key, if (value) 1 else 0)
                is ByteArray -> contentValues.put(key, value)
                else -> contentValues.put(key, value.toString())
            }
        }
        
        return db.insert(table, null, contentValues)
    }
    
    @JvmStatic
    fun update(
        dbName: String,
        table: String,
        values: Map<String, Any?>,
        whereClause: String? = null,
        whereArgs: Array<String>? = null
    ): Int {
        val db = databases[dbName]?.writableDatabase ?: return 0
        
        val contentValues = ContentValues()
        for ((key, value) in values) {
            when (value) {
                null -> contentValues.putNull(key)
                is String -> contentValues.put(key, value)
                is Int -> contentValues.put(key, value)
                is Long -> contentValues.put(key, value)
                is Float -> contentValues.put(key, value)
                is Double -> contentValues.put(key, value)
                is Boolean -> contentValues.put(key, if (value) 1 else 0)
                is ByteArray -> contentValues.put(key, value)
                else -> contentValues.put(key, value.toString())
            }
        }
        
        return db.update(table, contentValues, whereClause, whereArgs)
    }
    
    @JvmStatic
    fun delete(
        dbName: String,
        table: String,
        whereClause: String? = null,
        whereArgs: Array<String>? = null
    ): Int {
        val db = databases[dbName]?.writableDatabase ?: return 0
        return db.delete(table, whereClause, whereArgs)
    }
    
    @JvmStatic
    fun createTable(
        dbName: String,
        tableName: String,
        columns: List<Map<String, String>>
    ): Boolean {
        val columnDefs = columns.joinToString(", ") { col ->
            val name = col["name"] ?: return false
            val type = col["type"] ?: "TEXT"
            val constraints = col["constraints"] ?: ""
            "$name $type $constraints".trim()
        }
        
        val sql = "CREATE TABLE IF NOT EXISTS $tableName ($columnDefs)"
        return execute(dbName, sql)
    }
    
    @JvmStatic
    fun dropTable(dbName: String, tableName: String): Boolean {
        return execute(dbName, "DROP TABLE IF EXISTS $tableName")
    }
    
    @JvmStatic
    fun tableExists(dbName: String, tableName: String): Boolean {
        val result = query(
            dbName,
            "SELECT name FROM sqlite_master WHERE type='table' AND name=?",
            arrayOf(tableName)
        )
        return result.isNotEmpty()
    }
    
    @JvmStatic
    fun getTableNames(dbName: String): List<String> {
        val result = query(
            dbName,
            "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%'"
        )
        return result.mapNotNull { it["name"] as? String }
    }
    
    @JvmStatic
    fun beginTransaction(dbName: String) {
        databases[dbName]?.writableDatabase?.beginTransaction()
    }
    
    @JvmStatic
    fun setTransactionSuccessful(dbName: String) {
        databases[dbName]?.writableDatabase?.setTransactionSuccessful()
    }
    
    @JvmStatic
    fun endTransaction(dbName: String) {
        databases[dbName]?.writableDatabase?.endTransaction()
    }
    
    @JvmStatic
    fun count(dbName: String, table: String, whereClause: String? = null): Long {
        val sql = if (whereClause != null) {
            "SELECT COUNT(*) FROM $table WHERE $whereClause"
        } else {
            "SELECT COUNT(*) FROM $table"
        }
        val result = query(dbName, sql)
        return result.firstOrNull()?.values?.firstOrNull() as? Long ?: 0L
    }
    
    private fun cursorToList(cursor: Cursor): List<Map<String, Any?>> {
        val list = mutableListOf<Map<String, Any?>>()
        
        cursor.use {
            while (it.moveToNext()) {
                val row = mutableMapOf<String, Any?>()
                for (i in 0 until it.columnCount) {
                    val name = it.getColumnName(i)
                    val value = when (it.getType(i)) {
                        Cursor.FIELD_TYPE_NULL -> null
                        Cursor.FIELD_TYPE_INTEGER -> it.getLong(i)
                        Cursor.FIELD_TYPE_FLOAT -> it.getDouble(i)
                        Cursor.FIELD_TYPE_STRING -> it.getString(i)
                        Cursor.FIELD_TYPE_BLOB -> it.getBlob(i)
                        else -> it.getString(i)
                    }
                    row[name] = value
                }
                list.add(row)
            }
        }
        
        return list
    }
    
    private class DatabaseHelper(
        context: Context,
        name: String,
        version: Int
    ) : SQLiteOpenHelper(context, "$name.db", null, version) {
        
        override fun onCreate(db: SQLiteDatabase) {
            // يتم إنشاء الجداول يدوياً
        }
        
        override fun onUpgrade(db: SQLiteDatabase, oldVersion: Int, newVersion: Int) {
            // يتم تحديث المخطط يدوياً
        }
    }
}
