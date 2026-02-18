# 📋 توصيات Phase 1.3.1 - تحليل سريع
# 📋 Phase 1.3.1 Recommendations - Quick Analysis

**التاريخ:** 2 يناير 2026  
**المدة المتوقعة:** 3-5 أيام  
**الحالة:** 🟢 جاهز للتنفيذ

---

## ✅ ما هو موجود بالفعل (Already Complete)

### 1. Parser يدعم Type Annotations كاملاً ✅

```cpp
// src/parser/parser_core_helpers.cpp
Data::DataType ParserCore::parseType() {
    // ✅ يدعم: رقم، عشري، نص، منطقي، فراغ، مصفوفة، قاموس
}

std::vector<Parameter> ParserCore::parseTypedParameterList() {
    // ✅ يدعم: (أ: رقم، ب: نص، ج: منطقي = صحيح)
}
```

### 2. AST يحفظ Type Annotations ✅

```cpp
// include/parser/ast/declarations.h
class FunctionDecl {
    Data::DataType returnType;  // ✅ نوع الإرجاع
    std::vector<Parameter> parameters;  // ✅ مع الأنواع
};

struct Parameter {
    Data::DataType type;  // ✅ نوع المعامل
    ExprPtr defaultValue;  // ✅ قيمة افتراضية
};
```

### 3. أمثلة تعمل الآن ✅

```s
# ✅ كل هذا الكود يُحلّل بنجاح:
متغير عمر: رقم = 25
متغير اسم: نص = "أحمد"

دالة رقم مربع(س: رقم) {
    إرجاع س * س
}

دالة نص دمج(أ: نص، ب: نص = "افتراضي") {
    إرجاع أ + ب
}
```

---

## 🔧 ما المطلوب في Phase 1.3.1 (What's Needed)

### المشكلة: Type Annotations موجودة لكن محدودة

**الأنواع المدعومة حالياً:**
- ✅ رقم (INTEGER)
- ✅ عشري (DOUBLE)
- ✅ نص (STRING)
- ✅ منطقي (BOOLEAN)
- ✅ فراغ (VOID)
- ✅ مصفوفة (ARRAY) - بدون type parameters
- ✅ قاموس (MAP) - بدون type parameters

**الأنواع المطلوبة:**
- ❌ Generic types: `مصفوفة<رقم>`
- ❌ Union types: `رقم | نص`
- ❌ Optional types: `رقم?`
- ❌ Function types: `(رقم، نص) -> منطقي`
- ❌ Complex generics: `قاموس<نص، مصفوفة<رقم>>`

---

## 🎯 خطة التنفيذ (Implementation Plan)

### المرحلة 1: توسيع AST (يوم 1-2)

#### 1.1 إنشاء عقد الأنواع المتقدمة

**الملف الجديد:** `include/parser/ast/type_nodes.h`

