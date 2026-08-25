# -*- coding: utf-8 -*-
"""
مِقياسُ سطحِ اللغة (Language Surface Meter) — نظام القياس الحتميّ
═══════════════════════════════════════════════════════════════════════════

(AR) الفكرة: سؤالُ «كم توسَّعت لغة ص؟» يجب أن يكونَ **جوابًا مقيسًا** لا
     انطباعًا ولا رقمًا منثورًا في وثيقة. فهذه الأداة:
       1. تشتقُّ عددَ كلِّ سطحٍ لغويٍّ من `language-truth/` — المكانِ الوحيدِ
          الذي تُولَدُ فيه حقيقةٌ لغويّة — عبر خريطةٍ **صريحةٍ** (ملفّ، مسارُ
          مفتاح، قسم). لا عدَّ تخمينيّ: كلُّ مفتاحٍ مُسمًّى باسمِه.
       2. تحرسُ التغطيةَ في **الاتّجاهَين**: أيُّ ملفِّ YAML في مصدر الحقيقة
          لا يُنسَبُ إلى قسمٍ ولا يُستثنى بسببٍ مكتوب ⇒ إخفاق. وأيُّ مدخلٍ في
          الخريطةِ يشيرُ إلى ملفٍّ أو مفتاحٍ غيرِ موجود ⇒ إخفاق. فقائمةُ
          الإذنِ لا تتعفّنُ في أيِّ جهة.
       3. تودِعُ الأعدادَ في `ledger/surface_latest.yaml` وتُلحِقُ صفًّا مؤرَّخًا
          بـ`ledger/history.tsv` — فيصيرُ التوسُّعُ **منحنًى لا لقطة**.
       4. `--check` (وضعُ CI): تعيدُ الاشتقاقَ وتقارنُ بالمُودَع؛ أيُّ خلافٍ
          يُفشِلُ الاختبار. فالخارطةُ حقيقةٌ مقيسةٌ لا نثرٌ يُصان باليد.

     ⚠️ ما لا تحرسُه عمدًا: أعدادُ أسطرِ الكودِ وملفّاتِ الاختبار. تلك تتغيّرُ
     في كلِّ إيداع، فحارسٌ عليها يحمرُّ دائمًا ثمّ يُعطَّل ثمّ يخضرُّ بلا قياس.
     تُحسَبُ عند العرضِ (`--render`) بوصفِها جردًا حيًّا، لا عقدًا مُودَعًا.

(EN) Language-surface meter. Derives every countable language surface from
     `language-truth/` via an EXPLICIT (file, key-path, section) map, guards
     coverage in BOTH directions (unmapped SoT file => fail; mapped-but-absent
     file/key => fail), commits the counts to a ledger, and appends a dated
     history row so growth is a curve rather than a snapshot. `--check` is the
     CI mode: re-derive and fail on any drift from the committed ledger.
     Deliberately NOT guarded: LOC / test-file counts (they change every
     commit; a guard on them reddens always, gets disabled, then goes green
     without measuring). Those are computed live at render time only.

الاستخدام / Usage:
    python tests/metrics/surface/surface_map.py --check      # CI
    python tests/metrics/surface/surface_map.py --update     # قرارٌ بشريّ
    python tests/metrics/surface/surface_map.py --render OUT.md
    python tests/metrics/surface/surface_map.py --self-test
"""

import argparse
import os
import subprocess
import sys
from pathlib import Path

import yaml

# (AR) كونسول ويندوز قد يكون بترميزٍ محلّيّ — نفرضُ UTF-8 للعربيّة.
# (EN) Force UTF-8 stdout for Arabic on locale-encoded Windows consoles.
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")

HERE = Path(__file__).resolve().parent
PROJECT_ROOT = HERE.parents[2]
TRUTH = PROJECT_ROOT / "language-truth"
LEDGER_DIR = HERE / "ledger"
LEDGER_FILE = LEDGER_DIR / "surface_latest.yaml"
HISTORY_FILE = LEDGER_DIR / "history.tsv"

