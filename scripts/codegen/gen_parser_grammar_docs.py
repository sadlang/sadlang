#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ============================================================================
# (AR) gen_parser_grammar_docs.py — مولّد توثيق قواعد المحلل من مصدر الحقيقة
#      يقرأ language-truth/grammar/*.yaml (قواعد الإنتاج) ويُنتج توثيق Markdown
#      تحت docs/parser_rule/_generated/ — مع: BNF، تفصيل البدائل، المسار إلى المحلل
#      (maps_to)، مخطّط مسار الدوال حتى AST، مخطّط البنية النحويّة (Mermaid)،
#      وروابط «يستدعي / مُستدعى من» المُشتقّة من رسم القواعد.
#
#      ⚠️ المخرجات مُولَّدة آليًّا — لا تُحرَّر يدويًّا. عدّل YAML المصدر ثم أعد التشغيل.
#
# الاستخدام / Usage:
#   python scripts/codegen/gen_parser_grammar_docs.py            # يولّد
#   python scripts/codegen/gen_parser_grammar_docs.py --check    # تحقّق CI (هل محدَّث؟)
# ============================================================================

import sys
import re
import glob
import pathlib
import argparse

try:
    import yaml
except ImportError:
    print("PyYAML مطلوب: pip install pyyaml", file=sys.stderr)
    sys.exit(2)

REPO = pathlib.Path(__file__).resolve().parents[2]
GRAMMAR_DIR = REPO / "language-truth" / "grammar"
OUT_DIR = REPO / "docs" / "parser_rule" / "_generated"
BANNER_AR = (
    "> ⚠️ **هذا الملف مُولَّد آليًّا** من `language-truth/grammar/*.yaml` عبر\n"
    "> `scripts/codegen/gen_parser_grammar_docs.py` — **لا تُحرِّره يدويًّا**.\n"
    "> عدّل YAML المصدر ثم أعد التوليد.\n"
)

LAYER_AR = {
    "program": "النواة (البرنامج)",
    "statements": "الجمل",
    "declarations": "التصريحات والوحدات",
    "oop": "البرمجة الكائنية",
    "expressions": "التعابير وسلسلة الأسبقية",
    "patterns": "أنماط المطابقة",
    "advanced": "البنيات المتقدمة",
    "lexical": "القواعد المعجمية",
}


def load_productions():
    files = sorted(f for f in glob.glob(str(GRAMMAR_DIR / "*.yaml"))
                   if "_notation" not in pathlib.Path(f).name)
    layers = []
    for f in files:
        data = yaml.safe_load(open(f, encoding="utf-8"))
        if not data or "productions" not in data:
            continue
        layers.append((pathlib.Path(f).name, data))
    return layers


def load_lexicon():
    kw, op = {}, {}
    kwf = REPO / "language-truth" / "keywords.yaml"
    if kwf.exists():
        txt = kwf.read_text(encoding="utf-8")
        for m in re.finditer(r'id:\s*"(KW-[A-Z]+-\d+)"[^\n]*?word:\s*"([^"]+)"', txt):
            kw[m.group(1)] = m.group(2)
    opf = REPO / "language-truth" / "operators.yaml"
    if opf.exists():
        txt = opf.read_text(encoding="utf-8")
        for m in re.finditer(r'id:\s*(op\.[a-z_0-9]+),\s*symbol:\s*"([^"]+)"[^\n]*?name_ar:\s*"([^"]+)"', txt):
            op[m.group(1)] = (m.group(2), m.group(3))
    return {"kw": kw, "op": op}


def sanitize(text):
    if text is None:
        return ""
    t = str(text)
    t = t.replace('"', "'").replace("|", "∣").replace("[", "(").replace("]", ")")
    t = t.replace("{", "❲").replace("}", "❳").replace("\n", " ")
    return t.strip()


def leaf_label(sym):
    kind = sym.get("kind")
    if kind == "terminal":
        return f'«{sym.get("value") or sym.get("ref") or "؟"}»'
    if kind == "nonterminal":
        return sym.get("ref", "").split(".")[-1] or "؟"
    return kind or "؟"


