/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 * 
 * هذا الملف يحتوي على تنفيذ مولد كود LLVM IR الذي يترجم SIR إلى LLVM IR
 * This file contains the implementation of LLVM IR code generator that
 * translates SIR to LLVM IR
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#include "llvm_codegen.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <iostream>
#include <fstream>

namespace Sad {
namespace LLVM {

// ============================================================================
// Constructor & Destructor / المنشئ والمدمر
// ============================================================================

/**
 * المنشئ - تهيئة مولد الكود
 * Constructor - Initialize code generator
 */
LLVMCodeGen::LLVMCodeGen()
    : context_(nullptr)
    , module_(nullptr)
    , builder_(nullptr)
    , targetMachine_(nullptr)
    , typeMapper_(nullptr)           // تهيئة محول الأنواع / Initialize type mapper
    , controlFlow_(nullptr)          // تهيئة مدير التحكم / Initialize control flow
    , hasErrors_(false)
{
    // تهيئة أهداف LLVM / Initialize LLVM targets
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
}

/**
 * المدمر - تنظيف الموارد
 * Destructor - Clean up resources
 */
LLVMCodeGen::~LLVMCodeGen() {
    // LLVM unique_ptr will handle cleanup automatically
    // سيتم التنظيف تلقائياً عبر unique_ptr
}

// ============================================================================
// Initialization / التهيئة
// ============================================================================

/**
 * تهيئة مولد الكود مع هدف محدد
 * Initialize code generator with specific target
 */
bool LLVMCodeGen::initialize(const std::string& moduleName, const std::string& targetTriple) {
    try {
        // إنشاء سياق LLVM جديد / Create new LLVM context
        context_ = std::make_unique<llvm::LLVMContext>();
        
        // إنشاء وحدة جديدة / Create new module
        module_ = std::make_unique<llvm::Module>(moduleName, *context_);
        
        // تعيين سلسلة الهدف / Set target triple
        module_->setTargetTriple(targetTriple);
        
        // إنشاء البناء / Create builder
        builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);
        
        // تهيئة الآلة الهدف / Initialize target machine
        std::string error;
        auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
        
        if (!target) {
            reportError("Failed to lookup target: " + error);
            return false;
        }
        
        llvm::TargetOptions opt;
        targetMachine_ = target->createTargetMachine(
            targetTriple,
            "generic",
            "",
            opt,
            llvm::Optional<llvm::Reloc::Model>()
        );
        
        if (!targetMachine_) {
            reportError("Failed to create target machine");
            return false;
        }
        
        // تعيين data layout
        module_->setDataLayout(targetMachine_->createDataLayout());
        
        // إنشاء محول الأنواع / Create type mapper
        typeMapper_ = std::make_unique<LLVMTypeMapper>(*context_);
        
        // إنشاء مدير الذاكرة / Create memory manager
        memoryManager_ = std::make_unique<LLVMMemoryManager>(*context_, *builder_, typeMapper_.get());
        
        // إنشاء بناء التعابير مع مدير الذاكرة / Create expression builder with memory manager
        expressionBuilder_ = std::make_unique<LLVMExpressionBuilder>(*context_, *builder_, typeMapper_.get(), memoryManager_.get());
        
        // إنشاء مدير تدفق التحكم / Create control flow manager
        // سيتم إنشاؤه عند توليد كل دالة / Will be created per function
        controlFlow_ = nullptr;
        
        return true;
    }
    catch (const std::exception& e) {
        reportError(std::string("Initialization failed: ") + e.what());
        return false;
    }
}

/**
 * تهيئة مولد الكود بدون هدف محدد (استخدام الهدف الافتراضي)
 * Initialize code generator without specific target (use default target)
 */
bool LLVMCodeGen::initialize(const std::string& moduleName) {
    // استخدام هدف النظام الحالي / Use current system target
    std::string targetTriple = llvm::sys::getDefaultTargetTriple();
    return initialize(moduleName, targetTriple);
}

// ============================================================================
// Main Generation / التوليد الرئيسي
// ============================================================================

/**
 * توليد LLVM IR من وحدة SIR
 * Generate LLVM IR from SIR module
 */
std::unique_ptr<llvm::Module> LLVMCodeGen::generate(std::shared_ptr<SIRModule> sirModule) {
    if (!sirModule) {
        reportError("SIR module is null");
        return nullptr;
    }
    
    // إصدار الوحدة / Emit module
    emitModule(sirModule);
    
    // التحقق من الوحدة / Verify module
    if (!verify()) {
        reportError("Module verification failed");
        return nullptr;
    }
    
    // إرجاع الوحدة / Return module
    return std::move(module_);
}

/**
 * التحقق من صحة الوحدة
 * Verify module correctness
 */
bool LLVMCodeGen::verify() const {
    if (!module_) {
        return false;
    }
    
    std::string error;
    llvm::raw_string_ostream errorStream(error);
    
    if (llvm::verifyModule(*module_, &errorStream)) {
        std::cerr << "خطأ في التحقق من الوحدة / Module verification error:\n";
        std::cerr << error << std::endl;
        return false;
    }
    
    return true;
}

// ============================================================================
// Module Emission / إصدار الوحدة
// ============================================================================

/**
 * إصدار وحدة SIR كاملة
 * Emit complete SIR module
 */
void LLVMCodeGen::emitModule(std::shared_ptr<SIRModule> sirModule) {
    // إصدار المتغيرات العامة / Emit global variables
    emitGlobalVariables(sirModule);
    
    // إصدار الثوابت / Emit constants
    emitConstants(sirModule);
    
    // إصدار الدوال / Emit functions
    emitGlobalFunctions(sirModule);
}

/**
 * إصدار الدوال العامة
 * Emit global functions
 */
void LLVMCodeGen::emitGlobalFunctions(std::shared_ptr<SIRModule> sirModule) {
    // المرور الأول: إنشاء توقيعات الدوال / First pass: Create function prototypes
    for (const auto& func : sirModule->getFunctions()) {
        emitFunctionPrototype(func);
    }
    
    // المرور الثاني: إصدار أجسام الدوال / Second pass: Emit function bodies
    for (const auto& func : sirModule->getFunctions()) {
        if (auto llvmFunc = context_info_.functions[func->getName()]) {
            emitFunctionBody(func, llvmFunc);
        }
    }
}

/**
 * إصدار المتغيرات العامة
 * Emit global variables
 */
void LLVMCodeGen::emitGlobalVariables(std::shared_ptr<SIRModule> sirModule) {
    for (const auto& globalVar : sirModule->getGlobalVariables()) {
        // تحويل النوع / Convert type
        llvm::Type* type = convertType(globalVar->getType());
        
        // إنشاء متغير عام / Create global variable
        llvm::Constant* initializer = nullptr;
        if (globalVar->hasInitializer()) {
            // ✅ TODO 1: تحويل المُهيئ إلى LLVM constant / Convert initializer to LLVM constant
            auto initValue = globalVar->getInitializer();
            
            // تحويل حسب النوع / Convert based on type
            if (initValue->isInteger()) {
                int64_t val = initValue->getIntValue();
                initializer = llvm::ConstantInt::get(type, val, true);
            } else if (initValue->isFloat()) {
                double val = initValue->getFloatValue();
                initializer = llvm::ConstantFP::get(type, val);
            } else if (initValue->isBoolean()) {
                bool val = initValue->getBoolValue();
                initializer = llvm::ConstantInt::get(type, val ? 1 : 0);
            } else if (initValue->isString()) {
                // نص ثابت / Constant string
                std::string str = initValue->getStringValue();
                initializer = llvm::ConstantDataArray::getString(*context_, str, true);
            } else if (initValue->isNull()) {
                initializer = llvm::Constant::getNullValue(type);
            } else {
                // قيمة افتراضية / Default value
                initializer = llvm::Constant::getNullValue(type);
            }
        } else {
            initializer = llvm::Constant::getNullValue(type);
        }
        
        auto globalLLVM = new llvm::GlobalVariable(
            *module_,
            type,
            false, // isConstant
            llvm::GlobalValue::ExternalLinkage,
            initializer,
            globalVar->getName()
        );
        
        // حفظ في السياق / Save in context
        context_info_.namedValues[globalVar->getName()] = globalLLVM;
    }
}

/**
 * إصدار الثوابت
 * Emit constants
 */
void LLVMCodeGen::emitConstants(std::shared_ptr<SIRModule> sirModule) {
    for (const auto& constant : sirModule->getConstants()) {
        llvm::Constant* llvmConst = nullptr;
        
        // ✅ TODO 2: تحويل SIR constant إلى LLVM constant / Convert SIR constant to LLVM constant
        auto constType = constant->getType();
        
        if (constType == SIR::SIRType::I64) {
            // ثابت عدد صحيح / Integer constant
            int64_t val = constant->getIntValue();
            llvmConst = llvm::ConstantInt::get(getInt64Type(), val, true);
        } else if (constType == SIR::SIRType::F64) {
            // ثابت عدد عشري / Float constant
            double val = constant->getFloatValue();
            llvmConst = llvm::ConstantFP::get(getDoubleType(), val);
        } else if (constType == SIR::SIRType::BOOL) {
            // ثابت منطقي / Boolean constant
            bool val = constant->getBoolValue();
            llvmConst = llvm::ConstantInt::get(getInt1Type(), val ? 1 : 0);
        } else if (constType == SIR::SIRType::STRING) {
            // ثابت نصي / String constant
            std::string str = constant->getStringValue();
            llvmConst = llvm::ConstantDataArray::getString(*context_, str, true);
        } else if (constType == SIR::SIRType::PTR) {
            // مؤشر null / Null pointer
            llvm::Type* ptrType = convertType(constant->getType());
            llvmConst = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrType));
        } else if (constType == SIR::SIRType::ARRAY) {
            // مصفوفة ثابتة / Constant array
            auto elements = constant->getArrayElements();
            std::vector<llvm::Constant*> llvmElements;
            for (const auto& elem : elements) {
                // تحويل كل عنصر / Convert each element
                llvmElements.push_back(convertConstant(elem));
            }
            llvm::ArrayType* arrayType = llvm::ArrayType::get(
                llvmElements[0]->getType(), 
                llvmElements.size()
            );
            llvmConst = llvm::ConstantArray::get(arrayType, llvmElements);
        } else {
            // قيمة افتراضية / Default value
            llvmConst = llvm::Constant::getNullValue(convertType(constType));
        }
        
