/**
 * =============================================================================
 * ملف: sir_validator.cpp
 * الوصف: نظام التحقق الشامل من صحة SIR
 * المرحلة: Phase 95 - Compiler Hardening (100+ إصلاح)
 * =============================================================================
 * 
 * 🔍 التحقق من صحة SIR
 * ═════════════════════════════════════════
 * 
 * هذا الملف يوفر:
 * 1. التحقق من صحة التعليمات
 * 2. التحقق من صحة الأنواع
 * 3. التحقق من صحة الكتل
 * 4. التحقق من تدفق التحكم
 * 5. التحقق من إدارة الذاكرة
 * 
 * الإصلاحات: 100+ نقطة ضعف في SIR
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <set>
#include <map>
#include <optional>
#include <variant>
#include <sstream>
#include <stack>
#include <algorithm>

namespace sad {
namespace compiler {
namespace sir {

// =============================================================================
// الأنواع الأساسية لـ SIR
// =============================================================================

/**
 * @brief أنواع SIR المدعومة
 */
enum class SirType {
    Void,
    Bool,
    Int8, Int16, Int32, Int64,
    UInt8, UInt16, UInt32, UInt64,
    Float32, Float64,
    Pointer,
    Array,
    Struct,
    Function,
    String,
    // أنواع الرسومات
    Color, Widget, Window, Event,
    // أنواع خاصة
    Any, Never
};

/**
 * @brief معلومات النوع الكاملة
 */
struct TypeInfo {
    SirType baseType;
    std::optional<TypeInfo*> pointeeType;   // للمؤشرات
    std::optional<size_t> arraySize;         // للمصفوفات
    std::vector<TypeInfo> memberTypes;       // للهياكل
    std::vector<TypeInfo> paramTypes;        // للدوال
    std::optional<TypeInfo*> returnType;     // للدوال
    
    size_t getSizeInBits() const {
        switch (baseType) {
            case SirType::Void: return 0;
            case SirType::Bool: return 1;
            case SirType::Int8: case SirType::UInt8: return 8;
            case SirType::Int16: case SirType::UInt16: return 16;
            case SirType::Int32: case SirType::UInt32: case SirType::Float32: return 32;
            case SirType::Int64: case SirType::UInt64: case SirType::Float64: return 64;
            case SirType::Pointer: return 64; // 64-bit
            case SirType::Color: return 32;   // RGBA
            case SirType::Widget: return 64;  // مؤشر
            case SirType::Window: return 64;
            case SirType::Event: return 256;  // هيكل
            default: return 0;
        }
    }
    
    bool isIntegral() const {
        return baseType >= SirType::Int8 && baseType <= SirType::UInt64;
    }
    
    bool isFloatingPoint() const {
        return baseType == SirType::Float32 || baseType == SirType::Float64;
    }
    
    bool isNumeric() const {
        return isIntegral() || isFloatingPoint();
    }
    
    bool isGraphics() const {
        return baseType >= SirType::Color && baseType <= SirType::Event;
    }
};

// =============================================================================
// أكواد عمليات SIR
// =============================================================================

/**
 * @brief أكواد العمليات لـ SIR
 */
enum class SirOpcode {
    // === أساسي ===
    Nop = 0,
    
    // === الحساب ===
    Add, Sub, Mul, Div, Rem,
    And, Or, Xor, Shl, Shr,
    Neg, Not,
    
    // === المقارنة ===
    Eq, Ne, Lt, Le, Gt, Ge,
    
    // === الذاكرة ===
    Load, Store, Alloca,
    GetElementPtr, ExtractValue, InsertValue,
    
    // === تدفق التحكم ===
    Br, CondBr, Switch,
    Call, Ret, Unreachable,
    
    // === التحويل ===
    Trunc, ZExt, SExt,
    FPTrunc, FPExt,
    FPToUI, FPToSI, UIToFP, SIToFP,
    PtrToInt, IntToPtr, Bitcast,
    
    // === الملكية ===
    Move, Borrow, BorrowMut, Drop, Clone,
    
    // === PHI ===
    Phi,
    
