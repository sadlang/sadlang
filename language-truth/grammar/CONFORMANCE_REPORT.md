# تقرير مطابقة قواعد لغة ص — المترجّم وحدَه

> **مُولَّد آلياً** بـ`scripts/codegen/check_grammar_conformance.py --run`. لا يُحرَّر يدوياً.
> التوليد: 2026-09-04 08:50:41

كل اختبار يُشغَّل عبر **المترجم** (`sad-build`) ويُقارَن مخرجُه بعقدِ البذرة:
`نجحت` = طابقَ العقدَ؛ `تباعد` = خالفَه (هنا يظهر ما يجب تصحيحه).

> ⚠️ **محرّكٌ واحد**: لم يجرِ في هذا الشوطِ محرّكٌ ثانٍ، فلا حكمَ هنا على تكافؤٍ بين محرّكَين — والقاعدةُ «مُغطّاة» تعني أنّ المترجّمَ وفّى بعقدِ بذورِها، لا أنّ محرّكَين اتّفقا.

## الملخص

- إجمالي الاختبارات: **3051** — نجحت: **3031** — تباعد/إخفاق: **1** — متخطًّى: **19**
- القواعد: 107 — مُغطّاة (المترجّم): **104** · مطلقة (محرّكان): 0 · فجوة مترجم: 1 · مكسورة: 0 · بلا اختبارات: 2
- الأزمنة: في أثر البناء `build/_grammar_conformance.json` (غير متعقَّب) — لا تُودَع لأنّ العدّاء يتوازى فلا يُعاد إنتاجها.
- التفصيل الكامل لكل اختبار: [`CONFORMANCE_REPORT_detail.md`](./CONFORMANCE_REPORT_detail.md)

## التباعدات والإخفاقات (للتصحيح)

| الاختبار | الحالة | مخرج المترجم |
|---|---|---|
| `tests/behavior/rules_matrix/30_oop/gr.oop.extension/negative/060_ext_unknown_type.ص` | تباعد المخرجات ⚠️ | `1` |

## المتخطّى (لم يُشغَّل — غيرُ مقيسٍ لا ناجح)

