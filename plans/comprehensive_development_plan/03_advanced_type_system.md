# 📝 المرحلة 3: نظام الأنواع المتقدم
## Phase 3: Advanced Type System

**المدة:** أسبوعان (10 أيام عمل)  
**الأولوية:** 🔴 P0 - حرجة  
**التبعيات:** المرحلة 1، 2  
**التأثير:** Type safety + Performance

---

## 🎯 الأهداف

### الهدف الرئيسي
بناء نظام أنواع متقدم حسب `rules/rules/01_types.md` و `06_cpp_features.md`.

### الأهداف الفرعية
1. ✅ Union Types (رقم | نص)
2. ✅ Optional Types (رقم?)
3. ✅ Generic Types (مصفوفة<T>)
4. ✅ Type Inference محسّن
5. ✅ Static Type Checker
6. ✅ Type Aliases
7. ✅ Intersection Types
8. ✅ Type Guards

---

## 📋 المهام التفصيلية

### المهمة 3.1: Union Types (يومان)

```cpp
// Syntax
var value: int | string = 42
value = "hello"  // OK

var قيمة: رقم | نص = 42
قيمة = "مرحبا"  // OK

// In function parameters
function process(data: int | string | bool) {
    // Type narrowing required
    if (typeof data == "int") {
        return data * 2
    } else if (typeof data == "string") {
        return data.toUpper()
    }
}

// Implementation
namespace Type {

class UnionType : public TypeInfo {
public:
    std::vector<TypeInfo*> types;
    
    UnionType(std::vector<TypeInfo*> t) : types(std::move(t)) {}
    
    bool matches(TypeInfo* other) const override {
        // Check if other matches any of the union members
        for (auto* type : types) {
            if (type->matches(other)) {
                return true;
            }
        }
        return false;
    }
    
    bool isAssignableFrom(TypeInfo* other) const override {
        // Can assign if other is one of the union members
        for (auto* type : types) {
            if (type->isAssignableFrom(other)) {
                return true;
            }
        }
        return false;
    }
    
    std::string toString() const override {
        std::string result;
        for (size_t i = 0; i < types.size(); i++) {
            if (i > 0) result += " | ";
            result += types[i]->toString();
        }
        return result;
    }
};

} // namespace Type

// Parser
std::unique_ptr<Type::TypeInfo> ParserCore::parseType() {
    auto type = parseBasicType();
    
    // Check for union: |
    if (match(TT::PIPE)) {
        std::vector<Type::TypeInfo*> types;
        types.push_back(type.release());
        
        do {
            types.push_back(parseBasicType().release());
        } while (match(TT::PIPE));
        
        return std::make_unique<Type::UnionType>(std::move(types));
    }
    
    return type;
}

// Type Checker
bool TypeChecker::checkAssignment(TypeInfo* target, TypeInfo* source) {
    // Union type handling
    if (auto* union_type = dynamic_cast<UnionType*>(target)) {
        return union_type->isAssignableFrom(source);
    }
    
    // ... other type checks
}
```

---

### المهمة 3.2: Optional Types (يوم)

```cpp
// Syntax
var name: string? = null
name = "Ahmed"  // OK

var age: int? = 25
age = null  // OK

// Function with optional return
function findUser(id: int): User? {
    if (id > 0) {
        return User(id)
    }
    return null
}

// Safe navigation operator
var user_name = user?.name  // null if user is null

// Implementation
class OptionalType : public TypeInfo {
public:
    TypeInfo* inner_type;
    
    OptionalType(TypeInfo* inner) : inner_type(inner) {}
    
    bool matches(TypeInfo* other) const override {
        if (other->isNull()) return true;
        return inner_type->matches(other);
    }
    
    bool isOptional() const override { return true; }
    
    std::string toString() const override {
        return inner_type->toString() + "?";
    }
};

// Parser
std::unique_ptr<Type::TypeInfo> ParserCore::parseType() {
    auto type = parseUnionType();
    
    // Optional: type?
    if (match(TT::QUESTION)) {
        return std::make_unique<Type::OptionalType>(type.release());
    }
    
    return type;
}

// Safe navigation operator: ?.
std::unique_ptr<AST::Expr> ParserCore::parseMemberAccess() {
    auto object = parsePrimary();
    
    while (true) {
        // Safe navigation: ?.
        if (match(TT::QUESTION_DOT)) {
            std::string member = consume(TT::IDENTIFIER, 
                "Expected member name").value;
            
            object = std::make_unique<AST::SafeMemberAccess>(
                std::move(object),
                member
            );
        }
        // Regular access: .
        else if (match(TT::DOT)) {
            std::string member = consume(TT::IDENTIFIER,
                "Expected member name").value;
            
            object = std::make_unique<AST::MemberAccess>(
                std::move(object),
                member
            );
        }
        else {
            break;
        }
    }
    
    return object;
}
```

