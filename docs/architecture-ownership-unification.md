# خطة توحيد نظام الملكية في الطبقة المشتركة

> **الحالة:** مقترح للمراجعة (لم يُنفَّذ بعد)
> **المؤلف:** فريق إعادة الهيكلة المعمارية
> **التاريخ:** 2026
> **المراجع المعمارية:** CW-01, CW-02, CW-03, CW-19, CW-20, BF-09, BF-10

---

## 1. ملخص تنفيذي

نظام فحص الملكية والاستعارة (Ownership & Borrow Checking) في لغة ص **مكرَّر حالياً** في طبقتين منفصلتين:

| الطبقة | المسار | namespace | المسؤولية |
|---|---|---|---|
| المترجم | `compiler/src/semantic/` (10 ملفات .cpp + 5 .h) | `Sad::Semantic::*` | فحص ساكن قبل توليد LLVM IR |
| المفسّر | `interpreter/src/managers/ownership_manager.{h,cpp}` | `Sad::Data::*` | تتبع ديناميكي أثناء التنفيذ |

هذا التكرار يُنتج **أربع مشاكل جوهرية**:

1. **عدم اتساق النتائج بين المسارات**: نفس البرنامج قد يجتاز فحص `sad` (المفسّر) ويفشل في `sadc` (المترجم) أو العكس.
2. **مضاعفة كلفة الصيانة**: أي تعديل في قواعد الملكية يجب تطبيقه في مكانين بالتوازي.
3. **استحالة بناء أداة فحص مستقلة** (`sad-check`): المنطق مدفون داخل طبقات التنفيذ.
4. **مخالفة CW-19 (DRY) و CW-03 (Separation of Concerns)**: قواعد لغة موحدة موزعة على طبقتي تنفيذ.

**الحل المقترح:** نقل كل منطق الملكية إلى طبقة `shared/ownership/` جديدة، وجعل المفسّر والمترجم وVM وWASM **مستهلكين** لها فقط.

---

## 2. الدليل المادي على التكرار

### 2.1 تعدادات متطابقة معرّفة مرتين

| التعداد | في `compiler/include/semantic/ownership_tracker.h` | في `interpreter/include/managers/ownership_manager.h` |
|---|---|---|
| `OwnershipState` | `Owned, Moved, Borrowed, BorrowedMut, Dropped, Uninitialized` | **نفس القيم تماماً** |
| `BorrowKind` | `Shared, Mutable` | **نفس القيم تماماً** |

### 2.2 خرائط الاستخدام

- **المترجم**: 10 ملفات `.cpp` + 5 رؤوس + 4 مستوردين خارجيين (`tools/compiler/compiler_driver*`, اختبار `compiler/tests/test_borrow_checker.cpp`)
- **المفسّر**: 18 ملفاً يستورد `ownership_manager.h` (جميع `expression_evaluator_*.cpp` + `statement_executor.h` + `expression_evaluator.h` + `interpreter_core.h`)
- **VM**: يستهلك مباشرة عبر `sad_interpreter`
- **WASM**: غير معروف (يجب التحقق)

### 2.3 منطق متوازٍ

- المترجم: `Sad::Semantic::BorrowChecker::checkBorrow(...)` → خطأ ساكن
- المفسّر: `Sad::Data::OwnershipManager::createBorrow(...)` في `expression_evaluator_core.cpp:387` → خطأ وقت تشغيل

كلاهما يُطبّق نفس القواعد المنطقية بصياغتين مختلفتين.

---

## 3. التصميم المقترح

### 3.1 الهيكل الفيزيائي الجديد

```
shared/ownership/
├── CMakeLists.txt                          # ينشئ مكتبة sad_ownership
├── include/ownership/
│   ├── ownership_state.h                   # OwnershipState, BorrowKind, OwnershipErrorKind
│   ├── source_location.h                   # SourceLocation (مشترك)
│   ├── borrow_info.h                       # BorrowInfo, VariableOwnership
│   ├── ownership_tracker.h                 # OwnershipTracker (المحرك الأساسي)
│   ├── borrow_checker.h                    # BorrowChecker (الزائر AST)
│   ├── lifetime_analyzer.h                 # LifetimeAnalyzer
│   ├── move_analyzer.h                     # MoveAnalyzer
│   ├── unsafe_checker.h                    # UnsafeChecker
│   ├── pattern_ownership.h                 # PatternOwnership
│   ├── exhaustiveness.h                    # ExhaustivenessChecker
│   ├── this_binding.h                      # ThisBinding
│   ├── cascade_prevention_compile.h        # نسخة المترجم (مع إعادة تسمية)
│   └── ownership_diagnostic.h              # نوع موحَّد لرسائل الأخطاء
└── src/
    ├── ownership_tracker.cpp
    ├── borrow_checker.cpp
    ├── borrow_checker_visitors.cpp
    ├── lifetime_analyzer.cpp
    ├── move_analyzer.cpp
    ├── unsafe_checker.cpp
    ├── pattern_ownership.cpp
    ├── exhaustiveness.cpp
    ├── this_binding.cpp
    └── cascade_prevention_compile.cpp
```

