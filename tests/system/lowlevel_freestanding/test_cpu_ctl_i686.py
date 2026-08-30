"""
============================================================================
(AR) اختبارات منع انحدار للّبنة م-لغة.3.12: خفض مدمجات النظام إلى أسمبليٍّ مضمَّنٍ
     حرًّا (freestanding) **على هدف i686** بدل نداءات runtime مستضافة sad_ll_*.
     - اقرأ_سجل_تحكم/اكتب_سجل_تحكم: كانت sad_ll_read_cr/write_cr (مستضاف) ⇒
       صارت `mov %crN` أسمبليًّا مضمَّنًا حرًّا (بعرض سجلّ الهدف: 32 بتًّا هنا).
     - اقرأ_سجل_نموذج: كانت rdmsr+shlq/rax (x86-64) ⇒ صارت rdmsr + دمج edx:eax
       بلا تعليمات 64-بت.
     - ابطل_صفحة: invlpg بوسيط i64 ⇒ عنوانٌ بعرض سجلّ الهدف (32-بت على i686).
     - حرّاس ضدّ الإفراط: المستضافُ يبقى على نداءات sad_ll_* بلا مسّ.
       (⚠️ عدا `افرغ_ذاكرة_ترجمة`: مسارُها المستضافُ وُحِّد مع الحرّ عمدًا،
        وحارسُه في `test_cpu_ctl_target_width.py`.)
     - رقم CR غير الثابت حرًّا ⇒ خطأ ترجمة واضح (mov %crN يتطلّب N حرفيًّا).
(EN) Regression tests for milestone 3.12: system builtins lower to freestanding
     inline asm for an i686 target instead of hosted sad_ll_* calls.
----------------------------------------------------------------------------
(AR) لماذا --أظهر-llvm؟ الخفض يعيش في توليد LLVM؛ الراية تشغّل المسار كاملًا
     وتُخرج IR بلا اعتماد على lld-link/LIB (عزل عن بيئة الربط).

(AR) ولماذا «‎--هدف=i686-unknown-elf‎» صراحةً في كلّ حالةٍ حرّة: الوضعُ الحرّ بلا
     «‎--هدف‎» يقع على ثالوث **المضيف** — وهو x86_64 على كلّ آلات البناء عندنا.
     فكانت حالاتُ هذا الملفّ تحمل اسم i686 وتقيس x86_64، وتصادق على أسمبليٍّ
     ٣٢‑بتّيٍّ على هدفٍ ٦٤‑بتّيّ. المدى المحفوظ: هذا الملفُّ يحكم على **نصّ IR**
     لا على مخرَج المُجمِّع؛ الحكمُ بالتجميع في ‎test_cpu_ctl_target_width.py‎.
============================================================================
"""
from __future__ import annotations

import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

# (AR) ومسارات بلا ‎.exe‎ للينكس/ماك: بدونها يبتلع ‎skipif‎ الملفَّ أخضرَ صامتًا في CI.
# (AR) وRelease أوّلًا كبقيّة ملفّات المجلّد: كان هذا الملفُّ وحدَه يقدّم Debug،
#      فيفحص محلّيًّا ثنائيًّا أقدمَ من الذي تفحصه أخواتُه ⇒ إخفاقٌ مربكٌ سببُه
#      اختلافُ الثنائيّ لا اختلافُ الكود.
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
# (AR) هدفُ الحالات الحرّة صريحٌ كي تقيس ما يقوله اسمُها (لا ثالوثَ المضيف).
TARGET_I686 = "--هدف=i686-unknown-elf"
# (AR) 🔑 الهدفُ المُستضافُ مُصَرَّحٌ أيضاً. هذه الحالةُ تحرُسُ المسارَ المُستضاف،
#      لكنَّ دعواها تخُصُّ عائلةَ x86 لا محالة، وبلا «--هدف» تقعُ على ثالوثِ
#      المُشَغِّل. فتُصَرَّحُ بثالوثٍ مُستضافٍ x86_64 كي يبقى المقيسُ مُستضافاً
#      والمعماريّةُ مُصَرَّحةً — لا مُورَثةً من الآلة.
# (EN) The hosted target is declared too: the case guards the hosted path but its
#      claim is x86-family, so it declares a hosted x86_64 triple instead of
#      inheriting the runner's.
TARGET_X64_HOSTED = "--هدف=x86_64-unknown-linux-gnu"


def _compile(source: str, *extra_flags: str) -> tuple[int, str, str]:
    """(AR) يترجم المصدر بـ sad-build مع الرايات؛ يعيد (رمز الخروج، الخرج، IR)."""
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


# ─────────────── الوضع الحرّ: inline asm i686 لا نداء مستضاف ───────────────

def test_read_cr_freestanding_emits_mov_cr():
    """(AR) اقرأ_سجل_تحكم(0) حرًّا ⇒ `mov %cr0` inline asm، لا sad_ll_read_cr."""
    code, out, ir = _compile("متغير ق = اقرأ_سجل_تحكم(0)\nاطبع(ق)\n", FREESTANDING, TARGET_I686)
    assert code == 0, "اقرأ_سجل_تحكم(0) فشلت حرًّا:\n" + out
    assert "mov %cr0" in ir, "IR الحرّ بلا `mov %cr0` — لم يُخفَّض inline asm i686:\n" + ir[:2000]
    assert "sad_ll_read_cr" not in ir, "IR الحرّ ما زال ينادي sad_ll_read_cr المستضاف — سيفشل الربط"


