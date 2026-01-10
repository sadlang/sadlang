/*
 * ============================================================================
 * مدير تدفق التحكم في LLVM - ملف الرأس
 * LLVM Control Flow Manager - Header File
 * ============================================================================
 * 
 * هذا الملف يحتوي على مدير متقدم لتدفق التحكم في LLVM IR
 * This file contains an advanced control flow manager for LLVM IR
 * 
 * الميزات / Features:
 * - إنشاء جمل if/else مع دعم elif
 * - إنشاء حلقات while و for
 * - إنشاء جمل match/switch متقدمة
 * - إنشاء جمل try/catch (استثناءات)
 * - إدارة الكتل الأساسية تلقائياً
 * - إدارة PHI nodes للمتغيرات
 * - تحسين تدفق التحكم
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement
 * ============================================================================
 */

#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Instructions.h>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace Sad {
namespace LLVM {

// ============================================================================
// فئة LoopContext - سياق الحلقة / Loop Context Class
// ============================================================================

/**
 * سياق الحلقة - يحفظ معلومات الحلقة الحالية
 * Loop context - Stores current loop information
 * 
 * يُستخدم للتعامل مع جمل break و continue
 * Used for handling break and continue statements
 */
struct LoopContext {
    llvm::BasicBlock* headerBlock;    ///< كتلة رأس الحلقة / Loop header block
    llvm::BasicBlock* bodyBlock;      ///< كتلة جسم الحلقة / Loop body block
    llvm::BasicBlock* exitBlock;      ///< كتلة الخروج / Loop exit block
    llvm::BasicBlock* continueBlock;  ///< كتلة continue / Continue block
    
    /**
     * منشئ سياق الحلقة
     * Loop context constructor
     */
    LoopContext(llvm::BasicBlock* header, llvm::BasicBlock* body, 
                llvm::BasicBlock* exit, llvm::BasicBlock* cont)
        : headerBlock(header), bodyBlock(body), exitBlock(exit), continueBlock(cont) {}
};

// ============================================================================
// فئة PHITracker - متتبع PHI nodes
// ============================================================================

/**
 * PHITracker - يتتبع PHI nodes للمتغيرات
 * PHITracker - Tracks PHI nodes for variables
 * 
 * يُستخدم للجمع بين قيم من فروع مختلفة
 * Used to merge values from different branches
 */
class PHITracker {
public:
    /**
     * تسجيل PHI node جديد
     * Register new PHI node
     */
    void registerPHI(const std::string& varName, llvm::PHINode* phi);
    
    /**
     * إضافة قيمة إلى PHI node
     * Add value to PHI node
     */
    void addIncoming(const std::string& varName, llvm::Value* value, llvm::BasicBlock* block);
    
    /**
     * الحصول على PHI node لمتغير
     * Get PHI node for variable
     */
    llvm::PHINode* getPHI(const std::string& varName) const;
    
    /**
     * مسح جميع PHI nodes
     * Clear all PHI nodes
     */
    void clear();
    
private:
    std::unordered_map<std::string, llvm::PHINode*> phiNodes_;  ///< تخزين PHI nodes / PHI nodes storage
};

// ============================================================================
// فئة LLVMControlFlow - مدير تدفق التحكم / Control Flow Manager
// ============================================================================

/**
 * LLVMControlFlow - مدير تدفق التحكم في LLVM
 * LLVMControlFlow - Control flow manager for LLVM
 * 
 * المسؤوليات / Responsibilities:
 * - إنشاء جمل if/else/elif
 * - إنشاء حلقات while و for
 * - إنشاء جمل match/switch
 * - إنشاء جمل try/catch
 * - إدارة الكتل الأساسية
 * - إدارة PHI nodes
 * - تحسين تدفق التحكم
 * 
 * الاستخدام / Usage:
 * @code
 * llvm::LLVMContext context;
 * llvm::IRBuilder<> builder(context);
 * llvm::Function* func = ...;
 * 
 * LLVMControlFlow cf(context, builder, func);
 * 
 * // إنشاء if statement
 * llvm::Value* condition = ...;
 * cf.beginIf(condition);
 * // then branch code
 * cf.beginElse();
 * // else branch code
 * cf.endIf();
 * @endcode
 */
class LLVMControlFlow {
public:
    // ========================================================================
    // المنشئ والمدمر / Constructor & Destructor
    // ========================================================================
    
