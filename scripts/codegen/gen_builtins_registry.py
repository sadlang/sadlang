"""
============================================================================
(AR) مُولِّد builtin_registry_generated.h من ملفات language-truth/builtins/*.yaml
(EN) Generator for builtin_registry_generated.h from YAML — Sad Language
----------------------------------------------------------------------------
(AR) يقرأ جميع ملفات builtins/*.yaml ويُنتج ملف C++ header يحوي:
     1. namespace Names::X { constexpr string_view CPP_ID = "canonical"; }
     2. CORE_IO_BUILTINS[] — دوال IO الأساسية
     3. TYPE_CONSTRUCTOR_BUILTINS[] — بانيات الأنواع
     4. MODULE_FUNCTION_BUILTINS[] — دوال الوحدات
     5. TYPE_METHOD_BUILTINS[] — طرق الأنواع
     6. static_assert للتحقق من الأعداد
     7. دوال البحث (findBuiltinByName، isAvailableWithoutImport)
     8. ALL_BUILTINS — سجل شامل (BuiltinMeta) لكل المدمجات للأدوات (LSP/sadinfo)
        + دالّتا البحث الشامل findBuiltinMeta و isKnownBuiltin

(EN) Reads builtins/*.yaml and produces a C++ header with the same structure
     as the hand-written builtin_registry.h — drop-in replacement.

الاستخدام:
    python gen_builtins_registry.py \\
        --yaml-dir language-truth/builtins/ \\
        --index    language-truth/builtins/_index.yaml \\
        --out-h    shared/builtins/generated/builtin_registry_generated.h \\
        --quiet
============================================================================
"""

from __future__ import annotations

import argparse
import sys
import traceback
from pathlib import Path
from typing import Any
from collections import defaultdict

try:
    import yaml
except ImportError:
    print("[gen_builtins_registry] FATAL: pyyaml not installed. pip install pyyaml", file=sys.stderr)
    sys.exit(1)

# ─── المسارات الافتراضية ───
_THIS_DIR = Path(__file__).parent
_REPO_ROOT = _THIS_DIR.parents[1]
_DEFAULT_YAML_DIR = _REPO_ROOT / "language-truth" / "builtins"
_DEFAULT_INDEX    = _DEFAULT_YAML_DIR / "_index.yaml"
_DEFAULT_OUT_H    = _REPO_ROOT / "shared" / "builtins" / "generated" / "builtin_registry_generated.h"

# ─── تصنيفات الـ category ───
CATEGORY_TO_ENUM = {
    "CORE_IO":           "BuiltinCategory::CORE_IO",
    "TYPE_CONSTRUCTOR":  "BuiltinCategory::TYPE_CONSTRUCTOR",
    "MODULE_FUNCTION":   "BuiltinCategory::MODULE_FUNCTION",
    "TYPE_METHOD":       "BuiltinCategory::TYPE_METHOD",
}

# ─── استراتيجيات الترجمة ───
STRATEGY_TO_ENUM = {
    "INLINE":          "CompilerStrategy::INLINE",
    "RUNTIME_CALL":    "CompilerStrategy::RUNTIME_CALL",
    "LLVM_INTRINSIC":  "CompilerStrategy::LLVM_INTRINSIC",
}

# ─── معرّفات الوحدات ───
MODULE_TO_ENUM = {
    "NONE":         "ModuleId::NONE",
    "MATH":         "ModuleId::MATH",
    "STRINGS":      "ModuleId::STRINGS",
    "BASICS":       "ModuleId::BASICS",
    "ASYNC":        "ModuleId::ASYNC",
    "ASSERTIONS":   "ModuleId::ASSERTIONS",
    "MAPS":         "ModuleId::MAPS",
    "PLATFORM":     "ModuleId::PLATFORM",
    "NETWORK":      "ModuleId::NETWORK",
    "CRYPTO":       "ModuleId::CRYPTO",
    "SOCKETS":      "ModuleId::SOCKETS",
}

