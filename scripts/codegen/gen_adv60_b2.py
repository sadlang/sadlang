#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# (AR) مولّد تغطية الملفّ 60 — الدفعة 2: القنوات (أطلق/اختر) + الواجهة (UI) + inline_asm.
#      مؤرَّضة على الاختبارات الأساسيّة المارّة؛ القنوات محاكاة دقيقة (runner بمهلة/اختبار).
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[2]
BASE = ROOT / "tests" / "behavior" / "rules_matrix" / "60_advanced"

# (rule, cat, seq, desc, requires, skip, expected_or_error, body)
# skip: None | "interp"  (يُضاف @skip_interpreter)
T = [
    # ---- gr.adv.go (أطلق) ----
    ("gr.adv.go", "edge", "051", "أطلق كتلة بمتغيّر محلّيّ", "اطبع_سطر، قناة", None, "9",
     'متغير ق = قناة()\nأطلق\n    متغير ن = 9\n    ق.أرسل(ن)\nنهاية\nاطبع_سطر(ق.استقبل())'),
    ("gr.adv.go", "negative", "050", "أطلق بلا جسم/تعبير يُرفض", "قناة", None, None,
     'أطلق'),

    # ---- gr.adv.select (اختر) ----
    ("gr.adv.select", "edge", "050", "اختر بفرع افتراضيّ والقناة جاهزة (يأخذ القناة)", "اطبع_سطر، قناة", None, "ج",
     'متغير ق = قناة()\nق.أرسل(3)\nاختر\n    عندما ق.استقبل():\n        اطبع_سطر("ج")\n    افتراضي:\n        اطبع_سطر("ف")\nنهاية'),
    ("gr.adv.select", "negative", "050", "اختر بلا «:» بعد القناة يُرفض", "قناة", None, None,
     'اختر\n    عندما 1\n        اطبع_سطر("ج")\nنهاية'),

    # ---- gr.adv.ui_decl (واجهة) — smoke تحليليّ مزدوج ----
    ("gr.adv.ui_decl", "edge", "050", "واجهة بحالتين وطريقة", "اطبع_سطر", None, "ok",
     'واجهة عدّاد\n    @حالة العدد = 0\n    @حالة الخطوة = 1\n    دالة زد()\n        هذا.العدد = هذا.العدد + هذا.الخطوة\n    نهاية\nنهاية\nاطبع_سطر("ok")'),

    # ---- gr.adv.ui_state (@حالة) ----
    ("gr.adv.ui_state", "edge", "050", "حالة بقيمة نصّيّة", "اطبع_سطر", None, "ok",
     'واجهة لوحة\n    @حالة الاسم = "بدون"\nنهاية\nاطبع_سطر("ok")'),

    # ---- gr.adv.widget (اعرض) — مترجم فقط ----
    ("gr.adv.widget", "edge", "050", "عرض عنصر زرّ", "اطبع_سطر", "interp", "ok",
     'اعرض زر("موافق")\nاطبع_سطر("ok")'),

    # ---- gr.adv.ui_modifier_chain — مترجم فقط ----
    ("gr.adv.ui_modifier_chain", "edge", "050", "سلسلة معدِّلات متعدّدة", "اطبع_سطر", "interp", "ok",
     'اعرض نص("مرحبا").حجم(32).لون("أحمر")\nاطبع_سطر("ok")'),

    # ---- gr.adv.ui_event — مترجم فقط ----
    ("gr.adv.ui_event", "edge", "050", "معالج حدث بجسم كتلة", "اطبع_سطر", "interp", "ok",
     'اعرض زر("اضغط").عند_النقر => اطبع_سطر("نُقر")\nاطبع_سطر("ok")'),

    # ---- gr.adv.inline_asm (@تجميع) — مترجم فقط ----
    ("gr.adv.inline_asm", "edge", "050", "تجميع مضمّن بتعليمتين", "اطبع_سطر", "interp", "ok",
     '@تجميع("nop")\n@تجميع("nop")\nاطبع_سطر("ok")'),
]

def main():
    written = 0
    for rule, cat, seq, desc, req, skip, exp, body in T:
        d = BASE / rule / cat
        d.mkdir(parents=True, exist_ok=True)
        name = f"{seq}_{cat}_{rule.split('.')[-1]}.ص"
        lines = [f"# @rule: {rule}", f"# @description: {desc}"]
        if req:
            lines.append(f"# @requires: {req}")
        if skip == "interp":
            lines.append("# @skip_interpreter")
        if exp is None:
            lines.append("# @expect_error")
        else:
            lines.append(f"# @expected: {exp}")
        lines.append("")
        lines.append(body)
        (d / name).write_text("\n".join(lines) + "\n", encoding="utf-8")
        written += 1
    return written

if __name__ == "__main__":
    main()
