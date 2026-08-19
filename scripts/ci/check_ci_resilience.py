#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارسُ صمودِ CI: لا جلبٌ شبكيٌّ بلا إعادةٍ، ولا وظيفةٌ بلا حدٍّ زمنيّ.

     🔑 الدافعُ مقيسٌ لا مُفترَض. مُسِحت آخرُ ٢٠٠ شوطٍ في ١٩ آب ٢٠٢٦؛ وسيطُ
     زمنِ الشوطِ نحوُ ٢٠ دقيقة، وفيها أربعةُ أشواطٍ خرجت عن السربِ وكلُّها
     عالقةٌ على **خطوةِ جلبٍ شبكيّ**:

       · 32066236587 و32066237695 — 🪟 Windows (Release)، «تحميل LLVM 18»:
         ٥:٠١:٢١ لكلٍّ على `Invoke-WebRequest`، ولم يقطعْهما حدُّ زمنٍ البتّةَ
         بل إلغاءُ التزامن — أي أنّهما كانا سيمضيان.
       · 32094983203 — «قياس منافذ العائلة»، «تثبيت LLVM 18»: ٦:٠٠:٠٢
       · 32096599168 — 🐧 Linux (Debug)، «تثبيت المتطلبات»:      ٦:٠٠:٠٧
         وهذان بلغا سقفَ ﭼِتهَب الافتراضيَّ (٣٦٠ دقيقة) لأنّ وظيفتَيهما كانتا
         بلا `timeout-minutes` — فالسقفُ الوحيدُ كان سقفَ المنصّةِ لا سقفَنا.

     وسبقَ أن سُدَّ `apt` وحدَه بـ`apt_retry.sh`، ثمّ تبيّن أنّ الرقعةَ وُصِلت
     في ملفٍّ وتُرِكَ إخوتُه. فهذا الحارسُ يحرسُ **الصنفَ** لا الحادثةَ، ويُحاكَمُ
     في الاتّجاهَين: يُحمِّرُ الفعلَ العاريَ، ويُحمِّرُ كذلك اختفاءَ المُغلِّفاتِ
     أو انفكاكَها فلا تصيرُ رقعةً ميّتة.

     خمسةُ فحوص:
       أ) كلُّ نداءِ `apt` يمرُّ عبرَ `scripts/ci/apt_retry.sh`.
       ب) كلُّ جالبٍ شبكيٍّ محروسٍ يمرُّ عبرَ `net_retry.sh`/`net_retry.ps1`،
          أو بأعلامٍ أصيلةٍ تُعيدُ وتَحُدّ، أو يحمل إعفاءً مُعلَّلًا **مع حدٍّ
          زمنيٍّ صريح** (للأفعالِ غيرِ متكرّرةِ الأثرِ كـPOST — إعادتُها تُكرّرُ
          أثرَها فلا تُعاد).
       ج) كلُّ وظيفةٍ تُعلِنُ `timeout-minutes` لا يتجاوز السقفَ المعقول.
       د) لا اسمَ متغيّرِ صدَفةٍ غيرَ لاتينيٍّ في المُغلِّفات — bash لا يُسنِدُه.
       هـ) المُغلِّفاتُ الثلاثةُ موجودةٌ وموصولة — فلا تصيرُ رقعةً ميّتة.

     ⚠️ ما ليس محروسًا ولماذا — **مقيسٌ لا مُفترَض**:
       · `pip install` يعيدُ المحاولةَ ٥ مرّاتٍ افتراضًا (قُرِئ من
         `pip install --help`: «--retries … default: 5») ومهلةُ مقبسِه ١٥ ث.
       · `npm ci`/`npm install` يعيدان مرّتين افتراضًا (`fetch-retries=2`).
       فإدخالُهما هنا يضاعفُ الإعادةَ بلا مكسبٍ مقيس.

(EN) CI resilience guard: no unretried network fetch, no unbounded job.
     Judged in both directions — a bare fetch reddens, and so does a wrapper
     going missing or unreferenced. pip and npm are deliberately unguarded
     because they already retry by default (measured, not assumed).
