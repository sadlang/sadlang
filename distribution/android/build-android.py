# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════
# تطبيق مُثبّت لغة ص — Android
# Sad Language Installer for Android
# ═══════════════════════════════════════════════════════════════════
"""
بناء تطبيق مُثبّت لغة ص كحزمة APK لأجهزة أندرويد.

الاستخدام:
    python build-android.py
    python build-android.py --release
    python build-android.py --arch arm64-v8a
    python build-android.py --arch "arm64-v8a,x86_64"
"""

import os
import sys
import json
import shutil
import hashlib
import argparse
import subprocess
from pathlib import Path

SCRIPT_DIR  = Path(__file__).parent
REPO_ROOT   = SCRIPT_DIR.parent.parent
ANDROID_DIR = REPO_ROOT / "tools" / "android"
OUTPUT_DIR  = SCRIPT_DIR.parent / "output" / "android"
APP_DIR     = SCRIPT_DIR / "sad-installer-app"
BUILD_APK   = ANDROID_DIR / "build_apk.py"

# ── ألوان الطباعة
CYAN   = "\033[96m"
GREEN  = "\033[92m"
YELLOW = "\033[93m"
RED    = "\033[91m"
RESET  = "\033[0m"

def banner():
    print(f"""
{CYAN}╔═══════════════════════════════════════════════════╗
║     لغة ص — بناء APK للأندرويد                  ║
║     Sad Language Android APK Builder             ║
╚═══════════════════════════════════════════════════╝{RESET}""")

def info (m): print(f"  {CYAN}[>>]{RESET} {m}")
def ok   (m): print(f"  {GREEN}[OK]{RESET} {m}")
def warn (m): print(f"  {YELLOW}[!!]{RESET} {m}")
def error(m): print(f"  {RED}[XX]{RESET} {m}", file=sys.stderr)


def ensure_app_dir(version: str) -> Path:
    """إنشاء/تحديث مجلد تطبيق المُثبّت"""
    APP_DIR.mkdir(parents=True, exist_ok=True)

    # ملف main.ص — واجهة المُثبّت
    main_sad = APP_DIR / "main.ص"
    main_sad.write_text(f'''\
# تطبيق مُثبّت لغة ص لأندرويد
# الإصدار: {version}

متغير الإصدار = "{version}"
متغير الدليل_الافتراضي = "/data/local/sad-lang"

دالة عرض_الترحيب()
    اطبع_سطر("╔═══════════════════════════════════╗")
    اطبع_سطر("║   مُثبّت لغة ص v" + الإصدار + "       ║")
    اطبع_سطر("║   Sad Language Installer          ║")
    اطبع_سطر("╚═══════════════════════════════════╝")
    اطبع_سطر("")
    اطبع_سطر("مرحباً بك في لغة ص!")
    اطبع_سطر("لغة برمجة عربية حديثة لأجهزة أندرويد")
نهاية

دالة عرض_الميزات()
    اطبع_سطر("")
    اطبع_سطر("الميزات المتاحة في هذا الإصدار:")
    اطبع_سطر("  ● مفسّر لغة ص الكامل")
    اطبع_سطر("  ● بيئة REPL تفاعلية")
    اطبع_سطر("  ● محرر كود مدمج بتمييز الصياغة")
    اطبع_سطر("  ● المكتبة القياسية الكاملة")
    اطبع_سطر("  ● دعم RTL العربي الكامل")
    اطبع_سطر("  ● تشغيل ملفات .ص مباشرة")
نهاية

دالة رئيسية()
    عرض_الترحيب()
    عرض_الميزات()
    اطبع_سطر("")
    اطبع_سطر("جاري التهيئة...")
    اطبع_سطر("✓ تم بنجاح!")
نهاية
''', encoding='utf-8')

    # ملف تكوين المشروع
    config = {
        "app_name": "لغة ص",
        "app_name_en": "Sad Language",
        "package_name": "org.sadlang.interpreter",
        "version_code": 1,
        "version_name": version,
        "min_sdk": 21,
        "target_sdk": 34,
        "main_file": "main.ص",
        "permissions": [
            "INTERNET",
            "WRITE_EXTERNAL_STORAGE",
            "READ_EXTERNAL_STORAGE"
        ],
        "features": ["compiler", "editor", "stdlib"]
    }
    (APP_DIR / "project.json").write_text(
        json.dumps(config, ensure_ascii=False, indent=2),
        encoding='utf-8'
    )
    return APP_DIR


def build_via_sad_builder(args) -> bool:
    """البناء عبر build_apk.py الموجود في tools/android"""
    if not BUILD_APK.exists():
        warn(f"build_apk.py غير موجود: {BUILD_APK}")
        return False

    cmd = [
        sys.executable, str(BUILD_APK),
        "--project", str(APP_DIR),
        "--output", str(OUTPUT_DIR),
    ]
    if args.release:
        cmd.append("--release")

    info("تشغيل باني APK...")
    info(f"الأمر: {' '.join(cmd)}")

    result = subprocess.run(cmd, capture_output=False, text=True)
    return result.returncode == 0


