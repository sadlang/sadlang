#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ======================================================================
# (AR) ملف: scripts/governance/gen_governance_digest.py
# (AR) الوصف: مولّد GOVERNANCE_DIGEST.md — يلخّص حالة الحوكمة في صفحة واحدة
#             من مصادرها القانونية (السبرنت، آخر تقرير تحقّق، القواعد GR،
#             وحالة كل نظام تحت systems/). يحمل بصمة محتوى مُحدَّدة (deterministic)
#             ليُمكِّن حارس CI من كشف أي انجراف (RFC 0004 / ADR-GOV-001).
# (EN) Description: Generator for GOVERNANCE_DIGEST.md — a one-page summary of
#                  governance state derived from its canonical sources. Emits a
#                  deterministic content fingerprint so a CI guard can detect drift.
# ======================================================================
# (AR) مبدأ التحديد (determinism): لا نستخدم وقت الجدار (now). البصمة والتاريخ
#      مشتقّان من محتوى المصادر فقط، فإعادة التوليد في أي يوم تُنتج نفس الملف
#      ما لم تتغيّر المصادر — وهذا شرط نجاح حارس الطزاجة في CI.
# ======================================================================

import argparse
import hashlib
import re
import sys
from pathlib import Path

# (AR) ضمان إخراج UTF-8 على كل المنصّات (Windows قد يستخدم ترميزًا محلّيًّا)
try:
    sys.stdout.reconfigure(encoding="utf-8")
    sys.stderr.reconfigure(encoding="utf-8")
except (AttributeError, ValueError):
    pass


def read_text(path: Path) -> str:
    """(AR) قراءة ملف UTF-8 بأمان (يُرجع نصًّا فارغًا إن غاب)."""
    try:
        return path.read_text(encoding="utf-8")
    except FileNotFoundError:
        return ""


def latest_verification_report(status_dir: Path):
    """(AR) إيجاد آخر VERIFICATION_REPORT_<date>.md بالترتيب المعجمي للتاريخ."""
    reports = sorted(status_dir.glob("VERIFICATION_REPORT_*.md"))
    return reports[-1] if reports else None


def extract_sprint_line(sprint_md: str) -> str:
    """(AR) استخراج سطر «الأسبوع/السبرنت» من ترويسة SPRINT_CURRENT.md."""
    for line in sprint_md.splitlines():
        if "الأسبوع:" in line or "Sprint #" in line:
            # تنظيف الزخارف من سطر الاقتباس
            return line.lstrip("> ").replace("**", "").strip()
    return "غير محدَّد"


def extract_report_meta(report_md: str):
    """(AR) استخراج العنوان والتاريخ والحالة من ترويسة YAML لتقرير التحقّق."""
    def field(name):
        m = re.search(rf'^{name}:\s*"?(.+?)"?\s*$', report_md, re.MULTILINE)
        return m.group(1).strip() if m else "?"
    return field("title"), field("date"), field("status")


def extract_gr_rules(instructions_md: str):
    """(AR) استخراج عناوين القواعد GR-01..NN من النواة النحيلة (مصدرها هناك)."""
    rules = []
    for m in re.finditer(r"\*\*(GR-\d+):\*\*\s*(.+)", instructions_md):
        title = m.group(2).strip().rstrip(".")
        rules.append(f"{m.group(1)}: {title}")
    return rules


def extract_system_status(status_md: str) -> str:
    """(AR) استخراج حالة النظام بتدرّج تراجعيّ (الصيغ تتفاوت بين الأنظمة):
    1) سطر صريح «## الحالة: ...».
    2) حقل `status:` في ترويسة YAML (دورة حياة الوثيقة).
    3) أوّل عنوان `## ...` ذي معنى كتلميح.
    """
    m = re.search(r"^##\s*الحالة:\s*(.+)$", status_md, re.MULTILINE)
    if m:
        return m.group(1).strip()
    m = re.search(r'^status:\s*"?([^"\n]+)"?\s*$', status_md, re.MULTILINE)
    yaml_status = m.group(1).strip() if m else None
    for line in status_md.splitlines():
        line = line.strip()
        if line.startswith("## ") and "ملخّص" not in line and "ملخص" not in line:
            heading = line[3:].strip()
            return f"{yaml_status} — {heading}" if yaml_status else heading
    return yaml_status or "غير معروفة"


