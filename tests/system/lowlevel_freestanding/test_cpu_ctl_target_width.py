"""
============================================================================
(AR) حارسُ عرضِ سجلّاتِ التحكّم: عرضُ السجلّ يُشتقُّ من **الهدف** لا من **الوضع**.

     العطبُ المقيس الذي ولّد هذا الملفّ: خفضُ `اقرأ_سجل_تحكم` و`اكتب_سجل_تحكم`
     و`افرغ_ذاكرة_ترجمة` و`ابطل_صفحة` و`حمل_جدول_*` كان يتفرّع على
     `freestanding_` — وهو **وضعٌ** (بلا libc) — ثمّ يثبّت عرضَ i686 (٣٢ بتًّا)
     في نصّ الأسمبلي. فبُنيت نواةُ النحلة (٥٩٬٠٤٧ سطرًا) بـ`--هدف=x86_64`:
     الواجهةُ الأماميّةُ ولّدت IR كاملًا **بصفر خطأٍ وصفر تحذير**، ثمّ أخفق
     المُجمِّعُ بـ**١٩ خطأً كلُّها `cannot compile inline asm`**، من ثلاث تعليمات
     لا غير. والعددان مختلفان عمدًا: ما في الـIR **٢٥ ظهورًا** — `mov %cr3, %eax`
     (٢٢) و`mov %cr2, %eax` (٢) و`mov %cr0, %eax` (١) — وما ردَّه المُجمِّعُ ١٩
     خطأً — عددان لمقياسين لا لمقياسٍ واحد، وسببُ الفارق غيرُ مقيسٍ فلا يُدَّعى.
     ولا يكتبها أحدٌ في النواة، بل يولّدها المترجمُ نفسُه.

     **مدى هذا الحارس**: عرضُ السجلّ في الأسمبلي المولَّد. لا يشمل **شكلَ**
     واصفِ الجدول نفسِه (٢ بايت حدٌّ + ٤ بايت قاعدةٍ في الوضع المحميّ مقابل ٨ في
     الوضع الطويل) — تلك بياناتٌ يبنيها مصدرُ النواة بلغة ص لا يمسُّها المترجم.
     فاجتيازُ هذا الملفّ على x86_64 يعني «الأسمبليُّ صحيحُ العرض»، لا «النواةُ
     جاهزةٌ للوضع الطويل».

     ⚠️ ولماذا يُجمَّع الناتجُ هنا ولا يُكتفى بفحص نصّ IR: الحارسُ القائمُ
     (`test_cpu_ctl_i686.py`) يفحص نصَّ IR بـ`--أظهر-llvm` ولا يمرّ بالمُجمِّع
     قطّ، **ولا يمرّر `--هدف` أصلًا** فيقع على ثالوث المضيف. فكان يرى
     `mov %cr0, reg32` على هدفٍ ٦٤‑بتّيّ **ويُصدّقه**: أسمبليٌّ لا يقبله مُجمِّعٌ
     يجتاز حارسًا يقرأ النصَّ ولا يجمّعه. فالحكمُ هنا على مخرَجِ المُجمِّع.

(EN) Control-register width must follow the target, not the freestanding mode.
     The prior guard checked IR text only, never assembled, and never passed a
     target — so `mov %cr0, reg32` on a 64-bit target passed it. This one
     assembles.
============================================================================
"""
from __future__ import annotations

import os
import re
import shutil
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
SAD_BUILD = next((p for p in _CANDIDATES if p.exists()), None)

pytestmark = pytest.mark.skipif(
    SAD_BUILD is None,
    reason="sad-build.exe غير مبني — يتطلب: cmake --build build --target sad-build",
)

FREESTANDING = "--حرّ"
EMIT_LLVM = "--أظهر-llvm"

# (AR) مُهلةُ التوليد كمُهلةِ أخواتِ هذا المجلّد. ومُهلةُ التجميع أطولُ لأنّها
#      تضيف مرورَ clang كاملًا على IR فوق زمنِ التوليد.
_GEN_TIMEOUT_SECONDS = 120
_ASM_TIMEOUT_SECONDS = 180

