#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) بوّابةُ وصلِ الخلفيّة الأصليّة بالمُشغِّل — «--خلفية-أصلية» مسارٌ حيّ لا مِخبَريّ.

     كانت مخفّضاتُ SIR ⇒ شيفرةِ الآلة مُبرهَنةً باختبارات وحدةٍ تستدعي الهيدرات
     مباشرةً؛ فكان بالإمكان أن تخضرَّ كلُّها بينما لا يبلغُها مستخدِمٌ واحدٌ من
     سطر الأوامر. هذه البوّابةُ تُغلق تلك الفجوة: تُشغّل ثنائيَّ sad-build نفسَه
     على مصدرِ ص، وتؤكّد أمرين — أنّ المدعومَ يُنتِج ELF64 صحيحَ البنية بالمعماريّة
     المطلوبة، وأنّ غيرَ المدعوم يُرفَض صراحةً لا يُبتلَع صامتًا.

     لا تُنفَّذ الثنائيّاتُ هنا: مخرَجُنا ELF لينكس، والبوّابةُ تعمل على ويندوز
     أيضًا. برهانُ التشغيلِ الحيّ مكانُه scripts/native_backend/prove_*.sh.

(EN) Gate proving the native backend is wired into the driver — "--خلفية-أصلية"
     is a live path, not a lab one. Runs the real sad-build binary on ص source
     and asserts both directions: supported targets yield a well-formed ELF64 for
     the requested machine, and unsupported ones are rejected outright rather
     than silently swallowed. Binaries are not executed here (Linux ELF, gate
     also runs on Windows); live-run proof lives in scripts/native_backend/.
