"""
============================================================================
(AR) اختبارات منع انحدار: **الوضع الحرّ لا يبعث رموزًا مستضافة**.

     العطب المقيس الذي ولّد هذه الاختبارات: `emitUtf8LocaleInit` كانت تبعث
     `call @setlocale(LC_ALL, ".UTF8")` في مقدّمة نقطة الدخول بلا فحص الوضع
     الحرّ. تعليلها سليم للمستضاف (دوالّ CRT الضيّقة تؤوّل `char*` بصفحة نظام
     محلّيّة فيفشل فتحُ مسارٍ عربيّ)، لكنّ الوضع الحرّ لا CRT فيه أصلًا:
     التهيئة بلا موضوع، و`setlocale` رمزٌ لا يعرّفه أحد ⇒ يُخفق الربط بـ
     `undefined symbol: setlocale` عند **كلّ** ثنائيّ حرّ. أوقف ذلك بوّابةَ
     إقلاع نواة النحلة كاملةً.

     ⚠️ التوكيد الأوّل **قائمة سماحٍ لا قائمة منع**: لو منعنا `setlocale`
     بالاسم لأمسكنا هذا العيب وحده وتركنا العائلة. أيّ `declare` في وحدةٍ
     حرّة يجب أن يكون تعليمةَ LLVM جوهريّة (`llvm.*`) أو رمزًا عرّفه المستخدم
     صراحةً — وما عداه رمزٌ مستضافٌ مسرَّب.

(EN) Regression tests: freestanding must not declare hosted symbols.
     Measured defect: the UTF-8 locale prologue emitted `setlocale` even under
     `--حرّ`, breaking the link of every freestanding binary. The first
     assertion is an ALLOW-list, not a deny-list, so it catches the whole
     family of future hosted-builtin leaks rather than this one symbol.
----------------------------------------------------------------------------
(AR) لماذا --أظهر-llvm؟ التسريب يعيش في إصدار LLVM قبل الربط؛ رصدُه هنا أرخص
     وأدقّ من انتظار `undefined symbol` من الرابط.
============================================================================
"""
from __future__ import annotations

import re
import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

# (AR) ⚠️ Release أوّلًا هنا خلافًا لبقيّة اختبارات هذا المجلّد: هذه الاختبارات
#      تحرس إصلاحًا حديثًا، وثنائيّ Debug بائتٌ يجعلها تسقط وإن كان المصدر
#      سليمًا — أي إخفاقٌ يشير إلى الاختبار لا إلى العيب.
# (AR) ومسارات بلا ‎.exe‎ للينكس/ماك: بدونها يبتلع ‎skipif‎ الملفَّ أخضرَ صامتًا.
_CANDIDATES = [
    ROOT / "build" / "bin" / "Release" / "sad-build.exe",
    ROOT / "build" / "bin" / "sad-build.exe",
    ROOT / "build" / "bin" / "Debug" / "sad-build.exe",
    ROOT / "build" / "bin" / "Release" / "sad-build",
    ROOT / "build" / "bin" / "sad-build",
    ROOT / "build" / "bin" / "Debug" / "sad-build",
]
SAD_BUILD = max(
    (p for p in _CANDIDATES if p.exists()),
    # (AR) أحدثُ الموجودَين لا أوّلُهما: قِيسَ ثنائيُّ Release عمرُه ١٣ يومًا
    #      يسبق Debug طازجًا في القائمة، فولّدت الأداةُ عطبًا وهميًّا.
    # (EN) Newest, not first: a 13-day-old Release binary preceded a fresh
    #      Debug one and the instrument fabricated a defect.
    key=lambda p: p.stat().st_mtime,
    default=None,
)

pytestmark = pytest.mark.skipif(
    SAD_BUILD is None,
    reason="sad-build.exe غير مبني — يتطلب: cmake --build build --target sad-build",
)

FREESTANDING = "--حرّ"
EMIT_LLVM = "--أظهر-llvm"
I686 = "--هدف=i686-unknown-none-elf"

# (AR) رموز يعرّفها المستخدم صراحةً في حالة الاختبار نفسها — لا تسريب.
_ALLOWED_EXTERNS: set[str] = set()


def _compile(source: str, *extra_flags: str) -> tuple[int, str, str]:
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(source, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), *extra_flags, EMIT_LLVM, "-o", str(out_ll)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=120, cwd=work,
        )
        ir = out_ll.read_text(encoding="utf-8", errors="replace") if out_ll.exists() else ""
        return proc.returncode, (proc.stdout or "") + (proc.stderr or ""), ir


def _declared_externs(ir: str) -> set[str]:
    """(AR) أسماء كلّ `declare` عدا تعليمات LLVM الجوهريّة."""
    names = set(re.findall(r"^declare[^@]*@\"?([\w.$\\]+)\"?\(", ir, re.M))
    return {n for n in names if not n.startswith("llvm.")}


# ─────────────── قائمة السماح: صفر رمزٍ مستضافٍ مسرَّب ───────────────

def test_freestanding_declares_no_hosted_symbol():
    """(AR) وحدة حرّة لا تُصرّح رمزًا خارجيًّا لم يطلبه المستخدم."""
    code, out, ir = _compile('اطبع("مرحبا")\n', FREESTANDING, I686)
    assert code == 0, "فشلت الترجمة الحرّة:\n" + out
    leaked = _declared_externs(ir) - _ALLOWED_EXTERNS
    assert not leaked, (
        "رموزٌ مستضافةٌ مسرَّبة في الوضع الحرّ — كلٌّ منها يُخفق الربط بـ"
        f"undefined symbol: {sorted(leaked)}\n" + ir[:2000]
    )


def test_freestanding_entry_has_no_locale_init():
    """(AR) حارس انحدار مرسًى: غلاف @main الحرّ = نداء __sad_main وحده."""
    code, out, ir = _compile('اطبع("مرحبا")\n', FREESTANDING, I686)
    assert code == 0, out
    assert "setlocale" not in ir, "عاد setlocale إلى الوضع الحرّ:\n" + ir[:2000]
    match = re.search(r"define i32 @main\(\) \{(.*?)\n\}", ir, re.S)
    assert match, "لا غلاف @main في المخرَج الحرّ:\n" + ir[:1500]
    body = match.group(1)
    assert body.count("call") == 1, (
        "غلاف main الحرّ ينادي أكثر من __sad_main:\n" + body
    )


# ─────────────── الوجه الآخر: المستضاف لا ينحدر ───────────────

def test_hosted_still_initialises_locale():
    """(AR) المستضاف يبقى مُهيِّئًا المحليّة — وإلّا انحدرت مسارات UTF-8 العربيّة.

    هذا التوكيد يمنع «الإصلاح» بالحذف المطلق بدل الحذف المشروط.
    """
    code, out, ir = _compile('اطبع("مرحبا")\n')
    assert code == 0, out
    assert "call ptr @setlocale" in ir, (
        "سقطت تهيئة المحليّة من المستضاف — انحدارٌ في فتح المسارات العربيّة:\n"
        + ir[:2000]
    )
