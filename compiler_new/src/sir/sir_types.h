/**
 * =============================================================================
 * ملف: sir_types.h
 * الوصف: أنواع SIR الأساسية (Sad Intermediate Representation)
 * المهمة: T260 - SIR basic types
 * المرحلة: Phase 27 - User Story 24 (SIR Layer)
 * =============================================================================
 *
 * 📚 دليل المبتدئ لطبقة SIR
 * ═══════════════════════════
 *
 * ما هي SIR؟
 * ──────────
 * SIR = Sad Intermediate Representation
 * تمثيل وسيط خاص بلغة حزين
 *
 * طبقات الترجمة:
 * ```
 *     كود حزين (نص)
 *          │
 *          ▼
 *        AST (شجرة)
 *          │
 *          ▼
 *        SIR (هنا!) ← تحليل الملكية يحدث هنا
 *          │
 *          ▼
 *      LLVM IR (منخفض)
 *          │
 *          ▼
 *      كود الآلة
 * ```
 *
 * لماذا SIR؟
 * ──────────
 * 1. تحليل الملكية أسهل من AST
 * 2. أكثر تعبيراً من LLVM IR
 * 3. تحسينات خاصة بالملكية
 * 4. رسائل خطأ أوضح
 *
 * مثال التحويل:
 * ─────────────
 * ```sad
 * متغير س = 10        →  SIR: Alloc(س, عدد)
 * متغير ص = &س        →  SIR: Borrow(ص, س, ثابت)
 * متغير ع = س         →  SIR: Move(ع, س)  أو  Copy(ع, س)
 * ```
 *
 * =============================================================================
 */

#ifndef SAD_SIR_TYPES_H
#define SAD_SIR_TYPES_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <variant>
#include <map>
#include <algorithm>
#include "sad_type_system.h"

namespace sad::sir
{

    // =============================================================================
    // الأنواع الأساسية
    // =============================================================================

    /**
     * معرف فريد للقيم في SIR
     *
     * كل قيمة لها معرف فريد:
     * %0 = متغير س
     * %1 = متغير ص
     */
    using ValueId = uint32_t;

    /**
     * معرف فريد للكتل الأساسية
     */
    using BlockId = uint32_t;

    /**
     * معرف فريد للدوال
     */
    using FunctionId = uint32_t;

    // =============================================================================
    // موقع المصدر
    // =============================================================================

    /**
     * موقع في الكود المصدري
     *
     * للإشارة إلى مكان الخطأ
     */
    struct SourceLocation
    {
        std::string filename;
        int line = 0;
        int column = 0;
        int endLine = 0;
        int endColumn = 0;

        std::string toString() const
        {
            return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
        }
    };

    // =============================================================================
    // أنواع SIR — مأخوذة من نظام الأنواع الموحد
    // =============================================================================

    /**
     * (AR) جلب SadTypeKind و SadTypePtr من نظام الأنواع الموحد
     * (EN) Bring SadTypeKind and SadTypePtr from unified type system
     */
    using Sad::Types::SadType;
    using Sad::Types::SadTypeKind;
    using Sad::Types::SadTypePtr;
    using Sad::Types::SadTypeRegistry;

    // =============================================================================
    // دوال مساعدة لإنشاء أنواع SIR — بديل مباشر لـ SirType factory methods
    // (AR) تستدعي SadTypeRegistry مباشرة بدون أي غلاف وسيط
    // (EN) Direct SadTypeRegistry calls — no intermediate wrapper
    // =============================================================================

    /**
     * @brief (AR) إرجاع SadTypePtr لنوع Void (لاشيء)
     * @brief (EN) Return SadTypePtr for Void type
     */
    inline SadTypePtr sirVoid()
    {
        return SadTypeRegistry::instance().getVoid();
    }

    /**
     * @brief (AR) إرجاع SadTypePtr لنوع Boolean (منطقي)
     * @brief (EN) Return SadTypePtr for Boolean type
     */
    inline SadTypePtr sirBool()
    {
        return SadTypeRegistry::instance().getBoolean();
    }

    /**
     * @brief (AR) إرجاع SadTypePtr لنوع Integer (عدد صحيح، i32 في LLVM)
     * @brief (EN) Return SadTypePtr for Integer type (i32 in LLVM)
     */
    inline SadTypePtr sirInt32()
    {
        return SadTypeRegistry::instance().getInteger();
    }

