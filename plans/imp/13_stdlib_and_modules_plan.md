# 📚 خطة تنفيذ المكتبة القياسية والموديولات / StdLib & Modules Implementation Plan

**التاريخ:** 4 نوفمبر 2025  
**الإصدار:** 1.0  
**المدة المقدرة:** 3 أسابيع

---

## 📋 ملخص / Summary

### (AR) الملخص
خطة تفصيلية لتطوير المكتبة القياسية (StdLib) ونظام الموديولات للغة "ص". يشمل io، string، math، time، وتكامل Graphics.

### (EN) Summary
Detailed plan for developing the Standard Library (StdLib) and module system for Sad language. Includes io, string, math, time, and Graphics integration.

---

## 🎯 الأهداف / Objectives

1. ✅ تنفيذ StdLib Manager (من 09_integration)
2. ✅ بناء io module (print, input, file I/O)
3. ✅ بناء string module (manipulation, regex)
4. ✅ بناء math module (trig, random, stats)
5. ✅ بناء time module (date, sleep, timing)
6. ✅ تكامل graphics module
7. ✅ نظام module loading ديناميكي
8. ✅ FFI interface لـ C/C++

---

## 📦 الاعتماديات / Dependencies

### من language_spec:
- ✅ `01_types.md` - أنواع البيانات
- ✅ `02_functions.md` - Function signatures

### من plans:
- ✅ `09_integration_and_architecture.md` - StdLib Manager interface
- ✅ `12_interpreter_runtime_plan.md` - Value system

---

## 🏗️ البنية المعمارية / Architecture

### StdLib Manager (From 09_integration)

```cpp
// include/stdlib/core/stdlib_manager.h
namespace Sad::StdLib {

using NativeFunction = std::function<Value(const std::vector<Value>&)>;

class StdLibManager {
private:
    std::map<std::string, std::map<std::string, NativeFunction>> modules;
    
public:
    void registerModule(const std::string& moduleName,
                       const std::map<std::string, NativeFunction>& functions) {
        modules[moduleName] = functions;
        DEBUG_PRINT("Registered module: %s with %zu functions", 
                    moduleName.c_str(), functions.size());
    }
    
    void registerFunction(const std::string& moduleName,
                         const std::string& funcName,
                         NativeFunction func) {
        modules[moduleName][funcName] = func;
        DEBUG_PRINT("Registered function: %s.%s", 
                    moduleName.c_str(), funcName.c_str());
    }
    
    NativeFunction getFunction(const std::string& moduleName,
                               const std::string& funcName) const {
        auto moduleIt = modules.find(moduleName);
        if (moduleIt == modules.end()) {
            throw RuntimeError("Module not found: " + moduleName);
        }
        
        auto funcIt = moduleIt->second.find(funcName);
        if (funcIt == moduleIt->second.end()) {
            throw RuntimeError("Function not found: " + moduleName + "." + funcName);
        }
        
        return funcIt->second;
    }
    
    bool hasModule(const std::string& moduleName) const {
        return modules.count(moduleName) > 0;
    }
    
    std::vector<std::string> getModuleNames() const {
        std::vector<std::string> names;
        for (const auto& [name, _] : modules) {
            names.push_back(name);
        }
        return names;
    }
    
    // Initialize all standard modules
    void initializeStandardModules() {
        DEBUG_PRINT("Initializing standard library modules...");
        
        initIOModule();
        initStringModule();
        initMathModule();
        initTimeModule();
        
        DEBUG_PRINT("Standard library initialized with %zu modules", modules.size());
    }
    
private:
    void initIOModule();
    void initStringModule();
    void initMathModule();
    void initTimeModule();
};

} // namespace Sad::StdLib
```

---

## 📁 الملفات المطلوبة / Required Files

