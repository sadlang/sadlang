"""
(AR) gen_tool_sot.py — مُولِّد كتالوج C++ لنظام «مصدر حقيقة الأدوات» (Tool SoT).
(EN) gen_tool_sot.py — C++ catalog generator for the tools' Source-of-Truth system.

(AR) عامٌّ لأيّ أداة: يقرأ مجلّد أداةٍ تحت language-truth/tools/<tool>/ الحاوي:
       _meta.yaml       — هوية الأداة (فضاء C++، بادئة، إصدار، ثوابت تنسيق)
       errors.yaml      — كتالوج أخطاء الأداة (خاصّ بها، لا بلغة ص)
       messages.yaml    — رسائل الواجهة غير-الخطئيّة
       commands.yaml    — أوامر الأداة التفاعليّة (اختياريّ)
     ويُنتج ملفّين مولَّدين مُلتزَمين:
       <out-h>          — التعدادات + الجداول + تواقيع الدوال
       <out-cpp>        — الجداول + تطبيق الدوال المساعدة
     كلّ نصّ ثنائيّ اللغة يأتي من YAML؛ لا حرفيّة رسالةٍ تُكتب يدوياً في C++.

     هذا النظام مستقلّ تماماً عن مصدر حقيقة اللغة (keywords/errors/types…):
     الأدوات لا توسّع لغة ص، بل تملك مصدر حقيقتها الخاصّ (قابلٌ للتوسّع لكلّ أداة).

(EN) Tool-agnostic: reads a tool directory and emits a committed C++ catalog
     (enums + tables + helpers). All bilingual text comes from YAML.

     Idempotent (write-if-changed). Any load/validation error fails the build.
"""
from __future__ import annotations

import argparse
import io
import sys
import traceback
from pathlib import Path
from typing import Any

# (AR) اطبع UTF-8 دائمًا (طرفيّة Windows قد تكون cp1255 فتنهار على العربيّة/الرموز).
# (EN) Always print UTF-8 (Windows console may be cp1255 and choke on Arabic/emoji).
try:
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8")
except Exception:  # pragma: no cover
    pass

# (AR) إضافة مجلّد codegen إلى sys.path (مثل بقيّة المولّدات)
_codegen_dir = str(Path(__file__).parent)
if _codegen_dir not in sys.path:
    sys.path.insert(0, _codegen_dir)

from _lib.emit import cpp_string_literal, write_if_changed
from _lib.loader import load_yaml, load_schema, validate_schema

# (AR) خرائط ثابتة — لا حرفيّات مبعثرة داخل منطق التوليد نفسه.
_SEVERITY_ENUM = {"error": "Error", "warning": "Warning", "info": "Info"}

# (AR) مفتاح مساعدة الآبلتات العربيّة (تعريب ١٠ — شريحة الإغلاق):
#      _APPLET_DESC_KEY — حقل الوصف العربيّ لكلّ آبلت في applets.yaml، وقد صار
#      إلزاميًّا (required) في المخطّط tool_applets.schema.json بعد بلوغ التغطية
#      الكاملة 272/272 — الاكتمال يفرضه المخطّط ذاته أبديًّا لكلّ آبلت جديد.
#      سقّاطة «عتبة_تغطية_الوصف» الانتقاليّة (الشريحة ١) حُذفت من الـSoT ومن هنا؛
#      بقي في _validate_cross تحقّق لافراغيّة بسيط دفاعًا في العمق خلف المخطّط.
# (EN) Arabic applet-help key (closing slice): the description field is now
#      required by the schema itself at full 272/272 coverage; the transitional
#      ratchet threshold was removed. A simple non-emptiness check remains in
#      _validate_cross as defense-in-depth behind the schema.
_APPLET_DESC_KEY = "وصف"