        if (llvmConst) {
            context_info_.constants[constant->getName()] = llvmConst;
        }
    }
}

// ============================================================================
// Function Emission / إصدار الدوال
// ============================================================================

/**
 * إصدار دالة LLVM من دالة SIR
 * Emit LLVM function from SIR function
 */
llvm::Function* LLVMCodeGen::emitFunction(std::shared_ptr<SIRFunction> sirFunc) {
    // إنشاء التوقيع أولاً / Create prototype first
    llvm::Function* llvmFunc = emitFunctionPrototype(sirFunc);
    
    // ثم إصدار الجسم / Then emit body
    emitFunctionBody(sirFunc, llvmFunc);
    
    return llvmFunc;
}

/**
 * إصدار توقيع الدالة فقط
 * Emit function signature only
 */
llvm::Function* LLVMCodeGen::emitFunctionPrototype(std::shared_ptr<SIRFunction> sirFunc) {
    // تحويل أنواع المعاملات / Convert parameter types
    std::vector<llvm::Type*> paramTypes;
    for (const auto& param : sirFunc->getParameters()) {
        paramTypes.push_back(convertType(param->getType()));
    }
    
    // تحويل نوع الرجوع / Convert return type
    llvm::Type* returnType = convertType(sirFunc->getReturnType());
    
    // إنشاء نوع الدالة / Create function type
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        returnType,
        paramTypes,
        sirFunc->isVarArg()
    );
    
    // إنشاء الدالة / Create function
    llvm::Function* llvmFunc = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        sirFunc->getName(),
        module_.get()
    );
    
    // تعيين أسماء المعاملات / Set parameter names
    size_t idx = 0;
    for (auto& arg : llvmFunc->args()) {
        arg.setName(sirFunc->getParameters()[idx++]->getName());
    }
    
    // حفظ في السياق / Save in context
    context_info_.functions[sirFunc->getName()] = llvmFunc;
    
    return llvmFunc;
}

