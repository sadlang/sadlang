#!/usr/bin/env python3
# ═══════════════════════════════════════════════════════════════════════════════════
# ملف: test_runner_normalization.py
# الوصف: اختبار وحدة لمنطق التطبيع والمقارنة في runner.py (ADR-004 / TEST-007)
#
# (AR) يتحقق من أن وضع التطبيع:
#   1. @unordered يجعل الترتيب لا يؤثر (التزامن).
#   2. تساهل عائم بـ epsilon يقبل فروق التمثيل العشري.
#   3. الفروق الحقيقية (مثل لاشيء ≠ رقم) تبقى فشلاً — لا تُقنَّع (BF-09).
#   4. بدون وسوم وبلا فروق عائمة = مقارنة حرفية (لا تغيير سلوكي).
#
# (EN) Unit tests for runner.py normalization/comparison. No external deps (stdlib).
# التشغيل: python tests/unit/testing/test_runner_normalization.py
# ═══════════════════════════════════════════════════════════════════════════════════

import sys
import importlib.util
from pathlib import Path

# (AR) تحميل runner.py كوحدة (يعيش في جذر tests/)
_RUNNER = Path(__file__).resolve().parents[2] / "runner.py"
_spec = importlib.util.spec_from_file_location("sad_runner", _RUNNER)
runner = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(runner)

compare_outputs = runner.compare_outputs
TestMetadata = runner.TestMetadata


def _meta(**kw):
    m = TestMetadata()
    for k, v in kw.items():
        setattr(m, k, v)
    return m


_PASSED = 0
_FAILED = 0


def check(name: str, cond: bool):
    global _PASSED, _FAILED
    if cond:
        _PASSED += 1
        print(f"  ✅ {name}")
    else:
        _FAILED += 1
        print(f"  ❌ {name}")


def main():
    # 1) تطابق حرفي بسيط بلا وسوم
    check("تطابق حرفي", compare_outputs("60\n5\n10", "60\n5\n10", _meta()))

    # 2) اختلاف ترتيب بلا @unordered = فشل (سلوك حرفي محفوظ)
    check("ترتيب مختلف بلا وسم = فشل",
          not compare_outputs("أ\nب\nج", "ج\nب\nأ", _meta()))

    # 3) @unordered يجعل الترتيب لا يؤثر (التزامن)
    check("@unordered يقبل إعادة الترتيب",
          compare_outputs("أ\nب\nج", "ج\nب\nأ", _meta(unordered=True)))

    # 4) @unordered لا يقبل قيمة مفقودة (مجموعتان مختلفتان)
    check("@unordered يرفض فقد قيمة",
          not compare_outputs("أ\nب\nج", "ج\nب", _meta(unordered=True)))

    # 5) تساهل عائم: فرق تمثيل ضئيل يُقبَل
    check("تساهل عائم يقبل 3.0 ≈ 3.0000000001",
          compare_outputs("3.0", "3.0000000001", _meta()))

    # 6) تساهل عائم ضمن نص مختلط (رمز برمز)
    check("تساهل عائم ضمن سطر مختلط",
          compare_outputs("النتيجة 2.5", "النتيجة 2.5000000001", _meta()))

    # 7) فرق رقمي حقيقي يبقى فشلاً (لا يُقنَّع)
    check("فرق رقمي حقيقي = فشل",
          not compare_outputs("3.0", "4.0", _meta()))

    # 8) خطأ مترجم حقيقي (لاشيء ≠ نمط بِتّات) يبقى فشلاً — حالة 058 الفعلية
    check("لاشيء ≠ 4611686018427387904 = فشل (BF-09)",
          not compare_outputs("لاشيء", "4611686018427387904", _meta()))

    # 9) @nondeterministic يستلزم فرز (يقبل إعادة ترتيب)
    check("@nondeterministic يقبل إعادة الترتيب",
          compare_outputs("1\n2\n3", "3\n1\n2", _meta(nondeterministic=True, unordered=True)))

    # 10) عدد أسطر مختلف = فشل دائماً
    check("عدد أسطر مختلف = فشل",
          not compare_outputs("أ\nب", "أ", _meta(unordered=True)))

    print(f"\nالنتيجة: نجح {_PASSED} / فشل {_FAILED}")
    sys.exit(0 if _FAILED == 0 else 1)


if __name__ == "__main__":
    main()
