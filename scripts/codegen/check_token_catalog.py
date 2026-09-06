#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
═══════════════════════════════════════════════════════════════════════════════════
ملف: check_token_catalog.py
الوصف: حارس اكتمال كتالوج المفردات — يقابل تعداد KEYWORD_* في token.h
       بسجلّات tokenType في language-truth/keywords.yaml (مصدر الحقيقة).

(AR) الفجوة التي يسدّها (دَين «اكتمال كتالوج مفردات SoT»):
     token.h يعرّف رموز KEYWORD_* أكثر ممّا يسجّله الكتالوج، ولا حارس كان يقابلهما —
     فتتّسع الفجوة صامتةً مع كلّ كلمة جديدة. هذا الحارس (يعمل ضمن `x.py gen --check`):
       ① يستخرج كلّ عنصر KEYWORD_* المُعرَّف فعليًّا في تعداد token.h (بعد إسقاط التعليقات).
       ② يستخرج كلّ tokenType: KEYWORD_* مُسجَّل في keywords.yaml.
       ③ يفشل إن ظهر رمز جديد في token.h غير مُسجَّل في الكتالوج وغير مُعلَن في قائمة
          «الدَّين الموروث» أدناه — فلا تتّسع الفجوة بعد اليوم.
       ④ يفشل إن بات بندُ دَينٍ مُسجَّلًا في الكتالوج أو حُذف من token.h — فيجب حذفه من
          القائمة (القائمة تنكمش فقط، لا تكبر إلا بقرار لغويّ موثَّق).
       ⑤ يفشل إن أشار الكتالوج إلى KEYWORD_* غير موجود في token.h (رمز شبح).

(EN) Vocabulary-catalog completeness guard (runs inside `x.py gen --check`):
     diffs the KEYWORD_* enumerators actually declared in token.h against the
     tokenType entries registered in language-truth/keywords.yaml. New uncataloged
     enumerators fail the gate; the pre-existing gap is pinned as an explicit,
     shrink-only "inherited debt" allowlist below; stale allowlist entries and
     catalog-only phantoms fail too.

─── الاستخدام ──────────────────────────────────────────────────────────────────
    python scripts/codegen/check_token_catalog.py            # الحارس (rc=0 سليم)
    python scripts/codegen/check_token_catalog.py --list-gap # اطبع فجوة الدَّين الحاليّة