# (AR) عقد صيغة سطر التاريخ (calendar.yaml:line_format): %s واحد لاسم اليوم ثمّ ستّة
#      تحويلات عدديّة (سنة/شهر/يوم/ساعة/دقيقة/ثانية) — يُفحَص هنا فلا ينفجر snprintf.
# (EN) date line_format contract: exactly one %s then six integer conversions.
import re as _re
_FMT_DIRECTIVE = _re.compile(r"%(0?\d*)([a-zA-Z%])")
_CAL_EXPECTED_CONVERSIONS = ["s", "d", "d", "d", "d", "d", "d"]
_CAL_WEEKDAYS_COUNT = 7


def _cpp_or_nullptr(s: str) -> str:
    """(AR) نصّ فارغ/مفقود → nullptr (اتّساقاً مع gen_error_messages)."""
    if not s:
        return "nullptr"
    return cpp_string_literal(s)


def _bilingual(obj: Any, lang: str) -> str:
    if not isinstance(obj, dict):
        return ""
    return obj.get(lang, "") or ""


def _load_and_validate(tool_dir: Path, schema_dir: Path, name: str,
                       schema_file: str, required: bool) -> dict | None:
    """(AR) يحمّل ملفّ أداةٍ ويتحقّق منه ضدّ مخطّطه. يُرجع None إن كان اختيارياً وغائباً."""
    path = tool_dir / name
    if not path.exists():
        if required:
            raise RuntimeError(f"ملفّ الأداة المطلوب مفقود / required tool file missing: {path}")
        return None
    data = load_yaml(path)
    schema = load_schema(schema_dir / schema_file)
    validate_schema(data, schema)
    return data


