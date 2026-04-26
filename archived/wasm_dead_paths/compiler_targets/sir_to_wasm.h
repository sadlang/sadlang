/**
 * =============================================================================
 * @file sir_to_wasm.h
 * @brief (AR) تحويل SIR إلى WASM مباشر
 * @brief (EN) Convert SIR (Sad IR) to WASM directly
 * 
 * المهمة: T241 - SIR to WASM Compiler
 * المرحلة: سبتمبر 2026 — WASM Direct Compilation
 * =============================================================================
 */

#ifndef SAD_SIR_TO_WASM_H
#define SAD_SIR_TO_WASM_H

#include "wasm_direct.h"
#include "../sir/sir_opcodes.h"
#include <memory>
#include <stack>

namespace sad::wasm {

/**
 * @brief تحويل SIR إلى WASM
 */
class SIRToWasm {
public:
    SIRToWasm() : gen_(std::make_unique<WasmDirectGenerator>()) {}
    
    // ─────────────────────────────────────────────────────────────────────
    // الواجهة الرئيسية
    // ─────────────────────────────────────────────────────────────────────
    
    /**
     * @brief تحويل وحدة SIR كاملة
     */
    bool compile(const std::vector<uint8_t>& sirBytecode) {
        // هنا سيتم تحليل SIR bytecode وتحويله
        // حالياً نوفر واجهة برمجية مباشرة
        return true;
    }
    
    /**
     * @brief بدء دالة
     */
    void beginFunction(const std::string& name,
                       const std::vector<ValType>& params,
                       const std::vector<ValType>& results,
                       bool exported = true) {
        gen_->beginFunction(name, params, results, exported);
        localCount_ = params.size();
        labelStack_.clear();
    }
    
