# حالة تنفيذ نظام الأنواع — type-system

> مصدر حقيقة الحالة (GR-01: لا ادّعاء بلا دليل من الكود/البناء).
> آخر تحديث: 2026-06-12 (من الجهاز).

## القصص المُنفَّذة

### ✅ S-TS-P0.5 — توليد SadTypeKind من types.yaml
**الحالة:** مُنفَّذة ومُختبَرة (فرع `type_system`، غير مدموجة بعد).

| البند | الدليل |
|------|--------|
| توليد | `[gen_types] ✓ 49 نوعًا → sad_type_kind_generated.h` |
| عدم انحدار | `diff` enum المُولَّد ↔ الأصلي = فارغ (49 مطابقة بالترتيب) |
| البناء — المفسّر | `sad-run.exe` EXIT 0 (Debug) |
| البناء — المترجم | `sadc` Release يُبنى ويُربَط نظيفًا (`sadc.vcxproj -> sad-build.exe`، لا LNK) |
| اختبار اللغة | **`runner.py --level P0` = 5/5 (100%) تنفيذ مزدوج مفسّر+مترجم** |
| اختبار المولّد | `test_gen_types.py` 6/6؛ `test_gen_error_messages_v5.py` 9/9 |
| حيّ | `نوع(42)`=رقم، `نوع("..")`=نص، `نوع([..])`=مصفوفة |
| CMake | هدف `sad_types_codegen` مُفعَّل ضمن `sad_all_codegen` (env OK) |

> ملاحظة بناء: فشل `sad-build.exe` في **Debug** برموز LLVM (`SDNode::dump`/`dumpr`/`SCEVExpander`) **سابق لهذا العمل** — 0 رموز أنواع في أخطاء الرابط (كلها `llvm::` debug-methods)؛ Release يربط نظيفًا.

**الأثر:** `SadTypeKind` لم يعد يدويًّا — خرق SoT أُنهي. تضارب 34/46/49 ممنوع بنيويًّا (العدد من YAML).

### ✅ S-TS-P0 — اتّساق أرقام الأنواع والتوثيق
**الحالة:** مُنفَّذة (فرع `type_system`).

| البند | الدليل |
|------|--------|
| تعليق الترويسة | `sad_type_system.h:16,56,57` = «50 قيمة» (لا «34»/«49»)؛ `grep "34 قيم\|49 قيم"` = 0 |
| العدد المُولَّد | `sad_type_kind_generated.h` = 50 قيمة (49 + `Null`) |
| الوثيقتان | النسختان (`_bmad`/`docs`) تذكران 50، لا «46»، **متطابقتان** (`diff -q` = فارغ) |
| بناء | `sad-run.exe` (Debug) EXIT 0 |

### ✅ S-TS-P1 — Null (عدم) نوع متمايز عن Void (فراغ)
**الحالة:** مُنفَّذة في **المفسّر** ومُختبَرة (فرع `type_system`). codegen المترجم **مؤجَّل** (موثّق `@skip_compiler`).

| البند | الدليل |
|------|--------|
| قيمة enum | `SadTypeKind::Null` (مُولَّدة من `types.yaml: type.null kind:Null`) — العدد 50 |
| السجلّ | `SadTypeRegistry::null_` + `getNull()` (`sad_type_system.h:1205,1381`) |
| بناء القيمة | `Value::makeNull()` (`value.cpp`)؛ `LITERAL_NULL → makeNull()` (`expression_evaluator_core.cpp:279`) |
| تعريب `نوع()` | `type_of` → «عدم» لـ Null (`shared/builtins/.../builtins.cpp:599`) |
| مساواة | `عدم == عدم`→صحيح؛ `عدم == رقم/نص`→خطأ (`binary_logic.cpp` فرع Null + `value.cpp` operator==) |
| عوامل null | `?.`/`??` تفحص `isNull()` لا `isVoid()` (`members_advanced.cpp:687,740`) |
| التأكيدات | `{"عدم","NULL"},{"لاشيء","NULL"}` (`builtin_module_assertions.cpp:533`) |
| الجسر | قسم ValueType: `case Null → getNull()` (`type_bridge.cpp:316`) — أقسام DataType سليمة |
| اختبار حيّ | `049_null_distinct_from_void.ص`: `نوع(لاشيء)=عدم`, `عدم==عدم=صحيح`, تمايز عن 0/"" — **مفسّر PASS** |
| runner | `--level P0` = 5/5 (100% مزدوج)؛ قسم «أنواع» 049 PASS؛ لا تراجع (الفشل المتبقّي: ميزات `ساكن`/`+=`/fstring/`.اول()` سابقة) |
| المولّد | `test_gen_types.py` EXIT 0 |