```
include/stdlib/
├── core/
│   ├── stdlib_manager.h         ✅ UPDATE (from 09_integration)
│   └── native_function.h        🆕 NEW
├── io/
│   ├── io_module.h              🆕 NEW
│   ├── print.h                  🆕 NEW
│   ├── input.h                  🆕 NEW
│   └── file_io.h                🆕 NEW
├── string/
│   ├── string_module.h          🆕 NEW
│   ├── manipulation.h           🆕 NEW
│   └── regex.h                  🆕 NEW (optional)
├── math/
│   ├── math_module.h            🆕 NEW
│   ├── basic.h                  🆕 NEW (sqrt, pow, abs)
│   ├── trigonometry.h           🆕 NEW (sin, cos, tan)
│   ├── random.h                 🆕 NEW
│   └── statistics.h             🆕 NEW (optional)
├── time/
│   ├── time_module.h            🆕 NEW
│   ├── datetime.h               🆕 NEW
│   └── timing.h                 🆕 NEW (sleep, clock)
└── graphics/
    └── graphics_module.h         ⏳ See Plan 14

src/stdlib/
├── core/
│   ├── stdlib_manager.cpp       ✅ UPDATE
│   └── native_function.cpp      🆕 NEW
├── io/
│   ├── io_module.cpp            🆕 NEW
│   ├── print.cpp                🆕 NEW
│   ├── input.cpp                🆕 NEW
│   └── file_io.cpp              🆕 NEW
├── string/
│   ├── string_module.cpp        🆕 NEW
│   ├── manipulation.cpp         🆕 NEW
│   └── regex.cpp                🆕 NEW
├── math/
│   ├── math_module.cpp          🆕 NEW
│   ├── basic.cpp                🆕 NEW
│   ├── trigonometry.cpp         🆕 NEW
│   ├── random.cpp               🆕 NEW
│   └── statistics.cpp           🆕 NEW
└── time/
    ├── time_module.cpp          🆕 NEW
    ├── datetime.cpp             🆕 NEW
    └── timing.cpp               🆕 NEW

tests/stdlib_tests/
├── io_module_test.cpp           🆕 NEW (15 tests)
├── string_module_test.cpp       🆕 NEW (20 tests)
├── math_module_test.cpp         🆕 NEW (15 tests)
├── time_module_test.cpp         🆕 NEW (10 tests)
└── module_loading_test.cpp      🆕 NEW (8 tests)
```

---

## 🔧 تقسيم العمل / Task Breakdown

### Week 1: IO Module

#### Task 1.1: Print & Input (يوم 1-2)
**الملفات:**
- `include/stdlib/io/io_module.h`
- `src/stdlib/io/print.cpp`
- `src/stdlib/io/input.cpp`

**الوظائف:**
```cpp
// src/stdlib/io/print.cpp
namespace Sad::StdLib::IO {

// اطبع / print
Value print(const std::vector<Value>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::cout << args[i].toString();
        if (i < args.size() - 1) std::cout << " ";
    }
    std::cout << "\n";
    return Value(); // None
}

// اطبع_خطأ / print_error
Value printError(const std::vector<Value>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::cerr << args[i].toString();
        if (i < args.size() - 1) std::cerr << " ";
    }
    std::cerr << "\n";
    return Value();
}

// إقرأ / input
Value input(const std::vector<Value>& args) {
    // Print prompt if provided
    if (!args.empty()) {
        std::cout << args[0].toString();
    }
    
    std::string line;
    std::getline(std::cin, line);
    
    return Value(line);
}

// إقرأ_رقم / input_int
Value inputInt(const std::vector<Value>& args) {
    if (!args.empty()) {
        std::cout << args[0].toString();
    }
    
    int64_t value;
    std::cin >> value;
    std::cin.ignore(); // clear newline
    
    return Value(value);
}

} // namespace Sad::StdLib::IO
```

**التسجيل في StdLib Manager:**
```cpp
void StdLibManager::initIOModule() {
    registerFunction("io", "اطبع", IO::print);
    registerFunction("io", "print", IO::print);
    registerFunction("io", "اطبع_خطأ", IO::printError);
    registerFunction("io", "print_error", IO::printError);
    registerFunction("io", "إقرأ", IO::input);
    registerFunction("io", "input", IO::input);
    registerFunction("io", "إقرأ_رقم", IO::inputInt);
    registerFunction("io", "input_int", IO::inputInt);
    
    DEBUG_PRINT("IO module initialized with 8 functions");
}
```

**الاختبارات:**
```cpp
TEST(IOModuleTest, Print) {
    testing::internal::CaptureStdout();
    
    std::vector<Value> args = {Value("Hello"), Value(42)};
    IO::print(args);
    
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Hello 42\n");
}

TEST(IOModuleTest, Input) {
    // Mock stdin
    std::istringstream input("test input\n");
    std::cin.rdbuf(input.rdbuf());
    
    Value result = IO::input({});
    EXPECT_EQ(result.asString(), "test input");
}
```

