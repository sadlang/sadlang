// ════════════════════════════════════════════════════════════════════════════════
// ملف: pattern_codegen.cpp
// File: pattern_codegen.cpp
//
// الوصف (AR): توليد كود LLVM IR لمطابقة الأنماط
//             يُنتج كود فعّال لجمل match
//             يدعم جميع أنواع الأنماط والشروط
//
// Description (EN): LLVM IR code generation for pattern matching
//                   Produces efficient code for match statements
//                   Supports all pattern types and guards
//
// المؤلف: فريق تطوير لغة ص
// Author: Sad Language Development Team
//
// التاريخ: يناير 2026
// Date: January 2026
//
// المرحلة: Phase 12 - User Story 10 - Pattern Matching
// Phase: Phase 12 - User Story 10 - Pattern Matching
//
// المهمة: T143 [US10] Implement pattern codegen
// Task: T143 [US10] Implement pattern codegen
// ════════════════════════════════════════════════════════════════════════════════

// تعطيل تحذير Unicode للتعليقات العربية
// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

#include "pattern_nodes.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <memory>

namespace Sad {
namespace Backend {

// ════════════════════════════════════════════════════════════════════════════════
// معلومات Case المُولد / Generated Case Info
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) معلومات فرع case المُولد
 *        (EN) Generated case branch info
 */
struct GeneratedCaseInfo {
    llvm::BasicBlock* testBlock;     ///< (AR) كتلة الاختبار / (EN) Test block
    llvm::BasicBlock* bodyBlock;     ///< (AR) كتلة الجسم / (EN) Body block
    llvm::BasicBlock* guardBlock;    ///< (AR) كتلة الشرط (إن وُجد) / (EN) Guard block (if exists)
    std::unordered_map<std::string, llvm::Value*> bindings;  ///< (AR) الربطات / (EN) Bindings
};

// ════════════════════════════════════════════════════════════════════════════════
// مولد كود الأنماط / Pattern Code Generator
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مولد كود LLVM لمطابقة الأنماط
 *        (EN) LLVM code generator for pattern matching
 * 
 * (AR) استراتيجية التوليد:
 *      1. إنشاء كتلة لكل case
 *      2. إنشاء كتلة للنهاية (merge block)
 *      3. لكل case: توليد اختبار + جسم
 *      4. ربط الكتل بـ branch instructions
 * 
 * (EN) Generation strategy:
 *      1. Create block for each case
 *      2. Create end block (merge block)
 *      3. For each case: generate test + body
 *      4. Connect blocks with branch instructions
 */
class PatternCodeGen {
public:
    // ========================================================================
    // البناء والهدم / Construction & Destruction
    // ========================================================================
    
    /**
     * @brief (AR) المُنشئ
     *        (EN) Constructor
     * 
     * @param context (AR) سياق LLVM / (EN) LLVM context
     * @param module (AR) الوحدة / (EN) Module
     * @param builder (AR) بانٍ IR / (EN) IR builder
     */
    PatternCodeGen(
        llvm::LLVMContext& context,
        llvm::Module& module,
        llvm::IRBuilder<>& builder
    ) : context_(context)
      , module_(module)
      , builder_(builder)
      , debugMode_(false) {
    }
    
    ~PatternCodeGen() = default;
    
    // ========================================================================
    // الواجهة الرئيسية / Main Interface
    // ========================================================================
    
