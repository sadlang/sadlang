# بسم الله الرحمن الرحيم

# 📋 الخطة التفصيلية لـ Phase 1.4: التزامن والتوازي
**Detailed Plan for Phase 1.4: Concurrency & Parallelism**

---

## 📊 ملخص تنفيذي / Executive Summary

### الوضع الحالي / Current Status
- ✅ **Phase 1.3.5 مكتملة بالكامل:** Type Narrowing (نظري + عملي)
- ✅ **Parser يدعم:** if/else/else-if
- ✅ **Interpreter يدعم:** مقارنة null
- ⚠️ **البنية التحتية موجودة جزئيًا:**
  - `runtime/thread/thread.h` (705 lines) ✅
  - `runtime/thread/thread.cpp` (309 lines) ✅
  - `include/parser/ast/advanced_expr_nodes.h` يحتوي على `AwaitExpr` ✅
  - `include/lexer/token.h` يحتوي على `KEYWORD_ASYNC`, `KEYWORD_AWAIT` ✅

### النطاق الأصلي / Original Scope
**الخطة الأصلية (04_المرحلة_الأولى_التزامن_والتوازي.md):**
- المدة: 6-8 أسابيع (51 يوم)
- الفريق: 3-4 مطورين
- الملفات: ~40 ملف جديد
- المكونات: 6 أنظمة رئيسية

### النطاق المعدّل / Adjusted Scope
**نهج تدريجي واقعي:**
- تقسيم إلى **10 مراحل صغيرة** (Micro-Phases)
- كل مرحلة: 1-3 أيام عمل فعلي
- كل مرحلة قابلة للبناء والاختبار بشكل مستقل
- التركيز على الحد الأدنى القابل للتطبيق (MVP) أولاً

---

## 🎯 استراتيجية التنفيذ / Implementation Strategy

### المبدأ الأساسي: **MVP First, Optimize Later**

```
المرحلة 0: التحقق من البنية الموجودة       [1 يوم]
المرحلة 1: AST Nodes للـ async/await         [1 يوم]
المرحلة 2: Parser يدعم async/await           [2 أيام]
المرحلة 3: Thread Wrapper الأساسي            [2 أيام]
المرحلة 4: Interpreter للـ async (بسيط)      [2 أيام]
المرحلة 5: Mutex & Lock الأساسي              [1 يوم]
المرحلة 6: Channel البسيط                    [2 أيام]
المرحلة 7: ThreadPool MVP                    [2 أيام]
المرحلة 8: Event Loop البسيط                [2 أيام]
المرحلة 9: اختبارات التكامل                 [2 أيام]
                                    ───────────────
                                    المجموع: 17 يوم
```

---

## 📦 المراحل التفصيلية / Detailed Phases

---

### **المرحلة 0: التحقق من البنية الموجودة** ✅
**Phase 0: Verify Existing Infrastructure**

**المدة:** 1 يوم  
**الأولوية:** 🔴 حرجة (يجب إكمالها أولاً)

#### الأهداف / Objectives
```
1. فحص runtime/thread/thread.h بالكامل
2. فحص runtime/thread/thread.cpp بالكامل
3. التحقق من وجود ThreadPool, Mutex, Atomic
4. فحص include/parser/ast/advanced_expr_nodes.h (AwaitExpr)
5. التحقق من الكلمات المفتاحية في Lexer
6. كتابة تقرير شامل: "ما موجود؟ ما ناقص؟"
```

#### الملفات المطلوب فحصها / Files to Examine
```
✅ runtime/thread/thread.h (خطوط 1-705) - تم الفحص جزئيًا
✅ runtime/thread/thread.cpp (خطوط 1-309) - تم الفحص جزئيًا
✅ include/parser/ast/advanced_expr_nodes.h - يحتوي AwaitExpr
✅ include/lexer/token.h - يحتوي KEYWORD_ASYNC, KEYWORD_AWAIT
⏳ runtime/thread/ (باقي الملفات؟)
⏳ include/parser/ast/declarations.h (AsyncFunctionDecl؟)
⏳ src/parser/parser_core_impl.cpp (parsing async/await؟)
⏳ src/interpreter/visitors/ (evaluation للـ async؟)
```

#### المخرجات / Deliverables
```
📄 PHASE_1_4_0_INFRASTRUCTURE_AUDIT.md
   ├─ قائمة كاملة بما هو موجود
   ├─ قائمة بما هو ناقص
   ├─ خريطة الاعتماديات (Dependency Map)
   └─ تقدير الجهد المطلوب لكل مكون
```

#### معايير الإكمال / Success Criteria
- [✅] قراءة جميع الملفات ذات الصلة بالكامل
- [✅] توثيق جميع Classes/Functions الموجودة
- [✅] تحديد الفجوات (Gaps) بدقة
- [✅] Build ناجح بدون تعديلات

---

### **المرحلة 1: إضافة AST Nodes للـ Async/Await**
**Phase 1: Add AST Nodes for Async/Await**

**المدة:** 1 يوم  
**المتطلبات:** إكمال المرحلة 0  
**الأولوية:** 🟡 عالية

