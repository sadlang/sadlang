/**
 * =============================================================================
 * ملف: wasm.cpp
 * الوصف: Backend لـ WebAssembly
 * المهمة: T239 - WebAssembly backend
 * المرحلة: Phase 24 - User Story 21 (Cross-Platform Build --كل-الأهداف)
 * =============================================================================
 * 
 * 🌐 دليل المبتدئ لـ WebAssembly
 * ═══════════════════════════════
 * 
 * ما هو WebAssembly (Wasm)؟
 * ──────────────────────────
 * صيغة تعليمات ثنائية تعمل في المتصفح.
 * تتيح تشغيل كود سريع داخل صفحات الويب.
 * 
 * لماذا Wasm مهم؟
 * ────────────────
 * ✓ سرعة قريبة من الكود الأصلي
 * ✓ يعمل في جميع المتصفحات الحديثة
 * ✓ آمن (sandbox)
 * ✓ محمول عبر المنصات
 * 
 * مفاهيم Wasm الأساسية:
 * ───────────────────────
 * 
 * 1. أنواع القيم:
 *    i32  - عدد صحيح 32-bit
 *    i64  - عدد صحيح 64-bit
 *    f32  - عدد عشري 32-bit
 *    f64  - عدد عشري 64-bit
 * 
 * 2. المكدس (Stack Machine):
 *    Wasm يعمل بنموذج المكدس:
 *    - كل عملية تأخذ من المكدس
 *    - وتضع النتيجة على المكدس
 * 
 * 3. الدوال والاستيراد:
 *    - تعريف دوال داخلية
 *    - استيراد دوال من JavaScript
 * 
 * 4. الذاكرة الخطية:
 *    - مصفوفة bytes متصلة
 *    - يمكن تكبيرها أثناء التشغيل
 * 
 * مثال Wasm Text (WAT):
 * ─────────────────────
 * ```wat
 * (module
 *   (func $add (param $a i32) (param $b i32) (result i32)
 *     local.get $a
 *     local.get $b
 *     i32.add
 *   )
 *   (export "add" (func $add))
 * )
 * ```
 * 
 * أدوات التجميع:
 * ─────────────────
 * - Emscripten: تحويل C/C++ لـ Wasm
 * - wasm-ld: ربط ملفات Wasm
 * - wasm2wat / wat2wasm: تحويل بين الصيغ
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <cstdint>

namespace sad::targets {

// =============================================================================
// أنواع WebAssembly
// =============================================================================

enum class WasmType {
    I32,     // عدد صحيح 32-bit
    I64,     // عدد صحيح 64-bit
    F32,     // عشري 32-bit
    F64,     // عشري 64-bit
    V128,    // SIMD vector
    FuncRef, // مرجع دالة
    ExternRef // مرجع خارجي
};

// =============================================================================
// تعليمات WebAssembly
// =============================================================================

enum class WasmOpcode : uint8_t {
    // التحكم
    Unreachable = 0x00,
    Nop = 0x01,
    Block = 0x02,
    Loop = 0x03,
    If = 0x04,
    Else = 0x05,
    End = 0x0B,
    Br = 0x0C,
    BrIf = 0x0D,
    BrTable = 0x0E,
    Return = 0x0F,
    Call = 0x10,
    CallIndirect = 0x11,
    
    // المكدس
    Drop = 0x1A,
    Select = 0x1B,
    
    // المتغيرات المحلية
    LocalGet = 0x20,
    LocalSet = 0x21,
    LocalTee = 0x22,
    GlobalGet = 0x23,
    GlobalSet = 0x24,
    
    // الذاكرة
    I32Load = 0x28,
    I64Load = 0x29,
    F32Load = 0x2A,
    F64Load = 0x2B,
    I32Store = 0x36,
    I64Store = 0x37,
    F32Store = 0x38,
    F64Store = 0x39,
    MemorySize = 0x3F,
    MemoryGrow = 0x40,
    
    // الثوابت
    I32Const = 0x41,
    I64Const = 0x42,
    F32Const = 0x43,
    F64Const = 0x44,
    
    // المقارنة i32
    I32Eqz = 0x45,
    I32Eq = 0x46,
    I32Ne = 0x47,
    I32LtS = 0x48,
    I32LtU = 0x49,
    I32GtS = 0x4A,
    I32GtU = 0x4B,
    I32LeS = 0x4C,
    I32LeU = 0x4D,
    I32GeS = 0x4E,
    I32GeU = 0x4F,
    
    // الحساب i32
    I32Add = 0x6A,
    I32Sub = 0x6B,
    I32Mul = 0x6C,
    I32DivS = 0x6D,
    I32DivU = 0x6E,
    I32RemS = 0x6F,
    I32RemU = 0x70,
    I32And = 0x71,
    I32Or = 0x72,
    I32Xor = 0x73,
    I32Shl = 0x74,
    I32ShrS = 0x75,
    I32ShrU = 0x76,
    
    // الحساب i64
    I64Add = 0x7C,
    I64Sub = 0x7D,
    I64Mul = 0x7E,
    I64DivS = 0x7F,
    I64DivU = 0x80,
    
    // الحساب f32
    F32Abs = 0x8B,
    F32Neg = 0x8C,
    F32Sqrt = 0x91,
    F32Add = 0x92,
    F32Sub = 0x93,
    F32Mul = 0x94,
    F32Div = 0x95,
    
    // الحساب f64
    F64Abs = 0x99,
    F64Neg = 0x9A,
    F64Sqrt = 0x9F,
    F64Add = 0xA0,
    F64Sub = 0xA1,
    F64Mul = 0xA2,
    F64Div = 0xA3,
    
    // التحويل
    I32WrapI64 = 0xA7,
    I64ExtendI32S = 0xAC,
    I64ExtendI32U = 0xAD,
    F32ConvertI32S = 0xB2,
    F64ConvertI32S = 0xB7
};

// =============================================================================
// تعليمة Wasm
// =============================================================================

struct WasmInstruction {
    WasmOpcode opcode;
    
    // للثوابت
    int32_t i32_val;
    int64_t i64_val;
    float f32_val;
    double f64_val;
    
    // للمتغيرات والقفز
    uint32_t index;
    
    // للذاكرة
    uint32_t align;
    uint32_t offset;
    
    // تعليق
    std::string comment;
    
    WasmInstruction() : opcode(WasmOpcode::Nop), i32_val(0), index(0), align(0), offset(0) {}
    
    static WasmInstruction Const(int32_t val) {
        WasmInstruction inst;
        inst.opcode = WasmOpcode::I32Const;
        inst.i32_val = val;
        return inst;
    }
    
    static WasmInstruction LocalGet(uint32_t idx) {
        WasmInstruction inst;
        inst.opcode = WasmOpcode::LocalGet;
        inst.index = idx;
        return inst;
    }
    
    static WasmInstruction LocalSet(uint32_t idx) {
        WasmInstruction inst;
        inst.opcode = WasmOpcode::LocalSet;
        inst.index = idx;
        return inst;
    }
    
    /**
     * تحويل لنص WAT
     */
    std::string toWat() const;
};

