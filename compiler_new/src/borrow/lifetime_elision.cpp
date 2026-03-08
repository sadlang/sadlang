/**
 * =============================================================================
 * ملف: lifetime_elision.cpp
 * الوصف: قواعد حذف العمر (Lifetime Elision)
 * المهمة: T204 - Implement lifetime elision rules
 * المرحلة: Phase 20 - User Story 17 (Lifetime Annotations)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هو حذف العمر (Elision)؟
 * ──────────────────────────
 * في معظم الحالات، يمكن للمترجم استنتاج الأعمار تلقائياً!
 * 
 * بدلاً من كتابة:
 *   دالة طول<'أ>(نص: &'أ نص) -> صحيح
 * 
 * يمكنك كتابة:
 *   دالة طول(نص: &نص) -> صحيح
 * 
 * المترجم يضيف الأعمار تلقائياً!
 * 
 * قواعد الحذف الثلاث:
 * ─────────────────────
 * 
 * 1. **قاعدة المدخلات**: كل مرجع مدخل يحصل على عمر خاص
 *    ```
 *    دالة foo(x: &نص, y: &نص)
 *    // يصبح:
 *    دالة foo<'أ, 'ب>(x: &'أ نص, y: &'ب نص)
 *    ```
 * 
 * 2. **قاعدة المخرج الواحد**: إذا كان هناك عمر مدخل واحد فقط،
 *    يُستخدم لجميع المراجع المُرجَعة
 *    ```
 *    دالة first(x: &نص) -> &نص
 *    // يصبح:
 *    دالة first<'أ>(x: &'أ نص) -> &'أ نص
 *    ```
 * 
 * 3. **قاعدة هذا**: في التوابع، عمر `هذا` يُستخدم للمخرجات
 *    ```
 *    تابع نص(&هذا) -> &نص
 *    // يصبح:
 *    تابع نص<'أ>(&'أ هذا) -> &'أ نص
 *    ```
 * 
 * متى يجب كتابة الأعمار صراحة؟
 * ─────────────────────────────
 * عندما لا تستطيع القواعد استنتاج العمر الصحيح:
 *   - دالة تأخذ مرجعين وترجع مرجعاً
 *   - بنية تحتوي مراجع متعددة
 *   - علاقات عمر معقدة
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <optional>
#include <sstream>
#include <iostream>

namespace sad::borrow::elision {

// =============================================================================
// هياكل البيانات
// =============================================================================

/**
 * نوع عمر
 */
enum class LifetimeKind {
    Named,      // 'أ - مسمى صريح
    Static,     // 'ثابت - ثابت
    Anonymous,  // '_ - مجهول
    Elided,     // محذوف (سيُستنتج)
    Generated,  // مُولَّد بواسطة الحذف
};

/**
 * معرّف عمر
 */
struct LifetimeId {
    LifetimeKind kind;
    std::string name;
    size_t index;  // للأعمار المُولَّدة
    
    static LifetimeId named(const std::string& n) {
        return {LifetimeKind::Named, n, 0};
    }
    
    static LifetimeId staticLt() {
        return {LifetimeKind::Static, "ثابت", 0};
    }
    
    static LifetimeId anonymous() {
        return {LifetimeKind::Anonymous, "_", 0};
    }
    
    static LifetimeId elided() {
        return {LifetimeKind::Elided, "", 0};
    }
    
    static LifetimeId generated(size_t idx) {
        LifetimeId lt;
        lt.kind = LifetimeKind::Generated;
        lt.index = idx;
        // توليد اسم عربي: '_أ, '_ب, '_ج, ...
        static const char* names[] = {"أ", "ب", "ج", "د", "هـ", "و", "ز"};
        lt.name = "'_" + std::string(names[idx % 7]);
        if (idx >= 7) lt.name += std::to_string(idx / 7);
        return lt;
    }
    
    std::string toString() const {
        switch (kind) {
            case LifetimeKind::Named: return "'" + name;
            case LifetimeKind::Static: return "'ثابت";
            case LifetimeKind::Anonymous: return "'_";
            case LifetimeKind::Elided: return "";
            case LifetimeKind::Generated: return name;
        }
        return "";
    }
    