"""

import re
import sys
from pathlib import Path

WORKFLOWS = Path(".github/workflows")
APT_WRAPPER = Path("scripts/ci/apt_retry.sh")
NET_WRAPPER_SH = Path("scripts/ci/net_retry.sh")
NET_WRAPPER_PS = Path("scripts/ci/net_retry.ps1")
WRAPPERS = (APT_WRAPPER, NET_WRAPPER_SH, NET_WRAPPER_PS)

# (AR) سقفُ حدِّ الوظيفة: وجودُ `timeout-minutes` وحدَه لا يكفي — «٣٦٠» يُرضي
#      فحصَ الوجودِ ويعيدُ العطبَ نفسَه. فالحدُّ يُقاسُ لا يُعلَن فحسب.
# (EN) Existence alone is not enough: `timeout-minutes: 360` would satisfy a
#      naive check and reinstate the very defect. Cap it.
JOB_TIMEOUT_CEILING = 120

# (AR) المرساةُ هي **الفعلُ** لا هجاؤه: استدعاءُ apt بأمرٍ من أوامرِه.
APT_ACT = re.compile(
    r"(?<![\w./-])apt(?:-get|itude)?\s+"
    r"(?:update|upgrade|dist-upgrade|full-upgrade|install|remove|purge|build-dep)"
    r"(?![\w-])"
)

# (AR) الجالباتُ المحروسة: اسمُ الأمرِ **في موضعِ الأمر** لا في أيِّ موضع —
#      وإلّا حُسِبَت `wget` اسمَ حزمةٍ في `install -y … wget gnupg` مخالفةً.
# (EN) Guarded fetchers, matched in command position only — otherwise `wget`
#      as a package name inside an install list would count as a violation.
NET_COMMANDS = {
    "wget": "wget",
    "curl": "curl",
    "brew": "brew",
    "choco": "choco",
    "winget": "winget",
    "scoop": "scoop",
    "invoke-webrequest": "Invoke-WebRequest",
    "iwr": "Invoke-WebRequest",
    "invoke-restmethod": "Invoke-RestMethod",
    "irm": "Invoke-RestMethod",
    "start-bitstransfer": "Start-BitsTransfer",
}

# (AR) أوامرُ brew المحلّيّةُ لا تلمسُ الشبكةَ — `brew --prefix` استعلامُ مسار.
LOCAL_BREW_SUBCOMMANDS = {"--prefix", "--cellar", "--repository", "list", "config"}

# (AR) علامةُ الإعفاء: للفعلِ غيرِ متكرّرِ الأثرِ الذي لا تصحُّ إعادتُه.
WAIVER = re.compile(r"SAD_CI_NO_RETRY\s*:\s*(\S.*)$")

# (AR) وحدودُ الزمنِ المقبولةُ في الإعفاء — إعفاءٌ بلا حدٍّ يعيدُ العطبَ كلَّه.
TIME_BOUNDS = ("--max-time", "--connect-timeout", "-TimeoutSec", "--timeout")

# (AR) المُغلِّفُ ليس الطريقَ الوحيد: بعضُ الجالباتِ تُعيدُ وتَحُدُّ بأعلامِها.
#      والحكمُ على **الأثر** لا على الوسيلة: مُقيَّدٌ ومُعادٌ يكفي كيفما تحقّق.
#      ⚠️ وهذا ليس تفضيلًا بل قياسٌ: وظيفتا `remind` و`dispatch` في
#      `notify-sadlang-docs` و`dev-guide-sync-reminder` **بلا `actions/checkout`**
#      فلا وجودَ لـ`scripts/` فيهما أصلًا — والمُغلِّفُ هناك مستحيلٌ لا مرجوح.
# (EN) The wrapper is not the only road: some fetchers retry and bound natively.
#      Measured, not preferred — the notifier jobs have no checkout at all, so a
#      repo-script wrapper is impossible there, not merely undesirable.
NATIVE_BOUNDS = {
    "curl": (("--retry",), ("--max-time", "--connect-timeout")),
    "wget": (("--tries=", "--tries "), ("--timeout=", "--timeout ", "--read-timeout")),
    "Invoke-WebRequest": (("-MaximumRetryCount",), ("-TimeoutSec",)),
    "Invoke-RestMethod": (("-MaximumRetryCount",), ("-TimeoutSec",)),
}

SEGMENT_SPLIT = re.compile(r"\|\||&&|[|;]")
PREFIXES = ("sudo", "time", "command", "env", "exec", "!", "-", "run:", "&")


def مقاطع_الأمر(سطر):
    """(AR) يُعيدُ أوّلَ كلمةٍ في كلِّ مقطعِ أمرٍ داخلَ السطر."""
    مجرد = سطر.strip()
    for بادئة in ("- run:", "run:", "- name:", "name:"):
        if مجرد.startswith(بادئة):
            مجرد = مجرد[len(بادئة):].strip()
            break
    # (AR) الاستبدالُ لا الحذف: `$(cmd)` و`` `cmd` `` مواضعُ أمرٍ كذلك.
    مجرد = مجرد.replace("$(", " ").replace("`", " ").replace("(", " ")
    كلمات_أُوَل = []
    for مقطع in SEGMENT_SPLIT.split(مجرد):
        رموز = مقطع.split()
        while رموز and (رموز[0] in PREFIXES or "=" in رموز[0].split("/")[0][:40]):
            رموز = رموز[1:]
        if رموز:
            كلمات_أُوَل.append(رموز)
    return كلمات_أُوَل


def أسطر_قابلة_للتنفيذ(نص):
    """(AR) يُرجِعُ (رقم، سطر) للأسطرِ التي تُنفَّذ — بلا تعليقٍ ولا كتلةِ توثيق.

    ما بينَ سياجَي ``` نصٌّ يُعرَض على القارئِ (ملاحظاتُ إصدار) لا فعلٌ يُنفَّذ؛
    وحسبانُه فعلًا هو فخُّ «الهجاءُ لا الفعل» مقلوبًا.
    """
    داخل_سياج = False
    for رقم, سطر in enumerate(نص.splitlines(), start=1):
        مجرد = سطر.strip()
        if مجرد.startswith("```"):
            داخل_سياج = not داخل_سياج
            continue
        if داخل_سياج or مجرد.startswith("#"):
            continue
        yield رقم, سطر, مجرد