```cpp
namespace Sad::AST {

// ═══════════════════════════════════════════════════════
// نوع عام (Generic Type): مصفوفة<T>
// ═══════════════════════════════════════════════════════
class GenericTypeNode : public ASTNode {
public:
    std::string baseTypeName;  // "مصفوفة"، "قاموس"
    std::vector<TypeNodePtr> typeParameters;  // [رقم]، [نص, مصفوفة<رقم>]
    
    GenericTypeNode(std::string base, std::vector<TypeNodePtr> params)
        : baseTypeName(std::move(base))
        , typeParameters(std::move(params)) {}
    
    std::string toString() const override {
        std::string result = baseTypeName + "<";
        for (size_t i = 0; i < typeParameters.size(); ++i) {
            if (i > 0) result += "، ";
            result += typeParameters[i]->toString();
        }
        result += ">";
        return result;
    }
};

// ═══════════════════════════════════════════════════════
// نوع اتحاد (Union Type): رقم | نص
// ═══════════════════════════════════════════════════════
class UnionTypeNode : public ASTNode {
public:
    std::vector<TypeNodePtr> types;  // [رقم، نص، منطقي]
    
    explicit UnionTypeNode(std::vector<TypeNodePtr> t)
        : types(std::move(t)) {}
    
    std::string toString() const override {
        std::string result;
        for (size_t i = 0; i < types.size(); ++i) {
            if (i > 0) result += " | ";
            result += types[i]->toString();
        }
        return result;
    }
};

// ═══════════════════════════════════════════════════════
// نوع اختياري (Optional Type): رقم?
// ═══════════════════════════════════════════════════════
class OptionalTypeNode : public ASTNode {
public:
    TypeNodePtr baseType;  // رقم
    
    explicit OptionalTypeNode(TypeNodePtr base)
        : baseType(std::move(base)) {}
    
    std::string toString() const override {
        return baseType->toString() + "?";
    }
};

// ═══════════════════════════════════════════════════════
// نوع دالة (Function Type): (رقم، نص) -> منطقي
// ═══════════════════════════════════════════════════════
class FunctionTypeNode : public ASTNode {
public:
    std::vector<TypeNodePtr> paramTypes;  // [رقم، نص]
    TypeNodePtr returnType;  // منطقي
    
    FunctionTypeNode(std::vector<TypeNodePtr> params, TypeNodePtr ret)
        : paramTypes(std::move(params))
        , returnType(std::move(ret)) {}
    
    std::string toString() const override {
        std::string result = "(";
        for (size_t i = 0; i < paramTypes.size(); ++i) {
            if (i > 0) result += "، ";
            result += paramTypes[i]->toString();
        }
        result += ") -> " + returnType->toString();
        return result;
    }
};

// ═══════════════════════════════════════════════════════
// نوع بدائي (Primitive Type): رقم، نص، منطقي
// ═══════════════════════════════════════════════════════
class PrimitiveTypeNode : public ASTNode {
public:
    Data::DataType type;  // INTEGER, STRING, BOOLEAN, etc.
    
    explicit PrimitiveTypeNode(Data::DataType t) : type(t) {}
    
    std::string toString() const override {
        // تحويل DataType إلى نص
        switch (type) {
            case Data::DataType::INTEGER: return "رقم";
            case Data::DataType::DOUBLE: return "عشري";
            case Data::DataType::STRING: return "نص";
            case Data::DataType::BOOLEAN: return "منطقي";
            case Data::DataType::VOID: return "فراغ";
            default: return "مجهول";
        }
    }
};

} // namespace Sad::AST
```

#### 1.2 تحديث Data::DataType

**الملف:** `include/data/types/value.h`

```cpp
enum class DataType {
    // الأنواع البسيطة (موجودة)
    VOID, INTEGER, DOUBLE, STRING, BOOLEAN,
    
    // الأنواع المركبة (موجودة)
    ARRAY, MAP, TUPLE, FUNCTION,
    
    // الأنواع المتقدمة (جديدة)
    GENERIC,        // مصفوفة<T>
    UNION,          // A | B
    OPTIONAL,       // T?
    FUNCTION_TYPE,  // (T1, T2) -> T3
    
    // أنواع خاصة
    UNKNOWN, ANY, ERROR
};
```

### المرحلة 2: توسيع Parser (يوم 2-3)

#### 2.1 إنشاء دوال تحليل جديدة

**الملف الجديد:** `src/parser/parser_types.cpp`