def _validate_cross(meta: dict, errors: dict | None, messages: dict | None,
                    commands: dict | None, applets: dict | None,
                    calendar: dict | None = None) -> list[str]:
    """(AR) تحقّقات دلاليّة تتجاوز المخطّط: تطابق tool، تفرّد الرموز، بادئة المعرّفات."""
    problems: list[str] = []
    tool = meta.get("tool", "")
    prefix = meta.get("prefix", "")

    for label, data in (("errors", errors), ("messages", messages),
                        ("commands", commands), ("applets", applets),
                        ("calendar", calendar)):
        if data is not None and data.get("tool") != tool:
            problems.append(
                f"[tool] '{label}.yaml' tool='{data.get('tool')}' لا يطابق _meta.tool='{tool}'"
            )

    if errors is not None:
        seen_code: set[str] = set()
        seen_id: set[str] = set()
        for e in errors.get("errors", []):
            code, eid = e.get("code", ""), e.get("id", "")
            if code in seen_code:
                problems.append(f"[errors] تكرار code '{code}'")
            seen_code.add(code)
            if eid in seen_id:
                problems.append(f"[errors] تكرار id '{eid}'")
            seen_id.add(eid)
            if prefix and not eid.startswith(prefix):
                problems.append(f"[errors] id '{eid}' لا يبدأ ببادئة الأداة '{prefix}'")

    if messages is not None:
        seen_key: set[str] = set()
        for m in messages.get("messages", []):
            k = m.get("key", "")
            if k in seen_key:
                problems.append(f"[messages] تكرار key '{k}'")
            seen_key.add(k)

    if commands is not None:
        seen_name: set[str] = set()
        for c in commands.get("commands", []):
            for n in (c.get("name", ""), c.get("arabic_name", "")):
                if n in seen_name:
                    problems.append(f"[commands] تكرار اسم أمر '{n}'")
                seen_name.add(n)

    if applets is not None:
        # (AR) الاسم العربيّ الصريح مفتاح الموزِّع ⇒ يجب تفرّده (لا يجوز اسمان لبرنامجين).
        # (EN) the explicit Arabic name is the dispatcher key ⇒ must be unique.
        seen_ar: set[str] = set()
        for a in applets.get("applets", []):
            ar = a.get("arabic", "")
            if ar in seen_ar:
                problems.append(f"[applets] تكرار اسم عربيّ '{ar}'")
            seen_ar.add(ar)
        # (AR) اكتمال الوصف (تعريب ١٠ — شريحة الإغلاق): «وصف» صار required في
        #      المخطّط ذاته بعد بلوغ 272/272، فالمخطّط هو الحارس الأوّل (غيابه
        #      يفشل validate_schema قبل الوصول هنا). هذا تحقّق لافراغيّة بسيط
        #      دفاعًا في العمق — سقّاطة العتبة الانتقاليّة حُذفت.
        # (EN) description completeness (closing slice): the field is required by
        #      the schema itself; this is a simple non-emptiness defense-in-depth
        #      check. The transitional ratchet threshold was removed.
        for a in applets.get("applets", []):
            if not str(a.get(_APPLET_DESC_KEY, "") or "").strip():
                problems.append(
                    f"[applets] الآبلت '{a.get('arabic', '')}' بلا حقل "
                    f"«{_APPLET_DESC_KEY}» غير فارغ — الوصف العربيّ إلزاميّ "
                    f"لكلّ آبلت (يفرضه المخطّط tool_applets.schema.json)")

    if calendar is not None:
        # (AR) حارس التصادم الساكن (تعريب ٨ شريحة ١): اسم آبلت التاريخ يجب أن يكون
        #      مُعرَّفًا في معجم الموزِّع، ويجب ألّا يساوي أيّ اسم أمر REPL — الفخّ
        #      الموثَّق: «تاريخ» (بلا أل التعريف) = history قائم في commands.yaml،
        #      و«التاريخ» (بأل التعريف) هو آبلت التاريخ. تساويهما يعني اختطاف وجهة.
        # (EN) static collision guard: the date applet must exist in the dispatcher
        #      lexicon and must never equal a REPL command name («تاريخ» = history).
        date_applet = calendar.get("date_applet", "")
        if applets is not None:
            applet_names = {a.get("arabic", "") for a in applets.get("applets", [])}
            if date_applet not in applet_names:
                problems.append(
                    f"[calendar] date_applet '{date_applet}' غير موجود في applets.yaml")
        if commands is not None:
            for c in commands.get("commands", []):
                for n in (c.get("name", ""), c.get("arabic_name", "")):
                    if n == date_applet:
                        problems.append(
                            f"[calendar] date_applet '{date_applet}' يصطدم باسم أمر REPL "
                            f"'{n}' (handler={c.get('handler', '')}) — وجهتان لاسم واحد")
        # (AR) تفرّد أسماء الأيّام السبعة (العدد يفرضه المخطّط 7/7).
        wds = calendar.get("weekdays", [])
        if len(set(wds)) != len(wds):
            problems.append("[calendar] تكرار في أسماء الأيّام weekdays")
        if len(wds) != _CAL_WEEKDAYS_COUNT:
            problems.append(f"[calendar] عدد الأيّام {len(wds)} ≠ {_CAL_WEEKDAYS_COUNT}")
        # (AR) عقد line_format: %s واحد أوّلًا ثمّ ستّة تحويلات عدديّة — بترتيب صارم.
        convs = [m.group(2) for m in _FMT_DIRECTIVE.finditer(calendar.get("line_format", ""))
                 if m.group(2) != "%"]
        if convs != _CAL_EXPECTED_CONVERSIONS:
            problems.append(
                f"[calendar] line_format يخالف العقد (%s ثمّ ستّة أعداد): {convs}")

    return problems


