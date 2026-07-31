#!/usr/bin/env python3
# ======================================================================
# x.py — منسّق البناء الموحَّد للغة ص / Unified build orchestrator
# ======================================================================
# (AR) ينفّذ المرحلة 0 من RFC «قلب موحَّد بحدود داخلية صارمة» (sadlang-rfcs#10):
#      البوّابة الوحيدة للبناء. يبني المفسّر (sad-run) والمترجم (sad-build)
#      **ذرّيًّا في نفس التهيئة**، ثم يثبّت الثنائيَّين في `dist/<Config>/` مع
#      بصمة SHA-256 لكلٍّ منهما. هذا يقتل عثرتين موثَّقتين:
#        1. تذبذب Debug/Release (مفسّر بتهيئة ومترجم بأخرى) — مستحيل الآن لأن
#           الأمر الواحد يبني الهدفين معًا لكل تهيئة، وإن فشل أحدهما فشل الكل.
#        2. الثنائيّ البائت — `verify` يعيد حساب البصمة فيكشف أيّ مخرَج قديم.
#
# (EN) Implements Phase 0 of the RFC "unified core with strict internal
#      boundaries" (sadlang-rfcs#10): the single build gateway. Builds the
#      interpreter (sad-run) and compiler (sad-build) ATOMICALLY in the SAME
#      configuration, then stages both binaries into `dist/<Config>/` with a
#      SHA-256 fingerprint each. Kills two documented gotchas: Debug/Release
#      drift and the stale-binary trap.
#
# الاستخدام / Usage:
#   python x.py build --config Debug,Release   # بناء الهدفين معًا لكل تهيئة
#   python x.py verify --config Debug          # تحقّق من تطابق dist والبصمات
#   python x.py gen                            # توليد المصدر من language-truth/ (YAML)
#   python x.py gen --check                    # حارس انجراف: يفشل إن انحرف المولَّد (CI)
#   python x.py test --config Debug -- --level P0   # بناء (إن لزم) ثم runner
#   python x.py conformance                    # فاحص مطابقة القواعد القائم
#   python x.py clean                          # حذف build/ و dist/
# ======================================================================

from __future__ import annotations

import argparse
import hashlib
import json
import os
import platform
import shutil
import subprocess
import sys
import tempfile
from datetime import datetime, timezone
from pathlib import Path

# (AR) ضمان إخراج عربيّ سليم على طرفية ويندوز (cp1252 افتراضًا) وأنابيب CI.
# (EN) Ensure correct Arabic output on Windows consoles (cp1252 default) and CI pipes.
for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8", errors="replace")  # type: ignore[union-attr]
    except (AttributeError, ValueError):
        pass

# ──────────────────────────────────────────────────────────────────────
# ثوابت المشروع / Project constants
# ──────────────────────────────────────────────────────────────────────
ROOT = Path(__file__).resolve().parent
BUILD_DIR = ROOT / "build"
DIST_DIR = ROOT / "dist"

# (AR) الهدفان اللذان يجب أن يُبنيا معًا دائمًا — قلب ضمان التطابق السلوكيّ.
# (EN) The two targets that must ALWAYS build together — the heart of the guard.
ENGINES = {
    "sad-run": "المفسّر / interpreter",
    "sad-build": "المترجم / compiler",
}

EXE = ".exe" if platform.system() == "Windows" else ""
VALID_CONFIGS = ("Debug", "Release", "RelWithDebInfo", "MinSizeRel")

