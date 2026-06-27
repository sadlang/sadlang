#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارس طبقات أهداف البناء — RFC sadlang-rfcs#10 (المرحلة 3، بوّابة G4).

    الخلفية: monorepo لغة ص يقسّم أهداف CMake إلى ثلاث طبقات (انظر
    docs/architecture/cmake-target-boundaries.md):
        ① الأساس المشترك    : sad_shared (نواة اللغة الحقيقيّة).
        ½ الحزام المشترك     : sad_type_system / sad_semantic / sad_memory_* /
                              sad_security_core / sad_null_safety / sad_mobile.
        ② نظام المفسّر فقط   : sad_interp(=sad_core) / sad_builtins / sad_lowlevel /
                              sad_ui_bridge / sad_ui / sad_network / sad_http /
                              sad_websocket / sadnet / sad_rt_runtime / sad_profiler_lib.
        ③ نظام المترجم فقط   : sad_compiler / sad_frontend / sad_optimizer /
                              sad_llvm_backend / sad_tools / sad_ui_ir /
                              sad_abstraction / sad_security.
        التنفيذيّان (L2)     : sad-run (مفسّر) / sad-build (مترجم).

    اللامتغيِّر المفروض (G4 — «لا روابط مباشرة بين المحرّكَين»):
        • لا يربط هدفُ نظامِ المفسّر أيَّ هدف من نظام المترجم، والعكس.
        • sad-run لا يربط أيَّ هدف من نظام المترجم.
        • sad-build لا يربط أيَّ هدف من نظام المفسّر.
        • الأساس sad_shared لا يربط أيَّ هدف من المحرّكَين (يبقى قاعدةً نقيّة).

    الحدّ نظيفٌ اليوم بالممارسة، فالحارس **وقائيّ لا تصحيحيّ**: يثبّت الوضع
    السليم ويفشل إن تسلّل رابطٌ عابرٌ للمحرّكَين. شقيقه على مستوى التضمين هو
    check_interpreter_boundary.py؛ هذا يعمل على مستوى رسم الربط في CMake.

(EN) CMake target-layering guard. Parses every `target_link_libraries(...)` across
     the repo's CMake sources, classifies each known `sad_*` target into a layer,
     and fails if any edge crosses the interpreter↔compiler engine boundary, or if
     the shared foundation links upward into an engine. Preventive: holds today.

    المرجع: docs/architecture/cmake-target-boundaries.md §3-§6.
