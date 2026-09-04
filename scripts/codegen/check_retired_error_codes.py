#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارسُ الرموزِ المُتقاعَدة — اتّجاهانِ لا واحد
==============================================================================

(AR) رمزٌ في `language-truth/errors/` يحملُ حقلَ `retired` يعني: **لا يمكنُ أن
     يُرفَعَ بعدَ اليوم**، وقيمةُ الحقلِ هي سببُ ذلك. والحقلُ بلا مستهلكٍ تعليقٌ
     بصيغةِ بيانات لا عقد — فهذا مستهلكُه.

(AR) 🔑 ويحرسُ في **الاتّجاهَين**، لأنّ أحدَهما وحدَه يُنتِجُ عطبًا معاكسًا:

     ① لا باعِث: إن وُجِدَ في شفرةٍ مكتوبةٍ بيدٍ موضعٌ يبعثُ رمزًا مُتقاعَدًا،
        فالتقاعدُ دعوى كاذبة — أو أنّ الحالةَ عادت فيجبُ رفعُ التقاعدِ بقرار.

     ② لا حذف: النصُّ يبقى في الكتالوجِ وفي تعدادِ `error_codes.h`. وحذفُ مدخلٍ
        يبدو «ميّتًا» أسقطَ تشخيصًا من قبلُ في هذا المستودعِ نفسِه، وثنائيٌّ
        قديمٌ أو أثرٌ محفوظٌ قد يحملُ الرمزَ فيصيرُ بلا نصٍّ يُفسّرُه. فالتقاعدُ
        منعُ بعثٍ لا محوُ ذاكرة.

(EN) Retired-error-code guard, in BOTH directions. A `retired` field in the SoT
     means the code can no longer be raised, and the field's value is why. A
     field nobody reads is a comment in data format, not a contract — this is
     its reader.
     ① No emitter: a hand-written site raising a retired code makes the
        retirement a false claim.
     ② No deletion: the text stays in the catalog and in the error_codes.h enum.
        Deleting a seemingly dead entry has degraded a diagnostic in this repo
        before; retirement forbids raising, it does not erase memory.

الاستعمال / Usage:
    python scripts/codegen/check_retired_error_codes.py [--تفصيل]
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

import yaml

REPO_ROOT = Path(__file__).resolve().parents[2]
ERRORS_DIR = REPO_ROOT / "language-truth" / "errors"
ENUM_HEADER = REPO_ROOT / "shared" / "errors" / "include" / "error_codes.h"

# (AR) شجرُ المصادرِ المكتوبةِ بيد. المولَّدُ مستثنًى: هو صدى الكتالوجِ لا باعثًا.
# (EN) Hand-written source trees; generated output is the catalog's echo, not an emitter.
SOURCE_ROOTS = ("compiler", "shared", "tools", "apps")
SOURCE_SUFFIXES = (".cpp", ".h", ".hpp", ".cc")


def read_retired() -> list[dict]:
    """(AR) يقرأ المُتقاعَدين من الكتالوج. / (EN) Reads retired entries from the SoT."""
    retired: list[dict] = []
    for path in sorted(ERRORS_DIR.glob("*.yaml")):
        data = yaml.safe_load(path.read_text(encoding="utf-8")) or {}
        for value in data.values():
            if not isinstance(value, list):
                continue
            for entry in value:
                if isinstance(entry, dict) and entry.get("retired"):
                    retired.append({
                        "id": entry.get("id"),
                        "code": entry.get("code"),
                        "file": path.name,
                        "reason": entry["retired"].get("ar", ""),
                        "since": entry["retired"].get("since", ""),
                    })
    return retired


def source_files() -> list[Path]:
    """(AR) كلُّ مصدرٍ مكتوبٍ بيدٍ خارجَ المولَّدِ وخارجَ شجرِ البناء."""
    files: list[Path] = []
    for root_name in SOURCE_ROOTS:
        root = REPO_ROOT / root_name
        if not root.exists():
            continue
        for path in root.rglob("*"):
            if path.suffix not in SOURCE_SUFFIXES or not path.is_file():
                continue
            parts = set(path.parts)
            if "generated" in parts or "build" in parts:
                continue
            files.append(path)
    return files


def main() -> int:
    parser = argparse.ArgumentParser(
        description="حارسُ رموزِ الأخطاءِ المُتقاعَدة")
    parser.add_argument("--تفصيل", dest="verbose", action="store_true")
    args = parser.parse_args()

    retired = read_retired()
    # (AR) العددُ يُطبَعُ دائمًا: حارسٌ يصمتُ عن نطاقِه يُقرأُ «غطّى الكلَّ».
    # (EN) Always print the scope; a guard silent about its scope reads as total.
    print(f"🔍 رموزٌ مُتقاعَدة في مصدرِ الحقيقة: {len(retired)}")
    if not retired:
        print("✓ لا رمزَ مُتقاعَدًا — لا شيءَ يُحرَس.")
        return 0

    for item in retired:
        print(f"   • {item['id']} ({item['code']}) — مُذْ {item['since']}")
        if args.verbose:
            print(f"     {item['reason']}")

    if not ENUM_HEADER.exists():
        print(f"❌ تعدادُ الرموزِ مفقود: {ENUM_HEADER}")
        return 2
    enum_text = ENUM_HEADER.read_text(encoding="utf-8")

    failures: list[str] = []

    # ── ② لا حذف: النصُّ والتعدادُ يبقيان ────────────────────────────────────
    for item in retired:
        if item["code"] and item["code"] not in enum_text:
            failures.append(
                f"{item['id']}: الرمزُ المُتقاعَد «{item['code']}» غابَ عن "
                f"{ENUM_HEADER.name} — التقاعُدُ منعُ بعثٍ لا محوُ نصّ. "
                f"وثنائيٌّ قديمٌ يحملُ الرمزَ يصيرُ بلا تشخيصٍ يُفسّرُه.")

    # ── ① لا باعِث ───────────────────────────────────────────────────────────
    symbols = {item["code"]: item for item in retired if item["code"]}
    patterns = {
        code: re.compile(r"\b" + re.escape(code) + r"\b") for code in symbols
    }
    for path in source_files():
        try:
            text = path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        for code, pattern in patterns.items():
            for match_line in pattern.finditer(text):
                line_no = text.count("\n", 0, match_line.start()) + 1
                # (AR) سطرُ التعريفِ في التعدادِ نفسِه ليس بعثًا.
                if path == ENUM_HEADER:
                    continue
                rel = path.relative_to(REPO_ROOT).as_posix()
                failures.append(
                    f"{symbols[code]['id']}: باعثٌ لرمزٍ مُتقاعَد في "
                    f"{rel}:{line_no} — إمّا أنّ التقاعُدَ دعوى كاذبة، وإمّا أنّ "
                    f"الحالةَ عادت فيُرفَعُ التقاعُدُ بقرارٍ مُدوَّن.")

    if failures:
        print("")
        print("❌ حارسُ الرموزِ المُتقاعَدة:")
        for line in failures:
            print(f"   • {line}")
        return 1

    print("")
    print(f"✓ {len(retired)} رمزًا مُتقاعَدًا: لا باعثَ لأيٍّ منها، ونصوصُها باقية.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