/**
 * إصدار جسم الدالة
 * Emit function body
 */
void LLVMCodeGen::emitFunctionBody(std::shared_ptr<SIRFunction> sirFunc, llvm::Function* llvmFunc) {
    // إنشاء كتلة entry / Create entry block
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(
        *context_,
        "entry",
        llvmFunc
    );
    
    builder_->SetInsertPoint(entryBlock);  // تعيين نقطة الإدراج / Set insert point
    context_info_.currentFunction = llvmFunc;  // تعيين الدالة الحالية / Set current function
    context_info_.currentBlock = entryBlock;   // تعيين الكتلة الحالية / Set current block
    
    // إنشاء مدير تدفق التحكم للدالة / Create control flow manager for function
    controlFlow_ = std::make_unique<LLVMControlFlow>(*context_, *builder_, llvmFunc);
    
    // تخصيص معاملات الدالة / Allocate function parameters
    emitFunctionParameters(sirFunc, llvmFunc);
    
    // إصدار الكتل الأساسية / Emit basic blocks
    for (const auto& bb : sirFunc->getBasicBlocks()) {
        if (bb->getName() != "entry") { // تخطي entry لأنه موجود / Skip entry as it exists
            emitBasicBlock(bb, llvmFunc);
        }
    }
    
    // إصدار تعليمات كل كتلة / Emit instructions for each block
    for (const auto& bb : sirFunc->getBasicBlocks()) {
        // الحصول على الكتلة المقابلة / Get corresponding block
        llvm::BasicBlock* llvmBlock;
        if (bb->getName() == "entry") {
            llvmBlock = entryBlock;  // استخدام كتلة entry الموجودة / Use existing entry block
        } else {
            llvmBlock = context_info_.basicBlocks[bb->getName()];  // الحصول من الجدول / Get from table
        }
        
        builder_->SetInsertPoint(llvmBlock);  // الانتقال إلى الكتلة / Move to block
        context_info_.currentBlock = llvmBlock;  // تحديث السياق / Update context
        
        // إصدار كل تعليمة / Emit each instruction
        for (const auto& inst : bb->getInstructions()) {
            emitInstruction(inst);  // توليد التعليمة / Generate instruction
        }
    }
    
    // تنظيف السياق / Clean up context
    context_info_.currentFunction = nullptr;
    context_info_.currentBlock = nullptr;
    context_info_.namedValues.clear();  // مسح جدول المتغيرات / Clear variables table
    context_info_.allocas.clear();      // مسح جدول التخصيصات / Clear allocations table
    
    // تنظيف مدير تدفق التحكم / Clean up control flow manager
    controlFlow_.reset();  // حذف مدير التحكم / Delete control flow manager
}

/**
 * إصدار معاملات الدالة
 * Emit function parameters
 */
void LLVMCodeGen::emitFunctionParameters(std::shared_ptr<SIRFunction> sirFunc, llvm::Function* llvmFunc) {
    size_t idx = 0;
    for (auto& arg : llvmFunc->args()) {
        // إنشاء alloca لكل معامل / Create alloca for each parameter
        llvm::AllocaInst* alloca = createEntryBlockAlloca(
            llvmFunc,
            arg.getName().str(),
            arg.getType()
        );
        
        // تخزين القيمة / Store value
        builder_->CreateStore(&arg, alloca);
        
        // حفظ في السياق / Save in context
        context_info_.namedValues[arg.getName().str()] = alloca;
        context_info_.allocas[arg.getName().str()] = alloca;
        
        idx++;
    }
}

// ============================================================================
// Basic Block Emission / إصدار الكتل الأساسية
// ============================================================================

/**
 * إصدار كتلة أساسية
 * Emit basic block
 */
llvm::BasicBlock* LLVMCodeGen::emitBasicBlock(std::shared_ptr<SIRBasicBlock> sirBlock,
                                              llvm::Function* function) {
    // إنشاء الكتلة / Create block
    llvm::BasicBlock* block = llvm::BasicBlock::Create(
        *context_,
        sirBlock->getName(),
        function
    );
    
    // حفظ في السياق / Save in context
    context_info_.basicBlocks[sirBlock->getName()] = block;
    
    return block;
}

