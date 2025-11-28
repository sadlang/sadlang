# Phase 7: Inheritance Implementation Plan
# خطة تنفيذ المرحلة 7: الوراثة

## 🎯 Overview | نظرة عامة

**Goal**: Implement full single inheritance support with method overriding and base class access.

**الهدف**: تنفيذ دعم كامل للوراثة الأحادية مع تجاوز الطرق والوصول للصنف الأساس.

---

## ✅ Prerequisites (Already Complete) | المتطلبات (مكتملة)

1. ✅ Basic classes with fields and methods
2. ✅ Access modifiers (public, private, protected)
3. ✅ Static members
4. ✅ Properties with getters/setters
5. ✅ Object instantiation with `new`
6. ✅ `this` keyword support

---

## 📋 Phase 7 Components | مكونات المرحلة 7

### 7.1: Basic Inheritance Syntax ✅ (Already Implemented)
**Status**: Parser already supports inheritance syntax

**Current Support**:
```sad
صنف حيوان
    عام دالة صوت()
        اطبع("صوت حيوان");
    نهاية
نهاية

صنف كلب يرث حيوان
    # Inherits from حيوان
نهاية
```

**Already in Parser**:
- ✅ `يرث` / `inherits` keyword
- ✅ Base class name parsing
- ✅ ClassDecl stores base class name

---

### 7.2: Method Overriding | تجاوز الطرق
**Priority**: P0 (Critical)
**Estimated Time**: 3-4 hours

**Requirements**:
1. Derived class can override base class methods
2. Method signature must match (same parameters)
3. Runtime dispatches to most derived version
4. Support `تجاوز` / `override` keyword (optional)

**Syntax**:
```sad
صنف حيوان
    عام دالة صوت()
        اطبع("صوت حيوان");
    نهاية
نهاية

صنف كلب يرث حيوان
    عام تجاوز دالة صوت()
        اطبع("نباح!");
    نهاية
نهاية

متغير ك = كلب();
ك.صوت();  # يطبع: نباح!
```

**Implementation Steps**:
1. ✅ Parser: Support `override` keyword (optional modifier)
2. ⏳ ClassType: Store override flag in MethodInfo
3. ⏳ Runtime: Check method signature compatibility
4. ⏳ Runtime: Virtual dispatch to derived method

---

### 7.3: Base Class Member Access | الوصول لأعضاء الصنف الأساس
**Priority**: P0 (Critical)
**Estimated Time**: 2-3 hours

**Requirements**:
1. Access inherited public members
2. Access inherited protected members
3. Block access to inherited private members
4. Proper member lookup chain (derived → base)

**Implementation Steps**:
1. ⏳ ClassType: Implement member lookup with inheritance chain
2. ⏳ Runtime: Search base class if member not found in derived
3. ⏳ Runtime: Check access modifiers with inheritance context
4. ⏳ Test: Verify member access from derived class

---

### 7.4: `super` Keyword | كلمة `الأساس`
**Priority**: P1 (High)
**Estimated Time**: 2 hours

**Requirements**:
1. Call base class methods from derived class
2. Support `الأساس` / `super` keyword
3. Access base class version of overridden method

**Syntax**:
```sad
صنف كلب يرث حيوان
    عام تجاوز دالة صوت()
        الأساس.صوت();  # استدعاء طريقة الصنف الأساسي
        اطبع("نباح!");
    نهاية
نهاية
```

**Implementation Steps**:
1. ✅ Lexer: `super` / `الأساس` already as KEYWORD_SUPER
2. ⏳ Parser: Enable SuperExpr parsing (currently commented)
3. ⏳ AST: SuperExpr node (already exists)
4. ⏳ Runtime: Resolve super.member to base class method

---

### 7.5: Constructor Chaining | تسلسل البواني
**Priority**: P1 (High)
**Estimated Time**: 2-3 hours