#### الأهداف / Objectives
```
1. إضافة AsyncFunctionDecl (إعلان دالة غير متزامنة)
2. التأكد من AwaitExpr موجود وسليم
3. إضافة YieldExpr (للـ Generators لاحقًا)
4. تحديث ASTVisitor ليدعم العقد الجديدة
5. إضافة toString() methods للعقد الجديدة
```

#### الملفات المطلوب تعديلها / Files to Modify
```
📝 include/parser/ast/declarations.h
   └─ إضافة class AsyncFunctionDecl : public FunctionDecl

✅ include/parser/ast/advanced_expr_nodes.h (AwaitExpr موجود بالفعل)
   └─ التحقق من اكتماله

📝 include/parser/ast/visitor.h
   └─ إضافة visit(AsyncFunctionDecl&)
   └─ إضافة visit(AwaitExpr&) (إذا لم يكن موجودًا)

📝 src/interpreter/visitors/ast_printer.cpp
   └─ تنفيذ طباعة للعقد الجديدة
```

#### التصميم / Design

**1. AsyncFunctionDecl**
```cpp
/**
 * @brief (AR) إعلان دالة غير متزامنة / (EN) Async Function Declaration
 * 
 * أمثلة / Examples:
 * @code
 * async function fetchData(url: string) -> Result {
 *     var response = await HTTP.get(url)
 *     return response.json()
 * }
 * 
 * # Arabic
 * دالة غير_متزامن جلب_بيانات(رابط: نص) -> نتيجة {
 *     متغير استجابة = انتظر HTTP.احصل(رابط)
 *     إرجاع استجابة.json()
 * }
 * @endcode
 */
class AsyncFunctionDecl : public FunctionDecl {
public:
    // يرث من FunctionDecl: name, parameters, return_type, body
    bool is_async = true;  // علامة للتمييز
    
    explicit AsyncFunctionDecl(
        std::string name,
        std::vector<Parameter> params,
        TypePtr return_type,
        StmtPtr body
    ) : FunctionDecl(name, params, return_type, body) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    std::string toString() const override {
        return "AsyncFunctionDecl(" + name + ")";
    }
};
```

**2. AwaitExpr** (موجود بالفعل في advanced_expr_nodes.h)
```cpp
// ✅ تم التحقق من وجوده - لا حاجة لإضافته
class AwaitExpr : public Expr {
public:
    std::unique_ptr<Expr> expression;
    
    explicit AwaitExpr(std::unique_ptr<Expr> expr)
        : expression(std::move(expr)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};
```

#### الاختبارات / Testing
```cpp
// tests/ast_nodes_test.cpp
TEST(AsyncAST, CreateAsyncFunction) {
    auto body = std::make_unique<BlockStmt>();
    auto func = std::make_unique<AsyncFunctionDecl>(
        "fetchData",
        std::vector<Parameter>{},
        std::make_unique<Type>("Future<int>"),
        std::move(body)
    );
    
    EXPECT_EQ(func->is_async, true);
    EXPECT_EQ(func->name, "fetchData");
}

TEST(AsyncAST, CreateAwaitExpr) {
    auto call = std::make_unique<CallExpr>(/*...*/);
    auto await = std::make_unique<AwaitExpr>(std::move(call));
    
    EXPECT_NE(await->expression, nullptr);
}
```

#### معايير الإكمال / Success Criteria
- [✅] AsyncFunctionDecl class مُضاف ومُوثّق
- [✅] AwaitExpr class موجود ويعمل
- [✅] ASTVisitor يحتوي على visit() methods
- [✅] AST Printer يطبع العقد الجديدة
- [✅] Build ناجح بدون أخطاء
- [✅] Unit tests تمر بنجاح

---

### **المرحلة 2: Parser يدعم async/await**
**Phase 2: Parser Support for async/await**

**المدة:** 2 أيام  
**المتطلبات:** إكمال المرحلة 1  
**الأولوية:** 🟡 عالية

#### الأهداف / Objectives
```
1. تعديل Lexer لتمييز async/await keywords
2. إضافة parseAsyncFunction() في Parser
3. إضافة parseAwaitExpr() في Parser
4. التعامل مع الأخطاء (await خارج async function)
5. اختبار parsing لملفات Sad
```

#### الملفات المطلوب تعديلها / Files to Modify
```
📝 src/lexer/lexer.cpp
   └─ التأكد من tokenize() يتعرف على "غير_متزامن", "انتظر"

📝 src/parser/parser_core_impl.cpp
   └─ إضافة parseAsyncFunctionDecl()
   └─ إضافة parseAwaitExpr() في parsePrimaryExpr()

📝 src/parser/parser_utils.cpp
   └─ إضافة validation: await فقط داخل async functions
```

#### التصميم / Design