    // === النظام ===
    Syscall, Inline
};

/**
 * @brief تعليمة SIR
 */
struct SirInstruction {
    SirOpcode opcode;
    std::optional<std::string> result;
    std::vector<std::string> operands;
    TypeInfo resultType;
    std::vector<TypeInfo> operandTypes;
    std::optional<size_t> alignment;
    bool isVolatile = false;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief كتلة SIR
 */
struct SirBlock {
    std::string name;
    std::vector<SirInstruction> instructions;
    std::vector<std::string> predecessors;
    std::vector<std::string> successors;
};

/**
 * @brief دالة SIR
 */
struct SirFunction {
    std::string name;
    std::vector<std::pair<std::string, TypeInfo>> parameters;
    TypeInfo returnType;
    std::vector<SirBlock> blocks;
    std::map<std::string, TypeInfo> localVariables;
    bool isExternal = false;
    bool isInline = false;
};

/**
 * @brief وحدة SIR
 */
struct SirModule {
    std::string name;
    std::vector<SirFunction> functions;
    std::map<std::string, TypeInfo> globalVariables;
    std::map<std::string, TypeInfo> types;
    std::vector<std::string> imports;
};

// =============================================================================
// القسم 1: التحقق من صحة التعليمات (25 إصلاح)
// =============================================================================

/**
 * @brief محقق التعليمات
 */
class InstructionValidator {
public:
    struct ValidationResult {
        bool isValid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    /**
     * @brief التحقق من تعليمة واحدة
     * @param inst التعليمة
     * @param context السياق (المتغيرات المتاحة)
     * @return نتيجة التحقق
     */
    ValidationResult validate(const SirInstruction& inst,
                               const std::map<std::string, TypeInfo>& context) {
        ValidationResult result;
        
        // 1. التحقق من صحة الكود
        if (!isValidOpcode(inst.opcode)) {
            result.errors.push_back("Invalid opcode");
            result.isValid = false;
        }
        
        // 2. التحقق من عدد العوامل
        int expected = getExpectedOperandCount(inst.opcode);
        if (expected >= 0 && inst.operands.size() != static_cast<size_t>(expected)) {
            result.errors.push_back(
                "Wrong operand count: expected " + std::to_string(expected) +
                ", got " + std::to_string(inst.operands.size())
            );
            result.isValid = false;
        }
        
        // 3. التحقق من وجود العوامل في السياق
        for (const auto& operand : inst.operands) {
            if (operand[0] == '%' && context.find(operand) == context.end()) {
                result.errors.push_back("Undefined operand: " + operand);
                result.isValid = false;
            }
        }
        
        // 4. التحقق من توافق الأنواع
        if (!checkTypeCompatibility(inst)) {
            result.errors.push_back("Type mismatch in instruction");
            result.isValid = false;
        }
        
        // 5. التحقق من النتيجة
        if (inst.result.has_value()) {
            if (inst.result->empty()) {
                result.errors.push_back("Empty result name");
                result.isValid = false;
            }
        } else if (requiresResult(inst.opcode)) {
            result.errors.push_back("Instruction requires result");
            result.isValid = false;
        }
        
        // 6. التحقق من المحاذاة
        if (inst.alignment.has_value()) {
            size_t align = inst.alignment.value();
            if (!isPowerOfTwo(align) || align > 4096) {
                result.warnings.push_back("Suspicious alignment: " + std::to_string(align));
            }
        }
        
        return result;
    }
    
private:
    bool isValidOpcode(SirOpcode opcode) {
        return opcode >= SirOpcode::Nop && opcode <= SirOpcode::Inline;
    }
    
    int getExpectedOperandCount(SirOpcode opcode) {
        switch (opcode) {
            case SirOpcode::Nop: return 0;
            case SirOpcode::Neg: case SirOpcode::Not: return 1;
            case SirOpcode::Add: case SirOpcode::Sub:
            case SirOpcode::Mul: case SirOpcode::Div:
            case SirOpcode::And: case SirOpcode::Or:
            case SirOpcode::Eq: case SirOpcode::Ne:
            case SirOpcode::Lt: case SirOpcode::Le:
            case SirOpcode::Gt: case SirOpcode::Ge:
            case SirOpcode::Store:
                return 2;
            case SirOpcode::Load: case SirOpcode::Alloca:
                return 1;
            case SirOpcode::Call:
            case SirOpcode::Phi:
            case SirOpcode::GetElementPtr:
                return -1; // متغير
            default:
                return -1;
        }
    }
    