# ═══════════════════════════════════════════════════════════════════════════
# (AR) الخريطةُ الصريحة: قسم ← (ملفّ، مسارُ مفتاح، وسم).
#      `*` في اسمِ الملفِّ يعني «كلُّ ملفّات المجلد المطابقة» ويُجمَعُ عددُها.
#      لا يُعَدُّ مفتاحٌ لم يُسَمَّ هنا — فالتوسُّعُ الصامتُ مستحيل.
# (EN) The explicit map: section <- (file, key-path, label). A `*` in the file
#      name aggregates across matching files. Nothing unnamed is ever counted.
# ═══════════════════════════════════════════════════════════════════════════
SECTIONS = [
    {
        "id": "01_core",
        "name_ar": "النواةُ اللغويّة",
        "name_en": "Core language",
        "is_language_surface": True,
        "entries": [
            ("keywords.yaml", "categories.reserved.keywords", "كلماتٌ محجوزة"),
            ("keywords.yaml", "categories.contextual.keywords", "كلماتٌ سياقيّة"),
            ("keywords.yaml", "categories.operators.keywords", "كلماتٌ عاملة"),
            ("keywords.yaml", "categories.builtin_types.keywords", "أسماءُ أنواعٍ مدمجة"),
            ("operators.yaml", "operators", "عوامل"),
            ("grammar/[0-9]*.yaml", "productions", "قواعدُ نحويّة"),
            ("grammar/lowers_to.yaml", "rules", "قواعدُ خفض"),
            ("expr_constructs.yaml", "constructs", "تراكيبُ تعبير"),
            ("grammar_constructs.yaml", "constructs", "تراكيبُ قواعد"),
            ("oop_constructs.yaml", "constructs", "تراكيبُ كائنيّة"),
            ("patterns.yaml", "patterns", "أنماطُ مطابقة"),
            ("directives.yaml", "directives", "موجِّهات"),
        ],
    },
    {
        "id": "02_types",
        "name_ar": "نظامُ الأنواع",
        "name_en": "Type system",
        "is_language_surface": True,
        "entries": [
            ("types.yaml", "types", "أنواعٌ معرَّفة"),
            ("types.yaml", "categories_ar", "تصنيفاتٌ عربيّة"),
            ("type_methods.yaml", "methods", "طرائقُ أنواع"),
            ("type_methods.yaml", "targets", "أنواعٌ هدفٌ للطرائق"),
        ],
    },
    {
        "id": "03a_builtins_hardware",
        "name_ar": "مدمجات · عتادٌ ونواةُ نظام",
        "name_en": "Builtins / hardware & kernel",
        "is_language_surface": True,
        "entries": [
            ("builtins/kernel*.yaml", "functions", "مدمجاتُ النواة"),
            ("builtins/compiler_os_hardware.yaml", "functions", "عتادُ النظام"),
            ("builtins/compiler_uefi.yaml", "functions", "UEFI"),
            ("builtins/compiler_embedded.yaml", "functions", "مضمَّنة"),
            ("builtins/compiler_simd.yaml", "functions", "SIMD"),
        ],
    },
    {
        "id": "03b_builtins_ui",
        "name_ar": "مدمجات · واجهةٌ رسوميّة",
        "name_en": "Builtins / UI",
        "is_language_surface": True,
        "entries": [
            ("builtins/ui_widgets.yaml", "functions", "ودجات"),
            ("builtins/ui_platform.yaml", "functions", "منصّةُ الواجهة"),
            ("builtins/compiler_ui.yaml", "functions", "واجهةُ المترجم"),
        ],
    },
    {
        "id": "03c_builtins_core",
        "name_ar": "مدمجات · أساسيّاتٌ لغويّة",
        "name_en": "Builtins / language core",
        "is_language_surface": True,
        "entries": [
            ("builtins/basics.yaml", "functions", "أساسيّات"),
            ("builtins/core.yaml", "functions", "قلب"),
            ("builtins/types.yaml", "functions", "أنواع"),
            ("builtins/strings.yaml", "functions", "نصوص"),
            ("builtins/arrays.yaml", "functions", "مصفوفات"),
            ("builtins/maps.yaml", "functions", "خرائط"),
            ("builtins/math.yaml", "functions", "رياضيّات"),
            ("builtins/exceptions.yaml", "functions", "استثناءات"),
            ("builtins/assertions.yaml", "functions", "تأكيدات"),
        ],
    },
    {
        "id": "03d_builtins_system",
        "name_ar": "مدمجات · نظامٌ ومنصّةٌ وأمن",
        "name_en": "Builtins / system, platform, security",
        "is_language_surface": True,
        "entries": [
            ("builtins/ffi.yaml", "functions", "جسرُ الأجنبيّ"),
            ("builtins/compiler_os_core.yaml", "functions", "قلبُ النظام"),
            ("builtins/compiler_os_system.yaml", "functions", "خدماتُ النظام"),
            ("builtins/compiler_system.yaml", "functions", "نظامُ المترجم"),
            ("builtins/compiler_io_utils.yaml", "functions", "أدواتُ دخل/خرج"),
            ("builtins/compiler_security.yaml", "functions", "أمنُ المترجم"),
            ("builtins/crypto.yaml", "functions", "تشفير"),
            ("builtins/platform.yaml", "functions", "منصّة"),
            ("builtins/processes.yaml", "functions", "عمليّات"),
        ],
    },
    {
        "id": "03e_builtins_network",
        "name_ar": "مدمجات · شبكة",
        "name_en": "Builtins / network",
        "is_language_surface": True,
        "entries": [
            ("builtins/http_client.yaml", "functions", "عميلُ HTTP"),
            ("builtins/http_server.yaml", "functions", "خادمُ HTTP"),
            ("builtins/sockets.yaml", "functions", "مقابس"),
            ("builtins/sadnet.yaml", "functions", "شبكةُ ص"),
            ("builtins/websocket_client.yaml", "functions", "عميلُ WebSocket"),
            ("builtins/websocketserver.yaml", "functions", "خادمُ WebSocket"),
            ("builtins/network_utils.yaml", "functions", "أدواتُ الشبكة"),
        ],
    },
    {
        "id": "03f_builtins_concurrency",
        "name_ar": "مدمجات · تزامن",
        "name_en": "Builtins / concurrency",
        "is_language_surface": True,
        "entries": [
            ("builtins/async_advanced.yaml", "functions", "تزامنٌ متقدّم"),
            ("builtins/concurrency.yaml", "functions", "تزامن"),
        ],
    },
    {
        "id": "04_ui",
        "name_ar": "الواجهةُ الرسوميّة",
        "name_en": "UI surface",
        "is_language_surface": True,
        "entries": [
            ("ui_nodes.yaml", "nodes", "عُقَد"),
            ("ui_props.yaml", "keys", "مفاتيحُ خصائص"),
            ("ui_props.yaml", "values", "قيَمُ خصائص"),
            ("ui_events.yaml", "entries", "أحداث"),
            ("ui_colors.yaml", "colors", "ألوانٌ مسمّاة"),
            ("ui_animations.yaml", "entries", "حركات"),
            ("ui_modifiers.yaml", "modifiers", "مُعدِّلات"),
            ("ui_easings.yaml", "entries", "مُنحنيات"),
            ("ui_propagation.yaml", "entries", "قواعدُ انتشار"),
            ("ui_nodes.yaml", "platform_renderers", "مُصيِّراتُ منصّات"),
            ("ui_nodes.yaml", "render_targets", "أهدافُ تصيير"),
            ("ui_nodes.yaml", "backend_support", "دعمُ الخلفيّات"),
            ("ui_nodes.yaml", "render_exemptions", "إعفاءاتُ تصيير"),
        ],
    },
    {
        "id": "05_stdlib",
        "name_ar": "المكتبةُ القياسيّة",
        "name_en": "Standard library",
        "is_language_surface": True,
        "entries": [
            ("stdlib/modules.yaml", "modules", "وحداتٌ معلنة"),
            ("stdlib/functions.yaml", "functions", "دوالٌّ معلنة"),
        ],
    },
    {
        "id": "06_backend",
        "name_ar": "الخلفيّةُ الأصليّة",
        "name_en": "Native backend",
        "is_language_surface": False,
        "entries": [
            ("backend/sir_opcodes.yaml", "opcodes", "أوپكودات SIR"),
            ("backend/arch_specific_opcodes.yaml", "opcodes", "أوپكوداتُ معماريّة"),
            ("backend/arch_specific_opcodes.yaml", "families", "عائلاتُ معماريّة"),
            ("backend/targets.yaml", "targets", "أهدافُ ترجمة"),
            ("backend/targets.yaml", "stages", "مراحل"),
            ("backend/targets.yaml", "out_of_scope", "خارجَ النطاق"),
            ("backend/value_repr.yaml", "dyn_kinds", "أنواعُ القيمةِ الديناميّة"),
            ("backend/value_repr.yaml", "display_texts", "نصوصُ العرض"),
            ("backend/value_repr.yaml", "map_layout", "تخطيطُ الخريطة"),
            ("backend/*/instructions.yaml", "instructions", "تعليماتُ المعماريّات"),
            ("backend/*/registers.yaml", "classes", "أصنافُ المسجّلات"),
            ("backend/*/isel.yaml", "patterns", "أنماطُ اختيارِ التعليمات"),
            ("backend/abi/*.yaml", "call", "عقدُ النداء"),
            ("backend/abi/*.yaml", "syscall", "عقدُ نداءِ النظام"),
        ],
    },
    {
        "id": "07_diagnostics",
        "name_ar": "التشخيصُ والأخطاء",
        "name_en": "Diagnostics",
        "is_language_surface": False,
        "entries": [
            ("errors/*.yaml", "errors", "مداخلُ كتالوجِ الأخطاء"),
            ("backend/native_diagnostics.yaml", "tag", "وسومُ تشخيصِ الخلفيّة"),
            ("backend/native_diagnostics.yaml", "pure", "تشخيصاتٌ خالصة"),
            ("backend/native_diagnostics.yaml", "kind", "أصنافُ التشخيص"),
            ("backend/native_diagnostics.yaml", "format", "صِيَغُ التشخيص"),
            ("backend/native_diagnostics.yaml", "vreg", "مسجّلاتٌ افتراضيّة"),
            ("backend/native_diagnostics.yaml", "helper", "مُعينات"),
        ],
    },
    {
        "id": "08_tooling",
        "name_ar": "سطحُ الأدوات",
        "name_en": "Tooling surface",
        "is_language_surface": False,
        "entries": [
            ("cli_flags.yaml", "flags", "أعلامُ سطرِ الأوامر"),
            ("cli_flags.yaml", "short_flags", "أعلامٌ مختصرة"),
            ("cli_flags.yaml", "value_constants", "ثوابتُ قيَم"),
            ("cli_flags.yaml", "messages", "رسائلُ الأدوات"),
            ("tools/repl/applets.yaml", "applets", "أُصيلاتُ الصدفة"),
            ("tools/repl/commands.yaml", "commands", "أوامرُ الصدفة"),
            ("tools/repl/errors.yaml", "errors", "أخطاءُ الصدفة"),
            ("tools/repl/messages.yaml", "messages", "رسائلُ الصدفة"),
            ("tools/repl/calendar.yaml", "weekdays", "أيّامُ الأسبوع"),
            ("tools/repl/_meta.yaml", "display_name", "أسماءُ العرض"),
            ("tools/repl/_meta.yaml", "formatting", "قواعدُ التنسيق"),
        ],
    },
    {
        "id": "09_dialects",
        "name_ar": "اللهجات",
        "name_en": "Dialects",
        "is_language_surface": True,
        "entries": [
            ("dialects/assembly.yaml", "architectures", "معماريّاتُ لهجةِ التجميع"),
            ("dialects/assembly.yaml", "operand_kinds", "أصنافُ المعاملات"),
            ("dialects/assembly.yaml", "block", "مفاتيحُ الكتلة"),
            ("dialects/assembly.yaml", "instruction_line", "مفاتيحُ سطرِ التعليمة"),
            ("dialects/assembly.yaml", "clobbers", "مفاتيحُ الإتلاف"),
            ("dialects/assembly.yaml", "label", "مفاتيحُ اللافتة"),
            ("dialects/assembly.yaml", "consumers", "مستهلكون"),
            ("dialects/assembly_mnemonics/*.yaml", "instructions", "مُختصراتُ التعليمات"),
            ("dialects/assembly_mnemonics/*.yaml", "registers", "أصنافُ المسجّلات"),
            ("dialects/assembly_mnemonics/*.yaml", "syntax", "مفاتيحُ الصياغة"),
            ("dialects/assembly_mnemonics/*.yaml", "host_macros", "ماكرو المضيف"),
            ("dialects/assembly_mnemonics/*.yaml", "triple_aliases", "مرادفاتُ الثُّلاثيّ"),
        ],
    },
]

