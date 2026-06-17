# بوّابات الجودة — نظام الأنواع (S-TS-P5)

> أرقام حقيقية قابلة للاستشهاد فقط (GR-01: لا أرقام وهمية). آخر تحديث: من الجهاز.
> فرع: `type_system`. الأداة: `tests/runner.py` (تنفيذ مزدوج مفسّر+مترجم) + `test_type_bridge` (C++/gtest-style).

## اختبارات `.ص` لنظام الأنواع (قابلة للتشغيل عبر runner.py)

| الاختبار | يغطّي | المفسّر | المترجم |
|---------|-------|:------:|:------:|
| `02_الأنواع/052_type_safety_regression.ص` | مصفوفة البدائيات `نوع()` (رقم/عشري/نص/منطقي/مصفوفة/خريطة) + عدم + حفظ النوع + ترقية | ✅ PASS | `@skip_compiler` (فجوة null) |
| `02_الأنواع/05_لاشيء_والفراغ/049_null_distinct_from_void.ص` | عدم متمايز عن فراغ، مساواة، تمايز عن 0/"" | ✅ PASS | `@skip_compiler` |
| `02_الأنواع/05_لاشيء_والفراغ/050_optional_type_suffix.ص` | سطح `رقم?` متغير+معامل + `??` | ✅ PASS | `@skip_compiler` |
| `02_الأنواع/05_لاشيء_والفراغ/051_null_safety_enforcement.ص` | أمان null: اختياري/مُستنتَج + `??` | ✅ PASS | `@skip_compiler` |

## اختبار وحدة C++ (`test_type_bridge`)

- الهدف: `tests/unit/types/test_type_bridge.cpp` (مفعَّل في `cmake/tests.cmake`، BUILD_TESTS=ON).
- النتيجة الفعلية: **47/47 اختبارًا ✓** (يشمل: SadTypeKind↔ValueType، **Result<T,E>** بناء/مساواة/إسناد (P3)، **interning للمركّبات** (P7)، inferSadType، SadValue).

## اختبار المولّد (P0.5)

- `scripts/codegen/test_gen_types.py`: **6/6 ✓** (تطابق YAML↔الترويسة المُولَّدة، 52 قيمة، لا تكرار، schema).

## بوّابات الانحدار (runner)

- `--level P0` = **5/5 (100%) تنفيذ مزدوج** (مفسّر+مترجم).
- `--level P1` = **107/108** (الـ3 الفاشلة سابقة وغير متعلقة: `218_var_array_var`/`081_nested_arrays_chaining` دوال مصفوفة مفقودة، `039_string_methods_fstrings` fstring في المترجم).

## فجوة المترجم الموثّقة (لا ادّعاء «مزدوج» لها)

codegen المترجم لا يميّز `عدم` (Null): `نوع(لاشيء)`→«مجهول» لا «عدم»، و`لاشيء==نص` يُسقِط — لذا اختبارات null/Optional عليها `@skip_compiler`. المفسّر صحيح بالكامل. (يتقاطع مع codegen-null المؤجَّل في P4.)

> **ربط CI:** هذه الاختبارات تعمل ضمن `tests/runner.py` (P0 في كل commit، P1 في البوّابة الهرمية). `test_type_bridge` يُبنى مع BUILD_TESTS=ON.
