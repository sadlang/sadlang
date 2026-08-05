#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ======================================================================
# (AR) حارس «لا نصَّ تشخيصٍ خامّ في المحلّل النحويّ».
#      كلّ رسالة خطأ/تحذير يجب أن تُركَّب مركزيًّا من كتالوج الأخطاء المولَّد من
#      language-truth/errors/*.yaml عبر errorCatalog/errorCatalogExpected/warnCatalog،
#      لا أن تُكتب حرفيًّا في الكود. النصّ المكتوب يدويًّا يفقد رمز الخطأ، فلا يراه
#      sad-check --json ولا الـLSP، ولا يُترجَم ولا يخضع لمستوى الشرح.
#
#      ⭐ لا يبحث عن نصّ عربيّ: أحد المواضع مكتوب بايتات سداسيّة ("\xD9\x85...")
#      هربًا من مشكلة ترميز، فيُفلت من أيّ بحث حرفيّ. الحارس يفحص **البنية**:
#      وسيط سلسلةٍ حرفيّةٍ يبلغ مصرفًا تشخيصيًّا — أيًّا كان ترميز محتواه.
#
#      نمط الدَّين: يبدأ بخطّ أساسٍ مُعلَن (BASELINE) بالمواضع المعروفة الباقية،
#      فيفشل على **الجديد** فقط. كلّما كُنِس موضع أُنقص من خطّ الأساس، ولا يُزاد
#      إليه أبدًا — هو سقف ينحدر إلى الصفر لا رصيد يُسحب منه.
#
# (EN) Guard: no hand-written diagnostic text in the parser. Every message must be
#      rendered centrally from the SoT-generated error catalog. Structural (not
#      text-based) so hex-escaped literals cannot hide. Ratchet baseline: fails on
#      new violations only; the baseline may shrink, never grow.
# ======================================================================
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCOPE = ROOT / "shared" / "parser" / "src"

# (AR) المصارف التشخيصيّة التي يُمنع أن تبلغها سلسلةٌ حرفيّة.
#      errorCatalog* و warnCatalog مستثناة: وسائطها معرّفات رموز وحشوات، لا نثر.
#      errorWithFixIt وسيطه الأوّل رسالةٌ نصّيّة كذلك، فهو مصرفٌ حقيقيّ يجب حراسته.
SINKS = ("error", "errorAt", "warn", "warnAt", "errorWithFixIt")

# (AR) نداء مصرف يعقبه — بعد أقواس/فراغ/أسطر — سلسلةٌ حرفيّة (تُلتقط بمحتواها).
_CALL = re.compile(
    r'(?<![A-Za-z0-9_])(?:' + "|".join(SINKS) + r')\s*\(\s*R?"((?:\\.|[^"\\])*)"'
)

# (AR) ‏consume(نوع، "رسالة") — المصرف الذي كُنِست منه ٩٧ رسالة. وسيطُه الثاني هو
#      النثر، فلا يلتقطه _CALL (يفحص الوسيط الأوّل). كلّ مواضعه اليوم "" فارغة،
#      فإضافتُه لا تُنمّي خطّ الأساس البتّة — لكنّها تمنع عودةَ ما كُنِس. بدونه كان
#      الحارس يحرس كلَّ شيءٍ إلّا البابَ الذي دخل منه الدَّين أصلًا.
# (EN) consume(type, "msg") — the very sink the sweep emptied. The prose is the
#      SECOND argument, so _CALL (first-arg only) never saw it. All sites are "" today,
#      so this adds nothing to the baseline while blocking the regression it was
#      written to prevent.
_CONSUME = re.compile(
    r'(?<![A-Za-z0-9_])consume\s*\(\s*[^,()]*(?:\([^()]*\))?[^,()]*,\s*R?"((?:\\.|[^"\\])*)"'
)
# (AR) std::cerr << "..."  — لافتة مطبوعة يدويًّا بلا تسجيل مركزيّ.
_CERR = re.compile(r'std::cerr\s*<<\s*(?:\n\s*)?"((?:\\.|[^"\\])*)"')

