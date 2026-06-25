#!/usr/bin/env python3
# ======================================================================
# x.py — منسّق البناء الموحَّد للغة ص / Unified build orchestrator
# ======================================================================
# (AR) ينفّذ المرحلة 0 من RFC «قلب موحَّد بحدود داخلية صارمة» (sadlang-rfcs#10):
#      البوّابة الوحيدة للبناء. يبني المفسّر (sad-run) والمترجم (sad-build)
#      **ذرّيًّا في نفس التهيئة**، ثم يثبّت الثنائيَّين في `dist/<Config>/` مع
#      بصمة SHA-256 لكلٍّ منهما. هذا يقتل عثرتين موثَّقتين:
#        1. تذبذب Debug/Release (مفسّر بتهيئة ومترجم بأخرى) — مستحيل الآن لأن
#           الأمر الواحد يبني الهدفين معًا لكل تهيئة، وإن فشل أحدهما فشل الكل.
#        2. الثنائيّ البائت — `verify` يعيد حساب البصمة فيكشف أيّ مخرَج قديم.
#
# (EN) Implements Phase 0 of the RFC "unified core with strict internal
#      boundaries" (sadlang-rfcs#10): the single build gateway. Builds the
#      interpreter (sad-run) and compiler (sad-build) ATOMICALLY in the SAME
#      configuration, then stages both binaries into `dist/<Config>/` with a
#      SHA-256 fingerprint each. Kills two documented gotchas: Debug/Release
#      drift and the stale-binary trap.
#
# الاستخدام / Usage:
#   python x.py build --config Debug,Release   # بناء الهدفين معًا لكل تهيئة
#   python x.py verify --config Debug          # تحقّق من تطابق dist والبصمات
#   python x.py test --config Debug -- --level P0   # بناء (إن لزم) ثم runner
#   python x.py conformance                    # فاحص مطابقة القواعد القائم
#   python x.py clean                          # حذف build/ و dist/
# ======================================================================

from __future__ import annotations

import argparse
import hashlib
import json
import platform
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

# (AR) ضمان إخراج عربيّ سليم على طرفية ويندوز (cp1252 افتراضًا) وأنابيب CI.
# (EN) Ensure correct Arabic output on Windows consoles (cp1252 default) and CI pipes.
for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8", errors="replace")  # type: ignore[union-attr]
    except (AttributeError, ValueError):
        pass

# ──────────────────────────────────────────────────────────────────────
# ثوابت المشروع / Project constants
# ──────────────────────────────────────────────────────────────────────
ROOT = Path(__file__).resolve().parent
BUILD_DIR = ROOT / "build"
DIST_DIR = ROOT / "dist"

# (AR) الهدفان اللذان يجب أن يُبنيا معًا دائمًا — قلب ضمان التطابق السلوكيّ.
# (EN) The two targets that must ALWAYS build together — the heart of the guard.
ENGINES = {
    "sad-run": "المفسّر / interpreter",
    "sad-build": "المترجم / compiler",
}

EXE = ".exe" if platform.system() == "Windows" else ""
VALID_CONFIGS = ("Debug", "Release", "RelWithDebInfo", "MinSizeRel")


# ──────────────────────────────────────────────────────────────────────
# أدوات مساعدة / Helpers
# ──────────────────────────────────────────────────────────────────────
def _log(msg: str) -> None:
    print(f"[x.py] {msg}", flush=True)


def _fail(msg: str) -> "NoReturn":  # type: ignore[name-defined]
    print(f"[x.py] ✗ خطأ / error: {msg}", file=sys.stderr, flush=True)
    sys.exit(1)


def _run(cmd: list[str]) -> None:
    """(AR) ينفّذ أمرًا ويفشل بصوتٍ عالٍ. / Run a command, fail loudly."""
    _log("» " + " ".join(str(c) for c in cmd))
    result = subprocess.run(cmd, cwd=ROOT)
    if result.returncode != 0:
        _fail(f"الأمر فشل برمز {result.returncode}: {' '.join(map(str, cmd))}")


