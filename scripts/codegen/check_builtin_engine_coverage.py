#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ «الوعدُ المُعلَنُ يُقاسُ ولا يُدَّعى».

     مدمجٌ في مصدرِ الحقيقةِ يحملُ أحدَ حقلَين لا ثالثَ لهما ولا كليهما:

       • `status: stable`  ⇒ **دعوى أنّه يعمل.** فيجبُ أن يوجدَ له مرجعُ إرسالٍ
         مؤهَّلٌ بنطاقِه في شفرةِ الشجرة. وإلّا فهو **وعدٌ كاذب**.
       • `intent: مؤجَّل` ⇒ **دَينٌ مُقِرٌّ بنفسِه.** الإعلانُ يبقى ولا يَعِدُ.

     🔑 **ما يقيسُه هذا الحارسُ بالضبط، ولا يدّعي أكثر:** أنّ الحقلَ في مصدرِ
        الحقيقةِ **لم ينجرفْ عن الشفرة**. فهو كاشفُ انجرافٍ لا تدقيقٌ مستقلّ:
        الوسمُ اشتُقَّ من هذا المسبارِ نفسِه، فبندُ ① صفرٌ لحظةَ الاشتقاقِ بالبناء.
        وقيمتُه فيما بعد: يحمرُّ إن حُذِفت ذراعُ إرسالٍ من الشفرةِ ولم يُنزَعِ
        الوعد، أو إن وُسِمَ بندٌ `stable` بلا وصل. (بُرهنَ بالحقن: حذفُ ذراعِ
        WebSocket يُحمِّرُ بندَ ①.)

     ثلاثةُ لامتغيِّراتٍ وسقفٌ رابع:

       ① وعدٌ كاذب : `status: stable` بلا مرجعِ إرسال ⇒ **صفر، لا سقف**.
       ② دَينُ تأجيل: عددُ `intent: مؤجَّل` ⇒ سقفٌ **نازلٌ** لا يُرفَعُ إلّا بقرار.
       ③ حقلٌ فاسد : بندٌ بلا حقلٍ، **أو بالحقلَين معًا**، **أو بقيمةٍ غيرِ
                      مُعلَنةٍ في التعداد** ⇒ **صفر**.
                      🔑 وإغلاقُ التعدادِ هو الشرطُ الحاسم، لا وجودُ الحقل: كان ③
                      يسألُ `is None` فقط، فكانت أيُّ قيمةٍ ثالثةٍ بابَ هروبٍ من
                      اللامتغيِّراتِ الأربعةِ جميعًا — ① يشترطُ `== "stable"`
                      و② يشترطُ `== "مؤجَّل"`، فما خالفَهما أفلتَ منهما ومن ③.
                      وبُرهنَ بالحقن: ذراعُ إرسالٍ محذوفةٌ + `status: experimental`
                      ⇒ ‏`①0 ②-1 ③0 ④0` ورمزُ **صفر**، والبندُ يختفي من
                      الميزانَين معًا. و`experimental` قيمةٌ **يباركُها مخطَّطُ
                      المستودعِ نفسُه** (`stability: [stable, experimental,
                      deprecated]`) فليست فرضًا بعيدًا.
       ④ ذراعٌ بلا إعلان: مرجعٌ مؤهَّلٌ لنطاقٍ ومعرِّفٍ لا يعرفُهما مصدرُ الحقيقة.

     ⚠️ **ولا يُقاسُ ههنا** الوصلُ بالاسمِ العربيِّ سلسلةً حرفيّة. جُرِّبَ وأُسقِط:
        المسحُ في `stdlib/` و`shared/` و`features/` يلتقطُ رسائلَ أخطاءٍ وأسماءَ
        ألوانٍ وأسماءَ مقاطعِ رابطٍ كما يلتقطُ أسماءَ المدمجات — قِيس: واحدٌ
        وثلاثونَ بندًا قُرِئَ «منفَّذًا» بشاهدٍ مثلِ `"خطأ", // false` في مُجمَّعِ
        السلاسل، و`"قفل"` اسمَ عرضٍ لعنصرِ تعداد. مسبارٌ غيرُ معايَرٍ لا يدخلُ بوّابة.

     🔑 **والألقابُ تُشتَقُّ من الشفرةِ ولا تُثبَّتُ باليد.** كانت مثبّتةً في نسخةٍ
        أولى (`Ar|Bn|Bs|Names`) والشجرةُ فيها عشرونَ لقبًا (`Bwsc` لعميلِ
        WebSocket، `Bm` للرياضيّات، `Bnu` لمنافعِ الشبكة…) — فوُسِمَ **سبعةٌ
        وثلاثونَ مدمجًا منفَّذًا دَينًا كاذبًا**، منها `Bm::PI` وهو يبعثُ ثابتًا،
        و`Bwsc::NEW_CLIENT` وهو يبعثُ أوپكودًا. ولقبٌ يُثبَّتُ باليدِ يبلى.

