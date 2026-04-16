/*
 * ============================================================================
 * LLVM IR Code Generator - Alloca and Move Operations
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
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
#include <algorithm>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
        /**
         * إصدار تعليمة تخصيص ذاكرة
         * Emit memory allocation instruction
         *
         * Source: llvm_codegen.h:446
         */
        llvm::Value *LLVMCodeGen::emitAlloca(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            std::string regName = inst->result.has_value() ? inst->result->name : "alloca";

            // ================================================================
            // حالة 1: تخصيص كائن جديد (ALLOC مع اسم صنف كـ metadata)
            // Case 1: New object allocation (ALLOC with class name metadata)
            // ================================================================
            if (!inst->operands.empty() &&
                inst->operands[0].type == SIROperandType::CONSTANT &&
                inst->operands[0].dataType == SadTypeKind::String)
            {

                std::string className = inst->operands[0].name;

                auto structIt = context_info_.classStructTypes.find(className);
                if (structIt != context_info_.classStructTypes.end())
                {
                    // (AR) تخصيص هيكلة الصنف على الكومة (malloc) لتبقى بعد عودة الدالة
                    // (EN) Allocate class struct on HEAP (malloc) so it survives beyond function return
                    // (AR) جديد = تخصيص كومة دائماً — alloca يُحذف عند عودة الدالة
                    // (EN) new = heap allocation always — alloca dies when function returns
                    llvm::StructType *structType = structIt->second;

                    // (AR) حساب حجم الهيكل
                    // (EN) Calculate struct size
                    const llvm::DataLayout &DL = module_->getDataLayout();
                    uint64_t structSize = DL.getTypeAllocSize(structType);
                    llvm::Value *sizeVal = llvm::ConstantInt::get(
                        llvm::Type::getInt64Ty(*context_), structSize);

                    // (AR) استدعاء malloc للتخصيص على الكومة
                    // (EN) Call malloc for heap allocation
                    llvm::Function *mallocFn = module_->getFunction("malloc");
                    if (!mallocFn)
                    {
                        // (AR) إعلان malloc إذا لم يكن موجوداً
                        // (EN) Declare malloc if not found
                        llvm::FunctionType *mallocTy = llvm::FunctionType::get(
                            llvm::PointerType::get(*context_, 0),
                            {llvm::Type::getInt64Ty(*context_)}, false);
                        mallocFn = llvm::Function::Create(
                            mallocTy, llvm::Function::ExternalLinkage, "malloc", module_.get());
                    }

                    llvm::Value *rawPtr = builder_->CreateCall(mallocFn, {sizeVal}, regName + ".heap");

                    // (AR) تصفير الذاكرة المخصصة
                    // (EN) Zero-initialize the allocated memory
                    builder_->CreateMemSet(rawPtr,
                                           llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context_), 0),
                                           sizeVal, llvm::MaybeAlign(8));

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) تهيئة حقول المصفوفات — إصلاح حرج لمنع انهيار null pointer
                    //      حقول المصفوفة المُهيَّأة بـ [] تحتاج تخصيص SadArray فعلي
                    //      بدلاً من ترك المؤشر صفرياً بعد memset(0)
                    // (EN) Initialize array fields — critical fix to prevent null pointer crash
                    //      Array fields initialized with [] need actual SadArray allocation
                    //      instead of leaving the pointer null after memset(0)
                    // ═══════════════════════════════════════════════════════════════
                    if (sirModule_)
                    {
                        auto sirClass = sirModule_->getClass(className);
                        if (sirClass && !sirClass->arrayFields_.empty())
                        {
                            // (AR) بنية SadArray: {i64 length, i64 capacity, ptr data}
                            // (EN) SadArray struct: {i64 length, i64 capacity, ptr data}
                            auto i64Ty = llvm::Type::getInt64Ty(*context_);
                            auto *ptrTy = llvm::PointerType::getUnqual(*context_);
                            llvm::StructType *arrTy = llvm::StructType::getTypeByName(*context_, "SadArray");
                            if (!arrTy)
                            {
                                arrTy = llvm::StructType::create(*context_, {i64Ty, i64Ty, ptrTy}, "SadArray");
                            }
                            auto *arrStructSize = llvm::ConstantExpr::getSizeOf(arrTy);

                            int fieldIdx = 0;
                            for (const auto &fieldName : sirClass->fieldOrder_)
                            {
                                if (sirClass->isArrayField(fieldName))
                                {
                                    // (AR) تخصيص SadArray {length=0, capacity=8, data=malloc(8*ptrsize)}
                                    // (EN) Allocate SadArray {length=0, capacity=8, data=malloc(8*ptrsize)}
                                    llvm::Value *arrPtr = builder_->CreateCall(
                                        mallocFn, {builder_->CreateIntCast(arrStructSize, i64Ty, false)},
                                        fieldName + ".arr");

                                    // length = 0
                                    llvm::Value *lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, fieldName + ".len");
                                    builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), lenGep);

                                    // capacity = 8
                                    llvm::Value *capGep = builder_->CreateStructGEP(arrTy, arrPtr, 1, fieldName + ".cap");
                                    builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 8), capGep);

                                    // data = malloc(8 * sizeof(ptr))
                                    auto *ptrSize = llvm::ConstantExpr::getSizeOf(ptrTy);
                                    llvm::Value *dataSize = builder_->CreateMul(
                                        llvm::ConstantInt::get(i64Ty, 8),
                                        builder_->CreateIntCast(ptrSize, i64Ty, false),
                                        fieldName + ".datasz");
                                    llvm::Value *dataPtr = builder_->CreateCall(
                                        mallocFn, {dataSize}, fieldName + ".data");
                                    llvm::Value *dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, fieldName + ".datagep");
                                    builder_->CreateStore(dataPtr, dataGep);

                                    // (AR) تخزين مؤشر المصفوفة في حقل الكائن (fieldIdx + 1 بسبب vtable)
                                    // (EN) Store array pointer in object field (fieldIdx + 1 for vtable)
                                    llvm::Value *objFieldGep = builder_->CreateStructGEP(
                                        structType, rawPtr, fieldIdx + 1, fieldName + ".objfield");
                                    builder_->CreateStore(arrPtr, objFieldGep);

