/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include "sir_constants.h"
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

// ============================================================================
// (AR) عمليات الكائنات والمؤشرات - ObjectNew, ObjectGet, ObjectSet, Addr, PtrAdd, PtrCast
// (EN) Object and pointer operations - ObjectNew, ObjectGet, ObjectSet, Addr, PtrAdd, PtrCast
// (AR) تم فصل هذا الملف عن llvm_codegen_concurrency.cpp وفق قاعدة CW-05
// ============================================================================
        llvm::Value *LLVMCodeGen::emitAddr(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("ADDR requires 1 operand");
                return nullptr;
            }
            // Return the alloca pointer itself (not loading the value)
            llvm::Value *ptr = context_info_.namedValues[inst->operands[0].name];
            if (!ptr)
            {
                ptr = resolveOperand(inst->operands[0]);
            }
            if (!ptr)
                return nullptr;

            // Convert pointer to i64 if needed
            llvm::Value *result = ptr;
            if (ptr->getType()->isPointerTy())
            {
                result = builder_->CreatePtrToInt(ptr, getInt64Type(), "addr");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitPtrAdd(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("PTR_ADD requires 2 operands (ptr, offset)");
                return nullptr;
            }
            llvm::Value *ptr = resolveOperand(inst->operands[0]);
            llvm::Value *offset = resolveOperand(inst->operands[1]);
            if (!ptr || !offset)
                return nullptr;

            // If ptr is an integer, convert to pointer
            if (ptr->getType()->isIntegerTy())
            {
                ptr = builder_->CreateIntToPtr(ptr, llvm::PointerType::getUnqual(*context_), "ptr.conv");
            }

            // GEP with i8 element type for byte-level offset
            llvm::Value *result = builder_->CreateGEP(
                llvm::Type::getInt8Ty(*context_), ptr, {offset}, "ptr_add");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitPtrCast(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("PTR_CAST requires 1 operand");
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // In opaque pointer world, pointer casts are essentially no-ops
            // but we may need int-to-ptr or ptr-to-int
            llvm::Value *result = val;
            if (val->getType()->isIntegerTy())
            {
                result = builder_->CreateIntToPtr(val, llvm::PointerType::getUnqual(*context_), "ptr_cast");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // Phase N: OOP Instructions / تعليمات البرمجة الكائنية
        // ============================================================================

        llvm::Value *LLVMCodeGen::emitObjectNew(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("OBJECT_NEW requires class name operand");
                return nullptr;
            }

            std::string className = inst->operands[0].name;

            // Look up class struct type
            auto structIt = context_info_.classStructTypes.find(className);
            if (structIt == context_info_.classStructTypes.end())
            {
                reportError("Class not found: " + className);
                return nullptr;
            }

            llvm::StructType *structType = structIt->second;

            // Allocate on heap using malloc for objects (they may outlive the scope)
            auto *dlSize = llvm::ConstantExpr::getSizeOf(structType);
            auto *mallocType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*context_), {getInt64Type()}, false);
            auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *rawPtr = builder_->CreateCall(mallocFunc, {dlSize}, className + "_new");

            // Zero-initialize the object
            auto *memsetType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*context_),
                {llvm::PointerType::getUnqual(*context_),
                 llvm::Type::getInt32Ty(*context_), getInt64Type()},
                false);
            auto memsetFunc = module_->getOrInsertFunction("memset", memsetType);
            builder_->CreateCall(memsetFunc, {rawPtr,
                                              llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0),
                                              dlSize});

            // ═══════════════════════════════════════════════════════════════════════
            // (AR) تهيئة حقول المصفوفات — إصلاح حرج لمنع انهيار null pointer
            //      حقول المصفوفة المُهيَّأة بـ [] تحتاج تخصيص SadArray فعلي
            //      بدلاً من ترك المؤشر صفرياً بعد memset(0)
            // (EN) Initialize array fields — critical fix to prevent null pointer crash
            //      Array fields initialized with [] need actual SadArray allocation
            //      instead of leaving the pointer null after memset(0)
            // ═══════════════════════════════════════════════════════════════════════
            if (sirModule_)
            {
                auto sirClass = sirModule_->getClass(className);
                if (sirClass && !sirClass->arrayFields_.empty())
                {
                    // (AR) بنية SadArray: {i64 length, i64 capacity, ptr data}
                    // (EN) SadArray struct: {i64 length, i64 capacity, ptr data}
                    llvm::StructType *arrTy = llvm::StructType::create(*context_, {getInt64Type(), getInt64Type(), llvm::PointerType::getUnqual(*context_)}, "SadArray.init");
                    auto *arrStructSize = llvm::ConstantExpr::getSizeOf(arrTy);
                    auto i64Ty = getInt64Type();
                    auto *ptrTy = llvm::PointerType::getUnqual(*context_);

                    int fieldIdx = 0;
                    for (const auto &fieldName : sirClass->fieldOrder_)
                    {
                        if (sirClass->isArrayField(fieldName))
                        {
                            // (AR) تخصيص SadArray {length=0, capacity=8, data=malloc(8*ptrsize)}
                            // (EN) Allocate SadArray {length=0, capacity=8, data=malloc(8*ptrsize)}
                            llvm::Value *arrPtr = builder_->CreateCall(
                                mallocFunc, {builder_->CreateIntCast(arrStructSize, i64Ty, false)},
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
                                mallocFunc, {dataSize}, fieldName + ".data");
                            llvm::Value *dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, fieldName + ".datagep");
                            builder_->CreateStore(dataPtr, dataGep);

                            // (AR) تخزين مؤشر المصفوفة في حقل الكائن (fieldIdx + 1 بسبب vtable في الحقل 0)
                            // (EN) Store array pointer in object field (fieldIdx + 1 because vtable is field 0)
                            llvm::Value *objFieldGep = builder_->CreateStructGEP(
                                structType, rawPtr, fieldIdx + 1, fieldName + ".objfield");
                            builder_->CreateStore(arrPtr, objFieldGep);
                        }
                        fieldIdx++;
                    }
                }
            }

            // (AR) تخزين مؤشر vtable في الحقل 0
            // (EN) Store vtable pointer in field 0
            storeVtablePtr(rawPtr, className);

            // Track class association
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = rawPtr;
                context_info_.objectClassMap[inst->result->name] = className;
            }

            return rawPtr;
        }

        llvm::Value *LLVMCodeGen::emitObjectGet(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("OBJECT_GET requires 2 operands (object, field_name)");
                return nullptr;
            }

            std::string objRegName = inst->operands[0].name;
            std::string fieldName = inst->operands[1].name;

            // Find object pointer
            llvm::Value *objPtr = context_info_.namedValues[objRegName];
            if (!objPtr)
            {
                // (AR) Fallback: البحث في المتغيرات العامة LLVM
                // (EN) Fallback: search in LLVM global variables
                auto *globalVar = module_->getNamedGlobal(objRegName);
                // (AR) إذا لم نجد، نحاول بإزالة % من المقدمة
                // (EN) If not found, try without leading %
                if (!globalVar && !objRegName.empty() && objRegName[0] == '%')
                {
                    globalVar = module_->getNamedGlobal(objRegName.substr(1));
                }
                if (globalVar)
                {
                    llvm::Value *loaded = builder_->CreateLoad(getInt64Type(), globalVar, objRegName + ".glob.load");
                    objPtr = builder_->CreateIntToPtr(loaded,
                                                      llvm::PointerType::getUnqual(*context_), objRegName + ".glob.ptr");
                    context_info_.namedValues[objRegName] = objPtr;
                }
                else
                {
                    reportError("Object not found: " + objRegName);
                    return nullptr;
                }
            }

            // Look up class mapping
            auto classIt = context_info_.objectClassMap.find(objRegName);
            std::string className;
            if (classIt != context_info_.objectClassMap.end())
            {
                className = classIt->second;
            }

            // (AR) Fallback 0: البحث بدون % في objectClassMap
            // (EN) Fallback 0: search objectClassMap without leading %
            if (className.empty() && !objRegName.empty() && objRegName[0] == '%')
            {
                auto classIt2 = context_info_.objectClassMap.find(objRegName.substr(1));
                if (classIt2 != context_info_.objectClassMap.end())
                {
                    className = classIt2->second;
                    context_info_.objectClassMap[objRegName] = className;
                }
            }

            // (AR) Fallback 0b: إذا كان alloca لـ StructType، نستنتج الصنف
            // (EN) Fallback 0b: if alloca of StructType, infer class from struct type
            if (className.empty())
            {
                llvm::Value *objCheck = context_info_.namedValues[objRegName];
                if (objCheck)
                {
                    if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(objCheck))
                    {
                        if (auto *st = llvm::dyn_cast<llvm::StructType>(allocaInst->getAllocatedType()))
                        {
                            for (const auto &[clsName, clsSt] : context_info_.classStructTypes)
                            {
                                if (clsSt == st)
                                {
                                    className = clsName;
                                    context_info_.objectClassMap[objRegName] = className;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            // (AR) Fallback: استنتاج الصنف من اسم الحقل
            // (EN) Fallback: infer class from field name
            if (className.empty())
            {
                for (const auto &[clsName, fieldVec] : context_info_.classFieldNames)
                {
                    for (const auto &fn : fieldVec)
                    {
                        if (fn == fieldName)
                        {
                            className = clsName;
                            context_info_.objectClassMap[objRegName] = className;
                            break;
                        }
                    }
                    if (!className.empty())
                        break;
                }
            }

            if (className.empty())
            {
                reportError("No class mapping for: " + objRegName);
                return nullptr;
            }

            // (AR) فحص الخاصية: إذا وُجدت دالة __get_fieldName → استدعاؤها بدلاً من الوصول المباشر
            // (EN) Property check: if __get_fieldName exists → call it instead of direct access
            {
                std::string getterName = className + ".__get_" + fieldName;
                llvm::Function *getter = module_->getFunction(getterName);
                if (!getter)
                    getter = module_->getFunction("__get_" + fieldName);
                if (getter)
                {
                    llvm::Value *result = builder_->CreateCall(getter, {objPtr},
                                                               getter->getReturnType()->isVoidTy() ? "" : (fieldName + ".prop"));
                    if (inst->result.has_value())
                    {
                        context_info_.namedValues[inst->result->name] = result;
                    }
                    return result;
                }
            }

            auto structIt = context_info_.classStructTypes.find(className);
            auto fieldNamesIt = context_info_.classFieldNames.find(className);

            if (structIt == context_info_.classStructTypes.end() ||
                fieldNamesIt == context_info_.classFieldNames.end())
            {
                reportError("Class struct not found: " + className);
                return nullptr;
            }

            llvm::StructType *structType = structIt->second;
            const auto &fieldNames = fieldNamesIt->second;

            // Find field index
            int fieldIndex = -1;
            for (size_t i = 0; i < fieldNames.size(); i++)
            {
                if (fieldNames[i] == fieldName)
                {
                    fieldIndex = static_cast<int>(i);
                    break;
                }
            }

            // (AR) إذا لم يُعثر على الحقل، ابحث في سلسلة الوراثة
            // (EN) If field not found, search inheritance chain (safety fallback)
            if (fieldIndex < 0)
            {
                std::string parentClass = className;
                while (fieldIndex < 0)
                {
                    auto parentIt = context_info_.classParentMap.find(parentClass);
                    if (parentIt == context_info_.classParentMap.end())
                        break;
                    parentClass = parentIt->second;

                    auto parentFieldsIt = context_info_.classFieldNames.find(parentClass);
                    auto parentStructIt = context_info_.classStructTypes.find(parentClass);
                    if (parentFieldsIt == context_info_.classFieldNames.end() ||
                        parentStructIt == context_info_.classStructTypes.end())
                        break;

                    const auto &parentFieldNames = parentFieldsIt->second;
                    for (size_t i = 0; i < parentFieldNames.size(); i++)
                    {
                        if (parentFieldNames[i] == fieldName)
                        {
                            // (AR) الحقل موجود في الأب - مؤشره نفسه في الابن (الحقول الموروثة أولاً)
                            // (EN) Field found in parent - same index in child (inherited fields come first)
                            fieldIndex = static_cast<int>(i);
                            break;
                        }
                    }
                }
            }

            // (AR) الحقل غير موجود — نبحث في كل الأصناف كـ fallback
            // (EN) Field not found — search all classes as fallback
            if (fieldIndex < 0)
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
                            auto newStructIt = context_info_.classStructTypes.find(clsName);
                            if (newStructIt != context_info_.classStructTypes.end())
                            {
                                structType = newStructIt->second;
                            }
                            break;
                        }
                    }
                    if (fieldIndex >= 0)
                        break;
                }
            }

            if (fieldIndex < 0)
            {
                reportError("Field '" + fieldName + "' not found in class '" + className + "' or its parents");
                return nullptr;
            }

            // Resolve object pointer (may need loading from alloca)
            llvm::Value *actualObj = objPtr;
            if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(objPtr))
            {
                if (!allocaInst->getAllocatedType()->isStructTy())
                {
                    llvm::Value *loaded = builder_->CreateLoad(
                        allocaInst->getAllocatedType(), allocaInst, objRegName + ".load");
                    if (loaded->getType()->isIntegerTy())
                    {
                        actualObj = builder_->CreateIntToPtr(loaded,
                                                             llvm::PointerType::getUnqual(*context_), objRegName + ".ptr");
                    }
                }
            }
            // (AR) إذا كان الكائن لا يزال i64 (مثلاً من array_get) — حوّله إلى ptr
            // (EN) If object is still i64 (e.g. from array_get) — cast to ptr
            if (actualObj->getType()->isIntegerTy())
            {
                actualObj = builder_->CreateIntToPtr(actualObj,
                                                     llvm::PointerType::getUnqual(*context_), objRegName + ".i2p");
            }

            // (AR) إزاحة vtable: الحقل 0 في الهيكل محجوز لمؤشر vtable
            // (EN) vtable offset: field 0 in struct is reserved for vtable ptr
            int structIndex = getFieldStructIndex(className, fieldIndex);

            // GEP + Load
            llvm::Value *gep = builder_->CreateStructGEP(structType, actualObj, structIndex,
                                                         fieldName + "_gep");
            llvm::Type *fieldType = structType->getElementType(structIndex);
            llvm::Value *result = builder_->CreateLoad(fieldType, gep, fieldName + ".val");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitObjectSet(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
            {
                reportError("OBJECT_SET requires 3 operands (object, field_name, value)");
                return nullptr;
            }

            std::string objRegName = inst->operands[0].name;
            std::string fieldName = inst->operands[1].name;
            llvm::Value *value = resolveOperand(inst->operands[2]);

            llvm::Value *objPtr = context_info_.namedValues[objRegName];
            if (!objPtr || !value)
            {
                reportError("Operands not found for OBJECT_SET");
                return nullptr;
            }

            auto classIt = context_info_.objectClassMap.find(objRegName);
            std::string className;
            if (classIt != context_info_.objectClassMap.end())
            {
                className = classIt->second;
            }

            // (AR) Fallback 0: البحث بدون % في objectClassMap
            // (EN) Fallback 0: search objectClassMap without leading %
            if (className.empty() && !objRegName.empty() && objRegName[0] == '%')
            {
                auto classIt2 = context_info_.objectClassMap.find(objRegName.substr(1));
                if (classIt2 != context_info_.objectClassMap.end())
                {
                    className = classIt2->second;
                    context_info_.objectClassMap[objRegName] = className;
                }
            }

            // (AR) Fallback 0b: إذا كان alloca لـ StructType، نستنتج الصنف
            // (EN) Fallback 0b: if alloca of StructType, infer class from struct type
            if (className.empty())
            {
                if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(objPtr))
                {
                    if (auto *st = llvm::dyn_cast<llvm::StructType>(allocaInst->getAllocatedType()))
                    {
                        for (const auto &[clsName, clsSt] : context_info_.classStructTypes)
                        {
                            if (clsSt == st)
                            {
                                className = clsName;
                                context_info_.objectClassMap[objRegName] = className;
                                break;
                            }
                        }
                    }
                }
            }

            // (AR) Fallback: استنتاج الصنف من اسم الحقل
            // (EN) Fallback: infer class from field name
            if (className.empty())
            {
                for (const auto &[clsName, fieldVec] : context_info_.classFieldNames)
                {
                    for (const auto &fn : fieldVec)
                    {
                        if (fn == fieldName)
                        {
                            className = clsName;
                            context_info_.objectClassMap[objRegName] = className;
                            break;
                        }
                    }
                    if (!className.empty())
                        break;
                }
            }

            if (className.empty())
            {
                reportError("No class mapping for: " + objRegName);
                return nullptr;
            }

            // (AR) فحص الخاصية: إذا وُجدت دالة __set_fieldName → استدعاؤها بدلاً من التعيين المباشر
            // (EN) Property check: if __set_fieldName exists → call it instead of direct store
            {
                std::string setterName = className + ".__set_" + fieldName;
                llvm::Function *setter = module_->getFunction(setterName);
                if (!setter)
                    setter = module_->getFunction("__set_" + fieldName);
                if (setter)
                {
                    builder_->CreateCall(setter, {objPtr, value});
                    return value;
                }
            }

            auto structIt = context_info_.classStructTypes.find(className);
            auto fieldNamesIt = context_info_.classFieldNames.find(className);

            if (structIt == context_info_.classStructTypes.end() ||
                fieldNamesIt == context_info_.classFieldNames.end())
            {
                reportError("Class struct not found: " + className);
                return nullptr;
            }

            llvm::StructType *structType = structIt->second;
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

            // (AR) إذا لم يُعثر على الحقل، ابحث في سلسلة الوراثة
            // (EN) If field not found, search inheritance chain (safety fallback)
            if (fieldIndex < 0)
            {
                std::string parentClass = className;
                while (fieldIndex < 0)
                {
                    auto parentIt = context_info_.classParentMap.find(parentClass);
                    if (parentIt == context_info_.classParentMap.end())
                        break;
                    parentClass = parentIt->second;

                    auto parentFieldsIt = context_info_.classFieldNames.find(parentClass);
                    if (parentFieldsIt == context_info_.classFieldNames.end())
                        break;

                    const auto &parentFieldNames = parentFieldsIt->second;
                    for (size_t i = 0; i < parentFieldNames.size(); i++)
                    {
                        if (parentFieldNames[i] == fieldName)
                        {
                            fieldIndex = static_cast<int>(i);
                            break;
                        }
                    }
                }
            }

            // (AR) الحقل غير موجود — نبحث في كل الأصناف كـ fallback
            // (EN) Field not found — search all classes as fallback
            if (fieldIndex < 0)
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
                            auto newStructIt = context_info_.classStructTypes.find(clsName);
                            if (newStructIt != context_info_.classStructTypes.end())
                            {
                                structType = newStructIt->second;
                            }
                            break;
                        }
                    }
                    if (fieldIndex >= 0)
                        break;
                }
            }

            if (fieldIndex < 0)
            {
                reportError("Field '" + fieldName + "' not found in class '" + className + "' or its parents");
                return nullptr;
            }

            // Resolve object pointer
            llvm::Value *actualObj = objPtr;
            if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(objPtr))
            {
                if (!allocaInst->getAllocatedType()->isStructTy())
                {
                    llvm::Value *loaded = builder_->CreateLoad(
                        allocaInst->getAllocatedType(), allocaInst, objRegName + ".load");
                    if (loaded->getType()->isIntegerTy())
                    {
                        actualObj = builder_->CreateIntToPtr(loaded,
                                                             llvm::PointerType::getUnqual(*context_), objRegName + ".ptr");
                    }
                }
            }
            // (AR) إذا كان الكائن لا يزال i64 — حوّله إلى ptr
            // (EN) If object is still i64 — cast to ptr
            if (actualObj->getType()->isIntegerTy())
            {
                actualObj = builder_->CreateIntToPtr(actualObj,
                                                     llvm::PointerType::getUnqual(*context_), objRegName + ".i2p");
            }

            // (AR) إزاحة vtable: الحقل 0 في الهيكل محجوز لمؤشر vtable
            // (EN) vtable offset: field 0 in struct is reserved for vtable ptr
            int structIndex = getFieldStructIndex(className, fieldIndex);

            // GEP + Store
            llvm::Value *gep = builder_->CreateStructGEP(structType, actualObj, structIndex,
                                                         fieldName + "_gep");
            builder_->CreateStore(value, gep);

            return value;
        }

        // ====================================================================

    } // namespace LLVM
} // namespace Sad