# ═══════════════════════════════════════════════════════════════════════════
# (AR) الاستثناءاتُ المُعلَّلة: ملفٌّ لا يُعَدُّ سطحًا — ولكلٍّ سببٌ مكتوب.
#      استثناءٌ بلا سببٍ ممنوع، لأنّ «متجاهَل» يجبُ أن يظلَّ قرارًا لا نسيانًا.
# (EN) Reasoned exclusions. Every ignore carries a written reason: "ignored"
#      must stay a decision, never an oversight.
# ═══════════════════════════════════════════════════════════════════════════
IGNORED = {
    "_meta/_index.yaml": "فهرسُ مصدرِ الحقيقة — بيانٌ عن البيان لا سطحُ لغة",
    "_meta/_version.yaml": "ترقيمُ إصدارِ مصدرِ الحقيقة",
    "builtins/_index.yaml": "فهرسُ ملفّاتِ المدمجات — يُعَدُّ محتواها لا فهرسُها",
    "grammar/_notation.yaml": "ترميزُ EBNF — لغةُ وصفِ القواعدِ لا قواعدُ اللغة",
    "learning/lessons.yaml": "موادُّ تعليميّة — لا تُوسِّعُ سطحَ اللغة",
    "learning/exercises.yaml": "تمارينُ تعليميّة — لا تُوسِّعُ سطحَ اللغة",
    "tools/repl/busybox_reference.yaml": "مرجعٌ خارجيٌّ (BusyBox) للمقارنةِ لا عقدٌ لنا",
}