(EN) Guard: a declared builtin either claims to work (`status: stable` — needs a
     namespace-qualified dispatch reference, zero tolerance) or declares debt
     (`intent: مؤجَّل` — descending ceiling). Neither-or-both fails too. It is a
     DRIFT detector, not an independent audit: the marking was derived from this
     same probe. Namespace aliases are derived from the source, never hard-coded:
     hard-coding four of the tree's twenty mislabelled 37 implemented builtins as
     debt. Bare Arabic-string matching was tried and dropped — uncalibrated.
============================================================================
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parents[2]
SOT_DIR = ROOT / "language-truth" / "builtins"

# (AR) مناطقُ التنفيذِ المقيسة. التنفيذُ لا يعيشُ في المترجّمِ وحدَه.
AREAS = ("compiler", "shared", "runtime", "stdlib", "tools", "features", "platform")

# (AR) يُستبعَدُ: المولَّدُ من مصدرِ الحقيقة (يحملُ اسمَ كلِّ بندٍ مُعلَنٍ فيُثبِتُ
#      الإعلانَ لا التنفيذ) · وثالثُ الأطراف · **ومجلّداتُ الاختبارِ والبناء**
#      (بُرهنَ بالحقن: ملفُّ ترويسةٍ واحدٌ في `compiler/tests/` كان يكفي لغسلِ
#      ألفٍ ومئتَي بندٍ فتُقرأُ كلُّها منفَّذة).
# (EN) Excludes generated SoT mirrors, third-party, and test/build trees: one
#      header dropped in compiler/tests/ could launder 1204 of 1205 declarations.
SKIP_PARTS = ("third_party", "generated", "tests", "test", "build")

CPP_SUFFIXES = (".cpp", ".h", ".hpp", ".cc")

# ═══ السقوفُ المُودَعة ═══════════════════════════════════════════════════════
# (AR) ① الوعدُ الكاذبُ لا سقفَ له — الثابتُ ههنا توثيقٌ لا تسامح.
CEILING_FALSE_PROMISE = 0

# (AR) ② دَينُ التأجيل. مُقاسٌ في ٢٠٢٦-٠٩-٠٥ بمسبارٍ مؤهَّلٍ بالنطاقِ وألقابٍ
#      مشتقّة: ‏١٢٠٥ مُعلَنًا = ٦٠٦ منفَّذًا + ٥٩٩ مؤجَّلًا.
#      **ينزلُ بالوصلِ ولا يُرفَع** إلّا بقرارٍ مكتوبٍ يُسمّي الأسماءَ وسببَها.
#
#      (AR) ويحلُّ محلَّ `CEILING_COMPILER_MISSING = 564` الذي كان يسألُ سؤالًا
#      آخر — «هل يُرسِلُه المترجّم؟» — بمسبارٍ يمسحُ التعليقاتِ فيُخفَّضُ بسطرِ
#      تعليقٍ واحد (قِيس: ٥٦٤ ← ٥٥٩)، ويطابقُ أيَّ لفظٍ كبيرِ الحروف.
CEILING_DEFERRED = 599