# ──────────────────────────────────────────────────────────────────────
# توليد الترويسة / Header emission
# ──────────────────────────────────────────────────────────────────────
_HEADER_TOP = """\
// بسم الله الرحمن الرحيم
// {out_name} — كتالوج «مصدر حقيقة الأدوات» المولَّد آلياً / auto-generated Tool-SoT catalog
// (AR) ⚠️ لا تُعدِّل يدوياً — عدِّل language-truth/tools/{tool}/*.yaml ثمّ أعد التوليد (x.py gen).
// (EN) ⚠️ DO NOT EDIT — modify language-truth/tools/{tool}/*.yaml then rebuild (x.py gen).
// (AR) المصدر: _meta.yaml + errors.yaml + messages.yaml + commands.yaml + applets.yaml + calendar.yaml
// (EN) Source: _meta.yaml + errors.yaml + messages.yaml + commands.yaml + applets.yaml + calendar.yaml

#pragma once

#include <cstddef>
#include <string>
#include <string_view>

{ns_open}

// ── الهوية / Meta ──
inline constexpr const char *kToolId = {tool_lit};
inline constexpr const char *kVersion = {version_lit};
inline constexpr const char *kDisplayNameAr = {dname_ar};
inline constexpr const char *kDisplayNameEn = {dname_en};
inline constexpr const char *kBilingualSeparator = {sep_lit};
inline constexpr const char *kDetailSeparator = {detail_sep_lit};
"""

_HEADER_ERRORS = """
// ── الأخطاء / Errors ──
enum class Error {{
{error_enum}
}};

enum class Severity {{ Error, Warning, Info }};

/// (AR) إدخال خطأٍ مولَّد — نصوص ثنائيّة اللغة (nullptr للنصّ الغائب).
struct ErrorEntry {{
    Error code;
    const char *id;
    Severity severity;
    const char *titleAr;
    const char *titleEn;
    const char *briefAr;
    const char *briefEn;
    const char *hintAr;
    const char *hintEn;
    bool hasDetail;
}};

/// (AR) بحث عن إدخال خطأ — يُرجع nullptr إن لم يوجد.
const ErrorEntry *findError(Error code);

/// (AR) رسالة ثنائيّة اللغة: «briefAr / briefEn» + «: detail» إن وُجد تفصيل.
std::string errorMessage(Error code, std::string_view detail = {{}});
"""

_HEADER_MESSAGES = """
// ── الرسائل / Messages ──
enum class Message {{
{message_enum}
}};

struct MessageEntry {{
    Message key;
    const char *ar;
    const char *en;
}};

const MessageEntry *findMessage(Message key);
std::string_view messageAr(Message key);
std::string_view messageEn(Message key);
/// (AR) «ar / en» على سطرٍ واحد.
std::string messageBoth(Message key);
"""

_HEADER_COMMANDS = """
// ── الأوامر / Commands ──
enum class CommandHandler {{
{handler_enum}
}};

struct CommandEntry {{
    CommandHandler handler;
    const char *name;
    const char *arabicName;
    const char *descAr;
    const char *descEn;
    const char *usage;
}};

extern const CommandEntry kCommands[];
extern const std::size_t kCommandsCount;
"""

_HEADER_APPLETS = """
// ── معجم الأوامر العربيّة / Arabic applet lexicon ──
// (AR) اسمٌ عربيّ صريح → برنامج التنفيذ الحقيقيّ؛ يترجمه الموزِّع قبل execvp.
//      descAr (تعريب ١٠ — شريحة الإغلاق): وصف عربيّ فصيح بسطر واحد يعرضه
//      «:مساعدة اسم» — غير فارغ دائمًا: التغطية اكتملت 272/272 والمخطّط يفرض
//      «وصف» لكلّ آبلت (بقي النوع مؤشّرًا والمستهلكون يدافعون عن nullptr عمقًا).
// (EN) an explicit Arabic name → the real exec program; the dispatcher translates before execvp.
//      descAr: one-line Arabic help description shown by «:مساعدة name» — always
//      non-null now: full coverage reached and the schema requires it (consumers keep null-guards as defense).
struct AppletEntry {
    const char *arabic;
    const char *exec;
    const char *descAr;
};

extern const AppletEntry kApplets[];
extern const std::size_t kAppletsCount;

/// (AR) يترجم اسمًا عربيًّا صريحًا إلى برنامج التنفيذ؛ nullptr إن لم يُعرَّف (فيبقى الاسم كما هو).
/// (EN) translates an explicit Arabic name to its exec program; nullptr if undefined (kept as-is).
const char *appletExec(std::string_view arabic);

/// (AR) يعيد مدخل المعجم كاملًا لاسمٍ عربيّ صريح (لقراءة descAr)؛ nullptr إن لم يُعرَّف.
/// (EN) full lexicon entry for an explicit Arabic name (to read descAr); nullptr if undefined.
const AppletEntry *findApplet(std::string_view arabic);
"""

