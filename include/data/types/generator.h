/**
 * @file generator.h
 * @brief (AR) فئة المولّد - دعم الدوال المولّدة
 * @brief (EN) Generator class - Generator function support
 * 
 * @description
 * (AR) المولّدات هي دوال خاصة تستخدم `أنتج` (yield) لإنتاج قيم متتالية.
 *      تحافظ على حالتها بين الاستدعاءات، مما يجعلها فعالة للذاكرة
 *      عند العمل مع تسلسلات كبيرة.
 * 
 * (EN) Generators are special functions that use `yield` to produce 
 *      successive values. They maintain their state between calls,
 *      making them memory-efficient for large sequences.
 * 
 * @example
 * ```s
 * # تعريف مولد
 * دالة مولد عد_تنازلي(رقم بداية)
 *     بينما بداية > 0
 *         أنتج بداية
 *         بداية = بداية - 1
 *     نهاية
 * نهاية
 * 
 * # استخدام المولد
 * مولد م = عد_تنازلي(5)
 * لكل(قيمة في م)
 *     اطبع_سطر(قيمة)
 * نهاية
 * # الناتج: 5، 4، 3، 2، 1
 * ```
 * 
 * @author S Language Development Team
 * @date January 5, 2026
 * @version 1.0
 */

#pragma once

#include "value.h"
#include "../../parser/ast/declarations.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

namespace Sad {
namespace Data {

/**
 * @brief (AR) حالة المولّد
 * @brief (EN) Generator state
 */
enum class GeneratorState {
    CREATED,    ///< (AR) تم إنشاؤه لكن لم يبدأ / (EN) Created but not started
    RUNNING,    ///< (AR) قيد التنفيذ / (EN) Currently running
    SUSPENDED,  ///< (AR) موقوف (بعد yield) / (EN) Suspended (after yield)
    COMPLETED   ///< (AR) اكتمل التنفيذ / (EN) Execution completed
};

/**
 * @brief (AR) فئة المولّد
 * @brief (EN) Generator class
 * 
 * يمثل مولّد يمكنه إنتاج قيم متتالية باستخدام yield.
 * Represents a generator that can produce successive values using yield.
 */
class Generator {
public:
    /**
     * @brief (AR) البناء
     * @brief (EN) Constructor
     * 
     * @param name (AR) اسم المولّد / (EN) Generator name
     * @param functionBody (AR) جسم الدالة المولّدة / (EN) Generator function body
     * @param parameters (AR) معاملات الدالة / (EN) Function parameters
     */
    Generator(const std::string& name, 
              std::shared_ptr<AST::Statement> functionBody,
              const std::vector<AST::Parameter>& parameters);
    
    /**
     * @brief (AR) تهيئة المولّد مع معاملات
     * @brief (EN) Initialize generator with arguments
     * 
     * @param args (AR) قيم المعاملات / (EN) Argument values
     */
    void initialize(const std::vector<Value>& args);
    
    /**
     * @brief (AR) الحصول على القيمة التالية
     * @brief (EN) Get next value
     * 
     * @return (AR) القيمة التالية أو قيمة فارغة إذا انتهى المولّد
     *         (EN) Next value or void if generator is exhausted
     */
    Value next();
    
    /**
     * @brief (AR) التحقق من وجود قيم أخرى
     * @brief (EN) Check if more values exist
     * 
     * @return true (AR) إذا كان هناك المزيد / (EN) if more values available
     */
    bool hasNext() const;
    
    /**
     * @brief (AR) الحصول على حالة المولّد
     * @brief (EN) Get generator state
     */
    GeneratorState getState() const { return state_; }
    
    /**
     * @brief (AR) الحصول على اسم المولّد
     * @brief (EN) Get generator name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief (AR) إعادة تعيين المولّد
     * @brief (EN) Reset generator
     * 
     * يعيد المولّد لحالته الأولى.
     * Resets generator to initial state.
     */
    void reset();
    
    /**
     * @brief (AR) جمع جميع القيم في مصفوفة
     * @brief (EN) Collect all values into array
     * 
     * تحذير: قد يكون غير فعال للمولدات اللانهائية!
     * Warning: May be inefficient for infinite generators!
     * 
     * @param maxValues (AR) الحد الأقصى للقيم (0 = بلا حد)
     *                  (EN) Maximum values (0 = unlimited)
     * @return (AR) مصفوفة بجميع القيم / (EN) Array of all values
     */
    std::vector<Value> collect(size_t maxValues = 0);
    
    /**
     * @brief (AR) تعيين قيمة yield الحالية
     * @brief (EN) Set current yield value
     * 
     * يُستخدم من StatementExecutor عند تنفيذ yield.
     * Used by StatementExecutor when executing yield.
     */
    void setYieldedValue(const Value& value);
    
    /**
     * @brief (AR) تعيين حالة الانتهاء
     * @brief (EN) Set completed state
     */
    void setCompleted();

    // Local variables state
    std::unordered_map<std::string, Value> localVariables;
    
    // Current statement index (for resumption)
    size_t currentStatementIndex = 0;

private:
    std::string name_;
    std::shared_ptr<AST::Statement> functionBody_;
    std::vector<AST::Parameter> parameters_;
    std::vector<Value> arguments_;
    GeneratorState state_;
    Value currentYieldedValue_;
    
    // Statement index for resumption
    size_t resumeIndex_;
    
    // Flag to track if iteration should continue
    bool shouldContinue_;
};

/**
 * @brief (AR) قيمة مولّد في نظام القيم
 * @brief (EN) Generator value in value system
 * 
 * يلف مؤشر Generator للاستخدام في نظام Value.
 * Wraps Generator pointer for use in Value system.
 */
class GeneratorValue {
public:
    explicit GeneratorValue(std::shared_ptr<Generator> gen) : generator_(gen) {}
    
    std::shared_ptr<Generator> getGenerator() const { return generator_; }
    
    // Iterator-like interface
    Value next() { return generator_->next(); }
    bool hasNext() const { return generator_->hasNext(); }
    GeneratorState getState() const { return generator_->getState(); }
    
private:
    std::shared_ptr<Generator> generator_;
};

} // namespace Data
} // namespace Sad
