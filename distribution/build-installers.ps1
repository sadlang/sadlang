# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════
# سكريبت بناء مُثبّتات لغة ص — جميع المنصات
# Build all Sad Language installers
# ═══════════════════════════════════════════════════════════════════════
#
# الاستخدام:
#   .\build-installers.ps1 -Platform all       # كل المنصات
#   .\build-installers.ps1 -Platform windows   # Windows فقط (.exe)
#   .\build-installers.ps1 -Platform linux     # Linux فقط (AppImage/deb/rpm)
#   .\build-installers.ps1 -Platform macos     # macOS فقط (.pkg/.dmg)
#   .\build-installers.ps1 -Platform android   # Android فقط (.apk)
#   .\build-installers.ps1 -Sign               # توقيع رقمي (يتطلب شهادة)
# ═══════════════════════════════════════════════════════════════════════

[CmdletBinding()]
param(
    [ValidateSet("all", "windows", "linux", "macos", "android")]
    [string]$Platform = "windows",

    [string]$Version = "1.0.0",
    [switch]$Sign,
    [switch]$Release,
    [string]$SignCert  = "",
    [string]$SignPass  = ""
)

$ErrorActionPreference = "Stop"
$ScriptDir  = $PSScriptRoot
$RepoRoot   = Split-Path $ScriptDir -Parent
$BuildDir   = Join-Path $RepoRoot "build"
$OutputDir  = Join-Path $ScriptDir "output"
$AssetsDir  = Join-Path $ScriptDir "assets"

$Config     = if ($Release) { "Release" } else { "Debug" }
$BinDir     = Join-Path $BuildDir "bin\$Config"

# ──────────────────────────────────────────────────────────────────────
# الألوان
# ──────────────────────────────────────────────────────────────────────
function Write-Banner {
    Write-Host ""
    Write-Host "  ╔═══════════════════════════════════════════════╗" -ForegroundColor Cyan
    Write-Host "  ║    لغة ص — بناء حزم التوزيع v$Version        ║" -ForegroundColor Cyan
    Write-Host "  ║    Sad Language Distribution Builder          ║" -ForegroundColor Cyan
    Write-Host "  ╚═══════════════════════════════════════════════╝" -ForegroundColor Cyan
    Write-Host ""
}
function Write-Step  { param($m) Write-Host "  [>>] " -ForegroundColor Blue   -NoNewline; Write-Host $m }
function Write-OK    { param($m) Write-Host "  [OK] " -ForegroundColor Green  -NoNewline; Write-Host $m }
function Write-Warn  { param($m) Write-Host "  [!!] " -ForegroundColor Yellow -NoNewline; Write-Host $m }
function Write-Error2{ param($m) Write-Host "  [XX] " -ForegroundColor Red    -NoNewline; Write-Host $m }

# ──────────────────────────────────────────────────────────────────────
# التحضير
# ──────────────────────────────────────────────────────────────────────
function Prepare-Output {
    Write-Step "إنشاء مجلد الخرج..."
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
    New-Item -ItemType Directory -Path $AssetsDir -Force | Out-Null

    # إنشاء أيقونة افتراضية إذا لم تكن موجودة
    $IconPath = Join-Path $AssetsDir "sad_icon.ico"
    if (-not (Test-Path $IconPath)) {
        Write-Warn "الأيقونة غير موجودة — سيتم استخدام أيقونة افتراضية"
        # نسخ من أي مصدر متاح
        $FallbackIcon = Join-Path $RepoRoot "tools\vscode-extension\images\sad-icon.png"
        if (Test-Path $FallbackIcon) {
            Copy-Item $FallbackIcon (Join-Path $AssetsDir "sad_icon.png") -Force
        }
    }

    # ملف إعداد افتراضي
    $ConfigFile = Join-Path $AssetsDir "sad.config.json"
    if (-not (Test-Path $ConfigFile)) {
        @{
            version     = $Version
            stdlib_path = "`$SAD_HOME/stdlib"
            encoding    = "utf-8"
            rtl         = $true
            repl = @{
                history_size = 1000
                prompt       = ">>> "
            }
        } | ConvertTo-Json -Depth 5 | Set-Content $ConfigFile -Encoding UTF8
    }

    Write-OK "مجلد الخرج: $OutputDir"
}

function Check-Binary { param($name, $required = $true)
    $path = Join-Path $BinDir $name
    if (Test-Path $path) {
        Write-OK "  موجود: $name"
        return $true
    } else {
        if ($required) { Write-Warn "  مفقود: $name (مطلوب)" }
        else            { Write-Warn "  مفقود: $name (اختياري — سيتم تخطيه)" }
        return $false
    }
}