    /**
     * منشئ مدير تدفق التحكم
     * Control flow manager constructor
     * 
     * @param context سياق LLVM / LLVM context
     * @param builder بناء LLVM / LLVM builder
     * @param function الدالة الحالية / Current function
     */
    LLVMControlFlow(llvm::LLVMContext& context, 
                    llvm::IRBuilder<>& builder,
                    llvm::Function* function);
    
    /**
     * مدمر مدير تدفق التحكم
     * Control flow manager destructor
     */
    ~LLVMControlFlow() = default;
    
    // ========================================================================
    // جمل if/else / if/else Statements
    // ========================================================================
    
    /**
     * بدء جملة if
     * Begin if statement
     * 
     * @param condition شرط If / If condition
     */
    void beginIf(llvm::Value* condition);
    
    /**
     * بدء فرع else
     * Begin else branch
     */
    void beginElse();
    
    /**
     * بدء فرع elif (else if)
     * Begin elif (else if) branch
     * 
     * @param condition شرط elif / elif condition
     */
    void beginElif(llvm::Value* condition);
    
    /**
     * إنهاء جملة if
     * End if statement
     */
    void endIf();
    
    // ========================================================================
    // حلقات while / while Loops
    // ========================================================================
    
    /**
     * بدء حلقة while
     * Begin while loop
     * 
     * @param condition شرط الحلقة / Loop condition
     */
    void beginWhile(llvm::Value* condition);
    
    /**
     * إنهاء حلقة while
     * End while loop
     */
    void endWhile();
    
    // ========================================================================
    // حلقات for / for Loops
    // ========================================================================
    
    /**
     * بدء حلقة for
     * Begin for loop
     * 
     * @param init تهيئة المتغير / Variable initialization
     * @param condition شرط الحلقة / Loop condition
     * @param increment زيادة المتغير / Variable increment
     */
    void beginFor(llvm::Value* init, llvm::Value* condition, llvm::Value* increment);
    
    /**
     * إنهاء حلقة for
     * End for loop
     */
    void endFor();
    
    // ========================================================================
    // جمل match/switch / match/switch Statements
    // ========================================================================
    
    /**
     * بدء جملة match/switch
     * Begin match/switch statement
     * 
     * @param value القيمة المطلوب مطابقتها / Value to match
     * @param numCases عدد الحالات المتوقع / Expected number of cases
     */
    void beginMatch(llvm::Value* value, unsigned numCases = 10);
    
    /**
     * إضافة حالة case إلى match
     * Add case to match
     * 
     * @param caseValue قيمة الحالة / Case value
     */
    void addCase(llvm::Value* caseValue);
    
    /**
     * إضافة حالة default
     * Add default case
     */
    void addDefault();
    
    /**
     * إنهاء جملة match
     * End match statement
     */
    void endMatch();
    
    // ========================================================================
    // جمل try/catch / try/catch Statements
    // ========================================================================
    
    /**
     * بدء كتلة try
     * Begin try block
     */
    void beginTry();
    
    /**
     * بدء كتلة catch
     * Begin catch block
     * 
     * @param exceptionType نوع الاستثناء / Exception type
     */
    void beginCatch(llvm::Type* exceptionType);
    
    /**
     * إنهاء جملة try/catch
     * End try/catch statement
     */
    void endTry();
    
    // ========================================================================
    // جمل break و continue / break & continue Statements
    // ========================================================================
    
    /**
     * توليد جملة break
     * Generate break statement
     */
    void emitBreak();
    
    /**
     * توليد جملة continue
     * Generate continue statement
     */
    void emitContinue();
    
    // ========================================================================
    // إدارة الكتل الأساسية / Basic Block Management
    // ========================================================================
    
    /**
     * إنشاء كتلة أساسية جديدة
     * Create new basic block
     * 
     * @param name اسم الكتلة / Block name
     * @return الكتلة المُنشأة / Created block
     */
    llvm::BasicBlock* createBlock(const std::string& name);
    
    /**
     * الانتقال إلى كتلة
     * Move to block
     * 
     * @param block الكتلة المطلوب الانتقال إليها / Block to move to
     */
    void moveToBlock(llvm::BasicBlock* block);
    
    /**
     * الحصول على الكتلة الحالية
     * Get current block
     * 
     * @return الكتلة الحالية / Current block
     */
    llvm::BasicBlock* getCurrentBlock() const;
    
    // ========================================================================
    // إدارة PHI Nodes / PHI Node Management
    // ========================================================================
    