# ──────────────────────────────────────────────────────────────────────
# (AR) نطاقات التوليد من مصدر الحقيقة — المرحلة 1 من sadlang-rfcs#10
#      language-truth/*.yaml هو **مصدر الحقيقة الوحيد**؛ الملفّات المولَّدة
#      أدناه مشتقّات خالصة تبقى متعقَّبة في git (نمط Go `go generate`)، و
#      `x.py gen --check` حارس انجراف يفشل إن لم تَعُد مطابقةً لِما يولّده YAML.
#
#      اثنا عشر نطاقًا مستقلًّا (أشقّاء بلا اعتماد متبادل) فيُستدعى مولّد كلٍّ مباشرةً —
#      لا حاجة إلى تهيئة CMake الثقيلة لبوّابة فحصٍ يجب أن تكون خفيفة. أربعةٌ منها
#      (types/keywords/builtins/error_messages) تطابق هدف CMake `sad_all_codegen`
#      (cmake/codegen.cmake)؛ ونطاقات الواجهة الستّة (ui_props/ui_modifiers +
#      مفردات ui_animations/ui_easings/ui_events + ui_colors) **تُولَّد عبر x.py
#      فقط** — هيدرات ورقيّة مُلتزَمة داخل مكتبة الرسومات الطرفيّة، بلا هدف CMake
#      (فلا يعيد بناءُ CMake توليدَها؛ حارس الانجراف وحده يمسكها). السلوك
#      (write_if_changed) يفرض أسطر LF فالمقارنة بالبايت آمنة عبر المنصّات.
# (EN) Source-of-truth codegen domains — Phase 1 of sadlang-rfcs#10. The YAML in
#      language-truth/ is the SINGLE source of truth; the generated files stay
#      tracked in git (Go `go generate` pattern) and `x.py gen --check` is the
#      drift guard. Twelve independent siblings; each generator is invoked directly —
#      no heavy CMake configure for a check that must stay light. Four
#      (types/keywords/builtins/error_messages) mirror CMake's `sad_all_codegen`;
#      the five UI domains (ui_props/ui_modifiers + the ui_animations/ui_easings/
#      ui_events vocabularies) are x.py-ONLY — papered headers committed inside the
#      leaf graphics library, with no CMake target (so a CMake build does NOT
#      regenerate them; only the drift guard catches edits).
#
# (AR) كلّ نطاق: مولّده، مجلّد الإخراج في الشجرة، ملفّاته المولَّدة، ودالة تبني
#      وسائط CLI بدلالة مجلّد الإخراج (شجرة المصدر لـ`gen`، مجلّد مؤقّت لـ`--check`).
CODEGEN_DOMAINS = (
    {
        "name": "types",
        "script": "gen_types.py",
        "out_dir": "shared/types/generated",
        "outputs": ("sad_type_kind_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/types.yaml",
            "--schema", "language-truth/_schemas/type.schema.json",
            "--header", f"{d}/sad_type_kind_generated.h",
            "--quiet",
        ],
    },
    {
        "name": "keywords",
        "script": "gen_keywords.py",
        "out_dir": "shared/lexer/generated",
        "outputs": ("keywords_generated.h", "keywords_generated.cpp"),
        "args": lambda d: [
            "--yaml", "language-truth/keywords.yaml",
            "--schema", "language-truth/_schemas/keywords.schema.json",
            "--header", f"{d}/keywords_generated.h",
            "--source", f"{d}/keywords_generated.cpp",
            "--quiet",
        ],
    },
    {
        "name": "builtins",
        "script": "gen_builtins_registry.py",
        "out_dir": "shared/builtins/generated",
        "outputs": ("builtin_registry_generated.h",),
        "args": lambda d: [
            "--yaml-dir", "language-truth/builtins",
            "--index", "language-truth/builtins/_index.yaml",
            "--out-h", f"{d}/builtin_registry_generated.h",
            "--quiet",
        ],
    },
    {
        # (AR) مفاتيح خصائص عناصر واجهة SadUI (عنوان/محتوى/…) — مصدر الحقيقة الوحيد.
        #      يُولَّد إلى هيدر مُلتزَم داخل مكتبة الرسومات الطرفيّة (لا اعتماد ربط جديد).
        "name": "ui_props",
        "script": "gen_ui_props.py",
        "out_dir": "features/graphics/core/include/sad_ui",
        "outputs": ("prop_keys.h",),
        "args": lambda d: [
            "--yaml", "language-truth/ui_props.yaml",
            "--schema", "language-truth/_schemas/ui_props.schema.json",
            "--header", f"{d}/prop_keys.h",
            "--quiet",
        ],
    },
    {
        # (AR) طرق (معدّلات) عناصر واجهة SadUI (ابن/عند_*/حرّك/…) — مصدر الحقيقة الوحيد.
        "name": "ui_modifiers",
        "script": "gen_ui_modifiers.py",
        "out_dir": "features/graphics/core/include/sad_ui",
        "outputs": ("ui_modifiers.h",),
        "args": lambda d: [
            "--yaml", "language-truth/ui_modifiers.yaml",
            "--schema", "language-truth/_schemas/ui_modifiers.schema.json",
            "--header", f"{d}/ui_modifiers.h",
            "--quiet",
        ],
    },
    {
        # (AR) مفردات حركة SadUI النصّيّة (ظهور/انزلاق_يمين/…) — قانونيّ بلا تشكيل.
        #      X-macro مولَّد يستهلكه ir.h في المكتبة الطرفيّة (لا اعتماد ربط جديد).
        "name": "ui_animations",
        "script": "gen_ui_vocab.py",
        "out_dir": "features/graphics/core/include/sad_ui/generated",
        "outputs": ("animation_vocab_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/ui_animations.yaml",
            "--schema", "language-truth/_schemas/ui_vocab.schema.json",
            "--header", f"{d}/animation_vocab_generated.h",
            "--quiet",
        ],
    },
    {
        # (AR) مفردات منحنيات تسارع SadUI (خطي/مرن/…) — قانونيّ بلا تشكيل.
        "name": "ui_easings",
        "script": "gen_ui_vocab.py",
        "out_dir": "features/graphics/core/include/sad_ui/generated",
        "outputs": ("easing_vocab_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/ui_easings.yaml",
            "--schema", "language-truth/_schemas/ui_vocab.schema.json",
            "--header", f"{d}/easing_vocab_generated.h",
            "--quiet",
        ],
    },
    {
        # (AR) مفردات أحداث SadUI (عند_النقر/عند_التغيير/…) — قانونيّ بلا تشكيل.
        #      يستهلكه types.cpp (stringToIREventType) فيتوحّد المحرّكان في getEvents().
        "name": "ui_events",
        "script": "gen_ui_vocab.py",
        "out_dir": "features/graphics/core/include/sad_ui/generated",
        "outputs": ("event_vocab_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/ui_events.yaml",
            "--schema", "language-truth/_schemas/ui_vocab.schema.json",
            "--header", f"{d}/event_vocab_generated.h",
            "--quiet",
        ],
    },
    {
        # (AR) أطوار انتشار حدث SadUI (لا_انتشار/فقاعة/التقاط/كلاهما) — قانونيّ.
        #      يستهلكه types.cpp (stringToEventPropagation) فيتوحّد المحرّكان في
        #      معنى معدّل `.تفرع("…")`.
        "name": "ui_propagation",
        "script": "gen_ui_vocab.py",
        "out_dir": "features/graphics/core/include/sad_ui/generated",
        "outputs": ("propagation_vocab_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/ui_propagation.yaml",
            "--schema", "language-truth/_schemas/ui_vocab.schema.json",
            "--header", f"{d}/propagation_vocab_generated.h",
            "--quiet",
        ],
    },
    {
        # (AR) ألوان SadUI (تعداد `ألوان` المدمَج + جدول hex/RGBA موحَّد) — قانونيّ.
        #      مخرَجان: بادئة التعداد (تُحقَن في المحرّكين) + جداول المكتبة الموحَّدة.
        "name": "ui_colors",
        "script": "gen_ui_colors.py",
        "out_dir": "features/graphics/core/include/sad_ui/generated",
        "outputs": ("color_prelude_generated.h", "color_table_generated.h"),
        "args": lambda d: [
            "--yaml", "language-truth/ui_colors.yaml",
            "--schema", "language-truth/_schemas/ui_colors.schema.json",
            "--prelude", f"{d}/color_prelude_generated.h",
            "--table", f"{d}/color_table_generated.h",
            "--quiet",
        ],
    },
    {
        "name": "error_messages",
        "script": "gen_error_messages.py",
        "out_dir": "shared/errors/generated",
        "outputs": ("error_messages_generated.h", "error_messages_generated.cpp"),
        "args": lambda d: [
            "--yaml-dir", "language-truth/errors",
            "--schema", "language-truth/_schemas/error.schema.json",
            "--enum-header", "shared/errors/include/error_codes.h",
            "--header", f"{d}/error_messages_generated.h",
            "--source", f"{d}/error_messages_generated.cpp",
            "--quiet",
        ],
    },
    {
        # (AR) نظام «مصدر حقيقة الأدوات» (Tool SoT) — مستقلّ عن مصدر حقيقة اللغة.
        #      أوّل أداة: sad-repl (أخطاء الأداة + رسائل واجهتها + أوامرها). الناتج
        #      مُلتزَم داخل tools/repl/generated ويُجمَّع مباشرةً (بلا هدف CMake؛ حارس
        #      الانجراف يكفي — كنمط ui_*). كلّ أداةٍ لاحقة تُضيف نطاقًا مماثلًا.
        # (EN) Tools' Source-of-Truth system — independent of the language SoT. First
        #      tool: sad-repl (tool errors + UI messages + commands). Output committed
        #      under tools/repl/generated and compiled directly (no CMake target; the
        #      drift guard suffices — like the ui_* domains). Each future tool adds a peer.
        "name": "tool_repl",
        "script": "gen_tool_sot.py",
        "out_dir": "tools/repl/generated",
        "outputs": ("repl_sot_generated.h", "repl_sot_generated.cpp"),
        "args": lambda d: [
            "--tool-dir", "language-truth/tools/repl",
            "--schema-dir", "language-truth/tools/_schemas",
            "--out-h", f"{d}/repl_sot_generated.h",
            "--out-cpp", f"{d}/repl_sot_generated.cpp",
            "--quiet",
        ],
    },
    {
        # (AR) توثيق قواعد المحلّل المولَّد من language-truth/grammar/*.yaml —
        #      Markdown مُلتزَم تحت docs/parser_rule/_generated (8 طبقات + فهرس).
        #      كان للمولّد `--check` خاصّ خارج هذه البوّابة ⇒ خطر وثائق بائتة؛
        #      بضمّه نطاقًا يعيد `gen` توليدَه ويمسك `gen --check` انحرافَه.
        # (EN) Parser-grammar docs generated from language-truth/grammar/*.yaml —
        #      committed Markdown under docs/parser_rule/_generated (8 layers +
        #      INDEX). Its standalone --check lived outside this gate (stale-docs
        #      risk); as a domain, `gen` regenerates and `gen --check` guards it.
        "name": "parser_grammar_docs",
        "script": "gen_parser_grammar_docs.py",
        "out_dir": "docs/parser_rule/_generated",
        "outputs": (
            "INDEX.md",
            "00_program.md",
            "10_statements.md",
            "20_declarations.md",
            "30_oop.md",
            "40_expressions.md",
            "50_patterns.md",
            "60_advanced.md",
            "70_lexical.md",
        ),
        "args": lambda d: ["--out-dir", d, "--quiet"],
    },
    {
        # (AR) أعلام مترجم لغة ص الطويلة (--) — المصدر الوحيد للحقيقة. اسم عربيّ
        #      قانونيّ وحيد لكلّ علم، بلا مرادفات ولا توافق خلفيّ. يُولَّد جدول
        #      constexpr مُلتزَم يستهلكه المحلِّل بدل السلاسل الحرّة (يسدّ خرق «لا
        #      نصّ في الكود» في طبقة الـCLI؛ الأعلام القصيرة القياسيّة تبقى عرفًا).
        # (EN) Compiler long (--) flags — single source of truth. One canonical
        #      Arabic name per flag, no aliases, no backward compat. Emits a
        #      committed constexpr table the driver consumes instead of raw strings.
        "name": "cli_flags",
        "script": "gen_cli_flags.py",
        "out_dir": "shared/cli/include",
        "outputs": ("cli_flags_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/cli_flags.yaml",
            "--schema", "language-truth/_schemas/cli_flags.schema.json",
            "--header", f"{d}/cli_flags_generated.h",
            "--quiet",
        ],
    },
    {
        # (AR) لهجة التجميع العربيّ (م١ RFC اللهجات الأصيلة) — جدول منمنمات/سجلّات/
        #      تلويث/كلمات الكتلة مولَّد من language-truth/dialects/. هيدر واحد
        #      constexpr مُلتزَم يستهلكه المحلّل والفاحص الدلاليّ والمترجم sadc (لا
        #      اعتماد ربط جديد — كنمط ui_*؛ يُضاف مجلّده لـinclude_directories الجذر).
        # (EN) Arabic assembly dialect (M1 of the native-dialects RFC) — mnemonic/
        #      register/clobber/block-keyword table generated from language-truth/
        #      dialects/. One committed constexpr header consumed by parser, semantic
        #      checker, and sadc.
        "name": "asm_dialect",
        "script": "gen_asm_dialect.py",
        "out_dir": "shared/dialects/generated",
        "outputs": ("asm_dialect_generated.h",),
        "args": lambda d: [
            "--dialect", "language-truth/dialects/assembly.yaml",
            "--mnemonics", "language-truth/dialects/assembly_mnemonics/i686.yaml",
            "--dialect-schema", "language-truth/_schemas/dialect.schema.json",
            "--mnemonics-schema", "language-truth/_schemas/assembly_mnemonics.schema.json",
            "--header", f"{d}/asm_dialect_generated.h",
            "--quiet",
        ],
    },
    {
        # (AR) توثيق Markdown لأعلام سطر الأوامر — مولَّد من المصدر الوحيد نفسه،
        #      مجموعًا حسب المحرّك المستهلِك. بضمّه نطاقًا يمسك `gen --check`
        #      انحرافَه فلا يصير بائتًا عند تعديل الأعلام.
        # (EN) CLI-flags Markdown docs generated from the same SoT, grouped by
        #      consuming engine. As a domain, `gen --check` guards it against drift.
        "name": "cli_flags_docs",
        "script": "gen_cli_flags_docs.py",
        "out_dir": "docs/cli_flags/_generated",
        "outputs": ("cli_flags.ar.md",),
        "args": lambda d: [
            "--yaml", "language-truth/cli_flags.yaml",
            "--schema", "language-truth/_schemas/cli_flags.schema.json",
            "--out-dir", d,
            "--quiet",
        ],
    },
    {
        # (AR) جدول ترميز الخلفيّة السياديّة (x86-64، variable) — مولَّد من SoT إلى
        #      هيدر C++ يستهلكه المحرّك (lookupEncSpec). بضمّه نطاقًا يمسك
        #      `gen --check` انحرافَه عن instructions.yaml (بوّابة فحص توليد الكود).
        # (EN) Sovereign backend encoding table (x86-64 variable) generated from SoT
        #      into a C++ header consumed by the encoder. As a domain, `gen --check`
        #      guards it against drift from instructions.yaml.
        "name": "backend_encoding_x86_64",
        "script": "gen_backend_encoding.py",
        "out_dir": "compiler/include/backend/native/generated",
        "outputs": ("x86_64_encoding_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/backend/x86_64/instructions.yaml",
            "--schema", "language-truth/_schemas/backend_encoding.schema.json",
            "--header", f"{d}/x86_64_encoding_generated.h",
            "--quiet",
        ],
    },
    {
        # (AR) الهدف الثاني: جدول ترميز AArch64 (عائلة fixed32) من نفس المولّد —
        #      يُبرهِن عموميّةَ النهج الجدوليّ عبر عائلتَي الترميز بمولّدٍ واحد.
        # (EN) Second target: AArch64 (fixed32) encoding table from the same
        #      generator — proving the table-driven approach across both families.
        "name": "backend_encoding_arm64",
        "script": "gen_backend_encoding.py",
        "out_dir": "compiler/include/backend/native/generated",
        "outputs": ("arm64_encoding_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/backend/arm64/instructions.yaml",
            "--schema", "language-truth/_schemas/backend_encoding.schema.json",
            "--header", f"{d}/arm64_encoding_generated.h",
            "--quiet",
        ],
    },
    {
        # (AR) وسومُ تشخيصِ التخفيض الأصليّ: حمولاتُ {detail} التي يمرّرها المخفّضان
        #      (x86/ARM64) لـfail() — تُوحَّد ثوابتَ مسمّاةً مولَّدة من SoT بدل حرفيّاتٍ
        #      خام، ويمسك `gen --check` انحرافَها عن native_diagnostics.yaml.
        # (EN) Native-lowering diagnostic tags: the {detail} payloads both lowerers
        #      pass to fail(), single-sourced as SoT-generated named constants
        #      (no raw literals); `gen --check` guards drift from the YAML.
        "name": "backend_native_diagnostics",
        "script": "gen_native_diagnostics.py",
        "out_dir": "compiler/include/backend/native/generated",
        "outputs": ("native_diagnostics_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/backend/native_diagnostics.yaml",
            "--header", f"{d}/native_diagnostics_generated.h",
            "--quiet",
        ],
    },
)

