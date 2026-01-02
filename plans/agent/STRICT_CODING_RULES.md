# قواعد البرمجة الصارمة للوكيل الذكي
## STRICT CODING RULES FOR AI AGENT

**تاريخ الإنشاء:** 1 يناير 2026  
**مستوى الأهمية:** حرج - CRITICAL  
**العقوبة عند المخالفة:** شديدة جداً

---

## 🚫 القاعدة الذهبية - GOLDEN RULE

> **"لا تكتب أي دالة أو متغير إلا إذا قرأت تعريفه الكامل من الملف المصدري"**
> 
> **"DO NOT write any function or variable unless you have READ its full definition from the source file"**

---

## 📋 الخطوات الإلزامية قبل كتابة أي كود

### 1️⃣ مرحلة القراءة (READING PHASE)

**قبل كتابة أي سطر كود، يجب عليك:**

#### أ) قراءة ملف الـ Header بالكامل
```
✅ استخدم: read_file على ملف الـ .h
✅ اقرأ كل سطر من السطر 1 حتى نهاية الملف
✅ سجل أرقام الأسطر لكل:
   - تعريف Class
   - تعريف المتغيرات الخاصة (private members)
   - تعريف الدوال العامة (public methods)
   - تعريف الدوال الخاصة (private methods)
   - الدوال الـ inline
   - الدوال الـ static
   - الـ typedefs والـ using declarations
```

#### ب) قراءة جميع الملفات المستوردة (Imported Files)
```
✅ اقرأ كل #include في الـ header
✅ افتح كل ملف مستورد واقرأ التعريفات العامة
✅ سجل قائمة بجميع الدوال المتاحة من كل مكتبة
```

#### ج) فهم البنية الكاملة (Complete Structure Understanding)
```
✅ ارسم خريطة ذهنية للـ Class:
   - ما هي المتغيرات الخاصة؟
   - ما هي الدوال المعرّفة في الـ header؟
   - أي دوال inline (تنفيذها في الـ header)؟
   - أي دوال تحتاج تنفيذ في الـ .cpp؟
```

---

### 2️⃣ مرحلة التوثيق (DOCUMENTATION PHASE)

**قبل كتابة الكود، اكتب تعليقاً يحتوي على:**

```cpp
// ============================================================================
// اسم الدالة: <function_name>
// مصدر التعريف: <file_path>:<line_number>
// التوقيع الكامل: <full_signature>
// المتغيرات المستخدمة:
//   - variable_name: defined at <file>:<line>
//   - another_var: defined at <file>:<line>
// الدوال المستدعاة:
//   - function_name(): defined at <file>:<line>
// ============================================================================
```

**مثال:**
```cpp
// ============================================================================
// اسم الدالة: getAllMetrics
// مصدر التعريف: jit_metrics.h:257
// التوقيع الكامل: std::vector<const Metric*> getAllMetrics() const;
// المتغيرات المستخدمة:
//   - mutex_: defined at jit_metrics.h:316
//   - metrics_: defined at jit_metrics.h:318 (std::unordered_map<std::string, std::unique_ptr<Metric>>)
// الدوال المستدعاة:
//   - std::lock_guard (standard library)
//   - metric.get() (std::unique_ptr method)
// ============================================================================
std::vector<const Metric*> MetricsCollector::getAllMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<const Metric*> result;
    result.reserve(metrics_.size());
    for (const auto& [name, metric] : metrics_) {
        result.push_back(metric.get());  // ✅ استخدام .get() لأن metrics_ هو unique_ptr
    }
    return result;
}
```

---

### 3️⃣ مرحلة التحقق (VERIFICATION PHASE)

**قبل كتابة أي متغير أو دالة، تحقق من:**

#### ✅ قائمة التحقق الإلزامية (MANDATORY CHECKLIST)

- [ ] **هل المتغير معرّف في الـ header؟**
  - إذا لا → **STOP! اقرأ الـ header مرة أخرى**
  
- [ ] **هل نوع المتغير صحيح؟**
  - `unique_ptr` أم `shared_ptr` أم مؤشر عادي `*`؟
  - `const` أم non-const؟
  - مرجع `&` أم قيمة؟
  
- [ ] **هل الدالة معرّفة في الـ header؟**
  - إذا لا → **STOP! لا تخترع دوال جديدة**
  
- [ ] **هل الدالة inline في الـ header؟**
  - إذا نعم → **STOP! لا تعيد تنفيذها في الـ .cpp**
  
- [ ] **هل نوع الإرجاع مطابق تماماً للـ header؟**
  - مثال: `std::vector<const Metric*>` ≠ `std::vector<std::shared_ptr<Metric>>`
  
