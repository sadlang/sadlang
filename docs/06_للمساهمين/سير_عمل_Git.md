---
نوع: دليل
المستوى: متوسط
الإصدار: "1.0"
آخر_تحديث: "2026-06-12"
الكلمات_المفتاحية: [git, فرع, دمج, merge, branch, حماية, ruleset, سير-عمل]
الحالة: مكتمل
---

# دليل سير عمل Git — الفروع والدمج وحماية الفرع الرئيسي

> 🟢 مكتمل
>
> هذا الدليل عملي: **كيف تبدأ ميزة جديدة**، **كيف تدمج فرعاً في الفرع الرئيسي**،
> و**ما هي حماية الفرع الرئيسي** ولماذا تُرفض بعض الدفعات. القيم هنا مُتحقَّق منها
> فعليّاً من إعدادات المستودع (Ruleset `16775713`)، لا افتراضات.
>
> **مرجع السياسة الكامل:** [`.github/BRANCH_PROTECTION_POLICY.md`](../../.github/BRANCH_PROTECTION_POLICY.md)

---

## 0. الخلاصة في سطر واحد

الفرع الرئيسي (`sadlang`) **مقفل**: لا دفع مباشر، لا تاريخ غير خطّي، لا commits غير موقَّعة.
كل تغيير يدخل **عبر PR يُدمج بـ squash فقط**.

---

## 1. حماية الفرع الرئيسي (Main Branch Protection)

الفرع الافتراضي `sadlang` محميٌّ عبر **Repository Ruleset** اسمه
`PMF v1.9.2 - Main Branch Protection` (وضع: `active`، يُطبَّق على `~DEFAULT_BRANCH`).

| القاعدة | المعنى العملي | ماذا يحدث لو خالفت |
|---|---|---|
| `pull_request` | كل تغيير يمرّ بـ PR (لا دفع مباشر) | `git push origin sadlang` → **مرفوض** |
| `required_signatures` | كل commit يصل للفرع **موقَّع** | دفع commit غير موقَّع → **مرفوض** |
| `required_linear_history` | تاريخ خطّي — **لا merge commits** (commit بأبوين) | دمج `merge` عبر API → **`405 Merge commits are not allowed`** |
| `non_fast_forward` | منع force-push / إعادة الكتابة | `push --force` → **مرفوض** |
| `deletion` | منع حذف الفرع | حذف `sadlang` → **مرفوض** |
| `pull_request.approvals = 0` | **لا مراجعة إلزامية حاليّاً** (فريق مالك واحد) | الدمج يمضي دون موافقة ثانية |

**ملاحظة دقيقة عن منع `merge`:** قاعدة `pull_request` في الـRuleset تُدرج
`allowed_merge_methods: [merge, squash, rebase]` — أي أن `merge` يبدو **مسموحاً** ظاهريّاً.
لكن المنع الفعلي يأتي من قاعدة **منفصلة** هي `required_linear_history`: فأيّ commit بأبوين
(merge commit) يُرفض، ولذلك يردّ GitHub `405 Merge commits are not allowed`.
أما `squash` و`rebase` فينتجان commit بأب واحد (تاريخ خطّي) → **كلاهما مسموح عمليّاً**.

**ملاحظتان مهمّتان:**

1. **لا توجد فحوص CI إلزامية في الـRuleset.** فحوص GitHub Actions (Windows/Linux/macOS/تحليل ساكن)
   إشارة جودة لا بوّابة صارمة. مع ذلك، حالة الـPR على الفرع الافتراضي تظهر `BLOCKED` بسبب
   **سياسة الفرع** نفسها (لا بالضرورة بسبب CI)، فيرفضها `gh pr merge` العاديّ ويتطلّب الدمج
   **تجاوز المالك** (REST API أو `--admin` — انظر §4.2). (التحقّق المحلّي قبل الدمج يبقى مسؤوليتك — انظر §3.)
2. **لماذا ينجح الدمج عبر API رغم `required_signatures`؟** لأن GitHub **يوقّع** commit الـsquash
   من طرف الخادم (web-flow signature). أما الدفع المحلّي المباشر فيحمل commits غير موقَّعة → يُرفض.