# (AR) ③ حقلٌ فاسد: بلا حقلٍ، أو بالحقلَين، أو بقيمةٍ خارجَ التعداد.
CEILING_FIELD_BROKEN = 0

# (AR) 🔑 **التعدادُ مُغلَق.** كلُّ قيمةٍ سواهما فاسدة. وكلفةُ الإغلاقِ اليومَ صفر —
#      مقيسًا في ٢٠٢٦-٠٩-٠٥: كلُّ الـ٦٠٦ بالضبط `stable`، وكلُّ الـ٥٩٩ بالضبط
#      `مؤجَّل`، ولا قيمةَ ثالثةً في الشجرة. وتوسيعُ التعدادِ قرارٌ يُكتَبُ ههنا
#      ويُلحَقُ به مجسٌّ في المِحقنة — لا انجرافٌ يُكتَبُ في ملفِّ yaml.
#      ⚠️ ويُشتَرَطُ النصُّ قبلَ العضويّة: `status: [stable]` قائمةٌ لا تُجزَّأُ،
#         فكانت تُلقي `TypeError` فيخرجُ الحارسُ بالرمز ٢ «لم يقِسْ شيئًا» — وهو
#         عطبُ **محتوًى** في مصدرِ الحقيقةِ موضعُه ③. (قِيسَ في مراجعةٍ خصميّة.)
STATUS_VALUES = frozenset({"stable"})
INTENT_VALUES = frozenset({"مؤجَّل"})

# (AR) ④ مرجعٌ مؤهَّلٌ لا يعرفُه مصدرُ الحقيقة. مُقاسٌ صفرًا اليومَ فالسقفُ صفر:
#      كلُّ زوجٍ (نطاق، معرِّف) مُرسَلٍ في الشجرةِ مُعلَنٌ في مصدرِ الحقيقة.
#      🔑 وهذا الاتّجاهُ يقيسُ اليومَ ما لم يكن يقيسُه قبلُ: كانت الصيغتانِ
#      المُودَعتانِ (`funcName == "<عربي>"` وصفُّ جدولٍ) تطابقانِ **صفرًا** في
#      البانياتِ كلِّها — سقفٌ مضروبٌ على مجموعةٍ فارغةٍ أبدًا.
CEILING_UNDECLARED_ARMS = 0


# ═══ كاسحُ التعليقات ════════════════════════════════════════════════════════
def _strip_comments(src: str) -> str:
    """(AR) آلةُ حالاتٍ تحذفُ التعليقَ ولا تمسُّ سلسلةً ولا محرَفًا ولا سلسلةً خامّة.
    وتعبيرٌ نمطيٌّ لا يكفي: `/*` داخلَ سلسلةٍ يمدُّ التطابقَ عبرَ شفرةٍ حقيقيّة —
    قِيسَ ابتلاعُ ٤٥٪ من نصِّ المترجّمِ بهذه العلّةِ بعينِها.
    """
    out: list[str] = []
    i, n = 0, len(src)
    while i < n:
        c = src[i]
        if c == "R" and i + 1 < n and src[i + 1] == '"':          # R"tag( … )tag"
            j = src.find("(", i + 2)
            if j != -1:
                tag = src[i + 2:j]
                end = src.find(")" + tag + '"', j)
                if end != -1:
                    stop = end + len(tag) + 2
                    out.append(src[i:stop]); i = stop; continue
        if c in "\"'":
            quote = c
            out.append(c); i += 1
            while i < n:
                if src[i] == "\\":
                    out.append(src[i:i + 2]); i += 2; continue
                out.append(src[i])
                if src[i] == quote:
                    i += 1; break
                if src[i] == "\n" and quote == '"':                # سلسلةٌ لم تُغلَق
                    i += 1; break
                i += 1
            continue
        if c == "/" and i + 1 < n:
            if src[i + 1] == "/":
                j = src.find("\n", i)
                i = n if j == -1 else j
                continue
            if src[i + 1] == "*":
                j = src.find("*/", i + 2)
                out.append(" ")
                i = n if j == -1 else j + 2
                continue
        out.append(c); i += 1
    return "".join(out)