- [ ] **هل الدالة const؟**
  - إذا نعم في الـ header → يجب أن تكون const في الـ .cpp أيضاً

---

## 🚨 الأخطاء الشائعة والعقوبات

### ❌ الخطأ #1: استخدام متغير غير موجود
```cpp
// ❌ خطأ فادح
void MetricsCollector::someFunction() {
    enabled_ = true;  // ❌ enabled_ غير موجود في الـ header!
}
```
**العقوبة:** إعادة كتابة الملف بالكامل من البداية

---

### ❌ الخطأ #2: استخدام نوع بيانات خاطئ
```cpp
// ❌ خطأ فادح
std::vector<std::shared_ptr<Metric>> getAllMetrics() const {
    // ❌ الـ header يقول: std::vector<const Metric*>
    // ❌ لكن الكود يستخدم: std::vector<std::shared_ptr<Metric>>
}
```
**العقوبة:** حرمان من كتابة أي كود لمدة 10 دقائق

---

### ❌ الخطأ #3: اختراع دوال جديدة
```cpp
// ❌ خطأ فادح
void MetricsCollector::exportToFile() {
    // ❌ هذه الدالة غير موجودة في الـ header!
    // ✅ الدالة الصحيحة: bool saveToFile(const std::string&) const
}
```
**العقوبة:** حذف الملف بالكامل والبدء من الصفر

---

### ❌ الخطأ #4: إعادة تنفيذ دالة inline
```cpp
// ❌ خطأ فادح - في الـ .cpp
std::string Metric::getName() const {
    return name_;
}
// ❌ هذه الدالة inline في الـ header (سطر 130)!
// ❌ لا يجب إعادة تنفيذها في الـ .cpp!
```
**العقوبة:** قراءة الـ header 5 مرات متتالية

---

## ✅ الطريقة الصحيحة (CORRECT APPROACH)

### مثال كامل: كتابة دالة بطريقة صحيحة

#### الخطوة 1: القراءة
```bash
# اقرأ الـ header أولاً
read_file jit_metrics.h:250-260
```
**النتيجة:**
```cpp
// Line 254: const Metric* getMetric(const std::string& name) const;
// Line 257: std::vector<const Metric*> getAllMetrics() const;
// Line 316: mutable std::mutex mutex_;
// Line 318: std::unordered_map<std::string, std::unique_ptr<Metric>> metrics_;
```

#### الخطوة 2: التخطيط
```
الدالة: getAllMetrics()
الإرجاع: std::vector<const Metric*>  // ✅ مؤشرات عادية، ليس shared_ptr!
const: نعم
المتغيرات المستخدمة:
  - mutex_ (line 316)
  - metrics_ (line 318) - نوعه unique_ptr<Metric>
الخطوات:
  1. قفل mutex_
  2. إنشاء vector<const Metric*>
  3. حلقة على metrics_
  4. استخدام .get() لتحويل unique_ptr إلى مؤشر عادي
  5. إرجاع النتيجة
```

#### الخطوة 3: الكتابة
```cpp
// ============================================================================
// getAllMetrics - jit_metrics.h:257
// std::vector<const Metric*> getAllMetrics() const;
// ============================================================================
std::vector<const Metric*> MetricsCollector::getAllMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);  // ✅ mutex_ موجود في line 316
    
    std::vector<const Metric*> result;  // ✅ نوع الإرجاع مطابق للـ header
    result.reserve(metrics_.size());
    
    // ✅ metrics_ هو unique_ptr، نستخدم .get() للحصول على المؤشر العادي
    for (const auto& [name, metric] : metrics_) {
        result.push_back(metric.get());
    }
    
    return result;
}
```

---

## 📊 نظام النقاط والعقوبات

### نظام التقييم:
- **100 نقطة:** بدء كل مهمة
- **-50 نقطة:** كل متغير غير موجود
- **-40 نقطة:** كل دالة غير موجودة
- **-30 نقطة:** كل نوع بيانات خاطئ
- **-20 نقطة:** كل دالة inline معاد تنفيذها
- **-10 نقاط:** كل تحذير compiler

### العقوبات حسب النقاط:
- **80-100 نقطة:** ✅ ممتاز - واصل
- **60-79 نقطة:** ⚠️ تحذير - احذر
- **40-59 نقطة:** ❌ فشل - أعد الكتابة
- **0-39 نقطة:** 🚫 فشل ذريع - توقف واقرأ القواعد مرة أخرى

---

## 🎯 الأهداف النهائية