// =============================================================================
// دالة Wasm
// =============================================================================

struct WasmFunction {
    std::string name;
    std::vector<WasmType> params;
    std::vector<WasmType> results;
    std::vector<WasmType> locals;
    std::vector<WasmInstruction> body;
    bool exported = false;
    
    /**
     * تحويل لنص WAT
     */
    std::string toWat() const;
};

// =============================================================================
// استيراد/تصدير
// =============================================================================

struct WasmImport {
    std::string module;
    std::string name;
    std::string internalName;
    
    enum Type { Function, Memory, Table, Global } type;
    
    // للدوال
    std::vector<WasmType> funcParams;
    std::vector<WasmType> funcResults;
    
    // للذاكرة
    uint32_t memoryMin = 0;
    uint32_t memoryMax = 0;
};

struct WasmExport {
    std::string name;
    std::string internalName;
    WasmImport::Type type;
};

// =============================================================================
// مُولد Wasm
// =============================================================================

class WasmCodeGen {
public:
    WasmCodeGen() {
        // ذاكرة افتراضية
        memoryPages_ = 1;
    }
    
    /**
     * بدء دالة جديدة
     */
    void beginFunction(const std::string& name,
                       const std::vector<WasmType>& params,
                       const std::vector<WasmType>& results,
                       bool exported = true) {
        WasmFunction func;
        func.name = name;
        func.params = params;
        func.results = results;
        func.exported = exported;
        currentFunc_ = func;
    }
    
