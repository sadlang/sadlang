// بسم الله الرحمن الرحيم
// ===================================================================
// Constant Folding & Propagation Optimizer - محسّن طي الثوابت ونشرها
// Phase 6.2: Constant Folding & Propagation
// ===================================================================
// الحمد لله رب العالمين

#ifndef SAD_OPTIMIZER_CONSTANT_FOLDING_H
#define SAD_OPTIMIZER_CONSTANT_FOLDING_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <optional>
#include <variant>

namespace sad {
namespace optimizer {

// ===================================================================
// Forward Declarations
// ===================================================================

class BasicBlock;
class Function;
class Instruction;
class Value;
class Constant;

// ===================================================================
// Constant Value - قيمة ثابتة
// ===================================================================

/**
 * Represents a compile-time constant value
 * تمثل قيمة ثابتة في وقت الترجمة
 */
class ConstantValue {
public:
    enum class Type {
        INTEGER,     // عدد صحيح
        FLOAT,       // عدد عشري
        BOOLEAN,     // منطقي
        STRING,      // نص
        NULL_VALUE,  // قيمة فارغة
        UNDEFINED    // غير معرّف
    };

    ConstantValue() : type_(Type::UNDEFINED) {}
    
    // Constructors for different types
    static ConstantValue make_int(int64_t value);
    static ConstantValue make_float(double value);
    static ConstantValue make_bool(bool value);
    static ConstantValue make_string(const std::string& value);
    static ConstantValue make_null();
    static ConstantValue make_undefined();

    // Type checking
    bool is_integer() const { return type_ == Type::INTEGER; }
    bool is_float() const { return type_ == Type::FLOAT; }
    bool is_boolean() const { return type_ == Type::BOOLEAN; }
    bool is_string() const { return type_ == Type::STRING; }
    bool is_null() const { return type_ == Type::NULL_VALUE; }
    bool is_undefined() const { return type_ == Type::UNDEFINED; }
    bool is_numeric() const { return is_integer() || is_float(); }

    // Value getters
    int64_t get_int() const;
    double get_float() const;
    bool get_bool() const;
    const std::string& get_string() const;

    // Conversions
    double to_number() const;
    bool to_bool() const;
    std::string to_string() const;

    // Type
    Type get_type() const { return type_; }

private:
    Type type_;
    std::variant<int64_t, double, bool, std::string> value_;
};

// ===================================================================
// Constant Folding Pass - ممر طي الثوابت
// ===================================================================

/**
 * Constant Folding & Propagation Optimizer
 * محسّن طي الثوابت ونشرها
 * 
 * يقوم بـ:
 * 1. Constant Folding - طي الثوابت (تقييم العمليات في وقت الترجمة)
 * 2. Constant Propagation - نشر الثوابت (استبدال المتغيرات بقيمها)
 * 3. Algebraic Simplification - التبسيط الجبري (x+0 → x, x*1 → x)
 * 4. Strength Reduction - تخفيض القوة (x*2 → x<<1)
 * 
 * Performs:
 * 1. Constant Folding (compile-time evaluation)
 * 2. Constant Propagation (replace variables with constants)
 * 3. Algebraic Simplification (x+0 → x, x*1 → x)
 * 4. Strength Reduction (x*2 → x<<1)
 */
class ConstantFolding {
public:
    ConstantFolding() = default;
    ~ConstantFolding() = default;

    // Main optimization entry point
    bool optimize(Function* function);
    bool optimize_module(std::vector<Function*>& functions);

    // Statistics
    struct Stats {
        size_t constants_folded = 0;          // عمليات تم طيها
        size_t constants_propagated = 0;      // ثوابت تم نشرها
        size_t algebraic_simplifications = 0; // تبسيطات جبرية
        size_t strength_reductions = 0;       // تخفيضات قوة
        size_t total_optimizations = 0;       // مجموع التحسينات

        void reset();
        std::string to_string() const;
    };

    const Stats& get_stats() const { return stats_; }
    void reset_stats() { stats_.reset(); }

    // Configuration
    struct Config {
        bool enable_folding = true;           // تفعيل طي الثوابت
        bool enable_propagation = true;       // تفعيل نشر الثوابت
        bool enable_algebraic = true;         // تفعيل التبسيط الجبري
        bool enable_strength_reduction = true; // تفعيل تخفيض القوة
        bool aggressive_mode = false;         // الوضع العدواني
        size_t max_iterations = 5;           // أقصى عدد تكرارات
    };

