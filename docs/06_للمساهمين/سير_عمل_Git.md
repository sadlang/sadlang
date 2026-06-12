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

الفرع الرئيسي (`graphic`) **مقفل**: لا دفع مباشر، لا تاريخ غير خطّي، لا commits غير موقَّعة.
كل تغيير يدخل **عبر PR يُدمج بـ squash فقط**.

---

## 1. حماية الفرع الرئيسي (Main Branch Protection)

الفرع الافتراضي `graphic` محميٌّ عبر **Repository Ruleset** اسمه
`PMF v1.9.2 - Main Branch Protection` (وضع: `active`، يُطبَّق على `~DEFAULT_BRANCH`).

| القاعدة | المعنى العملي | ماذا يحدث لو خالفت |
|---|---|---|
| `pull_request` | كل تغيير يمرّ بـ PR (لا دفع مباشر) | `git push origin graphic` → **مرفوض** |
| `required_signatures` | كل commit يصل للفرع **موقَّع** | دفع commit غير موقَّع → **مرفوض** |
| `required_linear_history` | تاريخ خطّي — **لا merge commits** | دمج `merge` عبر API → **`405 Merge commits are not allowed`** |
| `non_fast_forward` | منع force-push / إعادة الكتابة | `push --force` → **مرفوض** |
| `deletion` | منع حذف الفرع | حذف `graphic` → **مرفوض** |
| `pull_request.approvals = 0` | **لا مراجعة إلزامية حاليّاً** (فريق مالك واحد) | الدمج يمضي دون موافقة ثانية |

**ملاحظتان مهمّتان:**

1. **لا توجد فحوص CI إلزامية في الـRuleset.** فحوص GitHub Actions (Windows/Linux/macOS/تحليل ساكن)
   تظهر على الـPR وقد يقول GitHub `BLOCKED` أثناء انتظارها، لكنها **لا تمنع الدمج عبر REST API**.
   هي إشارة جودة، لا بوّابة صارمة. (التحقّق المحلّي قبل الدمج يبقى مسؤوليتك — انظر §3.)
2. **لماذا ينجح الدمج عبر API رغم `required_signatures`؟** لأن GitHub **يوقّع** commit الـsquash
   من طرف الخادم (web-flow signature). أما الدفع المحلّي المباشر فيحمل commits غير موقَّعة → يُرفض.

> **القيود الأصلية الأكثر صرامة** (مراجعة إلزامية + Code Owner) **مُخفَّفة مؤقتاً** لأن الفريق مالك
> واحد. تُعاد عند انضمام مراجِع ثانٍ — تفاصيل في
> [`.github/BRANCH_PROTECTION_POLICY.md`](../../.github/BRANCH_PROTECTION_POLICY.md).

---

## 2. بدء ميزة جديدة

### 2.1 انطلق من فرع رئيسي محدَّث

```bash
git checkout graphic
git fetch origin
git reset --hard origin/graphic   # طابِق الريموت تماماً قبل التفريع
```

### 2.2 أنشئ فرع ميزة بتسمية واضحة

استخدم بادئة تصف النوع — `feat/`، `fix/`، `test/`، `refactor/`، `docs/`:

```bash
git checkout -b feat/اسم-الميزة-المختصر graphic
```

### 2.3 اعمل والتزم بوحدات صغيرة

- رسائل commit بصيغة [Conventional Commits](https://www.conventionalcommits.org/):
  `feat(scope): وصف` · `fix(scope): وصف` · `test(scope): وصف`.
- إن وُجد توقيع GPG محلّي فعّله (`commit.gpgsign true`)؛ وإلا فالخادم يوقّع عند الدمج.

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
gh pr create --base graphic --head feat/اسم-الميزة-المختصر `
  --title "feat(scope): العنوان" --body "الوصف"
```

### 4.2 ادمج — **squash حصراً**

التاريخ الخطّي يمنع merge commits، فالطريقة الوحيدة المسموحة هي **squash**
(و`rebase` نظريّاً). استخدم REST API:

```bash
gh api --method PUT repos/SalehKadah/s-programming-language/pulls/<رقم>/merge `
  -f merge_method=squash
```

> ✅ `{"merged": true}` = نجح. الخادم يوقّع commit الناتج.
> ❌ `405 Merge commits are not allowed` = استخدمت `merge_method=merge` — حوّلها إلى `squash`.

### 4.3 إن كان الـPR متعارضاً (CONFLICTING)

عالج التعارض **في فرع الميزة** (لا في `graphic`) ثم ادفعه ليصبح الـPR قابلاً للدمج:

```bash
git checkout feat/اسم-الميزة-المختصر
git merge origin/graphic        # أو: git rebase origin/graphic
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
git checkout graphic
git fetch origin
git reset --hard origin/graphic
git branch -d feat/اسم-الميزة-المختصر       # محلّياً
git push origin --delete feat/اسم-الميزة-المختصر  # الريموت (إن لم يُحذف تلقائياً)
```

---

## 5. أخطاء شائعة وحلولها

| العَرَض | السبب | الحل |
|---|---|---|
| `push declined due to repository rule violations` على `graphic` | دفع مباشر للفرع المحميّ | افتح PR بدلاً منه (§4.1) |
| `405 Merge commits are not allowed` | `merge_method=merge` | استخدم `merge_method=squash` |
| الدفع مرفوض بسبب توقيع | commit غير موقَّع على فرع يفرض التوقيع | الدمج عبر API (الخادم يوقّع)، أو فعّل GPG محلّياً |
| `gh pr view` يقول `BLOCKED` | فحوص CI ما زالت `pending` | ليست بوّابة صارمة؛ تحقّق محلّياً ثم ادمج عبر API |
| تعارض في ملف مشترك ساخن | فرعان لمسا نفس السطور | حُلّه في فرع الميزة (§4.3) قبل الدمج |

---

## مراجع

- [`.github/BRANCH_PROTECTION_POLICY.md`](../../.github/BRANCH_PROTECTION_POLICY.md) — السياسة الكاملة وخطة إعادة التشديد
- [`.github/PULL_REQUEST_TEMPLATE.md`](../../.github/PULL_REQUEST_TEMPLATE.md) — قالب وصف الـPR
- [دستور التوثيق](../CONTRIBUTING_DOCS.md) · [دليل الأسلوب](../STYLE_GUIDE.md)
