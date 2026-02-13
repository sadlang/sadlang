/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ███████╗  █████╗  ██████╗      ██╗      █████╗  ███╗   ██╗ ██████╗        ║
 * ║   ██╔════╝ ██╔══██╗ ██╔══██╗     ██║     ██╔══██╗ ████╗  ██║ ██╔════╝       ║
 * ║   ███████╗ ███████║ ██║  ██║     ██║     ███████║ ██╔██╗ ██║ ██║  ███╗      ║
 * ║   ╚════██║ ██╔══██║ ██║  ██║     ██║     ██╔══██║ ██║╚██╗██║ ██║   ██║      ║
 * ║   ███████║ ██║  ██║ ██████╔╝     ███████╗██║  ██║ ██║ ╚████║ ╚██████╔╝      ║
 * ║   ╚══════╝ ╚═╝  ╚═╝ ╚═════╝      ╚══════╝╚═╝  ╚═╝ ╚═╝  ╚═══╝  ╚═════╝       ║
 * ║                                                                              ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║  ملف: lazy_comprehension.cpp                                                 ║
 * ║  الوصف: التقييم الكسول للمولدات والـ Comprehensions                          ║
 * ║  المهمة: T149 - Phase 13 (US11)                                             ║
 * ║  المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              نظرة عامة | Overview
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 الهدف | Goal:
 *    تنفيذ التقييم الكسول (Lazy Evaluation) للمولدات والتعبيرات التوليدية
 *    Implement lazy evaluation for generators and generator expressions
 *
 * 📋 المفاهيم الأساسية | Core Concepts:
 *
 *    1️⃣ تعبير_مولد | Generator Expression:
 *       (س² لكل س في مدى(100))  // لا يُحسب حتى الحاجة
 *       (x² for x in range(100))  // Not computed until needed
 *
 *    2️⃣ مكرر_كسول | Lazy Iterator:
 *       متكرر يُنتج القيم عند الطلب فقط
 *       Iterator that produces values only on demand
 *
 *    3️⃣ تسلسل_كسول | Lazy Chain:
 *       مدى(∞) |> صفّي(زوجي) |> حوّل(مربع) |> خذ(10)
 *       range(∞) |> filter(even) |> map(square) |> take(10)
 *
 *    4️⃣ حساب_مؤجل | Deferred Computation:
 *       العمليات تُخزّن كوصف وتُنفّذ لاحقاً
 *       Operations stored as description, executed later
 *
 * 🎨 الصيغة العربية | Arabic Syntax:
 *
 *    // تعبير مولد كسول
 *    دع المربعات = (س² لكل س في مدى(1, لانهاية))
 *    
 *    // سلسلة عمليات كسولة
 *    دع النتيجة = الأرقام
 *        |> صفّي(|ن| ن % 2 == 0)
 *        |> حوّل(|ن| ن * ن)
 *        |> خذ(5)
 *        |> اجمع()
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <memory>
#include <vector>
#include <functional>
#include <optional>
#include <variant>
#include <string>
#include <sstream>
#include <unordered_map>
#include <cassert>
#include <any>