### 3.2 التصنيف المنطقي (Namespaces)

كل المحتوى تحت `Sad::Ownership::*`:

- `Sad::Ownership::OwnershipState`
- `Sad::Ownership::BorrowKind`
- `Sad::Ownership::OwnershipTracker` — المحرك المركزي (لا يتغير سلوكه)
- `Sad::Ownership::BorrowChecker` — يأخذ `ASTNode*` ويُرجع `std::vector<OwnershipDiagnostic>`
- `Sad::Ownership::OwnershipDiagnostic { kind, message_ar, message_en, location }` — نوع موحَّد للأخطاء

### 3.3 الواجهة العامة الموحَّدة

```cpp
namespace Sad { namespace Ownership {

// نوع موحَّد للتشخيص (يستخدمه المترجم والمفسّر بنفس الشكل)
struct OwnershipDiagnostic {
    OwnershipErrorKind kind;
    std::string variableName;
    std::string messageAr;
    std::string messageEn;
    SourceLocation location;
    bool isError;  // false = تحذير
};

// نقطة الدخول الموحَّدة للفحص الساكن
class BorrowChecker : public ASTVisitor {
public:
    explicit BorrowChecker(OwnershipTracker& tracker);
    
    // فحص شامل لشجرة AST
    std::vector<OwnershipDiagnostic> check(ASTNode* root);
    
    // فحص تدريجي (للمفسّر — قبل تنفيذ كل عبارة)
    std::vector<OwnershipDiagnostic> checkStatement(StmtNode* stmt);
    std::vector<OwnershipDiagnostic> checkExpression(ExprNode* expr);
};

}}
```

### 3.4 تكامل المسارات مع الطبقة الموحَّدة

#### المترجم (`sadc`)
```cpp
// في compiler/src/sadc_main.cpp أو compiler_driver.cpp
Sad::Ownership::OwnershipTracker tracker;
Sad::Ownership::BorrowChecker checker(tracker);
auto diagnostics = checker.check(astRoot);
if (!diagnostics.empty()) {
    reportAndExit(diagnostics);
}
// ثم: SIRBuilder → LLVMCodeGen
```

#### المفسّر (`sad`) — تحويل إلى محول رفيع
```cpp
// في interpreter/src/managers/ownership_manager.cpp
namespace Sad { namespace Data {

class OwnershipManager {
    Sad::Ownership::OwnershipTracker tracker_;
    Sad::Ownership::BorrowChecker checker_;
public:
    // قبل تنفيذ كل عبارة، نُجري الفحص باستخدام نفس المنطق
    void beforeStatement(StmtNode* stmt) {
        auto diags = checker_.checkStatement(stmt);
        for (auto& d : diags) {
            if (d.isError) throw RuntimeError(d.messageAr);
        }
    }
};

}}
```

نفس القواعد، نفس الرسائل، نفس النتائج عبر كل المسارات.

#### VM
يستهلك تلقائياً عبر `sad_interpreter` (لا تغيير).

#### WASM
يستهلك `sad_ownership` مباشرة (تحقق لاحقاً من نقطة الدخول).

### 3.5 الأداة المستقلة `sad-check` (مستقبل)

```
tools/sad-check/
├── CMakeLists.txt
└── main.cpp    # Lexer → Parser → BorrowChecker → تقرير
```

لا يحتاج LLVM، لا يحتاج VM، لا يحتاج runtime. ثنائي صغير سريع.

---

## 4. مخطط التبعيات قبل وبعد

### قبل (التكرار الحالي)

```
sad_compiler ──▶ compiler/src/semantic/* (Sad::Semantic)
                                          │
                                          ├── OwnershipState (نسخة 1)
                                          └── BorrowKind     (نسخة 1)

sad_interpreter ──▶ interpreter/src/managers/ownership_manager.* (Sad::Data)
                                                                  │
                                                                  ├── OwnershipState (نسخة 2)
                                                                  └── BorrowKind     (نسخة 2)
```

