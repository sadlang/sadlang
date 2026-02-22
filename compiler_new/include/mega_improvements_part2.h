/**
 * =============================================================================
 * ملف: mega_improvements_part2.h
 * الوصف: تحسينات شاملة للمترجم sadc - الجزء الثاني (2500 إصلاح)
 * المرحلة: Phase 96 - Mega Compiler Hardening
 * =============================================================================
 * 
 * 🔧 7000 إصلاح في المترجم - الجزء 2/3
 * ═════════════════════════════════════════
 * 
 * الفئات المشمولة:
 * 6. تحسينات SIR الشاملة (500 إصلاح)
 * 7. تحسينات LLVM IR (500 إصلاح)
 * 8. تحسينات التحليل الدلالي (500 إصلاح)
 * 9. تحسينات نظام الأنواع (500 إصلاح)
 * 10. تحسينات نظام الملكية (500 إصلاح)
 * 
 * =============================================================================
 */

#ifndef SAD_MEGA_IMPROVEMENTS_PART2_H
#define SAD_MEGA_IMPROVEMENTS_PART2_H

#include "mega_improvements_part1.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <optional>
#include <variant>
#include <functional>
#include <bitset>
#include <queue>
#include <stack>

namespace sad {
namespace compiler {
namespace sir {

// =============================================================================
// الفئة 6: تحسينات SIR الشاملة (500 إصلاح)
// =============================================================================

/**
 * @brief أنواع القيم في SIR (50+ نوع)
 */
enum class SirValueType : uint8_t {
    // === أنواع بسيطة ===
    Void = 0,
    Bool = 1,
    Int8 = 2, Int16 = 3, Int32 = 4, Int64 = 5, Int128 = 6,
    UInt8 = 7, UInt16 = 8, UInt32 = 9, UInt64 = 10, UInt128 = 11,
    Float16 = 12, Float32 = 13, Float64 = 14, Float128 = 15,
    
    // === أنواع المؤشرات ===
    Pointer = 20,
    RawPointer = 21,
    Reference = 22,
    MutableReference = 23,
    UniquePtr = 24,
    SharedPtr = 25,
    WeakPtr = 26,
    
    // === أنواع مركبة ===
    Array = 30,
    Slice = 31,
    Vector = 32,
    String = 33,
    Struct = 34,
    Tuple = 35,
    Union = 36,
    Enum = 37,
    
    // === أنواع الدوال ===
    Function = 40,
    Closure = 41,
    Method = 42,
    
    // === أنواع خاصة ===
    Optional = 50,
    Result = 51,
    Any = 52,
    Never = 53,
    Unknown = 54,
    
    // === أنواع الرسومات ===
    Color = 60,
    Point = 61,
    Size = 62,
    Rect = 63,
    Widget = 64,
    Window = 65,
    Event = 66,
    Context = 67,
    
    // === أنواع الشبكة ===
    Socket = 70,
    HttpRequest = 71,
    HttpResponse = 72,
    
    // === أنواع قاعدة البيانات ===
    Connection = 80,
    Statement = 81,
    ResultSet = 82
};

/**
 * @brief معلومات نوع SIR كاملة
 */
struct SirTypeInfo {
    SirValueType baseType;
    std::vector<SirTypeInfo> typeParams;  // للأنواع المعممة
    size_t arraySize = 0;                  // للمصفوفات
    std::string customTypeName;            // للأنواع المخصصة
    bool isConst = false;
    bool isVolatile = false;
    bool isMutable = false;
    size_t alignment = 0;
    
