#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ «رتبةُ المدمجِ تُعلَن مرّةً واحدةً في مصدرِ الحقيقة».

     يُحرَس من أربعةِ أطراف:

       (١) **رقمٌ حرفيّ**: موضعُ فحصٍ يمرّرُ عددًا مكتوبًا في الشيفرة ⇒ نسخةٌ
           ثانيةٌ للحقيقةِ تنجرفُ بلا أن يحمرَّ شيء. المطلوبُ ثابتٌ مُولَّد.
       (٢) **ثابتٌ غريب**: ذراعٌ تحرسُ أسماءً وتقيسُ بثابتِ مدمجٍ **آخرَ** ⇒
           رتبةٌ صحيحةُ الشكلِ خاطئةُ المرجع، وهي أخفى من الرقمِ الحرفيّ.
       (٣) **إعلانٌ ميّت**: `arity` في مصدرِ الحقيقةِ لا تفرضُه ذراعٌ ⇒ عقدٌ
           لا يُطبَّق أبدًا وخضرةٌ بلا حراسة.
       (٤) **فرضٌ غيرُ مُعلَن**: ذراعٌ تفرضُ رتبةً لمدمجٍ بلا `arity` في مصدرِ
           الحقيقة ⇒ الحقيقةُ في الشيفرةِ لا في مصدرها.

     والدَّينُ المتبقّي (مدمجاتٌ لا رتبةَ مفروضةً لها أصلًا) **يُطبَع بعددِه**
     ولا يُبتَلَع: صمتُ الحارسِ عن غيرِ المقيسِ يُقرَأ تغطيةً وهو ليس بها.
(EN) Guard: builtin arity is declared once in SoT — no literals, no foreign
     constants, no dead declarations, no unmeasured enforcement. Debt printed.
============================================================================
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
# (AR) 🔑 **شبكةُ رمزِ الخروجِ قلبٌ واحدٌ** — انظر ترويسةَ الوحدة.
from _lib.guard_exit import رمز_الخروج  # noqa: E402

import yaml

sys.path.insert(0, str(Path(__file__).resolve().parent))
from builtin_arity_extract import ArityCheck, arity_checks  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
SOT_DIR = ROOT / "language-truth" / "builtins"
TYPE_METHODS = ROOT / "language-truth" / "type_methods.yaml"
_TARGET_LABEL = {"ARRAY": "Array", "STRING": "String", "MAP": "Map",
                 "CHANNEL": "Channel", "ANY": "Any"}


class ToolFault(Exception):
    """(AR) عطبُ آلةٍ: مرجعٌ لم يُقرأْ — لا حكمَ للحارسِ على المحتوى (رمز ٢)."""


# ═══ حدودٌ نازلةٌ على ما كان يُطبَعُ ولا يُقاس ═════════════════════════════
# (AR) 🔑 **«يُطبَع بعددِه ولا يُبتَلَع» كان نصفَ الوعد.** الترويسةُ تقولُ إنّ
#      الدَّينَ يُعلَن، وهو يُعلَنُ فعلًا — لكنّه **لا يُحاكَم**: مدمجٌ جديدٌ
#      يُعلَنُ في مصدرِ الحقيقةِ بلا فرضِ رتبةٍ يرفعُ العددَ سطرًا واحدًا
#      ويمرُّ أخضر. فصارَ للعددِ سقفٌ نازلٌ كسائرِ ديونِ هذا المستودع.
CEILING_UNENFORCED = 767
# (AR) وما يُفرَضُ برقمٍ في الشرطِ لا من مصدرِ الحقيقة — دَينٌ ثانٍ أضيقُ.
CEILING_INLINE_ONLY = 1
# (AR) 🔑 **وبابُ «لا يبلغُها المترجّم» كان يبتلعُ الإخفاق.** وقِيسَ حيًّا:
#      إخفاءُ `builtin_arity_generated.h` أبقى الحارسَ **أخضرَ** ورفعَ العددَ
#      من ٢٢ إلى ٩١ — يتبخّرُ نصفُ الحراسةِ ويُطبَعُ ذلك سطرَ إحصاءٍ لا شكوى.
#      فالترويسةُ صارت مرجعًا واجبًا (رمز ٢ عندَ غيابِها)، والعددُ تحتَ سقف.
CEILING_UNREACHABLE_METHODS = 22


