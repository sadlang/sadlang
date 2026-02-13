// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4819)
#endif

/**
 * @file smart_ptr.h
 * @brief المؤشرات الذكية - عداد المراجع / Smart Pointers - Reference Counting
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 * @phase المرحلة 4: US2 - أمان الذاكرة
 * 
 * =============================================================================
 *                    المؤشرات الذكية في لغة ص
 *                    Smart Pointers in Sad Language
 * =============================================================================
 * 
 *     لغة ص تدعم وضعين للذاكرة:
 *     
 *     1. **الوضع الآمن (افتراضي)**: نظام ملكية صارم مثل Rust
 *     2. **الوضع المُدار**: عداد مراجع ذكي مثل Swift
 *     
 *     هذا الملف يُنفِّذ المؤشرات الذكية للوضع المُدار:
 *     
 *     • `Rc<T>`  - عداد مراجع (خيط واحد)
 *     • `Arc<T>` - عداد مراجع ذري (متعدد الخيوط)
 *     • `Weak<T>` - مرجع ضعيف (لا يمنع الحذف)
 * 
 * =============================================================================
 *                    متى نستخدم كل نوع؟
 * =============================================================================
 * 
 *     ┌─────────────────────────────────────────────────────────────────────┐
 *     │  النوع     │ الاستخدام                                              │
 *     ├─────────────────────────────────────────────────────────────────────┤
 *     │  Rc<T>     │ مشاركة البيانات في خيط واحد                           │
 *     │  Arc<T>    │ مشاركة البيانات بين خيوط متعددة                        │
 *     │  Weak<T>   │ كسر الدورات المرجعية                                   │
 *     └─────────────────────────────────────────────────────────────────────┘
 * 
 *     **مثال Rc:**
 *     ```sad
 *     دع أ = عم<عدد>.جديد(42)     // إنشاء
 *     دع ب = أ.استنسخ()           // مشاركة (العداد = 2)
 *     اطبع(*أ)                    // استخدام
 *     ```
 *     
 *     **مثال Arc:**
 *     ```sad
 *     دع مشترك = عم_ذري<قائمة<عدد>>.جديد([1، 2، 3])
 *     
 *     خيط.أنشئ(|| {
 *         دع نسخة = مشترك.استنسخ()
 *         // استخدام آمن بين الخيوط
 *     })
 *     ```
 */

#ifndef SAD_SEMANTIC_SMART_PTR_H
#define SAD_SEMANTIC_SMART_PTR_H

#include <atomic>
#include <memory>
#include <type_traits>
#include <stdexcept>
#include <iostream>

namespace Sad {
namespace Runtime {

// =============================================================================
//                    كتلة التحكم / Control Block
// =============================================================================

/**
 * @class ControlBlock
 * @brief كتلة التحكم لعداد المراجع / Control block for reference counting
 * 
 *     تُخزِّن عدد المراجع القوية والضعيفة
 */
template<typename T>
class ControlBlock {
public:
    /// (AR) عدد المراجع القوية / (EN) Strong reference count
    std::atomic<size_t> strongCount{1};
    
    /// (AR) عدد المراجع الضعيفة / (EN) Weak reference count
    std::atomic<size_t> weakCount{0};
    
    /// (AR) البيانات المُدارة / (EN) Managed data
    T* data = nullptr;
    
    /**
     * @brief المنشئ / Constructor
     */
    explicit ControlBlock(T* ptr) : data(ptr) {}
    
    /**
     * @brief الهادم / Destructor
     */
    ~ControlBlock() {
        // (AR) البيانات تُحذف عندما تصل المراجع القوية لصفر
        // (EN) Data deleted when strong count reaches zero
    }
    
    /**
     * @brief زيادة العداد القوي / Increment strong count
     */
    void incrementStrong() {
        strongCount.fetch_add(1, std::memory_order_relaxed);
    }
    
    /**
     * @brief إنقاص العداد القوي / Decrement strong count
     * @return true إذا وصل العداد لصفر
     */
    bool decrementStrong() {
        size_t prev = strongCount.fetch_sub(1, std::memory_order_acq_rel);
        return prev == 1;
    }
    
    /**
     * @brief زيادة العداد الضعيف / Increment weak count
     */
    void incrementWeak() {
        weakCount.fetch_add(1, std::memory_order_relaxed);
    }
    