    /**
     * إضافة متغير محلي
     */
    uint32_t addLocal(WasmType type) {
        currentFunc_.locals.push_back(type);
        return currentFunc_.params.size() + currentFunc_.locals.size() - 1;
    }
    
    /**
     * إصدار تعليمة
     */
    void emit(WasmOpcode op) {
        WasmInstruction inst;
        inst.opcode = op;
        currentFunc_.body.push_back(inst);
    }
    
    /**
     * إصدار ثابت i32
     */
    void emitI32Const(int32_t val) {
        currentFunc_.body.push_back(WasmInstruction::Const(val));
    }
    
    /**
     * تحميل متغير محلي
     */
    void emitLocalGet(uint32_t idx) {
        currentFunc_.body.push_back(WasmInstruction::LocalGet(idx));
    }
    
    /**
     * تخزين في متغير محلي
     */
    void emitLocalSet(uint32_t idx) {
        currentFunc_.body.push_back(WasmInstruction::LocalSet(idx));
    }
    
    /**
     * جمع i32
     */
    void emitI32Add() {
        emit(WasmOpcode::I32Add);
    }
    
    /**
     * طرح i32
     */
    void emitI32Sub() {
        emit(WasmOpcode::I32Sub);
    }
    
    /**
     * ضرب i32
     */
    void emitI32Mul() {
        emit(WasmOpcode::I32Mul);
    }
    
    /**
     * مقارنة ==
     */
    void emitI32Eq() {
        emit(WasmOpcode::I32Eq);
    }
    
    /**
     * مقارنة <
     */
    void emitI32Lt() {
        emit(WasmOpcode::I32LtS);
    }
    
    /**
     * عودة
     */
    void emitReturn() {
        emit(WasmOpcode::Return);
    }
    
    /**
     * استدعاء دالة
     */
    void emitCall(uint32_t funcIdx) {
        WasmInstruction inst;
        inst.opcode = WasmOpcode::Call;
        inst.index = funcIdx;
        currentFunc_.body.push_back(inst);
    }
    
    /**
     * بداية if
     */
    void emitIf(WasmType resultType = WasmType::I32) {
        emit(WasmOpcode::If);
    }
    
    /**
     * else
     */
    void emitElse() {
        emit(WasmOpcode::Else);
    }
    
    /**
     * نهاية block
     */
    void emitEnd() {
        emit(WasmOpcode::End);
    }
    
    /**
     * حلقة
     */
    void emitLoop() {
        emit(WasmOpcode::Loop);
    }
    
    /**
     * قفز
     */
    void emitBr(uint32_t depth) {
        WasmInstruction inst;
        inst.opcode = WasmOpcode::Br;
        inst.index = depth;
        currentFunc_.body.push_back(inst);
    }
    
    /**
     * قفز شرطي
     */
    void emitBrIf(uint32_t depth) {
        WasmInstruction inst;
        inst.opcode = WasmOpcode::BrIf;
        inst.index = depth;
        currentFunc_.body.push_back(inst);
    }
    
    /**
     * إنهاء الدالة
     */
    void endFunction() {
        functions_.push_back(currentFunc_);
        currentFunc_ = WasmFunction();
    }
    
    /**
     * إضافة استيراد
     */
    void addImport(const WasmImport& imp) {
        imports_.push_back(imp);
    }
    
    /**
     * إضافة استيراد console.log
     */
    void importConsoleLog() {
        WasmImport imp;
        imp.module = "console";
        imp.name = "log";
        imp.internalName = "$console_log";
        imp.type = WasmImport::Function;
        imp.funcParams = {WasmType::I32};
        imports_.push_back(imp);
    }
    