def test_write_cr_freestanding_emits_mov_to_cr():
    """(AR) اكتب_سجل_تحكم(3, قيمة) حرًّا ⇒ `mov $0, %cr3`، لا sad_ll_write_cr."""
    code, out, ir = _compile("اكتب_سجل_تحكم(3، 305419896)\n", FREESTANDING, TARGET_I686)
    assert code == 0, "اكتب_سجل_تحكم فشلت حرًّا:\n" + out
    assert "%cr3" in ir, "IR الحرّ بلا كتابة إلى %cr3:\n" + ir[:2000]
    assert "sad_ll_write_cr" not in ir, "IR الحرّ ما زال ينادي sad_ll_write_cr المستضاف"


def test_read_msr_freestanding_no_x86_64():
    """(AR) اقرأ_سجل_نموذج حرًّا ⇒ rdmsr + دمج edx:eax بلا shlq/rax (x86-64)."""
    code, out, ir = _compile("متغير ق = اقرأ_سجل_نموذج(432)\nاطبع(ق)\n", FREESTANDING, TARGET_I686)
    assert code == 0, "اقرأ_سجل_نموذج فشلت حرًّا:\n" + out
    assert "rdmsr" in ir, "IR الحرّ بلا rdmsr:\n" + ir[:2000]
    assert "shlq" not in ir and "%rdx" not in ir and "%rax" not in ir, (
        "IR الحرّ يحوي تعليمات/سجلّات x86-64 (shlq/rdx/rax) — لا يعمل على i686"
    )


def test_invlpg_freestanding_i686():
    """(AR) ابطل_صفحة حرًّا ⇒ invlpg بعنوان 32-بت، لا نداء مستضاف."""
    code, out, ir = _compile("ابطل_صفحة(1048576)\n", FREESTANDING, TARGET_I686)
    assert code == 0, "ابطل_صفحة فشلت حرًّا:\n" + out
    assert "invlpg" in ir, "IR الحرّ بلا invlpg:\n" + ir[:2000]
    assert "sad_ll_" not in ir, "IR الحرّ ينادي رمز sad_ll مستضاف"


# ─────────────── حرّاس ضدّ الإفراط: المستضاف بلا مسّ ───────────────

def test_read_cr_hosted_keeps_runtime():
    """(AR) اقرأ_سجل_تحكم(0) مستضافًا يبقى على نداء sad_ll_read_cr (لا تسريب حرّ)."""
    code, out, ir = _compile("متغير ق = اقرأ_سجل_تحكم(0)\nاطبع(ق)\n", TARGET_X64_HOSTED)
    assert code == 0, "اقرأ_سجل_تحكم مستضافًا فشلت (انحدار):\n" + out
    assert "sad_ll_read_cr" in ir, "IR المستضاف فقد sad_ll_read_cr — تغيّر سلوك مستضاف غير مقصود"
    assert "mov %cr0" not in ir, "IR المستضاف يحوي inline asm cr0 — تسريب مسار حرّ للمستضاف"


# ─────────────── رقم CR غير ثابت حرًّا ⇒ خطأ ترجمة واضح ───────────────

def test_non_constant_cr_num_freestanding_errors():
    """(AR) رقم سجلّ تحكّم غير ثابت حرًّا ⇒ خروج غير صفريّ (mov %crN يتطلّب N حرفيًّا)."""
    src = (
        "دالة ف(رقم ن)\n"
        "    متغير ق = اقرأ_سجل_تحكم(ن)\n"
        "    ارجع ق\n"
        "نهاية\n"
    )
    code, out, _ = _compile(src, FREESTANDING, TARGET_I686)
    assert code != 0, "رقم CR غير ثابت نجح حرًّا — لا يمكن إصدار `mov %crN` برقم متغيّر:\n" + out


# ─────────────── lidt/lgdt حرًّا بمؤشّر واصف (دَين 3.12 المُغلَق) ───────────────

def test_lidt_freestanding_with_pointer():
    """(AR) حمل_جدول_مقاطعات(مؤشّر) حرًّا ⇒ lidt inline asm، لا sad_ll_idt_load."""
    code, out, ir = _compile("حمل_جدول_مقاطعات(1048576)\n", FREESTANDING, TARGET_I686)
    assert code == 0, "حمل_جدول_مقاطعات بمؤشّر فشلت حرًّا:\n" + out
    assert "lidt" in ir, "IR الحرّ بلا lidt:\n" + ir[:2000]
    assert "sad_ll_idt_load" not in ir, "IR الحرّ ما زال ينادي sad_ll_idt_load المستضاف"


def test_lgdt_freestanding_with_pointer():
    """(AR) حمل_جدول_واصفات(مؤشّر) حرًّا ⇒ lgdt inline asm، لا sad_ll_gdt_load."""
    code, out, ir = _compile("حمل_جدول_واصفات(1048576)\n", FREESTANDING, TARGET_I686)
    assert code == 0, "حمل_جدول_واصفات بمؤشّر فشلت حرًّا:\n" + out
    assert "lgdt" in ir, "IR الحرّ بلا lgdt:\n" + ir[:2000]
    assert "sad_ll_gdt_load" not in ir, "IR الحرّ ما زال ينادي sad_ll_gdt_load المستضاف"


def test_lidt_freestanding_without_pointer_errors():
    """(AR) حمل_جدول_مقاطعات() بلا مؤشّر حرًّا ⇒ خروج غير صفريّ (lidt يتطلّب مؤشّر واصف)."""
    code, out, _ = _compile("حمل_جدول_مقاطعات()\n", FREESTANDING, TARGET_I686)
    assert code != 0, "حمل_جدول_مقاطعات بلا مؤشّر نجحت حرًّا — lidt يتطلّب واصفًا:\n" + out
