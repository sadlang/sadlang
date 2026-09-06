#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
═══════════════════════════════════════════════════════════════════════════════════
ملف: check_lowers_to.py
الوصف: حارس البوّابة الثالثة — يتحقّق أنّ ربط القواعد بأوپكودات SIR سليمٌ ومحدَّث،
       ويُبلّغ عن دعم الخلفيّة الأصليّة (بلا LLVM) لكلّ قاعدةٍ ولكلّ معماريّة.

(AR) ما يفحصه:
       ① الكتالوج محدَّث: يستدعي gen_sir_opcodes_yaml.py --check فيقارنه بتعداد المترجم.
       ② التغطية: لكلّ قاعدةٍ في grammar/*.yaml مدخلٌ في lowers_to.yaml.
       ③ المفردات: كلّ أوپكودٍ مذكورٍ في lowers_to موجودٌ في الكتالوج.
       ④ الانجراف: كلّ اسمٍ في backend/*/isel.yaml موجودٌ في تعداد المترجم
          (عدا دَينٍ مُعلَنٍ بقرار المالك: يُطبَع ولا يُفشِل).
       ⑤ المعماريّات: المخفوضةُ فعلًا مقابل قائمة backend/targets.yaml —
          هدفٌ موسومٌ lowered بلا خفضٍ يُفشِل، والمخطَّطُ دَينٌ مُعلَن.

(AR) المبدأ: القاعدة الموسومة not_derived ليست ناجحةً ولا فاشلة — هي **غير مقيسة**،
     وتُعدّ دَينًا يظهر في التقرير. ولا يُفشِل الحارسُ البناءَ بسببها إلّا مع
     ‎--require-derived، كي لا يتحوّل الدَّينُ المعلومُ إلى بوّابةٍ تعطّل العمل صامتةً.

الاستخدام:
    python scripts/codegen/check_lowers_to.py
    python scripts/codegen/check_lowers_to.py --require-derived   # يشدّد البوّابة
    python scripts/codegen/check_lowers_to.py --json build/_lowers_to.json
