#!/usr/bin/env python3
# بسم الله الرحمن الرحيم
# =============================================================================
# (AR) برهانُ **التصريفِ** للخلفيّةِ الأصليّة — بصمةٌ بايتيّةٌ مستقلّةٌ عن المضيف
# (EN) The native backend's *emission* proof — a host-independent byte fingerprint
# =============================================================================
# (AR) هذا البرهانُ يقيسُ الدرجةَ الأولى من درجتَي «تعملُ على كلِّ الأنظمة»:
#
#        ① التصريفُ (emit)  — كاتبُ ELF64 يكتبُ بايتاتٍ، والبايتاتُ لا تعرفُ
#          مضيفَها. فهو صالحٌ على المنصّاتِ الثلاثِ وفي التكوينَين، ويُقاسُ هنا.
#        ② التنفيذُ (run)   — يحتاجُ نظامًا يقرأُ ELF. يبقى على لينكسَ (وqemu
#          لـAArch64 وRV64)، ويقيسُه `run_native_proofs.sh`.
#
#      وخلطُ الدرجتَينِ هو مكمنُ الأخضرِ الكاذب: خطوةٌ تُسمّى «براهينُ الخلفيّةِ
#      الأصليّة» على ماك ولا تقيسُ إلّا وجودَ الملفِّ أسوأُ من غيابِها.
#
#      **ما يحرسُه هذا الملفُّ تحديدًا:** أنّ صورةَ ELF المُخرَجةَ لمصدرٍ بعينِه
#      وهدفٍ بعينِه **واحدةٌ بايتًا بايتًا على المنصّاتِ الثلاث**. وهي دعوى
#      قابلةٌ للكسرِ فعلًا: أيُّ اعتمادٍ على ترتيبِ جدولِ تجزئةٍ، أو على حجمِ
#      `size_t` المضيف، أو على نهاياتِ الأسطرِ، أو على مسارٍ يتسرّبُ إلى الصورة
#      — يكسرُها. ولا يكشفُ ذلك اختبارٌ يقيسُ «هل خرجَ بـ٤٢» لأنّه يمرُّ بصورٍ
#      مختلفةٍ تعطي النتيجةَ نفسَها.
#
#      ⚠️ **السجلُّ سِقّاطةٌ ثنائيّةُ الاتّجاه**: بصمةٌ تخالفُ المُسجَّلَ ⇒ أحمر،
#      وبصمةٌ مسجَّلةٌ لمدخلٍ لم يعد يُقاس ⇒ أحمر أيضًا. فالسطرُ البائتُ في
#      السجلِّ يُقرأ تغطيةً وهو ليس منها. والتحديثُ المتعمَّدُ بـ‎--سجّل‎.
#
# (EN) This proves degree ① of "works on every system": emission. The ELF writer
#      writes bytes, and bytes do not know their host, so it is valid on all
#      three platforms in both configurations and is measured here. Degree ② —
#      execution — needs an ELF-reading OS and stays on Linux (plus qemu for
#      AArch64/RV64), measured by run_native_proofs.sh. Conflating the two is
#      where false green lives: a step named "native backend proofs" on macOS
#      that only checks a file exists is worse than no step at all.
#
#      What this file guards: that the emitted ELF image for a given source and
#      target is *byte-identical across the three platforms*. That claim is
#      genuinely breakable — any dependence on hash-table ordering, on the host's
#      size_t, on line endings, or on a path leaking into the image breaks it —
#      and no "did it exit 42" test catches it, since different images can give
#      the same exit code.
#
#      The ledger is a two-way ratchet: a fingerprint that differs from the
#      record is red, and a recorded entry no longer measured is red too, since a
#      stale line reads as coverage it is not. Deliberate updates use --سجّل.
# =============================================================================

from __future__ import annotations

import argparse
import hashlib
import json
import os
import subprocess
import sys
import tempfile
from pathlib import Path

