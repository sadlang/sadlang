/*
 * ============================================================================
 * مولد كود LLVM IR - رأس الملف
 * LLVM IR Code Generator - Header File
 * ============================================================================
 *
 * الوصف (Description):
 *   هذا الملف يحتوي على مولد كود LLVM IR الذي يترجم SIR (Sad Intermediate
 *   Representation) إلى LLVM IR. يوفر تحويل الأنواع، إصدار الدوال، ترجمة
 *   التعليمات، ومعالجة تدفق التحكم.
 *
 *   This file contains the LLVM IR code generator that translates SIR (Sad
 *   Intermediate Representation) to LLVM IR. Provides type conversion,
 *   function emission, instruction translation, and control flow handling.
 *
 * المكونات الرئيسية (Main Components):
 *   - LLVMCodeGen: الفئة الرئيسية لتوليد LLVM IR
 *   - LLVMCodeGen: Main class for LLVM IR generation
 *
 *   - Type Mapping: تحويل أنواع Sad إلى أنواع LLVM
 *   - Type Mapping: Convert Sad types to LLVM types
 *
 *   - Function Emission: إصدار دوال LLVM من SIR functions
 *   - Function Emission: Emit LLVM functions from SIR functions
 *
 *   - Instruction Translation: ترجمة تعليمات SIR إلى LLVM IR
 *   - Instruction Translation: Translate SIR instructions to LLVM IR
 *
 *   - Control Flow: معالجة القفزات والفروع والاستدعاءات
 *   - Control Flow: Handle jumps, branches, and calls
 *
 * الاستخدام (Usage):
 *   ```cpp
 *   LLVMCodeGen codegen;
 *   codegen.initialize("my_module", "x86_64-linux-gnu");
 *   auto llvm_module = codegen.generate(sir_module);
 *   codegen.emitToFile("output.ll");
 *   ```
 *
 * التبعيات (Dependencies):
 *   - LLVM Core Libraries
 *   - SIR (Sad IR) definitions
 *   - Type system
 *
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#ifndef SAD_LLVM_CODEGEN_H
#define SAD_LLVM_CODEGEN_H

#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// LLVM Headers
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Triple.h>

// Sad LLVM Components (مكونات Sad LLVM)
#include "llvm_type_mapper.h"
#include "llvm_optimizer.h"                              // إضافة محسّن LLVM / Add LLVM optimizer
#include "llvm_codegen_context.h"                        // (AR) قاعدة الحالة (Phase 7 Step 0) / (EN) State base
#include "builders/arithmetic/arithmetic_codegen.h"      // (AR) Phase 7 Step 1: ArithmeticCodeGen
#include "builders/memory/memory_codegen.h"              // (AR) Phase 7 Step 2: MemoryCodeGen
#include "builders/arithmetic/controlflow_codegen.h"     // (AR) Phase 7 Step 3: ControlFlowCodeGen
#include "builders/memory/aggregate_ops_codegen.h"       // (AR) Phase 7 Step 4: AggregateOpsCodeGen
#include "builders/collections/array_ops_codegen.h"      // (AR) Phase 7 Step 5: ArrayOpsCodeGen
#include "builders/collections/string_ops_codegen.h"     // (AR) Phase 7 Step 6: StringOpsCodeGen
#include "builders/collections/array_builtins_codegen.h" // (AR) Phase 7 Step 7: ArrayBuiltinsCodeGen
#include "builders/builtins/math_builtins_codegen.h"     // (AR) Phase 7 Step 8: MathBuiltinsCodeGen
#include "builders/collections/map_ops_codegen.h"        // (AR) Phase 7 Step 9: MapOpsCodeGen
#include "builders/arithmetic/exception_codegen.h"       // (AR) Phase 7 Step 10: ExceptionCodeGen
#include "builders/platform/lowlevel_codegen.h"          // (AR) Phase 7 Step 11: LowlevelCodeGen
#include "builders/oop/enum_ops_codegen.h"               // (AR) Phase 7 Step 12: EnumOpsCodeGen
#include "builders/platform/hardware_ffi_codegen.h"      // (AR) Phase 7 Step 13: HardwareFFICodeGen
#include "builders/memory/closure_codegen.h"             // (AR) Phase 7 Step 14: ClosureCodeGen
#include "builders/builtins/io_builtins_codegen.h"       // (AR) Phase 7 Step 15: IOBuiltinsCodeGen
#include "builders/builtins/security_builtins_codegen.h" // (AR) Phase 7 Step 15: SecurityBuiltinsCodeGen
#include "builders/platform/ffi_remain_codegen.h"        // (AR) Phase 7 Step 15: FFIRemainCodeGen
#include "builders/core/freestanding_codegen.h"          // (AR) Phase 7 Step 16: FreestandingCodeGen
#include "builders/oop/objects_arrays_codegen.h"         // (AR) Phase 7 Step 17: ObjectsArraysCodeGen
#include "builders/oop/oop_ops_codegen.h"                // (AR) Phase 7 Step 18: OOPOpsCodeGen
#include "builders/directives/concurrency_codegen.h"     // (AR) Phase 8 Step 1: ConcurrencyCodeGen
#include "builders/platform/ui_codegen.h"                // (AR) Phase 8 Step 2: UICodeGen
#include "builders/oop/classes_vtables_codegen.h"        // (AR) Phase 8 Step 3: ClassesVtablesCodeGen
#include "builders/oop/functions_codegen.h"              // (AR) Phase 8 Step 4: FunctionsCodeGen
#include "builders/builtins/builtin_funcs_codegen.h"     // (AR) Phase 8 Step 5: BuiltinFuncsCodeGen
#include "builders/builtins/network_builtins_codegen.h"  // (AR) Phase 8 Step 6: NetworkBuiltinsCodeGen
#include "builders/builtins/processes_builtins_codegen.h" // (AR) وحدة عمليات — بدائيّات التشغيل
#include "builders/directives/coroutines_codegen.h"      // (AR) Phase 8 Step 7: CoroutinesCodeGen
#include "builders/collections/strings_codegen.h"        // (AR) Phase 8 Step 8: StringsCodeGen
#include "builders/core/instr_core_codegen.h"            // (AR) Phase 8 Step 9: InstrCoreCodeGen
#include "builders/directives/simd_codegen.h"            // (AR) Phase 8 Step 10
#include "builders/platform/instr_lowlevel_codegen.h"    // (AR) Phase 8 Step 10
#include "builders/platform/file_casts_codegen.h"        // (AR) Phase 8 Step 10
#include "builders/directives/directives_codegen.h"      // (AR) Phase 8 Step 10
#include "builders/platform/instr_platform_codegen.h"    // (AR) Phase 8 Step 10
#include "builders/core/output_codegen.h"                // (AR) Phase 8 Step 10
#include "builders/core/types_codegen.h"                 // (AR) Phase 9: TypesCodeGen

// Sad SIR Components (مكونات Sad SIR)
// Source: compiler/frontend/include/sir_*.h - مضاف في CMake include_directories line 27
#include "sir_module.h"      // Source: compiler/frontend/include/sir_module.h
#include "sir_instruction.h" // Source: compiler/frontend/include/sir_instruction.h
#include "sir_types.h"       // Source: compiler/frontend/include/sir_types.h

namespace Sad
{
    namespace LLVM
    {

        // ============================================================================
        // Forward Declarations / التصريحات المسبقة
        // ============================================================================

        // Use SIR types from Compiler namespace
        // استخدام أنواع SIR من namespace Compiler
        using SIRModule = Compiler::SIR::SIRModule;
        using SIRFunction = Compiler::SIR::SIRFunction;
        using SIRBasicBlock = Compiler::SIR::SIRBasicBlock;
        using SIRInstruction = Compiler::SIR::SIRInstruction;
        using SadTypeKind = Compiler::SIR::SadTypeKind;
        using SIROperand = Compiler::SIR::SIROperand;
        using SIROperandType = Compiler::SIR::SIROperandType;

        // ============================================================================
        // CodeGenContext - سياق توليد الكود
        // CodeGenContext - Code Generation Context
        // ============================================================================

        /**
         * سياق توليد الكود يحتوي على معلومات الحالة أثناء الترجمة
         * Code generation context holds state information during translation
         */
        struct CodeGenContext
        {
            // الدالة الحالية / Current function
            llvm::Function *currentFunction = nullptr;

            // الكتلة الأساسية الحالية / Current basic block
            llvm::BasicBlock *currentBlock = nullptr;

            // جدول القيم المسماة (المتغيرات المحلية)
            // Named values table (local variables)
            std::unordered_map<std::string, llvm::Value *> namedValues;

            // (AR) جدول المتغيرات العامة — لا يُمسح عند دخول دالة جديدة
            // (EN) Global variables table — NOT cleared on function entry
            std::unordered_map<std::string, llvm::Value *> globalValues;

            // (AR) أسماء المتغيّرات العامّة المتطايرة (@متطاير، اللبنة 3.14) — تُوسَم
            //      قراءاتها/كتاباتها المباشرة volatile (GlobalVariable لا يحمل السمة)
            std::set<std::string> volatileGlobals;
            // (AR) مؤشّرات الـglobals المتطايرة — تمريرة لاحقة تعلّم كلّ load/store
            //      يمسّها volatile (متينة ضدّ تعدّد مسارات إصدار الأحمال)
            std::set<const llvm::GlobalVariable *> volatileGlobalVars;

            // جدول الدوال / Function table
            std::unordered_map<std::string, llvm::Function *> functions;

            // (AR) أنواع SIR لمعاملات كل دالة (بالترتيب، شاملةً self للطرق). يُستخدم عند
            //      الاستدعاء لتوسيم قيمةٍ محسوسة (صحيح/عشريّ/منطقيّ/نصّ) تُمرَّر لمعاملٍ
            //      ديناميّ Any، فيتطابق تمثيل المُستدعي والمُستدعاة (ISSUE-076/084).
            // (EN) Per-function SIR parameter types (in order, incl. self for methods). Used at
            //      call sites to tag a concrete value (int/float/bool/string) passed to a dynamic
            //      Any parameter, keeping caller/callee representations in agreement (ISSUE-076/084).
            std::unordered_map<std::string, std::vector<SadTypeKind>> functionParamSirTypes;

            // جدول الكتل الأساسية / Basic block table
            std::unordered_map<std::string, llvm::BasicBlock *> basicBlocks;

            // (AR) جدول «كتلة الخروج الفعليّة» لكلّ كتلة SIR: الكتلة الحيّة في LLVM بعد
            //      خفض تعليمات الكتلة. قد تُقسَّم الكتلة أثناء الخفض (فحص حدود المصفوفة
            //      يولّد set.bc.ok، تحويل %SadDyn يولّد فروعًا…)، فيصير سلفُ الدمج الفعليّ
            //      كتلةً غير التي تحمل اسم SIR. تستهلكها emitPhi لتسجيل الوارد من السلف
            //      الصحيح، وإلّا «PHI node entries do not match predecessors».
            // (EN) Per-SIR-block «effective exit block»: the live LLVM block after the
            //      block's instructions are lowered. A block may split mid-lowering (array
            //      bounds check emits set.bc.ok, %SadDyn coercion emits branches…), so the
            //      true predecessor of a merge is a block other than the SIR-named one.
            //      emitPhi consults this to record the incoming from the correct
            //      predecessor; otherwise «PHI node entries do not match predecessors».
            std::unordered_map<std::string, llvm::BasicBlock *> basicBlockExits;

            // جدول القيم الثابتة / Constant values table
            std::unordered_map<std::string, llvm::Constant *> constants;

            // خريطة تخصيص الذاكرة / Allocation map
            std::unordered_map<std::string, llvm::AllocaInst *> allocas;

            // ================================================================
            // دعم الأصناف / Class Support
            // ================================================================

            // خريطة السجل/المتغير → اسم الصنف
            // Register/variable → class name mapping
            std::unordered_map<std::string, std::string> objectClassMap;

            // (AR) تتبع الحقول التي تم الوصول إليها لكل سجل كائن — لمطابقة متعددة الحقول
            // (EN) Track fields accessed per object register — for multi-field matching
            std::unordered_map<std::string, std::unordered_set<std::string>> objectFieldsAccessed;

            // اسم الصنف → نوع الهيكل LLVM
            // Class name → LLVM struct type
            std::unordered_map<std::string, llvm::StructType *> classStructTypes;

            // اسم الصنف → أسماء الحقول بالترتيب
            // Class name → ordered field names
            std::unordered_map<std::string, std::vector<std::string>> classFieldNames;

            // (AR) الأصناف المضمَّنة لغويًّا (كصنف «حدث» من SoT) — تُستثنى من احتياطيّ
            //      «تخمين الصنف من اسم الحقل»: تسجيل «حدث» غير المشروط (② rfcs#46)
            //      جعل حقوله (س/ص/قيمة…) تفوز بالتخمين على حقول أصناف المستخدم
            //      فيُبنى GEP بتخطيط «حدث» فوق كائن المستخدم (انحدار dev الثلاثيّ).
            // (EN) Language-builtin classes (e.g. the SoT «حدث» event class) — excluded
            //      from the infer-class-from-field-name fallback: its unconditional
            //      registration made its fields (x/y/value…) win the guess over user
            //      classes, building GEPs with the event layout over user objects.
            std::unordered_set<std::string> builtinClassNames;

            // اسم الصنف → اسم الصنف الأب (للوراثة)
            // Class name → parent class name (for inheritance)
            std::unordered_map<std::string, std::string> classParentMap;

            // اسم الصنف الحالي في حالة البناء (فارغ خارج الباني)
            // Current constructor class name (empty outside constructors)
            std::string currentConstructorClass;

            // اسم الصنف الحالي في حالة الدالة (فارغ خارج دوال الصنف)
            // Current method class name (empty outside class methods)

            // ================================================================
            // دعم vtable والاستدعاء الافتراضي / vtable & Virtual Dispatch
            // ================================================================

            // اسم الصنف → متغير عام لـ vtable
            // Class name → vtable global variable
            std::unordered_map<std::string, llvm::GlobalVariable *> classVtableGlobals;

            // اسم الصنف → ترتيب الدوال في vtable (اسم الدالة الكامل)
            // Class name → ordered method names in vtable slots
            std::unordered_map<std::string, std::vector<std::string>> classVtableLayout;

            // اسم الصنف → اسم دالة الهدم
            // Class name → destructor function name
            std::unordered_map<std::string, std::string> classDestructors;

            // أسماء الأصناف المجردة (لا يمكن إنشاء كائنات منها)
            // Abstract class names (cannot be instantiated)
            std::unordered_set<std::string> abstractClasses;

            // (AR) أسماء بنى @تمثيل_سي (تخطيط C-متوافق: بلا ترويسة vtable في الحقل 0) [RFC #53 F2-ب]
            // (EN) @تمثيل_سي struct names (C-ABI layout: no vtable header at field 0) [RFC #53 F2-ب]
            std::unordered_set<std::string> cReprClasses;

            // (AR) خطط نداء الدوالّ الخارجيّة ذات معاملات/عائد بنية @تمثيل_سي بالقيمة —
            //      بمفتاح اسم دالّة ص. تُملأ في emitFunctionPrototype وتُقرأ في emitCall. [RFC #53 F2-ج]
            // (EN) Call plans for extern functions with by-value @تمثيل_سي struct params/return —
            //      keyed by ص function name. Filled in emitFunctionPrototype, read in emitCall. [RFC #53 F2-ج]
            std::unordered_map<std::string, CReprCallPlan> creprCallPlans;

            // ================================================================
            // دعم الكوروتين / Coroutine Support
            // ================================================================
            bool isCoroutineFunction = false;          ///< هل الدالة الحالية كوروتين
            bool isGeneratorFunction = false;          ///< هل الدالة الحالية مولّد
            llvm::Value *coroHandle = nullptr;         ///< مقبض الكوروتين الحالي
            llvm::Value *coroPromise = nullptr;        ///< مؤشر الوعد (promise) للقيمة المُرجعة
            llvm::Value *coroId = nullptr;             ///< token من coro.id
            llvm::BasicBlock *coroCleanupBB = nullptr; ///< كتلة تنظيف الكوروتين
            llvm::BasicBlock *coroSuspendBB = nullptr; ///< كتلة تعليق الكوروتين
            llvm::BasicBlock *coroFinalBB = nullptr;   ///< كتلة الإنهاء النهائي
            int coroSuspendCount = 0;                  ///< عداد نقاط التعليق
            std::string currentMethodClass;
        };

        // ============================================================================
        // LLVMCodeGen - مولد كود LLVM الرئيسي
        // LLVMCodeGen - Main LLVM Code Generator
        // ============================================================================

        /**
         * مولد كود LLVM يترجم SIR إلى LLVM IR
         * LLVM code generator translates SIR to LLVM IR
         *
         * الوظائف الرئيسية (Main Functions):
         *   - initialize(): تهيئة السياق والوحدة
         *   - generate(): توليد LLVM IR من SIR
         *   - emitToFile(): حفظ LLVM IR إلى ملف
         *
         * مثال (Example):
         *   ```cpp
         *   LLVMCodeGen gen;
         *   gen.initialize("program", "x86_64-linux-gnu");
         *   auto module = gen.generate(sir);
         *   gen.verify();
         *   gen.emitToFile("output.ll");
         *   ```
         */
        // (AR) LLVMCodeGen يرث من LLVMCodeGenContext (Phase 7 Step 0)
        //      الحالة المشتركة (context_, module_, builder_, ...) معرّفة في القاعدة.
        // (EN) LLVMCodeGen inherits from LLVMCodeGenContext (Phase 7 Step 0)
        //      Shared state (context_, module_, builder_, ...) defined in the base.
        class LLVMCodeGen : public LLVMCodeGenContext
        {
            // (AR) Phase 7 Step 1: ArithmeticCodeGen يصل للحقول الخاصة عبر friend
            // (EN) Phase 7 Step 1: ArithmeticCodeGen accesses private state via friend
            // (AR) Phase 7 Step 2: MemoryCodeGen
            // (AR) Phase 7 Step 3: ControlFlowCodeGen
            // (AR) Phase 7 Step 4: AggregateOpsCodeGen
            // (AR) Phase 7 Step 5: ArrayOpsCodeGen
            // (AR) Phase 7 Step 6: StringOpsCodeGen
            // (AR) Phase 7 Step 7: ArrayBuiltinsCodeGen
            // (AR) Phase 7 Step 8: MathBuiltinsCodeGen
            // (AR) Phase 7 Step 9: MapOpsCodeGen
            // (AR) Phase 7 Step 10: ExceptionCodeGen
            // (AR) Phase 7 Step 11: LowlevelCodeGen
            // (AR) Phase 7 Step 12: EnumOpsCodeGen
            // (AR) Phase 7 Step 13: HardwareFFICodeGen
            // (AR) Phase 7 Step 14: ClosureCodeGen
            // (AR) Phase 7 Step 15: IO/Security/FFI builtins
            // (AR) Phase 7 Step 16: FreestandingCodeGen
            // (AR) Phase 7 Step 17: ObjectsArraysCodeGen
            // (AR) Phase 7 Step 18: OOPOpsCodeGen
        public:
            // ========================================================================
            // Constructor & Destructor / المنشئ والمدمر
            // ========================================================================

            LLVMCodeGen();
            ~LLVMCodeGen();

            // منع النسخ / Prevent copying
            LLVMCodeGen(const LLVMCodeGen &) = delete;
            LLVMCodeGen &operator=(const LLVMCodeGen &) = delete;

            // ========================================================================
            // Initialization / التهيئة
            // ========================================================================

            /**
             * تهيئة مولد الكود
             * Initialize the code generator
             *
             * @param moduleName اسم الوحدة / Module name
             * @param targetTriple سلسلة الهدف (مثل x86_64-linux-gnu) / Target triple
             * @return true إذا نجحت التهيئة / true if initialization succeeded
             */
            bool initialize(const std::string &moduleName, const std::string &targetTriple);

            /**
             * تهيئة السياق فقط بدون هدف محدد
             * Initialize context only without specific target
             *
             * @param moduleName اسم الوحدة / Module name
             * @return true إذا نجحت التهيئة / true if initialization succeeded
             */
            bool initialize(const std::string &moduleName);

            // ========================================================================
            // Main Generation / التوليد الرئيسي
            // ========================================================================

            /**
             * توليد LLVM IR من وحدة SIR
             * Generate LLVM IR from SIR module
             *
             * @param sirModule وحدة SIR المدخلة / Input SIR module
             * @return وحدة LLVM الناتجة / Resulting LLVM module
             */
            std::unique_ptr<llvm::Module> generate(std::shared_ptr<SIRModule> sirModule);

            // ========================================================================
            // (AR) ISSUE-063 (عائلة الترقية الديناميّة %SadDyn): مسحٌ مسبق لوحدة SIR يقرّر
            //      «ديناميّةَ» الخانات قبل الإصدار — بدل الترقية أثناء التدفّق (التي كانت
            //      تُهاجر القيمة داخل فرعٍ قد لا يُسلك ⇒ فقدان القيمة، وتُبقي قراءات الحلقة
            //      على الخانة القديمة ⇒ تعليق، وتفصم النسخة العامّة عن المحلّيّة):
            //      1) خاناتٌ محلّيّة/عامّة تُخزَّن فيها قيمةٌ ديناميّة (Any) أو مزيجُ
            //         نصّ/عشريّ ⇒ تُخصَّص %SadDyn منذ كتلة الدخول.
            //      2) حقول أصنافٍ تُخزَّن فيها قيمٌ ديناميّة أو من نوعٍ مخالفٍ للمعلَن ⇒
            //         يُرفَع نوع الحقل في SIR إلى Any (فيصير %SadDyn في الهيكل).
            //      3) دوالٌ تُرجع قيمًا ديناميّة ⇒ يُرفَع نوع إرجاعها إلى Any.
            // (EN) ISSUE-063 (%SadDyn promotion family): a pre-scan over the SIR module that
            //      decides slot dyn-ness BEFORE emission — replacing the mid-flow promotion
            //      (which migrated the value inside a possibly-untaken branch ⇒ value loss,
            //      left loop reads on the stale slot ⇒ hang, and split the global from its
            //      local view): 1) local/global slots ever stored a dynamic (Any) value or a
            //      string/float mix ⇒ allocated as %SadDyn from the entry block; 2) class
            //      fields stored dynamic/mismatched values ⇒ SIR field type raised to Any
            //      (⇒ %SadDyn struct slot); 3) functions returning dynamic values ⇒ return
            //      type raised to Any.
            // ========================================================================
            void collectDynSlots(std::shared_ptr<SIRModule> sirModule);

            /// (AR) هل هذه الخانة (محلّيًّا في funcName أو عامّةً) قرّرها المسحُ ديناميّةً؟
            /// (EN) did the pre-scan decide this slot (locally in funcName, or globally) is dynamic?
            bool isDynSlot(const std::string &funcName, const std::string &slotName) const;

            /// (AR) أسماء المتغيّرات العامّة الديناميّة (بلا بادئة %) / (EN) dynamic global slot names (no % prefix)
            std::set<std::string> dynGlobalSlots_;
            /// (AR) الخانات المحلّيّة الديناميّة لكلّ دالّة / (EN) dynamic local slots per function
            std::map<std::string, std::set<std::string>> dynLocalSlots_;

            /**
             * الحصول على الوحدة الحالية
             * Get current module
             *
             * @return مؤشر للوحدة / Pointer to module
             */
            llvm::Module *getModule() const { return module_.get(); }

            /**
             * الحصول على مُنشئ التعليمات
             * Get IR builder
             *
             * @return مؤشر للبانِي / Pointer to IR builder
             */
            llvm::IRBuilder<> *getBuilder() const { return builder_.get(); }

            /**
             * الحصول على سياق LLVM
             * Get LLVM context
             *
             * @return مؤشر للسياق / Pointer to context
             */
            llvm::LLVMContext *getContext() const { return context_.get(); }

            /**
             * نوع `size_t` الهدف (i32 على 32-بت، i64 على 64-بت)
             * Target `size_t` type (i32 on 32-bit, i64 on 64-bit)
             *
             * (AR) ⚠️ إلزاميّ لكلّ توقيع دالّة مكتبيّة C يأخذ حجمًا
             *      (memcpy/memmove/memset/memcmp…): الخلفيّة تخفّض
             *      `llvm.mem*` إلى نداء مكتبيّ بوسيط `size_t`، فتعريفٌ
             *      بـ i64 ثابت على هدف 32-بت يقرأ 8 بايت بينما النداء
             *      يدفع 4 ⇒ النصف الأعلى قمامة ⇒ دوس ذاكرة صامت.
             * (EN) Mandatory for any C library signature taking a size.
             *
             * (AR) ⚠️ هذا عرضُ **المؤشّر** (خاصّةُ ABI). ولعرضِ **سجلّ** الآلة —
             *      وهو ما تُقيَّد به سجلّاتُ التحكّم والأسمبليُّ المضمَّن — انظر
             *      `getTargetGprType`. يفترقان على x32 (مؤشّرٌ 32 وسجلٌّ 64).
             * (EN) This is the *pointer* width (an ABI property). For the machine
             *      *register* width — what inline asm is constrained by — see
             *      getTargetGprType. They differ on x32.
             *
             * @return نوع الحجم الموافق للهدف / Target-matching size type
             */
            llvm::IntegerType *getSizeType() const {
                return module_->getDataLayout().getIntPtrType(*context_);
            }

            /**
             * تحويل قيمة حجم إلى `size_t` الهدف
             * Coerce a size value to the target `size_t`
             */
            llvm::Value *coerceToSize(llvm::Value *v, const llvm::Twine &name = "sz") {
                llvm::Type *szTy = getSizeType();
                return v->getType() == szTy
                           ? v
                           : builder_->CreateZExtOrTrunc(v, szTy, name);
            }

            // ================================================================
            // (AR) مُصدِرات دوالّ التخصيص المكتبيّة — **المسار الوحيد المسموح**
            //      لإصدار نداء malloc/realloc/calloc/free/strlen.
            //
            //      ⚠️ لماذا مركزيّة؟ توقيع هذه الدوالّ يأخذ/يعيد `size_t`،
            //      وهو i32 على 32-بت. تصريحٌ بـ i64 ثابت في موقع استدعاء
            //      يجعل النداء يدفع 8 بايت لدالّة تقرأ 4 (أو العكس في
            //      strlen: يقرأ النصف الأعلى قمامةً من edx) ⇒ حجم هائل ⇒
            //      دوس ذاكرة صامت. تفرّق التصاريح عبر 23 ملفًّا هو ما أنتج
            //      هذا العيب أصلًا؛ فالمركزيّة هنا حارسٌ بنيويّ لا تجميل.
            //
            //      الأحجام الداخليّة في المترجم i64؛ هذه المُصدِرات تكيّفها.
            // (EN) Sole sanctioned path for emitting allocator libcalls —
            //      centralizes the target-dependent `size_t` contract.
            // ================================================================

            /** malloc(size_t) → ptr */
            llvm::Value *emitMalloc(llvm::Value *size, const llvm::Twine &name = "") {
                llvm::Type *ptrTy = llvm::PointerType::getUnqual(*context_);
                auto fn = module_->getOrInsertFunction(
                    "malloc", llvm::FunctionType::get(ptrTy, {getSizeType()}, false));
                return builder_->CreateCall(fn, {coerceToSize(size, "malloc.size")}, name);
            }

            /** realloc(ptr, size_t) → ptr */
            llvm::Value *emitRealloc(llvm::Value *ptr, llvm::Value *size,
                                     const llvm::Twine &name = "") {
                llvm::Type *ptrTy = llvm::PointerType::getUnqual(*context_);
                auto fn = module_->getOrInsertFunction(
                    "realloc",
                    llvm::FunctionType::get(ptrTy, {ptrTy, getSizeType()}, false));
                return builder_->CreateCall(fn, {ptr, coerceToSize(size, "realloc.size")}, name);
            }

            /** calloc(size_t, size_t) → ptr */
            llvm::Value *emitCalloc(llvm::Value *count, llvm::Value *size,
                                    const llvm::Twine &name = "") {
                llvm::Type *ptrTy = llvm::PointerType::getUnqual(*context_);
                llvm::Type *szTy = getSizeType();
                auto fn = module_->getOrInsertFunction(
                    "calloc", llvm::FunctionType::get(ptrTy, {szTy, szTy}, false));
                return builder_->CreateCall(
                    fn, {coerceToSize(count, "calloc.n"), coerceToSize(size, "calloc.size")}, name);
            }

            /** free(ptr) */
            llvm::Value *emitFreeCall(llvm::Value *ptr) {
                llvm::Type *ptrTy = llvm::PointerType::getUnqual(*context_);
                auto fn = module_->getOrInsertFunction(
                    "free",
                    llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {ptrTy}, false));
                return builder_->CreateCall(fn, {ptr});
            }

            /**
             * strlen(ptr) → i64
             *
             * (AR) التوقيع المُصدَر يعيد `size_t`؛ نمدّده إلى i64 لأنّ أطوال
             *      المترجم الداخليّة i64. بلا هذا يقرأ المستدعي edx قمامةً.
             */
            llvm::Value *emitStrlen(llvm::Value *str, const llvm::Twine &name = "len") {
                llvm::Type *ptrTy = llvm::PointerType::getUnqual(*context_);
                llvm::Type *szTy = getSizeType();
                auto fn = module_->getOrInsertFunction(
                    "strlen", llvm::FunctionType::get(szTy, {ptrTy}, false));
                llvm::Value *n = builder_->CreateCall(fn, {str}, name);
                return builder_->CreateZExt(n, llvm::Type::getInt64Ty(*context_), name + ".i64");
            }

            /**
             * التحقق من صحة الوحدة
             * Verify module correctness
             *
             * @return true إذا كانت الوحدة صحيحة / true if module is valid
             */
            bool verify() const { return fns_->verify(); }

            // ========================================================================
            // Optimization / التحسين
            // ========================================================================

            /**
             * تعيين مستوى التحسين
             * Set optimization level
             *
             * @param level مستوى التحسين (O0, O1, O2, O3, Os, Oz)
             */
            void setOptimizationLevel(sad::OptimizationLevel level) { fns_->setOptimizationLevel(level); }

            /**
             * (AR) ضبط وضع LTO — يفعل FullLTO أو ThinLTO
             * (EN) Set LTO mode — enable Full or Thin LTO
             */
            void setLTOMode(bool enable_full, bool enable_thin) { fns_->setLTOMode(enable_full, enable_thin); }

            /**
             * الحصول على مستوى التحسين الحالي
             * Get current optimization level
             *
             * @return مستوى التحسين / Optimization level
             */
            sad::OptimizationLevel getOptimizationLevel() const;

            /**
             * تحسين الوحدة الحالية
             * Optimize current module
             *
             * @return true إذا نجح التحسين / true if optimization succeeded
             *
             * @details
             * (AR) يطبق تحسينات LLVM حسب المستوى المحدد:
             *      - O0: بدون تحسين (للتنقيح)
             *      - O1: تحسينات أساسية
             *      - O2: تحسينات قياسية (موصى به)
             *      - O3: تحسينات عدوانية (أقصى أداء)
             *      - Os/Oz: تحسين للحجم
             *
             * (EN) Applies LLVM optimizations based on level:
             *      - O0: No optimization (debug)
             *      - O1: Basic optimizations
             *      - O2: Standard optimizations (recommended)
             *      - O3: Aggressive optimizations (max performance)
             *      - Os/Oz: Size optimizations
             */
            bool optimize() { return fns_->optimize(); }

            /**
             * تمكين/تعطيل التحسين التلقائي بعد التوليد
             * Enable/disable automatic optimization after generation
             *
             * @param enable true للتمكين / true to enable
             */
            void setAutoOptimize(bool enable) { autoOptimize_ = enable; }

            /**
             * الحصول على إحصائيات التحسين
             * Get optimization statistics
             *
             * @return إحصائيات التحسين / Optimization statistics
             */
            const sad::OptimizationStats &getOptimizationStats() const { return fns_->getOptimizationStats(); }

            /**
             * طباعة إحصائيات التحسين
             * Print optimization statistics
             */
            void printOptimizationStats() const { fns_->printOptimizationStats(); }

            // ========================================================================
            // Module Emission / إصدار الوحدة
            // ========================================================================

            /**
             * إصدار وحدة SIR كاملة
             * Emit complete SIR module
             *
             * @param sirModule وحدة SIR / SIR module
             */
            void emitModule(std::shared_ptr<SIRModule> sirModule) { cls_->emitModule(sirModule); }

            /**
             * معالجة الأصناف وإنشاء أنواع الهياكل
             * Pre-process classes and create struct types
             *
             * @param sirModule وحدة SIR / SIR module
             */
            void preprocessClasses(std::shared_ptr<SIRModule> sirModule) { cls_->preprocessClasses(sirModule); }

            /**
             * إصدار الدوال العامة / Global functions
             * Emit global functions
             *
             * @param sirModule وحدة SIR / SIR module
             */
            void emitGlobalFunctions(std::shared_ptr<SIRModule> sirModule) { fns_->emitGlobalFunctions(sirModule); }

            /**
             * إضافة دالة main كـ wrapper للدالة الرئيسية العربية
             * Add main wrapper for Arabic main function
             *
             * @param sirModule وحدة SIR / SIR module
             */
            void emitMainWrapper(std::shared_ptr<SIRModule> sirModule) { fns_->emitMainWrapper(sirModule); }

            /**
             * (AR) تعيين وضع الوحدة — تخطي إنشاء دالة main wrapper
             * (EN) Set module mode — skip main wrapper generation
             *
             * @param mode (AR) صحيح لتفعيل وضع الوحدة / (EN) true to enable module mode
             */
            void setModuleMode(bool mode) { moduleMode_ = mode; }

            /**
             * (AR) تعيين وضع مستقل — توليد وقت تشغيل مدمج لدوال C الأساسية
             * (EN) Set freestanding mode — emit built-in C runtime implementations
             *
             * @param mode (AR) صحيح لتفعيل الوضع المستقل / (EN) true to enable freestanding
             */
            void setFreestanding(bool mode) { freestanding_ = mode; }

            /**
             * (AR) نوعُ سجلّ الأغراض العامّة للهدف (i32 على i686 وi64 على x86_64).
             *      تُصاغ به سجلّاتُ التحكّم وعناوينُ إبطالِ الصفحات وواصفاتُ الجداول.
             *      اشتقاقُه من `freestanding_` يخلط وضعًا (بلا libc) بهدفٍ (عرضِ
             *      السجلّ)، فيُصدر `mov %cr3, %eax` لهدفٍ 64-بتّيّ ويُخفِق المُجمِّعُ
             *      بـ«cannot compile inline asm».
             *
             *      ⚠️ ولماذا **معماريّةُ الثالوث** لا عرضُ المؤشّر في تخطيط البيانات:
             *      عرضُ المؤشّر خاصّةُ ABI لا خاصّةُ سجلّ. على `x86_64‑…‑gnux32`
             *      يعطي التخطيطُ ‎p:32:32‎ فيُشتقّ i32، ويردّ المُجمِّعُ
             *      «instruction requires: Not 64-bit mode» — أي العطبُ نفسُه بمحكٍّ
             *      أدقَّ لكن ما زال خاطئًا. المحكُّ هو عرضُ السجلّ: `isArch64Bit`.
             * (EN) Target general-purpose register type, derived from the triple's
             *      architecture — not from the freestanding mode, and not from the
             *      data layout's pointer width (an ABI property: x32 is 64-bit
             *      registers with 32-bit pointers).
             */
            llvm::Type* getTargetGprType() const {
                // (AR) التطبيعُ لازمٌ: ثالوثٌ ثلاثيُّ المكوّنات يُقرأ مكوّنُه الثاني
                //      بائعًا لا نظامًا (انظر تحذير freestanding_codegen.h).
                return llvm::Triple(llvm::Triple::normalize(module_->getTargetTriple()))
                        .isArch64Bit()
                    ? llvm::Type::getInt64Ty(*context_)
                    : llvm::Type::getInt32Ty(*context_);
            }

            /**
             * (AR) تعيين حجم الكومة الساكنة الحرّة بالبايت (0 = الافتراضيّ المحافظ)
             * (EN) Set the freestanding static heap size in bytes (0 = default)
             */
            void setFreestandingHeapBytes(std::uint64_t bytes) { freestandingHeapBytes_ = bytes; }

            /**
             * (AR) توليد تطبيقات وقت التشغيل المستقل لدوال C الأساسية
             * (EN) Emit freestanding runtime implementations for essential C functions
             *      malloc, free, memcpy, memset, strlen, strcmp, strcpy, strcat,
             *      realloc, calloc, printf, sprintf, pow
             *      Uses linkonce_odr linkage so user can override with custom implementations
             */
            void emitFreestandingRuntime() { freest_->emitFreestandingRuntime(); }

            /**
             * (AR) نداء هلع الوضع الحرّ الموحّد: call void @__sad_panic(i64 reason).
             *      يوحّد بناء النوع/الإدراج/النداء عبر مواقع الهلع الخمسة كي لا
             *      يُمرَّر رمز سبب خاطئ يدويًّا (رمز السبب من sir_constants.h). لا
             *      يُصدر unreachable الختاميّ — المنادي يضيفه (قد يُشارَك مع المسار
             *      المستضاف). يُدرَج عند نقطة الإدراج الحاليّة للباني.
             * (EN) Unified freestanding panic call: call void @__sad_panic(i64 reason).
             *      Centralizes type/insert/call construction across the five panic
             *      sites so no wrong reason code is passed by hand. Does NOT emit the
             *      trailing unreachable (the caller adds it; may be shared with the
             *      hosted path). Emitted at the builder's current insert point.
             */
            void emitFreestandingPanicCall(int64_t reasonCode)
            {
                auto *i64Ty = llvm::Type::getInt64Ty(*context_);
                auto *panicFT = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*context_), {i64Ty}, false);
                auto panicFn = module_->getOrInsertFunction("__sad_panic", panicFT);
                builder_->CreateCall(panicFn, {llvm::ConstantInt::get(i64Ty, reasonCode)});
            }

            /**
             * إصدار المتغيرات العامة
             * Emit global variables
             *
             * @param sirModule وحدة SIR / SIR module
             */
            void emitGlobalVariables(std::shared_ptr<SIRModule> sirModule) { cls_->emitGlobalVariables(sirModule); }

            /**
             * إصدار الثوابت
             * Emit constants
             *
             * @param sirModule وحدة SIR / SIR module
             */
            void emitConstants(std::shared_ptr<SIRModule> sirModule) { fns_->emitConstants(sirModule); }

            // ========================================================================
            // Function Emission / إصدار الدوال
            // ========================================================================

            /**
             * إصدار دالة LLVM من دالة SIR
             * Emit LLVM function from SIR function
             *
             * @param sirFunc دالة SIR / SIR function
             * @return دالة LLVM / LLVM function
             */
            llvm::Function *emitFunction(std::shared_ptr<SIRFunction> sirFunc) { return fns_->emitFunction(sirFunc); }

            /**
             * إصدار جسم الدالة
             * Emit function body
             *
             * @param sirFunc دالة SIR / SIR function
             * @param llvmFunc دالة LLVM / LLVM function
             */
            void emitFunctionBody(std::shared_ptr<SIRFunction> sirFunc, llvm::Function *llvmFunc) { fns_->emitFunctionBody(sirFunc, llvmFunc); }

            /**
             * إصدار توقيع الدالة فقط (بدون جسم)
             * Emit function signature only (without body)
             *
             * @param sirFunc دالة SIR / SIR function
             * @return دالة LLVM / LLVM function
             */
            llvm::Function *emitFunctionPrototype(std::shared_ptr<SIRFunction> sirFunc) { return fns_->emitFunctionPrototype(sirFunc); }

            /**
             * إصدار معاملات الدالة
             * Emit function parameters
             *
             * @param sirFunc دالة SIR / SIR function
             * @param llvmFunc دالة LLVM / LLVM function
             */
            void emitFunctionParameters(std::shared_ptr<SIRFunction> sirFunc, llvm::Function *llvmFunc);

            // ========================================================================
            // Basic Block Emission / إصدار الكتل الأساسية
            // ========================================================================

            /**
             * إصدار كتلة أساسية
             * Emit basic block
             *
             * @param sirBlock كتلة SIR الأساسية / SIR basic block
             * @param function الدالة المحتوية / Containing function
             * @return كتلة LLVM الأساسية / LLVM basic block
             */
            llvm::BasicBlock *emitBasicBlock(std::shared_ptr<SIRBasicBlock> sirBlock,
                                             llvm::Function *function);

            /**
             * إنشاء كتلة أساسية فارغة
             * Create empty basic block
             *
             * @param name اسم الكتلة / Block name
             * @param function الدالة المحتوية / Containing function
             * @return كتلة LLVM الأساسية / LLVM basic block
             */
            llvm::BasicBlock *createBasicBlock(const std::string &name, llvm::Function *function);

            // ========================================================================
            // Instruction Emission / إصدار التعليمات
            // ========================================================================

            /**
             * إصدار تعليمة من SIR
             * Emit instruction from SIR
             *
             * @param inst تعليمة SIR / SIR instruction
             * @return قيمة LLVM الناتجة / Resulting LLVM value
             */
            llvm::Value *emitInstruction(std::shared_ptr<SIRInstruction> inst);

            // ------------------------------------------------------------------------
            // Arithmetic Instructions / التعليمات الحسابية
            // (AR) Phase 7 Step 1: delegate إلى ArithmeticCodeGen
            // ------------------------------------------------------------------------

            llvm::Value *emitAdd(std::shared_ptr<SIRInstruction> inst) { return arith_->emitAdd(inst); }
            llvm::Value *emitSub(std::shared_ptr<SIRInstruction> inst) { return arith_->emitSub(inst); }
            llvm::Value *emitMul(std::shared_ptr<SIRInstruction> inst) { return arith_->emitMul(inst); }
            llvm::Value *emitDiv(std::shared_ptr<SIRInstruction> inst) { return arith_->emitDiv(inst); }
            llvm::Value *emitMod(std::shared_ptr<SIRInstruction> inst) { return arith_->emitMod(inst); }
            llvm::Value *emitNeg(std::shared_ptr<SIRInstruction> inst) { return arith_->emitNeg(inst); }
            llvm::Value *emitTruncU8(std::shared_ptr<SIRInstruction> inst) { return arith_->emitTruncU8(inst); }
            llvm::Value *emitBoxDyn(std::shared_ptr<SIRInstruction> inst) { return arith_->emitBoxDyn(inst); }
            llvm::Value *emitNullAssert(std::shared_ptr<SIRInstruction> inst) { return arith_->emitNullAssert(inst); }

            // ------------------------------------------------------------------------
            // Bitwise Instructions / التعليمات الثنائية (Phase 7 Step 1: delegate)
            // ------------------------------------------------------------------------

            llvm::Value *emitAnd(std::shared_ptr<SIRInstruction> inst) { return arith_->emitAnd(inst); }
            llvm::Value *emitOr(std::shared_ptr<SIRInstruction> inst) { return arith_->emitOr(inst); }
            llvm::Value *emitXor(std::shared_ptr<SIRInstruction> inst) { return arith_->emitXor(inst); }
            llvm::Value *emitNot(std::shared_ptr<SIRInstruction> inst) { return arith_->emitNot(inst); }
            llvm::Value *emitShl(std::shared_ptr<SIRInstruction> inst) { return arith_->emitShl(inst); }
            llvm::Value *emitShr(std::shared_ptr<SIRInstruction> inst) { return arith_->emitShr(inst); }

            // ------------------------------------------------------------------------
            // Comparison Instructions / تعليمات المقارنة (Phase 7 Step 1: delegate Eq/Ne/Lt/Le)
            // (AR) Gt/Ge باقيتان في llvm_codegen_memory_control.cpp — لخطوة لاحقة
            // ------------------------------------------------------------------------

            llvm::Value *emitCmpEq(std::shared_ptr<SIRInstruction> inst) { return arith_->emitCmpEq(inst); }
            llvm::Value *emitCmpNe(std::shared_ptr<SIRInstruction> inst) { return arith_->emitCmpNe(inst); }
            llvm::Value *emitCmpLt(std::shared_ptr<SIRInstruction> inst) { return arith_->emitCmpLt(inst); }
            llvm::Value *emitCmpLe(std::shared_ptr<SIRInstruction> inst) { return arith_->emitCmpLe(inst); }
            llvm::Value *emitCmpGt(std::shared_ptr<SIRInstruction> inst) { return arith_->emitCmpGt(inst); }
            llvm::Value *emitCmpGe(std::shared_ptr<SIRInstruction> inst) { return arith_->emitCmpGe(inst); }

            // ------------------------------------------------------------------------
            // Memory Instructions / تعليمات الذاكرة
            // ------------------------------------------------------------------------

            // (AR) Phase 7 Step 2: delegate إلى MemoryCodeGen
            llvm::Value *emitLoad(std::shared_ptr<SIRInstruction> inst) { return mem_->emitLoad(inst); }
            llvm::Value *emitStore(std::shared_ptr<SIRInstruction> inst) { return mem_->emitStore(inst); }
            llvm::Value *emitAlloca(std::shared_ptr<SIRInstruction> inst) { return mem_->emitAlloca(inst); }
            llvm::Value *emitGEP(std::shared_ptr<SIRInstruction> inst) { return mem_->emitGEP(inst); }
            llvm::Value *emitMove(std::shared_ptr<SIRInstruction> inst) { return mem_->emitMove(inst); }

            // ------------------------------------------------------------------------
            // Control Flow Instructions / تعليمات تدفق التحكم
            // ------------------------------------------------------------------------

            // (AR) Phase 7 Step 3: delegate إلى ControlFlowCodeGen
            llvm::Value *emitBranch(std::shared_ptr<SIRInstruction> inst) { return cf_->emitBranch(inst); }
            llvm::Value *emitCondBranch(std::shared_ptr<SIRInstruction> inst) { return cf_->emitCondBranch(inst); }
            llvm::Value *emitCall(std::shared_ptr<SIRInstruction> inst) { return cf_->emitCall(inst); }
            llvm::Value *emitReturn(std::shared_ptr<SIRInstruction> inst) { return cf_->emitReturn(inst); }
            llvm::Value *emitSwitch(std::shared_ptr<SIRInstruction> inst) { return cf_->emitSwitch(inst); }

            // ------------------------------------------------------------------------
            // Builtin Functions / الدوال المضمنة
            // ------------------------------------------------------------------------

            llvm::Value *emitBuiltinPrint(std::shared_ptr<SIRInstruction> inst) { return iob_->emitBuiltinPrint(inst); } // اطبع / Print
            llvm::Value *emitBuiltinRead(std::shared_ptr<SIRInstruction> inst) { return baf_->emitBuiltinRead(inst); }   // اقرأ / Read/Input
            // (AR) Phase 7 Step 6: delegate إلى StringOpsCodeGen
            llvm::Value *emitStringConcat(std::shared_ptr<SIRInstruction> inst) { return strops_->emitStringConcat(inst); }
            void ensureArrayToStringHelper() { strs_->ensureArrayToStringHelper(); } // توليد دالة __sad_array_to_string / Generate array-to-string helper
            void ensureArrayToStringStrHelper() { strs_->ensureArrayToStringStrHelper(); } // نظير نصّيّ __sad_array_to_string_str / String variant
            void ensureArrayToStringFloatHelper() { strs_->ensureArrayToStringFloatHelper(); } // نظير عشريّ __sad_array_to_string_float / Float variant
            void ensureArrayToStringDynHelper() { strs_->ensureArrayToStringDynHelper(); } // نظير موسوم __sad_array_to_string_dyn / boxed variant
            void ensureMapToStringHelper(bool quoteKeys) { strs_->ensureMapToStringHelper(quoteKeys); } // __sad_map_to_string / Map-to-string helper
            llvm::Value *emitStringCharAt(std::shared_ptr<SIRInstruction> inst) { return strops_->emitStringCharAt(inst); }
            llvm::Value *emitStringCmp(std::shared_ptr<SIRInstruction> inst) { return strops_->emitStringCmp(inst); }
            llvm::Value *emitStringOrdCmp(std::shared_ptr<SIRInstruction> inst) { return strops_->emitStringOrdCmp(inst); }

            // (AR) دالة مساعدة: تحميل القيمة تلقائياً من alloca إذا لزم الأمر
            // (EN) Helper: Auto-load value from alloca pointer if needed
            llvm::Value *resolveValue(llvm::Value *val, SadTypeKind sirType);

            llvm::Value *emitStringToI64(std::shared_ptr<SIRInstruction> inst) { return strops_->emitStringToI64(inst); }
            llvm::Value *emitStringToF64(std::shared_ptr<SIRInstruction> inst) { return strops_->emitStringToF64(inst); }
            // (AR) Phase 7 Step 8: 19 math builtins delegate إلى MathBuiltinsCodeGen
            llvm::Value *emitBuiltinSqrt(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinSqrt(inst); }
            llvm::Value *emitBuiltinLog(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinLog(inst); }
            llvm::Value *emitBuiltinPow(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinPow(inst); }
            llvm::Value *emitBuiltinAbs(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinAbs(inst); }
            llvm::Value *emitBuiltinRound(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinRound(inst); }
            llvm::Value *emitBuiltinFloor(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinFloor(inst); }
            llvm::Value *emitBuiltinCeil(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinCeil(inst); }
            llvm::Value *emitBuiltinSin(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinSin(inst); }
            llvm::Value *emitBuiltinCos(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinCos(inst); }
            llvm::Value *emitBuiltinTan(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinTan(inst); }
            llvm::Value *emitBuiltinLog10(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinLog10(inst); }
            llvm::Value *emitBuiltinLog2(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinLog2(inst); }
            llvm::Value *emitBuiltinAsin(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinAsin(inst); }
            llvm::Value *emitBuiltinAcos(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinAcos(inst); }
            llvm::Value *emitBuiltinAtan(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinAtan(inst); }
            llvm::Value *emitBuiltinTrunc(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinTrunc(inst); }
            llvm::Value *emitBuiltinFmod(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinFmod(inst); }
            llvm::Value *emitBuiltinClamp(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinClamp(inst); }

            // String Functions (12) — (AR) Phase 7 Step 6: delegate إلى StringOpsCodeGen
            llvm::Value *emitBuiltinStringLength(std::shared_ptr<SIRInstruction> inst) { return strops_->emitBuiltinStringLength(inst); }
            llvm::Value *emitBuiltinStringToUpper(std::shared_ptr<SIRInstruction> inst) { return strops_->emitBuiltinStringToUpper(inst); }
            llvm::Value *emitBuiltinStringToLower(std::shared_ptr<SIRInstruction> inst) { return strops_->emitBuiltinStringToLower(inst); }
            llvm::Value *emitBuiltinStringFind(std::shared_ptr<SIRInstruction> inst) { return strops_->emitBuiltinStringFind(inst); }
            llvm::Value *emitBuiltinStringReplace(std::shared_ptr<SIRInstruction> inst) { return strops_->emitBuiltinStringReplace(inst); }
            llvm::Value *emitBuiltinStringSubstring(std::shared_ptr<SIRInstruction> inst) { return strops_->emitBuiltinStringSubstring(inst); }
            llvm::Value *emitBuiltinStringTrim(std::shared_ptr<SIRInstruction> inst) { return strops_->emitBuiltinStringTrim(inst); }
            llvm::Value *emitBuiltinStringSplit(std::shared_ptr<SIRInstruction> inst) { return strops_->emitBuiltinStringSplit(inst); }
            llvm::Value *emitBuiltinStringJoin(std::shared_ptr<SIRInstruction> inst) { return strops_->emitBuiltinStringJoin(inst); }
            llvm::Value *emitBuiltinStringStartsWith(std::shared_ptr<SIRInstruction> inst) { return strops_->emitBuiltinStringStartsWith(inst); }
            llvm::Value *emitBuiltinStringEndsWith(std::shared_ptr<SIRInstruction> inst) { return strops_->emitBuiltinStringEndsWith(inst); }
            llvm::Value *emitBuiltinStringContains(std::shared_ptr<SIRInstruction> inst) { return strops_->emitBuiltinStringContains(inst); }

            // ================================================================
            // (AR) دوال مساعدة للخرائط (Maps) — تُنشأ كدوال LLVM داخلية عند الحاجة
            //      getOrCreateMapFindSlot: بحث خطي عن مفتاح أو أول خانة فارغة
            //      getOrCreateMapCollect: تجميع العناصر غير الفارغة في مصفوفة
            // (EN) Map helper functions — created as internal LLVM functions on-demand
            //      getOrCreateMapFindSlot: linear search for key or first empty slot
            //      getOrCreateMapCollect: collect non-null entries into array
            // ================================================================
            llvm::Function *getOrCreateMapFindSlot() { return mapops_->getOrCreateMapFindSlot(); }
            llvm::Function *getOrCreateMapCollect() { return mapops_->getOrCreateMapCollect(); }

            // ================================================================
            // (AR) إصلاح setjmp/longjmp مع optimizer:
            //      يجعل كل loads/stores على GlobalVariables في دوال تستدعي _setjmp
            //      volatile لمنع LLVM optimizer من كسر السلوك الصحيح للاستثناءات.
            //      يُستدعى بعد emitModule() وقبل optimize() في generate().
            //      السبب: LLVM GlobalOpt يُحوّل internal globals إلى SSA form تُخزَّن في
            //      callee-saved registers. لكن longjmp يُعيد هذه الـ registers للقيم
            //      القديمة (وقت setjmp) فيفسد النتائج. Volatile يمنع هذا التحسين.
            // (EN) Fix setjmp/longjmp optimizer corruption:
            //      Marks all loads/stores to GlobalVariables in functions calling _setjmp
            //      as volatile to prevent LLVM optimizer from breaking exception behavior.
            //      Called after emitModule() and before optimize() in generate().
            //      Reason: LLVM GlobalOpt converts internal globals to SSA form stored in
            //      callee-saved registers. longjmp restores those registers to their old
            //      values (at setjmp time) corrupting the results. Volatile prevents this.
            // ================================================================
            void markSetjmpGlobalsVolatile() { exc_->markSetjmpGlobalsVolatile(); }

            // (AR) دوال مساعدة تُفوّض إليها emitCall لتقليل حجم الملف (Strangler Fig)
            //      emitCallException: معالجة دوال runtime الاستثناءات (__sad_alloc_jmpbuf, __sad_raise, ...)
            //      emitCallMap: معالجة دوال runtime الخرائط (__sad_map_create, __sad_map_get, ...)
            // (EN) Helper functions called by emitCall to reduce file size (Strangler Fig)
            //      emitCallException: handle exception runtime functions
            //      emitCallMap: handle map runtime functions
            std::optional<llvm::Value *> emitCallException(const std::string &funcName,
                                                           std::vector<llvm::Value *> &args, std::shared_ptr<SIRInstruction> inst)
            {
                return exc_->emitCallException(funcName, args, inst);
            }
            std::optional<llvm::Value *> emitCallMap(const std::string &funcName,
                                                     std::vector<llvm::Value *> &args, std::shared_ptr<SIRInstruction> inst)
            {
                return mapops_->emitCallMap(funcName, args, inst);
            }

            // (AR) دوال dispatcher الفرعية للتعليمات — مستخرجة من emitInstruction (Strangler Fig v3.1)
            //      emitInstructionCore     : الجوهر (حساب، async، كائنات، سلاسل، FFI، مصفوفات...)
            //      emitInstructionLowlevel : مستوى منخفض (CPU، UEFI، APIC، GDT، Paging...)
            //      emitInstructionPlatform : منصات (Android، UI، توجيهات @حجم/@ذري، وحدات)
            // (EN) Sub-dispatchers for emitInstruction — extracted by Strangler Fig v3.1
            //      emitInstructionCore     : core (arithmetic, async, objects, strings, FFI, arrays...)
            //      emitInstructionLowlevel : low-level (CPU, UEFI, APIC, GDT, Paging...)
            //      emitInstructionPlatform : platform (Android, UI, @sizeof/@atomic directives, modules)
            llvm::Value *emitInstructionCore(std::shared_ptr<SIRInstruction> inst) { return ic_->emitInstructionCore(inst); }
            llvm::Value *emitInstructionLowlevel(std::shared_ptr<SIRInstruction> inst) { return ill_->emitInstructionLowlevel(inst); }
            llvm::Value *emitInstructionPlatform(std::shared_ptr<SIRInstruction> inst) { return ip_->emitInstructionPlatform(inst); }

            // (AR) الطبقة الرابعة: دوال الشبكة (TCP/UDP، HTTP، عناوين)
            //      تُصدر استدعاءات لدوال C API من features/network/core
            // (EN) Tier 4: Network functions (TCP/UDP, HTTP, addresses)
            //      Emits calls to C API functions from features/network/core
            llvm::Value *emitNetworkBuiltin(std::shared_ptr<SIRInstruction> inst) { return nb_->emitNetworkBuiltin(inst); }

            // (AR) وحدةُ «عمليات»: fork/execvp، waitpid، pipe2، close، open — دوالٌّ
            //      تُولَّدُ في الوحدةِ المُصدَرةِ نفسِها وتنادي libc (لا مكتبةَ تُربَط).
            // (EN) The «عمليات» module: emitted as module-local functions calling libc.
            llvm::Value *emitProcessesBuiltin(std::shared_ptr<SIRInstruction> inst) { return proc_->emitProcessesBuiltin(inst); }

            // (AR) الطبقة الخامسة: تعليمات SIMD المتجهات (Phase 3)
            //      تنتج تعليمات LLVM متجهة <N x T> أصلية: FMA, sqrt.v8f64,
            //      vector.reduce.fadd، إلخ. الأداء يتفوق على C++ scalar.
            // (EN) Tier 5: SIMD vector instructions (Phase 3)
            //      Emits native LLVM <N x T> instructions: FMA, sqrt.v8f64,
            //      vector.reduce.fadd, etc. Performance exceeds C++ scalar.
            llvm::Value *emitInstructionSimd(std::shared_ptr<SIRInstruction> inst) { return simd_->emitInstructionSimd(inst); }

            // (AR) دالة مساعدة: إصدار استدعاء دالة C خارجية للشبكة
            // (EN) Helper: emit call to an extern C network function
            llvm::Value *emitNetworkCall(std::shared_ptr<SIRInstruction> inst, const char *cFuncName, llvm::Type *returnType, const std::vector<llvm::Type *> &paramTypes) { return nb_->emitNetworkCall(inst, cFuncName, returnType, paramTypes); }

            // (AR) Phase 7 Step 5: delegate إلى ArrayOpsCodeGen (تبقى wrappers لأن array_file_coro.cpp يستدعيها)
            llvm::Value *normalizeArrayPtr(llvm::Value *arrPtr, const char *label = "arr") { return arr_->normalizeArrayPtr(arrPtr, label); }
            llvm::Value *normalizeArrayIndex(llvm::Value *index, llvm::Value *arrPtr, const char *label = "idx") { return arr_->normalizeArrayIndex(index, arrPtr, label); }
            void emitBoundsCheck(llvm::Value *index, llvm::Value *arrPtr, const char *label = "bc") { arr_->emitBoundsCheck(index, arrPtr, label); }

            // Array Functions (10) — (AR) Phase 7 Step 7: 8 مفوّضة إلى ArrayBuiltinsCodeGen (Append/Remove تبقى)
            llvm::Value *emitBuiltinArrayAppend(std::shared_ptr<SIRInstruction> inst) { return strs_->emitBuiltinArrayAppend(inst); }
            llvm::Value *emitBuiltinArrayRemove(std::shared_ptr<SIRInstruction> inst) { return strs_->emitBuiltinArrayRemove(inst); }
            llvm::Value *emitBuiltinArraySize(std::shared_ptr<SIRInstruction> inst) { return arrb_->emitBuiltinArraySize(inst); }
            llvm::Value *emitBuiltinArrayIndexOf(std::shared_ptr<SIRInstruction> inst) { return arrb_->emitBuiltinArrayIndexOf(inst); }
            llvm::Value *emitBuiltinArrayContains(std::shared_ptr<SIRInstruction> inst) { return arrb_->emitBuiltinArrayContains(inst); }
            llvm::Value *emitBuiltinArrayReverse(std::shared_ptr<SIRInstruction> inst) { return arrb_->emitBuiltinArrayReverse(inst); }
            llvm::Value *emitBuiltinArraySort(std::shared_ptr<SIRInstruction> inst) { return arrb_->emitBuiltinArraySort(inst); }
            llvm::Value *emitBuiltinArrayFirst(std::shared_ptr<SIRInstruction> inst) { return arrb_->emitBuiltinArrayFirst(inst); }
            llvm::Value *emitBuiltinArrayLast(std::shared_ptr<SIRInstruction> inst) { return arrb_->emitBuiltinArrayLast(inst); }
            llvm::Value *emitBuiltinArraySlice(std::shared_ptr<SIRInstruction> inst) { return arrb_->emitBuiltinArraySlice(inst); }

            // File I/O Functions (8)
            llvm::Value *emitBuiltinFileRead(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileRead(inst); }
            llvm::Value *emitBuiltinFileWrite(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileWrite(inst); }
            llvm::Value *emitBuiltinFileWriteBytes(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileWriteBytes(inst); }
            llvm::Value *emitBuiltinFileReadBytes(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileReadBytes(inst); }
            llvm::Value *emitBuiltinFileSize(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileSize(inst); }
            llvm::Value *emitBuiltinFileAppend(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileAppend(inst); }
            llvm::Value *emitBuiltinFileDelete(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileDelete(inst); }
            llvm::Value *emitBuiltinFileCopy(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileCopy(inst); }
            llvm::Value *emitBuiltinFileMove(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileMove(inst); }
            llvm::Value *emitBuiltinFileCreateDir(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileCreateDir(inst); }
            llvm::Value *emitBuiltinFileListDir(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileListDir(inst); }
            llvm::Value *emitBuiltinRegexSearch(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinRegexSearch(inst); }
            llvm::Value *emitBuiltinRegexMatch(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinRegexMatch(inst); }
            llvm::Value *emitBuiltinFileIsDir(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileIsDir(inst); }
            llvm::Value *emitBuiltinFileRemoveDir(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileRemoveDir(inst); }
            llvm::Value *emitBuiltinFileExists(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileExists(inst); }
            llvm::Value *emitBuiltinFileIsFile(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileIsFile(inst); }
            llvm::Value *emitBuiltinFileIsSymlink(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileIsSymlink(inst); }
            llvm::Value *emitBuiltinFileRealPath(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileRealPath(inst); }
            llvm::Value *emitBuiltinFileAbsPath(std::shared_ptr<SIRInstruction> inst) { return fc_->emitBuiltinFileAbsPath(inst); }

            // Utility Functions (4)
            // (AR) Phase 7 Step 8: emitBuiltinRandom delegate إلى MathBuiltinsCodeGen
            llvm::Value *emitBuiltinRandom(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinRandom(inst); }
            llvm::Value *emitBuiltinSleep(std::shared_ptr<SIRInstruction> inst) { return baf_->emitBuiltinSleep(inst); }
            llvm::Value *emitBuiltinExit(std::shared_ptr<SIRInstruction> inst) { return baf_->emitBuiltinExit(inst); }
            llvm::Value *emitBuiltinTypeOf(std::shared_ptr<SIRInstruction> inst) { return strs_->emitBuiltinTypeOf(inst); }

            // New stdlib builtins - دوال المكتبة القياسية الجديدة
            llvm::Value *emitBuiltinIsType(std::shared_ptr<SIRInstruction> inst, const std::string &typeName) { return baf_->emitBuiltinIsType(inst, typeName); }
            llvm::Value *emitBuiltinToBool(std::shared_ptr<SIRInstruction> inst) { return baf_->emitBuiltinToBool(inst); }
            llvm::Value *emitBuiltinReadLine(std::shared_ptr<SIRInstruction> inst) { return baf_->emitBuiltinReadLine(inst); }
            llvm::Value *emitBuiltinClearScreen(std::shared_ptr<SIRInstruction> inst) { return baf_->emitBuiltinClearScreen(inst); }
            llvm::Value *emitBuiltinSum(std::shared_ptr<SIRInstruction> inst) { return baf_->emitBuiltinSum(inst); }

            // ================================================================
            // عمليات برمجة أنظمة التشغيل — OS Development Operations
            // ================================================================
            llvm::Value *emitInlineAsm(std::shared_ptr<SIRInstruction> inst) { return strs_->emitInlineAsm(inst); }  // تجميع مضمّن
            llvm::Value *emitPortWrite(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitPortWrite(inst); } // اكتب_منفذ / outb/outw/outl
            llvm::Value *emitPortRead(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitPortRead(inst); }   // اقرأ_منفذ / inb/inw/inl
            llvm::Value *emitMemWrite(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitMemWrite(inst); }   // اكتب_ذاكرة / poke
            llvm::Value *emitMemRead(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitMemRead(inst); }     // اقرأ_ذاكرة / peek
            llvm::Value *emitInterrupt(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitInterrupt(inst); } // مقاطعة / int
            llvm::Value *emitHalt(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitHalt(inst); }           // توقف / hlt
            llvm::Value *emitCli(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitCli(inst); }             // تعطيل_مقاطعات / cli
            llvm::Value *emitSti(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitSti(inst); }             // تفعيل_مقاطعات / sti
            llvm::Value *emitVgaWrite(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitVgaWrite(inst); }   // شاشة_اكتب / vga_write
            llvm::Value *emitVgaClear(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitVgaClear(inst); }   // شاشة_امسح / vga_clear
            llvm::Value *emitMemCopy(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitMemCopy(inst); }     // انسخ_ذاكرة / memcpy
            llvm::Value *emitMemSet(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitMemSet(inst); }       // املأ_ذاكرة / memset
            llvm::Value *emitMemFill32(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitMemFill32(inst); } // املأ_ذاكرة32 / memset32 (REP STOSD)
            llvm::Value *emitMemCopy32(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitMemCopy32(inst); } // انسخ_ذاكرة32 / memcpy32 (REP MOVSD)
            llvm::Value *emitAddrOf(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitAddrOf(inst); }       // عنوان / addr_of

            // ================================================================
            // عمليات Embedded المتقدمة — Advanced Embedded Operations (18)
            // ================================================================
            // Serial I/O (4)
            llvm::Value *emitSerialInit(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitSerialInit(inst); }   // تسلسلي_هيئ / serial_init
            llvm::Value *emitSerialWrite(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitSerialWrite(inst); } // تسلسلي_ارسل / serial_send
            llvm::Value *emitSerialRead(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitSerialRead(inst); }   // تسلسلي_استقبل / serial_receive
            llvm::Value *emitSerialReady(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitSerialReady(inst); } // تسلسلي_جاهز / serial_ready
            // GPIO (3)
            llvm::Value *emitGpioWrite(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitGpioWrite(inst); } // منفذ_رقمي_اكتب / gpio_write
            llvm::Value *emitGpioRead(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitGpioRead(inst); }   // منفذ_رقمي_اقرأ / gpio_read
            llvm::Value *emitGpioMode(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitGpioMode(inst); }   // حدد_وضع_منفذ / gpio_mode
            // Timer (3)
            llvm::Value *emitTimerInit(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitTimerInit(inst); } // مؤقت_هيئ / timer_init
            llvm::Value *emitTimerRead(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitTimerRead(inst); } // مؤقت_قراءة / timer_read
            llvm::Value *emitTimerWait(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitTimerWait(inst); } // مؤقت_انتظر / timer_wait
            // System Control (3)
            llvm::Value *emitReset(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitReset(inst); } // اعد_تشغيل / reset
            llvm::Value *emitCpuId(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitCpuId(inst); } // معرف_المعالج / cpuid
            llvm::Value *emitRdtsc(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitRdtsc(inst); } // عداد_الدورات / rdtsc
            // Memory Barriers (3)
            llvm::Value *emitMfence(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitMfence(inst); } // حاجز_ذاكرة / mfence
            llvm::Value *emitLfence(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitLfence(inst); } // حاجز_قراءة / lfence
            llvm::Value *emitSfence(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitSfence(inst); } // حاجز_كتابة / sfence
            // DMA (2)
            llvm::Value *emitDmaInit(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitDmaInit(inst); }   // نقل_مباشر_هيئ / dma_init
            llvm::Value *emitDmaStart(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitDmaStart(inst); } // نقل_مباشر_ابدأ / dma_start

            // ================================================================
            // دوال الأمان — Security Functions (14)
            // ================================================================
            llvm::Value *emitBuiltinSecurityAssert(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecurityAssert(inst); }               // تأكد
            llvm::Value *emitBuiltinSecurityVerify(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecurityVerify(inst); }               // تحقق
            llvm::Value *emitBuiltinSecurityIsSafe(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecurityIsSafe(inst); }               // آمن
            llvm::Value *emitBuiltinSecurityPanic(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecurityPanic(inst); }                 // ذعر
            llvm::Value *emitBuiltinSecurityHash(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecurityHash(inst); }                   // هاش
            llvm::Value *emitBuiltinSecurityEncrypt(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecurityEncrypt(inst); }             // شفّر
            llvm::Value *emitBuiltinSecurityDecrypt(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecurityDecrypt(inst); }             // فك_تشفير
            llvm::Value *emitBuiltinSecurityAssertType(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecurityAssertType(inst); }       // تأكد_نوع
            llvm::Value *emitBuiltinSecurityAssertEqual(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecurityAssertEqual(inst); }     // تأكد_مساواة
            llvm::Value *emitBuiltinSecurityAssertGreater(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecurityAssertGreater(inst); } // تأكد_أكبر
            llvm::Value *emitBuiltinSecuritySanitize(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecuritySanitize(inst); }           // نظّف
            llvm::Value *emitBuiltinSecurityTimestamp(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecurityTimestamp(inst); }         // وقت_الآن
            llvm::Value *emitBuiltinSecuritySecureRandom(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecuritySecureRandom(inst); }   // عشوائي_آمن
            llvm::Value *emitBuiltinSecurityBase64Encode(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinSecurityBase64Encode(inst); }   // ترميز_64
            llvm::Value *emitBuiltinCryptoBlake3Hash(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoBlake3Hash(inst); }           // بلايك3
            llvm::Value *emitBuiltinCryptoBlake3KeyedHash(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoBlake3KeyedHash(inst); } // هاش_مفتاح
            llvm::Value *emitBuiltinCryptoKdfPbkdf2(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoKdfPbkdf2(inst); }             // اشتق_مفتاح_مرور
            llvm::Value *emitBuiltinCryptoKdfHkdf(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoKdfHkdf(inst); }                 // اشتق_مفتاح
            llvm::Value *emitBuiltinCryptoAeadEncrypt(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoAeadEncrypt(inst); }         // شفّر_موثّق
            llvm::Value *emitBuiltinCryptoAeadDecrypt(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoAeadDecrypt(inst); }         // فك_تشفير_موثّق
            llvm::Value *emitBuiltinCryptoKdfArgon2id(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoKdfArgon2id(inst); }         // أرجون2
            llvm::Value *emitBuiltinCryptoX25519KeygenPriv(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoX25519KeygenPriv(inst); }   // ولّد_مفتاح_خاص_x25519
            llvm::Value *emitBuiltinCryptoX25519DerivePub(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoX25519DerivePub(inst); }     // اشتق_مفتاح_عام_x25519
            llvm::Value *emitBuiltinCryptoX25519Exchange(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoX25519Exchange(inst); }       // تبادل_مفتاح
            llvm::Value *emitBuiltinCryptoEd25519KeygenPriv(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoEd25519KeygenPriv(inst); } // ولّد_مفتاح_خاص_توقيع
            llvm::Value *emitBuiltinCryptoEd25519DerivePub(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoEd25519DerivePub(inst); }   // اشتق_مفتاح_عام_توقيع
            llvm::Value *emitBuiltinCryptoEd25519Sign(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoEd25519Sign(inst); }             // وقّع
            llvm::Value *emitBuiltinCryptoEd25519Verify(std::shared_ptr<SIRInstruction> inst) { return secb_->emitBuiltinCryptoEd25519Verify(inst); }         // تحقق_توقيع

            // ================================================================
            // التكامل مع C/C++ — FFI Functions (20)
            // ================================================================
            llvm::Value *emitFFIPrintf(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIPrintf(inst); }    // طباعة_تنسيق / printf
            llvm::Value *emitFFIMalloc(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIMalloc(inst); }    // حجز / malloc
            llvm::Value *emitFFIFree(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIFree(inst); }        // حرر / free
            llvm::Value *emitFFIRealloc(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIRealloc(inst); }  // اعد_حجز / realloc
            llvm::Value *emitFFICalloc(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFICalloc(inst); }    // حجز_صفري / calloc
            llvm::Value *emitFFIStrlen(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIStrlen(inst); }    // طول_نص_س / strlen → __sad_utf8_strlen
            llvm::Function *getOrCreateUtf8Strlen() { return hwffi_->getOrCreateUtf8Strlen(); }                         // (AR) بناء __sad_utf8_strlen عند الحاجة
            llvm::Function *getOrCreateUtf8ByteToChar() { return hwffi_->getOrCreateUtf8ByteToChar(); }                 // (AR) تحويل موقع البايت إلى فهرس الحرف UTF-8
            llvm::Function *getOrCreateUtf8CharToByte() { return hwffi_->getOrCreateUtf8CharToByte(); }                 // (AR) تحويل فهرس الحرف إلى موقع البايت UTF-8
            llvm::Value *emitFFIStrcpy(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIStrcpy(inst); }    // انسخ_نص_س / strcpy
            llvm::Value *emitFFIStrcmp(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIStrcmp(inst); }    // قارن_نص_س / strcmp
            llvm::Value *emitFFIStrcat(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIStrcat(inst); }    // الحق_نص_س / strcat
            llvm::Value *emitFFIMemcpy(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIMemcpy(inst); }    // انسخ_ذاكرة_س / memcpy
            llvm::Value *emitFFIMemset(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIMemset(inst); }    // عبئ_ذاكرة_س / memset
            llvm::Value *emitFFIFopen(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIFopen(inst); }      // افتح_ملف_س / fopen
            llvm::Value *emitFFIFclose(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIFclose(inst); }    // اغلق_ملف_س / fclose
            llvm::Value *emitFFIFwrite(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIFwrite(inst); }    // اكتب_ملف_س / fputs
            llvm::Value *emitFFIFread(std::shared_ptr<SIRInstruction> inst) { return hwffi_->emitFFIFread(inst); }      // اقرأ_ملف_س / fgets
            llvm::Value *emitFFISystem(std::shared_ptr<SIRInstruction> inst) { return ffir_->emitFFISystem(inst); }     // نفذ_امر / system
            llvm::Value *emitFFIGetenv(std::shared_ptr<SIRInstruction> inst) { return ffir_->emitFFIGetenv(inst); }     // قيمة_بيئة / getenv
            llvm::Value *emitFFIAtoi(std::shared_ptr<SIRInstruction> inst) { return ffir_->emitFFIAtoi(inst); }         // نص_لعدد / atoi
            llvm::Value *emitFFIAtof(std::shared_ptr<SIRInstruction> inst) { return ffir_->emitFFIAtof(inst); }         // نص_لعشري / atof
            llvm::Value *emitFFISnprintf(std::shared_ptr<SIRInstruction> inst) { return ffir_->emitFFISnprintf(inst); } // تنسيق_نص / snprintf

            // ================================================================
            // (AR) دوال مساعدة لتوليد وقت التشغيل المستقل (freestanding)
            // (EN) Freestanding runtime helper functions
            // ================================================================
            void emitFreestandingMalloc(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingMalloc(i8Ty, i64Ty, ptrTy); }
            void emitFreestandingFree(llvm::Type *ptrTy, llvm::Type *voidTy) { freest_->emitFreestandingFree(ptrTy, voidTy); }
            void emitFreestandingMemcpy(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingMemcpy(i8Ty, i64Ty, ptrTy); }
            void emitFreestandingMemmove(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingMemmove(i8Ty, i64Ty, ptrTy); }
            void emitFreestandingMemset(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingMemset(i8Ty, i32Ty, i64Ty, ptrTy); }
            void emitFreestandingMemcmp(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingMemcmp(i8Ty, i32Ty, i64Ty, ptrTy); }
            void emitFreestandingStrlen(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingStrlen(i8Ty, i64Ty, ptrTy); }
            void emitFreestandingStrcmp(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingStrcmp(i8Ty, i32Ty, i64Ty, ptrTy); }
            void emitFreestandingStrcpy(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingStrcpy(i8Ty, i64Ty, ptrTy); }
            void emitFreestandingStrcat(llvm::Type *ptrTy) { freest_->emitFreestandingStrcat(ptrTy); }
            void emitFreestandingRealloc(llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingRealloc(i64Ty, ptrTy); }
            void emitFreestandingCalloc(llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingCalloc(i64Ty, ptrTy); }
            void emitFreestandingPrintf(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingPrintf(i8Ty, i32Ty, i64Ty, ptrTy); }
            void emitFreestandingSprintf(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingSprintf(i8Ty, i32Ty, i64Ty, ptrTy); }
            void emitFreestandingPow(llvm::Type *dblTy) { freest_->emitFreestandingPow(dblTy); }
            void emitFreestandingSerialPuts(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingSerialPuts(i8Ty, i64Ty, ptrTy); }
            void emitFreestandingSerialPutInt(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingSerialPutInt(i8Ty, i64Ty, ptrTy); }
            void emitFreestandingItoa(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingItoa(i8Ty, i32Ty, i64Ty, ptrTy); }
            void emitFreestandingFtoa(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingFtoa(i8Ty, i32Ty, i64Ty, ptrTy); }
            void emitFreestandingXtoa(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy) { freest_->emitFreestandingXtoa(i8Ty, i32Ty, i64Ty, ptrTy); }

            // ========================================================================
            // Async/Await & Concurrency / تعليمات التزامن
            // ========================================================================
            llvm::Value *emitAsyncSpawn(std::shared_ptr<SIRInstruction> inst) { return baf_->emitAsyncSpawn(inst); }                              // أنشئ_مهمة
            llvm::Value *emitAsyncAwait(std::shared_ptr<SIRInstruction> inst) { return baf_->emitAsyncAwait(inst); }                              // انتظر_مهمة
            llvm::Value *emitAsyncYield(std::shared_ptr<SIRInstruction> inst) { return baf_->emitAsyncYield(inst); }                              // أنتج
            llvm::Value *emitAsyncSleep(std::shared_ptr<SIRInstruction> inst) { return baf_->emitAsyncSleep(inst); }                              // نوم_غير_متزامن
            llvm::Value *emitAsyncCreateFuture(std::shared_ptr<SIRInstruction> inst) { return baf_->emitAsyncCreateFuture(inst); }                // أنشئ_مستقبل
            llvm::Value *emitAsyncResolveFuture(std::shared_ptr<SIRInstruction> inst) { return baf_->emitAsyncResolveFuture(inst); }              // أوفِ_مستقبل
            llvm::Value *emitAsyncGetFuture(std::shared_ptr<SIRInstruction> inst) { return baf_->emitAsyncGetFuture(inst); }                      // احصل_مستقبل
            llvm::Value *emitAsyncCreateChannel(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncCreateChannel(inst); }           // أنشئ_قناة
            llvm::Value *emitAsyncChannelSend(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncChannelSend(inst); }               // أرسل_قناة
            llvm::Value *emitAsyncChannelRecv(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncChannelRecv(inst); }               // استقبل_قناة
            llvm::Value *emitAsyncChannelClose(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncChannelClose(inst); }             // أغلق_قناة
            llvm::Value *emitAsyncChannelIsClosed(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncChannelIsClosed(inst); }       // هل_القناة_مغلقة
            llvm::Value *emitAsyncChannelHasData(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncChannelHasData(inst); }         // هل_القناة_تحتوي_بيانات
            llvm::Value *emitAsyncChannelSize(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncChannelSize(inst); }               // حجم_القناة
            llvm::Value *emitAsyncChannelCapacity(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncChannelCapacity(inst); }       // سعة_القناة
            llvm::Value *emitAsyncChannelTrySend(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncChannelTrySend(inst); }         // حاول_ارسل
            llvm::Value *emitAsyncChannelTryRecv(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncChannelTryRecv(inst); }         // حاول_استقبل
            llvm::Value *emitAsyncChannelSendTimeout(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncChannelSendTimeout(inst); } // أرسل_بمهلة
            llvm::Value *emitAsyncChannelRecvTimeout(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncChannelRecvTimeout(inst); } // استقبل_بمهلة
            llvm::Value *emitAsyncMutexCreate(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncMutexCreate(inst); }               // أنشئ_قفل
            llvm::Value *emitAsyncMutexLock(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncMutexLock(inst); }                   // اقفل
            llvm::Value *emitAsyncMutexUnlock(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncMutexUnlock(inst); }               // افتح_قفل
            llvm::Value *emitAsyncMutexTryLock(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncMutexTryLock(inst); }             // حاول_قفل
            llvm::Value *emitAsyncMutexIsLocked(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncMutexIsLocked(inst); }           // مقفل
            llvm::Value *emitAsyncFutureIsReady(std::shared_ptr<SIRInstruction> inst) { return baf_->emitAsyncFutureIsReady(inst); }              // جاهز
            llvm::Value *emitAsyncThreadSpawn(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncThreadSpawn(inst); }               // أنشئ_خيط
            llvm::Value *emitAsyncThreadJoin(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncThreadJoin(inst); }                 // انضم_خيط
            llvm::Value *emitAsyncAtomicLoad(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncAtomicLoad(inst); }                 // حمّل_ذري
            llvm::Value *emitAsyncAtomicStore(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncAtomicStore(inst); }               // خزّن_ذري
            llvm::Value *emitAsyncAtomicAdd(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncAtomicAdd(inst); }                   // أضف_ذري
            llvm::Value *emitAsyncAtomicCAS(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncAtomicCAS(inst); }                   // قارن_وبدّل
            llvm::Value *emitAsyncWaitAll(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncWaitAll(inst); }                       // انتظر_الكل
            llvm::Value *emitAsyncWaitAny(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncWaitAny(inst); }                       // انتظر_أي
            llvm::Value *emitAsyncSelect(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncSelect(inst); }                         // اختر_قناة

            // ================================================================
            // Section 14c: WaitGroup Emit Functions / دوال إصدار مجموعة الانتظار
            // ================================================================
            llvm::Value *emitAsyncWgCreate(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncWgCreate(inst); } // مجموعة_انتظار
            llvm::Value *emitAsyncWgAdd(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncWgAdd(inst); }       // أضف (عداد)
            llvm::Value *emitAsyncWgDone(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncWgDone(inst); }     // أنهي (عداد--)
            llvm::Value *emitAsyncWgWait(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncWgWait(inst); }     // انتظر
            llvm::Value *emitAsyncWgCount(std::shared_ptr<SIRInstruction> inst) { return concur_->emitAsyncWgCount(inst); }   // العداد

            // ================================================================
            // Section 14b: LLVM Coroutine Emit Functions / دوال إصدار الكوروتين
            // ================================================================
            void emitCoroutinePreamble(std::shared_ptr<SIRFunction> sirFunc, llvm::Function *llvmFunc) { coro_->emitCoroutinePreamble(sirFunc, llvmFunc); }
            void emitCoroutineEpilogue() { coro_->emitCoroutineEpilogue(); }
            llvm::Value *emitCoroSuspend(std::shared_ptr<SIRInstruction> inst) { return coro_->emitCoroSuspend(inst); }
            llvm::Value *emitCoroReturn(std::shared_ptr<SIRInstruction> inst) { return coro_->emitCoroReturn(inst); }

            // ================================================================
            // Section 14c: Generator Emit Functions / دوال إصدار المولّد
            // ================================================================
            llvm::Value *emitGeneratorYield(std::shared_ptr<SIRInstruction> inst) { return coro_->emitGeneratorYield(inst); }
            llvm::Value *emitGeneratorConsume(std::shared_ptr<SIRInstruction> inst) { return coro_->emitGeneratorConsume(inst); }

            // ================================================================
            // Section 15: عمليات وحدات نظام التشغيل المتقدمة / Advanced OS Module Operations
            // (AR) دعم المترجم للمكتبة المنخفضة المستوى الكاملة (19 وحدة)
            // (EN) Compiler support for the full low-level library (19 modules)
            // ================================================================

            // 15a. وحدة المعالج المتقدمة / Advanced CPU Module (8)
            llvm::Value *emitLowlevelCpuGetInfo(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelCpuGetInfo(inst); }         // معلومات_المعالج
            llvm::Value *emitLowlevelCpuGetFeatures(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelCpuGetFeatures(inst); } // ميزات_المعالج
            llvm::Value *emitLowlevelCpuReadMSR(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelCpuReadMSR(inst); }         // اقرأ_سجل_نموذج
            llvm::Value *emitLowlevelCpuWriteMSR(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelCpuWriteMSR(inst); }       // اكتب_سجل_نموذج
            llvm::Value *emitLowlevelCpuReadCR(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelCpuReadCR(inst); }           // اقرأ_سجل_تحكم
            llvm::Value *emitLowlevelCpuWriteCR(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelCpuWriteCR(inst); }         // اكتب_سجل_تحكم
            llvm::Value *emitLowlevelCpuInvlpg(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelCpuInvlpg(inst); }           // ابطل_صفحة
            llvm::Value *emitLowlevelCpuGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelCpuGetReport(inst); }     // تقرير_المعالج
            llvm::Value *emitLowlevelSymbolAddr(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSymbolAddr(inst); }        // عنوان_رمز

            // 15b. وحدة GDT (3)
            llvm::Value *emitLowlevelGdtInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelGdtInit(inst); }           // هيئ_جدول_واصفات
            llvm::Value *emitLowlevelGdtLoad(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelGdtLoad(inst); }           // حمل_جدول_واصفات
            llvm::Value *emitLowlevelGdtGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelGdtGetReport(inst); } // تقرير_واصفات

            // 15c. وحدة الترحيل / Paging (5)
            llvm::Value *emitLowlevelPagingInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelPagingInit(inst); }           // هيئ_ترحيل
            llvm::Value *emitLowlevelPagingMap(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelPagingMap(inst); }             // رحل_صفحة
            llvm::Value *emitLowlevelPagingUnmap(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelPagingUnmap(inst); }         // الغ_ترحيل
            llvm::Value *emitLowlevelPagingFlushTlb(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelPagingFlushTlb(inst); }   // افرغ_ذاكرة_ترجمة
            llvm::Value *emitLowlevelPagingGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelPagingGetReport(inst); } // تقرير_ترحيل

            // 15d. وحدة المقاطعات المتقدمة / Advanced Interrupts/IDT (5)
            llvm::Value *emitLowlevelIdtInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelIdtInit(inst); }               // هيئ_جدول_مقاطعات
            llvm::Value *emitLowlevelIdtLoad(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelIdtLoad(inst); }               // حمل_جدول_مقاطعات
            llvm::Value *emitLowlevelIdtRegisterIsr(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelIdtRegisterIsr(inst); } // سجل_معالج_مقاطعة
            llvm::Value *emitLowlevelIdtEnableIrq(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelIdtEnableIrq(inst); }     // فعل_طلب_مقاطعة
            llvm::Value *emitLowlevelIdtGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelIdtGetReport(inst); }     // تقرير_مقاطعات

            // 15e. وحدة PCI (5)
            llvm::Value *emitLowlevelPciEnumerate(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelPciEnumerate(inst); }           // عدد_أجهزة_ناقل
            llvm::Value *emitLowlevelPciReadConfig(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelPciReadConfig(inst); }         // اقرأ_اعدادات_ناقل
            llvm::Value *emitLowlevelPciWriteConfig(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelPciWriteConfig(inst); }       // اكتب_اعدادات_ناقل
            llvm::Value *emitLowlevelPciGetDeviceCount(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelPciGetDeviceCount(inst); } // عدد_الأجهزة
            llvm::Value *emitLowlevelPciGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelPciGetReport(inst); }           // تقرير_ناقل

            // 15f. وحدة DMA المتقدمة (4)
            llvm::Value *emitLowlevelDmaInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelDmaInit(inst); }           // هيئ_نقل_مباشر
            llvm::Value *emitLowlevelDmaTransfer(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelDmaTransfer(inst); }   // ابدأ_نقل
            llvm::Value *emitLowlevelDmaStatus(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelDmaStatus(inst); }       // حالة_نقل
            llvm::Value *emitLowlevelDmaGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelDmaGetReport(inst); } // تقرير_نقل

            // 15g. وحدة الشاشة / Framebuffer (8)
            llvm::Value *emitLowlevelFbInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelFbInit(inst); }             // هيئ_شاشة
            llvm::Value *emitLowlevelFbSetPixel(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelFbSetPixel(inst); }     // ارسم_نقطة
            llvm::Value *emitLowlevelFbDrawRect(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelFbDrawRect(inst); }     // ارسم_مستطيل
            llvm::Value *emitLowlevelFbFillRect(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelFbFillRect(inst); }     // املأ_مستطيل
            llvm::Value *emitLowlevelFbDrawLine(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelFbDrawLine(inst); }     // ارسم_خط
            llvm::Value *emitLowlevelFbDrawString(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelFbDrawString(inst); } // ارسم_نص
            llvm::Value *emitLowlevelFbClear(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelFbClear(inst); }           // امسح_شاشة
            llvm::Value *emitLowlevelFbGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelFbGetReport(inst); }   // تقرير_شاشة

            // 15h. وحدة ACPI (4)
            llvm::Value *emitLowlevelAcpiInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiInit(inst); }           // هيئ_طاقة
            llvm::Value *emitLowlevelAcpiFindTable(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiFindTable(inst); } // ابحث_جدول_طاقة
            llvm::Value *emitLowlevelAcpiShutdown(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiShutdown(inst); }   // اطفئ
            llvm::Value *emitLowlevelAcpiGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiGetReport(inst); } // تقرير_طاقة

            // 15i. وحدة التزامن / Sync (8)
            llvm::Value *emitLowlevelSpinlockInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSpinlockInit(inst); }     // هيئ_قفل_دوار
            llvm::Value *emitLowlevelSpinlockLock(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSpinlockLock(inst); }     // اقفل_دوار
            llvm::Value *emitLowlevelSpinlockUnlock(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSpinlockUnlock(inst); } // افتح_قفل_دوار
            llvm::Value *emitLowlevelMutexInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelMutexInit(inst); }           // هيئ_كابح
            llvm::Value *emitLowlevelMutexLock(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelMutexLock(inst); }           // اقفل_كابح
            llvm::Value *emitLowlevelMutexUnlock(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelMutexUnlock(inst); }       // افتح_كابح
            llvm::Value *emitLowlevelSemaphoreInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSemaphoreInit(inst); }   // هيئ_اشارة
            llvm::Value *emitLowlevelBarrierInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelBarrierInit(inst); }       // هيئ_حاجز

            // 15j. وحدة المجدول / Scheduler (6)
            llvm::Value *emitLowlevelSchedInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSchedInit(inst); }                 // هيئ_مجدول
            llvm::Value *emitLowlevelSchedCreateProc(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSchedCreateProc(inst); }     // انشئ_عملية
            llvm::Value *emitLowlevelSchedCreateThread(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSchedCreateThread(inst); } // انشئ_خيط_نواة
            llvm::Value *emitLowlevelSchedYield(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSchedYield(inst); }               // تنازل
            llvm::Value *emitLowlevelSchedSleep(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSchedSleep(inst); }               // نوم_مجدول
            llvm::Value *emitLowlevelSchedGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSchedGetReport(inst); }       // تقرير_مجدول

            // 15k. وحدة الإقلاع / Boot (3)
            llvm::Value *emitLowlevelBootGetInfo(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelBootGetInfo(inst); }           // معلومات_اقلاع
            llvm::Value *emitLowlevelBootGetMemoryMap(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelBootGetMemoryMap(inst); } // خريطة_ذاكرة_اقلاع
            llvm::Value *emitLowlevelBootGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelBootGetReport(inst); }       // تقرير_اقلاع

            // 15l. وحدة نظام الملفات / VFS (7)
            llvm::Value *emitLowlevelVfsMount(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelVfsMount(inst); }         // حمل_قرص
            llvm::Value *emitLowlevelVfsUnmount(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelVfsUnmount(inst); }     // افصل_قرص
            llvm::Value *emitLowlevelVfsOpen(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelVfsOpen(inst); }           // افتح_ملف_نواة
            llvm::Value *emitLowlevelVfsRead(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelVfsRead(inst); }           // اقرأ_ملف_نواة
            llvm::Value *emitLowlevelVfsWrite(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelVfsWrite(inst); }         // اكتب_ملف_نواة
            llvm::Value *emitLowlevelVfsClose(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelVfsClose(inst); }         // اغلق_ملف_نواة
            llvm::Value *emitLowlevelVfsGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelVfsGetReport(inst); } // تقرير_ملفات

            // 15m. وحدة APIC (5)
            llvm::Value *emitLowlevelApicInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicInit(inst); }           // هيئ_متحكم_مقاطعات
            llvm::Value *emitLowlevelApicSendEoi(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicSendEoi(inst); }     // ارسل_نهاية_مقاطعة
            llvm::Value *emitLowlevelApicSendIpi(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicSendIpi(inst); }     // ارسل_مقاطعة_معالج
            llvm::Value *emitLowlevelApicSetTimer(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicSetTimer(inst); }   // اضبط_مؤقت_متحكم
            llvm::Value *emitLowlevelApicGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicGetReport(inst); } // تقرير_متحكم_مقاطعات

            // 15n. وحدة HPET (4)
            llvm::Value *emitLowlevelHpetInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelHpetInit(inst); }           // هيئ_مؤقت_دقيق
            llvm::Value *emitLowlevelHpetRead(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelHpetRead(inst); }           // اقرأ_مؤقت_دقيق
            llvm::Value *emitLowlevelHpetSleep(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelHpetSleep(inst); }         // نوم_دقيق
            llvm::Value *emitLowlevelHpetGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelHpetGetReport(inst); } // تقرير_مؤقت_دقيق

            // 15o. وحدة استدعاءات النظام / Syscall (4)
            llvm::Value *emitLowlevelSyscallInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSyscallInit(inst); }           // هيئ_استدعاءات
            llvm::Value *emitLowlevelSyscallRegister(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSyscallRegister(inst); }   // سجل_استدعاء
            llvm::Value *emitLowlevelSyscallInvoke(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSyscallInvoke(inst); }       // نفذ_استدعاء
            llvm::Value *emitLowlevelSyscallGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelSyscallGetReport(inst); } // تقرير_استدعاءات

            // 15p. عمليات الذاكرة المتقدمة / Advanced Memory (4)
            llvm::Value *emitLowlevelMemAllocPhys(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelMemAllocPhys(inst); } // خصص_فيزيائي
            llvm::Value *emitLowlevelMemFreePhys(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelMemFreePhys(inst); }   // حرر_فيزيائي
            llvm::Value *emitLowlevelMemMapRegion(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelMemMapRegion(inst); } // رحل_منطقة
            llvm::Value *emitLowlevelMemGetReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelMemGetReport(inst); } // تقرير_ذاكرة_نواة

            // =================================================================
            // القسم 16: بروتوكول UEFI / UEFI Boot Protocol (37 دالة)
            // =================================================================

            // 16a. التهيئة والتحكم (5)
            llvm::Value *emitLowlevelUefiInit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiInit(inst); }
            llvm::Value *emitLowlevelUefiExitBootServices(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiExitBootServices(inst); }
            llvm::Value *emitLowlevelUefiIsInitialized(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiIsInitialized(inst); }
            llvm::Value *emitLowlevelUefiBsExited(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiBsExited(inst); }
            llvm::Value *emitLowlevelUefiResetSystem(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiResetSystem(inst); }

            // 16b. إدارة الذاكرة (7)
            llvm::Value *emitLowlevelUefiAllocPages(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiAllocPages(inst); }
            llvm::Value *emitLowlevelUefiFreePages(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiFreePages(inst); }
            llvm::Value *emitLowlevelUefiAllocPool(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiAllocPool(inst); }
            llvm::Value *emitLowlevelUefiFreePool(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiFreePool(inst); }
            llvm::Value *emitLowlevelUefiGetMemoryMap(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiGetMemoryMap(inst); }
            llvm::Value *emitLowlevelUefiGetMemmapKey(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiGetMemmapKey(inst); }
            llvm::Value *emitLowlevelUefiTotalMemory(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiTotalMemory(inst); }

            // 16c. بروتوكول الرسوميات GOP (10)
            llvm::Value *emitLowlevelUefiInitGop(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiInitGop(inst); }
            llvm::Value *emitLowlevelUefiSetGopMode(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiSetGopMode(inst); }
            llvm::Value *emitLowlevelUefiQueryGopMode(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiQueryGopMode(inst); }
            llvm::Value *emitLowlevelUefiGopModeCount(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiGopModeCount(inst); }
            llvm::Value *emitLowlevelUefiCurrentGopMode(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiCurrentGopMode(inst); }
            llvm::Value *emitLowlevelUefiFramebufferBase(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiFramebufferBase(inst); }
            llvm::Value *emitLowlevelUefiFramebufferSize(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiFramebufferSize(inst); }
            llvm::Value *emitLowlevelUefiFillScreen(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiFillScreen(inst); }
            llvm::Value *emitLowlevelUefiDrawRect(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiDrawRect(inst); }
            llvm::Value *emitLowlevelUefiGopBlt(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiGopBlt(inst); }

            // 16d. خدمات وقت التشغيل (4)
            llvm::Value *emitLowlevelUefiGetTime(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiGetTime(inst); }
            llvm::Value *emitLowlevelUefiSetTime(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiSetTime(inst); }
            llvm::Value *emitLowlevelUefiGetVariable(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiGetVariable(inst); }
            llvm::Value *emitLowlevelUefiSetVariable(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiSetVariable(inst); }

            // 16e. نظام الملفات (6)
            llvm::Value *emitLowlevelUefiOpenVolume(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiOpenVolume(inst); }
            llvm::Value *emitLowlevelUefiOpenFile(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiOpenFile(inst); }
            llvm::Value *emitLowlevelUefiReadFile(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiReadFile(inst); }
            llvm::Value *emitLowlevelUefiWriteFile(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiWriteFile(inst); }
            llvm::Value *emitLowlevelUefiCloseFile(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiCloseFile(inst); }
            llvm::Value *emitLowlevelUefiFileInfo(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiFileInfo(inst); }

            // 16f. بروتوكولات ومعلومات (5)
            llvm::Value *emitLowlevelUefiLocateProtocol(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiLocateProtocol(inst); }
            llvm::Value *emitLowlevelUefiRevision(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiRevision(inst); }
            llvm::Value *emitLowlevelUefiVendor(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiVendor(inst); }
            llvm::Value *emitLowlevelUefiFwRevision(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiFwRevision(inst); }
            llvm::Value *emitLowlevelUefiReport(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelUefiReport(inst); }

            // --- القسم 17: ACPI الموسّع / Extended ACPI ---
            llvm::Value *emitLowlevelAcpiInitFull(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiInitFull(inst); }
            llvm::Value *emitLowlevelAcpiInitRsdp(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiInitRsdp(inst); }
            llvm::Value *emitLowlevelAcpiEnable(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiEnable(inst); }
            llvm::Value *emitLowlevelAcpiDisable(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiDisable(inst); }
            llvm::Value *emitLowlevelAcpiIsInitialized(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiIsInitialized(inst); }
            llvm::Value *emitLowlevelAcpiVersion(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiVersion(inst); }
            llvm::Value *emitLowlevelAcpiReboot(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiReboot(inst); }
            llvm::Value *emitLowlevelAcpiSleep(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiSleep(inst); }
            llvm::Value *emitLowlevelAcpiDelayUs(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiDelayUs(inst); }
            llvm::Value *emitLowlevelAcpiReadPmTimer(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiReadPmTimer(inst); }
            llvm::Value *emitLowlevelAcpiIsPm32bit(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiIsPm32bit(inst); }
            llvm::Value *emitLowlevelAcpiProcessorCount(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiProcessorCount(inst); }
            llvm::Value *emitLowlevelAcpiLocalApicAddr(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiLocalApicAddr(inst); }
            llvm::Value *emitLowlevelAcpiEcamBase(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelAcpiEcamBase(inst); }

            // --- القسم 18: APIC الموسّع / Extended APIC ---
            llvm::Value *emitLowlevelApicSupported(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicSupported(inst); }
            llvm::Value *emitLowlevelApicX2Supported(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicX2Supported(inst); }
            llvm::Value *emitLowlevelApicId(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicId(inst); }
            llvm::Value *emitLowlevelApicIoCount(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicIoCount(inst); }
            llvm::Value *emitLowlevelApicInitTimer(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicInitTimer(inst); }
            llvm::Value *emitLowlevelApicStartTimer(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicStartTimer(inst); }
            llvm::Value *emitLowlevelApicStopTimer(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicStopTimer(inst); }
            llvm::Value *emitLowlevelApicTimerCount(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicTimerCount(inst); }
            llvm::Value *emitLowlevelApicCalibrate(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicCalibrate(inst); }
            llvm::Value *emitLowlevelApicSetPriority(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicSetPriority(inst); }
            llvm::Value *emitLowlevelApicDisablePic(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicDisablePic(inst); }
            llvm::Value *emitLowlevelApicMaskIrq(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicMaskIrq(inst); }
            llvm::Value *emitLowlevelApicUnmaskIrq(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicUnmaskIrq(inst); }
            llvm::Value *emitLowlevelApicRouteIrq(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicRouteIrq(inst); }
            llvm::Value *emitLowlevelApicSendIpiAll(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicSendIpiAll(inst); }
            llvm::Value *emitLowlevelApicSendInitIpi(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicSendInitIpi(inst); }
            llvm::Value *emitLowlevelApicSendSipi(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicSendSipi(inst); }
            llvm::Value *emitLowlevelApicWaitDelivery(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicWaitDelivery(inst); }
            llvm::Value *emitLowlevelApicInitIo(std::shared_ptr<SIRInstruction> inst) { return ll_->emitLowlevelApicInitIo(inst); }

            // Helper for declaring runtime functions
            llvm::Function *declareRuntimeFunction(const std::string &name,
                                                   llvm::Type *returnType,
                                                   const std::vector<llvm::Type *> &argTypes);

            // Helper for resolving SIR operands to LLVM values (Phase 7 Step 1: delegate)
            llvm::Value *resolveOperand(const SIROperand &operand) { return arith_->resolveOperand(operand); }

            // ------------------------------------------------------------------------
            // Type Conversion Instructions / تعليمات تحويل الأنواع
            // (AR) Phase 7 Step 1: emitCast + 8 SIR conversions delegated
            // ------------------------------------------------------------------------

            llvm::Value *emitCast(std::shared_ptr<SIRInstruction> inst) { return arith_->emitCast(inst); }
            llvm::Value *emitBitCast(std::shared_ptr<SIRInstruction> inst) { return arith_->emitBitCast(inst); }
            llvm::Value *emitIntToPtr(std::shared_ptr<SIRInstruction> inst) { return arith_->emitIntToPtr(inst); }
            llvm::Value *emitPtrToInt(std::shared_ptr<SIRInstruction> inst) { return arith_->emitPtrToInt(inst); }
            llvm::Value *emitTrunc(std::shared_ptr<SIRInstruction> inst) { return arith_->emitTrunc(inst); }
            llvm::Value *emitZExt(std::shared_ptr<SIRInstruction> inst) { return arith_->emitZExt(inst); }
            llvm::Value *emitSExt(std::shared_ptr<SIRInstruction> inst) { return arith_->emitSExt(inst); }

            // SIR Type Conversion opcodes (Phase 7 Step 1: delegate)
            llvm::Value *emitI64ToF64(std::shared_ptr<SIRInstruction> inst) { return arith_->emitI64ToF64(inst); }
            llvm::Value *emitF64ToI64(std::shared_ptr<SIRInstruction> inst) { return arith_->emitF64ToI64(inst); }
            llvm::Value *emitF64ToI64SatOp(std::shared_ptr<SIRInstruction> inst) { return arith_->emitF64ToI64SatOp(inst); } // عشري→صحيح مُشبَع (دلالة البتّيّات الموحَّدة منصّيًّا)
            llvm::Value *emitI64ToBool(std::shared_ptr<SIRInstruction> inst) { return arith_->emitI64ToBool(inst); }
            llvm::Value *emitBoolToI64(std::shared_ptr<SIRInstruction> inst) { return arith_->emitBoolToI64(inst); }
            llvm::Value *emitI64ToString(std::shared_ptr<SIRInstruction> inst) { return arith_->emitI64ToString(inst); }
            llvm::Value *emitF64ToString(std::shared_ptr<SIRInstruction> inst) { return arith_->emitF64ToString(inst); }
            llvm::Value *emitBoolToString(std::shared_ptr<SIRInstruction> inst) { return arith_->emitBoolToString(inst); }
            llvm::Value *emitArrayToString(std::shared_ptr<SIRInstruction> inst) { return arith_->emitArrayToString(inst); }

            // ------------------------------------------------------------------------
            // OOP Instructions / تعليمات البرمجة الكائنية
            // ------------------------------------------------------------------------
            llvm::Value *emitObjectNew(std::shared_ptr<SIRInstruction> inst) { return oop_->emitObjectNew(inst); }                // إنشاء كائن
            llvm::Value *emitObjectGet(std::shared_ptr<SIRInstruction> inst) { return oop_->emitObjectGet(inst); }                // قراءة خاصية
            llvm::Value *emitObjectSet(std::shared_ptr<SIRInstruction> inst) { return oop_->emitObjectSet(inst); }                // تعيين خاصية
            llvm::Value *emitObjectCall(std::shared_ptr<SIRInstruction> inst) { return objarr_->emitObjectCall(inst); }           // استدعاء طريقة
            llvm::Value *emitInstanceOf(std::shared_ptr<SIRInstruction> inst) { return objarr_->emitInstanceOf(inst); }           // تحقق النوع
            llvm::Value *emitObjectCast(std::shared_ptr<SIRInstruction> inst) { return objarr_->emitObjectCast(inst); }           // تحويل كائن
            llvm::Value *emitClassDef(std::shared_ptr<SIRInstruction> inst) { return objarr_->emitClassDef(inst); }               // تعريف صنف
            llvm::Value *emitMethodDef(std::shared_ptr<SIRInstruction> inst) { return objarr_->emitMethodDef(inst); }             // تعريف طريقة
            llvm::Value *emitFieldDef(std::shared_ptr<SIRInstruction> inst) { return objarr_->emitFieldDef(inst); }               // تعريف حقل
            llvm::Value *emitConstructorCall(std::shared_ptr<SIRInstruction> inst) { return objarr_->emitConstructorCall(inst); } // استدعاء منشئ

            // ========================================================================
            // (AR) أنواع البيانات الجبرية — تعدادات ADT
            // (EN) Algebraic Data Types — ADT Enums
            // ========================================================================
            llvm::Value *emitEnumConstruct(std::shared_ptr<SIRInstruction> inst) { return enumops_->emitEnumConstruct(inst); }
            llvm::Value *emitEnumGetTag(std::shared_ptr<SIRInstruction> inst) { return enumops_->emitEnumGetTag(inst); }
            llvm::Value *emitEnumGetPayload(std::shared_ptr<SIRInstruction> inst) { return enumops_->emitEnumGetPayload(inst); }
            llvm::Value *emitEnumIsVariant(std::shared_ptr<SIRInstruction> inst) { return enumops_->emitEnumIsVariant(inst); }
            llvm::Value *emitEnumFree(std::shared_ptr<SIRInstruction> inst) { return enumops_->emitEnumFree(inst); }

            // ========================================================================
            // vtable & Virtual Dispatch / جدول الدوال الافتراضية
            // ========================================================================

            /// (AR) بناء vtables لجميع الأصناف بعد preprocessClasses
            /// (EN) Build vtables for all classes after preprocessClasses
            void buildClassVtables(std::shared_ptr<SIRModule> sirModule) { cls_->buildClassVtables(sirModule); }

            /// (AR) تحديث مداخل vtable المؤجلة بعد إصدار جميع الدوال
            /// (EN) Patch deferred vtable entries after all functions are emitted
            void patchClassVtables() { cls_->patchClassVtables(); }

            /// (AR) استدعاء افتراضي عبر vtable
            /// (EN) Virtual dispatch via vtable
            llvm::Value *emitVirtualCall(llvm::Value *objPtr, const std::string &className,
                                         const std::string &methodName,
                                         const std::vector<llvm::Value *> &extraArgs)
            {
                return cls_->emitVirtualCall(objPtr, className, methodName, extraArgs);
            }

            /// (AR) تخزين مؤشر vtable في الحقل 0 من الكائن
            /// (EN) Store vtable pointer in field 0 of object
            void storeVtablePtr(llvm::Value *objPtr, const std::string &className) { cls_->storeVtablePtr(objPtr, className); }

            /// (AR) استدعاء دالة الهدم للكائن
            /// (EN) Call destructor for object
            void emitDestructorCall(llvm::Value *objPtr, const std::string &className) { cls_->emitDestructorCall(objPtr, className); }

            /// (AR) الحصول على إزاحة الحقول بسبب vtable
            /// (EN) Get field offset due to vtable pointer at index 0
            int getFieldStructIndex(const std::string &className, int userFieldIndex) const { return cls_->getFieldStructIndex(className, userFieldIndex); }

            /// (AR) تصنيف ABI لتمرير/إرجاع بنية @تمثيل_سي بالقيمة عبر حدّ FFI [RFC #53 F2-ج]
            /// (EN) ABI classification for by-value C-repr struct pass/return at FFI boundary [RFC #53 F2-ج]
            CReprAbiInfo classifyCReprAbi(const std::string &className) const { return cls_->classifyCReprAbi(className); }

            // ------------------------------------------------------------------------
            // Missing Bitwise / عمليات ثنائية ناقصة
            // ------------------------------------------------------------------------
            llvm::Value *emitSar(std::shared_ptr<SIRInstruction> inst) { return closure_->emitSar(inst); } // إزاحة حسابية يمين
            llvm::Value *emitRol(std::shared_ptr<SIRInstruction> inst) { return closure_->emitRol(inst); } // دوران يسار

            // ------------------------------------------------------------------------
            // Missing Memory / عمليات ذاكرة ناقصة
            // ------------------------------------------------------------------------
            llvm::Value *emitCallIndirect(std::shared_ptr<SIRInstruction> inst) { return closure_->emitCallIndirect(inst); } // استدعاء غير مباشر
            llvm::Value *emitAllocHeap(std::shared_ptr<SIRInstruction> inst) { return closure_->emitAllocHeap(inst); }       // تخصيص كومة
            llvm::Value *emitFreeMem(std::shared_ptr<SIRInstruction> inst) { return closure_->emitFreeMem(inst); }           // تحرير ذاكرة
            llvm::Value *emitAddr(std::shared_ptr<SIRInstruction> inst) { return oop_->emitAddr(inst); }                     // عنوان متغير
            llvm::Value *emitPtrAdd(std::shared_ptr<SIRInstruction> inst) { return oop_->emitPtrAdd(inst); }                 // حساب مؤشرات
            llvm::Value *emitPtrCast(std::shared_ptr<SIRInstruction> inst) { return oop_->emitPtrCast(inst); }               // تحويل مؤشر

            // ------------------------------------------------------------------------
            // Closures / الإغلاقات
            // ------------------------------------------------------------------------
            llvm::Value *emitClosureCreate(std::shared_ptr<SIRInstruction> inst) { return closure_->emitClosureCreate(inst); } // إنشاء بنية إغلاق
            llvm::Value *emitClosureCall(std::shared_ptr<SIRInstruction> inst) { return closure_->emitClosureCall(inst); }     // استدعاء إغلاق
            llvm::Value *emitEnvLoad(std::shared_ptr<SIRInstruction> inst) { return closure_->emitEnvLoad(inst); }             // تحميل من بيئة إغلاق
            llvm::Value *emitEnvStore(std::shared_ptr<SIRInstruction> inst) { return closure_->emitEnvStore(inst); }           // [Fix #51] تخزين في بيئة إغلاق

            // ------------------------------------------------------------------------
            // Array core / عمليات المصفوفات الأساسية
            // ------------------------------------------------------------------------
            // (AR) Phase 7 Step 5: delegate إلى ArrayOpsCodeGen
            llvm::Value *emitArrayNew(std::shared_ptr<SIRInstruction> inst) { return arr_->emitArrayNew(inst); }
            llvm::Value *emitArrayGet(std::shared_ptr<SIRInstruction> inst) { return arr_->emitArrayGet(inst); }
            llvm::Value *emitArraySet(std::shared_ptr<SIRInstruction> inst) { return arr_->emitArraySet(inst); }
            llvm::Value *emitArrayLen(std::shared_ptr<SIRInstruction> inst) { return arr_->emitArrayLen(inst); }
            llvm::Value *emitArrayConcat(std::shared_ptr<SIRInstruction> inst) { return arr_->emitArrayConcat(inst); }
            llvm::Value *emitArrayZip(std::shared_ptr<SIRInstruction> inst) { return arr_->emitArrayZip(inst); } // زاوج — اقتران مصفوفتين أزواجًا

            // String core
            llvm::Value *emitStringNew(std::shared_ptr<SIRInstruction> inst) { return arr_->emitStringNew(inst); }

            // Builtin Extra — (AR) Phase 7 Step 8: Min/Max delegate إلى MathBuiltinsCodeGen
            llvm::Value *emitBuiltinMin(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinMin(inst); }
            llvm::Value *emitBuiltinMax(std::shared_ptr<SIRInstruction> inst) { return mathb_->emitBuiltinMax(inst); }
            llvm::Value *emitBuiltinAssert(std::shared_ptr<SIRInstruction> inst) { return baf_->emitBuiltinAssert(inst); } // تأكيد
            llvm::Value *emitBuiltinDebug(std::shared_ptr<SIRInstruction> inst) { return baf_->emitBuiltinDebug(inst); }   // تنقيح

            // ------------------------------------------------------------------------
            // Aggregate Instructions / تعليمات التجميع
            // ------------------------------------------------------------------------

            // (AR) Phase 7 Step 4: delegate إلى AggregateOpsCodeGen
            llvm::Value *emitExtractValue(std::shared_ptr<SIRInstruction> inst) { return agg_->emitExtractValue(inst); }
            llvm::Value *emitInsertValue(std::shared_ptr<SIRInstruction> inst) { return agg_->emitInsertValue(inst); }
            llvm::Value *emitExtractElement(std::shared_ptr<SIRInstruction> inst) { return agg_->emitExtractElement(inst); }
            llvm::Value *emitInsertElement(std::shared_ptr<SIRInstruction> inst) { return agg_->emitInsertElement(inst); }

            // ------------------------------------------------------------------------
            // Phi & Select / فاي والاختيار
            // ------------------------------------------------------------------------

            llvm::Value *emitPhi(std::shared_ptr<SIRInstruction> inst) { return baf_->emitPhi(inst); } // عقدة فاي / Phi node
            llvm::Value *emitSelect(std::shared_ptr<SIRInstruction> inst) { return agg_->emitSelect(inst); }

            // ========================================================================
            // Type Conversion / تحويل الأنواع
            // ========================================================================

            /**
             * تحويل نوع Sad إلى نوع LLVM
             * Convert Sad type to LLVM type
             *
             * @param sadType نوع Sad / Sad type
             * @return نوع LLVM / LLVM type
             */
            // ====================================================================
            // (AR) Phase 9: تحويل الأنواع — مفوّض إلى TypesCodeGen
            // (EN) Phase 9: Type conversions — delegated to TypesCodeGen
            // ====================================================================
            llvm::Type *convertType(std::shared_ptr<Type> sadType) { return types_->convertType(sadType); }

            llvm::FunctionType *convertFunctionType(std::shared_ptr<Type> returnType,
                                                    const std::vector<std::shared_ptr<Type>> &paramTypes,
                                                    bool isVarArg = false)
            {
                return types_->convertFunctionType(returnType, paramTypes, isVarArg);
            }

            /**
             * الحصول على الأنواع الأساسية / Get primitive types (Phase 9 → TypesCodeGen)
             */
            llvm::Type *getVoidType() { return types_->getVoidType(); }
            llvm::Type *getInt1Type() { return types_->getInt1Type(); }
            llvm::Type *getInt8Type() { return types_->getInt8Type(); }
            llvm::Type *getInt16Type() { return types_->getInt16Type(); }
            llvm::Type *getInt32Type() { return types_->getInt32Type(); }
            llvm::Type *getInt64Type() { return types_->getInt64Type(); }
            llvm::Type *getFloatType() { return types_->getFloatType(); }
            llvm::Type *getDoubleType() { return types_->getDoubleType(); }
            llvm::Type *getInt8PtrType() { return types_->getInt8PtrType(); }

            // ========================================================================
            // Constants / الثوابت — Phase 9 → TypesCodeGen
            // ========================================================================

            llvm::Constant *getConstantInt(int64_t value, int bits = 64) { return types_->getConstantInt(value, bits); }
            llvm::Constant *getConstantFloat(double value, bool isDouble = false) { return types_->getConstantFloat(value, isDouble); }
            llvm::Constant *getConstantString(const std::string &value) { return types_->getConstantString(value); }
            llvm::Constant *getConstantBool(bool value) { return types_->getConstantBool(value); }
            llvm::Constant *getNullPtr(llvm::Type *ptrType) { return types_->getNullPtr(ptrType); }

            // ========================================================================
            // (AR) قيم الموزّع الإشاريّة (Sentinels) — فصل «عُولج» عن «opcode غير معروف»
            //      الموزّع (emitInstruction) يفسّر nullptr «لم تتعرّف الطبقة على الـopcode
            //      ⇒ جرّب التالية»، فمعالجٌ يبلّغ خطأً حقيقيًّا (أو ينجح بلا قيمة) ثمّ
            //      يعيد nullptr كان يُسقط الموزّع عبر الطبقات فيطبع «Unsupported opcode»
            //      بائتًا فوق الخطأ الأصليّ (النمط المثبَت قبل #185/#188).
            // (EN) Dispatcher sentinels — separate "handled" from "unknown opcode".
            //      The dispatcher treats nullptr as "tier didn't recognize the opcode ⇒
            //      try the next tier", so a handler that reported a real error (or
            //      succeeded with no value) and returned nullptr made the dispatcher
            //      fall through and print a spurious "Unsupported opcode" on top of
            //      the original diagnostic (the pattern fixed in #185/#188).
            // ========================================================================

            /// (AR) «عُولج بنجاح، لا قيمة ذات معنى» — للمدمجات بلا سجلّ نتيجة (اطبع/نم/اخرج).
            /// (EN) "Handled successfully, no meaningful value" — for result-less builtins.
            llvm::Value *builtinHandledSentinel()
            {
                return llvm::ConstantInt::get(getInt64Type(), 0);
            }

            /// (AR) «عُولج وأُبلغ خطأ حقيقيّ» — يربط سجلّ النتيجة (إن وُجد) بثابت صفريّ
            ///      (نفس احتياط resolveOperand) فلا تتتالى «سجلّ غير معرَّف» على المستهلكين،
            ///      ثمّ يعيد الإشارة فيتوقّف الموزّع بلا بلاغ opcode زائف. يُستدعى بعد reportError.
            /// (EN) "Handled with a reported error" — binds the result register (if any)
            ///      to a zero constant (resolveOperand's own fallback) to avoid cascading
            ///      "undefined register" errors, then returns the sentinel so the
            ///      dispatcher stops without a spurious opcode report. Call after reportError.
            llvm::Value *builtinErrorSentinel(const std::shared_ptr<SIRInstruction> &inst)
            {
                if (inst && inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] =
                        llvm::ConstantInt::get(getInt64Type(), 0);
                }
                return builtinHandledSentinel();
            }

            // ========================================================================
            // Output / الإخراج
            // ========================================================================

            /**
             * حفظ LLVM IR إلى ملف نصي
             * Save LLVM IR to text file
             *
             * @param filename اسم الملف / File name
             * @return true إذا نجح الحفظ / true if save succeeded
             */
            bool emitToFile(const std::string &filename) const { return out_->emitToFile(filename); }

            /**
             * حفظ LLVM IR إلى نص
             * Save LLVM IR to string
             *
             * @return نص LLVM IR / LLVM IR text
             */
            std::string emitToString() const { return out_->emitToString(); }

            /**
             * إصدار ملف assembly
             * Emit assembly file
             *
             * @param filename اسم الملف / File name
             * @return true if successful
             */
            bool emitAssembly(const std::string &filename) { return out_->emitAssembly(filename); }

            /**
             * إصدار ملف assembly من وحدة خارجية
             * Emit assembly file from external module
             *
             * @param filename اسم الملف / File name
             * @param module الوحدة / Module
             * @return true if successful
             */
            bool emitAssembly(const std::string &filename, llvm::Module *module) { return out_->emitAssembly(filename, module); }

            /**
             * إصدار ملف object
             * Emit object file
             *
             * @param filename اسم الملف / File name
             * @return true if successful
             */
            bool emitObjectFile(const std::string &filename) { return out_->emitObjectFile(filename); }

            /**
             * إصدار ملف object من وحدة خارجية
             * Emit object file from external module
             *
             * @param filename اسم الملف / File name
             * @param module الوحدة / Module
             * @return true if successful
             */
            bool emitObjectFile(const std::string &filename, llvm::Module *module) { return out_->emitObjectFile(filename, module); }

            /**
             * طباعة LLVM IR إلى stderr
             * Print LLVM IR to stderr
             */
            void dump() const { out_->dump(); }

            // ========================================================================
            // Error Handling / معالجة الأخطاء
            // ========================================================================
            // (AR) reportError() / hasErrors() / getErrors() مُعرَّفة في
            //      LLVMCodeGenContext (القاعدة) — لا حاجة لإعادة الإعلان هنا.
            // (EN) reportError() / hasErrors() / getErrors() are defined in the
            //      LLVMCodeGenContext base — no need to redeclare here.

        private:
            // ========================================================================
            // Private Members / الأعضاء الخاصة
            // ========================================================================
            // (AR) الحقول المنقولة إلى LLVMCodeGenContext (Phase 7 Step 0):
            //      context_, module_, builder_, targetMachine_, typeMapper_,
            //      optimizer_, sirModule_, optimizationLevel_, autoOptimize_,
            //      moduleMode_, freestanding_, hasErrors_, errors_
            // (EN) Fields relocated to LLVMCodeGenContext (Phase 7 Step 0).

        public:
            // (AR) Phase 8 Cleanup: نُقل context_info_ إلى public ليصل إليه
            //      sub-codegens مباشرة بدون الحاجة إلى friend declarations.
            //      هو per-function state وليس shared state، لذا يبقى في
            //      LLVMCodeGen وليس في LLVMCodeGenContext base.
            // (EN) Phase 8 Cleanup: context_info_ moved to public so sub-codegens
            //      can access it directly without needing friend declarations.
            //      It's per-function state (not shared base state), so it stays
            //      in LLVMCodeGen rather than LLVMCodeGenContext.
            CodeGenContext context_info_;

        private:
            // (AR) Phase 7 Step 1: مكوّن فرعي للعمليات الحسابية والمقارنات والتحويلات
            // (EN) Phase 7 Step 1: sub-codegen for arithmetic, comparisons & conversions
            std::unique_ptr<ArithmeticCodeGen> arith_;

            // (AR) Phase 7 Step 2: مكوّن فرعي لعمليات الذاكرة (Load/Store/Alloca/GEP/Move)
            // (EN) Phase 7 Step 2: sub-codegen for memory operations
            std::unique_ptr<MemoryCodeGen> mem_;

            // (AR) Phase 7 Step 3: مكوّن فرعي لتدفق التحكم (Branch/CondBranch/Call/Return/Switch)
            std::unique_ptr<ControlFlowCodeGen> cf_;

            // (AR) Phase 7 Step 4: مكوّن فرعي لعمليات التجميع (ExtractValue/InsertValue/ExtractElement/InsertElement/Select)
            std::unique_ptr<AggregateOpsCodeGen> agg_;

            // (AR) Phase 7 Step 5: مكوّن فرعي لعمليات المصفوفات (Array New/Get/Set/Len/Concat + StringNew + bounds helpers)
            std::unique_ptr<ArrayOpsCodeGen> arr_;

            // (AR) Phase 7 Step 6: مكوّن فرعي لعمليات النصوص (17 method)
            std::unique_ptr<StringOpsCodeGen> strops_;

            // (AR) Phase 7 Step 7: مكوّن فرعي لدوال المصفوفات المضمنة (8 methods)
            std::unique_ptr<ArrayBuiltinsCodeGen> arrb_;

            // (AR) Phase 7 Step 8: مكوّن فرعي لدوال الرياضيات (21 methods: Min/Max + Sqrt..Clamp + Random)
            std::unique_ptr<MathBuiltinsCodeGen> mathb_;

            // (AR) Phase 7 Step 9: مكوّن فرعي لعمليات الخرائط (3 methods: emitCallMap + 2 helpers)
            std::unique_ptr<MapOpsCodeGen> mapops_;

            // (AR) Phase 7 Step 10: مكوّن فرعي للاستثناءات (2 methods: emitCallException + markSetjmpGlobalsVolatile)
            std::unique_ptr<ExceptionCodeGen> exc_;

            // (AR) Phase 7 Step 11: مكوّن فرعي لتعليمات منخفضة المستوى (153 methods: CPU/UEFI/ACPI/APIC/...)
            std::unique_ptr<LowlevelCodeGen> ll_;

            // (AR) Phase 7 Step 12: مكوّن فرعي لعمليات التعدادات ADT (5 methods)
            std::unique_ptr<EnumOpsCodeGen> enumops_;

            // (AR) Phase 7 Step 13: مكوّن فرعي لـ OS/Embedded/FFI (51 methods)
            std::unique_ptr<HardwareFFICodeGen> hwffi_;

            // (AR) Phase 7 Step 14: مكوّن فرعي للإغلاقات والاستدعاءات غير المباشرة (9 methods)
            std::unique_ptr<ClosureCodeGen> closure_;

            // (AR) Phase 7 Step 15: مكوّنات فرعية لـ IO/Security/FFI builtins (20 methods total)
            std::unique_ptr<IOBuiltinsCodeGen> iob_;
            std::unique_ptr<SecurityBuiltinsCodeGen> secb_;
            std::unique_ptr<FFIRemainCodeGen> ffir_;

            // (AR) Phase 7 Step 16: مكوّن فرعي لدوال البيئة بدون نظام تشغيل (19 methods)
            std::unique_ptr<FreestandingCodeGen> freest_;

            // (AR) Phase 7 Step 17: عمليات الكائنات (7 methods)
            std::unique_ptr<ObjectsArraysCodeGen> objarr_;
            // (AR) Phase 7 Step 18: عمليات OOP والمؤشرات (6 methods)
            std::unique_ptr<OOPOpsCodeGen> oop_;
            // (AR) Phase 8 Step 1: تزامن وقنوات (31 methods)
            std::unique_ptr<ConcurrencyCodeGen> concur_;
            // (AR) Phase 8 Step 2: عناصر واجهة المستخدم (40 methods)
            std::unique_ptr<UICodeGen> ui_;
            // (AR) Phase 8 Step 3: دورة حياة الأصناف وvtables (9 methods)
            std::unique_ptr<ClassesVtablesCodeGen> cls_;
            // (AR) Phase 8 Step 4: إصدار الدوال والتحسين والتحقق (12 methods)
            std::unique_ptr<FunctionsCodeGen> fns_;
            // (AR) Phase 8 Step 5: دوال مدمجة + رياضيات/غير متزامن (19 methods)
            std::unique_ptr<BuiltinFuncsCodeGen> baf_;
            // (AR) Phase 8 Step 6: دوال الشبكة المدمجة (2 methods كبيرة)
            std::unique_ptr<NetworkBuiltinsCodeGen> nb_;
            std::unique_ptr<ProcessesBuiltinsCodeGen> proc_;
            // (AR) Phase 8 Step 7: الكوروتينات والمولّدات (6 methods)
            std::unique_ptr<CoroutinesCodeGen> coro_;
            // (AR) Phase 8 Step 8: نصوص + helpers (5 methods)
            std::unique_ptr<StringsCodeGen> strs_;
            // (AR) Phase 8 Step 9: emitInstructionCore (29KB switch)
            std::unique_ptr<InstrCoreCodeGen> ic_;
            // (AR) Phase 8 Step 10
            std::unique_ptr<SimdCodeGen> simd_;
            std::unique_ptr<InstrLowlevelCodeGen> ill_;
            std::unique_ptr<FileCastsCodeGen> fc_;
            std::unique_ptr<DirectivesCodeGen> dir_;
            std::unique_ptr<InstrPlatformCodeGen> ip_;
            std::unique_ptr<OutputCodeGen> out_;
            // (AR) Phase 9: مكوّن فرعي للأنواع والثوابت والتحويلات
            // (EN) Phase 9: Types, constants & conversions sub-codegen
            std::unique_ptr<TypesCodeGen> types_;

            // ========================================================================
            // Helper Methods / دوال مساعدة
            // ========================================================================
            // (AR) getNamedValue/setNamedValue/createEntryBlockAlloca أُزيلت في
            //      Phase 7 Step 0 (إعلانات ميتة — لم تُستدعَ أبداً ولم تُعرّف).
            // (EN) getNamedValue/setNamedValue/createEntryBlockAlloca removed in
            //      Phase 7 Step 0 (dead declarations — never called, never defined).

            /**
             * الحصول على الكتلة الحالية
             * Get current block
             */
            llvm::BasicBlock *getCurrentBlock() const { return context_info_.currentBlock; }

            /**
             * تعيين الكتلة الحالية
             * Set current block
             */
            void setCurrentBlock(llvm::BasicBlock *block) { context_info_.currentBlock = block; }

            // ========================================================================
            // القسم 20: دوال Android Runtime
            // Section 20: Android Runtime Functions
            // ========================================================================

            /**
             * استدعاء دالة Runtime خارجية لأندرويد
             * Call an external Android runtime function
             *
             * @param funcName اسم الدالة / Function name
             * @param retType نوع القيمة المُرجعة / Return type
             * @param argTypes أنواع المعاملات / Argument types
             * @param argValues قيم المعاملات / Argument values
             * @return القيمة المُرجعة من الدالة / Return value
             */
            llvm::Value *emitAndroidRuntimeCall(
                const std::string &funcName,
                llvm::Type *retType,
                const std::vector<llvm::Type *> &argTypes,
                const std::vector<llvm::Value *> &argValues);

            // 20a. إدارة الذاكرة / Memory Management
            llvm::Value *emitAndroidAlloc(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidFree(std::shared_ptr<SIRInstruction> inst);

            // 20b. النصوص / Strings
            llvm::Value *emitAndroidStringCreate(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidStringConcat(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidStringLength(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidStringSubstr(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidStringCompare(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidStringFree(std::shared_ptr<SIRInstruction> inst);

            // 20c. المصفوفات / Arrays
            llvm::Value *emitAndroidArrayCreate(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidArrayGet(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidArraySet(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidArrayLength(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidArrayPush(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidArrayPop(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidArrayFree(std::shared_ptr<SIRInstruction> inst);

            // 20d. الخرائط / Maps
            llvm::Value *emitAndroidMapCreate(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidMapGet(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidMapSet(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidMapHas(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidMapDelete(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidMapSize(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidMapFree(std::shared_ptr<SIRInstruction> inst);

            // 20e. الشبكات / Network
            llvm::Value *emitAndroidNetConnect(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidNetSend(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidNetRecv(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidNetClose(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidHttpRequest(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidWsConnect(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidWsSend(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidWsRecv(std::shared_ptr<SIRInstruction> inst);

            // 20f. الخيوط / Threading
            llvm::Value *emitAndroidThreadCreate(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidThreadJoin(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidMutexCreate(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidMutexLock(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidMutexUnlock(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidChannelCreate(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidChannelSend(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidChannelRecv(std::shared_ptr<SIRInstruction> inst);

            // 20g. واجهة المستخدم / UI
            llvm::Value *emitAndroidUiInit(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidUiCreateWidget(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidUiSetText(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidUiSetCallback(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidUiShow(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidUiHide(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidUiUpdate(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidUiRun(std::shared_ptr<SIRInstruction> inst);

            // 20h. الطباعة والتنقيح / Logging
            llvm::Value *emitAndroidLog(std::shared_ptr<SIRInstruction> inst);
            llvm::Value *emitAndroidPrint(std::shared_ptr<SIRInstruction> inst);

        public:
            // (AR) Phase 8 Cleanup: نُقلت دوال UI/Atomic/Sizeof إلى public
            //      لأنها inline wrappers تُستدعى من sub-codegens (instr_platform_ops)
            //      عبر الـ dispatcher بدون الحاجة إلى friend declarations.
            // (EN) Phase 8 Cleanup: UI/Atomic/Sizeof methods moved to public
            //      because they are inline wrappers called from sub-codegens
            //      (instr_platform_ops) via dispatcher — eliminates friend need.

            // =====================================================================
            // 21. نظام الواجهة الموحد / Unified UI System (sad_ui.h)
            // =====================================================================
            // 21a. مصانع العناصر / Widget Factories
            llvm::Value *emitUiColumn(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiColumn(inst); }
            llvm::Value *emitUiRow(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiRow(inst); }
            llvm::Value *emitUiStack(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiStack(inst); }
            llvm::Value *emitUiContainer(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiContainer(inst); }
            llvm::Value *emitUiText(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiText(inst); }
            llvm::Value *emitUiTextStyled(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiTextStyled(inst); }
            llvm::Value *emitUiButton(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiButton(inst); }
            llvm::Value *emitUiButtonVariant(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiButtonVariant(inst); }
            llvm::Value *emitUiIconButton(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiIconButton(inst); }
            llvm::Value *emitUiFab(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiFab(inst); }
            llvm::Value *emitUiTextField(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiTextField(inst); }
            llvm::Value *emitUiCheckbox(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiCheckbox(inst); }
            llvm::Value *emitUiSwitch(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSwitch(inst); }
            llvm::Value *emitUiSlider(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSlider(inst); }
            llvm::Value *emitUiCard(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiCard(inst); }
            llvm::Value *emitUiScaffold(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiScaffold(inst); }
            llvm::Value *emitUiAppBar(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAppBar(inst); }
            llvm::Value *emitUiSpacer(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSpacer(inst); }
            llvm::Value *emitUiDivider(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiDivider(inst); }
            llvm::Value *emitUiDialog(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiDialog(inst); }
            // 21a-إضافي. مصانع إضافيّة (م-مصانع) — موجِّهات إلى UICodeGen
            llvm::Value *emitUiImage(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiImage(inst); }
            llvm::Value *emitUiIcon(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiIcon(inst); }
            llvm::Value *emitUiTextButton(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiTextButton(inst); }
            llvm::Value *emitUiGrid(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiGrid(inst); }
            llvm::Value *emitUiCenter(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiCenter(inst); }
            llvm::Value *emitUiPadding(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiPadding(inst); }
            llvm::Value *emitUiAlign(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAlign(inst); }
            llvm::Value *emitUiExpanded(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiExpanded(inst); }
            llvm::Value *emitUiFlexible(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiFlexible(inst); }
            llvm::Value *emitUiSizedBox(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSizedBox(inst); }
            llvm::Value *emitUiWrap(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiWrap(inst); }
            llvm::Value *emitUiBox(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiBox(inst); }
            llvm::Value *emitUiScrollView(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiScrollView(inst); }
            llvm::Value *emitUiBottomNav(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiBottomNav(inst); }
            llvm::Value *emitUiSnackbar(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSnackbar(inst); }
            llvm::Value *emitUiTooltip(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiTooltip(inst); }
            llvm::Value *emitUiProgress(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiProgress(inst); }
            llvm::Value *emitUiLazyColumn(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiLazyColumn(inst); }
            llvm::Value *emitUiLazyRow(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiLazyRow(inst); }
            llvm::Value *emitUiListView(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiListView(inst); }
            llvm::Value *emitUiTextArea(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiTextArea(inst); }
            llvm::Value *emitUiDrawer(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiDrawer(inst); }
            llvm::Value *emitUiWidgetByType(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiWidgetByType(inst); }
            llvm::Value *emitUiSpinBox(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSpinBox(inst); }
            llvm::Value *emitUiGroupBox(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiGroupBox(inst); }
            llvm::Value *emitUiSpinner(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSpinner(inst); }
            llvm::Value *emitUiStatusBar(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiStatusBar(inst); }
            llvm::Value *emitUiWindow(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiWindow(inst); }
            llvm::Value *emitUiTitleBar(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiTitleBar(inst); }
            llvm::Value *emitUiScrollBar(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiScrollBar(inst); }
            llvm::Value *emitUiTaskbar(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiTaskbar(inst); }
            llvm::Value *emitUiStartMenu(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiStartMenu(inst); }
            llvm::Value *emitUiSystemTray(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSystemTray(inst); }
            llvm::Value *emitUiMenu(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiMenu(inst); }
            llvm::Value *emitUiToolbar(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiToolbar(inst); }
            llvm::Value *emitUiSegmentedControl(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSegmentedControl(inst); }
            llvm::Value *emitUiTreeView(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiTreeView(inst); }
            llvm::Value *emitUiSplitView(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSplitView(inst); }
            llvm::Value *emitUiDataTable(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiDataTable(inst); }
            llvm::Value *emitUiBreadcrumb(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiBreadcrumb(inst); }
            llvm::Value *emitUiPicker(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiPicker(inst); }
            llvm::Value *emitUiDatePicker(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiDatePicker(inst); }
            llvm::Value *emitUiStepper(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiStepper(inst); }
            llvm::Value *emitUiRatingBar(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiRatingBar(inst); }
            llvm::Value *emitUiAlert(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAlert(inst); }
            llvm::Value *emitUiSafeArea(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSafeArea(inst); }
            llvm::Value *emitUiSurface(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSurface(inst); }
            // 21e. أثر المعدّلات (م-أ3ر) — موجِّهات: خاصّيّة عامّة (L1) + حدث عند_* (L2) + تحريك (L3) + دمج متعدّد الوسائط
            llvm::Value *emitUiSetPropStr(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetPropStr(inst); }
            llvm::Value *emitUiSetPropInt(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetPropInt(inst); }
            llvm::Value *emitUiSetPropNum(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetPropNum(inst); }
            llvm::Value *emitUiSetPropBool(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetPropBool(inst); }
            llvm::Value *emitUiSetPropDyn(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetPropDyn(inst); }
            llvm::Value *emitUiAddEvent(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAddEvent(inst); }
            llvm::Value *emitUiAnimBegin(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAnimBegin(inst); }
            llvm::Value *emitUiAnimDuration(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAnimDuration(inst); }
            llvm::Value *emitUiAnimEasing(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAnimEasing(inst); }
            llvm::Value *emitUiSetEventPhase(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetEventPhase(inst); }
            llvm::Value *emitUiSetEventData(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetEventData(inst); }
            llvm::Value *emitUiAnimDelay(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAnimDelay(inst); }
            llvm::Value *emitUiAnimRepeat(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAnimRepeat(inst); }
            llvm::Value *emitUiAnimAutoReverse(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAnimAutoReverse(inst); }
            llvm::Value *emitUiPropJoinAdd(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiPropJoinAdd(inst); }
            llvm::Value *emitUiPropJoinCommit(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiPropJoinCommit(inst); }
            // 21b. إدارة الشجرة / Tree Management
            llvm::Value *emitUiAddChild(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAddChild(inst); }
            llvm::Value *emitUiRemoveChild(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiRemoveChild(inst); }
            llvm::Value *emitUiClearChildren(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiClearChildren(inst); }
            // 21c. ضبط الخصائص / Property Setters
            llvm::Value *emitUiSetText(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetText(inst); }
            llvm::Value *emitUiSetSize(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetSize(inst); }
            llvm::Value *emitUiSetFlex(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetFlex(inst); }
            llvm::Value *emitUiSetBackground(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetBackground(inst); }
            llvm::Value *emitUiSetForeground(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetForeground(inst); }
            llvm::Value *emitUiSetSpacing(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetSpacing(inst); }
            llvm::Value *emitUiSetPadding(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetPadding(inst); }
            llvm::Value *emitUiSetAlignment(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetAlignment(inst); }
            llvm::Value *emitUiSetBorder(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetBorder(inst); }
            llvm::Value *emitUiSetElevation(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetElevation(inst); }
            llvm::Value *emitUiSetOpacity(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetOpacity(inst); }
            llvm::Value *emitUiSetVisibility(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetVisibility(inst); }
            // 21d. إدارة التطبيق / App Management
            llvm::Value *emitUiAppCreate(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAppCreate(inst); }
            llvm::Value *emitUiAppSetRoot(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAppSetRoot(inst); }
            llvm::Value *emitUiAppLayout(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAppLayout(inst); }
            llvm::Value *emitUiAppRender(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAppRender(inst); }
            llvm::Value *emitUiAppRun(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAppRun(inst); }
            llvm::Value *emitUiPrintTree(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiPrintTree(inst); }
            llvm::Value *emitUiToggleTheme(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiToggleTheme(inst); }
            llvm::Value *emitUiDarkMode(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiDarkMode(inst); }
            llvm::Value *emitUiLightMode(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiLightMode(inst); }
            llvm::Value *emitUiIsDark(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiIsDark(inst); }
            llvm::Value *emitUiNextKey(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiNextKey(inst); }
            llvm::Value *emitUiNavigate(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiNavigate(inst); }
            llvm::Value *emitUiNavBack(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiNavBack(inst); }
            llvm::Value *emitUiNavRoot(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiNavRoot(inst); }
            llvm::Value *emitUiReplacePage(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiReplacePage(inst); }
            llvm::Value *emitUiPageCount(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiPageCount(inst); }
            llvm::Value *emitUiCurrentPage(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiCurrentPage(inst); }
            llvm::Value *emitUiNavigateTransition(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiNavigateTransition(inst); }
            llvm::Value *emitUiBackTransition(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiBackTransition(inst); }
            // (إكمال corui) الانتقال الكامل + الحالة + النافذة + توليد الويب
            llvm::Value *emitUiNavigateExitTransition(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiNavigateExitTransition(inst); }
            llvm::Value *emitUiUpdateState(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiUpdateState(inst); }
            llvm::Value *emitUiStopPropagation(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiStopPropagation(inst); }
            llvm::Value *emitUiSetState(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetState(inst); }
            llvm::Value *emitUiSetTitle(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiSetTitle(inst); }
            llvm::Value *emitUiCloseWindow(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiCloseWindow(inst); }
            llvm::Value *emitUiGenPlatform(std::shared_ptr<SIRInstruction> inst, const char *runtimeName) { return ui_->emitUiGenPlatform(inst, runtimeName); }
            llvm::Value *emitUiAppDestroy(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiAppDestroy(inst); }
            llvm::Value *emitUiWidgetDestroy(std::shared_ptr<SIRInstruction> inst) { return ui_->emitUiWidgetDestroy(inst); }

            // ================================================================
            // Section 21: التوجيهات / Directives (@حجم, @ذري)
            // (AR) دعم توجيهات اللغة منخفضة المستوى
            // (EN) Support for low-level language directives
            // ================================================================
            llvm::Value *emitSizeof(std::shared_ptr<SIRInstruction> inst) { return dir_->emitSizeof(inst); }
            llvm::Value *emitAtomicLoad(std::shared_ptr<SIRInstruction> inst) { return dir_->emitAtomicLoad(inst); }
            llvm::Value *emitAtomicStore(std::shared_ptr<SIRInstruction> inst) { return dir_->emitAtomicStore(inst); }
            llvm::Value *emitAtomicAdd(std::shared_ptr<SIRInstruction> inst) { return dir_->emitAtomicAdd(inst); }
            llvm::Value *emitAtomicSub(std::shared_ptr<SIRInstruction> inst) { return dir_->emitAtomicSub(inst); }
            llvm::Value *emitAtomicExchange(std::shared_ptr<SIRInstruction> inst) { return dir_->emitAtomicExchange(inst); }
            llvm::Value *emitAtomicCmpXchg(std::shared_ptr<SIRInstruction> inst) { return dir_->emitAtomicCmpXchg(inst); }
        };

    } // namespace LLVM
} // namespace Sad

#endif // SAD_LLVM_CODEGEN_H
