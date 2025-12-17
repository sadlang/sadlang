// بسم الله الرحمن الرحيم
// ===================================================================
// Constant Folding Implementation - تنفيذ طي الثوابت
// Phase 6.2: Constant Folding & Propagation
// ===================================================================

#include "constant_folding.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <limits>

namespace sad {
namespace optimizer {

// ===================================================================
// ConstantValue Implementation
// ===================================================================

ConstantValue ConstantValue::make_int(int64_t value) {
    ConstantValue cv;
    cv.type_ = Type::INTEGER;
    cv.value_ = value;
    return cv;
}

ConstantValue ConstantValue::make_float(double value) {
    ConstantValue cv;
    cv.type_ = Type::FLOAT;
    cv.value_ = value;
    return cv;
}

ConstantValue ConstantValue::make_bool(bool value) {
    ConstantValue cv;
    cv.type_ = Type::BOOLEAN;
    cv.value_ = value;
    return cv;
}

ConstantValue ConstantValue::make_string(const std::string& value) {
    ConstantValue cv;
    cv.type_ = Type::STRING;
    cv.value_ = value;
    return cv;
}

ConstantValue ConstantValue::make_null() {
    ConstantValue cv;
    cv.type_ = Type::NULL_VALUE;
    return cv;
}

ConstantValue ConstantValue::make_undefined() {
    ConstantValue cv;
    cv.type_ = Type::UNDEFINED;
    return cv;
}

int64_t ConstantValue::get_int() const {
    return std::get<int64_t>(value_);
}

double ConstantValue::get_float() const {
    return std::get<double>(value_);
}

bool ConstantValue::get_bool() const {
    return std::get<bool>(value_);
}

const std::string& ConstantValue::get_string() const {
    return std::get<std::string>(value_);
}

double ConstantValue::to_number() const {
    switch (type_) {
        case Type::INTEGER:
            return static_cast<double>(get_int());
        case Type::FLOAT:
            return get_float();
        case Type::BOOLEAN:
            return get_bool() ? 1.0 : 0.0;
        default:
            return 0.0;
    }
}

bool ConstantValue::to_bool() const {
    switch (type_) {
        case Type::INTEGER:
            return get_int() != 0;
        case Type::FLOAT:
            return get_float() != 0.0;
        case Type::BOOLEAN:
            return get_bool();
        case Type::STRING:
            return !get_string().empty();
        case Type::NULL_VALUE:
            return false;
        default:
            return false;
    }
}

std::string ConstantValue::to_string() const {
    switch (type_) {
        case Type::INTEGER:
            return std::to_string(get_int());
        case Type::FLOAT:
            return std::to_string(get_float());
        case Type::BOOLEAN:
            return get_bool() ? "true" : "false";
        case Type::STRING:
            return get_string();
        case Type::NULL_VALUE:
            return "null";
        case Type::UNDEFINED:
            return "undefined";
        default:
            return "";
    }
}

// ===================================================================
// Statistics Implementation
// ===================================================================

void ConstantFolding::Stats::reset() {
    constants_folded = 0;
    constants_propagated = 0;
    algebraic_simplifications = 0;
    strength_reductions = 0;
    total_optimizations = 0;
}

std::string ConstantFolding::Stats::to_string() const {
    std::ostringstream oss;
    oss << "Constant Folding Statistics:\n";
    oss << "  Constants folded: " << constants_folded << "\n";
    oss << "  Constants propagated: " << constants_propagated << "\n";
    oss << "  Algebraic simplifications: " << algebraic_simplifications << "\n";
    oss << "  Strength reductions: " << strength_reductions << "\n";
    oss << "  Total optimizations: " << total_optimizations << "\n";
    return oss.str();
}

// ===================================================================
// Main Optimization Entry Point
// ===================================================================

bool ConstantFolding::optimize(Function* function) {
    if (!function) {
        return false;
    }

    if (verbose_) {
        std::cout << "Running Constant Folding on function: " 
                  << function->get_name() << std::endl;
    }

    bool changed = false;

    if (config_.max_iterations > 1) {
        changed = run_iterative_optimization(function);
    } else {
        // Single pass
        if (config_.enable_folding) {
            changed |= fold_constants(function);
        }

        if (config_.enable_propagation) {
            build_constant_map(function);
            changed |= propagate_constants(function);
        }

        if (config_.enable_algebraic) {
            changed |= simplify_algebraic(function);
        }

        if (config_.enable_strength_reduction) {
            changed |= reduce_strength(function);
        }
    }

    if (changed) {
        stats_.total_optimizations++;
    }

    return changed;
}

bool ConstantFolding::optimize_module(std::vector<Function*>& functions) {
    bool changed = false;

    for (auto* function : functions) {
        changed |= optimize(function);
    }

    return changed;
}

bool ConstantFolding::run_iterative_optimization(Function* function) {
    bool global_changed = false;
    size_t iteration = 0;

    while (iteration < config_.max_iterations) {
        bool changed = false;

        if (config_.enable_folding) {
            changed |= fold_constants(function);
        }

        if (config_.enable_propagation) {
            build_constant_map(function);
            changed |= propagate_constants(function);
        }

        if (config_.enable_algebraic) {
            changed |= simplify_algebraic(function);
        }

        if (config_.enable_strength_reduction) {
            changed |= reduce_strength(function);
        }

        if (!changed) {
            break;
        }

        global_changed = true;
        iteration++;
    }

    if (verbose_ && iteration > 0) {
        std::cout << "Constant folding converged after " << iteration 
                  << " iterations" << std::endl;
    }

    return global_changed;
}

// ===================================================================
// Phase 1: Constant Folding
// ===================================================================

bool ConstantFolding::fold_constants(Function* function) {
    bool changed = false;

    for (auto* block : function->get_basic_blocks()) {
        for (auto* inst : block->get_instructions()) {
            auto folded = try_fold_instruction(inst);
            
            if (folded.has_value()) {
                replace_instruction_with_constant(inst, folded.value());
                stats_.constants_folded++;
                changed = true;

                if (verbose_) {
                    print_optimization("Folded constant: " + inst->to_string());
                }
            }
        }
    }

    return changed;
}

std::optional<ConstantValue> ConstantFolding::try_fold_instruction(Instruction* inst) {
    if (!inst || !has_constant_operands(inst)) {
        return std::nullopt;
    }

    auto operands = get_constant_operands(inst);
    
    switch (inst->get_opcode()) {
        // Binary operations
        case Opcode::ADD:
        case Opcode::SUB:
        case Opcode::MUL:
        case Opcode::DIV:
        case Opcode::MOD:
        case Opcode::AND:
        case Opcode::OR:
        case Opcode::XOR:
        case Opcode::SHL:
        case Opcode::SHR:
            if (operands.size() == 2) {
                return fold_binary_op(inst->get_opcode(), operands[0], operands[1]);
            }
            break;

        // Unary operations
        case Opcode::NEG:
        case Opcode::NOT:
            if (operands.size() == 1) {
                return fold_unary_op(inst->get_opcode(), operands[0]);
            }
            break;

        // Comparison operations
        case Opcode::CMP_EQ:
        case Opcode::CMP_NE:
        case Opcode::CMP_LT:
        case Opcode::CMP_LE:
        case Opcode::CMP_GT:
        case Opcode::CMP_GE:
            if (operands.size() == 2) {
                return fold_comparison(inst->get_opcode(), operands[0], operands[1]);
            }
            break;

        default:
            break;
    }

    return std::nullopt;
}

std::optional<ConstantValue> ConstantFolding::fold_binary_op(
    Opcode op,
    const ConstantValue& left,
    const ConstantValue& right
) {
    using namespace arithmetic;

    // Integer operations
    if (left.is_integer() && right.is_integer()) {
        int64_t a = left.get_int();
        int64_t b = right.get_int();

        switch (op) {
            case Opcode::ADD:
                if (auto result = add_int(a, b)) {
                    return ConstantValue::make_int(*result);
                }
                break;
            case Opcode::SUB:
                if (auto result = sub_int(a, b)) {
                    return ConstantValue::make_int(*result);
                }
                break;
            case Opcode::MUL:
                if (auto result = mul_int(a, b)) {
                    return ConstantValue::make_int(*result);
                }
                break;
            case Opcode::DIV:
                if (auto result = div_int(a, b)) {
                    return ConstantValue::make_int(*result);
                }
                break;
            case Opcode::MOD:
                if (auto result = mod_int(a, b)) {
                    return ConstantValue::make_int(*result);
                }
                break;
            case Opcode::AND:
                return ConstantValue::make_int(and_int(a, b));
            case Opcode::OR:
                return ConstantValue::make_int(or_int(a, b));
            case Opcode::XOR:
                return ConstantValue::make_int(xor_int(a, b));
            case Opcode::SHL:
                return ConstantValue::make_int(shl_int(a, b));
            case Opcode::SHR:
                return ConstantValue::make_int(shr_int(a, b));
            default:
                break;
        }
    }

    // Float operations
    if (left.is_numeric() && right.is_numeric()) {
        double a = left.to_number();
        double b = right.to_number();

        switch (op) {
            case Opcode::ADD:
                return ConstantValue::make_float(add_float(a, b));
            case Opcode::SUB:
                return ConstantValue::make_float(sub_float(a, b));
            case Opcode::MUL:
                return ConstantValue::make_float(mul_float(a, b));
            case Opcode::DIV:
                if (auto result = div_float(a, b)) {
                    return ConstantValue::make_float(*result);
                }
                break;
            default:
                break;
        }
    }

    return std::nullopt;
}

std::optional<ConstantValue> ConstantFolding::fold_unary_op(
    Opcode op,
    const ConstantValue& operand
) {
    using namespace arithmetic;

    switch (op) {
        case Opcode::NEG:
            if (operand.is_integer()) {
                return ConstantValue::make_int(-operand.get_int());
            } else if (operand.is_float()) {
                return ConstantValue::make_float(-operand.get_float());
            }
            break;

        case Opcode::NOT:
            if (operand.is_integer()) {
                return ConstantValue::make_int(not_int(operand.get_int()));
            } else if (operand.is_boolean()) {
                return ConstantValue::make_bool(logical_not(operand.get_bool()));
            }
            break;

        default:
            break;
    }

    return std::nullopt;
}

std::optional<ConstantValue> ConstantFolding::fold_comparison(
    Opcode op,
    const ConstantValue& left,
    const ConstantValue& right
) {
    using namespace arithmetic;

    bool result = false;

    switch (op) {
        case Opcode::CMP_EQ:
            result = cmp_eq(left, right);
            break;
        case Opcode::CMP_NE:
            result = cmp_ne(left, right);
            break;
        case Opcode::CMP_LT:
            result = cmp_lt(left, right);
            break;
        case Opcode::CMP_LE:
            result = cmp_le(left, right);
            break;
        case Opcode::CMP_GT:
            result = cmp_gt(left, right);
            break;
        case Opcode::CMP_GE:
            result = cmp_ge(left, right);
            break;
        default:
            return std::nullopt;
    }

    return ConstantValue::make_bool(result);
}

// ===================================================================
// Phase 2: Constant Propagation
// ===================================================================

bool ConstantFolding::propagate_constants(Function* function) {
    bool changed = false;

    for (const auto& pair : constant_map_) {
        Value* variable = pair.first;
        const ConstantValue& constant = pair.second;

        if (replace_with_constant(variable, constant)) {
            stats_.constants_propagated++;
            changed = true;

            if (verbose_) {
                print_optimization("Propagated constant: " + variable->get_name());
            }
        }
    }

    return changed;
}

void ConstantFolding::build_constant_map(Function* function) {
    constant_map_.clear();

    for (auto* block : function->get_basic_blocks()) {
        for (auto* inst : block->get_instructions()) {
            // Check for simple constant assignments: x = constant
            if (inst->is_assignment() && inst->get_num_operands() == 1) {
                Value* lhs = inst->get_lhs();
                Value* rhs = inst->get_operand(0);

                if (is_constant(rhs)) {
                    auto const_val = get_constant_value(rhs);
                    if (const_val.has_value()) {
                        constant_map_[lhs] = const_val.value();
                    }
                }
            }
        }
    }
}

std::optional<ConstantValue> ConstantFolding::get_constant_value(Value* value) {
    if (!value) {
        return std::nullopt;
    }

    // Check if it's a literal constant
    if (auto* constant = dynamic_cast<Constant*>(value)) {
        switch (constant->get_type()) {
            case ConstantType::INTEGER:
                return ConstantValue::make_int(constant->get_int_value());
            case ConstantType::FLOAT:
                return ConstantValue::make_float(constant->get_float_value());
            case ConstantType::BOOLEAN:
                return ConstantValue::make_bool(constant->get_bool_value());
            case ConstantType::STRING:
                return ConstantValue::make_string(constant->get_string_value());
            default:
                break;
        }
    }

    // Check constant map
    auto it = constant_map_.find(value);
    if (it != constant_map_.end()) {
        return it->second;
    }

    return std::nullopt;
}

bool ConstantFolding::replace_with_constant(Value* variable, const ConstantValue& constant) {
    // Implementation depends on IR structure
    // Replace all uses of variable with the constant value
    return false;  // Placeholder
}

// ===================================================================
// Phase 3: Algebraic Simplification
// ===================================================================

bool ConstantFolding::simplify_algebraic(Function* function) {
    bool changed = false;

    for (auto* block : function->get_basic_blocks()) {
        for (auto* inst : block->get_instructions()) {
            if (try_simplify_instruction(inst)) {
                stats_.algebraic_simplifications++;
                changed = true;
            }
        }
    }

    return changed;
}

bool ConstantFolding::try_simplify_instruction(Instruction* inst) {
    bool changed = false;

    changed |= simplify_identity(inst);
    changed |= simplify_absorption(inst);
    changed |= simplify_idempotent(inst);
    changed |= simplify_inverse(inst);

    return changed;
}

bool ConstantFolding::simplify_identity(Instruction* inst) {
    if (!inst || inst->get_num_operands() != 2) {
        return false;
    }

    Value* left = inst->get_operand(0);
    Value* right = inst->get_operand(1);

    // Check for identity operations
    switch (inst->get_opcode()) {
        case Opcode::ADD:
        case Opcode::SUB:
        case Opcode::OR:
        case Opcode::XOR:
            // x + 0 = x, x - 0 = x, x | 0 = x, x ^ 0 = x
            if (is_zero(right)) {
                replace_instruction_with_value(inst, left);
                return true;
            }
            if (inst->get_opcode() == Opcode::ADD && is_zero(left)) {
                // 0 + x = x
                replace_instruction_with_value(inst, right);
                return true;
            }
            break;

        case Opcode::MUL:
        case Opcode::DIV:
            // x * 1 = x, x / 1 = x
            if (is_one(right)) {
                replace_instruction_with_value(inst, left);
                return true;
            }
            if (inst->get_opcode() == Opcode::MUL && is_one(left)) {
                // 1 * x = x
                replace_instruction_with_value(inst, right);
                return true;
            }
            break;

        case Opcode::AND:
            // x & -1 = x (all bits set)
            if (is_all_ones(right)) {
                replace_instruction_with_value(inst, left);
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

bool ConstantFolding::simplify_absorption(Instruction* inst) {
    if (!inst || inst->get_num_operands() != 2) {
        return false;
    }

    Value* left = inst->get_operand(0);
    Value* right = inst->get_operand(1);

    switch (inst->get_opcode()) {
        case Opcode::MUL:
            // x * 0 = 0
            if (is_zero(left) || is_zero(right)) {
                replace_instruction_with_constant(inst, ConstantValue::make_int(0));
                return true;
            }
            break;

        case Opcode::AND:
            // x & 0 = 0
            if (is_zero(left) || is_zero(right)) {
                replace_instruction_with_constant(inst, ConstantValue::make_int(0));
                return true;
            }
            break;

        case Opcode::OR:
            // x | -1 = -1
            if (is_all_ones(left)) {
                replace_instruction_with_value(inst, left);
                return true;
            }
            if (is_all_ones(right)) {
                replace_instruction_with_value(inst, right);
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

bool ConstantFolding::simplify_idempotent(Instruction* inst) {
    if (!inst || inst->get_num_operands() != 2) {
        return false;
    }

    Value* left = inst->get_operand(0);
    Value* right = inst->get_operand(1);

    if (left != right) {
        return false;
    }

    switch (inst->get_opcode()) {
        case Opcode::AND:
        case Opcode::OR:
            // x & x = x, x | x = x
            replace_instruction_with_value(inst, left);
            return true;

        case Opcode::XOR:
            // x ^ x = 0
            replace_instruction_with_constant(inst, ConstantValue::make_int(0));
            return true;

        default:
            break;
    }

    return false;
}

bool ConstantFolding::simplify_inverse(Instruction* inst) {
    if (!inst || inst->get_num_operands() != 2) {
        return false;
    }

    Value* left = inst->get_operand(0);
    Value* right = inst->get_operand(1);

    if (left != right) {
        return false;
    }

    switch (inst->get_opcode()) {
        case Opcode::SUB:
            // x - x = 0
            replace_instruction_with_constant(inst, ConstantValue::make_int(0));
            return true;

        case Opcode::DIV:
            // x / x = 1 (assuming x != 0)
            replace_instruction_with_constant(inst, ConstantValue::make_int(1));
            return true;

        case Opcode::MOD:
            // x % x = 0
            replace_instruction_with_constant(inst, ConstantValue::make_int(0));
            return true;

        default:
            break;
    }

    return false;
}

// ===================================================================
// Phase 4: Strength Reduction
// ===================================================================

bool ConstantFolding::reduce_strength(Function* function) {
    bool changed = false;

    for (auto* block : function->get_basic_blocks()) {
        for (auto* inst : block->get_instructions()) {
            changed |= reduce_multiply_to_shift(inst);
            changed |= reduce_divide_to_shift(inst);
            changed |= reduce_modulo_to_and(inst);
            changed |= reduce_power_to_multiply(inst);
        }
    }

    return changed;
}

bool ConstantFolding::reduce_multiply_to_shift(Instruction* inst) {
    if (!inst || inst->get_opcode() != Opcode::MUL) {
        return false;
    }

    Value* right = inst->get_operand(1);
    auto constant = get_constant_value(right);

    if (constant.has_value() && constant->is_integer()) {
        int64_t value = constant->get_int();
        
        if (is_power_of_two(value)) {
            // x * 2^n → x << n
            int shift = get_log2(value);
            replace_multiply_with_shift(inst, shift);
            stats_.strength_reductions++;
            return true;
        }
    }

    return false;
}

bool ConstantFolding::reduce_divide_to_shift(Instruction* inst) {
    if (!inst || inst->get_opcode() != Opcode::DIV) {
        return false;
    }

    Value* right = inst->get_operand(1);
    auto constant = get_constant_value(right);

    if (constant.has_value() && constant->is_integer()) {
        int64_t value = constant->get_int();
        
        if (is_power_of_two(value)) {
            // x / 2^n → x >> n
            int shift = get_log2(value);
            replace_divide_with_shift(inst, shift);
            stats_.strength_reductions++;
            return true;
        }
    }

    return false;
}

bool ConstantFolding::reduce_modulo_to_and(Instruction* inst) {
    if (!inst || inst->get_opcode() != Opcode::MOD) {
        return false;
    }

    Value* right = inst->get_operand(1);
    auto constant = get_constant_value(right);

    if (constant.has_value() && constant->is_integer()) {
        int64_t value = constant->get_int();
        
        if (is_power_of_two(value)) {
            // x % 2^n → x & (2^n - 1)
            int64_t mask = value - 1;
            replace_modulo_with_and(inst, mask);
            stats_.strength_reductions++;
            return true;
        }
    }

    return false;
}

bool ConstantFolding::reduce_power_to_multiply(Instruction* inst) {
    // x^2 → x*x, x^3 → x*x*x (for small powers)
    // Implementation depends on having POW instruction
    return false;
}

// ===================================================================
// Utility Functions
// ===================================================================

bool ConstantFolding::is_constant(Value* value) {
    return dynamic_cast<Constant*>(value) != nullptr || 
           constant_map_.find(value) != constant_map_.end();
}

bool ConstantFolding::has_constant_operands(Instruction* inst) {
    if (!inst) {
        return false;
    }

    for (size_t i = 0; i < inst->get_num_operands(); i++) {
        if (!is_constant(inst->get_operand(i))) {
            return false;
        }
    }

    return inst->get_num_operands() > 0;
}

std::vector<ConstantValue> ConstantFolding::get_constant_operands(Instruction* inst) {
    std::vector<ConstantValue> result;

    if (!inst) {
        return result;
    }

    for (size_t i = 0; i < inst->get_num_operands(); i++) {
        auto val = get_constant_value(inst->get_operand(i));
        if (val.has_value()) {
            result.push_back(val.value());
        }
    }

    return result;
}

void ConstantFolding::replace_instruction_with_constant(
    Instruction* inst,
    const ConstantValue& constant
) {
    // Implementation depends on IR structure
    // Create a constant value and replace instruction with it
}

bool ConstantFolding::is_power_of_two(int64_t n) const {
    return n > 0 && (n & (n - 1)) == 0;
}

int ConstantFolding::get_log2(int64_t n) const {
    int log = 0;
    while (n > 1) {
        n >>= 1;
        log++;
    }
    return log;
}

void ConstantFolding::print_optimization(const std::string& message) {
    std::cout << "[ConstantFolding] " << message << std::endl;
}

// ===================================================================
// Arithmetic Operations
// ===================================================================

namespace arithmetic {

std::optional<int64_t> add_int(int64_t a, int64_t b) {
    // Check for overflow
    if ((b > 0 && a > std::numeric_limits<int64_t>::max() - b) ||
        (b < 0 && a < std::numeric_limits<int64_t>::min() - b)) {
        return std::nullopt;
    }
    return a + b;
}

std::optional<int64_t> sub_int(int64_t a, int64_t b) {
    if ((b < 0 && a > std::numeric_limits<int64_t>::max() + b) ||
        (b > 0 && a < std::numeric_limits<int64_t>::min() + b)) {
        return std::nullopt;
    }
    return a - b;
}

std::optional<int64_t> mul_int(int64_t a, int64_t b) {
    if (a == 0 || b == 0) {
        return 0;
    }
    if (a > std::numeric_limits<int64_t>::max() / b ||
        a < std::numeric_limits<int64_t>::min() / b) {
        return std::nullopt;
    }
    return a * b;
}

std::optional<int64_t> div_int(int64_t a, int64_t b) {
    if (b == 0) {
        return std::nullopt;  // Division by zero
    }
    return a / b;
}

std::optional<int64_t> mod_int(int64_t a, int64_t b) {
    if (b == 0) {
        return std::nullopt;
    }
    return a % b;
}

double add_float(double a, double b) {
    return a + b;
}

double sub_float(double a, double b) {
    return a - b;
}

double mul_float(double a, double b) {
    return a * b;
}

std::optional<double> div_float(double a, double b) {
    if (b == 0.0) {
        return std::nullopt;
    }
    return a / b;
}

int64_t and_int(int64_t a, int64_t b) {
    return a & b;
}

int64_t or_int(int64_t a, int64_t b) {
    return a | b;
}

int64_t xor_int(int64_t a, int64_t b) {
    return a ^ b;
}

int64_t not_int(int64_t a) {
    return ~a;
}

int64_t shl_int(int64_t a, int64_t b) {
    return a << b;
}

int64_t shr_int(int64_t a, int64_t b) {
    return a >> b;
}

bool cmp_eq(const ConstantValue& a, const ConstantValue& b) {
    if (a.is_numeric() && b.is_numeric()) {
        return a.to_number() == b.to_number();
    }
    if (a.is_boolean() && b.is_boolean()) {
        return a.get_bool() == b.get_bool();
    }
    if (a.is_string() && b.is_string()) {
        return a.get_string() == b.get_string();
    }
    return false;
}

bool cmp_ne(const ConstantValue& a, const ConstantValue& b) {
    return !cmp_eq(a, b);
}

bool cmp_lt(const ConstantValue& a, const ConstantValue& b) {
    if (a.is_numeric() && b.is_numeric()) {
        return a.to_number() < b.to_number();
    }
    return false;
}

bool cmp_le(const ConstantValue& a, const ConstantValue& b) {
    return cmp_lt(a, b) || cmp_eq(a, b);
}

bool cmp_gt(const ConstantValue& a, const ConstantValue& b) {
    return !cmp_le(a, b);
}

bool cmp_ge(const ConstantValue& a, const ConstantValue& b) {
    return !cmp_lt(a, b);
}

bool logical_and(bool a, bool b) {
    return a && b;
}

bool logical_or(bool a, bool b) {
    return a || b;
}

bool logical_not(bool a) {
    return !a;
}

} // namespace arithmetic

// ===================================================================
// Factory Functions
// ===================================================================

std::unique_ptr<ConstantFolding> create_standard_constant_folder() {
    auto cf = std::make_unique<ConstantFolding>();
    
    ConstantFolding::Config config;
    config.enable_folding = true;
    config.enable_propagation = true;
    config.enable_algebraic = true;
    config.enable_strength_reduction = true;
    config.aggressive_mode = false;
    config.max_iterations = 3;
    
    cf->set_config(config);
    return cf;
}

std::unique_ptr<ConstantFolding> create_aggressive_constant_folder() {
    auto cf = std::make_unique<ConstantFolding>();
    
    ConstantFolding::Config config;
    config.enable_folding = true;
    config.enable_propagation = true;
    config.enable_algebraic = true;
    config.enable_strength_reduction = true;
    config.aggressive_mode = true;
    config.max_iterations = 10;
    
    cf->set_config(config);
    return cf;
}

bool needs_constant_folding(Function* function) {
    if (!function) {
        return false;
    }

    // Quick heuristic: if function has constants, likely benefits
    for (auto* block : function->get_basic_blocks()) {
        for (auto* inst : block->get_instructions()) {
            for (size_t i = 0; i < inst->get_num_operands(); i++) {
                if (dynamic_cast<Constant*>(inst->get_operand(i))) {
                    return true;
                }
            }
        }
    }

    return false;
}

} // namespace optimizer
} // namespace sad
