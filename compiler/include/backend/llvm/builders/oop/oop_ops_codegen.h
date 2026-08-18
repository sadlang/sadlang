/*
 * ============================================================================
 * OOPOpsCodeGen - LLVM IR for OOP/pointer operations
 * Phase 7 Step 18 - 6 methods (Addr, PtrAdd, PtrCast, ObjectNew, ObjectGet, ObjectSet)
 * ============================================================================
 */
#ifndef SAD_LLVM_OOP_OPS_CODEGEN_H
#define SAD_LLVM_OOP_OPS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class OOPOpsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit OOPOpsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    OOPOpsCodeGen(const OOPOpsCodeGen &) = delete;
    OOPOpsCodeGen &operator=(const OOPOpsCodeGen &) = delete;

    llvm::Value *emitAddr(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitPtrAdd(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitPtrCast(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitObjectNew(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitObjectGet(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitObjectSet(std::shared_ptr<SIRInstruction> inst);

    /// (AR) خفضُ `OBJECT_NULL_CHECK`: يمرّ بالمُستقبِلِ على البابِ نفسِه الذي تمرّ
    ///      به القراءةُ والكتابة، بلفظِ العمليّةِ الذي تحمله التعليمةُ من موضعِ
    ///      النداء. ولا نتيجةَ له — حارسٌ محضٌ يُصدَر لأثرِه لا لقيمته.
    /// (EN) Lowering for OBJECT_NULL_CHECK: routes the receiver through the same
    ///      door as read and write, using the operation label the instruction
    ///      carries from the call site. Result-less: a pure guard.
    llvm::Value *emitObjectNullCheck(std::shared_ptr<SIRInstruction> inst);

private:
    // (AR) هل الحقل موجود في تخطيط أيّ صنف معروف (بما فيها المضمَّنة)؟ —
    //      يميّز عطب التخطيط المُجهِض عن الوصول الديناميكيّ (شأن زمن التشغيل)
    // (EN) Does the field exist in any known class layout (builtins included)?
    //      Distinguishes aborting layout defects from dynamic (runtime) access.
    bool fieldExistsInAnyClass(const std::string &fieldName) const;

    /// (AR) بابُ عائلةِ الكائنِ إلى `emitRaiseIfNull`: مُستقبِلٌ عدميٌّ ⇒ RUN033 كما
    ///      يفعل المفسّر، بدلَ انهيارِ تجزئةٍ برمزِ خروجٍ صفر. يُنادى **قبل** حلِّ
    ///      الصنف، لأنّ المُستقبِلَ العدميَّ لا صنفَ له فيرتدُّ المُصدِرُ قبلَ الوصولِ
    ///      إلى GEP — انظر التعليلَ المقيسَ عند التعريف.
    /// (EN) The object family's door into emitRaiseIfNull: a null receiver raises
    ///      RUN033 as the interpreter does, instead of segfaulting with exit code 0.
    ///      Called BEFORE class resolution — a null receiver has no class, so the
    ///      emitter bails out before the GEP. See the measured rationale at the
    ///      definition.
    ///     ولفظُ العمليّةِ معاملٌ لأنّ المرجعَ يفرّق: «member access» قراءةً و«.=»
    ///     كتابةً — والتوحيدُ يُنتِج رفعًا صحيحًا بنصٍّ مخالف.
    /// (EN) The operation label is a parameter because the reference differs:
    ///      "member access" on read, ".=" on write.
    void raiseIfObjectReceiverIsNull(llvm::Value *objPtr, const char *tag,
                                     const char *operation);

    /// (AR) بابٌ ثانٍ للعائلةِ نفسِها: مُستقبِلٌ ديناميٌّ وسمُه **ليس كائنًا**
    ///      ⇒ RUN033 باسمِ نوعِه زمنَ التشغيل، لا انهيارَ تجزئةٍ ولا خطأَ مترجِمٍ
    ///      داخليًّا. ولم يكن ممكنًا قبلَ تصحيحِ وسمِ `%SadDyn`: كان الكائنُ
    ///      يُوسَمُ نصًّا (`Pointer ⇒ DynKind::Str`)، فحارسٌ على الوسمِ كان يرفعُ
    ///      على المُستقبِلِ السليم. — ISSUE-142.
    /// (EN) The family's second door: a dynamic receiver whose runtime tag is NOT an
    ///      object raises RUN033 naming its runtime type, instead of segfaulting or
    ///      emitting an internal compiler error. Impossible before the %SadDyn tag was
    ///      corrected: an object used to be tagged as a string, so a tag-based guard
    ///      would have rejected valid receivers — ISSUE-142.
    void raiseIfDynReceiverIsNotObject(llvm::Value *dynValue, const char *tag,
                                       const char *operation);
};

}} // namespace Sad::LLVM
#endif
