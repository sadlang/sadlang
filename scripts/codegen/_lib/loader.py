"""
============================================================================
(AR) loader.py — تحميل ملفات YAML و JSON Schema والتحقق منهما
(EN) loader.py — YAML and JSON Schema loading and validation
----------------------------------------------------------------------------
(AR) يوفر دوال تحميل موحَّدة تُستخدم من gen_keywords.py وغيرها.
     أي خطأ في القراءة أو التحقق يُرفع كاستثناء يُسبِّب فشل البناء.
(EN) Provides unified loading functions used by gen_keywords.py and others.
     Any reading or validation error raises an exception that fails the build.
----------------------------------------------------------------------------
(AR) أُعيدت كتابته نظيفاً بعد فقدان المصدر الأصلي (كان غير متعقّب في git).
     المنطق مُستعاد من .pyc المفكوك ومن مواصفات الاستخدام في المولِّدات.
(EN) Cleanly rewritten after the original (untracked) source was lost.
============================================================================
"""
from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

try:
    import yaml
except ImportError:
    print(
        "[_lib.loader] FATAL: PyYAML not installed.\n"
        "[_lib.loader] Install: pip install pyyaml jsonschema",
        file=sys.stderr,
    )
    sys.exit(2)

try:
    from jsonschema import validate as _jsonschema_validate
except ImportError:
    print(
        "[_lib.loader] FATAL: jsonschema not installed.\n"
        "[_lib.loader] Install: pip install pyyaml jsonschema",
        file=sys.stderr,
    )
    sys.exit(2)


def load_yaml(yaml_path: Path) -> dict[str, Any]:
    """
    (AR) يقرأ ملف YAML بترميز UTF-8 ويتحقق أن الجذر كائن (mapping).
         يُرفع ValueError إذا لم يكن الجذر dict.
    (EN) Reads a YAML file as UTF-8 and asserts the root is a mapping.
    """
    text = Path(yaml_path).read_text(encoding="utf-8")
    data = yaml.safe_load(text)
    if not isinstance(data, dict):
        raise ValueError(
            f"YAML root must be a mapping, got {type(data).__name__} "
            f"in file: {yaml_path}"
        )
    return data


def load_schema(schema_path: Path) -> dict[str, Any]:
    """(AR) يقرأ ملف JSON Schema بترميز UTF-8. / (EN) Reads a JSON Schema as UTF-8."""
    return json.loads(Path(schema_path).read_text(encoding="utf-8"))


def validate_schema(data: Any, schema: dict[str, Any]) -> None:
    """
    (AR) يتحقق من بنية البيانات ضد JSON Schema. يُرفع ValidationError عند الفشل.
         الفحوصات الدلالية الإضافية (تكرار، shadda) تبقى في كل مولِّد.
    (EN) Validates data against JSON Schema. Raises ValidationError on failure.
    """
    _jsonschema_validate(data, schema)
