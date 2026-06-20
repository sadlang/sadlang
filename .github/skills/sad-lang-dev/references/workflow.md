# سير العمل الصارم + معيار الإنجاز (Definition of Done)

> هذا الملف يزيل الغموض التنفيذي: **ماذا أعدّل بالضبط، بأي ترتيب، وكيف أُثبت أنه تمّ.**
> كل تغيير يتبع التسلسل، ولا يُعتبر منجَزاً إلا باستيفاء معيار الإنجاز (DoD) كاملاً.

## 0. قاعدة الملفات المُولَّدة (احفظها أولاً — مصدر غموض شائع)

الملفات تحت `shared/*/generated/` **متتبَّعة في git** (وليست build-only):
`keywords_generated.{h,cpp}`، `builtin_registry_generated.h`، إلخ.

- **لا تُحرِّرها يدوياً** — عدّل YAML المصدر ثم أعد التوليد.
- **لكن التزم بتضمينها في الـ commit** بعد إعادة التوليد — هي جزء من «قائمة الملفات» (File List).
- لا تترك YAML معدّلاً ومُولَّداً قديماً (أو العكس) — يجب أن يتطابقا في نفس الـ commit.

## 1. التسلسل الإلزامي لأي تغيير

1. **افهم الطبقة** — حدّد أين يقع التغيير من [./architecture.md](./architecture.md) §2.
2. **اقرأ 3 ملفات مشابهة** في نفس المجلد قبل الكتابة (BF-25).
3. **هل مدفوع بالبيانات؟** إن نعم: ابدأ من `language-truth/` (أو `data/language/` للأخطاء — راجع [./error-system.md](./error-system.md)).
4. **اكتب اختباراً يفشل أولاً** يعيد إنتاج الحاجة/الخطأ (BF-01, BF-12).
5. **نفّذ في الطبقة الصحيحة** عابراً الطبقات بالترتيب (Lexer→Parser→AST→Visitors/SIR→LLVM).
6. **أعد التوليد** إن مسستَ YAML (CMake أو `gen_*.py` — تحقّق `--help`).
7. **ابنِ المفسر والمترجم** (§البناء في [./build-test.md](./build-test.md)).
8. **شغّل الاختبارات** حتى تمرّ 100% (التنفيذ المزدوج + عدم التراجع).
9. **حدّث قائمة الملفات** بكل ما تغيّر (بما فيه المُولَّد).

## 2. قائمة الملفات المتوقّعة حسب نوع التغيير (File List)

| التغيير | الملفات التي تتغيّر عادةً |
|---------|--------------------------|
| **كلمة مفتاحية** | `language-truth/keywords.yaml` + `shared/lexer/generated/keywords_generated.{h,cpp}` (مُولَّد) + `shared/parser/src/<dir>/` + `shared/ast/include/` + `interpreter/src/visitors/` + `compiler/src/frontend/` (+`sir_types.h` opcode) + اختبار `.ص` |
| **دالة مضمنة** | `language-truth/builtins/<domain>.yaml` (+`_index.yaml` إن ملف جديد) + `shared/builtins/generated/builtin_registry_generated.h` (مُولَّد) + `interpreter/src/builtins/builtin_*.cpp` + `compiler/src/backend/llvm/builders/builtins/*.cpp` + اختبار `.ص` |
| **رمز خطأ** | `shared/errors/include/error_codes.h` + `language-truth/errors/<cat>.yaml` (المصدر) + `shared/errors/generated/*` (مُولَّد عند البناء) + اختبار |
| **توجيه `@`** | `language-truth/directives.yaml` + مُولَّد + parser + AST + visitors + codegen + اختبار |
| **opcode SIR** | `compiler/include/frontend/sir_types.h` + `SIRBuilder` + `compiler/src/backend/llvm/` + اختبار |

## 3. معيار الإنجاز (Definition of Done) — قائمة تحقق

علّم التغيير منجَزاً **فقط** عند استيفاء كل بند:

