"""
============================================================================
(AR) test_ui_render_completeness.py — اختباراتُ حارسِ اكتمالِ التصيير.

     قيمةُ حارسٍ في ما **يفشل** عليه لا في مروره على شجرةٍ نظيفة؛ فكلُّ اختبارٍ
     هنا يبني شجرةً مصغَّرةً (مصدرَ حقيقةٍ + رأسًا مولَّدًا + مصدرَ C++) ويُثبت
     أنّ الحارسَ يلتقط صنفًا بعينِه من الانحراف.
(EN) Tests for the render-completeness guard: each builds a miniature tree and
     pins one class of drift the guard must catch.
============================================================================
"""
from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

import pytest
import yaml

_GUARD_PATH = Path(__file__).with_name("check_ui_render_completeness.py")
ROOT = Path(__file__).resolve().parents[2]


def _load_guard():
    spec = importlib.util.spec_from_file_location("ui_render_guard", _GUARD_PATH)
    assert spec and spec.loader, "تعذّر تحميل حارس اكتمال التصيير"
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


guard = _load_guard()

_RENDERER_REL = "renderer.cpp"
_HEADER_REL = "node_types_generated.h"
_SOT_REL = "ui_nodes.yaml"
_TARGET_ID = "toy"


def _header(*ids: str) -> str:
    # (AR) نُقلّد الرأسَ المولَّد بتعليقِ الصيغةِ الذي يسبق القائمةَ فعلًا —
    #      فهو الفخُّ الذي يُدخِل `Id` وهمًا في الجرد إن لم تُتخطَّ التعليقات.
    entries = "".join(f'    X({i}, "اسم_{i}") \\\n' for i in ids)
    return (
        '// (AR) X(Id, "الاسم_القانونيّ") — الترتيبُ هنا هو ترتيبُ أعضاء التعداد.\n'
        "#define SAD_UI_NODE_TYPE_LIST(X) \\\n" + entries + "\n"
        f"#define SAD_UI_NODE_TYPE_COUNT {len(ids)}\n"
    )


def _sot(nodes: list[dict], exemptions: list[dict] | None = None) -> dict:
    return {
        "nodes": nodes,
        "render_targets": [
            {"id": _TARGET_ID, "label_ar": "هدفٌ تجريبيّ", "sources": [_RENDERER_REL]}
        ],
        "render_exemptions": exemptions or [],
    }


def _run(tmp: Path, sot: dict, ids: list[str], cases: list[str]) -> int:
    (tmp / _SOT_REL).write_text(
        yaml.safe_dump(sot, allow_unicode=True, sort_keys=False), encoding="utf-8"
    )
    (tmp / _HEADER_REL).write_text(_header(*ids), encoding="utf-8")
    body = "".join(f"        case UINodeType::{c}: break;\n" for c in cases)
    (tmp / _RENDERER_REL).write_text(
        "void render(UINodeType t) {\n    switch (t) {\n" + body + "    }\n}\n",
        encoding="utf-8",
    )
    return guard.main(sot_path=tmp / _SOT_REL, header=tmp / _HEADER_REL, root=tmp)


def test_complete_target_passes(tmp_path: Path):
    nodes = [{"id": "Alpha"}, {"id": "Beta"}]
    assert _run(tmp_path, _sot(nodes), ["Alpha", "Beta"], ["Alpha", "Beta"]) == 0


def test_undeclared_gap_fails(tmp_path: Path):
    # (AR) عينُ العيب: عقدةٌ جديدةٌ يُغفِلها هدفٌ ⇒ تسقط للحاويةِ العامّةِ صامتةً.
    nodes = [{"id": "Alpha"}, {"id": "Beta"}]
    assert _run(tmp_path, _sot(nodes), ["Alpha", "Beta"], ["Alpha"]) == 1


def test_declared_exemption_passes(tmp_path: Path):
    nodes = [{"id": "Alpha"}, {"id": "Beta", "render_exempt": ["known"]}]
    exempt = [{"id": "known", "targets": [_TARGET_ID], "reason_ar": "دَينٌ مرصود"}]
    assert _run(tmp_path, _sot(nodes, exempt), ["Alpha", "Beta"], ["Alpha"]) == 0


def test_stale_exemption_fails(tmp_path: Path):
    # (AR) الاتّجاهُ الثاني: عقدةٌ استُثنيت ثمّ صارت مُصيَّرة. لو مرّ هذا لظلّت
    #      القائمةُ غطاءً دائمًا يحمي فجواتٍ لم تعد قائمة.
    nodes = [{"id": "Alpha"}, {"id": "Beta", "render_exempt": ["known"]}]
    exempt = [{"id": "known", "targets": [_TARGET_ID], "reason_ar": "دَينٌ مرصود"}]
    assert _run(tmp_path, _sot(nodes, exempt), ["Alpha", "Beta"], ["Alpha", "Beta"]) == 1