def _sot_arities() -> tuple[dict[tuple[str, str], tuple[int, int]], int]:
    table: dict[tuple[str, str], tuple[int, int]] = {}
    total = 0
    files = sorted(SOT_DIR.glob("*.yaml"))
    if not files:
        raise ToolFault("لا ملفَّ مدمجاتٍ في %s — لم يُقرأْ مصدرُ الحقيقة"
                        % SOT_DIR.name)
    for path in files:
        doc = yaml.safe_load(path.read_text(encoding="utf-8")) or {}
        for fn in doc.get("functions") or []:
            total += 1
            arity = fn.get("arity")
            if arity:
                # (AR) الرتبةُ المفتوحةُ أعلاها تُعلَن بـ`variadic: true` بلا
                #      `max`؛ ويقابلُها في الرأسِ المُولَّدِ `UNBOUNDED`.
                # (AR) وحقلٌ ناقصٌ في مصدرِ الحقيقةِ عطبُ آلةٍ يُسمّى، لا
                #      `KeyError` يتفجّرُ أثرًا رجعيًّا برمزِ ١.
                try:
                    top = (sys.maxsize if arity.get("variadic")
                           else int(arity["max"]))
                    table[(fn["namespace"], fn["cpp_id"])] = (int(arity["min"]),
                                                             top)
                except (KeyError, TypeError, ValueError) as exc:
                    raise ToolFault(
                        "إعلانُ رتبةٍ معطوبٌ في %s ⇐ %s::%s (%s)"
                        % (path.name, fn.get("namespace"), fn.get("cpp_id"),
                           exc.__class__.__name__))
    # (AR) وطرائقُ الأنواعِ سطحٌ ثانٍ لعقودِ الرتبة، فتُقرَأ ههنا أيضًا؛ وإلّا
    #      قُرئت حراستُها «فرضًا غيرَ مُعلَن» وهي معلَنةٌ في ملفِّها.
    #      🔑 والمعرِّفُ يُشتَقُّ بالخوارزميّةِ **نفسِها** التي في المولِّد،
    #      وإلّا سمّى كلٌّ الشيءَ باسمٍ وانحرفا بلا أن يحمرَّ شيء.
    for target, cpp_id, arity in _type_method_arities():
        total += 1
        if arity:
            top = sys.maxsize if arity.get("variadic") else int(arity["max"])
            table[(f"TypeMethods::{target}", cpp_id)] = (int(arity["min"]), top)
    return table, total


def _type_method_arities():
    # (AR) 🔑 وغيابُ الملفِّ كان **رجوعًا صامتًا**: يُقرأُ سطحًا ثانيًا كاملًا
    #      خاليًا، فتصيرُ عقودُه كلُّها «لا يبلغُها المترجّم» ويخضرُّ الحارس.
    if not TYPE_METHODS.exists():
        raise ToolFault("طرائقُ الأنواعِ مفقودة: %s" % TYPE_METHODS.name)
    doc = yaml.safe_load(TYPE_METHODS.read_text(encoding="utf-8")) or {}
    by_target: dict[str, list[dict]] = {}
    for method in doc.get("methods") or []:
        by_target.setdefault(method.get("target_type", "ANY"), []).append(method)
    for target, methods in by_target.items():
        used: dict[str, int] = {}
        for method in methods:
            base = re.sub(r"[^a-zA-Z0-9]", "_",
                          str(method.get("method_en", ""))).upper().strip("_")
            if not base or base[0].isdigit() or not any(c.isalpha() for c in base):
                base = "METHOD"
            if base not in used:
                used[base] = 1
                cpp_id = base
            else:
                cpp_id = f"{base}_{used[base]}"
                used[base] += 1
            yield _TARGET_LABEL.get(target, target), cpp_id, method.get("arity")


_GENERATED_ARITY = (ROOT / "shared" / "builtins" / "generated"
                    / "builtin_arity_generated.h")
_BUILDERS = ROOT / "compiler" / "src" / "frontend" / "builders"
_TABLE_ROW = re.compile(r'\{"(\w+)",\s*"([^"]+)",\s*\{')


