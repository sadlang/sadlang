# -*- coding: utf-8 -*-
"""قياسُ طبقاتِ الأنواعِ وجسورِها — لا نثرًا بل رقمًا يُعادُ إنتاجُه.

(AR) هذا الأمرُ يقيسُ ثلاثةَ أشياءَ في شجرةِ العملِ نفسِها:

  ① **انقلابُ الطبقات**: كم ترويسةً يملكُها المترجّمُ (`compiler/include/`)
     تُضمِّنُها الطبقةُ المشتركةُ (`shared/`). الصفرُ يعني أنّ «المشترَك» يُبنى
     بلا المترجّم.

  ② **الجسورُ المُخمِّنة**: دالّةُ عبورٍ بين طبقتَي نوعٍ فرعُها الافتراضيُّ
     يردُّ **نوعًا بعينِه** بدلَ أن يردَّ «مجهولًا» أو يكسِر. وهذا هو العطبُ
     الذي سُجِّلَ في `shared/types/src/sad_type_system.cpp` **أربعَ مرّاتٍ**
     (`أي` · الاختياريّ · «عدم» · ثمانيةُ ألفاظٍ عدديّة): «لا أعرف» تنقلبُ
     يقينًا كاذبًا. والعدّادُ يقيسُ **نمطَ** الفرعِ لا حالةً منه.

  ③ **موتُ `Sad::Data::Type`**: كم موضعًا يُنشئُ منه كائنًا في شفرةِ الإنتاج
     (خارجَ `tests/`). الصفرُ يعني أنّ الصنفَ اسمٌ لا قيمةَ له.

(AR) ⚠️ **والوسائطُ جزءٌ من الأمرِ لا حاشيةٌ له**: الصيغةُ المجرّدةُ تطبعُ
     المقيسَ ولا تحكمُ عليه، فمَن يتّبعُ الصفحةَ يرى «✅» فوقَ عددٍ بالٍ.
     مرِّرْ `--انقلاب N --مخمِّنة N --ثالث N` ليُخفِقَ الأمرُ عندَ الانجراف.

(EN) Measures the type-layer inversion, the guessing bridges, and whether
     Sad::Data::Type is dead. Pass the published numbers as arguments so the
     command fails on drift instead of printing a stale figure under a tick.
"""

import argparse
import os
import re
import sys

# (AR) مخرَجُ ويندوزَ الافتراضيُّ صفحةُ رموزٍ محلّيّةٌ تكسِرُ العربيّة.
# (EN) Windows' default console codepage breaks Arabic output.
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


# ═══════════════════════════════════════════════════════════════════════════
# (AR) سجلُّ الجسورِ — كلُّ مدخلٍ يُبحَثُ عنه في الشجرة، فإن غاب أخفقَ الأمر.
#      فلا يُقرأُ اختفاءُ جسرٍ «إصلاحًا» وهو نقلٌ أو إعادةُ تسمية.
# (EN) Bridge registry. A missing entry fails the run: a vanished bridge must
#      not read as a fix when it is merely a rename or a move.
# ═══════════════════════════════════════════════════════════════════════════
BRIDGES = [
    # (الملفّ، التوقيعُ المُلتقَط، وصفُ العبور)
    ("shared/types/src/sad_type_system.cpp",
     "SadTypePtr SadType::fromValueType",
     "SadTypeKind → SadTypePtr"),
    ("shared/semantic/src/semantic/type_checker.cpp",
     "TypePtr TypeChecker::sadKindToTypePtr",
     "SadTypeKind → TypeSystem::TypePtr"),
    ("shared/types/include/sad_type_system.h",
     "SadTypePtr getByKind",
     "SadTypeKind → SadTypePtr (سجلّ)"),
    ("shared/parser/src/core/parser_helpers.cpp",
     "Types::SadTypeKind ParserCore::mapTokenTypeToKind",
     "رمزٌ معجميّ → SadTypeKind"),
    ("compiler/src/frontend/sir_builder_helpers.cpp",
     "Sad::Types::SadTypePtr SIRBuilder::astTypeToSadType",
     "SadTypeKind → SadTypePtr (تفويض)"),
    ("compiler/src/backend/llvm/llvm_type_mapper.cpp",
     "llvm::Type *LLVMTypeMapper::mapSIRType",
     "SadTypeKind → llvm::Type"),
    ("compiler/src/backend/llvm/llvm_type_mapper.cpp",
     "llvm::Type *LLVMTypeMapper::mapSadType(const Sad::Types::SadTypePtr",
     "SadTypePtr → llvm::Type (تفويض)"),
]

