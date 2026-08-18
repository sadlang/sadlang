#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) **بوّابةٌ تمنع صنفَ ISSUE-150: نصُّ ص مكتوبٌ بصيغةٍ لا تقبلها اللغةُ اليوم.**

     المقيسُ الذي وُلِدت منه (٢٠٢٦-٠٨-١٧): مِجَسُّ
     `tests/system/lowlevel_freestanding/test_bounds_diag_sovereign.py` كان
     يكتب مصدرَه `متغير س: رقم؟ = لاشيء` — صيغةٌ حُذِفت من اللغةِ في `f56baa6c`
     وتُثبِّت رفضَها بذرةُ `VE053`. فصار المِجَسُّ يُخفِق بـSYN001، **وثلاثةُ
     حرّاسٍ لعقدِ رموزِ الهلع صاروا غيرَ مقيسين** — لا لأنّ العقدَ انكسر بل لأنّ
     **مصدرَ المِجَسِّ لم يعد يُصرَّف**.

     🔑 وأخطرُ الثلاثةِ `test_panic_reason_codes_are_distinct`: لا توكيدَ فيه
     على رمزِ الخروجِ أصلًا (يُهمِله بـ`_`)، فيسقط على
     `assert '@__sad_panic(i64 2)' in ''` برسالةِ **«التأكيد يجب أن يمرّر رمز 2
     حصرًا»** — أي يُقرأ انحدارًا في عقدِ الهلعِ وهو انحدارٌ في **صيغةِ
     الاختبار**. وأخواه يُخفقان على `assert code == 0` فتحمل رسالتُهما نصَّ
     SYN001، فيُقرآن على وجهِهما. ⇒ **صخبُ الإخفاقِ رهنُ ترتيبِ التوكيدات**، فلا
     يصحّ الاتّكالُ عليه.

     ⚠️ ولا يراهم أيُّ حارسٍ قائم: مسحُ `tests/behavior` يفحص ملفّاتِ `.ص`،
     وهؤلاء نصوصُ ص **داخلَ ملفّات بايثون** ولا يقع عليها نحوٌ البتّةَ. وكذلك
     الصيغُ المكتوبةُ في **التعليقات**: `صنف س { ... }` بأقواسٍ معقوفةٍ ليست
     لغةَ ص (الكتلةُ تُغلَق بـ`نهاية`)، ومع ذلك تسرّبت إلى تعليقٍ في المصرِّفِ
     وإلى صفٍّ في سجلِّ العيوبِ وإلى ترويسةِ بذرةٍ — تُعلِّم قارئَها نحوًا كاذبًا
     ولا يُخفِق عليها شيءٌ لأنّها لا تُصرَّف.

     الفحصان:

       ① **مِجَسّاتُ بايثون** — تُستخرَج سلاسلُ ص من كلِّ `*.py` تحت `tests/`
          وتُمرَّر على المصرِّف. يُخفِق الفحصُ إن ظهر تشخيصٌ نحويٌّ `SYNxxx`.
          ويُحكَم بالتشخيصِ لا برمزِ الخروج: رمزُ الخروجِ يخلط النحوَ بالدلالةِ
          وبعجزِ المنصّة، والمقصودُ هنا النحوُ وحدَه.

       ② **صيغٌ محذوفةٌ أو لم توجد قطُّ** — جدولُ أنماطٍ يُمسَح على التعليقاتِ
          والوثائقِ والمصادر. يُضاف إليه صفٌّ **يومَ تُحذَف صيغةٌ**، فلا يبقى
          الحذفُ مسؤوليّةَ ذاكرةِ من حذف.

     ⚠️ والاستثناءُ صريحٌ لا صامت: مِجَسٌّ يُقصَد به الرفضُ يُعلَّم بـ
     `@صيغة-مرفوضة` في تعليقٍ فوقَه، وسطرٌ في الوثائقِ يقصد عرضَ المرفوضِ
     يُعلَّم بالعلامةِ نفسِها في السطرِ أو السطرِ الذي قبلَه. فيبقى كلُّ تجاوزٍ
     **مكتوبًا** يُقرأ في المراجعة، لا قائمةً في الحارسِ لا يمرّ عليها أحد.

