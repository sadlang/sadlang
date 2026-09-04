"""
============================================================================
(AR) gen_error_messages.py — توليد كتالوج رسائل أخطاء C++ من error_messages.yaml
(EN) gen_error_messages.py — C++ error messages catalog codegen from YAML
----------------------------------------------------------------------------
(AR) يقرأ language-truth/errors/*.yaml (V5، عبر --yaml-dir) أو ملفاً واحداً (V4، --yaml)
     ويُنتج ملفين:
       - shared/errors/generated/error_messages_generated.h
       - shared/errors/generated/error_messages_generated.cpp

القرارات (PRD D-1..D-5):
  D-1: YAML SOT + JSON Schema (draft-07)
  D-2: ترتيب ثابت = category ثم id (CW-27)
  D-3: نص فارغ → nullptr في C++
  D-5: مولِّد idempotent
============================================================================
"""
from __future__ import annotations

import argparse
import re
import sys
import traceback
from pathlib import Path
from typing import Any

# (AR) إضافة مجلد codegen إلى sys.path
_codegen_dir = str(Path(__file__).parent)
if _codegen_dir not in sys.path:
    sys.path.insert(0, _codegen_dir)

from _lib.emit import cpp_string_literal, write_if_changed
from _lib.loader import load_yaml, load_schema, validate_schema

# (AR) regex لاستخراج enum ErrorCode من header
ENUM_RE = re.compile(
    r"enum\s+class\s+ErrorCode\s*(?::\s*\w+\s*)?\{(.+?)\};",
    re.DOTALL,
)
ENUM_VALUE_RE = re.compile(
    r"^\s*([A-Z][A-Z0-9_]+)\s*(?:,|=|\}|$)",
    re.MULTILINE,
)


def extract_enum_values(error_codes_h: Path) -> set:
    """(AR) يستخرج كل قيم ErrorCode من header — للتحقق الدلالي #2 و #6."""
    text = error_codes_h.read_text(encoding="utf-8")
    m = ENUM_RE.search(text)
    if not m:
        raise RuntimeError(f"لم يُعثَر على enum ErrorCode في {error_codes_h}")
    body = m.group(1)
    # (AR) إزالة التعليقات أولاً
    body = re.sub(r"//[^\n]*", "", body)
    body = re.sub(r"/\*.*?\*/", "", body, flags=re.DOTALL)
    return set(ENUM_VALUE_RE.findall(body))


# (AR) regex لـ placeholders بصيغة {snake_case}
PLACEHOLDER_RE = re.compile(r"\{([a-z][a-z_]*)\}")

# ============================================================================
# (AR) دعم V5: قراءة 8 ملفات language-truth/errors/*.yaml ودمجها في بنية موحَّدة.
#      كل ملف V5 يحوي category نصية واحدة + errors[]، بلا categories/prefix.
#      الترتيب والبادئة ثابتان (مطابقان لتعداد category في error.schema.json).
# (EN) V5 support: load + merge language-truth/errors/*.yaml into the unified
#      structure the rest of the generator expects (categories map + errors[]).
# ============================================================================
CATEGORY_ORDER = [
    "lexical", "syntactic", "semantic", "runtime",
    "ownership", "import", "io", "internal",
]
CATEGORY_PREFIX = {
    "lexical": "LEX", "syntactic": "SYN", "semantic": "SEM", "runtime": "RUN",
    "ownership": "OWN", "import": "IMP", "io": "IO", "internal": "INT",
}


