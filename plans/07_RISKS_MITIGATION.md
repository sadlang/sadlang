# ⚠️ المخاطر وطرق التخفيف

**تاريخ الإنشاء:** 4 نوفمبر 2025  
**آخر تحديث:** 4 نوفمبر 2025

---

## 📋 جدول المحتويات

1. [مخاطر تقنية](#مخاطر-تقنية)
2. [مخاطر الجدولة](#مخاطر-الجدولة)
3. [مخاطر الجودة](#مخاطر-الجودة)
4. [مخاطر الموارد](#مخاطر-الموارد)
5. [مخاطر التصميم](#مخاطر-التصميم)

---

## 🔧 مخاطر تقنية

### 1. دعم UTF-8 والحروف العربية

**الخطر:** 🔴 عالي  
**الوصف:** صعوبة في معالجة النصوص العربية (UTF-8 multi-byte) في Lexer/Parser.

**التأثير:**
- فشل tokenization للمعرفات العربية
- مشاكل في حساب المواقع (line/column)
- أخطاء في معالجة النصوص

**التخفيف:**
```cpp
// ✅ استخدام مكتبات UTF-8 مثبتة
#include <utf8.h>  // utf8cpp library

bool isArabicLetter(uint32_t codepoint) {
    return (codepoint >= 0x0600 && codepoint <= 0x06FF) ||  // Arabic
           (codepoint >= 0x0750 && codepoint <= 0x077F);    // Arabic Supplement
}

// معالجة آمنة
std::string source = "رقم س = 10";
auto it = source.begin();
while (it != source.end()) {
    uint32_t cp = utf8::next(it, source.end());
    if (isArabicLetter(cp)) {
        // معالجة الحرف العربي
    }
}
```

**خطة احتياطية:**
1. استخدام مكتبة `utf8cpp` القياسية
2. اختبارات شاملة مع نصوص عربية
3. دعم فني من مطورين عرب

**الحالة:** ✅ تم التخفيف (في Lexer)

---

### 2. أولوية العوامل (Operator Precedence)

**الخطر:** 🟠 متوسط  
**الوصف:** أخطاء في ترتيب تنفيذ العمليات الحسابية.

**مثال المشكلة:**
```s
# يجب أن يُفسر كـ: 2 + (3 * 4) = 14
# وليس: (2 + 3) * 4 = 20
رقم نتيجة = 2 + 3 * 4
```

**التخفيف:**
```cpp
// ✅ Precedence Climbing Algorithm
std::unique_ptr<Expression> ParserCore::parseExpression(int min_precedence) {
    auto left = parsePrimary();
    
    while (!isAtEnd() && getPrecedence(peek()) >= min_precedence) {
        Token op = advance();
        int precedence = getPrecedence(op);
        auto right = parseExpression(precedence + 1);
        left = std::make_unique<BinaryOpExpr>(std::move(left), op, std::move(right));
    }
    
    return left;
}

int getPrecedence(Token op) {
    static std::unordered_map<TokenType, int> precedence = {
        {TokenType::OR, 1},
        {TokenType::AND, 2},
        {TokenType::EQUAL_EQUAL, 3},
        {TokenType::LESS, 4},
        {TokenType::PLUS, 5},
        {TokenType::MINUS, 5},
        {TokenType::STAR, 6},
        {TokenType::SLASH, 6}
    };
    return precedence.count(op.getType()) ? precedence[op.getType()] : 0;
}
```

**الاختبارات:**
```cpp
TEST(ParserTest, OperatorPrecedence) {
    // 2 + 3 * 4 = 14
    EXPECT_EQ(parse("2 + 3 * 4")->evaluate(), 14);
    
    // (2 + 3) * 4 = 20
    EXPECT_EQ(parse("(2 + 3) * 4")->evaluate(), 20);
}
```

**الحالة:** ⏳ قادم (في Parser)

---

### 3. إدارة الذاكرة (Memory Management)

**الخطر:** 🟠 متوسط  
**الوصف:** Memory leaks أو dangling pointers في AST/Interpreter.

**التأثير:**
- استهلاك ذاكرة متزايد
- Crashes عشوائية
- مشاكل أداء

**التخفيف:**
```cpp
// ✅ استخدام Smart Pointers حصرياً
class ASTNode {
public:
    virtual ~ASTNode() = default;  // virtual destructor
};

class ParserCore {
private:
    // ✅ unique_ptr - ملكية حصرية
    std::unique_ptr<ASTNode> parseExpression();
    
    // ✅ shared_ptr - ملكية مشتركة (نادر)
    std::shared_ptr<VariableManager> var_mgr_;
};

// ❌ تجنب raw pointers
// ASTNode* node = new ASTNode();  // NO!

// ✅ استخدام move semantics
auto expr = parseExpression();
auto stmt = std::make_unique<ReturnStmt>(std::move(expr));
```

**أدوات الفحص:**
- Valgrind (Linux/WSL)
- AddressSanitizer (clang/gcc)
- Visual Studio Memory Profiler

**الحالة:** ⚠️ يحتاج مراقبة مستمرة

---

### 4. تعقيد الـ AST (AST Complexity)

**الخطر:** 🟡 منخفض  
**الوصف:** شجرة AST معقدة جداً تسبب صعوبة صيانة.

**التخفيف:**
1. **استخدام Visitor Pattern:**
```cpp
class ASTVisitor {
public:
    virtual void visit(LiteralExpr* expr) = 0;
    virtual void visit(BinaryOpExpr* expr) = 0;
    // ...
};

class EvalVisitor : public ASTVisitor {
    void visit(BinaryOpExpr* expr) override {
        auto left = evaluate(expr->getLeft());
        auto right = evaluate(expr->getRight());
        return applyOperator(expr->getOp(), left, right);
    }
};
```

2. **تبسيط العقد:**
   - عقد صغيرة ومركزة
   - تجنب inheritance عميق
   - استخدام composition عند الحاجة

**الحالة:** ✅ مُخطط له

---

### 5. أداء المفسر (Interpreter Performance)

**الخطر:** 🟡 منخفض  
**الوصف:** بطء التنفيذ في البرامج الكبيرة.

**التأثير:**
- تجربة مستخدم سيئة
- غير قابل للاستخدام في Production

**التخفيف:**

#### قصير المدى (Phase 4-5):
```cpp
// ✅ Cache للقيم المحسوبة
class Interpreter {
private:
    std::unordered_map<std::string, Value> constant_cache_;
    
    Value evaluate(Expression* expr) {
        if (auto* literal = dynamic_cast<LiteralExpr*>(expr)) {
            return literal->getValue();  // no computation
        }
        // ...
    }
};
```

#### طويل المدى (Phase 8):
1. **Bytecode Compiler:**
```
AST → Bytecode → VM execution
(10-100x faster than tree-walking interpreter)
```

2. **JIT Compilation** (مستقبلي):
```
Bytecode → Machine Code (LLVM)
```

**الحالة:** ⏳ مؤجل للمرحلة 8

---

## 📅 مخاطر الجدولة

### 1. تقدير الوقت غير دقيق

**الخطر:** 🟠 متوسط  
**الوصف:** المهام تأخذ وقت أطول من المتوقع.

**الأسباب:**
- تعقيد غير متوقع
- bugs صعبة التتبع
- تبعيات مخفية

**التخفيف:**
1. **Buffer Time:**
   - أضف 20-30% للتقديرات
   - خطط للأسوأ

2. **مراجعة دورية:**
   ```
   كل أسبوع: مراجعة التقدم
   - ما تم؟
   - ما المتبقي؟
   - هل نحتاج تعديل الخطة؟
   ```

3. **التقسيم:**
   - مهام صغيرة (< 1 يوم)
   - نقاط تسليم واضحة

**الحالة:** ⚠️ يحتاج مراقبة

---

### 2. Scope Creep (توسع النطاق)

**الخطر:** 🟠 متوسط  
**الوصف:** إضافة ميزات غير مخططة تأخر المشروع.

**مثال:**
```
خطة: Parser أساسي
واقع: Parser + تحسينات + ميزات إضافية
نتيجة: 2 أسبوع → 4 أسابيع
```

**التخفيف:**
1. **تجميد الميزات:**
   - لكل مرحلة: نطاق محدد
   - أي ميزة جديدة → backlog للمرحلة التالية

2. **MVP أولاً:**
   ```
   Phase 2: Parser basics
   - ✅ parseExpression (basic)
   - ✅ parseStatement (basic)
   - ❌ parseClass (مؤجل)
   - ❌ parseGraphics (مؤجل)
   ```

**الحالة:** ✅ محدد في الخطة

---

### 3. التبعيات بين المهام

**الخطر:** 🟡 منخفض  
**الوصف:** تأخر مهمة يؤخر مهام أخرى.

**مثال:**
```
Parser → Interpreter → StdLib
  ⬇️        ⬇️           ⬇️
 تأخير → لا يمكن البدء → لا يمكن البدء
```

**التخفيف:**
1. **تحديد المسار الحرج:**
```
Critical Path:
Lexer → Parser → Interpreter → StdLib (أساسي)

Parallel Work:
- Documentation (مستقل)
- Tests (مع التطوير)
- Graphics (بعد StdLib)
```

2. **بدائل موقتة:**
```cpp
// إذا تأخر StdLib، استخدم placeholders
void print(const std::string& msg) {
    std::cout << msg << std::endl;  // temporary
}
```

**الحالة:** ✅ مخطط

---

## 🎯 مخاطر الجودة

### 1. تغطية اختبارات منخفضة

**الخطر:** 🔴 عالي  
**الوصف:** كود بدون اختبارات كافية = bugs مخفية.

**التأثير:**
- أخطاء تظهر في Production
- صعوبة Refactoring
- ثقة منخفضة في الكود

**التخفيف:**
1. **هدف واضح:**
   ```
   - Lexer: ≥ 90%
   - Parser: ≥ 85%
   - Interpreter: ≥ 80%
   - إجمالي: ≥ 80%
   ```

2. **Test-Driven Development (TDD):**
   ```cpp
   // 1. اكتب الاختبار أولاً
   TEST(ParserTest, ParseAddition) {
       auto ast = parse("2 + 3");
       EXPECT_EQ(ast->evaluate(), 5);
   }
   
   // 2. اكتب الكود ليمر الاختبار
   std::unique_ptr<Expression> Parser::parseExpression() {
       // implementation
   }
   ```

3. **CI يفرض التغطية:**
   ```yaml
   # .github/workflows/ci.yml
   - name: Check Coverage
     run: |
       coverage=$(get_coverage)
       if [ $coverage -lt 80 ]; then
         echo "Coverage too low: $coverage%"
         exit 1
       fi
   ```

**الحالة:** ⚠️ يحتاج تنفيذ صارم

---

### 2. توثيق ناقص

**الخطر:** 🟠 متوسط  
**الوصف:** كود بدون توثيق = صعوبة الصيانة.

**التخفيف:**
1. **إلزامي في PR Checklist:**
   ```markdown
   - [ ] كل دالة عامة موثقة (AR + EN)
   - [ ] أمثلة موجودة
   ```

2. **أدوات تلقائية:**
   ```powershell
   # سكريبت للتحقق من التوثيق
   $undocumented = Select-String -Path src/**/*.h `
       -Pattern "^[^/]*\w+\s+\w+\([^)]*\)\s*;" `
       -NotMatch "\/\*\*"
   
   if ($undocumented) {
       Write-Error "Found undocumented functions!"
   }
   ```

**الحالة:** ✅ معرّف في المعايير

---

### 3. Technical Debt (الديون التقنية)

**الخطر:** 🟡 منخفض  
**الوصف:** shortcuts تتراكم وتصعب الصيانة لاحقاً.

**أمثلة:**
```cpp
// ❌ Debt - hardcoded values
if (token.getType() == 42) { ... }

// ❌ Debt - duplicate code
void funcA() { /* 50 lines */ }
void funcB() { /* same 50 lines */ }

// ❌ Debt - TODOs كثيرة
// TODO: fix this later
// TODO: refactor
```

**التخفيف:**
1. **منع التراكم:**
   - Code review صارم
   - Refactor فوراً
   - لا "temporary" hacks

2. **تتبع الديون:**
   ```cpp
   // DEBT: This uses O(n²) algorithm. 
   //       Refactor to use hash map (O(n)) in Phase 8.
   void slowFunction() {
       // ...
   }
   ```

**الحالة:** ✅ مراقب

---

## 👥 مخاطر الموارد

### 1. نقص المطورين

**الخطر:** 🟠 متوسط  
**الوصف:** فريق صغير = تقدم بطيء.

**التخفيف:**
1. **تبسيط المساهمة:**
   - وثائق واضحة
   - [دليل البدء السريع](08_QUICK_START.md)
   - Good First Issues

2. **تقسيم العمل:**
   ```
   مطور 1: Lexer/Parser
   مطور 2: Interpreter/Data
   مطور 3: StdLib/Tests
   مطور 4: Documentation
   ```

**الحالة:** ⚠️ يحتاج توظيف/مساهمين

---

### 2. تضارب الأولويات

**الخطر:** 🟡 منخفض  
**الوصف:** المطورون لديهم التزامات أخرى.

**التخفيف:**
1. **جدولة واقعية:**
   - توقع 20-30 ساعة/أسبوع (وليس 40)
   - احسب الإجازات والالتزامات

2. **مرونة:**
   - السماح بتبديل المهام
   - لا deadlines صارمة (MVP فقط)

**الحالة:** ✅ مُراعى

---

## 🎨 مخاطر التصميم

### 1. تغيير المعمارية لاحقاً

**الخطر:** 🔴 عالي  
**الوصف:** اكتشاف خلل معماري بعد كتابة آلاف الأسطر.

**مثال:**
```
اكتشاف: Visitor Pattern لا يناسب احتياجنا
نتيجة: إعادة كتابة Interpreter كاملاً
```

**التخفيف:**
1. **مراجعة معمارية مبكرة:**
   - Phase 0: مراجعة التصميم
   - Prototypes صغيرة
   - استشارة خبراء

2. **معمارية مرنة:**
   ```cpp
   // ✅ استخدام interfaces
   class IEvaluator {
   public:
       virtual Value evaluate(Expression* expr) = 0;
   };
   
   // يمكن استبدال التنفيذ لاحقاً
   class TreeWalkingEvaluator : public IEvaluator { ... };
   class BytecodeEvaluator : public IEvaluator { ... };
   ```

**الحالة:** ✅ معمارية واضحة في [`معمارية_لغة_ص.md`](../معمارية_لغة_ص.md)

---

### 2. Over-Engineering (هندسة زائدة)

**الخطر:** 🟡 منخفض  
**الوصف:** تعقيد غير ضروري يبطئ التطوير.

**أمثلة:**
```cpp
// ❌ Over-engineered
class AbstractFactoryBuilderProviderFactoryImpl { ... }

// ✅ بسيط
class Parser { ... }
```

**التخفيف:**
1. **KISS (Keep It Simple, Stupid):**
   - ابدأ بسيط
   - عقّد عند الحاجة فقط

2. **YAGNI (You Aren't Gonna Need It):**
   - لا تكتب كود "قد نحتاجه مستقبلاً"

**الحالة:** ✅ مُراعى

---

## 📊 ملخص المخاطر

| الخطر | الاحتمال | التأثير | الأولوية | الحالة |
|-------|----------|---------|----------|--------|
| UTF-8 | متوسط | عالي | 🔴 | ✅ مُخفف |
| Operator Precedence | متوسط | متوسط | 🟠 | ⏳ قادم |
| Memory Leaks | منخفض | عالي | 🟠 | ⚠️ مراقب |
| Performance | منخفض | متوسط | 🟡 | ⏳ مؤجل |
| تقدير الوقت | عالي | متوسط | 🟠 | ⚠️ مراقب |
| Scope Creep | متوسط | متوسط | 🟠 | ✅ محدد |
| تغطية منخفضة | متوسط | عالي | 🔴 | ⚠️ إلزامي |
| نقص التوثيق | منخفض | متوسط | 🟠 | ✅ معايير |
| Technical Debt | منخفض | متوسط | 🟡 | ✅ مراقب |
| نقص المطورين | متوسط | عالي | 🟠 | ⚠️ توظيف |
| تغيير المعمارية | منخفض | عالي | 🔴 | ✅ واضحة |

---

## 🎯 خطة الاستجابة السريعة

### إذا حدث خطر حرج:

1. **أوقف التطوير الجديد** (إذا لزم)
2. **قيّم الوضع:**
   - ما حجم المشكلة؟
   - كم ستأخذ للحل؟
   - هل تؤثر على باقي المشروع؟

3. **خيارات:**
   - **حل فوري:** إذا كان سريع
   - **Workaround مؤقت:** إذا كان معقد
   - **تعديل الخطة:** إذا كان جوهري

4. **تواصل:**
   - أخبر الفريق
   - حدّث الوثائق
   - سجل الدروس المستفادة

---

## 📝 سجل المخاطر المحققة

| التاريخ | الخطر | التأثير | الحل | الوقت المفقود |
|--------|-------|---------|------|---------------|
| - | - | - | - | - |

_(سيُحدث عند حدوث مخاطر فعلية)_

---

**آخر تحديث:** 4 نوفمبر 2025  
**المرجع:** [الخطة الرئيسية](00_MASTER_PLAN.md)