/**
 * إنشاء كتلة أساسية فارغة
 * Create empty basic block
 */
llvm::BasicBlock* LLVMCodeGen::createBasicBlock(const std::string& name, llvm::Function* function) {
    return llvm::BasicBlock::Create(*context_, name, function);
}

// ============================================================================
// Instruction Emission / إصدار التعليمات
// ============================================================================

/**
 * إصدار تعليمة من SIR
 * Emit instruction from SIR
 */
llvm::Value* LLVMCodeGen::emitInstruction(std::shared_ptr<SIRInstruction> inst) {
    // تحديد نوع التعليمة وإصدارها / Determine instruction type and emit
    switch (inst->getOpcode()) {
        // Arithmetic / حسابية
        case SIROpcode::Add: return emitAdd(inst);
        case SIROpcode::Sub: return emitSub(inst);
        case SIROpcode::Mul: return emitMul(inst);
        case SIROpcode::Div: return emitDiv(inst);
        case SIROpcode::Mod: return emitMod(inst);
        case SIROpcode::Neg: return emitNeg(inst);
        
        // Bitwise / ثنائية
        case SIROpcode::And: return emitAnd(inst);
        case SIROpcode::Or: return emitOr(inst);
        case SIROpcode::Xor: return emitXor(inst);
        case SIROpcode::Not: return emitNot(inst);
        case SIROpcode::Shl: return emitShl(inst);
        case SIROpcode::Shr: return emitShr(inst);
        
        // Comparison / مقارنة
        case SIROpcode::CmpEq: return emitCmpEq(inst);
        case SIROpcode::CmpNe: return emitCmpNe(inst);
        case SIROpcode::CmpLt: return emitCmpLt(inst);
        case SIROpcode::CmpLe: return emitCmpLe(inst);
        case SIROpcode::CmpGt: return emitCmpGt(inst);
        case SIROpcode::CmpGe: return emitCmpGe(inst);
        
        // Memory / ذاكرة
        case SIROpcode::Load: return emitLoad(inst);
        case SIROpcode::Store: return emitStore(inst);
        case SIROpcode::Alloca: return emitAlloca(inst);
        case SIROpcode::GEP: return emitGEP(inst);
        
        // Control flow / تدفق التحكم
        case SIROpcode::Branch: return emitBranch(inst);
        case SIROpcode::CondBranch: return emitCondBranch(inst);
        case SIROpcode::Call: return emitCall(inst);
        case SIROpcode::Return: return emitReturn(inst);
        case SIROpcode::Switch: return emitSwitch(inst);
        
        // Type conversion / تحويل الأنواع
        case SIROpcode::Cast: return emitCast(inst);
        case SIROpcode::BitCast: return emitBitCast(inst);
        case SIROpcode::IntToPtr: return emitIntToPtr(inst);
        case SIROpcode::PtrToInt: return emitPtrToInt(inst);
        case SIROpcode::Trunc: return emitTrunc(inst);
        case SIROpcode::ZExt: return emitZExt(inst);
        case SIROpcode::SExt: return emitSExt(inst);
        
        // Aggregate / تجميع
        case SIROpcode::ExtractValue: return emitExtractValue(inst);
        case SIROpcode::InsertValue: return emitInsertValue(inst);
        case SIROpcode::ExtractElement: return emitExtractElement(inst);
        case SIROpcode::InsertElement: return emitInsertElement(inst);
        
        // Phi & Select / فاي واختيار
        case SIROpcode::Phi: return emitPhi(inst);
        case SIROpcode::Select: return emitSelect(inst);
        
        default:
            reportError("Unknown instruction opcode");
            return nullptr;
    }
}

// ============================================================================
// Arithmetic Instructions / التعليمات الحسابية
// ============================================================================

/**
 * جمع / Add
 */
llvm::Value* LLVMCodeGen::emitAdd(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    
    // التحقق من النوع (صحيح أم عشري) / Check type (integer or float)
    if (lhs->getType()->isIntegerTy()) {
        return builder_->CreateAdd(lhs, rhs, inst->getName());
    } else {
        return builder_->CreateFAdd(lhs, rhs, inst->getName());
    }
}

/**
 * طرح / Subtract
 */
llvm::Value* LLVMCodeGen::emitSub(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    
    if (lhs->getType()->isIntegerTy()) {
        return builder_->CreateSub(lhs, rhs, inst->getName());
    } else {
        return builder_->CreateFSub(lhs, rhs, inst->getName());
    }
}

/**
 * ضرب / Multiply
 */
llvm::Value* LLVMCodeGen::emitMul(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    
    if (lhs->getType()->isIntegerTy()) {
        return builder_->CreateMul(lhs, rhs, inst->getName());
    } else {
        return builder_->CreateFMul(lhs, rhs, inst->getName());
    }
}

/**
 * قسمة / Divide
 */
llvm::Value* LLVMCodeGen::emitDiv(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    
    if (lhs->getType()->isIntegerTy()) {
        // قسمة صحيحة مع إشارة / Signed integer division
        return builder_->CreateSDiv(lhs, rhs, inst->getName());
    } else {
        return builder_->CreateFDiv(lhs, rhs, inst->getName());
    }
}

/**
 * باقي القسمة / Modulo
 */
llvm::Value* LLVMCodeGen::emitMod(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    
    if (lhs->getType()->isIntegerTy()) {
        return builder_->CreateSRem(lhs, rhs, inst->getName());
    } else {
        return builder_->CreateFRem(lhs, rhs, inst->getName());
    }
}