def اجمع_مخالفات_apt(الجذر):
    مخالفات = []
    for ملف in sorted((الجذر / WORKFLOWS).glob("*.y*ml")):
        نص = ملف.read_text(encoding="utf-8", errors="replace")
        for رقم, _سطر, مجرد in أسطر_قابلة_للتنفيذ(نص):
            if not APT_ACT.search(مجرد):
                continue
            if APT_WRAPPER.name in مجرد:
                continue
            مخالفات.append((ملف.relative_to(الجذر).as_posix(), رقم, مجرد))
    return مخالفات


def فيه_رمز(الأسطر, فهرس, رموز):
    """(AR) الرمزُ قد يقع على سطرِ الفعلِ أو على أسطرِ المواصلةِ بعدَه."""
    نافذة = "\n".join(الأسطر[فهرس:فهرس + 5])
    return any(رمز in نافذة for رمز in رموز)


def له_حد_زمني(الأسطر, فهرس):
    return فيه_رمز(الأسطر, فهرس, TIME_BOUNDS)


def له_إعفاء(الأسطر, فهرس):
    """(AR) الإعفاءُ تعليقٌ يسبقُ الفعلَ ويحملُ سببًا — لا علامةً عارية."""
    مؤشر = فهرس - 1
    while مؤشر >= 0 and not الأسطر[مؤشر].strip():
        مؤشر -= 1
    if مؤشر < 0:
        return None
    مطابقة = WAIVER.search(الأسطر[مؤشر])
    return مطابقة.group(1).strip() if مطابقة else None