# ═══════════════════════════════════════════════════════════════════════════
# (AR) جسورٌ **مُتقاعِدة** — تُذكَرُ ولا تُبحَث، ويُحرَسُ أنّها لم تعُدْ في الشجرة.
#      فحذفُ صفٍّ من السجلِّ يُخفي التاريخَ، وبقاؤه بلا حراسةٍ يُخفِقُ الأمرَ
#      أبدًا. وهذا نظيرُ حقلِ `retired` في كتالوجِ الأخطاء.
# (EN) Retired bridges: named, not searched, and guarded as gone. Deleting the
#      row would erase the history; keeping it live would fail forever.
# ═══════════════════════════════════════════════════════════════════════════
RETIRED = [
    # (AR) 🔑 سجلُّ الهرمِ الثاني كلُّه حُذِف. وكان `internPrimitiveType` بابَه
    #      إلى التعدادِ المولَّد؛ وقد صارَ السؤالُ يُجابُ من سجلِّ الهرمِ الواحد
    #      (`SadTypeRegistry::getByKind`) وهو جسرٌ **قائمٌ سلفًا** في هذا السجلّ.
    #      فجسرانِ صارا واحدًا — لا جسرٌ اختفى بلا أثر.
    ("shared/type_system/src/type_registry.cpp",
     "TypePtr TypeRegistry::internPrimitiveType",
     "سجلُّ الهرمِ الثاني — أُذيبَ في SadTypeRegistry::getByKind"),
    # (AR) 🔑 وكان وصفُهما في السجلِّ «TypeSystem::Type → llvm::Type» وهو **غلط**
    #      مقيس: النوعُ في توقيعِهما كان `Sad::LLVM::Type` — هرمًا **ثالثًا**
    #      لا صلةَ له بـ`Sad::TypeSystem::Type`. وقِيسَ أنّ لا مُنشِئَ له في
    #      المستودعِ كلِّه، فحُذِفَ الهرمُ وعنقودُه المغلق.
    ("compiler/src/backend/llvm/llvm_type_mapper.cpp",
     "llvm::Type *LLVMTypeMapper::mapSadType(std::shared_ptr<Type>",
     "هرمٌ ثالثٌ (Sad::LLVM::Type) لا يُنشِئُه أحد — حُذِفَ بعنقودِه"),
    ("compiler/src/backend/llvm/builders/core/types_ops.cpp",
     "llvm::Type *TypesCodeGen::convertType",
     "تفويضٌ إلى الهرمِ الثالثِ المحذوف"),
    ("shared/semantic/src/semantic/type_checker.cpp",
     "dataTypeToTypePtr",
     "غلافٌ لا يفعلُ إلّا نداءَ sadKindToTypePtr — باسمِ نظامٍ (DataType) محذوف"),
]

# (AR) ألفاظٌ تدلُّ على أنّ الفرعَ يردُّ **نوعًا بعينِه** — أي يُخمّن.
# (EN) Tokens marking a concrete guess.
GUESS = (
    "getVoid", "getInteger", "getInt64Type", "getVoidType", "getAny",
    "getString", "getFloat", "getBoolean", "getInt1Type", "getStringPtrType",
)


def read(rel):
    path = os.path.join(ROOT, rel)
    if not os.path.exists(path):
        return None
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        return f.read()


def body_of(text, anchor):
    """(AR) يستخرجُ جسمَ الدالّةِ بموازنةِ الأقواسِ ابتداءً من التوقيع."""
    i = text.find(anchor)
    if i < 0:
        return None
    j = text.find("{", i)
    if j < 0:
        return None
    depth, k = 0, j
    while k < len(text):
        if text[k] == "{":
            depth += 1
        elif text[k] == "}":
            depth -= 1
            if depth == 0:
                return text[j:k + 1]
        k += 1
    return None


def strip_comments(body):
    body = re.sub(r"/\*.*?\*/", "", body, flags=re.S)
    body = re.sub(r"//[^\n]*", "", body)
    return body


def _match_block(text, open_idx):
    """(AR) يوازنُ الأقواسَ ابتداءً من `{` عندَ open_idx فيردُّ نهايتَه."""
    depth, k = 0, open_idx
    while k < len(text):
        if text[k] == "{":
            depth += 1
        elif text[k] == "}":
            depth -= 1
            if depth == 0:
                return k
        k += 1
    return len(text) - 1


