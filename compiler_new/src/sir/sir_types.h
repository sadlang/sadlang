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

namespace sad::sir {

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
struct SourceLocation {
    std::string filename;
    int line = 0;
    int column = 0;
    int endLine = 0;
    int endColumn = 0;
    
    std::string toString() const {
        return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

// =============================================================================
// أنواع SIR
// =============================================================================

/**
 * تصنيف الأنواع
 */
enum class TypeKind {
    // أنواع بدائية
    Void,           // لا شيء
    Bool,           // منطقي
    Int8,           // عدد8
    Int16,          // عدد16
    Int32,          // عدد32
    Int64,          // عدد64
    UInt8,          // طبيعي8
    UInt16,         // طبيعي16
    UInt32,         // طبيعي32
    UInt64,         // طبيعي64
    Float32,        // عشري32
    Float64,        // عشري64
    Char,           // حرف
    
    // أنواع مركبة
    String,         // نص
    Array,          // مصفوفة
    Slice,          // شريحة
    Tuple,          // صف
    Struct,         // صنف
    Enum,           // تعداد
    Union,          // اتحاد
    
    // أنواع المراجع
    Reference,      // مرجع &T
    MutableRef,     // مرجع متغير &متغير T
    RawPointer,     // مؤشر خام *T
    
    // أنواع الدوال
    Function,       // دالة
    Closure,        // إغلاق
    
    // أنواع خاصة
    Optional,       // اختياري ?T
    Result,         // نتيجة<T, E>
    Never,          // لا يرجع !
    Generic,        // نوع عام T
    TypeAlias       // اسم مستعار للنوع
};

/**
 * نوع SIR
 */
class SirType {
public:
    TypeKind kind;
    std::string name;                           // اسم النوع (للأصناف والتعدادات)
    std::vector<std::shared_ptr<SirType>> params;  // معاملات النوع
    bool isMutable = false;                     // قابل للتغيير؟
    std::optional<std::string> lifetimeName;    // اسم دورة الحياة
    
    // مساعدات للإنشاء
    static std::shared_ptr<SirType> Void() {
        auto t = std::make_shared<SirType>();
        t->kind = TypeKind::Void;
        t->name = "لاشيء";
        return t;
    }
    
    static std::shared_ptr<SirType> Bool() {
        auto t = std::make_shared<SirType>();
        t->kind = TypeKind::Bool;
        t->name = "منطقي";
        return t;
    }
    
    static std::shared_ptr<SirType> Int32() {
        auto t = std::make_shared<SirType>();
        t->kind = TypeKind::Int32;
        t->name = "عدد";
        return t;
    }
    
    static std::shared_ptr<SirType> Int64() {
        auto t = std::make_shared<SirType>();
        t->kind = TypeKind::Int64;
        t->name = "عدد64";
        return t;
    }
    
    static std::shared_ptr<SirType> Float64() {
        auto t = std::make_shared<SirType>();
        t->kind = TypeKind::Float64;
        t->name = "عشري64";
        return t;
    }
    
    static std::shared_ptr<SirType> String() {
        auto t = std::make_shared<SirType>();
        t->kind = TypeKind::String;
        t->name = "نص";
        return t;
    }
    
    static std::shared_ptr<SirType> Reference(std::shared_ptr<SirType> inner, bool mut = false) {
        auto t = std::make_shared<SirType>();
        t->kind = mut ? TypeKind::MutableRef : TypeKind::Reference;
        t->name = mut ? "&متغير" : "&";
        t->params.push_back(inner);
        t->isMutable = mut;
        return t;
    }
    
    static std::shared_ptr<SirType> Array(std::shared_ptr<SirType> elem, int size = -1) {
        auto t = std::make_shared<SirType>();
        t->kind = TypeKind::Array;
        t->name = "مصفوفة";
        t->params.push_back(elem);
        return t;
    }
    
    static std::shared_ptr<SirType> Optional(std::shared_ptr<SirType> inner) {
        auto t = std::make_shared<SirType>();
        t->kind = TypeKind::Optional;
        t->name = "اختياري";
        t->params.push_back(inner);
        return t;
    }
    
    // تحقق من نوع المرجع
    bool isReference() const {
        return kind == TypeKind::Reference || kind == TypeKind::MutableRef;
    }
    
    bool isMutableReference() const {
        return kind == TypeKind::MutableRef;
    }
    
    // تحقق من القابلية للنسخ
    bool isCopyable() const {
        switch (kind) {
            case TypeKind::Bool:
            case TypeKind::Int8:
            case TypeKind::Int16:
            case TypeKind::Int32:
            case TypeKind::Int64:
            case TypeKind::UInt8:
            case TypeKind::UInt16:
            case TypeKind::UInt32:
            case TypeKind::UInt64:
            case TypeKind::Float32:
            case TypeKind::Float64:
            case TypeKind::Char:
            case TypeKind::Reference:  // المراجع الثابتة قابلة للنسخ
                return true;
            default:
                return false;
        }
    }
    
    // تمثيل نصي
    std::string toString() const {
        std::string result = name;
        
        if (!params.empty()) {
            result += "<";
            for (size_t i = 0; i < params.size(); i++) {
                if (i > 0) result += "، ";
                result += params[i]->toString();
            }
            result += ">";
        }
        
        if (lifetimeName) {
            result += "<" + *lifetimeName + ">";
        }
        
        return result;
    }
};

// =============================================================================
// معلومات الملكية
// =============================================================================

/**
 * حالة الملكية لقيمة
 */
enum class OwnershipState {
    Owned,          // مملوكة بالكامل
    Borrowed,       // مستعارة (للقراءة)
    MutBorrowed,    // مستعارة (للكتابة)
    Moved,          // منقولة (غير صالحة)
    Dropped,        // مُسقطة (تم تحريرها)
    Uninitialized   // غير مُهيأة
};

/**
 * معلومات الملكية لقيمة
 */
struct OwnershipInfo {
    OwnershipState state = OwnershipState::Uninitialized;
    std::optional<ValueId> borrowedFrom;        // مستعارة من أي قيمة؟
    std::vector<ValueId> borrowedBy;            // من يستعير منها؟
    std::optional<std::string> lifetimeName;    // دورة الحياة
    SourceLocation definedAt;                   // أين عُرفت؟
    std::optional<SourceLocation> movedAt;      // أين نُقلت؟
    std::optional<SourceLocation> droppedAt;    // أين أُسقطت؟
};

// =============================================================================
// القيم في SIR
// =============================================================================

/**
 * قيمة SIR
 * 
 * كل متغير أو نتيجة عملية يمثل بقيمة
 */
struct SirValue {
    ValueId id;
    std::string name;                           // اسم المتغير (للتوثيق)
    std::shared_ptr<SirType> type;
    OwnershipInfo ownership;
    SourceLocation location;
    
    // هل هذه قيمة مؤقتة؟
    bool isTemporary() const {
        return name.empty() || name[0] == '_';
    }
};

// =============================================================================
// المعاملات (Parameters)
// =============================================================================

/**
 * معامل دالة
 */
struct SirParameter {
    std::string name;
    std::shared_ptr<SirType> type;
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
struct Lifetime {
    std::string name;           // اسم دورة الحياة (مثل 'أ)
    BlockId startBlock;         // كتلة البداية
    BlockId endBlock;           // كتلة النهاية
    int startLine;
    int endLine;
    
    // هل دورة حياة ثابتة؟
    bool isStatic() const {
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
struct LifetimeConstraint {
    std::string longer;     // دورة الحياة الأطول
    std::string shorter;    // دورة الحياة الأقصر
    SourceLocation where;   // أين نشأ القيد؟
    
    std::string toString() const {
        return longer + ": " + shorter;
    }
};

// =============================================================================
// معلومات الاستعارة
// =============================================================================

/**
 * نوع الاستعارة
 */
enum class BorrowKind {
    Shared,     // استعارة مشتركة (للقراءة)
    Unique      // استعارة حصرية (للكتابة)
};

/**
 * استعارة نشطة
 */
struct ActiveBorrow {
    ValueId borrower;           // من يستعير؟
    ValueId lender;             // من يُعير؟
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
class SirContext {
public:
    // إنشاء قيمة جديدة
    ValueId createValue(const std::string& name, std::shared_ptr<SirType> type) {
        ValueId id = nextValueId_++;
        values_[id] = {id, name, type, {}, {}};
        return id;
    }
    
    // الحصول على قيمة
    SirValue* getValue(ValueId id) {
        auto it = values_.find(id);
        return it != values_.end() ? &it->second : nullptr;
    }
    
    // إنشاء كتلة جديدة
    BlockId createBlock(const std::string& name = "") {
        BlockId id = nextBlockId_++;
        blockNames_[id] = name.empty() ? "bb" + std::to_string(id) : name;
        return id;
    }
    
    // إدارة دورات الحياة
    void addLifetime(const Lifetime& lt) {
        lifetimes_[lt.name] = lt;
    }
    
    const Lifetime* getLifetime(const std::string& name) const {
        auto it = lifetimes_.find(name);
        return it != lifetimes_.end() ? &it->second : nullptr;
    }
    
    // إدارة القيود
    void addConstraint(const LifetimeConstraint& c) {
        constraints_.push_back(c);
    }
    
    const std::vector<LifetimeConstraint>& getConstraints() const {
        return constraints_;
    }
    
    // إدارة الاستعارات
    void addBorrow(const ActiveBorrow& b) {
        activeBorrows_.push_back(b);
    }
    
    void removeBorrow(ValueId borrower) {
        activeBorrows_.erase(
            std::remove_if(activeBorrows_.begin(), activeBorrows_.end(),
                [borrower](const ActiveBorrow& b) { return b.borrower == borrower; }),
            activeBorrows_.end()
        );
    }
    
    const std::vector<ActiveBorrow>& getActiveBorrows() const {
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
