"""
============================================================================
(AR) test_gen_error_messages_v5.py — اختبارات مسار V5 (--yaml-dir)
     يغطّي load_error_directory + الدمج + التحقّق الدلالي على
     language-truth/errors/*.yaml الفعلية (الصيغة المتداخلة المُعتمَدة).
(EN) Tests for the V5 directory-merge path of gen_error_messages.py.
     Covers EM-V5-1/T1 (ADR-DOCS-V4-005).
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

from _lib.loader import load_schema  # noqa: E402
from gen_error_messages import (  # noqa: E402
    CATEGORY_ORDER,
    extract_enum_values,
    load_error_directory,
    sort_entries,
    validate_semantically,
)

ERR_DIR = ROOT / "language-truth" / "errors"
SCHEMA_PATH = ROOT / "language-truth" / "_schemas" / "error.schema.json"
ENUM_HEADER = ROOT / "shared" / "errors" / "include" / "error_codes.h"

pytestmark = pytest.mark.skipif(
    not (ERR_DIR.exists() and SCHEMA_PATH.exists() and ENUM_HEADER.exists()),
    reason="مصادر V5 غير مكتملة في هذا الفرع",
)


@pytest.fixture(scope="module")
def merged():
    """(AR) تحميل ودمج كل ملفات الفئات V5."""
    return load_error_directory(ERR_DIR, load_schema(SCHEMA_PATH))


@pytest.fixture(scope="module")
def enum_values():
    return extract_enum_values(ENUM_HEADER)


def test_merged_has_expected_keys(merged):
    """(AR) البنية الموحّدة الناتجة تحوي المفاتيح المتوقّعة."""
    assert set(merged.keys()) >= {"version", "categories", "errors"}
    assert isinstance(merged["errors"], list) and merged["errors"]
    assert isinstance(merged["categories"], dict) and merged["categories"]


def test_categories_have_prefix_and_official_order(merged):
    """(AR) كل فئة مدموجة لها prefix، والترتيب رسمي (CW-27)."""
    cats = list(merged["categories"].keys())
    # الترتيب يطابق ترتيب CATEGORY_ORDER (مع تجاهل الفئات الغائبة)
    expected = [c for c in CATEGORY_ORDER if c in cats]
    assert cats == expected, f"ترتيب الفئات غير رسمي: {cats}"
    for cat, meta in merged["categories"].items():
        assert meta.get("prefix"), f"الفئة '{cat}' بلا prefix"


def test_all_codes_unique(merged):
    """(AR) لا تكرار code بعد الدمج."""
    codes = [e["code"] for e in merged["errors"]]
    dups = {c for c in codes if codes.count(c) > 1}
    assert not dups, f"رموز مكرّرة عبر الملفات: {dups}"


def test_ids_match_category_prefix(merged):
    """(AR) كل id يبدأ بـ prefix فئته."""
    for e in merged["errors"]:
        prefix = merged["categories"][e["category"]]["prefix"]
        assert e["id"].startswith(prefix), f"{e['id']} لا يبدأ بـ {prefix}"


def test_semantic_validation_passes(merged, enum_values):
    """(AR) التحقّق الدلالي الكامل (6 فحوص) يمرّ بلا أخطاء — جوهر EM-V5-1."""
    merged["errors"] = sort_entries(merged)
    errors = validate_semantically(merged, enum_values)
    assert not errors, "فشل التحقّق الدلالي:\n" + "\n".join(errors)


def test_codes_equal_enum(merged, enum_values):
    """(AR) رموز V5 == enum تماماً (orphan=0, missing=0) — شرط حلّ BLK-1."""
    codes = {e["code"] for e in merged["errors"]}
    assert codes - enum_values == set(), f"orphan: {sorted(codes - enum_values)[:5]}"
    assert enum_values - codes == set(), f"missing: {sorted(enum_values - codes)[:5]}"


def test_merge_is_idempotent(merged):
    """(AR) D-5: الترتيب ثابت — إعادة الفرز لا تغيّر النتيجة."""
    once = [e["code"] for e in sort_entries(merged)]
    twice = [e["code"] for e in sort_entries({**merged, "errors": sort_entries(merged)})]
    assert once == twice


def _brief_ar(entry):
    """(AR) يعيد brief.ar كنص (يدعم dict أو str)."""
    b = entry.get("brief")
    return b.get("ar", "") if isinstance(b, dict) else (b or "")


def test_em_cpp_builtin_error_placeholders(merged):
    """
    (AR) حارس انحدار لترحيل EM-CPP (PR #24): رموز أخطاء الدوال المضمنة المُرحَّلة
         يجب أن تعرض اسم الدالة/التفصيل عبر placeholders — لا رسائل عامة.
    (EN) Regression guard for EM-CPP: migrated builtin error codes must surface
         the function name / detail via placeholders, not generic messages.
    """
    by_code = {e["code"]: e for e in merged["errors"]}

    # RUN_BUILTIN_REQUIRES_ARG يعرض اسم الدالة {func} (يُحقَن من ctx.error)
    req = by_code.get("RUN_BUILTIN_REQUIRES_ARG")
    assert req, "RUN_BUILTIN_REQUIRES_ARG مفقود من الكتالوج"
    assert "{func}" in _brief_ar(req), "brief لـ RUN_BUILTIN_REQUIRES_ARG لا يحوي {func}"
    assert "func" in (req.get("placeholders") or []), "placeholders بلا 'func'"

    # RUN_ASSERTION_FAILED يحفظ تفصيل التأكيد {detail}
    af = by_code.get("RUN_ASSERTION_FAILED")
    assert af, "RUN_ASSERTION_FAILED مفقود"
    assert "{detail}" in _brief_ar(af), "brief لـ RUN_ASSERTION_FAILED لا يحوي {detail}"
    assert "detail" in (af.get("placeholders") or []), "placeholders بلا 'detail'"


def test_em_cpp7_internal_ice_codes(merged):
    """
    (AR) حارس EM-CPP-7 (PR #34/#35): أخطاء المترجم الداخلية (ICE) من الكتالوج.
         يجب أن تكون فئتها 'internal'، وتحمل {detail} (بيانات لا نثر)، وتوسم
         صراحةً كخطأ مترجم يُبلَّغ — حتى لا يتسرّب نصّ رسالة مكتوب يدوياً في C++.
    (EN) EM-CPP-7 guard: internal compiler errors (ICE) come from the catalog —
         category 'internal', {detail} placeholder, explicit "report" framing.
    """
    by_code = {e["code"]: e for e in merged["errors"]}
    for code in ("INT_COMPILER_NULL_IR", "INT_COMPILER_INVALID_OPERANDS"):
        e = by_code.get(code)
        assert e, f"{code} مفقود من الكتالوج (internal.yaml)"
        assert e["category"] == "internal", f"{code} ليس في فئة internal"
        assert "{detail}" in _brief_ar(e), f"brief لـ {code} لا يحوي {{detail}}"
        assert "detail" in (e.get("placeholders") or []), f"{code}: placeholders بلا 'detail'"
        # (AR) وسم «خطأ مترجم — أبلِغ» موجود (لا يُعامَل كخطأ مستخدِم)
        assert "مترجم" in _brief_ar(e), f"{code}: لا يوسم كخطأ مترجم داخلي"
