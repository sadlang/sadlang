# تقرير مطابقة قواعد لغة ص — مقارنة المفسر والمترجم

> **مُولَّد آلياً** بـ`scripts/codegen/check_grammar_conformance.py --run`. لا يُحرَّر يدوياً.
> التوليد: 2026-08-10 23:12:59

كل اختبار يُشغَّل عبر **المفسر** (sad-run) و**المترجم** (sadc) ويُقارَن مخرجاهما:
`تطابق` = المخرجان متطابقان؛ `تباعد` = اختلفا (هنا يظهر ما يجب تصحيحه).

## الملخص

- إجمالي الاختبارات: **2915** — تطابق مزدوج: **2914** — تباعد/إخفاق: **0** — متخطًّى: **1**
- القواعد: 107 — مطلقة: **105** · فجوة مترجم: 0 · مكسورة: 0 · بلا اختبارات: 2
- زمن التنفيذ: المفسر **220.3s** (متوسط 76ms/اختبار) · المترجم **8309.9s** (متوسط 2851ms/اختبار)
- التفصيل الكامل لكل اختبار: [`CONFORMANCE_REPORT_detail.md`](./CONFORMANCE_REPORT_detail.md)

## التباعدات والإخفاقات (للتصحيح)

✅ **لا تباعد** — كل اختبار **شُغِّل** أعطى مخرجاً متطابقاً في المفسر والمترجم (و**1** لم يُشغَّل — انظر «المتخطّى» أدناه).

## المتخطّى (لم يُشغَّل — غيرُ مقيسٍ لا ناجح)

| الاختبار | سببُ التخطّي |
|---|---|
| `tests/behavior/rules_matrix/20_declarations/gr.decl.reexport/basic/وحدة_مصدر.ص` | تخطي: skip_compiler + skip_interpreter |

## اختبارات كاشفة للثغرات (Gaps) — غير مُبوَّبة (لا تُفشِل البناء)

تتعمّد اختبار ميزات مشكوكة لكشف ما لا يعمل. كاشفة: **110** — تكشف ثغرة: **33**. (راجع [DISCOVERED_ISSUES.md](./DISCOVERED_ISSUES.md))

