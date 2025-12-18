# 📝 المرحلة 10: الأدوات والبنية التحتية
## Phase 10: Tools & Infrastructure

**المدة:** أسبوعان (10 أيام)  
**الأولوية:** 🟢 P3 - عادية  
**التبعيات:** جميع المراحل السابقة

---

## 🎯 الأهداف

1. ✅ LSP Server كامل
2. ✅ Interactive Debugger
3. ✅ Profiler
4. ✅ Package Manager
5. ✅ Testing Framework
6. ✅ Documentation Generator

---

## 📋 المهام

### المهمة 10.1: LSP Server كامل (3 أيام)

```typescript
// Language Server Protocol features

// 1. Auto-completion (✅ 40% done → 100%)
- Keywords
- Variables in scope
- Functions
- Class members
- Imports
- Snippets

// 2. Go to Definition (❌ → ✅)
function jumpToDefinition(position) {
    // Find symbol definition
    // Jump to file/line
}

// 3. Find References (❌ → ✅)
function findAllReferences(symbol) {
    // Search across project
    // Return all usages
}

// 4. Hover Information (❌ → ✅)
function getHoverInfo(position) {
    return {
        type: "int",
        documentation: "Variable x",
        signature: "var x: int = 42"
    }
}

// 5. Code Formatting (❌ → ✅)
function formatDocument(doc) {
    // Apply formatting rules
    // Return formatted text
}

// 6. Diagnostics (✅ 50% → 100%)
function getDiagnostics(doc) {
    // Syntax errors
    // Type errors
    // Lint warnings
}

// 7. Refactoring (❌ → ✅)
- Rename symbol
- Extract function
- Extract variable
- Inline variable

// 8. Code Actions (❌ → ✅)
- Quick fixes
- Add import
- Generate constructor
- Implement interface
```

**Implementation:**
```cpp
// tools/lsp/sad_lsp_server.h
class SadLSPServer {
    Workspace workspace;
    SymbolTable symbol_table;
    TypeChecker type_checker;
    
public:
    void initialize(InitializeParams params);
    
    // Text synchronization
    void onDidOpen(DidOpenTextDocumentParams params);
    void onDidChange(DidChangeTextDocumentParams params);
    void onDidSave(DidSaveTextDocumentParams params);
    
    // Language features
    CompletionList getCompletion(CompletionParams params);
    Location gotoDefinition(DefinitionParams params);
    std::vector<Location> findReferences(ReferenceParams params);
    Hover getHover(HoverParams params);
    std::vector<Diagnostic> getDiagnostics(string uri);
    std::vector<TextEdit> formatDocument(DocumentFormattingParams params);
    WorkspaceEdit renameSymbol(RenameParams params);
    std::vector<CodeAction> getCodeActions(CodeActionParams params);
};
```

### المهمة 10.2: Interactive Debugger (3 أيام)

```bash
# Command-line debugger
$ sad-debug program.s

(sad-dbg) break 15          # Set breakpoint at line 15
Breakpoint 1 at line 15

(sad-dbg) run               # Start execution
Breakpoint 1 hit at line 15

(sad-dbg) print x           # Inspect variable
x = 42 (int)

(sad-dbg) step              # Step into
Stopped at line 16

(sad-dbg) next              # Step over
Stopped at line 17

(sad-dbg) continue          # Continue execution
Program exited normally

(sad-dbg) backtrace         # Show stack trace
#0 factorial(n=5) at math.s:15
#1 main() at program.s:10

(sad-dbg) watch x           # Watch variable
Watchpoint 1: x

(sad-dbg) list              # Show source code
10:  function factorial(n) {
11:      if (n <= 1) return 1
12:      return n * factorial(n - 1)
13:  }
14:  
15:  var result = factorial(5)  <- breakpoint
16:  print(result)
```

**Implementation:**
```cpp
// tools/debugger/debugger.h
class Debugger {
    Interpreter* interpreter;
    std::vector<Breakpoint> breakpoints;
    std::vector<Watchpoint> watchpoints;
    CallStack call_stack;
    
public:
    void run();
    void setBreakpoint(string file, int line);
    void step();
    void stepOver();
    void stepOut();
    void continue_();
    
    Value inspect(string variable);
    void printStackTrace();
    void printLocalVariables();
    
    void watchVariable(string name);
    void showSourceContext(int lines);
};

struct Breakpoint {
    string file;
    int line;
    bool enabled;
    string condition;  // Optional
};

struct Watchpoint {
    string variable;
    Value last_value;
};
```

### المهمة 10.3: Profiler (2 أيام)

```bash
# Profile program
$ sad-profile program.s

Profiling Results:
==================

Functions by Time:
Function               Calls    Time (ms)    % Total
========================================================
fibonacci              1000     523.4        45.2%
search_binary          5000     312.1        26.9%
calculate_sum          10000    154.3        13.3%
process_data           500      89.2         7.7%
main                   1        78.0         6.9%

Memory Usage:
Function               Allocs   Bytes        Peak
========================================================
fibonacci              50000    1.2 MB       2.1 MB
search_binary          25000    512 KB       768 KB
calculate_sum          10000    256 KB       256 KB

Hotspots:
Line 15 in fibonacci: 45.2% of total time
Line 42 in search_binary: 26.9% of total time

Recommendations:
- Consider memoization for fibonacci
- Use iterative approach for search_binary
```