_HEADER_CALENDAR = """
// ── التقويم / Calendar ──
// (AR) الشريحة الأولى من «تعريب ٨»: أسماء الأيّام السبعة فقط (calendar.yaml).
//      التوسّع (أشهر/هجريّ/أرقام مشرقيّة/منطقة زمنيّة) قرار مالك معلَّق — لا يُوسَّع هنا.
// (EN) First slice: the seven Arabic weekday names only; months/Hijri are pending owner decisions.

/// (AR) الاسم العربيّ القانونيّ لآبلت التاريخ — يلتقطه الموزِّع بلا وسائط كأمر داخليّ.
/// (EN) canonical Arabic date-applet name — intercepted argument-less as a builtin.
inline constexpr const char *kDateAppletArabic = {date_applet_lit};

/// (AR) صيغة printf لسطر التاريخ العربيّ: %s اسم اليوم ثمّ سنة/شهر/يوم/ساعة/دقيقة/ثانية.
/// (EN) printf format of the Arabic date line: %s weekday then y/m/d h:m:s integers.
inline constexpr const char *kDateLineFormat = {line_format_lit};

/// (AR) أسماء الأيّام مرتّبة بترتيب tm_wday في POSIX: 0=الأحد … 6=السبت.
/// (EN) weekday names ordered by POSIX tm_wday: 0=Sunday … 6=Saturday.
extern const char *const kWeekdays[];
extern const std::size_t kWeekdaysCount;

/// (AR) اسم اليوم لفهرس tm_wday؛ nullptr خارج المدى [0،6].
/// (EN) weekday name for a tm_wday index; nullptr outside [0,6].
const char *weekdayName(int tmWday);
"""


def _indent_enum(values: list[str]) -> str:
    return ",\n".join(f"    {v}" for v in values)


def _ns_wrap(namespace: str) -> tuple[str, str]:
    """(AR) يفتح/يغلق فضاء الأسماء المتشعّب (Sad::REPL::SoT)."""
    parts = [p for p in namespace.split("::") if p]
    open_lines = "\n".join(f"namespace {p} {{" for p in parts)
    close_lines = "\n".join(f"}} // namespace {p}" for p in reversed(parts))
    return open_lines, close_lines


def emit_header(meta: dict, errors: dict | None, messages: dict | None,
                commands: dict | None, applets: dict | None, out_name: str,
                calendar: dict | None = None) -> str:
    ns_open, ns_close = _ns_wrap(meta["cpp_namespace"])
    fmt = meta["formatting"]
    dname = meta.get("display_name") or {}
    parts = [_HEADER_TOP.format(
        out_name=out_name,
        tool=meta["tool"],
        ns_open=ns_open,
        tool_lit=cpp_string_literal(meta["tool"]),
        version_lit=cpp_string_literal(meta["version"]),
        dname_ar=cpp_string_literal(_bilingual(dname, "ar") or meta["tool"]),
        dname_en=cpp_string_literal(_bilingual(dname, "en") or meta["tool"]),
        sep_lit=cpp_string_literal(fmt["bilingual_separator"]),
        detail_sep_lit=cpp_string_literal(fmt["detail_separator"]),
    )]

    if errors is not None:
        codes = [e["code"] for e in errors["errors"]]
        parts.append(_HEADER_ERRORS.format(error_enum=_indent_enum(codes)))

    if messages is not None:
        keys = [m["key"] for m in messages["messages"]]
        parts.append(_HEADER_MESSAGES.format(message_enum=_indent_enum(keys)))

    if commands is not None:
        handlers: list[str] = []
        for c in commands["commands"]:
            h = c["handler"]
            if h not in handlers:
                handlers.append(h)
        parts.append(_HEADER_COMMANDS.format(handler_enum=_indent_enum(handlers)))

    if applets is not None:
        parts.append(_HEADER_APPLETS)

    if calendar is not None:
        parts.append(_HEADER_CALENDAR.format(
            date_applet_lit=cpp_string_literal(calendar["date_applet"]),
            line_format_lit=cpp_string_literal(calendar["line_format"]),
        ))

    parts.append("\n" + ns_close + "\n")
    return "".join(parts)


