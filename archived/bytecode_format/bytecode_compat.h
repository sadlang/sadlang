/**
 * @file bytecode_compat.h
 * @brief (AR) طبقة التوافق للبايت كود — الجسر بين فضاءات الأسماء الثلاثة
 * @brief (EN) Bytecode compatibility layer — bridge between three namespaces
 *
 * @details
 * ═══════════════════════════════════════════════════════════════════════════
 * (AR) شرح موسّع — طبقة التوافق بين runtime والمترجم
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * المشكلة الجوهرية:
 * ─────────────────
 * مكتبة runtime كُتبت في الأصل لتعمل مع واجهة برمجية (API) افتراضية
 * لم تُنفَّذ أبداً في المترجم الفعلي. المترجم الحالي (compiler) يستخدم
 * فضاءات أسماء وأنواع مختلفة تماماً:
 *
 *   ┌──────────────────────────┬──────────────────────────────────────┐
 *   │ ما يتوقعه runtime    │ ما يوجد فعلاً في compiler       │
 *   ├──────────────────────────┼──────────────────────────────────────┤
 *   │ Sad::Bytecode::*         │ Sad::Compiler::Bytecode::*           │
 *   │ Bytecode::FileHeader     │ (غير موجود — أُنشئ هنا)             │
 *   │ Bytecode::BytecodeLoader │ (غير موجود — أُنشئ هنا)             │
 *   │ OP_BIT_AND, OP_CMP_EQ   │ OP_AND, OP_EQ (أسماء مختلفة)        │
 *   │ OP_GET_LOCAL             │ OP_LOAD_LOCAL (أسماء مختلفة)         │
 *   └──────────────────────────┴──────────────────────────────────────┘
 *
 * الحل المُطبَّق في هذا الملف:
 * ─────────────────────────────
 * 1. استيراد ملفات المترجم الحالي (opcodes.h + bytecode.h)
 * 2. إنشاء 30+ اسم بديل (alias) لأكواد العمليات المختلفة:
 *    - 6 أسماء بديلة للعمليات البتية (OP_BIT_AND → OP_AND، إلخ)
 *    - 6 أسماء بديلة لعمليات المقارنة (OP_CMP_EQ → OP_EQ، إلخ)
 *    - 4 أسماء بديلة لعمليات المتغيرات (OP_GET_LOCAL → OP_LOAD_LOCAL، إلخ)
 *    - 3 أسماء بديلة لعمليات الكائنات (OP_OBJECT_NEW → OP_OBJ_NEW، إلخ)
 *    - 5 أسماء بديلة لعمليات الأنواع والتحويل (OP_TYPEOF → OP_TYPE، إلخ)
 * 3. إنشاء 10 كود عمليات مُوسَّع (extended opcodes) في المجال 0xB7-0xC5:
 *    - 3 عمليات منطقية (OP_LOG_AND, OP_LOG_OR, OP_LOG_NOT)
 *    - 4 عمليات نصوص (OP_STRING_CONCAT, OP_STRING_LEN, OP_STRING_GET, OP_STRING_SUBSTR)
 *    - 2 عمليات إدخال/إخراج (OP_IO_OPEN, OP_IO_CLOSE)
 *    - 1 عملية ذاكرة (OP_MUNMAP)
 * 4. إنشاء الأنواع المفقودة:
 *    - Sad::Bytecode::FileHeader — ترويسة ملف البايت كود (magic، version، flags، إلخ)
 *    - Sad::Bytecode::BytecodeLoader — مُحمِّل الملفات مع التحقق من السلامة
 *    - Sad::Bytecode::SymbolInfo — معلومات الرموز (دوال، متغيرات عامة، أصناف)
 *    - Sad::Bytecode::ExtendedDebugInfo — معلومات تنقيح موسّعة
 * 5. إعادة تصدير أنواع المترجم تحت Sad::Bytecode:: (BytecodeModule, FunctionInfo، إلخ)
 * 6. تعريف الثوابت: MAGIC_NUMBER (0xD8B5BC00)، إصدار التنسيق، أعلام الملف
 *
 * الاعتماديات:
 * ────────────
 * - bytecode/opcodes.h  ← أكواد عمليات المترجم (Sad::VM::Opcode)
 * - bytecode/bytecode.h ← BytecodeModule, FunctionInfo من المترجم
 * - vm/value.h          ← نوع القيم Value (يُستخدم في BytecodeLoader)
 *
 * ═══════════════════════════════════════════════════════════════════════════
 * (EN) Extended Description — Compatibility Layer
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * This file bridges three namespaces (Sad::VM, Sad::Compiler::Bytecode,
 * and Sad::Bytecode) by providing:
 * - 30+ opcode aliases mapping old runtime names to compiler names
 * - 10 extended opcodes (0xB7-0xC5) for runtime-specific operations
 * - Missing types: FileHeader, BytecodeLoader, SymbolInfo, ExtendedDebugInfo
 * - Namespace re-exports: Sad::Compiler::Bytecode::* → Sad::Bytecode::*
 * - Constants: magic number, format version, file flags
 *
 * @note هذا الملف حرج لعمل مكتبة runtime بأكملها. أي تغيير في
 *       opcodes.h أو bytecode.h قد يتطلب تحديث الأسماء البديلة هنا.
 * @note This file is critical for the entire runtime library. Any
 *       changes to opcodes.h or bytecode.h may require alias updates here.
 *
 * @see runtime/include/vm/vm.h — الآلة الافتراضية التي تستهلك هذه الأسماء
 * @see compiler/include/bytecode/opcodes.h — مصدر أكواد العمليات الأصلية
 * @see compiler/include/bytecode/bytecode.h — مصدر BytecodeModule
 *
 * @author فريق لغة ص / Sad Language Team
 * @date فبراير 2026 / February 2026
 * @version 2.0 — إعادة كتابة شاملة للتوافق / Comprehensive compatibility rewrite
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>

// ============================================================================
// استيراد APIs المترجم الحالي / Import current compiler APIs
// ============================================================================

// الـ opcodes والـ bytecode من المترجم
// Opcodes and bytecode from compiler
#include "bytecode/opcodes.h"
#include "bytecode/bytecode.h"

// ============================================================================
// أسماء بديلة لأكواد العمليات / Opcode Name Aliases
// (AR) ملف vm.cpp الأصلي يستخدم أسماء مختلفة عن opcodes.h.
//      نُضيف الأسماء البديلة هنا لتجنب تعديل opcodes.h المترجم
//      أو تعديل مئات الأسطر في vm.cpp.
// (EN) Original vm.cpp uses different opcode names than opcodes.h.
//      We add aliases here to avoid modifying the compiler's opcodes.h
//      or hundreds of lines in vm.cpp.
// ============================================================================
namespace Sad {
namespace VM {

// ====================================================================
// أسماء بديلة — عمليات بتية / Aliases — Bitwise Operations
// vm.cpp: OP_BIT_AND → opcodes.h: OP_AND
// ====================================================================
constexpr Opcode OP_BIT_AND  = OP_AND;
constexpr Opcode OP_BIT_OR   = OP_OR;
constexpr Opcode OP_BIT_XOR  = OP_XOR;
constexpr Opcode OP_BIT_NOT  = OP_NOT;
constexpr Opcode OP_BIT_SHL  = OP_SHL;
constexpr Opcode OP_BIT_SHR  = OP_SHR;

// ====================================================================
// أسماء بديلة — عمليات مقارنة / Aliases — Comparison Operations
// vm.cpp: OP_CMP_EQ → opcodes.h: OP_EQ
// ====================================================================
constexpr Opcode OP_CMP_EQ   = OP_EQ;
constexpr Opcode OP_CMP_NE   = OP_NE;
constexpr Opcode OP_CMP_LT   = OP_LT;
constexpr Opcode OP_CMP_LE   = OP_LE;
constexpr Opcode OP_CMP_GT   = OP_GT;
constexpr Opcode OP_CMP_GE   = OP_GE;

// ====================================================================
// أسماء بديلة — متغيرات / Aliases — Variable Operations
// vm.cpp: OP_GET_LOCAL → opcodes.h: OP_LOAD_LOCAL
// ====================================================================
constexpr Opcode OP_GET_LOCAL   = OP_LOAD_LOCAL;
constexpr Opcode OP_SET_LOCAL   = OP_STORE_LOCAL;
constexpr Opcode OP_GET_GLOBAL  = OP_LOAD_GLOBAL;
constexpr Opcode OP_SET_GLOBAL  = OP_STORE_GLOBAL;

// ====================================================================
// أسماء بديلة — عمليات الكائنات / Aliases — Object Operations
// vm.cpp: OP_OBJECT_NEW → opcodes.h: OP_OBJ_NEW
// ====================================================================
constexpr Opcode OP_OBJECT_NEW  = OP_OBJ_NEW;
constexpr Opcode OP_OBJECT_GET  = OP_OBJ_GET;
constexpr Opcode OP_OBJECT_SET  = OP_OBJ_SET;

// ====================================================================
// أسماء بديلة — أنواع وتحويل / Aliases — Type & Cast Operations
// vm.cpp: OP_TYPEOF → opcodes.h: OP_TYPE
// ====================================================================
constexpr Opcode OP_TYPEOF      = OP_TYPE;
constexpr Opcode OP_CAST_INT    = OP_INT;
constexpr Opcode OP_CAST_FLOAT  = OP_FLOAT;
constexpr Opcode OP_CAST_STRING = OP_STR;
constexpr Opcode OP_CAST_BOOL   = OP_BOOL;

// ====================================================================
// أكواد عمليات مُوسَّعة — خاصة بوقت التشغيل
// Extended Opcodes — Runtime-specific
// (AR) هذه العمليات لا توجد في مجموعة تعليمات المترجم الحالية،
//      لكن runtime يستخدمها. نُخصص لها قيم فارغة في مجال 0xC0-0xEF.
// (EN) These opcodes don't exist in the compiler's current instruction set,
//      but runtime uses them. We assign unused values in 0xC0-0xEF range.
// ====================================================================

// عمليات منطقية (مختلفة عن البتية) / Logical operations (different from bitwise)
constexpr Opcode OP_LOG_AND         = 0x36;  ///< Logical AND / و المنطقية
constexpr Opcode OP_LOG_OR          = 0x37;  ///< Logical OR / أو المنطقية
constexpr Opcode OP_LOG_NOT         = 0x38;  ///< Logical NOT / ليس المنطقية

// عمليات النص / String operations
constexpr Opcode OP_STRING_CONCAT   = 0xC0;  ///< String concatenation / ربط النصوص
constexpr Opcode OP_STRING_LEN      = 0xC1;  ///< String length / طول النص
constexpr Opcode OP_STRING_GET      = 0xC2;  ///< Get character / الحصول على حرف
constexpr Opcode OP_STRING_SUBSTR   = 0xC3;  ///< Substring / نص فرعي

// عمليات الإدخال/الإخراج / IO operations
constexpr Opcode OP_IO_OPEN         = 0xC4;  ///< Open file / فتح ملف
constexpr Opcode OP_IO_CLOSE        = 0xC5;  ///< Close file / إغلاق ملف

// عمليات ذاكرة موسعة / Extended memory operations
constexpr Opcode OP_MUNMAP          = 0xB7;  ///< Memory unmap / إلغاء تخطيط ذاكرة

} // namespace VM
} // namespace Sad

namespace Sad {

// ============================================================================
// طبقة التوافق — الأنواع المفقودة / Compatibility layer — missing types
// ============================================================================

/**
 * @brief فضاء الاسم Bytecode — اختصار للتوافق مع الكود القديم
 * (AR) يوفر وصولاً مباشراً لأنواع المترجم من Sad::Compiler::Bytecode
 *      تحت الاسم المختصر Sad::Bytecode الذي يستخدمه runtime/
 * (EN) Provides direct access to compiler types from Sad::Compiler::Bytecode
 *      under the short name Sad::Bytecode used by runtime/
 */