(EN) Guard against the ISSUE-150 class: Sad source written in syntax the
     language no longer (or never) accepts. Two checks: (1) extract Sad source
     strings from every tests/**/*.py and compile them, failing on any SYNxxx
     diagnostic — judged by diagnostic, not exit code, since the exit code
     conflates syntax with semantics and platform gaps; (2) a pattern table of
     deleted / never-existing forms scanned over comments, docs and sources.
     Both honour an explicit `@صيغة-مرفوضة` opt-out marker so every waiver is
     written where a reviewer reads it.
============================================================================
"""
from __future__ import annotations

import argparse
import ast
import concurrent.futures
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def _اجعل_المخرج_يونيكود() -> None:
    """(AR) يُثبِّت ترميزَ المخرَجِ على UTF-8 قبل أوّلِ طباعة.

    (AR) 🔑 **العلّةُ المقيسة**: على مُشغِّلِ ويندوز في CI يكون ترميزُ stdout
         الافتراضيُّ `cp1252`، فأوّلُ حرفٍ عربيٍّ أو `⚠️` يرفع
         `UnicodeEncodeError` ⇒ خروجٌ ١ ⇒ **البوّابةُ تسقط قبلَ أن تقولَ شيئًا**.
         وقعَ فعلًا: الرسالةُ التي كانت تُطبَع هي إعلانُ التخطّي «sad-build غير
         مبنيّ» — أي أنّ البوّابةَ انهارت وهي تُحاول أن تكون **صريحةً** في أنّها
         لم تقِس. فصار التخطّي المُعلَنُ إخفاقًا مُبهَمًا، وهو أسوأُ الاثنين.

    (AR) ⚠️ ولمَ هنا لا في سيرِ العمل: ضبطُ `PYTHONIOENCODING` في خطوةٍ واحدةٍ
         يُصلِحُ تلك الخطوةَ وحدَها ويترك السكربتَ هشًّا لكلِّ مُنادٍ آخر —
         محلّيًّا أو في بوّابةٍ لاحقة. والعقدُ أن يطبعَ السكربتُ عربيّتَه أينما
         شُغِّل، فيُحمَل في السكربتِ نفسِه.
    """
    for تيار in (sys.stdout, sys.stderr):
        إعادة_الضبط = getattr(تيار, "reconfigure", None)
        if إعادة_الضبط is not None:
            إعادة_الضبط(encoding="utf-8", errors="replace")


_اجعل_المخرج_يونيكود()

# (AR) علامةُ التجاوزِ الصريحة — واحدةٌ للفحصَين كي لا يتعدّد الاصطلاح.
WAIVER = "@صيغة-مرفوضة"

# (AR) تجاوزٌ على مستوى الملفِّ كلِّه، يُكتَب في ترويستِه. لزِمَ لأنّ بعضَ الملفّاتِ
#      **وظيفتُها** وصفُ المرفوض — كـ`20_declarations.yaml` الذي يشرح الصيغةَ
#      المحذوفةَ وثمنَ حذفِها. ⚠️ ولأنّ وصفَه كتلةٌ نصّيّةٌ في YAML (`>`)، فأيُّ
#      «تعليقٍ» يُكتَب داخلَها **يصير جزءًا من النصّ** ويتسرّب إلى الوثائقِ
#      المولَّدة — قِيس ذلك. فالتجاوزُ السطريُّ لا يصلح هناك أصلًا.
WAIVER_FILE = "@صيغة-مرفوضة-الملف"
WAIVER_FILE_WINDOW = 20

# (AR) رمزُ التشخيصِ النحويّ. يُطابَق على الرمزِ لا على النصِّ: النصُّ ثنائيُّ
#      اللغةِ ويتبدّل، والرمزُ عقدٌ في `language-truth/errors`.
SYNTAX_DIAG = re.compile(r"\bSYN\d{3}\b")

# ═════════════════════════════════════════════════════════════════════
# (AR) ① استخراجُ مقاطعِ ص من ملفّات بايثون
# ═════════════════════════════════════════════════════════════════════

# (AR) دلالةُ «هذه سلسلةُ مصدرِ ص»: كلمةُ تصريحٍ أو جملةٍ في **بدايةِ سطر**.
#      البدايةُ شرطٌ لازمٌ: بدونها تُلتقَط رسائلُ التشخيصِ وأسماءُ الأنواعِ في
#      نصوصِ التوكيد، فيصير الحارسُ يُصرِّف نثرًا عربيًّا ويُخفِق عليه.
_STATEMENT_WORDS = (
    "دالة", "صنف", "بنية", "تعداد", "واجهة", "متغير", "ثابت",
    "استورد", "اطبع_سطر", "اطبع", "ارجع", "إذا", "بينما", "لكل",
    "رقم", "نص", "منطقي", "عشري", "أي", "مصفوفة", "خريطة",
)
SAD_SOURCE = re.compile(
    r"(?:^|\n)[ \t]*(?:" + "|".join(_STATEMENT_WORDS) + r")(?:[ \t(]|$)"
)

# (AR) قوالبُ المولّدات: سلسلةٌ فيها موضعُ استبدالٍ ليست مصدرًا صالحًا بذاتِها.
#      تُستثنى **بالوصفِ لا بالمسار** كي يشملَ الاستثناءُ كلَّ مولّدٍ قادم.
TEMPLATE_HOLE = re.compile(r"%[sdr]|\{\w*\}|\{\{|\$\{")


# (AR) المولّداتُ تُستثنى: سلاسلُها **شذراتٌ** تُركَّب في ملفّاتِ `.ص` — ومخرَجُها
#      هو المقيسُ في مصفوفةِ السلوك. فحصُ الشذرةِ منفردةً يُخفِق بداهةً ويُغرِق
#      البلاغَ بضجيجٍ لا يدلّ على عطب.
PY_SKIP_DIRS = {"__pycache__", ".git", "_generators"}


def _iter_python_files(root: Path):
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in PY_SKIP_DIRS]
        for name in sorted(filenames):
            if name.endswith(".py"):
                yield Path(dirpath) / name


def _literal_text(node: ast.AST, known: dict[str, str]) -> str | None:
    """(AR) يحلّ الحرفيّةَ وضمَّها: `س = "أ" + جسد` نمطٌ شائعٌ في المِجَسّات."""
    if isinstance(node, ast.Constant) and isinstance(node.value, str):
        return node.value
    if isinstance(node, ast.Name):
        return known.get(node.id)
    if isinstance(node, ast.BinOp) and isinstance(node.op, ast.Add):
        left = _literal_text(node.left, known)
        right = _literal_text(node.right, known)
        return None if left is None or right is None else left + right
    return None


def _waived(lines: list[str], lineno: int, look_back: int = 6) -> bool:
    """(AR) هل عُلِّم المقطعُ بالتجاوزِ الصريحِ في الأسطرِ القليلةِ فوقَه؟"""
    start = max(0, lineno - 1 - look_back)
    return any(WAIVER in ln for ln in lines[start:lineno])


def collect_snippets(root: Path) -> tuple[list[tuple[Path, int, str]], int]:
    """(AR) يُعيد (المقاطعَ المرشَّحة، عددَ المُتجاوَزِ عنها صراحةً)."""
    snippets: list[tuple[Path, int, str]] = []
    waived = 0
    for path in _iter_python_files(root):
        text = path.read_text(encoding="utf-8", errors="replace")
        lines = text.splitlines()
        try:
            tree = ast.parse(text)
        except SyntaxError:
            # (AR) ملفُّ بايثون المكسورُ ليس شأنَ هذا الحارس، ولا يُبتلَع صامتًا.
            print(f"⚠️  تعذّر تحليلُ ملفِّ بايثون (خارجَ نطاقِ هذا الحارس): {path}")
            continue
        known: dict[str, str] = {}
        assigns = sorted(
            (n for n in ast.walk(tree) if isinstance(n, ast.Assign)),
            key=lambda n: (n.lineno, n.col_offset),
        )
        for node in assigns:
            if len(node.targets) != 1:
                continue
            target = node.targets[0]
            if not isinstance(target, ast.Name):
                continue
            value = _literal_text(node.value, known)
            if value is None:
                continue
            known[target.id] = value
            if "\n" not in value or not SAD_SOURCE.search(value):
                continue
            if TEMPLATE_HOLE.search(value):
                continue
            if _waived(lines, node.lineno):
                waived += 1
                continue
            snippets.append((path, node.lineno, value))
    return snippets, waived


def _مواضع_المصرف() -> list[Path]:
    """(AR) المواضعُ التي يُبحَث فيها عن `sad-build` — مُفرَدةً كي تُطبَع عند الإخفاق.

    (AR) 🔑 «غيرُ مبنيّ» وحدَها لا تكفي: قِيسَ أنّ الإخفاقَ على ويندوزِ CI سببُه
         أنّ هدفَ `sad-build` **لا يُبنى هناك أصلًا** (مثبّتُ LLVM لويندوز بلا
         مكتباتِ تطوير ⇒ MSB1009، والخطوةُ غيرُ قاتلةٍ عمدًا؛ وفي Debug تُتخطّى
         لأنّها Release-only). فطباعةُ المواضعِ تجعل السببَ **مقروءًا من السجلّ**
         لا مُستنتَجًا بعد تحقيق.
    """
    مواضع: list[Path] = []
    for sub in ("Debug", "Release", ""):
        for name in ("sad-build.exe", "sad-build"):
            مواضع.append(ROOT / "build" / "bin" / sub / name if sub
                         else ROOT / "build" / "bin" / name)
    return مواضع


def _find_compiler() -> Path | None:
    for candidate in _مواضع_المصرف():
        if candidate.exists():
            return candidate
    return None


def _syntax_of(compiler: Path, source: str) -> str:
    """(AR) يُصرّف المقطعَ ويُعيد التشخيصَ النحويَّ إن وُجِد، وإلّا سلسلةً فارغة."""
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "مقطع.ص"
        src.write_text(source, encoding="utf-8")
        out = Path(work) / "مقطع.ll"
        try:
            proc = subprocess.run(
                # (AR) «--بلا-رئيسية» كي لا يُخلَط غيابُ الدالّةِ الرئيسةِ بالنحو،
                #      و«--أظهر-llvm» لأنّ المقاطعَ لا تُراد ثنائيّاتٍ.
                [str(compiler), str(src), "--بلا-رئيسية", "--أظهر-llvm", "-o", str(out)],
                capture_output=True, text=True, encoding="utf-8",
                errors="replace", timeout=180, cwd=work,
            )
        except subprocess.TimeoutExpired:
            return "انتهت المهلة قبل أن يُنهي المصرِّفُ التحليل"
        blob = (proc.stdout or "") + (proc.stderr or "")
        codes = sorted(set(SYNTAX_DIAG.findall(blob)))
        if not codes:
            return ""
        head = next((ln.strip() for ln in blob.splitlines() if SYNTAX_DIAG.search(ln)), "")
        return f"{'، '.join(codes)} · {head[:200]}"


def check_python_probes(jobs: int) -> tuple[int, list[str]]:
    snippets, waived = collect_snippets(ROOT / "tests")
    compiler = _find_compiler()
    if compiler is None:
        # (AR) تخطٍّ **مرئيّ**: تخطٍّ صامتٌ يُقرأ نجاحًا، وهو العطبُ الذي نحرسه.
        print("⚠️  الفحصُ ① لم يُشغَّل: sad-build غير مبنيّ "
              "(cmake --build build --target sad-build)")
        print(f"    المرشَّحون: {len(snippets)} مقطعًا · المُتجاوَزُ عنه صراحةً: {waived}")
        for موضع in _مواضع_المصرف():
            print(f"    ✗ {موضع}")
        # (AR) وسمٌ يقرؤه GitHub فيظهر على الطلبِ نفسِه: تخطٍّ في السجلِّ وحدَه
        #      يمرُّ على مَن يقرأ الأخضرَ ولا يفتح ٤٠٠ سطرٍ من السجلّ.
        print("::warning title=بوّابة مقاطع ص::الفحص ① لم يُقَس على هذه المنصّة — "
              "sad-build غير مبنيّ")
        return 0, []

    failures: list[str] = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
        futures = {
            pool.submit(_syntax_of, compiler, text): (path, lineno)
            for path, lineno, text in snippets
        }
        for future in concurrent.futures.as_completed(futures):
            path, lineno = futures[future]
            diag = future.result()
            if diag:
                rel = path.relative_to(ROOT).as_posix()
                failures.append(f"{rel}:{lineno} · {diag}")

    print(f"① مِجَسّاتُ بايثون: {len(snippets)} مقطعًا · "
          f"مُتجاوَزٌ عنه صراحةً {waived} · مرفوضٌ نحويًّا {len(failures)}")
    return len(snippets), sorted(failures)


# ═════════════════════════════════════════════════════════════════════
# (AR) ② جدولُ الصيغِ المحذوفةِ أو التي لم توجد قطُّ
# ═════════════════════════════════════════════════════════════════════
#
# (AR) 🔑 يُضاف إليه صفٌّ **يومَ تُحذَف صيغةٌ** — وإلّا بقي الحذفُ مسؤوليّةَ
#      ذاكرةِ من حذف، وهو الذي أخفق في `f56baa6c`.

# (AR) معرّفٌ عربيّ. اشتراطُه بين المُفتَتحِ والقوسِ شرطٌ **لازمٌ للدقّة**: بدونه
#      يلتقط النمطُ نثرًا مثل «بنية `examples/{name}.yaml`» — و«بنية» و«واجهة»
#      اسمانِ عربيّانِ شائعانِ قبل أن يكونا كلمتَين مفتاحيّتَين. قِيس أنّ النمطَ
#      المرسَلَ يُخرِج ١٩٢٦ بلاغًا جُلُّها نثر، وبلاغٌ بهذه النسبةِ من الضجيجِ
#      يُدرَّب القارئُ على تخطّيه فيصير غيابُه أنفعَ من وجودِه.
_ARABIC_NAME = r"[ء-ي_][ء-ي٠-٩\w_]*"

# (AR) قوسٌ معقوفٌ ليس موضعَ استبدالٍ في قالبِ رسالة: «صنف {name} ... نهاية» في
#      `language-truth/errors` نصُّ تشخيصٍ لا نحوٌ، و«{0}» كذلك.
_NOT_PLACEHOLDER = r"\{(?![A-Za-z_]\w*\}|\d+\})"

# (AR) ⚠️ يُشترَط أن يلي النقطتَين **كلمةُ نوعٍ** لا أيُّ لفظٍ عربيّ: «متغير
#      متطاير: كلّ الوصول volatile» عنوانُ قسمٍ في تعليق، و«متغير العداد:
#      _iter_counter» وصفُ تسميةٍ داخليّة. قِيس أنّ الاكتفاءَ بحرفٍ عربيٍّ بعد
#      النقطتَين يلتقط الاثنَين.
_TYPE_WORDS = "|".join((
    "رقم", "نص", "منطقي", "عشري", "أي", "مصفوفة", "خريطة",
    "طبيعي", "حرف", "فراغ", "كائن", "مجموعة", "دالة",
))

DEAD_FORMS = [
    (
        "كتلةٌ بأقواسٍ معقوفة",
        # (AR) ⚠️ المعرّفُ **لازمٌ** لا اختياريّ: «بنية {i8، i64}» في تعليقٍ يصف
        #      هيكلَ LLVM بالاسمِ العربيِّ المجرَّد، و«تعداد نتيجة {…} → {i8،…}»
        #      يصف خريطةَ أنواع. اشتراطُ اسمٍ عربيٍّ بين المُفتَتحِ والقوسِ يفصل
        #      **الصيغةَ** عن **الاسمِ العامّ**، وهو الفصلُ الذي بدونه يصير
        #      البلاغُ ثلاثةَ أرباعِه نثرًا.
        re.compile(r"(?:^|[^\wء-ي])(?:صنف|بنية|تعداد|دالة)\s+"
                   + _ARABIC_NAME + r"\s*(?:\([^)\n]*\))?\s*" + _NOT_PLACEHOLDER),
        "الكتلةُ في ص تُغلَق بـ«نهاية» لا بقوسٍ معقوف — والمعقوفُ للخرائطِ "
        "والاستيعاب. صيغةٌ لم توجد قطُّ، وكتابتُها في تعليقٍ تُعلِّم نحوًا كاذبًا.",
    ),
    (
        "كتلةٌ بلا اسمٍ بأقواسٍ معقوفة",
        # (AR) الشكلُ الذي وقعتُ فيه أنا: «صنف { أي ح = 5 }». مقصورٌ على
        #      «صنف» و«دالة» لأنّ «بنية» و«تعداد» يردان اسمَين عامَّين مجرَّدَين.
        re.compile(r"(?:^|[^\wء-ي])(?:صنف|دالة)\s*" + _NOT_PLACEHOLDER),
        "الكتلةُ في ص تُغلَق بـ«نهاية» لا بقوسٍ معقوف.",
    ),
    (
        "شرطٌ أو حلقةٌ بأقواسٍ معقوفة",
        re.compile(r"(?:^|[^\wء-ي])(?:إذا|بينما|لكل)\s*\([^)\n]*\)\s*" + _NOT_PLACEHOLDER),
        "الشرطُ والحلقةُ في ص يُغلَقان بـ«نهاية» لا بقوسٍ معقوف.",
    ),
    (
        "تصريحٌ بنقطتَين (محذوفٌ في f56baa6c)",
        # (AR) يُشترَط أن يبدأ ما بعد النقطتَين بحرفٍ عربيّ: «متغير العداد:
        #      _iter_counter = 0» تعليقٌ يصف تسميةً داخليّة لا تصريحًا. و«ثابت»
        #      خرجت من المُفتَتحات لأنّها صفةٌ عربيّةٌ شائعةٌ («ثابت صعودي: …»).
        re.compile(r"(?:^|[^\wء-ي])متغير\s+" + _ARABIC_NAME
                   + r"\s*:\s*(?:" + _TYPE_WORDS + r")\b"),
        "صيغةُ «متغير س: رقم» حُذِفت من اللغةِ ٢٠٢٦-٠٨-١٦ وتُثبِّت رفضَها بذرةُ "
        "VE053. والباقي هو الصفةُ اللفظيّة: «رقم عدمي س».",
    ),
]

# (AR) نطاقُ الحجب: الأشجارُ التي **تُعرِّف اللغةَ أو تُدلِّل عليها**. وضيقُه
#      مقصودٌ ومقيس، لا تهاونًا:
#
#      · `_bmad-output` وثائقُ تخطيطٍ مؤرشفةٌ تصف صيغًا مقترَحةً لم تُنفَّذ.
#      · `tests/_archive` اختباراتٌ مؤرشفةٌ بلهجةٍ قديمةٍ ماتت كلُّها.
#      · ⚠️ و`runtime/` و`compiler/` و`shared/` و`tools/` **خارجَ الحجبِ اليومَ
#        وفيها دَينٌ حقيقيّ**: قِيس ١٦٠ موضعًا (٢٠٢٦-٠٨-١٧). وسببُ إخراجِها
#        ليس قلّةَ الأهمّيّة بل أنّ الفصلَ فيها يحتاج معرفةَ **لغةِ الملفّ**:
#        `runtime/freestanding/*.h` ماكرواتُ C بأسماءٍ عربيّة، فـ«إذا (شرط) {»
#        فيها **لغةُ C صحيحةٌ** لا نحوُ ص كاذب — واللفظُ واحدٌ في الحالَين.
#        فحجبُها بهذا النمطِ يُخفِق على الصحيحِ، وحارسٌ كذلك يُدرَّب القارئُ على
#        تخطّيه. سُجِّل الأمرُ عيبًا مستقلًّا بدل أن يُبتلَع صامتًا.
SCAN_ROOTS = ("language-truth", "tests", "docs")

SCAN_SUFFIXES = (".ص", ".cpp", ".h", ".hpp", ".md", ".py", ".yaml", ".yml")

# (AR) أشجارٌ لا تُمسَح: مولَّدةٌ أو خارجَ ملكيّةِ المستودع. المولَّدُ يُصحَّح من
#      مصدرِه لا في مخرَجه، فمسحُه يُنتِج بلاغًا لا يُصلَحُ في موضعِه.
SCAN_SKIP_PARTS = {
    ".git", "build", "__pycache__", "node_modules", "_generated",
    "generated", "third_party", "external", "_archive",
}


def _iter_scan_files(root: Path):
    for top in SCAN_ROOTS:
        base = root / top
        if not base.is_dir():
            continue
        for dirpath, dirnames, filenames in os.walk(base):
            dirnames[:] = [d for d in dirnames if d not in SCAN_SKIP_PARTS]
            for name in sorted(filenames):
                if name.endswith(SCAN_SUFFIXES):
                    yield Path(dirpath) / name


# (AR) سجلُّ الدَّينِ المعروف — **سقّاطةٌ لا قائمةُ عفو**:
#
#      · مخالفةٌ ليست فيه ⇒ إخفاق. فالدَّينُ لا ينمو صامتًا.
#      · سطرٌ فيه لم تعُد له مخالفةٌ ⇒ إخفاقٌ أيضًا. فمن سدّ ملزَمٌ بحذفِ سطرِه،
#        ولا يبقى السجلُّ يشهد بدَينٍ مقضيّ. وهذا هو الفرقُ بينه وبين قائمةِ
#        استثناءاتٍ: القائمةُ تُنسى، والسقّاطةُ تُخفِق في الاتّجاهَين.
#
#      ⚠️ والمفتاحُ (المسارُ + نصُّ السطر) لا رقمُ السطر: الرقمُ يزحف بأيِّ تحريرٍ
#      فوقَه فيُخفِق الحارسُ لسببٍ غيرِ الذي وُضِع له — وهو عينُ عطبِ ISSUE-150.
DEBT_LEDGER = Path(__file__).resolve().parent / "data" / "sad_dead_syntax_debt.txt"


def _load_ledger() -> set[str]:
    if not DEBT_LEDGER.exists():
        return set()
    entries = set()
    for raw in DEBT_LEDGER.read_text(encoding="utf-8").splitlines():
        line = raw.strip()
        if line and not line.startswith("#"):
            entries.add(line)
    return entries


def check_dead_forms(record: bool = False) -> tuple[int, list[str]]:
    failures: list[str] = []
    scanned = 0
    seen: set[str] = set()
    detail: dict[str, str] = {}
    self_name = Path(__file__).name
    for path in _iter_scan_files(ROOT):
        if path.name == self_name:
            continue  # (AR) الحارسُ يحمل أنماطَه نصًّا؛ مسحُ نفسِه بلاغٌ عن نفسِه.
        try:
            text = path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        scanned += 1
        lines = text.splitlines()
        if any(WAIVER_FILE in ln for ln in lines[:WAIVER_FILE_WINDOW]):
            continue
        rel = path.relative_to(ROOT).as_posix()
        for label, pattern, why in DEAD_FORMS:
            for index, line in enumerate(lines):
                if not pattern.search(line):
                    continue
                if WAIVER in line or (index and WAIVER in lines[index - 1]):
                    continue
                key = f"{rel}\t{line.strip()[:120]}"
                seen.add(key)
                detail[key] = (f"{rel}:{index + 1} · {label} · {line.strip()[:120]}\n"
                               f"      ⇒ {why}")

    if record:
        DEBT_LEDGER.parent.mkdir(parents=True, exist_ok=True)
        header = (
            "# (AR) سجلُّ دَينِ الصيغِ الميّتة — يُولَّد بـ`--سجّل` ويُقلَّم باليد.\n"
            "#      سطرٌ هنا يعني: مخالفةٌ **معروفةٌ** لم تُسَدّ بعدُ. سدُّها يوجب\n"
            "#      حذفَ سطرِها، وإلّا أخفق الحارسُ على السطرِ البائت.\n"
            "#      الصيغة: <المسار>\\t<نصُّ السطرِ المخالف>\n"
        )
        DEBT_LEDGER.write_text(header + "\n".join(sorted(seen)) + "\n", encoding="utf-8")
        print(f"② سُجِّل الدَّين: {len(seen)} مخالفةً في {DEBT_LEDGER.name}")
        return scanned, []

    ledger = _load_ledger()
    fresh = sorted(seen - ledger)
    stale = sorted(ledger - seen)
    for key in fresh:
        failures.append(detail[key])
    for key in stale:
        path_part = key.split("\t", 1)[0]
        failures.append(
            f"{path_part} · سطرٌ بائتٌ في سجلِّ الدَّين — لم تعُد له مخالفة\n"
            f"      ⇒ سُدَّ الموضعُ فاحذف سطرَه من {DEBT_LEDGER.name}: {key.split(chr(9), 1)[1][:100]}")
    print(f"② الصيغُ الميّتة: {scanned} ملفًّا مُسِح · {len(seen)} مخالفةً قائمة "
          f"(دَينٌ مسجَّل {len(ledger)}) · جديدٌ {len(fresh)} · بائتٌ {len(stale)}")
    return scanned, failures


def main() -> int:
    parser = argparse.ArgumentParser(description="بوّابةُ صيغةِ مقاطعِ ص خارجَ ملفّات .ص")
    parser.add_argument("--jobs", type=int, default=min(8, (os.cpu_count() or 4)),
                        help="عددُ المصرِّفاتِ المتوازية للفحصِ ①")
    parser.add_argument("--فحص", choices=("الكل", "مجسات", "صيغ"), default="الكل")
    parser.add_argument("--سجّل", action="store_true",
                        help="يعيد كتابة سجلّ الدَّين من المقيس اليومَ — لا يُنادى في CI")
    args = parser.parse_args()

    which = getattr(args, "فحص")
    failures: list[str] = []

    if which in ("الكل", "مجسات"):
        _, probe_failures = check_python_probes(args.jobs)
        failures += probe_failures
    if which in ("الكل", "صيغ"):
        _, form_failures = check_dead_forms(record=getattr(args, "سجّل"))
        failures += form_failures

    if failures:
        print("\n❌ نصُّ ص بصيغةٍ لا تقبلها اللغةُ اليوم:\n")
        for item in failures:
            print(f"   • {item}")
        print(f"\n   المجموع: {len(failures)}")
        print("   💡 إن كان المقصودُ عرضَ المرفوضِ فعلِّمه بـ"
              f"«{WAIVER}» في السطرِ أو فوقَه — تجاوزٌ مكتوبٌ يُقرأ في المراجعة.")
        return 1

    print("\n✅ الحارس سليم — لا مقطعَ ص بصيغةٍ مرفوضةٍ خارجَ ما عُلِّم صراحةً")
    return 0


if __name__ == "__main__":
    sys.exit(main())