"""
import os
import re
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except (AttributeError, ValueError):
    pass

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# (AR) أسماء بديلة (alias) تُطبَّع إلى الهدف الفعليّ قبل التصنيف.
# (EN) Alias names normalized to their real target before classification.
ALIASES = {
    "sad_core": "sad_interp",       # alias توافق (libraries.cmake)
    "sad_semantic": "sad_ownership",  # sad_semantic = alias لـ sad_ownership
    "sad_formatter": "sad_formatter_lib",
}

# (AR) تصنيف الطبقات. كلُّ هدفٍ غير مذكور هنا (مكتبات نظام/خارجيّة/أدوات
#      مستقلّة) يُتجاهَل في فحص العبور بين المحرّكَين.
FOUNDATION = {"sad_shared"}
BAND = {
    "sad_type_system", "sad_ownership", "sad_memory_gc", "sad_memory_policy",
    "sad_security_core", "sad_null_safety", "sad_mobile",
}
INTERP = {
    "sad_interp", "sad_builtins", "sad_lowlevel", "sad_ui_bridge", "sad_ui",
    "sad_network", "sad_http", "sad_websocket", "sadnet", "sad_rt_runtime",
    "sad_profiler_lib",
}
COMPILER = {
    "sad_compiler", "sad_frontend", "sad_optimizer", "sad_llvm_backend",
    "sad_tools", "sad_ui_ir", "sad_abstraction", "sad_security",
}
EXE_INTERP = {"sad-run"}
EXE_COMPILER = {"sad-build"}

# (AR) الكلمات المفتاحيّة في target_link_libraries — ليست تبعيّات.
KEYWORDS = {"PUBLIC", "PRIVATE", "INTERFACE", "LINK_PUBLIC", "LINK_PRIVATE"}

# (AR) جذور تُستثنى من المسح (مخرجات بناء/تاريخيّ/توزيع/طرف ثالث).
SKIP_DIRS = {
    "build", "build-debug", "build-release", "out", "dist", ".git",
    "archived", "distribution", "_scratch", "third_party", "vcpkg",
    "vcpkg_installed", "node_modules",
}

TLL_RE = re.compile(r"target_link_libraries\s*\(", re.IGNORECASE)


def layer_of(target):
    """يرجع وسم الطبقة لهدفٍ مطبَّع، أو None إن كان مجهولًا/خارجيًّا."""
    if target in FOUNDATION:
        return "foundation"
    if target in BAND:
        return "band"
    if target in INTERP:
        return "interp"
    if target in COMPILER:
        return "compiler"
    if target in EXE_INTERP:
        return "exe-interp"
    if target in EXE_COMPILER:
        return "exe-compiler"
    return None


def normalize(token):
    """ينظّف اسم هدف من علامات الاقتباس ويطبّق الـalias."""
    t = token.strip().strip('"').strip("'")
    return ALIASES.get(t, t)


def is_real_dep(token):
    """يستبعد المتغيّرات والمولّدات والمكتبات الخارجيّة المعرَّفة بالـnamespace."""
    if not token:
        return False
    if token.startswith("${") or token.startswith("$<") or token.startswith("$("):
        return False
    if "::" in token:           # OpenSSL::SSL وأمثاله
        return False
    return True


def iter_cmake_files():
    for dp, dirs, files in os.walk(ROOT):
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
        for f in files:
            if f == "CMakeLists.txt" or f.endswith(".cmake"):
                yield os.path.join(dp, f)


def parse_links(path):
    """يستخرج (الهدف, التبعيّة, السطر) من كلّ target_link_libraries في ملفّ.

    يدعم النداءات متعدّدة الأسطر حتّى القوس المغلق المقابل.
    """
    edges = []
    try:
        with open(path, encoding="utf-8", errors="replace") as fh:
            text = fh.read()
    except OSError:
        return edges
    lines = text.splitlines()
    i = 0
    n = len(lines)
    while i < n:
        if not TLL_RE.search(lines[i]):
            i += 1
            continue
        start_line = i + 1
        # (AR) اجمع النداء كاملًا (موازنة الأقواس) ابتداءً من «(».
        buf = lines[i][lines[i].lower().index("target_link_libraries"):]
        depth = buf.count("(") - buf.count(")")
        while depth > 0 and i + 1 < n:
            i += 1
            buf += " " + lines[i]
            depth += lines[i].count("(") - lines[i].count(")")
        inner = buf[buf.index("(") + 1: buf.rindex(")") if ")" in buf else len(buf)]
        # (AR) أزل التعليقات داخل النداء.
        inner = re.sub(r"#.*", "", inner)
        toks = inner.replace("\t", " ").split()
        if not toks:
            i += 1
            continue
        target = normalize(toks[0])
        for raw in toks[1:]:
            if raw in KEYWORDS:
                continue
            if not is_real_dep(raw):
                continue
            dep = normalize(raw)
            edges.append((target, dep, start_line))
        i += 1
    return edges


# (AR) قواعد العبور الممنوعة: (طبقة الهدف) → (طبقة التبعيّة) مع سبب.
def violation_reason(src_layer, dst_layer):
    if src_layer in ("interp", "exe-interp") and dst_layer == "compiler":
        return "هدف/تنفيذيّ المفسّر يربط هدفًا من نظام المترجم"
    if src_layer in ("compiler", "exe-compiler") and dst_layer == "interp":
        return "هدف/تنفيذيّ المترجم يربط هدفًا من نظام المفسّر"
    if src_layer == "foundation" and dst_layer in ("interp", "compiler"):
        return "الأساس sad_shared يربط هدفًا من أحد المحرّكَين (يجب أن يبقى قاعدةً نقيّة)"
    return None


def main():
    edges = []
    files = 0
    for path in iter_cmake_files():
        these = parse_links(path)
        if these:
            files += 1
        rel = os.path.relpath(path, ROOT).replace("\\", "/")
        for target, dep, ln in these:
            edges.append((rel, ln, target, dep))

    violations = []
    cross_checked = 0
    for rel, ln, target, dep in edges:
        sl = layer_of(target)
        dl = layer_of(dep)
        if sl is None or dl is None:
            continue
        cross_checked += 1
        why = violation_reason(sl, dl)
        if why:
            violations.append((rel, ln, target, dep, sl, dl, why))

    print("== حارس طبقات أهداف البناء / CMake layering guard ==")
    print(f"   ملفّات CMake ذات روابط: {files}")
    print(f"   حواف ربط بين أهداف معروفة: {cross_checked}")
    if violations:
        print(f"\n[FAIL] {len(violations)} اختراق(ات) لطبقات الأهداف:\n")
        for rel, ln, target, dep, sl, dl, why in violations:
            print(f"  {rel}:{ln}  {target} [{sl}] → {dep} [{dl}]  ← {why}")
        print("\nالمحرّكان (المفسّر/المترجم) يجب ألّا يرتبطا مباشرةً.")
        print("راجع docs/architecture/cmake-target-boundaries.md §3-§6.")
        return 1
    print("\n[OK] لا اختراق — طبقات الأهداف سليمة (لا روابط مباشرة بين المحرّكَين).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