    /**
     * @brief إنقاص العداد الضعيف / Decrement weak count
     * @return true إذا وصل العداد لصفر
     */
    bool decrementWeak() {
        size_t prev = weakCount.fetch_sub(1, std::memory_order_acq_rel);
        return prev == 1;
    }
    
    /**
     * @brief هل البيانات ما زالت موجودة؟ / Is data still alive?
     */
    bool isAlive() const {
        return strongCount.load(std::memory_order_acquire) > 0;
    }
};

// =============================================================================
//                    Rc<T> - عداد المراجع / Reference Counter
// =============================================================================

/**
 * @class Rc
 * @brief عداد مراجع لخيط واحد / Single-threaded reference counter
 * 
 * (AR) يُستخدم لمشاركة البيانات داخل خيط واحد. أسرع من Arc لكن
 *      ليس آمناً للخيوط المتعددة.
 * 
 * (EN) Used for sharing data within a single thread. Faster than Arc but
 *      not thread-safe.
 * 
 * **مثال / Example:**
 * ```cpp
 * auto ptr = Rc<int>::create(42);
 * auto clone = ptr.clone();
 * std::cout << *ptr << "\n";  // 42
 * ```
 */
template<typename T>
class Rc {
public:
    // =========================================================================
    //                    البناء / Construction
    // =========================================================================
    
    /**
     * @brief منشئ افتراضي (فارغ) / Default constructor (empty)
     */
    Rc() : control_(nullptr) {}
    
    /**
     * @brief إنشاء من قيمة / Create from value
     * 
     *     مثال:
     *         auto ptr = Rc<int>::create(42);
     */
    static Rc<T> create(T value) {
        Rc<T> rc;
        rc.control_ = new ControlBlock<T>(new T(std::move(value)));
        return rc;
    }
    
    /**
     * @brief إنشاء في المكان / Create in-place
     * 
     *     مثال:
     *         auto ptr = Rc<std::string>::makeInPlace("مرحباً");
     */
    template<typename... Args>
    static Rc<T> makeInPlace(Args&&... args) {
        Rc<T> rc;
        rc.control_ = new ControlBlock<T>(new T(std::forward<Args>(args)...));
        return rc;
    }
    
    /**
     * @brief منشئ النسخ / Copy constructor
     */
    Rc(const Rc& other) : control_(other.control_) {
        if (control_) {
            control_->incrementStrong();
        }
    }
    
    /**
     * @brief منشئ النقل / Move constructor
     */
    Rc(Rc&& other) noexcept : control_(other.control_) {
        other.control_ = nullptr;
    }
    
    /**
     * @brief الهادم / Destructor
     */
    ~Rc() {
        release();
    }
    
    // =========================================================================
    //                    التعيين / Assignment
    // =========================================================================
    
    /**
     * @brief عامل تعيين النسخ / Copy assignment
     */
    Rc& operator=(const Rc& other) {
        if (this != &other) {
            release();
            control_ = other.control_;
            if (control_) {
                control_->incrementStrong();
            }
        }
        return *this;
    }
    
    /**
     * @brief عامل تعيين النقل / Move assignment
     */
    Rc& operator=(Rc&& other) noexcept {
        if (this != &other) {
            release();
            control_ = other.control_;
            other.control_ = nullptr;
        }
        return *this;
    }
    
    // =========================================================================
    //                    الوصول / Access
    // =========================================================================
    
    /**
     * @brief إلغاء المرجعية / Dereference
     */
    T& operator*() {
        if (!control_ || !control_->data) {
            throw std::runtime_error("خطأ: محاولة الوصول لمؤشر فارغ!");
        }
        return *control_->data;
    }
    
    const T& operator*() const {
        if (!control_ || !control_->data) {
            throw std::runtime_error("خطأ: محاولة الوصول لمؤشر فارغ!");
        }
        return *control_->data;
    }
    
    /**
     * @brief الوصول للأعضاء / Member access
     */
    T* operator->() {
        if (!control_ || !control_->data) {
            throw std::runtime_error("خطأ: محاولة الوصول لمؤشر فارغ!");
        }
        return control_->data;
    }
    