# ─── namespace → comment ───
NS_COMMENTS = {
    "Core":              ("دوال الإدخال/الإخراج الأساسية", "Core I/O functions — no import required"),
    "TypeCtor":          ("بانيات الأنواع", "Type constructors — no import required"),
    "Concurrency":       ("دوال التزامن الأساسية", "Core concurrency — no import required"),
    "Math":              ("وحدة رياضيات", "Math module"),
    "Strings":           ("وحدة نصوص", "Strings module"),
    "Arrays":            ("عمليات المصفوفات المدمجة", "Built-in array operations"),
    "Basics":            ("وحدة أساسيات", "Basics module"),
    "Assertions":        ("وحدة تأكيدات", "Assertions module"),
    "Maps":              ("وحدة خرائط", "Maps module"),
    "Sockets":           ("وحدة مقابس", "Sockets module"),
    "HttpClient":        ("عميل HTTP", "HTTP Client"),
    "HttpServer":        ("خادم HTTP", "HTTP Server"),
    "NetworkUtils":      ("أدوات الشبكة", "Network utilities"),
    "WebSocketClient":   ("عميل WebSocket", "WebSocket Client"),
}


def _parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    """
    @brief (AR) يُحلِّل مُعطيات سطر الأوامر.
    @brief (EN) Parse command-line arguments.
    """
    p = argparse.ArgumentParser(
        description="Generate builtin_registry_generated.h from YAML files"
    )
    p.add_argument("--yaml-dir", type=Path, default=_DEFAULT_YAML_DIR,
                   help="Directory containing builtins/*.yaml files")
    p.add_argument("--index",    type=Path, default=_DEFAULT_INDEX,
                   help="Path to _index.yaml file listing YAML files in order")
    p.add_argument("--out-h",    type=Path, default=_DEFAULT_OUT_H,
                   help="Output C++ header file path")
    p.add_argument("--quiet",    action="store_true",
                   help="Suppress info messages")
    return p.parse_args(argv)


def load_all_functions(yaml_dir: Path, index_path: Path, quiet: bool) -> list[dict[str, Any]]:
    """
    @brief (AR) يقرأ جميع ملفات YAML بالترتيب المحدد في _index.yaml.
    @brief (EN) Load all functions from YAML files in order specified by index.
    """
    if not index_path.exists():
        raise FileNotFoundError(f"[gen_builtins_registry] Index not found: {index_path}")

    with open(index_path, encoding="utf-8") as f:
        index_data = yaml.safe_load(f)

    file_list: list[str] = index_data.get("files", [])
    all_functions: list[dict] = []

    for filename in file_list:
        yaml_path = yaml_dir / filename
        if not yaml_path.exists():
            if not quiet:
                print(f"  [skip] {filename} — not found", file=sys.stderr)
            continue

        with open(yaml_path, encoding="utf-8") as f:
            data = yaml.safe_load(f)

        functions: list[dict] = data.get("functions", [])
        for fn in functions:
            fn["_source_file"] = filename
        all_functions.extend(functions)

        if not quiet:
            print(f"  [load] {filename}: {len(functions)} functions")

    return all_functions


def group_by_namespace(functions: list[dict]) -> dict[str, list[dict]]:
    """
    @brief (AR) يُجمِّع الدوال حسب الـ namespace.
    @brief (EN) Group functions by namespace.
    """
    groups: dict[str, list[dict]] = defaultdict(list)
    for fn in functions:
        ns = fn.get("namespace", "Unknown")
        groups[ns].append(fn)
    return dict(groups)