```cpp
#include "../include/parser/parser_core.h"
#include "../include/parser/ast/type_nodes.h"

namespace Sad::Parser {

// ═══════════════════════════════════════════════════════
// تحليل نوع معقد (Complex Type)
// ═══════════════════════════════════════════════════════
TypeNodePtr ParserCore::parseComplexType() {
    // 1. نوع بدائي أو اسم نوع
    TypeNodePtr baseType = parsePrimaryType();
    
    // 2. التحقق من الإضافات
    while (true) {
        // Generic: مصفوفة<رقم>
        if (match(TokenType::LESS)) {
            std::vector<TypeNodePtr> params;
            do {
                params.push_back(parseComplexType());
            } while (match(TokenType::COMMA) || match(TokenType::ARABIC_COMMA));
            consume(TokenType::GREATER, "Expected '>'");
            baseType = std::make_unique<GenericTypeNode>(
                baseType->toString(), std::move(params)
            );
        }
        // Optional: رقم?
        else if (match(TokenType::QUESTION)) {
            baseType = std::make_unique<OptionalTypeNode>(std::move(baseType));
        }
        // Union: رقم | نص
        else if (match(TokenType::PIPE)) {
            std::vector<TypeNodePtr> types;
            types.push_back(std::move(baseType));
            do {
                types.push_back(parseComplexType());
            } while (match(TokenType::PIPE));
            baseType = std::make_unique<UnionTypeNode>(std::move(types));
        }
        else {
            break;
        }
    }
    
    return baseType;
}

// ═══════════════════════════════════════════════════════
// تحليل نوع بدائي
// ═══════════════════════════════════════════════════════
TypeNodePtr ParserCore::parsePrimaryType() {
    // 1. Function type: (رقم، نص) -> منطقي
    if (match(TokenType::LEFT_PAREN)) {
        std::vector<TypeNodePtr> paramTypes;
        if (!check(TokenType::RIGHT_PAREN)) {
            do {
                paramTypes.push_back(parseComplexType());
            } while (match(TokenType::COMMA) || match(TokenType::ARABIC_COMMA));
        }
        consume(TokenType::RIGHT_PAREN, "Expected ')'");
        consume(TokenType::ARROW, "Expected '->'");
        TypeNodePtr returnType = parseComplexType();
        return std::make_unique<FunctionTypeNode>(
            std::move(paramTypes), std::move(returnType)
        );
    }
    
    // 2. نوع بسيط: رقم، نص، منطقي
    if (isTypeKeyword(peek())) {
        Data::DataType type = parseType();  // الدالة الموجودة
        return std::make_unique<PrimitiveTypeNode>(type);
    }
    
    // 3. اسم نوع مخصص (للأصناف)
    if (check(TokenType::IDENTIFIER)) {
        std::string typeName = advance().lexeme;
        return std::make_unique<PrimitiveTypeNode>(Data::DataType::UNKNOWN);
    }
    
    error("Expected type");
    return nullptr;
}

} // namespace Sad::Parser
```

#### 2.2 تحديث parseType() الموجودة

```cpp
// في parser_core_helpers.cpp
// استبدال parseType() باستدعاء parseComplexType()

Data::DataType ParserCore::parseType() {
    TypeNodePtr typeNode = parseComplexType();
    
    // تحويل مؤقت إلى DataType
    // (لاحقاً سنحفظ TypeNode مباشرة في AST)
    if (auto* primitive = dynamic_cast<PrimitiveTypeNode*>(typeNode.get())) {
        return primitive->type;
    }
    
    return Data::DataType::UNKNOWN;
}
```

### المرحلة 3: تحديث AST (يوم 3-4)

#### 3.1 تحديث FunctionDecl و Parameter

```cpp
// في include/parser/ast/declarations.h

class FunctionDecl : public Statement {
public:
    std::string name;
    std::vector<Parameter> parameters;
    
    // استبدال Data::DataType بـ TypeNodePtr
    TypeNodePtr returnTypeNode;  // ✅ جديد (بدلاً من returnType)
    
    StmtPtr body;
    bool isExported;
    ExprList decorators;
};

struct Parameter {
    std::string name;
    
    // استبدال Data::DataType بـ TypeNodePtr
    TypeNodePtr typeNode;  // ✅ جديد (بدلاً من type)
    
    ExprPtr defaultValue;
    bool isVarargs;
    bool isKwargs;
};
```

### المرحلة 4: الاختبارات (يوم 4-5)

#### 4.1 اختبارات Parser

**الملف:** `tests/parser_tests/test_advanced_types.cpp`

```cpp
#include <gtest/gtest.h>
#include "../../include/parser/parser_core.h"

TEST(AdvancedTypesTest, GenericTypes) {
    // مصفوفة<رقم>
    std::string code = R"(
        متغير أرقام: مصفوفة<رقم> = [1، 2، 3]
    )";
    
    Parser parser(code);
    auto ast = parser.parse();
    
    ASSERT_TRUE(ast != nullptr);
    // التحقق من GenericTypeNode
}

TEST(AdvancedTypesTest, UnionTypes) {
    // رقم | نص
    std::string code = R"(
        دالة معالج(قيمة: رقم | نص) {
            اطبع(قيمة)
        }
    )";
    
    Parser parser(code);
    auto ast = parser.parse();
    
    ASSERT_TRUE(ast != nullptr);
    // التحقق من UnionTypeNode
}

TEST(AdvancedTypesTest, OptionalTypes) {
    // رقم?
    std::string code = R"(
        دالة رقم? احصل_عمر(اسم: نص) {
            إرجاع لاشيء
        }
    )";
    
    Parser parser(code);
    auto ast = parser.parse();
    
    ASSERT_TRUE(ast != nullptr);
    // التحقق من OptionalTypeNode
}

TEST(AdvancedTypesTest, FunctionTypes) {
    // (رقم، نص) -> منطقي
    std::string code = R"(
        متغير معالج: (رقم، نص) -> منطقي
    )";
    
    Parser parser(code);
    auto ast = parser.parse();
    
    ASSERT_TRUE(ast != nullptr);
    // التحقق من FunctionTypeNode
}

TEST(AdvancedTypesTest, ComplexTypes) {
    // قاموس<نص، مصفوفة<رقم>>
    std::string code = R"(
        متغير بيانات: قاموس<نص، مصفوفة<رقم>>
    )";
    
    Parser parser(code);
    auto ast = parser.parse();
    
    ASSERT_TRUE(ast != nullptr);
    // التحقق من Nested GenericTypeNode
}
```

