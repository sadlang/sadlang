/*
 * ============================================================================
 * LLVM IR Code Generator - Store Operations
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

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
        /**
         * Emit store to memory instruction
         *
         * Source: llvm_codegen.h:445
         */
        llvm::Value *LLVMCodeGen::emitStore(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                reportError("Store instruction requires 2 operands");
                return nullptr;
            }

            // ================================================================
            // (AR) حالة member assign: 3 operands = (قيمة، كائن، اسم_الحقل)
            // (EN) Member assign case: 3 operands = (value, object, field_name)
            // SIR: store %اسم, %self, "الاسم"
            // ================================================================
            if (inst->operands.size() >= 3)
            {
                // (AR) توحيد المسار: تعيين العضو عبر OBJECT_SET لضمان دعم الخصائص
                // (EN) Unified path: member assignment via OBJECT_SET to ensure property support
                auto objectSetInst = std::make_shared<SIRInstruction>(SIROpcode::OBJECT_SET);
                objectSetInst->operands.push_back(inst->operands[1]); // object
                objectSetInst->operands.push_back(inst->operands[2]); // field name
                objectSetInst->operands.push_back(inst->operands[0]); // value
                return emitObjectSet(objectSetInst);

                llvm::Value *value = resolveOperand(inst->operands[0]);
                if (!value)
                {
                    reportError("emitStore: cannot resolve value operand: " + inst->operands[0].name);
                    return nullptr;
                }

                const std::string &objName = inst->operands[1].name;
                std::string fieldName = inst->operands[2].name;

                // إزالة علامات التنصيص إن وجدت
                if (!fieldName.empty() && fieldName.front() == '"')
                {
                    fieldName = fieldName.substr(1);
                }
                if (!fieldName.empty() && fieldName.back() == '"')
                {
                    fieldName = fieldName.substr(0, fieldName.size() - 1);
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] emitStore: member assign " << objName << "." << fieldName << std::endl;
#endif

                // البحث عن الكائن في namedValues
                auto objIt = context_info_.namedValues.find(objName);
                if (objIt == context_info_.namedValues.end())
                {
                    // (AR) Fallback: البحث في المتغيرات العامة LLVM
                    // (EN) Fallback: search in LLVM global variables
                    auto *globalVar = module_->getNamedGlobal(objName);
                    // (AR) إذا لم نجد، نحاول بإزالة % من المقدمة
                    // (EN) If not found, try without leading %
                    if (!globalVar && !objName.empty() && objName[0] == '%')
                    {
                        std::string cleanName = objName.substr(1);
                        globalVar = module_->getNamedGlobal(cleanName);
                    }
                    if (globalVar)
                    {
                        // (AR) تحميل القيمة من المتغير العام وتحويلها إلى مؤشر
                        // (EN) Load value from global and convert to pointer
                        llvm::Value *loaded = builder_->CreateLoad(getInt64Type(), globalVar, objName + ".glob.load");
                        llvm::Value *ptr = builder_->CreateIntToPtr(loaded,
                                                                    llvm::PointerType::getUnqual(*context_), objName + ".glob.ptr");
                        context_info_.namedValues[objName] = ptr;
                        objIt = context_info_.namedValues.find(objName);
                    }
                    else
                    {
                        reportError("emitStore: object not found: " + objName);
                        return nullptr;
                    }
                }
                llvm::Value *objPtr = objIt->second;

                // ================================================================
                // (AR) إذا كان objPtr من نوع alloca i64، يجب تحميل القيمة وتحويلها لمؤشر
                // (EN) If objPtr is alloca i64, load the value and convert to pointer
                // ================================================================
                if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(objPtr))
                {
                    if (allocaInst->getAllocatedType()->isIntegerTy(64))
                    {
                        // تحميل قيمة i64 من alloca
                        llvm::Value *ptrVal = builder_->CreateLoad(getInt64Type(), allocaInst, objName + ".ptrval");
                        // تحويل i64 إلى ptr
                        objPtr = builder_->CreateIntToPtr(ptrVal, llvm::PointerType::get(*context_, 0), objName + ".objptr");
#ifndef NDEBUG
                        std::cout << "[DEBUG] emitStore: converted i64 to ptr for " << objName << std::endl;
#endif
                    }
                }
                else if (llvm::dyn_cast<llvm::GlobalVariable>(objPtr))
                {
                    // (AR) متغير عام يحمل مؤشر كائن — يجب تحميل المؤشر أولاً ثم التحويل
                    // (EN) Global variable holding object pointer — must load pointer first then cast
                    llvm::Value *ptrVal = builder_->CreateLoad(getInt64Type(), objPtr, objName + ".glob.ptrval");
                    objPtr = builder_->CreateIntToPtr(ptrVal, llvm::PointerType::get(*context_, 0), objName + ".glob.objptr");
#ifndef NDEBUG
                    std::cout << "[DEBUG] emitStore: converted global i64 to ptr for " << objName << std::endl;
#endif
                }

                // البحث عن اسم الصنف في objectClassMap
                std::string className;
                auto classIt = context_info_.objectClassMap.find(objName);
                if (classIt != context_info_.objectClassMap.end())
                {
                    className = classIt->second;
                }

                // ================================================================
                // (AR) Fallback 0: البحث بدون % في objectClassMap
                // (EN) Fallback 0: search objectClassMap without leading %
                // ================================================================
                if (className.empty() && !objName.empty() && objName[0] == '%')
                {
                    auto classIt2 = context_info_.objectClassMap.find(objName.substr(1));
                    if (classIt2 != context_info_.objectClassMap.end())
                    {
                        className = classIt2->second;
                        context_info_.objectClassMap[objName] = className;
                    }
                }

                // ================================================================
                // (AR) Fallback 0b: إذا كان alloca لـ StructType، نستنتج الصنف من نوع الهيكل
                // (EN) Fallback 0b: if alloca of StructType, infer class from struct type
                // ================================================================
                if (className.empty())
                {
                    llvm::Value *origObj = objIt->second;
                    if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(origObj))
                    {
                        if (auto *st = llvm::dyn_cast<llvm::StructType>(allocaInst->getAllocatedType()))
                        {
                            for (const auto &[clsName, clsSt] : context_info_.classStructTypes)
                            {
                                if (clsSt == st)
                                {
                                    className = clsName;
                                    context_info_.objectClassMap[objName] = className;
                                    break;
                                }
                            }
                        }
                    }
                }

                // ================================================================
                // (AR) Fallback: إذا لم يُعرف الصنف، نبحث في كل الأصناف عن حقل بهذا الاسم
                // (EN) Fallback: if class unknown, search all classes for the field name
                // ================================================================
                if (className.empty())
                {
                    for (const auto &[clsName, fieldVec] : context_info_.classFieldNames)
                    {
                        for (const auto &fn : fieldVec)
                        {
                            if (fn == fieldName)
                            {
                                className = clsName;
                                context_info_.objectClassMap[objName] = className;
#ifndef NDEBUG
                                std::cout << "[DEBUG] emitStore: inferred class '" << className
                                          << "' for object '" << objName << "' by field '" << fieldName << "'" << std::endl;
#endif
                                break;
                            }
                        }
                        if (!className.empty())
                            break;
                    }
                }

                if (className.empty())
                {
// (AR) Fallback نهائي: تجاوز الخطأ — نواصل البرنامج بدون تعيين الحقل
// (EN) Final fallback: skip field store silently instead of failing completely
#ifndef NDEBUG
                    std::cout << "[WARNING] emitStore: no class info for object: " << objName
                              << ", field: " << fieldName << " — skipping field store" << std::endl;
#endif
                    // (AR) نرجع null لكن لا نوقف الترجمة
                    // (EN) Return null but don't stop compilation
                    return llvm::Constant::getNullValue(getInt64Type());
                }

                // البحث عن نوع الهيكل
                auto structIt = context_info_.classStructTypes.find(className);
                if (structIt == context_info_.classStructTypes.end())
                {
                    reportError("emitStore: struct type not found for class: " + className);
                    return nullptr;
                }
                llvm::StructType *structType = structIt->second;

                // البحث عن ترتيب الحقل
                auto fieldNamesIt = context_info_.classFieldNames.find(className);
                if (fieldNamesIt == context_info_.classFieldNames.end())
                {
                    reportError("emitStore: field names not found for class: " + className);
                    return nullptr;
                }

                const auto &fieldNames = fieldNamesIt->second;
                int fieldIndex = -1;
                for (size_t i = 0; i < fieldNames.size(); i++)
                {
                    if (fieldNames[i] == fieldName)
                    {
                        fieldIndex = static_cast<int>(i);
                        break;
                    }
                }

                if (fieldIndex < 0)
                {
                    // (AR) الحقل غير موجود — نبحث في الأصناف الأب ثم كل الأصناف
                    // (EN) Field not found — search parent classes then all classes
                    std::string searchClass = className;
                    bool foundField = false;
                    // (AR) البحث في الأصناف الأب
                    while (true)
                    {
                        auto parentIt = context_info_.classParentMap.find(searchClass);
                        if (parentIt == context_info_.classParentMap.end() || parentIt->second.empty())
                            break;
                        searchClass = parentIt->second;
                        auto pFieldIt = context_info_.classFieldNames.find(searchClass);
                        if (pFieldIt != context_info_.classFieldNames.end())
                        {
                            for (size_t i = 0; i < pFieldIt->second.size(); i++)
                            {
                                if (pFieldIt->second[i] == fieldName)
                                {
                                    fieldIndex = static_cast<int>(i);
                                    className = searchClass;
                                    foundField = true;
                                    break;
                                }
                            }
                        }
                        if (foundField)
                            break;
                    }
                    // (AR) البحث في كل الأصناف كـ fallback
                    if (!foundField)
                    {
                        for (const auto &[clsName, fieldVec] : context_info_.classFieldNames)
                        {
                            if (clsName == className)
                                continue;
                            for (size_t i = 0; i < fieldVec.size(); i++)
                            {
                                if (fieldVec[i] == fieldName)
                                {
                                    fieldIndex = static_cast<int>(i);
                                    className = clsName;
                                    foundField = true;
                                    break;
                                }
                            }
                            if (foundField)
                                break;
                        }
                    }
                    if (!foundField)
                    {
                        reportError("emitStore: field '" + fieldName + "' not found in class: " + className);
                        return nullptr;
                    }
                    // (AR) تحديث نوع الهيكل مع الصنف الجديد
                    structIt = context_info_.classStructTypes.find(className);
                    if (structIt == context_info_.classStructTypes.end())
                    {
                        reportError("emitStore: struct type not found for class: " + className);
                        return nullptr;
                    }
                    structType = structIt->second;
                }

                // (AR) إضافة إزاحة vtable — الحقل 0 في الهيكل هو مؤشر vtable
                // (EN) Add vtable offset — field 0 in struct is vtable pointer
                fieldIndex = getFieldStructIndex(className, fieldIndex);

                // (AR) إذا كان الكائن لا يزال i64 — حوّله إلى ptr
                // (EN) If object is still i64 — cast to ptr for GEP
                if (objPtr->getType()->isIntegerTy())
                {
                    objPtr = builder_->CreateIntToPtr(objPtr,
                                                      llvm::PointerType::getUnqual(*context_), objName + ".i2p");
                }

                // إنشاء GEP للحقل
                llvm::Value *gep = builder_->CreateStructGEP(structType, objPtr, fieldIndex, fieldName + "_gep");

                // تحويل النوع إذا لزم الأمر
                llvm::Type *fieldType = structType->getElementType(fieldIndex);
                if (value->getType() != fieldType)
                {
                    if (value->getType()->isIntegerTy() && fieldType->isIntegerTy())
                    {
                        value = builder_->CreateIntCast(value, fieldType, true, "cast");
                    }
                    else if (value->getType()->isPointerTy() && fieldType->isIntegerTy(64))
                    {
                        value = builder_->CreatePtrToInt(value, fieldType, "ptr2int");
                    }
                    else if (value->getType()->isIntegerTy() && fieldType->isPointerTy())
                    {
                        value = builder_->CreateIntToPtr(value, fieldType, "int2ptr");
                    }
                }

                auto *storeResult = builder_->CreateStore(value, gep);
