#!/usr/bin/env python3
# ======================================================================
# x.py — منسّق البناء الموحَّد للغة ص / Unified build orchestrator
# ======================================================================
# (AR) ينفّذ المرحلة 0 من RFC «قلب موحَّد بحدود داخلية صارمة» (sadlang-rfcs#10):
#      البوّابة الوحيدة للبناء. يبني خلفيّتَي المترجم — `sad-build` (بـLLVM)
#      و`sad-build-native` (السياديّةُ بلا LLVM) — **ذرّيًّا في نفس التهيئة**، ثم
#      يثبّت الثنائيَّين في `dist/<Config>/` مع بصمة SHA-256 لكلٍّ منهما. هذا يقتل
#      عثرتين موثَّقتين:
#        1. تذبذب Debug/Release (خلفيّةٌ بتهيئة وأخرى بغيرها) — مستحيل الآن لأن
#           الأمر الواحد يبني الهدفين معًا لكل تهيئة، وإن فشل أحدهما فشل الكل.
#        2. الثنائيّ البائت — `verify` يعيد حساب البصمة فيكشف أيّ مخرَج قديم.
#
#      🔑 مسارُ المحرّكِ الواحد: كان الهدفانِ هنا «المفسّر + المترجم»، وكان
#      الحارسُ يمنعُ تباعدَ محرّكَي التنفيذ. بحذفِ المفسّر لم يمُتِ الحارسُ بل
#      **انتقلَ محورُه** إلى المحورِ الباقي: خلفيّةُ LLVM مقابلَ الخلفيّةِ
#      السياديّة. حارسٌ بهدفٍ واحدٍ حارسٌ بلا معنى — فلا تُنقِص القائمةَ إلى واحد.
#
# (EN) Implements Phase 0 of the RFC "unified core with strict internal
#      boundaries" (sadlang-rfcs#10): the single build gateway. Builds BOTH
#      compiler backends — `sad-build` (LLVM) and `sad-build-native` (sovereign,
#      no LLVM) — ATOMICALLY in the SAME configuration, then stages both binaries
#      into `dist/<Config>/` with a SHA-256 fingerprint each. Kills two documented
#      gotchas: Debug/Release drift and the stale-binary trap.
#
#      🔑 Single-engine track: this pair used to be interpreter+compiler and the
#      guard prevented execution-engine drift. Deleting the interpreter did not
#      kill the guard — it MOVED its axis to the remaining one: LLVM backend vs
#      sovereign backend. A one-entry guard guards nothing.
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
#      الاسمُ `ENGINES` باقٍ لأنّ الدورَ باقٍ: هذانِ هما مُنتِجا الشفرة اللذانِ
#      يجبُ ألّا يتباعدا. أُزيلَ `sad-run` مع المفسّر في مسارِ المحرّكِ الواحد.
# (EN) The two targets that must ALWAYS build together — the heart of the guard.
ENGINES = {
    "sad-build": "المترجم بـLLVM / compiler (LLVM)",
    "sad-build-native": "المترجم السياديّ بلا LLVM / sovereign compiler (no LLVM)",
}

_WINDOWS = platform.system() == "Windows"
EXE = ".exe" if _WINDOWS else ""
VALID_CONFIGS = ("Debug", "Release", "RelWithDebInfo", "MinSizeRel")

# (AR) أنماط اسم المكتبة الساكنة — نجرّبها كلَّها لأنّ اللاحقة تحدّدها **سلسلة
#      الأدوات** لا نظام التشغيل: MinGW/Clang-GNU على ويندوز يُنتج
#      `libfoo.a`، وMSVC يُنتج `foo.lib`. تثبيتُ نمطٍ واحد يعني تخطّيًا صامتًا
#      لكلّ المكتبات على سلسلةٍ مغايرة ودِست يبدو سليمًا. الأنماط هنا هي نفسها
#      التي يقبلها سائق المترجم
#      (compiler_driver_build_utils.cpp::has_library_file_in_dir) فلا تتباعد
#      الطبقتان في ما تَعُدّه «مكتبةً موجودة».
# (EN) Static-library name patterns: the suffix is decided by the TOOLCHAIN, not
#      the OS. Mirrors has_library_file_in_dir on the driver side so both layers
#      agree on what counts as a present library.
LIB_PATTERNS = ("{name}.lib", "lib{name}.a", "{name}.a")
# (AR) كلُّ لاحقةٍ نعدّها مكتبةً حين ننظّف dist من البائت (انظر _stage).
LIB_SUFFIXES = (".lib", ".a")

