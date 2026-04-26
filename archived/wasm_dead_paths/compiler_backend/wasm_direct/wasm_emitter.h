/**
 * @file wasm_emitter.h
 * @brief (AR) مُصدر ثنائي مباشر لـ WebAssembly — بدون LLVM أو Emscripten
 * @brief (EN) Direct WebAssembly binary emitter — no LLVM or Emscripten needed
 *
 * يولد ملفات .wasm صالحة مباشرة من هياكل بيانات مُبسّطة
 */

#pragma once
#include "wasm_binary.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <fstream>

namespace Sad {
namespace Compiler {
namespace WasmDirect {

/**
 * مُصدر WASM مباشر
 * يبني وحدة WASM كاملة ويُصدرها كملف ثنائي .wasm
 */
class WasmEmitter {
public:
    WasmEmitter();
    ~WasmEmitter() = default;

    // ─── إضافة نوع دالة (توقيع) ───
    uint32_t addFuncType(const WasmFuncType& type);

    // ─── إضافة استيراد دالة ───
    uint32_t addImport(const std::string& module, const std::string& field, 
                       const WasmFuncType& type);

    // ─── إضافة دالة ───
    uint32_t addFunction(uint32_t typeIndex);
    uint32_t addFunction(const WasmFuncType& type);

    // ─── تعيين جسم دالة ───
    void setFunctionBody(uint32_t funcIndex, const WasmFuncBody& body);

    // ─── إضافة تصدير ───
    void addExport(const std::string& name, ExportKind kind, uint32_t index);
    void exportFunction(const std::string& name, uint32_t funcIndex);
    void exportMemory(const std::string& name, uint32_t memIndex = 0);

    // ─── الذاكرة ───
    void setMemory(uint32_t initialPages, uint32_t maxPages = 0);

    // ─── المتغيرات العامة ───
    uint32_t addGlobal(ValType type, bool mutable_, int32_t initValue);
    uint32_t addGlobalF64(ValType type, bool mutable_, double initValue);

    // ─── البيانات ───
    uint32_t addDataSegment(uint32_t offset, const std::vector<uint8_t>& data);
    uint32_t addDataSegment(uint32_t offset, const std::string& str);

    // ─── الجدول ───
    void setTable(uint32_t initialSize, uint32_t maxSize = 0);

    // ─── دالة البداية ───
    void setStartFunction(uint32_t funcIndex);

    // ─── التوليد ───
    std::vector<uint8_t> emit() const;
    bool emitToFile(const std::string& filePath) const;

    // ─── معلومات ───
    uint32_t getFunctionCount() const;
    uint32_t getImportCount() const;
    uint32_t getTotalFunctionIndex() const; // imports + functions

    // ─── مؤشر البيانات ───
    uint32_t getDataOffset() const { return dataOffset_; }

private:
    // أنواع الدوال
    std::vector<WasmFuncType> types_;

    // الاستيرادات
    std::vector<WasmImport> imports_;

    // فهارس أنواع الدوال المحلية
    std::vector<uint32_t> functionTypes_;

    // أجسام الدوال
    std::vector<WasmFuncBody> functionBodies_;

    // التصديرات
    std::vector<WasmExport> exports_;

    // العامة
    std::vector<WasmGlobal> globals_;

    // مقاطع البيانات
    std::vector<WasmDataSegment> dataSegments_;

    // الذاكرة
    uint32_t memoryInitial_ = 16;   // 16 صفحة = 1MB
    uint32_t memoryMax_ = 256;      // 256 صفحة = 16MB
    bool hasMemory_ = true;

    // الجدول
    bool hasTable_ = false;
    uint32_t tableInitial_ = 0;
    uint32_t tableMax_ = 0;

    // دالة البداية
    bool hasStart_ = false;
    uint32_t startFunc_ = 0;

    // إزاحة البيانات الحالية
    mutable uint32_t dataOffset_ = 1024; // بداية البيانات بعد 1KB

    // مساعدات بناء الأقسام
    std::vector<uint8_t> buildTypeSection() const;
    std::vector<uint8_t> buildImportSection() const;
    std::vector<uint8_t> buildFunctionSection() const;
    std::vector<uint8_t> buildTableSection() const;
    std::vector<uint8_t> buildMemorySection() const;
    std::vector<uint8_t> buildGlobalSection() const;
    std::vector<uint8_t> buildExportSection() const;
    std::vector<uint8_t> buildStartSection() const;
    std::vector<uint8_t> buildCodeSection() const;
    std::vector<uint8_t> buildDataSection() const;
    std::vector<uint8_t> buildDataCountSection() const;

    void writeSection(std::vector<uint8_t>& output, SectionId id,
                      const std::vector<uint8_t>& content) const;

    uint32_t findOrAddType(const WasmFuncType& type);
};

// ════════════════════════════════════════════════════════════════════════════════
//  Code Builder — مُنشئ الكود (مساعد لبناء أكواد العمليات)
// ════════════════════════════════════════════════════════════════════════════════

/**
 * مُنشئ أكواد العمليات لدالة WASM
 * يُسهّل بناء سلسلة التعليمات
 */
class WasmCodeBuilder {
public:
    WasmCodeBuilder() = default;