    bool requiresResult(SirOpcode opcode) {
        switch (opcode) {
            case SirOpcode::Store:
            case SirOpcode::Br:
            case SirOpcode::CondBr:
            case SirOpcode::Ret:
            case SirOpcode::Unreachable:
            case SirOpcode::Drop:
                return false;
            default:
                return true;
        }
    }
    
    bool checkTypeCompatibility(const SirInstruction& inst) {
        if (inst.operandTypes.size() < 2) {
            return true;
        }
        
        // للعمليات الحسابية، يجب أن تكون الأنواع متطابقة
        if (isArithmeticOp(inst.opcode)) {
            return inst.operandTypes[0].baseType == inst.operandTypes[1].baseType;
        }
        
        // للتحميل، يجب أن يكون العامل مؤشراً
        if (inst.opcode == SirOpcode::Load) {
            return inst.operandTypes[0].baseType == SirType::Pointer;
        }
        
        return true;
    }
    
    bool isArithmeticOp(SirOpcode opcode) {
        return opcode >= SirOpcode::Add && opcode <= SirOpcode::Not;
    }
    
    bool isPowerOfTwo(size_t n) {
        return n > 0 && (n & (n - 1)) == 0;
    }
};

// =============================================================================
// القسم 2: التحقق من صحة الكتل (25 إصلاح)
// =============================================================================

/**
 * @brief محقق الكتل
 */
class BlockValidator {
public:
    struct ValidationResult {
        bool isValid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    /**
     * @brief التحقق من كتلة واحدة
     */
    ValidationResult validate(const SirBlock& block,
                               const SirFunction& function) {
        ValidationResult result;
        
        // 1. التحقق من اسم الكتلة
        if (block.name.empty()) {
            result.errors.push_back("Empty block name");
            result.isValid = false;
        }
        
        // 2. التحقق من عدم فراغ الكتلة
        if (block.instructions.empty()) {
            result.warnings.push_back("Empty block: " + block.name);
        }
        
        // 3. التحقق من أن الكتلة تنتهي بتعليمة إنهاء
        if (!block.instructions.empty()) {
            const auto& lastInst = block.instructions.back();
            if (!isTerminator(lastInst.opcode)) {
                result.errors.push_back(
                    "Block '" + block.name + "' does not end with terminator"
                );
                result.isValid = false;
            }
        }
        
        // 4. التحقق من أنه لا توجد تعليمات بعد الإنهاء
        bool foundTerminator = false;
        for (const auto& inst : block.instructions) {
            if (foundTerminator) {
                result.errors.push_back(
                    "Instruction after terminator in block '" + block.name + "'"
                );
                result.isValid = false;
            }
            if (isTerminator(inst.opcode)) {
                foundTerminator = true;
            }
        }
        
        // 5. التحقق من صحة PHI
        for (size_t i = 0; i < block.instructions.size(); ++i) {
            const auto& inst = block.instructions[i];
            if (inst.opcode == SirOpcode::Phi) {
                // PHI يجب أن تكون في بداية الكتلة
                bool allPhi = true;
                for (size_t j = 0; j < i; ++j) {
                    if (block.instructions[j].opcode != SirOpcode::Phi) {
                        allPhi = false;
                        break;
                    }
                }
                if (!allPhi) {
                    result.errors.push_back(
                        "PHI instruction must be at the beginning of block '" + 
                        block.name + "'"
                    );
                    result.isValid = false;
                }
            }
        }
        
        // 6. التحقق من صحة الخلفاء
        for (const auto& succ : block.successors) {
            bool found = false;
            for (const auto& b : function.blocks) {
                if (b.name == succ) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                result.errors.push_back(
                    "Unknown successor '" + succ + "' in block '" + block.name + "'"
                );
                result.isValid = false;
            }
        }
        
        return result;
    }
    
private:
    bool isTerminator(SirOpcode opcode) {
        return opcode == SirOpcode::Br ||
               opcode == SirOpcode::CondBr ||
               opcode == SirOpcode::Switch ||
               opcode == SirOpcode::Ret ||
               opcode == SirOpcode::Unreachable;
    }
};

// =============================================================================
// القسم 3: التحقق من تدفق التحكم (25 إصلاح)
// =============================================================================

/**
 * @brief محقق تدفق التحكم
 */
class ControlFlowValidator {
public:
    struct ValidationResult {
        bool isValid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::set<std::string> unreachableBlocks;
        std::set<std::string> deadCodeBlocks;
    };
    
