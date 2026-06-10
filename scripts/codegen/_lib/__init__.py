"""
============================================================================
(AR) حزمة _lib — المكتبة المشتركة بين جميع مولِّدات الكود في scripts/codegen/
(EN) _lib package — Shared library for all scripts/codegen/ generators
----------------------------------------------------------------------------
(AR) أُعيدت كتابتها نظيفاً بعد فقدان المصدر الأصلي (الاستيراد النسبي صحّح).
     context.py (GenerationContext) غير مُستخدَم من أي مولِّد حالياً ومصدره
     المفكوك معطوب — فاستُبعِد من إعادة التصدير حتى يُعاد بناؤه عند الحاجة.
(EN) Cleanly rewritten after source loss (relative imports fixed). context.py
     is unused by current generators and its decompiled source is broken —
     excluded from re-exports until rebuilt when needed.
============================================================================
"""
from .emit import cpp_string_literal, vector_literal, write_if_changed
from .loader import load_yaml, load_schema, validate_schema
from .validator import validate_no_duplicates, validate_canonical_utf8

__all__ = [
    "cpp_string_literal",
    "vector_literal",
    "write_if_changed",
    "load_yaml",
    "load_schema",
    "validate_schema",
    "validate_no_duplicates",
    "validate_canonical_utf8",
]