### بعد (التوحيد)

```
                                  ┌─▶ sad_compiler
sad_ownership (Sad::Ownership) ──┼─▶ sad_interpreter (محول رفيع)
                                  ├─▶ sad_vm
                                  ├─▶ sad_wasm
                                  └─▶ sad-check (أداة مستقلة)
```

---

## 5. خطة التنفيذ (مراحل قابلة للتراجع)

### المرحلة أ: إنشاء `shared/ownership/` (قابلة للتراجع)
1. إنشاء بنية المجلد + `CMakeLists.txt` فارغ.
2. نسخ (وليس نقل) الملفات من `compiler/{src,include}/semantic/` إلى `shared/ownership/`.
3. تحديث جميع `#include` الداخلية لتستخدم `ownership/` بدلاً من `semantic/`.
4. توحيد namespace إلى `Sad::Ownership` عبر بحث/استبدال.
5. **بناء فقط** `sad_ownership` للتأكد من النظافة (لا تربط بأي مسار بعد).

✅ **معيار النجاح:** المكتبة تُبنى بدون أخطاء.
🔄 **التراجع:** حذف مجلد `shared/ownership/` (لم يُلمس أي شيء آخر).

### المرحلة ب: ربط المترجم بالمكتبة الجديدة
1. حذف `compiler/{src,include}/semantic/` (بعد التأكد من النسخ).
2. تحديث `cmake/sources.cmake` (إزالة `compiler/src/semantic/*` من `COMPILER_SOURCES`).
3. تحديث `cmake/libraries.cmake` (`target_link_libraries(sad_compiler PUBLIC sad_ownership)`).
4. تحديث 4 مستوردين: `compiler_driver.h`, `compiler_driver_analysis.cpp`, `test_borrow_checker.cpp`, واستيراد داخلي في `pattern_ownership.cpp`.
5. تحديث namespace في كل مكان: `Sad::Semantic::` → `Sad::Ownership::`.
6. **بناء `sadc` + اختبار `test_borrow_checker`**.

✅ **معيار النجاح:** `sadc.exe` يُبنى ويعمل، اختبار `test_borrow_checker` يمر.
🔄 **التراجع:** استرجاع مجلد `compiler/src/semantic/` من git.

### المرحلة د: تحويل المفسّر إلى محول رفيع
1. إعادة كتابة `interpreter/src/managers/ownership_manager.cpp` ليكون wrapper فوق `Sad::Ownership::OwnershipTracker`.
2. الحفاظ على واجهة `Sad::Data::OwnershipManager` (لا تكسر 18 مستهلكاً).
3. حذف التعدادات المكررة (`OwnershipState`, `BorrowKind` في `Sad::Data`) واستبدالها بـ `using Sad::Ownership::OwnershipState;`.
4. ربط `sad_interpreter` بـ `sad_ownership` في `cmake/libraries.cmake`.
5. **بناء `sad.exe` + اختبارات شاملة**.

✅ **معيار النجاح:**
- `sad.exe` يُبنى ويعمل.
- نفس برنامج `.ص` بانتهاك ملكية يفشل **بنفس الرسالة** في `sad` و `sadc`.
- `ctest` يمر بدون تراجع.

🔄 **التراجع:** استرجاع `ownership_manager.cpp` و `cmake/libraries.cmake` من git.

### المرحلة هـ: التحقق الشامل
1. اختبار تحقق متماثل: ملف `.ص` واحد يُنتج نفس الأخطاء عبر `sad` و `sad --vm` و `sadc`.
2. تشغيل `ctest --test-dir build` كاملاً.
3. تحديث ذاكرة المستودع `/memories/repo/architecture_refactor_status.md`.
4. تحديث `docs/architecture-summary.md` بحالة الدين الجديد.

### المرحلة ج (مؤجلة): أداة `sad-check`
بعد استقرار المراحل أ-هـ، نضيف الأداة المستقلة. لا تعتمد عليها بقية الخطة.

---

## 6. تقييم المخاطر

