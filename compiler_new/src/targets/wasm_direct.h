/**
 * =============================================================================
 * @file wasm_direct.h
 * @brief (AR) مُولد WASM مباشر — .ص → .wasm بدون Emscripten
 * @brief (EN) Direct WASM generator — .ص → .wasm without Emscripten
 * 
 * المهمة: T240 - WASM Direct Output
 * المرحلة: سبتمبر 2026 — WASM Direct Compilation
 * =============================================================================
 * 
 * 🎯 الهدف
 * ════════
 * تحويل كود لغة ص مباشرة إلى ملف .wasm ثنائي
 * يعمل في المتصفح بدون Emscripten أو أدوات خارجية
 * 
 * 📦 الميزات
 * ══════════
 * ✓ توليد WASM binary مباشر
 * ✓ إدارة ذاكرة (Linear Memory + GC)
 * ✓ JavaScript Interop (imports/exports)
 * ✓ دعم الأنواع العربية
 * ✓ String encoding (UTF-8)
 * ✓ Object heap management
 * 
 * =============================================================================
 */

#ifndef SAD_WASM_DIRECT_H
#define SAD_WASM_DIRECT_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <functional>

namespace sad::wasm {

// =============================================================================
// ثوابت WASM
// =============================================================================

constexpr uint8_t WASM_MAGIC[] = {0x00, 0x61, 0x73, 0x6D};  // \0asm
constexpr uint8_t WASM_VERSION[] = {0x01, 0x00, 0x00, 0x00}; // version 1

// Section IDs
enum class SectionId : uint8_t {
    Custom    = 0,
    Type      = 1,
    Import    = 2,
    Function  = 3,
    Table     = 4,
    Memory    = 5,
    Global    = 6,
    Export    = 7,
    Start     = 8,
    Element   = 9,
    Code      = 10,
    Data      = 11,
    DataCount = 12
};

// Value Types
enum class ValType : uint8_t {
    I32     = 0x7F,
    I64     = 0x7E,
    F32     = 0x7D,
    F64     = 0x7C,
    V128    = 0x7B,
    FuncRef = 0x70,
    ExternRef = 0x6F
};

// Export Types
enum class ExportKind : uint8_t {
    Func   = 0x00,
    Table  = 0x01,
    Memory = 0x02,
    Global = 0x03
};

// =============================================================================
// LEB128 Encoding
// =============================================================================

class LEB128 {
public:
    /**
     * @brief ترميز عدد صحيح غير مُعلّم بـ LEB128
     */
    static std::vector<uint8_t> encodeUnsigned(uint64_t value) {
        std::vector<uint8_t> result;
        do {
            uint8_t byte = value & 0x7F;
            value >>= 7;
            if (value != 0) {
                byte |= 0x80;
            }
            result.push_back(byte);
        } while (value != 0);
        return result;
    }
    
    /**
     * @brief ترميز عدد صحيح مُعلّم بـ LEB128
     */
    static std::vector<uint8_t> encodeSigned(int64_t value) {
        std::vector<uint8_t> result;
        bool more = true;
        while (more) {
            uint8_t byte = value & 0x7F;
            value >>= 7;
            
            // Handle sign extension
            bool signBit = (byte & 0x40) != 0;
            if ((value == 0 && !signBit) || (value == -1 && signBit)) {
                more = false;
            } else {
                byte |= 0x80;
            }
            result.push_back(byte);
        }
        return result;
    }
};

// =============================================================================
// تعليمات WASM (Opcodes)
// =============================================================================

namespace Op {
    // Control
    constexpr uint8_t Unreachable = 0x00;
    constexpr uint8_t Nop         = 0x01;
    constexpr uint8_t Block       = 0x02;
    constexpr uint8_t Loop        = 0x03;
    constexpr uint8_t If          = 0x04;
    constexpr uint8_t Else        = 0x05;
    constexpr uint8_t End         = 0x0B;
    constexpr uint8_t Br          = 0x0C;
    constexpr uint8_t BrIf        = 0x0D;
    constexpr uint8_t BrTable     = 0x0E;
    constexpr uint8_t Return      = 0x0F;
    constexpr uint8_t Call        = 0x10;
    constexpr uint8_t CallIndirect = 0x11;
    
    // Parametric
    constexpr uint8_t Drop   = 0x1A;
    constexpr uint8_t Select = 0x1B;
    
    // Local/Global
    constexpr uint8_t LocalGet  = 0x20;
    constexpr uint8_t LocalSet  = 0x21;
    constexpr uint8_t LocalTee  = 0x22;
    constexpr uint8_t GlobalGet = 0x23;
    constexpr uint8_t GlobalSet = 0x24;
    