---

## 📋 قائمة الملفات المطلوبة (Required Files)

### ملفات جديدة (New Files):
```
include/parser/ast/type_nodes.h          # عقد الأنواع المتقدمة
src/parser/ast/type_nodes.cpp            # تنفيذ toString() وغيرها
src/parser/parser_types.cpp              # دوال تحليل الأنواع
tests/parser_tests/test_advanced_types.cpp  # اختبارات
```

### ملفات للتعديل (Files to Modify):
```
include/data/types/value.h               # إضافة GENERIC, UNION, OPTIONAL
include/parser/ast/declarations.h        # استبدال DataType بـ TypeNodePtr
src/parser/parser_core_helpers.cpp       # تحديث parseType()
```

---

## ✅ معايير النجاح (Success Criteria)

### Phase 1.3.1 Complete عندما:

1. ✅ Parser يدعم:
   - `مصفوفة<رقم>`
   - `رقم | نص`
   - `رقم?`
   - `(رقم، نص) -> منطقي`
   - `قاموس<نص، مصفوفة<رقم>>`

2. ✅ AST يحتوي على TypeNodePtr بدلاً من DataType

3. ✅ جميع الاختبارات تمر:
   ```bash
   cd build
   cmake --build . --target test_advanced_types
   .\bin\Debug\test_advanced_types.exe
   ```

4. ✅ أمثلة تعمل:
   ```s
   # Generic
   متغير أرقام: مصفوفة<رقم> = [1، 2، 3]
   
   # Union
   دالة معالج(قيمة: رقم | نص) { }
   
   # Optional
   دالة رقم? احصل(مفتاح: نص) { }
   
   # Function type
   متغير معالج: (رقم) -> نص
   
   # Complex
   متغير بيانات: قاموس<نص، مصفوفة<رقم>>
   ```

---

## 🚀 الخطوة التالية بعد Phase 1.3.1

### Phase 1.3.2: Type Checker Integration (5-7 أيام)

1. إنشاء TypeMapper:
   ```cpp
   TypeNodePtr → compiler/type_system/TypePtr
   ```

2. ربط Parser بـ Type Checker

3. تفعيل Type checking في Compiler

### Phase 1.3.3: Type Inference (7-10 أيام)

1. Algorithm W implementation
2. Constraint generation
3. Unification engine

---

## 📊 الجدول الزمني (Timeline)

```
اليوم 1-2:  إنشاء type_nodes.h وتنفيذ الأصناف
اليوم 2-3:  توسيع Parser (parseComplexType)
اليوم 3-4:  تحديث AST (TypeNodePtr)
اليوم 4-5:  الاختبارات والتوثيق

المدة الكلية: 3-5 أيام
```

---

## 🎯 الخلاصة (Summary)

### ✅ ما هو جاهز:
- Parser الأساسي ✅
- AST الأساسي ✅
- Type annotations البسيطة ✅

### 🔧 ما المطلوب:
- عقد الأنواع المتقدمة (type_nodes.h)
- توسيع Parser للأنواع المعقدة
- تحديث AST لاستخدام TypeNodePtr

### 🚀 النتيجة:
```s
# سيعمل كل هذا الكود:
متغير أرقام: مصفوفة<رقم> = [1، 2، 3]
متغير بيانات: قاموس<نص، مصفوفة<رقم>>
دالة معالج(قيمة: رقم | نص) { }
دالة رقم? احصل(مفتاح: نص) { }
متغير دالة_معالج: (رقم، نص) -> منطقي
```

---

**جاهز للبدء! 🚀**