# (AR) 🔑 **قيمةٌ غيرُ مقتبسةٍ فيها « #» تُبتَرُ صامتةً.**
#
#      في YAML تبدأُ المِحْرَفةُ `#` تعليقًا متى سبقَتْها مسافةٌ داخلَ قيمةٍ
#      غيرِ مقتبسة. فرسالةُ خطأٍ تصفُ صيغةً تحوي `#` — وهي حالٌ متكرّرةٌ في
#      لغةٍ تعليقُها `#` و`#*` — تُقرأُ منقوصةً بلا أيِّ إنذار: لا المُخطَّطُ
#      يمنعُها ولا المولِّدُ، لأنّ ما وصلَ إليهما نصٌّ صحيحٌ البنيةِ ناقصُ
#      المعنى.
#
#      ⚠️ وقد وقعَ ذلك فعلًا عند إضافةِ `LEX007`: كُتِبت
#      «تعليق كتلة بدأ بـ #* ولم يُغلق بـ *#» بلا اقتباس، فصارت في الكتالوجِ
#      «تعليق كتلة بدأ بـ»، وطُبِعت للمستخدمِ كذلك. والحارسُ يُخفِقُ الآن
#      قبلَ التوليد.
# (EN) An unquoted value containing " #" is silently truncated: in YAML a hash
#      preceded by a space starts a comment inside an unquoted scalar. An error
#      message describing syntax that contains # — common in a language whose
#      comments are # and #* — is read back incomplete with no warning at all,
#      because what reaches the schema and the generator is structurally valid
#      and semantically maimed. This happened when LEX007 was added. The guard
#      now fails before generation.
def check_unquoted_hash(path: Path) -> list:
    """(AR) يُرجِعُ مواضعَ القيَمِ غيرِ المقتبسةِ التي تحوي « #» فتُبتَر."""
    problems = []
    text = path.read_text(encoding="utf-8")
    for lineno, line in enumerate(text.splitlines(), 1):
        stripped = line.lstrip()
        if not stripped or stripped.startswith("#") or stripped.startswith("- "):
            continue
        if ":" not in stripped:
            continue
        value = stripped.split(":", 1)[1].strip()
        if not value:
            continue
        # (AR) المقتبسُ وكتلُ YAML (| و >) في مأمن.
        if value[0] in "'\"|>&*":
            continue
        if " #" in value:
            problems.append(
                f"{path.name}:{lineno}: قيمةٌ غيرُ مقتبسةٍ فيها « #» — تُبتَرُ عندَها: "
                f"{stripped[:90]}"
            )
    return problems


def load_error_directory(yaml_dir: Path, schema: dict) -> dict:
    """
    (AR) يقرأ كل language-truth/errors/*.yaml، يتحقّق من كلٍّ مقابل schema المتداخل،
         ثم يدمجها في dict موحَّد: {version, categories:{cat:{prefix,...}}, errors:[...]}.
    (EN) Loads + validates each per-category V5 file, merges into the unified dict.
    """
    files = sorted(yaml_dir.glob("*.yaml"))
    if not files:
        raise RuntimeError(f"لا ملفات .yaml في {yaml_dir}")

    merged_errors: list = []
    categories: dict = {}
    version = "5.0"
    seen_categories: list = []

    hash_problems: list = []
    for f in files:
        hash_problems.extend(check_unquoted_hash(f))
    if hash_problems:
        raise RuntimeError(
            "قيَمٌ غيرُ مقتبسةٍ تحوي « #» فتُبتَرُ صامتةً (اقتبسها بعلامتَي '…'):\n  "
            + "\n  ".join(hash_problems)
        )

    for f in files:
        d = load_yaml(f)
        validate_schema(d, schema)  # (AR) تحقّق بنيوي لكل ملف على حدة
        cat = d.get("category", "")
        if cat not in CATEGORY_PREFIX:
            raise RuntimeError(f"فئة غير معروفة '{cat}' في {f.name}")
        categories[cat] = {
            "prefix": CATEGORY_PREFIX[cat],
            "description_ar": d.get("description_ar", ""),
            "description_en": d.get("description_en", ""),
        }
        seen_categories.append(cat)
        merged_errors.extend(d.get("errors", []))

    # (AR) إعادة بناء categories بالترتيب الرسمي (CW-27 — الترتيب دلالي لا أبجدي)
    ordered = {c: categories[c] for c in CATEGORY_ORDER if c in categories}
    return {"version": version, "categories": ordered, "errors": merged_errors}


