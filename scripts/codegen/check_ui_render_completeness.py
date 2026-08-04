#!/usr/bin/env python3
"""
============================================================================
(AR) check_ui_render_completeness.py — حارسُ اكتمالِ تصييرِ عُقَدِ الواجهة.

     المشكلة (ث٤ + ث٨ من تحقيقِ دعمِ المنصّات): `UINodeType` تعدادٌ واحدٌ
     مولَّدٌ من مصدرِ الحقيقة، لكنّ كلَّ مُرسِّمٍ ومولّدِ منصّةٍ يوزّع عليه
     بـ`switch` **مستقلّ**. فحينَ تُضاف عقدةٌ جديدةٌ لا يُخبِرك شيءٌ أنّ مولّدَ
     أندرويد أغفلها: الشيفرةُ تُترجَم، والعقدةُ تسقط في الحالةِ الافتراضيّةِ
     فتخرج «حاويةً عامّة» — انحرافٌ صامتٌ بين المنصّات لا يُكشَف إلّا بالعين.

     هذا الحارسُ يجعل الفجوةَ **قرارًا مكتوبًا**: يقرأ جردَ العُقَدِ وأهدافَ
     التصييرِ ومجموعاتِ الاستثناءِ من `language-truth/ui_nodes.yaml` (مصدرُ
     الحقيقةِ وحدَه — لا مسارَ ولا اسمَ هدفٍ مكتوبٌ هنا)، ثمّ يستخرج مجموعةَ
     `case UINodeType::…` من مصادرِ كلِّ هدف، ويفشل على:
       • فجوةٍ غيرِ مُعلَنة — عقدةٌ بلا حالةٍ وبلا استثناء.
       • استثناءٍ متعفّن — عقدةٌ مُستثناةٌ صارت لها حالةٌ فعلًا (فالاستثناءُ
         يجب أن يُرفَع، وإلّا حَمَت القائمةُ فجوةً لم تعد قائمة).
     الاتّجاهان معًا ضروريّان: الأوّلُ يمنع الانحدار، والثاني يمنع القائمةَ
     من أن تصير غطاءً دائمًا يُخفي ما تحته.

(EN) UI render-completeness guard: diff each render target's `case
     UINodeType::…` set against the SoT node inventory, honouring declared
     exemption groups — and rejecting stale ones.
============================================================================
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parents[2]
_SOT = ROOT / "language-truth" / "ui_nodes.yaml"
_NODE_HEADER = (
    ROOT / "features" / "graphics" / "core" / "include" / "sad_ui"
    / "generated" / "node_types_generated.h"
)

_TYPE_LIST_MACRO = "SAD_UI_NODE_TYPE_LIST"
_TYPE_COUNT_MACRO = "SAD_UI_NODE_TYPE_COUNT"

# (AR) `case UINodeType::X:` — مع أو بلا تأهيلٍ بفضاءِ الاسم، ومع لواحقِ `{`.
_CASE_RE = re.compile(r"case\s+(?:sad::\s*)?(?:ui::\s*)?UINodeType::(\w+)")
_X_ENTRY_RE = re.compile(r'X\((\w+)\s*,\s*"')

_OK = 0
_FAIL = 1


def _fail(message: str) -> None:
    print(f"❌ {message}")


def _enum_ids_from_header(header: Path) -> list[str]:
    """(AR) معرّفاتُ التعدادِ كما يراها المُصرِّف فعلًا (لا كما تصفها الـYAML)."""
    src = header.read_text(encoding="utf-8")
    # (AR) رأسٌ بلا الماكرو (مبتورٌ أو مولَّدٌ بصيغةٍ أخرى) كان يُخرج ValueError
    #      خامًّا بدل تشخيصٍ مفهوم. نُرجع جردًا فارغًا فيتكفّل فحصُ التطابقِ مع
    #      مصدرِ الحقيقةِ بالرسالةِ الصحيحة: «شغّل python x.py gen».
    start = src.find(f"#define {_TYPE_LIST_MACRO}")
    end = src.find(f"#define {_TYPE_COUNT_MACRO}")
    if start < 0 or end < 0 or end <= start:
        return []
    ids: list[str] = []
    for line in src[start:end].splitlines():
        # (AR) الرأسُ المولَّدُ يشرح صيغةَ الماكرو في تعليقٍ `X(Id, "…")` —
        #      فلولا تخطّي التعليقاتِ لدخل `Id` وهمًا في الجرد.
        if line.lstrip().startswith("//"):
            continue
        match = _X_ENTRY_RE.search(line)
        if match:
            ids.append(match.group(1))
    return ids


def _cases_in(root: Path, sources: list[str]) -> set[str]:
    covered: set[str] = set()
    for rel in sources:
        covered |= set(_CASE_RE.findall((root / rel).read_text(encoding="utf-8")))
    return covered


def main(sot_path: Path = _SOT, header: Path = _NODE_HEADER, root: Path = ROOT) -> int:
    sot = yaml.safe_load(sot_path.read_text(encoding="utf-8"))
    nodes = sot["nodes"]
    targets = sot["render_targets"]
    exemptions = {e["id"]: e for e in sot["render_exemptions"]}

    status = _OK
    target_ids = [t["id"] for t in targets]

    # ── سلامةُ الإعلانِ نفسِه ───────────────────────────────────────────────
    for target in targets:
        for rel in target["sources"]:
            if not (root / rel).is_file():
                _fail(f"هدفُ التصيير «{target['id']}» يعلن مصدرًا غيرَ موجود: {rel}")
                status = _FAIL
    for gid, group in exemptions.items():
        unknown = [t for t in group["targets"] if t not in target_ids]
        if unknown:
            _fail(f"مجموعةُ الاستثناء «{gid}» تشير إلى أهدافٍ مجهولة: {unknown}")
            status = _FAIL
        if not str(group.get("reason_ar", "")).strip():
            _fail(f"مجموعةُ الاستثناء «{gid}» بلا سبب — الفجوةُ تُبرَّر أو تُسَدّ")
            status = _FAIL

    # (AR) إعلانٌ معطوبٌ يُبطِل المقارنةَ كلَّها (مصدرٌ لا يُقرَأ، هدفٌ مجهول)،
    #      فنقف هنا بدل أن نُغرِق المستخدمَ بفجواتٍ مشتقّةٍ من العطبِ نفسِه.
    if status != _OK:
        return status

    enum_ids = _enum_ids_from_header(header)
    sot_ids = [n["id"] for n in nodes]
    if enum_ids != sot_ids:
        _fail(
            "جردُ العُقَدِ في مصدرِ الحقيقةِ لا يطابق الرأسَ المولَّد "
            f"({len(sot_ids)} مقابل {len(enum_ids)}) — شغّل `python x.py gen`"
        )
        return _FAIL

    # ── مَن يُستثنى من ماذا ────────────────────────────────────────────────
    exempt_in: dict[str, set[str]] = {t: set() for t in target_ids}
    for node in nodes:
        for gid in node.get("render_exempt") or []:
            group = exemptions.get(gid)
            if group is None:
                _fail(f"العقدة «{node['id']}» تشير إلى مجموعةِ استثناءٍ مجهولة: {gid}")
                status = _FAIL
                continue
            for tid in group["targets"]:
                if tid in exempt_in:
                    exempt_in[tid].add(node["id"])

    # (AR) مجموعةُ استثناءٍ لا تشير إليها عقدةٌ = تبريرٌ لفجوةٍ لم تعد قائمة.
    #      كان هذا الفحصُ في الـpytest وحدَه فلا تراه بوّابةُ x.py؛ نقلناه هنا
    #      فيسري على كلِّ من يشغّل الحارس.
    used_groups = {gid for node in nodes for gid in (node.get("render_exempt") or [])}
    for gid in exemptions:
        if gid not in used_groups:
            _fail(f"مجموعةُ الاستثناء «{gid}» لا تشير إليها عقدةٌ واحدة — احذفها")
            status = _FAIL

    # ── المقارنةُ باتّجاهَيها ───────────────────────────────────────────────
    for target in targets:
        tid = target["id"]
        covered = _cases_in(root, target["sources"])
        missing = [i for i in enum_ids if i not in covered and i not in exempt_in[tid]]
        stale = [i for i in sorted(exempt_in[tid]) if i in covered]
        if missing:
            _fail(
                f"{target['label_ar']} ({tid}): {len(missing)} عقدةً بلا حالةِ تصييرٍ "
                f"وبلا استثناءٍ مُعلَن — {missing}"
            )
            status = _FAIL
        if stale:
            _fail(
                f"{target['label_ar']} ({tid}): استثناءٌ متعفّن — هذه العُقَدُ صارت "
                f"مُصيَّرةً فارفعْ استثناءَها من مصدرِ الحقيقة: {stale}"
            )
            status = _FAIL

    if status == _OK:
        total = len(enum_ids)
        print(f"✅ اكتمالُ التصيير: {total} عقدةً × {len(target_ids)} أهداف")
        for target in targets:
            tid = target["id"]
            print(
                f"   • {target['label_ar']} ({tid}): "
                f"{total - len(exempt_in[tid])} مُصيَّرة، {len(exempt_in[tid])} مُستثناةٌ بإعلان"
            )
    return status


if __name__ == "__main__":
    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except (AttributeError, ValueError):  # (AR) مجرًى ملتقَطٌ لا يقبل إعادةَ الضبط
        pass
    sys.exit(main())
