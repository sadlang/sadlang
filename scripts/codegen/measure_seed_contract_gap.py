# -*- coding: utf-8 -*-
"""
(AR) مِقياسُ عدّادَي «أ» و«هـ» في `CLAUDE.md` — البذورُ المتخطّاةُ والبذورُ بلا عقد.

     🔑 لماذا سكربتٌ لا فقرةُ نثر؟ لأنّ العدّادَ الذي تعريفُه في النثرِ يُعادُ
     بناؤه في كلِّ قراءةٍ فيُنتِجُ رقمًا مختلفًا. مقيسٌ (٢٠٢٦-٠٩-٠٣): العدّادُ
     «هـ» قيسَ **ثلاثَ مرّاتٍ بثلاثةِ تعريفاتٍ مُعادِ بنائِها من الوصفِ نفسِه**
     فأعطى ١٨٥ (الوثيقة) و١٨٤ (مراجعة) و١٩٠ (قراءةٌ حرفيّة) — ولا واحدَ منها
     خاطئٌ بالضرورة، بل التعريفُ هو الغائب. والوثيقةُ نفسُها تَعِدُ صراحةً:
     «فكلُّ رقمٍ هنا يُعادُ قياسُه بأمرٍ يُذكَرُ معه» — وهذا الملفُّ هو ذلك الأمر.

     والتعريفانِ هنا **شفرةٌ لا وصف**، فيُحاجَجُ فيهما ويُغيَّرانِ بإيداعٍ لا
     بقراءة:
       أ · بذرةٌ متخطّاة   = ملفٌّ فيه سطرٌ يبدأ بـ`# @skip_compiler`.
       هـ · بذرةٌ بلا عقد = ملفٌّ **يُشغَّلُ** (ليس متخطًّى) ولا يحملُ `@expected`
                            ولا وسمًا سالبًا ⇒ يُشغَّلُ ولا يُحاكَمُ على شيء.

     و`_archive` مستثنًى في العدّادَين: شجرةٌ محفوظةٌ لا تُشغَّل.

(EN) Measures counters "أ" (skipped seeds) and "هـ" (seeds with no contract) from
     CLAUDE.md. Why a script and not a paragraph? Because a counter whose
     definition lives in prose is re-derived on every reading and yields a
     different number each time: "هـ" was measured three times from the same
     description and gave 185, 184 and 190. None is necessarily wrong — the
     DEFINITION was missing. The definitions here are code, so they are argued
     with and changed by a commit, not by a reading.
"""
import argparse
import io
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SEEDS_ROOT = os.path.join(ROOT, "tests", "behavior")
SAD_SUFFIX = u".ص"

# (AR) شجرةٌ محفوظةٌ لا تُشغَّل — استثناؤها جزءٌ من التعريفِ لا تفصيلُ تنفيذ.
# (EN) An archived tree that never runs — its exclusion is part of the definition.
EXCLUDED_DIRS = ("_archive",)

SKIP_MARK = re.compile(u"^#\\s*@skip_compiler", re.M)
EXPECTED_MARK = re.compile(u"^#\\s*@expected", re.M)
NEGATIVE_MARK = re.compile(u"^#\\s*@(expect_error|expect_reject|expect_reject_any)", re.M)


def iter_seeds():
    """(AR) يمشي بذورَ السلوكِ كلَّها عدا الشجرةَ المحفوظة."""
    for base, dirs, files in os.walk(SEEDS_ROOT):
        dirs[:] = [d for d in dirs if d not in EXCLUDED_DIRS]
        for name in files:
            if name.endswith(SAD_SUFFIX):
                yield os.path.join(base, name)


def group_of(path):
    """(AR) المجلّدُ الأعلى الذي تنتمي إليه البذرة."""
    rel = os.path.relpath(path, SEEDS_ROOT).replace("\\", "/")
    return rel.split("/")[0]


def classify():
    """(AR) يردُّ (متخطّاة، بلا عقد) خريطتَي مجلّد←عدد، مع الإجماليّ."""
    skipped, no_contract, total = {}, {}, 0
    for path in iter_seeds():
        try:
            text = io.open(path, encoding="utf-8").read()
        except (IOError, UnicodeDecodeError):
            continue
        total += 1
        group = group_of(path)
        is_skipped = bool(SKIP_MARK.search(text))
        if is_skipped:
            skipped[group] = skipped.get(group, 0) + 1
            # (AR) بذرةٌ لا تُشغَّلُ لا تُوصَفُ بأنّها «تُشغَّلُ ولا تُحاكَم».
            # (EN) A seed that never runs is not "runs without being judged".
            continue
        if not EXPECTED_MARK.search(text) and not NEGATIVE_MARK.search(text):
            no_contract[group] = no_contract.get(group, 0) + 1
    return skipped, no_contract, total


def render(title, counts):
    total = sum(counts.values())
    detail = u" · ".join(u"%s %d" % (k, v)
                         for k, v in sorted(counts.items(), key=lambda kv: (-kv[1], kv[0])))
    sys.stdout.write(u"  %-34s %4d   %s\n" % (title, total, detail))
    return total


def main():
    parser = argparse.ArgumentParser(add_help=True, description=__doc__)
    parser.add_argument("--أ", dest="expect_skipped", type=int, default=None,
                        help="(AR) العددُ المتوقَّعُ للعدّاد أ — يُخفِقُ إن خالفَه المقيس")
    parser.add_argument("--هـ", dest="expect_no_contract", type=int, default=None,
                        help="(AR) العددُ المتوقَّعُ للعدّاد هـ — يُخفِقُ إن خالفَه المقيس")
    args = parser.parse_args()

    skipped, no_contract, total = classify()

    sys.stdout.write(u"═" * 70 + u"\n")
    sys.stdout.write(u"  مِقياسُ عدّادَي أ وهـ — %d بذرةً (بلا %s)\n"
                     % (total, u"، ".join(EXCLUDED_DIRS)))
    sys.stdout.write(u"═" * 70 + u"\n")
    a = render(u"أ · متخطّاة (@skip_compiler)", skipped)
    e = render(u"هـ · تُشغَّلُ بلا عقد", no_contract)
    sys.stdout.write(u"─" * 70 + u"\n")

    failures = []
    if args.expect_skipped is not None and args.expect_skipped != a:
        failures.append(u"أ: المتوقَّع %d والمقيس %d" % (args.expect_skipped, a))
    if args.expect_no_contract is not None and args.expect_no_contract != e:
        failures.append(u"هـ: المتوقَّع %d والمقيس %d" % (args.expect_no_contract, e))

    for line in failures:
        sys.stdout.write(u"  ❌ %s\n" % line)
    if failures:
        return 1
    sys.stdout.write(u"  ✅ مقيسٌ — والتعريفُ في هذا الملفِّ لا في نثرٍ خارجَه.\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
