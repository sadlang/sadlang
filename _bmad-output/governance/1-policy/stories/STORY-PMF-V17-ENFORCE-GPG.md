# STORY-PMF-V17-ENFORCE-GPG — تفعيل GPG Signing + Branch Protection على main

> **النوع:** ستوري بشري (يتطلَّب تنفيذ صالح يدوياً — لا يمكن للوكلاء تنفيذها)
> **المرجع:** PMF v1.7 §5.2.2 + `.github/BRANCH_PROTECTION_POLICY.md`
> **الأولوية:** 🔴 حرجة (تسد الثغرة #11 من نقد Pen-Tester)
> **الحالة:** `COMPLETE` ✅ (مع **تعديل مؤقت 2026-06-10** — انظر التنبيه)
>
> ⚠️ **تعديل مؤقت (2026-06-10):** بطلب المالك، خُفِّف **شرط المراجعة فقط** على Ruleset 16775713
> (`required_approving_review_count: 1→0` + إلغاء `require_code_owner_review`/`require_last_push_approval`/
> `required_review_thread_resolution`/`dismiss_stale_reviews_on_push`) **ليكفي المالك وحده للدمج
> (فريق صغير جداً)**. **حماية البيانات لم تُمَسّ:** التوقيع GPG + منع force-push + منع الحذف +
> التاريخ الخطّي + PR الإلزامي تبقى فعّالة. **يُعاد الوضع الآمن عند كبر الفريق.**
> التفصيل: `.github/BRANCH_PROTECTION_POLICY.md`.
> **تاريخ التنفيذ:** 2026-05-23T08:00:03+03:00
> **منفِّذ:** صالح (SalehKadah)
> **GPG KEY_ID:** `04507AC6CE9FDDDF` (RSA-4096)
> **Fingerprint:** `002607BEAB8CC2CB8168236E04507AC6CE9FDDDF`
> **Ruleset ID:** 16775713 (active, non-bypassable)
> **المُلكية:** صالح حصراً (admin GitHub repo)

---

## السياق

مراجعة Pen-Tester (#9) كشفت أن استبدال Agent-Signature بـ GPG + Branch Protection
في v1.7 **لم يُفعَّل فعلياً** — السياسة موثَّقة لكن GitHub لم يطبِّقها بعد.
نتيجة: النظام حالياً **أقل أماناً من v1.6** (لا Agent-Signature، لا GPG enforced).

## معايير القبول (DoD)

- [x] تفعيل Branch Protection على فرع `main` (عبر Repository Ruleset 16775713):
  - [x] Require signed commits = ✅ (`required_signatures`)
  - [x] Require PR before merge = ✅ (`pull_request` rule)
  - [x] Require 1 approval (Saleh) = ✅ (`required_approving_review_count: 1`)
  - [x] Require Code Owner review = ✅ (`require_code_owner_review: true`)
  - [x] Require conversation resolution = ✅ (`required_review_thread_resolution: true`)
  - [x] Disable force push = ✅ (`non_fast_forward`)
  - [x] Disable deletions = ✅ (`deletion`)
  - [x] Require linear history = ✅ (`required_linear_history`)
  - [x] Require last push approval = ✅ (`require_last_push_approval: true`)
  - [x] Dismiss stale reviews on push = ✅ (`dismiss_stale_reviews_on_push: true`)
  - [x] Admin bypass disabled = ✅ (`bypass_actors: []`, `current_user_can_bypass: never`)
- [x] تسجيل مفتاح GPG العلني لصالح في GitHub:
  ```bash
  gpg --armor --export <KEY_ID> | gh gpg-key add -
  ```
- [ ] التحقق بأمر:
  ```bash
  gh api repos/<owner>/<repo>/branches/main/protection \
    --jq '.required_signatures.enabled'
  # يجب أن يُرجِع: true
  ```
- [ ] تشغيل `monthly-pmf-check.yml` يدوياً (workflow_dispatch) والتأكد أن
      step "فحص Branch Protection + GPG" يُرجِع `gpg_enforced=True`.
- [ ] تحديث هذا الملف: تغيير الحالة إلى `COMPLETE` + إضافة timestamp.

## خطوات التنفيذ (لصالح)

### 1. توليد مفتاح GPG (إذا لم يكن موجوداً)

```bash
gpg --full-generate-key
# اختر: RSA 4096, no expiry, real name + email المطابق لـ GitHub
gpg --list-secret-keys --keyid-format=long
# انسخ KEY_ID (المقطع بعد rsa4096/)
```

### 2. ربط المفتاح بـ Git + GitHub

```bash
git config --global user.signingkey <KEY_ID>
git config --global commit.gpgsign true
git config --global tag.gpgsign true

gpg --armor --export <KEY_ID> | gh gpg-key add -
```

### 3. تفعيل Branch Protection

**عبر GitHub UI:**
- Settings → Branches → Add rule → Branch name pattern: `main`
- فعِّل كل البنود المذكورة في `.github/BRANCH_PROTECTION_POLICY.md`.

**أو عبر CLI:**
```bash
gh api repos/:owner/:repo/branches/main/protection \
  --method PUT \
  --input .github/branch-protection-main.json
```

### 4. التحقق

```bash
# جرّب push كوميت غير موقَّع — يجب أن يُرفض
git commit --no-gpg-sign -m "test"
git push origin main  # → ❌ rejected

# جرّب push كوميت موقَّع — يجب أن يمر
git commit -m "test"  # سيوقَّع تلقائياً
git push origin main  # → ✅
```

## الفترة الانتقالية (قبل التفعيل)

طوال فترة `PENDING_SALEH_ACTION`، تطبَّق سياسة احتياطية:
1. **لا merge مباشر على main** — كل تغيير عبر PR.
2. **PR labels إلزامية:** `awaiting-saleh-review` على كل PR يلمس `_bmad-output/governance/1-policy/*`.
3. **CODEOWNERS موجود ويعمل** — صالح يجب أن يوافق على ملفات الحوكمة.
4. الوكلاء يُسجِّلون كل تعديل في AUDIT_LOG.jsonl (آلية v1.5).

## التواريخ

- **مُنشأة:** 2026-05-22T08:15:00Z
- **مكتشفة بواسطة:** Pen-Tester (CRITIQUE_PENTESTER_2026-05-22.md, ثغرة #11)
- **مُنفِّذ:** صالح (SalehKadah)
- **تاريخ التنفيذ الفعلي:** 2026-05-23T08:00:03+03:00
- **آخر تحديث:** 2026-05-23T08:00:03+03:00

## نتائج التحقق الفعلي (2026-05-23)

```
$ gh api repos/sadlang/sadlang/rulesets --jq '.[] | {id, name, enforcement, target}'
{
  "enforcement": "active",
  "id": 16775713,
  "name": "PMF v1.9.2 - Main Branch Protection",
  "target": "branch"
}

$ gh gpg-key list
EMAIL              KEY ID            ADDED
kadah253@gmail...  04507AC6CE9FDDDF  2026-05-23

$ git config --global --get-regexp 'user.signingkey|commit.gpgsign|tag.gpgsign'
user.signingkey 04507AC6CE9FDDDF
commit.gpgsign true
tag.gpgsign true
```

**ملاحظات تنفيذية:**
1. تم تحويل المستودع من Private إلى Public لتجاوز قيد GitHub Free على Branch Protection للمستودعات الخاصة.
2. استُخدم Repository Rulesets API (الأحدث) بدلاً من Classic Branch Protection.
3. المفتاح بدون passphrase (مناسب للأتمتة) — يمكن إضافة passphrase لاحقاً بـ `gpg --edit-key 04507AC6CE9FDDDF` ثم `passwd`.
4. الفروع الأخرى (مثل `graphic` الحالي بـ 42 commit ahead) ستحتاج إلى Merge عبر PR موقَّع إلى `main`.

## التحويل إلى COMPLETE

عند تنفيذ كل بنود DoD، استبدل ترويسة هذا الملف بـ:

```
> **الحالة:** COMPLETE
> **تاريخ التنفيذ:** YYYY-MM-DDTHH:MM:SSZ
> **منفِّذ:** صالح
> **تحقق:** [نتيجة gh api ...]
```

وأضف سطراً في PMF §12 (changelog) أو في AUDIT_LOG.jsonl.

---

**ملاحظة من Pen-Tester:** بدون تنفيذ هذه الستوري، كل ضمانات v1.7 الأمنية = نظرية.