# (AR) ثواليثُ مجرّدةٌ لا رايات — أخواتُ هذا الملفّ تسمّي الرايةَ TARGET_*،
#      فاسمُ TRIPLE_* هنا يمنع نسخَ سطرٍ ينتج «--هدف=--هدف=…».
# (AR) الهدفان اللذان يبني عليهما مشروعُ النحلة: القائمُ (i686) والمقصود (x86_64).
TRIPLE_I686 = "i686-unknown-elf"
TRIPLE_X86_64 = "x86_64-unknown-elf"
# (AR) x32: سجلّاتٌ ٦٤‑بتّيّةٌ بمؤشّراتٍ ٣٢‑بتّيّة — يفصل «عرضَ السجلّ» عن «عرضِ المؤشّر».
TRIPLE_X32 = "x86_64-unknown-linux-gnux32"

# (AR) مصادرُ ص تُمارِس كلَّ مدمَجٍ يُصدر سجلَّ تحكّمٍ أو عنوانَ صفحةٍ أو واصفَ جدول.
_SOURCES = {
    "اقرأ_سجل_تحكم": "متغير قيمة = اقرأ_سجل_تحكم(3)\nاطبع(قيمة)\n",
    "اكتب_سجل_تحكم": "اكتب_سجل_تحكم(3، 305419896)\n",
    "افرغ_ذاكرة_ترجمة": "افرغ_ذاكرة_ترجمة()\n",
    "ابطل_صفحة": "ابطل_صفحة(1048576)\n",
    "حمل_جدول_واصفات": "حمل_جدول_واصفات(1048576)\n",
    "حمل_جدول_مقاطعات": "حمل_جدول_مقاطعات(1048576)\n",
}

# (AR) مدمَجاتٌ تدخل الحكمَ الحاسمَ (التجميع) **دون** فحوصِ العرض أعلاه.
#      ولماذا الفصل: هذه تُصدِر `rdtsc`/`rdmsr`، وناتجُهما محفورٌ في العتاد في
#      `edx:eax` على المعماريّتين معًا — فـ`%eax` هنا صوابٌ لا عطب، وفحصُ
#      «لا سجلَّ ٣٢‑بتّيًّا على هدفٍ ٦٤‑بتّيّ» كان ليدينَ الصوابَ.
#      وما يلزمها فعلًا هو المُجمِّع: الصيغةُ السابقةُ لـ`rdtsc` كانت
#      `={rax}` + `shl $32, %rdx`، فيردّها مُجمِّعُ i686 — معماريّةِ النواة —
#      بـ«couldn't allocate output register for constraint '{rax}'»، وتمرّ
#      مع ذلك في كلّ فحصٍ يقرأ نصَّ IR. فبلا هذه الحالة يبقى نصفُ الرقعة
#      بلا بذرةٍ تُخفِق.
_ASSEMBLE_ONLY_SOURCES = {
    "عداد_الدورات": "متغير دورات = عداد_الدورات()\nاطبع(دورات)\n",
    "مؤقت_انتظر": "مؤقت_انتظر(1000)\n",
    "اقرأ_سجل_نموذج": "متغير سجل = اقرأ_سجل_نموذج(174)\nاطبع(سجل)\n",
}


def _run_sad_build(source: str, target: str, work: Path, out_ll: Path,
                   freestanding: bool = True) -> subprocess.CompletedProcess:
    """(AR) نداءُ sad-build الوحيدُ في هذا الملفّ.

    موحَّدٌ عمدًا: الحكمُ الحاسمُ (التجميع) كان يعيد بناءَ قائمة الوسائط بنفسه،
    فأيُّ رايةٍ تُضاف لاحقًا تُنسى في أحد الموضعين ⇒ يقيس الحكمُ الحاسمُ شيئًا
    غيرَ الذي يقيسه بقيّةُ الملفّ.
    """
    src = work / "t.ص"
    src.write_text(source, encoding="utf-8")
    flags = ([FREESTANDING] if freestanding else []) + [f"--هدف={target}"]
    return subprocess.run(
        [str(SAD_BUILD), str(src), *flags, EMIT_LLVM, "-o", str(out_ll)],
        capture_output=True, text=True, encoding="utf-8", errors="replace",
        timeout=_GEN_TIMEOUT_SECONDS, cwd=work,
    )