/**
 * نفي / Negate
 */
llvm::Value* LLVMCodeGen::emitNeg(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* operand = getNamedValue(inst->getOperand(0)->getName());
    
    if (operand->getType()->isIntegerTy()) {
        return builder_->CreateNeg(operand, inst->getName());
    } else {
        return builder_->CreateFNeg(operand, inst->getName());
    }
}

// ============================================================================
// Bitwise Instructions / التعليمات الثنائية
// ============================================================================

/**
 * AND ثنائي / Bitwise AND
 */
llvm::Value* LLVMCodeGen::emitAnd(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    return builder_->CreateAnd(lhs, rhs, inst->getName());
}

/**
 * OR ثنائي / Bitwise OR
 */
llvm::Value* LLVMCodeGen::emitOr(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    return builder_->CreateOr(lhs, rhs, inst->getName());
}

/**
 * XOR ثنائي / Bitwise XOR
 */
llvm::Value* LLVMCodeGen::emitXor(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    return builder_->CreateXor(lhs, rhs, inst->getName());
}

/**
 * NOT ثنائي / Bitwise NOT
 */
llvm::Value* LLVMCodeGen::emitNot(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* operand = getNamedValue(inst->getOperand(0)->getName());
    return builder_->CreateNot(operand, inst->getName());
}

/**
 * إزاحة يسار / Shift left
 */
llvm::Value* LLVMCodeGen::emitShl(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    return builder_->CreateShl(lhs, rhs, inst->getName());
}

/**
 * إزاحة يمين / Shift right
 */
llvm::Value* LLVMCodeGen::emitShr(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    // إزاحة حسابية (مع الإشارة) / Arithmetic shift (with sign)
    return builder_->CreateAShr(lhs, rhs, inst->getName());
}

// ============================================================================
// Comparison Instructions / تعليمات المقارنة
// ============================================================================

/**
 * يساوي / Equal
 */
llvm::Value* LLVMCodeGen::emitCmpEq(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    
    if (lhs->getType()->isIntegerTy()) {
        return builder_->CreateICmpEQ(lhs, rhs, inst->getName());
    } else {
        return builder_->CreateFCmpOEQ(lhs, rhs, inst->getName());
    }
}

/**
 * لا يساوي / Not equal
 */
llvm::Value* LLVMCodeGen::emitCmpNe(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    
    if (lhs->getType()->isIntegerTy()) {
        return builder_->CreateICmpNE(lhs, rhs, inst->getName());
    } else {
        return builder_->CreateFCmpONE(lhs, rhs, inst->getName());
    }
}

/**
 * أصغر من / Less than
 */
llvm::Value* LLVMCodeGen::emitCmpLt(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    
    if (lhs->getType()->isIntegerTy()) {
        return builder_->CreateICmpSLT(lhs, rhs, inst->getName());
    } else {
        return builder_->CreateFCmpOLT(lhs, rhs, inst->getName());
    }
}

/**
 * أصغر أو يساوي / Less or equal
 */
llvm::Value* LLVMCodeGen::emitCmpLe(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    
    if (lhs->getType()->isIntegerTy()) {
        return builder_->CreateICmpSLE(lhs, rhs, inst->getName());
    } else {
        return builder_->CreateFCmpOLE(lhs, rhs, inst->getName());
    }
}

/**
 * أكبر من / Greater than
 */
llvm::Value* LLVMCodeGen::emitCmpGt(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    
    if (lhs->getType()->isIntegerTy()) {
        return builder_->CreateICmpSGT(lhs, rhs, inst->getName());
    } else {
        return builder_->CreateFCmpOGT(lhs, rhs, inst->getName());
    }
}

/**
 * أكبر أو يساوي / Greater or equal
 */
llvm::Value* LLVMCodeGen::emitCmpGe(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* lhs = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* rhs = getNamedValue(inst->getOperand(1)->getName());
    
    if (lhs->getType()->isIntegerTy()) {
        return builder_->CreateICmpSGE(lhs, rhs, inst->getName());
    } else {
        return builder_->CreateFCmpOGE(lhs, rhs, inst->getName());
    }
}

// ============================================================================
// Memory Instructions / تعليمات الذاكرة
// ============================================================================

/**
 * تحميل / Load
 */
llvm::Value* LLVMCodeGen::emitLoad(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* ptr = getNamedValue(inst->getOperand(0)->getName());
    return builder_->CreateLoad(ptr->getType()->getPointerElementType(), ptr, inst->getName());
}

/**
 * تخزين / Store
 */
llvm::Value* LLVMCodeGen::emitStore(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* value = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* ptr = getNamedValue(inst->getOperand(1)->getName());
    return builder_->CreateStore(value, ptr);
}

/**
 * تخصيص / Allocate
 */
llvm::Value* LLVMCodeGen::emitAlloca(std::shared_ptr<SIRInstruction> inst) {
    llvm::Type* type = convertType(inst->getType());
    return builder_->CreateAlloca(type, nullptr, inst->getName());
}

/**
 * Get Element Pointer
 */
llvm::Value* LLVMCodeGen::emitGEP(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* ptr = getNamedValue(inst->getOperand(0)->getName());
    
    std::vector<llvm::Value*> indices;
    for (size_t i = 1; i < inst->getNumOperands(); ++i) {
        indices.push_back(getNamedValue(inst->getOperand(i)->getName()));
    }
    
    return builder_->CreateGEP(
        ptr->getType()->getPointerElementType(),
        ptr,
        indices,
        inst->getName()
    );
}