def _sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def _parse_configs(raw: str) -> list[str]:
    configs = [c.strip() for c in raw.split(",") if c.strip()]
    if not configs:
        _fail("لم تُحدَّد أيّ تهيئة / no configuration given")
    for c in configs:
        if c not in VALID_CONFIGS:
            _fail(f"تهيئة غير معروفة / unknown configuration: {c} "
                  f"(المتاح / valid: {', '.join(VALID_CONFIGS)})")
    return configs


def _is_multi_config() -> bool:
    """(AR) هل المولِّد متعدّد التهيئات (مثل Visual Studio)؟ يُقرأ من CMakeCache.
    (EN) Is this a multi-config generator (e.g. Visual Studio)?"""
    cache = BUILD_DIR / "CMakeCache.txt"
    if not cache.exists():
        # (AR) على ويندوز الافتراضي VS متعدد التهيئات. / Default on Windows is VS.
        return platform.system() == "Windows"
    for line in cache.read_text(encoding="utf-8", errors="ignore").splitlines():
        if line.startswith("CMAKE_CONFIGURATION_TYPES"):
            return True
    return False


def _generator() -> str:
    cache = BUILD_DIR / "CMakeCache.txt"
    if cache.exists():
        for line in cache.read_text(encoding="utf-8", errors="ignore").splitlines():
            if line.startswith("CMAKE_GENERATOR:"):
                return line.split("=", 1)[1].strip()
    return "(default)"


def _find_binary(name: str, config: str) -> Path | None:
    """(AR) يحدّد موقع المخرَج بعد البناء عبر المرشّحات المعروفة.
    (EN) Locate the produced binary across known candidate locations."""
    candidates = [
        BUILD_DIR / "bin" / config / f"{name}{EXE}",  # multi-config (VS)
        BUILD_DIR / "bin" / f"{name}{EXE}",            # single-config
        BUILD_DIR / config / "bin" / f"{name}{EXE}",
    ]
    for c in candidates:
        if c.exists():
            return c
    return None


# ──────────────────────────────────────────────────────────────────────
# الأوامر / Commands
# ──────────────────────────────────────────────────────────────────────
def cmd_configure(args: argparse.Namespace) -> None:
    """(AR) تهيئة CMake إن لم تكن موجودة. / Configure CMake if needed."""
    _configure_if_needed()


def _configure_if_needed() -> None:
    if (BUILD_DIR / "CMakeCache.txt").exists():
        return
    _log("تهيئة CMake لأول مرة / first-time CMake configure …")
    _run(["cmake", "-S", str(ROOT), "-B", str(BUILD_DIR)])


def _stage(config: str) -> dict:
    """(AR) ينسخ الثنائيَّين إلى dist/<config>/ ويكتب manifest بالبصمات.
    (EN) Copy both engines to dist/<config>/ and write a fingerprint manifest."""
    dest = DIST_DIR / config
    dest.mkdir(parents=True, exist_ok=True)
    manifest = {
        "config": config,
        "generated_utc": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "generator": _generator(),
        "binaries": {},
    }
    for name in ENGINES:
        src = _find_binary(name, config)
        if src is None:
            _fail(f"لم يُعثر على مخرَج الهدف / target output not found: "
                  f"{name} ({config}). هل اكتمل البناء؟ / did the build complete?")
        out = dest / f"{name}{EXE}"
        shutil.copy2(src, out)
        manifest["binaries"][name] = {
            "file": out.name,
            "sha256": _sha256(out),
            "size": out.stat().st_size,
            "source": str(src.relative_to(ROOT)).replace("\\", "/"),
        }
    (dest / "manifest.json").write_text(
        json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )
    return manifest


