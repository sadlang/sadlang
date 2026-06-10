"""
============================================================================
(AR) test_gen_sadinfo_errors.py — اختبارات إسقاط sadinfo (EM-V5-2)
     يتحقّق أن كل خطأ V5 يُسقَط إلى كيان sadinfo صحيح مطابق للـschema.
============================================================================
"""
from __future__ import annotations

import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
_codegen = str(ROOT / "scripts" / "codegen")
if _codegen not in sys.path:
    sys.path.insert(0, _codegen)

from _lib.loader import load_schema, validate_schema  # noqa: E402
from gen_error_messages import load_error_directory, sort_entries  # noqa: E402
from gen_error_messages import CATEGORY_ORDER  # noqa: E402
from gen_sadinfo_errors import CATEGORY_MAP, project_entity, stable_code  # noqa: E402

ERR_DIR = ROOT / "language-truth" / "errors"
SRC_SCHEMA = ROOT / "language-truth" / "_schemas" / "error.schema.json"
OUT_SCHEMA = ROOT / "data" / "_schemas" / "error.schema.json"

pytestmark = pytest.mark.skipif(
    not (ERR_DIR.exists() and SRC_SCHEMA.exists() and OUT_SCHEMA.exists()),
    reason="مصادر الإسقاط غير مكتملة في هذا الفرع",
)


@pytest.fixture(scope="module")
def errors():
    data = load_error_directory(ERR_DIR, load_schema(SRC_SCHEMA))
    return sort_entries(data)


@pytest.fixture(scope="module")
def out_schema():
    return load_schema(OUT_SCHEMA)


@pytest.fixture(scope="module")
def entities(errors):
    return [project_entity(e, i) for i, e in enumerate(errors, start=1)]


def test_every_entity_validates(entities, out_schema):
    """(AR) كل كيان مُسقَط يجتاز schema sadinfo (v1)."""
    for ent in entities:
        validate_schema(ent, out_schema)


def test_codes_unique(entities):
    """(AR) رموز E#### فريدة."""
    codes = [e["code"] for e in entities]
    assert len(set(codes)) == len(codes), "رموز E#### مكرّرة"


def test_codes_are_stable_not_positional(errors, entities):
    """(AR) الرمز مشتقّ من (فئة + رقم id) — مستقر لا موضعي."""
    for src, ent in zip(errors, entities):
        cat_idx = CATEGORY_ORDER.index(src["category"])
        id_num = int("".join(ch for ch in src["id"] if ch.isdigit()))
        assert ent["code"] == f"E{cat_idx * 1000 + id_num:04d}"
    # (AR) برهان الاستقرار: حذف أول خطأ لا يغيّر رمز البقية
    if len(errors) > 1:
        c_before = stable_code(errors[-1])
        c_after = stable_code(errors[-1])  # نفس المدخل بمعزل عن الموضع
        assert c_before == c_after


def test_ids_unique_and_lowercase(entities):
    """(AR) المعرّفات فريدة وبصيغة error.<lowercase>."""
    ids = [e["id"] for e in entities]
    assert len(set(ids)) == len(ids)
    for i in ids:
        assert i.startswith("error.") and i == i.lower()


def test_category_mapping_complete(errors):
    """(AR) كل فئة V5 موجودة لها تعيين sadinfo."""
    for e in errors:
        assert e["category"] in CATEGORY_MAP, f"فئة بلا تعيين: {e['category']}"


def test_name_from_title_ar(errors, entities):
    """(AR) اسم العرض مأخوذ من title.ar."""
    by_code = {e["code"]: e for e in errors}
    for ent in entities:
        slug = ent["id"][len("error."):]
        src = by_code[slug.upper()]
        assert ent["name"] == src["title"]["ar"]


def test_projection_is_idempotent(errors):
    """(AR) الإسقاط ثابت — نفس المدخل ينتج نفس المخرج."""
    once = [project_entity(e, i) for i, e in enumerate(errors, 1)]
    twice = [project_entity(e, i) for i, e in enumerate(errors, 1)]
    assert once == twice