namespace sad {
namespace compiler {
namespace lazy {

// ═══════════════════════════════════════════════════════════════════════════════
//                           التصريحات الأمامية | Forward Declarations
// ═══════════════════════════════════════════════════════════════════════════════

class LazyIterator;
class LazyChain;
class LazyOperation;
class GeneratorState;
class ThunkManager;

// ═══════════════════════════════════════════════════════════════════════════════
//                              أنواع الكسل | Lazy Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🏷️ نوع_الكسل | LazyKind
 * 
 * أنواع التقييم الكسول المختلفة
 * Different kinds of lazy evaluation
 */
enum class LazyKind {
    // ═══════════════════════════════════════════════════════════════════════════
    //                           مولد | Generator
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * مولد - دالة تُنتج قيم متتالية
     * Generator - function that yields sequential values
     * 
     * مثال | Example:
     *   دالة مربعات() -> مولد<عدد> {
     *       متغير ن = 0
     *       طالما صحيح {
     *           أنتج ن * ن
     *           ن += 1
     *       }
     *   }
     */
    GENERATOR,
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                     تعبير_مولد | Generator Expression
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * تعبير مولد - comprehension كسول
     * Generator expression - lazy comprehension
     * 
     * مثال | Example:
     *   دع الزوجيات = (س لكل س في مدى(100) إذا س % 2 == 0)
     */
    GENERATOR_EXPRESSION,
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                         سلسلة_كسولة | Lazy Chain
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * سلسلة كسولة - عمليات متسلسلة بالأنبوب
     * Lazy chain - piped operations
     * 
     * مثال | Example:
     *   مدى(∞) |> صفّي(زوجي) |> حوّل(مربع) |> خذ(5)
     */
    LAZY_CHAIN,
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                            تأخير | Thunk
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * تأخير - حساب مؤجل مع تخزين مؤقت
     * Thunk - deferred computation with memoization
     * 
     * مثال | Example:
     *   دع كسول = مؤجل { عملية_بطيئة() }
     */
    THUNK,
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                        تدفق_لانهائي | Infinite Stream
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * تدفق لانهائي - مصدر بيانات بلا نهاية
     * Infinite stream - endless data source
     * 
     * مثال | Example:
     *   دع الطبيعية = تدفق_من(1, |ن| ن + 1)  // 1, 2, 3, 4, ...
     */
    INFINITE_STREAM
};

/**
 * 🔧 نوع_العملية_الكسولة | LazyOperationKind
 * 
 * أنواع العمليات في السلسلة الكسولة
 * Types of operations in lazy chain
 */
enum class LazyOperationKind {
    MAP,        // حوّل - تحويل كل عنصر
    FILTER,     // صفّي - ترشيح العناصر
    TAKE,       // خذ - أخذ عدد محدد
    TAKE_WHILE, // خذ_طالما - أخذ طالما الشرط صحيح
    DROP,       // أسقط - تجاهل عدد محدد
    DROP_WHILE, // أسقط_طالما - تجاهل طالما الشرط صحيح
    FLAT_MAP,   // حوّل_مسطح - تحويل مع تسطيح
    ENUMERATE,  // عدّد - إضافة الفهرس
    ZIP,        // دمج - دمج مع متكرر آخر
    CHUNK,      // قسّم - تقسيم لمجموعات
    WINDOW,     // نافذة - نافذة متحركة
    SCAN,       // امسح - تجميع تدريجي
    CYCLE,      // كرر - تكرار لانهائي
    CHAIN,      // سلسل - ربط متكررات
    INTERLEAVE, // تشابك - تناوب بين متكررات
    UNIQUE,     // فريد - إزالة المكررات
    PEEK,       // الق_نظرة - عمل جانبي
    COLLECT     // اجمع - جمع في مجموعة
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         حالة المولد | Generator State
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📦 حالة_المولد | GeneratorState
 * 
 * يحفظ حالة المولد بين استدعاءات yield
 * Preserves generator state between yield calls
 */
class GeneratorState {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    //                           حالة التنفيذ | Execution State
    // ═══════════════════════════════════════════════════════════════════════════
    enum class Status {
        CREATED,    // مُنشأ - لم يبدأ بعد
        RUNNING,    // يعمل - قيد التنفيذ
        SUSPENDED,  // معلق - في انتظار next()
        COMPLETED,  // مكتمل - انتهى
        FAILED      // فشل - حدث خطأ
    };

private:
    /**
     * الحالة الحالية
     * Current status
     */
    Status status_ = Status::CREATED;
    
    /**
     * نقطة الاستئناف - أين نستأنف بعد yield
     * Resume point - where to resume after yield
     */
    int resumePoint_ = 0;
    
    /**
     * المتغيرات المحلية المحفوظة
     * Saved local variables
     */
    struct LocalVariable {
        std::string name;       // اسم المتغير
        std::string type;       // نوع المتغير
        bool captured;          // هل مُلتقط من الخارج؟
    };
    std::vector<LocalVariable> locals_;
    
    /**
     * قيم المتغيرات المحفوظة (للتنفيذ)
     * Saved variable values (for execution)
     */
    std::unordered_map<std::string, std::any> savedValues_;
    
    /**
     * مؤشر المكدس
     * Stack pointer
     */
    size_t stackPointer_ = 0;

public:
    // ═══════════════════════════════════════════════════════════════════════════
    //                              البناء | Construction
    // ═══════════════════════════════════════════════════════════════════════════
    
    GeneratorState() = default;
    
