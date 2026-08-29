"""
============================================================================
(AR) test_gen_types.py — اختبارات مُولِّد الأنواع (S-TS-P0.5)
     يتحقّق أن SadTypeKind المُولَّد من types.yaml يطابق العدد والترتيب،
     ويُمنع التكرار، ويُحترَم المخطّط.
(EN) Tests for gen_types.py — validates the generated SadTypeKind enum
     matches types.yaml (count, order, no duplicates, schema).
============================================================================
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
_codegen = str(ROOT / "scripts" / "codegen")
if _codegen not in sys.path:
    sys.path.insert(0, _codegen)

from _lib.loader import load_yaml, load_schema, validate_schema  # noqa: E402
from gen_types import emit_header, DEFAULT_INIT_VOCAB  # noqa: E402

YAML_PATH = ROOT / "language-truth" / "types.yaml"
SCHEMA_PATH = ROOT / "language-truth" / "_schemas" / "type.schema.json"
GEN_HEADER = ROOT / "shared" / "types" / "generated" / "sad_type_kind_generated.h"

pytestmark = pytest.mark.skipif(
    not (YAML_PATH.exists() and SCHEMA_PATH.exists()),
    reason="types.yaml أو المخطّط غير موجود",
)



def _enum_block(text: str, enum_name: str) -> str:
    """(AR) جسمُ تعدادٍ بعينِه — فمرساةُ الاختبارِ التعدادُ المقصودُ لا أيُّ سطرٍ
       يُشبِه عضوَ تعداد. أُضيف حين صار في الترويسةِ تعدادان، فكان النمطُ العامُّ
       يلتقط أعضاءَ الثاني ويحسبها من الأوّل.
       (EN) Body of ONE named enum — the test's anchor is the intended enum, not
       any line that looks like an enumerator. Added once the header carried two
       enums and the loose pattern started swallowing the second one's members."""
    m = re.search(
        r"enum class " + re.escape(enum_name) + r" : int\n\s*\{\n(.*?)\n\s*\};",
        text,
        re.DOTALL,
    )
    assert m, f"لم يُعثَر على تعداد {enum_name}"
    return m.group(1)

def _types() -> list[dict]:
    return load_yaml(YAML_PATH)["types"]


def test_yaml_loads_and_nonempty():
    types = _types()
    assert isinstance(types, list) and len(types) >= 49


def test_every_entry_matches_schema():
    schema = load_schema(SCHEMA_PATH)
    for entry in _types():
        validate_schema(entry, schema)  # raises on failure


def test_no_duplicate_kinds_or_ids():
    types = _types()
    kinds = [t["kind"] for t in types]
    ids = [t["id"] for t in types]
    assert len(kinds) == len(set(kinds)), "تكرار في kind"
    assert len(ids) == len(set(ids)), "تكرار في id"


def test_surface_types_have_token():
    for t in _types():
        if t.get("surface"):
            assert t.get("token_type", "").startswith("TYPE_"), t["id"]


def test_emitted_enum_order_matches_yaml():
    types = _types()
    header = emit_header(types)
    # (AR) استخرج معرّفات enum بالترتيب من جسمِ SadTypeKind وحدَه
    emitted = re.findall(
        r"^\s+([A-Z][A-Za-z0-9]*),", _enum_block(header, "SadTypeKind"), re.MULTILINE
    )
    assert emitted == [t["kind"] for t in types]


