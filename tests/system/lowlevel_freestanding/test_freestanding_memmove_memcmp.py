"""
============================================================================
(AR) اختبارات منع انحدار: إكمال وقت التشغيل الحرّ بـ memmove وmemcmp.
     clang تحت -ffreestanding يفترض وجود memset/memcpy/memmove/memcmp عند
     خفض intrinsics. المترجم كان يُصدر memset/memcpy الضعيفتين فقط؛ فبقيت
     memmove/memcmp بلا مزوّد ⇒ يلزم تعريفهما في طبقة C خارجيّة (نواة النحلة
     تراب.c). هذا التغيير يُصدرهما ضعيفتين (weak_odr) نظير memset/memcpy،
     فيصير وقت التشغيل الحرّ مكتفيًا ذاتيًّا (يسدّ آخر اعتماد على تراب.c لهما).

     الحرّاس هنا:
       - memmove/memcmp مُعرَّفتان (define) لا مُصرَّحتان فقط، بربط weak_odr.
       - كلّ منهما يظهر رمزًا **مرّة واحدة** (التعريف) ⇒ لا نداء ذاتيّ ولّده
         تمييز LLVM للحلقات (loop-idiom) ⇒ لا تكرار لانهائيّ (يمنعه no-builtins).
       - memset/memcpy ما زالتا مُصدَّرتين (لا انحدار).
       - الوحدة تجتاز مدقّق LLVM (رمز خروج 0) ⇒ IR سليم بنيويًّا (PHI/هيمنة).
(EN) Regression tests: freestanding runtime completed with memmove and memcmp,
     emitted weak_odr like memset/memcpy so the freestanding runtime is
     self-sufficient (removes the last dependency on an external C layer).
----------------------------------------------------------------------------
(AR) لماذا --أظهر-llvm؟ الخفض يعيش في توليد LLVM؛ الراية تُخرج IR بلا اعتماد
     على lld-link/LIB (عزل عن بيئة الربط) — نمط test_cpu_ctl_i686.
============================================================================
"""
from __future__ import annotations

import re
import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

# (AR) ومسارات بلا ‎.exe‎ للينكس/ماك: بدونها يبتلع ‎skipif‎ الملفَّ أخضرَ صامتًا في CI.
_CANDIDATES = [
    ROOT / "build" / "bin" / "Release" / "sad-build.exe",
    ROOT / "build" / "bin" / "Debug" / "sad-build.exe",
    ROOT / "build" / "bin" / "sad-build.exe",
    ROOT / "build" / "bin" / "Release" / "sad-build",
    ROOT / "build" / "bin" / "Debug" / "sad-build",
    ROOT / "build" / "bin" / "sad-build",
]
SAD_BUILD = next((p for p in _CANDIDATES if p.exists()), None)

pytestmark = pytest.mark.skipif(
    SAD_BUILD is None,
    reason="sad-build.exe غير مبني — يتطلب: python x.py build --config Release",
)

FREESTANDING = "--حرّ"
EMIT_LLVM = "--أظهر-llvm"

# (AR) برنامج حرّ تافه — يكفي لتشغيل مسار emitFreestandingRuntime (يُصدر
#      malloc/free/memcpy/memmove/memset/memcmp… بلا اعتماد على استعمالها).
_TRIVIAL = "متغير س = 1\nاطبع(س)\n"


def _compile_ir(source: str, *extra_flags: str) -> tuple[int, str, str]:
    """(AR) يترجم المصدر حرًّا مع إخراج IR؛ يعيد (رمز الخروج، الخرج، IR)."""
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(source, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), FREESTANDING, *extra_flags, EMIT_LLVM, "-o", str(out_ll)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=120, cwd=work,
        )
        ir = out_ll.read_text(encoding="utf-8", errors="replace") if out_ll.exists() else ""
        return proc.returncode, (proc.stdout or "") + (proc.stderr or ""), ir


def _compile_ir_hosted(source: str, target: str) -> tuple[int, str, str]:
    """(AR) يترجم المصدر **مستضافًا** (بلا ‎--حرّ‎) لهدف محدَّد؛ يعيد IR.

    (AR) دوالّ مثل qsort/strspn/fread محجوبة في الوضع الحرّ، لكنّها تُستدعى
         مستضافةً وتربط libc حقيقيّة — وهناك يقع عطب توقيع size_t على 32-بت.
    """
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(source, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), f"--هدف={target}", EMIT_LLVM, "-o", str(out_ll)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=120, cwd=work,
        )
        ir = out_ll.read_text(encoding="utf-8", errors="replace") if out_ll.exists() else ""
        return proc.returncode, (proc.stdout or "") + (proc.stderr or ""), ir


