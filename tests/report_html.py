#!/usr/bin/env python3
# ═══════════════════════════════════════════════════════════════════════════════════
# (AR) مولّد تقرير HTML لاختبارات التنفيذ المزدوج
# (EN) HTML Report Generator for Dual Execution Tests
#
# يقرأ بيانات الاختبارات (list[dict]) وينشئ صفحة HTML بصرية تشمل:
# - ملخص عام بالأرقام والنسب
# - رسوم بيانية (شريطي + دائري) عبر Chart.js CDN
# - جدول تفصيلي مع فلترة وبحث
# - مقارنة أوقات المفسر مقابل المترجم
# - تصميم متجاوب يدعم RTL (العربية)
# ═══════════════════════════════════════════════════════════════════════════════════

import json
import time
from pathlib import Path
from typing import Any

# ═══════════════════════════════════════════════════════════════════════════════════
# (AR) قالب HTML الكامل — مضمن لتجنب اعتمادات خارجية
# ═══════════════════════════════════════════════════════════════════════════════════

_HTML_TEMPLATE = r"""<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>تقرير اختبارات التنفيذ المزدوج — لغة ص</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.7/dist/chart.umd.min.js"></script>
    <style>
        :root {
            --bg: #0d1117;
            --card: #161b22;
            --border: #30363d;
            --text: #e6edf3;
            --text2: #8b949e;
            --green: #3fb950;
            --red: #f85149;
            --yellow: #d29922;
            --blue: #58a6ff;
            --purple: #bc8cff;
        }
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            background: var(--bg);
            color: var(--text);
            font-family: 'Segoe UI', Tahoma, Arial, sans-serif;
            padding: 24px;
            direction: rtl;
        }
        .header {
            text-align: center;
            margin-bottom: 32px;
            padding: 24px;
            background: var(--card);
            border-radius: 12px;
            border: 1px solid var(--border);
        }
        .header h1 { font-size: 1.8em; margin-bottom: 8px; }
        .header .subtitle { color: var(--text2); font-size: 0.95em; }
        .header .badge {
            display: inline-block;
            padding: 4px 12px;
            border-radius: 16px;
            font-size: 0.85em;
            font-weight: bold;
            margin-top: 8px;
        }
        .badge-pass { background: rgba(63,185,80,0.2); color: var(--green); }
        .badge-fail { background: rgba(248,81,73,0.2); color: var(--red); }

        /* ─── الملخص ─── */
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(160px, 1fr));
            gap: 16px;
            margin-bottom: 32px;
        }
        .stat-card {
            background: var(--card);
            border: 1px solid var(--border);
            border-radius: 12px;
            padding: 20px;
            text-align: center;
        }
        .stat-card .num { font-size: 2.2em; font-weight: bold; }
        .stat-card .label { color: var(--text2); font-size: 0.9em; margin-top: 4px; }
        .stat-card.pass .num { color: var(--green); }
        .stat-card.fail .num { color: var(--red); }
        .stat-card.skip .num { color: var(--yellow); }
        .stat-card.total .num { color: var(--blue); }
        .stat-card.time .num { color: var(--purple); }

        /* ─── الرسوم البيانية ─── */
        .charts-row {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 24px;
            margin-bottom: 32px;
        }
        @media (max-width: 768px) { .charts-row { grid-template-columns: 1fr; } }
        .chart-card {
            background: var(--card);
            border: 1px solid var(--border);
            border-radius: 12px;
            padding: 20px;
        }
        .chart-card h3 { margin-bottom: 12px; font-size: 1.1em; }
        .chart-container { position: relative; height: 280px; }

        /* ─── الجدول ─── */
        .table-section {
            background: var(--card);
            border: 1px solid var(--border);
            border-radius: 12px;
            padding: 20px;
            overflow-x: auto;
        }
        .table-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 16px;
            flex-wrap: wrap;
            gap: 12px;
        }
        .table-header h3 { font-size: 1.1em; }
        .filter-bar {
            display: flex;
            gap: 8px;
            align-items: center;
        }
        .filter-bar input {
            background: var(--bg);
            border: 1px solid var(--border);
            border-radius: 6px;
            padding: 6px 12px;
            color: var(--text);
            font-size: 0.9em;
            width: 200px;
        }
        .filter-bar select {
            background: var(--bg);
            border: 1px solid var(--border);
            border-radius: 6px;
            padding: 6px 8px;
            color: var(--text);
            font-size: 0.9em;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            font-size: 0.9em;
        }
        th {
            text-align: right;
            padding: 10px 12px;
            border-bottom: 2px solid var(--border);
            color: var(--text2);
            font-weight: 600;
            cursor: pointer;
            user-select: none;
        }
        th:hover { color: var(--text); }
        td {
            padding: 8px 12px;
            border-bottom: 1px solid var(--border);
        }
        tr:hover { background: rgba(88,166,255,0.05); }
        .status-badge {
            display: inline-block;
            padding: 2px 8px;
            border-radius: 10px;
            font-size: 0.8em;
            font-weight: bold;
        }
        .status-PASS { background: rgba(63,185,80,0.15); color: var(--green); }
        .status-FAIL_OUTPUT, .status-FAIL_COMPILE, .status-FAIL_INTERP, .status-FAIL_TIMEOUT {
            background: rgba(248,81,73,0.15); color: var(--red);
        }
        .status-SKIP { background: rgba(210,153,34,0.15); color: var(--yellow); }
        .time-bar {
            display: inline-block;
            height: 6px;
            border-radius: 3px;
            min-width: 4px;
        }
        .time-interp { background: var(--blue); }
        .time-compiler { background: var(--purple); }
        .error-text { color: var(--red); font-size: 0.8em; max-width: 300px; }

        /* ─── التذييل ─── */
        .footer {
            text-align: center;
            margin-top: 32px;
            color: var(--text2);
            font-size: 0.85em;
        }
    </style>
</head>
<body>

<div class="header">
    <h1>📊 تقرير اختبارات التنفيذ المزدوج</h1>
    <div class="subtitle">لغة ص — المفسر + المترجم | {{TIMESTAMP}}</div>
    <div class="badge {{BADGE_CLASS}}">{{BADGE_TEXT}}</div>
</div>

<!-- ═══ الملخص ═══ -->
<div class="stats-grid">
    <div class="stat-card total">
        <div class="num">{{TOTAL}}</div>
        <div class="label">إجمالي الاختبارات</div>
    </div>
    <div class="stat-card pass">
        <div class="num">{{PASSED}}</div>
        <div class="label">نجح ✅</div>
    </div>
    <div class="stat-card fail">
        <div class="num">{{FAILED}}</div>
        <div class="label">فشل ❌</div>
    </div>
    <div class="stat-card skip">
        <div class="num">{{SKIPPED}}</div>
        <div class="label">تخطي ⏭️</div>
    </div>
    <div class="stat-card time">
        <div class="num">{{ELAPSED}}s</div>
        <div class="label">الوقت الكلي</div>
    </div>
    <div class="stat-card" style="border-color: var(--blue);">
        <div class="num" style="color: var(--blue);">{{RATE}}%</div>
        <div class="label">نسبة النجاح</div>
    </div>
</div>

<!-- ═══ تفصيل أنماط النجاح ═══ -->
<div class="stats-grid">
    <div class="stat-card" style="border-color: var(--green);">
        <div class="num" style="color: var(--green);">{{DUAL_PARITY_PASSED}}</div>
        <div class="label">تكافؤ مزدوج ✅ (مفسر+مترجم)</div>
    </div>
    <div class="stat-card" style="border-color: var(--blue);">
        <div class="num" style="color: var(--blue);">{{INTERP_ONLY_PASSED}}</div>
        <div class="label">مفسر فقط ✅ (@expected)</div>
    </div>
    <!-- (AR) الدلوُ الثالث: سالبُ المترجّمِ المنفرد — المفسّرُ لا يُشغَّلُ عليه أصلًا،
             فعرضُه في دلوِ «تكافؤٍ مزدوج» شهادةُ اتّفاقٍ لم يُقَسْ أحدُ طرفَيه. -->
    <!-- (EN) Third bucket: compiler-only negatives — the interpreter never runs on
             them, so showing them as "dual parity" asserts unmeasured agreement. -->
    <div class="stat-card" style="border-color: var(--yellow);">
        <div class="num" style="color: var(--yellow);">{{COMPILER_ONLY_PASSED}}</div>
        <div class="label">مترجم فقط ✅ (سالبٌ مترجَم)</div>
    </div>
</div>

<!-- ═══ الرسوم البيانية ═══ -->
<div class="charts-row">
    <div class="chart-card">
        <h3>📈 توزيع النتائج</h3>
        <div class="chart-container">
            <canvas id="statusChart"></canvas>
        </div>
    </div>
    <div class="chart-card">
        <h3>⏱️ مقارنة الأوقات (ms)</h3>
        <div class="chart-container">
            <canvas id="timeChart"></canvas>
        </div>
    </div>
</div>

<!-- ═══ الجدول ═══ -->
<div class="table-section">
    <div class="table-header">
        <h3>📋 تفاصيل الاختبارات ({{TOTAL}})</h3>
        <div class="filter-bar">
            <input type="text" id="searchBox" placeholder="🔍 بحث..." oninput="filterTable()">
            <select id="statusFilter" onchange="filterTable()">
                <option value="">الكل</option>
                <option value="PASS">نجح</option>
                <option value="FAIL">فشل</option>
                <option value="SKIP">تخطي</option>
            </select>
        </div>
    </div>
    <table id="resultsTable">
        <thead>
            <tr>
                <th onclick="sortTable(0)">#</th>
                <th onclick="sortTable(1)">الملف</th>
                <th onclick="sortTable(2)">الحالة</th>
                <th onclick="sortTable(3)">النمط</th>
                <th onclick="sortTable(4)">المفسر (ms)</th>
                <th onclick="sortTable(5)">المترجم (ms)</th>
                <th>ملاحظات</th>
            </tr>
        </thead>
        <tbody>
            {{TABLE_ROWS}}
        </tbody>
    </table>
</div>

<div class="footer">
    تم الإنشاء تلقائياً بواسطة runner.py — اختبارات التنفيذ المزدوج لـ لغة ص
</div>

<script>
// ═══ البيانات ═══
const testData = {{TEST_DATA_JSON}};

// ═══ رسم توزيع النتائج (Doughnut) ═══
new Chart(document.getElementById('statusChart'), {
    type: 'doughnut',
    data: {
        labels: ['نجح', 'فشل', 'تخطي'],
        datasets: [{
            data: [{{PASSED}}, {{FAILED}}, {{SKIPPED}}],
            backgroundColor: ['#3fb950', '#f85149', '#d29922'],
            borderColor: '#161b22',
            borderWidth: 3
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            legend: {
                position: 'bottom',
                labels: { color: '#e6edf3', font: { size: 13 } }
            }
        }
    }
});

// ═══ رسم مقارنة الأوقات (Bar) ═══
const dualTests = testData.filter(t => t.compiler_time_ms > 0);
new Chart(document.getElementById('timeChart'), {
    type: 'bar',
    data: {
        labels: dualTests.map(t => t.file.split('/').pop().replace('.ص', '')),
        datasets: [
            {
                label: 'المفسر',
                data: dualTests.map(t => t.interp_time_ms),
                backgroundColor: '#58a6ff',
                borderRadius: 4
            },
            {
                label: 'المترجم',
                data: dualTests.map(t => t.compiler_time_ms),
                backgroundColor: '#bc8cff',
                borderRadius: 4
            }
        ]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            x: {
                ticks: { color: '#8b949e', font: { size: 10 }, maxRotation: 45 },
                grid: { color: '#30363d' }
            },
            y: {
                ticks: { color: '#8b949e' },
                grid: { color: '#30363d' },
                title: { display: true, text: 'ms', color: '#8b949e' }
            }
        },
        plugins: {
            legend: {
                labels: { color: '#e6edf3' }
            }
        }
    }
});

// ═══ فلترة الجدول ═══
function filterTable() {
    const search = document.getElementById('searchBox').value.toLowerCase();
    const status = document.getElementById('statusFilter').value;
    const rows = document.querySelectorAll('#resultsTable tbody tr');
    rows.forEach(row => {
        const text = row.textContent.toLowerCase();
        const rowStatus = row.dataset.status || '';
        const matchSearch = !search || text.includes(search);
        const matchStatus = !status || rowStatus.includes(status);
        row.style.display = (matchSearch && matchStatus) ? '' : 'none';
    });
}

// ═══ ترتيب الجدول ═══
let sortDir = {};
function sortTable(colIdx) {
    const table = document.getElementById('resultsTable');
    const tbody = table.querySelector('tbody');
    const rows = Array.from(tbody.querySelectorAll('tr'));
    sortDir[colIdx] = !sortDir[colIdx];
    rows.sort((a, b) => {
        let aVal = a.cells[colIdx]?.textContent.trim() || '';
        let bVal = b.cells[colIdx]?.textContent.trim() || '';
        const aNum = parseFloat(aVal);
        const bNum = parseFloat(bVal);
        if (!isNaN(aNum) && !isNaN(bNum)) {
            return sortDir[colIdx] ? aNum - bNum : bNum - aNum;
        }
        return sortDir[colIdx] ? aVal.localeCompare(bVal, 'ar') : bVal.localeCompare(aVal, 'ar');
    });
    rows.forEach(r => tbody.appendChild(r));
}
</script>
</body>
</html>"""