def test_arabic_name_function_emitted():
    # (AR) دالة أسماء نوع() المولَّدة — مصدر حقيقة واحد للمحرّكين. تتحقّق من:
    #      وجود الدالة، وتطبيق typeof_ar فوق word (Class/Struct → كائن لا صنف/بنية).
    # (EN) Generated نوع() name function — one SoT for both engines. Asserts the
    #      function exists and typeof_ar overrides word (Class/Struct → كائن).
    types = _types()
    header = emit_header(types)
    assert "sadTypeKindArabicName" in header
    # (AR) رقم (word) لـInteger، وكائن (typeof_ar) لـClass — مُرمَّزة UTF-8 بـ\x
    word_int = "".join(f"\\x{b:02x}" for b in "رقم".encode("utf-8"))
    typeof_class = "".join(f"\\x{b:02x}" for b in "كائن".encode("utf-8"))
    assert f"case SadTypeKind::Integer: return \"{word_int}\"" in header
    assert f"case SadTypeKind::Class: return \"{typeof_class}\"" in header


@pytest.mark.skipif(not GEN_HEADER.exists(), reason="الترويسة المُولَّدة غير موجودة")
def test_generated_header_in_sync_with_yaml():
    types = _types()
    on_disk = GEN_HEADER.read_text(encoding="utf-8")
    emitted = re.findall(
        r"^\s+([A-Z][A-Za-z0-9]*),",
        _enum_block(emit_header(types), "SadTypeKind"),
        re.MULTILINE,
    )
    disk_kinds = re.findall(
        r"^\s+([A-Z][A-Za-z0-9]*),\s+///<", _enum_block(on_disk, "SadTypeKind"), re.MULTILINE
    )
    assert disk_kinds == emitted, "الترويسة المُولَّدة غير متزامنة — أعد التوليد"


# ============================================================================
# (AR) القيمةُ الافتراضيّةُ للتهيئة (default_init) — الحقلُ الذي وحّد ستَّ نسخ
# (EN) default_init — the field that unified six hand-written tables
# ============================================================================


def test_every_type_declares_a_known_default_init():
    """(AR) كلُّ صفٍّ يُعلن قيمةً افتراضيّةً من المفرداتِ المعروفة — فلا نوعَ
       يدخل مصدرَ الحقيقةِ بلا قرار، ولا لفظَ يدخل بلا مستهلكٍ يعرفه."""
    known = {word for word, _cxx, _ar, _en in DEFAULT_INIT_VOCAB}
    for entry in _types():
        assert "default_init" in entry, f"{entry['id']} بلا default_init"
        assert entry["default_init"] in known, (
            f"{entry['id']}: default_init={entry['default_init']!r} ليس في المفردات {sorted(known)}"
        )


def test_no_bare_yaml_null_in_default_init():
    """(AR) 🔑 فخُّ التحويلِ الصامت: `default_init: null` عاريةً يحوّلها المُحمِّلُ
       إلى العدم، فيصير الحقلُ None ويسقط الصفُّ صامتًا بلا رسالة. اللفظُ
       المقصودُ `null_value`، وهذا الحارسُ يمنع عودةَ الشكلِ العاري.
       (EN) A bare `null` is coerced to None by the YAML loader, so the row loses
       its field silently. The intended word is `null_value`."""
    raw = YAML_PATH.read_text(encoding="utf-8")
    assert "default_init: null," not in raw, "لفظُ null عاريًا — استعمل null_value"
    assert all(entry.get("default_init") is not None for entry in _types())


def test_default_init_table_matches_yaml_row_for_row():
    """(AR) الجدولُ المُولَّدُ مفهرسٌ بقيمةِ التعداد — فترتيبُه هو ترتيبُ الصفوفِ
       نفسُه، وأيُّ انزياحٍ يجعل نوعًا يقرأ افتراضيَّ جارِه."""
    types = _types()
    header = emit_header(types)
    cxx_of = {word: cxx for word, cxx, _ar, _en in DEFAULT_INIT_VOCAB}
    table = re.search(
        r"SAD_TYPE_DEFAULT_INIT_TABLE = \{\{\n(.*?)\n\s*\}\};", header, re.DOTALL
    )
    assert table, "جدولُ القيمِ الافتراضيّةِ غيرُ مبثوث"
    emitted = re.findall(r"SadDefaultInit::([A-Za-z]+),", table.group(1))
    expected = [cxx_of[entry["default_init"]] for entry in types]
    assert emitted == expected
    assert len(emitted) == len(types), "طولُ الجدولِ يخالف عددَ الأنواع"


