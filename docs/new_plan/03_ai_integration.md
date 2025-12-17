# 🤖 تكامل الذكاء الاصطناعي / AI Integration Plan

**المرحلة / Phase:** 5  
**المدة / Duration:** 6-12 شهر / months  
**الأولوية / Priority:** P1 (مهمة / Important)

---

## 📋 نظرة عامة / Overview

### (AR) الوصف
دمج كامل للذكاء الاصطناعي في لغة ص يشمل: توليد كود من العربية الطبيعية، اكتشاف وإصلاح أخطاء ذكي، اقتراحات سياقية، وتحسين مستمر بناءً على استخدام المطور.

### (EN) Description
Full AI integration for Sad language including: Arabic natural language to code generation, smart error detection and fixing, context-aware suggestions, and continuous improvement based on developer usage.

---

## 🎯 الأهداف / Objectives

### الأهداف الرئيسية

1. **توليد كود من العربية (NL→Code)**
   - تحويل وصف عربي إلى كود
   - دعم السياق والتفاعل
   - دقة عالية (70%+)

2. **اكتشاف وإصلاح الأخطاء الذكي**
   - كشف أخطاء نحوية ودلالية
   - اقتراح إصلاحات تلقائية
   - تفسيرات واضحة

3. **اقتراحات سياقية**
   - إكمال تلقائي ذكي
   - refactoring suggestions
   - أنماط أفضل (best practices)

4. **التعلم المستمر**
   - RLHF من feedback المطورين
   - Personalization حسب الأسلوب
   - Federated learning للخصوصية

---

## 🏗️ المعمارية / Architecture

### نظرة عامة على المكونات

```
┌──────────────────────────────────────────────────────┐
│                    s_ide (Client)                    │
│  ├─ Editor UI                                        │
│  ├─ AI Suggestions Panel                            │
│  └─ Telemetry (opt-in)                              │
└────────────┬────────────────────────────────────────┘
             │ HTTP/WebSocket
┌────────────▼─────────────────────────────────────────┐
│              s_ai (AI Service)                        │
├──────────────────────────────────────────────────────┤
│  API Gateway                                         │
│  ├─ /generate - توليد كود                           │
│  ├─ /fix - إصلاح أخطاء                             │
│  ├─ /suggest - اقتراحات                            │
│  └─ /analyze - تحليل                                │
├──────────────────────────────────────────────────────┤
│  Context Analyzer                                    │
│  ├─ Code Context Extractor                          │
│  ├─ Symbol Table Builder                            │
│  ├─ Type Inference                                   │
│  └─ Semantic Analyzer                                │
├──────────────────────────────────────────────────────┤
│  NL→Code Generator                                   │
│  ├─ Arabic NLP Pipeline                             │
│  ├─ Intent Classifier                               │
│  ├─ Code Template Matcher                           │
│  └─ Code Generator (LLM)                            │
├──────────────────────────────────────────────────────┤
│  Error Detector & Fixer                             │
│  ├─ Syntax Error Detector                           │
│  ├─ Semantic Error Detector                         │
│  ├─ Type Error Detector                             │
│  └─ Auto-Fix Generator                              │
├──────────────────────────────────────────────────────┤
│  Learning Engine                                     │
│  ├─ RLHF Trainer                                    │
│  ├─ Personalization Model                           │
│  └─ Federated Learning Aggregator                  │
├──────────────────────────────────────────────────────┤
│  Model Serving                                       │
│  ├─ Local Models (quantized)                        │
│  ├─ Cloud Models (GPT-4, Claude)                   │
│  └─ Caching Layer                                    │
└──────────────────────────────────────────────────────┘
```

---

## 📂 المشروع / Project Structure