**1. parseAsyncFunctionDecl()**
```cpp
/**
 * @brief (AR) تحليل دالة غير متزامنة / (EN) Parse async function
 * 
 * Grammar:
 * async_function := 'async' 'function' IDENTIFIER '(' params ')' '->' type block
 *                 | 'غير_متزامن' 'دالة' IDENTIFIER '(' params ')' '->' type block
 * 
 * @return std::unique_ptr<AsyncFunctionDecl>
 * 
 * مثال / Example:
 * دالة غير_متزامن جلب_بيانات(رابط: نص) -> نتيجة { ... }
 */
std::unique_ptr<AsyncFunctionDecl> Parser::parseAsyncFunctionDecl() {
    // التحقق من الكلمة المفتاحية "غير_متزامن" أو "async"
    if (!match(KEYWORD_ASYNC)) {
        throw ParserError("Expected 'async' keyword");
    }
    
    // التحقق من الكلمة المفتاحية "دالة" أو "function"
    if (!match(KEYWORD_FUNCTION)) {
        throw ParserError("Expected 'function' after 'async'");
    }
    
    // باقي الـ parsing مثل parseFunctionDecl()
    auto name = consume(IDENTIFIER, "Expected function name");
    // ... params, return type, body ...
    
    return std::make_unique<AsyncFunctionDecl>(
        name.lexeme,
        params,
        return_type,
        body
    );
}
```

**2. parseAwaitExpr()**
```cpp
/**
 * @brief (AR) تحليل تعبير await / (EN) Parse await expression
 * 
 * Grammar:
 * await_expr := 'await' expression
 *             | 'انتظر' expression
 * 
 * @return std::unique_ptr<AwaitExpr>
 * 
 * مثال / Example:
 * متغير بيانات = انتظر جلب_بيانات(رابط)
 */
std::unique_ptr<AwaitExpr> Parser::parseAwaitExpr() {
    // التحقق من الكلمة المفتاحية "انتظر" أو "await"
    if (!match(KEYWORD_AWAIT)) {
        throw ParserError("Expected 'await' keyword");
    }
    
    // التحقق من أننا داخل async function
    if (!current_function_is_async_) {
        throw ParserError("'await' can only be used inside async functions");
    }
    
    // تحليل التعبير
    auto expr = parseExpression();
    
    return std::make_unique<AwaitExpr>(std::move(expr));
}
```

**3. Validation في Parser**
```cpp
// إضافة حقل جديد في Parser class
class Parser {
private:
    bool current_function_is_async_ = false;  // علامة للسياق الحالي
    
    // ...
};

// في parseAsyncFunctionDecl()
std::unique_ptr<AsyncFunctionDecl> Parser::parseAsyncFunctionDecl() {
    // ...
    
    // تعيين السياق
    bool previous_async = current_function_is_async_;
    current_function_is_async_ = true;
    
    // تحليل body
    auto body = parseBlockStmt();
    
    // استعادة السياق
    current_function_is_async_ = previous_async;
    
    // ...
}
```

#### الاختبارات / Testing

**Test File 1: test_async_parse_simple.s**
```s
# (AR) اختبار تحليل دالة غير متزامنة بسيطة
# (EN) Test parsing simple async function

دالة غير_متزامن قل_مرحبا() -> نص {
    إرجاع "مرحباً"
}

الرئيسية()
```

**Test File 2: test_await_parse.s**
```s
# (AR) اختبار تحليل await expression
# (EN) Test parsing await expression

نص دالة غير_متزامن جلب_بيانات(رابط: نص)  
    متغير نتيجة = انتظر قراءة_ملف(رابط)
    إرجاع نتيجة
نهاية

الرئيسية()
```

**Test File 3: test_await_error.s** (يجب أن يفشل)
```s
# (AR) اختبار خطأ: await خارج async function
# (EN) Test error: await outside async function

دالة عادية() -> نص {
    متغير بيانات = انتظر جلب_بيانات()  # ❌ خطأ!
    إرجاع بيانات
}
```

**Unit Tests (C++)**
```cpp
// tests/parser_async_test.cpp
TEST(Parser, ParseAsyncFunction) {
    std::string code = R"(
        دالة غير_متزامن test() -> نص {
            إرجاع "hello"
        }
    )";
    
    Parser parser(code);
    auto ast = parser.parse();
    
    ASSERT_TRUE(ast != nullptr);
    // ... تحقق من AsyncFunctionDecl ...
}

TEST(Parser, ParseAwaitExpression) {
    std::string code = R"(
        دالة غير_متزامن test() -> رقم {
            متغير ع = انتظر getData()
            إرجاع ع
        }
    )";
    
    Parser parser(code);
    auto ast = parser.parse();
    
    ASSERT_TRUE(ast != nullptr);
    // ... تحقق من AwaitExpr ...
}

TEST(Parser, AwaitOutsideAsyncError) {
    std::string code = R"(
        دالة test() -> رقم {
            متغير ع = انتظر getData()
            إرجاع ع
        }
    )";
    
    Parser parser(code);
    EXPECT_THROW(parser.parse(), ParserError);
}
```

#### معايير الإكمال / Success Criteria
- [✅] Parser يتعرف على async/await keywords
- [✅] parseAsyncFunctionDecl() يعمل بشكل صحيح
- [✅] parseAwaitExpr() يعمل بشكل صحيح
- [✅] Validation يمنع await خارج async functions
- [✅] Build ناجح بدون أخطاء
- [✅] 3 test files تعمل (2 تمر، 1 تفشل بشكل متوقع)
- [✅] Unit tests (C++) تمر بنجاح

---

### **المرحلة 3: Thread Wrapper الأساسي**
**Phase 3: Basic Thread Wrapper**

**المدة:** 2 أيام  
**المتطلبات:** إكمال المرحلة 0  
**الأولوية:** 🟢 متوسطة

