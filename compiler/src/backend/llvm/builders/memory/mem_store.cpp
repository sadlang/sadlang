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
#include "builders/memory/memory_codegen.h" // (Phase 7 Step 2)
#include "sad_dyn_repr.h" // (AR) ISSUE-063: ترقية الخانات إلى %SadDyn وتعليب المحسوس / (EN) ISSUE-063: %SadDyn slot promotion + packing
#include "sad_type_utils.h" // (AR) SEM045 (أ٢): kindToArabic لرسالة حارس الفراغ / (EN) SEM045: Arabic type name for the Void guard
#include "llvm_codegen.h"
#include "sad_debug_log.h"

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
        llvm::Value *MemoryCodeGen::emitStore(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Store"}});
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
                return cg_.emitObjectSet(objectSetInst);

                llvm::Value *value = cg_.resolveOperand(inst->operands[0]);
                if (!value)
                {
                    cg_.reportError(::Sad::Errors::ErrorCode::INT_BACKEND_EMIT, {{"detail", std::string("emitStore: cannot resolve value operand:") + inst->operands[0].name}});
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
                SAD_DEBUG_LOG_LINE("[DEBUG] emitStore: member assign " << objName << "." << fieldName);
#endif

                // البحث عن الكائن في namedValues
                auto objIt = cg_.context_info_.namedValues.find(objName);
                if (objIt == cg_.context_info_.namedValues.end())
                {
                    // (AR) Fallback: البحث في المتغيرات العامة LLVM
                    // (EN) Fallback: search in LLVM global variables
                    auto *globalVar = cg_.module_->getNamedGlobal(objName);
                    // (AR) إذا لم نجد، نحاول بإزالة % من المقدمة
                    // (EN) If not found, try without leading %
                    if (!globalVar && !objName.empty() && objName[0] == '%')
                    {
                        std::string cleanName = objName.substr(1);
                        globalVar = cg_.module_->getNamedGlobal(cleanName);
                    }
                    if (globalVar)
                    {
                        // (AR) تحميل القيمة من المتغير العام وتحويلها إلى مؤشر
                        // (EN) Load value from global and convert to pointer
                        llvm::Value *loaded = cg_.builder_->CreateLoad(cg_.getInt64Type(), globalVar, objName + ".glob.load");
                        llvm::Value *ptr = cg_.builder_->CreateIntToPtr(loaded,
                                                                    llvm::PointerType::getUnqual(*cg_.context_), objName + ".glob.ptr");
                        cg_.context_info_.namedValues[objName] = ptr;
                        objIt = cg_.context_info_.namedValues.find(objName);
                    }
                    else
                    {
                        cg_.reportError(::Sad::Errors::ErrorCode::INT_BACKEND_EMIT, {{"detail", std::string("emitStore: object not found:") + objName}});
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
                        llvm::Value *ptrVal = cg_.builder_->CreateLoad(cg_.getInt64Type(), allocaInst, objName + ".ptrval");
                        // تحويل i64 إلى ptr
                        objPtr = cg_.builder_->CreateIntToPtr(ptrVal, llvm::PointerType::get(*cg_.context_, 0), objName + ".objptr");
#ifndef NDEBUG
                        SAD_DEBUG_LOG_LINE("[DEBUG] emitStore: converted i64 to ptr for " << objName);
#endif
                    }
                }
                else if (llvm::dyn_cast<llvm::GlobalVariable>(objPtr))
                {
                    // (AR) متغير عام يحمل مؤشر كائن — يجب تحميل المؤشر أولاً ثم التحويل
                    // (EN) Global variable holding object pointer — must load pointer first then cast
                    llvm::Value *ptrVal = cg_.builder_->CreateLoad(cg_.getInt64Type(), objPtr, objName + ".glob.ptrval");
                    objPtr = cg_.builder_->CreateIntToPtr(ptrVal, llvm::PointerType::get(*cg_.context_, 0), objName + ".glob.objptr");
#ifndef NDEBUG
                    SAD_DEBUG_LOG_LINE("[DEBUG] emitStore: converted global i64 to ptr for " << objName);
#endif
                }

                // البحث عن اسم الصنف في objectClassMap
                std::string className;
                auto classIt = cg_.context_info_.objectClassMap.find(objName);
                if (classIt != cg_.context_info_.objectClassMap.end())
                {
                    className = classIt->second;
                }

                // ================================================================
                // (AR) Fallback 0: البحث بدون % في objectClassMap
                // (EN) Fallback 0: search objectClassMap without leading %
                // ================================================================
                if (className.empty() && !objName.empty() && objName[0] == '%')
                {
                    auto classIt2 = cg_.context_info_.objectClassMap.find(objName.substr(1));
                    if (classIt2 != cg_.context_info_.objectClassMap.end())
                    {
                        className = classIt2->second;
                        cg_.context_info_.objectClassMap[objName] = className;
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
                            for (const auto &[clsName, clsSt] : cg_.context_info_.classStructTypes)
                            {
                                if (clsSt == st)
                                {
                                    className = clsName;
                                    cg_.context_info_.objectClassMap[objName] = className;
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
                                cg_.context_info_.objectClassMap[objName] = className;
#ifndef NDEBUG
                                SAD_DEBUG_LOG_LINE("[DEBUG] emitStore: inferred class '" << className
                                          << "' for object '" << objName << "' by field '" << fieldName << "'");
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
                    return llvm::Constant::getNullValue(cg_.getInt64Type());
                }

                // البحث عن نوع الهيكل
                auto structIt = cg_.context_info_.classStructTypes.find(className);
                if (structIt == cg_.context_info_.classStructTypes.end())
                {
                    cg_.reportError(::Sad::Errors::ErrorCode::INT_BACKEND_EMIT, {{"detail", std::string("emitStore: struct type not found for class:") + className}});
                    return nullptr;
                }
                llvm::StructType *structType = structIt->second;

                // البحث عن ترتيب الحقل
                auto fieldNamesIt = cg_.context_info_.classFieldNames.find(className);
                if (fieldNamesIt == cg_.context_info_.classFieldNames.end())
                {
                    cg_.reportError(::Sad::Errors::ErrorCode::INT_BACKEND_EMIT, {{"detail", std::string("emitStore: field names not found for class:") + className}});
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
                        auto parentIt = cg_.context_info_.classParentMap.find(searchClass);
                        if (parentIt == cg_.context_info_.classParentMap.end() || parentIt->second.empty())
                            break;
                        searchClass = parentIt->second;
                        auto pFieldIt = cg_.context_info_.classFieldNames.find(searchClass);
                        if (pFieldIt != cg_.context_info_.classFieldNames.end())
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
                        cg_.reportError(::Sad::Errors::ErrorCode::INT_BACKEND_EMIT, {{"detail", std::string("emitStore: field '") + fieldName + "' not found in class: " + className}});
                        return nullptr;
                    }
                    // (AR) تحديث نوع الهيكل مع الصنف الجديد
                    structIt = cg_.context_info_.classStructTypes.find(className);
                    if (structIt == cg_.context_info_.classStructTypes.end())
                    {
                        cg_.reportError(::Sad::Errors::ErrorCode::INT_BACKEND_EMIT, {{"detail", std::string("emitStore: struct type not found for class:") + className}});
                        return nullptr;
                    }
                    structType = structIt->second;
                }

                // (AR) إضافة إزاحة vtable — الحقل 0 في الهيكل هو مؤشر vtable
                // (EN) Add vtable offset — field 0 in struct is vtable pointer
                fieldIndex = cg_.getFieldStructIndex(className, fieldIndex);

                // (AR) الكائنُ إلى مؤشّرٍ من بابِ `objectPointerOperand` الواحد —
                //      كان هنا فرعُ المقبضِ i64 وحدَه، فالموسومُ يعبرُ إلى GEP.
                // (EN) Object to pointer through the single objectPointerOperand door —
                //      this was the i64-handle branch alone, so a tagged value reached GEP.
                objPtr = objectPointerOperand(cg_, objPtr, objName + ".i2p");

                // إنشاء GEP للحقل
                llvm::Value *gep = cg_.builder_->CreateStructGEP(structType, objPtr, fieldIndex, fieldName + "_gep");

                // تحويل النوع إذا لزم الأمر
                llvm::Type *fieldType = structType->getElementType(fieldIndex);
                if (value->getType() != fieldType)
                {
                    if (value->getType()->isIntegerTy() && fieldType->isIntegerTy())
                    {
                        value = cg_.builder_->CreateIntCast(value, fieldType, true, "cast");
                    }
                    else if (value->getType()->isPointerTy() && fieldType->isIntegerTy(64))
                    {
                        value = cg_.builder_->CreatePtrToInt(value, fieldType, "ptr2int");
                    }
                    else if (value->getType()->isIntegerTy() && fieldType->isPointerTy())
                    {
                        value = cg_.builder_->CreateIntToPtr(value, fieldType, "int2ptr");
                    }
                }

                auto *storeResult = cg_.builder_->CreateStore(value, gep);
#ifndef NDEBUG
                SAD_DEBUG_LOG_LINE("[DEBUG] emitStore: field '" << fieldName << "' stored via GEP index " << fieldIndex);
#endif
                return storeResult;
            }

            // ================================================================
            // (AR) حالة تخزين عادية: 2 operands
            // (EN) Normal store case: 2 operands
            // ================================================================

            // (AR) احصل على القيمة المراد تخزينها
            // (EN) Get value to store - use cg_.resolveOperand for all types
            llvm::Value *value = cg_.resolveOperand(inst->operands[0]);
            const auto &valueOp = inst->operands[0];

            // (AR) احصل على المؤشر للتخزين فيه
            // (EN) Get pointer to store into
            const std::string &ptrName = inst->operands[1].name;
            llvm::Value *ptr = nullptr;

            // (AR) البحث بـ find() بدلاً من [] لتجنب إدخال nullptr في الخريطة
            // (EN) Use find() instead of [] to avoid inserting nullptr into the map
            auto ptrIt = cg_.context_info_.namedValues.find(ptrName);
            if (ptrIt != cg_.context_info_.namedValues.end())
            {
                ptr = ptrIt->second;
            }

            // (AR) إذا لم نجد المؤشر في namedValues، نبحث في المتغيرات العامة
            // (EN) If pointer not found in namedValues, search global variables
            if (!ptr)
            {
                // (AR) البحث بالاسم الأصلي أولاً
                // (EN) Search with original name first
                llvm::GlobalVariable *gv = cg_.module_->getGlobalVariable(ptrName);
                // (AR) إذا لم نجد، نحاول بإزالة % من المقدمة (السجلات تبدأ بـ % لكن المتغيرات العامة لا)
                // (EN) If not found, try without leading % (registers start with % but globals don't)
                if (!gv && !ptrName.empty() && ptrName[0] == '%')
                {
                    gv = cg_.module_->getGlobalVariable(ptrName.substr(1));
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
                auto git = cg_.context_info_.globalValues.find(cleanName);
                if (git == cg_.context_info_.globalValues.end())
                {
                    git = cg_.context_info_.globalValues.find(ptrName);
                }
                if (git != cg_.context_info_.globalValues.end() && git->second != nullptr)
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
                    llvm::Function *currentFunc = cg_.builder_->GetInsertBlock()->getParent();
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
                    cg_.context_info_.namedValues[ptrName] = newAlloca;
                    ptr = newAlloca;
                }
                if (!value || !ptr)
                {
                    cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", std::string("Operands not found for store: value=") + valueOp.name + ", ptr=" + ptrName}});
                    return nullptr;
                }
            }

            // ================================================================
            // (AR) ISSUE-063: مواءمة %SadDyn مع الخانات (قبل أنماط الترقية القديمة):
            //      1) قيمة %SadDyn في خانةٍ غير-%SadDyn ⇒ رقِّ الخانة إلى %SadDyn
            //         (نظير ترقية double في Fix #46) — كان CreateStore يكتب هيكل
            //         16 بايت في خانة i64 ⇒ IR فاسد ⇒ انهيار (قسمة `/` ديناميّة
            //         مخزَّنة في متغيّرٍ علويّ).
            //      2) قيمة محسوسة في خانة %SadDyn ⇒ علّبها (toDyn) قبل التخزين.
            // (EN) ISSUE-063: reconcile %SadDyn with slots (before the legacy promotion
            //      patterns): 1) a %SadDyn value into a non-%SadDyn slot ⇒ promote the
            //      slot to %SadDyn (mirror of the Fix #46 double promotion) — CreateStore
            //      used to write a 16-byte struct into an i64 slot ⇒ invalid IR ⇒ crash
            //      (a dynamic `/` result stored into a top-level variable). 2) a concrete
            //      value into a %SadDyn slot ⇒ pack it (toDyn) before storing.
            // ================================================================
            llvm::StructType *dynTy = getSadDynType(*cg_.context_);
            // ================================================================
            // (AR) ترقيةُ خانةٍ إلى %SadDyn تُبدّل مكانَ التخزين، لكنّ التحميلاتِ
            //      والتخزيناتِ **المُصدَرةَ سلفًا** تبقى مُشيرةً إلى الخانةِ القديمة.
            //      وهذا جوابٌ خاطئٌ صامت: في `مجموع = مجموع + ش.العمر` يُصدَر تحميلُ
            //      الطرفِ الأيسرِ قبلَ التخزينِ الذي يُطلق الترقية، فيقرأ اللولبُ في
            //      كلِّ دورةٍ الخانةَ القديمةَ التي لم يعد يكتبها أحد ⇒ المجموعُ =
            //      آخِرُ حدٍّ لا مجموعُ الحدود. فالإصلاحُ إعادةُ توجيهِ كلِّ استعمالٍ
            //      قائمٍ إلى الخانةِ الجديدة: التحميلُ يُفكّ تعليبَه إلى نوعِه القديم،
            //      والتخزينُ يُعلَّب. وإن وُجد استعمالٌ ليس تحميلًا ولا تخزينًا (مؤشّرٌ
            //      مُمرَّرٌ، GEP…) فلا يُعاد التوجيهُ أصلًا: تُترَك الخانةُ القديمةُ
            //      وتُرحَّل قيمتُها كما كان — لا نُصلح ما لا نفهمه.
            // (EN) Promoting a slot to %SadDyn moves the storage, but loads/stores
            //      ALREADY emitted still point at the old slot — a silent wrong answer
            //      (`sum = sum + p.age`: the LHS load precedes the store that triggers
            //      promotion, so every iteration re-reads a slot nobody writes anymore).
            //      Redirect every existing use: loads unbox to their old type, stores
            //      box. If any use is neither (a GEP, a passed pointer), redirect nothing
            //      and fall back to migrating the value as before.
            // ================================================================
            // (AR) تعليبُ الثابتِ الابتدائيّ. **يُرجع nullptr لما لا يفهمه** فيعود
            //      المُنادي إلى الترحيلِ زمنَ التشغيل: التخمينُ هنا جوابٌ خاطئٌ صامت
            //      (مبادِئٌ مؤشّرُ حرفيّةٍ نصّيّةٍ خُمِّن «عدمًا» ⇒ قراءةٌ قبلَ أوّلِ
            //      إسنادٍ تُعيد «لاشيء» بدل النصّ بلا تشخيص).
            //      و`i1` وسمُه **Bool** لا Int، وحمولتُه بالتمديدِ الصفريّ: قيمةُ
            //      `APInt(1,1)` موقَّعةً ‎−1‎ لا ١ ⇒ «‑1» بدل «صحيح».
            // (EN) Box a constant initializer, returning nullptr for anything it does not
            //      understand so the caller falls back to the runtime migration — guessing
            //      here is a silent wrong answer (a string-literal pointer guessed as Null
            //      reads back as «لاشيء»). `i1` is tagged Bool, not Int, and zero-extended:
            //      APInt(1,1) sign-extends to −1, which would print «-1» instead of «صحيح».
            auto boxInitializer = [&](llvm::Constant *init,
                                      llvm::StructType *dTy) -> llvm::Constant * {
                if (!init)
                    return nullptr;
                llvm::Type *i8Ty = cg_.builder_->getInt8Ty();
                llvm::Type *i64Ty = cg_.getInt64Type();
                uint8_t kind;
                llvm::Constant *payload;
                if (auto *ci = llvm::dyn_cast<llvm::ConstantInt>(init))
                {
                    const bool isBool = ci->getType()->isIntegerTy(1);
                    kind = static_cast<uint8_t>(isBool ? DynKind::Bool : DynKind::Int);
                    payload = llvm::ConstantInt::get(
                        i64Ty, isBool ? ci->getZExtValue()
                                      : static_cast<uint64_t>(ci->getSExtValue()));
                }
                else if (auto *cf = llvm::dyn_cast<llvm::ConstantFP>(init))
                {
                    kind = static_cast<uint8_t>(DynKind::Float);
                    payload = llvm::ConstantInt::get(
                        i64Ty, cf->getValueAPF().bitcastToAPInt().getZExtValue());
                }
                else if (llvm::isa<llvm::ConstantPointerNull>(init))
                {
                    kind = static_cast<uint8_t>(DynKind::Null);
                    payload = llvm::ConstantInt::get(i64Ty, 0);
                }
                else
                    return nullptr; // (AR) مبادِئٌ لا نعرف وسمَه ⇒ لا نُخمّن
                return llvm::ConstantStruct::get(
                    dTy, {llvm::ConstantInt::get(i8Ty, kind), payload});
            };

            auto redirectSlotUses = [&](llvm::Value *oldSlot, llvm::Value *newSlot,
                                        llvm::Type *oldTy) -> bool {
                std::vector<llvm::User *> uses(oldSlot->user_begin(), oldSlot->user_end());
                for (llvm::User *u : uses)
                {
                    if (auto *ld = llvm::dyn_cast<llvm::LoadInst>(u))
                    {
                        if (ld->getPointerOperand() != oldSlot)
                            return false;
                        continue;
                    }
                    if (auto *st = llvm::dyn_cast<llvm::StoreInst>(u))
                    {
                        if (st->getPointerOperand() != oldSlot)
                            return false;
                        continue;
                    }
                    return false;
                }
                llvm::BasicBlock *savedBB = cg_.builder_->GetInsertBlock();
                auto savedIt = cg_.builder_->GetInsertPoint();
                for (llvm::User *u : uses)
                {
                    if (auto *ld = llvm::dyn_cast<llvm::LoadInst>(u))
                    {
                        cg_.builder_->SetInsertPoint(ld);
                        llvm::Value *dv = cg_.builder_->CreateLoad(dynTy, newSlot, "promo.reload");
                        llvm::Value *cv = nullptr;
                        if (oldTy->isDoubleTy())
                            cv = unpackDouble(cg_, dv);
                        else if (oldTy->isPointerTy())
                            cv = unpackPtr(cg_, dv);
                        else
                        {
                            cv = unpackI64(cg_, dv);
                            if (oldTy->isIntegerTy() && !oldTy->isIntegerTy(64))
                                cv = cg_.builder_->CreateTrunc(cv, oldTy, "promo.trunc");
                        }
                        ld->replaceAllUsesWith(cv);
                        ld->eraseFromParent();
                    }
                    else if (auto *st = llvm::dyn_cast<llvm::StoreInst>(u))
                    {
                        cg_.builder_->SetInsertPoint(st);
                        llvm::Value *bv = toDyn(cg_, st->getValueOperand(), SadTypeKind::Unknown);
                        cg_.builder_->CreateStore(bv, newSlot);
                        st->eraseFromParent();
                    }
                }
                if (savedBB)
                    cg_.builder_->SetInsertPoint(savedBB, savedIt);
                return true;
            };

            if (value && ptr)
            {
                llvm::Type *slotTy = nullptr;
                if (auto *ai = llvm::dyn_cast<llvm::AllocaInst>(ptr))
                    slotTy = ai->getAllocatedType();
                else if (auto *g = llvm::dyn_cast<llvm::GlobalVariable>(ptr))
                    slotTy = g->getValueType();

                // ════════════════════════════════════════════════════════════
                // (AR) SEM045 (RFC عقد الغياب — أ٢): الحارس الزمنيّ قبل STORE —
                //      قيمةٌ ديناميّةٌ تُكتَب في خانةٍ **أعلنت نوعَها**. الفيصلُ سجلُّ
                //      `declaredTypedSlots` من الوجه الأماميّ لا نوعُ خانة LLVM:
                //      المسحُ المسبقُ (collectDynSlots) يجعل الخانةَ المصنّفةَ
                //      %SadDyn **منذ الحجز** حين يمرّ بها ديناميٌّ، ففرعُ الترقية
                //      أدناه لا يراها أصلًا (رقعةٌ في تلك الطبقة حارسٌ ميّت —
                //      قِيس). والخانةُ المستنتَجةُ (بلا تصريح) ليست في السجلّ فلا
                //      تُحرَس — عقدُ SEM045 نفسُه في المفسّر (hasDeclaredType).
                //      يُفحَص وسمُ القيمة: «فراغ» ⇒ تشخيصٌ بدرجةٍ مُشتقّةٍ من سياسة
                //      الذاكرة زمنَ التوليد. حدّان معلَنان: معاملاتُ الدوالّ ليست في
                //      السجلّ (موجة لاحقة)، والعامُّ المكتوبُ من دالّةٍ أخرى يُبحَث
                //      في سجلّ تلك الدالّة فيفلت (التصريحُ العلويّ محلّيُّ main).
                // (EN) SEM045 (stage أ٢): pre-STORE runtime guard — a dyn value
                //      written into an EXPLICITLY-declared slot. The discriminator
                //      is the frontend's declaredTypedSlots record, not the LLVM
                //      slot type: the pre-scan allocates typed-but-tainted slots as
                //      %SadDyn from the start, so the promotion branch below never
                //      sees them (measured dead chokepoint). Declared limits:
                //      function parameters and cross-function global stores.
                // ════════════════════════════════════════════════════════════
                if (isSadDyn(value) &&
                    cg_.voidStoreGuard() != Sad::LLVM::LLVMCodeGen::VoidStoreGuard::None)
                {
                    llvm::Function *curFn = cg_.builder_->GetInsertBlock()
                                                ? cg_.builder_->GetInsertBlock()->getParent()
                                                : nullptr;
                    if (curFn)
                    {
                        const auto declaredKind = cg_.declaredSlotKind(
                            curFn->getName().str(), inst->operands[1].name);
                        if (declaredKind != Sad::Types::SadTypeKind::Unknown)
                        {
                            emitDynVoidStoreGuard(
                                cg_, value, inst->operands[1].name,
                                Sad::Types::kindToArabic(declaredKind),
                                cg_.voidStoreGuard() ==
                                    Sad::LLVM::LLVMCodeGen::VoidStoreGuard::Fatal);
                        }
                    }
                }

                if (slotTy && isSadDyn(value) && slotTy != dynTy)
                {
                    if (auto *g = llvm::dyn_cast<llvm::GlobalVariable>(ptr))
                    {
                        // (AR) ترقية متغيّر عامّ إلى %SadDyn: جديدٌ بنفس الاسم + ترحيل القيمة
                        // (EN) Promote a global to %SadDyn: new global + migrate the old value
                        std::string globalName = g->getName().str();
                        auto *newGV = new llvm::GlobalVariable(
                            *cg_.module_, dynTy, false,
                            llvm::GlobalValue::InternalLinkage,
                            llvm::ConstantAggregateZero::get(dynTy),
                            globalName + ".dyn");
                        // (AR) القيمةُ الابتدائيّةُ تُعلَّب ثابتًا (لا نداءَ زمنِ تشغيل)،
                        //      ثمّ يُعاد توجيهُ كلِّ استعمالٍ قائم. فإن تعذّر التوجيهُ
                        //      رُحِّلت القيمةُ زمنَ التشغيل كما كان.
                        // (EN) Box the initializer as a constant, then redirect every
                        //      existing use; if redirection is not possible, migrate at
                        //      runtime as before.
                        llvm::Constant *boxedInit =
                            g->hasInitializer() ? boxInitializer(g->getInitializer(), dynTy) : nullptr;
                        const bool redirected = boxedInit && redirectSlotUses(g, newGV, slotTy);
                        if (redirected)
                            newGV->setInitializer(boxedInit);
                        else
                        {
                            llvm::Value *oldVal = cg_.builder_->CreateLoad(slotTy, g, "old.glob.val");
                            cg_.builder_->CreateStore(
                                toDyn(cg_, oldVal, SadTypeKind::Unknown), newGV);
                        }
                        for (auto &kv : cg_.context_info_.globalValues)
                            if (kv.second == g)
                                kv.second = newGV;
                        for (auto &kv : cg_.context_info_.namedValues)
                            if (kv.second == g)
                                kv.second = newGV;
                        // (AR) الحذفُ **بعد** تصحيحِ الخرائط: لو حُذف قبلَها لقارنّا
                        //      مؤشّرًا محرَّرًا فتبقى فيها إحالاتٌ ميّتة. ومجموعةُ
                        //      المتطايرات تُنقَل كذلك: إبقاءُ مؤشّرٍ محرَّرٍ فيها يجعل
                        //      عامًّا لاحقًا يقع في العنوانِ نفسِه يُوسَم متطايرًا خطأً.
                        // (EN) Erase AFTER fixing the maps. The volatile set is migrated too:
                        //      a freed pointer left there would mark a later global that lands
                        //      on the same address as volatile.
                        if (cg_.context_info_.volatileGlobalVars.erase(g))
                            cg_.context_info_.volatileGlobalVars.insert(newGV);
                        if (redirected && g->use_empty())
                            g->eraseFromParent();
                        ptr = newGV;
                    }
                    else if (auto *ai = llvm::dyn_cast<llvm::AllocaInst>(ptr))
                    {
                        // (AR) ترقية alloca إلى %SadDyn (نظير ترقية double/متجه)
                        // (EN) Promote the alloca to %SadDyn (mirror of double/vector promotion)
                        llvm::Function *currentFunc = cg_.builder_->GetInsertBlock()->getParent();
                        llvm::IRBuilder<> entryBuilder(&currentFunc->getEntryBlock(),
                                                       currentFunc->getEntryBlock().begin());
                        std::string allocaName = ai->getName().str();
                        llvm::AllocaInst *newAlloca = entryBuilder.CreateAlloca(
                            dynTy, nullptr, allocaName + ".dyn");
                        const bool redirected = redirectSlotUses(ai, newAlloca, slotTy);
                        if (!redirected)
                        {
                            llvm::Value *oldVal = cg_.builder_->CreateLoad(slotTy, ai, "old.slot.val");
                            cg_.builder_->CreateStore(
                                toDyn(cg_, oldVal, SadTypeKind::Unknown), newAlloca);
                        }
                        for (auto &kv : cg_.context_info_.namedValues)
                            if (kv.second == ai)
                                kv.second = newAlloca;
                        if (redirected && ai->use_empty())
                            ai->eraseFromParent();
                        ptr = newAlloca;
                    }
                }
                else if (slotTy == dynTy && !isSadDyn(value))
                {
                    // (AR) محسوسٌ في خانة %SadDyn ⇒ تعليبٌ بنوع SIR الساكن
                    // (EN) A concrete value into a %SadDyn slot ⇒ pack by its static SIR type
                    value = toDyn(cg_, value, valueOp.dataType);
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
                        value = cg_.builder_->CreatePtrToInt(value, cg_.getInt64Type(), "obj.ptrtoint");
                    }
                }
                else if (auto *gv = llvm::dyn_cast<llvm::GlobalVariable>(ptr))
                {
                    // (AR) تخزين مؤشر في متغير عام i64 — تحويل ptr→i64
                    // (EN) Storing pointer into global i64 variable — convert ptr→i64
                    if (value->getType()->isPointerTy() && gv->getValueType()->isIntegerTy(64))
                    {
                        value = cg_.builder_->CreatePtrToInt(value, cg_.getInt64Type(), "obj.glob.ptrtoint");
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
                        llvm::Value *oldVal = cg_.builder_->CreateLoad(cg_.getInt64Type(), gv, "old.glob.i64");

                        // (AR) إنشاء متغير عام جديد بنوع double
                        // (EN) Create new global variable with double type
                        auto *newGV = new llvm::GlobalVariable(
                            *cg_.module_, cg_.builder_->getDoubleTy(), false,
                            llvm::GlobalValue::InternalLinkage,
                            llvm::ConstantFP::get(cg_.builder_->getDoubleTy(), 0.0),
                            globalName + ".f64");

                        // (AR) نقل القيمة القديمة إلى المتغير الجديد
                        // (EN) Migrate old value to new global
                        llvm::Value *oldAsF64 = cg_.builder_->CreateSIToFP(oldVal, cg_.builder_->getDoubleTy(), "old.glob.f64");
                        cg_.builder_->CreateStore(oldAsF64, newGV);

                        // (AR) استبدال المرجع في globalValues و namedValues
                        // (EN) Replace references in globalValues and namedValues
                        for (auto &kv : cg_.context_info_.globalValues)
                        {
                            if (kv.second == gv)
                            {
                                kv.second = newGV;
                                break;
                            }
                        }
                        for (auto &kv : cg_.context_info_.namedValues)
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
                    // ================================================================
                    // (AR) 🔑 ترقيةُ المتغيّرِ العامِّ إلى نوعِ المتجه — نظيرُ ترقيةِ
                    //      الـalloca أدناه، وكان **مفقودًا** فبقيَ نصفُ العلّةِ حيًّا.
                    //      قِيس: `متجه(١٫٠، ٢٫٠)` في الفضاءِ الأعلى يُنتج
                    //        @م = internal global i64 0
                    //        store <2 x double> …, ptr @م, align 16
                    //      أي **ستّةَ عشرَ بايتًا تُكتَبُ في ثمانية** بمحاذاةِ ١٦ على
                    //      عامٍّ محاذاتُه ٨ ⇒ انهيارُ الثنائيِّ segfault؛ ثمّ تُقرَأُ
                    //      الخانةُ بنوعِها المُعلَنِ فيصلُ المعاملُ سُلَّميًّا إلى ذراعِ
                    //      SIMD. وداخلَ دالّةٍ يعملُ الأمرُ سليمًا لأنّ الخانةَ alloca
                    //      وترقيتُها مكتوبة — **الرقعةُ سُدَّت في صنفِ تخزينٍ وتركت أخاه**.
                    //      🔑 ولم يحمرَّ `verifyModule` لأنّ المؤشّرَ مُبهَمٌ (ptr) منذ
                    //         LLVM 15: لا نوعَ في المؤشّرِ يُكذِّبُ حجمَ المخزون.
                    // (EN) Promote a GLOBAL to the vector type — the twin of the alloca
                    //      promotion below, which was missing: a 16-byte <2 x double>
                    //      was stored into an 8-byte i64 global (align 16 on an 8-aligned
                    //      global) ⇒ segfault. Opaque pointers keep verifyModule silent.
                    // ================================================================
                    else if (value->getType()->isVectorTy() && !gv->getValueType()->isVectorTy())
                    {
                        std::string globalName = gv->getName().str();
                        llvm::Type *vecTy = value->getType();
                        auto *newGV = new llvm::GlobalVariable(
                            *cg_.module_, vecTy, false,
                            llvm::GlobalValue::InternalLinkage,
                            llvm::ConstantAggregateZero::get(vecTy),
                            globalName + ".vec");
                        // (AR) محاذاةٌ صريحةٌ بعرضِ المتجه — تعليماتُ SIMD تشترطُها،
                        //      والقيمةُ القديمةُ **لا تُرحَّل**: خانةٌ كانت i64 ثمّ صارت
                        //      متجهًا لا معنًى لمحتواها السابقِ في التمثيلِ الجديد.
                        // (EN) Explicit vector-width alignment; the old scalar content is
                        //      deliberately NOT migrated (no meaning in the new representation).
                        unsigned alignBytes = vecTy->getPrimitiveSizeInBits() / 8;
                        if (alignBytes >= 16)
                            newGV->setAlignment(llvm::Align(alignBytes));

                        for (auto &kv : cg_.context_info_.globalValues)
                            if (kv.second == gv)
                            {
                                kv.second = newGV;
                                break;
                            }
                        for (auto &kv : cg_.context_info_.namedValues)
                            if (kv.second == gv)
                            {
                                kv.second = newGV;
                                break;
                            }
                        ptr = newGV;
                    }
                    // (AR) تخزين i64 في متغير عام double — تحويل i64→double (SIToFP)
                    // (EN) Storing i64 into global double variable — convert i64→double (SIToFP)
                    else if (value->getType()->isIntegerTy(64) && gv->getValueType()->isDoubleTy())
                    {
                        value = cg_.builder_->CreateSIToFP(value, cg_.builder_->getDoubleTy(), "i64.to.f64.store");
                    }
                }

                // (AR) التحقق من تطابق الأنواع لمتغيرات alloca المحلية أيضاً
                // (EN) Check type compatibility for local alloca variables too
                if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(ptr))
                {
                    // ================================================================
                    // (AR) [Phase 3 — SIMD] تخزين متجه <N x T> في alloca i64 — ترقية
                    //      عند `متغير أ = متجه(...)` يكون SIROpcode::ALLOC قد خصّص i64
                    //      (لأن SadTypeKind::Vector يُعالج كـ default → i64)، لكن القيمة
                    //      الفعلية متجه LLVM (32/64 بايت). نُرقّي الـ alloca إلى نوع
                    //      المتجه الكامل لضمان store/load سليم وأداء SIMD حقيقي.
                    // (EN) [Phase 3 — SIMD] Storing <N x T> vector into i64 alloca — promote.
                    //      `var a = vector(...)` allocates i64 (since Vector type defaults
                    //      to i64), but the actual value is an LLVM vector (32/64 bytes).
                    //      Promote the alloca to the full vector type to ensure correct
                    //      store/load and real SIMD performance.
                    // ================================================================
                    if (value->getType()->isVectorTy() &&
                        !allocaInst->getAllocatedType()->isVectorTy())
                    {
                        llvm::Function *currentFunc = cg_.builder_->GetInsertBlock()->getParent();
                        llvm::IRBuilder<> entryBuilder(&currentFunc->getEntryBlock(),
                                                       currentFunc->getEntryBlock().begin());
                        std::string allocaName = allocaInst->getName().str();
                        // (AR) ننشئ alloca جديد بنوع المتجه الفعلي (يستنتج المحاذاة الصحيحة)
                        llvm::AllocaInst *newAlloca = entryBuilder.CreateAlloca(
                            value->getType(), nullptr, allocaName + ".vec");
                        // (AR) ضبط محاذاة صريحة لتعليمات SIMD (16/32/64 بايت)
                        unsigned vecBits = value->getType()->getPrimitiveSizeInBits();
                        unsigned alignBytes = vecBits / 8;
                        if (alignBytes >= 16)
                            newAlloca->setAlignment(llvm::Align(alignBytes));

                        // (AR) استبدال المرجع في namedValues
                        for (auto &kv : cg_.context_info_.namedValues)
                        {
                            if (kv.second == allocaInst)
                            {
                                kv.second = newAlloca;
                                break;
                            }
                        }
                        ptr = newAlloca;
                        allocaInst = newAlloca;
                        // القيمة بالفعل متجه — لا تحويل
                    }
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
                        llvm::Function *currentFunc = cg_.builder_->GetInsertBlock()->getParent();
                        llvm::IRBuilder<> entryBuilder(&currentFunc->getEntryBlock(),
                                                       currentFunc->getEntryBlock().begin());
                        std::string allocaName = allocaInst->getName().str();
                        llvm::AllocaInst *newAlloca = entryBuilder.CreateAlloca(
                            cg_.builder_->getDoubleTy(), nullptr, allocaName + ".f64");

                        // (AR) نقل القيمة القديمة (i64) إلى الـ alloca الجديد (double)
                        // (EN) Migrate old value (i64) to new alloca (double)
                        llvm::Value *oldVal = cg_.builder_->CreateLoad(cg_.getInt64Type(), allocaInst, "old.i64.val");
                        llvm::Value *oldAsF64 = cg_.builder_->CreateSIToFP(oldVal, cg_.builder_->getDoubleTy(), "old.as.f64");
                        cg_.builder_->CreateStore(oldAsF64, newAlloca);

                        // (AR) استبدال المرجع في namedValues + تحديث ptr
                        // (EN) Replace reference in namedValues + update ptr
                        for (auto &kv : cg_.context_info_.namedValues)
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
                        value = cg_.builder_->CreateSIToFP(value, cg_.builder_->getDoubleTy(), "i64.to.f64.alloca");
                    }
                    // (AR) تخزين i1 (bool) في alloca i64 — توسيع i1→i64
                    // (EN) Storing i1 (bool) into i64 alloca — zero-extend i1→i64
                    else if (value->getType()->isIntegerTy(1) &&
                             allocaInst->getAllocatedType()->isIntegerTy(64))
                    {
                        value = cg_.builder_->CreateZExt(value, cg_.getInt64Type(), "i1.to.i64.alloca");
                    }
                }
            }

            auto *storeResult = cg_.builder_->CreateStore(value, ptr);

            // ================================================================
            // نشر خريطة الأصناف عند تخزين كائن في متغير
            // Propagate class map when storing object into variable
            // ================================================================
            // (AR) الكتابة فوق أيّ ربط سابق (بذرة تصريح أو نشر أقدم) — إعادة
            //      إسناد المتغيّر لكائن من صنف آخر يجب أن تحدّث صنفه وإلّا
            //      بقي GEP بتخطيط الصنف القديم فوق الكائن الجديد (قيمة خاطئة صامتة)
            // (EN) Overwrite any prior binding (declared-param seed or older
            //      propagation) — reassigning the variable to another class's
            //      object must update its class, else GEPs keep the stale layout.
            if (cg_.context_info_.objectClassMap.count(valueOp.name))
            {
#ifndef NDEBUG
                auto prevIt = cg_.context_info_.objectClassMap.find(ptrName);
                if (prevIt != cg_.context_info_.objectClassMap.end() &&
                    prevIt->second != cg_.context_info_.objectClassMap[valueOp.name])
                {
                    SAD_DEBUG_LOG_LINE("[DEBUG] emitStore: OVERWRITE class '" << prevIt->second
                              << "' -> '" << cg_.context_info_.objectClassMap[valueOp.name]
                              << "' on " << ptrName << " (from " << valueOp.name << ")");
                }
#endif
                cg_.context_info_.objectClassMap[ptrName] = cg_.context_info_.objectClassMap[valueOp.name];
#ifndef NDEBUG
                SAD_DEBUG_LOG_LINE("[DEBUG] emitStore: propagated class '"
                          << cg_.context_info_.objectClassMap[valueOp.name]
                          << "' from " << valueOp.name << " to " << ptrName);
#endif
            }

            // م-أ03: فحص إذا كان المتغير متطايراً (volatile) — لسجلات الأجهزة MMIO
            // (AR) اللبنة 3.14: أو كان المخزن العامّ موسومًا @متطاير (بالاسم مجرَّدًا من %)
            {
                std::string vname = ptrName;
                if (!vname.empty() && vname[0] == '%')
                    vname = vname.substr(1);
                if (inst->operands[1].name.find("volatile") != std::string::npos ||
                    inst->operands[1].name.find("\xd9\x85\xd8\xaa\xd8\xb7\xd8\xa7\xd9\x8a\xd8\xb1") != std::string::npos ||
                    cg_.context_info_.volatileGlobals.count(vname))
                {
                    storeResult->setVolatile(true);
                }
            }

            return storeResult;
        }

    } // namespace LLVM
} // namespace Sad
