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

     التفعيل: `SAD_LOWLEVEL_REQUIRE_BUILD=1` في البيئة. تُضبَط في CI حيث نعلم
     أنّ المترجمَ بُني في هذا التشغيل بالذات. ومحلّيًّا تبقى غيرَ مضبوطة، فمن
     لم يبنِ المترجمَ بعدُ يرى تخطّيًا لا شلّالَ إخفاقات.

(EN) Guard against silent skips. Every test file here does its own sad-build
     discovery and carries skipif(SAD_BUILD is None); the candidate lists were
     .exe-only, so on Linux — the one CI platform that actually builds the
     compiler — the whole suite skipped itself and reported success
     (measured: 75 passed, 51 skipped, job green). Fixing the lists does not
     stop the next file from repeating the drift, so this catches any skip for
     that reason regardless of source. Enabled by SAD_LOWLEVEL_REQUIRE_BUILD=1.
============================================================================
"""
from __future__ import annotations

import os

# (AR) شاهدُ السبب: نصُّ `reason` المشترك في كلّ ملفّات المجلّد.
_MISSING_BUILD_MARK = "sad-build"
_REQUIRE_ENV = "SAD_LOWLEVEL_REQUIRE_BUILD"

_skipped_for_missing_build: list[str] = []


def pytest_runtest_logreport(report) -> None:
    if not report.skipped:
        return
    # (AR) سببُ التخطّي يقع في `longrepr` كثلاثيّة (ملفّ، سطر، نصّ).
    reason = ""
    if isinstance(report.longrepr, tuple) and len(report.longrepr) == 3:
        reason = str(report.longrepr[2])
    else:
        reason = str(report.longrepr or "")
    if _MISSING_BUILD_MARK in reason:
        _skipped_for_missing_build.append(report.nodeid)


def pytest_sessionfinish(session, exitstatus) -> None:  # noqa: ARG001
    if os.environ.get(_REQUIRE_ENV) != "1":
        return
    if not _skipped_for_missing_build:
        return
    count = len(_skipped_for_missing_build)
    sample = "\n  ".join(_skipped_for_missing_build[:5])
    print(
        f"\n❌ {count} اختبارًا تخطّى لعدم العثور على ثنائيّ sad-build، "
        f"و{_REQUIRE_ENV}=1 يقول إنّه مبنيّ.\n"
        f"  السببُ المرجَّح: قائمةُ ‎_CANDIDATES‎ في ملفٍّ ما لا تحمل نظيرَ المسار "
        f"بلا ‎.exe‎ (لينكس/ماك).\n  أمثلة:\n  {sample}"
    )
    session.exitstatus = 1
