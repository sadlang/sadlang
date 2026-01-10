# بيئة التفاعل المباشر (REPL)

## ما هو REPL؟

REPL هو اختصار لـ Read-Eval-Print Loop، أي حلقة القراءة-التقييم-الطباعة. هي بيئة تفاعلية تسمح للمبرمج بكتابة كود لغة ص وتنفيذه فوراً سطراً بسطر، ورؤية النتائج مباشرة. هذه الأداة مفيدة جداً للتعلم والتجريب واختبار الأفكار السريعة دون الحاجة لإنشاء ملف وحفظه وترجمته.

تعمل بيئة REPL بشكل تكراري: تقرأ ما يكتبه المستخدم، تقيّمه (تنفذه)، تطبع النتيجة، ثم تعود لانتظار المدخل التالي. هذه الحلقة المستمرة تجعل التفاعل مع اللغة سريعاً ومباشراً، مما يسهل فهم كيفية عمل تعابير وعبارات اللغة.

## بنية REPL

يتواجد REPL في المجلد `tools/repl/` ويتكون من:

```
tools/repl/
├── main.cpp              # نقطة الدخول
├── repl_engine.cpp       # محرك REPL الرئيسي
├── repl_engine.h         # واجهة المحرك
├── repl_commands.cpp     # أوامر REPL الخاصة
├── repl_commands.h       # واجهة الأوامر
├── history_manager.cpp   # إدارة السجل
├── history_manager.h     # واجهة السجل
├── CMakeLists.txt        # ملف البناء
└── README.md             # التوثيق
```

## تشغيل REPL

```bash
sad repl
```

عند التشغيل، ترى:

```
╔════════════════════════════════════════════════════════════════════╗
║                    لغة ص - Sad Language REPL                      ║
║                        الإصدار 1.0.0                               ║
╠════════════════════════════════════════════════════════════════════╣
║  اكتب :مساعدة للمساعدة أو :خروج للخروج                             ║
║  Type :help for help or :exit to quit                              ║
╚════════════════════════════════════════════════════════════════════╝

ص> 
```

## استخدام REPL

### التعابير البسيطة

```
ص> 2 + 3
=> 5

ص> "مرحباً" + " " + "بالعالم"
=> "مرحباً بالعالم"

ص> 10 > 5
=> صحيح
```

### المتغيرات

```
ص> متغير س = 42
ص> س * 2
=> 84

ص> متغير اسم = "أحمد"
ص> اطبع("مرحباً يا " + اسم)
مرحباً يا أحمد
```

### الدوال

```
ص> دالة مربع(ن) {
...>     أرجع ن * ن
...> }

ص> مربع(5)
=> 25

ص> مربع(مربع(3))
=> 81
```

### الهياكل المعقدة

```
ص> متغير قائمة = [1, 2, 3, 4, 5]
ص> لكل ع في قائمة {
...>     اطبع(ع * 2)
...> }
2
4
6
8
10
```

## المحرك الرئيسي

```cpp
// من repl_engine.cpp
REPLEngine::REPLEngine(const REPLConfig& config)
    : config_(config)
    , state_(REPLState::Ready)
    , bracketDepth_(0)
    , lastWasEmpty_(false)
{
    // تفعيل دعم الألوان على ويندوز
#ifdef _WIN32
    if (config_.enableColor) {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
#endif

    // تهيئة سجل الأوامر
    if (config_.enableHistory) {
        history_ = std::make_unique<HistoryManager>(
            config_.maxHistorySize,
            config_.historyFile
        );
        history_->load();
    }
    
    // تهيئة معالج الأوامر
    commands_ = std::make_unique<REPLCommands>(this);
    
    // تهيئة المفسر
    Interpreter::InterpreterOptions interpOpts;
    interpOpts.enableDebugMode = false;
    interpOpts.printResults = false;
    interpreter_ = std::make_unique<Interpreter::Interpreter>(interpOpts);
}
```

## حلقة التنفيذ

```cpp
int REPLEngine::run()
{
    printHeader();
    
    while (state_ != REPLState::Exiting) {
        try {
            printPrompt();
            std::string line = readLine();
            
            if (!processLine(line)) {
                break;
            }
        }
        catch (const std::exception& e) {
            printError(std::string("خطأ داخلي / Internal error: ") + e.what());
            state_ = REPLState::Ready;
        }
    }
    
    std::cout << GREEN << "وداعاً! Goodbye!" << RESET << std::endl;
    return 0;
}
```

## الأوامر الخاصة

REPL يدعم أوامر خاصة تبدأ بـ `:`:

| الأمر | الوصف |
|-------|-------|
| `:مساعدة` أو `:help` | عرض المساعدة |
| `:خروج` أو `:exit` | الخروج من REPL |
| `:مسح` أو `:clear` | مسح الشاشة |
| `:سجل` أو `:history` | عرض سجل الأوامر |
| `:حفظ <ملف>` أو `:save <file>` | حفظ الجلسة |
| `:تحميل <ملف>` أو `:load <file>` | تحميل ملف |
| `:نوع <تعبير>` أو `:type <expr>` | عرض نوع تعبير |
| `:متغيرات` أو `:vars` | عرض المتغيرات المعرفة |
| `:إعادة` أو `:reset` | إعادة تعيين البيئة |