def _emit_ir(source: str, target: str, freestanding: bool = True) -> tuple[int, str, str]:
    """(AR) يترجم المصدر للهدف المعطى؛ يعيد (رمز الخروج، الخرج، نصّ IR)."""
    with tempfile.TemporaryDirectory() as work:
        out_ll = Path(work) / "t.ll"
        proc = _run_sad_build(source, target, Path(work), out_ll, freestanding)
        ir = out_ll.read_text(encoding="utf-8", errors="replace") if out_ll.exists() else ""
        return proc.returncode, (proc.stdout or "") + (proc.stderr or ""), ir


# (AR) المنمنماتُ التي يحمل معاملُها عرضَ سجلّ: `%cr` يغطّي قراءةَ سجلّ التحكّم
#      وكتابتَه وإفراغَ ذاكرة الترجمة معًا.
_MNEMONICS = ("%cr", "lgdt", "lidt", "invlpg")

# (AR) أسماءُ السجلّات في نصّ الأسمبلي: %eax… ٣٢‑بتّيّة، %rax… ٦٤‑بتّيّة.
#      لا تلتقط %cr3 (بادئةُ cr) ولا %rip.
_REG32 = re.compile(r'%e(?:ax|bx|cx|dx|si|di|bp|sp)\b')
_REG64 = re.compile(r'%r(?:ax|bx|cx|dx|si|di|bp|sp)\b')


def _asm_ops(ir: str) -> list[tuple[str, str, list[str]]]:
    """(AR) كلُّ `asm` يحوي إحدى المنمنمات: (نصُّ الأسمبلي، نوعُ الإرجاع، أنواعُ الوسائط).

    ⚠️ يُعاد **نصُّ الأسمبلي** لا الأنواعُ وحدَها: صيغةُ إفراغِ ذاكرة الترجمة
    العاطبةُ (`mov %cr3, %eax; mov %eax, %cr3`) تحمل المركمَ حرفيًّا في النصّ
    ولا معاملَ لها ولا نوعَ إرجاعٍ إلّا `void`. فحارسٌ يقرأ الأنواعَ وحدَها
    يجتازها قبل الإصلاح وبعده سواءً بسواء — ويكون تأكيدًا بصفرِ شاهد.
    """
    ops: list[tuple[str, str, list[str]]] = []
    pattern = re.compile(
        r'call\s+(\S+)\s+asm[^"]*"([^"]*)"\s*,\s*"[^"]*"\s*\(([^)]*)\)')
    for ret_ty, body, args in pattern.findall(ir):
        if not any(m in body for m in _MNEMONICS):
            continue
        arg_types = re.findall(r'\b(i\d+)\b', args)
        ops.append((body, ret_ty, arg_types))
    return ops


def _widths(ops: list[tuple[str, str, list[str]]]) -> list[str]:
    """(AR) أنواعُ المعاملات المحمولة، بلا `void` — فالفراغُ يبقى فراغًا يُكشَف."""
    out: list[str] = []
    for _body, ret_ty, arg_types in ops:
        if ret_ty != "void":
            out.append(ret_ty)
        out.extend(arg_types)
    return out


@pytest.mark.parametrize("builtin, source", sorted(_SOURCES.items()))
def test_register_width_follows_target_i686(builtin, source):
    """(AR) على i686: لا معاملَ i64 ولا سجلَّ ٦٤‑بتّيًّا في نصّ الأسمبلي."""
    code, out, ir = _emit_ir(source, TRIPLE_I686)
    assert code == 0, f"{builtin} أخفقت حرًّا على {TRIPLE_I686}:\n{out}"
    ops = _asm_ops(ir)
    assert ops, f"{builtin}: لا أسمبليَّ مولَّدًا على {TRIPLE_I686} — تأكيدٌ بصفرِ شاهد:\n{ir[:2000]}"
    assert "i64" not in _widths(ops), (
        f"{builtin}: معاملٌ بعرض ٦٤ بتًّا على هدفٍ ٣٢‑بتّيّ:\n{ir[:2000]}")
    for body, _ret, _args in ops:
        assert not _REG64.search(body), (
            f"{builtin}: سجلٌّ ٦٤‑بتّيٌّ مثبَّتٌ في نصّ الأسمبلي على i686: {body!r}")