def fallback_of(body):
    """(AR) يستخرجُ **نصَّ الفرعِ الافتراضيِّ وحدَه** لا جسمَ الدالّةِ كلَّه.

    (AR) ⚠️ وهذا التمييزُ هو الفرقُ بين قياسٍ وضجيج: أوّلُ صيغةٍ كتبتُها ضمّت
         ٤٠٠ حرفٍ من ذيلِ الدالّةِ إلى الفرعِ الافتراضيّ، فقرأت `getAnyType()`
         الواقعةَ في حالةِ `Class` صراحةً وحكمت على `internPrimitiveType`
         و`sadKindToTypePtr` بالتخمينِ — وفرعُهما الافتراضيُّ صادقٌ حرفيًّا
         (`internType(...)` و`getUnknownType()`). كذَّبَها أوّلُ تشغيل.
    (EN) Returns the default branch alone. An earlier version appended the last
         400 chars of the body and thus read an explicit `Class` case as the
         default, misjudging two honest bridges. The first run falsified it.
    """
    code = strip_comments(body)
    sw = code.rfind("switch")
    if sw < 0:
        return code  # (AR) لا `switch`: الدالّةُ كلُّها سقوطُها
    ob = code.find("{", sw)
    if ob < 0:
        return code
    cb = _match_block(code, ob)
    switch_body, post_switch = code[ob:cb + 1], code[cb + 1:]
    d = switch_body.rfind("default:")
    if d >= 0:
        return switch_body[d:] + chr(10) + post_switch
    # (AR) `switch` بلا فرعٍ افتراضيّ ⇒ السقوطُ هو ما بعدَه.
    return post_switch


def classify(body):
    """(AR) مُخمِّنٌ = فرعُه الافتراضيُّ يردُّ نوعًا **بعينِه**."""
    tail = fallback_of(body)
    return "مُخمِّن" if any(g in tail for g in GUESS) else "صادق"


def measure_inversion():
    """(AR) تضميناتُ ترويساتِ المترجّمِ من داخلِ `shared/`.

    (AR) 🔑 **ويُحسَبُ الاسمُ المجرَّدُ كما يُحسَبُ المسار.** أوّلُ صيغةٍ لهذه
         الدالّةِ لم تعُدَّ إلّا `#include "types/enum_types.h"`، فأعطت صفرًا
         بينما `shared/ownership/src/exhaustiveness.cpp` يضمُّ `"enum_types.h"`
         مجرَّدًا من مجلَّدٍ مُسطَّحٍ على المسار — تبعيّةٌ مقلوبةٌ كاملةٌ تهربُ من
         العدّادِ لأنّها مكتوبةٌ بهجاءٍ آخر. كشفَها البناءُ لا القياس.
         والاسمُ المجرَّدُ لا يُحسَبُ إلّا حين **لا يشترِكُ فيه** رأسٌ مملوكٌ
         لـ`shared/` — و`type.h` مثالُه: اسمانِ لملفَّين.
    (EN) Bare-name includes count too. The first version matched only the
         path-qualified spelling and returned zero while a full inverted
         dependency was spelled bare. A bare name counts only when no
         shared/-owned header shares it (type.h is the counter-example).
    """
    hits = []
    pat = re.compile(r'#\s*include\s+[<"]([^">]+)[">]')
    comp_dir = os.path.join(ROOT, "compiler", "include")
    compiler_rel, compiler_base = set(), {}
    for base, _dirs, files in os.walk(comp_dir):
        for fn in files:
            if fn.endswith((".h", ".hpp")):
                rel = os.path.relpath(os.path.join(base, fn), comp_dir).replace("\\", "/")
                compiler_rel.add(rel)
                compiler_base.setdefault(fn, rel)
    shared_base = set()
    for base, dirs, files in os.walk(os.path.join(ROOT, "shared")):
        dirs[:] = [d for d in dirs if d not in ("_archive", "build")]
        for fn in files:
            if fn.endswith((".h", ".hpp")):
                shared_base.add(fn)
    for base, dirs, files in os.walk(os.path.join(ROOT, "shared")):
        dirs[:] = [d for d in dirs if d not in ("_archive", "build")]
        for fn in files:
            if not fn.endswith((".h", ".hpp", ".cpp", ".cc")):
                continue
            p = os.path.join(base, fn)
            with open(p, "r", encoding="utf-8", errors="replace") as f:
                for n, line in enumerate(f, 1):
                    m = pat.search(line)
                    if not m:
                        continue
                    inc = m.group(1)
                    owned = inc in compiler_rel or (
                        "/" not in inc and inc in compiler_base and inc not in shared_base)
                    if owned:
                        hits.append("%s:%d → %s" % (
                            os.path.relpath(p, ROOT).replace("\\", "/"), n, inc))
    return hits