**معايير القبول:**
- ✅ print() works with multiple args
- ✅ input() reads user input
- ✅ printError() writes to stderr
- ✅ Tests pass (≥10 tests)

---

#### Task 1.2: File I/O (يوم 3-4)
**الملفات:**
- `src/stdlib/io/file_io.cpp`

**الوظائف:**
```cpp
// إقرأ_ملف / read_file
Value readFile(const std::vector<Value>& args) {
    if (args.empty()) {
        throw RuntimeError("read_file requires a filename");
    }
    
    std::string filename = args[0].asString();
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw RuntimeError("Cannot open file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    DEBUG_PRINT("Read file: %s (%zu bytes)", 
                filename.c_str(), buffer.str().length());
    
    return Value(buffer.str());
}

// إكتب_ملف / write_file
Value writeFile(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw RuntimeError("write_file requires filename and content");
    }
    
    std::string filename = args[0].asString();
    std::string content = args[1].asString();
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw RuntimeError("Cannot write to file: " + filename);
    }
    
    file << content;
    
    DEBUG_PRINT("Wrote to file: %s (%zu bytes)", 
                filename.c_str(), content.length());
    
    return Value(); // None
}

// موجود_ملف / file_exists
Value fileExists(const std::vector<Value>& args) {
    if (args.empty()) {
        throw RuntimeError("file_exists requires a filename");
    }
    
    std::string filename = args[0].asString();
    std::ifstream file(filename);
    
    return Value(file.good());
}
```

**الاختبارات:**
```cpp
TEST(FileIOTest, ReadFile) {
    // Create test file
    std::ofstream out("test.txt");
    out << "test content";
    out.close();
    
    Value content = IO::readFile({Value("test.txt")});
    EXPECT_EQ(content.asString(), "test content");
    
    // Cleanup
    std::remove("test.txt");
}

TEST(FileIOTest, WriteFile) {
    IO::writeFile({Value("test_output.txt"), Value("Hello World")});
    
    std::ifstream in("test_output.txt");
    std::string content((std::istreambuf_iterator<char>(in)),
                        std::istreambuf_iterator<char>());
    
    EXPECT_EQ(content, "Hello World");
    
    std::remove("test_output.txt");
}
```

---

### Week 2: String & Math Modules

#### Task 2.1: String Module (يوم 5-7)
**الملفات:**
- `src/stdlib/string/string_module.cpp`
- `src/stdlib/string/manipulation.cpp`

**الوظائف:**
```cpp
// طول / length
Value length(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isString()) {
        throw RuntimeError("length requires a string argument");
    }
    
    return Value(static_cast<int64_t>(args[0].asString().length()));
}

// كبير / upper
Value upper(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isString()) {
        throw RuntimeError("upper requires a string argument");
    }
    
    std::string str = args[0].asString();
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    
    return Value(str);
}

// صغير / lower
Value lower(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isString()) {
        throw RuntimeError("lower requires a string argument");
    }
    
    std::string str = args[0].asString();
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    
    return Value(str);
}

// قسّم / split
Value split(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isString()) {
        throw RuntimeError("split requires a string argument");
    }
    
    std::string str = args[0].asString();
    std::string delimiter = args.size() > 1 ? args[1].asString() : " ";
    
    std::vector<Value> parts;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        parts.push_back(Value(str.substr(start, end - start)));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    parts.push_back(Value(str.substr(start)));
    
    return Value::makeArray(parts);
}

// إدمج / join
Value join(const std::vector<Value>& args) {
    if (args.size() < 2 || !args[0].isArray() || !args[1].isString()) {
        throw RuntimeError("join requires array and delimiter");
    }
    
    const auto& arr = args[0].asArray();
    std::string delimiter = args[1].asString();
    
    std::string result;
    for (size_t i = 0; i < arr.size(); ++i) {
        result += arr[i].toString();
        if (i < arr.size() - 1) result += delimiter;
    }
    
    return Value(result);
}

// يبدأ_بـ / starts_with
Value startsWith(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw RuntimeError("starts_with requires string and prefix");
    }
    
    std::string str = args[0].asString();
    std::string prefix = args[1].asString();
    
    return Value(str.find(prefix) == 0);
}

// ينتهي_بـ / ends_with
Value endsWith(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw RuntimeError("ends_with requires string and suffix");
    }
    
    std::string str = args[0].asString();
    std::string suffix = args[1].asString();
    
    if (str.length() < suffix.length()) return Value(false);
    
    return Value(str.compare(str.length() - suffix.length(), 
                            suffix.length(), suffix) == 0);
}
```

