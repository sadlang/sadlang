#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
مولّد اختبارات سلوك عميقة لاستيعابات لغة ص (ترتيب أنتج) — RFC 25 م1ب.
يحاكي دلالة ص في بايثون ليحسب @expected حتميًّا (قائمة/مجموعة/قاموس).
كلّ ملفّ يُنفَّذ لاحقًا عبر runner.py على المفسّر والمترجم (تكافؤ مزدوج).
"""
import os, sys

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
AR_COMMA = "،"

def arr(nums):
    return "[" + AR_COMMA.join(str(n) for n in nums) + " ]".strip()

def arr_lit(nums):
    return "[" + (AR_COMMA + " ").join(str(n) for n in nums) + "]"

# تعبيرات الناتج: (اسم، دالّة بايثون، نصّ ص بدلالة المتغيّر «س»)
OUT_EXPRS = [
    ("id",     lambda s: s,        "س"),
    ("double", lambda s: s*2,      "س * 2"),
    ("square", lambda s: s*s,      "س * س"),
    ("inc",    lambda s: s+1,      "س + 1"),
    ("dec",    lambda s: s-1,      "س - 1"),
    ("times10",lambda s: s*10,     "س * 10"),
    ("mod3",   lambda s: s%3,      "س % 3"),
    ("plus5",  lambda s: s+5,      "س + 5"),
    ("neg",    lambda s: -s,       "0 - س"),
    ("triple", lambda s: s*3,      "س * 3"),
]

# شروط: (اسم، دالّة بايثون، نصّ ص)
CONDS = [
    ("gt2",   lambda s: s > 2,      "س > 2"),
    ("ge3",   lambda s: s >= 3,     "س >= 3"),
    ("lt4",   lambda s: s < 4,      "س < 4"),
    ("even",  lambda s: s % 2 == 0, "س % 2 == 0"),
    ("odd",   lambda s: s % 2 == 1, "س % 2 == 1"),
    ("ne3",   lambda s: s != 3,     "س != 3"),
    ("gt0",   lambda s: s > 0,      "س > 0"),
    ("ge2",   lambda s: s >= 2,     "س >= 2"),
]

SOURCES = [
    [1, 2, 3],
    [1, 2, 3, 4],
    [1, 2, 3, 4, 5],
    [2, 4, 6],
    [5, 3, 1],
    [1, 1, 2, 3],
    [3, 1, 3, 2, 1],
    [10, 20, 30],
    [0, 1, 2],
    [4, 4, 4],
    [1, 2, 2, 3, 3, 3],
    [7],
    [6, 5, 4, 3, 2, 1],
]

def write(path, content):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(content)

def header(rule, desc, expected, requires="اطبع_سطر", priority="P2", expect_error=False):
    lines = [f"# @rule: {rule}", f"# @description: {desc}", f"# @priority: {priority}",
             f"# @requires: {requires}"]
    if expect_error:
        lines.append("# @expect_error")
    else:
        lines.append(f"# @expected: {expected}")
    return "\n".join(lines) + "\n"

count = {"list": 0, "set": 0, "dict": 0, "neg": 0}
files = []

def emit(rule_dir, cat, idx, name, body, rule, desc, expected, requires="اطبع_سطر", priority="P2", expect_error=False):
    fn = f"{idx:03d}_{name}.ص"
    path = os.path.join(ROOT, "60_advanced", rule_dir, cat, fn)
    content = header(rule, desc, expected, requires, priority, expect_error) + body + "\n"
    write(path, content)
    files.append(path)

# ─────────────────────────────────────────────────────────────
# استيعاب القائمة — أساسيّ (خريطة/حساب) + probe بالفهرسة والطول
# ─────────────────────────────────────────────────────────────
idx = 100
for src in SOURCES:
    for oname, ofn, oexpr in OUT_EXPRS:
        vals = [ofn(s) for s in src]
        # probe: طول ثمّ آخر عنصر
        expected = str(vals[-1])
        body = (f"متغير ز = [لكل س في {arr_lit(src)} أنتج {oexpr}]\n"
                f"اطبع_سطر(ز[{len(vals)-1}])")
        emit("gr.adv.list_comprehension", "basic", idx, f"map_{oname}_{len(src)}",
             body, "gr.adv.list_comprehension",
             f"استيعاب قائمة: أنتج {oexpr} على مصدر بطول {len(src)}؛ probe آخر عنصر", expected)
        idx += 1
        count["list"] += 1
        if count["list"] >= 40:
            break
    if count["list"] >= 40:
        break

# قائمة — طول بعد ترشيح (edge)
idx = 100
ecount = 0
for src in SOURCES:
    for cname, cfn, cexpr in CONDS:
        filtered = [s for s in src if cfn(s)]
        expected = str(len(filtered))
        body = (f"متغير ز = [لكل س في {arr_lit(src)} إذا {cexpr} أنتج س]\n"
                f"اطبع_سطر(طول(ز))")
        emit("gr.adv.list_comprehension", "edge", idx, f"filter_{cname}_len{len(src)}",
             body, "gr.adv.list_comprehension",
             f"استيعاب قائمة بترشيح {cexpr}؛ طول الناتج", expected, requires="اطبع_سطر، طول")
        idx += 1
        ecount += 1
        if ecount >= 24:
            break
    if ecount >= 24:
        break

# قائمة — ترشيح + خريطة معًا، probe أوّل عنصر (edge)
idx = 200
e2 = 0
for src in [[1,2,3,4,5],[2,4,6,8],[1,3,5,7],[6,5,4,3,2,1],[10,20,30,40]]:
    for (oname,ofn,oexpr),(cname,cfn,cexpr) in [
        (OUT_EXPRS[1],CONDS[0]),(OUT_EXPRS[2],CONDS[3]),(OUT_EXPRS[5],CONDS[2]),
        (OUT_EXPRS[3],CONDS[6]),(OUT_EXPRS[0],CONDS[7])]:
        vals=[ofn(s) for s in src if cfn(s)]
        if not vals:  # نتجنّب الفهرسة على فارغ
            continue
        expected=str(vals[0])
        body=(f"متغير ز = [لكل س في {arr_lit(src)} إذا {cexpr} أنتج {oexpr}]\n"
              f"اطبع_سطر(ز[0])")
        emit("gr.adv.list_comprehension","edge",idx,f"fmap_{cname}_{oname}",
             body,"gr.adv.list_comprehension",
             f"استيعاب قائمة ترشيح {cexpr} ثمّ أنتج {oexpr}؛ probe أوّل", expected)
        idx+=1; e2+=1

# ─────────────────────────────────────────────────────────────
# استيعاب المجموعة — إزالة تكرار على قيمة الناتج (basic/edge)
# ─────────────────────────────────────────────────────────────
def dedup_order(xs):
    seen=set(); out=[]
    for x in xs:
        if x not in seen:
            seen.add(x); out.append(x)
    return out

idx=100
scount=0
for src in SOURCES:
    for oname,ofn,oexpr in OUT_EXPRS[:6]:
        vals=dedup_order([ofn(s) for s in src])
        expected=str(len(vals))
        body=(f"متغير م = {{لكل س في {arr_lit(src)} أنتج {oexpr}}}\n"
              f"اطبع_سطر(طول(م))")
        emit("gr.adv.set_comprehension","basic",idx,f"dedup_{oname}_{len(src)}",
             body,"gr.adv.set_comprehension",
             f"استيعاب مجموعة أنتج {oexpr}؛ طول بعد إزالة التكرار", expected,
             requires="اطبع_سطر، طول")
        idx+=1; scount+=1
        if scount>=30: break
    if scount>=30: break

# مجموعة — ترشيح ثمّ dedup (edge)
idx=100
se=0
for src in [[1,2,2,3,4,4],[5,3,1,3,5],[2,4,6,4,2],[1,1,1,2,2,3],[6,5,4,3,2,1]]:
    for cname,cfn,cexpr in CONDS:
        vals=dedup_order([s for s in src if cfn(s)])
        expected=str(len(vals))
        body=(f"متغير م = {{لكل س في {arr_lit(src)} إذا {cexpr} أنتج س}}\n"
              f"اطبع_سطر(طول(م))")
        emit("gr.adv.set_comprehension","edge",idx,f"filter_{cname}_{se}",
             body,"gr.adv.set_comprehension",
             f"استيعاب مجموعة ترشيح {cexpr} ثمّ إزالة تكرار؛ طول", expected,
             requires="اطبع_سطر، طول")
        idx+=1; se+=1
        if se>=15: break
    if se>=15: break

# مجموعة — خرائط غير حقنيّة (تطوي مدخلات متمايزة) — سدّ ثغرة تغطية زائفة (مراجعة Amelia #1):
#   الطول يميّز «إزالة التكرار على الناتج» عن «على المصدر» وعن «تجاهل أنتج»؛
#   وprobe قيمة بالفهرسة (ترتيب أوّل-ظهور) يثبت أنّ «أنتج» طُبِّق فعلًا (قيمة الناتج لا عنصر المصدر).
FOLD_MAPS = [("mod2", lambda s: s % 2, "س % 2"),
             ("mod3", lambda s: s % 3, "س % 3"),
             ("mod4", lambda s: s % 4, "س % 4")]
FOLD_SRC = [[1,2,3,4],[1,2,3,4,5],[2,4,6,8],[1,3,5,7],[1,2,3,4,5,6]]
idx=300
for src in FOLD_SRC:
    for oname,ofn,oexpr in FOLD_MAPS:
        vals=dedup_order([ofn(s) for s in src])
        # طول: المصدر متمايز أطول من الناتج المطويّ ⇒ يفشل محرّك يزيل على المصدر أو يتجاهل أنتج
        emit("gr.adv.set_comprehension","edge",idx,f"fold_{oname}_len{len(src)}",
             (f"متغير م = {{لكل س في {arr_lit(src)} أنتج {oexpr}}}\n"
              f"اطبع_سطر(طول(م))"),
             "gr.adv.set_comprehension",
             f"مجموعة بخريطة غير حقنيّة {oexpr}؛ الطول يميّز إزالة التكرار على الناتج", str(len(vals)),
             requires="اطبع_سطر، طول")
        idx+=1
        # probe قيمة عند الفهرس 0 وآخر فهرس (ترتيب أوّل-ظهور)
        for k in sorted(set([0, len(vals)-1])):
            emit("gr.adv.set_comprehension","edge",idx,f"foldval_{oname}_{len(src)}_{k}",
                 (f"متغير م = {{لكل س في {arr_lit(src)} أنتج {oexpr}}}\n"
                  f"اطبع_سطر(م[{k}])"),
                 "gr.adv.set_comprehension",
                 f"مجموعة {oexpr}؛ probe قيمة م[{k}] يثبت تطبيق أنتج (لا عنصر المصدر)", str(vals[k]),
                 requires="اطبع_سطر")
            idx+=1

# ─────────────────────────────────────────────────────────────
# استيعاب القاموس — مفاتيح نصّيّة فريدة (basic/edge)
# ─────────────────────────────────────────────────────────────
idx=100
dcount=0
for src in SOURCES:
    # نضمن مفاتيح فريدة: نستعمل مصدرًا بعناصر فريدة
    usrc=dedup_order(src)
    for oname,ofn,oexpr in OUT_EXPRS[:6]:
        # القيمة = ofn(س)، المفتاح = نص(س)؛ probe مفتاح آخر عنصر فريد
        probe=usrc[-1]
        expected=str(ofn(probe))
        body=(f"متغير د = {{لكل س في {arr_lit(src)} أنتج نص(س): {oexpr}}}\n"
              f'اطبع_سطر(د["{probe}"])')
        emit("gr.adv.dict_comprehension","basic",idx,f"kv_{oname}_{len(usrc)}",
             body,"gr.adv.dict_comprehension",
             f"استيعاب قاموس نص(س):{oexpr}؛ probe مفتاح «{probe}»", expected,
             requires="اطبع_سطر، نص")
        idx+=1; dcount+=1
        if dcount>=30: break
    if dcount>=30: break

# قاموس — ترشيح (edge): probe طول عبر عدّ المفاتيح الفريدة الباقية
idx=100
de=0
for src in [[1,2,3,4],[1,2,3,4,5],[2,3,4,5,6],[1,3,5,7],[6,5,4,3,2,1]]:
    usrc=dedup_order(src)
    for cname,cfn,cexpr in CONDS:
        keep=[s for s in usrc if cfn(s)]
        if not keep:
            continue
        probe=keep[-1]
        expected=str(probe*10)
        body=(f"متغير د = {{لكل س في {arr_lit(src)} إذا {cexpr} أنتج نص(س): س * 10}}\n"
              f'اطبع_سطر(د["{probe}"])')
        emit("gr.adv.dict_comprehension","edge",idx,f"filter_{cname}_{de}",
             body,"gr.adv.dict_comprehension",
             f"استيعاب قاموس بترشيح {cexpr}؛ probe مفتاح باقٍ «{probe}»", expected,
             requires="اطبع_سطر، نص")
        idx+=1; de+=1
        if de>=15: break
    if de>=15: break

# قاموس — مفتاح يطوي (نص(س%2)) بقيم مختلفة ⇒ آخر-يفوز؛ سدّ فجوة تصادُم المفاتيح (مراجعة Amelia #3)
idx=300
for src in [[1,2,3,4],[1,2,3,4,5,6],[2,3,4,5],[1,2,3,4,5]]:
    d={}
    for s in src:
        d[s % 2] = s  # آخر-يفوز على المفتاح المطويّ
    for kkey in sorted(d.keys()):
        emit("gr.adv.dict_comprehension","edge",idx,f"lastwins_k{kkey}_{len(src)}",
             (f"متغير د = {{لكل س في {arr_lit(src)} أنتج نص(س % 2): س}}\n"
              f'اطبع_سطر(د["{kkey}"])'),
             "gr.adv.dict_comprehension",
             f"قاموس بمفتاح مطويّ نص(س%2)؛ آخر-يفوز ⇒ د[«{kkey}»] = آخر س بهذا المفتاح", str(d[kkey]),
             requires="اطبع_سطر، نص")
        idx+=1

# عمق: مصدر متغيّر + تسلسل استيعابين + استيعاب يُرجَع من دالّة (مراجعة Amelia #5)
DEPTH = [
    ("gr.adv.set_comprehension","edge","depth_var_source_fold",
     "متغير ق = [1، 2، 3، 4]\nمتغير م = {لكل س في ق أنتج س % 3}\nاطبع_سطر(طول(م))",
     str(len(dedup_order([s % 3 for s in [1,2,3,4]]))), "اطبع_سطر، طول",
     "مجموعة بمصدر متغيّر ق وخريطة تطوي؛ طول الناتج"),
    ("gr.adv.list_comprehension","edge","depth_chained",
     "متغير أ = [لكل س في [1، 2، 3] أنتج س * 2]\nمتغير ب = [لكل ي في أ أنتج ي + 1]\nاطبع_سطر(ب[2])",
     "7", "اطبع_سطر", "تسلسل: قائمة ب مصدرها قائمة أ (استيعاب فوق استيعاب)؛ ب[2]"),
    ("gr.adv.list_comprehension","edge","depth_in_function",
     "دالة ف()\n    ارجع [لكل س في [1، 2، 3] أنتج س * س]\nنهاية\nمتغير ز = ف()\nاطبع_سطر(ز[2])",
     "9", "اطبع_سطر", "استيعاب قائمة يُرجَع من دالّة؛ ز[2]"),
    ("gr.adv.dict_comprehension","edge","depth_var_source",
     "متغير ق = [1، 2، 3]\nمتغير د = {لكل س في ق أنتج نص(س): س * 10}\nاطبع_سطر(د[\"3\"])",
     "30", "اطبع_سطر، نص", "قاموس بمصدر متغيّر ق؛ probe د[«3»]"),
]
idx=400
for rule,cat,name,body,expected,req,desc in DEPTH:
    emit(rule,cat,idx,name,body,rule,desc,expected,requires=req)
    idx+=1

# ─────────────────────────────────────────────────────────────
# سلبيّات — الترتيب القديم/غياب أنتج/في/متغيّر (negative)
# ─────────────────────────────────────────────────────────────
neg_cases = [
    ("gr.adv.list_comprehension","list_old_order","متغير ز = [س * 2 لكل س في [1، 2، 3]]\nاطبع_سطر(ز)","الترتيب القديم للقائمة يُرفض"),
    ("gr.adv.list_comprehension","list_no_yield","متغير ز = [لكل س في [1، 2، 3]]\nاطبع_سطر(ز)","قائمة بلا أنتج تُرفض"),
    ("gr.adv.list_comprehension","list_no_in","متغير ز = [لكل س [1، 2، 3] أنتج س]\nاطبع_سطر(ز)","قائمة بلا «في» تُرفض"),
    ("gr.adv.list_comprehension","list_no_var","متغير ز = [لكل في [1، 2، 3] أنتج س]\nاطبع_سطر(ز)","قائمة بلا متغيّر تُرفض"),
    ("gr.adv.list_comprehension","list_unclosed","متغير ز = [لكل س في [1، 2، 3] أنتج س\nاطبع_سطر(1)","قائمة غير مغلقة تُرفض"),
    ("gr.adv.set_comprehension","set_old_order","متغير م = {س * 2 لكل س في [1، 2، 3]}\nاطبع_سطر(م)","الترتيب القديم للمجموعة يُرفض"),
    ("gr.adv.set_comprehension","set_no_yield","متغير م = {لكل س في [1، 2، 3] س}\nاطبع_سطر(م)","مجموعة بلا أنتج تُرفض"),
    ("gr.adv.set_comprehension","set_no_in","متغير م = {لكل س [1، 2، 3] أنتج س}\nاطبع_سطر(م)","مجموعة بلا «في» تُرفض"),
    ("gr.adv.dict_comprehension","dict_old_order","متغير د = {نص(س): س لكل س في [1، 2، 3]}\nاطبع_سطر(د)","الترتيب القديم للقاموس يُرفض"),
    ("gr.adv.dict_comprehension","dict_no_yield","متغير د = {لكل س في [1، 2، 3] نص(س): س}\nاطبع_سطر(د)","قاموس بلا أنتج يُرفض"),
    ("gr.adv.dict_comprehension","dict_no_colon","متغير د = {لكل س في [1، 2، 3] أنتج نص(س) س}\nاطبع_سطر(د)","قاموس بلا «:» بعد المفتاح يُرفض"),
    ("gr.adv.dict_comprehension","dict_no_in","متغير د = {لكل س [1، 2، 3] أنتج نص(س): س}\nاطبع_سطر(د)","قاموس بلا «في» يُرفض"),
]
for i,(rule,name,body,desc) in enumerate(neg_cases, start=100):
    cat_dir = rule
    req = "اطبع_سطر، نص" if "نص(" in body else "اطبع_سطر"
    emit(cat_dir,"negative",i,name,body,rule,desc,"",requires=req,priority="P1",expect_error=True)
    count["neg"]+=1

# ملاحظة: تتعايش ملفّات مكتوبة يدويًّا (أرقام 00x) مع المولَّدة (idx≥100) في نفس الأدلّة —
# المولّد لا يُعيد إنتاجها ولا يحذفها؛ إعادة التوليد تُحدِّث المولَّدة فقط وتُبقي اليدويّة سليمة.
def _c(rule):
    return sum(1 for p in files if rule in p)
print(f"list={_c('list_comprehension')} set={_c('set_comprehension')} "
      f"dict={_c('dict_comprehension')} total_generated={len(files)}")
