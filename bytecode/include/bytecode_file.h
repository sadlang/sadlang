/**
 * @file bytecode_file.h
 * @brief تنسيق ملف البايت كود / Bytecode File Format
 * @brief .sbc file format specification and loader
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 * 
 * @details
 * (AR) تنسيق ملف .sbc:
 *      - الترويسة: رقم سحري، إصدار، معلومات
 *      - قسم الثوابت: أعداد، نصوص، دوال
 *      - قسم الكود: التعليمات، معلومات التصحيح
 *      - جدول الرموز: أسماء المتغيرات والدوال
 *      - المجموع الاختباري: للتحقق من السلامة
 * 
 * (EN) .sbc file format:
 *      - Header: magic number, version, metadata
 *      - Constant pool: numbers, strings, functions
 *      - Code section: instructions, debug info
 *      - Symbol table: variable and function names
 *      - Checksum: integrity verification
 */

#pragma once

#include "vm.h"
#include <string>
#include <vector>
#include <fstream>
#include <cstdint>

namespace Sad {
namespace Bytecode {

// ========================================
// File Format Constants
// ========================================

// (AR) الرقم السحري: "SBC\0" / (EN) Magic number: "SBC\0"
constexpr uint32_t MAGIC_NUMBER = 0x00434253; // "SBC" in little-endian

// (AR) إصدار التنسيق / (EN) Format version
constexpr uint16_t FORMAT_VERSION_MAJOR = 1;
constexpr uint16_t FORMAT_VERSION_MINOR = 0;

// (AR) أقسام الملف / (EN) File sections
enum class SectionType : uint8_t {
    HEADER = 0x01,      // (AR) الترويسة / (EN) Header
    CONSTANTS = 0x02,   // (AR) الثوابت / (EN) Constants
    CODE = 0x03,        // (AR) الكود / (EN) Code
    DEBUG = 0x04,       // (AR) معلومات التصحيح / (EN) Debug info
    SYMBOLS = 0x05,     // (AR) جدول الرموز / (EN) Symbol table
    CHECKSUM = 0xFF     // (AR) المجموع الاختباري / (EN) Checksum
};

// (AR) أنواع الثوابت / (EN) Constant types
enum class ConstantType : uint8_t {
    NULL_VALUE = 0x00,
    BOOL_VALUE = 0x01,
    INT_VALUE = 0x02,
    FLOAT_VALUE = 0x03,
    STRING_VALUE = 0x04,
    FUNCTION = 0x05
};

// ========================================
// File Header - ترويسة الملف
// ========================================

/**
 * @brief ترويسة ملف البايت كود / Bytecode file header
 * 
 * الحجم الكلي: 32 بايت / Total size: 32 bytes
 */
struct FileHeader {
    uint32_t magic;           // (AR) الرقم السحري / (EN) Magic number (4 bytes)
    uint16_t versionMajor;    // (AR) الإصدار الرئيسي / (EN) Major version (2 bytes)
    uint16_t versionMinor;    // (AR) الإصدار الفرعي / (EN) Minor version (2 bytes)
    uint32_t flags;           // (AR) علامات / (EN) Flags (4 bytes)
    uint64_t timestamp;       // (AR) وقت الترجمة / (EN) Compilation timestamp (8 bytes)
    uint32_t codeSize;        // (AR) حجم قسم الكود / (EN) Code section size (4 bytes)
    uint32_t constantsCount;  // (AR) عدد الثوابت / (EN) Constants count (4 bytes)
    uint32_t symbolsCount;    // (AR) عدد الرموز / (EN) Symbols count (4 bytes)
    
    /**
     * @brief التحقق من صحة الترويسة / Validate header
     * @return true إذا كانت صحيحة / true if valid
     */
    bool isValid() const;
    
    /**
     * @brief طباعة معلومات الترويسة / Print header information
     */
    void print() const;
};

// علامات الترويسة / Header flags
constexpr uint32_t FLAG_DEBUG_INFO = 0x0001;      // (AR) يحتوي على معلومات تصحيح
constexpr uint32_t FLAG_OPTIMIZED = 0x0002;       // (AR) مُحسَّن
constexpr uint32_t FLAG_COMPRESSED = 0x0004;      // (AR) مضغوط
constexpr uint32_t FLAG_64BIT = 0x0008;           // (AR) 64 بت

// ========================================
// Constant Pool Entry
// ========================================

/**
 * @brief عنصر في مجمع الثوابت / Constant pool entry
 */
struct ConstantEntry {
    ConstantType type;        // (AR) نوع الثابت / (EN) Constant type
    
