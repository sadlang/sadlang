# سجل تنفيذ المرحلة 3: الدوال
# Phase 03 Implementation Log: Functions

## التاريخ / Date: 2025-01-XX

## الملفات المعدلة / Modified Files
- `compiler/frontend/src/sir_builder.cpp` - تنفيذ `buildFunctionCall`

## تفاصيل التنفيذ / Implementation Details

### 1. buildFunctionCall (sir_builder.cpp:1797-1910)

#### المصادر المرجعية / Reference Sources:
| المكون | الملف | السطر |
|--------|-------|-------|
| CallExpr | expressions.h | 289-304 |
| CallExpr::callee | expressions.h | 290 |
| CallExpr::arguments | expressions.h | 291 |
| VariableExpr::name | expressions.h | 212 |
| SIRInstruction::Call | sir_instruction.h | 231-238 |
| SIROperand::Function | sir_types.h | 390-395 |
| SIROperand::Register | sir_types.h | 355-362 |
| FunctionInfo | sir_builder.h | 162-175 |
| functionTable_ | sir_builder.h | 719 |

#### الخطوات المنفذة / Implemented Steps:
1. ✅ استخراج اسم الدالة من `callee` (VariableExpr)
2. ✅ بناء المعاملات باستخدام `buildExpression()`
3. ✅ البحث عن الدالة في `functionTable_`
4. ✅ إنشاء `SIROperand::Function` لاسم الدالة
5. ✅ إنشاء `SIRInstruction::Call` مع النتيجة والمعاملات
6. ✅ إضافة التعليمة إلى `currentBlock_`

#### الكود المضاف / Added Code:
```cpp
BuildResult SIRBuilder::buildFunctionCall(AST::FunctionCallNode* call) {
    // الخطوة 1: استخراج اسم الدالة
    std::string funcName;
    if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr*>(call->callee.get())) {
        funcName = varExpr->name;
    }
    
    // الخطوة 2: بناء المعاملات
    std::vector<SIROperand> argOperands;
    for (const auto& arg : call->arguments) {
        BuildResult argResult = buildExpression(arg.get());
        SIROperand argOp = SIROperand::Register(argResult.registerName, argResult.type);
        argOperands.push_back(argOp);
    }
    
    // الخطوة 3: البحث عن نوع الإرجاع
    SIRType returnType = SIRType::VOID;
    auto it = functionTable_.find(funcName);
    if (it != functionTable_.end()) {
        returnType = it->second.returnType;
    }
    
    // الخطوة 4-5: إنشاء التعليمة
    std::string resultReg = newTempRegister();
    SIROperand funcOp = SIROperand::Function(funcName);
    SIROperand resultOp = SIROperand::Register(resultReg, returnType);
    SIRInstruction callInst = SIRInstruction::Call(resultOp, funcOp, argOperands);
    
    // الخطوة 6: إضافة للكتلة
    currentBlock_->instructions.push_back(callInst);
    
    return BuildResult(resultReg, returnType);
}
```

## نتائج الاختبارات / Test Results

### الاختبار 1: استدعاء بسيط
**الملف:** `test_phase3_function_call.s`
```s
دالة جمع(رقم أ، رقم ب)
    ارجع أ + ب
نهاية

دالة رئيسية()
    رقم نتيجة = جمع(10، 32)
    ارجع نتيجة
نهاية
```
**النتيجة المتوقعة:** 42
**النتيجة الفعلية:** 42 ✅

### الاختبار 2: استدعاءات متداخلة
**الملف:** `test_phase3_nested_calls.s`
```s
دالة مضاعفة(رقم ن)
    ارجع ن * 2
نهاية

دالة جمع(رقم أ، رقم ب)
    ارجع أ + ب
نهاية

دالة رئيسية()
    رقم نتيجة = جمع(مضاعفة(5)، 32)
    ارجع نتيجة
نهاية
```
**النتيجة المتوقعة:** 42 (مضاعفة(5)=10، جمع(10،32)=42)
**النتيجة الفعلية:** 42 ✅

## الحالة / Status
✅ **مكتمل** - تم تنفيذ `buildFunctionCall` بنجاح

## الخطوات التالية / Next Steps
1. تحسين `buildUnaryOp` للعمليات الأحادية
2. إضافة دعم الدوال المدمجة (اطبع، اقرأ، إلخ)
3. المرحلة 4: التحكم بالتدفق (if/while/for)