# (AR) هذا الملفُّ في scripts/native_backend/ — جذرُ المستودعِ أبواه.
# (EN) This file lives in scripts/native_backend/ — the repo root is two up.
REPO_ROOT = Path(__file__).resolve().parents[2]
LEDGER_PATH = Path(__file__).resolve().parent / "elf_emit_fingerprints.json"

# (AR) قِيَمُ e_machine في ELF — من المواصفةِ لا من تخمين.
# (EN) ELF e_machine values — from the specification, not a guess.
E_MACHINE_X86_64 = 0x3E
E_MACHINE_AARCH64 = 0xB7
E_MACHINE_RISCV = 0xF3

# (AR) ثوابتُ ترويسةِ ELF64 المقروءة هنا (إزاحةٌ، عرضٌ).
# (EN) The ELF64 header fields read here (offset, width).
EI_CLASS_OFFSET = 4
ELF_CLASS_64 = 2
EI_DATA_OFFSET = 5
ELF_DATA_LSB = 1
E_TYPE_OFFSET = 16
E_TYPE_EXEC = 2
E_MACHINE_OFFSET = 18
E_ENTRY_OFFSET = 24
ELF64_HEADER_SIZE = 64

# (AR) الأهدافُ المقيسة. الثالوثُ كاملٌ صراحةً — و«unknown-linux-elf» هو ما
#      تستعمله براهينُ التنفيذِ الحيِّ نفسُها، فالمقيسانِ هدفٌ واحد.
# (EN) The measured targets. Full explicit triples, matching what the live
#      execution proofs use, so both degrees measure the same target.
TARGETS = [
    ("x86_64", "x86_64-unknown-linux-elf", E_MACHINE_X86_64),
    ("aarch64", "aarch64-unknown-linux-elf", E_MACHINE_AARCH64),
    ("riscv64", "riscv64-unknown-linux-elf", E_MACHINE_RISCV),
]

# (AR) مستويا التحسين. ليسا تزيُّدًا: عيبٌ حقيقيٌّ سابقٌ في RV64 كان يمرُّ في -O2
#      ويُخفِق في -O0 لأنّ DCE كان يحذفُ التعليمةَ غيرَ المُخفَّضة — أي أنّ
#      «مُخفَّض» كانت مشروطةً بتشغيلِ المُحسِّنِ دون أن يقولَ ذلك أحد.
# (EN) Both optimization levels. Not padding: a real RV64 defect used to pass at
#      -O2 and fail at -O0 because DCE deleted the un-lowered instruction.
OPT_LEVELS = ["-O0", "-O2"]

# (AR) مجموعةُ المصادر. صغيرةٌ عمدًا وثابتةُ الشكل: المقيسُ **حتميّةُ الإخراجِ عبرَ
#      المضيفين** لا سعةُ اللغة. وكلُّ برنامجٍ يُكتَبُ ببادئةِ BOM كما تفعلُ بقيّةُ
#      البراهين، ونهاياتُ أسطرِه ‎\n‎ صراحةً — فنهايةُ السطرِ لو تسرّبت إلى الصورة
#      لصارت البصمةُ تابعةً لنظامِ الملفّاتِ لا للمُخفِّض.
# (EN) The source corpus. Deliberately small and uniform: what is measured is
#      cross-host determinism of emission, not language coverage. Each program is
#      written with a BOM (as the other proofs do) and explicit \n line endings,
#      so a line ending leaking into the image would be a fingerprint that tracks
#      the filesystem rather than the lowerer.
PROGRAMS = {
    "decl_print": "متغير س = 42\nاطبع_سطر(س)\n",
    "decl_unsigned": "متغير طبيعي س = 18446744073709551615\nاطبع_سطر(س)\n",
    "decl_bool": "متغير س = صحيح\nاطبع_سطر(س)\n",
}


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="برهانُ البصمةِ البايتيّةِ لصورةِ ELF من الخلفيّةِ الأصليّة",
    )
    parser.add_argument(
        "--مترجم", dest="compiler", required=True,
        help="مسارُ sad-build-native (أو sad-build — كلاهما يُخرِج الصورةَ نفسَها)",
    )
    parser.add_argument(
        "--سجّل", dest="record", action="store_true",
        help="يُعيدُ كتابةَ سجلِّ البصماتِ بدل مقارنتِه (تحديثٌ متعمَّد)",
    )
    parser.add_argument(
        "--مجلد-العمل", dest="work_dir", default=None,
        help="مجلَّدُ الملفّاتِ الوسيطة (افتراضُه مجلَّدٌ مؤقَّتٌ يُحذَف)",
    )
    return parser.parse_args()