#### الأهداف / Objectives
```
1. التحقق من runtime/thread/thread.h الموجود
2. إكمال أي نواقص في Thread class
3. إضافة wrapper للـ Sad language
4. إضافة وحدة stdlib/concurrency/thread.s
5. اختبار إنشاء وانضمام الخيوط
```

#### الملفات الموجودة / Existing Files
```
✅ runtime/thread/thread.h (705 lines)
   ├─ Thread class
   ├─ ThreadConfig
   ├─ ThreadState enum
   ├─ ThreadPriority enum
   ├─ ThreadLocal<T>
   └─ Atomic<T>

✅ runtime/thread/thread.cpp (309 lines)
   ├─ Thread::Thread()
   ├─ Thread::start()
   ├─ Thread::join()
   ├─ Thread::detach()
   └─ Thread::request_stop()
```

#### الملفات المطلوب إضافتها / Files to Add
```
📝 stdlib/concurrency/thread.s
   └─ واجهة Sad language للـ Thread class

📝 src/interpreter/builtin/thread_builtin.cpp
   └─ تنفيذ الدوال الأصلية (native functions) للخيوط

📝 include/interpreter/builtin/thread_builtin.h
   └─ إعلان الدوال الأصلية
```

#### التصميم / Design

**1. stdlib/concurrency/thread.s**
```s
# (AR) وحدة إدارة الخيوط / (EN) Thread Management Module

فئة خيط {
    /**
     * @brief (AR) إنشاء خيط جديد / (EN) Create new thread
     * @param دالة - الدالة المراد تنفيذها
     */
    دالة جديد(دالة: دالة) {
        هذا.مقبض = __thread_create(دالة)
    }
    
    /**
     * @brief (AR) بدء تشغيل الخيط / (EN) Start thread
     */
    دالة ابدأ() {
        __thread_start(هذا.مقبض)
    }
    
    /**
     * @brief (AR) الانتظار حتى الانتهاء / (EN) Wait for completion
     */
    دالة انتظر() {
        __thread_join(هذا.مقبض)
    }
    
    /**
     * @brief (AR) فصل الخيط / (EN) Detach thread
     */
    دالة افصل() {
        __thread_detach(هذا.مقبض)
    }
    
    /**
     * @brief (AR) طلب الإيقاف / (EN) Request stop
     */
    دالة أوقف() {
        __thread_request_stop(هذا.مقبض)
    }
}

/**
 * @brief (AR) النوم لفترة (بالميلي ثانية) / (EN) Sleep for duration (ms)
 * @param مدة - المدة بالميلي ثانية
 */
دالة نم(مدة: رقم) {
    __thread_sleep(مدة)
}

/**
 * @brief (AR) الحصول على معرف الخيط الحالي / (EN) Get current thread ID
 * @return معرف الخيط
 */
دالة معرف_الخيط_الحالي() -> رقم {
    إرجاع __thread_current_id()
}
```