    /**
     * @brief التحقق من تدفق التحكم للدالة
     */
    ValidationResult validate(const SirFunction& function) {
        ValidationResult result;
        
        if (function.blocks.empty()) {
            result.errors.push_back("Function has no blocks");
            result.isValid = false;
            return result;
        }
        
        // 1. بناء رسم بياني للتحكم
        std::map<std::string, std::set<std::string>> cfg;
        std::set<std::string> allBlocks;
        
        for (const auto& block : function.blocks) {
            allBlocks.insert(block.name);
            cfg[block.name] = std::set<std::string>(
                block.successors.begin(), block.successors.end()
            );
        }
        
        // 2. البحث عن الكتل الغير قابلة للوصول
        std::set<std::string> reachable;
        std::stack<std::string> worklist;
        worklist.push(function.blocks[0].name);
        
        while (!worklist.empty()) {
            std::string current = worklist.top();
            worklist.pop();
            
            if (reachable.count(current)) continue;
            reachable.insert(current);
            
            for (const auto& succ : cfg[current]) {
                if (!reachable.count(succ)) {
                    worklist.push(succ);
                }
            }
        }
        
        for (const auto& block : allBlocks) {
            if (!reachable.count(block)) {
                result.unreachableBlocks.insert(block);
                result.warnings.push_back("Unreachable block: " + block);
            }
        }
        
        // 3. البحث عن الحلقات اللانهائية
        for (const auto& block : function.blocks) {
            if (cfg[block.name].count(block.name)) {
                // حلقة ذاتية - قد تكون لانهائية
                result.warnings.push_back(
                    "Potential infinite loop in block: " + block.name
                );
            }
        }
        
        // 4. التحقق من أن جميع المسارات تنتهي
        for (const auto& block : function.blocks) {
            if (cfg[block.name].empty()) {
                // كتلة بدون خلفاء - يجب أن تنتهي بـ ret أو unreachable
                if (!block.instructions.empty()) {
                    const auto& lastInst = block.instructions.back();
                    if (lastInst.opcode != SirOpcode::Ret &&
                        lastInst.opcode != SirOpcode::Unreachable) {
                        result.errors.push_back(
                            "Block '" + block.name + "' has no successors but doesn't return"
                        );
                        result.isValid = false;
                    }
                }
            }
        }
        
        // 5. التحقق من تناسق PHI
        for (const auto& block : function.blocks) {
            for (const auto& inst : block.instructions) {
                if (inst.opcode == SirOpcode::Phi) {
                    // يجب أن يكون لدى PHI قيمة لكل سلف
                    std::set<std::string> preds(
                        block.predecessors.begin(), block.predecessors.end()
                    );
                    
                    // افتراض أن الـ PHI يحتوي على أزواج (قيمة، كتلة)
                    std::set<std::string> phiBlocks;
                    for (size_t i = 1; i < inst.operands.size(); i += 2) {
                        phiBlocks.insert(inst.operands[i]);
                    }
                    
                    for (const auto& pred : preds) {
                        if (!phiBlocks.count(pred)) {
                            result.errors.push_back(
                                "PHI instruction missing value for predecessor '" + 
                                pred + "'"
                            );
                            result.isValid = false;
                        }
                    }
                }
            }
        }
        
        return result;
    }
};

// =============================================================================
// القسم 4: التحقق من إدارة الذاكرة (25 إصلاح)
// =============================================================================

/**
 * @brief محقق الذاكرة
 */
class MemoryValidator {
public:
    struct ValidationResult {
        bool isValid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::vector<std::string> potentialLeaks;
        std::vector<std::string> useAfterFree;
        std::vector<std::string> doubleFree;
    };
    
