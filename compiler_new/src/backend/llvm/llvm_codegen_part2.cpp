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
#include <set>  // For pre-scan label discovery
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

void LLVMCodeGen::emitConstants(std::shared_ptr<SIRModule> sirModule) {
    if (!sirModule) {
        return;
    }
    
    // (AR) ╪Ñ╪╡╪»╪º╪▒ ╪º┘ה╪½┘ט╪º╪¿╪¬ ╪º┘ה┘ז╪╡┘ך╪⌐ ┘ד┘ו╪¬╪║┘ך╪▒╪º╪¬ ╪╣╪º┘ו╪⌐
    // (EN) Emit string constants as global variables
    const auto& stringConstants = sirModule->getConstants();
    
    for (size_t i = 0; i < stringConstants.size(); i++) {
        const std::string& str = stringConstants[i];
        std::string constName = "str.const." + std::to_string(i);
        
        // (AR) ╪¬╪¡┘ג┘ג ╪ú┘ז ╪º┘ה╪½╪º╪¿╪¬ ┘ה┘ו ┘ך┘ן┘ז╪┤╪ú ┘ו┘ז ┘ג╪¿┘ה
        // (EN) Check constant hasn't been created already
        if (module_->getGlobalVariable(constName)) {
            continue;
        }
        
        // (AR) ╪Ñ┘ז╪┤╪º╪í ┘ו╪╡┘ב┘ט┘ב╪⌐ ╪½╪º╪¿╪¬╪⌐ ┘ה┘ה┘ז╪╡
        // (EN) Create constant array for the string
        llvm::Constant* strConstant = llvm::ConstantDataArray::getString(*context_, str, true);
        
        // (AR) ╪Ñ┘ז╪┤╪º╪í ┘ו╪¬╪║┘ך╪▒ ╪╣╪º┘ו ╪½╪º╪¿╪¬
        // (EN) Create constant global variable
        auto* gv = new llvm::GlobalVariable(
            *module_,
            strConstant->getType(),
            true,  // isConstant
            llvm::GlobalValue::PrivateLinkage,
            strConstant,
            constName
        );
        gv->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
        gv->setAlignment(llvm::Align(1));
    }
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪º┘ה╪»┘ט╪º┘ה ╪º┘ה╪╣╪º┘ו╪⌐
 * Emit global functions
 * 
 * Source: llvm_codegen.h:323
 * @param sirModule ┘ט╪¡╪»╪⌐ SIR / SIR module
 */
void LLVMCodeGen::emitGlobalFunctions(std::shared_ptr<SIRModule> sirModule) {
    if (!sirModule) {
        return;
    }
    
    // Source: SIRModule::getFunctions() returns const std::vector<std::shared_ptr<SIRFunction>>&
    const auto& functions = sirModule->getFunctions();
    
    // ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 1: ╪Ñ┘ז╪┤╪º╪í ╪¬┘ט┘ג┘ך╪╣╪º╪¬ ╪¼┘ו┘ך╪╣ ╪º┘ה╪»┘ט╪º┘ה ╪ú┘ט┘ה╪º┘כ
    // Phase 1: Create all function signatures first
    for (const auto& sirFunc : functions) {
        if (!sirFunc) continue;
        
        llvm::Function* llvmFunc = emitFunctionPrototype(sirFunc);
        if (llvmFunc) {
            // ╪¡┘ב╪╕ ┘ב┘ך ╪º┘ה╪│┘ך╪º┘ג
            // Save to context
            // Source: context_info_ is defined at llvm_codegen.h:643
            // Source: CodeGenContext::functions is at llvm_codegen.h:618
            // Source: SIRFunction::getName() is at sir_module.h:306
            context_info_.functions[sirFunc->getName()] = llvmFunc;
        }
    }
    
    // ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 2: ╪Ñ╪╡╪»╪º╪▒ ╪ú╪¼╪│╪º┘ו ╪º┘ה╪»┘ט╪º┘ה
    // Phase 2: Emit function bodies
    for (const auto& sirFunc : functions) {
        if (!sirFunc) continue;
        
        // Source: SIRFunction::getName() is at sir_module.h:306
        auto it = context_info_.functions.find(sirFunc->getName());
        if (it != context_info_.functions.end()) {
            emitFunctionBody(sirFunc, it->second);
        }
    }
    
    // ╪Ñ╪╢╪º┘ב╪⌐ ╪»╪º┘ה╪⌐ main ┘ד┘א wrapper ┘ה┘ה╪»╪º┘ה╪⌐ ╪º┘ה╪▒╪ª┘ך╪│┘ך╪⌐ ╪º┘ה╪╣╪▒╪¿┘ך╪⌐
    // Add main function as wrapper for Arabic main function
    emitMainWrapper(sirModule);
}

/**
 * ╪Ñ╪╢╪º┘ב╪⌐ ╪»╪º┘ה╪⌐ main ┘ד┘א wrapper
 * Emit main wrapper function
 */
void LLVMCodeGen::emitMainWrapper(std::shared_ptr<SIRModule> sirModule) {
    if (!sirModule || !module_) return;
    
    // (AR) البحث عن الدالة الرئيسية
    // (EN) Find the main entry point function
    // Priority: 1) "رئيسية"  2) "main"  3) "__sad_main"
    llvm::Function* mainFunc = nullptr;
    std::string mainName;
    
    // (AR) أولا: البحث عن دالة "رئيسية" (UTF-8 hex escape)
    // (EN) First: look for Arabic "رئيسية" function
    std::string arabicMain = "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9";
    auto it = context_info_.functions.find(arabicMain);
    if (it != context_info_.functions.end()) {
        mainFunc = it->second;
        mainName = arabicMain;
    }
    
    // (AR) ثانيا: البحث عن "main"
    // (EN) Second: look for "main"
    if (!mainFunc) {
        it = context_info_.functions.find("main");
        if (it != context_info_.functions.end()) {
            mainFunc = it->second;
            mainName = "main";
        }
    }
    
    // (AR) ثالثا: البحث عن "__sad_main" (الكود التنفيذي في المستوى الأعلى)
    // (EN) Third: look for "__sad_main" (top-level executable code wrapper)
    if (!mainFunc) {
        it = context_info_.functions.find("__sad_main");
        if (it != context_info_.functions.end()) {
            mainFunc = it->second;
            mainName = "__sad_main";
        }
    }
    
    // (AR) إذا لم نجد أي دالة رئيسية لا حاجة لـ wrapper
    // (EN) If no main function found, no wrapper needed
    if (!mainFunc) return;
    
    // (AR) إذا كان اسم الدالة بالفعل "main" لا نحتاج wrapper
    // (EN) If function is already named "main", no wrapper needed
    if (mainName == "main") return;
    
    // (AR) إنشاء دالة main wrapper: int main() { call mainFunc(); return 0; }
    // (EN) Create main wrapper function
    llvm::FunctionType* mainType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context_),  // int return type
        {},                                  // no parameters
        false                                // not vararg
    );
    
    llvm::Function* wrapper = llvm::Function::Create(
        mainType,
        llvm::Function::ExternalLinkage,
        "main",
        module_.get()
    );
    
    // (AR) إنشاء basic block
    // (EN) Create entry basic block
    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(*context_, "entry", wrapper);
    builder_->SetInsertPoint(entryBB);
    
    // (AR) استدعاء الدالة الرئيسية
    // (EN) Call the main function
    llvm::Value* result = builder_->CreateCall(mainFunc);
    
    // (AR) تحويل النتيجة إلى i32 إذا لزم الأمر
    // (EN) Convert result to i32 if needed
    if (result->getType()->isIntegerTy(64)) {
        result = builder_->CreateTrunc(result, llvm::Type::getInt32Ty(*context_));
    } else if (result->getType()->isVoidTy()) {
        result = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
    }
    
    // (AR) إرجاع النتيجة
    // (EN) Return the result
    builder_->CreateRet(result);
}