# ═══ المسابر ════════════════════════════════════════════════════════════════
# (AR) إعلانُ لقبٍ لفضاءِ أسماءِ المدمجات — يُشتَقُّ ولا يُثبَّت.
_ALIAS_DECL = re.compile(
    r"namespace\s+([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(?:::)?(?:Sad::)?"
    r"Builtins::Names(?:::([A-Za-z_][A-Za-z0-9_]*))?\s*;")
# (AR) جدولُ الرتبِ إشارةٌ ثانيةٌ مؤهَّلة: لا تُطلَبُ رتبةُ مدمجٍ إلّا لمُرسَل.
_ARITY_REF = re.compile(r"\bAr::([A-Za-z_][A-Za-z0-9_]*)::([A-Z][A-Z0-9_]*)\b")
_FULLY_QUALIFIED = re.compile(
    r"\bBuiltins::Names::([A-Za-z_][A-Za-z0-9_]*)::([A-Z][A-Z0-9_]*)\b")


def _sources() -> list[Path]:
    files: list[Path] = []
    for area in AREAS:
        base = ROOT / area
        if not base.is_dir():
            continue
        for path in base.rglob("*"):
            if path.suffix not in CPP_SUFFIXES or not path.is_file():
                continue
            if any(part in SKIP_PARTS for part in path.parts):
                continue
            files.append(path)
    return sorted(files)


def _dispatch_pairs() -> tuple[set[tuple[str, str]], dict[str, str], set[str]]:
    """(AR) كلُّ زوجِ (نطاق، معرِّف) مُشارٍ إليه إشارةً مؤهَّلةً بنطاقِه."""
    blob = "\n".join(
        _strip_comments(p.read_text(encoding="utf-8", errors="replace"))
        for p in _sources())

    scoped: dict[str, str] = {}
    roots: set[str] = {"Names"}
    for alias, ns in _ALIAS_DECL.findall(blob):
        if ns:
            scoped[alias] = ns
        else:
            roots.add(alias)

    pairs: set[tuple[str, str]] = set()
    if scoped:
        rx = re.compile(r"\b(" + "|".join(sorted(map(re.escape, scoped)))
                        + r")::([A-Z][A-Z0-9_]*)\b")
        for alias, cid in rx.findall(blob):
            pairs.add((scoped[alias], cid))
    rx_root = re.compile(r"\b(" + "|".join(sorted(map(re.escape, roots)))
                         + r")::([A-Za-z_][A-Za-z0-9_]*)::([A-Z][A-Z0-9_]*)\b")
    for _alias, ns, cid in rx_root.findall(blob):
        pairs.add((ns, cid))
    pairs.update(_FULLY_QUALIFIED.findall(blob))
    pairs.update(_ARITY_REF.findall(blob))
    return pairs, scoped, roots


def _declared() -> list[dict]:
    rows: list[dict] = []
    for path in sorted(SOT_DIR.glob("*.yaml")):
        doc = yaml.safe_load(path.read_text(encoding="utf-8")) or {}
        for fn in doc.get("functions") or []:
            rows.append({
                "ns": fn["namespace"],
                "cpp_id": fn["cpp_id"],
                "canonical": fn.get("canonical", ""),
                "status": fn.get("status"),
                "intent": fn.get("intent"),
                "file": path.name,
            })
    return rows


# ═══ عيارُ الأداةِ نفسِها ═══════════════════════════════════════════════════
# (AR) قناريّةُ الاتّجاهِ السالب: بندٌ مُعلَنٌ **مؤجَّلٌ** لا ذراعَ إرسالٍ له.
#      يجبُ ألّا يراهُ المسبارُ أبدًا. ولولاها لكانت المجسّاتُ كلُّها **دعاوى
#      وجود** — «هل ترى؟» — فيجتازُها حارسٌ مُفرَغٌ يزعمُ أنّ كلَّ مُعلَنٍ مُرسَل.
#      (بُرهنَ بالحقن: مسبارٌ يُرجِعُ كلَّ الأزواجِ طبعَ ٥/٥ ✓ وخرجَ بصفر.)
NEGATIVE_CANARY = ("KernelGpu", "GPU_0")