def _table_enforced() -> tuple[set[tuple[str, str]], set[str]]:
    """(AR) يُعيد ما يفرضُه الجدولُ المُولَّد، ثمّ الأهدافَ التي **لا يبلغُها**
       موضعُ الإرسال. والوصولُ يُقاسُ من أثرين: صفٌّ في الجدول، وربطُ الهدفِ
       بوسمِ نوعٍ في الملفِّ الذي ينادي `lookup`. فقائمةُ الإذنِ ليست نثرًا
       يتعفّن — تُشتَقُّ من الشيفرةِ فتتبعُها في الاتّجاهين."""
    # (AR) 🔑 **والترويسةُ المُولَّدةُ مرجعٌ واجبٌ لا اختياريّ.** كان الشرطُ
    #      `if exists()` فغيابُها يُفرِغُ `reachable` بلا صوت، ويُعادُ تصنيفُ
    #      كلِّ عقدٍ «لا يبلغُه المترجّم» — أخضرُ لأنّ الشرطَ لا يمكنُ أن يكونَ
    #      كاذبًا. (قِيسَ: ٢٢ ⇐ ٩١ ورمزُ صفر.)
    if not _GENERATED_ARITY.is_file():
        raise ToolFault("جدولُ الرتبِ المُولَّدُ مفقود: %s — أعِدْ التوليد"
                        % _GENERATED_ARITY.name)
    reachable: set[str] = set()
    for path in _BUILDERS.glob("*.cpp"):
        text = path.read_text(encoding="utf-8")
        if "TypeMethods::lookup(" not in text:
            continue
        reachable |= set(re.findall(r'"([A-Z]{3,})"', text))
    header = _GENERATED_ARITY.read_text(encoding="utf-8")
    rows = _TABLE_ROW.findall(header)
    if not rows:
        raise ToolFault("جدولُ الرتبِ المُولَّدُ بلا صفٍّ واحد — تغيّرَ شكلُه؟")
    reachable &= {t for t, _ in rows}
    if not reachable:
        raise ToolFault("لا هدفَ تبلغُه أذرعُ الإرسال — قارئُ `lookup` أعمى")
    enforced = {(f"TypeMethods::{target}", cpp_id)
                for target, cpp_id, arity in _type_method_arities()
                if arity and target in {_TARGET_LABEL.get(t, t) for t in reachable}}
    declared_targets = {target for target, _, arity in _type_method_arities()
                        if arity}
    # (AR) والاسمُ صريحٌ عندَ التعريفِ لا عندَ الإسناد: في `main` يحملُ
    #      `unreachable` **طرائقَ** لا أهدافًا، فاسمٌ واحدٌ بدلالتَينِ في ملفٍّ
    #      واحدٍ فخُّ قراءةٍ لرقعةٍ لاحقة. (كشفَته مراجعةٌ خصميّة.)
    unreachable_targets = (declared_targets
                           - {_TARGET_LABEL.get(t, t) for t in reachable})
    return enforced, unreachable_targets