/**
 * ╪º┘ה╪¬╪¡┘ג┘ג ┘ו┘ז ╪╡╪¡╪⌐ ╪º┘ה┘ט╪¡╪»╪⌐
 * Verify module correctness
 * 
 * Source: llvm_codegen.h:257
 * @return true ╪Ñ╪░╪º ┘ד╪º┘ז╪¬ ╪º┘ה┘ט╪¡╪»╪⌐ ╪╡╪¡┘ך╪¡╪⌐ / true if module is valid
 */
bool LLVMCodeGen::verify() const {
    // Source: module_ is defined at llvm_codegen.h:634
    if (!module_) {
        std::cerr << "LLVM CodeGen Error: Module is null in verify()\n";
        return false;
    }
    
    std::string errorMsg;
    llvm::raw_string_ostream errorStream(errorMsg);
    
    if (llvm::verifyModule(*module_, &errorStream)) {
        std::cerr << "Module verification failed:\n" << errorStream.str() << std::endl;
        return false;
    }
    
    return true;
}

/**
 * ╪¬╪╖╪¿┘ך┘ג ╪º┘ה╪¬╪¡╪│┘ך┘ז╪º╪¬ ╪╣┘ה┘י ╪º┘ה┘ט╪¡╪»╪⌐
 * Apply optimizations to module
 * 
 * Source: llvm_codegen.h:294
 * @return true ╪Ñ╪░╪º ┘ז╪¼╪¡ ╪º┘ה╪¬╪¡╪│┘ך┘ז / true if optimization succeeded
 */