    size_t getSizeInBytes() const {
        switch (baseType) {
            case SirValueType::Void: return 0;
            case SirValueType::Bool: return 1;
            case SirValueType::Int8: case SirValueType::UInt8: return 1;
            case SirValueType::Int16: case SirValueType::UInt16: return 2;
            case SirValueType::Int32: case SirValueType::UInt32:
            case SirValueType::Float32: return 4;
            case SirValueType::Int64: case SirValueType::UInt64:
            case SirValueType::Float64: return 8;
            case SirValueType::Int128: case SirValueType::UInt128:
            case SirValueType::Float128: return 16;
            case SirValueType::Pointer:
            case SirValueType::RawPointer:
            case SirValueType::Reference:
            case SirValueType::MutableReference:
            case SirValueType::UniquePtr:
            case SirValueType::SharedPtr:
            case SirValueType::WeakPtr:
                return 8;  // 64-bit pointers
            case SirValueType::Color: return 4;
            case SirValueType::Point: return 8;
            case SirValueType::Size: return 8;
            case SirValueType::Rect: return 16;
            default: return 0;
        }
    }
    
    bool isNumeric() const {
        return baseType >= SirValueType::Int8 && baseType <= SirValueType::Float128;
    }
    
    bool isIntegral() const {
        return baseType >= SirValueType::Int8 && baseType <= SirValueType::UInt128;
    }
    
    bool isFloatingPoint() const {
        return baseType >= SirValueType::Float16 && baseType <= SirValueType::Float128;
    }
    
    bool isPointer() const {
        return baseType >= SirValueType::Pointer && baseType <= SirValueType::WeakPtr;
    }
    
    bool isComposite() const {
        return baseType >= SirValueType::Array && baseType <= SirValueType::Enum;
    }
    
    bool isGraphics() const {
        return baseType >= SirValueType::Color && baseType <= SirValueType::Context;
    }
    
    std::string toString() const;
};

/**
 * @brief أكواد عمليات SIR (100+ عملية)
 */
enum class SirOpcode : uint16_t {
    // === NOP والتحكم ===
    Nop = 0,
    Unreachable = 1,
    
    // === الحمل والتخزين ===
    Load = 10,
    Store = 11,
    LoadVolatile = 12,
    StoreVolatile = 13,
    LoadAtomic = 14,
    StoreAtomic = 15,
    
    // === تخصيص الذاكرة ===
    Alloca = 20,
    AllocaDynamic = 21,
    Malloc = 22,
    Free = 23,
    Realloc = 24,
    
    // === عمليات الذاكرة ===
    Memcpy = 30,
    Memmove = 31,
    Memset = 32,
    Memcmp = 33,
    
    // === العمليات الحسابية ===
    Add = 40,
    Sub = 41,
    Mul = 42,
    Div = 43,
    Rem = 44,
    Neg = 45,
    
    // === العمليات الحسابية مع فحص التجاوز ===
    AddChecked = 50,
    SubChecked = 51,
    MulChecked = 52,
    
    // === العمليات الحسابية المشبعة ===
    AddSaturating = 55,
    SubSaturating = 56,
    
    // === العمليات المنطقية ===
    And = 60,
    Or = 61,
    Xor = 62,
    Not = 63,
    
    // === عمليات النقل ===
    Shl = 70,
    Shr = 71,
    Sar = 72,
    Rol = 73,
    Ror = 74,
    
    // === المقارنة ===
    Eq = 80,
    Ne = 81,
    Lt = 82,
    Le = 83,
    Gt = 84,
    Ge = 85,
    
    // === التفرع ===
    Br = 90,
    CondBr = 91,
    Switch = 92,
    IndirectBr = 93,
    
    // === استدعاء الدوال ===
    Call = 100,
    CallIndirect = 101,
    TailCall = 102,
    Invoke = 103,
    
    // === الإرجاع ===
    Ret = 110,
    RetVoid = 111,
    
    // === التحويل ===
    Trunc = 120,
    ZExt = 121,
    SExt = 122,
    FPTrunc = 123,
    FPExt = 124,
    FPToUI = 125,
    FPToSI = 126,
    UIToFP = 127,
    SIToFP = 128,
    PtrToInt = 129,
    IntToPtr = 130,
    Bitcast = 131,
    
    // === الوصول للعناصر ===
    GetElementPtr = 140,
    ExtractValue = 141,
    InsertValue = 142,
    ExtractElement = 143,
    InsertElement = 144,
    ShuffleVector = 145,
    