def اجمع_مخالفات_الشبكة(الجذر):
    مخالفات = []
    for ملف in sorted((الجذر / WORKFLOWS).glob("*.y*ml")):
        نص = ملف.read_text(encoding="utf-8", errors="replace")
        الأسطر = نص.splitlines()
        for رقم, _سطر, مجرد in أسطر_قابلة_للتنفيذ(نص):
            if NET_WRAPPER_SH.name in مجرد or NET_WRAPPER_PS.name in مجرد:
                continue
            for رموز in مقاطع_الأمر(مجرد):
                أمر = NET_COMMANDS.get(رموز[0].lower().lstrip("&").strip("\"'"))
                if أمر is None:
                    continue
                if أمر == "brew" and len(رموز) > 1 and رموز[1] in LOCAL_BREW_SUBCOMMANDS:
                    continue
                سبب = له_إعفاء(الأسطر, رقم - 1)
                if سبب is not None:
                    if not له_حد_زمني(الأسطر, رقم - 1):
                        مخالفات.append((
                            ملف.relative_to(الجذر).as_posix(), رقم, أمر, مجرد,
                            "إعفاءٌ بلا حدٍّ زمنيٍّ صريح — الإعفاءُ من الإعادةِ "
                            "لا من الحدّ (" + سبب + ")",
                        ))
                    continue
                أعلام = NATIVE_BOUNDS.get(أمر)
                if أعلام is not None:
                    إعادة = فيه_رمز(الأسطر, رقم - 1, أعلام[0])
                    حد = فيه_رمز(الأسطر, رقم - 1, أعلام[1])
                    if إعادة and حد:
                        continue
                    ناقص = ("بلا إعادةِ محاولة" if not إعادة else "")
                    if not حد:
                        ناقص = (ناقص + " و" if ناقص else "") + "بلا حدٍّ زمنيّ"
                    مخالفات.append((
                        ملف.relative_to(الجذر).as_posix(), رقم, أمر, مجرد,
                        "جلبٌ شبكيٌّ " + ناقص,
                    ))
                    break
                مخالفات.append((
                    ملف.relative_to(الجذر).as_posix(), رقم, أمر, مجرد,
                    "جلبٌ شبكيٌّ بلا إعادةِ محاولة — ولا أعلامَ إعادةٍ أصيلةً "
                    "لهذا الأمر، فالمُغلِّفُ هو الطريق",
                ))
                break
    return مخالفات


def افحص_حدود_الوظائف(الجذر):
    """(AR) كلُّ وظيفةٍ محدودةٌ بحدٍّ لنا لا بسقفِ المنصّةِ (٣٦٠ دقيقة)."""
    أعطاب = []
    for ملف in sorted((الجذر / WORKFLOWS).glob("*.y*ml")):
        نص = ملف.read_text(encoding="utf-8", errors="replace")
        الأسطر = نص.splitlines()
        بداية_الوظائف = None
        for فهرس, سطر in enumerate(الأسطر):
            if re.match(r"^jobs:\s*$", سطر.rstrip()):
                بداية_الوظائف = فهرس
                break
        اسم_الملف = ملف.relative_to(الجذر).as_posix()
        if بداية_الوظائف is None:
            أعطاب.append((اسم_الملف, 1, "لا كتلةَ `jobs:` — الحارسُ لا يقرأُ هذا الملفّ"))
            continue
        وظائف = []
        for فهرس in range(بداية_الوظائف + 1, len(الأسطر)):
            سطر = الأسطر[فهرس].rstrip()
            if سطر and not سطر.startswith(" "):
                break
            مطابقة = re.match(r"^  ([A-Za-z0-9_][A-Za-z0-9_-]*):\s*$", سطر)
            if مطابقة:
                وظائف.append((مطابقة.group(1), فهرس))
        for موضع, (اسم, فهرس) in enumerate(وظائف):
            نهاية = وظائف[موضع + 1][1] if موضع + 1 < len(وظائف) else len(الأسطر)
            حد = None
            for سطر in الأسطر[فهرس:نهاية]:
                مطابقة = re.match(r"^    timeout-minutes:\s*(\d+)\s*$", سطر.rstrip())
                if مطابقة:
                    حد = int(مطابقة.group(1))
                    break
            if حد is None:
                أعطاب.append((اسم_الملف, فهرس + 1,
                              "الوظيفة `" + اسم + "` بلا `timeout-minutes` — "
                              "سقفُها سقفُ المنصّةِ ٣٦٠ دقيقة"))
            elif حد > JOB_TIMEOUT_CEILING:
                أعطاب.append((اسم_الملف, فهرس + 1,
                              "الوظيفة `" + اسم + "` حدُّها " + str(حد)
                              + " دقيقة — فوق السقفِ المتّفَقِ "
                              + str(JOB_TIMEOUT_CEILING)))
    return أعطاب