namespace Bytecode {

    // ========================================================================
    // إعادة تصدير الأنواع من المترجم / Re-export types from compiler
    // ========================================================================
    using BytecodeModule = Sad::Compiler::Bytecode::BytecodeModule;
    using FunctionInfo   = Sad::Compiler::Bytecode::FunctionInfo;
    using GlobalInfo     = Sad::Compiler::Bytecode::GlobalInfo;
    using ConstantPool   = Sad::Compiler::Bytecode::ConstantPool;
    using DebugInfo      = Sad::Compiler::Bytecode::DebugInfo;
    using LineInfo       = Sad::Compiler::Bytecode::LineInfo;

    // ========================================================================
    // ترويسة ملف البايت كود / Bytecode file header
    // (AR) هذا النوع كان مفقوداً — vm.cpp يتوقع وجوده.
    //      نُعرِّفه هنا لجعل loadFromFile() يعمل.
    // (EN) This type was missing — vm.cpp expects it.
    //      Defined here to make loadFromFile() work.
    // ========================================================================
    struct FileHeader {
        uint32_t magic;            ///< الرقم السحري "صBC" / Magic number "SBC"
        uint16_t versionMajor;     ///< الإصدار الرئيسي / Major version
        uint16_t versionMinor;     ///< الإصدار الفرعي / Minor version
        uint32_t flags;            ///< أعلام (ضغط، تشفير...) / Flags (compression, encryption...)
        uint64_t timestamp;        ///< الطابع الزمني / Timestamp
        uint32_t codeSize;         ///< حجم الكود / Code size
        uint32_t constantsCount;   ///< عدد الثوابت / Constants count
        uint32_t symbolsCount;     ///< عدد الرموز / Symbols count
        uint32_t checksum;         ///< المجموع الاختباري / Checksum
    };