bool LLVMCodeGen::optimize() {
    // Source: optimizer_ is defined at llvm_codegen.h:657
    if (!optimizer_) {
        reportError("Optimizer not initialized");
        return false;
    }
    
    // Source: module_ is defined at llvm_codegen.h:634
    if (!module_) {
        reportError("Module not initialized");
        return false;
    }
    
    // Source: llvm_optimizer.h:123 - bool optimize(llvm::Module* module)
    return optimizer_->optimize(module_.get());
}

/**
 * ╪¬╪╣┘ך┘ך┘ז ┘ו╪│╪¬┘ט┘י ╪º┘ה╪¬╪¡╪│┘ך┘ז
 * Set optimization level
 * 
 * Source: llvm_codegen.h:266
 * @param level ┘ו╪│╪¬┘ט┘י ╪º┘ה╪¬╪¡╪│┘ך┘ז / Optimization level
 */
void LLVMCodeGen::setOptimizationLevel(sad::OptimizationLevel level) {
    // Source: optimizationLevel_ is defined at llvm_codegen.h:660
    optimizationLevel_ = level;
    
    // Source: optimizer_ is defined at llvm_codegen.h:657
    if (optimizer_) {
        optimizer_->setOptimizationLevel(level);
    }
}

/**
 * ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ╪Ñ╪¡╪╡╪º╪ª┘ך╪º╪¬ ╪º┘ה╪¬╪¡╪│┘ך┘ז
 * Get optimization statistics
 * 
 * Source: llvm_codegen.h:305
 * @return ╪Ñ╪¡╪╡╪º╪ª┘ך╪º╪¬ ╪º┘ה╪¬╪¡╪│┘ך┘ז / Optimization statistics
 */
const sad::OptimizationStats& LLVMCodeGen::getOptimizationStats() const {
    // Source: optimizer_ is defined at llvm_codegen.h:657
    static sad::OptimizationStats emptyStats;
    
    if (!optimizer_) {
        return emptyStats;
    }
    
    return optimizer_->getStats();
}

/**
 * ╪╖╪¿╪º╪╣╪⌐ ╪Ñ╪¡╪╡╪º╪ª┘ך╪º╪¬ ╪º┘ה╪¬╪¡╪│┘ך┘ז
 * Print optimization statistics
 * 
 * Source: llvm_codegen.h:313
 */
void LLVMCodeGen::printOptimizationStats() const {
    // Source: optimizer_ is defined at llvm_codegen.h:657
    if (optimizer_) {
        optimizer_->printStats();
    }
}