    /**
     * @brief إضافة متغير محلي
     */
    uint32_t addLocal(ValType type) {
        localCount_++;
        return gen_->addLocal(type);
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // تحويل تعليمات SIR إلى WASM
    // ─────────────────────────────────────────────────────────────────────
    
    // ═══════════════════════════════════════════════════════════════
    // الثوابت
    // ═══════════════════════════════════════════════════════════════
    
    void emitLoadConst(int64_t value) {
        if (value >= INT32_MIN && value <= INT32_MAX) {
            gen_->emitI32Const(static_cast<int32_t>(value));
        } else {
            gen_->emitI64Const(value);
        }
    }
    
    void emitLoadConstF64(double value) {
        gen_->emitF64Const(value);
    }
    
    void emitLoadString(const std::string& str) {
        uint32_t addr = gen_->addString(str);
        gen_->emitI32Const(addr);
    }
    
    // ═══════════════════════════════════════════════════════════════
    // المتغيرات
    // ═══════════════════════════════════════════════════════════════
    
    void emitLoadLocal(uint32_t idx) {
        gen_->emitLocalGet(idx);
    }
    
    void emitStoreLocal(uint32_t idx) {
        gen_->emitLocalSet(idx);
    }
    
    void emitLoadGlobal(uint32_t idx) {
        gen_->emitGlobalGet(idx);
    }
    
    void emitStoreGlobal(uint32_t idx) {
        gen_->emitGlobalSet(idx);
    }
    
    // ═══════════════════════════════════════════════════════════════
    // العمليات الحسابية
    // ═══════════════════════════════════════════════════════════════
    
    void emitAdd(bool isFloat = false) {
        if (isFloat) gen_->emitF64Add();
        else gen_->emitI32Add();
    }
    
    void emitSub(bool isFloat = false) {
        if (isFloat) gen_->emitF64Sub();
        else gen_->emitI32Sub();
    }
    
    void emitMul(bool isFloat = false) {
        if (isFloat) gen_->emitF64Mul();
        else gen_->emitI32Mul();
    }
    
    void emitDiv(bool isFloat = false) {
        if (isFloat) gen_->emitF64Div();
        else gen_->emitI32DivS();
    }
    
    void emitMod() {
        gen_->emitI32RemS();
    }
    
    // ═══════════════════════════════════════════════════════════════
    // العمليات المنطقية
    // ═══════════════════════════════════════════════════════════════
    
    void emitAnd() { gen_->emitI32And(); }
    void emitOr() { gen_->emitI32Or(); }
    void emitXor() { gen_->emitI32Xor(); }
    
    void emitNot() {
        gen_->emitI32Eqz();
    }
    
    // ═══════════════════════════════════════════════════════════════
    // المقارنات
    // ═══════════════════════════════════════════════════════════════
    
    void emitEq() { gen_->emitI32Eq(); }
    void emitNe() { gen_->emitI32Ne(); }
    void emitLt() { gen_->emitI32LtS(); }
    void emitGt() { gen_->emitI32GtS(); }
    void emitLe() { gen_->emitI32LeS(); }
    void emitGe() { gen_->emitI32GeS(); }
    
    // ═══════════════════════════════════════════════════════════════
    // التحكم في التدفق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * @brief بداية if
     */
    void emitIf() {
        gen_->emitIf();
        labelStack_.push_back(LabelType::If);
    }
    
    /**
     * @brief else
     */
    void emitElse() {
        gen_->emitElse();
    }
    
    /**
     * @brief بداية حلقة
     */
    void emitLoop() {
        gen_->emitLoop();
        labelStack_.push_back(LabelType::Loop);
    }
    
    /**
     * @brief بداية block
     */
    void emitBlock() {
        gen_->emitBlock();
        labelStack_.push_back(LabelType::Block);
    }
    
    /**
     * @brief نهاية block/if/loop
     */
    void emitEnd() {
        gen_->emitEnd();
        if (!labelStack_.empty()) {
            labelStack_.pop_back();
        }
    }
    
    /**
     * @brief قفز (break)
     */
    void emitBr(uint32_t depth = 0) {
        gen_->emitBr(depth);
    }
    
    /**
     * @brief قفز شرطي
     */
    void emitBrIf(uint32_t depth = 0) {
        gen_->emitBrIf(depth);
    }
    
    /**
     * @brief عودة من الدالة
     */
    void emitReturn() {
        gen_->emitReturn();
    }
    
    // ═══════════════════════════════════════════════════════════════
    // استدعاء الدوال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * @brief استدعاء دالة
     */
    void emitCall(uint32_t funcIdx) {
        gen_->emitCall(funcIdx);
    }
    
    /**
     * @brief استدعاء دالة مدمجة (اطبع)
     */
    void emitCallPrint(bool isString = false) {
        if (isString) {
            // console_log_str(ptr, len)
            gen_->emitCall(1);  // index 1 = console_log_str
        } else {
            // console_log_i32(val)
            gen_->emitCall(0);  // index 0 = console_log_i32
        }
    }
    
    /**
     * @brief إنهاء الدالة
     */
    void endFunction() {
        gen_->endFunction();
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // التوليد النهائي
    // ─────────────────────────────────────────────────────────────────────
    
    std::vector<uint8_t> generate() {
        return gen_->generate();
    }
    
    bool saveWasm(const std::string& path) {
        return gen_->saveToFile(path);
    }
    
    std::string generateJsGlue() const {
        return gen_->generateJsGlue();
    }
    
    std::string generateHtml(const std::string& wasmFile) const {
        return gen_->generateHtml(wasmFile);
    }
    
    WasmDirectGenerator* getGenerator() { return gen_.get(); }

private:
    enum class LabelType { Block, Loop, If };
    
    std::unique_ptr<WasmDirectGenerator> gen_;
    size_t localCount_ = 0;
    std::vector<LabelType> labelStack_;
};

/**
 * @brief مُترجم AST إلى WASM مباشرة
 * 
 * يأخذ AST من لغة ص ويولد WASM بدون المرور بـ LLVM أو SIR
 */
class ASTToWasm {
public:
    ASTToWasm() : compiler_() {}
    
    /**
     * @brief ترجمة برنامج كامل
     */
    bool compileProgram() {
        // إنشاء دالة main
        compiler_.beginFunction("main", {}, {ValType::I32}, true);
        
        // سيتم استدعاء هذه الطريقة من الـ visitor
        
        // إرجاع 0 للنجاح
        compiler_.emitLoadConst(0);
        compiler_.emitReturn();
        compiler_.endFunction();
        
        return true;
    }
    
    /**
     * @brief حفظ الملفات
     */
    bool save(const std::string& basePath) {
        // حفظ .wasm
        if (!compiler_.saveWasm(basePath + ".wasm")) {
            return false;
        }
        
        // حفظ .js
        std::ofstream jsFile(basePath + ".js");
        if (jsFile) {
            jsFile << compiler_.generateJsGlue();
        }
        
        // حفظ .html
        std::ofstream htmlFile(basePath + ".html");
        if (htmlFile) {
            std::string wasmName = basePath.substr(basePath.find_last_of("/\\") + 1) + ".wasm";
            htmlFile << compiler_.generateHtml(wasmName);
        }
        
        return true;
    }
    
    SIRToWasm& getCompiler() { return compiler_; }
    
private:
    SIRToWasm compiler_;
};

} // namespace sad::wasm

#endif // SAD_SIR_TO_WASM_H