def emit_names_namespaces(ns_groups: dict[str, list[dict]], all_type_methods: list[dict] = None) -> str:
    """
    @brief (AR) يُولِّد كتلة namespace Names { ... } بما فيها TypeMethods.
    @brief (EN) Emit the Names namespace with all sub-namespaces including TypeMethods.
    """
    lines = []
    lines.append("        namespace Names")
    lines.append("        {")

    for ns, fns in ns_groups.items():
        ar_comment, en_comment = NS_COMMENTS.get(ns, (ns, ns))
        lines.append(f"")
        lines.append(f"            // ─────────── {ar_comment} ───────────")
        lines.append(f"            namespace {ns}")
        lines.append(f"            {{")
        for fn in fns:
            cpp_id   = fn["cpp_id"]
            canonical = fn["canonical"]
            desc_ar  = fn.get("description_ar", "")
            lines.append(f'                // (AR) {desc_ar}')
            lines.append(f'                inline constexpr std::string_view {cpp_id} = "{canonical}";')
        lines.append(f"            }}")

    # إضافة TypeMethods namespace
    if all_type_methods:
        type_method_names = emit_type_method_names(all_type_methods)
        if type_method_names:
            lines.append(type_method_names)

    lines.append("")
    lines.append("        } // namespace Names")
    return "\n".join(lines)


def emit_builtin_arrays(functions: list[dict]) -> str:
    """
    @brief (AR) يُولِّد مصفوفات CORE_IO / TYPE_CONSTRUCTOR / MODULE_FUNCTION.
    @brief (EN) Emit the builtin entry arrays.
    (AR) يستخدم حقل lookup_table لتحديد أي مصفوفة تنتمي إليها الدالة.
    """
    # استخدام lookup_table إن وُجد، وإلا التصنيف التلقائي
    core_io   = [f for f in functions if f.get("lookup_table") == "CORE_IO"]
    type_ctor = [f for f in functions if f.get("lookup_table") == "TYPE_CONSTRUCTOR"]
    mod_fns   = [f for f in functions if f.get("lookup_table") == "MODULE_FUNCTION"]
    # fallback للدوال بدون lookup_table
    if not core_io:
        core_io   = [f for f in functions if f.get("category") == "CORE_IO"]
    if not type_ctor:
        type_ctor = [f for f in functions if f.get("category") == "TYPE_CONSTRUCTOR"]
    if not mod_fns:
        mod_fns   = [f for f in functions if f.get("category") == "MODULE_FUNCTION"]

    def entry_line(fn: dict) -> str:
        ns       = fn["namespace"]
        cpp_id   = fn["cpp_id"]
        cat      = CATEGORY_TO_ENUM.get(fn.get("category", ""), "BuiltinCategory::MODULE_FUNCTION")
        module   = MODULE_TO_ENUM.get(fn.get("module", "NONE"), "ModuleId::NONE")
        strategy = STRATEGY_TO_ENUM.get(fn.get("compiler_strategy", "RUNTIME_CALL"), "CompilerStrategy::RUNTIME_CALL")
        return f"            {{Names::{ns}::{cpp_id}, {cat}, {module}, {strategy}}},"

    lines = []

    # CORE_IO
    lines.append(f"")
    lines.append(f"        inline constexpr std::array<BuiltinEntry, {len(core_io)}> CORE_IO_BUILTINS = {{{{")
    for fn in core_io:
        lines.append(entry_line(fn))
    lines.append(f"        }}}};")

    # TYPE_CONSTRUCTOR
    lines.append(f"")
    lines.append(f"        inline constexpr std::array<BuiltinEntry, {len(type_ctor)}> TYPE_CONSTRUCTOR_BUILTINS = {{{{")
    for fn in type_ctor:
        lines.append(entry_line(fn))
    lines.append(f"        }}}};")

    # MODULE_FUNCTION — مُجمَّعة حسب الوحدة
    lines.append(f"")
    lines.append(f"        inline constexpr std::array<BuiltinEntry, {len(mod_fns)}> MODULE_FUNCTION_BUILTINS = {{{{")
    # تجميع حسب الوحدة
    by_module: dict[str, list[dict]] = defaultdict(list)
    for fn in mod_fns:
        by_module[fn.get("module", "NONE")].append(fn)
    for module_name, module_fns in by_module.items():
        lines.append(f"            // ─── وحدة {module_name} ({len(module_fns)} دالة) ───")
        for fn in module_fns:
            lines.append(entry_line(fn))
    lines.append(f"        }}}};")

    return "\n".join(lines)


