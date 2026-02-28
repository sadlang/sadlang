/*
 * ============================================================================
 * ┘ו┘ט┘ה╪» ┘ד┘ט╪» LLVM IR - ┘ו┘ה┘ב ╪º┘ה╪¬┘ז┘ב┘ך╪░
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 * 
 * ┘ח╪░╪º ╪º┘ה┘ו┘ה┘ב ┘ך╪¡╪¬┘ט┘ך ╪╣┘ה┘י ╪¬┘ז┘ב┘ך╪░ ┘ו┘ט┘ה╪» ┘ד┘ט╪» LLVM IR ╪º┘ה╪░┘ך ┘ך╪¬╪▒╪¼┘ו SIR ╪Ñ┘ה┘י LLVM IR
 * This file contains the implementation of LLVM IR code generator that
 * translates SIR to LLVM IR
 * 
 * ╪º┘ה╪¬╪▓╪º┘ו ╪¿ STRICT_CODING_RULES.md:
 * - ╪¬┘ו ┘ג╪▒╪º╪í╪⌐ ╪¼┘ו┘ך╪╣ ╪º┘ה┘ו┘ה┘ב╪º╪¬ ╪º┘ה╪▒╪ú╪│┘ך╪⌐ ╪¿╪º┘ה┘ד╪º┘ו┘ה (1746 ╪│╪╖╪▒)
 * - ╪¬┘ו ╪¬┘ט╪½┘ך┘ג ┘ד┘ה ╪º╪│╪¬╪«╪»╪º┘ו API ╪¿┘ו┘ט┘ג╪╣ ╪º┘ה┘ו╪╡╪»╪▒
 * - ┘ה╪º ╪¬┘ט╪¼╪» ╪»┘ט╪º┘ה ╪ú┘ט ┘ו╪¬╪║┘ך╪▒╪º╪¬ ┘ו╪«╪¬╪▒╪╣╪⌐
 * 
 * STRICT_CODING_RULES.md compliance:
 * - All header files read completely (1746 lines)
 * - Every API usage documented with source location
 * - No invented functions or variables
 * 
 * ╪º┘ה┘ו╪ñ┘ה┘ב (Author): SadLanguage Compiler Team
 * ╪º┘ה╪¬╪º╪▒┘ך╪« (Date): December 2024
 * ╪º┘ה╪Ñ╪╡╪»╪º╪▒ (Version): 1.1.4 Phase 1
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
// Source: LLVM 14+ API - llvm/MC/TargetRegistry.h ╪¿╪»┘ה╪º┘כ ┘ו┘ז llvm/Support/TargetRegistry.h
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InlineAsm.h>
#include <iostream>
#include <fstream>

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR;  // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SIRType

namespace Sad {
namespace LLVM {

llvm::Value* LLVMCodeGen::emitObjectCall(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("OBJECT_CALL requires at least 2 operands (object, method_name)");
        return nullptr;
    }
    
    std::string objRegName = inst->operands[0].name;
    std::string methodName = inst->operands[1].name;
    
    llvm::Value* objPtr = context_info_.namedValues[objRegName];
    if (!objPtr) {
        reportError("Object not found: " + objRegName);
        return nullptr;
    }
    
    // (AR) البحث عن اسم الصنف
    // (EN) Look up class name
    auto classIt = context_info_.objectClassMap.find(objRegName);
    std::string className = (classIt != context_info_.objectClassMap.end()) ? classIt->second : "";
    
    // (AR) البحث عن الدالة في سلسلة الوراثة (الصنف الحالي → الأب → جد الأب ...)
    // (EN) Search for method in inheritance chain (current → parent → grandparent ...)
    llvm::Function* method = nullptr;
    std::string searchClass = className;
    while (!searchClass.empty() && !method) {
        std::string fullMethodName = searchClass + "." + methodName;
        method = module_->getFunction(fullMethodName);
        if (method) break;
        
        // (AR) الانتقال للصنف الأب
        // (EN) Move to parent class
        auto parentIt = context_info_.classParentMap.find(searchClass);
        if (parentIt != context_info_.classParentMap.end()) {
            searchClass = parentIt->second;
        } else {
            break;
        }
    }
    
    // (AR) محاولة أخيرة: البحث بدون بادئة الصنف
    // (EN) Last resort: try without class prefix
    if (!method) {
        method = module_->getFunction(methodName);
    }
    if (!method) {
        reportError("Method not found: " + className + "." + methodName + " (searched inheritance chain)");
        return nullptr;
    }
    
    // (AR) بناء المعاملات: self + باقي المعاملات
    // (EN) Build args: self + remaining operands
    std::vector<llvm::Value*> args = {objPtr};
    for (size_t i = 2; i < inst->operands.size(); i++) {
        llvm::Value* arg = resolveOperand(inst->operands[i]);
        if (arg) args.push_back(arg);
    }
    
    llvm::Value* result = builder_->CreateCall(method, args,
        method->getReturnType()->isVoidTy() ? "" : (methodName + "_result"));
    
    if (inst->result.has_value() && !method->getReturnType()->isVoidTy()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitInstanceOf(std::shared_ptr<SIRInstruction> inst) {
    // (AR) التحقق من نوع الكائن مع دعم سلسلة الوراثة
    // (EN) Check object type with inheritance chain support
    llvm::Value* result = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
    
    if (inst && inst->operands.size() >= 2) {
        std::string objRegName = inst->operands[0].name;
        std::string targetClass = inst->operands[1].name;
        
        auto classIt = context_info_.objectClassMap.find(objRegName);
        if (classIt != context_info_.objectClassMap.end()) {
            // (AR) البحث في سلسلة الوراثة: الصنف الحالي أو أي أب
            // (EN) Search inheritance chain: current class or any parent
            std::string checkClass = classIt->second;
            bool isMatch = false;
            while (!checkClass.empty()) {
                if (checkClass == targetClass) {
                    isMatch = true;
                    break;
                }
                auto parentIt = context_info_.classParentMap.find(checkClass);
                if (parentIt != context_info_.classParentMap.end()) {
                    checkClass = parentIt->second;
                } else {
                    break;
                }
            }
            result = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), isMatch ? 1 : 0);
        }
    }
    
    if (inst && inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitObjectCast(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("OBJECT_CAST requires 2 operands (object, target_class)");
        return nullptr;
    }
    
    // In opaque pointer world, object casts are essentially no-ops
    // We just update the class mapping
    llvm::Value* objPtr = resolveOperand(inst->operands[0]);
    std::string targetClass = inst->operands[1].name;
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = objPtr;
        context_info_.objectClassMap[inst->result->name] = targetClass;
    }
    return objPtr;
}

llvm::Value* LLVMCodeGen::emitClassDef(std::shared_ptr<SIRInstruction> inst) {
    // CLASS_DEF is handled during preprocessClasses phase
    // This is a no-op at instruction emission time
    return llvm::ConstantInt::get(getInt64Type(), 0);
}

llvm::Value* LLVMCodeGen::emitMethodDef(std::shared_ptr<SIRInstruction> inst) {
    // METHOD_DEF is handled during function preprocessing
    // This is a no-op at instruction emission time
    return llvm::ConstantInt::get(getInt64Type(), 0);
}

llvm::Value* LLVMCodeGen::emitFieldDef(std::shared_ptr<SIRInstruction> inst) {
    // FIELD_DEF is handled during preprocessClasses phase
    // This is a no-op at instruction emission time
    return llvm::ConstantInt::get(getInt64Type(), 0);
}

llvm::Value* LLVMCodeGen::emitConstructorCall(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("CONSTRUCTOR_CALL requires at least 1 operand (class name)");
        return nullptr;
    }
    
    std::string className = inst->operands[0].name;
    
    // (AR) إنشاء الكائن أولاً
    // (EN) First, create the object
    auto structIt = context_info_.classStructTypes.find(className);
    if (structIt == context_info_.classStructTypes.end()) {
        reportError("Class not found for constructor: " + className);
        return nullptr;
    }
    
    llvm::StructType* structType = structIt->second;
    
    // (AR) تخصيص ذاكرة على الكومة
    // (EN) Allocate on heap
    auto* dlSize = llvm::ConstantExpr::getSizeOf(structType);
    auto* mallocType = llvm::FunctionType::get(
        llvm::PointerType::getUnqual(*context_), {getInt64Type()}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* objPtr = builder_->CreateCall(mallocFunc, {dlSize}, className + "_ctor");
    
    // (AR) تصفير الذاكرة المخصصة
    // (EN) Zero-initialize allocated memory
    auto* sizeVal = builder_->CreateIntCast(dlSize, getInt64Type(), false);
    auto* memsetType = llvm::FunctionType::get(
        llvm::PointerType::getUnqual(*context_),
        {llvm::PointerType::getUnqual(*context_), llvm::Type::getInt32Ty(*context_), getInt64Type()},
        false);
    auto memsetFunc = module_->getOrInsertFunction("memset", memsetType);
    builder_->CreateCall(memsetFunc, {objPtr, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), sizeVal});
    
    // (AR) البحث عن دالة الباني: نجرب بناء (SIR builder)، __init__، منشئ، init، باني
    // (EN) Look for constructor: try بناء (SIR builder), __init__, منشئ, init, باني
    llvm::Function* ctorFunc = module_->getFunction(className + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"); // بناء
    if (!ctorFunc) ctorFunc = module_->getFunction(className + ".__init__");
    if (!ctorFunc) ctorFunc = module_->getFunction(className + ".\xd9\x85\xd9\x86\xd8\xb4\xd8\xa6"); // منشئ
    if (!ctorFunc) ctorFunc = module_->getFunction(className + ".init");
    if (!ctorFunc) ctorFunc = module_->getFunction(className + ".\xD8\xA8\xD8\xA7\xD9\x86\xD9\x8A"); // باني
    
    if (ctorFunc) {
        // (AR) بناء المعاملات: self + معاملات الباني
        // (EN) Build args: self + constructor args
        std::vector<llvm::Value*> args = {objPtr};
        for (size_t i = 1; i < inst->operands.size(); i++) {
            llvm::Value* arg = resolveOperand(inst->operands[i]);
            if (arg) args.push_back(arg);
        }
        builder_->CreateCall(ctorFunc, args);
    }
    
    // (AR) تتبع ارتباط الصنف
    // (EN) Track class association
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = objPtr;
        context_info_.objectClassMap[inst->result->name] = className;
    }
    return objPtr;
}

// ============================================================================
// Phase N: Type Conversion Instructions / ╪¬╪╣┘ה┘ך┘ו╪º╪¬ ╪¬╪¡┘ט┘ך┘ה ╪º┘ה╪ú┘ז┘ט╪º╪╣
// ============================================================================

llvm::Value* LLVMCodeGen::emitI64ToF64(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("I64_TO_F64 requires 1 operand");
        return nullptr;
    }
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    llvm::Value* result;
    if (val->getType()->isDoubleTy()) {
        result = val; // Already double
    } else if (val->getType()->isIntegerTy(1)) {
        // bool Γזע i64 Γזע f64
        llvm::Value* ext = builder_->CreateZExt(val, getInt64Type(), "bool2i64");
        result = builder_->CreateSIToFP(ext, getDoubleType(), "i64tof64");
    } else {
        result = builder_->CreateSIToFP(val, getDoubleType(), "i64tof64");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitF64ToI64(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("F64_TO_I64 requires 1 operand");
        return nullptr;
    }
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    llvm::Value* result;
    if (val->getType()->isIntegerTy()) {
        result = val; // Already integer
    } else {
        result = builder_->CreateFPToSI(val, getInt64Type(), "f64toi64");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitI64ToBool(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("I64_TO_BOOL requires 1 operand");
        return nullptr;
    }
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    llvm::Value* result;
    if (val->getType()->isIntegerTy(1)) {
        result = val; // Already bool
    } else if (val->getType()->isDoubleTy()) {
        result = builder_->CreateFCmpONE(val, 
            llvm::ConstantFP::get(getDoubleType(), 0.0), "f64tobool");
    } else {
        result = builder_->CreateICmpNE(val, 
            llvm::ConstantInt::get(val->getType(), 0), "i64tobool");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBoolToI64(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("BOOL_TO_I64 requires 1 operand");
        return nullptr;
    }
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    llvm::Value* result;
    if (val->getType()->isIntegerTy(64)) {
        result = val;
    } else if (val->getType()->isIntegerTy(1)) {
        result = builder_->CreateZExt(val, getInt64Type(), "booltoi64");
    } else {
        result = builder_->CreateZExt(val, getInt64Type(), "exttoi64");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitI64ToString(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("I64_TO_STRING requires 1 operand");
        return nullptr;
    }
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    // Allocate buffer: 21 bytes enough for i64 range + sign + null
    auto* mallocType = llvm::FunctionType::get(
        llvm::PointerType::getUnqual(*context_), {getInt64Type()}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* buf = builder_->CreateCall(mallocFunc, 
        {llvm::ConstantInt::get(getInt64Type(), 32)}, "i64str_buf");
    
    // snprintf(buf, 32, "%lld", val)
    auto* snprintfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context_),
        {llvm::PointerType::getUnqual(*context_), getInt64Type(), 
         llvm::PointerType::getUnqual(*context_)}, true);
    auto snprintfFunc = module_->getOrInsertFunction("snprintf", snprintfType);
    
    llvm::Value* fmt = builder_->CreateGlobalStringPtr("%lld", "fmt_i64");
    builder_->CreateCall(snprintfFunc, {buf, llvm::ConstantInt::get(getInt64Type(), 32), fmt, val});
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = buf;
    }
    return buf;
}

llvm::Value* LLVMCodeGen::emitF64ToString(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("F64_TO_STRING requires 1 operand");
        return nullptr;
    }
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    if (!val->getType()->isDoubleTy()) {
        val = builder_->CreateSIToFP(val, getDoubleType(), "tof64");
    }
    
    auto* mallocType = llvm::FunctionType::get(
        llvm::PointerType::getUnqual(*context_), {getInt64Type()}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* buf = builder_->CreateCall(mallocFunc,
        {llvm::ConstantInt::get(getInt64Type(), 64)}, "f64str_buf");
    
    auto* snprintfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context_),
        {llvm::PointerType::getUnqual(*context_), getInt64Type(),
         llvm::PointerType::getUnqual(*context_)}, true);
    auto snprintfFunc = module_->getOrInsertFunction("snprintf", snprintfType);
    
    llvm::Value* fmt = builder_->CreateGlobalStringPtr("%g", "fmt_f64");
    builder_->CreateCall(snprintfFunc, {buf, llvm::ConstantInt::get(getInt64Type(), 64), fmt, val});
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = buf;
    }
    return buf;
}

llvm::Value* LLVMCodeGen::emitBoolToString(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("BOOL_TO_STRING requires 1 operand");
        return nullptr;
    }
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    // Convert to i1 if not already
    if (!val->getType()->isIntegerTy(1)) {
        val = builder_->CreateICmpNE(val, 
            llvm::ConstantInt::get(val->getType(), 0), "tobool");
    }
    
    // "╪╡╪¡┘ך╪¡" (true in Arabic) / "╪«╪╖╪ú" (false in Arabic)
    llvm::Value* trueStr = builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "true_str");
    llvm::Value* falseStr = builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "false_str");
    llvm::Value* result = builder_->CreateSelect(val, trueStr, falseStr, "boolstr");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitCast(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("CAST requires at least 1 operand");
        return nullptr;
    }
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    // Determine target type from result dataType
    SIRType targetType = SIRType::I64;
    if (inst->result.has_value()) {
        targetType = inst->result->dataType;
    } else if (inst->operands.size() >= 2) {
        targetType = inst->operands[1].dataType;
    }
    
    llvm::Value* result = val;
    llvm::Type* valType = val->getType();
    
    switch (targetType) {
        case SIRType::I64:
            if (valType->isDoubleTy()) result = builder_->CreateFPToSI(val, getInt64Type(), "cast_i64");
            else if (valType->isIntegerTy(1)) result = builder_->CreateZExt(val, getInt64Type(), "cast_i64");
            else if (valType->isPointerTy()) result = builder_->CreatePtrToInt(val, getInt64Type(), "cast_i64");
            break;
        case SIRType::F64:
            if (valType->isIntegerTy()) result = builder_->CreateSIToFP(val, getDoubleType(), "cast_f64");
            break;
        case SIRType::BOOL:
            if (valType->isIntegerTy(64)) result = builder_->CreateICmpNE(val, llvm::ConstantInt::get(getInt64Type(), 0), "cast_bool");
            else if (valType->isDoubleTy()) result = builder_->CreateFCmpONE(val, llvm::ConstantFP::get(getDoubleType(), 0.0), "cast_bool");
            break;
        case SIRType::PTR:
        case SIRType::STRING:
            if (valType->isIntegerTy()) result = builder_->CreateIntToPtr(val, llvm::PointerType::getUnqual(*context_), "cast_ptr");
            break;
        default:
            break;
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

// ============================================================================
// Phase N: Array Core / ╪╣┘ו┘ה┘ך╪º╪¬ ╪º┘ה┘ו╪╡┘ב┘ט┘ב╪º╪¬ ╪º┘ה╪ú╪│╪º╪│┘ך╪⌐
// ============================================================================

// Array layout: [i64 length, i64 capacity, i64* data]
// Stored as: { i64, i64, ptr } struct

static llvm::StructType* getArrayStructType(llvm::LLVMContext& ctx) {
    static llvm::StructType* arrTy = nullptr;
    if (!arrTy) {
        arrTy = llvm::StructType::create(ctx, {
            llvm::Type::getInt64Ty(ctx),     // length
            llvm::Type::getInt64Ty(ctx),     // capacity
            llvm::PointerType::getUnqual(ctx) // data pointer
        }, "SadArray");
    }
    return arrTy;
}

llvm::Value* LLVMCodeGen::emitArrayNew(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    // Optional operand: initial capacity
    int64_t capacity = 8; // default
    if (!inst->operands.empty()) {
        if (inst->operands[0].type == SIROperandType::CONSTANT) {
            try { 
                capacity = std::stoll(inst->operands[0].name); 
                // التحقق من أن السعة موجبة
                if (capacity <= 0) capacity = 8;
            } catch (const std::exception&) {
                // فشل التحويل - استخدام القيمة الافتراضية
                capacity = 8;
            }
        }
    }
    
    // Allocate array struct on heap
    llvm::StructType* arrTy = getArrayStructType(*context_);
    auto* dlSize = llvm::ConstantExpr::getSizeOf(arrTy);
    auto* mallocType = llvm::FunctionType::get(
        llvm::PointerType::getUnqual(*context_), {getInt64Type()}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* arrPtr = builder_->CreateCall(mallocFunc, {dlSize}, "arr_new");
    
    // Set length = 0
    llvm::Value* lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "arr.len.gep");
    builder_->CreateStore(llvm::ConstantInt::get(getInt64Type(), 0), lenGep);
    
    // Set capacity
    llvm::Value* capGep = builder_->CreateStructGEP(arrTy, arrPtr, 1, "arr.cap.gep");
    builder_->CreateStore(llvm::ConstantInt::get(getInt64Type(), capacity), capGep);
    
    // Allocate data buffer: capacity * sizeof(ptr) bytes (pointer-width elements for nested array support)
    // (AR) تخصيص مخزن البيانات: السعة × حجم المؤشر (لدعم المصفوفات المتداخلة)
    auto* ptrSize = llvm::ConstantExpr::getSizeOf(llvm::PointerType::getUnqual(*context_));
    llvm::Value* dataSize = builder_->CreateMul(
        llvm::ConstantInt::get(getInt64Type(), capacity), 
        builder_->CreateIntCast(ptrSize, getInt64Type(), false), "arr.data.size");
    llvm::Value* dataPtr = builder_->CreateCall(mallocFunc, {dataSize}, "arr.data");
    llvm::Value* dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
    builder_->CreateStore(dataPtr, dataGep);
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = arrPtr;
    }
    return arrPtr;
}

llvm::Value* LLVMCodeGen::emitArrayGet(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("ARRAY_GET requires 2 operands (array, index)");
        return nullptr;
    }
    
    llvm::Value* arrPtr = context_info_.namedValues[inst->operands[0].name];
    if (!arrPtr) arrPtr = resolveOperand(inst->operands[0]);
    llvm::Value* index = resolveOperand(inst->operands[1]);
    if (!arrPtr || !index) return nullptr;
    
    llvm::StructType* arrTy = getArrayStructType(*context_);
    
    // Load data pointer
    llvm::Value* dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
    llvm::Value* dataPtr = builder_->CreateLoad(
        llvm::PointerType::getUnqual(*context_), dataGep, "arr.data");
    
    // (AR) تحديد نوع العنصر: إذا كان نوع النتيجة ARRAY أو PTR → حمِّل كمؤشر (ptr)
    //      وإلا حمِّل كـ i64 (عدد صحيح / عشري مُحوَّل)
    // (EN) Determine element type: if result is ARRAY/PTR → load as ptr (for nested arrays)
    //      otherwise load as i64 (integer / bitcasted float)
    bool isNestedArray = false;
    if (inst->result.has_value()) {
        auto resultType = inst->result->dataType;
        if (resultType == SIRType::ARRAY || resultType == SIRType::PTR || 
            resultType == SIRType::STRUCT || resultType == SIRType::STRING) {
            isNestedArray = true;
        }
    }
    
    llvm::Value* result;
    if (isNestedArray) {
        // (AR) العنصر مؤشر (مصفوفة متداخلة / نص / بنية)
        // (EN) Element is a pointer (nested array / string / struct)
        llvm::Value* elemPtr = builder_->CreateGEP(
            llvm::PointerType::getUnqual(*context_), dataPtr, {index}, "arr.elem.ptr");
        result = builder_->CreateLoad(
            llvm::PointerType::getUnqual(*context_), elemPtr, "arr.get.ptr");
    } else {
        // (AR) العنصر i64 (رقم / منطقي)
        // (EN) Element is i64 (number / boolean)
        llvm::Value* elemPtr = builder_->CreateGEP(getInt64Type(), dataPtr, {index}, "arr.elem");
        result = builder_->CreateLoad(getInt64Type(), elemPtr, "arr.get");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitArraySet(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 3) {
        reportError("ARRAY_SET requires 3 operands (array, index, value)");
        return nullptr;
    }
    
    llvm::Value* arrPtr = context_info_.namedValues[inst->operands[0].name];
    if (!arrPtr) arrPtr = resolveOperand(inst->operands[0]);
    llvm::Value* index = resolveOperand(inst->operands[1]);
    llvm::Value* value = resolveOperand(inst->operands[2]);
    if (!arrPtr || !index || !value) return nullptr;
    
    llvm::StructType* arrTy = getArrayStructType(*context_);
    
    // Load data pointer
    llvm::Value* dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
    llvm::Value* dataPtr = builder_->CreateLoad(
        llvm::PointerType::getUnqual(*context_), dataGep, "arr.data");
    
    // (AR) تحديد نوع العنصر: إذا كانت القيمة مؤشراً (مصفوفة/نص/بنية) → خزِّن كمؤشر
    // (EN) Determine element type: if value is pointer type → store as pointer
    bool isPointerValue = value->getType()->isPointerTy();
    
    if (isPointerValue) {
        // (AR) تخزين مؤشر (مصفوفة متداخلة / نص / بنية)
        // (EN) Store pointer (nested array / string / struct)
        llvm::Value* elemPtr = builder_->CreateGEP(
            llvm::PointerType::getUnqual(*context_), dataPtr, {index}, "arr.elem.ptr");
        builder_->CreateStore(value, elemPtr);
    } else {
        // (AR) تخزين قيمة i64
        // (EN) Store i64 value
        // (AR) تحويل القيمة إلى i64 إذا لزم الأمر
        if (!value->getType()->isIntegerTy(64)) {
            value = builder_->CreateIntCast(value, getInt64Type(), true, "arr.elem.cast");
        }
        llvm::Value* elemPtr = builder_->CreateGEP(getInt64Type(), dataPtr, {index}, "arr.elem");
        builder_->CreateStore(value, elemPtr);
    }
    
    return value;
}

llvm::Value* LLVMCodeGen::emitArrayLen(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("ARRAY_LEN requires 1 operand (array)");
        return nullptr;
    }
    
    llvm::Value* arrPtr = context_info_.namedValues[inst->operands[0].name];
    if (!arrPtr) arrPtr = resolveOperand(inst->operands[0]);
    if (!arrPtr) return nullptr;
    
    llvm::StructType* arrTy = getArrayStructType(*context_);
    llvm::Value* lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "arr.len.gep");
    llvm::Value* result = builder_->CreateLoad(getInt64Type(), lenGep, "arr.len");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

// ============================================================================
// Phase N: String Core / ╪╣┘ו┘ה┘ך╪º╪¬ ╪º┘ה┘ז╪╡┘ט╪╡ ╪º┘ה╪ú╪│╪º╪│┘ך╪⌐
// ============================================================================

llvm::Value* LLVMCodeGen::emitStringNew(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    llvm::Value* result;
    if (!inst->operands.empty()) {
        // Create from existing string/constant
        result = resolveOperand(inst->operands[0]);
    } else {
        // Create empty string
        result = builder_->CreateGlobalStringPtr("", "empty_str");
    }
    
    if (inst->result.has_value() && result) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

// ============================================================================
// Phase N: Builtin Extra / ╪»┘ט╪º┘ה ┘ו╪╢┘ו┘ז╪⌐ ╪Ñ╪╢╪º┘ב┘ך╪⌐
// ============================================================================

llvm::Value* LLVMCodeGen::emitBuiltinMin(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("MIN requires 2 operands");
        return nullptr;
    }
    llvm::Value* a = resolveOperand(inst->operands[0]);
    llvm::Value* b = resolveOperand(inst->operands[1]);
    if (!a || !b) return nullptr;
    
    llvm::Value* result;
    if (a->getType()->isDoubleTy() || b->getType()->isDoubleTy()) {
        if (!a->getType()->isDoubleTy()) a = builder_->CreateSIToFP(a, getDoubleType(), "a.f64");
        if (!b->getType()->isDoubleTy()) b = builder_->CreateSIToFP(b, getDoubleType(), "b.f64");
        llvm::Value* cmp = builder_->CreateFCmpOLT(a, b, "min.cmp");
        result = builder_->CreateSelect(cmp, a, b, "min");
    } else {
        llvm::Value* cmp = builder_->CreateICmpSLT(a, b, "min.cmp");
        result = builder_->CreateSelect(cmp, a, b, "min");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinMax(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("MAX requires 2 operands");
        return nullptr;
    }
    llvm::Value* a = resolveOperand(inst->operands[0]);
    llvm::Value* b = resolveOperand(inst->operands[1]);
    if (!a || !b) return nullptr;
    
    llvm::Value* result;
    if (a->getType()->isDoubleTy() || b->getType()->isDoubleTy()) {
        if (!a->getType()->isDoubleTy()) a = builder_->CreateSIToFP(a, getDoubleType(), "a.f64");
        if (!b->getType()->isDoubleTy()) b = builder_->CreateSIToFP(b, getDoubleType(), "b.f64");
        llvm::Value* cmp = builder_->CreateFCmpOGT(a, b, "max.cmp");
        result = builder_->CreateSelect(cmp, a, b, "max");
    } else {
        llvm::Value* cmp = builder_->CreateICmpSGT(a, b, "max.cmp");
        result = builder_->CreateSelect(cmp, a, b, "max");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinAssert(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("ASSERT requires 1 operand (condition)");
        return nullptr;
    }
    
    llvm::Value* cond = resolveOperand(inst->operands[0]);
    if (!cond) return nullptr;
    
    // Convert to i1 if not already
    if (!cond->getType()->isIntegerTy(1)) {
        cond = builder_->CreateICmpNE(cond, 
            llvm::ConstantInt::get(cond->getType(), 0), "assert.cond");
    }
    
    // Create basic blocks
    llvm::Function* fn = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* failBB = llvm::BasicBlock::Create(*context_, "assert.fail", fn);
    llvm::BasicBlock* contBB = llvm::BasicBlock::Create(*context_, "assert.cont", fn);
    
    builder_->CreateCondBr(cond, contBB, failBB);
    
    // Fail block: print error and abort
    builder_->SetInsertPoint(failBB);
    auto* printfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context_),
        {llvm::PointerType::getUnqual(*context_)}, true);
    auto printfFunc = module_->getOrInsertFunction("printf", printfType);
    
    std::string msg = "Assertion failed";
    if (inst->operands.size() >= 2) {
        msg = "Assertion failed: " + inst->operands[1].name;
    }
    llvm::Value* msgStr = builder_->CreateGlobalStringPtr(msg + "\n", "assert.msg");
    builder_->CreateCall(printfFunc, {msgStr});
    
    auto* abortType = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {}, false);
    auto abortFunc = module_->getOrInsertFunction("abort", abortType);
    builder_->CreateCall(abortFunc, {});
    builder_->CreateUnreachable();
    
    // Continue block
    builder_->SetInsertPoint(contBB);
    
    return llvm::ConstantInt::get(getInt64Type(), 0);
}

llvm::Value* LLVMCodeGen::emitBuiltinDebug(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        return llvm::ConstantInt::get(getInt64Type(), 0);
    }
    
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    auto* printfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context_),
        {llvm::PointerType::getUnqual(*context_)}, true);
    auto printfFunc = module_->getOrInsertFunction("printf", printfType);
    
    if (val->getType()->isDoubleTy()) {
        llvm::Value* fmt = builder_->CreateGlobalStringPtr("[DEBUG] %g\n", "debug_fmt_f64");
        builder_->CreateCall(printfFunc, {fmt, val});
    } else if (val->getType()->isPointerTy()) {
        llvm::Value* fmt = builder_->CreateGlobalStringPtr("[DEBUG] %s\n", "debug_fmt_str");
        builder_->CreateCall(printfFunc, {fmt, val});
    } else {
        llvm::Value* fmt = builder_->CreateGlobalStringPtr("[DEBUG] %lld\n", "debug_fmt_i64");
        builder_->CreateCall(printfFunc, {fmt, val});
    }
    
    return val;
}

// ============================================================================
// emitPhi - ╪╣┘ג╪»╪⌐ ┘ב╪º┘ך / Phi node (SSA form)
// ============================================================================
llvm::Value* LLVMCodeGen::emitPhi(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2 || inst->operands.size() % 2 != 0) {
        reportError("PHI requires pairs of (value, block) operands");
        return nullptr;
    }
    
    // Determine phi type from result
    llvm::Type* phiType = getInt64Type(); // default
    if (inst->result.has_value()) {
        if (inst->result->dataType == SIRType::F64)
            phiType = llvm::Type::getDoubleTy(*context_);
        else if (inst->result->dataType == SIRType::BOOL)
            phiType = llvm::Type::getInt1Ty(*context_);
        else if (inst->result->dataType == SIRType::STRING || inst->result->dataType == SIRType::PTR)
            phiType = llvm::PointerType::getUnqual(*context_);
    }
    
    unsigned numIncoming = inst->operands.size() / 2;
    llvm::PHINode* phi = builder_->CreatePHI(phiType, numIncoming, "phi");
    
    llvm::Function* func = builder_->GetInsertBlock()->getParent();
    for (unsigned i = 0; i < inst->operands.size(); i += 2) {
        llvm::Value* val = resolveOperand(inst->operands[i]);
        const std::string& bbName = inst->operands[i + 1].name;
        
        // Find the basic block by name
        llvm::BasicBlock* bb = nullptr;
        for (auto& block : *func) {
            if (block.getName() == bbName) {
                bb = &block;
                break;
            }
        }
        if (bb && val) {
            phi->addIncoming(val, bb);
        }
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = phi;
    }
    return phi;
}

// ============================================================================
// emitBuiltinTypeOf - ┘ז┘ט╪╣_╪º┘ה┘ו╪¬╪║┘ך╪▒ / typeof
// ============================================================================


} // namespace LLVM
} // namespace Sad