def mermaid_for(prod):
    lines = ["```mermaid", "flowchart LR"]
    nid = 0

    def node(label, shape="box"):
        nonlocal nid
        nid += 1
        name = f"n{nid}"
        lbl = sanitize(label)
        if shape == "diamond":
            lines.append(f'  {name}{{"{lbl}"}}')
        elif shape == "round":
            lines.append(f'  {name}(["{lbl}"])')
        else:
            lines.append(f'  {name}["{lbl}"]')
        return name

    def edge(a, b, label=None):
        if label:
            lines.append(f'  {a} -- "{sanitize(label)}" --> {b}')
        else:
            lines.append(f"  {a} --> {b}")

    def emit_symbol(sym):
        kind = sym.get("kind")
        if kind in ("terminal", "nonterminal"):
            n = node(leaf_label(sym))
            return n, n
        children = sym.get("children", []) or []
        if kind == "group":
            return emit_seq(children)
        if kind == "optional":
            e = node("◇", "diamond"); x = node("◇", "diamond")
            f, l = emit_seq(children)
            edge(e, f); edge(l, x); edge(e, x, "تخطّي")
            return e, x
        if kind in ("repeat", "repeat1"):
            e = node("◇", "diamond"); x = node("◇", "diamond")
            f, l = emit_seq(children)
            edge(e, f); edge(l, x); edge(l, f, "تكرار")
            if kind == "repeat":
                edge(e, x, "صفر/أكثر")
            return e, x
        if kind == "alt":
            e = node("◆", "diamond"); x = node("◆", "diamond")
            for ch in children:
                f, l = emit_symbol(ch)
                edge(e, f); edge(l, x)
            return e, x
        n = node(leaf_label(sym))
        return n, n

    def emit_seq(symbols):
        if not symbols:
            p = node("ε", "round")
            return p, p
        first_in = None
        prev_out = None
        for sym in symbols:
            f, l = emit_symbol(sym)
            if first_in is None:
                first_in = f
            else:
                edge(prev_out, f)
            prev_out = l
        return first_in, prev_out

    start = node(prod["lhs"]["name_ar"], "round")
    for alt in prod.get("alternatives", []):
        f, l = emit_seq(alt["symbols"])
        edge(start, f, alt.get("label"))
        done = node("⇒ " + (prod.get("ast_node") or "AST"), "round")
        edge(l, done)
    lines.append("```")
    return "\n".join(lines)


def call_path(prod):
    out = []
    for i, m in enumerate(prod.get("maps_to", []), 1):
        f = m["file"]; fn = m["function"]
        out.append(f"{i}. [`{fn}`](../../../{f}) — `{f}`")
    return out or ["— (لا تتبُّع maps_to مُسجَّل)"]


def short_func(prod):
    mt = prod.get("maps_to") or []
    if not mt:
        return None
    return mt[0]["function"].split("::")[-1]


def collect_nt_refs(prod):
    seen = []

    def walk(sym):
        if not isinstance(sym, dict):
            return
        if sym.get("kind") == "nonterminal" and sym.get("ref"):
            if sym["ref"] not in seen:
                seen.append(sym["ref"])
        for c in sym.get("children", []) or []:
            walk(c)

    for alt in prod.get("alternatives", []):
        for s in alt["symbols"]:
            walk(s)
    return seen


def sym_to_ebnf(sym):
    kind = sym.get("kind")
    if kind == "terminal":
        return f'«{sym.get("value") or sym.get("ref") or "؟"}»'
    if kind == "nonterminal":
        return sym.get("ref", "").split(".")[-1]
    inner = " ".join(sym_to_ebnf(c) for c in (sym.get("children") or []))
    if kind == "optional":
        return f"[ {inner} ]"
    if kind == "repeat":
        return f"{{ {inner} }}"
    if kind == "repeat1":
        return f"( {inner} )+"
    if kind == "group":
        return f"( {inner} )"
    if kind == "alt":
        return "( " + " | ".join(sym_to_ebnf(c) for c in (sym.get("children") or [])) + " )"
    return kind or "؟"


def alt_breakdown(prod):
    out = []
    alts = prod.get("alternatives", [])
    multi = len(alts) > 1
    for i, alt in enumerate(alts, 1):
        seq = " ".join(sym_to_ebnf(s) for s in alt["symbols"])
        lbl = alt.get("label")
        prefix = f"**{i}.** " if multi else "- "
        out.append(f"{prefix}*{lbl}:* `{seq}`" if lbl else f"{prefix}`{seq}`")
    return out


