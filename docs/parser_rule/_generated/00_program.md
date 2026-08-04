# قواعد المحلل — طبقة النواة (البرنامج)

> ⚠️ **هذا الملف مُولَّد آليًّا** من `language-truth/grammar/*.yaml` عبر
> `scripts/codegen/gen_parser_grammar_docs.py` — **لا تُحرِّره يدويًّا**.
> عدّل YAML المصدر ثم أعد التوليد.


- **الطبقة:** `program` · **ملف المصدر:** `language-truth/grammar/00_program.yaml`
- **الوصف:** البنية العليا — البرنامج، التصريح الأعلى، الجملة، الكتلة
- **عدد القواعد:** 4

> **قراءة المخطّطات:** «📊 مخطّط البنية النحويّة» يُظهر تسلسل الرموز (تكرار «تكرار»، اختياري «تخطّي»، بدائل ◆). «مخطّط مسار الدوال» يُظهر دوال المحلل التي تُستدعى حتى بناء عقدة AST.

## نظرة عامّة — مسار دوال الطبقة
> الاستدعاءات الداخليّة بين دوال قواعد هذه الطبقة (الروابط عبر الطبقات مذكورة في كل قاعدة).
```mermaid
flowchart TD
  o1["برنامج<br/>parseProgram()"]
  o2["تصريح<br/>parseDeclaration()"]
  o3["جملة<br/>parseStatement()"]
  o4["كتلة<br/>parseBlockStmt()"]
  o1 --> o2
  o2 --> o3
  o3 --> o4
  o4 --> o3
```

---

<a id="gr.program.program"></a>
### gr.program.program — برنامج <span dir="ltr">(Program)</span>

- **الرقم التسلسليّ:** `ق-001` · **المعرّف الموحَّد:** `gr.program.program` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** نقطة الجذر — يقرأ المُحلِّل تصريحات متتالية حتى نهاية الملف

#### 📐 BNF
```bnf
Program = { Declaration } EOF ;
```

#### 🧩 تفصيل البدائل
- `{ declaration } «<نهاية الملف>»`