# (AR) تجريد التعليقات وسلاسل الأحرف حتّى لا تُحسب أمثلةُ التوثيق خرقًا.
#      ⚠ الفرع الأوّل (سلسلةٌ مزدوجة) لا يُجرَّد بل يُترك كما هو: بغيره كان
#      «//» داخل سلسلةٍ (مثل "انظر http://…") يُقرأ تعليقًا فيُمحى بقيّةُ السطر
#      ويُفلت الخرق. ترتيبُ البدائل هو الحارس: السلسلةُ تُلتقط قبل «//».
_STRIP = re.compile(r'"(?:\\.|[^"\\])*"|//[^\n]*|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'', re.S)


def _blank(m):
    """(AR) يمحو التعليقَ/سلسلةَ الأحرف مبقيًا الأسطر، ويُبقي السلاسلَ المزدوجة."""
    s = m.group(0)
    if s.startswith('"'):
        return s
    return re.sub(r'[^\n]', ' ', s)

# (AR) بايتات سداسيّة داخل السلسلة — بها كُتب نصٌّ عربيٌّ هربًا من مشكلة ترميز.
_HEX = re.compile(r'\\x([0-9A-Fa-f]{2})')


def is_prose(literal: str) -> bool:
    """(AR) هل السلسلة نثرٌ موجَّهٌ للقارئ؟ الفواصلُ والرموزُ التزيينيّة ليست نثرًا.

    يفكّ ‎\\xNN إلى بايتاتها ثمّ يفكّ UTF-8، فيلتقط النصّ العربيّ المُرمَّز سداسيًّا
    كما يلتقط المكتوب مباشرةً. المعيار: وجود حرفٍ هجائيٍّ واحدٍ فأكثر.
    """
    had_hex = bool(_HEX.search(literal))
    raw = _HEX.sub(lambda m: chr(int(m.group(1), 16)), literal)
    # (AR) إسقاط ما تبقّى من محارف الهروب (\n \t \" \\) — حرف 'n' فيها ليس نثرًا.
    raw = re.sub(r'\\.', '', raw)
    # (AR) فكُّ UTF-8 يخصّ المكتوبَ بايتاتٍ سداسيّةً وحده. تطبيقُه على نصٍّ عربيٍّ
    #      مكتوبٍ مباشرةً كان يمحوه (latin-1 + ignore لا يمثّل ما فوق U+00FF)
    #      فيُفلت أيُّ تشخيصٍ عربيٍّ خالصٍ لا حرفَ لاتينيًّا فيه.
    if had_hex:
        try:
            raw = raw.encode("latin-1", "ignore").decode("utf-8", "ignore")
        except Exception:
            pass
    return any(ch.isalpha() for ch in raw)

# ──────────────────────────────────────────────────────────────────────
# (AR) خطّ الأساس: الدَّين المعروف يوم كتابة الحارس. مسموحٌ إنقاصه لا زيادته.
#      المفتاح مسار نسبيّ، والقيمة عدد المواضع المتبقّية في الملفّ.
# (EN) Baseline: known debt at guard-authoring time. May shrink, never grow.
# ──────────────────────────────────────────────────────────────────────
BASELINE = {
    # (AR) لافتاتُ «{ } في الطرق/الباني» ورسالةُ استدعاء الأساس — نثرٌ بلا رمز خطأ.
    "shared/parser/src/declarations/parser_oop.cpp": 3,
    # (AR) 1 رسالةُ «بعد كلمة النوع…» + 3 آثارِ تنقيحٍ إنجليزيّةٍ محروسةٍ بمتغيّر بيئة.
    "shared/parser/src/core/parser_main.cpp": 4,
    # (AR) ✅ كُنِس: «حالة» داخل «طابق» كانت تُطبَع بلا رمزٍ فلا يراها sad-check --json
    #      ولا الـLSP. صارت SYN031 بحشواتٍ كلُّها تهجئةُ معجمٍ من kw().
    "shared/parser/src/statements/parser_advanced.cpp": 0,
    # (AR) «متوقع اسم متغير بعد &» — مكتوبةٌ بايتاتٍ سداسيّةً فتُفلت من البحث النصّيّ.
    "shared/parser/src/core/parser_expressions.cpp": 1,
    # (AR) تلميحاتُ الاسترداد («🔧 هل تقصد…»، «✓ تمّ الاسترداد…») — تحتاج صنفَ
    #      «ملاحظة» في الكتالوج قبل أن تُكنَس، فهي ليست أخطاءً ولا تحذيرات.
    "shared/parser/src/core/parser_helpers.cpp": 10,
    # (AR) ١٩ رسالةَ consume متعدّدةَ الأسطر. (كانت ٢٠: كُنِست لافتةُ تحذيرِ
    #      الودجات المُهمَلة مع حذفِ خريطةِ المُهمَل كلِّها — لا توافقَ خلفيًّا.)
    #      ⚠ الرقم كان «1» قبل حراسة consume — لا لأنّ الدَّين ازداد بل لأنّ الحارسَ
    #      كان أعمى عن المصرف. الملفّ لم يُكنَس أصلًا؛ كنسُه يحتاج رفعَ رسائلِ
    #      consume إلى كتالوج الأخطاء (لا كتالوجَ ودجاتٍ ثانيًا — مصدرُ أسماءِ
    #      العُقَد صار language-truth/ui_nodes.yaml وحدَه).
    "shared/parser/src/ui/parser_ui.cpp": 19,
}


def scan(text: str) -> list:
    """(AR) يُعيد قائمة (رقم السطر، مقتطف) لكلّ سلسلة حرفيّة تبلغ مصرفًا تشخيصيًّا."""
    # (AR) نُبقي أطوال النصّ كما هي (نستبدل بفراغات) ليظلّ رقم السطر صحيحًا.
    clean = _STRIP.sub(_blank, text)
    hits = []
    for rx in (_CALL, _CONSUME, _CERR):
        for m in rx.finditer(clean):
            if not is_prose(m.group(1)):
                continue  # فاصلٌ أو رمزٌ تزيينيّ لا نثرَ فيه
            ln = clean.count("\n", 0, m.start()) + 1
            snip = text[m.start():m.start() + 70].replace("\n", " ").strip()
            hits.append((ln, snip))
    return sorted(hits)


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    if not SCOPE.is_dir():
        print("حارس نصّ المحلّل: المجلّد غير موجود: %s" % SCOPE)
        return 2

    files = sorted(SCOPE.rglob("*.cpp"))
    found = {}
    for f in files:
        rel = f.relative_to(ROOT).as_posix()
        hits = scan(f.read_text(encoding="utf-8", errors="surrogateescape"))
        if hits:
            found[rel] = hits

    print("حارس «لا نصّ تشخيص خامّ في المحلّل النحويّ»:")
    print("  النطاق:   %s" % SCOPE.relative_to(ROOT).as_posix())
    print("  الملفّات: %d" % len(files))

    regressions, improvements = [], []
    for rel, hits in sorted(found.items()):
        allowed = BASELINE.get(rel, 0)
        if len(hits) > allowed:
            regressions.append((rel, allowed, hits))
        elif len(hits) < allowed:
            improvements.append((rel, allowed, len(hits)))
    for rel, allowed in sorted(BASELINE.items()):
        if rel not in found and allowed:
            improvements.append((rel, allowed, 0))

    total_debt = sum(len(h) for h in found.values())
    print("  الدَّين الحاليّ: %d موضعًا في %d ملفًّا (خطّ الأساس %d)"
          % (total_debt, len(found), sum(BASELINE.values())))

    if regressions:
        print("  ✗ انحدار (%d ملفًّا) — رسالةٌ جديدةٌ مكتوبةٌ في الكود:" % len(regressions))
        for rel, allowed, hits in regressions:
            print("     %s: %d موضعًا والمسموح %d" % (rel, len(hits), allowed))
            for ln, snip in hits[allowed:]:
                print("        :%d  %s" % (ln, snip))
        print("  الصواب: errorCatalog(Errors::ErrorCode::SYN_xxx, {{...}}) — "
              "والرموز من language-truth/errors/*.yaml.")
        return 1

    if improvements:
        print("  ⚠ كُنِست مواضعُ ولم يُنقَص خطّ الأساس — حدّثْ BASELINE في هذا الملفّ:")
        for rel, allowed, now in improvements:
            print("     %s: %d ⇐ %d" % (rel, now, allowed))
        return 1

    if total_debt:
        print("  ✓ لا انحدار — الدَّين المتبقّي ضمن خطّ الأساس المُعلَن (وهو سقفٌ ينحدر).")
    else:
        print("  ✓ سليم — لا نصّ تشخيص خامّ في المحلّل إطلاقًا.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