# (AR) حرّاس فحص خالصون (لا مخرجات مولَّدة) يعملون ضمن `x.py gen --check` فقط —
#      يكمّلون حارس الانجراف بفحوص اتّساق بين مصدر الحقيقة وبقيّة الشجرة:
#        • token_catalog: تعداد KEYWORD_* في token.h ↔ كتالوج keywords.yaml
#          (مع قائمة «دَين موروث» صريحة تنكمش فقط — انظر الحارس نفسه).
#        • rules_matrix: سلامة وسوم @rule في tests/behavior/rules_matrix ↔ قواعد
#          language-truth/grammar (بنمط --check لبقيّة المولّدات).
#      كلّ حارس: سكربت في scripts/codegen/ يعيد rc=0 سليمًا وغير صفر عند الانجراف.
# (EN) Pure check-only guards (no generated outputs) that run inside
#      `x.py gen --check` only, complementing the drift guard with SoT-vs-tree
#      consistency checks. Each guard is a scripts/codegen/ script returning
#      rc=0 when clean, non-zero on drift.
SOT_CHECK_GUARDS = (
    {
        "name": "token_catalog",
        "script": "check_token_catalog.py",
        "args": (),
    },
    {
        "name": "rules_matrix",
        "script": "gen_rules_matrix.py",
        "args": ("--check",),
    },
    {
        # (AR) اتّساق خصائص الواجهة: primary_prop لكلّ عنصر ⇐ مفتاح في ui_props.yaml.
        # (EN) UI-property consistency: each widget primary_prop maps to a ui_props key.
        "name": "ui_props_consistency",
        "script": "check_ui_props_consistency.py",
        "args": (),
    },
    {
        # (AR) لا مفاتيح خصائص خام في features/graphics — استعمل ثوابت props:: المولَّدة.
        # (EN) No raw property-key literals in graphics — use generated props:: constants.
        "name": "no_raw_props",
        "script": "check_no_raw_props.py",
        "args": (),
    },
    {
        # (AR) لا « #» غير مقتبس في حقول وصف language-truth (يبتره YAML تعليقًا صامتًا).
        # (EN) No unquoted ` #` in language-truth description fields (YAML truncates it).
        "name": "yaml_desc_hash",
        "script": "check_yaml_desc_hash.py",
        "args": (),
    },
    {
        # (AR) تفرّد canonical عبر نطاقات المدمجات — المسجّل يفهرس بالاسم وحده،
        #      فالتكرار عبر النطاقات يحسمه ترتيب الاستيراد بصمت (تصادم «ضم»).
        # (EN) Builtin canonical uniqueness across namespaces — the registry keys
        #      by name only; cross-namespace duplicates are import-order races.
        "name": "builtins_canonical_unique",
        "script": "check_builtins_canonical_unique.py",
        "args": (),
    },
    {
        # (AR) تغطية معجم آبلتات sad-repl مقابل مقام busybox defconfig المثبَّت: لا
        #      exec ميّت (ليس آبلتًا حقيقيًّا) ولا فجوة (آبلت defconfig بلا اسم عربيّ).
        # (EN) sad-repl applet lexicon coverage vs the pinned busybox defconfig
        #      denominator: no dead exec, no uncovered defconfig applet.
        "name": "applet_coverage",
        "script": "check_applet_coverage.py",
        "args": (),
    },
)


