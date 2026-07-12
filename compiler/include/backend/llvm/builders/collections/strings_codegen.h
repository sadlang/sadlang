/*
 * ============================================================================
 * StringsCodeGen - LLVM IR for string ops + helpers
 * Phase 8 Step 8 - 5 methods (from 3 source files)
 * ============================================================================
 */
#ifndef SAD_LLVM_STRINGS_CODEGEN_H
#define SAD_LLVM_STRINGS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class StringsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit StringsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    StringsCodeGen(const StringsCodeGen &) = delete;
    StringsCodeGen &operator=(const StringsCodeGen &) = delete;

    llvm::Value *emitInlineAsm(std::shared_ptr<SIRInstruction> inst);
    void ensureArrayToStringHelper();
    // (AR) نظير نصّيّ: يبني «[س0، س1، ...]» بعناصر نصّيّة (%s). يخصّص مخزنه داخليًّا
    //      (طول متغيّر) ويُعيده — المستدعي يحرّره. / (EN) String variant: builds "[s0, s1, ...]"
    //      with string elements (%s); mallocs its own buffer (variable length) and returns it —
    //      the caller frees it.
    void ensureArrayToStringStrHelper();
    // (AR) نظير عشريّ: يبني «[س0, س1, ...]» حيث كلّ خانة تحمل بتّات double
    //      (bitcast(double→i64) عند التخزين). يفكّها bitcast⇒__sad_format_double
    //      (أو __sad_ftoa حرًّا) ⇒ يطابق تمثيل المفسّر. يخصّص مخزنه ويُعيده —
    //      المستدعي يحرّره. / (EN) Float variant: builds "[x0, x1, ...]" where each
    //      slot holds double bits (bitcast(double→i64) at store); decoded via
    //      bitcast⇒__sad_format_double (or __sad_ftoa freestanding). Mallocs its own
    //      buffer and returns it — the caller frees it.
    void ensureArrayToStringFloatHelper();
    // (AR) يبني «{"م0": ق0، "م1": ق1، …}» من خريطة (مفاتيح مقتبسة، قيم حسب النوع:
    //      نصّ %s، رقم %lld، منطقيّ صحيح/خطأ). يخصّص مخزنه ويُعيده — المستدعي يحرّره.
    // (EN) Builds "{"k0": v0, …}" from a map (quoted keys; values by type tag: string
    //      %s, int %lld, bool صحيح/خطأ). Mallocs its own buffer and returns it — caller frees.
    void ensureMapToStringHelper();
    llvm::Value *emitBuiltinTypeOf(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinArrayAppend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinArrayRemove(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
