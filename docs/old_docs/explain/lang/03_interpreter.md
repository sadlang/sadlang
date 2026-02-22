# المفسر (Interpreter)

## ما هو المفسر؟

المفسر (Interpreter) هو أحد المكونات الأساسية في لغة ص، وهو المسؤول عن قراءة وتنفيذ الكود المصدري مباشرة دون الحاجة إلى ترجمته أولاً إلى كود آلة. يعمل المفسر بطريقة تسلسلية حيث يقرأ كل عبارة في البرنامج، ويحللها، ثم ينفذها فوراً قبل الانتقال إلى العبارة التالية. هذا النهج يجعل المفسر مثالياً للتطوير السريع والتجريب، حيث يمكن للمبرمج رؤية نتائج تغييراته فوراً دون انتظار عملية الترجمة.

الفرق الجوهري بين المفسر والمترجم هو أن المفسر ينفذ البرنامج أثناء قراءته، بينما المترجم يحول البرنامج بالكامل إلى ملف تنفيذي أولاً ثم يتم تشغيله لاحقاً. المفسر أبطأ في التنفيذ لأنه يقوم بعملية التحليل في كل مرة يُشغَّل فيها البرنامج، لكنه يوفر مرونة أكبر وسرعة في دورة التطوير. في لغة ص، المفسر يُستخدم بشكل رئيسي في بيئة REPL وللتطوير السريع، بينما المترجم يُستخدم للإنتاج والبرامج التي تحتاج أداءً عالياً.

## بنية المفسر

يتواجد المفسر في المجلد `interpreter_new/` ويتكون من عدة مكونات متخصصة:

```
interpreter_new/
├── include/
│   ├── expression_evaluator.h
│   ├── statement_executor.h
│   └── ...
├── src/
│   ├── visitors/
│   │   ├── expression_evaluator.cpp    # مُقيِّم التعابير
│   │   ├── statement_executor.cpp      # منفذ العبارات
│   │   └── statement_executor_oop.cpp  # دعم البرمجة الكائنية
│   ├── builtins/                       # الدوال المدمجة
│   ├── core/                           # المكونات الأساسية
│   ├── managers/                       # مديرو المتغيرات والنطاقات
│   └── oop/                            # دعم الأصناف والكائنات
```

## مكونات المفسر الرئيسية

### مُقيِّم التعابير (Expression Evaluator)

مُقيِّم التعابير هو المكون المسؤول عن حساب قيم التعابير في البرنامج. يستخدم نمط الزائر (Visitor Pattern) للمرور على شجرة AST وتقييم كل عقدة. عندما يواجه تعبيراً، يقوم بحساب قيمته وإرجاعها.

**تقييم القيم الحرفية:** عند مواجهة قيمة حرفية مثل رقم أو نص، يتم تحويلها مباشرة إلى قيمة في الذاكرة:

```cpp
// من expression_evaluator.cpp
void ExpressionEvaluator::visitLiteralExpr(LiteralExpr& node) {
    lastResult_ = tokenToValue(node.token);
}

Value ExpressionEvaluator::tokenToValue(const Token& token) {
    switch (token.getType()) {
        case TokenType::NUMBER_INTEGER:
            return Value(std::stoi(token.getValue()));
        
        case TokenType::NUMBER_DOUBLE:
            return Value(std::stod(token.getValue()));
        
        case TokenType::STRING_LITERAL:
            return Value(token.getValue());
        
        case TokenType::LITERAL_TRUE:
            return Value(true);
        
        case TokenType::LITERAL_FALSE:
            return Value(false);
        
        // ...
    }
}
```

**تقييم المتغيرات:** عند الوصول إلى متغير، يبحث المفسر في مدير المتغيرات عن قيمته الحالية:

```cpp
void ExpressionEvaluator::visitVariableExpr(VariableExpr& node) {
    // التحقق من وجود المتغير
    if (!variableManager_.exists(node.name)) {
        // التحقق من وجود صنف بهذا الاسم
        // ...
        
        // متغير غير معرّف - إرسال خطأ
        ErrorManager::getInstance().reportError(
            ErrorCode::SEM_UNDEFINED_VARIABLE,
            // ...
            "متغير غير معرّف: " + node.name
        );
        return;
    }
    lastResult_ = variableManager_.get(node.name);
}
```

**تقييم التعابير الثنائية:** للعمليات الحسابية والمنطقية، يقوم المفسر بتقييم الطرفين ثم تطبيق العملية:

```cpp
void ExpressionEvaluator::visitBinaryExpr(BinaryExpr& node) {
    // تقييم الطرف الأيسر
    node.left->accept(*this);
    Value left = lastResult_;
    
    // تقييم الطرف الأيمن
    node.right->accept(*this);
    Value right = lastResult_;
    
    // تطبيق العملية حسب نوعها
    switch (node.op) {
        case TokenType::OP_PLUS:
            // جمع الأرقام أو ضم النصوص
            // ...
            break;
        case TokenType::OP_MINUS:
            // طرح
            break;
        // ...
    }
}
```

### منفذ العبارات (Statement Executor)

منفذ العبارات مسؤول عن تنفيذ العبارات التي لا تُنتج قيماً مباشرة، مثل التصريحات والحلقات والشروط.

**تنفيذ الشروط:** عند مواجهة عبارة `إذا`، يقوم المفسر بتقييم الشرط أولاً، ثم ينفذ الكتلة المناسبة:

```cpp
void StatementExecutor::visitIfStmt(IfStmt& stmt) {
    // تقييم الشرط
    stmt.condition->accept(evaluator_);
    Value condition = evaluator_.getLastResult();
    
    if (condition.toBoolean()) {
        // تنفيذ كتلة "إذا"
        stmt.thenBranch->accept(*this);
    } else if (stmt.elseBranch) {
        // تنفيذ كتلة "وإلا"
        stmt.elseBranch->accept(*this);
    }
}
```

**تنفيذ الحلقات:** الحلقات تتطلب تكرار تنفيذ كتلة الكود طالما الشرط صحيح:

```cpp
void StatementExecutor::visitWhileStmt(WhileStmt& stmt) {
    while (true) {
        // تقييم الشرط
        stmt.condition->accept(evaluator_);
        Value condition = evaluator_.getLastResult();
        
        if (!condition.toBoolean()) {
            break;  // الشرط خاطئ، اخرج من الحلقة
        }
        
        // تنفيذ جسم الحلقة
        stmt.body->accept(*this);
        
        // التعامل مع break و continue
        if (shouldBreak_) {
            shouldBreak_ = false;
            break;
        }
        if (shouldContinue_) {
            shouldContinue_ = false;
            continue;
        }
    }
}
```

### مدير المتغيرات (Variable Manager)

مدير المتغيرات يتعامل مع تخزين واسترجاع قيم المتغيرات، مع دعم النطاقات المتداخلة. عند دخول نطاق جديد (مثل دالة أو كتلة)، يتم إنشاء بيئة جديدة. عند الخروج، يتم التخلص من المتغيرات المحلية.

```cpp
// تعريف متغير جديد
void VariableManager::define(const std::string& name, const Value& value) {
    currentScope_->variables[name] = value;
}

// الحصول على قيمة متغير
Value VariableManager::get(const std::string& name) {
    // البحث من النطاق الحالي إلى النطاق العام
    Scope* scope = currentScope_;
    while (scope) {
        auto it = scope->variables.find(name);
        if (it != scope->variables.end()) {
            return it->second;
        }
        scope = scope->parent;
    }
    throw RuntimeError("متغير غير معرّف: " + name);
}
```

## دعم البرمجة الكائنية

المفسر يدعم البرمجة الكائنية بشكل كامل، بما في ذلك الأصناف والوراثة والواجهات.

**إنشاء الكائنات:** عند استخدام `جديد`، يقوم المفسر بـ:
1. إنشاء كائن جديد في الذاكرة
2. تعيين الخصائص الافتراضية
3. استدعاء المُنشئ مع الوسائط

**الوصول للأعضاء:** عند الوصول لخاصية أو طريقة، يبحث المفسر في الكائن، ثم في الصنف الأب إذا لزم الأمر.

**الكلمة المفتاحية `هذا`:** داخل الطرق، `هذا` يشير إلى الكائن الحالي:

```cpp
void ExpressionEvaluator::visitThisExpr(ThisExpr& node) {
    if (variableManager_.exists("هذا")) {
        lastResult_ = variableManager_.get("هذا");
    } else {
        ErrorManager::getInstance().reportError(
            // خطأ: استخدام 'هذا' خارج سياق الصنف
        );
    }
}
```

## الدوال المدمجة (Builtins)

المفسر يأتي مع مجموعة من الدوال المدمجة للعمليات الشائعة:

- **اطبع():** طباعة قيمة على الشاشة
- **ادخل():** قراءة مدخلات من المستخدم
- **طول():** الحصول على طول نص أو قائمة
- **نوع():** معرفة نوع قيمة
- **رقم()، نص()، عشري():** تحويل الأنواع

## معالجة الأخطاء

المفسر يوفر رسائل خطأ واضحة بالعربية والإنجليزية:

```cpp
ErrorManager::getInstance().reportError(
    ErrorCode::SEM_UNDEFINED_VARIABLE,
    SourceLocation("<input>", line, column),
    "متغير غير معرّف: " + name,           // بالعربية
    "Undefined variable: " + name          // بالإنجليزية
);
```

## مميزات المفسر

- **التنفيذ الفوري:** لا حاجة لانتظار الترجمة
- **التصحيح السهل:** يمكن فحص حالة البرنامج في أي لحظة
- **الرسائل الواضحة:** أخطاء مفصلة بالعربية
- **المرونة:** يمكن تعديل الكود وإعادة تشغيله فوراً
- **دعم REPL:** تفاعل مباشر مع اللغة

## متى تستخدم المفسر؟

- **التعلم والتجريب:** جرب أفكاراً سريعاً
- **التطوير:** كتابة وتجربة الكود
- **التصحيح:** فهم سلوك البرنامج خطوة بخطوة
- **REPL:** تفاعل مباشر مع اللغة
- **السكريبتات الصغيرة:** برامج لا تحتاج أداءً عالياً

## الخلاصة

المفسر في لغة ص هو أداة قوية للتطوير السريع والتعلم. يوفر تنفيذاً مباشراً للكود مع رسائل خطأ واضحة ودعم كامل لميزات اللغة. على الرغم من أنه أبطأ من الكود المترجم، إلا أن سرعته في دورة التطوير تجعله الخيار المفضل أثناء كتابة البرنامج وتصحيحه.