═══════════════════════════════════════════════════════════════════════════════════
"""

import argparse
import io
import json
import os
import re
import subprocess
import sys
from pathlib import Path

try:
    import yaml
except ImportError:
    print("❌ مطلوب PyYAML: pip install pyyaml", file=sys.stderr)
    sys.exit(2)

ROOT = Path(__file__).resolve().parent.parent.parent
GRAMMAR_DIR = ROOT / "language-truth" / "grammar"
BACKEND_DIR = ROOT / "language-truth" / "backend"
CATALOG = BACKEND_DIR / "sir_opcodes.yaml"
LOWERS = GRAMMAR_DIR / "lowers_to.yaml"
TARGETS = BACKEND_DIR / "targets.yaml"


def load_targets() -> list[dict]:
    """(AR) قائمةُ المعماريّات المستهدَفة من مصدر الحقيقة — لا تُكتب في سكربت.

    (AR) تُميّز ثلاثَ حالاتٍ لا تُخلط: الملفُّ مفقود، أو مشوَّهٌ نحويًّا، أو موجودٌ
         بقائمةٍ فارغة. الخلطُ بينها يُنتج رسالةَ «مفقود» مضلّلةً لملفٍّ موجود.
    """
    if not TARGETS.exists():
        raise FileNotFoundError(f"مفقود: {TARGETS.relative_to(ROOT)}")
    try:
        data = yaml.safe_load(_read(TARGETS)) or {}
    except yaml.YAMLError as ع:
        raise ValueError(f"{TARGETS.name} مشوَّهٌ نحويًّا: {ع}") from ع
    return [t for t in (data.get("targets") or []) if t.get("id")]


def _read(p: Path) -> str:
    return io.open(p, encoding="utf-8-sig").read()


# ═══ حدودٌ يُقاسُ بها **أنّ الحارسَ نظر** ═════════════════════════════════
# (AR) 🔑 **صفرُ قاعدةٍ وصفرُ أوپكودٍ كانا يمرّانِ خُضرًا.** لامتغيِّراتُ هذا
#      الحارسِ كلُّها **فروقُ مجموعات**: تغطيةٌ (قواعدُ ناقصةٌ من `lowers_to`)،
#      ومفرداتٌ (أوپكودٌ غيرُ معروف)، وانجرافٌ (اسمٌ في `isel` خارجَ التعداد).
#      والفرقُ بينَ خاليَينِ خالٍ — فمجلَّدُ قواعدَ أُعيدت تسميتُه، أو كتالوجٌ
#      بلا أوپكودٍ واحد، يُخرِجانِ «✅ الحارس سليم» ورمزَ صفر. وهذه أرضيّاتُ
#      عمًى **دونَ نصفِ المقيسِ** عمدًا فلا يشدُّها عملٌ مشروع — ولا يُنثَرُ
#      المقيسُ ههنا نسخةً ثانية: الحارسُ يطبعُه في كلِّ تشغيلةٍ إلى جوارِ حدِّه.
FLOOR_RULES = 40
FLOOR_OPCODES = 200
# (AR) وقواعدُ «غير مقيسة» دَينٌ مُعلَنٌ لا يُفشِلُ إلّا مع `--require-derived`،
#      فيُترَكُ سقفًا نازلًا يُقاسُ في كلِّ تشغيلة — وإلّا نما بلا احمرار.
#      ⚠️ **وهو مشدودٌ على المقيسِ، ومخرَجُه يُسمّى صراحةً.** فحالةُ `derived`
#         يمنحُها المولِّدُ من **وجودِ بذورٍ إيجابيّةٍ** في `rules_matrix`،
#         والقاعدةُ الجديدةُ تولدُ بلا بذورٍ فترفعُ العددَ حتّى تُكتَبَ
#         اختباراتُها — فيبدو الشدُّ قاسيًا. وجُرِّبَ تركُ هامشِ واحدٍ **فردَّه
#         قلبُ العيارِ نفسُه**: «حدٌّ فيه فجوةٌ — المجسّاتُ لا تعضُّ على هامش»،
#         وهو محقّ: سقفٌ فوقَ المقيسِ يبتلعُ انحدارًا كاملًا عندَ الحافّة.
#         وكاتبُ هذا الحارسِ صرَّحَ بأنّه أبقاه غيرَ مُفشِلٍ عمدًا «كي لا يتحوّلَ
#         الدَّينُ المعلومُ إلى بوّابةٍ تعطّلُ العملَ **صامتةً**» — والصمتُ هو
#         المنهيُّ عنه لا الإفشال: فالشكوى تُسمّي المخرَجَينِ معًا (بذرةٌ
#         إيجابيّةٌ للقاعدةِ، أو رفعُ السقفِ في الإيداعِ نفسِه) كسائرِ سقّاطاتِ
#         هذا المستودع.
CEILING_NOT_DERIVED = 2
# (AR) 🔑 **ودَينُ `isel` قائمةُ إذنٍ تُحاكَمُ في الاتّجاهَين.** كانت تُقرأُ في
#      اتّجاهٍ واحد: اسمٌ خارجَها يُفشِل. فبندٌ **بلِيَ** (صارَ الاسمُ موجودًا في
#      التعداد، أو زالَ ذِكرُه من `isel`) يبقى فيها صامتًا يُجيزُ عودتَه غدًا.
DECLARED_ISEL_DEBT = {"CONST_I64"}


def main() -> int:
    ap = argparse.ArgumentParser(description="حارس ربط القواعد بأوپكودات SIR")
    ap.add_argument("--require-derived", action="store_true",
                    help="اجعل القواعد غير المشتقّة تُفشِل البوّابة")
    ap.add_argument("--json", help="مسار كتابة تقرير JSON")
    args = ap.parse_args()
    os.environ.setdefault("PYTHONUTF8", "1")
    try:
        # (AR) stderr أيضًا: رسائلُ SystemExit من المولّدات تمرّ عبره، وتشويهُها
        #      على وحدة تحكّم ويندوز يُفقد أوضحَ تشخيصٍ في أسوأ لحظة.
        sys.stdout.reconfigure(encoding="utf-8")
        sys.stderr.reconfigure(encoding="utf-8")
    except Exception:
        pass

    errors: list[str] = []

    # (AR) 🔑 **ومرجعٌ مفقودٌ عطبُ آلةٍ (٢) لا حكمٌ على المحتوى (١).** كان يخرجُ
    #      بـ١ فيقرؤه `x.py` «وجدَ انجرافًا»، والعلاجانِ مختلفانِ تمامًا:
    #      إعادةُ توليدٍ مقابلَ إصلاحِ قاعدةٍ في مصدرِ الحقيقة.
    if not CATALOG.exists():
        print(f"❌ عطبُ آلة: مفقود: {CATALOG.relative_to(ROOT)} — "
              "شغّل gen_sir_opcodes_yaml.py. لم يُقَسْ شيء.")
        return 2
    if not LOWERS.exists():
        print(f"❌ عطبُ آلة: مفقود: {LOWERS.relative_to(ROOT)} — "
              "شغّل gen_grammar_lowers_to.py. لم يُقَسْ شيء.")
        return 2

    cat = yaml.safe_load(_read(CATALOG)) or {}
    opcodes = {e["name"]: e for e in cat.get("opcodes", []) or []}
    archs = sorted({a for e in opcodes.values() for a in (e.get("native_lowered") or [])})

    # ── ① طزاجةُ الكتالوج ────────────────────────────────────────────────────
    # (AR) لا يكفي أن يوجد الملفّ: كتالوجٌ بائتٌ يمرّ أخضرَ هو نفسُ الانجراف الذي
    #      بُنيت هذه الحزمةُ لسدّه. نستدعي المولّد بـ‎--check فيقارن المولَّدَ بالمصدر.
    مولّد = Path(__file__).with_name("gen_sir_opcodes_yaml.py")
    طزاجة = "غير مفحوصة"
    if not مولّد.exists():
        errors.append(f"  ✗ مفقود المولّد: {مولّد.name} — طزاجةُ الكتالوج غير قابلةٍ للفحص")
    else:
        نتيجة = subprocess.run([sys.executable, str(مولّد), "--check"],
                               capture_output=True, text=True, encoding="utf-8", cwd=str(ROOT))
        if نتيجة.returncode == 0:
            طزاجة = "محدَّث"
        else:
            # (AR) المولّد يبثّ إخفاقاته الحرِجة عبر SystemExit ⇒ **stderr**، وبَياناتِ
            #      الانجراف عبر stdout. قراءةُ stdout وحدَه تُنتج تشخيصًا كاذبًا:
            #      «الكتالوج بائت» لعطبٍ سببُه مسارٌ مفقودٌ في targets.yaml مثلًا.
            مخرَج = ((نتيجة.stdout or "") + (نتيجة.stderr or "")).strip().splitlines()
            طزاجة = "بائت" if نتيجة.stdout.strip() else "تعذّر فحصُها"
            errors.append(f"  ✗ فشل فحصُ طزاجة الكتالوج (rc={نتيجة.returncode}) — "
                          f"gen_sir_opcodes_yaml.py --check")
            for سطر in مخرَج[-6:]:
                errors.append(f"      {سطر}")

    # ── ② التغطية ────────────────────────────────────────────────────────────
    rule_ids: list[str] = []
    for yf in sorted(GRAMMAR_DIR.glob("*.yaml")):
        if yf.name.startswith("_") or yf.name == LOWERS.name:
            continue
        data = yaml.safe_load(_read(yf)) or {}
        for prod in data.get("productions", []) or []:
            if prod.get("id"):
                rule_ids.append(prod["id"])

    low = yaml.safe_load(_read(LOWERS)) or {}
    rows = {r["id"]: r for r in low.get("rules", []) or []}

    # (AR) 🔑 **والعمى يُقالُ قبلَ فروقِ المجموعات.** فرقُ خاليَينِ خالٍ:
    #      قواعدُ صفرٌ ومداخلُ صفرٌ ⇒ «✅ الحارس سليم» ورمزُ صفر.
    if len(rule_ids) < FLOOR_RULES:
        print(f"❌ عطبُ آلة: قواعدُ النحوِ المقروءةُ {len(rule_ids)} دونَ "
              f"الأرضيّةِ {FLOOR_RULES} — قارئُ القواعدِ أعمى. لم يُقَسْ شيء.")
        return 2
    if len(opcodes) < FLOOR_OPCODES:
        print(f"❌ عطبُ آلة: أوپكوداتُ الكتالوجِ {len(opcodes)} دونَ الأرضيّةِ "
              f"{FLOOR_OPCODES} — الكتالوجُ لم يُقرأ. لم يُقَسْ شيء.")
        return 2

    # (AR) 🔑 **ورمزُ ٢ يقعُ قبلَ أوّلِ قياسٍ لا بعدَه.** كان فحصُ `isel`
    #      و`targets` في موضعِهما من التقرير، فيعودُ الحارسُ بـ٢ **بعدَ** أن
    #      قاسَ التغطيةَ والمفرداتِ والانجرافَ فعلًا — فتُحذَفُ شكاواه الصحيحةُ
    #      صامتةً ويُطبَعُ «لم يقِسْ شيئًا» وهو قاسَ وأصاب. (قِيسَ: خرقُ محتوًى
    #      مع قائمةِ منصّاتٍ فارغةٍ يبتلعُ سطرَ «قاعدة بلا مدخل».)
    if not sorted(BACKEND_DIR.glob("*/isel.yaml")):
        print("❌ عطبُ آلة: لا ملفَّ isel.yaml في أيِّ خلفيّة — "
              "لم يُقَسْ انجرافٌ. لم يُقَسْ شيء.")
        return 2
    try:
        targets = load_targets()
    except (FileNotFoundError, ValueError) as ع:
        print(f"❌ عطبُ آلة: {ع} — قائمةُ المنصّات مصدرُ حقيقةٍ إلزاميّ. "
              "لم يُقَسْ شيء.")
        return 2
    if not targets:
        print(f"❌ عطبُ آلة: {TARGETS.name} بلا هدفٍ واحد — لا معنى للبوّابة "
              "الثالثة. لم يُقَسْ شيء.")
        return 2

    missing = [r for r in rule_ids if r not in rows]
    extra = [r for r in rows if r not in rule_ids]
    for r in missing:
        errors.append(f"  ✗ قاعدة بلا مدخل في lowers_to.yaml: {r}")
    for r in extra:
        errors.append(f"  ✗ مدخلٌ لقاعدةٍ غير موجودة: {r}")

    # ── ③ المفردات ───────────────────────────────────────────────────────────
    for rid, row in sorted(rows.items()):
        for op in (row.get("lowers_to") or []):
            if op not in opcodes:
                errors.append(f"  ✗ {rid}: أوپكودٌ غير معروفٍ في الكتالوج: {op}")

    # ── ④ الانجراف بين isel.yaml وتعداد المترجم ─────────────────────────────
    # (AR) دَينٌ مُقِرٌّ بنفسِه: أسماءٌ يذكرها isel.yaml ولا وجودَ لها في التعداد،
    #      قرَّر المالك (٢٠٢٦-٠٨-٠٩) **إبقاءَها معلَّمةً** لا إصلاحَها الآن. تُطبَع
    #      في كلّ تشغيلٍ ولا تُفشِل الحارس، كي يصلح وصلُه بـCI بلا تعمية.
    #      ⚠️ أيُّ اسمٍ منجرفٍ **خارج** هذه القائمة يُفشِل فورًا.
    drift: list[str] = []
    debt: list[str] = []
    debt_seen: set[str] = set()
    for isel in sorted(BACKEND_DIR.glob("*/isel.yaml")):
        for name in re.findall(r"^\s*-\s*sir:\s*(\S+)", _read(isel), re.M):
            if name in opcodes:
                continue
            سطر = (f"{isel.parent.name}/isel.yaml يذكر أوپكودًا غير موجودٍ "
                   f"في تعداد المترجم: {name}")
            if name in DECLARED_ISEL_DEBT:
                debt_seen.add(name)
                debt.append(سطر)
            else:
                drift.append(سطر)
    # (AR) 🔑 **والقائمةُ تُحاكَمُ في الاتّجاهِ الآخرِ كذلك.** بندٌ لا ينطبقُ اليومَ
    #      (صارَ الاسمُ في التعداد، أو زالَ ذِكرُه من `isel`) دَينٌ **بالٍ**: يبقى
    #      صامتًا فيُجيزُ عودةَ الانجرافِ غدًا بلا احمرار. والقائمةُ تنكمشُ.
    for name in sorted(DECLARED_ISEL_DEBT - debt_seen):
        errors.append(f"  ✗ بندُ دَينٍ بالٍ في DECLARED_ISEL_DEBT: {name} — "
                      "لم يعُدْ منجرفًا؛ احذفْه (القائمةُ تنكمشُ فقط).")
    # (AR) ونصُّ الانجرافِ يُميَّزُ عن نصِّ الدَّينِ المُعلَن: الجملةُ واحدةٌ في
    #      الاثنَين، فمرساةٌ عليها لا تُفرِّقُ حمرةً من تقريرِ دَينٍ في تشغيلةٍ
    #      خضراء — وهو ما يجعلُ مجسَّ عيارٍ يمرُّ على غيرِ ما يدَّعي.
    errors.extend(f"  ✗ {x} ⇐ انجرافٌ غيرُ مُعلَن" for x in sorted(set(drift)))
    debt = sorted(set(debt))

    # ── التقرير ──────────────────────────────────────────────────────────────
    derived = [r for r in rows.values() if r.get("status") == "derived"]
    not_derived = [r for r in rows.values() if r.get("status") != "derived"]

    print("═" * 70)
    print(f"  حارس lowers_to — {len(rule_ids)} (الأرضيّة {FLOOR_RULES})"
          f" قاعدة، {len(opcodes)} (الأرضيّة {FLOOR_OPCODES}) أوپكودًا")
    print("═" * 70)
    print(f"\n① الكتالوج: {len(opcodes)} أوپكودًا (الطزاجة: {طزاجة}) · "
          f"مخفوضٌ أصليًّا: {sum(1 for e in opcodes.values() if e.get('native_lowered'))}")
    print(f"② التغطية: {len(rows)}/{len(rule_ids)} قاعدةً لها مدخل · "
          f"مُشتقّة {len(derived)} · غير مقيسة {len(not_derived)} "
          f"(السقف {CEILING_NOT_DERIVED})")
    # (AR) والدَّينُ المُعلَنُ يُحاكَمُ في كلِّ تشغيلةٍ لا مع `--require-derived`
    #      وحدَها: بلا سقفٍ ينمو «غيرُ المقيسِ» سطرًا سطرًا والحارسُ أخضر.
    if len(not_derived) > CEILING_NOT_DERIVED:
        errors.append(
            f"  ✗ قواعدُ غيرُ مقيسة: {len(not_derived)} فوقَ السقفِ "
            f"{CEILING_NOT_DERIVED} — السقفُ ينحدرُ ولا يُرفَع. والمخرَجانِ: "
            "أضِفْ بذرةً إيجابيّةً للقاعدةِ في tests/behavior/rules_matrix "
            "فتصيرَ `derived`، أو ارفعِ السقفَ في الإيداعِ نفسِه بقرارٍ يُرى "
            "في الفرق.")
    print("③ المفردات: كلُّ أوپكودٍ مذكورٍ في lowers_to معروفٌ في الكتالوج")
    print(f"④ الانجراف: {len(debt)} دَينًا مُعلَنًا (يُطبَع ولا يُفشِل)")
    for سطر in debt:
        print(f"   ⚠️ {سطر}")

    print("\n▸ دعم الخلفيّة الأصليّة (بلا LLVM) — من المُشتقّ فقط:")
    for a in archs:
        ok = [r for r in derived if a in (r.get("native_ok") or [])]
        print(f"   {a:<10} {len(ok)}/{len(derived)} قاعدة مُشتقّة "
              f"({len(ok)}/{len(rule_ids)} من الكلّ)")

    # ── ⑤ المعماريّات المستهدَفة مقابل المُنجَز ───────────────────────────────
    # (AR) بوّابةُ «المنصّات الخمس» تُقاس بقائمة targets.yaml لا بما وُجد صدفةً
    #      في المستودع؛ وإلّا صارت البوّابةُ خضراءَ بمعياريْن اثنين وسُمّيت خمسة.
    # (AR) والقائمةُ قُرِئَت قبلَ أوّلِ قياسٍ أعلاه — مرجعٌ إلزاميٌّ غيابُه
    #      عطبُ آلةٍ لا حكمٌ على المحتوى.
    # (AR) والشرطُ محقَّقٌ دائمًا بعدَ الرقعةِ أعلاه — يُبقى صراحةً لأنّ حذفَه
    #      يُغري بإعادةِ الفرعِ الصامتِ حينَ يُنقَلُ الكودُ.
    if targets:
        مخفوضة = ("معماريّةٌ واحدةٌ مخفوضة" if len(archs) == 1
                  else "معماريّتان مخفوضتان" if len(archs) == 2
                  else f"{len(archs)} معماريّاتٍ مخفوضة")
        print(f"\n⑤ المعماريّات المستهدَفة: {len(targets)} في مصدر الحقيقة · {مخفوضة} فعلًا")
        for t in targets:
            live = t["id"] in archs
            مخطَّطة = t.get("status") == "planned"
            حال = "✅ مخفوضة" if live else ("⏳ مخطَّطة" if مخطَّطة else "❌ موسومةٌ lowered بلا خفض")
            print(f"   {t['id']:<10} {t.get('milestone', ''):<8} {حال}  — {t.get('name_ar', '')}")
            # (AR) الاتّجاه الذي يهمّ: ادّعاءٌ في مصدر الحقيقة لا يسنده المترجم.
            #      المخطَّطُ دَينٌ مشروع؛ أمّا الموسومُ lowered بلا خفضٍ فكذبٌ يُفشِل.
            if not live and not مخطَّطة:
                errors.append(f"  ✗ targets.yaml يسم {t['id']} بـ«{t.get('status')}» "
                              f"ولا خفضَ أصليًّا له في المترجم")
        غائبة = [t["id"] for t in targets if t["id"] not in archs]
        if غائبة:
            عدد = f"{len(غائبة)} معماريّاتٍ" if 3 <= len(غائبة) <= 10 else f"{len(غائبة)} معماريّةً"
            print(f"   ▸ دَينٌ معلَن: {عدد} بلا خفضٍ أصليّ ({'، '.join(غائبة)}) — "
                  f"لا قاعدةَ تجتاز ب٣ قبل سدّها")
        زائدة = [a for a in archs if a not in {t["id"] for t in targets}]
        if زائدة:
            errors.append(f"  ✗ معماريّةٌ مخفوضةٌ خارج قائمة المستهدَفات: {'، '.join(زائدة)}")

    # (AR) أكثر الأوپكودات منعًا للدعم الأصليّ — ترتيب أولويّات العمل
    blockers: dict[str, int] = {}
    for r in derived:
        for a in archs:
            for op in (r.get(f"native_missing_{a}") or []):
                blockers[op] = blockers.get(op, 0) + 1
    if blockers:
        print("\n   أكثر الأوپكودات منعًا (أوپكود ← عدد القواعد المحجوبة):")
        for op, n in sorted(blockers.items(), key=lambda kv: -kv[1])[:12]:
            print(f"     {op:<28} {n}")

    if not_derived:
        print(f"\n   قواعد غير مقيسة ({len(not_derived)}):")
        for r in sorted(not_derived, key=lambda x: x["id"])[:15]:
            print(f"     {r['id']:<32} {r.get('reason', '')}")
        if len(not_derived) > 15:
            print(f"     … و{len(not_derived) - 15} أخرى")

    if args.json:
        out = {
            "rules_total": len(rule_ids),
            "derived": len(derived),
            "not_derived": len(not_derived),
            "archs": archs,
            "native_ok": {a: sum(1 for r in derived if a in (r.get("native_ok") or []))
                          for a in archs},
            "blockers": blockers,
            "errors": errors,
        }
        p = Path(args.json)
        p.parent.mkdir(parents=True, exist_ok=True)
        io.open(p, "w", encoding="utf-8", newline="\n").write(
            json.dumps(out, ensure_ascii=False, indent=2))
        print(f"\n▸ كُتب التقرير: {p}")

    print()
    if errors:
        print("الأخطاء:")
        print("\n".join(errors))
    gate_ok = not errors and (not args.require_derived or not not_derived)
    print("═" * 70)
    print("  ✅ الحارس سليم" if gate_ok else "  ❌ فشل الحارس")
    return 0 if gate_ok else 1


def _رمز_الخروج(دالّة) -> int:
    """(AR) 🔑 **شبكةٌ عليا: الانهيارُ غيرُ المتوقَّعِ عطبُ آلةٍ لا حكمُ محتوى.**
    الأثرُ الرجعيُّ يخرجُ برمزِ ١ فيقرؤه `x.py` «وجدَ انجرافًا» — والعلاجانِ
    مختلفانِ تمامًا. وقِيسَ حيًّا: مخطَّطُ YAML يتبدّلُ (صفٌّ مكانَ خريطة)
    فينهارُ القارئُ بـ`AttributeError` ويخرجُ الحارسُ بـ١."""
    try:
        return دالّة()
    except SystemExit:
        raise
    except BaseException as علّة:          # noqa: BLE001 — الشبكةُ تعمُّ عمدًا
        import traceback
        traceback.print_exc()
        print("[حارس] ✗ عطبُ آلة: انهيارٌ غيرُ متوقَّعٍ (%s) — لم يُقَسْ شيء"
              % علّة.__class__.__name__, file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(_رمز_الخروج(main))