# ══════════════════════════════════════════════════════════════════════
#  1. مُثبّت Windows (.exe) — InnoSetup
# ══════════════════════════════════════════════════════════════════════
function Build-WindowsInstaller {
    Write-Host ""
    Write-Host "  ┌── Windows Installer (.exe) ──────────────────────────┐" -ForegroundColor Cyan

    # البحث عن InnoSetup
    $ISSCompiler = $null
    $Candidates = @(
        "C:\Program Files (x86)\Inno Setup 6\ISCC.exe",
        "C:\Program Files\Inno Setup 6\ISCC.exe",
        (Get-Command ISCC.exe -ErrorAction SilentlyContinue)?.Source
    )
    foreach ($c in $Candidates) {
        if ($c -and (Test-Path $c)) { $ISSCompiler = $c; break }
    }

    if (-not $ISSCompiler) {
        Write-Warn "  InnoSetup غير مثبّت!"
        Write-Host ""
        Write-Host "  تثبيت InnoSetup:" -ForegroundColor Yellow
        Write-Host "  1. حمّل من: https://jrsoftware.org/isdl.php" -ForegroundColor Gray
        Write-Host "  2. ثبّت ملف innosetup-6.x.x.exe"             -ForegroundColor Gray
        Write-Host "  3. أضِف ملف العربي: download Arabic.isl"     -ForegroundColor Gray
        Write-Host ""

        # محاولة تحميل InnoSetup تلقائياً
        $WantInstall = Read-Host "  هل تريد تحميل InnoSetup تلقائياً؟ (y/n)"
        if ($WantInstall -eq 'y') {
            Write-Step "  جاري تحميل InnoSetup 6..."
            $InstallerUrl  = "https://jrsoftware.org/download.php/is.exe"
            $InstallerPath = "$env:TEMP\innosetup-installer.exe"
            Invoke-WebRequest -Uri $InstallerUrl -OutFile $InstallerPath -UseBasicParsing
            Write-Step "  تشغيل المثبّت..."
            Start-Process $InstallerPath "/silent" -Wait
            # البحث مرة أخرى
            foreach ($c in $Candidates) {
                if ($c -and (Test-Path $c)) { $ISSCompiler = $c; break }
            }
        }
        if (-not $ISSCompiler) {
            Write-Error2 "  تعذّر العثور على InnoSetup — تخطي بناء Windows Installer"
            return
        }
    }

    Write-OK "  InnoSetup: $ISSCompiler"

    # التحقق من الملفات
    Write-Step "  التحقق من الملفات التنفيذية..."
    $hasInterpreter = Check-Binary "sad.exe"    $true
    $hasCompiler    = Check-Binary "sadc.exe"   $false
    $hasLSP         = Check-Binary "sad-lsp.exe" $false
    $hasFmt         = Check-Binary "sad-fmt.exe" $false
    $hasPkg         = Check-Binary "sad-pkg.exe" $false
    $hasREPL        = Check-Binary "sad-repl.exe" $false
    $hasVSIX        = Test-Path (Join-Path $RepoRoot "tools\vscode-extension\sad-language-2.0.0.vsix")

    if (-not $hasInterpreter) {
        Write-Error2 "  المفسر sad.exe غير موجود!"
        Write-Host "  شغّل أولاً: cmake --build build --config $Config --target sad" -ForegroundColor Yellow
        return
    }

    # تعديل ملف .iss ليشير للمسارات الصحيحة
    $ISSFile = Join-Path $ScriptDir "installer\sad-setup.iss"
    Write-Step "  بناء المثبّت..."

    $Args = @(
        "/DMyOutputDir=$OutputDir",
        "/DMyBinDir=$BinDir",
        "/DMyRepoRoot=$RepoRoot",
        "/DMyVersion=$Version",
        $ISSFile
    )

    if ($Sign -and $SignCert) {
        $Args += "/Ssigntool=signtool sign /f $SignCert /p $SignPass `$f"
    }

    & $ISSCompiler $Args
    if ($LASTEXITCODE -ne 0) {
        Write-Error2 "  فشل بناء المثبّت (exit code: $LASTEXITCODE)"
        return
    }

    $OutFile = Join-Path $OutputDir "sad-setup-$Version-windows-x64.exe"
    if (Test-Path $OutFile) {
        $Size = [math]::Round((Get-Item $OutFile).Length / 1MB, 1)
        Write-OK "  تم البناء: sad-setup-$Version-windows-x64.exe ($Size MB)"
        # حساب SHA256
        $Hash = (Get-FileHash $OutFile -Algorithm SHA256).Hash
        "$Hash  sad-setup-$Version-windows-x64.exe" | Set-Content "$OutFile.sha256"
        Write-OK "  SHA256: $Hash"
    }

    Write-Host "  └─────────────────────────────────────────────────────┘" -ForegroundColor Cyan
}