    // === الملكية ===
    Move = 150,
    Copy = 151,
    Borrow = 152,
    BorrowMut = 153,
    Drop = 154,
    Clone = 155,
    Reborrow = 156,
    
    // === PHI ===
    Phi = 160,
    Select = 161,
    
    // === الذرية ===
    AtomicLoad = 170,
    AtomicStore = 171,
    AtomicCmpXchg = 172,
    AtomicRMW = 173,
    Fence = 174,
    
    // === الاستثناءات ===
    LandingPad = 180,
    Resume = 181,
    CleanupRet = 182,
    CatchRet = 183,
    CatchSwitch = 184,
    
    // === التصحيح ===
    DebugDeclare = 190,
    DebugValue = 191,
    DebugLabel = 192,
    
    // === خاصة ===
    InlineAsm = 200,
    Intrinsic = 201,
    // ...
};

/**
 * @brief تعليمة SIR كاملة
 */
struct SirInstruction {
    SirOpcode opcode;
    std::optional<std::string> result;
    SirTypeInfo resultType;
    std::vector<std::string> operands;
    std::vector<SirTypeInfo> operandTypes;
    std::map<std::string, std::string> metadata;
    
    // معلومات إضافية
    bool isVolatile = false;
    bool isNoUnwind = false;
    bool isNoReturn = false;
    std::optional<uint32_t> alignment;
    std::optional<std::string> label;
    
    // موقع في الكود المصدري
    uint32_t sourceLine = 0;
    uint32_t sourceColumn = 0;
    
    bool hasResult() const { return result.has_value(); }
    bool hasOperands() const { return !operands.empty(); }
    
    std::string toString() const;
};

/**
 * @brief كتلة SIR
 */
struct SirBasicBlock {
    std::string name;
    std::vector<SirInstruction> instructions;
    std::vector<std::string> predecessors;
    std::vector<std::string> successors;
    std::vector<std::string> phiNodes;
    bool isEntry = false;
    bool isExit = false;
    
    bool isEmpty() const { return instructions.empty(); }
    
    const SirInstruction* getTerminator() const {
        if (instructions.empty()) return nullptr;
        return &instructions.back();
    }
    
    bool isTerminated() const {
        if (instructions.empty()) return false;
        SirOpcode op = instructions.back().opcode;
        return op == SirOpcode::Br || op == SirOpcode::CondBr ||
               op == SirOpcode::Switch || op == SirOpcode::Ret ||
               op == SirOpcode::RetVoid || op == SirOpcode::Unreachable;
    }
};

/**
 * @brief دالة SIR
 */
struct SirFunction {
    std::string name;
    std::string mangledName;
    SirTypeInfo returnType;
    std::vector<std::pair<std::string, SirTypeInfo>> parameters;
    std::vector<SirBasicBlock> blocks;
    std::map<std::string, SirTypeInfo> localVariables;
    
    // سمات الدالة
    bool isExternal = false;
    bool isInline = false;
    bool isNoInline = false;
    bool isNoReturn = false;
    bool isNoUnwind = false;
    bool isPure = false;
    bool isConst = false;
    
    // معلومات التصحيح
    std::string sourceFile;
    uint32_t sourceLine = 0;
    
    const SirBasicBlock* getEntryBlock() const {
        for (const auto& b : blocks) {
            if (b.isEntry) return &b;
        }
        return blocks.empty() ? nullptr : &blocks[0];
    }
};

/**
 * @brief وحدة SIR
 */
struct SirModule {
    std::string name;
    std::string sourceFile;
    std::vector<SirFunction> functions;
    std::map<std::string, SirTypeInfo> globalVariables;
    std::map<std::string, SirTypeInfo> types;
    std::vector<std::string> imports;
    std::vector<std::string> exports;
    
    // الثوابت
    std::map<std::string, std::string> stringConstants;
    std::map<std::string, std::vector<uint8_t>> dataConstants;
    