# ═══════════════════════════════════════════════════════════════════════════
# (AR) الاشتقاق
# ═══════════════════════════════════════════════════════════════════════════
def _load(path):
    with open(path, encoding="utf-8") as fh:
        return yaml.safe_load(fh)


def _dig(data, key_path):
    """(AR) يمشي في مسارِ مفتاحٍ منقوطٍ ويرجعُ القيمةَ أو None إن غاب."""
    node = data
    for part in key_path.split("."):
        if not isinstance(node, dict) or part not in node:
            return None
        node = node[part]
    return node


def _match_files(pattern):
    """(AR) يرجعُ مسارات نسبيّةً مرتّبةً — الترتيبُ ثابتٌ ليكونَ العدُّ حتميًّا."""
    matched = sorted(p.relative_to(TRUTH).as_posix() for p in TRUTH.glob(pattern))
    return matched


def derive(strict=True):
    """(AR) يشتقُّ كلَّ الأعداد من مصدر الحقيقة، ويحرسُ التغطيةَ في الاتّجاهَين.

    يرجعُ (facts, problems). عند strict تُرفَعُ المشكلاتُ للمُنادي ليُخفِق.
    """
    problems = []
    seen_files = set()
    facts = {"sections": {}}

    for section in SECTIONS:
        counts = {}
        section_total = 0
        for pattern, key_path, label in section["entries"]:
            files = _match_files(pattern)
            if not files:
                problems.append(
                    "مدخلٌ في الخريطةِ لا يطابقُ ملفًّا: %s (%s/%s)"
                    % (pattern, section["id"], key_path)
                )
                continue
            subtotal = 0
            hit = False
            for rel in files:
                seen_files.add(rel)
                value = _dig(_load(TRUTH / rel), key_path)
                if value is None:
                    continue
                if not isinstance(value, (list, dict)):
                    problems.append(
                        "مفتاحٌ غيرُ معدود: %s:%s (نوعُه %s)" % (rel, key_path, type(value).__name__)
                    )
                    continue
                hit = True
                subtotal += len(value)
            if not hit:
                problems.append(
                    "مفتاحٌ غائبٌ في كلِّ الملفّاتِ المطابقة: %s:%s" % (pattern, key_path)
                )
                continue
            # (AR) وسمُ المدخلِ يجمعُ الوسمَ العربيَّ ومسارَ المفتاح — فالقارئُ
            #      يرى المعنى، والحارسُ يرى المصدر.
            counts["%s (%s)" % (label, key_path)] = subtotal
            section_total += subtotal
        facts["sections"][section["id"]] = {
            "name_ar": section["name_ar"],
            "name_en": section["name_en"],
            "is_language_surface": section["is_language_surface"],
            "total": section_total,
            "counts": counts,
        }

    # (AR) حارسُ التغطية — الاتّجاهُ الأوّل: ملفٌّ في مصدرِ الحقيقةِ بلا نسبة.
    all_files = sorted(p.relative_to(TRUTH).as_posix() for p in TRUTH.rglob("*.yaml"))
    for rel in all_files:
        if rel in seen_files or rel in IGNORED:
            continue
        problems.append(
            "ملفُّ مصدرِ حقيقةٍ غيرُ منسوبٍ لقسمٍ ولا مستثنًى بسبب: %s" % rel
        )
    # (AR) الاتّجاهُ الثاني: استثناءٌ لملفٍّ لم يعُد موجودًا — قائمةُ إذنٍ متعفِّنة.
    for rel in sorted(IGNORED):
        if rel not in all_files:
            problems.append("استثناءٌ لملفٍّ غيرِ موجود (قائمةُ إذنٍ متعفِّنة): %s" % rel)
        elif rel in seen_files:
            problems.append("ملفٌّ مستثنًى ومعدودٌ في آنٍ واحد: %s" % rel)

    facts["totals"] = {
        "language_surface": sum(
            s["total"] for s in facts["sections"].values() if s["is_language_surface"]
        ),
        "all_sections": sum(s["total"] for s in facts["sections"].values()),
        "sot_files_counted": len(seen_files),
        "sot_files_ignored": len(IGNORED),
    }

    if strict and problems:
        return facts, problems
    return facts, problems