| الاختبار | الثغرة | النتيجة | مفسر(ms) | مترجم(ms) |
|---|---|---|---|---|
| `001_go_single_line_compiler.ص` | ISSUE-072 | تعمل ✅ | 81 | 3041 |
| `001_toplevel_yield_compiler.ص` | ISSUE-073 | تعمل ✅ | 82 | 0 |
| `001_list_comp.ص` | ISSUE-016 | تعمل ✅ | 68 | 3012 |
| `002_dict_comp.ص` | ISSUE-017 | تعمل ✅ | 76 | 3796 |
| `001_map_literal_index.ص` | تحقّق — خريطة حرفية + فهرسة | تعمل ✅ | 74 | 3136 |
| `002_map_index_set.ص` | تحقّق — إسناد بالفهرس للخريطة | تعمل ✅ | 58 | 3297 |
| `003_map_set_get_method.ص` | ISSUE-010 | تعمل ✅ | 63 | 3850 |
| `004_map_keys.ص` | تحقّق — .مفاتيح() | تعمل ✅ | 60 | 3330 |
| `005_map_values.ص` | تحقّق — .قيم() | تعمل ✅ | 68 | 3384 |
| `006_map_size.ص` | تحقّق — .حجم() | تعمل ✅ | 68 | 3035 |
| `001_ctor_calls_method.ص` | ISSUE-061 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) | 53 | 2925 |
| `001_tuple.ص` | تحقّق — tuple | تعمل ✅ | 62 | 2922 |
| `002_slice.ص` | تحقّق — .شريحة() | تعمل ✅ | 78 | 3015 |
| `003_flatten.ص` | ISSUE-018 | تكشف ثغرة ❌ (فشل الترجمة ❌) | 67 | 75 |
| `004_neg_index.ص` | تحقّق — فهرسة سالبة | تعمل ✅ | 50 | 3191 |
| `005_range_pattern.ص` | تحقّق — نمط نطاق 1..10 | تعمل ✅ | 128 | 2967 |
| `006_multi_assign.ص` | ISSUE-014 | تكشف ثغرة ❌ (فشل المفسر ❌) | 57 | 0 |
| `001_defer.ص` | تحقّق — أجّل | تعمل ✅ | 65 | 3739 |
| `002_macro.ص` | تحقّق — ماكرو | تعمل ✅ | 66 | 3065 |
| `003_extension.ص` | تحقّق — امتداد | تعمل ✅ | 134 | 2914 |
| `004_contract_requires.ص` | تحقّق — عقد يتطلب | تعمل ✅ | 57 | 4004 |
| `005_contract_ensures.ص` | تحقّق — عقد يضمن | تعمل ✅ | 53 | 3399 |
| `006_directive_size.ص` | تحقّق — توجيه @حجم | تعمل ✅ | 69 | 2977 |
| `007_generic.ص` | ISSUE-020 | تكشف ثغرة ❌ (فشل المفسر ❌) | 63 | 0 |
| `008_async_await.ص` | ISSUE-021 | تكشف ثغرة ❌ (فشل المفسر ❌) | 77 | 0 |
| `001_match_arm_type_change.ص` | ISSUE-101 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) | 104 | 3398 |
| `002_match_arm_array_element_type.ص` | ISSUE-101 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) | 57 | 3445 |
| `003_match_arm_shadows_parameter.ص` | ISSUE-103 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) | 61 | 3799 |
| `001_branch_untaken_promotion.ص` | ISSUE-063 | تعمل ✅ | 61 | 3462 |
| `002_branch_taken_promotion.ص` | ISSUE-063 | تعمل ✅ | 59 | 3549 |
| `003_self_division_loop.ص` | ISSUE-063 | تعمل ✅ | 57 | 3514 |
| `004_self_division_loop_int.ص` | ISSUE-063 | تعمل ✅ | 60 | 3791 |
| `005_global_promotion_split.ص` | ISSUE-063 | تعمل ✅ | 62 | 4002 |
| `006_dyn_into_field.ص` | ISSUE-063 | تعمل ✅ | 60 | 3496 |
| `007_string_after_promotion.ص` | ISSUE-063 | تعمل ✅ | 57 | 3476 |
| `001_channel.ص` | تحقّق — قناة إرسال/استقبال | تعمل ✅ | 70 | 3031 |
| `002_select.ص` | تحقّق — اختر | تعمل ✅ | 66 | 3060 |
| `003_waitgroup.ص` | تحقّق — مجموعة_انتظار | تعمل ✅ | 58 | 2958 |
| `002_return_top_level.ص` | ISSUE-004 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) | 64 | 0 |
| `0010_topmod_mod_private.ص` | ISSUE-032 | تكشف ثغرة ❌ (فشل الترجمة ❌) | 74 | 103 |
| `0011_topmod_mod_static.ص` | ISSUE-032 | تكشف ثغرة ❌ (فشل المفسر ❌) | 62 | 0 |
| `001_return_map.ص` | ISSUE-025 | تعمل ✅ | 91 | 3483 |
| `002_export_struct.ص` | — | تعمل ✅ | 69 | 2965 |
| `003_block_shadow.ص` | ISSUE-028 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) | 59 | 2916 |
| `004_import_haseb_alias.ص` | ISSUE-027 | تكشف ثغرة ❌ (فشل المفسر ❌) | 62 | 0 |
| `005_func_no_parens.ص` | ISSUE-029 | تعمل ✅ | 56 | 0 |
| `006_const_attr_mutable.ص` | ISSUE-030 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) | 130 | 0 |
| `007_const_public_combo.ص` | ISSUE-031 | تكشف ثغرة ❌ (فشل المفسر ❌) | 63 | 0 |
| `008_topmod_mod_const.ص` | ISSUE-032 | تكشف ثغرة ❌ (فشل الترجمة ❌) | 63 | 83 |
| `009_topmod_mod_public.ص` | ISSUE-032 | تكشف ثغرة ❌ (فشل الترجمة ❌) | 60 | 64 |
| `001_spread_array.ص` | ISSUE-012 | تكشف ثغرة ❌ (فشل المفسر ❌) | 91 | 0 |
| `002_nested_index.ص` | تحقّق — فهرسة متداخلة | تعمل ✅ | 58 | 3146 |
| `002_wrong_variant_field.ص` | ISSUE-080 | تكشف ثغرة ❌ (فشل المفسر ❌) | 62 | 0 |
| `001_untyped_adt_param_field.ص` | ISSUE-080 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) | 62 | 3627 |
| `001_int_array_float_element.ص` | ISSUE-080 | تعمل ✅ | 59 | 0 |
| `001_switch_only_default.ص` | ISSUE-054 | تعمل ✅ | 126 | 3090 |
| `002_return_in_arm.ص` | ISSUE-055 | تعمل ✅ | 58 | 3730 |
| `001_float_payload_return.ص` | ISSUE-084 | تعمل ✅ | 66 | 4088 |
| `001_float_payload_in_list.ص` | ISSUE-084 | تعمل ✅ | 62 | 3161 |
| `001_int64_precision_dual.ص` | ISSUE-INT64-PRECISION | تعمل ✅ | 64 | 3103 |
| `001_lambda.ص` | تحقّق — لامدا | تعمل ✅ | 66 | 3121 |
| `002_closure.ص` | تحقّق — إغلاق | تعمل ✅ | 181 | 3556 |
| `003_pipeline.ص` | تحقّق — أنبوب |> | تعمل ✅ | 61 | 3489 |
| `004_reduce.ص` | تحقّق — .اختزل() | تعمل ✅ | 66 | 3146 |
| `005_foreach.ص` | تحقّق — .لكل() | تعمل ✅ | 62 | 3103 |
| `001_optional_chain_null_compiler.ص` | ISSUE-064 | تعمل ✅ | 64 | 2796 |
| `001_free_slice_func_compiler.ص` | ISSUE-066 | تعمل ✅ | 64 | 3192 |
| `001_open_slice_compiler.ص` | ISSUE-063 | تعمل ✅ | 67 | 3046 |
| `001_tuple_dot_compiler.ص` | ISSUE-062 | تعمل ✅ | 65 | 3085 |
| `001_arr_add.ص` | ISSUE-001 | تكشف ثغرة ❌ (فشل المفسر ❌) | 73 | 0 |
| `002_arr_length_method.ص` | تحقّق — .الطول() على مصفوفة | تعمل ✅ | 70 | 3003 |
| `003_arr_first.ص` | ISSUE-008 | تكشف ثغرة ❌ (فشل المفسر ❌) | 136 | 0 |
| `004_arr_reverse.ص` | تحقّق — .عكس() | تعمل ✅ | 55 | 3160 |
| `005_arr_sort.ص` | تحقّق — .رتب() | تعمل ✅ | 64 | 3170 |
| `006_arr_map.ص` | تحقّق — .خريطة() | تعمل ✅ | 66 | 3164 |
| `007_arr_filter.ص` | تحقّق — .رشح() | تعمل ✅ | 57 | 3330 |
| `008_arr_pop.ص` | تحقّق — .احذف_اخير() | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) | 62 | 2965 |
| `001_str_contains.ص` | تحقّق — .يحتوي() على نص | تعمل ✅ | 59 | 2948 |
| `002_str_split.ص` | ISSUE-007 | تكشف ثغرة ❌ (فشل المفسر ❌) | 59 | 0 |
| `003_str_trim.ص` | ISSUE-009 | تكشف ثغرة ❌ (فشل المفسر ❌) | 67 | 0 |
| `004_str_startswith.ص` | تحقّق — .يبدأ_بـ() | تعمل ✅ | 64 | 2882 |
| `005_str_replace.ص` | تحقّق — .استبدل() | تعمل ✅ | 65 | 2961 |
| `006_str_substr.ص` | ISSUE-015 | تكشف ثغرة ❌ (فشل المفسر ❌) | 64 | 0 |
| `007_str_lower.ص` | تحقّق — .تحويل_صغير() | تعمل ✅ | 55 | 2814 |
| `001_literal_element_unchecked.ص` | ISSUE-083 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) | 60 | 2972 |
| `001_mixed_list_string_element.ص` | ISSUE-082 | تعمل ✅ | 64 | 2975 |
| `001_template_string_arg_compiler.ص` | ISSUE-071 | تكشف ثغرة ❌ (تجاوز المهلة ⏱) | 56 | 30057 |
| `001_class_method.ص` | تحقّق — صنف+باني+طريقة | تعمل ✅ | 73 | 2650 |
| `002_field_access.ص` | تحقّق — وصول حقل | تعمل ✅ | 61 | 2633 |
| `003_inherit.ص` | تحقّق — وراثة | تعمل ✅ | 59 | 2626 |
| `005_struct.ص` | تحقّق — بنية | تعمل ✅ | 61 | 2845 |
| `006_enum.ص` | تحقّق — تعداد | تعمل ✅ | 70 | 2956 |
| `007_enum_value.ص` | تحقّق — تعداد بقيمة | تعمل ✅ | 70 | 2948 |
| `008_trait_impl.ص` | تحقّق — سمة+نفّذ | تعمل ✅ | 73 | 2686 |
| `009_static_method.ص` | تحقّق — دالة ساكنة | تعمل ✅ | 51 | 2655 |
| `010_operator_overload.ص` | تحقّق — تحميل عامل + | تعمل ✅ | 57 | 2788 |
| `011_property.ص` | تحقّق — خاصية احصل/عيّن | تعمل ✅ | 95 | 2639 |
| `011_type_of_object.ص` | تحقّق — نوع() لكائن يُرجع «كائن» (إصلاح P0-1 / STORY-UI-W15-01) | تعمل ✅ | 58 | 2791 |
| `012_ui_decl_instantiation.ص` | تحقّق — إنشاء مكوّن واجهة (الكلمة المفتاحية «واجهة» لتعريف مكوّن UI) | تعمل ✅ | 107 | 3079 |
| `013_ui_decl_methods_inherit.ص` | تحقّق — دوال مكوّن «واجهة» ووراثته (STORY-UI-W15-02 AC-4/AC-5) | تعمل ✅ | 60 | 2896 |
| `001_yield_generator.ص` | ISSUE-040 | تكشف ثغرة ❌ (فشل المفسر ❌) | 59 | 0 |
| `002_ffi_extern.ص` | ISSUE-041 | تكشف ثغرة ❌ (فشل المفسر ❌) | 54 | 0 |
| `003_property_test_exit.ص` | ISSUE-042 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) | 55 | 2845 |
| `001_range_no_import.ص` | ISSUE-002 | تعمل ✅ | 66 | 2961 |
| `002_range_with_import.ص` | تحقّق — مدى مع استيراد أساسيات | تعمل ✅ | 70 | 2929 |
| `001_float_array_print.ص` | ISSUE-080 | تعمل ✅ | 63 | 2906 |
| `002_float_array_to_string.ص` | ISSUE-080 | تعمل ✅ | 62 | 2827 |
| `003_string_array_to_string.ص` | ISSUE-080 | تعمل ✅ | 50 | 2815 |
| `001_struct_field_nested_compiler.ص` | ISSUE-070 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) | 56 | 2634 |
| `001_hetero_nested_compiler.ص` | ISSUE-070 | تكشف ثغرة ❌ (تباعد المخرجات ⚠️) | 58 | 2684 |