    bool isElided() const { return kind == LifetimeKind::Elided; }
    bool isGenerated() const { return kind == LifetimeKind::Generated; }
    
    bool operator==(const LifetimeId& other) const {
        if (kind != other.kind) return false;
        if (kind == LifetimeKind::Generated) return index == other.index;
        return name == other.name;
    }
};

/**
 * نوع مرجعي (للتحليل)
 */
struct RefTypeInfo {
    std::optional<LifetimeId> lifetime;
    bool isMutable;
    std::string typeName;
    bool isSelf;  // هل هو &هذا؟
    
    bool hasLifetime() const {
        return lifetime.has_value() && !lifetime->isElided();
    }
};

/**
 * معامل دالة
 */
struct ParamInfo {
    std::string name;
    std::optional<RefTypeInfo> refType;  // إذا كان مرجعاً
    std::string plainType;  // إذا لم يكن مرجعاً
    
    bool isRef() const { return refType.has_value(); }
    bool isSelfRef() const { return isRef() && refType->isSelf; }
};

/**
 * توقيع دالة (قبل الحذف)
 */
struct FunctionSig {
    std::string name;
    std::vector<LifetimeId> explicitLifetimes;  // الأعمار المعلنة صراحة
    std::vector<ParamInfo> params;
    std::optional<RefTypeInfo> returnType;
    bool isMethod;  // هل هو تابع؟
};

/**
 * توقيع دالة (بعد الحذف)
 */
struct ElisionResult {
    std::vector<LifetimeId> lifetimes;  // جميع الأعمار (صريحة + مُولَّدة)
    std::vector<ParamInfo> params;      // المعاملات مع الأعمار المُولَّدة
    std::optional<RefTypeInfo> returnType;
    bool success;
    std::string error;
    
    static ElisionResult ok(
        std::vector<LifetimeId> lts,
        std::vector<ParamInfo> ps,
        std::optional<RefTypeInfo> ret
    ) {
        return {lts, ps, ret, true, ""};
    }
    
    static ElisionResult fail(const std::string& err) {
        return {{}, {}, std::nullopt, false, err};
    }
};

// =============================================================================
// محرك قواعد الحذف
// =============================================================================

/**
 * محرك قواعد حذف العمر
 * 
 * يطبق القواعد الثلاث لاستنتاج الأعمار
 */
class LifetimeElisionEngine {
public:
    /**
     * تطبيق قواعد الحذف على توقيع دالة
     */
    ElisionResult elide(const FunctionSig& sig) {
        inputLifetimeCount_ = 0;
        generatedLifetimes_.clear();
        
        // نسخ الأعمار الصريحة
        for (const auto& lt : sig.explicitLifetimes) {
            generatedLifetimes_.push_back(lt);
        }
        
        // القاعدة 1: كل مرجع مدخل يحصل على عمر
        std::vector<ParamInfo> newParams;
        LifetimeId selfLifetime;
        bool hasSelf = false;
        
        for (const auto& param : sig.params) {
            ParamInfo newParam = param;
            
            if (param.isRef()) {
                RefTypeInfo newRef = *param.refType;
                
                if (!newRef.hasLifetime()) {
                    // توليد عمر جديد
                    auto lt = LifetimeId::generated(inputLifetimeCount_++);
                    newRef.lifetime = lt;
                    generatedLifetimes_.push_back(lt);
                }
                
                // تتبع عمر &هذا
                if (param.isSelfRef()) {
                    selfLifetime = *newRef.lifetime;
                    hasSelf = true;
                }
                
                newParam.refType = newRef;
            }
            
            newParams.push_back(newParam);
        }
        
        // القاعدة 2 و 3: تحديد عمر المخرج
        std::optional<RefTypeInfo> newReturn;
        
        if (sig.returnType.has_value() && !sig.returnType->hasLifetime()) {
            RefTypeInfo newRef = *sig.returnType;
            
            // القاعدة 3: إذا كان هناك &هذا، استخدم عمره
            if (hasSelf && sig.isMethod) {
                newRef.lifetime = selfLifetime;
                newReturn = newRef;
            }
            // القاعدة 2: إذا كان هناك عمر مدخل واحد فقط
            else if (inputLifetimeCount_ == 1) {
                // البحث عن العمر الوحيد
                for (const auto& lt : generatedLifetimes_) {
                    if (lt.isGenerated() || 
                        (lt.kind == LifetimeKind::Named && 
                         std::find(sig.explicitLifetimes.begin(), 
                                   sig.explicitLifetimes.end(), lt) 
                         == sig.explicitLifetimes.end())) {
                        newRef.lifetime = lt;
                        break;
                    }
                }
                // إذا لم نجد، استخدم أول عمر مُولَّد
                if (!newRef.lifetime.has_value() && !generatedLifetimes_.empty()) {
                    for (const auto& lt : generatedLifetimes_) {
                        if (lt.isGenerated()) {
                            newRef.lifetime = lt;
                            break;
                        }
                    }
                }
                newReturn = newRef;
            }
            // لا يمكن استنتاج العمر
            else {
                return ElisionResult::fail(
                    "لا يمكن استنتاج عمر المخرج: يوجد أكثر من عمر مدخل واحد. "
                    "يرجى تحديد العمر صراحة."
                );
            }
        } else {
            newReturn = sig.returnType;
        }
        
        return ElisionResult::ok(generatedLifetimes_, newParams, newReturn);
    }
    
