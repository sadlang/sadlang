"""
============================================================================
(AR) اختبارات منع انحدار: **حجمُ الكومة الساكنة الحرّة**.

     العطب المقيس الذي ولّد هذه الاختبارات: `HEAP_SIZE` كان رقمًا **واحدًا**
     (512م.ب) لكلّ الأهداف. القيمةُ نفسُها قرارٌ سليمٌ مقيسٌ لنطاق المستخدم
     (`0cab87a2`، بقياسٍ حيٍّ على قشرة sad-os)، لكنّ تعميمَها على المعدن
     العاري قاتل: الكومةُ مصفوفةٌ في `.bss`،
     وسكربتُ ربطِ النواةِ يضعها داخل `[_kernel_start.._kernel_end]`، ومخصِّصُ
     الإطارات يحجز ذلك المدى كلَّه ⇒ امتدّ `_kernel_end` إلى ≈514م.ب بينما
     تُقلع نواةُ النحلة بـ`-m 128M` ⇒ صفرُ إطاراتٍ حرّة ⇒ ماتت كلُّ خدمةٍ
     تحجز ذاكرة. والفرضيّةُ المكتوبة في الكود («‎.bss‎ صفريّةٌ لا تُكلِّف؛
     التصفيحُ عند أوّل مسّ») صحيحةٌ في نطاق المستخدم وباطلةٌ على المعدن العاري:
     لا مُصفِّحَ هناك — النواةُ هي المُصفِّح.

     ⚠️ لماذا يُثبَّت الافتراضيُّ برقمٍ صريح: العددُ الكبيرُ لا يُخفق البناءَ
     ولا الربط — يُخفق **الإقلاع** على عتادٍ أصغر، بعد الدمج بأيّامٍ وفي
     مستودعٍ آخر. فبلا توكيدٍ هنا يعود الانحدارُ صامتًا كما جاء أوّلَ مرّة.

(EN) Regression tests: freestanding static heap size.
     Measured defect: HEAP_SIZE silently became 512 MiB, extending _kernel_end
     past available RAM so the frame allocator reserved every free frame.
     The default is pinned to an exact number here because an oversized heap
     breaks neither the build nor the link — only the boot, on other hardware,
     in another repository, days later.
============================================================================
"""
from __future__ import annotations

import re
import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

# (AR) ⚠️ Release أوّلًا: هذه الاختبارات تحرس إصلاحًا حديثًا، وثنائيّ Debug
#      بائتٌ يجعلها تسقط وإن كان المصدر سليمًا.
# (AR) ⚠️ ومسارات بلا ‎.exe‎: على لينكس/ماك الثنائيّ ‎build/bin/sad-build‎. بدونها
#      يبقى ‎SAD_BUILD = None‎ فيبتلع ‎skipif‎ الملفَّ كلَّه **أخضرَ صامتًا** —
#      وهو بعينه صنفُ الاختبار الذي يصادق على نفسه، على المنصّة التي تبني
#      المترجمَ فعلًا في CI.
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
I686 = "--هدف=i686-unknown-none-elf"
HEAP_SIZE_FLAG = "--حجم-الكومة"

MIB = 1024 * 1024

# (AR) الافتراضيّ مشروطٌ بالهدف: لا مُصفِّحَ تحت المعدن العاري فالمعلَنُ مقيم،
#      وتحت نطاق المستخدم يُصفَّح عند الطلب فالإعلانُ الكبير مجّانيّ.
DEFAULT_HEAP_BARE_METAL_MIB = 4
DEFAULT_HEAP_USERSPACE_MIB = 512

# (AR) نقطةُ التفاف ‎sh_size‎ على ELF32: ‎4096م.ب = 2^32‎ بالضبط. مقيسٌ أنّ
#      القسم يصير مئاتِ بايتات والمُجمِّعُ يخرج بصفر. لا يجوز أن يكون سقفًا
#      **معلَنًا** في أيّ طبقة — هذه القيمة بعينها هي العيب.
WRAP_POINT_MIB = 4096

# (AR) نصفُ فضاء العنونة على 32-بت — سقفُ التوليد الوحيد، معتمدٌ على عرض الهدف.
MAX_HEAP_32BIT_MIB = 2048

LINUX_I686 = "--هدف=i686-unknown-linux-gnu"

# (AR) هدفٌ بعرض 16 بتًّا: سقفُه (32ك.ب) **دون** أيّ افتراضيٍّ نعلنه بالميغابايت.
MSP430 = "--هدف=msp430-none-elf"