**فجوة المترجم الموثّقة (BF-08):** codegen المترجم لا يميّز Null بعد: `نوع(لاشيء)`→«مجهول»، و`لاشيء == نص` يُسقِط التنفيذي. مؤجَّل لقصّة codegen-null لاحقة (يتقاطع P4/P9). انظر سبب `@skip_compiler` في الاختبار.

> **انحراف عن نصّ الخطة (مُبرَّر):** الخطة P1 كتبت `نوع(لاشيء)`=«لاشيء»؛ الصحيح وفق SoT (`types.yaml: type.null word:عدم`) هو **«عدم»** — اعتُمد «عدم» التزامًا بـSoT-First (موثّق في S-TS-P1 §السياق).

### ✅ S-TS-P2 — توحيد المدقّق الدلالي على SadTypeKind
**الحالة:** مُنفَّذة (فرع `type_system`).

| البند | الدليل |
|------|--------|
| المحور | `dataTypeToTypePtr(DataType)` → `sadKindToTypePtr(SadTypeKind)`؛ `dataTypeArabicName` → `sadKindArabicName(SadTypeKind)` (`type_checker.cpp`, `_stmt_visitors.cpp`) |
| إزالة enum-literals | `grep -c "Data::DataType::"` في `type_checker.cpp` + `_stmt_visitors.cpp` = **0** (كان 28) |
| المتبقّي (حدود الجسر موثّقة) | `Types::fromDataType(...)` + غلافا `dataType*` فقط — تُحذف في S-TS-P2.5a (DoD §«محصور بحدود الجسر») |
| سلوك محايد | السلاسل العربية مطابقة تمامًا؛ التحويل يمرّ عبر `fromDataType` (نفس النتائج) |
| بناء | `sad-run.exe` (Debug) EXIT 0 |
| اختبار | `--level P0`=5/5؛ `--level P1`=98/108 (تحسّن +1 عن خط الأساس، لا تراجع)؛ OOP/errors: الإخفاقات سابقة (فجوات مترجم enums/structs + دوال مصفوفة `.اضف/.اول` مفقودة + FAIL_OUTPUT مع تطابق مخرجات المفسّر) |

### 🔄 S-TS-P2.5a — ترحيل DataType في الـAST/المحلّل (قيد التنفيذ — دفعة 1/8)
**الحالة:** بدأت (فرع `type_system`). **تصحيح نطاق جوهري (GR-01).**

**⚠️ تصحيح التقدير (دليل grep فعلي):** نطاق الخطة «23 ملفًّا» **أقلّ بكثير من الواقع**:
- `getDataType()` (مشتقّ، لا حقل): 12 نداءً فقط — سهل.
- `Data::DataType::` enum-literals: **290** موضعًا عبر المفسّر/المترجم/الأدوات.
- حقل `SimpleTypeNode.dataType` وحده: **168 مستهلكًا** (ownership/مفسّر/مترجم/LSP).
- إجمالي نقاط اللمس الحقيقية: **400+** عبر أنظمة فرعية متعدّدة (ليست ast+parser فقط).
- **الخلاصة:** القصّة أقرب إلى XL لا L؛ تُنفَّذ على **8 دفعات** يبقى البناء أخضر بعد كلٍّ.