def test_emitted_vocabulary_count_matches_source():
    header = emit_header(_types())
    assert f"SAD_DEFAULT_INIT_COUNT = {len(DEFAULT_INIT_VOCAB)};" in header
    members = re.findall(
        r"^\s+([A-Z][A-Za-z0-9]*),", _enum_block(header, "SadDefaultInit"), re.MULTILINE
    )
    assert members == [cxx for _w, cxx, _ar, _en in DEFAULT_INIT_VOCAB]
    assert members[0] == "Unspecified", "«Unspecified» يجب أن تكون صفرًا"


def test_unknown_default_init_breaks_generation():
    """(AR) 🔑 بذرةٌ يجب أن تُحمِّر: لفظٌ لا يعرفه المُولِّدُ **يكسر التوليد** بدل
       أن يسقط في احتياطٍ صامت. ويسبقها شاهدٌ موجَبٌ على الصفوفِ نفسِها كي تكون
       الحمرةُ أثرَ الطفرةِ لا أثرَ أيِّ شيءٍ آخر.
       (EN) A word the generator does not know must BREAK generation. A positive
       control on the unmutated rows proves the redness comes from the mutation."""
    types = [dict(entry) for entry in _types()]
    emit_header(types)  # (AR) شاهدٌ موجَب: يمرّ بلا طفرة

    types[0]["default_init"] = "لفظ_غير_معروف"
    with pytest.raises(ValueError, match="DEFAULT_INIT_VOCAB"):
        emit_header(types)


def test_prose_type_count_matches_row_count():
    """(AR) 🔑 عددٌ منثورٌ نسخةٌ ثانيةٌ لحقيقةٍ تُعَدّ: نثرُ الترويسةِ كان يقول
       «49 نوعًا» والصفوفُ ٥٢ — انجرافٌ بقي حتّى قِيس. يُحرَس هنا بالعدِّ لا
       بالمراجعة.
       (EN) A prose number is a second copy of a counted fact — the header said
       «49» while the rows were 52. Guarded by counting, not by review."""
    data = load_yaml(YAML_PATH)
    rows = len(data["types"])
    # (AR) نقيد المطابقة بعدد الأنواع المقوس «(N نوعًا)/(N)» لا بكل رقم في
    #      النثر — اشتراط «كل رقم = عدد الصفوف» كان يحمر زورا لأي رقم آخر
    #      مستقبلي (مثل «64») في الوصف (هشاشة رصدتها المراجعة).
    # (EN) Match only the parenthesized type count, not every number in the
    #      prose — requiring ALL numbers to equal the row count falsely
    #      reddened any future unrelated number (e.g. «64») in the text.
    patterns = {
        "description_ar": r"\((\d+)\s*نوعًا\)",
        "description_en": r"kinds \((\d+)\)",
    }
    for key, pattern in patterns.items():
        matches = [int(n) for n in re.findall(pattern, data[key])]
        assert matches, f"{key} لا يحمل عددَ الأنواعِ المقوسَ يُقارَن"
        assert all(n == rows for n in matches), (
            f"{key} يقول {matches} والصفوفُ {rows} — أعد ضبطَ النثر"
        )


