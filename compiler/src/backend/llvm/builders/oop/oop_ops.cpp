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
#include "value.h" // (AR) أسماءُ الأنواعِ من `Value::getTypeName` لا من نصٍّ مكتوب / (EN) type names from Value
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


        // (AR) وسمُ السجلّ «%» تفصيلُ تمثيلٍ داخليٍّ لا يُعرَضُ على كاتبِ البرنامج:
        //      الرسالةُ تقولُ «س» لا «%س».
        // (EN) The '%' register sigil is an internal representation detail and is
        //      not shown to the program author.
        static std::string stripRegisterSigil(const std::string &registerName)
        {
            return (!registerName.empty() && registerName[0] == '%')
                       ? registerName.substr(1)
                       : registerName;
        }

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


        // ========================================================================
        // (AR) 🔑 عائلةُ **الكائنِ** كانت غائبةً عن بابِ الرفعِ العامّ
        //
        //      `emitRaiseIfNull` بابٌ عامٌّ لكلِّ عائلةِ قيم، ويُنادى من النصِّ
        //      والمصفوفةِ والخريطة — و**صفرَ مرّةٍ** من الكائن. فكان أبسطُ برنامجٍ
        //      ممكن:
        //
        //          متغير ك            ← خانةٌ تُهيّأ صفرًا
        //          اطبع(ك.حقل)
        //
        //      يُبنى بـrc=0 ثمّ **ينهار 0xC0000005**، بينما يرفعُ المفسّرُ
        //      RUN033 «نوع المعامل 'VOID' غير مدعوم في العملية 'member access'».
        //
        //      ⚠️ ولا علاقةَ لهذا بالمحلّل. كان الشكلُ الذي كشفه «صنف س ك»
        //      (معرِّفٌ زائدٌ بعد اسمِ الصنف) فبدا عطبًا نحويًّا، ونقضه الضابطُ:
        //      `متغير ك` — تصريحٌ صريحٌ سليمٌ لا يمسُّه ذاك الفرعُ البتّة —
        //      يُنتِج الانهيارَ عينَه. فالمحلّلُ **طريقٌ** إلى الشكلِ لا **علّتُه**،
        //      ورقعةٌ هناك كانت ستُخفي المِجَسَّ وتترك أقصرَ برنامجٍ ينهار.
        //
        //      🔑 **والموضعُ قبلَ حلِّ الصنفِ عمدًا.** وُضِع الحارسُ أوّلًا عند GEP
        //      فاجتاز البناءَ وأخضرَ مِجَسَّ الحقل — وترك الخاصّيّةَ تُخرِج `0`
        //      بـrc=0. وقراءةُ SIR أظهرت السبب: مُستقبِلٌ عدميٌّ **لا صنفَ له**،
        //      فيرتدُّ المُصدِرُ عند «No class mapping» قبل GEP بمراحل. والمفسّرُ
        //      يرفع **بلا أن يسأل عن الصنفِ أصلًا** — فالسؤالُ عن العضوِ لاؽٍ
        //      حين يكون المُستقبِلُ عدمًا. فليكن الحارسُ حيث لا يلزمُه صنف.
        //
        //      و`{type}` لا يُمرّر: البابُ يملؤه من **شكلِ** العدمِ (VOID لخانةٍ
        //      صفريّة، NULL لحارسِ العدم) كما يفرّق المفسّر — انظر تعليلَه هناك.
        //
        // (EN) 🔑 The OBJECT family was missing from the general raise door:
        //      emitRaiseIfNull is called from the string, array and map paths and ZERO
        //      times from the object path, so `var k` followed by `k.field` built with
        //      rc=0 and then segfaulted where the interpreter raises RUN033. NOT a parser
        //      defect: the shape that exposed it was `Class a b`, but the control - a
        //      plain, valid `var k` that never touches that branch - reproduces the
        //      identical crash.
        //      Placed BEFORE class resolution deliberately: the first placement, at the
        //      GEP, built and greened the field probe while a property still answered 0
        //      with rc=0. Reading the SIR showed why - a null receiver has no class, so
        //      the emitter bails at "No class mapping" long before the GEP. The
        //      interpreter raises without ever asking for a class, because asking which
        //      member is moot once the receiver is null. So the guard goes where no class
        //      is needed. {type} is filled by the door itself.
        // ========================================================================
        void OOPOpsCodeGen::raiseIfObjectReceiverIsNull(llvm::Value *objPtr, const char *tag,
                                                        const char *operation)
        {
            // (AR) تطبيعٌ **للفحصِ وحدَه**: `namedValues` تُعيد الخانةَ، وعنوانُ
            //      الخانةِ ليس عدمًا أبدًا — فحارسٌ يُوضَع عليه يُصدَر ولا يُطلَق أبدًا.
            //      ويبقى المسارُ الأصليُّ يُطبّع لنفسِه، فلا يتغيّر ترتيبُ الإصدارِ
            //      ولا أسماءُ السجلّات (السابقةُ نفسُها في `emitArraySet`).
            //      ⚠️ وخانةُ الهيكلِ استثناءٌ لا نسيان: `alloca %class.X` عنوانُها
            //      **هو** الكائن، فتحميلُها يقرأ أوّلَ حقلٍ مؤشّرًا.
            // (EN) Normalize FOR THE CHECK ONLY: `namedValues` hands back the slot, whose
            //      address is never null, so a guard on it emits and never fires. The
            //      original path still normalizes for itself, so emission order and
            //      register names are unchanged (same precedent as emitArraySet). A struct
            //      alloca is excluded deliberately: its address IS the object.
            if (!objPtr || !cg_.builder_ || !cg_.builder_->GetInsertBlock())
                return;

            llvm::Value *checked = objPtr;

            // ════════════════════════════════════════════════════════════════════
            // (AR) 🔑 الحالةُ الأولى: قيمةُ `%SadDyn` — **وهي التي فاتت أوّلَ مرّة،
            //      وأسقطها سطري أنا**. `loadDynSlot` أعلاه يُحوّل خانةَ «أي» إلى
            //      **قيمةٍ** هيكليّة، فلم تعُد تُطابِق فرعَي `dyn_cast` أدناه (ليست
            //      خانةً)، ومرّت إلى `emitRaiseIfNull` فارتدّ **صامتًا** عند
            //      `else return` لأنّ الهيكلَ ليس مؤشّرًا ولا صحيحًا: صفرُ تعليمةِ
            //      حراسةٍ تُصدَر.
            //
            //      والمقيس: `أي ك` ثمّ `ك.قيمة` ⇒ `0xC0000005`، و`أي ك = لاشيء`
            //      كذلك — بينما يرفع المرجعُ RUN033 بالشكلَين. أي أنّ إيداعَ
            //      `loadDynSlot` **أبطل** حارسَ الإيداعِ الذي يليه، والاثنان على
            //      السطرَين نفسِهما.
            //
            //      ⇒ **رقعةٌ في مسارٍ تُغيّر شرطَ دخولِ الرقعةِ التي تليه.** ولا
            //      يكشفه إلّا قياسُ التقاطع: البذرةُ ١٢٩ تقيس «أي» **حيًّا**،
            //      و٠٨٤–٠٨٨ تقيس العدمَ على `متغير` — و«أي × عدم» لم تكن في أيٍّ
            //      منهما، فبدت التغطيةُ تامّةً وفيها ثقبٌ بحجمِ حاصلِ الضرب.
            //
            //      والحمولةُ i64 هي التي تحمل الشكلَين اللذين يفرّق بينهما البابُ:
            //      `zeroinitializer` ⇒ صفرٌ ⇒ `VOID`، و`kSadNullSentinel` ⇒ `NULL`.
            //      ⚠️ ولا يُفحَص الوسمُ هنا كما يُفحَص في `array_ops.cpp:176`: وسمُ
            //      المؤشّرِ في هذه الشجرةِ **يكذب** (`Pointer ⇒ DynKind::Str` في
            //      `sad_dyn_repr.cpp:516`)، فالكائنُ في خانةِ «أي» موسومٌ نصًّا.
            //      وحارسٌ يُبنى على وسمٍ كاذبٍ يُخفِق على المُستقبِلِ السليم.
            // (EN) Case 1: a %SadDyn VALUE — the case that was missed, and my own
            //      loadDynSlot line is what created it: the slot becomes a struct
            //      value, matches neither dyn_cast below, and emitRaiseIfNull bails
            //      silently (not pointer, not integer) emitting ZERO guard code.
            //      Measured: `أي ك` then `ك.قيمة` segfaults where the reference
            //      raises RUN033. No tag check here (unlike array_ops.cpp:176): the
            //      pointer tag lies in this tree (Pointer => DynKind::Str), so an
            //      object in an «أي» slot is tagged as a string.
            // ════════════════════════════════════════════════════════════════════
            if (Sad::LLVM::isSadDyn(checked))
            {
                // (AR) ⚠️ والوسمُ يُفحَصُ الآنَ قبلَ إسقاطِ الحمولة: صار الوسمُ صادقًا (ISSUE-142)
                //      فلم يعُدِ النهيُ المكتوبُ أعلاه قائمًا. والترتيبُ لازمٌ: إسقاطُ
                //      الحمولةِ يمحو الوسمَ، فما بعدَه لا يملكُ تمييزَ عددٍ من مؤشّر.
                // (EN) The tag is checked BEFORE the payload is projected: it now tells the
                //      truth, and projection erases it — nothing downstream can tell an
                //      integer from a pointer.
                raiseIfDynReceiverIsNotObject(checked, tag, operation);
                checked = Sad::LLVM::dynPayloadI64(cg_, checked);
            }
            else if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(objPtr))
            {
                if (allocaInst->getAllocatedType()->isStructTy())
                    return;
                checked = cg_.builder_->CreateLoad(allocaInst->getAllocatedType(), allocaInst,
                                                   std::string(tag) + ".chk.load");
            }
            else if (auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(objPtr))
            {
                if (globalVar->getValueType()->isStructTy())
                    return;
                checked = cg_.builder_->CreateLoad(globalVar->getValueType(), globalVar,
                                                   std::string(tag) + ".chk.gload");
            }

            // (AR) 🔑 والمنفذانِ **لا يتشاركان اللفظَ**: المرجعُ يقول «member access» في
            //      القراءةِ و«.=» في الكتابة، وهما موضعان مختلفان في المفسّر
            //      (`expression_evaluator_members.cpp` مقابل `..._members_assign.cpp`).
            //      ⚠️ وقد مرّرتُ «member access» للمنفذَين أوّلًا فاجتاز البناءُ واخضرَّ
            //      منفذُ القراءةِ — وكشفت البذرةُ التوأمُ ٠٨٦ أنّ الكتابةَ ترفعُ **رفعًا
            //      صحيحًا بلفظٍ خطأ**: rc=1 والنصُّ يخالف المرجع. أي أنّ نصفَ العائلةِ
            //      كان سيُعلَن مكافئًا وهو ليس كذلك، ولا يكشفه إلّا قياسُ المنفذَين معًا.
            //      فاللفظُ معاملٌ لا ثابتًا، ويأتي من موضعِ النداءِ لا من البابِ.
            // (EN) The two ports do NOT share the label: the reference says
            //      "member access" on read and ".=" on write - two distinct sites in
            //      the interpreter. Passing "member access" for both compiled fine and
            //      greened the read port, while the write port raised correctly with the
            //      WRONG text (rc=1, text diverging from the reference). Only measuring
            //      both ports catches that, so the label is a parameter, not a constant.
            cg_.emitRaiseIfNull(checked, ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                                {{"operation", operation}}, tag);
        }

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) 🔑 المُستقبِلُ الديناميُ الذي **ليس كائنًا** — ما لم يكن حراستُه ممكنةً
        //
        //      `emitRaiseIfNull` يسألُ سؤالًا واحدًا: أهذه الحمولةُ عدمٌ؟ وحمولةُ العددِ
        //      ليست صفرًا ولا حارسًا، فتجتازُ البابَ وتبلُغُ ما بعدَه. والمقيس:
        //
        //          أي ك = شخص()      ← يُحلُّ الصنفُ من الإسنادِ الأوّل
        //          ك = 5
        //          اطبع_سطر(ك.اسم)   ← `0xC0000005` · والمرجعُ RUN033 بـINTEGER
        //
        //      وشكلٌ ثانٍ لا صنفَ فيه أصلًا (`أي ك = 5` ثمّ `ك.قيمة`) كان يُخرِجُ
        //      **خطأَ مترجِمٍ داخليًّا** يطلُبُ من المستخدِمِ أن يُبلِّغَ عن علّةِ مترجِم —
        //      وهي شكوى في محلِّها لولا أنّ الكودَ سليمٌ والعلّةَ علّتُه هو.
        //
        //      ⚠️ **ولم يكن هذا الحارسُ ممكنًا قبلَ اليوم.** التعليقُ الملاصقُ في
        //      `raiseIfObjectReceiverIsNull` كان ينهى صراحةً عن فحصِ الوسمِ هنا لأنّ
        //      `Pointer ⇒ DynKind::Str` كان يَسِمُ الكائنَ نصًّا: حارسٌ يُبنى عليه
        //      يرفعُ على `أي ك = شخص()` ثمّ `ك.اسم` — أي يُخفِقُ على المُستقبِلِ
        //      السليم. فصدقُ الوسمِ شرطُ وجودِ الحارسِ لا تحسينٌ يسبقُه.
        //
        //      🔑 والاسمُ يُملأُ من `Value` عينِها التي يملأُ منها المفسّرُ، ذراعًا ذراعًا:
        //      رفعٌ باسمٍ مُخترَعٍ يجتازُ «rc=1» ويكذبُ في النصِّ — وهو أخفى من لا رفع.
        //
        //      ⚠️ ولا ذراعَ هنا لـ«عدم»/«فراغ»: لهما بابُهما المقيسُ بشكلَيه
        //      (`VOID` مقابل `NULL`) وبذرتاه ــ وتكرارُهما هنا يُنتِجُ رفعَين
        //      بنصَّين قد ينحرفان عند أوّلِ تحريرٍ للكتالوج.
        //
        //      ⚠️ والافتراضيُ يمرُّ ولا يرفع: وسمٌ لا نملكُ له اسمًا (تعدادٌ جبريٌّ)
        //      يُرفَعُ عليه باسمٍ مُخمَّنٍ كذبٌ، والسكوتُ عنه يُبقي السلوكَ كما كان.
        // (EN) A dynamic receiver that is NOT an object. emitRaiseIfNull asks one
        //      question — is this payload null? — and an integer payload is neither zero
        //      nor the sentinel, so it sails through. This guard was IMPOSSIBLE before
        //      the tag was corrected: the adjacent comment above explicitly forbade a
        //      tag check here because Pointer => DynKind::Str tagged objects as strings,
        //      so the guard would have rejected valid receivers. Names come from the
        //      same Value the interpreter fills from, arm by arm. Null/Void are
        //      deliberately absent (their own measured door handles both shapes), and
        //      the default falls through rather than raise with a guessed name.
        // ═══════════════════════════════════════════════════════════════════════
        void OOPOpsCodeGen::raiseIfDynReceiverIsNotObject(llvm::Value *dynValue, const char *tag,
                                                          const char *operation)
        {
            if (!dynValue || !cg_.builder_ || !cg_.builder_->GetInsertBlock())
                return;
            if (!Sad::LLVM::isSadDyn(dynValue))
                return;

            llvm::Value *kindByte = Sad::LLVM::dynKindByte(cg_, dynValue);
            llvm::IntegerType *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            const std::string stem = std::string(tag) + ".dynkind";

            llvm::BasicBlock *contBB =
                llvm::BasicBlock::Create(*cg_.context_, stem + ".object", curFunc);

            // (AR) الوسمُ ⇒ اسمُ النوعِ كما يكتبُه المفسّرُ — من `Value` لا من نصٍّ مكتوب.
            // (EN) Tag => the interpreter's own type name, from `Value`, never a literal.
            const std::vector<std::pair<uint8_t, std::string>> nonObjectKinds = {
                {Sad::LLVM::DynKind::Int, Sad::Data::Value(static_cast<int64_t>(0)).getTypeName()},
                {Sad::LLVM::DynKind::Float, Sad::Data::Value(0.0).getTypeName()},
                {Sad::LLVM::DynKind::Str, Sad::Data::Value(std::string()).getTypeName()},
                {Sad::LLVM::DynKind::Bool, Sad::Data::Value(false).getTypeName()},
                {Sad::LLVM::DynKind::Array,
                 Sad::Data::Value(Sad::Data::Value::ArrayType()).getTypeName()},
                {Sad::LLVM::DynKind::Map,
                 Sad::Data::Value(Sad::Data::Value::MapType()).getTypeName()},
            };

            llvm::SwitchInst *sw = cg_.builder_->CreateSwitch(
                kindByte, contBB, static_cast<unsigned>(nonObjectKinds.size()));

            for (const auto &kindAndName : nonObjectKinds)
            {
                llvm::BasicBlock *raiseBB =
                    llvm::BasicBlock::Create(*cg_.context_, stem + ".raise", curFunc);
                sw->addCase(llvm::ConstantInt::get(i8Ty, kindAndName.first), raiseBB);
                cg_.builder_->SetInsertPoint(raiseBB);
                cg_.emitNullRaiseBody(::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                                      {{"operation", operation}, {"type", kindAndName.second}},
                                      stem);
                cg_.builder_->CreateUnreachable();
            }

            cg_.builder_->SetInsertPoint(contBB);
        }

        // ════════════════════════════════════════════════════════════════════════
        // (AR) 🔑 منفذُ **نداءِ الطريقةِ** — ثالثُ منافذِ العائلةِ وآخرُها.
        //
        //      ولا يختلف عن أختَيه إلّا في موضعِ الإصدار: القراءةُ والكتابةُ
        //      تُصدَران في الخلفيّةِ عند `OBJECT_GET`/`OBJECT_SET`، وهذا يُصدَر في
        //      **الواجهةِ** عند موضعِ النداء — لأنّ دامجَ الدوالِّ يمحو النداءَ قبل
        //      أن تراه الخلفيّة. والتعليلُ المقيسُ عند تعريفِ الأوپكودِ في
        //      `sir_types.h`.
        //
        //      واللفظُ من التعليمةِ لا من هذا الباب: المرجعُ يقول `.اسم()` لكلِّ
        //      طريقةٍ باسمها، فلا يصحّ ثابتٌ واحدٌ لكلِّ النداءات — كما لم يصحّ
        //      لفظٌ واحدٌ للقراءةِ والكتابة.
        // (EN) The METHOD-CALL port: the family's third and last. It differs from
        //      its siblings only in where it is emitted — read/write are emitted in
        //      the backend at OBJECT_GET/OBJECT_SET, this one in the FRONTEND at the
        //      call site, because the inliner erases the call before the backend sees
        //      it. The label comes from the instruction, not from this door: the
        //      reference says «.name()» per method, so no single constant can serve
        //      every call, exactly as no single label served read and write.
        // ════════════════════════════════════════════════════════════════════════
        llvm::Value *OOPOpsCodeGen::emitObjectNullCheck(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS,
                                {{"detail", "OBJECT_NULL_CHECK"}});
                return nullptr;
            }

            const std::string &recvName = inst->operands[0].name;
            const std::string &operation = inst->operands[1].name;

            // (AR) 🔑 قيمةُ «عولِجت» — لا `nullptr`: الموزّعُ يقرأ `nullptr` علامةَ
            //      «لم تُعالَج ⇒ جرّب الطبقةَ التالية» فيسقط عبرَ الطبقاتِ كلِّها
            //      ويبلّغ «Unsupported opcode:70» زائفًا فوقَ حارسٍ أُصدر بنجاح —
            //      على **كلِّ** نداءِ طريقةٍ مستقبِلُه عامٌّ (قِيس: `ح.ضاعف(3)`
            //      يطبع 6 صحيحًا والبلاغُ الداخليُّ يعلوه). نمطُ اطبع/#185 عينُه.
            // (EN) A "handled" sentinel — never nullptr: the dispatcher reads
            //      nullptr as "unhandled ⇒ try the next tier", falls through every
            //      tier, and spuriously reports "Unsupported opcode:70" on top of a
            //      successfully emitted guard — on EVERY method call with a global
            //      receiver (measured). The print/#185 pattern.
            llvm::Value *handledSentinel =
                llvm::ConstantInt::get(cg_.getInt64Type(), 0);

            auto it = cg_.context_info_.namedValues.find(recvName);
            if (it == cg_.context_info_.namedValues.end() || !it->second)
            {
                // (AR) مُستقبِلٌ لا خانةَ له في هذا النطاق: لا حارسَ يُصدَر ولا خطأَ
                //      يُبلَّغ — الحارسُ إضافةٌ إلى مسارٍ قائمٍ لا شرطٌ لصحّتِه، ورفعُ
                //      تشخيصٍ داخليٍّ هنا يُحوّل برنامجًا يعمل إلى إخفاقِ ترجمة.
                // (EN) A receiver with no slot in this scope: emit no guard and report
                //      nothing. The guard is an addition to a working path, not a
                //      precondition of it; an internal diagnostic here would turn a
                //      working program into a compile failure.
                return handledSentinel;
            }

            llvm::Value *objPtr = Sad::LLVM::loadDynSlot(cg_, it->second);
            raiseIfObjectReceiverIsNull(objPtr, "obj.callchk", operation.c_str());
            return handledSentinel;
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

            // (AR) خانةٌ نوعُها %SadDyn: `namedValues` تُعيد **الخانةَ** لا القيمة، فيرى
            //      `isSadDyn` أدناه `ptr` فيحكم «ليست موسومة»، وتنزلق الخانةُ إلى مسارِ
            //      «مؤشّرٌ جاهز» فيُعامَل **عنوانُ الخانةِ** مؤشّرَ كائن — وتُقرأ بايتاتُ
            //      الوسمِ والحمولةِ حقولًا. والمقيس: `أي ك = عداد()` ثمّ `ك.قيمة` يطبع
            //      حمولةَ المؤشّرِ عددًا (rc=0) بينما يطبع المرجعُ 5.
            //      ويُنادى البابُ **قبل** اشتقاقِ `normalizedObjPtr` و`actualObj` معًا،
            //      لأنّ الاشتقاقَين توأمان ومسارُ الخاصّيّة (`__get_*`) يمرّ بالأوّلِ وحدَه.
            // (EN) A slot whose allocated type is %SadDyn: `namedValues` hands back the
            //      SLOT, not the value, so the `isSadDyn` test below sees a `ptr`, decides
            //      "not tagged", and the slot falls through to the "already a pointer" path
            //      where its ADDRESS is used as an object pointer — the tag and payload
            //      bytes are then read as fields. Called BEFORE both `normalizedObjPtr` and
            //      `actualObj` are derived, since they are twins and the property-getter
            //      path (`__get_*`) goes through the first one only.
            objPtr = Sad::LLVM::loadDynSlot(cg_, objPtr);

            // ════════════════════════════════════════════════════════════════
            // (AR) 🔑 «أهُو مستقبِلٌ ديناميّ؟» — يُسأَلُ **هنا** لا أدنى.
            //      الحرّاسُ أدناه تُسقِطُ الحمولةَ فتمحو الوسمَ، فما بعدَها لا يملكُ
            //      تمييزَ خانةٍ ديناميّةٍ من كائنٍ ساكن. واللقطةُ رخيصةٌ والسؤالُ
            //      المتأخّرُ مستحيل.
            // (EN) Whether the receiver is dynamic is captured HERE: the guards below
            //      project the payload and erase the tag, after which nothing can tell
            //      a dynamic slot from a static object.
            // ════════════════════════════════════════════════════════════════
            const bool receiverIsDynamic = Sad::LLVM::isSadDyn(objPtr);

            raiseIfObjectReceiverIsNull(objPtr, "obj.get", "member access");


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
                // (AR) ⚠️ ومِحكُ الحقلِ هذا لا يليقُ بمُستقبِلٍ ديناميٍّ — ومع ذلك يبقى (ISSUE-142)
                //
                //      المِحكُ يسألُ أيوجدُ الحقلُ في تخطيطِ صنفٍ ما، وهو مِحكٌ لخانةٍ لها
                //      صنفٌ ساكنٌ ضاعَ ربطُه؛ وخانةُ «أي» لا صنفَ لها **بالبناء**. فإجراؤه
                //      عليها يجعلُ جوابَ المُصرِّفِ تابعًا لـ**اسمِ الحقل**: `ك.قيمة` يُجهِضُ
                //      الترجمةَ لأنّ للصنفِ المضمَّنِ «حدث» حقلًا بذلك الاسم، و`ك.طول` يُبنى
                //      لأنّ لا صنفَ يعرِفُه — والبرنامجانِ واحدٌ في المعنى.
                //
                //      🔑 **وقِيسَ استثناءُ الديناميِّ منه فرُدّ.** إسقاطُ المِحكِ عن الديناميِّ
                //      أخرجَ خمسةَ أشكالٍ (`أي ك = عدد/نصّ/عشريّ/منطقيّ/لاشيء` ثمّ `ك.حقل`)
                //      من إجهاضِ الترجمةِ إلى RUN033 مطابِقٍ للمرجعِ رمزًا ونصًّا — وأخرجَ معها
                //      المُستقبِلَ التعداديَّ (`أي ك = خيار.بعض(5)` ثمّ `ك.قيمة`) من إجهاضٍ
                //      إلى **`0` بـrc=0** والمرجعُ يرفعُ RUN005. فالإسقاطُ يشتري خمسةً
                //      بتحويلِ إخفاقِ بناءٍ إلى **كذبٍ صامت**، وهو ثمنٌ لا يُدفع.
                //
                //      وسببُه مُقاسٌ لا مُفترَض: قيمةُ التعدادِ الجبريِّ تحملُ وسمَ `Obj`
                //      لا `Adt` (مقيسٌ: `نوع()` عليها يُجيبُ «كائن» والمرجعُ «خريطة»)،
                //      فتمرُّ من حارسِ الوسمِ أعلاه دونَ أن يرفعَ. ⇒ تصحيحُ وسمِ التعدادِ
                //      شرطٌ لإسقاطِ المِحكِ هنا — كما كان صدقُ وسمِ الكائنِ شرطًا للحارسِ نفسِه.
                // (EN) This field test does not fit a dynamic receiver, yet it stays. Measured:
                //      dropping it for dynamic receivers moves five shapes from an aborting
                //      internal error to a RUN033 matching the reference in code and text — and
                //      moves the ADT receiver from an abort to `0` with rc=0 where the reference
                //      raises RUN005. Buying five shapes with one SILENT LIE is not a trade to
                //      make. The cause is measured: an ADT value carries the `Obj` tag, not
                //      `Adt`, so it slips past the tag guard above. Fixing the ADT tag is a
                //      precondition for dropping this test — exactly as a truthful object tag
                //      was a precondition for the guard itself.
                // ============================================================
                // (AR) 🔑 الحارسُ يبقى، والمتغيّرُ **ما يُقالُ للمستعمِل**.
                //
                //      كان الفرعانِ يصرفانِ إلى INT005/INT002 — «خطأ مترجم
                //      داخلي … يُرجى الإبلاغ» — فيُطلَبُ من كاتبِ البرنامجِ بلاغُ
                //      علّةِ مترجِم. وقِيسَ (٢٠٢٦-٠٩-٠٣) أنّ أبسطَ ما يُطلقُهما لا
                //      علّةَ فيه: «متغير س = "أبج"» ثمّ «س.الطول» — و«الطول»
                //      ليست عضوًا معلَنًا في مصدرِ الحقيقةِ أصلًا (المعلَنُ
                //      دالّةٌ: «طول(س)»). فالمترجّمُ لم ينهَرْ، بل عجزَ عن ربطِ
                //      سجلِّ المستقبِلِ بصنف.
                //
                //      و`fieldExistsInAnyClass` تبقى مُستعمَلةً وحكمُها يُصان:
                //      هي التي تفصلُ حالَ «عضوٌ حقيقيٌّ ومستقبِلٌ ديناميّ»
                //      (SEM050 — قيدٌ قائمٌ في المترجّم) من حالِ «اسمٌ لا يحملُه
                //      صنفٌ معرَّف» (SEM051 — خطأٌ في البرنامج). ورمزانِ لا
                //      خانةُ نصٍّ، فلا سلسلةَ نصّيّةً تدخلُ شفرةَ المترجّم.
                // (EN) 🔑 The guard stays; what changes is WHAT THE USER IS
                //      TOLD. Both arms reported an INTERNAL error asking the author
                //      to file a compiler bug; measured, the simplest triggering
                //      program has no compiler defect. fieldExistsInAnyClass keeps
                //      its role, now separating a standing limitation (SEM050) from
                //      a defect in the program (SEM051) — two catalog codes rather
                //      than a text slot, so no string literal enters the source.
                // ============================================================
                // (AR) 🔑 رمزٌ واحدٌ لا فرعان: `fieldExistsInAnyClass` تقرأُ
                //      **جدولَ حقولِ الأصنافِ وحدَه**، فلا ترى الطرائقَ المعلَنةَ على
                //      الأنواعِ ولا حمولاتِ التعداداتِ الجبريّة. وقِيسَ (مراجعةٌ
                //      خصميّة، ٢٠٢٦-٠٩-٠٣) أنّ فرعَها السالبَ اتّهمَ برامجَ صحيحة:
                //      `م.الطول` و«الطول» طريقةٌ **مستقرّةٌ معلَنة**، و`ص.نق` و«نق»
                //      حمولةُ تعدادٍ مُصرَّحةٌ في السطرِ نفسِه. فحُذف ذلك الرمزُ ولم
                //      يبقَ إلّا ما يُقاسُ فعلًا: تعذّرَ تحديدُ الصنف.
                // (EN) 🔑 One code, not two: fieldExistsInAnyClass reads the
                //      CLASS-FIELD table only and cannot see declared type methods or
                //      algebraic-enum payloads. Measured (adversarial review): its
                //      negative arm accused correct programs. Only the measured claim
                //      remains — the class could not be resolved.
                // ════════════════════════════════════════════════════════
                // (AR) 🔑 المستقبِلُ الديناميُّ **شأنُ وقتِ تشغيلٍ لا إجهاضُ ترجمة**.
                //
                //      كان الحكمُ يقعُ هنا على الجميع، فيتوقّفُ على حلقةِ تخمينٍ
                //      أعلاه تُسنِدُ الصنفَ من **اسمِ العضو**: «هل يوجدُ في هذا
                //      الملفِّ أيُّ صنفٍ فيه حقلٌ بهذا الاسم؟». ومقيسُه
                //      (٢٠٢٦-٠٩-٠٣): `متغير ك` ثمّ `ك.القيمة` **يُجهَض**؛ وإضافةُ
                //      صنفٍ أجنبيٍّ لا يُنشَأُ منه كائنٌ ولا يُذكَر، فيه حقلٌ اسمُه
                //      «القيمة»، تجعلُ البرنامجَ **نفسَه** يُترجَمُ ويرفعُ `RUN033`
                //      رفعًا صحيحًا. أي أنّ جوابَ المترجّمِ كان معلَّقًا على جدولِ
                //      أسماءٍ عالميٍّ لا على المستقبِلِ ولا على العضو.
                //
                //      والقيمُ المُترجَمةُ لا تحملُ هويّةَ نوعٍ زمنيّة (منصوصٌ عليه
                //      في `value_repr.yaml`: الكائنُ بنيةٌ خامٌّ بلا ترويسة)، فتعذُّرُ
                //      الحسمِ على خانةٍ ديناميّةٍ هو **الحالُ الطبيعيّةُ** لا عطبٌ.
                //      ولمّا لم يبلغْ هذا الموضعَ إلّا ما لا يحملُ **أيُّ** صنفٍ
                //      معرَّفٍ عضوًا باسمِه، فالوصولُ لا يمكنُ أن ينجحَ لأيِّ قيمة
                //      ⇒ رفعٌ زمنيٌّ صريحٌ بـ`RUN075`، لا إجهاضٌ ولا قراءةُ بايتاتٍ
                //      بتخطيطٍ مُخمَّن.
                //
                //      ⚠️ ولا تُحذَفُ حلقةُ التخمينِ أعلاه في هذه الرقعة: هي اليومَ
                //      **الطريقُ الوحيدُ** إلى تخطيطِ كائنٍ في خانةِ «أي» (مقيسٌ:
                //      `أي ك = عداد()` ثمّ `ك.قيمة` يطبعُ ٥ بها). وحذفُها يلزمُه
                //      كلمةُ هويّةِ نوعٍ في ترويسةِ القيمة — تغييرُ عقدٍ ثنائيٍّ
                //      يمسُّ `@تمثيل_سي`، مشروعٌ مستقلٌّ لا يُدَسُّ في رقعةِ عضو.
                //      والذي زال بهذه الرقعةِ أن يكونَ **الحكمُ** تابعًا لها.
                // (EN) 🔑 A dynamic receiver is a RUN-TIME matter, not an aborted
                //      compilation. The verdict used to hang on a guess loop above that
                //      infers the class from the MEMBER NAME; measured, adding an unused,
                //      unrelated class with a field of that name flips the same program
                //      from abort to a correct RUN033 raise. Compiled values carry no
                //      runtime type identity, so failing to resolve a dynamic receiver is
                //      normal, not a defect. Since only members no declared class carries
                //      reach here, the access can never succeed — hence an explicit
                //      RUN075 raise rather than an abort or a guessed-layout read. The
                //      guess loop stays: it is currently the only path to an object held
                //      in an «أي» slot. What changed is that the VERDICT no longer
                //      depends on it.
                // ════════════════════════════════════════════════════════
                if (receiverIsDynamic && cg_.builder_ && cg_.builder_->GetInsertBlock())
                {
                    llvm::Function *untypedFunc =
                        cg_.builder_->GetInsertBlock()->getParent();
                    llvm::BasicBlock *untypedRaiseBB = llvm::BasicBlock::Create(
                        *cg_.context_, "obj.member.untyped.raise", untypedFunc);
                    llvm::BasicBlock *untypedContBB = llvm::BasicBlock::Create(
                        *cg_.context_, "obj.member.untyped.cont", untypedFunc);
                    cg_.builder_->CreateBr(untypedRaiseBB);
                    cg_.builder_->SetInsertPoint(untypedRaiseBB);
                    cg_.emitNullRaiseBody(
                        ::Sad::Errors::ErrorCode::RUN_MEMBER_ON_UNTYPED_RECEIVER,
                        {{"member", fieldName}}, "obj.member.untyped");
                    cg_.builder_->CreateUnreachable();
                    // (AR) كتلةٌ تاليةٌ بلا سابقٍ — يبقى الخفضُ سليمَ البنيةِ فلا
                    //      تُلحَقُ تعليماتٌ بكتلةٍ منتهيةٍ بـ`unreachable`.
                    // (EN) A successor with no predecessors keeps lowering well-formed.
                    cg_.builder_->SetInsertPoint(untypedContBB);
                    return cg_.builtinErrorSentinel(inst);
                }

                cg_.reportError(::Sad::Errors::ErrorCode::SEM_MEMBER_RECEIVER_CLASS_UNKNOWN,
                                {{"receiver", stripRegisterSigil(objRegName)},
                                 {"member", fieldName}});
                // (AR) 🔑 خانةُ النتيجةِ تُملأُ حتّى عندَ الرفض — وإلّا تلا
                //      التشخيصَ الصحيحَ «مرجع غير معرَّف … يُرجى الإبلاغ»، فبَطَلَ
                //      نصفُ فائدتِه. مقيسٌ (٢٠٢٦-٠٩-٠٣): «س.الطول» أخرجَ SEM051
                //      صحيحًا ثمّ انهيارًا داخليًّا على `%1` من بعدِه.
                //      وليست تسترًا: البوّابةُ العامّةُ في `compiler_driver_backend`
                //      تُفشلُ الترجمةَ حتمًا بعدَ أيِّ خطأ فلا ثنائيَّ يُسلَّم؛ غايتُها
                //      بقاءُ الخفضِ سليمَ البنيةِ حتّى تُجمَعَ بقيّةُ الأخطاءِ الحقيقيّة.
                // (EN) 🔑 Bind the result slot even when rejecting; otherwise a
                //      correct diagnostic is followed by "undefined register — please
                //      report", undoing half its value. Not suppression: the general
                //      gate still fails the compilation, so no binary is emitted; this
                //      only keeps lowering well-formed so the rest of the real errors
                //      are collected.
                // (AR) 🔑 `builtinErrorSentinel` مُعينٌ **قائمٌ في الشجرة**
                //      يفعلُ الأمرَين معًا: يربطُ خانةَ النتيجةِ فلا يتتالى «سجلّ غير
                //      معرَّف»، ويُعيدُ إشارةً غيرَ صفريّةٍ فيتوقّفُ الموزّعُ بلا بلاغِ
                //      «أوپكود غير مدعوم» زائف. وكانت هنا نسخةٌ يدويّةٌ منه — نسخةٌ
                //      ثانيةٌ لمنطقٍ واحدٍ تتباعدُ عن أصلِها.
                // (EN) 🔑 builtinErrorSentinel already exists in the tree and does
                //      both jobs: binds the result slot so no "undefined register"
                //      cascades, and returns a non-null sentinel so the dispatcher stops
                //      without a spurious "unsupported opcode". This was a hand-rolled
                //      second copy of it.
                return cg_.builtinErrorSentinel(inst);
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
            // (AR) الكائنُ نفسُه قد يَرِد موسومًا (%SadDyn) حين يكون حقلًا من نوع «أي»
            //      يحمل مرجعَ كائن — كما في `ش.عنوان.مدينة`. الوصولُ إلى حقلٍ يلزمه
            //      مؤشّر، وتمريرُ الهيكلِ الموسومِ كما هو كان يُجهض المترجمَ بتأكيدِ
            //      LLVM «Ptr must have pointer type» — إخفاقًا داخليًّا لا تشخيصًا.
            // (EN) The object itself can arrive tagged (%SadDyn) when it is an `أي`
            //      field holding an object reference (`p.address.city`). Field access
            //      needs a pointer; passing the tagged struct through used to abort the
            //      compiler on «Ptr must have pointer type» instead of diagnosing.
            if (isSadDyn(actualObj))
                actualObj = unpackPtr(cg_, actualObj);
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

            // (AR) خانةٌ نوعُها %SadDyn: `namedValues` تُعيد **الخانةَ** لا القيمة، فيرى
            //      `isSadDyn` أدناه `ptr` فيحكم «ليست موسومة»، وتنزلق الخانةُ إلى مسارِ
            //      «مؤشّرٌ جاهز» فيُعامَل **عنوانُ الخانةِ** مؤشّرَ كائن — وتُقرأ بايتاتُ
            //      الوسمِ والحمولةِ حقولًا. والمقيس: `أي ك = عداد()` ثمّ `ك.قيمة` يطبع
            //      حمولةَ المؤشّرِ عددًا (rc=0) بينما يطبع المرجعُ 5.
            //      ويُنادى البابُ **قبل** اشتقاقِ `normalizedObjPtr` و`actualObj` معًا،
            //      لأنّ الاشتقاقَين توأمان ومسارُ الخاصّيّة (`__get_*`) يمرّ بالأوّلِ وحدَه.
            // (EN) A slot whose allocated type is %SadDyn: `namedValues` hands back the
            //      SLOT, not the value, so the `isSadDyn` test below sees a `ptr`, decides
            //      "not tagged", and the slot falls through to the "already a pointer" path
            //      where its ADDRESS is used as an object pointer — the tag and payload
            //      bytes are then read as fields. Called BEFORE both `normalizedObjPtr` and
            //      `actualObj` are derived, since they are twins and the property-getter
            //      path (`__get_*`) goes through the first one only.
            objPtr = Sad::LLVM::loadDynSlot(cg_, objPtr);

            // ════════════════════════════════════════════════════════════════
            // (AR) 🔑 «أهُو مستقبِلٌ ديناميّ؟» — يُسأَلُ **هنا** لا أدنى.
            //      الحرّاسُ أدناه تُسقِطُ الحمولةَ فتمحو الوسمَ، فما بعدَها لا يملكُ
            //      تمييزَ خانةٍ ديناميّةٍ من كائنٍ ساكن. واللقطةُ رخيصةٌ والسؤالُ
            //      المتأخّرُ مستحيل.
            // (EN) Whether the receiver is dynamic is captured HERE: the guards below
            //      project the payload and erase the tag, after which nothing can tell
            //      a dynamic slot from a static object.
            // ════════════════════════════════════════════════════════════════
            const bool receiverIsDynamic = Sad::LLVM::isSadDyn(objPtr);

            raiseIfObjectReceiverIsNull(objPtr, "obj.set", ".=");


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
                // (AR) ⚠️ ومِحكُ الحقلِ هذا لا يليقُ بمُستقبِلٍ ديناميٍّ — ومع ذلك يبقى (ISSUE-142)
                //
                //      المِحكُ يسألُ أيوجدُ الحقلُ في تخطيطِ صنفٍ ما، وهو مِحكٌ لخانةٍ لها
                //      صنفٌ ساكنٌ ضاعَ ربطُه؛ وخانةُ «أي» لا صنفَ لها **بالبناء**. فإجراؤه
                //      عليها يجعلُ جوابَ المُصرِّفِ تابعًا لـ**اسمِ الحقل**: `ك.قيمة` يُجهِضُ
                //      الترجمةَ لأنّ للصنفِ المضمَّنِ «حدث» حقلًا بذلك الاسم، و`ك.طول` يُبنى
                //      لأنّ لا صنفَ يعرِفُه — والبرنامجانِ واحدٌ في المعنى.
                //
                //      🔑 **وقِيسَ استثناءُ الديناميِّ منه فرُدّ.** إسقاطُ المِحكِ عن الديناميِّ
                //      أخرجَ خمسةَ أشكالٍ (`أي ك = عدد/نصّ/عشريّ/منطقيّ/لاشيء` ثمّ `ك.حقل`)
                //      من إجهاضِ الترجمةِ إلى RUN033 مطابِقٍ للمرجعِ رمزًا ونصًّا — وأخرجَ معها
                //      المُستقبِلَ التعداديَّ (`أي ك = خيار.بعض(5)` ثمّ `ك.قيمة`) من إجهاضٍ
                //      إلى **`0` بـrc=0** والمرجعُ يرفعُ RUN005. فالإسقاطُ يشتري خمسةً
                //      بتحويلِ إخفاقِ بناءٍ إلى **كذبٍ صامت**، وهو ثمنٌ لا يُدفع.
                //
                //      وسببُه مُقاسٌ لا مُفترَض: قيمةُ التعدادِ الجبريِّ تحملُ وسمَ `Obj`
                //      لا `Adt` (مقيسٌ: `نوع()` عليها يُجيبُ «كائن» والمرجعُ «خريطة»)،
                //      فتمرُّ من حارسِ الوسمِ أعلاه دونَ أن يرفعَ. ⇒ تصحيحُ وسمِ التعدادِ
                //      شرطٌ لإسقاطِ المِحكِ هنا — كما كان صدقُ وسمِ الكائنِ شرطًا للحارسِ نفسِه.
                // (EN) This field test does not fit a dynamic receiver, yet it stays. Measured:
                //      dropping it for dynamic receivers moves five shapes from an aborting
                //      internal error to a RUN033 matching the reference in code and text — and
                //      moves the ADT receiver from an abort to `0` with rc=0 where the reference
                //      raises RUN005. Buying five shapes with one SILENT LIE is not a trade to
                //      make. The cause is measured: an ADT value carries the `Obj` tag, not
                //      `Adt`, so it slips past the tag guard above. Fixing the ADT tag is a
                //      precondition for dropping this test — exactly as a truthful object tag
                //      was a precondition for the guard itself.
                // ============================================================
                // (AR) 🔑 الحارسُ يبقى، والمتغيّرُ **ما يُقالُ للمستعمِل**.
                //
                //      كان الفرعانِ يصرفانِ إلى INT005/INT002 — «خطأ مترجم
                //      داخلي … يُرجى الإبلاغ» — فيُطلَبُ من كاتبِ البرنامجِ بلاغُ
                //      علّةِ مترجِم. وقِيسَ (٢٠٢٦-٠٩-٠٣) أنّ أبسطَ ما يُطلقُهما لا
                //      علّةَ فيه: «متغير س = "أبج"» ثمّ «س.الطول» — و«الطول»
                //      ليست عضوًا معلَنًا في مصدرِ الحقيقةِ أصلًا (المعلَنُ
                //      دالّةٌ: «طول(س)»). فالمترجّمُ لم ينهَرْ، بل عجزَ عن ربطِ
                //      سجلِّ المستقبِلِ بصنف.
                //
                //      و`fieldExistsInAnyClass` تبقى مُستعمَلةً وحكمُها يُصان:
                //      هي التي تفصلُ حالَ «عضوٌ حقيقيٌّ ومستقبِلٌ ديناميّ»
                //      (SEM050 — قيدٌ قائمٌ في المترجّم) من حالِ «اسمٌ لا يحملُه
                //      صنفٌ معرَّف» (SEM051 — خطأٌ في البرنامج). ورمزانِ لا
                //      خانةُ نصٍّ، فلا سلسلةَ نصّيّةً تدخلُ شفرةَ المترجّم.
                // (EN) 🔑 The guard stays; what changes is WHAT THE USER IS
                //      TOLD. Both arms reported an INTERNAL error asking the author
                //      to file a compiler bug; measured, the simplest triggering
                //      program has no compiler defect. fieldExistsInAnyClass keeps
                //      its role, now separating a standing limitation (SEM050) from
                //      a defect in the program (SEM051) — two catalog codes rather
                //      than a text slot, so no string literal enters the source.
                // ============================================================
                // (AR) 🔑 رمزٌ واحدٌ لا فرعان: `fieldExistsInAnyClass` تقرأُ
                //      **جدولَ حقولِ الأصنافِ وحدَه**، فلا ترى الطرائقَ المعلَنةَ على
                //      الأنواعِ ولا حمولاتِ التعداداتِ الجبريّة. وقِيسَ (مراجعةٌ
                //      خصميّة، ٢٠٢٦-٠٩-٠٣) أنّ فرعَها السالبَ اتّهمَ برامجَ صحيحة:
                //      `م.الطول` و«الطول» طريقةٌ **مستقرّةٌ معلَنة**، و`ص.نق` و«نق»
                //      حمولةُ تعدادٍ مُصرَّحةٌ في السطرِ نفسِه. فحُذف ذلك الرمزُ ولم
                //      يبقَ إلّا ما يُقاسُ فعلًا: تعذّرَ تحديدُ الصنف.
                // (EN) 🔑 One code, not two: fieldExistsInAnyClass reads the
                //      CLASS-FIELD table only and cannot see declared type methods or
                //      algebraic-enum payloads. Measured (adversarial review): its
                //      negative arm accused correct programs. Only the measured claim
                //      remains — the class could not be resolved.
                // ════════════════════════════════════════════════════════
                // (AR) 🔑 المستقبِلُ الديناميُّ **شأنُ وقتِ تشغيلٍ لا إجهاضُ ترجمة**.
                //
                //      كان الحكمُ يقعُ هنا على الجميع، فيتوقّفُ على حلقةِ تخمينٍ
                //      أعلاه تُسنِدُ الصنفَ من **اسمِ العضو**: «هل يوجدُ في هذا
                //      الملفِّ أيُّ صنفٍ فيه حقلٌ بهذا الاسم؟». ومقيسُه
                //      (٢٠٢٦-٠٩-٠٣): `متغير ك` ثمّ `ك.القيمة` **يُجهَض**؛ وإضافةُ
                //      صنفٍ أجنبيٍّ لا يُنشَأُ منه كائنٌ ولا يُذكَر، فيه حقلٌ اسمُه
                //      «القيمة»، تجعلُ البرنامجَ **نفسَه** يُترجَمُ ويرفعُ `RUN033`
                //      رفعًا صحيحًا. أي أنّ جوابَ المترجّمِ كان معلَّقًا على جدولِ
                //      أسماءٍ عالميٍّ لا على المستقبِلِ ولا على العضو.
                //
                //      والقيمُ المُترجَمةُ لا تحملُ هويّةَ نوعٍ زمنيّة (منصوصٌ عليه
                //      في `value_repr.yaml`: الكائنُ بنيةٌ خامٌّ بلا ترويسة)، فتعذُّرُ
                //      الحسمِ على خانةٍ ديناميّةٍ هو **الحالُ الطبيعيّةُ** لا عطبٌ.
                //      ولمّا لم يبلغْ هذا الموضعَ إلّا ما لا يحملُ **أيُّ** صنفٍ
                //      معرَّفٍ عضوًا باسمِه، فالوصولُ لا يمكنُ أن ينجحَ لأيِّ قيمة
                //      ⇒ رفعٌ زمنيٌّ صريحٌ بـ`RUN075`، لا إجهاضٌ ولا قراءةُ بايتاتٍ
                //      بتخطيطٍ مُخمَّن.
                //
                //      ⚠️ ولا تُحذَفُ حلقةُ التخمينِ أعلاه في هذه الرقعة: هي اليومَ
                //      **الطريقُ الوحيدُ** إلى تخطيطِ كائنٍ في خانةِ «أي» (مقيسٌ:
                //      `أي ك = عداد()` ثمّ `ك.قيمة` يطبعُ ٥ بها). وحذفُها يلزمُه
                //      كلمةُ هويّةِ نوعٍ في ترويسةِ القيمة — تغييرُ عقدٍ ثنائيٍّ
                //      يمسُّ `@تمثيل_سي`، مشروعٌ مستقلٌّ لا يُدَسُّ في رقعةِ عضو.
                //      والذي زال بهذه الرقعةِ أن يكونَ **الحكمُ** تابعًا لها.
                // (EN) 🔑 A dynamic receiver is a RUN-TIME matter, not an aborted
                //      compilation. The verdict used to hang on a guess loop above that
                //      infers the class from the MEMBER NAME; measured, adding an unused,
                //      unrelated class with a field of that name flips the same program
                //      from abort to a correct RUN033 raise. Compiled values carry no
                //      runtime type identity, so failing to resolve a dynamic receiver is
                //      normal, not a defect. Since only members no declared class carries
                //      reach here, the access can never succeed — hence an explicit
                //      RUN075 raise rather than an abort or a guessed-layout read. The
                //      guess loop stays: it is currently the only path to an object held
                //      in an «أي» slot. What changed is that the VERDICT no longer
                //      depends on it.
                // ════════════════════════════════════════════════════════
                if (receiverIsDynamic && cg_.builder_ && cg_.builder_->GetInsertBlock())
                {
                    llvm::Function *untypedFunc =
                        cg_.builder_->GetInsertBlock()->getParent();
                    llvm::BasicBlock *untypedRaiseBB = llvm::BasicBlock::Create(
                        *cg_.context_, "obj.member.untyped.raise", untypedFunc);
                    llvm::BasicBlock *untypedContBB = llvm::BasicBlock::Create(
                        *cg_.context_, "obj.member.untyped.cont", untypedFunc);
                    cg_.builder_->CreateBr(untypedRaiseBB);
                    cg_.builder_->SetInsertPoint(untypedRaiseBB);
                    cg_.emitNullRaiseBody(
                        ::Sad::Errors::ErrorCode::RUN_MEMBER_ON_UNTYPED_RECEIVER,
                        {{"member", fieldName}}, "obj.member.untyped");
                    cg_.builder_->CreateUnreachable();
                    // (AR) كتلةٌ تاليةٌ بلا سابقٍ — يبقى الخفضُ سليمَ البنيةِ فلا
                    //      تُلحَقُ تعليماتٌ بكتلةٍ منتهيةٍ بـ`unreachable`.
                    // (EN) A successor with no predecessors keeps lowering well-formed.
                    cg_.builder_->SetInsertPoint(untypedContBB);
                    return cg_.builtinErrorSentinel(inst);
                }

                cg_.reportError(::Sad::Errors::ErrorCode::SEM_MEMBER_RECEIVER_CLASS_UNKNOWN,
                                {{"receiver", stripRegisterSigil(objRegName)},
                                 {"member", fieldName}});
                // (AR) 🔑 خانةُ النتيجةِ تُملأُ حتّى عندَ الرفض — وإلّا تلا
                //      التشخيصَ الصحيحَ «مرجع غير معرَّف … يُرجى الإبلاغ»، فبَطَلَ
                //      نصفُ فائدتِه. مقيسٌ (٢٠٢٦-٠٩-٠٣): «س.الطول» أخرجَ SEM051
                //      صحيحًا ثمّ انهيارًا داخليًّا على `%1` من بعدِه.
                //      وليست تسترًا: البوّابةُ العامّةُ في `compiler_driver_backend`
                //      تُفشلُ الترجمةَ حتمًا بعدَ أيِّ خطأ فلا ثنائيَّ يُسلَّم؛ غايتُها
                //      بقاءُ الخفضِ سليمَ البنيةِ حتّى تُجمَعَ بقيّةُ الأخطاءِ الحقيقيّة.
                // (EN) 🔑 Bind the result slot even when rejecting; otherwise a
                //      correct diagnostic is followed by "undefined register — please
                //      report", undoing half its value. Not suppression: the general
                //      gate still fails the compilation, so no binary is emitted; this
                //      only keeps lowering well-formed so the rest of the real errors
                //      are collected.
                // (AR) 🔑 `builtinErrorSentinel` مُعينٌ **قائمٌ في الشجرة**
                //      يفعلُ الأمرَين معًا: يربطُ خانةَ النتيجةِ فلا يتتالى «سجلّ غير
                //      معرَّف»، ويُعيدُ إشارةً غيرَ صفريّةٍ فيتوقّفُ الموزّعُ بلا بلاغِ
                //      «أوپكود غير مدعوم» زائف. وكانت هنا نسخةٌ يدويّةٌ منه — نسخةٌ
                //      ثانيةٌ لمنطقٍ واحدٍ تتباعدُ عن أصلِها.
                // (EN) 🔑 builtinErrorSentinel already exists in the tree and does
                //      both jobs: binds the result slot so no "undefined register"
                //      cascades, and returns a non-null sentinel so the dispatcher stops
                //      without a spurious "unsupported opcode". This was a hand-rolled
                //      second copy of it.
                return cg_.builtinErrorSentinel(inst);
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
            // (AR) الكائنُ نفسُه قد يَرِد موسومًا (%SadDyn) حين يكون حقلًا من نوع «أي»
            //      يحمل مرجعَ كائن — كما في `ش.عنوان.مدينة`. الوصولُ إلى حقلٍ يلزمه
            //      مؤشّر، وتمريرُ الهيكلِ الموسومِ كما هو كان يُجهض المترجمَ بتأكيدِ
            //      LLVM «Ptr must have pointer type» — إخفاقًا داخليًّا لا تشخيصًا.
            // (EN) The object itself can arrive tagged (%SadDyn) when it is an `أي`
            //      field holding an object reference (`p.address.city`). Field access
            //      needs a pointer; passing the tagged struct through used to abort the
            //      compiler on «Ptr must have pointer type» instead of diagnosing.
            if (isSadDyn(actualObj))
                actualObj = unpackPtr(cg_, actualObj);
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