#ifndef NDEBUG
                std::cout << "[DEBUG] emitStore: field '" << fieldName << "' stored via GEP index " << fieldIndex << std::endl;
#endif
                return storeResult;
            }

            // ================================================================
            // (AR) حالة تخزين عادية: 2 operands
            // (EN) Normal store case: 2 operands
            // ================================================================

            // (AR) احصل على القيمة المراد تخزينها
            // (EN) Get value to store - use resolveOperand for all types
            llvm::Value *value = resolveOperand(inst->operands[0]);
            const auto &valueOp = inst->operands[0];

            // (AR) احصل على المؤشر للتخزين فيه
            // (EN) Get pointer to store into
            const std::string &ptrName = inst->operands[1].name;
            llvm::Value *ptr = nullptr;

            // (AR) البحث بـ find() بدلاً من [] لتجنب إدخال nullptr في الخريطة
            // (EN) Use find() instead of [] to avoid inserting nullptr into the map
            auto ptrIt = context_info_.namedValues.find(ptrName);
            if (ptrIt != context_info_.namedValues.end())
            {
                ptr = ptrIt->second;
            }

            // (AR) إذا لم نجد المؤشر في namedValues، نبحث في المتغيرات العامة
            // (EN) If pointer not found in namedValues, search global variables
            if (!ptr)
            {
                // (AR) البحث بالاسم الأصلي أولاً
                // (EN) Search with original name first
                llvm::GlobalVariable *gv = module_->getGlobalVariable(ptrName);
                // (AR) إذا لم نجد، نحاول بإزالة % من المقدمة (السجلات تبدأ بـ % لكن المتغيرات العامة لا)
                // (EN) If not found, try without leading % (registers start with % but globals don't)
                if (!gv && !ptrName.empty() && ptrName[0] == '%')
                {
                    gv = module_->getGlobalVariable(ptrName.substr(1));
                }
                if (gv)
                {
                    ptr = gv; // (AR) استخدام المتغير العام مباشرة كمؤشر
                }
            }

            // (AR) بديل ثانٍ: البحث في الخريطة الدائمة للمتغيرات العالمية
            // (EN) Second fallback: search persistent globalValues map
            if (!ptr)
            {
                std::string cleanName = ptrName;
                if (!cleanName.empty() && cleanName[0] == '%')
                {
                    cleanName = cleanName.substr(1);
                }
                auto git = context_info_.globalValues.find(cleanName);
                if (git == context_info_.globalValues.end())
                {
                    git = context_info_.globalValues.find(ptrName);
                }
                if (git != context_info_.globalValues.end() && git->second != nullptr)
                {
                    ptr = git->second;
                }
            }

            if (!value || !ptr)
            {
                // (AR) إذا لم نجد المؤشر، ننشئ alloca جديداً — متغير محلي لم يُصرَّح عنه بتعليمة ALLOCA منفصلة
                // (EN) If pointer not found, create a new alloca — local variable without prior ALLOCA instruction
                if (value && !ptr && !ptrName.empty())
                {
                    llvm::Function *currentFunc = builder_->GetInsertBlock()->getParent();
                    llvm::IRBuilder<> entryBuilder(&currentFunc->getEntryBlock(),
                                                   currentFunc->getEntryBlock().begin());
                    // (AR) استخدام نوع القيمة الفعلي بدلاً من تحويل ptr إلى i64
                    //      النصوص (ptr) يجب أن تبقى ptr وليس i64
                    //      الأعداد الصحيحة (i64) تبقى i64
                    //      الأعداد العشرية (double) تبقى double
                    // (EN) Use actual value type instead of converting ptr to i64
                    //      Strings (ptr) must remain ptr not i64
                    llvm::Type *allocType = value->getType();
                    llvm::AllocaInst *newAlloca = entryBuilder.CreateAlloca(allocType, nullptr, ptrName);
                    context_info_.namedValues[ptrName] = newAlloca;
                    ptr = newAlloca;
                }
                if (!value || !ptr)
                {
                    reportError("Operands not found for store: value=" + valueOp.name + ", ptr=" + ptrName);
                    return nullptr;
                }
            }

            // ================================================================
            // معالجة تخزين مؤشر كائن في متغير i64
            // Handle storing object pointer into i64 variable
            // ================================================================
            if (value && ptr)
            {
                if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(ptr))
                {
                    // (AR) إذا كانت القيمة مؤشر (ptr) والهدف i64، نحول ptr→i64
                    // (EN) If value is pointer (ptr) and target is i64, convert ptr→i64
                    if (value->getType()->isPointerTy() &&
                        allocaInst->getAllocatedType()->isIntegerTy(64))
                    {
                        value = builder_->CreatePtrToInt(value, getInt64Type(), "obj.ptrtoint");
                    }
                }
                else if (auto *gv = llvm::dyn_cast<llvm::GlobalVariable>(ptr))
                {
                    // (AR) تخزين مؤشر في متغير عام i64 — تحويل ptr→i64
                    // (EN) Storing pointer into global i64 variable — convert ptr→i64
                    if (value->getType()->isPointerTy() && gv->getValueType()->isIntegerTy(64))
                    {
                        value = builder_->CreatePtrToInt(value, getInt64Type(), "obj.glob.ptrtoint");
                    }
                    // ================================================================
                    // (AR) [Fix #46] تخزين double في متغير عام i64 — ترقية المتغير العام إلى double
                    //      بدلاً من قطع الجزء العشري، ننشئ متغيراً عاماً جديداً بنوع double
                    //      لمطابقة سلوك المفسر (duck typing ديناميكي).
                    // (EN) [Fix #46] Storing double into global i64 — promote global to double
                    //      Instead of FPToSI truncation, create a new global of double type.
                    // ================================================================
                    else if (value->getType()->isDoubleTy() && gv->getValueType()->isIntegerTy(64))
                    {
                        // (AR) ترقية المتغير العام: حذف القديم + إنشاء جديد بنفس الاسم بنوع double
                        // (EN) Promote global: create new global with double type
                        std::string globalName = gv->getName().str();
                        llvm::Value *oldVal = builder_->CreateLoad(getInt64Type(), gv, "old.glob.i64");

                        // (AR) إنشاء متغير عام جديد بنوع double
                        // (EN) Create new global variable with double type
                        auto *newGV = new llvm::GlobalVariable(
                            *module_, builder_->getDoubleTy(), false,
                            llvm::GlobalValue::InternalLinkage,
                            llvm::ConstantFP::get(builder_->getDoubleTy(), 0.0),
                            globalName + ".f64");

                        // (AR) نقل القيمة القديمة إلى المتغير الجديد
                        // (EN) Migrate old value to new global
                        llvm::Value *oldAsF64 = builder_->CreateSIToFP(oldVal, builder_->getDoubleTy(), "old.glob.f64");
                        builder_->CreateStore(oldAsF64, newGV);

                        // (AR) استبدال المرجع في globalValues و namedValues
                        // (EN) Replace references in globalValues and namedValues
                        for (auto &kv : context_info_.globalValues)
                        {
                            if (kv.second == gv)
                            {
                                kv.second = newGV;
                                break;
                            }
                        }
                        for (auto &kv : context_info_.namedValues)
                        {
                            if (kv.second == gv)
                            {
                                kv.second = newGV;
                                break;
                            }
                        }
                        ptr = newGV;
                        // (AR) لا حاجة لتحويل — القيمة double والمتغير الآن double
                        // (EN) No conversion needed — value is double and global is now double
                    }
                    // (AR) تخزين i64 في متغير عام double — تحويل i64→double (SIToFP)
                    // (EN) Storing i64 into global double variable — convert i64→double (SIToFP)
                    else if (value->getType()->isIntegerTy(64) && gv->getValueType()->isDoubleTy())
                    {
                        value = builder_->CreateSIToFP(value, builder_->getDoubleTy(), "i64.to.f64.store");
                    }
                }

                // (AR) التحقق من تطابق الأنواع لمتغيرات alloca المحلية أيضاً
                // (EN) Check type compatibility for local alloca variables too
                if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(ptr))
                {
                    // ================================================================
                    // (AR) [Fix #46] تخزين double في alloca i64 — ترقية alloca إلى double
                    //      هذا يحدث عند `عدد3 = عدد3 / 10` حيث OP_DIVIDE ينتج double
                    //      لكن المتغير كان i64. بدلاً من قطع الجزء العشري (FPToSI)،
                    //      نُرقّي الـ alloca نفسه إلى double لمطابقة سلوك المفسر.
                    //      المفسر يحتفظ بالقيمة العشرية — المترجم يجب أن يفعل نفس الشيء.
                    // (EN) [Fix #46] Storing double in i64 alloca — promote alloca to double
                    //      Instead of FPToSI truncation, promote the alloca type to double
                    //      to match interpreter behavior (dynamic typing).
                    // ================================================================
                    if (value->getType()->isDoubleTy() &&
                        allocaInst->getAllocatedType()->isIntegerTy(64))
                    {
                        // (AR) ترقية نوع alloca: إنشاء alloca جديد double واستبدال القديم
                        // (EN) Promote alloca type: create new double alloca and replace old one
                        llvm::Function *currentFunc = builder_->GetInsertBlock()->getParent();
                        llvm::IRBuilder<> entryBuilder(&currentFunc->getEntryBlock(),
                                                       currentFunc->getEntryBlock().begin());
                        std::string allocaName = allocaInst->getName().str();
                        llvm::AllocaInst *newAlloca = entryBuilder.CreateAlloca(
                            builder_->getDoubleTy(), nullptr, allocaName + ".f64");

                        // (AR) نقل القيمة القديمة (i64) إلى الـ alloca الجديد (double)
                        // (EN) Migrate old value (i64) to new alloca (double)
                        llvm::Value *oldVal = builder_->CreateLoad(getInt64Type(), allocaInst, "old.i64.val");
                        llvm::Value *oldAsF64 = builder_->CreateSIToFP(oldVal, builder_->getDoubleTy(), "old.as.f64");
                        builder_->CreateStore(oldAsF64, newAlloca);

                        // (AR) استبدال المرجع في namedValues + تحديث ptr
                        // (EN) Replace reference in namedValues + update ptr
                        for (auto &kv : context_info_.namedValues)
                        {
                            if (kv.second == allocaInst)
                            {
                                kv.second = newAlloca;
                                break;
                            }
                        }
                        ptr = newAlloca;
                        // (AR) لا حاجة لتحويل — القيمة بالفعل double والـ alloca الآن double
                        // (EN) No conversion needed — value is already double and alloca is now double
                    }
                    // (AR) تخزين i64 في alloca double — تحويل i64→double
                    // (EN) Storing i64 into double alloca — convert i64→double
                    else if (value->getType()->isIntegerTy(64) &&
                             allocaInst->getAllocatedType()->isDoubleTy())
                    {
                        value = builder_->CreateSIToFP(value, builder_->getDoubleTy(), "i64.to.f64.alloca");
                    }
                    // (AR) تخزين i1 (bool) في alloca i64 — توسيع i1→i64
                    // (EN) Storing i1 (bool) into i64 alloca — zero-extend i1→i64
                    else if (value->getType()->isIntegerTy(1) &&
                             allocaInst->getAllocatedType()->isIntegerTy(64))
                    {
                        value = builder_->CreateZExt(value, getInt64Type(), "i1.to.i64.alloca");
                    }
                }
            }

            auto *storeResult = builder_->CreateStore(value, ptr);

            // ================================================================
            // نشر خريطة الأصناف عند تخزين كائن في متغير
            // Propagate class map when storing object into variable
            // ================================================================
            if (context_info_.objectClassMap.count(valueOp.name) &&
                !context_info_.objectClassMap.count(ptrName))
            {
                context_info_.objectClassMap[ptrName] = context_info_.objectClassMap[valueOp.name];
#ifndef NDEBUG
                std::cout << "[DEBUG] emitStore: propagated class '"
                          << context_info_.objectClassMap[valueOp.name]
                          << "' from " << valueOp.name << " to " << ptrName << std::endl;
#endif
            }

            // م-أ03: فحص إذا كان المتغير متطايراً (volatile) — لسجلات الأجهزة MMIO
            if (inst->operands[1].name.find("volatile") != std::string::npos ||
                inst->operands[1].name.find("\xd9\x85\xd8\xaa\xd8\xb7\xd8\xa7\xd9\x8a\xd8\xb1") != std::string::npos)
            {
                storeResult->setVolatile(true);
            }

            return storeResult;
        }

    } // namespace LLVM
} // namespace Sad
