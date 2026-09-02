#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِجَسُّ «على أيِّ محرّكٍ يعملُ هذا المدمَج؟» — يُقاسُ ولا يُعلَن.

     ¶ لماذا مِجَسٌّ أصلًا، ومصدرُ الحقيقةِ فيه حقلُ `status`؟

       لأنّ `status: stable` **لا يعني أنّ المدمَجَ يصلُ إلى ثنائيّ**. كلُّ
       المدمَجاتِ `stable` في `language-truth/builtins/`، ومع ذلك يرفضُ
       المترجّمُ منها مئاتٍ بـSEM047 المسمّى. الحقلُ **دعوى** والمِجَسُّ **قياس**،
       ونشرُ الدعوى في التوثيقِ يجعلُ القارئَ يبني على ما لا يعمل.

     ¶ لماذا لا يُشتَقُّ من مسحِ الشيفرةِ نصًّا؟

       مسحُ `registerBuiltinFunction("…")` في مصدرِ المفسّرِ **غيرُ مُعايَر**:
       قِيس أنّ ٤٢٣ اسمًا معلَنًا لا يردُ نصُّه في `interpreter/src/**.cpp`
       إطلاقًا، ومنها ما يعملُ فعلًا (المفسّرُ يحلُّ أسماءً بطرقٍ أخرى).
       والحَكَمُ الوحيدُ الصادقُ هو **المحرّكُ نفسُه**.

     ¶ الشكلان المقيسان — ولكلٍّ سببُه

       • المفسّر: `متغير س = <الاسم>` — **مرجعُ اسمٍ بلا نداء**. مقصودٌ ألّا
         يُنفَّذَ شيء: في القائمةِ `معالج_توقف` و`أغلق_مقبس` و`احذف_ملف`،
         وتنفيذُ المدمَجاتِ كلِّها يوقفُ المعالجَ ويفتحُ مقابسَ ويحذفُ ملفّات.
         والرمزُ الحاكمُ SEM001 على الاسمِ نفسِه.

       • المترجّم: نداءٌ برتبةِ `arity.min` داخلَ **دالّةٍ لا تُنادى** — أجسامُ
         الدوالِّ تُبنى قبلَ التوپلفل، فيصدرُ الحكمُ بلا تنفيذ. والرمزُ الحاكمُ
         SEM047 المسمّى («معلَنةٌ … ولا توزيعَ لها في المترجم») أو رسالةُ
         «استدعاء دالة غير معرّفة». (والمترجّمُ لا يقبلُ مرجعَ مدمَجٍ بلا نداء
         — قِيس: «Undefined variable» حتّى للعامل، فالشكلُ الواحدُ لا يكفي
         للمحرّكَين.) ويُترجَمُ بـ`-c` بلا ربطٍ ولا يُشغَّلُ ناتجُه قطّ.

     ¶ 🔑 لماذا اسمٌ واحدٌ لكلّ تشغيلٍ لا دفعة؟

       الدفعةُ جُرِّبت وكانت أسرعَ **وأكذب**: خطأُ نوعٍ من وسيطٍ صوريٍّ في اسمٍ
       سابقٍ يبتُرُ المسحَ، فتُقرَأُ الأسماءُ التاليةُ «مدعومةً» وهي لم تُفحَص.
       قِيست دفعاتٌ أعطت صفرَ غيابٍ ومعها سبعةَ عشرَ خطأً من صنفٍ آخر. الأداةُ
       التي تختصرُ الزمنَ باختصارِ القياسِ تُفسِدُ ما تقيسه.

     ¶ 🔑 لا حكمَ «مدعوم» إلّا بإثباتٍ موجب

       الصمتُ ليس شهادة. كان جانبُ المترجّمِ يصنّفُ «مدعومًا» كلَّ ما لم يطابقْ
       SEM047 ولا «غير معرّفة» — فأيُّ فشلٍ آخرَ (خطأُ نوعٍ من وسيطٍ صوريّ،
       تعذّرُ فتحِ المصدرِ العربيّ، رسالةُ «تنتمي إلى وحدةٍ لم تُستورَد») كان
       يُقرَأُ دعمًا. واليومَ الغيابُ يُفحَصُ أوّلًا برمزِه المسمّى (SEM047 ثمّ
       «غير معرّفة»)، ثمّ يُثبَتُ الحضورُ بإثباتٍ موجب: رمزُ خروجٍ صفرٌ، أو
       **تشخيصٌ يسمّي المدمَجَ بعينِه** (والمترجّمُ لا يسمّي إلّا ما حلَّه)، أو
       علامةٌ بنيويّةٌ على بلوغِ الخلفيّة. وما عدا ذلك **ملتبسٌ يمنعُ الكتابة**.

     ¶ المعايرة (تُنفَّذُ في كلِّ تشغيلٍ **لكلِّ محرّكٍ** وتُدوَّنُ في المُخرَج)

       يُسأَلُ عن عيّنةٍ سؤالٌ **ثانٍ مستقلّ** (في المفسّر: استعلامٌ عن سِجِلِّه
       الحيِّ بجارٍ مخترَعٍ بدلَ المرجع؛ في المترجّم: نداءٌ في التوپلفل بدلَ
       جسمِ دالّةٍ لا تُنادى) — ولا يُنفَّذُ مدمَجٌ واحدٌ في أيٍّ منهما،
       ويُقارَنُ الحكمان. اختلافُهما يعني أنّ المِجَسَّ يقيسُ شيئًا غيرَ الذي
       يُنشَر — فيُعطَّلُ المُخرَجُ ولا يُكتَب. ومعايرةُ محرّكٍ واحدٍ **لا** تُجيزُ
       نشرَ عمودِ الآخر: الدعوى تُقيَّدُ بما قِيس.

(EN) Measures, per builtin, whether each engine actually resolves it. The
     SoT `status` field is a claim, not a measurement; source scanning is
     uncalibrated. One name per run: batching truncates and lies. No
     "supported" verdict without positive proof; both engines are calibrated.