# ═══════════════════════════════════════════════════════════════════════════
# (AR) الحكمُ الخالص — دالّةٌ بلا دخل/خرج، فيمكنُ بذرُها في الاختبارِ الذاتيّ.
# ═══════════════════════════════════════════════════════════════════════════
def drift_verdict(committed, derived):
    """(AR) يرجعُ قائمةَ خلافاتٍ بين المُودَعِ والمُشتَقّ. فارغةٌ ⇒ لا انجراف."""
    drift = []
    old_sections = (committed or {}).get("sections", {})
    new_sections = (derived or {}).get("sections", {})

    for sid in sorted(set(old_sections) | set(new_sections)):
        old = old_sections.get(sid)
        new = new_sections.get(sid)
        if old is None:
            drift.append("قسمٌ جديدٌ غيرُ مُودَع: %s (المجموع %d)" % (sid, new["total"]))
            continue
        if new is None:
            drift.append("قسمٌ مُودَعٌ اختفى: %s" % sid)
            continue
        old_counts = old.get("counts", {})
        new_counts = new.get("counts", {})
        for key in sorted(set(old_counts) | set(new_counts)):
            before = old_counts.get(key)
            after = new_counts.get(key)
            if before == after:
                continue
            if before is None:
                drift.append("%s · سطحٌ جديد: %s = %s" % (sid, key, after))
            elif after is None:
                drift.append("%s · سطحٌ اختفى: %s (كان %s)" % (sid, key, before))
            else:
                delta = after - before
                drift.append(
                    "%s · %s: %s ← %s (%+d)" % (sid, key, before, after, delta)
                )
    return drift


