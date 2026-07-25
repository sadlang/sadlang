/*
 * ============================================================================
 * ClassesVtablesCodeGen - LLVM IR for class lifecycle, vtables, virtual dispatch
 * Phase 8 Step 3 - 9 methods (module/preprocess/vtables/destructor)
 * ============================================================================
 */
#ifndef SAD_LLVM_CLASSES_VTABLES_CODEGEN_H
#define SAD_LLVM_CLASSES_VTABLES_CODEGEN_H

#include <memory>
#include <string>
#include <vector>
#include <llvm/IR/Value.h>
#include "sir_module.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRModule = Compiler::SIR::SIRModule;

// ============================================================================
// (AR) تصنيف ABI لتمرير/إرجاع بنية @تمثيل_سي بالقيمة عبر حدّ FFI [RFC #53 F2-ج]
//      Direct = تُمرَّر في سجلّات (مُكيَّفة إلى قطعة/قطعتين بحجم سجلّ)؛
//      Memory = تُمرَّر بمؤشّرٍ خفيّ (byval للوسيط / sret للعائد).
// (EN) ABI classification for passing/returning a @تمثيل_سي struct BY VALUE across
//      the FFI boundary [RFC #53 F2-ج]. Direct = in registers (coerced to 1–2
//      register-sized pieces); Memory = via a hidden pointer (byval arg / sret return).
// ============================================================================
enum class CReprAbiKind
{
    NotCRepr, ///< (AR) ليست بنية @تمثيل_سي (لا تصنيف) / (EN) Not a C-repr struct
    Direct,   ///< (AR) في سجلّات (pieces) / (EN) In registers (pieces)
    Memory    ///< (AR) بمؤشّر خفيّ (byval/sret) / (EN) Hidden pointer (byval/sret)
};

struct CReprAbiInfo
{
    CReprAbiKind kind = CReprAbiKind::NotCRepr;
    std::vector<llvm::Type *> pieces; ///< (AR) قطع التكييف (Direct: 1–2) / (EN) Coercion pieces (Direct: 1–2)
    uint64_t sizeBytes = 0;           ///< (AR) حجم البنية بالبايت / (EN) Struct size in bytes
};

// ============================================================================
// (AR) خطّة نداء دالّة خارجيّة ذات معاملات/عائد بنية @تمثيل_سي بالقيمة [RFC #53 F2-ج].
//      تُحسب مرّةً عند إصدار التصريح (emitFunctionPrototype) وتُقرأ عند موقع النداء
//      كي يتطابق التوقيعُ والوسائطُ المولَّدة تمامًا (شكلُ ABI واحد على الطرفين).
// (EN) Call plan for an extern function with by-value @تمثيل_سي struct params/return
//      [RFC #53 F2-ج]. Computed once at declaration (emitFunctionPrototype) and read at
//      the call site so the signature and the emitted args match exactly (one ABI shape).
// ============================================================================
struct CReprArgPlan
{
    bool isStruct = false;   ///< (AR) هذا الوسيط بنية @تمثيل_سي بالقيمة / (EN) This arg is a by-value C-repr struct
    CReprAbiInfo abi;        ///< (AR) تصنيفه (صالح إن isStruct) / (EN) Its classification (valid if isStruct)
    std::string className;   ///< (AR) صنف البنية (للمسار الذاكريّ byval) / (EN) Struct class (for the Memory/byval path)
};

struct CReprCallPlan
{
    bool active = false;             ///< (AR) للدالّة معامل/عائد بنية بالقيمة / (EN) Has a by-value struct param/return
    bool sretReturn = false;         ///< (AR) عائد بالذاكرة: مؤشّر sret خفيّ أوّلًا / (EN) Memory return: hidden sret pointer first
    bool directStructReturn = false; ///< (AR) عائد بنية في سجلّات: يُجسَّد كائنًا / (EN) Direct struct return: materialize object
    CReprAbiInfo returnAbi;          ///< (AR) تصنيف العائد إن كان بنية / (EN) Return classification if struct
    std::string returnClassName;     ///< (AR) صنف العائد (للتجسيد) / (EN) Return class (for materialization)
    std::string linkSymbol;          ///< (AR) رمز رابط LLVM للدالّة (يلتقطه موقع النداء) / (EN) LLVM link symbol (call site looks it up)
    std::vector<CReprArgPlan> args;  ///< (AR) لكلّ وسيط ص بالترتيب / (EN) One per ص argument, in order
};

class ClassesVtablesCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit ClassesVtablesCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    ClassesVtablesCodeGen(const ClassesVtablesCodeGen &) = delete;
    ClassesVtablesCodeGen &operator=(const ClassesVtablesCodeGen &) = delete;

    void emitModule(std::shared_ptr<SIRModule> sirModule);
    void preprocessClasses(std::shared_ptr<SIRModule> sirModule);
    void emitGlobalVariables(std::shared_ptr<SIRModule> sirModule);
    void buildClassVtables(std::shared_ptr<SIRModule> sirModule);
    void patchClassVtables();
    llvm::Value *emitVirtualCall(llvm::Value *objPtr, const std::string &className, const std::string &methodName, const std::vector<llvm::Value *> &extraArgs);
    void storeVtablePtr(llvm::Value *objPtr, const std::string &className);
    void emitDestructorCall(llvm::Value *objPtr, const std::string &className);
    int getFieldStructIndex(const std::string &className, int userFieldIndex) const;

    /// (AR) صنّف كيفيّة تمرير/إرجاع بنية @تمثيل_سي بالقيمة حسب ABI الهدف (SysV/Win64).
    ///      يُرجِع NotCRepr إن لم يكن الصنف بنية @تمثيل_سي مسجَّلة أو غاب نوعها. [RFC #53 F2-ج]
    /// (EN) Classify by-value pass/return of a @تمثيل_سي struct per the target ABI
    ///      (SysV/Win64). Returns NotCRepr if the class is not a registered C-repr
    ///      struct or its LLVM type is missing. [RFC #53 F2-ج]
    CReprAbiInfo classifyCReprAbi(const std::string &className) const;
};

}} // namespace Sad::LLVM
#endif
