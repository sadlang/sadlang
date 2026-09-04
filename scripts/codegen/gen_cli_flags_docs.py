#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مُولِّد توثيق Markdown من cli_flags.yaml — أعلام سطر أوامر لغة ص
(EN) Markdown docs generator from cli_flags.yaml — Sad CLI flags
----------------------------------------------------------------------------
(AR) يقرأ المصدر الوحيد لأعلام سطر الأوامر ويُنتج ملفّ Markdown واحدًا مُلتزَمًا
     يوثّق كلّ الأعلام مجموعةً حسب المحرّك المستهلِك (المترجم/الذاكرة)، ويَسِمُ
     قسمَ أعلامِ المحرّكِ المحذوفِ دَينًا مُقيَّدًا لا عرضًا لما يعمل.
     يعيد استخدام مُحمِّل ومُحقِّق gen_cli_flags.py (لا ازدواج تحقّق). يُحرَس
     بـ x.py gen --check كي لا تصير الوثيقة بائتة.
(EN) Reads the single source of truth for CLI flags and emits one committed
     Markdown file documenting every flag grouped by consuming engine. Reuses
     gen_cli_flags.py's loader/validator. Guarded by x.py gen --check.
============================================================================
"""

from __future__ import annotations

import argparse
import sys
import traceback
from pathlib import Path
from typing import Any

# (AR) إعادة استخدام مُحمِّل/مُحقِّق مولِّد الهيدر (مصدر تحقّق واحد، لا ازدواج).
# (EN) Reuse the header generator's loader/validator (single validation source).
sys.path.insert(0, str(Path(__file__).resolve().parent))
from gen_cli_flags import load_yaml, load_schema, validate_data  # noqa: E402


def md_cell(text: str) -> str:
    """(AR) يُؤمِّن نصًّا لخليّة جدول Markdown: يهرّب | ويطوي الأسطر كي لا تُكسَر البنية.
    (EN) Make text safe for a Markdown table cell: escape | and flatten newlines."""
    return text.replace("\\", "\\\\").replace("|", "\\|").replace("\r", " ").replace("\n", " ")


def _kind_ar(flag: dict[str, Any]) -> str:
    return "منطقيّ" if flag["kind"] == "bool" else "قيميّ"


def _name_cell(flag: dict[str, Any]) -> str:
    """(AR) الاسم القانونيّ، مع `=` للأعلام القيميّة إشارةً لصيغة القيمة."""
    name = flag["canonical"]
    if flag["kind"] == "value":
        name += "="
    return f"`{name}`"


def _hint_cell(flag: dict[str, Any]) -> str:
    hint = flag.get("value_hint", "")
    return f"`{md_cell(hint)}`" if hint else "—"


def _table(flags: list[dict[str, Any]]) -> str:
    lines: list[str] = []
    lines.append("| العلم القانونيّ | النوع | القيمة | الوصف |")
    lines.append("|---|---|---|---|")
    for f in flags:
        lines.append(
            f"| {_name_cell(f)} | {_kind_ar(f)} | {_hint_cell(f)} | {md_cell(f['desc_ar'])} |"
        )
    return "\n".join(lines)


def emit_markdown(data: dict[str, Any]) -> str:
    flags = data["flags"]

    # (AR) تقسيم الأعلام إلى أقسامٍ متنافية شاملة: كلّ علمٍ في قسمٍ واحد بالضبط.
    #      الذاكرة أوّلًا (تسبق أيّ محرّك) كي لا يُحسَب علم ذاكرةٍ مرّتين حتى لو حمل
    #      توليفة محرّكات ثلاثيّة مستقبلًا. حارسٌ يفشل التوليد عند أيّ إفلات.
    # (EN) Partition into mutually-exclusive, exhaustive sections: each flag lands in
    #      exactly one. Memory takes precedence so a future triple-engine memory flag
    #      never double-counts. A guard fails generation on any miss.
    compiler_only: list[dict[str, Any]] = []
    shared: list[dict[str, Any]] = []
    memory: list[dict[str, Any]] = []
    interp_only: list[dict[str, Any]] = []
    for f in flags:
        eng = set(f["engines"])
        if "memory" in eng:
            memory.append(f)
        elif "compiler" in eng and "interpreter" in eng:
            shared.append(f)
        elif "compiler" in eng:
            compiler_only.append(f)
        elif "interpreter" in eng:
            interp_only.append(f)
        else:
            raise ValueError(
                f"Flag '{f['id']}' has un-classifiable engines {sorted(eng)}."
            )
    total = len(compiler_only) + len(shared) + len(memory) + len(interp_only)
    if total != len(flags):
        raise ValueError(
            f"Docs partition lost/duplicated flags: sections sum={total}, flags={len(flags)}."
        )

    shorts = data.get("short_flags", [])
    values = data.get("value_constants", [])

    out: list[str] = []
    out.append("<!-- AUTO-GENERATED FROM language-truth/cli_flags.yaml — DO NOT EDIT MANUALLY -->")
    out.append("<!-- (AR) ملفّ مولَّد. لتعديله: عدّل YAML وأعد التوليد (x.py gen). -->")
    out.append("")
    out.append("# أعلام سطر الأوامر في لغة ص (Sad)")
    out.append("")
    out.append(
        "> **المصدر الوحيد للحقيقة:** `language-truth/cli_flags.yaml` "
        "(نسخة الأعلام: {version}).".format(version=data["version"])
    )
    out.append(">")
    out.append(
        "> لكلّ علمٍ طويل **اسمٌ عربيّ قانونيّ وحيد** — بلا مرادفات ولا توافق خلفيّ. "
        "استعمال اسمٍ ملغًى يُنتج «خيار غير معروف» وخروجًا ≠ 0 في المترجم."
    )
    out.append(">")
    out.append(
        "> **عدد الأعلام الطويلة:** {n} · **الأعلام القصيرة القياسيّة:** {s}".format(
            n=len(flags), s=len(shorts)
        )
    )
    out.append("")
    out.append("## كيف تُقرأ الجداول")
    out.append("")
    out.append("- **النوع:** `منطقيّ` = بلا قيمة؛ `قيميّ` = يقبل القيمة بصيغة **`الاسم=قيمة` حصريًّا** (لا كوسيطٍ منفصل).")
    out.append("- **المحرّك:** `المترجم` = `sad-build` · `الذاكرة` = ماسح سياسة الذاكرة المسبق.")
    out.append("- الوسم `المفسّر` باقٍ في المصدر بلا ثنائيٍّ يقرؤه — انظر القسم 3.")
    out.append("")

    out.append("## 1) أعلام المترجم (`sad-build`) الخاصّة")
    out.append("")
    out.append(_table(compiler_only))
    out.append("")

    # (AR) كان العنوانُ «(تعمل في المحرّكين معًا)» — والشقُّ الثاني زال، لكنّ
    #      الشقَّ الأوّلَ **صحيحٌ ومقيس**: العشرةُ كلُّها يقبلُها `sad-build`
    #      برمزِ خروجٍ ٠. فحذفُ الجملةِ كلِّها يُسقِطُ حقيقةً قائمة.
    # (EN) The heading said "works in both engines"; the second half is gone but
    #      the first is true and measured — all ten are accepted by sad-build
    #      with exit code 0. Dropping the whole sentence would drop a live fact.
    out.append("## 2) عائلة سياسة الذاكرة (تعمل في `sad-build`)")
    out.append("")
    out.append(_table(memory))
    out.append("")

    # (AR) 🔑 كان العنوانُ «أعلام المفسّر الخاصّة» وتحتَه جدولٌ عاديّ، فتُقرأُ
    #      هذه الأعلامُ على أنّها متاحة في **المرجعِ المُودَعِ الذي يُقرَأُ من
    #      الشجرة**. (وكُتِبَ هنا أوّلًا «تُشحَنُ داخلَ الحزم» — ودعوى كاذبة:
    #      `cmake/install.cmake` لا يشحنُ من `docs/` إلّا `docs/book/` وهي
    #      غيرُ موجودةٍ أصلًا، ولا ينسخُ أيُّ مسارِ تحزيمٍ `docs/`. والتصحيحُ
    #      هنا لا في مكانٍ آخرَ: التعليلُ الكاذبُ يُبطِلُ الرقعةَ الصحيحة.)
    #      يناقضُ حكمَ المصدرِ حرفيًّا: «لا يُقرَأُ وجودُ هذه المدخلاتِ أنّ الأعلامَ
    #      تعمل». فالمصدرُ قيَّدَ الدَّينَ والوثيقةُ كانت تمحوه.
    #      والجدولُ باقٍ: الإخفاءُ يُسقِطُ السجلَّ، والوسمُ يُبقيه صادقًا.
    # (EN) The heading read "interpreter-only flags" over a plain table, so a
    #      committed reference read straight from the tree presented them as
    #      available — (this first said "ships inside packages", which is false:
    #      install.cmake ships nothing from docs/ except docs/book/, which does
    #      not exist, and no packaging path copies docs/) —
    #      contradicting the source of truth verbatim: "their presence must NOT
    #      be read as these work". The table stays: hiding it would drop the
    #      record; labelling it keeps the record honest.
    out.append("## 3) أعلام المحرّك المحذوف — دَينٌ مُقيَّد، لا أعلامٌ متاحة")
    out.append("")
    out.append(
        "> ⚠️ **هذه الأعلام بلا مستهلك.** كانت خاصّةً بالمفسّر (`sad-run`)، "
        "وقد زال، فلم يبقَ ثنائيٌّ يقرؤها. وهي مُعلَنةٌ في المصدر **قصدًا لا سهوًا** — "
        "حذفُ علمٍ مُعلَنٍ قرارُ سطحِ لغةٍ لا تنظيفُ شجرة، والسؤالُ المفتوح أيُّها يجب "
        "أن يعرضَه `sad-build`. فلا يُقرَأُ وجودُها هنا أنّها تعمل: تمريرُ أيٍّ منها "
        "اليومَ يُنتج «خيار غير معروف»."
    )
    out.append("")
    out.append(_table(interp_only))
    out.append("")

    out.append("## 4) أعلامٌ كانت مشتركة (المترجم + المحرّك المحذوف)")
    out.append("")
    out.append(
        "هذه الأعلام تعمل في `sad-build`. وكان الجزء الثاني من وسمها "
        "(`interpreter`) يصف محرّكًا زال، فلا يُقرأ منه شيء — يُنقّى في المصدر "
        "مع حسم القسم 3 لا قبله."
    )
    out.append("")
    out.append(_table(shared))
    out.append("")

    out.append("## 5) الأعلام القصيرة القياسيّة (أعراف موروثة)")
    out.append("")
    out.append("| العلم | الوصف |")
    out.append("|---|---|")
    for s in shorts:
        out.append(f"| `{md_cell(s['name'])}` | {md_cell(s['desc_ar'])} |")
    out.append("")

    if values:
        out.append("## 6) القيم القانونيّة للأعلام القيميّة")
        out.append("")
        out.append("| القيمة | ملاحظة |")
        out.append("|---|---|")
        for v in values:
            out.append(f"| `{md_cell(v['value'])}` | {md_cell(v.get('note', ''))} |")
        out.append("")

    out.append("## 7) قواعد السلوك")
    out.append("")
    out.append("1. **القيمة بصيغة `=` حصريًّا:** `--جامع=tracing` صحيح؛ أمّا `--جامع tracing` فيُعامَل `--جامع` صيغةً مجرّدة و`tracing` وسيطًا مستقلًّا.")
    out.append("2. **الصيغة المجرّدة لعلمٍ قيميّ لها معنًى:** `--جامع` = الجمع الكامل؛ `--تحسين-الربط` = LTO كامل؛ `--اشرح` = شرح مفصّل.")
    out.append("3. **فشل عالٍ لا صامت:** أيّ علمٍ طويل مجهول يُنتج «خيار غير معروف» وخروجًا ≠ 0 في المترجم.")
    out.append("4. **لا مرادفات بنيويًّا:** المخطّط يفرض حقلًا قانونيًّا مفردًا، ويشترط حرفًا عربيًّا واحدًا على الأقلّ (يمنع اسمًا لاتينيًّا خالصًا)، ويمنع أيّ حقل `alias`/`aliases`.")
    out.append("")
    out.append("*مُشتقّ حرفيًّا من المصدر الوحيد. عند تعديل الأعلام، عدّل YAML وأعد التوليد (`x.py gen`).*")
    out.append("")
    return "\n".join(out)


def write_if_changed(path: Path, content: str) -> bool:
    if path.exists() and path.read_text(encoding="utf-8") == content:
        return False
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8", newline="\n")
    return True


def main() -> int:
    ap = argparse.ArgumentParser(description="Generate Sad CLI flags Markdown docs")
    ap.add_argument("--yaml", required=True, type=Path)
    ap.add_argument("--schema", required=True, type=Path)
    ap.add_argument("--out-dir", required=True, type=Path)
    ap.add_argument("--quiet", action="store_true")
    args = ap.parse_args()

    try:
        data = load_yaml(args.yaml)
        schema = load_schema(args.schema)
        validate_data(data, schema)

        md = emit_markdown(data)
        out_path = args.out_dir / "cli_flags.ar.md"
        changed = write_if_changed(out_path, md)

        if not args.quiet:
            print(
                f"[gen_cli_flags_docs] OK: {len(data['flags'])} flags, "
                f"docs={'updated' if changed else 'unchanged'}"
            )
        return 0

    except Exception:
        print("[gen_cli_flags_docs] ERROR:", file=sys.stderr)
        traceback.print_exc(file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