- [ ] التغيير في **الطبقة الصحيحة** فقط — لا ترقيع في مكان الاستعمال (BF-10, BF-09)
- [ ] بدأ من **YAML** إن كان مدفوعاً بالبيانات؛ **لم أُحرّر** أي `generated/` يدوياً
- [ ] أُعيد التوليد، و**YAML + المُولَّد متطابقان** في نفس الـ commit
- [ ] الدعم مضاف في **المفسر والمترجم** (أو `@skip_compiler` موثّق بسبب صريح)
- [ ] **اختبار `.ص`** جديد يُغطّي الحالة، بصيغة `@expected` الصحيحة
- [ ] `runner.py --level P0` (وقسم الميزة) **يمرّ 100%**
- [ ] `runner.py --level P1` يمرّ (قبل أي PR) — **لا تراجع** (BF-29)
- [ ] `sadc` يبني بلا أخطاء، و`sad-run` يعمل بلا تراجع
- [ ] تعليقات **مزدوجة اللغة** على كل API عام (CW-08)
- [ ] **التوافق الخلفي** محفوظ (لا تغيير معنى opcode/token/خطأ موجود — CW-24)
- [ ] **قائمة الملفات** محدَّثة بكل الملفات المتغيّرة (بما فيها المُولَّدة)

## 4. ممنوعات صريحة (لا تُعلّم منجَزاً إن وقع أيٌّ منها)

- ❌ تحرير ملف تحت `generated/` يدوياً.
- ❌ تعطيل/تبسيط اختبار لتفادي فشله بدل إصلاح السبب (BF-09).
- ❌ ادّعاء أن الاختبارات مكتوبة/ناجحة دون وجودها ونجاحها فعلاً (100%).
- ❌ دعم المفسر فقط دون المترجم بلا `@skip_compiler` موثّق.
- ❌ تمرير `--level P0` فقط واعتباره كافياً لـ PR (المطلوب P1 على الأقل).

## 5. سير عمل الفروع (git على فرع `dev`)

> **العمل يتكامل في فرع `dev`، لا في `sadlang`.** الفرعان **محميّان على GitHub**
> (Rulesets: `dev`=`17779574`، `sadlang`=`16775713`) بقواعد متطابقة:
> منع الحذف · منع force-push · تاريخ خطّيّ · **توقيع GPG إلزاميّ** · **PR إلزاميّ**.
> المستودع نُقل إلى منظمة **`sadlang/s-programming-language`** (استخدم `sadlang/...` في `gh api`).
> راجع `.github/BRANCH_PROTECTION_POLICY.md` و`C:/s_lang/temp-brunch/README.md`.

**القاعدة الذهبية:** لا تُودِع/تدفع **مباشرةً** على `dev` أو `sadlang` — كل تغيير يدخل `dev` **عبر PR**
من فرع `agent/*` معزول في worktree.

1. **أنشئ worktree للمهمة** (فرع من `dev`):
   ```bash
   cd /c/s_lang/s-programming-language
   git fetch origin
   git worktree add /c/s_lang/temp-brunch/<مهمة> -b agent/<مهمة> origin/dev
   ```
2. **اعمل وأودِع داخل المجلد الفرعيّ** (commits **موقّعة GPG** — `commit.gpgsign=true`،
   المفتاح مُهيّأ؛ وإلا يرفض `dev` الدمج لاشتراط `required_signatures`):
   ```bash
   cd /c/s_lang/temp-brunch/<مهمة>
   git add -A && git commit -m "وصف"   # موقّع تلقائيًّا
   ```
3. **ادفع الفرع وافتح PR إلى `dev`** (بعد اجتياز DoD §3 — خصوصًا `runner.py --level P1`):
   ```bash
   git push -u origin agent/<مهمة>
   gh pr create --base dev --head agent/<مهمة> --title "<عنوان>" --body "<وصف + قائمة الملفات>"
   gh pr merge --merge
   ```
4. **نظّف بعد الدمج:**
   ```bash
   cd /c/s_lang/s-programming-language
   git worktree remove /c/s_lang/temp-brunch/<مهمة> && git branch -D agent/<مهمة>
   ```

**ممنوعات الفروع:** ❌ `git push origin dev` مباشرةً (محظور بالحماية) · ❌ العمل في المجلد الأساسي
على `dev`/`sadlang` مباشرةً · ❌ commit غير موقّع (يُرفَض عند الدمج) · ❌ خلط مهمّتين في worktree واحد.

> **إضافة لـDoD §3 عند العمل المحكوم بفرع:** [ ] العمل على فرع `agent/*` من `dev` (لا commit مباشر على `dev`/`sadlang`) · [ ] كل الـcommits موقّعة GPG · [ ] الدمج عبر PR إلى `dev`.