# ══════════════════════════════════════════════════════════════════════
#  2. Linux Packages (AppImage + deb + rpm)
# ══════════════════════════════════════════════════════════════════════
function Build-LinuxPackages {
    Write-Host ""
    Write-Host "  ┌── Linux Packages (AppImage + deb + rpm) ────────────┐" -ForegroundColor Green

    $LinuxOutputDir = Join-Path $OutputDir "linux"
    New-Item -ItemType Directory -Path $LinuxOutputDir -Force | Out-Null

    # GitHub Actions يبني هذا على Linux runner
    # هنا ننشئ سكريبت bash للتنفيذ على Linux
    $buildScript = Join-Path $ScriptDir "linux\build-linux.sh"
    New-Item -ItemType Directory -Path (Split-Path $buildScript) -Force | Out-Null

    $bashContent = @'
#!/usr/bin/env bash
# ═══════════════════════════════════════════════
# بناء حزم Linux — AppImage + deb + rpm
# ═══════════════════════════════════════════════
set -euo pipefail

VERSION="${1:-1.0.0}"
ARCH="${2:-x86_64}"
BIN_DIR="${3:-../../build/bin/Release}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
OUTPUT_DIR="$SCRIPT_DIR/../output/linux"
mkdir -p "$OUTPUT_DIR"

echo "══════════════════════════════════════════"
echo "  لغة ص — بناء حزم Linux"
echo "  الإصدار: $VERSION  البنية: $ARCH"
echo "══════════════════════════════════════════"

# ─── التحقق من الملفات ───
check_bin() { [ -f "$BIN_DIR/$1" ] && echo "  [OK] موجود: $1" || echo "  [!!] مفقود: $1"; }
check_bin "sad"
check_bin "sadc"
check_bin "sad-lsp"
check_bin "sad-fmt"
check_bin "sad-pkg"

# ════════════════════════════════════════
# 1. AppImage — يعمل على أي توزيعة
# ════════════════════════════════════════
echo ""
echo "  [>>] بناء AppImage..."

# هيكل AppDir
APP_DIR="$OUTPUT_DIR/sad.AppDir"
rm -rf "$APP_DIR"
mkdir -p "$APP_DIR/usr/bin"
mkdir -p "$APP_DIR/usr/lib/sad/stdlib"
mkdir -p "$APP_DIR/usr/share/applications"
mkdir -p "$APP_DIR/usr/share/icons/hicolor/256x256/apps"
mkdir -p "$APP_DIR/usr/share/doc/sad-lang"

# نسخ الملفات التنفيذية
for bin in sad sadc sad-lsp sad-fmt sad-pkg sad-repl; do
    [ -f "$BIN_DIR/$bin" ] && cp "$BIN_DIR/$bin" "$APP_DIR/usr/bin/" && chmod +x "$APP_DIR/usr/bin/$bin"
done

# المكتبة القياسية
[ -d "$REPO_ROOT/stdlib" ] && cp -r "$REPO_ROOT/stdlib/"* "$APP_DIR/usr/lib/sad/stdlib/"

# ملف .desktop
cat > "$APP_DIR/usr/share/applications/sad-lang.desktop" << EOF
[Desktop Entry]
Type=Application
Name=لغة ص
Name[en]=Sad Programming Language
Comment=مفسّر لغة ص البرمجية
Comment[en]=Sad Programming Language Interpreter
Exec=sad %F
Icon=sad-lang
MimeType=text/x-sad;
Categories=Development;IDE;
Keywords=sad;ص;arabic;programming;
StartupNotify=false
Terminal=true
EOF

# ملف AppRun
cat > "$APP_DIR/AppRun" << 'EOF'
#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"
export SAD_HOME="$HERE/usr"
export SAD_STDLIB="$HERE/usr/lib/sad/stdlib"
export PATH="$HERE/usr/bin:$PATH"
exec "$HERE/usr/bin/sad" "$@"
EOF
chmod +x "$APP_DIR/AppRun"

# Desktop + icon رئيسية
cp "$APP_DIR/usr/share/applications/sad-lang.desktop" "$APP_DIR/sad-lang.desktop"
# أيقونة (نضع نصاً كبديل إن لم توجد PNG)
if [ -f "$REPO_ROOT/distribution/assets/sad_icon.png" ]; then
    cp "$REPO_ROOT/distribution/assets/sad_icon.png" "$APP_DIR/usr/share/icons/hicolor/256x256/apps/sad-lang.png"
    cp "$APP_DIR/usr/share/icons/hicolor/256x256/apps/sad-lang.png" "$APP_DIR/sad-lang.png"
fi

# بناء AppImage (يتطلب appimagetool)
if command -v appimagetool &>/dev/null; then
    ARCH="$ARCH" appimagetool "$APP_DIR" "$OUTPUT_DIR/sad-$VERSION-$ARCH.AppImage"
    echo "  [OK] تم: sad-$VERSION-$ARCH.AppImage"
    sha256sum "$OUTPUT_DIR/sad-$VERSION-$ARCH.AppImage" > "$OUTPUT_DIR/sad-$VERSION-$ARCH.AppImage.sha256"
else
    echo "  [!!] appimagetool غير مثبّت — تحميل من release.appimage.github.io"
    # تحميل appimagetool تلقائياً
    TOOL_URL="https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
    wget -q "$TOOL_URL" -O /tmp/appimagetool-x86_64.AppImage 2>/dev/null || \
    curl -sL "$TOOL_URL" -o /tmp/appimagetool-x86_64.AppImage
    chmod +x /tmp/appimagetool-x86_64.AppImage
    ARCH="$ARCH" /tmp/appimagetool-x86_64.AppImage "$APP_DIR" "$OUTPUT_DIR/sad-$VERSION-$ARCH.AppImage"
    echo "  [OK] تم: sad-$VERSION-$ARCH.AppImage"
fi

# ════════════════════════════════════════
# 2. حزمة .deb — Debian/Ubuntu
# ════════════════════════════════════════
echo ""
echo "  [>>] بناء حزمة .deb..."

DEB_PKG="$OUTPUT_DIR/deb/sad-lang_${VERSION}_${ARCH}"
rm -rf "$DEB_PKG"
mkdir -p "$DEB_PKG/DEBIAN"
mkdir -p "$DEB_PKG/usr/bin"
mkdir -p "$DEB_PKG/usr/lib/sad/stdlib"
mkdir -p "$DEB_PKG/usr/share/applications"
mkdir -p "$DEB_PKG/usr/share/doc/sad-lang"
mkdir -p "$DEB_PKG/usr/share/mime/packages"
mkdir -p "$DEB_PKG/usr/share/icons/hicolor/256x256/apps"
mkdir -p "$DEB_PKG/etc/sad-lang"

# ملف control
DEB_ARCH=$([ "$ARCH" = "x86_64" ] && echo "amd64" || echo "arm64")
cat > "$DEB_PKG/DEBIAN/control" << EOF
Package: sad-lang
Version: $VERSION
Architecture: $DEB_ARCH
Maintainer: فريق لغة ص <team@sad-lang.org>
Installed-Size: $(du -sk "$APP_DIR/usr" | cut -f1)
Depends: libc6 (>= 2.17), libstdc++6 (>= 8)
Recommends: code
Section: devel
Priority: optional
Homepage: https://sad-lang.org
Description: لغة ص البرمجية العربية
 Sad is a modern Arabic programming language with full RTL support,
 object-oriented programming, async/await, pattern matching, and
 LLVM native compilation.
 .
 يتضمن المفسر sad وأدوات التطوير الشاملة.
EOF

# ملف copyright
cat > "$DEB_PKG/usr/share/doc/sad-lang/copyright" << EOF
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: sad-lang
Upstream-Contact: team@sad-lang.org
Source: https://github.com/sad-lang/sad-language

Files: *
Copyright: 2025-2026 فريق لغة ص
License: MIT
EOF

# ملف postinst — بعد التثبيت
cat > "$DEB_PKG/DEBIAN/postinst" << 'EOF'
#!/bin/bash
set -e
update-mime-database /usr/share/mime 2>/dev/null || true
update-desktop-database /usr/share/applications 2>/dev/null || true
echo "تم تثبيت لغة ص بنجاح!"
echo "الاستخدام: sad <ملف.ص>  أو  sad --repl"
EOF
chmod 755 "$DEB_PKG/DEBIAN/postinst"

# ملف prerm — قبل الإزالة
cat > "$DEB_PKG/DEBIAN/prerm" << 'EOF'
#!/bin/bash
set -e
echo "جاري إزالة لغة ص..."
EOF
chmod 755 "$DEB_PKG/DEBIAN/prerm"

# نسخ الملفات
for bin in sad sadc sad-lsp sad-fmt sad-pkg sad-repl; do
    [ -f "$BIN_DIR/$bin" ] && cp "$BIN_DIR/$bin" "$DEB_PKG/usr/bin/" && chmod +x "$DEB_PKG/usr/bin/$bin"
done
[ -d "$REPO_ROOT/stdlib" ] && cp -r "$REPO_ROOT/stdlib/"* "$DEB_PKG/usr/lib/sad/stdlib/"
cp "$APP_DIR/usr/share/applications/sad-lang.desktop" "$DEB_PKG/usr/share/applications/"
[ -f "$APP_DIR/usr/share/icons/hicolor/256x256/apps/sad-lang.png" ] && \
    cp "$APP_DIR/usr/share/icons/hicolor/256x256/apps/sad-lang.png" \
       "$DEB_PKG/usr/share/icons/hicolor/256x256/apps/"

# MIME type لامتداد .ص
cat > "$DEB_PKG/usr/share/mime/packages/sad-lang.xml" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
  <mime-type type="text/x-sad">
    <comment>ملف لغة ص</comment>
    <comment xml:lang="en">Sad source file</comment>
    <glob pattern="*.ص"/>
    <glob pattern="*.sad"/>
    <magic priority="50">
      <match type="string" offset="0" value="# بسم"/>
      <match type="string" offset="0" value="دالة"/>
    </magic>
  </mime-type>
</mime-info>
EOF

# ملف إعداد
cat > "$DEB_PKG/etc/sad-lang/sad.config.json" << EOF
{
    "version": "$VERSION",
    "stdlib_path": "/usr/lib/sad/stdlib",
    "encoding": "utf-8",
    "rtl": true
}
EOF

# بناء .deb
dpkg-deb --build --root-owner-group "$DEB_PKG" \
    "$OUTPUT_DIR/sad-lang_${VERSION}_${DEB_ARCH}.deb" 2>/dev/null || \
    fakeroot dpkg-deb --build "$DEB_PKG" \
    "$OUTPUT_DIR/sad-lang_${VERSION}_${DEB_ARCH}.deb"
echo "  [OK] تم: sad-lang_${VERSION}_${DEB_ARCH}.deb"
sha256sum "$OUTPUT_DIR/sad-lang_${VERSION}_${DEB_ARCH}.deb" > \
    "$OUTPUT_DIR/sad-lang_${VERSION}_${DEB_ARCH}.deb.sha256"

# ════════════════════════════════════════
# 3. حزمة .rpm — Fedora/RHEL/SUSE
# ════════════════════════════════════════
echo ""
echo "  [>>] بناء حزمة .rpm..."

if command -v rpmbuild &>/dev/null; then
    RPM_ROOT="$OUTPUT_DIR/rpm/BUILD"
    mkdir -p "$RPM_ROOT"
    mkdir -p "$OUTPUT_DIR/rpm/SPECS"
    mkdir -p "$OUTPUT_DIR/rpm/SOURCES"
    mkdir -p "$OUTPUT_DIR/rpm/RPMS"
    mkdir -p "$OUTPUT_DIR/rpm/SRPMS"
    mkdir -p "$OUTPUT_DIR/rpm/BUILDROOT"

    # حزمة tar.gz للمصدر
    TAR_DIR="$RPM_ROOT/sad-lang-$VERSION"
    mkdir -p "$TAR_DIR/bin"
    mkdir -p "$TAR_DIR/stdlib"
    for bin in sad sadc sad-lsp sad-fmt sad-pkg sad-repl; do
        [ -f "$BIN_DIR/$bin" ] && cp "$BIN_DIR/$bin" "$TAR_DIR/bin/"
    done
    [ -d "$REPO_ROOT/stdlib" ] && cp -r "$REPO_ROOT/stdlib/"* "$TAR_DIR/stdlib/"
    tar czf "$OUTPUT_DIR/rpm/SOURCES/sad-lang-$VERSION.tar.gz" -C "$RPM_ROOT" "sad-lang-$VERSION"

    RPM_ARCH=$([ "$ARCH" = "x86_64" ] && echo "x86_64" || echo "aarch64")
    cat > "$OUTPUT_DIR/rpm/SPECS/sad-lang.spec" << EOF
Name:       sad-lang
Version:    $VERSION
Release:    1%{?dist}
Summary:    لغة ص — لغة برمجة عربية حديثة
License:    MIT
URL:        https://sad-lang.org
Source0:    %{name}-%{version}.tar.gz
BuildArch:  $RPM_ARCH
Requires:   glibc >= 2.17
Requires:   libstdc++ >= 8

%description
لغة ص هي لغة برمجة عربية حديثة تدعم:
- البرمجة الكائنية الكاملة
- البرمجة المتزامنة (async/await)
- مطابقة الأنماط
- التحويل إلى كود أصلي (LLVM)

%prep
%setup -q

%install
mkdir -p %{buildroot}/usr/bin
mkdir -p %{buildroot}/usr/lib/sad/stdlib
for bin in bin/*; do
    install -m 755 \$bin %{buildroot}/usr/bin/
done
cp -r stdlib/* %{buildroot}/usr/lib/sad/stdlib/

%post
echo "تم تثبيت لغة ص $VERSION بنجاح"
echo "الاستخدام: sad <ملف.ص>  أو  sad --repl"

%files
/usr/bin/sad
%{?_with_compiler:/usr/bin/sadc}
/usr/lib/sad/stdlib/

%changelog
* $(date '+%a %b %d %Y') فريق لغة ص <team@sad-lang.org> - $VERSION-1
- الإصدار الرسمي الأول
EOF

    rpmbuild -bb \
        --define "_topdir $OUTPUT_DIR/rpm" \
        --define "_rpmdir $OUTPUT_DIR/rpm/RPMS" \
        "$OUTPUT_DIR/rpm/SPECS/sad-lang.spec"
    echo "  [OK] تم بناء حزمة .rpm"
    sha256sum "$OUTPUT_DIR/rpm/RPMS/$RPM_ARCH/sad-lang-$VERSION"*.rpm | head -1 \
        > "$OUTPUT_DIR/rpm/RPMS/$RPM_ARCH/sad-lang-$VERSION.rpm.sha256" 2>/dev/null || true
else
    echo "  [!!] rpmbuild غير مثبّت — تخطي بناء .rpm"
    echo "  تثبيت: sudo dnf install rpm-build  أو  sudo apt install rpm"
fi

echo ""
echo "══════════════════════════════════════════════════════"
echo "  [OK] انتهى بناء حزم Linux"
echo "  المجلد: $OUTPUT_DIR"
ls -lh "$OUTPUT_DIR"/*.AppImage "$OUTPUT_DIR"/*.deb 2>/dev/null | awk '{print "    "$NF, $5}'
echo "══════════════════════════════════════════════════════"
'@
    $bashContent | Set-Content $buildScript -Encoding UTF8 -NoNewline
    # تحويل CRLF إلى LF
    $bytes = [System.IO.File]::ReadAllBytes($buildScript)
    $cr = [byte]0x0D
    $bytes = $bytes | Where-Object { $_ -ne $cr }
    [System.IO.File]::WriteAllBytes($buildScript, $bytes)

    Write-OK "تم إنشاء: linux\build-linux.sh"
    Write-Host "  └─────────────────────────────────────────────────────┘" -ForegroundColor Green
}

# ══════════════════════════════════════════════════════════════════════
#  3. macOS Packages (.pkg + .dmg)
# ══════════════════════════════════════════════════════════════════════
function Build-macOSPackages {
    Write-Host ""
    Write-Host "  ┌── macOS Packages (.pkg + .dmg) ─────────────────────┐" -ForegroundColor Yellow

    $macOutputDir = Join-Path $OutputDir "macos"
    New-Item -ItemType Directory -Path $macOutputDir -Force | Out-Null

    $macScript = Join-Path $ScriptDir "macos\build-macos.sh"
    New-Item -ItemType Directory -Path (Split-Path $macScript) -Force | Out-Null

    $macContent = @'
#!/usr/bin/env bash
# ═══════════════════════════════════════════════
# بناء حزم macOS — .pkg + .dmg
# ═══════════════════════════════════════════════
set -euo pipefail

VERSION="${1:-1.0.0}"
ARCH="${2:-$(uname -m)}"   # x86_64 أو arm64
BIN_DIR="${3:-../../build/bin/Release}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
OUTPUT_DIR="$SCRIPT_DIR/../output/macos"
mkdir -p "$OUTPUT_DIR"

echo "══════════════════════════════════════════"
echo "  لغة ص — بناء حزم macOS"
echo "  الإصدار: $VERSION  البنية: $ARCH"
echo "══════════════════════════════════════════"

# ─── هيكل التثبيت ───
PKG_ROOT="$OUTPUT_DIR/pkg_root"
SCRIPTS_DIR="$OUTPUT_DIR/pkg_scripts"
PKG_BUILD="$OUTPUT_DIR/pkg_build"
DMG_APP="$OUTPUT_DIR/Sad.app"

rm -rf "$PKG_ROOT" "$SCRIPTS_DIR" "$PKG_BUILD" "$DMG_APP"
mkdir -p "$PKG_ROOT/usr/local/bin"
mkdir -p "$PKG_ROOT/usr/local/lib/sad/stdlib"
mkdir -p "$PKG_ROOT/usr/local/share/man/man1"
mkdir -p "$PKG_ROOT/usr/local/share/doc/sad-lang"
mkdir -p "$SCRIPTS_DIR"

# ─── نسخ الملفات ───
for bin in sad sadc sad-lsp sad-fmt sad-pkg sad-repl; do
    [ -f "$BIN_DIR/$bin" ] && cp "$BIN_DIR/$bin" "$PKG_ROOT/usr/local/bin/" && \
        chmod +x "$PKG_ROOT/usr/local/bin/$bin"
done

# تضمين معماريتين (Universal Binary) إذا توفرتا
if [ -f "$BIN_DIR/../Release-arm64/sad" ] && [ -f "$BIN_DIR/sad" ]; then
    echo "  [>>] إنشاء Universal Binary (x86_64 + arm64)..."
    lipo -create "$BIN_DIR/sad" "$BIN_DIR/../Release-arm64/sad" \
        -output "$PKG_ROOT/usr/local/bin/sad"
    echo "  [OK] Universal Binary تم"
fi

# المكتبة القياسية
[ -d "$REPO_ROOT/stdlib" ] && cp -r "$REPO_ROOT/stdlib/"* "$PKG_ROOT/usr/local/lib/sad/stdlib/"

# man page
cat > "$PKG_ROOT/usr/local/share/man/man1/sad.1" << 'EOF'
.TH SAD 1 "2026" "1.0.0" "لغة ص"
.SH الاسم
sad \- مُفسّر لغة ص البرمجية العربية
.SH الاستخدام
\fBsad\fR [\fIخيارات\fR] [\fIملف.ص\fR]
.SH الخيارات
.TP
\fB--repl\fR
تشغيل بيئة REPL التفاعلية
.TP
\fB--version\fR
عرض الإصدار
.SH مثال
sad برنامج.ص
EOF

# ─── سكريبتات التثبيت ───
cat > "$SCRIPTS_DIR/postinstall" << 'EOF'
#!/bin/bash
echo "تم تثبيت لغة ص بنجاح!"
echo "أضف /usr/local/bin إلى PATH إذا لم يكن موجوداً:"
echo "  echo 'export PATH=/usr/local/bin:\$PATH' >> ~/.zshrc"
echo ""
echo "تشغيل: sad <ملف.ص>  أو  sad --repl"
EOF
chmod +x "$SCRIPTS_DIR/postinstall"

# ─── بناء حزمة .pkg ───
echo ""
echo "  [>>] بناء حزمة .pkg..."

PKG_FILE="$OUTPUT_DIR/sad-lang-$VERSION-$ARCH.pkg"

# component pkg
pkgbuild --root "$PKG_ROOT" \
         --scripts "$SCRIPTS_DIR" \
         --identifier org.sad-lang.sad \
         --version "$VERSION" \
         --install-location "/" \
         "$PKG_BUILD/sad-lang-component.pkg"

# product archive (مع صفحة ترحيب وترخيص)
cat > "$PKG_BUILD/Distribution.xml" << EOF
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="1">
    <title>لغة ص — Sad Programming Language</title>
    <background file="background.png" scaling="proportional" alignment="bottomleft"/>
    <welcome file="welcome.html" mime-type="text/html"/>
    <license file="LICENSE" mime-type="text/plain"/>
    <conclusion file="conclusion.html" mime-type="text/html"/>
    <options customize="allow" require-scripts="false" rootVolumeOnly="false"/>
    <volume-check>
        <allowed-os-versions>
            <os-version min="10.15"/>
        </allowed-os-versions>
    </volume-check>
    <choices-outline>
        <line choice="default">
            <line choice="sad-lang"/>
        </line>
    </choices-outline>
    <choice id="default"/>
    <choice id="sad-lang" visible="false">
        <pkg-ref id="org.sad-lang.sad"/>
    </choice>
    <pkg-ref id="org.sad-lang.sad" version="$VERSION" onConclusion="none">sad-lang-component.pkg</pkg-ref>
    <pkg-ref id="org.sad-lang.sad">
        <bundle-version/>
    </pkg-ref>
</installer-gui-script>
EOF

# ملفات الترحيب
cat > "$PKG_BUILD/welcome.html" << EOF
<html dir="rtl"><body>
<h2>مرحباً بك في مُثبّت لغة ص</h2>
<p>لغة ص هي لغة برمجة عربية حديثة.</p>
<p>الإصدار: $VERSION</p>
</body></html>
EOF

cat > "$PKG_BUILD/conclusion.html" << EOF
<html dir="rtl"><body>
<h2>اكتمل تثبيت لغة ص!</h2>
<p>افتح Terminal وشغّل: <code>sad --repl</code></p>
</body></html>
EOF

# نسخ الترخيص
[ -f "$REPO_ROOT/LICENSE" ] && cp "$REPO_ROOT/LICENSE" "$PKG_BUILD/"

productbuild --distribution "$PKG_BUILD/Distribution.xml" \
             --package-path "$PKG_BUILD" \
             --resources "$PKG_BUILD" \
             "$PKG_FILE" 2>/dev/null || \
pkgbuild --root "$PKG_ROOT" \
         --scripts "$SCRIPTS_DIR" \
         --identifier org.sad-lang.sad \
         --version "$VERSION" \
         --install-location "/" \
         "$PKG_FILE"

echo "  [OK] تم: sad-lang-$VERSION-$ARCH.pkg"
shasum -a 256 "$PKG_FILE" > "$PKG_FILE.sha256"

# ─── بناء .dmg ───
echo ""
echo "  [>>] بناء ملف .dmg..."

DMG_DIR="$OUTPUT_DIR/dmg_content"
rm -rf "$DMG_DIR"
mkdir -p "$DMG_DIR"

# نسخ المحتوى
cp -r "$PKG_ROOT/usr/local/bin" "$DMG_DIR/bin"
cp "$PKG_FILE" "$DMG_DIR/"
[ -f "$REPO_ROOT/README.md" ] && cp "$REPO_ROOT/README.md" "$DMG_DIR/README.md"

# نسخ أمثلة
[ -d "$REPO_ROOT/examples" ] && cp -r "$REPO_ROOT/examples" "$DMG_DIR/أمثلة"

DMG_FILE="$OUTPUT_DIR/sad-lang-$VERSION-$ARCH.dmg"

if command -v create-dmg &>/dev/null; then
    create-dmg \
        --volname "لغة ص $VERSION" \
        --volicon "$REPO_ROOT/distribution/assets/sad_icon.icns" \
        --window-pos 200 120 \
        --window-size 600 400 \
        --icon-size 100 \
        --icon "sad-lang-$VERSION-$ARCH.pkg" 150 185 \
        --hide-extension "sad-lang-$VERSION-$ARCH.pkg" \
        --app-drop-link 425 185 \
        "$DMG_FILE" "$DMG_DIR" || \
    hdiutil create -volname "لغة ص $VERSION" \
        -srcfolder "$DMG_DIR" \
        -ov -format UDZO "$DMG_FILE"
else
    hdiutil create -volname "لغة ص $VERSION" \
        -srcfolder "$DMG_DIR" \
        -ov -format UDZO "$DMG_FILE"
fi

echo "  [OK] تم: sad-lang-$VERSION-$ARCH.dmg"
shasum -a 256 "$DMG_FILE" > "$DMG_FILE.sha256"

echo ""
echo "══════════════════════════════════════════════════════"
echo "  [OK] انتهى بناء حزم macOS"
ls -lh "$OUTPUT_DIR"/*.pkg "$OUTPUT_DIR"/*.dmg 2>/dev/null
echo "══════════════════════════════════════════════════════"
'@
    $macContent | Set-Content $macScript -Encoding UTF8 -NoNewline
    $bytes = [System.IO.File]::ReadAllBytes($macScript)
    $cr = [byte]0x0D
    $bytes = $bytes | Where-Object { $_ -ne $cr }
    [System.IO.File]::WriteAllBytes($macScript, $bytes)

    Write-OK "تم إنشاء: macos\build-macos.sh"
    Write-Host "  └─────────────────────────────────────────────────────┘" -ForegroundColor Yellow
}

# ══════════════════════════════════════════════════════════════════════
#  4. Android APK
# ══════════════════════════════════════════════════════════════════════
function Build-AndroidAPK {
    Write-Host ""
    Write-Host "  ┌── Android APK ───────────────────────────────────────┐" -ForegroundColor Magenta

    $AndroidDir  = Join-Path $RepoRoot "tools\android"
    $BuildScript = Join-Path $AndroidDir "build_apk.py"

    if (-not (Test-Path $BuildScript)) {
        Write-Error2 "  سكريبت بناء APK غير موجود: $BuildScript"
        Write-Host "  └─────────────────────────────────────────────────────┘" -ForegroundColor Magenta
        return
    }

    # إنشاء مشروع تطبيق مُثبّت لغة ص
    $InstallerApp = Join-Path $ScriptDir "android\sad-installer-app"
    New-Item -ItemType Directory -Path $InstallerApp -Force | Out-Null

    # ملف المشروع الرئيسي لتطبيق المُثبّت
    $MainSadFile = @'
# تطبيق مُثبّت لغة ص لأندرويد
# هذا الكود يعمل داخل تطبيق الأندرويد

متغير الإصدار = "1.0.0"
متغير اسم_التطبيق = "لغة ص"

دالة رئيسية()
    اطبع_سطر("╔═══════════════════════════════════╗")
    اطبع_سطر("║     مُثبّت لغة ص v" + الإصدار + "         ║")
    اطبع_سطر("╚═══════════════════════════════════╝")
    اطبع_سطر("")
    اطبع_سطر("مرحباً بك في لغة ص!")
    اطبع_سطر("لغة برمجة عربية حديثة لأجهزة أندرويد")
    اطبع_سطر("")
    اطبع_سطر("الميزات المتاحة:")
    اطبع_سطر("  ● مفسّر لغة ص الكامل")
    اطبع_سطر("  ● محرر كود مدمج")
    اطبع_سطر("  ● بيئة REPL تفاعلية")
    اطبع_سطر("  ● المكتبة القياسية الكاملة")
    اطبع_سطر("")
    اطبع_سطر("جاري التهيئة...")
نهاية
'@
    $MainSadFile | Set-Content (Join-Path $InstallerApp "main.ص") -Encoding UTF8

    # تكوين مشروع أندرويد
    $ProjectConfig = @{
        app_name       = "لغة ص"
        app_name_en    = "Sad Language"
        package_name   = "org.sadlang.interpreter"
        version_code   = 1
        version_name   = $Version
        min_sdk        = 21
        target_sdk     = 34
        main_file      = "main.ص"
        permissions    = @("INTERNET", "WRITE_EXTERNAL_STORAGE", "READ_EXTERNAL_STORAGE")
        features       = @("interpreter", "repl", "editor", "stdlib")
    }
    $ProjectConfig | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $InstallerApp "project.json") -Encoding UTF8

    # سكريبت بناء APK
    $BuildCmd = @"
# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════
# بناء APK لغة ص — Sad Language Android Installer
# ═══════════════════════════════════════════════════════
"""
بناء تطبيق مُثبّت لغة ص كحزمة APK لأجهزة أندرويد.

الاستخدام:
    python build-android.py
    python build-android.py --release
    python build-android.py --arch arm64-v8a
    python build-android.py --arch "arm64-v8a,x86_64"
"""

import os, sys, json, shutil, subprocess, argparse
from pathlib import Path

SCRIPT_DIR  = Path(__file__).parent
REPO_ROOT   = SCRIPT_DIR.parent.parent
ANDROID_DIR = REPO_ROOT / "tools" / "android"
OUTPUT_DIR  = SCRIPT_DIR / ".." / "output" / "android"
OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

APP_DIR     = SCRIPT_DIR / "sad-installer-app"
BUILD_APK   = ANDROID_DIR / "build_apk.py"

def banner():
    print("""
╔═══════════════════════════════════════════════════╗
║     لغة ص — بناء APK للأندرويد                  ║
║     Sad Language Android APK Builder             ║
╚═══════════════════════════════════════════════════╝""")

def build_apk(args):
    banner()
    print(f"  [>>] بناء APK لغة ص v{args.version}...")
    print(f"  [>>] البنية: {args.arch}")

    cmd = [
        sys.executable, str(BUILD_APK),
        "--project", str(APP_DIR),
        "--output", str(OUTPUT_DIR),
        "--arch",   args.arch,
    ]
    if args.release:
        cmd.append("--release")

    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"  [XX] فشل بناء APK:")
        print(result.stderr)
        return False

    # البحث عن الـ APK
    apk_files = list(OUTPUT_DIR.glob("**/*.apk"))
    if apk_files:
        for apk in apk_files:
            size_mb = apk.stat().st_size / (1024*1024)
            print(f"  [OK] تم: {apk.name} ({size_mb:.1f} MB)")
            # SHA256
            import hashlib
            sha = hashlib.sha256(apk.read_bytes()).hexdigest()
            (apk.parent / f"{apk.name}.sha256").write_text(f"{sha}  {apk.name}")
            print(f"  [OK] SHA256: {sha[:16]}...")
    else:
        # إنشاء APK نموذجي (placeholder)
        placeholder = OUTPUT_DIR / f"sad-lang-{args.version}-android.apk.note"
        placeholder.write_text(
            f"APK سيتم بناؤه آلياً عبر GitHub Actions عند توفر Android NDK/SDK\n"
            f"الإصدار: {args.version}\n"
            f"البنية: {args.arch}\n", encoding='utf-8')
        print("  [!!] APK placeholder أُنشئ — يتطلب Android SDK/NDK للبناء الكامل")

    print(f"\n  [OK] مجلد الخرج: {OUTPUT_DIR}")
    return True

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="بناء APK لغة ص")
    parser.add_argument("--version", default="1.0.0")
    parser.add_argument("--arch", default="arm64-v8a,x86_64,armeabi-v7a")
    parser.add_argument("--release", action="store_true")
    build_apk(parser.parse_args())