# ──────────────────────────────────────────────────────────────────────
# أدوات مساعدة / Helpers
# ──────────────────────────────────────────────────────────────────────
def _log(msg: str) -> None:
    print(f"[x.py] {msg}", flush=True)


def _fail(msg: str) -> "NoReturn":  # type: ignore[name-defined]
    print(f"[x.py] ✗ خطأ / error: {msg}", file=sys.stderr, flush=True)
    sys.exit(1)


def _run(cmd: list[str]) -> None:
    """(AR) ينفّذ أمرًا ويفشل بصوتٍ عالٍ. / Run a command, fail loudly."""
    _log("» " + " ".join(str(c) for c in cmd))
    result = subprocess.run(cmd, cwd=ROOT)
    if result.returncode != 0:
        _fail(f"الأمر فشل برمز {result.returncode}: {' '.join(map(str, cmd))}")


def _norm(b: bytes) -> bytes:
    """(AR) يطبّع نهايات الأسطر للمقارنة: git يعامل الملفّات المولَّدة text eol=lf
        فيُطبّعها على القرص، وحارس الانجراف يقارن **المحتوى** لا سياسة الأسطر — كي
        لا يُسبّب مولّدٌ يكتب CRLF على ويندوز انحرافًا كاذبًا.
    (EN) Normalize EOLs for comparison: the drift guard compares content, not EOL
        policy (git treats the generated files as text eol=lf)."""
    return b.replace(b"\r\n", b"\n")


