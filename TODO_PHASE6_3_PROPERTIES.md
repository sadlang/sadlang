# TODO: Phase 6.3 - Properties Implementation

**Status**: 📋 Ready to Start  
**Priority**: HIGH  
**Estimated Time**: 4-6 hours  
**Start Date**: November 28, 2025 (planned)

---

## 🎯 Goal

Implement **Properties/Getters/Setters** syntax in Sad Language to enable:
- Encapsulated field access
- Data validation
- Computed properties
- Elegant API design

---

## 📝 Task Breakdown

### 1. Keywords & Lexer (30 min)
- [ ] Add `خاصية` / `property` keyword
- [ ] Add `احصل` / `get` keyword  
- [ ] Add `عيّن` / `set` keyword
- [ ] Test lexer with new keywords

**Files to modify:**
- `include/lexer/token.h` - Add token types
- `src/lexer/lexer.cpp` - Add keyword mappings

---

### 2. AST Nodes (1 hour)

#### PropertyDecl Node
```cpp
class PropertyDecl : public MemberDecl {
public:
    std::string name;
    DataType type;
    std::unique_ptr<GetterBlock> getter;
    std::unique_ptr<SetterBlock> setter;
    AccessModifier visibility;
    bool isStatic;
};

class GetterBlock {
public:
    std::unique_ptr<BlockStmt> body;
};

class SetterBlock {
public:
    std::string parameterName;
    std::unique_ptr<BlockStmt> body;
};
```

**Files to create/modify:**
- `include/parser/ast/property_nodes.h` - New file
- `include/parser/ast/ast_visitor.h` - Add visitor methods
- `src/parser/ast/property_nodes.cpp` - Implementation

**Tasks:**
- [ ] Create PropertyDecl class
- [ ] Create GetterBlock class
- [ ] Create SetterBlock class
- [ ] Add visitor methods
- [ ] Add toString() methods

---

### 3. Parser Enhancement (1.5 hours)

#### Property Parsing Logic
```
خاصية <type> <name>
    احصل
        <statements>
    نهاية
    [عيّن(<param>)
        <statements>
    نهاية]
نهاية
```

**Files to modify:**
- `src/parser/parser_core_oop.cpp` - Add parseProperty()
- `src/parser/parser.h` - Add method declaration

**Tasks:**
- [ ] Implement `parseProperty()` method
- [ ] Parse getter block
- [ ] Parse setter block (optional)
- [ ] Handle read-only properties (getter only)
- [ ] Handle write-only properties (setter only)
- [ ] Integrate with class member parsing

---

### 4. Runtime Implementation (2 hours)

#### Property Access
- Property reads → Call getter
- Property writes → Call setter

**Files to modify:**
- `src/interpreter/visitors/expression_evaluator.cpp`
  - Modify `visitMemberExpr()` - check if field is property, call getter
  - Modify `visitMemberAssignExpr()` - check if field is property, call setter
- `src/interpreter/visitors/statement_executor_oop.cpp`
  - Add property registration in `visitClassDecl()`
- `include/data/types/class_type.h`
  - Add property storage in ClassType

**Tasks:**
- [ ] Add property storage to ClassType
- [ ] Implement property getter invocation
- [ ] Implement property setter invocation
- [ ] Handle missing getter/setter errors
- [ ] Handle parameter passing to setter

---

### 5. Testing (1 hour)

#### Test Cases

**Test 1: Basic Property**
```sad
صنف شخص
    خاص نص _الاسم
    
    خاصية نص الاسم
        احصل
            ارجع _الاسم
        نهاية
        عيّن(نص قيمة)
            _الاسم = قيمة
        نهاية
    نهاية
نهاية   عيّن(نص قيمة)
            _الاسم = قيمة
        نهاية
    نهاية
نهاية

شخص س = جديد شخص()
س.الاسم = "أحمد"      // ✅ Calls setter
اطبع(س.الاسم)         // ✅ Calls getter, outputs "أحمد"
```

**Test 2: Read-Only Property**
```sad
صنف دائرة
    خاص رقم نصف_القطر
    
    خاصية رقم المساحة
        احصل
            ارجع 3.14 * نصف_القطر * نصف_القطر
        نهاية
    نهاية
نهاية


دائرة د = جديد دائرة()
د.نصف_القطر = 5
اطبع(د.المساحة)       // ✅ 78.5
د.المساحة = 100        // ❌ Error: read-only property
```