    const T* operator->() const {
        if (!control_ || !control_->data) {
            throw std::runtime_error("خطأ: محاولة الوصول لمؤشر فارغ!");
        }
        return control_->data;
    }
    
    /**
     * @brief الحصول على المؤشر الخام / Get raw pointer
     */
    T* get() { return control_ ? control_->data : nullptr; }
    const T* get() const { return control_ ? control_->data : nullptr; }
    
    // =========================================================================
    //                    الاستعلام / Querying
    // =========================================================================
    
    /**
     * @brief هل المؤشر صالح؟ / Is pointer valid?
     */
    explicit operator bool() const {
        return control_ && control_->data;
    }
    
    /**
     * @brief عدد المراجع / Reference count
     */
    size_t useCount() const {
        return control_ ? control_->strongCount.load() : 0;
    }
    
    /**
     * @brief هل هذا المرجع الوحيد؟ / Is this the only reference?
     */
    bool unique() const {
        return useCount() == 1;
    }
    
    // =========================================================================
    //                    الاستنساخ / Cloning
    // =========================================================================
    
    /**
     * @brief استنساخ المؤشر (زيادة العداد) / Clone pointer (increment count)
     */
    Rc<T> clone() const {
        Rc<T> copy;
        copy.control_ = control_;
        if (control_) {
            control_->incrementStrong();
        }
        return copy;
    }
    
    // =========================================================================
    //                    التنقيح / Debugging
    // =========================================================================
    
    /**
     * @brief طباعة معلومات التنقيح / Print debug info
     */
    void dump() const {
        std::cout << "[عم<T>] العنوان: " << (void*)control_;
        if (control_) {
            std::cout << " | العداد القوي: " << control_->strongCount.load()
                      << " | العداد الضعيف: " << control_->weakCount.load();
        }
        std::cout << "\n";
    }

private:
    ControlBlock<T>* control_;
    
    /**
     * @brief تحرير المرجع / Release reference
     */
    void release() {
        if (control_) {
            if (control_->decrementStrong()) {
                // (AR) آخر مرجع قوي - احذف البيانات
                // (EN) Last strong reference - delete data
                delete control_->data;
                control_->data = nullptr;
                
                // (AR) إذا لا توجد مراجع ضعيفة، احذف كتلة التحكم
                // (EN) If no weak references, delete control block
                if (control_->weakCount.load() == 0) {
                    delete control_;
                }
            }
            control_ = nullptr;
        }
    }
    
    // (AR) السماح لـ Weak بالوصول / (EN) Allow Weak access
    template<typename U> friend class Weak;
};

// =============================================================================
//                    Arc<T> - عداد مراجع ذري / Atomic Reference Counter
// =============================================================================

/**
 * @class Arc
 * @brief عداد مراجع ذري للخيوط المتعددة / Thread-safe atomic reference counter
 * 
 * (AR) مشابه لـ Rc لكن آمن للخيوط المتعددة. يستخدم عمليات ذرية.
 * (EN) Similar to Rc but thread-safe. Uses atomic operations.
 * 
 * **مثال / Example:**
 * ```cpp
 * auto shared = Arc<std::vector<int>>::create({1, 2, 3});
 * 
 * std::thread t([clone = shared.clone()]() {
 *     // استخدام آمن
 * });
 * ```
 */
template<typename T>
class Arc {
public:
    // =========================================================================
    //                    البناء / Construction
    // =========================================================================
    
    /**
     * @brief منشئ افتراضي (فارغ) / Default constructor (empty)
     */
    Arc() : control_(nullptr) {}
    
    /**
     * @brief إنشاء من قيمة / Create from value
     */
    static Arc<T> create(T value) {
        Arc<T> arc;
        arc.control_ = new ControlBlock<T>(new T(std::move(value)));
        return arc;
    }
    
    /**
     * @brief إنشاء في المكان / Create in-place
     */
    template<typename... Args>
    static Arc<T> makeInPlace(Args&&... args) {
        Arc<T> arc;
        arc.control_ = new ControlBlock<T>(new T(std::forward<Args>(args)...));
        return arc;
    }
    
    /**
     * @brief منشئ النسخ / Copy constructor
     */
    Arc(const Arc& other) : control_(other.control_) {
        if (control_) {
            control_->incrementStrong();
        }
    }
    