def _define_re(name: str) -> re.Pattern:
    # define [linkage...] <ret> @name(
    return re.compile(r"define\b[^\n]*@" + re.escape(name) + r"\s*\(")


def test_memmove_defined_weak_odr():
    """(AR) memmove مُعرَّفة weak_odr في الوضع الحرّ (لا مجرّد تصريح)."""
    code, out, ir = _compile_ir(_TRIVIAL)
    assert code == 0, "الترجمة الحرّة فشلت (IR غير سليم؟):\n" + out
    m = _define_re("memmove").search(ir)
    assert m, "IR الحرّ بلا `define ... @memmove(` — لم يُصدَر التعريف:\n" + ir[:1500]
    line = ir[m.start():ir.find("\n", m.start())]
    assert "weak_odr" in line, "memmove ليست weak_odr (تنجو من llvm-link ودمج الوحدات):\n" + line


def test_memcmp_defined_weak_odr():
    """(AR) memcmp مُعرَّفة weak_odr وتعيد i32."""
    code, out, ir = _compile_ir(_TRIVIAL)
    assert code == 0, "الترجمة الحرّة فشلت:\n" + out
    m = _define_re("memcmp").search(ir)
    assert m, "IR الحرّ بلا `define ... @memcmp(`:\n" + ir[:1500]
    line = ir[m.start():ir.find("\n", m.start())]
    assert "weak_odr" in line, "memcmp ليست weak_odr:\n" + line
    assert " i32 " in line, "memcmp لا تعيد i32 (عقد C: int memcmp):\n" + line


def test_no_self_recursion_memmove_memcmp():
    """(AR) كلّ رمز مرّة واحدة (التعريف) ⇒ لا نداء ذاتيّ من loop-idiom (لا تكرار لانهائيّ)."""
    code, out, ir = _compile_ir(_TRIVIAL)
    assert code == 0, out
    # (AR) في برنامج تافه لا مستدعٍ خارجيّ لهما ⇒ الظهور الوحيد هو سطر define.
    assert ir.count("@memmove") == 1, (
        "@memmove يظهر أكثر من مرّة — نداء ذاتيّ محتمل ولّده تمييز الحلقات "
        "(no-builtins يجب أن يمنعه) ⇒ خطر تكرار لانهائيّ"
    )
    assert ir.count("@memcmp") == 1, "@memcmp يظهر أكثر من مرّة — نداء ذاتيّ محتمل"


def test_memset_memcpy_still_emitted():
    """(AR) لا انحدار: memset/memcpy القائمتان ما زالتا مُصدَّرتين."""
    code, out, ir = _compile_ir(_TRIVIAL)
    assert code == 0, out
    assert _define_re("memcpy").search(ir), "انحدار: memcpy لم تعد مُصدَّرة"
    assert _define_re("memset").search(ir), "انحدار: memset لم تعد مُصدَّرة"


# ───── طول دوالّ الذاكرة = size_t الهدف (لا i64 ثابتًا) ─────
# (AR) الخلفيّة تخفّض llvm.memset/memcpy/memmove (وexpandMemCmp) إلى **نداءات
#      مكتبيّة بوسيط size_t**. تعريفٌ بـi64 ثابت على هدف 32-بت يقرأ 8 بايت بينما
#      النداء يدفع 4 ⇒ النصف الأعلى قمامة ⇒ حجم هائل ⇒ دوس ذاكرة صامت. رصدته
#      مراجعة أميليا بالتفكيك (نواة النحلة، بعد أن حُذف تعريف C الذي كان يقنّعه).

I686 = "--هدف=i686-unknown-elf"
_MEM_FNS = ("memcpy", "memmove", "memset", "memcmp")
# (AR) المخصّصات: طولها آخر وسيط أيضًا (calloc وسيطاه كلاهما size_t).
_ALLOC_FNS = ("malloc", "realloc", "calloc")


def _len_param_type(ir: str, name: str) -> str:
    """(AR) نوع آخر وسيط (الطول) في سطر define الخاصّ بالدالّة."""
    m = _define_re(name).search(ir)
    assert m, f"IR بلا تعريف @{name}"
    line = ir[m.start():ir.find("\n", m.start())]
    inner = line[line.index("(") + 1:line.rindex(")")]
    return inner.split(",")[-1].strip().split()[0]