@pytest.mark.skipif(not GEN_HEADER.exists(), reason="الترويسة المُولَّدة غير موجودة")
def test_generated_header_default_init_in_sync():
    """(AR) الترويسةُ على القرصِ مُودَعةٌ في git، فقد تبلى إن عُدِّل YAML بلا توليد."""
    on_disk = GEN_HEADER.read_text(encoding="utf-8")
    fresh = emit_header(_types())
    for marker in ("enum class SadDefaultInit", "SAD_TYPE_DEFAULT_INIT_TABLE", "sadTypeKindDefaultInit"):
        assert marker in on_disk, f"{marker} غائبٌ عن الترويسةِ المُودَعة — أعد التوليد"
    disk_table = re.search(
        r"SAD_TYPE_DEFAULT_INIT_TABLE = \{\{\n(.*?)\n\s*\}\};", on_disk, re.DOTALL
    )
    fresh_table = re.search(
        r"SAD_TYPE_DEFAULT_INIT_TABLE = \{\{\n(.*?)\n\s*\}\};", fresh, re.DOTALL
    )
    assert disk_table and fresh_table
    assert re.findall(r"SadDefaultInit::([A-Za-z]+),", disk_table.group(1)) == re.findall(
        r"SadDefaultInit::([A-Za-z]+),", fresh_table.group(1)
    ), "جدولُ القيمِ الافتراضيّةِ غيرُ متزامن — أعد التوليد"


def test_not_a_slot_kinds_are_proven_rejected_by_a_behaviour_test():
    """(AR) 🔑 «لا خانةَ تحمله» دعوى **سلوكيّةٌ لا تصنيفيّة**: لا يكفي أن تُكتَب
       في مصدرِ الحقيقة، بل يلزم أن يرفضَها المحرّكان فعلًا. فهذا الحارسُ يربط
       كلَّ نوعٍ موسومٍ `not_a_slot` باختبارِ سلوكٍ يُصرِّح به خانةً ويشترط
       `SEM040` بعينِه — لا `@expect_error` عاريًا يمرّ على أيِّ خطأ.
       فإن وُسِم نوعٌ جديدٌ بها بلا برهانٍ سلوكيٍّ، احمرّ هذا الحارس.
       (EN) «no slot can hold it» is a BEHAVIOURAL claim, not a taxonomy label:
       writing it in the SoT is not enough — both engines must actually reject
       it. This guard ties every `not_a_slot` kind to a behaviour test that
       declares it as a slot and demands SEM040 specifically."""
    words = [entry["word"] for entry in _types() if entry["default_init"] == "not_a_slot"]
    assert words, "لا نوعَ موسومٌ not_a_slot — أزِلِ الحارسَ أو أعِدِ الوسم"

    behavior_dir = ROOT / "tests" / "behavior"
    assert behavior_dir.is_dir(), "مجلّدُ اختباراتِ السلوكِ غيرُ موجود"

    rejection_tests = {}
    for path in behavior_dir.rglob("*.\u0635"):
        text = path.read_text(encoding="utf-8", errors="replace")
        if "@expect_error: SEM040" in text:
            rejection_tests[path.name] = text
    assert rejection_tests, "لا اختبارَ يشترط SEM040 بعينِه"

    for word in words:
        declared = re.compile(
            r"^[ \t]*(?:\u0645\u062a\u063a\u064a\u0631[ \t]+)?" + re.escape(word) + r"[ \t]+\S",
            re.MULTILINE,
        )
        hits = [name for name, text in rejection_tests.items() if declared.search(text)]
        assert hits, (
            f"«{word}» موسومٌ not_a_slot ولا اختبارَ سلوكٍ يُصرِّح به خانةً "
            f"ويشترط SEM040 — الدعوى غيرُ مقيسة"
        )


def test_not_a_slot_is_distinct_from_unspecified():
    """(AR) الفرقُ ليس تسمية: «unspecified» دَينٌ ينتظر قرارًا، و«not_a_slot»
       استحالةٌ لا قرارَ فيها. ودمجُهما يجعل قارئًا يحسب على «فراغ» دَينًا."""
    vocab = {word for word, _cxx, _ar, _en in DEFAULT_INIT_VOCAB}
    assert {"unspecified", "not_a_slot"} <= vocab
    by_word = {entry["word"]: entry for entry in _types()}
    assert by_word["\u0641\u0631\u0627\u063a"]["default_init"] == "not_a_slot", (
        "«فراغ» نوعُ إرجاعٍ لا نوعُ خانة — وسمُه unspecified يجعله دَينًا وهو ليس كذلك"
    )