# (AR) معدنٌ عارٍ بعرض 64 بتًّا — ‎BareMetalPortIO‎ يشمله، فهو مسارُ نواةٍ حقيقيّ.
X86_64_BARE = "--هدف=x86_64-unknown-none-elf"

# (AR) أقصى ما يعبّر عنه العلمُ: ‎uint32‎ ميغابايت ≈ 4 پ.ب. مقيسٌ أنّ هذا الرقم
#      كان يُنتج قسم ‎.bss‎ حجمُه 4 503 599 626 322 880 بايتًا في كائنٍ حجمُه 11ك.ب،
#      بخروجٍ صفريٍّ بلا تشخيص — صمتٌ نظيرُ صمتِ التفافِ ELF32.
MAX_EXPRESSIBLE_MIB = 4294967295

# (AR) نصفُ العنونة القانونيّة (2^48) = 128 ت.ب. المرجعُ عرضُ العنونة لا الكلمة:
#      سقفٌ عند نصفِ ‎2^64‎ لا يردعُ شيئًا لأنّ مجال العلمِ نفسِه دونه بمراحل.
MAX_HEAP_64BIT_MIB = 128 * 1024 * 1024

# (AR) شواهدُ نصّيّة تميّز المسارَ المقصود عن «خيار غير معروف». بلا هذه الشواهد
#      يمرّ توكيدُ «الرمز ≠ 0» على ثنائيٍّ **لا يعرف العلم أصلًا** — أخضرُ للسبب
#      الخطأ، وهو بعينه صنفُ الاختبار الذي يصادق على نفسه.
SEM037_MARK = "كومة الوضع الحرّ"
CAP_MARK = "الأقصى"
NEEDS_FREESTANDING_MARK = "تدير الكومةَ مكتبةُ النظام"
REQUIRES_ARGUMENT_MARK = "يتطلّب قيمة"


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


def _heap_bytes(ir: str) -> int:
    """(AR) حجمُ ‎@__sad_heap‎ المعلَن في الإصدار، بالبايت."""
    match = re.search(r"@__sad_heap\s*=\s*(?:\S+\s+)*?global\s+\[(\d+) x i8\]", ir)
    assert match, "لا كومةَ ساكنةٌ في المخرَج الحرّ:\n" + ir[:1500]
    return int(match.group(1))


SOURCE = 'اطبع("مرحبا")\n'


# ─────────────── الافتراضيّ مثبَّتٌ برقم ───────────────

def test_bare_metal_default_is_pinned():
    """(AR) افتراضيّ المعدن العاري 4م.ب — رفعُه صامتًا يكسر الإقلاع لا البناء."""
    code, out, ir = _compile(SOURCE, FREESTANDING, I686)
    assert code == 0, "فشلت الترجمة الحرّة:\n" + out
    measured = _heap_bytes(ir)
    assert measured == DEFAULT_HEAP_BARE_METAL_MIB * MIB, (
        f"حجمُ كومة المعدن العاري {measured} بايت ({measured // MIB}م.ب) — "
        f"المتوقَّع {DEFAULT_HEAP_BARE_METAL_MIB}م.ب.\n"
        "الكومةُ مصفوفةٌ في ‎.bss‎ داخل ‎[_kernel_start.._kernel_end]‎ ويحجزها "
        "مخصِّصُ الإطارات كاملةً، فالمعلَنُ مقيمٌ دائمًا هنا. من احتاج أكبرَ "
        "فبـ«--حجم-الكومة=…» عند موضع البناء حيث يُعرَف الرام."
    )


def test_userspace_default_is_pinned():
    """(AR) نطاقُ المستخدم يبقى 512م.ب — القرارُ المقيس في ‎0cab87a2‎ لم يُمسّ.

    البذرةُ المقصودة: من «يُصلح» العطبَ بخفضِ رقمٍ واحدٍ للجميع يُقصّر عمرَ
    تشغيل القشرة ثمانيةَ أضعافٍ صامتًا — لا بوّابةَ نقعٍ تلتقط ذلك.
    """
    code, out, ir = _compile(SOURCE, FREESTANDING, LINUX_I686)
    assert code == 0, out
    measured = _heap_bytes(ir)
    assert measured == DEFAULT_HEAP_USERSPACE_MIB * MIB, (
        f"حجمُ كومة نطاق المستخدم {measured} بايت — المتوقَّع "
        f"{DEFAULT_HEAP_USERSPACE_MIB}م.ب. التصفيحُ عند الطلب يجعل المعلَنَ "
        "غيرَ مقيم، فالخفضُ هنا انحدارُ عمرِ تشغيلٍ بلا مقابل."
    )


