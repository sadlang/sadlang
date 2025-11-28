# 🔧 API Reference - Sad Programming Language
# مرجع API - لغة ص

**Technical documentation for Sad Language interpreter components 🏗️**

---

## 📑 Table of Contents

1. [Overview](#overview)
2. [Lexer API](#lexer-api)
3. [Parser API](#parser-api)
4. [Interpreter API](#interpreter-api)
5. [AST Nodes](#ast-nodes)
6. [Built-in Functions](#built-in-functions)
7. [Error System](#error-system)
8. [Integration Guide](#integration-guide)

---

## 🌟 Overview

The Sad Programming Language interpreter is built with a modular architecture consisting of three main components:

### Architecture Flow
```
Source Code (.s) → Lexer → Tokens → Parser → AST → Interpreter → Execution
```

### Component Responsibilities
- **Lexer**: Tokenizes Arabic source code into meaningful tokens
- **Parser**: Builds Abstract Syntax Tree (AST) from tokens
- **Interpreter**: Executes the AST and manages program state

---

## 🔤 Lexer API

### Class: `Lexer`

The Lexer component handles tokenization of Sad language source code.

#### Constructor
```cpp
Lexer::Lexer(const std::string& source)
```
- **Parameters**: `source` - The Sad language source code as UTF-8 string
- **Purpose**: Initializes the lexer with source code

#### Methods

##### `std::vector<Token> tokenize()`
```cpp
std::vector<Token> tokenize()
```
- **Returns**: Vector of tokens representing the source code
- **Throws**: `LexerException` for invalid syntax
- **Purpose**: Converts source code into tokens

##### `Token getNextToken()`
```cpp
Token getNextToken()
```
- **Returns**: The next token in the stream
- **Purpose**: Sequential token access for parser

#### Token Types

| Token Type | Arabic Keyword | Description |
|------------|----------------|-------------|
| `NUMBER` | - | Integer literals (`123`, `-45`) |
| `FLOAT` | - | Floating-point literals (`3.14`, `-2.5e10`) |
| `STRING` | - | String literals (`"مرحبا"`, `"Hello"`) |
| `IDENTIFIER` | - | Variable/function names (`العمر`, `age`) |
| `INT_TYPE` | `رقم` | Integer type declaration |
| `FLOAT_TYPE` | `عشري` | Float type declaration |
| `STRING_TYPE` | `نص` | String type declaration |
| `BOOL_TYPE` | `منطق` | Boolean type declaration |
| `ARRAY_TYPE` | `مصفوفة` | Array type declaration |
| `TRUE` | `صحيح` | Boolean true value |
| `FALSE` | `خطأ` | Boolean false value |
| `IF` | `إذا` | If statement |
| `ELSE` | `وإلا` | Else statement |
| `WHILE` | `بينما` | While loop |
| `FUNCTION` | `دالة` | Function declaration |
| `RETURN` | `أرجع` | Return statement |
| `PRINT` | `اطبع` | Print function |
| `CLASS` | `صنف` | Class declaration |
| `NEW` | `جديد` | Object instantiation |
| `TRY` | `حاول` | Try block |
| `CATCH` | `امسك` | Catch block |
| `FINALLY` | `أخيراً` | Finally block |
| `THROW` | `ارمي` | Throw statement |
| `END` | `نهاية` | Block terminator |

#### Usage Example
```cpp
#include "lexer.h"

// Tokenize Sad code
std::string sadCode = "رقم العمر = 25 ; اطبع(العمر) ;";
Lexer lexer(sadCode);
std::vector<Token> tokens = lexer.tokenize();

// Process tokens
for (const auto& token : tokens) {
    std::cout << "Type: " << token.type 
              << ", Value: " << token.value 
              << ", Line: " << token.line << std::endl;
}
```

---

## 🌳 Parser API

### Class: `Parser`

The Parser constructs an Abstract Syntax Tree (AST) from tokens.

#### Constructor
```cpp
Parser::Parser(const std::vector<Token>& tokens)
```
- **Parameters**: `tokens` - Vector of tokens from lexer
- **Purpose**: Initializes parser with token stream

#### Methods

##### `std::unique_ptr<ASTNode> parse()`
```cpp
std::unique_ptr<ASTNode> parse()
```
- **Returns**: Root node of the AST
- **Throws**: `ParseException` for syntax errors
- **Purpose**: Parses tokens into AST

##### `std::unique_ptr<ASTNode> parseStatement()`
```cpp
std::unique_ptr<ASTNode> parseStatement()
```
- **Returns**: Single statement AST node
- **Purpose**: Parse individual statements

##### `std::unique_ptr<ASTNode> parseExpression()`
```cpp
std::unique_ptr<ASTNode> parseExpression()
```
- **Returns**: Expression AST node
- **Purpose**: Parse expressions with operator precedence

#### Grammar Rules

The parser implements the following grammar (simplified):

```ebnf
program         := statement_list
statement_list  := statement*
statement       := var_decl | assignment | if_stmt | while_stmt | 
                  function_decl | class_decl | return_stmt | 
                  print_stmt | expression_stmt
var_decl        := type IDENTIFIER ("=" expression)? ";"
assignment      := IDENTIFIER "=" expression ";"
if_stmt         := IF "(" expression ")" statement_list 
                  (ELSE statement_list)? END
while_stmt      := WHILE "(" expression ")" statement_list END
function_decl   := FUNCTION IDENTIFIER "(" param_list? ")" 
                  statement_list END
expression      := logical_or
logical_or      := logical_and (("||") logical_and)*
logical_and     := equality (("&&") equality)*
equality        := comparison (("==" | "!=") comparison)*
comparison      := term ((">" | ">=" | "<" | "<=") term)*
term            := factor (("+" | "-") factor)*
factor          := unary (("*" | "/" | "%") unary)*
unary           := ("!" | "-") unary | primary
primary         := NUMBER | FLOAT | STRING | TRUE | FALSE | 
                  IDENTIFIER | "(" expression ")" | array_literal
```

#### Usage Example
```cpp
#include "parser.h"

// Parse tokens into AST
std::vector<Token> tokens = /* from lexer */;
Parser parser(tokens);
std::unique_ptr<ASTNode> ast = parser.parse();

// Traverse AST (visitor pattern)
ASTVisitor visitor;
ast->accept(visitor);
```

---

## 🚀 Interpreter API

### Class: `Interpreter`

The Interpreter executes the AST and manages program state.

#### Constructor
```cpp
Interpreter::Interpreter()
```
- **Purpose**: Initializes interpreter with built-in functions

#### Methods

##### `Value execute(ASTNode* node)`
```cpp
Value execute(ASTNode* node)
```
- **Parameters**: `node` - AST node to execute
- **Returns**: Execution result value
- **Throws**: `RuntimeException` for runtime errors
- **Purpose**: Execute AST nodes

##### `void defineVariable(const std::string& name, const Value& value)`
```cpp
void defineVariable(const std::string& name, const Value& value)
```
- **Parameters**: 
  - `name` - Variable name
  - `value` - Variable value
- **Purpose**: Define new variable in current scope

##### `Value getVariable(const std::string& name)`
```cpp
Value getVariable(const std::string& name)
```
- **Parameters**: `name` - Variable name
- **Returns**: Variable value
- **Throws**: `UndefinedVariableException`
- **Purpose**: Get variable value from scope chain

##### `void pushScope()`
```cpp
void pushScope()
```
- **Purpose**: Create new scope (function call, block)

##### `void popScope()`
```cpp
void popScope()
```
- **Purpose**: Exit current scope

#### Value System

The interpreter uses a dynamic value system:

```cpp
class Value {
public:
    enum Type {
        NIL,
        BOOLEAN,
        INTEGER,
        FLOAT,
        STRING,
        ARRAY,
        OBJECT
    };
    
    Value();                          // Nil value
    Value(bool boolean);              // Boolean value
    Value(int integer);               // Integer value
    Value(double floating);           // Float value
    Value(const std::string& string); // String value
    Value(const std::vector<Value>& array); // Array value
    
    Type getType() const;
    bool isTruthy() const;
    std::string toString() const;
    
    // Type-specific getters
    bool asBoolean() const;
    int asInteger() const;
    double asFloat() const;
    std::string asString() const;
    std::vector<Value> asArray() const;
};
```

#### Usage Example
```cpp
#include "interpreter.h"

// Execute Sad program
std::unique_ptr<ASTNode> ast = /* from parser */;
Interpreter interpreter;

try {
    Value result = interpreter.execute(ast.get());
    std::cout << "Program result: " << result.toString() << std::endl;
} catch (const RuntimeException& e) {
    std::cerr << "Runtime error: " << e.what() << std::endl;
}
```

---

## 🌿 AST Nodes

### Base Class: `ASTNode`

All AST nodes inherit from the base `ASTNode` class:

```cpp
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual Value accept(Interpreter* interpreter) = 0;
    virtual std::string toString() const = 0;
};
```

### Node Types

#### `ProgramNode`
```cpp
class ProgramNode : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
public:
    ProgramNode(std::vector<std::unique_ptr<ASTNode>> statements);
    Value accept(Interpreter* interpreter) override;
};
```

#### `VariableDeclarationNode`
```cpp
class VariableDeclarationNode : public ASTNode {
    std::string type;
    std::string name;
    std::unique_ptr<ASTNode> initializer;
public:
    VariableDeclarationNode(std::string type, std::string name, 
                           std::unique_ptr<ASTNode> initializer = nullptr);
    Value accept(Interpreter* interpreter) override;
};
```

#### `AssignmentNode`
```cpp
class AssignmentNode : public ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> value;
public:
    AssignmentNode(std::string name, std::unique_ptr<ASTNode> value);
    Value accept(Interpreter* interpreter) override;
};
```

#### `BinaryOpNode`
```cpp
class BinaryOpNode : public ASTNode {
    std::unique_ptr<ASTNode> left;
    TokenType operator_;
    std::unique_ptr<ASTNode> right;
public:
    BinaryOpNode(std::unique_ptr<ASTNode> left, TokenType op, 
                std::unique_ptr<ASTNode> right);
    Value accept(Interpreter* interpreter) override;
};
```

#### `IfStatementNode`
```cpp
class IfStatementNode : public ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> then_statements;
    std::vector<std::unique_ptr<ASTNode>> else_statements;
public:
    IfStatementNode(std::unique_ptr<ASTNode> condition,
                   std::vector<std::unique_ptr<ASTNode>> then_stmts,
                   std::vector<std::unique_ptr<ASTNode>> else_stmts = {});
    Value accept(Interpreter* interpreter) override;
};
```

#### `WhileStatementNode`
```cpp
class WhileStatementNode : public ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> body;
public:
    WhileStatementNode(std::unique_ptr<ASTNode> condition,
                      std::vector<std::unique_ptr<ASTNode>> body);
    Value accept(Interpreter* interpreter) override;
};
```

#### `FunctionDeclarationNode`
```cpp
class FunctionDeclarationNode : public ASTNode {
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters; // (type, name)
    std::vector<std::unique_ptr<ASTNode>> body;
public:
    FunctionDeclarationNode(std::string name,
                           std::vector<std::pair<std::string, std::string>> params,
                           std::vector<std::unique_ptr<ASTNode>> body);
    Value accept(Interpreter* interpreter) override;
};
```

#### `FunctionCallNode`
```cpp
class FunctionCallNode : public ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> arguments;
public:
    FunctionCallNode(std::string name,
                    std::vector<std::unique_ptr<ASTNode>> args);
    Value accept(Interpreter* interpreter) override;
};
```

#### `ClassDeclarationNode`
```cpp
class ClassDeclarationNode : public ASTNode {
    std::string name;
    std::string superclass;
    std::vector<std::unique_ptr<ASTNode>> methods;
public:
    ClassDeclarationNode(std::string name, std::string super,
                        std::vector<std::unique_ptr<ASTNode>> methods);
    Value accept(Interpreter* interpreter) override;
};
```

---

## 📚 Built-in Functions

### Core Functions

#### `اطبع` (Print)
```cpp
Value builtin_print(const std::vector<Value>& args)
```
- **Parameters**: Variable number of values to print
- **Returns**: Nil
- **Purpose**: Print values to stdout with Arabic formatting

#### `طول` (Length)
```cpp
Value builtin_length(const std::vector<Value>& args)
```
- **Parameters**: Array or string value
- **Returns**: Integer length
- **Purpose**: Get length of arrays and strings

#### `نوع` (Type)
```cpp
Value builtin_type(const std::vector<Value>& args)
```
- **Parameters**: Any value
- **Returns**: String type name in Arabic
- **Purpose**: Get runtime type information

### Math Functions

#### `مطلق` (Absolute)
```cpp
Value builtin_abs(const std::vector<Value>& args)
```
- **Parameters**: Numeric value
- **Returns**: Absolute value
- **Purpose**: Calculate absolute value

#### `جذر` (Square Root)
```cpp
Value builtin_sqrt(const std::vector<Value>& args)
```
- **Parameters**: Positive numeric value
- **Returns**: Square root
- **Purpose**: Calculate square root

#### `قوة` (Power)
```cpp
Value builtin_pow(const std::vector<Value>& args)
```
- **Parameters**: Base and exponent values
- **Returns**: Base raised to exponent
- **Purpose**: Calculate power

### String Functions

#### `طول_نص` (String Length)
```cpp
Value builtin_string_length(const std::vector<Value>& args)
```
- **Parameters**: String value
- **Returns**: String length in characters
- **Purpose**: Get UTF-8 string length

#### `جزء_نص` (Substring)
```cpp
Value builtin_substring(const std::vector<Value>& args)
```
- **Parameters**: String, start index, length
- **Returns**: Substring
- **Purpose**: Extract substring

### Array Functions

#### `أضف_عنصر` (Add Element)
```cpp
Value builtin_push(const std::vector<Value>& args)
```
- **Parameters**: Array, element to add
- **Returns**: Modified array
- **Purpose**: Add element to array

#### `احذف_عنصر` (Remove Element)
```cpp
Value builtin_pop(const std::vector<Value>& args)
```
- **Parameters**: Array
- **Returns**: Removed element
- **Purpose**: Remove and return last element

### Type Conversion Functions

#### `إلى_رقم` (To Integer)
```cpp
Value builtin_to_int(const std::vector<Value>& args)
```
- **Parameters**: Value to convert
- **Returns**: Integer value
- **Purpose**: Convert to integer

#### `إلى_نص` (To String)
```cpp
Value builtin_to_string(const std::vector<Value>& args)
```
- **Parameters**: Value to convert
- **Returns**: String representation
- **Purpose**: Convert to string

---

## 🚨 Error System

### Exception Hierarchy

```cpp
class SadException : public std::exception {
    std::string message;
    int line;
public:
    SadException(const std::string& msg, int line = -1);
    const char* what() const noexcept override;
    int getLine() const;
};

class LexerException : public SadException {
public:
    LexerException(const std::string& msg, int line);
};

class ParseException : public SadException {
public:
    ParseException(const std::string& msg, int line);
};

class RuntimeException : public SadException {
public:
    RuntimeException(const std::string& msg, int line = -1);
};

class UndefinedVariableException : public RuntimeException {
public:
    UndefinedVariableException(const std::string& name, int line = -1);
};

class TypeException : public RuntimeException {
public:
    TypeException(const std::string& expected, const std::string& actual, int line = -1);
};

class DivisionByZeroException : public RuntimeException {
public:
    DivisionByZeroException(int line = -1);
};

class IndexOutOfBoundsException : public RuntimeException {
public:
    IndexOutOfBoundsException(int index, int size, int line = -1);
};
```

### Error Handling Strategy

1. **Lexical Errors**: Invalid characters, unclosed strings
2. **Syntax Errors**: Malformed expressions, missing tokens
3. **Runtime Errors**: Type mismatches, undefined variables
4. **Logic Errors**: Division by zero, array bounds

### Usage Example
```cpp
try {
    // Lexer phase
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    // Parser phase
    Parser parser(tokens);
    auto ast = parser.parse();
    
    // Interpreter phase
    Interpreter interpreter;
    auto result = interpreter.execute(ast.get());
    
} catch (const LexerException& e) {
    std::cerr << "Lexical error on line " << e.getLine() << ": " << e.what() << std::endl;
} catch (const ParseException& e) {
    std::cerr << "Syntax error on line " << e.getLine() << ": " << e.what() << std::endl;
} catch (const RuntimeException& e) {
    std::cerr << "Runtime error: " << e.what() << std::endl;
} catch (const SadException& e) {
    std::cerr << "General error: " << e.what() << std::endl;
}
```

---

## 🔗 Integration Guide

### Basic Integration

```cpp
#include "sad_interpreter.h"

class SadInterpreter {
private:
    std::unique_ptr<Lexer> lexer;
    std::unique_ptr<Parser> parser;
    std::unique_ptr<Interpreter> interpreter;
    
public:
    SadInterpreter() : interpreter(std::make_unique<Interpreter>()) {}
    
    Value executeCode(const std::string& source) {
        // Tokenize
        lexer = std::make_unique<Lexer>(source);
        auto tokens = lexer->tokenize();
        
        // Parse
        parser = std::make_unique<Parser>(tokens);
        auto ast = parser->parse();
        
        // Execute
        return interpreter->execute(ast.get());
    }
    
    Value executeFile(const std::string& filename) {
        std::ifstream file(filename);
        std::string source((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        return executeCode(source);
    }
};
```

### REPL Implementation

```cpp
class SadREPL {
private:
    SadInterpreter interpreter;
    
public:
    void run() {
        std::string line;
        std::cout << "لغة ص - الوضع التفاعلي" << std::endl;
        std::cout << "Sad Language - Interactive Mode" << std::endl;
        
        while (true) {
            std::cout << ">> ";
            if (!std::getline(std::cin, line)) break;
            
            if (line == "خروج" || line == "exit") break;
            
            try {
                Value result = interpreter.executeCode(line + ";");
                if (result.getType() != Value::NIL) {
                    std::cout << result.toString() << std::endl;
                }
            } catch (const SadException& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }
};
```

### Custom Built-in Functions

```cpp
class CustomInterpreter : public Interpreter {
public:
    CustomInterpreter() : Interpreter() {
        // Add custom built-in functions
        defineBuiltin("وقت_الآن", [](const std::vector<Value>& args) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            return Value(std::to_string(time_t));
        });
        
        defineBuiltin("عشوائي", [](const std::vector<Value>& args) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_real_distribution<> dis(0.0, 1.0);
            return Value(dis(gen));
        });
    }
};
```

### Performance Optimization

1. **Token Caching**: Cache frequently used tokens
2. **AST Optimization**: Constant folding, dead code elimination
3. **Scope Optimization**: Efficient variable lookup
4. **Memory Management**: Smart pointers, object pooling

### Thread Safety

The interpreter is **not thread-safe** by default. For multi-threaded usage:

```cpp
class ThreadSafeInterpreter {
private:
    std::mutex interpreter_mutex;
    std::unique_ptr<SadInterpreter> interpreter;
    
public:
    ThreadSafeInterpreter() : interpreter(std::make_unique<SadInterpreter>()) {}
    
    Value executeCode(const std::string& source) {
        std::lock_guard<std::mutex> lock(interpreter_mutex);
        return interpreter->executeCode(source);
    }
};
```

---

## 📊 API Summary

| Component | Primary Classes | Key Methods | Purpose |
|-----------|----------------|-------------|----------|
| **Lexer** | `Lexer` | `tokenize()`, `getNextToken()` | Source → Tokens |
| **Parser** | `Parser` | `parse()`, `parseStatement()` | Tokens → AST |
| **Interpreter** | `Interpreter` | `execute()`, `defineVariable()` | AST → Execution |
| **AST** | Various `*Node` | `accept()`, `toString()` | Tree representation |
| **Value** | `Value` | Type conversions, operations | Runtime values |
| **Errors** | Various `*Exception` | Error reporting | Exception handling |

---

**🏗️ This API documentation provides the foundation for extending and integrating with Sad Language**

For implementation examples and extended usage, see the [User Guide](USER_GUIDE_EN.md) and [source code examples](../examples/).

---

*Last updated: November 28, 2025*  
*Sad Language - Version 1.0*