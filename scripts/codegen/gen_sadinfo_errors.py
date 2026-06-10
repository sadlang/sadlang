"""
============================================================================
(AR) gen_sadinfo_errors.py — إسقاط كتالوج أخطاء sadinfo من language-truth/errors/
(EN) gen_sadinfo_errors.py — Project sadinfo error catalog from language-truth/errors/
----------------------------------------------------------------------------
(AR) القرار: ADR-DOCS-V4-005 (ق-فرعي-2) — sadinfo إسقاط مُولَّد لا مصدر يدوي.
     يقرأ language-truth/errors/*.yaml (المصدر الموحَّد، الصيغة المتداخلة)
     ويُنتج كياناً لكل خطأ: <out-dir>/error_<slug>/_index.yaml
     مطابقاً لـ data/_schemas/error.schema.json (v1: schema_version/id/kind/
     code/name/category/severity/since/owners).

تعيين الفئات (محسوم 2026-06-10، EM-V5-2):
     lexical→lexer، syntactic→parser، semantic→type، runtime→runtime،
     ownership→runtime، import→linker، io→io.
الرمز E####: تسلسلي عالمي بترتيب (category, id).
حقول بلا مصدر في V5: owners=["@sadlang-core"]، since="0.1.0"، severity="error".
============================================================================
"""
from __future__ import annotations

import argparse
import sys
import traceback
from pathlib import Path

_codegen_dir = str(Path(__file__).parent)
if _codegen_dir not in sys.path:
    sys.path.insert(0, _codegen_dir)

import re  # noqa: E402

from _lib.loader import load_schema, validate_schema  # noqa: E402
from gen_error_messages import (  # noqa: E402
    CATEGORY_ORDER,
    load_error_directory,
    sort_entries,
)

# (AR) رقم id من المعرّف النصي: "LEX001" → 1
_ID_NUM_RE = re.compile(r"(\d+)$")

# (AR) تعيين فئات V5 → فئات sadinfo (enum: lexer/parser/type/runtime/security/linker/io/concurrency)
CATEGORY_MAP = {
    "lexical": "lexer",
    "syntactic": "parser",
    "semantic": "type",
    "runtime": "runtime",
    "ownership": "runtime",   # (AR) أخطاء الملكية وقت تشغيل
    "import": "linker",       # (AR) الاستيراد = ربط
    "io": "io",
    "internal": "runtime",    # (AR) احتياطي إن أُعيد internal
}
DEFAULT_OWNERS = ["@sadlang-core"]
DEFAULT_SINCE = "0.1.0"
DEFAULT_SEVERITY = "error"


def stable_code(entry: dict) -> str:
    """
    (AR) رمز E#### **مستقر** مشتقّ من (فئة V5 + رقم id) — لا موضعي.
         E = (فهرس الفئة في CATEGORY_ORDER × 1000) + رقم id.
         مثال: LEX001 (lexical=0) → E0001؛ SYN002 (syntactic=1) → E1002.
         مستقر: لا يتغيّر بإدراج/حذف أخطاء أخرى (يحفظ عقد «مفتاح أساسي للأدوات»).
    """
    cat = entry.get("category", "")
    try:
        cat_idx = CATEGORY_ORDER.index(cat)
    except ValueError as exc:
        raise RuntimeError(f"فئة V5 غير معروفة '{cat}' للرمز {entry.get('code')}") from exc
    m = _ID_NUM_RE.search(entry.get("id", ""))
    if not m:
        raise RuntimeError(f"id بلا رقم: {entry.get('id')}")
    id_num = int(m.group(1))
    if id_num >= 1000:
        raise RuntimeError(f"رقم id ≥ 1000 يكسر مدى الفئة: {entry.get('id')}")
    return f"E{cat_idx * 1000 + id_num:04d}"


def project_entity(entry: dict, _unused: int = 0) -> dict:
    """(AR) يحوّل خطأ V5 واحداً إلى كيان sadinfo (الرمز مستقر، لا موضعي)."""
    code = entry["code"]                      # مثل LEX_INVALID_CHARACTER
    slug = code.lower()                       # lex_invalid_character
    v5_cat = entry.get("category", "")
    sad_cat = CATEGORY_MAP.get(v5_cat)
    if sad_cat is None:
        raise RuntimeError(f"فئة V5 غير مُعيَّنة '{v5_cat}' للرمز {code}")
    name_ar = ""
    title = entry.get("title")
    if isinstance(title, dict):
        name_ar = title.get("ar", "") or ""
    return {
        "schema_version": 1,
        "id": f"error.{slug}",
        "kind": "error",
        "code": stable_code(entry),
        "name": name_ar or slug,
        "category": sad_cat,
        "severity": DEFAULT_SEVERITY,
        "since": DEFAULT_SINCE,
        "owners": list(DEFAULT_OWNERS),
    }


def _yaml_dump(d: dict) -> str:
    """(AR) إخراج YAML ثابت الترتيب (idempotent) بلا اعتماد ترتيب dict عشوائي."""
    import yaml
    return yaml.safe_dump(d, allow_unicode=True, sort_keys=False, default_flow_style=False)


def main() -> int:
    p = argparse.ArgumentParser(description="sadinfo error catalog projector (V5)")
    p.add_argument("--yaml-dir", required=True, type=Path,
                   help="language-truth/errors/")
    p.add_argument("--src-schema", required=True, type=Path,
                   help="schema المصدر المتداخل (language-truth/_schemas/error.schema.json)")
    p.add_argument("--out-schema", required=True, type=Path,
                   help="schema كيان sadinfo (data/_schemas/error.schema.json)")
    p.add_argument("--out-dir", required=True, type=Path,
                   help="مجلد إخراج الكيانات المُولَّدة")
    p.add_argument("--quiet", action="store_true")
    args = p.parse_args()

    try:
        src_schema = load_schema(args.src_schema)
        out_schema = load_schema(args.out_schema)
        data = load_error_directory(args.yaml_dir, src_schema)
        data["errors"] = sort_entries(data)
    except Exception as e:
        print(f"[gen_sadinfo_errors] FATAL: {e}", file=sys.stderr)
        if not args.quiet:
            traceback.print_exc(file=sys.stderr)
        return 2

    # (AR) إسقاط: حساب كل الكيانات أولاً (تحقّق schema قبل أي كتابة).
    args.out_dir.mkdir(parents=True, exist_ok=True)
    entities = []
    for entry in data["errors"]:
        ent = project_entity(entry)
        try:
            validate_schema(ent, out_schema)
        except Exception as e:
            print(f"[gen_sadinfo_errors] FATAL: كيان '{ent['id']}' يفشل schema: {e}",
                  file=sys.stderr)
            return 1
        entities.append((f"error_{entry['code'].lower()}", ent))

    # (AR) مرآة وفيّة: احذف الكيانات اليتيمة المُولَّدة سابقاً (error_*) غير الموجودة الآن.
    wanted = {name for name, _ in entities}
    import shutil
    for child in args.out_dir.iterdir():
        if child.is_dir() and child.name.startswith("error_") and child.name not in wanted:
            shutil.rmtree(child)

    for name, ent in entities:
        ent_dir = args.out_dir / name
        ent_dir.mkdir(parents=True, exist_ok=True)
        (ent_dir / "_index.yaml").write_text(_yaml_dump(ent), encoding="utf-8")

    if not args.quiet:
        print(f"[gen_sadinfo_errors] ✓ {len(entities)} كيان → {args.out_dir}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