def cmd_build(args: argparse.Namespace) -> None:
    configs = _parse_configs(args.config)

    if args.clean and BUILD_DIR.exists():
        _log("حذف مجلد البناء / removing build dir …")
        shutil.rmtree(BUILD_DIR, ignore_errors=True)

    _configure_if_needed()
    multi = _is_multi_config()

    for config in configs:
        _log(f"━━ بناء التهيئة / building config: {config} "
             f"(المحرّكان معًا / both engines together) ━━")
        # (AR) جوهر المرحلة 0: أمر بناء واحد يضمّ الهدفين. إن فشل أحدهما، فشل
        #      الأمر كلّه — فلا يخرج زوجٌ نصفُ مبنيّ بتهيئتين مختلفتين أبدًا.
        # (EN) Phase-0 core: one build command spanning BOTH targets. If either
        #      fails the whole command fails — never a half-built mismatched pair.
        build_cmd = ["cmake", "--build", str(BUILD_DIR)]
        if multi:
            build_cmd += ["--config", config]
        for target in ENGINES:
            build_cmd += ["--target", target]
        _run(build_cmd)

        manifest = _stage(config)
        _log(f"✓ {config}: ثُبّت المحرّكان في / staged engines to dist/{config}/")
        for name, info in manifest["binaries"].items():
            _log(f"    {name:<10} sha256={info['sha256'][:16]}…  ({info['size']} bytes)")

    _log("✓ اكتمل البناء الذرّيّ / atomic build complete.")


def cmd_verify(args: argparse.Namespace) -> None:
    """(AR) حارس البوّابة G1: يؤكّد أن dist يحوي زوجًا متطابق التهيئة وأن
        البصمات تطابق الملفات الفعلية (كشف الثنائيّ البائت).
    (EN) The G1 gate guard: assert dist holds a same-config pair and that
        fingerprints match the actual files (stale-binary detection)."""
    if args.all:
        if not DIST_DIR.exists():
            _fail("لا يوجد dist/ — شغّل build أولًا / no dist/, run build first.")
        configs = sorted(p.name for p in DIST_DIR.iterdir() if p.is_dir())
        if not configs:
            _fail("dist/ فارغ / dist/ is empty.")
    else:
        configs = _parse_configs(args.config)

    for config in configs:
        manifest_path = DIST_DIR / config / "manifest.json"
        if not manifest_path.exists():
            _fail(f"لا manifest للتهيئة / no manifest for config: {config} "
                  f"— شغّل: python x.py build --config {config}")
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

        if manifest.get("config") != config:
            _fail(f"تضارب تهيئة في manifest / manifest config mismatch: "
                  f"المجلد {config} يحوي تهيئة {manifest.get('config')}")

        missing = [n for n in ENGINES if n not in manifest.get("binaries", {})]
        if missing:
            _fail(f"محرّكات ناقصة في / engines missing in {config}: {', '.join(missing)}")

        for name, info in manifest["binaries"].items():
            out = DIST_DIR / config / info["file"]
            if not out.exists():
                _fail(f"ثنائيّ مفقود / missing binary: {out}")
            actual = _sha256(out)
            if actual != info["sha256"]:
                _fail(f"بصمة بائتة / stale fingerprint: {name} ({config}) — "
                      f"الملف على القرص لا يطابق manifest. أعد البناء.")
        _log(f"✓ {config}: زوج متطابق التهيئة، بصمات سليمة / consistent pair, fingerprints OK.")

    _log("✓ التحقّق نجح / verification passed.")


def cmd_test(args: argparse.Namespace) -> None:
    configs = _parse_configs(args.config)
    if len(configs) != 1:
        _fail("test يقبل تهيئة واحدة فقط / test accepts exactly one config.")
    config = configs[0]

    # (AR) ابنِ المحرّكين معًا ثم تحقّق قبل تشغيل الاختبارات.
    # (EN) Build both engines together, then verify, before running tests.
    build_args = argparse.Namespace(config=config, clean=False)
    cmd_build(build_args)
    cmd_verify(argparse.Namespace(config=config, all=False))

    runner = ROOT / "tests" / "runner.py"
    if not runner.exists():
        _fail(f"runner غير موجود / runner not found: {runner}")
    cmd = [sys.executable, str(runner), *args.runner_args]
    _log("تشغيل الـrunner / running test runner …")
    result = subprocess.run(cmd, cwd=ROOT)
    sys.exit(result.returncode)