# (AR) 🔴 اسمُ متغيّرِ صدَفةٍ غيرُ لاتينيٍّ **ليس اسمًا**: bash يرى `س=قيمة` أمرًا
#      لا إسنادًا، و`$س` تبقى نصًّا حرفيًّا — والنحوُ سليمٌ تمامًا فلا يراه
#      `bash -n`. قِيسَ في شوطِ 0ce56e40: إسقاطُ المرآةِ نُوديَ وردَّ «لا قائمةَ»
#      لأنّ المسارَ لم يُسنَدْ قطُّ، فبدا امتناعًا مشروعًا وهو عطبٌ صامت.
#      (وأسماءُ الدوالِّ العربيّةُ مقبولةٌ في bash — الفرقُ في المتغيّراتِ وحدَها.)
# (EN) A non-ASCII shell variable name is not a name: bash treats `س=v` as a
#      command and leaves `$س` literal — syntactically valid, invisible to
#      `bash -n`. Function names may be Arabic; variables may not.
SHELL_DIR = Path("scripts/ci")
اسناد_صدفة = re.compile(r"^\s*(?:local\s+|export\s+|readonly\s+)?([^\s=]+)=")
مرجع_صدفة = re.compile(r"\$\{?([^\s${}\"'()\[\]/:,.-]+)")
اسم_لاتيني = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")


def افحص_اسماء_الصدفة(الجذر):
    """(AR) لا اسمَ متغيّرٍ غيرَ لاتينيٍّ في مُغلِّفاتِ الصدَفة."""
    أعطاب = []
    for ملف in sorted((الجذر / SHELL_DIR).glob("*.sh")):
        for رقم, سطر in enumerate(
                ملف.read_text(encoding="utf-8", errors="replace").splitlines(), 1):
            مجرد = سطر.strip()
            if مجرد.startswith("#"):
                continue
            مشبوهة = set()
            مطابقة = اسناد_صدفة.match(سطر)
            if مطابقة and not اسم_لاتيني.match(مطابقة.group(1)):
                if not مطابقة.group(1).startswith("$"):
                    مشبوهة.add(مطابقة.group(1))
            for اسم in مرجع_صدفة.findall(سطر):
                if any(ord(ح) > 127 for ح in اسم):
                    مشبوهة.add(اسم)
            for اسم in sorted(مشبوهة):
                أعطاب.append((
                    ملف.relative_to(الجذر).as_posix(), رقم,
                    "اسمُ متغيّرِ صدَفةٍ غيرُ لاتينيّ `" + اسم
                    + "` — bash لا يُسنِدُه، والنحوُ سليمٌ فلا يراه `bash -n`"))
    return أعطاب