def build_gradle_apk(args) -> bool:
    """بناء APK عبر Gradle (إذا توفر Android SDK)"""
    android_sdk = (
        os.environ.get("ANDROID_SDK_ROOT") or
        os.environ.get("ANDROID_HOME") or
        str(Path.home() / "Android" / "Sdk")
    )

    gradle_wrapper = ANDROID_DIR / "gradlew"
    gradlew_bat    = ANDROID_DIR / "gradlew.bat"

    if os.name == 'nt' and gradlew_bat.exists():
        gradle_cmd = str(gradlew_bat)
    elif gradle_wrapper.exists():
        gradle_cmd = str(gradle_wrapper)
    else:
        warn("Gradle wrapper غير موجود")
        return False

    build_task = f":app:assemble{'Release' if args.release else 'Debug'}"
    cmd = [gradle_cmd, build_task, f"-PsadVersion={args.version}"]

    info(f"Gradle build: {build_task}")
    result = subprocess.run(cmd, cwd=str(ANDROID_DIR), capture_output=False, text=True)
    if result.returncode != 0:
        error("فشل Gradle build")
        return False

    # نقل APK للخرج
    apk_pattern = "Release" if args.release else "Debug"
    for apk in (ANDROID_DIR / "app" / "build" / "outputs" / "apk").rglob("*.apk"):
        dest = OUTPUT_DIR / f"sad-lang-{args.version}-android-{apk.stem}.apk"
        shutil.copy2(apk, dest)
        ok(f"تم نقل: {dest.name}")

    return True


def create_placeholder(args) -> Path:
    """إنشاء placeholder عند غياب Android SDK"""
    note_file = OUTPUT_DIR / f"sad-lang-{args.version}-android.README.txt"
    note_file.write_text(
        f"لبناء APK لغة ص لأجهزة أندرويد:\n\n"
        f"1. ثبّت Android Studio أو Android SDK\n"
        f"2. ثبّت Android NDK r25+\n"
        f"3. اضبط: ANDROID_SDK_ROOT أو ANDROID_HOME\n"
        f"4. شغّل: python distribution/android/build-android.py --release\n\n"
        f"أو انتظر حزمة APK الرسمية من GitHub Releases:\n"
        f"https://github.com/sad-lang/sad-language/releases/tag/v{args.version}\n\n"
        f"البنيات المدعومة:\n"
        f"  arm64-v8a  (أجهزة حديثة — أفضل أداء)\n"
        f"  armeabi-v7a (أجهزة قديمة)\n"
        f"  x86_64     (محاكي أندرويد)\n",
        encoding='utf-8'
    )
    warn(f"تم إنشاء: {note_file.name}")
    warn("يتطلب بناء APK حقيقي: Android NDK + SDK")
    return note_file


def sha256_file(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def build_apk(args):
    banner()
    info(f"الإصدار: {args.version} | البنية: {args.arch} | {'Release' if args.release else 'Debug'}")

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    ensure_app_dir(args.version)

    success = False

    # محاولة 1: build_apk.py
    if BUILD_APK.exists():
        info("محاولة البناء عبر أداة build_apk.py...")
        success = build_via_sad_builder(args)

    # محاولة 2: Gradle
    if not success:
        info("محاولة البناء عبر Gradle...")
        success = build_gradle_apk(args)

    # محاولة 3: placeholder
    if not success:
        note = create_placeholder(args)
        return

    # البحث عن APKs وحساب checksums
    apk_files = list(OUTPUT_DIR.glob("**/*.apk"))
    if apk_files:
        print("")
        ok("الحزم المبنية:")
        for apk in apk_files:
            size_mb = apk.stat().st_size / (1024 * 1024)
            h = sha256_file(apk)
            ok(f"  {apk.name} ({size_mb:.1f} MB)")
            ok(f"  SHA256: {h[:32]}...")
            (apk.parent / f"{apk.name}.sha256").write_text(
                f"{h}  {apk.name}\n", encoding='utf-8')

    print(f"\n  مجلد الخرج: {OUTPUT_DIR}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="بناء APK مُثبّت لغة ص لأجهزة أندرويد")
    parser.add_argument("--version", default="1.0.0",
                        help="رقم الإصدار (مثال: 1.0.0)")
    parser.add_argument("--arch", default="arm64-v8a,x86_64,armeabi-v7a",
                        help="البنيات المستهدفة (مفصولة بفاصلة)")
    parser.add_argument("--release", action="store_true",
                        help="بناء إصدار الإنتاج (Release)")
    build_apk(parser.parse_args())