def read_u16_le(data: bytes, offset: int) -> int:
    return int.from_bytes(data[offset:offset + 2], "little")


def read_u64_le(data: bytes, offset: int) -> int:
    return int.from_bytes(data[offset:offset + 8], "little")


def check_elf_structure(image: bytes, expected_machine: int) -> list[str]:
    """(AR) ثوابتُ البنية — تُقاسُ دائمًا حتّى حين يُعادُ تسجيلُ البصمة.
    (EN) Structural invariants — always measured, even when re-recording."""
    problems: list[str] = []

    if len(image) < ELF64_HEADER_SIZE:
        problems.append(f"الصورةُ أقصرُ من ترويسةِ ELF64 ({len(image)} بايت)")
        return problems

    if image[:4] != b"\x7fELF":
        problems.append("لا توقيعَ ELF في أوّلِ الصورة")
        return problems

    if image[EI_CLASS_OFFSET] != ELF_CLASS_64:
        problems.append(f"EI_CLASS={image[EI_CLASS_OFFSET]} لا {ELF_CLASS_64} (64-بت)")

    if image[EI_DATA_OFFSET] != ELF_DATA_LSB:
        problems.append(f"EI_DATA={image[EI_DATA_OFFSET]} لا {ELF_DATA_LSB} (little-endian)")

    e_type = read_u16_le(image, E_TYPE_OFFSET)
    if e_type != E_TYPE_EXEC:
        problems.append(f"e_type={e_type} لا {E_TYPE_EXEC} (ET_EXEC)")

    e_machine = read_u16_le(image, E_MACHINE_OFFSET)
    if e_machine != expected_machine:
        problems.append(
            f"e_machine={e_machine:#x} والمتوقَّع {expected_machine:#x} — "
            "صورةٌ لمعماريّةٍ غيرِ المطلوبة"
        )

    # (AR) نقطةُ دخولٍ صفرٌ تعني صورةً لا تُقلِع، وتمرُّ من كلِّ فحصٍ بنيويٍّ آخر.
    # (EN) A zero entry point means an image that cannot start, and it passes
    #      every other structural check.
    if read_u64_le(image, E_ENTRY_OFFSET) == 0:
        problems.append("e_entry=0 — صورةٌ بلا نقطةِ دخول")

    return problems


def emit_one(compiler: Path, work_dir: Path, program_name: str, source: str,
             target_name: str, triple: str, opt: str) -> tuple[bytes | None, str]:
    """(AR) يُصرِّفُ برنامجًا واحدًا لهدفٍ واحد، ويعيدُ بايتاتِ الصورةِ أو سببَ الفشل."""
    stem = f"{program_name}_{target_name}{opt}"
    source_path = work_dir / f"{stem}.ص"
    output_path = work_dir / f"{stem}.elf"

    # (AR) BOM ثمّ المصدر، وبنمطٍ ثنائيٍّ كي لا يترجمَ المضيفُ نهاياتِ الأسطر —
    #      فالمقيسُ حتميّةُ الإخراجِ، ولا يجوزُ أن يقرّرَها نظامُ الملفّات.
    # (EN) BOM then source, in binary mode so the host cannot translate line
    #      endings — determinism is what is measured; the filesystem must not
    #      get a vote.
    with open(source_path, "wb") as handle:
        handle.write(b"\xef\xbb\xbf")
        handle.write(source.encode("utf-8"))

    command = [
        str(compiler), str(source_path),
        "--خلفية-أصلية", f"--هدف={triple}", opt,
        "-o", str(output_path),
    ]
    completed = subprocess.run(
        command, capture_output=True, text=True, encoding="utf-8", errors="replace",
    )

    if completed.returncode != 0 or not output_path.exists():
        detail = (completed.stderr or completed.stdout or "").strip()
        return None, f"لم يُنتَج ثنائيّ (رمز {completed.returncode}): {detail[:400]}"

    return output_path.read_bytes(), ""