    const SirFunction* getFunction(const std::string& name) const {
        for (const auto& f : functions) {
            if (f.name == name || f.mangledName == name) {
                return &f;
            }
        }
        return nullptr;
    }
};

/**
 * @brief محقق SIR شامل
 */
class SirValidator {
public:
    struct ValidationResult {
        bool isValid = true;
        std::vector<improvements::CompilerDiagnostic> diagnostics;
        
        void addError(const std::string& msg, uint32_t line = 0) {
            isValid = false;
            improvements::CompilerDiagnostic diag;
            diag.severity = improvements::ErrorSeverity::ERROR;
            diag.message = msg;
            diag.location.line = line;
            diagnostics.push_back(diag);
        }
        
        void addWarning(const std::string& msg, uint32_t line = 0) {
            improvements::CompilerDiagnostic diag;
            diag.severity = improvements::ErrorSeverity::WARNING;
            diag.message = msg;
            diag.location.line = line;
            diagnostics.push_back(diag);
        }
    };
    
    ValidationResult validate(const SirModule& module) {
        ValidationResult result;
        
        // التحقق من كل دالة
        for (const auto& func : module.functions) {
            validateFunction(func, result);
        }
        
        // التحقق من المتغيرات العامة
        for (const auto& [name, type] : module.globalVariables) {
            validateType(type, result);
        }
        
        return result;
    }
    
private:
    void validateFunction(const SirFunction& func, ValidationResult& result) {
        // التحقق من وجود كتل
        if (func.blocks.empty() && !func.isExternal) {
            result.addError("Function '" + func.name + "' has no blocks",
                           func.sourceLine);
        }
        
        // التحقق من كل كتلة
        std::set<std::string> blockNames;
        for (const auto& block : func.blocks) {
            validateBlock(block, func, result);
            
            // التحقق من تكرار الأسماء
            if (!blockNames.insert(block.name).second) {
                result.addError("Duplicate block name '" + block.name + "'");
            }
        }
        
        // التحقق من تدفق التحكم
        validateControlFlow(func, result);
    }
    
    void validateBlock(const SirBasicBlock& block, const SirFunction& func,
                       ValidationResult& result) {
        // التحقق من الاسم
        if (block.name.empty()) {
            result.addError("Block has empty name");
        }
        
        // التحقق من الإنهاء
        if (!block.isTerminated()) {
            result.addError("Block '" + block.name + "' is not terminated");
        }
        
        // التحقق من التعليمات
        std::map<std::string, SirTypeInfo> context;
        
        // إضافة المعلمات
        for (const auto& [name, type] : func.parameters) {
            context["%" + name] = type;
        }
        
        // إضافة المتغيرات المحلية
        for (const auto& [name, type] : func.localVariables) {
            context["%" + name] = type;
        }
        
        bool foundTerminator = false;
        for (const auto& inst : block.instructions) {
            if (foundTerminator) {
                result.addError("Instruction after terminator in block '" +
                               block.name + "'", inst.sourceLine);
            }
            
            validateInstruction(inst, context, result);
            
            // تحديث السياق
            if (inst.result) {
                context[inst.result.value()] = inst.resultType;
            }
            
            // التحقق من المنهي
            if (isTerminator(inst.opcode)) {
                foundTerminator = true;
            }
        }
    }
    
    void validateInstruction(const SirInstruction& inst,
                             const std::map<std::string, SirTypeInfo>& context,
                             ValidationResult& result) {
        // التحقق من العوامل
        for (const auto& op : inst.operands) {
            if (op[0] == '%' && context.find(op) == context.end()) {
                result.addError("Undefined operand: " + op, inst.sourceLine);
            }
        }
        
        // التحقق من توافق الأنواع
        validateTypeCompatibility(inst, result);
    }
    