def _sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def _parse_configs(raw: str) -> list[str]:
    configs = [c.strip() for c in raw.split(",") if c.strip()]
    if not configs:
        _fail("لم تُحدَّد أيّ تهيئة / no configuration given")
    for c in configs:
        if c not in VALID_CONFIGS:
            _fail(f"تهيئة غير معروفة / unknown configuration: {c} "
                  f"(المتاح / valid: {', '.join(VALID_CONFIGS)})")
    return configs


def _is_multi_config() -> bool:
    """(AR) هل المولِّد متعدّد التهيئات (مثل Visual Studio)؟ يُقرأ من CMakeCache.
    (EN) Is this a multi-config generator (e.g. Visual Studio)?"""
    cache = BUILD_DIR / "CMakeCache.txt"
    if not cache.exists():
        # (AR) على ويندوز الافتراضي VS متعدد التهيئات. / Default on Windows is VS.
        return platform.system() == "Windows"
    for line in cache.read_text(encoding="utf-8", errors="ignore").splitlines():
        if line.startswith("CMAKE_CONFIGURATION_TYPES"):
            return True
    return False


def _generator() -> str:
    cache = BUILD_DIR / "CMakeCache.txt"
    if cache.exists():
        for line in cache.read_text(encoding="utf-8", errors="ignore").splitlines():
            if line.startswith("CMAKE_GENERATOR:"):
                return line.split("=", 1)[1].strip()
    return "(default)"