---

### المهمة 3.3: Generic Types (3 أيام)

```cpp
// Syntax
var numbers: Array<int>
var names: Array<string>
var map: Map<string, int>

// Generic function
function identity<T>(value: T): T {
    return value
}

var x = identity<int>(42)
var y = identity<string>("hello")

// Generic class
class Box<T> {
    private value: T
    
    constructor(v: T) {
        this.value = v
    }
    
    function get(): T {
        return this.value
    }
}

var int_box = Box<int>(42)
var str_box = Box<string>("hello")

// Implementation
class GenericType : public TypeInfo {
public:
    std::string base_name;  // "Array", "Map"
    std::vector<TypeInfo*> type_parameters;  // <int>, <string, int>
    
    GenericType(std::string name, std::vector<TypeInfo*> params)
        : base_name(std::move(name)),
          type_parameters(std::move(params)) {}
    
    bool matches(TypeInfo* other) const override {
        auto* other_generic = dynamic_cast<GenericType*>(other);
        if (!other_generic) return false;
        
        if (base_name != other_generic->base_name) return false;
        
        if (type_parameters.size() != other_generic->type_parameters.size()) {
            return false;
        }
        
        for (size_t i = 0; i < type_parameters.size(); i++) {
            if (!type_parameters[i]->matches(other_generic->type_parameters[i])) {
                return false;
            }
        }
        
        return true;
    }
    
    std::string toString() const override {
        std::string result = base_name + "<";
        for (size_t i = 0; i < type_parameters.size(); i++) {
            if (i > 0) result += ", ";
            result += type_parameters[i]->toString();
        }
        result += ">";
        return result;
    }
};

class TypeParameter {
public:
    std::string name;  // T, U, V
    TypeInfo* constraint = nullptr;  // T: Comparable
    
    TypeParameter(std::string n) : name(std::move(n)) {}
};

// Generic function declaration
class GenericFunctionDecl : public FunctionDecl {
public:
    std::vector<TypeParameter> type_parameters;
    
    // Instantiation cache
    std::unordered_map<std::string, FunctionDecl*> instantiations;
};

// Parser
std::unique_ptr<Type::TypeInfo> ParserCore::parseType() {
    std::string type_name = consume(TT::IDENTIFIER, "Expected type name").value;
    
    // Generic type parameters: <T>
    if (match(TT::LESS)) {
        std::vector<Type::TypeInfo*> params;
        
        do {
            params.push_back(parseType().release());
        } while (match(TT::COMMA));
        
        consume(TT::GREATER, "Expected '>' after type parameters");
        
        return std::make_unique<Type::GenericType>(
            type_name,
            std::move(params)
        );
    }
    
    return resolveTypeName(type_name);
}

std::unique_ptr<AST::FunctionDecl> ParserCore::parseGenericFunction() {
    consume(TT::KEYWORD_FUNCTION, "Expected 'function'");
    
    std::string name = consume(TT::IDENTIFIER, "Expected function name").value;
    
    // Type parameters: <T, U>
    std::vector<Type::TypeParameter> type_params;
    
    if (match(TT::LESS)) {
        do {
            std::string param_name = consume(TT::IDENTIFIER,
                "Expected type parameter name").value;
            
            Type::TypeParameter param(param_name);
            
            // Optional constraint: T: Comparable
            if (match(TT::COLON)) {
                param.constraint = parseType().release();
            }
            
            type_params.push_back(std::move(param));
        } while (match(TT::COMMA));
        
        consume(TT::GREATER, "Expected '>' after type parameters");
    }
    
    // Regular function parsing
    auto func = parseFunctionBody();
    
    if (!type_params.empty()) {
        auto generic_func = std::make_unique<AST::GenericFunctionDecl>();
        generic_func->type_parameters = std::move(type_params);
        // Copy other fields from func
        return generic_func;
    }
    
    return func;
}
```