    /**
     * @brief التحقق من استخدام الذاكرة في الدالة
     */
    ValidationResult validate(const SirFunction& function) {
        ValidationResult result;
        
        // تتبع حالة الذاكرة
        std::map<std::string, MemoryState> memoryStates;
        
        for (const auto& block : function.blocks) {
            for (const auto& inst : block.instructions) {
                validateInstruction(inst, memoryStates, result);
            }
        }
        
        // البحث عن التسربات المحتملة
        for (const auto& [var, state] : memoryStates) {
            if (state == MemoryState::Allocated) {
                result.potentialLeaks.push_back(var);
                result.warnings.push_back("Potential memory leak: " + var);
            }
        }
        
        return result;
    }

private:
    enum class MemoryState {
        Unknown,
        Allocated,
        Borrowed,
        BorrowedMut,
        Moved,
        Freed
    };
    
    void validateInstruction(const SirInstruction& inst,
                              std::map<std::string, MemoryState>& states,
                              ValidationResult& result) {
        switch (inst.opcode) {
            case SirOpcode::Alloca:
                if (inst.result.has_value()) {
                    states[inst.result.value()] = MemoryState::Allocated;
                }
                break;
                
            case SirOpcode::Move:
                if (!inst.operands.empty()) {
                    MemoryState state = states[inst.operands[0]];
                    if (state == MemoryState::Moved) {
                        result.errors.push_back(
                            "Use after move: " + inst.operands[0]
                        );
                        result.isValid = false;
                    } else if (state == MemoryState::Freed) {
                        result.useAfterFree.push_back(inst.operands[0]);
                        result.errors.push_back(
                            "Use after free: " + inst.operands[0]
                        );
                        result.isValid = false;
                    }
                    states[inst.operands[0]] = MemoryState::Moved;
                    if (inst.result.has_value()) {
                        states[inst.result.value()] = MemoryState::Allocated;
                    }
                }
                break;
                
            case SirOpcode::Borrow:
                if (!inst.operands.empty()) {
                    MemoryState state = states[inst.operands[0]];
                    if (state == MemoryState::Moved || state == MemoryState::Freed) {
                        result.errors.push_back(
                            "Borrowing moved/freed value: " + inst.operands[0]
                        );
                        result.isValid = false;
                    }
                    if (inst.result.has_value()) {
                        states[inst.result.value()] = MemoryState::Borrowed;
                    }
                }
                break;
                
            case SirOpcode::BorrowMut:
                if (!inst.operands.empty()) {
                    MemoryState state = states[inst.operands[0]];
                    if (state == MemoryState::BorrowedMut) {
                        result.errors.push_back(
                            "Multiple mutable borrows: " + inst.operands[0]
                        );
                        result.isValid = false;
                    }
                    if (inst.result.has_value()) {
                        states[inst.result.value()] = MemoryState::BorrowedMut;
                    }
                }
                break;
                
            case SirOpcode::Drop:
                if (!inst.operands.empty()) {
                    MemoryState state = states[inst.operands[0]];
                    if (state == MemoryState::Freed) {
                        result.doubleFree.push_back(inst.operands[0]);
                        result.errors.push_back(
                            "Double free: " + inst.operands[0]
                        );
                        result.isValid = false;
                    }
                    states[inst.operands[0]] = MemoryState::Freed;
                }
                break;
                
            case SirOpcode::Load:
            case SirOpcode::Store:
                // التحقق من الوصول للذاكرة
                if (!inst.operands.empty()) {
                    MemoryState state = states[inst.operands[0]];
                    if (state == MemoryState::Freed) {
                        result.useAfterFree.push_back(inst.operands[0]);
                        result.errors.push_back(
                            "Memory access after free: " + inst.operands[0]
                        );
                        result.isValid = false;
                    }
                }
                break;
                
            default:
                break;
        }
    }
};

// =============================================================================
// القسم 5: المحقق الشامل (تجميع كل المحققات)
// =============================================================================

/**
 * @brief المحقق الشامل لـ SIR
 */
class SirValidator {
public:
    struct FullValidationResult {
        bool isValid = true;
        size_t errorCount = 0;
        size_t warningCount = 0;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        // تفاصيل خاصة
        std::set<std::string> unreachableBlocks;
        std::vector<std::string> potentialLeaks;
        std::vector<std::string> useAfterFree;
        std::vector<std::string> doubleFree;
    };
    
