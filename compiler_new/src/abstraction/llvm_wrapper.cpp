/**
 * =============================================================================
 * ملف: llvm_wrapper.cpp
 * الوصف: تنفيذ طبقة تجريد LLVM لمترجم لغة ص
 * المهمة: T1030 - Phase 91
 * =============================================================================
 */

#include "llvm_wrapper.hpp"

#ifdef SAD_USE_LLVM
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#endif

#include <sstream>

namespace sad {
namespace llvm_wrapper {

// ═══════════════════════════════════════════════════════════════════════════════
//                              تنفيذ فئة القيمة
// ═══════════════════════════════════════════════════════════════════════════════

bool قيمة::صالحة() const {
#ifdef SAD_USE_LLVM
    return m_قيمة != nullptr;
#else
    return false;
#endif
}

std::string قيمة::اسم() const {
#ifdef SAD_USE_LLVM
    if (m_قيمة) {
        return std::string(m_قيمة->getName());
    }
#endif
    return "";
}

void قيمة::عيِّن_اسم(const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_قيمة) {
        m_قيمة->setName(اسم);
    }
#endif
}

bool قيمة::ثابتة() const {
#ifdef SAD_USE_LLVM
    return m_قيمة && llvm::isa<llvm::Constant>(m_قيمة);
#else
    return false;
#endif
}

std::string قيمة::إلى_نص() const {
#ifdef SAD_USE_LLVM
    if (!m_قيمة) return "<null>";
    
    std::string str;
    llvm::raw_string_ostream os(str);
    m_قيمة->print(os);
    return os.str();
#else
    return "<no-llvm>";
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              تنفيذ فئة النوع
// ═══════════════════════════════════════════════════════════════════════════════

نوع نوع::أنشئ_أساسي(نوع_أساسي النوع, void* سياق) {
#ifdef SAD_USE_LLVM
    auto* ctx = static_cast<llvm::LLVMContext*>(سياق);
    
    switch (النوع) {
        case نوع_أساسي::ع1:
            return نوع(llvm::Type::getInt1Ty(*ctx));
        case نوع_أساسي::ع8:
        case نوع_أساسي::ح8:
            return نوع(llvm::Type::getInt8Ty(*ctx));
        case نوع_أساسي::ع16:
        case نوع_أساسي::ح16:
            return نوع(llvm::Type::getInt16Ty(*ctx));
        case نوع_أساسي::ع32:
        case نوع_أساسي::ح32:
            return نوع(llvm::Type::getInt32Ty(*ctx));
        case نوع_أساسي::ع64:
        case نوع_أساسي::ح64:
            return نوع(llvm::Type::getInt64Ty(*ctx));
        case نوع_أساسي::ع128:
            return نوع(llvm::Type::getInt128Ty(*ctx));
        case نوع_أساسي::عش32:
            return نوع(llvm::Type::getFloatTy(*ctx));
        case نوع_أساسي::عش64:
            return نوع(llvm::Type::getDoubleTy(*ctx));
        case نوع_أساسي::فراغ:
            return نوع(llvm::Type::getVoidTy(*ctx));
        case نوع_أساسي::مؤشر:
            return نوع(llvm::PointerType::get(*ctx, 0));
    }
#endif
    return نوع();
}

نوع نوع::أنشئ_مصفوفة(نوع نوع_العنصر, size_t الحجم) {
#ifdef SAD_USE_LLVM
    if (نوع_العنصر.صالح()) {
        return نوع(llvm::ArrayType::get(نوع_العنصر.خام(), الحجم));
    }
#endif
    return نوع();
}

نوع نوع::أنشئ_مؤشر(void* سياق) {
#ifdef SAD_USE_LLVM
    auto* ctx = static_cast<llvm::LLVMContext*>(سياق);
    return نوع(llvm::PointerType::get(*ctx, 0));
#else
    return نوع();
#endif
}

نوع نوع::أنشئ_هيكل(const std::vector<نوع>& الحقول, bool معبأ) {
#ifdef SAD_USE_LLVM
    if (الحقول.empty()) return نوع();
    
    std::vector<llvm::Type*> llvm_types;
    for (const auto& حقل : الحقول) {
        if (!حقل.صالح()) return نوع();
        llvm_types.push_back(حقل.خام());
    }
    
    auto* ctx = &llvm_types[0]->getContext();
    return نوع(llvm::StructType::get(*ctx, llvm_types, معبأ));
#else
    return نوع();
#endif
}

نوع نوع::أنشئ_دالة(نوع نوع_الإرجاع, const std::vector<نوع>& المعطيات, bool متغير) {
#ifdef SAD_USE_LLVM
    if (!نوع_الإرجاع.صالح()) return نوع();
    
    std::vector<llvm::Type*> param_types;
    for (const auto& معطى : المعطيات) {
        if (!معطى.صالح()) return نوع();
        param_types.push_back(معطى.خام());
    }
    
    return نوع(llvm::FunctionType::get(نوع_الإرجاع.خام(), param_types, متغير));
#else
    return نوع();
#endif
}

bool نوع::صالح() const {
#ifdef SAD_USE_LLVM
    return m_نوع != nullptr;
#else
    return false;
#endif
}

bool نوع::هو_عدد_صحيح() const {
#ifdef SAD_USE_LLVM
    return m_نوع && m_نوع->isIntegerTy();
#else
    return false;
#endif
}

bool نوع::هو_عشري() const {
#ifdef SAD_USE_LLVM
    return m_نوع && m_نوع->isFloatingPointTy();
#else
    return false;
#endif
}

bool نوع::هو_مؤشر() const {
#ifdef SAD_USE_LLVM
    return m_نوع && m_نوع->isPointerTy();
#else
    return false;
#endif
}

bool نوع::هو_مصفوفة() const {
#ifdef SAD_USE_LLVM
    return m_نوع && m_نوع->isArrayTy();
#else
    return false;
#endif
}

bool نوع::هو_هيكل() const {
#ifdef SAD_USE_LLVM
    return m_نوع && m_نوع->isStructTy();
#else
    return false;
#endif
}

bool نوع::هو_فراغ() const {
#ifdef SAD_USE_LLVM
    return m_نوع && m_نوع->isVoidTy();
#else
    return false;
#endif
}

bool نوع::هو_دالة() const {
#ifdef SAD_USE_LLVM
    return m_نوع && m_نوع->isFunctionTy();
#else
    return false;
#endif
}

size_t نوع::حجم_بالبت() const {
#ifdef SAD_USE_LLVM
    if (!m_نوع) return 0;
    if (m_نوع->isIntegerTy()) {
        return m_نوع->getIntegerBitWidth();
    }
    // للأنواع الأخرى، نحتاج DataLayout
    return 0;
#else
    return 0;
#endif
}

std::string نوع::إلى_نص() const {
#ifdef SAD_USE_LLVM
    if (!m_نوع) return "<null>";
    
    std::string str;
    llvm::raw_string_ostream os(str);
    m_نوع->print(os);
    return os.str();
#else
    return "<no-llvm>";
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              تنفيذ فئة الكتلة الأساسية
// ═══════════════════════════════════════════════════════════════════════════════

bool كتلة_أساسية::صالحة() const {
#ifdef SAD_USE_LLVM
    return m_كتلة != nullptr;
#else
    return false;
#endif
}

std::string كتلة_أساسية::اسم() const {
#ifdef SAD_USE_LLVM
    if (m_كتلة) {
        return std::string(m_كتلة->getName());
    }
#endif
    return "";
}

bool كتلة_أساسية::لها_منهي() const {
#ifdef SAD_USE_LLVM
    return m_كتلة && m_كتلة->getTerminator() != nullptr;
#else
    return false;
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              تنفيذ فئة الدالة
// ═══════════════════════════════════════════════════════════════════════════════

bool دالة::صالحة() const {
#ifdef SAD_USE_LLVM
    return m_دالة != nullptr;
#else
    return false;
#endif
}

std::string دالة::اسم() const {
#ifdef SAD_USE_LLVM
    if (m_دالة) {
        return std::string(m_دالة->getName());
    }
#endif
    return "";
}

نوع دالة::نوع_الإرجاع() const {
#ifdef SAD_USE_LLVM
    if (m_دالة) {
        return نوع(m_دالة->getReturnType());
    }
#endif
    return نوع();
}

size_t دالة::عدد_المعطيات() const {
#ifdef SAD_USE_LLVM
    return m_دالة ? m_دالة->arg_size() : 0;
#else
    return 0;
#endif
}

قيمة دالة::معطى(size_t الفهرس) const {
#ifdef SAD_USE_LLVM
    if (m_دالة && الفهرس < m_دالة->arg_size()) {
        return قيمة(m_دالة->getArg(الفهرس));
    }
#endif
    return قيمة();
}

std::vector<قيمة> دالة::المعطيات() const {
    std::vector<قيمة> نتيجة;
#ifdef SAD_USE_LLVM
    if (m_دالة) {
        for (auto& arg : m_دالة->args()) {
            نتيجة.push_back(قيمة(&arg));
        }
    }
#endif
    return نتيجة;
}

كتلة_أساسية دالة::أنشئ_كتلة(const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_دالة) {
        auto* block = llvm::BasicBlock::Create(
            m_دالة->getContext(),
            اسم,
            m_دالة
        );
        return كتلة_أساسية(block);
    }
#endif
    return كتلة_أساسية();
}

كتلة_أساسية دالة::كتلة_الدخول() const {
#ifdef SAD_USE_LLVM
    if (m_دالة && !m_دالة->empty()) {
        return كتلة_أساسية(&m_دالة->getEntryBlock());
    }
#endif
    return كتلة_أساسية();
}

size_t دالة::عدد_الكتل() const {
#ifdef SAD_USE_LLVM
    return m_دالة ? m_دالة->size() : 0;
#else
    return 0;
#endif
}

void دالة::عيِّن_مضمنة_دائماً() {
#ifdef SAD_USE_LLVM
    if (m_دالة) {
        m_دالة->addFnAttr(llvm::Attribute::AlwaysInline);
    }
#endif
}

void دالة::عيِّن_بدون_استثناءات() {
#ifdef SAD_USE_LLVM
    if (m_دالة) {
        m_دالة->addFnAttr(llvm::Attribute::NoUnwind);
    }
#endif
}

bool دالة::تحقق() const {
#ifdef SAD_USE_LLVM
    if (!m_دالة) return false;
    return !llvm::verifyFunction(*m_دالة, &llvm::errs());
#else
    return false;
#endif
}

std::string دالة::إلى_ir() const {
#ifdef SAD_USE_LLVM
    if (!m_دالة) return "";
    
    std::string str;
    llvm::raw_string_ostream os(str);
    m_دالة->print(os);
    return os.str();
#else
    return "";
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              تنفيذ فئة مُنشئ IR
// ═══════════════════════════════════════════════════════════════════════════════

#ifdef SAD_USE_LLVM
مُنشئ_IR::مُنشئ_IR(llvm::LLVMContext& سياق)
    : m_builder(std::make_unique<llvm::IRBuilder<>>(سياق)) {}
#endif

void مُنشئ_IR::عيِّن_موضع_الإدراج(كتلة_أساسية كتلة) {
#ifdef SAD_USE_LLVM
    if (m_builder && كتلة.صالحة()) {
        m_builder->SetInsertPoint(كتلة.خام());
    }
#endif
}

كتلة_أساسية مُنشئ_IR::الكتلة_الحالية() const {
#ifdef SAD_USE_LLVM
    if (m_builder) {
        return كتلة_أساسية(m_builder->GetInsertBlock());
    }
#endif
    return كتلة_أساسية();
}

// ─────────────────────────────────────────────────────────────────────────────
// الثوابت
// ─────────────────────────────────────────────────────────────────────────────

قيمة مُنشئ_IR::أنشئ_ثابت_صحيح(نوع النوع, int64_t القيمة) {
#ifdef SAD_USE_LLVM
    if (النوع.صالح() && النوع.هو_عدد_صحيح()) {
        return قيمة(llvm::ConstantInt::get(النوع.خام(), القيمة, true));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_ثابت_عشري(نوع النوع, double القيمة) {
#ifdef SAD_USE_LLVM
    if (النوع.صالح() && النوع.هو_عشري()) {
        return قيمة(llvm::ConstantFP::get(النوع.خام(), القيمة));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_ثابت_نص(const std::string& النص) {
#ifdef SAD_USE_LLVM
    if (m_builder) {
        return قيمة(m_builder->CreateGlobalStringPtr(النص));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_null(نوع النوع) {
#ifdef SAD_USE_LLVM
    if (النوع.صالح()) {
        return قيمة(llvm::Constant::getNullValue(النوع.خام()));
    }
#endif
    return قيمة();
}

// ─────────────────────────────────────────────────────────────────────────────
// العمليات الحسابية
// ─────────────────────────────────────────────────────────────────────────────

قيمة مُنشئ_IR::أنشئ_جمع(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateAdd(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_طرح(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateSub(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_ضرب(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateMul(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_قسمة(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateSDiv(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_باقي(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateSRem(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_سالب(قيمة القيمة, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && القيمة.صالحة()) {
        return قيمة(m_builder->CreateNeg(القيمة.خام(), اسم));
    }
#endif
    return قيمة();
}

// ─────────────────────────────────────────────────────────────────────────────
// العمليات العشرية
// ─────────────────────────────────────────────────────────────────────────────

قيمة مُنشئ_IR::أنشئ_جمع_عشري(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateFAdd(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_طرح_عشري(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateFSub(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_ضرب_عشري(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateFMul(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_قسمة_عشري(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateFDiv(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

// ─────────────────────────────────────────────────────────────────────────────
// العمليات المنطقية
// ─────────────────────────────────────────────────────────────────────────────

قيمة مُنشئ_IR::أنشئ_و(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateAnd(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_أو(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateOr(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_نفي(قيمة القيمة, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && القيمة.صالحة()) {
        return قيمة(m_builder->CreateNot(القيمة.خام(), اسم));
    }
#endif
    return قيمة();
}

// ─────────────────────────────────────────────────────────────────────────────
// المقارنات
// ─────────────────────────────────────────────────────────────────────────────

قيمة مُنشئ_IR::أنشئ_مقارنة_تساوي(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateICmpEQ(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_مقارنة_أقل(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateICmpSLT(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_مقارنة_أكبر(قيمة يسار, قيمة يمين, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && يسار.صالحة() && يمين.صالحة()) {
        return قيمة(m_builder->CreateICmpSGT(يسار.خام(), يمين.خام(), اسم));
    }
#endif
    return قيمة();
}

// ─────────────────────────────────────────────────────────────────────────────
// الذاكرة
// ─────────────────────────────────────────────────────────────────────────────

قيمة مُنشئ_IR::أنشئ_حجز(نوع النوع, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && النوع.صالح()) {
        return قيمة(m_builder->CreateAlloca(النوع.خام(), nullptr, اسم));
    }
#endif
    return قيمة();
}

قيمة مُنشئ_IR::أنشئ_تحميل(نوع النوع, قيمة مؤشر, const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && النوع.صالح() && مؤشر.صالحة()) {
        return قيمة(m_builder->CreateLoad(النوع.خام(), مؤشر.خام(), اسم));
    }
#endif
    return قيمة();
}

void مُنشئ_IR::أنشئ_تخزين(قيمة القيمة, قيمة مؤشر) {
#ifdef SAD_USE_LLVM
    if (m_builder && القيمة.صالحة() && مؤشر.صالحة()) {
        m_builder->CreateStore(القيمة.خام(), مؤشر.خام());
    }
#endif
}

// ─────────────────────────────────────────────────────────────────────────────
// التحكم في التدفق
// ─────────────────────────────────────────────────────────────────────────────

void مُنشئ_IR::أنشئ_قفزة(كتلة_أساسية الهدف) {
#ifdef SAD_USE_LLVM
    if (m_builder && الهدف.صالحة()) {
        m_builder->CreateBr(الهدف.خام());
    }
#endif
}

void مُنشئ_IR::أنشئ_قفزة_مشروطة(قيمة شرط, كتلة_أساسية إذا_صحيح, كتلة_أساسية إذا_خطأ) {
#ifdef SAD_USE_LLVM
    if (m_builder && شرط.صالحة() && إذا_صحيح.صالحة() && إذا_خطأ.صالحة()) {
        m_builder->CreateCondBr(شرط.خام(), إذا_صحيح.خام(), إذا_خطأ.خام());
    }
#endif
}

void مُنشئ_IR::أنشئ_رجوع(قيمة القيمة) {
#ifdef SAD_USE_LLVM
    if (m_builder && القيمة.صالحة()) {
        m_builder->CreateRet(القيمة.خام());
    }
#endif
}

void مُنشئ_IR::أنشئ_رجوع_فراغ() {
#ifdef SAD_USE_LLVM
    if (m_builder) {
        m_builder->CreateRetVoid();
    }
#endif
}

// ─────────────────────────────────────────────────────────────────────────────
// استدعاء الدوال
// ─────────────────────────────────────────────────────────────────────────────

قيمة مُنشئ_IR::أنشئ_استدعاء(دالة الدالة, 
                            const std::vector<قيمة>& المعطيات,
                            const std::string& اسم) {
#ifdef SAD_USE_LLVM
    if (m_builder && الدالة.صالحة()) {
        std::vector<llvm::Value*> args;
        for (const auto& arg : المعطيات) {
            if (!arg.صالحة()) return قيمة();
            args.push_back(arg.خام());
        }
        
        return قيمة(m_builder->CreateCall(الدالة.خام(), args, اسم));
    }
#endif
    return قيمة();
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              تنفيذ فئة الوحدة
// ═══════════════════════════════════════════════════════════════════════════════

std::unique_ptr<وحدة> وحدة::أنشئ(const std::string& اسم) {
    auto module = std::make_unique<وحدة>();
    
#ifdef SAD_USE_LLVM
    module->m_context = std::make_unique<llvm::LLVMContext>();
    module->m_module = std::make_unique<llvm::Module>(اسم, *module->m_context);
    module->m_builder = std::make_unique<مُنشئ_IR>(*module->m_context);
#endif
    
    return module;
}

دالة وحدة::أنشئ_دالة(const std::string& اسم,
                     نوع نوع_الإرجاع,
                     const std::vector<نوع>& أنواع_المعطيات,
                     bool متغيرة_المعطيات,
                     دالة::ربط الربط) {
#ifdef SAD_USE_LLVM
    if (!m_module || !نوع_الإرجاع.صالح()) {
        return دالة();
    }
    
    std::vector<llvm::Type*> param_types;
    for (const auto& type : أنواع_المعطيات) {
        if (!type.صالح()) return دالة();
        param_types.push_back(type.خام());
    }
    
    auto* func_type = llvm::FunctionType::get(
        نوع_الإرجاع.خام(),
        param_types,
        متغيرة_المعطيات
    );
    
    llvm::GlobalValue::LinkageTypes linkage;
    switch (الربط) {
        case دالة::ربط::خارجي:
            linkage = llvm::GlobalValue::ExternalLinkage;
            break;
        case دالة::ربط::داخلي:
            linkage = llvm::GlobalValue::InternalLinkage;
            break;
        case دالة::ربط::خاص:
            linkage = llvm::GlobalValue::PrivateLinkage;
            break;
        case دالة::ربط::ضعيف:
            linkage = llvm::GlobalValue::WeakAnyLinkage;
            break;
    }
    
    auto* func = llvm::Function::Create(
        func_type,
        linkage,
        اسم,
        m_module.get()
    );
    
    return دالة(func);
#else
    return دالة();
#endif
}

std::optional<دالة> وحدة::احصل_على_دالة(const std::string& اسم) const {
#ifdef SAD_USE_LLVM
    if (m_module) {
        if (auto* func = m_module->getFunction(اسم)) {
            return دالة(func);
        }
    }
#endif
    return std::nullopt;
}

نوع وحدة::احصل_نوع_أساسي(نوع_أساسي النوع) {
#ifdef SAD_USE_LLVM
    if (m_context) {
        return نوع::أنشئ_أساسي(النوع, m_context.get());
    }
#endif
    return نوع();
}

نوع وحدة::نوع_مؤشر() {
#ifdef SAD_USE_LLVM
    if (m_context) {
        return نوع::أنشئ_مؤشر(m_context.get());
    }
#endif
    return نوع();
}

void* وحدة::السياق() {
#ifdef SAD_USE_LLVM
    return m_context.get();
#else
    return nullptr;
#endif
}

مُنشئ_IR& وحدة::المُنشئ() {
    return *m_builder;
}

bool وحدة::تحقق() const {
#ifdef SAD_USE_LLVM
    if (!m_module) return false;
    return !llvm::verifyModule(*m_module, &llvm::errs());
#else
    return false;
#endif
}

std::string وحدة::إلى_ir() const {
#ifdef SAD_USE_LLVM
    if (!m_module) return "";
    
    std::string str;
    llvm::raw_string_ostream os(str);
    m_module->print(os, nullptr);
    return os.str();
#else
    return "";
#endif
}

bool وحدة::اكتب_ir(const std::string& مسار) const {
#ifdef SAD_USE_LLVM
    if (!m_module) return false;
    
    std::error_code ec;
    llvm::raw_fd_ostream os(مسار, ec);
    if (ec) return false;
    
    m_module->print(os, nullptr);
    return true;
#else
    return false;
#endif
}

bool وحدة::اكتب_bitcode(const std::string& مسار) const {
#ifdef SAD_USE_LLVM
    if (!m_module) return false;
    
    std::error_code ec;
    llvm::raw_fd_ostream os(مسار, ec, llvm::sys::fs::OF_None);
    if (ec) return false;
    
    llvm::WriteBitcodeToFile(*m_module, os);
    return true;
#else
    return false;
#endif
}

void وحدة::عيِّن_triple(const std::string& triple) {
#ifdef SAD_USE_LLVM
    if (m_module) {
        m_module->setTargetTriple(triple);
    }
#endif
}

void وحدة::عيِّن_data_layout(const std::string& layout) {
#ifdef SAD_USE_LLVM
    if (m_module) {
        m_module->setDataLayout(layout);
    }
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              الدوال المساعدة
// ═══════════════════════════════════════════════════════════════════════════════

namespace مساعد {

void هيّئ_الأهداف() {
#ifdef SAD_USE_LLVM
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
#endif
}

std::string triple_محلي() {
#ifdef SAD_USE_LLVM
    return llvm::sys::getDefaultTargetTriple();
#else
    return "";
#endif
}

std::string إصدار_llvm() {
#ifdef SAD_USE_LLVM
    return LLVM_VERSION_STRING;
#else
    return "غير متوفر";
#endif
}

bool هدف_مدعوم(منصة_هدف الهدف) {
#ifdef SAD_USE_LLVM
    // التحقق من دعم الهدف
    std::string triple;
    switch (الهدف) {
        case منصة_هدف::محلي:
            return true;
        case منصة_هدف::x86_64_windows:
            triple = "x86_64-pc-windows-msvc";
            break;
        case منصة_هدف::x86_64_linux:
            triple = "x86_64-unknown-linux-gnu";
            break;
        case منصة_هدف::x86_64_macos:
            triple = "x86_64-apple-darwin";
            break;
        case منصة_هدف::aarch64_linux:
            triple = "aarch64-unknown-linux-gnu";
            break;
        case منصة_هدف::aarch64_macos:
            triple = "arm64-apple-darwin";
            break;
        case منصة_هدف::wasm32:
            triple = "wasm32-unknown-unknown";
            break;
        case منصة_هدف::wasm64:
            triple = "wasm64-unknown-unknown";
            break;
        case منصة_هدف::riscv64:
            triple = "riscv64-unknown-linux-gnu";
            break;
    }
    
    std::string error;
    auto* target = llvm::TargetRegistry::lookupTarget(triple, error);
    return target != nullptr;
#else
    return false;
#endif
}

} // namespace مساعد

} // namespace llvm_wrapper
} // namespace sad