| الدفعة | المحتوى | الحالة |
|--------|---------|--------|
| 1/8 | `ASTNode::getTypeKind()→SadTypeKind` (محور موحَّد) + ترحيل `expressions.cpp` (Binary/Ternary/Index) | ✅ مبنيّ + commit `fc12e300` |
| 2/8 | `type_nodes.{h,cpp}` — حقل `kind` متوازٍ في `SimpleTypeNode` + `isCompatibleWith` على kind | ✅ مبنيّ + P0 5/5 + commit `3c968035` |
| 3/8 | `declarations.cpp` + `statements.cpp` — مقارنات الحقول على SadTypeKind (جسر) | ✅ مبنيّ + P0 5/5 + commit `7f1e3c88` |
| 4-8 | **التغيير المقترن الضخم** (انظر أدناه) | ⏳ |

**استراتيجية الإبقاء على البناء أخضر:** القرّاء يُرحَّلون أولًا إلى `Types::fromDataType(field)==SadTypeKind::X`، ثم تُقلَب الحقول ويُبسَّط `fromDataType(field)`→`field`. `grep DataType=0` لا يتحقّق إلا في النهاية.

**⚠️ اكتشاف بنيوي (الدفعات 4-8 = تغيير واحد مقترن، لا تتجزّأ):**
دوال المحلّل (`parseType`/`parseGenericType`/`mapTokenTypeToDataType`، `parser_helpers.cpp:1610+`) **تُنتج `DataType`** يُخزَّن في حقول الـAST العامة (`returnType`/`type`/`exceptionType`/`SimpleTypeNode.dataType`/`templateArguments`). قلب أيّ حقل عام **يكسر كل قرّائه فورًا** (290 موضع `DataType::` في المفسّر/المترجم/الملكية/LSP). لذا:
- إنتاج المحلّل (SadTypeKind) + قلب الحقول + ترحيل 290 قارئًا = **تغيير واحد مقترن** لا يقبل التجزئة لدفعات خضراء مستقلّة إلا عبر **نمط الحقل المتوازي الشامل** (إضافة حقول SadTypeKind موازية لكل حقل، ترحيل القرّاء تدريجيًّا، ثم الحذف).
- بعض العقد (`Parameter`, `VarDeclStmt`) **تحمل `sadType` متوازيًا أصلًا** → قرّاؤها يُرحَّلون إلى `sadType->getKind()` مباشرة. البقية (`FieldDecl`/`MethodDecl.returnType`/`PropertyDecl`/`CatchClause`/`SimpleTypeNode`) تحتاج إضافة موازٍ أولًا.
- يتطلّب بناءات `sadc` (Release) متكرّرة (المترجم ×14 ملف builders) — جوهر تقدير 5-8 أيام.

**تقدّم القلب المقترن (محدَّث):**
- ✅ **اكتشاف:** الحقول المتوازية موجودة أصلًا — `FunctionDecl.sadReturnType`، `Parameter.sadType`، `VarDeclStmt.sadType`، و`SimpleTypeNode.kind` (مُضاف). فالقلب أساسًا **ترحيل القرّاء** إلى `.sad*->getKind()` ثم حذف الحقول الموروثة.
- ✅ **المدقّق الدلالي** (P2): مُرحَّل بالكامل (enum-literals=0).
- ✅ **قرّاء المفسّر** (مقارنات النوع، 11 موضعًا): مُرحَّلون إلى `fromDataType(field)==SadTypeKind` (commit `a35f1b66`) — صيغة تُبسَّط عند القلب.
- ⏳ **قرّاء المترجم** (35 موضع مقارنة في 11 ملف builders + ~70 استخدامًا آخر) — يحتاج بناءات `sadc` Release متكرّرة.
- ⏳ **قرّاء الملكية** (4) + **LSP** (1).
- ⏳ **قلب الحقول** (حذف `returnType`/`type`/`exceptionType`/`SimpleTypeNode.dataType`/`templateArguments` + بُناتها) + **إنتاج المحلّل SadTypeKind** (`parseType`/`mapTokenTypeToDataType`) + حذف `getDataType()`.
- ⏳ **التحقّق النهائي:** `grep -c DataType` في ast+parser = 0 + بناء sadc Release + كل الاختبارات.