def test_defaults_differ_by_target():
    """(AR) الفارقُ نفسُه هو العقد: رقمٌ واحدٌ للجميع هو ما ولّد العطب."""
    _, _, bare = _compile(SOURCE, FREESTANDING, I686)
    _, _, user = _compile(SOURCE, FREESTANDING, LINUX_I686)
    assert _heap_bytes(bare) < _heap_bytes(user), (
        "تساوى افتراضيّا المعدن العاري ونطاق المستخدم — عاد الرقمُ الواحد."
    )


# ─────────────── العلم يعمل في الاتّجاهين ───────────────

@pytest.mark.parametrize("mib", [1, 16, 256])
def test_flag_sets_heap_size(mib: int):
    """(AR) «--حجم-الكومة=ن» يضبط الكومةَ إلى ن ميغابايت بالضبط."""
    code, out, ir = _compile(SOURCE, FREESTANDING, I686, f"{HEAP_SIZE_FLAG}={mib}")
    assert code == 0, out
    assert _heap_bytes(ir) == mib * MIB, (
        f"«{HEAP_SIZE_FLAG}={mib}» أعطى {_heap_bytes(ir)} بايت"
    )


def test_flag_can_exceed_default():
    """(AR) العلم لا يقتصر على التصغير — من يملك رامًا يطلب سعةً أكبر صراحةً."""
    code, out, ir = _compile(SOURCE, FREESTANDING, I686, f"{HEAP_SIZE_FLAG}=512")
    assert code == 0, out
    assert _heap_bytes(ir) == 512 * MIB


# ─────────────── حارسُ الالتفاف: أخطرُ مسارٍ في هذا العلم ───────────────

def test_heap_at_32bit_address_space_is_rejected():
    """(AR) 4096م.ب على هدفٍ 32-بت تلتفّ في ‎sh_size‎ إلى 496 بايتًا.

    مقيس: ‎.bss‎ عند 4095م.ب = 4 293 919 216، وعند 4096م.ب = **496**.
    ويخرج المُجمِّعُ والرابطُ بصفرٍ بلا تشخيص. والأخطر أنّ فحصَ الفيضان في
    ‎malloc‎ يقارن بـi64، فيظنّ المخصِّصُ أنّ لديه 4ﭼ.ب والقسمُ 496 بايتًا ⇒
    إفسادُ ذاكرةٍ صامت. يجب أن تُرفَض زمنَ الترجمة لا أن تُسلَّم.
    """
    code, out, _ = _compile(
        SOURCE, FREESTANDING, I686, f"{HEAP_SIZE_FLAG}={WRAP_POINT_MIB}"
    )
    assert code != 0, (
        f"{WRAP_POINT_MIB}م.ب قُبلت على هدفٍ 32-بت — تلتفّ ‎.bss‎ إلى مئاتِ بايتات "
        "ويكتب المخصِّصُ خارجها صامتًا:\n" + out
    )
    assert SEM037_MARK in out, (
        "رُفضت عبر مسارٍ آخر لا عبر تشخيص سقفِ الكومة — الرمزُ غيرُ الصفريّ وحدَه "
        "يأتي أيضًا من «خيار غير معروف»:\n" + out
    )


def test_heap_above_half_address_space_is_rejected():
    """(AR) السقفُ نصفُ فضاء العنونة — يترك مجالًا لبقيّة الأقسام والمكدّس."""
    code, out, _ = _compile(
        SOURCE, FREESTANDING, I686, f"{HEAP_SIZE_FLAG}={MAX_HEAP_32BIT_MIB + 1}"
    )
    assert code != 0, f"{MAX_HEAP_32BIT_MIB + 1}م.ب قُبلت على 32-بت:\n" + out
    assert f"{CAP_MARK} {MAX_HEAP_32BIT_MIB}" in out, (
        f"التشخيصُ لم يُسمِّ السقفَ {MAX_HEAP_32BIT_MIB}م.ب — إمّا أنّ الرفضَ جاء من "
        "مسارٍ آخر، وإمّا أنّ الإرشادَ لم يعُد قابلًا للّصق:\n" + out
    )


def test_64bit_target_is_capped_too():
    """(AR) 64 بتًّا ليست مُستثناةً — الصمتُ هناك نظيرُ صمتِ التفافِ ELF32.

    مقيسٌ قبل الإصلاح: ‎--هدف=x86_64-unknown-none-elf --حجم-الكومة=4294967295‎
    أنتجت كائنًا حجمُه 11ك.ب يحمل قسم ‎.bss‎ حجمُه 4 503 599 626 322 880 بايتًا،
    وخرج المترجمُ بصفرٍ بلا تشخيص. ‎sh_size‎ في ELF64 لا يلتفّ، لكنّ الرابطَ
    وحدَه من يشكو — بعد الترجمة، ورسالتُه غامضة.

    ⚠️ البذرةُ المقصودة الثانية: من «يسدّ» الثغرةَ بسقفٍ عند نصفِ ‎2^64‎ يكتب
    حارسًا لا ينطلق أبدًا — مجالُ العلمِ نفسِه (‎uint32‎ م.ب ≈ ‎2^52‎) دونه
    بأحدَ عشرَ رتبةً. فالمرجعُ عرضُ العنونة القانونيّ (48 بتًّا) لا عرضُ الكلمة.
    """
    code, out, _ = _compile(
        SOURCE, FREESTANDING, X86_64_BARE, f"{HEAP_SIZE_FLAG}={MAX_EXPRESSIBLE_MIB}"
    )
    assert code != 0, (
        f"{MAX_EXPRESSIBLE_MIB}م.ب (≈4 پ.ب) قُبلت على معدنٍ عارٍ 64-بت — "
        "يُبَثُّ قسمُ ‎.bss‎ بحجمٍ لا وجودَ له في أيّ آلة:\n" + out
    )
    assert SEM037_MARK in out, (
        "رُفضت عبر مسارٍ آخر لا عبر تشخيص سقفِ الكومة:\n" + out
    )
    assert f"{CAP_MARK} {MAX_HEAP_64BIT_MIB}" in out, (
        f"السقفُ المُعلَن ليس {MAX_HEAP_64BIT_MIB}م.ب — إن صار نصفَ ‎2^64‎ فهو "
        "حارسٌ لا ينطلق أبدًا:\n" + out
    )


def test_64bit_ordinary_size_still_passes():
    """(AR) السقفُ الجديد لا يخنق الاستعمالَ العاديّ على 64 بتًّا."""
    code, out, ir = _compile(
        SOURCE, FREESTANDING, X86_64_BARE, f"{HEAP_SIZE_FLAG}=1024"
    )
    assert code == 0, "رُفضت كومةُ 1ﭼ.ب على 64-بت وهي داخل السقف:\n" + out
    assert _heap_bytes(ir) == 1024 * MIB


def test_heap_at_64bit_cap_is_accepted():
    """(AR) نظيرُ اختبارِ حدّ الـ32-بت: الحدُّ نفسُه يمرّ، وإلّا كان إزاحةً بواحد.

    بلا هذا التوكيد يبقى فرعُ الـ64 مغطًّى من جهةِ الرفضِ وحدَها، فإزاحةٌ بواحدٍ
    مستقبليّةٌ تُضيّق السقفَ صامتةً ولا يمسكها شيء.
    """
    code, out, ir = _compile(
        SOURCE, FREESTANDING, X86_64_BARE, f"{HEAP_SIZE_FLAG}={MAX_HEAP_64BIT_MIB}"
    )
    assert code == 0, f"{MAX_HEAP_64BIT_MIB}م.ب رُفضت وهي الحدُّ نفسُه:\n" + out
    assert _heap_bytes(ir) == MAX_HEAP_64BIT_MIB * MIB


def test_heap_at_32bit_cap_is_accepted():
    """(AR) الحدُّ المقبولُ نفسُه يجب أن يمرّ — وإلّا كان السقفُ إزاحةً بواحد."""
    code, out, ir = _compile(
        SOURCE, FREESTANDING, I686, f"{HEAP_SIZE_FLAG}={MAX_HEAP_32BIT_MIB}"
    )
    assert code == 0, f"{MAX_HEAP_32BIT_MIB}م.ب رُفضت وهي داخل السقف:\n" + out
    assert _heap_bytes(ir) == MAX_HEAP_32BIT_MIB * MIB


# ─────────────── العلم بلا --حرّ لا يُقبَل صامتًا ───────────────

def test_heap_size_without_freestanding_is_rejected():
    """(AR) علمٌ يُقبَل بلا أثرٍ هو الفشلُ الصامتُ بعينه.

    قبل الإصلاح: `sad-build t.ص --حجم-الكومة=256` ⇒ EXIT=0، صفرُ تشخيص،
    صفرُ أثر. فمن ينسى `--حرّ` يظنّ أنّه ضبط شيئًا.
    """
    code, out, _ = _compile(SOURCE, f"{HEAP_SIZE_FLAG}=256")
    assert code != 0, "العلمُ قُبل مستضافًا بلا أثرٍ ولا تشخيص:\n" + out
    assert NEEDS_FREESTANDING_MARK in out, "رُفض بلا تسمية السبب:\n" + out


def test_heap_size_is_accepted_via_no_std_attribute():
    """(AR) الوضعُ الحرُّ يُفعَّل من طريقين، والعلمُ يجب أن يصلح لكليهما.

    البذرةُ المقصودة: الفحصُ كان يقع زمنَ وزمِ سطر الأوامر، وسمةُ المصدر لا
    تُقرأ إلّا بعد فتح الملفّ ⇒ نواةٌ حرّةٌ فعلًا تُرفَض برسالةٍ تصفُها
    بالمستضافة، فيصير العلمُ الجديد عديمَ الفائدة لكلِّ نواةٍ تكتفي بالسمة —
    وهو المسارُ الموثَّق في اللغة.
    """
    source = "#![no_std]\n" + SOURCE
    code, out, ir = _compile(source, I686, f"{HEAP_SIZE_FLAG}=16")
    assert code == 0, (
        "رُفض «--حجم-الكومة» مع سمة ‎#![no_std]‎ — البناءُ حرٌّ فعلًا:\n" + out
    )
    assert _heap_bytes(ir) == 16 * MIB, (
        f"السمةُ فعّلت الوضعَ الحرَّ لكنّ العلمَ لم يُطبَّق: {_heap_bytes(ir)} بايت"
    )


# ─────────────── القيمُ الفاسدة تُرفَض بصوت ───────────────

@pytest.mark.parametrize("bad", ["0", "64م", "abc", "-8", "12.5"])
def test_invalid_heap_size_is_rejected(bad: str):
    """(AR) قيمةٌ فاسدة تُخفق الترجمة — لا تُقتطع صامتةً إلى رقمٍ مقبول.

    الفخّ المقصود: `stoul("64م")` تعيد 64 بلا شكوى، فتصير الوحدةُ المكتوبةُ
    خطأً حجمًا صالحًا. والتحقّقُ محرفًا-محرفًا يمنع ذلك.

    ⚠️ التوكيدُ على **نصّ الرسالة** لا على الرمز وحده: الرمزُ غيرُ الصفريّ
    يأتي أيضًا من «خيار غير معروف»، فيمرّ الاختبارُ للسبب الخطأ ويخفي أنّ
    المسار المقصود لم يُبلَغ أصلًا.
    """
    code, out, _ = _compile(SOURCE, FREESTANDING, I686, f"{HEAP_SIZE_FLAG}={bad}")
    assert code != 0, (
        f"«{HEAP_SIZE_FLAG}={bad}» قُبلت صامتةً — القيمُ الفاسدة يجب أن تُخفق:\n" + out
    )
    assert "حجم الكومة" in out, (
        f"«{HEAP_SIZE_FLAG}={bad}» رُفضت عبر مسارٍ آخر (خيار غير معروف؟) لا عبر "
        "تشخيص حجم الكومة:\n" + out
    )


def test_bare_flag_without_value_names_the_cause():
    """(AR) «--حجم-الكومة» بلا قيمة يجب أن يشكو الافتقارَ إلى قيمة."""
    code, out, _ = _compile(SOURCE, FREESTANDING, I686, HEAP_SIZE_FLAG)
    assert code != 0, "العلمُ بلا قيمةٍ قُبل:\n" + out
    # (AR) اسمُ العلم وحدَه يظهر أيضًا في «خيار غير معروف: --حجم-الكومة»، فيمرّ
    #      التوكيدُ على ثنائيٍّ لا يعرف العلمَ أصلًا. الشاهدُ هو سببُ الرفض.
    assert REQUIRES_ARGUMENT_MARK in out, (
        "رُفض بلا تسمية السبب (افتقارُ العلمِ إلى قيمة):\n" + out
    )
    assert HEAP_SIZE_FLAG in out, "رُفض بلا تسمية العلم:\n" + out


def test_narrow_target_default_never_blocks_compilation():
    """(AR) هدفٌ أضيقُ من الافتراضيّ: يُقلَّص صامتًا، ولا يُخفِق البناء.

    البذرةُ المقصودة: على هدفٍ بعرض 16 بتًّا سقفُ الهدف 32ك.ب، فافتراضيُّ
    المعدنِ العاري (4م.ب) يجاوزه. الإبلاغُ عنه خطأً يجعل الترجمةَ مستحيلةً
    **بلا أيّ علمٍ مُمرَّر**، وقيمةُ الإرشادِ تصير 0 و«--حجم-الكومة=0» مرفوضةٌ
    بدورها ⇒ طريقٌ مسدود. الافتراضيُّ اختيارُنا لا خطأُ المستخدم.
    """
    code, out, _ = _compile(SOURCE, FREESTANDING, MSP430)
    # (AR) تمييزُ «الهدفُ غيرُ مُسجَّلٍ في هذه البنية من LLVM» عن العيب: توزيعاتُ
    #      لينكس قد تبني LLVM بلا MSP430. التخطّي هنا يحمل سببَه الحقيقيَّ في
    #      نصّه، فلا يُقرأ نجاحًا.
    #
    #      ⚠️ الشرطان إنجليزيّان حصرًا وهما نصُّ LLVM المستقرّ. وكلُّ نمطٍ عربيٍّ
    #      عامٍّ هنا يبتلع تشخيصاتٍ حقيقيّة: «غير مدعوم» وحدَها تطابق «نوع المعامل
    #      … غير مدعوم» و«الفهرسة غير مدعومة» و«صيغة عنونة غير مدعومة»، فينقلب
    #      الحارسُ إلى ساترٍ للعيب الذي كُتب لكشفه — ويظهر حرفَ ‎s‎ واحدًا.
    if code != 0 and re.search(r"(unable to find target|No available targets)", out):
        pytest.skip(f"هدف msp430 غير مُسجَّل في هذه البنية من LLVM:\n{out.strip()[:300]}")
    assert code == 0, (
        "أخفقت الترجمةُ على هدفٍ ضيّقٍ بلا أيّ علمٍ مُمرَّر — الافتراضيُّ يجب أن "
        "يُقلَّص لا أن يُخفِق:\n" + out
    )
    assert SEM037_MARK not in out, (
        "أُبلغ عن تجاوزِ السقفِ والمستخدمُ لم يطلب حجمًا أصلًا:\n" + out
    )


def test_no_layer_advertises_the_wrap_point_as_a_limit():
    """(AR) سقفٌ **معلَنٌ** عند 4096م.ب يُقدّم نقطةَ الالتفاف حدًّا آمنًا.

    البذرةُ المقصودة: من يعيد سقفًا ثابتًا إلى طبقة المحلِّل (حيث لا يُعرَف
    عرضُ الهدف) يُنتج سقفَين متناقضَين — أحدُهما يقبل ما يرفضه الآخر. فيجب
    أن يأتي الرفضُ من تشخيص **التوليد** الذي يعرف العرض، لا من رقمٍ ثابت.
    """
    code, out, _ = _compile(
        SOURCE, FREESTANDING, I686, f"{HEAP_SIZE_FLAG}={WRAP_POINT_MIB}"
    )
    assert code != 0, out
    # (AR) شرطٌ مسبق: التوكيدُ السالبُ وحدَه أخضرُ على أيِّ مخرَجٍ كائنًا ما كان —
    #      حتّى على مخرَجٍ لا يذكر الكومةَ إطلاقًا. فنُثبت أوّلًا أنّ التشخيصَ
    #      المقصودَ حاضر، ثمّ ننفي أن يكون سقفُه نقطةَ الالتفاف.
    assert SEM037_MARK in out and CAP_MARK in out, (
        "لا تشخيصَ سقفٍ في المخرَج — التوكيدُ السالبُ أدناه بلا معنًى:\n" + out
    )
    assert not re.search(rf"(الحدّ الأقصى|الأقصى|maximum)[^\n]*{WRAP_POINT_MIB}", out), (
        f"رسالةٌ تُعلن {WRAP_POINT_MIB}م.ب حدًّا أقصى — وهي نقطةُ الالتفاف نفسُها:\n" + out
    )