# (AR) مكتبات وقت التشغيل التي **يربطها المترجَم في برامج المستخدم** — لا
#      يستعملها المحرّكان أنفسُهما. سائق المترجم يبحث عنها بجوار ثنائيّه
#      (`get_executable_dir()` وما فوقه؛ انظر
#      tools/compiler/compiler_driver_build_utils.cpp::append_bundled_network_libraries)،
#      فإن لم تُنسَخ إلى dist/ لم يجدها الثنائيّ المُثبَّت: تُترجَم برامج
#      الواجهة والشبكة ثمّ **يفشل ربطها** برموزٍ غير معرَّفة (sad_text،
#      sad_generate_web…).
#      حدُّ الادّعاء صراحةً: هذا يجعل dist/ مكتفيًا **داخل شجرة المستودع**
#      لا على جهازٍ آخر — فـsad_graphics تجرّ SDL2/SDL2_ttf المُورَّدتين
#      ولا تُنضَّدان هنا؛ الربطُ ينجح لأنّ سائق المترجم يصعد آباءَ ثنائيّه
#      فيجد features/graphics/third_party. حزمةٌ منقولةٌ تحتاج شحنَ الموردات
#      بجوار الثنائيّ (دَينٌ موثَّق).
#      اختياريّة كلُّها: ما لم يُبنَ في هذه التهيئة يُتخطّى بلا فشل (الوضع
#      الحرّ مثلًا هدفٌ منفصل لا تبنيه بوّابة المحرّكين) — والمتخطَّى يُسجَّل
#      في مخرَج البناء كي لا يكون الغيابُ صامتًا.
# (EN) Runtime libraries the COMPILER links into user programs (not used by the
#      engines themselves). The driver looks for them next to its own binary, so
#      a dist/ without them compiles UI/network programs that then fail to link.
#      All optional: whatever this config did not build is skipped.
RUNTIME_LIBRARIES = (
    "sad_graphics_runtime",
    "sad_graphics",
    "sad_graphics_runtime_freestanding",
    "sad_graphics_freestanding",
    "sad_http",
    "sad_network",
    "sad_websocket",
)

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
        # (AR) 🔑 نطاقاتُ المعاملات: كان الجدولُ مكتوبًا باليدِ في
        #      `TypeChecker::visitBinaryExpr` ومصدرُ الحقيقةِ لا يعرفُ منه شيئًا —
        #      لا حقلَ لنطاقِ المعاملاتِ في مخطَّطِ العواملِ إطلاقًا. وأخطرُ من ذلك
        #      أنّ `operator.schema.json` **لم يكن مُصادَقًا في أيِّ بوّابة**:
        #      قائمةُ الأزواجِ في `language-truth/tests/test_schema_validation.py`
        #      فارغةٌ منذ M0، فانجرفت البياناتُ عن مخطَّطِها حتّى صارَ ٤٣ مدخلًا
        #      من ٤٣ مخالِفًا. فوصلُ المصادقةِ داخلَ المولِّدِ يجعلُ الانجرافَ
        #      يحمرُّ في هذه البوّابةِ القائمةِ لا في اختبارٍ لا يُشغَّلُ أصلًا.
        # (EN) Operand domains. The table was hand-written C++ and the SoT had no
        #      operand field at all; worse, operator.schema.json had NO validator
        #      anywhere, so the data drifted to 43/43 violations. Validating inside
        #      the generator makes drift redden in this live gate.
        "name": "operator_domains",
        "script": "gen_operator_domains.py",
        "out_dir": "shared/semantic/generated",
        "outputs": ("operator_domains_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/operators.yaml",
            "--schema", "language-truth/_schemas/operator.schema.json",
            "--types", "language-truth/types.yaml",
            "--header", f"{d}/operator_domains_generated.h",
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
        # (AR) رتبُ المدمجاتِ التي يفرضُها المترجم: كانت أرقامًا حرفيّةً في أذرعِ
        #      الأماميّة ومصدرُ الحقيقةِ لا يعرفُها — عقدٌ لا يقيسُه أحد. صارت
        #      تُعلَن بحقل `arity` ويُولَّد منها الجدول، فالرقمُ نسخةٌ واحدة.
        "name": "builtins_arity",
        "script": "gen_builtins_arity.py",
        "out_dir": "shared/builtins/generated",
        "outputs": ("builtin_arity_generated.h",),
        "args": lambda d: [
            "--yaml-dir", "language-truth/builtins",
            "--out-h", f"{d}/builtin_arity_generated.h",
            "--quiet",
        ],
    },
    {
        # (AR) أوپكودات SIR المقيَّدة بعائلة معالج (تبثّ `cli`/`outb`/`mov %crN`…).
        #      كان المترجم يبثّها لأيّ هدفٍ بخروجٍ صفريّ، فيخفق المُجمِّعُ برسالةٍ
        #      لا تدلّ — أو يخرج ثنائيٌّ لا يعمل. الجدول يجعل القيد مُعلَنًا.
        "name": "arch_specific_opcodes",
        "script": "gen_arch_specific_opcodes.py",
        "out_dir": "compiler/include/backend/generated",
        "outputs": ("arch_specific_opcodes_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/backend/arch_specific_opcodes.yaml",
            "--opcodes", "language-truth/backend/sir_opcodes.yaml",
            "--out-h", f"{d}/arch_specific_opcodes_generated.h",
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
        # (AR) مفرداتُ لغةِ ص كما يراها المُلوِّنُ النحويُّ في «كتلة_كود».
        #      كانت جداولَ مكتوبةً بيدٍ في مكتبة الرسومات فانحرفت عن المعجم
        #      بـ٣١ كلمةً محجوزةً مقيسة (تلوينٌ ناقصٌ صامت). رأسٌ ذاتيُّ الاكتفاء
        #      عمدًا: المكتبةُ طرفيّةٌ فلا تعتمد طبقةَ المحلّل — نظيرُ prop_keys.h.
        "name": "ui_syntax_keywords",
        "script": "gen_ui_syntax_keywords.py",
        "out_dir": "features/graphics/core/include/sad_ui/generated",
        "outputs": ("syntax_keywords_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/keywords.yaml",
            # (AR) مدخلٌ ثانٍ: الدوالُّ المدمجةُ العامّة. يُمرَّر صراحةً لا اتّكالًا
            #      على الافتراضيِّ المشتقِّ من موضعِ السكربت — فالوصلُ هنا مرئيّ.
            "--builtins", "language-truth/builtins/core.yaml",
            "--header", f"{d}/syntax_keywords_generated.h",
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
        # (AR) أنواع عُقَد واجهة SadUI (نص_عنصر/زر/عمود/نافذة/…) — مصدر الحقيقة الوحيد.
        #      X-macro مولَّد يبني تعدادَ UINodeType نفسَه وجدولَي الاسم في المكتبة
        #      الطرفيّة، فيراه **المحرّكان معًا** عبرها (لا تعدادَ ثانٍ في أيٍّ منهما).
        "name": "ui_nodes",
        "script": "gen_ui_nodes.py",
        "out_dir": "features/graphics/core/include/sad_ui/generated",
        "outputs": ("node_types_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/ui_nodes.yaml",
            "--widgets", "language-truth/builtins/ui_widgets.yaml",
            "--schema", "language-truth/_schemas/ui_nodes.schema.json",
            "--header", f"{d}/node_types_generated.h",
            "--only", "graphics",
            "--quiet",
        ],
    },
    {
        # (AR) ما يعرفه المسارُ التصريحيُّ في المحلّل من أسماء (أوّليّات/حاويات/
        #      أحداث). المحلّل في الطبقة الأساس فلا يضمّ رأسًا من مكتبة الرسومات:
        #      المصدرُ واحدٌ والمخرَجان اثنان، فلا اسمَ عربيٌّ حرفيٌّ في أيِّ طرف.
        "name": "ui_parser_nodes",
        "script": "gen_ui_nodes.py",
        "out_dir": "shared/parser/include/generated",
        "outputs": ("ui_parser_nodes_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/ui_nodes.yaml",
            "--widgets", "language-truth/builtins/ui_widgets.yaml",
            "--schema", "language-truth/_schemas/ui_nodes.schema.json",
            "--events", "language-truth/ui_events.yaml",
            "--parser-header", f"{d}/ui_parser_nodes_generated.h",
            "--only", "parser",
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
    # (AR) 🔑 نُزع نطاق `tool_repl`: أداةُ `sad-repl` حُذفت مع المفسّر،
    #      فبقاءُ النطاق يجعل `x.py gen --check` يُحمِّر على مخرَجٍ لا
    #      مستهلكَ له — بوّابةٌ تفشلُ لغيابِ ما لا يُراد.
    # (EN) Removed the tool_repl codegen scope: sad-repl went with the
    #      interpreter, so the scope only reddened the gate.
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
            # (AR) معجم لكلّ معماريّة — والمولّد يرفض أيّ خلاف بين هذه القائمة
            #      وحقل architectures في assembly.yaml، فلا تُضاف معماريّة في
            #      موضع وتُنسى في الآخر.
            "--mnemonics",
            "language-truth/dialects/assembly_mnemonics/aarch64.yaml",
            "language-truth/dialects/assembly_mnemonics/i686.yaml",
            "language-truth/dialects/assembly_mnemonics/riscv64.yaml",
            "language-truth/dialects/assembly_mnemonics/x86_64.yaml",
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
    {
        # (AR) تمثيلُ القيمة زمنَ التشغيل: وسومُ SadDyn (DynKind) + نصوصُ عرضِ القيم
        #      (لاشيء/صحيح/خطأ) — كانت مكتوبةً يدويًّا في المفسّر وLLVM والخلفيّة الأصليّة
        #      فانجرفت (عرضُ العدم «عدم» أصليًّا مقابل «لاشيء» في المفسّر/LLVM). تُوحَّد هنا
        #      مصدرًا واحدًا يستهلكه الثلاثةُ، ويحرس `gen --check` انجرافَها.
        # (EN) Runtime value representation: SadDyn tags + value display texts,
        #      single-sourced (were hand-copied across interpreter/LLVM/native and drifted).
        "name": "backend_value_repr",
        "script": "gen_value_repr.py",
        "out_dir": "shared/types/generated",
        "outputs": ("value_repr_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/backend/value_repr.yaml",
            "--header", f"{d}/value_repr_generated.h",
            "--quiet",
        ],
    },
    {
        # (AR) أسماءُ التوجيهاتِ (@): يحتاجُها التشخيصُ ليُسمّيَ التوجيهَ الذي رُفِض
        #      («لا يمكن تطبيق 'حجم النوع' على 'صنف'»). وكتابتُها سلسلةً خامّةً في
        #      المترجّمِ وأخرى في المفسّرِ هي العلّةُ نفسُها التي وُلد `size_bytes`
        #      لسدِّها: حقيقةٌ واحدةٌ في مواضعَ كثيرةٍ تنجرف.
        # (EN) Directive (@) names, needed by diagnostics that name a directive.
        "name": "directive_names",
        "script": "gen_directives.py",
        "out_dir": "shared/types/generated",
        "outputs": ("directive_names_generated.h",),
        "args": lambda d: [
            "--yaml", "language-truth/directives.yaml",
            "--header", f"{d}/directive_names_generated.h",
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
        # (AR) ربطُ القواعد بأوپكودات SIR ودعمُ الخلفيّة الأصليّة (بلا LLVM):
        #      يستدعي gen_sir_opcodes_yaml.py --check فيمسك كتالوجًا بائتًا عن تعداد
        #      المترجم، ثمّ يتحقّق أنّ لكلّ قاعدةٍ نحويّةٍ مدخلًا في lowers_to.yaml،
        #      وأنّ المعماريّات المخفوضة فعلًا تطابق backend/targets.yaml (لا هدفَ
        #      يدّعي lowered بلا خفض، ولا معماريّةَ تُخفَّض خارج القائمة).
        #      رخيصٌ: لا يعيد ترجمةَ الاختبارات — الاشتقاقُ الكامل يدويٌّ بـ--recompute.
        # (EN) Grammar↔SIR mapping + native (LLVM-free) backend support: catches a
        #      stale opcode catalog, rules missing a lowers_to entry, and drift
        #      between actually-lowered ISAs and backend/targets.yaml. Cheap: no rebuild.
        "name": "grammar_lowers_to",
        "script": "check_lowers_to.py",
        "args": (),
    },
    {
        # (AR) اتّساق خصائص الواجهة: primary_prop لكلّ عنصر ⇐ مفتاح في ui_props.yaml.
        # (EN) UI-property consistency: each widget primary_prop maps to a ui_props key.
        "name": "ui_props_consistency",
        "script": "check_ui_props_consistency.py",
        "args": (),
    },
    {
        # (AR) اكتمال التصيير: كلّ عقدة UINodeType لها حالة في كلّ هدف تصيير،
        #      أو استثناءٌ مُعلَنٌ بسببٍ مكتوب في ui_nodes.yaml (ث٤+ث٨).
        # (EN) Render completeness: every UINodeType has a case in every render
        #      target, or a declared+justified exemption in ui_nodes.yaml.
        "name": "ui_render_completeness",
        "script": "check_ui_render_completeness.py",
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
        # (AR) الاسم القانونيّ للمدمجة قابلٌ للنداء: المُشكِّل يجرّد التشكيلَ من
        #      المعرّفات بينما يفهرس السجلُّ بالاسم حرفيًّا، فاسمٌ يحملُ شدّةً لا
        #      يُنادى أبدًا (قِيس: ثلاثُ مدمجات APIC معلَنة stable كانت ميتة).
        # (EN) A builtin's canonical name must be callable: the lexer strips
        #      harakat from identifiers while the registry keys the literal name.
        "name": "builtins_canonical_callable",
        "script": "check_builtins_canonical_callable.py",
        "args": (),
    },
    {
        # (AR) رتبةُ المدمجِ تُعلَن مرّةً واحدةً في مصدرِ الحقيقة: لا رقمَ حرفيًّا في
        #      ذراعِ الأماميّة (نسخةٌ ثانيةٌ للحقيقة تنجرفُ بلا احمرار)، ولا ثابتَ
        #      مدمجٍ آخر، ولا إعلانًا لا تفرضُه ذراع، ولا فرضًا غيرَ مُعلَن.
        # (EN) Builtin arity is declared once in SoT and measured from both ends.
        "name": "builtin_arity_sot",
        "script": "check_builtin_arity_sot.py",
        "args": (),
    },
    {
        # (AR) الوعدُ المُعلَنُ يُقاسُ ولا يُدَّعى. مدمَجٌ يحملُ `status: stable`
        #      دعوى أنّه يعمل، فيجبُ أن يوجدَ له تنفيذٌ في الشجرة — **صفرٌ لا سقف**.
        #      ومَن أُجِّلَ يحملُ `intent: مؤجَّل`، وله سقفٌ نازلٌ. وبندٌ بلا حقلٍ،
        #      أو بالحقلَين، **أو بقيمةٍ خارجَ التعدادِ المُغلَق** يُحمِّر — وإلّا
        #      صارَ كلُّ ذلك بابَ هروبٍ من الاثنَين.
        #      🔑 والعددُ لا يُنثَرُ ههنا: نسختانِ باليدِ لحقيقةٍ واحدةٍ تتباعدان.
        #      السقفُ في `CEILING_DEFERRED`، وعمقُ العيارِ في `MIN_PROBES`.
        #      🔑 وحلَّ هذا محلَّ `CEILING_COMPILER_MISSING = 564`: كان يسألُ «هل
        #      يُرسِلُه المترجّم؟» بمسبارٍ يمسحُ التعليقاتِ ويطابقُ أيَّ لفظٍ كبير،
        #      فيُخفَّضُ العددُ بسطرِ تعليقٍ واحد (قِيس: ٥٦٤ ← ٥٥٩). والأداةُ اليومَ
        #      تُعايِرُ نفسَها في كلِّ تشغيل، وتُنهي بالرمز 2 إن عمِيت — ومجسُّها
        #      **سالبٌ** أيضًا: بندٌ مؤجَّلٌ يجبُ ألّا يراهُ المسبار، وإلّا لاجتازَ
        #      العيارَ حارسٌ مُفرَغٌ يزعمُ أنّ كلَّ مُعلَنٍ مُرسَل.
        # (EN) A declared builtin either claims to work (`status: stable` — must
        #      have an implementation, zero tolerance) or declares debt
        #      (`intent: مؤجَّل` — descending ceiling). Replaces a ceiling whose
        #      probe scanned comments and could be lowered by one comment line.
        "name": "builtin_engine_coverage",
        "script": "check_builtin_engine_coverage.py",
        "args": (),
    },
    {
        # (AR) لا نصَّ تشخيصٍ مكتوبًا يدويًّا في المحلّل النحويّ — كلّ رسالة تُركَّب
        #      من كتالوج الأخطاء المولَّد. حارسٌ بنيويّ (لا نصّيّ) بخطّ أساسٍ ينحدر.
        # (EN) No hand-written diagnostic text in the parser — every message renders
        #      from the generated error catalog. Structural guard, shrinking baseline.
        "name": "no_raw_parser_text",
        "script": "check_no_raw_parser_text.py",
        "args": (),
    },
    # (AR) 🔑 نُزع حارس `applet_coverage`: موضوعُه معجمُ أصيلاتِ صدفةِ
    #      `sad-repl` — والأداةُ ومصدرُ حقيقتِها زالا مع المفسّر. حارسٌ بلا
    #      موضوعٍ إمّا ينهار وإمّا يخضرُّ على الفراغ، وكلاهما كذب.
    # (EN) Removed: its subject (the sad-repl applet lexicon and its SoT)
    #      went with the interpreter. A subjectless guard either crashes or
    #      goes green on emptiness — both are lies.
    {
        # (AR) لا خريطةَ «مدمَجة ⇒ وحدة» يدويّةً في محرّك: كانت لكلٍّ خريطتُه فتباعدا
        #      عن مصدر الحقيقة وعن بعضهما (جذر رياضيّةٌ تطلب «نصوص» في المصرّف،
        #      وحرّةٌ في المفسّر) ⇒ ثلاثون مدمَجةً تعمل مفسَّرةً ولا تُصرَّف.
        # (EN) No hand-written builtin⇒module map in an engine: each had its own,
        #      drifting from the SoT and from each other ⇒ 30 builtins ran in the
        #      interpreter and would not compile at all.
        "name": "no_handwritten_import_gate",
        "script": "check_no_handwritten_import_gate.py",
        "args": (),
    },
    {
        # (AR) كلُّ `terminal.ref` في قواعد النحو له وجودٌ في كتالوج الرموز: كانت
        #      ٢٥ إحالةً شبحيّةً (`OP_COMMA` ٢٢ مرّة، `EOF`، `KEYWORD_ITERATIONS`،
        #      `KEYWORD_SEED`) تعبر خضراءَ لأنّ أحدًا لم يكن يقابلها بالكتالوج.
        # (EN) Every grammar `terminal.ref` must exist in the token catalog: 25
        #      ghost refs used to pass green because nothing cross-checked them.
        "name": "grammar_terminal_refs",
        "script": "check_grammar_terminal_refs.py",
        "args": (),
    },
    {
        # (AR) ألفاظُ الأنواع في المحلّل مشتقّةٌ من types.yaml: كانت نسخةً مُصلَّبةً
        #      فانجرفت وبقيت فيها «مضاعف» بعد إزالتها من اللغة. يقارن الحارسُ
        #      جدولَي «اللفظ ⇒ النوع» في المحلّل بمصدر الحقيقة، ويشترط أن يكون كلّ
        #      لفظٍ سطحيًّا أو مُعلَنًا في `removed_type_words`.
        # (EN) Parser type words must come from types.yaml: the hardcoded copy had
        #      drifted, still listing «مضاعف» after the language removed it.
        "name": "type_words_sot",
        "script": "check_type_words_sot.py",
        "args": (),
    },
    {
        # (AR) `@حجم(نوع)` يُشتَقُّ من `types.yaml` (حقل `size_bytes`) ولا يُكتَبُ في
        #      المحرّكَين. كان جدولَين مكتوبَين باليدِ تباعدا — قياسًا حيًّا — في
        #      ستّةٍ من أحدَ عشرَ نوعًا (خريطة ٤٨/٢٤ · بايت ٠/٨ · حرف ٠/١ ·
        #      أي ٧٢/٨ · فراغ ٠/٨ · عدم ٠/٨)، وافتراضاهما يكذبان: نوعٌ مجهولٌ
        #      يُخرِجُ ٨ في المترجّمِ و٠ في المفسّرِ بلا تشخيص. والحارسُ يقيسُ
        #      جسمَ الدالّةِ وحدَه (بموازنةِ الأقواس) لا الملفَّ، فلا يحمرُّ كذبًا
        #      على تشخيصاتٍ عربيّةٍ مشروعةٍ في الملفِّ نفسِه.
        # (EN) @حجم must derive from types.yaml's size_bytes. It used to be two
        #      hand-written tables disagreeing on 6 of 11 measured types with two
        #      lying defaults. The guard measures the function body only.
        "name": "sizeof_from_sot",
        "script": "check_sizeof_from_sot.py",
        "args": (),
    },
    {
        # (AR) تقريرا المطابقةِ المُودَعان يشهدان لملفّاتٍ موجودة: شهد التقريرُ
        #      لـ`gr.oop.new` بثلاثةَ عشرَ اختبارًا وسرَدها بأسمائها، وأحدَ عشرَ منها
        #      لم تدخل المستودعَ قطّ (ابتلعها .gitignore) — فالاستنساخُ النظيفُ لا
        #      يُعيد إنتاجَ الشهادة. ولم يكن المولِّدُ في `gen --check` أصلًا، فلا حدَّ
        #      للانجرافِ في الزمن. الحارسُ يقابل الصفوفَ بالقرصِ بلا تشغيلِ اختبارات.
        # (EN) The committed conformance reports must attest to files that exist:
        #      the report vouched for 13 gr.oop.new tests while 11 were never in the
        #      repo at all. The generator was not part of gen --check, so nothing
        #      bounded the drift. This guard diffs rows against disk, no test runs.
        "name": "conformance_report_fresh",
        "script": "check_conformance_report_fresh.py",
        "args": (),
    },
    {
        # (AR) 🔑 حقلُ `retired` في كتالوجِ الأخطاءِ يلزمُه مستهلك: حقلٌ لا
        #      يقرؤه أحدٌ تعليقٌ بصيغةِ بيانات لا عقد. ويحرسُ اتّجاهَين: لا
        #      باعثَ لرمزٍ مُتقاعَد (وإلّا فالتقاعدُ دعوى كاذبة)، ولا حذفَ
        #      لنصِّه من التعداد (فحذفُ مدخلٍ «ميّت» أسقطَ تشخيصًا من قبل).
        # (EN) The catalog's `retired` field needs a reader, or it is a comment
        #      in data format. Two directions: no emitter for a retired code, and
        #      no deletion of its text from the enum.
        "name": "retired_error_codes",
        "script": "check_retired_error_codes.py",
        "args": (),
    },
    {
        # (AR) الحارسُ الفوقيّ: «عيارٌ قديمٌ لحارسٍ جديدٍ ليس عيارًا». سجلُّ العيارِ
        #      يحملُ بصمةَ حارسِه وقتَ الحقن؛ فإن تغيّرَ الحارسُ ولم يُعَدِ العيارُ
        #      حمِرَ هذا. ومعه سقفٌ **نازلٌ** للحرّاسِ بلا سجلّ (٢٦ من ٢٧) — ولم
        #      يُجعَلْ صفرًا لأنّ الصفرَ اليومَ كذبٌ يُسكَّنُ ولا يُصلَح.
        #      🔑 والعلّةُ التي يمنعُها مُدوَّنةٌ مرارًا في سجلِّ دروسِ هذا المستودع:
        #      حارسٌ اخضرَّ لأنّه لا يستطيعُ أن يحمرّ.
        # (EN) Meta-guard: a stale calibration is not a calibration. Fails on
        #      fingerprint drift, on a record pointing at a missing guard, and when
        #      the count of uncalibrated guards grows past its descending ceiling.
        "name": "calibration_fresh",
        "script": "check_calibration_fresh.py",
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


def _find_library(name: str, config: str) -> Path | None:
    """(AR) يحدّد موقع مكتبة وقت تشغيل بعد البناء (نظير _find_binary).
    (EN) Locate a produced runtime library (mirror of _find_binary)."""
    # (AR) التهيئةُ أولى من النمط: لو صُدِّرت الأنماطُ خارجًا لهزم احتياطُ
    #      أحاديِّ التهيئة (build/lib/foo.lib) المسارَ الصحيحَ لهذه التهيئة
    #      (build/lib/<config>/libfoo.a) فنُنضِّد مكتبةَ تهيئةٍ أخرى.
    # (EN) Config outranks pattern: otherwise the single-config fallback would
    #      beat the correct per-config path and stage another config's library.
    for directory in (BUILD_DIR / "lib" / config,   # multi-config (VS)
                      BUILD_DIR / "lib",            # single-config
                      BUILD_DIR / config / "lib"):
        for pattern in LIB_PATTERNS:
            candidate = directory / pattern.format(name=name)
            if candidate.exists():
                return candidate
    return None


def _find_runtime_library(name: str, config: str) -> Path | None:
    """(AR) نظيرُها لمكتباتِ وقتِ التشغيلِ وحدَها، بتفضيلِ Release.
        السببُ ليس تحسينًا: سائقُ المترجمِ يربطُ CRT الإصدارَ دائمًا، وأرشيفُ
        Debug بـMSVC يستدعي CRT التنقيح (`__imp__CrtDbgReport`) — فنسخُ أرشيفِ
        Debug إلى dist/Debug يُنتِجُ توزيعةً **تُترجِمُ ولا تربط**: كلُّ برنامجِ
        واجهةٍ يفشلُ بـLNK2001. نُنضِّدُ ما يربطه السائقُ فعلًا، ونعودُ إلى تهيئةِ
        الطلبِ حين لا تكونُ Release مبنيّة (ومنه المولِّدُ أحاديُّ التهيئة).
        (ملاحظةٌ صريحة: هذا يجعلُ dist/Debug يحملُ أرشيفَ Release لوقتِ التشغيلِ
        بينما المحرّكانِ فيه Debug — وهو المزيجُ الذي يربطُه السائقُ من شجرةِ
        البناءِ أصلًا، لا خلطًا جديدًا.)
    (EN) Runtime-library counterpart with a Release preference. Not an
        optimization: the driver always links the release CRT, so a Debug MSVC
        archive staged into dist/Debug yields a distribution that compiles but
        cannot link (LNK2001 on `__imp__CrtDbgReport`). Fall back to the
        requested config when Release was not built (single-config generators)."""
    if config != "Release":
        preferred = _find_library(name, "Release")
        if preferred is not None:
            return preferred
    return _find_library(name, config)


# ──────────────────────────────────────────────────────────────────────
# الأوامر / Commands
# ──────────────────────────────────────────────────────────────────────
def cmd_configure(args: argparse.Namespace) -> None:
    """(AR) تهيئة CMake إن لم تكن موجودة. / Configure CMake if needed."""
    _configure_if_needed()


def _configure_if_needed() -> None:
    _request_cmake_api()
    if (BUILD_DIR / "CMakeCache.txt").exists():
        return
    _log("تهيئة CMake لأول مرة / first-time CMake configure …")
    _run(["cmake", "-S", str(ROOT), "-B", str(BUILD_DIR)])


def _request_cmake_api() -> None:
    """(AR) يضع استعلامَ file-API قبلَ أيِّ توليد، فتُنتِج CMake فهرسَ الأهداف
        مجّانًا مع التوليد. الاستعلامُ عديمُ الحالة: ملفٌّ فارغٌ باسمِ النسخة.
    (EN) Drop a stateless CMake file-API query so generation emits the target
        index for free."""
    query = BUILD_DIR / ".cmake" / "api" / "v1" / "query" / "codemodel-v2"
    if query.exists():
        return
    query.parent.mkdir(parents=True, exist_ok=True)
    query.touch()


def _known_targets() -> set[str] | None:
    """(AR) أسماءُ الأهدافِ التي تعرفُها هذه التهيئةُ فعلًا، من ردِّ file-API.
        يعودُ None حين لا يتوفّرُ الردّ — وحينَها **لا نحذفُ شيئًا**: الجهلُ
        بالوجودِ ليس دليلَ غياب.
    (EN) Targets this configuration actually defines, from the file-API reply.
        None when unavailable — and then we drop nothing: not knowing is not
        evidence of absence."""
    reply = BUILD_DIR / ".cmake" / "api" / "v1" / "reply"
    if not reply.is_dir():
        return None
    names: set[str] = set()
    for codemodel in reply.glob("codemodel-v2-*.json"):
        try:
            data = json.loads(codemodel.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError):
            continue
        for configuration in data.get("configurations", []):
            for target in configuration.get("targets", []):
                name = target.get("name")
                if name:
                    names.add(name)
    return names or None


def _buildable_runtime_libraries() -> tuple[list[str], list[str]]:
    """(AR) يفصلُ مكتباتِ وقتِ التشغيلِ إلى «تُبنى الآن» و«غيرُ معرَّفةٍ في هذه
        التهيئة». عقدُ الوحدةِ أعلاه: كلُّها اختياريّةٌ يُتخطّى غيرُ المبنيِّ بلا
        فشل — فتمريرُ هدفٍ لا وجودَ له إلى `cmake --build --target` يُفشِلُ
        البناءَ كلَّه، وهو ما يقعُ في بناءٍ بلا رسوماتٍ (SAD_ENABLE_GRAPHICS=OFF).
    (EN) Split runtime libraries into "build now" and "not defined in this
        configuration". Passing an undefined target to `cmake --build --target`
        fails the whole build, which is exactly what a headless
        (SAD_ENABLE_GRAPHICS=OFF) configuration would hit."""
    known = _known_targets()
    if known is None:
        return list(RUNTIME_LIBRARIES), []
    buildable = [name for name in RUNTIME_LIBRARIES if name in known]
    undefined = [name for name in RUNTIME_LIBRARIES if name not in known]
    return buildable, undefined


def _stage(config: str) -> dict:
    """(AR) ينسخ الثنائيَّين إلى dist/<config>/ ويكتب manifest بالبصمات.
    (EN) Copy both backends to dist/<config>/ and write a fingerprint manifest."""
    dest = DIST_DIR / config
    dest.mkdir(parents=True, exist_ok=True)
    manifest = {
        "config": config,
        "generated_utc": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "generator": _generator(),
        "binaries": {},
        "libraries": {},
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
    # (AR) مكتبات وقت التشغيل: يحتاجها **الرابط** حين يبني المترجَمُ برنامجَ
    #      مستخدمٍ يستورد رسوماتٍ أو شبكة. غيابها لا يمنع تشغيل المحرّكين، لذا
    #      لا نفشل على المفقود — نسجّل الموجود ونمضي.
    #      (Amelia مراجعة٣) نمسح البائتَ أوّلًا: النسخُ فوق الموجود يُبقي مكتبةً
    #      لهدفٍ لم تعد التهيئةُ تبنيه، وسائقُ المترجم يكشف الواجهةَ بوجود
    #      **الملفّ** لا بالـmanifest (has_library_file_in_dir) ⇒ يُربَط منطقُ
    #      واجهةٍ ميّتٌ ببرنامج المستخدم و`verify` صامت. بعد المسح يصير dist/
    #      مرآةً لِما بُني فعلًا في هذه الجولة لا تراكمًا تاريخيًّا.
    #      (مراجعة٣ب) المسحُ **مقصورٌ على ما نملكه** — أسماءُ RUNTIME_LIBRARIES
    #      بأنماطها — فمن يشحن SDL2.lib بجوار الثنائيّ لا نحذف مورده. ونحذف
    #      بعد النسخ لا قبله: لو أخفق الاكتشافُ ونجح البناءُ لَمَحونا سليمًا
    #      بلا بديل، فيفقد dist دعمَ الواجهات على تحسينٍ لا على عطب.
    # (EN) Wipe stale libs — but only files WE own (RUNTIME_LIBRARIES × patterns),
    #      and only after the copies succeeded, so a discovery miss never destroys
    #      a working dist. The driver detects UI by FILE presence, not the manifest.
    owned = {
        pattern.format(name=name)
        for name in RUNTIME_LIBRARIES
        for pattern in LIB_PATTERNS
    }
    skipped: list[str] = []
    staged_files: set[str] = set()
    for name in RUNTIME_LIBRARIES:
        src = _find_runtime_library(name, config)
        if src is None:
            skipped.append(name)
            continue
        out = dest / src.name
        shutil.copy2(src, out)
        staged_files.add(out.name)
        manifest["libraries"][name] = {
            "file": out.name,
            "sha256": _sha256(out),
            "size": out.stat().st_size,
            "source": str(src.relative_to(ROOT)).replace("\\", "/"),
        }
    for old in dest.iterdir():
        if old.is_file() and old.name in owned and old.name not in staged_files:
            old.unlink()
    # (AR) المتخطَّى جزءٌ من الحقيقة: يقرؤه cmd_build فيُعلنه، ويقرؤه cmd_verify
    #      فيُميّز في رسالة اليتيم «بقيّةَ بناءٍ سابق» عن «دخيلٍ مجهول».
    manifest["libraries_skipped"] = skipped
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
             f"(الخلفيّتان معًا / both backends together) ━━")
        # (AR) جوهر المرحلة 0: أمر بناء واحد يضمّ الهدفين. إن فشل أحدهما، فشل
        #      الأمر كلّه — فلا يخرج زوجٌ نصفُ مبنيّ بتهيئتين مختلفتين أبدًا.
        # (EN) Phase-0 core: one build command spanning BOTH targets. If either
        #      fails the whole command fails — never a half-built mismatched pair.
        build_cmd = ["cmake", "--build", str(BUILD_DIR)]
        if multi:
            build_cmd += ["--config", config]
        for target in ENGINES:
            build_cmd += ["--target", target]
        # (AR) مكتباتُ وقتِ التشغيلِ تُبنى **مع** المحرّكَين لا بعدَهما: المترجمُ
        #      يربطها في برامجِ المستخدم، فأرشيفٌ بائتٌ يعني ثنائيًّا يفشلُ ربطُه
        #      برمزٍ موجودٍ فعلًا في المصدر — تشخيصٌ مُضلِّلٌ يقودُ إلى مطاردةِ وهم.
        # (EN) Build the runtime archives WITH the engines: the compiler links them
        #      into user programs, so a stale archive yields "unresolved external
        #      symbol" for a symbol that is right there in the source.
        buildable_libraries, undefined_libraries = _buildable_runtime_libraries()
        for target in buildable_libraries:
            build_cmd += ["--target", target]
        _run(build_cmd)
        if undefined_libraries:
            _log("مكتبات وقت تشغيل غير معرَّفة في هذه التهيئة (تُتخطّى بلا فشل) / "
                 "runtime libraries not defined in this configuration (skipped): "
                 + ", ".join(undefined_libraries))

        # (AR) السائقُ يفضّلُ أرشيفاتِ Release مهما كانت تهيئتُه (أرشيفُ Debug
        #      بـMSVC يستلزمُ CRT التنقيح `_calloc_dbg` بينما يربطُ السائقُ CRT
        #      الإصدارَ دائمًا) — فبناءُ Debug وحدَه يترك ما يُربَط فعلًا بائتًا.
        #      نُحدِّثُ أرشيفاتِ Release كذلك في البناءِ متعدّدِ التهيئات.
        # (EN) The driver prefers Release archives whatever its own configuration
        #      (a Debug MSVC archive needs the debug CRT while the driver always
        #      links the release one), so a Debug-only build leaves what is actually
        #      linked stale. Refresh the Release archives too on multi-config.
        if multi and config != "Release" and buildable_libraries:
            refresh_cmd = ["cmake", "--build", str(BUILD_DIR), "--config", "Release"]
            for target in buildable_libraries:
                refresh_cmd += ["--target", target]
            _log("تحديث أرشيفات وقت التشغيل بتهيئة Release (هي ما يربطه المترجم) / "
                 "refreshing Release runtime archives (what the compiler links)")
            _run(refresh_cmd)

        manifest = _stage(config)
        _log(f"✓ {config}: ثُبّتت الخلفيّتان في / staged backends to dist/{config}/")
        for name, info in manifest["binaries"].items():
            _log(f"    {name:<10} sha256={info['sha256'][:16]}…  ({info['size']} bytes)")
        staged_libs = manifest.get("libraries", {})
        if staged_libs:
            _log(f"    + {len(staged_libs)} مكتبة وقت تشغيل للرابط / runtime libs "
                 f"for the linker: {', '.join(sorted(staged_libs))}")
        # (AR) الغيابُ يُعلَن ولا يُسكَت عنه: مكتبةٌ متوقَّعةٌ لم تُبنَ تعني برنامجَ
        #      مستخدمٍ يفشل ربطُه لاحقًا برموزٍ غير معرَّفة، والسببُ هنا لا هناك.
        skipped_libs = manifest.get("libraries_skipped", [])
        if skipped_libs:
            _log(f"    − لم تُبنَ في هذه التهيئة (تُخطّيت) / not built here: "
                 f"{', '.join(sorted(skipped_libs))}")

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
            _fail(f"خلفيّات ناقصة في / backends missing in {config}: {', '.join(missing)}")

        for name, info in manifest["binaries"].items():
            out = DIST_DIR / config / info["file"]
            if not out.exists():
                _fail(f"ثنائيّ مفقود / missing binary: {out}")
            actual = _sha256(out)
            if actual != info["sha256"]:
                _fail(f"بصمة بائتة / stale fingerprint: {name} ({config}) — "
                      f"الملف على القرص لا يطابق manifest. أعد البناء.")

        # (AR) مكتبات وقت التشغيل تخضع لنفس كشف البيات: مكتبةٌ بائتةٌ في dist/
        #      تعني برنامجَ مستخدمٍ يُربَط بمنطقٍ قديمٍ صامتًا — أخطرُ من فشل ربط.
        for name, info in manifest.get("libraries", {}).items():
            out = DIST_DIR / config / info["file"]
            if not out.exists():
                _fail(f"مكتبة وقت تشغيل مفقودة / missing runtime library: {out}")
            if _sha256(out) != info["sha256"]:
                _fail(f"بصمة بائتة / stale fingerprint: {name} ({config}) — "
                      f"الملف على القرص لا يطابق manifest. أعد البناء.")

        # (AR) (Amelia مراجعة٣) الاتّجاه المعاكس: ملفُّ مكتبةٍ في dist/ لا يذكره
        #      الـmanifest. البصماتُ تكشف تبدُّلَ المذكور، وهذا يكشف الدخيل —
        #      وهو الأخطر: سائقُ المترجم يكشف الواجهةَ بوجود الملفّ فيربط منطقًا
        #      لا يعرف أحدٌ من أيّ التزامٍ جاء.
        # (EN) The other direction: a library file in dist/ the manifest does not
        #      list. The driver detects UI by file presence, so an orphan silently
        #      links logic from an unknown commit.
        #      النطاقُ مقصورٌ على أسماءِ RUNTIME_LIBRARIES: موردٌ يشحنه المستخدم
        #      بجوار الثنائيّ (SDL2.lib مثلًا) ليس دخيلًا فلا يُفشِل البوّابة.
        listed = {info["file"] for info in manifest.get("libraries", {}).values()}
        owned = {
            pattern.format(name=name)
            for name in RUNTIME_LIBRARIES
            for pattern in LIB_PATTERNS
        }
        skipped_files = {
            pattern.format(name=name)
            for name in manifest.get("libraries_skipped", [])
            for pattern in LIB_PATTERNS
        }
        orphans = sorted(
            p.name for p in (DIST_DIR / config).iterdir()
            if p.is_file() and p.name in owned and p.name not in listed
        )
        if orphans:
            # (AR) نُميّز بقيّةَ بناءٍ سابقٍ لهدفٍ لم يُبنَ الآن عن الدخيل المجهول.
            stale = [n for n in orphans if n in skipped_files]
            hint = (f" (بقيّةُ بناءٍ سابق لأهدافٍ لم تُبنَ في هذه التهيئة: "
                    f"{', '.join(stale)})" if stale else "")
            _fail(f"مكتبات يتيمة في / orphan libraries in dist/{config}: "
                  f"{', '.join(orphans)}{hint} — أعد البناء (x.py build) لتنظيفها.")

        _log(f"✓ {config}: زوج متطابق التهيئة، بصمات سليمة / consistent pair, fingerprints OK.")

    _log("✓ التحقّق نجح / verification passed.")


def cmd_test(args: argparse.Namespace) -> None:
    configs = _parse_configs(args.config)
    if len(configs) != 1:
        _fail("test يقبل تهيئة واحدة فقط / test accepts exactly one config.")
    config = configs[0]

    # (AR) ابنِ الخلفيّتين معًا ثم تحقّق قبل تشغيل الاختبارات.
    # (EN) Build both backends together, then verify, before running tests.
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
        # (AR) 🔑 الرمزُ 2 عطبُ **آلةٍ** لا عطبُ **محتوى**: الحارسُ لم ينظرْ أصلًا
        #      (مسبارٌ أعمى، أو عيارُ أداةٍ سقط). وخلطُه بالرمز 1 يجعلُ أداةً
        #      عمياءَ تُقرأُ «وجدَت عطبًا» وهي لم تقِسْ شيئًا — وكلاهما يُفشِلُ
        #      البوّابةَ، لكنّ التشخيصَ يوجّهُ إلى الموضعِ الصحيح.
        # (EN) Exit 2 is a TOOL fault (blind probe / failed self-calibration), not a
        #      content fault. Both fail the gate; conflating them misdirects the fix.
        if result.returncode == 2:
            _fail(f"عطبُ آلةٍ في حارس الاتّساق — لم يقِسْ شيئًا / guard tool fault"
                  f" (did not measure): {guard['name']}")
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

    pb = sub.add_parser("build", help="بناء الخلفيّتين معًا لكل تهيئة / build both backends per config")
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