> **القيود الأصلية الأكثر صرامة** (مراجعة إلزامية + Code Owner) **مُخفَّفة مؤقتاً** لأن الفريق مالك
> واحد. تُعاد عند انضمام مراجِع ثانٍ — تفاصيل في
> [`.github/BRANCH_PROTECTION_POLICY.md`](../../.github/BRANCH_PROTECTION_POLICY.md).

---

## 2. بدء ميزة جديدة

### 2.1 انطلق من فرع رئيسي محدَّث

```bash
git checkout sadlang
git fetch origin
git reset --hard origin/sadlang   # طابِق الريموت تماماً قبل التفريع
```

### 2.2 أنشئ فرع ميزة بتسمية واضحة

استخدم بادئة تصف النوع — `feat/`، `fix/`، `test/`، `refactor/`، `docs/`:

```bash
git checkout -b feat/اسم-الميزة-المختصر sadlang
```

### 2.3 اعمل والتزم بوحدات صغيرة

- رسائل commit بصيغة [Conventional Commits](https://www.conventionalcommits.org/):
  `feat(scope): وصف` · `fix(scope): وصف` · `test(scope): وصف`.
- إن وُجد توقيع GPG محلّي فعّله (`commit.gpgsign true`)؛ وإلا فالخادم يوقّع عند الدمج.

> **توقيع GPG محلّياً (اختياري):** بما أن الدمج عبر الخادم يوقّع تلقائيّاً، التوقيع المحلّي
> ليس ضروريّاً للمساهمة. لكن إن أردت دفع commits موقَّعة مباشرةً إلى فرع ميزتك بتوقيع موثَّق:
> ولِّد مفتاحاً (`gpg --full-generate-key`)، أضِف مفتاحه العامّ إلى
> GitHub → Settings → SSH and GPG keys، ثم `git config user.signingkey <المعرّف>` و
> `git config commit.gpgsign true`.

### 2.4 قلِّل التداخل مع الميزات الموازية

إن كان هناك أكثر من ميزة قيد العمل في آنٍ واحد:

- **قسِّم بحدود ملفّية**: كل ميزة تملك ملفاتها قدر الإمكان.
- **الملفات المشتركة الساخنة** (`shared/errors/include/error_codes.h`،
  `language-truth/errors/*.yaml`، `CMakeLists.txt`، `.github/workflows/ci.yml`):
  نسِّق التعديل أو اجعل فرعاً واحداً "مالكاً" لها، لأنها أكثر مصادر تعارض الدمج.
- العزل لا يلغي التعارض: فرعان يلمسان نفس السطور → تعارض عند الدمج يُحلّ يدويّاً (§4.3).

---

## 3. قبل فتح PR — التحقّق المحلّي (إلزامي عمليّاً)

بما أن CI ليست بوّابة صارمة، **تحقّق محليّاً**:

```bash
# 1) يبني المفسّر بلا أخطاء
cmake --build build --target sad-run

# 2) إن لمست رسائل الأخطاء: أعِد توليد الكتالوج من YAML
python scripts/codegen/gen_error_messages.py `
  --yaml-dir language-truth/errors `
  --schema language-truth/_schemas/error.schema.json `
  --enum-header shared/errors/include/error_codes.h `
  --header shared/errors/generated/error_messages_generated.h `
  --source shared/errors/generated/error_messages_generated.cpp

# 3) الاختبارات الخضراء
python -m pytest tests/system/builtin_errors/ scripts/codegen/test_gen_error_messages_v5.py -q
```

---

## 4. دمج فرع في الفرع الرئيسي

### 4.1 ادفع الفرع وافتح PR

```bash
git push -u origin feat/اسم-الميزة-المختصر
gh pr create --base sadlang --head feat/اسم-الميزة-المختصر `
  --title "feat(scope): العنوان" --body "الوصف"
```

### 4.2 ادمج — **squash حصراً**

التاريخ الخطّي يرفض merge commits، فالطريقتان المسموحتان عمليّاً هما **`squash`**
(الموصى بها — تطوي الفرع في commit واحد نظيف) و**`rebase`**. تجنّب `merge`.

> ⚠️ **مهمّ:** سياسة الفرع الافتراضي تجعل حالة الـPR تظهر `BLOCKED`، فيرفض `gh pr merge <رقم> --squash`
> العاديّ الدمجَ برسالة *"base branch policy prohibits the merge"*. الدمج يتطلّب **تجاوز المالك (admin)**.

الطريقة المُتحقَّق منها (تجاوز المالك عبر REST API):

```bash
gh api --method PUT repos/SalehKadah/s-programming-language/pulls/<رقم>/merge `
  -f merge_method=squash
```

أو مكافئها عبر `gh` مع علم التجاوز:

```bash
gh pr merge <رقم> --squash --admin
```

> ✅ `{"merged": true}` = نجح. الخادم يوقّع commit الناتج.
> ❌ `405 Merge commits are not allowed` = استخدمت `merge_method=merge` — حوّلها إلى `squash`.
> ❌ `base branch policy prohibits the merge` = استخدمت `gh pr merge` بلا `--admin` — أضِف `--admin` أو استخدم REST API.

### 4.3 إن كان الـPR متعارضاً (CONFLICTING)

عالج التعارض **في فرع الميزة** (لا في `sadlang`) ثم ادفعه ليصبح الـPR قابلاً للدمج:

```bash
git checkout feat/اسم-الميزة-المختصر
git merge origin/sadlang        # أو: git rebase origin/sadlang
# حُلّ التعارضات في الملفات المعلَّمة، ثم:
git add <الملفات>
git commit                      # (للـmerge) أو: git rebase --continue
git push origin feat/اسم-الميزة-المختصر
```

ثم أعِد §4.2. ستظهر الحالة `MERGEABLE`.

> **ملاحظة:** لأن الفرع الرئيسي يفرض تاريخاً خطيّاً، فإن دمج الـPR نفسه يكون squash —
> لكن حلّ التعارض داخل فرع الميزة يمكن أن يكون merge أو rebase بحرّية (فرع الميزة غير محميّ).

### 4.4 بعد الدمج — نظّف وزامِن

```bash
git checkout sadlang
git fetch origin
git reset --hard origin/sadlang
git branch -d feat/اسم-الميزة-المختصر       # محلّياً
git push origin --delete feat/اسم-الميزة-المختصر  # الريموت (إن لم يُحذف تلقائياً)
```

---

## 5. أخطاء شائعة وحلولها

| العَرَض | السبب | الحل |
|---|---|---|
| `push declined due to repository rule violations` على `sadlang` | دفع مباشر للفرع المحميّ | افتح PR بدلاً منه (§4.1) |
| `405 Merge commits are not allowed` | `merge_method=merge` | استخدم `merge_method=squash` |
| الدفع مرفوض بسبب توقيع | commit غير موقَّع على فرع يفرض التوقيع | الدمج عبر API (الخادم يوقّع)، أو فعّل GPG محلّياً |
| `gh pr view` يقول `BLOCKED` | سياسة الفرع الافتراضي (وليس بالضرورة فحوص CI) | تجاوز المالك: REST API أو `gh pr merge --admin` |
| `base branch policy prohibits the merge` | `gh pr merge` بلا `--admin` | أضِف `--admin` أو استخدم REST API (§4.2) |
| تعارض في ملف مشترك ساخن | فرعان لمسا نفس السطور | حُلّه في فرع الميزة (§4.3) قبل الدمج |

---

## مراجع

- [`.github/BRANCH_PROTECTION_POLICY.md`](../../.github/BRANCH_PROTECTION_POLICY.md) — السياسة الكاملة وخطة إعادة التشديد
- [`.github/PULL_REQUEST_TEMPLATE.md`](../../.github/PULL_REQUEST_TEMPLATE.md) — قالب وصف الـPR
- [دليل أسلوب الكتابة](../دليل_أسلوب_الكتابة.md) — أسلوب الوثائق العربية
- [دليل المساهمين](README.md) · [فهرس فئة المساهمين](index.md)
