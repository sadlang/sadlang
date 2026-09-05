#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِحقنةُ عيارِ حارسِ المرساة — **يُحقَنُ العطبُ ويُثبَتُ الاحمرار**.

     حارسٌ لم يُثبَتْ احمرارُه ليس حارسًا. وأغلبُ الأعطابِ المُدوَّنةِ في سجلِّ
     دروسِ هذا المستودعِ صورةٌ واحدةٌ مكرّرة: **أخضرُ لأنّه لا يستطيعُ أن يحمرّ**.

     مجسّاتٌ عدّتُها `len(PROBES)` وأرضيّتُها `MIN_PROBES` — ولا تُنثَرُ ههنا
     عددًا يبلى. وأربعةٌ منها **يجبُ أن تبقى خضراءَ** فهي اختباراتُ انحدار:
     ما لا يُقاسُ ينمو، وما يُقاسُ خطأً يُوقِفُ الدمجَ بلا سبب.

     🔑 **والبايتاتُ تُستعادُ ويُتحقَّقُ منها بـsha256** بعدَ كلِّ حقنة. وترفضُ
        المِحقنةُ القياسَ على **أرضيّةٍ ملوّثةٍ** بأثرِ تشغيلةٍ لم تُنهَ — وأثرُها
        **يُشتقُّ من المجسِّ نفسِه** لا يُكتَبُ نسخةً ثانيةً تبلى بأوّلِ تعديل.

(EN) Injection harness for the anchor-integrity guard: inject a defect, prove
     the guard reddens, restore the bytes exactly (sha256-verified). Four
     probes must stay GREEN — they are regression tests, not defects.
