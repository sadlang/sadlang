# المهام النشطة (Active Tasks)

> **مالك المجلد:** PM (John) فقط — الوكلاء يقرأون ولا يكتبون هنا مباشرة.

## القواعد

1. كل مهمة تُمثَّل بملفين متجاورين:
   - `TASK-<id>.md` — تعريف المهمة (يكتبه PM فقط)
   - `TASK-<id>.state.md` — حالة التنفيذ (يحدّثه الوكيل المسؤول كل ≤ 12 ساعة عمل)

2. الترقيم: `TASK-001`, `TASK-002`, ... تسلسلي عبر كل المشروع (لا يُعاد تدويره أبداً).

3. عند الانتهاء (DoD ✅):
   - PM ينقل الملفين إلى [../done/](../done/)
   - يضيف سطراً في [../daily/](../daily/) لليوم الحالي

4. **سياسة Lost Agent (48h):** إذا `last_heartbeat` في `state.md` أقدم من 48 ساعة، PM يفتح الملف ويعيد التكليف.

## القوالب

- [TASK_TEMPLATE.md](../templates/TASK_TEMPLATE.md)
- [STATE_TEMPLATE.md](../templates/STATE_TEMPLATE.md)

## الحدود (WIP Limits)

| الوكيل | Active (هنا) | Queued (في BACKLOG) |
|---|---|---|
| Alpha | 1 | 2 |
| Beta | 1 | 2 |
| Gamma | 2 | 3 |
| **المجموع** | **≤ 4** | — |