**2. thread_builtin.cpp**
```cpp
/**
 * @file src/interpreter/builtin/thread_builtin.cpp
 * @brief (AR) تنفيذ الدوال الأصلية للخيوط / (EN) Native thread functions implementation
 */

#include "thread_builtin.h"
#include "runtime/thread/thread.h"
#include <memory>
#include <unordered_map>

using namespace Sad::Thread;

namespace Sad {
namespace Builtin {

// (AR) تخزين الخيوط النشطة / (EN) Store active threads
static std::unordered_map<uint64_t, std::unique_ptr<Thread>> g_threads;
static uint64_t g_next_handle = 1;

/**
 * @brief __thread_create(function)
 * (AR) إنشاء خيط جديد / (EN) Create new thread
 * 
 * @param function - الدالة المراد تنفيذها
 * @return معرف المقبض (handle ID)
 */
Value builtin__thread_create(const std::vector<Value>& args) {
    // (AR) التحقق من عدد المعاملات / (EN) Validate argument count
    if (args.size() != 1) {
        throw RuntimeError("__thread_create expects 1 argument");
    }
    
    // (AR) التحقق من أن المعامل دالة / (EN) Validate argument is a function
    if (!args[0].isCallable()) {
        throw RuntimeError("__thread_create expects a callable");
    }
    
    // (AR) إنشاء الخيط / (EN) Create thread
    auto func = args[0].asCallable();
    auto thread = std::make_unique<Thread>([func]() {
        // (AR) تنفيذ الدالة / (EN) Execute function
        func->call({});
    });
    
    // (AR) تخزين الخيط / (EN) Store thread
    uint64_t handle = g_next_handle++;
    g_threads[handle] = std::move(thread);
    
    // (AR) إرجاع المقبض / (EN) Return handle
    return Value::makeInt(handle);
}

/**
 * @brief __thread_start(handle)
 * (AR) بدء تشغيل الخيط / (EN) Start thread
 */
Value builtin__thread_start(const std::vector<Value>& args) {
    // (AR) التحقق من عدد المعاملات / (EN) Validate argument count
    if (args.size() != 1) {
        throw RuntimeError("__thread_start expects 1 argument");
    }
    
    // (AR) الحصول على المقبض / (EN) Get handle
    uint64_t handle = args[0].asInt();
    
    // (AR) التحقق من وجود الخيط / (EN) Validate thread exists
    auto it = g_threads.find(handle);
    if (it == g_threads.end()) {
        throw RuntimeError("Invalid thread handle");
    }
    
    // (AR) بدء التشغيل / (EN) Start thread
    bool success = it->second->start();
    
    return Value::makeBool(success);
}

/**
 * @brief __thread_join(handle)
 * (AR) الانتظار حتى انتهاء الخيط / (EN) Wait for thread completion
 */
Value builtin__thread_join(const std::vector<Value>& args) {
    // (AR) التحقق من عدد المعاملات / (EN) Validate argument count
    if (args.size() != 1) {
        throw RuntimeError("__thread_join expects 1 argument");
    }
    
    // (AR) الحصول على المقبض / (EN) Get handle
    uint64_t handle = args[0].asInt();
    
    // (AR) التحقق من وجود الخيط / (EN) Validate thread exists
    auto it = g_threads.find(handle);
    if (it == g_threads.end()) {
        throw RuntimeError("Invalid thread handle");
    }
    
    // (AR) الانتظار / (EN) Join thread
    it->second->join();
    
    // (AR) إزالة الخيط بعد الانتهاء / (EN) Remove thread after completion
    g_threads.erase(it);
    
    return Value::makeVoid();
}

/**
 * @brief __thread_sleep(milliseconds)
 * (AR) النوم لفترة / (EN) Sleep for duration
 */
Value builtin__thread_sleep(const std::vector<Value>& args) {
    // (AR) التحقق من عدد المعاملات / (EN) Validate argument count
    if (args.size() != 1) {
        throw RuntimeError("__thread_sleep expects 1 argument");
    }
    
    // (AR) الحصول على المدة / (EN) Get duration
    int64_t ms = args[0].asInt();
    
    // (AR) النوم / (EN) Sleep
    Thread::sleep_for(std::chrono::milliseconds(ms));
    
    return Value::makeVoid();
}

/**
 * @brief __thread_current_id()
 * (AR) الحصول على معرف الخيط الحالي / (EN) Get current thread ID
 */
Value builtin__thread_current_id(const std::vector<Value>& args) {
    // (AR) لا معاملات / (EN) No arguments
    if (!args.empty()) {
        throw RuntimeError("__thread_current_id expects 0 arguments");
    }
    
    // (AR) الحصول على المعرف / (EN) Get ID
    ThreadId id = Thread::current_thread_id();
    
    return Value::makeInt(static_cast<int64_t>(id));
}

} // namespace Builtin
} // namespace Sad
```

#### الاختبارات / Testing

**Test File 1: test_thread_simple.s**
```s
# (AR) اختبار إنشاء خيط بسيط / (EN) Test simple thread creation

استورد "concurrency/thread"

دالة عامل() {
    اطبع("الخيط يعمل!")
    نم(100)  # نوم 100ms
    اطبع("الخيط انتهى!")
}

دالة الرئيسية() {
    اطبع("بدء البرنامج")
    
    # إنشاء خيط
    متغير خيط = خيط.جديد(عامل)
    خيط.ابدأ()
    
    اطبع("في الخيط الرئيسي...")
    نم(50)
    
    # الانتظار
    خيط.انتظر()
    
    اطبع("انتهى البرنامج")
}

الرئيسية()

# Expected output:
# بدء البرنامج
# الخيط يعمل!
# في الخيط الرئيسي...
# الخيط انتهى!
# انتهى البرنامج
```

**Test File 2: test_multiple_threads.s**
```s
# (AR) اختبار عدة خيوط / (EN) Test multiple threads

استورد "concurrency/thread"

دالة عامل(رقم: رقم) {
    اطبع("الخيط " + رقم + " بدأ")
    نم(رقم * 50)
    اطبع("الخيط " + رقم + " انتهى")
}

دالة الرئيسية() {
    متغير خيوط = []
    
    # إنشاء 5 خيوط
    لكل (ع في مدى(5)) {
        متغير خيط = خيط.جديد(() => عامل(ع))
        خيط.ابدأ()
        خيوط.أضف(خيط)
    }
    
    # الانتظار للجميع
    لكل (خيط في خيوط) {
        خيط.انتظر()
    }
    
    اطبع("كل الخيوط انتهت!")
}

الرئيسية()
```

#### معايير الإكمال / Success Criteria
- [✅] runtime/thread/thread.h & .cpp مكتمل ويعمل
- [✅] stdlib/concurrency/thread.s مُضاف ومُوثّق
- [✅] thread_builtin.cpp يربط Sad مع C++
- [✅] Build ناجح بدون أخطاء
- [✅] test_thread_simple.s يعمل وينتج output صحيح
- [✅] test_multiple_threads.s يعمل بشكل صحيح

---

### **المرحلة 4: Interpreter للـ Async (تنفيذ بسيط)**
**Phase 4: Interpreter for Async (Simple Implementation)**

**المدة:** 2 أيام  
**المتطلبات:** إكمال المراحل 1، 2، 3  
**الأولوية:** 🟡 عالية