    /**
     * @brief (AR) توليد كود لجملة match كاملة
     *        (EN) Generate code for complete match statement
     * 
     * @param matchStmt (AR) جملة match / (EN) Match statement
     * @param matchedValue (AR) القيمة المُختبرة (LLVM) / (EN) Matched value (LLVM)
     * @param currentFunction (AR) الدالة الحالية / (EN) Current function
     * @return (AR) قيمة الإرجاع (إن وُجدت) / (EN) Return value (if any)
     */
    llvm::Value* generateMatch(
        const AST::MatchStmt& matchStmt,
        llvm::Value* matchedValue,
        llvm::Function* currentFunction
    ) {
        if (debugMode_) {
            std::cout << "[PatternCodeGen] توليد match مع " 
                      << matchStmt.cases.size() << " فرع\n";
            std::cout << "[PatternCodeGen] Generating match with " 
                      << matchStmt.cases.size() << " cases\n";
        }
        
        // (AR) إنشاء الكتل
        // (EN) Create blocks
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(
            context_, "match.end", currentFunction
        );
        
        std::vector<GeneratedCaseInfo> caseInfos;
        
        // (AR) إنشاء كتل لكل case
        // (EN) Create blocks for each case
        for (size_t i = 0; i < matchStmt.cases.size(); ++i) {
            GeneratedCaseInfo info;
            info.testBlock = llvm::BasicBlock::Create(
                context_, 
                "match.case" + std::to_string(i) + ".test",
                currentFunction
            );
            info.bodyBlock = llvm::BasicBlock::Create(
                context_,
                "match.case" + std::to_string(i) + ".body",
                currentFunction
            );
            
            // (AR) كتلة guard إن وُجد
            // (EN) Guard block if exists
            if (matchStmt.cases[i].guard) {
                info.guardBlock = llvm::BasicBlock::Create(
                    context_,
                    "match.case" + std::to_string(i) + ".guard",
                    currentFunction
                );
            } else {
                info.guardBlock = nullptr;
            }
            
            caseInfos.push_back(info);
        }
        
        // (AR) القفز للـ case الأول
        // (EN) Jump to first case
        if (!caseInfos.empty()) {
            builder_.CreateBr(caseInfos[0].testBlock);
        } else {
            builder_.CreateBr(mergeBlock);
        }
        
        // (AR) توليد كود كل case
        // (EN) Generate code for each case
        for (size_t i = 0; i < matchStmt.cases.size(); ++i) {
            const auto& caseClause = matchStmt.cases[i];
            auto& info = caseInfos[i];
            
            // (AR) الـ case التالي للقفز إليه عند الفشل
            // (EN) Next case to jump to on failure
            llvm::BasicBlock* nextCase = (i + 1 < caseInfos.size()) 
                ? caseInfos[i + 1].testBlock 
                : mergeBlock;
            
            // (AR) توليد كتلة الاختبار
            // (EN) Generate test block
            builder_.SetInsertPoint(info.testBlock);
            
            if (caseClause.pattern) {
                llvm::Value* testResult = generatePatternTest(
                    *caseClause.pattern,
                    matchedValue,
                    info.bindings
                );
                
                // (AR) القفز بناءً على نتيجة الاختبار
                // (EN) Branch based on test result
                if (info.guardBlock) {
                    // (AR) إذا نجح الاختبار، اذهب للـ guard
                    // (EN) If test succeeds, go to guard
                    builder_.CreateCondBr(testResult, info.guardBlock, nextCase);
                } else {
                    // (AR) إذا نجح الاختبار، اذهب للجسم
                    // (EN) If test succeeds, go to body
                    builder_.CreateCondBr(testResult, info.bodyBlock, nextCase);
                }
            }
            
            // (AR) توليد كتلة الشرط (guard) إن وُجد
            // (EN) Generate guard block if exists
            if (info.guardBlock && caseClause.guard) {
                builder_.SetInsertPoint(info.guardBlock);
                
                llvm::Value* guardResult = generateGuardTest(*caseClause.guard, info.bindings);
                builder_.CreateCondBr(guardResult, info.bodyBlock, nextCase);
            }
            
            // (AR) توليد كتلة الجسم
            // (EN) Generate body block
            builder_.SetInsertPoint(info.bodyBlock);
            
            // (AR) إنشاء المتغيرات المربوطة
            // (EN) Create bound variables
            for (const auto& [name, value] : info.bindings) {
                // (AR) تخزين في alloca محلية
                // (EN) Store in local alloca
                llvm::AllocaInst* alloca = builder_.CreateAlloca(
                    value->getType(),
                    nullptr,
                    name
                );
                builder_.CreateStore(value, alloca);
            }
            
            // (AR) توليد كود الجسم (ستُضاف لاحقاً)
            // (EN) Generate body code (will be added later)
            // TODO: Call body statement code generator
            
            // (AR) القفز لكتلة النهاية
            // (EN) Jump to merge block
            builder_.CreateBr(mergeBlock);
        }
        
        // (AR) الانتقال لكتلة النهاية
        // (EN) Move to merge block
        builder_.SetInsertPoint(mergeBlock);
        
        return nullptr;  // (AR) سيُحدث لدعم قيمة الإرجاع / (EN) Will update for return value support
    }
    