def افحص_المغلفات(الجذر):
    """(AR) الاتّجاهُ الثاني: المُغلِّفاتُ موجودةٌ وموصولةٌ فعلًا — لا رقعةً ميّتة."""
    أعطاب = []
    for مغلف in WRAPPERS:
        if not (الجذر / مغلف).is_file():
            أعطاب.append("المُغلِّفُ " + مغلف.as_posix() + " غيرُ موجود")
            continue
        وصلات = 0
        for ملف in sorted((الجذر / WORKFLOWS).glob("*.y*ml")):
            وصلات += ملف.read_text(encoding="utf-8", errors="replace").count(
                مغلف.as_posix())
        if وصلات == 0:
            أعطاب.append("المُغلِّفُ " + مغلف.as_posix()
                         + " موجودٌ ولا يستدعيه سيرُ عملٍ واحد — رقعةٌ ميّتة")
    return أعطاب


def main():
    الجذر = Path(__file__).resolve().parents[2]
    مخالفات_apt = اجمع_مخالفات_apt(الجذر)
    مخالفات_شبكة = اجمع_مخالفات_الشبكة(الجذر)
    أعطاب_حدود = افحص_حدود_الوظائف(الجذر)
    أعطاب_أسماء = افحص_اسماء_الصدفة(الجذر)
    أعطاب_مغلفات = افحص_المغلفات(الجذر)

    الكل = (len(مخالفات_apt) + len(مخالفات_شبكة) + len(أعطاب_حدود)
            + len(أعطاب_أسماء) + len(أعطاب_مغلفات))
    if الكل == 0:
        print("✅ حارسُ صمودِ CI: كلُّ جلبٍ عبرَ مُغلِّف، وكلُّ وظيفةٍ محدودةُ "
              "الزمن، وكلُّ اسمِ متغيّرٍ لاتينيّ")
        return 0

    for عطب in أعطاب_مغلفات:
        print("::error::حارسُ صمودِ CI: " + عطب)
    for مسار, رقم, نص in مخالفات_apt:
        print("::error file=" + مسار + ",line=" + str(رقم)
              + "::نداءُ apt عارٍ — مرِّرْه عبرَ " + APT_WRAPPER.as_posix()
              + " │ " + نص)
    for مسار, رقم, أمر, نص, سبب in مخالفات_شبكة:
        print("::error file=" + مسار + ",line=" + str(رقم)
              + "::" + أمر + ": " + سبب + " │ " + نص)
    for مسار, رقم, سبب in أعطاب_حدود + أعطاب_أسماء:
        print("::error file=" + مسار + ",line=" + str(رقم) + "::" + سبب)

    print("")
    print("❌ حارسُ صمودِ CI: " + str(len(مخالفات_apt)) + " نداءَ apt عاريًا · "
          + str(len(مخالفات_شبكة)) + " جلبًا شبكيًّا بلا إعادة · "
          + str(len(أعطاب_حدود)) + " وظيفةً بلا حدٍّ سليم · "
          + str(len(أعطاب_أسماء)) + " اسمَ متغيّرٍ غيرَ لاتينيّ · "
          + str(len(أعطاب_مغلفات)) + " عطبًا في المُغلِّفات")
    print("   الإصلاح: bash " + APT_WRAPPER.as_posix() + " install -y <الحزم>")
    print("            bash " + NET_WRAPPER_SH.as_posix() + " brew install <الحزم>")
    print("            pwsh " + NET_WRAPPER_PS.as_posix() + " -Uri <العنوان> -OutFile <الملفّ>")
    print("            timeout-minutes: <عدد> لكلِّ وظيفة (≤ "
          + str(JOB_TIMEOUT_CEILING) + ")")
    print("   أو بأعلامٍ أصيلةٍ حيثُ لا checkout: "
          "curl --retry 3 --max-time 120 · wget --tries=3 --timeout=30")
    print("   وللفعلِ غيرِ متكرّرِ الأثر: علِّقْ قبلَه `SAD_CI_NO_RETRY: <السبب>`")
    print("   واجعلْ له حدًّا زمنيًّا صريحًا (" + "، ".join(TIME_BOUNDS) + ")")
    return 1


if __name__ == "__main__":
    sys.exit(main())