#ifndef NDEBUG
                                    std::cout << "[DEBUG] emitAlloca: initialized array field '"
                                              << fieldName << "' for class '" << className << "'" << std::endl;
#endif
                                }
                                fieldIdx++;
                            }
                        }
                    }

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) تطبيق القيم الابتدائية للحقول — إصلاح حرج للأصناف بدون باني
                    //      بعد memset(0) كل الحقول تصبح صفرية. هذا الكود يُعيد القيم الابتدائية
                    //      المُعرّفة في تعريف الصنف (مثل: متغير عام س = 10)
                    //      يبحث في سلسلة الوراثة الكاملة (من الأب الأعلى إلى الابن)
                    // (EN) Apply field default values — critical fix for classes without constructors
                    //      After memset(0) all fields are zeroed. This restores default values
                    //      defined in class definition (e.g., var public x = 10)
                    //      Searches full inheritance chain (root parent to child)
                    // ═══════════════════════════════════════════════════════════════
                    if (sirModule_)
                    {
                        auto sirClass = sirModule_->getClass(className);
                        if (sirClass)
                        {
                            // (AR) بناء سلسلة الوراثة من الأب الأعلى إلى الصنف الحالي
                            // (EN) Build inheritance chain from root parent to current class
                            std::vector<std::shared_ptr<Sad::Compiler::SIR::SIRClass>> inheritanceChain;
                            {
                                auto current = sirClass;
                                while (current)
                                {
                                    inheritanceChain.push_back(current);
                                    if (!current->parentClass.empty())
                                    {
                                        current = sirModule_->getClass(current->parentClass);
                                    }
                                    else
                                    {
                                        current = nullptr;
                                    }
                                }
                                std::reverse(inheritanceChain.begin(), inheritanceChain.end());
                            }

                            // (AR) تطبيق القيم الابتدائية لكل حقل
                            // (EN) Apply default values for each field
                            int fieldIdx2 = 0;
                            for (const auto &fieldName : sirClass->fieldOrder_)
                            {
                                // (AR) البحث عن القيمة الابتدائية في سلسلة الوراثة
                                // (EN) Search for default value in inheritance chain
                                std::string defaultVal;
                                SadTypeKind defaultType = SadTypeKind::Unknown;
                                for (const auto &ancestor : inheritanceChain)
                                {
                                    auto it = ancestor->fieldDefaultValues_.find(fieldName);
                                    if (it != ancestor->fieldDefaultValues_.end())
                                    {
                                        defaultVal = it->second.first;
                                        defaultType = it->second.second;
                                        break;
                                    }
                                }

                                if (!defaultVal.empty() && defaultType != SadTypeKind::Unknown)
                                {
                                    // (AR) fieldIdx2 + 1 لأن الحقل 0 هو vtable pointer
                                    // (EN) fieldIdx2 + 1 because field 0 is vtable pointer
                                    llvm::Value *fieldGep = builder_->CreateStructGEP(
                                        structType, rawPtr, fieldIdx2 + 1, fieldName + ".default_init");

                                    switch (defaultType)
                                    {
                                    case SadTypeKind::Integer:
                                    {
                                        int64_t intVal = 0;
                                        try
                                        {
                                            intVal = std::stoll(defaultVal);
                                        }
                                        catch (...)
                                        {
                                        }
                                        builder_->CreateStore(
                                            llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), intVal), fieldGep);
                                        break;
                                    }
                                    case SadTypeKind::Float:
                                    {
                                        double dblVal = 0.0;
                                        try
                                        {
                                            dblVal = std::stod(defaultVal);
                                        }
                                        catch (...)
                                        {
                                        }
                                        builder_->CreateStore(
                                            llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), dblVal), fieldGep);
                                        break;
                                    }
                                    case SadTypeKind::Boolean:
                                    {
                                        bool boolVal = (defaultVal == "\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD" ||
                                                        defaultVal == "true" || defaultVal == "1");
                                        builder_->CreateStore(
                                            llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), boolVal ? 1 : 0), fieldGep);
                                        break;
                                    }
                                    case SadTypeKind::String:
                                    {
                                        // (AR) إنشاء نص ثابت عبر sad_string_new_cstr
                                        // (EN) Create constant string via sad_string_new_cstr
                                        auto *strFnTy = llvm::FunctionType::get(
                                            llvm::PointerType::getUnqual(*context_),
                                            {llvm::PointerType::getUnqual(*context_)}, false);
                                        auto strFn = module_->getOrInsertFunction("sad_string_new_cstr", strFnTy);
                                        auto *strConst = builder_->CreateGlobalStringPtr(defaultVal, fieldName + ".defstr");
                                        llvm::Value *strVal = builder_->CreateCall(strFn, {strConst}, fieldName + ".defval");
                                        builder_->CreateStore(strVal, fieldGep);
                                        break;
                                    }
                                    default:
                                        break;
                                    }
                                }
                                fieldIdx2++;
                            }
                        }
                    }

                    // (AR) تخزين مؤشر vtable في الحقل 0 — ضروري للاستدعاء الافتراضي
                    // (EN) Store vtable pointer in field 0 — required for virtual dispatch
                    storeVtablePtr(rawPtr, className);

                    llvm::Value *result = rawPtr;

                    if (inst->result.has_value())
                    {
                        context_info_.namedValues[inst->result->name] = result;
                        context_info_.objectClassMap[inst->result->name] = className;
                    }

                    return result;
                }
            }

            // ================================================================
            // حالة 2: داخل الباني أو دالة الصنف - حقول الصنف تصبح GEP من self
            // Case 2: Inside constructor or method - class fields become GEP from self
            // ================================================================
            std::string activeClass;
            if (!context_info_.currentConstructorClass.empty())
            {
                activeClass = context_info_.currentConstructorClass;
            }
            else if (!context_info_.currentMethodClass.empty())
            {
                activeClass = context_info_.currentMethodClass;
            }

            if (!activeClass.empty() && inst->result.has_value())
            {
                std::string fieldName = inst->result->name;
                // (AR) إزالة بادئة % إن وجدت
                // (EN) Strip % prefix if present
                if (!fieldName.empty() && fieldName[0] == '%')
                {
                    fieldName = fieldName.substr(1);
                }

                auto fieldIt = context_info_.classFieldNames.find(activeClass);
                if (fieldIt != context_info_.classFieldNames.end())
                {
                    const auto &fieldNames = fieldIt->second;
                    int fieldIndex = -1;
                    for (size_t i = 0; i < fieldNames.size(); i++)
                    {
                        if (fieldNames[i] == fieldName)
                        {
                            fieldIndex = static_cast<int>(i);
                            break;
                        }
                    }

                    if (fieldIndex >= 0)
                    {
                        // (AR) إضافة إزاحة vtable — الحقل 0 في الهيكل هو مؤشر vtable
                        // (EN) Add vtable offset — field 0 in struct is vtable pointer
                        fieldIndex = getFieldStructIndex(activeClass, fieldIndex);

                        // (AR) استخدام GEP من self بدلاً من alloca محلي
                        // (EN) Use GEP from self instead of local alloca
                        llvm::Value *selfPtr = nullptr;
                        auto selfIt = context_info_.namedValues.find("self");
                        if (selfIt != context_info_.namedValues.end())
                        {
                            selfPtr = selfIt->second;
                        }
                        else
                        {
                            auto selfIt2 = context_info_.namedValues.find("%self");
                            if (selfIt2 != context_info_.namedValues.end())
                            {
                                selfPtr = selfIt2->second;
                            }
                        }

                        if (selfPtr)
                        {
                            llvm::StructType *structType = context_info_.classStructTypes[activeClass];

                            // (AR) إذا كان self alloca، نحمّله القيمة ونحولها لمؤشر
                            // (EN) If self is an alloca, load value and convert to pointer
                            llvm::Value *actualSelf = selfPtr;
                            if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(selfPtr))
                            {
                                if (!allocaInst->getAllocatedType()->isStructTy())
                                {
                                    // self هو alloca i64 يحتوي مؤشر محول ptrtoint
                                    // self is an i64 alloca holding a ptrtoint-ed pointer
                                    llvm::Value *selfVal = builder_->CreateLoad(
                                        allocaInst->getAllocatedType(), allocaInst, "self.val");
                                    actualSelf = builder_->CreateIntToPtr(
                                        selfVal, llvm::PointerType::getUnqual(*context_), "self.ptr");
                                }
                            }

                            llvm::Value *gep = builder_->CreateStructGEP(
                                structType, actualSelf, fieldIndex, fieldName + "_ptr");

                            // ═══════════════════════════════════════════════════════════════
                            // (AR) حماية دفاعية: لا نكتب فوق alloca معامل دالة موجود
                            //      إذا كان namedValues يحتوي بالفعل على alloca لهذا الاسم
                            //      (تم إنشاؤه بواسطة emitFunctionParameters)، لا نستبدله بـ GEP الحقل
                            //      هذا يمنع قراءة حقل غير مُهيّأ بدلاً من معامل الدالة
                            // (EN) Defensive guard: don't overwrite function parameter alloca
                            //      If namedValues already has an alloca for this name
                            //      (created by emitFunctionParameters), don't replace it with field GEP
                            //      This prevents reading uninitialized field instead of function param
                            // ═══════════════════════════════════════════════════════════════
                            auto existingIt = context_info_.namedValues.find(inst->result->name);
                            if (existingIt != context_info_.namedValues.end() &&
                                llvm::dyn_cast<llvm::AllocaInst>(existingIt->second))
                            {
                                // (AR) الاسم موجود بالفعل كـ alloca (معامل) — لا نكتب فوقه
                                //      نخزّن GEP الحقل تحت اسم بديل للوصول عبر هذا.حقل
                                // (EN) Name already exists as alloca (parameter) — don't overwrite
                                //      Store field GEP under alternative name for this.field access
                                context_info_.namedValues["__field__." + fieldName] = gep;
#ifndef NDEBUG
                                std::cout << "[DEBUG] emitAlloca: field '" << fieldName
                                          << "' conflicts with param — stored as __field__." << fieldName
                                          << " (class: " << activeClass << ")" << std::endl;
#endif
                            }
                            else
                            {
                                context_info_.namedValues[inst->result->name] = gep;
                            }

#ifndef NDEBUG
                            std::cout << "[DEBUG] emitAlloca: class field '" << fieldName
                                      << "' mapped to GEP index " << fieldIndex
                                      << " (class: " << activeClass << ")" << std::endl;
#endif

                            return gep;
                        }
                    }
                }
            }

            // ================================================================
            // حالة 3: تخصيص عادي
            // Case 3: Normal allocation
            // ================================================================
            llvm::Type *allocType = getInt64Type(); // default
            if (inst->result.has_value())
            {
                switch (inst->result->dataType)
                {
                case SadTypeKind::Float:
                    allocType = getDoubleType();
                    break;
                case SadTypeKind::Boolean:
                    allocType = llvm::Type::getInt1Ty(*context_);
                    break;
                case SadTypeKind::String:
                case SadTypeKind::Pointer:
                    // (AR) أنواع المؤشرات: النصوص ومؤشرات UI widgets
                    // (EN) Pointer types: strings and UI widget pointers
                    allocType = llvm::PointerType::getUnqual(*context_);
                    break;
                default:
                    allocType = getInt64Type();
                    break;
                }
            }

            // ================================================================
            // (AR) فحص إذا كان الاسم يطابق متغيراً عاماً — نستخدمه بدلاً من alloca محلي
            // (EN) Check if name matches a global variable — use it instead of local alloca
            // ================================================================
            if (inst->result.has_value())
            {
                std::string cleanName = regName;
                if (!cleanName.empty() && cleanName[0] == '%')
                {
                    cleanName = cleanName.substr(1);
                }
                // (AR) البحث في المتغيرات العامة
                // (EN) Search global variables
                llvm::GlobalVariable *gv = module_->getGlobalVariable(cleanName);
                if (!gv && cleanName != regName)
                {
                    gv = module_->getGlobalVariable(regName);
                }
                if (!gv)
                {
                    // (AR) البحث في الخريطة الدائمة
                    // (EN) Search persistent map
                    auto git = context_info_.globalValues.find(cleanName);
                    if (git == context_info_.globalValues.end())
                    {
                        git = context_info_.globalValues.find(regName);
                    }
                    if (git != context_info_.globalValues.end() && git->second != nullptr)
                    {
                        if (auto *gvFromMap = llvm::dyn_cast<llvm::GlobalVariable>(git->second))
                        {
                            gv = gvFromMap;
                        }
                    }
                }
                if (gv)
                {
                    // (AR) إذا كان المتغير العالمي مصفوفة [N x i8] (ثابت نصي من CreateGlobalStringPtr)
                    //      نُنشئ GEP للعنصر الأول بدلاً من تخزين GlobalVariable مباشرة
                    //      هذا يمنع خطأ load [N x i8] + inttoptr في resolveOperand
                    // (EN) If global is [N x i8] array (string constant from CreateGlobalStringPtr),
                    //      create GEP to first element instead of storing GlobalVariable directly.
                    //      This prevents load [N x i8] + inttoptr error in resolveOperand
                    if (gv->getValueType()->isArrayTy())
                    {
                        llvm::Value *gepVal = builder_->CreateInBoundsGEP(
                            gv->getValueType(), gv,
                            {llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 0),
                             llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 0)},
                            inst->result->name + ".ptr");
                        context_info_.namedValues[inst->result->name] = gepVal;
                        return gepVal;
                    }
                    // (AR) المتغير العام موجود — نستخدمه مباشرة بدلاً من alloca
                    // (EN) Global variable exists — use it directly instead of alloca
                    context_info_.namedValues[inst->result->name] = gv;
                    return gv;
                }
            }

            // Hoist alloca to entry block to avoid stack growth in loops
            llvm::Function *func = builder_->GetInsertBlock()->getParent();
            llvm::IRBuilder<> tmpBuilder(&func->getEntryBlock(),
                                         func->getEntryBlock().begin());
            llvm::Value *result = tmpBuilder.CreateAlloca(allocType, nullptr, regName);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة نقل (تعيين سجل)
         * Emit move instruction (register assignment)
         *
         * MOVE يأخذ مصدر واحد (ثابت أو سجل) ويخزنه في سجل النتيجة
         * MOVE takes one source (constant or register) and stores it in the result register
         */
        llvm::Value *LLVMCodeGen::emitMove(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.empty())
            {
                reportError("Move instruction requires 1 operand");
                return nullptr;
            }

            if (!inst->result.has_value())
            {
                reportError("Move instruction requires a result register");
                return nullptr;
            }

            // (AR) تحويل المعامل المصدر إلى قيمة LLVM
            // (EN) Resolve source operand to LLVM value
            llvm::Value *value = nullptr;
            const auto &srcOp = inst->operands[0];

            if (srcOp.type == SIROperandType::CONSTANT)
            {
                switch (srcOp.dataType)
                {
                case SadTypeKind::Integer:
                    value = getConstantInt(srcOp.intValue, 64);
                    break;
                case SadTypeKind::Float:
                    value = getConstantFloat(srcOp.floatValue, true);
                    break;
                case SadTypeKind::Boolean:
                    value = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), srcOp.boolValue);
                    break;
                case SadTypeKind::String:
                    value = getConstantString(srcOp.name);
                    break;
                default:
                    reportError("Unsupported constant type in move");
                    return nullptr;
                }
            }
            // ================================================================
            // (AR) Fix #41b: MOVE مع مؤشر دالة (FUNCTION operand)
            //      عند ضمّ (inline) دالة تُرجع لامدا، المُحسّن يُنتج:
            //        %%27 = move @__lambda_0
            //      هنا srcOp.type == FUNCTION — نبحث عن الدالة في الوحدة
            //      ونحوّل مؤشرها إلى i64 لتخزينه في سجل عادي
            // (EN) Fix #41b: MOVE with function pointer (FUNCTION operand)
            //      When inlining a function that returns a lambda, optimizer produces:
            //        %%27 = move @__lambda_0
            //      Here srcOp.type == FUNCTION — look up function in module
            //      and convert its pointer to i64 for storage in a regular register
            // ================================================================
            else if (srcOp.type == SIROperandType::FUNCTION)
            {
                llvm::Function *fn = module_->getFunction(srcOp.name);
                if (fn)
                {
                    // (AR) تحويل مؤشر الدالة إلى i64 ليتوافق مع باقي النظام
                    // (EN) Convert function pointer to i64 for compatibility
                    value = builder_->CreatePtrToInt(fn, getInt64Type(), "fnptr2i");
                }
                else
                {
                    reportError("Function not found in move: " + srcOp.name);
                    return nullptr;
                }
            }
            else
            {
                // (AR) سجل - البحث في namedValues
                // (EN) Register - lookup in namedValues
                auto it = context_info_.namedValues.find(srcOp.name);
                if (it != context_info_.namedValues.end())
                {
                    value = it->second;
                }

                // (AR) إذا لم نجد في namedValues، نبحث في المتغيرات العامة
                // (EN) If not found in namedValues, search global variables
                if (!value && !srcOp.name.empty())
                {
                    // (AR) إزالة % من بداية الاسم إذا وجدت
                    // (EN) Strip % prefix from name if present
                    std::string globalName = srcOp.name;
                    if (!globalName.empty() && globalName[0] == '%')
                    {
                        globalName = globalName.substr(1);
                    }
                    llvm::GlobalVariable *gv = module_->getGlobalVariable(globalName);
                    if (gv)
                    {
                        value = gv; // (AR) استخدام المتغير العام مباشرة كمؤشر
                    }
                }
            }

            if (!value)
            {
                // (AR) قيمة المصدر غير موجودة، إنشاء alloca بديل
                // (EN) Source value not found, create fallback alloca
                if (!srcOp.name.empty())
                {
                    llvm::Function *currentFunc = builder_->GetInsertBlock()->getParent();
                    llvm::IRBuilder<> entryBuilder(&currentFunc->getEntryBlock(),
                                                   currentFunc->getEntryBlock().begin());
                    llvm::AllocaInst *newAlloca = entryBuilder.CreateAlloca(getInt64Type(), nullptr, srcOp.name);
                    context_info_.namedValues[srcOp.name] = newAlloca;
                    value = newAlloca;
                }
                else
                {
                    reportError("Source value not found for move: " + srcOp.name);
                    return nullptr;
                }
            }

            // (AR) تخزين القيمة في سجل النتيجة
            // (EN) Store value in result register
            context_info_.namedValues[inst->result->name] = value;

            return value;
        }

        /**
         * إصدار تعليمة GEP (Get Element Pointer)
         * Emit GEP instruction
         *
         * Source: llvm_codegen.h:447
         */
        llvm::Value *LLVMCodeGen::emitGEP(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                reportError("GEP instruction requires at least 2 operands");
                return nullptr;
            }

            llvm::Value *ptr = context_info_.namedValues[inst->operands[0].name];

            if (!ptr)
            {
                reportError("Pointer not found for GEP");
                return nullptr;
            }

            std::vector<llvm::Value *> indices;
            for (size_t i = 1; i < inst->operands.size(); ++i)
            {
                llvm::Value *idx = context_info_.namedValues[inst->operands[i].name];
                if (idx)
                {
                    indices.push_back(idx);
                }
            }

            // Source: llvm_codegen.cpp:1868 - LLVM 18 Opaque Pointers - استخدام i64 كنوع افتراضي
            llvm::Type *elementType = getInt64Type();
            llvm::Value *result = builder_->CreateGEP(elementType, ptr, indices, "geptmp");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        // ============================================================================
        // المرحلة 6: تعليمات تدفق التحكم
        // Phase 6: Control Flow Instructions
        // ============================================================================

        /**
         * إصدار تعليمة فرع غير مشروط
         * Emit unconditional branch instruction
         *
         * Source: llvm_codegen.h:453
         */

    } // namespace LLVM
} // namespace Sad