```cpp
// من repl_commands.cpp
bool REPLCommands::execute(const std::string& command) {
    if (command == ":مساعدة" || command == ":help") {
        showHelp();
        return true;
    }
    else if (command == ":خروج" || command == ":exit" || command == ":quit") {
        engine_->exit();
        return true;
    }
    else if (command == ":مسح" || command == ":clear") {
        clearScreen();
        return true;
    }
    else if (command.starts_with(":حفظ ") || command.starts_with(":save ")) {
        std::string filename = extractArgument(command);
        saveSession(filename);
        return true;
    }
    // ... المزيد من الأوامر
    
    return false;
}
```

## دعم الأسطر المتعددة

REPL يكتشف تلقائياً العبارات غير المكتملة:

```
ص> دالة حساب(س، ص) {
...>     متغير نتيجة = س + ص
...>     إذا (نتيجة > 10) {
...>         أرجع نتيجة * 2
...>     }
...>     أرجع نتيجة
...> }
```

```cpp
bool REPLEngine::isIncomplete(const std::string& code) {
    int braces = 0;
    int parens = 0;
    int brackets = 0;
    
    for (char c : code) {
        switch (c) {
            case '{': braces++; break;
            case '}': braces--; break;
            case '(': parens++; break;
            case ')': parens--; break;
            case '[': brackets++; break;
            case ']': brackets--; break;
        }
    }
    
    return braces > 0 || parens > 0 || brackets > 0;
}
```

## سجل الأوامر (History)

REPL يحفظ سجل الأوامر للرجوع إليها:

```cpp
// من history_manager.cpp
class HistoryManager {
public:
    HistoryManager(size_t maxSize, const std::string& filePath);
    
    // إضافة سطر للسجل
    void add(const std::string& line);
    
    // التنقل في السجل
    std::string previous();
    std::string next();
    
    // البحث في السجل
    std::vector<std::string> search(const std::string& query);
    
    // حفظ وتحميل
    void save();
    void load();
    
private:
    std::vector<std::string> history_;
    size_t maxSize_;
    size_t currentIndex_;
    std::string filePath_;
};
```

استخدام السهمين للتنقل:
- **↑ (سهم لأعلى):** الأمر السابق
- **↓ (سهم لأسفل):** الأمر التالي

## الإكمال التلقائي

REPL يدعم الإكمال التلقائي بالضغط على Tab:

```
ص> اط<Tab>
=> اطبع

ص> متغ<Tab>
=> متغير
```

## تلوين الكود

REPL يُلوِّن الكود لتسهيل القراءة:

- **الكلمات المفتاحية:** أزرق
- **النصوص:** أخضر
- **الأرقام:** أصفر
- **التعليقات:** رمادي
- **الأخطاء:** أحمر

```cpp
// أكواد الألوان
static const char* RESET = "\033[0m";
static const char* BOLD = "\033[1m";
static const char* RED = "\033[31m";
static const char* GREEN = "\033[32m";
static const char* YELLOW = "\033[33m";
static const char* BLUE = "\033[34m";
static const char* CYAN = "\033[36m";
static const char* MAGENTA = "\033[35m";
```

## معالجة الأخطاء

الأخطاء تُعرض بوضوح دون إنهاء الجلسة:

```
ص> 10 / 0
خطأ: قسمة على صفر
Error: Division by zero

ص> متغير_غير_موجود
خطأ: متغير غير معرّف: متغير_غير_موجود
Error: Undefined variable: متغير_غير_موجود
```

## إعدادات REPL

```cpp
struct REPLConfig {
    bool enableColor = true;        // تفعيل الألوان
    bool enableHistory = true;      // تفعيل السجل
    size_t maxHistorySize = 1000;   // حجم السجل
    std::string historyFile = "~/.sad_history";  // ملف السجل
    std::string prompt = "ص> ";     // المحث
    std::string continuePrompt = "...> ";  // محث الاستمرار
    bool autoSave = true;           // حفظ تلقائي للسجل
};
```

## التقييم

```cpp
std::string REPLEngine::evaluate(const std::string& line)
{
    try {
        // التحليل المعجمي والنحوي
        Lexer::LexerCore lexer(line);
        Parser::ParserCore parser(lexer);
        auto ast = parser.parseProgram();
        
        if (ast.empty()) {
            return "";
        }
        
        // التنفيذ
        // ...
        
        return "تم التنفيذ بنجاح / Executed successfully";
    }
    catch (const std::exception& e) {
        return std::string("خطأ / Error: ") + e.what();
    }
}
```

## ميزات إضافية

### عرض وقت التنفيذ

```
ص> :وقت مربع(1000000)
=> 1000000000000
الوقت: 0.023 ثانية
```

### فحص القيم

```
ص> :فحص قائمة
النوع: قائمة<رقم>
الطول: 5
القيم: [1, 2, 3, 4, 5]
```

### وضع التصحيح

```
ص> :تصحيح تشغيل
وضع التصحيح مُفعَّل

ص> دالة مثال() { أرجع 42 }
[DEBUG] تعريف دالة: مثال
[DEBUG] المعاملات: 0
[DEBUG] نوع الإرجاع: رقم
```

## الخلاصة

بيئة REPL في لغة ص هي أداة قوية للتعلم والتجريب. توفر تفاعلاً مباشراً مع اللغة، مع دعم للألوان وسجل الأوامر والإكمال التلقائي. الأوامر الخاصة تسهل التعامل مع البيئة، ومعالجة الأخطاء الواضحة تساعد على التعلم. سواء كنت مبتدئاً يتعلم اللغة أو مطوراً متقدماً يختبر فكرة، REPL يوفر طريقة سريعة ومريحة للتفاعل مع لغة ص.