# ═══════════════════════════════════════════════════════════════════════════
# (AR) الجردُ الحيّ — يُحسَبُ عند العرضِ فقط، ولا يدخلُ السِّجلَّ المحروس.
# ═══════════════════════════════════════════════════════════════════════════
def live_inventory():
    def count_files(rel, suffixes):
        base = PROJECT_ROOT / rel
        if not base.exists():
            return 0
        return sum(1 for p in base.rglob("*") if p.suffix in suffixes)

    def count_lines(rel, suffixes):
        base = PROJECT_ROOT / rel
        total = 0
        if not base.exists():
            return 0
        for p in base.rglob("*"):
            if p.suffix in suffixes and p.is_file():
                try:
                    with open(p, "rb") as fh:
                        total += fh.read().count(b"\n")
                except OSError:
                    pass
        return total

    cpp = {".cpp", ".h", ".hpp"}
    sad = {".ص"}
    engines = {}
    for rel in ("shared", "compiler", "interpreter", "runtime", "stdlib", "tools"):
        engines[rel] = {
            "cpp_files": count_files(rel, cpp),
            "cpp_lines": count_lines(rel, cpp),
            "sad_files": count_files(rel, sad),
        }
    tests = {}
    behavior = PROJECT_ROOT / "tests" / "behavior"
    if behavior.exists():
        for child in sorted(behavior.iterdir()):
            if child.is_dir():
                tests[child.name] = sum(1 for _ in child.rglob("*.ص"))
    return {"engines": engines, "behavior_tests": tests}


# ═══════════════════════════════════════════════════════════════════════════
# (AR) الإيداعُ والعرض
# ═══════════════════════════════════════════════════════════════════════════
def _dump(facts):
    return yaml.safe_dump(facts, allow_unicode=True, sort_keys=False, default_flow_style=False)


def write_ledger(facts, stamp):
    LEDGER_DIR.mkdir(parents=True, exist_ok=True)
    header = (
        "# سِجلُّ سطحِ لغة ص — مُشتَقٌّ آليًّا من language-truth/\n"
        "# ⚠️ لا يُحرَّرُ باليد: python tests/metrics/surface/surface_map.py --update\n"
        "# آخرُ اشتقاق / last derivation: %s\n" % stamp
    )
    # (AR) سطرُ نهايةٍ LF صراحةً: بلا ذلك يكتبُ ويندوزُ CRLF فيُودَعُ السِّجلُّ
    #      مختلفًا بايتيًّا عن نظيرِه على لينكس — ضجيجُ فروقٍ بلا معنى.
    LEDGER_FILE.write_text(header + _dump(facts), encoding="utf-8", newline="\n")

    section_ids = sorted(facts["sections"])
    row = [stamp, str(facts["totals"]["language_surface"]), str(facts["totals"]["all_sections"])]
    row += [str(facts["sections"][sid]["total"]) for sid in section_ids]
    if not HISTORY_FILE.exists():
        cols = ["date", "language_surface", "all_sections"] + section_ids
        HISTORY_FILE.write_text("\t".join(cols) + "\n", encoding="utf-8", newline="\n")
    # (AR) لا يُلحَقُ صفٌّ يطابقُ سابقَه: `--update` قد يُنادى مرارًا في اليومِ
    #      نفسِه، وصفٌّ مكرَّرٌ يجعلُ المنحنى يبدو حركةً وهو سكون.
    # (EN) Skip a row identical to the last: repeated --update in one day would
    #      otherwise make a flat curve look like motion.
    line = "\t".join(row)
    existing = HISTORY_FILE.read_text(encoding="utf-8").splitlines()
    if existing and existing[-1] == line:
        return
    with open(HISTORY_FILE, "a", encoding="utf-8", newline="\n") as fh:
        fh.write(line + "\n")