    // Memory Load
    constexpr uint8_t I32Load    = 0x28;
    constexpr uint8_t I64Load    = 0x29;
    constexpr uint8_t F32Load    = 0x2A;
    constexpr uint8_t F64Load    = 0x2B;
    constexpr uint8_t I32Load8S  = 0x2C;
    constexpr uint8_t I32Load8U  = 0x2D;
    constexpr uint8_t I32Load16S = 0x2E;
    constexpr uint8_t I32Load16U = 0x2F;
    
    // Memory Store
    constexpr uint8_t I32Store   = 0x36;
    constexpr uint8_t I64Store   = 0x37;
    constexpr uint8_t F32Store   = 0x38;
    constexpr uint8_t F64Store   = 0x39;
    constexpr uint8_t I32Store8  = 0x3A;
    constexpr uint8_t I32Store16 = 0x3B;
    
    // Memory
    constexpr uint8_t MemorySize = 0x3F;
    constexpr uint8_t MemoryGrow = 0x40;
    
    // Constants
    constexpr uint8_t I32Const = 0x41;
    constexpr uint8_t I64Const = 0x42;
    constexpr uint8_t F32Const = 0x43;
    constexpr uint8_t F64Const = 0x44;
    
    // i32 Comparison
    constexpr uint8_t I32Eqz  = 0x45;
    constexpr uint8_t I32Eq   = 0x46;
    constexpr uint8_t I32Ne   = 0x47;
    constexpr uint8_t I32LtS  = 0x48;
    constexpr uint8_t I32LtU  = 0x49;
    constexpr uint8_t I32GtS  = 0x4A;
    constexpr uint8_t I32GtU  = 0x4B;
    constexpr uint8_t I32LeS  = 0x4C;
    constexpr uint8_t I32LeU  = 0x4D;
    constexpr uint8_t I32GeS  = 0x4E;
    constexpr uint8_t I32GeU  = 0x4F;
    
    // i64 Comparison
    constexpr uint8_t I64Eqz  = 0x50;
    constexpr uint8_t I64Eq   = 0x51;
    constexpr uint8_t I64Ne   = 0x52;
    constexpr uint8_t I64LtS  = 0x53;
    constexpr uint8_t I64GtS  = 0x55;
    constexpr uint8_t I64LeS  = 0x57;
    constexpr uint8_t I64GeS  = 0x59;
    
    // f32/f64 Comparison
    constexpr uint8_t F32Eq = 0x5B;
    constexpr uint8_t F64Eq = 0x61;
    constexpr uint8_t F64Lt = 0x63;
    constexpr uint8_t F64Gt = 0x64;
    constexpr uint8_t F64Le = 0x65;
    constexpr uint8_t F64Ge = 0x66;
    
    // i32 Arithmetic
    constexpr uint8_t I32Clz    = 0x67;
    constexpr uint8_t I32Ctz    = 0x68;
    constexpr uint8_t I32Popcnt = 0x69;
    constexpr uint8_t I32Add    = 0x6A;
    constexpr uint8_t I32Sub    = 0x6B;
    constexpr uint8_t I32Mul    = 0x6C;
    constexpr uint8_t I32DivS   = 0x6D;
    constexpr uint8_t I32DivU   = 0x6E;
    constexpr uint8_t I32RemS   = 0x6F;
    constexpr uint8_t I32RemU   = 0x70;
    constexpr uint8_t I32And    = 0x71;
    constexpr uint8_t I32Or     = 0x72;
    constexpr uint8_t I32Xor    = 0x73;
    constexpr uint8_t I32Shl    = 0x74;
    constexpr uint8_t I32ShrS   = 0x75;
    constexpr uint8_t I32ShrU   = 0x76;
    constexpr uint8_t I32Rotl   = 0x77;
    constexpr uint8_t I32Rotr   = 0x78;
    
    // i64 Arithmetic
    constexpr uint8_t I64Add  = 0x7C;
    constexpr uint8_t I64Sub  = 0x7D;
    constexpr uint8_t I64Mul  = 0x7E;
    constexpr uint8_t I64DivS = 0x7F;
    constexpr uint8_t I64And  = 0x83;
    constexpr uint8_t I64Or   = 0x84;
    constexpr uint8_t I64Xor  = 0x85;
    
    // f32 Arithmetic
    constexpr uint8_t F32Abs  = 0x8B;
    constexpr uint8_t F32Neg  = 0x8C;
    constexpr uint8_t F32Ceil = 0x8D;
    constexpr uint8_t F32Floor= 0x8E;
    constexpr uint8_t F32Sqrt = 0x91;
    constexpr uint8_t F32Add  = 0x92;
    constexpr uint8_t F32Sub  = 0x93;
    constexpr uint8_t F32Mul  = 0x94;
    constexpr uint8_t F32Div  = 0x95;
    
