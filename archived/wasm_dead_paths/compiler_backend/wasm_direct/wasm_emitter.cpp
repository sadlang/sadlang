/**
 * @file wasm_emitter.cpp
 * @brief (AR) تطبيق المُصدر الثنائي المباشر لـ WebAssembly
 * @brief (EN) Direct WebAssembly binary emitter implementation
 *
 * يولد ملفات .wasm صالحة بصيغة WebAssembly 1.0 الثنائية
 * بدون أي اعتماد على LLVM أو Emscripten
 */

#include "wasm_emitter.h"
#include <algorithm>
#include <cassert>
#include <cstring>

namespace Sad {
namespace Compiler {
namespace WasmDirect {

// ════════════════════════════════════════════════════════════════════════════════
//  WasmEmitter Implementation
// ════════════════════════════════════════════════════════════════════════════════

WasmEmitter::WasmEmitter() = default;

uint32_t WasmEmitter::findOrAddType(const WasmFuncType& type) {
    for (uint32_t i = 0; i < types_.size(); ++i) {
        if (types_[i] == type) return i;
    }
    types_.push_back(type);
    return static_cast<uint32_t>(types_.size() - 1);
}

uint32_t WasmEmitter::addFuncType(const WasmFuncType& type) {
    return findOrAddType(type);
}

uint32_t WasmEmitter::addImport(const std::string& module, const std::string& field,
                                 const WasmFuncType& type) {
    uint32_t typeIdx = findOrAddType(type);
    imports_.push_back({module, field, ImportKind::Function, typeIdx});
    return static_cast<uint32_t>(imports_.size() - 1);
}

uint32_t WasmEmitter::addFunction(uint32_t typeIndex) {
    functionTypes_.push_back(typeIndex);
    functionBodies_.emplace_back();
    return static_cast<uint32_t>(imports_.size() + functionTypes_.size() - 1);
}

uint32_t WasmEmitter::addFunction(const WasmFuncType& type) {
    uint32_t typeIdx = findOrAddType(type);
    return addFunction(typeIdx);
}

void WasmEmitter::setFunctionBody(uint32_t funcIndex, const WasmFuncBody& body) {
    uint32_t localIdx = funcIndex - static_cast<uint32_t>(imports_.size());
    if (localIdx < functionBodies_.size()) {
        functionBodies_[localIdx] = body;
    }
}

void WasmEmitter::addExport(const std::string& name, ExportKind kind, uint32_t index) {
    exports_.push_back({name, kind, index});
}

void WasmEmitter::exportFunction(const std::string& name, uint32_t funcIndex) {
    addExport(name, ExportKind::Function, funcIndex);
}

void WasmEmitter::exportMemory(const std::string& name, uint32_t memIndex) {
    addExport(name, ExportKind::Memory, memIndex);
}

void WasmEmitter::setMemory(uint32_t initialPages, uint32_t maxPages) {
    memoryInitial_ = initialPages;
    memoryMax_ = maxPages;
    hasMemory_ = true;
}

uint32_t WasmEmitter::addGlobal(ValType type, bool mutable_, int32_t initValue) {
    WasmGlobal g;
    g.type = type;
    g.mutable_ = mutable_;
    // تعبير التهيئة: i32.const value, end
    g.initExpr.push_back(Op::I32Const);
    encodeLEB128S(g.initExpr, initValue);
    g.initExpr.push_back(Op::End);
    globals_.push_back(g);
    return static_cast<uint32_t>(globals_.size() - 1);
}

uint32_t WasmEmitter::addGlobalF64(ValType type, bool mutable_, double initValue) {
    WasmGlobal g;
    g.type = type;
    g.mutable_ = mutable_;
    g.initExpr.push_back(Op::F64Const);
    encodeF64(g.initExpr, initValue);
    g.initExpr.push_back(Op::End);
    globals_.push_back(g);
    return static_cast<uint32_t>(globals_.size() - 1);
}

uint32_t WasmEmitter::addDataSegment(uint32_t offset, const std::vector<uint8_t>& data) {
    dataSegments_.push_back({0, offset, data});
    uint32_t startAddr = offset;
    uint32_t endAddr = offset + static_cast<uint32_t>(data.size());
    if (endAddr > dataOffset_) {
        // محاذاة على 8 بايت
        dataOffset_ = (endAddr + 7) & ~7u;
    }
    return startAddr;
}

uint32_t WasmEmitter::addDataSegment(uint32_t offset, const std::string& str) {
    std::vector<uint8_t> data(str.begin(), str.end());
    data.push_back(0); // null terminator
    return addDataSegment(offset, data);
}

void WasmEmitter::setTable(uint32_t initialSize, uint32_t maxSize) {
    hasTable_ = true;
    tableInitial_ = initialSize;
    tableMax_ = maxSize;
}

void WasmEmitter::setStartFunction(uint32_t funcIndex) {
    hasStart_ = true;
    startFunc_ = funcIndex;
}

uint32_t WasmEmitter::getFunctionCount() const {
    return static_cast<uint32_t>(functionTypes_.size());
}

uint32_t WasmEmitter::getImportCount() const {
    return static_cast<uint32_t>(imports_.size());
}

uint32_t WasmEmitter::getTotalFunctionIndex() const {
    return static_cast<uint32_t>(imports_.size() + functionTypes_.size());
}

// ════════════════════════════════════════════════════════════════════════════════
//  بناء الأقسام
// ════════════════════════════════════════════════════════════════════════════════

void WasmEmitter::writeSection(std::vector<uint8_t>& output, SectionId id,
                                const std::vector<uint8_t>& content) const {
    if (content.empty()) return;
    output.push_back(static_cast<uint8_t>(id));
    encodeLEB128U(output, static_cast<uint32_t>(content.size()));
    output.insert(output.end(), content.begin(), content.end());
}

std::vector<uint8_t> WasmEmitter::buildTypeSection() const {
    if (types_.empty()) return {};
    std::vector<uint8_t> section;
    encodeLEB128U(section, static_cast<uint32_t>(types_.size()));
    for (const auto& type : types_) {
        section.push_back(FUNC_TYPE_MARKER);
        encodeLEB128U(section, static_cast<uint32_t>(type.params.size()));
        for (auto p : type.params) section.push_back(static_cast<uint8_t>(p));
        encodeLEB128U(section, static_cast<uint32_t>(type.results.size()));
        for (auto r : type.results) section.push_back(static_cast<uint8_t>(r));
    }
    return section;
}

std::vector<uint8_t> WasmEmitter::buildImportSection() const {
    if (imports_.empty()) return {};
    std::vector<uint8_t> section;
    encodeLEB128U(section, static_cast<uint32_t>(imports_.size()));
    for (const auto& imp : imports_) {
        encodeString(section, imp.moduleName);
        encodeString(section, imp.fieldName);
        section.push_back(static_cast<uint8_t>(imp.kind));
        encodeLEB128U(section, imp.typeIndex);
    }
    return section;
}

std::vector<uint8_t> WasmEmitter::buildFunctionSection() const {
    if (functionTypes_.empty()) return {};
    std::vector<uint8_t> section;
    encodeLEB128U(section, static_cast<uint32_t>(functionTypes_.size()));
    for (auto typeIdx : functionTypes_) {
        encodeLEB128U(section, typeIdx);
    }
    return section;
}

std::vector<uint8_t> WasmEmitter::buildTableSection() const {
    if (!hasTable_) return {};
    std::vector<uint8_t> section;
    encodeLEB128U(section, 1); // عدد الجداول = 1
    section.push_back(static_cast<uint8_t>(ValType::FuncRef));
    if (tableMax_ > 0) {
        section.push_back(0x01); // has max
        encodeLEB128U(section, tableInitial_);
        encodeLEB128U(section, tableMax_);
    } else {
        section.push_back(0x00); // no max
        encodeLEB128U(section, tableInitial_);
    }
    return section;
}

std::vector<uint8_t> WasmEmitter::buildMemorySection() const {
    if (!hasMemory_) return {};
    std::vector<uint8_t> section;
    encodeLEB128U(section, 1); // عدد الذواكر = 1
    if (memoryMax_ > 0) {
        section.push_back(0x01); // has max
        encodeLEB128U(section, memoryInitial_);
        encodeLEB128U(section, memoryMax_);
    } else {
        section.push_back(0x00); // no max
        encodeLEB128U(section, memoryInitial_);
    }
    return section;
}

std::vector<uint8_t> WasmEmitter::buildGlobalSection() const {
    if (globals_.empty()) return {};
    std::vector<uint8_t> section;
    encodeLEB128U(section, static_cast<uint32_t>(globals_.size()));
    for (const auto& g : globals_) {
        section.push_back(static_cast<uint8_t>(g.type));
        section.push_back(g.mutable_ ? 0x01 : 0x00);
        section.insert(section.end(), g.initExpr.begin(), g.initExpr.end());
    }
    return section;
}

std::vector<uint8_t> WasmEmitter::buildExportSection() const {
    if (exports_.empty()) return {};
    std::vector<uint8_t> section;
    encodeLEB128U(section, static_cast<uint32_t>(exports_.size()));
    for (const auto& exp : exports_) {
        encodeString(section, exp.name);
        section.push_back(static_cast<uint8_t>(exp.kind));
        encodeLEB128U(section, exp.index);
    }
    return section;
}

std::vector<uint8_t> WasmEmitter::buildStartSection() const {
    if (!hasStart_) return {};
    std::vector<uint8_t> section;
    encodeLEB128U(section, startFunc_);
    return section;
}

std::vector<uint8_t> WasmEmitter::buildCodeSection() const {
    if (functionBodies_.empty()) return {};
    std::vector<uint8_t> section;
    encodeLEB128U(section, static_cast<uint32_t>(functionBodies_.size()));
    
    for (const auto& body : functionBodies_) {
        // بناء محتوى الدالة
        std::vector<uint8_t> funcContent;
        
        // عدد مجموعات المتغيرات المحلية
        encodeLEB128U(funcContent, static_cast<uint32_t>(body.locals.size()));
        for (const auto& local : body.locals) {
            encodeLEB128U(funcContent, local.count);
            funcContent.push_back(static_cast<uint8_t>(local.type));
        }
        
        // كود الدالة
        funcContent.insert(funcContent.end(), body.code.begin(), body.code.end());
        
        // إنهاء الدالة
        funcContent.push_back(Op::End);
        
        // حجم الدالة + المحتوى
        encodeLEB128U(section, static_cast<uint32_t>(funcContent.size()));
        section.insert(section.end(), funcContent.begin(), funcContent.end());
    }
    return section;
}

std::vector<uint8_t> WasmEmitter::buildDataSection() const {
    if (dataSegments_.empty()) return {};
    std::vector<uint8_t> section;
    encodeLEB128U(section, static_cast<uint32_t>(dataSegments_.size()));
    for (const auto& seg : dataSegments_) {
        section.push_back(0x00); // active segment, memory 0
        // تعبير الإزاحة: i32.const offset, end
        section.push_back(Op::I32Const);
        encodeLEB128S(section, static_cast<int32_t>(seg.offset));
        section.push_back(Op::End);
        // البيانات
        encodeLEB128U(section, static_cast<uint32_t>(seg.data.size()));
        section.insert(section.end(), seg.data.begin(), seg.data.end());
    }
    return section;
}

std::vector<uint8_t> WasmEmitter::buildDataCountSection() const {
    if (dataSegments_.empty()) return {};
    std::vector<uint8_t> section;
    encodeLEB128U(section, static_cast<uint32_t>(dataSegments_.size()));
    return section;
}

// ════════════════════════════════════════════════════════════════════════════════
//  إصدار الوحدة الكاملة
// ════════════════════════════════════════════════════════════════════════════════

std::vector<uint8_t> WasmEmitter::emit() const {
    std::vector<uint8_t> output;
    output.reserve(4096);

    // Magic number + version
    output.insert(output.end(), WASM_MAGIC, WASM_MAGIC + 4);
    output.insert(output.end(), WASM_VERSION, WASM_VERSION + 4);

    // الأقسام بالترتيب المطلوب
    writeSection(output, SectionId::Type,     buildTypeSection());
    writeSection(output, SectionId::Import,   buildImportSection());
    writeSection(output, SectionId::Function, buildFunctionSection());
    writeSection(output, SectionId::Table,    buildTableSection());
    writeSection(output, SectionId::Memory,   buildMemorySection());
    writeSection(output, SectionId::Global,   buildGlobalSection());
    writeSection(output, SectionId::Export,   buildExportSection());
    writeSection(output, SectionId::Start,    buildStartSection());
    writeSection(output, SectionId::DataCount, buildDataCountSection());
    writeSection(output, SectionId::Code,     buildCodeSection());
    writeSection(output, SectionId::Data,     buildDataSection());

    return output;
}

bool WasmEmitter::emitToFile(const std::string& filePath) const {
    auto binary = emit();
    std::ofstream file(filePath, std::ios::binary);
    if (!file) return false;
    file.write(reinterpret_cast<const char*>(binary.data()), binary.size());
    return file.good();
}

// ════════════════════════════════════════════════════════════════════════════════
//  WasmCodeBuilder Implementation
// ════════════════════════════════════════════════════════════════════════════════

WasmCodeBuilder& WasmCodeBuilder::i32Const(int32_t value) {
    code_.push_back(Op::I32Const);
    encodeLEB128S(code_, value);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::i64Const(int64_t value) {
    code_.push_back(Op::I64Const);
    encodeLEB128S64(code_, value);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::f64Const(double value) {
    code_.push_back(Op::F64Const);
    encodeF64(code_, value);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::f32Const(float value) {
    code_.push_back(Op::F32Const);
    encodeF32(code_, value);
    return *this;
}

// ─── المتغيرات ───

WasmCodeBuilder& WasmCodeBuilder::localGet(uint32_t index) {
    code_.push_back(Op::LocalGet);
    encodeLEB128U(code_, index);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::localSet(uint32_t index) {
    code_.push_back(Op::LocalSet);
    encodeLEB128U(code_, index);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::localTee(uint32_t index) {
    code_.push_back(Op::LocalTee);
    encodeLEB128U(code_, index);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::globalGet(uint32_t index) {
    code_.push_back(Op::GlobalGet);
    encodeLEB128U(code_, index);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::globalSet(uint32_t index) {
    code_.push_back(Op::GlobalSet);
    encodeLEB128U(code_, index);
    return *this;
}

// ─── الحسابية i32 ───

WasmCodeBuilder& WasmCodeBuilder::i32Add() { code_.push_back(Op::I32Add); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32Sub() { code_.push_back(Op::I32Sub); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32Mul() { code_.push_back(Op::I32Mul); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32DivS() { code_.push_back(Op::I32DivS); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32RemS() { code_.push_back(Op::I32RemS); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32And() { code_.push_back(Op::I32And); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32Or() { code_.push_back(Op::I32Or); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32Xor() { code_.push_back(Op::I32Xor); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32Shl() { code_.push_back(Op::I32Shl); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32ShrS() { code_.push_back(Op::I32ShrS); return *this; }

// ─── الحسابية f64 ───

WasmCodeBuilder& WasmCodeBuilder::f64Add() { code_.push_back(Op::F64Add); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64Sub() { code_.push_back(Op::F64Sub); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64Mul() { code_.push_back(Op::F64Mul); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64Div() { code_.push_back(Op::F64Div); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64Neg() { code_.push_back(Op::F64Neg); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64Abs() { code_.push_back(Op::F64Abs); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64Sqrt() { code_.push_back(Op::F64Sqrt); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64Floor() { code_.push_back(Op::F64Floor); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64Ceil() { code_.push_back(Op::F64Ceil); return *this; }

// ─── المقارنة i32 ───

WasmCodeBuilder& WasmCodeBuilder::i32Eqz() { code_.push_back(Op::I32Eqz); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32Eq() { code_.push_back(Op::I32Eq); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32Ne() { code_.push_back(Op::I32Ne); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32LtS() { code_.push_back(Op::I32LtS); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32GtS() { code_.push_back(Op::I32GtS); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32LeS() { code_.push_back(Op::I32LeS); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32GeS() { code_.push_back(Op::I32GeS); return *this; }

// ─── المقارنة f64 ───

WasmCodeBuilder& WasmCodeBuilder::f64Eq() { code_.push_back(Op::F64Eq); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64Ne() { code_.push_back(Op::F64Ne); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64Lt() { code_.push_back(Op::F64Lt); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64Gt() { code_.push_back(Op::F64Gt); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64Le() { code_.push_back(Op::F64Le); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64Ge() { code_.push_back(Op::F64Ge); return *this; }

// ─── التحويل ───

WasmCodeBuilder& WasmCodeBuilder::i32WrapI64() { code_.push_back(Op::I32WrapI64); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i32TruncF64S() { code_.push_back(Op::I32TruncF64S); return *this; }
WasmCodeBuilder& WasmCodeBuilder::i64ExtendI32S() { code_.push_back(Op::I64ExtendI32S); return *this; }
WasmCodeBuilder& WasmCodeBuilder::f64ConvertI32S() { code_.push_back(Op::F64ConvertI32S); return *this; }

// ─── الذاكرة ───

void WasmCodeBuilder::emitMemArg(uint32_t align, uint32_t offset) {
    encodeLEB128U(code_, align);
    encodeLEB128U(code_, offset);
}

WasmCodeBuilder& WasmCodeBuilder::i32Load(uint32_t align, uint32_t offset) {
    code_.push_back(Op::I32Load); emitMemArg(align, offset); return *this;
}

WasmCodeBuilder& WasmCodeBuilder::i32Store(uint32_t align, uint32_t offset) {
    code_.push_back(Op::I32Store); emitMemArg(align, offset); return *this;
}

WasmCodeBuilder& WasmCodeBuilder::f64Load(uint32_t align, uint32_t offset) {
    code_.push_back(Op::F64Load); emitMemArg(align, offset); return *this;
}

WasmCodeBuilder& WasmCodeBuilder::f64Store(uint32_t align, uint32_t offset) {
    code_.push_back(Op::F64Store); emitMemArg(align, offset); return *this;
}

WasmCodeBuilder& WasmCodeBuilder::i32Load8U(uint32_t align, uint32_t offset) {
    code_.push_back(Op::I32Load8U); emitMemArg(align, offset); return *this;
}

WasmCodeBuilder& WasmCodeBuilder::i32Store8(uint32_t align, uint32_t offset) {
    code_.push_back(Op::I32Store8); emitMemArg(align, offset); return *this;
}

WasmCodeBuilder& WasmCodeBuilder::memorySize() {
    code_.push_back(Op::MemorySize);
    code_.push_back(0x00); // memory index
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::memoryGrow() {
    code_.push_back(Op::MemoryGrow);
    code_.push_back(0x00); // memory index
    return *this;
}

// ─── التحكم ───

WasmCodeBuilder& WasmCodeBuilder::block(uint8_t blockType) {
    code_.push_back(Op::Block);
    code_.push_back(blockType);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::loop(uint8_t blockType) {
    code_.push_back(Op::Loop);
    code_.push_back(blockType);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::ifBlock(uint8_t blockType) {
    code_.push_back(Op::If);
    code_.push_back(blockType);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::elseBlock() {
    code_.push_back(Op::Else);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::end() {
    code_.push_back(Op::End);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::br(uint32_t depth) {
    code_.push_back(Op::Br);
    encodeLEB128U(code_, depth);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::brIf(uint32_t depth) {
    code_.push_back(Op::BrIf);
    encodeLEB128U(code_, depth);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::ret() {
    code_.push_back(Op::Return);
    return *this;
}

// ─── الاستدعاء ───

WasmCodeBuilder& WasmCodeBuilder::call(uint32_t funcIndex) {
    code_.push_back(Op::Call);
    encodeLEB128U(code_, funcIndex);
    return *this;
}

WasmCodeBuilder& WasmCodeBuilder::callIndirect(uint32_t typeIndex, uint32_t tableIndex) {
    code_.push_back(Op::CallIndirect);
    encodeLEB128U(code_, typeIndex);
    encodeLEB128U(code_, tableIndex);
    return *this;
}

// ─── أخرى ───

WasmCodeBuilder& WasmCodeBuilder::drop() { code_.push_back(Op::Drop); return *this; }
WasmCodeBuilder& WasmCodeBuilder::select() { code_.push_back(Op::Select); return *this; }
WasmCodeBuilder& WasmCodeBuilder::nop() { code_.push_back(Op::Nop); return *this; }
WasmCodeBuilder& WasmCodeBuilder::unreachable() { code_.push_back(Op::Unreachable); return *this; }

// ─── المتغيرات المحلية ───

void WasmCodeBuilder::addLocal(ValType type, uint32_t count) {
    // دمج مع المجموعة الأخيرة إذا كانت من نفس النوع
    if (!locals_.empty() && locals_.back().type == type) {
        locals_.back().count += count;
    } else {
        locals_.push_back({count, type});
    }
}

std::vector<uint8_t> WasmCodeBuilder::build() const {
    return code_;
}

WasmFuncBody WasmCodeBuilder::buildBody() const {
    return {locals_, code_};
}

} // namespace WasmDirect
} // namespace Compiler
} // namespace Sad
