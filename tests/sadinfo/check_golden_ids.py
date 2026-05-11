#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
بسم الله الرحمن الرحيم
═══════════════════════════════════════════════════════════════════════════════
ملف: tests/sadinfo/check_golden_ids.py
(AR) اختبار snapshot ذهبي: يُشغّل sadinfo --dump-keywords --include-deprecated،
     يستخرج قائمة ids، ويقارنها بالملف الذهبي المُسجَّل. أي اختلاف يفشل البناء.

     هذا يضمن أن أي تغيير على english في keywords.yaml يكسر اختبار CI صراحة،
     لتجنب تعديل الـ id بصمت (BF-22 — اختبار الحالة السلبية).

(EN) Golden snapshot test: runs sadinfo and compares ids to recorded baseline.
     Ensures any english field change in keywords.yaml visibly breaks CI.
═══════════════════════════════════════════════════════════════════════════════

الاستخدام / Usage:
    python check_golden_ids.py <sadinfo.exe> <golden.json>
"""

import json
import subprocess
import sys
import tempfile
from pathlib import Path


def main() -> int:
    if len(sys.argv) != 3:
        print(f"الاستخدام / usage: {sys.argv[0]} <sadinfo.exe> <golden.json>",
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

    # (AR) شغّل sadinfo بكل الكلمات (شامل المُهمَلة) إلى ملف مؤقت
    # (EN) Run sadinfo with all keywords (including deprecated) to a temp file
    with tempfile.NamedTemporaryFile(suffix=".json", delete=False) as tmp:
        tmp_path = Path(tmp.name)
    try:
        result = subprocess.run(
            [str(sadinfo_exe), "--dump-keywords",
             "--include-deprecated", "-o", str(tmp_path)],
            capture_output=True, text=True, encoding="utf-8"
        )
        if result.returncode != 0:
            print(f"[FAIL] sadinfo exit code = {result.returncode}", file=sys.stderr)
            print(f"stderr: {result.stderr}", file=sys.stderr)
            return 1

        # (AR) قراءة المخرج واستخراج ids
        with tmp_path.open(encoding="utf-8") as f:
            actual_data = json.load(f)
        actual_ids = [k["id"] for k in actual_data["keywords"]]

        # (AR) قراءة الملف الذهبي
        with golden_path.open(encoding="utf-8") as f:
            golden_data = json.load(f)
        golden_ids = golden_data["ids"]

        # (AR) المقارنة الكاملة
        if actual_ids == golden_ids:
            print(f"[PASS] {len(actual_ids)} ids match golden snapshot")
            return 0

        # (AR) فشل — أظهر الفروقات
        print("[FAIL] ids mismatch with golden snapshot", file=sys.stderr)
        print(f"  expected count = {len(golden_ids)}", file=sys.stderr)
        print(f"  actual   count = {len(actual_ids)}", file=sys.stderr)

        actual_set = set(actual_ids)
        golden_set = set(golden_ids)
        added   = sorted(actual_set - golden_set)
        removed = sorted(golden_set - actual_set)
        if added:
            print(f"  +added ({len(added)}): {added[:10]}{'...' if len(added) > 10 else ''}",
                  file=sys.stderr)
        if removed:
            print(f"  -removed ({len(removed)}): {removed[:10]}{'...' if len(removed) > 10 else ''}",
                  file=sys.stderr)

        # (AR) فحص ترتيب فقط لو نفس المجموعة
        if actual_set == golden_set and actual_ids != golden_ids:
            print("  [INFO] same ids but different order", file=sys.stderr)
        return 1
    finally:
        try:
            tmp_path.unlink()
        except OSError:
            pass


if __name__ == "__main__":
    sys.exit(main())