def _find_binary(name: str, config: str) -> Path | None:
    """(AR) يحدّد موقع المخرَج بعد البناء عبر المرشّحات المعروفة.
    (EN) Locate the produced binary across known candidate locations."""
    candidates = [
        BUILD_DIR / "bin" / config / f"{name}{EXE}",  # multi-config (VS)
        BUILD_DIR / "bin" / f"{name}{EXE}",            # single-config
        BUILD_DIR / config / "bin" / f"{name}{EXE}",
    ]
    for c in candidates:
        if c.exists():
            return c
    return None


# ──────────────────────────────────────────────────────────────────────
# الأوامر / Commands
# ──────────────────────────────────────────────────────────────────────
def cmd_configure(args: argparse.Namespace) -> None:
    """(AR) تهيئة CMake إن لم تكن موجودة. / Configure CMake if needed."""
    _configure_if_needed()


def _configure_if_needed() -> None:
    if (BUILD_DIR / "CMakeCache.txt").exists():
        return
    _log("تهيئة CMake لأول مرة / first-time CMake configure …")
    _run(["cmake", "-S", str(ROOT), "-B", str(BUILD_DIR)])


def _stage(config: str) -> dict:
    """(AR) ينسخ الثنائيَّين إلى dist/<config>/ ويكتب manifest بالبصمات.
    (EN) Copy both engines to dist/<config>/ and write a fingerprint manifest."""
    dest = DIST_DIR / config
    dest.mkdir(parents=True, exist_ok=True)
    manifest = {
        "config": config,
        "generated_utc": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "generator": _generator(),
        "binaries": {},
    }
    for name in ENGINES:
        src = _find_binary(name, config)
        if src is None:
            _fail(f"لم يُعثر على مخرَج الهدف / target output not found: "
                  f"{name} ({config}). هل اكتمل البناء؟ / did the build complete?")
        out = dest / f"{name}{EXE}"
        shutil.copy2(src, out)
        manifest["binaries"][name] = {
            "file": out.name,
            "sha256": _sha256(out),
            "size": out.stat().st_size,
            "source": str(src.relative_to(ROOT)).replace("\\", "/"),
        }
    (dest / "manifest.json").write_text(
        json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )
    return manifest


def cmd_build(args: argparse.Namespace) -> None:
    configs = _parse_configs(args.config)

    if args.clean and BUILD_DIR.exists():
        _log("حذف مجلد البناء / removing build dir …")
        shutil.rmtree(BUILD_DIR, ignore_errors=True)

    _configure_if_needed()
    multi = _is_multi_config()

    for config in configs:
        _log(f"━━ بناء التهيئة / building config: {config} "
             f"(المحرّكان معًا / both engines together) ━━")
        # (AR) جوهر المرحلة 0: أمر بناء واحد يضمّ الهدفين. إن فشل أحدهما، فشل
        #      الأمر كلّه — فلا يخرج زوجٌ نصفُ مبنيّ بتهيئتين مختلفتين أبدًا.
        # (EN) Phase-0 core: one build command spanning BOTH targets. If either
        #      fails the whole command fails — never a half-built mismatched pair.
        build_cmd = ["cmake", "--build", str(BUILD_DIR)]
        if multi:
            build_cmd += ["--config", config]
        for target in ENGINES:
            build_cmd += ["--target", target]
        _run(build_cmd)

        manifest = _stage(config)
        _log(f"✓ {config}: ثُبّت المحرّكان في / staged engines to dist/{config}/")
        for name, info in manifest["binaries"].items():
            _log(f"    {name:<10} sha256={info['sha256'][:16]}…  ({info['size']} bytes)")

    _log("✓ اكتمل البناء الذرّيّ / atomic build complete.")