def render_markdown(facts, out_path, stamp):
    inv = live_inventory()
    lines = []
    add = lines.append
    add("# خارطة أقسام لغة ص — سطحٌ مقيسٌ لا موصوف")
    add("")
    add("> ⚠️ **هذا الملفُّ مُولَّدٌ آليًّا — لا يُحرَّرُ باليد.**")
    add("> يُعادُ توليدُه بـ`python tests/metrics/surface/surface_map.py --render <path>`،")
    add("> وكلُّ عددٍ فيه مُشتَقٌّ من `language-truth/` ومحروسٌ بـ`Lang_Surface_Check`.")
    add(">")
    add("> **تاريخُ الاشتقاق:** %s" % stamp)
    add("")
    add("---")
    add("")
    add("## المجاميع")
    add("")
    add("| المجموع | العدد |")
    add("|---|---|")
    add("| سطحُ اللغةِ (الأقسامُ اللغويّة) | %d |" % facts["totals"]["language_surface"])
    add("| سطحُ كلِّ الأقسامِ (بالخلفيّةِ والأدوات) | %d |" % facts["totals"]["all_sections"])
    add("| ملفّاتُ مصدرِ حقيقةٍ معدودة | %d |" % facts["totals"]["sot_files_counted"])
    add("| ملفّاتٌ مستثناةٌ بسببٍ مكتوب | %d |" % facts["totals"]["sot_files_ignored"])
    add("")
    add("---")
    add("")
    add("## الأقسام")
    add("")
    for sid in sorted(facts["sections"]):
        sec = facts["sections"][sid]
        mark = "لغويّ" if sec["is_language_surface"] else "غيرُ لغويّ"
        add("### %s — %s  ·  **%d**  ·  _%s_" % (sid, sec["name_ar"], sec["total"], mark))
        add("")
        add("| السطح | العدد |")
        add("|---|---|")
        for key in sorted(sec["counts"], key=lambda k: (-sec["counts"][k], k)):
            add("| %s | %d |" % (key, sec["counts"][key]))
        add("")
    add("---")
    add("")
    add("## الجردُ الحيّ (غيرُ محروس — يتغيّرُ كلَّ إيداع)")
    add("")
    add("| المسار | ملفّاتُ C++ | أسطرُ C++ | ملفّاتُ ص |")
    add("|---|---|---|---|")
    for rel in sorted(inv["engines"]):
        e = inv["engines"][rel]
        add("| `%s` | %d | %d | %d |" % (rel, e["cpp_files"], e["cpp_lines"], e["sad_files"]))
    add("")
    add("| مجموعةُ اختبارِ السلوك | برامجُ `.ص` |")
    add("|---|---|")
    for name in sorted(inv["behavior_tests"], key=lambda k: -inv["behavior_tests"][k]):
        add("| `%s` | %d |" % (name, inv["behavior_tests"][name]))
    add("")
    add("---")
    add("")
    add("**لماذا فُصِلَ الجردُ الحيُّ عن السطحِ المحروس:** السطحُ يتغيّرُ بفعلٍ متعمَّدٍ")
    add("في مصدرِ الحقيقة، فيصحُّ أن يحرسَه حارسٌ يحمرُّ على أيِّ خلاف. أمّا أسطرُ")
    add("الكودِ وعددُ الاختباراتِ فتتغيّرُ في كلِّ إيداع، وحارسٌ عليها يحمرُّ دائمًا")
    add("ثمّ يُعطَّل ثمّ يخضرُّ بلا قياس.")
    add("")
    Path(out_path).parent.mkdir(parents=True, exist_ok=True)
    Path(out_path).write_text("\n".join(lines), encoding="utf-8", newline="\n")


# ═══════════════════════════════════════════════════════════════════════════
# (AR) الاختبارُ الذاتيُّ — بذورٌ متعاكسةٌ على الحكمِ الخالص.
#      ⚠️ لا يحتاجُ مصدرَ حقيقةٍ ولا سِجلًّا: يُغذّي `drift_verdict` بقياساتٍ
#      مُصطنَعةٍ ويشترطُ **أحكامًا متعاكسة** — سكوتًا على التطابق، وإخفاقًا على
#      كلِّ صورةٍ من صورِ الانجرافِ الأربع. لأنّ حارسًا لا تُبرهَنُ عضّتُه
#      حارسٌ لا يعضّ.
# ═══════════════════════════════════════════════════════════════════════════
def self_test():
    def facts(sections):
        return {"sections": sections}

    base = facts({"01_core": {"total": 10, "counts": {"أ (a)": 4, "ب (b)": 6}}})
    cases = [
        ("تطابقٌ تامّ ⇒ سكوت", base, base, 0),
        (
            "عددٌ نما ⇒ إخفاق",
            base,
            facts({"01_core": {"total": 11, "counts": {"أ (a)": 5, "ب (b)": 6}}}),
            1,
        ),
        (
            "عددٌ نقص ⇒ إخفاق",
            base,
            facts({"01_core": {"total": 9, "counts": {"أ (a)": 3, "ب (b)": 6}}}),
            1,
        ),
        (
            "سطحٌ جديدٌ ظهر ⇒ إخفاق",
            base,
            facts({"01_core": {"total": 13, "counts": {"أ (a)": 4, "ب (b)": 6, "ج (c)": 3}}}),
            1,
        ),
        (
            "سطحٌ اختفى ⇒ إخفاق",
            base,
            facts({"01_core": {"total": 4, "counts": {"أ (a)": 4}}}),
            1,
        ),
        (
            "قسمٌ جديدٌ كامل ⇒ إخفاق",
            base,
            facts(
                {
                    "01_core": {"total": 10, "counts": {"أ (a)": 4, "ب (b)": 6}},
                    "02_types": {"total": 7, "counts": {"د (d)": 7}},
                }
            ),
            1,
        ),
        ("قسمٌ اختفى ⇒ إخفاق", facts({"01_core": base["sections"]["01_core"], "02_x": {"total": 1, "counts": {"هـ (e)": 1}}}), base, 1),
        (
            "مجموعٌ تغيّرَ والتفاصيلُ ثابتة ⇒ سكوت (المجموعُ مُشتَقٌّ لا مصدر)",
            base,
            facts({"01_core": {"total": 999, "counts": {"أ (a)": 4, "ب (b)": 6}}}),
            0,
        ),
    ]
    ok = True
    for name, committed, derived, expect_nonzero in cases:
        drift = drift_verdict(committed, derived)
        got = 1 if drift else 0
        mark = "✅" if got == expect_nonzero else "❌"
        if got != expect_nonzero:
            ok = False
        print("  %s %s — خلافات: %d" % (mark, name, len(drift)))

    # (AR) بذرةٌ على حارسِ التغطية: استثناءٌ لملفٍّ غيرِ موجودٍ يجبُ أن يُكتشَف.
    strays = [rel for rel in IGNORED if not (TRUTH / rel).exists()]
    if strays:
        ok = False
        print("  ❌ استثناءاتٌ لملفّاتٍ غيرِ موجودة: %s" % ", ".join(strays))
    else:
        print("  ✅ كلُّ الاستثناءاتِ تشيرُ إلى ملفّاتٍ قائمة")

    if not ok:
        print("❌ الاختبارُ الذاتيُّ لمِقياسِ السطحِ أخفق")
        return 1
    print("✅ الاختبارُ الذاتيُّ لمِقياسِ السطحِ اجتاز")
    return 0