============================================================================
"""
from __future__ import annotations

import hashlib
import os
import subprocess
import sys
from datetime import date
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GUARD = ROOT / "scripts" / "codegen" / "check_anchor_integrity.py"
HARNESS = Path(__file__).resolve()
RECORD_DIR = ROOT / "scripts" / "codegen" / "calibration"
RECORD = RECORD_DIR / "check_anchor_integrity.yaml"

# (AR) ثابتانِ بدلَ محرَفَي هروب: تُنقَلُ الشفرةُ عبرَ صدفاتٍ تبتلعُ الخطَّ المائلَ
#      فتنكسرُ بصمتًا.
CRLF = (chr(13) + chr(10)).encode("ascii")
LF_ = chr(10).encode("ascii")


# ═══ البصمةُ تُقاسُ على التمثيلِ الذي يملكُه git ═══════════════════════════
# (AR) 🔑 **مرجعُ القياسِ يلائمُ مرجعَ البصمة.** `.gitattributes` يُعلِنُ
#      `*.py text eol=lf`، فما يُخرِجُه `git checkout` بـLF مهما كانت نهاياتُ
#      الأسطرِ على قرصِ الكاتب. وبصمةٌ على البايتاتِ الخامِّ تُسجَّلُ على تمثيلٍ
#      **لا يملكُه المستودع**، فتُحمِّرُ كلَّ استنساخٍ نظيفٍ بحمرةٍ لا علاقةَ لها
#      بالمحتوى. (بُرهِنَ في مراجعةٍ خصميّةٍ على حارسٍ سابق.)
def _sha_bytes(blob: bytes) -> str:
    return hashlib.sha256((blob or b"").replace(CRLF, LF_)).hexdigest()


def _sha_norm(path) -> str:
    """(AR) بصمةٌ على النصِّ بعدَ تطبيعِ نهاياتِ الأسطرِ إلى ما يُودِعُه git."""
    return _sha_bytes(path.read_bytes())


def _is_date(stamp: str) -> bool:
    """(AR) تاريخٌ فعليٌّ لا شكلٌ يُشبِهُه — و`\\d` في بايثون يوني‌كوديّ."""
    if not stamp.isascii():
        return False
    try:
        date.fromisoformat(stamp)
    except ValueError:
        return False
    return True


def _run_guard() -> tuple[int, str]:
    env = dict(os.environ, PYTHONIOENCODING="utf-8", PYTHONUTF8="1")
    proc = subprocess.run([sys.executable, str(GUARD)], capture_output=True,
                          text=True, encoding="utf-8", cwd=str(ROOT), env=env)
    return proc.returncode, (proc.stdout or "") + (proc.stderr or "")


def _run_eol_invariance() -> tuple[int, str]:
    """(AR) يُنفَّذُ والحارسُ مكتوبٌ بصورةِ LF — ويمرُّ بـ`_sha_norm` **كاتبةِ
    السجلِّ بعينِها** لا بطبقةٍ تحتَها. فحارسٌ يشهدُ لطبقةٍ أسفلَ التي تُكتَبُ
    منها الحقيقةُ يخضرُّ وإن تجاوزتْها العليا. والاستعادةُ مضمونةٌ في `_probe`."""
    blob = GUARD.read_bytes()
    as_lf = _sha_norm(GUARD)
    GUARD.write_bytes(blob.replace(LF_, CRLF))
    as_crlf = _sha_norm(GUARD)
    GUARD.write_bytes(blob)
    if as_lf == as_crlf:
        return 0, "بصمةٌ ثابتةٌ عبرَ نهاياتِ الأسطر: %s" % as_lf[:16]
    return 1, ("انجرافٌ بنهايةِ السطر: LF=%s ≠ CRLF=%s"
               % (as_lf[:16], as_crlf[:16]))


# ═══ الأثرُ يُشتقُّ من المجسِّ ولا يُكتَبُ مرّتَين ══════════════════════════
# (AR) ما يُوسَمُ أثرًا هو ما **لا يُحمِّرُ الحارسَ**: حقنُ بذرةٍ أو مصدرِ حقيقةٍ
#      يفضحُ نفسَه، أمّا ملفٌّ يُنشَأُ في `_archive` (وهو متروكٌ بالتصميم) فأثرُه
#      صامتٌ تمامًا. ولذا `residue=True` يُطلَبُ صراحةً ولا يُفترَض.
MIN_RESIDUE_MARK = 16
_CREATED = object()


def _sub(old: bytes, new: bytes, count: int = 1, residue: bool = False):
    def apply(blob: bytes) -> bytes:
        if blob is None or blob.count(old) < 1:
            raise AssertionError("المرساةُ غيرُ موجودة: %r" % old[:48])
        return blob.replace(old, new, count)
    apply.residue = new if residue else None
    return apply


def _append(extra: bytes):
    def apply(blob: bytes) -> bytes:
        return (blob or b"") + extra
    mark = extra.strip()
    if len(mark) < MIN_RESIDUE_MARK:
        raise AssertionError("سِمةُ أثرٍ أقصرُ من أن تكونَ دليلًا (%d < %d): %r"
                             % (len(mark), MIN_RESIDUE_MARK, mark))
    apply.residue = mark
    return apply


def _create(body: bytes):
    def apply(_blob: bytes) -> bytes:
        return body
    apply.residue = _CREATED
    return apply


SEED = "tests/behavior/rules_matrix/10_statements/gr.stmt.if/basic/001_true_cmp_gt.ص"
GRAMMAR = "language-truth/grammar/10_statements.yaml"
ARCHIVED = "tests/_archive/zz_anchor_probe.ص"
IF_ANCHOR = "# @rule: gr.stmt.if".encode("utf-8")

# (AR) 🔑 وبذرةُ المجسِّ ② **ليست أيَّ بذرة**: يلزمُها قاعدةٌ مرساتُها واحدةٌ
#      فقط، وإلّا لم يُيتِّمْها نزعُ مرساةٍ منها. (وقعتُ فيه أوّلَ مرّة: نُزِعت
#      مرساةُ `gr.stmt.if` فبقيت القاعدةُ مشهودةً ببذورٍ أخرى، ولم يعضَّ المجسّ
#      — **خللٌ في المجسِّ لا في الحارس**.) و`gr.adv.ffi_linkage` مقيسٌ في
#      ٢٠٢٦-٠٩-٠٥ أنّه الوحيدُ بمرساةٍ يتيمةِ العدد.
#      ولو زالت المرساةُ عن مكانِها لأَلقى `_sub` وانتهت المِحقنةُ بالرمز ٢
#      «لم تُقَسْ» — لا بمرورٍ صامت.
SOLE = ("tests/behavior/rules_matrix/_interactions/_generated/"
        "adv.ffi_extern_block__adv.ffi_linkage/001_ffi_block_linkage.ص")
SOLE_ANCHOR = "# @rule: gr.adv.ffi_extern_block, gr.adv.ffi_linkage".encode("utf-8")
STDLIB = "language-truth/stdlib/functions.yaml"
SIR = "language-truth/backend/sir_opcodes.yaml"
_BOM = b"\xef\xbb\xbf"


def _bom_invented(blob: bytes) -> bytes:
    """(AR) بادئةُ BOM + مرساةٌ مخترَعةٌ **في السطرِ الأوّل** — وهي الحالةُ التي
    كانت تُبتلَعُ صامتة. وتُنزَعُ بادئةٌ سابقةٌ إن وُجِدت لئلّا تتكرّر."""
    body = blob[len(_BOM):] if blob.startswith(_BOM) else blob
    return _BOM + "# @rule: gr.stmt.zz_bom_invented\n".encode("utf-8") + body


def _dup_opcode(blob: bytes) -> bytes:
    """(AR) أوپكودٌ جديدٌ باسمٍ **موجودٍ** — يذوبُ في عنوانِ غيرِه."""
    text = blob.decode("utf-8")
    marker = "\n  - name: "
    at = text.index(marker)
    name = text[at + len(marker):].split("\n", 1)[0].strip()
    add = f"\n  - name: {name}\n    description_ar: بندٌ مُعلَنٌ باسمٍ موجود"
    return (text[:at] + add + text[at:]).encode("utf-8")

# (AR) (اسمٌ، ملفٌّ، عطبٌ، رمزٌ منتظَر، نصٌّ منتظَر[، عدّاءٌ])
PROBES = (
    ("① مرساةٌ يتيمة — وسمٌ يُسمّي قاعدةً مخترَعة",
     SEED, _sub(IF_ANCHOR, "# @rule: gr.stmt.zz_invented".encode("utf-8")),
     1, "مرساةٌ يتيمة: 1"),

    ("② دَينُ الشهادةِ ينمو — تُنزَعُ آخرُ مرساةٍ لقاعدة",
     SOLE, _sub(SOLE_ANCHOR, "# @rule: gr.adv.ffi_extern_block".encode("utf-8")),
     1, "بلا شهادة:    3"),

    # (AR) الصيغةُ المعمَّمةُ ليست نصًّا في وثيقةٍ — تُقاسُ. ولولا هذا لكان
    #      `@مُعلَن:` وسمًا ميّتًا يُكتَبُ ولا يقرؤه أحد.
    ("③ `@مُعلَن:` يُقرأُ فعلًا — مدمجٌ مخترَع",
     SEED, _append("\n# @مُعلَن: bi.ZzNever.NEVER\n".encode("utf-8")),
     1, "bi.ZzNever.NEVER"),

    # (AR) وهذا ما لم يكن يُقاسُ قطُّ: الرمزُ كان علامةَ «بذرةٌ سالبة» فقط،
    #      فبذرةٌ تكتبُ `SEM999` تمرُّ صامتةً ولا يقابلُها الكتالوج.
    ("④ `@expect_error` يُقابَلُ بالكتالوج — SEM999",
     SEED, _append("\n# @expect_error: SEM999\n".encode("utf-8")),
     1, "err.SEM999"),

    # (AR) العنوانُ العاري ليس عنوانًا: ٢٦ معرِّفَ مدمجٍ يتكرّرُ عبرَ النطاقات.
    ("⑤ العنوانُ مؤهَّلٌ — معرِّفٌ عارٍ يُرَدُّ",
     SEED, _append("\n# @مُعلَن: bi.ADD\n".encode("utf-8")),
     1, "bi.ADD"),

    ("⑥ مُعلَنٌ جديدٌ بلا شهادةٍ يرفعُ الدَّين",
     GRAMMAR, _append("  - id: gr.stmt.zz_probe\n    lhs: {nonterminal: ZzProbe}\n"
                      "    ebnf: \"ZzProbe = 'zz' ;\"\n".encode("utf-8")),
     1, "gr.stmt.zz_probe"),

    # ═══ اختباراتُ انحدارٍ — يجبُ أن تبقى خضراء ═══
    # (AR) `_archive` متروكٌ بالتصميم: شجرةٌ محفوظةٌ بلهجةٍ ماتت لا تُشغَّل.
    #      وهذا المجسُّ يُثبِتُ أنّ تركَها **مقصودٌ ومقيسٌ** لا سهوٌ في الكنس.
    ("⑦ الأرشيفُ متروكٌ — مرساةٌ يتيمةٌ فيه لا تُحمِّر",
     ARCHIVED, _create("# @rule: gr.zz.archived_never\nاطبع_سطر(\"x\")\n"
                       .encode("utf-8")),
     0, "مرساةٌ يتيمة (ترسو على غيرِ مُعلَن): 0"),

    # (AR) وسمٌ في **جسمِ** الملفِّ لا في أوّلِ سطرٍ ليس مرساة. ولولا هذا لكان
    #      ذكرُ اسمِ قاعدةٍ في وصفٍ نثريٍّ يُقرأُ شهادةً.
    ("⑧ نصٌّ في الجسمِ ليس مرساةً — لا شهادةَ بالذِّكر",
     SEED, _append("اطبع_سطر(\"@rule: gr.zz.in_body\")\n".encode("utf-8")),
     0, "مرساةٌ يتيمة (ترسو على غيرِ مُعلَن): 0"),

    # (AR) بادئةُ BOM تسبقُ `#` فلا يطابقُها `^#`. وفي الشجرةِ ٨٦ بذرةً
    #      ببادئةٍ، أربعٌ منها مرساتُها في السطرِ الأوّل. ولولا هذا المجسِّ لعادَ
    #      العطبُ بأوّلِ رقعةٍ تُعيدُ `utf-8` مكانَ `utf-8-sig`.
    ("⑪ بادئةُ BOM لا تبتلعُ المرساة",
     SEED, _bom_invented, 1, "مرساةٌ يتيمة: 1"),

    # (AR) الصيغةُ الخاليةُ من النقطتَين هي **الأغلبيّةُ المُودَعة** (١٣١ مقابل
    #      ١٠١). وكان الحارسُ يقرأُ أقلَّ من نصفِ ما يزعمُ حراستَه.
    ("⑫ `@expect_error` بلا نقطتَين يُقابَلُ بالكتالوج",
     SEED, _append("\n# @expect_error SEM999\n".encode("utf-8")),
     1, "err.SEM999"),

    # (AR) انكماشُ المُعلَنِ ليس تقدُّمًا: عائلةٌ تسقطُ صامتةً و`0 ≤ السقف`.
    ("⑬ عائلةٌ تنكمشُ — أرضيّةُ المُعلَن",
     STDLIB, _sub(b"functions:", b"functions_ZZ_RENAMED:"),
     1, "انكمشَ المُعلَن"),

    # (AR) بندٌ مُعلَنٌ باسمٍ موجودٍ يذوبُ في عنوانِ غيرِه فلا يُعَدُّ ولا يُطالَب.
    ("⑭ معرِّفٌ مكرَّرٌ يبتلعُ بندًا — سقفُ التصادم",
     SIR, _dup_opcode, 1, "نما الذوبان"),

    # (AR) الهجاءُ الثاني: `compile_` **بين** `expect_` و`error`، فلا لاحقةٌ
    #      تبلغُه. ٧٥ موضعًا في الشجرة، وحارسانِ شقيقانِ يعدّانِه ندًّا.
    ("⑮ `@expect_compile_error` هجاءٌ ثانٍ يُقابَلُ بالكتالوج",
     SEED, _append("\n# @expect_compile_error: SEM999\n".encode("utf-8")),
     1, "err.SEM999"),

    # (AR) قائمةٌ برموزٍ — تناظرًا مع `@rule:`. ولولا انتزاعِ الرمزِ من صدرِ كلِّ
    #      جزءٍ لبقيَ الرمزُ الثاني غيرَ مقروءٍ صامتًا.
    ("⑯ قائمةُ رموزٍ — الثاني يُقرأُ كالأوّل",
     SEED, _append("\n# @expect_error: SEM001, SEM999\n".encode("utf-8")),
     1, "err.SEM999"),

    # (AR) طبقةُ القيدِ مستثناةٌ من الإعلان، فحذفُ أوپكودٍ من `sir` له توأمٌ فيها
    #      **ينكمشُ فعلًا** وتعضُّ الأرضيّة. وقبلَ الاستثناءِ كان التوأمُ يملأُ
    #      العنوانَ الشاغرَ فيمرُّ الحذفُ صامتًا.
    ("⑰ حذفُ أوپكودٍ له توأمٌ في طبقةِ القيدِ يعضّ",
     SIR, _sub(b"  - name: BUILTIN_CLI\n", b""),
     1, "انكمشَ المُعلَن"),

    ("⑨ مسبارٌ أعمى — رمزُ عطبِ آلةٍ 2",
     "scripts/codegen/check_anchor_integrity.py",
     _sub(b'SKIP_PARTS = ("_archive",)',
          b'SKIP_PARTS = ("_archive", "behavior", "unit")', residue=True),
     2, "عيارُ الأداة:"),

    ("⑩ البصمةُ لا تتغيّرُ بنهاياتِ الأسطرِ (LF)",
     "scripts/codegen/check_anchor_integrity.py",
     lambda blob: (blob or b"").replace(CRLF, LF_),
     0, "بصمةٌ ثابتة", _run_eol_invariance),
)

# (AR) أرضيّةُ العمق. تُقرأُ في الحارسِ الفوقيِّ أيضًا (`CEILING_MIN_PROBES`)،
#      وههنا تمنعُ **المِحقنةَ نفسَها** من إعلانِ نجاحٍ بلا قياس: بـ`PROBES = ()`
#      تُرجِعُ صفرًا وتكتبُ `0/0`. وأداةُ قياسٍ تُعلِنُ نجاحًا بلا أن تقيسَ هي
#      عينُ ما تُنشَأُ لمنعِه.
MIN_PROBES = 17


def _derive_residue() -> tuple:
    """(AR) (مسارٌ، بصمةُ أثرٍ) لكلِّ مجسٍّ يتركُ أثرًا صامتًا. `None` = ملفٌّ يُنشَأ."""
    out = []
    for entry in PROBES:
        path, mutate = entry[1], entry[2]
        mark = getattr(mutate, "residue", None)
        if mark is _CREATED:
            out.append((path, None))
        elif mark:
            out.append((path, mark))
    return tuple(out)


def _residue() -> list[str]:
    """(AR) أثرُ تشغيلةٍ سابقةٍ لم تُنهَ. وجودُه ⇒ لا قياسَ، رمزُ ٢.

    و`finally` يحمي من الاستثناءِ ومن Ctrl-C، **ولا يحمي من SIGKILL**.
    وأداةٌ تقيسُ فوقَ أثرِ نفسِها تُثبِتُ ما لا تعرف.
    """
    found: list[str] = []
    for rel, mark in _derive_residue():
        path = ROOT / rel
        if mark is None:
            if path.exists():
                found.append(f"{rel} — ملفُّ مجسٍّ باقٍ")
        elif path.is_file() and mark in path.read_bytes():
            found.append(f"{rel} — أثرُ حقنٍ باقٍ: {mark.decode('utf-8')[:40]}")
    return found


def _probe(path: str, mutate, want_code: int, want_text: str,
           runner=None) -> tuple[bool, str]:
    target = ROOT / path
    existed = target.exists()
    original = target.read_bytes() if existed else None
    before = hashlib.sha256(original).hexdigest() if existed else None
    try:
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(mutate(original))
        code, out = (runner or _run_guard)()
    finally:
        if existed:
            target.write_bytes(original)          # type: ignore[arg-type]
        else:
            target.unlink(missing_ok=True)
    if existed:
        if hashlib.sha256(target.read_bytes()).hexdigest() != before:
            raise AssertionError("لم تُستعَدِ البايتاتُ في " + path)
    elif target.exists():
        raise AssertionError("لم يُحذَفِ الملفُّ المؤقّت: " + path)
    ok = (code == want_code) and (want_text in out)
    detail = "رمز=%d (منتظَر %d)" % (code, want_code)
    if want_text not in out:
        detail += " · النصُّ المنتظَرُ غائب"
    return ok, detail


def _write_record(passed: int, stamp: str) -> None:
    RECORD_DIR.mkdir(parents=True, exist_ok=True)
    lines = [
        "# ═══════════════════════════════════════════════════════════════════",
        "# سجلُّ عيارٍ — مُشتَقٌّ آليًّا، لا يُحرَّرُ باليد.",
        "# ⚠️ إن تغيّرَ الحارسُ أو المِحقنةُ ولم يُعَدِ العيارُ حمِرَ",
        "#    check_calibration_fresh.py. وإعادتُه:",
        "#    python scripts/codegen/calibrate_anchor_integrity.py --record --date=YYYY-MM-DD",
        "# ═══════════════════════════════════════════════════════════════════",
        "version: 2",
        "guard: scripts/codegen/check_anchor_integrity.py",
        "harness: scripts/codegen/calibrate_anchor_integrity.py",
        "guard_sha256: %s" % _sha_norm(GUARD),
        "harness_sha256: %s" % _sha_norm(HARNESS),
        "calibrated_at: %s" % stamp,
        "probes_total: %d" % len(PROBES),
        "probes_passed: %d" % passed,
        "result: %d/%d" % (passed, len(PROBES)),
        "probes:",
    ]
    for entry in PROBES:
        name, path, _m, code, text = entry[:5]
        role = ("انحدار — يجبُ أن يبقى أخضر" if code == 0
                else "عطبُ آلة" if code == 2 else "يجبُ أن يحمرّ")
        lines.append('  - name: "%s"' % name)
        lines.append("    target: %s" % path)
        lines.append("    expect_exit: %d" % code)
        lines.append('    expect_text: "%s"' % text)
        lines.append('    role: "%s"' % role)
    RECORD.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    record = "--record" in sys.argv
    stamp = ""
    for arg in sys.argv[1:]:
        if arg.startswith("--date="):
            stamp = arg.split("=", 1)[1]
    if record and stamp and not _is_date(stamp):
        print("✗ --date شكلُه YYYY-MM-DD — و%r ليس تاريخًا. سجلٌّ يحملُ نصًّا"
              " مكانَ تاريخٍ لا يُقالُ عنه متى عُويِر." % stamp)
        return 2
    if record and not stamp:
        print("✗ --record يلزمُه --date=YYYY-MM-DD (لا يُقرأُ وقتُ النظامِ"
              " لئلّا يتغيّرَ السجلُّ بلا عمل)")
        return 2

    print("عيارُ حارسِ «المُعلَنُ يعرفُ مَن يشهدُ له» بالحقن:")
    print("%-50s %-9s %s" % ("المجسّ", "النتيجة", "التفصيل"))
    print("-" * 88)

    dirty = _residue()
    if dirty:
        print("✗ عطبُ آلة: أرضيّةٌ ملوّثةٌ بأثرِ تشغيلةٍ سابقةٍ لم تُنهَ —"
              " لا يُقاسُ فوقَه:")
        for item in dirty:
            print("    · %s" % item)
        print("  نظِّفْ ثمّ أعِدْ: git checkout -- <الملفّ> · احذفِ الملفَّ الباقي.")
        return 2

    if len(PROBES) < MIN_PROBES:
        print("✗ عطبُ آلة: %d مجسًّا < %d — مِحقنةٌ قُلِّصت، فلا تُعلِنُ نجاحًا."
              % (len(PROBES), MIN_PROBES))
        return 2

    passed = 0
    for entry in PROBES:
        name, path, mutate, code, text = entry[:5]
        ok, detail = _probe(path, mutate, code, text,
                            entry[5] if len(entry) > 5 else None)
        passed += 1 if ok else 0
        print("%-50s %-9s %s" % (name, "عضَّ" if ok else "لم يعضّ", detail))
    print("-" * 88)
    print("النتيجة: %d/%d" % (passed, len(PROBES)))

    code, out = _run_guard()
    tail = [ln for ln in out.strip().split("\n") if ln.strip()][-1:]
    print("وبعدَ الاستعادة: رمز=%d · %s" % (code, tail[0].strip() if tail else ""))

    if passed != len(PROBES) or code != 0:
        print("✗ لا يُودَعُ سجلُّ عيارٍ ناقص.")
        return 1
    if record:
        _write_record(passed, stamp)
        print("✓ أُودِعَ السجلُّ: %s" % RECORD.relative_to(ROOT).as_posix())
    return 0


if __name__ == "__main__":
    # (AR) 🔑 انهيارٌ ليس حكمًا: مرساةُ حقنٍ اختفَتْ، أو ملفُّ مجسٍّ غيرُ مقروء
    #      ⇒ **لم تُقَسْ**، فرمزُها ٢ لا ١.
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:  # noqa: BLE001 — كلُّ انهيارٍ عطبُ آلةٍ لا حكم
        sys.stdout.reconfigure(encoding="utf-8")
        print(f"✗ عطبُ آلةٍ — لم يُقَسْ شيء: {exc.__class__.__name__}: {exc}")
        raise SystemExit(2)