    /**
     * @brief (AR) إرجاع SadTypePtr لنوع Integer 64 بت (عدد كبير، i64 في LLVM)
     *        ملاحظة: SadTypeRegistry لا يفرّق بين Int32/Int64 — كلاهما Integer
     *        لكن sadTypeToLlvm يتعامل مع SadTypeKind::Int64 إذا عُيّن صراحة
     * @brief (EN) Return SadTypePtr for 64-bit Integer type (i64 in LLVM)
     */
    inline SadTypePtr sirInt64()
    {
        return SadTypeRegistry::instance().getInteger();
    }

    /**
     * @brief (AR) إرجاع SadTypePtr لنوع Float (عشري، double في LLVM)
     * @brief (EN) Return SadTypePtr for Float type (double in LLVM)
     */
    inline SadTypePtr sirFloat64()
    {
        return SadTypeRegistry::instance().getFloat();
    }

    /**
     * @brief (AR) إرجاع SadTypePtr لنوع String (نص)
     * @brief (EN) Return SadTypePtr for String type
     */
    inline SadTypePtr sirString()
    {
        return SadTypeRegistry::instance().getString();
    }

    /**
     * @brief (AR) إنشاء SadTypePtr لنوع Reference (مرجع)
     * @brief (EN) Create SadTypePtr for Reference type
     * @param inner النوع الداخلي / Inner type
     * @param mut هل المرجع قابل للتغيير / Is mutable reference
     */
    inline SadTypePtr sirReference(SadTypePtr inner = nullptr, bool mut = false)
    {
        if (!inner)
            inner = SadTypeRegistry::instance().getAny();
        return SadTypeRegistry::instance().makeReference(inner, mut);
    }

    /**
     * @brief (AR) إنشاء SadTypePtr لنوع Array (مصفوفة)
     * @brief (EN) Create SadTypePtr for Array type
     * @param elem نوع العنصر / Element type
     */
    inline SadTypePtr sirArray(SadTypePtr elem = nullptr)
    {
        return SadTypeRegistry::instance().makeArray(std::move(elem));
    }

    /**
     * @brief (AR) إنشاء SadTypePtr لنوع Optional (اختياري)
     * @brief (EN) Create SadTypePtr for Optional type
     * @param inner النوع الداخلي / Inner type
     */
    inline SadTypePtr sirOptional(SadTypePtr inner = nullptr)
    {
        if (!inner)
            inner = SadTypeRegistry::instance().getAny();
        return SadTypeRegistry::instance().makeOptional(std::move(inner));
    }

    // =============================================================================
    // معلومات الملكية
    // =============================================================================

    /**
     * حالة الملكية لقيمة
     */
    enum class OwnershipState
    {
        Owned,        // مملوكة بالكامل
        Borrowed,     // مستعارة (للقراءة)
        MutBorrowed,  // مستعارة (للكتابة)
        Moved,        // منقولة (غير صالحة)
        Dropped,      // مُسقطة (تم تحريرها)
        Uninitialized // غير مُهيأة
    };

    /**
     * معلومات الملكية لقيمة
     */
    struct OwnershipInfo
    {
        OwnershipState state = OwnershipState::Uninitialized;
        std::optional<ValueId> borrowedFrom;     // مستعارة من أي قيمة؟
        std::vector<ValueId> borrowedBy;         // من يستعير منها؟
        std::optional<std::string> lifetimeName; // دورة الحياة
        SourceLocation definedAt;                // أين عُرفت؟
        std::optional<SourceLocation> movedAt;   // أين نُقلت؟
        std::optional<SourceLocation> droppedAt; // أين أُسقطت؟
    };

    // =============================================================================
    // القيم في SIR
    // =============================================================================

    /**
     * قيمة SIR
     *
     * كل متغير أو نتيجة عملية يمثل بقيمة
     */
    struct SirValue
    {
        ValueId id;
        std::string name; // اسم المتغير (للتوثيق)
        SadTypePtr type;  // (AR) النوع الموحد / (EN) Unified type (SadTypePtr)
        OwnershipInfo ownership;
        SourceLocation location;

        // هل هذه قيمة مؤقتة؟
        bool isTemporary() const
        {
            return name.empty() || name[0] == '_';
        }
    };

