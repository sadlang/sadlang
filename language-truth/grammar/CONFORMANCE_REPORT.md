# تقرير مطابقة قواعد لغة ص — المترجّم وحدَه

> **مُولَّد آلياً** بـ`scripts/codegen/check_grammar_conformance.py --run`. لا يُحرَّر يدوياً.
> التوليد: 2026-09-07 23:17:06

كل اختبار يُشغَّل عبر **المترجم** (`sad-build`) ويُقارَن مخرجُه بعقدِ البذرة:
`نجحت` = طابقَ العقدَ؛ `تباعد` = خالفَه (هنا يظهر ما يجب تصحيحه).

> ⚠️ **محرّكٌ واحد**: لم يجرِ في هذا الشوطِ محرّكٌ ثانٍ، فلا حكمَ هنا على تكافؤٍ بين محرّكَين — والقاعدةُ «مُغطّاة» تعني أنّ المترجّمَ وفّى بعقدِ بذورِها، لا أنّ محرّكَين اتّفقا.

## الملخص

- إجمالي الاختبارات: **115** — نجحت: **84** — تباعد/إخفاق: **30** — متخطًّى: **1**
- القواعد: 107 — مُغطّاة (المترجّم): **0** · مطلقة (محرّكان): 0 · فجوة مترجم: 0 · مكسورة: 0 · بلا اختبارات: 2 · لم تُشغَّل: 105
- الأزمنة: في أثر البناء `build/_grammar_conformance.json` (غير متعقَّب) — لا تُودَع لأنّ العدّاء يتوازى فلا يُعاد إنتاجها.
- التفصيل الكامل لكل اختبار: [`CONFORMANCE_REPORT_detail.md`](./CONFORMANCE_REPORT_detail.md)

## التباعدات والإخفاقات (للتصحيح)

| الاختبار | الحالة | مخرج المترجم |
|---|---|---|
| `tests/behavior/grammar_gaps/باني_يستدعي_طريقة/001_ctor_calls_method.ص` | تباعد المخرجات ⚠️ | `` |
| `tests/behavior/grammar_gaps/بنى_بيانات/003_flatten.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/بنى_بيانات/006_multi_assign.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/بنيات_متقدمة/007_generic.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/بنيات_متقدمة/008_async_await.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/تباعد_نوع_عند_إعادة_التصريح/001_match_arm_type_change.ص` | تباعد المخرجات ⚠️ | `42⏎نص` |
| `tests/behavior/grammar_gaps/تباعد_نوع_عند_إعادة_التصريح/002_match_arm_array_element_type.ص` | تباعد المخرجات ⚠️ | `أ` |
| `tests/behavior/grammar_gaps/تباعد_نوع_عند_إعادة_التصريح/003_match_arm_shadows_parameter.ص` | تباعد المخرجات ⚠️ | `1` |
| `tests/behavior/grammar_gaps/تساهل_الأسماء/002_return_top_level.ص` | تباعد المخرجات ⚠️ | `` |
| `tests/behavior/grammar_gaps/تصريحات/0012_static_array_module_unusable.ص` | تباعد المخرجات ⚠️ | `[RUN002] الفهرس 2 خارج نطاق مصفوفة (الطول 0)` |
| `tests/behavior/grammar_gaps/تصريحات/0013_static_array_in_function_diverges.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/تصريحات/0014_nameless_decl_swallows_next_line.ص` | تباعد المخرجات ⚠️ | `` |
| `tests/behavior/grammar_gaps/تصريحات/0015_tuple_destructure_compiler_crash.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/تصريحات/0016_tuple_destructure_in_function_crash.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/تصريحات/003_block_shadow.ص` | تباعد المخرجات ⚠️ | `2⏎2` |
| `tests/behavior/grammar_gaps/تعابير_متقدمة/001_spread_array.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/تعداد_تصادم_حقل/002_wrong_variant_field.ص` | تباعد المخرجات ⚠️ | `Error [RUN005]: ADT field access on a variant that` |
| `tests/behavior/grammar_gaps/تعداد_وسيط_وصول_حقل/001_untyped_adt_param_field.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/دقة_الصحيح_64بت/001_int64_precision_dual.ص` | تباعد المخرجات ⚠️ | `1099511627777⏎9007199254740993⏎9007199254740991⏎10` |
| `tests/behavior/grammar_gaps/طرق_المصفوفات/001_arr_add.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/طرق_المصفوفات/003_arr_first.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/طرق_النصوص/002_str_split.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/طرق_النصوص/003_str_trim.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/طرق_النصوص/006_str_substr.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/قائمة_عنصر_حرفيّ_مسطّح/001_literal_element_unchecked.ص` | تباعد المخرجات ⚠️ | `طابق` |
| `tests/behavior/grammar_gaps/متقدمة/001_yield_generator.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/متقدمة/002_ffi_extern.ص` | فشل الترجمة ❌ | `` |
| `tests/behavior/grammar_gaps/متقدمة/003_property_test_exit.ص` | تباعد المخرجات ⚠️ | `` |
| `tests/behavior/grammar_gaps/نمط_حقل_بنية_متداخل/001_struct_field_nested_compiler.ص` | تباعد المخرجات ⚠️ | `لا` |
| `tests/behavior/grammar_gaps/نمط_متداخل_مختلط/001_hetero_nested_compiler.ص` | تباعد المخرجات ⚠️ | `لا` |