    /**
     * إضافة متغير محلي | Add local variable
     */
    void addLocal(const std::string& name, const std::string& type, bool captured = false) {
        locals_.push_back({name, type, captured});
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                            إدارة الحالة | State Management
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تحديث نقطة الاستئناف | Update resume point
     */
    void setResumePoint(int point) {
        resumePoint_ = point;
    }
    
    int getResumePoint() const {
        return resumePoint_;
    }
    
    /**
     * تحديث الحالة | Update status
     */
    void setStatus(Status s) {
        status_ = s;
    }
    
    Status getStatus() const {
        return status_;
    }
    
    /**
     * هل انتهى؟ | Is completed?
     */
    bool isCompleted() const {
        return status_ == Status::COMPLETED || status_ == Status::FAILED;
    }
    
    /**
     * هل يمكن الاستئناف؟ | Can resume?
     */
    bool canResume() const {
        return status_ == Status::SUSPENDED || status_ == Status::CREATED;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                          حفظ/استعادة القيم | Save/Restore Values
    // ═══════════════════════════════════════════════════════════════════════════
    
    template<typename T>
    void saveValue(const std::string& name, const T& value) {
        savedValues_[name] = value;
    }
    
    template<typename T>
    std::optional<T> loadValue(const std::string& name) const {
        auto it = savedValues_.find(name);
        if (it != savedValues_.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (...) {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }
    
    /**
     * الحصول على المتغيرات المحلية | Get locals
     */
    const std::vector<LocalVariable>& getLocals() const {
        return locals_;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         المكرر الكسول | Lazy Iterator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔄 المكرر_الكسول | LazyIterator
 * 
 * واجهة موحدة للمكررات الكسولة
 * Unified interface for lazy iterators
 */
class LazyIterator {
public:
    virtual ~LazyIterator() = default;
    
    /**
     * الحصول على العنصر التالي | Get next element
     * 
     * @return العنصر التالي أو فارغ إذا انتهى
     */
    virtual std::optional<std::any> next() = 0;
    
    /**
     * إلقاء نظرة على التالي بدون استهلاكه | Peek at next without consuming
     */
    virtual std::optional<std::any> peek() = 0;
    
    /**
     * هل انتهى؟ | Is exhausted?
     */
    virtual bool isExhausted() const = 0;
    
    /**
     * تلميح الحجم (إن أمكن) | Size hint (if available)
     */
    virtual std::optional<size_t> sizeHint() const {
        return std::nullopt;  // غير معروف افتراضياً
    }
    
    /**
     * نوع العنصر | Element type
     */
    virtual std::string elementType() const = 0;
};

/**
 * 📊 مكرر_المدى_الكسول | LazyRangeIterator
 * 
 * مكرر كسول للنطاقات العددية
 * Lazy iterator for numeric ranges
 */
class LazyRangeIterator : public LazyIterator {
private:
    int64_t current_;
    int64_t end_;
    int64_t step_;
    bool exhausted_ = false;
    bool infinite_;  // مدى لانهائي؟

public:
    /**
     * إنشاء مدى محدود | Create finite range
     * 
     * @param start البداية | Start
     * @param end النهاية | End (exclusive)
     * @param step الخطوة | Step
     */
    LazyRangeIterator(int64_t start, int64_t end, int64_t step = 1)
        : current_(start), end_(end), step_(step), infinite_(false) {
        // التحقق من الصحة
        assert(step != 0 && "الخطوة لا يمكن أن تكون صفر | Step cannot be zero");
    }
    
    /**
     * إنشاء مدى لانهائي | Create infinite range
     * 
     * @param start البداية | Start
     * @param step الخطوة | Step
     */
    static LazyRangeIterator infinite(int64_t start = 0, int64_t step = 1) {
        LazyRangeIterator iter(start, 0, step);
        iter.infinite_ = true;
        return iter;
    }
    
    std::optional<std::any> next() override {
        if (exhausted_) return std::nullopt;
        
        // تحقق من النهاية (إن لم يكن لانهائي)
        if (!infinite_) {
            if (step_ > 0 && current_ >= end_) {
                exhausted_ = true;
                return std::nullopt;
            }
            if (step_ < 0 && current_ <= end_) {
                exhausted_ = true;
                return std::nullopt;
            }
        }
        
        int64_t result = current_;
        current_ += step_;
        return result;
    }
    
    std::optional<std::any> peek() override {
        if (exhausted_) return std::nullopt;
        
        if (!infinite_) {
            if (step_ > 0 && current_ >= end_) return std::nullopt;
            if (step_ < 0 && current_ <= end_) return std::nullopt;
        }
        
        return current_;
    }
    
    bool isExhausted() const override {
        return exhausted_;
    }
    
    std::optional<size_t> sizeHint() const override {
        if (infinite_) return std::nullopt;
        if (step_ > 0) {
            return static_cast<size_t>((end_ - current_ + step_ - 1) / step_);
        } else {
            return static_cast<size_t>((current_ - end_ - step_ - 1) / (-step_));
        }
    }
    
    std::string elementType() const override {
        return "عدد_صحيح";  // عدد صحيح | Integer
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         العملية الكسولة | Lazy Operation
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 العملية_الكسولة | LazyOperation
 * 
 * تمثيل عملية كسولة في السلسلة
 * Representation of a lazy operation in a chain
 */
class LazyOperation {
public:
    using Predicate = std::function<bool(const std::any&)>;
    using Transform = std::function<std::any(const std::any&)>;
    
private:
    LazyOperationKind kind_;
    
    // معاملات العملية
    std::optional<Predicate> predicate_;   // للترشيح
    std::optional<Transform> transform_;    // للتحويل
    std::optional<size_t> count_;          // للأخذ/الإسقاط
    std::shared_ptr<LazyIterator> other_;  // للدمج
    
public:
    // ═══════════════════════════════════════════════════════════════════════════
    //                            مُنشئات العمليات | Operation Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إنشاء عملية حوّل | Create map operation
     * 
     * مثال | Example:
     *   |> حوّل(|س| س * 2)
     */
    static LazyOperation map(Transform f) {
        LazyOperation op;
        op.kind_ = LazyOperationKind::MAP;
        op.transform_ = f;
        return op;
    }
    
    /**
     * إنشاء عملية صفّي | Create filter operation
     * 
     * مثال | Example:
     *   |> صفّي(|س| س > 0)
     */
    static LazyOperation filter(Predicate p) {
        LazyOperation op;
        op.kind_ = LazyOperationKind::FILTER;
        op.predicate_ = std::move(p);
        return op;
    }
    
    /**
     * إنشاء عملية خذ | Create take operation
     * 
     * مثال | Example:
     *   |> خذ(10)
     */
    static LazyOperation take(size_t n) {
        LazyOperation op;
        op.kind_ = LazyOperationKind::TAKE;
        op.count_ = n;
        return op;
    }
    
    /**
     * إنشاء عملية خذ_طالما | Create take_while operation
     * 
     * مثال | Example:
     *   |> خذ_طالما(|س| س < 100)
     */
    static LazyOperation takeWhile(Predicate p) {
        LazyOperation op;
        op.kind_ = LazyOperationKind::TAKE_WHILE;
        op.predicate_ = std::move(p);
        return op;
    }
    
    /**
     * إنشاء عملية أسقط | Create drop operation
     * 
     * مثال | Example:
     *   |> أسقط(5)
     */
    static LazyOperation drop(size_t n) {
        LazyOperation op;
        op.kind_ = LazyOperationKind::DROP;
        op.count_ = n;
        return op;
    }
    
    /**
     * إنشاء عملية أسقط_طالما | Create drop_while operation
     * 
     * مثال | Example:
     *   |> أسقط_طالما(|س| س < 0)
     */
    static LazyOperation dropWhile(Predicate p) {
        LazyOperation op;
        op.kind_ = LazyOperationKind::DROP_WHILE;
        op.predicate_ = std::move(p);
        return op;
    }
    
    /**
     * إنشاء عملية عدّد | Create enumerate operation
     * 
     * مثال | Example:
     *   |> عدّد()  // (0, أ), (1, ب), (2, ج)
     */
    static LazyOperation enumerate() {
        LazyOperation op;
        op.kind_ = LazyOperationKind::ENUMERATE;
        return op;
    }
    
    /**
     * إنشاء عملية دمج | Create zip operation
     * 
     * مثال | Example:
     *   |> دمج(قائمة_أخرى)
     */
    static LazyOperation zip(std::shared_ptr<LazyIterator> other) {
        LazyOperation op;
        op.kind_ = LazyOperationKind::ZIP;
        op.other_ = other;
        return op;
    }
    
    /**
     * إنشاء عملية فريد | Create unique operation
     * 
     * مثال | Example:
     *   |> فريد()  // إزالة المكررات المتتالية
     */
    static LazyOperation unique() {
        LazyOperation op;
        op.kind_ = LazyOperationKind::UNIQUE;
        return op;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                            الوصول للبيانات | Data Access
    // ═══════════════════════════════════════════════════════════════════════════
    
    LazyOperationKind kind() const { return kind_; }
    const std::optional<Predicate>& predicate() const { return predicate_; }
    const std::optional<Transform>& transform() const { return transform_; }
    std::optional<size_t> count() const { return count_; }
    const std::shared_ptr<LazyIterator>& other() const { return other_; }
    
private:
    LazyOperation() = default;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         السلسلة الكسولة | Lazy Chain
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔗 السلسلة_الكسولة | LazyChain
 * 
 * سلسلة من العمليات الكسولة على مكرر
 * Chain of lazy operations on an iterator
 * 
 * مثال | Example:
 *   مدى(1, ∞)
 *       |> صفّي(|ن| ن % 2 == 0)
 *       |> حوّل(|ن| ن * ن)
 *       |> خذ(5)
 *       |> اجمع()
 *   // النتيجة: [4, 16, 36, 64, 100]
 */
class LazyChain : public LazyIterator {
private:
    std::shared_ptr<LazyIterator> source_;
    std::vector<LazyOperation> operations_;
    
    // حالة التنفيذ
    size_t currentOp_ = 0;
    bool exhausted_ = false;
    
    // حالة العمليات المحددة
    size_t takeCount_ = 0;    // للـ take
    size_t dropCount_ = 0;    // للـ drop
    bool dropComplete_ = false;
    size_t enumIndex_ = 0;    // للـ enumerate
    std::optional<std::any> lastValue_; // للـ unique

public:
    /**
     * إنشاء سلسلة من مكرر | Create chain from iterator
     */
    explicit LazyChain(std::shared_ptr<LazyIterator> source)
        : source_(source) {}
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                          إضافة العمليات | Adding Operations
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إضافة عملية للسلسلة | Add operation to chain
     * 
     * تُرجع مرجعاً للسلسلة للتسلسل
     * Returns reference to chain for chaining
     */
    LazyChain& add(LazyOperation op) {
        operations_.push_back(std::move(op));
        return *this;
    }
    
    /**
     * حوّل | Map
     */
    LazyChain& map(LazyOperation::Transform f) {
        return add(LazyOperation::map(f));
    }
    
    /**
     * صفّي | Filter
     */
    LazyChain& filter(LazyOperation::Predicate p) {
        return add(LazyOperation::filter(p));
    }
    
    /**
     * خذ | Take
     */
    LazyChain& take(size_t n) {
        return add(LazyOperation::take(n));
    }
    
    /**
     * أسقط | Drop
     */
    LazyChain& drop(size_t n) {
        return add(LazyOperation::drop(n));
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                          تنفيذ التكرار | Iteration Execution
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::optional<std::any> next() override {
        if (exhausted_) return std::nullopt;
        
        while (true) {
            // الحصول على العنصر التالي من المصدر
            auto item = source_->next();
            if (!item) {
                exhausted_ = true;
                return std::nullopt;
            }
            
            // تطبيق كل العمليات
            bool skip = false;
            bool stop = false;
            std::any current = *item;
            
            for (size_t i = 0; i < operations_.size() && !skip && !stop; ++i) {
                const auto& op = operations_[i];
                
                switch (op.kind()) {
                    case LazyOperationKind::FILTER: {
                        // صفّي - تحقق من الشرط
                        if (op.predicate() && !(*op.predicate())(current)) {
                            skip = true;  // تخطي هذا العنصر
                        }
                        break;
                    }
                    
                    case LazyOperationKind::MAP: {
                        // حوّل - تحويل القيمة
                        if (op.transform()) {
                            current = (*op.transform())(current);
                        }
                        break;
                    }
                    
                    case LazyOperationKind::TAKE: {
                        // خذ - حد أقصى
                        if (op.count() && takeCount_ >= *op.count()) {
                            stop = true;
                            exhausted_ = true;
                        } else {
                            takeCount_++;
                        }
                        break;
                    }
                    
                    case LazyOperationKind::TAKE_WHILE: {
                        // خذ_طالما - توقف عند فشل الشرط
                        if (op.predicate() && !(*op.predicate())(current)) {
                            stop = true;
                            exhausted_ = true;
                        }
                        break;
                    }
                    
                    case LazyOperationKind::DROP: {
                        // أسقط - تخطي أول n
                        if (!dropComplete_ && op.count()) {
                            if (dropCount_ < *op.count()) {
                                dropCount_++;
                                skip = true;
                            } else {
                                dropComplete_ = true;
                            }
                        }
                        break;
                    }
                    
                    case LazyOperationKind::DROP_WHILE: {
                        // أسقط_طالما - تخطي طالما الشرط صحيح
                        if (!dropComplete_ && op.predicate()) {
                            if ((*op.predicate())(current)) {
                                skip = true;
                            } else {
                                dropComplete_ = true;
                            }
                        }
                        break;
                    }
                    
                    case LazyOperationKind::ENUMERATE: {
                        // عدّد - إضافة الفهرس
                        // نُنتج زوجاً (فهرس، قيمة)
                        struct EnumPair {
                            size_t index;
                            std::any value;
                        };
                        current = EnumPair{enumIndex_++, current};
                        break;
                    }
                    
                    case LazyOperationKind::UNIQUE: {
                        // فريد - إزالة المكررات المتتالية
                        // ملاحظة: هذا يتطلب مقارنة مخصصة حسب النوع
                        // للتبسيط نفترض الأعداد
                        if (lastValue_.has_value()) {
                            try {
                                auto last = std::any_cast<int64_t>(*lastValue_);
                                auto curr = std::any_cast<int64_t>(current);
                                if (last == curr) {
                                    skip = true;
                                }
                            } catch (...) {
                                // أنواع مختلفة، لا نتخطى
                            }
                        }
                        if (!skip) {
                            lastValue_ = current;
                        }
                        break;
                    }
                    
                    default:
                        // عمليات أخرى...
                        break;
                }
            }
            
            if (stop) return std::nullopt;
            if (!skip) return current;
            // إذا skip، نستمر في الحلقة
        }
    }
    
    std::optional<std::any> peek() override {
        // للتبسيط، peek غير مدعوم في السلسلة
        return std::nullopt;
    }
    
    bool isExhausted() const override {
        return exhausted_;
    }
    
    std::string elementType() const override {
        // نوع العنصر يعتمد على العمليات
        return source_->elementType();  // تبسيط
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                            الجمع | Collection
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * جمع كل العناصر في قائمة | Collect all elements into a list
     * 
     * ⚠️ تحذير: لا تستخدم مع مكررات لانهائية!
     * ⚠️ Warning: Don't use with infinite iterators!
     */
    template<typename T>
    std::vector<T> collect() {
        std::vector<T> result;
        while (auto item = next()) {
            try {
                result.push_back(std::any_cast<T>(*item));
            } catch (...) {
                // تجاهل العناصر التي لا يمكن تحويلها
            }
        }
        return result;
    }
    
    /**
     * طي/تجميع | Fold/Reduce
     */
    template<typename T, typename Acc>
    Acc fold(Acc initial, std::function<Acc(Acc, T)> f) {
        Acc acc = initial;
        while (auto item = next()) {
            try {
                acc = f(acc, std::any_cast<T>(*item));
            } catch (...) {
                // تجاهل
            }
        }
        return acc;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                           مدير التأخير | Thunk Manager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 💤 التأخير | Thunk
 * 
 * حساب مؤجل مع تخزين مؤقت (memoization)
 * Deferred computation with memoization
 */
template<typename T>
class Thunk {
private:
    std::function<T()> computation_;
    mutable std::optional<T> cached_;
    mutable bool evaluated_ = false;

public:
    /**
     * إنشاء تأخير | Create thunk
     * 
     * مثال | Example:
     *   دع كسول = مؤجل { عملية_مكلفة() }
     */
    explicit Thunk(std::function<T()> computation)
        : computation_(std::move(computation)) {}
    
    /**
     * إجبار التقييم | Force evaluation
     */
    const T& force() const {
        if (!evaluated_) {
            cached_ = computation_();
            evaluated_ = true;
        }
        return *cached_;
    }
    
    /**
     * هل تم التقييم؟ | Is evaluated?
     */
    bool isEvaluated() const {
        return evaluated_;
    }
    
    /**
     * عامل الاستدعاء | Call operator
     */
    const T& operator()() const {
        return force();
    }
};

/**
 * 📦 مدير_التأخيرات | ThunkManager
 * 
 * يدير مجموعة من التأخيرات المسماة
 * Manages a collection of named thunks
 */
class ThunkManager {
private:
    struct ThunkEntry {
        std::function<std::any()> computation;
        std::optional<std::any> cached;
        bool evaluated = false;
        std::string type;
    };
    
    std::unordered_map<std::string, ThunkEntry> thunks_;

public:
    /**
     * تسجيل تأخير | Register thunk
     */
    template<typename T>
    void registerThunk(const std::string& name, std::function<T()> computation) {
        ThunkEntry entry;
        entry.computation = [computation]() -> std::any {
            return computation();
        };
        entry.type = typeid(T).name();
        thunks_[name] = std::move(entry);
    }
    
    /**
     * إجبار تقييم تأخير | Force thunk evaluation
     */
    std::optional<std::any> force(const std::string& name) {
        auto it = thunks_.find(name);
        if (it == thunks_.end()) return std::nullopt;
        
        auto& entry = it->second;
        if (!entry.evaluated) {
            entry.cached = entry.computation();
            entry.evaluated = true;
        }
        return entry.cached;
    }
    
    /**
     * هل تم التقييم؟ | Is evaluated?
     */
    bool isEvaluated(const std::string& name) const {
        auto it = thunks_.find(name);
        return it != thunks_.end() && it->second.evaluated;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                      محول تعبير المولد | Generator Expression Transformer
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔄 محول_تعبير_المولد | GeneratorExpressionTransformer
 * 
 * يحول تعبير المولد إلى شجرة العمليات الكسولة
 * Transforms generator expression to lazy operation tree
 */
class GeneratorExpressionTransformer {
public:
    /**
     * 📝 نتيجة_التحويل | TransformResult
     */
    struct TransformResult {
        std::shared_ptr<LazyChain> chain;
        std::string elementType;
        bool isInfinite;
        std::vector<std::string> capturedVariables;
    };
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                         تحليل تعبير المولد | Parse Generator Expression
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * معالجة تعبير مولد | Process generator expression
     * 
     * يحول:
     *   (تعبير لكل متغير في متكرر إذا شرط)
     * إلى:
     *   متكرر |> صفّي(شرط) |> حوّل(تعبير)
     */
    TransformResult transform(
        const std::string& expression,  // التعبير
        const std::string& variable,    // اسم المتغير
        const std::string& iterable,    // المتكرر
        const std::string& condition    // الشرط (اختياري)
    ) {
        TransformResult result;
        result.isInfinite = false;
        result.capturedVariables.push_back(variable);
        
        // تحليل المتكرر
        auto source = analyzeIterable(iterable, result.isInfinite);
        
        // إنشاء السلسلة
        result.chain = std::make_shared<LazyChain>(source);
        
        // إضافة الترشيح إن وجد
        if (!condition.empty()) {
            // هنا نحتاج لتحويل الشرط إلى دالة
            // للتبسيط نفترض أن الشرط سيُترجم لاحقاً
        }
        
        // إضافة التحويل
        // هنا نحتاج لتحويل التعبير إلى دالة
        // للتبسيط نفترض أن التعبير سيُترجم لاحقاً
        
        return result;
    }
    
private:
    /**
     * تحليل نوع المتكرر | Analyze iterable type
     */
    std::shared_ptr<LazyIterator> analyzeIterable(
        const std::string& iterable,
        bool& isInfinite
    ) {
        // تحليل بسيط: هل هو مدى؟
        // مدى(1, 10) أو مدى(لانهاية) أو range(1, 10)
        
        // للتبسيط نُنشئ مدى افتراضي
        isInfinite = (iterable.find("لانهاية") != std::string::npos ||
                      iterable.find("infinity") != std::string::npos);
        
        if (isInfinite) {
            return std::make_shared<LazyRangeIterator>(
                LazyRangeIterator::infinite(0, 1)
            );
        }
        
        // مدى محدود افتراضي
        return std::make_shared<LazyRangeIterator>(0, 100, 1);
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                      مولد كود التكرار الكسول | Lazy Iteration Codegen
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 مولد_كود_الكسل | LazyCodegen
 * 
 * يولد كود LLVM IR للتكرار الكسول
 * Generates LLVM IR for lazy iteration
 */
class LazyCodegen {
public:
    /**
     * 📋 معلومات_الهيكل | StructInfo
     * 
     * معلومات الهيكل المُولد للمكرر الكسول
     */
    struct StructInfo {
        std::string name;
        std::vector<std::pair<std::string, std::string>> fields;
        std::string vtableName;
    };
    
    /**
     * توليد هيكل المكرر | Generate iterator struct
     * 
     * يُنتج هيكل يحتوي:
     * - مؤشر vtable للعمليات
     * - حالة المكرر
     * - المتغيرات الملتقطة
     */
    StructInfo generateIteratorStruct(
        const std::string& name,
        LazyKind kind,
        const std::vector<std::pair<std::string, std::string>>& captured
    ) {
        StructInfo info;
        info.name = name + "_مكرر";
        info.vtableName = name + "_جدول_عمليات";
        
        // إضافة الحقول الأساسية
        info.fields.push_back({"_vtable", "ptr"});
        info.fields.push_back({"_state", "i32"});
        info.fields.push_back({"_resumePoint", "i32"});
        
        // إضافة المتغيرات الملتقطة
        for (const auto& [varName, varType] : captured) {
            info.fields.push_back({varName, varType});
        }
        
        return info;
    }
    
    /**
     * توليد دالة next | Generate next function
     * 
     * تُنتج الكود اللازم لاستئناف المولد من نقطة yield
     */
    std::string generateNextFunction(
        const std::string& iteratorName,
        const std::vector<int>& yieldPoints
    ) {
        std::stringstream ss;
        
        ss << "define ptr @" << iteratorName << "_التالي(ptr %self) {\n";
        ss << "entry:\n";
        ss << "  ; تحميل نقطة الاستئناف\n";
        ss << "  %resume_ptr = getelementptr inbounds %." << iteratorName << ", ptr %self, i32 0, i32 2\n";
        ss << "  %resume_point = load i32, ptr %resume_ptr\n";
        ss << "  ; القفز لنقطة الاستئناف المناسبة\n";
        ss << "  switch i32 %resume_point, label %end [\n";
        
        for (size_t i = 0; i < yieldPoints.size(); ++i) {
            ss << "    i32 " << i << ", label %yield_" << i << "\n";
        }
        
        ss << "  ]\n\n";
        
        // توليد نقاط yield / Generate yield points
        for (size_t i = 0; i < yieldPoints.size(); ++i) {
            ss << "yield_" << i << ":\n";
            ss << "  ; Yield point " << i << "\n";
            ss << "  br label %end\n\n";
        }
        
        ss << "end:\n";
        ss << "  ret ptr null\n";
        ss << "}\n";
        
        return ss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         تحسينات السلسلة | Chain Optimizations
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ⚡ محسّن_السلسلة | ChainOptimizer
 * 
 * يُحسّن سلسلة العمليات الكسولة
 * Optimizes lazy operation chains
 */
class ChainOptimizer {
public:
    /**
     * دمج الفلاتر المتتالية | Fuse consecutive filters
     * 
     * صفّي(ش1) |> صفّي(ش2) → صفّي(ش1 و ش2)
     */
    static void fuseFilters(std::vector<LazyOperation>& ops) {
        std::vector<LazyOperation> optimized;
        
        for (size_t i = 0; i < ops.size(); ++i) {
            if (ops[i].kind() == LazyOperationKind::FILTER &&
                i + 1 < ops.size() &&
                ops[i + 1].kind() == LazyOperationKind::FILTER) {
                // دمج الفلترين
                auto p1 = ops[i].predicate();
                auto p2 = ops[i + 1].predicate();
                if (p1 && p2) {
                    auto combined = [p1, p2](const std::any& x) {
                        return (*p1)(x) && (*p2)(x);
                    };
                    optimized.push_back(LazyOperation::filter(combined));
                    ++i;  // تخطي الفلتر التالي
                    continue;
                }
            }
            optimized.push_back(ops[i]);
        }
        
        ops = std::move(optimized);
    }
    
    /**
     * تحريك take للأعلى | Move take upward
     * 
     * حوّل(f) |> خذ(n) → خذ(n) |> حوّل(f)
     * هذا يقلل عدد التحويلات
     */
    static void moveTakeUp(std::vector<LazyOperation>& ops) {
        // البحث عن take وتحريكه قبل map إن أمكن
        for (size_t i = 1; i < ops.size(); ++i) {
            if (ops[i].kind() == LazyOperationKind::TAKE &&
                ops[i - 1].kind() == LazyOperationKind::MAP) {
                std::swap(ops[i], ops[i - 1]);
            }
        }
    }
    
    /**
     * إزالة العمليات المتعاكسة | Remove inverse operations
     * 
     * أسقط(n) |> خذ(m) حيث m > n يمكن تبسيطه
     */
    static void removeInverseOps(std::vector<LazyOperation>& ops) {
        // تنفيذ مبسط
    }
    
    /**
     * تحسين شامل | Full optimization
     */
    static void optimize(std::vector<LazyOperation>& ops) {
        fuseFilters(ops);
        moveTakeUp(ops);
        removeInverseOps(ops);
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         الدوال المساعدة | Helper Functions
// ═══════════════════════════════════════════════════════════════════════════════

namespace helpers {

/**
 * إنشاء مدى كسول | Create lazy range
 * 
 * مثال | Example:
 *   دع أرقام = مدى_كسول(1, 100)
 */
inline std::shared_ptr<LazyChain> lazyRange(int64_t start, int64_t end, int64_t step = 1) {
    auto source = std::make_shared<LazyRangeIterator>(start, end, step);
    return std::make_shared<LazyChain>(source);
}

/**
 * إنشاء مدى لانهائي كسول | Create lazy infinite range
 * 
 * مثال | Example:
 *   دع طبيعية = مدى_لانهائي(1)
 */
inline std::shared_ptr<LazyChain> lazyInfiniteRange(int64_t start = 0, int64_t step = 1) {
    auto source = std::make_shared<LazyRangeIterator>(
        LazyRangeIterator::infinite(start, step)
    );
    return std::make_shared<LazyChain>(source);
}

/**
 * تحويل قائمة لمكرر كسول | Convert list to lazy iterator
 * 
 * مثال | Example:
 *   دع كسول = كسول_من([1, 2, 3, 4, 5])
 */
template<typename T>
class LazyListIterator : public LazyIterator {
private:
    std::vector<T> items_;
    size_t index_ = 0;
    
public:
    explicit LazyListIterator(std::vector<T> items)
        : items_(std::move(items)) {}
    
    std::optional<std::any> next() override {
        if (index_ >= items_.size()) return std::nullopt;
        return items_[index_++];
    }
    
    std::optional<std::any> peek() override {
        if (index_ >= items_.size()) return std::nullopt;
        return items_[index_];
    }
    
    bool isExhausted() const override {
        return index_ >= items_.size();
    }
    
    std::optional<size_t> sizeHint() const override {
        return items_.size() - index_;
    }
    
    std::string elementType() const override {
        return typeid(T).name();
    }
};

template<typename T>
std::shared_ptr<LazyChain> lazyFrom(std::vector<T> items) {
    auto source = std::make_shared<LazyListIterator<T>>(std::move(items));
    return std::make_shared<LazyChain>(source);
}

} // namespace helpers

} // namespace lazy
} // namespace compiler
} // namespace sad

#include <sstream>

// ═══════════════════════════════════════════════════════════════════════════════
//                              اختبارات | Tests
// ═══════════════════════════════════════════════════════════════════════════════

#ifdef SAD_LAZY_TESTS

#include <iostream>

void testLazyRange() {
    using namespace sad::compiler::lazy;
    
    std::cout << "═══ اختبار المدى الكسول ═══\n";
    
    // مدى محدود
    auto range = helpers::lazyRange(1, 10, 2);
    range->filter([](const std::any& x) {
        return std::any_cast<int64_t>(x) > 3;
    })->map([](const std::any& x) {
        auto n = std::any_cast<int64_t>(x);
        return n * n;
    });
    
    std::cout << "مربعات الأرقام > 3 في [1,10) بخطوة 2:\n";
    while (auto item = range->next()) {
        std::cout << "  " << std::any_cast<int64_t>(*item) << "\n";
    }
}

void testLazyInfinite() {
    using namespace sad::compiler::lazy;
    
    std::cout << "\n═══ اختبار المدى اللانهائي ═══\n";
    
    auto infinite = helpers::lazyInfiniteRange(1, 1);
    infinite->filter([](const std::any& x) {
        auto n = std::any_cast<int64_t>(x);
        return n % 2 == 0;  // أرقام زوجية
    })->map([](const std::any& x) {
        auto n = std::any_cast<int64_t>(x);
        return n * n;  // مربعات
    })->take(5);
    
    std::cout << "أول 5 مربعات زوجية:\n";
    auto results = infinite->collect<int64_t>();
    for (auto r : results) {
        std::cout << "  " << r << "\n";
    }
}

void testThunk() {
    using namespace sad::compiler::lazy;
    
    std::cout << "\n═══ اختبار التأخير ═══\n";
    
    int computeCount = 0;
    Thunk<int> lazy([&computeCount]() {
        std::cout << "  جاري الحساب...\n";
        computeCount++;
        return 42;
    });
    
    std::cout << "قبل أول استدعاء\n";
    std::cout << "  القيمة: " << lazy.force() << "\n";
    std::cout << "  عدد مرات الحساب: " << computeCount << "\n";
    
    std::cout << "بعد ثاني استدعاء\n";
    std::cout << "  القيمة: " << lazy.force() << "\n";
    std::cout << "  عدد مرات الحساب: " << computeCount << " (نفس العدد - مخزن مؤقتاً)\n";
}

int main() {
    testLazyRange();
    testLazyInfinite();
    testThunk();
    return 0;
}

#endif // SAD_LAZY_TESTS