def validate_semantically(data: dict, enum_values: set) -> list:
    """
    (AR) ست تحقُّقات دلالية تتجاوز Schema:
         1. تفرّد code + تفرّد id
         2. كل code موجود في enum
         3. id يبدأ بـ prefix الفئة
         4. كل {placeholder} في النصوص مذكور في placeholders array
         5. الترتيب: category → id
         6. الاكتمال: كل ErrorCode في enum له entry
    """
    errors: list = []
    seen_codes: dict = {}
    seen_ids: dict = {}
    cat_order = list(data.get("categories", {}).keys())
    prev_key = None

    for idx, entry in enumerate(data.get("errors", [])):
        code = entry.get("code", "")
        eid = entry.get("id", "")
        cat = entry.get("category", "")

        # #1: تفرّد code و id
        if code in seen_codes:
            errors.append(
                f"[#1 تفرّد] تكرار code '{code}' عند index {idx} "
                f"(سبق في {seen_codes[code]})"
            )
        else:
            seen_codes[code] = idx
        if eid in seen_ids:
            errors.append(
                f"[#1 تفرّد] تكرار id '{eid}' عند index {idx} "
                f"(سبق في {seen_ids[eid]})"
            )
        else:
            seen_ids[eid] = idx

        # #2: code موجود في enum
        if code not in enum_values:
            errors.append(
                f"[#2 enum] code '{code}' غير موجود في ErrorCode enum (index {idx})"
            )

        # #3: id يبدأ بـ prefix الفئة
        cat_def = data.get("categories", {}).get(cat)
        if not isinstance(cat_def, dict):
            errors.append(
                f"[#3 prefix] category '{cat}' للـ entry '{eid}' غير معرّف في categories"
            )
        else:
            prefix = cat_def.get("prefix", "")
            if prefix and not eid.startswith(prefix):
                errors.append(
                    f"[#3 prefix] id '{eid}' لا يبدأ بـ prefix '{prefix}' "
                    f"للفئة '{cat}' (entry {idx})"
                )

        # #4: كل {placeholder} في النصوص مذكور في placeholders array
        declared = set(entry.get("placeholders", []) or [])
        for field_name in ("title", "brief", "fix_hint", "detailed", "teacher"):
            field = entry.get(field_name)
            if not isinstance(field, dict):
                continue
            for lang in ("ar", "en"):
                text = field.get(lang, "") or ""
                used = set(PLACEHOLDER_RE.findall(text))
                missing = used - declared
                if missing:
                    errors.append(
                        f"[#4 placeholders] {eid}.{field_name}.{lang}: "
                        f"placeholders غير مُعرَّفة: {sorted(missing)}"
                    )

        # #5: الترتيب (category بترتيبه في categories، ثم id)
        try:
            cat_idx = cat_order.index(cat)
        except ValueError:
            cat_idx = 9999
        cur_key = (cat_idx, eid)
        if prev_key is not None and cur_key < prev_key:
            errors.append(
                f"[#5 ترتيب] entry {idx} (cat={cat}, id={eid}) خارج الترتيب "
                f"— يجب فرز جميع الإدخالات بـ (category, id)"
            )
        prev_key = cur_key

    # #6: الاكتمال
    yaml_codes = set(seen_codes.keys())
    missing = enum_values - yaml_codes
    if missing:
        sample = sorted(missing)[:5]
        errors.append(
            f"[#6 اكتمال] ErrorCodes في enum بلا entry في YAML "
            f"(عدد {len(missing)}): {sample}..."
        )
    orphan = yaml_codes - enum_values
    if orphan:
        errors.append(
            f"[#6 اكتمال] entries في YAML لرموز ليست في enum: {sorted(orphan)}"
        )

    return errors


def _cpp_or_nullptr(s: str) -> str:
    """
    (AR) D-3: نص فارغ/مفقود → nullptr (وضوح + ذاكرة).
    """
    if not s:
        return "nullptr"
    return cpp_string_literal(s)


def _get_bilingual(entry: dict, field: str, lang: str) -> str:
    """(AR) قراءة آمنة لحقل ثنائي اللغة، يُرجع '' إذا غير موجود."""
    obj = entry.get(field)
    if not isinstance(obj, dict):
        return ""
    return obj.get(lang, "") or ""