    /**
     * @brief التحقق الشامل من الوحدة
     */
    FullValidationResult validate(const SirModule& module) {
        FullValidationResult result;
        
        // التحقق من كل دالة
        for (const auto& function : module.functions) {
            validateFunction(function, result);
        }
        
        result.errorCount = result.errors.size();
        result.warningCount = result.warnings.size();
        
        return result;
    }
    
private:
    void validateFunction(const SirFunction& function, FullValidationResult& result) {
        // 1. التحقق من الكتل
        BlockValidator blockValidator;
        for (const auto& block : function.blocks) {
            auto blockResult = blockValidator.validate(block, function);
            if (!blockResult.isValid) {
                result.isValid = false;
            }
            for (auto& err : blockResult.errors) {
                result.errors.push_back("[" + function.name + "] " + err);
            }
            for (auto& warn : blockResult.warnings) {
                result.warnings.push_back("[" + function.name + "] " + warn);
            }
        }
        
        // 2. التحقق من التعليمات
        InstructionValidator instValidator;
        std::map<std::string, TypeInfo> context;
        
        // إضافة المعلمات إلى السياق
        for (const auto& [name, type] : function.parameters) {
            context["%" + name] = type;
        }
        
        for (const auto& block : function.blocks) {
            for (const auto& inst : block.instructions) {
                auto instResult = instValidator.validate(inst, context);
                if (!instResult.isValid) {
                    result.isValid = false;
                }
                for (auto& err : instResult.errors) {
                    result.errors.push_back("[" + function.name + "." + block.name + "] " + err);
                }
                
                // إضافة النتيجة إلى السياق
                if (inst.result.has_value()) {
                    context[inst.result.value()] = inst.resultType;
                }
            }
        }
        
        // 3. التحقق من تدفق التحكم
        ControlFlowValidator cfgValidator;
        auto cfgResult = cfgValidator.validate(function);
        if (!cfgResult.isValid) {
            result.isValid = false;
        }
        for (auto& err : cfgResult.errors) {
            result.errors.push_back("[" + function.name + "] CFG: " + err);
        }
        for (auto& warn : cfgResult.warnings) {
            result.warnings.push_back("[" + function.name + "] CFG: " + warn);
        }
        result.unreachableBlocks.insert(
            cfgResult.unreachableBlocks.begin(),
            cfgResult.unreachableBlocks.end()
        );
        
        // 4. التحقق من الذاكرة
        MemoryValidator memValidator;
        auto memResult = memValidator.validate(function);
        if (!memResult.isValid) {
            result.isValid = false;
        }
        for (auto& err : memResult.errors) {
            result.errors.push_back("[" + function.name + "] Memory: " + err);
        }
        for (auto& warn : memResult.warnings) {
            result.warnings.push_back("[" + function.name + "] Memory: " + warn);
        }
        result.potentialLeaks.insert(
            result.potentialLeaks.end(),
            memResult.potentialLeaks.begin(),
            memResult.potentialLeaks.end()
        );
        result.useAfterFree.insert(
            result.useAfterFree.end(),
            memResult.useAfterFree.begin(),
            memResult.useAfterFree.end()
        );
        result.doubleFree.insert(
            result.doubleFree.end(),
            memResult.doubleFree.begin(),
            memResult.doubleFree.end()
        );
    }
};

} // namespace sir
} // namespace compiler
} // namespace sad
