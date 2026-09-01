#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ «إعلاناتُ المدمَجاتِ تُطابقُ مخطَّطَها، والمخطَّطُ يستطيعُ أن يحمرَّ».

     🔑 لم يكن `builtin_function.schema.json` حارسًا قطّ — كان قائمةً
        **للكتابةِ فقط**. سكربتا هجرةٍ (`gen_compiler_builtins_yaml.py:83-88`
        و`gen_kernel_yaml.py`) يُلحِقانِ بـ`enum` المجموعاتِ ما يُضيفانه، ولا
        أحدَ يقرؤها بعدَ ذلك. فبقيَ فيها ٤٢ قيمةً بينما يستعملُ مصدرُ الحقيقةِ
        ٥٤: **٣٥٥ إعلانًا (٢٩٫٥٪) في ١٧ مجموعةً خارجَ القائمة** — منها
        `UIWidgets` بـ١١٢ و`UIPlatform` بـ٤٣ — **وخمسُ قيمٍ ميّتةٍ** فيها
        (`Array` `IO` `Map` `String` `System`) لا يستعملُها أحد.
        مخطَّطٌ لا يُقرَأُ ليس حدًّا، بل زينةٌ تُقرَأُ حدًّا.

     ⚠️ والاتّجاهان مقصودان:
        ① **مستعمَلٌ وغيرُ مُدرَج** ⇒ مجموعةٌ دخلت بلا قرار. القائمةُ مغلقةٌ
           عمدًا: إضافةُ مجموعةٍ قرارٌ يُكتَبُ في المخطَّطِ صراحةً.
        ② **مُدرَجٌ وغيرُ مستعمَل** ⇒ قيمةٌ ميّتةٌ توسّعُ القبولَ بلا مقابل،
           فتمرُّ بها أخطاءُ الكتابةِ صامتةً. حارسٌ أحاديُّ الاتّجاهِ يسمحُ
           للقائمةِ أن تنتفخَ حتّى تقبلَ كلَّ شيء.

     ولا يُشتَقُّ الـ`enum` من الاستعمالِ في كلِّ تشغيل: طرفانِ من أصلٍ واحدٍ
     لا يكشفانِ شيئًا. المخطَّطُ **مُودَعٌ ومقروء**، والاستعمالُ يُقاسُ عليه.

     العلاج عند الاحمرار: حرّر `language-truth/_schemas/builtin_function.schema.json`
     عمدًا — أو صحّح `namespace` في ملفّ المدمَج.

(EN) Validates every builtin declaration against its schema, and guards the
     namespace enum in both directions: used-but-unlisted and listed-but-unused.
     The schema was append-only and never read; 355 declarations sat outside it.