def _calibrate(pairs: set[tuple[str, str]], scoped: dict[str, str]) -> list[str]:
    """(AR) ستّةُ مجسّاتٍ تُنفَّذُ في كلِّ تشغيل. إخفاقُ أحدِها = عطبُ آلة."""
    fails: list[str] = []
    if _strip_comments('a // x\nb /* y */ c "d // e" f') != 'a \nb   c "d // e" f':
        fails.append("الكاسحُ لا يميّزُ التعليقَ من السلسلة")
    if "KEEP" not in _strip_comments('const char* s = "/*"; int KEEP = 1;'):
        fails.append("الكاسحُ يبتلعُ شفرةً بعدَ `/*` داخلَ سلسلة")
    if len(scoped) < 10:
        fails.append(f"اشتقاقُ الألقابِ عادَ بـ{len(scoped)} — والشجرةُ فيها عشرون")
    if ("Core", "LENGTH") not in pairs:
        fails.append("المسبارُ لا يرى Core.LENGTH وهو مُرسَلٌ قطعًا")
    # (AR) مجسُّ اللقب: مسارٌ لا يراه إلّا الاشتقاق (Bwsc = WebSocketClient).
    if ("WebSocketClient", "NEW_CLIENT") not in pairs:
        fails.append("المسبارُ لا يرى مسارَ اللقب WebSocketClient.NEW_CLIENT")
    # (AR) المجسُّ السالب: يعضُّ التوسيعَ حيثُ تعجزُ دعاوى الوجود.
    if NEGATIVE_CANARY in pairs:
        fails.append(f"المسبارُ يرى {'.'.join(NEGATIVE_CANARY)} وهو مؤجَّلٌ بلا إرسال"
                     " — مسبارٌ يوسِّعُ لا يقيس")
    return fails


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    rows = _declared()
    pairs, scoped, roots = _dispatch_pairs()

    probes = 6
    fails = _calibrate(pairs, scoped)
    print("حارس «الوعدُ المُعلَنُ يُقاسُ ولا يُدَّعى»:")
    if fails:
        print(f"  ✗ عيارُ الأداة: {probes - len(fails)}/{probes} — لا يُبنى على قراءتِها:")
        for msg in fails:
            print(f"      · {msg}")
        return 2
    print(f"  عيارُ الأداة: {probes}/{probes} ✓ "
          f"(ألقابٌ مشتقّة: {len(scoped)} · ألقابُ جذر: {len(roots)})")

    declared_keys = {(r["ns"], r["cpp_id"]) for r in rows}
    implemented = [r for r in rows if (r["ns"], r["cpp_id"]) in pairs]
    false_promise = [r for r in rows
                     if r["status"] == "stable" and (r["ns"], r["cpp_id"]) not in pairs]
    deferred = [r for r in rows if r["intent"] == "مؤجَّل"]
    broken = [r for r in rows
              if (r["status"] is None and r["intent"] is None)
              or (r["status"] is not None and r["intent"] is not None)
              or (r["status"] is not None
                  and (not isinstance(r["status"], str)
                       or r["status"] not in STATUS_VALUES))
              or (r["intent"] is not None
                  and (not isinstance(r["intent"], str)
                       or r["intent"] not in INTENT_VALUES))]
    undeclared = sorted(pairs - declared_keys)

    print(f"  معلَنٌ في مصدرِ الحقيقة: {len(rows)}")
    print(f"  له مرجعُ إرسالٍ مؤهَّل: {len(implemented)}")
    print(f"  ① وعدٌ كاذب (stable بلا إرسال): {len(false_promise)}"
          f" (المسموح {CEILING_FALSE_PROMISE} — لا سقفَ له)")
    print(f"  ② دَينُ تأجيل (intent: مؤجَّل): {len(deferred)}"
          f" (السقف {CEILING_DEFERRED} — نازلٌ لا يُرفَع)")
    print(f"  ③ حقلٌ فاسد (بلا حقلٍ، أو بالحقلَين، أو بقيمةٍ خارجَ التعداد):"
          f" {len(broken)}"
          f" (المسموح {CEILING_FIELD_BROKEN})")
    print(f"  ④ مرجعٌ بلا إعلان: {len(undeclared)} (السقف {CEILING_UNDECLARED_ARMS})")

    failed = False
    if len(false_promise) > CEILING_FALSE_PROMISE:
        print(f"  ✗ وعدٌ كاذب: {len(false_promise)} مدمجًا يقولُ `status: stable`"
              " ولا مرجعَ إرسالٍ له — يُوصَلُ أو يُوسَمُ `intent: مؤجَّل`:")
        for row in false_promise[:12]:
            print(f"      · {row['ns']}.{row['cpp_id']} ({row['canonical']}) — {row['file']}")
        if len(false_promise) > 12:
            print(f"      … و{len(false_promise) - 12} غيرُها")
        failed = True
    if len(deferred) > CEILING_DEFERRED:
        print(f"  ✗ نما دَينُ التأجيل: {len(deferred)} > {CEILING_DEFERRED} —"
              " إعلانٌ جديدٌ بلا إرسالٍ يُنفَقُ من حصّةٍ لا من سقفٍ يُرفَع.")
        failed = True
    if len(broken) > CEILING_FIELD_BROKEN:
        print(f"  ✗ حقلٌ فاسد: {len(broken)} — كلُّ مدمجٍ يحملُ `status:"
              f" {'|'.join(sorted(STATUS_VALUES))}` أو `intent:"
              f" {'|'.join(sorted(INTENT_VALUES))}`، واحدًا لا صفرًا ولا اثنَين،"
              " وبقيمةٍ من التعدادِ لا بقيمةٍ ثالثة:")
        for row in broken[:12]:
            print(f"      · {row['ns']}.{row['cpp_id']} — {row['file']}"
                  f" (status={row['status']!r} intent={row['intent']!r})")
        failed = True
    if len(undeclared) > CEILING_UNDECLARED_ARMS:
        print(f"  ✗ مرجعٌ بلا إعلان: {len(undeclared)} > {CEILING_UNDECLARED_ARMS} —"
              " أُرسِلَ زوجٌ لا يعرفُه مصدرُ الحقيقة:")
        for ns, cid in undeclared[:12]:
            print(f"      · {ns}::{cid}")
        failed = True
    if failed:
        return 1
    print("  ✓ لا وعدَ كاذبًا، ولم ينمُ دَينُ التأجيل، ولا حقلَ فاسدًا.")
    return 0


if __name__ == "__main__":
    # (AR) 🔑 انهيارٌ ليس حكمًا. بندٌ ناقصُ المفتاحِ في yaml، أو ملفٌّ ليس UTF-8،
    #      أو yaml لا يُحلَّل — كلُّها **لم تقِسْ شيئًا**، فرمزُها ٢ لا ١. ولولا
    #      هذا لطبعَ x.py «فشل حارس الاتّساق» (عطبُ محتوًى يُتَّهمُ به الطوّر)
    #      عن انهيارٍ في الأداةِ نفسِها. والرمزُ ١ محجوزٌ لحكمٍ صدرَ عن قياسٍ تمّ.
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:  # noqa: BLE001 — كلُّ انهيارٍ عطبُ آلةٍ لا حكم
        sys.stdout.reconfigure(encoding="utf-8")
        print(f"  ✗ عطبُ آلةٍ — لم يُقَسْ شيء: {exc.__class__.__name__}: {exc}")
        raise SystemExit(2)
