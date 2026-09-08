/*
 * ============================================================================
 * محول أنواع Sad/SIR إلى LLVM - ملف التنفيذ
 * Sad/SIR to LLVM Type Mapper - Implementation File
 * ============================================================================
 *
 * هذا الملف يحتوي على تنفيذ محول أنواع Sad و SIR إلى LLVM IR
 * This file contains implementation of Sad and SIR to LLVM type mapper
 *
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement
 * ============================================================================
 */

#include "llvm_type_mapper.h"
#include "sad_dyn_repr.h"
#include <stdexcept>

namespace Sad
{
    namespace LLVM
    {

        // ============================================================================
        // المنشئ والمدمر / Constructor & Destructor
        // ============================================================================

        /**
         * منشئ محول الأنواع - يتم تهيئة السياق
         * Type mapper constructor - Initialize context
         */
        LLVMTypeMapper::LLVMTypeMapper(llvm::LLVMContext &context)
            : context_(context)
        {
            // لا حاجة لتهيئة أخرى هنا / No further initialization needed here
            // التخزين المؤقت سيُملأ تدريجياً / Cache will be filled gradually
        }

        // ============================================================================
        // تحويل أنواع Sad / Sad Type Conversion
        // ============================================================================

        // ════════════════════════════════════════════════════════════════════════
        // (AR) 🔑 وكان ههنا هرمُ أنواعٍ **ثالثٌ** اسمُه `Sad::LLVM::Type` —
        //      قاعدةٌ مجرّدةٌ وثلاثةُ فروعٍ (`TupleType` · `TaggedUnionType` ·
        //      `FunctionPointerType`) في `llvm_type_mapper_composite.h`. وقِيسَ
        //      أنّ **لا موضعَ في المستودعِ كلِّه يُنشئُ واحدًا منها**: لا
        //      `make_shared` ولا `new`؛ وكلُّ ما كان يمسُّها `dynamic_cast` من
        //      مؤشّرٍ لا يُملأُ أبدًا — فالهرمُ كان يُقرأُ ولا يُكتَب.
        //      أمرُ القياس (⇒ صفر):
        //        grep -rEn "make_shared<TupleType>|new TupleType" compiler shared
        //      فحُذِفَ الهرمُ ومعه عنقودُه المغلق: مُحوِّلاتُ الأنواعِ الخمسةُ في
        //      LLVMTypeMapper وطريقتا التحويلِ في TypesCodeGen — لا مُنادِيَ
        //      لواحدةٍ منها من خارجِ العنقود.
        // (EN) A THIRD type hierarchy (Sad::LLVM::Type + 3 subclasses) lived
        //      here. Measured: nothing in the tree ever constructs one, so the
        //      whole closed cluster consuming it was dead code. Removed.
        // ════════════════════════════════════════════════════════════════════════


        // ============================================================================
        // تحويل أنواع SIR / SIR Type Conversion
        // ============================================================================

        /**
         * تحويل نوع SIR إلى نوع LLVM
         * Convert SIR type to LLVM type
         */
        llvm::Type *LLVMTypeMapper::mapSIRType(Compiler::SIR::SadTypeKind sirType)
        {
            // تحويل مباشر بناءً على نوع SIR / Direct conversion based on SIR type
            switch (sirType)
            {
            case Compiler::SIR::SadTypeKind::Unit:
                // نوع فارغ / Void type
                return getVoidType();

            case Compiler::SIR::SadTypeKind::Integer:
                // عدد صحيح 64-bit / 64-bit integer
                return getInt64Type();

            case Compiler::SIR::SadTypeKind::Float:
                // عدد عشري 64-bit / 64-bit float
                return getDoubleType();

            case Compiler::SIR::SadTypeKind::Boolean:
                // منطقي (i1) / Boolean (i1)
                return getBoolType();

            case Compiler::SIR::SadTypeKind::Pointer:
                // مؤشر عام (i8*) / Generic pointer (i8*)
                return getStringPtrType();

            case Compiler::SIR::SadTypeKind::String:
                // نص (i8*) / String (i8*)
                return getStringPtrType();

            case Compiler::SIR::SadTypeKind::Array:
                // مصفوفة - يحتاج معلومات إضافية / Array - needs additional info
                // افتراضي: مؤشر / Default: pointer
                return getStringPtrType();

            case Compiler::SIR::SadTypeKind::Struct:
                // بنية - يحتاج تعريف / Struct - needs definition
                // افتراضي: مؤشر / Default: pointer
                return getStringPtrType();

            case Compiler::SIR::SadTypeKind::Function:
                // دالة - يحتاج توقيع / Function - needs signature
                // افتراضي: مؤشر دالة / Default: function pointer
                return getStringPtrType();

            case Compiler::SIR::SadTypeKind::Map:
                // قاموس/خريطة — مؤشر لبنية وقت التشغيل
                // Dictionary/map — pointer to runtime struct
                return getStringPtrType();

            case Compiler::SIR::SadTypeKind::UInt8:
                // بايت (8-bit) / Byte (8-bit unsigned)
                return llvm::Type::getInt8Ty(context_);

            case Compiler::SIR::SadTypeKind::UInt64:
                // طبيعي (64-bit) — الإشارة تُحمَل في العمليّة لا في نوع LLVM
                // uint64 — signedness carried by the op, not the LLVM type
                return getInt64Type();

            case Compiler::SIR::SadTypeKind::Error:
                // خطأ — مؤشر لبنية الخطأ / Error — pointer to error struct
                return getStringPtrType();

            case Compiler::SIR::SadTypeKind::Null:
                // (AR) عدم: يُمثَّل بحارس i64 (kSadNullSentinel) — S-TS-P4 codegen
                // (EN) null: represented as an i64 sentinel (kSadNullSentinel) — S-TS-P4 codegen
                return getInt64Type();

            case Compiler::SIR::SadTypeKind::Any:
                // (AR) ISSUE-076 (حلّ %SadDyn الجذريّ): الديناميّ نوعُه المميّز %SadDyn
                //      ({i8 وسم، i64 حمولة}) لا i64، فيُميّزه المدقِّق عن المحدَّد.
                // (EN) ISSUE-076 (%SadDyn root fix): the dynamic type lowers to the distinct
                //      %SadDyn ({i8 kind, i64 payload}), not i64, so the verifier tells it apart.
                return getSadDynType(context_);

            default:
                // نوع غير معروف - افتراضي i64 / Unknown type - default i64
                return getInt64Type();
            }
        }

