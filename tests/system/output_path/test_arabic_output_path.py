"""
============================================================================
(AR) **مسارُ المخرَجِ العربيُّ يُبنى عريضًا لا ضيّقًا — وإلّا سقطَ البرنامجُ
     بعدَ الربطِ بلا حرفٍ واحدٍ في stdout ولا في stderr.**

     العطبُ المقيس (٢٠٢٦-٠٨-٢٩، ويندوز Debug بـ‎/MDd‎ حقيقيّ): السائقُ كان
     يبني `std::filesystem::path` من نصٍّ **ضيّقٍ** بالمُنشئِ المباشر. وMSVC
     يحوّلُ النصَّ الضيّقَ بصفحةِ ترميزِ النظامِ لا بـUTF‑8، فأيُّ اسمِ مخرَجٍ
     بحروفٍ عربيّةٍ يُطلقُ توكيدًا في CRT التنقيحيّ ⇒ `0x80000003` بمخرَجٍ
     فارغٍ تمامًا. ٣٩٦ إخفاقًا من ٧٤٧ في مجموعةِ P1، وكلُّها بالعَرَضِ نفسِه.

     ⚠️ **ولا شيءَ في الشجرةِ كان يكتبُ مخرَجًا باسمٍ عربيّ**: مجموعةُ P1 كلُّها
     تُخرِجُ إلى أسماءٍ مولَّدةٍ لاتينيّة، فالعطبُ نجا من ٧٤٧ برنامجًا. وهو
     المعنى الحرفيُّ لـ«أخضرُ لأنّه غيرُ مقيس»: العلّةُ لم تكن نادرةً بل
     **خارجَ ما تلمسُه المجموعة**.

     ⚠️ وضابطُ المسارِ اللاتينيِّ ليس زينة: سائقٌ مكسورٌ بالكلّيّةِ (لا يربطُ
     أصلًا) يُخفِقُ في الحالةِ العربيّةِ فيُقرأُ الإخفاقُ «عطبُ ترميز» وهو عطبُ
     ربط. فالضابطُ يُثبتُ أنّ الفارقَ **حروفُ الاسمِ وحدَها**.

(EN) The compiler driver must build output paths as UTF-8-aware wide paths.
     Measured on a real /MDd Windows Debug build: constructing
     std::filesystem::path from a narrow UTF-8 string makes MSVC convert via
     the system codepage, so any Arabic output name tripped a debug-CRT
     assertion (0x80000003) with completely empty stdout AND stderr — 396 of
     747 P1 programs. Nothing in the tree wrote an Arabic output name (P1
     generates Latin ones), so the defect survived 747 programs: green because
     UNMEASURED, not green because correct. The Latin control proves the only
     difference is the letters in the name, not linking in general.
============================================================================
"""
from __future__ import annotations

import os
import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

# (AR) ⚠️ مساراتٌ بلا ‎.exe‎ أيضًا — على لينكس وماك الثنائيُّ ‎build/bin/sad-build‎.
#      بدونها يبتلع ‎skipif‎ الملفَّ كلَّه **أخضرَ صامتًا**.
# (EN) Extension-less candidates too; otherwise skipif eats the file green.
_CANDIDATES = [
    ROOT / "build" / "bin" / "Debug" / "sad-build.exe",
    ROOT / "build" / "bin" / "Release" / "sad-build.exe",
    ROOT / "build" / "bin" / "sad-build.exe",
    ROOT / "build" / "bin" / "Debug" / "sad-build",
    ROOT / "build" / "bin" / "Release" / "sad-build",
    ROOT / "build" / "bin" / "sad-build",
]
SAD_BUILD = max(
    (p for p in _CANDIDATES if p.exists()),
    # (AR) أحدثُ الموجودَين لا أوّلُهما — ثنائيٌّ قديمٌ يسبقُ طازجًا يولّدُ عطبًا وهميًّا.
    # (EN) Newest, not first: a stale binary ahead of a fresh one fabricates defects.
    key=lambda p: p.stat().st_mtime,
    default=None,
)

pytestmark = pytest.mark.skipif(
    SAD_BUILD is None,
    reason="sad-build غير مبنيّ — يتطلّب: cmake --build build --target sad-build",
)