def test_exemption_without_a_reason_fails(tmp_path: Path):
    nodes = [{"id": "Alpha", "render_exempt": ["mute"]}]
    exempt = [{"id": "mute", "targets": [_TARGET_ID], "reason_ar": "   "}]
    assert _run(tmp_path, _sot(nodes, exempt), ["Alpha"], []) == 1


def test_unknown_exemption_group_fails(tmp_path: Path):
    nodes = [{"id": "Alpha", "render_exempt": ["ghost"]}]
    assert _run(tmp_path, _sot(nodes), ["Alpha"], []) == 1


def test_unknown_target_in_exemption_fails(tmp_path: Path):
    nodes = [{"id": "Alpha", "render_exempt": ["known"]}]
    exempt = [{"id": "known", "targets": ["nowhere"], "reason_ar": "سبب"}]
    assert _run(tmp_path, _sot(nodes, exempt), ["Alpha"], ["Alpha"]) == 1


def test_missing_source_file_fails(tmp_path: Path):
    sot = _sot([{"id": "Alpha"}])
    sot["render_targets"][0]["sources"] = ["ghost.cpp"]
    (tmp_path / _SOT_REL).write_text(
        yaml.safe_dump(sot, allow_unicode=True, sort_keys=False), encoding="utf-8"
    )
    (tmp_path / _HEADER_REL).write_text(_header("Alpha"), encoding="utf-8")
    assert guard.main(
        sot_path=tmp_path / _SOT_REL, header=tmp_path / _HEADER_REL, root=tmp_path
    ) == 1


def test_sot_out_of_sync_with_generated_header_fails(tmp_path: Path):
    # (AR) الرأسُ هو ما يراه المُصرِّف؛ انحرافُه عن الـSoT يُبطِل كلَّ ما بعده.
    nodes = [{"id": "Alpha"}]
    assert _run(tmp_path, _sot(nodes), ["Alpha", "Beta"], ["Alpha", "Beta"]) == 1


def test_qualified_case_labels_are_counted(tmp_path: Path):
    # (AR) بعضُ المصادرِ تكتب `case sad::ui::UINodeType::X` مؤهَّلًا كاملًا.
    (tmp_path / _SOT_REL).write_text(
        yaml.safe_dump(_sot([{"id": "Alpha"}]), allow_unicode=True, sort_keys=False),
        encoding="utf-8",
    )
    (tmp_path / _HEADER_REL).write_text(_header("Alpha"), encoding="utf-8")
    (tmp_path / _RENDERER_REL).write_text(
        "switch (t) { case sad::ui::UINodeType::Alpha: break; }\n", encoding="utf-8"
    )
    assert guard.main(
        sot_path=tmp_path / _SOT_REL, header=tmp_path / _HEADER_REL, root=tmp_path
    ) == 0


def test_every_declared_exemption_group_is_used(tmp_path: Path):
    # (AR) مجموعةٌ لا تشير إليها عقدةٌ = بندٌ ميّتٌ في مصدرِ الحقيقة.
    sot = yaml.safe_load((ROOT / "language-truth" / "ui_nodes.yaml").read_text("utf-8"))
    referenced = {g for n in sot["nodes"] for g in (n.get("render_exempt") or [])}
    declared = {e["id"] for e in sot["render_exemptions"]}
    assert declared == referenced, f"مجموعاتُ استثناءٍ بلا عقدة: {declared - referenced}"


def test_dead_exemption_group_fails_in_the_guard_itself(tmp_path: Path):
    # (AR) الفحصُ أعلاه يقرأ الشجرةَ الحقيقيّةَ فلا يسري على من يشغّل الحارسَ
    #      وحدَه. هنا نتأكّد أنّ الحارسَ **نفسَه** يفشل على مجموعةٍ ميّتة.
    nodes = [{"id": "Alpha"}]
    exempt = [{"id": "orphan", "targets": [_TARGET_ID], "reason_ar": "بندٌ ميّت"}]
    assert _run(tmp_path, _sot(nodes, exempt), ["Alpha"], ["Alpha"]) == 1


def test_header_without_the_macro_reports_instead_of_crashing(tmp_path: Path):
    # (AR) رأسٌ مبتورٌ كان يُخرج ValueError خامًّا بدل رسالةِ «شغّل x.py gen».
    (tmp_path / _SOT_REL).write_text(
        yaml.safe_dump(_sot([{"id": "Alpha"}]), allow_unicode=True, sort_keys=False),
        encoding="utf-8",
    )
    (tmp_path / _HEADER_REL).write_text("// رأسٌ بلا الماكرو\n", encoding="utf-8")
    (tmp_path / _RENDERER_REL).write_text("switch (t) { }\n", encoding="utf-8")
    assert guard.main(
        sot_path=tmp_path / _SOT_REL, header=tmp_path / _HEADER_REL, root=tmp_path
    ) == 1


def test_guard_passes_on_the_current_tree():
    assert guard.main() == 0


if __name__ == "__main__":
    sys.exit(pytest.main([__file__, "-v"]))