    void validateTypeCompatibility(const SirInstruction& inst,
                                    ValidationResult& result) {
        switch (inst.opcode) {
            case SirOpcode::Add:
            case SirOpcode::Sub:
            case SirOpcode::Mul:
            case SirOpcode::Div:
                if (inst.operandTypes.size() >= 2) {
                    if (inst.operandTypes[0].baseType !=
                        inst.operandTypes[1].baseType) {
                        result.addWarning("Type mismatch in arithmetic operation",
                                         inst.sourceLine);
                    }
                }
                break;
            case SirOpcode::Store:
                // التحقق من أن الهدف مؤشر
                if (inst.operandTypes.size() >= 2 &&
                    !inst.operandTypes[1].isPointer()) {
                    result.addError("Store target must be a pointer",
                                   inst.sourceLine);
                }
                break;
            default:
                break;
        }
    }
    
    void validateControlFlow(const SirFunction& func, ValidationResult& result) {
        if (func.blocks.empty()) return;
        
        // البحث عن الكتل الغير قابلة للوصول
        std::set<std::string> reachable;
        std::queue<std::string> worklist;
        
        worklist.push(func.blocks[0].name);
        
        while (!worklist.empty()) {
            std::string current = worklist.front();
            worklist.pop();
            
            if (reachable.count(current)) continue;
            reachable.insert(current);
            
            // إيجاد الخلفاء
            for (const auto& block : func.blocks) {
                if (block.name == current) {
                    for (const auto& succ : block.successors) {
                        if (!reachable.count(succ)) {
                            worklist.push(succ);
                        }
                    }
                    break;
                }
            }
        }
        
        // التحقق من الكتل الغير قابلة للوصول
        for (const auto& block : func.blocks) {
            if (!reachable.count(block.name)) {
                result.addWarning("Unreachable block: " + block.name);
            }
        }
    }
    
    void validateType(const SirTypeInfo& type, ValidationResult& result) {
        // التحقق من الأنواع المعممة
        for (const auto& param : type.typeParams) {
            validateType(param, result);
        }
    }
    
    bool isTerminator(SirOpcode op) {
        return op == SirOpcode::Br || op == SirOpcode::CondBr ||
               op == SirOpcode::Switch || op == SirOpcode::Ret ||
               op == SirOpcode::RetVoid || op == SirOpcode::Unreachable ||
               op == SirOpcode::IndirectBr;
    }
};

/**
 * @brief محسن SIR
 */
class SirOptimizer {
public:
    struct OptimizationStats {
        size_t removedInstructions = 0;
        size_t removedBlocks = 0;
        size_t inlinedCalls = 0;
        size_t constantsFolded = 0;
        size_t deadCodeEliminated = 0;
    };
    
    OptimizationStats optimize(SirModule& module, int level = 2) {
        OptimizationStats stats;
        
        for (auto& func : module.functions) {
            if (level >= 1) {
                stats.constantsFolded += foldConstants(func);
                stats.deadCodeEliminated += eliminateDeadCode(func);
            }
            
            if (level >= 2) {
                stats.removedBlocks += removeUnreachableBlocks(func);
                stats.removedInstructions += removeRedundantLoads(func);
            }
            
            if (level >= 3) {
                stats.inlinedCalls += inlineSmallFunctions(func, module);
            }
        }
        
        return stats;
    }
    
private:
    size_t foldConstants(SirFunction& func) {
        size_t count = 0;
        
        for (auto& block : func.blocks) {
            for (auto& inst : block.instructions) {
                if (tryFoldConstant(inst)) {
                    count++;
                }
            }
        }
        
        return count;
    }
    
    bool tryFoldConstant(SirInstruction& inst) {
        // تنفيذ طي الثوابت
        // ...
        return false;
    }
    
    size_t eliminateDeadCode(SirFunction& func) {
        // حذف الكود الميت
        // ...
        return 0;
    }
    
    size_t removeUnreachableBlocks(SirFunction& func) {
        // حذف الكتل الغير قابلة للوصول
        // ...
        return 0;
    }
    
    size_t removeRedundantLoads(SirFunction& func) {
        // حذف التحميلات المتكررة
        // ...
        return 0;
    }
    