```
s_ai/
├── CMakeLists.txt
├── README.md
├── requirements.txt          # Python dependencies
├── config/
│   ├── models.yaml          # Model configurations
│   ├── prompts.yaml         # Prompt templates
│   └── endpoints.yaml       # API endpoints
├── include/                 # C++ headers (if needed)
│   └── ai_client.h
├── src/                     # C++ source (if needed)
│   └── ai_client.cpp
├── python/                  # Python AI service
│   ├── main.py             # FastAPI server
│   ├── api/
│   │   ├── __init__.py
│   │   ├── generate.py     # NL→Code endpoint
│   │   ├── fix.py          # Error fixing endpoint
│   │   ├── suggest.py      # Suggestions endpoint
│   │   └─ analyze.py       # Code analysis endpoint
│   ├── services/
│   │   ├── context_analyzer.py
│   │   ├── nl_code_generator.py
│   │   ├── error_detector.py
│   │   ├── error_fixer.py
│   │   └── learning_engine.py
│   ├── models/
│   │   ├── arabic_tokenizer.py
│   │   ├── intent_classifier.py
│   │   ├── code_llm.py
│   │   └── personalization.py
│   └── utils/
│       ├── prompt_builder.py
│       ├── cache.py
│       └── telemetry.py
├── tests/
│   ├── test_generate.py
│   ├── test_fix.py
│   ├── test_suggest.py
│   └── integration_tests.py
├── models/                  # Model weights (gitignored)
│   ├── tokenizer/
│   ├── intent_classifier/
│   └── code_llm/
└── data/                    # Training/eval data
    ├── samples/
    ├── feedback/
    └── metrics/
```

---

## 🚀 المراحل / Implementation Phases

### المرحلة 5.1: نواة AI (3 أشهر)

#### 5.1.1 Context Analyzer (شهر 1)

**الوصف:**
- تحليل السياق البرمجي
- استخراج Symbol table
- Type inference
- Semantic information

**التنفيذ:**
```python
# python/services/context_analyzer.py
class ContextAnalyzer:
    def analyze(self, code: str, cursor_position: int) -> Context:
        """تحليل السياق في موضع المؤشر"""
        ast = self.parser.parse(code)
        symbols = self.extract_symbols(ast)
        types = self.infer_types(ast, symbols)
        scope = self.get_scope(ast, cursor_position)
        
        return Context(
            ast=ast,
            symbols=symbols,
            types=types,
            current_scope=scope,
            available_symbols=self.get_available_symbols(scope)
        )
    
    def extract_symbols(self, ast) -> SymbolTable:
        """استخراج جدول الرموز"""
        visitor = SymbolTableVisitor()
        ast.accept(visitor)
        return visitor.symbol_table
```

**المهام:**
- [ ] AST integration مع s_language
- [ ] Symbol table extraction
- [ ] Type inference engine
- [ ] Scope analysis
- [ ] اختبارات شاملة

---

#### 5.1.2 Arabic NLP Pipeline (شهر 1)

**الوصف:**
- معالجة النصوص العربية
- Intent classification
- Entity extraction

**التنفيذ:**
```python
# python/models/arabic_tokenizer.py
class ArabicTokenizer:
    def __init__(self):
        self.tokenizer = load_pretrained_tokenizer('aubmindlab/bert-base-arabertv2')
    
    def tokenize(self, text: str) -> List[str]:
        """تقسيم النص العربي"""
        return self.tokenizer.tokenize(text)
    
    def normalize(self, text: str) -> str:
        """تطبيع النص العربي"""
        # إزالة التشكيل
        text = remove_diacritics(text)
        # توحيد الحروف
        text = normalize_arabic(text)
        return text

# python/models/intent_classifier.py
class IntentClassifier:
    INTENTS = [
        'create_function',    # إنشاء دالة
        'create_class',       # إنشاء صنف
        'create_variable',    # إنشاء متغير
        'create_loop',        # إنشاء حلقة
        'create_condition',   # إنشاء شرط
        'modify_code',        # تعديل كود
        'explain_code',       # شرح كود
    ]
    
    def classify(self, text: str) -> Intent:
        """تصنيف النية من النص"""
        normalized = self.tokenizer.normalize(text)
        features = self.extract_features(normalized)
        intent = self.model.predict(features)
        confidence = self.model.predict_proba(features).max()
        
        return Intent(
            type=intent,
            confidence=confidence,
            entities=self.extract_entities(normalized)
        )
```

**المهام:**
- [ ] Arabic tokenizer
- [ ] Intent classifier training
- [ ] Entity extraction
- [ ] Template matching
- [ ] اختبارات دقة

---

#### 5.1.3 Basic Code Generator (شهر 1)