def cmd_verify(args: argparse.Namespace) -> None:
    """(AR) حارس البوّابة G1: يؤكّد أن dist يحوي زوجًا متطابق التهيئة وأن
        البصمات تطابق الملفات الفعلية (كشف الثنائيّ البائت).
    (EN) The G1 gate guard: assert dist holds a same-config pair and that
        fingerprints match the actual files (stale-binary detection)."""
    if args.all:
        if not DIST_DIR.exists():
            _fail("لا يوجد dist/ — شغّل build أولًا / no dist/, run build first.")
        configs = sorted(p.name for p in DIST_DIR.iterdir() if p.is_dir())
        if not configs:
            _fail("dist/ فارغ / dist/ is empty.")
    else:
        configs = _parse_configs(args.config)

    for config in configs:
        manifest_path = DIST_DIR / config / "manifest.json"
        if not manifest_path.exists():
            _fail(f"لا manifest للتهيئة / no manifest for config: {config} "
                  f"— شغّل: python x.py build --config {config}")
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

        if manifest.get("config") != config:
            _fail(f"تضارب تهيئة في manifest / manifest config mismatch: "
                  f"المجلد {config} يحوي تهيئة {manifest.get('config')}")

        missing = [n for n in ENGINES if n not in manifest.get("binaries", {})]
        if missing:
            _fail(f"محرّكات ناقصة في / engines missing in {config}: {', '.join(missing)}")

        for name, info in manifest["binaries"].items():
            out = DIST_DIR / config / info["file"]
            if not out.exists():
                _fail(f"ثنائيّ مفقود / missing binary: {out}")
            actual = _sha256(out)
            if actual != info["sha256"]:
                _fail(f"بصمة بائتة / stale fingerprint: {name} ({config}) — "
                      f"الملف على القرص لا يطابق manifest. أعد البناء.")
        _log(f"✓ {config}: زوج متطابق التهيئة، بصمات سليمة / consistent pair, fingerprints OK.")

    _log("✓ التحقّق نجح / verification passed.")


def cmd_test(args: argparse.Namespace) -> None:
    configs = _parse_configs(args.config)
    if len(configs) != 1:
        _fail("test يقبل تهيئة واحدة فقط / test accepts exactly one config.")
    config = configs[0]

    # (AR) ابنِ المحرّكين معًا ثم تحقّق قبل تشغيل الاختبارات.
    # (EN) Build both engines together, then verify, before running tests.
    build_args = argparse.Namespace(config=config, clean=False)
    cmd_build(build_args)
    cmd_verify(argparse.Namespace(config=config, all=False))

    runner = ROOT / "tests" / "runner.py"
    if not runner.exists():
        _fail(f"runner غير موجود / runner not found: {runner}")
    cmd = [sys.executable, str(runner), *args.runner_args]
    _log("تشغيل الـrunner / running test runner …")
    result = subprocess.run(cmd, cwd=ROOT)
    sys.exit(result.returncode)


def cmd_conformance(args: argparse.Namespace) -> None:
    """(AR) تمرير رفيع إلى فاحص مطابقة القواعد القائم (يصبح أعمق في مرحلة لاحقة).
    (EN) Thin pass-through to the existing grammar-conformance checker."""
    checker = ROOT / "scripts" / "codegen" / "check_grammar_conformance.py"
    if not checker.exists():
        _fail(f"الفاحص غير موجود / checker not found: {checker}")
    cmd = [sys.executable, str(checker), *args.checker_args]
    result = subprocess.run(cmd, cwd=ROOT)
    sys.exit(result.returncode)


def _run_generator(domain: dict, out_dir: Path) -> None:
    """(AR) يستدعي مولّد النطاق كاتبًا مخرجاته إلى out_dir.
    (EN) Invoke a domain's generator, writing its outputs into out_dir."""
    out_dir.mkdir(parents=True, exist_ok=True)
    script = ROOT / "scripts" / "codegen" / domain["script"]
    if not script.exists():
        _fail(f"مولِّد مفقود / generator missing: {script}")
    cmd = [sys.executable, str(script), *domain["args"](out_dir.as_posix())]
    env = os.environ.copy()
    env["PYTHONIOENCODING"] = "utf-8"
    _log("» " + " ".join(cmd))
    result = subprocess.run(cmd, cwd=ROOT, env=env)
    if result.returncode != 0:
        _fail(f"فشل توليد النطاق / codegen failed for domain: {domain['name']}")


def _gen_check() -> None:
    """(AR) حارس الانجراف (G-codegen): يعيد التوليد إلى مجلّد مؤقّت ويقارنه
        بالملفّات المتعقَّبة دون لمس الشجرة. يفشل عند أيّ انحراف أو ملفّ مفقود.
    (EN) Drift guard: regenerate into a temp dir and compare against the tracked
        files WITHOUT mutating the tree. Fails on any drift or missing file."""
    drift: list[tuple[str, str]] = []
    with tempfile.TemporaryDirectory(prefix="sad_gen_check_") as tmp:
        tmp_root = Path(tmp)
        for domain in CODEGEN_DOMAINS:
            tmp_dir = tmp_root / domain["name"]
            _run_generator(domain, tmp_dir)
            for fname in domain["outputs"]:
                rel = f"{domain['out_dir']}/{fname}"
                fresh = tmp_dir / fname
                tracked = ROOT / domain["out_dir"] / fname
                if not fresh.exists():
                    _fail(f"المولِّد لم يُنتج / generator did not produce: {rel}")
                if not tracked.exists():
                    drift.append((rel, "مفقود في المستودع / missing in repo"))
                elif _norm(tracked.read_bytes()) != _norm(fresh.read_bytes()):
                    drift.append((rel, "منحرف عن YAML / drifted from YAML"))

    if drift:
        print("", file=sys.stderr)
        print("❌ المصدر المولَّد منحرف عن language-truth/ (مصدر الحقيقة):",
              file=sys.stderr)
        print("❌ Generated sources drifted from language-truth/ (source of truth):",
              file=sys.stderr)
        for rel, why in drift:
            print(f"     • {rel}  — {why}", file=sys.stderr)
        print("", file=sys.stderr)
        print("   الإصلاح / fix:  python x.py gen   ثم التزِم الناتج / then commit the result.",
              file=sys.stderr)
        _fail("انحراف المصدر المولَّد / generated-source drift detected.")

    _log("✓ المصدر المولَّد متزامن تمامًا مع language-truth/ "
         "/ generated sources are in sync with the YAML SoT.")

    # (AR) حرّاس الفحص الخالصون (لا مخرجات): اتّساق مصدر الحقيقة مع بقيّة الشجرة.
    # (EN) Pure check-only guards: SoT-vs-tree consistency (no generated outputs).
    for guard in SOT_CHECK_GUARDS:
        script = ROOT / "scripts" / "codegen" / guard["script"]
        if not script.exists():
            _fail(f"حارس مفقود / guard missing: {script}")
        cmd = [sys.executable, str(script), *guard["args"]]
        env = os.environ.copy()
        env["PYTHONIOENCODING"] = "utf-8"
        _log("» " + " ".join(cmd))
        result = subprocess.run(cmd, cwd=ROOT, env=env)
        if result.returncode != 0:
            _fail(f"فشل حارس الاتّساق / consistency guard failed: {guard['name']}")

    _log("✓ حرّاس الاتّساق كلّها خضراء / all SoT consistency guards passed.")