"""
import argparse
import platform
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

# (AR) مخرَجٌ عربيّ على أيّ طرفيّة: ترميزُ وحدةِ تحكّمِ ويندوز قد يكون صفحةً
#      محلّيّةً ترفض العربيّة والرموز، فتنفجر البوّابة عند الطباعة لا عند
#      الحكم — فيُقرَأ الإخفاقُ خطأً على أنّه إخفاقُ خلفيّة.
# (EN) Arabic output on any terminal: a Windows console codepage may reject
#      Arabic/symbols, blowing up on print rather than on judgement.
for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8", errors="replace")
    except (AttributeError, ValueError):
        pass

# (AR) العَلَمُ القانونيّ من مصدر الحقيقة language-truth/cli_flags.yaml (flag.native_backend).
FLAG_NATIVE = "--خلفية-أصلية"
FLAG_TARGET = "--هدف"

# (AR) ثوابتُ ELF المُرآة في compiler/include/backend/native/elf64_writer.h.
ELF_MAGIC = b"\x7fELF"
ELFCLASS64 = 2
ELFDATA2LSB = 1
ET_EXEC = 2
EM_X86_64 = 62
EM_AARCH64 = 183
# (AR) إزاحةُ الشيفرة = رأس ELF (64) + program header (56) — kCodeOffset في الكاتب.
CODE_OFFSET = 120

# (AR) مصدرُ ص: «دالة رئيسية() ارجع 42 نهاية» — أبسطُ برنامجٍ يبلغ RET بقيمةٍ ثابتة.
SOURCE = "دالة رئيسية()\n    ارجع 42\nنهاية\n"

# (AR) شظايا تشخيصٍ من كتالوج أخطاء SoT (INT020 هدفٌ غير مدعوم، INT021 نوعُ مخرَجٍ
#      متعارض). مطابقتُها تميّز رفضًا مقصودًا من إخفاقٍ عارضٍ برمزٍ غير صفريّ —
#      خطأِ تحليلٍ مثلًا — فلا تخضرّ حالةُ الرفض للسبب الخطأ.
REJECT_TARGET = "غير مدعوم"
REJECT_OUTPUT = "متعارض"

# (AR) هدفٌ مدعومٌ دائمًا — يُمرّر حين يُراد عزلُ حارسٍ آخر عن حارسِ الهدف.
ELF_TARGET = FLAG_TARGET + "=x86_64-unknown-linux-gnu"

# (AR) هل حاويةُ المضيف ELF؟ تُقرّر ما يفعله الهدفُ الافتراضيّ (بلا «--هدف»).
HOST_IS_ELF = sys.platform.startswith("linux")


def host_machine() -> int:
    """(AR) e_machine المتوقَّع دونَ «--هدف»: الثالوثُ يصير ثالوثَ المضيف، فالمعماريّةُ
    معماريّتُه. تثبيتُ 62 هنا كان يُفشل البوّابةَ على عدّاءِ ARM64 (أپل سيليكون)."""
    m = platform.machine().lower()
    return EM_AARCH64 if m in ("arm64", "aarch64") else EM_X86_64


def parse_elf64(blob: bytes):
    """(AR) يفكّ رأسَ ELF64 إلى حقولٍ يُحكَم عليها؛ يرمي عند أيّ خللٍ بنيويّ."""
    if len(blob) <= CODE_OFFSET:
        raise ValueError(f"الملفّ أقصر من رأس ELF + program header ({len(blob)} بايت)")
    if blob[:4] != ELF_MAGIC:
        raise ValueError(f"سحرُ ELF مفقود: {blob[:4]!r}")
    if blob[4] != ELFCLASS64:
        raise ValueError(f"ELFCLASS ليس 64 ({blob[4]})")
    if blob[5] != ELFDATA2LSB:
        raise ValueError(f"ترتيبُ البايتات ليس little-endian ({blob[5]})")
    e_type, e_machine = struct.unpack_from("<HH", blob, 16)
    e_entry, e_phoff = struct.unpack_from("<QQ", blob, 24)
    e_phnum = struct.unpack_from("<H", blob, 56)[0]
    return {
        "e_type": e_type,
        "e_machine": e_machine,
        "e_entry": e_entry,
        "e_phoff": e_phoff,
        "e_phnum": e_phnum,
        "size": len(blob),
    }


def run_compiler(compiler: Path, src: Path, out: Path, extra_args):
    """(AR) يشغّل sad-build بالعَلَم الأصليّ؛ يعيد (رمزَ الخروج، المخرَجَ النصّيّ)."""
    cmd = [str(compiler), str(src), FLAG_NATIVE, "-o", str(out)] + list(extra_args)
    proc = subprocess.run(cmd, capture_output=True, text=True, encoding="utf-8",
                          errors="replace", timeout=120)
    return proc.returncode, (proc.stdout or "") + (proc.stderr or "")


def prepare(workdir: Path, name: str, prefix: str):
    """(AR) مصدرٌ طازج ومخرَجٌ محذوف. مخرَجٌ بائتٌ من حالةٍ سابقة يجعل «المخرَجُ ELF
    سليم» خُضرةً كاذبة: يُحكَم على ملفٍّ لم تكتبه هذه التشغيلة."""
    src = workdir / "طرف.ص"
    src.write_text(SOURCE, encoding="utf-8")
    out = workdir / f"{prefix}_{name}"
    if out.exists():
        out.unlink()
    return src, out


def check_accepts(compiler: Path, workdir: Path, name: str, target, want_machine: int):
    """(AR) حالةُ قبول: ترجمةٌ بالعَلَم ثمّ حكمٌ على بنية ELF ومعماريّته."""
    src, out = prepare(workdir, name, "out")
    extra = [f"{FLAG_TARGET}={target}"] if target else []

    code, log = run_compiler(compiler, src, out, extra)
    if code != 0:
        return False, f"[{name}] فشلت الترجمة (رمز {code}) | {log.strip()[:400]}"
    if not out.exists():
        return False, f"[{name}] لم يُكتب المخرَج: {out} | {log.strip()[:400]}"

    try:
        hdr = parse_elf64(out.read_bytes())
    except ValueError as exc:
        return False, f"[{name}] مخرَجٌ ليس ELF64 سليمًا: {exc}"

    if hdr["e_type"] != ET_EXEC:
        return False, f"[{name}] e_type={hdr['e_type']} والمتوقَّع ET_EXEC({ET_EXEC})"
    if hdr["e_machine"] != want_machine:
        return False, (f"[{name}] e_machine={hdr['e_machine']} "
                       f"والمتوقَّع {want_machine} — الهدفُ لم يصل إلى الكاتب")
    if hdr["e_phnum"] != 1:
        return False, f"[{name}] e_phnum={hdr['e_phnum']} والمتوقَّع 1"
    if hdr["e_entry"] % 0x1000 != CODE_OFFSET:
        return False, (f"[{name}] نقطةُ الدخول {hdr['e_entry']:#x} لا تقع عند "
                       f"إزاحةِ الشيفرة ({CODE_OFFSET:#x}) داخل الصفحة")
    if hdr["size"] <= CODE_OFFSET:
        return False, f"[{name}] لا شيفرةَ بعد الرؤوس ({hdr['size']} بايت)"

    return True, (f"[{name}] ELF64 ET_EXEC e_machine={hdr['e_machine']} "
                  f"دخول={hdr['e_entry']:#x} حجم={hdr['size']} بايت")


def check_rejects(compiler: Path, workdir: Path, name: str, extra_args, want_fragment: str):
    """(AR) حالةُ رفض: العَلَمُ مع خيارٍ لا تنتجه الخلفيّةُ يجب أن يُخفِق برمزٍ غير
    صفريّ، بلا مخرَجٍ متروك، وبتشخيصٍ من كتالوج SoT. القبولُ الصامتُ هنا هو العيبُ
    بعينه: ثنائيٌّ تنفيذيّ باسمِ ملفِّ كائن، أو ELF لهدفٍ لا يقرؤه."""
    src, out = prepare(workdir, name, "rej")

    code, log = run_compiler(compiler, src, out, extra_args)
    if code == 0:
        return False, f"[{name}] قُبِل صامتًا (رمز 0) والمتوقَّع رفضٌ صريح"
    if out.exists():
        return False, f"[{name}] رُفِض لكنّه خلّف مخرَجًا: {out}"
    if want_fragment not in log:
        return False, (f"[{name}] رُفِض بتشخيصٍ لا يطابق المتوقَّع "
                       f"(«{want_fragment}» غائب) | {log.strip()[:400]}")
    return True, f"[{name}] رُفِض برمز {code} وتشخيصٍ من كتالوج SoT"


def main() -> int:
    ap = argparse.ArgumentParser(description="بوّابة وصل الخلفيّة الأصليّة بالمُشغِّل")
    ap.add_argument("--compiler", required=True, type=Path, help="مسار sad-build")
    args = ap.parse_args()

    if not args.compiler.exists():
        print(f"✗ المترجم غير موجود: {args.compiler}", file=sys.stderr)
        return 1

    # (AR) أهدافٌ صريحة: كلٌّ يجب أن يُنتج ELF بمعماريّته هو.
    accepts = [
        ("x86_64_linux", "x86_64-unknown-linux-gnu", EM_X86_64),
        # (AR) الثالوثُ AArch64 ⇒ المخفّضُ الآخر وe_machine آخر — برهانُ أنّ
        #      «--هدف» يبلغ مفترقَ المعماريّة لا أنّه يُبتلَع صامتًا.
        ("aarch64_linux", "aarch64-unknown-linux-gnu", EM_AARCH64),
        # (AR) الوضعُ الحرّ (بلا نظام) — الحالةُ التي وُلدت لها هذه الخلفيّة.
        ("x86_64_freestanding", "x86_64-unknown-none-elf", EM_X86_64),
    ]

    # (AR) حالاتُ الرفض: الحارسانِ الصريحان. بلا هذين كان المترجمُ يكتب ELF
    #      تنفيذيًّا لهدفِ ويندوز/wasm ولنوعِ مخرَجٍ لا ينتجه — كلاهما برمز 0.
    rejects = [
        ("target_windows", [f"{FLAG_TARGET}=x86_64-pc-windows-msvc"], REJECT_TARGET),
        ("target_wasm", [f"{FLAG_TARGET}=wasm32-unknown-unknown"], REJECT_TARGET),
        # (AR) هدفٌ مدعومٌ صراحةً كي يكون المرفوضُ نوعَ المخرَج لا الهدف: بلا «--هدف»
        #      يسبق حارسُ الهدفِ على مضيفٍ غيرِ ELF فتخضرّ الحالةُ للسبب الخطأ.
        ("output_object", [ELF_TARGET, "-c"], REJECT_OUTPUT),
        ("output_llvm_ir", [ELF_TARGET, "--أظهر-llvm"], REJECT_OUTPUT),
    ]

    # (AR) الهدفُ الافتراضيّ (بلا «--هدف») = ثالوثُ المضيف. على مضيفٍ ELF يجب أن
    #      ينجح بمعماريّة المضيف؛ وعلى ويندوز/ماك يجب أن يُرفَض صراحةً — فالخلفيّةُ
    #      لا تكتب PE ولا Mach-O. كتابةُ ELF باسم t.exe صامتةً هي العيبُ الذي
    #      يحرسه هذا الفرع.
    if HOST_IS_ELF:
        accepts.append(("host_default", None, host_machine()))
    else:
        rejects.append(("host_default_rejected", [], REJECT_TARGET))

    total = len(accepts) + len(rejects)
    failures = []
    with tempfile.TemporaryDirectory(prefix="sad_native_gate_") as tmp:
        workdir = Path(tmp)
        for name, target, machine in accepts:
            ok, msg = check_accepts(args.compiler, workdir, name, target, machine)
            print(("✓ " if ok else "✗ ") + msg)
            if not ok:
                failures.append(name)
        for name, extra, fragment in rejects:
            ok, msg = check_rejects(args.compiler, workdir, name, extra, fragment)
            print(("✓ " if ok else "✗ ") + msg)
            if not ok:
                failures.append(name)

    if failures:
        print(f"\n✗ بوّابةُ الخلفيّة الأصليّة: أخفقت {len(failures)}/{total} "
              f"({'، '.join(failures)})", file=sys.stderr)
        return 1

    print(f"\n✓ بوّابةُ الخلفيّة الأصليّة: {total}/{total} — «{FLAG_NATIVE}» موصولٌ "
          f"من سطر الأوامر إلى بايتات ELF بلا LLVM، ويرفض ما لا يُنتجه")
    return 0


if __name__ == "__main__":
    sys.exit(main())
