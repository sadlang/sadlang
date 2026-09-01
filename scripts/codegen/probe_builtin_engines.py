#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِجَسُّ «على أيِّ محرّكٍ يعملُ هذا المدمَج؟» — يُقاسُ ولا يُعلَن.

     ¶ لماذا مِجَسٌّ أصلًا، ومصدرُ الحقيقةِ فيه حقلُ `status`؟

       لأنّ `status: stable` **لا يعني أنّ المدمَجَ يصلُ إلى ثنائيّ**. الـ١٢٠٥
       مدمَجًا كلُّها `stable` في `language-truth/builtins/`، ومع ذلك يرفضُ
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
         وتنفيذُ ١٢٠٥ مدمَجًا يوقفُ المعالجَ ويفتحُ مقابسَ ويحذفُ ملفّات.
         والرمزُ الحاكمُ SEM001 على الاسمِ نفسِه.

       • المترجّم: نداءٌ برتبةِ `arity.min` داخلَ **دالّةٍ لا تُنادى** — أجسامُ
         الدوالِّ تُبنى قبلَ التوپلفل، فيصدرُ الحكمُ بلا تنفيذ. والرمزُ الحاكمُ
         SEM047 المسمّى («معلَنةٌ … ولا توزيعَ لها في المترجم») أو رسالةُ
         «استدعاء دالة غير معرّفة». (والمترجّمُ لا يقبلُ مرجعَ مدمَجٍ بلا نداء
         — قِيس: «Undefined variable» حتّى للعامل، فالشكلُ الواحدُ لا يكفي
         للمحرّكَين.)

     ¶ 🔑 لماذا اسمٌ واحدٌ لكلّ تشغيلٍ لا دفعة؟

       الدفعةُ جُرِّبت وكانت أسرعَ **وأكذب**: خطأُ نوعٍ من وسيطٍ صوريٍّ في اسمٍ
       سابقٍ يبتُرُ المسحَ، فتُقرَأُ الأسماءُ التاليةُ «مدعومةً» وهي لم تُفحَص.
       قِيست دفعاتٌ أعطت صفرَ غيابٍ ومعها سبعةَ عشرَ خطأً من صنفٍ آخر. الأداةُ
       التي تختصرُ الزمنَ باختصارِ القياسِ تُفسِدُ ما تقيسه.

     ¶ المعايرة (تُنفَّذُ في كلِّ تشغيلٍ وتُدوَّنُ في المُخرَج)

       يُسأَلُ عن عيّنةٍ سؤالٌ **ثانٍ مستقلّ** (النداءُ الفعليُّ في المفسّرِ بدل
       المرجع)، ويُقارَنُ الحكمان. اختلافُهما يعني أنّ المِجَسَّ يقيسُ شيئًا غيرَ
       الذي يُنشَر — فيُعطَّلُ المُخرَجُ ولا يُكتَب.

(EN) Measures, per builtin, whether each engine actually resolves it. The
     SoT `status` field is a claim, not a measurement; source scanning is
     uncalibrated. One name per run: batching truncates and lies.