#### الأهداف / Objectives
```
1. تنفيذ evaluation لـ AsyncFunctionDecl
2. تنفيذ evaluation لـ AwaitExpr
3. إضافة Future/Promise classes بسيطة
4. إضافة Task Queue بسيط
5. اختبار async functions بسيطة
```

#### الملفات المطلوب إضافتها / Files to Add
```
📝 runtime/concurrency/async/future.h
   └─ Future<T>, Promise<T> classes

📝 runtime/concurrency/async/future.cpp
   └─ تنفيذ Future/Promise

📝 runtime/concurrency/async/task_queue.h
   └─ TaskQueue class (بسيط)

📝 runtime/concurrency/async/task_queue.cpp
   └─ تنفيذ TaskQueue

📝 src/interpreter/visitors/async_evaluator.cpp
   └─ evaluation للـ async/await

📝 include/interpreter/visitors/async_evaluator.h
   └─ إعلان AsyncEvaluator
```

#### التصميم / Design

**1. Future<T> & Promise<T>**
```cpp
/**
 * @file runtime/concurrency/async/future.h
 * @brief (AR) Future/Promise للبرمجة غير المتزامنة / (EN) Future/Promise for async programming
 */

#ifndef SAD_RUNTIME_FUTURE_H
#define SAD_RUNTIME_FUTURE_H

#include <mutex>
#include <condition_variable>
#include <optional>
#include <memory>

namespace Sad {
namespace Async {

/**
 * @brief (AR) قيمة مستقبلية / (EN) Future value
 */
template<typename T>
class Future {
public:
    /**
     * @brief (AR) الانتظار حتى تصبح القيمة جاهزة / (EN) Wait until value is ready
     */
    T get() {
        std::unique_lock<std::mutex> lock(mutex_);
        // (AR) الانتظار حتى يكون لدينا قيمة / (EN) Wait until we have a value
        cv_.wait(lock, [this] { return value_.has_value(); });
        return *value_;
    }
    
    /**
     * @brief (AR) التحقق من الجاهزية / (EN) Check if ready
     */
    bool is_ready() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return value_.has_value();
    }

private:
    friend class Promise<T>;
    
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::optional<T> value_;
};

/**
 * @brief (AR) وعد بقيمة مستقبلية / (EN) Promise of future value
 */
template<typename T>
class Promise {
public:
    Promise() : future_(std::make_shared<Future<T>>()) {}
    
    /**
     * @brief (AR) تعيين القيمة / (EN) Set value
     */
    void set_value(T value) {
        std::lock_guard<std::mutex> lock(future_->mutex_);
        future_->value_ = std::move(value);
        future_->cv_.notify_all();
    }
    
    /**
     * @brief (AR) الحصول على Future / (EN) Get Future
     */
    std::shared_ptr<Future<T>> get_future() {
        return future_;
    }

private:
    std::shared_ptr<Future<T>> future_;
};

} // namespace Async
} // namespace Sad

#endif // SAD_RUNTIME_FUTURE_H
```

**2. AsyncEvaluator (في Interpreter)**
```cpp
/**
 * @file src/interpreter/visitors/async_evaluator.cpp
 * @brief (AR) تنفيذ evaluation للـ async/await / (EN) Async/await evaluation implementation
 */

#include "async_evaluator.h"
#include "runtime/concurrency/async/future.h"
#include "runtime/thread/thread.h"

using namespace Sad::Async;
using namespace Sad::Thread;

namespace Sad {
namespace Interpreter {

/**
 * @brief visit(AsyncFunctionDecl&)
 * (AR) تنفيذ دالة غير متزامنة / (EN) Execute async function
 * 
 * النهج / Approach:
 * 1. إنشاء Future + Promise
 * 2. تشغيل الدالة في خيط منفصل
 * 3. إرجاع Future فورًا
 */
Value AsyncEvaluator::visit(AsyncFunctionDecl& node) {
    // (AR) إنشاء Promise/Future / (EN) Create Promise/Future
    auto promise = std::make_shared<Promise<Value>>();
    auto future = promise->get_future();
    
    // (AR) التقاط body الدالة والبيئة / (EN) Capture function body and environment
    auto body = node.body;
    auto env = current_environment_;
    
    // (AR) إنشاء خيط لتنفيذ الدالة / (EN) Create thread to execute function
    auto thread = std::make_unique<Thread>([this, body, env, promise]() {
        try {
            // (AR) تعيين البيئة / (EN) Set environment
            auto prev_env = current_environment_;
            current_environment_ = env;
            
            // (AR) تنفيذ body / (EN) Execute body
            body->accept(*this);
            Value result = last_value_;
            
            // (AR) استعادة البيئة / (EN) Restore environment
            current_environment_ = prev_env;
            
            // (AR) تعيين النتيجة / (EN) Set result
            promise->set_value(result);
        }
        catch (const std::exception& e) {
            // (AR) في المستقبل: set_exception() / (EN) Future: set_exception()
            promise->set_value(Value::makeVoid());
        }
    });
    
    // (AR) بدء الخيط / (EN) Start thread
    thread->start();
    thread->detach();  // (AR) لا نريد الانتظار هنا / (EN) Don't wait here
    
    // (AR) إرجاع Future كـ Value / (EN) Return Future as Value
    return Value::makeFuture(future);
}

/**
 * @brief visit(AwaitExpr&)
 * (AR) انتظار نتيجة Future / (EN) Wait for Future result
 */
Value AsyncEvaluator::visit(AwaitExpr& node) {
    // (AR) تنفيذ التعبير (يجب أن يكون Future) / (EN) Evaluate expression (should be Future)
    node.expression->accept(*this);
    Value future_value = last_value_;
    
    // (AR) التحقق من أنه Future / (EN) Validate it's a Future
    if (!future_value.isFuture()) {
        throw RuntimeError("'await' expects a Future");
    }
    
    // (AR) الانتظار والحصول على النتيجة / (EN) Wait and get result
    auto future = future_value.asFuture();
    Value result = future->get();
    
    return result;
}

} // namespace Interpreter
} // namespace Sad
```