    /**
     * إنشاء PHI node
     * Create PHI node
     * 
     * @param type نوع PHI node / PHI node type
     * @param name اسم PHI node / PHI node name
     * @param numIncoming عدد القيم الواردة المتوقع / Expected incoming values
     * @return PHI node المُنشأ / Created PHI node
     */
    llvm::PHINode* createPHI(llvm::Type* type, const std::string& name, unsigned numIncoming = 2);
    
    /**
     * إضافة قيمة إلى PHI node
     * Add value to PHI node
     * 
     * @param phi PHI node المستهدف / Target PHI node
     * @param value القيمة المضافة / Value to add
     * @param block الكتلة المصدر / Source block
     */
    void addPHIIncoming(llvm::PHINode* phi, llvm::Value* value, llvm::BasicBlock* block);
    
    // ========================================================================
    // تحسين تدفق التحكم / Control Flow Optimization
    // ========================================================================
    
    /**
     * تحسين جملة if بسيطة
     * Optimize simple if statement
     * 
     * @param condition الشرط / Condition
     * @param thenValue قيمة then / Then value
     * @param elseValue قيمة else / Else value
     * @return قيمة محسّنة (select instruction) / Optimized value (select instruction)
     */
    llvm::Value* optimizeSimpleIf(llvm::Value* condition, llvm::Value* thenValue, llvm::Value* elseValue);
    
    /**
     * دمج كتل أساسية فارغة
     * Merge empty basic blocks
     */
    void mergeEmptyBlocks();
    
private:
    // ========================================================================
    // البيانات الخاصة / Private Data
    // ========================================================================
    
    llvm::LLVMContext& context_;       ///< سياق LLVM / LLVM context
    llvm::IRBuilder<>& builder_;       ///< بناء LLVM / LLVM builder
    llvm::Function* function_;         ///< الدالة الحالية / Current function
    
    /**
     * مكدس سياقات if/else
     * if/else context stack
     */
    struct IfContext {
        llvm::BasicBlock* thenBlock;   ///< كتلة then
        llvm::BasicBlock* elseBlock;   ///< كتلة else
        llvm::BasicBlock* mergeBlock;  ///< كتلة الدمج / Merge block
        bool hasElse;                  ///< هل يوجد else؟ / Has else?
    };
    std::vector<IfContext> ifStack_;   ///< مكدس if / if stack
    
    /**
     * مكدس سياقات الحلقات
     * Loop context stack
     */
    std::vector<LoopContext> loopStack_;  ///< مكدس الحلقات / Loop stack
    
    /**
     * سياق match الحالي
     * Current match context
     */
    struct MatchContext {
        llvm::SwitchInst* switchInst;  ///< تعليمة switch
        llvm::BasicBlock* defaultBlock;  ///< كتلة default
        llvm::BasicBlock* mergeBlock;    ///< كتلة الدمج / Merge block
        llvm::BasicBlock* currentCase;   ///< الحالة الحالية / Current case
    };
    std::unique_ptr<MatchContext> matchContext_;  ///< سياق match / Match context
    
    /**
     * متتبع PHI nodes
     * PHI node tracker
     */
    PHITracker phiTracker_;  ///< متتبع PHI / PHI tracker
    
    /**
     * عداد الكتل للأسماء الفريدة
     * Block counter for unique names
     */
    unsigned blockCounter_;  ///< عداد الكتل / Block counter
    
    // ========================================================================
    // دوال مساعدة خاصة / Private Helper Functions
    // ========================================================================
    
    /**
     * الحصول على اسم كتلة فريد
     * Get unique block name
     * 
     * @param prefix بادئة الاسم / Name prefix
     * @return اسم فريد / Unique name
     */
    std::string getUniqueBlockName(const std::string& prefix);
    
    /**
     * إنشاء فرع شرطي
     * Create conditional branch
     * 
     * @param condition الشرط / Condition
     * @param trueBlock كتلة true / True block
     * @param falseBlock كتلة false / False block
     */
    void createCondBr(llvm::Value* condition, llvm::BasicBlock* trueBlock, llvm::BasicBlock* falseBlock);
    
    /**
     * إنشاء فرع غير شرطي
     * Create unconditional branch
     * 
     * @param targetBlock الكتلة المستهدفة / Target block
     */
    void createBr(llvm::BasicBlock* targetBlock);
};

} // namespace LLVM
} // namespace Sad