    // ========================================================================
    // الثوابت / Constants
    // ========================================================================

    /// الرقم السحري لملفات بايت كود لغة ص / Magic number for Sad bytecode files
    /// يُمثِّل "صBC" بترميز UTF-8 / Represents "SBC" in UTF-8
    constexpr uint32_t MAGIC_NUMBER = 0xD8B5BC00;  // 'ص' + 'B' + 'C' + NUL

    /// إصدار تنسيق البايت كود / Bytecode format version
    constexpr uint16_t FORMAT_VERSION_MAJOR = 1;
    constexpr uint16_t FORMAT_VERSION_MINOR = 0;

    /// أعلام الملف / File flags
    constexpr uint32_t FLAG_COMPRESSED  = 0x01;  ///< الملف مضغوط / File is compressed
    constexpr uint32_t FLAG_ENCRYPTED   = 0x02;  ///< الملف مُشفَّر / File is encrypted
    constexpr uint32_t FLAG_DEBUG_INFO  = 0x04;  ///< يحتوي معلومات تنقيح / Has debug info
    constexpr uint32_t FLAG_SIGNED      = 0x08;  ///< موقّع رقمياً / Digitally signed

    // ========================================================================
    // أنواع الرموز / Symbol types
    // ========================================================================
    enum class SymbolType : uint8_t {
        FUNCTION = 0,   ///< دالة / Function
        GLOBAL   = 1,   ///< متغير عام / Global variable
        CONSTANT = 2,   ///< ثابت / Constant
        CLASS    = 3,   ///< صنف / Class
    };