# ============================================================================
# (AR) الصفةُ العدديّة — الإشارةُ والعرضُ مُعلَنَين، وما يُشتَقُّ منهما.
# (EN) Numeric traits — declared signedness and width, and their derivations.
# ============================================================================

_NUMERIC_WORDS_EXPECTED = {
    # (AR) لقطةٌ مكتوبةٌ باليدِ عمدًا — حارسٌ ثنائيُّ الاتّجاه. اشتقاقُها من
    #      types.yaml يجعلُها تُوافقُه دائمًا فلا تحرسُ شيئًا.
    # (EN) Hand-written on purpose: deriving it from types.yaml would make it
    #      agree by construction and guard nothing.
    # (AR) حُدِّثَت ٢٩ آب ٢٠٢٦ بموجةِ توحيدِ الأسماء: «عدد*» ⇐ «رقم*»،
    #      «طبيعي64» ⇐ «طبيعي»، وحُذِفَ «بايت» و«عدد64» و«عشري64».
    "رقم": "signed",                  # رقم
    "عشري": "float",            # عشري
    "رقم8": "signed",                 # رقم8
    "رقم16": "signed",                # رقم16
    "رقم32": "signed",                # رقم32
    "طبيعي8": "unsigned",  # طبيعي8
    "طبيعي16": "unsigned", # طبيعي16
    "طبيعي32": "unsigned", # طبيعي32
    "طبيعي": "unsigned",   # طبيعي
    "عشري32": "float",          # عشري32
}


def test_every_numeric_kind_declares_its_signedness():
    """(AR) الإشارةُ حقيقةٌ **مُعلَنة**: كلُّ نوعٍ عدديٍّ يقولها في types.yaml،
       ولا يُستنبَطُ شيءٌ منها من هجاءِ الاسم. واستنباطُها من «UInt» يجعل
       الحقيقةَ رهنَ التسمية، فيكفي نوعٌ يُسمّى بغيرِ النمطِ ليصمتَ الاستنباط."""
    by_word = {entry["word"]: entry for entry in _types()}
    for word, expected in _NUMERIC_WORDS_EXPECTED.items():
        assert word in by_word, f"«{word}» مفقودٌ من types.yaml"
        assert by_word[word].get("numeric") == expected, (
            f"«{word}» يجب أن يُعلِن numeric: {expected}"
        )


def test_numeric_kinds_are_exactly_the_declared_set():
    """(AR) الحارسُ ثنائيُّ الاتّجاه: لا نوعَ عدديٍّ بلا إعلان، ولا إعلانَ
       على نوعٍ ليس عددًا. والحارسُ أحاديُّ الاتّجاهِ يمرّ على الزيادةِ صامتًا."""
    declared = {e["word"] for e in _types() if e.get("numeric")}
    assert declared == set(_NUMERIC_WORDS_EXPECTED), (
        f"مجموعةُ الأنواعِ العدديّةِ تغيّرت: زائد={declared - set(_NUMERIC_WORDS_EXPECTED)} "
        f"ناقص={set(_NUMERIC_WORDS_EXPECTED) - declared}"
    )


def test_numeric_class_table_matches_yaml_row_for_row():
    """(AR) الجدولُ المولَّدُ صفٌّ بصفٍّ على ترتيبِ types.yaml — لا انزياحَ
       يخلط إشارةَ نوعٍ بنوع."""
    from gen_types import NUMERIC_CLASS_VOCAB

    cxx_of = {word: cxx for word, cxx, _ar, _en in NUMERIC_CLASS_VOCAB}
    types = _types()
    header = emit_header(types)
    block = header.split("SAD_TYPE_NUMERIC_CLASS_TABLE = {{", 1)[1].split("}};", 1)[0]
    cells = re.findall(r"SadNumericClass::(\w+),", block)
    assert len(cells) == len(types), (
        f"طولُ الجدولِ {len(cells)} لا يساوي عددَ الأنواعِ {len(types)}"
    )
    for cell, entry in zip(cells, types):
        assert cell == cxx_of[entry.get("numeric", "__absent__")], (
            f"انزياحٌ عند {entry['kind']}: الجدولُ يقول {cell}"
        )