        /**
         * تحويل نوع SIR إلى نوع مؤشر LLVM
         * Convert SIR type to LLVM pointer type
         */
        llvm::PointerType *LLVMTypeMapper::mapSIRTypeToPointer(Compiler::SIR::SadTypeKind sirType)
        {
            // الحصول على النوع الأساسي أولاً / Get base type first
            llvm::Type *baseType = mapSIRType(sirType);

            // إنشاء نوع مؤشر / Create pointer type
            return createPointerType(baseType);
        }

        /**
         * @brief (AR) تحويل SadTypePtr مباشرة إلى نوع LLVM — الطريق الموحد الجديد
         * @brief (EN) Convert SadTypePtr directly to LLVM type — new unified path
         *
         * (AR) يُفوّض إلى mapSIRType باستخدام getKind() من SadType
         *      هذا يضمن تطابق النتائج بين المسارين القديم والجديد
         * (EN) Delegates to mapSIRType using getKind() from SadType
         *      This ensures identical results between old and new paths
         */
        llvm::Type *LLVMTypeMapper::mapSadType(const Sad::Types::SadTypePtr &sadType)
        {
            if (!sadType)
                return getInt64Type(); // fallback
            return mapSIRType(sadType->getKind());
        }

        // ============================================================================
        // أنواع أساسية / Primitive Types
        // ============================================================================

        /**
         * الحصول على نوع فارغ LLVM / Get LLVM void type
         */
        llvm::Type *LLVMTypeMapper::getVoidType() const
        {
            return llvm::Type::getVoidTy(context_); // نوع فارغ / Void type
        }

        /**
         * الحصول على نوع منطقي (i1) / Get boolean type (i1)
         */
        llvm::Type *LLVMTypeMapper::getBoolType() const
        {
            return llvm::Type::getInt1Ty(context_); // i1 للمنطقيات / i1 for booleans
        }

        /**
         * الحصول على نوع عدد صحيح 8-bit / Get 8-bit integer type
         */
        llvm::Type *LLVMTypeMapper::getInt8Type() const
        {
            return llvm::Type::getInt8Ty(context_); // i8
        }

        /**
         * الحصول على نوع عدد صحيح 16-bit / Get 16-bit integer type
         */
        llvm::Type *LLVMTypeMapper::getInt16Type() const
        {
            return llvm::Type::getInt16Ty(context_); // i16
        }

        /**
         * الحصول على نوع عدد صحيح 32-bit / Get 32-bit integer type
         */
        llvm::Type *LLVMTypeMapper::getInt32Type() const
        {
            return llvm::Type::getInt32Ty(context_); // i32
        }

        /**
         * الحصول على نوع عدد صحيح 64-bit / Get 64-bit integer type
         */
        llvm::Type *LLVMTypeMapper::getInt64Type() const
        {
            return llvm::Type::getInt64Ty(context_); // i64
        }

        /**
         * الحصول على نوع عدد عشري 32-bit (float) / Get 32-bit float type
         */
        llvm::Type *LLVMTypeMapper::getFloatType() const
        {
            return llvm::Type::getFloatTy(context_); // float (32-bit)
        }

        /**
         * الحصول على نوع عدد عشري 64-bit (double) / Get 64-bit double type
         */
        llvm::Type *LLVMTypeMapper::getDoubleType() const
        {
            return llvm::Type::getDoubleTy(context_); // double (64-bit)
        }