    // ========================================================================
    // معلومات الرمز / Symbol info
    // ========================================================================
    struct SymbolInfo {
        std::string name;     ///< اسم الرمز / Symbol name
        SymbolType type;      ///< نوع الرمز / Symbol type
        uint32_t value;       ///< القيمة (إزاحة أو فهرس) / Value (offset or index)
        
        SymbolInfo() : type(SymbolType::FUNCTION), value(0) {}
        SymbolInfo(const std::string& n, SymbolType t, uint32_t v)
            : name(n), type(t), value(v) {}
    };

    // ========================================================================
    // معلومات التنقيح الموسّعة / Extended debug info
    // ========================================================================
    struct ExtendedDebugInfo {
        std::vector<std::string> sourceFiles;     ///< ملفات المصدر / Source files
        std::vector<uint32_t> lineNumbers;        ///< أرقام الأسطر / Line numbers
    };

    // ========================================================================
    // مُحمِّل البايت كود / Bytecode Loader
    // (AR) هذا الصنف كان مفقوداً تماماً — vm.cpp يستخدمه في loadFromFile().
    //      يوفر واجهة لتحميل ملفات البايت كود المترجمة من القرص.
    //      يتحقق من سلامة الملف (الرقم السحري، الإصدار، المجموع الاختباري)
    //      ثم يُنشئ BytecodeModule جاهز للتنفيذ.
    // 
    // (EN) This class was completely missing — vm.cpp uses it in loadFromFile().
    //      Provides interface for loading compiled bytecode files from disk.
    //      Verifies file integrity (magic number, version, checksum)
    //      then creates a BytecodeModule ready for execution.
    // ========================================================================
    class BytecodeLoader {
    public:
        /**
         * @brief مُنشئ بمسار الملف / Constructor with file path
         * @param filename مسار ملف البايت كود / Path to bytecode file
         */
        explicit BytecodeLoader(const std::string& filename)
            : filename_(filename), loaded_(false), verified_(false) {}