============================================================================
"""
from __future__ import annotations

import argparse
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
IMPORT_MODULES = ["نصوص", "أساسيات", "رياضيات", "تأكيدات", "خرائط",
                  "تزامن_متقدم", "منصة", "شبكة", "مقابس", "تشفير"]


# (AR) 🔑 تصادمُ الاسمِ يحلُّ صامتًا — وقد وقع: `متغير م = شبكة` يحلُّ لأنّ
#      التمهيدَ فيه `استورد شبكة`، فيُرجَعُ إلى **الوحدة** لا إلى المدمَجِ
#      المسمّى `شبكة` (UIWidgets). فيُقرأُ «يحلُّه المفسّر» وهو لا يحلُّه:
#      قِيس أنّ حذفَ استيرادِ وحدتِه يقلبُ الحكمَ إلى SEM001، وأنّ نداءَه
#      يقولُ SEM004. أداةُ القياسِ كانت تُفسِدُ قياسَها بتمهيدِها.
#      والعلاجُ: يُحذَفُ من التمهيدِ استيرادُ الوحدةِ التي تُشبهُ الاسمَ المقيس.
#      وإن كان المدمَجُ نفسُه ساكنًا في تلك الوحدة فالتصادمُ لا يُحَلُّ بحذفٍ
#      ولا يُخمَّن: يُصنَّفُ **ملتبسًا** فيَمنعُ كتابةَ المُخرَج.
def preamble_for(name: str) -> str:
    """تمهيدٌ خاصٌّ بالاسمِ المقيس: بلا الوحدةِ التي تُصادمُه."""
    return "\n".join("استورد " + m for m in IMPORT_MODULES if m != name)


def collides_with_own_module(fn) -> bool:
    """اسمٌ يساوي اسمَ وحدتِه — تصادمٌ لا يُحَلُّ بحذفِ الاستيراد."""
    return fn["canonical"] in IMPORT_MODULES and fn.get("module") == fn["canonical"]
TAIL = 'اطبع_سطر("حي")\n'

ANSI = re.compile(r"\x1b\[[0-9;]*m")
SEM001 = re.compile(r"المتغير '([^']+)'")          # اسمٌ لا يحلُّ في المفسّر
SEM004 = re.compile(r"الدالة '([^']+)' غير معرفة")  # سؤالُ المعايرةِ الثاني
SEM047 = re.compile(r"المدمَجة '([^']+)'")          # معلَنٌ بلا توزيعٍ في المترجم
UNDEF = re.compile(r"استدعاء دالة غير معرّفة '([^']+)'")

# (AR) المعايرةُ **طبقيّةٌ** لا عشوائيّةً منتظمة. العشوائيُّ المنتظمُ يقيسُ
#      المجموعاتِ الكبيرةَ ويكادُ لا يلمسُ الصغيرة، وحكمُ المِجَسِّ ليس واحدًا
#      عبرها: المدمَجُ قد يكونُ منادًى في الإرسالِ دون أن يكونَ قيمةً أُولى
#      يُرجَعُ إليها باسمِها المجرَّد، وهذا اختلافُ **شكلٍ** لا اختلافُ وجود.
#      فلو أخطأ الشكلُ في مجموعةٍ بأسرِها لَذابَ خطؤها في متوسّطِ عيّنةٍ
#      منتظمة. الطبقيُّ يُلزِمُ كلَّ مجموعةٍ بحصّةٍ، فيظهرُ الخللُ حيثُ هو.
CALIBRATION_PER_NAMESPACE = 3   # حصّةُ كلِّ مجموعةٍ من كلِّ جانب
CALIBRATION_FLOOR = 25          # أرضيّةٌ لكلِّ جانبٍ مهما قلَّت المجموعات


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


def run(cmd: list, timeout: int) -> str:
    try:
        proc = subprocess.run(cmd, capture_output=True, timeout=timeout)
    except subprocess.TimeoutExpired:
        return "TIMEOUT"
    return ANSI.sub("", (proc.stdout + proc.stderr).decode("utf-8", "replace"))


def probe_interpreter(names: list, run_exe: Path, tmp: Path):
    """مرجعُ اسمٍ بلا نداء — لا يُنفَّذُ مدمَجٌ واحد."""
    absent, unclear = set(), []
    src = tmp / "i.ص"
    for name in names:
        src.write_text(preamble_for(name) + "\nمتغير مـجـس = " + name
                       + "\n" + TAIL, encoding="utf-8")
        out = run([str(run_exe), str(src)], 60)
        if out == "TIMEOUT":
            unclear.append((name, "timeout"))
            continue
        if "حي" in out and "error [" not in out:
            continue
        match = SEM001.search(out)
        if match and match.group(1) == name:
            absent.add(name)
        else:
            head = out.strip().splitlines()[0][:70] if out.strip() else "فارغ"
            unclear.append((name, head))
    return absent, unclear


def probe_compiler(fns: list, build_exe: Path, tmp: Path):
    """نداءٌ داخلَ دالّةٍ لا تُنادى — الحكمُ أماميٌّ بلا تنفيذ."""
    absent, unknown, unclear = set(), set(), []
    src = tmp / "c.ص"
    exe = tmp / "c.exe"
    for fn in fns:
        name = fn["canonical"]
        args = "، ".join(["0"] * arity_min(fn))
        src.write_text(preamble_for(name) + "\nدالة مجس()\n    " + name
                       + "(" + args + ")\nنهاية\n" + TAIL,
                       encoding="utf-8")
        out = run([str(build_exe), str(src), "-o", str(exe)], 300)
        if out == "TIMEOUT":
            unclear.append((name, "timeout"))
            continue
        if name in SEM047.findall(out):
            absent.add(name)
        elif name in UNDEF.findall(out):
            unknown.add(name)
    return absent, unknown, unclear


# ---------------------------------------------------------------------------
# (AR) لماذا للمفسّرِ شكلٌ غيرُ شكلِ المترجّم؟ لأنّه لا شكلَ لنداءٍ **بلا أثر**
#      يراهُ المفسّرُ ساكنًا. جُرِّبت ثلاثةٌ، وكلُّها عمياء — قِيست لا افتُرضت:
#        ① نداءٌ داخل دالّةٍ لا تُنادى ..... يطبعُ «حي» حتّى لاسمٍ مخترَعٍ لا وجودَ له
#        ② نداءٌ في فرعٍ ميّت `إذا (1 == 2)` .. كذلك
#        ③ `--فحص-الأنواع` مع ① .......... كذلك
#      و`sad-run check` غيرُ موجودٍ أصلًا رغم إعلانِه في لافتةِ الاستعمال.
#      فبقيَ المرجعُ المجرَّدُ `متغير م = الاسم` وحدَه، وهو يقيسُ **حلَّ الاسمِ
#      قيمةً أُولى**. والفرقُ بينه وبين النداءِ هو بالضبط ما تحرسُه المعايرة.
#
# (AR) ولا يصلحُ «نداءٌ برتبةٍ خاطئةٍ» بديلًا: رسالةُ SEM004 واحدةٌ للاسمِ
#      المعروفِ والمخترَعِ معًا («غير معرفة بعدد معاملات ٧») — فلا تُميّز.
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


def calibrate(fns_by_name: dict, absent: set, run_exe: Path, tmp: Path, seed: int):
    """سؤالٌ ثانٍ مستقلٌّ عن العيّنةِ نفسِها: النداءُ الفعليُّ بدل المرجع.

    الخلافُ لا يُجمَعُ في متوسّطٍ بل يُسمّى باسمِه في `disagreements`: رقمُ
    «٢٤/٢٥» يُقرأُ نجاحًا، واسمُ المخالفِ يُقرأُ ثغرةً تُلاحَق.
    """
    present = sorted(set(fns_by_name) - absent)
    rng = random.Random(seed)
    sample_a, ns_a = _stratified(sorted(absent), fns_by_name,
                                 CALIBRATION_PER_NAMESPACE, CALIBRATION_FLOOR, rng)
    sample_p, ns_p = _stratified(present, fns_by_name,
                                 CALIBRATION_PER_NAMESPACE, CALIBRATION_FLOOR, rng)
    src = tmp / "k.ص"

    def call_says_absent(name: str):
        args = "، ".join(["0"] * arity_min(fns_by_name[name]))
        src.write_text(preamble_for(name) + "\n" + name + "(" + args
                       + ")\n" + TAIL, encoding="utf-8")
        out = run([str(run_exe), str(src)], 60)
        if out == "TIMEOUT":
            return None
        match = SEM004.search(out)
        return bool(match and match.group(1) == name)

    disagree = []
    agree_absent = 0
    for n in sample_a:
        verdict = call_says_absent(n)
        if verdict is True:
            agree_absent += 1
        else:
            disagree.append({"canonical": n, "probe": "غائب",
                             "namespace": fns_by_name[n].get("namespace") or "NONE"})
    agree_present = 0
    for n in sample_p:
        verdict = call_says_absent(n)
        if verdict is False:
            agree_present += 1
        else:
            disagree.append({"canonical": n, "probe": "موجود",
                             "namespace": fns_by_name[n].get("namespace") or "NONE"})
    return {
        "seed": seed,
        "strategy": "طبقيٌّ بحسب المجموعة",
        "per_namespace": CALIBRATION_PER_NAMESPACE,
        "absent_namespaces": ns_a,
        "present_namespaces": ns_p,
        "absent_sample": len(sample_a),
        "absent_agreed": agree_absent,
        "present_sample": len(sample_p),
        "present_agreed": agree_present,
        "disagreements": disagree,
    }


def git_sha() -> str:
    try:
        out = subprocess.run(["git", "-C", str(ROOT), "rev-parse", "HEAD"],
                             capture_output=True, timeout=30)
        return out.stdout.decode().strip() or "غير معروف"
    except Exception:
        return "غير معروف"


# (AR) الملفّاتُ التي تُقرِّرُ حلَّ اسمِ المدمَجِ فعلًا — لا مجلّدُ البانياتِ كلُّه.
#      حارسانِ يقرآنها، ولكلٍّ سؤالُه:
#        ① أفيها تعديلٌ غيرُ مُودَع؟ ⇒ الإيداعُ المسجَّلُ لا يصفُ ما قِيس.
#        ② أتغيّرت **بعدَ** بناءِ الثنائيّ؟ ⇒ الثنائيُّ أقدمُ من الشيفرة، فالقياسُ
#           يصفُ ماضيًا ويُنشَرُ حاضرًا. 🔑 وهذه وقعت فعلًا: ثنائيّا ٢٥–٢٦ آب
#           قِيسا ونُسِبا إلى إيداعِ أيلول، وبينهما أربعةُ إيداعاتٍ مسّت الإرسالَ
#           (منها إحياءُ وحدةِ صNet بـ٢٢ دالّة) — فكانت اثنتان وعشرون دالّةً
#           حيّةً ستُوثَّقُ «لا يحلُّها أيُّ محرّك». الزمنُ متغيّرٌ ثالثٌ يُقاس.
DISPATCH_PATHS = (
    "compiler/src/frontend/builders/call_main.cpp",
    "compiler/src/frontend/builders/builtins_",
    "interpreter/src/builtins/",
    "shared/builtins/",
    "language-truth/builtins/",
)


def dirty_dispatch_paths() -> list:
    """ملفّاتٌ غيرُ مُودَعةٍ في طبقةِ الإرسال — تُبطِلُ نِسبةَ القياسِ إلى الإيداع."""
    try:
        out = subprocess.run(["git", "-C", str(ROOT), "status", "--porcelain"],
                             capture_output=True, timeout=60)
        lines = out.stdout.decode("utf-8", "replace").splitlines()
    except Exception:
        return []
    dirty = []
    for line in lines:
        path = line[3:].strip().strip('"')
        if any(path.startswith(p) for p in DISPATCH_PATHS):
            dirty.append(path)
    return sorted(dirty)


def dispatch_commits_after(when: float) -> list:
    """إيداعاتٌ مسّت طبقةَ الإرسالِ بعدَ لحظةِ بناءِ الثنائيّ."""
    stamp = time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime(when))
    try:
        out = subprocess.run(
            ["git", "-C", str(ROOT), "log", "--oneline", f"--since={stamp}",
             "--", *DISPATCH_PATHS],
            capture_output=True, timeout=60)
        return [l for l in out.stdout.decode("utf-8", "replace").splitlines() if l]
    except Exception:
        return []


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


HEADER = """\
# ============================================================================
# builtin_engine_support.yaml — سِجِلُّ **قياسٍ**، لا سطحَ لغةٍ ولا مصدرَ حقيقة.
#
# لا تحرّره بيد. يُولَّد بـ:
#     python scripts/codegen/probe_builtin_engines.py --build-dir build/bin/Release
#
# ما يقوله كلُّ سطر: هل يحلُّ هذا الاسمَ **المحرّكُ نفسُه**؟ لا ما يدّعيه حقلُ
# `status` (كلُّ الـ١٢٠٥ `stable`)، ولا ما يستنتجه مسحُ نصٍّ غيرُ مُعايَر.
#
# الحدُّ المقصود: المقيسُ **حلُّ الاسم**، لا صحّةُ التنفيذِ ولا سلامةُ العائد.
# «مدعوم» تعني «المحرّكُ يعرفُه»، ولا تعني «قِيس أثرُه». يُقالُ ما بُرهن.
# ============================================================================
"""


def emit(path: Path, fns: list, comp_absent: set, comp_unknown: set,
         interp_absent: set, calib: dict, elapsed: float,
         engines: list) -> None:
    rows = []
    for fn in fns:
        name = fn["canonical"]
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
        "version": 1,
        "kind": "measurement",
        "measured_commit": git_sha(),
        # (AR) بصمةُ الثنائيَّين المقيسَين: الحكمُ صادرٌ عنهما لا عن الإيداعِ وحدَه.
        "measured_binaries": engines,
        "elapsed_seconds": round(elapsed),
        "method": {
            "interpreter": "مرجعُ اسمٍ بلا نداء؛ الحكمُ SEM001 على الاسم",
            "compiler": "نداءٌ برتبة arity.min في دالّةٍ لا تُنادى؛ الحكمُ SEM047",
            "granularity": "اسمٌ واحدٌ لكلّ تشغيل — الدفعةُ تبتُرُ فتكذب",
        },
        "calibration": calib,
        "counts": {
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
    path.write_text(HEADER + body, encoding="utf-8")


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
    late = dispatch_commits_after(built_at)
    if late and not args.allow_stale_binaries:
        stamp = time.strftime("%Y-%m-%d %H:%M", time.localtime(built_at))
        print(f"✗ الثنائيّان بُنيا في {stamp}، وبعدهما {len(late)} إيداعًا مسَّ")
        print("  طبقةَ الإرسال. أعِد البناءَ ثمّ القياس — أو مرِّر")
        print("  --allow-stale-binaries إن كنتَ تقيسُ عمدًا حالةً قديمة:")
        for line in late[:10]:
            print(f"    · {line}")
        return 1

    fns = load_builtins()
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
    print(f"معلَنٌ في مصدر الحقيقة: {len(fns)} مدمَجًا "
          f"({len(names)} اسمًا قانونيًّا)", flush=True)

    started = time.time()
    with tempfile.TemporaryDirectory() as td:
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
            comp_absent, comp_unknown, comp_unclear = probe_compiler(
                fns, build_exe, tmp)
            print(f"[المترجّم] SEM047: {len(comp_absent)} · "
                  f"اسمٌ مجهول: {len(comp_unknown)} · "
                  f"ملتبس: {len(comp_unclear)} · "
                  f"{round(time.time() - t0)}ث", flush=True)

        if args.engine != "both":
            print("مِجَسٌّ جزئيّ — لا يُكتَبُ المُخرَج.")
            return 0

        calib = calibrate(fns_by_name, interp_absent, run_exe, tmp, args.seed)

    ok = (calib["absent_agreed"] == calib["absent_sample"]
          and calib["present_agreed"] == calib["present_sample"])
    print(f"[المعايرة] «غائب» ⇒ غائبٌ بالنداء: "
          f"{calib['absent_agreed']}/{calib['absent_sample']} · "
          f"«موجود» ⇒ ليس غائبًا: "
          f"{calib['present_agreed']}/{calib['present_sample']}")
    if not ok:
        print("✗ المِجَسُّ غيرُ مُعايَر: الحكمان اختلفا على العيّنةِ نفسِها.")
        print("  لا يُكتَبُ المُخرَج — رقمٌ من أداةٍ لم تُعايَر أسوأُ من السكوت.")
        # رفضٌ بلا اسمٍ يُوقِفُ الأداةَ ولا يدلُّ على موضعِ الخلل.
        for d in calib.get("disagreements") or []:
            print(f"  ⚠ {d['canonical']} ({d['namespace']}) — "
                  f"المرجعُ المجرَّدُ قال «{d['probe']}» والنداءُ خالفه")
        return 1
    if interp_unclear or comp_unclear:
        print(f"✗ ملتبسٌ لم يُصنَّف: {len(interp_unclear) + len(comp_unclear)} —"
              " لا يُكتَبُ المُخرَجُ ناقصًا.")
        for name, why in (interp_unclear + comp_unclear)[:10]:
            print(f"    · {name}: {why}")
        return 1

    out_path = Path(args.out)
    emit(out_path, fns, comp_absent, comp_unknown, interp_absent, calib,
         time.time() - started,
         [binary_fingerprint(run_exe), binary_fingerprint(build_exe)])
    print(f"✓ كُتب: {out_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