        /**
         * الحصول على نوع مؤشر i8* (للنصوص) / Get i8* pointer type (for strings)
         */
        llvm::PointerType *LLVMTypeMapper::getStringPtrType() const
        {
#if LLVM_VERSION_MAJOR >= 15
            return llvm::PointerType::get(context_, 0); // opaque pointer
#else
            return llvm::Type::getInt8PtrTy(context_); // i8* للنصوص / i8* for strings
#endif
        }

        /**
         * الحصول على نوع عدد صحيح بحجم محدد / Get integer type with specific bit width
         */
        llvm::Type *LLVMTypeMapper::getIntType(unsigned bitWidth) const
        {
            // التحقق من الأحجام المدعومة / Check supported sizes
            switch (bitWidth)
            {
            case 1:
                return getBoolType(); // i1 للمنطقيات / i1 for booleans
            case 8:
                return getInt8Type(); // i8
            case 16:
                return getInt16Type(); // i16
            case 32:
                return getInt32Type(); // i32
            case 64:
                return getInt64Type(); // i64
            default:
                // حجم غير مدعوم - افتراضي i64 / Unsupported size - default i64
                return getInt64Type();
            }
        }

        // ============================================================================
        // أنواع مركبة / Composite Types
        // ============================================================================

        /**
         * إنشاء نوع مصفوفة LLVM / Create LLVM array type
         */
        llvm::ArrayType *LLVMTypeMapper::createArrayType(llvm::Type *elementType, size_t size)
        {
            // إنشاء نوع مصفوفة / Create array type
            return llvm::ArrayType::get(elementType, size);
        }

        /**
         * إنشاء نوع بنية (struct) / Create struct type
         */
        llvm::StructType *LLVMTypeMapper::createStructType(
            const std::string &name,
            const std::vector<llvm::Type *> &fieldTypes,
            bool isPacked)
        {
            // التحقق من التخزين المؤقت / Check cache
            auto it = structCache_.find(name);
            if (it != structCache_.end())
            {
                return it->second; // إرجاع من التخزين المؤقت / Return from cache
            }

            // إنشاء بنية جديدة / Create new struct
            llvm::StructType *structType = llvm::StructType::create(
                context_,   // السياق / Context
                fieldTypes, // أنواع الحقول / Field types
                name,       // الاسم / Name
                isPacked    // هل مُرصّة؟ / Is packed?
            );

            // حفظ في التخزين المؤقت / Save to cache
            structCache_[name] = structType;

            return structType; // إرجاع البنية / Return struct
        }

        /**
         * إنشاء نوع مؤشر / Create pointer type
         */
        llvm::PointerType *LLVMTypeMapper::createPointerType(llvm::Type *pointeeType)
        {
            // إنشاء نوع مؤشر / Create pointer type
            return pointeeType->getPointerTo();
        }

        /**
         * إنشاء نوع دالة / Create function type
         */
        llvm::FunctionType *LLVMTypeMapper::createFunctionType(
            llvm::Type *returnType,
            const std::vector<llvm::Type *> &paramTypes,
            bool isVarArg)
        {
            // إنشاء نوع دالة / Create function type
            return llvm::FunctionType::get(
                returnType, // نوع الرجوع / Return type
                paramTypes, // أنواع المعاملات / Parameter types
                isVarArg    // هل تقبل عدد متغير؟ / Is variadic?
            );
        }

        // ============================================================================
        // إدارة التخزين المؤقت / Cache Management
        // ============================================================================

        /**
         * مسح التخزين المؤقت للأنواع / Clear type cache
         */
        void LLVMTypeMapper::clearCache()
        {
            typeCache_.clear();   // مسح تخزين الأنواع / Clear type cache
            structCache_.clear(); // مسح تخزين البنى / Clear struct cache
        }

        /**
         * الحصول على عدد الأنواع المخزنة مؤقتاً / Get number of cached types
         */
        size_t LLVMTypeMapper::getCacheSize() const
        {
            return typeCache_.size() + structCache_.size(); // مجموع التخزين / Total cache
        }

        // ============================================================================
        // دوال مساعدة خاصة / Private Helper Functions
        // ============================================================================


        /**
         * الحصول على نوع من التخزين المؤقت / Get type from cache
         */
        llvm::Type *LLVMTypeMapper::getCachedType(const std::string &typeName) const
        {
            // البحث في التخزين المؤقت / Search in cache
            auto it = typeCache_.find(typeName);

            if (it != typeCache_.end())
            {
                return it->second; // تم العثور عليه / Found
            }

            return nullptr; // لم يتم العثور عليه / Not found
        }

        /**
         * حفظ نوع في التخزين المؤقت / Save type to cache
         */
        void LLVMTypeMapper::cacheType(const std::string &typeName, llvm::Type *type)
        {
            // حفظ في التخزين المؤقت / Save to cache
            typeCache_[typeName] = type;
        }

    } // namespace LLVM
} // namespace Sad