def generate_html_report(
    results: list[dict[str, Any]],
    output_path: Path,
    total: int,
    passed: int,
    failed: int,
    skipped: int,
    elapsed: float,
    dual_parity_passed: int = 0,
    interp_only_passed: int = 0,
    compiler_only_passed: int = 0,
    timestamp: str = "",
) -> Path:
    """
    (AR) إنشاء تقرير HTML من بيانات الاختبارات مع فصل مؤشرات التكافؤ.
    (EN) Generate HTML report from test data with parity/interpreter-only indicators.

    المعاملات:
        results: قائمة بيانات الاختبارات (كل عنصر dict بنفس بنية JSON)
        output_path: مسار ملف HTML الناتج
        total, passed, failed, skipped: إحصائيات
        dual_parity_passed: عدد الاختبارات التي نجحت بتكافؤ مزدوج
        interp_only_passed: عدد الاختبارات التي نجحت عبر المفسر فقط
        compiler_only_passed: عدد الاختبارات التي نجحت عبر المترجم فقط (سالبٌ مترجَم)
        elapsed: الوقت الكلي بالثواني
        timestamp: طابع الوقت (اختياري)
    """
    if not timestamp:
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")

    rate = round((passed / total * 100), 1) if total > 0 else 0
    badge_class = "badge-pass" if failed == 0 else "badge-fail"
    badge_text = "✅ جميع الاختبارات نجحت" if failed == 0 else f"❌ {failed} اختبار فشل"

    # (AR) بناء صفوف الجدول
    table_rows = []
    for i, t in enumerate(results, 1):
        status = t.get("status", "UNKNOWN")
        file_name = t.get("file", "")
        interp_ms = t.get("interp_time_ms", 0)
        compiler_ms = t.get("compiler_time_ms", 0)
        error = t.get("error", "")

        # (AR) شريط الوقت البصري
        max_time = max(interp_ms, compiler_ms, 1)
        interp_bar_w = max(4, int(interp_ms / max_time * 80))
        compiler_bar_w = max(4, int(compiler_ms / max_time * 80)) if compiler_ms > 0 else 0

        interp_bar = f'<span class="time-bar time-interp" style="width:{interp_bar_w}px"></span> {interp_ms:.0f}'
        compiler_bar = (
            f'<span class="time-bar time-compiler" style="width:{compiler_bar_w}px"></span> {compiler_ms:.0f}'
            if compiler_ms > 0
            else '<span style="color:var(--text2)">—</span>'
        )

        status_class = f"status-{status}"
        status_label = {
            "PASS": "✅ نجح",
            "FAIL_OUTPUT": "❌ خرج مختلف",
            "FAIL_COMPILE": "❌ خطأ ترجمة",
            "FAIL_INTERP": "❌ خطأ تفسير",
            "FAIL_TIMEOUT": "⏰ مهلة",
            "SKIP": "⏭️ تخطي",
        }.get(status, status)

        error_html = f'<span class="error-text">{error}</span>' if error else ""

        # (AR) عمود النمط: تكافؤ مزدوج أو مفسر فقط أو مترجم فقط.
        #      🔑 وكانَ الفرعُ الأخيرُ يبتلعُ كلَّ ما ليس «مفسّرًا فقط» فيَصِمُ
        #      سالبَ المترجّمِ المنفردَ بـ«تكافؤٍ مزدوج» — وهو المحرّكُ الواحدُ
        #      بعينِه. والقيمةُ تصلُ من `tests/runner.py` مصنَّفةً بالمُسنَدِ
        #      المشترَك `interpreter_is_skipped`، فيكفي أن تُعرَض.
        # (EN) The trailing branch used to swallow everything that was not
        #      interpreter-only, labelling compiler-only negatives "dual parity".
        mode = t.get("mode", "dual_parity")
        if mode == "interpreter_only":
            mode_html = '<span style="color:var(--blue); font-size:0.85em;">🔵 مفسر فقط</span>'
        elif mode == "compiler_only":
            mode_html = '<span style="color:var(--yellow); font-size:0.85em;">🟡 مترجم فقط</span>'
        else:
            mode_html = '<span style="color:var(--green); font-size:0.85em;">🟢 تكافؤ مزدوج</span>'

        row = (
            f'<tr data-status="{status}">'
            f"<td>{i}</td>"
            f"<td>{file_name}</td>"
            f'<td><span class="status-badge {status_class}">{status_label}</span></td>'
            f"<td>{mode_html}</td>"
            f"<td>{interp_bar}</td>"
            f"<td>{compiler_bar}</td>"
            f"<td>{error_html}</td>"
            f"</tr>"
        )
        table_rows.append(row)

    # (AR) استبدال المتغيرات في القالب
    html = _HTML_TEMPLATE
    html = html.replace("{{TIMESTAMP}}", timestamp)
    html = html.replace("{{TOTAL}}", str(total))
    html = html.replace("{{PASSED}}", str(passed))
    html = html.replace("{{FAILED}}", str(failed))
    html = html.replace("{{SKIPPED}}", str(skipped))
    html = html.replace("{{ELAPSED}}", f"{elapsed:.1f}")
    html = html.replace("{{RATE}}", str(rate))
    html = html.replace("{{BADGE_CLASS}}", badge_class)
    html = html.replace("{{BADGE_TEXT}}", badge_text)
    html = html.replace("{{DUAL_PARITY_PASSED}}", str(dual_parity_passed))
    html = html.replace("{{INTERP_ONLY_PASSED}}", str(interp_only_passed))
    html = html.replace("{{COMPILER_ONLY_PASSED}}", str(compiler_only_passed))
    html = html.replace("{{TABLE_ROWS}}", "\n            ".join(table_rows))
    html = html.replace("{{TEST_DATA_JSON}}", json.dumps(results, ensure_ascii=False))

    # (AR) كتابة الملف
    output_path.parent.mkdir(parents=True, exist_ok=True)
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(html)

    return output_path