def build_digest(root: Path):
    """(AR) بناء نصّ الـDigest كاملًا من المصادر القانونية + البصمة."""
    gov = root / "_bmad-output" / "governance" / "1-policy"
    sprint_path = gov / "sprints" / "SPRINT_CURRENT.md"
    status_dir = gov / "status"
    instructions_path = root / ".github" / "copilot-instructions.md"
    systems_dir = root / "_bmad-output" / "systems"

    # (AR) نجمع كل مصادر البصمة بترتيب ثابت لضمان التحديد
    fingerprint_sources = []

    sprint_md = read_text(sprint_path)
    fingerprint_sources.append(sprint_md)
    sprint_line = extract_sprint_line(sprint_md)

    report_path = latest_verification_report(status_dir)
    report_md = read_text(report_path) if report_path else ""
    fingerprint_sources.append(report_md)
    rep_title, rep_date, rep_status = extract_report_meta(report_md)
    rep_rel = report_path.relative_to(root).as_posix() if report_path else "—"

    instructions_md = read_text(instructions_path)
    fingerprint_sources.append(instructions_md)
    gr_rules = extract_gr_rules(instructions_md)

    # (AR) حالة كل نظام (نتخطّى _TEMPLATE)
    systems = []
    for status_file in sorted(systems_dir.glob("*/status/implementation_status.md")):
        name = status_file.parent.parent.name
        if name == "_TEMPLATE":
            continue
        body = read_text(status_file)
        fingerprint_sources.append(body)
        systems.append((name, extract_system_status(body)))

    # (AR) البصمة = SHA-256 لمحتوى كل المصادر مسلسلًا — مُحدَّدة تمامًا
    h = hashlib.sha256()
    for src in fingerprint_sources:
        h.update(src.encode("utf-8"))
        h.update(b"\x00")
    fingerprint = h.hexdigest()[:16]

    # (AR) بناء النصّ
    lines = []
    lines.append("# 📋 GOVERNANCE_DIGEST — ملخّص الحوكمة (مولَّد آليًّا)")
    lines.append("")
    lines.append("<!-- ⚠️ ملف مولَّد آليًّا — لا تُحرّره يدويًّا. -->")
    lines.append(f"<!-- مولَّد-من: بصمة={fingerprint} | المولّد=scripts/governance/gen_governance_digest.py -->")
    lines.append(f"<!-- إعادة التوليد: python scripts/governance/gen_governance_digest.py -->")
    lines.append("")
    lines.append("> هذا الـDigest بوابة الحوكمة المتدرّجة (ADR-GOV-001 / RFC sadlang/rfcs#5).")
    lines.append("> للمهام الخفيفة (تعديل داخل ملف قائم تحت `status/` أو `stories/`) يكفي قراءته.")
    lines.append("> للمهام البنيويّة (إنشاء/حذف ملف، أو لمس `decisions/`|`planning/`|`sprints/`)")
    lines.append("> تلزم القراءة الكاملة للملفات الخمسة (انظر `.github/copilot-instructions.md`).")
    lines.append("")
    lines.append("## 🏃 السبرنت الحالي")
    lines.append("")
    lines.append(f"- {sprint_line}")
    lines.append(f"- المصدر: `_bmad-output/governance/1-policy/sprints/SPRINT_CURRENT.md`")
    lines.append("")
    lines.append("## ✅ آخر تقرير تحقّق (مصدر حقيقة الحالة)")
    lines.append("")
    lines.append(f"- **التقرير:** `{rep_rel}`")
    lines.append(f"- **التاريخ:** {rep_date} — **الحالة:** {rep_status}")
    lines.append(f"- **العنوان:** {rep_title}")
    lines.append("")
    lines.append("## 📜 القواعد الحوكميّة (GR) — العناوين")
    lines.append("")
    if gr_rules:
        for r in gr_rules:
            lines.append(f"- **{r}**")
    else:
        lines.append("- (تعذّر استخراج القواعد من النواة)")
    lines.append("")
    lines.append("## 🧩 حالة الأنظمة (`systems/`)")
    lines.append("")
    lines.append("| النظام | الحالة |")
    lines.append("|--------|--------|")
    for name, st in systems:
        lines.append(f"| `{name}` | {st} |")
    lines.append("")
    lines.append("---")
    lines.append(f"<!-- نهاية الـDigest — بصمة={fingerprint} -->")
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser(description="مولّد GOVERNANCE_DIGEST.md")
    parser.add_argument("--root", default=".", help="جذر المستودع")
    parser.add_argument("--out", default="_bmad-output/GOVERNANCE_DIGEST.md",
                        help="مسار الملف المولَّد (نسبيّ للجذر)")
    parser.add_argument("--check", action="store_true",
                        help="وضع الفحص: يفشل إن اختلف الملف المُلتزَم عن المولَّد")
    args = parser.parse_args()

    root = Path(args.root).resolve()
    out_path = root / args.out
    content = build_digest(root)

    if args.check:
        existing = read_text(out_path)
        if existing != content:
            sys.stderr.write(
                "❌ (AR) GOVERNANCE_DIGEST.md غير متزامن مع مصادره!\n"
                "   أعد التوليد: python scripts/governance/gen_governance_digest.py\n"
                "❌ (EN) GOVERNANCE_DIGEST.md is out of sync with its sources.\n")
            sys.exit(1)
        print("✅ GOVERNANCE_DIGEST.md متزامن مع مصادره.")
        return

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text(content, encoding="utf-8")
    print(f"✅ كُتب الـDigest: {out_path}")


if __name__ == "__main__":
    main()