def _tool_fault(message: str) -> int:
    print("حارس «رتبةُ المدمجِ من مصدرِ الحقيقة»: ✗ عطبُ آلة")
    print("  " + message + " — لم يُقَسْ شيء")
    return 2


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    try:
        declared, total = _sot_arities()
        sites = arity_checks()
        table_enforced, unreachable_targets = _table_enforced()
    except ToolFault as exc:
        return _tool_fault(str(exc))

    # (AR) 🔑 **وصفرُ موضعِ فحصٍ ليس سلامة.** مستخرِجٌ يعمى (تبدّلَ شكلُ الذراعِ
    #      في الأماميّة) يجعلُ `enforced` خاويةً، فتُقرأُ خمسُمئةِ إعلانٍ حيٍّ
    #      «عقدًا لا يُطبَّق» — رفضٌ كاذبٌ عريضٌ برمزِ ١ يوجّهُ إلى الملفِّ الخطأ.
    if not sites:
        return _tool_fault("لا موضعَ فحصٍ استُخرِج — مستخرِجُ الأذرعِ أعمى")
    if not declared:
        return _tool_fault("لا إعلانَ رتبةٍ واحدًا في مصدرِ الحقيقة")

    literals: list[str] = []
    foreign: list[str] = []
    enforced: set[tuple[str, str]] = set()
    inline_sites: list[ArityCheck] = []
    inline_named: set[tuple[str, str]] = set()
    silent_sites: list[ArityCheck] = []
    for site in sites:
        # (AR) الرفضُ في الشرطِ يُعزَل قبلَ كلِّ حكم: ليس رقمًا حرفيًّا يُمرَّر
        #      إلى حارسٍ (فلا يُحمِّر)، وليس ختمًا من مصدرِ الحقيقةِ (فلا يُحتسَب
        #      مفروضًا). صنفٌ ثالثٌ يُعدّ ويُعلَن على حدة.
        if site.silent:
            silent_sites.append(site)
            continue
        if site.inline:
            inline_sites.append(site)
            inline_named.update(site.names)
            continue
        enforced.update(site.names)
        # (AR) ثابتُ الرتبةِ المُنتقى لأخٍ في الذراعِ نفسِها فرضٌ له باسمِه —
        #      وإغفالُه يُحمِّرُ الحارسَ بـ«إعلانٌ ميّت» على عقدٍ مفروضٍ فعلًا.
        enforced.update(site.also_constants)
        # (AR) وثابتُ الرتبةِ المُمرَّرُ إلى الحارسِ فرضٌ لصاحبِه **مهما كانت
        #      صيغةُ مقارنةِ الاسمِ في الذراع**: أذرعُ اللاتزامنِ تقارنُ بايتاتٍ
        #      مهرَّبةً لا ثوابتَ أسماءٍ، فلا يرى المستخرِجُ لها اسمًا — وقراءةُ
        #      ذلك «عقدًا لا يُطبَّق» تُحمِّرُ على حراسةٍ قائمةٍ فعلًا. وبقاءُ
        #      المقارنةِ بالبايتاتِ دَينٌ آخرُ يُسمّى على حدة.
        if site.constant:
            enforced.add(site.constant)
        if site.literal:
            literals.append(f"{site.file}:{site.line}")
            continue
        # (AR) الثابتُ يجب أن يكون لأحدِ أسماءِ الذراعِ نفسِها — وإلّا فالرتبةُ
        #      تُقرأ من مدمجٍ آخر، فتنجرفُ بصمتٍ حين تتغيّرُ رتبةُ ذاك.
        if site.constant and site.names and site.constant not in site.names:
            names = "، ".join(f"{ns}::{cid}" for ns, cid in site.names)
            foreign.append(
                f"{site.file}:{site.line} — الذراعُ تحرسُ [{names}] وتقيسُ بثابتِ "
                f"{site.constant[0]}::{site.constant[1]}")

    # (AR) طرائقُ الأنواعِ تُفرَضُ بجدولٍ مُولَّدٍ في موضعِ إرسالٍ واحد، لا بثابتٍ
    #      في كلِّ فرع. فالفرضُ يُقاسُ من **الأثرَين معًا**: صفٌّ في الجدولِ
    #      المُولَّد، ونداءُ `lookup` قائمٌ في بانياتِ الأماميّة. وسقوطُ أيِّهما
    #      يُعيدُ العقدَ إعلانًا ميّتًا فيحمرّ — وهو المطلوب.
    enforced.update(table_enforced)

    problems: list[str] = []
    if literals:
        problems.append(
            "رتبٌ مكتوبةٌ أرقامًا حرفيّةً في مواضعِ الفحصِ (نسخةٌ ثانيةٌ للحقيقة):\n    "
            + "\n    ".join(literals))
    if foreign:
        problems.append(
            "مواضعُ تقيسُ بثابتِ مدمجٍ آخر:\n    " + "\n    ".join(foreign))
    if silent_sites:
        # (AR) 🔑 الذراعُ الخامسة: رفضٌ لا يسجّلُ خطأً. المصرّفُ يخرجُ بصفرٍ
        #      ويُنتجُ ثنائيًّا **بلا النداء** — سطرٌ في كودِ نواةٍ «يُنفَّذ»
        #      وهو غيرُ موجود. لا يُخفق فلا يُرى، وهو أخطرُ من الرقمِ الحرفيّ
        #      بمراتب: ذاك عقدٌ ينجرف، وهذا عملٌ يتبخّر.
        problems.append(
            f"أذرعُ رفضٍ لا تسجّلُ خطأً ({len(silent_sites)}) ⇒ النداءُ يتبخّرُ "
            "والمصرّفُ يخرجُ بصفر:\n    "
            + "\n    ".join(f"{s.file}:{s.line} — {s.silent}"
                            for s in silent_sites[:25]))

    # (AR) عقدٌ يفرضُه المفسّرُ ولا يبلغُه المترجّم: طرائقُ «أي» و«قناة» لا وسمَ
    #      لهما يُميَّزُ به المستقبِلُ عند التصريف — و«أي» تتصادمُ هجاءً بِرتبٍ
    #      مختلفة (`عين` ٢ للخريطةِ و١ لِـ«مستقبل») ففرضُها يردُّ نداءً صحيحًا.
    #      يُعَدُّ ويُعلَن، ولا يُحمِّر: الإعلانُ صادقٌ والفرضُ نصفُ بالغ.
    unreachable = {(ns, cid) for ns, cid in declared.keys() - enforced
                   if ns.startswith("TypeMethods::")
                   and ns.split("::", 1)[1] in unreachable_targets}
    dead = sorted((declared.keys() - enforced) - unreachable)
    if dead:
        problems.append(
            f"إعلاناتُ `arity` لا تفرضُها ذراعٌ ({len(dead)}) ⇒ عقدٌ لا يُطبَّق:\n    "
            + "\n    ".join(f"{ns}::{cid}" for ns, cid in dead[:20]))

    unmeasured = sorted(enforced - declared.keys())
    if unmeasured:
        problems.append(
            f"أذرعٌ تفرضُ رتبةً بلا `arity` في مصدرِ الحقيقة ({len(unmeasured)}):\n    "
            + "\n    ".join(f"{ns}::{cid}" for ns, cid in unmeasured[:20]))

    # ═══ الديونُ الثلاثةُ تُحاكَمُ بسقوفٍ نازلةٍ لا تُطبَعُ وحدَها ═══
    inline_named_debt = sorted(inline_named - declared.keys())
    unenforced_count = total - len(declared) - len(inline_named_debt)
    if unenforced_count > CEILING_UNENFORCED:
        problems.append(
            f"مدمجاتٌ لا يفرضُ رتبتَها أحد: {unenforced_count} فوقَ السقفِ "
            f"{CEILING_UNENFORCED} — النداءُ الناقصُ يمرُّ صامتًا. اشتقَّ العقدَ "
            "من الشيفرةِ إلى مصدرِ الحقيقة، أو ارفعِ السقفَ بقرارٍ يُرى في الفرق.")
    if len(inline_named_debt) > CEILING_INLINE_ONLY:
        problems.append(
            f"رتبٌ مفروضةٌ برقمٍ في الشرط: {len(inline_named_debt)} فوقَ السقفِ "
            f"{CEILING_INLINE_ONLY} — ارفعِ العددَ إلى مصدرِ الحقيقة.")
    if len(unreachable) > CEILING_UNREACHABLE_METHODS:
        problems.append(
            f"عقودٌ لا يبلغُها المترجّم: {len(unreachable)} فوقَ السقفِ "
            f"{CEILING_UNREACHABLE_METHODS} — وهذا البابُ يبتلعُ الإخفاق، "
            "فلا يتّسعُ إلّا بقرارٍ صريح.")

    if problems:
        print("حارس «رتبةُ المدمجِ من مصدرِ الحقيقة»: ✗")
        for problem in problems:
            print("  " + problem)
        return 1

    # (AR) الدَّينُ يُقسَم قسمين لأنّهما مختلفان في الخطرِ وفي العلاج:
    #      ما يُرفَض برقمٍ محلّيّ (الرتبةُ مفروضةٌ، والعلاجُ رفعُ العددِ إلى
    #      مصدرِ الحقيقة)، وما لا يفرضُه أحدٌ (النداءُ يتبخّرُ صامتًا، والعلاجُ
    #      اشتقاقُ عقدٍ من الشيفرةِ لا اختراعُه). جمعُهما رقمًا واحدًا يُقرأ
    #      أسوأَ من الواقعِ ويخفي أيَّهما يلزمُ العملُ عليه أوّلًا.
    sealed_sites = [s for s in sites if not s.inline]
    print("حارس «رتبةُ المدمجِ من مصدرِ الحقيقة»:")
    print(f"  مُعلَنةٌ ومفروضة: {len(declared)} مدمجًا · مواضعُ فحص: "
          f"{len(sealed_sites)} (كلُّها بثوابتَ مُولَّدة)")
    print(f"  مفروضةٌ برقمٍ في الشرطِ لا من مصدرِ الحقيقة: "
          f"{len(inline_named_debt)} (السقف {CEILING_INLINE_ONLY}) مدمجًا "
          f"في {len(inline_sites)} موضعَ رفض — الرتبةُ محروسةٌ والعددُ منفلت.")
    print(f"  يفرضُها المفسّرُ ولا يبلغُها المترجّم: {len(unreachable)} "
          f"(السقف {CEILING_UNREACHABLE_METHODS}) طريقةً "
          f"({'، '.join(sorted(unreachable_targets)) or 'لا هدف'}) — لا وسمَ "
          "نوعٍ يميّزُ المستقبِلَ عند التصريف. مُعلَنٌ ومقيسٌ لا مسكوتٌ عنه.")
    print(f"  لا يفرضُها أحد: {unenforced_count} (السقف {CEILING_UNENFORCED}) "
          f"من {total} مدمجًا — النداءُ الناقصُ يمرّ صامتًا. لا يُخترَع لها "
          "عقد، بل يُشتقّ.")
    print("  ✓ لا رقمَ حرفيًّا، ولا ثابتًا غريبًا، ولا إعلانًا ميّتًا، ولا فرضًا "
          "غيرَ مُعلَن.")
    return 0


if __name__ == "__main__":
    sys.exit(رمز_الخروج(main))