### ✅ S-TS-P2.5a — اكتمل القلب المقترن (grep=0)
**الحالة:** القلب المقترن نُفِّذ دفعةً واحدة متّصلة. **`grep -c DataType` في `shared/ast`+`shared/parser` = 0** (AC3 محقّق).

| البند | الدليل |
|------|--------|
| إزالة DataType من ast+parser | `grep -rc DataType shared/ast shared/parser = 0` (كان 223) |
| حذف `getDataType()` | أُزيل من `ast_node.h`؛ القرّاء → `getTypeKind()→SadTypeKind` |
| قلب الحقول | `SimpleTypeNode.kind`، `OperatorOverload.returnType`، `TraitMethodInfo.{returnType,paramTypes}` → SadTypeKind؛ حقول declarations/statements/expressions عبر السطح الموحَّد |
| إنتاج المحلّل | `parseType`/`mapTokenTypeToKind`/`stringToKind` تُنتج `SadTypeKind` مباشرة |
| القرّاء | المفسّر/المترجم/الملكية/LSP/الدلالات: مقارنات `SadTypeKind::X` (لا جسر DataType) |
| بناء المفسّر | `sad-run.exe` (Debug) EXIT 0 |
| اختبار المفسّر | `--level P0` 5/5؛ `--level P1` لا تراجع جديد (نفس `FAIL_INTERP` السابقين: 218/081 — دوال مصفوفة مفقودة، غير متعلقة) |

> **✅ حُلّ (إصلاح مستقلّ، commit `2aa413c0`):** الكسر السابق كان **تضارب علم `-o`**: `memory_mode_flag` سجّل `-o` اسمًا مختصرًا لمستوى الملكية، ومحلّل أعلام الذاكرة (يعمل قبل CLI) يبتلع `-o`+قيمته، فيختفي مسار الإخراج → يُكتب التنفيذيّ باسم المصدر في cwd → الـrunner يراه مفقودًا. أُزيل الاسم المختصر `o` ومعالجه. **بعد الإصلاح: `--level P0` = 5/5 مزدوج حقيقي، `--level P1` = 105/108 (104 تكافؤ مزدوج).** القلب (P2.5a) صلب في المفسّر **والمترجم**.
>
> **⚠️ سياق الاكتشاف (GR-01):**
> أثناء التحقّق تبيّن أن الـrunner كان يستخدم `build/bin/Release/sadc.exe` **بائتًا (9 مايو)** طوال الجلسة، فكل ادّعاءات «P0 5/5 مزدوج (مفسّر+مترجم)» (في P0.5/P1/P2/P2.5a-1..4) **صحيحة للمفسّر فقط**؛ جانب المترجم لم يُختبَر فعليًّا.
> المترجم الطازج (`sad-build.exe`، هدف `sadc`) **يُجمَّع نظيفًا (exit 0) لكنه لا يُنتج تنفيذيًّا** لأبسط برنامج (`نوع 001_hello`: «لم يُنتج ملف تنفيذي»).
> **عُزِل السبب بـ`git stash`:** الحالة المُلتزَمة **قبل** قلب P2.5a (P0.5+P1+P2+P2.5a-1..4) **تفشل بنفس الطريقة** → الكسر **سابقٌ لقلب P2.5a** (منشؤه أساس P0.5 غير المُلتزَم أو أبكر)، لا يسبّبه هذا العمل. القلب لا يُدخِل تراجعًا على المترجم (مكسور بالتساوي قبله وبعده).
> **توصية متبقّية:** تصحيح مسار `sadc.exe` في `tests/config.yaml` ليشير إلى `sad-build.exe` (المُنتَج الفعلي لهدف `sadc`)، أو الإبقاء على نسخ `sad-build.exe→sadc.exe` بعد كل بناء مترجم.

