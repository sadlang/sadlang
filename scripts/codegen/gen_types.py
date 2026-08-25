#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مُولِّد كود C++ من types.yaml — لغة ص
(EN) C++ codegen from types.yaml — Sad Language
----------------------------------------------------------------------------
(AR) يقرأ مصدر الحقيقة للأنواع (types.yaml) ويُنتج:
       - sad_type_kind_generated.h : تعداد SadTypeKind الموحَّد (كل القيم)
     ⚠️ الترتيب حرج: القيم تُولَّد بترتيب القائمة (توافق ثنائي/تسلسل).
     يُستدعى من CMake وقت البناء عند تغيّر YAML.
(EN) Reads the type SoT (types.yaml) and emits the unified SadTypeKind enum.
     ORDER IS CRITICAL — emitted in list order (binary/serialization compat).
----------------------------------------------------------------------------
(AR) أي خطأ في YAML أو schema يُسبب فشل البناء بـ stderr مفصّل.
(EN) Any YAML/schema error fails the build with detailed stderr.
============================================================================
"""

from __future__ import annotations

import argparse
import sys
import traceback
from pathlib import Path
from typing import Any

# (AR) [S-TS-P11] إعادة ضبط ترميز الإخراج إلى UTF-8 — لمنع انهيار طباعة الرموز (✓)
#      والعربية على وحدات تحكّم Windows (cp1252) أثناء استدعاء CMake للمولّد.
# (EN) [S-TS-P11] Force UTF-8 output so the ✓ glyph and Arabic don't crash on
#      Windows cp1252 consoles when CMake invokes the generator.
for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8", errors="replace")  # type: ignore[attr-defined]
    except Exception:
        pass

# (AR) إضافة مجلد codegen للمسار لاستيراد المكتبة المشتركة _lib
# (EN) Add codegen dir to path for the shared _lib import
_CODEGEN_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(_CODEGEN_DIR))

try:
    from _lib import (  # type: ignore
        load_yaml,
        load_schema,
        validate_schema,
        write_if_changed,
        validate_no_duplicates,
    )
except ImportError as e:
    print(f"[gen_types] FATAL: cannot import _lib helpers: {e}", file=sys.stderr)
    sys.exit(2)

try:
    from jsonschema import ValidationError
except ImportError:
    print("[gen_types] FATAL: jsonschema not installed.", file=sys.stderr)
    print("[gen_types] Install: pip install pyyaml jsonschema", file=sys.stderr)
    sys.exit(2)


# =====================================================================
# (AR) عناوين الأقسام لكل فئة (عربي/إنجليزي) — للتعليقات فقط
# (EN) Section headers per category — for comments only
# =====================================================================
CATEGORY_SECTION = {
    "primitive":       ("أنواع بدائية", "Primitive types"),
    "sized_primitive": ("أنواع بدائية بحجم محدد (للمترجم)", "Sized primitives (for compiler)"),
    "composite":       ("أنواع مركبة", "Composite types"),
    "oop":             ("أنواع كائنية", "OOP types"),
    "function":        ("أنواع وظيفية", "Function types"),
    "advanced":        ("أنواع متقدمة", "Advanced types"),
    "pointer":         ("مؤشرات ومراجع", "Pointers and references"),
    "special":         ("أنواع خاصة", "Special types"),
    "async":           ("أنواع غير متزامنة", "Async types"),
    "graphics":        ("أنواع الرسومات", "Graphics types"),
    "simd":            ("متجهات SIMD", "SIMD vectors"),
}


# =====================================================================
# (AR) مفرداتُ القيمةِ الافتراضيّةِ للتهيئة (default_init) — الترتيبُ حرجٌ:
#      «Unspecified» أوّلُها فقيمتُها صفرٌ، فخانةٌ مُصفَّرةٌ تعني «لا افتراضيَّ
#      مُعلَنًا» لا قيمةً مُخترَعة. وكلُّ لفظٍ في types.yaml لا يَرِد هنا **يكسر
#      البناء**: فلا يُضاف لفظٌ إلى مصدرِ الحقيقةِ ويسقط صامتًا في احتياطٍ.
#      ⚠️ ولا يُكتَب اللفظ null عاريًا في YAML: يحوّله المُحمِّل إلى العدم
#      فيصير الحقلُ None ويسقط الصفُّ صامتًا — ولذلك اللفظ null_value.
# (EN) default_init vocabulary — order is critical: «Unspecified» is first so
#      its value is 0, meaning a zeroed slot reads as "no declared default"
#      rather than an invented value. Any word in types.yaml missing from this
#      map BREAKS THE BUILD instead of falling silently into a fallback.
#      Never write a bare `null` in YAML — the loader turns it into None.
# =====================================================================
# (AR) (لفظ YAML، اسم C++، وصف عربي، وصف إنجليزي)
# (EN) (YAML word, C++ enumerator, Arabic doc, English doc)
DEFAULT_INIT_VOCAB: list[tuple[str, str, str, str]] = [
    ("unspecified",  "Unspecified", "لا افتراضيَّ مُعلَنًا — يُبقي كلُّ محرّكٍ احتياطَه",
     "no declared default - each engine keeps its own fallback"),
    ("int_zero",     "IntZero",     "صفرٌ صحيح", "integer zero"),
    ("float_zero",   "FloatZero",   "صفرٌ عشريّ", "float zero"),
    ("bool_false",   "BoolFalse",   "خطأ", "boolean false"),
    ("empty_string", "EmptyString", "نصٌّ فارغ", "empty string"),
    ("null_value",   "Null",        "لاشيء — العدمُ الصريح", "the explicit null value"),
    ("void",         "Void",        "فراغٌ — لم تُسنَدْ بعدُ، متمايزٌ عن العدم",
     "void - never assigned, distinct from null"),
    # (AR) 🔑 «لا محلَّ للسؤال» ليست «لم يُحسَمْ بعدُ». النوعُ الموسومُ بهذه لا
    #      تحمله خانةٌ أصلًا فيرفضه المحلّلُ المشترك — فلا قيمةَ افتراضيّةَ له
    #      لأنّه لا خانةَ له، لا لأنّ القرارَ مؤجَّل. وخلطُها بـUnspecified
    #      يجعل قارئًا يحسب على «فراغ» دَينًا ولا دَينَ عليه.
    # (EN) «no slot can hold it» is NOT «undecided». A kind marked this way is
    #      rejected by the shared parser, so it has no default because it has no
    #      slot — not because a decision is pending.
    ("not_a_slot",   "NotASlot",    "لا خانةَ تحمله — يرفضه المحلّل (SEM040)",
     "no slot can hold it - rejected by the parser (SEM040)"),
]


def emit_header(types: list[dict[str, Any]], removed: list[dict[str, Any]] | None = None) -> str:
    """
    (AR) يُنتج محتوى sad_type_kind_generated.h من قائمة الأنواع المُرتّبة.
    (EN) Emits sad_type_kind_generated.h content from the ordered type list.
    """
    lines: list[str] = []
    lines.append("// ============================================================================")
    lines.append("// AUTO-GENERATED FROM language-truth/types.yaml — DO NOT EDIT MANUALLY")
    lines.append("// (AR) ملف مولَّد تلقائياً. لإجراء تعديل، عدّل types.yaml وأعد البناء.")
    lines.append("// (EN) Auto-generated file. To modify, edit types.yaml and rebuild.")
    lines.append("// ============================================================================")
    lines.append("")
    lines.append("#pragma once")
    lines.append("")
    lines.append("#include <array>")
    lines.append("#include <cstddef>")
    lines.append("#include <string_view>")
    lines.append("")
    lines.append("namespace Sad")
    lines.append("{")
    lines.append("    namespace Types")
    lines.append("    {")
    lines.append("")
    lines.append("        /**")
    lines.append("         * @brief (AR) التعداد الموحد لكل أنواع لغة ص — مُولَّد من types.yaml")
    lines.append("         * @brief (EN) Unified type kind enum — generated from types.yaml")
    lines.append("         *")
    lines.append(f"         * (AR) عدد القيم: {len(types)} — الترتيب حرج (توافق ثنائي).")
    lines.append(f"         * (EN) {len(types)} values — order is critical (binary compat).")
    lines.append("         */")
    lines.append("        enum class SadTypeKind : int")
    lines.append("        {")

    prev_cat: str | None = None
    # (AR) أطول اسم kind لمحاذاة التعليقات / (EN) longest kind for comment alignment
    max_kind = max(len(t["kind"]) for t in types)

    for t in types:
        cat = t["category"]
        if cat != prev_cat:
            ar, en = CATEGORY_SECTION.get(cat, (cat, cat))
            lines.append(f"            // ─── {ar} / {en} ───")
            prev_cat = cat
        kind = t["kind"]
        word = t.get("word", "")
        name_en = t.get("name_en", "")
        pad = " " * (max_kind - len(kind))
        lines.append(f"            {kind},{pad} ///< {word} / {name_en}")

    lines.append("        };")
    lines.append("")
    lines.append("        /**")
    lines.append("         * @brief (AR) عدد قيم SadTypeKind — تستهلكه static_assert في جسور")
    lines.append("         *             الأنواع (مثل astTypeToSIRType) كي **يكسر البناءَ** عند")
    lines.append("         *             إضافة نوعٍ جديدٍ إلى types.yaml، فلا يسقط النوعُ الجديد")
    lines.append("         *             صامتًا في فرعٍ افتراضيٍّ يُخمّن تمثيلًا خاطئًا.")
    lines.append("         * @brief (EN) SadTypeKind cardinality — consumed by static_assert in the")
    lines.append("         *             type bridges (e.g. astTypeToSIRType) so that adding a kind")
    lines.append("         *             to types.yaml BREAKS THE BUILD instead of letting the new")
    lines.append("         *             kind fall silently into a guessing default branch.")
    lines.append("         */")
    lines.append(f"        inline constexpr int SAD_TYPE_KIND_COUNT = {len(types)};")
    lines.append("")

    # ========================================================================
    # (AR) 🔑 القيمةُ الافتراضيّةُ لخانةٍ صُرِّح نوعُها ولم تُهيَّأ — جدولٌ **واحدٌ
    #      مُولَّد** بدلَ ستِّ نسخٍ يدويّة. كان لكلِّ موضعٍ جدولُه، فانجرفَ الجدولُ
    #      في الموضعِ الذي يُنسى: «طبيعي64» غابت عن نسخةِ المترجمِ وحدَها فبقيت
    #      خانتُها تُقرأ على ما تركه المكدَّس، مقيسةً تتبدّل في كلِّ تشغيل.
    #
    #      والقيمةُ **رمزيّةٌ لا تعبيرَ C++**: كلُّ محرّكٍ يترجمها إلى تمثيلِه
    #      (Value في المفسّر، SIROperand في المترجم)، فيبقى القرارُ واحدًا
    #      والتمثيلُ لكلٍّ. ولذلك يوزّع المستهلكُ على **سبعِ** قيمٍ لا اثنتَين
    #      وخمسين — وهو ما يجعل الشمولَ قابلًا للفحصِ أصلًا.
    #
    #      والجدولُ **مصفوفةٌ مفهرسةٌ بقيمةِ التعداد** لا switch: فطولُها مربوطٌ
    #      بـSAD_TYPE_KIND_COUNT بنيويًّا، ولا ذراعَ يمكن أن تُنسى.
    # (EN) Default value of a declared-but-uninitialised slot - ONE generated
    #      table replacing six hand-written copies. The value is symbolic, not a
    #      C++ expression: each engine lowers it to its own representation, so the
    #      decision is single and the representation is local. Consumers switch on
    #      SEVEN values instead of fifty-two, which is what makes exhaustiveness
    #      checkable at all. An enum-indexed array, not a switch: its length is
    #      tied to SAD_TYPE_KIND_COUNT structurally and no arm can be forgotten.
    # ========================================================================
    known_words = {word for word, _cxx, _ar, _en in DEFAULT_INIT_VOCAB}
    for entry in types:
        seen = entry.get("default_init")
        if seen not in known_words:
            raise ValueError(
                f"types.yaml: {entry.get('id', '?')} has default_init={seen!r}, "
                f"which is not in DEFAULT_INIT_VOCAB ({sorted(known_words)}). "
                "Add it to gen_types.py AND to every consumer that switches on it."
            )

    lines.append("        /**")
    lines.append("         * @brief (AR) مفرداتُ القيمةِ الافتراضيّةِ للتهيئة — مُولَّدة من types.yaml")
    lines.append("         * @brief (EN) Default-initialisation vocabulary — generated from types.yaml")
    lines.append("         *")
    lines.append("         * (AR) رمزيّةٌ لا تمثيليّة: يترجمها كلُّ محرّكٍ إلى قيمتِه الخاصّة.")
    lines.append("         *      «Unspecified» صفرٌ عمدًا: خانةٌ مُصفَّرةٌ تعني «غيرُ مُعلَن».")
    lines.append("         * (EN) Symbolic, not representational — each engine lowers it itself.")
    lines.append("         *      «Unspecified» is 0 on purpose: a zeroed slot reads as undeclared.")
    lines.append("         */")
    lines.append("        enum class SadDefaultInit : int")
    lines.append("        {")
    _max_cxx = max(len(cxx) for _w, cxx, _ar, _en in DEFAULT_INIT_VOCAB)
    for _word, cxx, desc_ar, desc_en in DEFAULT_INIT_VOCAB:
        pad = " " * (_max_cxx - len(cxx))
        lines.append(f"            {cxx},{pad} ///< {desc_ar} / {desc_en}")
    lines.append("        };")
    lines.append("")
    lines.append("        /**")
    lines.append("         * @brief (AR) عددُ مفرداتِ SadDefaultInit — تستهلكه static_assert في كلِّ")
    lines.append("         *             مستهلكٍ يوزّع عليها، فإضافةُ مفردةٍ جديدةٍ **تكسر البناءَ**")
    lines.append("         *             عند كلِّ مَن يقرّر تمثيلَها، بدل أن تسقط في احتياطٍ صامت.")
    lines.append("         * @brief (EN) SadDefaultInit cardinality — consumed by static_assert at every")
    lines.append("         *             consumer, so adding a vocabulary word BREAKS THE BUILD at each")
    lines.append("         *             site that must decide its representation.")
    lines.append("         */")
    lines.append(
        f"        inline constexpr int SAD_DEFAULT_INIT_COUNT = {len(DEFAULT_INIT_VOCAB)};"
    )
    lines.append("")
    _cxx_of = {word: cxx for word, cxx, _ar, _en in DEFAULT_INIT_VOCAB}
    lines.append("        /**")
    lines.append("         * @brief (AR) جدولُ القيمِ الافتراضيّةِ مفهرسًا بقيمةِ SadTypeKind — مُولَّد")
    lines.append("         * @brief (EN) Default-init table indexed by SadTypeKind value — generated")
    lines.append("         */")
    lines.append(
        "        inline constexpr std::array<SadDefaultInit, SAD_TYPE_KIND_COUNT> "
        "SAD_TYPE_DEFAULT_INIT_TABLE = {{"
    )
    _max_entry = max(len(_cxx_of[entry["default_init"]]) for entry in types)
    for entry in types:
        entry_cxx = _cxx_of[entry["default_init"]]
        pad = " " * (_max_entry - len(entry_cxx))
        lines.append(
            f"            SadDefaultInit::{entry_cxx},{pad} // "
            f"{entry['kind']} \u2014 {entry.get('word', '')}"
        )
    lines.append("        }};")
    lines.append("")
    lines.append("        /**")
    lines.append("         * @brief (AR) القيمةُ الافتراضيّةُ الرمزيّةُ لنوعٍ ما — مُولَّدة من types.yaml")
    lines.append("         * @brief (EN) Symbolic default-init of a kind — generated from types.yaml")
    lines.append("         *")
    lines.append("         * (AR) دالّةٌ كلّيّةٌ بنيويًّا: الفهرسةُ بقيمةِ التعدادِ الكثيفة، وقيمةٌ")
    lines.append("         *      خارجَ المدى تُرجِع «Unspecified» — أي «لا أعرف» لا قيمةً مُخترَعة.")
    lines.append("         * (EN) Structurally total: dense enum indexing; an out-of-range value")
    lines.append("         *      yields «Unspecified» - never an invented value.")
    lines.append("         */")
    lines.append("        inline constexpr SadDefaultInit sadTypeKindDefaultInit(SadTypeKind kind)")
    lines.append("        {")
    lines.append("            const int index = static_cast<int>(kind);")
    lines.append("            if (index < 0 || index >= SAD_TYPE_KIND_COUNT)")
    lines.append("            {")
    lines.append("                return SadDefaultInit::Unspecified;")
    lines.append("            }")
    lines.append("            return SAD_TYPE_DEFAULT_INIT_TABLE[static_cast<std::size_t>(index)];")
    lines.append("        }")
    lines.append("")

    # ========================================================================
    # (AR) 🔑 جدولُ أحجامِ الخاناتِ — مصدرُ `@حجم(نوع)` في المحرّكَين معًا.
    #      كان جدولَين مكتوبَين باليدِ تباعدا في ٦ من ١١ نوعًا وافتراضاهما
    #      يكذبان (٠ في المفسّرِ و٨ في المترجّم للنوعِ المجهول). والغيابُ ههنا
    #      **مُعلَنٌ لا مسكوتٌ عنه**: `kSadTypeSizeUnknown` قيمةٌ مميّزةٌ سالبةٌ
    #      تُلزِمُ المُنادي أن يُشخِّصَ، فلا يستطيعُ أن يقرأَها رقمًا صحيحًا سهوًا.
    # (EN) Slot-size table — the single source for `@حجم` in both engines. The
    #      absence is DECLARED, not silent: kSadTypeSizeUnknown is a distinct
    #      negative value that forces the caller to diagnose.
    # ========================================================================
    lines.append("        /**")
    lines.append("         * @brief (AR) قيمةُ «لا حجمَ ثابتَ لهذا النوع» — تُشخَّصُ ولا تُطبَع")
    lines.append("         * @brief (EN) Sentinel: this kind has no fixed slot size — diagnose, never print")
    lines.append("         */")
    lines.append("        inline constexpr int kSadTypeSizeUnknown = -1;")
    lines.append("")
    lines.append("        /**")
    lines.append("         * @brief (AR) جدولُ حجمِ الخانةِ بالبايتات مفهرسًا بـSadTypeKind — مُولَّد")
    lines.append("         * @brief (EN) Slot size in bytes indexed by SadTypeKind — generated")
    lines.append("         */")
    lines.append(
        "        inline constexpr std::array<int, SAD_TYPE_KIND_COUNT> "
        "SAD_TYPE_SIZE_BYTES_TABLE = {{"
    )
    for entry in types:
        raw = entry.get("size_bytes")
        cell = "kSadTypeSizeUnknown" if raw is None else str(int(raw))
        lines.append(
            f"            {cell}," + " " * max(0, 20 - len(cell)) + " // "
            f"{entry['kind']} — {entry.get('word', '')}"
        )
    lines.append("        }};")
    lines.append("")
    lines.append("        /**")
    lines.append("         * @brief (AR) حجمُ خانةِ نوعٍ بالبايتات — مُولَّد من types.yaml")
    lines.append("         * @brief (EN) Slot size of a kind in bytes — generated from types.yaml")
    lines.append("         *")
    lines.append("         * (AR) يُرجِعُ kSadTypeSizeUnknown لِما لا حجمَ ثابتَ له (صنفٌ، متّجهٌ،")
    lines.append("         *      نوعٌ عامّ) ولِقيمةٍ خارجَ المدى — «لا أعرف» لا رقمًا مُخترَعًا.")
    lines.append("         * (EN) Returns kSadTypeSizeUnknown for kinds without a fixed size and")
    lines.append("         *      for out-of-range values — «unknown», never an invented number.")
    lines.append("         */")
    lines.append("        inline constexpr int sadTypeKindSizeBytes(SadTypeKind kind)")
    lines.append("        {")
    lines.append("            const int index = static_cast<int>(kind);")
    lines.append("            if (index < 0 || index >= SAD_TYPE_KIND_COUNT)")
    lines.append("            {")
    lines.append("                return kSadTypeSizeUnknown;")
    lines.append("            }")
    lines.append("            return SAD_TYPE_SIZE_BYTES_TABLE[static_cast<std::size_t>(index)];")
    lines.append("        }")
    lines.append("")

    # ========================================================================
    # (AR) دالة الاسم العربي لـنوع() — مصدر حقيقة واحد للمحرّكين (مفسّر + مترجم).
    #      تُرجع typeof_ar إن وُجد، وإلا word، وإلا «مجهول». تُرمَّز القيم بـ\xHH
    #      (بايتات UTF-8) لضمان صحّتها في MSVC بغضّ النظر عن ترميز الملف المصدر.
    # (EN) Arabic name for نوع()/typeof — single source of truth for BOTH engines.
    #      Returns typeof_ar if set, else word, else «مجهول». Names are emitted as
    #      \xHH UTF-8 byte escapes so MSVC reads them correctly regardless of file
    #      encoding (mirrors the previous hand-written interpreter literals).
    # ========================================================================
    def hex_escape(s: str) -> str:
        return "".join(f"\\x{b:02x}" for b in s.encode("utf-8"))

    unknown = hex_escape("مجهول")
    lines.append("        /**")
    lines.append("         * @brief (AR) الاسم العربي الذي تُرجعه نوع() لنوعٍ ما — مُولَّد من types.yaml")
    lines.append("         * @brief (EN) Arabic name returned by نوع()/typeof for a kind — generated")
    lines.append("         */")
    lines.append("        inline const char *sadTypeKindArabicName(SadTypeKind kind)")
    lines.append("        {")
    lines.append("            switch (kind)")
    lines.append("            {")
    for t in types:
        name = t.get("typeof_ar") or t.get("word") or "مجهول"
        lines.append(f'            case SadTypeKind::{t["kind"]}: return "{hex_escape(name)}"; // {name}')
    lines.append(f'            default: return "{unknown}"; // مجهول')
    lines.append("            }")
    lines.append("        }")
    lines.append("")

    surface = [t for t in types if t.get("surface") is True]

    # ========================================================================
    # (AR) الاتّجاه المعاكس: اللفظ ⇒ النوع. كان **غيرَ مولَّدٍ إطلاقًا**، فكتبه
    #      كلُّ موضعٍ يحتاجه بيدِه — ثلاثةُ جداولَ في ملفَّين. وقياسُ ISSUE-113
    #      أظهر ثمنَ ذلك: أُصلح «عدم» في المواضعِ المفتاحيّةِ الثلاثةِ فلم يتغيّر
    #      السلوكُ بتّةً، لأنّ تصريحَ المتغيّرِ يسلك جدولَ **الاسم** لا الرمز،
    #      وهو جدولٌ ثالثٌ لم يره الإصلاح. جدولٌ واحدٌ مولَّدٌ يُنهي هذا الصنف.
    # (EN) The reverse direction (word ⇒ kind) was never generated, so every site
    #      hand-rolled it — three tables in two files. ISSUE-113 measured the cost:
    #      fixing «عدم» at the token sites changed nothing, because variable
    #      declarations take the *name* table, a third table the fix never saw.
    # ========================================================================
    lines.append("        // ─── اللفظ ⇒ النوع / Word ⇒ kind ───")
    lines.append("        /**")
    lines.append("         * @brief (AR) نوعُ اللفظِ السطحيّ — مُولَّد من types.yaml")
    lines.append("         * @brief (EN) Kind of a surface type word — generated from types.yaml")
    lines.append("         *")
    lines.append("         * (AR) يُرجِع Unknown لِما ليس لفظَ نوعٍ سطحيّ — فالمُنادي يميّز")
    lines.append("         *      «ليس نوعًا» عن «نوعٌ مجهول» بموضعِه لا بهذه الدالّة.")
    lines.append("         * (EN) Returns Unknown for anything that is not a surface type word.")
    lines.append("         */")
    lines.append("        inline SadTypeKind sadTypeKindFromArabicName(std::string_view word)")
    lines.append("        {")
    for t in surface:
        name = t.get("word") or "?"
        lines.append(f'            if (word == "{hex_escape(name)}") return SadTypeKind::{t["kind"]}; // {name}')
    lines.append("            return SadTypeKind::Unknown;")
    lines.append("        }")
    lines.append("")

    # ========================================================================
    # (AR) أسماء الأنواع السطحية (surface:true) — المرئيّة في كود المستخدم.
    #      للأدوات (LSP: تلوينها كأنواع، الإكمال). مشتقّة من types.yaml لا تهريد.
    # (EN) Surface type names (surface:true) — user-visible in source. For tooling
    #      (LSP type coloring/completion). Derived from types.yaml, not hardcoded.
    # ========================================================================
    lines.append("        // ─── أسماء الأنواع السطحية / Surface type names ───")
    lines.append("        /**")
    lines.append("         * @brief (AR) أسماء الأنواع السطحية (surface:true) — مُولَّدة من types.yaml")
    lines.append("         * @brief (EN) Surface type names (surface:true) — generated from types.yaml")
    lines.append("         *")
    lines.append(f"         * (AR) العدد: {len(surface)} — يستهلكها مزوّد الرموز الدلاليّة لتلوينها أنواعًا.")
    lines.append(f"         * (EN) {len(surface)} names — consumed by the semantic-tokens provider.")
    lines.append("         */")
    lines.append(f"        inline constexpr std::array<std::string_view, {len(surface)}> SURFACE_TYPE_NAMES = {{{{")
    for t in surface:
        name = t.get("word") or "?"
        lines.append(f'            "{hex_escape(name)}", // {name}')
    lines.append("        }};")
    lines.append("")

    # ========================================================================
    # (AR) ألفاظ أنواعٍ أُزيلت من اللغة (removed_type_words) — يجب أن يظلّ
    #      المحلّل يتعرّف عليها كي يُبلِّغ SYN014 موجِّهًا إلى البديل. حذفُها من
    #      تعرُّف المحلّل يبدِّل التشخيص الودّيّ بآخرَ **معقولٍ خاطئ** (لفظُ النوع
    #      واسمُ المتغيّر يُقرآن اسمًا واحدًا فيه مسافة ⇒ SYN016 ينصح باستعمال
    #      اللفظ المُزال اسمًا للمتغيّر). فالقائمة مُولَّدة لا مُصلَّبة.
    # (EN) Type words removed from the language: the parser must still recognise
    #      them to emit SYN014 pointing at the replacement. Dropping them turns a
    #      helpful diagnostic into a plausible-but-wrong one.
    # ========================================================================
    removed_list = removed or []
    lines.append("        // ─── ألفاظ أنواعٍ مُزالة / Removed type words ───")
    lines.append("        /**")
    lines.append("         * @brief (AR) ألفاظ الأنواع المُزالة — مُولَّدة من types.yaml")
    lines.append("         * @brief (EN) Removed type words — generated from types.yaml")
    lines.append("         *")
    lines.append(f"         * (AR) العدد: {len(removed_list)} — يستهلكها المحلّل ليُبقي تشخيص SYN014 حيًّا.")
    lines.append(f"         * (EN) {len(removed_list)} — consumed by the parser to keep SYN014 alive.")
    lines.append("         */")
    lines.append(
        f"        inline constexpr std::array<std::string_view, {len(removed_list)}> REMOVED_TYPE_NAMES = {{{{"
    )
    for r in removed_list:
        name = r.get("word") or "?"
        lines.append(f'            "{hex_escape(name)}", // {name} ⇒ {r.get("replacement", "?")}')
    lines.append("        }};")
    lines.append("")

    # ========================================================================
    # (AR) وصف النوع السطحيّ بالعربية (description_ar) بحسب الكلمة — للأدوات
    #      (LSP: تلميح hover). يُرجِع الوصف من types.yaml أو "" إن لم يكن سطحيًّا.
    #      بهذا يكون وصف الأنواع في التلميح مشتقًّا من مصدر الحقيقة لا مهرَّدًا.
    # (EN) Arabic description of a surface type by word (from description_ar) — for
    #      tooling (LSP hover). Returns "" for non-surface words. Keeps hover type
    #      descriptions sourced from the SoT instead of hand-edited.
    # ========================================================================
    lines.append("        /**")
    lines.append("         * @brief (AR) وصف النوع السطحيّ بالعربية بحسب كلمته — مُولَّد من types.yaml")
    lines.append("         * @brief (EN) Arabic description of a surface type by its word — generated")
    lines.append("         */")
    lines.append("        inline const char *surfaceTypeDescriptionAr(std::string_view word)")
    lines.append("        {")
    for t in surface:
        name = t.get("word") or "?"
        desc = t.get("description_ar", "")
        lines.append(f'            if (word == "{hex_escape(name)}") return "{hex_escape(desc)}"; // {name}')
    lines.append('            return "";')
    lines.append("        }")
    lines.append("")
    lines.append("    } // namespace Types")
    lines.append("} // namespace Sad")
    lines.append("")
    return "\n".join(lines)


# =====================================================================
# (AR) مُولِّد تخطيط البُنى المضمَّنة ذات الحقول (حدث…) — مصدر حقيقة واحد
#      يستهلكه: المترجم (هيكل + POD الجسر)، وقت التشغيل (تعبئة POD من EventData).
# (EN) Built-in field-bearing struct layout generator (حدث…) — single SoT
#      consumed by: the compiler (struct + bridge POD) and the runtime (POD fill).
# =====================================================================
# (AR) خانة ABI لكلّ نوع حقل — تحدّد نوع خانة POD وخانة LLVM
# (EN) ABI slot per field kind — determines POD member type and LLVM slot type
_KIND_C_TYPE = {
    "Float":   "double",
    "Integer": "int64_t",
    "Boolean": "int64_t",  # (AR) منطقيّ يُنقل كـ i64 (أأمن ABI) ثمّ يُقتطع i1 في الـthunk
    "String":  "const char *",
}
_KIND_ABI_SLOT = {
    "Float":   "F64",
    "Integer": "I64",
    "Boolean": "I64",
    "String":  "PTR",
}
_EVENT_TYPE_SOURCE = "__type"  # (AR) مصدر خاصّ: نوع الحدث المحقون (ليس عضو EventData)


def _pod_member(source: str) -> str:
    # (AR) اسم عضو POD: نظير EventData الإنجليزيّ، وeventType لنوع الحدث المحقون
    return "eventType" if source == _EVENT_TYPE_SOURCE else source


def emit_layout_header(types: list[dict[str, Any]]) -> str:
    """
    (AR) يُنتج sad_event_layout_generated.h من الأنواع التي تحمل `fields`.
    (EN) Emits sad_event_layout_generated.h from types carrying `fields`.
    """
    def hex_escape(s: str) -> str:
        return "".join(f"\\x{b:02x}" for b in s.encode("utf-8"))

    # (AR) اليوم: حدث فقط يحمل حقولًا — لكن المولّد عامّ لأيّ نوع بحقول
    field_types = [t for t in types if isinstance(t.get("fields"), list) and t["fields"]]
    event = next((t for t in field_types if t["kind"] == "Event"), None)

    L: list[str] = []
    L.append("// ============================================================================")
    L.append("// AUTO-GENERATED FROM language-truth/types.yaml — DO NOT EDIT MANUALLY")
    L.append("// (AR) تخطيط البُنى المضمَّنة (حدث). عدّل types.yaml (حقول حدث) وأعد البناء.")
    L.append("// (EN) Built-in struct layouts (حدث). To modify, edit types.yaml and rebuild.")
    L.append("// ============================================================================")
    L.append("")
    L.append("#pragma once")
    L.append("")
    L.append("#include <array>")
    L.append("#include <cstdint>")
    L.append("#include <string_view>")
    L.append('#include "sad_type_kind_generated.h"')
    L.append("")
    L.append("namespace Sad")
    L.append("{")
    L.append("    namespace Types")
    L.append("    {")
    L.append("        namespace EventLayout")
    L.append("        {")

    if event is None:
        # (AR) لا نوع حدث بحقول — رأسٌ فارغ صالح (لا يكسر البناء)
        L.append("            // (AR) لا حقول معرّفة لنوع حدث في types.yaml.")
        L.append("        } // namespace EventLayout")
        L.append("    } // namespace Types")
        L.append("} // namespace Sad")
        L.append("")
        return "\n".join(L)

    fields = event["fields"]
    n = len(fields)

    # ─── خانة ABI للجسر / bridge ABI slot ───
    L.append("            // (AR) خانة الجسر ثلاثيّة الأنواع (تبسيط ABI): i64/double/مؤشّر")
    L.append("            // (EN) Tri-typed bridge slot (ABI simplification): i64/double/pointer")
    L.append("            enum class AbiSlot : int { I64, F64, PTR };")
    L.append("")

    # ─── بنية POD الجسر (وقت التشغيل ← thunk المترجم) ───
    L.append("            // (AR) POD الجسر: يملؤه وقت التشغيل من EventData ويقرؤه thunk المترجم")
    L.append("            // (EN) Bridge POD: filled by runtime from EventData, read by compiler thunk")
    L.append("            struct SadEventPod")
    L.append("            {")
    for f in fields:
        c_type = _KIND_C_TYPE[f["kind"]]
        member = _pod_member(f["source"])
        sep = "" if c_type.endswith("*") else " "
        L.append(f"                {c_type}{sep}{member} = {{}}; // {f['name']} ← {f['source']}")
    L.append("            };")
    L.append("")

    # ─── جدول الحقول (اسم عربيّ + نوع + خانة ABI) للمترجم/الأدوات ───
    L.append("            // (AR) وصف حقل: الاسم العربيّ + SadTypeKind + خانة ABI — بترتيب البنية")
    L.append("            // (EN) Field descriptor: Arabic name + SadTypeKind + ABI slot — struct order")
    L.append("            struct SadEventField")
    L.append("            {")
    L.append("                std::string_view nameUtf8; // (AR) الاسم العربيّ (مفتاح خريطة المفسّر/حقل بنية المترجم)")
    L.append("                std::string_view source;   // (AR) عضو EventData الإنجليزيّ المصدر (أو __type لنوع الحدث)")
    L.append("                SadTypeKind      kind;")
    L.append("                AbiSlot          abi;")
    L.append("            };")
    L.append("")
    L.append(f"            inline constexpr std::array<SadEventField, {n}> SAD_EVENT_FIELDS = {{{{")
    for f in fields:
        nm = hex_escape(f["name"])
        L.append(
            f'                SadEventField{{ "{nm}", "{f["source"]}", SadTypeKind::{f["kind"]}, '
            f'AbiSlot::{_KIND_ABI_SLOT[f["kind"]]} }}, // {f["name"]} ← {f["source"]}'
        )
    L.append("            }};")
    L.append("")
    L.append(f"            inline constexpr int SAD_EVENT_FIELD_COUNT = {n};")
    L.append("")
    # ─── اسم البنية بلغة ص (مصدر الحقيقة: word) — لمزامنة صنف المترجم المضمَّن ───
    L.append("            // (AR) اسم البنية بلغة ص (من types.yaml: word) — يُشتقّ منه المترجم")
    L.append("            //      صنفَ «حدث» المضمَّن فلا يُكتب الاسم حرفيًّا في كود المترجم.")
    L.append("            // (EN) Sad-language struct name (types.yaml: word) — the compiler")
    L.append("            //      derives its built-in event class from this (no literal in code).")
    L.append(
        f'            inline constexpr std::string_view SAD_EVENT_STRUCT_NAME = '
        f'"{hex_escape(event["word"])}"; // {event["word"]}'
    )
    L.append("")

    # ─── تعبئة POD من EventData (وقت التشغيل فقط) خلف حارس ───
    L.append("            // (AR) تعبئة POD من EventData — للتضمين في وقت التشغيل فقط:")
    L.append("            //      عرّف SAD_EVENT_POD_WITH_EVENTDATA وضمّن types.h قبل هذا الرأس.")
    L.append("            // (EN) Fill POD from EventData — runtime-side only: define")
    L.append("            //      SAD_EVENT_POD_WITH_EVENTDATA and include types.h before this header.")
    L.append("#ifdef SAD_EVENT_POD_WITH_EVENTDATA")
    L.append("            inline void sadFillEventPod(const ::sad::ui::EventData &e,")
    L.append("                                        int64_t eventType, SadEventPod &pod)")
    L.append("            {")
    for f in fields:
        src = f["source"]
        member = _pod_member(src)
        if src == _EVENT_TYPE_SOURCE:
            L.append(f"                pod.{member} = eventType;")
        elif f["kind"] == "Boolean":
            L.append(f"                pod.{member} = e.{src} ? 1 : 0;")
        elif f["kind"] == "String":
            L.append(f"                pod.{member} = e.{src}.c_str();")
        elif f["kind"] == "Integer":
            L.append(f"                pod.{member} = static_cast<int64_t>(e.{src});")
        else:  # Float
            L.append(f"                pod.{member} = static_cast<double>(e.{src});")
    L.append("            }")
    L.append("#endif // SAD_EVENT_POD_WITH_EVENTDATA")
    L.append("")
    L.append("        } // namespace EventLayout")
    L.append("    } // namespace Types")
    L.append("} // namespace Sad")
    L.append("")
    return "\n".join(L)


def main() -> int:
    ap = argparse.ArgumentParser(
        description="Generate SadTypeKind enum (C++) from types.yaml (Sad language)."
    )
    ap.add_argument("--yaml",   required=True, type=Path, help="path to types.yaml")
    ap.add_argument("--schema", required=True, type=Path, help="path to type.schema.json")
    ap.add_argument("--header", required=True, type=Path, help="output .h path")
    ap.add_argument("--layout-header", type=Path, default=None,
                    help="output .h path for built-in struct layouts (حدث) — optional")
    ap.add_argument("--quiet",  action="store_true", help="suppress info output")
    args = ap.parse_args()

    try:
        data = load_yaml(args.yaml)
        schema = load_schema(args.schema)

        types = data.get("types")
        if not isinstance(types, list) or not types:
            raise ValueError("types.yaml must contain a non-empty 'types' list")

        # (AR) فحص المخطّط لكل إدخال / (EN) schema-validate each entry
        for i, entry in enumerate(types):
            try:
                validate_schema(entry, schema)
            except ValidationError as e:
                raise ValidationError(
                    f"types[{i}] (id={entry.get('id','?')}): {e.message}"
                ) from e

        # (AR) لا تكرار في kind ولا id / (EN) no duplicate kinds or ids
        validate_no_duplicates(types, key="kind", context="types.yaml kind")
        validate_no_duplicates(types, key="id", context="types.yaml id")

        header_text = emit_header(types, data.get("removed_type_words") or [])
        changed = write_if_changed(args.header, header_text)

        if not args.quiet:
            state = "written" if changed else "unchanged"
            print(f"[gen_types] ✓ {len(types)} نوعًا → {args.header.name} ({state})")

        # (AR) رأس تخطيط البُنى المضمَّنة (حدث) — اختياريّ
        # (EN) Built-in struct layout header (حدث) — optional
        if args.layout_header is not None:
            layout_text = emit_layout_header(types)
            changed2 = write_if_changed(args.layout_header, layout_text)
            if not args.quiet:
                state2 = "written" if changed2 else "unchanged"
                print(f"[gen_types] ✓ تخطيط حدث → {args.layout_header.name} ({state2})")
        return 0

    except (ValidationError, ValueError) as e:
        print(f"[gen_types] FATAL: validation error: {e}", file=sys.stderr)
        return 1
    except Exception as e:  # noqa: BLE001
        print(f"[gen_types] FATAL: {e}", file=sys.stderr)
        traceback.print_exc()
        return 1


if __name__ == "__main__":
    sys.exit(main())