    union {
        bool boolValue;
        int64_t intValue;
        double floatValue;
        struct {
            uint32_t offset;  // (AR) موقع في قسم النصوص / (EN) Offset in string section
            uint32_t length;  // (AR) طول النص / (EN) String length
        } stringValue;
        struct {
            uint32_t codeOffset;    // (AR) موقع الكود / (EN) Code offset
            uint32_t codeSize;      // (AR) حجم الكود / (EN) Code size
            uint16_t arity;         // (AR) عدد المعاملات / (EN) Parameter count
            uint16_t localCount;    // (AR) عدد المتغيرات المحلية / (EN) Local variable count
        } functionValue;
    };
};

// ========================================
// Symbol Table Entry
// ========================================

/**
 * @brief عنصر في جدول الرموز / Symbol table entry
 */
struct SymbolEntry {
    uint32_t nameOffset;      // (AR) موقع الاسم / (EN) Name offset
    uint32_t nameLength;      // (AR) طول الاسم / (EN) Name length
    uint8_t type;             // (AR) نوع الرمز / (EN) Symbol type
    uint8_t scope;            // (AR) نطاق الرمز / (EN) Symbol scope
    uint16_t index;           // (AR) الفهرس / (EN) Index
};

// أنواع الرموز / Symbol types
constexpr uint8_t SYMBOL_VARIABLE = 0x01;
constexpr uint8_t SYMBOL_FUNCTION = 0x02;
constexpr uint8_t SYMBOL_CONSTANT = 0x03;

// نطاقات الرموز / Symbol scopes
constexpr uint8_t SCOPE_GLOBAL = 0x01;
constexpr uint8_t SCOPE_LOCAL = 0x02;

// ========================================
// Debug Information
// ========================================

/**
 * @brief معلومات التصحيح / Debug information
 */
struct DebugInfo {
    std::vector<std::pair<uint32_t, uint32_t>> lineNumbers; // (AR) موقع التعليمة -> رقم السطر
    std::vector<std::string> sourceFiles;                    // (AR) ملفات المصدر
};

// ========================================
// Bytecode File Writer
// ========================================

/**
 * @brief كاتب ملفات البايت كود / Bytecode file writer
 */
class BytecodeWriter {
public:
    /**
     * @brief المُنشئ / Constructor
     * @param filename اسم الملف / File name
     */
    explicit BytecodeWriter(const std::string& filename);
    
    /**
     * @brief المُدمِّر / Destructor
     */
    ~BytecodeWriter();
    
    /**
     * @brief كتابة الترويسة / Write header
     * @param header الترويسة / Header
     * @return true إذا نجحت / true if successful
     */
    bool writeHeader(const FileHeader& header);
    
    /**
     * @brief كتابة الثوابت / Write constants
     * @param constants قائمة الثوابت / Constants list
     * @return true إذا نجحت / true if successful
     */
    bool writeConstants(const std::vector<VM::Value>& constants);
    
    /**
     * @brief كتابة الكود / Write code
     * @param code البايت كود / Bytecode
     * @param size حجم الكود / Code size
     * @return true إذا نجحت / true if successful
     */
    bool writeCode(const uint8_t* code, size_t size);
    
    /**
     * @brief كتابة معلومات التصحيح / Write debug info
     * @param debug معلومات التصحيح / Debug information
     * @return true إذا نجحت / true if successful
     */
    bool writeDebugInfo(const DebugInfo& debug);
    
    /**
     * @brief كتابة جدول الرموز / Write symbol table
     * @param symbols قائمة الرموز / Symbol list
     * @return true إذا نجحت / true if successful
     */
    bool writeSymbols(const std::vector<SymbolEntry>& symbols);
    
    /**
     * @brief كتابة المجموع الاختباري / Write checksum
     * @return true إذا نجحت / true if successful
     */
    bool writeChecksum();
    
    /**
     * @brief إنهاء الكتابة وإغلاق الملف / Finalize and close file
     * @return true إذا نجحت / true if successful
     */
    bool finalize();
    
private:
    std::ofstream file_;
    uint32_t checksum_;
    bool finalized_;
    
    /**
     * @brief تحديث المجموع الاختباري / Update checksum
     * @param data البيانات / Data
     * @param size الحجم / Size
     */
    void updateChecksum(const void* data, size_t size);
    
    /**
     * @brief كتابة بايت / Write byte
     */
    void writeByte(uint8_t value);
    
    /**
     * @brief كتابة عدد 16 بت / Write 16-bit integer
     */
    void writeUInt16(uint16_t value);
    
    /**
     * @brief كتابة عدد 32 بت / Write 32-bit integer
     */
    void writeUInt32(uint32_t value);
    
    /**
     * @brief كتابة عدد 64 بت / Write 64-bit integer
     */
    void writeUInt64(uint64_t value);
    
    /**
     * @brief كتابة عدد حقيقي / Write float
     */
    void writeDouble(double value);
    
    /**
     * @brief كتابة نص / Write string
     */
    void writeString(const std::string& str);
};

// ========================================
// Bytecode File Loader
// ========================================

/**
 * @brief قارئ ملفات البايت كود / Bytecode file loader
 */
class BytecodeLoader {
public:
    /**
     * @brief المُنشئ / Constructor
     * @param filename اسم الملف / File name
     */
    explicit BytecodeLoader(const std::string& filename);
    