def emit_static_asserts(functions: list[dict]) -> str:
    """
    @brief (AR) يُولِّد static_assert للتحقق من الأعداد.
    @brief (EN) Emit compile-time size checks.
    """
    core_io   = sum(1 for f in functions if f.get("lookup_table") == "CORE_IO"
                    or (not f.get("lookup_table") and f.get("category") == "CORE_IO"))
    type_ctor = sum(1 for f in functions if f.get("lookup_table") == "TYPE_CONSTRUCTOR"
                    or (not f.get("lookup_table") and f.get("category") == "TYPE_CONSTRUCTOR"))
    mod_fns   = sum(1 for f in functions if f.get("lookup_table") == "MODULE_FUNCTION"
                    or (not f.get("lookup_table") and f.get("category") == "MODULE_FUNCTION"))

    lines = [
        "",
        "        // ════════════════════════════════════════════════════════════",
        "        // (AR) تحقق وقت الترجمة — مُولَّد تلقائياً",
        "        // (EN) Compile-time validation — auto-generated",
        "        // ════════════════════════════════════════════════════════════",
        f'        static_assert(CORE_IO_BUILTINS.size() == {core_io}, "CORE_IO count mismatch");',
        f'        static_assert(TYPE_CONSTRUCTOR_BUILTINS.size() == {type_ctor}, "TYPE_CONSTRUCTOR count mismatch");',
        f'        static_assert(MODULE_FUNCTION_BUILTINS.size() == {mod_fns}, "MODULE_FUNCTION count mismatch");',
    ]
    return "\n".join(lines)


TARGET_TYPE_TO_ENUM = {
    "STRING":  "TargetType::STRING",
    "ARRAY":   "TargetType::ARRAY",
    "MAP":     "TargetType::MAP",
    "CHANNEL": "TargetType::CHANNEL",
    "ANY":     "TargetType::ANY",
}


def load_type_methods(yaml_dir: Path, quiet: bool) -> tuple[list[dict], list[dict]]:
    """
    @brief (AR) يقرأ type_methods.yaml ويُرجع (جميع الطرق، طرق المصفوفة الـ32).
    @brief (EN) Load type methods: (all_methods, lookup_array_methods).
    """
    tm_path = yaml_dir.parent / "type_methods.yaml"
    if not tm_path.exists():
        if not quiet:
            print(f"  [skip] type_methods.yaml — not found", file=sys.stderr)
        return ([], [])

    with open(tm_path, encoding="utf-8") as f:
        data = yaml.safe_load(f)

    methods = data.get("methods", [])
    lookup_methods = [m for m in methods if m.get("in_lookup_array") is True]
    if not quiet:
        print(f"  [load] type_methods.yaml: {len(methods)} total, {len(lookup_methods)} in lookup array")
    return (methods, lookup_methods)


