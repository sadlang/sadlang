// ============================================================================
// llvm_codegen_lowlevel.cpp — توليد LLVM IR لعمليات المكتبة المنخفضة المستوى
// LLVM IR Generation for Low-Level OS Library Operations
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 2026
// الإصدار / Version: 5.0 — دعم 19 وحدة لبرمجة أنظمة التشغيل
//
// (AR) هذا الملف يولّد LLVM IR لعمليات المكتبة المنخفضة المستوى.
//      العمليات البدائية تُترجم إلى inline assembly مباشرة.
//      العمليات العالية المستوى تُترجم إلى استدعاءات runtime C.
// (EN) This file generates LLVM IR for low-level library operations.
//      Primitive ops are translated to inline assembly directly.
//      High-level ops are translated to C runtime function calls.
// ============================================================================

#include "llvm_codegen.h"
#include "builders/platform/lowlevel_codegen.h"
// (AR) [علة قسمة العام — نحلة · شقيقُ hardware_ffi_ops] كلُّ معاملٍ في هذا الملفِّ
//      سياقُه عدديٌّ: إمّا يُمرَّر i64 إلى نداءِ وقتِ تشغيلٍ حرٍّ، وإمّا يُقصُّ إلى
//      عرضِ سجلِّ الهدفِ قبل أسمبليٍّ سطريّ. فإن حُلَّ المعاملُ `%SadDyn` (عامٌّ
//      رُقّي بإسنادِ ناتجِ `//` الديناميكيِّ) انفجرَ الطرفان: `CreateIntCast` يُصدر
//      `zext %SadDyn to i64` ونداءُ وقتِ التشغيلِ يمرِّرُ بنيةً لمعاملٍ i64 — كلاهما
//      يُسقط verifyModule. قِيس على `اكتب_سجل_تحكم(3، عامٌّ_ديناميكيّ)` و
//      `ابطل_صفحة(عامٌّ_ديناميكيّ)` — وهما بوّابتا إدارةِ الذاكرةِ في نواةِ النحلة.
//      لذا يُحلُّ كلُّ معاملٍ هنا بـ`resolveUnboxedIntOperand` (فكٌّ بوسمِه: عشريٌّ
//      قيمةً لا بتّاتٍ)، وهي هويّةٌ على القيمِ غيرِ الديناميكيّة فلا تغيّرُ IR القائم.
// (EN) [Global-division bug — nahla · sibling of hardware_ffi_ops] every operand in
//      this file is integer-context (i64 runtime-call arg, or a width cast before
//      inline asm), so all resolve through the tag-respecting unbox helper; it is
//      the identity on non-dynamic values.
#include "sad_dyn_repr.h"
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

