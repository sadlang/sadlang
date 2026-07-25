// ============================================================================
// llvm_codegen_classes_vtables.cpp — معالجة الأصناف وجداول الدوال الافتراضية
// LLVM IR: Class preprocessing, vtables, virtual calls
// ============================================================================
// (AR) هذا الملف يحتوي على:
//      - preprocessClasses: إنشاء أنواع الهياكل LLVM للأصناف
//      - emitModule: إصدار وحدة SIR كاملة
//      - emitGlobalVariables: المتغيرات العامة
//      - buildClassVtables, patchClassVtables: بناء جداول الدوال الافتراضية
//      - storeVtablePtr, emitVirtualCall, emitDestructorCall
// (EN) Class preprocessing, module emission, vtables, virtual dispatch
// تم استخراج هذا الملف من llvm_codegen_init.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include "llvm_codegen.h"
#include "builders/oop/classes_vtables_codegen.h"
#include "sad_dyn_repr.h" // (AR) ISSUE-076: النوع الديناميّ %SadDyn لخانات حمولة ADT
#include "sad_event_layout_generated.h" // (② rfcs#46) اسم صنف «حدث» المضمَّن من SoT — لوسم builtinClassNames
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
#include <llvm/TargetParser/Triple.h> // (AR) [RFC #53 F2-ج] تمييز SysV/Win64 لتصنيف ABI
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <functional>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
        // ============================================================================
        // معالجة الأصناف - إنشاء أنواع الهياكل LLVM
        // Pre-process classes - create LLVM struct types
        // ============================================================================
        void ClassesVtablesCodeGen::preprocessClasses(std::shared_ptr<SIRModule> sirModule)
        {
            if (!sirModule)
                return;

            // (AR) ترتيب الأصناف حسب الاعتمادية (الأب قبل الابن)
            // (EN) Topological sort: process parent classes before children
            const auto &allClasses = sirModule->getClasses();
            std::vector<std::shared_ptr<SIRClass>> sortedClasses;
            std::unordered_set<std::string> processed;

            // (AR) دالة مساعدة للترتيب التبولوجي
            // (EN) Helper for topological sort
            std::function<void(const std::shared_ptr<SIRClass> &)> processClass;
            processClass = [&](const std::shared_ptr<SIRClass> &cls)
            {
                if (!cls || processed.count(cls->name))
                    return;
                // (AR) معالجة الأب أولاً
                // (EN) Process parent first
                if (!cls->parentClass.empty() && !processed.count(cls->parentClass))
                {
                    for (const auto &other : allClasses)
                    {
                        if (other && other->name == cls->parentClass)
                        {
                            processClass(other);
                            break;
                        }
                    }
                }
                processed.insert(cls->name);
                sortedClasses.push_back(cls);
            };

            for (const auto &cls : allClasses)
            {
                processClass(cls);
            }

            // (AR) معالجة كل صنف في الوحدة وإنشاء نوع هيكل LLVM له
            // (EN) Process each class in the module and create LLVM struct type
            for (const auto &sirClass : sortedClasses)
            {
                if (!sirClass)
                    continue;

                const std::string &className = sirClass->name;

                // (AR) وسم صنف «حدث» المضمَّن (② rfcs#46، اسمه من SoT المولَّد) —
                //      يبقى مسجَّلًا كاملًا (thunk الحدث يحتاج تخطيطه) لكنّه يُستثنى
                //      من احتياطيّ تخمين الصنف من اسم الحقل (انظر builtinClassNames).
                // (EN) Mark the builtin «حدث» event class (② rfcs#46, name from the
                //      generated SoT) — still fully registered (the event thunk needs
                //      its layout) but excluded from the infer-class-from-field-name
                //      fallback (see builtinClassNames).
                if (className == ::Sad::Types::EventLayout::SAD_EVENT_STRUCT_NAME)
                {
                    cg_.context_info_.builtinClassNames.insert(className);
                }

                // (AR) تسجيل علاقة الوراثة
                // (EN) Register inheritance relationship
                if (!sirClass->parentClass.empty())
                {
                    cg_.context_info_.classParentMap[className] = sirClass->parentClass;
#ifndef NDEBUG
                    std::cout << "[DEBUG] preprocessClasses: '" << className
                              << "' inherits from '" << sirClass->parentClass << "'" << std::endl;
#endif
                }

                // (AR) تسجيل الصنف المجرد
                // (EN) Register abstract class
                if (sirClass->isAbstract)
                {
                    cg_.context_info_.abstractClasses.insert(className);
                }

                // (AR) جمع أنواع الحقول بالترتيب
                //      الحقل 0 دائماً مؤشر vtable — إلا في بنى ADT (تبدأ بـ __tag)
                // (EN) Collect field types in order
                //      Field 0 is always vtable pointer — except for ADT structs (starting with __tag)
                std::vector<llvm::Type *> fieldTypes;
                std::vector<std::string> fieldNames;

                // (AR) فحص إذا كانت بنية ADT (الحقل الأول هو __tag)
                // (EN) Check if this is an ADT struct (first field is __tag)
                bool isADTStruct = !sirClass->fieldOrder_.empty() &&
                                   sirClass->fieldOrder_[0] == "__tag";

                // (AR) RFC #53 F2-ب: بنية @تمثيل_سي تُسقِط ترويسة vtable ⇒ الحقل 0 هو أوّل حقل
                //      للمستخدم، فيطابق التخطيطُ بنيةَ C نظيرةً (بلا إزاحة 8 بايت خفيّة). نسجّلها
                //      لتُصحّح إزاحات GEP في getFieldStructIndex ويُتخطّى تخزين vtable.
                // (EN) RFC #53 F2-ب: a @تمثيل_سي struct drops the vtable header ⇒ field 0 is the
                //      first user field, so the layout matches a peer C struct (no hidden 8-byte
                //      shift). Register it so getFieldStructIndex fixes GEP offsets and the
                //      vtable store is skipped.
                if (sirClass->isCRepr)
                {
                    cg_.context_info_.cReprClasses.insert(className);
                }

                if (!isADTStruct && !sirClass->isCRepr)
                {
                    // (AR) الحقل 0: مؤشر vtable (ptr) — لدعم الاستدعاء الافتراضي
                    // (EN) Field 0: vtable pointer (ptr) — for virtual dispatch
                    fieldTypes.push_back(llvm::PointerType::getUnqual(*cg_.context_));
                }

                for (const auto &fieldName : sirClass->fieldOrder_)
                {
                    // (AR) تحويل أنواع الحقول بشكل صحيح بدلاً من استخدام i64 لكل شيء
                    // (EN) Convert field types properly instead of using i64 for everything
                    auto fieldIt = sirClass->fields_.find(fieldName);
                    if (fieldIt != sirClass->fields_.end())
                    {
                        switch (fieldIt->second)
                        {
                        case SadTypeKind::Integer:
                            fieldTypes.push_back(cg_.getInt64Type());
                            break;
                        case SadTypeKind::Float:
                            fieldTypes.push_back(cg_.getDoubleType());
                            break;
                        case SadTypeKind::Boolean:
                            fieldTypes.push_back(cg_.getInt1Type());
                            break;
                        case SadTypeKind::String:
                        case SadTypeKind::Pointer:
                            fieldTypes.push_back(llvm::PointerType::getUnqual(*cg_.context_));
                            break;
                        case SadTypeKind::Any:
                            // (AR) ISSUE-076 (%SadDyn): حقلٌ ديناميّ (حمولة ADT غير منمّطة) ⇒ خانة
                            //      واصفة لذاتها %SadDyn = { i8 kind; i64 payload } بدل وسم البتّات.
                            // (EN) ISSUE-076 (%SadDyn): a dynamic field (untyped ADT payload) ⇒ the
                            //      self-describing slot %SadDyn = { i8 kind; i64 payload } (no bit-tagging).
                            fieldTypes.push_back(getSadDynType(*cg_.context_));
                            break;
                        case SadTypeKind::Struct:
                        {
                            // (AR) بحث عن نوع هيكل الصنف المُرجع
                            // (EN) Look up referenced class struct type
                            // حالياً نستخدم مؤشر عام للكائنات المتداخلة
                            fieldTypes.push_back(llvm::PointerType::getUnqual(*cg_.context_));
                            break;
                        }
                        default:
                            // (AR) احتياطي: i64 للأنواع غير المعروفة
                            // (EN) Fallback: i64 for unknown types
                            fieldTypes.push_back(cg_.getInt64Type());
                            break;
                        }
                    }
                    else
                    {
                        fieldTypes.push_back(cg_.getInt64Type()); // احتياطي / fallback
                    }
                    fieldNames.push_back(fieldName);
                }

                // (AR) إنشاء نوع الهيكل
                // (EN) Create struct type
                if (!fieldTypes.empty())
                {
                    // (AR) بنى ADT تستخدم اسم الصنف مباشرة، الأصناف العادية تستخدم "class."
                    // (EN) ADT structs use class name directly, regular classes use "class." prefix
                    std::string structTypeName = isADTStruct ? className : ("class." + className);
                    llvm::StructType *structType = llvm::StructType::create(
                        *cg_.context_, fieldTypes, structTypeName);
                    cg_.context_info_.classStructTypes[className] = structType;
                    cg_.context_info_.classFieldNames[className] = fieldNames;

#ifndef NDEBUG
                    std::cout << "[DEBUG] preprocessClasses: created struct type for class '"
                              << className << "' with " << fieldNames.size() << " fields" << std::endl;
#endif
                }
            }
        }

        /**
         * إصدار وحدة SIR كاملة
         * Emit complete SIR module
         *
         * Source: llvm_codegen.h:316
         * @param sirModule وحدة SIR / SIR module
         */
        void ClassesVtablesCodeGen::emitModule(std::shared_ptr<SIRModule> sirModule)
        {
            if (!sirModule)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_NULL_IR, {{"detail", "SIR"}});
                return;
            }

            // إصدار المتغيرات العامة
            // Emit global variables
            emitGlobalVariables(sirModule);

            // إصدار الثوابت
            // Emit constants
            cg_.emitConstants(sirModule);

            // (AR) إصدار الدوال العامة
            // (EN) Emit global functions
            cg_.emitGlobalFunctions(sirModule);

            // (AR) في الوضع المستقل: توليد تطبيقات مدمجة لدوال C الأساسية
            // (EN) In freestanding mode: emit built-in C runtime implementations
            if (cg_.freestanding_)
            {
                cg_.emitFreestandingRuntime();
            }

            // (AR) اللبنة 3.14: تمريرة @متطاير اللاحقة — تعلّم كلّ load/store يمسّ
            //      مخزنًا عامًّا موسومًا @متطاير بأنّه volatile. متينة ضدّ تعدّد مسارات
            //      إصدار الأحمال (arith_resolve/mem_load/oop_ops/…): نطابق بالمؤشّر
            //      (GlobalVariable الأساس بعد تجريد التحويلات) لا بالاسم.
            if (!cg_.context_info_.volatileGlobalVars.empty())
            {
                for (llvm::Function &fn : *cg_.module_)
                {
                    for (llvm::BasicBlock &bb : fn)
                    {
                        for (llvm::Instruction &inst : bb)
                        {
                            llvm::Value *ptr = nullptr;
                            if (auto *ld = llvm::dyn_cast<llvm::LoadInst>(&inst))
                                ptr = ld->getPointerOperand();
                            else if (auto *st = llvm::dyn_cast<llvm::StoreInst>(&inst))
                                ptr = st->getPointerOperand();
                            else
                                continue;
                            auto *gv = llvm::dyn_cast<llvm::GlobalVariable>(
                                ptr->stripPointerCasts());
                            if (gv && cg_.context_info_.volatileGlobalVars.count(gv))
                            {
                                if (auto *ld = llvm::dyn_cast<llvm::LoadInst>(&inst))
                                    ld->setVolatile(true);
                                else if (auto *st = llvm::dyn_cast<llvm::StoreInst>(&inst))
                                    st->setVolatile(true);
                            }
                        }
                    }
                }
            }
        }

        /**
         * إصدار المتغيرات العامة
         * Emit global variables
         *
         * Source: llvm_codegen.h:329
         * @param sirModule وحدة SIR / SIR module
         */
        void ClassesVtablesCodeGen::emitGlobalVariables(std::shared_ptr<SIRModule> sirModule)
        {
            if (!sirModule)
            {
                return;
            }

            // Source: SIRModule::getGlobalVariables() returns const std::vector<std::shared_ptr<SIRGlobalVariable>>&
            // من sir_module.h - الوصول للمتغيرات العامة
            const auto &globals = sirModule->getGlobalVariables();

            for (const auto &globalVar : globals)
            {
                if (!globalVar)
                    continue;

                // Source: SIRGlobalVariable::getName() is at sir_module.h:374
                // Source: SIRGlobalVariable::getType() is at sir_module.h:380
                // Source: SIRGlobalVariable::getIsConstant() is at sir_module.h:386
                std::string varName = globalVar->getName();
                SadTypeKind varType = globalVar->getType();
                bool isConstant = globalVar->getIsConstant();

                // ================================================================
                // (AR) ISSUE-063: متغيّرٌ عامّ قرّره المسحُ المسبق ديناميًّا (أو صرّحت به
                //      الواجهة Any) ⇒ يُنشأ %SadDyn من البداية بهويّته العامّة الوحيدة —
                //      كانت ترقيةُ منتصف التدفّق تُنشئ «name.dyn» جديدًا داخل __sad_main
                //      فتنفصم نسخةُ الدوال (المُصدَرة على القديم) عن نسخة المستوى الأعلى.
                //      الصفر الابتدائيّ = {وسم عدم، 0} — تخزينُ التهيئة في __sad_main
                //      يعلّبه قبل أوّل قراءة.
                // (EN) ISSUE-063: a global the pre-scan (or the frontend, via Any) decided
                //      is dynamic ⇒ created as %SadDyn from the start under its single
                //      global identity — the mid-flow promotion used to mint a fresh
                //      "name.dyn" inside __sad_main, splitting functions (emitted against
                //      the old global) from the top level. Zero-init = {Null kind, 0};
                //      the __sad_main init store packs it before the first read.
                // ================================================================
                if (cg_.dynGlobalSlots_.count(varName) || varType == SadTypeKind::Any)
                {
                    llvm::StructType *dynTy = getSadDynType(*cg_.context_);
                    auto *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
                    auto *i64Ty = llvm::Type::getInt64Ty(*cg_.context_);

                    // (AR) قيمة أولى معلَّبة ثابتًا حسب النوع المعلَن (إن وُجدت) وإلّا {عدم، 0}
                    // (EN) constant-packed initializer per the declared type (if any), else {Null, 0}
                    llvm::Constant *dynInit = llvm::ConstantAggregateZero::get(dynTy);
                    if (!globalVar->initialValue.empty())
                    {
                        try
                        {
                            uint8_t kind = DynKind::Null;
                            llvm::Constant *payload = llvm::ConstantInt::get(i64Ty, 0);
                            switch (varType)
                            {
                            case SadTypeKind::Integer:
                                kind = DynKind::Int;
                                payload = llvm::ConstantInt::get(
                                    i64Ty, std::stoll(globalVar->initialValue), true);
                                break;
                            case SadTypeKind::Float:
                            {
                                kind = DynKind::Float;
                                llvm::APFloat apf(std::stod(globalVar->initialValue));
                                payload = llvm::ConstantInt::get(
                                    i64Ty, apf.bitcastToAPInt().getZExtValue());
                                break;
                            }
                            case SadTypeKind::Boolean:
                                kind = DynKind::Bool;
                                payload = llvm::ConstantInt::get(
                                    i64Ty, std::stoll(globalVar->initialValue) != 0 ? 1 : 0);
                                break;
                            default:
                                break;
                            }
                            if (kind != DynKind::Null)
                                dynInit = llvm::ConstantStruct::get(
                                    dynTy, {llvm::ConstantInt::get(i8Ty, kind), payload});
                        }
                        catch (...)
                        {
                            dynInit = llvm::ConstantAggregateZero::get(dynTy);
                        }
                    }

                    auto *dynGV = new llvm::GlobalVariable(
                        *cg_.module_, dynTy, false,
                        llvm::GlobalValue::InternalLinkage,
                        dynInit,
                        varName);
                    cg_.context_info_.namedValues[varName] = dynGV;
                    cg_.context_info_.globalValues[varName] = dynGV;
                    continue;
                }

                // تحويل النوع إلى LLVM
                // Convert type to LLVM
                llvm::Type *llvmType = nullptr;

                // تحويل أنواع SIR الأساسية
                // Convert basic SIR types
                // Source: SadTypeKind enum is defined at sir_types.h:114-119
                switch (varType)
                {
                case SadTypeKind::Integer:
                    llvmType = cg_.getInt64Type();
                    break;
                case SadTypeKind::Float:
                    llvmType = cg_.getDoubleType();
                    break;
                case SadTypeKind::Boolean:
                    llvmType = cg_.getInt1Type();
                    break;
                case SadTypeKind::Pointer:
                    llvmType = cg_.getInt8PtrType();
                    break;
                case SadTypeKind::Void:
                    llvmType = cg_.getVoidType();
                    break;
                default:
                    llvmType = cg_.getInt64Type(); // افتراضي / Default
                    break;
                }

                if (!llvmType)
                {
                    cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", std::string("Failed to convert type for global variable:") + varName}});
                    continue;
                }

                // إنشاء قيمة أولية
                // Create initializer
                llvm::Constant *initializer = nullptr;

                // Source: SIRGlobalVariable::initialValue is at sir_module.h:344
                if (!globalVar->initialValue.empty())
                {
                    // (AR) تحليل القيمة الأولية بناءً على النوع
                    // (EN) Parse initial value based on type
                    try
                    {
                        switch (varType)
                        {
                        case SadTypeKind::Integer:
                        case SadTypeKind::Boolean:
                            initializer = llvm::ConstantInt::get(
                                llvmType, std::stoll(globalVar->initialValue), true);
                            break;
                        case SadTypeKind::Float:
                            initializer = llvm::ConstantFP::get(
                                llvmType, std::stod(globalVar->initialValue));
                            break;
                        case SadTypeKind::String:
                        {
                            // (AR) المتغير الساكن النصي: أنشئ ثابت نصي عالمي ثم خزّن عنوانه كـ i64
                            //      باستخدام ConstantExpr::getPtrToInt لتكون القيمة الأولية صحيحة
                            //      حتى يعمل inttoptr لاحقاً ويُطبع النص بشكل صحيح
                            // (EN) Static string variable: create global string constant then store
                            //      its address as i64 using ConstantExpr::getPtrToInt so the
                            //      initializer is correct and inttoptr works later for proper printing
                            auto *strData = llvm::ConstantDataArray::getString(
                                *cg_.context_, globalVar->initialValue, true);
                            auto *strGlobal = new llvm::GlobalVariable(
                                *cg_.module_,
                                strData->getType(),
                                true, // constant
                                llvm::GlobalValue::PrivateLinkage,
                                strData,
                                varName + ".str.init");
                            strGlobal->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
                            // تحويل المؤشر إلى i64 كقيمة أولية ثابتة
                            initializer = llvm::ConstantExpr::getPtrToInt(
                                strGlobal, llvm::Type::getInt64Ty(*cg_.context_));
                            break;
                        }
                        default:
                            initializer = llvm::Constant::getNullValue(llvmType);
                            break;
                        }
                    }
                    catch (...)
                    {
                        initializer = llvm::Constant::getNullValue(llvmType);
                    }
                }
                else
                {
                    initializer = llvm::Constant::getNullValue(llvmType);
                }

                // إنشاء المتغير العام
                // Create global variable
                // Source: cg_.module_ is defined at llvm_codegen.h:634
                // (AR) الربط الداخلي يمنع تضارب الرموز المتكررة بين ملفات LLVM المدمجة.
                // (EN) Internal linkage prevents duplicate symbol conflicts across merged LLVM units.
                // (AR) إذا كان ثابتاً بدون قيمة أولية حرفية، ننشئه كـ global (قابل للكتابة)
                //      حتى يتمكن __sad_main من كتابة القيمة المحسوبة عبر STORE.
                //      الثبات يُفرض على مستوى اللغة (المحلل النحوي) لا على مستوى LLVM.
                // (EN) If constant but no literal initializer, create as global (mutable)
                //      so __sad_main can write the computed value via STORE.
                //      Constness is enforced at language level (parser), not LLVM level.
                bool llvmConstant = isConstant && !globalVar->initialValue.empty();
                // (AR) اللبنة 3.14: @رمز("اسم") ⇒ رمز رابط ثابت مُصدَّر (ExternalLinkage)
                //      باسم linkName بدل الاسم الداخليّ المُشوَّه. مفتاح خريطة القيَم
                //      يبقى الاسم العربيّ (varName) كي تُحلّ مراجع ص الداخليّة.
                const bool hasLinkSym = !globalVar->linkName.empty();
                // (AR) اللبنة 3.14: احرس تصادم رمز @رمز المُصدَّر (SEM022): تكرار
                //      الاسم أو مصادفة رمز محجوز لزمن التشغيل ⇒ الرابط يعيد التسمية
                //      صامتًا فيفقد غرضه أو يُفسد نداءات memset/malloc… رفعُ خطأ مفهوم.
                // (AR) مصدرا التصادم اللذان تحرسهما القائمة:
                //   (أ) رمز يُصدر له المترجم تعريفًا **ضعيفًا** — لكن فقط تحت
                //       cg_.freestanding_ (emitFreestandingRuntime أعلاه): @رمز
                //       عليه = تعريف ثالث ⇒ تصادم فالرابط يختار عشوائيًّا.
                //   (ب) رمز منصّة توفّره **libc** في الوضع المستضاف (memset،
                //       __stack_chk_guard…): @رمز قويّ عليه يقنّع رمز libc — إمّا
                //       خطأ ربط، أو تقنيع **صامت** لكوكي SSP بقيمة ثابتة (تعطيل
                //       حماية تحطّم المكدّس). لذا يبقى الحجب في المستضاف.
                // (AR) kReservedRt: محجوزة **دائمًا** (المصدر (أ) في الحرّ أو (ب)
                //      في المستضاف، وكلاهما لبقيّة القائمة). __stack_chk_fail هنا
                //      دالّة زمن تشغيل ⇒ ربط **بيانة** باسمها غير مشروع أبدًا
                //      (SSP ينادي دالّة)، فيبقى محجوزًا للمتغيّرات في كِلا الوضعين
                //      (مسار @رمز على الدوالّ منفصل لا يستشير هذه القائمة ⇒ هجرة
                //      __stack_chk_fail دالّةً في أخ.3 غير متأثّرة).
                // (AR) kHostedOnlyReserved: بيانات منصّة يوفّرها المصدر (ب) فقط
                //      (لا احتياطيّ ضعيف من المترجم أصلًا) ⇒ تُحجَب في المستضاف
                //      وتُرخى في **الوضع الحرّ** حيث لا مصدر آخر فيصير @رمز التعريف
                //      الوحيد (RFC إرخاء الحارس؛ يُمكِّن هجرة __stack_chk_guard إلى ص).
                //      التكرار الفعليّ يبقى محروسًا أدناه (getNamedValue) وبالرابط.
                if (hasLinkSym)
                {
                    static const std::set<std::string> kReservedRt = {
                        "memset", "memcpy", "memmove", "memcmp", "strlen",
                        "malloc", "free", "realloc",
                        "__sad_heap", "__sad_heap_offset", "__sad_main",
                        "__divdi3", "__udivdi3", "__moddi3", "__umoddi3",
                        "__stack_chk_fail"};
                    static const std::set<std::string> kHostedOnlyReserved = {
                        "__stack_chk_guard"};
                    const std::string &sym = globalVar->getLinkName();
                    const bool reserved = kReservedRt.count(sym) ||
                        (!cg_.freestanding_ && kHostedOnlyReserved.count(sym));
                    if (reserved)
                    {
                        cg_.reportError(::Sad::Errors::ErrorCode::SEM_SYMBOL_NAME_CONFLICT,
                            {{"detail", "@رمز(\"" + sym + "\"): اسم محجوز لزمن التشغيل المضمَّن — اختر اسمًا آخر"}});
                        continue;
                    }
                    if (cg_.module_->getNamedValue(sym))
                    {
                        cg_.reportError(::Sad::Errors::ErrorCode::SEM_SYMBOL_NAME_CONFLICT,
                            {{"detail", "@رمز(\"" + sym + "\"): رمز مُصدَّر بهذا الاسم موجود مسبقًا (تكرار)"}});
                        continue;
                    }
                }
                // (AR) اللبنة 3.14: بيانات مضمَّنة بايتات(...) ⇒ ConstantDataArray في
                //      .rodata (isConstant) برمز @رمز المُصدَّر. محاذاة 1 (بيانات جدول).
                if (globalVar->isByteBlob)
                {
                    auto *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
                    auto *arrTy = llvm::ArrayType::get(i8Ty, globalVar->byteData.size());
                    llvm::Constant *blobInit = llvm::ConstantDataArray::get(
                        *cg_.context_,
                        llvm::ArrayRef<uint8_t>(globalVar->byteData.data(),
                                                globalVar->byteData.size()));
                    auto *blobGV = new llvm::GlobalVariable(
                        *cg_.module_, arrTy, /*isConstant*/ isConstant,
                        hasLinkSym ? llvm::GlobalValue::ExternalLinkage
                                   : llvm::GlobalValue::InternalLinkage,
                        blobInit,
                        hasLinkSym ? globalVar->getLinkName() : varName);
                    blobGV->setAlignment(llvm::Align(1));
                    cg_.context_info_.namedValues[varName] = blobGV;
                    cg_.context_info_.globalValues[varName] = blobGV;
                    continue;
                }
                // (AR) اللبنة 3.16: مصفوفة تخزين ساكن مصفَّرة ⇒ [N x i8] zeroinitializer
                //      في .bss. قابلة للكتابة (لا isConstant)، محاذاة 16 (تسمح وصولًا
                //      i32/i64 مُحاذًى)، رمز @رمز مُصدَّر. نهلة تعنونها بـعنوان_رمز+اكتب_ذاكرة32.
                if (globalVar->isZeroArray)
                {
                    auto *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
                    auto *arrTy = llvm::ArrayType::get(i8Ty, globalVar->zeroArrayCount);
                    auto *zeroInit = llvm::ConstantAggregateZero::get(arrTy);
                    auto *zaGV = new llvm::GlobalVariable(
                        *cg_.module_, arrTy, /*isConstant*/ false,
                        hasLinkSym ? llvm::GlobalValue::ExternalLinkage
                                   : llvm::GlobalValue::InternalLinkage,
                        zeroInit,
                        hasLinkSym ? globalVar->getLinkName() : varName);
                    zaGV->setAlignment(llvm::Align(16));
                    // (AR) متطاير على المصفوفة الساكنة: سجّلها للتمريرة اللاحقة (نظير القياسيّ)
                    if (globalVar->isVolatile)
                    {
                        cg_.context_info_.volatileGlobals.insert(varName);
                        cg_.context_info_.volatileGlobalVars.insert(zaGV);
                    }
                    cg_.context_info_.namedValues[varName] = zaGV;
                    cg_.context_info_.globalValues[varName] = zaGV;
                    continue;
                }
                auto *globalLLVM = new llvm::GlobalVariable(
                    *cg_.module_,
                    llvmType,
                    llvmConstant,
                    hasLinkSym ? llvm::GlobalValue::ExternalLinkage
                               : llvm::GlobalValue::InternalLinkage,
                    initializer,
                    hasLinkSym ? globalVar->getLinkName() : varName);
                // (AR) @متطاير: سجّل المخزن (اسمًا ومؤشّرًا) كي تعلّمه تمريرة لاحقة
                //      volatile على كلّ load/store يمسّه (GlobalVariable لا يحمل السمة).
                if (globalVar->isVolatile)
                {
                    cg_.context_info_.volatileGlobals.insert(varName);
                    cg_.context_info_.volatileGlobalVars.insert(globalLLVM);
                }

                // حفظ في السياق
                // Save to context
                // Source: cg_.context_info_ is defined at llvm_codegen.h:643
                // Source: CodeGenContext::namedValues is at llvm_codegen.h:617
                cg_.context_info_.namedValues[varName] = globalLLVM;
                // (AR) حفظ في الخريطة الدائمة للمتغيرات العالمية (لا تُمسح عند دخول الدوال)
                // (EN) Save in persistent global variables map (not cleared on function entry)
                cg_.context_info_.globalValues[varName] = globalLLVM;
            }
        }

        /**
         * إصدار الثوابت
         * Emit constants
         *
         * Source: llvm_codegen.h:335
         * @param sirModule وحدة SIR / SIR module
         */

        // ============================================================================
        // OOP vtable & Virtual Dispatch / جدول الدوال الافتراضية
        // ============================================================================

        /**
         * (AR) بناء vtable لكل صنف يحتوي على دوال
         * (EN) Build vtable for each class that has methods
         *
         * التصميم:
         *   - vtable = مصفوفة ثابتة من مؤشرات الدوال [ptr, ptr, ...]
         *   - الصنف الابن يرث vtable الأب وينسخها مع استبدال الدوال المُعاد تعريفها
         *   - الحقل 0 في كل كائن يُشير إلى vtable الخاص بصنفه الحقيقي
         */
        void ClassesVtablesCodeGen::buildClassVtables(std::shared_ptr<SIRModule> sirModule)
        {
            if (!sirModule)
                return;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // (AR) ترتيب الأصناف: الأب قبل الابن (نفس الترتيب في preprocessClasses)
            // (EN) Process in topo order: parents before children
            const auto &allClasses = sirModule->getClasses();
            std::vector<std::shared_ptr<SIRClass>> sorted;
            std::unordered_set<std::string> done;

            std::function<void(const std::shared_ptr<SIRClass> &)> topoSort;
            topoSort = [&](const std::shared_ptr<SIRClass> &cls)
            {
                if (!cls || done.count(cls->name))
                    return;
                if (!cls->parentClass.empty() && !done.count(cls->parentClass))
                {
                    for (const auto &other : allClasses)
                    {
                        if (other && other->name == cls->parentClass)
                        {
                            topoSort(other);
                            break;
                        }
                    }
                }
                done.insert(cls->name);
                sorted.push_back(cls);
            };
            for (const auto &cls : allClasses)
                topoSort(cls);

            for (const auto &cls : sorted)
            {
                if (!cls)
                    continue;
                const std::string &className = cls->name;

                // (AR) RFC #53 F2-ب: بنية @تمثيل_سي بلا ترويسة vtable في الحقل 0 (الحقل 0 بيانات
                //      المستخدم). لو بُني لها vtable وسُجّل في classVtableLayout، لأصبح استدعاء
                //      طريقةٍ عليها إرسالًا افتراضيًّا يقرأ الحقل 0 كمؤشّر vtable ⇒ انهيار/تنفيذ
                //      عشوائيّ (مراجعة أميليا C-1). البنى غير موروثة ⇒ طرقها غير افتراضيّة أصلًا،
                //      فتخطّي الـvtable يجعل الاستدعاء مباشرًا (className.method) بإزاحات حقول
                //      صحيحة عبر getFieldStructIndex. الهدم يُستدعى بالاسم لا عبر vtable.
                // (EN) RFC #53 F2-ب: a @تمثيل_سي struct has no vtable header (field 0 is user data).
                //      Building a vtable and registering it in classVtableLayout would turn a
                //      method call into a virtual dispatch that reads field 0 as a vtable pointer
                //      ⇒ crash/UB (Amelia review C-1). Structs don't inherit ⇒ their methods are
                //      never virtual, so skipping the vtable makes calls direct (className.method)
                //      with correct field offsets via getFieldStructIndex. Destructors dispatch by
                //      name, not through the vtable.
                if (cg_.context_info_.cReprClasses.count(className))
                {
                    // (AR) سجّل الهدم إن وُجد (يُستدعى بالاسم) دون بناء vtable
                    // (EN) Still register a destructor (called by name) without a vtable
                    for (const auto &[methodName, methodFunc] : cls->methods_)
                    {
                        std::string shortName = methodName;
                        size_t dp = methodName.find('.');
                        if (dp != std::string::npos)
                            shortName = methodName.substr(dp + 1);
                        if (shortName == "\xD9\x87\xD8\xAF\xD9\x85" || shortName == "__del__" ||
                            shortName == "__destroy__")
                        {
                            cg_.context_info_.classDestructors[className] =
                                (methodName.find('.') != std::string::npos) ? methodName
                                                                            : (className + "." + methodName);
                        }
                    }
                    continue;
                }

                // (AR) بناء تخطيط vtable: ابدأ من الأب
                // (EN) Build vtable layout: start from parent
                std::vector<std::string> vtableSlots;
                if (!cls->parentClass.empty())
                {
                    auto parentIt = cg_.context_info_.classVtableLayout.find(cls->parentClass);
                    if (parentIt != cg_.context_info_.classVtableLayout.end())
                    {
                        vtableSlots = parentIt->second; // نسخ vtable الأب
                    }
                }

                // (AR) إضافة/استبدال دوال الصنف الحالي
                // (EN) Add/override methods from current class
                for (const auto &[methodName, methodFunc] : cls->methods_)
                {
                    // (AR) methodName هو الاسم الكامل مثل "صنف.دالة" — نستخرج الاسم القصير
                    // (EN) methodName is already fully qualified like "class.method" — extract short name
                    std::string shortMethodName = methodName;
                    std::string fullName = methodName; // already "className.method"
                    size_t dotPos = methodName.find('.');
                    if (dotPos != std::string::npos)
                    {
                        shortMethodName = methodName.substr(dotPos + 1);
                    }
                    else
                    {
                        // (AR) إذا لم يكن مؤهلاً، أضف اسم الصنف
                        // (EN) If not qualified, prepend class name
                        fullName = className + "." + methodName;
                    }

                    // تجاهل الباني — ليس في vtable
                    if (shortMethodName == "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1" || shortMethodName == "بناء" ||
                        shortMethodName == "__init__" || shortMethodName == "init" ||
                        shortMethodName == "\xD8\xA8\xD8\xA7\xD9\x86\xD9\x8A" || shortMethodName == "باني" ||
                        shortMethodName == "\xD9\x85\xD9\x86\xD8\xB4\xD8\xA6" || shortMethodName == "منشئ")
                    {
                        continue;
                    }

                    // تجاهل الهدم — يُعالج بشكل منفصل
                    if (shortMethodName == "\xD9\x87\xD8\xAF\xD9\x85" || shortMethodName == "هدم" ||
                        shortMethodName == "__del__" || shortMethodName == "__destroy__")
                    {
                        // تسجيل الهدم
                        cg_.context_info_.classDestructors[className] = fullName;
                        continue;
                    }

                    // هل هذه الدالة تعيد تعريف دالة من الأب؟
                    bool overridden = false;
                    for (size_t i = 0; i < vtableSlots.size(); i++)
                    {
                        // استخراج اسم الدالة من الاسم الكامل "صنف.دالة"
                        size_t dot = vtableSlots[i].rfind('.');
                        std::string slotMethodName = (dot != std::string::npos)
                                                         ? vtableSlots[i].substr(dot + 1)
                                                         : vtableSlots[i];
                        if (slotMethodName == shortMethodName)
                        {
                            vtableSlots[i] = fullName; // استبدال بتنفيذ الابن
                            overridden = true;
                            break;
                        }
                    }
                    if (!overridden)
                    {
                        vtableSlots.push_back(fullName); // إضافة دالة جديدة
                    }
                }

                // تسجيل التخطيط
                cg_.context_info_.classVtableLayout[className] = vtableSlots;

                // (AR) إنشاء متغير عام ثابت لـ vtable
                // (EN) Create constant global for vtable
                if (!vtableSlots.empty())
                {
                    auto vtableArrayTy = llvm::ArrayType::get(ptrTy, vtableSlots.size());

                    std::vector<llvm::Constant *> vtableEntries;
                    for (const auto &fullMethodName : vtableSlots)
                    {
                        llvm::Function *fn = cg_.module_->getFunction(fullMethodName);
                        if (fn)
                        {
                            vtableEntries.push_back(fn);
                        }
                        else
                        {
                            // الدالة لم تُعرّف بعد — سيتم ربطها لاحقاً (null مؤقتاً)
                            vtableEntries.push_back(llvm::ConstantPointerNull::get(ptrTy));
                        }
                    }

                    auto vtableInit = llvm::ConstantArray::get(vtableArrayTy, vtableEntries);
                    auto vtableGlobal = new llvm::GlobalVariable(
                        *cg_.module_, vtableArrayTy, true, // isConstant=true
                        llvm::GlobalValue::PrivateLinkage, vtableInit,
                        "vtable." + className);

                    cg_.context_info_.classVtableGlobals[className] = vtableGlobal;

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildVtable: '" << className
                              << "' with " << vtableSlots.size() << " slots" << std::endl;
#endif
                }
            }
        }

        /**
         * (AR) تحديث مداخل vtable المؤجلة بعد إصدار جميع الدوال.
         * (EN) Patch deferred vtable entries after all functions have been emitted.
         */
        void ClassesVtablesCodeGen::patchClassVtables()
        {
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            for (const auto &[className, vtableGlobal] : cg_.context_info_.classVtableGlobals)
            {
                auto layoutIt = cg_.context_info_.classVtableLayout.find(className);
                if (layoutIt == cg_.context_info_.classVtableLayout.end() || !vtableGlobal)
                {
                    continue;
                }

                const auto &slots = layoutIt->second;
                if (slots.empty())
                {
                    continue;
                }

                std::vector<llvm::Constant *> entries;
                entries.reserve(slots.size());

                for (const auto &fullMethodName : slots)
                {
                    llvm::Function *fn = cg_.module_->getFunction(fullMethodName);
                    if (fn)
                    {
                        entries.push_back(fn);
                    }
                    else
                    {
                        // Keep null as defensive fallback, but report missing entry.
                        cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", std::string("Missing vtable method at patch time:") + fullMethodName}});
                        entries.push_back(llvm::ConstantPointerNull::get(ptrTy));
                    }
                }

                auto arrTy = llvm::ArrayType::get(ptrTy, entries.size());
                vtableGlobal->setInitializer(llvm::ConstantArray::get(arrTy, entries));
            }
        }

        /**
         * (AR) تخزين مؤشر vtable في الحقل 0 من الكائن
         * (EN) Store vtable pointer in field 0 of the object
         */
        void ClassesVtablesCodeGen::storeVtablePtr(llvm::Value *objPtr, const std::string &className)
        {
            // (AR) RFC #53 F2-ب: بنية @تمثيل_سي بلا ترويسة vtable — الحقل 0 هو أوّل حقل مستخدم،
            //      فتخزين مؤشّر vtable هنا سيطمس بياناته. نتخطّاه صراحةً (بنى البيانات بلا vtable
            //      عادةً، لكن هذا حارس متانة).
            // (EN) RFC #53 F2-ب: a @تمثيل_سي struct has no vtable header — field 0 is the first
            //      user field, so storing a vtable pointer here would clobber its data. Skip it
            //      explicitly (data structs usually have no vtable, but this is a robustness guard).
            if (cg_.context_info_.cReprClasses.count(className))
                return;

            auto vtableIt = cg_.context_info_.classVtableGlobals.find(className);
            if (vtableIt == cg_.context_info_.classVtableGlobals.end())
                return; // لا vtable

            auto structIt = cg_.context_info_.classStructTypes.find(className);
            if (structIt == cg_.context_info_.classStructTypes.end())
                return;

            llvm::StructType *structType = structIt->second;

            // GEP إلى الحقل 0 (مؤشر vtable)
            llvm::Value *vtablePtrSlot = cg_.builder_->CreateStructGEP(structType, objPtr, 0, "vtable.slot");

            // تحويل المصفوفة العامة إلى مؤشر (decay)
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::Value *vtablePtr = cg_.builder_->CreateBitCast(vtableIt->second, ptrTy, "vtable.ptr");

            cg_.builder_->CreateStore(vtablePtr, vtablePtrSlot);
        }

        /**
         * (AR) الاستدعاء الافتراضي: تحميل مؤشر الدالة من vtable + استدعاء غير مباشر
         * (EN) Virtual dispatch: load function pointer from vtable + indirect call
         */
        llvm::Value *ClassesVtablesCodeGen::emitVirtualCall(llvm::Value *objPtr, const std::string &className,
                                                  const std::string &methodName,
                                                  const std::vector<llvm::Value *> &extraArgs)
        {
            // (AR) البحث عن رقم الفتحة (slot) في vtable
            // (EN) Find the vtable slot index
            auto layoutIt = cg_.context_info_.classVtableLayout.find(className);
            if (layoutIt == cg_.context_info_.classVtableLayout.end())
                return nullptr;

            const auto &layout = layoutIt->second;
            int slotIndex = -1;
            for (size_t i = 0; i < layout.size(); i++)
            {
                size_t dot = layout[i].rfind('.');
                std::string slotMethodName = (dot != std::string::npos)
                                                 ? layout[i].substr(dot + 1)
                                                 : layout[i];
                if (slotMethodName == methodName)
                {
                    slotIndex = static_cast<int>(i);
                    break;
                }
            }

            if (slotIndex < 0)
                return nullptr; // ليست في vtable

            auto structIt = cg_.context_info_.classStructTypes.find(className);
            if (structIt == cg_.context_info_.classStructTypes.end())
                return nullptr;

            llvm::StructType *structType = structIt->second;
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // (AR) الخطوة 1: تحميل مؤشر vtable من الحقل 0
            // (EN) Step 1: Load vtable pointer from field 0
            llvm::Value *vtableSlotAddr = cg_.builder_->CreateStructGEP(structType, objPtr, 0, "vtable.addr");
            llvm::Value *vtablePtr = cg_.builder_->CreateLoad(ptrTy, vtableSlotAddr, "vtable.load");

            // (AR) الخطوة 2: حساب عنوان الفتحة في vtable
            // (EN) Step 2: GEP into vtable array at slot index
            llvm::Value *slotAddr = cg_.builder_->CreateGEP(
                ptrTy, vtablePtr,
                {llvm::ConstantInt::get(cg_.getInt64Type(), slotIndex)},
                "vslot.addr");

            // (AR) الخطوة 3: تحميل مؤشر الدالة
            // (EN) Step 3: Load function pointer
            llvm::Value *fnPtr = cg_.builder_->CreateLoad(ptrTy, slotAddr, "vfn.ptr");

            // (AR) الخطوة 4: بناء المعاملات (self + extra)
            // (EN) Step 4: Build args (self + extra)
            std::vector<llvm::Value *> callArgs = {objPtr};
            callArgs.insert(callArgs.end(), extraArgs.begin(), extraArgs.end());

            // (AR) الخطوة 5: بناء نوع الدالة — نحتاج لمعرفة التوقيع
            // (EN) Step 5: Build function type — need to know the signature
            // البحث عن الدالة الأصلية لمعرفة نوعها
            std::string origFuncName = layout[slotIndex];
            llvm::Function *origFunc = cg_.module_->getFunction(origFuncName);

            llvm::FunctionType *fnType = nullptr;
            if (origFunc)
            {
                fnType = origFunc->getFunctionType();
            }
            else
            {
                // احتياطي: افتراض توقيع (ptr, ...) → i64
                std::vector<llvm::Type *> argTypes;
                for (auto *a : callArgs)
                    argTypes.push_back(a->getType());
                fnType = llvm::FunctionType::get(cg_.getInt64Type(), argTypes, false);
            }

            // (AR) الخطوة 6: تحويل أنواع الوسائط لتتطابق مع توقيع الدالة
            // (EN) Step 6: Coerce argument types to match function signature
            unsigned numParams = fnType->getNumParams();
            for (size_t i = 0; i < callArgs.size() && i < numParams; i++)
            {
                llvm::Type *expected = fnType->getParamType(i);
                if (callArgs[i]->getType() != expected)
                {
                    if (expected->isIntegerTy(64) && callArgs[i]->getType()->isPointerTy())
                    {
                        callArgs[i] = cg_.builder_->CreatePtrToInt(callArgs[i], expected, "varg.p2i");
                    }
                    else if (expected->isPointerTy() && callArgs[i]->getType()->isIntegerTy(64))
                    {
                        callArgs[i] = cg_.builder_->CreateIntToPtr(callArgs[i], expected, "varg.i2p");
                    }
                    else if (expected->isIntegerTy(64) && callArgs[i]->getType()->isIntegerTy(1))
                    {
                        callArgs[i] = cg_.builder_->CreateZExt(callArgs[i], expected, "varg.zext");
                    }
                }
            }

            // (AR) الخطوة 7: الاستدعاء غير المباشر
            // (EN) Step 7: Indirect call
            llvm::Value *result = cg_.builder_->CreateCall(fnType, fnPtr, callArgs,
                                                       fnType->getReturnType()->isVoidTy() ? "" : (methodName + "_virt"));

            return result;
        }

        /**
         * (AR) استدعاء دالة الهدم للكائن (إن وُجدت)
         * (EN) Call destructor for object (if exists)
         */
        void ClassesVtablesCodeGen::emitDestructorCall(llvm::Value *objPtr, const std::string &className)
        {
            // البحث في سلسلة الوراثة عن أول هدم
            std::string searchClass = className;
            while (!searchClass.empty())
            {
                auto dtorIt = cg_.context_info_.classDestructors.find(searchClass);
                if (dtorIt != cg_.context_info_.classDestructors.end())
                {
                    llvm::Function *dtorFunc = cg_.module_->getFunction(dtorIt->second);
                    if (dtorFunc)
                    {
                        cg_.builder_->CreateCall(dtorFunc, {objPtr});
                    }
                    return;
                }
                auto parentIt = cg_.context_info_.classParentMap.find(searchClass);
                if (parentIt != cg_.context_info_.classParentMap.end())
                {
                    searchClass = parentIt->second;
                }
                else
                {
                    break;
                }
            }
        }

        /**
         * (AR) حساب فهرس الحقل الحقيقي في الهيكل مع إزاحة vtable
         * (EN) Calculate actual field struct index with vtable offset
         *
         * الحقل 0 في الهيكل → مؤشر vtable (محجوز)
         * الحقل 1 في الهيكل → أول حقل للمستخدم (الفهرس 0 في classFieldNames)
         */
        int ClassesVtablesCodeGen::getFieldStructIndex(const std::string &className, int userFieldIndex) const
        {
            // (AR) RFC #53 F2-ب: بنية @تمثيل_سي بلا ترويسة vtable ⇒ الحقل 0 هو أوّل حقل مستخدم،
            //      فلا إزاحة. غيرها: الحقل 0 مؤشر vtable ⇒ +1.
            // (EN) RFC #53 F2-ب: a @تمثيل_سي struct has no vtable header ⇒ field 0 is the first
            //      user field, no offset. Otherwise field 0 is the vtable pointer ⇒ +1.
            if (cg_.context_info_.cReprClasses.count(className))
            {
                return userFieldIndex;
            }
            // كل صنف يملك vtable pointer في الحقل 0
            return userFieldIndex + 1;
        }

        // ====================================================================
        // (AR) [RFC #53 F2-ج] تصنيف ABI لتمرير/إرجاع بنية @تمثيل_سي بالقيمة.
        //      يعمل على نوع LLVM المسجَّل (بلا ترويسة، من F2-ب) والـDataLayout
        //      والثالوث — بلا حاجة للوصول إلى SIR. يغطّي الحقول العدديّة
        //      (صحيح/عشريّ/مؤشّر). القرار مطابقٌ لِما يولّده Clang لهدفَي
        //      System V AMD64 وWindows x64.
        // (EN) [RFC #53 F2-ج] ABI classification for by-value pass/return of a
        //      @تمثيل_سي struct. Operates on the registered (header-less, F2-ب)
        //      LLVM type + DataLayout + triple — no SIR access. Covers scalar
        //      fields (int/float/pointer). Matches Clang's lowering for System V
        //      AMD64 and Windows x64.
        // ====================================================================
        CReprAbiInfo ClassesVtablesCodeGen::classifyCReprAbi(const std::string &className) const
        {
            CReprAbiInfo info; // kind = NotCRepr افتراضيًّا

            // (AR) ليست بنية @تمثيل_سي مسجَّلة ⇒ لا تصنيف (المستدعي يُبقيها مؤشّرًا).
            // (EN) Not a registered C-repr struct ⇒ no classification (caller keeps it a pointer).
            if (!cg_.context_info_.cReprClasses.count(className))
                return info;

            auto stIt = cg_.context_info_.classStructTypes.find(className);
            if (stIt == cg_.context_info_.classStructTypes.end() || !stIt->second)
                return info;

            llvm::StructType *st = stIt->second;
            const llvm::DataLayout &DL = cg_.module_->getDataLayout();
            llvm::LLVMContext &ctx = *cg_.context_;
            const uint64_t size = DL.getTypeAllocSize(st);
            info.sizeBytes = size;

            const bool isWin64 = llvm::Triple(cg_.module_->getTargetTriple()).isOSWindows();

            // (AR) بنية فارغة (بلا حقول): لا شيء يُمرَّر — عاملها بمؤشّرٍ آمن (نادرة).
            // (EN) Empty struct (no fields): nothing to pass — treat as Memory (rare).
            if (size == 0)
            {
                info.kind = CReprAbiKind::Memory;
                return info;
            }

            if (isWin64)
            {
                // (AR) Win64: بنية بحجم 1/2/4/8 ⇒ سجلّ صحيح واحد بذلك الحجم؛ غيرها ⇒ بمؤشّر خفيّ.
                // (EN) Win64: struct of size 1/2/4/8 ⇒ one integer register of that size; else hidden pointer.
                if (size == 1 || size == 2 || size == 4 || size == 8)
                {
                    info.kind = CReprAbiKind::Direct;
                    info.pieces.push_back(llvm::Type::getIntNTy(ctx, static_cast<unsigned>(size * 8)));
                }
                else
                {
                    info.kind = CReprAbiKind::Memory;
                }
                return info;
            }

            // (AR) System V AMD64: >16 بايت ⇒ ذاكرة؛ وإلّا ثمانيّة/ثمانيّتان تُصنَّف كلٌّ INTEGER أو SSE.
            // (EN) System V AMD64: >16 bytes ⇒ memory; else 1–2 eightbytes each classified INTEGER or SSE.
            if (size > 16)
            {
                info.kind = CReprAbiKind::Memory;
                return info;
            }

            info.kind = CReprAbiKind::Direct;
            const llvm::StructLayout *SL = DL.getStructLayout(st);
            // (AR) ثمانيّةٌ تُصنَّف SSE فقط إن كانت كلّ الحقول المتراكبة عليها عشريّة (قاعدة SysV).
            // (EN) An eightbyte is SSE only if every field overlapping it is floating-point (SysV rule).
            auto classifyEightbyte = [&](uint64_t offStart, uint64_t nBytes) -> llvm::Type *
            {
                bool allFloat = true;
                for (unsigned i = 0; i < st->getNumElements(); ++i)
                {
                    uint64_t eOff = SL->getElementOffset(i);
                    uint64_t eSize = DL.getTypeAllocSize(st->getElementType(i));
                    bool overlaps = (eOff < offStart + nBytes) && (eOff + eSize > offStart);
                    if (overlaps && !st->getElementType(i)->isFloatingPointTy())
                        allFloat = false;
                }
                if (allFloat)
                    return (nBytes <= 4) ? llvm::Type::getFloatTy(ctx) : llvm::Type::getDoubleTy(ctx);
                return llvm::Type::getIntNTy(ctx, static_cast<unsigned>(nBytes * 8));
            };

            uint64_t firstBytes = (size < 8) ? size : 8;
            info.pieces.push_back(classifyEightbyte(0, firstBytes));
            if (size > 8)
                info.pieces.push_back(classifyEightbyte(8, size - 8));
            return info;
        }

    } // namespace LLVM
} // namespace Sad
