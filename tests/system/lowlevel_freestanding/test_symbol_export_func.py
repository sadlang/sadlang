"""
============================================================================
(AR) اختبارات منع انحدار للّبنة 3.17: «@رمز على الدوالّ» — تصدير دالّة ص تحت
     اسم رمز ASCII بربط خارجيّ، ليستدعيها كود C/المترجم بالاسم حرفيًّا (يُطفئ
     تعريفات C مثل __sad_panic بتعريف ص مكافئ).

     - @رمز("bee_x") دالة اسم(...) ⇒ الدالّة تُصدَّر في LLVM IR باسم @bee_x
       (linkName يغلب الاسم الداخليّ عبر getLinkName)، مع جسم (define لا declare)
       وربط خارجيّ (لا internal).
     - تركيب مع «دالة لا_ترجع» (3.15): @رمز + لا_ترجع معًا ⇒ الرمز المُصدَّر
       يحمل سمة noreturn.
     - مسار @رمز على «متغير/ثابت» (3.14/3.16) يبقى سليمًا (لا انحدار).
----------------------------------------------------------------------------
(AR) لماذا مطابقة الاسم لا الوجود؟ الاسم ASCII المختار (bee_export_probe) لا
     يظهر في أيّ إصدار حرّ افتراضيّ، فوجوده في IR دليل قاطع على أثر @رمز وحده؛
     وغيابه عند إسقاط @رمز يثبت أنّ التصدير سببه لا مصادفة.
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
NO_MAIN = "--بلا-رئيسية"
MODULE_MODE = "--وحدة"

# (AR) افتراضٌ ضِمنيّ لتوكيدات الربط/النداء أدناه: مسار --أظهر-llvm يُخرِج IR خامًّا
#      بلا تحسين (‏-O0). لذا الدالّة الداخليّة (internal) لا تُضمَّن/تُحذَف، ونداء
#      المستدعية لا ينطوي — فتوكيدا M4 (define internal) وM5 (≥2 نداء) صادقان.
#      إن أُدرج يومًا تمريرُ تحسينٍ قبل الإخراج فقد يهتزّ هذان التوكيدان (توثيق
#      رصد Amelia — بند متبقٍّ LOW).

_EXPORT_SYM = "bee_export_probe"
_PANIC_SYM = "bee_panic_probe"

# (AR) دالّة مُصدَّرة عبر @رمز — جسم بسيط ذو عائد.
_FUNC_EXPORTED = (
    '@رمز("' + _EXPORT_SYM + '") دالة برهان(رقم س)\n'
    "    ارجع س + 1\n"
    "نهاية\n"
)
# (AR) ضابط: نفس الدالّة بلا @رمز — الاسم ASCII يجب أن يغيب.
_FUNC_PLAIN = (
    "دالة برهان(رقم س)\n"
    "    ارجع س + 1\n"
    "نهاية\n"
)
# (AR) تركيب @رمز + لا_ترجع — رمز مُصدَّر بسمة noreturn.
_FUNC_EXPORTED_NORETURN = (
    '@رمز("' + _PANIC_SYM + '") دالة لا_ترجع برهان()\n'
    "    بينما (صحيح)\n"
    "        أسبت_المعالج()\n"
    "    نهاية\n"
    "نهاية\n"
)
# (AR) مسار @رمز على متغيّر ساكن (3.16) — يجب أن يبقى سليمًا (لا انحدار).
_VAR_EXPORTED = '@رمز("bee_var_probe") متغير ساكن خزن مصفوفة[16]\n'


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


def _define_line(ir: str, symbol: str) -> str | None:
    """(AR) يعيد سطر define الخاصّ بالرمز إن وُجد."""
    for line in ir.splitlines():
        if line.startswith("define") and ("@" + symbol + "(") in line:
            return line
    return None


def test_exported_function_emits_symbol():
    """(AR) @رمز على دالّة ⇒ الرمز ASCII يظهر معرَّفًا (define) في IR."""
    rc, log, ir = _compile(_FUNC_EXPORTED, FREESTANDING, NO_MAIN)
    assert rc == 0, f"فشل الترجمة: {log}"
    assert _define_line(ir, _EXPORT_SYM) is not None, \
        f"الرمز المُصدَّر @{_EXPORT_SYM} غائب عن IR:\n{ir}"


def test_plain_function_absent_symbol():
    """(AR) ضابط: بلا @رمز ⇒ الاسم ASCII يغيب (التصدير سببه @رمز لا مصادفة)."""
    rc, log, ir = _compile(_FUNC_PLAIN, FREESTANDING, NO_MAIN)
    assert rc == 0, f"فشل الترجمة: {log}"
    assert ("@" + _EXPORT_SYM) not in ir, \
        f"الاسم ASCII ظهر بلا @رمز — تسرّب لا مصادفة:\n{ir}"


def test_exported_function_is_definition_not_declaration():
    """(AR) الدالّة المُصدَّرة لها جسم (define) لا مجرّد إعلان (declare)."""
    rc, log, ir = _compile(_FUNC_EXPORTED, FREESTANDING, NO_MAIN)
    assert rc == 0, f"فشل الترجمة: {log}"
    # (AR) declare @bee_export_probe يعني بلا جسم — يجب ألّا يظهر
    assert not re.search(r"^declare[^\n]*@" + _EXPORT_SYM + r"\(", ir, re.M), \
        f"الرمز أُعلن بلا جسم (declare) بدل تعريفه (define):\n{ir}"
    assert _define_line(ir, _EXPORT_SYM) is not None


# (AR) وضع الوحدة (--وحدة): دالّة عاديّة ذات جسم تُدوَّل (internal) لتفادي تصادم
#      الرموز عبر الوحدات؛ الدالّة المُصدَّرة عبر @رمز (linkName≠الاسم ⇒ FFI) تبقى
#      external. مساعد + مُصدَّرة معًا في الوضع نفسه ⇒ التباين دليلٌ أنّ الربط
#      الخارجيّ سببه @رمز لا الوضع (رصد Amelia M4: التوكيد كان أجوفَ في --حرّ
#      وحده حيث كلّ الدوالّ external).
_FUNC_EXPORTED_PLUS_PLAIN = (
    '@رمز("' + _EXPORT_SYM + '") دالة مُصدَّرة(رقم س)\n'
    "    ارجع مساعد_داخليّ(س) + 1\n"
    "نهاية\n"
    "دالة مساعد_داخليّ(رقم ص)\n"
    "    ارجع ص + 2\n"
    "نهاية\n"
)


def test_exported_function_external_linkage():
    """(AR) في وضع الوحدة: المُصدَّرة external والعاديّة internal — الربط الخارجيّ
    سببه @رمز لا الوضع (توكيد ذو معنى، رصد Amelia M4)."""
    rc, log, ir = _compile(_FUNC_EXPORTED_PLUS_PLAIN, FREESTANDING, MODULE_MODE, NO_MAIN)
    assert rc == 0, f"فشل الترجمة: {log}"
    dline = _define_line(ir, _EXPORT_SYM)
    assert dline is not None, f"الرمز المُصدَّر @{_EXPORT_SYM} غائب:\n{ir}"
    assert "internal" not in dline, \
        f"الرمز المُصدَّر داخليّ (internal) فلا يراه كود C:\n{dline}"
    # (AR) الدالّة العاديّة يجب أن تُدوَّل في وضع الوحدة — إثبات أنّ التمييز فعّال
    #      (لولا ذلك لكان توكيد «external» أجوفَ: كلّ الدوالّ external افتراضيًّا).
    plain_internal = re.search(r"^define internal [^\n]*@[^\n]*\(", ir, re.M)
    assert plain_internal is not None, \
        f"الدالّة العاديّة لم تُدوَّل في وضع الوحدة — التمييز معطَّل فالتوكيد أجوف:\n{ir}"


def test_exported_noreturn_combo():
    """(AR) @رمز + دالة لا_ترجع ⇒ رمز مُصدَّر يحمل noreturn (تركيب 3.15+3.17)."""
    rc, log, ir = _compile(_FUNC_EXPORTED_NORETURN, FREESTANDING, NO_MAIN)
    assert rc == 0, f"فشل الترجمة: {log}"
    dline = _define_line(ir, _PANIC_SYM)
    assert dline is not None, f"الرمز @{_PANIC_SYM} غائب:\n{ir}"
    # (AR) noreturn إمّا في سطر define مباشرةً أو عبر مجموعة سمات #N مُشار إليها.
    has_inline = "noreturn" in dline
    grp = re.search(r"#(\d+)", dline)
    has_group = False
    if grp:
        attrs = re.search(r"^attributes #" + grp.group(1) + r" = \{[^}]*noreturn", ir, re.M)
        has_group = attrs is not None
    assert has_inline or has_group, \
        f"الرمز المُصدَّر @{_PANIC_SYM} بلا سمة noreturn:\n{dline}"


def test_var_export_path_intact():
    """(AR) لا انحدار: @رمز على متغيّر ساكن (3.16) يبقى يُصدِّر رمزه."""
    rc, log, ir = _compile(_VAR_EXPORTED, FREESTANDING, NO_MAIN)
    assert rc == 0, f"فشل الترجمة: {log}"
    assert "@bee_var_probe" in ir, \
        f"مسار @رمز على المتغيّر انكسر — الرمز غائب:\n{ir}"


# (AR) تصادم اسم رمز مُصدَّر مكرَّر — دالّتان مُعرَّفتان بنفس @رمز.
_FUNC_DUP_SYMBOL = (
    '@رمز("bee_dup_probe") دالة اولى()\n    ارجع\nنهاية\n'
    '@رمز("bee_dup_probe") دالة ثانية()\n    ارجع\nنهاية\n'
)
# (AR) نداء داخليّ لدالّة مُصدَّرة — يجب أن يُحلّ للرمز المُصدَّر (لا يتعطّل).
#      الدالّة ذاتيّة العودة عمدًا: النداء الذاتيّ غير القابل للتضمين يضمن
#      ظهور `call @bee_callee_probe` فعليًّا (جسم بسيط غير عوديّ يُضمَّن
#      فيُطوى النداء ⇒ لا تعليمة call، فلا يثبت شيئًا).
_FUNC_EXPORT_AND_CALL = (
    '@رمز("bee_callee_probe") دالة مُصدَّرة(رقم س)\n'
    "    إذا (س > 0)\n"
    "        ارجع مُصدَّرة(س - 1) + 1\n"
    "    نهاية\n"
    "    ارجع 0\n"
    "نهاية\n"
    "دالة مستدعية(رقم ص)\n    ارجع مُصدَّرة(ص) + 1\nنهاية\n"
)


def test_duplicate_link_symbol_rejected():
    """(AR) SEM024: رمزان مُصدَّران باسم رابط واحد ⇒ خطأ قاتل (لا دمج صامت)."""
    rc, log, ir = _compile(_FUNC_DUP_SYMBOL, FREESTANDING, NO_MAIN)
    assert rc != 0, f"التصادم مرّ بلا خطأ (دمج صامت) — IR:\n{ir}"
    assert "SEM024" in log or "bee_dup_probe" in log, \
        f"الخطأ لا يذكر التصادم بوضوح:\n{log}"


# (AR) تصادم دالّة/متغيّر تحت اسم رمز واحد — LLVM يُعيد تسمية أحدهما (@اسم.1)
#      فيَضيع عقد الـABI صامتًا. يجب أن يُرفَض بـSEM024 (رصد Amelia H2).
_FUNC_VAR_CLASH = (
    '@رمز("bee_clash_probe") متغير ساكن خزن مصفوفة[16]\n'
    '@رمز("bee_clash_probe") دالة برهان()\n    ارجع\nنهاية\n'
)


def test_func_var_symbol_collision_rejected():
    """(AR) SEM024: دالّة ومتغيّر باسم رمز مُصدَّر واحد ⇒ خطأ قاتل (لا إعادة
    تسمية صامتة تُضيّع عقد الـABI). سدّ ثغرة func-vs-var (رصد Amelia H2)."""
    rc, log, ir = _compile(_FUNC_VAR_CLASH, FREESTANDING, NO_MAIN)
    assert rc != 0, f"تصادم دالّة/متغيّر مرّ بلا خطأ (إعادة تسمية صامتة) — IR:\n{ir}"
    assert "SEM024" in log or "bee_clash_probe" in log, \
        f"الخطأ لا يذكر التصادم بوضوح:\n{log}"


def test_internal_call_resolves_to_export():
    """(AR) نداء دالّة @رمز من دالّة ص أخرى يُحلّ للرمز المُصدَّر (ثغرة تغطية).
    نتوقّع نداءَين اثنين على الأقلّ: النداء الذاتيّ داخل «مُصدَّرة» + نداء
    «مستدعية» الخارجيّ. اشتراط ≥2 يمنع نجاحًا زائفًا يقنع بالنداء الذاتيّ
    وحده بينما فشل ربطُ نداء المستدعية بالرمز المُصدَّر (رصد Amelia M5)."""
    rc, log, ir = _compile(_FUNC_EXPORT_AND_CALL, FREESTANDING, NO_MAIN)
    assert rc == 0, f"فشل الترجمة: {log}"
    assert _define_line(ir, "bee_callee_probe") is not None, \
        f"الرمز المُصدَّر غائب:\n{ir}"
    call_sites = re.findall(r"call[^\n]*@bee_callee_probe\(", ir)
    assert len(call_sites) >= 2, (
        f"توقّعت ≥2 نداء للرمز المُصدَّر (ذاتيّ + مستدعية) فوُجد {len(call_sites)} — "
        f"نداء المستدعية الخارجيّ لم يُحلّ للرمز المُصدَّر:\n{ir}"
    )