def test_numeric_without_width_breaks_generation():
    """(AR) عددٌ بلا عرضٍ مُعلَنٍ لا يُبتَرُ ولا يُحسَبُ مداه — فهو دَينٌ صامت،
       والمولِّدُ يرفضه بدل أن يولّدَ جدولًا يكذب."""
    types = [dict(entry) for entry in _types()]
    victim = next(entry for entry in types if entry.get("numeric"))
    victim.pop("size_bytes", None)
    with pytest.raises(ValueError, match="size_bytes"):
        emit_header(types)


def test_unknown_numeric_class_breaks_generation():
    """(AR) صنفٌ عدديٌّ خارجَ المفردات يكسر التوليد — فلا يسقط في ذراعٍ تُخمِّن."""
    types = [dict(entry) for entry in _types()]
    victim = next(entry for entry in types if entry.get("numeric"))
    victim["numeric"] = "\u0645\u062c\u0647\u0648\u0644"
    with pytest.raises(ValueError, match="NUMERIC_CLASS_VOCAB"):
        emit_header(types)


def test_dominance_rule_is_generated_and_general():
    """(AR) 🔑 قاعدةُ الهيمنةِ مولَّدةٌ عامّةٌ لا سُلَّمَ أسماءٍ محصورًا.

       والسُّلَّمُ القديم (`عشري ← طبيعي ← بايت ← رقم`) يحمل خللًا كامنًا:
       يَسِمُ `بايت + رقم` بـ«بايت»، فلحظةَ تنفيذِ البترِ يصيرُ ٢٠٠ + ١٠٠
       يساوي ٤٤ بدلَ ٣٠٠ المقيسةِ اليوم — انحدارٌ صامتٌ يولدُ مع الإصلاح.
       فالقاعدةُ ههنا: الأعرضُ يفوز، وعندَ التساوي يفوزُ اللا-موقَّع."""
    header = emit_header(_types())
    assert "sadNumericDominantKind" in header, "قاعدةُ الهيمنةِ غيرُ مولَّدة"
    body = header.split("sadNumericDominantKind", 1)[1]
    # (AR) لا سُلَّمَ أسماءٍ: لا ذكرَ لـUInt64 ولا Byte في جسدِ القاعدة
    rule = body.split("\n        }", 1)[0]
    for banned in ("SadTypeKind::UInt64", "SadTypeKind::Byte"):
        assert banned not in rule, (
            f"القاعدةُ ما زالت محصورةً باسمٍ بعينِه ({banned}) — فهي تعمى عن كلِّ نوعٍ سواه"
        )


def test_normalize_and_range_helpers_are_generated():
    """(AR) البترُ عمليّةٌ واحدةٌ مولَّدةٌ تُطبَّقُ عندَ كلِّ معبر — لا نسخةٌ لكلِّ معبر.
       فمعبرٌ واحدٌ منسيٌّ يُبطِلُ عقدَ النوعِ كلَّه (`ك.ح = 300` تُعطي ٣٠٠)."""
    header = emit_header(_types())
    for symbol in (
        "sadTypeKindNumericClass",
        "sadTypeKindIsUnsignedInteger",
        "sadTypeKindNumericBits",
        "sadTypeKindIntegerMin",
        "sadTypeKindIntegerMax",
        "sadTypeKindNormalizeInteger",
        "sadTypeKindIntegerFits",
    ):
        assert symbol in header, f"«{symbol}» غيرُ مولَّد"
