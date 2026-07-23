"""
============================================================================
(AR) اختبارات منع انحدار: إكمال وقت التشغيل الحرّ بـ memmove وmemcmp.
     clang تحت -ffreestanding يفترض وجود memset/memcpy/memmove/memcmp عند
     خفض intrinsics. المترجم كان يُصدر memset/memcpy الضعيفتين فقط؛ فبقيت
     memmove/memcmp بلا مزوّد ⇒ يلزم تعريفهما في طبقة C خارجيّة (نواة النحلة
     تراب.c). هذا التغيير يُصدرهما ضعيفتين (weak_odr) نظير memset/memcpy،
     فيصير وقت التشغيل الحرّ مكتفيًا ذاتيًّا (يسدّ آخر اعتماد على تراب.c لهما).

     الحرّاس هنا:
       - memmove/memcmp مُعرَّفتان (define) لا مُصرَّحتان فقط، بربط weak_odr.
       - كلّ منهما يظهر رمزًا **مرّة واحدة** (التعريف) ⇒ لا نداء ذاتيّ ولّده
         تمييز LLVM للحلقات (loop-idiom) ⇒ لا تكرار لانهائيّ (يمنعه no-builtins).
       - memset/memcpy ما زالتا مُصدَّرتين (لا انحدار).
       - الوحدة تجتاز مدقّق LLVM (رمز خروج 0) ⇒ IR سليم بنيويًّا (PHI/هيمنة).
(EN) Regression tests: freestanding runtime completed with memmove and memcmp,
     emitted weak_odr like memset/memcpy so the freestanding runtime is
     self-sufficient (removes the last dependency on an external C layer).
----------------------------------------------------------------------------
(AR) لماذا --أظهر-llvm؟ الخفض يعيش في توليد LLVM؛ الراية تُخرج IR بلا اعتماد
     على lld-link/LIB (عزل عن بيئة الربط) — نمط test_cpu_ctl_i686.
============================================================================
"""
from __future__ import annotations

import re
import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

_CANDIDATES = [
    ROOT / "build" / "bin" / "Release" / "sad-build.exe",
    ROOT / "build" / "bin" / "Debug" / "sad-build.exe",
    ROOT / "build" / "bin" / "sad-build.exe",
]
SAD_BUILD = next((p for p in _CANDIDATES if p.exists()), None)

pytestmark = pytest.mark.skipif(
    SAD_BUILD is None,
    reason="sad-build.exe غير مبني — يتطلب: python x.py build --config Release",
)

FREESTANDING = "--حرّ"
EMIT_LLVM = "--أظهر-llvm"

# (AR) برنامج حرّ تافه — يكفي لتشغيل مسار emitFreestandingRuntime (يُصدر
#      malloc/free/memcpy/memmove/memset/memcmp… بلا اعتماد على استعمالها).
_TRIVIAL = "متغير س = 1\nاطبع(س)\n"


def _compile_ir(source: str) -> tuple[int, str, str]:
    """(AR) يترجم المصدر حرًّا مع إخراج IR؛ يعيد (رمز الخروج، الخرج، IR)."""
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(source, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), FREESTANDING, EMIT_LLVM, "-o", str(out_ll)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=120, cwd=work,
        )
        ir = out_ll.read_text(encoding="utf-8", errors="replace") if out_ll.exists() else ""
        return proc.returncode, (proc.stdout or "") + (proc.stderr or ""), ir


def _define_re(name: str) -> re.Pattern:
    # define [linkage...] <ret> @name(
    return re.compile(r"define\b[^\n]*@" + re.escape(name) + r"\s*\(")


def test_memmove_defined_weak_odr():
    """(AR) memmove مُعرَّفة weak_odr في الوضع الحرّ (لا مجرّد تصريح)."""
    code, out, ir = _compile_ir(_TRIVIAL)
    assert code == 0, "الترجمة الحرّة فشلت (IR غير سليم؟):\n" + out
    m = _define_re("memmove").search(ir)
    assert m, "IR الحرّ بلا `define ... @memmove(` — لم يُصدَر التعريف:\n" + ir[:1500]
    line = ir[m.start():ir.find("\n", m.start())]
    assert "weak_odr" in line, "memmove ليست weak_odr (تنجو من llvm-link ودمج الوحدات):\n" + line


def test_memcmp_defined_weak_odr():
    """(AR) memcmp مُعرَّفة weak_odr وتعيد i32."""
    code, out, ir = _compile_ir(_TRIVIAL)
    assert code == 0, "الترجمة الحرّة فشلت:\n" + out
    m = _define_re("memcmp").search(ir)
    assert m, "IR الحرّ بلا `define ... @memcmp(`:\n" + ir[:1500]
    line = ir[m.start():ir.find("\n", m.start())]
    assert "weak_odr" in line, "memcmp ليست weak_odr:\n" + line
    assert " i32 " in line, "memcmp لا تعيد i32 (عقد C: int memcmp):\n" + line


def test_no_self_recursion_memmove_memcmp():
    """(AR) كلّ رمز مرّة واحدة (التعريف) ⇒ لا نداء ذاتيّ من loop-idiom (لا تكرار لانهائيّ)."""
    code, out, ir = _compile_ir(_TRIVIAL)
    assert code == 0, out
    # (AR) في برنامج تافه لا مستدعٍ خارجيّ لهما ⇒ الظهور الوحيد هو سطر define.
    assert ir.count("@memmove") == 1, (
        "@memmove يظهر أكثر من مرّة — نداء ذاتيّ محتمل ولّده تمييز الحلقات "
        "(no-builtins يجب أن يمنعه) ⇒ خطر تكرار لانهائيّ"
    )
    assert ir.count("@memcmp") == 1, "@memcmp يظهر أكثر من مرّة — نداء ذاتيّ محتمل"


def test_memset_memcpy_still_emitted():
    """(AR) لا انحدار: memset/memcpy القائمتان ما زالتا مُصدَّرتين."""
    code, out, ir = _compile_ir(_TRIVIAL)
    assert code == 0, out
    assert _define_re("memcpy").search(ir), "انحدار: memcpy لم تعد مُصدَّرة"
    assert _define_re("memset").search(ir), "انحدار: memset لم تعد مُصدَّرة"