    // f64 Arithmetic
    constexpr uint8_t F64Abs  = 0x99;
    constexpr uint8_t F64Neg  = 0x9A;
    constexpr uint8_t F64Ceil = 0x9B;
    constexpr uint8_t F64Floor= 0x9C;
    constexpr uint8_t F64Sqrt = 0x9F;
    constexpr uint8_t F64Add  = 0xA0;
    constexpr uint8_t F64Sub  = 0xA1;
    constexpr uint8_t F64Mul  = 0xA2;
    constexpr uint8_t F64Div  = 0xA3;
    
    // Conversions
    constexpr uint8_t I32WrapI64     = 0xA7;
    constexpr uint8_t I32TruncF32S   = 0xA8;
    constexpr uint8_t I32TruncF64S   = 0xAA;
    constexpr uint8_t I64ExtendI32S  = 0xAC;
    constexpr uint8_t I64ExtendI32U  = 0xAD;
    constexpr uint8_t I64TruncF64S   = 0xB0;
    constexpr uint8_t F32ConvertI32S = 0xB2;
    constexpr uint8_t F32ConvertI64S = 0xB4;
    constexpr uint8_t F64ConvertI32S = 0xB7;
    constexpr uint8_t F64ConvertI64S = 0xB9;
    constexpr uint8_t I32ReinterpretF32 = 0xBC;
    constexpr uint8_t F32ReinterpretI32 = 0xBE;
}

// =============================================================================
// نوع الدالة
// =============================================================================

struct FuncType {
    std::vector<ValType> params;
    std::vector<ValType> results;
    
    bool operator==(const FuncType& other) const {
        return params == other.params && results == other.results;
    }
};

// =============================================================================
// الاستيراد
// =============================================================================

struct Import {
    std::string module;
    std::string name;
    
    enum class Kind { Func, Table, Memory, Global } kind;
    
    // للدوال
    uint32_t typeIndex = 0;
    
    // للذاكرة
    uint32_t memMin = 0;
    uint32_t memMax = 0;
    bool hasMax = false;
    
    // للمتغيرات العامة
    ValType globalType = ValType::I32;
    bool globalMutable = false;
};

// =============================================================================
// التصدير
// =============================================================================

struct Export {
    std::string name;
    ExportKind kind;
    uint32_t index;
};

// =============================================================================
// الدالة
// =============================================================================

struct Function {
    std::string name;
    uint32_t typeIndex;
    std::vector<ValType> locals;
    std::vector<uint8_t> code;
    bool exported = false;
};

// =============================================================================
// البيانات
// =============================================================================

struct DataSegment {
    uint32_t memoryIndex = 0;
    int32_t offset = 0;
    std::vector<uint8_t> data;
};

// =============================================================================
// المتغير العام
// =============================================================================

struct Global {
    ValType type;
    bool mutable_;
    std::vector<uint8_t> init;  // Init expression
};

// =============================================================================
// إدارة الذاكرة
// =============================================================================

class MemoryManager {
public:
    static constexpr uint32_t PAGE_SIZE = 65536;  // 64KB per page
    static constexpr uint32_t HEAP_START = 1024;   // بداية الـ heap
    
    MemoryManager(uint32_t initialPages = 1, uint32_t maxPages = 256)
        : initialPages_(initialPages), maxPages_(maxPages), heapPtr_(HEAP_START) {}
    
    /**
     * @brief حجز ذاكرة في الـ heap
     * @param size حجم الذاكرة المطلوبة
     * @return عنوان الذاكرة المحجوزة
     */
    uint32_t allocate(uint32_t size) {
        // محاذاة على 8 bytes
        uint32_t aligned = (size + 7) & ~7;
        uint32_t addr = heapPtr_;
        heapPtr_ += aligned;
        allocations_[addr] = aligned;
        return addr;
    }
    
    /**
     * @brief تحرير ذاكرة
     */
    void free(uint32_t addr) {
        allocations_.erase(addr);
    }
    
    /**
     * @brief حجز نص UTF-8
     * @return عنوان النص في الذاكرة
     */
    uint32_t allocateString(const std::string& str) {
        uint32_t addr = allocate(str.size() + 5);  // 4 bytes length + null
        stringTable_[addr] = str;
        return addr;
    }
    
    /**
     * @brief إنشاء Data segments للنصوص
     */
    std::vector<DataSegment> createDataSegments() const {
        std::vector<DataSegment> segments;
        
        for (const auto& [addr, str] : stringTable_) {
            DataSegment seg;
            seg.offset = addr;
            
            // كتابة الطول أولاً (4 bytes, little-endian)
            uint32_t len = str.size();
            seg.data.push_back(len & 0xFF);
            seg.data.push_back((len >> 8) & 0xFF);
            seg.data.push_back((len >> 16) & 0xFF);
            seg.data.push_back((len >> 24) & 0xFF);
            
            // كتابة النص
            for (char c : str) {
                seg.data.push_back(static_cast<uint8_t>(c));
            }
            seg.data.push_back(0);  // null terminator
            
            segments.push_back(seg);
        }
        
        return segments;
    }
    