    /**
     * توليد WAT (نص)
     */
    std::string generateWat() const {
        std::ostringstream ss;
        
        ss << ";; مُولد من لغة ص للويب\n";
        ss << ";; WebAssembly Text Format (WAT)\n\n";
        
        ss << "(module\n";
        
        // الاستيرادات
        for (const auto& imp : imports_) {
            ss << "  (import \"" << imp.module << "\" \"" << imp.name << "\"\n";
            if (imp.type == WasmImport::Function) {
                ss << "    (func " << imp.internalName << " ";
                if (!imp.funcParams.empty()) {
                    ss << "(param";
                    for (auto t : imp.funcParams) {
                        ss << " " << typeToString(t);
                    }
                    ss << ")";
                }
                if (!imp.funcResults.empty()) {
                    ss << " (result";
                    for (auto t : imp.funcResults) {
                        ss << " " << typeToString(t);
                    }
                    ss << ")";
                }
                ss << ")";
            } else if (imp.type == WasmImport::Memory) {
                ss << "    (memory " << imp.memoryMin << ")";
            }
            ss << ")\n";
        }
        
        // الذاكرة
        if (imports_.empty() || std::none_of(imports_.begin(), imports_.end(),
            [](const WasmImport& i) { return i.type == WasmImport::Memory; })) {
            ss << "  (memory (export \"memory\") " << memoryPages_ << ")\n";
        }
        
        ss << "\n";
        
        // الدوال
        for (const auto& func : functions_) {
            ss << func.toWat() << "\n";
        }
        
        ss << ")\n";
        
        return ss.str();
    }
    
    /**
     * توليد كود ثنائي
     */
    std::vector<uint8_t> generateBinary() const {
        std::vector<uint8_t> binary;
        
        // Magic number
        binary.push_back(0x00);
        binary.push_back(0x61); // 'a'
        binary.push_back(0x73); // 's'
        binary.push_back(0x6D); // 'm'
        
        // Version
        binary.push_back(0x01);
        binary.push_back(0x00);
        binary.push_back(0x00);
        binary.push_back(0x00);
        
        // باقي الأقسام (مبسط)
        // ...
        
        return binary;
    }
    
private:
    std::vector<WasmFunction> functions_;
    std::vector<WasmImport> imports_;
    WasmFunction currentFunc_;
    uint32_t memoryPages_;
    
    static std::string typeToString(WasmType t) {
        switch (t) {
            case WasmType::I32: return "i32";
            case WasmType::I64: return "i64";
            case WasmType::F32: return "f32";
            case WasmType::F64: return "f64";
            default: return "i32";
        }
    }
};

// =============================================================================
// تحويل لنص
// =============================================================================

std::string opcodeToWat(WasmOpcode op) {
    switch (op) {
        case WasmOpcode::Nop: return "nop";
        case WasmOpcode::Return: return "return";
        case WasmOpcode::Call: return "call";
        case WasmOpcode::LocalGet: return "local.get";
        case WasmOpcode::LocalSet: return "local.set";
        case WasmOpcode::I32Const: return "i32.const";
        case WasmOpcode::I32Add: return "i32.add";
        case WasmOpcode::I32Sub: return "i32.sub";
        case WasmOpcode::I32Mul: return "i32.mul";
        case WasmOpcode::I32Eq: return "i32.eq";
        case WasmOpcode::I32LtS: return "i32.lt_s";
        case WasmOpcode::If: return "if";
        case WasmOpcode::Else: return "else";
        case WasmOpcode::End: return "end";
        case WasmOpcode::Loop: return "loop";
        case WasmOpcode::Br: return "br";
        case WasmOpcode::BrIf: return "br_if";
        default: return "???";
    }
}

std::string WasmInstruction::toWat() const {
    std::ostringstream ss;
    ss << opcodeToWat(opcode);
    
    switch (opcode) {
        case WasmOpcode::I32Const:
            ss << " " << i32_val;
            break;
        case WasmOpcode::LocalGet:
        case WasmOpcode::LocalSet:
        case WasmOpcode::Call:
        case WasmOpcode::Br:
        case WasmOpcode::BrIf:
            ss << " " << index;
            break;
        default:
            break;
    }
    
    return ss.str();
}