HEADER_TEMPLATE = '''// ============================================================================
// (AR) error_messages_generated.h — كتالوج رسائل الأخطاء المولَّد آلياً
// (EN) error_messages_generated.h — Auto-generated error messages catalog
// ----------------------------------------------------------------------------
// (AR) ⚠️ لا تُعدِّل هذا الملف يدوياً — عدِّل language-truth/errors/*.yaml
//          ثم أعد البناء (CMake يُشغّل gen_error_messages.py تلقائياً).
// (EN) ⚠️ DO NOT EDIT — modify language-truth/errors/*.yaml then rebuild.
// ----------------------------------------------------------------------------
// (AR) عدد الإدخالات: {count} | إصدار YAML: {version}
// ============================================================================

#pragma once

#include "error_codes.h"
#include <cstddef>
#include <string_view>

namespace Sad::Errors::Generated {{

/**
 * @brief (AR) إدخال مولَّد لكل ErrorCode — نصوص ثنائية اللغة + معاجل ميتاداتا.
 *        (EN) Generated entry per ErrorCode — bilingual text + metadata.
 *
 * @note (AR) نص فارغ يُمَثَّل بـ nullptr (راجع PRD D-3).
 *       (EN) Empty text is represented as nullptr (see PRD D-3).
 */
struct GeneratedErrorEntry {{
    ErrorCode    code;
    const char*  id;          ///< مثل "SEM002"
    const char*  category;    ///< مثل "semantic"
    const char*  titleAr;     const char* titleEn;
    const char*  briefAr;     const char* briefEn;
    const char*  detailedAr;  const char* detailedEn;
    const char*  teacherAr;   const char* teacherEn;
    const char*  fixHintAr;   const char* fixHintEn;
    const char*  codeExample;
}};

/// (AR) مصفوفة كل الأخطاء — مرتبة بـ (category بالترتيب الرسمي ثم id تصاعدياً).
extern const GeneratedErrorEntry kErrorMessages[];

/// (AR) عدد العناصر في kErrorMessages.
extern const std::size_t kErrorMessagesCount;

/// (AR) بحث O(1) عن إدخال بـErrorCode — يُرجع nullptr إذا غير موجود.
/// (EN) O(1) lookup by ErrorCode — returns nullptr if not found.
const GeneratedErrorEntry* findByCode(ErrorCode code);

/// (AR) بحث بـid نصي (مثال: "SEM002") — يُرجع nullptr إذا غير موجود.
const GeneratedErrorEntry* findById(std::string_view id);

}} // namespace Sad::Errors::Generated
'''


def emit_header(data: dict) -> str:
    return HEADER_TEMPLATE.format(
        count=len(data["errors"]),
        version=data.get("version", "?"),
    )


SOURCE_PROLOGUE = '''// ============================================================================
// (AR) error_messages_generated.cpp — كتالوج رسائل الأخطاء المولَّد آلياً
// (EN) error_messages_generated.cpp — Auto-generated error messages catalog
// ----------------------------------------------------------------------------
// (AR) ⚠️ لا تُعدِّل هذا الملف يدوياً — عدِّل language-truth/errors/*.yaml ثم أعد البناء.
// (AR) هذا الملف هو المصدر الوحيد لـ:
//        - getErrorCodeString / getErrorDescription   (تطبيق دوال error_codes.h)
//        - ErrorCatalog::registerDefaults             (تطبيق دالة error_catalog.h)
//        - kErrorMessages[] + findByCode/findById     (واجهة جديدة)
// (EN) Single source for getErrorCodeString, getErrorDescription,
//      ErrorCatalog::registerDefaults, and lookup helpers.
// ============================================================================

#include "error_messages_generated.h"
#include "error_catalog.h"
#include <sstream>
#include <unordered_map>

namespace Sad::Errors::Generated {{

const GeneratedErrorEntry kErrorMessages[] = {{
'''


