#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
=============================================================================
تحليل تغطية ميزات لغة ص — Feature Coverage Analyzer
=============================================================================
يقيس نسبة ميزات لغة ص المغطاة بالاختبارات الموجودة.

المنهج:
  1. يُعرّف 70+ ميزة أساسية للغة مع أنماط regex تكشفها.
  2. يقرأ جميع ملفات الاختبار (.ص) في tests/dual_execution/tests/.
  3. يفحص كل ميزة — هل تظهر في أي اختبار؟
  4. يحسب نسبة التغطية الإجمالية وبالفئة.
  5. يولّد تقرير نصي في build/coverage_report.txt.
  6. يولّد تقرير HTML في build/coverage_html/index.html.

الاستخدام:
  python scripts/feature_coverage.py
  python scripts/feature_coverage.py --json build/coverage_data.json
=============================================================================
"""

import sys
import os
import re
import json
import argparse
import datetime
from pathlib import Path
from collections import defaultdict

# =============================================================================
# المسارات الأساسية
# =============================================================================
ROOT = Path(__file__).parent.parent
# مجلد الاختبارات (يحتوي على مجلدات فرعية: core, advanced, oop, concurrency, negative)
TEST_DIR = ROOT / "tests" / "dual_execution"
OUTPUT_DIR = ROOT / "build"
OUTPUT_TEXT = OUTPUT_DIR / "coverage_report.txt"
OUTPUT_HTML_DIR = OUTPUT_DIR / "coverage_html"
OUTPUT_JSON = OUTPUT_DIR / "coverage_data.json"


# =============================================================================
# تعريف جدول الميزات (Feature ID, الاسم, الفئة, أنماط regex)
# =============================================================================
# كل ميزة: (id, اسم_الميزة, الفئة, [أنماط])
# نمط واحد كافٍ للعثور على الميزة — OR logic
FEATURES = [
    # ─────────────────── أساسيات ───────────────────
    ("F01", "طباعة (اطبع / اطبع_سطر)",         "أساسيات",          [r"اطبع\s*\(", r"اطبع_سطر\s*\("]),
    ("F02", "متغير",                              "أساسيات",          [r"\bمتغير\b"]),
    ("F03", "ثابت",                               "أساسيات",          [r"\bثابت\b"]),
    ("F04", "نص string literal",                  "أساسيات",          [r'"[^"]*"']),
    ("F05", "أرقام صحيحة",                       "أساسيات",          [r'\b\d+\b']),
    ("F06", "أرقام عشرية",                       "أساسيات",          [r'\b\d+\.\d+\b']),
    ("F07", "قيم منطقية صحيح/خطأ",              "أساسيات",          [r'\bصحيح\b', r'\bخطأ\b']),
    ("F08", "لاشيء null",                        "أساسيات",          [r'\bلاشيء\b']),
    ("F09", "تعليقات #",                         "أساسيات",          [r'^\s*#']),
    ("F10", "وصول خاصية .property",             "أساسيات",          [r'\w+\.\w+']),

    # ─────────────────── عمليات ────────────────────
    ("F11", "جمع وطرح + -",                     "عمليات حسابية",    [r'[\+\-]']),
    ("F12", "ضرب وقسمة وباقي * / %",            "عمليات حسابية",    [r'[\*\/\%]']),
    ("F13", "أس **",                             "عمليات حسابية",    [r'\*\*']),
    ("F14", "مقارنة == != < > <= >=",           "عمليات حسابية",    [r'==|!=|<=|>=']),
    ("F15", "منطقي و / أو / ليس",               "عمليات حسابية",    [r'\bو\b', r'\bأو\b', r'\bليس\b', r'&&', r'\|\|']),
    ("F16", "عامل ثلاثي ? :",                   "عمليات حسابية",    [r'\?(?!\?)']),
    ("F17", "عامل نيل ثنائي ??",                "عمليات حسابية",    [r'\?\?']),
    ("F18", "إسناد مركّب += -= *=",              "عمليات حسابية",    [r'\+=|\-=|\*=']),
    ("F19", "عمليات بتية & | ^ ~ << >>",        "عمليات حسابية",    [r'&(?!&)|\|(?!\|)|\^|~|<<|>>']),

    # ─────────────────── تحكم ──────────────────────
    ("F20", "شرط إذا / وإلا",                   "تحكم في التدفق",   [r'\bإذا\b', r'\bاذا\b']),
    ("F21", "حلقة بينما",                        "تحكم في التدفق",   [r'\bبينما\b']),
    ("F22", "حلقة لكل ... في",                   "تحكم في التدفق",   [r'\bلكل\b']),
    ("F23", "توقف / استمر",                      "تحكم في التدفق",   [r'\bتوقف\b', r'\bاستمر\b']),

    # ─────────────────── دوال ──────────────────────
    ("F24", "تعريف دالة",                         "دوال",             [r'\bدالة\b']),
    ("F25", "إرجاع",                             "دوال",             [r'\bارجع\b']),
    ("F26", "لامدا / دوال مجهولة",               "دوال",             [r'\bلامدا\b']),
    ("F27", "دوال غير متزامنة",                  "دوال",             [r'\bغير_متزامن\b']),
    ("F28", "انتظر await",                       "دوال",             [r'\bانتظر\b']),
    ("F29", "مولّد / أنتج",                      "دوال",             [r'\bمولد\b', r'\bأنتج\b']),

    # ─────────────────── OOP ────────────────────────
    ("F30", "تعريف صنف",                          "OOP",              [r'\bصنف\b']),
    ("F31", "وراثة يرث",                          "OOP",              [r'\bيرث\b']),
    ("F32", "باني constructor",                   "OOP",              [r'\bباني\b']),
    ("F33", "هذا / الأساس",                      "OOP",              [r'\bهذا\b', r'\bالأساس\b']),
    ("F34", "تعدد أشكال / مجرد",                 "OOP",              [r'\bمجرد\b']),
    ("F35", "تغليف عام / خاص / محمي",            "OOP",              [r'\bعام\b', r'\bخاص\b', r'\bمحمي\b']),
    ("F36", "بنية struct",                        "OOP",              [r'\bبنية\b']),
    ("F37", "تعداد enum",                         "OOP",              [r'\bتعداد\b']),
    ("F38", "سمة trait / نفّذ impl",             "OOP",              [r'\bسمة\b', r'\bنفّذ\b', r'\bنفذ\b']),
    ("F39", "امتداد extension methods",           "OOP",              [r'\bامتداد\b']),
    ("F40", "عامل زائد overload",                "OOP",              [r'\bعامل\b']),
    ("F41", "خاصية / احصل / عيّن",              "OOP",              [r'\bخاصية\b']),
    ("F42", "هدم destructor",                    "OOP",              [r'\bهدم\b']),
    ("F43", "جديد new",                           "OOP",              [r'\bجديد\b']),

    # ─────────────────── هياكل البيانات ────────────
    ("F44", "مصفوفة []",                          "هياكل البيانات",   [r'\[.*\]']),
    ("F45", "خريطة {}",                           "هياكل البيانات",   [r'\{[^}]+:[^}]+\}']),
    ("F46", "دوال مصفوفة (اضف، رتب...)",         "هياكل البيانات",   [r'\.(اضف|احذف_اخير|رتب|عكس|شريحة|رشح|اختزل|مسطح)\s*\(']),
    ("F47", "دوال نص (تقسيم، استبدل...)",        "هياكل البيانات",   [r'\.(تقسيم|استبدل|استخراج|قص_أطراف|يبدأ_بـ|ينتهي_بـ|تحويل_كبير|تحويل_صغير)\s*\(']),
    ("F48", "دوال خريطة (مفاتيح، قيم...)",       "هياكل البيانات",   [r'\.(مفاتيح|قيم|يحتوي|احذف)\s*\(']),

    # ─────────────────── معالجة أخطاء ─────────────
    ("F49", "حاول / امسك / أخيراً",             "معالجة أخطاء",     [r'\bحاول\b']),
    ("F50", "ارمي استثناء",                      "معالجة أخطاء",     [r'\bارمي\b']),

    # ─────────────────── مطابقة أنماط ─────────────
    ("F51", "طابق / عندما",                       "مطابقة أنماط",     [r'\bطابق\b']),
    ("F52", "افتراضي default",                    "مطابقة أنماط",     [r'\bافتراضي\b']),
    ("F53", "نطاق n..m",                          "مطابقة أنماط",     [r'\d+\.\.\d+']),
    ("F54", "نمط قائمة [أ, ب]",                  "مطابقة أنماط",     [r'عندما\s*\[']),
    ("F55", "نمط OR عندما 1||2",                 "مطابقة أنماط",     [r'عندما.*\|\|']),

    # ─────────────────── وحدات ─────────────────────
    ("F56", "استيراد وحدة",                       "وحدات",            [r'\bاستورد\b']),
    ("F57", "من...استورد",                        "وحدات",            [r'\bمن\b.*\bاستورد\b']),

    # ─────────────────── تحويل أنواع ───────────────
    ("F58", "رقم() / نص() / عشري()",            "تحويل أنواع",      [r'\bرقم\s*\(', r'\bنص\s*\(', r'\bعشري\s*\(']),
    ("F59", "نوع() / طول()",                    "تحويل أنواع",      [r'\bنوع\s*\(', r'\bطول\s*\(']),

    # ─────────────────── تزامن ─────────────────────
    ("F60", "قناة channel",                       "تزامن",            [r'\bقناة\s*\(']),
    ("F61", "إطلاق goroutine أطلق",              "تزامن",            [r'\bأطلق\b', r'\bاطلق\b']),
    ("F62", "انتظر_الكل",                         "تزامن",            [r'\bانتظر_الكل\s*\(']),
    ("F63", "اختر select",                        "تزامن",            [r'\bاختر\b']),
    ("F64", "قفل mutex",                          "تزامن",            [r'\bقفل\s*\(']),
    ("F65", "مجموعة_انتظار WaitGroup",            "تزامن",            [r'\bمجموعة_انتظار\s*\(']),
    ("F66", "مستقبل future",                      "تزامن",            [r'\bمستقبل\s*\(']),

    # ─────────────────── ميزات متقدمة ──────────────
    ("F67", "ماكرو macro",                        "متقدم",            [r'\bماكرو\b']),
    ("F68", "تأجيل defer (أجّل)",                "متقدم",            [r'\bأجّل\b']),
    ("F69", "توجيهات @ (حجم، ذري...)",          "متقدم",            [r'@حجم|@ذري|@غير_آمن|@متطاير|@تجميع']),
    ("F70", "عقود يتطلب / يضمن",                "متقدم",            [r'\bيتطلب\b', r'\bيضمن\b']),
    ("F71", "فضاء أسماء",                         "متقدم",            [r'\bفضاء\b']),
    ("F72", "رئيسية main entry",                 "متقدم",            [r'\bرئيسية\b']),

    # ─────────────────── دوال مدمجة إضافية ─────────
    ("F73", "إدخال (اقرأ)",                       "دوال مدمجة",       [r'\bاقرأ\s*\(']),
    ("F74", "دوال رياضيات (جذر، لوغ، أس...)",   "دوال مدمجة",       [r'\bجذر\s*\(', r'\bلوغ\s*\(', r'\bأس\s*\(']),
    ("F75", "وصول فهرس arr[i]",                  "دوال مدمجة",       [r'\w+\[\s*\w+\s*\]']),
]

# إجمالي الميزات المعرّفة
TOTAL_FEATURES = len(FEATURES)


# =============================================================================
# قراءة ملفات الاختبار
# =============================================================================
def load_test_files(test_dir: Path) -> dict:
    """
    يقرأ جميع ملفات .ص في مجلد الاختبارات ويُعيد:
    { "اسم_الملف.ص": "محتوى الملف" }
    """
    files = {}
    if not test_dir.exists():
        print(f"[تحذير] مجلد الاختبارات غير موجود: {test_dir}")
        return files

    # البحث recursive في جميع المجلدات الفرعية
    for f in sorted(test_dir.rglob("*.ص")):
        try:
            content = f.read_text(encoding="utf-8", errors="replace")
            files[f.name] = content
        except Exception as e:
            print(f"[تحذير] تعذّر قراءة {f.name}: {e}")

    return files


# =============================================================================
# تحليل تغطية الميزات
# =============================================================================
def analyze_coverage(test_files: dict) -> dict:
    """
    يفحص كل ميزة ضد جميع ملفات الاختبار.
    يُعيد قاموس نتائج شامل.
    """
    # دمج كل محتوى الاختبارات في نص واحد كبير (لكشف الأنماط العامة)
    combined = "\n".join(test_files.values())

    results = []
    covered_count = 0
    category_stats = defaultdict(lambda: {"total": 0, "covered": 0})

    for fid, name, category, patterns in FEATURES:
        # فحص الأنماط
        covered = False
        matched_in = []

        for filename, content in test_files.items():
            for pattern in patterns:
                try:
                    if re.search(pattern, content, re.MULTILINE):
                        covered = True
                        if filename not in matched_in:
                            matched_in.append(filename)
                        break
                except re.error:
                    pass
            if covered:
                break

        if covered:
            covered_count += 1

        category_stats[category]["total"] += 1
        if covered:
            category_stats[category]["covered"] += 1

        results.append({
            "id": fid,
            "name": name,
            "category": category,
            "covered": covered,
            "test_files": matched_in[:3],  # أول 3 ملفات تغطي هذه الميزة
        })

    overall_pct = (covered_count / TOTAL_FEATURES * 100) if TOTAL_FEATURES > 0 else 0.0

    return {
        "timestamp": datetime.datetime.now().isoformat(timespec="seconds"),
        "total_features": TOTAL_FEATURES,
        "covered_features": covered_count,
        "uncovered_features": TOTAL_FEATURES - covered_count,
        "coverage_pct": round(overall_pct, 1),
        "total_test_files": len(test_files),
        "features": results,
        "category_stats": {
            cat: {
                "total": v["total"],
                "covered": v["covered"],
                "pct": round(v["covered"] / v["total"] * 100, 1) if v["total"] > 0 else 0.0,
            }
            for cat, v in sorted(category_stats.items())
        },
    }


# =============================================================================
# إخراج التقرير النصي
# =============================================================================
def generate_text_report(data: dict, path: Path) -> str:
    """يولّد تقريراً نصياً مفصلاً."""
    lines = []
    sep = "=" * 70

    lines.append(sep)
    lines.append("  تقرير تغطية ميزات لغة ص — Feature Coverage Report")
    lines.append(f"  التاريخ: {data['timestamp']}")
    lines.append(sep)
    lines.append("")

    pct = data["coverage_pct"]
    bar_filled = int(pct / 2)
    bar = "█" * bar_filled + "░" * (50 - bar_filled)
    status = "✅ تجاوز الهدف" if pct >= 50 else "⚠️  دون الهدف (50%)"

    lines.append(f"  التغطية الإجمالية: {pct}%  [{bar}]")
    lines.append(f"  الحالة: {status}")
    lines.append(f"  الميزات المغطاة: {data['covered_features']} / {data['total_features']}")
    lines.append(f"  ملفات الاختبار: {data['total_test_files']}")
    lines.append("")

    # إحصاءات بالفئة
    lines.append("─" * 70)
    lines.append("  التغطية حسب الفئة:")
    lines.append("─" * 70)
    for cat, stats in data["category_stats"].items():
        bar_w = int(stats["pct"] / 2)
        cat_bar = "█" * bar_w + "░" * (50 - bar_w)
        lines.append(
            f"  {cat:<22} {stats['pct']:5.1f}%  "
            f"({stats['covered']}/{stats['total']})  [{cat_bar[:25]}]"
        )
    lines.append("")

    # الميزات غير المغطاة
    uncovered = [f for f in data["features"] if not f["covered"]]
    if uncovered:
        lines.append("─" * 70)
        lines.append(f"  الميزات غير المغطاة ({len(uncovered)} ميزة):")
        lines.append("─" * 70)
        for f in uncovered:
            lines.append(f"  ✗  [{f['id']}]  {f['category']:<22}  {f['name']}")
    lines.append("")

    # الميزات المغطاة
    covered = [f for f in data["features"] if f["covered"]]
    lines.append("─" * 70)
    lines.append(f"  الميزات المغطاة ({len(covered)} ميزة):")
    lines.append("─" * 70)
    for f in covered:
        sample = f["test_files"][0] if f["test_files"] else "—"
        lines.append(f"  ✓  [{f['id']}]  {f['category']:<22}  {f['name']}  ({sample})")
    lines.append("")
    lines.append(sep)

    text = "\n".join(lines)
    path.write_text(text, encoding="utf-8")
    return text


# =============================================================================
# إخراج التقرير HTML
# =============================================================================
def generate_html_report(data: dict, output_dir: Path) -> None:
    """يولّد تقرير HTML تفاعلي."""
    output_dir.mkdir(parents=True, exist_ok=True)

    pct = data["coverage_pct"]
    status_color = "#28a745" if pct >= 50 else "#dc3545"
    bar_color    = "#28a745" if pct >= 50 else "#ffc107"

    # بناء أسطر الجدول
    rows_html = []
    for f in data["features"]:
        icon    = "✅" if f["covered"] else "❌"
        row_cls = "covered" if f["covered"] else "uncovered"
        sample  = f["test_files"][0] if f["test_files"] else "—"
        rows_html.append(
            f'<tr class="{row_cls}">'
            f'<td>{f["id"]}</td>'
            f'<td>{icon}</td>'
            f'<td>{f["category"]}</td>'
            f'<td>{f["name"]}</td>'
            f'<td>{sample}</td>'
            f'</tr>'
        )

    # بناء صفوف الفئات
    cat_rows = []
    for cat, s in data["category_stats"].items():
        c_color = "#28a745" if s["pct"] >= 50 else ("#ffc107" if s["pct"] >= 25 else "#dc3545")
        cat_rows.append(
            f'<tr>'
            f'<td>{cat}</td>'
            f'<td style="color:{c_color};font-weight:bold">{s["pct"]}%</td>'
            f'<td>{s["covered"]} / {s["total"]}</td>'
            f'</tr>'
        )

    html = f"""<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>تغطية ميزات لغة ص</title>
  <style>
    body {{
      font-family: 'Segoe UI', Tahoma, Arial, sans-serif;
      background: #0d1117; color: #e6edf3;
      margin: 0; padding: 20px;
    }}
    h1 {{ color: #58a6ff; text-align: center; font-size: 1.8em; }}
    h2 {{ color: #79c0ff; border-bottom: 1px solid #30363d; padding-bottom: 8px; }}
    .summary-box {{
      background: #161b22; border: 1px solid #30363d; border-radius: 8px;
      padding: 20px; margin: 20px auto; max-width: 700px; text-align: center;
    }}
    .big-pct {{
      font-size: 4em; font-weight: bold;
      color: {status_color};
    }}
    .progress-bar-outer {{
      background: #21262d; border-radius: 10px;
      height: 20px; width: 100%; margin: 12px 0;
      overflow: hidden;
    }}
    .progress-bar-inner {{
      background: {bar_color};
      height: 100%;
      width: {pct}%;
      border-radius: 10px;
      transition: width 1s ease;
    }}
    .stats {{ display: flex; gap: 20px; justify-content: center; flex-wrap: wrap; }}
    .stat-card {{
      background: #21262d; border: 1px solid #30363d; border-radius: 8px;
      padding: 14px 24px; text-align: center;
    }}
    .stat-card .num {{ font-size: 2em; font-weight: bold; color: #58a6ff; }}
    .stat-card .lbl {{ font-size: 0.85em; color: #8b949e; }}
    table {{
      width: 100%; border-collapse: collapse; margin: 16px 0;
      font-size: 0.9em;
    }}
    th {{ background: #21262d; color: #79c0ff; padding: 10px; text-align: right; }}
    td {{ padding: 8px 10px; border-bottom: 1px solid #21262d; }}
    tr.covered {{ background: rgba(40, 167, 69, 0.05); }}
    tr.uncovered {{ background: rgba(220, 53, 69, 0.05); }}
    tr:hover {{ background: #1f6feb22; }}
    .badge-ok  {{ color: #3fb950; }}
    .badge-err {{ color: #f85149; }}
    #filterInput {{
      background: #21262d; color: #e6edf3;
      border: 1px solid #30363d; border-radius: 6px;
      padding: 8px 14px; width: 300px; font-size: 1em;
      margin-bottom: 10px;
    }}
    footer {{ text-align: center; color: #484f58; margin-top: 40px; font-size: 0.8em; }}
  </style>
</head>
<body>

<h1>🔍 تقرير تغطية ميزات لغة ص</h1>
<p style="text-align:center;color:#8b949e">{data["timestamp"]}</p>

<!-- ملخص رئيسي -->
<div class="summary-box">
  <div class="big-pct">{pct}%</div>
  <div class="progress-bar-outer"><div class="progress-bar-inner"></div></div>
  <p style="color:{status_color};font-weight:bold;font-size:1.2em">
    {"✅ تجاوزت الهدف — جاهز للمرحلة 3" if pct >= 50 else "⚠️  دون الهدف 50%"}
  </p>
  <div class="stats">
    <div class="stat-card">
      <div class="num" style="color:#3fb950">{data["covered_features"]}</div>
      <div class="lbl">ميزة مغطاة</div>
    </div>
    <div class="stat-card">
      <div class="num" style="color:#f85149">{data["uncovered_features"]}</div>
      <div class="lbl">ميزة غير مغطاة</div>
    </div>
    <div class="stat-card">
      <div class="num">{data["total_features"]}</div>
      <div class="lbl">إجمالي الميزات</div>
    </div>
    <div class="stat-card">
      <div class="num" style="color:#e3b341">{data["total_test_files"]}</div>
      <div class="lbl">ملف اختبار</div>
    </div>
  </div>
</div>

<!-- إحصاء الفئات -->
<h2>التغطية حسب الفئة</h2>
<table>
  <thead><tr><th>الفئة</th><th>النسبة</th><th>مغطاة / إجمالي</th></tr></thead>
  <tbody>
    {"".join(cat_rows)}
  </tbody>
</table>

<!-- جدول الميزات الكامل -->
<h2>قائمة الميزات التفصيلية</h2>
<input id="filterInput" type="text" placeholder="ابحث عن ميزة..." oninput="filterTable()">
<table id="featTable">
  <thead>
    <tr>
      <th>المعرّف</th>
      <th>الحالة</th>
      <th>الفئة</th>
      <th>الميزة</th>
      <th>مثال اختبار</th>
    </tr>
  </thead>
  <tbody>
    {"".join(rows_html)}
  </tbody>
</table>

<script>
function filterTable() {{
  var q = document.getElementById("filterInput").value.toLowerCase();
  var rows = document.querySelectorAll("#featTable tbody tr");
  rows.forEach(function(r) {{
    r.style.display = r.textContent.toLowerCase().includes(q) ? "" : "none";
  }});
}}
</script>

<footer>
  لغة ص — Feature Coverage Analysis | تم التوليد بواسطة scripts/feature_coverage.py
</footer>
</body>
</html>"""

    (output_dir / "index.html").write_text(html, encoding="utf-8")


# =============================================================================
# النقطة الرئيسية
# =============================================================================
def main():
    parser = argparse.ArgumentParser(description="تحليل تغطية ميزات لغة ص")
    parser.add_argument("--test-dir",  default=str(TEST_DIR),    help="مجلد ملفات الاختبار .ص")
    parser.add_argument("--output-dir",default=str(OUTPUT_DIR),  help="مجلد الإخراج")
    parser.add_argument("--json",      default=str(OUTPUT_JSON), help="مسار ملف JSON للنتائج")
    parser.add_argument("--no-html",   action="store_true",       help="تخطي إنشاء HTML")
    parser.add_argument("--no-text",   action="store_true",       help="تخطي إنشاء التقرير النصي")
    args = parser.parse_args()

    test_dir   = Path(args.test_dir)
    output_dir = Path(args.output_dir)
    html_dir   = output_dir / "coverage_html"
    text_path  = output_dir / "coverage_report.txt"
    json_path  = Path(args.json)

    output_dir.mkdir(parents=True, exist_ok=True)

    print("📂 قراءة ملفات الاختبار...")
    files = load_test_files(test_dir)
    print(f"   تم تحميل {len(files)} ملف اختبار")

    print("🔍 تحليل التغطية...")
    data = analyze_coverage(files)

    # حفظ JSON
    json_path.write_text(json.dumps(data, ensure_ascii=False, indent=2), encoding="utf-8")
    print(f"   ✓ JSON: {json_path}")

    # التقرير النصي
    if not args.no_text:
        text = generate_text_report(data, text_path)
        print(f"   ✓ التقرير النصي: {text_path}")
        print()
        # طباعة الملخص
        for line in text.split("\n")[:30]:
            print(line)

    # التقرير HTML
    if not args.no_html:
        generate_html_report(data, html_dir)
        print(f"   ✓ HTML: {html_dir / 'index.html'}")

    # النتيجة النهائية
    pct = data["coverage_pct"]
    print()
    print("=" * 60)
    print(f"  التغطية الإجمالية: {pct}%")
    print(f"  المغطاة: {data['covered_features']} / {data['total_features']}")
    if pct >= 50:
        print("  ✅ تجاوزت الهدف (50%) — الخطة جاهزة للمرحلة 3")
    else:
        print(f"  ⚠️  {50 - pct:.1f}% متبقية للوصول للهدف")
    print("=" * 60)

    # exit code: 0 = نجح، 1 = دون الهدف
    sys.exit(0 if pct >= 50 else 1)


if __name__ == "__main__":
    main()
