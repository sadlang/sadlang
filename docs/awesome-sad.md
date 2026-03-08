# Awesome Sad

قائمة موارد ومشاريع وأدوات لغة ص بصيغة عملية تساعد المطور على الانتقال من التعلم إلى البناء الفعلي.

## 1) اللغة والنواة

1. `shared/lexer/` - التحليل المعجمي.
2. `shared/parser/` - التحليل النحوي.
3. `shared/ast/` - شجرة البنية المجردة.
4. `shared/types/` - نظام القيم والأنواع.
5. `interpreter_new/` - مفسر لغة ص.
6. `compiler_new/` - المترجم `sadc`.
7. `vm/` - مسار الآلة الافتراضية.

## 2) المكتبة القياسية (stdlib)

8. `stdlib/core/` - الدوال الأساسية المدمجة.
9. `stdlib/io/` - الإدخال والإخراج.
10. `stdlib/math/` - العمليات الرياضية.
11. `stdlib/string/` - معالجة النصوص العامة.
12. `stdlib/نص/` - معالجة النص العربي.
13. `stdlib/json/` - JSON parsing/stringify.
14. `stdlib/filesystem/` - الملفات والمسارات.
15. `stdlib/network/` - الشبكات العامة.
16. `stdlib/http/` - HTTP abstractions.
17. `stdlib/async/` - قنوات وتزامن.
18. `stdlib/system/` - دوال النظام.
19. `stdlib/image/` - معالجة الصور.
20. `stdlib/xml/` - XML parser/DOM.
21. `stdlib/database/` - قواعد البيانات.
22. `stdlib/crypto/` - التشفير والتجزئة.
23. `stdlib/embedded/` - مسارات الأنظمة المضمنة.
24. `stdlib/audio3d/` - الصوت ثلاثي الأبعاد.

## 3) الأدوات الرسمية

25. `tools/lsp/` - خادم اللغة.
26. `tools/formatter/` - منسق الكود.
27. `tools/pkg/` - مدير الحزم.
28. `tools/repl/` - البيئة التفاعلية.
29. `tools/vscode-extension/` - إضافة VS Code.
30. `tools/docgen/` - مولد التوثيق.

## 4) التوثيق والتعلم

31. `docs/SAD_LANGUAGE_COMPLETE_REFERENCE.md` - المرجع الشامل.
32. `docs/book/README.md` - كتاب لغة ص.
33. `docs/book/BUILD.md` - مسار HTML/PDF للكتاب.
34. `docs/curriculum/README.md` - المنهج الجامعي.
35. `docs/curriculum/weekly-plan.md` - خطة 16 أسبوع.
36. `docs/api/README.md` - فهرس API docs.
37. `docs/ANNUAL_PLAN_2026.md` - خطة التنفيذ السنوية.
38. `README.md` - مدخل المشروع.

## 5) أمثلة ومشاريع

39. `examples/test_simple.ص` - مثال أولي.
40. `examples/apps/تطبيق_الحاسبة.ص` - تطبيق حاسبة.
41. `examples/apps/تطبيق_المهام.ص` - تطبيق مهام.
42. `examples/apps/تطبيق_الدردشة.ص` - نموذج دردشة.
43. `examples/apps/تطبيق_الإنتاجية.ص` - نموذج إنتاجية.
44. `examples/apps/test_ui.ص` - مثال واجهات.
45. `examples/apps/test_flutter_style.ص` - مثال نمط Flutter.

## 6) الويب والموقع

46. `website/docs/index.md` - الصفحة الرئيسية للموقع.
47. `website/docs/playground.md` - Playground.
48. `website/docs/book/index.md` - واجهة عرض الكتاب.
49. `website/docs/book/book.md` - النسخة المجمعة للكتاب.

## 7) النشر والبناء

50. `cmake/` - وحدات البناء.
51. `distribution/` - تجهيزات التوزيع.
52. `scripts/build_book.ps1` - بناء الكتاب HTML/PDF.

## طريقة الاستخدام المقترحة

1. ابدأ بـ `README.md` ثم المرجع الشامل.
2. انتقل إلى `docs/book/` للتعلم المنهجي.
3. جرّب أمثلة `examples/apps/` خطوة بخطوة.
4. استخدم `docs/api/` أثناء التطوير المرجعي.
5. فعّل الأدوات (`lsp`, `formatter`, `pkg`) ضمن سير العمل اليومي.