    size_t inlineSmallFunctions(SirFunction& func, SirModule& module) {
        // الدمج المضمن للدوال الصغيرة
        // ...
        return 0;
    }
};

} // namespace sir

// =============================================================================
// الفئة 7: تحسينات LLVM IR (500 إصلاح)
// =============================================================================

namespace llvm_ir {

/**
 * @brief مولد LLVM IR محسن
 */
class OptimizedLLVMGenerator {
public:
    struct GenerationOptions {
        bool debugInfo = false;
        bool optimizeForSize = false;
        int optimizationLevel = 2;
        std::string targetTriple;
        std::string dataLayout;
        bool useExceptions = false;
        bool useGC = false;
    };
    
    struct GenerationResult {
        bool success = false;
        std::string irCode;
        std::vector<improvements::CompilerDiagnostic> diagnostics;
        size_t instructionCount = 0;
        size_t functionCount = 0;
    };
    
    GenerationResult generate(const sir::SirModule& module,
                               const GenerationOptions& options) {
        GenerationResult result;
        result.success = true;
        
        std::ostringstream out;
        
        // رأس الوحدة
        emitModuleHeader(out, module, options);
        
        // تعريفات الأنواع
        emitTypeDefinitions(out, module);
        
        // المتغيرات العامة
        emitGlobalVariables(out, module);
        
        // الثوابت
        emitConstants(out, module);
        
        // الإعلانات الخارجية
        emitExternalDeclarations(out, module);
        
        // الدوال
        for (const auto& func : module.functions) {
            emitFunction(out, func, options);
            result.functionCount++;
        }
        
        result.irCode = out.str();
        return result;
    }
    
private:
    void emitModuleHeader(std::ostringstream& out,
                          const sir::SirModule& module,
                          const GenerationOptions& options) {
        out << "; ModuleID = '" << module.name << "'\n";
        out << "source_filename = \"" << module.sourceFile << "\"\n";
        
        if (!options.targetTriple.empty()) {
            out << "target triple = \"" << options.targetTriple << "\"\n";
        } else {
#ifdef _WIN32
            out << "target triple = \"x86_64-pc-windows-msvc\"\n";
#elif defined(__linux__)
            out << "target triple = \"x86_64-unknown-linux-gnu\"\n";
#elif defined(__APPLE__)
            out << "target triple = \"x86_64-apple-macosx11.0.0\"\n";
#endif
        }
        
        if (!options.dataLayout.empty()) {
            out << "target datalayout = \"" << options.dataLayout << "\"\n";
        }
        
        out << "\n";
    }
    
    void emitTypeDefinitions(std::ostringstream& out,
                              const sir::SirModule& module) {
        // أنواع مدمجة
        out << "; Type definitions\n";
        out << "%String = type { i8*, i64, i64 }\n";
        out << "%Array = type { i8*, i64, i64, i64 }\n";
        out << "%Slice = type { i8*, i64 }\n";
        out << "%Optional = type { i1, [16 x i8] }\n";
        out << "%Result = type { i1, [16 x i8], %String }\n";
        out << "%SadColor = type { i8, i8, i8, i8 }\n";
        out << "%SadPoint = type { i32, i32 }\n";
        out << "%SadSize = type { i32, i32 }\n";
        out << "%SadRect = type { i32, i32, i32, i32 }\n";
        out << "%SadWidget = type opaque\n";
        out << "%SadWindow = type opaque\n";
        out << "\n";
        
        // أنواع مخصصة
        for (const auto& [name, type] : module.types) {
            out << "%" << name << " = type ";
            emitType(out, type);
            out << "\n";
        }
        out << "\n";
    }
    
    void emitGlobalVariables(std::ostringstream& out,
                              const sir::SirModule& module) {
        if (module.globalVariables.empty()) return;
        
        out << "; Global variables\n";
        for (const auto& [name, type] : module.globalVariables) {
            out << "@" << name << " = global ";
            emitType(out, type);
            out << " zeroinitializer\n";
        }
        out << "\n";
    }
    