    // =============================================================================
    // المعاملات (Parameters)
    // =============================================================================

    /**
     * معامل دالة
     */
    struct SirParameter
    {
        std::string name;
        SadTypePtr type; // (AR) النوع الموحد / (EN) Unified type (SadTypePtr)
        bool isMutable = false;
        std::optional<std::string> lifetime;
        SourceLocation location;
    };

    // =============================================================================
    // دورة الحياة (Lifetime)
    // =============================================================================

    /**
     * دورة حياة
     */
    struct Lifetime
    {
        std::string name;   // اسم دورة الحياة (مثل 'أ)
        BlockId startBlock; // كتلة البداية
        BlockId endBlock;   // كتلة النهاية
        int startLine;
        int endLine;

        // هل دورة حياة ثابتة؟
        bool isStatic() const
        {
            return name == "ثابت" || name == "static";
        }
    };

    // =============================================================================
    // قيود دورة الحياة
    // =============================================================================

    /**
     * قيد دورة حياة
     *
     * مثال: 'أ: 'ب (أي 'أ يعيش أطول من 'ب)
     */
    struct LifetimeConstraint
    {
        std::string longer;   // دورة الحياة الأطول
        std::string shorter;  // دورة الحياة الأقصر
        SourceLocation where; // أين نشأ القيد؟

        std::string toString() const
        {
            return longer + ": " + shorter;
        }
    };

    // =============================================================================
    // معلومات الاستعارة
    // =============================================================================

    /**
     * نوع الاستعارة
     */
    enum class BorrowKind
    {
        Shared, // استعارة مشتركة (للقراءة)
        Unique  // استعارة حصرية (للكتابة)
    };

    /**
     * استعارة نشطة
     */
    struct ActiveBorrow
    {
        ValueId borrower; // من يستعير؟
        ValueId lender;   // من يُعير؟
        BorrowKind kind;
        Lifetime lifetime;
        SourceLocation location;
    };

    // =============================================================================
    // السياق العام لـ SIR
    // =============================================================================

    /**
     * سياق SIR
     *
     * يحتوي على كل المعلومات المشتركة
     */
    class SirContext
    {
    public:
        // إنشاء قيمة جديدة
        ValueId createValue(const std::string &name, SadTypePtr type)
        {
            ValueId id = nextValueId_++;
            values_[id] = {id, name, type, {}, {}};
            return id;
        }

        // الحصول على قيمة
        SirValue *getValue(ValueId id)
        {
            auto it = values_.find(id);
            return it != values_.end() ? &it->second : nullptr;
        }

        // إنشاء كتلة جديدة
        BlockId createBlock(const std::string &name = "")
        {
            BlockId id = nextBlockId_++;
            blockNames_[id] = name.empty() ? "bb" + std::to_string(id) : name;
            return id;
        }

        // إدارة دورات الحياة
        void addLifetime(const Lifetime &lt)
        {
            lifetimes_[lt.name] = lt;
        }

        const Lifetime *getLifetime(const std::string &name) const
        {
            auto it = lifetimes_.find(name);
            return it != lifetimes_.end() ? &it->second : nullptr;
        }

        // إدارة القيود
        void addConstraint(const LifetimeConstraint &c)
        {
            constraints_.push_back(c);
        }

        const std::vector<LifetimeConstraint> &getConstraints() const
        {
            return constraints_;
        }

        // إدارة الاستعارات
        void addBorrow(const ActiveBorrow &b)
        {
            activeBorrows_.push_back(b);
        }

        void removeBorrow(ValueId borrower)
        {
            activeBorrows_.erase(
                std::remove_if(activeBorrows_.begin(), activeBorrows_.end(),
                               [borrower](const ActiveBorrow &b)
                               { return b.borrower == borrower; }),
                activeBorrows_.end());
        }

        const std::vector<ActiveBorrow> &getActiveBorrows() const
        {
            return activeBorrows_;
        }

    private:
        std::map<ValueId, SirValue> values_;
        std::map<BlockId, std::string> blockNames_;
        std::map<std::string, Lifetime> lifetimes_;
        std::vector<LifetimeConstraint> constraints_;
        std::vector<ActiveBorrow> activeBorrows_;

        ValueId nextValueId_ = 0;
        BlockId nextBlockId_ = 0;
    };

} // namespace sad::sir

#endif // SAD_SIR_TYPES_H
