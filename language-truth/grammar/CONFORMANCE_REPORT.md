# تقرير مطابقة قواعد لغة ص — مقارنة المفسر والمترجم

> **مُولَّد آلياً** بـ`scripts/codegen/check_grammar_conformance.py --run`. لا يُحرَّر يدوياً.
> التوليد: 2026-08-20 12:05:36

كل اختبار يُشغَّل عبر **المفسر** (sad-run) و**المترجم** (sadc) ويُقارَن مخرجاهما:
`تطابق` = المخرجان متطابقان؛ `تباعد` = اختلفا (هنا يظهر ما يجب تصحيحه).

## الملخص

- إجمالي الاختبارات: **3021** — تطابق مزدوج: **3013** — تباعد/إخفاق: **0** — متخطًّى: **8**
- القواعد: 107 — مطلقة: **105** · فجوة مترجم: 0 · مكسورة: 0 · بلا اختبارات: 2
- الأزمنة: في أثر البناء `build/_grammar_conformance.json` (غير متعقَّب) — لا تُودَع لأنّ العدّاء يتوازى فلا يُعاد إنتاجها.
- التفصيل الكامل لكل اختبار: [`CONFORMANCE_REPORT_detail.md`](./CONFORMANCE_REPORT_detail.md)

## التباعدات والإخفاقات (للتصحيح)

✅ **لا تباعد** — كل اختبار **شُغِّل** أعطى مخرجاً متطابقاً في المفسر والمترجم (و**8** لم يُشغَّل — انظر «المتخطّى» أدناه).

## المتخطّى (لم يُشغَّل — غيرُ مقيسٍ لا ناجح)

| الاختبار | سببُ التخطّي |
|---|---|
| `tests/behavior/rules_matrix/20_declarations/gr.decl.reexport/basic/وحدة_مصدر.ص` | تخطي: skip_compiler + skip_interpreter |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/basic/005_aarch64_three_operand.ص` | تخطي: @arch aarch64 riscv64 والمضيف x86_64 |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/basic/006_aarch64_bit_ops.ص` | تخطي: @arch aarch64 والمضيف x86_64 |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/edge/054_aarch64_memory_addressing.ص` | تخطي: @arch aarch64 riscv64 والمضيف x86_64 |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/edge/055_aarch64_operand_tail.ص` | تخطي: @arch aarch64 riscv64 والمضيف x86_64 |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/edge/057_aarch64_barriers.ص` | تخطي: @arch aarch64 والمضيف x86_64 |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/negative/062_arch_lexicon_rejects_foreign_mnemonic.ص` | تخطي: @arch aarch64 والمضيف x86_64 |
| `tests/behavior/rules_matrix/60_advanced/gr.adv.asm_dialect/negative/063_arch_lexicon_rejects_foreign_register.ص` | تخطي: @arch aarch64 والمضيف x86_64 |

## اختبارات كاشفة للثغرات (Gaps) — غير مُبوَّبة (لا تُفشِل البناء)