def main() -> int:
    arguments = parse_arguments()
    compiler = Path(arguments.compiler).resolve()
    if not compiler.exists():
        print(f"FAIL: لا مترجمَ عند {compiler}", file=sys.stderr)
        return 2

    recorded: dict[str, str] = {}
    if LEDGER_PATH.exists():
        recorded = json.loads(LEDGER_PATH.read_text(encoding="utf-8"))

    measured: dict[str, str] = {}
    failures: list[str] = []
    measured_count = 0

    with tempfile.TemporaryDirectory() as temporary:
        work_dir = Path(arguments.work_dir) if arguments.work_dir else Path(temporary)
        work_dir.mkdir(parents=True, exist_ok=True)

        for program_name, source in PROGRAMS.items():
            for target_name, triple, expected_machine in TARGETS:
                for opt in OPT_LEVELS:
                    key = f"{program_name}|{target_name}|{opt}"
                    measured_count += 1

                    image, error = emit_one(
                        compiler, work_dir, program_name, source,
                        target_name, triple, opt,
                    )
                    if image is None:
                        failures.append(f"{key}: {error}")
                        continue

                    problems = check_elf_structure(image, expected_machine)
                    if problems:
                        failures.append(f"{key}: " + " · ".join(problems))
                        continue

                    digest = hashlib.sha256(image).hexdigest()
                    measured[key] = digest

                    if not arguments.record:
                        if key not in recorded:
                            failures.append(
                                f"{key}: مدخلٌ غيرُ مسجَّل — بصمتُه {digest[:16]}… "
                                "(سجِّله بـ--سجّل بعد مراجعته)"
                            )
                        elif recorded[key] != digest:
                            failures.append(
                                f"{key}: البصمةُ تغيّرت — المسجَّل "
                                f"{recorded[key][:16]}… والمقيس {digest[:16]}…"
                            )

    # (AR) الاتّجاهُ الثاني للسِقّاطة: مدخلٌ مسجَّلٌ لم يُقَس في هذا الشوط.
    # (EN) The ratchet's second direction: a recorded entry not measured this run.
    if not arguments.record:
        for key in sorted(set(recorded) - set(measured)):
            failures.append(f"{key}: مسجَّلٌ ولم يُقَس — سطرٌ بائتٌ يُقرأ تغطيةً")

    if arguments.record:
        LEDGER_PATH.write_text(
            json.dumps(measured, indent=2, sort_keys=True, ensure_ascii=False) + "\n",
            encoding="utf-8",
        )
        print(f"سُجِّلت {len(measured)} بصمةً في {LEDGER_PATH.name}")
        if failures:
            for failure in failures:
                print(f"  ❌ {failure}")
            return 1
        return 0

    print(f"── بصماتُ صورةِ ELF: {len(measured)}/{measured_count} مقيسةً ──")
    if failures:
        for failure in failures:
            print(f"  ❌ {failure}")
        print(f"FAIL: {len(failures)} خللًا في بصماتِ التصريفِ الأصليّ")
        return 1

    print("PASS: كلُّ صورةٍ مطابقةٌ للمسجَّلِ بايتًا بايتًا وبنيتُها صحيحة")
    return 0


if __name__ == "__main__":
    sys.exit(main())
