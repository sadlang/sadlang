/*
 * ============================================================================
 * StringOpsCodeGen — توليد LLVM IR لعمليات النصوص
 * Phase 7 Step 6 — 17 methods من 3 ملفات
 * ============================================================================
 */
#ifndef SAD_LLVM_STRING_OPS_CODEGEN_H
#define SAD_LLVM_STRING_OPS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class StringOpsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit StringOpsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    StringOpsCodeGen(const StringOpsCodeGen &) = delete;
    StringOpsCodeGen &operator=(const StringOpsCodeGen &) = delete;

    // (AR) تطبيعُ معاملٍ نصّيّ إلى مؤشّر: قيمةُ النصّ قد تصل i64 خامًا (مؤشّرٌ
    //      مُحوَّلٌ بـptrtoint) حين تأتي من سجلّ نتيجةٍ عابرٍ لا من متغيّرٍ مُصرَّح.
    //      كان بعضُ البواعث يُطبّع وبعضُها لا، فيسقط التوليدُ بتأكيد LLVM
    //      «نداءُ دالّةٍ بتوقيعٍ سيّئ». نظيرُ normalizeArrayPtr للمصفوفات.
    // (EN) Normalize a string operand to a pointer: a string value can arrive as a
    //      raw i64 (ptrtoint'd pointer) when it comes from a transient result
    //      register rather than a declared variable. Some emitters normalized and
    //      some did not, so codegen hit the LLVM "Calling a function with a bad
    //      signature!" assertion. Mirrors normalizeArrayPtr for arrays.
    llvm::Value *normalizeStringPtr(llvm::Value *str, const char *label);

    // (AR) العمليات الأساسية
    llvm::Value *emitStringConcat(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitStringCharAt(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitStringCmp(std::shared_ptr<SIRInstruction>);

    // (AR) [ز.١٣] الترتيبُ المعجميُّ لنصّين: يُرجع i64 في ‎{-1, 0, +1}‎ بمقارنةِ بايتاتِ
    //      UTF-8 **غيرِ الموقَّعة** (وهي عينُ ترتيبِ نقاطِ الترميز، لأنّ UTF-8 حافظٌ
    //      للترتيب)، وفضِّ التعادلِ بالطولِ فالبادئةُ أصغر. يبعثه الأمامُ لـ`< <= > >=`
    //      بين نصّين ثمّ يقارن نتيجتَه بالصفر. متمايزٌ عن emitStringCmp الذي دلالتُه
    //      منطقيّةٌ (متساويان؟ ⇒ i1) لا ترتيبيّة.
    // (EN) [ز.١٣] Lexicographic ordering of two strings: returns an i64 in {-1, 0, +1} by
    //      comparing **unsigned** UTF-8 bytes (which is exactly code-point order, since UTF-8
    //      is order-preserving), ties broken by length so a prefix sorts first. The frontend
    //      emits it for `< <= > >=` between two strings and then compares the result against
    //      zero. Distinct from emitStringCmp, whose meaning is boolean (equal? ⇒ i1), not
    //      ordinal.
    llvm::Value *emitStringOrdCmp(std::shared_ptr<SIRInstruction>);

    // (AR) التحويلات
    llvm::Value *emitStringToI64(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitStringToF64(std::shared_ptr<SIRInstruction>);

    // (AR) builtins (ops.cpp)
    llvm::Value *emitBuiltinStringLength(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringToUpper(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringToLower(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringFind(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringReplace(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringSubstring(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringTrim(std::shared_ptr<SIRInstruction>);

    // (AR) builtins (ops2.cpp)
    llvm::Value *emitBuiltinStringSplit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringJoin(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringStartsWith(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringEndsWith(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringContains(std::shared_ptr<SIRInstruction>);

private:
    // (AR) مساعد «تقسيم» الموحَّد: يُصدَر مرّة، بدلالة السلسلة الفرعيّة الكاملة
    //      المطابِقة للمفسّر (str.find/substr)، ذاتيّ الاحتواء (malloc/memcpy/
    //      strlen/realloc فقط) فيعمل مستضافًا وحرًّا معًا. يعيد @__sad_string_split.
    // (EN) Unified split helper: substring semantics matching interpreter, self-
    //      contained (only malloc/memcpy/strlen/realloc) so it works hosted AND
    //      freestanding. Emitted once; returns @__sad_string_split.
    llvm::Function *ensureStringSplitHelper();
};

}} // namespace Sad::LLVM
#endif
