/*
 * ============================================================================
 * مدير تدفق التحكم في LLVM - ملف التنفيذ
 * LLVM Control Flow Manager - Implementation File
 * ============================================================================
 * 
 * هذا الملف يحتوي على تنفيذ مدير تدفق التحكم في LLVM IR
 * This file contains implementation of LLVM control flow manager
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement
 * ============================================================================
 */

#include "llvm_control_flow.h"
#include <llvm/IR/Constants.h>
#include <sstream>

namespace Sad {
namespace LLVM {

// ============================================================================
// PHITracker - تنفيذ متتبع PHI nodes
// ============================================================================

/**
 * تسجيل PHI node جديد
 * Register new PHI node
 */
void PHITracker::registerPHI(const std::string& varName, llvm::PHINode* phi) {
    phiNodes_[varName] = phi;  // حفظ PHI node / Save PHI node
}

/**
 * إضافة قيمة إلى PHI node
 * Add value to PHI node
 */
void PHITracker::addIncoming(const std::string& varName, llvm::Value* value, llvm::BasicBlock* block) {
    // البحث عن PHI node / Search for PHI node
    auto it = phiNodes_.find(varName);
    if (it != phiNodes_.end()) {
        it->second->addIncoming(value, block);  // إضافة القيمة / Add value
    }
}

/**
 * الحصول على PHI node لمتغير
 * Get PHI node for variable
 */
llvm::PHINode* PHITracker::getPHI(const std::string& varName) const {
    // البحث عن PHI node / Search for PHI node
    auto it = phiNodes_.find(varName);
    if (it != phiNodes_.end()) {
        return it->second;  // إرجاع PHI node / Return PHI node
    }
    return nullptr;  // لم يتم العثور عليه / Not found
}

/**
 * مسح جميع PHI nodes
 * Clear all PHI nodes
 */
void PHITracker::clear() {
    phiNodes_.clear();  // مسح التخزين / Clear storage
}

// ============================================================================
// LLVMControlFlow - المنشئ والمدمر
// ============================================================================

/**
 * منشئ مدير تدفق التحكم
 * Control flow manager constructor
 */
LLVMControlFlow::LLVMControlFlow(llvm::LLVMContext& context,
                                 llvm::IRBuilder<>& builder,
                                 llvm::Function* function)
    : context_(context)
    , builder_(builder)
    , function_(function)
    , blockCounter_(0)
{
    // لا حاجة لتهيئة إضافية / No additional initialization needed
}

// ============================================================================
// جمل if/else / if/else Statements
// ============================================================================

/**
 * بدء جملة if
 * Begin if statement
 */
void LLVMControlFlow::beginIf(llvm::Value* condition) {
    // إنشاء الكتل الثلاث / Create three blocks
    llvm::BasicBlock* thenBlock = createBlock("if.then");    // كتلة then / then block
    llvm::BasicBlock* elseBlock = createBlock("if.else");    // كتلة else / else block
    llvm::BasicBlock* mergeBlock = createBlock("if.end");    // كتلة الدمج / merge block
    
    // إنشاء فرع شرطي / Create conditional branch
    createCondBr(condition, thenBlock, elseBlock);
    
    // حفظ السياق في المكدس / Save context to stack
    IfContext ctx;
    ctx.thenBlock = thenBlock;
    ctx.elseBlock = elseBlock;
    ctx.mergeBlock = mergeBlock;
    ctx.hasElse = false;  // لا يوجد else بعد / No else yet
    ifStack_.push_back(ctx);
    
    // الانتقال إلى كتلة then / Move to then block
    moveToBlock(thenBlock);
}

/**
 * بدء فرع else
 * Begin else branch
 */
void LLVMControlFlow::beginElse() {
    // التحقق من وجود if نشط / Check for active if
    if (ifStack_.empty()) {
        return;  // لا يوجد if نشط / No active if
    }
    
    // الحصول على السياق / Get context
    IfContext& ctx = ifStack_.back();
    
    // إنشاء فرع إلى كتلة الدمج من then / Create branch to merge from then
    if (!getCurrentBlock()->getTerminator()) {
        createBr(ctx.mergeBlock);
    }
    
    // الانتقال إلى كتلة else / Move to else block
    moveToBlock(ctx.elseBlock);
    
    // تعيين hasElse / Set hasElse
    ctx.hasElse = true;
}

/**
 * بدء فرع elif (else if)
 * Begin elif (else if) branch
 */
void LLVMControlFlow::beginElif(llvm::Value* condition) {
    // التحقق من وجود if نشط / Check for active if
    if (ifStack_.empty()) {
        return;  // لا يوجد if نشط / No active if
    }
    
    // الحصول على السياق / Get context
    IfContext& ctx = ifStack_.back();
    
    // إنشاء فرع إلى كتلة الدمج من الفرع السابق / Create branch to merge from previous branch
    if (!getCurrentBlock()->getTerminator()) {
        createBr(ctx.mergeBlock);
    }
    
    // الانتقال إلى كتلة else الحالية / Move to current else block
    moveToBlock(ctx.elseBlock);
    
    // إنشاء كتل جديدة لـ elif / Create new blocks for elif
    llvm::BasicBlock* elifThenBlock = createBlock("elif.then");  // كتلة elif then
    llvm::BasicBlock* elifElseBlock = createBlock("elif.else");  // كتلة elif else جديدة
    
    // إنشاء فرع شرطي / Create conditional branch
    createCondBr(condition, elifThenBlock, elifElseBlock);
    
    // تحديث السياق / Update context
    ctx.elseBlock = elifElseBlock;  // كتلة else الجديدة / New else block
    
    // الانتقال إلى كتلة elif then / Move to elif then block
    moveToBlock(elifThenBlock);
}

/**
 * إنهاء جملة if
 * End if statement
 */
void LLVMControlFlow::endIf() {
    // التحقق من وجود if نشط / Check for active if
    if (ifStack_.empty()) {
        return;  // لا يوجد if نشط / No active if
    }
    
    // الحصول على السياق / Get context
    IfContext ctx = ifStack_.back();
    ifStack_.pop_back();  // إزالة من المكدس / Remove from stack
    
    // إنشاء فرع إلى كتلة الدمج من الفرع الحالي / Create branch to merge from current branch
    if (!getCurrentBlock()->getTerminator()) {
        createBr(ctx.mergeBlock);
    }
    
    // إذا لم يكن هناك else، نحتاج لربط كتلة else بكتلة الدمج / If no else, connect else block to merge
    if (!ctx.hasElse) {
        moveToBlock(ctx.elseBlock);
        if (!ctx.elseBlock->getTerminator()) {
            createBr(ctx.mergeBlock);
        }
    }
    
    // الانتقال إلى كتلة الدمج / Move to merge block
    moveToBlock(ctx.mergeBlock);
}

// ============================================================================
// حلقات while / while Loops
// ============================================================================

/**
 * بدء حلقة while
 * Begin while loop
 */
void LLVMControlFlow::beginWhile(llvm::Value* condition) {
    // إنشاء الكتل / Create blocks
    llvm::BasicBlock* headerBlock = createBlock("while.header");  // كتلة الرأس / Header block
    llvm::BasicBlock* bodyBlock = createBlock("while.body");      // كتلة الجسم / Body block
    llvm::BasicBlock* exitBlock = createBlock("while.exit");      // كتلة الخروج / Exit block
    
    // فرع غير شرطي إلى كتلة الرأس / Unconditional branch to header
    createBr(headerBlock);
    
    // الانتقال إلى كتلة الرأس / Move to header block
    moveToBlock(headerBlock);
    
    // إنشاء فرع شرطي / Create conditional branch
    createCondBr(condition, bodyBlock, exitBlock);
    
    // حفظ سياق الحلقة / Save loop context
    LoopContext loopCtx(headerBlock, bodyBlock, exitBlock, headerBlock);  // continue يذهب إلى header
    loopStack_.push_back(loopCtx);
    
    // الانتقال إلى كتلة الجسم / Move to body block
    moveToBlock(bodyBlock);
}

/**
 * إنهاء حلقة while
 * End while loop
 */
void LLVMControlFlow::endWhile() {
    // التحقق من وجود حلقة نشطة / Check for active loop
    if (loopStack_.empty()) {
        return;  // لا توجد حلقة نشطة / No active loop
    }
    
    // الحصول على السياق / Get context
    LoopContext ctx = loopStack_.back();
    loopStack_.pop_back();  // إزالة من المكدس / Remove from stack
    
    // إنشاء فرع للعودة إلى كتلة الرأس / Create branch back to header
    if (!getCurrentBlock()->getTerminator()) {
        createBr(ctx.headerBlock);
    }
    
    // الانتقال إلى كتلة الخروج / Move to exit block
    moveToBlock(ctx.exitBlock);
}

// ============================================================================
// حلقات for / for Loops
// ============================================================================

/**
 * بدء حلقة for
 * Begin for loop
 */
void LLVMControlFlow::beginFor(llvm::Value* init, llvm::Value* condition, llvm::Value* increment) {
    // تنفيذ التهيئة / Execute initialization
    // (init already executed by caller)
    
    // إنشاء الكتل / Create blocks
    llvm::BasicBlock* headerBlock = createBlock("for.header");    // كتلة الرأس / Header block
    llvm::BasicBlock* bodyBlock = createBlock("for.body");        // كتلة الجسم / Body block
    llvm::BasicBlock* incBlock = createBlock("for.inc");          // كتلة الزيادة / Increment block
    llvm::BasicBlock* exitBlock = createBlock("for.exit");        // كتلة الخروج / Exit block
    
    // فرع غير شرطي إلى كتلة الرأس / Unconditional branch to header
    createBr(headerBlock);
    
    // الانتقال إلى كتلة الرأس / Move to header block
    moveToBlock(headerBlock);
    
    // إنشاء فرع شرطي / Create conditional branch
    createCondBr(condition, bodyBlock, exitBlock);
    
    // حفظ سياق الحلقة / Save loop context
    LoopContext loopCtx(headerBlock, bodyBlock, exitBlock, incBlock);  // continue يذهب إلى inc
    loopStack_.push_back(loopCtx);
    
    // الانتقال إلى كتلة الجسم / Move to body block
    moveToBlock(bodyBlock);
}

/**
 * إنهاء حلقة for
 * End for loop
 */
void LLVMControlFlow::endFor() {
    // التحقق من وجود حلقة نشطة / Check for active loop
    if (loopStack_.empty()) {
        return;  // لا توجد حلقة نشطة / No active loop
    }
    
    // الحصول على السياق / Get context
    LoopContext ctx = loopStack_.back();
    loopStack_.pop_back();  // إزالة من المكدس / Remove from stack
    
    // إنشاء فرع إلى كتلة الزيادة / Create branch to increment block
    if (!getCurrentBlock()->getTerminator()) {
        createBr(ctx.continueBlock);  // continueBlock = incBlock
    }
    
    // الانتقال إلى كتلة الزيادة / Move to increment block
    moveToBlock(ctx.continueBlock);
    
    // تنفيذ الزيادة هنا (يتم بواسطة المستدعي) / Execute increment here (done by caller)
    // ...
    
    // فرع للعودة إلى كتلة الرأس / Branch back to header
    if (!ctx.continueBlock->getTerminator()) {
        createBr(ctx.headerBlock);
    }
    
    // الانتقال إلى كتلة الخروج / Move to exit block
    moveToBlock(ctx.exitBlock);
}

// ============================================================================
// جمل match/switch / match/switch Statements
// ============================================================================

/**
 * بدء جملة match/switch
 * Begin match/switch statement
 */
void LLVMControlFlow::beginMatch(llvm::Value* value, unsigned numCases) {
    // إنشاء كتلة default وكتلة merge / Create default and merge blocks
    llvm::BasicBlock* defaultBlock = createBlock("match.default");  // كتلة default
    llvm::BasicBlock* mergeBlock = createBlock("match.end");        // كتلة الدمج / Merge block
    
    // إنشاء تعليمة switch / Create switch instruction
    llvm::SwitchInst* switchInst = builder_.CreateSwitch(value, defaultBlock, numCases);
    
    // حفظ السياق / Save context
    matchContext_ = std::make_unique<MatchContext>();
    matchContext_->switchInst = switchInst;
    matchContext_->defaultBlock = defaultBlock;
    matchContext_->mergeBlock = mergeBlock;
    matchContext_->currentCase = nullptr;
}

/**
 * إضافة حالة case إلى match
 * Add case to match
 */
void LLVMControlFlow::addCase(llvm::Value* caseValue) {
    // التحقق من وجود match نشط / Check for active match
    if (!matchContext_) {
        return;  // لا يوجد match نشط / No active match
    }
    
    // إذا كانت هناك حالة سابقة، أنشئ فرع إلى merge / If previous case, create branch to merge
    if (matchContext_->currentCase) {
        if (!getCurrentBlock()->getTerminator()) {
            createBr(matchContext_->mergeBlock);
        }
    }
    
    // إنشاء كتلة للحالة / Create case block
    llvm::BasicBlock* caseBlock = createBlock("match.case");
    
    // إضافة الحالة إلى switch / Add case to switch
    llvm::ConstantInt* constValue = llvm::dyn_cast<llvm::ConstantInt>(caseValue);
    if (constValue) {
        matchContext_->switchInst->addCase(constValue, caseBlock);
    }
    
    // تحديث الحالة الحالية / Update current case
    matchContext_->currentCase = caseBlock;
    
    // الانتقال إلى كتلة الحالة / Move to case block
    moveToBlock(caseBlock);
}

/**
 * إضافة حالة default
 * Add default case
 */
void LLVMControlFlow::addDefault() {
    // التحقق من وجود match نشط / Check for active match
    if (!matchContext_) {
        return;  // لا يوجد match نشط / No active match
    }
    
    // إذا كانت هناك حالة سابقة، أنشئ فرع إلى merge / If previous case, create branch to merge
    if (matchContext_->currentCase) {
        if (!getCurrentBlock()->getTerminator()) {
            createBr(matchContext_->mergeBlock);
        }
    }
    
    // الانتقال إلى كتلة default / Move to default block
    moveToBlock(matchContext_->defaultBlock);
    
    // تحديث الحالة الحالية / Update current case
    matchContext_->currentCase = matchContext_->defaultBlock;
}

/**
 * إنهاء جملة match
 * End match statement
 */
void LLVMControlFlow::endMatch() {
    // التحقق من وجود match نشط / Check for active match
    if (!matchContext_) {
        return;  // لا يوجد match نشط / No active match
    }
    
    // إنشاء فرع إلى merge من الحالة الحالية / Create branch to merge from current case
    if (!getCurrentBlock()->getTerminator()) {
        createBr(matchContext_->mergeBlock);
    }
    
    // الانتقال إلى كتلة الدمج / Move to merge block
    moveToBlock(matchContext_->mergeBlock);
    
    // مسح السياق / Clear context
    matchContext_.reset();
}

// ============================================================================
// جمل try/catch / try/catch Statements
// ============================================================================

/**
 * بدء كتلة try
 * Begin try block
 */
void LLVMControlFlow::beginTry() {
    // TODO: تنفيذ معالجة الاستثناءات / TODO: Implement exception handling
    // يحتاج دعم landing pads و invoke instructions
    // Needs support for landing pads and invoke instructions
}

/**
 * بدء كتلة catch
 * Begin catch block
 */
void LLVMControlFlow::beginCatch(llvm::Type* exceptionType) {
    // TODO: تنفيذ معالجة الاستثناءات / TODO: Implement exception handling
}

/**
 * إنهاء جملة try/catch
 * End try/catch statement
 */
void LLVMControlFlow::endTry() {
    // TODO: تنفيذ معالجة الاستثناءات / TODO: Implement exception handling
}

// ============================================================================
// جمل break و continue / break & continue Statements
// ============================================================================

/**
 * توليد جملة break
 * Generate break statement
 */
void LLVMControlFlow::emitBreak() {
    // التحقق من وجود حلقة نشطة / Check for active loop
    if (loopStack_.empty()) {
        return;  // لا توجد حلقة نشطة / No active loop
    }
    
    // الحصول على سياق الحلقة / Get loop context
    const LoopContext& ctx = loopStack_.back();
    
    // إنشاء فرع إلى كتلة الخروج / Create branch to exit block
    createBr(ctx.exitBlock);
}

/**
 * توليد جملة continue
 * Generate continue statement
 */
void LLVMControlFlow::emitContinue() {
    // التحقق من وجود حلقة نشطة / Check for active loop
    if (loopStack_.empty()) {
        return;  // لا توجد حلقة نشطة / No active loop
    }
    
    // الحصول على سياق الحلقة / Get loop context
    const LoopContext& ctx = loopStack_.back();
    
    // إنشاء فرع إلى كتلة continue (header أو inc) / Create branch to continue block
    createBr(ctx.continueBlock);
}

// ============================================================================
// إدارة الكتل الأساسية / Basic Block Management
// ============================================================================

/**
 * إنشاء كتلة أساسية جديدة
 * Create new basic block
 */
llvm::BasicBlock* LLVMControlFlow::createBlock(const std::string& name) {
    // الحصول على اسم فريد / Get unique name
    std::string uniqueName = getUniqueBlockName(name);
    
    // إنشاء الكتلة / Create block
    return llvm::BasicBlock::Create(context_, uniqueName, function_);
}

/**
 * الانتقال إلى كتلة
 * Move to block
 */
void LLVMControlFlow::moveToBlock(llvm::BasicBlock* block) {
    builder_.SetInsertPoint(block);  // تعيين نقطة الإدراج / Set insert point
}

/**
 * الحصول على الكتلة الحالية
 * Get current block
 */
llvm::BasicBlock* LLVMControlFlow::getCurrentBlock() const {
    return builder_.GetInsertBlock();  // الحصول على كتلة الإدراج الحالية / Get current insert block
}

// ============================================================================
// إدارة PHI Nodes / PHI Node Management
// ============================================================================

/**
 * إنشاء PHI node
 * Create PHI node
 */
llvm::PHINode* LLVMControlFlow::createPHI(llvm::Type* type, const std::string& name, unsigned numIncoming) {
    // إنشاء PHI node / Create PHI node
    llvm::PHINode* phi = builder_.CreatePHI(type, numIncoming, name);
    
    // تسجيل في المتتبع / Register in tracker
    phiTracker_.registerPHI(name, phi);
    
    return phi;  // إرجاع PHI node / Return PHI node
}

/**
 * إضافة قيمة إلى PHI node
 * Add value to PHI node
 */
void LLVMControlFlow::addPHIIncoming(llvm::PHINode* phi, llvm::Value* value, llvm::BasicBlock* block) {
    phi->addIncoming(value, block);  // إضافة القيمة الواردة / Add incoming value
}

// ============================================================================
// تحسين تدفق التحكم / Control Flow Optimization
// ============================================================================

/**
 * تحسين جملة if بسيطة
 * Optimize simple if statement
 */
llvm::Value* LLVMControlFlow::optimizeSimpleIf(llvm::Value* condition, llvm::Value* thenValue, llvm::Value* elseValue) {
    // استخدام select instruction بدلاً من if/else / Use select instruction instead of if/else
    return builder_.CreateSelect(condition, thenValue, elseValue, "select");
}

/**
 * دمج كتل أساسية فارغة
 * Merge empty basic blocks
 */
void LLVMControlFlow::mergeEmptyBlocks() {
    // TODO: تنفيذ دمج الكتل الفارغة / TODO: Implement empty block merging
    // يحتاج تحليل CFG / Needs CFG analysis
}

// ============================================================================
// دوال مساعدة خاصة / Private Helper Functions
// ============================================================================

/**
 * الحصول على اسم كتلة فريد
 * Get unique block name
 */
std::string LLVMControlFlow::getUniqueBlockName(const std::string& prefix) {
    // إنشاء اسم فريد / Create unique name
    std::ostringstream oss;
    oss << prefix << "." << blockCounter_++;  // إضافة رقم فريد / Add unique number
    return oss.str();
}

/**
 * إنشاء فرع شرطي
 * Create conditional branch
 */
void LLVMControlFlow::createCondBr(llvm::Value* condition, llvm::BasicBlock* trueBlock, llvm::BasicBlock* falseBlock) {
    builder_.CreateCondBr(condition, trueBlock, falseBlock);  // إنشاء فرع شرطي / Create conditional branch
}

/**
 * إنشاء فرع غير شرطي
 * Create unconditional branch
 */
void LLVMControlFlow::createBr(llvm::BasicBlock* targetBlock) {
    builder_.CreateBr(targetBlock);  // إنشاء فرع غير شرطي / Create unconditional branch
}

} // namespace LLVM
} // namespace Sad