    /**
     * @brief المُدمِّر / Destructor
     */
    ~BytecodeLoader();
    
    /**
     * @brief تحميل الملف / Load file
     * @return true إذا نجحت / true if successful
     */
    bool load();
    
    /**
     * @brief التحقق من صحة الملف / Verify file
     * @return true إذا كان صحيحًا / true if valid
     */
    bool verify();
    
    /**
     * @brief الحصول على الترويسة / Get header
     * @return الترويسة / Header
     */
    const FileHeader& getHeader() const { return header_; }
    
    /**
     * @brief الحصول على الثوابت / Get constants
     * @return قائمة الثوابت / Constants list
     */
    const std::vector<VM::Value>& getConstants() const { return constants_; }
    
    /**
     * @brief الحصول على الكود / Get code
     * @return البايت كود / Bytecode
     */
    const std::vector<uint8_t>& getCode() const { return code_; }
    
    /**
     * @brief الحصول على معلومات التصحيح / Get debug info
     * @return معلومات التصحيح / Debug information
     */
    const DebugInfo& getDebugInfo() const { return debug_; }
    
    /**
     * @brief الحصول على جدول الرموز / Get symbol table
     * @return قائمة الرموز / Symbol list
     */
    const std::vector<SymbolEntry>& getSymbols() const { return symbols_; }
    
    /**
     * @brief الحصول على آخر خطأ / Get last error
     * @return رسالة الخطأ / Error message
     */
    const std::string& getError() const { return error_; }
    
private:
    std::ifstream file_;
    FileHeader header_;
    std::vector<VM::Value> constants_;
    std::vector<uint8_t> code_;
    DebugInfo debug_;
    std::vector<SymbolEntry> symbols_;
    std::string error_;
    uint32_t checksum_;
    uint32_t calculatedChecksum_;
    
    /**
     * @brief قراءة الترويسة / Read header
     * @return true إذا نجحت / true if successful
     */
    bool readHeader();
    
    /**
     * @brief قراءة الثوابت / Read constants
     * @return true إذا نجحت / true if successful
     */
    bool readConstants();
    
    /**
     * @brief قراءة الكود / Read code
     * @return true إذا نجحت / true if successful
     */
    bool readCode();
    
    /**
     * @brief قراءة معلومات التصحيح / Read debug info
     * @return true إذا نجحت / true if successful
     */
    bool readDebugInfo();
    
    /**
     * @brief قراءة جدول الرموز / Read symbol table
     * @return true إذا نجحت / true if successful
     */
    bool readSymbols();
    
    /**
     * @brief قراءة المجموع الاختباري / Read checksum
     * @return true إذا نجحت / true if successful
     */
    bool readChecksum();
    
    /**
     * @brief تحديث المجموع الاختباري / Update checksum
     * @param data البيانات / Data
     * @param size الحجم / Size
     */
    void updateChecksum(const void* data, size_t size);
    
    /**
     * @brief قراءة بايت / Read byte
     */
    uint8_t readByte();
    
    /**
     * @brief قراءة عدد 16 بت / Read 16-bit integer
     */
    uint16_t readUInt16();
    
    /**
     * @brief قراءة عدد 32 بت / Read 32-bit integer
     */
    uint32_t readUInt32();
    
    /**
     * @brief قراءة عدد 64 بت / Read 64-bit integer
     */
    uint64_t readUInt64();
    
    /**
     * @brief قراءة عدد حقيقي / Read float
     */
    double readDouble();
    
    /**
     * @brief قراءة نص / Read string
     */
    std::string readString(uint32_t length);
    
    /**
     * @brief تسجيل خطأ / Log error
     * @param msg رسالة الخطأ / Error message
     */
    void setError(const std::string& msg);
};

// ========================================
// Utility Functions
// ========================================

/**
 * @brief حساب المجموع الاختباري CRC32 / Calculate CRC32 checksum
 * @param data البيانات / Data
 * @param size الحجم / Size
 * @return المجموع الاختباري / Checksum
 */
uint32_t calculateCRC32(const void* data, size_t size);

/**
 * @brief تحويل قيمة VM إلى عنصر ثابت / Convert VM value to constant entry
 * @param value القيمة / Value
 * @return عنصر الثابت / Constant entry
 */
ConstantEntry valueToConstant(const VM::Value& value);

/**
 * @brief تحويل عنصر ثابت إلى قيمة VM / Convert constant entry to VM value
 * @param entry عنصر الثابت / Constant entry
 * @param loader القارئ / Loader (for string data)
 * @return القيمة / Value
 */
VM::Value constantToValue(const ConstantEntry& entry, BytecodeLoader* loader);

} // namespace Bytecode
} // namespace Sad
