# -*- coding: utf-8 -*-
"""
(AR) حارسُ عقدِ ترتيبِ الخريطة — «المرورُ بترتيبِ الإدخال».

     العقد (مصدرُ الحقيقة، `gr.expr.map_literal`): «مفاتيح» و«قيم» و«لكل»
     والطباعةُ تمرُّ بالترتيبِ الذي أُدخِلَت به المفاتيح.

     🔑 **ولمَ حارسٌ ساكنٌ وليست الاختباراتُ وحدَها كافية:** ثلاثةُ محرّكاتٍ
     تُنفِّذُ هذا العقد، وواحدٌ منها لا يستطيعُ اختبارٌ سلوكيٌّ أن يحكمَ عليه
     اليوم — زمنُ التشغيلِ الأصليُّ (`sad_embedded_runtime.c`) خريطتُه
     **عنونةٌ مفتوحةٌ بـFNV-1a**، وترتيبُ خاناتِها ترتيبُ سَلّةٍ لا ترتيبُ إدخال.
     وهو ناجٍ اليومَ لسببٍ واحدٍ فقط: **لا دالّةَ مرورٍ فيه أصلًا** — لا مفاتيحَ
     ولا قيمَ ولا تعدادَ خانات، إنشاءٌ وضبطٌ وجلبٌ لا غير. فأوّلُ مَن يُضيفُ
     المرورَ هناك يخرقُ العقدَ **صامتًا** ولا اختبارَ يراه، لأنّ لا اختبارَ
     يُشغَّلُ على تلك الخلفيّةِ بعد.

     ⚠️ فالمرساةُ هنا **الفعلُ لا هجاؤه**: يُخفِقُ الحارسُ متى ظهرت في زمنِ
     التشغيلِ الأصليِّ دالّةٌ تمرُّ على خاناتِ الخريطة — أيًّا كان اسمُها —
     ليصيرَ القرارُ واعيًا: إمّا أن تُرتَّبَ الحاوية، أو يُوثَّقَ الخرقُ عمدًا.
     تعليقٌ يقولُ «انتبهْ لاحقًا» دَينٌ ينتظرُ أن يكذب؛ وهذا حارسٌ ينطق.

     ISSUE-180 (خريطةُ المفسّرِ كانت مُهشَّرة) · ISSUE-181 (ثقبُ الحذفِ في
     خلفيّةِ LLVM). كلاهما مُصلَحٌ؛ وهذا الحارسُ يمنعُ عودةَ العلّةِ من البابِ
     الثالثِ الذي لم يُفتَح بعد.

(EN) Map ordering contract guard. Three engines implement "iteration follows
     insertion order". Two are covered by behaviour tests; the third — the native
     embedded runtime — is an FNV-1a open-addressing table whose slot order is
     bucket order, and it only escapes the contract today because it has NO
     iteration function at all (create/set/get only). Whoever adds one there
     would break the contract silently, since no behaviour test runs on that
     backend yet. So this guard anchors on the ACT, not on a spelling: it fails
     the moment a slot-walking function appears in the native runtime, forcing a
     deliberate decision instead of a comment that waits to become a lie.
"""
import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]
NATIVE_RUNTIME = REPO_ROOT / "tools" / "compiler" / "runtime" / "sad_embedded_runtime.c"
VALUE_HEADER = REPO_ROOT / "shared" / "types" / "include" / "value.h"
GRAMMAR_SOT = REPO_ROOT / "language-truth" / "grammar" / "40_expressions.yaml"

# (AR) الفعلُ المحظور: حلقةٌ تمرُّ على `map->slots` — هذا هو المرورُ مهما سُمّيت
#      الدالّة. ويُستثنى ما هو موجودٌ اليومَ بالضرورة: البحثُ عن خانةٍ بعينِها
#      (`set`/`get`) وإعادةُ التهشير، وكلاهما لا يُصدِرُ ترتيبًا إلى المستخدم.
# (EN) The forbidden act is a loop over map->slots, whatever the function is called.
#      Today's necessary walkers — per-key probing and rehash — emit no order to the user.
SLOT_WALK = re.compile(r"map->slots|->slots\s*\[")
ALLOWED_WALKERS = ("sad_map_rehash", "__sad_map_set", "__sad_map_get", "__sad_map_create")

