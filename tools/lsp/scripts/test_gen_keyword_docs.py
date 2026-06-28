"""
============================================================================
(AR) test_gen_keyword_docs.py — اختبارات مولّد أوصاف hover (نظام أداة LSP)
     يثبّت المبدأ الحوكميّ: الأداة **تستهلك** مصدر الحقيقة (keywords.yaml)
     للتحقّق فقط ولا توسّعه. يغطّي:
       • الحالة الإيجابية: توليد ناجح + توسيع الأسماء البديلة + شمول كلّ
         الكلمات الرئيسية غير المدمجة (لا فجوة تغطية).
       • الحتميّة: التوليد مرّتين على نفس المدخل يعطي نفس المخرج (بايتيًّا).
       • الحالات السلبية (حارس الاستهلاك): مفتاحٌ ليس كلمةً معجمية، مفتاحٌ
         نوعٌ مدمج، وصفٌ متعدّد الأسطر/بلا علامة نهاية/بفراغ، version مفقود،
         تصادم مفتاحٍ منبثقٍ من اسمٍ بديل.
(EN) Tests for gen_keyword_docs.py — the LSP-owned hover-docs generator that
     CONSUMES the SoT for validation only. Covers the positive path, generator
     determinism, full keyword coverage, and the negative consumption guards.
============================================================================
"""
from __future__ import annotations

import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]
_scripts = str(Path(__file__).resolve().parent)
if _scripts not in sys.path:
    sys.path.insert(0, _scripts)

import gen_keyword_docs as gkd  # noqa: E402

DOCS_PATH = ROOT / "tools" / "lsp" / "data" / "keyword_docs.yaml"
SOT_PATH = ROOT / "language-truth" / "keywords.yaml"

pytestmark = pytest.mark.skipif(
    not (DOCS_PATH.exists() and SOT_PATH.exists()),
    reason="keyword_docs.yaml أو keywords.yaml غير موجود",
)


def _sot():
    return gkd.collect_sot_keywords(SOT_PATH)


def _docs():
    return gkd.load_yaml(DOCS_PATH)["descriptions"]


# ── الحالة الإيجابية ──────────────────────────────────────────────────────

def test_every_doc_key_is_a_real_keyword():
    """(AR) المبدأ الحوكميّ: كلّ مفتاح وصف كلمةٌ معجمية حقيقية غير مدمجة."""
    primary, builtins = _sot()
    gkd.validate_descriptions(_docs(), primary, builtins)  # raises on failure


def test_full_coverage_no_gap():
    """(AR) كلّ كلمة رئيسية غير مدمجة موصوفة — لا فجوة تغطية."""
    primary, _ = _sot()
    docs = _docs()
    missing = [w for w in primary if w not in docs]
    assert not missing, f"كلمات بلا وصف: {missing}"


def test_no_doc_key_outside_lexicon():
    """(AR) لا مفتاح وصفٍ خارج معجم الكلمات الرئيسية (لا توسعة صامتة)."""
    primary, _ = _sot()
    extra = [w for w in _docs() if w not in primary]
    assert not extra, f"مفاتيح ليست كلماتٍ رئيسية: {extra}"


def test_aliases_expanded_into_header():
    """(AR) الأسماء البديلة من المعجم تأخذ وصف رئيسيّها (مثل «اذا»←«إذا»)."""
    primary, _ = _sot()
    docs = _docs()
    header = gkd.emit_header(docs, primary)
    # «اذا» اسمٌ بديل لـ«إذا» في المعجم ⇒ يجب أن يظهر في الترويسة.
    assert '"اذا"' in header or "اذا" in header


def test_generator_is_deterministic():
    """(AR) التوليد مرّتين على نفس المدخل ⇒ نفس المخرج بايتيًّا (حتميّة)."""
    primary, _ = _sot()
    docs = _docs()
    assert gkd.emit_header(docs, primary) == gkd.emit_header(docs, primary)


def test_header_is_self_contained():
    """(AR) الترويسة header-only: pragma once + الدالة المنشورة."""
    primary, _ = _sot()
    header = gkd.emit_header(_docs(), primary)
    assert "#pragma once" in header
    assert "keywordDocDescriptionAr" in header


# ── الحالات السلبية (حارس الاستهلاك) ─────────────────────────────────────

def test_reject_non_keyword_key():
    primary, builtins = _sot()
    with pytest.raises(ValueError, match="not a primary keyword"):
        gkd.validate_descriptions({"ليست_كلمة": "وصف."},
                                  primary, builtins)


def test_reject_builtin_type_key():
    primary, builtins = _sot()
    assert builtins, "يُفترض وجود أنواع مدمجة في المعجم"
    a_builtin = next(iter(builtins))
    with pytest.raises(ValueError, match="builtin type"):
        gkd.validate_descriptions({a_builtin: "وصف."}, primary, builtins)


def test_reject_multiline_description():
    primary, builtins = _sot()
    word = next(iter(primary))
    with pytest.raises(ValueError, match="single line"):
        gkd.validate_descriptions({word: "سطر١.\nسطر٢."}, primary, builtins)


def test_reject_missing_terminator():
    primary, builtins = _sot()
    word = next(iter(primary))
    with pytest.raises(ValueError, match="sentence terminator"):
        gkd.validate_descriptions({word: "بلا علامة نهاية"}, primary, builtins)


def test_reject_surrounding_whitespace():
    primary, builtins = _sot()
    word = next(iter(primary))
    with pytest.raises(ValueError, match="whitespace"):
        gkd.validate_descriptions({word: " وصف. "}, primary, builtins)


def test_reject_empty_description():
    primary, builtins = _sot()
    word = next(iter(primary))
    with pytest.raises(ValueError, match="non-empty"):
        gkd.validate_descriptions({word: ""}, primary, builtins)


def test_collision_guard_in_emit():
    """(AR) لو انبثق اسمٌ بديلٌ يساوي كلمةً موصوفة أخرى ⇒ فشلٌ صريح لا فقدٌ صامت."""
    word = "أ"  # رئيسيّة وهمية
    primary = {word: [word]}  # اسمٌ بديل يساوي رئيسيّه ⇒ تصادم
    with pytest.raises(ValueError, match="collision"):
        gkd.emit_header({word: "وصف."}, primary)


# ── سلامة literal C++ ────────────────────────────────────────────────────

def test_cpp_literal_escapes_specials():
    assert gkd.cpp_string_literal('a"b\\c') == '"a\\"b\\\\c"'
    assert gkd.cpp_string_literal("x\ty") == '"x\\ty"'
    assert gkd.cpp_string_literal("\x0c") == '"\\x0c"'  # \f ⇒ هروب سداسيّ


def test_cpp_literal_preserves_utf8_codepoints():
    # المحارف العربية تبقى كما هي (تُكتَب كبايتات UTF-8 عند الحفظ).
    assert gkd.cpp_string_literal("دالة") == '"دالة"'


if __name__ == "__main__":
    sys.exit(pytest.main([__file__, "-q"]))
