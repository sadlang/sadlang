"""
============================================================================
(AR) بوّابةُ معماريّةِ الهدف — الحكمُ بالسلوك لا بقراءة الجدول.

     `test_arch_specific_opcodes_sot.py` يحرس **الجدول** (لا اسمَ ميّتًا فيه
     ولا مُصدِرَ x86 خارجَه). وهذا الملفُّ يحرس **أثرَه**: أيرفض المترجمُ فعلًا
     مدمجةً خاصّةً بـx86 لهدفٍ غيرِ x86، ويقبله لهدفٍ من العائلة؟

     ولماذا الفصل: أوّلُ صياغةٍ للبوّابة كانت مبنيّةً كاملةً — جدولٌ صحيحٌ
     وحارسُ انجرافٍ أخضر — ولا تُطلَق **ولا مرّةً واحدة**، لأنّ المفتاحَ كان
     نصَّ `sirOpcodeToString` وهي تعيد المنمنمة (`add.i64`) لا اسمَ التعداد.
     ثمّ صارت تُطلَق على i686 خطأً، لأنّ `getArchName()` تعيد «i686» بينما
     الاسمُ القانونيُّ «i386». العطبان لا يراهما إلّا تشغيلُ المترجم.

     ⚠️ فالحالةُ الموجبة (i686 وx86_64 **يُقبَلان**) ليست زينة: هي التي تمسك
     بوّابةً تحرس أكثرَ ممّا ينبغي فتمنع المعماريّةَ التي تعمل عليها النواة.
============================================================================
"""
from __future__ import annotations

import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

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

_GEN_TIMEOUT_SECONDS = 120

# (AR) أهدافٌ داخلَ عائلة x86 — يجب أن تُقبَل. i686 أوّلًا: هي معماريّةُ النواة.
IN_FAMILY = ["i686-unknown-elf", "x86_64-unknown-elf"]
# (AR) أهدافٌ خارجَها — يجب أن تُرفَض بتشخيصٍ يسمّي التعليمةَ والعائلة.
OUT_OF_FAMILY = ["aarch64-unknown-elf", "riscv64-unknown-elf"]

# (AR) عيّنةٌ من عائلاتِ القيد الثلاث: عدّادُ دورات، منفذُ دخل/خرج، سجلُّ تحكّم.
_SOURCES = {
    "عداد_الدورات": ("متغير قيمة = عداد_الدورات()\nاطبع(قيمة)\n", "rdtsc"),
    "اكتب_منفذ": ("اكتب_منفذ(96، 255)\n", "outb"),
    "اقرأ_سجل_تحكم": ("متغير قيمة = اقرأ_سجل_تحكم(3)\nاطبع(قيمة)\n", "mov %cr"),
}


def _compile(source: str, target: str, freestanding: bool = True) -> tuple[int, str]:
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(source, encoding="utf-8")
        flags = (["--حرّ"] if freestanding else []) + [f"--هدف={target}"]
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), *flags, "--أظهر-llvm", "-o", str(out_ll)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=_GEN_TIMEOUT_SECONDS, cwd=work,
        )
        return proc.returncode, (proc.stdout or "") + (proc.stderr or "")


@pytest.mark.parametrize("target", OUT_OF_FAMILY)
@pytest.mark.parametrize("builtin", sorted(_SOURCES))
def test_x86_builtin_rejected_off_family(builtin, target):
    """(AR) مدمجةٌ خاصّةٌ بـx86 لهدفٍ غيرِ x86 ⇒ خروجٌ غيرُ صفريٍّ يسمّي التعليمة."""
    source, witness = _SOURCES[builtin]
    code, out = _compile(source, target)
    assert code != 0, (
        f"{builtin} نجحت على {target} — يُبَثُّ `{witness}` لمعماريّةٍ لا تعرفه، "
        f"فيخرج ثنائيٌّ لا يعمل أو يُخفِق المُجمِّعُ برسالةٍ لا تدلّ:\n{out}")
    assert witness in out, (
        f"{builtin} رُفضت على {target} لكنّ التشخيص لا يسمّي `{witness}` — "
        f"الرفضُ قد يكون لسببٍ آخر:\n{out}")


@pytest.mark.parametrize("target", IN_FAMILY)
@pytest.mark.parametrize("builtin", sorted(_SOURCES))
def test_x86_builtin_accepted_in_family(builtin, target):
    """(AR) والحالةُ الموجبة: البوّابةُ لا تحرس أكثرَ ممّا ينبغي.

    i686 هنا ليست حالةً عابرة: هي معماريّةُ نواة النحلة اليوم، ورفضُها
    يوقف المشروعَ كلَّه.
    """
    source, _witness = _SOURCES[builtin]
    code, out = _compile(source, target)
    assert code == 0, (
        f"{builtin} رُفضت على {target} وهي داخلَ عائلة x86 — "
        f"البوّابةُ تحرس أكثرَ ممّا ينبغي:\n{out}")


# ─────────── المستضاف: القيدُ معماريٌّ لا خاصٌّ بالوضع الحرّ ───────────

@pytest.mark.parametrize("builtin", sorted(_SOURCES))
def test_x86_builtin_rejected_off_family_hosted(builtin):
    """(AR) والمستضافُ كذلك: التعليمةُ غائبةٌ عن **المعماريّة** لا عن المكتبة.

    قِيس قبل إدراج الرمز في بوّابة السائق المستضافة: هدفٌ مستضافٌ على aarch64
    يطبع الخطأَ **ويخرج بصفر** ويكتب ملفَّ الخرج — فبوّابةُ الوضعِ الحرِّ في
    السائق مقصورةٌ على `options_.freestanding` ولا تبلغ هذا المسار.
    """
    source, witness = _SOURCES[builtin]
    code, out = _compile(source, "aarch64-unknown-linux-gnu", freestanding=False)
    assert code != 0, (
        f"{builtin} خرجت بصفر مستضافةً على aarch64 رغم بثّ `{witness}` — "
        f"البوّابةُ تُبلِّغ ولا تُحبِط:\n{out}")
    assert witness in out, (
        f"{builtin} رُفضت مستضافةً لكنّ التشخيص لا يسمّي `{witness}`:\n{out}")


def test_no_internal_error_follows_the_diagnostic():
    """(AR) البلاغُ الحقيقيُّ وحدَه — لا «خطأ مترجم داخليّ» يتلوه.

    الرجوعُ من البوّابة بلا ربطِ الناتج كان يترك سجلَّ النتيجة غيرَ معرَّفٍ،
    فيتلو التشخيصَ الصحيحَ بلاغُ علّةٍ يطلب من المستعمل الإبلاغَ عن عيبٍ
    لا وجودَ له.
    """
    source, _witness = _SOURCES["عداد_الدورات"]
    _code, out = _compile(source, "aarch64-unknown-elf")
    assert "Undefined register" not in out and "مرجع غير معرَّف" not in out, (
        "بلاغُ «مرجع غير معرَّف» يتلو تشخيصَ البوّابة — الناتجُ غيرُ مربوط:\n" + out)
