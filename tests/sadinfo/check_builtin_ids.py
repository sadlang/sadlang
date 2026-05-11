#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
بسم الله الرحمن الرحيم
═══════════════════════════════════════════════════════════════════════════════
ملف: tests/sadinfo/check_builtin_ids.py
(AR) اختبار snapshot ذهبي لـ --dump-builtins:
     - يُشغّل sadinfo --dump-builtins، يجمع functions[].id + methods[].id
       (بترتيب الإدراج الأصلي)، ويقارنها بالملف الذهبي.
     - أي إعادة ترتيب أو إضافة/حذف غير مقصودة تُكسر الاختبار صراحة (BF-22).

(EN) Golden snapshot test for --dump-builtins: runs sadinfo, collects ids of
     functions then methods (in declaration order), compares to baseline.
═══════════════════════════════════════════════════════════════════════════════

الاستخدام / Usage:
    python check_builtin_ids.py <sadinfo.exe> <golden_ids.json>
"""

import json
import subprocess
import sys
import tempfile
from pathlib import Path


def main() -> int:
    if len(sys.argv) != 3:
        print(f"الاستخدام / usage: {sys.argv[0]} <sadinfo.exe> <golden_ids.json>",
              file=sys.stderr)
        return 2

    sadinfo_exe = Path(sys.argv[1])
    golden_path = Path(sys.argv[2])

    if not sadinfo_exe.exists():
        print(f"[FAIL] sadinfo executable not found: {sadinfo_exe}", file=sys.stderr)
        return 1
    if not golden_path.exists():
        print(f"[FAIL] golden file not found: {golden_path}", file=sys.stderr)
        return 1

    with tempfile.NamedTemporaryFile(suffix=".json", delete=False) as tmp:
        tmp_path = Path(tmp.name)
    try:
        result = subprocess.run(
            [str(sadinfo_exe), "--dump-builtins", "-o", str(tmp_path)],
            capture_output=True, text=True, encoding="utf-8"
        )
        if result.returncode != 0:
            print(f"[FAIL] sadinfo exit code = {result.returncode}", file=sys.stderr)
            print(f"stderr: {result.stderr}", file=sys.stderr)
            return 1

        with tmp_path.open(encoding="utf-8") as f:
            actual_data = json.load(f)
        actual_ids = ([fn["id"] for fn in actual_data["functions"]]
                      + [m["id"] for m in actual_data["methods"]])

        with golden_path.open(encoding="utf-8") as f:
            golden_ids = json.load(f)

        if actual_ids == golden_ids:
            print(f"[PASS] {len(actual_ids)} builtin ids match golden snapshot "
                  f"(functions={actual_data['functionCount']}, "
                  f"methods={actual_data['methodCount']})")
            return 0

        print("[FAIL] builtin ids mismatch with golden snapshot", file=sys.stderr)
        print(f"  expected count = {len(golden_ids)}", file=sys.stderr)
        print(f"  actual   count = {len(actual_ids)}", file=sys.stderr)

        actual_set = set(actual_ids)
        golden_set = set(golden_ids)
        added = sorted(actual_set - golden_set)
        removed = sorted(golden_set - actual_set)
        if added:
            print(f"  [+] added (in actual, not in golden): {added}", file=sys.stderr)
        if removed:
            print(f"  [-] removed (in golden, not in actual): {removed}", file=sys.stderr)
        if not added and not removed:
            print("  ids set equal but order differs — first divergence:", file=sys.stderr)
            for i, (a, g) in enumerate(zip(actual_ids, golden_ids)):
                if a != g:
                    print(f"    index {i}: actual={a!r} vs golden={g!r}", file=sys.stderr)
                    break
        return 1
    finally:
        try:
            tmp_path.unlink()
        except OSError:
            pass


if __name__ == "__main__":
    sys.exit(main())