def emit_type_method_names(all_methods: list[dict]) -> str:
    """
    @brief (AR) يُولِّد namespace Names::TypeMethods::* بثوابت string_view لكل طريقة.
    @brief (EN) Emit Names::TypeMethods::* namespace with string_view constants.
    (AR) يجمع الطرق حسب الهدف (مصفوفة/نص/خريطة/...) ويُنتج ثوابت UPPER_SNAKE_CASE.
    """
    if not all_methods:
        return ""

    import re

    seen_ids: dict[str, int] = {}

    def to_cpp_id(m: dict) -> str:
        """توليد cpp_id فريد من method_en أو ترقيم"""
        method_en = m.get("method_en", "")
        base = re.sub(r'[^a-zA-Z0-9]', '_', str(method_en)).upper().strip('_')
        if not base or base[0].isdigit() or not any(c.isalpha() for c in base):
            # استخدام ترقيم إذا كان الاسم الإنجليزي غير صالح
            base = "METHOD_" + str(len(seen_ids))
        return base

    # تجميع حسب target_type
    TARGET_LABEL = {
        "ARRAY":   "Array",
        "STRING":  "String",
        "MAP":     "Map",
        "CHANNEL": "Channel",
        "ANY":     "Any",
    }

    TARGET_AR = {
        "ARRAY":   "المصفوفات",
        "STRING":  "النصوص",
        "MAP":     "الخرائط",
        "CHANNEL": "القنوات",
        "ANY":     "عامة",
    }

    by_target: dict[str, list[dict]] = {}
    for m in all_methods:
        tt = m.get("target_type", "ANY")
        by_target.setdefault(tt, []).append(m)

    lines = []
    lines.append("")
    lines.append("            // ─────────── طرق الأنواع المدمجة ───────────")
    lines.append("            namespace TypeMethods")
    lines.append("            {")

    for tt, methods in sorted(by_target.items()):
        ns_label = TARGET_LABEL.get(tt, tt)
        ar_label = TARGET_AR.get(tt, tt)
        lines.append(f"")
        lines.append(f"                // ─── {ar_label} ───")
        lines.append(f"                namespace {ns_label}")
        lines.append(f"                {{")
        # تتبع الـ cpp_ids المستخدمة في هذا الـ namespace
        used_ids: dict[str, int] = {}
        for m in methods:
            method_name = m["method"]
            method_en   = m.get("method_en", "")
            base_id = re.sub(r'[^a-zA-Z0-9]', '_', str(method_en)).upper().strip('_')
            if not base_id or base_id[0].isdigit() or not any(c.isalpha() for c in base_id):
                base_id = "METHOD"
            # ضمان التفرد
            if base_id not in used_ids:
                cpp_id = base_id
                used_ids[base_id] = 1
            else:
                cnt = used_ids[base_id]
                cpp_id = f"{base_id}_{cnt}"
                used_ids[base_id] = cnt + 1
            desc_ar = m.get("description_ar", method_name)
            lines.append(f'                    // (AR) {desc_ar}')
            lines.append(f'                    inline constexpr std::string_view {cpp_id} = "{method_name}";')
        lines.append(f"                }}")

    lines.append("            } // namespace TypeMethods")
    return "\n".join(lines)


def emit_type_method_array(type_methods: list[dict]) -> str:
    """
    @brief (AR) يُولِّد TYPE_METHOD_BUILTINS.
    @brief (EN) Emit TYPE_METHOD_BUILTINS array.
    """
    if not type_methods:
        return ""

    lines = []
    lines.append("")
    lines.append("        struct TypeMethodEntry")
    lines.append("        {")
    lines.append("            std::string_view methodName; /// (AR) اسم الطريقة / (EN) Method name")
    lines.append("            TargetType targetType;       /// (AR) نوع الكائن المستهدف / (EN) Target type")
    lines.append("        };")
    lines.append("")
    lines.append(f"        inline constexpr std::array<TypeMethodEntry, {len(type_methods)}> TYPE_METHOD_BUILTINS = {{{{")

    # تجميع حسب target_type للتوضيح
    by_target: dict[str, list[dict]] = {}
    for m in type_methods:
        tt = m.get("target_type", "ANY")
        by_target.setdefault(tt, []).append(m)

    for tt, methods in by_target.items():
        target_ar = {"ARRAY": "المصفوفات", "STRING": "النصوص", "MAP": "الخرائط",
                     "CHANNEL": "القنوات", "ANY": "عامة"}.get(tt, tt)
        lines.append(f"            // ─── طرق {target_ar} ───")
        for m in methods:
            name = m["method"]
            target_enum = TARGET_TYPE_TO_ENUM.get(tt, "TargetType::ANY")
            lines.append(f'            {{"{name}", {target_enum}}},')

    lines.append("        }};")
    lines.append("")
    lines.append(f'        static_assert(TYPE_METHOD_BUILTINS.size() == {len(type_methods)}, "TYPE_METHOD count mismatch");')
    return "\n".join(lines)


def _cpp_str(s: Any) -> str:
    """
    @brief (AR) تهريب نصّ ليكون سلسلة C++ صالحة (يحمي من " و\\ والأسطر).
    @brief (EN) Escape a string for safe embedding in a C++ string literal.
    """
    s = "" if s is None else str(s)
    s = s.replace("\\", "\\\\").replace('"', '\\"')
    s = s.replace("\n", "\\n").replace("\r", "\\r").replace("\t", "\\t")
    return s