def callpath_mermaid(prod, id2func, id2name):
    me = short_func(prod)
    if not me:
        return None
    lines = ["```mermaid", "flowchart TD"]
    nid = 0
    fnodes = {}

    def fnode(fn):
        nonlocal nid
        if fn in fnodes:
            return fnodes[fn]
        nid += 1
        name = f"f{nid}"
        fnodes[fn] = name
        lines.append(f'  {name}["{sanitize(fn)}()"]')
        return name

    root = fnode(me)
    seen = set()
    for ref in collect_nt_refs(prod):
        cf = id2func.get(ref)
        nm = id2name.get(ref, ref.split(".")[-1])
        if cf and cf != me and (root, cf) not in seen:
            seen.add((root, cf))
            cn = fnode(cf)
            lines.append(f'  {root} -- "{sanitize(nm)}" --> {cn}')
    nid += 1
    ast = f"f{nid}"
    lines.append(f'  {ast}(["⇒ {sanitize(prod.get("ast_node") or "AST")}"])')
    lines.append(f"  {root} --> {ast}")
    lines.append("```")
    return "\n".join(lines)


def rule_link(ref, gmaps, text=None):
    f = gmaps["id2file"].get(ref, "")
    label = text or gmaps["id2name"].get(ref, ref)
    return f"[{label}]({f}#{ref})"


def render_production(prod, seq, gmaps):
    id2func = gmaps["id2func"]; id2name = gmaps["id2name"]; id2callers = gmaps["id2callers"]
    md = []
    lhs = prod["lhs"]
    md.append(f'<a id="{prod["id"]}"></a>')
    md.append(f'### {prod["id"]} — {lhs["name_ar"]} <span dir="ltr">({lhs["nonterminal"]})</span>')
    md.append("")
    md.append(f'- **الرقم التسلسليّ:** `ق-{seq:03d}` · **المعرّف الموحَّد:** `{prod["id"]}` · **الحالة:** {prod.get("status","?")} · **منذ:** {prod.get("since","?")}')
    if prod.get("description_ar"):
        md.append(f'- **الوصف:** {prod["description_ar"].strip()}')
    md.append("")
    md.append("#### 📐 BNF")
    md.append("```bnf")
    md.append((prod.get("ebnf") or "").strip())
    md.append("```")
    md.append("")
    md.append("#### 🧩 تفصيل البدائل")
    md.extend(alt_breakdown(prod))
    md.append("")
    md.append("#### 🔻 المسار إلى المحلل (دوال التحليل) ⇒ AST")
    md.append("**دالة (دوال) الدخول:**")
    for line in call_path(prod):
        md.append(line)
    md.append(f'- **عقدة AST المُنتَجة:** `{prod.get("ast_node","—")}`')
    calls = []
    for ref in collect_nt_refs(prod):
        cf = id2func.get(ref)
        if cf and cf != short_func(prod):
            calls.append(rule_link(ref, gmaps, text=f"`{cf}`"))
    if calls:
        md.append("- **يستدعي دوال:** " + "، ".join(dict.fromkeys(calls)))
    callers = id2callers.get(prod["id"], [])
    if callers:
        cl = "، ".join(rule_link(c, gmaps, text=f"`{id2func.get(c)}`") for c in callers if id2func.get(c))
        if cl:
            md.append("- **مُستدعى من:** " + cl)
    lex = gmaps["lex"]
    refs = prod.get("references") or {}
    refbits = []
    if refs.get("keywords"):
        refbits.append("كلمات: " + "، ".join(f'«{lex["kw"].get(k, k)}»' for k in refs["keywords"]))
    if refs.get("operators"):
        ops = []
        for o in refs["operators"]:
            sym = lex["op"].get(o)
            ops.append(f'«{sym[0]}»' if sym else o)
        refbits.append("عوامل: " + "، ".join(ops))
    if refbits:
        md.append(f'- **روابط المعجم:** {" · ".join(refbits)}')
    md.append("")
    cp = callpath_mermaid(prod, id2func, id2name)
    if cp:
        md.append("##### مخطّط مسار الدوال (حتى AST)")
        md.append(cp)
        md.append("")
    md.append("#### 📊 مخطّط البنية النحويّة (Mermaid)")
    md.append(mermaid_for(prod))
    md.append("")
    if prod.get("example"):
        md.append("#### مثال")
        md.append("```sad")
        md.append(prod["example"].strip())
        md.append("```")
        md.append("")
    md.append("---")
    md.append("")
    return "\n".join(md)


