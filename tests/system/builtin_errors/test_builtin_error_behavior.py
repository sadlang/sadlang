"""
============================================================================
(AR) اختبارات سلوكية end-to-end لترحيل أخطاء الدوال المضمنة (EM-CPP).
     تشغّل لقطات .ص عبر المفسر (sad-run) وتتحقّق أن أخطاء الدوال المضمنة
     تُعرَض من الكتالوج المُولَّد (رمز [RUN0XX] + اسم الدالة) — لا رسائل خام.
(EN) End-to-end behavioral tests for the builtin-error migration (EM-CPP).
     Runs .ص snippets through the interpreter and asserts builtin errors render
     from the generated catalog (code [RUN0XX] + function name), not raw strings.
----------------------------------------------------------------------------
(AR) يُثبت: المسار shared/builtins → BuiltinError → callNative → throwRuntime →
     الكتالوج. الدوال الأساسية (طول/أضف) قابلة للوصول (غير مُبوّبة).
============================================================================
"""
from __future__ import annotations

import os
import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

# (AR) مواقع محتملة لـ sad-run.exe (Debug build)
_CANDIDATES = [
    ROOT / "build" / "bin" / "Debug" / "sad-run.exe",
    ROOT / "build" / "bin" / "sad-run.exe",
    ROOT / "build" / "bin" / "Release" / "sad-run.exe",
]
SAD_RUN = max(
    (p for p in _CANDIDATES if p.exists()),
    # (AR) أحدثُ الموجودَين لا أوّلُهما: قِيسَ ثنائيُّ Release عمرُه ١٣ يومًا
    #      يسبق Debug طازجًا في القائمة، فولّدت الأداةُ عطبًا وهميًّا.
    # (EN) Newest, not first: a 13-day-old Release binary preceded a fresh
    #      Debug one and the instrument fabricated a defect.
    key=lambda p: p.stat().st_mtime,
    default=None,
)

pytestmark = pytest.mark.skipif(
    SAD_RUN is None,
    reason="sad-run.exe غير مبني — يتطلب: cmake --build build --target sad-run",
)


def _run(source: str) -> str:
    """(AR) يكتب لقطة .ص ويشغّلها، ويعيد stdout+stderr مجمَّعين."""
    with tempfile.NamedTemporaryFile(
        "w", suffix=".ص", delete=False, encoding="utf-8"
    ) as f:
        f.write(source)
        path = f.name
    try:
        proc = subprocess.run(
            [str(SAD_RUN), path],
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=30,
        )
        return (proc.stdout or "") + (proc.stderr or "")
    finally:
        os.unlink(path)


def test_length_no_args_renders_run037_with_name():
    """(AR) طول() بلا وسائط → [RUN037] من الكتالوج مع اسم الدالة 'طول'."""
    out = _run("طول()\n")
    assert "[RUN037]" in out, f"رمز الخطأ المتوقّع [RUN037] غائب:\n{out}"
    assert "طول" in out, f"اسم الدالة 'طول' غائب عن الرسالة:\n{out}"
    # (AR) تأكيد أنها رسالة الكتالوج لا رسالة خام قديمة
    assert "الدالة المدمجة" in out, f"رسالة الكتالوج غائبة:\n{out}"


def test_no_raw_builtin_messages_leak():
    """(AR) لا تتسرّب رسائل خام قديمة (مثل 'requires at least one argument')."""
    out = _run("طول()\n")
    assert "requires at least one argument" not in out, (
        "رسالة خام قديمة تسرّبت — يجب أن تأتي من الكتالوج:\n" + out
    )


def test_imported_module_function_is_callable():
    """
    (AR) حارس: استيراد وحدة مضمنة ثم استدعاء دالة منها يعمل (بالاسم المُسجَّل الصحيح).
         يحرس ضد تشخيص 'علة الاستيراد' الخاطئ — الاستيراد سليم؛ كان الخطأ في أسماء الاختبار.
         لوغ = Bm::LOG (اللوغاريتم الطبيعي)؛ لوغ(e) ≈ 1.
    (EN) Guard: importing a builtin module then calling its function works (correct
         registered name). Refutes the false 'import bug' diagnosis.
    """
    out = _run('استورد رياضيات\nاطبع_سطر(لوغ(2.718))\n')
    assert "0.99" in out or "1.0" in out or "1\n" in out, (
        "لوغ(2.718) لم يُرجع ~1 بعد استورد رياضيات — الاستيراد/الإرسال معطوب:\n" + out
    )
    assert "SEM004" not in out, f"SEM004 — الدالة غير موجودة بعد الاستيراد:\n{out}"