    /**
     * هل يحتاج هذا التوقيع إلى أعمار صريحة؟
     */
    bool needsExplicitLifetimes(const FunctionSig& sig) const {
        // عدّ المراجع في المدخلات
        int inputRefs = 0;
        bool hasSelf = false;
        
        for (const auto& param : sig.params) {
            if (param.isRef()) {
                inputRefs++;
                if (param.isSelfRef()) hasSelf = true;
            }
        }
        
        // إذا لم يكن هناك مرجع مُرجَع، لا حاجة
        if (!sig.returnType.has_value()) return false;
        
        // إذا كان هناك &هذا، القاعدة 3 تنطبق
        if (hasSelf && sig.isMethod) return false;
        
        // إذا كان هناك مرجع مدخل واحد فقط، القاعدة 2 تنطبق
        if (inputRefs == 1) return false;
        
        // نحتاج أعمار صريحة
        return true;
    }
    
private:
    size_t inputLifetimeCount_ = 0;
    std::vector<LifetimeId> generatedLifetimes_;
};

// =============================================================================
// مُزيّن الأعمار
// =============================================================================

/**
 * يُزيّن الكود بالأعمار المستنتجة (للتوثيق)
 */
class LifetimeAnnotator {
public:
    /**
     * توليد توقيع دالة كامل مع الأعمار
     */
    std::string annotate(const FunctionSig& original, const ElisionResult& elided) {
        if (!elided.success) return "/* خطأ: " + elided.error + " */";
        
        std::ostringstream ss;
        ss << "دالة " << original.name;
        
        // الأعمار
        if (!elided.lifetimes.empty()) {
            ss << "<";
            bool first = true;
            for (const auto& lt : elided.lifetimes) {
                if (lt.kind == LifetimeKind::Named || lt.isGenerated()) {
                    if (!first) ss << ", ";
                    ss << lt.toString();
                    first = false;
                }
            }
            ss << ">";
        }
        
        // المعاملات
        ss << "(";
        for (size_t i = 0; i < elided.params.size(); i++) {
            if (i > 0) ss << ", ";
            const auto& param = elided.params[i];
            ss << param.name << ": ";
            
            if (param.isRef()) {
                ss << "&";
                if (param.refType->lifetime.has_value()) {
                    ss << param.refType->lifetime->toString() << " ";
                }
                if (param.refType->isMutable) ss << "متغير ";
                ss << param.refType->typeName;
            } else {
                ss << param.plainType;
            }
        }
        ss << ")";
        
        // نوع الإرجاع
        if (elided.returnType.has_value()) {
            ss << " -> &";
            if (elided.returnType->lifetime.has_value()) {
                ss << elided.returnType->lifetime->toString() << " ";
            }
            if (elided.returnType->isMutable) ss << "متغير ";
            ss << elided.returnType->typeName;
        }
        
        return ss.str();
    }
};

