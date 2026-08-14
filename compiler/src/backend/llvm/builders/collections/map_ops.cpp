/*
 * ============================================================================
 * LLVM IR Code Generator - Map (Hash Map) Call Handlers
 * (AR) معالجة دوال runtime الخرائط المستدعاة عبر cg_.emitCall
 *      + دوال LLVM مساعدة: getOrCreateMapFindSlot, getOrCreateMapCollect
 *      مستخرجة من llvm_codegen_branch_call.cpp بواسطة نمط Strangler Fig (CW-05)
 * (EN) Hash Map runtime call handlers + LLVM helper functions
 *      extracted from llvm_codegen_branch_call.cpp via Strangler Fig
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/collections/map_ops_codegen.h"
#include "sad_dyn_repr.h" // (AR) DynKind لتهيئة الحقل ٤ homogKind / (EN) DynKind for field 4 homogKind init
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
#include <optional>
#include <iostream>
#include <fstream>

using namespace Sad::Compiler::SIR;
using namespace Sad::Compiler; // (AR) للوصول لثوابت sir_constants.h

namespace Sad
{
    namespace LLVM
    {
        /**
         * @brief (AR) معالجة دوال runtime الخرائط المضمنة في cg_.emitCall
         *             __sad_map_create, __sad_map_set_typed, __sad_map_get,
         *             __sad_map_size, __sad_map_has, __sad_map_delete,
         *             __sad_map_keys, __sad_map_values, __sad_map_merge
         *        (EN) Handle built-in map runtime functions dispatched from cg_.emitCall
         *
         * @return std::nullopt اذا لم يكن funcName دالة خريطة (تابع البحث)
         *         std::optional(value) اذا جرى التعامل معها (قد تكون nullptr عند الخطأ)
         * @return std::nullopt if funcName is not a map function (keep looking)
         *         std::optional(value) if handled (value may be nullptr on error)
         */
        // (AR) انظر التوثيق في map_ops_codegen.h — توسيعُ الخريطةِ قبلَ الامتلاء.
        // (EN) See map_ops_codegen.h — grow the map before it fills.
        void MapOpsCodeGen::emitMapGrowIfFull(llvm::Value *mapPtr)
        {
            auto *i64Ty = cg_.getInt64Type();
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // (AR) التخطيطُ من مصدرِ الحقيقة (value_repr.yaml) لا من نسخةٍ محلّيّة:
            //      الخلفيّةُ الأصليّةُ تقرأ الثوابتَ عينَها، فلا يتباعدُ المُخفِّضان.
            // (EN) Layout from the SoT (value_repr.yaml), not a local copy: the native
            //      backend reads the same constants, so the two lowerings cannot drift.
            constexpr int64_t kSlotBytes = ::Sad::Types::repr::kMapSlotBytes;
            constexpr int64_t kGrowthFactor = ::Sad::Types::repr::kMapGrowthFactor;
            constexpr int64_t kFieldCount = ::Sad::Types::repr::kMapFieldCount;
            constexpr int64_t kFieldCapacity = ::Sad::Types::repr::kMapFieldCapacity;
            constexpr int64_t kFieldKeys = ::Sad::Types::repr::kMapFieldKeys;
            constexpr int64_t kFieldValues = ::Sad::Types::repr::kMapFieldValues;
            constexpr int64_t kFieldTypes = ::Sad::Types::repr::kMapFieldTypes;

            auto fieldGep = [&](int64_t field, const char *nm) {
                return cg_.builder_->CreateGEP(
                    i64Ty, mapPtr, {llvm::ConstantInt::get(i64Ty, field)}, nm);
            };

            llvm::Value *countGep = fieldGep(kFieldCount, "grow.count.gep");
            llvm::Value *capGep = fieldGep(kFieldCapacity, "grow.cap.gep");
            llvm::Value *count = cg_.builder_->CreateLoad(i64Ty, countGep, "grow.count");
            llvm::Value *cap = cg_.builder_->CreateLoad(i64Ty, capGep, "grow.cap");

            // (AR) `count + 1 >= cap` لا `count >= cap`: نُبقي خانةً فارغةً بعدَ الإدراجِ
            //      المحتمَل، فلا يعودُ البحثُ الخطّيُّ يفشلُ في إيجادِ فراغٍ أبدًا.
            // (EN) `count + 1 >= cap`, not `count >= cap`: keep an empty slot after the possible
            //      insert, so the linear scan can never fail to find a free slot.
            llvm::Value *needed = cg_.builder_->CreateAdd(
                count, llvm::ConstantInt::get(i64Ty, 1), "grow.needed");
            llvm::Value *isFull = cg_.builder_->CreateICmpUGE(needed, cap, "grow.is.full");

            llvm::Function *parentFn = cg_.builder_->GetInsertBlock()->getParent();
            auto *growBB = llvm::BasicBlock::Create(*cg_.context_, "mgrow.do", parentFn);
            auto *contBB = llvm::BasicBlock::Create(*cg_.context_, "mgrow.cont", parentFn);
            cg_.builder_->CreateCondBr(isFull, growBB, contBB);

            cg_.builder_->SetInsertPoint(growBB);
            llvm::Value *oldBytes = cg_.builder_->CreateMul(
                cap, llvm::ConstantInt::get(i64Ty, kSlotBytes), "grow.old.bytes");
            llvm::Value *newCap = cg_.builder_->CreateMul(
                cap, llvm::ConstantInt::get(i64Ty, kGrowthFactor), "grow.new.cap");
            llvm::Value *newBytes = cg_.builder_->CreateMul(
                newCap, llvm::ConstantInt::get(i64Ty, kSlotBytes), "grow.new.bytes");

            auto loadArray = [&](int64_t field, const char *nm) {
                return cg_.builder_->CreateIntToPtr(
                    cg_.builder_->CreateLoad(i64Ty, fieldGep(field, nm), nm), ptrTy, nm);
            };
            llvm::Value *oldKeys = loadArray(kFieldKeys, "grow.old.keys");
            llvm::Value *oldValues = loadArray(kFieldValues, "grow.old.vals");
            llvm::Value *oldTypes = loadArray(kFieldTypes, "grow.old.types");

            llvm::Value *newKeys = cg_.emitMalloc(newBytes, "grow.new.keys");
            llvm::Value *newValues = cg_.emitMalloc(newBytes, "grow.new.vals");
            llvm::Value *newTypes = cg_.emitMalloc(newBytes, "grow.new.types");

            // (AR) تصفيرُ المفاتيحِ كاملةً أوّلًا — الخاناتُ المضافةُ يجبُ أن تكونَ عدمًا
            //      كي يعرفَها البحثُ فارغةً؛ ثمّ تُنسخُ القديمةُ فوقَها.
            // (EN) Zero the whole keys buffer first — the added slots must read as null so the
            //      scan sees them empty; then copy the old ones over.
            cg_.builder_->CreateMemSet(newKeys, cg_.builder_->getInt8(0), newBytes,
                                       llvm::MaybeAlign(kSlotBytes));
            cg_.builder_->CreateMemCpy(newKeys, llvm::MaybeAlign(kSlotBytes),
                                       oldKeys, llvm::MaybeAlign(kSlotBytes), oldBytes);
            cg_.builder_->CreateMemCpy(newValues, llvm::MaybeAlign(kSlotBytes),
                                       oldValues, llvm::MaybeAlign(kSlotBytes), oldBytes);
            cg_.builder_->CreateMemCpy(newTypes, llvm::MaybeAlign(kSlotBytes),
                                       oldTypes, llvm::MaybeAlign(kSlotBytes), oldBytes);

            cg_.builder_->CreateStore(newCap, capGep);
            cg_.builder_->CreateStore(cg_.builder_->CreatePtrToInt(newKeys, i64Ty),
                                      fieldGep(kFieldKeys, "grow.dst.keys"));
            cg_.builder_->CreateStore(cg_.builder_->CreatePtrToInt(newValues, i64Ty),
                                      fieldGep(kFieldValues, "grow.dst.vals"));
            cg_.builder_->CreateStore(cg_.builder_->CreatePtrToInt(newTypes, i64Ty),
                                      fieldGep(kFieldTypes, "grow.dst.types"));
            cg_.builder_->CreateBr(contBB);

            cg_.builder_->SetInsertPoint(contBB);
        }

        // (AR) انظر التوثيق في map_ops_codegen.h — كتلةُ فشلِ عدمِ تطابقِ الوسم.
        // (EN) See map_ops_codegen.h — the dyn tag-mismatch failure block.
        void MapOpsCodeGen::emitDynTypeMismatchFailure(const char *label)
        {
            if (cg_.freestanding_)
            {
                cg_.emitFreestandingPanicCall(Sad::Compiler::kSadPanicDynTypeMismatch);
            }
            else
            {
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                auto *i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
                auto *printfType = llvm::FunctionType::get(i32Ty, {ptrTy}, true);
                auto printfFunc = cg_.module_->getOrInsertFunction("printf", printfType);
                llvm::Value *msg = cg_.builder_->CreateGlobalStringPtr(
                    Sad::Compiler::kDynTypeMismatchMapMsg, std::string(label) + ".dyn.fmt");
                cg_.builder_->CreateCall(printfFunc, {msg});
                auto *exitType =
                    llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {i32Ty}, false);
                auto exitFunc = cg_.module_->getOrInsertFunction("exit", exitType);
                cg_.builder_->CreateCall(exitFunc, {llvm::ConstantInt::get(i32Ty, 1)});
            }
            cg_.builder_->CreateUnreachable();
        }

        // (AR) انظر التوثيق في map_ops_codegen.h — تطبيعُ مفتاحِ الخريطة.
        // (EN) See map_ops_codegen.h — map key normalization.
        llvm::Value *MapOpsCodeGen::normalizeMapKey(llvm::Value *keyValue, const char *label)
        {
            if (!keyValue)
                return nullptr;

            auto *i64Ty = cg_.getInt64Type();
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            if (Sad::LLVM::isSadDyn(keyValue))
            {
                llvm::Value *kind = Sad::LLVM::dynKindByte(cg_, keyValue);
                auto *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
                llvm::Value *isStr = cg_.builder_->CreateICmpEQ(
                    kind, llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Str),
                    std::string(label) + ".is.str");

                llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
                llvm::BasicBlock *failBB =
                    llvm::BasicBlock::Create(*cg_.context_, std::string(label) + ".dyn.fail", curFunc);
                llvm::BasicBlock *okBB =
                    llvm::BasicBlock::Create(*cg_.context_, std::string(label) + ".dyn.ok", curFunc);
                cg_.builder_->CreateCondBr(isStr, okBB, failBB);

                cg_.builder_->SetInsertPoint(failBB);
                emitDynTypeMismatchFailure(label);

                cg_.builder_->SetInsertPoint(okBB);
                return Sad::LLVM::unpackPtr(cg_, keyValue);
            }

            if (keyValue->getType() == i64Ty)
                return cg_.builder_->CreateIntToPtr(keyValue, ptrTy, label);

            return keyValue;
        }

        // (AR) انظر التوثيق في map_ops_codegen.h — التطبيعُ الموحَّدُ لمؤشّرِ الخريطة.
        // (EN) See map_ops_codegen.h — the unified map-pointer normalization.
        llvm::Value *MapOpsCodeGen::normalizeMapPtr(llvm::Value *mapValue, const char *label)
        {
            if (!mapValue)
                return nullptr;

            // (AR) خانةٌ نوعُها `%SadDyn` تُحمَّلُ أوّلًا — وإلّا فشل فحصُ `isSadDyn` أدناه
            //      (النوعُ `ptr` لا بنية) فسقطت الخانةُ إلى مسارِ المؤشّرِ الجاهز وعُومِل
            //      عنوانُها خريطةً. النظيرُ في `normalizeArrayPtr`.
            // (EN) A `%SadDyn` slot is loaded first — otherwise the isSadDyn check below
            //      fails (the type is `ptr`, not a struct), the slot falls to the ready-pointer
            //      path, and its address is treated as a map. Twin of normalizeArrayPtr.
            mapValue = Sad::LLVM::loadDynSlot(cg_, mapValue);

            auto *i64Ty = cg_.getInt64Type();
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // (AR) الحالةُ الأولى: قيمةٌ موسومةٌ زمنَ التشغيل (%SadDyn) — نوعُ الكائنِ
            //      الساكنُ «أي». نحرسُ الوسمَ ثمّ نفكّ الحمولةَ مؤشّرًا.
            // (EN) Case one: a runtime-tagged value (%SadDyn) — static type «أي».
            //      Guard the tag, then unpack the payload as a pointer.
            if (Sad::LLVM::isSadDyn(mapValue))
            {
                llvm::Value *kind = Sad::LLVM::dynKindByte(cg_, mapValue);
                auto *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
                llvm::Value *isMap = cg_.builder_->CreateICmpEQ(
                    kind, llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Map),
                    std::string(label) + ".is.map");

                llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
                llvm::BasicBlock *failBB =
                    llvm::BasicBlock::Create(*cg_.context_, std::string(label) + ".dyn.fail", curFunc);
                llvm::BasicBlock *okBB =
                    llvm::BasicBlock::Create(*cg_.context_, std::string(label) + ".dyn.ok", curFunc);
                cg_.builder_->CreateCondBr(isMap, okBB, failBB);

                cg_.builder_->SetInsertPoint(failBB);
                emitDynTypeMismatchFailure(label);

                cg_.builder_->SetInsertPoint(okBB);
                return Sad::LLVM::unpackPtr(cg_, mapValue);
            }

            // (AR) الحالةُ الثانية: i64 خامٌ (مؤشّرٌ مُحوَّلٌ بـptrtoint).
            // (EN) Case two: a raw i64 (a ptrtoint'd pointer).
            if (mapValue->getType() == i64Ty)
                return cg_.builder_->CreateIntToPtr(mapValue, ptrTy, label);

            return mapValue;
        }

        std::optional<llvm::Value *> MapOpsCodeGen::emitCallMap(
            const std::string &funcName,
            std::vector<llvm::Value *> &args,
            std::shared_ptr<SIRInstruction> inst)
        {
            // ================================================================
            // (AR) دوال الخرائط (Hash Map) المضمنة — Runtime Functions
            //      بنية الخريطة في الذاكرة: {count:i64, capacity:i64, keys:ptr*, values:i64*, types:i64*}
            //      keys: مصفوفة مؤشرات نصوص (C strings) — null يعني خانة فارغة
            //      values: مصفوفة i64 — القيم المخزنة (ptrtoint للنصوص)
            //      types: مصفوفة i64 — نوع كل قيمة (0=نص, 1=رقم, 2=عشري, 3=منطقي)
            //      البحث: linear scan مع strcmp — مناسب للخرائط الصغيرة (<100 عنصر)
            // (EN) Hash Map built-in runtime functions
            //      Memory layout: {count:i64, capacity:i64, keys:ptr*, values:i64*, types:i64*}
            //      Search: linear scan with strcmp — suitable for small maps (<100 elements)
            // ================================================================

            if (funcName == "__sad_map_create")
            {
                // (AR) إنشاء خريطة جديدة بسعة أولية — malloc(5*8)=40 bytes للبنية
                //      ثم malloc(cap * 8) لكل من keys, values, types
                //      وتصفير keys بـ memset(0) لتعليم الخانات الفارغة
                // (EN) Create new map with initial capacity — malloc header + 3 arrays
                auto *i64Ty = cg_.getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                auto *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);

                // (AR) حساب السعة: max(حجم_المُعطى * 2, 8)
                llvm::Value *hint = args.empty() ? llvm::ConstantInt::get(i64Ty, 0) : args[0];
                llvm::Value *doubled = cg_.builder_->CreateMul(
                    hint, llvm::ConstantInt::get(i64Ty, ::Sad::Types::repr::kMapGrowthFactor), "cap.hint");
                llvm::Value *minCap = llvm::ConstantInt::get(i64Ty, ::Sad::Types::repr::kMapMinCapacity);
                llvm::Value *cmp = cg_.builder_->CreateICmpUGT(doubled, minCap, "cap.cmp");
                llvm::Value *capacity = cg_.builder_->CreateSelect(cmp, doubled, minCap, "cap.final");

                // (AR) تخصيص البنية الرئيسية: 5 حقول * 8 = 40 بايت

                llvm::Value *mapPtr = cg_.emitMalloc(
                    llvm::ConstantInt::get(i64Ty, ::Sad::Types::repr::kMapHeaderBytes), "map.ptr");

                // (AR) تخصيص المصفوفات (keys, values, types)
                llvm::Value *arrBytes = cg_.builder_->CreateMul(
                    capacity, llvm::ConstantInt::get(i64Ty, ::Sad::Types::repr::kMapSlotBytes), "arr.bytes");
                llvm::Value *keysPtr = cg_.emitMalloc(arrBytes, "map.keys");
                llvm::Value *valsPtr = cg_.emitMalloc(arrBytes, "map.vals");
                llvm::Value *typesPtr = cg_.emitMalloc(arrBytes, "map.types");

                // (AR) تصفير keys بالكامل — null يعني خانة فارغة
                cg_.builder_->CreateMemSet(keysPtr, cg_.builder_->getInt8(0), arrBytes, llvm::MaybeAlign(8));

                // (AR) تخزين الحقول في البنية: [0]=count, [1]=capacity, [2]=keys, [3]=values, [4]=types
                llvm::Value *countGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                            {llvm::ConstantInt::get(i64Ty, 0)}, "map.count.gep");
                cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), countGep);

                llvm::Value *capGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                          {llvm::ConstantInt::get(i64Ty, 1)}, "map.cap.gep");
                cg_.builder_->CreateStore(capacity, capGep);

                llvm::Value *keysGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                           {llvm::ConstantInt::get(i64Ty, 2)}, "map.keys.gep");
                cg_.builder_->CreateStore(cg_.builder_->CreatePtrToInt(keysPtr, i64Ty), keysGep);

                llvm::Value *valsGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                           {llvm::ConstantInt::get(i64Ty, 3)}, "map.vals.gep");
                cg_.builder_->CreateStore(cg_.builder_->CreatePtrToInt(valsPtr, i64Ty), valsGep);

                llvm::Value *typesGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                            {llvm::ConstantInt::get(i64Ty, 4)}, "map.types.gep");
                cg_.builder_->CreateStore(cg_.builder_->CreatePtrToInt(typesPtr, i64Ty), typesGep);

                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = mapPtr;
                return mapPtr;
            }

            if (funcName == "__sad_map_set_typed")
            {
                // (AR) إدراج/تحديث زوج (مفتاح، قيمة) في الخريطة
                //      args: [0]=map, [1]=key(ptr), [2]=value(i64/ptr), [3]=typeTag(i64)
                //      خوارزمية: linear scan — ابحث عن مفتاح موجود أو أول خانة فارغة
                // (EN) Insert/update key-value pair in map
                //      Linear scan: find existing key or first empty slot
                if (args.size() < 4)
                    return nullptr;

                auto *i64Ty = cg_.getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

                // (AR) تحويل map من i64 إلى ptr — المتغيرات تُخزَن كـ i64
                llvm::Value *mapPtr = normalizeMapPtr(args[0], "mset.map.ptr");
                llvm::Value *key = normalizeMapKey(args[1], "mset.key.ptr");
                llvm::Value *value = args[2];
                llvm::Value *typeTag = args[3];

                // (AR) توسيعٌ قبلَ الإدراجِ — يحفظُ الثابتَ «خانةٌ فارغةٌ واحدةٌ على الأقلّ».
                // (EN) Grow before insert — maintains the "at least one empty slot" invariant.
                emitMapGrowIfFull(mapPtr);

                // (AR) تحميل count, capacity, keys, values, types من البنية
                llvm::Value *countGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                            {llvm::ConstantInt::get(i64Ty, 0)}, "mset.count.gep");
                llvm::Value *count = cg_.builder_->CreateLoad(i64Ty, countGep, "mset.count");
                llvm::Value *capGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                          {llvm::ConstantInt::get(i64Ty, 1)}, "mset.cap.gep");
                llvm::Value *cap = cg_.builder_->CreateLoad(i64Ty, capGep, "mset.cap");

                llvm::Value *keysArrGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                              {llvm::ConstantInt::get(i64Ty, 2)}, "mset.keys.gep");
                llvm::Value *keysI64 = cg_.builder_->CreateLoad(i64Ty, keysArrGep, "mset.keys.i64");
                llvm::Value *keysArr = cg_.builder_->CreateIntToPtr(keysI64, ptrTy, "mset.keys.ptr");

                llvm::Value *valsArrGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                              {llvm::ConstantInt::get(i64Ty, 3)}, "mset.vals.gep");
                llvm::Value *valsI64 = cg_.builder_->CreateLoad(i64Ty, valsArrGep, "mset.vals.i64");
                llvm::Value *valsArr = cg_.builder_->CreateIntToPtr(valsI64, ptrTy, "mset.vals.ptr");

                llvm::Value *typesArrGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                               {llvm::ConstantInt::get(i64Ty, 4)}, "mset.types.gep");
                llvm::Value *typesI64 = cg_.builder_->CreateLoad(i64Ty, typesArrGep, "mset.types.i64");
                llvm::Value *typesArr = cg_.builder_->CreateIntToPtr(typesI64, ptrTy, "mset.types.ptr");

                // (AR) بحث خطي: استدعاء __sad_map_find_slot لإيجاد الخانة
                // (EN) Linear search: call __sad_map_find_slot helper
                llvm::Function *findSlotFn = getOrCreateMapFindSlot();
                llvm::Value *slotIdx = cg_.builder_->CreateCall(findSlotFn,
                                                            {keysArr, cap, key}, "mset.slot");

                // (AR) فحص: هل الخانة فارغة (مفتاح المفتاح == null)؟
                llvm::Value *slotKeyGep = cg_.builder_->CreateGEP(ptrTy, keysArr,
                                                              {slotIdx}, "mset.slot.key.gep");
                llvm::Value *existingKey = cg_.builder_->CreateLoad(ptrTy, slotKeyGep, "mset.existing.key");
                llvm::Value *isNull = cg_.builder_->CreateICmpEQ(existingKey,
                                                             llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "mset.is.new");

                // (AR) تخزين المفتاح (نسخة strdup)
                auto *strdupType = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
                auto strdupFunc = cg_.module_->getOrInsertFunction("strdup", strdupType);
                llvm::Value *keyCopy = cg_.builder_->CreateCall(strdupFunc, {key}, "mset.key.copy");
                // (AR) إذا كان الخانة جديدة، استخدم النسخة — وإلا احتفظ بالمفتاح القديم
                llvm::Value *finalKey = cg_.builder_->CreateSelect(isNull, keyCopy, existingKey, "mset.final.key");
                cg_.builder_->CreateStore(finalKey, slotKeyGep);

                // (AR) تخزين القيمة بتحويل نوعها حسب typeTag
                // (EN) Store value — for strings (type 0), store ptr as i64; for ints, store directly
                llvm::Value *valAsI64 = nullptr;
                llvm::Value *dynDerivedTag = nullptr;
                // ════════════════════════════════════════════════════════════
                // (AR) قيمةٌ موسومةٌ زمنَ التشغيل (%SadDyn): تصل من عنصرِ مصفوفةٍ
                //      مختلطةٍ أو دالّةٍ مجهولةِ النوع سكونيًّا. كان الباعثُ يحاول
                //      ZExt على بنيةٍ ⇒ انهيارُ التوليد. والأمامُ يعطيها الوسمَ 0
                //      (نصّ) لأنّه لا يعرف نوعَها، فلو صدّقناه لخُزّن العددُ نصًّا
                //      وعاد مقتبَسًا في التوليد. لذا نشتقّ الوسمَ من وسمِ القيمة
                //      نفسِها زمنَ التشغيل، بلا فروع (select):
                //        صحيح ⇒ 1 · منطقيّ ⇒ 3 · نصّ/عشريّ/غيرها ⇒ 0
                //      والعشريُّ يُحوَّل نصًّا مطابقةً لمسار العشريّ الثابت في الأمام
                //      (expression_collections.cpp: F64_TO_STRING بوسمٍ 0).
                // (EN) A runtime-tagged value (%SadDyn): arrives from a mixed-array
                //      element or a statically-unknown call result. The emitter tried
                //      ZExt on a struct ⇒ codegen crash. The frontend hands it tag 0
                //      (string) because it cannot know the type, so trusting that
                //      would store a number as text and re-emit it quoted. Derive the
                //      tag from the value's own runtime kind instead, branchlessly:
                //        Int ⇒ 1 · Bool ⇒ 3 · Str/Float/other ⇒ 0
                //      Float is stringified, mirroring the frontend's static float
                //      path (F64_TO_STRING with tag 0).
                // ════════════════════════════════════════════════════════════
                if (Sad::LLVM::isSadDyn(value))
                {
                    llvm::Value *kind = Sad::LLVM::dynKindByte(cg_, value);
                    llvm::Value *payload = Sad::LLVM::dynPayloadI64(cg_, value);
                    auto *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
                    auto kindIs = [&](uint8_t k, const char *nm) {
                        return cg_.builder_->CreateICmpEQ(
                            kind, llvm::ConstantInt::get(i8Ty, k), nm);
                    };
                    llvm::Value *isInt = kindIs(Sad::LLVM::DynKind::Int, "mset.dyn.is.int");
                    llvm::Value *isBool = kindIs(Sad::LLVM::DynKind::Bool, "mset.dyn.is.bool");
                    llvm::Value *isFloat = kindIs(Sad::LLVM::DynKind::Float, "mset.dyn.is.float");
                    llvm::Value *isStr = kindIs(Sad::LLVM::DynKind::Str, "mset.dyn.is.str");
                    // (AR) [م-٠٠١] العدمُ صار له وسمٌ في فضاءِ قيمِ الخريطة، فلم يعد
                    //      «لا تمثيلَ له» — يُقبَلُ كما تُقبَلُ سائرُ الأوسامِ القياسيّة.
                    // (EN) [card م-٠٠١] Null now has a tag in the map value space, so it is no
                    //      longer "unrepresentable" — accepted like the other scalar tags.
                    llvm::Value *isNullKind = kindIs(Sad::LLVM::DynKind::Null, "mset.dyn.is.null");
                    // (AR) والفراغُ كذلك: نتيجةُ قراءةِ مفتاحٍ غائبٍ تُخزَّنُ في خريطةٍ
                    //      أخرى، فلها وسمُها المميَّزُ ٥ لا تسويةٌ مع العدم.
                    // (EN) And Void: the result of reading an absent key gets stored into another
                    //      map, so it keeps its own tag 5 rather than being flattened into Null.
                    llvm::Value *isVoidKind = kindIs(Sad::LLVM::DynKind::Void, "mset.dyn.is.void");
                    llvm::Value *isMapKind = kindIs(Sad::LLVM::DynKind::Map, "mset.dyn.is.map");
                    llvm::Value *isArrayKind = kindIs(Sad::LLVM::DynKind::Array, "mset.dyn.is.array");

                    // ════════════════════════════════════════════════════════════
                    // (AR) حارسُ التمثيل: فضاءُ أوسامِ قيمةِ الخريطةِ أربعةٌ لا غير
                    //      (نصّ/صحيح/عشريّ/منطقيّ). فقيمةٌ وسمُها عدمٌ أو مصفوفةٌ أو
                    //      خريطةٌ أو كائنٌ **لا تمثيلَ لها** هنا؛ وكان الفرعُ يسقطها
                    //      إلى وسمِ النصِّ بحمولةٍ مؤشّرًا خامًا، فأوّلُ قارئٍ يعاملها
                    //      `char*` ⇒ قمامةٌ أو انهيارٌ صامت. نفشل صاخبًا بدلًا من ذلك.
                    // (EN) Representation guard: the map value tag space has exactly
                    //      four members (string/int/float/bool). A value tagged Null,
                    //      Array, Map or Obj has **no representation** here; the old
                    //      branch dropped it into the string tag with a raw pointer
                    //      payload, so the first reader treated it as a `char*` ⇒
                    //      garbage or a silent crash. Fail loudly instead.
                    // ════════════════════════════════════════════════════════════
                    llvm::Value *isRepresentable = cg_.builder_->CreateOr(
                        cg_.builder_->CreateOr(
                            cg_.builder_->CreateOr(isInt, isBool, "mset.dyn.int.bool"),
                            cg_.builder_->CreateOr(isFloat, isStr, "mset.dyn.float.str"),
                            "mset.dyn.scalar"),
                        cg_.builder_->CreateOr(
                            cg_.builder_->CreateOr(isNullKind, isVoidKind, "mset.dyn.nullish"),
                            cg_.builder_->CreateOr(isMapKind, isArrayKind, "mset.dyn.container"),
                            "mset.dyn.rest"),
                        "mset.dyn.representable");

                    llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
                    llvm::BasicBlock *tagFailBB =
                        llvm::BasicBlock::Create(*cg_.context_, "mset.dyn.tag.fail", curFunc);
                    llvm::BasicBlock *representableBB =
                        llvm::BasicBlock::Create(*cg_.context_, "mset.dyn.ok", curFunc);
                    cg_.builder_->CreateCondBr(isRepresentable, representableBB, tagFailBB);

                    cg_.builder_->SetInsertPoint(tagFailBB);
                    emitDynTypeMismatchFailure("mset.dyn");

                    // ════════════════════════════════════════════════════════════
                    // (AR) [م-٠٠١] كان العشريُّ يُحوَّلُ نصًّا هنا (وفي المسارِ الساكنِ
                    //      أماميًّا) لأنّ الوسمَ ٢ لم يكن مقروءًا في أيِّ قارئ. فكانت
                    //      `نوع(م["ك"])` تُرجعُ «نصّ» والمفسّرُ «عشريّ» — تباعُدٌ صامت.
                    //      الوسمُ اليومَ مقروءٌ في كلِّ قارئ، فتُحفَظُ بتّاتُ الـdouble
                    //      كما هي بوسمِها: صفرُ تخصيصٍ (كان مخصِّصًا نصًّا عند كلِّ
                    //      إسناد)، وصفرُ فقدٍ في الدقّة، والنوعُ محفوظٌ عبرَ الحدّ.
                    // (EN) [card م-٠٠١] Floats used to be stringified here (and on the
                    //      static frontend path) because tag 2 was read by no reader. So
                    //      `نوع(م["k"])` said «نصّ» while the interpreter said «عشريّ» — a
                    //      silent divergence. Every reader honours the tag now, so the
                    //      double's bits are stored as-is under their own tag: zero
                    //      allocation (it used to allocate a string on every assignment),
                    //      zero precision loss, and the type survives the boundary.
                    // ════════════════════════════════════════════════════════════
                    //      ولمّا زال فرعُ التنصيصِ زالت معه الكتلُ الثلاثُ التي كانت
                    //      تخدمُه (عشريّ/عاديّ/ملتقى) — لا فرعَ بعدَ حارسِ الوسم.
                    // (EN) With the stringify branch gone, so are the three blocks that
                    //      served it (float/plain/join) — no branching after the tag guard.
                    cg_.builder_->SetInsertPoint(representableBB);
                    valAsI64 = payload;

                    // (AR) الوسمُ يُشتقّ من وسمِ القيمة نفسِها: صحيح⇒١ · عشريّ⇒٢ ·
                    //      منطقيّ⇒٣ · نصّ⇒٠.
                    // (EN) The tag is derived from the value's own runtime kind:
                    //      Int⇒1 · Float⇒2 · Bool⇒3 · Str⇒0.
                    llvm::Value *tagBool = cg_.builder_->CreateSelect(
                        isBool, llvm::ConstantInt::get(i64Ty, Sad::Compiler::kMapValueTagBoolean),
                        llvm::ConstantInt::get(i64Ty, Sad::Compiler::kMapValueTagString),
                        "mset.dyn.tag.bool");
                    llvm::Value *tagFloat = cg_.builder_->CreateSelect(
                        isFloat, llvm::ConstantInt::get(i64Ty, Sad::Compiler::kMapValueTagFloat),
                        tagBool, "mset.dyn.tag.float");
                    llvm::Value *tagVoid = cg_.builder_->CreateSelect(
                        isVoidKind, llvm::ConstantInt::get(i64Ty, Sad::Compiler::kMapValueTagVoid),
                        tagFloat, "mset.dyn.tag.void");
                    llvm::Value *tagNull = cg_.builder_->CreateSelect(
                        isNullKind, llvm::ConstantInt::get(i64Ty, Sad::Compiler::kMapValueTagNull),
                        tagVoid, "mset.dyn.tag.null");
                    llvm::Value *tagMap = cg_.builder_->CreateSelect(
                        isMapKind, llvm::ConstantInt::get(i64Ty, Sad::Compiler::kMapValueTagMap),
                        tagNull, "mset.dyn.tag.map");
                    llvm::Value *tagArray = cg_.builder_->CreateSelect(
                        isArrayKind, llvm::ConstantInt::get(i64Ty, Sad::Compiler::kMapValueTagArray),
                        tagMap, "mset.dyn.tag.array");
                    dynDerivedTag = cg_.builder_->CreateSelect(
                        isInt, llvm::ConstantInt::get(i64Ty, Sad::Compiler::kMapValueTagInteger),
                        tagArray, "mset.dyn.tag");
                }
                else if (value->getType()->isDoubleTy())
                    // (AR) عشريٌّ ساكنٌ ⇒ بتّاتُه كما هي؛ الوسمُ يأتي من الأمامِ (٢).
                    // (EN) A static double ⇒ its raw bits; the tag comes from the frontend (2).
                    valAsI64 = cg_.builder_->CreateBitCast(value, i64Ty, "mset.val.f64bits");
                else if (value->getType()->isPointerTy())
                    valAsI64 = cg_.builder_->CreatePtrToInt(value, i64Ty, "mset.val.i64");
                else if (value->getType() == i64Ty)
                    valAsI64 = value;
                else
                    valAsI64 = cg_.builder_->CreateZExtOrTrunc(value, i64Ty, "mset.val.ext");

                llvm::Value *valSlotGep = cg_.builder_->CreateGEP(i64Ty, valsArr,
                                                              {slotIdx}, "mset.val.gep");
                cg_.builder_->CreateStore(valAsI64, valSlotGep);

                // (AR) تخزين نوع القيمة
                llvm::Value *typeSlotGep = cg_.builder_->CreateGEP(i64Ty, typesArr,
                                                               {slotIdx}, "mset.type.gep");
                llvm::Value *typeAsI64;
                if (dynDerivedTag)
                    typeAsI64 = dynDerivedTag;
                else if (typeTag->getType() == i64Ty)
                    typeAsI64 = typeTag;
                else
                    typeAsI64 = cg_.builder_->CreateZExtOrTrunc(typeTag, i64Ty, "mset.type.ext");
                cg_.builder_->CreateStore(typeAsI64, typeSlotGep);

                // (AR) زيادة count فقط إذا كانت الخانة جديدة
                llvm::Value *newCount = cg_.builder_->CreateAdd(count, llvm::ConstantInt::get(i64Ty, 1), "mset.new.count");
                llvm::Value *finalCount = cg_.builder_->CreateSelect(isNull, newCount, count, "mset.final.count");
                cg_.builder_->CreateStore(finalCount, countGep);

                return llvm::ConstantInt::get(i64Ty, 0);
            }

            if (funcName == kRuntimeMapGet || funcName == kRuntimeMapGetI64 ||
                funcName == kRuntimeMapGetDyn)
            {
                // (AR) قراءة قيمة من الخريطة بالمفتاح — ذكية حسب type tag
                //      __sad_map_get: يُرجع ptr (نص) — إذا القيمة رقم يُحوّلها لنص عبر sprintf
                //      __sad_map_get_i64: يُرجع i64 مباشرة — إذا القيمة نص يُرجع 0
                //      args: [0]=map, [1]=key(ptr)
                // (EN) Read value from map by key — type-tag aware
                //      __sad_map_get: returns ptr — if value is int, converts via sprintf
                //      __sad_map_get_i64: returns i64 — if value is string, returns 0
                if (args.size() < 2)
                    return nullptr;

                auto *i64Ty = cg_.getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

                // (AR) تحويل map من i64 إلى ptr — المتغيرات تُخزَن كـ i64
                llvm::Value *mapPtr = normalizeMapPtr(args[0], "mget.map.ptr");
                llvm::Value *key = normalizeMapKey(args[1], "mget.key.ptr");

                // (AR) تحميل capacity, keys, values, types من البنية
                llvm::Value *capGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                          {llvm::ConstantInt::get(i64Ty, 1)}, "mget.cap.gep");
                llvm::Value *cap = cg_.builder_->CreateLoad(i64Ty, capGep, "mget.cap");

                llvm::Value *keysArrGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                              {llvm::ConstantInt::get(i64Ty, 2)}, "mget.keys.gep");
                llvm::Value *keysI64 = cg_.builder_->CreateLoad(i64Ty, keysArrGep, "mget.keys.i64");
                llvm::Value *keysArr = cg_.builder_->CreateIntToPtr(keysI64, ptrTy, "mget.keys.ptr");

                llvm::Value *valsArrGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                              {llvm::ConstantInt::get(i64Ty, 3)}, "mget.vals.gep");
                llvm::Value *valsI64 = cg_.builder_->CreateLoad(i64Ty, valsArrGep, "mget.vals.i64");
                llvm::Value *valsArr = cg_.builder_->CreateIntToPtr(valsI64, ptrTy, "mget.vals.ptr");

                llvm::Value *typesArrGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                               {llvm::ConstantInt::get(i64Ty, 4)}, "mget.types.gep");
                llvm::Value *typesI64 = cg_.builder_->CreateLoad(i64Ty, typesArrGep, "mget.types.i64");
                llvm::Value *typesArr = cg_.builder_->CreateIntToPtr(typesI64, ptrTy, "mget.types.ptr");

                // (AR) بحث عن المفتاح
                llvm::Function *findSlotFn = getOrCreateMapFindSlot();
                llvm::Value *slotIdx = cg_.builder_->CreateCall(findSlotFn,
                                                            {keysArr, cap, key}, "mget.slot");

                // (AR) قراءة القيمة ونوعها
                llvm::Value *valGep = cg_.builder_->CreateGEP(i64Ty, valsArr,
                                                          {slotIdx}, "mget.val.gep");
                llvm::Value *valI64 = cg_.builder_->CreateLoad(i64Ty, valGep, "mget.val");

                llvm::Value *typeGep = cg_.builder_->CreateGEP(i64Ty, typesArr,
                                                           {slotIdx}, "mget.type.gep");
                llvm::Value *typeTag = cg_.builder_->CreateLoad(i64Ty, typeGep, "mget.type");

                // ════════════════════════════════════════════════════════════
                // (AR) [م-٠٠١ ق٣] القراءةُ الموسومةُ زمنَ التشغيل — تُرجع %SadDyn.
                //      كان كلُّ مسارِ قراءةٍ يسوّي القيمةَ إلى نصٍّ (أو i64 خامّ)
                //      فيضيعُ نوعُها عندَ الحدّ: `دالة رقم اقرأ(خريطة م) ارجع م["أ"]`
                //      كانت تُرجع المؤشّرَ عددًا. الوسمُ محفوظٌ أصلًا في مصفوفةِ
                //      types، فلا يلزمُ تمثيلٌ جديد — يلزمُ ألّا نرميَه.
                //
                //      ترجمةُ الوسوم: وسمُ الخريطةِ (٠ نصّ · ١ صحيح · ٢ عشريّ ·
                //      ٣ منطقيّ) ⇒ وسمُ %SadDyn (Str · Int · Float · Bool). لا
                //      تطابقَ عدديًّا بين الفضاءَين فالترجمةُ صريحةٌ بلا فروع.
                //
                //      ومفتاحٌ غائبٌ ⇒ عدم: مصفوفتا values/types غيرُ مصفَّرتَين
                //      عندَ الإنشاء (المصفَّرةُ keys وحدَها)، فقراءةُ خانةٍ فارغةٍ
                //      تُرجعُ وسمًا قمامةً. الحارسُ هنا يقرأ مفتاحَ الخانة: إن كان
                //      عدمًا فالقيمةُ **فراغٌ** لا عدم — والفرقُ منصوصٌ في مصدرِ
                //      الحقيقة: «فراغ» ما لم يُرجَع أصلًا (مفتاحٌ غائب)، و«عدم» قيمةُ
                //      لاشيءَ الصريحة. وبهذا يُميَّزُ «مفتاحٌ غائبٌ» من «قيمةٍ فارغة»،
                //      وهو ما كان متعذّرًا في المصرّف.
                // (EN) [card م-٠٠١, ق٣] Runtime-tagged read — returns %SadDyn.
                //      Every read path flattened the value to a string (or a raw
                //      i64), losing its type at the boundary. The tag is already
                //      stored in the types array — no new representation is
                //      needed; what was needed was not to throw it away.
                //
                //      Tag translation: map tag (0 str · 1 int · 2 float · 3 bool)
                //      ⇒ %SadDyn kind (Str · Int · Float · Bool). The two spaces do
                //      not coincide numerically, so translate explicitly, branchless.
                //
                //      Missing key ⇒ Null: the values/types arrays are not zeroed at
                //      creation (only keys are), so reading an empty slot yields a
                //      garbage tag. The guard reads the slot's key: a null key means a
                //      null value — which is also how "absent key" becomes
                //      distinguishable from "empty value" in the compiler.
                // ════════════════════════════════════════════════════════════
                if (funcName == kRuntimeMapGetDyn)
                {
                    auto *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);

                    llvm::Value *slotKeyGep = cg_.builder_->CreateGEP(ptrTy, keysArr,
                                                                     {slotIdx}, "mgetd.slot.key.gep");
                    llvm::Value *slotKey = cg_.builder_->CreateLoad(ptrTy, slotKeyGep, "mgetd.slot.key");
                    llvm::Value *isPresent = cg_.builder_->CreateICmpNE(
                        slotKey,
                        llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)),
                        "mgetd.present");

                    auto tagIs = [&](int64_t mapTag, const char *nm) {
                        return cg_.builder_->CreateICmpEQ(
                            typeTag, llvm::ConstantInt::get(i64Ty, mapTag), nm);
                    };
                    llvm::Value *isInt = tagIs(kMapValueTagInteger, "mgetd.is.int");
                    llvm::Value *isFloat = tagIs(kMapValueTagFloat, "mgetd.is.float");
                    llvm::Value *isBool = tagIs(kMapValueTagBoolean, "mgetd.is.bool");
                    llvm::Value *isNullTag = tagIs(kMapValueTagNull, "mgetd.is.null");
                    llvm::Value *isVoidTag = tagIs(kMapValueTagVoid, "mgetd.is.void");
                    llvm::Value *isMapTag = tagIs(kMapValueTagMap, "mgetd.is.map");
                    llvm::Value *isArrayTag = tagIs(kMapValueTagArray, "mgetd.is.array");

                    // (AR) الافتراضُ نصّ (وسمُ الخريطةِ ٠) ثمّ نُبدّلُه بالوسمِ المطابق.
                    // (EN) Default to Str (map tag 0), then override with the matching kind.
                    llvm::Value *kind = llvm::ConstantInt::get(i8Ty, DynKind::Str);
                    kind = cg_.builder_->CreateSelect(
                        isInt, llvm::ConstantInt::get(i8Ty, DynKind::Int), kind, "mgetd.k.int");
                    kind = cg_.builder_->CreateSelect(
                        isFloat, llvm::ConstantInt::get(i8Ty, DynKind::Float), kind, "mgetd.k.float");
                    kind = cg_.builder_->CreateSelect(
                        isBool, llvm::ConstantInt::get(i8Ty, DynKind::Bool), kind, "mgetd.k.bool");
                    kind = cg_.builder_->CreateSelect(
                        isNullTag, llvm::ConstantInt::get(i8Ty, DynKind::Null), kind, "mgetd.k.null");
                    kind = cg_.builder_->CreateSelect(
                        isVoidTag, llvm::ConstantInt::get(i8Ty, DynKind::Void), kind, "mgetd.k.void");
                    kind = cg_.builder_->CreateSelect(
                        isMapTag, llvm::ConstantInt::get(i8Ty, DynKind::Map), kind, "mgetd.k.map");
                    kind = cg_.builder_->CreateSelect(
                        isArrayTag, llvm::ConstantInt::get(i8Ty, DynKind::Array), kind, "mgetd.k.array");
                    kind = cg_.builder_->CreateSelect(
                        isPresent, kind, llvm::ConstantInt::get(i8Ty, DynKind::Void), "mgetd.k.final");

                    llvm::Value *payload = cg_.builder_->CreateSelect(
                        isPresent, valI64, llvm::ConstantInt::get(i64Ty, 0), "mgetd.payload");

                    // (AR) وسيطٌ ثالثٌ ⇒ قيمةٌ افتراضيّةٌ تحلُّ محلَّ «لاشيء» عندَ غيابِ
                    //      المفتاح. تُعلَّبُ بنوعِها السكونيِّ من معامِلِ التعليمة، فيبقى
                    //      نوعُها محفوظًا كنوعِ القيمةِ الموجودة.
                    // (EN) A third argument ⇒ a default replacing «لاشيء» when the key is
                    //      absent. It is packed by its static type taken from the
                    //      instruction operand, so its type survives just like a present
                    //      value's does.
                    if (args.size() > 2 && inst && inst->operands.size() > 3)
                    {
                        llvm::Value *fallback =
                            Sad::LLVM::toDyn(cg_, args[2], inst->operands[3].dataType);
                        llvm::Value *fallbackKind = Sad::LLVM::dynKindByte(cg_, fallback);
                        llvm::Value *fallbackPayload = Sad::LLVM::dynPayloadI64(cg_, fallback);
                        kind = cg_.builder_->CreateSelect(isPresent, kind, fallbackKind, "mgetd.k.def");
                        payload = cg_.builder_->CreateSelect(
                            isPresent, payload, fallbackPayload, "mgetd.payload.def");
                    }

                    llvm::Value *result = Sad::LLVM::makeDyn(cg_, kind, payload);
                    if (inst->result.has_value())
                        cg_.context_info_.namedValues[inst->result->name] = result;
                    return result;
                }

                if (funcName == kRuntimeMapGet)
                {
                    // (AR) إرجاع ptr — لكن نفحص: إذا type=0 (نص) نحول مباشرة
                    //      إذا type=1 (رقم) نحول بـ sprintf لنص
                    //      هذا يحل مشكلة الخرائط المختلطة الأنواع
                    // (EN) Return ptr — but check type: if string → inttoptr, if int → sprintf to string
                    llvm::Value *isString = cg_.builder_->CreateICmpEQ(typeTag,
                                                                   llvm::ConstantInt::get(i64Ty, 0), "mget.is.str");

                    // (AR) نستخدم select: إذا نص → inttoptr(val)
                    //      إذا رقم → sprintf(buf, "%lld", val) → buf
                    llvm::Value *strPtr = cg_.builder_->CreateIntToPtr(valI64, ptrTy, "mget.as.ptr");

                    // (AR) تخصيص ذاكرة مؤقتة 32 بايت لتحويل الرقم لنص

                    llvm::Value *buf = cg_.emitMalloc(llvm::ConstantInt::get(i64Ty, 32), "mget.sprintf.buf");

                    // (AR) استدعاء sprintf لتحويل الرقم لنص
                    //      توقيع sprintf الحقيقي له معاملان ثابتان فقط (str, format) — كل
                    //      ما بعدهما vararg. إضافة i64Ty كمعامل "ثابت" ثالث هنا كانت تجعل
                    //      LLVM يُمرِّر valI64 على أنه fixed argument بدل أول vararg حقيقي.
                    //      هذا غير ضار على System V AMD64 (x86_64) لأن fixed/vararg الأولى
                    //      تُمرَّر بنفس الطريقة عبر registers — لكنه خاطئ على ABI الصارم
                    //      لـApple AArch64 الذي يتطلب تمرير كل معاملات vararg الحقيقية عبر
                    //      stack فقط، فيقرأ sprintf الحقيقي (المُصرَّف بتوقيعه الصحيح ثنائي
                    //      المعامل) قيمة عشوائية من stack بدل valI64 من السجل x2 — وهذا
                    //      السبب الجذري لفشل تحويل الأرقام داخل الخرائط على macOS/arm64.
                    // (EN) sprintf's real signature has only 2 fixed params (str, format)
                    //      — everything else is vararg. Declaring i64Ty as a third "fixed"
                    //      param made LLVM pass valI64 as a fixed argument instead of the
                    //      first real vararg. Harmless on System V AMD64 (x86_64), since
                    //      fixed and early vararg args are passed identically via
                    //      registers — but wrong on Apple's strict AArch64 ABI, which
                    //      requires all real vararg arguments to go through the stack:
                    //      the real sprintf (compiled against its true 2-param signature)
                    //      then reads garbage off the stack instead of valI64 from x2 —
                    //      the root cause of corrupted integer-to-string map values on
                    //      macOS/arm64.
                    auto *sprintfType = llvm::FunctionType::get(
                        llvm::Type::getInt32Ty(*cg_.context_),
                        {ptrTy, ptrTy}, true);
                    auto sprintfFn = cg_.module_->getOrInsertFunction("sprintf", sprintfType);
                    llvm::Value *fmtStr = cg_.builder_->CreateGlobalStringPtr("%lld", "mget.fmt.lld");
                    cg_.builder_->CreateCall(sprintfFn, {buf, fmtStr, valI64});

                    // ════════════════════════════════════════════════════════════
                    // (AR) [م-٠٠١] الوسمُ ٢ (عشريّ) صارت حمولتُه بتّاتِ double لا مؤشّرَ
                    //      نصّ، فصياغتُه بـ%lld تطبعُ عددًا فلكيًّا. نُنسّقُه بـ
                    //      `__sad_format_double` — الدالّةُ نفسُها التي يستعملُها
                    //      F64_TO_STRING، فيتطابقُ التنسيقُ مع سائرِ المصرِّف.
                    //      المخزنُ ٥١٢ بايتًا: ‎%.6f‎ لـDBL_MAX نحوُ ٣١٦ محرفًا.
                    // (EN) [card م-٠٠١] Tag 2 (float) now carries a double's bits rather
                    //      than a string pointer, so formatting it with %lld prints an
                    //      astronomical integer. Format it with `__sad_format_double` —
                    //      the very function F64_TO_STRING uses — so the rendering matches
                    //      the rest of the compiler. A 512-byte buffer: %.6f for DBL_MAX
                    //      is around 316 characters.
                    // ════════════════════════════════════════════════════════════
                    constexpr int64_t kDoubleTextBufferBytes = 512;
                    llvm::Value *isFloatTag = cg_.builder_->CreateICmpEQ(
                        typeTag, llvm::ConstantInt::get(i64Ty, kMapValueTagFloat), "mget.is.float");
                    llvm::Value *floatBuf = cg_.emitMalloc(
                        llvm::ConstantInt::get(i64Ty, kDoubleTextBufferBytes), "mget.f64.buf");
                    auto *formatDoubleType = llvm::FunctionType::get(
                        llvm::Type::getVoidTy(*cg_.context_),
                        {ptrTy, llvm::Type::getDoubleTy(*cg_.context_)}, false);
                    auto formatDoubleFn =
                        cg_.module_->getOrInsertFunction("__sad_format_double", formatDoubleType);
                    llvm::Value *asDouble = cg_.builder_->CreateBitCast(
                        valI64, llvm::Type::getDoubleTy(*cg_.context_), "mget.as.f64");
                    cg_.builder_->CreateCall(formatDoubleFn, {floatBuf, asDouble});

                    // (AR) [م-٠٠١] والوسمُ ٤ (عدم) يُعرَضُ «لاشيء» من مصدرِ الحقيقةِ نفسِه
                    //      الذي يستعملُه المفسّرُ وdynToString — لا `%lld` على حمولةٍ صفريّة.
                    // (EN) [card م-٠٠١] Tag 4 (null) renders «لاشيء» from the same source of truth
                    //      the interpreter and dynToString use — not `%lld` on a zero payload.
                    llvm::Value *isNullTagText = cg_.builder_->CreateOr(
                        cg_.builder_->CreateICmpEQ(
                            typeTag, llvm::ConstantInt::get(i64Ty, kMapValueTagNull), "mget.is.null"),
                        cg_.builder_->CreateICmpEQ(
                            typeTag, llvm::ConstantInt::get(i64Ty, kMapValueTagVoid), "mget.is.void"),
                        "mget.is.nullish");
                    llvm::Value *nullText = cg_.builder_->CreateGlobalStringPtr(
                        ::Sad::Types::repr::kNullDisplay, "mget.nulltext");

                    // (AR) اختيار: نص أصلي أو الرقم المُحوّل (صحيحًا أو عشريًّا) أو «لاشيء»
                    // (EN) Select: the original string, the converted number (int or float), or «لاشيء»
                    llvm::Value *numericText =
                        cg_.builder_->CreateSelect(isFloatTag, floatBuf, buf, "mget.numeric");
                    llvm::Value *nonStringText = cg_.builder_->CreateSelect(
                        isNullTagText, nullText, numericText, "mget.nonstring");
                    llvm::Value *result = cg_.builder_->CreateSelect(isString, strPtr, nonStringText, "mget.result");

                    if (inst->result.has_value())
                        cg_.context_info_.namedValues[inst->result->name] = result;
                    return result;
                }
                else
                {
                    // __sad_map_get_i64: (AR) إرجاع i64 مباشرة — النوع المناسب للأرقام
                    // (EN) Return i64 directly — suitable for integer values
                    llvm::Value *result = valI64;

                    // (AR) [م-٠٠١] سجلُّ النتيجةِ عشريٌّ ⇒ الحمولةُ بتّاتُ double
                    //      (الوسمُ ٢) فتُعادُ إلى نوعِها. الأمامُ وحدَه يعرفُ أنّ نوعَ
                    //      قيمِ الخريطةِ عشريٌّ سكونيًّا، فيُعلنُه في نوعِ السجلّ.
                    // (EN) [card م-٠٠١] A float result register ⇒ the payload is a
                    //      double's bits (tag 2), so cast it back. Only the frontend knows
                    //      the map's value type is statically float; it says so through
                    //      the result register's type.
                    if (inst->result.has_value() &&
                        inst->result->dataType == SadTypeKind::Float)
                        result = cg_.builder_->CreateBitCast(
                            result, llvm::Type::getDoubleTy(*cg_.context_), "mget.f64");

                    if (inst->result.has_value())
                        cg_.context_info_.namedValues[inst->result->name] = result;
                    return result;
                }
            }

            if (funcName == "__sad_map_size")
            {
                // (AR) حجم الخريطة — قراءة count من البنية
                // (EN) Map size — read count from header
                if (args.empty())
                    return nullptr;

                auto *i64Ty = cg_.getInt64Type();

                // (AR) تحويل map من i64 إلى ptr
                llvm::Value *mapArg = normalizeMapPtr(args[0], "msize.map.ptr");
                llvm::Value *countGep = cg_.builder_->CreateGEP(i64Ty, mapArg,
                                                            {llvm::ConstantInt::get(i64Ty, 0)}, "msize.count.gep");
                llvm::Value *result = cg_.builder_->CreateLoad(i64Ty, countGep, "msize.count");

                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = result;
                return result;
            }

            if (funcName == "__sad_map_has")
            {
                // (AR) فحص وجود مفتاح — ابحث ثم تحقق إذا الخانة مشغولة
                // (EN) Check if key exists — find slot, check if occupied
                if (args.size() < 2)
                    return nullptr;

                auto *i64Ty = cg_.getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

                // (AR) تحويل map من i64 إلى ptr
                llvm::Value *mapPtr = normalizeMapPtr(args[0], "mhas.map.ptr");
                llvm::Value *key = normalizeMapKey(args[1], "mhas.key.ptr");

                llvm::Value *capGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                          {llvm::ConstantInt::get(i64Ty, 1)}, "mhas.cap.gep");
                llvm::Value *cap = cg_.builder_->CreateLoad(i64Ty, capGep, "mhas.cap");

                llvm::Value *keysArrGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                              {llvm::ConstantInt::get(i64Ty, 2)}, "mhas.keys.gep");
                llvm::Value *keysI64 = cg_.builder_->CreateLoad(i64Ty, keysArrGep, "mhas.keys.i64");
                llvm::Value *keysArr = cg_.builder_->CreateIntToPtr(keysI64, ptrTy, "mhas.keys.ptr");

                llvm::Function *findSlotFn = getOrCreateMapFindSlot();
                llvm::Value *slotIdx = cg_.builder_->CreateCall(findSlotFn,
                                                            {keysArr, cap, key}, "mhas.slot");

                // (AR) قراءة المفتاح في الخانة — إذا != null → موجود
                llvm::Value *slotKeyGep = cg_.builder_->CreateGEP(ptrTy, keysArr,
                                                              {slotIdx}, "mhas.slot.key.gep");
                llvm::Value *slotKey = cg_.builder_->CreateLoad(ptrTy, slotKeyGep, "mhas.slot.key");
                llvm::Value *isNotNull = cg_.builder_->CreateICmpNE(slotKey,
                                                                llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "mhas.found");
                llvm::Value *result = cg_.builder_->CreateZExt(isNotNull, i64Ty, "mhas.result");

                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = result;
                return result;
            }

            if (funcName == "__sad_map_delete")
            {
                // (AR) حذف عنصر من الخريطة — البحث عن الخانة ثم تصفير المفتاح وإنقاص العداد
                // (EN) Delete map entry — find slot, null the key, decrement count
                if (args.size() < 2)
                    return nullptr;

                auto *i64Ty = cg_.getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

                llvm::Value *mapPtr = normalizeMapPtr(args[0], "mdel.map.ptr");
                llvm::Value *key = normalizeMapKey(args[1], "mdel.key.ptr");

                // (AR) تحميل capacity و keys array
                llvm::Value *capGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                          {llvm::ConstantInt::get(i64Ty, 1)}, "mdel.cap.gep");
                llvm::Value *cap = cg_.builder_->CreateLoad(i64Ty, capGep, "mdel.cap");

                llvm::Value *keysArrGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                              {llvm::ConstantInt::get(i64Ty, 2)}, "mdel.keys.gep");
                llvm::Value *keysI64 = cg_.builder_->CreateLoad(i64Ty, keysArrGep, "mdel.keys.i64");
                llvm::Value *keysArr = cg_.builder_->CreateIntToPtr(keysI64, ptrTy, "mdel.keys.ptr");

                // (AR) البحث عن الخانة
                llvm::Function *findSlotFn = getOrCreateMapFindSlot();
                llvm::Value *slotIdx = cg_.builder_->CreateCall(findSlotFn,
                                                            {keysArr, cap, key}, "mdel.slot");

                // (AR) فحص وجود المفتاح في الخانة
                llvm::Value *slotKeyGep = cg_.builder_->CreateGEP(ptrTy, keysArr,
                                                              {slotIdx}, "mdel.slot.key.gep");
                llvm::Value *slotKey = cg_.builder_->CreateLoad(ptrTy, slotKeyGep, "mdel.slot.key");
                llvm::Value *isNotNull = cg_.builder_->CreateICmpNE(slotKey,
                                                                llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "mdel.found");

                // (AR) إذا موجود: null المفتاح + إنقاص العداد
                llvm::Function *parentFn = cg_.builder_->GetInsertBlock()->getParent();
                llvm::BasicBlock *deleteBB = llvm::BasicBlock::Create(*cg_.context_, "mdel.do", parentFn);
                llvm::BasicBlock *endBB = llvm::BasicBlock::Create(*cg_.context_, "mdel.end", parentFn);
                cg_.builder_->CreateCondBr(isNotNull, deleteBB, endBB);

                cg_.builder_->SetInsertPoint(deleteBB);
                // (AR) تصفير المفتاح
                cg_.builder_->CreateStore(llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), slotKeyGep);
                // (AR) إنقاص العداد
                llvm::Value *countGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                            {llvm::ConstantInt::get(i64Ty, 0)}, "mdel.count.gep");
                llvm::Value *count = cg_.builder_->CreateLoad(i64Ty, countGep, "mdel.count");
                llvm::Value *newCount = cg_.builder_->CreateSub(count, llvm::ConstantInt::get(i64Ty, 1), "mdel.new.count");
                cg_.builder_->CreateStore(newCount, countGep);
                cg_.builder_->CreateBr(endBB);

                cg_.builder_->SetInsertPoint(endBB);
                return llvm::ConstantInt::get(i64Ty, 0);
            }

            if (funcName == "__sad_map_keys" || funcName == "__sad_map_values")
            {
                // (AR) إرجاع مصفوفة المفاتيح أو القيم من الخريطة
                //      مفاتيح: نسخ المفاتيح غير الفارغة إلى مصفوفة جديدة
                //      قيم: نسخ القيم المقابلة للمفاتيح غير الفارغة
                // (EN) Return array of keys or values from map
                if (args.empty())
                    return nullptr;

                auto *i64Ty = cg_.getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

                // (AR) تحويل map من i64 إلى ptr
                llvm::Value *mapPtr = normalizeMapPtr(args[0], "mkvs.map.ptr");
                bool isKeys = (funcName == "__sad_map_keys");

                // (AR) تحميل count, capacity, keys, values
                llvm::Value *countGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                            {llvm::ConstantInt::get(i64Ty, 0)}, "mkvs.count.gep");
                llvm::Value *count = cg_.builder_->CreateLoad(i64Ty, countGep, "mkvs.count");
                llvm::Value *capGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                          {llvm::ConstantInt::get(i64Ty, 1)}, "mkvs.cap.gep");
                llvm::Value *cap = cg_.builder_->CreateLoad(i64Ty, capGep, "mkvs.cap");

                llvm::Value *keysArrGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                              {llvm::ConstantInt::get(i64Ty, 2)}, "mkvs.keys.gep");
                llvm::Value *keysI64 = cg_.builder_->CreateLoad(i64Ty, keysArrGep, "mkvs.keys.i64");
                llvm::Value *keysArr = cg_.builder_->CreateIntToPtr(keysI64, ptrTy, "mkvs.keys.ptr");

                llvm::Value *srcArrGep;
                if (isKeys)
                {
                    srcArrGep = keysArrGep; // (AR) نسخ المفاتيح
                }
                else
                {
                    srcArrGep = cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                    {llvm::ConstantInt::get(i64Ty, 3)}, "mkvs.vals.gep"); // (AR) نسخ القيم
                }
                llvm::Value *srcI64 = cg_.builder_->CreateLoad(i64Ty, srcArrGep, "mkvs.src.i64");
                llvm::Value *srcArr = cg_.builder_->CreateIntToPtr(srcI64, ptrTy, "mkvs.src.ptr");

                // (AR) استدعاء دالة مساعدة لبناء المصفوفة من الخانات غير الفارغة
                llvm::Function *collectFn = getOrCreateMapCollect();
                // ════════════════════════════════════════════════════════════
                // (AR) [م-٠٠١] المفاتيحُ نصوصٌ قطعًا ⇒ متجانسةٌ بوسمِ Str بلا مصفوفةِ أوسام.
                //      أمّا القيمُ فمختلطةٌ بطبعِها، فتُمرَّرُ مصفوفةُ أوسامِ الخريطةِ ليحملَ
                //      كلُّ عنصرٍ وسمَه. كان الوسمُ يُثبَّت «عددًا» للقيمِ جميعًا، فمصفوفةُ
                //      `خريطة_قيم` على خريطةٍ نصّيّةٍ تطبعُ مؤشّراتٍ لا نصوصًا.
                // (EN) [card م-٠٠١] Keys are always strings ⇒ homogeneous under Str with no tags
                //      array. Values are mixed by nature, so the map's tag array is passed and each
                //      element carries its own kind. The kind used to be pinned to Int for every
                //      value, so `خريطة_قيم` over a string map printed pointers, not strings.
                // ════════════════════════════════════════════════════════════
                llvm::Value *collectHomogKind = llvm::ConstantInt::get(
                    llvm::Type::getInt8Ty(*cg_.context_),
                    isKeys ? Sad::LLVM::DynKind::Str : Sad::LLVM::DynKind::Int);
                llvm::Value *collectTypes =
                    isKeys ? llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy))
                           : cg_.builder_->CreateIntToPtr(
                                 cg_.builder_->CreateLoad(
                                     i64Ty,
                                     cg_.builder_->CreateGEP(i64Ty, mapPtr,
                                                             {llvm::ConstantInt::get(i64Ty, 4)},
                                                             "mkvs.types.gep"),
                                     "mkvs.types.i64"),
                                 ptrTy, "mkvs.types.ptr");
                llvm::Value *result = cg_.builder_->CreateCall(
                    collectFn, {keysArr, srcArr, cap, count, collectHomogKind, collectTypes},
                    "mkvs.result");

                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = result;
                return result;
            }

            if (funcName == "__sad_map_merge")
            {
                // (AR) دمج خريطة مصدر في خريطة هدف — غير مدعوم حالياً، يُتجاهل بهدوء
                // (EN) Merge source map into target — not supported yet, silently ignored
                return llvm::ConstantInt::get(cg_.getInt64Type(), 0);
            }

            // ════════════════════════════════════════════════════════════════
            // (AR) [م-٠٠١ ق١] نسخٌ سطحيٌّ للخريطة — أساسُ نقاءِ `خريطة_عين`
            //      و`خريطة_احذف`. السعةُ تُنسخُ كما هي فتبقى الخاناتُ متطابقةً
            //      موضعًا ولا يلزمُ إعادةُ تجزئة.
            //
            //      المفاتيحُ تُشارَكُ مؤشّراتٍ لا تُستنسَخُ نصًّا: لا شيءَ في هذا
            //      التمثيلِ يُحرّرُ مفتاحًا قطُّ (`__sad_map_delete` يُصفّرُ الخانةَ
            //      ولا يُحرّر)، فالمشاركةُ آمنةٌ ولا تُنشئُ تحريرًا مزدوجًا.
            // (EN) [card م-٠٠١, ق١] Shallow map copy — the basis of `خريطة_عين`
            //      and `خريطة_احذف` purity. Capacity is copied verbatim so slots
            //      stay positionally identical and no rehash is needed.
            //
            //      Keys are shared as pointers rather than re-duplicated: nothing in
            //      this representation ever frees a key (`__sad_map_delete` nulls the
            //      slot without freeing), so sharing is safe and creates no double free.
            // ════════════════════════════════════════════════════════════════
            if (funcName == kRuntimeMapCopy)
            {
                if (args.empty())
                    return nullptr;

                auto *i64Ty = cg_.getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

                // (AR) حجمُ ترويسةِ الخريطةِ وعرضُ خانتِها — بمسمّياتٍ لا أرقامٍ عارية.
                // (EN) Map header size and slot width — named, not bare numbers.
                constexpr int64_t kMapHeaderBytes = ::Sad::Types::repr::kMapHeaderBytes;
                constexpr int64_t kSlotBytes = ::Sad::Types::repr::kMapSlotBytes;
                constexpr int64_t kFieldCount = ::Sad::Types::repr::kMapFieldCount;
                constexpr int64_t kFieldCapacity = ::Sad::Types::repr::kMapFieldCapacity;
                constexpr int64_t kFieldKeys = ::Sad::Types::repr::kMapFieldKeys;
                constexpr int64_t kFieldValues = ::Sad::Types::repr::kMapFieldValues;
                constexpr int64_t kFieldTypes = ::Sad::Types::repr::kMapFieldTypes;

                llvm::Value *srcMap = normalizeMapPtr(args[0], "mcopy.src.ptr");

                auto loadField = [&](int64_t field, const char *nm) {
                    llvm::Value *gep = cg_.builder_->CreateGEP(
                        i64Ty, srcMap, {llvm::ConstantInt::get(i64Ty, field)}, nm);
                    return cg_.builder_->CreateLoad(i64Ty, gep, nm);
                };
                llvm::Value *count = loadField(kFieldCount, "mcopy.count");
                llvm::Value *capacity = loadField(kFieldCapacity, "mcopy.cap");
                llvm::Value *srcKeys = cg_.builder_->CreateIntToPtr(
                    loadField(kFieldKeys, "mcopy.keys.i64"), ptrTy, "mcopy.keys.ptr");
                llvm::Value *srcValues = cg_.builder_->CreateIntToPtr(
                    loadField(kFieldValues, "mcopy.vals.i64"), ptrTy, "mcopy.vals.ptr");
                llvm::Value *srcTypes = cg_.builder_->CreateIntToPtr(
                    loadField(kFieldTypes, "mcopy.types.i64"), ptrTy, "mcopy.types.ptr");

                llvm::Value *arrayBytes = cg_.builder_->CreateMul(
                    capacity, llvm::ConstantInt::get(i64Ty, kSlotBytes), "mcopy.arr.bytes");
                llvm::Value *newMap = cg_.emitMalloc(
                    llvm::ConstantInt::get(i64Ty, kMapHeaderBytes), "mcopy.map");
                llvm::Value *newKeys = cg_.emitMalloc(arrayBytes, "mcopy.new.keys");
                llvm::Value *newValues = cg_.emitMalloc(arrayBytes, "mcopy.new.vals");
                llvm::Value *newTypes = cg_.emitMalloc(arrayBytes, "mcopy.new.types");

                cg_.builder_->CreateMemCpy(newKeys, llvm::MaybeAlign(8),
                                           srcKeys, llvm::MaybeAlign(8), arrayBytes);
                cg_.builder_->CreateMemCpy(newValues, llvm::MaybeAlign(8),
                                           srcValues, llvm::MaybeAlign(8), arrayBytes);
                cg_.builder_->CreateMemCpy(newTypes, llvm::MaybeAlign(8),
                                           srcTypes, llvm::MaybeAlign(8), arrayBytes);

                auto storeField = [&](int64_t field, llvm::Value *value, const char *nm) {
                    llvm::Value *gep = cg_.builder_->CreateGEP(
                        i64Ty, newMap, {llvm::ConstantInt::get(i64Ty, field)}, nm);
                    cg_.builder_->CreateStore(value, gep);
                };
                storeField(kFieldCount, count, "mcopy.dst.count");
                storeField(kFieldCapacity, capacity, "mcopy.dst.cap");
                storeField(kFieldKeys, cg_.builder_->CreatePtrToInt(newKeys, i64Ty), "mcopy.dst.keys");
                storeField(kFieldValues, cg_.builder_->CreatePtrToInt(newValues, i64Ty), "mcopy.dst.vals");
                storeField(kFieldTypes, cg_.builder_->CreatePtrToInt(newTypes, i64Ty), "mcopy.dst.types");

                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = newMap;
                return newMap;
            }

            // (AR) [م-٠٠١ ق٢] ازل_تشكيل — نظيرُ المفسّرِ في المصرّف.
            // (EN) [card م-٠٠١, ق٢] Strip diacritics — the compiler counterpart.
            if (funcName == kRuntimeStripDiacritics)
            {
                if (args.empty())
                    return nullptr;
                llvm::Value *result = cg_.builder_->CreateCall(
                    getOrCreateStripDiacritics(), {args[0]}, "strip.result");
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = result;
                return result;
            }

            return std::nullopt;
        }

        // ================================================================
        // (AR) دوال مساعدة للخرائط — تُنشأ كدوال LLVM داخلية عند الحاجة
        // (EN) Map helper functions — created as internal LLVM functions on-demand
        // ================================================================

        /**
         * @brief (AR) بناءُ `__sad_strip_diacritics` — ptr(ptr) [م-٠٠١ ق٢]
         * @brief (EN) Build `__sad_strip_diacritics` — ptr(ptr) [card م-٠٠١, ق٢]
         *
         * (AR) التشكيلُ العربيُّ يقعُ في مجالَين، وكلاهما مِحرفان في UTF-8 ببادئةٍ
         *      مميّزةٍ لا تظهرُ إلّا بادئةً، فيكفي مسحٌ بايتيٌّ بلا فكِّ ترميزٍ كامل:
         *        U+064B..U+065F (الفتحتان…الهمزةُ تحت) ⇒ D9 8B..9F
         *        U+0610..U+061A (علاماتُ الضبطِ القرآنيّة) ⇒ D8 90..9A
         *      وهذا هو المجالان اللذان يحذفُهما المفسّرُ حرفًا بحرف
         *      (builtin_module_maps_text.cpp) — التطابقُ مقصودٌ لا مصادفة.
         *
         *      يُقرأ البايتُ التالي بلا حارسٍ عمدًا: النصُّ منتهٍ بصفرٍ فالفهرسُ
         *      i+1 عندَ i<len يقعُ داخلَ المخزنِ يقينًا (أقصاه بايتُ الصفر).
         * (EN) Arabic diacritics live in two ranges, both two-byte UTF-8 with a lead
         *      byte that never appears as a continuation byte, so a plain byte scan
         *      suffices — no full decoding:
         *        U+064B..U+065F ⇒ D9 8B..9F   ·   U+0610..U+061A ⇒ D8 90..9A
         *      These are exactly the ranges the interpreter drops
         *      (builtin_module_maps_text.cpp) — the match is deliberate.
         *
         *      The next byte is read without a bounds guard on purpose: the string is
         *      NUL-terminated, so index i+1 for i<len is always inside the buffer (at
         *      worst it is the terminator itself).
         */
        llvm::Function *MapOpsCodeGen::getOrCreateStripDiacritics()
        {
            llvm::Function *existing = cg_.module_->getFunction(kRuntimeStripDiacritics);
            if (existing && !existing->empty())
                return existing;

            auto *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto *i64Ty = cg_.getInt64Type();
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // (AR) حدودُ المجالَين — ثوابتُ مسمّاةٌ لا أرقامٌ عارية.
            // (EN) The two range bounds — named constants, not bare numbers.
            constexpr uint8_t kLeadHarakat = 0xD9;      // U+0640..U+067F
            constexpr uint8_t kHarakatFirst = 0x8B;     // U+064B
            constexpr uint8_t kHarakatLast = 0x9F;      // U+065F
            constexpr uint8_t kLeadQuranicMark = 0xD8;  // U+0600..U+063F
            constexpr uint8_t kQuranicMarkFirst = 0x90; // U+0610
            constexpr uint8_t kQuranicMarkLast = 0x9A;  // U+061A

            auto *fnType = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
            llvm::Function *fn = llvm::Function::Create(
                fnType, llvm::Function::InternalLinkage, kRuntimeStripDiacritics, cg_.module_.get());

            llvm::BasicBlock *savedBB = cg_.builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint;
            const bool hasSavedPoint = (savedBB != nullptr);
            if (hasSavedPoint)
                savedPoint = cg_.builder_->GetInsertPoint();

            auto *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            auto *nullBB = llvm::BasicBlock::Create(*cg_.context_, "src.null", fn);
            auto *setupBB = llvm::BasicBlock::Create(*cg_.context_, "setup", fn);
            auto *checkBB = llvm::BasicBlock::Create(*cg_.context_, "loop.check", fn);
            auto *bodyBB = llvm::BasicBlock::Create(*cg_.context_, "loop.body", fn);
            auto *skipBB = llvm::BasicBlock::Create(*cg_.context_, "skip.mark", fn);
            auto *copyBB = llvm::BasicBlock::Create(*cg_.context_, "copy.byte", fn);
            auto *doneBB = llvm::BasicBlock::Create(*cg_.context_, "done", fn);

            llvm::Argument *srcArg = fn->arg_begin();
            srcArg->setName("src");

            // (AR) عدّادان في الذاكرة (readIndex/writeIndex) — أوضحُ من شبكةِ PHI،
            //      وmem2reg يرفعُهما إلى سجلّاتٍ في التحسين.
            // (EN) Two in-memory counters (read/write index) — clearer than a PHI web;
            //      mem2reg promotes them to registers during optimization.
            cg_.builder_->SetInsertPoint(entry);
            llvm::Value *readIndexSlot = cg_.builder_->CreateAlloca(i64Ty, nullptr, "read.index");
            llvm::Value *writeIndexSlot = cg_.builder_->CreateAlloca(i64Ty, nullptr, "write.index");
            llvm::Value *isNullSrc = cg_.builder_->CreateICmpEQ(
                srcArg, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)),
                "src.is.null");
            cg_.builder_->CreateCondBr(isNullSrc, nullBB, setupBB);

            // (AR) مصدرٌ عدمٌ ⇒ نُعيدُه كما هو؛ المتصلُ يتعاملُ مع العدمِ كعادتِه.
            // (EN) Null source ⇒ return it unchanged; the caller handles null as usual.
            cg_.builder_->SetInsertPoint(nullBB);
            cg_.builder_->CreateRet(srcArg);

            cg_.builder_->SetInsertPoint(setupBB);
            // (AR) `cg_.emitStrlen` لا تصريحٌ يدويّ: توقيعُ `strlen` يُعيدُ `size_t` وهو
            //      i32 على ٣٢-بت، فتصريحٌ ثابتٌ بـi64 يقرأُ النصفَ الأعلى قمامةً من edx.
            //      المُصدِراتُ المركزيّةُ هي **المسارُ الوحيدُ المسموحُ** لهذه النداءات.
            // (EN) `cg_.emitStrlen`, not a hand-rolled declaration: `strlen` returns `size_t`,
            //      which is i32 on 32-bit, so a hardcoded i64 declaration reads the high half as
            //      garbage from edx. The central emitters are the **sole sanctioned path**.
            llvm::Value *length = cg_.emitStrlen(srcArg, "src.len");
            // (AR) الناتجُ لا يزيدُ على المصدرِ قطُّ (حذفٌ فقط) + بايتُ الإنهاء.
            // (EN) The result never exceeds the source (removal only) + terminator.
            llvm::Value *outSize = cg_.builder_->CreateAdd(
                length, llvm::ConstantInt::get(i64Ty, 1), "out.size");
            llvm::Value *outPtr = cg_.emitMalloc(outSize, "strip.out");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), readIndexSlot);
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), writeIndexSlot);
            cg_.builder_->CreateBr(checkBB);

            cg_.builder_->SetInsertPoint(checkBB);
            llvm::Value *readIndex = cg_.builder_->CreateLoad(i64Ty, readIndexSlot, "i");
            llvm::Value *atEnd = cg_.builder_->CreateICmpUGE(readIndex, length, "at.end");
            cg_.builder_->CreateCondBr(atEnd, doneBB, bodyBB);

            cg_.builder_->SetInsertPoint(bodyBB);
            llvm::Value *leadPtr = cg_.builder_->CreateGEP(i8Ty, srcArg, {readIndex}, "lead.ptr");
            llvm::Value *leadByte = cg_.builder_->CreateLoad(i8Ty, leadPtr, "lead");
            llvm::Value *nextIndex = cg_.builder_->CreateAdd(
                readIndex, llvm::ConstantInt::get(i64Ty, 1), "i.next");
            llvm::Value *trailPtr = cg_.builder_->CreateGEP(i8Ty, srcArg, {nextIndex}, "trail.ptr");
            llvm::Value *trailByte = cg_.builder_->CreateLoad(i8Ty, trailPtr, "trail");

            auto byteEquals = [&](llvm::Value *v, uint8_t c, const char *nm) {
                return cg_.builder_->CreateICmpEQ(v, llvm::ConstantInt::get(i8Ty, c), nm);
            };
            auto byteInRange = [&](llvm::Value *v, uint8_t lo, uint8_t hi, const char *nm) {
                llvm::Value *ge = cg_.builder_->CreateICmpUGE(v, llvm::ConstantInt::get(i8Ty, lo));
                llvm::Value *le = cg_.builder_->CreateICmpULE(v, llvm::ConstantInt::get(i8Ty, hi));
                return cg_.builder_->CreateAnd(ge, le, nm);
            };

            llvm::Value *isHarakat = cg_.builder_->CreateAnd(
                byteEquals(leadByte, kLeadHarakat, "lead.is.d9"),
                byteInRange(trailByte, kHarakatFirst, kHarakatLast, "trail.in.harakat"),
                "is.harakat");
            llvm::Value *isQuranicMark = cg_.builder_->CreateAnd(
                byteEquals(leadByte, kLeadQuranicMark, "lead.is.d8"),
                byteInRange(trailByte, kQuranicMarkFirst, kQuranicMarkLast, "trail.in.quranic"),
                "is.quranic");
            llvm::Value *isDiacritic = cg_.builder_->CreateOr(isHarakat, isQuranicMark, "is.diacritic");
            cg_.builder_->CreateCondBr(isDiacritic, skipBB, copyBB);

            // (AR) تشكيلٌ ⇒ نتجاوزُ بايتَيه معًا بلا كتابة.
            // (EN) A diacritic ⇒ skip both of its bytes, writing nothing.
            cg_.builder_->SetInsertPoint(skipBB);
            cg_.builder_->CreateStore(
                cg_.builder_->CreateAdd(readIndex, llvm::ConstantInt::get(i64Ty, 2), "i.plus2"),
                readIndexSlot);
            cg_.builder_->CreateBr(checkBB);

            // (AR) بايتٌ عاديٌّ ⇒ نسخٌ بايتًا بايتًا؛ بايتاتُ الاستمرارِ تُنسخُ تباعًا
            //      فتبقى المِحارفُ متعدّدةُ البايتاتِ سليمةً.
            // (EN) An ordinary byte ⇒ copy it; continuation bytes are copied in turn,
            //      so multi-byte characters stay intact.
            cg_.builder_->SetInsertPoint(copyBB);
            llvm::Value *writeIndex = cg_.builder_->CreateLoad(i64Ty, writeIndexSlot, "j");
            llvm::Value *outSlot = cg_.builder_->CreateGEP(i8Ty, outPtr, {writeIndex}, "out.slot");
            cg_.builder_->CreateStore(leadByte, outSlot);
            cg_.builder_->CreateStore(nextIndex, readIndexSlot);
            cg_.builder_->CreateStore(
                cg_.builder_->CreateAdd(writeIndex, llvm::ConstantInt::get(i64Ty, 1), "j.plus1"),
                writeIndexSlot);
            cg_.builder_->CreateBr(checkBB);

            cg_.builder_->SetInsertPoint(doneBB);
            llvm::Value *finalWrite = cg_.builder_->CreateLoad(i64Ty, writeIndexSlot, "j.final");
            llvm::Value *termSlot = cg_.builder_->CreateGEP(i8Ty, outPtr, {finalWrite}, "out.term");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), termSlot);
            cg_.builder_->CreateRet(outPtr);

            if (hasSavedPoint)
                cg_.builder_->SetInsertPoint(savedBB, savedPoint);
            return fn;
        }

        /**
         * @brief (AR) إنشاء/استرجاع دالة __sad_map_find_slot — بحث خطي عن مفتاح أو أول خانة فارغة
         *        (EN) Get or create __sad_map_find_slot — linear scan for key or first empty slot
         *
         * @return llvm::Function* (AR) الدالة: (ptr keysArr, i64 capacity, ptr key) → i64 slotIndex
         *
         * (AR) الخوارزمية: لكل i من 0 إلى capacity-1:
         *      إذا keys[i] == null → ارجع i (خانة فارغة)
         *      إذا strcmp(keys[i], key) == 0 → ارجع i (مفتاح موجود)
         *      خلاف ذلك → استمر
         *      إذا وصلنا النهاية بدون إيجاد → ارجع 0 (fallback)
         */
        llvm::Function *MapOpsCodeGen::getOrCreateMapFindSlot()
        {
            const char *fnName = "__sad_map_find_slot";
            llvm::Function *fn = cg_.module_->getFunction(fnName);
            if (fn)
                return fn;

            auto *i64Ty = cg_.getInt64Type();
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto *i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            // (AR) التوقيع: (ptr keysArr, i64 capacity, ptr key) → i64
            auto *fnType = llvm::FunctionType::get(i64Ty, {ptrTy, i64Ty, ptrTy}, false);
            fn = llvm::Function::Create(fnType, llvm::Function::InternalLinkage, fnName, *cg_.module_);

            auto *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            auto *loop = llvm::BasicBlock::Create(*cg_.context_, "loop", fn);
            auto *body = llvm::BasicBlock::Create(*cg_.context_, "body", fn);
            auto *checkKey = llvm::BasicBlock::Create(*cg_.context_, "check.key", fn);
            auto *found = llvm::BasicBlock::Create(*cg_.context_, "found", fn);
            auto *next = llvm::BasicBlock::Create(*cg_.context_, "next", fn);
            auto *notFound = llvm::BasicBlock::Create(*cg_.context_, "not.found", fn);

            auto argIt = fn->arg_begin();
            llvm::Value *keysArr = &*argIt++;
            llvm::Value *capacity = &*argIt++;
            llvm::Value *searchKey = &*argIt++;

            llvm::IRBuilder<> b(*cg_.context_);

            // entry: → loop
            b.SetInsertPoint(entry);
            b.CreateBr(loop);

            // loop: PHI idx = 0, cmp idx < capacity
            b.SetInsertPoint(loop);
            auto *idx = b.CreatePHI(i64Ty, 2, "idx");
            idx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            auto *done = b.CreateICmpUGE(idx, capacity, "done");
            b.CreateCondBr(done, notFound, body);

            // body: load keys[idx]
            b.SetInsertPoint(body);
            auto *keyGep = b.CreateGEP(ptrTy, keysArr, {idx}, "key.gep");
            auto *slotKey = b.CreateLoad(ptrTy, keyGep, "slot.key");
            auto *isNull = b.CreateICmpEQ(slotKey,
                                          llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "is.null");
            b.CreateCondBr(isNull, found, checkKey);

            // checkKey: strcmp(keys[idx], searchKey) == 0?
            b.SetInsertPoint(checkKey);
            auto *strcmpType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto strcmpFn = cg_.module_->getOrInsertFunction("strcmp", strcmpType);
            auto *cmpRes = b.CreateCall(strcmpFn, {slotKey, searchKey}, "cmp");
            auto *isMatch = b.CreateICmpEQ(cmpRes, b.getInt32(0), "match");
            b.CreateCondBr(isMatch, found, next);

            // found: return idx
            b.SetInsertPoint(found);
            b.CreateRet(idx);

            // next: idx++, → loop
            b.SetInsertPoint(next);
            auto *nextIdx = b.CreateAdd(idx, llvm::ConstantInt::get(i64Ty, 1), "next.idx");
            idx->addIncoming(nextIdx, next);
            b.CreateBr(loop);

            // notFound: return 0 (fallback — shouldn't happen with proper capacity)
            b.SetInsertPoint(notFound);
            b.CreateRet(llvm::ConstantInt::get(i64Ty, 0));

            return fn;
        }

        /**
         * @brief (AR) إنشاء/استرجاع دالة __sad_map_collect — تجميع العناصر غير الفارغة في مصفوفة SadArray
         *        (EN) Get or create __sad_map_collect — collect non-null map entries into SadArray
         *
         * @return llvm::Function* (AR) الدالة: (ptr keysArr, ptr srcArr, i64 capacity, i64 count) → ptr SadArray
         *
         * (AR) بنية SadArray: {i64 length, i64 capacity, ptr data}
         *      تخصص المصفوفة + بيانات، ثم تنسخ العناصر غير الفارغة
         */
        llvm::Function *MapOpsCodeGen::getOrCreateMapCollect()
        {
            const char *fnName = "__sad_map_collect";
            llvm::Function *fn = cg_.module_->getFunction(fnName);
            if (fn)
                return fn;

            auto *i64Ty = cg_.getInt64Type();
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // (AR) التوقيع: (ptr keysArr, ptr srcArr, i64 capacity, i64 count, i8 homogKind) → ptr
            //      homogKind: وسمُ عنصر المصفوفة الناتجة (Str للمفاتيح، عدد للقيم) — يُخزَّن في
            //      الحقل ٤ فتُقرأ الفهرسةُ عبر مسار Any موسومةً لا مؤشّرًا خامًّا.
            // (EN) homogKind: the DynKind of the collected array's elements (Str for keys, Int
            //      for values) — stored in field 4 so an Any-context index reads it tagged.
            // (AR) [م-٠٠١] المعامل السادس `typesArr`: مصفوفةُ أوسامِ الخريطة. إن كان عدمًا
            //      فالناتجُ متجانسٌ بوسمِ `homogKind` (حالُ المفاتيح: نصوصٌ قطعًا). وإن كان
            //      حاضرًا فالقيمُ مختلطةٌ، فيُبنى للناتجِ **مخزنُ أوسام** (الحقلُ ٣) بوسمِ
            //      كلِّ عنصرٍ على حدة، فتقرؤُها الفهرسةُ موسومةً كما تقرأُ أيَّ مصفوفةٍ
            //      مختلطة. كان الوسمُ يُثبَّت «عددًا» لكلِّ القيم، فتُطبَعُ مؤشّراتُ النصوصِ
            //      وبتّاتُ العشريِّ أعدادًا فلكيّة.
            // (EN) [card م-٠٠١] The sixth parameter `typesArr`: the map's tag array. If null the
            //      result is homogeneous under `homogKind` (the keys case: always strings). If
            //      present the values are mixed, so the result gets a **tags buffer** (field 3)
            //      holding each element's own kind, and indexing reads them tagged exactly as it
            //      reads any heterogeneous array. The kind used to be pinned to Int for every
            //      value, so string pointers and float bits printed as astronomical integers.
            auto *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto *fnType =
                llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty, i64Ty, i8Ty, ptrTy}, false);
            fn = llvm::Function::Create(fnType, llvm::Function::InternalLinkage, fnName, *cg_.module_);

            auto *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            auto *loop = llvm::BasicBlock::Create(*cg_.context_, "loop", fn);
            auto *body = llvm::BasicBlock::Create(*cg_.context_, "body", fn);
            auto *copySlot = llvm::BasicBlock::Create(*cg_.context_, "copy.slot", fn);
            auto *skip = llvm::BasicBlock::Create(*cg_.context_, "skip", fn);
            auto *done = llvm::BasicBlock::Create(*cg_.context_, "done", fn);

            auto argIt = fn->arg_begin();
            llvm::Value *keysArr = &*argIt++;
            llvm::Value *srcArr = &*argIt++;
            llvm::Value *capacity = &*argIt++;
            llvm::Value *count = &*argIt++;
            llvm::Value *homogKindArg = &*argIt++;
            llvm::Value *typesArr = &*argIt++;

            llvm::IRBuilder<> b(*cg_.context_);

            // entry: allocate SadArray {len, cap, data}
            b.SetInsertPoint(entry);
            // (AR) بانٍ محلّيّ (b) لا cg_.builder_ ⇒ لا نستطيع cg_.emitMalloc؛
            //      نصرّح بنوع size_t الهدف ونكيّف الوسائط يدويًّا.
            llvm::Type *szTy = cg_.getSizeType();
            auto *mallocType = llvm::FunctionType::get(ptrTy, {szTy}, false);
            auto mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocType);

            // (AR) بنية SadArray الكاملة {len, cap, data, tags, homogKind} — نحسب حجمها
            //      من التخطيط (لا 24 الثابتة القديمة التي كانت تُخصّص ٣ حقولٍ فقط ⇒ قراءةُ
            //      الحقلين ٣/٤ عبر مسار Any كانت تتجاوز التخصيص ⇒ انهيار كومة).
            // (EN) Full SadArray struct {len, cap, data, tags, homogKind} — size from the data
            //      layout (not the old hardcoded 24, which allocated only 3 fields ⇒ reading
            //      fields 3/4 via the Any path over-read the allocation ⇒ heap crash).
            auto *ptrTyL = llvm::PointerType::getUnqual(*cg_.context_);
            auto *sadArrTy = llvm::StructType::get(*cg_.context_, {i64Ty, i64Ty, ptrTyL, ptrTyL, i8Ty});
            uint64_t sadArrSz = cg_.module_->getDataLayout().getTypeAllocSize(sadArrTy);
            llvm::Value *arrPtr = b.CreateCall(mallocFn,
                                               {llvm::ConstantInt::get(szTy, sadArrSz)}, "arr.ptr");
            // (AR) مصفوفة البيانات: count * 8 bytes
            llvm::Value *dataBytes = b.CreateMul(count, llvm::ConstantInt::get(i64Ty, 8), "data.bytes");
            // (AR) ضمان عدم تخصيص 0 bytes (للخرائط الفارغة)
            llvm::Value *minBytes = llvm::ConstantInt::get(i64Ty, 8);
            llvm::Value *cmpZero = b.CreateICmpUGT(dataBytes, llvm::ConstantInt::get(i64Ty, 0), "cmp.zero");
            llvm::Value *safeBytes = b.CreateSelect(cmpZero, dataBytes, minBytes, "safe.bytes");
            llvm::Value *dataPtr = b.CreateCall(mallocFn,
                {b.CreateZExtOrTrunc(safeBytes, szTy, "safe.bytes.sz")}, "data.ptr");

            // (AR) تخزين: [0]=length=0 (سنزيدها), [1]=cap=count, [2]=data
            auto *lenGep = b.CreateGEP(i64Ty, arrPtr, {llvm::ConstantInt::get(i64Ty, 0)}, "len.gep");
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), lenGep);
            auto *capGep = b.CreateGEP(i64Ty, arrPtr, {llvm::ConstantInt::get(i64Ty, 1)}, "cap.gep");
            b.CreateStore(count, capGep);
            auto *datGep = b.CreateGEP(i64Ty, arrPtr, {llvm::ConstantInt::get(i64Ty, 2)}, "dat.gep");
            b.CreateStore(b.CreatePtrToInt(dataPtr, i64Ty), datGep);

            // (AR) الحقل ٣ (tags) = null (متجانسة، مسارٌ ساكن)؛ الحقل ٤ (homogKind) = الوسمُ
            //      المُمرَّر (Str للمفاتيح، عدد للقيم). بدونهما تُقرأ عبر مسار Any قمامةً/تنهار.
            // (EN) Field 3 (tags) = null (homogeneous, static path); field 4 (homogKind) = the
            //      passed kind (Str for keys, Int for values). Without them the Any read reads
            //      garbage / crashes.
            // (AR) [م-٠٠١] فإن مُرِّرت مصفوفةُ الأوسامِ خُصِّص للناتجِ مخزنُ أوسامٍ بحجمِ
            //      العناصرِ ووُضِع في الحقلِ ٣، فتسلكُ القراءةُ مسارَ المصفوفةِ المختلطة.
            // (EN) [card م-٠٠١] When a tags array is passed, a tags buffer of the element count is
            //      allocated for the result and placed in field 3, so reads take the
            //      heterogeneous-array path.
            llvm::Value *hasTypes = b.CreateICmpNE(
                typesArr, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)),
                "has.types");
            // (AR) بلا أوسامٍ ⇒ بايتٌ واحدٌ رمزيٌّ لا يُقرأُ قطُّ (الحقلُ ٣ يبقى عدمًا)،
            //      كي يبقى المسارُ بلا فرعٍ ولا يُخصَّصَ ما لا يُستعمَل.
            // (EN) No tags ⇒ a single token byte that is never read (field 3 stays null), keeping
            //      the path branch-free without allocating what is not used.
            llvm::Value *tagsNeeded = b.CreateSelect(cmpZero, count,
                                                     llvm::ConstantInt::get(i64Ty, 1), "tags.needed");
            llvm::Value *tagsBytes = b.CreateSelect(hasTypes, tagsNeeded,
                                                    llvm::ConstantInt::get(i64Ty, 1), "tags.bytes");
            llvm::Value *tagsBuf = b.CreateCall(
                mallocFn, {b.CreateZExtOrTrunc(tagsBytes, szTy, "tags.bytes.sz")}, "tags.buf");
            llvm::Value *tagsField = b.CreateSelect(
                hasTypes, tagsBuf,
                llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "tags.field");
            auto *tagsGepC = b.CreateGEP(i64Ty, arrPtr, {llvm::ConstantInt::get(i64Ty, 3)}, "tags.gep");
            b.CreateStore(tagsField, tagsGepC);
            auto *hkGepC = b.CreateGEP(i64Ty, arrPtr, {llvm::ConstantInt::get(i64Ty, 4)}, "homogkind.slot");
            b.CreateStore(homogKindArg, hkGepC); // i8 store at byte offset 32 (field 4)

            b.CreateBr(loop);

            // loop: PHI srcIdx=0, dstIdx=0
            b.SetInsertPoint(loop);
            auto *srcIdx = b.CreatePHI(i64Ty, 2, "src.idx");
            srcIdx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            auto *dstIdx = b.CreatePHI(i64Ty, 2, "dst.idx");
            dstIdx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            auto *isDone = b.CreateICmpUGE(srcIdx, capacity, "is.done");
            b.CreateCondBr(isDone, done, body);

            // body: check if keys[srcIdx] != null
            b.SetInsertPoint(body);
            auto *keyGep = b.CreateGEP(ptrTy, keysArr, {srcIdx}, "key.gep");
            auto *slotKey = b.CreateLoad(ptrTy, keyGep, "slot.key");
            auto *isNull = b.CreateICmpEQ(slotKey,
                                          llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "is.null");
            b.CreateCondBr(isNull, skip, copySlot);

            // copySlot: data[dstIdx] = src[srcIdx], dstIdx++
            b.SetInsertPoint(copySlot);
            auto *srcGep = b.CreateGEP(i64Ty, srcArr, {srcIdx}, "src.gep");
            auto *srcVal = b.CreateLoad(i64Ty, srcGep, "src.val");
            auto *dstGep = b.CreateGEP(i64Ty, dataPtr, {dstIdx}, "dst.gep");
            b.CreateStore(srcVal, dstGep);

            // ════════════════════════════════════════════════════════════════
            // (AR) [م-٠٠١] وسمُ العنصرِ الناتج: يُترجَمُ وسمُ الخريطةِ إلى وسمِ %SadDyn.
            //      المسارُ بلا فروعٍ عمدًا، وأمانُه من موضعَين:
            //      • **قاعدةُ القراءة** تُبدَّلُ إلى `srcArr` حين لا أوسامَ، وهي مصفوفةٌ
            //        صالحةٌ بسعةِ `capacity` و`srcIdx < capacity` ⇒ لا قراءةَ من عدم.
            //        (اختيارُ المؤشّرِ قبلَ القراءةِ لا بعدَها — الاختيارُ بعدَ القراءةِ
            //        لا يمنعُ القراءةَ نفسَها.)
            //      • **فهرسُ الكتابة** يُثبَّتُ صفرًا حين لا أوسامَ، والمخزنُ حينَها
            //        بايتٌ واحدٌ ⇒ لا تجاوُز. والحقلُ ٣ عدمٌ فلا يقرأُ أحدٌ ما كُتِب.
            // (EN) [card م-٠٠١] The result element's kind: the map tag translated to a %SadDyn
            //      kind. The path is deliberately branch-free, and safe on two counts:
            //      • the **load base** is switched to `srcArr` when there are no tags — a valid
            //        array of `capacity` entries with `srcIdx < capacity` ⇒ no null read.
            //        (Selecting the pointer *before* the load, not after: selecting afterwards
            //        would not prevent the load itself.)
            //      • the **store index** is pinned to zero when there are no tags, and the buffer
            //        is then a single byte ⇒ no overrun. Field 3 is null, so nobody reads it.
            // ════════════════════════════════════════════════════════════════
            auto *typesBase = b.CreateSelect(hasTypes, typesArr, srcArr, "types.base");
            auto *typeGepC = b.CreateGEP(i64Ty, typesBase, {srcIdx}, "type.gep");
            auto *rawTag = b.CreateLoad(i64Ty, typeGepC, "src.type");
            auto mapTagIs = [&](int64_t t, const char *nm) {
                return b.CreateICmpEQ(rawTag, llvm::ConstantInt::get(i64Ty, t), nm);
            };
            llvm::Value *elemKind = llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Str);
            elemKind = b.CreateSelect(mapTagIs(Sad::Compiler::kMapValueTagInteger, "t.is.int"),
                                      llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Int),
                                      elemKind, "elem.k.int");
            elemKind = b.CreateSelect(mapTagIs(Sad::Compiler::kMapValueTagFloat, "t.is.float"),
                                      llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Float),
                                      elemKind, "elem.k.float");
            elemKind = b.CreateSelect(mapTagIs(Sad::Compiler::kMapValueTagBoolean, "t.is.bool"),
                                      llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Bool),
                                      elemKind, "elem.k.bool");
            elemKind = b.CreateSelect(mapTagIs(Sad::Compiler::kMapValueTagNull, "t.is.null"),
                                      llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Null),
                                      elemKind, "elem.k.null");
            elemKind = b.CreateSelect(mapTagIs(Sad::Compiler::kMapValueTagVoid, "t.is.void"),
                                      llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Void),
                                      elemKind, "elem.k.void");
            elemKind = b.CreateSelect(mapTagIs(Sad::Compiler::kMapValueTagMap, "t.is.map"),
                                      llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Map),
                                      elemKind, "elem.k.map");
            elemKind = b.CreateSelect(mapTagIs(Sad::Compiler::kMapValueTagArray, "t.is.array"),
                                      llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Array),
                                      elemKind, "elem.k.array");
            llvm::Value *tagIdx = b.CreateSelect(hasTypes, dstIdx,
                                                 llvm::ConstantInt::get(i64Ty, 0), "tag.idx");
            auto *tagDstGep = b.CreateGEP(i8Ty, tagsBuf, {tagIdx}, "tag.dst.gep");
            b.CreateStore(elemKind, tagDstGep);
            auto *nextDst = b.CreateAdd(dstIdx, llvm::ConstantInt::get(i64Ty, 1), "next.dst");
            auto *nextSrc1 = b.CreateAdd(srcIdx, llvm::ConstantInt::get(i64Ty, 1), "next.src1");
            srcIdx->addIncoming(nextSrc1, copySlot);
            dstIdx->addIncoming(nextDst, copySlot);
            b.CreateBr(loop);

            // skip: srcIdx++, dstIdx stays
            b.SetInsertPoint(skip);
            auto *nextSrc2 = b.CreateAdd(srcIdx, llvm::ConstantInt::get(i64Ty, 1), "next.src2");
            srcIdx->addIncoming(nextSrc2, skip);
            dstIdx->addIncoming(dstIdx, skip);
            b.CreateBr(loop);

            // done: store final length, return arrPtr
            b.SetInsertPoint(done);
            b.CreateStore(dstIdx, lenGep);
            b.CreateRet(arrPtr);

            return fn;
        }

    } // namespace LLVM
} // namespace Sad