## المتخطّى (لم يُشغَّل — غيرُ مقيسٍ لا ناجح)

| الاختبار | سببُ التخطّي |
|---|---|
| `tests/behavior/grammar_gaps/تعليب_عنصر_مصفوفة/001_int_array_float_element.ص` | تخطي: skip_compiler + skip_interpreter |

## اختبارات كاشفة للثغرات (Gaps) — غير مُبوَّبة (لا تُفشِل البناء)

تتعمّد اختبار ميزات مشكوكة لكشف ما لا يعمل. كاشفة: **115** — تكشف ثغرة: **31**. (راجع [DISCOVERED_ISSUES.md](./DISCOVERED_ISSUES.md))

| الاختبار | الثغرة | النتيجة |
|---|---|---|
| `001_go_single_line_compiler.ص` | ISSUE-072 | تعمل ✅ |
| `001_toplevel_yield_compiler.ص` | ISSUE-073 | تعمل ✅ |
| `001_list_comp.ص` | ISSUE-016 | تعمل ✅ |
| `002_dict_comp.ص` | ISSUE-017 | تعمل ✅ |
| `001_map_literal_index.ص` | تحقّق — خريطة حرفية + فهرسة | تعمل ✅ |
| `002_map_index_set.ص` | تحقّق — إسناد بالفهرس للخريطة | تعمل ✅ |
| `003_map_set_get_method.ص` | ISSUE-010 | تعمل ✅ |
| `004_map_keys.ص` | تحقّق — .مفاتيح() | تعمل ✅ |
| `005_map_values.ص` | تحقّق — .قيم() | تعمل ✅ |
| `006_map_size.ص` | تحقّق — .حجم() | تعمل ✅ |
| `001_ctor_calls_method.ص` | ISSUE-061 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `001_tuple.ص` | تحقّق — tuple | تعمل ✅ |
| `002_slice.ص` | تحقّق — .شريحة() | تعمل ✅ |
| `003_flatten.ص` | ISSUE-018 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `004_neg_index.ص` | تحقّق — فهرسة سالبة | تعمل ✅ |
| `005_range_pattern.ص` | تحقّق — نمط نطاق 1..10 | تعمل ✅ |
| `006_multi_assign.ص` | ISSUE-014 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `001_defer.ص` | تحقّق — أجّل | تعمل ✅ |
| `002_macro.ص` | تحقّق — ماكرو | تعمل ✅ |
| `003_extension.ص` | تحقّق — امتداد | تعمل ✅ |
| `004_contract_requires.ص` | تحقّق — عقد يتطلب | تعمل ✅ |
| `005_contract_ensures.ص` | تحقّق — عقد يضمن | تعمل ✅ |
| `006_directive_size.ص` | تحقّق — توجيه @حجم | تعمل ✅ |
| `007_generic.ص` | ISSUE-020 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `008_async_await.ص` | ISSUE-021 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `001_match_arm_type_change.ص` | ISSUE-101 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `002_match_arm_array_element_type.ص` | ISSUE-101 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `003_match_arm_shadows_parameter.ص` | ISSUE-103 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `001_branch_untaken_promotion.ص` | ISSUE-063 | تعمل ✅ |
| `002_branch_taken_promotion.ص` | ISSUE-063 | تعمل ✅ |
| `003_self_division_loop.ص` | ISSUE-063 | تعمل ✅ |
| `004_self_division_loop_int.ص` | ISSUE-063 | تعمل ✅ |
| `005_global_promotion_split.ص` | ISSUE-063 | تعمل ✅ |
| `006_dyn_into_field.ص` | ISSUE-063 | تعمل ✅ |
| `007_string_after_promotion.ص` | ISSUE-063 | تعمل ✅ |
| `001_channel.ص` | تحقّق — قناة إرسال/استقبال | تعمل ✅ |
| `002_select.ص` | تحقّق — اختر | تعمل ✅ |
| `003_waitgroup.ص` | تحقّق — مجموعة_انتظار | تعمل ✅ |
| `002_return_top_level.ص` | ISSUE-004 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `0010_topmod_mod_private.ص` | ISSUE-032 | تعمل ✅ |
| `0011_topmod_mod_static.ص` | ISSUE-032 | تعمل ✅ |
| `0012_static_array_module_unusable.ص` | ISSUE-122 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `0013_static_array_in_function_diverges.ص` | ISSUE-122 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `0014_nameless_decl_swallows_next_line.ص` | ISSUE-123 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `0015_tuple_destructure_compiler_crash.ص` | ISSUE-124 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `0016_tuple_destructure_in_function_crash.ص` | ISSUE-124 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `001_return_map.ص` | ISSUE-025 | تعمل ✅ |
| `002_export_struct.ص` | — | تعمل ✅ |
| `003_block_shadow.ص` | ISSUE-028 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `004_import_haseb_alias.ص` | ISSUE-027 | تعمل ✅ |
| `005_func_no_parens.ص` | ISSUE-029 | تعمل ✅ |
| `006_const_attr_mutable.ص` | ISSUE-030 | تعمل ✅ |
| `007_const_public_combo.ص` | ISSUE-031 | تعمل ✅ |
| `008_topmod_mod_const.ص` | ISSUE-032 | تعمل ✅ |
| `009_topmod_mod_public.ص` | ISSUE-032 | تعمل ✅ |
| `001_spread_array.ص` | ISSUE-012 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `002_nested_index.ص` | تحقّق — فهرسة متداخلة | تعمل ✅ |
| `002_wrong_variant_field.ص` | ISSUE-080 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `001_untyped_adt_param_field.ص` | ISSUE-080 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `001_int_array_float_element.ص` | ISSUE-080 | تكشف ثغرة ❌ (تخطٍّ) |
| `001_switch_only_default.ص` | ISSUE-054 | تعمل ✅ |
| `002_return_in_arm.ص` | ISSUE-055 | تعمل ✅ |
| `001_float_payload_return.ص` | ISSUE-084 | تعمل ✅ |
| `001_float_payload_in_list.ص` | ISSUE-084 | تعمل ✅ |
| `001_int64_precision_dual.ص` | ISSUE-INT64-PRECISION | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `001_lambda.ص` | تحقّق — لامدا | تعمل ✅ |
| `002_closure.ص` | تحقّق — إغلاق | تعمل ✅ |
| `003_pipeline.ص` | تحقّق — أنبوب \|> | تعمل ✅ |
| `004_reduce.ص` | تحقّق — .اختزل() | تعمل ✅ |
| `005_foreach.ص` | تحقّق — .لكل() | تعمل ✅ |
| `001_optional_chain_null_compiler.ص` | ISSUE-064 | تعمل ✅ |
| `001_free_slice_func_compiler.ص` | ISSUE-066 | تعمل ✅ |
| `001_open_slice_compiler.ص` | ISSUE-063 | تعمل ✅ |
| `001_tuple_dot_compiler.ص` | ISSUE-062 | تعمل ✅ |
| `001_arr_add.ص` | ISSUE-001 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `002_arr_length_method.ص` | تحقّق — .الطول() على مصفوفة | تعمل ✅ |
| `003_arr_first.ص` | ISSUE-008 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `004_arr_reverse.ص` | تحقّق — .عكس() | تعمل ✅ |
| `005_arr_sort.ص` | تحقّق — .رتب() | تعمل ✅ |
| `006_arr_map.ص` | تحقّق — .خريطة() | تعمل ✅ |
| `007_arr_filter.ص` | تحقّق — .رشح() | تعمل ✅ |
| `008_arr_pop.ص` | تحقّق — .احذف_اخير() | تعمل ✅ |
| `001_str_contains.ص` | تحقّق — .يحتوي() على نص | تعمل ✅ |
| `002_str_split.ص` | ISSUE-007 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `003_str_trim.ص` | ISSUE-009 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `004_str_startswith.ص` | تحقّق — .يبدأ_بـ() | تعمل ✅ |
| `005_str_replace.ص` | تحقّق — .استبدل() | تعمل ✅ |
| `006_str_substr.ص` | ISSUE-015 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `007_str_lower.ص` | تحقّق — .تحويل_صغير() | تعمل ✅ |
| `001_literal_element_unchecked.ص` | ISSUE-083 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `001_mixed_list_string_element.ص` | ISSUE-082 | تعمل ✅ |
| `001_template_string_arg_compiler.ص` | ISSUE-071 | تعمل ✅ |
| `001_class_method.ص` | تحقّق — صنف+باني+طريقة | تعمل ✅ |
| `002_field_access.ص` | تحقّق — وصول حقل | تعمل ✅ |
| `003_inherit.ص` | تحقّق — وراثة | تعمل ✅ |
| `005_struct.ص` | تحقّق — بنية | تعمل ✅ |
| `006_enum.ص` | تحقّق — تعداد | تعمل ✅ |
| `007_enum_value.ص` | تحقّق — تعداد بقيمة | تعمل ✅ |
| `008_trait_impl.ص` | تحقّق — سمة+نفّذ | تعمل ✅ |
| `009_static_method.ص` | تحقّق — دالة ساكنة | تعمل ✅ |
| `010_operator_overload.ص` | تحقّق — تحميل عامل + | تعمل ✅ |
| `011_property.ص` | تحقّق — خاصية احصل/عيّن | تعمل ✅ |
| `011_type_of_object.ص` | تحقّق — نوع() لكائن يُرجع «كائن» (إصلاح P0-1 / STORY-UI-W15-01) | تعمل ✅ |
| `012_ui_decl_instantiation.ص` | تحقّق — إنشاء مكوّن واجهة (الكلمة المفتاحية «واجهة» لتعريف مكوّن UI) | تعمل ✅ |
| `013_ui_decl_methods_inherit.ص` | تحقّق — دوال مكوّن «واجهة» ووراثته (STORY-UI-W15-02 AC-4/AC-5) | تعمل ✅ |
| `001_yield_generator.ص` | ISSUE-040 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `002_ffi_extern.ص` | ISSUE-041 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `003_property_test_exit.ص` | ISSUE-042 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `001_range_no_import.ص` | ISSUE-002 | تعمل ✅ |
| `002_range_with_import.ص` | تحقّق — مدى مع استيراد أساسيات | تعمل ✅ |
| `001_float_array_print.ص` | ISSUE-080 | تعمل ✅ |
| `002_float_array_to_string.ص` | ISSUE-080 | تعمل ✅ |
| `003_string_array_to_string.ص` | ISSUE-080 | تعمل ✅ |
| `001_struct_field_nested_compiler.ص` | ISSUE-070 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `001_hetero_nested_compiler.ص` | ISSUE-070 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |

## المقارنة المزدوجة لكل قاعدة

| القاعدة | الطبقة | اختبارات | الحُكم |
|---|---|---|---|
| `gr.adv.asm_dialect` | advanced | 30 | لم تُشغَّل |
| `gr.adv.await` | advanced | 4 | لم تُشغَّل |
| `gr.adv.contract` | advanced | 5 | لم تُشغَّل |
| `gr.adv.defer` | advanced | 4 | لم تُشغَّل |
| `gr.adv.dict_comprehension` | advanced | 65 | لم تُشغَّل |
| `gr.adv.ffi_ctype` | advanced | 0 | بلا اختبارات |
| `gr.adv.ffi_extern_block` | advanced | 2 | لم تُشغَّل |
| `gr.adv.ffi_linkage` | advanced | 1 | لم تُشغَّل |
| `gr.adv.go` | advanced | 3 | لم تُشغَّل |
| `gr.adv.inline_asm` | advanced | 2 | لم تُشغَّل |
| `gr.adv.lifetime_params` | advanced | 4 | لم تُشغَّل |
| `gr.adv.list_comprehension` | advanced | 106 | لم تُشغَّل |
| `gr.adv.macro` | advanced | 5 | لم تُشغَّل |
| `gr.adv.property_test` | advanced | 0 | بلا اختبارات |
| `gr.adv.select` | advanced | 4 | لم تُشغَّل |
| `gr.adv.set_comprehension` | advanced | 98 | لم تُشغَّل |
| `gr.adv.template_args` | advanced | 4 | لم تُشغَّل |
| `gr.adv.template_decl` | advanced | 7 | لم تُشغَّل |
| `gr.adv.template_params` | advanced | 8 | لم تُشغَّل |
| `gr.adv.type` | advanced | 10 | لم تُشغَّل |
| `gr.adv.ui_decl` | advanced | 4 | لم تُشغَّل |
| `gr.adv.ui_event` | advanced | 2 | لم تُشغَّل |
| `gr.adv.ui_modifier_chain` | advanced | 4 | لم تُشغَّل |
| `gr.adv.ui_state` | advanced | 4 | لم تُشغَّل |
| `gr.adv.where_clause` | advanced | 2 | لم تُشغَّل |
| `gr.adv.widget` | advanced | 4 | لم تُشغَّل |
| `gr.adv.with` | advanced | 4 | لم تُشغَّل |
| `gr.adv.yield` | advanced | 5 | لم تُشغَّل |
| `gr.decl.arg_list` | declarations | 17 | لم تُشغَّل |
| `gr.decl.export` | declarations | 28 | لم تُشغَّل |
| `gr.decl.extern` | declarations | 22 | لم تُشغَّل |
| `gr.decl.function` | declarations | 195 | لم تُشغَّل |
| `gr.decl.import` | declarations | 38 | لم تُشغَّل |
| `gr.decl.parameters` | declarations | 61 | لم تُشغَّل |
| `gr.decl.reexport` | declarations | 8 | لم تُشغَّل |
| `gr.decl.type_ref` | declarations | 7 | لم تُشغَّل |
| `gr.decl.variable` | declarations | 249 | لم تُشغَّل |
| `gr.expr.array_literal` | expressions | 57 | لم تُشغَّل |
| `gr.expr.assignment` | expressions | 36 | لم تُشغَّل |
| `gr.expr.bitwise_and` | expressions | 22 | لم تُشغَّل |
| `gr.expr.bitwise_or` | expressions | 20 | لم تُشغَّل |
| `gr.expr.bitwise_xor` | expressions | 21 | لم تُشغَّل |
| `gr.expr.comparison` | expressions | 57 | لم تُشغَّل |
| `gr.expr.decorator` | expressions | 12 | لم تُشغَّل |
| `gr.expr.directive` | expressions | 12 | لم تُشغَّل |
| `gr.expr.equality` | expressions | 38 | لم تُشغَّل |
| `gr.expr.expression` | expressions | 20 | لم تُشغَّل |
| `gr.expr.factor` | expressions | 55 | لم تُشغَّل |
| `gr.expr.fstring` | expressions | 20 | لم تُشغَّل |
| `gr.expr.lambda` | expressions | 36 | لم تُشغَّل |
| `gr.expr.logical_and` | expressions | 32 | لم تُشغَّل |
| `gr.expr.logical_or` | expressions | 34 | لم تُشغَّل |
| `gr.expr.map_literal` | expressions | 53 | لم تُشغَّل |
| `gr.expr.null_coalesce` | expressions | 20 | لم تُشغَّل |
| `gr.expr.pipeline` | expressions | 20 | لم تُشغَّل |
| `gr.expr.postfix` | expressions | 71 | لم تُشغَّل |
| `gr.expr.power` | expressions | 23 | لم تُشغَّل |
| `gr.expr.primary` | expressions | 65 | لم تُشغَّل |
| `gr.expr.range` | expressions | 28 | لم تُشغَّل |
| `gr.expr.term` | expressions | 86 | لم تُشغَّل |
| `gr.expr.ternary` | expressions | 27 | لم تُشغَّل |
| `gr.expr.unary` | expressions | 41 | لم تُشغَّل |
| `gr.lex.comment` | lexical | 20 | لم تُشغَّل |
| `gr.lex.double` | lexical | 15 | لم تُشغَّل |
| `gr.lex.fstring` | lexical | 11 | لم تُشغَّل |
| `gr.lex.identifier` | lexical | 21 | لم تُشغَّل |
| `gr.lex.integer` | lexical | 16 | لم تُشغَّل |
| `gr.lex.lifetime` | lexical | 10 | لم تُشغَّل |
| `gr.lex.raw_string` | lexical | 12 | لم تُشغَّل |
| `gr.lex.string` | lexical | 19 | لم تُشغَّل |
| `gr.oop.class` | oop | 128 | لم تُشغَّل |
| `gr.oop.constructor` | oop | 30 | لم تُشغَّل |
| `gr.oop.destructor` | oop | 11 | لم تُشغَّل |
| `gr.oop.enum` | oop | 31 | لم تُشغَّل |
| `gr.oop.extension` | oop | 12 | لم تُشغَّل |
| `gr.oop.field` | oop | 34 | لم تُشغَّل |
| `gr.oop.impl` | oop | 20 | لم تُشغَّل |
| `gr.oop.member` | oop | 34 | لم تُشغَّل |
| `gr.oop.method` | oop | 51 | لم تُشغَّل |
| `gr.oop.modifiers` | oop | 25 | لم تُشغَّل |
| `gr.oop.new` | oop | 14 | لم تُشغَّل |
| `gr.oop.operator` | oop | 18 | لم تُشغَّل |
| `gr.oop.property` | oop | 16 | لم تُشغَّل |
| `gr.oop.struct` | oop | 44 | لم تُشغَّل |
| `gr.oop.this_super` | oop | 11 | لم تُشغَّل |
| `gr.oop.trait` | oop | 24 | لم تُشغَّل |
| `gr.pattern.binding` | patterns | 4 | لم تُشغَّل |
| `gr.pattern.list` | patterns | 29 | لم تُشغَّل |
| `gr.pattern.or` | patterns | 7 | لم تُشغَّل |
| `gr.pattern.pattern` | patterns | 9 | لم تُشغَّل |
| `gr.pattern.primary` | patterns | 27 | لم تُشغَّل |
| `gr.pattern.struct` | patterns | 9 | لم تُشغَّل |
| `gr.program.block` | program | 39 | لم تُشغَّل |
| `gr.program.declaration` | program | 20 | لم تُشغَّل |
| `gr.program.program` | program | 20 | لم تُشغَّل |
| `gr.program.statement` | program | 19 | لم تُشغَّل |
| `gr.stmt.break` | statements | 39 | لم تُشغَّل |
| `gr.stmt.continue` | statements | 32 | لم تُشغَّل |
| `gr.stmt.expression` | statements | 114 | لم تُشغَّل |
| `gr.stmt.for` | statements | 250 | لم تُشغَّل |
| `gr.stmt.if` | statements | 237 | لم تُشغَّل |
| `gr.stmt.match` | statements | 213 | لم تُشغَّل |
| `gr.stmt.return` | statements | 75 | لم تُشغَّل |
| `gr.stmt.switch` | statements | 37 | لم تُشغَّل |
| `gr.stmt.throw` | statements | 27 | لم تُشغَّل |
| `gr.stmt.try` | statements | 55 | لم تُشغَّل |
| `gr.stmt.while` | statements | 204 | لم تُشغَّل |