**Test 3: Validation in Setter**
```sad
صنف حساب
    خاص رقم _الرصيد
    
    خاصية رقم الرصيد
        احصل
            ارجع _الرصيد
        نهاية
        عيّن(رقم قيمة)
            اذا قيمة >= 0
                _الرصيد = قيمة
            والا
                اطبع("خطأ: الرصيد لا يمكن أن يكون سالباً")
            نهاية
        نهاية
    نهاية
نهاية   نهاية
    نهاية
نهاية

حساب ح = جديد حساب()
ح.الرصيد = 1000        // ✅ Valid
ح.الرصيد = -100        // ❌ Prints error, doesn't change
```

**Test 4: Static Property**
```sad
صنف تطبيق
    ساكن خاص نص _اسم_التطبيق
    
    ساكن خاصية نص الاسم
        احصل
            ارجع _اسم_التطبيق
        نهاية
        عيّن(نص قيمة)
            _اسم_التطبيق = قيمة
        نهاية
    نهاية
نهاية

تطبيق.الاسم = "تطبيقي"
اطبع(تطبيق.الاسم)     // ✅ "تطبيقي"
```

**Test 5: Computed Property**
```sad
صنف مستطيل
    عام رقم الطول
    عام رقم العرض
    
    خاصية رقم المساحة
        احصل
            ارجع الطول * العرض
        نهاية
    نهاية
    
    خاصية رقم المحيط
        احصل
            ارجع 2 * (الطول + العرض)
        نهاية
    نهاية
نهاية   احصل
            ارجع 2 * (الطول + العرض)
        نهاية
    نهاية
نهاية

مستطيل م = جديد مستطيل()
م.الطول = 10
م.العرض = 5
اطبع(م.المساحة)       // ✅ 50
اطبع(م.المحيط)        // ✅ 30
```

**Test 6: Chained Property Access**
```sad
صنف عنوان
    عام نص المدينة
نهاية

صنف شخص
    خاص عنوان _العنوان
    
    خاصية عنوان العنوان
        احصل
            ارجع _العنوان
        نهاية
        عيّن(عنوان ع)
            _العنوان = ع
        نهاية
    نهاية
نهاية   عيّن(عنوان ع)
            _العنوان = ع
        نهاية
    نهاية
نهاية

شخص س = جديد شخص()
عنوان ع = جديد عنوان()
ع.المدينة = "الرياض"
س.العنوان = ع
اطبع(س.العنوان.المدينة)  // ✅ "الرياض"
```

**Files to create:**
- `tests/oop/phase6/test_property_basic.s`
- `tests/oop/phase6/test_property_readonly.s`
- `tests/oop/phase6/test_property_validation.s`
- `tests/oop/phase6/test_property_static.s`
- `tests/oop/phase6/test_property_computed.s`
- `tests/oop/phase6/test_property_comprehensive.s`

---

### 6. Documentation (30 min)

**Files to create:**
- `PHASE6_3_PROPERTIES_REPORT.md` - Detailed report
- `PHASE6_3_QUICK_SUMMARY.md` - Quick reference

**Content:**
- Implementation details
- Test results
- Usage examples
- Comparison with other languages

---

## 🔍 Edge Cases to Handle

- [ ] Property without getter (compile error)
- [ ] Accessing write-only property (runtime error)
- [ ] Writing to read-only property (runtime error)
- [ ] Property with same name as field (conflict)
- [ ] Static property access
- [ ] Property in inherited class
- [ ] Property overriding
- [ ] this/هذا in getter/setter

---

## 📊 Success Criteria

✅ All 6 test cases pass  
✅ Clean compilation (no errors)  
✅ Properties work with inheritance  
✅ Static properties work  
✅ Error messages are clear and bilingual  
✅ Code is well-documented  
✅ Performance is acceptable (no overhead)

---

## 🚧 Potential Challenges

1. **Parser Complexity**: Properties have nested blocks
   - Solution: Use recursive descent parsing

2. **Runtime Dispatch**: Distinguish field access from property access
   - Solution: Check ClassType for property definitions first

3. **Setter Parameters**: Need to pass value to setter
   - Solution: Store parameter in scope before executing setter body

4. **Static Properties**: Need different handling than instance properties
   - Solution: Similar to static methods, check isStatic flag

---

## 📝 Notes

- Properties should feel transparent (like fields)
- Getters are parameter-less
- Setters have exactly one parameter
- Properties can be inherited and overridden
- Access modifiers apply to the property as a whole

---

## ✅ Completion Checklist

- [ ] Lexer updated
- [ ] AST nodes created
- [ ] Parser implemented
- [ ] Runtime working
- [ ] All tests passing
- [ ] Documentation complete
- [ ] Code reviewed
- [ ] Performance acceptable

---

**Ready to start**: When user says **"أكمل Phase 6.3"**

**Expected completion**: 1 day (with testing)
