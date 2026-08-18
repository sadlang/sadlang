# -*- coding: utf-8 -*-
"""
(AR) حارسُ عقدِ مسارِ المحرّك — `tests/runner.py`.

     العقد: كلُّ مسارِ محرّكٍ يُسلَّم إلى `subprocess` **مطلقٌ**.

     🔑 ولمَ حارسٌ لهذا بالذات: `run_interpreter` يُشغّلُ المحرّكَ في مجلّدِ عملٍ
     خاصٍّ (`cwd=work_dir`). ومسارٌ نسبيٌّ يُحَلُّ على POSIX **بعدَ** `chdir`
     فيتبخّر ⇒ `FileNotFoundError` في مللي ثانيةٍ تُقرأ `FAIL_INTERP` على كلِّ
     ملفّ. وعلى ويندوز يُكمِلُ `CreateProcess` الاسمَ الجزئيَّ بمجلّدِ العمليّةِ
     الأمِّ فيمرّ.

     ⚠️ وهذا الفرقُ بالذات هو ما جعلَ العطبَ يعيش: أخضرُ على المنصّةِ التي
     يُطوَّرُ عليها، أحمرُ على المنصّتَين اللتَين لا يُقرأُ سجلُّهما — ومبتلَعٌ
     هناك بـ`|| echo`. فبقيَ التكافؤُ الثنائيُّ **صفرًا** ولا حارسَ يقول.

(EN) Engine-path contract guard for tests/runner.py: every engine path handed to
     subprocess must be ABSOLUTE, because the engine runs with a private cwd and
     POSIX resolves a relative program only after the chdir.
"""
import importlib.util
import os
import subprocess
import sys
import tempfile
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]
RUNNER = REPO_ROOT / "tests" / "runner.py"


def _load_runner():
    """(AR) يستورد runner.py وحدةً — لأنّ ما لا يُستورَد لا يُحرَس."""
    spec = importlib.util.spec_from_file_location("sad_dual_runner", RUNNER)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def test_resolve_binary_returns_absolute_for_relative_input():
    """(AR) الحدُّ الأدنى: مسارٌ نسبيٌّ موجودٌ يُرَدُّ مطلقًا."""
    runner = _load_runner()
    # (AR) الغيابُ نفسُه كسرُ عقدٍ لا انهيارُ أداة: دالّةٌ مُعشَّشةٌ في `main`
    #      لا تُستورَد، وما لا يُستورَد لا يُحرَس — فيُحكَمُ عليه صراحةً.
    assert hasattr(runner, "_resolve_binary"), (
        "عقدٌ مكسور: `_resolve_binary` ليست على مستوى الوحدة — "
        "فلا يمكن حراسةُ عقدِ «مطلقٌ دائمًا» أصلًا"
    )
    previous = os.getcwd()
    os.chdir(str(REPO_ROOT))
    try:
        relative = Path("tests") / "runner.py"
        assert relative.exists(), "شرطُ المِجَسّ: الملفُّ النسبيُّ موجودٌ من جذرِ المستودع"
        resolved = runner._resolve_binary(relative)
        assert resolved.is_absolute(), (
            "عقدٌ مكسور: `_resolve_binary` ردَّ مسارًا نسبيًّا "
            f"({resolved}) — يتبخّر على POSIX عندَ تغييرِ cwd"
        )
        assert resolved.exists()
    finally:
        os.chdir(previous)


def test_relative_program_vanishes_when_cwd_changes():
    """
    (AR) الشاهدُ السالب: الآليّةُ نفسُها تُقاس لا تُدَّعى.

         يُشغَّلُ مُفسِّرُ پايثونَ **بمسارٍ نسبيّ** ومعه `cwd` مغايرٌ. فإن نجحَ
         فالمنصّةُ تُكمِلُ بالأمِّ (ويندوز)، وإن أخفقَ فهي تُحلُّ بعدَ `chdir`
         (POSIX). وفي الحالتَين: **العقدُ المطلقُ هو الوحيدُ الذي يعملُ في
         الاثنتَين** — وهذا ما يُثبِتُه هذا الشاهد.
    """
    interpreter = Path(sys.executable)
    relative_program = os.path.relpath(str(interpreter), str(interpreter.parent))
    outside = tempfile.mkdtemp()
    try:
        subprocess.run([relative_program, "-c", "pass"], cwd=outside,
                       capture_output=True, timeout=60)
        resolves_against_parent = True
    except (FileNotFoundError, OSError):
        resolves_against_parent = False

    # (AR) بالمسارِ المطلقِ يجب أن ينجحَ على كلِّ منصّة — بلا استثناء.
    done = subprocess.run([str(interpreter), "-c", "pass"], cwd=outside,
                          capture_output=True, timeout=60)
    assert done.returncode == 0, (
        "المسارُ المطلقُ يجب أن يعملَ في أيِّ مجلّدِ عملٍ على أيِّ منصّة"
    )
    # (AR) لا نحكمُ على المنصّة — نُسجِّلُ سلوكَها ليُقرأ في السجلّ.
    print("  المسارُ النسبيُّ مع cwd مغاير: "
          + ("يُكمَلُ بالعمليّةِ الأمّ (ويندوز)" if resolves_against_parent
             else "يتبخّر بعدَ chdir (POSIX)"))


def test_interpreter_failure_reason_is_printed():
    """
    (AR) رمزُ الإخفاقِ وحدَه لا يُشخِّص: `FAIL_INTERP` يجب أن يُطبَعَ معه سببُه،
         وإلّا صار «❌ FAIL_INTERP 001_hello.ص (مفسر: 1ms)» سطرًا لا يُميّز
         «المحرّكُ لم يوجَدْ» من «المحرّكُ رفضَ البرنامج».
    """
    source = RUNNER.read_text(encoding="utf-8")
    marker = "if result.error_message and result.status in ("
    start = source.index(marker)
    window = source[start:start + 400]
    assert "Status.FAIL_INTERP" in window, (
        "عقدٌ مكسور: خطأُ المفسّرِ لا يُطبَع — فيظهرُ في CI رمزًا بلا سبب"
    )


def main() -> int:
    failures = []
    for name, fn in sorted(globals().items()):
        if not name.startswith("test_") or not callable(fn):
            continue
        try:
            fn()
            print(f"  ✅ {name}")
        except AssertionError as exc:
            failures.append((name, str(exc)))
            print(f"  ❌ {name}\n     ↳ {exc}")
        except Exception as exc:  # (AR) انهيارُ الحارسِ عطبٌ يُحكَم لا يُنشَر مسارًا
            failures.append((name, f"{type(exc).__name__}: {exc}"))
            print(f"  ❌ {name}\n     ↳ {type(exc).__name__}: {exc}")
    if failures:
        print(f"❌ حارسُ مسارِ المحرّك: {len(failures)} عقدًا مكسورًا")
        return 1
    print("✅ حارسُ مسارِ المحرّك: العقدُ سليم — كلُّ مسارِ محرّكٍ مطلق")
    return 0


if __name__ == "__main__":
    sys.exit(main())