// ============================================================================
// ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 3: ╪Ñ╪╡╪»╪º╪▒ ╪º┘ה╪»┘ט╪º┘ה
// Phase 3: Function Emission
// ============================================================================
// Note: getModule() is defined inline at llvm_codegen.h:211 - ┘ה╪º ╪¡╪º╪¼╪⌐ ┘ה╪¬┘ז┘ב┘ך╪░┘ח╪º ┘ח┘ז╪º

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪»╪º┘ה╪⌐ LLVM ┘ו┘ז ╪»╪º┘ה╪⌐ SIR
 * Emit LLVM function from SIR function
 * 
 * Source: llvm_codegen.h:347
 * @param sirFunc ╪»╪º┘ה╪⌐ SIR / SIR function
 * @return ╪»╪º┘ה╪⌐ LLVM / LLVM function
 */
llvm::Function* LLVMCodeGen::emitFunction(std::shared_ptr<SIRFunction> sirFunc) {
    if (!sirFunc) {
        reportError("SIR function is null in emitFunction");
        return nullptr;
    }
    
    // ╪Ñ┘ז╪┤╪º╪í ╪¬┘ט┘ג┘ך╪╣ ╪º┘ה╪»╪º┘ה╪⌐
    // Create function prototype
    llvm::Function* llvmFunc = emitFunctionPrototype(sirFunc);
    
    if (!llvmFunc) {
        return nullptr;
    }
    
    // ╪Ñ╪╡╪»╪º╪▒ ╪¼╪│┘ו ╪º┘ה╪»╪º┘ה╪⌐
    // Emit function body
    emitFunctionBody(sirFunc, llvmFunc);
    
    return llvmFunc;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬┘ט┘ג┘ך╪╣ ╪º┘ה╪»╪º┘ה╪⌐ ┘ב┘ג╪╖ (╪¿╪»┘ט┘ז ╪¼╪│┘ו)
 * Emit function signature only (without body)
 * 
 * Source: llvm_codegen.h:368
 * @param sirFunc ╪»╪º┘ה╪⌐ SIR / SIR function
 * @return ╪»╪º┘ה╪⌐ LLVM / LLVM function
 */
llvm::Function* LLVMCodeGen::emitFunctionPrototype(std::shared_ptr<SIRFunction> sirFunc) {
    if (!sirFunc) {
        reportError("SIR function is null in emitFunctionPrototype");
        return nullptr;
    }
    
    // Source: SIRFunction::getName() is at sir_module.h:306
    std::string funcName = sirFunc->getName();
    
    // Source: SIRFunction::returnType is PUBLIC member at sir_module.h:251
    SIRType returnSIRType = sirFunc->returnType;
    
    // ╪¬╪¡┘ט┘ך┘ה ┘ז┘ט╪╣ ╪º┘ה╪Ñ╪▒╪¼╪º╪╣
    // Convert return type
    llvm::Type* returnType = nullptr;
    switch (returnSIRType) {
        case SIRType::VOID:
            returnType = getVoidType();
            break;
        case SIRType::I64:
            returnType = getInt64Type();
            break;
        case SIRType::F64:
            returnType = getDoubleType();
            break;
        case SIRType::BOOL:
            returnType = getInt1Type();
            break;
        case SIRType::PTR:
            returnType = getInt8PtrType();
            break;
        case SIRType::STRING:
            returnType = getInt8PtrType();
            break;
        default:
            returnType = getVoidType();
            break;
    }
    
    // ╪¬╪¡┘ט┘ך┘ה ╪ú┘ז┘ט╪º╪╣ ╪º┘ה┘ו╪╣╪º┘ו┘ה╪º╪¬
    // Convert parameter types
    std::vector<llvm::Type*> paramTypes;
    
    // Source: SIRFunction::getParameters() is at sir_module.h:293
    const auto& params = sirFunc->getParameters();
    
    for (const auto& param : params) {
        // Source: SIRParameter::type is PUBLIC member at sir_module.h:230
        llvm::Type* paramType = nullptr;
        
        switch (param.type) {
            case SIRType::I64:
                paramType = getInt64Type();
                break;
            case SIRType::F64:
                paramType = getDoubleType();
                break;
            case SIRType::BOOL:
                paramType = getInt1Type();
                break;
            case SIRType::PTR:
                paramType = getInt8PtrType();
                break;
            case SIRType::STRING:
                paramType = getInt8PtrType();
                break;
            default:
                paramType = getInt64Type();
                break;
        }
        
        paramTypes.push_back(paramType);
    }
    
    // ╪Ñ┘ז╪┤╪º╪í ┘ז┘ט╪╣ ╪º┘ה╪»╪º┘ה╪⌐
    // Create function type
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        returnType,
        paramTypes,
        false  // isVarArg
    );
    
    // ╪Ñ┘ז╪┤╪º╪í ╪º┘ה╪»╪º┘ה╪⌐
    // Create function
    // Source: module_ is defined at llvm_codegen.h:634
    llvm::Function* llvmFunc = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        funcName,
        module_.get()
    );
    
    // ╪¬╪│┘ו┘ך╪⌐ ╪º┘ה┘ו╪╣╪º┘ו┘ה╪º╪¬
    // Name parameters
    unsigned idx = 0;
    for (auto& arg : llvmFunc->args()) {
        if (idx < params.size()) {
            // Source: SIRParameter::name is PUBLIC member at sir_module.h:229
            arg.setName(params[idx].name);
        }
        idx++;
    }
    
    return llvmFunc;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¼╪│┘ו ╪º┘ה╪»╪º┘ה╪⌐
 * Emit function body
 * 
 * Source: llvm_codegen.h:356
 * @param sirFunc ╪»╪º┘ה╪⌐ SIR / SIR function
 * @param llvmFunc ╪»╪º┘ה╪⌐ LLVM / LLVM function
 */