@pytest.mark.parametrize("fn", _MEM_FNS)
def test_mem_length_is_i32_on_i686(fn):
    """(AR) على i686: الطول i32 (size_t) — يطابق النداء المكتبيّ الذي تولّده الخلفيّة."""
    code, out, ir = _compile_ir(_TRIVIAL, I686)
    assert code == 0, f"الترجمة الحرّة لـi686 فشلت:\n{out}"
    ty = _len_param_type(ir, fn)
    assert ty == "i32", (
        f"@{fn} على i686 طوله {ty} لا i32 — عدم تطابق ABI مع النداء المكتبيّ "
        f"(الخلفيّة تدفع size_t=4 بايت، والتعريف يقرأ 8) ⇒ حجم قمامة ودوس ذاكرة"
    )


# ───── المخصّصات: نفس عقد size_t ─────
# (AR) ⚠️ هذه الحرّاس تقفل ما رصدته الجولة الثانية من المراجعة: malloc/realloc/
#      calloc/strlen كانت i64 ثابتًا فتنكسر على 32-بت **المستضاف** (يربط libc
#      حقيقيّة). ولم تكن مغطّاة إطلاقًا بحرّاس mem* أعلاه.
#      strlen معيارها **العائد** لا الوسيط — على 32-بت تعيد eax وحده، فإعلانها
#      i64 يجعل المستدعي يقرأ edx قمامةً.

@pytest.mark.parametrize("fn", _ALLOC_FNS)
def test_alloc_size_is_i32_on_i686(fn):
    """(AR) على i686: وسيط الحجم في المخصّصات i32 (size_t)."""
    code, out, ir = _compile_ir(_TRIVIAL, I686)
    assert code == 0, out
    ty = _len_param_type(ir, fn)
    assert ty == "i32", (
        f"@{fn} على i686 حجمه {ty} لا i32 — عدم تطابق ABI مع libc/النداء المولَّد"
    )


@pytest.mark.parametrize("fn", _ALLOC_FNS)
def test_alloc_size_is_i64_on_host(fn):
    """(AR) لا انحدار على 64-بت: وسيط الحجم i64."""
    code, out, ir = _compile_ir(_TRIVIAL)
    assert code == 0, out
    assert _len_param_type(ir, fn) == "i64", f"@{fn} على 64-بت يجب أن يكون i64"


def _ret_type(ir: str, name: str) -> str:
    """(AR) نوع عائد الدالّة من سطر define (الرمز الذي يسبق ‎@الاسم‎)."""
    m = re.search(r"define\s+[^\n]*?(\S+)\s+@" + re.escape(name) + r"\s*\(", ir)
    assert m, f"IR بلا تعريف @{name}"
    return m.group(1)


def test_strlen_returns_size_t():
    """(AR) عائد strlen = size_t الهدف: i32 على i686، i64 على المضيف."""
    _, _, ir32 = _compile_ir(_TRIVIAL, I686)
    _, _, ir64 = _compile_ir(_TRIVIAL)
    assert _ret_type(ir32, "strlen") == "i32", (
        "strlen على i686 يعيد i64 — المستدعي يقرأ edx قمامةً في النصف الأعلى"
    )
    assert _ret_type(ir64, "strlen") == "i64", "انحدار: strlen على 64-بت"


@pytest.mark.parametrize("fn", _MEM_FNS)
def test_mem_length_is_i64_on_host_64bit(fn):
    """(AR) على الهدف المضيف (64-بت): الطول i64 — لا انحدار في المسار القائم."""
    code, out, ir = _compile_ir(_TRIVIAL)
    assert code == 0, out
    assert _len_param_type(ir, fn) == "i64", f"@{fn} على 64-بت يجب أن يكون طوله i64"


# ───── عرض بيانات حلقة الكلمة ≠ نوع الطول ─────
# (AR) فخّ رصدته المراجعة: استعمال نوع الطول نفسه لحمل/خزن حلقة الكلمة المضاعفة
#      يجعلها تنسخ 4 بايت بخطوة 8 على i686 (فساد صامت أخطر من العيب الأصليّ)،
#      ويجعل إزاحات نمط التعبئة 32..56 في memset ‎poison‎ (shl بعدد ≥ عرض النوع).

def _body(ir: str, name: str) -> str:
    m = _define_re(name).search(ir)
    assert m, f"IR بلا تعريف @{name}"
    end = ir.index("\n}", m.start())
    return ir[m.start():end]