    // ─── الثوابت ───
    WasmCodeBuilder& i32Const(int32_t value);
    WasmCodeBuilder& i64Const(int64_t value);
    WasmCodeBuilder& f64Const(double value);
    WasmCodeBuilder& f32Const(float value);

    // ─── المتغيرات المحلية ───
    WasmCodeBuilder& localGet(uint32_t index);
    WasmCodeBuilder& localSet(uint32_t index);
    WasmCodeBuilder& localTee(uint32_t index);

    // ─── المتغيرات العامة ───
    WasmCodeBuilder& globalGet(uint32_t index);
    WasmCodeBuilder& globalSet(uint32_t index);

    // ─── الحسابية i32 ───
    WasmCodeBuilder& i32Add();
    WasmCodeBuilder& i32Sub();
    WasmCodeBuilder& i32Mul();
    WasmCodeBuilder& i32DivS();
    WasmCodeBuilder& i32RemS();
    WasmCodeBuilder& i32And();
    WasmCodeBuilder& i32Or();
    WasmCodeBuilder& i32Xor();
    WasmCodeBuilder& i32Shl();
    WasmCodeBuilder& i32ShrS();

    // ─── الحسابية f64 ───
    WasmCodeBuilder& f64Add();
    WasmCodeBuilder& f64Sub();
    WasmCodeBuilder& f64Mul();
    WasmCodeBuilder& f64Div();
    WasmCodeBuilder& f64Neg();
    WasmCodeBuilder& f64Abs();
    WasmCodeBuilder& f64Sqrt();
    WasmCodeBuilder& f64Floor();
    WasmCodeBuilder& f64Ceil();

    // ─── المقارنة i32 ───
    WasmCodeBuilder& i32Eqz();
    WasmCodeBuilder& i32Eq();
    WasmCodeBuilder& i32Ne();
    WasmCodeBuilder& i32LtS();
    WasmCodeBuilder& i32GtS();
    WasmCodeBuilder& i32LeS();
    WasmCodeBuilder& i32GeS();

    // ─── المقارنة f64 ───
    WasmCodeBuilder& f64Eq();
    WasmCodeBuilder& f64Ne();
    WasmCodeBuilder& f64Lt();
    WasmCodeBuilder& f64Gt();
    WasmCodeBuilder& f64Le();
    WasmCodeBuilder& f64Ge();

    // ─── التحويل ───
    WasmCodeBuilder& i32WrapI64();
    WasmCodeBuilder& i32TruncF64S();
    WasmCodeBuilder& i64ExtendI32S();
    WasmCodeBuilder& f64ConvertI32S();

    // ─── الذاكرة ───
    WasmCodeBuilder& i32Load(uint32_t align = 2, uint32_t offset = 0);
    WasmCodeBuilder& i32Store(uint32_t align = 2, uint32_t offset = 0);
    WasmCodeBuilder& f64Load(uint32_t align = 3, uint32_t offset = 0);
    WasmCodeBuilder& f64Store(uint32_t align = 3, uint32_t offset = 0);
    WasmCodeBuilder& i32Load8U(uint32_t align = 0, uint32_t offset = 0);
    WasmCodeBuilder& i32Store8(uint32_t align = 0, uint32_t offset = 0);
    WasmCodeBuilder& memorySize();
    WasmCodeBuilder& memoryGrow();

    // ─── التحكم ───
    WasmCodeBuilder& block(uint8_t blockType = Op::BlockTypeVoid);
    WasmCodeBuilder& loop(uint8_t blockType = Op::BlockTypeVoid);
    WasmCodeBuilder& ifBlock(uint8_t blockType = Op::BlockTypeVoid);
    WasmCodeBuilder& elseBlock();
    WasmCodeBuilder& end();
    WasmCodeBuilder& br(uint32_t depth);
    WasmCodeBuilder& brIf(uint32_t depth);
    WasmCodeBuilder& ret();

    // ─── الاستدعاء ───
    WasmCodeBuilder& call(uint32_t funcIndex);
    WasmCodeBuilder& callIndirect(uint32_t typeIndex, uint32_t tableIndex = 0);

    // ─── أخرى ───
    WasmCodeBuilder& drop();
    WasmCodeBuilder& select();
    WasmCodeBuilder& nop();
    WasmCodeBuilder& unreachable();

    // ─── الحصول على الكود ───
    std::vector<uint8_t> build() const;
    const std::vector<uint8_t>& code() const { return code_; }

    // ─── المتغيرات المحلية ───
    void addLocal(ValType type, uint32_t count = 1);
    std::vector<WasmLocal> getLocals() const { return locals_; }

    // ─── بناء جسم الدالة ───
    WasmFuncBody buildBody() const;

private:
    std::vector<uint8_t> code_;
    std::vector<WasmLocal> locals_;

    void emitMemArg(uint32_t align, uint32_t offset);
};

} // namespace WasmDirect
} // namespace Compiler
} // namespace Sad