**Implementation:**
```cpp
// tools/profiler/profiler.h
class Profiler {
    struct FunctionStats {
        string name;
        int call_count;
        double total_time_ms;
        double average_time_ms;
        size_t memory_allocated;
        size_t peak_memory;
    };
    
    std::unordered_map<string, FunctionStats> function_stats;
    std::vector<LineProfile> hotspots;
    
public:
    void start();
    void stop();
    
    void onFunctionEnter(string name);
    void onFunctionExit(string name);
    void onAllocation(size_t bytes);
    
    void generateReport();
    void exportFlameGraph(string filename);
};
```

### المهمة 10.4: Package Manager (يوم واحد)

```bash
# Initialize project
$ sad-pkg init
Created sad-project.json

# Install package
$ sad-pkg install math-advanced
Installing math-advanced@1.2.0...
✓ Installed successfully

# Update packages
$ sad-pkg update
Checking for updates...
  math-advanced: 1.2.0 → 1.3.0
Update? [Y/n] y
✓ Updated

# Publish package
$ sad-pkg publish
Publishing my-package@1.0.0...
✓ Published to sad-packages.org

# Search packages
$ sad-pkg search graphics
Found 5 packages:
  - graphics-2d (Drawing and rendering)
  - graphics-3d (3D graphics engine)
  - ui-toolkit (User interface widgets)
```

**Project file (sad-project.json):**
```json
{
  "name": "my-project",
  "version": "1.0.0",
  "description": "My Sad project",
  "author": "Your Name",
  "license": "MIT",
  "dependencies": {
    "math-advanced": "^1.2.0",
    "http-client": "^2.0.0"
  },
  "devDependencies": {
    "test-framework": "^1.0.0"
  },
  "scripts": {
    "test": "sad test/**/*.s",
    "build": "sad-compile src/main.s",
    "run": "sad src/main.s"
  }
}
```

### المهمة 10.5: Testing Framework (يوم واحد)

```s
// tests/test_math.s
استورد اختبار

اختبار.describe("Math functions", function() {
    
    اختبار.it("should add two numbers", function() {
        var result = add(2, 3)
        اختبار.expect(result).to_equal(5)
    })
    
    اختبار.it("should multiply correctly", function() {
        اختبار.expect(multiply(3, 4)).to_equal(12)
    })
    
    اختبار.it("should handle negative numbers", function() {
        اختبار.expect(add(-5, 3)).to_equal(-2)
    })
})

اختبار.describe("Array operations", function() {
    
    اختبار.before_each(function() {
        // Setup before each test
        this.array = [1, 2, 3]
    })
    
    اختبار.it("should append element", function() {
        this.array.append(4)
        اختبار.expect(this.array.length()).to_equal(4)
    })
})
```

**Run tests:**
```bash
$ sad-test tests/

Running tests...

Math functions
  ✓ should add two numbers
  ✓ should multiply correctly
  ✓ should handle negative numbers

Array operations
  ✓ should append element

4 tests passed, 0 failed
Time: 23ms
```

### المهمة 10.6: Documentation Generator (يوم واحد)

```s
#**
 * حساب المجموع
 * Calculate sum of two numbers
 * 
 * @param a - العدد الأول / First number
 * @param b - العدد الثاني / Second number
 * @return المجموع / The sum
 * @example
 *   var result = add(2, 3)  // Returns 5
 **#
function add(a: int, b: int): int {
    return a + b
}
```

**Generate docs:**
```bash
$ sad-doc src/ --output docs/

Generating documentation...
  Processing src/math.s
  Processing src/string.s
  Processing src/array.s

✓ Documentation generated at docs/
```

**Output (HTML):**
```html
<!DOCTYPE html>
<html dir="rtl">
<head>
    <title>Sad Language Documentation</title>
</head>
<body>
    <h1>دوال الرياضيات / Math Functions</h1>
    
    <div class="function">
        <h2>add(a: int, b: int): int</h2>
        <p class="ar">حساب المجموع</p>
        <p class="en">Calculate sum of two numbers</p>
        
        <h3>Parameters:</h3>
        <ul>
            <li><code>a</code> - العدد الأول / First number</li>
            <li><code>b</code> - العدد الثاني / Second number</li>
        </ul>
        
        <h3>Example:</h3>
        <pre><code>var result = add(2, 3)  // Returns 5</code></pre>
    </div>
</body>
</html>
```

---

## 📅 الجدول الزمني

| اليوم | المهمة | الساعات |
|------|---------|----------|
| 1-3 | LSP Server complete | 24 |
| 4-6 | Interactive debugger | 24 |
| 7-8 | Profiler | 16 |
| 9 | Package manager | 8 |
| 10 | Testing + Docgen | 8 |

**المجموع:** 80 ساعة

---

## 🎯 النتائج المتوقعة

- ✅ تجربة تطوير احترافية
- ✅ LSP كامل في VS Code
- ✅ Debugger تفاعلي
- ✅ Profiler للأداء
- ✅ Package manager functional
- ✅ Testing framework
- ✅ Auto documentation

---

**الحالة:** 📝 جاهز للتنفيذ
