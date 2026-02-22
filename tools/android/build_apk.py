#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
═══════════════════════════════════════════════════════════════════════════════
أداة بناء تطبيقات أندرويد بلغة ص
═══════════════════════════════════════════════════════════════════════════════

هذه الأداة تقوم ببناء ملف APK من مشروع ص

الاستخدام:
    python build_apk.py --project <مسار_المشروع> --output <مسار_الخرج>
    python build_apk.py --project ./my_app --release
    
═══════════════════════════════════════════════════════════════════════════════
"""

import os
import sys
import json
import shutil
import argparse
import subprocess
from pathlib import Path
from typing import Optional, Dict, Any


class SadApkBuilder:
    """باني تطبيقات أندرويد من كود ص"""
    
    def __init__(self, project_path: str, output_path: str = None, release: bool = False):
        self.project_path = Path(project_path).resolve()
        self.output_path = Path(output_path).resolve() if output_path else self.project_path / "build"
        self.release = release
        self.build_dir = self.output_path / "android_build"
        
        # مسارات SDK
        self.android_sdk = self._find_android_sdk()
        self.android_ndk = self._find_android_ndk()
        
        # إعدادات افتراضية
        self.config = {
            "app_name": "تطبيق ص",
            "app_name_en": "Sad App",
            "package_name": "com.sad.app",
            "version_code": 1,
            "version_name": "1.0.0",
            "min_sdk": 21,
            "target_sdk": 34,
            "main_file": "main.ص",
            "icon": None,
            "permissions": []
        }
    
    def _find_android_sdk(self) -> Optional[Path]:
        """البحث عن Android SDK"""
        # المسارات الشائعة
        paths = [
            os.environ.get("ANDROID_SDK_ROOT"),
            os.environ.get("ANDROID_HOME"),
            Path.home() / "Android" / "Sdk",
            Path.home() / "AppData" / "Local" / "Android" / "Sdk",  # Windows
            Path("/usr/local/android-sdk"),
        ]
        
        for p in paths:
            if p and Path(p).exists():
                return Path(p)
        
        return None
    
    def _find_android_ndk(self) -> Optional[Path]:
        """البحث عن Android NDK"""
        if self.android_sdk:
            ndk_dir = self.android_sdk / "ndk"
            if ndk_dir.exists():
                # أحدث إصدار
                versions = sorted(ndk_dir.iterdir(), reverse=True)
                if versions:
                    return versions[0]
        
        # من متغيرات البيئة
        ndk_path = os.environ.get("ANDROID_NDK_HOME") or os.environ.get("NDK_HOME")
        if ndk_path and Path(ndk_path).exists():
            return Path(ndk_path)
        
        return None
    
    def load_project_config(self) -> bool:
        """تحميل إعدادات المشروع"""
        config_file = self.project_path / "تطبيق.json"
        if not config_file.exists():
            config_file = self.project_path / "app.json"
        
        if config_file.exists():
            try:
                with open(config_file, "r", encoding="utf-8") as f:
                    user_config = json.load(f)
                    self.config.update(user_config)
                print(f"✓ تم تحميل إعدادات المشروع من {config_file.name}")
                return True
            except Exception as e:
                print(f"✗ خطأ في قراءة الإعدادات: {e}")
                return False
        else:
            print("⚠ لم يتم العثور على ملف إعدادات، سيتم استخدام الإعدادات الافتراضية")
            return True
    
    def validate_project(self) -> bool:
        """التحقق من صحة المشروع"""
        print("\n── التحقق من المشروع ──")
        
        # التحقق من وجود الملف الرئيسي
        main_file = self.project_path / self.config["main_file"]
        if not main_file.exists():
            print(f"✗ الملف الرئيسي غير موجود: {main_file}")
            return False
        print(f"✓ الملف الرئيسي: {main_file.name}")
        
        # التحقق من SDK
        if not self.android_sdk:
            print("✗ لم يتم العثور على Android SDK")
            print("  قم بتعيين ANDROID_SDK_ROOT أو ANDROID_HOME")
            return False
        print(f"✓ Android SDK: {self.android_sdk}")
        
        if not self.android_ndk:
            print("✗ لم يتم العثور على Android NDK")
            return False
        print(f"✓ Android NDK: {self.android_ndk}")
        
        return True
    
    def setup_build_directory(self) -> bool:
        """إعداد مجلد البناء"""
        print("\n── إعداد مجلد البناء ──")
        
        try:
            # إنشاء المجلد
            if self.build_dir.exists():
                shutil.rmtree(self.build_dir)
            self.build_dir.mkdir(parents=True)
            
            # هيكل مشروع أندرويد
            dirs = [
                "app/src/main/java/sad/android/app",
                "app/src/main/res/layout",
                "app/src/main/res/values",
                "app/src/main/res/mipmap-hdpi",
                "app/src/main/res/mipmap-mdpi",
                "app/src/main/res/mipmap-xhdpi",
                "app/src/main/res/mipmap-xxhdpi",
                "app/src/main/assets/sad",
                "app/src/main/jniLibs/arm64-v8a",
                "app/src/main/jniLibs/armeabi-v7a",
                "app/src/main/jniLibs/x86_64",
            ]
            
            for d in dirs:
                (self.build_dir / d).mkdir(parents=True, exist_ok=True)
            
            print(f"✓ تم إنشاء مجلد البناء: {self.build_dir}")
            return True
            
        except Exception as e:
            print(f"✗ خطأ في إنشاء مجلد البناء: {e}")
            return False
    
    def copy_sad_files(self) -> bool:
        """نسخ ملفات ص إلى assets"""
        print("\n── نسخ ملفات ص ──")
        
        assets_dir = self.build_dir / "app/src/main/assets/sad"
        
        try:
            # نسخ جميع ملفات .ص
            count = 0
            for sad_file in self.project_path.rglob("*.ص"):
                rel_path = sad_file.relative_to(self.project_path)
                dest = assets_dir / rel_path
                dest.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(sad_file, dest)
                count += 1
            
            print(f"✓ تم نسخ {count} ملف ص")
            return True
            
        except Exception as e:
            print(f"✗ خطأ في نسخ الملفات: {e}")
            return False
    
    def generate_android_manifest(self) -> bool:
        """إنشاء AndroidManifest.xml"""
        print("\n── إنشاء AndroidManifest.xml ──")
        
        permissions_xml = ""
        for perm in self.config.get("permissions", []):
            perm_name = self._translate_permission(perm)
            permissions_xml += f'    <uses-permission android:name="{perm_name}" />\n'
        
        manifest = f'''<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="{self.config['package_name']}">

{permissions_xml}
    <application
        android:allowBackup="true"
        android:icon="@mipmap/ic_launcher"
        android:label="{self.config['app_name']}"
        android:roundIcon="@mipmap/ic_launcher_round"
        android:supportsRtl="true"
        android:theme="@style/Theme.SadApp">
        
        <activity
            android:name=".MainActivity"
            android:exported="true"
            android:configChanges="orientation|screenSize|keyboard|keyboardHidden"
            android:windowSoftInputMode="adjustResize">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
        
    </application>

</manifest>
'''
        
        manifest_path = self.build_dir / "app/src/main/AndroidManifest.xml"
        manifest_path.write_text(manifest, encoding="utf-8")
        print("✓ تم إنشاء AndroidManifest.xml")
        return True
    
    def _translate_permission(self, perm: str) -> str:
        """ترجمة أسماء الصلاحيات"""
        mapping = {
            "الإنترنت": "android.permission.INTERNET",
            "الموقع": "android.permission.ACCESS_FINE_LOCATION",
            "الكاميرا": "android.permission.CAMERA",
            "التخزين": "android.permission.WRITE_EXTERNAL_STORAGE",
            "الهاتف": "android.permission.CALL_PHONE",
            "جهات_الاتصال": "android.permission.READ_CONTACTS",
            "الميكروفون": "android.permission.RECORD_AUDIO",
            "الاهتزاز": "android.permission.VIBRATE",
        }
        return mapping.get(perm, f"android.permission.{perm.upper()}")
    
    def generate_main_activity(self) -> bool:
        """إنشاء MainActivity.kt"""
        print("\n── إنشاء MainActivity.kt ──")
        
        activity = f'''package {self.config['package_name']}

import sad.android.runtime.SadActivity

class MainActivity : SadActivity() {{
    override val mainFile: String = "{self.config['main_file']}"
}}
'''
        
        # تحويل package name إلى مسار
        package_path = self.config['package_name'].replace('.', '/')
        activity_dir = self.build_dir / f"app/src/main/java/{package_path}"
        activity_dir.mkdir(parents=True, exist_ok=True)
        
        (activity_dir / "MainActivity.kt").write_text(activity, encoding="utf-8")
        print("✓ تم إنشاء MainActivity.kt")
        return True
    
    def generate_build_gradle(self) -> bool:
        """إنشاء ملفات build.gradle"""
        print("\n── إنشاء ملفات Gradle ──")
        
        # Root build.gradle
        root_gradle = '''plugins {
    id 'com.android.application' version '8.1.0' apply false
    id 'org.jetbrains.kotlin.android' version '1.9.0' apply false
}
'''
        (self.build_dir / "build.gradle").write_text(root_gradle)
        
        # App build.gradle
        app_gradle = f'''plugins {{
    id 'com.android.application'
    id 'org.jetbrains.kotlin.android'
}}

android {{
    namespace '{self.config["package_name"]}'
    compileSdk {self.config["target_sdk"]}

    defaultConfig {{
        applicationId "{self.config["package_name"]}"
        minSdk {self.config["min_sdk"]}
        targetSdk {self.config["target_sdk"]}
        versionCode {self.config["version_code"]}
        versionName "{self.config["version_name"]}"
        
        ndk {{
            abiFilters 'arm64-v8a', 'armeabi-v7a', 'x86_64'
        }}
    }}

    buildTypes {{
        release {{
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android-optimize.txt')
        }}
    }}
    
    buildFeatures {{
        compose true
    }}
    
    composeOptions {{
        kotlinCompilerExtensionVersion '1.5.1'
    }}
}}

dependencies {{
    implementation 'androidx.core:core-ktx:1.12.0'
    implementation 'androidx.lifecycle:lifecycle-runtime-ktx:2.6.2'
    implementation 'androidx.activity:activity-compose:1.8.0'
    implementation platform('androidx.compose:compose-bom:2023.10.00')
    implementation 'androidx.compose.ui:ui'
    implementation 'androidx.compose.ui:ui-graphics'
    implementation 'androidx.compose.material3:material3'
}}
'''
        (self.build_dir / "app/build.gradle").write_text(app_gradle)
        
        # settings.gradle
        settings_gradle = f'''pluginManagement {{
    repositories {{
        google()
        mavenCentral()
        gradlePluginPortal()
    }}
}}
dependencyResolutionManagement {{
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {{
        google()
        mavenCentral()
    }}
}}
rootProject.name = "{self.config['app_name_en']}"
include ':app'
'''
        (self.build_dir / "settings.gradle").write_text(settings_gradle)
        
        # gradle.properties
        gradle_props = '''android.useAndroidX=true
kotlin.code.style=official
android.nonTransitiveRClass=true
'''
        (self.build_dir / "gradle.properties").write_text(gradle_props)
        
        print("✓ تم إنشاء ملفات Gradle")
        return True
    
    def generate_resources(self) -> bool:
        """إنشاء ملفات الموارد"""
        print("\n── إنشاء الموارد ──")
        
        # strings.xml
        strings = f'''<?xml version="1.0" encoding="utf-8"?>
<resources>
    <string name="app_name">{self.config["app_name"]}</string>
</resources>
'''
        (self.build_dir / "app/src/main/res/values/strings.xml").write_text(strings, encoding="utf-8")
        
        # themes.xml
        themes = '''<?xml version="1.0" encoding="utf-8"?>
<resources>
    <style name="Theme.SadApp" parent="android:Theme.Material.Light.NoActionBar">
        <item name="android:statusBarColor">#6200EE</item>
    </style>
</resources>
'''
        (self.build_dir / "app/src/main/res/values/themes.xml").write_text(themes, encoding="utf-8")
        
        print("✓ تم إنشاء الموارد")
        return True
    
    def build_native_library(self) -> bool:
        """بناء المكتبة الأصلية"""
        print("\n── بناء المكتبة الأصلية ──")
        
        # مسار مكتبة sad_android
        sad_android_path = Path(__file__).parent
        
        # TODO: بناء المكتبة باستخدام CMake و NDK
        # في الوقت الحالي، نفترض أن المكتبة مبنية مسبقاً
        
        print("⚠ يجب بناء المكتبة الأصلية يدوياً أو استخدام مكتبة مبنية مسبقاً")
        return True
    
    def run_gradle_build(self) -> bool:
        """تشغيل بناء Gradle"""
        print("\n── بناء APK ──")
        
        gradle_wrapper = self.build_dir / "gradlew"
        if not gradle_wrapper.exists():
            # تحميل Gradle Wrapper
            print("⚠ يجب إضافة Gradle Wrapper للمشروع")
            return False
        
        build_type = "assembleRelease" if self.release else "assembleDebug"
        
        try:
            result = subprocess.run(
                [str(gradle_wrapper), build_type],
                cwd=self.build_dir,
                capture_output=True,
                text=True
            )
            
            if result.returncode != 0:
                print(f"✗ فشل البناء: {result.stderr}")
                return False
            
            print("✓ تم بناء APK بنجاح")
            return True
            
        except Exception as e:
            print(f"✗ خطأ في تشغيل Gradle: {e}")
            return False
    
    def build(self) -> bool:
        """تنفيذ عملية البناء الكاملة"""
        print("═" * 60)
        print("   أداة بناء تطبيقات أندرويد بلغة ص   ")
        print("═" * 60)
        print(f"\nالمشروع: {self.project_path}")
        print(f"الخرج: {self.output_path}")
        print(f"النوع: {'إصدار' if self.release else 'تطوير'}")
        
        steps = [
            ("تحميل الإعدادات", self.load_project_config),
            ("التحقق من المشروع", self.validate_project),
            ("إعداد مجلد البناء", self.setup_build_directory),
            ("نسخ ملفات ص", self.copy_sad_files),
            ("إنشاء AndroidManifest", self.generate_android_manifest),
            ("إنشاء MainActivity", self.generate_main_activity),
            ("إنشاء ملفات Gradle", self.generate_build_gradle),
            ("إنشاء الموارد", self.generate_resources),
            ("بناء المكتبة الأصلية", self.build_native_library),
        ]
        
        for step_name, step_func in steps:
            if not step_func():
                print(f"\n✗ فشل في: {step_name}")
                return False
        
        print("\n" + "═" * 60)
        print("   ✓ تم إعداد المشروع بنجاح!   ")
        print("═" * 60)
        print(f"\nلبناء APK، افتح المشروع في Android Studio:")
        print(f"  {self.build_dir}")
        print("\nأو شغّل:")
        print(f"  cd {self.build_dir}")
        print("  ./gradlew assembleDebug")
        
        return True


def main():
    parser = argparse.ArgumentParser(
        description="أداة بناء تطبيقات أندرويد بلغة ص",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
أمثلة:
  %(prog)s --project ./my_app
  %(prog)s --project ./my_app --output ./build
  %(prog)s --project ./my_app --release
        """
    )
    
    parser.add_argument(
        "--project", "-p",
        required=True,
        help="مسار مشروع ص"
    )
    
    parser.add_argument(
        "--output", "-o",
        help="مسار الخرج (افتراضي: <project>/build)"
    )
    
    parser.add_argument(
        "--release", "-r",
        action="store_true",
        help="بناء إصدار (release) بدلاً من تطوير (debug)"
    )
    
    args = parser.parse_args()
    
    builder = SadApkBuilder(args.project, args.output, args.release)
    
    success = builder.build()
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