**الوصف:**
- توليد كود من templates
- LLM integration بسيط
- Validation

**التنفيذ:**
```python
# python/services/nl_code_generator.py
class NLCodeGenerator:
    def __init__(self):
        self.intent_classifier = IntentClassifier()
        self.template_matcher = TemplateMatcher()
        self.llm = CodeLLM()
    
    def generate(self, 
                prompt: str, 
                context: Context,
                mode: str = 'hybrid') -> GeneratedCode:
        """توليد كود من وصف عربي"""
        
        # 1. تحليل النية
        intent = self.intent_classifier.classify(prompt)
        
        # 2. محاولة template أولاً (سريع)
        if mode == 'template' or mode == 'hybrid':
            template_code = self.template_matcher.match(intent, context)
            if template_code:
                return GeneratedCode(
                    code=template_code,
                    method='template',
                    confidence=intent.confidence
                )
        
        # 3. استخدام LLM (دقيق لكن بطيء)
        if mode == 'llm' or mode == 'hybrid':
            llm_prompt = self.build_prompt(prompt, intent, context)
            generated = self.llm.generate(llm_prompt)
            validated = self.validate(generated, context)
            
            return GeneratedCode(
                code=validated,
                method='llm',
                confidence=0.8  # من LLM
            )
        
        raise ValueError(f"No generation method succeeded")
    
    def build_prompt(self, user_prompt: str, 
                    intent: Intent, 
                    context: Context) -> str:
        """بناء prompt لـ LLM"""
        return f"""
أنت مساعد برمجة ذكي للغة ص.

السياق:
{context.to_string()}

المهمة:
{user_prompt}

النية المكتشفة: {intent.type}

تعليمات:
- اكتب كود بلغة ص
- اتبع أسلوب السياق المعطى
- استخدم أسماء عربية واضحة
- أضف تعليقات مفيدة

الكود:
```sad
"""
```

**أمثلة Prompts:**
```
# مثال 1: دالة بسيطة
"اكتب دالة لحساب مجموع رقمين"

→ Generated:
دالة جمع(أ: رقم، ب: رقم) -> رقم {
    أرجع أ + ب
}

# مثال 2: حلقة
"اطبع الأرقام من 1 إلى 10"

→ Generated:
لكل رقم في نطاق(1, 11) {
    اطبع(رقم)
}

# مثال 3: صنف
"أنشئ صنف شخص مع اسم وعمر"

→ Generated:
صنف شخص {
    خاص الاسم: نص
    خاص العمر: رقم
    
    منشئ(اسم: نص، عمر: رقم) {
        هذا.الاسم = اسم
        هذا.العمر = عمر
    }
    
    عام دالة معلومات() -> نص {
        أرجع "الاسم: " + هذا.الاسم + "، العمر: " + نص(هذا.العمر)
    }
}
```

**المهام:**
- [ ] Template system
- [ ] LLM integration (OpenAI/local)
- [ ] Prompt engineering
- [ ] Code validation
- [ ] Interactive refinement
- [ ] اختبارات

---

### المرحلة 5.2: Error Detection & Fixing (2 أشهر)

#### 5.2.1 Error Detector (شهر 1)

**التنفيذ:**
```python
# python/services/error_detector.py
class ErrorDetector:
    def detect(self, code: str) -> List[Error]:
        """كشف جميع أنواع الأخطاء"""
        errors = []
        
        # 1. أخطاء نحوية
        syntax_errors = self.detect_syntax_errors(code)
        errors.extend(syntax_errors)
        
        # 2. أخطاء دلالية
        if not syntax_errors:
            semantic_errors = self.detect_semantic_errors(code)
            errors.extend(semantic_errors)
        
        # 3. أخطاء أنواع
        type_errors = self.detect_type_errors(code)
        errors.extend(type_errors)
        
        # 4. تحذيرات (code smells)
        warnings = self.detect_code_smells(code)
        errors.extend(warnings)
        
        return errors
    
    def detect_syntax_errors(self, code: str) -> List[SyntaxError]:
        """كشف أخطاء نحوية"""
        try:
            ast = self.parser.parse(code)
            return []
        except ParseError as e:
            return [SyntaxError(
                message=e.message,
                line=e.line,
                column=e.column,
                severity='error'
            )]
    
    def detect_semantic_errors(self, code: str) -> List[SemanticError]:
        """كشف أخطاء دلالية"""
        ast = self.parser.parse(code)
        analyzer = SemanticAnalyzer()
        return analyzer.analyze(ast)
    
    def detect_code_smells(self, code: str) -> List[Warning]:
        """كشف code smells"""
        smells = []
        
        # متغيرات غير مستخدمة
        unused = self.find_unused_variables(code)
        smells.extend(unused)
        
        # دوال طويلة جداً
        long_funcs = self.find_long_functions(code)
        smells.extend(long_funcs)
        
        # تعقيد دوري عالي
        complex = self.find_complex_code(code)
        smells.extend(complex)
        
        return smells
```

---

#### 5.2.2 Auto-Fixer (شهر 1)

**التنفيذ:**
```python
# python/services/error_fixer.py
class AutoFixer:
    def fix(self, code: str, error: Error) -> List[Fix]:
        """توليد إصلاحات محتملة"""
        
        if error.type == 'syntax':
            return self.fix_syntax_error(code, error)
        elif error.type == 'semantic':
            return self.fix_semantic_error(code, error)
        elif error.type == 'type':
            return self.fix_type_error(code, error)
        else:
            return []
    
    def fix_syntax_error(self, code: str, error: SyntaxError) -> List[Fix]:
        """إصلاح أخطاء نحوية"""
        fixes = []
        
        # مثال: قوس ناقص
        if 'missing' in error.message and 'بrace' in error.message:
            fixed_code = self.add_missing_brace(code, error.line)
            fixes.append(Fix(
                description="إضافة قوس ناقص",
                code=fixed_code,
                confidence=0.9
            ))
        
        # مثال: فاصلة منقوطة ناقصة
        if 'semicolon' in error.message:
            fixed_code = self.add_semicolon(code, error.line)
            fixes.append(Fix(
                description="إضافة فاصلة منقوطة",
                code=fixed_code,
                confidence=0.95
            ))
        
        return fixes
    
    def fix_type_error(self, code: str, error: TypeError) -> List[Fix]:
        """إصلاح أخطاء أنواع"""
        fixes = []
        
        # مثال: تحويل نوع
        if 'type mismatch' in error.message:
            # اقتراح تحويل صريح
            fixed = self.add_type_conversion(code, error)
            fixes.append(Fix(
                description=f"تحويل من {error.found} إلى {error.expected}",
                code=fixed,
                confidence=0.8
            ))
        
        return fixes
```

**المهام:**
- [ ] Syntax error fixes
- [ ] Semantic error fixes
- [ ] Type error fixes
- [ ] Multiple fix suggestions
- [ ] Fix ranking
- [ ] اختبارات

---

### المرحلة 5.3: Learning Engine (2 أشهر)

#### 5.3.1 RLHF Training

**الوصف:**
- جمع feedback من المطورين
- Fine-tuning Models
- A/B testing

**التنفيذ:**
```python
# python/services/learning_engine.py
class LearningEngine:
    def record_feedback(self, 
                       suggestion_id: str,
                       accepted: bool,
                       modified: Optional[str] = None):
        """تسجيل feedback المطور"""
        feedback = Feedback(
            id=suggestion_id,
            accepted=accepted,
            modified_code=modified,
            timestamp=datetime.now()
        )
        self.feedback_db.save(feedback)
    
    def train_from_feedback(self, batch_size: int = 100):
        """تدريب من feedback"""
        feedbacks = self.feedback_db.get_recent(batch_size)
        
        # تحضير بيانات training
        training_data = []
        for fb in feedbacks:
            if fb.accepted or fb.modified_code:
                training_data.append({
                    'prompt': fb.original_prompt,
                    'context': fb.context,
                    'target': fb.modified_code or fb.generated_code,
                    'reward': 1.0 if fb.accepted else 0.5
                })
        
        # Fine-tune model
        self.model.fine_tune(training_data)
```

---

#### 5.3.2 Personalization

**الوصف:**
- تعلم أسلوب المطور
- تفضيلات التسمية
- أنماط الكود المفضلة

**التنفيذ:**
```python
# python/models/personalization.py
class PersonalizationModel:
    def learn_style(self, user_id: str, code_samples: List[str]):
        """تعلم أسلوب المستخدم"""
        style = CodeStyle()
        
        # تحليل أسلوب التسمية
        style.naming_convention = self.extract_naming_style(code_samples)
        
        # تحليل تفضيلات التنسيق
        style.indentation = self.extract_indentation(code_samples)
        style.brace_style = self.extract_brace_style(code_samples)
        
        # تحليل الأنماط المفضلة
        style.preferred_patterns = self.extract_patterns(code_samples)
        
        self.user_styles[user_id] = style
    
    def apply_style(self, code: str, user_id: str) -> str:
        """تطبيق أسلوب المستخدم"""
        if user_id not in self.user_styles:
            return code
        
        style = self.user_styles[user_id]
        
        # تطبيق التنسيق
        code = self.format_code(code, style)
        
        # تطبيق أنماط التسمية
        code = self.apply_naming_convention(code, style)
        
        return code
```

**المهام:**
- [ ] Style extraction
- [ ] Preference learning
- [ ] Pattern recognition
- [ ] Style application
- [ ] اختبارات

---

### المرحلة 5.4: Production Deployment (1 شهر)

#### API Service

**FastAPI Server:**
```python
# python/main.py
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

app = FastAPI(title="Sad AI Service")

class GenerateRequest(BaseModel):
    prompt: str
    context: dict
    mode: str = 'hybrid'

class GenerateResponse(BaseModel):
    code: str
    method: str
    confidence: float
    alternatives: List[str] = []

@app.post("/api/v1/generate")
async def generate_code(req: GenerateRequest) -> GenerateResponse:
    """توليد كود من وصف عربي"""
    try:
        generator = NLCodeGenerator()
        context = Context.from_dict(req.context)
        result = generator.generate(req.prompt, context, req.mode)
        
        return GenerateResponse(
            code=result.code,
            method=result.method,
            confidence=result.confidence
        )
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/v1/fix")
async def fix_errors(code: str) -> List[Fix]:
    """إصلاح أخطاء تلقائي"""
    detector = ErrorDetector()
    fixer = AutoFixer()
    
    errors = detector.detect(code)
    all_fixes = []
    
    for error in errors:
        fixes = fixer.fix(code, error)
        all_fixes.extend(fixes)
    
    return all_fixes

@app.post("/api/v1/suggest")
async def get_suggestions(code: str, cursor: int) -> List[Suggestion]:
    """اقتراحات ذكية"""
    analyzer = ContextAnalyzer()
    suggester = SmartSuggester()
    
    context = analyzer.analyze(code, cursor)
    suggestions = suggester.suggest(context)
    
    return suggestions
```

**المهام:**
- [ ] FastAPI server
- [ ] API documentation
- [ ] Rate limiting
- [ ] Caching
- [ ] Monitoring
- [ ] Docker deployment

---

## 📊 مقاييس النجاح / Success Metrics

### الأداء (Performance)
- ✅ توليد كود: <500ms (local), <2s (cloud)
- ✅ Error detection: <100ms
- ✅ Suggestions: <200ms

### الدقة (Accuracy)
- ✅ NL→Code: 70%+ acceptance rate
- ✅ Error fixes: 80%+ correct
- ✅ Suggestions: 60%+ used

### تجربة المستخدم (UX)
- ✅ User satisfaction: 4/5 stars
- ✅ Daily active usage: 50%+
- ✅ Feedback submission: 20%+

---

## 🔐 الخصوصية والأمان / Privacy & Security

### مبادئ الخصوصية

1. **محلي أولاً (Local First)**
   - Models محلية مُكمّمة (quantized)
   - لا إرسال كود بدون إذن

2. **موافقة صريحة (Explicit Consent)**
   - خيار واضح لاستخدام cloud
   - شفافية في استخدام البيانات

3. **التشفير (Encryption)**
   - تشفير البيانات المرسلة
   - عدم تخزين كود حساس

4. **Federated Learning**
   - تدريب موزع
   - عدم مركزية البيانات

---

**آخر تحديث:** 29 نوفمبر 2025  
**الحالة:** ✅ خطة جاهزة للتنفيذ