std::string WasmFunction::toWat() const {
    std::ostringstream ss;
    
    ss << "  (func $" << name;
    
    // تصدير
    if (exported) {
        ss << " (export \"" << name << "\")";
    }
    
    // المعطيات
    for (size_t i = 0; i < params.size(); i++) {
        ss << " (param $p" << i << " ";
        switch (params[i]) {
            case WasmType::I32: ss << "i32"; break;
            case WasmType::I64: ss << "i64"; break;
            case WasmType::F32: ss << "f32"; break;
            case WasmType::F64: ss << "f64"; break;
            default: ss << "i32";
        }
        ss << ")";
    }
    
    // النتيجة
    if (!results.empty()) {
        ss << " (result";
        for (auto r : results) {
            ss << " ";
            switch (r) {
                case WasmType::I32: ss << "i32"; break;
                case WasmType::I64: ss << "i64"; break;
                case WasmType::F32: ss << "f32"; break;
                case WasmType::F64: ss << "f64"; break;
                default: ss << "i32";
            }
        }
        ss << ")";
    }
    
    ss << "\n";
    
    // المتغيرات المحلية
    for (size_t i = 0; i < locals.size(); i++) {
        ss << "    (local $l" << i << " ";
        switch (locals[i]) {
            case WasmType::I32: ss << "i32"; break;
            case WasmType::I64: ss << "i64"; break;
            default: ss << "i32";
        }
        ss << ")\n";
    }
    
    // الجسم
    for (const auto& inst : body) {
        ss << "    " << inst.toWat() << "\n";
    }
    
    ss << "  )";
    
    return ss.str();
}

} // namespace sad::targets

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadWasmCodeGen {
    sad::targets::WasmCodeGen* gen;
};

SadWasmCodeGen* sad_wasm_codegen_new() {
    auto* ctx = new SadWasmCodeGen();
    ctx->gen = new sad::targets::WasmCodeGen();
    return ctx;
}

void sad_wasm_begin_func(SadWasmCodeGen* ctx, const char* name,
                         int param_count, int result_count) {
    std::vector<sad::targets::WasmType> params(param_count, sad::targets::WasmType::I32);
    std::vector<sad::targets::WasmType> results(result_count, sad::targets::WasmType::I32);
    ctx->gen->beginFunction(name, params, results);
}

void sad_wasm_emit_i32_const(SadWasmCodeGen* ctx, int val) {
    ctx->gen->emitI32Const(val);
}

void sad_wasm_emit_local_get(SadWasmCodeGen* ctx, unsigned int idx) {
    ctx->gen->emitLocalGet(idx);
}

void sad_wasm_emit_i32_add(SadWasmCodeGen* ctx) {
    ctx->gen->emitI32Add();
}

void sad_wasm_emit_return(SadWasmCodeGen* ctx) {
    ctx->gen->emitReturn();
}

void sad_wasm_end_func(SadWasmCodeGen* ctx) {
    ctx->gen->endFunction();
}

const char* sad_wasm_get_wat(SadWasmCodeGen* ctx) {
    static std::string result;
    result = ctx->gen->generateWat();
    return result.c_str();
}

void sad_wasm_codegen_free(SadWasmCodeGen* ctx) {
    if (ctx) {
        delete ctx->gen;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef WASM_TEST

#include <cassert>

void testSimpleFunction() {
    std::cout << "🌐 اختبار دالة بسيطة...\n";
    
    sad::targets::WasmCodeGen gen;
    
    gen.beginFunction("add", 
                      {sad::targets::WasmType::I32, sad::targets::WasmType::I32},
                      {sad::targets::WasmType::I32});
    
    gen.emitLocalGet(0);
    gen.emitLocalGet(1);
    gen.emitI32Add();
    gen.emitReturn();
    gen.endFunction();
    
    std::string wat = gen.generateWat();
    
    assert(wat.find("func $add") != std::string::npos);
    assert(wat.find("local.get 0") != std::string::npos);
    assert(wat.find("i32.add") != std::string::npos);
    
    std::cout << "   ✅ نجح\n";
}

void testControlFlow() {
    std::cout << "🔀 اختبار التحكم...\n";
    
    sad::targets::WasmCodeGen gen;
    
    gen.beginFunction("max",
                      {sad::targets::WasmType::I32, sad::targets::WasmType::I32},
                      {sad::targets::WasmType::I32});
    
    gen.emitLocalGet(0);
    gen.emitLocalGet(1);
    gen.emitI32Lt();
    gen.emitIf();
    gen.emitLocalGet(1);
    gen.emitElse();
    gen.emitLocalGet(0);
    gen.emitEnd();
    gen.emitReturn();
    gen.endFunction();
    
    std::string wat = gen.generateWat();
    
    assert(wat.find("if") != std::string::npos);
    assert(wat.find("else") != std::string::npos);
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات Backend WebAssembly\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testSimpleFunction();
    testControlFlow();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // WASM_TEST