def layer_overview_mermaid(data, gmaps):
    ids = {p["id"] for p in data["productions"]}
    id2func = gmaps["id2func"]
    lines = ["```mermaid", "flowchart TD"]
    nodes = {}
    nid = 0
    for p in data["productions"]:
        fn = id2func.get(p["id"])
        if not fn:
            continue
        nid += 1
        nm = f"o{nid}"
        nodes[p["id"]] = nm
        lines.append(f'  {nm}["{sanitize(p["lhs"]["name_ar"])}<br/>{sanitize(fn)}()"]')
    edges = set()
    for p in data["productions"]:
        if p["id"] not in nodes:
            continue
        for ref in collect_nt_refs(p):
            if ref in ids and ref in nodes and ref != p["id"]:
                key = (nodes[p["id"]], nodes[ref])
                if key not in edges:
                    edges.add(key)
                    lines.append(f"  {key[0]} --> {key[1]}")
    lines.append("```")
    if len(nodes) < 2 or not edges:
        return None
    return "\n".join(lines)


def render_layer(fname, data, start_seq, gmaps):
    layer = data.get("layer", "?")
    title = LAYER_AR.get(layer, layer)
    md = [f"# قواعد المحلل — طبقة {title}", ""]
    md.append(BANNER_AR)
    md.append("")
    md.append(f'- **الطبقة:** `{layer}` · **ملف المصدر:** `language-truth/grammar/{fname}`')
    md.append(f'- **الوصف:** {data.get("description_ar","")}')
    md.append(f'- **عدد القواعد:** {len(data["productions"])}')
    md.append("")
    md.append("> **قراءة المخطّطات:** «📊 مخطّط البنية النحويّة» يُظهر تسلسل الرموز (تكرار «تكرار»،"
              " اختياري «تخطّي»، بدائل ◆). «مخطّط مسار الدوال» يُظهر دوال المحلل التي تُستدعى حتى بناء عقدة AST.")
    md.append("")
    ov = layer_overview_mermaid(data, gmaps)
    if ov:
        md.append("## نظرة عامّة — مسار دوال الطبقة")
        md.append("> الاستدعاءات الداخليّة بين دوال قواعد هذه الطبقة (الروابط عبر الطبقات مذكورة في كل قاعدة).")
        md.append(ov)
        md.append("")
    md.append("---")
    md.append("")
    seq = start_seq
    for prod in data["productions"]:
        md.append(render_production(prod, seq, gmaps))
        seq += 1
    return "\n".join(md), seq


def overall_flow_mermaid(gmaps):
    id2prod = gmaps["id2prod"]; id2name = gmaps["id2name"]
    roots = ["gr.program.program", "gr.program.declaration", "gr.program.statement"]
    lines = ["```mermaid", "flowchart TD"]
    nodes = {}
    nid = 0

    def nd(rid):
        nonlocal nid
        if rid in nodes:
            return nodes[rid]
        nid += 1
        nm = f"x{nid}"
        nodes[rid] = nm
        lines.append(f'  {nm}["{sanitize(id2name.get(rid, rid.split(".")[-1]))}"]')
        return nm
    seen = set()
    for r in roots:
        p = id2prod.get(r)
        if not p:
            continue
        a = nd(r)
        for ref in collect_nt_refs(p):
            if ref in id2prod:
                b = nd(ref)
                if (a, b) not in seen:
                    seen.add((a, b))
                    lines.append(f"  {a} --> {b}")
    lines.append("```")
    return "\n".join(lines)


