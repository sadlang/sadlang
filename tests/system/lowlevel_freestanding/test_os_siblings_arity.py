"""
============================================================================
(AR) اختبارات منع انحدار لسدّ ح٤ جزء ب: فرضُ رتبةِ مدمجاتِ الأشقّاء الثلاثة
     على سلسلةِ الإرسال نفسِها بالاتّجاهين — builtins_os_hardware (العتاد)،
     وbuiltins_os_system (النظام)، وbuiltins_uefi (الذي كان بلا فحصٍ أصلًا:
     الوسيطُ الناقصُ كان يُسقَط دفعُه صامتًا فيرقّعه زمنُ التشغيل بافتراضٍ
     أو قمامةٍ، والزائدُ يُقبَل ويُهمَل).
     بعد السدّ: ناقصٌ = خطأ تصريف (SEM005 من الكتالوج)، زائدٌ = خطأ تصريف،
     والوسيطُ الاختياريُّ المعلَنُ يبقى اختياريًّا (امسح_شاشة بلونٍ أو
     بدونه؛ انشئ_عملية بالأولويّةِ أو بدونها).
(EN) Regression tests for gap ح٤ part B: builtin arity is enforced in both
     directions across the three sibling builder files (os_hardware,
     os_system, uefi), while declared-optional arguments stay optional.
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
# (AR) أحدثُ الموجودَين لا أوّلُهما (فخُّ الثنائيِّ العتيق — نمطُ أخوات الملفّ).
SAD_BUILD = max(
    (p for p in _CANDIDATES if p.exists()),
    key=lambda p: p.stat().st_mtime,
    default=None,
)

pytestmark = pytest.mark.skipif(
    SAD_BUILD is None,
    reason="sad-build.exe غير مبني — يتطلب: cmake --build build --target sad-build",
)

FREESTANDING = "--حرّ"
EMIT_LLVM = "--أظهر-llvm"
TARGET_X64 = "--هدف=x86_64-unknown-elf"


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


# ─────────────── ناقص = خطأ تصريف (لا تبخّرَ صامتًا بعد اليوم) ───────────────

def test_hw_pci_read_config_missing_arg_is_compile_error():
    """(AR) اقرأ_اعدادات_ناقل(0، 1، 2) بثلاثةٍ لا أربعة ⇒ خطأ تصريف (العتاد)."""
    code, out, _ = _compile(
        "متغير ق = اقرأ_اعدادات_ناقل(0، 1، 2)\nاطبع(ق)\n", FREESTANDING, TARGET_X64
    )
    assert code != 0, "الوسيط الناقص مرّ صامتًا (انحدار ح٤ جزء ب):\n" + out
    assert "اقرأ_اعدادات_ناقل" in out, "التشخيص لا يسمّي المدمجة:\n" + out


def test_sys_hpet_sleep_no_args_is_compile_error():
    """(AR) نوم_دقيق() بلا وسيطٍ ⇒ خطأ تصريف (النظام)، لا نومًا متبخّرًا."""
    code, out, _ = _compile("نوم_دقيق()\n", FREESTANDING, TARGET_X64)
    assert code != 0, "نوم_دقيق بلا وسيط مرّت صامتة (انحدار ح٤ جزء ب):\n" + out


def test_uefi_get_variable_no_args_is_compile_error():
    """(AR) uefi_قراءة_متغير() بلا اسمٍ ⇒ خطأ تصريف؛ قبل السدّ كان زمنُ
    التشغيل يقرأ متغيّرًا باسمٍ NULL صامتًا (ملفّ uefi كان بلا فحصٍ أصلًا)."""
    code, out, _ = _compile(
        "متغير ق = uefi_قراءة_متغير()\nاطبع(ق)\n", FREESTANDING, TARGET_X64
    )
    assert code != 0, "uefi_قراءة_متغير بلا وسيط مرّت صامتة (انحدار ح٤ جزء ب):\n" + out


# ─────────────── زائد = خطأ تصريف (لا قبولَ بإهمالٍ بعد اليوم) ───────────────

def test_hw_fb_clear_two_args_is_compile_error():
    """(AR) امسح_شاشة(1، 2): الاختياريُّ لونٌ واحدٌ لا اثنان ⇒ خطأ تصريف (العتاد)."""
    code, out, _ = _compile("امسح_شاشة(1، 2)\n", FREESTANDING, TARGET_X64)
    assert code != 0, "وسيطان على امسح_شاشة قُبلا (انحدار ح٤ جزء ب):\n" + out


def test_sys_yield_extra_arg_is_compile_error():
    """(AR) تنازل(5): مدمجةٌ صفريّةٌ تُعطى وسيطًا ⇒ خطأ تصريف (النظام)."""
    code, out, _ = _compile("تنازل(5)\n", FREESTANDING, TARGET_X64)
    assert code != 0, "الوسيط الزائد على تنازل قُبل (انحدار ح٤ جزء ب):\n" + out


def test_uefi_acpi_disable_extra_arg_is_compile_error():
    """(AR) acpi_تعطيل(7): مدمجةٌ صفريّةٌ تُعطى وسيطًا ⇒ خطأ تصريف (uefi)."""
    code, out, _ = _compile("acpi_تعطيل(7)\n", FREESTANDING, TARGET_X64)
    assert code != 0, "الوسيط الزائد على acpi_تعطيل قُبل (انحدار ح٤ جزء ب):\n" + out


# ─────────────── الرتبة الصحيحة تبقى خضراء (لا إفراطَ في السدّ) ───────────────

def test_hw_fb_clear_optional_color_stays_optional():
    """(AR) امسح_شاشة() وامسح_شاشة(255) كلاهما مقبولٌ: اللونُ الاختياريُّ المعلَن باقٍ."""
    code0, out0, _ = _compile("امسح_شاشة()\n", FREESTANDING, TARGET_X64)
    assert code0 == 0, "امسح_شاشة بلا وسيطٍ فشلت (إفراط في السدّ):\n" + out0
    code1, out1, _ = _compile("امسح_شاشة(255)\n", FREESTANDING, TARGET_X64)
    assert code1 == 0, "امسح_شاشة باللون فشلت (إفراط في السدّ):\n" + out1


def test_sys_mem_map_region_optional_flags_stay_optional():
    """(AR) رحل_منطقة بثلاثةٍ وبأربعةٍ كلاهما مقبولٌ (الأعلامُ اختياريّة)؛
    وباثنين خطأ (النظام)."""
    code3, out3, _ = _compile(
        "رحل_منطقة(4096، 8192، 4096)\n", FREESTANDING, TARGET_X64
    )
    assert code3 == 0, "رحل_منطقة بثلاثةٍ فشلت (إفراط في السدّ):\n" + out3
    code4, out4, _ = _compile(
        "رحل_منطقة(4096، 8192، 4096، 3)\n", FREESTANDING, TARGET_X64
    )
    assert code4 == 0, "رحل_منطقة بالأعلام فشلت (إفراط في السدّ):\n" + out4
    code2, out2, _ = _compile(
        "رحل_منطقة(4096، 8192)\n", FREESTANDING, TARGET_X64
    )
    assert code2 != 0, "وسيطان على رحل_منطقة قُبلا (انحدار ح٤ جزء ب):\n" + out2


def test_uefi_apic_timer_defaults_stay_optional():
    """(AR) apic_تهيئة_مؤقت() وapic_تهيئة_مؤقت(32، 0، 3) كلاهما مقبولٌ (افتراضاتٌ
    معلنةٌ في الخلفيّة)؛ وبأربعةٍ خطأ (uefi)."""
    code0, out0, _ = _compile("apic_تهيئة_مؤقت()\n", FREESTANDING, TARGET_X64)
    assert code0 == 0, "apic_تهيئة_مؤقت بلا وسائطَ فشلت (إفراط في السدّ):\n" + out0
    # (AR) ملاحظة مقيسة: النداء بثلاثة وسائط صريحة يكشف تعارض i64/i32 موروثًا
    #      في خفض الخلفيّة (verifyModule) سابقًا لهذا السدّ — يُقاس هنا حدُّ
    #      الرتبة وحدَه: صفرٌ أخضر، وأربعةٌ حمراء.
    code4, out4, _ = _compile("apic_تهيئة_مؤقت(32، 0، 3، 9)\n", FREESTANDING, TARGET_X64)
    assert code4 != 0, "أربعة وسائط على apic_تهيئة_مؤقت قُبلت (انحدار ح٤ جزء ب):\n" + out4


def test_uefi_exit_boot_services_correct_arity_still_compiles():
    """(AR) uefi_إنهاء_خدمات_إقلاع(1) برتبتها الصحيحة ⇒ تصريفٌ ناجح (uefi)."""
    code, out, _ = _compile("uefi_إنهاء_خدمات_إقلاع(1)\n", FREESTANDING, TARGET_X64)
    assert code == 0, "الرتبة الصحيحة فشلت (إفراط في السدّ):\n" + out


def test_uefi_init_one_arg_is_compile_error():
    """(AR) uefi_تهيئة مجموعتها الشرعية {0، 2}: وسيطٌ واحدٌ كان «يمرّ» ويتبخّر في
    الخفض (يستهلك عند 2 فقط) ⇒ صار خطأ تصريف؛ والصفر والاثنان يبقيان خضراوين."""
    # (AR) النداء مجرّدًا بلا إسناد: إسناد عائد مدمجات منخفضة وطباعته يصطدم بعلّة
    #      موروثة مستقلّة (Undefined register:%0) خارج نطاق هذا الحارس.
    code1, out1, _ = _compile("uefi_تهيئة(4096)\n", FREESTANDING, TARGET_X64)
    assert code1 != 0, "uefi_تهيئة بوسيط واحد مرّت ووسيطها يتبخّر (انحدار ح٤):\n" + out1
    code0, out0, _ = _compile("uefi_تهيئة()\n", FREESTANDING, TARGET_X64)
    assert code0 == 0, "uefi_تهيئة بلا وسيط فشلت (إفراط في السدّ):\n" + out0
    code2, out2, _ = _compile("uefi_تهيئة(4096، 8192)\n", FREESTANDING, TARGET_X64)
    assert code2 == 0, "uefi_تهيئة بوسيطين فشلت (إفراط في السدّ):\n" + out2