// =============================================================================
// أمثلة على الحذف
// =============================================================================

/**
 * أمثلة توضيحية لقواعد الحذف
 */
class ElisionExamples {
public:
    void printExamples() {
        std::cout << "═══════════════════════════════════════════════\n";
        std::cout << "   أمثلة على قواعد حذف العمر\n";
        std::cout << "═══════════════════════════════════════════════\n\n";
        
        // مثال 1: القاعدة 2 (مدخل واحد)
        std::cout << "1. القاعدة الثانية - مدخل مرجعي واحد:\n";
        std::cout << "   الأصل: دالة طول(نص: &نص) -> صحيح\n";
        std::cout << "   مكتمل: دالة طول<'أ>(نص: &'أ نص) -> صحيح\n\n";
        
        // مثال 2: القاعدة 2 مع مخرج
        std::cout << "2. القاعدة الثانية - مدخل ومخرج:\n";
        std::cout << "   الأصل: دالة أول(قائمة: &قائمة) -> &عنصر\n";
        std::cout << "   مكتمل: دالة أول<'أ>(قائمة: &'أ قائمة) -> &'أ عنصر\n\n";
        
        // مثال 3: القاعدة 3 (تابع)
        std::cout << "3. القاعدة الثالثة - تابع مع &هذا:\n";
        std::cout << "   الأصل: تابع اسم(&هذا) -> &نص\n";
        std::cout << "   مكتمل: تابع اسم<'أ>(&'أ هذا) -> &'أ نص\n\n";
        
        // مثال 4: يحتاج تعليقات صريحة
        std::cout << "4. يحتاج أعماراً صريحة:\n";
        std::cout << "   خطأ: دالة أطول(أ: &نص, ب: &نص) -> &نص\n";
        std::cout << "   صحيح: دالة أطول<'أ>(أ: &'أ نص, ب: &'أ نص) -> &'أ نص\n\n";
        
        // مثال 5: أعمار متعددة
        std::cout << "5. أعمار متعددة مستقلة:\n";
        std::cout << "   الأصل: دالة foo(أ: &نص, ب: &نص)\n";
        std::cout << "   مكتمل: دالة foo<'أ, 'ب>(أ: &'أ نص, ب: &'ب نص)\n\n";
    }
};

} // namespace sad::borrow::elision

// =============================================================================
// واجهة C للتكامل
// =============================================================================