**التسجيل:**
```cpp
void StdLibManager::initStringModule() {
    registerFunction("string", "طول", String::length);
    registerFunction("string", "length", String::length);
    registerFunction("string", "كبير", String::upper);
    registerFunction("string", "upper", String::upper);
    registerFunction("string", "صغير", String::lower);
    registerFunction("string", "lower", String::lower);
    registerFunction("string", "قسّم", String::split);
    registerFunction("string", "split", String::split);
    registerFunction("string", "إدمج", String::join);
    registerFunction("string", "join", String::join);
    registerFunction("string", "يبدأ_بـ", String::startsWith);
    registerFunction("string", "starts_with", String::startsWith);
    registerFunction("string", "ينتهي_بـ", String::endsWith);
    registerFunction("string", "ends_with", String::endsWith);
    
    DEBUG_PRINT("String module initialized with 14 functions");
}
```

**الاختبارات:**
```cpp
TEST(StringModuleTest, Length) {
    Value result = String::length({Value("hello")});
    EXPECT_EQ(result.asInteger(), 5);
}

TEST(StringModuleTest, Upper) {
    Value result = String::upper({Value("hello")});
    EXPECT_EQ(result.asString(), "HELLO");
}

TEST(StringModuleTest, Split) {
    Value result = String::split({Value("a,b,c"), Value(",")});
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.asArray().size(), 3);
    EXPECT_EQ(result.asArray()[0].asString(), "a");
}

TEST(StringModuleTest, StartsWith) {
    Value result = String::startsWith({Value("hello"), Value("he")});
    EXPECT_TRUE(result.asBool());
}
```

---

#### Task 2.2: Math Module (يوم 8-10)
**الملفات:**
- `src/stdlib/math/math_module.cpp`
- `src/stdlib/math/basic.cpp`
- `src/stdlib/math/trigonometry.cpp`
- `src/stdlib/math/random.cpp`

**الوظائف:**
```cpp
#include <cmath>
#include <random>

namespace Sad::StdLib::Math {

// قيمة_مطلقة / abs
Value abs(const std::vector<Value>& args) {
    if (args.empty()) throw RuntimeError("abs requires an argument");
    
    if (args[0].isInteger()) {
        return Value(std::abs(args[0].asInteger()));
    } else if (args[0].isFloat()) {
        return Value(std::abs(args[0].asFloat()));
    }
    
    throw RuntimeError("abs requires numeric argument");
}

// جذر / sqrt
Value sqrt(const std::vector<Value>& args) {
    if (args.empty()) throw RuntimeError("sqrt requires an argument");
    
    double value = args[0].isFloat() ? args[0].asFloat() 
                                      : static_cast<double>(args[0].asInteger());
    
    if (value < 0) throw RuntimeError("sqrt of negative number");
    
    return Value(std::sqrt(value));
}

// قوة / pow
Value pow(const std::vector<Value>& args) {
    if (args.size() < 2) throw RuntimeError("pow requires two arguments");
    
    double base = args[0].isFloat() ? args[0].asFloat() 
                                     : static_cast<double>(args[0].asInteger());
    double exponent = args[1].isFloat() ? args[1].asFloat() 
                                         : static_cast<double>(args[1].asInteger());
    
    return Value(std::pow(base, exponent));
}

// جيب / sin
Value sin(const std::vector<Value>& args) {
    if (args.empty()) throw RuntimeError("sin requires an argument");
    
    double radians = args[0].isFloat() ? args[0].asFloat() 
                                        : static_cast<double>(args[0].asInteger());
    
    return Value(std::sin(radians));
}

// جيب_تمام / cos
Value cos(const std::vector<Value>& args) {
    if (args.empty()) throw RuntimeError("cos requires an argument");
    
    double radians = args[0].isFloat() ? args[0].asFloat() 
                                        : static_cast<double>(args[0].asInteger());
    
    return Value(std::cos(radians));
}

// عشوائي / random
Value random(const std::vector<Value>& args) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    if (args.empty()) {
        // Return float [0.0, 1.0)
        std::uniform_real_distribution<> dis(0.0, 1.0);
        return Value(dis(gen));
    } else if (args.size() == 1) {
        // Return int [0, max)
        int64_t max = args[0].asInteger();
        std::uniform_int_distribution<int64_t> dis(0, max - 1);
        return Value(dis(gen));
    } else {
        // Return int [min, max)
        int64_t min = args[0].asInteger();
        int64_t max = args[1].asInteger();
        std::uniform_int_distribution<int64_t> dis(min, max - 1);
        return Value(dis(gen));
    }
}

// الثابت π / PI
Value pi(const std::vector<Value>&) {
    return Value(M_PI);
}

// الثابت e / E
Value e(const std::vector<Value>&) {
    return Value(M_E);
}

} // namespace Sad::StdLib::Math
```