# ──────────────────────────────────────────────────────────────────────
# توليد المصدر / Source emission
# ──────────────────────────────────────────────────────────────────────
def _emit_error_table(ns: str, errors: dict) -> str:
    rows = []
    for e in errors["errors"]:
        sev = _SEVERITY_ENUM[e["severity"]]
        rows.append(
            f"    {{ Error::{e['code']}, {cpp_string_literal(e['id'])}, Severity::{sev},\n"
            f"      {_cpp_or_nullptr(_bilingual(e.get('title'), 'ar'))}, "
            f"{_cpp_or_nullptr(_bilingual(e.get('title'), 'en'))},\n"
            f"      {_cpp_or_nullptr(_bilingual(e.get('brief'), 'ar'))}, "
            f"{_cpp_or_nullptr(_bilingual(e.get('brief'), 'en'))},\n"
            f"      {_cpp_or_nullptr(_bilingual(e.get('hint'), 'ar'))}, "
            f"{_cpp_or_nullptr(_bilingual(e.get('hint'), 'en'))},\n"
            f"      {'true' if e.get('has_detail') else 'false'} }},\n"
        )
    return (
        "static const ErrorEntry kErrors[] = {\n"
        + "".join(rows)
        + "};\n"
        "static const std::size_t kErrorsCount = sizeof(kErrors) / sizeof(kErrors[0]);\n\n"
        "const ErrorEntry *findError(Error code) {\n"
        "    for (std::size_t i = 0; i < kErrorsCount; ++i) {\n"
        "        if (kErrors[i].code == code) return &kErrors[i];\n"
        "    }\n"
        "    return nullptr;\n"
        "}\n\n"
        "std::string errorMessage(Error code, std::string_view detail) {\n"
        "    const ErrorEntry *e = findError(code);\n"
        "    if (!e) return std::string(detail);\n"
        "    std::string out = std::string(e->briefAr) + kBilingualSeparator + e->briefEn;\n"
        "    if (e->hasDetail && !detail.empty()) {\n"
        "        out += std::string(kDetailSeparator) + std::string(detail);\n"
        "    }\n"
        "    return out;\n"
        "}\n\n"
    )


def _emit_message_table(messages: dict) -> str:
    rows = []
    for m in messages["messages"]:
        rows.append(
            f"    {{ Message::{m['key']}, "
            f"{cpp_string_literal(m['ar'])}, {cpp_string_literal(m['en'])} }},\n"
        )
    return (
        "static const MessageEntry kMessages[] = {\n"
        + "".join(rows)
        + "};\n"
        "static const std::size_t kMessagesCount = sizeof(kMessages) / sizeof(kMessages[0]);\n\n"
        "const MessageEntry *findMessage(Message key) {\n"
        "    for (std::size_t i = 0; i < kMessagesCount; ++i) {\n"
        "        if (kMessages[i].key == key) return &kMessages[i];\n"
        "    }\n"
        "    return nullptr;\n"
        "}\n\n"
        "std::string_view messageAr(Message key) {\n"
        "    const MessageEntry *m = findMessage(key);\n"
        "    return m ? std::string_view(m->ar) : std::string_view();\n"
        "}\n\n"
        "std::string_view messageEn(Message key) {\n"
        "    const MessageEntry *m = findMessage(key);\n"
        "    return m ? std::string_view(m->en) : std::string_view();\n"
        "}\n\n"
        "std::string messageBoth(Message key) {\n"
        "    const MessageEntry *m = findMessage(key);\n"
        "    if (!m) return std::string();\n"
        "    return std::string(m->ar) + kBilingualSeparator + m->en;\n"
        "}\n\n"
    )


