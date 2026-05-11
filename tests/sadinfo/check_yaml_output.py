#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
بسم الله الرحمن الرحيم
═══════════════════════════════════════════════════════════════════════════════
ملف: tests/sadinfo/check_yaml_output.py
(AR) يتحقق أن مخرج `sadinfo --format yaml` صالح YAML بنيوياً، ويحوي
     الحقول الجذرية المتوقعة بالقيم الصحيحة. يفشل CI عند:
       - YAML غير صالح (parse error)
       - حقول جذرية مفقودة
       - عدم تطابق ids مع snapshot JSON الذهبي

(EN) Validates `sadinfo --format yaml` output: parses with PyYAML, checks
     root fields, and ensures ids match the JSON golden snapshot.
═══════════════════════════════════════════════════════════════════════════════
"""

import json
import subprocess
import sys
import tempfile
from pathlib import Path

try:
    import yaml
except ImportError:
    print("[SKIP] PyYAML not installed — skipping YAML validation test")
    sys.exit(0)  # (AR) لا نفشل CI عند غياب التبعية الاختيارية


def main() -> int:
    if len(sys.argv) != 3:
        print(f"الاستخدام / usage: {sys.argv[0]} <sadinfo.exe> <golden.json>",
              file=sys.stderr)
        return 2

    sadinfo_exe = Path(sys.argv[1])
    golden_path = Path(sys.argv[2])

    if not sadinfo_exe.exists():
        print(f"[FAIL] sadinfo not found: {sadinfo_exe}", file=sys.stderr)
        return 1

    # (AR) شغّل sadinfo بصيغة YAML شاملة المُهملة
    with tempfile.NamedTemporaryFile(suffix=".yaml", delete=False) as tmp:
        tmp_path = Path(tmp.name)
    try:
        result = subprocess.run(
            [str(sadinfo_exe), "--dump-keywords",
             "--include-deprecated", "--format", "yaml",
             "-o", str(tmp_path)],
            capture_output=True, text=True, encoding="utf-8"
        )
        if result.returncode != 0:
            print(f"[FAIL] sadinfo exit={result.returncode}", file=sys.stderr)
            print(f"  stderr: {result.stderr}", file=sys.stderr)
            return 1

        # (AR) parse YAML
        try:
            with tmp_path.open(encoding="utf-8") as f:
                data = yaml.safe_load(f)
        except yaml.YAMLError as ex:
            print(f"[FAIL] YAML parse error: {ex}", file=sys.stderr)
            return 1

        if not isinstance(data, dict):
            print("[FAIL] root is not a mapping", file=sys.stderr)
            return 1

        # (AR) فحص الحقول الجذرية المتوقعة
        required_root = ["schemaVersion", "tool", "toolVersion",
                         "totalCount", "categoryCounts", "keywords"]
        for k in required_root:
            if k not in data:
                print(f"[FAIL] missing root field: {k}", file=sys.stderr)
                return 1

        if data["tool"] != "sadinfo":
            print(f"[FAIL] tool != 'sadinfo' (got {data['tool']!r})",
                  file=sys.stderr)
            return 1

        if data["totalCount"] != len(data["keywords"]):
            print(f"[FAIL] totalCount ({data['totalCount']}) != "
                  f"len(keywords) ({len(data['keywords'])})",
                  file=sys.stderr)
            return 1

        # (AR) فحص حقول الـ entry الأول
        first = data["keywords"][0]
        for f in ("id", "word", "english", "category",
                  "tokenType", "emittedByLexer"):
            if f not in first:
                print(f"[FAIL] first entry missing field: {f}", file=sys.stderr)
                return 1

        # (AR) قارن ids مع snapshot الذهبي
        with golden_path.open(encoding="utf-8") as f:
            golden = json.load(f)
        actual_ids = [k["id"] for k in data["keywords"]]
        if actual_ids != golden["ids"]:
            print("[FAIL] YAML ids differ from golden JSON snapshot",
                  file=sys.stderr)
            print(f"  expected count = {len(golden['ids'])}", file=sys.stderr)
            print(f"  actual   count = {len(actual_ids)}", file=sys.stderr)
            return 1

        print(f"[PASS] YAML valid, totalCount={data['totalCount']}, "
              f"all {len(actual_ids)} ids match golden snapshot")
        return 0
    finally:
        try:
            tmp_path.unlink()
        except OSError:
            pass


if __name__ == "__main__":
    sys.exit(main())