    /**
     * @brief منشئ النقل / Move constructor
     */
    Arc(Arc&& other) noexcept : control_(other.control_) {
        other.control_ = nullptr;
    }
    
    /**
     * @brief الهادم / Destructor
     */
    ~Arc() {
        release();
    }
    
    // =========================================================================
    //                    التعيين / Assignment
    // =========================================================================
    
    Arc& operator=(const Arc& other) {
        if (this != &other) {
            release();
            control_ = other.control_;
            if (control_) {
                control_->incrementStrong();
            }
        }
        return *this;
    }
    
    Arc& operator=(Arc&& other) noexcept {
        if (this != &other) {
            release();
            control_ = other.control_;
            other.control_ = nullptr;
        }
        return *this;
    }
    
    // =========================================================================
    //                    الوصول / Access
    // =========================================================================
    
    T& operator*() {
        if (!control_ || !control_->data) {
            throw std::runtime_error("خطأ: محاولة الوصول لمؤشر ذري فارغ!");
        }
        return *control_->data;
    }
    
    const T& operator*() const {
        if (!control_ || !control_->data) {
            throw std::runtime_error("خطأ: محاولة الوصول لمؤشر ذري فارغ!");
        }
        return *control_->data;
    }
    
    T* operator->() {
        if (!control_ || !control_->data) {
            throw std::runtime_error("خطأ: محاولة الوصول لمؤشر ذري فارغ!");
        }
        return control_->data;
    }
    
    const T* operator->() const {
        if (!control_ || !control_->data) {
            throw std::runtime_error("خطأ: محاولة الوصول لمؤشر ذري فارغ!");
        }
        return control_->data;
    }
    
    T* get() { return control_ ? control_->data : nullptr; }
    const T* get() const { return control_ ? control_->data : nullptr; }
    
    // =========================================================================
    //                    الاستعلام / Querying
    // =========================================================================
    
    explicit operator bool() const {
        return control_ && control_->data;
    }
    
    size_t useCount() const {
        return control_ ? control_->strongCount.load(std::memory_order_acquire) : 0;
    }
    
    bool unique() const {
        return useCount() == 1;
    }
    
    // =========================================================================
    //                    الاستنساخ / Cloning
    // =========================================================================
    
    Arc<T> clone() const {
        Arc<T> copy;
        copy.control_ = control_;
        if (control_) {
            control_->incrementStrong();
        }
        return copy;
    }
    
    // =========================================================================
    //                    التنقيح / Debugging
    // =========================================================================
    
    void dump() const {
        std::cout << "[عم_ذري<T>] العنوان: " << (void*)control_;
        if (control_) {
            std::cout << " | العداد القوي: " << control_->strongCount.load()
                      << " | العداد الضعيف: " << control_->weakCount.load();
        }
        std::cout << "\n";
    }

private:
    ControlBlock<T>* control_;
    
    void release() {
        if (control_) {
            if (control_->decrementStrong()) {
                delete control_->data;
                control_->data = nullptr;
                
                if (control_->weakCount.load(std::memory_order_acquire) == 0) {
                    delete control_;
                }
            }
            control_ = nullptr;
        }
    }
    
    template<typename U> friend class Weak;
    template<typename U> friend class Arc;
};

// =============================================================================
//                    Weak<T> - مرجع ضعيف / Weak Reference
// =============================================================================

/**
 * @class Weak
 * @brief مرجع ضعيف لا يمنع الحذف / Weak reference that doesn't prevent deletion
 * 
 * (AR) يُستخدم لكسر الدورات المرجعية. لا يُبقي الكائن حياً.
 * (EN) Used to break reference cycles. Doesn't keep object alive.
 * 
 * **مثال / Example:**
 * ```cpp
 * auto strong = Rc<int>::create(42);
 * Weak<int> weak = Weak<int>::fromRc(strong);
 * 
 * if (auto locked = weak.upgrade()) {
 *     std::cout << *locked << "\n";
 * }
 * ```
 */
template<typename T>
class Weak {
public:
    /**
     * @brief منشئ افتراضي / Default constructor
     */
    Weak() : control_(nullptr) {}
    