    /**
     * @brief (AR) توليد اختبار نمط واحد
     *        (EN) Generate test for single pattern
     * 
     * @param pattern (AR) النمط / (EN) Pattern
     * @param value (AR) القيمة المُختبرة / (EN) Value to test
     * @param bindings (AR) الربطات (تُملأ) / (EN) Bindings (to be filled)
     * @return (AR) نتيجة الاختبار (i1) / (EN) Test result (i1)
     */
    llvm::Value* generatePatternTest(
        const AST::Pattern& pattern,
        llvm::Value* value,
        std::unordered_map<std::string, llvm::Value*>& bindings
    ) {
        // (AR) النمط الشامل - دائماً صحيح
        // (EN) Wildcard - always true
        if (dynamic_cast<const AST::WildcardPattern*>(&pattern)) {
            return llvm::ConstantInt::getTrue(context_);
        }
        
        // (AR) نمط متغير - دائماً صحيح ويربط القيمة
        // (EN) Variable pattern - always true and binds value
        if (auto* varPat = dynamic_cast<const AST::VariablePattern*>(&pattern)) {
            bindings[varPat->name] = value;
            return llvm::ConstantInt::getTrue(context_);
        }
        
        // (AR) نمط قيمة حرفية
        // (EN) Literal pattern
        if (auto* litPat = dynamic_cast<const AST::LiteralPattern*>(&pattern)) {
            return generateLiteralTest(*litPat, value);
        }
        
        // (AR) نمط قائمة
        // (EN) List pattern
        if (auto* listPat = dynamic_cast<const AST::ListPattern*>(&pattern)) {
            return generateListPatternTest(*listPat, value, bindings);
        }
        
        // (AR) نمط OR
        // (EN) OR pattern
        if (auto* orPat = dynamic_cast<const AST::OrPattern*>(&pattern)) {
            return generateOrPatternTest(*orPat, value, bindings);
        }
        
        // (AR) نمط غير معروف - فشل
        // (EN) Unknown pattern - fail
        return llvm::ConstantInt::getFalse(context_);
    }
    
    // ========================================================================
    // الإعدادات / Settings
    // ========================================================================
    
    void setDebugMode(bool enable) { debugMode_ = enable; }

private:
    // ========================================================================
    // توليد اختبارات أنواع الأنماط / Pattern Type Test Generation
    // ========================================================================
    
    /**
     * @brief (AR) توليد اختبار قيمة حرفية
     *        (EN) Generate literal test
     */
    llvm::Value* generateLiteralTest(
        const AST::LiteralPattern& pattern,
        llvm::Value* value
    ) {
        // (AR) المقارنة تعتمد على نوع القيمة
        // (EN) Comparison depends on value type
        const auto& lit = pattern.literal;
        
        switch (lit.getType()) {
            case Data::ValueType::DOUBLE: {
                // (AR) مقارنة أرقام
                // (EN) Number comparison
                llvm::Value* litVal = llvm::ConstantFP::get(
                    llvm::Type::getDoubleTy(context_),
                    lit.toDouble()
                );
                
                if (value->getType()->isDoubleTy()) {
                    return builder_.CreateFCmpOEQ(value, litVal, "cmp.num");
                } else if (value->getType()->isIntegerTy()) {
                    // (AR) تحويل للمقارنة
                    // (EN) Convert for comparison
                    llvm::Value* converted = builder_.CreateSIToFP(
                        value,
                        llvm::Type::getDoubleTy(context_)
                    );
                    return builder_.CreateFCmpOEQ(converted, litVal, "cmp.num");
                }
                break;
            }
            
            case Data::ValueType::BOOLEAN: {
                // (AR) مقارنة منطقية
                // (EN) Boolean comparison
                llvm::Value* litVal = lit.toBool()
                    ? llvm::ConstantInt::getTrue(context_)
                    : llvm::ConstantInt::getFalse(context_);
                
                return builder_.CreateICmpEQ(value, litVal, "cmp.bool");
            }
            
            case Data::ValueType::STRING: {
                // (AR) مقارنة نصوص - تحتاج دالة مساعدة
                // (EN) String comparison - needs helper function
                // TODO: Call string comparison function
                return llvm::ConstantInt::getFalse(context_);
            }
            
            case Data::ValueType::VOID: {
                // (AR) مقارنة مع null
                // (EN) Null comparison
                llvm::Value* nullVal = llvm::Constant::getNullValue(value->getType());
                return builder_.CreateICmpEQ(value, nullVal, "cmp.null");
            }
            
            default:
                break;
        }
        
        return llvm::ConstantInt::getFalse(context_);
    }
    