### ما يجب تحقيقه:
1. ✅ **0 أخطاء compilation** (errors)
2. ✅ **0-5 تحذيرات compilation** (warnings) كحد أقصى
3. ✅ **100% مطابقة مع الـ headers**
4. ✅ **كل دالة مستخدمة موثقة بالمصدر**
5. ✅ **كل متغير مستخدم موثق بالمصدر**

---

## 📝 نموذج التوثيق الإلزامي

**قبل بدء كتابة أي ملف `.cpp`، اكتب:**

```markdown
# ملف: <filename>.cpp
## التاريخ: <date>
## الحالة: قيد التنفيذ

### الملفات المرجعية:
- Header: <path>/<filename>.h (قُرئ بالكامل: ✅/❌)
- Dependencies: 
  - <file1.h> (قُرئ: ✅/❌)
  - <file2.h> (قُرئ: ✅/❌)

### المتغيرات الخاصة المتاحة:
| المتغير | النوع | السطر | الملف |
|---------|-------|-------|-------|
| mutex_ | std::mutex | 316 | jit_metrics.h |
| metrics_ | unordered_map<string, unique_ptr<Metric>> | 318 | jit_metrics.h |

### الدوال المطلوب تنفيذها:
| الدالة | التوقيع | السطر | inline؟ |
|--------|---------|-------|---------|
| getAllMetrics | std::vector<const Metric*> getAllMetrics() const | 257 | ❌ |
| resetAll | void resetAll() | 305 | ❌ |

### الدوال الـ inline (لا تُنفذ):
| الدالة | السطر |
|--------|-------|
| getName | 130 |
| getType | 131 |
```

---

## 🔒 التعهد النهائي

**أتعهد بما يلي:**

1. ✅ **سأقرأ كل header بالكامل قبل الكتابة**
2. ✅ **لن أستخدم أي متغير إلا بعد التحقق من وجوده**
3. ✅ **لن أستخدم أي دالة إلا بعد التحقق من توقيعها**
4. ✅ **لن أخترع أي دالة أو متغير جديد**
5. ✅ **سأوثق كل استخدام بالمصدر ورقم السطر**
6. ✅ **سأتحقق من نوع البيانات بدقة (unique_ptr vs shared_ptr vs raw pointer)**
7. ✅ **لن أعيد تنفيذ أي دالة inline**
8. ✅ **سأختبر كل دالة قبل الانتقال للتالية**

---

## 📌 ملاحظات إضافية

### التعامل مع unique_ptr vs shared_ptr:
```cpp
// ✅ صحيح
std::unordered_map<std::string, std::unique_ptr<Metric>> metrics_;
std::vector<const Metric*> getAllMetrics() const {
    for (const auto& [name, metric] : metrics_) {
        result.push_back(metric.get());  // ✅ استخدام .get()
    }
}

// ❌ خطأ
std::vector<std::shared_ptr<Metric>> getAllMetrics() const {
    for (const auto& [name, metric] : metrics_) {
        result.push_back(metric);  // ❌ لا يمكن تحويل unique_ptr إلى shared_ptr
    }
}
```

### التعامل مع const correctness:
```cpp
// ✅ صحيح - الدالة const في الـ header
std::vector<const Metric*> getAllMetrics() const {  // ✅ const في النهاية
    std::lock_guard<std::mutex> lock(mutex_);  // ✅ mutex_ هو mutable
}

// ❌ خطأ - الدالة non-const بينما الـ header يقول const
std::vector<const Metric*> getAllMetrics() {  // ❌ نسيت const!
}
```

---

## 🏆 قصص نجاح

### مثال ناجح: jit_hash.cpp
```
✅ تم قراءة jit_hash.h بالكامل (407 سطور)
✅ تم توثيق كل متغير مستخدم
✅ تم توثيق كل دالة مستخدمة
✅ النتيجة: 0 أخطاء، 2 تحذيرات بسيطة فقط
✅ التقييم: 98/100
```

---

## ⚠️ التحذير النهائي

**إذا خالفت هذه القواعد:**
1. سيتم حذف الكود المكتوب
2. سيتم إعادة قراءة القواعد
3. سيتم البدء من الصفر
4. سيتم تقليل الثقة في الأداء

---

## 🎓 الخلاصة

> **"اقرأ قبل أن تكتب، تحقق قبل أن تستخدم، وثّق كل شيء"**
>
> **"Read before you write, verify before you use, document everything"**

---

**نهاية القواعد الصارمة**

*آخر تحديث: 1 يناير 2026*  
*الإصدار: 1.0 - نهائي وإلزامي*