| المخاطرة | الاحتمال | التأثير | التخفيف |
|---|---|---|---|
| كسر 18 مستهلكاً للمفسّر | متوسط | عالي | الحفاظ على واجهة `Sad::Data::OwnershipManager` كـ wrapper |
| اختلاف خفي بين منطق Tracker القديم في `Sad::Data` ومنطق `Sad::Semantic` | عالي | متوسط | كتابة اختبارات مزدوجة (نفس البرنامج → نفس النتيجة) قبل الحذف |
| كسر اختبار `test_borrow_checker` | منخفض | منخفض | تشغيل الاختبار بعد كل مرحلة |
| تأثير على وقت البناء | منخفض | منخفض | المكتبة الجديدة صغيرة (~10 ملفات) |
| كسر LLVM Debug build (مشكلة قائمة) | غير ذي صلة | — | غير متأثر بهذا التغيير |

---

## 7. معايير القبول

- [ ] `shared/ownership/` موجود ويُبنى بنجاح.
- [ ] `compiler/src/semantic/` و `compiler/include/semantic/` محذوفان.
- [ ] `Sad::Data::OwnershipManager` تحوّل إلى wrapper رفيع فوق `Sad::Ownership::OwnershipTracker`.
- [ ] لا يوجد تعريف مكرر لـ `OwnershipState` أو `BorrowKind` في المشروع.
- [ ] `sad.exe` و `sadc.exe` يُبنيان بنجاح في وضع Release.
- [ ] برنامج اختبار: `let x = 5; let y = move x; print(x);` يُنتج نفس رسالة الخطأ العربية في `sad` و `sadc`.
- [ ] `ctest` يمر بدون تراجع عن قاعدة الاختبارات الحالية.
- [ ] `/memories/repo/architecture_refactor_status.md` محدَّث بإنجاز هذه المرحلة.

---

## 8. ما هذه الوثيقة لا تغطّيه (نطاق مستقبلي)

- بناء أداة `sad-check` المستقلة (المرحلة ج — مؤجلة).
- توحيد `Sad::Errors::CascadePrevention` (مختلف معمارياً — لا علاقة بنظام الملكية).
- توحيد `type_checker` (موجود مسبقاً في `shared/semantic/` — مستقل).
- إضافة قواعد ملكية جديدة (هذه إعادة هيكلة فقط، لا توسعة منطقية).

---

## 9. مرجع سريع للملفات المتأثرة

### ملفات تُنقل (15 ملف)
```
compiler/src/semantic/borrow_checker.cpp           → shared/ownership/src/
compiler/src/semantic/borrow_checker_visitors.cpp  → shared/ownership/src/
compiler/src/semantic/cascade_prevention.cpp       → shared/ownership/src/cascade_prevention_compile.cpp
compiler/src/semantic/exhaustiveness.cpp           → shared/ownership/src/
compiler/src/semantic/lifetime_analyzer.cpp        → shared/ownership/src/
compiler/src/semantic/move_analyzer.cpp            → shared/ownership/src/
compiler/src/semantic/ownership_tracker.cpp        → shared/ownership/src/
compiler/src/semantic/pattern_ownership.cpp        → shared/ownership/src/
compiler/src/semantic/this_binding.cpp             → shared/ownership/src/
compiler/src/semantic/unsafe_checker.cpp           → shared/ownership/src/
compiler/include/semantic/borrow_checker.h         → shared/ownership/include/ownership/
compiler/include/semantic/lifetime_analyzer.h      → shared/ownership/include/ownership/
compiler/include/semantic/move_analyzer.h          → shared/ownership/include/ownership/
compiler/include/semantic/ownership_tracker.h     → shared/ownership/include/ownership/
compiler/include/semantic/unsafe_checker.h         → shared/ownership/include/ownership/
```

### ملفات تُعدَّل (لا تُنقل)
```
interpreter/include/managers/ownership_manager.h   (تحويل إلى wrapper)
interpreter/src/managers/ownership_manager.cpp     (تحويل إلى wrapper)
cmake/sources.cmake                                 (إزالة compiler/src/semantic/*)
cmake/libraries.cmake                               (إضافة sad_ownership + روابط)
shared/CMakeLists.txt                               (إضافة add_subdirectory(ownership))
tools/compiler/compiler_driver.h                    (تحديث #include + namespace)
tools/compiler/compiler_driver_analysis.cpp        (تحديث #include + namespace)
compiler/tests/test_borrow_checker.cpp              (تحديث #include + namespace)
```

### ملفات جديدة (4 ملفات)
```
shared/ownership/CMakeLists.txt
shared/ownership/include/ownership/ownership_state.h    (تعدادات موحَّدة)
shared/ownership/include/ownership/ownership_diagnostic.h
shared/ownership/include/ownership/source_location.h
```

---

**نهاية الوثيقة. ينتظر مراجعة المستخدم قبل التنفيذ.**
