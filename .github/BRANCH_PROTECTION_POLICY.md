# سياسة Branch Protection لفرع main

> **المرجع:** PMF v1.9.2 §5.2.2 — استبدال Agent-Signature بـ GPG + Branch Protection
> **تاريخ التفعيل المتوقَّع:** عبر `STORY-PMF-V17-ENFORCE-GPG`
> **المُلكية:** صالح (admin)

---

## ⚠️ تعديل مؤقت ساري (2026-06-10) — تخفيف شرط المراجعة

> **القرار:** بطلب المالك (صالح) — **تخفيف شرط المراجعة** على Ruleset `16775713`
> (الفرع الافتراضي `graphic`) ليكفي **المالك وحده** للدمج. **السبب: الفريق صغير جداً** (مالك واحد،
> لا مراجِع ثانٍ — تعذّر دمج PR لأن GitHub يمنع موافقة صاحب PR على نفسه).
>
> **مؤقت:** يُعاد تفعيل الوضع الآمن الكامل (مراجعة إلزامية + Code Owner) **عند كبر الفريق**.

| البند | القيمة الآمنة (الأصل) | القيمة المؤقتة الحالية |
|---|---|---|
| `required_approving_review_count` | `1` | **`0`** ⬇️ |
| `require_code_owner_review` | `true` | **`false`** ⬇️ |
| `require_last_push_approval` | `true` | **`false`** ⬇️ |
| `required_review_thread_resolution` | `true` | **`false`** ⬇️ |
| `dismiss_stale_reviews_on_push` | `true` | **`false`** ⬇️ |

**ما بقي مفعّلاً (حماية البيانات لم تُمَسّ):** ✅ `required_signatures` (GPG)
· ✅ `non_fast_forward` (منع force-push) · ✅ `deletion` (منع حذف الفرع)
· ✅ `required_linear_history` · ✅ `pull_request` (PR يبقى إلزامياً).

**شرط إعادة التفعيل الآمن (TODO):** عند انضمام مراجِع ثانٍ — أعِد
`required_approving_review_count: 1` + `require_code_owner_review: true` + الباقي إلى `true`.

---

## البنود الإلزامية

| البند | الحقل في GitHub API | القيمة |
|---|---|---|
| اشتراط commits موقَّعة بـ GPG | `required_signatures.enabled` | `true` |
| منع force push | `allow_force_pushes` | `false` |
| منع حذف الفرع | `allow_deletions` | `false` |
| اشتراط PR قبل merge | `required_pull_request_reviews` | enabled |
| عدد المراجعات المطلوبة | `required_approving_review_count` | `1` |
| مراجعة Code Owners إلزامية | `require_code_owner_reviews` | `true` |
| إبطال المراجعات عند push جديد | `dismiss_stale_reviews` | `true` |
| موافقة على آخر push | `require_last_push_approval` | `true` |
| حل كل المحادثات قبل merge | `required_conversation_resolution` | `true` |
| تاريخ خطي (لا merge commits معقّدة) | `required_linear_history` | `true` |
| تطبيق القواعد على admins أيضاً | `enforce_admins` | `true` |

## التفعيل

استخدم الملف `branch-protection-main.json` في نفس المجلد:

```bash
gh api repos/:owner/:repo/branches/main/protection \
  --method PUT \
  --input .github/branch-protection-main.json
```

## التحقق

```bash
gh api repos/:owner/:repo/branches/main/protection \
  --jq '{
    signatures: .required_signatures.enabled,
    force_push: .allow_force_pushes.enabled,
    deletions:  .allow_deletions.enabled,
    pr_reviews: .required_pull_request_reviews.required_approving_review_count,
    code_owners: .required_pull_request_reviews.require_code_owner_reviews
  }'
```

النتيجة المتوقَّعة:
```json
{
  "signatures": true,
  "force_push": false,
  "deletions": false,
  "pr_reviews": 1,
  "code_owners": true
}
```

## الفترة الانتقالية

قبل التفعيل، تطبَّق السياسة الاحتياطية الموصوفة في
`_bmad-output/governance/1-policy/STORY-PMF-V17-ENFORCE-GPG.md` (قسم "الفترة الانتقالية").

---

## حماية فرع `dev` (2026-06-17)

فرع `dev` (فرع تكامل عمل الوكلاء) محميّ **بنفس قواعد `graphic`** عبر Ruleset
مستقلّ `17779574` ("PMF - Dev Branch Protection"، `enforcement: active`، يستهدف
`refs/heads/dev`، `bypass_actors: []`). القواعد المتطابقة:
`deletion` · `non_fast_forward` · `required_linear_history` · `required_signatures` (GPG)
· `pull_request` (PR إلزاميّ، 0 مراجعات حاليًّا — فريق صغير، كتخفيف graphic).

- **التكوين المحفوظ:** [`ruleset-dev-protection.json`](./ruleset-dev-protection.json).
- **سير عمل الوكلاء:** فروع `agent/*` في worktrees بمجلد `C:/s_lang/temp-brunch/`،
  تُدمَج في `dev` **عبر PR فقط** (لا دفع مباشر). راجع `C:/s_lang/temp-brunch/README.md`.
- **إعادة التطبيق/التحقّق:**
  ```bash
  gh api repos/sadlang/s-programming-language/rulesets --method POST \
    --input .github/ruleset-dev-protection.json        # إنشاء
  gh api repos/sadlang/s-programming-language/rulesets/17779574 \
    --jq '{name, enforcement, refs:.conditions.ref_name.include, rules:[.rules[].type]}'  # تحقّق
  ```
  > ملاحظة: المستودع نُقل إلى منظمة **`sadlang`** (`sadlang/s-programming-language`).
