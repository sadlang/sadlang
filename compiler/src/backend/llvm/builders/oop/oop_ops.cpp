/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/oop/oop_ops_codegen.h"
#include "builders/collections/array_ops_codegen.h" // SAD_ARRAY_SLOT_BYTES
#include "sad_dyn_repr.h" // (AR) ISSUE-063: تعليب/فكّ %SadDyn عند حقول الأصناف / (EN) %SadDyn pack/unpack at class fields
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

        bool OOPOpsCodeGen::fieldExistsInAnyClass(const std::string &fieldName) const
        {
            for (const auto &[clsName, fieldVec] : cg_.context_info_.classFieldNames)
            {
                for (const auto &fn : fieldVec)
                {
                    if (fn == fieldName)
                        return true;
                }
            }
            return false;
        }

        llvm::Value *OOPOpsCodeGen::emitAddr(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ADDR"}});
                return nullptr;
            }
            // Return the alloca pointer itself (not loading the value)
            llvm::Value *ptr = cg_.context_info_.namedValues[inst->operands[0].name];
            if (!ptr)
            {
                ptr = cg_.resolveOperand(inst->operands[0]);
            }
            if (!ptr)
                return nullptr;

            // Convert pointer to i64 if needed
            llvm::Value *result = ptr;
            if (ptr->getType()->isPointerTy())
            {
                result = cg_.builder_->CreatePtrToInt(ptr, cg_.getInt64Type(), "addr");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *OOPOpsCodeGen::emitPtrAdd(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "PTR_ADD"}});
                return nullptr;
            }
            llvm::Value *ptr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *offset = cg_.resolveOperand(inst->operands[1]);
            if (!ptr || !offset)
                return nullptr;

            // If ptr is an integer, convert to pointer
            if (ptr->getType()->isIntegerTy())
            {
                ptr = cg_.builder_->CreateIntToPtr(ptr, llvm::PointerType::getUnqual(*cg_.context_), "ptr.conv");
            }

            // GEP with i8 element type for byte-level offset
            llvm::Value *result = cg_.builder_->CreateGEP(
                llvm::Type::getInt8Ty(*cg_.context_), ptr, {offset}, "ptr_add");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *OOPOpsCodeGen::emitPtrCast(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "PTR_CAST"}});
                return nullptr;
            }
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // In opaque pointer world, pointer casts are essentially no-ops
            // but we may need int-to-ptr or ptr-to-int
            llvm::Value *result = val;
            if (val->getType()->isIntegerTy())
            {
                result = cg_.builder_->CreateIntToPtr(val, llvm::PointerType::getUnqual(*cg_.context_), "ptr_cast");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // Phase N: OOP Instructions / تعليمات البرمجة الكائنية
        // ============================================================================

        llvm::Value *OOPOpsCodeGen::emitObjectNew(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "OBJECT_NEW"}});
                return nullptr;
            }

            std::string className = inst->operands[0].name;

            // Look up class struct type
            auto structIt = cg_.context_info_.classStructTypes.find(className);
            if (structIt == cg_.context_info_.classStructTypes.end())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_UNDEFINED_REF, {{"detail", std::string("Class not found:") + className}});
                return nullptr;
            }

            llvm::StructType *structType = structIt->second;

            // Allocate on heap using malloc for objects (they may outlive the scope)
            auto *dlSize = llvm::ConstantExpr::getSizeOf(structType);
            llvm::Value *rawPtr = cg_.emitMalloc(dlSize, className + "_new");

            // Zero-initialize the object
            cg_.builder_->CreateMemSet(rawPtr,
                                   llvm::ConstantInt::get(llvm::Type::getInt8Ty(*cg_.context_), 0),
                                   dlSize, llvm::MaybeAlign(8));

            // ═══════════════════════════════════════════════════════════════════════
            // (AR) تهيئة حقول المصفوفات — إصلاح حرج لمنع انهيار null pointer
            //      حقول المصفوفة المُهيَّأة بـ [] تحتاج تخصيص SadArray فعلي
            //      بدلاً من ترك المؤشر صفرياً بعد memset(0)
            // (EN) Initialize array fields — critical fix to prevent null pointer crash
            //      Array fields initialized with [] need actual SadArray allocation
            //      instead of leaving the pointer null after memset(0)
            // ═══════════════════════════════════════════════════════════════════════
            if (cg_.sirModule_)
            {
                auto sirClass = cg_.sirModule_->getClass(className);
                if (sirClass && !sirClass->arrayFields_.empty())
                {
                    // (AR) بنية SadArray: {i64 length, i64 capacity, ptr data, ptr tags}
                    // (EN) SadArray struct: {i64 length, i64 capacity, ptr data, ptr tags}
                    llvm::StructType *arrTy = llvm::StructType::create(*cg_.context_, {cg_.getInt64Type(), cg_.getInt64Type(), llvm::PointerType::getUnqual(*cg_.context_), llvm::PointerType::getUnqual(*cg_.context_), cg_.getInt8Type()}, "SadArray.init"); // homogKind (option A2): DynKind of a homogeneous array; read only when tags==null
                    auto *arrStructSize = llvm::ConstantExpr::getSizeOf(arrTy);
                    auto i64Ty = cg_.getInt64Type();

                    int fieldIdx = 0;
                    for (const auto &fieldName : sirClass->fieldOrder_)
                    {
                        if (sirClass->isArrayField(fieldName))
                        {
                            // (AR) تخصيص SadArray {length=0, capacity=8, data=malloc(8*ptrsize)}
                            // (EN) Allocate SadArray {length=0, capacity=8, data=malloc(8*ptrsize)}
                            llvm::Value *arrPtr = cg_.emitMalloc(
                                cg_.builder_->CreateIntCast(arrStructSize, i64Ty, false),
                                fieldName + ".arr");

                            // length = 0
                            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, fieldName + ".len");
                            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), lenGep);

                            // capacity = 8
                            llvm::Value *capGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 1, fieldName + ".cap");
                            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 8), capGep);

                            // (AR) data = malloc(السعة 8 × حجم الخانة الموحَّد 8) —
                            //      لا getSizeOf(ptr) (=4 على i686 يخالف خطوة i64)
                            // (EN) data = malloc(capacity 8 × unified slot 8), not getSizeOf(ptr)
                            llvm::Value *dataSize = cg_.builder_->CreateMul(
                                llvm::ConstantInt::get(i64Ty, 8),
                                llvm::ConstantInt::get(i64Ty, SAD_ARRAY_SLOT_BYTES),
                                fieldName + ".datasz");
                            llvm::Value *dataPtr = cg_.emitMalloc(dataSize, fieldName + ".data");
                            llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, fieldName + ".datagep");
                            cg_.builder_->CreateStore(dataPtr, dataGep);

                            // (AR) tags = null (مصفوفة متجانسة ابتداءً؛ الوسمُ يُخصَّص كسولًا عند أوّل عنصر مختلط)
                            // (EN) tags = null (homogeneous initially; tags buffer lazily allocated on first mixed element)
                            llvm::Value *tagsGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 3, fieldName + ".tagsgep");
                            cg_.builder_->CreateStore(llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*cg_.context_)), tagsGep);

                            // (AR) الحقل ٤ (homogKind) = DynKind::Int افتراضًا (خامل — يُكتب ولا يُقرأ بعد)
                            // (EN) Field 4 (homogKind) = DynKind::Int default (inert — written, not yet read)
                            llvm::Value *hkGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 4, fieldName + ".homogkindgep");
                            cg_.builder_->CreateStore(llvm::ConstantInt::get(cg_.getInt8Type(), Sad::LLVM::DynKind::Int), hkGep);

                            // (AR) الإزاحة عبر getFieldStructIndex — تُسقِط ترويسة vtable لبنى @تمثيل_سي [RFC #53 F2-ب]
                            // (EN) Offset via getFieldStructIndex — drops the vtable header for @تمثيل_سي structs [RFC #53 F2-ب]
                            llvm::Value *objFieldGep = cg_.builder_->CreateStructGEP(
                                structType, rawPtr, cg_.getFieldStructIndex(className, fieldIdx),
                                fieldName + ".objfield");
                            cg_.builder_->CreateStore(arrPtr, objFieldGep);
                        }
                        fieldIdx++;
                    }
                }
            }

            // (AR) تخزين مؤشر vtable في الحقل 0
            // (EN) Store vtable pointer in field 0
            cg_.storeVtablePtr(rawPtr, className);

            // Track class association
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = rawPtr;
                cg_.context_info_.objectClassMap[inst->result->name] = className;
            }

            return rawPtr;
        }

        llvm::Value *OOPOpsCodeGen::emitObjectGet(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_FIELD_LAYOUT, {{"detail", "OBJECT_GET"}});
                return nullptr;
            }

            std::string objRegName = inst->operands[0].name;
            std::string fieldName = inst->operands[1].name;

            // Find object pointer
            llvm::Value *objPtr = cg_.context_info_.namedValues[objRegName];
            if (!objPtr)
            {
                // (AR) Fallback: البحث في المتغيرات العامة LLVM
                // (EN) Fallback: search in LLVM global variables
                auto *globalVar = cg_.module_->getNamedGlobal(objRegName);
                // (AR) إذا لم نجد، نحاول بإزالة % من المقدمة
                // (EN) If not found, try without leading %
                if (!globalVar && !objRegName.empty() && objRegName[0] == '%')
                {
                    globalVar = cg_.module_->getNamedGlobal(objRegName.substr(1));
                }
                if (globalVar)
                {
                    llvm::Value *loaded = cg_.builder_->CreateLoad(cg_.getInt64Type(), globalVar, objRegName + ".glob.load");
                    objPtr = cg_.builder_->CreateIntToPtr(loaded,
                                                      llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".glob.ptr");
                    cg_.context_info_.namedValues[objRegName] = objPtr;
                }
                else
                {
                    cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_UNDEFINED_REF, {{"detail", std::string("Object not found:") + objRegName}});
                    return nullptr;
                }
            }

            // Look up class mapping
            auto classIt = cg_.context_info_.objectClassMap.find(objRegName);
            std::string className;
            if (classIt != cg_.context_info_.objectClassMap.end())
            {
                className = classIt->second;
            }

            // (AR) Fallback 0: البحث بدون % في objectClassMap
            // (EN) Fallback 0: search objectClassMap without leading %
            if (className.empty() && !objRegName.empty() && objRegName[0] == '%')
            {
                auto classIt2 = cg_.context_info_.objectClassMap.find(objRegName.substr(1));
                if (classIt2 != cg_.context_info_.objectClassMap.end())
                {
                    className = classIt2->second;
                    cg_.context_info_.objectClassMap[objRegName] = className;
                }
            }

            // (AR) Fallback 0b: إذا كان alloca لـ StructType، نستنتج الصنف
            // (EN) Fallback 0b: if alloca of StructType, infer class from struct type
            if (className.empty())
            {
                llvm::Value *objCheck = cg_.context_info_.namedValues[objRegName];
                if (objCheck)
                {
                    if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(objCheck))
                    {
                        if (auto *st = llvm::dyn_cast<llvm::StructType>(allocaInst->getAllocatedType()))
                        {
                            for (const auto &[clsName, clsSt] : cg_.context_info_.classStructTypes)
                            {
                                if (clsSt == st)
                                {
                                    className = clsName;
                                    cg_.context_info_.objectClassMap[objRegName] = className;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            // (AR) Fallback: استنتاج الصنف من اسم الحقل
            // (EN) Fallback: infer class from field name
            // (AR) تُستثنى الأصناف المضمَّنة («حدث») وإلّا فازت حقولها (س/ص/قيمة…)
            //      بالتخمين على أصناف المستخدم ⇒ GEP بتخطيطها فوق كائنه (انحدار #251).
            // (EN) Builtin classes («حدث») are skipped, else their fields (x/y/value…)
            //      win the guess over user classes ⇒ wrong-layout GEP (#251 regression).
            if (className.empty())
            {
                for (const auto &[clsName, fieldVec] : cg_.context_info_.classFieldNames)
                {
                    if (cg_.context_info_.builtinClassNames.count(clsName))
                        continue;
                    for (const auto &fn : fieldVec)
                    {
                        if (fn == fieldName)
                        {
                            className = clsName;
                            cg_.context_info_.objectClassMap[objRegName] = className;
                            break;
                        }
                    }
                    if (!className.empty())
                        break;
                }
            }

            if (className.empty())
            {
                // (AR) تمييز عطب التخطيط عن الوصول الديناميكيّ: إن كان الحقل موجودًا في
                //      تخطيط صنفٍ معروف (بما فيها المضمَّنة) فتعذُّر الربط عطبُ تخطيطٍ
                //      حقيقيّ يُجهض الترجمة (بوّابة INT_SIR_FIELD_LAYOUT في السائق)؛
                //      وإلّا فهو وصولٌ ديناميكيّ لعضوٍ لا يعرفه أيّ صنف («م.الطول» على
                //      نصّ) — شأنُ زمنِ تشغيلٍ في المرجع (RUN033) لا يُجهض الترجمة.
                // (EN) Distinguish layout corruption from dynamic access: a member that
                //      exists in some known class layout (builtins included) failing to
                //      bind is a real layout defect (driver gate aborts); a member no
                //      class knows is dynamic access — a runtime matter in the reference
                //      engine (RUN033), which must not abort compilation.
                cg_.reportError(fieldExistsInAnyClass(fieldName)
                                    ? ::Sad::Errors::ErrorCode::INT_SIR_FIELD_LAYOUT
                                    : ::Sad::Errors::ErrorCode::INT_SIR_UNDEFINED_REF,
                                {{"detail", std::string("No class mapping for:") + objRegName}});
                return nullptr;
            }

            // (AR) تطبيع مؤشر الكائن: فك alloca/global إلى مؤشر كائن فعلي
            // (EN) Normalize object pointer: unwrap alloca/global into actual object pointer
            llvm::Value *normalizedObjPtr = objPtr;
            if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(normalizedObjPtr))
            {
                if (!allocaInst->getAllocatedType()->isStructTy())
                {
                    llvm::Value *loaded = cg_.builder_->CreateLoad(allocaInst->getAllocatedType(), allocaInst, objRegName + ".self.load");
                    if (loaded->getType()->isIntegerTy())
                    {
                        normalizedObjPtr = cg_.builder_->CreateIntToPtr(loaded,
                                                                    llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".self.ptr");
                    }
                    else if (loaded->getType()->isPointerTy())
                    {
                        normalizedObjPtr = loaded;
                    }
                }
            }
            else if (auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(normalizedObjPtr))
            {
                llvm::Value *loaded = cg_.builder_->CreateLoad(globalVar->getValueType(), globalVar, objRegName + ".self.gload");
                if (loaded->getType()->isIntegerTy())
                {
                    normalizedObjPtr = cg_.builder_->CreateIntToPtr(loaded,
                                                                llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".self.gptr");
                }
                else if (loaded->getType()->isPointerTy())
                {
                    normalizedObjPtr = loaded;
                }
            }
            if (normalizedObjPtr->getType()->isIntegerTy())
            {
                normalizedObjPtr = cg_.builder_->CreateIntToPtr(normalizedObjPtr,
                                                            llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".self.i2p");
            }

            // (AR) فحص الخاصية: إذا وُجدت دالة __get_fieldName → استدعاؤها بدلاً من الوصول المباشر
            // (EN) Property check: if __get_fieldName exists → call it instead of direct access
            {
                if (!fieldName.empty() && fieldName.front() == '"')
                    fieldName = fieldName.substr(1);
                if (!fieldName.empty() && fieldName.back() == '"')
                    fieldName = fieldName.substr(0, fieldName.size() - 1);

                std::string getterName = className + ".__get_" + fieldName;
                llvm::Function *getter = cg_.module_->getFunction(getterName);
                if (!getter)
                {
                    auto it = cg_.context_info_.functions.find(getterName);
                    if (it != cg_.context_info_.functions.end())
                        getter = it->second;
                }
                if (!getter)
                    getter = cg_.module_->getFunction("__get_" + fieldName);
                if (!getter)
                {
                    auto it = cg_.context_info_.functions.find("__get_" + fieldName);
                    if (it != cg_.context_info_.functions.end())
                        getter = it->second;
                }
                if (getter)
                {
                    llvm::Value *selfArg = normalizedObjPtr;
                    if (getter->arg_size() >= 1)
                    {
                        llvm::Type *expectedSelfTy = getter->getFunctionType()->getParamType(0);
                        if (expectedSelfTy->isIntegerTy() && selfArg->getType()->isPointerTy())
                        {
                            selfArg = cg_.builder_->CreatePtrToInt(selfArg, expectedSelfTy, "prop.get.self.p2i");
                        }
                        else if (expectedSelfTy->isPointerTy() && selfArg->getType()->isIntegerTy())
                        {
                            selfArg = cg_.builder_->CreateIntToPtr(selfArg, expectedSelfTy, "prop.get.self.i2p");
                        }
                    }

                    llvm::Value *result = cg_.builder_->CreateCall(getter, {selfArg},
                                                               getter->getReturnType()->isVoidTy() ? "" : (fieldName + ".prop"));
                    if (inst->result.has_value())
                    {
                        cg_.context_info_.namedValues[inst->result->name] = result;
                    }
                    return result;
                }
            }

            auto structIt = cg_.context_info_.classStructTypes.find(className);
            auto fieldNamesIt = cg_.context_info_.classFieldNames.find(className);

            if (structIt == cg_.context_info_.classStructTypes.end() ||
                fieldNamesIt == cg_.context_info_.classFieldNames.end())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_FIELD_LAYOUT, {{"detail", std::string("Class struct not found:") + className}});
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
                    auto parentIt = cg_.context_info_.classParentMap.find(parentClass);
                    if (parentIt == cg_.context_info_.classParentMap.end())
                        break;
                    parentClass = parentIt->second;

                    auto parentFieldsIt = cg_.context_info_.classFieldNames.find(parentClass);
                    auto parentStructIt = cg_.context_info_.classStructTypes.find(parentClass);
                    if (parentFieldsIt == cg_.context_info_.classFieldNames.end() ||
                        parentStructIt == cg_.context_info_.classStructTypes.end())
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
                // (AR) المضمَّنة («حدث») مستثناة من التخمين (انحدار #251)
                // (EN) Builtins («حدث») excluded from the guess (#251 regression)
                for (const auto &[clsName, fieldVec] : cg_.context_info_.classFieldNames)
                {
                    if (clsName == className ||
                        cg_.context_info_.builtinClassNames.count(clsName))
                        continue;
                    for (size_t i = 0; i < fieldVec.size(); i++)
                    {
                        if (fieldVec[i] == fieldName)
                        {
                            fieldIndex = static_cast<int>(i);
                            className = clsName;
                            auto newStructIt = cg_.context_info_.classStructTypes.find(clsName);
                            if (newStructIt != cg_.context_info_.classStructTypes.end())
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
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_FIELD_LAYOUT, {{"detail", std::string("Field '") + fieldName + "' not found in class '" + className + "' or its parents"}});
                return nullptr;
            }

            // Resolve object pointer (may need loading from alloca)
            llvm::Value *actualObj = normalizedObjPtr;
            if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(objPtr))
            {
                if (!allocaInst->getAllocatedType()->isStructTy())
                {
                    llvm::Value *loaded = cg_.builder_->CreateLoad(
                        allocaInst->getAllocatedType(), allocaInst, objRegName + ".load");
                    if (loaded->getType()->isIntegerTy())
                    {
                        actualObj = cg_.builder_->CreateIntToPtr(loaded,
                                                             llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".ptr");
                    }
                }
            }
            else if (auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(objPtr))
            {
                llvm::Value *loaded = cg_.builder_->CreateLoad(globalVar->getValueType(), globalVar, objRegName + ".load.g");
                if (loaded->getType()->isIntegerTy())
                {
                    actualObj = cg_.builder_->CreateIntToPtr(loaded,
                                                         llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".ptr.g");
                }
                else if (loaded->getType()->isPointerTy())
                {
                    actualObj = loaded;
                }
            }
            // (AR) إذا كان الكائن لا يزال i64 (مثلاً من array_get) — حوّله إلى ptr
            // (EN) If object is still i64 (e.g. from array_get) — cast to ptr
            if (actualObj->getType()->isIntegerTy())
            {
                actualObj = cg_.builder_->CreateIntToPtr(actualObj,
                                                     llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".i2p");
            }

            // (AR) إزاحة vtable: الحقل 0 في الهيكل محجوز لمؤشر vtable
            // (EN) vtable offset: field 0 in struct is reserved for vtable ptr
            int structIndex = cg_.getFieldStructIndex(className, fieldIndex);

            // GEP + Load
            llvm::Value *gep = cg_.builder_->CreateStructGEP(structType, actualObj, structIndex,
                                                         fieldName + "_gep");
            llvm::Type *fieldType = structType->getElementType(structIndex);
            llvm::Value *result = cg_.builder_->CreateLoad(fieldType, gep, fieldName + ".val");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *OOPOpsCodeGen::emitObjectSet(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_FIELD_LAYOUT, {{"detail", "OBJECT_SET"}});
                return nullptr;
            }

            std::string objRegName = inst->operands[0].name;
            std::string fieldName = inst->operands[1].name;
            llvm::Value *value = cg_.resolveOperand(inst->operands[2]);

            llvm::Value *objPtr = cg_.context_info_.namedValues[objRegName];
            if (!objPtr)
            {
                // (AR) Fallback: البحث عن الكائن في المتغيرات العامة LLVM
                // (EN) Fallback: resolve object from LLVM globals
                auto *globalVar = cg_.module_->getNamedGlobal(objRegName);
                if (!globalVar && !objRegName.empty() && objRegName[0] == '%')
                {
                    globalVar = cg_.module_->getNamedGlobal(objRegName.substr(1));
                }
                if (globalVar)
                {
                    llvm::Value *loaded = cg_.builder_->CreateLoad(cg_.getInt64Type(), globalVar, objRegName + ".glob.load");
                    objPtr = cg_.builder_->CreateIntToPtr(loaded,
                                                      llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".glob.ptr");
                    cg_.context_info_.namedValues[objRegName] = objPtr;
                }
            }

            if (!objPtr || !value)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            auto classIt = cg_.context_info_.objectClassMap.find(objRegName);
            std::string className;
            if (classIt != cg_.context_info_.objectClassMap.end())
            {
                className = classIt->second;
            }

            // (AR) Fallback 0: البحث بدون % في objectClassMap
            // (EN) Fallback 0: search objectClassMap without leading %
            if (className.empty() && !objRegName.empty() && objRegName[0] == '%')
            {
                auto classIt2 = cg_.context_info_.objectClassMap.find(objRegName.substr(1));
                if (classIt2 != cg_.context_info_.objectClassMap.end())
                {
                    className = classIt2->second;
                    cg_.context_info_.objectClassMap[objRegName] = className;
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
                        for (const auto &[clsName, clsSt] : cg_.context_info_.classStructTypes)
                        {
                            if (clsSt == st)
                            {
                                className = clsName;
                                cg_.context_info_.objectClassMap[objRegName] = className;
                                break;
                            }
                        }
                    }
                }
            }

            // (AR) Fallback: استنتاج الصنف من اسم الحقل
            // (EN) Fallback: infer class from field name
            // (AR) المضمَّنة («حدث») مستثناة من التخمين (انحدار #251)
            // (EN) Builtins («حدث») excluded from the guess (#251 regression)
            if (className.empty())
            {
                for (const auto &[clsName, fieldVec] : cg_.context_info_.classFieldNames)
                {
                    if (cg_.context_info_.builtinClassNames.count(clsName))
                        continue;
                    for (const auto &fn : fieldVec)
                    {
                        if (fn == fieldName)
                        {
                            className = clsName;
                            cg_.context_info_.objectClassMap[objRegName] = className;
                            break;
                        }
                    }
                    if (!className.empty())
                        break;
                }
            }

            if (className.empty())
            {
                // (AR) التمييز نفسه المطبَّق في emitObjectGet: حقلٌ يعرفه صنفٌ ما ⇒ عطب
                //      تخطيط مُجهِض؛ عضوٌ لا يعرفه أحد ⇒ ديناميكيّ (شأن زمن تشغيل)
                // (EN) Same distinction as emitObjectGet: known-in-some-class field ⇒
                //      aborting layout defect; unknown-to-all member ⇒ dynamic (runtime)
                cg_.reportError(fieldExistsInAnyClass(fieldName)
                                    ? ::Sad::Errors::ErrorCode::INT_SIR_FIELD_LAYOUT
                                    : ::Sad::Errors::ErrorCode::INT_SIR_UNDEFINED_REF,
                                {{"detail", std::string("No class mapping for:") + objRegName}});
                return nullptr;
            }

            // (AR) تطبيع مؤشر الكائن قبل استدعاء setter والوصول المباشر للحقل
            // (EN) Normalize object pointer before setter call and direct field store
            llvm::Value *normalizedObjPtr = objPtr;
            if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(normalizedObjPtr))
            {
                if (!allocaInst->getAllocatedType()->isStructTy())
                {
                    llvm::Value *loaded = cg_.builder_->CreateLoad(allocaInst->getAllocatedType(), allocaInst, objRegName + ".self.load");
                    if (loaded->getType()->isIntegerTy())
                    {
                        normalizedObjPtr = cg_.builder_->CreateIntToPtr(loaded,
                                                                    llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".self.ptr");
                    }
                    else if (loaded->getType()->isPointerTy())
                    {
                        normalizedObjPtr = loaded;
                    }
                }
            }
            else if (auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(normalizedObjPtr))
            {
                llvm::Value *loaded = cg_.builder_->CreateLoad(globalVar->getValueType(), globalVar, objRegName + ".self.gload");
                if (loaded->getType()->isIntegerTy())
                {
                    normalizedObjPtr = cg_.builder_->CreateIntToPtr(loaded,
                                                                llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".self.gptr");
                }
                else if (loaded->getType()->isPointerTy())
                {
                    normalizedObjPtr = loaded;
                }
            }
            if (normalizedObjPtr->getType()->isIntegerTy())
            {
                normalizedObjPtr = cg_.builder_->CreateIntToPtr(normalizedObjPtr,
                                                            llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".self.i2p");
            }

            // (AR) فحص الخاصية: إذا وُجدت دالة __set_fieldName → استدعاؤها بدلاً من التعيين المباشر
            // (EN) Property check: if __set_fieldName exists → call it instead of direct store
            {
                if (!fieldName.empty() && fieldName.front() == '"')
                    fieldName = fieldName.substr(1);
                if (!fieldName.empty() && fieldName.back() == '"')
                    fieldName = fieldName.substr(0, fieldName.size() - 1);

                std::string setterName = className + ".__set_" + fieldName;
                llvm::Function *setter = cg_.module_->getFunction(setterName);
                if (!setter)
                {
                    auto it = cg_.context_info_.functions.find(setterName);
                    if (it != cg_.context_info_.functions.end())
                        setter = it->second;
                }
                if (!setter)
                    setter = cg_.module_->getFunction("__set_" + fieldName);
                if (!setter)
                {
                    auto it = cg_.context_info_.functions.find("__set_" + fieldName);
                    if (it != cg_.context_info_.functions.end())
                        setter = it->second;
                }
                if (setter)
                {
                    llvm::Value *selfArg = normalizedObjPtr;
                    llvm::Value *valueArg = value;

                    if (setter->arg_size() >= 1)
                    {
                        llvm::Type *expectedSelfTy = setter->getFunctionType()->getParamType(0);
                        if (expectedSelfTy->isIntegerTy() && selfArg->getType()->isPointerTy())
                        {
                            selfArg = cg_.builder_->CreatePtrToInt(selfArg, expectedSelfTy, "prop.set.self.p2i");
                        }
                        else if (expectedSelfTy->isPointerTy() && selfArg->getType()->isIntegerTy())
                        {
                            selfArg = cg_.builder_->CreateIntToPtr(selfArg, expectedSelfTy, "prop.set.self.i2p");
                        }
                    }

                    if (setter->arg_size() >= 2)
                    {
                        llvm::Type *expectedValTy = setter->getFunctionType()->getParamType(1);
                        // (AR) ISSUE-063: مواءمة %SadDyn مع توقيع الـsetter (تعليب/فكّ)
                        // (EN) ISSUE-063: reconcile %SadDyn with the setter signature (pack/unpack)
                        {
                            llvm::StructType *dynTy = getSadDynType(*cg_.context_);
                            if (expectedValTy == dynTy && !isSadDyn(valueArg))
                            {
                                valueArg = toDyn(cg_, valueArg, inst->operands[2].dataType);
                            }
                            else if (expectedValTy != dynTy && isSadDyn(valueArg))
                            {
                                if (expectedValTy->isDoubleTy())
                                    valueArg = unpackDouble(cg_, valueArg);
                                else if (expectedValTy->isPointerTy())
                                    valueArg = unpackPtr(cg_, valueArg);
                                else
                                    valueArg = dynPayloadI64(cg_, valueArg);
                            }
                        }
                        llvm::Type *actualValTy = valueArg->getType();
                        if (expectedValTy != actualValTy)
                        {
                            if (expectedValTy->isIntegerTy() && actualValTy->isIntegerTy())
                            {
                                valueArg = cg_.builder_->CreateIntCast(valueArg, expectedValTy, true, "prop.set.val.icast");
                            }
                            else if (expectedValTy->isDoubleTy() && actualValTy->isIntegerTy())
                            {
                                valueArg = cg_.builder_->CreateSIToFP(valueArg, expectedValTy, "prop.set.val.i2f");
                            }
                            else if (expectedValTy->isIntegerTy() && actualValTy->isDoubleTy())
                            {
                                valueArg = cg_.builder_->CreateFPToSI(valueArg, expectedValTy, "prop.set.val.f2i");
                            }
                            else if (expectedValTy->isPointerTy() && actualValTy->isIntegerTy())
                            {
                                valueArg = cg_.builder_->CreateIntToPtr(valueArg, expectedValTy, "prop.set.val.i2p");
                            }
                            else if (expectedValTy->isIntegerTy() && actualValTy->isPointerTy())
                            {
                                valueArg = cg_.builder_->CreatePtrToInt(valueArg, expectedValTy, "prop.set.val.p2i");
                            }
                        }
                    }

                    cg_.builder_->CreateCall(setter, {selfArg, valueArg});
                    return value;
                }
            }

            auto structIt = cg_.context_info_.classStructTypes.find(className);
            auto fieldNamesIt = cg_.context_info_.classFieldNames.find(className);

            if (structIt == cg_.context_info_.classStructTypes.end() ||
                fieldNamesIt == cg_.context_info_.classFieldNames.end())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_FIELD_LAYOUT, {{"detail", std::string("Class struct not found:") + className}});
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
                    auto parentIt = cg_.context_info_.classParentMap.find(parentClass);
                    if (parentIt == cg_.context_info_.classParentMap.end())
                        break;
                    parentClass = parentIt->second;

                    auto parentFieldsIt = cg_.context_info_.classFieldNames.find(parentClass);
                    if (parentFieldsIt == cg_.context_info_.classFieldNames.end())
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
                // (AR) المضمَّنة («حدث») مستثناة من التخمين (انحدار #251)
                // (EN) Builtins («حدث») excluded from the guess (#251 regression)
                for (const auto &[clsName, fieldVec] : cg_.context_info_.classFieldNames)
                {
                    if (clsName == className ||
                        cg_.context_info_.builtinClassNames.count(clsName))
                        continue;
                    for (size_t i = 0; i < fieldVec.size(); i++)
                    {
                        if (fieldVec[i] == fieldName)
                        {
                            fieldIndex = static_cast<int>(i);
                            className = clsName;
                            auto newStructIt = cg_.context_info_.classStructTypes.find(clsName);
                            if (newStructIt != cg_.context_info_.classStructTypes.end())
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
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_FIELD_LAYOUT, {{"detail", std::string("Field '") + fieldName + "' not found in class '" + className + "' or its parents"}});
                return nullptr;
            }

            // Resolve object pointer
            llvm::Value *actualObj = normalizedObjPtr;
            if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(objPtr))
            {
                if (!allocaInst->getAllocatedType()->isStructTy())
                {
                    llvm::Value *loaded = cg_.builder_->CreateLoad(
                        allocaInst->getAllocatedType(), allocaInst, objRegName + ".load");
                    if (loaded->getType()->isIntegerTy())
                    {
                        actualObj = cg_.builder_->CreateIntToPtr(loaded,
                                                             llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".ptr");
                    }
                }
            }
            else if (auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(objPtr))
            {
                llvm::Value *loaded = cg_.builder_->CreateLoad(globalVar->getValueType(), globalVar, objRegName + ".load.g");
                if (loaded->getType()->isIntegerTy())
                {
                    actualObj = cg_.builder_->CreateIntToPtr(loaded,
                                                         llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".ptr.g");
                }
                else if (loaded->getType()->isPointerTy())
                {
                    actualObj = loaded;
                }
            }
            // (AR) إذا كان الكائن لا يزال i64 — حوّله إلى ptr
            // (EN) If object is still i64 — cast to ptr
            if (actualObj->getType()->isIntegerTy())
            {
                actualObj = cg_.builder_->CreateIntToPtr(actualObj,
                                                     llvm::PointerType::getUnqual(*cg_.context_), objRegName + ".i2p");
            }

            // (AR) إزاحة vtable: الحقل 0 في الهيكل محجوز لمؤشر vtable
            // (EN) vtable offset: field 0 in struct is reserved for vtable ptr
            int structIndex = cg_.getFieldStructIndex(className, fieldIndex);

            // GEP + Store
            llvm::Value *gep = cg_.builder_->CreateStructGEP(structType, actualObj, structIndex,
                                                         fieldName + "_gep");

            // ================================================================
            // (AR) ISSUE-063: مواءمة القيمة مع نوع خانة الحقل:
            //      1) خانة %SadDyn وقيمة محسوسة ⇒ تعليب (toDyn) بنوع SIR الساكن؛
            //      2) قيمة %SadDyn وخانة محسوسة ⇒ فكّ حسب نوع الخانة (double⇒unpackDouble،
            //         ptr⇒unpackPtr، i64⇒الحمولة) — كان CreateStore يكتب 16 بايت خامًا
            //         فوق خانة 8 بايت ⇒ إفساد ذاكرة الكائن.
            // (EN) ISSUE-063: reconcile the value with the field slot type:
            //      1) %SadDyn slot + concrete value ⇒ pack (toDyn) by its static SIR type;
            //      2) %SadDyn value + concrete slot ⇒ unpack per slot type (double⇒
            //         unpackDouble, ptr⇒unpackPtr, i64⇒payload) — CreateStore used to
            //         write 16 raw bytes over an 8-byte slot ⇒ object memory corruption.
            // ================================================================
            {
                llvm::Type *fieldSlotTy = structType->getElementType(structIndex);
                llvm::StructType *dynTy = getSadDynType(*cg_.context_);
                if (fieldSlotTy == dynTy && !isSadDyn(value))
                {
                    value = toDyn(cg_, value, inst->operands[2].dataType);
                }
                else if (fieldSlotTy != dynTy && isSadDyn(value))
                {
                    if (fieldSlotTy->isDoubleTy())
                        value = unpackDouble(cg_, value);
                    else if (fieldSlotTy->isPointerTy())
                        value = unpackPtr(cg_, value);
                    else if (fieldSlotTy->isIntegerTy(1))
                        value = cg_.builder_->CreateTrunc(
                            dynPayloadI64(cg_, value), llvm::Type::getInt1Ty(*cg_.context_),
                            fieldName + ".dyn.i1");
                    else
                        value = dynPayloadI64(cg_, value);
                }
            }
            cg_.builder_->CreateStore(value, gep);

            return value;
        }

        // ====================================================================

    } // namespace LLVM
} // namespace Sad