    void set_config(const Config& config) { config_ = config; }
    const Config& get_config() const { return config_; }

private:
    // ===============================================================
    // Phase 1: Constant Folding - طي الثوابت
    // ===============================================================

    /**
     * Fold constant expressions at compile time
     * طي التعبيرات الثابتة في وقت الترجمة
     */
    bool fold_constants(Function* function);

    /**
     * Try to fold a single instruction
     * محاولة طي تعليمة واحدة
     */
    std::optional<ConstantValue> try_fold_instruction(Instruction* inst);

    /**
     * Fold binary operations
     * طي العمليات الثنائية
     */
    std::optional<ConstantValue> fold_binary_op(
        Opcode op,
        const ConstantValue& left,
        const ConstantValue& right
    );

    /**
     * Fold unary operations
     * طي العمليات الأحادية
     */
    std::optional<ConstantValue> fold_unary_op(
        Opcode op,
        const ConstantValue& operand
    );

    /**
     * Fold comparison operations
     * طي عمليات المقارنة
     */
    std::optional<ConstantValue> fold_comparison(
        Opcode op,
        const ConstantValue& left,
        const ConstantValue& right
    );

    // ===============================================================
    // Phase 2: Constant Propagation - نشر الثوابت
    // ===============================================================

    /**
     * Propagate constants through the function
     * نشر الثوابت عبر الدالة
     */
    bool propagate_constants(Function* function);

    /**
     * Build constant value map (reaching definitions)
     * بناء خريطة القيم الثابتة
     */
    void build_constant_map(Function* function);

    /**
     * Get constant value for a variable
     * الحصول على القيمة الثابتة لمتغير
     */
    std::optional<ConstantValue> get_constant_value(Value* value);

    /**
     * Replace uses of variable with constant
     * استبدال استخدامات المتغير بالثابت
     */
    bool replace_with_constant(Value* variable, const ConstantValue& constant);

    // ===============================================================
    // Phase 3: Algebraic Simplification - التبسيط الجبري
    // ===============================================================

    /**
     * Simplify algebraic expressions
     * تبسيط التعبيرات الجبرية
     */
    bool simplify_algebraic(Function* function);

    /**
     * Try to simplify a single instruction
     * محاولة تبسيط تعليمة واحدة
     */
    bool try_simplify_instruction(Instruction* inst);

    /**
     * Identity simplifications (x+0 → x, x*1 → x)
     * تبسيطات الهوية
     */
    bool simplify_identity(Instruction* inst);

    /**
     * Absorption simplifications (x*0 → 0, x&0 → 0)
     * تبسيطات الامتصاص
     */
    bool simplify_absorption(Instruction* inst);

    /**
     * Idempotent simplifications (x|x → x, x&x → x)
     * تبسيطات التكافؤ
     */
    bool simplify_idempotent(Instruction* inst);

    /**
     * Inverse simplifications (x-x → 0, x/x → 1)
     * تبسيطات العكس
     */
    bool simplify_inverse(Instruction* inst);

    // ===============================================================
    // Phase 4: Strength Reduction - تخفيض القوة
    // ===============================================================

    /**
     * Replace expensive operations with cheaper ones
     * استبدال العمليات المكلفة بعمليات أرخص
     */
    bool reduce_strength(Function* function);

    /**
     * Replace multiplication by power of 2 with shift
     * استبدال الضرب بقوة 2 بالإزاحة
     */
    bool reduce_multiply_to_shift(Instruction* inst);

    /**
     * Replace division by power of 2 with shift
     * استبدال القسمة بقوة 2 بالإزاحة
     */
    bool reduce_divide_to_shift(Instruction* inst);

    /**
     * Replace modulo by power of 2 with AND
     * استبدال المعامل بقوة 2 بـ AND
     */
    bool reduce_modulo_to_and(Instruction* inst);

    /**
     * Replace power operation with multiplication
     * استبدال عملية الأس بالضرب
     */
    bool reduce_power_to_multiply(Instruction* inst);

    // ===============================================================
    // Utility Functions - دوال مساعدة
    // ===============================================================

    /**
     * Check if value is constant
     * فحص إذا كانت القيمة ثابتة
     */
    bool is_constant(Value* value);