SOURCE_EPILOGUE = '''}};

const std::size_t kErrorMessagesCount =
    sizeof(kErrorMessages) / sizeof(kErrorMessages[0]);

// (AR) lookup يُبنى مرّة واحدة عند أول استدعاء (CW-26 + RAII).
//      thread-safe: C++11 يضمن init مرة واحدة للـ static local.
static const std::unordered_map<ErrorCode, const GeneratedErrorEntry*>& byCodeMap() {{
    static const auto map = [] {{
        std::unordered_map<ErrorCode, const GeneratedErrorEntry*> m;
        m.reserve(kErrorMessagesCount);
        for (std::size_t i = 0; i < kErrorMessagesCount; ++i) {{
            m.emplace(kErrorMessages[i].code, &kErrorMessages[i]);
        }}
        return m;
    }}();
    return map;
}}

const GeneratedErrorEntry* findByCode(ErrorCode code) {{
    const auto& m = byCodeMap();
    auto it = m.find(code);
    return (it != m.end()) ? it->second : nullptr;
}}

const GeneratedErrorEntry* findById(std::string_view id) {{
    for (std::size_t i = 0; i < kErrorMessagesCount; ++i) {{
        if (id == kErrorMessages[i].id) return &kErrorMessages[i];
    }}
    return nullptr;
}}

}} // namespace Sad::Errors::Generated

// ============================================================================
// (AR) تطبيق دوال error_codes.h من yaml (يستبدل error_codes.cpp القديم)
// (EN) Implementation of error_codes.h API from YAML (replaces legacy error_codes.cpp)
// ============================================================================
namespace Sad {{
    namespace Errors {{

        std::string getErrorCodeString(ErrorCode code) {{
            const auto* e = Generated::findByCode(code);
            return (e && e->id) ? std::string(e->id) : std::string("UNKNOWN");
        }}

        std::string getErrorDescription(ErrorCode code, Language lang) {{
            const auto* e = Generated::findByCode(code);
            if (!e) return std::string("Unknown error code");
            const char* ar = e->briefAr ? e->briefAr : "";
            const char* en = e->briefEn ? e->briefEn : "";
            switch (lang) {{
                case Language::ARABIC:  return std::string(ar);
                case Language::ENGLISH: return std::string(en);
                case Language::BOTH: {{
                    std::ostringstream oss;
                    oss << ar << " / " << en;
                    return oss.str();
                }}
            }}
            return std::string(ar);
        }}

        // (AR) إرجاع اسم فئة الخطأ (lexical/syntactic/semantic/runtime/internal)
        // (EN) Return error category name from yaml category field
        std::string getErrorCategory(ErrorCode code) {{
            const auto* e = Generated::findByCode(code);
            return (e && e->category) ? std::string(e->category) : std::string("unknown");
        }}

// ============================================================================
// (AR) تطبيق ErrorCatalog::registerDefaults من yaml
// (EN) Implementation of ErrorCatalog::registerDefaults from YAML
// ============================================================================
        namespace {{
            inline std::string _s(const char* p) {{ return p ? std::string(p) : std::string(); }}
        }}

        void ErrorCatalog::registerDefaults() {{
            using G = Generated::GeneratedErrorEntry;
            for (std::size_t i = 0; i < Generated::kErrorMessagesCount; ++i) {{
                const G& e = Generated::kErrorMessages[i];
                ErrorTemplate t;
                t.id          = _s(e.id);
                t.titleAr     = _s(e.titleAr);
                t.titleEn     = _s(e.titleEn);
                t.briefAr     = _s(e.briefAr);
                t.briefEn     = _s(e.briefEn);
                t.detailedAr  = _s(e.detailedAr);
                t.detailedEn  = _s(e.detailedEn);
                t.teacherAr   = _s(e.teacherAr);
                t.teacherEn   = _s(e.teacherEn);
                t.fixHintAr   = _s(e.fixHintAr);
                t.fixHintEn   = _s(e.fixHintEn);
                t.codeExample = _s(e.codeExample);
                registerTemplate(e.code, std::move(t));
            }}
        }}

    }} // namespace Errors
}} // namespace Sad
'''


def _emit_entry(entry: dict) -> str:
    """(AR) سطر تهيئة واحد للـ kErrorMessages[]."""
    code = entry["code"]
    eid = entry["id"]
    cat = entry["category"]
    title_ar = _get_bilingual(entry, "title", "ar")
    title_en = _get_bilingual(entry, "title", "en")
    brief_ar = _get_bilingual(entry, "brief", "ar")
    brief_en = _get_bilingual(entry, "brief", "en")
    det_ar = _get_bilingual(entry, "detailed", "ar")
    det_en = _get_bilingual(entry, "detailed", "en")
    tch_ar = _get_bilingual(entry, "teacher", "ar")
    tch_en = _get_bilingual(entry, "teacher", "en")
    fix_ar = _get_bilingual(entry, "fix_hint", "ar")
    fix_en = _get_bilingual(entry, "fix_hint", "en")
    example = entry.get("code_example", "") or ""

    return (
        f"    {{ ErrorCode::{code}, {cpp_string_literal(eid)}, {cpp_string_literal(cat)},\n"
        f"      {_cpp_or_nullptr(title_ar)}, {_cpp_or_nullptr(title_en)},\n"
        f"      {_cpp_or_nullptr(brief_ar)}, {_cpp_or_nullptr(brief_en)},\n"
        f"      {_cpp_or_nullptr(det_ar)}, {_cpp_or_nullptr(det_en)},\n"
        f"      {_cpp_or_nullptr(tch_ar)}, {_cpp_or_nullptr(tch_en)},\n"
        f"      {_cpp_or_nullptr(fix_ar)}, {_cpp_or_nullptr(fix_en)},\n"
        f"      {_cpp_or_nullptr(example)} }},\n"
    )