"@
    $BuildCmd | Set-Content (Join-Path $ScriptDir "android\build-android.py") -Encoding UTF8

    Write-OK "تم إنشاء: android\build-android.py"
    Write-OK "تم إنشاء: android\sad-installer-app\"
    Write-Host "  └─────────────────────────────────────────────────────┘" -ForegroundColor Magenta
}

# ══════════════════════════════════════════════════════════════════════
#  5. GitHub Actions Workflow — بناء آلي لكل المنصات
# ══════════════════════════════════════════════════════════════════════
function Build-GitHubActionsWorkflow {
    Write-Host ""
    Write-Host "  ┌── GitHub Actions Workflow ───────────────────────────┐" -ForegroundColor DarkCyan

    $WorkflowDir = Join-Path $RepoRoot ".github\workflows"
    New-Item -ItemType Directory -Path $WorkflowDir -Force | Out-Null

    $WorkflowFile = Join-Path $WorkflowDir "build-installers.yml"

    $workflowContent = @'
# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════
# GitHub Actions — بناء مُثبّتات لغة ص لجميع المنصات
# يُشغَّل تلقائياً عند: إنشاء tag جديد v*.*.* أو push لـ main
# ═══════════════════════════════════════════════════════════════════

name: "Build Distribution Installers"

on:
  push:
    tags:
      - "v*.*.*"
  workflow_dispatch:
    inputs:
      version:
        description: "رقم الإصدار (مثال: 1.0.0)"
        required: true
        default: "1.0.0"
      release:
        description: "هل هذا إصدار رسمي؟"
        required: true
        type: boolean
        default: false

env:
  VERSION: ${{ github.ref_type == 'tag' && github.ref_name || format('v{0}', inputs.version) }}
  BUILD_TYPE: Release

jobs:
  # ─────────────────────────────────────────────────────────
  # Windows x64 — .exe Installer (InnoSetup)
  # ─────────────────────────────────────────────────────────
  windows-installer:
    name: "Windows Installer (.exe)"
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v4
        with: { submodules: recursive }

      - name: "إعداد MSVC"
        uses: microsoft/setup-msbuild@v2

      - name: "إعداد CMake"
        uses: lukka/get-cmake@latest

      - name: "بناء sad.exe"
        run: |
          cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
          cmake --build build --config Release --target sad
          cmake --build build --config Release --target sad-lsp  2>$null || true
          cmake --build build --config Release --target sad-pkg  2>$null || true
          cmake --build build --config Release --target sad-fmt  2>$null || true
          cmake --build build --config Release --target sad-repl 2>$null || true

      - name: "تثبيت InnoSetup"
        run: |
          choco install innosetup --yes --no-progress
          echo "C:\Program Files (x86)\Inno Setup 6" >> $env:GITHUB_PATH

      - name: "تحميل ملف Arabic.isl"
        run: |
          $url = "https://raw.githubusercontent.com/jrsoftware/issrc/main/Files/Languages/Unofficial/Arabic.isl"
          $dest = "C:\Program Files (x86)\Inno Setup 6\Languages\Arabic.isl"
          Invoke-WebRequest -Uri $url -OutFile $dest -UseBasicParsing

      - name: "بناء Windows Installer (.exe)"
        run: |
          $ver = "$env:VERSION".TrimStart('v')
          New-Item -ItemType Directory -Path distribution\output -Force
          ISCC.exe `
            "/DMyVersion=$ver" `
            "/DMyOutputDir=$(Resolve-Path distribution\output)" `
            "/DMyBinDir=$(Resolve-Path build\bin\Release)" `
            "/DMyRepoRoot=$(Resolve-Path .)" `
            distribution\installer\sad-setup.iss

      - name: "رفع النتيجة"
        uses: actions/upload-artifact@v4
        with:
          name: sad-windows-installer
          path: distribution/output/sad-setup-*.exe
          retention-days: 30

  # ─────────────────────────────────────────────────────────
  # Linux x64 — AppImage + deb + rpm
  # ─────────────────────────────────────────────────────────
  linux-packages:
    name: "Linux Packages (AppImage + deb + rpm)"
    runs-on: ubuntu-22.04
    steps:
      - uses: actions/checkout@v4
        with: { submodules: recursive }

      - name: "تثبيت التبعيات"
        run: |
          sudo apt-get update -qq
          sudo apt-get install -y cmake ninja-build g++ \
            dpkg-dev fakeroot rpm \
            libfuse2 wget

      - name: "بناء sad"
        run: |
          cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
          cmake --build build --config Release --target sad
          cmake --build build --config Release --target sad-lsp  || true
          cmake --build build --config Release --target sad-pkg  || true
          cmake --build build --config Release --target sad-fmt  || true
          cmake --build build --config Release --target sad-repl || true

      - name: "بناء الحزم"
        run: |
          chmod +x distribution/linux/build-linux.sh
          VER="${VERSION#v}"
          bash distribution/linux/build-linux.sh "$VER" x86_64 build/bin/Release

      - name: "رفع النتائج"
        uses: actions/upload-artifact@v4
        with:
          name: sad-linux-packages
          path: |
            distribution/output/linux/*.AppImage
            distribution/output/linux/*.deb
            distribution/output/linux/**/*.rpm
          retention-days: 30

  # ─────────────────────────────────────────────────────────
  # Linux ARM64
  # ─────────────────────────────────────────────────────────
  linux-arm64:
    name: "Linux ARM64"
    runs-on: ubuntu-22.04
    steps:
      - uses: actions/checkout@v4
        with: { submodules: recursive }

      - uses: docker/setup-qemu-action@v3

      - name: "بناء لـ ARM64"
        uses: addnab/docker-run-action@v3
        with:
          image: arm64v8/ubuntu:22.04
          options: -v ${{ github.workspace }}:/repo
          run: |
            apt-get update -qq && apt-get install -y cmake g++ ninja-build dpkg-dev fakeroot wget
            cd /repo
            cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
            cmake --build build --config Release --target sad || true
            chmod +x distribution/linux/build-linux.sh
            bash distribution/linux/build-linux.sh "$(echo $VERSION | sed 's/v//')" aarch64 build/bin/Release || true

      - uses: actions/upload-artifact@v4
        with:
          name: sad-linux-arm64
          path: distribution/output/linux/*aarch64*
          retention-days: 30

  # ─────────────────────────────────────────────────────────
  # macOS — .pkg + .dmg
  # ─────────────────────────────────────────────────────────
  macos-packages:
    name: "macOS Packages (.pkg + .dmg)"
    strategy:
      matrix:
        include:
          - os: macos-13     # Intel x86_64
            arch: x86_64
          - os: macos-14     # Apple Silicon arm64
            arch: arm64
    runs-on: ${{ matrix.os }}
    steps:
      - uses: actions/checkout@v4
        with: { submodules: recursive }

      - name: "بناء sad"
        run: |
          cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_OSX_ARCHITECTURES=${{ matrix.arch }}
          cmake --build build --config Release --target sad
          cmake --build build --config Release --target sad-lsp  || true
          cmake --build build --config Release --target sad-pkg  || true
          cmake --build build --config Release --target sad-fmt  || true

      - name: "تثبيت create-dmg"
        run: brew install create-dmg || true

      - name: "بناء .pkg و .dmg"
        run: |
          chmod +x distribution/macos/build-macos.sh
          VER="${VERSION#v}"
          bash distribution/macos/build-macos.sh "$VER" "${{ matrix.arch }}" build/bin/Release

      - uses: actions/upload-artifact@v4
        with:
          name: sad-macos-${{ matrix.arch }}
          path: |
            distribution/output/macos/*.pkg
            distribution/output/macos/*.dmg
          retention-days: 30

  # ─────────────────────────────────────────────────────────
  # Android APK
  # ─────────────────────────────────────────────────────────
  android-apk:
    name: "Android APK"
    runs-on: ubuntu-22.04
    steps:
      - uses: actions/checkout@v4
        with: { submodules: recursive }

      - uses: actions/setup-java@v4
        with:
          java-version: "17"
          distribution: "temurin"

      - name: "إعداد Android NDK"
        uses: android-actions/setup-android@v3

      - name: "بناء APK"
        run: |
          pip install -q pathlib
          VER="${VERSION#v}"
          python distribution/android/build-android.py \
            --version "$VER" \
            --arch "arm64-v8a,x86_64,armeabi-v7a" \
            --release

      - uses: actions/upload-artifact@v4
        with:
          name: sad-android-apk
          path: distribution/output/android/*.apk
          retention-days: 30

  # ─────────────────────────────────────────────────────────
  # إنشاء GitHub Release
  # ─────────────────────────────────────────────────────────
  create-release:
    name: "إنشاء GitHub Release"
    runs-on: ubuntu-latest
    needs:
      - windows-installer
      - linux-packages
      - linux-arm64
      - macos-packages
      - android-apk
    if: github.ref_type == 'tag'
    steps:
      - uses: actions/checkout@v4

      - name: "تحميل جميع الملفات"
        uses: actions/download-artifact@v4
        with:
          path: release-assets
          merge-multiple: true

      - name: "حساب SHA256 لجميع الملفات"
        run: |
          cd release-assets
          find . -name "*.exe" -o -name "*.AppImage" -o -name "*.deb" \
              -o -name "*.rpm" -o -name "*.pkg" -o -name "*.dmg" \
              -o -name "*.apk" | xargs -I{} sha256sum {} >> SHA256SUMS.txt
          cat SHA256SUMS.txt

      - name: "إنشاء Release"
        uses: softprops/action-gh-release@v2
        with:
          name: "لغة ص ${{ env.VERSION }}"
          body: |
            ## 🎉 لغة ص ${{ env.VERSION }}

            ### تثبيت سريع

            **Windows:**
            ```powershell
            irm https://sad-lang.org/install.ps1 | iex
            ```

            **Linux/macOS:**
            ```bash
            curl -fsSL https://sad-lang.org/install.sh | sh
            ```

            ### الحزم المتاحة

            | المنصة | النوع | الملف |
            |--------|-------|-------|
            | Windows x64 | مُثبّت GUI (.exe) | `sad-setup-${{ env.VERSION }}-windows-x64.exe` |
            | Linux x64 | AppImage | `sad-${{ env.VERSION }}-x86_64.AppImage` |
            | Linux x64 | حزمة deb | `sad-lang_${{ env.VERSION }}_amd64.deb` |
            | Linux x64 | حزمة rpm | `sad-lang-${{ env.VERSION }}-1.x86_64.rpm` |
            | Linux ARM64 | AppImage | `sad-${{ env.VERSION }}-aarch64.AppImage` |
            | macOS Intel | pkg + dmg | `sad-lang-${{ env.VERSION }}-x86_64.pkg` |
            | macOS Apple Silicon | pkg + dmg | `sad-lang-${{ env.VERSION }}-arm64.pkg` |
            | Android | APK | `sad-lang-${{ env.VERSION }}-android.apk` |

            ### التحقق من التوقيعات
            ```bash
            sha256sum -c SHA256SUMS.txt
            ```
          files: |
            release-assets/**/*.exe
            release-assets/**/*.AppImage
            release-assets/**/*.deb
            release-assets/**/*.rpm
            release-assets/**/*.pkg
            release-assets/**/*.dmg
            release-assets/**/*.apk
            release-assets/SHA256SUMS.txt
          draft: ${{ github.event.inputs.release != 'true' }}
          prerelease: false
          generate_release_notes: true
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
'@
    $workflowContent | Set-Content $WorkflowFile -Encoding UTF8 -NoNewline
    # تحويل CRLF إلى LF
    $bytes = [System.IO.File]::ReadAllBytes($WorkflowFile)
    $cr = [byte]0x0D
    $bytes = $bytes | Where-Object { $_ -ne $cr }
    [System.IO.File]::WriteAllBytes($WorkflowFile, $bytes)

    Write-OK "تم إنشاء: .github\workflows\build-installers.yml"
    Write-Host "  └─────────────────────────────────────────────────────┘" -ForegroundColor DarkCyan
}

# ══════════════════════════════════════════════════════════════════════
#  MAIN
# ══════════════════════════════════════════════════════════════════════
Write-Banner
Prepare-Output

switch ($Platform) {
    "windows" { Build-WindowsInstaller }
    "linux"   { Build-LinuxPackages }
    "macos"   { Build-macOSPackages }
    "android" { Build-AndroidAPK }
    "all"     {
        Build-WindowsInstaller
        Build-LinuxPackages
        Build-macOSPackages
        Build-AndroidAPK
        Build-GitHubActionsWorkflow
    }
}

# دائماً أنشئ GitHub Actions
Build-GitHubActionsWorkflow

Write-Host ""
Write-Host "  ╔═══════════════════════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "  ║  انتهى بناء أدوات التوزيع بنجاح!                   ║" -ForegroundColor Cyan
Write-Host "  ║  Distribution builder completed successfully!        ║" -ForegroundColor Cyan
Write-Host "  ╚═══════════════════════════════════════════════════════╝" -ForegroundColor Cyan
Write-Host ""
Write-Host "  الخطوة التالية:" -ForegroundColor Yellow
Write-Host "  1. تثبيت InnoSetup 6 من: jrsoftware.org/isdl.php"
Write-Host "  2. تنزيل Arabic.isl ووضعه في مجلد Languages/"
Write-Host "  3. تشغيل سكريبت البناء:"
Write-Host "     .\distribution\build-installers.ps1 -Platform windows -Release" -ForegroundColor Green
Write-Host ""