def _stamp():
    """(AR) تاريخُ آخرِ إيداعٍ في git — لا ساعةُ الجدار، فالتوليدُ يبقى حتميًّا."""
    try:
        out = subprocess.run(
            ["git", "log", "-1", "--format=%cs"],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=10,
        )
        if out.returncode == 0 and out.stdout.strip():
            return out.stdout.strip()
    except (OSError, subprocess.SubprocessError):
        pass
    return "unknown"


def main():
    parser = argparse.ArgumentParser(description="مِقياسُ سطحِ لغة ص")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--check", action="store_true", help="مقارنةُ المُشتَقِّ بالمُودَع (CI)")
    group.add_argument("--update", action="store_true", help="إعادةُ الاشتقاقِ والإيداع")
    group.add_argument("--self-test", action="store_true", help="بذورُ الحكمِ المتعاكسة")
    parser.add_argument("--render", metavar="OUT", help="كتابةُ الخارطةِ بصيغةِ Markdown")
    args = parser.parse_args()

    if args.self_test:
        return self_test()

    print("═══ مِقياسُ سطحِ لغة ص — اشتقاقٌ من مصدرِ الحقيقة ═══")
    facts, problems = derive()
    if problems:
        print("❌ حارسُ التغطيةِ أخفق — %d مشكلة:" % len(problems))
        for p in problems:
            print("   • %s" % p)
        return 1
    print(
        "  ✅ التغطية: %d ملفَّ مصدرِ حقيقةٍ معدود · %d مستثنًى بسبب"
        % (facts["totals"]["sot_files_counted"], facts["totals"]["sot_files_ignored"])
    )
    for sid in sorted(facts["sections"]):
        sec = facts["sections"][sid]
        print("  📏 %-28s %5d" % (sid, sec["total"]))
    print("  ── سطحُ اللغة: %d · كلُّ الأقسام: %d"
          % (facts["totals"]["language_surface"], facts["totals"]["all_sections"]))

    stamp = _stamp()

    if args.render:
        render_markdown(facts, args.render, stamp)
        print("  📝 كُتبت الخارطة: %s" % args.render)

    if args.update:
        write_ledger(facts, stamp)
        print("  💾 أُودِعَ السِّجلّ: %s" % LEDGER_FILE)
        return 0

    # (AR) الافتراضيُّ هو الفحص — فلا يمرُّ استدعاءٌ ساذجٌ بلا حكم.
    if not LEDGER_FILE.exists():
        print("❌ لا سِجلَّ مُودَعًا — شغّل --update أوّلًا")
        return 1
    committed = _load(LEDGER_FILE)
    drift = drift_verdict(committed, facts)
    if drift:
        print("❌ انجرافٌ عن السِّجلِّ المُودَع — %d خلافًا:" % len(drift))
        for d in drift:
            print("   • %s" % d)
        print("   ↳ إن كان التوسُّعُ مقصودًا: --update ثمّ أودِع السِّجلَّ مع التغيير")
        return 1
    print("✅ لا انجراف — الخارطةُ تطابقُ مصدرَ الحقيقة")
    return 0


if __name__ == "__main__":
    sys.exit(main())