def cmd_gen(args: argparse.Namespace) -> None:
    """(AR) يولّد المصدر من language-truth/ (YAML مصدر الحقيقة). مع --check
        يتحوّل إلى حارس انجراف لا يكتب شيئًا (لبوّابة CI).
    (EN) Regenerate sources from the language-truth/ YAML SoT. With --check it
        becomes a non-mutating drift guard (for the CI gate)."""
    if args.check:
        _gen_check()
        return
    for domain in CODEGEN_DOMAINS:
        _run_generator(domain, ROOT / domain["out_dir"])
        for fname in domain["outputs"]:
            _log(f"    ✓ {domain['out_dir']}/{fname}")
    _log("✓ اكتمل التوليد من language-truth/ / codegen from the YAML SoT complete.")


def cmd_clean(args: argparse.Namespace) -> None:
    for d in (BUILD_DIR, DIST_DIR):
        if d.exists():
            _log(f"حذف / removing {d.name}/ …")
            shutil.rmtree(d, ignore_errors=True)
    _log("✓ تم التنظيف / cleaned.")


# ──────────────────────────────────────────────────────────────────────
# واجهة الأوامر / CLI
# ──────────────────────────────────────────────────────────────────────
def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="x.py",
        description="منسّق البناء الموحَّد للغة ص / Sad Lang unified build orchestrator",
    )
    sub = p.add_subparsers(dest="command", required=True)

    pb = sub.add_parser("build", help="بناء المحرّكين معًا لكل تهيئة / build both engines per config")
    pb.add_argument("--config", default="Debug",
                    help="قائمة تهيئات مفصولة بفواصل / comma-separated configs (Debug,Release)")
    pb.add_argument("--clean", action="store_true", help="حذف build/ قبل البناء / wipe build/ first")
    pb.set_defaults(func=cmd_build)

    pv = sub.add_parser("verify", help="تحقّق من تطابق dist والبصمات / verify dist pair & fingerprints")
    pv.add_argument("--config", default="Debug", help="تهيئات للتحقّق / configs to verify")
    pv.add_argument("--all", action="store_true", help="تحقّق من كل تهيئات dist/ / verify all dist configs")
    pv.set_defaults(func=cmd_verify)

    pt = sub.add_parser("test", help="بناء + تحقّق + تشغيل runner / build + verify + run runner")
    pt.add_argument("--config", default="Debug", help="تهيئة واحدة / single config")
    pt.add_argument("runner_args", nargs=argparse.REMAINDER,
                    help="وسائط تُمرَّر للـrunner بعد -- / args passed to runner after --")
    pt.set_defaults(func=cmd_test)

    pg = sub.add_parser("gen", help="توليد المصدر من language-truth/ (YAML مصدر الحقيقة) "
                                    "/ regenerate sources from the YAML SoT")
    pg.add_argument("--check", action="store_true",
                    help="حارس انجراف: يفشل إن انحرف المولَّد، دون كتابة (لبوّابة CI) "
                         "/ drift guard: fail on drift without writing (CI gate)")
    pg.set_defaults(func=cmd_gen)

    pc = sub.add_parser("configure", help="تهيئة CMake / configure CMake")
    pc.set_defaults(func=cmd_configure)

    pf = sub.add_parser("conformance", help="فاحص مطابقة القواعد / grammar-conformance checker")
    pf.add_argument("checker_args", nargs=argparse.REMAINDER,
                    help="وسائط تُمرَّر للفاحص بعد -- / args passed to the checker after --")
    pf.set_defaults(func=cmd_conformance)

    pcl = sub.add_parser("clean", help="حذف build/ و dist/ / remove build & dist")
    pcl.set_defaults(func=cmd_clean)

    return p


def main() -> None:
    args = build_parser().parse_args()
    # (AR) إزالة `--` الفاصل إن مرّره argparse ضمن REMAINDER.
    # (EN) Strip a leading `--` separator left by argparse in REMAINDER.
    for attr in ("runner_args", "checker_args"):
        vals = getattr(args, attr, None)
        if vals and vals and vals[0] == "--":
            setattr(args, attr, vals[1:])
    args.func(args)


if __name__ == "__main__":
    main()
