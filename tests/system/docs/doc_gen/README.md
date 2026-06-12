# اختبارات استخراج التوثيق المزدوج (sad ↔ sadc)

## الهدف
التحقق من أن **المفسر `sad`** و **المترجم `sadc`** ينتجان توثيقاً متطابقاً
لكل ميزات لغة ص ولكل صيغ الإخراج المدعومة (`markdown`, `json`, `html`, `pdf`).

## التشغيل
```powershell
python tests/doc_gen_dual_execution/runner.py            # كل الاختبارات
python tests/doc_gen_dual_execution/runner.py --skip-pdf # بدون اختبارات PDF
python tests/doc_gen_dual_execution/runner.py --verbose  # تفاصيل لكل اختبار
python tests/doc_gen_dual_execution/runner.py --filter inheritance
```

## بنية المجلدات
- `inputs/single/` — ملفات `.ص` مفردة لاختبار ميزات منفردة
- `inputs/projects/` — مجلدات مشاريع متعددة الملفات
- `_tmp/` — مخرجات مؤقتة (تُحذف تلقائياً)

## معايير المطابقة
| الصيغة | المعيار |
|--------|---------|
| markdown | تطابق بايتي 100% |
| json | تطابق بايتي 100% |
| html | تطابق بايتي 100% |
| pdf | الفرق < 5KB (timestamps في metadata فقط) |

## التغطية
- تصاريح: `دالة`, `صنف`, `سمة`, `تعداد`, `بنية`, `فضاء`, `صدّر`, `متغير`
- وراثة بسيطة، متعددة، عميقة، متعددة الأصناف الأب
- توثيق مع: كود، جداول، قوائم، روابط، عناوين متعددة المستويات
- مشاريع متعددة الملفات + استثناءات `--docs-exclude`
- مخططات Mermaid في HTML/PDF عند وجود وراثة
- ملفات بلا توثيق (يجب أن تكون فارغة في كلا الجانبين)
