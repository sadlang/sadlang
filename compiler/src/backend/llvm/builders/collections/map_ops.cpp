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
                llvm::Value *doubled = cg_.builder_->CreateMul(hint, llvm::ConstantInt::get(i64Ty, 2), "cap.hint");
                llvm::Value *minCap = llvm::ConstantInt::get(i64Ty, 8);
                llvm::Value *cmp = cg_.builder_->CreateICmpUGT(doubled, minCap, "cap.cmp");
                llvm::Value *capacity = cg_.builder_->CreateSelect(cmp, doubled, minCap, "cap.final");

                // (AR) تخصيص البنية الرئيسية: 5 حقول * 8 = 40 بايت

                llvm::Value *mapPtr = cg_.emitMalloc(llvm::ConstantInt::get(i64Ty, 40), "map.ptr");

                // (AR) تخصيص المصفوفات (keys, values, types)
                llvm::Value *arrBytes = cg_.builder_->CreateMul(capacity, llvm::ConstantInt::get(i64Ty, 8), "arr.bytes");
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
                llvm::Value *mapPtr = args[0];
                if (mapPtr->getType() == i64Ty)
                    mapPtr = cg_.builder_->CreateIntToPtr(mapPtr, ptrTy, "mset.map.ptr");
                llvm::Value *key = args[1];
                llvm::Value *value = args[2];
                llvm::Value *typeTag = args[3];

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
                llvm::Value *valAsI64;
                if (value->getType()->isPointerTy())
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
                if (typeTag->getType() == i64Ty)
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

            if (funcName == "__sad_map_get" || funcName == "__sad_map_get_i64")
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
                llvm::Value *mapPtr = args[0];
                if (mapPtr->getType() == i64Ty)
                    mapPtr = cg_.builder_->CreateIntToPtr(mapPtr, ptrTy, "mget.map.ptr");
                llvm::Value *key = args[1];

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

                if (funcName == "__sad_map_get")
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

                    // (AR) اختيار: نص أصلي أو الرقم المُحوّل
                    llvm::Value *result = cg_.builder_->CreateSelect(isString, strPtr, buf, "mget.result");

                    if (inst->result.has_value())
                        cg_.context_info_.namedValues[inst->result->name] = result;
                    return result;
                }
                else
                {
                    // __sad_map_get_i64: (AR) إرجاع i64 مباشرة — النوع المناسب للأرقام
                    // (EN) Return i64 directly — suitable for integer values
                    llvm::Value *result = valI64;

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
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

                // (AR) تحويل map من i64 إلى ptr
                llvm::Value *mapArg = args[0];
                if (mapArg->getType() == i64Ty)
                    mapArg = cg_.builder_->CreateIntToPtr(mapArg, ptrTy, "msize.map.ptr");
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
                llvm::Value *mapPtr = args[0];
                if (mapPtr->getType() == i64Ty)
                    mapPtr = cg_.builder_->CreateIntToPtr(mapPtr, ptrTy, "mhas.map.ptr");
                llvm::Value *key = args[1];

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

                llvm::Value *mapPtr = args[0];
                if (mapPtr->getType() == i64Ty)
                    mapPtr = cg_.builder_->CreateIntToPtr(mapPtr, ptrTy, "mdel.map.ptr");
                llvm::Value *key = args[1];

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
                llvm::Value *mapPtr = args[0];
                if (mapPtr->getType() == i64Ty)
                    mapPtr = cg_.builder_->CreateIntToPtr(mapPtr, ptrTy, "mkvs.map.ptr");
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
                // (AR) وسمُ عنصر الناتج: مفاتيحُ الخريطة نصوصٌ (Str)؛ قيمُها مختلطةٌ محفوظةٌ
                //      i64 خامًّا ⇒ عدد افتراضًا (لا يوقِف الانهيار على أيّ حال).
                // (EN) Result element kind: map keys are strings (Str); values are mixed raw
                //      i64 ⇒ default Int (either way it stops the crash).
                llvm::Value *collectHomogKind = llvm::ConstantInt::get(
                    llvm::Type::getInt8Ty(*cg_.context_),
                    isKeys ? Sad::LLVM::DynKind::Str : Sad::LLVM::DynKind::Int);
                llvm::Value *result = cg_.builder_->CreateCall(collectFn,
                                                           {keysArr, srcArr, cap, count, collectHomogKind}, "mkvs.result");

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

            return std::nullopt;
        }

        // ================================================================
        // (AR) دوال مساعدة للخرائط — تُنشأ كدوال LLVM داخلية عند الحاجة
        // (EN) Map helper functions — created as internal LLVM functions on-demand
        // ================================================================

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
            auto *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto *fnType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty, i64Ty, i8Ty}, false);
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
            auto *tagsGepC = b.CreateGEP(i64Ty, arrPtr, {llvm::ConstantInt::get(i64Ty, 3)}, "tags.gep");
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), tagsGepC); // null pointer (0 bits)
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
