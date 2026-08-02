/*
 * ============================================================================
 * ExceptionCodeGen — توليد LLVM IR لاستثناءات try/catch/throw + setjmp/longjmp
 * Phase 7 Step 10 — 2 methods (emitCallException + markSetjmpGlobalsVolatile)
 * ============================================================================
 */
#ifndef SAD_LLVM_EXCEPTION_CODEGEN_H
#define SAD_LLVM_EXCEPTION_CODEGEN_H

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class ExceptionCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit ExceptionCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    ExceptionCodeGen(const ExceptionCodeGen &) = delete;
    ExceptionCodeGen &operator=(const ExceptionCodeGen &) = delete;

    std::optional<llvm::Value *> emitCallException(const std::string &funcName,
                                                   std::vector<llvm::Value *> &args,
                                                   std::shared_ptr<SIRInstruction> inst);
    void markSetjmpGlobalsVolatile();

private:
    // (AR) يبعثُ كتلةَ «استثناءٌ لم يلتقطه أحد»: يقرأ النوعَ والرسالةَ والحمولةَ من
    //      عالميّاتِ الاستثناء (المخزَّنةِ عند الرمي) ويُبلِّغ بها ثمّ يخرج نظيفًا —
    //      مستضاف ⇒ sad_report_unhandled_exception؛ حرّ ⇒ __sad_panic برمزٍ مميَّز.
    //      ينتهي دائمًا بـunreachable. مُستخرَجٌ كي يتشارك مسارا __sad_raise (لا
    //      معالِجَ أصلًا) و__sad_raise_current (نفد المكدّسُ بعد تشغيل «أجّل») تشخيصًا
    //      واحدًا؛ كان الثاني بلا حارسٍ ⇒ فهرسٌ ‎-1‎ ⇒ longjmp إلى قمامة ⇒ SIGSEGV.
    // (EN) Emits the «nobody caught this exception» block: reads type/message/payload
    //      from the exception globals (stored at raise time), reports them, then exits
    //      cleanly — hosted ⇒ sad_report_unhandled_exception; freestanding ⇒ __sad_panic
    //      with a distinct code. Always terminated by unreachable. Extracted so both
    //      __sad_raise (no handler at all) and __sad_raise_current (the stack ran out
    //      after running defers) share one diagnostic; the latter had no guard ⇒ index
    //      -1 ⇒ longjmp to garbage ⇒ SIGSEGV.
    void emitUnhandledExceptionReport(const char *label);
};

}} // namespace Sad::LLVM
#endif