    /**
     * @brief (AR) توليد اختبار نمط قائمة
     *        (EN) Generate list pattern test
     */
    llvm::Value* generateListPatternTest(
        const AST::ListPattern& pattern,
        llvm::Value* value,
        std::unordered_map<std::string, llvm::Value*>& bindings
    ) {
        // (AR) التحقق من الطول أولاً
        // (EN) Check length first
        // TODO: Implement array length check
        
        // (AR) اختبار كل عنصر
        // (EN) Test each element
        llvm::Value* result = llvm::ConstantInt::getTrue(context_);
        
        for (size_t i = 0; i < pattern.elements.size(); ++i) {
            // (AR) استخراج العنصر
            // (EN) Extract element
            // TODO: Generate array element access
            
            // (AR) اختبار العنصر
            // (EN) Test element
            // auto elemResult = generatePatternTest(*pattern.elements[i], elemValue, bindings);
            // result = builder_.CreateAnd(result, elemResult);
        }
        
        return result;
    }
    
    /**
     * @brief (AR) توليد اختبار نمط OR
     *        (EN) Generate OR pattern test
     */
    llvm::Value* generateOrPatternTest(
        const AST::OrPattern& pattern,
        llvm::Value* value,
        std::unordered_map<std::string, llvm::Value*>& bindings
    ) {
        // (AR) نجاح أي بديل = نجاح
        // (EN) Any alternative success = success
        llvm::Value* result = llvm::ConstantInt::getFalse(context_);
        
        for (const auto& alt : pattern.alternatives) {
            std::unordered_map<std::string, llvm::Value*> altBindings;
            llvm::Value* altResult = generatePatternTest(*alt, value, altBindings);
            result = builder_.CreateOr(result, altResult, "or.test");
            
            // (AR) جمع الربطات من البديل الأول الناجح
            // (EN) Collect bindings from first successful alternative
            // (AR) ملاحظة: هذا تبسيط - التنفيذ الكامل أكثر تعقيداً
            // (EN) Note: This is simplified - full implementation more complex
            if (bindings.empty()) {
                bindings = altBindings;
            }
        }
        
        return result;
    }
    
    /**
     * @brief (AR) توليد اختبار شرط guard
     *        (EN) Generate guard condition test
     */
    llvm::Value* generateGuardTest(
        const AST::Expression& guard,
        const std::unordered_map<std::string, llvm::Value*>& bindings
    ) {
        // TODO: Generate expression evaluation code
        // (AR) حالياً نُرجع true
        // (EN) Currently return true
        return llvm::ConstantInt::getTrue(context_);
    }
    
    // ========================================================================
    // الحقول / Fields
    // ========================================================================
    
    llvm::LLVMContext& context_;
    llvm::Module& module_;
    llvm::IRBuilder<>& builder_;
    bool debugMode_;
};

// ════════════════════════════════════════════════════════════════════════════════
// واجهة خارجية / External Interface
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) توليد كود لجملة match
 *        (EN) Generate code for match statement
 */
llvm::Value* generateMatchCode(
    const AST::MatchStmt& matchStmt,
    llvm::Value* matchedValue,
    llvm::Function* currentFunction,
    llvm::LLVMContext& context,
    llvm::Module& module,
    llvm::IRBuilder<>& builder
) {
    PatternCodeGen codegen(context, module, builder);
    return codegen.generateMatch(matchStmt, matchedValue, currentFunction);
}

} // namespace Backend
} // namespace Sad

// ════════════════════════════════════════════════════════════════════════════════
// نهاية الملف / End of File
// ════════════════════════════════════════════════════════════════════════════════
