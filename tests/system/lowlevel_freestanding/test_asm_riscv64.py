"""
============================================================================
(AR) المعماريّة الرابعة `riscv64` — واختبارُ الدعوى لا إعلانُها.

     الدعوى: «معماريّةٌ جديدةٌ لا تحتاج تعديلَ C++، ملفُّ YAML وحدَه». وهذه
     تُقاس بأن يُصدِر الخافضُ **نكهةً لم يرَها من قبل**، لا بأن يُبنى المشروع.

     🔑 ولماذا RISC-V بالذات: نكهتُها **تركيبةٌ لم تظهر في الثلاثة السابقة**:
       • الوجهة أوّلًا وصريحة  — كـAArch64 لا كـAT&T
       • العنونة `8(x1)`       — كـAT&T لا كـAArch64
       • ولا بادئة لسجلّ ولا لثابت — لا `%` ولا `$` ولا `#`
     فلو كان المحوران مقترنَين في الخافض (وجهة-أوّلًا ⇒ أقواس مربّعة) لظهر
     هنا حتمًا. والاختبار الحاسم هو **٤**: المصدر نفسه لهدفين ⇒ نصّان مختلفان.

     ⚠️ ولا يُقاس هذا في مصفوفة القواعد: لا مضيفَ riscv64 في CI، فاختبارٌ
     سلوكيٌّ هناك يُتخطّى دائمًا ولا يقيس شيئًا. والمقيس هنا **النصّ المُصدَر**
     في IR، وهو الطبقة التي تسكنها النكهة — ويُقاس من أيّ مضيف.
(EN) The fourth architecture, riscv64: a claim measured, not announced. Its
     flavour is a combination none of the first three had (dest-first like
     AArch64, `8(x1)` addressing like AT&T, and no prefixes at all), so a
     lowering that coupled the two axes would break here. Test 4 is decisive:
     one source, two targets, two different emitted texts. Measured on the IR
     text because CI has no riscv64 host.
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
RISCV64 = "--هدف=riscv64-unknown-elf"
AARCH64 = "--هدف=aarch64-unknown-elf"

# (AR) نصّ القالب المضمَّن وحدَه — لا الـIR كلّه: الـIR مليء بـ`%` لأسماء SSA،
#      فالحكم عليه كلِّه يخلط بادئةَ سجلٍّ في المُجمِّع باسمِ سجلٍّ في IR.
# (EN) Judge the inline-asm template only: IR is full of % for SSA names.
_قالب = re.compile(r'asm sideeffect[^"]*"([^"]*)"')


def _ترجم(source: str, *flags: str) -> tuple[int, str, str]:
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(source, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), FREESTANDING, *flags, EMIT_LLVM, "-o", str(out_ll)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=120, cwd=work,
        )
        ir = out_ll.read_text(encoding="utf-8", errors="replace") if out_ll.exists() else ""
        return proc.returncode, (proc.stdout or "") + (proc.stderr or ""), ir


def _قوالب(ir: str) -> list[str]:
    return _قالب.findall(ir)


# (AR) كتل منفصلة: م١ تدعم متغيّر خرج واحدًا لكلّ كتلة (SEM028).
_المصدر = (
    "دالة اقرأ_وأجمع()\n"
    "    متغير أ = 5\n"
    "    متغير ب = 3\n"
    "    متغير قيمة = 0\n"
    "    متغير نواة = 0\n"
    "    تجميع\n"
    "        اجمع {أ}، {ب}\n"
    "    نهاية\n"
    "    تجميع\n"
    "        حمّل {قيمة}، [{ب} + 8]\n"
    "    نهاية\n"
    "    تجميع\n"
    "        اضبط_جدول_المتّجهات {أ}\n"
    "    نهاية\n"
    "    تجميع\n"
    "        اقرأ_رقم_النواة {نواة}\n"
    "    نهاية\n"
    "نهاية\n"
)


def test_dest_is_repeated_without_any_prefix():
    """(AR) «add $0, $0, $2» — وجهة مكرَّرة (explicit) وبلا بادئة سجلّ."""
    code, out, ir = _ترجم(_المصدر, RISCV64)
    assert code == 0, "ترجمة riscv64 الحرّة فشلت:\n" + out
    قوالب = _قوالب(ir)
    assert any(re.fullmatch(r"add \$\d+, \$\d+, \$\d+", t) for t in قوالب), (
        "لا قالب «add $N, $N, $M» — الوجهة لم تُكرَّر أو تسرّبت بادئة:\n"
        + "\n".join(قوالب)
    )


def test_memory_is_paren_form_not_brackets():
    """(AR) 🔑 «8($N)» لا «[$N, 8]» — عنونة AT&T مع ترتيب AArch64 معًا.

    هذه هي التركيبة التي تفصل محورَي النكهة: لو كان الخافض يستنتج شكل العنونة
    من ترتيب المعاملات لأصدر أقواسًا مربّعة هنا.
    """
    code, out, ir = _ترجم(_المصدر, RISCV64)
    assert code == 0, out
    قوالب = _قوالب(ir)
    مطابق = [t for t in قوالب if t.startswith("ld ")]
    assert مطابق, "لا قالب «ld»:\n" + "\n".join(قوالب)
    assert any("(" in t and "[" not in t for t in مطابق), (
        "عنونة riscv64 صدرت بأقواس مربّعة — المحوران مقترنان في الخافض:\n"
        + "\n".join(مطابق)
    )


def test_head_and_tail_on_a_second_architecture():
    """(AR) الرأس والذيل يعملان هنا كذلك: «csrw mtvec, $N» و«csrr $N, mhartid»."""
    code, out, ir = _ترجم(_المصدر, RISCV64)
    assert code == 0, out
    قوالب = _قوالب(ir)
    assert any(re.fullmatch(r"csrw mtvec, \$\d+", t) for t in قوالب), (
        "لا «csrw mtvec, $N» — الرأس لم يُصدَر قبل المعامل:\n" + "\n".join(قوالب)
    )
    assert any(re.fullmatch(r"csrr \$\d+, mhartid", t) for t in قوالب), (
        "لا «csrr $N, mhartid» — الذيل لم يُصدَر بعد المعامل:\n" + "\n".join(قوالب)
    )


def test_same_source_two_targets_two_texts():
    """(AR) 🔑 الحاسم: المصدر نفسه لهدفين ⇒ نصّان مختلفان.

    لو كان الخافض يقرأ نكهةً واحدةً مبثوثةً فيه لخرج النصّان متطابقَين، ولكان
    أحد الهدفين يتلقّى نصًّا يرفضه مُجمِّعُه — أو أسوأ: يقبله بدلالة أخرى.
    """
    مصدر = (
        "دالة انقل_من_ذاكرة()\n"
        "    متغير ب = 3\n"
        "    متغير قيمة = 0\n"
        "    تجميع\n"
        "        حمّل {قيمة}، [{ب} + 8]\n"
        "    نهاية\n"
        "نهاية\n"
    )
    رمز_ر, خرج_ر, ir_ر = _ترجم(مصدر, RISCV64)
    رمز_أ, خرج_أ, ir_أ = _ترجم(مصدر, AARCH64)
    assert رمز_ر == 0, "riscv64:\n" + خرج_ر
    assert رمز_أ == 0, "aarch64:\n" + خرج_أ
    قوالب_ر = [t for t in _قوالب(ir_ر) if t.startswith("ld")]
    قوالب_أ = [t for t in _قوالب(ir_أ) if t.startswith("ldr")]
    assert قوالب_ر and قوالب_أ, (
        "قالب حمل مفقود — riscv64: %r · aarch64: %r" % (قوالب_ر, قوالب_أ)
    )
    assert قوالب_ر[0] != قوالب_أ[0], (
        "الهدفان أعطيا النصَّ نفسَه (%r) — النكهة ليست مقروءةً من مصدر الحقيقة"
        % قوالب_ر[0]
    )
    assert "[" in قوالب_أ[0] and "[" not in قوالب_ر[0], (
        "شكل العنونة لم يفترق: aarch64=%r · riscv64=%r" % (قوالب_أ[0], قوالب_ر[0])
    )