extern "C" {

using namespace sad::borrow::elision;

/**
 * إنشاء محرك الحذف
 */
void* sad_elision_engine_new() {
    return new LifetimeElisionEngine();
}

/**
 * تحرير المحرك
 */
void sad_elision_engine_free(void* engine) {
    delete static_cast<LifetimeElisionEngine*>(engine);
}

/**
 * هل يحتاج التوقيع أعماراً صريحة؟
 */
int sad_needs_explicit_lifetimes(void* engine, void* signature) {
    if (!engine || !signature) return -1;
    auto* e = static_cast<LifetimeElisionEngine*>(engine);
    auto* sig = static_cast<FunctionSig*>(signature);
    return e->needsExplicitLifetimes(*sig) ? 1 : 0;
}

/**
 * طباعة أمثلة الحذف
 */
void sad_print_elision_examples() {
    ElisionExamples examples;
    examples.printExamples();
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef LIFETIME_ELISION_TEST

#include <cassert>
#include <algorithm>

void testRule1InputLifetimes() {
    sad::borrow::elision::LifetimeElisionEngine engine;
    
    // دالة foo(x: &نص, y: &نص)
    sad::borrow::elision::FunctionSig sig;
    sig.name = "foo";
    sig.isMethod = false;
    sig.params = {
        {"x", sad::borrow::elision::RefTypeInfo{{}, false, "نص", false}, ""},
        {"y", sad::borrow::elision::RefTypeInfo{{}, false, "نص", false}, ""}
    };
    
    auto result = engine.elide(sig);
    
    assert(result.success);
    assert(result.lifetimes.size() == 2);  // عمران مُولَّدان
    
    std::cout << "✅ اختبار القاعدة الأولى (مدخلات) نجح!\n";
}

void testRule2SingleInput() {
    sad::borrow::elision::LifetimeElisionEngine engine;
    
    // دالة first(list: &قائمة) -> &عنصر
    sad::borrow::elision::FunctionSig sig;
    sig.name = "first";
    sig.isMethod = false;
    sig.params = {
        {"list", sad::borrow::elision::RefTypeInfo{{}, false, "قائمة", false}, ""}
    };
    sig.returnType = sad::borrow::elision::RefTypeInfo{{}, false, "عنصر", false};
    
    auto result = engine.elide(sig);
    
    assert(result.success);
    assert(result.returnType.has_value());
    assert(result.returnType->lifetime.has_value());
    
    // يجب أن يكون عمر المخرج = عمر المدخل
    assert(*result.params[0].refType->lifetime == *result.returnType->lifetime);
    
    std::cout << "✅ اختبار القاعدة الثانية (مدخل واحد) نجح!\n";
}

void testRule3MethodSelf() {
    sad::borrow::elision::LifetimeElisionEngine engine;
    
    // تابع name(&هذا) -> &نص
    sad::borrow::elision::FunctionSig sig;
    sig.name = "name";
    sig.isMethod = true;
    sig.params = {
        {"هذا", sad::borrow::elision::RefTypeInfo{{}, false, "Self", true}, ""}
    };
    sig.returnType = sad::borrow::elision::RefTypeInfo{{}, false, "نص", false};
    
    auto result = engine.elide(sig);
    
    assert(result.success);
    assert(result.returnType.has_value());
    
    // يجب أن يكون عمر المخرج = عمر &هذا
    assert(*result.params[0].refType->lifetime == *result.returnType->lifetime);
    
    std::cout << "✅ اختبار القاعدة الثالثة (تابع &هذا) نجح!\n";
}

void testNeedsExplicitLifetimes() {
    sad::borrow::elision::LifetimeElisionEngine engine;
    
    // دالة longest(a: &نص, b: &نص) -> &نص
    // يحتاج أعماراً صريحة!
    sad::borrow::elision::FunctionSig sig;
    sig.name = "longest";
    sig.isMethod = false;
    sig.params = {
        {"a", sad::borrow::elision::RefTypeInfo{{}, false, "نص", false}, ""},
        {"b", sad::borrow::elision::RefTypeInfo{{}, false, "نص", false}, ""}
    };
    sig.returnType = sad::borrow::elision::RefTypeInfo{{}, false, "نص", false};
    
    assert(engine.needsExplicitLifetimes(sig));
    
    auto result = engine.elide(sig);
    assert(!result.success);  // يجب أن يفشل
    
    std::cout << "✅ اختبار كشف الحاجة لأعمار صريحة نجح!\n";
}

void testAnnotator() {
    sad::borrow::elision::LifetimeElisionEngine engine;
    sad::borrow::elision::LifetimeAnnotator annotator;
    
    // دالة first(list: &قائمة) -> &عنصر
    sad::borrow::elision::FunctionSig sig;
    sig.name = "أول";
    sig.isMethod = false;
    sig.params = {
        {"قائمة", sad::borrow::elision::RefTypeInfo{{}, false, "قائمة", false}, ""}
    };
    sig.returnType = sad::borrow::elision::RefTypeInfo{{}, false, "عنصر", false};
    
    auto result = engine.elide(sig);
    std::string annotated = annotator.annotate(sig, result);
    
    // التحقق من أن التوقيع يحتوي على عمر
    assert(annotated.find("'_أ") != std::string::npos || 
           annotated.find("'أ") != std::string::npos);
    
    std::cout << "✅ اختبار المُزيّن نجح!\n";
    std::cout << "   التوقيع: " << annotated << "\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات قواعد حذف العمر\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testRule1InputLifetimes();
    testRule2SingleInput();
    testRule3MethodSelf();
    testNeedsExplicitLifetimes();
    testAnnotator();
    
    std::cout << "\n";
    
    // طباعة الأمثلة
    sad::borrow::elision::ElisionExamples examples;
    examples.printExamples();
    
    std::cout << "✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // LIFETIME_ELISION_TEST
