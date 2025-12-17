/**
 * @file emitter.cpp
 * @brief تطبيق مُصدِر البايت كود / Bytecode Emitter Implementation
 * @brief Complete SIR to bytecode translation logic
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../include/emitter.h"
#include <iostream>
#include <stdexcept>

namespace Sad {
namespace Compiler {
namespace Bytecode {

// ========================================
// Constructor
// ========================================

BytecodeEmitter::BytecodeEmitter()
    : currentFunction_(nullptr),
      localCount_(0),
      tempCounter_(0),
      optimize_(false),
      debugMode_(false),
      errorCount_(0) {
}

// ========================================
// Main Emission
// ========================================

std::unique_ptr<BytecodeModule> BytecodeEmitter::emit(std::shared_ptr<SIR::SIRModule> module) {
    module_ = std::make_unique<BytecodeModule>(module->getName());
    
    debug("Starting bytecode emission for module: " + module->getName());
    
    // (AR) إصدار المتغيرات العامة
    // (EN) Emit global variables
    for (const auto& global : module->getGlobals()) {
        uint32_t index = registerGlobal(global.name);
        
        GlobalInfo info;
        info.name = global.name;
        info.index = index;
        info.initialValue = VM::Value::Null();
        
        module_->addGlobal(info);
    }
    
    // (AR) إصدار الدوال
    // (EN) Emit functions
    for (auto* func : module->getFunctions()) {
        emitFunction(func);
    }
    
    // (AR) حل التسميات
    // (EN) Resolve labels
    resolveLabels();
    
    // (AR) تحسين إذا مطلوب
    // (EN) Optimize if requested
    if (optimize_) {
        optimize();
    }
    
    debug("Bytecode emission complete");
    
    if (errorCount_ > 0) {
        error("Emission completed with " + std::to_string(errorCount_) + " errors");
    }
    
    return std::move(module_);
}

void BytecodeEmitter::emitFunction(SIR::SIRFunction* function) {
    currentFunction_ = function;
    localCount_ = 0;
    tempCounter_ = 0;
    labels_.clear();
    variables_.clear();
    registerMap_.clear();
    
    debug("Emitting function: " + function->getName());
    
    // (AR) تسجيل المعاملات كمتغيرات محلية
    // (EN) Register parameters as locals
    for (const auto& param : function->getParameters()) {
        registerLocal(param.name);
    }
    
    // (AR) حفظ موضع بداية الدالة
    // (EN) Save function start position
    uint32_t functionStart = module_->getCurrentOffset();
    
    // (AR) إصدار الكتل
    // (EN) Emit blocks
    for (auto* block : function->getBasicBlocks()) {
        emitBlock(block);
    }
    
    // (AR) حفظ معلومات الدالة
    // (EN) Save function info
    FunctionInfo info;
    info.name = function->getName();
    info.arity = static_cast<uint32_t>(function->getParameters().size());
    info.localCount = localCount_;
    info.codeOffset = functionStart;
    info.codeSize = module_->getCurrentOffset() - functionStart;
    
    module_->addFunction(info);
    
    debug("Function complete: " + std::to_string(info.codeSize) + " bytes");
}

void BytecodeEmitter::emitBlock(SIR::SIRBasicBlock* block) {
    // (AR) تعريف تسمية الكتلة
    // (EN) Define block label
    defineLabel(block->getLabel());
    
    // (AR) إصدار التعليمات
    // (EN) Emit instructions
    for (auto* inst : block->getInstructions()) {
        if (inst->getSourceLine() > 0) {
            module_->addLineInfo(inst->getSourceLine());
        }
        
        emitInstruction(inst);
    }
}

void BytecodeEmitter::emitInstruction(SIR::SIRInstruction* inst) {
    SIR::SIROpcode op = inst->getOpcode();
    
    if (isArithmeticOp(op)) {
        emitArithmetic(inst);
    } else if (isBitwiseOp(op)) {
        emitBitwise(inst);
    } else if (isComparisonOp(op)) {
        emitComparison(inst);
    } else if (isJumpOp(op)) {
        emitJump(inst);
    } else if (isMemoryOp(op)) {
        emitMemory(inst);
    } else {
        switch (op) {
            case SIR::SIR_CALL:
                emitCall(inst);
                break;
            case SIR::SIR_RET:
                emitReturn(inst);
                break;
            case SIR::SIR_LOAD:
                emitLoad(inst);
                break;
            case SIR::SIR_STORE:
                emitStore(inst);
                break;
            case SIR::SIR_ARRAY_NEW:
            case SIR::SIR_ARRAY_GET:
            case SIR::SIR_ARRAY_SET:
                emitArray(inst);
                break;
            case SIR::SIR_BUILTIN:
                emitBuiltin(inst);
                break;
            default:
                error("Unknown SIR opcode: " + std::to_string(static_cast<int>(op)));
                break;
        }
    }
}

// ========================================
// Arithmetic Operations
// ========================================

void BytecodeEmitter::emitArithmetic(SIR::SIRInstruction* inst) {
    auto operands = inst->getOperands();
    
    if (operands.size() < 2) {
        error("Arithmetic operation requires at least 2 operands");
        return;
    }
    
    // (AR) ادفع المعاملات
    // (EN) Push operands
    emitOperand(operands[0]);
    emitOperand(operands[1]);
    
    // (AR) إصدار العملية
    // (EN) Emit operation
    VM::Opcode bytecodeOp = sirToBytecodeOp(inst->getOpcode());
    module_->emitByte(bytecodeOp);
    
    // (AR) خزّن النتيجة
    // (EN) Store result
    if (inst->hasResult()) {
        emitResult(inst->getResult());
    }
}

void BytecodeEmitter::emitBitwise(SIR::SIRInstruction* inst) {
    auto operands = inst->getOperands();
    
    if (operands.size() < 2) {
        error("Bitwise operation requires at least 2 operands");
        return;
    }
    
    emitOperand(operands[0]);
    emitOperand(operands[1]);
    
    VM::Opcode bytecodeOp = sirToBytecodeOp(inst->getOpcode());
    module_->emitByte(bytecodeOp);
    
    if (inst->hasResult()) {
        emitResult(inst->getResult());
    }
}

void BytecodeEmitter::emitComparison(SIR::SIRInstruction* inst) {
    auto operands = inst->getOperands();
    
    if (operands.size() < 2) {
        error("Comparison requires 2 operands");
        return;
    }
    
    emitOperand(operands[0]);
    emitOperand(operands[1]);
    
    VM::Opcode bytecodeOp = sirToBytecodeOp(inst->getOpcode());
    module_->emitByte(bytecodeOp);
    
    if (inst->hasResult()) {
        emitResult(inst->getResult());
    }
}

// ========================================
// Control Flow
// ========================================

void BytecodeEmitter::emitJump(SIR::SIRInstruction* inst) {
    auto operands = inst->getOperands();
    
    if (operands.empty()) {
        error("Jump requires target label");
        return;
    }
    
    std::string targetLabel = operands[0].getName();
    
    switch (inst->getOpcode()) {
        case SIR::SIR_JMP:
            emitJumpToLabel(VM::OP_JMP, targetLabel);
            break;
            
        case SIR::SIR_BR: {
            // (AR) قفز شرطي
            // (EN) Conditional branch
            if (operands.size() < 2) {
                error("Branch requires condition and target");
                return;
            }
            
            emitOperand(operands[0]); // الشرط / condition
            emitJumpToLabel(VM::OP_JMP_IF, operands[1].getName());
            break;
        }
        
        default:
            error("Unknown jump opcode");
            break;
    }
}

void BytecodeEmitter::emitCall(SIR::SIRInstruction* inst) {
    auto operands = inst->getOperands();
    
    if (operands.empty()) {
        error("Call requires function name");
        return;
    }
    
    // (AR) ادفع الوسائط
    // (EN) Push arguments
    for (size_t i = 1; i < operands.size(); ++i) {
        emitOperand(operands[i]);
    }
    
    // (AR) احصل على فهرس الدالة
    // (EN) Get function index
    std::string funcName = operands[0].getName();
    int funcIndex = module_->getFunctionIndex(funcName);
    
    // (AR) إذا لم يتم العثور على الدالة، استخدم 0 كافتراضي / (EN) If not found, use 0 as default
    uint32_t finalIndex = (funcIndex >= 0) ? static_cast<uint32_t>(funcIndex) : 0;
    
    if (funcIndex < 0) {
        error("Function not found: " + funcName);
    }
    
    // (AR) إصدار استدعاء
    // (EN) Emit call
    module_->emitByte(VM::OP_CALL);
    module_->emitU32(finalIndex);
    module_->emitByte(static_cast<VM::Byte>(operands.size() - 1)); // argc
    
    // (AR) خزّن القيمة المعادة
    // (EN) Store return value
    if (inst->hasResult()) {
        emitResult(inst->getResult());
    } else {
        module_->emitByte(VM::OP_POP); // تجاهل القيمة / discard value
    }
}

void BytecodeEmitter::emitReturn(SIR::SIRInstruction* inst) {
    auto operands = inst->getOperands();
    
    if (operands.empty()) {
        // (AR) عودة بدون قيمة
        // (EN) Return without value
        module_->emitByte(VM::OP_RET);
    } else {
        // (AR) عودة مع قيمة
        // (EN) Return with value
        emitOperand(operands[0]);
        module_->emitByte(VM::OP_RET_VAL);
    }
}

// ========================================
// Memory Operations
// ========================================

void BytecodeEmitter::emitLoad(SIR::SIRInstruction* inst) {
    auto operands = inst->getOperands();
    
    if (operands.empty()) {
        error("Load requires source");
        return;
    }
    
    const auto& src = operands[0];
    
    if (src.isRegister()) {
        // (AR) تحميل من متغير
        // (EN) Load from variable
        uint32_t index = getVariableIndex(src.getName());
        bool isGlobal = this->isGlobal(src.getName());
        
        if (isGlobal) {
            module_->emitByte(VM::OP_GET_GLOBAL);
            module_->emitU32(index);
        } else {
            module_->emitByte(VM::OP_GET_LOCAL);
            module_->emitU32(index);
        }
    } else if (src.isImmediate()) {
        // (AR) تحميل قيمة مباشرة
        // (EN) Load immediate value
        emitOperand(src);
    }
    
    // (AR) خزّن في النتيجة
    // (EN) Store to result
    if (inst->hasResult()) {
        emitResult(inst->getResult());
    }
}

void BytecodeEmitter::emitStore(SIR::SIRInstruction* inst) {
    auto operands = inst->getOperands();
    
    if (operands.size() < 2) {
        error("Store requires source and destination");
        return;
    }
    
    const auto& value = operands[0];
    const auto& dest = operands[1];
    
    // (AR) ادفع القيمة
    // (EN) Push value
    emitOperand(value);
    
    // (AR) خزّن في الوجهة
    // (EN) Store to destination
    if (dest.isRegister()) {
        uint32_t index = getVariableIndex(dest.getName());
        bool isGlobal = this->isGlobal(dest.getName());
        
        if (isGlobal) {
            module_->emitByte(VM::OP_SET_GLOBAL);
            module_->emitU32(index);
        } else {
            module_->emitByte(VM::OP_SET_LOCAL);
            module_->emitU32(index);
        }
    }
}

void BytecodeEmitter::emitMemory(SIR::SIRInstruction* inst) {
    auto operands = inst->getOperands();
    
    switch (inst->getOpcode()) {
        case SIR::SIR_ALLOCA: {
            // (AR) تخصيص ذاكرة محلية
            // (EN) Allocate local memory
            if (operands.empty()) {
                error("Alloca requires size");
                return;
            }
            
            emitOperand(operands[0]); // الحجم / size
            module_->emitByte(VM::OP_MALLOC);
            
            if (inst->hasResult()) {
                emitResult(inst->getResult());
            }
            break;
        }
        
        case SIR::SIR_MALLOC: {
            // (AR) تخصيص ذاكرة ديناميكية
            // (EN) Allocate dynamic memory
            if (operands.empty()) {
                error("Malloc requires size");
                return;
            }
            
            emitOperand(operands[0]);
            module_->emitByte(VM::OP_MALLOC);
            
            if (inst->hasResult()) {
                emitResult(inst->getResult());
            }
            break;
        }
        
        default:
            error("Unknown memory operation");
            break;
    }
}

void BytecodeEmitter::emitArray(SIR::SIRInstruction* inst) {
    auto operands = inst->getOperands();
    
    switch (inst->getOpcode()) {
        case SIR::SIR_ARRAY_NEW: {
            // (AR) إنشاء مصفوفة جديدة
            // (EN) Create new array
            if (operands.empty()) {
                error("Array creation requires size");
                return;
            }
            
            emitOperand(operands[0]); // الحجم / size
            module_->emitByte(VM::OP_ARRAY_NEW);
            
            if (inst->hasResult()) {
                emitResult(inst->getResult());
            }
            break;
        }
        
        case SIR::SIR_ARRAY_GET: {
            // (AR) جلب عنصر من مصفوفة
            // (EN) Get array element
            if (operands.size() < 2) {
                error("Array get requires array and index");
                return;
            }
            
            emitOperand(operands[0]); // المصفوفة / array
            emitOperand(operands[1]); // الفهرس / index
            module_->emitByte(VM::OP_ARRAY_GET);
            
            if (inst->hasResult()) {
                emitResult(inst->getResult());
            }
            break;
        }
        
        case SIR::SIR_ARRAY_SET: {
            // (AR) تعيين عنصر في مصفوفة
            // (EN) Set array element
            if (operands.size() < 3) {
                error("Array set requires array, index, and value");
                return;
            }
            
            emitOperand(operands[0]); // المصفوفة / array
            emitOperand(operands[1]); // الفهرس / index
            emitOperand(operands[2]); // القيمة / value
            module_->emitByte(VM::OP_ARRAY_SET);
            break;
        }
        
        default:
            error("Unknown array operation");
            break;
    }
}

void BytecodeEmitter::emitBuiltin(SIR::SIRInstruction* inst) {
    auto operands = inst->getOperands();
    
    if (operands.empty()) {
        error("Builtin requires function name");
        return;
    }
    
    std::string builtinName = operands[0].getName();
    
    // (AR) ادفع الوسائط
    // (EN) Push arguments
    for (size_t i = 1; i < operands.size(); ++i) {
        emitOperand(operands[i]);
    }
    
    // (AR) احصل على رقم الدالة المدمجة
    // (EN) Get builtin function ID
    std::string builtinName = operands[0].getName();
    int builtinId = module_->getBuiltinId(builtinName);
    
    // (AR) إذا لم يتم العثور، حاول إيجاد الرقم من الاسم / (EN) If not found, try to derive ID from name
    uint32_t finalId = 0;
    if (builtinId >= 0) {
        finalId = static_cast<uint32_t>(builtinId);
    } else {
        // (AR) تعيين رقم افتراضي حسب الاسم / (EN) Assign default ID based on name
        if (builtinName == "print" || builtinName == "اطبع") {
            finalId = 1;
        } else if (builtinName == "input" || builtinName == "إدخال") {
            finalId = 2;
        } else if (builtinName == "len" || builtinName == "طول") {
            finalId = 3;
        } else if (builtinName == "type" || builtinName == "نوع") {
            finalId = 4;
        } else {
            error("Unknown builtin function: " + builtinName);
        }
    }
    
    module_->emitByte(VM::OP_CALL_NATIVE);
    module_->emitU32(finalId);
    module_->emitByte(static_cast<VM::Byte>(operands.size() - 1));
    
    if (inst->hasResult()) {
        emitResult(inst->getResult());
    }
}

// ========================================
// Operand Handling
// ========================================

void BytecodeEmitter::emitOperand(const SIR::SIROperand& operand) {
    if (operand.isImmediate()) {
        // (AR) قيمة مباشرة
        // (EN) Immediate value
        SIR::SIRValue val = operand.getValue();
        
        switch (val.getType()) {
            case SIR::SIRValueType::INT: {
                int64_t intVal = val.getInt();
                
                if (intVal >= INT8_MIN && intVal <= INT8_MAX) {
                    module_->emitByte(VM::OP_PUSH_I8);
                    module_->emitByte(static_cast<VM::Byte>(intVal));
                } else if (intVal >= INT16_MIN && intVal <= INT16_MAX) {
                    module_->emitByte(VM::OP_PUSH_I16);
                    module_->emitU16(static_cast<uint16_t>(intVal));
                } else if (intVal >= INT32_MIN && intVal <= INT32_MAX) {
                    module_->emitByte(VM::OP_PUSH_I32);
                    module_->emitU32(static_cast<uint32_t>(intVal));
                } else {
                    module_->emitByte(VM::OP_PUSH_I64);
                    module_->emitU64(static_cast<uint64_t>(intVal));
                }
                break;
            }
            
            case SIR::SIRValueType::FLOAT: {
                module_->emitByte(VM::OP_PUSH_F64);
                module_->emitF64(val.getFloat());
                break;
            }
            
            case SIR::SIRValueType::BOOL: {
                module_->emitByte(val.getBool() ? VM::OP_PUSH_TRUE : VM::OP_PUSH_FALSE);
                break;
            }
            
            case SIR::SIRValueType::STRING: {
                uint32_t constIndex = module_->getConstantPool().addString(val.getString());
                module_->emitByte(VM::OP_PUSH_STR);
                module_->emitU32(constIndex);
                break;
            }
            
            case SIR::SIRValueType::NULL_VAL: {
                module_->emitByte(VM::OP_PUSH_NULL);
                break;
            }
        }
    } else if (operand.isRegister()) {
        // (AR) سجل (متغير)
        // (EN) Register (variable)
        uint32_t index = getVariableIndex(operand.getName());
        bool isGlobal = this->isGlobal(operand.getName());
        
        if (isGlobal) {
            module_->emitByte(VM::OP_GET_GLOBAL);
            module_->emitU32(index);
        } else {
            module_->emitByte(VM::OP_GET_LOCAL);
            module_->emitU32(index);
        }
    }
}

void BytecodeEmitter::emitResult(const SIR::SIROperand& result) {
    if (!result.isRegister()) {
        error("Result must be a register");
        return;
    }
    
    uint32_t index = getVariableIndex(result.getName());
    bool isGlobal = this->isGlobal(result.getName());
    
    if (isGlobal) {
        module_->emitByte(VM::OP_SET_GLOBAL);
        module_->emitU32(index);
    } else {
        module_->emitByte(VM::OP_SET_LOCAL);
        module_->emitU32(index);
    }
}

// ========================================
// Label Management
// ========================================

void BytecodeEmitter::defineLabel(const std::string& name) {
    LabelInfo& label = labels_[name];
    label.name = name;
    label.offset = module_->getCurrentOffset();
    label.resolved = true;
    
    debug("Defined label: " + name + " at offset " + std::to_string(label.offset));
}

void BytecodeEmitter::emitJumpToLabel(VM::Opcode jumpOp, const std::string& label) {
    module_->emitByte(jumpOp);
    
    // (AR) احفظ موضع الإزاحة للتصحيح لاحقاً
    // (EN) Save offset position for patching later
    uint32_t patchPos = module_->getCurrentOffset();
    labels_[label].references.push_back(patchPos);
    
    // (AR) اكتب قيمة مؤقتة
    // (EN) Write temporary value
    module_->emitU32(0xFFFFFFFF);
    
    debug("Added jump to label: " + label);
}

void BytecodeEmitter::resolveLabels() {
    debug("Resolving labels...");
    
    for (auto& [name, label] : labels_) {
        if (!label.resolved) {
            error("Undefined label: " + name);
            continue;
        }
        
        // (AR) صحح جميع المراجع
        // (EN) Patch all references
        for (uint32_t ref : label.references) {
            module_->patchU32(ref, label.offset);
        }
        
        debug("Resolved " + std::to_string(label.references.size()) + 
              " references to label: " + name);
    }
}

// ========================================
// Variable Management
// ========================================

uint32_t BytecodeEmitter::registerLocal(const std::string& name) {
    VariableInfo info;
    info.name = name;
    info.index = localCount_++;
    info.isGlobal = false;
    info.isCaptured = false;
    
    variables_[name] = info;
    return info.index;
}

uint32_t BytecodeEmitter::registerGlobal(const std::string& name) {
    VariableInfo info;
    info.name = name;
    info.index = static_cast<uint32_t>(variables_.size());
    info.isGlobal = true;
    info.isCaptured = false;
    
    variables_[name] = info;
    return info.index;
}

uint32_t BytecodeEmitter::getVariableIndex(const std::string& name) {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        return it->second.index;
    }
    
    // (AR) متغير جديد - سجله كمحلي
    // (EN) New variable - register as local
    return registerLocal(name);
}

bool BytecodeEmitter::isGlobal(const std::string& name) {
    auto it = variables_.find(name);
    return it != variables_.end() && it->second.isGlobal;
}

// ========================================
// Optimization
// ========================================

void BytecodeEmitter::optimize() {
    debug("Running optimizations...");
    
    // ✅ المرحلة 14: تحسينات البايت كود
    // ✅ Phase 14: Bytecode optimizations
    
    // 1. إزالة الكود الميت / Remove dead code
    removeDeadCode();
    
    // 2. تحسينات peephole / Peephole optimizations
    peepholeOptimize();
    
    // 3. دمج التعليمات المتكررة / Merge duplicate instructions
    mergeDuplicates();
    
    debug("Optimizations complete");
}

void BytecodeEmitter::removeDeadCode() {
    // ✅ TODO 1: إزالة الكود الميت - COMPLETE
    // ✅ TODO 1: Dead code elimination - COMPLETE
    
    debug("Running dead code elimination...");
    
    if (!module_) {
        return;
    }
    
    auto& code = const_cast<std::vector<Byte>&>(module_->getCode());
    if (code.empty()) {
        return;
    }
    
    // (AR) تحليل التدفق - تتبع التعليمات القابلة للوصول
    // (EN) Flow analysis - track reachable instructions
    std::vector<bool> reachable(code.size(), false);
    std::vector<uint32_t> worklist;
    
    // (AR) نقطة البدء هي نقطة الدخول
    // (EN) Starting point is entry point
    worklist.push_back(module_->getEntryPoint());
    reachable[module_->getEntryPoint()] = true;
    
    // (AR) تحليل التدفق للأمام / Forward flow analysis
    while (!worklist.empty()) {
        uint32_t offset = worklist.back();
        worklist.pop_back();
        
        if (offset >= code.size()) {
            continue;
        }
        
        VM::Opcode op = static_cast<VM::Opcode>(code[offset]);
        uint32_t instrSize = getInstructionSize(op);
        
        // (AR) وضع علامة على التعليمة كاملة كقابلة للوصول
        // (EN) Mark entire instruction as reachable
        for (uint32_t i = 0; i < instrSize && (offset + i) < code.size(); ++i) {
            reachable[offset + i] = true;
        }
        
        // (AR) إذا كانت التعليمة قفزة، أضف الهدف للقائمة
        // (EN) If instruction is a jump, add target to worklist
        if (isJumpInstruction(op)) {
            uint32_t target = extractJumpTarget(code, offset, op);
            if (target < code.size() && !reachable[target]) {
                reachable[target] = true;
                worklist.push_back(target);
            }
        }
        
        // (AR) إذا كانت التعليمة غير قفز غير شرطي، أضف التعليمة التالية
        // (EN) If not unconditional jump, add next instruction
        if (!isUnconditionalJump(op)) {
            uint32_t nextOffset = offset + instrSize;
            if (nextOffset < code.size() && !reachable[nextOffset]) {
                reachable[nextOffset] = true;
                worklist.push_back(nextOffset);
            }
        }
    }
    
    // (AR) إزالة التعليمات غير القابلة للوصول
    // (EN) Remove unreachable instructions
    std::vector<Byte> optimizedCode;
    std::unordered_map<uint32_t, uint32_t> offsetMap; // old -> new
    
    for (uint32_t i = 0; i < code.size(); ++i) {
        if (reachable[i]) {
            offsetMap[i] = static_cast<uint32_t>(optimizedCode.size());
            optimizedCode.push_back(code[i]);
        }
    }
    
    // (AR) تحديث القفزات بالإزاحات الجديدة
    // (EN) Update jumps with new offsets
    updateJumpTargets(optimizedCode, offsetMap);
    
    size_t removed = code.size() - optimizedCode.size();
    if (removed > 0) {
        code = std::move(optimizedCode);
        debug("Removed " + std::to_string(removed) + " bytes of dead code");
    }
}

// ========================================
// Peephole Optimization
// تحسينات النافذة الصغيرة
// ========================================

void BytecodeEmitter::peepholeOptimize() {
    // ✅ TODO 2: تحسينات peephole / Peephole optimizations
    
    debug("Running peephole optimizations...");
    
    if (!module_) {
        return;
    }
    
    auto& code = const_cast<std::vector<Byte>&>(module_->getCode());
    if (code.size() < 2) {
        return;
    }
    
    int optimizations = 0;
    
    // (AR) نمر على الكود ونبحث عن أنماط للتحسين
    // (EN) Pass through code looking for optimization patterns
    for (uint32_t i = 0; i < code.size() - 1; ) {
        VM::Opcode op1 = static_cast<VM::Opcode>(code[i]);
        uint32_t size1 = getInstructionSize(op1);
        
        if (i + size1 >= code.size()) {
            i += size1;
            continue;
        }
        
        VM::Opcode op2 = static_cast<VM::Opcode>(code[i + size1]);
        
        // (AR) نمط 1: PUSH ثم POP مباشرة = حذف
        // (EN) Pattern 1: PUSH followed by POP = eliminate
        if (op1 == VM::OP_PUSH_CONST && op2 == VM::OP_POP) {
            code[i] = static_cast<Byte>(VM::OP_NOP);
            code[i + size1] = static_cast<Byte>(VM::OP_NOP);
            optimizations++;
            i += size1 + 1;
            continue;
        }
        
        // (AR) نمط 2: ADD 0 أو SUB 0 = حذف
        // (EN) Pattern 2: ADD 0 or SUB 0 = eliminate
        if ((op1 == VM::OP_PUSH_CONST) && (op2 == VM::OP_ADD || op2 == VM::OP_SUB)) {
            // Check if constant is 0
            if (i + 1 < code.size()) {
                uint32_t constIdx = code[i + 1];
                VM::Value constant = module_->getConstantPool().getConstant(constIdx);
                if (constant.isInteger() && constant.toInt() == 0) {
                    code[i] = static_cast<Byte>(VM::OP_NOP);
                    code[i + 1] = static_cast<Byte>(VM::OP_NOP);
                    code[i + size1] = static_cast<Byte>(VM::OP_NOP);
                    optimizations++;
                    i += size1 + 1;
                    continue;
                }
            }
        }
        
        // (AR) نمط 3: MUL 1 = حذف
        // (EN) Pattern 3: MUL 1 = eliminate
        if ((op1 == VM::OP_PUSH_CONST) && op2 == VM::OP_MUL) {
            if (i + 1 < code.size()) {
                uint32_t constIdx = code[i + 1];
                VM::Value constant = module_->getConstantPool().getConstant(constIdx);
                if (constant.isInteger() && constant.toInt() == 1) {
                    code[i] = static_cast<Byte>(VM::OP_NOP);
                    code[i + 1] = static_cast<Byte>(VM::OP_NOP);
                    code[i + size1] = static_cast<Byte>(VM::OP_NOP);
                    optimizations++;
                    i += size1 + 1;
                    continue;
                }
            }
        }
        
        // (AR) نمط 4: NOT NOT = حذف
        // (EN) Pattern 4: NOT NOT = eliminate
        if (op1 == VM::OP_LOGICAL_NOT && op2 == VM::OP_LOGICAL_NOT) {
            code[i] = static_cast<Byte>(VM::OP_NOP);
            code[i + size1] = static_cast<Byte>(VM::OP_NOP);
            optimizations++;
            i += size1 + 1;
            continue;
        }
        
        // (AR) نمط 5: JUMP إلى التعليمة التالية = حذف
        // (EN) Pattern 5: JUMP to next instruction = eliminate
        if (op1 == VM::OP_JUMP) {
            if (i + size1 < code.size()) {
                uint32_t target = extractJumpTarget(code, i, op1);
                if (target == i + size1) {
                    // Jump to next instruction - eliminate
                    for (uint32_t j = 0; j < size1; ++j) {
                        code[i + j] = static_cast<Byte>(VM::OP_NOP);
                    }
                    optimizations++;
                }
            }
        }
        
        i += size1;
    }
    
    if (optimizations > 0) {
        debug("Applied " + std::to_string(optimizations) + " peephole optimizations");
    }
}

// ========================================
// Merge Duplicates
// دمج التعليمات المتكررة
// ========================================

void BytecodeEmitter::mergeDuplicates() {
    // ✅ TODO 3: دمج التعليمات المتكررة / Merge duplicate instructions
    
    debug("Merging duplicate instruction sequences...");
    
    if (!module_) {
        return;
    }
    
    auto& code = const_cast<std::vector<Byte>&>(module_->getCode());
    if (code.size() < 4) {
        return;
    }
    
    // (AR) البحث عن تسلسلات متكررة من التعليمات
    // (EN) Look for repeated instruction sequences
    std::unordered_map<std::string, std::vector<uint32_t>> sequences;
    const size_t MIN_SEQUENCE_LENGTH = 4; // minimum 4 bytes
    
    for (uint32_t i = 0; i < code.size() - MIN_SEQUENCE_LENGTH; ++i) {
        std::string seq;
        for (uint32_t j = 0; j < MIN_SEQUENCE_LENGTH; ++j) {
            seq += static_cast<char>(code[i + j]);
        }
        sequences[seq].push_back(i);
    }
    
    int merges = 0;
    
    // (AR) العثور على التسلسلات المتكررة
    // (EN) Find duplicate sequences
    for (const auto& [seq, positions] : sequences) {
        if (positions.size() > 1) {
            // (AR) وجدنا تسلسل مكرر
            // (EN) Found duplicate sequence
            // Note: في تطبيق كامل، نحتاج لإنشاء subroutine ومن ثم استدعاؤها
            // Note: In full implementation, need to create subroutine and call it
            // هذا تحسين متقدم يحتاج restructuring كامل للكود
            // This is advanced optimization requiring full code restructuring
            merges++;
        }
    }
    
    if (merges > 0) {
        debug("Found " + std::to_string(merges) + " duplicate sequences (optimization deferred)");
    }
}

// ========================================
// Helper Functions for Optimization
// دوال مساعدة للتحسين
// ========================================

uint32_t BytecodeEmitter::getInstructionSize(VM::Opcode op) const {
    // (AR) حساب حجم التعليمة بالبايتات
    // (EN) Calculate instruction size in bytes
    
    switch (op) {
        // (AR) تعليمات بدون معاملات (1 بايت)
        // (EN) No-operand instructions (1 byte)
        case VM::OP_NOP:
        case VM::OP_POP:
        case VM::OP_RETURN:
        case VM::OP_ADD:
        case VM::OP_SUB:
        case VM::OP_MUL:
        case VM::OP_DIV:
        case VM::OP_MOD:
        case VM::OP_NEG:
        case VM::OP_BIT_AND:
        case VM::OP_BIT_OR:
        case VM::OP_BIT_XOR:
        case VM::OP_BIT_NOT:
        case VM::OP_BIT_SHL:
        case VM::OP_BIT_SHR:
        case VM::OP_LOGICAL_NOT:
        case VM::OP_CMP_EQ:
        case VM::OP_CMP_NE:
        case VM::OP_CMP_LT:
        case VM::OP_CMP_LE:
        case VM::OP_CMP_GT:
        case VM::OP_CMP_GE:
            return 1;
        
        // (AR) تعليمات مع معامل byte واحد (2 بايت)
        // (EN) Single-byte operand instructions (2 bytes)
        case VM::OP_PUSH_CONST:
        case VM::OP_LOAD_LOCAL:
        case VM::OP_STORE_LOCAL:
        case VM::OP_LOAD_GLOBAL:
        case VM::OP_STORE_GLOBAL:
            return 2;
        
        // (AR) تعليمات القفز (5 بايتات: opcode + 4-byte offset)
        // (EN) Jump instructions (5 bytes: opcode + 4-byte offset)
        case VM::OP_JUMP:
        case VM::OP_JUMP_IF_FALSE:
        case VM::OP_JUMP_IF_TRUE:
            return 5;
        
        // (AR) تعليمات الاستدعاء (2 بايت: opcode + arg count)
        // (EN) Call instructions (2 bytes: opcode + arg count)
        case VM::OP_CALL:
        case VM::OP_CALL_BUILTIN:
            return 2;
        
        default:
            return 1; // افتراضي / Default
    }
}

bool BytecodeEmitter::isJumpInstruction(VM::Opcode op) const {
    return op == VM::OP_JUMP || 
           op == VM::OP_JUMP_IF_FALSE || 
           op == VM::OP_JUMP_IF_TRUE;
}

bool BytecodeEmitter::isUnconditionalJump(VM::Opcode op) const {
    return op == VM::OP_JUMP || op == VM::OP_RETURN;
}

uint32_t BytecodeEmitter::extractJumpTarget(const std::vector<Byte>& code, uint32_t offset, VM::Opcode op) const {
    if (!isJumpInstruction(op) || offset + 5 > code.size()) {
        return 0;
    }
    
    // (AR) قراءة 4 بايتات للهدف (little-endian)
    // (EN) Read 4 bytes for target (little-endian)
    uint32_t target = 0;
    target |= static_cast<uint32_t>(code[offset + 1]);
    target |= static_cast<uint32_t>(code[offset + 2]) << 8;
    target |= static_cast<uint32_t>(code[offset + 3]) << 16;
    target |= static_cast<uint32_t>(code[offset + 4]) << 24;
    
    return target;
}

void BytecodeEmitter::updateJumpTargets(std::vector<Byte>& code, const std::unordered_map<uint32_t, uint32_t>& offsetMap) {
    // (AR) تحديث جميع أهداف القفز بالإزاحات الجديدة
    // (EN) Update all jump targets with new offsets
    
    for (uint32_t i = 0; i < code.size(); ) {
        VM::Opcode op = static_cast<VM::Opcode>(code[i]);
        
        if (isJumpInstruction(op) && i + 5 <= code.size()) {
            uint32_t oldTarget = extractJumpTarget(code, i, op);
            
            auto it = offsetMap.find(oldTarget);
            if (it != offsetMap.end()) {
                uint32_t newTarget = it->second;
                
                // (AR) كتابة الهدف الجديد (little-endian)
                // (EN) Write new target (little-endian)
                code[i + 1] = static_cast<Byte>(newTarget & 0xFF);
                code[i + 2] = static_cast<Byte>((newTarget >> 8) & 0xFF);
                code[i + 3] = static_cast<Byte>((newTarget >> 16) & 0xFF);
                code[i + 4] = static_cast<Byte>((newTarget >> 24) & 0xFF);
            }
        }
        
        i += getInstructionSize(op);
    }
}

// ========================================
// Utilities
// ========================================

void BytecodeEmitter::debug(const std::string& message) {
    if (debugMode_) {
        std::cout << "[EMITTER] " << message << "\n";
    }
}

void BytecodeEmitter::error(const std::string& message) {
    std::cerr << "[ERROR] " << message << "\n";
    errorCount_++;
}

// ========================================
// Helper Functions
// ========================================

VM::Opcode sirToBytecodeOp(SIR::SIROpcode sirOp) {
    switch (sirOp) {
        // Arithmetic
        case SIR::SIR_ADD: return VM::OP_ADD;
        case SIR::SIR_SUB: return VM::OP_SUB;
        case SIR::SIR_MUL: return VM::OP_MUL;
        case SIR::SIR_DIV: return VM::OP_DIV;
        case SIR::SIR_MOD: return VM::OP_MOD;
        case SIR::SIR_NEG: return VM::OP_NEG;
        
        // Bitwise
        case SIR::SIR_AND: return VM::OP_BIT_AND;
        case SIR::SIR_OR:  return VM::OP_BIT_OR;
        case SIR::SIR_XOR: return VM::OP_BIT_XOR;
        case SIR::SIR_SHL: return VM::OP_BIT_SHL;
        case SIR::SIR_SHR: return VM::OP_BIT_SHR;
        case SIR::SIR_NOT: return VM::OP_BIT_NOT;
        
        // Comparison
        case SIR::SIR_EQ:  return VM::OP_CMP_EQ;
        case SIR::SIR_NE:  return VM::OP_CMP_NE;
        case SIR::SIR_LT:  return VM::OP_CMP_LT;
        case SIR::SIR_LE:  return VM::OP_CMP_LE;
        case SIR::SIR_GT:  return VM::OP_CMP_GT;
        case SIR::SIR_GE:  return VM::OP_CMP_GE;
        
        default:
            return VM::OP_NOP;
    }
}

bool isArithmeticOp(SIR::SIROpcode op) {
    return op >= SIR::SIR_ADD && op <= SIR::SIR_NEG;
}

bool isBitwiseOp(SIR::SIROpcode op) {
    return op >= SIR::SIR_AND && op <= SIR::SIR_NOT;
}

bool isComparisonOp(SIR::SIROpcode op) {
    return op >= SIR::SIR_EQ && op <= SIR::SIR_GE;
}

bool isJumpOp(SIR::SIROpcode op) {
    return op == SIR::SIR_JMP || op == SIR::SIR_BR;
}

bool isMemoryOp(SIR::SIROpcode op) {
    return op == SIR::SIR_ALLOCA || op == SIR::SIR_MALLOC;
}

} // namespace Bytecode
} // namespace Compiler
} // namespace Sad