@pytest.mark.parametrize("builtin, source", sorted(_SOURCES.items()))
def test_register_width_follows_target_x86_64(builtin, source):
    """(AR) على x86_64: لا معاملَ i32 ولا سجلَّ ٣٢‑بتّيًّا — عينُ العطب المقيس.

    الشقُّ الثاني (نصُّ الأسمبلي) هو الذي يمسك `mov %cr3, %eax` — أكثرَ التعليمات
    المُخفِقة تكرارًا في النواة (٢٢ ظهورًا)، ولا تحمل معاملًا مكتوبَ النوع أصلًا
    فلا يراها فحصُ الأنواع.
    """
    code, out, ir = _emit_ir(source, TRIPLE_X86_64)
    assert code == 0, f"{builtin} أخفقت حرًّا على {TRIPLE_X86_64}:\n{out}"
    ops = _asm_ops(ir)
    assert ops, f"{builtin}: لا أسمبليَّ مولَّدًا على {TRIPLE_X86_64} — تأكيدٌ بصفرِ شاهد:\n{ir[:2000]}"
    assert "i32" not in _widths(ops), (
        f"{builtin}: معاملٌ بعرض ٣٢ بتًّا على هدفٍ ٦٤‑بتّيّ — "
        f"هذا `mov %crN, reg32` الذي يرفضه المُجمِّع:\n{ir[:2000]}")
    for body, _ret, _args in ops:
        assert not _REG32.search(body), (
            f"{builtin}: سجلٌّ ٣٢‑بتّيٌّ مثبَّتٌ في نصّ الأسمبلي على هدفٍ ٦٤‑بتّيّ — "
            f"هذا هو `cannot compile inline asm` بعينه: {body!r}")


# ─────────── x32: عرضُ المؤشّر ليس عرضَ السجلّ ───────────

def test_x32_keeps_64bit_registers():
    """(AR) على `gnux32` المؤشّرُ ٣٢ بتًّا والسجلّاتُ ٦٤ — فالعرضُ من المعماريّة.

    لو اشتُقّ العرضُ من تخطيط البيانات (‎p:32:32‎) لعاد i32 ولردّ المُجمِّعُ
    «instruction requires: Not 64-bit mode»: العطبُ نفسُه بمحكٍّ أدقَّ وما زال خاطئًا.
    """
    code, out, ir = _emit_ir("".join(_SOURCES.values()), TRIPLE_X32)
    assert code == 0, f"التوليد أخفق على x32:\n{out}"
    ops = _asm_ops(ir)
    assert ops, f"لا أسمبليَّ مولَّدًا على x32 — تأكيدٌ بصفرِ شاهد:\n{ir[:2000]}"
    assert "i32" not in _widths(ops), (
        f"معاملٌ بعرض ٣٢ بتًّا على x32 — العرضُ اشتُقّ من المؤشّر لا من السجلّ:\n{ir[:2000]}")
    for body, _ret, _args in ops:
        assert not _REG32.search(body), (
            f"سجلٌّ ٣٢‑بتّيٌّ مثبَّتٌ في نصّ الأسمبلي على x32: {body!r}")


# ─────────── المسارُ المستضاف: تغيّر عمدًا فيلزمه حارس ───────────

def test_hosted_flush_tlb_is_register_constrained():
    """(AR) `افرغ_ذاكرة_ترجمة` مستضافةً: قيدُ سجلٍّ لا مركمٌ حرفيّ.

    وُحِّد مسارُها المستضافُ مع الحرّ في الرقعة نفسِها (كان `movq %cr3, %rax`
    بقيد `~{rax}` مثبَّتِ المعماريّة). فالتغييرُ مقصودٌ، وهذا حارسُه: بلا هذه
    الحالةِ يبقى المسارُ المستضافُ بلا حمرةٍ ممكنة.
    """
    code, out, ir = _emit_ir("افرغ_ذاكرة_ترجمة()\n", TRIPLE_X86_64, freestanding=False)
    assert code == 0, f"افرغ_ذاكرة_ترجمة أخفقت مستضافةً:\n{out}"
    ops = _asm_ops(ir)
    assert ops, f"لا أسمبليَّ مولَّدًا مستضافًا — تأكيدٌ بصفرِ شاهد:\n{ir[:2000]}"
    for body, _ret, _args in ops:
        assert not _REG64.search(body) and not _REG32.search(body), (
            f"مركمٌ مثبَّتٌ في المسار المستضاف بدل قيدِ سجلّ: {body!r}")
    assert "%cr3" in "".join(body for body, _r, _a in ops), (
        f"المسارُ المستضافُ فقد كتابةَ %cr3 — لم يعد يُفرِغ ذاكرةَ الترجمة:\n{ir[:2000]}")