def measure_data_type():
    """(AR) إنشاءُ كائنٍ من `Sad::Data::Type` في شفرةِ الإنتاج (خارجَ tests/)."""
    hits = []
    pat = re.compile(r"\b(?:Data::)?Type\s+\w+\s*\(\s*\"|new\s+(?:Sad::)?Data::Type\b")
    for base, dirs, files in os.walk(ROOT):
        dirs[:] = [d for d in dirs
                   if d not in ("_archive", "build", ".git", "tests", "node_modules")]
        for fn in files:
            if not fn.endswith((".cpp", ".cc", ".h", ".hpp")):
                continue
            p = os.path.join(base, fn)
            with open(p, "r", encoding="utf-8", errors="replace") as f:
                txt = f.read()
            if "Data::Type" not in txt and "using Type = Data::Type" not in txt:
                continue
            for n, line in enumerate(txt.splitlines(), 1):
                if pat.search(line):
                    hits.append("%s:%d" % (os.path.relpath(p, ROOT).replace("\\", "/"), n))
    return hits


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--انقلاب", type=int, default=None, dest="inversion")
    ap.add_argument("--مخمِّنة", type=int, default=None, dest="guessing")
    ap.add_argument("--جسور", type=int, default=None, dest="bridges")
    ap.add_argument("--ثالث", type=int, default=None, dest="datatype")
    args = ap.parse_args()

    failures = []
    guarded = 0
    total_guardable = 4

    print("═" * 74)
    print("① انقلابُ الطبقات — تضميناتُ `compiler/include/` من داخلِ `shared/`")
    print("═" * 74)
    inv = measure_inversion()
    for h in inv:
        print("   ⛔ " + h)
    print("   المقيس: %d" % len(inv))

    print("")
    print("═" * 74)
    print("② جسورُ الأنواع — والفرعُ الافتراضيُّ لكلٍّ منها")
    print("═" * 74)
    guessers, missing = [], []
    for rel, anchor, desc in BRIDGES:
        txt = read(rel)
        body = body_of(txt, anchor) if txt else None
        if body is None:
            missing.append("%s :: %s" % (rel, anchor))
            print("   ❓ مفقود: %s :: %s" % (rel, anchor))
            continue
        verdict = classify(body)
        mark = "⚠️ " if verdict == "مُخمِّن" else "✅"
        if verdict == "مُخمِّن":
            guessers.append("%s :: %s" % (rel, anchor))
        print("   %s %-11s %s  — %s" % (mark, verdict, desc, anchor.split("::")[-1]))
    for rel, sym, why in RETIRED:
        txt = read(rel) or ""
        if sym in txt:
            print("   ⛔ مُتقاعِدٌ عادَ: %s :: %s" % (rel, sym))
            failures.append("جسرٌ مُتقاعِدٌ عادَ إلى الشجرة: %s" % sym)
        else:
            print("   🪦 مُتقاعِد    %s — %s" % (sym, why))
    print("   الجسورُ المقيسة: %d · المُخمِّنةُ منها: %d · المُتقاعِدة: %d"
          % (len(BRIDGES) - len(missing), len(guessers), len(RETIRED)))
    if missing:
        failures.append("جسورٌ لم تُوجَدْ في الشجرة (%d) — نقلٌ أو إعادةُ تسميةٍ لا إصلاح" % len(missing))

    print("")
    print("═" * 74)
    print("③ `Sad::Data::Type` — إنشاءُ كائنٍ منه في شفرةِ الإنتاج")
    print("═" * 74)
    dt = measure_data_type()
    for h in dt:
        print("   • " + h)
    print("   المقيس: %d" % len(dt))

    print("")
    print("═" * 74)
    checks = [(args.inversion, len(inv), "انقلاب"),
              (args.guessing, len(guessers), "مخمِّنة"),
              (args.bridges, len(BRIDGES) - len(missing), "جسور"),
              (args.datatype, len(dt), "ثالث")]
    for expected, actual, name in checks:
        if expected is None:
            continue
        guarded += 1
        if expected != actual:
            failures.append("%s: المكتوبُ %d والمقيسُ %d" % (name, expected, actual))
    print("حُرِسَ %d عددًا من %d." % (guarded, total_guardable))
    if failures:
        for f in failures:
            print("❌ " + f)
        return 1
    print("✅ مقيسٌ — ولا انجراف.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