    uint32_t getInitialPages() const { return initialPages_; }
    uint32_t getMaxPages() const { return maxPages_; }
    uint32_t getHeapPtr() const { return heapPtr_; }
    
private:
    uint32_t initialPages_;
    uint32_t maxPages_;
    uint32_t heapPtr_;
    std::map<uint32_t, uint32_t> allocations_;
    std::map<uint32_t, std::string> stringTable_;
};

// =============================================================================
// مُولد WASM المباشر
// =============================================================================

class WasmDirectGenerator {
public:
    WasmDirectGenerator() : memory_(2, 256) {
        // إضافة استيرادات JavaScript الافتراضية
        addDefaultImports();
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // إدارة الأنواع
    // ─────────────────────────────────────────────────────────────────────
    
    /**
     * @brief إضافة نوع دالة
     * @return فهرس النوع
     */
    uint32_t addFuncType(const std::vector<ValType>& params,
                         const std::vector<ValType>& results) {
        FuncType ft{params, results};
        
        // التحقق من وجود النوع
        for (size_t i = 0; i < types_.size(); i++) {
            if (types_[i] == ft) {
                return static_cast<uint32_t>(i);
            }
        }
        
        types_.push_back(ft);
        return static_cast<uint32_t>(types_.size() - 1);
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // الاستيرادات
    // ─────────────────────────────────────────────────────────────────────
    
    /**
     * @brief استيراد دالة JavaScript
     */
    void importFunction(const std::string& module, const std::string& name,
                        const std::vector<ValType>& params,
                        const std::vector<ValType>& results) {
        Import imp;
        imp.module = module;
        imp.name = name;
        imp.kind = Import::Kind::Func;
        imp.typeIndex = addFuncType(params, results);
        imports_.push_back(imp);
        importedFuncCount_++;
    }
    
    /**
     * @brief إضافة الاستيرادات الافتراضية
     */
    void addDefaultImports() {
        // console.log للأعداد
        importFunction("env", "console_log_i32", {ValType::I32}, {});
        
        // console.log للنصوص (يأخذ عنوان وطول)
        importFunction("env", "console_log_str", {ValType::I32, ValType::I32}, {});
        
        // إدخال المستخدم
        importFunction("env", "prompt_i32", {}, {ValType::I32});
        
        // Math functions
        importFunction("env", "math_sqrt", {ValType::F64}, {ValType::F64});
        importFunction("env", "math_sin", {ValType::F64}, {ValType::F64});
        importFunction("env", "math_cos", {ValType::F64}, {ValType::F64});
        importFunction("env", "math_random", {}, {ValType::F64});
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // الدوال
    // ─────────────────────────────────────────────────────────────────────
    
    /**
     * @brief بدء دالة جديدة
     */
    void beginFunction(const std::string& name,
                       const std::vector<ValType>& params,
                       const std::vector<ValType>& results,
                       bool exported = true) {
        currentFunc_ = Function();
        currentFunc_.name = name;
        currentFunc_.typeIndex = addFuncType(params, results);
        currentFunc_.exported = exported;
        
        // تعيين أسماء المعطيات
        currentScope_.clear();
        for (size_t i = 0; i < params.size(); i++) {
            currentScope_["$p" + std::to_string(i)] = static_cast<uint32_t>(i);
        }
    }
    
    /**
     * @brief إضافة متغير محلي
     */
    uint32_t addLocal(ValType type, const std::string& name = "") {
        currentFunc_.locals.push_back(type);
        uint32_t idx = static_cast<uint32_t>(
            types_[currentFunc_.typeIndex].params.size() + 
            currentFunc_.locals.size() - 1
        );
        if (!name.empty()) {
            currentScope_[name] = idx;
        }
        return idx;
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // إصدار التعليمات
    // ─────────────────────────────────────────────────────────────────────
    
    void emit(uint8_t opcode) {
        currentFunc_.code.push_back(opcode);
    }
    
    void emitLEB(uint32_t value) {
        auto encoded = LEB128::encodeUnsigned(value);
        for (uint8_t b : encoded) {
            currentFunc_.code.push_back(b);
        }
    }
    
    void emitSignedLEB(int32_t value) {
        auto encoded = LEB128::encodeSigned(value);
        for (uint8_t b : encoded) {
            currentFunc_.code.push_back(b);
        }
    }
    
    // ثوابت
    void emitI32Const(int32_t value) {
        emit(Op::I32Const);
        emitSignedLEB(value);
    }
    
    void emitI64Const(int64_t value) {
        emit(Op::I64Const);
        auto encoded = LEB128::encodeSigned(value);
        for (uint8_t b : encoded) {
            currentFunc_.code.push_back(b);
        }
    }
    
    void emitF32Const(float value) {
        emit(Op::F32Const);
        uint32_t bits;
        memcpy(&bits, &value, sizeof(bits));
        currentFunc_.code.push_back(bits & 0xFF);
        currentFunc_.code.push_back((bits >> 8) & 0xFF);
        currentFunc_.code.push_back((bits >> 16) & 0xFF);
        currentFunc_.code.push_back((bits >> 24) & 0xFF);
    }
    
    void emitF64Const(double value) {
        emit(Op::F64Const);
        uint64_t bits;
        memcpy(&bits, &value, sizeof(bits));
        for (int i = 0; i < 8; i++) {
            currentFunc_.code.push_back((bits >> (i * 8)) & 0xFF);
        }
    }
    
    // متغيرات محلية
    void emitLocalGet(uint32_t idx) { emit(Op::LocalGet); emitLEB(idx); }
    void emitLocalSet(uint32_t idx) { emit(Op::LocalSet); emitLEB(idx); }
    void emitLocalTee(uint32_t idx) { emit(Op::LocalTee); emitLEB(idx); }
    
    // متغيرات عامة
    void emitGlobalGet(uint32_t idx) { emit(Op::GlobalGet); emitLEB(idx); }
    void emitGlobalSet(uint32_t idx) { emit(Op::GlobalSet); emitLEB(idx); }
    
    // استدعاء
    void emitCall(uint32_t funcIdx) { emit(Op::Call); emitLEB(funcIdx); }
    
    // عمليات حسابية
    void emitI32Add() { emit(Op::I32Add); }
    void emitI32Sub() { emit(Op::I32Sub); }
    void emitI32Mul() { emit(Op::I32Mul); }
    void emitI32DivS() { emit(Op::I32DivS); }
    void emitI32RemS() { emit(Op::I32RemS); }
    void emitI32And() { emit(Op::I32And); }
    void emitI32Or() { emit(Op::I32Or); }
    void emitI32Xor() { emit(Op::I32Xor); }
    
    void emitF64Add() { emit(Op::F64Add); }
    void emitF64Sub() { emit(Op::F64Sub); }
    void emitF64Mul() { emit(Op::F64Mul); }
    void emitF64Div() { emit(Op::F64Div); }
    
    // مقارنات
    void emitI32Eq() { emit(Op::I32Eq); }
    void emitI32Ne() { emit(Op::I32Ne); }
    void emitI32LtS() { emit(Op::I32LtS); }
    void emitI32GtS() { emit(Op::I32GtS); }
    void emitI32LeS() { emit(Op::I32LeS); }
    void emitI32GeS() { emit(Op::I32GeS); }
    void emitI32Eqz() { emit(Op::I32Eqz); }
    
    // تحكم
    void emitReturn() { emit(Op::Return); }
    void emitDrop() { emit(Op::Drop); }
    void emitEnd() { emit(Op::End); }
    
    void emitBlock(ValType resultType = ValType::I32) {
        emit(Op::Block);
        emit(static_cast<uint8_t>(resultType));
    }
    
    void emitLoop() {
        emit(Op::Loop);
        emit(0x40);  // void
    }
    
    void emitIf(ValType resultType = ValType::I32) {
        emit(Op::If);
        emit(0x40);  // void بشكل افتراضي
    }
    
    void emitElse() { emit(Op::Else); }
    
    void emitBr(uint32_t depth) { emit(Op::Br); emitLEB(depth); }
    void emitBrIf(uint32_t depth) { emit(Op::BrIf); emitLEB(depth); }
    
    // ذاكرة
    void emitI32Load(uint32_t align = 2, uint32_t offset = 0) {
        emit(Op::I32Load);
        emitLEB(align);
        emitLEB(offset);
    }
    
    void emitI32Store(uint32_t align = 2, uint32_t offset = 0) {
        emit(Op::I32Store);
        emitLEB(align);
        emitLEB(offset);
    }
    
    void emitMemoryGrow() {
        emit(Op::MemoryGrow);
        emitLEB(0);  // memory index
    }
    
    /**
     * @brief إنهاء الدالة الحالية
     */
    void endFunction() {
        emit(Op::End);  // End function body
        functions_.push_back(currentFunc_);
        
        if (currentFunc_.exported) {
            Export exp;
            exp.name = currentFunc_.name;
            exp.kind = ExportKind::Func;
            exp.index = importedFuncCount_ + 
                        static_cast<uint32_t>(functions_.size() - 1);
            exports_.push_back(exp);
        }
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // إضافة نص للذاكرة
    // ─────────────────────────────────────────────────────────────────────
    
    uint32_t addString(const std::string& str) {
        return memory_.allocateString(str);
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // التوليد
    // ─────────────────────────────────────────────────────────────────────
    
    /**
     * @brief توليد ملف WASM ثنائي
     */
    std::vector<uint8_t> generate() {
        std::vector<uint8_t> wasm;
        
        // Magic + Version
        wasm.insert(wasm.end(), WASM_MAGIC, WASM_MAGIC + 4);
        wasm.insert(wasm.end(), WASM_VERSION, WASM_VERSION + 4);
        
        // Type Section
        writeTypeSection(wasm);
        
        // Import Section
        writeImportSection(wasm);
        
        // Function Section
        writeFunctionSection(wasm);
        
        // Memory Section
        writeMemorySection(wasm);
        
        // Global Section (heap pointer)
        writeGlobalSection(wasm);
        
        // Export Section
        writeExportSection(wasm);
        
        // Code Section
        writeCodeSection(wasm);
        
        // Data Section
        writeDataSection(wasm);
        
        return wasm;
    }
    
    /**
     * @brief حفظ الملف
     */
    bool saveToFile(const std::string& path) {
        auto binary = generate();
        std::ofstream file(path, std::ios::binary);
        if (!file) return false;
        file.write(reinterpret_cast<const char*>(binary.data()), binary.size());
        return true;
    }
    
    /**
     * @brief توليد كود JavaScript المرافق
     */
    std::string generateJsGlue() const {
        std::ostringstream js;
        
        js << "// مُولد من لغة ص — Sad WASM Runtime\n\n";
        
        js << "const SadRuntime = {\n";
        js << "  memory: null,\n";
        js << "  instance: null,\n";
        js << "  decoder: new TextDecoder('utf-8'),\n";
        js << "  encoder: new TextEncoder(),\n\n";
        
        js << "  // قراءة نص من الذاكرة\n";
        js << "  readString(ptr) {\n";
        js << "    const view = new DataView(this.memory.buffer);\n";
        js << "    const len = view.getUint32(ptr, true);\n";
        js << "    const bytes = new Uint8Array(this.memory.buffer, ptr + 4, len);\n";
        js << "    return this.decoder.decode(bytes);\n";
        js << "  },\n\n";
        
        js << "  // الاستيرادات للـ WASM\n";
        js << "  imports: {\n";
        js << "    env: {\n";
        js << "      console_log_i32: (val) => console.log('🔢', val),\n";
        js << "      console_log_str: (ptr, len) => {\n";
        js << "        const bytes = new Uint8Array(SadRuntime.memory.buffer, ptr, len);\n";
        js << "        console.log('📝', SadRuntime.decoder.decode(bytes));\n";
        js << "      },\n";
        js << "      prompt_i32: () => parseInt(prompt('أدخل رقمًا:') || '0'),\n";
        js << "      math_sqrt: Math.sqrt,\n";
        js << "      math_sin: Math.sin,\n";
        js << "      math_cos: Math.cos,\n";
        js << "      math_random: Math.random\n";
        js << "    }\n";
        js << "  },\n\n";
        
        js << "  // تحميل وتشغيل\n";
        js << "  async load(url) {\n";
        js << "    const response = await fetch(url);\n";
        js << "    const bytes = await response.arrayBuffer();\n";
        js << "    const { instance } = await WebAssembly.instantiate(bytes, this.imports);\n";
        js << "    this.instance = instance;\n";
        js << "    this.memory = instance.exports.memory;\n";
        js << "    return this;\n";
        js << "  },\n\n";
        
        js << "  // استدعاء دالة\n";
        js << "  call(name, ...args) {\n";
        js << "    return this.instance.exports[name](...args);\n";
        js << "  }\n";
        js << "};\n\n";
        
        js << "// تحميل تلقائي\n";
        js << "// SadRuntime.load('program.wasm').then(rt => rt.call('main'));\n";
        
        return js.str();
    }
    
    /**
     * @brief توليد HTML للاختبار
     */
    std::string generateHtml(const std::string& wasmFile) const {
        std::ostringstream html;
        
        html << "<!DOCTYPE html>\n";
        html << "<html lang=\"ar\" dir=\"rtl\">\n";
        html << "<head>\n";
        html << "  <meta charset=\"UTF-8\">\n";
        html << "  <title>برنامج لغة ص</title>\n";
        html << "  <style>\n";
        html << "    body { font-family: 'Segoe UI', Tahoma, sans-serif; padding: 20px; }\n";
        html << "    #output { background: #1e1e1e; color: #d4d4d4; padding: 15px;\n";
        html << "              border-radius: 8px; font-family: monospace; }\n";
        html << "  </style>\n";
        html << "</head>\n";
        html << "<body>\n";
        html << "  <h1>🔮 برنامج لغة ص</h1>\n";
        html << "  <div id=\"output\">جاري التحميل...</div>\n";
        html << "  <script>\n";
        html << generateJsGlue();
        html << "\n    SadRuntime.load('" << wasmFile << "').then(rt => {\n";
        html << "      document.getElementById('output').textContent = '';\n";
        html << "      rt.call('main');\n";
        html << "    });\n";
        html << "  </script>\n";
        html << "</body>\n";
        html << "</html>\n";
        
        return html.str();
    }

private:
    // ─────────────────────────────────────────────────────────────────────
    // كتابة الأقسام
    // ─────────────────────────────────────────────────────────────────────
    
    void writeSection(std::vector<uint8_t>& wasm, SectionId id,
                      const std::vector<uint8_t>& content) {
        wasm.push_back(static_cast<uint8_t>(id));
        auto sizeBytes = LEB128::encodeUnsigned(content.size());
        wasm.insert(wasm.end(), sizeBytes.begin(), sizeBytes.end());
        wasm.insert(wasm.end(), content.begin(), content.end());
    }
    
    void writeTypeSection(std::vector<uint8_t>& wasm) {
        if (types_.empty()) return;
        
        std::vector<uint8_t> content;
        auto count = LEB128::encodeUnsigned(types_.size());
        content.insert(content.end(), count.begin(), count.end());
        
        for (const auto& type : types_) {
            content.push_back(0x60);  // func type
            
            auto paramCount = LEB128::encodeUnsigned(type.params.size());
            content.insert(content.end(), paramCount.begin(), paramCount.end());
            for (auto p : type.params) {
                content.push_back(static_cast<uint8_t>(p));
            }
            
            auto resultCount = LEB128::encodeUnsigned(type.results.size());
            content.insert(content.end(), resultCount.begin(), resultCount.end());
            for (auto r : type.results) {
                content.push_back(static_cast<uint8_t>(r));
            }
        }
        
        writeSection(wasm, SectionId::Type, content);
    }
    
    void writeImportSection(std::vector<uint8_t>& wasm) {
        if (imports_.empty()) return;
        
        std::vector<uint8_t> content;
        auto count = LEB128::encodeUnsigned(imports_.size());
        content.insert(content.end(), count.begin(), count.end());
        
        for (const auto& imp : imports_) {
            // Module name
            auto modLen = LEB128::encodeUnsigned(imp.module.size());
            content.insert(content.end(), modLen.begin(), modLen.end());
            content.insert(content.end(), imp.module.begin(), imp.module.end());
            
            // Name
            auto nameLen = LEB128::encodeUnsigned(imp.name.size());
            content.insert(content.end(), nameLen.begin(), nameLen.end());
            content.insert(content.end(), imp.name.begin(), imp.name.end());
            
            // Kind + details
            content.push_back(0x00);  // func
            auto typeIdx = LEB128::encodeUnsigned(imp.typeIndex);
            content.insert(content.end(), typeIdx.begin(), typeIdx.end());
        }
        
        writeSection(wasm, SectionId::Import, content);
    }
    
    void writeFunctionSection(std::vector<uint8_t>& wasm) {
        if (functions_.empty()) return;
        
        std::vector<uint8_t> content;
        auto count = LEB128::encodeUnsigned(functions_.size());
        content.insert(content.end(), count.begin(), count.end());
        
        for (const auto& func : functions_) {
            auto typeIdx = LEB128::encodeUnsigned(func.typeIndex);
            content.insert(content.end(), typeIdx.begin(), typeIdx.end());
        }
        
        writeSection(wasm, SectionId::Function, content);
    }
    
    void writeMemorySection(std::vector<uint8_t>& wasm) {
        std::vector<uint8_t> content;
        content.push_back(0x01);  // 1 memory
        content.push_back(0x01);  // has max
        auto min = LEB128::encodeUnsigned(memory_.getInitialPages());
        content.insert(content.end(), min.begin(), min.end());
        auto max = LEB128::encodeUnsigned(memory_.getMaxPages());
        content.insert(content.end(), max.begin(), max.end());
        
        writeSection(wasm, SectionId::Memory, content);
    }
    
    void writeGlobalSection(std::vector<uint8_t>& wasm) {
        std::vector<uint8_t> content;
        content.push_back(0x01);  // 1 global (heap pointer)
        
        // heap pointer: i32, mutable
        content.push_back(static_cast<uint8_t>(ValType::I32));
        content.push_back(0x01);  // mutable
        
        // init expr: i32.const heapPtr
        content.push_back(Op::I32Const);
        auto heapPtr = LEB128::encodeSigned(memory_.getHeapPtr());
        content.insert(content.end(), heapPtr.begin(), heapPtr.end());
        content.push_back(Op::End);
        
        writeSection(wasm, SectionId::Global, content);
    }
    
    void writeExportSection(std::vector<uint8_t>& wasm) {
        // تصدير الذاكرة
        Export memExport{"memory", ExportKind::Memory, 0};
        
        std::vector<uint8_t> content;
        auto count = LEB128::encodeUnsigned(exports_.size() + 1);
        content.insert(content.end(), count.begin(), count.end());
        
        // Memory export
        auto nameLen = LEB128::encodeUnsigned(memExport.name.size());
        content.insert(content.end(), nameLen.begin(), nameLen.end());
        content.insert(content.end(), memExport.name.begin(), memExport.name.end());
        content.push_back(static_cast<uint8_t>(ExportKind::Memory));
        content.push_back(0x00);
        
        // Function exports
        for (const auto& exp : exports_) {
            auto nameLen = LEB128::encodeUnsigned(exp.name.size());
            content.insert(content.end(), nameLen.begin(), nameLen.end());
            content.insert(content.end(), exp.name.begin(), exp.name.end());
            content.push_back(static_cast<uint8_t>(exp.kind));
            auto idx = LEB128::encodeUnsigned(exp.index);
            content.insert(content.end(), idx.begin(), idx.end());
        }
        
        writeSection(wasm, SectionId::Export, content);
    }
    
    void writeCodeSection(std::vector<uint8_t>& wasm) {
        if (functions_.empty()) return;
        
        std::vector<uint8_t> content;
        auto count = LEB128::encodeUnsigned(functions_.size());
        content.insert(content.end(), count.begin(), count.end());
        
        for (const auto& func : functions_) {
            std::vector<uint8_t> funcBody;
            
            // Locals
            if (func.locals.empty()) {
                funcBody.push_back(0x00);
            } else {
                // Compress locals by type
                std::vector<std::pair<uint32_t, ValType>> compressed;
                ValType currentType = func.locals[0];
                uint32_t currentCount = 1;
                
                for (size_t i = 1; i < func.locals.size(); i++) {
                    if (func.locals[i] == currentType) {
                        currentCount++;
                    } else {
                        compressed.push_back({currentCount, currentType});
                        currentType = func.locals[i];
                        currentCount = 1;
                    }
                }
                compressed.push_back({currentCount, currentType});
                
                auto localCount = LEB128::encodeUnsigned(compressed.size());
                funcBody.insert(funcBody.end(), localCount.begin(), localCount.end());
                
                for (const auto& [cnt, type] : compressed) {
                    auto c = LEB128::encodeUnsigned(cnt);
                    funcBody.insert(funcBody.end(), c.begin(), c.end());
                    funcBody.push_back(static_cast<uint8_t>(type));
                }
            }
            
            // Code
            funcBody.insert(funcBody.end(), func.code.begin(), func.code.end());
            
            // Function body size
            auto bodySize = LEB128::encodeUnsigned(funcBody.size());
            content.insert(content.end(), bodySize.begin(), bodySize.end());
            content.insert(content.end(), funcBody.begin(), funcBody.end());
        }
        
        writeSection(wasm, SectionId::Code, content);
    }
    
    void writeDataSection(std::vector<uint8_t>& wasm) {
        auto segments = memory_.createDataSegments();
        if (segments.empty()) return;
        
        std::vector<uint8_t> content;
        auto count = LEB128::encodeUnsigned(segments.size());
        content.insert(content.end(), count.begin(), count.end());
        
        for (const auto& seg : segments) {
            content.push_back(0x00);  // active, memory 0
            
            // Offset expression
            content.push_back(Op::I32Const);
            auto offset = LEB128::encodeSigned(seg.offset);
            content.insert(content.end(), offset.begin(), offset.end());
            content.push_back(Op::End);
            
            // Data
            auto dataLen = LEB128::encodeUnsigned(seg.data.size());
            content.insert(content.end(), dataLen.begin(), dataLen.end());
            content.insert(content.end(), seg.data.begin(), seg.data.end());
        }
        
        writeSection(wasm, SectionId::Data, content);
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // الحقول
    // ─────────────────────────────────────────────────────────────────────
    
    std::vector<FuncType> types_;
    std::vector<Import> imports_;
    std::vector<Function> functions_;
    std::vector<Export> exports_;
    MemoryManager memory_;
    
    Function currentFunc_;
    std::map<std::string, uint32_t> currentScope_;
    uint32_t importedFuncCount_ = 0;
};

} // namespace sad::wasm

#endif // SAD_WASM_DIRECT_H