def _emit_command_table(commands: dict) -> str:
    rows = []
    for c in commands["commands"]:
        rows.append(
            f"    {{ CommandHandler::{c['handler']}, "
            f"{cpp_string_literal(c['name'])}, {cpp_string_literal(c['arabic_name'])},\n"
            f"      {_cpp_or_nullptr(_bilingual(c.get('description'), 'ar'))}, "
            f"{_cpp_or_nullptr(_bilingual(c.get('description'), 'en'))}, "
            f"{cpp_string_literal(c['usage'])} }},\n"
        )
    return (
        "const CommandEntry kCommands[] = {\n"
        + "".join(rows)
        + "};\n"
        "const std::size_t kCommandsCount = sizeof(kCommands) / sizeof(kCommands[0]);\n\n"
    )


def _emit_applet_table(applets: dict) -> str:
    rows = []
    for a in applets["applets"]:
        rows.append(
            f"    {{ {cpp_string_literal(a['arabic'])}, {cpp_string_literal(a['exec'])}, "
            f"{_cpp_or_nullptr(a.get(_APPLET_DESC_KEY, ''))} }},\n"
        )
    return (
        "const AppletEntry kApplets[] = {\n"
        + "".join(rows)
        + "};\n"
        "const std::size_t kAppletsCount = sizeof(kApplets) / sizeof(kApplets[0]);\n\n"
        "const char *appletExec(std::string_view arabic) {\n"
        "    for (std::size_t i = 0; i < kAppletsCount; ++i) {\n"
        "        if (arabic == kApplets[i].arabic) return kApplets[i].exec;\n"
        "    }\n"
        "    return nullptr;\n"
        "}\n\n"
        "const AppletEntry *findApplet(std::string_view arabic) {\n"
        "    for (std::size_t i = 0; i < kAppletsCount; ++i) {\n"
        "        if (arabic == kApplets[i].arabic) return &kApplets[i];\n"
        "    }\n"
        "    return nullptr;\n"
        "}\n\n"
    )


def _emit_calendar_table(calendar: dict) -> str:
    rows = "".join(f"    {cpp_string_literal(w)},\n" for w in calendar["weekdays"])
    return (
        "// (AR) جدول الأيّام مرتّب بترتيب tm_wday (0=الأحد) — من calendar.yaml.\n"
        "// (EN) weekday table ordered by tm_wday (0=Sunday) — from calendar.yaml.\n"
        "const char *const kWeekdays[] = {\n"
        + rows
        + "};\n"
        "const std::size_t kWeekdaysCount = sizeof(kWeekdays) / sizeof(kWeekdays[0]);\n\n"
        "const char *weekdayName(int tmWday) {\n"
        "    if (tmWday < 0 || static_cast<std::size_t>(tmWday) >= kWeekdaysCount) {\n"
        "        return nullptr;\n"
        "    }\n"
        "    return kWeekdays[tmWday];\n"
        "}\n\n"
    )


