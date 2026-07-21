#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارس تغطية معجم آبلتات صدَفة ص (sad-repl) — يثبّت «المقام المرجعيّ» فيصير
     ادّعاء التغطية قابلًا للقياس (X/إجماليّ) بدل تقدير ظنّيّ. يقابل جذرَين:
       1. language-truth/tools/repl/applets.yaml         — معجم exec ↔ اسم عربيّ.
       2. language-truth/tools/repl/busybox_reference.yaml — قائمة busybox القطعيّة
          (defconfig) التي يبنيها نظام ص السياديّ + آبلتات مسموحة خارج المقام.
     يفشل بصوتٍ عالٍ إن:
       • exec في المعجم ليس آبلت defconfig ولا في القائمة المسموحة (extra_allowed)
         ⇒ يمسك اسمًا ميّتًا لن يُحَلّ أبدًا على sad-os (نظير static-sh التاريخيّ).
       • آبلت defconfig بلا اسم عربيّ في المعجم (فجوة تغطية) ⇒ يفرض 403/403.
       • exec مكرَّر في المعجم.
     ملاحظة: المرجع مثبَّت بإصدار busybox + بصمة sha256؛ ترقية الإصدار تُلزم إعادة
     التقاط القائمة وسدّ أيّ فجوة جديدة — وهو المقصود (يحفظ صدق ادّعاء الاكتمال).

(EN) Coverage guard for the sad-repl Arabic applet lexicon. Pins the reference
     denominator (busybox defconfig applet set that sad-os builds) so coverage is
     a measured X/total, not an estimate. Fails on: an exec that is neither a
     defconfig applet nor in extra_allowed (a dead name), an uncovered defconfig
     applet (a gap), or a duplicate exec.
============================================================================
"""
import io
import sys
from pathlib import Path

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8")

try:
    import yaml
except ImportError:
    print("[check_applet_coverage] FATAL: pyyaml not installed", file=sys.stderr)
    sys.exit(1)

_codegen_dir = str(Path(__file__).parent)
if _codegen_dir not in sys.path:
    sys.path.insert(0, _codegen_dir)
from _lib.emit import normalize_arabic  # noqa: E402

_ROOT = Path(__file__).parents[2]
_APPLETS = _ROOT / "language-truth" / "tools" / "repl" / "applets.yaml"
_REFERENCE = _ROOT / "language-truth" / "tools" / "repl" / "busybox_reference.yaml"


def main() -> int:
    errors: list[str] = []

    if not _REFERENCE.exists():
        print(
            f"[check_applet_coverage] FATAL: ملفّ المرجع مفقود {_REFERENCE} "
            f"— أعد التقاطه عبر `busybox --list` (make defconfig).",
            file=sys.stderr,
        )
        return 1
    if not _APPLETS.exists():
        print(f"[check_applet_coverage] FATAL: معجم الآبلتات مفقود {_APPLETS}", file=sys.stderr)
        return 1

    ref = yaml.safe_load(_REFERENCE.read_text(encoding="utf-8"))
    defconfig = set(ref.get("defconfig_applets", []))
    extra_allowed = set(ref.get("extra_allowed", []))
    version = ref.get("busybox_version", "?")
    if not defconfig:
        print("[check_applet_coverage] FATAL: no defconfig_applets in busybox_reference.yaml", file=sys.stderr)
        return 1

    data = yaml.safe_load(_APPLETS.read_text(encoding="utf-8"))
    execs: list[str] = []
    arabics: list[str] = []
    for entry in data.get("applets", []):
        ex = entry.get("exec")
        if ex is not None:
            execs.append(ex)
        ar = entry.get("arabic")
        if ar is not None:
            arabics.append(ar)

    # 1) لا exec مكرَّر
    seen: set[str] = set()
    for ex in execs:
        if ex in seen:
            errors.append(f"exec مكرَّر في applets.yaml: «{ex}»")
        seen.add(ex)
    exec_set = seen

    # 2) لا exec ميّت (ليس آبلت defconfig ولا في extra_allowed)
    allowed = defconfig | extra_allowed
    for ex in sorted(exec_set):
        if ex not in allowed:
            errors.append(
                f"exec «{ex}» ليس آبلت busybox {version} defconfig ولا في extra_allowed "
                f"— اسم ميّت لن يُحَلّ على sad-os (صحّح الإملاء أو أدرِجه في busybox_reference.yaml)"
            )

    # 3) لا فجوة تغطية: كلّ آبلت defconfig له اسم عربيّ
    gaps = sorted(defconfig - exec_set)
    for g in gaps:
        errors.append(
            f"آبلت defconfig «{g}» بلا اسم عربيّ في applets.yaml (فجوة تغطية — عرّبه أو برّر استثناءه)"
        )

    # 4) لا تصادم تحت التطبيع: اسمان عربيّان مختلفان لا يجوز أن يُطبَّعا لصيغة واحدة،
    #    وإلّا صارت المطابقة المتسامحة في appletExec ملتبسةً (أيّ exec يُختار؟). يحرس
    #    normalize_arabic (L2) المشترك مع المولّد C++؛ إضافة اسم يخرق هذا تُفشل CI.
    norm_buckets: dict[str, list[str]] = {}
    for ar in arabics:
        norm_buckets.setdefault(normalize_arabic(ar), []).append(ar)
    for norm, names in sorted(norm_buckets.items()):
        uniq = sorted(set(names))
        if len(uniq) > 1:
            errors.append(
                f"تصادم تطبيع: الأسماء {uniq} تُطبَّع كلّها إلى «{norm}» — "
                f"المطابقة المتسامحة في appletExec ستلتبس (غيّر اسمًا كي يتفرّد تحت L2)"
            )

    if errors:
        print(f"✗ فشل حارس تغطية آبلتات sad-repl (المرجع: busybox {version} defconfig):")
        for e in errors:
            print(f"  - {e}")
        return 1

    covered = len(defconfig & exec_set)
    n_extra = len(exec_set & extra_allowed)
    n_norm = len({normalize_arabic(a) for a in arabics})
    print(
        f"✓ تغطية آبلتات sad-repl كاملة: {covered}/{len(defconfig)} من مقام busybox {version} "
        f"defconfig (100٪)، +{n_extra} آبلت مسموح خارج المقام، صفر اسم ميّت، صفر تكرار، "
        f"{n_norm} صيغة مطبَّعة متفرّدة (صفر تصادم L2)."
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
