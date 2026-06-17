# قواعد المحلل — طبقة أنماط المطابقة

> ⚠️ **هذا الملف مُولَّد آليًّا** من `language-truth/grammar/*.yaml` عبر
> `scripts/codegen/gen_parser_grammar_docs.py` — **لا تُحرِّره يدويًّا**.
> عدّل YAML المصدر ثم أعد التوليد.


- **الطبقة:** `patterns` · **ملف المصدر:** `language-truth/grammar/50_patterns.yaml`
- **الوصف:** أنماط المطابقة — شامل/حرفيّ/متغيّر/نطاق/قائمة/بنية/ربط/بدائل
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
  o3 --> o1
  o4 --> o1
  o5 --> o1
  o6 --> o2
```

---

<a id="gr.pattern.pattern"></a>
### gr.pattern.pattern — نمط <span dir="ltr">(Pattern)</span>

- **الرقم التسلسليّ:** `ق-065` · **المعرّف الموحَّد:** `gr.pattern.pattern` · **الحالة:** stable · **منذ:** 1.0.0
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
- **مُستدعى من:** [`parseMatchStmt`](10_statements.md#gr.stmt.match)، [`parsePattern`](50_patterns.md#gr.pattern.pattern)، [`parseListPattern`](50_patterns.md#gr.pattern.list)، [`parseStructPattern`](50_patterns.md#gr.pattern.struct)، [`parsePattern`](50_patterns.md#gr.pattern.binding)

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

- **الرقم التسلسليّ:** `ق-066` · **المعرّف الموحَّد:** `gr.pattern.primary` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** حرفيّ (يدعم السالب)، نطاق «1..10» ⇒ RangePattern، أو مُعرّف ⇒ VariablePattern (يربط القيمة)

#### 📐 BNF
```bnf
PrimaryPattern = [ '-' ] Number [ '..' [ '-' ] Number ]
               | String | BoolLit | NullLit | Identifier ;
```

#### 🧩 تفصيل البدائل
**1.** *حرفيّ / نطاق رقميّ:* `[ «-» ] ( «NUMBER_INTEGER» | «NUMBER_DOUBLE» ) [ «..» [ «-» ] ( «NUMBER_INTEGER» | «NUMBER_DOUBLE» ) ]`
**2.** *حرفيّ آخر:* `( «STRING_LITERAL» | «صحيح» | «خطأ» | «لاشيء» )`
**3.** *متغيّر (ربط):* `«IDENTIFIER»`

#### 🔻 المسار إلى المحلل (دوال التحليل) ⇒ AST
**دالة (دوال) الدخول:**
1. [`ParserCore::parsePrimaryPattern`](../../../shared/parser/src/statements/parser_advanced.cpp) — `shared/parser/src/statements/parser_advanced.cpp`
- **عقدة AST المُنتَجة:** `LiteralPattern | RangePattern | VariablePattern`
- **مُستدعى من:** [`parsePattern`](50_patterns.md#gr.pattern.pattern)، [`parsePattern`](50_patterns.md#gr.pattern.or)

##### مخطّط مسار الدوال (حتى AST)
```mermaid
flowchart TD
  f1["parsePrimaryPattern()"]
  f2(["⇒ LiteralPattern ∣ RangePattern ∣ VariablePattern"])
  f1 --> f2
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
  n14["«-»"]
  n12 --> n14
  n14 --> n13
  n12 -- "تخطّي" --> n13
  n11 --> n12
  n15{"◆"}
  n16{"◆"}
  n17["«NUMBER_INTEGER»"]
  n15 --> n17
  n17 --> n16
  n18["«NUMBER_DOUBLE»"]
  n15 --> n18
  n18 --> n16
  n13 --> n15
  n9 --> n11
  n16 --> n10
  n9 -- "تخطّي" --> n10
  n6 --> n9
  n1 -- "حرفيّ / نطاق رقميّ" --> n2
  n19(["⇒ LiteralPattern ∣ RangePattern ∣ VariablePattern"])
  n10 --> n19
  n20{"◆"}
  n21{"◆"}
  n22["«STRING_LITERAL»"]
  n20 --> n22
  n22 --> n21
  n23["«صحيح»"]
  n20 --> n23
  n23 --> n21
  n24["«خطأ»"]
  n20 --> n24
  n24 --> n21
  n25["«لاشيء»"]
  n20 --> n25
  n25 --> n21
  n1 -- "حرفيّ آخر" --> n20
  n26(["⇒ LiteralPattern ∣ RangePattern ∣ VariablePattern"])
  n21 --> n26
  n27["«IDENTIFIER»"]
  n1 -- "متغيّر (ربط)" --> n27
  n28(["⇒ LiteralPattern ∣ RangePattern ∣ VariablePattern"])
  n27 --> n28
```

---

<a id="gr.pattern.list"></a>
### gr.pattern.list — نمط قائمة <span dir="ltr">(ListPattern)</span>

- **الرقم التسلسليّ:** `ق-067` · **المعرّف الموحَّد:** `gr.pattern.list` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** تفكيك المصفوفة «[أ، ب]»

#### 📐 BNF
```bnf
ListPattern = '[' [ Pattern { ( ',' | '،' ) Pattern } ] ']' ;
```

#### 🧩 تفصيل البدائل
- `«[» [ pattern { «،» pattern } ] «]»`

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
  n3 --> n5
  n7 --> n4
  n3 -- "تخطّي" --> n4
  n2 --> n3
  n10["«)»"]
  n4 --> n10
  n1 --> n2
  n11(["⇒ ListPattern"])
  n10 --> n11
```

---

<a id="gr.pattern.struct"></a>
### gr.pattern.struct — نمط بنية <span dir="ltr">(StructPattern)</span>

- **الرقم التسلسليّ:** `ق-068` · **المعرّف الموحَّد:** `gr.pattern.struct` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** تفكيك الكائن «{س: س}»

#### 📐 BNF
```bnf
StructPattern = '{' Identifier ':' Pattern { ( ',' | '،' ) Identifier ':' Pattern } '}' ;
```

#### 🧩 تفصيل البدائل
- `«{» ( «IDENTIFIER» «:» pattern )+ «}»`

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
  n6["«:»"]
  n5 --> n6
  n7["pattern"]
  n6 --> n7
  n3 --> n5
  n7 --> n4
  n7 -- "تكرار" --> n5
  n2 --> n3
  n8["«❳»"]
  n4 --> n8
  n1 --> n2
  n9(["⇒ StructPattern"])
  n8 --> n9
```

---

<a id="gr.pattern.binding"></a>
### gr.pattern.binding — نمط ربط <span dir="ltr">(BindingPattern)</span>

- **الرقم التسلسليّ:** `ق-069` · **المعرّف الموحَّد:** `gr.pattern.binding` · **الحالة:** stable · **منذ:** 1.0.0
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

- **الرقم التسلسليّ:** `ق-070` · **المعرّف الموحَّد:** `gr.pattern.or` · **الحالة:** stable · **منذ:** 1.0.0
- **الوصف:** بدائل «1 | 2» (تستخدم OP_OR)

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
- **روابط المعجم:** عوامل: «||»

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