void LLVMCodeGen::emitFunctionBody(std::shared_ptr<SIRFunction> sirFunc, llvm::Function* llvmFunc) {
    if (!sirFunc || !llvmFunc) {
        reportError("Null function in emitFunctionBody");
        return;
    }
    
    // ╪¡┘ב╪╕ ╪º┘ה╪»╪º┘ה╪⌐ ╪º┘ה╪¡╪º┘ה┘ך╪⌐ ┘ב┘ך ╪º┘ה╪│┘ך╪º┘ג
    // Save current function in context
    // Source: context_info_ is defined at llvm_codegen.h:643
    // Source: CodeGenContext::currentFunction is at llvm_codegen.h:615
    context_info_.currentFunction = llvmFunc;
    
    // ┘ו╪│╪¡ ╪º┘ה┘ג┘ך┘ו ╪º┘ה┘ו╪│┘ו╪º╪⌐ ┘ה┘ה╪»╪º┘ה╪⌐ ╪º┘ה╪¼╪»┘ך╪»╪⌐
    // Clear named values for new function
    // Source: CodeGenContext::namedValues is at llvm_codegen.h:617
    context_info_.namedValues.clear();
    
    // ================================================================
    // ┘ד╪┤┘ב ╪º┘ה╪¿╪º┘ז┘ך: ╪Ñ╪░╪º ┘ד╪º┘ז ╪º╪│┘ו ╪º┘ה╪»╪º┘ה╪⌐ ┘ך╪¡╪¬┘ט┘ך ".╪¿┘ז╪º╪í"
    // Detect constructor: if function name contains ".╪¿┘ז╪º╪í"
    // ================================================================
    context_info_.currentConstructorClass.clear();
    context_info_.currentMethodClass.clear();
    std::string funcName = sirFunc->getName();
    // بناء = \xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1
    std::string ctorSuffix = ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1";
    size_t ctorPos = funcName.find(ctorSuffix);
    if (ctorPos != std::string::npos) {
        context_info_.currentConstructorClass = funcName.substr(0, ctorPos);
        std::cout << "[DEBUG] emitFunctionBody: detected constructor for class '"
                  << context_info_.currentConstructorClass << "'" << std::endl;
    } else {
        // ================================================================
        // كشف الدوال (Methods): إذا كان الاسم يحتوي على "." والبادئة صنف معروف
        // Detect methods: if name contains "." and prefix is a known class
        // ================================================================
        size_t dotPos = funcName.find('.');
        if (dotPos != std::string::npos) {
            std::string prefix = funcName.substr(0, dotPos);
            if (context_info_.classStructTypes.find(prefix) != context_info_.classStructTypes.end()) {
                context_info_.currentMethodClass = prefix;
                std::cout << "[DEBUG] emitFunctionBody: detected method for class '"
                          << context_info_.currentMethodClass << "'" << std::endl;
            }
        }
    }
    
    // Source: SIRFunction::getBasicBlocks() is at sir_module.h:299
    const auto& basicBlocks = sirFunc->getBasicBlocks();
    
    if (basicBlocks.empty()) {
        // ╪»╪º┘ה╪⌐ ╪¿╪»┘ט┘ז ╪¼╪│┘ו (╪¬╪╡╪▒┘ך╪¡ ┘ב┘ג╪╖)
        // Function without body (declaration only)
        return;
    }
    
    // ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 1: ╪Ñ┘ז╪┤╪º╪í ╪¼┘ו┘ך╪╣ ╪º┘ה┘ד╪¬┘ה ╪º┘ה╪ú╪│╪º╪│┘ך╪⌐ ╪ú┘ט┘ה╪º┘כ (┘ג╪¿┘ה ╪º┘ה┘ו╪╣╪º┘ו┘ה╪º╪¬)
    // Phase 1: Create all basic blocks FIRST (before parameters)
    // Source: context_info_.basicBlocks is at llvm_codegen.h:619
    context_info_.basicBlocks.clear();
    
    // ========================================================================
    // FIX: Pre-scan all instructions to discover referenced labels
    // This fixes the bug where buildIfStatement creates blocks that aren't
    // registered in SIRFunction (currentFunction_ issue)
    // ========================================================================
    std::set<std::string> allLabels;
    for (const auto& sirBlock : basicBlocks) {
        if (!sirBlock) continue;
        allLabels.insert(sirBlock->name);
        // Scan instructions for BR_COND and BR labels
        for (const auto& inst : sirBlock->instructions) {
            if (inst.opcode == SIROpcode::BR_COND) {
                // BR_COND has: condition, trueLabel, falseLabel
                if (inst.operands.size() >= 3) {
                    if (inst.operands[1].type == SIROperandType::LABEL) {
                        allLabels.insert(inst.operands[1].name);
                    }
                    if (inst.operands[2].type == SIROperandType::LABEL) {
                        allLabels.insert(inst.operands[2].name);
                    }
                }
            } else if (inst.opcode == SIROpcode::BR) {
                // BR has: targetLabel
                if (!inst.operands.empty() && inst.operands[0].type == SIROperandType::LABEL) {
                    allLabels.insert(inst.operands[0].name);
                }
            }
        }
    }
    
    std::cout << "[DEBUG] emitFunctionBody: discovered " << allLabels.size() << " labels (including referenced)" << std::endl;
    
    // Create LLVM blocks for ALL discovered labels
    // FIX: Create "entry" block first so it's the LLVM function entry point
    if (allLabels.count("entry")) {
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(
            *context_, "entry", llvmFunc);
        context_info_.basicBlocks["entry"] = entryBlock;
    }
    for (const auto& labelName : allLabels) {
        if (labelName == "entry") continue;  // already created
        std::cout << "[DEBUG] emitFunctionBody: creating block '" << labelName << "'" << std::endl;
        
        llvm::BasicBlock* llvmBlock = llvm::BasicBlock::Create(
            *context_,  // Source: context_ is at llvm_codegen.h:631
            labelName,
            llvmFunc
        );
        
        context_info_.basicBlocks[labelName] = llvmBlock;
    }
    
    // Phase 2: Add parameters to named values (after blocks are created)
    emitFunctionParameters(sirFunc, llvmFunc);
    
    // DEBUG: Dump SIR blocks and instructions
    std::cout << "\n[SIR-DUMP] Function: " << sirFunc->getName() << std::endl;
    for (const auto& sirBlock : basicBlocks) {
        if (!sirBlock) continue;
        std::cout << "[SIR-DUMP]   Block '" << sirBlock->name << "' addr=" << (void*)sirBlock.get()
                  << " has " << sirBlock->instructions.size() << " instructions:" << std::endl;
        for (size_t i = 0; i < sirBlock->instructions.size(); i++) {
            const auto& inst = sirBlock->instructions[i];
            std::cout << "[SIR-DUMP]     [" << i << "] opcode=" << static_cast<int>(inst.opcode);
            if (inst.result.has_value()) {
                std::cout << " result=" << inst.result->name;
            }
            for (size_t j = 0; j < inst.operands.size(); j++) {
                std::cout << " op" << j << "=(" << static_cast<int>(inst.operands[j].type) 
                          << ":" << inst.operands[j].name << ")";
            }
            std::cout << std::endl;
        }
    }
    std::cout << "[SIR-DUMP] End function\n" << std::endl;
    
    // Phase 2: Emit instructions for each block
    for (const auto& sirBlock : basicBlocks) {
        if (!sirBlock) continue;
        
        // Source: SIRBasicBlock::name is PUBLIC member at sir_instruction.h:355
        std::string blockName = sirBlock->name;
        
        auto it = context_info_.basicBlocks.find(blockName);
        if (it == context_info_.basicBlocks.end()) {
            reportError("Basic block not found: " + blockName);
            continue;
        }
        
        llvm::BasicBlock* llvmBlock = it->second;
        
        // ╪¬╪╣┘ך┘ך┘ז ┘ז┘ג╪╖╪⌐ ╪º┘ה╪Ñ╪»╪▒╪º╪¼
        // Set insertion point
        // Source: builder_ is defined at llvm_codegen.h:637
        builder_->SetInsertPoint(llvmBlock);
        
        // Source: CodeGenContext::currentBlock is at llvm_codegen.h:616
        context_info_.currentBlock = llvmBlock;
        
        // ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪º╪¬ ╪º┘ה┘ד╪¬┘ה╪⌐
        // Emit block instructions
        // Source: SIRBasicBlock::instructions is PUBLIC member at sir_instruction.h:356
        for (const auto& inst : sirBlock->instructions) {
            // ╪¬╪¡┘ט┘ך┘ה const SIRInstruction& ╪Ñ┘ה┘י shared_ptr
            // Convert const SIRInstruction& to shared_ptr
            auto instPtr = std::make_shared<SIRInstruction>(inst);
            emitInstruction(instPtr);
        }
    }
    
    // ========================================================================
    // Phase 3: Ensure ALL basic blocks have terminators
    // FIX: Some blocks (then_X, merge_X) may be empty if the SIR builder
    // didn't generate instructions for them. Add appropriate terminators.
    // ========================================================================
    for (auto& [blockName, llvmBlock] : context_info_.basicBlocks) {
        if (!llvmBlock->getTerminator()) {
            builder_->SetInsertPoint(llvmBlock);
            // If function returns void, add ret void
            if (llvmFunc->getReturnType()->isVoidTy()) {
                builder_->CreateRetVoid();
            } else {
                // Return default value for non-void functions
                builder_->CreateRet(llvm::Constant::getNullValue(llvmFunc->getReturnType()));
            }
        }
    }
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ┘ו╪╣╪º┘ו┘ה╪º╪¬ ╪º┘ה╪»╪º┘ה╪⌐
 * Emit function parameters
 * 
 * Source: llvm_codegen.h:376
 * @param sirFunc ╪»╪º┘ה╪⌐ SIR / SIR function
 * @param llvmFunc ╪»╪º┘ה╪⌐ LLVM / LLVM function
 */


} // namespace LLVM
} // namespace Sad