def cmd_conformance(args: argparse.Namespace) -> None:
    """(AR) تمرير رفيع إلى فاحص مطابقة القواعد القائم (يصبح أعمق في مرحلة لاحقة).
    (EN) Thin pass-through to the existing grammar-conformance checker."""
    checker = ROOT / "scripts" / "codegen" / "check_grammar_conformance.py"
    if not checker.exists():
        _fail(f"الفاحص غير موجود / checker not found: {checker}")
    cmd = [sys.executable, str(checker), *args.checker_args]
    result = subprocess.run(cmd, cwd=ROOT)
    sys.exit(result.returncode)


def cmd_clean(args: argparse.Namespace) -> None:
    for d in (BUILD_DIR, DIST_DIR):
        if d.exists():
            _log(f"حذف / removing {d.name}/ …")
            shutil.rmtree(d, ignore_errors=True)
    _log("✓ تم التنظيف / cleaned.")


# ──────────────────────────────────────────────────────────────────────
# واجهة الأوامر / CLI
# ──────────────────────────────────────────────────────────────────────
def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="x.py",
        description="منسّق البناء الموحَّد للغة ص / Sad Lang unified build orchestrator",
    )
    sub = p.add_subparsers(dest="command", required=True)

    pb = sub.add_parser("build", help="بناء المحرّكين معًا لكل تهيئة / build both engines per config")
    pb.add_argument("--config", default="Debug",
                    help="قائمة تهيئات مفصولة بفواصل / comma-separated configs (Debug,Release)")
    pb.add_argument("--clean", action="store_true", help="حذف build/ قبل البناء / wipe build/ first")
    pb.set_defaults(func=cmd_build)

    pv = sub.add_parser("verify", help="تحقّق من تطابق dist والبصمات / verify dist pair & fingerprints")
    pv.add_argument("--config", default="Debug", help="تهيئات للتحقّق / configs to verify")
    pv.add_argument("--all", action="store_true", help="تحقّق من كل تهيئات dist/ / verify all dist configs")
    pv.set_defaults(func=cmd_verify)

    pt = sub.add_parser("test", help="بناء + تحقّق + تشغيل runner / build + verify + run runner")
    pt.add_argument("--config", default="Debug", help="تهيئة واحدة / single config")
    pt.add_argument("runner_args", nargs=argparse.REMAINDER,
                    help="وسائط تُمرَّر للـrunner بعد -- / args passed to runner after --")
    pt.set_defaults(func=cmd_test)

    pc = sub.add_parser("configure", help="تهيئة CMake / configure CMake")
    pc.set_defaults(func=cmd_configure)

    pf = sub.add_parser("conformance", help="فاحص مطابقة القواعد / grammar-conformance checker")
    pf.add_argument("checker_args", nargs=argparse.REMAINDER,
                    help="وسائط تُمرَّر للفاحص بعد -- / args passed to the checker after --")
    pf.set_defaults(func=cmd_conformance)

    pcl = sub.add_parser("clean", help="حذف build/ و dist/ / remove build & dist")
    pcl.set_defaults(func=cmd_clean)

    return p


def main() -> None:
    args = build_parser().parse_args()
    # (AR) إزالة `--` الفاصل إن مرّره argparse ضمن REMAINDER.
    # (EN) Strip a leading `--` separator left by argparse in REMAINDER.
    for attr in ("runner_args", "checker_args"):
        vals = getattr(args, attr, None)
        if vals and vals and vals[0] == "--":
            setattr(args, attr, vals[1:])
    args.func(args)


if __name__ == "__main__":
    main()