def emit_all_builtins(functions: list[dict]) -> str:
    """
    @brief (AR) يُولِّد سجلاً شاملاً (BuiltinMeta + ALL_BUILTINS) لكل المدمجات
                للأدوات (LSP/sadinfo): اسم + فضاء + تصنيف + وحدة + وصف + معاملات.
                نوع الإرجاع فارغ حتى يُضاف حقل `returns` في YAML.
    @brief (EN) Emit a comprehensive BuiltinMeta + ALL_BUILTINS registry covering
                EVERY builtin (not just the 72 in BuiltinEntry) for tooling.
    """
    lines: list[str] = []
    lines.append("        // ════════════════════════════════════════════════════════════════════")
    lines.append("        // (AR) سجل شامل لكل المدمجات للأدوات (LSP/sadinfo) — اسم + بيانات وصفية.")
    lines.append("        //      يغطّي كل الدوال، لا الـ72 القابلة للاستعلام في BuiltinEntry فقط.")
    lines.append("        //      returnType فارغ حتى يُضاف حقل `returns` في language-truth/builtins/*.yaml.")
    lines.append("        // (EN) Comprehensive registry of ALL builtins for tooling (LSP/sadinfo).")
    lines.append("        // ════════════════════════════════════════════════════════════════════")
    lines.append("        struct BuiltinMeta")
    lines.append("        {")
    lines.append("            std::string_view canonicalName;  /// (AR) الاسم الأساسي / (EN) Canonical name")
    lines.append("            std::string_view namespaceName;  /// (AR) الفضاء / (EN) Namespace")
    lines.append("            std::string_view category;       /// (AR) التصنيف / (EN) Category")
    lines.append("            std::string_view requiredModule; /// (AR) مفتاح الوحدة الخام / (EN) Raw module key")
    lines.append("            bool             requireImport;  /// (AR) هل يلزم استيراد؟ / (EN) Requires import?")
    lines.append("            std::string_view descriptionAr;  /// (AR) وصف عربي (تحويم) / (EN) Arabic description")
    lines.append("            std::string_view paramsCsv;      /// (AR) معاملات مفصولة بـ«، » / (EN) Params, '، '-separated")
    lines.append("            std::string_view returnType;     /// (AR) نوع الإرجاع (فارغ مؤقتاً) / (EN) Return type (empty for now)")
    lines.append("        };")
    lines.append("")
    lines.append(f"        inline constexpr std::array<BuiltinMeta, {len(functions)}> ALL_BUILTINS = {{{{")

    by_ns: dict[str, list[dict]] = defaultdict(list)
    for fn in functions:
        by_ns[fn.get("namespace", "Unknown")].append(fn)

    for ns, fns in by_ns.items():
        lines.append(f"            // ─── {ns} ({len(fns)}) ───")
        for fn in fns:
            cpp_id     = fn["cpp_id"]
            ns_esc     = _cpp_str(ns)
            cat        = _cpp_str(fn.get("category", ""))
            module     = _cpp_str(fn.get("module", "NONE"))
            req_imp    = "true" if fn.get("require_import", False) else "false"
            desc       = _cpp_str(fn.get("description_ar", ""))
            params     = fn.get("params") or []
            params_csv = _cpp_str("، ".join(str(p) for p in params))
            ret        = _cpp_str(fn.get("returns", ""))
            lines.append(
                f'            {{Names::{ns}::{cpp_id}, "{ns_esc}", "{cat}", '
                f'"{module}", {req_imp}, "{desc}", "{params_csv}", "{ret}"}},'
            )

    lines.append("        }};")
    lines.append("")
    lines.append(f'        static_assert(ALL_BUILTINS.size() == {len(functions)}, "ALL_BUILTINS count mismatch");')
    lines.append("")
    lines.append("        // ─── دوال بحث شاملة للأدوات / Comprehensive tooling lookups ───")
    lines.append("        // (AR) ملاحظة: بعض الأسماء الأساسية مشتركة بين فضاءات مختلفة")
    lines.append("        //      (مثل «أرسل» في HttpClient وAsyncAdvanced). يعيد هذا البحث")
    lines.append("        //      أوّل تطابق بترتيب language-truth/builtins/_index.yaml؛ لتعداد")
    lines.append("        //      كل التطابقات كرِّر على ALL_BUILTINS مباشرة (المرحلة اللاحقة).")
    lines.append("        // (EN) Note: a few canonical names are shared across namespaces.")
    lines.append("        //      This returns the FIRST match in _index.yaml order; iterate")
    lines.append("        //      ALL_BUILTINS directly to enumerate every match.")
    lines.append("        inline const BuiltinMeta* findBuiltinMeta(std::string_view name)")
    lines.append("        {")
    lines.append("            for (const auto& b : ALL_BUILTINS)")
    lines.append("                if (b.canonicalName == name) return &b;")
    lines.append("            return nullptr;")
    lines.append("        }")
    lines.append("")
    lines.append("        inline bool isKnownBuiltin(std::string_view name)")
    lines.append("        {")
    lines.append("            return findBuiltinMeta(name) != nullptr;")
    lines.append("        }")
    return "\n".join(lines)