FUNCTION_HEAD = re.compile(r"^[A-Za-z_][A-Za-z0-9_ \*]*\b(\w+)\s*\([^;]*\)\s*$")


def _functions_touching_slots(source: str):
    """(AR) يُرجِعُ أسماءَ الدوالِّ التي يمسُّ جسمُها خاناتِ الخريطة."""
    touching, current = [], None
    for line in source.split("\n"):
        head = FUNCTION_HEAD.match(line)
        if head:
            current = head.group(1)
        if current and SLOT_WALK.search(line):
            if current not in touching:
                touching.append(current)
    return touching


def test_native_runtime_has_no_unordered_iteration():
    """(AR) لا دالّةَ مرورٍ جديدةً على خاناتِ الخريطةِ في زمنِ التشغيلِ الأصليّ."""
    source = NATIVE_RUNTIME.read_text(encoding="utf-8", errors="replace")
    unexpected = [
        name for name in _functions_touching_slots(source)
        if not any(name.startswith(allowed) for allowed in ALLOWED_WALKERS)
    ]
    assert not unexpected, (
        "دالّةٌ جديدةٌ تمرُّ على خاناتِ الخريطةِ في زمنِ التشغيلِ الأصليّ: "
        + ", ".join(unexpected)
        + " — وخريطتُه مُهشَّرةٌ بترتيبِ سَلّة، فالمرورُ عبرَها يخرقُ عقدَ ترتيبِ "
        "الإدخال. رتِّبِ الحاويةَ أو وثِّقِ الخرقَ في ISSUE-180."
    )


def test_interpreter_map_is_insertion_ordered():
    """(AR) خريطةُ المفسّرِ ليست مُهشَّرةً — العقدُ يُقرأُ من الإعلانِ لا من نيّةٍ."""
    source = VALUE_HEADER.read_text(encoding="utf-8", errors="replace")
    declaration = [line for line in source.split("\n") if "using MapType" in line]
    assert declaration, "لم يُعثَر على إعلانِ MapType في value.h"
    assert "InsertionOrderedMap" in declaration[0], (
        "MapType ليست مرتّبةً بالإدخال: " + declaration[0].strip()
        + " — ترتيبُ المرورِ يعودُ حينَها قرارَ مكتبةِ المنصّةِ لا قرارَ اللغة (ISSUE-180)."
    )


def test_contract_is_written_in_the_source_of_truth():
    """(AR) العقدُ مكتوبٌ حيثُ يُقرأ — وإلّا كان سلوكًا لا وعدًا."""
    source = GRAMMAR_SOT.read_text(encoding="utf-8", errors="replace")
    assert "ترتيبُ المرورِ عقدٌ" in source, (
        "عقدُ ترتيبِ الخريطةِ غائبٌ عن مصدرِ الحقيقة (gr.expr.map_literal): "
        "سلوكٌ بلا وعدٍ مكتوبٍ يُنقَضُ بلا لومٍ في أوّلِ إعادةِ كتابة."
    )


def main():
    checks = [
        test_native_runtime_has_no_unordered_iteration,
        test_interpreter_map_is_insertion_ordered,
        test_contract_is_written_in_the_source_of_truth,
    ]
    failures = 0
    for check in checks:
        try:
            check()
            print("  ✅ " + check.__name__)
        except AssertionError as failure:
            failures += 1
            print("  ❌ " + check.__name__ + ": " + str(failure))
    if failures:
        print("❌ حارسُ ترتيبِ الخريطة: " + str(failures) + " إخفاق")
        return 1
    print("✅ حارسُ ترتيبِ الخريطة: العقدُ قائمٌ في المحرّكاتِ الثلاثة")
    return 0


if __name__ == "__main__":
    sys.exit(main())