def test_word_loop_payload_stays_i64_on_i686():
    """(AR) على i686: حلقة الكلمة في memcpy/memset تحمل/تخزن i64 لا i32."""
    code, out, ir = _compile_ir(_TRIVIAL, I686)
    assert code == 0, out
    assert "load i64, ptr %srcq" in _body(ir, "memcpy"), (
        "memcpy على i686 يحمل الكلمة بعرض الطول (i32) بخطوة 8 بايت "
        "⇒ ينسخ 4 من كلّ 8 ويترك الباقي قمامة"
    )
    assert "store i64" in _body(ir, "memset"), (
        "memset على i686 يخزن نمط التعبئة بعرض الطول (i32) بخطوة 8 بايت"
    )


def test_no_poison_shift_in_memset():
    """(AR) لا `shl iN` بعدد ≥ N في memset (يُنتج poison ⇒ تعبئة غير معرَّفة)."""
    for flags in ((I686,), ()):
        code, out, ir = _compile_ir(_TRIVIAL, *flags)
        assert code == 0, out
        for m in re.finditer(r"shl i(\d+) %\S+, (\d+)", _body(ir, "memset")):
            width, amount = int(m.group(1)), int(m.group(2))
            assert amount < width, (
                f"إزاحة poison في memset (هدف {flags or 'المضيف'}): "
                f"shl i{width} بمقدار {amount} ≥ عرض النوع"
            )


# ───── مسار النصوص/الذاكرة في كود المستخدم يتّسق مع التعريف الحرّ ─────
# (AR) مواقع استدعاء memcpy/memset في مولّدات النصوص والمصفوفات وأوامر الذاكرة
#      كانت تصرّح الطول i64 ثابتًا. مع تصحيح التعريف إلى size_t صار التوقيعان
#      متعارضين ⇒ جسر `memcpy.freestanding` بوسيط i64 ⇒ **فشل verifyModule**.
#      هذه الحالات تترجم فعليًّا برامج تسلك تلك المسارات على i686.

_STRING_CONCAT = 'متغير أ = "سلام"\nمتغير ب = أ + " عليكم"\n'
_STRING_SLICE = 'متغير أ = "سلام عليكم"\nمتغير ب = أ.جزء(0، 4)\nمتغير ج = أ.استبدل("سلام"، "أهلًا")\n'
_ARRAY_OPS = 'متغير م = [1، 2، 3]\nم.أضف(4)\nمتغير ش = م.شريحة(1، 3)\n'
# (AR) عنوان صريح — لا تخصيص: الاختبار يولّد IR فقط (لا يُشغَّل).
_MEM_BUILTIN = 'عبئ_ذاكرة_س(1048576، 0، 64)\nانسخ_ذاكرة_س(1048576، 2097152، 64)\n'

# (AR) نوع وسيط الطول في **سطر النداء** (لا سطر التعريف).
_CALL_HEAD_RE = re.compile(
    r"call [^\n]*@(memcpy|memmove|memset|memcmp|malloc|realloc|calloc)\("
)


def _mem_call_args(ir: str):
    """(AR) يُنتج (اسم الدالّة، وسائط النداء) لكلّ نداء mem*.

    (AR) لا يكفي regex بسيط: وسيط قد يحوي أقواسًا متداخلة
         (‎inttoptr (i64 N to ptr)‎)، فنمسح حتى القوس المُطابق ونقسم
         على الفواصل في العمق صفر.
    """
    for m in _CALL_HEAD_RE.finditer(ir):
        i, depth, start = m.end(), 1, m.end()
        while i < len(ir) and depth:
            depth += (ir[i] == "(") - (ir[i] == ")")
            i += 1
        inner = ir[start:i - 1]
        args, depth, last = [], 0, 0
        for j, ch in enumerate(inner):
            depth += (ch == "(") - (ch == ")")
            if ch == "," and depth == 0:
                args.append(inner[last:j])
                last = j + 1
        args.append(inner[last:])
        yield m.group(1), [a.strip() for a in args], ir[m.start():i]


