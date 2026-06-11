"""
============================================================================
(AR) حارس انحدار (الطبقة 4 من خطة منع تسرّب placeholders):
     يُشغّل مقاطع .ص تُطلِق أخطاءً، ويتأكّد أن الإخراج (وضع الإنتاج) **لا يحوي
     أي placeholder حرفي غير مُستبدَل** مثل {container}/{index}/{param}.
     حاجز الطبقة 1 (ErrorCatalog::substitute) يُفرّغ المفقود؛ هذا الحارس يضمن
     عدم تسرّب جديد إن أُضيف موقع خطأ ينسى placeholder مطلوباً.
(EN) Regression guard (Layer 4 of the placeholder-leak plan): runs .ص snippets
     that raise errors and asserts the production output contains NO literal
     unreplaced placeholder like {container}/{index}/{param}.
============================================================================
"""
from __future__ import annotations

import os
import re
import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
_CANDIDATES = [
    ROOT / "build" / "bin" / "Debug" / "sad-run.exe",
    ROOT / "build" / "bin" / "sad-run.exe",
    ROOT / "build" / "bin" / "Release" / "sad-run.exe",
]
SAD_RUN = next((p for p in _CANDIDATES if p.exists()), None)

pytestmark = pytest.mark.skipif(
    SAD_RUN is None,
    reason="sad-run.exe غير مبني — يتطلب: cmake --build build --target sad-run",
)

# (AR) نمط placeholder حرفي مُتسرّب: {كلمة_إنجليزية} (لا يطابق محتوى عربياً أو أقواساً برمجية)
_LEAK = re.compile(r"\{[a-z_]{3,}\}")

# (AR) مقاطع تُطلِق أخطاءً وقت التشغيل في مسارات متنوّعة (يجب أن تُرندَر بلا تسرّب)
_SNIPPETS = {
    "index_out_of_range": "متغير ل = [1، 2]؛ اطبع_سطر(ل[10])\n",
    "index_assign": "متغير ل = [1، 2]؛ ل[9] = 3\n",
}


def _run(source: str) -> str:
    with tempfile.NamedTemporaryFile("w", suffix=".ص", delete=False, encoding="utf-8") as f:
        f.write(source)
        path = f.name
    try:
        # (AR) وضع الإنتاج صراحةً — بلا SAD_DEBUG_PLACEHOLDERS
        env = dict(os.environ)
        env.pop("SAD_DEBUG_PLACEHOLDERS", None)
        proc = subprocess.run(
            [str(SAD_RUN), path],
            capture_output=True, text=True, encoding="utf-8",
            errors="replace", timeout=30, env=env,
        )
        return (proc.stdout or "") + (proc.stderr or "")
    finally:
        os.unlink(path)


@pytest.mark.parametrize("name,src", list(_SNIPPETS.items()))
def test_no_unreplaced_placeholder_leaks(name, src):
    """(AR) لا placeholder حرفي ({key}) يتسرّب لإخراج المستخدم في وضع الإنتاج."""
    out = _run(src)
    leaks = _LEAK.findall(out)
    assert not leaks, (
        f"تسرّب placeholder حرفي في '{name}': {leaks}\n"
        f"السبب المرجَّح: موقع خطأ لا يمرّر placeholder يطلبه الـbrief.\n--- الإخراج ---\n{out}"
    )


def test_index_error_shows_container_data():
    """
    (AR) بعد الطبقة 3: خطأ الفهرس يُظهر **نوع الحاوية الفعلي** (عبر getTypeName، دقيق
         للمصفوفة/الصفوف/النص) لا فراغاً ولا {container} حرفياً.
    """
    out = _run(_SNIPPETS["index_out_of_range"])
    assert "{container}" not in out, f"تسرّب {{container}}:\n{out}"
    # (AR) getTypeName يُرجع الاسم الداخلي (ARRAY) — متّسق مع بقية رسائل الأخطاء
    assert "ARRAY" in out, (
        f"لم تُمرَّر بيانات الحاوية الفعلية (الطبقة 3، obj.getTypeName):\n{out}"
    )
