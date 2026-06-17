# 📂 tasks/

> نظام المهام الموحَّد للوكلاء. راجع [AGENT_ORCHESTRATION.md](../AGENT_ORCHESTRATION.md) للبروتوكول الكامل.

## البنية

| مجلد | الغرض | من يَكتب فيه؟ |
|---|---|---|
| `inbox/` | أوامر مُكلَّفة لوكلاء | PM فقط |
| `in-progress/` | مهام نَشطة (الوكيل بدأ) | الوكيل (يَنقلها من inbox) |
| `outbox/` | نتائج مَكتوبة من الوكلاء | الوكلاء |
| `done/` | مَهام مُراجَعة ومُغلَقة | PM فقط (بعد المراجعة) |
| `locks/` | أقفال نَشطة لكل مَهمة | الوكلاء |
| `index.yaml` | فهرس مَوحَّد (SoT) | PM فقط (مؤقتاً) |

## التَسمية

```
inbox/T-{NNNN}-{agent_short}-{slug}.md
outbox/T-{NNNN}-{agent_short}-{slug}-RESULT.md
locks/T-{NNNN}.lock
```

أمثلة:
- `inbox/T-0001-alpha-keywords-yaml.md`
- `outbox/T-0001-alpha-keywords-yaml-RESULT.md`
- `locks/T-0001.lock`

## التَسلسل الزمني

كل أرقام T مُتسلسلة عبر المشروع كله (لا أرقام مُتكررة).
آخر رقم مَستخدم: انظر `index.yaml` → `stats.total`.