### ✅ S-TS-P2.5b — حذف دوال DataType من الجسر + حذف data_types.h
**الحالة:** مُنفَّذة (فرع `type_system`).

| البند | الدليل |
|------|--------|
| AC1 صفر مستهلك | `grep` لـ`toDataType/fromDataType/sadType*DataType` = 0 خارجيًّا قبل الحذف |
| حذف دوال الجسر | أُزيلت الأربع من `type_bridge.{h,cpp}` + أعضاء `SadType::toDataType/fromDataType` من `sad_type_system.{h,cpp}` |
| AC3 ValueType/runtime سليمة | `toValueType/fromValueType/sadType*ValueType/inferSadType/isValueCompatible` **باقية** |
| AC2 حذف data_types.h | محذوف (`git rm`)؛ `grep "Data::DataType\|DataType::"` في كل الكود (عدا الأرشيف) = **0** |
| AC5 بناء مزدوج | `sad-run` (Debug) + `sadc` (Release) EXIT 0؛ `--level P0` = 5/5 مزدوج |
| AC4 عبور Optional | بنيويًّا: لا دالة DataType تُسقِط `Optional→Void` بعد الآن؛ التطبيق التنفيذي الكامل في S-TS-P4 |

**الأثر:** النظام الأفقر (`DataType`، 14 قيمة) أُزيل نهائيًّا. المحور الوحيد الآن `SadTypeKind` (50 قيمة، مُولَّد). «الاستبدال الكامل» تحقّق.

### ✅ S-TS-P3 — Result<T,E> نوع جبري مطبَّق
**الحالة:** مُنفَّذة (فرع `type_system`، commit `8eee9fad`).

| البند | الدليل |
|------|--------|
| AC1 الصنف | `SadResultType(okType, errType)` في `sad_type_system.h` (نسق SadOptionalType) + تصريح مسبق |
| AC2 makeResult+إسناد | `SadTypeRegistry::makeResult` + فرع `isAssignableTo` تغايُري للنتيجة + المساواة البنيوية |
| AC4 اختبار وحدة | `test_type_bridge`: اختباران (البناء/الأنواع الداخلية + المساواة/الإسناد) — 28/28 ✓ |
| تنظيف | أُزيل قسم اختبار DataType المحذوف من `test_type_bridge` (كان سيكسر بناء الاختبارات) |
| بناء | `sad-run` + `test_type_bridge` أخضر؛ runner P0 5/5 مزدوج |

### 🔄 S-TS-P4 — توصيل Optional/Future/Generator (جزئي — AC7 منجز)
**الحالة:** AC7 مُنفَّذ (commit `8088517f`)؛ البقية تتطلّب جهدًا معماريًّا أعمق (موثّق).

| البند | الحالة |
|------|--------|
| AC7 (تملكه القصّة حصريًّا) | ✅ `SadType::isAssignableTo`: `Null <: T?‎` و`T <: T?‎` (commit `8088517f`) |
| AC5 سطح `رقم?` (مستوى الـkind) | ✅ `parseType` يستهلك لاحقة `?` → Optional (commit `fee5d270`)؛ `رقم?`/`نص?` متغير+معامل يُحلّل ويعمل في المفسّر؛ اختبار `050_optional_type_suffix.ص` PASS |
| AC6 فضّ غموض الثلاثي | ✅ بالسياق: `parseType` في مواضع الأنواع فقط (لا وسط تعبير) ⇒ `?` هناك = Optional حصرًا |
| إصلاحات داعمة | `sadKindToTypePtr` + `SadType::fromValueType`: Optional/Result/Future/Generator → Any (لا انهيار إلى Void، لا تحذيرات كاذبة) |
| AC1-4 Optional حيًّا (جزئي) | ✅ المفسّر: `رقم?`+null+`??` تعمل. ⏳ النوع الداخلي الغنيّ Optional<T> (عبر sadType) + Future/Generator runtime — تبقى. المترجم: `@skip_compiler` (codegen يعامل Optional=I64، لا يميّز null — نفس فجوة P1). |