HEADER_TEMPLATE = """\
// ============================================================================
// AUTO-GENERATED from language-truth/builtins/*.yaml — DO NOT EDIT MANUALLY
// (AR) مُولَّد تلقائياً من language-truth/builtins/*.yaml — لا تعدّله يدوياً
// (EN) Auto-generated by scripts/codegen/gen_builtins_registry.py
//
// To modify: edit language-truth/builtins/*.yaml then rebuild.
// لإجراء تعديل: عدّل YAML ثم أعد البناء.
// ============================================================================

#pragma once

#include "builtin_categories.h"
#include "module_definitions.h"
#include <string_view>
#include <cstddef>
#include <array>

// ============================================================================
// (AR) إبطال ماكروز نظام التشغيل التي تتعارض مع أسماء الثوابت أدناه.
//      المترجم (sadc) يضمّ رؤوس Windows التي تُعرّف DELETE/UNION/... كماكروز،
//      مما يكسر الثوابت المُولَّدة. لا يستخدم المشروع هذه الماكروز.
// (EN) Undefine OS macros that collide with constant names below.
//      sadc includes Windows headers defining DELETE/UNION/... as macros,
//      which would break the generated constants. The project uses none of them.
// ============================================================================
#ifdef DELETE
#undef DELETE
#endif
#ifdef UNION
#undef UNION
#endif
#ifdef INTERSECT
#undef INTERSECT
#endif
#ifdef DIFFERENCE
#undef DIFFERENCE
#endif
#ifdef IN
#undef IN
#endif
#ifdef OUT
#undef OUT
#endif
#ifdef OPTIONAL
#undef OPTIONAL
#endif
#ifdef ABSOLUTE
#undef ABSOLUTE
#endif
#ifdef RELATIVE
#undef RELATIVE
#endif

namespace Sad
{{
    namespace Builtins
    {{

{names_section}

        // ════════════════════════════════════════════════════════════════════
        //               الطبقة 1: دوال IO الأساسية (بدون استيراد)
        //               Layer 1: Core IO (no import)
        // ════════════════════════════════════════════════════════════════════

        struct BuiltinEntry
        {{
            std::string_view canonicalName;
            BuiltinCategory  category;
            ModuleId         requiredModule;
            CompilerStrategy compilerStrategy;
        }};

{arrays_section}

{type_method_section}

{static_asserts}

        // ════════════════════════════════════════════════════════════════════
        // (AR) دوال البحث في السجل
        // (EN) Registry lookup functions
        // ════════════════════════════════════════════════════════════════════

        inline const BuiltinEntry* findBuiltinByName(std::string_view name)
        {{
            for (const auto& entry : CORE_IO_BUILTINS)
                if (entry.canonicalName == name) return &entry;
            for (const auto& entry : TYPE_CONSTRUCTOR_BUILTINS)
                if (entry.canonicalName == name) return &entry;
            for (const auto& entry : MODULE_FUNCTION_BUILTINS)
                if (entry.canonicalName == name) return &entry;
            return nullptr;
        }}

        inline bool isAvailableWithoutImport(std::string_view name)
        {{
            for (const auto& entry : CORE_IO_BUILTINS)
                if (entry.canonicalName == name) return true;
            for (const auto& entry : TYPE_CONSTRUCTOR_BUILTINS)
                if (entry.canonicalName == name) return true;
            return false;
        }}

        inline ModuleId getRequiredModule(std::string_view name)
        {{
            const BuiltinEntry* entry = findBuiltinByName(name);
            return entry ? entry->requiredModule : ModuleId::NONE;
        }}

        /**
         * @brief (AR) اقتراح الوحدة المناسبة لدالة غير مستوردة (لرسائل الخطأ)
         * @brief (EN) Suggest the appropriate module for an unimported function (for error messages)
         */
        inline std::string_view suggestModuleForFunction(std::string_view name)
        {{
            const BuiltinEntry* entry = findBuiltinByName(name);
            if (!entry || entry->requiredModule == ModuleId::NONE)
                return "";
            return getCanonicalModuleName(entry->requiredModule);
        }}

{all_builtins_section}

    }} // namespace Builtins
}} // namespace Sad
"""