#### 🔻 المسار إلى المحلل (دوال التحليل) ⇒ AST
**دالة (دوال) الدخول:**
1. [`ParserCore::parseProgram`](../../../shared/parser/src/core/parser_main.cpp) — `shared/parser/src/core/parser_main.cpp`
- **عقدة AST المُنتَجة:** `StmtList`
- **يستدعي دوال:** [`parseDeclaration`](00_program.md#gr.program.declaration)

##### مخطّط مسار الدوال (حتى AST)
```mermaid
flowchart TD
  f1["parseProgram()"]
  f2["parseDeclaration()"]
  f1 -- "تصريح" --> f2
  f3(["⇒ StmtList"])
  f1 --> f3
```

#### 📊 مخطّط البنية النحويّة (Mermaid)
```mermaid
flowchart LR
  n1(["برنامج"])
  n2{"◇"}
  n3{"◇"}
  n4["declaration"]
  n2 --> n4
  n4 --> n3
  n4 -- "تكرار" --> n4
  n2 -- "صفر/أكثر" --> n3
  n5["«<نهاية الملف>»"]
  n3 --> n5
  n1 --> n2
  n6(["⇒ StmtList"])
  n5 --> n6
```

#### مثال
```sad
متغير س = 1
دالة جمع(أ، ب) ارجع أ + ب نهاية
```

---

<a id="gr.program.declaration"></a>
### gr.program.declaration — تصريح <span dir="ltr">(Declaration)</span>

- **الرقم التسلسليّ:** `ق-002` · **المعرّف الموحَّد:** `gr.program.declaration` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** المُحلِّل يميّز التصريح بالكلمة المفتاحية الأولى؛ وإلا يُفوّض للجملة. قائمة alternatives أعلاه هي الإنتاجات القانونية الأساسية، وليست حصراً لفروع الموزّع: الموزّع الفعلي يوجّه أيضاً — بكلمات سياقية — إلى إنتاجات موثّقة في طبقتها: القالب (gr.adv.template_decl)، الواجهة (gr.adv.ui_decl)، الامتداد (gr.oop.extension)، الماكرو (gr.adv.macro)، العقد (gr.adv.contract)، الاختبار (gr.adv.property_test)، عرض عنصر الواجهة (gr.adv.widget)، والتصريح الخارجي (gr.decl.extern). وثلاثُ بنى حيّة **بلا قاعدة SoT بعد** (فجوة مُسجَّلة، موطنها ملفّا 20/60): فضاء الأسماء («فضاء … نهاية»)، الاسم المستعار للنوع («نوع اسم = هدف»)، والتوجيهات الكتلية «@غير_آمن/@وقت_الترجمة/@متطاير» وسمات «[[…]]». كما يستهلك الموزّع صيغاً قديمة ويرفضها برسالة توجيهية (غير_متزامن دالة، دالة مولّد، مجرد/محكم قبل صنف) — حواجز هجرة لا قواعد لغة.

#### 📐 BNF
```bnf
Declaration = FunctionDecl | ClassDecl | StructDecl | EnumDecl
            | TraitDecl | ImplDecl | ImportDecl | ExportDecl
            | VarDecl | Statement ;
```

#### 🧩 تفصيل البدائل
**1.** *تصريحات معلَنة:* `( function | class | struct | enum | trait | impl | import | export | variable )`
**2.** *جملة عادية:* `statement`

#### 🔻 المسار إلى المحلل (دوال التحليل) ⇒ AST
**دالة (دوال) الدخول:**
1. [`ParserCore::parseDeclaration`](../../../shared/parser/src/core/parser_main.cpp) — `shared/parser/src/core/parser_main.cpp`
- **عقدة AST المُنتَجة:** `—`
- **يستدعي دوال:** [`parseFunctionDecl`](20_declarations.md#gr.decl.function)، [`parseClassDecl`](30_oop.md#gr.oop.class)، [`parseStructDecl`](30_oop.md#gr.oop.struct)، [`parseEnumDecl`](30_oop.md#gr.oop.enum)، [`parseTraitDecl`](30_oop.md#gr.oop.trait)، [`parseImplDecl`](30_oop.md#gr.oop.impl)، [`parseImportStmt`](20_declarations.md#gr.decl.import)، [`parseExportDecl`](20_declarations.md#gr.decl.export)، [`parseVarDecl`](20_declarations.md#gr.decl.variable)، [`parseStatement`](00_program.md#gr.program.statement)
- **مُستدعى من:** [`parseProgram`](00_program.md#gr.program.program)، [`parseExportDecl`](20_declarations.md#gr.decl.export)، [`parseGoStmt`](60_advanced.md#gr.adv.go)

##### مخطّط مسار الدوال (حتى AST)
```mermaid
flowchart TD
  f1["parseDeclaration()"]
  f2["parseFunctionDecl()"]
  f1 -- "تصريح دالة" --> f2
  f3["parseClassDecl()"]
  f1 -- "تصريح صنف" --> f3
  f4["parseStructDecl()"]
  f1 -- "تصريح بنية" --> f4
  f5["parseEnumDecl()"]
  f1 -- "تصريح تعداد" --> f5
  f6["parseTraitDecl()"]
  f1 -- "تصريح سمة" --> f6
  f7["parseImplDecl()"]
  f1 -- "كتلة تنفيذ" --> f7
  f8["parseImportStmt()"]
  f1 -- "تصريح استيراد" --> f8
  f9["parseExportDecl()"]
  f1 -- "تصريح تصدير" --> f9
  f10["parseVarDecl()"]
  f1 -- "تصريح متغير" --> f10
  f11["parseStatement()"]
  f1 -- "جملة" --> f11
  f12(["⇒ AST"])
  f1 --> f12
```

#### 📊 مخطّط البنية النحويّة (Mermaid)
```mermaid
flowchart LR
  n1(["تصريح"])
  n2{"◆"}
  n3{"◆"}
  n4["function"]
  n2 --> n4
  n4 --> n3
  n5["class"]
  n2 --> n5
  n5 --> n3
  n6["struct"]
  n2 --> n6
  n6 --> n3
  n7["enum"]
  n2 --> n7
  n7 --> n3
  n8["trait"]
  n2 --> n8
  n8 --> n3
  n9["impl"]
  n2 --> n9
  n9 --> n3
  n10["import"]
  n2 --> n10
  n10 --> n3
  n11["export"]
  n2 --> n11
  n11 --> n3
  n12["variable"]
  n2 --> n12
  n12 --> n3
  n1 -- "تصريحات معلَنة" --> n2
  n13(["⇒ AST"])
  n3 --> n13
  n14["statement"]
  n1 -- "جملة عادية" --> n14
  n15(["⇒ AST"])
  n14 --> n15
```

---

<a id="gr.program.statement"></a>
### gr.program.statement — جملة <span dir="ltr">(Statement)</span>

- **الرقم التسلسليّ:** `ق-003` · **المعرّف الموحَّد:** `gr.program.statement` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** موزّع الجمل التنفيذية — يختار حسب الرمز الحالي. قائمة alternatives أعلاه أساسية وليست حصراً: الموزّع الفعلي يوجّه أيضاً — بكلمات سياقية — إلى جمل موثّقة في طبقتها: الحالة/switch (gr.stmt.switch)، أنتِج (gr.adv.yield)، باستخدام (gr.adv.with)، أجّل (gr.adv.defer)، أطلِق (gr.adv.go)، اختر (gr.adv.select)، ارمِ (gr.stmt.throw). وله فرعٌ خاصٌّ يفكّ التباس «{» على مستوى الجملة بين كتلة وخريطة حرفية واستيعاب قاموس/مجموعة (يفوّض إلى gr.expr.map_literal) — سلوكٌ حيّ يُوثَّق تفصيلاً في طبقة التعبيرات (40).

#### 📐 BNF
```bnf
Statement = IfStatement | WhileStatement | ForStatement | MatchStatement
          | TryStatement | ReturnStatement | BreakStatement | ContinueStatement
          | BlockStatement | ExpressionStatement ;
```

#### 🧩 تفصيل البدائل
- `( if | while | for | match | try | return | break | continue | block | expression )`

#### 🔻 المسار إلى المحلل (دوال التحليل) ⇒ AST
**دالة (دوال) الدخول:**
1. [`ParserCore::parseStatement`](../../../shared/parser/src/core/parser_main.cpp) — `shared/parser/src/core/parser_main.cpp`
- **عقدة AST المُنتَجة:** `—`
- **يستدعي دوال:** [`parseIfStmt`](10_statements.md#gr.stmt.if)، [`parseWhileStmt`](10_statements.md#gr.stmt.while)، [`parseForStmt`](10_statements.md#gr.stmt.for)، [`parseMatchStmt`](10_statements.md#gr.stmt.match)، [`parseTryStmt`](10_statements.md#gr.stmt.try)، [`parseReturnStmt`](10_statements.md#gr.stmt.return)، [`parseBreakStmt`](10_statements.md#gr.stmt.break)، [`parseContinueStmt`](10_statements.md#gr.stmt.continue)، [`parseBlockStmt`](00_program.md#gr.program.block)، [`parseExpressionStmt`](10_statements.md#gr.stmt.expression)
- **مُستدعى من:** [`parseDeclaration`](00_program.md#gr.program.declaration)، [`parseBlockStmt`](00_program.md#gr.program.block)، [`parseMatchStmt`](10_statements.md#gr.stmt.match)، [`parseTryStmt`](10_statements.md#gr.stmt.try)، [`parseSwitchStmt`](10_statements.md#gr.stmt.switch)، [`parseWithStmt`](60_advanced.md#gr.adv.with)، [`parseDeferStmt`](60_advanced.md#gr.adv.defer)، [`parseSelectStmt`](60_advanced.md#gr.adv.select)، [`parseMacroDecl`](60_advanced.md#gr.adv.macro)، [`parseTestDecl`](60_advanced.md#gr.adv.property_test)

##### مخطّط مسار الدوال (حتى AST)
```mermaid
flowchart TD
  f1["parseStatement()"]
  f2["parseIfStmt()"]
  f1 -- "جملة إذا" --> f2
  f3["parseWhileStmt()"]
  f1 -- "جملة بينما" --> f3
  f4["parseForStmt()"]
  f1 -- "جملة لكل" --> f4
  f5["parseMatchStmt()"]
  f1 -- "جملة طابق" --> f5
  f6["parseTryStmt()"]
  f1 -- "جملة حاول" --> f6
  f7["parseReturnStmt()"]
  f1 -- "جملة ارجع" --> f7
  f8["parseBreakStmt()"]
  f1 -- "جملة توقف" --> f8
  f9["parseContinueStmt()"]
  f1 -- "جملة استمر" --> f9
  f10["parseBlockStmt()"]
  f1 -- "كتلة" --> f10
  f11["parseExpressionStmt()"]
  f1 -- "جملة تعبير" --> f11
  f12(["⇒ AST"])
  f1 --> f12
```

#### 📊 مخطّط البنية النحويّة (Mermaid)
```mermaid
flowchart LR
  n1(["جملة"])
  n2{"◆"}
  n3{"◆"}
  n4["if"]
  n2 --> n4
  n4 --> n3
  n5["while"]
  n2 --> n5
  n5 --> n3
  n6["for"]
  n2 --> n6
  n6 --> n3
  n7["match"]
  n2 --> n7
  n7 --> n3
  n8["try"]
  n2 --> n8
  n8 --> n3
  n9["return"]
  n2 --> n9
  n9 --> n3
  n10["break"]
  n2 --> n10
  n10 --> n3
  n11["continue"]
  n2 --> n11
  n11 --> n3
  n12["block"]
  n2 --> n12
  n12 --> n3
  n13["expression"]
  n2 --> n13
  n13 --> n3
  n1 --> n2
  n14(["⇒ AST"])
  n3 --> n14
```

---

<a id="gr.program.block"></a>
### gr.program.block — كتلة <span dir="ltr">(Block)</span>

- **الرقم التسلسليّ:** `ق-004` · **المعرّف الموحَّد:** `gr.program.block` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** الكتلة هي العمود الفقري لبنية لغة ص: لا أقواس {} — كل كتلة تنتهي بالكلمة المحجوزة «نهاية». بعض الكتل (مثل if) قد تُغلَق ضمنياً عبر «وإلا» بدل «نهاية».

#### 📐 BNF
```bnf
Block = { Statement } 'نهاية' ;
```

#### 🧩 تفصيل البدائل
- `{ statement } «نهاية»`

#### 🔻 المسار إلى المحلل (دوال التحليل) ⇒ AST
**دالة (دوال) الدخول:**
1. [`ParserCore::parseBlockStmt`](../../../shared/parser/src/statements/parser_statements.cpp) — `shared/parser/src/statements/parser_statements.cpp`
- **عقدة AST المُنتَجة:** `BlockStmt`
- **يستدعي دوال:** [`parseStatement`](00_program.md#gr.program.statement)
- **مُستدعى من:** [`parseStatement`](00_program.md#gr.program.statement)، [`parseIfStmt`](10_statements.md#gr.stmt.if)، [`parseWhileStmt`](10_statements.md#gr.stmt.while)، [`parseForStmt`](10_statements.md#gr.stmt.for)، [`parseFunctionDecl`](20_declarations.md#gr.decl.function)، [`parseMethodDeclaration`](30_oop.md#gr.oop.method)، [`parseConstructorDeclaration`](30_oop.md#gr.oop.constructor)، [`parseDestructorDeclaration`](30_oop.md#gr.oop.destructor)، [`parsePropertyDeclaration`](30_oop.md#gr.oop.property)، [`parseOperatorDecl`](30_oop.md#gr.oop.operator)، [`parseLambda`](40_expressions.md#gr.expr.lambda)
- **روابط المعجم:** كلمات: «نهاية»

##### مخطّط مسار الدوال (حتى AST)
```mermaid
flowchart TD
  f1["parseBlockStmt()"]
  f2["parseStatement()"]
  f1 -- "جملة" --> f2
  f3(["⇒ BlockStmt"])
  f1 --> f3
```

#### 📊 مخطّط البنية النحويّة (Mermaid)
```mermaid
flowchart LR
  n1(["كتلة"])
  n2{"◇"}
  n3{"◇"}
  n4["statement"]
  n2 --> n4
  n4 --> n3
  n4 -- "تكرار" --> n4
  n2 -- "صفر/أكثر" --> n3
  n5["«نهاية»"]
  n3 --> n5
  n1 --> n2
  n6(["⇒ BlockStmt"])
  n5 --> n6
```

#### مثال
```sad
إذا (س > 0)
    اطبع("موجب")
نهاية
```

---