@pytest.mark.parametrize(
    "src",
    [_STRING_CONCAT, _STRING_SLICE, _ARRAY_OPS, _MEM_BUILTIN],
    ids=["دمج_نصوص", "جزء_واستبدل", "مصفوفات", "مدمجات_ذاكرة"],
)
def test_user_mem_call_sites_use_size_t_on_i686(src):
    """(AR) كلّ **موقع استدعاء** لـmem* في كود المستخدم يمرّر الطول i32 على i686.

    (AR) ⚠️ لا يكفي ‎code == 0‎: مع المؤشّرات المعتّمة لا يقارن ‎verifyModule‎
         نوع وسيط النداء بتوقيع الدالّة حين لا يلزم ‎bitcast‎، وقد يمرّ
         ‎getOrInsertFunction‎ صامتًا إن سبق التعريفُ موقعَ الاستدعاء. لذا
         نفحص أنواع الوسائط في سطور ‎call‎ نفسها. (رصدته مراجعة أميليا
         بالمسخ: إعادة العيب أبقت 81 اختبارًا خضراء.)
    """
    code, out, ir = _compile_ir(src, I686)
    assert code == 0, "الترجمة الحرّة لـi686 فشلت:\n" + out
    assert ".freestanding" not in ir, (
        "أُنشئ جسر ‎*.freestanding‎ ⇒ تعارض توقيع بين موقع الاستدعاء والتعريف:\n"
        + "\n".join(l for l in ir.splitlines() if ".freestanding" in l)
    )

    calls = list(_mem_call_args(ir))
    assert calls, "لم يُرصد أيّ نداء mem* — الحارس بلا تغطية لهذا المصدر"
    for name, args, text in calls:
        len_ty = args[-1].split()[0]
        assert len_ty == "i32", (
            f"نداء @{name} على i686 يمرّر الطول {len_ty} لا i32 ⇒ "
            f"عدم تطابق مع التعريف (size_t) ⇒ حجم قمامة:\n  {text}"
        )


# ───── دوالّ size_t المستضافة (qsort/strspn/strncmp) على i686 ─────
# (AR) هذه محجوبة حرًّا فتُختبر مستضافةً. رصدت الجولة الثالثة أنّ qsort على
#      i686 المستضاف تتعطّل (SIGSEGV) لأنّ خانتَي size/compar تُزاحان حين
#      nmemb/size معلَنان i64. الحارس يبني للهدفين ويؤكّد i32 على 32-بت
#      وسلامة الوحدة (لا Call parameter type mismatch).

I686_HOSTED = "i686-unknown-linux-gnu"
X64_HOSTED = "x86_64-unknown-linux-gnu"

# (AR) لكلّ حالة: الدالّة المصرَّحة، والنصّ الذي يُطابَق ضدّه (الوسائط لـqsort/
#      strncmp حيث الحجم وسيط، والتصريح كاملًا لـstrspn حيث الحجم عائد).
def _decl(ir: str, fn: str) -> str:
    m = re.search(r"declare [^\n]*@" + re.escape(fn) + r"\([^)]*\)", ir)
    return m.group(0) if m else ""


_HOSTED_CASES = [
    ("رتب", 'متغير م = [3، 1، 2]\nم.رتب()\nاطبع(م)\n', "qsort"),
    ("قص", 'متغير ن = "  سلام  "\nاطبع(ن.قص())\n', "strspn"),
    ("يبدأ_بـ", 'متغير ن = "سلام عليكم"\nاطبع(ن.يبدأ_بـ("سلام"))\n', "strncmp"),
]


@pytest.mark.parametrize("label,src,fn", _HOSTED_CASES, ids=[c[0] for c in _HOSTED_CASES])
def test_hosted_size_t_fn_i686_verifies(label, src, fn):
    """(AR) الوحدة المستضافة لـi686 تجتاز verifyModule؛ ولا i64 في تصريح الدالّة.

    (AR) i64 في أيّ موضع من التصريح (وسيطًا كان أو عائدًا) على 32-بت ⇒ إزاحة
         خانات أو نصف أعلى قمامة. qsort مُثبَتة العطب (SIGSEGV) في الجولة ٣.
    """
    code, out, ir = _compile_ir_hosted(src, I686_HOSTED)
    assert code == 0, (
        f"i686 المستضاف فشل لـ{label} — غالبًا عدم تطابق نوع في نداء @{fn} "
        f"(size_t=i32 مقابل i64):\n{out}"
    )
    decl = _decl(ir, fn)
    assert decl, f"لم يُصرَّح @{fn} في IR المستضاف"
    assert "i64" not in decl, (
        f"@{fn} على i686 المستضاف ما زال يحوي i64 ⇒ عطب ABI size_t:\n  {decl}"
    )


@pytest.mark.parametrize("label,src,fn", _HOSTED_CASES, ids=[c[0] for c in _HOSTED_CASES])
def test_hosted_size_t_fn_x64_unchanged(label, src, fn):
    """(AR) لا انحدار: على x86-64 المستضاف التصريح يحوي i64 كما كان."""
    code, out, ir = _compile_ir_hosted(src, X64_HOSTED)
    assert code == 0, out
    decl = _decl(ir, fn)
    assert decl and "i64" in decl, f"@{fn} على x86-64 يجب أن يبقى i64:\n  {decl}"