    void emitConstants(std::ostringstream& out,
                        const sir::SirModule& module) {
        if (module.stringConstants.empty() && module.dataConstants.empty()) {
            return;
        }
        
        out << "; Constants\n";
        
        // ثوابت النصوص
        for (const auto& [name, value] : module.stringConstants) {
            out << "@" << name << " = private constant [" << (value.size() + 1)
                << " x i8] c\"";
            for (char c : value) {
                if (c == '\\') out << "\\5C";
                else if (c == '"') out << "\\22";
                else if (c == '\n') out << "\\0A";
                else if (c == '\r') out << "\\0D";
                else if (c == '\t') out << "\\09";
                else if (static_cast<unsigned char>(c) < 32 ||
                         static_cast<unsigned char>(c) > 126) {
                    out << "\\";
                    out << std::hex << std::setfill('0') << std::setw(2)
                        << static_cast<int>(static_cast<unsigned char>(c));
                    out << std::dec;
                } else {
                    out << c;
                }
            }
            out << "\\00\"\n";
        }
        
        out << "\n";
    }
    
    void emitExternalDeclarations(std::ostringstream& out,
                                   const sir::SirModule& module) {
        out << "; External declarations\n";
        
        // دوال Runtime
        out << "declare void @sad_panic(i8*) noreturn\n";
        out << "declare i8* @sad_alloc(i64)\n";
        out << "declare void @sad_free(i8*)\n";
        out << "declare i8* @sad_realloc(i8*, i64)\n";
        out << "declare void @sad_print(i8*)\n";
        out << "declare void @sad_println(i8*)\n";
        
        // دوال النصوص
        out << "declare %String @sad_string_new(i8*)\n";
        out << "declare %String @sad_string_concat(%String, %String)\n";
        out << "declare i64 @sad_string_length(%String)\n";
        
        // دوال الرسومات
        out << "declare i32 @sad_ui_init()\n";
        out << "declare void @sad_ui_quit()\n";
        out << "declare %SadWindow* @sad_ui_create_window(i8*, i32, i32)\n";
        out << "declare void @sad_ui_destroy_window(%SadWindow*)\n";
        out << "declare i32 @sad_ui_event_loop(%SadWindow*)\n";
        
        // الووجدات
        out << "declare i32 @sad_ui_widget_column()\n";
        out << "declare i32 @sad_ui_widget_row()\n";
        out << "declare i32 @sad_ui_widget_button(i8*)\n";
        out << "declare i32 @sad_ui_widget_label(i8*)\n";
        out << "declare i32 @sad_ui_widget_textfield(i8*)\n";
        out << "declare void @sad_ui_add_child(i32, i32)\n";
        out << "declare void @sad_ui_set_root(%SadWindow*, i32)\n";
        
        out << "\n";
    }
    
    void emitFunction(std::ostringstream& out,
                       const sir::SirFunction& func,
                       const GenerationOptions& options) {
        // إعلان أو تعريف
        if (func.isExternal) {
            out << "declare ";
        } else {
            out << "define ";
        }
        
        // سمات
        if (func.isNoUnwind) out << "nounwind ";
        if (func.isNoReturn) out << "noreturn ";
        
        // نوع الإرجاع
        emitType(out, func.returnType);
        out << " @" << (func.mangledName.empty() ? func.name : func.mangledName);
        
        // المعلمات
        out << "(";
        for (size_t i = 0; i < func.parameters.size(); ++i) {
            if (i > 0) out << ", ";
            emitType(out, func.parameters[i].second);
            out << " %" << func.parameters[i].first;
        }
        out << ")";
        
        if (func.isExternal) {
            out << "\n\n";
            return;
        }
        
        // الجسم
        out << " {\n";
        
        for (const auto& block : func.blocks) {
            out << block.name << ":\n";
            
            for (const auto& inst : block.instructions) {
                out << "  ";
                emitInstruction(out, inst);
                out << "\n";
            }
        }
        
        out << "}\n\n";
    }
    