# ─────────── الحكمُ الحاسم: يُجمَّع الناتجُ فعلًا، لا يُقرَأ نصُّه فقط ───────────

# (AR) المُجمِّع: «SAD_CLANG» أوّلًا، ثمّ المسار، ثمّ مواضعُ LLVM المعروفةُ عندنا.
#      وهذه الحالةُ هي **الحكمُ الحاسمُ الوحيد** في الملفّ (البقيّةُ تقرأ نصًّا)،
#      فتخطّيها يُفرِغ الملفَّ من برهانه ويبقيه أخضر. لذا:
#      «SAD_REQUIRE_CLANG=1» يقلب التخطّيَ إخفاقًا؛ الحارسُ نفسُه في conftest.py
#      بنمطِ نظيرِه `SAD_LOWLEVEL_REQUIRE_BUILD` — ونصُّ سببِ التخطّي أدناه هو
#      شاهدُه، فلا يُغيَّر بلا تغييرِه هناك.
def _find_clang() -> str | None:
    override = os.environ.get("SAD_CLANG")
    if override and Path(override).exists():
        return override
    return shutil.which("clang") or shutil.which("clang.exe")


CLANG = _find_clang()

@pytest.mark.skipif(
    CLANG is None,
    reason="clang غير موجود (لا في «SAD_CLANG» ولا في المسار) — لا مُجمِّعَ للحكم",
)
@pytest.mark.parametrize("target", [TRIPLE_I686, TRIPLE_X86_64, TRIPLE_X32])
def test_control_register_asm_actually_assembles(target):
    """(AR) كلُّ المدمَجات معًا تُجمَّع للأهدافِ الثلاثة — الشاهدُ مخرَجُ المُجمِّع لا نصُّ IR.

    هذه هي البذرةُ التي كانت تُخفِق بطرفَيها: على x86_64 كان المُجمِّعُ يردّ
    `cannot compile inline asm` لأنّ المُصدِرَ ثبّت `%eax` لهدفٍ ٦٤‑بتّيّ؛
    وعلى i686 كان يردّ `rdtsc` بصيغةِ `={rax}` لأنّ لا سجلَّ ٦٤‑بتّيًّا هناك.
    """
    source = "".join((*_SOURCES.values(), *_ASSEMBLE_ONLY_SOURCES.values()))
    with tempfile.TemporaryDirectory() as work:
        out_ll = Path(work) / "t.ll"
        out_obj = Path(work) / "t.o"
        gen = _run_sad_build(source, target, Path(work), out_ll)
        assert gen.returncode == 0, (
            f"توليدُ IR أخفق على {target}:\n{gen.stdout}{gen.stderr}")
        asm = subprocess.run(
            [CLANG, "-target", target, "-ffreestanding", "-nostdlib",
             "-c", str(out_ll), "-o", str(out_obj)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=_ASM_TIMEOUT_SECONDS, cwd=work,
        )
        diagnostics = (asm.stdout or "") + (asm.stderr or "")
        assert "cannot compile inline asm" not in diagnostics, (
            f"المُجمِّعُ رفض أسمبليًّا مولَّدًا على {target} — "
            f"عرضُ السجلّ لا يتبع الهدف:\n{diagnostics[:3000]}"
        )
        assert asm.returncode == 0, f"التجميع أخفق على {target}:\n{diagnostics[:3000]}"
        assert out_obj.exists(), f"لا ملفَّ كائنًا على {target} رغم خروجٍ صفريّ"