def emit_source(meta: dict, errors: dict | None, messages: dict | None,
                commands: dict | None, applets: dict | None,
                header_name: str, out_name: str,
                calendar: dict | None = None) -> str:
    ns_open, ns_close = _ns_wrap(meta["cpp_namespace"])
    parts = [
        "// بسم الله الرحمن الرحيم\n"
        f"// {out_name} — كتالوج «مصدر حقيقة الأدوات» المولَّد آلياً / auto-generated Tool-SoT catalog\n"
        f"// (AR) ⚠️ لا تُعدِّل يدوياً — عدِّل language-truth/tools/{meta['tool']}/*.yaml ثمّ أعد التوليد.\n"
        f"// (EN) ⚠️ DO NOT EDIT — modify language-truth/tools/{meta['tool']}/*.yaml then rebuild.\n\n"
        f'#include "{header_name}"\n\n'
        f"{ns_open}\n\n"
    ]
    if errors is not None:
        parts.append(_emit_error_table(meta["cpp_namespace"], errors))
    if messages is not None:
        parts.append(_emit_message_table(messages))
    if commands is not None:
        parts.append(_emit_command_table(commands))
    if applets is not None:
        parts.append(_emit_applet_table(applets))
    if calendar is not None:
        parts.append(_emit_calendar_table(calendar))
    parts.append(ns_close + "\n")
    return "".join(parts)


def main() -> int:
    p = argparse.ArgumentParser(description="Generate a Tool-SoT C++ catalog")
    p.add_argument("--tool-dir", required=True, type=Path,
                   help="language-truth/tools/<tool>/")
    p.add_argument("--schema-dir", required=True, type=Path,
                   help="language-truth/tools/_schemas/")
    p.add_argument("--out-h", required=True, type=Path, help="مسار الإخراج .h")
    p.add_argument("--out-cpp", required=True, type=Path, help="مسار الإخراج .cpp")
    p.add_argument("--quiet", action="store_true")
    args = p.parse_args()

    try:
        meta = _load_and_validate(args.tool_dir, args.schema_dir,
                                  "_meta.yaml", "tool_meta.schema.json", required=True)
        errors = _load_and_validate(args.tool_dir, args.schema_dir,
                                    "errors.yaml", "tool_errors.schema.json", required=False)
        messages = _load_and_validate(args.tool_dir, args.schema_dir,
                                      "messages.yaml", "tool_messages.schema.json", required=False)
        commands = _load_and_validate(args.tool_dir, args.schema_dir,
                                      "commands.yaml", "tool_commands.schema.json", required=False)
        applets = _load_and_validate(args.tool_dir, args.schema_dir,
                                     "applets.yaml", "tool_applets.schema.json", required=False)
        calendar = _load_and_validate(args.tool_dir, args.schema_dir,
                                      "calendar.yaml", "tool_calendar.schema.json", required=False)
    except Exception as e:
        print(f"[gen_tool_sot] FATAL: فشل تحميل/تحقّق YAML: {e}", file=sys.stderr)
        if not args.quiet:
            traceback.print_exc(file=sys.stderr)
        return 2

    problems = _validate_cross(meta, errors, messages, commands, applets, calendar)
    if problems:
        print("[gen_tool_sot] فشل التحقّق الدلاليّ:", file=sys.stderr)
        for pr in problems:
            print(f"  ❌ {pr}", file=sys.stderr)
        return 1

    header_name = args.out_h.name
    h_text = emit_header(meta, errors, messages, commands, applets, header_name,
                         calendar)
    c_text = emit_source(meta, errors, messages, commands, applets,
                         header_name, args.out_cpp.name, calendar)

    args.out_h.parent.mkdir(parents=True, exist_ok=True)
    args.out_cpp.parent.mkdir(parents=True, exist_ok=True)
    changed_h = write_if_changed(args.out_h, h_text)
    changed_c = write_if_changed(args.out_cpp, c_text)

    if not args.quiet:
        n_err = len(errors["errors"]) if errors else 0
        n_msg = len(messages["messages"]) if messages else 0
        n_cmd = len(commands["commands"]) if commands else 0
        n_app = len(applets["applets"]) if applets else 0
        n_cal = len(calendar["weekdays"]) if calendar else 0
        flag = "🔁" if (changed_h or changed_c) else "✓"
        print(f"[gen_tool_sot] {flag} {meta['tool']}: "
              f"{n_err} أخطاء / {n_msg} رسائل / {n_cmd} أوامر / {n_app} آبلت / "
              f"{n_cal} أيّام → "
              f"{args.out_h.name} + {args.out_cpp.name}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
