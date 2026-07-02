<div dir="rtl">

# نحو المحرّرات (TextMate) — مولَّد من مصدر الحقيقة

ملفّ [`sad.tmLanguage.json`](sad.tmLanguage.json) هو **ناتج مولَّد** لقواعد إبراز لغة ص،
تستهلكه المحرّرات (محراب، إضافات VS Code). **لا تُحرِّره يدويًّا.**

## التوليد

```bash
python3 scripts/codegen/gen_tmgrammar.py
```

يقرأ المولِّد مصدر الحقيقة في [`language-truth/`](../../language-truth/):
`keywords.yaml` · `types.yaml` · `operators.yaml` · `directives.yaml` — فالإبراز
**يتبع اللغة آليًّا** ولا ينجرف يدويًّا (مبدأ: المحرّرات تستهلك مصدر الحقيقة لا تكرّره).

## حارس الانجراف

```bash
python3 scripts/codegen/gen_tmgrammar.py --check
```

يفشل إن انجرف الملفّ المُلتزَم عن مصدر الحقيقة (يفرضه CI:
[`tmgrammar-check.yml`](../../.github/workflows/tmgrammar-check.yml)). عند تغيير
أيّ من ملفّات `language-truth` المعجميّة: أعد التوليد والتزِم الناتج.

## النطاقات (scopes)

| النطاق | المصدر |
|--------|--------|
| `keyword.control.sad` | كلمات التحكّم/الأخطاء/المطابقة (reserved) |
| `storage.type.sad` | فاتحات التعريف (دالة/صنف/بنية/تعداد/…) |
| `storage.modifier.sad` | مُعدِّلات الوصول/التخزين (عام/خاص/ساكن/…) |
| `keyword.operator.word.sad` | العوامل الكلميّة (و/أو/ليس) |
| `support.type.sad` | الأنواع المدمجة والسطحيّة (types.yaml) |
| `keyword.control.directive.sad` | التوجيهات `@` (directives.yaml) |
| `keyword.operator.sad` | العوامل الرمزيّة (operators.yaml، تشمل «؟ : / ؟؟ / ؟.») |
| `constant.language.sad` | القيم الحرفيّة (صحيح/خطأ/لاشيء) |
| `string.quoted.double.sad` · `string.quoted.raw.sad` · `string.quoted.interpolated.sad` | نصوص عاديّة/خام «r"…"»/منسَّقة «f"…{…}…"» |
| `comment.line.*` · `comment.block.*` | تعليق سطر «#»/توثيق «##»، كتلة «#* *#»/توثيق كتلة «#** **#» |
| `constant.numeric.*` | أرقام عشريّة (مع أُسّ)/سداسيّة/ثنائيّة/ثمانيّة، عربيّة ولاتينيّة |
| `storage.modifier.lifetime.sad` | أعمار «'مُعرِّف» |

> حدود الكلمات عربيّة-آمنة (نظرة سالبة على صنف المُعرِّف بدل `\b`).
> الأنماط مشتقّة من `grammar/70_lexical.yaml` (نصوص/أرقام/تعليقات/أعمار).

</div>