═══════════════════════════════════════════════════════════════════════════════════
"""

import argparse
import os
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
# (AR) 🔑 **شبكةُ رمزِ الخروجِ قلبٌ واحدٌ** — انظر ترويسةَ الوحدة.
from _lib.guard_exit import رمز_الخروج  # noqa: E402

try:
    import yaml
except ImportError:
    print("❌ مطلوب PyYAML: pip install pyyaml", file=sys.stderr)
    sys.exit(2)

ROOT = Path(__file__).resolve().parents[2]
TOKEN_HEADER = ROOT / "shared" / "lexer" / "include" / "token.h"
KEYWORDS_CATALOG = ROOT / "language-truth" / "keywords.yaml"

# (AR) بادئة رموز الكلمات المفتاحية في تعداد token.h — عقد التسمية الوحيد للحارس.
# (EN) The keyword-token prefix in token.h's enum — the guard's single naming contract.
KEYWORD_PREFIX = "KEYWORD_"

# (AR) عنصر تعداد مُعرَّف فعليًّا: يبدأ سطره (بعد إسقاط التعليقات) بالاسم، وقد يتبعه
#      `= قيمة`، ثمّ فاصلة **أو** قوس إغلاق التعداد `}` — فالعنصر الأخير في تعداد C++
#      يجوز بلا فاصلة لاحقة (النمط الفعليّ لآخر عناصر token.h اليوم)، ولولا `}` لغاب
#      رمز أخير جديد صامتًا واتّسعت الفجوة. الإشارات داخل التعليقات لا تُلتقط.
# (EN) A real enumerator: line starts (after comment stripping) with the name,
#      optionally `= value`, then a comma OR the enum's closing `}` — C++ allows
#      the final enumerator without a trailing comma (token.h's actual style), so
#      without `}` a new last-position symbol would silently escape the guard.
_RE_ENUMERATOR = re.compile(
    rf"^\s*({re.escape(KEYWORD_PREFIX)}[A-Z0-9_]+)\s*(?:=[^,}}]*)?\s*[,}}]",
    re.MULTILINE,
)
_RE_LINE_COMMENT = re.compile(r"//[^\n]*")
_RE_BLOCK_COMMENT = re.compile(r"/\*.*?\*/", re.DOTALL)

# ═══════════════════════════════════════════════════════════════════════════════════
# (AR) قائمة السماح — «الدَّين الموروث» (مثبَّتة بتاريخ 2026-07-11):
#      🔑 **ولا يُنثَرُ عددُها ههنا.** كان مكتوبًا «١٨ رمزًا» والمقيسُ ١٦: خرجَ
#      بندانِ (KEYWORD_ASM وKEYWORD_NORETURN) بتسجيلٍ لغويٍّ وبقيَ النثرُ كما هو —
#      نسخةٌ ثانيةٌ لحقيقةٍ تحتَها مباشرةً لا حارسَ عليها. العددُ يُشتَقُّ من
#      القائمةِ نفسِها، وسقفُه النازلُ في `CEILING_INHERITED_DEBT` وحدَه.
#      رموز KEYWORD_* مُعرَّفة في token.h ولم تُسجَّل بعد في keywords.yaml. الحارس
#      **لا** يضيف كلمات إلى الكتالوج (قرار لغويّ حصريّ لمالكي مصدر الحقيقة —
#      «الأدوات لا توسّع SoT اللغة»)؛ هو يمنع اتّساع الفجوة فقط. عند تسجيل أيّ
#      رمز أدناه في الكتالوج (أو حذفه من token.h) يجب حذف بنده هنا — قائمة تنكمش.
#      كلّ بند: الرمز ⇒ سبب بقائه خارج الكتالوج الآن.
# (EN) Shrink-only inherited-debt allowlist (pinned 2026-07-11): the
#      guard never adds catalog entries (a language decision, owners-only); it
#      only blocks NEW gaps. Cataloging or deleting a symbol requires removing
#      its entry here. Each entry: symbol ⇒ why it is currently uncataloged.
# ═══════════════════════════════════════════════════════════════════════════════════
INHERITED_DEBT: dict = {
    # ── رموز legacy (ينتجها المحلّل/المُعجم تاريخيًّا، لا كلمة كتالوج مباشرة لها) ──
    "KEYWORD_ELSE_IF":   "legacy: يُركَّب من كلمتين منفصلتين (وإلا + إذا) لا كلمة واحدة",
    "KEYWORD_EXTENDS":   "legacy: مرادف تاريخيّ لـKEYWORD_INHERITS (يرث)",
    # ── دفعة «برمجة الأنظمة/الوضع الحرّ» — مُعرَّفة في token.h ولم تُسجَّل بعد ──
    # (م١ RFC اللهجات الأصيلة) KEYWORD_ASM سُجِّل في keywords.yaml (KW-CTX-041 تجميع) — خرج من الدَّين.
    "KEYWORD_SIZEOF":    "دَين موروث: حجم (sizeof) — بانتظار تسجيل لغويّ في الكتالوج",
    "KEYWORD_UNSAFE":    "دَين موروث: غير_آمن (unsafe) — بانتظار تسجيل لغويّ في الكتالوج",
    "KEYWORD_PACKED":    "دَين موروث: محزوم (packed) — بانتظار تسجيل لغويّ في الكتالوج",
    # (اللبنة 3.15) KEYWORD_NORETURN سُجِّل في keywords.yaml (KW-CTX-043 لا_ترجع) — خرج من الدَّين.
    "KEYWORD_NAKED":     "دَين موروث: عارية (naked) — بانتظار تسجيل لغويّ في الكتالوج",
    "KEYWORD_REGISTER":  "دَين موروث: سجل (register) — بانتظار تسجيل لغويّ في الكتالوج",
    "KEYWORD_SECTION":   "دَين موروث: قسم (section) — بانتظار تسجيل لغويّ في الكتالوج",
    # ── دفعة «الملكيّة/الوحدات/المؤشّرات» — مُعرَّفة في token.h ولم تُسجَّل بعد ──
    "KEYWORD_LET":       "دَين موروث: دع (let) — بانتظار تسجيل لغويّ في الكتالوج",
    "KEYWORD_ATOMIC":    "دَين موروث: ذرّي (atomic) — بانتظار تسجيل لغويّ في الكتالوج",
    "KEYWORD_MODULE":    "دَين موروث: وحدة (module) — بانتظار تسجيل لغويّ في الكتالوج",
    "KEYWORD_USE":       "دَين موروث: استخدم (use) — بانتظار تسجيل لغويّ في الكتالوج",
    "KEYWORD_MUT":       "دَين موروث: متغيّر (mut) — بانتظار تسجيل لغويّ في الكتالوج",
    "KEYWORD_CONST_PTR": "دَين موروث: *ثابت (const_ptr) — بانتظار تسجيل لغويّ في الكتالوج",
    "KEYWORD_MUT_PTR":   "دَين موروث: *متغير (mut_ptr) — بانتظار تسجيل لغويّ في الكتالوج",
    "KEYWORD_COMPTIME":  "دَين موروث: وقت_ترجمة (comptime) — بانتظار تسجيل لغويّ في الكتالوج",
}


# (AR) 🔑 **سقفٌ نازلٌ على الدَّين — والقائمةُ «تنكمشُ فقط» دعوى حتّى تُقاس.**
#      الترويسةُ تقولُ ذلك منذُ اليوم الأوّل، ولا شيءَ كان يمنعُ سطرًا جديدًا:
#      رمزٌ غيرُ مُسجَّلٍ يُعلَنُ دَينًا فيخضرُّ الحارسُ في الحال. فالانكماشُ
#      يُقاسُ ولا يُوعَدُ به — ورفعُ السقفِ قرارٌ صريحٌ يُرى في الفرق.
CEILING_INHERITED_DEBT = 16


class ToolFault(Exception):
    """(AR) عطبُ آلةٍ: الحارسُ لم يقرأْ مرجعًا — لا حكمَ له على المحتوى (رمز ٢)."""


def _utf8_console() -> None:
    os.environ.setdefault("PYTHONUTF8", "1")
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            try:
                stream.reconfigure(encoding="utf-8", errors="replace")
            except Exception:
                pass


def extract_header_keywords(header: Path = TOKEN_HEADER) -> set:
    """(AR) عناصر KEYWORD_* المُعرَّفة فعليًّا في تعداد token.h (التعليقات مُسقَطة)."""
    if not header.is_file():
        raise ToolFault("ترويسةُ الرموزِ مفقودة: %s" % header)
    text = header.read_text(encoding="utf-8", errors="replace")
    text = _RE_BLOCK_COMMENT.sub("", text)
    text = _RE_LINE_COMMENT.sub("", text)
    return set(_RE_ENUMERATOR.findall(text))


def extract_catalog_keywords(catalog: Path = KEYWORDS_CATALOG) -> set:
    """(AR) كلّ tokenType: KEYWORD_* مُسجَّل في keywords.yaml (مشي YAML لا regex)."""
    if not catalog.is_file():
        raise ToolFault("كتالوجُ المفرداتِ مفقود: %s" % catalog)
    try:
        data = yaml.safe_load(catalog.read_text(encoding="utf-8")) or {}
    except (yaml.YAMLError, UnicodeDecodeError) as exc:
        raise ToolFault("كتالوجُ المفرداتِ لا يُحلَّلُ YAML: %s"
                        % exc.__class__.__name__)
    found: set = set()

    def walk(node) -> None:
        if isinstance(node, dict):
            tt = node.get("tokenType")
            if isinstance(tt, str) and tt.startswith(KEYWORD_PREFIX):
                found.add(tt)
            for v in node.values():
                walk(v)
        elif isinstance(node, list):
            for item in node:
                walk(item)

    walk(data)
    return found


def _tool_fault(message: str) -> int:
    print("❌ [token_catalog] عطبُ آلة: %s — لم يُقَسْ شيء" % message,
          file=sys.stderr)
    return 2


def run_check() -> int:
    try:
        header_kws = extract_header_keywords()
        catalog_kws = extract_catalog_keywords()
    except ToolFault as exc:
        return _tool_fault(str(exc))
    debt = set(INHERITED_DEBT)

    # (AR) 🔑 **والعمى يُقالُ أوّلًا وبرمزِ ٢.** كان الاستخراجُ الفارغُ يُلحَقُ
    #      ببقيّةِ الشكاوى **بعدَها** فيخرجُ برمزِ ١ — و`x.py` يقرؤه حكمًا على
    #      المحتوى. والأسوأُ أنّ تشخيصَه يغرقُ: ترويسةٌ لا تُقرأُ تجعلُ
    #      `catalog - header` كلَّ الكتالوجِ فتُطبَعُ ثمانونَ سطرَ «رمزٌ شبح»
    #      عن رموزٍ سليمة — رفضٌ كاذبٌ يوجّهُ المُصلِحَ إلى الملفِّ الخطأ.
    if not header_kws:
        return _tool_fault("لم يُستخرَجْ أيُّ %s* من %s — تغيّرَ شكلُ التعداد؟"
                           % (KEYWORD_PREFIX, TOKEN_HEADER.name))
    if not catalog_kws:
        return _tool_fault("لم يُستخرَجْ أيُّ tokenType من %s — تغيّرَ المخطّط؟"
                           % KEYWORDS_CATALOG.name)

    problems: list = []

    # ⓪ (AR) الدَّينُ ينكمشُ ولا ينمو — والدعوى تُقاسُ بسقفٍ نازل.
    if len(debt) > CEILING_INHERITED_DEBT:
        problems.append(
            f"الدَّينُ الموروثُ {len(debt)} فوقَ السقفِ {CEILING_INHERITED_DEBT} — "
            "القائمةُ تنكمشُ فقط؛ سجِّلِ الرمزَ في الكتالوجِ بدلَ إعلانِه دَينًا، "
            "أو ارفعِ السقفَ بقرارٍ صريحٍ يُرى في الفرق."
        )

    # ① (AR) رموز جديدة غير مُسجَّلة وغير مُعلَنة دَينًا — الفشل الأساسيّ (اتّساع الفجوة).
    new_gap = sorted(header_kws - catalog_kws - debt)
    for kw in new_gap:
        problems.append(
            f"رمز جديد في token.h غير مُسجَّل في الكتالوج: {kw} — سجّله في "
            f"language-truth/keywords.yaml (قرار لغويّ) أو أعلِنه دَينًا موثَّقًا في "
            f"INHERITED_DEBT مع سبب."
        )

    # ② (AR) بنود دَين بائتة — سُجِّلت في الكتالوج أو حُذفت من token.h ⇒ احذفها (انكماش).
    for kw in sorted(debt & catalog_kws):
        problems.append(
            f"بند دَين بائت: {kw} أصبح مُسجَّلًا في الكتالوج — احذفه من INHERITED_DEBT "
            f"(القائمة تنكمش فقط)."
        )
    for kw in sorted(debt - header_kws):
        problems.append(
            f"بند دَين بائت: {kw} لم يعُد مُعرَّفًا في token.h — احذفه من INHERITED_DEBT."
        )

    # ③ (AR) رموز شبح: الكتالوج يشير إلى KEYWORD_* غير موجود في token.h.
    for kw in sorted(catalog_kws - header_kws):
        problems.append(
            f"رمز شبح في الكتالوج: {kw} غير مُعرَّف في token.h — صحّح tokenType أو "
            f"أضِف الرمز إلى التعداد."
        )

    if problems:
        print("❌ [token_catalog --check] فجوة/انجراف بين token.h وكتالوج المفردات:",
              file=sys.stderr)
        for p in problems:
            print(f"   • {p}", file=sys.stderr)
        return 1

    print(f"✓ [token_catalog] token.h: {len(header_kws)} {KEYWORD_PREFIX}* · "
          f"الكتالوج: {len(catalog_kws)} · دَين موروث مُعلَن: {len(debt)} "
          f"(السقف {CEILING_INHERITED_DEBT}) — لا فجوة جديدة.")
    return 0


def list_gap() -> int:
    """(AR) يطبع الفجوة الحاليّة كاملة (للتقارير ولصيانة قائمة الدَّين)."""
    try:
        header_kws = extract_header_keywords()
        catalog_kws = extract_catalog_keywords()
    except ToolFault as exc:
        return _tool_fault(str(exc))
    gap = sorted(header_kws - catalog_kws)
    print(f"token.h: {len(header_kws)} · الكتالوج: {len(catalog_kws)} · "
          f"الفجوة: {len(gap)}")
    for kw in gap:
        mark = "دَين مُعلَن" if kw in INHERITED_DEBT else "⚠️ غير مُعلَن"
        print(f"  {kw:<22} [{mark}] {INHERITED_DEBT.get(kw, '')}")
    return 0


def main() -> int:
    _utf8_console()
    ap = argparse.ArgumentParser(
        description="حارس اكتمال كتالوج المفردات (token.h ↔ keywords.yaml)")
    ap.add_argument("--list-gap", action="store_true",
                    help="اطبع فجوة الدَّين الموروث الحاليّة كاملة")
    args = ap.parse_args()
    if args.list_gap:
        return list_gap()
    return run_check()


if __name__ == "__main__":
    sys.exit(رمز_الخروج(main))