    /**
     * Check if instruction has all constant operands
     * فحص إذا كانت جميع معاملات التعليمة ثابتة
     */
    bool has_constant_operands(Instruction* inst);

    /**
     * Get constant operands
     * الحصول على المعاملات الثابتة
     */
    std::vector<ConstantValue> get_constant_operands(Instruction* inst);

    /**
     * Replace instruction with constant
     * استبدال التعليمة بثابت
     */
    void replace_instruction_with_constant(
        Instruction* inst,
        const ConstantValue& constant
    );

    /**
     * Check if number is power of 2
     * فحص إذا كان الرقم قوة 2
     */
    bool is_power_of_two(int64_t n) const;

    /**
     * Get log2 of power of 2
     * الحصول على log2 لقوة 2
     */
    int get_log2(int64_t n) const;

    /**
     * Run iterative optimization
     * تشغيل التحسين التكراري
     */
    bool run_iterative_optimization(Function* function);

    /**
     * Print optimization info
     * طباعة معلومات التحسين
     */
    void print_optimization(const std::string& message);

private:
    // Configuration
    Config config_;

    // Statistics
    Stats stats_;

    // Constant value map (variable → constant)
    std::unordered_map<Value*, ConstantValue> constant_map_;

    // Worklist for propagation
    std::vector<Instruction*> worklist_;

    // Debug/logging
    bool verbose_ = false;
};

// ===================================================================
// Arithmetic Operations - العمليات الحسابية
// ===================================================================

namespace arithmetic {

// Integer operations
std::optional<int64_t> add_int(int64_t a, int64_t b);
std::optional<int64_t> sub_int(int64_t a, int64_t b);
std::optional<int64_t> mul_int(int64_t a, int64_t b);
std::optional<int64_t> div_int(int64_t a, int64_t b);
std::optional<int64_t> mod_int(int64_t a, int64_t b);

// Float operations
double add_float(double a, double b);
double sub_float(double a, double b);
double mul_float(double a, double b);
std::optional<double> div_float(double a, double b);

// Bitwise operations
int64_t and_int(int64_t a, int64_t b);
int64_t or_int(int64_t a, int64_t b);
int64_t xor_int(int64_t a, int64_t b);
int64_t not_int(int64_t a);
int64_t shl_int(int64_t a, int64_t b);
int64_t shr_int(int64_t a, int64_t b);

// Comparison operations
bool cmp_eq(const ConstantValue& a, const ConstantValue& b);
bool cmp_ne(const ConstantValue& a, const ConstantValue& b);
bool cmp_lt(const ConstantValue& a, const ConstantValue& b);
bool cmp_le(const ConstantValue& a, const ConstantValue& b);
bool cmp_gt(const ConstantValue& a, const ConstantValue& b);
bool cmp_ge(const ConstantValue& a, const ConstantValue& b);

// Logical operations
bool logical_and(bool a, bool b);
bool logical_or(bool a, bool b);
bool logical_not(bool a);

} // namespace arithmetic

// ===================================================================
// Algebraic Rules - القواعد الجبرية
// ===================================================================

namespace algebraic {

/**
 * Identity rules: x op identity = x
 * قواعد الهوية
 */
struct IdentityRule {
    Opcode op;
    ConstantValue identity;
};

/**
 * Absorption rules: x op absorber = absorber
 * قواعد الامتصاص
 */
struct AbsorptionRule {
    Opcode op;
    ConstantValue absorber;
};

/**
 * Get identity rules
 * الحصول على قواعد الهوية
 */
std::vector<IdentityRule> get_identity_rules();

/**
 * Get absorption rules
 * الحصول على قواعد الامتصاص
 */
std::vector<AbsorptionRule> get_absorption_rules();

} // namespace algebraic

// ===================================================================
// Factory Functions - دوال المصنع
// ===================================================================

/**
 * Create standard constant folder
 * إنشاء طاوي ثوابت قياسي
 */
std::unique_ptr<ConstantFolding> create_standard_constant_folder();

/**
 * Create aggressive constant folder
 * إنشاء طاوي ثوابت عدواني
 */
std::unique_ptr<ConstantFolding> create_aggressive_constant_folder();

/**
 * Quick check if function needs constant folding
 * فحص سريع إذا كانت الدالة تحتاج طي الثوابت
 */
bool needs_constant_folding(Function* function);

} // namespace optimizer
} // namespace sad

#endif // SAD_OPTIMIZER_CONSTANT_FOLDING_H