| الاختبار | سببُ التخطّي |
|---|---|
| `tests/behavior/rules_matrix/10_statements/gr.stmt.break/edge/004_break_in_try_runs_finally.ص` | تخطي: skip_compiler + skip_interpreter |
| `tests/behavior/rules_matrix/10_statements/gr.stmt.continue/edge/004_continue_in_try_runs_finally.ص` | تخطي: skip_compiler + skip_interpreter |
| `tests/behavior/rules_matrix/20_declarations/gr.decl.reexport/basic/وحدة_مصدر.ص` | تخطي: skip_compiler + skip_interpreter |
| `tests/behavior/rules_matrix/20_declarations/gr.decl.variable/negative/060_typed_form_rejects_modifier.ص` | تخطي: @expect_error بنمطٍ بصيغةِ المفسّرِ بلا مقابلٍ مترجَم (@expect_compile_error / @expect_error_compiled) |
| `tests/behavior/rules_matrix/20_declarations/gr.decl.variable/negative/061_removed_word_double_typed_form.ص` | تخطي: @expect_error بنمطٍ بصيغةِ المفسّرِ بلا مقابلٍ مترجَم (@expect_compile_error / @expect_error_compiled) |
| `tests/behavior/rules_matrix/20_declarations/gr.decl.variable/negative/062_removed_word_double_keyword_form.ص` | تخطي: @expect_error بنمطٍ بصيغةِ المفسّرِ بلا مقابلٍ مترجَم (@expect_compile_error / @expect_error_compiled) |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/basic/005_aarch64_three_operand.ص` | تخطي: @arch aarch64 riscv64 والمضيف x86_64 |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/basic/006_aarch64_bit_ops.ص` | تخطي: @arch aarch64 والمضيف x86_64 |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/edge/054_aarch64_memory_addressing.ص` | تخطي: @arch aarch64 riscv64 والمضيف x86_64 |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/edge/055_aarch64_operand_tail.ص` | تخطي: @arch aarch64 riscv64 والمضيف x86_64 |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/edge/057_aarch64_barriers.ص` | تخطي: @arch aarch64 والمضيف x86_64 |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/negative/059_clobber_suggestion_specials.ص` | تخطي: @expect_error بنمطٍ بصيغةِ المفسّرِ بلا مقابلٍ مترجَم (@expect_compile_error / @expect_error_compiled) |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/negative/060_register_suggestion_content.ص` | تخطي: @expect_error بنمطٍ بصيغةِ المفسّرِ بلا مقابلٍ مترجَم (@expect_compile_error / @expect_error_compiled) |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/negative/062_arch_lexicon_rejects_foreign_mnemonic.ص` | تخطي: @arch aarch64 والمضيف x86_64 |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/negative/063_arch_lexicon_rejects_foreign_register.ص` | تخطي: @arch aarch64 والمضيف x86_64 |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.template_decl/negative/051_removed_word_double_return_type.ص` | تخطي: @expect_error بنمطٍ بصيغةِ المفسّرِ بلا مقابلٍ مترجَم (@expect_compile_error / @expect_error_compiled) |
| `tests/behavior/rules_matrix/70_lexical/gr.lex.string/edge/009_null_escape.ص` | تخطي: skip_compiler + skip_interpreter |
| `tests/behavior/rules_matrix/_interactions/_combos/b2_tuple_function_unpack.ص` | تخطي: skip_compiler + skip_interpreter |
| `tests/behavior/rules_matrix/_interactions/_combos/b3_precedence_stress.ص` | تخطي: skip_interpreter بدون @expected |

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
| `gr.adv.asm_dialect` | advanced | 30 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.await` | advanced | 4 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.contract` | advanced | 5 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.defer` | advanced | 4 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.dict_comprehension` | advanced | 65 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.ffi_ctype` | advanced | 0 | بلا اختبارات |
| `gr.adv.ffi_extern_block` | advanced | 2 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.ffi_linkage` | advanced | 1 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.go` | advanced | 3 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.inline_asm` | advanced | 2 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.lifetime_params` | advanced | 4 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.list_comprehension` | advanced | 106 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.macro` | advanced | 5 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.property_test` | advanced | 0 | بلا اختبارات |
| `gr.adv.select` | advanced | 4 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.set_comprehension` | advanced | 98 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.template_args` | advanced | 4 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.template_decl` | advanced | 7 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.template_params` | advanced | 8 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.type` | advanced | 10 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.ui_decl` | advanced | 4 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.ui_event` | advanced | 2 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.ui_modifier_chain` | advanced | 4 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.ui_state` | advanced | 4 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.where_clause` | advanced | 2 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.widget` | advanced | 4 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.with` | advanced | 4 | مُغطّاة (المترجّم وحدَه) |
| `gr.adv.yield` | advanced | 5 | مُغطّاة (المترجّم وحدَه) |
| `gr.decl.arg_list` | declarations | 17 | مُغطّاة (المترجّم وحدَه) |
| `gr.decl.export` | declarations | 28 | مُغطّاة (المترجّم وحدَه) |
| `gr.decl.extern` | declarations | 22 | مُغطّاة (المترجّم وحدَه) |
| `gr.decl.function` | declarations | 192 | مُغطّاة (المترجّم وحدَه) |
| `gr.decl.import` | declarations | 38 | مُغطّاة (المترجّم وحدَه) |
| `gr.decl.parameters` | declarations | 61 | مُغطّاة (المترجّم وحدَه) |
| `gr.decl.reexport` | declarations | 8 | مُغطّاة (المترجّم وحدَه) |
| `gr.decl.type_ref` | declarations | 7 | مُغطّاة (المترجّم وحدَه) |
| `gr.decl.variable` | declarations | 235 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.array_literal` | expressions | 57 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.assignment` | expressions | 36 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.bitwise_and` | expressions | 22 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.bitwise_or` | expressions | 20 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.bitwise_xor` | expressions | 21 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.comparison` | expressions | 57 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.decorator` | expressions | 12 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.directive` | expressions | 12 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.equality` | expressions | 36 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.expression` | expressions | 20 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.factor` | expressions | 54 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.fstring` | expressions | 20 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.lambda` | expressions | 35 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.logical_and` | expressions | 32 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.logical_or` | expressions | 34 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.map_literal` | expressions | 52 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.null_coalesce` | expressions | 20 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.pipeline` | expressions | 20 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.postfix` | expressions | 69 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.power` | expressions | 23 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.primary` | expressions | 62 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.range` | expressions | 28 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.term` | expressions | 85 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.ternary` | expressions | 27 | مُغطّاة (المترجّم وحدَه) |
| `gr.expr.unary` | expressions | 41 | مُغطّاة (المترجّم وحدَه) |
| `gr.lex.comment` | lexical | 20 | مُغطّاة (المترجّم وحدَه) |
| `gr.lex.double` | lexical | 15 | مُغطّاة (المترجّم وحدَه) |
| `gr.lex.fstring` | lexical | 11 | مُغطّاة (المترجّم وحدَه) |
| `gr.lex.identifier` | lexical | 21 | مُغطّاة (المترجّم وحدَه) |
| `gr.lex.integer` | lexical | 16 | مُغطّاة (المترجّم وحدَه) |
| `gr.lex.lifetime` | lexical | 10 | مُغطّاة (المترجّم وحدَه) |
| `gr.lex.raw_string` | lexical | 12 | مُغطّاة (المترجّم وحدَه) |
| `gr.lex.string` | lexical | 19 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.class` | oop | 128 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.constructor` | oop | 30 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.destructor` | oop | 11 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.enum` | oop | 31 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.extension` | oop | 12 | فجوة مترجم |
| `gr.oop.field` | oop | 31 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.impl` | oop | 20 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.member` | oop | 34 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.method` | oop | 47 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.modifiers` | oop | 25 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.new` | oop | 14 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.operator` | oop | 18 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.property` | oop | 16 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.struct` | oop | 44 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.this_super` | oop | 11 | مُغطّاة (المترجّم وحدَه) |
| `gr.oop.trait` | oop | 24 | مُغطّاة (المترجّم وحدَه) |
| `gr.pattern.binding` | patterns | 4 | مُغطّاة (المترجّم وحدَه) |
| `gr.pattern.list` | patterns | 29 | مُغطّاة (المترجّم وحدَه) |
| `gr.pattern.or` | patterns | 7 | مُغطّاة (المترجّم وحدَه) |
| `gr.pattern.pattern` | patterns | 9 | مُغطّاة (المترجّم وحدَه) |
| `gr.pattern.primary` | patterns | 27 | مُغطّاة (المترجّم وحدَه) |
| `gr.pattern.struct` | patterns | 9 | مُغطّاة (المترجّم وحدَه) |
| `gr.program.block` | program | 39 | مُغطّاة (المترجّم وحدَه) |
| `gr.program.declaration` | program | 20 | مُغطّاة (المترجّم وحدَه) |
| `gr.program.program` | program | 20 | مُغطّاة (المترجّم وحدَه) |
| `gr.program.statement` | program | 19 | مُغطّاة (المترجّم وحدَه) |
| `gr.stmt.break` | statements | 39 | مُغطّاة (المترجّم وحدَه) |
| `gr.stmt.continue` | statements | 32 | مُغطّاة (المترجّم وحدَه) |
| `gr.stmt.expression` | statements | 114 | مُغطّاة (المترجّم وحدَه) |
| `gr.stmt.for` | statements | 249 | مُغطّاة (المترجّم وحدَه) |
| `gr.stmt.if` | statements | 236 | مُغطّاة (المترجّم وحدَه) |
| `gr.stmt.match` | statements | 213 | مُغطّاة (المترجّم وحدَه) |
| `gr.stmt.return` | statements | 75 | مُغطّاة (المترجّم وحدَه) |
| `gr.stmt.switch` | statements | 37 | مُغطّاة (المترجّم وحدَه) |
| `gr.stmt.throw` | statements | 27 | مُغطّاة (المترجّم وحدَه) |
| `gr.stmt.try` | statements | 55 | مُغطّاة (المترجّم وحدَه) |
| `gr.stmt.while` | statements | 203 | مُغطّاة (المترجّم وحدَه) |
