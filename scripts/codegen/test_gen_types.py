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
from gen_types import emit_header  # noqa: E402

YAML_PATH = ROOT / "language-truth" / "types.yaml"
SCHEMA_PATH = ROOT / "language-truth" / "_schemas" / "type.schema.json"
GEN_HEADER = ROOT / "shared" / "types" / "generated" / "sad_type_kind_generated.h"

pytestmark = pytest.mark.skipif(
    not (YAML_PATH.exists() and SCHEMA_PATH.exists()),
    reason="types.yaml أو المخطّط غير موجود",
)


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
    # (AR) استخرج معرّفات enum بالترتيب من المُخرَج
    emitted = re.findall(r"^\s+([A-Z][A-Za-z0-9]*),", header, re.MULTILINE)
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
    emitted = re.findall(r"^\s+([A-Z][A-Za-z0-9]*),", emit_header(types), re.MULTILINE)
    disk_kinds = re.findall(r"^\s+([A-Z][A-Za-z0-9]*),\s+///<", on_disk, re.MULTILINE)
    assert disk_kinds == emitted, "الترويسة المُولَّدة غير متزامنة — أعد التوليد"