============================================================================
"""
from __future__ import annotations

import json
import sys
from collections import Counter
from pathlib import Path

import yaml

# (AR) الطرفيّةُ قد تكون بترميزٍ غيرِ UTF-8، فتنهار رسالةُ النجاحِ العربيّةُ بـ
#      UnicodeEncodeError ويُبلَّغ فشلٌ كاذب. نظيرُ ما تفعله بقيّةُ الحرّاس.
for _stream in (sys.stdout, sys.stderr):
    if hasattr(_stream, "reconfigure"):
        _stream.reconfigure(encoding="utf-8")

ROOT = Path(__file__).resolve().parents[2]
SOT_DIR = ROOT / "language-truth" / "builtins"
SCHEMA = ROOT / "language-truth" / "_schemas" / "builtin_function.schema.json"
TYPES = ROOT / "language-truth" / "types.yaml"

# (AR) سقفٌ مُعلَنٌ للمخالفاتِ البنيويّةِ (غيرِ المجموعات) — صفرٌ اليوم، ويبقى
#      صفرًا: أُصلحت قبلَ إدخالِ الحارس، فلا دَينَ يُورَّث.
STRUCTURAL_CEILING = 0


def surface_type_words() -> set:
    """ألفاظُ الأنواعِ السطحيّةُ من مصدرِ حقيقةِ الأنواع — لا قائمةٌ ثانيةٌ هنا.

    المخطَّطُ وحدَه لا يستطيعُ ربطَ ملفَّين، فيُقاسُ الربطُ هنا: نوعُ المعاملِ
    المبنيِّ لفظُ نوعٍ سطحيٌّ قانونيّ، لا نصٌّ حرٌّ يشبهُه.
    """
    doc = yaml.safe_load(TYPES.read_text(encoding="utf-8")) or {}
    return {t["word"] for t in doc.get("types") or []
            if t.get("surface") and t.get("word")}


def load_entries():
    """(مسار الملفّ، الإعلان) لكلِّ مدمَجٍ في مصدرِ الحقيقة."""
    for path in sorted(SOT_DIR.glob("*.yaml")):
        doc = yaml.safe_load(path.read_text(encoding="utf-8")) or {}
        for fn in doc.get("functions") or []:
            yield path.name, fn


def main() -> int:
    if not SCHEMA.exists():
        print(f"✗ المخطَّطُ غير موجود: {SCHEMA}")
        return 1
    schema = json.loads(SCHEMA.read_text(encoding="utf-8"))
    props = schema.get("properties") or {}
    allowed = set((props.get("namespace") or {}).get("enum") or [])
    if not allowed:
        print("✗ `enum` المجموعاتِ فارغٌ أو غائبٌ — حارسٌ بلا مرجعٍ لا يحرس.")
        return 1

    entries = list(load_entries())
    used = Counter(fn.get("namespace") for _, fn in entries)

    print("حارس «إعلاناتُ المدمَجاتِ تُطابقُ مخطَّطَها»:")
    print(f"  المخطَّط: {SCHEMA.relative_to(ROOT)}")
    print(f"  إعلانات: {len(entries)} · مجموعاتٌ مسموحة: {len(allowed)} · "
          f"مستعمَلة: {len(used)}")

    failed = False

    # ① مستعمَلٌ وغيرُ مُدرَج
    unlisted = {ns: n for ns, n in used.items() if ns not in allowed}
    if unlisted:
        total = sum(unlisted.values())
        print(f"  ✗ مجموعةٌ مستعمَلةٌ وغيرُ مُدرَجةٍ في المخطَّط: {len(unlisted)} "
              f"({total} إعلانًا) — دخلت بلا قرار:")
        for ns, n in sorted(unlisted.items(), key=lambda kv: -kv[1])[:15]:
            print(f"      · {ns} ({n})")
        failed = True

    # ② مُدرَجٌ وغيرُ مستعمَل
    dead = sorted(allowed - set(used))
    if dead:
        print(f"  ✗ قيمةٌ مُدرَجةٌ وغيرُ مستعمَلة: {len(dead)} — تُوسّعُ القبولَ "
              "بلا مقابلٍ فتمرُّ بها أخطاءُ الكتابة:")
        for ns in dead[:15]:
            print(f"      · {ns}")
        failed = True

    # ③ المطابقةُ البنيويّةُ لكلِّ إعلان
    try:
        from jsonschema import Draft7Validator
    except ImportError:
        print("  ✗ حزمةُ jsonschema غائبةٌ — لا يُدَّعى تحقّقٌ لم يجرِ.")
        return 1
    validator = Draft7Validator(schema)
    problems = []
    for filename, fn in entries:
        for err in validator.iter_errors(fn):
            # المجموعاتُ محسوبةٌ أعلاه بتفصيلها؛ لا تُعَدُّ مرّتين.
            if list(err.absolute_path)[:1] == ["namespace"]:
                continue
            problems.append((filename, fn.get("canonical", "؟"),
                             "/".join(str(p) for p in err.absolute_path) or "—",
                             err.message[:90]))
    if len(problems) > STRUCTURAL_CEILING:
        print(f"  ✗ مخالفاتٌ بنيويّةٌ للمخطَّط: {len(problems)} "
              f"(السقفُ المُعلَن {STRUCTURAL_CEILING}):")
        for filename, name, where, why in problems[:15]:
            print(f"      · {filename} :: {name} [{where}] — {why}")
        failed = True

    # ④ نوعُ المعاملِ المبنيِّ لفظُ نوعٍ سطحيٌّ من types.yaml
    words = surface_type_words()
    if not words:
        print("  ✗ تعذّر اشتقاقُ ألفاظِ الأنواعِ من types.yaml — لا يُدَّعى ربطٌ لم يجرِ.")
        failed = True
    else:
        stray, structured = [], 0
        for filename, fn in entries:
            for param in fn.get("params") or []:
                if not isinstance(param, dict):
                    continue
                structured += 1
                if param.get("type") not in words:
                    stray.append((filename, fn.get("canonical", "؟"),
                                  param.get("name"), param.get("type")))
        print(f"  معاملاتٌ مبنيّةٌ (اسمٌ ونوعٌ مقروءانِ آليًّا): {structured} "
              f"من {sum(len(fn.get('params') or []) for _, fn in entries)}")
        if stray:
            print(f"  ✗ نوعُ معاملٍ ليس لفظَ نوعٍ سطحيًّا في types.yaml: {len(stray)}")
            for filename, name, pname, ptype in stray[:15]:
                print(f"      · {filename} :: {name}({pname}) — «{ptype}»")
            failed = True

    if failed:
        print("\n  العلاج: صحّح `namespace` في ملفّ المدمَج، أو حرّر المخطَّطَ")
        print("  عمدًا إن كانت المجموعةُ الجديدةُ قرارًا مقصودًا:")
        print(f"    {SCHEMA.relative_to(ROOT)}")
        return 1

    print(f"  ✓ سليم — كلُّ إعلانٍ يطابقُ المخطَّط، ولا مجموعةَ خارجَ القائمة، "
          "ولا قيمةَ ميّتةً فيها.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