// ============================================================================
// Control Flow Instructions / تعليمات تدفق التحكم
// ============================================================================

/**
 * فرع غير مشروط / Unconditional branch
 */
llvm::Value* LLVMCodeGen::emitBranch(std::shared_ptr<SIRInstruction> inst) {
    std::string targetLabel = inst->getOperand(0)->getName();
    llvm::BasicBlock* targetBlock = context_info_.basicBlocks[targetLabel];
    return builder_->CreateBr(targetBlock);
}

/**
 * فرع شرطي / Conditional branch
 */
llvm::Value* LLVMCodeGen::emitCondBranch(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* cond = getNamedValue(inst->getOperand(0)->getName());
    llvm::BasicBlock* thenBlock = context_info_.basicBlocks[inst->getOperand(1)->getName()];
    llvm::BasicBlock* elseBlock = context_info_.basicBlocks[inst->getOperand(2)->getName()];
    return builder_->CreateCondBr(cond, thenBlock, elseBlock);
}

/**
 * استدعاء دالة / Function call
 */
llvm::Value* LLVMCodeGen::emitCall(std::shared_ptr<SIRInstruction> inst) {
    std::string funcName = inst->getOperand(0)->getName();
    llvm::Function* func = context_info_.functions[funcName];
    
    std::vector<llvm::Value*> args;
    for (size_t i = 1; i < inst->getNumOperands(); ++i) {
        args.push_back(getNamedValue(inst->getOperand(i)->getName()));
    }
    
    return builder_->CreateCall(func, args, inst->getName());
}

/**
 * رجوع من دالة / Return
 */
llvm::Value* LLVMCodeGen::emitReturn(std::shared_ptr<SIRInstruction> inst) {
    if (inst->getNumOperands() == 0) {
        return builder_->CreateRetVoid();
    } else {
        llvm::Value* retVal = getNamedValue(inst->getOperand(0)->getName());
        return builder_->CreateRet(retVal);
    }
}

/**
 * تبديل / Switch
 */
llvm::Value* LLVMCodeGen::emitSwitch(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* value = getNamedValue(inst->getOperand(0)->getName());
    llvm::BasicBlock* defaultBlock = context_info_.basicBlocks[inst->getOperand(1)->getName()];
    
    size_t numCases = (inst->getNumOperands() - 2) / 2;
    llvm::SwitchInst* switchInst = builder_->CreateSwitch(value, defaultBlock, numCases);
    
    for (size_t i = 0; i < numCases; ++i) {
        auto caseValue = llvm::cast<llvm::ConstantInt>(
            getNamedValue(inst->getOperand(2 + i * 2)->getName())
        );
        llvm::BasicBlock* caseBlock = context_info_.basicBlocks[
            inst->getOperand(2 + i * 2 + 1)->getName()
        ];
        switchInst->addCase(caseValue, caseBlock);
    }
    
    return switchInst;
}

// ============================================================================
// Type Conversion Instructions / تعليمات تحويل الأنواع
// ============================================================================

llvm::Value* LLVMCodeGen::emitCast(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* value = getNamedValue(inst->getOperand(0)->getName());
    llvm::Type* targetType = convertType(inst->getType());
    return builder_->CreateCast(llvm::Instruction::CastOps::BitCast, value, targetType, inst->getName());
}

llvm::Value* LLVMCodeGen::emitBitCast(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* value = getNamedValue(inst->getOperand(0)->getName());
    llvm::Type* targetType = convertType(inst->getType());
    return builder_->CreateBitCast(value, targetType, inst->getName());
}

llvm::Value* LLVMCodeGen::emitIntToPtr(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* value = getNamedValue(inst->getOperand(0)->getName());
    llvm::Type* targetType = convertType(inst->getType());
    return builder_->CreateIntToPtr(value, targetType, inst->getName());
}

llvm::Value* LLVMCodeGen::emitPtrToInt(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* value = getNamedValue(inst->getOperand(0)->getName());
    llvm::Type* targetType = convertType(inst->getType());
    return builder_->CreatePtrToInt(value, targetType, inst->getName());
}

llvm::Value* LLVMCodeGen::emitTrunc(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* value = getNamedValue(inst->getOperand(0)->getName());
    llvm::Type* targetType = convertType(inst->getType());
    return builder_->CreateTrunc(value, targetType, inst->getName());
}

llvm::Value* LLVMCodeGen::emitZExt(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* value = getNamedValue(inst->getOperand(0)->getName());
    llvm::Type* targetType = convertType(inst->getType());
    return builder_->CreateZExt(value, targetType, inst->getName());
}

llvm::Value* LLVMCodeGen::emitSExt(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* value = getNamedValue(inst->getOperand(0)->getName());
    llvm::Type* targetType = convertType(inst->getType());
    return builder_->CreateSExt(value, targetType, inst->getName());
}

// ============================================================================
// Aggregate Instructions / تعليمات التجميع
// ============================================================================

llvm::Value* LLVMCodeGen::emitExtractValue(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* agg = getNamedValue(inst->getOperand(0)->getName());
    
    // ✅ TODO 3: استخراج الفهارس من التعليمة / Get indices from instruction
    std::vector<unsigned> indices;
    
    // الفهارس تبدأ من المعامل 1 / Indices start from operand 1
    for (size_t i = 1; i < inst->getNumOperands(); ++i) {
        auto operand = inst->getOperand(i);
        if (operand->isConstant()) {
            // فهرس ثابت / Constant index
            int64_t idx = operand->getIntValue();
            indices.push_back(static_cast<unsigned>(idx));
        } else {
            // فهرس متغير - نستخدم 0 كافتراضي / Variable index - use 0 as default
            indices.push_back(0);
        }
    }
    
    // إذا لم توجد فهارس، نستخدم 0 / If no indices, use 0
    if (indices.empty()) {
        indices.push_back(0);
    }
    
    return builder_->CreateExtractValue(agg, indices, inst->getName());
}

