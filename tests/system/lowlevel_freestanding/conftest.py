"""
============================================================================
(AR) حارسُ «التخطّي الصامت» لاختبارات الوضع الحرّ.

     العطبُ المقيس الذي ولّد هذا الملفّ: كلُّ اختبارٍ في هذا المجلّد يبني قائمةَ
     مرشّحيه لثنائيّ `sad-build` بنفسه، ويحمل
     `pytestmark = skipif(SAD_BUILD is None)`. وقوائمُ المرشّحين كانت مساراتِ
     `.exe` حصرًا، فعلى لينكس — وهي المنصّةُ الوحيدةُ التي تبني المترجمَ في CI —
     لم يُعثَر على الثنائيّ قطّ، فتخطّت المجموعةُ نفسَها وخرجت **خضراء**.
     قِيس في أوّل تشغيلٍ لخطوة CI: `75 passed, 51 skipped`، والوظيفةُ ناجحة.

     ⚠️ ولماذا حارسٌ لا إصلاحُ القوائمِ وحدَه: أُصلحت القوائمُ فعلًا، لكنّ
     الإصلاحَ لا يمنع الملفَّ **التالي** من تكرارِ العيب. القوائمُ إحدى عشرةَ
     نسخةً من كتلةٍ واحدة، وقد انحرفت فعلًا مرّةً. فالحارسُ هنا يمسك أيَّ تخطٍّ
     لهذا السبب مهما كان مصدرُه، بلا حاجةٍ إلى تذكُّرِ أحد.

     التفعيل: `SAD_LOWLEVEL_REQUIRE_BUILD=1` للمترجم، و`SAD_REQUIRE_CLANG=1`
     للمُجمِّع. تُضبَطان في CI حيث نعلم أنّ المورِدَ متاحٌ في هذا التشغيل بالذات.
     ومحلّيًّا تبقيان غيرَ مضبوطتين، فمن لم يبنِ المترجمَ بعدُ يرى تخطّيًا لا
     شلّالَ إخفاقات.

(EN) Guard against silent skips. Every test file here does its own sad-build
     discovery and carries skipif(SAD_BUILD is None); the candidate lists were
     .exe-only, so on Linux — the one CI platform that actually builds the
     compiler — the whole suite skipped itself and reported success
     (measured: 75 passed, 51 skipped, job green). Fixing the lists does not
     stop the next file from repeating the drift, so this catches any skip for
     that reason regardless of source. Enabled by SAD_LOWLEVEL_REQUIRE_BUILD=1
     (and SAD_REQUIRE_CLANG=1 for the assembling judgement).
============================================================================
"""
from __future__ import annotations

import os
import sys


def _report(message: str) -> None:
    """(AR) يكتب رسالةَ الحارس بلا أن يسقط على طرفيّةٍ ضيّقةِ الترميز.

    العطبُ المقيس: `print` برسالةٍ عربيّةٍ فيها ‎❌‎ يرفع `UnicodeEncodeError`
    على طرفيّةِ ويندوز بصفحةِ ترميزٍ غيرِ UTF-8 — فيسقط `pytest_sessionfinish`
    باستثناءٍ **يبتلع الرسالةَ نفسَها**، وهي كلُّ ما لأجله وُضِع الحارس.
    """
    text = message + "\n"
    stream = sys.stderr
    try:
        stream.write(text)
    except UnicodeEncodeError:
        encoding = stream.encoding or "ascii"
        stream.write(text.encode(encoding, "backslashreplace").decode(encoding))

# (AR) شاهدا السبب: نصُّ `reason` المشترك لكلّ صنفِ تخطٍّ.
#      و«clang» أُضيف حين صار في المجلّد حكمٌ يُجمِّع الناتجَ فعلًا (لا يقرأ نصَّ
#      IR وحدَه)؛ تخطّيه يُفرِغ ذلك الحكمَ من برهانه ويُبقي الملفَّ أخضر — وهو
#      العطبُ نفسُه الذي وُضِع هذا الحارسُ لأجله، بمورِدٍ آخر.
_SKIP_MARKS = {
    "sad-build": (
        "SAD_LOWLEVEL_REQUIRE_BUILD",
        "لعدم العثور على ثنائيّ sad-build",
        "قائمةُ ‎_CANDIDATES‎ في ملفٍّ ما لا تحمل نظيرَ المسار بلا ‎.exe‎ (لينكس/ماك)",
    ),
    "clang": (
        "SAD_REQUIRE_CLANG",
        "لعدم العثور على clang",
        "‎clang‎ ليس في المسار ولا في متغيّر البيئة ‎SAD_CLANG‎",
    ),
}

_skipped: dict[str, list[str]] = {mark: [] for mark in _SKIP_MARKS}


def pytest_runtest_logreport(report) -> None:
    if not report.skipped:
        return
    # (AR) سببُ التخطّي يقع في `longrepr` كثلاثيّة (ملفّ، سطر، نصّ).
    if isinstance(report.longrepr, tuple) and len(report.longrepr) == 3:
        reason = str(report.longrepr[2])
    else:
        reason = str(report.longrepr or "")
    for mark in _SKIP_MARKS:
        if mark in reason:
            _skipped[mark].append(report.nodeid)


def pytest_sessionfinish(session, exitstatus) -> None:  # noqa: ARG001
    for mark, (env, what, likely) in _SKIP_MARKS.items():
        nodes = _skipped[mark]
        if not nodes or os.environ.get(env) != "1":
            continue
        sample = "\n  ".join(nodes[:5])
        _report(
            f"\n❌ {len(nodes)} اختبارًا تخطّى {what}، و{env}=1 يقول إنّه متاح.\n"
            f"  السببُ المرجَّح: {likely}.\n  أمثلة:\n  {sample}"
        )
        session.exitstatus = 1
