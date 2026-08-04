# قواعد المحلل — طبقة أنماط المطابقة

> ⚠️ **هذا الملف مُولَّد آليًّا** من `language-truth/grammar/*.yaml` عبر
> `scripts/codegen/gen_parser_grammar_docs.py` — **لا تُحرِّره يدويًّا**.
> عدّل YAML المصدر ثم أعد التوليد.


- **الطبقة:** `patterns` · **ملف المصدر:** `language-truth/grammar/50_patterns.yaml`
- **الوصف:** أنماط المطابقة — شامل/حرفيّ/متغيّر/نطاق (حصريّ وشامل)/عضو تعداد مؤهَّل (unit وADT)/باني غير مؤهَّل (ConstructorPattern)/قائمة/بنية/ربط/بدائل
- **عدد القواعد:** 6

> **قراءة المخطّطات:** «📊 مخطّط البنية النحويّة» يُظهر تسلسل الرموز (تكرار «تكرار»، اختياري «تخطّي»، بدائل ◆). «مخطّط مسار الدوال» يُظهر دوال المحلل التي تُستدعى حتى بناء عقدة AST.

## نظرة عامّة — مسار دوال الطبقة
> الاستدعاءات الداخليّة بين دوال قواعد هذه الطبقة (الروابط عبر الطبقات مذكورة في كل قاعدة).
```mermaid
flowchart TD
  o1["نمط<br/>parsePattern()"]
  o2["نمط أوّليّ<br/>parsePrimaryPattern()"]
  o3["نمط قائمة<br/>parseListPattern()"]
  o4["نمط بنية<br/>parseStructPattern()"]
  o5["نمط ربط<br/>parsePattern()"]
  o6["نمط بدائل<br/>parsePattern()"]
  o1 --> o4
  o1 --> o3
  o1 --> o2
  o2 --> o1
  o3 --> o1
  o4 --> o1
  o5 --> o1
  o6 --> o2
```

---

<a id="gr.pattern.pattern"></a>
### gr.pattern.pattern — نمط <span dir="ltr">(Pattern)</span>

- **الرقم التسلسليّ:** `ق-066` · **المعرّف الموحَّد:** `gr.pattern.pattern` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** موزّع النمط: شامل «_»، بنية «{}»، قائمة «[]»، أوّليّ، ربط «@»، بدائل «|»

#### 📐 BNF
```bnf
Pattern = '_' | StructPattern | ListPattern
        | PrimaryPattern [ '@' Pattern ] | PrimaryPattern { '|' PrimaryPattern } ;
```

#### 🧩 تفصيل البدائل
**1.** *شامل:* `«_»`
**2.** *بنية / قائمة:* `( struct | list )`
**3.** *أوّليّ مع ربط/بدائل:* `primary [ ( ( «@» pattern ) | ( «|» primary )+ ) ]`