llvm::Value* LLVMCodeGen::emitInsertValue(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* agg = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* value = getNamedValue(inst->getOperand(1)->getName());
    
    // ✅ TODO 3: استخراج الفهارس من التعليمة / Get indices from instruction
    std::vector<unsigned> indices;
    
    // الفهارس تبدأ من المعامل 2 / Indices start from operand 2
    for (size_t i = 2; i < inst->getNumOperands(); ++i) {
        auto operand = inst->getOperand(i);
        if (operand->isConstant()) {
            // فهرس ثابت / Constant index
            int64_t idx = operand->getIntValue();
            indices.push_back(static_cast<unsigned>(idx));
        } else {
            // فهرس متغير - نستخدم 0 كافتراضي / Variable index - use 0 as default
            indices.push_back(0);
        }
    }
    
    // إذا لم توجد فهارس، نستخدم 0 / If no indices, use 0
    if (indices.empty()) {
        indices.push_back(0);
    }
    
    return builder_->CreateInsertValue(agg, value, indices, inst->getName());
}

llvm::Value* LLVMCodeGen::emitExtractElement(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* vec = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* idx = getNamedValue(inst->getOperand(1)->getName());
    return builder_->CreateExtractElement(vec, idx, inst->getName());
}

llvm::Value* LLVMCodeGen::emitInsertElement(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* vec = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* value = getNamedValue(inst->getOperand(1)->getName());
    llvm::Value* idx = getNamedValue(inst->getOperand(2)->getName());
    return builder_->CreateInsertElement(vec, value, idx, inst->getName());
}

// ============================================================================
// Phi & Select / فاي والاختيار
// ============================================================================

llvm::Value* LLVMCodeGen::emitPhi(std::shared_ptr<SIRInstruction> inst) {
    llvm::Type* type = convertType(inst->getType());
    llvm::PHINode* phi = builder_->CreatePHI(type, inst->getNumOperands() / 2, inst->getName());
    
    // إضافة القيم والكتل / Add values and blocks
    for (size_t i = 0; i < inst->getNumOperands(); i += 2) {
        llvm::Value* value = getNamedValue(inst->getOperand(i)->getName());
        llvm::BasicBlock* block = context_info_.basicBlocks[inst->getOperand(i + 1)->getName()];
        phi->addIncoming(value, block);
    }
    
    return phi;
}

llvm::Value* LLVMCodeGen::emitSelect(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* cond = getNamedValue(inst->getOperand(0)->getName());
    llvm::Value* trueVal = getNamedValue(inst->getOperand(1)->getName());
    llvm::Value* falseVal = getNamedValue(inst->getOperand(2)->getName());
    return builder_->CreateSelect(cond, trueVal, falseVal, inst->getName());
}

// ============================================================================
// Type Conversion / تحويل الأنواع
// ============================================================================

/**
 * تحويل نوع Sad إلى نوع LLVM
 * Convert Sad type to LLVM type
 */
llvm::Type* LLVMCodeGen::convertType(std::shared_ptr<Type> sadType) {
    // استخدام محول الأنواع المتقدم / Use advanced type mapper
    if (typeMapper_) {
        return typeMapper_->mapSadType(sadType);  // تحويل بواسطة TypeMapper / Convert via TypeMapper
    }
    
    // إذا لم يكن TypeMapper متاحاً، استخدام التحويل الأساسي / If TypeMapper not available, use basic conversion
    // ✅ TODO 4: تنفيذ كامل لتحويل الأنواع / Complete type conversion implementation
    
    // الأنواع الأساسية / Basic types
    if (sadType->isVoid()) {
        return getVoidType();
    } else if (sadType->isInteger()) {
        // دعم أحجام مختلفة / Support different sizes
        int bitWidth = sadType->getBitWidth();
        if (bitWidth == 8) return llvm::Type::getInt8Ty(*context_);
        if (bitWidth == 16) return llvm::Type::getInt16Ty(*context_);
        if (bitWidth == 32) return llvm::Type::getInt32Ty(*context_);
        return getInt64Type(); // افتراضي 64-bit / Default 64-bit
    } else if (sadType->isFloat()) {
        // float أو double / float or double
        if (sadType->isFloat32()) {
            return llvm::Type::getFloatTy(*context_);
        }
        return getDoubleType(); // افتراضي double / Default double
    } else if (sadType->isBoolean()) {
        return getInt1Type();
    } else if (sadType->isPointer()) {
        llvm::Type* pointeeType = convertType(sadType->getPointeeType());
        return pointeeType->getPointerTo();
    } else if (sadType->isArray()) {
        llvm::Type* elementType = convertType(sadType->getElementType());
        return llvm::ArrayType::get(elementType, sadType->getArraySize());
    } else if (sadType->isVoid()) {
        return getVoidType();
    }
    
    // افتراضي: i64
    return getInt64Type();
}

/**
 * تحويل نوع دالة Sad إلى نوع دالة LLVM
 */