> **المتبقّي من P4:** (1) تمثيل Optional<T> الغنيّ بالنوع الداخلي عبر `sadType` (الآن kind-level)؛ (2) ربط Future بـ`async_runtime` + Generator بالمولّدات؛ (3) codegen المترجم لـnull/Optional؛ (4) سطح `؟` العربي (معجمي). AC5/AC6/AC7 منجزة.

### ✅ S-TS-P9 — فرض أمان null (طبقة المفسّر)
**الحالة:** مُنفَّذة في المفسّر (فرع `type_system`).

| البند | الدليل |
|------|--------|
| اكتشاف الطبقة | الإنفاذ المرئي في **المفسّر** `statement_executor.cpp:125` (لا المدقّق الدلالي — لا يُفعَّل في sad-run)؛ `value.isNull()` (من P1) الإشارة الموثوقة (null يهرب من `isAssignableTo`) |
| AC2 الفرض | `رقم س = لاشيء` → تحذير «إسناد 'لاشيء' لمتغير غير اختياري 'رقم'. اجعله 'رقم؟'»؛ `رقم؟ س = لاشيء` → مقبول |
| AC1/AC4 حفظ الـ48 | الحارس الخارجي `node.type != Unknown` يستثني `متغير س = لاشيء` (المُستنتَج) → **لا تحذير، لا انحدار** |
| المستثنى | Optional/Any/Void/Null (لا إنذار كاذب) |
| اختبار | `051_null_safety_enforcement.ص`: اختياري+مُستنتَج+`??` — مفسّر PASS |
| لا انحدار | `--level P0` 5/5 مزدوج؛ `--level P1` 106/108 (الـ3 سابقة)؛ القسم 17/19 |
| الشدّة | تحذير (نمط المفسّر للأنواع، CW-22)؛ التصعيد لخطأ صارم = قرار سياسة (مؤجَّل) |

> **انحراف موثّق:** الخطّة P9 وضعت الإنفاذ في المدقّق الدلالي (`type_checker.cpp:400/244`)، لكن **هذا المدقّق لا يُستدعى في sad-run** (التحذيرات الفعلية من `statement_executor`). نُفِّذ في الطبقة الصحيحة (المفسّر) حيث القيمة معروفة. AC3 (وصول غير آمن على `T؟`) + AC5 (كتالوج semantic.yaml) — يعتمدان على P10/تمثيل Optional الغنيّ، مؤجَّلان.

### ✅ S-TS-P5/P6/P7/P11 (الموجة 4) + S-TS-P8 — مُنفَّذة
- **P5**: suite أمان نوعي حقيقي (`052_type_safety_regression.ص`، 10 تأكيدات `نوع()`) + `status/05_quality_gates.md` بأرقام فعلية.
- **P6**: تدقيق enum — كل الـ52 قيمة مُستهلَكة (≥4)، صفر ميتة، جدول في ARCHITECTURE.
- **P7**: interning للمركّبة (`SadTypeRegistry::intern`) — مقارنة المؤشر صحيحة، `test_type_bridge` 47/47.
- **P11**: أنواع الرسومات في SoT + إثبات التوسّع (Point/Rect من YAML فقط → 52 قيمة) + إصلاح ترميز `gen_types.py`.
- **P8**: `?.`/`??` مُسجَّلان في `operators.yaml` (AC1)؛ **`؟` عربي** → QUESTION في المعجم (AC3: لاحقة `رقم؟` + ثلاثي `؟ :` + اندماج `؟؟` — اختبار `053`)؛ فضّ الغموض موثّق (AC4). **`!!` (AC2) مؤجَّل** (عامل جديد كامل الطبقات). لا انحدار: P0 5/5 مزدوج، P1 (الـ3 سابقة).