============================================================================
"""
from __future__ import annotations

import argparse
import functools
import hashlib
import random
import re
import subprocess
import sys
import tempfile
import time
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parents[2]
SOT_DIR = ROOT / "language-truth" / "builtins"
OUT_PATH = ROOT / "language-truth" / "_meta" / "builtin_engine_support.yaml"

# (AR) كلُّ وحداتِ المكتبةِ تُستورَدُ في كلِّ مِجَسّ: بوّابةُ الاستيرادِ حقيقيّةٌ
#      ومقيسة (`طول_نص` بلا `استورد نصوص` ⇒ SEM001)، وتركُها يُنتجُ «غائبًا»
#      كاذبًا. وحاجةُ الاستيرادِ نفسُها معلَنةٌ في SoT فلا تحتاجُ قياسًا.
# (AR) 🔑 قائمةُ الوحداتِ **تُشتَقُّ** ولا تُكتَب. كانت مكتوبةً بيدٍ فسقطت
#      منها `عمليات` (PROCESSES)، فقِيست خمسةُ مدمَجاتٍ «لا يحلُّها المفسّر»
#      وهي تحلُّ باستيرادِ وحدتِها. والأسوأُ أنّ المعايرةَ لم تكشفها: السؤالُ
#      الثاني يستعملُ التمهيدَ نفسَه، فالشكلانِ يتّفقانِ على كذبةٍ واحدةٍ
#      لأنّ طرفَيهما من أصلٍ واحد. عِلاجُ الأصلِ أوثقُ من عيّنةٍ أوسع.
MODULE_HEADER = ROOT / "shared/builtins/include/module_definitions.h"
_MODULE_RE = re.compile(r'\{"([^"]+)",\s*ModuleId::(\w+)\}')


@functools.lru_cache(maxsize=1)
def module_aliases() -> dict:
    """`{ModuleId: الاسمُ العربيّ}` كما يعرفُها المحرّكان — لا كما أتذكّرُها.

    تُقرأُ كسولًا لا وقتَ الاستيراد: أداةُ قياسٍ ترفعُ `SystemExit` بمجرّدِ
    استيرادِها لا يمكنُ اختبارُها ولا استعارةُ ثوابتِها من حارسٍ آخر.
    """
    text = MODULE_HEADER.read_text(encoding="utf-8")
    table = {m.group(2): m.group(1) for m in _MODULE_RE.finditer(text)
             if m.group(2) != "NONE"}
    if not table:
        raise SystemExit("✗ تعذّر اشتقاقُ أسماءِ الوحدات من "
                         f"{MODULE_HEADER} — تمهيدٌ ناقصٌ يُنتِجُ نفيًا كاذبًا.")
    return table


def import_modules() -> list:
    """أسماءُ الوحداتِ العربيّةُ مرتَّبةً — تمهيدُ كلِّ مِجَسّ."""
    return sorted(module_aliases().values())


# (AR) 🔑 تصادمُ الاسمِ يحلُّ صامتًا — وقد وقع: `متغير م = شبكة` يحلُّ لأنّ
#      التمهيدَ فيه `استورد شبكة`، فيُرجَعُ إلى **الوحدة** لا إلى المدمَجِ
#      المسمّى `شبكة` (UIWidgets). فيُقرأُ «يحلُّه المفسّر» وهو لا يحلُّه:
#      قِيس أنّ حذفَ استيرادِ وحدتِه يقلبُ الحكمَ إلى SEM001، وأنّ نداءَه
#      يقولُ SEM004. أداةُ القياسِ كانت تُفسِدُ قياسَها بتمهيدِها.
#      والعلاجُ: يُحذَفُ من التمهيدِ استيرادُ الوحدةِ التي تُشبهُ الاسمَ المقيس.
def preamble_for(name: str) -> str:
    """تمهيدٌ خاصٌّ بالاسمِ المقيس: بلا الوحدةِ التي تُصادمُه."""
    return "\n".join("استورد " + m for m in import_modules() if m != name)


# (AR) 🔑 وطرفا هذا الشرَكِ كانا من نوعَين مختلفَين فلم يستطعْ أن يشتعل:
#      `IMPORT_MODULES` أسماءٌ **عربيّة** وحقلُ `module` في SoT معرّفاتُ
#      `ModuleId` **إنجليزيّة**، فـ`"NETWORK" == "شبكة"` مستحيلةٌ لأيِّ مُدخَل —
#      شرطٌ أخضرُ لأنّه لا يستطيعُ أن يكونَ كاذبًا. والحالةُ التي كُتب لها
#      كانت تنجو منه: مدمَجٌ اسمُه `شبكة` وحقلُه `NETWORK` يُحذَفُ استيرادُ
#      وحدتِه فيقولُ المفسّرُ SEM001 ⇒ **نفيٌ كاذب**. فيُترجَمُ الطرفانِ إلى
#      لسانٍ واحدٍ قبلَ المقارنة.
def collides_with_own_module(fn) -> bool:
    """اسمٌ يساوي اسمَ وحدتِه العربيَّ — تصادمٌ لا يُحَلُّ بحذفِ الاستيراد."""
    arabic = module_aliases().get(str(fn.get("module")))
    return bool(arabic) and fn["canonical"] == arabic


TAIL = 'اطبع_سطر("حي")\n'

ANSI = re.compile(r"\x1b\[[0-9;]*m")
SEM001 = re.compile(r"المتغير '([^']+)'")          # اسمٌ لا يحلُّ في المفسّر
SEM047 = re.compile(r"المدمَجة '([^']+)'")          # معلَنٌ بلا توزيعٍ في المترجم
UNDEF = re.compile(r"استدعاء دالة غير معرّفة '([^']+)'")   # اسمٌ يجهلُه المترجّم
# (AR) 🔑 وجودُ تشخيصٍ يُقاسُ **بالبنية** لا باللفظ: كان الشرطُ `"error [" in out`
#      وهو معلَّقٌ بكلمةٍ إنجليزيّةٍ في مستودعٍ يجري تعريبُه. يومَ تُعرَّبُ
#      «error» يصيرُ كلُّ تشخيصٍ غيرَ مرئيٍّ فيُقرَأُ كلُّ اسمٍ «مدعومًا».
#      ورمزُ التشخيصِ `[SEM001]` بنيةٌ لا تُعرَّب.
DIAG_CODE = re.compile(r"\[(?:SEM|SYN|LEX|TYP|RUN|IMP|GEN)\d{3}\]")
# (AR) المدمَجُ محجوبٌ بوحدةٍ حذفناها نحن من التمهيد (بسببِ تصادمِ الاسم):
#      ليس غيابًا ولا دعمًا — التباسٌ صريحٌ يوقفُ الأداة.
GATED = re.compile(r"تنتمي إلى وحدة '([^']+)' ولم تُستورَد")
# (AR) 🔑 حضورُ المدمَجِ يُثبَتُ بقاعدةٍ **عامّةٍ** لا بتعدادِ أشكالِ الرسائل.
#      عُدَّت ثلاثُ جولاتٍ من الالتباس (٢٣٣ ثمّ ٩٠ ثمّ ٤٦)، وكلُّها من صنفٍ
#      واحد: الوسيطُ الصوريُّ `0` خاطئُ النوع، فيشتكي المترجّمُ منه شكوًى
#      **تُسمّي المدمَجَ**: «الدالة 'طول_نص' تتوقّع 1 من المعاملات»، «خريطة_احصل
#      استقبلت وسيطًا أوّل ليس خريطةً»، «متوقع 'انتقل: عنصر واجهة' لكن وُجد
#      'i64'»، «‹حمل_سجل_المهمة›: تعليمةُ حلقةٍ صفريّة لا تُصدَر في وحدةٍ
#      مستضافة». تعدادُ هذه الأشكالِ لعبةُ ضربِ خُلدٍ لا تنتهي؛ والجامعُ بينها
#      أنّ المترجّمَ **سمّى الاسمَ** — ولا يُسمّي إلّا ما حلَّه.
#      وغيابُ الاسمِ يُفحَصُ **قبلَ** هذه القاعدة (SEM047 ثمّ «غير معرّفة»)،
#      فلا يبتلعُ العامُّ الخاصَّ. ومعه علامتانِ بنيويّتانِ على بلوغِ الخلفيّةِ
#      حتّى لو لم تُسمَّ: انهيارُ LLVM ورفضُ مُدقِّقِ الوحدة — وبلوغُ توليدِ
#      الكودِ لا يكونُ إلّا بعدَ حلِّ الاسم.
#      (وقِيس الطرفان: `احذف_ملف(0)` يُفشِلُ المُدقِّق و`احذف_ملف("س")` يُترجَمُ
#       برمزِ صفر. ويكشفُ هذا عرَضًا عيبًا في المترجّم: نوعُ وسيطِ المدمَجِ لا
#       يُفحَصُ أماميًّا دائمًا فيخرجُ «خطأٌ داخليّ» بدلَ خطأِ نوعٍ مفهوم —
#       عيبٌ يُبلَّغُ ولا يُصلَحُ من هنا.)
BACKEND_REACHED = re.compile(r"LLVM ERROR|وحدة LLVM المولَّدة غير سليمة")


def names_the_builtin(out: str, name: str) -> bool:
    """أذكرَ المترجّمُ هذا الاسمَ بعينِه؟ بحدودٍ لئلّا يُطابَقَ جزءُ اسمٍ أطول."""
    pattern = (r"(?<![\w؀-ۿ])" + re.escape(name)
               + r"(?![\w؀-ۿ])")
    return re.search(pattern, out) is not None


def compiler_resolves(name: str, rc, out: str):
    """`True` حضورًا، أو `None` التباسًا. الغيابُ يُفحَصُ قبلَ ندائها."""
    if rc == 0:
        return True
    if names_the_builtin(out, name) or BACKEND_REACHED.search(out):
        return True
    return None


# (AR) المعايرةُ **طبقيّةٌ** لا عشوائيّةً منتظمة. العشوائيُّ المنتظمُ يقيسُ
#      المجموعاتِ الكبيرةَ ويكادُ لا يلمسُ الصغيرة، وحكمُ المِجَسِّ ليس واحدًا
#      عبرها: المدمَجُ قد يكونُ منادًى في الإرسالِ دون أن يكونَ قيمةً أُولى
#      يُرجَعُ إليها باسمِها المجرَّد، وهذا اختلافُ **شكلٍ** لا اختلافُ وجود.
#      فلو أخطأ الشكلُ في مجموعةٍ بأسرِها لَذابَ خطؤها في متوسّطِ عيّنةٍ
#      منتظمة. الطبقيُّ يُلزِمُ كلَّ مجموعةٍ بحصّةٍ، فيظهرُ الخللُ حيثُ هو.
CALIBRATION_PER_NAMESPACE = 3   # حصّةُ كلِّ مجموعةٍ من كلِّ جانب
CALIBRATION_FLOOR = 25          # أرضيّةٌ لكلِّ جانبٍ مهما قلَّت المجموعات

# (AR) 🔑 ثلاثةُ أسماءٍ لا يقيسُها هذا الشكلُ مهما حُسِّن، ولكلٍّ سببٌ من صنفٍ
#      آخر: `خطأ` اسمُ مدمَجٍ **وحرفُ اللغةِ للكذب** معًا فنداؤه نداءُ قيمةٍ لا
#      دالّة؛ و`زاوج` يشتكي المترجّمُ من نوعِ وسيطِه بعبارةٍ لا تسمّيه؛
#      و`عشوائي_آمن` **يُسقِطُ المترجّمَ** (0xC0000005 بلا مخرَج) — عيبٌ يُبلَّغ.
#      وإجبارُ القاعدةِ على ابتلاعِها يُعيدُ الكذبةَ التي أُزيلت. فيُعلَنُ
#      العجزُ صراحةً: تُنزَعُ صفوفُها من السِّجِلِّ وتُسمَّى في `unmeasured`
#      بأسبابِها، فيعرضُها التوثيقُ «—» لا «✅» ولا «❌». وسقفٌ صغيرٌ يمنعُ
#      أن تصيرَ هذه السلّةُ مقبرةً صامتةً يُلقى فيها ما يصعبُ قياسُه.
MAX_UNMEASURED = 5
# (AR) سقفُ «لم يُجِب» في المعايرة: العيّنةُ لا تُفرَّغُ في سلّةِ الصمتِ فتخضرَّ
#      المعايرةُ على لا شيء. وهو أوسعُ قليلًا لأنّ الصمتَ هنا زمنيٌّ لا بنيويّ.
MAX_UNANSWERED = 12


def load_builtins() -> list:
    out = []
    for path in sorted(SOT_DIR.glob("*.yaml")):
        doc = yaml.safe_load(path.read_text(encoding="utf-8")) or {}
        for fn in doc.get("functions") or []:
            out.append(fn)
    return out


def arity_min(fn: dict) -> int:
    arity = fn.get("arity") or {}
    if "min" in arity:
        return int(arity["min"])
    return len(fn.get("params") or [])


# (AR) 🔑 الاسمُ الواحدُ قد يُعلَنَ في ملفَّين برتبتَين مختلفتَين (`استبدل`:
#      ٣ و٣ و١)، والمحرّكُ يعرفُ **اسمًا** لا إعلانًا. فلو قِيس كلُّ إعلانٍ
#      وجُمعت النتائجُ في مجموعةٍ واحدة لَقلَبت رتبةٌ واحدةٌ خاطئةٌ حكمَ الاسمِ
#      كلِّه إلى «غيرُ مدعوم»، ولَكُتبت للاسمِ صفوفٌ مكرّرةٌ في التوثيق.
#      فيُوحَّدُ الإعلانُ إلى اسمٍ برتبةٍ **دنيا**. والرتبةُ نفسُها لم تعُد
#      حاملةً للحكم: شكوى المترجّمِ من الرتبةِ تسمّي المدمَجَ فتشهدُ بحضورِه،
#      وSEM047 يسبقُها — فالحكمُ واحدٌ أيًّا كانت الرتبةُ المُمرَّرة.
def unify_declarations(fns: list) -> list:
    """إعلانٌ واحدٌ لكلِّ اسم: أدنى رتبةٍ، ومجموعةُ أوّلِ إعلانٍ مرتَّبًا."""
    merged = {}
    for fn in fns:
        name = fn["canonical"]
        prev = merged.get(name)
        if prev is None or arity_min(fn) < arity_min(prev):
            merged[name] = fn
    return [merged[n] for n in sorted(merged)]


# (AR) المجلَّدُ المؤقَّتُ يُسمّى عشوائيًّا في كلِّ تشغيل.
TMP_PLACEHOLDER = "<مؤقّت>"


def normalize_reason(text: str, tmp) -> str:
    """(AR) يُطبِّعُ المسارَ المؤقَّتَ في سببِ الالتباس.

    اسمُ المجلَّدِ يدخلُ حقلَ `reason`، فإذا أُعيدَ القياسُ ولم يتغيّرْ في اللغةِ شيء،
    اختلفَ السِّجِلُّ عن سابقِهِ خلافًا كاذبًا يُخفي الحقيقيّ.
    """
    root = str(tmp)
    for form in (root, root.replace(chr(92), "/")):
        text = text.replace(form, TMP_PLACEHOLDER)
    return text


def run(cmd: list, timeout: int, cwd=None):
    """يُرجِعُ `(رمزُ الخروج، النصّ)`؛ ورمزُ الخروجِ `None` عند انتهاءِ المهلة.

    رمزُ الخروجِ ليس زينةً: هو **الإثباتُ الموجبُ** الذي يقومُ عليه حكمُ
    «يدعمُه المترجّم». طرحُه كان يجعلُ الصمتَ شهادةً.

    🔑 و`cwd` دفاعٌ في العمق: كان المحرّكُ يرثُ مجلّدَ من أطلقَ الأداة —
    **جذرَ المستودعِ عادةً** — فأيُّ أثرٍ بمسارٍ نسبيٍّ يقعُ في شجرةِ العمل.
    يُحبَسُ الآن في المجلّدِ المؤقّتِ الذي يُمحى بعده. ولا يحبسُ المقابسَ ولا
    المسالكَ المطلقة، فليس بديلًا عن ألّا يُنفَّذَ شيءٌ أصلًا.
    """
    try:
        # (AR) 🔑 المُدخَلُ القياسيُّ **مقطوع**: الشكلُ الثاني في المعايرةِ نداءٌ
        #      فعليّ، فـ`اقرأ` و`قراءة_سطر` كانتا تنتظرانِ سطرًا لا يأتي حتّى
        #      تنتهيَ المهلةُ (٦٠ث لكلٍّ) فيُقرأُ صمتُهما خلافًا. وقطعُ المُدخَلِ
        #      يجعلُ الجوابَ فوريًّا ويمنعُ الأداةَ من انتظارِ مستعمِلٍ غائب.
        proc = subprocess.run(cmd, capture_output=True, timeout=timeout,
                              stdin=subprocess.DEVNULL,
                              cwd=str(cwd) if cwd else None)
    except subprocess.TimeoutExpired:
        return None, "TIMEOUT"
    return proc.returncode, ANSI.sub("", (proc.stdout + proc.stderr)
                                     .decode("utf-8", "replace"))


# (AR) 🔑 المقيسُ حلُّ الاسمِ في **الواجهة الأماميّة**، فيُوقَفُ المترجّمُ عند
#      `-c` (ترجمةٌ بلا ربط). قِيس أنّ الربطَ يُفشِلُ ٢٣٣ اسمًا برسالةِ الرابطِ
#      `LNK4098: defaultlib 'libcmt' conflicts` — تصادمُ مكتبةِ تشغيلٍ في
#      بيئةِ البناءِ لا علاقةَ له بحلِّ الاسم. وقراءةُ ذلك «غيرَ مدعوم» كذبةٌ
#      من الجهةِ الأخرى: الاسمُ حُلَّ ووُلِّدَ له كودٌ حتّى بلغَ الرابط. فيُقطَعُ
#      المسارُ قبلَ مرحلةٍ لا تقيسُ ما نزعمُ قياسَه — وهو أسرعُ ستَّ مرّاتٍ أيضًا.
def compile_only(build_exe: Path, src: Path, obj: Path) -> list:
    """أمرُ ترجمةٍ بلا ربط: `-c` — الحكمُ أماميٌّ فلا يُستدعى الرابطُ أصلًا."""
    return [str(build_exe), str(src), "-c", "-o", str(obj)]


def probe_interpreter(names: list, run_exe: Path, tmp: Path):
    """مرجعُ اسمٍ بلا نداء — لا يُنفَّذُ مدمَجٌ واحد."""
    absent, unclear = set(), []
    src = tmp / "i.ص"
    for name in names:
        src.write_text(preamble_for(name) + "\nمتغير مـجـس = " + name
                       + "\n" + TAIL, encoding="utf-8")
        rc, out = run([str(run_exe), str(src)], 60, cwd=tmp)
        if rc is None:
            unclear.append((name, "انتهت المهلة"))
            continue
        if rc == 0 and "حي" in out and not DIAG_CODE.search(out):
            continue
        if GATED.search(out):
            unclear.append((name, "محجوبٌ بوحدةٍ حُذفت من التمهيد"))
            continue
        match = SEM001.search(out)
        if match and match.group(1) == name:
            absent.add(name)
        else:
            head = (normalize_reason(out.strip().splitlines()[0], tmp)[:70]
                    if out.strip() else "فارغ")
            unclear.append((name, head))
    return absent, unclear


def probe_compiler(fns: list, build_exe: Path, tmp: Path):
    """نداءٌ داخلَ دالّةٍ لا تُنادى — الحكمُ أماميٌّ بلا تنفيذ.

    ولا يُقالُ «مدعوم» إلّا برمزِ خروجٍ صفر. كلُّ مخرَجٍ آخرَ لا يحملُ رمزَ
    الغيابِ المسمّى **يُصنَّفُ ملتبسًا** فيوقفُ الأداة.
    """
    absent, unknown, supported, unclear = set(), set(), set(), []
    src = tmp / "c.ص"
    obj = tmp / "c.o"
    for fn in fns:
        name = fn["canonical"]
        args = "، ".join(["0"] * arity_min(fn))
        src.write_text(preamble_for(name) + "\nدالة مجس()\n    " + name
                       + "(" + args + ")\nنهاية\n" + TAIL,
                       encoding="utf-8")
        rc, out = run(compile_only(build_exe, src, obj), 300, cwd=tmp)
        if rc is None:
            unclear.append((name, "انتهت المهلة"))
        elif name in SEM047.findall(out):
            absent.add(name)
        elif name in UNDEF.findall(out):
            unknown.add(name)
        elif GATED.search(out):
            unclear.append((name, "محجوبٌ بوحدةٍ حُذفت من التمهيد"))
        elif compiler_resolves(name, rc, out):
            supported.add(name)
        else:
            head = (normalize_reason(out.strip().splitlines()[0], tmp)[:70]
                    if out.strip() else "فارغ")
            unclear.append((name, f"رمز {rc}: {head}"))
    return absent, unknown, supported, unclear


# ---------------------------------------------------------------------------
# (AR) لماذا للمفسّرِ شكلٌ غيرُ شكلِ المترجّم؟ لأنّه لا شكلَ لنداءٍ **بلا أثر**
#      يراهُ المفسّرُ ساكنًا. جُرِّبت ثلاثةٌ، وكلُّها عمياء — قِيست لا افتُرضت:
#        ① نداءٌ داخل دالّةٍ لا تُنادى ..... يطبعُ «حي» حتّى لاسمٍ مخترَعٍ لا وجودَ له
#        ② نداءٌ في فرعٍ ميّت `إذا (1 == 2)` .. كذلك
#        ③ `--فحص-الأنواع` مع ① .......... كذلك
#      و`sad-run check` غيرُ موجودٍ أصلًا رغم إعلانِه في لافتةِ الاستعمال
#      (عيبٌ في `sad-run` لا في هذا المِجَسّ، وله تذكرتُه).
#      فبقيَ المرجعُ المجرَّدُ `متغير م = الاسم` وحدَه، وهو يقيسُ **حلَّ الاسمِ
#      قيمةً أُولى**. والفرقُ بينه وبين النداءِ هو بالضبط ما تحرسُه المعايرة.
#
# (AR) ولا يصلحُ «نداءٌ برتبةٍ خاطئةٍ» بديلًا — لا لأنّ الرسالةَ واحدةٌ (كان
#      هذا تبسيطًا؛ قِيس أنّ بعضَ المعروفينَ يُعطي «Too many arguments» وهو
#      تمييزٌ صحيح) بل لأنّ حارسَ الرتبةِ في المفسّرِ **غيرُ منتظم**: أسماءٌ
#      معروفةٌ تُعطي «غير معرفة» كالمخترَع (نفيٌ كاذب)، وأسماءٌ ذاتُ رتبةٍ
#      مفتوحةٍ **تُنفَّذُ فعلًا**. وجدولُ الرتبِ المولَّدُ من SoT يستهلكُه
#      المترجّمُ وحدَه (`builtin_arity_check.h`) ولا مرجعَ له في `interpreter/`
#      — ثغرةُ تكافؤٍ لها تذكرتُها، وليست أساسًا يُبنى عليه قياس.
# ---------------------------------------------------------------------------

def _stratified(names, fns_by_name, per_ns, floor, rng):
    """حصّةٌ ثابتةٌ لكلِّ مجموعةٍ، ثمّ تكملةٌ عشوائيّةٌ حتّى الأرضيّة."""
    by_ns = {}
    for n in names:
        by_ns.setdefault(fns_by_name[n].get("namespace") or "NONE", []).append(n)
    picked = []
    for ns in sorted(by_ns):
        pool = sorted(by_ns[ns])
        picked += rng.sample(pool, min(per_ns, len(pool)))
    rest = sorted(set(names) - set(picked))
    if len(picked) < floor and rest:
        picked += rng.sample(rest, min(floor - len(picked), len(rest)))
    return sorted(picked), len(by_ns)


def calibrate(fns_by_name: dict, absent: set, second_form, seed: int,
              strategy_note: str):
    """سؤالٌ ثانٍ مستقلٌّ عن العيّنةِ نفسِها، لجانبَي الحكمِ معًا.

    `second_form(name)` يُرجِعُ `True` (غائب) أو `False` (موجود) أو `None`
    (لم يُجِب الشكلُ الثاني أصلًا).

    🔑 و«لم يُجِب» ليس «خالف». قِيس أنّ الشكلَ الثانيَ في المفسّرِ **نداءٌ
    فعليّ**، فـ`اقرأ` و`قراءة_سطر` تنتظرانِ المُدخَلَ القياسيَّ حتّى تنتهيَ
    المهلة — فيُقرَأُ صمتُهما «خلافًا» وتحمرُّ الأداةُ بعلّةٍ ليست علّتَها.
    وعَدُّه اتّفاقًا كذبٌ، وعَدُّه خلافًا إنذارٌ كاذب. فيُسمّى ثالثًا:
    `unanswered` تُنزَعُ من نسبةِ الاتّفاقِ وتُسمّى بأسمائِها، وفوقها سقفٌ
    يمنعُ أن تُفرَّغَ العيّنةُ فيها فتصيرَ المعايرةُ خضراءَ على لا شيء.

    والخلافُ لا يُجمَعُ في متوسّطٍ بل يُسمّى باسمِه في `disagreements`: رقمُ
    «٢٤/٢٥» يُقرأُ نجاحًا، واسمُ المخالفِ يُقرأُ ثغرةً تُلاحَق.
    """
    present = sorted(set(fns_by_name) - absent)
    rng = random.Random(seed)
    sample_a, ns_a = _stratified(sorted(absent), fns_by_name,
                                 CALIBRATION_PER_NAMESPACE, CALIBRATION_FLOOR, rng)
    sample_p, ns_p = _stratified(present, fns_by_name,
                                 CALIBRATION_PER_NAMESPACE, CALIBRATION_FLOOR, rng)

    disagree, unanswered = [], []

    def poll(names, expected, label):
        agreed = asked = 0
        for n in names:
            verdict = second_form(n)
            entry = {"canonical": n, "probe": label,
                     "namespace": fns_by_name[n].get("namespace") or "NONE"}
            if verdict is None:
                unanswered.append(entry)
                continue
            asked += 1
            if verdict is expected:
                agreed += 1
            else:
                disagree.append(entry)
        return agreed, asked

    agree_absent, asked_absent = poll(sample_a, True, "غائب")
    agree_present, asked_present = poll(sample_p, False, "موجود")
    return {
        "seed": seed,
        "strategy": "طبقيٌّ بحسب المجموعة",
        "second_form": strategy_note,
        "per_namespace": CALIBRATION_PER_NAMESPACE,
        "absent_namespaces": ns_a,
        "present_namespaces": ns_p,
        "absent_drawn": len(sample_a),
        "absent_sample": asked_absent,
        "absent_agreed": agree_absent,
        "present_drawn": len(sample_p),
        "present_sample": asked_present,
        "present_agreed": agree_present,
        "disagreements": disagree,
        # لم يُجِب الشكلُ الثاني: يُسمَّون ولا يُحسَبون في أيِّ الجانبَين.
        "unanswered": unanswered,
    }


def calibrate_interpreter(fns_by_name, absent, run_exe, tmp, seed):
    """الشكلُ الثاني: **مِجَسُّ الجوار** — الاسمُ المقيسُ لا يُنادى قطّ.

    🔑 كان الشكلُ الثاني نداءً فعليًّا `الاسم(0)`، فكانت الأداةُ **تُنفِّذُ**
    ٦٨ مدمَجًا على جهازِ المستعمِلِ في كلِّ تشغيل. وقِيس أنّ هذا ليس نظريًّا:
    في مجلّدٍ فيه ملفٌّ اسمُه `0`، النداءُ `احذف_ملف(0)` **حذفه** — الوسيطُ
    الصوريُّ حُوِّلَ إلى النصّ «0» فلم يحمِ. وفي العيّنةِ `انبوب` و`نظام`
    و`تعطيل_مقاطعات` و`شغل_برنامجا` و`أرسل_فورا`. ومِجَسُّ المفسّرِ الأوّلُ
    بلا أثرٍ عمدًا، فكان حارسُه ينقضُ ما يحرسه.

    والعلاجُ ليس حجرًا بالـ`namespace`: قِيس أنّ ٩ من ٢٩ اسمًا خطِرًا تفلتُ
    منه، منها `احذف_ملف` نفسُه (`Basics`، جارُ `طول` و`اطبع_سطر`) — فالـ
    `namespace` تصنيفُ انتماءٍ معجميٍّ لا تصنيفُ أثر، ولا حقلَ للأثرِ في SoT.

    العلاجُ أن يُنادى **جارٌ مخترَعٌ قطعًا** (الاسمُ + حرف): `احذف_ملفظ()`.
    فيسقطُ المفسّرُ إلى فرعِ «دالّة غير معرّفة»، وهناك — قبلَ أيِّ تنفيذ —
    يُعدِّدُ سِجِلَّه الحيَّ ويقترحُ الأقربَ إليه
    (`interpreter/src/visitors/expression_evaluator_calls_dispatch.cpp:578-585`).
    فظهورُ الاسمِ في «هل قصدت» شهادةُ **تسجيلٍ في المحرّكِ نفسِه**، والمدمَجُ
    المقيسُ لم يُستدعَ. وهذا استعلامٌ عن سِجِلٍّ حيٍّ لا مسحُ نصٍّ غيرُ مُعايَر.

    قِيس: ١١٠/١١٠ في جانبِ «غائب» و٦٨/٦٨ في جانبِ «موجود» — بصفرِ تنفيذ،
    والملفُّ `0` باقٍ. ومكسبٌ عرَضيّ: `اقرأ` و`قراءة_سطر` كانتا تحجزانِ على
    المُدخَلِ القياسيِّ حتّى المهلة، وتُجيبانِ الآن فورًا.

    وحدُّه مُعلَن: «ظهرَ الاسمُ» إثباتٌ موجبٌ للحضور، و«لم يظهر» يُقرأُ غيابًا
    لأنّه وافق الشكلَ الأوّلَ في كلِّ الغائبينَ المقيسين — لا لأنّ السِّجِلَّ
    يُعدِّدُ كلَّ مسارٍ قد يحلُّ به المفسّرُ اسمًا.
    """
    src = tmp / "k.ص"

    def near_says_absent(name: str):
        # (AR) اسمٌ قصيرٌ جدًّا لا يصلحُ: جارُه يقعُ على مسافةِ تحريرٍ من عشراتِ
        #      الأسماءِ فيفوزُ غيرُه بالاقتراحِ الأوّل (يُطبَعُ `similar[0]` وحدَه،
        #      `dispatch.cpp:584`). يُترَكُ بلا جوابٍ ولا يُخمَّن — والمسافةُ
        #      تُحسَبُ بالبايتات والحرفُ العربيُّ بايتان، فعتبةُ ٤ حرفانِ عربيّان.
        if len(name.encode("utf-8")) < 4:
            return None
        src.write_text(preamble_for(name) + "\n" + name + "ظ()\n",
                       encoding="utf-8")
        rc, out = run([str(run_exe), str(src)], 60, cwd=tmp)
        if rc is None:
            return None
        return not re.search(r"هل قصدت: '" + re.escape(name) + r"'؟", out)

    return calibrate(fns_by_name, absent, near_says_absent, seed,
                     "جارٌ مخترَعٌ يُستعلَمُ به عن سِجِلِّ المفسّر؛ لا تنفيذَ البتّة")


def calibrate_compiler(fns_by_name, absent, build_exe, tmp, seed):
    """الشكلُ الثاني: نداءٌ في **التوپلفل** بدل جسمِ دالّةٍ لا تُنادى.

    مسارُ بناءِ التوپلفل غيرُ مسارِ بناءِ جسمِ الدالّة، فالسؤالُ مستقلٌّ حقًّا.
    ولا يُنفَّذُ الناتجُ: يُترجَمُ إلى `-o` ويُهمَل — فلا أثرَ جانبيّ.
    """
    src = tmp / "kc.ص"
    obj = tmp / "kc.o"

    def toplevel_says_absent(name: str):
        args = "، ".join(["0"] * arity_min(fns_by_name[name]))
        src.write_text(preamble_for(name) + "\n" + name + "(" + args
                       + ")\n" + TAIL, encoding="utf-8")
        rc, out = run(compile_only(build_exe, src, obj), 300, cwd=tmp)
        if rc is None:
            return None
        if name in SEM047.findall(out) or name in UNDEF.findall(out):
            return True
        if GATED.search(out):
            return None
        if compiler_resolves(name, rc, out):
            return False
        # مخرَجٌ لا يحملُ إشارةً معروفة: لا يُقرأُ حضورًا ولا غيابًا.
        return None

    return calibrate(fns_by_name, absent, toplevel_says_absent, seed,
                     "نداءٌ في التوپلفل؛ الحَكَمُ SEM047 أو «غير معرّفة»")


# (AR) 🔑 حرّاسُ نسبةِ القياسِ إلى الإيداعِ كانت تفشلُ **مفتوحةً**: كلُّ فشلٍ
#      في `git` (غيابُ الأمر، شجرةٌ ليست مستودعًا، مهلة) كان يُترجَمُ إلى
#      «نظيف» فيُكتَبُ سجلٌّ منسوبٌ إلى إيداعٍ لا يصفُه. وهما الحارسان اللذان
#      أُضيفا لمنعِ هذه الحادثةِ بعينها — فيجبُ أن يفشلا مغلقَين.
def _git(*argv, timeout: int = 60) -> str:
    cmd = ["git", "-C", str(ROOT), *argv]
    try:
        proc = subprocess.run(cmd, capture_output=True, timeout=timeout)
    except Exception as exc:                       # noqa: BLE001 — يُسمّى ويُوقِف
        raise SystemExit(f"✗ تعذّر تشغيلُ git ({' '.join(argv)}): {exc}\n"
                         "  لا يُنسَبُ قياسٌ إلى إيداعٍ لم يُقرأ.")
    if proc.returncode != 0:
        err = proc.stderr.decode("utf-8", "replace").strip()[:200]
        raise SystemExit(f"✗ فشل git ({' '.join(argv)}) برمز "
                         f"{proc.returncode}: {err}\n"
                         "  حارسٌ يفشلُ مفتوحًا أسوأُ من غيابِه.")
    return proc.stdout.decode("utf-8", "replace")


def git_sha() -> str:
    return _git("rev-parse", "HEAD", timeout=30).strip()


# (AR) الملفّاتُ التي تُقرِّرُ حلَّ اسمِ المدمَجِ فعلًا — لا مجلّدُ البانياتِ كلُّه.
#      ثلاثةُ حرّاسٍ يقرأونها، ولكلٍّ سؤالُه:
#        ① أفيها تعديلٌ غيرُ مُودَع؟ ⇒ الإيداعُ المسجَّلُ لا يصفُ ما قِيس.
#        ② أتغيّرت **بعدَ** بناءِ الثنائيّ؟ ⇒ الثنائيُّ أقدمُ من الشيفرة، فالقياسُ
#           يصفُ ماضيًا ويُنشَرُ حاضرًا. 🔑 وهذه وقعت فعلًا: ثنائيّا ٢٥–٢٦ آب
#           قِيسا ونُسِبا إلى إيداعِ أيلول، وبينهما أربعةُ إيداعاتٍ مسّت الإرسالَ
#           (منها إحياءُ وحدةِ صNet بـ٢٢ دالّة) — فكانت اثنتان وعشرون دالّةً
#           حيّةً ستُوثَّقُ «لا يحلُّها أيُّ محرّك». الزمنُ متغيّرٌ ثالثٌ يُقاس.
#        ③ (في `check_engine_support_fresh.py`) أتغيّرت بعدَ **الإيداعِ المقيس**؟
#           ⇒ السجلُّ يصفُ طبقةَ إرسالٍ لم تعُد قائمة. ولذلك تُستعارُ هذه
#           القائمةُ من هنا ولا تُنسَخ: نسختانِ تنجرفان.
DISPATCH_PATHS = (
    "compiler/src/frontend/builders/call_main.cpp",
    "compiler/src/frontend/builders/builtins_",
    "interpreter/src/builtins/",
    "shared/builtins/",
    "language-truth/builtins/",
)


def _porcelain_path(line: str) -> str:
    """المسارُ من سطرِ `git status --porcelain`، مع فكِّ صيغةِ إعادةِ التسمية.

    🔑 `R  قديم -> جديد` كان يُقرَأُ مسارًا واحدًا فيفلتُ الجديد. وإعادةُ
    تسميةِ ملفٍّ في طبقةِ الإرسالِ تغييرٌ فيها كسائرِ التغييرات.
    """
    path = line[3:].strip()
    if " -> " in path:
        path = path.split(" -> ", 1)[1].strip()
    return path.strip('"')


def dirty_dispatch_paths() -> list:
    """ملفّاتٌ غيرُ مُودَعةٍ في طبقةِ الإرسال — تُبطِلُ نِسبةَ القياسِ إلى الإيداع."""
    lines = _git("status", "--porcelain").splitlines()
    dirty = {_porcelain_path(line) for line in lines if line.strip()}
    return sorted(p for p in dirty
                  if any(p.startswith(prefix) for prefix in DISPATCH_PATHS))


def dispatch_files_changed_after(when: float) -> list:
    """ملفّاتُ طبقةِ الإرسالِ التي تغيّرَ **محتواها على القرص** بعدَ بناءِ الثنائيّ.

    🔑 والسؤالُ كان زمنَ الإيداعِ لا زمنَ الملفّ، وهذا خطأٌ في السؤال: مجرى
    العملِ الطبيعيُّ «ابنِ ← تحقّق ← أودِع»، فالإيداعُ يقعُ بعدَ البناءِ من
    **الشيفرةِ نفسِها** ويُقرَأُ «ثنائيٌّ أقدمُ من طبقةِ الإرسال» كذبًا. والذي
    يُبطِلُ الثنائيَّ تغيُّرُ المصدرِ لا تدوينُه في التاريخ.

    ويبقى الغرضُ الأصليُّ محفوظًا — الحادثةُ التي أُضيف لها الحارسُ (ثنائيّا
    ٢٥–٢٦ آب قِيسا ونُسِبا إلى إيداعِ أيلول وبينهما أربعةُ إيداعاتٍ مسّت
    الإرسال) تُكشَفُ كما كانت: تلك الإيداعاتُ غيّرت الملفّاتِ فعلًا فتصيرُ
    أحدثَ من الثنائيّ. أمّا الإيداعُ الذي لا يُغيّرُ ملفًّا بعدَ البناءِ فلا
    يُبطِلُ قياسًا.
    """
    changed = []
    for prefix in DISPATCH_PATHS:
        base = ROOT / prefix
        paths = [base] if base.is_file() else []
        if base.is_dir():
            paths = [p for p in base.rglob("*") if p.is_file()]
        elif not paths:
            # بادئةُ اسمٍ لا مسارٌ كامل (مثل `builtins_`) — تُوسَّعُ بالمطابقة.
            parent, stem = base.parent, base.name
            if parent.is_dir():
                paths = [p for p in parent.iterdir()
                         if p.is_file() and p.name.startswith(stem)]
        for path in paths:
            try:
                if path.stat().st_mtime > when:
                    changed.append(path.relative_to(ROOT).as_posix())
            except OSError:
                continue
    return sorted(changed)


def binary_fingerprint(path: Path) -> dict:
    """بصمةُ الثنائيِّ المقيسِ نفسِه — الحقيقةُ تُشتقُّ منه لا من عَلَمٍ خارجه."""
    stat = path.stat()
    digest = hashlib.sha256(path.read_bytes()).hexdigest()
    return {
        "path": path.name,
        "sha256": digest[:16],
        "bytes": stat.st_size,
        "built_at": time.strftime("%Y-%m-%dT%H:%M:%S",
                                  time.localtime(stat.st_mtime)),
    }


HEADER_TEMPLATE = """\
# ============================================================================
# builtin_engine_support.yaml — سِجِلُّ **قياسٍ**، لا سطحَ لغةٍ ولا مصدرَ حقيقة.
#
# لا تحرّره بيد. يُولَّد بـ:
#     python scripts/codegen/probe_builtin_engines.py --build-dir build/bin/Release
#
# ما يقوله كلُّ سطر: هل يحلُّ هذا الاسمَ **المحرّكُ نفسُه**؟ لا ما يدّعيه حقلُ
# `status` (كلُّ الـ{declarations} إعلانًا `stable`)، ولا ما يستنتجه مسحُ نصٍّ
# غيرُ مُعايَر. وصفٌّ واحدٌ لكلِّ **اسم**: الاسمُ يُعلَنُ مرّاتٍ والمحرّكُ يعرفُه
# مرّةً واحدة.
#
# الحدُّ المقصود: المقيسُ **حلُّ الاسم**، لا صحّةُ التنفيذِ ولا سلامةُ العائد.
# «مدعوم» تعني «المحرّكُ يعرفُه»، ولا تعني «قِيس أثرُه». يُقالُ ما بُرهن.
# ============================================================================
"""


def emit(path: Path, fns: list, comp_absent: set, comp_unknown: set,
         interp_absent: set, calib: dict, elapsed: float,
         engines: list, declarations: int, stale_allowed: bool,
         unmeasured: list) -> None:
    skip = {u["canonical"] for u in unmeasured}
    rows = []
    for fn in fns:
        name = fn["canonical"]
        if name in skip:
            continue
        rows.append({
            "canonical": name,
            "namespace": fn.get("namespace"),
            "module": fn.get("module"),
            "compiler": name not in comp_absent and name not in comp_unknown,
            "interpreter": name not in interp_absent,
        })
    both = sum(1 for r in rows if r["compiler"] and r["interpreter"])
    only_i = sum(1 for r in rows if r["interpreter"] and not r["compiler"])
    only_c = sum(1 for r in rows if r["compiler"] and not r["interpreter"])
    neither = sum(1 for r in rows if not r["compiler"] and not r["interpreter"])

    doc = {
        "version": 2,
        "kind": "measurement",
        "measured_commit": git_sha(),
        # (AR) بصمةُ الثنائيَّين المقيسَين: الحكمُ صادرٌ عنهما لا عن الإيداعِ وحدَه.
        "measured_binaries": engines,
        # (AR) 🔑 قياسٌ على ثنائيٍّ أقدمَ من طبقةِ الإرسالِ مسموحٌ به عمدًا
        #      بعَلَمٍ صريح — فيُدوَّنُ في المُخرَج. عَلَمٌ يُغيّرُ صدقَ السجلِّ
        #      ولا يترك أثرًا فيه دعوى نظافةٍ لا يقيسُها أحد.
        "stale_binaries_allowed": bool(stale_allowed),
        "elapsed_seconds": round(elapsed),
        "method": {
            "interpreter": ("مرجعُ اسمٍ بلا نداء؛ الحكمُ SEM001 على الاسم — "
                            "ولا يُنفَّذُ مدمَجٌ واحدٌ في المِجَسِّ ولا في معايرتِه"),
            "compiler": ("نداءٌ برتبة arity.min في دالّةٍ لا تُنادى، ترجمةٌ "
                         "بلا ربط (-c)؛ الغيابُ SEM047 ثمّ «غير معرّفة»، "
                         "والحضورُ رمزُ خروجٍ صفرٍ أو تشخيصٌ يسمّي المدمَجَ"),
            "granularity": "اسمٌ واحدٌ لكلّ تشغيل — الدفعةُ تبتُرُ فتكذب",
            "row_unit": "اسمٌ قانونيٌّ واحد (الإعلاناتُ المكرّرةُ تُوحَّدُ بأدنى رتبة)",
        },
        # (AR) 🔑 الدعوى تُقيَّدُ بما قِيس: مفتاحٌ لكلِّ محرّكٍ على حدة. كان
        #      المفتاحُ عامًّا باسم `calibration` وهو لا يصفُ إلّا المفسّر،
        #      فيُقرَأُ عمودُ المترجّمِ مُعايَرًا وهو لم يُسأل عنه سؤالٌ ثانٍ قطّ.
        "calibration": calib,
        # (AR) العجزُ يُسمّى ولا يُخفى: اسمٌ هنا لا يظهرُ في `functions`، فيعرضُه
        #      التوثيقُ «—» بدل حكمٍ مخمَّن.
        "unmeasured": unmeasured,
        "counts": {
            "declarations_in_sot": declarations,
            "unmeasured": len(unmeasured),
            "declared": len(rows),
            "compiler": sum(1 for r in rows if r["compiler"]),
            "interpreter": sum(1 for r in rows if r["interpreter"]),
            "both": both,
            "interpreter_only": only_i,
            "compiler_only": only_c,
            "neither": neither,
        },
        "functions": rows,
    }
    path.parent.mkdir(parents=True, exist_ok=True)
    body = yaml.safe_dump(doc, allow_unicode=True, sort_keys=False, width=200)
    # (AR) العددُ يُشتَقُّ ولا يُنثَر: رقمٌ مكتوبٌ في النثرِ نسخةٌ ثانيةٌ من حقيقةٍ
    #      تنجرفُ عند أوّلِ إضافةِ مدمَج.
    path.write_text(HEADER_TEMPLATE.format(declarations=declarations) + body,
                    encoding="utf-8")


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    ap = argparse.ArgumentParser(description="مِجَسُّ تغطية المحرّكَين للمدمَجات")
    ap.add_argument("--build-dir", default="build/bin/Release",
                    help="مجلّد الثنائيّات (يحوي sad-run و sad-build)")
    ap.add_argument("--out", default=str(OUT_PATH))
    ap.add_argument("--seed", type=int, default=11, help="بذرةُ عيّنةِ المعايرة")
    ap.add_argument("--engine", choices=["both", "interpreter", "compiler"],
                    default="both")
    ap.add_argument("--allow-stale-binaries", action="store_true",
                    help="اقبَل ثنائيًّا أقدمَ من آخرِ تغييرٍ في طبقةِ الإرسال")
    args = ap.parse_args()

    bin_dir = (ROOT / args.build_dir) if not Path(args.build_dir).is_absolute() \
        else Path(args.build_dir)
    run_exe = bin_dir / "sad-run.exe"
    build_exe = bin_dir / "sad-build.exe"
    for exe in (run_exe, build_exe):
        if not exe.exists():
            alt = exe.with_suffix("")
            if alt.exists():
                continue
            print(f"✗ لم يُعثر على {exe} — ابنِ المشروعَ أو مرّر --build-dir.")
            return 1
    if not run_exe.exists():
        run_exe = run_exe.with_suffix("")
    if not build_exe.exists():
        build_exe = build_exe.with_suffix("")

    dirty = dirty_dispatch_paths()
    if dirty:
        print("✗ طبقةُ الإرسالِ فيها تعديلٌ غيرُ مُودَع — لا يُنسَبُ قياسٌ إلى إيداعٍ")
        print("  لا يوافقُ الثنائيَّ المقيس. أودِعْ أو نظِّف ثمّ أعِد التشغيل:")
        for path in dirty[:10]:
            print(f"    · {path}")
        return 1

    # (AR) الثنائيُّ أقدمُ من طبقةِ الإرسال ⇒ القياسُ يصفُ ماضيًا ويُنشَرُ حاضرًا.
    built_at = min(run_exe.stat().st_mtime, build_exe.stat().st_mtime)
    late = dispatch_files_changed_after(built_at)
    if late and not args.allow_stale_binaries:
        stamp = time.strftime("%Y-%m-%d %H:%M", time.localtime(built_at))
        print(f"✗ الثنائيّان بُنيا في {stamp}، وبعدهما تغيّر {len(late)} ملفًّا")
        print("  في طبقةِ الإرسال. أعِد البناءَ ثمّ القياس — أو مرِّر")
        print("  --allow-stale-binaries إن كنتَ تقيسُ عمدًا حالةً قديمة:")
        for line in late[:10]:
            print(f"    · {line}")
        return 1

    declarations = load_builtins()
    fns = unify_declarations(declarations)
    fns_by_name = {f["canonical"]: f for f in fns}

    # شرَكٌ لتصادمٍ لا يُحَلُّ بحذفِ الاستيراد: مدمَجٌ اسمُه اسمُ وحدتِه نفسِها.
    # لا وجودَ له اليوم، ويومَ يوجدُ يجبُ أن يقفَ المِجَسُّ لا أن يُخمّن.
    irreducible = sorted(f["canonical"] for f in fns
                         if collides_with_own_module(f))
    if irreducible:
        print("✗ تصادمُ اسمٍ لا يُحَلُّ — الاسمُ هو اسمُ وحدتِه:")
        for name in irreducible:
            print(f"    · {name}")
        print("  لا يُكتَبُ المُخرَج: الحكمُ هنا تخمينٌ لا قياس.")
        return 1
    names = sorted(fns_by_name)
    print(f"معلَنٌ في مصدر الحقيقة: {len(declarations)} إعلانًا "
          f"({len(names)} اسمًا قانونيًّا)", flush=True)

    # (AR) 🔑 البصمةُ تُلتقَطُ **قبلَ** القياسِ وبعدَه وتُقارَن: بناءٌ متزامنٌ أو
    #      `checkout` أثناءَ التشغيلِ يجعلُ البصمةَ المسجَّلةَ غيرَ المقيسة.
    engines_before = [binary_fingerprint(run_exe), binary_fingerprint(build_exe)]

    started = time.time()
    # (AR) `ignore_cleanup_errors`: المجلّدُ يحوي ناتجَ ربطٍ (`c.exe`) قد يبقى
    #      مقفولًا لحظةً (مضادُّ فيروسات، رابطٌ لم ينتهِ بعد المهلة). بدونها
    #      يرفعُ الخروجُ من `with` استثناءً فيضيعُ قياسٌ استغرقَ دقائق.
    with tempfile.TemporaryDirectory(ignore_cleanup_errors=True) as td:
        tmp = Path(td)

        interp_absent, interp_unclear = set(), []
        if args.engine in ("both", "interpreter"):
            t0 = time.time()
            interp_absent, interp_unclear = probe_interpreter(names, run_exe, tmp)
            print(f"[المفسّر] لا يحلُّ الاسم: {len(interp_absent)} · "
                  f"ملتبس: {len(interp_unclear)} · "
                  f"{round(time.time() - t0)}ث", flush=True)

        comp_absent, comp_unknown, comp_unclear = set(), set(), []
        if args.engine in ("both", "compiler"):
            t0 = time.time()
            comp_absent, comp_unknown, comp_ok, comp_unclear = probe_compiler(
                fns, build_exe, tmp)
            print(f"[المترجّم] SEM047: {len(comp_absent)} · "
                  f"اسمٌ مجهول: {len(comp_unknown)} · "
                  f"بُني بنجاح: {len(comp_ok)} · "
                  f"ملتبس: {len(comp_unclear)} · "
                  f"{round(time.time() - t0)}ث", flush=True)

        if args.engine != "both":
            # (AR) رمزٌ مميَّزٌ لا صفر: «لم يُقس» ليس نجاحًا، ووظيفةُ CI تخضرُّ
            #      على الصفرِ فتُقرَأُ قياسًا وقد لم تكتبْ شيئًا.
            print("مِجَسٌّ جزئيّ — لا يُكتَبُ المُخرَج.")
            return 2

        # (AR) الالتباسُ يُفحَصُ **قبلَ** المعايرة: الأسماءُ الملتبسةُ تدخلُ
        #      عيّنةَ «موجود» فتُنتِجُ خلافًا، فتحمرُّ الأداةُ برسالةِ «غيرُ
        #      مُعايَر» والسببُ الحقيقيُّ التباسٌ لم يُصنَّف.
        unmeasured = [{"canonical": n, "engine": eng, "reason": why}
                      for eng, bucket in (("interpreter", interp_unclear),
                                          ("compiler", comp_unclear))
                      for n, why in bucket]
        unmeasured.sort(key=lambda u: (u["canonical"], u["engine"]))
        if len(unmeasured) > MAX_UNMEASURED:
            print(f"✗ ملتبسٌ لم يُصنَّف: {len(unmeasured)} — والسقفُ "
                  f"{MAX_UNMEASURED}. لا يُكتَبُ المُخرَجُ ناقصًا:")
            for u in unmeasured[:15]:
                print(f"    · {u['canonical']} [{u['engine']}]: {u['reason']}")
            return 1
        if unmeasured:
            print(f"⚠ تعذّر قياسُ {len(unmeasured)} اسمًا — تُنزَعُ من السِّجِلِّ "
                  "وتُسمّى فيه بأسبابِها:")
            for u in unmeasured:
                print(f"    · {u['canonical']} [{u['engine']}]: {u['reason']}")

        # (AR) المُتعذِّرُ قياسُه يُنزَعُ من عيّنةِ المعايرةِ أيضًا: اسمٌ لم يُحسَم
        #      حكمُه لا يصلحُ شاهدًا على اتّفاقِ شكلَين، وإبقاؤه يُحمِرُّ الأداةَ
        #      بعلّةٍ غيرِ علّتِها.
        measurable = {n: f for n, f in fns_by_name.items()
                      if n not in {u["canonical"] for u in unmeasured}}
        calib = {
            "interpreter": calibrate_interpreter(
                measurable, interp_absent & set(measurable), run_exe, tmp,
                args.seed),
            "compiler": calibrate_compiler(
                measurable, (comp_absent | comp_unknown) & set(measurable),
                build_exe, tmp, args.seed),
        }

    ok = True
    for engine, label in (("interpreter", "المفسّر"), ("compiler", "المترجّم")):
        side = calib[engine]
        print(f"[معايرة {label}] «غائب» ⇒ غائبٌ بالشكلِ الثاني: "
              f"{side['absent_agreed']}/{side['absent_sample']} · "
              f"«موجود» ⇒ ليس غائبًا: "
              f"{side['present_agreed']}/{side['present_sample']}")
        if side["unanswered"]:
            print(f"  ⚠ لم يُجِب الشكلُ الثاني عن {len(side['unanswered'])} اسمًا "
                  "(نُزِعت من النسبة):")
            for u in side["unanswered"]:
                print(f"      · {u['canonical']} ({u['namespace']})")
        if (side["absent_agreed"] != side["absent_sample"]
                or side["present_agreed"] != side["present_sample"]
                or not side["absent_sample"] or not side["present_sample"]
                or len(side["unanswered"]) > MAX_UNANSWERED):
            ok = False
    if not ok:
        print("✗ المِجَسُّ غيرُ مُعايَر: الحكمان اختلفا على العيّنةِ نفسِها.")
        print("  لا يُكتَبُ المُخرَج — رقمٌ من أداةٍ لم تُعايَر أسوأُ من السكوت.")
        # رفضٌ بلا اسمٍ يُوقِفُ الأداةَ ولا يدلُّ على موضعِ الخلل.
        for engine, label in (("interpreter", "المفسّر"), ("compiler", "المترجّم")):
            for d in calib[engine].get("disagreements") or []:
                print(f"  ⚠ [{label}] {d['canonical']} ({d['namespace']}) — "
                      f"المِجَسُّ قال «{d['probe']}» والشكلُ الثاني خالفه")
        return 1

    engines_after = [binary_fingerprint(run_exe), binary_fingerprint(build_exe)]
    if [e["sha256"] for e in engines_before] != [e["sha256"] for e in engines_after]:
        print("✗ تغيّر الثنائيّان أثناءَ القياس — البصمةُ المسجَّلةُ لن تكونَ")
        print("  البصمةَ المقيسة. أعِد التشغيلَ بلا بناءٍ متزامن.")
        return 1

    out_path = Path(args.out)
    emit(out_path, fns, comp_absent, comp_unknown, interp_absent, calib,
         time.time() - started, engines_after, len(declarations),
         args.allow_stale_binaries, unmeasured)
    print(f"✓ كُتب: {out_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