llvm::FunctionType* LLVMCodeGen::convertFunctionType(
    std::shared_ptr<Type> returnType,
    const std::vector<std::shared_ptr<Type>>& paramTypes,
    bool isVarArg)
{
    // استخدام محول الأنواع المتقدم / Use advanced type mapper
    if (typeMapper_) {
        return typeMapper_->mapFunctionType(returnType, paramTypes, isVarArg);  // تحويل بواسطة TypeMapper
    }
    
    // إذا لم يكن TypeMapper متاحاً، استخدام التحويل الأساسي / If TypeMapper not available, use basic conversion
    llvm::Type* retType = convertType(returnType);  // تحويل نوع الرجوع / Convert return type
    
    std::vector<llvm::Type*> params;  // قائمة أنواع المعاملات / Parameter types list
    for (const auto& paramType : paramTypes) {
        params.push_back(convertType(paramType));  // تحويل كل نوع معامل / Convert each parameter type
    }
    
    return llvm::FunctionType::get(retType, params, isVarArg);  // إنشاء نوع الدالة / Create function type
}

// ============================================================================
// Primitive Types / الأنواع الأساسية
// ============================================================================

llvm::Type* LLVMCodeGen::getVoidType() {
    return llvm::Type::getVoidTy(*context_);
}

llvm::Type* LLVMCodeGen::getInt1Type() {
    return llvm::Type::getInt1Ty(*context_);
}

llvm::Type* LLVMCodeGen::getInt8Type() {
    return llvm::Type::getInt8Ty(*context_);
}

llvm::Type* LLVMCodeGen::getInt16Type() {
    return llvm::Type::getInt16Ty(*context_);
}

llvm::Type* LLVMCodeGen::getInt32Type() {
    return llvm::Type::getInt32Ty(*context_);
}

llvm::Type* LLVMCodeGen::getInt64Type() {
    return llvm::Type::getInt64Ty(*context_);
}

llvm::Type* LLVMCodeGen::getFloatType() {
    return llvm::Type::getFloatTy(*context_);
}

llvm::Type* LLVMCodeGen::getDoubleType() {
    return llvm::Type::getDoubleTy(*context_);
}

llvm::Type* LLVMCodeGen::getInt8PtrType() {
    return llvm::Type::getInt8PtrTy(*context_);
}

// ============================================================================
// Constants / الثوابت
// ============================================================================

llvm::Constant* LLVMCodeGen::getConstantInt(int64_t value, int bits) {
    return llvm::ConstantInt::get(*context_, llvm::APInt(bits, value, true));
}

llvm::Constant* LLVMCodeGen::getConstantFloat(double value, bool isDouble) {
    if (isDouble) {
        return llvm::ConstantFP::get(getDoubleType(), value);
    } else {
        return llvm::ConstantFP::get(getFloatType(), value);
    }
}

llvm::Constant* LLVMCodeGen::getConstantString(const std::string& value) {
    return builder_->CreateGlobalStringPtr(value);
}

llvm::Constant* LLVMCodeGen::getConstantBool(bool value) {
    return llvm::ConstantInt::get(getInt1Type(), value ? 1 : 0);
}

llvm::Constant* LLVMCodeGen::getNullPtr(llvm::Type* ptrType) {
    return llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrType));
}

// ============================================================================
// Output / الإخراج
// ============================================================================

/**
 * حفظ LLVM IR إلى ملف
 * Save LLVM IR to file
 */
bool LLVMCodeGen::emitToFile(const std::string& filename) const {
    if (!module_) {
        return false;
    }
    
    std::error_code EC;
    llvm::raw_fd_ostream file(filename, EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        std::cerr << "خطأ في فتح الملف / Error opening file: " << EC.message() << std::endl;
        return false;
    }
    
    module_->print(file, nullptr);
    return true;
}

/**
 * حفظ LLVM IR إلى نص
 * Save LLVM IR to string
 */
std::string LLVMCodeGen::emitToString() const {
    if (!module_) {
        return "";
    }
    
    std::string str;
    llvm::raw_string_ostream stream(str);
    module_->print(stream, nullptr);
    return stream.str();
}

/**
 * طباعة LLVM IR إلى stderr
 * Print LLVM IR to stderr
 */
void LLVMCodeGen::dump() const {
    if (module_) {
        module_->print(llvm::errs(), nullptr);
    }
}

// ============================================================================
// Error Handling / معالجة الأخطاء
// ============================================================================

void LLVMCodeGen::reportError(const std::string& message) {
    hasErrors_ = true;
    errors_.push_back(message);
    std::cerr << "خطأ / Error: " << message << std::endl;
}

// ============================================================================
// Helper Methods / دوال مساعدة
// ============================================================================

llvm::Value* LLVMCodeGen::getNamedValue(const std::string& name) {
    auto it = context_info_.namedValues.find(name);
    if (it != context_info_.namedValues.end()) {
        // إذا كانت alloca، تحميل القيمة / If it's alloca, load the value
        if (auto alloca = llvm::dyn_cast<llvm::AllocaInst>(it->second)) {
            return builder_->CreateLoad(alloca->getAllocatedType(), alloca, name);
        }
        return it->second;
    }
    
    reportError("Unknown value: " + name);
    return nullptr;
}

void LLVMCodeGen::setNamedValue(const std::string& name, llvm::Value* value) {
    context_info_.namedValues[name] = value;
}

llvm::AllocaInst* LLVMCodeGen::createEntryBlockAlloca(llvm::Function* function,
                                                      const std::string& varName,
                                                      llvm::Type* type) {
    llvm::IRBuilder<> tmpBuilder(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpBuilder.CreateAlloca(type, nullptr, varName);
}

} // namespace LLVM
} // namespace Sad