# (AR) أبسطُ برنامجٍ يُثبِتُ أنّ العمليّةَ **بدأت** فعلًا: الطباعةُ وحدَها.
#      رمزُ الخروجِ صفرًا لا يكفي — الانهيارُ قبلَ الطباعةِ يعطي مخرَجًا فارغًا
#      وقد يعطي رمزًا يُقرأُ نجاحًا على بعضِ الأصداف.
SOURCE = 'اطبع_سطر("سلام")\n'
EXPECTED = "سلام"

EXE_SUFFIX = ".exe" if os.name == "nt" else ""


def _compile_and_run(work: Path, directory_name: str, file_stem: str):
    """(AR) يُترجمُ المصدرَ إلى `directory_name/file_stem` ثمّ يُشغّلُه.

    (EN) Compile to directory_name/file_stem and run it.
    Returns (compile_proc, output_path, run_proc_or_None).
    """
    source = work / "مصدر.ص"
    source.write_text(SOURCE, encoding="utf-8")

    out_dir = work / directory_name
    out_dir.mkdir(parents=True, exist_ok=True)
    output = out_dir / (file_stem + EXE_SUFFIX)

    compile_proc = subprocess.run(
        [str(SAD_BUILD), str(source), "-o", str(output)],
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        timeout=300,
    )
    if compile_proc.returncode != 0 or not output.exists():
        return compile_proc, output, None

    run_proc = subprocess.run(
        [str(output)],
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        timeout=300,
        cwd=str(out_dir),
    )
    return compile_proc, output, run_proc


def _judge(label: str, compile_proc, output: Path, run_proc) -> None:
    assert compile_proc.returncode == 0, (
        f"{label}: الترجمةُ أخفقت (رمز {compile_proc.returncode})\n"
        f"{compile_proc.stdout}\n{compile_proc.stderr}"
    )
    assert output.exists(), (
        f"{label}: رمزُ نجاحٍ بلا ثنائيّ — {output}\n"
        f"{compile_proc.stdout}\n{compile_proc.stderr}"
    )
    assert run_proc is not None, f"{label}: لم يُشغَّل الثنائيّ"
    # (AR) الرمزُ **ثمّ** المخرَج: انهيارُ ويندوزَ لا يكتبُ في stderr حرفًا،
    #      فالحكمُ على المخرَجِ وحدَه يُسمّي العَرَضَ لا السبب.
    assert run_proc.returncode == 0, (
        f"{label}: الثنائيُّ خرجَ بالرمز {run_proc.returncode} "
        f"(بالسادس عشر {run_proc.returncode & 0xFFFFFFFF:#x})\n"
        f"stdout={run_proc.stdout!r} stderr={run_proc.stderr!r}"
    )
    assert EXPECTED in run_proc.stdout, (
        f"{label}: المخرَجُ لا يحوي {EXPECTED!r} — "
        f"stdout={run_proc.stdout!r} stderr={run_proc.stderr!r}"
    )


def test_latin_output_path_is_the_control():
    """(AR) الضابط: مسارٌ لاتينيٌّ بحتٌ يجبُ أن يمرَّ — وإلّا فالعطبُ في الربطِ لا في الترميز."""
    with tempfile.TemporaryDirectory() as tmp:
        work = Path(tmp)
        _judge("ضابطٌ لاتينيّ", *_compile_and_run(work, "out_dir", "program"))


def test_arabic_output_file_name():
    """(AR) اسمُ الملفِّ عربيٌّ والمجلّدُ لاتينيّ — يعزلُ اسمَ الملفِّ وحدَه."""
    with tempfile.TemporaryDirectory() as tmp:
        work = Path(tmp)
        _judge("اسمُ ملفٍّ عربيّ", *_compile_and_run(work, "out_dir", "برنامج"))


def test_arabic_output_directory_and_file_name():
    """(AR) المجلّدُ والاسمُ عربيّان معًا — الحالةُ التي يكتبُها مستخدمُ ص فعلًا."""
    with tempfile.TemporaryDirectory() as tmp:
        work = Path(tmp)
        _judge("مجلّدٌ واسمٌ عربيّان", *_compile_and_run(work, "مجلّد_المخرَج", "برنامج"))