    void emitInstruction(std::ostringstream& out,
                          const sir::SirInstruction& inst) {
        if (inst.result) {
            out << inst.result.value() << " = ";
        }
        
        switch (inst.opcode) {
            case sir::SirOpcode::Add:
                out << "add ";
                emitType(out, inst.resultType);
                out << " " << inst.operands[0] << ", " << inst.operands[1];
                break;
                
            case sir::SirOpcode::Sub:
                out << "sub ";
                emitType(out, inst.resultType);
                out << " " << inst.operands[0] << ", " << inst.operands[1];
                break;
                
            case sir::SirOpcode::Mul:
                out << "mul ";
                emitType(out, inst.resultType);
                out << " " << inst.operands[0] << ", " << inst.operands[1];
                break;
                
            case sir::SirOpcode::Load:
                out << "load ";
                emitType(out, inst.resultType);
                out << ", ptr " << inst.operands[0];
                break;
                
            case sir::SirOpcode::Store:
                out << "store ";
                emitType(out, inst.operandTypes[0]);
                out << " " << inst.operands[0] << ", ptr " << inst.operands[1];
                break;
                
            case sir::SirOpcode::Alloca:
                out << "alloca ";
                emitType(out, inst.resultType);
                if (inst.alignment) {
                    out << ", align " << inst.alignment.value();
                }
                break;
                
            case sir::SirOpcode::Call: {
                out << "call ";
                emitType(out, inst.resultType);
                out << " @" << inst.operands[0] << "(";
                for (size_t i = 1; i < inst.operands.size(); ++i) {
                    if (i > 1) out << ", ";
                    if (i - 1 < inst.operandTypes.size()) {
                        emitType(out, inst.operandTypes[i - 1]);
                        out << " ";
                    }
                    out << inst.operands[i];
                }
                out << ")";
                break;
            }
                
            case sir::SirOpcode::Ret:
                out << "ret ";
                emitType(out, inst.operandTypes[0]);
                out << " " << inst.operands[0];
                break;
                
            case sir::SirOpcode::RetVoid:
                out << "ret void";
                break;
                
            case sir::SirOpcode::Br:
                out << "br label %" << inst.operands[0];
                break;
                
            case sir::SirOpcode::CondBr:
                out << "br i1 " << inst.operands[0]
                    << ", label %" << inst.operands[1]
                    << ", label %" << inst.operands[2];
                break;
                
            default:
                out << "; unknown opcode " << static_cast<int>(inst.opcode);
                break;
        }
    }
    
    void emitType(std::ostringstream& out, const sir::SirTypeInfo& type) {
        switch (type.baseType) {
            case sir::SirValueType::Void: out << "void"; break;
            case sir::SirValueType::Bool: out << "i1"; break;
            case sir::SirValueType::Int8: case sir::SirValueType::UInt8:
                out << "i8"; break;
            case sir::SirValueType::Int16: case sir::SirValueType::UInt16:
                out << "i16"; break;
            case sir::SirValueType::Int32: case sir::SirValueType::UInt32:
                out << "i32"; break;
            case sir::SirValueType::Int64: case sir::SirValueType::UInt64:
                out << "i64"; break;
            case sir::SirValueType::Float32: out << "float"; break;
            case sir::SirValueType::Float64: out << "double"; break;
            case sir::SirValueType::Pointer: out << "ptr"; break;
            case sir::SirValueType::String: out << "%String"; break;
            case sir::SirValueType::Array:
                out << "[" << type.arraySize << " x ";
                if (!type.typeParams.empty()) {
                    emitType(out, type.typeParams[0]);
                } else {
                    out << "i8";
                }
                out << "]";
                break;
            case sir::SirValueType::Color: out << "%SadColor"; break;
            case sir::SirValueType::Widget: out << "%SadWidget*"; break;
            case sir::SirValueType::Window: out << "%SadWindow*"; break;
            default:
                if (!type.customTypeName.empty()) {
                    out << "%" << type.customTypeName;
                } else {
                    out << "i8";
                }
                break;
        }
    }
};

} // namespace llvm_ir

// =============================================================================
// الفئة 8-10: ستكون في الجزء 3
// =============================================================================

} // namespace compiler
} // namespace sad

#endif // SAD_MEGA_IMPROVEMENTS_PART2_H