def emit_source(data: dict) -> str:
    parts = [SOURCE_PROLOGUE.format()]
    for entry in data["errors"]:
        parts.append(_emit_entry(entry))
    parts.append(SOURCE_EPILOGUE.format())
    return "".join(parts)


def sort_entries(data: dict) -> list:
    """
    (AR) D-2: ترتيب ثابت = category (بالترتيب الرسمي في categories) ثم id (تصاعدي).
    """
    cat_order = list(data.get("categories", {}).keys())

    def key(e: dict) -> tuple:
        cat = e.get("category", "")
        try:
            return (cat_order.index(cat), e.get("id", ""))
        except ValueError:
            return (9999, e.get("id", ""))

    return sorted(data["errors"], key=key)


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__.splitlines()[2])
    src = p.add_mutually_exclusive_group(required=True)
    src.add_argument("--yaml", type=Path, help="(V4) ملف error_messages.yaml واحد")
    src.add_argument(
        "--yaml-dir", type=Path,
        help="(V5) مجلد language-truth/errors/ — يُدمج كل *.yaml",
    )
    p.add_argument("--schema", required=True, type=Path)
    p.add_argument(
        "--enum-header", required=True, type=Path,
        help="مسار shared/errors/include/error_codes.h",
    )
    p.add_argument("--header", required=True, type=Path, help="مسار الإخراج .h")
    p.add_argument("--source", required=True, type=Path, help="مسار الإخراج .cpp")
    p.add_argument("--quiet", action="store_true")
    args = p.parse_args()

    # (AR) تحميل وتحقّق YAML — V5 (مجلد) أو V4 (ملف واحد)
    try:
        schema = load_schema(args.schema)
        if args.yaml_dir is not None:
            data = load_error_directory(args.yaml_dir, schema)   # V5
        else:
            data = load_yaml(args.yaml)                           # V4
            validate_schema(data, schema)
    except Exception as e:
        print(
            f"[gen_error_messages] FATAL: فشل تحميل/تحقّق YAML: {e}",
            file=sys.stderr,
        )
        if not args.quiet:
            traceback.print_exc(file=sys.stderr)
        return 2

    # (AR) قراءة enum من header
    try:
        enum_values = extract_enum_values(args.enum_header)
    except Exception as e:
        print(
            f"[gen_error_messages] FATAL: فشل قراءة enum من header: {e}",
            file=sys.stderr,
        )
        return 2

    # (AR) ترتيب ثم تحقّق دلالي
    data["errors"] = sort_entries(data)
    sem_errors = validate_semantically(data, enum_values)
    if sem_errors:
        print("[gen_error_messages] فشل التحقّق الدلالي:", file=sys.stderr)
        for e in sem_errors:
            print(f"  ❌ {e}", file=sys.stderr)
        return 1

    # (AR) إنشاء المجلدات + كتابة
    args.header.parent.mkdir(parents=True, exist_ok=True)
    args.source.parent.mkdir(parents=True, exist_ok=True)
    h_text = emit_header(data)
    c_text = emit_source(data)
    changed_h = write_if_changed(args.header, h_text)
    changed_c = write_if_changed(args.source, c_text)

    if not args.quiet:
        n = len(data["errors"])
        flag = "🔁" if (changed_h or changed_c) else "✓"
        print(
            f"[gen_error_messages] {flag} {n} رسالة → "
            f"{args.header.name} / {args.source.name}"
        )

    return 0


if __name__ == "__main__":
    sys.exit(main())
