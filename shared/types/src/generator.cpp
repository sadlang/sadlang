/**
 * @file generator.cpp
 * @brief (AR) تنفيذ فئة المولّد
 * @brief (EN) Generator class implementation
 * 
 * @author S Language Development Team
 * @date January 5, 2026
 */

#include "generator.h"
#include <stdexcept>
#include <iostream>

namespace Sad {
namespace Data {

// =========================================================================
// (AR) البناء / (EN) Constructor
// =========================================================================

Generator::Generator(const std::string& name,
                     std::shared_ptr<AST::Statement> functionBody,
                     const std::vector<AST::Parameter>& parameters)
    : name_(name)
    , functionBody_(functionBody)
    , parameters_(parameters)
    , state_(GeneratorState::CREATED)
    , resumeIndex_(0)
    , shouldContinue_(true)
{
    // (AR) تهيئة المولّد في حالة CREATED
    // (EN) Initialize generator in CREATED state
}

// =========================================================================
// (AR) تهيئة المولّد / (EN) Initialize Generator
// =========================================================================

void Generator::initialize(const std::vector<Value>& args) {
    // (AR) تخزين المعاملات / (EN) Store arguments
    arguments_ = args;
    
    // (AR) تهيئة المتغيرات المحلية من المعاملات
    // (EN) Initialize local variables from parameters
    for (size_t i = 0; i < parameters_.size() && i < args.size(); ++i) {
        localVariables[parameters_[i].name] = args[i];
    }
    
    // (AR) تحديث الحالة إلى SUSPENDED (جاهز للتنفيذ)
    // (EN) Update state to SUSPENDED (ready to run)
    state_ = GeneratorState::SUSPENDED;
    resumeIndex_ = 0;
    shouldContinue_ = true;
}

// =========================================================================
// (AR) الحصول على القيمة التالية / (EN) Get Next Value
// =========================================================================

Value Generator::next() {
    // (AR) التحقق من حالة المولّد / (EN) Check generator state
    if (state_ == GeneratorState::COMPLETED) {
        // (AR) المولّد انتهى - لا توجد قيم أخرى
        // (EN) Generator exhausted - no more values
        throw std::runtime_error("(AR) المولّد انتهى، لا توجد قيم أخرى / "
                                "(EN) Generator exhausted, no more values");
    }
    
    if (state_ == GeneratorState::CREATED) {
        // (AR) المولّد لم يُهيأ - خطأ
        // (EN) Generator not initialized - error
        throw std::runtime_error("(AR) المولّد لم يُهيأ. استدعِ initialize() أولاً / "
                                "(EN) Generator not initialized. Call initialize() first");
    }
    
    // (AR) تحديث الحالة إلى RUNNING
    // (EN) Update state to RUNNING
    state_ = GeneratorState::RUNNING;
    
    // (AR) ملاحظة: التنفيذ الفعلي يتم في StatementExecutor
    //      هذه الدالة تُستخدم فقط للحصول على القيمة المُخزّنة
    // (EN) Note: Actual execution happens in StatementExecutor
    //      This function is only used to retrieve stored value
    
    // (AR) إرجاع القيمة الحالية
    // (EN) Return current value
    Value result = currentYieldedValue_;
    
    // (AR) تحديث الحالة
    // (EN) Update state
    if (shouldContinue_) {
        state_ = GeneratorState::SUSPENDED;
    } else {
        state_ = GeneratorState::COMPLETED;
    }
    
    return result;
}

// =========================================================================
// (AR) التحقق من وجود قيم أخرى / (EN) Check Has Next
// =========================================================================

bool Generator::hasNext() const {
    return state_ != GeneratorState::COMPLETED && 
           state_ != GeneratorState::CREATED &&
           shouldContinue_;
}

// =========================================================================
// (AR) إعادة تعيين المولّد / (EN) Reset Generator
// =========================================================================

void Generator::reset() {
    state_ = GeneratorState::CREATED;
    resumeIndex_ = 0;
    currentStatementIndex = 0;
    localVariables.clear();
    currentYieldedValue_ = Value();
    shouldContinue_ = true;
}

// =========================================================================
// (AR) جمع جميع القيم / (EN) Collect All Values
// =========================================================================

std::vector<Value> Generator::collect(size_t maxValues) {
    std::vector<Value> result;
    size_t count = 0;
    
    while (hasNext() && (maxValues == 0 || count < maxValues)) {
        result.push_back(next());
        ++count;
    }
    
    return result;
}

// =========================================================================
// (AR) تعيين قيمة yield / (EN) Set Yielded Value
// =========================================================================

void Generator::setYieldedValue(const Value& value) {
    currentYieldedValue_ = value;
    state_ = GeneratorState::SUSPENDED;
}

// =========================================================================
// (AR) تعيين حالة الانتهاء / (EN) Set Completed
// =========================================================================

void Generator::setCompleted() {
    state_ = GeneratorState::COMPLETED;
    shouldContinue_ = false;
}

} // namespace Data
} // namespace Sad