---

### المهمة 3.4: Type Inference (يومان)

```cpp
// Type inference
var x = 42               // inferred: int
var y = 3.14             // inferred: float
var name = "Ahmed"       // inferred: string
var list = [1, 2, 3]     // inferred: Array<int>

// In functions
function add(a, b) {
    return a + b
}
// Inferred: function add(a: int, b: int): int
// OR: function add(a: float, b: float): float
// Depends on usage

// Implementation
class TypeInference {
public:
    std::unordered_map<std::string, Type::TypeInfo*> inferred_types;
    
    Type::TypeInfo* inferType(AST::Expr* expr) {
        // Literals
        if (auto* lit = dynamic_cast<AST::LiteralExpr*>(expr)) {
            if (lit->value.isInt()) {
                return Type::IntType::instance();
            }
            if (lit->value.isFloat()) {
                return Type::FloatType::instance();
            }
            if (lit->value.isString()) {
                return Type::StringType::instance();
            }
            if (lit->value.isBool()) {
                return Type::BoolType::instance();
            }
        }
        
        // Array literal
        if (auto* arr = dynamic_cast<AST::ArrayLiteral*>(expr)) {
            if (arr->elements.empty()) {
                return new Type::GenericType("Array", {Type::AnyType::instance()});
            }
            
            // Infer from first element
            auto* element_type = inferType(arr->elements[0].get());
            
            // Check all elements have same type
            for (size_t i = 1; i < arr->elements.size(); i++) {
                auto* elem_type = inferType(arr->elements[i].get());
                if (!element_type->matches(elem_type)) {
                    // Mixed types, use union
                    // ...
                }
            }
            
            return new Type::GenericType("Array", {element_type});
        }
        
        // Binary operations
        if (auto* bin = dynamic_cast<AST::BinaryExpr*>(expr)) {
            auto* left_type = inferType(bin->left.get());
            auto* right_type = inferType(bin->right.get());
            
            // Arithmetic operators
            if (bin->op == TT::PLUS || bin->op == TT::MINUS ||
                bin->op == TT::MULTIPLY || bin->op == TT::DIVIDE) {
                
                // int + int = int
                if (left_type->isInt() && right_type->isInt()) {
                    return Type::IntType::instance();
                }
                
                // float + anything = float
                if (left_type->isFloat() || right_type->isFloat()) {
                    return Type::FloatType::instance();
                }
                
                // string + string = string
                if (left_type->isString() && right_type->isString()) {
                    return Type::StringType::instance();
                }
            }
            
            // Comparison operators
            if (bin->op == TT::EQUAL_EQUAL || bin->op == TT::NOT_EQUAL ||
                bin->op == TT::LESS || bin->op == TT::GREATER) {
                return Type::BoolType::instance();
            }
        }
        
        // Variable reference
        if (auto* var = dynamic_cast<AST::VariableExpr*>(expr)) {
            auto it = inferred_types.find(var->name);
            if (it != inferred_types.end()) {
                return it->second;
            }
        }
        
        // Function call
        if (auto* call = dynamic_cast<AST::CallExpr*>(expr)) {
            // Get function return type
            // ...
        }
        
        return Type::AnyType::instance();
    }
    
    void inferVariableType(const std::string& name, AST::Expr* initializer) {
        auto* type = inferType(initializer);
        inferred_types[name] = type;
    }
};
```

---

### المهمة 3.5: Static Type Checker (3 أيام)