    /**
     * @brief إنشاء من Rc / Create from Rc
     */
    static Weak<T> fromRc(const Rc<T>& rc) {
        Weak<T> weak;
        weak.control_ = rc.control_;
        if (weak.control_) {
            weak.control_->incrementWeak();
        }
        return weak;
    }
    
    /**
     * @brief إنشاء من Arc / Create from Arc
     */
    static Weak<T> fromArc(const Arc<T>& arc) {
        Weak<T> weak;
        weak.control_ = arc.control_;
        if (weak.control_) {
            weak.control_->incrementWeak();
        }
        return weak;
    }
    
    /**
     * @brief منشئ النسخ / Copy constructor
     */
    Weak(const Weak& other) : control_(other.control_) {
        if (control_) {
            control_->incrementWeak();
        }
    }
    
    /**
     * @brief منشئ النقل / Move constructor
     */
    Weak(Weak&& other) noexcept : control_(other.control_) {
        other.control_ = nullptr;
    }
    
    /**
     * @brief الهادم / Destructor
     */
    ~Weak() {
        release();
    }
    
    // =========================================================================
    //                    التعيين / Assignment
    // =========================================================================
    
    Weak& operator=(const Weak& other) {
        if (this != &other) {
            release();
            control_ = other.control_;
            if (control_) {
                control_->incrementWeak();
            }
        }
        return *this;
    }
    
    Weak& operator=(Weak&& other) noexcept {
        if (this != &other) {
            release();
            control_ = other.control_;
            other.control_ = nullptr;
        }
        return *this;
    }
    
    // =========================================================================
    //                    الترقية / Upgrading
    // =========================================================================
    
    /**
     * @brief ترقية لمرجع قوي / Upgrade to strong reference
     * 
     * @return Rc صالح إذا كان الكائن ما زال حياً، فارغ إذا حُذف
     */
    Rc<T> upgradeToRc() const {
        if (control_ && control_->isAlive()) {
            control_->incrementStrong();
            Rc<T> rc;
            rc.control_ = control_;
            return rc;
        }
        return Rc<T>();
    }
    
    /**
     * @brief ترقية لمرجع ذري قوي / Upgrade to strong Arc reference
     */
    Arc<T> upgradeToArc() const {
        if (control_ && control_->isAlive()) {
            control_->incrementStrong();
            Arc<T> arc;
            arc.control_ = control_;
            return arc;
        }
        return Arc<T>();
    }
    
    // =========================================================================
    //                    الاستعلام / Querying
    // =========================================================================
    
    /**
     * @brief هل الكائن ما زال حياً؟ / Is object still alive?
     */
    bool expired() const {
        return !control_ || !control_->isAlive();
    }
    
    /**
     * @brief عدد المراجع القوية / Strong reference count
     */
    size_t useCount() const {
        return control_ ? control_->strongCount.load() : 0;
    }
    
    /**
     * @brief عدد المراجع الضعيفة / Weak reference count
     */
    size_t weakCount() const {
        return control_ ? control_->weakCount.load() : 0;
    }

private:
    ControlBlock<T>* control_;
    
    void release() {
        if (control_) {
            if (control_->decrementWeak()) {
                // (AR) إذا لا توجد مراجع قوية أيضاً، احذف كتلة التحكم
                // (EN) If no strong refs either, delete control block
                if (control_->strongCount.load() == 0) {
                    delete control_;
                }
            }
            control_ = nullptr;
        }
    }
};

// =============================================================================
//                    الأسماء العربية / Arabic Aliases
// =============================================================================

/**
 * @brief عم<T> - عداد مراجع (Rc)
 * 
 *     مثال:
 *         دع س = عم<عدد>.جديد(42)
 */
template<typename T>
using عم = Rc<T>;

/**
 * @brief عم_ذري<T> - عداد مراجع ذري (Arc)
 * 
 *     مثال:
 *         دع س = عم_ذري<نص>.جديد("مرحباً")
 */
template<typename T>
using عم_ذري = Arc<T>;

/**
 * @brief مرجع_ضعيف<T> - مرجع ضعيف (Weak)
 * 
 *     مثال:
 *         دع ض = مرجع_ضعيف<عدد>.من_عم(س)
 */
template<typename T>
using مرجع_ضعيف = Weak<T>;

} // namespace Runtime
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // SAD_SEMANTIC_SMART_PTR_H