#### 🔻 المسار إلى المحلل (دوال التحليل) ⇒ AST
**دالة (دوال) الدخول:**
1. [`ParserCore::parsePattern`](../../../shared/parser/src/statements/parser_advanced.cpp) — `shared/parser/src/statements/parser_advanced.cpp`
- **عقدة AST المُنتَجة:** `WildcardPattern | StructPattern | ListPattern | BindingPattern | OrPattern`
- **يستدعي دوال:** [`parseStructPattern`](50_patterns.md#gr.pattern.struct)، [`parseListPattern`](50_patterns.md#gr.pattern.list)، [`parsePrimaryPattern`](50_patterns.md#gr.pattern.primary)
- **مُستدعى من:** [`parseMatchStmt`](10_statements.md#gr.stmt.match)، [`parsePattern`](50_patterns.md#gr.pattern.pattern)، [`parsePrimaryPattern`](50_patterns.md#gr.pattern.primary)، [`parseListPattern`](50_patterns.md#gr.pattern.list)، [`parseStructPattern`](50_patterns.md#gr.pattern.struct)، [`parsePattern`](50_patterns.md#gr.pattern.binding)

##### مخطّط مسار الدوال (حتى AST)
```mermaid
flowchart TD
  f1["parsePattern()"]
  f2["parseStructPattern()"]
  f1 -- "نمط بنية" --> f2
  f3["parseListPattern()"]
  f1 -- "نمط قائمة" --> f3
  f4["parsePrimaryPattern()"]
  f1 -- "نمط أوّليّ" --> f4
  f5(["⇒ WildcardPattern ∣ StructPattern ∣ ListPattern ∣ BindingPattern ∣ OrPattern"])
  f1 --> f5
```

#### 📊 مخطّط البنية النحويّة (Mermaid)
```mermaid
flowchart LR
  n1(["نمط"])
  n2["«_»"]
  n1 -- "شامل" --> n2
  n3(["⇒ WildcardPattern ∣ StructPattern ∣ ListPattern ∣ BindingPattern ∣ OrPattern"])
  n2 --> n3
  n4{"◆"}
  n5{"◆"}
  n6["struct"]
  n4 --> n6
  n6 --> n5
  n7["list"]
  n4 --> n7
  n7 --> n5
  n1 -- "بنية / قائمة" --> n4
  n8(["⇒ WildcardPattern ∣ StructPattern ∣ ListPattern ∣ BindingPattern ∣ OrPattern"])
  n5 --> n8
  n9["primary"]
  n10{"◇"}
  n11{"◇"}
  n12{"◆"}
  n13{"◆"}
  n14["«@»"]
  n15["pattern"]
  n14 --> n15
  n12 --> n14
  n15 --> n13
  n16{"◇"}
  n17{"◇"}
  n18["«∣»"]
  n19["primary"]
  n18 --> n19
  n16 --> n18
  n19 --> n17
  n19 -- "تكرار" --> n18
  n12 --> n16
  n17 --> n13
  n10 --> n12
  n13 --> n11
  n10 -- "تخطّي" --> n11
  n9 --> n10
  n1 -- "أوّليّ مع ربط/بدائل" --> n9
  n20(["⇒ WildcardPattern ∣ StructPattern ∣ ListPattern ∣ BindingPattern ∣ OrPattern"])
  n11 --> n20
```

---

<a id="gr.pattern.primary"></a>
### gr.pattern.primary — نمط أوّليّ <span dir="ltr">(PrimaryPattern)</span>

- **الرقم التسلسليّ:** `ق-067` · **المعرّف الموحَّد:** `gr.pattern.primary` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** حرفيّ (يدعم السالب)؛ نطاق حصريّ «1..10» أو شامل «1..=10» ⇒ RangePattern؛ مُعرّف ⇒ VariablePattern (يربط القيمة)؛ «تعداد.قيمة» أو «شكل.دائرة(نق)» ⇒ EnumVariantPattern (unit/ADT مؤهَّل)؛ «عدد(ق)» أو «جمع(ي، ن)» ⇒ ConstructorPattern (باني بحمولة غير مؤهَّل، تجريبيّ)

#### 📐 BNF
```bnf
PrimaryPattern = [ '-' ] Number [ ( '..' | '..=' ) [ '-' ] Number ]
               | String | BoolLit | NullLit
               | Identifier ( '.' Identifier [ CtorArgs ] | CtorArgs )? ;
CtorArgs       = '(' [ Pattern { ( ',' | '،' ) Pattern } ] ')' ;
```

#### 🧩 تفصيل البدائل
**1.** *حرفيّ / نطاق رقميّ (حصريّ «..» أو شامل «..=»):* `[ «-» ] ( «NUMBER_INTEGER» | «NUMBER_DOUBLE» ) [ «..» [ «=» ] [ «-» ] ( «NUMBER_INTEGER» | «NUMBER_DOUBLE» ) ]`
**2.** *حرفيّ آخر:* `( «STRING_LITERAL» | «صحيح» | «خطأ» | «لاشيء» )`
**3.** *متغيّر (ربط) أو عضو تعداد (مؤهَّل EnumVariant / باني غير مؤهَّل Constructor):* `«IDENTIFIER» [ ( ( «.» «IDENTIFIER» [ «(» [ pattern { «،» pattern } ] «)» ] ) | ( «(» [ pattern { «،» pattern } ] «)» ) ) ]`

#### 🔻 المسار إلى المحلل (دوال التحليل) ⇒ AST
**دالة (دوال) الدخول:**
1. [`ParserCore::parsePrimaryPattern`](../../../shared/parser/src/statements/parser_advanced.cpp) — `shared/parser/src/statements/parser_advanced.cpp`
- **عقدة AST المُنتَجة:** `LiteralPattern | RangePattern | VariablePattern | EnumVariantPattern | ConstructorPattern`
- **يستدعي دوال:** [`parsePattern`](50_patterns.md#gr.pattern.pattern)
- **مُستدعى من:** [`parsePattern`](50_patterns.md#gr.pattern.pattern)، [`parsePattern`](50_patterns.md#gr.pattern.or)

##### مخطّط مسار الدوال (حتى AST)
```mermaid
flowchart TD
  f1["parsePrimaryPattern()"]
  f2["parsePattern()"]
  f1 -- "نمط" --> f2
  f3(["⇒ LiteralPattern ∣ RangePattern ∣ VariablePattern ∣ EnumVariantPattern ∣ ConstructorPattern"])
  f1 --> f3
```

#### 📊 مخطّط البنية النحويّة (Mermaid)
```mermaid
flowchart LR
  n1(["نمط أوّليّ"])
  n2{"◇"}
  n3{"◇"}
  n4["«-»"]
  n2 --> n4
  n4 --> n3
  n2 -- "تخطّي" --> n3
  n5{"◆"}
  n6{"◆"}
  n7["«NUMBER_INTEGER»"]
  n5 --> n7
  n7 --> n6
  n8["«NUMBER_DOUBLE»"]
  n5 --> n8
  n8 --> n6
  n3 --> n5
  n9{"◇"}
  n10{"◇"}
  n11["«..»"]
  n12{"◇"}
  n13{"◇"}
  n14["«=»"]
  n12 --> n14
  n14 --> n13
  n12 -- "تخطّي" --> n13
  n11 --> n12
  n15{"◇"}
  n16{"◇"}
  n17["«-»"]
  n15 --> n17
  n17 --> n16
  n15 -- "تخطّي" --> n16
  n13 --> n15
  n18{"◆"}
  n19{"◆"}
  n20["«NUMBER_INTEGER»"]
  n18 --> n20
  n20 --> n19
  n21["«NUMBER_DOUBLE»"]
  n18 --> n21
  n21 --> n19
  n16 --> n18
  n9 --> n11
  n19 --> n10
  n9 -- "تخطّي" --> n10
  n6 --> n9
  n1 -- "حرفيّ / نطاق رقميّ (حصريّ «..» أو شامل «..=»)" --> n2
  n22(["⇒ LiteralPattern ∣ RangePattern ∣ VariablePattern ∣ EnumVariantPattern ∣ ConstructorPattern"])
  n10 --> n22
  n23{"◆"}
  n24{"◆"}
  n25["«STRING_LITERAL»"]
  n23 --> n25
  n25 --> n24
  n26["«صحيح»"]
  n23 --> n26
  n26 --> n24
  n27["«خطأ»"]
  n23 --> n27
  n27 --> n24
  n28["«لاشيء»"]
  n23 --> n28
  n28 --> n24
  n1 -- "حرفيّ آخر" --> n23
  n29(["⇒ LiteralPattern ∣ RangePattern ∣ VariablePattern ∣ EnumVariantPattern ∣ ConstructorPattern"])
  n24 --> n29
  n30["«IDENTIFIER»"]
  n31{"◇"}
  n32{"◇"}
  n33{"◆"}
  n34{"◆"}
  n35["«.»"]
  n36["«IDENTIFIER»"]
  n35 --> n36
  n37{"◇"}
  n38{"◇"}
  n39["«(»"]
  n40{"◇"}
  n41{"◇"}
  n42["pattern"]
  n43{"◇"}
  n44{"◇"}
  n45["«،»"]
  n46["pattern"]
  n45 --> n46
  n43 --> n45
  n46 --> n44
  n46 -- "تكرار" --> n45
  n43 -- "صفر/أكثر" --> n44
  n42 --> n43
  n40 --> n42
  n44 --> n41
  n40 -- "تخطّي" --> n41
  n39 --> n40
  n47["«)»"]
  n41 --> n47
  n37 --> n39
  n47 --> n38
  n37 -- "تخطّي" --> n38
  n36 --> n37
  n33 --> n35
  n38 --> n34
  n48["«(»"]
  n49{"◇"}
  n50{"◇"}
  n51["pattern"]
  n52{"◇"}
  n53{"◇"}
  n54["«،»"]
  n55["pattern"]
  n54 --> n55
  n52 --> n54
  n55 --> n53
  n55 -- "تكرار" --> n54
  n52 -- "صفر/أكثر" --> n53
  n51 --> n52
  n49 --> n51
  n53 --> n50
  n49 -- "تخطّي" --> n50
  n48 --> n49
  n56["«)»"]
  n50 --> n56
  n33 --> n48
  n56 --> n34
  n31 --> n33
  n34 --> n32
  n31 -- "تخطّي" --> n32
  n30 --> n31
  n1 -- "متغيّر (ربط) أو عضو تعداد (مؤهَّل EnumVariant / باني غير مؤهَّل Constructor)" --> n30
  n57(["⇒ LiteralPattern ∣ RangePattern ∣ VariablePattern ∣ EnumVariantPattern ∣ ConstructorPattern"])
  n32 --> n57
```

---

<a id="gr.pattern.list"></a>
### gr.pattern.list — نمط قائمة <span dir="ltr">(ListPattern)</span>

- **الرقم التسلسليّ:** `ق-068` · **المعرّف الموحَّد:** `gr.pattern.list` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** تفكيك المصفوفة «[أ، ب]»؛ قائمة فارغة «[]»؛ نمط الباقي الطرفيّ «*اسم» (OP_MULTIPLY) يربط ذيل العناصر (has_rest) ويجب أن يكون آخِر عنصر

#### 📐 BNF
```bnf
ListPattern = '[' [ Pattern { ( ',' | '،' ) Pattern } [ ( ',' | '،' ) '*' Identifier ]
                  | '*' Identifier ] ']' ;
```

#### 🧩 تفصيل البدائل
- `«[» [ pattern { «،» pattern } [ [ «،» ] «*» «IDENTIFIER» ] ] «]»`

#### 🔻 المسار إلى المحلل (دوال التحليل) ⇒ AST
**دالة (دوال) الدخول:**
1. [`ParserCore::parseListPattern`](../../../shared/parser/src/statements/parser_advanced.cpp) — `shared/parser/src/statements/parser_advanced.cpp`
- **عقدة AST المُنتَجة:** `ListPattern`
- **يستدعي دوال:** [`parsePattern`](50_patterns.md#gr.pattern.pattern)
- **مُستدعى من:** [`parsePattern`](50_patterns.md#gr.pattern.pattern)

##### مخطّط مسار الدوال (حتى AST)
```mermaid
flowchart TD
  f1["parseListPattern()"]
  f2["parsePattern()"]
  f1 -- "نمط" --> f2
  f3(["⇒ ListPattern"])
  f1 --> f3
```

#### 📊 مخطّط البنية النحويّة (Mermaid)
```mermaid
flowchart LR
  n1(["نمط قائمة"])
  n2["«(»"]
  n3{"◇"}
  n4{"◇"}
  n5["pattern"]
  n6{"◇"}
  n7{"◇"}
  n8["«،»"]
  n9["pattern"]
  n8 --> n9
  n6 --> n8
  n9 --> n7
  n9 -- "تكرار" --> n8
  n6 -- "صفر/أكثر" --> n7
  n5 --> n6
  n10{"◇"}
  n11{"◇"}
  n12{"◇"}
  n13{"◇"}
  n14["«،»"]
  n12 --> n14
  n14 --> n13
  n12 -- "تخطّي" --> n13
  n15["«*»"]
  n13 --> n15
  n16["«IDENTIFIER»"]
  n15 --> n16
  n10 --> n12
  n16 --> n11
  n10 -- "تخطّي" --> n11
  n7 --> n10
  n3 --> n5
  n11 --> n4
  n3 -- "تخطّي" --> n4
  n2 --> n3
  n17["«)»"]
  n4 --> n17
  n1 --> n2
  n18(["⇒ ListPattern"])
  n17 --> n18
```

---

<a id="gr.pattern.struct"></a>
### gr.pattern.struct — نمط بنية <span dir="ltr">(StructPattern)</span>

- **الرقم التسلسليّ:** `ق-069` · **المعرّف الموحَّد:** `gr.pattern.struct` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** تفكيك الكائن «{س: نمط}»؛ بنية فارغة «{}»؛ اختصار «{حقل}» بلا نقطتين ⇒ يربط الحقل باسمه (VariablePattern)

#### 📐 BNF
```bnf
StructPattern = '{' [ Field { ( ',' | '،' ) Field } ] '}' ; Field = Identifier [ ':' Pattern ] ;
```

#### 🧩 تفصيل البدائل
- `«{» [ «IDENTIFIER» [ «:» pattern ] { «،» «IDENTIFIER» [ «:» pattern ] } ] «}»`

#### 🔻 المسار إلى المحلل (دوال التحليل) ⇒ AST
**دالة (دوال) الدخول:**
1. [`ParserCore::parseStructPattern`](../../../shared/parser/src/statements/parser_advanced.cpp) — `shared/parser/src/statements/parser_advanced.cpp`
- **عقدة AST المُنتَجة:** `StructPattern`
- **يستدعي دوال:** [`parsePattern`](50_patterns.md#gr.pattern.pattern)
- **مُستدعى من:** [`parsePattern`](50_patterns.md#gr.pattern.pattern)

##### مخطّط مسار الدوال (حتى AST)
```mermaid
flowchart TD
  f1["parseStructPattern()"]
  f2["parsePattern()"]
  f1 -- "نمط" --> f2
  f3(["⇒ StructPattern"])
  f1 --> f3
```

#### 📊 مخطّط البنية النحويّة (Mermaid)
```mermaid
flowchart LR
  n1(["نمط بنية"])
  n2["«❲»"]
  n3{"◇"}
  n4{"◇"}
  n5["«IDENTIFIER»"]
  n6{"◇"}
  n7{"◇"}
  n8["«:»"]
  n9["pattern"]
  n8 --> n9
  n6 --> n8
  n9 --> n7
  n6 -- "تخطّي" --> n7
  n5 --> n6
  n10{"◇"}
  n11{"◇"}
  n12["«،»"]
  n13["«IDENTIFIER»"]
  n12 --> n13
  n14{"◇"}
  n15{"◇"}
  n16["«:»"]
  n17["pattern"]
  n16 --> n17
  n14 --> n16
  n17 --> n15
  n14 -- "تخطّي" --> n15
  n13 --> n14
  n10 --> n12
  n15 --> n11
  n15 -- "تكرار" --> n12
  n10 -- "صفر/أكثر" --> n11
  n7 --> n10
  n3 --> n5
  n11 --> n4
  n3 -- "تخطّي" --> n4
  n2 --> n3
  n18["«❳»"]
  n4 --> n18
  n1 --> n2
  n19(["⇒ StructPattern"])
  n18 --> n19
```

---

<a id="gr.pattern.binding"></a>
### gr.pattern.binding — نمط ربط <span dir="ltr">(BindingPattern)</span>

- **الرقم التسلسليّ:** `ق-070` · **المعرّف الموحَّد:** `gr.pattern.binding` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** «ن @ 1..10» يربط «ن» ويطابق النطاق

#### 📐 BNF
```bnf
BindingPattern = Identifier '@' Pattern ;
```

#### 🧩 تفصيل البدائل
- `«IDENTIFIER» «@» pattern`

#### 🔻 المسار إلى المحلل (دوال التحليل) ⇒ AST
**دالة (دوال) الدخول:**
1. [`ParserCore::parsePattern`](../../../shared/parser/src/statements/parser_advanced.cpp) — `shared/parser/src/statements/parser_advanced.cpp`
- **عقدة AST المُنتَجة:** `BindingPattern`

##### مخطّط مسار الدوال (حتى AST)
```mermaid
flowchart TD
  f1["parsePattern()"]
  f2(["⇒ BindingPattern"])
  f1 --> f2
```

#### 📊 مخطّط البنية النحويّة (Mermaid)
```mermaid
flowchart LR
  n1(["نمط ربط"])
  n2["«IDENTIFIER»"]
  n3["«@»"]
  n2 --> n3
  n4["pattern"]
  n3 --> n4
  n1 --> n2
  n5(["⇒ BindingPattern"])
  n4 --> n5
```

---

<a id="gr.pattern.or"></a>
### gr.pattern.or — نمط بدائل <span dir="ltr">(OrPattern)</span>

- **الرقم التسلسليّ:** `ق-071` · **المعرّف الموحَّد:** `gr.pattern.or` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** بدائل «1 | 2» — الرمز المعياريّ «|» (OP_BITWISE_OR)؛ ويُقبل «||» (OP_OR) تساهلًا (ISSUE-033)

#### 📐 BNF
```bnf
OrPattern = PrimaryPattern '|' PrimaryPattern { '|' PrimaryPattern } ;
```

#### 🧩 تفصيل البدائل
- `primary ( «|» primary )+`

#### 🔻 المسار إلى المحلل (دوال التحليل) ⇒ AST
**دالة (دوال) الدخول:**
1. [`ParserCore::parsePattern`](../../../shared/parser/src/statements/parser_advanced.cpp) — `shared/parser/src/statements/parser_advanced.cpp`
- **عقدة AST المُنتَجة:** `OrPattern`
- **يستدعي دوال:** [`parsePrimaryPattern`](50_patterns.md#gr.pattern.primary)
- **روابط المعجم:** عوامل: «|»

##### مخطّط مسار الدوال (حتى AST)
```mermaid
flowchart TD
  f1["parsePattern()"]
  f2["parsePrimaryPattern()"]
  f1 -- "نمط أوّليّ" --> f2
  f3(["⇒ OrPattern"])
  f1 --> f3
```

#### 📊 مخطّط البنية النحويّة (Mermaid)
```mermaid
flowchart LR
  n1(["نمط بدائل"])
  n2["primary"]
  n3{"◇"}
  n4{"◇"}
  n5["«∣»"]
  n6["primary"]
  n5 --> n6
  n3 --> n5
  n6 --> n4
  n6 -- "تكرار" --> n5
  n2 --> n3
  n1 --> n2
  n7(["⇒ OrPattern"])
  n4 --> n7
```

---