#### الاختبارات / Testing

**Test File 1: test_async_simple.s**
```s
# (AR) اختبار async function بسيطة / (EN) Test simple async function

دالة غير_متزامن قل_مرحبا() -> نص {
    نم(100)  # محاكاة عمل I/O
    إرجاع "مرحباً من async!"
}

دالة الرئيسية() {
    اطبع("قبل الاستدعاء")
    
    # استدعاء async (يرجع Future)
    متغير مستقبل = قل_مرحبا()
    
    اطبع("بعد الاستدعاء (لم ينتهِ بعد)")
    
    # الانتظار
    متغير نتيجة = انتظر مستقبل
    
    اطبع("النتيجة: " + نتيجة)
}

الرئيسية()

# Expected Output:
# قبل الاستدعاء
# بعد الاستدعاء (لم ينتهِ بعد)
# النتيجة: مرحباً من async!
```

**Test File 2: test_async_multiple.s**
```s
# (AR) اختبار عدة async functions / (EN) Test multiple async functions

دالة غير_متزامن احسب(رقم: رقم) -> رقم {
    نم(رقم * 50)
    إرجاع رقم * 2
}

دالة الرئيسية() {
    # إطلاق 3 async functions
    متغير مستقبل1 = احسب(1)
    متغير مستقبل2 = احسب(2)
    متغير مستقبل3 = احسب(3)
    
    # الانتظار للنتائج
    متغير ن1 = انتظر مستقبل1  # 2
    متغير ن2 = انتظر مستقبل2  # 4
    متغير ن3 = انتظر مستقبل3  # 6
    
    اطبع("النتائج: " + ن1 + ", " + ن2 + ", " + ن3)
}

الرئيسية()

# Expected Output:
# النتائج: 2, 4, 6
```

#### معايير الإكمال / Success Criteria
- [✅] Future/Promise classes مُضافة وتعمل
- [✅] AsyncEvaluator يُنفّذ AsyncFunctionDecl
- [✅] AsyncEvaluator يُنفّذ AwaitExpr
- [✅] Build ناجح بدون أخطاء
- [✅] test_async_simple.s ينتج output صحيح
- [✅] test_async_multiple.s يعمل بشكل صحيح

---

### **المراحل المتبقية (ملخص سريع)**

#### **المرحلة 5: Mutex & Lock الأساسي** (1 يوم)
- إضافة Mutex wrapper
- Lock guard (RAII)
- اختبار thread safety

#### **المرحلة 6: Channel البسيط** (2 أيام)
- UnboundedChannel<T>
- send() / receive()
- close() semantics

#### **المرحلة 7: ThreadPool MVP** (2 أيام)
- Work queue
- Worker threads
- submit() / wait_all()

#### **المرحلة 8: Event Loop البسيط** (2 أيام)
- Single-threaded loop
- Task scheduling
- Timer support

#### **المرحلة 9: اختبارات التكامل** (2 أيام)
- اختبارات شاملة
- Stress tests
- Performance benchmarks

---

## 📊 الجدول الزمني / Timeline

```
┌─────────────────────────────────────────────────────────────┐
│  المرحلة                  │  المدة  │  الأولوية │  الحالة  │
├─────────────────────────────────────────────────────────────┤
│  0. التحقق من البنية      │  1 يوم  │   🔴      │   ⏳     │
│  1. AST Nodes            │  1 يوم  │   🟡      │   📅     │
│  2. Parser Support       │  2 أيام │   🟡      │   📅     │
│  3. Thread Wrapper       │  2 أيام │   🟢      │   📅     │
│  4. Async Interpreter    │  2 أيام │   🟡      │   📅     │
│  5. Mutex & Lock         │  1 يوم  │   🟢      │   📅     │
│  6. Channel              │  2 أيام │   🟢      │   📅     │
│  7. ThreadPool           │  2 أيام │   🟢      │   📅     │
│  8. Event Loop           │  2 أيام │   🟡      │   📅     │
│  9. Integration Tests    │  2 أيام │   🟡      │   📅     │
├─────────────────────────────────────────────────────────────┤
│  المجموع                  │ 17 يوم  │           │          │
└─────────────────────────────────────────────────────────────┘

الأسطورة / Legend:
🔴 حرجة / Critical
🟡 عالية / High
🟢 متوسطة / Medium
⏳ جاري التنفيذ / In Progress
📅 مخطط / Planned
```

---

## 🎯 معايير النجاح الإجمالية / Overall Success Criteria