**Requirements**:
1. Derived constructor calls base constructor
2. Explicit call with `الأساس()` / `super()`
3. Implicit call if not specified
4. Parameter passing to base constructor

**Syntax**:
```sad
صنف حيوان
    خاص نص الاسم;
    
    باني(اسم)
        هذا.الاسم = اسم;
    نهاية
نهاية

صنف كلب يرث حيوان
    باني(اسم_الكلب)
        الأساس(اسم_الكلب);  # استدعاء باني الصنف الأساسي
    نهاية
نهاية
```

**Implementation Steps**:
1. ⏳ Parser: Support super() call in constructor
2. ⏳ Runtime: Execute base constructor before derived
3. ⏳ Runtime: Pass parameters to base constructor
4. ⏳ Test: Constructor chaining with parameters

---

### 7.6: Protected Member Access | الوصول للأعضاء المحمية
**Priority**: P0 (Critical)
**Estimated Time**: 1-2 hours

**Requirements**:
1. Protected members accessible in derived classes
2. Protected members blocked from external access
3. Proper error messages

**Implementation Steps**:
1. ⏳ Runtime: Update checkMemberAccess for inheritance
2. ⏳ Runtime: Allow protected access within derived class
3. ⏳ Test: Protected member access scenarios

---

## 🧪 Testing Strategy | استراتيجية الاختبار

### Test Cases

1. **Basic Inheritance**
   - ✅ Parser accepts inheritance syntax
   - ⏳ Base class members accessible from derived
   - ⏳ Derived class creates base class fields

2. **Method Overriding**
   - ⏳ Override method with same signature
   - ⏳ Call overridden method from derived instance
   - ⏳ Error on signature mismatch

3. **Super Keyword**
   - ⏳ Call base method with `super.method()`
   - ⏳ Access base fields with `super.field`

4. **Constructor Chaining**
   - ⏳ Implicit base constructor call
   - ⏳ Explicit base constructor call with params
   - ⏳ Field initialization order

5. **Protected Access**
   - ⏳ Protected members accessible in derived
   - ⏳ Protected members blocked from outside

---

## 📊 Implementation Progress | التقدم في التنفيذ

### Phase 7.1: Basic Inheritance ✅ 100%
- ✅ Parser syntax support
- ✅ ClassDecl stores base class

### Phase 7.2: Method Overriding ⏳ 0%
- ⏳ Override keyword parsing
- ⏳ Virtual dispatch implementation
- ⏳ Signature validation

### Phase 7.3: Member Access ⏳ 0%
- ⏳ Inheritance chain lookup
- ⏳ Access control with inheritance

### Phase 7.4: Super Keyword ⏳ 0%
- ⏳ SuperExpr parsing
- ⏳ Super member resolution

### Phase 7.5: Constructor Chaining ⏳ 0%
- ⏳ Base constructor calls
- ⏳ Parameter passing

### Phase 7.6: Protected Access ⏳ 0%
- ⏳ Protected member rules

---

## 🎯 Success Criteria | معايير النجاح

✅ **Parser**:
- [x] Inheritance syntax accepted
- [ ] Override keyword parsed
- [ ] Super expressions parsed

✅ **Runtime**:
- [ ] Method overriding works correctly
- [ ] Member lookup follows inheritance chain
- [ ] Protected members accessible in derived
- [ ] Super keyword resolves to base class
- [ ] Constructor chaining functional

✅ **Testing**:
- [ ] 10+ inheritance test cases pass
- [ ] Error messages clear and bilingual

---

## 🚀 Next Session Plan | خطة الجلسة القادمة

**Start with**: 7.3 Member Access (easiest, builds foundation)  
**Then**: 7.2 Method Overriding  
**Then**: 7.6 Protected Access  
**Then**: 7.4 Super Keyword  
**Finally**: 7.5 Constructor Chaining  

**Estimated Total Time**: 10-14 hours

---

**Status**: 📝 Planning Complete  
**Ready to Start**: Phase 7.3 - Member Access