**التسجيل:**
```cpp
void StdLibManager::initMathModule() {
    registerFunction("math", "قيمة_مطلقة", Math::abs);
    registerFunction("math", "abs", Math::abs);
    registerFunction("math", "جذر", Math::sqrt);
    registerFunction("math", "sqrt", Math::sqrt);
    registerFunction("math", "قوة", Math::pow);
    registerFunction("math", "pow", Math::pow);
    registerFunction("math", "جيب", Math::sin);
    registerFunction("math", "sin", Math::sin);
    registerFunction("math", "جيب_تمام", Math::cos);
    registerFunction("math", "cos", Math::cos);
    registerFunction("math", "عشوائي", Math::random);
    registerFunction("math", "random", Math::random);
    registerFunction("math", "ط", Math::pi);
    registerFunction("math", "PI", Math::pi);
    registerFunction("math", "E", Math::e);
    
    DEBUG_PRINT("Math module initialized with 15 functions");
}
```

**الاختبارات:**
```cpp
TEST(MathModuleTest, Abs) {
    EXPECT_EQ(Math::abs({Value(-42)}).asInteger(), 42);
    EXPECT_DOUBLE_EQ(Math::abs({Value(-3.14)}).asFloat(), 3.14);
}

TEST(MathModuleTest, Sqrt) {
    EXPECT_DOUBLE_EQ(Math::sqrt({Value(16)}).asFloat(), 4.0);
    EXPECT_DOUBLE_EQ(Math::sqrt({Value(2.0)}).asFloat(), std::sqrt(2.0));
}

TEST(MathModuleTest, Pow) {
    EXPECT_DOUBLE_EQ(Math::pow({Value(2), Value(3)}).asFloat(), 8.0);
}

TEST(MathModuleTest, Random) {
    Value r = Math::random({});
    EXPECT_TRUE(r.isFloat());
    EXPECT_GE(r.asFloat(), 0.0);
    EXPECT_LT(r.asFloat(), 1.0);
}
```

---

### Week 3: Time Module & Integration

#### Task 3.1: Time Module (يوم 11-13)
**الملفات:**
- `src/stdlib/time/time_module.cpp`
- `src/stdlib/time/datetime.cpp`
- `src/stdlib/time/timing.cpp`

**الوظائف:**
```cpp
#include <chrono>
#include <thread>
#include <ctime>

namespace Sad::StdLib::Time {

// الوقت_الحالي / time
Value now(const std::vector<Value>&) {
    auto now = std::chrono::system_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()
    ).count();
    
    return Value(static_cast<int64_t>(seconds));
}

// نم / sleep
Value sleep(const std::vector<Value>& args) {
    if (args.empty()) throw RuntimeError("sleep requires duration argument");
    
    double seconds = args[0].isFloat() ? args[0].asFloat() 
                                        : static_cast<double>(args[0].asInteger());
    
    std::this_thread::sleep_for(
        std::chrono::milliseconds(static_cast<int>(seconds * 1000))
    );
    
    return Value(); // None
}

// ساعة / clock
Value clock(const std::vector<Value>&) {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    return Value(static_cast<double>(millis) / 1000.0);
}

// تاريخ / date
Value date(const std::vector<Value>&) {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm = std::localtime(&time);
    
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", tm);
    
    return Value(std::string(buffer));
}

// وقت / time_str
Value timeStr(const std::vector<Value>&) {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm = std::localtime(&time);
    
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", tm);
    
    return Value(std::string(buffer));
}

} // namespace Sad::StdLib::Time
```