namespace Sad {
namespace LLVM {

using SIRInstruction = Compiler::SIR::SIRInstruction;
using SIROperand = Compiler::SIR::SIROperand;
using SIROpcode = Compiler::SIR::SIROpcode;

// ============================================================================
// (AR) [علة قسمة العام — نحلة] فكُّ قيمةٍ محلولةٍ سلفًا إن كانت %SadDyn، بوسمِها
//      (عشريٌّ قيمةً لا بتّاتٍ). يُستعمَل حيثُ يُحَلُّ المعاملُ مرّةً ثمّ تفترقُ
//      ذراعُ الأسمبليِّ (تقصُّ فتلزمُها قيمةٌ عدديّة) عن ذراعِ نداءِ وقتِ التشغيل
//      (تمرُّ بـcoerceToParamType فتحفظُ ذراعَ الغيابِ الموسوم). هويّةٌ على غيرِه.
// (EN) Unbox an already-resolved value if it is %SadDyn (tag-respecting). Used
//      where one resolve feeds both an inline-asm arm (needs an integer) and a
//      runtime-call arm (whose single cast table must keep the absent-tag arm).
// ============================================================================
static llvm::Value* unpackIfDyn(LLVMCodeGen& cg, llvm::Value* v) {
    return (v && isSadDyn(v)) ? unpackI64(cg, v) : v;
}

// ============================================================================
// Helper: emit a call to a freestanding runtime C function
// ============================================================================
static llvm::Value* emitRuntimeCall(
    LLVMCodeGen* cg,
    llvm::IRBuilder<>& builder,
    llvm::Module* module,
    const std::string& funcName,
    llvm::Type* retType,
    const std::vector<llvm::Type*>& argTypes,
    const std::vector<llvm::Value*>& argValues)
{
    llvm::Function* fn = module->getFunction(funcName);
    if (!fn) {
        llvm::FunctionType* ft = llvm::FunctionType::get(retType, argTypes, false);
        fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, funcName, module);
    }
    // (AR) [عقد وقت التشغيل الحرّ] لاءِمْ كلَّ وسيطٍ لنوعِ معاملِه المصرَّحِ قبل
    //      النداء. الباعثُ قد يبني قيمًا i64 بينما توقيعُ الرمزِ يُصرَّح i32 (قِيس:
    //      `apic_تهيئة_مؤقت(32، 0، 3)` ⇒ `call void @sad_ll_apic_init_timer(i64…)`
    //      على تصريحٍ i32 ⇒ verifyModule يسقط). و`coerceToParamType` هي جدولُ
    //      التحويلِ الوحيدُ في الخلفيّة (يحترمُ وسمَ %SadDyn) فلا نسخةَ ثانيةً هنا.
    // (EN) [Freestanding runtime contract] coerce each argument to its declared
    //      parameter type via the backend's single cast table before the call.
    std::vector<llvm::Value*> coerced;
    coerced.reserve(argValues.size());
    llvm::FunctionType* ft = fn->getFunctionType();
    for (size_t i = 0; i < argValues.size(); ++i) {
        llvm::Value* v = argValues[i];
        // (AR) الملاءمةُ **ضيّقةٌ عمدًا**: توسيعُ/قصُّ عرضٍ بين عددَين، أو فكُّ قيمةٍ
        //      موسومةٍ %SadDyn إلى نوعِ المعامل. وما عدا ذلك يُترَك كما هو ليردَّه
        //      المُصادِق: ملاءمةٌ واسعةٌ كانت تحوّلُ عددًا إلى مؤشّرٍ بـinttoptr
        //      فتقبلُ `uefi_قراءة_متغير(5)` صامتةً وتسلّمُ النواةَ مؤشّرًا برّيًّا
        //      قيمتُه 5 — بناءٌ أخضرُ مكانَ خطأِ توقيعٍ كان يُسقطه verifyModule.
        // (EN) Deliberately narrow: integer width adjustment, or unboxing a tagged
        //      %SadDyn. Anything else is left for the verifier to reject — a wide
        //      coercion silently turned an integer into a pointer (inttoptr).
        const bool coercible =
            v && i < ft->getNumParams() &&
            (isSadDyn(v) ||
             (v->getType()->isIntegerTy() && ft->getParamType(i)->isIntegerTy()));
        coerced.push_back(coercible ? coerceToParamType(*cg, v, ft->getParamType(i)) : v);
    }
    return builder.CreateCall(fn, coerced);
}

// ============================================================================
// (AR) يربط ناتج تعليمة منخفضة بسجلّها الناتج في جدول القيم (namedValues) كي
//      يجده مستهلكوه (نمط دوالّ الحساب). دوالّ CPU المنخفضة كانت تعيد القيمة
//      بلا ربطها ⇒ «Undefined register» عند استهلاكها (كامن: exit 0 مستضافًا،
//      يظهر تحت بوّابة الوضع الحرّ). يعيد القيمة نفسها للتسلسل.
// (EN) Bind a low-level instruction's result value into namedValues so consumers
//      resolve it (as arithmetic emitters do). Returns the value for chaining.
// ============================================================================
static llvm::Value* bindLowlevelResult(LLVMCodeGen& cg,
    const std::shared_ptr<SIRInstruction>& inst, llvm::Value* result) {
    if (result && inst->result.has_value())
        cg.context_info_.namedValues[inst->result->name] = result;
    return result;
}

// ============================================================================
// 15a. وحدة المعالج المتقدمة / Advanced CPU Module
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelCpuGetInfo(std::shared_ptr<SIRInstruction> inst) {
    // (AR) استدعاء دالة runtime: sad_ll_cpu_get_info() -> i64 (مؤشر لبنية CPUInfo)
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_cpu_get_info", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuGetFeatures(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_cpu_get_features", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuReadMSR(std::shared_ptr<SIRInstruction> inst) {
    // (AR) rdmsr — قراءة سجل نموذج محدد
    // (EN) rdmsr instruction via inline assembly: ecx=reg -> edx:eax
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    // (AR) [علة قسمة العام] ذراعا rdmsr كلتاهما أسمبليّتان (لا نداءَ وقتِ تشغيلٍ
    //      يلائمُ الوسائط)، فالفكُّ بالوسمِ هنا لا في حدِّ النداء.
    llvm::Value* reg = resolveUnboxedIntOperand(cg_, inst->operands[0]);
    reg = cg_.builder_->CreateIntCast(reg, i32Ty, false);

    if (cg_.freestanding_) {
        // (AR) الوضع الحرّ: rdmsr يعيد edx:eax على الهدفين معًا؛ ندمجهما في i64
        //      بعمليّات LLVM لا بـshlq (فلا يفترض النصُّ عرضَ سجلّ).
        // (EN) Freestanding: rdmsr -> {eax,edx} on both targets; combine in IR.
        auto* retTy = llvm::StructType::get(*cg_.context_, {i32Ty, i32Ty});
        auto* asmTy = llvm::FunctionType::get(retTy, {i32Ty}, false);
        auto* inlineAsm = llvm::InlineAsm::get(asmTy,
            "rdmsr", "={eax},={edx},{ecx}", true, false, llvm::InlineAsm::AD_ATT);
        auto* result = cg_.builder_->CreateCall(asmTy, inlineAsm, {reg});
        auto* eax = cg_.builder_->CreateExtractValue(result, 0, "msr_lo");
        auto* edx = cg_.builder_->CreateExtractValue(result, 1, "msr_hi");
        auto* eax64 = cg_.builder_->CreateZExt(eax, i64Ty);
        auto* edx64 = cg_.builder_->CreateZExt(edx, i64Ty);
        auto* shifted = cg_.builder_->CreateShl(edx64, 32);
        return bindLowlevelResult(cg_, inst,
            cg_.builder_->CreateOr(eax64, shifted, "msr_val"));
    }

    // (AR) الوضع المستضاف: يبقى كما هو. ⚠️ دَينٌ مُقَرٌّ: هذا الأسمبليُّ
    //      (shlq/%rdx/%rax) مقيَّدٌ بـx86-64، فالمستضافُ هنا يفترض المعماريّةَ
    //      لا العرضَ وحدَه. والبابُ الباقي هو **المستضافُ على i686**: بوّابةُ
    //      المعماريّة تقبله (i686 داخلَ العائلة) ثمّ يبثّ `shlq`/`%rax` ولا
    //      وجودَ لهما هناك. أمّا مستضافُ aarch64 فتردّه البوّابةُ قبل بلوغِه.
    auto* asmTy = llvm::FunctionType::get(i64Ty, {i32Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(asmTy,
        "rdmsr; shlq $$32, %rdx; orq %rdx, %rax",
        "={rax},{ecx},~{rdx}", true);
    return bindLowlevelResult(cg_, inst,
        cg_.builder_->CreateCall(asmTy, inlineAsm, {reg}));
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuWriteMSR(std::shared_ptr<SIRInstruction> inst) {
    // (AR) wrmsr — كتابة سجل نموذج محدد
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    
    // (AR) [علة قسمة العام] wrmsr أسمبليٌّ صرف: الفكُّ بالوسمِ قبلَ القصِّ والبتر.
    llvm::Value* reg = resolveUnboxedIntOperand(cg_, inst->operands[0]);
    llvm::Value* val = resolveUnboxedIntOperand(cg_, inst->operands[1]);
    reg = cg_.builder_->CreateIntCast(reg, i32Ty, false);
    
    // Split value: eax = low 32, edx = high 32
    llvm::Value* lo = cg_.builder_->CreateTrunc(val, i32Ty);
    llvm::Value* hi = cg_.builder_->CreateTrunc(cg_.builder_->CreateLShr(val, 32), i32Ty);
    
    auto* asmTy = llvm::FunctionType::get(voidTy, {i32Ty, i32Ty, i32Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(asmTy,
        "wrmsr", "{ecx},{eax},{edx}", true);
    return cg_.builder_->CreateCall(asmTy, inlineAsm, {reg, lo, hi});
}

// (AR) يستخرج رقم سجلّ التحكّم الثابت من المعامل الأوّل، ويتحقّق ∈{0,2,3,4}.
//      تعليمة `mov %crN` تتطلّب N حرفيًّا في نصّ الأسمبلي فلا يقبل رقمًا متغيّرًا
//      في الوضع الحرّ. يعيد false ويبلّغ خطأً عند التعذّر.
// (EN) Extract constant CR number (must be literal for `mov %crN`); {0,2,3,4}.
static bool extractConstCrNum(LLVMCodeGen& cg, llvm::Value* crNum,
                              const char* who, unsigned& out) {
    auto* ci = llvm::dyn_cast<llvm::ConstantInt>(crNum);
    if (!ci) {
        cg.reportError(::Sad::Errors::ErrorCode::SEM_FREESTANDING_SYS_BUILTIN_ARG,
            {{"detail", std::string(who) + ": رقم سجلّ التحكّم يجب أن يكون ثابتًا حرفيًّا في الوضع الحرّ"}});
        return false;
    }
    unsigned n = static_cast<unsigned>(ci->getZExtValue());
    if (n != 0 && n != 2 && n != 3 && n != 4) {
        cg.reportError(::Sad::Errors::ErrorCode::SEM_FREESTANDING_SYS_BUILTIN_ARG,
            {{"detail", std::string(who) + ": سجلّ تحكّم غير مدعوم (المدعوم: CR0/CR2/CR3/CR4)"}});
        return false;
    }
    out = n;
    return true;
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuReadCR(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* crNum = cg_.resolveOperand(inst->operands[0]);

    if (cg_.freestanding_) {
        // (AR) `mov %crN, reg` ثمّ توسيعٌ بالأصفار إلى i64 (عقدُ جالبات العتاد).
        //      عرضُ السجلّ من الهدف: i686 ⇒ reg32، x86_64 ⇒ reg64. التوسيعُ إلى
        //      i64 على الهدف الـ64-بتّيّ لا يُصدر تعليمةً (النوعان متطابقان).
        unsigned n = 0;
        if (!extractConstCrNum(cg_, crNum, "اقرأ_سجل_تحكم", n)) {
            // (AR) اربط الناتج بصفر ثابت كي لا تتتالى «Undefined register» على
            //      مستهلكيه (نمط بوّابة SEM019)؛ البناء يُحبَط عبر بوّابة hasErrors.
            return bindLowlevelResult(cg_, inst, llvm::ConstantInt::get(i64Ty, 0));
        }
        auto* gprTy = cg_.getTargetGprType();
        auto* asmTy = llvm::FunctionType::get(gprTy, {}, false);
        auto* inlineAsm = llvm::InlineAsm::get(asmTy,
            "mov %cr" + std::to_string(n) + ", $0", "=r",
            true, false, llvm::InlineAsm::AD_ATT);
        auto* raw = cg_.builder_->CreateCall(asmTy, inlineAsm, {});
        return bindLowlevelResult(cg_, inst, cg_.builder_->CreateIntCast(raw, i64Ty, false));
    }

    // (AR) الوضع المستضاف: نداء runtime يحسم الفرع على رقم CR
    return bindLowlevelResult(cg_, inst, emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_read_cr", i64Ty, {i64Ty}, {crNum}));
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuWriteCR(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* crNum = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* val = cg_.resolveOperand(inst->operands[1]);

    if (cg_.freestanding_) {
        // (AR) `mov reg, %crN` — القيمة تُلاءَم لعرض سجلّ الهدف: تُقصّ إلى 32-بت على
        //      i686 (فضاءُ عناوينه)، وتبقى 64-بت على x86_64.
        unsigned n = 0;
        if (!extractConstCrNum(cg_, crNum, "اكتب_سجل_تحكم", n)) return nullptr;
        auto* gprTy = cg_.getTargetGprType();
        // (AR) [علة قسمة العام] الذراعُ الحرّةُ تقصُّ للأسمبلي فيلزمُها الفكُّ بالوسم؛
        //      والذراعُ المستضافةُ تمرُّ بـemitRuntimeCall فتلائمُها طاولةُ التحويلِ
        //      الوحيدةُ (وفيها ذراعُ الغيابِ الموسومِ — فلا يُفَكُّ مبكّرًا هنا).
        // (EN) Only the freestanding arm casts for inline asm and needs the unbox;
        //      the hosted arm goes through emitRuntimeCall's single cast table.
        llvm::Value* valGpr = cg_.builder_->CreateIntCast(
            unpackIfDyn(cg_, val), gprTy, false);
        auto* asmTy = llvm::FunctionType::get(voidTy, {gprTy}, false);
        auto* inlineAsm = llvm::InlineAsm::get(asmTy,
            "mov $0, %cr" + std::to_string(n), "r,~{memory}",
            true, false, llvm::InlineAsm::AD_ATT);
        return cg_.builder_->CreateCall(asmTy, inlineAsm, {valGpr});
    }

    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_write_cr", voidTy, {i64Ty, i64Ty}, {crNum, val});
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuInvlpg(std::shared_ptr<SIRInstruction> inst) {
    // (AR) invlpg — إبطال صفحة في TLB
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* addr = cg_.resolveOperand(inst->operands[0]);

    if (cg_.freestanding_) {
        // (AR) العنوان بعرض سجلّ الهدف: قيدُ r بنوع i64 يطلب سجلًّا 64-بت لا وجود
        //      له على i686، وقصُّه إلى 32-بت على x86_64 يُبطل الصفحةَ الخطأ.
        auto* gprTy = cg_.getTargetGprType();
        // (AR) [علة قسمة العام] الذراعُ الحرّةُ وحدَها تُفَكُّ (كما في اكتب_سجل_تحكم).
        llvm::Value* addrGpr = cg_.builder_->CreateIntCast(
            unpackIfDyn(cg_, addr), gprTy, false);
        auto* asmTy = llvm::FunctionType::get(voidTy, {gprTy}, false);
        auto* inlineAsm = llvm::InlineAsm::get(asmTy,
            "invlpg ($0)", "r,~{memory}", true, false, llvm::InlineAsm::AD_ATT);
        return cg_.builder_->CreateCall(asmTy, inlineAsm, {addrGpr});
    }

    auto* asmTy = llvm::FunctionType::get(voidTy, {i64Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(asmTy,
        "invlpg ($0)", "r", true);
    return cg_.builder_->CreateCall(asmTy, inlineAsm, {addr});
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_cpu_report", i8PtrTy, {}, {});
}

// ============================================================================
// عنوان_رمز / symbol_addr — عنوان رمز رابط خارجيّ كـرقم (i64)
// (AR) يأخذ اسم رمز رابط ثابتًا (سلسلة حرفيّة) ويُصدر ptrtoint على رمز خارجيّ.
//      إن كان الرمز مصرَّحًا مسبقًا (دالّة أو عالميّ) يُعاد استخدامه؛ وإلّا يُنشأ
//      تصريح رمز بيانات خارجيّ (i8) عند الطلب. مؤشّر الهدف (32-بت على i686 و64
//      على x86_64) يوسَّع بالأصفار إلى i64 (نظير عقد الجالبات (unsigned int)&sym كـu64).
//      قيد النطاق: للرموز الرابطة الخارجيّة حصرًا (بيانات/دوالّ C/أسمبلي/رموز .ld)؛
//      لا يرصد الأخطاء المطبعيّة (اسم لا يطابق ⇒ خطأ رابط متأخّر)، ولا يأخذ عنوان
//      رمز ص مُعرَّف داخليًّا (اسمه مُشوَّه mangled). الوسيط غير الثابت أو الفارغ
//      يُرفَض بخطأ SEM_SYMBOL_ADDR_ARG (يجب أن يُعرَف الاسم وقت الترجمة لإصدار
//      relocation). يقبله المترجم في الوضعين (الحرّ والمستضاف)؛ المفسّر يرفضه.
// (EN) Address of external linker symbol as i64. Constant string arg only.
// ============================================================================
llvm::Value* LowlevelCodeGen::emitLowlevelSymbolAddr(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);

    // (AR) الوسيط يجب أن يكون سلسلة حرفيّة ثابتة (اسمها في operand.name)
    const bool isConstStr = !inst->operands.empty()
        && inst->operands[0].type == Compiler::SIR::SIROperandType::CONSTANT
        && inst->operands[0].dataType == Compiler::SIR::SadTypeKind::String;
    if (!isConstStr || inst->operands[0].name.empty()) {
        // (AR) ميّز السبب: سلسلة فارغة (رمز بلا اسم) عن وسيط غير ثابت أصلًا.
        const char* detail = (isConstStr && inst->operands[0].name.empty())
            ? "عنوان_رمز: اسم الرمز سلسلة فارغة — مرّر اسم رمز رابط غير فارغ"
            : "عنوان_رمز يتطلّب اسم رمز ثابتًا (سلسلة حرفيّة)، لا تعبيرًا وقت تشغيل";
        cg_.reportError(::Sad::Errors::ErrorCode::SEM_SYMBOL_ADDR_ARG, {{"detail", detail}});
        // (AR) اربط صفرًا كي لا تتتالى «Undefined register»؛ البناء يُحبَط بـhasErrors.
        return bindLowlevelResult(cg_, inst, llvm::ConstantInt::get(i64Ty, 0));
    }

    const std::string& sym = inst->operands[0].name;

    // (AR) أعِد استخدام أيّ تصريح سابق للرمز (دالّة أو عالميّ) تفاديًا لتصادم
    //      إعادة التسمية في جدول رموز الوحدة؛ وإلّا أنشئ تصريح رمز بيانات خارجيّ.
    llvm::Constant* g = llvm::dyn_cast_or_null<llvm::GlobalValue>(
        cg_.module_->getNamedValue(sym));
    if (!g) {
        auto* i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
        g = cg_.module_->getOrInsertGlobal(sym, i8Ty); // ExternalLinkage عند الإنشاء
    }

    // (AR) ptrtoint إلى i64 — من مؤشّر الهدف (يوسَّع بالأصفار على الأهداف 32-بتّيّة)
    llvm::Value* addr = cg_.builder_->CreatePtrToInt(g, i64Ty, "symaddr");
    return bindLowlevelResult(cg_, inst, addr);
}

// ============================================================================
// 15b. وحدة GDT
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelGdtInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_gdt_init", voidTy, {}, {});
}

// (AR) يُصدر lgdt/lidt الحرّة: تعليمة تحمّل واصف الجدول (الحدّ 2-بايت + القاعدة
//      بعرض سجلّ الهدف: 4 بايت على i686 و8 على x86_64) من الذاكرة عبر مؤشّر
//      (operands[0]، يُلاءَم لعرض سجلّ الهدف). يتطلّب وجود المؤشّر؛ غيابه في الوضع
//      الحرّ خطأ استعمال. clobber "memory" يرسّخ كتابة الواصف قبل التحميل.
//      يُشارَك بين lgdt/lidt (المنمنمة الوحيدة تختلف).
// (EN) Emit freestanding lgdt/lidt: loads the table descriptor via a pointer whose
//      width follows the target register width.
static llvm::Value* emitFreestandingTableLoad(LLVMCodeGen& cg,
    const std::shared_ptr<SIRInstruction>& inst, const char* mnemonic, const char* who) {
    auto* voidTy = llvm::Type::getVoidTy(*cg.context_);
    if (inst->operands.empty()) {
        cg.reportError(::Sad::Errors::ErrorCode::SEM_FREESTANDING_SYS_BUILTIN_ARG,
            {{"detail", std::string(who) + ": يتطلّب مؤشّر واصف الجدول (الحدّ+القاعدة) في الوضع الحرّ"}});
        return nullptr;
    }
    auto* gprTy = cg.getTargetGprType();
    // (AR) [علة قسمة العام] المسارُ حرٌّ صرف: الفكُّ بالوسمِ قبل قصِّ عرضِ السجلّ.
    llvm::Value* ptr = resolveUnboxedIntOperand(cg, inst->operands[0]);
    llvm::Value* ptrGpr = cg.builder_->CreateIntCast(ptr, gprTy, false);
    auto* asmTy = llvm::FunctionType::get(voidTy, {gprTy}, false);
    auto* inlineAsm = llvm::InlineAsm::get(asmTy,
        std::string(mnemonic) + " ($0)", "r,~{memory}",
        true, false, llvm::InlineAsm::AD_ATT);
    return cg.builder_->CreateCall(asmTy, inlineAsm, {ptrGpr});
}

llvm::Value* LowlevelCodeGen::emitLowlevelGdtLoad(std::shared_ptr<SIRInstruction> inst) {
    // (AR) lgdt — تحميل جدول الواصفات العامة
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    if (cg_.freestanding_)
        return emitFreestandingTableLoad(cg_, inst, "lgdt", "حمل_جدول_واصفات");
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_gdt_load", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelGdtGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_gdt_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15c. وحدة الترحيل / Paging
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelPagingInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_paging_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPagingMap(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* vaddr = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* paddr = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* flags = (inst->operands.size() > 2) ? 
        cg_.resolveOperand(inst->operands[2]) : 
        llvm::ConstantInt::get(i64Ty, 0x3); // default: present + writable
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_paging_map", voidTy, {i64Ty, i64Ty, i64Ty}, {vaddr, paddr, flags});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPagingUnmap(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* vaddr = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_paging_unmap", voidTy, {i64Ty}, {vaddr});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPagingFlushTlb(std::shared_ptr<SIRInstruction> inst) {
    // (AR) قراءةُ cr3 ثمّ إعادةُ كتابته — إفراغُ ذاكرة الترجمة بالكامل. يُصاغ
    //      بقيدَي سجلٍّ (=r ثمّ r) لا باسمِ مركمٍ حرفيّ، فيختار مُخصِّصُ السجلّات
    //      سجلًّا عامًّا بعرض الهدف (32 بتًّا على i686 و64 على x86_64) أيًّا كان،
    //      بلا فرعٍ على الوضع ولا إتلافٍ لمركمٍ بعينه. والمسار واحدٌ في الوضعين
    //      لأنّ الدلالة واحدة.
    // (EN) Read cr3 then write it back — full TLB flush. Uses register constraints
    //      so the width follows the target instead of a hardcoded accumulator.
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* gprTy = cg_.getTargetGprType();
    auto* readTy = llvm::FunctionType::get(gprTy, {}, false);
    auto* readAsm = llvm::InlineAsm::get(readTy,
        "mov %cr3, $0", "=r", true, false, llvm::InlineAsm::AD_ATT);
    auto* cr3 = cg_.builder_->CreateCall(readTy, readAsm, {});
    auto* writeTy = llvm::FunctionType::get(voidTy, {gprTy}, false);
    auto* writeAsm = llvm::InlineAsm::get(writeTy,
        "mov $0, %cr3", "r,~{memory}", true, false, llvm::InlineAsm::AD_ATT);
    return cg_.builder_->CreateCall(writeTy, writeAsm, {cr3});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPagingGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_paging_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15d. وحدة المقاطعات المتقدمة / Advanced Interrupts (IDT)
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelIdtInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_idt_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelIdtLoad(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    if (cg_.freestanding_)
        return emitFreestandingTableLoad(cg_, inst, "lidt", "حمل_جدول_مقاطعات");
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_idt_load", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelIdtRegisterIsr(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* isrNum = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* handler = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_idt_register_isr", voidTy, {i64Ty, i64Ty}, {isrNum, handler});
}

llvm::Value* LowlevelCodeGen::emitLowlevelIdtEnableIrq(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* irq = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_idt_enable_irq", voidTy, {i64Ty}, {irq});
}

llvm::Value* LowlevelCodeGen::emitLowlevelIdtGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_idt_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15e. وحدة PCI
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelPciEnumerate(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_pci_enumerate", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPciReadConfig(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* bus = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* dev = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* func = cg_.resolveOperand(inst->operands[2]);
    llvm::Value* offset = cg_.resolveOperand(inst->operands[3]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_pci_read_config", i64Ty,
        {i64Ty, i64Ty, i64Ty, i64Ty}, {bus, dev, func, offset});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPciWriteConfig(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* bus = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* dev = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* func = cg_.resolveOperand(inst->operands[2]);
    llvm::Value* offset = cg_.resolveOperand(inst->operands[3]);
    llvm::Value* value = cg_.resolveOperand(inst->operands[4]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_pci_write_config", voidTy,
        {i64Ty, i64Ty, i64Ty, i64Ty, i64Ty}, {bus, dev, func, offset, value});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPciGetDeviceCount(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_pci_device_count", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPciGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_pci_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15f. وحدة DMA المتقدمة
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelDmaInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_dma_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelDmaTransfer(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* src = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* dst = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* size = cg_.resolveOperand(inst->operands[2]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_dma_transfer", voidTy, {i64Ty, i64Ty, i64Ty}, {src, dst, size});
}

llvm::Value* LowlevelCodeGen::emitLowlevelDmaStatus(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_dma_status", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelDmaGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_dma_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15g. وحدة الشاشة / Framebuffer
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelFbInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* w = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* h = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* bpp = (inst->operands.size() > 2) ?
        cg_.resolveOperand(inst->operands[2]) :
        llvm::ConstantInt::get(i64Ty, 32);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_init", voidTy, {i64Ty, i64Ty, i64Ty}, {w, h, bpp});
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbSetPixel(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* x = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* y = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* color = cg_.resolveOperand(inst->operands[2]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_set_pixel", voidTy, {i64Ty, i64Ty, i64Ty}, {x, y, color});
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbDrawRect(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types(inst->operands.size(), i64Ty);
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) vals.push_back(cg_.resolveOperand(op));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_draw_rect", voidTy, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbFillRect(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types(inst->operands.size(), i64Ty);
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) vals.push_back(cg_.resolveOperand(op));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_fill_rect", voidTy, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbDrawLine(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types(inst->operands.size(), i64Ty);
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) vals.push_back(cg_.resolveOperand(op));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_draw_line", voidTy, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbDrawString(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types(inst->operands.size(), i64Ty);
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) vals.push_back(cg_.resolveOperand(op));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_draw_string", voidTy, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbClear(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    if (!inst->operands.empty()) {
        llvm::Value* color = cg_.resolveOperand(inst->operands[0]);
        return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
            "sad_ll_fb_clear", voidTy, {i64Ty}, {color});
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_clear_default", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15h. وحدة ACPI
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiFindTable(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* sig = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_find_table", i64Ty, {i64Ty}, {sig});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiShutdown(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_shutdown", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15i. وحدة التزامن / Sync
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelSpinlockInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_spinlock_init", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSpinlockLock(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* lock = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_spinlock_lock", voidTy, {i64Ty}, {lock});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSpinlockUnlock(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* lock = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_spinlock_unlock", voidTy, {i64Ty}, {lock});
}

llvm::Value* LowlevelCodeGen::emitLowlevelMutexInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mutex_init", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelMutexLock(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* mtx = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mutex_lock", voidTy, {i64Ty}, {mtx});
}

llvm::Value* LowlevelCodeGen::emitLowlevelMutexUnlock(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* mtx = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mutex_unlock", voidTy, {i64Ty}, {mtx});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSemaphoreInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* count = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_semaphore_init", i64Ty, {i64Ty}, {count});
}

llvm::Value* LowlevelCodeGen::emitLowlevelBarrierInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* count = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_barrier_init", i64Ty, {i64Ty}, {count});
}

// ============================================================================
// 15j. وحدة المجدول / Scheduler
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelSchedInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_sched_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSchedCreateProc(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(cg_.resolveOperand(op));
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_sched_create_process", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelSchedCreateThread(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* name = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* entry = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_sched_create_thread", i64Ty, {i64Ty, i64Ty}, {name, entry});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSchedYield(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_sched_yield", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSchedSleep(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* ms = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_sched_sleep", voidTy, {i64Ty}, {ms});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSchedGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_sched_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15k. وحدة الإقلاع / Boot
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelBootGetInfo(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_boot_info", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelBootGetMemoryMap(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_boot_memory_map", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelBootGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_boot_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15l. وحدة نظام الملفات الافتراضي / VFS
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelVfsMount(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(cg_.resolveOperand(op));
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_mount", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelVfsUnmount(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* mp = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_unmount", voidTy, {i64Ty}, {mp});
}

llvm::Value* LowlevelCodeGen::emitLowlevelVfsOpen(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(cg_.resolveOperand(op));
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_open", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelVfsRead(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* fd = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* size = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_read", i64Ty, {i64Ty, i64Ty}, {fd, size});
}

llvm::Value* LowlevelCodeGen::emitLowlevelVfsWrite(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* fd = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* data = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_write", i64Ty, {i64Ty, i64Ty}, {fd, data});
}

llvm::Value* LowlevelCodeGen::emitLowlevelVfsClose(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* fd = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_close", voidTy, {i64Ty}, {fd});
}

llvm::Value* LowlevelCodeGen::emitLowlevelVfsGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15m. وحدة APIC
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelApicInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicSendEoi(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_send_eoi", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicSendIpi(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* target = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* vector = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_send_ipi", voidTy, {i64Ty, i64Ty}, {target, vector});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicSetTimer(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* divisor = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* count = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_set_timer", voidTy, {i64Ty, i64Ty}, {divisor, count});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15n. وحدة HPET
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelHpetInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_hpet_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelHpetRead(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_hpet_read", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelHpetSleep(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* ns = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_hpet_sleep", voidTy, {i64Ty}, {ns});
}

llvm::Value* LowlevelCodeGen::emitLowlevelHpetGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_hpet_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15o. وحدة استدعاءات النظام / Syscall
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelSyscallInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_syscall_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSyscallRegister(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* num = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* handler = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_syscall_register", voidTy, {i64Ty, i64Ty}, {num, handler});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSyscallInvoke(std::shared_ptr<SIRInstruction> inst) {
    // (AR) syscall — تنفيذ استدعاء نظام بالرقم والمعاملات
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(cg_.resolveOperand(op));
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_syscall_invoke", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelSyscallGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_syscall_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15p. عمليات الذاكرة المتقدمة / Advanced Memory
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelMemAllocPhys(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* size = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mem_alloc_phys", i64Ty, {i64Ty}, {size});
}

llvm::Value* LowlevelCodeGen::emitLowlevelMemFreePhys(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* addr = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mem_free_phys", voidTy, {i64Ty}, {addr});
}

llvm::Value* LowlevelCodeGen::emitLowlevelMemMapRegion(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(cg_.resolveOperand(op));
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mem_map_region", voidTy, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelMemGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mem_report", i8PtrTy, {}, {});
}

// ============================================================================
// القسم 16: بروتوكول UEFI — توليد LLVM IR
// Section 16: UEFI Boot Protocol — LLVM IR Generation
// ============================================================================

} // namespace LLVM
} // namespace Sad