```cpp
// Type checker validates types at parse/compile time

class TypeChecker {
public:
    std::unordered_map<std::string, Type::TypeInfo*> symbol_types;
    std::vector<TypeError> errors;
    
    void checkProgram(AST::Program* program) {
        for (auto& stmt : program->statements) {
            checkStatement(stmt.get());
        }
    }
    
    void checkStatement(AST::Stmt* stmt) {
        if (auto* var_decl = dynamic_cast<AST::VarDeclStmt*>(stmt)) {
            checkVarDeclaration(var_decl);
        }
        else if (auto* func_decl = dynamic_cast<AST::FunctionDecl*>(stmt)) {
            checkFunctionDeclaration(func_decl);
        }
        else if (auto* assign = dynamic_cast<AST::AssignmentStmt*>(stmt)) {
            checkAssignment(assign);
        }
        // ... other statements
    }
    
    void checkVarDeclaration(AST::VarDeclStmt* decl) {
        Type::TypeInfo* declared_type = decl->type;
        
        if (decl->initializer) {
            Type::TypeInfo* init_type = inferType(decl->initializer.get());
            
            // Check compatibility
            if (!declared_type->isAssignableFrom(init_type)) {
                errors.push_back(TypeError{
                    "Type mismatch: cannot assign " + 
                    init_type->toString() + " to " + 
                    declared_type->toString(),
                    decl->location
                });
            }
        }
        
        // Register variable type
        symbol_types[decl->name] = declared_type;
    }
    
    void checkAssignment(AST::AssignmentStmt* assign) {
        auto it = symbol_types.find(assign->target);
        if (it == symbol_types.end()) {
            errors.push_back(TypeError{
                "Undefined variable: " + assign->target,
                assign->location
            });
            return;
        }
        
        Type::TypeInfo* target_type = it->second;
        Type::TypeInfo* value_type = inferType(assign->value.get());
        
        if (!target_type->isAssignableFrom(value_type)) {
            errors.push_back(TypeError{
                "Type mismatch in assignment: cannot assign " +
                value_type->toString() + " to " +
                target_type->toString(),
                assign->location
            });
        }
    }
    
    void checkFunctionCall(AST::CallExpr* call) {
        // Get function signature
        auto* func_type = getFunctionType(call->callee);
        
        // Check argument count
        if (call->arguments.size() != func_type->parameters.size()) {
            errors.push_back(TypeError{
                "Wrong number of arguments: expected " +
                std::to_string(func_type->parameters.size()) +
                ", got " + std::to_string(call->arguments.size()),
                call->location
            });
            return;
        }
        
        // Check argument types
        for (size_t i = 0; i < call->arguments.size(); i++) {
            Type::TypeInfo* param_type = func_type->parameters[i].type;
            Type::TypeInfo* arg_type = inferType(call->arguments[i].get());
            
            if (!param_type->isAssignableFrom(arg_type)) {
                errors.push_back(TypeError{
                    "Type mismatch in argument " + std::to_string(i+1) +
                    ": expected " + param_type->toString() +
                    ", got " + arg_type->toString(),
                    call->location
                });
            }
        }
    }
    
    void reportErrors() {
        if (errors.empty()) {
            std::cout << "✅ No type errors found\n";
            return;
        }
        
        std::cout << "❌ Found " << errors.size() << " type errors:\n\n";
        
        for (const auto& error : errors) {
            std::cout << "Error at line " << error.location.line 
                      << ", column " << error.location.column << ":\n";
            std::cout << "  " << error.message << "\n\n";
        }
    }
};
```

---

### المهمة 3.6: Type Aliases (نصف يوم)

```cpp
// Syntax
type UserID = int
type Callback = function(int): void
type Point = {x: float, y: float}

// Arabic
نوع معرف_المستخدم = رقم
نوع استدعاء = دالة(رقم): فراغ

// Implementation
class TypeAlias {
public:
    std::string name;
    Type::TypeInfo* actual_type;
};

std::unordered_map<std::string, TypeAlias> type_aliases;

void registerTypeAlias(const std::string& name, Type::TypeInfo* type) {
    type_aliases[name] = {name, type};
}

Type::TypeInfo* resolveTypeAlias(const std::string& name) {
    auto it = type_aliases.find(name);
    if (it != type_aliases.end()) {
        return it->second.actual_type;
    }
    return nullptr;
}
```

---

## 📈 معايير النجاح

- ✅ Union types 100%
- ✅ Optional types 100%
- ✅ Generics 95%
- ✅ Type inference 90%
- ✅ Type checker 95%
- ✅ 200+ test cases
- ✅ Catches 90%+ of type errors

---

## 📅 الجدول الزمني

| اليوم | المهمة | الساعات |
|------|---------|----------|
| 1-2 | Union types + tests | 16 |
| 3 | Optional types + tests | 8 |
| 4-6 | Generic types + tests | 24 |
| 7-8 | Type inference + tests | 16 |
| 9-10 | Type checker + tests | 16 |

**المجموع:** 80 ساعة

---

**الحالة:** 📝 جاهز للتنفيذ