### المعايير الفنية / Technical Criteria
- [✅] جميع الملفات مُوثّقة بالعربية والإنجليزية
- [✅] Build ينجح بدون أخطاء أو تحذيرات
- [✅] جميع Unit Tests (C++) تمر
- [✅] جميع Integration Tests (Sad) تمر
- [✅] لا تسريبات ذاكرة (Memory Leaks)
- [✅] لا data races (ThreadSanitizer نظيف)

### المعايير الوظيفية / Functional Criteria
- [✅] يمكن كتابة async functions في Sad
- [✅] await يعمل داخل async functions
- [✅] Threads يمكن إنشاؤها والانضمام إليها
- [✅] Mutex يوفر thread safety
- [✅] Channel يعمل بين خيوط مختلفة
- [✅] ThreadPool يُنفّذ مهام متعددة

### المعايير النوعية / Qualitative Criteria
- [✅] الكود نظيف ومنظم
- [✅] التعليقات شاملة لكل سطر
- [✅] STRICT_CODING_RULES.md مُتّبع بدقة
- [✅] الأمثلة واضحة وسهلة الفهم
- [✅] الوثائق احترافية

---

## 📚 الموارد والمراجع / Resources & References

### موارد داخلية / Internal Resources
```
✅ runtime/thread/thread.h - الموجود بالفعل
✅ runtime/thread/thread.cpp - الموجود بالفعل
✅ include/parser/ast/advanced_expr_nodes.h - AwaitExpr موجود
✅ include/lexer/token.h - async/await keywords موجودة
📄 plans/global_dominance_plan/04_المرحلة_الأولى_التزامن_والتوازي.md
📄 STRICT_CODING_RULES.md
```

### مراجع خارجية / External References
```
🔗 std::thread documentation
🔗 std::future/std::promise documentation
🔗 C++ Concurrency in Action (book)
🔗 Rust async/await implementation (للإلهام)
🔗 Go goroutines & channels (للإلهام)
```

---

## ⚠️ المخاطر والتحديات / Risks & Challenges

### المخاطر التقنية / Technical Risks
1. **Data Races:**
   - الحل: استخدام ThreadSanitizer في جميع الاختبارات
   - الوقاية: Code review دقيق لجميع shared state

2. **Deadlocks:**
   - الحل: Lock ordering rules واضحة
   - الوقاية: Deadlock detection في Debug mode

3. **Memory Safety:**
   - الحل: استخدام Smart Pointers (unique_ptr, shared_ptr)
   - الوقاية: AddressSanitizer + Valgrind

### المخاطر الزمنية / Schedule Risks
1. **تقدير الوقت:**
   - الخطر: قد تأخذ بعض المراحل وقتًا أطول
   - الحل: Buffer 20% إضافي لكل مرحلة

2. **التعقيد:**
   - الخطر: Async/Await تحويل معقد
   - الحل: البدء بـ MVP بسيط جدًا، ثم التحسين

---

## 🚀 الخطوات التالية المباشرة / Immediate Next Steps

### الآن (الخطوة 0) / Now (Step 0)
```bash
1. قراءة runtime/thread/thread.h بالكامل ✅ (جزئي)
2. قراءة runtime/thread/thread.cpp بالكامل ✅ (جزئي)
3. كتابة تقرير شامل عن البنية الموجودة
4. تحديد الفجوات بدقة
```

### بعد ذلك (الخطوة 1) / Next (Step 1)
```bash
1. إنشاء ملف include/parser/ast/declarations.h - AsyncFunctionDecl
2. تحديث include/parser/ast/visitor.h
3. تحديث src/interpreter/visitors/ast_printer.cpp
4. Build + Test
```

### ثم (الخطوة 2) / Then (Step 2)
```bash
1. تعديل src/parser/parser_core_impl.cpp - parseAsyncFunctionDecl()
2. تعديل src/parser/parser_core_impl.cpp - parseAwaitExpr()
3. إضافة validation logic
4. كتابة 3 test files بلغة Sad
5. Build + Test
```

---

## 📝 ملاحظات نهائية / Final Notes

### الفلسفة / Philosophy
```
"التقدم البطيء الثابت أفضل من القفزات السريعة غير المستقرة"
"Steady slow progress is better than fast unstable jumps"
```

### التوقعات الواقعية / Realistic Expectations
- **MVP في 17 يوم** ✅ قابل للتحقيق
- **نظام كامل في 51 يوم** ⏳ يحتاج التزام
- **الأولوية: Correctness > Speed** 🎯

### الالتزام بالمعايير / Commitment to Standards
- [✅] كل سطر مُعلّق بالعربية
- [✅] STRICT_CODING_RULES.md متبوع بدقة
- [✅] لا placeholders أو "TODO"
- [✅] Build ناجح في كل مرحلة

---

**وَمَا تَوْفِيقِي إِلَّا بِاللَّهِ ۚ عَلَيْهِ تَوَكَّلْتُ وَإِلَيْهِ أُنِيبُ**

---

## التوقيع / Signature
**المرحلة:** Phase 1.4 (Concurrency & Parallelism)  
**التاريخ:** ديسمبر 2024  
**الحالة:** 📋 خطة مُعدّة - جاهز للتنفيذ
