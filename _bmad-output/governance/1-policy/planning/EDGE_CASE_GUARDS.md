# حواجز الحالات الحدية — Edge Case Guards

> **التاريخ:** 2025-11-21
> **النسخة:** v1.0
> **مُرتبط بـ:** [PROJECT_MANAGEMENT_FRAMEWORK.md](PROJECT_MANAGEMENT_FRAMEWORK.md) (PMF-001)
> **الغرض:** مرجع مرجعي (Reference) للحواجز التقنية والسياسية. لا يُقرأ في بدء الجلسة — يُستشار عند تنفيذ بروتوكول معيَّن.
> **الحالة:** سارية المفعول

---

## 0. كيف تستخدم هذا الملف

- **مرجع Random Access** — اقفز للقسم المعني (A-E) عند تنفيذ بروتوكول من PMF.
- كل حارس يحوي: السيناريو الحدي، الحارس القابل للتنفيذ، الموضع (CI/hook/cron/code).
- المسؤولية: الحواجز التقنية → الوكيل المُكلَّف؛ الحواجز السياسية → صالح في PR.

---

## A. حواجز AGENT_LOCK.json (التضارب بين الوكلاء)

| # | السيناريو الحدي | الحارس | الموضع |
|---|---|---|---|
| A.1 | ملف AGENT_LOCK.json تالف / JSON غير صالح | `try: json.load(f) except JSONDecodeError: backup_corrupt(f); locks={'active_claims':[]}` | على كل قراءة |
| A.2 | ساعة النظام تتقدَّم/تتراجع → expires_at خاطئ | استخدام UTC من خادم مرجعي + رفض حجوزات `expires_at > now + 5min` | في step كتابة الحجز |
| A.3 | وكيلان يكتبان الملف لحظياً (race) | قفل ملف نظامي: `msvcrt.locking` (Windows) / `fcntl.flock` (Linux) قبل read-modify-write | wrapper إلزامي |
| A.4 | الجلسة تنتهي بـ crash قبل تحرير القفل | TTL تلقائي 4 ساعات + cron ليلي ينظِّف الحجوزات المنتهية الصلاحية | cron job |
| A.5 | صالح يتجاوز AGENT_LOCK يدوياً للجميع | تسجيل إلزامي في `AUDIT_LOG.jsonl` مع `reason` لكل override | في أداة override |

---

## B. حواجز ميزانية الرموز (Token Budget)

| # | السيناريو الحدي | الحارس | الموضع |
|---|---|---|---|
| B.1 | `.github/copilot-instructions.md` يتجاوز 8K بعد إضافات | اختبار CI: `assert tiktoken_count(file) <= 8000` | workflow |
| B.2 | AGENT_CONTEXT.md يصل 1999 رمز ثم إضافة سطر | pre-commit hook: إذا `tokens > 1900` → أرشفة أقدم قسم إلى ملف موسَّع تلقائياً | git hook |
| B.3 | حساب `len/4` غير دقيق للعربية (UTF-8) | استخدام `tiktoken`: `enc = tiktoken.encoding_for_model('gpt-4'); len(enc.encode(text))` | في كل قياس |
| B.4 | وكيل بنافذة سياق < 32K (مثل GPT-3.5) | قائمة قراءة بديلة مُكثَّفة: STATUS.md فقط + قراءة أقسام PMF عند الطلب | في PMF قسم 4.1 |

---

## C. حواجز النسخ الاحتياطي والتراجع

| # | السيناريو الحدي | الحارس | الموضع |
|---|---|---|---|
| C.1 | وكيل sprint-status ينسى tag شهري | CI مجدوَل شهرياً: `if !git tag -l "governance-snapshot-$(date +%Y-%m)"; then exit 1` | scheduled workflow |
| C.2 | `git restore` يستعيد ملف لكن changelog لم يُحدَّث | post-restore hook: يُلحق سطر rollback تلقائياً في STATUS.md changelog | git hook |

---

## D. حواجز الحوكمة الذاتية

| # | السيناريو الحدي | الحارس | الموضع |
|---|---|---|---|
| D.1 | طبقة جديدة تُضاف للمشروع بعد المسح | trigger: عند إنشاء dir جديد على مستوى الجذر → invalidate docs/index.md | CI |
| D.2 | تاريخ المراجعة الشهرية (21) يصادف عطلة | نافذة 21-25 من الشهر؛ إن لم تكتمل بحلول 25 → escalate لصالح | تذكير cron |
| D.3 | PR يُعدِّل ملف حوكمة بدون مراجعة | `.github/CODEOWNERS`: `_bmad-output/governance/1-policy/* @saleh` يفرض موافقة صريحة | CODEOWNERS file |
| D.4 | وكيل ينشئ ملف يتيم في `_bmad-output/governance/1-policy/` | CI: لكل ملف في المجلد → تحقق أنه مذكور في PMF القسم 3 | workflow check |
| D.5 | تغييران في نفس اليوم → ترتيب غامض | استخدام `ISO datetime` كامل: `2025-11-21T14:30:00Z` بدل التاريخ فقط | في كل changelog |

---

## E. حواجز PRD/الواقع والتصعيد

| # | السيناريو الحدي | الحارس | الموضع |
|---|---|---|---|
| E.1 | الواقع نفسه عربة معطوبة (bug قديم) | استثناء: لو الواقع موسوم `KNOWN_BUG` في issues → PRD يفوز + ADR إلزامي | في PMF قسم 7 |
| E.2 | PENDING_REVIEW يبقى للأبد بلا عودة صالح | auto-escalate: إن عمر `PENDING_REVIEW > 14 يوماً` → إخطار مراجع بديل أو تجميد التبعيات | scheduled workflow |

---

## F. ملخص التغطية

| الفئة | عدد الحواجز |
|---|---|
| AGENT_LOCK race/corruption | 5 |
| Token budget | 4 |
| Backup/rollback | 2 |
| Governance self-protection | 5 |
| PRD/Reality + Escalation | 2 |
| **الإجمالي** | **18** |

---

## سجل التغييرات

| التاريخ (ISO) | الإصدار | التغيير | الكاتب |
|---|---|---|---|
| 2025-11-21T00:00:00Z | v1.0 | استخراج من PMF-001 القسم 14 (إعادة هيكلة بنيوية) | Amelia |