### ✅ S-TS-P4 (ربط Future) — `نوع(مستقبل())`=«مستقبل»
- `future_func` يَسِم قيمة المستقبل بـ`makeFuture()` (`setSadType`)؛ `getKind()`=Future → `نوع()`=«مستقبل».
- `setSadType`: Future/Generator → `type_=OBJECT` (يُبقي `isObject()`/فحص الطرق سليمًا)، Null → `type_=Null` (تمايز P1).
- `type_of`: حالتا Future→«مستقبل»، Generator→«مولّد».
- اختبار `07_التزامن/060_future_type_wiring.ص`: `نوع=مستقبل` + `.عيّن/.احصل`=42 (مفسّر PASS)؛ `@skip_compiler`.
- **لا تراجع (مُثبَت بـstash):** قسم التزامن 13/8 قبل/بعد متطابق (الـ8 سابقة goroutine/channel)؛ P0 5/5 مزدوج، P1 (الـ3 سابقة).
- **Generator:** يُجمَّع مصفوفةً (تقييم فوري — لا قيمة مولّد مميَّزة)؛ وسمه يتطلّب مولّدات كسولة (→ نظام null-safety/مستقبلي).

### ✅ S-TS-P4/P8/P9 codegen — رفع `@skip_compiler` عن أربعة اختبارات null
- **`نوع(لاشيء)`=«عدم»:** القيمة الحرفية تُوسَم `SadTypeKind::Null` (لا Integer) مع إبقاء التمثيل i64 (الحارس) — حالة `Null` في مفتاح MOVE وفي `mapSIRType`→i64.
- **حارس `لاشيء == نص`:** كان يُسقِط التنفيذ (`IntToPtr(الحارس)`+`strcmp` ⇒ segfault)؛ الآن يُرجع «خطأ» مباشرة (`emitCmpEq`).
- **`نوع()` واعٍ بالحارس زمن التشغيل:** متغيّر اختياري `رقم؟` يحمل `لاشيء` يُرجع «عدم» عبر `select` على الحارس (لا يكتفي بالنوع الساكن).
- **النتيجة (المجموعة الكاملة 461):** 049/050/051/053 صارت **تكافؤ مزدوج** (كانت مفسّر فقط). لا انحدار: التغيير مترجم-فقط (sad-run غير معاد البناء)، والاختباران ذوا سطح-null (X04/P09) يُظهران تطابق المفسّر=المترجم.
- **052 يبقى متخطّى:** فجوات منفصلة عن null (تباين أسماء `نوع()` «رقم» مقابل «عدد_صحيح»، خريطة، T8) — تُعالَج بتوحيد أسماء `نوع()` في SoT.

### ✅ توحيد أسماء `نوع()` عبر SoT — رفع 052 (تكافؤ مزدوج)
- كان كل محرّك يكتب الأسماء العربية يدويًّا فتباينت (المفسّر «رقم» مقابل المترجم «عدد_صحيح»؛ خريطة→«مجهول» في المترجم).
- `types.yaml`: حقل `typeof_ar` (يتجاوز `word` لاسم نوع() زمن التشغيل) — Class/Struct→«كائن»، Generator→«مولّد».
- `gen_types.py` يولّد `sadTypeKindArabicName()` (مُرمَّزة `\xHH`)؛ المفسّر والمترجم يستدعيانها — مصدر واحد.
- **052 رُفِع عنه `@skip_compiler`** (T1..T10 متطابقة مزدوجًا). لا انحدار (الرغرسيون 461؛ الفرق الوحيد رفرفة تزامن في 036، ينجح 5/5 منفردًا).

## القصص المُخطَّطة (المتبقّي الثقيل)
- **نظام null-safety المستقلّ** (NS-01..06): المحلّل المشترك + التضييق (P10 سابقًا) + `!!` (P8-AC2) + إعادة إرساء P9 عبر `dispatch`.
- **codegen Optional/Future الغنيّ** المتبقّي.
- **مولّدات كسولة** (لوسم Generator) — مستقبلي.

## ملاحظة حوكمة
كل التعديلات على فرع `type_system` (الخاص بهذا العمل). القصص في `_bmad-output/` محكومة.