**التسجيل:**
```cpp
void StdLibManager::initTimeModule() {
    registerFunction("time", "الوقت_الحالي", Time::now);
    registerFunction("time", "time", Time::now);
    registerFunction("time", "نم", Time::sleep);
    registerFunction("time", "sleep", Time::sleep);
    registerFunction("time", "ساعة", Time::clock);
    registerFunction("time", "clock", Time::clock);
    registerFunction("time", "تاريخ", Time::date);
    registerFunction("time", "date", Time::date);
    registerFunction("time", "وقت", Time::timeStr);
    registerFunction("time", "time_str", Time::timeStr);
    
    DEBUG_PRINT("Time module initialized with 10 functions");
}
```

**الاختبارات:**
```cpp
TEST(TimeModuleTest, Now) {
    Value t = Time::now({});
    EXPECT_TRUE(t.isInteger());
    EXPECT_GT(t.asInteger(), 0);
}

TEST(TimeModuleTest, Sleep) {
    auto start = std::chrono::high_resolution_clock::now();
    
    Time::sleep({Value(0.1)}); // 100ms
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    EXPECT_GE(duration, 90);  // at least 90ms
    EXPECT_LT(duration, 200); // less than 200ms
}

TEST(TimeModuleTest, Date) {
    Value d = Time::date({});
    EXPECT_TRUE(d.isString());
    EXPECT_EQ(d.asString().length(), 10); // YYYY-MM-DD
}
```

---

#### Task 3.2: Module Integration Test (يوم 14-15)
```cpp
TEST(StdLibIntegration, AllModulesRegistered) {
    StdLibManager manager;
    manager.initializeStandardModules();
    
    EXPECT_TRUE(manager.hasModule("io"));
    EXPECT_TRUE(manager.hasModule("string"));
    EXPECT_TRUE(manager.hasModule("math"));
    EXPECT_TRUE(manager.hasModule("time"));
}

TEST(StdLibIntegration, CompleteProgram) {
    std::string code = R"(
        استخدم io
        استخدم string
        استخدم math
        
        نص اسم = إقرأ("أدخل اسمك: ")
        نص كبير = string.كبير(اسم)
        
        رقم عشوائي = math.عشوائي(1، 100)
        رقم جذر = math.جذر(عشوائي)
        
        io.اطبع("مرحبا "، كبير)
        io.اطبع("الرقم العشوائي: "، عشوائي)
        io.اطبع("الجذر: "، جذر)
    )";
    
    // Execute and verify no errors
    auto result = executeCode(code);
    EXPECT_TRUE(result.success);
}
```

---

## 🧪 الاختبارات / Tests Summary

### Unit Tests (68+ tests)
- IO module (15 tests)
- String module (20 tests)
- Math module (15 tests)
- Time module (10 tests)
- Module loading (8 tests)

---

## ⏱️ الجدول الزمني / Timeline

| الأسبوع | المهام | الساعات |
|---------|--------|---------|
| Week 1 | IO Module (print, input, files) | 40 ساعة |
| Week 2 | String + Math Modules | 40 ساعة |
| Week 3 | Time Module + Integration | 40 ساعة |
| **Total** | **3 weeks** | **120 ساعة** |

---

## 🚀 أوامر PowerShell / PowerShell Commands

```powershell
# إنشاء فرع العمل
git checkout -b feat/stdlib/core-modules

# بناء واختبار
cd build
cmake --build . --config Debug
ctest -R stdlib_tests --output-on-failure -C Debug

# Commit
git add include/stdlib/* src/stdlib/* tests/stdlib_tests/*
git commit -m "[stdlib] Implemented core standard library modules

- IO module: print, input, file I/O (8 functions)
- String module: manipulation, split/join (14 functions)
- Math module: trig, random, basic math (15 functions)
- Time module: date, sleep, timing (10 functions)
- StdLib Manager with dynamic registration
- 70+ unit tests with ≥85% coverage"

git push --set-upstream origin feat/stdlib/core-modules
```

---

## ⚠️ المخاطر / Risks

| الخطر | التخفيف |
|-------|---------|
| File I/O security | Path validation + sandboxing |
| Random quality | استخدام std::mt19937 |
| Cross-platform time | استخدام chrono standard |

---

**آخر تحديث:** 4 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ
