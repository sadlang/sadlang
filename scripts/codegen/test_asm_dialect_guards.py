# -*- coding: utf-8 -*-
"""
(AR) بذورٌ يجب أن **تُخفِق**: حرّاسُ `gen_asm_dialect.py` مُحاكَمون بطفرة.

     حارسٌ لا يُحمِّر ليس حارسًا، و«موجودٌ» ليس «مقيسًا». فكلُّ طفرةٍ هنا تحقن
     في نسخةٍ من المعجم عطبًا يعرفه الحارسُ، وتشترط أن يخرج المولّدُ برمزٍ غيرِ
     صفريٍّ **وبرسالةٍ تسمّي العطب** — لا بأيّ حمرةٍ كانت. ومعها ضابطٌ موجب:
     المعاجمُ بلا طفرةٍ تُولَّد بنجاح، وإلّا كان الأحمرُ أثرَ أداةٍ لا أثرَ حارس.

     ⚠️ ولا تُمَسّ شجرةُ المستودع: تُنسَخ المعاجمُ الثلاثةُ إلى مجلّدٍ مؤقّتٍ
     وتُحقَن النسخة. فاختبارٌ يعدّل مصدرَ الحقيقةِ ثمّ ينهار يترك الشجرةَ مسمومة.

(EN) Seeds that must fail: the gen_asm_dialect guards are judged by mutation.
     Each seed injects a defect into a COPY of the lexicons and requires a
     non-zero exit whose message names the defect — plus a positive control so
     that a broken harness cannot read as a firing guard. The repository tree is
     never mutated.
"""
from __future__ import annotations

import shutil
import subprocess
import sys
from pathlib import Path

import pytest

الجذر = Path(__file__).resolve().parents[2]
مجلد_المعاجم = الجذر / "language-truth" / "dialects" / "assembly_mnemonics"
# (AR) تُقرأ من المجلّد لا تُكتب قائمةً: المولّد يحاكم قائمةَ `architectures` في
#      اللهجة مقابلَ المعاجم المُمرَّرة في الاتّجاهين، فقائمةٌ يدويّةٌ هنا تجعل
#      **إضافةَ معماريّةٍ** تُحمِّرُ بذورَ الحرّاس — وهو أحمرُ لا يدلّ على شيء.
#      وقد وقع ذلك فعلًا عند إضافة riscv64 رابعةً.
# (EN) Read from the directory: a hand-written list here turns *adding an
#      architecture* into a red seed — which happened when riscv64 landed.
المعاجم = sorted(p.name for p in مجلد_المعاجم.glob("*.yaml"))


def _ولّد(مجلد: Path, ترويسة: Path) -> subprocess.CompletedProcess:
    """(AR) يُشغّل المولّد على معاجمِ `مجلد` ويعيد النتيجة كاملةً."""
    أمر = [
        sys.executable, str(الجذر / "scripts" / "codegen" / "gen_asm_dialect.py"),
        "--dialect", str(الجذر / "language-truth" / "dialects" / "assembly.yaml"),
        "--mnemonics", *[str(مجلد / اسم) for اسم in المعاجم],
        "--dialect-schema", str(الجذر / "language-truth" / "_schemas" / "dialect.schema.json"),
        "--mnemonics-schema",
        str(الجذر / "language-truth" / "_schemas" / "assembly_mnemonics.schema.json"),
        "--header", str(ترويسة),
        "--quiet",
    ]
    return subprocess.run(أمر, capture_output=True, text=True,
                          encoding="utf-8", errors="replace", cwd=str(الجذر))


@pytest.fixture()
def معاجم_مؤقّتة(tmp_path: Path) -> Path:
    """(AR) نسخةٌ كاملةٌ من المعاجم الثلاثة — الطفرةُ تقع عليها لا على الشجرة."""
    وجهة = tmp_path / "mnemonics"
    وجهة.mkdir()
    for اسم in المعاجم:
        shutil.copy2(مجلد_المعاجم / اسم, وجهة / اسم)
    return وجهة


def _احقن(مجلد: Path, اسم: str, قديم: str, جديد: str) -> None:
    """(AR) حقنٌ بمرساةٍ فريدة — ومرساةٌ لا تُطابِق تُفشِل الاختبارَ لا تُهمَل."""
    مسار = مجلد / اسم
    نص = مسار.read_text(encoding="utf-8")
    assert نص.count(قديم) == 1, (
        "مرساةُ الطفرة غير فريدة في %s (%d) — البذرةُ لم تُزرَع، فالأخضرُ لا يعني شيئًا"
        % (اسم, نص.count(قديم))
    )
    مسار.write_text(نص.replace(قديم, جديد, 1), encoding="utf-8")


def test_control_unmutated_lexicons_generate(معاجم_مؤقّتة: Path, tmp_path: Path):
    """(AR) ضابطٌ موجب: بلا طفرةٍ يُولَّد بنجاح — وإلّا فالحمرةُ أثرُ الأداة."""
    نتيجة = _ولّد(معاجم_مؤقّتة, tmp_path / "h.h")
    assert نتيجة.returncode == 0, (
        "المولّدُ أخفقَ على معاجمَ سليمة — كلُّ حمرةٍ بعدَ هذا لا تُنسَب إلى حارس:\n"
        + (نتيجة.stdout or "") + (نتيجة.stderr or "")
    )


def test_label_mixed_with_register_is_rejected(معاجم_مؤقّتة: Path, tmp_path: Path):
    """(AR) لصيقةٌ مع معاملٍ آخر (cbz) ⇒ رفض.

    الخافضُ يفرد للّصيقةِ مسارًا مشروطًا بـ`operandClasses == "l"` تمامًا، فمنمنمةٌ
    تخلطها بسجلٍّ تُصيَّرُ لصيقتُها نصًّا خامًّا في مسارِ البيانات — لا مرجعَ قفز.
    """
    _احقن(معاجم_مؤقّتة, "aarch64.yaml",
          "    - { ar: اقفز,                  en: b,    operands: [لصيقة] }",
          "    - { ar: اقفز_إذا_صفر_سجل, en: cbz, operands: [مصدر, لصيقة] }\n"
          "    - { ar: اقفز,                  en: b,    operands: [لصيقة] }")
    نتيجة = _ولّد(معاجم_مؤقّتة, tmp_path / "h.h")
    خرج = (نتيجة.stdout or "") + (نتيجة.stderr or "")
    assert نتيجة.returncode != 0, "المولّدُ قبِلَ لصيقةً مخلوطةً بسجلّ:\n" + خرج
    assert "تخلط معامل «لصيقة»" in خرج, "حمَّرَ برسالةٍ لا تسمّي العطب:\n" + خرج


def test_operand_head_on_a_label_mnemonic_is_rejected(معاجم_مؤقّتة: Path, tmp_path: Path):
    """(AR) رأسٌ على منمنمةِ لصيقة ⇒ رفض: مسارُ اللصيقةِ ينتهي قبل المعاملات فيُسقطه."""
    _احقن(معاجم_مؤقّتة, "aarch64.yaml",
          "    - { ar: اقفز,                  en: b,    operands: [لصيقة] }",
          "    - { ar: اقفز,                  en: b,    operands: [لصيقة], "
          "operand_head: [\"x0\"] }")
    نتيجة = _ولّد(معاجم_مؤقّتة, tmp_path / "h.h")
    خرج = (نتيجة.stdout or "") + (نتيجة.stderr or "")
    assert نتيجة.returncode != 0, "المولّدُ قبِلَ رأسًا على منمنمةِ لصيقة:\n" + خرج
    assert "operand_head مع معامل لصيقة/بادئة" in خرج, (
        "حمَّرَ برسالةٍ لا تسمّي العطب:\n" + خرج
    )


def test_operand_head_containing_the_list_separator_is_rejected(
        معاجم_مؤقّتة: Path, tmp_path: Path):
    """(AR) عنصرُ رأسٍ يحوي فاصلَ القائمةِ نفسَه ⇒ رفض: يفسد التفكيك في الخافض."""
    _احقن(معاجم_مؤقّتة, "aarch64.yaml",
          'operand_head: ["vbar_el1"], note: "تنصيب',
          'operand_head: ["vbar_el1,x1"], note: "تنصيب')
    نتيجة = _ولّد(معاجم_مؤقّتة, tmp_path / "h.h")
    خرج = (نتيجة.stdout or "") + (نتيجة.stderr or "")
    assert نتيجة.returncode != 0, "المولّدُ قبِلَ رأسًا يحوي الفاصل:\n" + خرج
    assert "يحوي فاصل القائمة" in خرج, "حمَّرَ برسالةٍ لا تسمّي العطب:\n" + خرج


def test_operand_head_on_a_source_first_flavour_is_rejected(
        معاجم_مؤقّتة: Path, tmp_path: Path):
    """(AR) رأسٌ على نكهةٍ تعكس الترتيب (i686/AT&T) ⇒ رفض.

    🔑 وهذه البذرةُ تُزرَع في **معجمٍ آخر**: الحارسُ الذي يُحاكَم في معجمٍ واحدٍ
    قد يكون مربوطًا بمعماريّةٍ بعينِها. فزرعُها في i686 يُثبِت أنّه على النكهةِ
    لا على الاسم.
    """
    _احقن(معاجم_مؤقّتة, "i686.yaml",
          "en: nop,   operands: [] }",
          "en: nop,   operands: [], operand_head: [\"%eax\"] }")
    نتيجة = _ولّد(معاجم_مؤقّتة, tmp_path / "h.h")
    خرج = (نتيجة.stdout or "") + (نتيجة.stderr or "")
    assert نتيجة.returncode != 0, "المولّدُ قبِلَ رأسًا على نكهةٍ تعكس الترتيب:\n" + خرج
    assert "source_first" in خرج, "حمَّرَ برسالةٍ لا تسمّي العطب:\n" + خرج