def render_index(layers, total, gmaps):
    by_status = {}
    by_ast = {}
    for (_, data, _, _) in layers:
        for p in data["productions"]:
            by_status[p.get("status", "?")] = by_status.get(p.get("status", "?"), 0) + 1
            by_ast.setdefault(p.get("ast_node", "—"), []).append(p)

    md = ["# فهرس قواعد لغة ص — المُولَّد من مصدر الحقيقة", ""]
    md.append(BANNER_AR)
    md.append("")
    md.append("المصدر الموحَّد: [`language-truth/grammar/`](../../../language-truth/grammar/) "
              "(قواعد إنتاج YAML). هذا التوثيق مُشتقّ منه آليًّا.")
    md.append("")
    statline = "، ".join(f"{k}: {v}" for k, v in sorted(by_status.items()))
    md.append(f"**إجمالي القواعد:** {total} · **الطبقات:** {len(layers)} · "
              f"**عقد AST مميَّزة:** {len(by_ast)} · **الحالة:** {statline}")
    md.append("")
    md.append("## خريطة التوزيع العليا (البرنامج → التصريح → الجملة)")
    md.append(overall_flow_mermaid(gmaps))
    md.append("")
    md.append("## الطبقات")
    md.append("")
    md.append("| # | الطبقة | الملف المُولَّد | المصدر | عدد القواعد |")
    md.append("|---|--------|----------------|--------|-------------|")
    for i, (fname, data, outname, n) in enumerate(layers, 1):
        layer = data.get("layer")
        md.append(f"| {i} | {LAYER_AR.get(layer, layer)} | [{outname}]({outname}) | `{fname}` | {n} |")
    md.append("")
    md.append("## كل القواعد (المعرّف ⇒ عقدة AST ⇒ دالة المحلل)")
    md.append("")
    md.append("| ق-# | المعرّف الموحَّد | الاسم | عقدة AST | دالة المحلل |")
    md.append("|-----|------------------|-------|----------|-------------|")
    seq = 1
    for (fname, data, outname, n) in layers:
        for prod in data["productions"]:
            fn = prod.get("maps_to", [{}])[0].get("function", "—")
            link = f'[`{prod["id"]}`]({outname}#{prod["id"]})'
            ast_cell = (prod.get("ast_node", "—")).replace("|", "\\|")
            md.append(f'| ق-{seq:03d} | {link} | {prod["lhs"]["name_ar"]} | `{ast_cell}` | `{fn}` |')
            seq += 1
    md.append("")
    md.append("## فهرس حسب عقدة AST")
    md.append("")
    md.append("| عقدة AST | القواعد المُنتِجة |")
    md.append("|----------|--------------------|")
    for ast in sorted(by_ast):
        rules = "، ".join(rule_link(p["id"], gmaps, text=f'`{p["id"]}`') for p in by_ast[ast])
        md.append(f"| `{ast.replace('|', '\\|')}` | {rules} |")
    md.append("")
    return "\n".join(md)


def build_global_maps(layers_raw):
    id2func, id2name, id2prod, id2file = {}, {}, {}, {}
    for fname, data in layers_raw:
        outname = fname.replace(".yaml", ".md")
        for p in data["productions"]:
            id2prod[p["id"]] = p
            id2name[p["id"]] = p["lhs"]["name_ar"]
            id2file[p["id"]] = outname
            fn = short_func(p)
            if fn:
                id2func[p["id"]] = fn
    id2callers = {}
    for pid, p in id2prod.items():
        for ref in collect_nt_refs(p):
            id2callers.setdefault(ref, [])
            if pid not in id2callers[ref]:
                id2callers[ref].append(pid)
    return {"id2func": id2func, "id2name": id2name, "id2file": id2file,
            "id2callers": id2callers, "id2prod": id2prod, "lex": load_lexicon()}


def build():
    layers_raw = load_productions()
    gmaps = build_global_maps(layers_raw)
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    outputs = {}
    index_layers = []
    seq = 1
    for fname, data in layers_raw:
        outname = fname.replace(".yaml", ".md")
        content, seq = render_layer(fname, data, seq, gmaps)
        outputs[OUT_DIR / outname] = content
        index_layers.append((fname, data, outname, len(data["productions"])))
    total = seq - 1
    outputs[OUT_DIR / "INDEX.md"] = render_index(index_layers, total, gmaps)
    return outputs, total


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true", help="تحقّق فقط: هل المخرجات محدَّثة؟ (CI)")
    args = ap.parse_args()
    outputs, total = build()
    if args.check:
        stale = [p.name for p, c in outputs.items()
                 if (p.read_text(encoding="utf-8") if p.exists() else None) != c]
        if stale:
            print("STALE (أعد التوليد):", ", ".join(stale))
            sys.exit(1)
        print(f"OK — التوثيق محدَّث ({total} قاعدة).")
        return
    for path, content in outputs.items():
        path.write_text(content, encoding="utf-8")
    print(f"تم توليد {len(outputs)} ملفًا ({total} قاعدة) في {OUT_DIR.relative_to(REPO)}")


if __name__ == "__main__":
    main()