def write_if_changed(path: Path, content: str, quiet: bool) -> None:
    """
    @brief (AR) يكتب الملف فقط إذا تغيَّر المحتوى.
    @brief (EN) Write file only if content changed.
    """
    if path.exists():
        existing = path.read_text(encoding="utf-8")
        if existing == content:
            if not quiet:
                print(f"[gen_builtins_registry] No changes: {path}")
            return

    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8")
    if not quiet:
        print(f"[gen_builtins_registry] Written: {path}")


def run(argv: list[str] | None = None) -> int:
    """
    @brief (AR) نقطة الدخول الرئيسية.
    @brief (EN) Main entry point.
    """
    args = _parse_args(argv)

    try:
        if not args.quiet:
            print(f"[gen_builtins_registry] Loading from: {args.yaml_dir}")

        functions = load_all_functions(args.yaml_dir, args.index, args.quiet)

        if not functions:
            print("[gen_builtins_registry] ERROR: No functions found", file=sys.stderr)
            return 1

        # تحميل type_methods من type_methods.yaml
        all_type_methods, lookup_type_methods = load_type_methods(args.yaml_dir, args.quiet)

        ns_groups = group_by_namespace(functions)

        names_section       = emit_names_namespaces(ns_groups, all_type_methods)
        arrays_section      = emit_builtin_arrays(functions)
        type_method_section = emit_type_method_array(lookup_type_methods)
        static_asserts      = emit_static_asserts(functions)
        all_builtins_section = emit_all_builtins(functions)

        content = HEADER_TEMPLATE.format(
            names_section=names_section,
            arrays_section=arrays_section,
            type_method_section=type_method_section,
            static_asserts=static_asserts,
            all_builtins_section=all_builtins_section,
        )

        write_if_changed(args.out_h, content, args.quiet)

        if not args.quiet:
            core_io   = sum(1 for f in functions if f.get("category") == "CORE_IO")
            type_ctor = sum(1 for f in functions if f.get("category") == "TYPE_CONSTRUCTOR")
            mod_fns   = sum(1 for f in functions if f.get("category") == "MODULE_FUNCTION")
            print(f"[gen_builtins_registry] Summary:")
            print(f"  Total:            {len(functions)}")
            print(f"  CORE_IO:          {core_io}")
            print(f"  TYPE_CONSTRUCTOR: {type_ctor}")
            print(f"  MODULE_FUNCTION:  {mod_fns}")
            print(f"  Namespaces:       {len(ns_groups)}")

        return 0

    except Exception as exc:
        print(f"[gen_builtins_registry] FATAL: {exc}", file=sys.stderr)
        traceback.print_exc(file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(run())