## المقارنة المزدوجة لكل قاعدة

| القاعدة | الطبقة | اختبارات | الحُكم |
|---|---|---|---|
| `gr.adv.asm_dialect` | advanced | 20 | مطلقة (مفسر≡مترجم) |
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
| `gr.adv.template_decl` | advanced | 5 | مطلقة (مفسر≡مترجم) |
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
| `gr.decl.export` | declarations | 26 | مطلقة (مفسر≡مترجم) |
| `gr.decl.extern` | declarations | 22 | مطلقة (مفسر≡مترجم) |
| `gr.decl.function` | declarations | 188 | مطلقة (مفسر≡مترجم) |
| `gr.decl.import` | declarations | 38 | مطلقة (مفسر≡مترجم) |
| `gr.decl.parameters` | declarations | 56 | مطلقة (مفسر≡مترجم) |
| `gr.decl.reexport` | declarations | 8 | مطلقة (مفسر≡مترجم) |
| `gr.decl.type_ref` | declarations | 7 | مطلقة (مفسر≡مترجم) |
| `gr.decl.variable` | declarations | 147 | مطلقة (مفسر≡مترجم) |
| `gr.expr.array_literal` | expressions | 55 | مطلقة (مفسر≡مترجم) |
| `gr.expr.assignment` | expressions | 36 | مطلقة (مفسر≡مترجم) |
| `gr.expr.bitwise_and` | expressions | 22 | مطلقة (مفسر≡مترجم) |
| `gr.expr.bitwise_or` | expressions | 20 | مطلقة (مفسر≡مترجم) |
| `gr.expr.bitwise_xor` | expressions | 21 | مطلقة (مفسر≡مترجم) |
| `gr.expr.comparison` | expressions | 56 | مطلقة (مفسر≡مترجم) |
| `gr.expr.decorator` | expressions | 12 | مطلقة (مفسر≡مترجم) |
| `gr.expr.directive` | expressions | 12 | مطلقة (مفسر≡مترجم) |
| `gr.expr.equality` | expressions | 36 | مطلقة (مفسر≡مترجم) |
| `gr.expr.expression` | expressions | 20 | مطلقة (مفسر≡مترجم) |
| `gr.expr.factor` | expressions | 52 | مطلقة (مفسر≡مترجم) |
| `gr.expr.fstring` | expressions | 20 | مطلقة (مفسر≡مترجم) |
| `gr.expr.lambda` | expressions | 34 | مطلقة (مفسر≡مترجم) |
| `gr.expr.logical_and` | expressions | 32 | مطلقة (مفسر≡مترجم) |
| `gr.expr.logical_or` | expressions | 34 | مطلقة (مفسر≡مترجم) |
| `gr.expr.map_literal` | expressions | 51 | مطلقة (مفسر≡مترجم) |
| `gr.expr.null_coalesce` | expressions | 20 | مطلقة (مفسر≡مترجم) |
| `gr.expr.pipeline` | expressions | 20 | مطلقة (مفسر≡مترجم) |
| `gr.expr.postfix` | expressions | 65 | مطلقة (مفسر≡مترجم) |
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
| `gr.oop.class` | oop | 127 | مطلقة (مفسر≡مترجم) |
| `gr.oop.constructor` | oop | 30 | مطلقة (مفسر≡مترجم) |
| `gr.oop.destructor` | oop | 11 | مطلقة (مفسر≡مترجم) |
| `gr.oop.enum` | oop | 31 | مطلقة (مفسر≡مترجم) |
| `gr.oop.extension` | oop | 12 | مطلقة (مفسر≡مترجم) |
| `gr.oop.field` | oop | 30 | مطلقة (مفسر≡مترجم) |
| `gr.oop.impl` | oop | 20 | مطلقة (مفسر≡مترجم) |
| `gr.oop.member` | oop | 34 | مطلقة (مفسر≡مترجم) |
| `gr.oop.method` | oop | 47 | مطلقة (مفسر≡مترجم) |
| `gr.oop.modifiers` | oop | 25 | مطلقة (مفسر≡مترجم) |
| `gr.oop.new` | oop | 13 | مطلقة (مفسر≡مترجم) |
| `gr.oop.operator` | oop | 16 | مطلقة (مفسر≡مترجم) |
| `gr.oop.property` | oop | 16 | مطلقة (مفسر≡مترجم) |
| `gr.oop.struct` | oop | 44 | مطلقة (مفسر≡مترجم) |
| `gr.oop.this_super` | oop | 10 | مطلقة (مفسر≡مترجم) |
| `gr.oop.trait` | oop | 24 | مطلقة (مفسر≡مترجم) |
| `gr.pattern.binding` | patterns | 4 | مطلقة (مفسر≡مترجم) |
| `gr.pattern.list` | patterns | 29 | مطلقة (مفسر≡مترجم) |
| `gr.pattern.or` | patterns | 7 | مطلقة (مفسر≡مترجم) |
| `gr.pattern.pattern` | patterns | 9 | مطلقة (مفسر≡مترجم) |
| `gr.pattern.primary` | patterns | 27 | مطلقة (مفسر≡مترجم) |
| `gr.pattern.struct` | patterns | 9 | مطلقة (مفسر≡مترجم) |
| `gr.program.block` | program | 38 | مطلقة (مفسر≡مترجم) |
| `gr.program.declaration` | program | 20 | مطلقة (مفسر≡مترجم) |
| `gr.program.program` | program | 20 | مطلقة (مفسر≡مترجم) |
| `gr.program.statement` | program | 19 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.break` | statements | 39 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.continue` | statements | 32 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.expression` | statements | 114 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.for` | statements | 242 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.if` | statements | 235 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.match` | statements | 212 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.return` | statements | 75 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.switch` | statements | 37 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.throw` | statements | 27 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.try` | statements | 54 | مطلقة (مفسر≡مترجم) |
| `gr.stmt.while` | statements | 201 | مطلقة (مفسر≡مترجم) |