تتعمّد اختبار ميزات مشكوكة لكشف ما لا يعمل. كاشفة: **115** — تكشف ثغرة: **33**. (راجع [DISCOVERED_ISSUES.md](./DISCOVERED_ISSUES.md))

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
| `006_multi_assign.ص` | ISSUE-014 | تكشف ثغرة ❌ (فشل المفسر ❌) |
| `001_defer.ص` | تحقّق — أجّل | تعمل ✅ |
| `002_macro.ص` | تحقّق — ماكرو | تعمل ✅ |
| `003_extension.ص` | تحقّق — امتداد | تعمل ✅ |
| `004_contract_requires.ص` | تحقّق — عقد يتطلب | تعمل ✅ |
| `005_contract_ensures.ص` | تحقّق — عقد يضمن | تعمل ✅ |
| `006_directive_size.ص` | تحقّق — توجيه @حجم | تعمل ✅ |
| `007_generic.ص` | ISSUE-020 | تكشف ثغرة ❌ (فشل المفسر ❌) |
| `008_async_await.ص` | ISSUE-021 | تكشف ثغرة ❌ (فشل المفسر ❌) |
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
| `0012_static_array_module_unusable.ص` | ISSUE-122 | تكشف ثغرة ❌ (فشل المفسر ❌) |
| `0013_static_array_in_function_diverges.ص` | ISSUE-122 | تكشف ثغرة ❌ (فشل المفسر ❌) |
| `0014_nameless_decl_swallows_next_line.ص` | ISSUE-123 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `0015_tuple_destructure_compiler_crash.ص` | ISSUE-124 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `0016_tuple_destructure_in_function_crash.ص` | ISSUE-124 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
| `001_return_map.ص` | ISSUE-025 | تعمل ✅ |
| `002_export_struct.ص` | — | تعمل ✅ |
| `003_block_shadow.ص` | ISSUE-028 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `004_import_haseb_alias.ص` | ISSUE-027 | تكشف ثغرة ❌ (فشل المفسر ❌) |
| `005_func_no_parens.ص` | ISSUE-029 | تعمل ✅ |
| `006_const_attr_mutable.ص` | ISSUE-030 | تعمل ✅ |
| `007_const_public_combo.ص` | ISSUE-031 | تعمل ✅ |
| `008_topmod_mod_const.ص` | ISSUE-032 | تعمل ✅ |
| `009_topmod_mod_public.ص` | ISSUE-032 | تعمل ✅ |
| `001_spread_array.ص` | ISSUE-012 | تكشف ثغرة ❌ (فشل المفسر ❌) |
| `002_nested_index.ص` | تحقّق — فهرسة متداخلة | تعمل ✅ |
| `002_wrong_variant_field.ص` | ISSUE-080 | تكشف ثغرة ❌ (فشل المفسر ❌) |
| `001_untyped_adt_param_field.ص` | ISSUE-080 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `001_int_array_float_element.ص` | ISSUE-080 | تعمل ✅ |
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
| `001_arr_add.ص` | ISSUE-001 | تكشف ثغرة ❌ (فشل المفسر ❌) |
| `002_arr_length_method.ص` | تحقّق — .الطول() على مصفوفة | تعمل ✅ |
| `003_arr_first.ص` | ISSUE-008 | تكشف ثغرة ❌ (فشل المفسر ❌) |
| `004_arr_reverse.ص` | تحقّق — .عكس() | تعمل ✅ |
| `005_arr_sort.ص` | تحقّق — .رتب() | تعمل ✅ |
| `006_arr_map.ص` | تحقّق — .خريطة() | تعمل ✅ |
| `007_arr_filter.ص` | تحقّق — .رشح() | تعمل ✅ |
| `008_arr_pop.ص` | تحقّق — .احذف_اخير() | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `001_str_contains.ص` | تحقّق — .يحتوي() على نص | تعمل ✅ |
| `002_str_split.ص` | ISSUE-007 | تكشف ثغرة ❌ (فشل المفسر ❌) |
| `003_str_trim.ص` | ISSUE-009 | تكشف ثغرة ❌ (فشل المفسر ❌) |
| `004_str_startswith.ص` | تحقّق — .يبدأ_بـ() | تعمل ✅ |
| `005_str_replace.ص` | تحقّق — .استبدل() | تعمل ✅ |
| `006_str_substr.ص` | ISSUE-015 | تكشف ثغرة ❌ (فشل المفسر ❌) |
| `007_str_lower.ص` | تحقّق — .تحويل_صغير() | تعمل ✅ |
| `001_literal_element_unchecked.ص` | ISSUE-083 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) |
| `001_mixed_list_string_element.ص` | ISSUE-082 | تعمل ✅ |
| `001_template_string_arg_compiler.ص` | ISSUE-071 | تكشف ثغرة ❌ (فشل الترجمة ❌) |
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
| `001_yield_generator.ص` | ISSUE-040 | تكشف ثغرة ❌ (فشل المفسر ❌) |
| `002_ffi_extern.ص` | ISSUE-041 | تكشف ثغرة ❌ (فشل المفسر ❌) |
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
| `gr.adv.asm_dialect` | advanced | 30 | مطلقة (مفسر≡مترجم) |
| `gr.adv.await` | advanced | 4 | مطلقة (مفسر≡مترجم) |
| `gr.adv.contract` | advanced | 5 | مطلقة (مفسر≡مترجم) |
| `gr.adv.defer` | advanced | 4 | مطلقة (مفسر≡مترجم) |
| `gr.adv.dict_comprehension` | advanced | 65 | مطلقة (مفسر≡مترجم) |
| `gr.adv.ffi_ctype` | advanced | 0 | بلا اختبارات |
| `gr.adv.ffi_extern_block` | advanced | 2 | مطلقة (مفسر≡مترجم) |
| `gr.adv.ffi_linkage` | advanced | 1 | مطلقة (مفسر≡مترجم) |
| `gr.adv.go` | advanced | 3 | مطلقة (مفسر≡مترجم) |
| `gr.adv.inline_asm` | advanced | 2 | مطلقة (مفسر≡مترجم) |
| `gr.adv.lifetime_params` | advanced | 4 | مطلقة (مفسر≡مترجم) |
| `gr.adv.list_comprehension` | advanced | 106 | مطلقة (مفسر≡مترجم) |
| `gr.adv.macro` | advanced | 5 | مطلقة (مفسر≡مترجم) |
| `gr.adv.property_test` | advanced | 0 | بلا اختبارات |
| `gr.adv.select` | advanced | 4 | مطلقة (مفسر≡مترجم) |
| `gr.adv.set_comprehension` | advanced | 98 | مطلقة (مفسر≡مترجم) |
| `gr.adv.template_args` | advanced | 4 | مطلقة (مفسر≡مترجم) |
| `gr.adv.template_decl` | advanced | 7 | مطلقة (مفسر≡مترجم) |
| `gr.adv.template_params` | advanced | 5 | مطلقة (مفسر≡مترجم) |
| `gr.adv.type` | advanced | 10 | مطلقة (مفسر≡مترجم) |
| `gr.adv.ui_decl` | advanced | 4 | مطلقة (مفسر≡مترجم) |
| `gr.adv.ui_event` | advanced | 2 | مطلقة (مفسر≡مترجم) |
| `gr.adv.ui_modifier_chain` | advanced | 4 | مطلقة (مفسر≡مترجم) |
| `gr.adv.ui_state` | advanced | 4 | مطلقة (مفسر≡مترجم) |
| `gr.adv.where_clause` | advanced | 2 | مطلقة (مفسر≡مترجم) |
| `gr.adv.widget` | advanced | 4 | مطلقة (مفسر≡مترجم) |
| `gr.adv.with` | advanced | 4 | مطلقة (مفسر≡مترجم) |
| `gr.adv.yield` | advanced | 5 | مطلقة (مفسر≡مترجم) |
| `gr.decl.arg_list` | declarations | 17 | مطلقة (مفسر≡مترجم) |
| `gr.decl.export` | declarations | 28 | مطلقة (مفسر≡مترجم) |
| `gr.decl.extern` | declarations | 22 | مطلقة (مفسر≡مترجم) |
| `gr.decl.function` | declarations | 192 | مطلقة (مفسر≡مترجم) |
| `gr.decl.import` | declarations | 38 | مطلقة (مفسر≡مترجم) |
| `gr.decl.parameters` | declarations | 56 | مطلقة (مفسر≡مترجم) |
| `gr.decl.reexport` | declarations | 8 | مطلقة (مفسر≡مترجم) |
| `gr.decl.type_ref` | declarations | 7 | مطلقة (مفسر≡مترجم) |
| `gr.decl.variable` | declarations | 217 | مطلقة (مفسر≡مترجم) |
| `gr.expr.array_literal` | expressions | 57 | مطلقة (مفسر≡مترجم) |
| `gr.expr.assignment` | expressions | 36 | مطلقة (مفسر≡مترجم) |
| `gr.expr.bitwise_and` | expressions | 22 | مطلقة (مفسر≡مترجم) |
| `gr.expr.bitwise_or` | expressions | 20 | مطلقة (مفسر≡مترجم) |
| `gr.expr.bitwise_xor` | expressions | 21 | مطلقة (مفسر≡مترجم) |
| `gr.expr.comparison` | expressions | 57 | مطلقة (مفسر≡مترجم) |
| `gr.expr.decorator` | expressions | 12 | مطلقة (مفسر≡مترجم) |
| `gr.expr.directive` | expressions | 12 | مطلقة (مفسر≡مترجم) |
| `gr.expr.equality` | expressions | 36 | مطلقة (مفسر≡مترجم) |
| `gr.expr.expression` | expressions | 20 | مطلقة (مفسر≡مترجم) |
| `gr.expr.factor` | expressions | 54 | مطلقة (مفسر≡مترجم) |
| `gr.expr.fstring` | expressions | 20 | مطلقة (مفسر≡مترجم) |
| `gr.expr.lambda` | expressions | 35 | مطلقة (مفسر≡مترجم) |
| `gr.expr.logical_and` | expressions | 32 | مطلقة (مفسر≡مترجم) |
| `gr.expr.logical_or` | expressions | 34 | مطلقة (مفسر≡مترجم) |
| `gr.expr.map_literal` | expressions | 52 | مطلقة (مفسر≡مترجم) |
| `gr.expr.null_coalesce` | expressions | 20 | مطلقة (مفسر≡مترجم) |
| `gr.expr.pipeline` | expressions | 20 | مطلقة (مفسر≡مترجم) |
| `gr.expr.postfix` | expressions | 69 | مطلقة (مفسر≡مترجم) |
| `gr.expr.power` | expressions | 23 | مطلقة (مفسر≡مترجم) |
| `gr.expr.primary` | expressions | 62 | مطلقة (مفسر≡مترجم) |
| `gr.expr.range` | expressions | 28 | مطلقة (مفسر≡مترجم) |
| `gr.expr.term` | expressions | 81 | مطلقة (مفسر≡مترجم) |
| `gr.expr.ternary` | expressions | 27 | مطلقة (مفسر≡مترجم) |
| `gr.expr.unary` | expressions | 41 | مطلقة (مفسر≡مترجم) |
| `gr.lex.comment` | lexical | 20 | مطلقة (مفسر≡مترجم) |
| `gr.lex.double` | lexical | 15 | مطلقة (مفسر≡مترجم) |
| `gr.lex.fstring` | lexical | 11 | مطلقة (مفسر≡مترجم) |
| `gr.lex.identifier` | lexical | 21 | مطلقة (مفسر≡مترجم) |
| `gr.lex.integer` | lexical | 16 | مطلقة (مفسر≡مترجم) |
| `gr.lex.lifetime` | lexical | 10 | مطلقة (مفسر≡مترجم) |
| `gr.lex.raw_string` | lexical | 12 | مطلقة (مفسر≡مترجم) |
| `gr.lex.string` | lexical | 19 | مطلقة (مفسر≡مترجم) |
| `gr.oop.class` | oop | 128 | مطلقة (مفسر≡مترجم) |
| `gr.oop.constructor` | oop | 30 | مطلقة (مفسر≡مترجم) |
| `gr.oop.destructor` | oop | 11 | مطلقة (مفسر≡مترجم) |
| `gr.oop.enum` | oop | 31 | مطلقة (مفسر≡مترجم) |
| `gr.oop.extension` | oop | 12 | مطلقة (مفسر≡مترجم) |
| `gr.oop.field` | oop | 31 | مطلقة (مفسر≡مترجم) |
| `gr.oop.impl` | oop | 20 | مطلقة (مفسر≡مترجم) |
| `gr.oop.member` | oop | 34 | مطلقة (مفسر≡مترجم) |
| `gr.oop.method` | oop | 47 | مطلقة (مفسر≡مترجم) |
| `gr.oop.modifiers` | oop | 25 | مطلقة (مفسر≡مترجم) |
| `gr.oop.new` | oop | 14 | مطلقة (مفسر≡مترجم) |
| `gr.oop.operator` | oop | 18 | مطلقة (مفسر≡مترجم) |
| `gr.oop.property` | oop | 16 | مطلقة (مفسر≡مترجم) |
| `gr.oop.struct` | oop | 44 | مطلقة (مفسر≡مترجم) |
| `gr.oop.this_super` | oop | 11 | مطلقة (مفسر≡مترجم) |
| `gr.oop.trait` | oop | 24 | مطلقة (مفسر≡مترجم) |
| `gr.pattern.binding` | patterns | 4 | مطلقة (مفسر≡مترجم) |
| `gr.pattern.list` | patterns | 29 | مطلقة (مفسر≡مترجم) |
| `gr.pattern.or` | patterns | 7 | مطلقة (مفسر≡مترجم) |
| `gr.pattern.pattern` | patterns | 9 | مطلقة (مفسر≡مترجم) |
| `gr.pattern.primary` | patterns | 27 | مطلقة (مفسر≡مترجم) |
| `gr.pattern.struct` | patterns | 9 | مطلقة (مفسر≡مترجم) |
| `gr.program.block` | program | 39 | مطلقة (مفسر≡مترجم) |
| `gr.program.declaration` | program | 20 | مطلقة (مفسر≡مترجم) |
| `gr.program.program` | program | 20 | مطلقة (مفسر≡مترجم) |
| `gr.program.statement` | program | 19 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.break` | statements | 39 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.continue` | statements | 32 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.expression` | statements | 114 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.for` | statements | 249 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.if` | statements | 236 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.match` | statements | 213 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.return` | statements | 75 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.switch` | statements | 37 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.throw` | statements | 27 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.try` | statements | 55 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.while` | statements | 203 | مطلقة (مفسر≡مترجم) |