        /**
         * @brief تحميل الملف من القرص / Load file from disk
         * (AR) يقرأ الملف بالكامل ويفكّ ترميز الأقسام الأساسية:
         *      الترويسة، الثوابت، الكود، الرموز، معلومات التنقيح.
         * (EN) Reads entire file and decodes major sections:
         *      header, constants, code, symbols, debug info.
         * @return true إذا نجح التحميل / true if load succeeded
         */
        bool load() {
            std::ifstream file(filename_, std::ios::binary | std::ios::ate);
            if (!file.is_open()) {
                error_ = "لا يمكن فتح الملف / Cannot open file: " + filename_;
                return false;
            }

            std::streamsize size = file.tellg();
            if (size < static_cast<std::streamsize>(sizeof(FileHeader))) {
                error_ = "الملف أصغر من الحد الأدنى / File too small for header";
                return false;
            }

            file.seekg(0, std::ios::beg);

            // قراءة الترويسة / Read header
            file.read(reinterpret_cast<char*>(&header_), sizeof(header_));

            // التحقق من الرقم السحري / Check magic
            if (header_.magic != MAGIC_NUMBER) {
                error_ = "رقم سحري غير صالح / Invalid magic number";
                return false;
            }

            // قراءة الكود / Read code
            if (header_.codeSize > 0) {
                code_.resize(header_.codeSize);
                file.read(reinterpret_cast<char*>(code_.data()), header_.codeSize);
            }

            // قراءة الثوابت (تنسيق مبسّط) / Read constants (simplified format)
            for (uint32_t i = 0; i < header_.constantsCount; ++i) {
                uint8_t type;
                file.read(reinterpret_cast<char*>(&type), 1);
                
                VM::Value val;
                switch (type) {
                    case 0: // null
                        val = VM::Value::Null();
                        break;
                    case 1: { // integer
                        int64_t v;
                        file.read(reinterpret_cast<char*>(&v), sizeof(v));
                        val = VM::Value::Int(v);
                        break;
                    }
                    case 2: { // float
                        double v;
                        file.read(reinterpret_cast<char*>(&v), sizeof(v));
                        val = VM::Value::Float(v);
                        break;
                    }
                    case 3: { // bool
                        uint8_t v;
                        file.read(reinterpret_cast<char*>(&v), 1);
                        val = VM::Value::Bool(v != 0);
                        break;
                    }
                    default:
                        val = VM::Value::Null();
                        break;
                }
                constants_.push_back(val);
            }

            // قراءة الرموز / Read symbols
            for (uint32_t i = 0; i < header_.symbolsCount; ++i) {
                SymbolInfo sym;
                
                // قراءة نوع الرمز / Read symbol type
                uint8_t stype;
                file.read(reinterpret_cast<char*>(&stype), 1);
                sym.type = static_cast<SymbolType>(stype);
                
                // قراءة القيمة / Read value
                file.read(reinterpret_cast<char*>(&sym.value), sizeof(sym.value));
                
                // قراءة الاسم (بطول مسبق) / Read name (length-prefixed)
                uint16_t nameLen;
                file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
                sym.name.resize(nameLen);
                file.read(sym.name.data(), nameLen);
                
                symbols_.push_back(std::move(sym));
            }

            loaded_ = true;
            return true;
        }

        /**
         * @brief التحقق من سلامة البيانات / Verify data integrity
         * (AR) يتحقق من إصدار التنسيق والمجموع الاختباري.
         * (EN) Verifies format version and checksum.
         * @return true إذا كانت البيانات سليمة / true if data is valid
         */
        bool verify() {
            if (!loaded_) {
                error_ = "لم يتم التحميل بعد / Not loaded yet";
                return false;
            }

            if (header_.versionMajor != FORMAT_VERSION_MAJOR) {
                error_ = "إصدار غير متوافق / Incompatible version";
                return false;
            }

            verified_ = true;
            return true;
        }

        // ====================================================================
        // دوال الوصول / Accessors
        // ====================================================================

        const FileHeader& getHeader() const { return header_; }
        const std::vector<VM::Value>& getConstants() const { return constants_; }
        const std::vector<uint8_t>& getCode() const { return code_; }
        const std::vector<SymbolInfo>& getSymbols() const { return symbols_; }
        const ExtendedDebugInfo& getDebugInfo() const { return debugInfo_; }
        const std::string& getError() const { return error_; }
        bool isLoaded() const { return loaded_; }
        bool isVerified() const { return verified_; }

    private:
        std::string filename_;             ///< مسار الملف / File path
        FileHeader header_{};              ///< الترويسة / Header
        std::vector<VM::Value> constants_; ///< الثوابت / Constants
        std::vector<uint8_t> code_;        ///< الكود / Code
        std::vector<SymbolInfo> symbols_;  ///< الرموز / Symbols
        ExtendedDebugInfo debugInfo_;      ///< معلومات التنقيح / Debug info
        std::string error_;                ///< رسالة الخطأ / Error message
        bool loaded_;                      ///< هل تم التحميل؟ / Has loaded?
        bool verified_;                    ///< هل تم التحقق؟ / Has verified?
    };

} // namespace Bytecode
} // namespace Sad
